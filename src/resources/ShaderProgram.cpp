#include "vulkan/vulkan_raii.hpp"
#include <fstream>
#include <resources/ShaderProgram.hpp>

namespace {
std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  std::vector<char> buffer(file.tellg());
  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), buffer.size());
  file.close();
  return buffer;
}

vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device, std::span<const char> shaderCode) {
  vk::ShaderModuleCreateInfo createInfo{.codeSize = shaderCode.size(),
                                        .pCode = reinterpret_cast<const uint32_t *>(shaderCode.data())};
  vk::raii::ShaderModule shaderModule{device, createInfo};
  return shaderModule;
}
} // namespace

ShaderProgram::ShaderProgram(const vk::raii::Device &device, const std::string &path)
    : shaderModule(createShaderModule(device, readFile("shaders/shader.spv"))) {}
