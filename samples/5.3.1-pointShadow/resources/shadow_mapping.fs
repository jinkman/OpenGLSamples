#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // ִ��͸�ӱ任
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // ת���� [0,1] ��Χ ������ȱȽ�
    projCoords = projCoords * 0.5 + 0.5;
    // ��������ͼ���Դ��������
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // ��õ�ǰ���
    float currentDepth = projCoords.z;
    // ������Ӱʧ��
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // ������Ӱ��
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    //ȡ��Χƽ��ֵ
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // ����Զƽ���û����Ӱ
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.4);
    // ������
    vec3 ambient = 0.2 * color;
    // ������
    vec3 lightDir = normalize(lightPos); //����ͶӰ
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // ���淴��
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // ������Ӱ
    float shadow = shadows ? ShadowCalculation(fs_in.FragPosLightSpace) : 0.0;                      
    shadow = min(shadow, 0.75); // ����һ����Ӱǿ�ȣ�����Ӱ��������һЩ��ɢ/����
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0f);
}