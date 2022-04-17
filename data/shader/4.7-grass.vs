#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 modelMatrices;
layout (location = 6) in vec3 speedMatrices;

out VS_OUT {
    mat4 model;
    vec3 speed;
} vs_out;




void main()
{
    vs_out.speed = speedMatrices;
    vs_out.model = modelMatrices;
    gl_Position = vec4(aPos,1.0); 
}