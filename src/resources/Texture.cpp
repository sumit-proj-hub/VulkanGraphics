#include "core/VulkanContext.hpp"
#include "resources/Buffer.hpp"
#include "resources/ResourceUtils.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <resources/Texture.hpp>
#include <stb_image.h>
#include <string_view>

Image Texture::createTextureImageFromFile(const VulkanContext &ctx, std::string_view filePath) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(filePath.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  vk::DeviceSize imageSize = texWidth * texHeight * 4;

  if (!pixels) {
    throw std::runtime_error("Failed to load texture image!");
  }

  Buffer stagingBuffer = Buffer::createHostVisible(ctx, {}, pixels, imageSize, true);
  stbi_image_free(pixels);

  Image textureImage(ctx, {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)},
                     vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
                     vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, MemoryUsage::DeviceLocal,
                     vk::ImageAspectFlagBits::eColor);

  ctx.submitSingleTimeCommands([&](const vk::raii::CommandBuffer &cmd) {
    Image::transitionImageLayout(*textureImage.get(), cmd, vk::ImageLayout::eUndefined,
                                 vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits2::eNone,
                                 vk::AccessFlagBits2::eTransferWrite, vk::PipelineStageFlagBits2::eNone,
                                 vk::PipelineStageFlagBits2::eTransfer, vk::ImageAspectFlagBits::eColor);

    vk::BufferImageCopy region{
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource =
            vk::ImageSubresourceLayers{
                .aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1}};
    cmd.copyBufferToImage(*stagingBuffer.get(), *textureImage.get(), vk::ImageLayout::eTransferDstOptimal, region);

    Image::transitionImageLayout(*textureImage.get(), cmd, vk::ImageLayout::eTransferDstOptimal,
                                 vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits2::eTransferWrite,
                                 vk::AccessFlagBits2::eShaderRead, vk::PipelineStageFlagBits2::eTransfer,
                                 vk::PipelineStageFlagBits2::eFragmentShader, vk::ImageAspectFlagBits::eColor);
  });
  return textureImage;
}
