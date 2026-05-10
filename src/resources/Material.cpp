#include "resources/Material.hpp"

Material::Material(const VulkanContext &ctx, std::string_view texturePath,
                   const vk::raii::DescriptorSetLayout &materialDescriptorSetLayout)
    : texture(ctx, texturePath) {
  vk::DescriptorSetAllocateInfo allocInfo{
      .descriptorPool = ctx.getDescriptorPool(), .descriptorSetCount = 1, .pSetLayouts = &*materialDescriptorSetLayout};
  descriptorSet = std::move(ctx.getDevice().allocateDescriptorSets(allocInfo).front());

  vk::DescriptorImageInfo imageInfo{.sampler = ctx.samplers.linear,
                                    .imageView = texture.getImage().getView(),
                                    .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal};

  std::array descriptorWrites{
      vk::WriteDescriptorSet{.dstSet = descriptorSet,
                             .dstBinding = 0,
                             .dstArrayElement = 0,
                             .descriptorCount = 1,
                             .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                             .pImageInfo = &imageInfo},
  };
  ctx.getDevice().updateDescriptorSets(descriptorWrites, {});
}
