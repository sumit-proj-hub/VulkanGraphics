#pragma once

#include "core/VulkanContext.hpp"
#include "resources/Buffer.hpp"
#include "vulkan/vulkan_raii.hpp"

class FrameResources {
  std::vector<Buffer> uniformBuffers;
  std::vector<vk::raii::CommandBuffer> commandBuffers;
  std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
  std::vector<vk::raii::Fence> inFlightFences;
  std::vector<vk::raii::DescriptorSet> globalDescriptorSets;

  static std::vector<Buffer> createUniformBuffers(const VulkanContext &ctx);
  void createGlobalDescriptorSets(const VulkanContext &ctx,
                                  const vk::raii::DescriptorSetLayout &gloablDescriptorSetLayout);
  void createCommandBuffers(const VulkanContext &ctx);
  void createSyncObjects(const VulkanContext &ctx);

public:
  FrameResources(const VulkanContext &ctx, const vk::raii::DescriptorSetLayout &globalDescriptorSetLayout)
      : uniformBuffers(createUniformBuffers(ctx)) {
    createCommandBuffers(ctx);
    createSyncObjects(ctx);
    createGlobalDescriptorSets(ctx, globalDescriptorSetLayout);
  }
  FrameResources(const FrameResources &) = delete;
  FrameResources &operator=(const FrameResources &) = delete;

  const vk::raii::CommandBuffer &getCommandBuffer(uint32_t frameIndex) const { return commandBuffers[frameIndex]; }
  const Buffer &getUniformBuffer(uint32_t frameIndex) const { return uniformBuffers[frameIndex]; }
  vk::raii::Semaphore &getPresentCompleteSemaphore(uint32_t frameIndex) {
    return presentCompleteSemaphores[frameIndex];
  }
  const vk::raii::Fence &getInFlightFence(uint32_t frameIndex) const { return inFlightFences[frameIndex]; }
  const vk::raii::DescriptorSet &getGlobalDescriptorSet(uint32_t frameIndex) const {
    return globalDescriptorSets[frameIndex];
  }
};
