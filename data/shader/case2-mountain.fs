#version 330 core
out vec4 FragColor;

in vec2  texCoords; 
uniform sampler2D diffuseMap;
uniform sampler2D diffuseMap1;
uniform sampler2D perlinMap;

void main()
{    
    float total = (texture(perlinMap, texCoords).r-0.5)*2;
    vec3 color1 = texture(diffuseMap, texCoords*30).rgb;
    vec3 color2 = texture(diffuseMap1, texCoords*40).rgb;
    color2.x *= 0.88f;
    total = clamp(total*1.7,0.0,1.0);
    if(texCoords.x<0.005||texCoords.y<0.005)
       discard;
    FragColor = vec4(mix(color1,color2, total),1.0f); 
}