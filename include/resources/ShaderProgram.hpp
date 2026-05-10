#pragma once

#include "vulkan/vulkan_raii.hpp"

class ShaderProgram {
  vk::raii::ShaderModule shaderModule = nullptr;

public:
  ShaderProgram(const vk::raii::Device &device, const std::string &path);
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;

  static constexpr const char *vertEntry = "vertMain";
  static constexpr const char *fragEntry = "fragMain";

  const vk::raii::ShaderModule &getShaderModule() const { return shaderModule; }
};
