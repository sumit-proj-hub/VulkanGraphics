#pragma once

#include "core/VulkanContext.hpp"
#include "resources/ShaderProgram.hpp"
#include "vulkan/vulkan_raii.hpp"

class Pipeline {
  vk::raii::Pipeline pipeline = nullptr;
  vk::raii::PipelineLayout pipelineLayout = nullptr;

public:
  Pipeline(const VulkanContext &ctx, const ShaderProgram &shaderProgram, vk::SurfaceFormatKHR swapchainSurfaceFormat);
  Pipeline(const Pipeline &) = delete;
  Pipeline &operator=(const Pipeline &) = delete;

  const vk::raii::Pipeline &get() const { return pipeline; }
  const vk::raii::PipelineLayout &getLayout() const { return pipelineLayout; }
};
