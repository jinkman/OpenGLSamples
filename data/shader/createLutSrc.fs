#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texSrc;
uniform sampler2D lutTex;

uniform float lutSize;
uniform bool bUseLut;
uniform float sharpenV;
uniform float particleV;
uniform float fadeV;
uniform float cornerV;
uniform vec2 resolution;

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

vec3 getColorByUv(vec2 uv) {
    vec3 color = texture(texSrc, uv).rgb;
    color = lut(color);
    return color;
}

const float PI = 3.1415926;

const float PHI = 1.61803398874989484820459; // Φ = Golden Ratio 

float gold_noise(in vec2 xy) {
    return fract(tan(distance(xy*PHI, xy)) * xy.x);
}

vec3 postProcess()
{
    vec3 color = getColorByUv(TexCoord);
    if(sharpenV != 0.0) { // 锐化
        float centerMultiplier = 1.0 + 4.0 * sharpenV;
        float edgeMultiplier = sharpenV;
        vec2 imageWHFactor = vec2(1.0) / resolution;
    
        vec2 widthStep = vec2(imageWHFactor.x, 0.0);
        vec2 heightStep = vec2(0.0, imageWHFactor.y);
        vec2 leftTexCoord = TexCoord - widthStep;
        vec2 rightTexCoord = TexCoord + widthStep;
        vec2 topTexCoord = TexCoord + heightStep;
        vec2 bottomTexCoord = TexCoord - heightStep;
    
        vec3 textureColor = color;
        vec3 leftTextureColor = getColorByUv(leftTexCoord);
        vec3 rightTextureColor = getColorByUv(rightTexCoord);
        vec3 topTextureColor = getColorByUv(topTexCoord);
        vec3 bottomTextureColor = getColorByUv(bottomTexCoord);
    
        color = vec3(textureColor * centerMultiplier -
                                (leftTextureColor * edgeMultiplier
                                 + rightTextureColor * edgeMultiplier
                                 + topTextureColor * edgeMultiplier
                                 + bottomTextureColor * edgeMultiplier)
                );
    }
    
    if(fadeV != 0.0) { // 褪色
        float gray = dot(color, vec3(0.299, 0.587, 0.114)); // 将 RGB 转为灰度
        color = mix(color, vec3(gray), fadeV); // 将灰度和原色混合
    }
    
    if(particleV != 0.0) { // 颗粒
        float randV = gold_noise(TexCoord * resolution);
        vec3 mixColor = vec3(randV);
        color = mix(color, mixColor, particleV * 0.15);
    }
    

    if(cornerV != 0.0) { // 暗角
        vec2 coord = TexCoord * vec2(2.0) - vec2(1.0);
        float ratio = resolution.x / resolution.y;
        coord = ratio < 1.0 ? coord * vec2(ratio, 1.0) : coord * vec2(1.0, 1.0 / ratio);
        float maxLength = ratio > 1.0 ? sqrt(1 + 1.0 / (ratio * ratio)) : sqrt(1 + ratio * ratio);
        float dis = length(coord);
        // 符号
        float signV = sign(cornerV);
        float shoulderV = maxLength * (1.0 - abs(cornerV));
        if(dis > shoulderV) {
            vec3 mixColor = signV < 0.0 ? vec3(1.0) : vec3(0.0);
            float sinV = sin(clamp((dis - shoulderV), 0.0, 1.0) * PI * 0.5); // 0-1
            float mixV = clamp(sinV * 1.5, 0.0, 1.0);
            color = mix(color, mixColor, mixV);
        }
    }
    
    return color;
}

void main() 
{
    if(bUseLut)
    {
        vec3 color = postProcess();
        FragColor = vec4(color, 1.0);
    }
    else
    {
        FragColor = texture(texSrc, TexCoord);
    }
}