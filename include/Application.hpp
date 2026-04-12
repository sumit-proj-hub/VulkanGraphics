#pragma once

#include "core/VulkanContext.hpp"
#include "core/Window.hpp"
#include "render/Renderer.hpp"
#include "render/SceneUBO.hpp"
#include <GLFW/glfw3.h>

class Application {
  Window window;
  VulkanContext ctx;
  Renderer renderer;

  const std::vector<Vertex> vertices = {{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

                                        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

  const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

  Mesh mesh;

  static void updateScene(SceneUBO &ubo, float aspectRatio);

public:
  Application()
      : window(800, 600, "Vulkan Refactored"), ctx(window), renderer(ctx, window), mesh(ctx, vertices, indices) {}
  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void run();
};
