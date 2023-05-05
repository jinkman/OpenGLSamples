#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float samples;
uniform vec3 hsvV;
uniform float temperatureV;
uniform float contrastV;
uniform float highLightV;
uniform float shadowV;

const float EPSILON = 1e-10;


vec3 RGBtoHCV(in vec3 rgb)
{
    // RGB [0..1] to Hue-Chroma-Value [0..1]
    // Based on work by Sam Hocevar and Emil Persson
    vec4 p = (rgb.g < rgb.b) ? vec4(rgb.bg, -1., 2. / 3.) : vec4(rgb.gb, 0., -1. / 3.);
    vec4 q = (rgb.r < p.x) ? vec4(p.xyw, rgb.r) : vec4(rgb.r, p.yzx);
    float c = q.x - min(q.w, q.y);
    float h = abs((q.w - q.y) / (6. * c + EPSILON) + q.z);
    return vec3(h, c, q.x);
}

vec3 RGBtoHSV(in vec3 rgb)
{
    // RGB [0..1] to Hue-Saturation-Value [0..1]
    vec3 hcv = RGBtoHCV(rgb);
    float s = hcv.y / (hcv.z + EPSILON);
    return vec3(hcv.x, s, hcv.z);
}

vec3 HUEtoRGB(in float hue)
{
    // Hue [0..1] to RGB [0..1]
    // See http://www.chilliant.com/rgb2hsv.html
    vec3 rgb = abs(hue * 6. - vec3(3, 2, 4)) * vec3(1, -1, -1) + vec3(-1, 2, 2);
    return clamp(rgb, 0., 1.);
}


vec3 HSVtoRGB(in vec3 hsv)
{
    // Hue-Saturation-Value [0..1] to RGB [0..1]
    vec3 rgb = HUEtoRGB(hsv.x);
    return ((rgb - 1.) * hsv.y + 1.) * hsv.z;
}

const float LuminancePreservationFactor = 1.0;

const float PI2 = 6.2831853071;

// Valid from 1000 to 40000 K (and additionally 0 for pure full white)
vec3 colorTemperatureToRGB(const in float temperature){
  // Values from: http://blenderartists.org/forum/showthread.php?270332-OSL-Goodness&p=2268693&viewfull=1#post2268693   
  mat3 m = (temperature <= 6500.0) ? mat3(vec3(0.0, -2902.1955373783176, -8257.7997278925690),
	                                      vec3(0.0, 1669.5803561666639, 2575.2827530017594),
	                                      vec3(1.0, 1.3302673723350029, 1.8993753891711275)) : 
	 								 mat3(vec3(1745.0425298314172, 1216.6168361476490, -8257.7997278925690),
   	                                      vec3(-2666.3474220535695, -2173.1012343082230, 2575.2827530017594),
	                                      vec3(0.55995389139931482, 0.70381203140554553, 1.8993753891711275)); 
  return mix(clamp(vec3(m[0] / (vec3(clamp(temperature, 1000.0, 40000.0)) + m[1]) + m[2]), vec3(0.0), vec3(1.0)), vec3(1.0), smoothstep(1000.0, 0.0, temperature));
}

vec3 contrastAdjust( in vec3 color, in float c) {
    float t = 0.5 - c * 0.5; 
    return color * vec3(c) + vec3(t);
}

float luminance(vec3 color) {
    float fmin = min(min(color.r, color.g), color.b);
    float fmax = max(max(color.r, color.g), color.b);
    return (fmax + fmin) / 2.0;
}

// vec3 highLight(vec3 color, float progress) {
//     // float lumin = luminance(color);
//     const vec3 luminanceWeighting1 = vec3(0.3, 0.3, 0.3);
//     float lumin = dot(color, luminanceWeighting1);
//     float shadowLevel = 0.0;
//     float shadow = clamp((pow(lumin, 1.0 / (shadowLevel + 1.0)) + (-0.76) * pow(lumin, 2.0 / (shadowLevel + 1.0))) - lumin, 0.0, 1.0);
//     float highlight = clamp((1.0 - (pow(1.0 - lumin, 1.0 / (2.0 - progress)) + (-0.8) * pow(1.0 - lumin, 2.0 / (2.0 - progress)))) - lumin, -1.0, 0.0);
//     float divider = max(lumin - 0.0, 1e-6);
//     vec3 rgb = vec3(0.0, 0.0, 0.0) + ((lumin + shadow + highlight) - 0.0) * ((color - vec3(0.0,0.0, 0.0)) / divider);
//     rgb = clamp(rgb, 0.0, 1.0);
//     return rgb;
// }

// vec3 highLight(vec3 color, float progress) {
//     // 曲线编辑：x+(pow(x,0.5)-x)*pow(1.0-x,2.0)  https://zh.numberempire.com/graphingcalculator.php?functions=&xmin=0&xmax=3&ymin=-1.0&ymax=1.0&var=x
//     float lumin = clamp(luminance(color), 0.0, 1.0);
//     float maxV = 20.0;
//     float u = progress * (maxV - 1.0); // -1,1
//     u = u > 0.0 ? 1.0 / (1.0 + u) : 1.0 - u;
//     float extraLumin = (pow(lumin, u) - lumin) * pow(1.0 - lumin, 2.0);
//     return color * (1.0 + extraLumin);
// }

vec3 highLight(vec3 color, float progress) {
    // 曲线编辑：x+(0.5)*pow(1.0-x,2.0)  https://zh.numberempire.com/graphingcalculator.php?functions=&xmin=0&xmax=3&ymin=-1.0&ymax=1.0&var=x
    float lumin = clamp(luminance(color), 0.0, 1.0);
    float maxV = 0.3;
    float extraLumin = maxV * progress * pow(lumin, 4.0);
    return color * (1.0 + extraLumin);
}

vec3 shadow(vec3 color, float progress) {
    // 曲线编辑：x+(0.5)*pow(1.0-x,2.0)  https://zh.numberempire.com/graphingcalculator.php?functions=&xmin=0&xmax=3&ymin=-1.0&ymax=1.0&var=x
    float lumin = clamp(luminance(color), 0.0, 1.0);
    float maxV = 1.5;
    float extraLumin = maxV * progress * pow(1.0 - lumin, 4.0);
    return color * (1.0 + extraLumin);
}

vec3 lutColor(in vec3 color)
{
    // 色温
    vec3 outColor = mix(color, color * colorTemperatureToRGB(temperatureV), 0.8); 
    // HSV 色调、饱和度、亮度
    vec3 hsv = RGBtoHSV(outColor);
    vec3 rgb = HSVtoRGB(hsv + hsvV);
    // 对比度
    rgb = contrastAdjust(rgb, contrastV + 1.0);
    // 高亮
    rgb = highLight(rgb, highLightV);
    // 阴影
    rgb = shadow(rgb, shadowV);
    
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