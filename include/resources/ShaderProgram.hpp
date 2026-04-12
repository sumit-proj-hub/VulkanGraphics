#pragma once

#include "vulkan/vulkan_raii.hpp"

class ShaderProgram {
  vk::raii::ShaderModule shaderModule = nullptr;
  vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;

  static std::vector<char> readFile(const std::string &filename);
  [[nodiscard]] static vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device,
                                                                 std::span<const char> shaderCode);
  static vk::raii::DescriptorSetLayout createDescriptorSetLayout(const vk::raii::Device &device);

public:
  ShaderProgram(const vk::raii::Device &device, const std::string &path);
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;

  static constexpr const char *vertEntry = "vertMain";
  static constexpr const char *fragEntry = "fragMain";

  const vk::raii::ShaderModule &getShaderModule() const { return shaderModule; }
  const vk::raii::DescriptorSetLayout &getDescriptorSetLayout() const { return descriptorSetLayout; }
};
