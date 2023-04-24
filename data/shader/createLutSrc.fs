#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texSrc;
uniform sampler2D lutTex;

uniform float lutSize;

uniform bool bUseLut;

vec3 lut(vec3 c)
{
    vec2 rg = c.rg * vec2((lutSize - 1.0)/lutSize) + vec2(0.5/lutSize);
    float b = c.b * (lutSize - 1.0);
    float slice0=min(floor(b),(lutSize - 1.0));
    float slice1=min(slice0+1.0,(lutSize - 1.0));
    float slice_w=b-slice0;
    vec3 color0=texture(lutTex,vec2((slice0+rg.x)*(1.0/lutSize),rg.y)).xyz;
    vec3 color1=texture(lutTex,vec2((slice1+rg.x)*(1.0/lutSize),rg.y)).xyz;
    return mix(color0, color1, slice_w).rgb;
}

void main() 
{
    if(bUseLut)
    {
        vec3 color = texture(texSrc, TexCoord).rgb;
        vec3 lutColor = lut(color);
        FragColor = vec4(lutColor, 1.0);
    }
    else
    {
        FragColor = texture(texSrc, TexCoord);
    }
}