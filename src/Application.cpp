#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Application.hpp>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

void Application::updateCamera(float aspectRatio) {
  camera.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  camera.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
  camera.proj[1][1] *= -1;
}

void Application::run() {
  auto startTime = std::chrono::high_resolution_clock::now();
  while (!window.shouldClose()) {
    window.pollEvents();
    updateCamera(renderer.getAspectRatio());

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    renderObject.transform = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    renderer.drawFrame(ctx, window, std::array{renderObject}, camera);
  }
  ctx.getDevice().waitIdle();
}
