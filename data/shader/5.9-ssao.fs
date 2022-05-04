#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// 参数
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// 对应平铺
const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); 

uniform mat4 projection;
uniform bool useSSAO;

void main()
{
    // 获得输入
    if(useSSAO)
    {
        vec3 fragPos = texture(gPosition, TexCoords).xyz;
        vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
        vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
        // Gramm-Schmidt正交化
        vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
        vec3 bitangent = cross(normal, tangent);
        mat3 TBN = mat3(tangent, bitangent, normal);
        // 计算遮挡率
        float occlusion = 0.0;
        for(int i = 0; i < kernelSize; ++i)
        {
            // 获取样本位置
            vec3 sample = TBN * samples[i]; // 转换到TBN空间
            sample = fragPos + sample * radius; 
       
            vec4 offset = vec4(sample, 1.0);
            offset = projection * offset; // 转换到裁剪空间
            offset.xyz /= offset.w; // 透视除法
            offset.xyz = offset.xyz * 0.5 + 0.5; // 转换到 0.0 - 1.0
        
            // 获得样本深度
            float sampleDepth = texture(gPosition, offset.xy).z; 
        
            // 保证只在半径范围内有效
            float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
            occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
        }
        occlusion = 1.0 - (occlusion / kernelSize);
        FragColor = occlusion;
    }
    else
       FragColor = 1.0;
}