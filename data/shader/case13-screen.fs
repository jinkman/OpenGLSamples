#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texSrc;

uniform float mixFactor;

vec4 filterTest(in vec4 color)
{
    float grey = (color.r + color.g + color.b) / 3.0;
    return vec4(grey, grey, grey, color.a);
}

void main() 
{
    vec2 coord = TexCoord; 
    vec4 color = texture(texSrc, vec2(coord.x, 1.0 - coord.y));
    if(coord.x>mixFactor)
    {
        color = filterTest(color);
    }
    FragColor = color;
}