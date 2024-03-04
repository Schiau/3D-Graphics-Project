#include "Camera.hpp"
#include <glm/glm.hpp>
namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
        this->cameraUpDirection = glm::cross(cameraFrontDirection, cameraRightDirection);
        rotate(0.0f, 0.0f);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        glm::vec3 aux = glm::vec3(0);
        if (direction == MOVE_RIGHT)
            aux += glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;;
        if (direction == MOVE_LEFT)
            aux -= glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection)) * speed;
        if (direction == MOVE_FORWARD)
            aux += speed * cameraFrontDirection;
        if (direction == MOVE_BACKWARD)
            aux -= speed * cameraFrontDirection;
        cameraPosition += aux;
        cameraTarget += aux;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float yaw, float pitch) {

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraTarget = cameraPosition + direction;
        cameraFrontDirection = glm::normalize(direction);
    }


    //move camera up
    void Camera::up()
    {
        cameraPosition += glm::vec3(0, 0.5f, 0);
        cameraTarget += glm::vec3(0, 0.5f, 0);
    }
    //move camera down
    void Camera::down()
    {
        cameraPosition -= glm::vec3(0, 0.5f, 0);
        cameraTarget -= glm::vec3(0, 0.5f, 0);
    }
    //get camera Position
    glm::vec3 Camera::getPostion()
    {
        return cameraPosition;
    }
    //get camera target
    glm::vec3 Camera::getTarget()
    {
        return cameraTarget;
    }
    //set camera Position
    void Camera::setPostion(glm::vec3 position)
    {
        cameraPosition = position;
    }
    //set camera target
    void Camera::setTarget(glm::vec3 position)
    {
        cameraTarget = position;
    }
}
