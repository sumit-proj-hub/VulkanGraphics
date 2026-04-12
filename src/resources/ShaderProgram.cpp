#include "vulkan/vulkan_raii.hpp"
#include <fstream>
#include <resources/ShaderProgram.hpp>

std::vector<char> ShaderProgram::readFile(const std::string &filename) {
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

vk::raii::ShaderModule ShaderProgram::createShaderModule(const vk::raii::Device &device,
                                                         std::span<const char> shaderCode) {
  vk::ShaderModuleCreateInfo createInfo{.codeSize = shaderCode.size(),
                                        .pCode = reinterpret_cast<const uint32_t *>(shaderCode.data())};
  vk::raii::ShaderModule shaderModule{device, createInfo};
  return shaderModule;
}

vk::raii::DescriptorSetLayout ShaderProgram::createDescriptorSetLayout(const vk::raii::Device &device) {
  std::array bindings{vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1,
                                                     vk::ShaderStageFlagBits::eVertex, nullptr),
                      vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1,
                                                     vk::ShaderStageFlagBits::eFragment, nullptr)};
  vk::DescriptorSetLayoutCreateInfo layoutInfo{.bindingCount = bindings.size(), .pBindings = bindings.data()};
  return vk::raii::DescriptorSetLayout(device, layoutInfo);
}

ShaderProgram::ShaderProgram(const vk::raii::Device &device, const std::string &path)
    : shaderModule(createShaderModule(device, readFile("shaders/shader.spv"))),
      descriptorSetLayout(createDescriptorSetLayout(device)) {}
