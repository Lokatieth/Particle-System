#version 400
// out vec4 FragColor;

// in vec2 TexCoord;

// // texture samplers
// uniform sampler2D texture1;
// uniform sampler2D texture2;

// void main()
// {
// 	// linearly interpolate between both textures (80% container, 20% awesomeface)
// 	// FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0);
// 	FragColor = texture(texture2, TexCoord);
// }

out vec4 FragColor;
uniform float time;

in vec3 fColor;

void main()
{
    FragColor = vec4(fColor, 1.0);
}