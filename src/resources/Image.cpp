#include "core/VulkanContext.hpp"
#include <resources/Image.hpp>
#include <resources/ResourceUtils.hpp>

Image::Image(const VulkanContext &ctx, vk::Extent2D imageExtent, vk::Format format, vk::ImageTiling tiling,
             vk::ImageUsageFlags usage, MemoryUsage MemoryUsage, vk::ImageAspectFlags aspectFlags) {
  vk::ImageCreateInfo imageInfo{.imageType = vk::ImageType::e2D,
                                .format = format,
                                .extent = {imageExtent.width, imageExtent.height, 1},
                                .mipLevels = 1,
                                .arrayLayers = 1,
                                .samples = vk::SampleCountFlagBits::e1,
                                .tiling = tiling,
                                .usage = usage,
                                .sharingMode = vk::SharingMode::eExclusive};
  image = vk::raii::Image(ctx.getDevice(), imageInfo);
  vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
  vk::MemoryAllocateInfo allocInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = ResourceUtils::findMemoryType(ctx.getPhysicalDevice(), memRequirements.memoryTypeBits,
                                                       ResourceUtils::toMemoryPropertyFlags(MemoryUsage))};
  imageMemory = vk::raii::DeviceMemory(ctx.getDevice(), allocInfo);
  image.bindMemory(imageMemory, 0);
  vk::ImageViewCreateInfo viewInfo{.image = image,
                                   .viewType = vk::ImageViewType::e2D,
                                   .format = format,
                                   .subresourceRange = {aspectFlags, 0, 1, 0, 1}};
  imageView = vk::raii::ImageView(ctx.getDevice(), viewInfo);
}

void Image::transitionImageLayout(const vk::Image &image, const vk::raii::CommandBuffer &commandBuffer,
                                  vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask,
                                  vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask,
                                  vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspectFlags) {
  vk::ImageMemoryBarrier2 barrier{
      .srcStageMask = srcStageMask,
      .srcAccessMask = srcAccessMask,
      .dstStageMask = dstStageMask,
      .dstAccessMask = dstAccessMask,
      .oldLayout = oldLayout,
      .newLayout = newLayout,
      .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
      .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
      .image = image,
      .subresourceRange = vk::ImageSubresourceRange{
          .aspectMask = aspectFlags, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};
  vk::DependencyInfo dependencyInfo{
      .dependencyFlags = {}, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrier};
  commandBuffer.pipelineBarrier2(dependencyInfo);
}
