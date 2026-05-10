#pragma once

#include "core/VulkanContext.hpp"
#include "resources/ShaderProgram.hpp"
#include "vulkan/vulkan_raii.hpp"

class Pipeline {
  vk::raii::Pipeline pipeline = nullptr;
  vk::raii::PipelineLayout pipelineLayout = nullptr;
  vk::raii::DescriptorSetLayout materialDescriptorSetLayout = nullptr;
  vk::raii::DescriptorSetLayout globalDescriptorSetLayout = nullptr;

  static constexpr std::array materialDescriptorSetBinding{vk::DescriptorSetLayoutBinding(
      0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)};
  static constexpr std::array globalDescriptorSetBinding{vk::DescriptorSetLayoutBinding(
      0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr)};

public:
  Pipeline(const VulkanContext &ctx, const ShaderProgram &shaderProgram, vk::SurfaceFormatKHR swapchainSurfaceFormat);
  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  const vk::raii::Pipeline &get() const { return pipeline; }
  const vk::raii::PipelineLayout &getLayout() const { return pipelineLayout; }
  const vk::raii::DescriptorSetLayout &getMaterialDescriptorSetLayout() const { return materialDescriptorSetLayout; }
  const vk::raii::DescriptorSetLayout &getGlobalDescriptorSetLayout() const { return globalDescriptorSetLayout; }
};
