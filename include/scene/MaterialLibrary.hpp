#pragma once

#include "resources/Material.hpp"

struct MaterialLibrary {
  Material sample;

  MaterialLibrary(const VulkanContext &ctx, const vk::raii::DescriptorSetLayout &materialDescriptorSetLayout)
      : sample(ctx, "textures/texture.jpg", materialDescriptorSetLayout) {}

  MaterialLibrary(const MaterialLibrary &) = delete;
  MaterialLibrary &operator=(const MaterialLibrary &) = delete;
};
