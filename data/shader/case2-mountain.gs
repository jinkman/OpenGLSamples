#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];

uniform sampler2D perlinMap;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float heightSize;
uniform vec3 cameraPos;
out vec2 texCoords;

void perform(vec4 a, vec2 b)
{
    float y = (texture(perlinMap, b).r-0.5) * heightSize;
    vec4 position = a;
    position.y = y;
    position = model * position;
    texCoords = b;
    gl_Position = projection * view * position;
    EmitVertex();
}

void main() 
{   
    for(int i=0;i<3;i++)
        perform(gl_in[i].gl_Position,gs_in[i].texCoords);
    EndPrimitive();
} 



    
