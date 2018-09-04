#ifndef PARTICLE_GENERATOR_HPP
#define PARTICLE_GENERATOR_HPP
// #include "ParticleSystem.hpp"
#include <vector>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include "stb_image.h"
# include "Shader.hpp"
# include <GL/glew.h>
# include <GLFW/glfw3.h>
# include "ClContext.hpp"

struct Particle {
    glm::vec3 Position;
	glm::vec3 Velocity;
    glm::vec4 Color;
};

// typedef struct __attribute__ ((packed)) _Cl_Particle
// {
//     cl_float3 Position;
// 	cl_float3 Velocity;
//     cl_float4 Color;

// } ClParticle;

class ParticleGenerator
{
public:
	static ParticleGenerator *getInstance(Shader shader, GLuint amount);
	static ParticleGenerator *getInstance();
	static void kill();
	void provokeImpulse(glm::vec3 impact, bool outwards);
	void Update(GLfloat dt);
	void Draw();
	void stop();
	void resume();
	glm::vec3 attractor;
	cl_float3 retractor;
	float maxDist;
	float gravity;

private:
	// ParticleGenerator();
	// ~ParticleGenerator();
  	ParticleGenerator(Shader shader, GLuint amount);
	std::vector<Particle> particles;
	GLuint amount;
	unsigned int textureId;
	Shader shader;
	GLuint VAO;
	void init();
	static ParticleGenerator *_singleton;
};

#endif