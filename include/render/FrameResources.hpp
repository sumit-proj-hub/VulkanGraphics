#pragma once

#include "core/VulkanContext.hpp"
#include "resources/Buffer.hpp"
#include "resources/Texture.hpp"
#include "vulkan/vulkan_raii.hpp"

class FrameResources {
  std::vector<Buffer> uniformBuffers;
  std::vector<vk::raii::CommandBuffer> commandBuffers;
  std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
  std::vector<vk::raii::Fence> inFlightFences;
  std::vector<vk::raii::DescriptorSet> descriptorSets;

  static std::vector<Buffer> createUniformBuffers(const VulkanContext &ctx);
  void createDescriptorSets(const VulkanContext &ctx, const vk::raii::DescriptorSetLayout &descriptorSetLayout,
                            const Texture &texture);
  void createCommandBuffers(const VulkanContext &ctx);
  void createSyncObjects(const VulkanContext &ctx);

public:
  FrameResources(const VulkanContext &ctx, const vk::raii::DescriptorSetLayout &descriptorSetLayout,
                 const Texture &texture)
      : uniformBuffers(createUniformBuffers(ctx)) {
    createCommandBuffers(ctx);
    createSyncObjects(ctx);
    createDescriptorSets(ctx, descriptorSetLayout, texture);
  }

  const vk::raii::CommandBuffer &getCommandBuffer(uint32_t frameIndex) const { return commandBuffers[frameIndex]; }
  const Buffer &getUniformBuffer(uint32_t frameIndex) const { return uniformBuffers[frameIndex]; }
  vk::raii::Semaphore &getPresentCompleteSemaphore(uint32_t frameIndex) {
    return presentCompleteSemaphores[frameIndex];
  }
  const vk::raii::Fence &getInFlightFence(uint32_t frameIndex) const { return inFlightFences[frameIndex]; }
  const vk::raii::DescriptorSet &getDescriptorSet(uint32_t frameIndex) const { return descriptorSets[frameIndex]; }
};
