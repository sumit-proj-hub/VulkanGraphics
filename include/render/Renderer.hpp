#pragma once

#include "core/VulkanContext.hpp"
#include "core/Window.hpp"
#include "render/FrameResources.hpp"
#include "render/Pipeline.hpp"
#include "render/RenderObject.hpp"
#include "render/RenderTargets.hpp"
#include "render/Swapchain.hpp"
#include "resources/ShaderProgram.hpp"
#include "scene/Camera.hpp"
#include <cstdint>

class Renderer {
  Swapchain swapchain;
  RenderTargets renderTargets;
  ShaderProgram shaderProgram;
  Pipeline pipeline;
  FrameResources frameResources;
  uint32_t frameIndex = 0;

  void recordCommands(uint32_t imageIndex, std::span<const RenderObject> renderObjects) const;

public:
  static constexpr uint32_t maxFramesInFlight = 3;

  Renderer(const VulkanContext &ctx, Window &window)
      : swapchain(ctx, window), renderTargets(ctx, swapchain.getExtent()),
        shaderProgram(ctx.getDevice(), "shaders/shader.spv"),
        pipeline(ctx, shaderProgram, swapchain.getSurfaceFormat()),
        frameResources(ctx, pipeline.getGlobalDescriptorSetLayout()) {}
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  void drawFrame(const VulkanContext &ctx, Window &window, std::span<const RenderObject> renderObjects,
                 const Camera &camera);
  float getAspectRatio() const;

  const Pipeline &getPipeline() const { return pipeline; }
};
