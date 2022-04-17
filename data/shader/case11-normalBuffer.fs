#version 330 core
out vec4 FragColor;

#define HeightMapSize vec2(400,400)

in vec2 TexCoord;
uniform sampler2D depthMap;

float getDepth(vec2 texCoords)
{
    float depth = texture(depthMap,texCoords).r;
    return 1 - depth;
}

void main( void ) 
{
    vec2 off = 1.0 / HeightMapSize;
    float Scale = 1;   // Sample teh neighbor   
    float s0 = getDepth(TexCoord + vec2(-off.x,0));   
    float s1 = getDepth(TexCoord + vec2( off.x,0));   
    float s2 = getDepth(TexCoord + vec2( 0,-off.y));   
    float s3 = getDepth(TexCoord + vec2(0,off.y));   
    vec3 U = vec3(1,0,s1 - s0);   
    vec3 V = vec3(0,1,s3 - s2);   
    vec3 normal = normalize(Scale * cross(U,V));   // Pack [-1, 1] into [0, 1]   	
    FragColor = vec4(normal * 0.5 + 0.5,1);
    //FragColor = vec4(1.0);
}