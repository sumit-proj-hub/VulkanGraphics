#pragma once

#include "core/VulkanContext.hpp"
#include "resources/Image.hpp"
#include <string_view>

class Texture {
  Image textureImage;

  static Image createTextureImageFromFile(const VulkanContext &ctx, std::string_view filePath);

public:
  Texture(const VulkanContext &ctx, std::string_view filePath)
      : textureImage(createTextureImageFromFile(ctx, filePath)) {}

  const Image &getImage() const { return textureImage; }
};
