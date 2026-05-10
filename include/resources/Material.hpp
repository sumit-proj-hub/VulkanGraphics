#pragma once

#include "core/VulkanContext.hpp"
#include "resources/Texture.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <string_view>

class Material {
  Texture texture;
  vk::raii::DescriptorSet descriptorSet = nullptr;

public:
  Material(const VulkanContext &ctx, std::string_view texturePath,
           const vk::raii::DescriptorSetLayout &materialDescriptorSetLayout);

  const vk::raii::DescriptorSet &getDescriptorSet() const { return descriptorSet; }
};
