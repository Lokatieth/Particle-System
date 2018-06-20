#ifndef DISPLAY_HPP
#define DISPLAY_HPP

// #include "glad/glad.h" // include glad to get all the required OpenGL headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// #include "nuklear.h"

#include <string>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Display
{
  public:
	static Display *getInstance();
	static void kill();
	void initialize();
	void clean();
	void update();

  private:
	Display();
	~Display();

	// GLuint _vbo;
	// GLuint _vao;
	int _width;
	int _height;
	GLFWwindow *window;
	static Display *_singleton;
};


#endif