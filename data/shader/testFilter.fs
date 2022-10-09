#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texSrc;
uniform sampler3D lutTex;
uniform int lutSize;


uniform float mixFactor;

vec4 testFilter(in vec4 color)
{
    // Compute the 3D LUT lookup scale/offset factor
    vec3 scale = vec3((lutSize - 1.0) / lutSize);
    vec3 offset = vec3(1.0 / (2.0 * lutSize));

    // ****** Apply 3D LUT color transform! **************
    // This is our dependent texture read; The 3D texture's
    // lookup coordinates are dependent on the
    // previous texture read's result

    vec3 applyLut = texture(lutTex, scale * color.rgb + offset).rgb;
    return vec4(applyLut, color.a);
}

void main() 
{
    vec4 color = texture(texSrc, TexCoord);
    if(TexCoord.x>mixFactor)
    {
        color = testFilter(color);
    }
    FragColor = color;
}