#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

//uniform mat4 projection;
//uniform mat4 model;

out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    //vec4 pos = projection * model * vec4(aPos, 0.0, 1.0);
    //gl_Position = vec4(pos.xy, 0.0, 1.0f);
    gl_Position = vec4(aPos.xy, 0.0, 1.0f);
}