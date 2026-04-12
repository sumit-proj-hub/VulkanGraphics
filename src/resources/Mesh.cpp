#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <resources/Mesh.hpp>

Buffer Mesh::createVertexBuffer(const VulkanContext &ctx, std::span<const Vertex> vertices) {
  Buffer stagingBuffer = Buffer::createHostVisible(ctx, {}, vertices.data(), vertices.size() * sizeof(Vertex), true);
  return Buffer::createDeviceLocal(ctx, stagingBuffer, vk::BufferUsageFlagBits::eVertexBuffer);
}

Buffer Mesh::createIndexBuffer(const VulkanContext &ctx, std::span<const uint16_t> indices) {
  Buffer stagingBuffer = Buffer::createHostVisible(ctx, {}, indices.data(), indices.size() * sizeof(uint16_t), true);
  return Buffer::createDeviceLocal(ctx, stagingBuffer, vk::BufferUsageFlagBits::eIndexBuffer);
}

Mesh::Mesh(const VulkanContext &ctx, std::span<const Vertex> vertices, std::span<const uint16_t> indices)
    : vertexBuffer(createVertexBuffer(ctx, vertices)), indexBuffer(createIndexBuffer(ctx, indices)),
      indexCount(indices.size()) {}

void Mesh::bind(const vk::raii::CommandBuffer &cmd) const {
  cmd.bindVertexBuffers(0, *vertexBuffer.get(), {0});
  cmd.bindIndexBuffer(*indexBuffer.get(), 0, vk::IndexType::eUint16);
}

void Mesh::draw(const vk::raii::CommandBuffer &cmd) const { cmd.drawIndexed(indexCount, 1, 0, 0, 0); }
