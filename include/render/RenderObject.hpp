#pragma once

#include "resources/Material.hpp"
#include "resources/Mesh.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <glm/matrix.hpp>

struct RenderObject {
  const Material *material = nullptr;
  const Mesh *mesh = nullptr;
  glm::mat4 transform{1.0f};

  void draw(const vk::raii::CommandBuffer &cmd, const vk::raii::PipelineLayout &pipelineLayout) const;
};
