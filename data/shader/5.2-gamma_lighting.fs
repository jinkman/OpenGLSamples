#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool ifgamma;

void main()
{   
    float gamma = 2.2;      
    vec3 color ;
    float distance = length(lightPos - fs_in.FragPos);
    float attenuation = 0;
    if(ifgamma)
    {
        color = pow(texture(floorTexture, fs_in.TexCoords).rgb, vec3(gamma));
        attenuation = 1.0 / (distance * distance); 
    }
    else
    {
        color = texture(floorTexture, fs_in.TexCoords).rgb;
        attenuation = 1.0 / distance; 
    }
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // ¾µÃæ
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
       
    //½á¹û
    vec3 result = ambient + diffuse + specular;
    if(ifgamma)
        FragColor = vec4(pow(attenuation*result, vec3(1.0/gamma)),1.0);
    else
        FragColor = vec4(attenuation*result,1.0);
}