#include "graphics/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gdp::graphics {
    void Camera::updateViewMatrix() {
        m_view = glm::lookAt(m_position, m_target, m_up);
    }

    void Camera::updateProjectionMatrix() {
        m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    }
}