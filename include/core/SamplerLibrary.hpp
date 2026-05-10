#pragma once

#include "vulkan/vulkan_raii.hpp"

struct SamplerLibrary {
  vk::raii::Sampler linear = nullptr;

  void init(const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::Device &device);
  
  SamplerLibrary() = default;
  SamplerLibrary(const SamplerLibrary &) = delete;
  SamplerLibrary &operator=(const SamplerLibrary &) = delete;
};
