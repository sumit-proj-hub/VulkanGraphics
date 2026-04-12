#pragma once

#include "core/VulkanContext.hpp"
#include "resources/Vertex.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <resources/Buffer.hpp>

class Mesh {
  Buffer vertexBuffer;
  Buffer indexBuffer;
  uint32_t indexCount = 0;

  static Buffer createVertexBuffer(const VulkanContext &ctx, std::span<const Vertex> vertices);
  static Buffer createIndexBuffer(const VulkanContext &ctx, std::span<const uint16_t> indices);

public:
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;

  Mesh(const VulkanContext &ctx, std::span<const Vertex> vertices, std::span<const uint16_t> indices);
  void bind(const vk::raii::CommandBuffer &cmd) const;
  void draw(const vk::raii::CommandBuffer &cmd) const;
};
