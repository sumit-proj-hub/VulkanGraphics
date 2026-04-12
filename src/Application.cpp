#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "render/SceneUBO.hpp"
#include <Application.hpp>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

void Application::updateScene(SceneUBO &ubo, float aspectRatio) {
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float>(currentTime - startTime).count();

  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
  ubo.proj[1][1] *= -1;
}

void Application::run() {
  while (!window.shouldClose()) {
    window.pollEvents();
    renderer.drawFrame(ctx, window, mesh, [](SceneUBO &ubo, float aspectRatio) { updateScene(ubo, aspectRatio); });
  }
  ctx.getDevice().waitIdle();
}
