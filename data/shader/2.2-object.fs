#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoord;  

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform float shininess;

void main()
{
    vec3 diffuseColor = texture(diffuseMap, TexCoord).rgb;
    vec3 specularColor = texture(specularMap, TexCoord).rgb;
    // 环境光照
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * diffuseColor;
  	
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * diffuseColor;
    
    // 镜面光照
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);  //shininess次幂  高光反光度
    vec3 specular = specularStrength * spec * lightColor * specularColor;  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 