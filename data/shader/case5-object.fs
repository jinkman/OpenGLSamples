#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 resolution;
uniform samplerCube diffuseMap;
uniform sampler2D frameMap;


//相机参数
uniform vec3 cameraPosition;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec3 spherePos;

#define iResolution resolution.xy

//光线与球求交
bool sdSphere(in vec3 origin,in vec3 Dir,vec3 center,float Radius,inout vec3 Normal,inout float temp)
{
    vec3 oc = origin - center;
    float A = dot(Dir, Dir);
    float B = dot(oc, Dir);
    float C = dot(oc, oc) - Radius*Radius;
    float discriminant = B*B - A*C;
    if (discriminant > 1e-6) 
    {
        temp = (-B - sqrt(discriminant)) / A;  //较小解
        if (temp > 1e-6) 
        {
            Normal = (origin + temp * Dir - center) / Radius;
            return true;
        }
        temp = (-B + sqrt(discriminant)) / A;      //较大解
        if (temp > 1e-6)
        {
            Normal = (origin + temp * Dir - center) / Radius;
            return true;
        }
    }
    return false;
}

vec4 map(vec3 orig,vec3 ray)
{
    vec3 Normal=vec3(0.0);
    float temp=0.0;
    bool judge = sdSphere(orig,ray,spherePos,0.05,Normal,temp);
    if(judge==true)
        return vec4(texture(diffuseMap,Normal).rgb,temp/100.0);
        //return vec4(0.2, 0.3, 0.4, 1.0);
    else 
        return vec4(0.0); 
}

void mainImage( inout vec4 fragColor, in vec2 fragCoord )
{
    //标准化坐标系
    vec2 xy = gl_FragCoord.xy / resolution.xy;
    vec2 uv = (-1.0 + 2.0 * xy) * vec2(resolution.x/resolution.y,1.0);
    vec3 orig = cameraPosition;
    vec3 ray = normalize(uv.x*cameraRight + uv.y*cameraUp + 3.0*cameraFront);
    //计算颜色
    vec4 C = vec4(0.0);
    C= map(orig,ray);
    //如果没有击中
    if(C.a < 1e-6)
        C = texture(frameMap,TexCoord);
    else if(texture(frameMap,TexCoord).a < 1e-6)
        C=C;
    //如果击中
    else
    {
        if(texture(frameMap,TexCoord).a < C.a)
            C = texture(frameMap,TexCoord);
    }
    fragColor = C;   //最终颜色
    //C = texture(frameMap,TexCoord);
}

void main( void ) 
{
    vec4 color = vec4(0.0);
    mainImage(color, gl_FragCoord.xy);
    FragColor = color;
}