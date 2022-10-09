#version 330 core
out vec4 FragColor;

#define MaxTimes        300

in vec2 TexCoord;

uniform float size;
uniform vec2 offset;

float Magnitude(vec2 a)
{
    return a.x * a.x- a.y * a.y;
}

int mandelbrot(vec2 pos)
{
    vec2 result=vec2(0.0,0.0);
    for (int i=0;i<MaxTimes;i++)
    {
        float real=pow(result.x,2)-pow(result.y,2);
        float image=2*result.x*result.y;
        result.x=real+pos.x;
        result.y=image+pos.y;
        if(length(result)>2)
            return i;
    }
    return MaxTimes;
}

void main( void )
{   
    vec2 resolution=vec2(1.0f);  
    vec2 pos=(TexCoord-resolution/2.0)*5.0; //distance from left to right is 4
    pos*=size;
    pos+=offset;
    vec3 colors[7];
    //The color order
    colors[0] = vec3(1.0,0.0,0.0);
    colors[1] = vec3(1.0,0.5,0.0);
    colors[2] = vec3(1.0,1.0,0.0);
    colors[3] = vec3(0.0,1.0,0.0);
    colors[4] = vec3(0.0,1.0,1.0);
    colors[5] = vec3(0.0,0.0,1.0);
    colors[6] = vec3(0.5,0.0,0.5);

    int times = mandelbrot(pos); 
    vec3 color;
    if(times == MaxTimes)
        color = vec3(0.0f);
    else
        color = colors[int(mod(times,7.0))];
    FragColor=vec4(color,1.0);
}