#include "ParticleGenerator.hpp"
// #include "stb_image.h"
#include <cstdlib>
#include <iostream>
#include <ctime>


float lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

Particle::Particle()
{
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
    GLfloat rColor = 0.8 + ((rand() % 100) / 100.0f);
    this->Position = glm::vec3(random, random, random);
    this->Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    this->Velocity = glm::vec3(random - rColor, random + rColor, random * rColor);
}

ParticleGenerator::ParticleGenerator(Shader shader, GLuint amount)
    :  amount(amount), shader(shader)
{
	this->attractor = glm::vec3(0, 0, -1);
	this->retractor = glm::vec3(0, 0, -100);
	this->gravity = 0.2;
	this->init();
}

ParticleGenerator *ParticleGenerator::getInstance(Shader shader, GLuint amount)
{
	if (_singleton == NULL)
	{
		std::cout << "Creating Particle Singleton" << '\n';
		_singleton = new ParticleGenerator(shader, amount);
	}
	else
	{
		std::cout << "Particle Singleton DUPLICATE" << '\n';
	}
	return _singleton;
}

ParticleGenerator *ParticleGenerator::getInstance()
{
	assert(_singleton != NULL);
	return _singleton;
}

void ParticleGenerator::kill()
{
	if (_singleton != NULL)
	{
		delete _singleton;
		_singleton = NULL;
	}
}

void ParticleGenerator::provokeImpulse(glm::vec3 impact, bool outwards)
{
	glm::vec3 dirImpactNormalized;
	float toImpactLength;
	for (GLuint i = 0; i < amount; ++i)
	{
		Particle &p = this->particles[i];
		// Vecteur de l'attractor a la particle
		if (outwards == true)
		{
			dirImpactNormalized = glm::normalize(impact - p.Position);
			toImpactLength = glm::clamp(glm::length(impact - p.Position), 0.0f, 2.0f) / 2;
		}
		else
		{

			dirImpactNormalized = glm::normalize(p.Position - impact);
			toImpactLength = glm::clamp(glm::length(p.Position - impact), 0.0f, 2.0f) / 2;
		}
		p.Velocity += dirImpactNormalized * (1 - toImpactLength) * (1 - toImpactLength) * 3.0f;

	}
}




void ParticleGenerator::Update(GLfloat dt)
{
	this->maxDist = 0;
	std::srand(std::time(nullptr));
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		glm::vec3 distAttractor = p.Position - attractor;
		float distToAttr = glm::clamp(glm::length(distAttractor), 0.0f, 10.0f);
		gravity = (distToAttr / 10);
		p.Velocity.x += distAttractor.x > 0 ? gravity : -gravity;
		p.Velocity.y += distAttractor.y > 0 ? gravity : -gravity;
		p.Velocity.z += distAttractor.z > 0 ? gravity : -gravity;

		glm::vec3 distRetractor = p.Position - retractor;
		float distToRetr = glm::clamp(glm::length(distRetractor), 0.0f, 1.0f);
		gravity = (1 -  distToRetr) / 10;
		p.Velocity.x += distRetractor.x < 0 ? gravity : -gravity;
		p.Velocity.y += distRetractor.y < 0 ? gravity : -gravity;
		p.Velocity.z += distRetractor.z < 0 ? gravity : -gravity;
		if (i == 400)
			std::cout << "Distance to retractor is " << distToRetr  << '\n';




		float velocityLength = glm::length(p.Velocity);
		this->maxDist = velocityLength > maxDist ? velocityLength : maxDist;
		p.Velocity *=  0.99f;
		p.Position -= p.Velocity * dt / 2.0f;

		float Ratio = (velocityLength / this->maxDist);

		p.Color.x = 1 - (Ratio* Ratio);
		p.Color.z = Ratio;
		p.Color.y = (1 - Ratio);
		p.Color.a = 1;
	}  
}

// Render all particles
void ParticleGenerator::Draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.use();
    for (Particle particle : this->particles)
    {
		this->shader.setVec3("offset", particle.Position);
		this->shader.setVec4("color", particle.Color);
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureId);
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		
}
    // Don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // Set up mesh and attribute properties
    GLuint VBO;
    float particle_quad[] = {
        // positions     // uv
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // Set mesh attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glGenTextures(0, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// glDepthMask(GL_FALSE);
	// glEnable( GL_ALPHA_TEST );
	// glAlphaFunc( GL_NOTEQUAL, 0.0 );
	// glEnable (GL_BLEND);
	// glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("textures/faggotdog.png", &width, &height, &nrChannels, 0);
	// unsigned char *data = stbi_load("textures/pixie_large.png", &width, &height, &nrChannels, 0);
	if (data)
	{
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	    glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
	    std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	this->shader.use();
 	this->shader.setInt("textureId", 0);


    // Create this->amount default particle instances
    for (GLuint i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

ParticleGenerator *ParticleGenerator::_singleton = NULL;