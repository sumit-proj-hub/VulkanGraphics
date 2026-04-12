#pragma once

#include "core/VulkanContext.hpp"
#include "vulkan/vulkan.hpp"
#include <resources/Image.hpp>

class RenderTargets {
  static Image createDepthImage(const VulkanContext &ctx, vk::Extent2D swapChainExtent);

  Image depthImage;

public:
  RenderTargets(const VulkanContext &ctx, vk::Extent2D swapChainExtent)
      : depthImage(createDepthImage(ctx, swapChainExtent)) {}
  RenderTargets(const RenderTargets &) = delete;
  RenderTargets &operator=(const RenderTargets &) = delete;

  void recreate(const VulkanContext &ctx, vk::Extent2D swapChainExtent) {
    depthImage = createDepthImage(ctx, swapChainExtent);
  }

  const Image &getDepthImage() const { return depthImage; }
};
