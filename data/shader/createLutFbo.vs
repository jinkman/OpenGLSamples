#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    vec4 pos = projection * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
    gl_Position = vec4(pos.xy, 0.0, 1.0f);
}