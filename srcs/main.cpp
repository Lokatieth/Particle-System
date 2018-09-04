// #include "ParticleSystem.hpp"
#include "Display.hpp"
#include "ClContext.hpp"
#include <iostream>
#include <vector>
#include <string>
#include "glm/glm.hpp"

// Compute c = a + b.

int main(void)
{
	Display *display;
	ClContext *clContext;
	display = Display::getInstance(); 
	clContext = ClContext::getInstance();
	display->initialize();

	// glfwSetErrorCallback(glfw_error_callback);

	display->update();
	display->clean();
	display->kill();
	clContext->kill();


	return 0;
}