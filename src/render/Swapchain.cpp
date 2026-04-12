#include "core/VulkanContext.hpp"
#include "core/Window.hpp"
#include "vulkan/vulkan.hpp"
#include <render/Swapchain.hpp>

void Swapchain::create(const VulkanContext &ctx, const Window &window) {
  auto surfaceCapabilities = ctx.getPhysicalDevice().getSurfaceCapabilitiesKHR(*ctx.getSurface());
  extent = chooseSwapExtent(window.getFramebufferSize(), surfaceCapabilities);
  surfaceFormat = chooseSwapSurfaceFormat(ctx.getPhysicalDevice().getSurfaceFormatsKHR(*ctx.getSurface()));
  vk::SwapchainCreateInfoKHR swapChainCreateInfo{
      .surface = *ctx.getSurface(),
      .minImageCount = chooseSwapMinImageCount(surfaceCapabilities),
      .imageFormat = surfaceFormat.format,
      .imageColorSpace = surfaceFormat.colorSpace,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .imageSharingMode = vk::SharingMode::eExclusive,
      .preTransform = surfaceCapabilities.currentTransform,
      .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode = chooseSwapPresentMode(ctx.getPhysicalDevice().getSurfacePresentModesKHR(*ctx.getSurface())),
      .clipped = true};
  swapChain = vk::raii::SwapchainKHR(ctx.getDevice(), swapChainCreateInfo);
  images = swapChain.getImages();

  vk::ImageViewCreateInfo imageViewCreateInfo{.viewType = vk::ImageViewType::e2D,
                                              .format = surfaceFormat.format,
                                              .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
  for (auto &image : images) {
    imageViewCreateInfo.image = image;
    imageViews.emplace_back(ctx.getDevice(), imageViewCreateInfo);
  }
}

vk::Extent2D Swapchain::chooseSwapExtent(vk::Extent2D framebufferSize, const vk::SurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  }

  return {
      std::clamp<uint32_t>(framebufferSize.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
      std::clamp<uint32_t>(framebufferSize.height, capabilities.minImageExtent.height,
                           capabilities.maxImageExtent.height)};
}

vk::SurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
  assert(!availableFormats.empty());
  const auto formatIt = std::ranges::find_if(availableFormats, [](const vk::SurfaceFormatKHR &format) {
    return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
  });
  return formatIt == availableFormats.end() ? availableFormats[0] : *formatIt;
}

vk::PresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
  assert(std::ranges::any_of(availablePresentModes,
                             [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
  return std::ranges::any_of(availablePresentModes,
                             [](auto presentMode) { return presentMode == vk::PresentModeKHR::eMailbox; })
             ? vk::PresentModeKHR::eMailbox
             : vk::PresentModeKHR::eFifo;
}

uint32_t Swapchain::chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities) {
  uint32_t minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
  if (0 < surfaceCapabilities.maxImageCount && surfaceCapabilities.maxImageCount < minImageCount) {
    minImageCount = surfaceCapabilities.maxImageCount;
  }
  return minImageCount;
}

vk::Result Swapchain::present(uint32_t imageIndex, const vk::raii::Queue &queue) const {
  const vk::PresentInfoKHR presentInfoKHR{.waitSemaphoreCount = 1,
                                          .pWaitSemaphores = &*renderFinishSemaphores[imageIndex],
                                          .swapchainCount = 1,
                                          .pSwapchains = &*swapChain,
                                          .pImageIndices = &imageIndex};
  return queue.presentKHR(presentInfoKHR);
}
