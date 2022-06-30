#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

#define texNum 17
uniform sampler2D texSrc;
uniform sampler2D charsTex;

uniform float mixFactor;
uniform vec2 resolution;

uniform float pixelSize;


vec4 filterChar()
{
    vec2 charXYNum = vec2(resolution.x / pixelSize, resolution.y / pixelSize);
    vec2 uv = (vec2(0.5) + floor(TexCoord * charXYNum)) / charXYNum;

    vec4 color = texture(texSrc, uv);
    float grey = (color.r + color.g + color.b) / 3.0;

    float index = texNum - floor(grey * float(texNum - 1) + 0.5); 

    // cal UV
    float offsetx = mod(index, 5.0) / 5.0;
    float offsety = floor(index / 5.0) / 4.0;
    vec2 offset = vec2(offsetx, offsety);

    float charCol = texture(charsTex, offset + TexCoord * charXYNum).r;
    vec2 finalUV = mod(TexCoord * charXYNum, vec2(1.0));
    charCol = texture(charsTex, finalUV / vec2(5.0, 4.0) + offset).r;

    charCol = 1.0 - charCol;
    
    return vec4(color.rgb * vec3(charCol), color.a);
}

void main() 
{
    if(TexCoord.x>mixFactor)
    {
        FragColor = filterChar();
    }
    else
    {
        FragColor = texture(texSrc, TexCoord);
    }
}