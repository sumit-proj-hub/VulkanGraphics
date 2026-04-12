#pragma once

#include "core/SamplerLibrary.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <core/Window.hpp>
#include <functional>

class VulkanContext {
#ifdef NDEBUG
  static constexpr bool enableValidationLayers = false;
#else
  static constexpr bool enableValidationLayers = true;
#endif

  static constexpr std::array validationLayers{"VK_LAYER_KHRONOS_validation"};
  constexpr static std::array requiredDeviceExtensions{vk::KHRSwapchainExtensionName, vk::KHRSpirv14ExtensionName,
                                                       vk::KHRSynchronization2ExtensionName,
                                                       vk::KHRCreateRenderpass2ExtensionName};

  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
  vk::DebugUtilsMessengerEXT debugMessenger = nullptr;
  vk::raii::SurfaceKHR surface = nullptr;
  vk::raii::PhysicalDevice physicalDevice = nullptr;
  vk::raii::Device device = nullptr;
  vk::raii::Queue queue = nullptr;
  vk::raii::CommandPool commandPool = nullptr;
  vk::raii::DescriptorPool descriptorPool = nullptr;
  uint32_t queueIndex = UINT32_MAX;

  void createInstance(std::string_view applicationName, uint32_t version);
  static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                        vk::DebugUtilsMessageTypeFlagsEXT type,
                                                        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *);
  void setupDebugMessenger();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createCommandPool();
  void createDescriptorPool();

public:
  SamplerLibrary samplers;

  VulkanContext(const Window &window);
  VulkanContext(const VulkanContext &) = delete;
  VulkanContext &operator=(const VulkanContext &) = delete;

  const vk::raii::SurfaceKHR &getSurface() const { return surface; }
  const vk::raii::PhysicalDevice &getPhysicalDevice() const { return physicalDevice; }
  const vk::raii::Device &getDevice() const { return device; }
  const vk::raii::Queue &getQueue() const { return queue; }
  const vk::raii::CommandPool &getCommandPool() const { return commandPool; }
  const vk::raii::DescriptorPool &getDescriptorPool() const { return descriptorPool; }

  void submitSingleTimeCommands(std::function<void(const vk::raii::CommandBuffer &)> fn) const;
};
