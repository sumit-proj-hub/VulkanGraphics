#pragma once

#include "core/VulkanContext.hpp"
#include "core/Window.hpp"
#include "render/FrameResources.hpp"
#include "render/Pipeline.hpp"
#include "render/RenderTargets.hpp"
#include "render/SceneUBO.hpp"
#include "render/Swapchain.hpp"
#include "resources/Mesh.hpp"
#include "resources/ShaderProgram.hpp"
#include "resources/Texture.hpp"
#include <cstdint>
#include <functional>

class Renderer {
  Swapchain swapchain;
  RenderTargets renderTargets;
  ShaderProgram shaderProgram;
  Pipeline pipeline;
  Texture texture;
  FrameResources frameResources;
  uint32_t frameIndex = 0;

  void recordCommands(uint32_t imageIndex, const Mesh &mesh) const;

public:
  static constexpr uint32_t maxFramesInFlight = 3;

  Renderer(const VulkanContext &ctx, Window &window)
      : swapchain(ctx, window), renderTargets(ctx, swapchain.getExtent()),
        shaderProgram(ctx.getDevice(), "shaders/shader.spv"),
        pipeline(ctx, shaderProgram, swapchain.getSurfaceFormat()), texture(ctx, "textures/texture.jpg"),
        frameResources(ctx, shaderProgram.getDescriptorSetLayout(), texture) {}
  void drawFrame(const VulkanContext &ctx, Window &window, const Mesh &mesh,
                 std::function<void(SceneUBO &, float)> updateScene);
};
