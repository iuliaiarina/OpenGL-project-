#include "Camera.hpp"
#include <iostream>
namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        
        //TODO - Update the rest of camera parameters

    }

    
    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraTarget, cameraUpDirection);
    }

    glm::vec3 Camera::getPosition() {
        glm::vec3 pos = this->cameraPosition;
        return pos;
    }

    glm::vec3 cameraForward, cameraRight;
    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        // transfer in centru:
        //axele camerei + UPdirection
        cameraForward = normalize(this->cameraTarget - this->cameraPosition);
        cameraRight = normalize(cross(cameraForward, this->cameraUpDirection));
        switch (direction) {
        case MOVE_FORWARD:
            this->cameraPosition += this->cameraTarget * speed;
            //this->cameraTarget += cameraForward *speed;
            break;
        case MOVE_BACKWARD:
            this->cameraPosition -= this->cameraTarget * speed;
            //this->cameraTarget -= cameraForward * speed;
            break;
        case MOVE_LEFT:
            this->cameraPosition -= glm::normalize(glm::cross(cameraTarget, this->cameraUpDirection)) * speed;
            break;
        case MOVE_RIGHT:
            this->cameraPosition += glm::normalize(glm::cross(cameraTarget, this->cameraUpDirection)) * speed;
            break;
        case MOVE_UP:
            this->cameraPosition.y += speed;
            break;
        case MOVE_DOWN:
            this->cameraPosition.y -= speed;
            break;
        default:
            this->cameraPosition = this->cameraPosition ;
        }
    }

    void Camera::move(glm::vec3 position) {
        this->cameraPosition = position;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //printf("%lf %lf \n", pitch, yaw);
        glm::vec3 aux;
        aux.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        aux.y = sin(glm::radians(pitch));
        aux.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraTarget = glm::normalize(aux);

    }
}