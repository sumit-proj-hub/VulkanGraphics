#pragma once

#include <glm/matrix.hpp>

struct Camera {
    glm::mat4 view;
    glm::mat4 proj;
};
