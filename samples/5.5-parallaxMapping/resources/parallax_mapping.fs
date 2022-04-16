#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // ��Ȳ���
    const float minLayers = 8;
    const float maxLayers = 32;
    // �н�С������
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // ÿ�����
    float layerDepth = 1.0 / numLayers;
    // �����ϲ㿪ʼ����
    float currentLayerDepth = 0.0;
    // ÿ��Ӧ�ƶ�����������
    vec2 P = viewDir.xy / viewDir.z * heightScale; //�ƶ�����
    vec2 deltaTexCoords = P / numLayers;
  
    // ��ʼֵ
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // �ƶ�����
        currentTexCoords -= deltaTexCoords;
        // �ƶ��������������ֵ
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
        // ��ǰ���
        currentLayerDepth += layerDepth;  
    }
    
    // ǰһ����ȵ���������
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // �����ֵ����
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // ���Բ�ֵ
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{           
    // ����Ӳ���ͼ�����������Լ��߽��ⶪ��
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);       
    /*if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;*/

    // �ӷ�����ͼ��÷�������
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    // ��õ�ǰ����������ͼ��ɫ
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // ������
    vec3 ambient = 0.1 * color;
    // ������
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // �߹�    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}