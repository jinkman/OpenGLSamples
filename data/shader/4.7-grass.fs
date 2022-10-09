#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D diffuseMap;
uniform vec4 grassColor;

void main()
{
    vec4 texColor = texture(diffuseMap, TexCoords);
    if(texColor.a < 0.001)
        discard;
    FragColor = mix(texColor,grassColor,0.4);  
}


