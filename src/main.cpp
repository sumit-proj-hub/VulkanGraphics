#include "Application.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
  glfwInit();

  try {
    Application app;
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
