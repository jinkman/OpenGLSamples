#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float samples;
uniform vec3 hsvV;


vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 lutColor(in vec3 color)
{
    // 颜色相关算法
    // HSV 色调、饱和度、亮度
    // color.r *= 0.8;
    vec3 hsv = rgb2hsv(color);
    vec3 rgb = hsv2rgb(hsv + hsvV);
    return rgb;
}

float getActualUv(in float v)
{
    float onePixle = 1.0 / samples;
    return (v - 0.5 * onePixle) / max((1.0 - onePixle), 1e-6);
}

void main() 
{
    float r = getActualUv(fract(TexCoord.x * samples));
    float g = getActualUv(TexCoord.y);
    float b = floor(TexCoord.x * samples) / max((samples - 1.0f), 1e-6);
    FragColor = vec4(lutColor(vec3(r, g, b)), 1.0);
}