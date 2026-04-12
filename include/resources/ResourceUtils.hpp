#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

enum class MemoryUsage { DeviceLocal, HostVisible };

namespace ResourceUtils {
uint32_t findMemoryType(const vk::raii::PhysicalDevice &physicalDevice, uint32_t typeFilter,
                        vk::MemoryPropertyFlags memoryPropertyFlags);
vk::MemoryPropertyFlags toMemoryPropertyFlags(MemoryUsage memoryUsage);
vk::Format findSupportedFormat(const vk::raii::PhysicalDevice &physicalDevice,
                               const std::vector<vk::Format> &candidates, vk::ImageTiling tiling,
                               vk::FormatFeatureFlags features);
vk::Format findDepthFormat(const vk::raii::PhysicalDevice &physicalDevice);
} // namespace ResourceUtils
