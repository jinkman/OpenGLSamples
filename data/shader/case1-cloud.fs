#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 uSkyColor;
uniform vec3 uCloudColor;
uniform float uCloudSize;
uniform float uTime;

float rand(vec2 a)
{
    int n = int(a.x + a.y * 57);
    n = (n<<13) ^ n;
    return sin( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float noise(vec2 n)
{
    const vec2 d = vec2(0.0, 1.0);
    vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

void main()
{    
    // noise
    float total = 0.0;
    vec2 pos = TexCoords * uCloudSize;
    total += noise(pos * 1.0 + vec2(uTime * 2.0, 0.0));
    total += noise(pos * 2.0 + vec2(0.0, uTime * 3.0)) * 0.5;
    total += noise(pos * 4.0 + vec2(uTime * 4.0, 0.0)) * 0.25;
    total += noise(pos * 8.0 + vec2(0.0, uTime * 5.0)) * 0.125;
    vec3 color = mix(uSkyColor, uCloudColor, total);
    
    FragColor = vec4(color, 1.0);
}


