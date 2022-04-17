#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightColor;

out vec3 color;

void main()
{
    color = lightColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}