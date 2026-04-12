#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <GLFW/glfw3.h>
#include <core/Window.hpp>
#include <cstdint>

Window::Window(uint32_t initialWidth, uint32_t initialHeight, const char *windowTitle) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_FALSE);
  window = glfwCreateWindow(initialWidth, initialHeight, windowTitle, nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool Window::shouldClose() const { return glfwWindowShouldClose(window); }

bool Window::consumeFramebufferResized() {
  bool resized = framebufferResized;
  framebufferResized = false;
  return resized;
}

void Window::pollEvents() { glfwPollEvents(); }

vk::Extent2D Window::getFramebufferSize() const {
  int width = 0, height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

std::vector<const char *> Window::getRequiredInstanceExtensions() {
  uint32_t glfwExtensionCount = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  return extensions;
}

void Window::pauseTillMinimized() {
  int width = 0, height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(window, &width, &height);
    glfwWaitEvents();
  }
}

vk::raii::SurfaceKHR Window::createSurface(const vk::raii::Instance &instance) const {
  VkSurfaceKHR _surface;
  if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
    throw std::runtime_error("Failed to create window surface!");
  }
  return vk::raii::SurfaceKHR(instance, _surface);
}

Window::~Window() { glfwDestroyWindow(window); }

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
  auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  app->framebufferResized = true;
}
