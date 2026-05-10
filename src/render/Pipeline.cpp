#include "core/VulkanContext.hpp"
#include "resources/ResourceUtils.hpp"
#include "resources/ShaderProgram.hpp"
#include "resources/Vertex.hpp"
#include "scene/PushConstants.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <render/Pipeline.hpp>

Pipeline::Pipeline(const VulkanContext &ctx, const ShaderProgram &shaderProgram,
                   vk::SurfaceFormatKHR swapchainSurfaceFormat) {
  vk::DescriptorSetLayoutCreateInfo layoutInfo{.bindingCount = materialDescriptorSetBinding.size(),
                                               .pBindings = materialDescriptorSetBinding.data()};
  materialDescriptorSetLayout = vk::raii::DescriptorSetLayout(ctx.getDevice(), layoutInfo);
  layoutInfo = {.bindingCount = globalDescriptorSetBinding.size(), .pBindings = globalDescriptorSetBinding.data()};
  globalDescriptorSetLayout = vk::raii::DescriptorSetLayout(ctx.getDevice(), layoutInfo);

  vk::PipelineShaderStageCreateInfo vertShaderStageInfo{.stage = vk::ShaderStageFlagBits::eVertex,
                                                        .module = shaderProgram.getShaderModule(),
                                                        .pName = ShaderProgram::vertEntry};
  vk::PipelineShaderStageCreateInfo fragShaderStageInfo{.stage = vk::ShaderStageFlagBits::eFragment,
                                                        .module = shaderProgram.getShaderModule(),
                                                        .pName = ShaderProgram::fragEntry};
  vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  auto bindingDescription = Vertex::getBindingDescription();
  auto attributeDescriptions = Vertex::getAttributeDescriptions();
  vk::PipelineVertexInputStateCreateInfo vertexInputInfo{.vertexBindingDescriptionCount = 1,
                                                         .pVertexBindingDescriptions = &bindingDescription,
                                                         .vertexAttributeDescriptionCount =
                                                             attributeDescriptions.size(),
                                                         .pVertexAttributeDescriptions = attributeDescriptions.data()};

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology = vk::PrimitiveTopology::eTriangleList};
  vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1, .scissorCount = 1};
  vk::PipelineRasterizationStateCreateInfo rasterizer{.depthClampEnable = vk::False,
                                                      .rasterizerDiscardEnable = vk::False,
                                                      .polygonMode = vk::PolygonMode::eFill,
                                                      .cullMode = vk::CullModeFlagBits::eBack,
                                                      .frontFace = vk::FrontFace::eCounterClockwise,
                                                      .depthBiasEnable = vk::False,
                                                      .depthBiasSlopeFactor = 1.0f,
                                                      .lineWidth = 1.0f};
  vk::PipelineMultisampleStateCreateInfo multisampling{.rasterizationSamples = vk::SampleCountFlagBits::e1,
                                                       .sampleShadingEnable = vk::False};
  vk::PipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable = vk::False,
      .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
  vk::PipelineColorBlendStateCreateInfo colorBlending{.logicOpEnable = vk::False,
                                                      .logicOp = vk::LogicOp::eCopy,
                                                      .attachmentCount = 1,
                                                      .pAttachments = &colorBlendAttachment};
  vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{.depthTestEnable = vk::True,
                                                           .depthWriteEnable = vk::True,
                                                           .depthCompareOp = vk::CompareOp::eLess,
                                                           .depthBoundsTestEnable = vk::False,
                                                           .stencilTestEnable = vk::False};

  std::vector dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dynamicState{.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
                                                  .pDynamicStates = dynamicStates.data()};

  std::array<vk::DescriptorSetLayout, 2> setLayouts = {*globalDescriptorSetLayout, *materialDescriptorSetLayout};
  vk::PushConstantRange pushConstantRange{
      .stageFlags = vk::ShaderStageFlagBits::eVertex, .offset = 0, .size = sizeof(PushConstants)};
  vk::PipelineLayoutCreateInfo pipelineLayoutInfo{.setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
                                                  .pSetLayouts = setLayouts.data(),
                                                  .pushConstantRangeCount = 1,
                                                  .pPushConstantRanges = &pushConstantRange};
  pipelineLayout = vk::raii::PipelineLayout(ctx.getDevice(), pipelineLayoutInfo);

  vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain{
      vk::GraphicsPipelineCreateInfo{.stageCount = 2,
                                     .pStages = shaderStages,
                                     .pVertexInputState = &vertexInputInfo,
                                     .pInputAssemblyState = &inputAssembly,
                                     .pViewportState = &viewportState,
                                     .pRasterizationState = &rasterizer,
                                     .pMultisampleState = &multisampling,
                                     .pDepthStencilState = &depthStencilInfo,
                                     .pColorBlendState = &colorBlending,
                                     .pDynamicState = &dynamicState,
                                     .layout = pipelineLayout,
                                     .renderPass = nullptr},
      vk::PipelineRenderingCreateInfo{.colorAttachmentCount = 1,
                                      .pColorAttachmentFormats = &swapchainSurfaceFormat.format,
                                      .depthAttachmentFormat =
                                          ResourceUtils::findDepthFormat(ctx.getPhysicalDevice())}};
  pipeline =
      vk::raii::Pipeline(ctx.getDevice(), nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
}
