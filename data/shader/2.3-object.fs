#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoord;  

uniform sampler2D diffuseMap;
uniform sampler2D specularMap; 
uniform vec3 viewPos;

struct Light
{
    vec3 position;
    vec3 color;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant; 
    float linear;
    float quadratic;
    float shininess;
};

uniform Light light;



void main()
{
   // 环境光
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * texture(diffuseMap, TexCoord).rgb;
    
    // 漫反射 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffuseMap, TexCoord).rgb;  
    
    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.shininess);
    vec3 specular = light.color * spec * texture(specularMap, TexCoord).rgb;  
    
    // 平滑/软化边缘
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // 光照衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 