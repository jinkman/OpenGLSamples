#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 TexCoord;
} gs_in[];

out vec2 TexCoord;
out vec3 tNormal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


vec3 GetNormal()
{
    vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() 
{    
    vec3 normal = mat3(transpose(inverse(model))) * GetNormal();
    for(int i=0;i<3;i++)
    {
        gl_Position = projection * view * model * gl_in[i].gl_Position;
        FragPos = gl_Position.xyz;
        TexCoord = gs_in[i].TexCoord;
        tNormal = normal;	
        EmitVertex();
    }		
    EndPrimitive();
}