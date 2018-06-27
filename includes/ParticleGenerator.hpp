#ifndef PARTICLE_GENERATOR_HPP
#define PARTICLE_GENERATOR_HPP
#include <vector>
// #include "ParticleSystem.hpp"

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include "stb_image.h"
# include "Shader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;

    Particle();
};


class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, GLuint amount);
	void Update(GLfloat dt);
	void Draw();
private:
	std::vector<Particle> particles;
	GLuint amount;
	unsigned int textureId;
	Shader shader;
	GLuint VAO;
	void init();
};

#endif