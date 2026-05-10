#pragma once

#include "core/VulkanContext.hpp"
#include "core/Window.hpp"
#include "render/Renderer.hpp"
#include "scene/MaterialLibrary.hpp"
#include <GLFW/glfw3.h>

class Application {
  Window window{800, 600, "Vulkan Refactored"};
  VulkanContext ctx{window};
  Renderer renderer{ctx, window};

  MaterialLibrary materials{ctx, renderer.getPipeline().getMaterialDescriptorSetLayout()};

  const std::vector<Vertex> vertices = {{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

                                        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};
  const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
  Mesh mesh{ctx, vertices, indices};

  Camera camera;
  RenderObject renderObject{.material = &materials.sample, .mesh = &mesh};

  void updateCamera(float aspectRatio);

public:
  Application() = default;
  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

  void run();
};
