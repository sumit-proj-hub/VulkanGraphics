#pragma once

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vector>

class Window {
  GLFWwindow *window;
  bool framebufferResized = false;

  static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

public:
  Window(uint32_t initialWidth, uint32_t initialHeight, const char *windowTitle);
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  bool shouldClose() const;
  bool consumeFramebufferResized();
  static void pollEvents();
  vk::Extent2D getFramebufferSize() const;
  static std::vector<const char *> getRequiredInstanceExtensions();
  void pauseTillMinimized();
  vk::raii::SurfaceKHR createSurface(const vk::raii::Instance &instance) const;

  ~Window();
};
