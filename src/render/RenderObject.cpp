#include "render/RenderObject.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <cassert>

void RenderObject::draw(const vk::raii::CommandBuffer &cmd, const vk::raii::PipelineLayout &pipelineLayout) const {
  assert(material != nullptr && mesh != nullptr);
  constexpr uint32_t materialDescriptorSetNumber = 1;
  cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, materialDescriptorSetNumber,
                         *material->getDescriptorSet(), nullptr);
  mesh->draw(cmd);
}
