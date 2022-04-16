#version 330 core
out float depth;

in vec2 TexCoord;
uniform sampler2D diffuseMap;
uniform float heightScale;

void main( void ) 
{
    float depthValue;
    if(texture(diffuseMap,TexCoord).r>0.5)
        depthValue = 1.0 * heightScale;
    else
        depthValue = 0.0;
    depth = depthValue;
}