#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    vec2 texCoords;
} vs_out;

void main()
{  
    gl_Position = vec4(aPos.x, 0.0f, aPos.y, 1.0f);
    vs_out.texCoords = aTexCoords; 
}