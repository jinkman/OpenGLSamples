#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform float time;

const int ITETARION = 300; // The iteration limit

float bits = 6.0;
uniform float size;
uniform vec2 offset;


// Square of complex numbers
vec2 ComplexSquare(in vec2 val)
{
    return vec2(val.x*val.x-val.y*val.y, 2.0*val.x*val.y);
}

// Get the color based on the number of iterations
vec4 getRgb(in float itr) 
{
    // Maximum number of iterations
    if(itr == ITETARION) 
        return vec4(0, 0, 0, 0);
    float r = floor(itr/(bits*bits));
    itr -= r;
    float g = floor(itr/bits);
    itr -= g;
    float b = itr;
    return vec4(r, g, b, 1.0);
}

// Return iteration number
float fractal(vec2 z)
{
    vec2 c = vec2(cos(time), sin(time))*0.7;
    for(float i = 0.0; i < ITETARION; i++) 
    {
        if(z.x*z.x+z.y*z.y > 4.0) 
            return i;
        z = ComplexSquare(z)+c;
    }
    return ITETARION;
}

void main() 
{
    vec2 resolution=vec2(1.0f);  
    vec2 position = (TexCoord-resolution/2.0)*5.0; 
    position *= size;
    position += offset;
    vec2 z     = position.xy;   
    FragColor = vec4(getRgb(fractal(position))/bits);
}