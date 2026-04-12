#include "core/VulkanContext.hpp"
#include "resources/ResourceUtils.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <resources/Buffer.hpp>

Buffer::Buffer(const VulkanContext &ctx, vk::BufferUsageFlags usage, vk::DeviceSize size, MemoryUsage memoryUsage)
    : bufferSize(size) {
  vk::BufferCreateInfo bufferInfo{.size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive};
  buffer = vk::raii::Buffer(ctx.getDevice(), bufferInfo);
  vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
  vk::MemoryAllocateInfo allocInfo{
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = ResourceUtils::findMemoryType(ctx.getPhysicalDevice(), memRequirements.memoryTypeBits,
                                                       ResourceUtils::toMemoryPropertyFlags(memoryUsage))};
  bufferMemory = vk::raii::DeviceMemory(ctx.getDevice(), allocInfo);
  buffer.bindMemory(*bufferMemory, 0);
}

Buffer Buffer::createHostVisible(const VulkanContext &ctx, vk::BufferUsageFlags usage, const void *data, size_t size,
                                 bool isStaging) {
  if (isStaging) {
    usage |= vk::BufferUsageFlagBits::eTransferSrc;
  }
  Buffer hostVisibleBuffer(ctx, usage, size, MemoryUsage::HostVisible);
  hostVisibleBuffer.mapped = hostVisibleBuffer.bufferMemory.mapMemory(0, size);
  if (data != nullptr) {
    memcpy(hostVisibleBuffer.mapped, data, size);
  }
  if (isStaging) {
    hostVisibleBuffer.bufferMemory.unmapMemory();
    hostVisibleBuffer.mapped = nullptr;
  }
  return hostVisibleBuffer;
}

Buffer Buffer::createDeviceLocal(const VulkanContext &ctx, const Buffer &stagingBuffer, vk::BufferUsageFlags usage) {
  usage |= vk::BufferUsageFlagBits::eTransferDst;
  Buffer deviceLocalBuffer(ctx, usage, stagingBuffer.bufferSize, MemoryUsage::DeviceLocal);
  ctx.submitSingleTimeCommands([&](const vk::raii::CommandBuffer &cmd) {
    cmd.copyBuffer(*stagingBuffer.buffer, *deviceLocalBuffer.buffer, vk::BufferCopy(0, 0, stagingBuffer.bufferSize));
  });
  return deviceLocalBuffer;
}

Buffer::Buffer(Buffer &&other) noexcept
    : buffer(std::move(other.buffer)), bufferMemory(std::move(other.bufferMemory)), bufferSize(other.bufferSize),
      mapped(std::exchange(other.mapped, nullptr)) {}

Buffer &Buffer::operator=(Buffer &&other) noexcept {
  if (this != &other) {
    if (mapped) {
      bufferMemory.unmapMemory();
    }
    buffer = std::move(other.buffer);
    bufferMemory = std::move(other.bufferMemory);
    mapped = std::exchange(other.mapped, nullptr);
    bufferSize = other.bufferSize;
  }
  return *this;
}

Buffer::~Buffer() {
  if (mapped) {
    bufferMemory.unmapMemory();
  }
}
