#pragma once

#include "core/VulkanContext.hpp"
#include "resources/Image.hpp"
#include <string_view>

class Texture {
  Image textureImage;

public:
  Texture(const VulkanContext &ctx, std::string_view filePath);

  const Image &getImage() const { return textureImage; }
};
