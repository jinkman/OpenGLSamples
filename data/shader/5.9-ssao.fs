#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// settings
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// tile
const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); 

uniform mat4 projection;
uniform bool useSSAO;

void main()
{
    // input
    if(useSSAO)
    {
        vec3 fragPos = texture(gPosition, TexCoords).xyz;
        vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
        vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
        // Gramm-Schmidt
        vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
        vec3 bitangent = cross(normal, tangent);
        mat3 TBN = mat3(tangent, bitangent, normal);
        // cal occlusion ratio
        float occlusion = 0.0;
        for(int i = 0; i < kernelSize; ++i)
        {
            // get sample pos
            vec3 sample = TBN * samples[i]; // to TBN space
            sample = fragPos + sample * radius; 
       
            vec4 offset = vec4(sample, 1.0);
            offset = projection * offset; // Convert to crop space
            offset.xyz /= offset.w; // The perspective divide
            offset.xyz = offset.xyz * 0.5 + 0.5; // Convert to 0.0 - 1.0
        
            // get sample depth
            float sampleDepth = texture(gPosition, offset.xy).z; 
        
            // Guaranteed to be valid only within a radius
            float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
            occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
        }
        occlusion = 1.0 - (occlusion / kernelSize);
        FragColor = occlusion;
    }
    else
       FragColor = 1.0;
}