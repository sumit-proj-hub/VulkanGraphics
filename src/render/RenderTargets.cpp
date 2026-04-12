#include "core/VulkanContext.hpp"
#include "resources/ResourceUtils.hpp"
#include "vulkan/vulkan.hpp"
#include <render/RenderTargets.hpp>

Image RenderTargets::createDepthImage(const VulkanContext &ctx, vk::Extent2D swapChainExtent) {
  vk::Format depthFormat = ResourceUtils::findDepthFormat(ctx.getPhysicalDevice());
  return Image(ctx, swapChainExtent, depthFormat, vk::ImageTiling::eOptimal,
               vk::ImageUsageFlagBits::eDepthStencilAttachment, MemoryUsage::DeviceLocal,
               vk::ImageAspectFlagBits::eDepth);
}
