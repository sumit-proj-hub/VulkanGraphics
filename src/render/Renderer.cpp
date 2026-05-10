#include "render/RenderObject.hpp"
#include "scene/Camera.hpp"
#include "scene/PushConstants.hpp"
#include <render/Renderer.hpp>

void Renderer::recordCommands(uint32_t imageIndex, std::span<const RenderObject> renderObjects) const {
  auto &commandBuffer = frameResources.getCommandBuffer(frameIndex);
  commandBuffer.begin({});

  Image::transitionImageLayout(swapchain.getImage(imageIndex), commandBuffer, vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eColorAttachmentOptimal, {}, vk::AccessFlagBits2::eColorAttachmentWrite,
                               vk::PipelineStageFlagBits2::eNone, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                               vk::ImageAspectFlagBits::eColor);
  Image::transitionImageLayout(
      renderTargets.getDepthImage().get(), commandBuffer, vk::ImageLayout::eUndefined,
      vk::ImageLayout::eDepthAttachmentOptimal, vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
      vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
      vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
      vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
      vk::ImageAspectFlagBits::eDepth);

  vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
  vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);
  vk::RenderingAttachmentInfo colorAttachmentInfo{.imageView = swapchain.getImageView(imageIndex),
                                                  .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
                                                  .loadOp = vk::AttachmentLoadOp::eClear,
                                                  .storeOp = vk::AttachmentStoreOp::eStore,
                                                  .clearValue = clearColor};
  vk::RenderingAttachmentInfo depthAttachmentInfo{.imageView = renderTargets.getDepthImage().getView(),
                                                  .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
                                                  .loadOp = vk::AttachmentLoadOp::eClear,
                                                  .storeOp = vk::AttachmentStoreOp::eDontCare,
                                                  .clearValue = clearDepth};
  vk::RenderingInfo renderingInfo{.renderArea = vk::Rect2D{.offset = {0, 0}, .extent = swapchain.getExtent()},
                                  .layerCount = 1,
                                  .colorAttachmentCount = 1,
                                  .pColorAttachments = &colorAttachmentInfo,
                                  .pDepthAttachment = &depthAttachmentInfo};

  commandBuffer.beginRendering(renderingInfo);
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.get());
  commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchain.getExtent().width),
                                            static_cast<float>(swapchain.getExtent().height), 0.0f, 1.0f));
  commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchain.getExtent()));
  commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.getLayout(), 0,
                                   *frameResources.getGlobalDescriptorSet(frameIndex), nullptr);
  for (auto &renderObject : renderObjects) {
    PushConstants push{.model = renderObject.transform};
    commandBuffer.pushConstants<PushConstants>(*pipeline.getLayout(), vk::ShaderStageFlagBits::eVertex, 0, push);
    renderObject.draw(commandBuffer, pipeline.getLayout());
  }
  commandBuffer.endRendering();

  Image::transitionImageLayout(swapchain.getImage(imageIndex), commandBuffer, vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, {},
                               vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eNone,
                               vk::ImageAspectFlagBits::eColor);
  commandBuffer.end();
}

void Renderer::drawFrame(const VulkanContext &ctx, Window &window, std::span<const RenderObject> renderObjects,
                         const Camera &camera) {
  auto fenceResult = ctx.getDevice().waitForFences(*frameResources.getInFlightFence(frameIndex), vk::True, UINT64_MAX);
  if (fenceResult != vk::Result::eSuccess) {
    throw std::runtime_error("Failed to wait for fence!");
  }

  auto [result, imageIndex] = swapchain.acquireNextImage(frameResources.getPresentCompleteSemaphore(frameIndex));
  if (result == vk::Result::eErrorOutOfDateKHR) {
    window.consumeFramebufferResized();
    swapchain.recreate(ctx, window);
    renderTargets.recreate(ctx, swapchain.getExtent());
    return;
  } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
    assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
    throw std::runtime_error("Failed to acquire swapchain image!");
  }

  memcpy(frameResources.getUniformBuffer(frameIndex).getMapped(), &camera, sizeof(camera));

  ctx.getDevice().resetFences(*frameResources.getInFlightFence(frameIndex));
  frameResources.getCommandBuffer(frameIndex).reset();
  recordCommands(imageIndex, renderObjects);

  vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
  const vk::SubmitInfo submitInfo{.waitSemaphoreCount = 1,
                                  .pWaitSemaphores = &*frameResources.getPresentCompleteSemaphore(frameIndex),
                                  .pWaitDstStageMask = &waitDestinationStageMask,
                                  .commandBufferCount = 1,
                                  .pCommandBuffers = &*frameResources.getCommandBuffer(frameIndex),
                                  .signalSemaphoreCount = 1,
                                  .pSignalSemaphores = &*swapchain.getRenderFinishSemaphore(imageIndex)};
  ctx.getQueue().submit(submitInfo, *frameResources.getInFlightFence(frameIndex));

  result = swapchain.present(imageIndex, ctx.getQueue());
  if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR ||
      window.consumeFramebufferResized()) {
    swapchain.recreate(ctx, window);
    renderTargets.recreate(ctx, swapchain.getExtent());
  } else {
    assert(result == vk::Result::eSuccess);
  }

  frameIndex = (frameIndex + 1) % maxFramesInFlight;
}

float Renderer::getAspectRatio() const {
  return static_cast<float>(swapchain.getExtent().width) / static_cast<float>(swapchain.getExtent().height);
}
