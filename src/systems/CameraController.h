#pragma once
#include "entities/EntityManager.h"
#include "platform/IInput.h"
#include <glm/glm.hpp>

class CameraController {
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float distance;
    float yaw;
    float pitch;
    float minPitch;
    float maxPitch;
    float minDistance;
    float maxDistance;
    float sensitivity;

    CameraController();

    void update(float dt);
    void handleInput(IInput& input);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjection(float aspect) const;
};
