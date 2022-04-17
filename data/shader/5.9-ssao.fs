#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise; //�����ת����

uniform vec3 samples[64];

// ����
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// ��Ӧƽ��
const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); 

uniform mat4 projection;
uniform bool useSSAO;

void main()
{
    // �������
    if(useSSAO)
    {
        vec3 fragPos = texture(gPosition, TexCoords).xyz;
        vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
        vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
        // Gramm-Schmidt������
        vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
        vec3 bitangent = cross(normal, tangent);
        mat3 TBN = mat3(tangent, bitangent, normal);
        // �����ڵ���
        float occlusion = 0.0;
        for(int i = 0; i < kernelSize; ++i)
        {
            // ��ȡ����λ��
            vec3 sample = TBN * samples[i]; // ת����TBN�ռ�
            sample = fragPos + sample * radius; 
       
            vec4 offset = vec4(sample, 1.0);
            offset = projection * offset; // ת�����ü��ռ�
            offset.xyz /= offset.w; // ͸�ӳ���
            offset.xyz = offset.xyz * 0.5 + 0.5; // ת���� 0.0 - 1.0
        
            // ����������
            float sampleDepth = texture(gPosition, offset.xy).z; 
        
            // ��ֻ֤�ڰ뾶��Χ����Ч
            float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
            occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
        }
        occlusion = 1.0 - (occlusion / kernelSize);
        FragColor = occlusion;
    }
    else
       FragColor = 1.0;
}
