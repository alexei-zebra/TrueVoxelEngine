#include "Camera.h"
#include "Window.h"

#include <glm/ext.hpp>

Camera::Camera(glm::vec3 position, float fov) : fov(fov), position(position), zoom(1.0f), rotation(1.0f) {
    updateVectors();
}

void Camera::updateVectors(){
    front = glm::vec3(rotation * glm::vec4(0,0,-1,1));
    right = glm::vec3(rotation * glm::vec4(1,0,0,1));
    up = glm::vec3(rotation * glm::vec4(0,1,0,1));
    dir = glm::vec3(rotation * glm::vec4(0,0,-1,1));
    dir.y = 0;
    float len = glm::length(dir);
    if (len > 0.0f){
        dir.x /= len;
        dir.z /= len;
    }
}

void Camera::rotate(float x, float y, float z){
    rotation = glm::rotate(rotation, z, glm::vec3(0,0,1));
    rotation = glm::rotate(rotation, y, glm::vec3(0,1,0));
    rotation = glm::rotate(rotation, x, glm::vec3(1,0,0));

    updateVectors();
}

glm::mat4 Camera::getProjection(){
    float aspect = this->aspect;
    if (aspect == 0.0f){
        aspect = (float)Window::width / (float)Window::height;
    }
    if (perspective)
        return glm::perspective(fov*zoom, aspect, 0.05f, 1500.0f);
    else
        if (flipped)
            return glm::ortho(0.0f, fov*aspect, fov, 0.0f);
        else
            return glm::ortho(0.0f, fov*aspect, 0.0f, fov);
}

glm::mat4 Camera::getView(bool pos){
    glm::vec3 position = this->position;
    if (!pos) {
        position = glm::vec3(0.0f);
    }
    if (perspective) {
        return glm::lookAt(position, position+front, up);
    } else {
        return glm::translate(glm::mat4(1.0f), position);
    }
}

glm::mat4 Camera::getProjView(bool pos){
    return getProjection()*getView(pos);
}

void Camera::setFov(float fov) {
    this->fov = fov;
}

float Camera::getFov() const {
    return fov;
}
