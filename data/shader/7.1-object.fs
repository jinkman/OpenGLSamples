#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;

uniform vec3 textColor;
uniform vec3 outLineColor;

void main()
{    
    vec4 color = texture(text, TexCoords);
    vec4 finalColor = vec4(textColor, 1.0) * color.r;
    FragColor = finalColor;
    // FragColor = color;
}