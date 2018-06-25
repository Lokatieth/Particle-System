#version 400
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoord;

// out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lol;
// uniform mat4 transform;

// void main()
// {
// 	gl_Position = projection * view * model * vec4(aPos, 1.0);
// 	//gl_Position = transform * projection * view * model * vec4(aPos, 1.0);
// 	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
// }

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fColor;

void main()
{
    gl_Position =  lol * projection * view * model * vec4(aPos, 0.0, 0.5);
    fColor = aColor;
}