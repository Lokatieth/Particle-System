#include "Display.hpp"
#include "Shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "FileSystem.hpp"


void processInput(GLFWwindow *window, Camera *camera, float delta)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
		 float cameraSpeed = 2 * delta; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->pos += cameraSpeed * camera->front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->pos -= cameraSpeed * camera->front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->pos -= glm::normalize(glm::cross(camera->front, camera->up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->pos += glm::normalize(glm::cross(camera->front, camera->up)) * cameraSpeed;
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
        cam->lastX = xpos;
        cam->lastY = ypos;
        cam->firstMouse = false;
    }
	std::cout << "xpos = [" << xpos << "] and ypos = [" << ypos << "]\n";

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
	Shader ourShader("srcs/shaders/vs.glsl", "srcs/shaders/fs.glsl");

	unsigned int amount = 10000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed	
    float radius = 5.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
      	glm::mat4 model = glm::mat4(1);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 1.0f, 1.0f,
         0.05f, -0.05f,  1.0f, 1.0f, 1.0f,
        -0.05f, -0.05f,  1.0f, 1.0f, 1.0f,

        // -0.05f,  0.05f,  1.0f, 1.0f, 1.0f,
        //  0.05f, -0.05f,  1.0f, 1.0f, 1.0f,
        //  0.05f,  0.05f,  1.0f, 1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    // also set instance data

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;		
		processInput(window, _camera, _deltaTime);

		// glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->_camera->view =  glm::lookAt(this->_camera->pos, this->_camera->pos + this->_camera->front, this->_camera->up);
		glm::mat4 projection = glm::mat4(1);
		glm::mat4 model = glm::mat4(1);
		projection = glm::perspective(glm::radians(80.0f), (float)_width / (float)_height, 0.1f, 100.0f);
		
		
		ourShader.use();
		// unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glm::mat4 lol = glm::mat4(1);

		ourShader.setMat4("view", this->_camera->view);
		ourShader.setMat4("model", model);
		ourShader.setMat4("projection", projection);
		for (unsigned int i = 0; i < amount; i++)
        {
			glm::rotate(modelMatrices[i], (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
			ourShader.setMat4("lol", lol);
            ourShader.setMat4("model", modelMatrices[i]);
			glBindVertexArray(quadVAO);
			// glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
        }     


		
		// glDrawArrays(GL_TRIANGLES, 0, 4);
		//  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

// void Display::update()
// {
// 	this->_camera = Camera::getInstance();
// 	Shader ourShader("srcs/shaders/vs.glsl", "srcs/shaders/fs.glsl");

// 	float vertices[] = {
//         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//          0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

//         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

//         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
//         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
//     };
//     // world space positions of our cubes
//     glm::vec3 cubePositions[] = {
//         glm::vec3( 0.0f,  0.1f,  0.0f),
//         glm::vec3( 2.0f,  5.0f, -15.0f),
//         glm::vec3(-1.5f, -2.2f, -2.5f),
//         glm::vec3(-3.8f, -2.0f, -12.3f),
//         glm::vec3( 2.4f, -0.4f, -3.5f),
//         glm::vec3(-1.7f,  3.0f, -7.5f),
//         glm::vec3( 1.3f, -2.0f, -2.5f),
//         glm::vec3( 1.5f,  2.0f, -2.5f),
//         glm::vec3( 1.5f,  0.2f, -1.5f),
//         glm::vec3(-1.3f,  1.0f, -1.5f)
//     };
//     unsigned int VBO, VAO;//, EBO;
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//     // glGenBuffers(1, &EBO);

//     glBindVertexArray(VAO);

//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//     // position attribute
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);
//     // texture coord attribute
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);

//  	unsigned int texture1, texture2;
//     // texture 1
//     // ---------
//     glGenTextures(1, &texture1);
//     glBindTexture(GL_TEXTURE_2D, texture1);
//     // set the texture wrapping parameters
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     // set texture filtering parameters
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     // load image, create texture and generate mipmaps
//     int width, height, nrChannels;
//     stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
//     unsigned char *data = stbi_load("textures/faggotdog.png", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "Failed to load texture" << std::endl;
//     }
//     stbi_image_free(data);
//     // texture 2
//     // ---------
//     glGenTextures(1, &texture2);
//     glBindTexture(GL_TEXTURE_2D, texture2);
//     // set the texture wrapping parameters
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     // set texture filtering parameters
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     // load image, create texture and generate mipmaps
//     data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "Failed to load texture" << std::endl;
//     }
//     stbi_image_free(data);

//     // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
//     // -------------------------------------------------------------------------------------------
//     ourShader.use(); 
//     ourShader.setInt("texture1", 0);
//     ourShader.setInt("texture2", 1);

// 	while (!glfwWindowShouldClose(window))
// 	{
// 		float currentFrame = glfwGetTime();
//         _deltaTime = currentFrame - _lastFrame;
//         _lastFrame = currentFrame;		
// 		processInput(window, _camera, _deltaTime);

// 		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 		glActiveTexture(GL_TEXTURE0);
//         glBindTexture(GL_TEXTURE_2D, texture1);
//         glActiveTexture(GL_TEXTURE1);
//         glBindTexture(GL_TEXTURE_2D, texture2);


// 		this->_camera->view =  glm::lookAt(this->_camera->pos, this->_camera->pos + this->_camera->front, this->_camera->up);
// 		glm::mat4 projection = glm::mat4(1);
// 		projection = glm::perspective(glm::radians(80.0f), (float)_width / (float)_height, 0.1f, 100.0f);
		
		
// 		ourShader.use();
// 		// unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
// 		// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
// 		ourShader.setMat4("view", this->_camera->view);
// 		// ourShader.setMat4("model", model);
// 		ourShader.setMat4("projection", projection);

// 		glBindVertexArray(VAO);
// 		for (unsigned int i = 0; i < 10; i++)
//         {
//             // calculate the model matrix for each object and pass it to shader before drawing
//             glm::mat4 model = glm::mat4(1);
//             model = glm::translate(model, cubePositions[i]);
//             float angle = 20.0f * i;
//             model = glm::rotate(model, currentFrame * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//             ourShader.setMat4("model", model);

//             glDrawArrays(GL_TRIANGLES, 0, 36);
//         }


		
// 		// glDrawArrays(GL_TRIANGLES, 0, 4);
// 		//  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
// 		glfwPollEvents();
// 		glfwSwapBuffers(window);
// 	}
// }

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
	glfwGetWindowSize(window, &_width, &_height);
	glfwSetWindowAspectRatio(window, _width, _height);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

Display *Display::_singleton = NULL;
