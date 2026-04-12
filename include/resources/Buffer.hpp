#pragma once

#include "core/VulkanContext.hpp"
#include "resources/ResourceUtils.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"

class Buffer {
  vk::raii::Buffer buffer = nullptr;
  vk::raii::DeviceMemory bufferMemory = nullptr;
  vk::DeviceSize bufferSize = 0;
  void *mapped = nullptr;

  Buffer(const VulkanContext &ctx, vk::BufferUsageFlags usage, vk::DeviceSize size, MemoryUsage memoryUsage);

public:
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;
  Buffer(Buffer &&) noexcept;
  Buffer &operator=(Buffer &&) noexcept;

  static Buffer createHostVisible(const VulkanContext &ctx, vk::BufferUsageFlags usage, const void *data, size_t size,
                                  bool isStaging);
  static Buffer createDeviceLocal(const VulkanContext &ctx, const Buffer &stagingBuffer, vk::BufferUsageFlags usage);
  const vk::raii::Buffer &get() const { return buffer; }
  void *getMapped() const { return mapped; }

  ~Buffer();
};
