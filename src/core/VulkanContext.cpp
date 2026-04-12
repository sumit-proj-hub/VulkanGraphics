#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <core/VulkanContext.hpp>
#include <core/Window.hpp>
#include <functional>
#include <iostream>
#include <render/Renderer.hpp>
#include <stdexcept>

void VulkanContext::createInstance(std::string_view applicationName, uint32_t version) {
  vk::ApplicationInfo appInfo{.pApplicationName = applicationName.data(),
                              .applicationVersion = version,
                              .pEngineName = "No Engine",
                              .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                              .apiVersion = vk::ApiVersion14};

  std::vector<const char *> requiredLayers;
  if (enableValidationLayers) {
    requiredLayers.assign(validationLayers.begin(), validationLayers.end());
  }

  auto layerProperties = context.enumerateInstanceLayerProperties();
  if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const &requiredLayer) {
        return std::ranges::none_of(layerProperties, [requiredLayer](auto const &layerProperty) {
          return strcmp(layerProperty.layerName, requiredLayer) == 0;
        });
      })) {
    throw std::runtime_error("One or more required layers are not supported");
  }

  auto requiredExtensions = Window::getRequiredInstanceExtensions();
  if (enableValidationLayers) {
    requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);
  }

  auto extensionProperties = context.enumerateInstanceExtensionProperties();
  for (auto const &requiredExtension : requiredExtensions) {
    if (std::ranges::none_of(extensionProperties, [requiredExtension](auto const &extensionProperty) {
          return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
        })) {
      throw std::runtime_error("Required GLFW extension not supported: " + std::string(requiredExtension));
    }
  }

  vk::InstanceCreateInfo createInfo{.pApplicationInfo = &appInfo,
                                    .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
                                    .ppEnabledLayerNames = requiredLayers.data(),
                                    .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
                                    .ppEnabledExtensionNames = requiredExtensions.data()};
  instance = vk::raii::Instance(context, createInfo);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL
VulkanContext::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
                             const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
  if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError ||
      severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
    std::cerr << "Validation Layer: Type " << to_string(type) << " Msg: " << pCallbackData->pMessage << '\n';
  }
  return vk::False;
}

void VulkanContext::setupDebugMessenger() {
  if (!enableValidationLayers)
    return;
  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
  vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
  vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{
      .messageSeverity = severityFlags, .messageType = messageTypeFlags, .pfnUserCallback = &debugCallback};
  debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfo);
}

void VulkanContext::pickPhysicalDevice() {
  std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
  const auto devIter = std::ranges::find_if(devices, [&](const vk::raii::PhysicalDevice &device) {
    if (device.getProperties().apiVersion < VK_API_VERSION_1_3) {
      return false;
    }

    auto queueFamilies = device.getQueueFamilyProperties();
    if (std::ranges::none_of(queueFamilies,
                             [](const auto &qfp) { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); })) {
      return false;
    }

    auto availableDeviceExtensions = device.enumerateDeviceExtensionProperties();
    if (std::ranges::any_of(
            requiredDeviceExtensions, [&availableDeviceExtensions](const char *requiredDeviceExtension) {
              return std::ranges::none_of(
                  availableDeviceExtensions, [requiredDeviceExtension](const auto &availableDeviceExtension) {
                    return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
                  });
            })) {
      return false;
    }

    const auto features =
        device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                            vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

    const auto &core = features.get<vk::PhysicalDeviceFeatures2>().features;
    const auto &v11 = features.get<vk::PhysicalDeviceVulkan11Features>();
    const auto &v13 = features.get<vk::PhysicalDeviceVulkan13Features>();
    const auto &extDyn = features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

    return v13.dynamicRendering && v13.synchronization2 && core.samplerAnisotropy && v11.shaderDrawParameters &&
           extDyn.extendedDynamicState;
  });

  if (devIter != devices.end()) {
    physicalDevice = *devIter;
  } else {
    throw std::runtime_error("Failed to find a suitable GPU!");
  }
}

void VulkanContext::createLogicalDevice() {
  std::vector queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

  for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i) {
    if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
        physicalDevice.getSurfaceSupportKHR(i, *surface)) {
      queueIndex = i;
      break;
    }
  }

  if (queueIndex == UINT32_MAX) {
    throw std::runtime_error("Could not find a queue for graphics and present");
  }

  vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                     vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
      featureChain = {{.features = {.samplerAnisotropy = true}},
                      {.shaderDrawParameters = true},
                      {.synchronization2 = true, .dynamicRendering = true},
                      {.extendedDynamicState = true}};

  float queuePrority = 0.5f;
  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePrority};
  vk::DeviceCreateInfo deviceCreateInfo{.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
                                        .queueCreateInfoCount = 1,
                                        .pQueueCreateInfos = &deviceQueueCreateInfo,
                                        .enabledExtensionCount = requiredDeviceExtensions.size(),
                                        .ppEnabledExtensionNames = requiredDeviceExtensions.data()};

  device = vk::raii::Device(physicalDevice, deviceCreateInfo);
  queue = vk::raii::Queue(device, queueIndex, 0);
}

void VulkanContext::createCommandPool() {
  vk::CommandPoolCreateInfo poolInfo{.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                     .queueFamilyIndex = queueIndex};
  commandPool = vk::raii::CommandPool(device, poolInfo);
}

void VulkanContext::createDescriptorPool() {
  std::array poolSizes{vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, Renderer::maxFramesInFlight),
                       vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, Renderer::maxFramesInFlight)};

  vk::DescriptorPoolCreateInfo poolInfo{.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                        .maxSets = Renderer::maxFramesInFlight,
                                        .poolSizeCount = poolSizes.size(),
                                        .pPoolSizes = poolSizes.data()};
  descriptorPool = vk::raii::DescriptorPool(device, poolInfo);
}

VulkanContext::VulkanContext(const Window &window) {
  createInstance("Vulkan Graphics", VK_MAKE_VERSION(1, 0, 0));
  setupDebugMessenger();
  pickPhysicalDevice();
  surface = window.createSurface(instance);
  createLogicalDevice();
  samplers.init(physicalDevice, device);
  createCommandPool();
  createDescriptorPool();
}

void VulkanContext::submitSingleTimeCommands(std::function<void(const vk::raii::CommandBuffer &)> fn) const {
  vk::CommandBufferAllocateInfo allocInfo{
      .commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};
  auto cmd = std::move(device.allocateCommandBuffers(allocInfo).front());
  cmd.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

  fn(cmd);

  cmd.end();
  vk::raii::Fence fence{device, vk::FenceCreateInfo{}};
  queue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*cmd}, fence);
  auto result = device.waitForFences(*fence, true, UINT64_MAX);
  if (result != vk::Result::eSuccess) {
    throw std::runtime_error("VulkanContext::submitSingleTimeCommands: Fence Wait Failed");
  }
}
