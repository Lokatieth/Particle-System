#include "ParticleGenerator.hpp"
// #include "stb_image.h"
#include <cstdlib>
#include <iostream>
#include <ctime>

Particle::Particle()
{
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
    GLfloat rColor = 0.8 + ((rand() % 100) / 100.0f);
    this->Position = glm::vec3(random, -random, random * random) + random;
    this->Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    this->Velocity = glm::vec3(random - rColor, random + rColor, random);
}

ParticleGenerator::ParticleGenerator(Shader shader, GLuint amount)
    :  amount(amount), shader(shader)
{
	this->attractor = glm::vec3(0, 0, -1);
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

void ParticleGenerator::Update(GLfloat dt)
{
	std::srand(std::time(nullptr));
	for (GLuint i = 0; i < amount; ++i)
	{
		float rd = 1 + (float)std::rand()/((RAND_MAX + 1u)/40)/100;
		Particle &p = this->particles[i];
			p.Velocity.x += p.Position.x - attractor.x > 0 ? gravity * rd : -gravity * rd;
			p.Velocity.y += p.Position.y - attractor.y > 0 ? gravity * rd : -gravity * rd;
			p.Velocity.z += p.Position.z - attractor.z > 0 ? gravity * rd : -gravity * rd;
			p.Velocity *=  0.995f;
			p.Position -= p.Velocity * dt / 2.0f;
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
		// this->texture.Bind();
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
	unsigned char *data = stbi_load("textures/pixie_large.png", &width, &height, &nrChannels, 0);
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