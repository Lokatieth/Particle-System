#version 400
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoord;

// out vec2 TexCoord;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 ParticleColor;


//MVP
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Couleur et position des particules
uniform vec3 offset;
uniform vec4 color;	


// Pour faire le billboarding
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;

void main()
{
	vec3 fin = CameraRight_worldspace * aPos.x + CameraUp_worldspace * aPos.y;


	float scale = 0.01f;
	gl_Position = projection * view * model * vec4(fin * scale + offset, 1.0);
	ParticleColor = color;
	//gl_Position = transform * projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
} 