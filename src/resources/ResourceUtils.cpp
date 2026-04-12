#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <resources/ResourceUtils.hpp>
#include <stdexcept>

uint32_t ResourceUtils::findMemoryType(const vk::raii::PhysicalDevice &physicalDevice, uint32_t typeFilter,
                                       vk::MemoryPropertyFlags memoryPropertyFlags) {
  vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find a suitable memory type!");
}

vk::MemoryPropertyFlags ResourceUtils::toMemoryPropertyFlags(MemoryUsage memoryUsage) {
  switch (memoryUsage) {
  case MemoryUsage::DeviceLocal:
    return vk::MemoryPropertyFlagBits::eDeviceLocal;
  case MemoryUsage::HostVisible:
    return vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
  }
  throw std::runtime_error("Invalid memoryUsage!");
}

vk::Format ResourceUtils::findSupportedFormat(const vk::raii::PhysicalDevice &physicalDevice,
                                              const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
                                              vk::FormatFeatureFlags features) {
  for (auto format : candidates) {
    vk::FormatProperties props = physicalDevice.getFormatProperties(format);
    if ((tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) ||
        (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)) {
      return format;
    }
  }
  throw std::runtime_error("Failed to find supported format!");
}

vk::Format ResourceUtils::findDepthFormat(const vk::raii::PhysicalDevice &physicalDevice) {
  return ResourceUtils::findSupportedFormat(
      physicalDevice, {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
      vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}
