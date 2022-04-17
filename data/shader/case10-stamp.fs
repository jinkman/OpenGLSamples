#version 330 core
out vec4 FragColor;

in vec2 TexCoord; //纹理数据
in vec3 tNormal; //法线数据
in vec3 FragPos; //位置

uniform sampler2D texture0;
uniform vec3 viewPos;



void main()
{
    vec3 lightPos = vec3(0,0,20); //太阳位置
    vec3 lightColor = vec3(0.5);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec2 texCoords = TexCoord;
    texCoords.x = 1.0-texCoords.x;
    vec3 objectColor= texture(texture0, texCoords).rgb;
    // 环境光照
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
    
    // 漫反射 转化为单位向量，两个向量点乘返回cos值
    float diffuseStrength = 1.0;
    vec3 norm = normalize(tNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * diffuseStrength;
    
    // 镜面光照  算摄像机向量与反射光线的夹角
    float shininess = 10;
    float specularStrength = 1.0;
    vec3 reflectDir = reflect(-1.0*lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);  //shininess次幂  高光反光度
    vec3 specular = specularStrength * spec * lightColor;  
    
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}