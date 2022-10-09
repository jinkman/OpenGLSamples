#version 330 core
out vec4 FragColor;
in vec2 TexCoord;


uniform vec2 resolution;
uniform int octaves;
uniform float persistence;
uniform float delta;
uniform bool change;

//hash range [-1,1]
float hash2(vec2 n)
{   
    return fract(sin(dot(n, vec2(12.9898, 78.233))) * 43758.5453);
}

//Returns the number of feature points corresponding to the integer grid
int featureCount(vec2 n)
{
    float result = hash2(n) * 0.5 +0.5;
    return int(result * 6 +1);
}

vec2 rand(vec2 n)
{
    vec2 result = vec2(hash2(n.xy), hash2(n.yx));
    return result * 0.5 +0.5;
}

float noise(vec2 uv)
{
    vec2 N = floor(uv);
    float min_dis = 10000.0f;
    for(int i=-1;i<=1;i++)
    {
        for(int j=-1;j<=1;j++)
        {
            vec2 currentN = N + vec2(i,j);
            int count = featureCount(currentN);
            
            vec2 point = rand(currentN);  
            for(int k=0;k<count;k++)
            {
                float dis = length(uv - point - currentN);
                if(dis < min_dis)
                    min_dis = dis;
                point = rand(point);
            }  
        }
    }
    return min_dis;
}

void main(void) 
{
    vec2 uv = gl_FragCoord.xy / resolution.xy - 0.5;  //Standardized coordinate system
    uv = uv * resolution.xy / max(resolution.x, resolution.y);
    float total=0.0;
    if(change)
    {
        
        for(int i=0;i<octaves;++i)
        {
            float frequency = pow(2.0,i);
            float amplitude = pow(persistence,i);
            total += noise(uv*frequency*delta)*amplitude;
        }
    }
    else
        total = noise(uv*delta);
    vec3 color = vec3(total / sqrt(1));
    FragColor = vec4(color, 1.0);
}