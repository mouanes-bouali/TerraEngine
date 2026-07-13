#include "CameraSystem.h"
#include "platform/IInput.h"
#include <glm/gtc/matrix_transform.hpp>

CameraSystem::CameraSystem()
    : position(0.0f, 5.0f, 10.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , distance(10.0f)
    , yaw(-90.0f)
    , pitch(-30.0f)
    , minPitch(-89.0f)
    , maxPitch(89.0f)
    , minDistance(2.0f)
    , maxDistance(50.0f)
    , sensitivity(0.2f)
{
}

void CameraSystem::handleInput(IInput& input) {
    // Mouse drag for orbit (left button)
    if (input.isMouseButtonPressed(0)) {
        yaw   += input.getMouseDeltaX() * sensitivity;
        pitch -= input.getMouseDeltaY() * sensitivity;
    }
    
    // Mouse wheel for zoom
    distance -= input.getMouseWheelDelta() * 2.0f;
}

void CameraSystem::update(EntityManager& em, float dt) {
    // Clamp pitch
    pitch = glm::clamp(pitch, minPitch, maxPitch);
    distance = glm::clamp(distance, minDistance, maxDistance);
    
    // Convert spherical to Cartesian
    float radYaw = glm::radians(yaw);
    float radPitch = glm::radians(pitch);
    
    position.x = target.x + distance * cos(radPitch) * sin(radYaw);
    position.y = target.y + distance * sin(radPitch);
    position.z = target.z + distance * cos(radPitch) * cos(radYaw);
}

glm::mat4 CameraSystem::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 CameraSystem::getProjection(float aspect) const {
    return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
}