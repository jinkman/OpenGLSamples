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
    // 深度层数
    const float minLayers = 8;
    const float maxLayers = 32;
    // 夹角小层数多
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // 每层深度
    float layerDepth = 1.0 / numLayers;
    // 从最上层开始遍历
    float currentLayerDepth = 0.0;
    // 每层应移动的纹理坐标
    vec2 P = viewDir.xy / viewDir.z * heightScale; //移动比例
    vec2 deltaTexCoords = P / numLayers;
  
    // 初始值
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // 移动纹理
        currentTexCoords -= deltaTexCoords;
        // 移动后坐标纹理深度值
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
        // 当前深度
        currentLayerDepth += layerDepth;  
    }
    
    // 前一层深度的纹理坐标
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // 计算插值常量
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // 线性插值
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{           
    // 获得视差贴图的纹理坐标以及边界外丢弃
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);       
    /*if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;*/

    // 从发现贴图获得法线数据
    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    // 获得当前纹理坐标贴图颜色
    vec3 color = texture(diffuseMap, texCoords).rgb;
    // 环境光
    vec3 ambient = 0.1 * color;
    // 漫反射
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // 高光    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}