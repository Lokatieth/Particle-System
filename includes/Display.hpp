#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <string>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>
#include "ParticleSystem.hpp"
#include "Camera.hpp"

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
	Camera *_camera;
	int _width;
	int _height;
	float _deltaTime;
	float _lastFrame;
	GLFWwindow *window;
	static Display *_singleton;
};


#endif