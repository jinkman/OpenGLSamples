#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;
in vec3 FragPos; 

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform bool ifNormal;

uniform bool normalMapping;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{   
    vec3 normal; 
    vec3 lightDir; 
    vec3 viewDir; 
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb; 
    //  环境光
    vec3 ambient = 0.1 * color;
    
     
    if(ifNormal)
    {
	 normal = texture(normalMap, fs_in.TexCoords).rgb;
         normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
         lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
         viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    }
    
   else
    {
	normal = fs_in.Normal;
        lightDir = normalize(lightPos - FragPos);
        viewDir = normalize(viewPos - FragPos);
    }
	 
    // 漫反射
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    
    FragColor = vec4(ambient + diffuse + spec , 1.0f);
}