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
uniform sampler2D textureId;


in vec2 TexCoord;
in vec4 ParticleColor;

void main()
{
	// if(texColor.a < 0.1)
    //     discard;
    // FragColor = texture(textureId, TexCoord) * ParticleColor;
	vec4 texColor = texture(textureId, TexCoord);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor * ParticleColor;
    // FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}