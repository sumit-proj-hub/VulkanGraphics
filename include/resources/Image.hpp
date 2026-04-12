#pragma once

#include "core/VulkanContext.hpp"
#include "resources/ResourceUtils.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

class Image {
  vk::raii::Image image = nullptr;
  vk::raii::DeviceMemory imageMemory = nullptr;
  vk::raii::ImageView imageView = nullptr;

public:
  Image(const VulkanContext &ctx, vk::Extent2D imageExtent, vk::Format format, vk::ImageTiling tiling,
        vk::ImageUsageFlags imageUsage, MemoryUsage memoryUsage, vk::ImageAspectFlags aspectFlags);
  Image(const Image &) = delete;
  Image &operator=(const Image &) = delete;
  Image(Image &&) = default;
  Image &operator=(Image &&) = default;

  const vk::raii::Image &get() const { return image; }
  const vk::raii::ImageView &getView() const { return imageView; }

  static void transitionImageLayout(const vk::Image &image, const vk::raii::CommandBuffer &commandBuffer,
                                    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                    vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
                                    vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask,
                                    vk::ImageAspectFlags aspectFlags);
};
