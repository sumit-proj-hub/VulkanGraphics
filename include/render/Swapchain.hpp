#pragma once

#include "core/VulkanContext.hpp"
#include "core/Window.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <vector>

class Swapchain {
  vk::raii::SwapchainKHR swapChain = nullptr;
  std::vector<vk::Image> images;
  std::vector<vk::raii::ImageView> imageViews;
  vk::SurfaceFormatKHR surfaceFormat;
  vk::Extent2D extent;
  std::vector<vk::raii::Semaphore> renderFinishSemaphores;

  void create(const VulkanContext &ctx, const Window &window);
  static vk::Extent2D chooseSwapExtent(vk::Extent2D framebufferSize, const vk::SurfaceCapabilitiesKHR &capabilities);
  static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
  static uint32_t chooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities);
  static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

public:
  Swapchain(const VulkanContext &ctx, const Window &window) {
    create(ctx, window);
    for (size_t i = 0; i < images.size(); ++i) {
      renderFinishSemaphores.emplace_back(ctx.getDevice(), vk::SemaphoreCreateInfo());
    }
  };
  Swapchain(const Swapchain &) = delete;
  Swapchain &operator=(const Swapchain &) = delete;

  void recreate(const VulkanContext &ctx, const Window &window);

  vk::ResultValue<uint32_t> acquireNextImage(vk::raii::Semaphore &presentCompleteSemaphore) const {
    return swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphore, nullptr);
  }

  vk::Result present(uint32_t imageIndex, const vk::raii::Queue &queue) const;

  const vk::Image &getImage(uint32_t index) const { return images[index]; }
  const vk::raii::ImageView &getImageView(uint32_t index) const { return imageViews[index]; }
  vk::SurfaceFormatKHR getSurfaceFormat() const { return surfaceFormat; }
  vk::Extent2D getExtent() const { return extent; }
  const vk::raii::Semaphore &getRenderFinishSemaphore(uint32_t imageIndex) const {
    return renderFinishSemaphores[imageIndex];
  }
};
