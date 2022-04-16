#version 330 core
out vec4 FragColor;

uniform sampler2D SphereMap;
in vec2 TexCoords;

void main()
{           
    FragColor = texture(SphereMap, TexCoords);
}