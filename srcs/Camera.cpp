#include "Display.hpp"

Camera::Camera() : pos(glm::vec3(0, 0, -3)), front(glm::vec3(0, 0, 1)), up(glm::vec3(0.0, 1.0, 0.0))
{
	// cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 0.0f), cameraDirection));
	// view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
  	// 	   glm::vec3(0.0f, 0.0f, 0.0f), 
  	// 	   glm::vec3(0.0f, 1.0f, 0.0f));
	lastX = 840;
	lastY = 630;
	firstMouse = true;
}

Camera::~Camera()
{
}


Camera *Camera::getInstance()
{
	if (_camelton == NULL)
	{
		std::cout << "creating camera singleton" << '\n';
		_camelton = new Camera();
	}
	else
	{
		// std::cout << "Camera singleton already created, passing pointer to it" << '\n';
	}
	return _camelton;

}

void Camera::kill()
{
	if (_camelton != NULL)
	{
		delete _camelton;
		_camelton = NULL;
	}
}


Camera *Camera::_camelton = NULL;