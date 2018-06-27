#ifndef SHADER_HPP
#define SHADER_HPP


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
// #include "Display.hpp"

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
	public:
		// the program ID
		unsigned int ID;

		// constructor reads and builds the shader
		Shader(const GLchar *vertexPath, const GLchar *fragmentPath);
		// use/activate the shader
		void use();
		// utility uniform functions
		void setBool(const std::string &name, bool value) const;
		void setInt(const std::string &name, int value) const;
		void setFloat(const std::string &name, float value) const;
		void setMat4(const std::string &name, const glm::mat4 mat) const;
		void setVec2(const std::string &name, float x, float y) const;
		void setVec2(const std::string &name, const glm::vec2 &value) const;
		void setVec4(const std::string &name, const glm::vec4 &value) const;
		void setVec4(const std::string &name, float x, float y, float z, float w);


	private:
		void checkCompileErrors(unsigned int shader, std::string type);

};

#endif