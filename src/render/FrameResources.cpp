#include "core/VulkanContext.hpp"
#include "render/Renderer.hpp"
#include "render/SceneUBO.hpp"
#include "resources/Texture.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <cassert>
#include <render/FrameResources.hpp>

std::vector<Buffer> FrameResources::createUniformBuffers(const VulkanContext &ctx) {
  std::vector<Buffer> uniformBuffers;
  for (uint32_t i = 0; i < Renderer::maxFramesInFlight; ++i) {
    uniformBuffers.emplace_back(
        Buffer::createHostVisible(ctx, vk::BufferUsageFlagBits::eUniformBuffer, nullptr, sizeof(SceneUBO), false));
  }
  return uniformBuffers;
}

void FrameResources::createDescriptorSets(const VulkanContext &ctx,
                                          const vk::raii::DescriptorSetLayout &descriptorSetLayout,
                                          const Texture &texture) {
  std::vector<vk::DescriptorSetLayout> layouts(Renderer::maxFramesInFlight, *descriptorSetLayout);
  vk::DescriptorSetAllocateInfo allocInfo{.descriptorPool = ctx.getDescriptorPool(),
                                          .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
                                          .pSetLayouts = layouts.data()};
  descriptorSets = ctx.getDevice().allocateDescriptorSets(allocInfo);

  for (uint32_t i = 0; i < Renderer::maxFramesInFlight; ++i) {
    vk::DescriptorBufferInfo bufferInfo{.buffer = uniformBuffers[i].get(), .offset = 0, .range = sizeof(SceneUBO)};
    vk::DescriptorImageInfo imageInfo{.sampler = ctx.samplers.linear,
                                      .imageView = texture.getImage().getView(),
                                      .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal};

    std::array descriptorWrites{
        vk::WriteDescriptorSet{.dstSet = descriptorSets[i],
                               .dstBinding = 0,
                               .dstArrayElement = 0,
                               .descriptorCount = 1,
                               .descriptorType = vk::DescriptorType::eUniformBuffer,
                               .pBufferInfo = &bufferInfo},
        vk::WriteDescriptorSet{.dstSet = descriptorSets[i],
                               .dstBinding = 1,
                               .dstArrayElement = 0,
                               .descriptorCount = 1,
                               .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                               .pImageInfo = &imageInfo},
    };
    ctx.getDevice().updateDescriptorSets(descriptorWrites, {});
  }
}

void FrameResources::createCommandBuffers(const VulkanContext &ctx) {
  vk::CommandBufferAllocateInfo allocInfo{.commandPool = ctx.getCommandPool(),
                                          .level = vk::CommandBufferLevel::ePrimary,
                                          .commandBufferCount = Renderer::maxFramesInFlight};
  commandBuffers = vk::raii::CommandBuffers(ctx.getDevice(), allocInfo);
}

void FrameResources::createSyncObjects(const VulkanContext &ctx) {
  assert(presentCompleteSemaphores.empty() && inFlightFences.empty());
  for (uint32_t i = 0; i < Renderer::maxFramesInFlight; ++i) {
    presentCompleteSemaphores.emplace_back(ctx.getDevice(), vk::SemaphoreCreateInfo());
    inFlightFences.emplace_back(ctx.getDevice(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
  }
}
