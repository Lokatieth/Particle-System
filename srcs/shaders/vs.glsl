#version 400
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoord;

// out vec2 TexCoord;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 ParticleColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 offset;
uniform vec4 color;	

void main()
{
	float scale = 0.1f;
	gl_Position = projection * view * model * vec4(aPos * scale + vec3(offset, 1.0), 1.0);
	ParticleColor = color;
	//gl_Position = transform * projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
} 