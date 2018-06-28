#include "Display.hpp"
#include "Shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void processInput(GLFWwindow *window, Camera *camera, float delta)
{
	float modifier = 1.0f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		modifier = 10.0f;
		std::cout << "voooom" << '\n';
	}
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		modifier = 1.0f;
		std::cout << "hiiii" << '\n';
	}
	float cameraSpeed = 2 * delta * modifier; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->pos += cameraSpeed * camera->front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->pos -= cameraSpeed * camera->front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->pos -= glm::normalize(glm::cross(camera->front, camera->up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->pos += glm::normalize(glm::cross(camera->front, camera->up)) * cameraSpeed;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	(void)mods;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		Camera *cam;
		cam = Camera::getInstance();
		ParticleGenerator *gen;
		gen = ParticleGenerator::getInstance();
		std::cout << "So far so good" << '\n';
		gen->attractor = glm::vec3(cam->pos + cam->front );
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// glfwGetCursorPos(window, &xp	os, &ypos);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	 
	Camera *cam;
	cam = Camera::getInstance();

	// (void)xpos;
	// (void)ypos;
	// (void)window;
    if (cam->firstMouse)
    {
		glfwSetCursorPos(window, 0, 0);
		cam->lastX = xpos;
        cam->lastY = ypos;
        cam->firstMouse = false;
    }
	// std::cout << "xpos = [" << xpos << "] and ypos = [" << ypos << "]\n";

    float xoffset = xpos - cam->lastX;
    float yoffset = cam->lastY - ypos; // reversed since y-coordinates go from bottom to top
    cam->lastX = xpos;
    cam->lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cam->yaw += xoffset;
    cam->pitch += yoffset;

    // make sure that when cam->pitch is out of bounds, screen doesn't get flipped
    if (cam->pitch > 89.0f)
        cam->pitch = 89.0f;
    if (cam->pitch < -89.0f)
        cam->pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    front.y = sin(glm::radians(cam->pitch));
    front.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    cam->front = glm::normalize(front);
}



Display::Display()
{
	std::cout << "Display constructor" << '\n';
	_deltaTime = 0.0f;
	_lastFrame = 0.0f;
}

Display::~Display()
{
	std::cout << "Display destructor" << '\n';
}

Display *Display::getInstance()
{
	if (_singleton == NULL)
	{
		std::cout << "creating signelton" << '\n';
		_singleton = new Display();
	}
	else
	{
		std::cout << "singleton already created" << '\n';
	}
	return _singleton;

}

void Display::kill()
{
	if (_singleton != NULL)
	{
		delete _singleton;
		_singleton = NULL;
	}
}

void Display::update()
{
	this->_camera = Camera::getInstance();
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader ourShader("srcs/shaders/vs.glsl", "srcs/shaders/fs.glsl");

	this->_particleGenerator = ParticleGenerator::getInstance(ourShader, 15000);
	// ParticleGenerator generator(ourShader, 5000);
    srand(glfwGetTime()); // initialize random seed	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;		
		processInput(window, _camera, _deltaTime);

		// glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// glClearColor(0.3f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->_particleGenerator->Update(_deltaTime);
		this->_particleGenerator->Draw();
		this->_camera->view =  glm::lookAt(this->_camera->pos, this->_camera->pos +	this->_camera->front, this->_camera->up);
		glm::mat4 projection = glm::mat4(1);
		glm::mat4 model = glm::mat4(1);
		projection = glm::perspective(glm::radians(80.0f), (float)_width / (float)_height, 0.1f, 100.0f);
		
		
		ourShader.use();
		// unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		// glm::mat4 lol = glm::mat4(1);
		ourShader.setVec3("CameraRight_worldspace", this->_camera->view[0][0], this->_camera->view[1][0], this->_camera->view[2][0]);
		ourShader.setVec3("CameraUp_worldspace", this->_camera->view[0][1], this->_camera->view[1][1], this->_camera->view[2][1]);
		ourShader.setMat4("view", this->_camera->view);
		ourShader.setMat4("model", model);
		ourShader.setMat4("projection", projection);
		// for (unsigned int i = 0; i < amount; i++)
        // {
		// 	glm::rotate(modelMatrices[i], (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
		// 	ourShader.setMat4("lol", lol);
        //     ourShader.setMat4("model", modelMatrices[i]);
		// 	glBindVertexArray(quadVAO);
		// 	// glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
		// 	glDrawArrays(GL_TRIANGLES, 0, 6);
		// 	glBindVertexArray(0);
        // }     


		
		// glDrawArrays(GL_TRIANGLES, 0, 4);
		//  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

void Display::clean()
{
	glfwTerminate();
}

void Display::initialize()
{
	_height = 1260;
	_width = 1680;

	if (!glfwInit())
		return;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(_width, _height, "Particle System", NULL, NULL);
	glfwSetWindowPos(window, 700, 100);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwGetWindowSize(window, &_width, &_height);
	glfwSetWindowAspectRatio(window, _width, _height);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glDepthFunc(GL_LESS);
}

Display *Display::_singleton = NULL;
