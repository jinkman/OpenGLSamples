#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform float time;
uniform vec2 resolution;
uniform samplerCube Map;

uniform vec3 cameraPosition;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

#define iResolution resolution.xy
#define iGlobalTime time
#define iMouse (mouse.xy * resolution.xy)

#define AA_SAMPLES 1
#define EULER 2.7182818284590452353602874


float wave(vec2 uv, vec2 emitter, float speed, float phase, float timeshift)
{
    float dst = distance(uv, emitter);
    return pow(EULER, sin(dst * phase - (iGlobalTime + timeshift) * speed)) / EULER;
}
vec2 wavedrag(vec2 uv, vec2 emitter){
	return normalize(uv - emitter);
}

#define DRAG_MULT 4.0

float getwaves(vec2 position)
{
    position *= 0.1;
    float iter = 0.0;
    float phase = 5.0;
    float speed = 3.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<12;i++)
    {
        vec2 p = vec2(sin(iter), cos(iter)) * 30.0;
        float res = wave(position, p, speed, phase, 0.0);
        float res2 = wave(position, p, speed, phase, 0.006);
        position -= wavedrag(position, p) * (res - res2) * weight * DRAG_MULT;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
       //speed *= 0.67;
    }
    return w / ws;
}

float getwavesHI(vec2 position){
    position *= 0.1;
	float iter = 0.0;
    float phase = 5.0;
    float speed = 3.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<35;i++){
        vec2 p = vec2(sin(iter), cos(iter)) * 30.0;
        float res = wave(position, p, speed, phase, 0.0);
        float res2 = wave(position, p, speed, phase, 0.006);
        position -= wavedrag(position, p) * (res - res2) * weight * DRAG_MULT;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
       // speed *= 0.67;
    }
    return w / ws;
}

float H = 0.0;
vec3 normal(vec2 pos, float e, float depth){
    vec2 ex = vec2(e, 0);
    H = getwavesHI(pos.xy) * depth;
    vec3 a = vec3(pos.x, H, pos.y);
    return normalize(cross(normalize(a-vec3(pos.x - e, getwavesHI(pos.xy - ex.xy) * depth, pos.y)), 
                           normalize(a-vec3(pos.x, getwavesHI(pos.xy + ex.yx) * depth, pos.y + e))));
}

float rand2sTimex(vec2 co)   //产生随时间变化的正弦函数
{
    return fract(sin(dot(co.xy * iGlobalTime,vec2(12.9898,78.233))) * 43758.5453);
}

float raymarchwater(vec3 camera, vec3 start, vec3 end, float depth)
{
    vec3 pos = start;
    float h = 0.0;
    float hupper = depth;
    float hlower = 0.0;
    vec2 zer = vec2(0.0);
    vec3 dir = normalize(end - start);
    for(int i=0;i<500;i++)
    {
        h = getwaves(pos.xz) * depth - depth;
        if(h + 0.01 > pos.y) 
            return distance(pos, camera);
        pos += dir * (pos.y - h);
    }
    return -1.0;
}

//与平面相交测试
float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ 
    //N*p(t)+D=0  y=R0+Rd*t   t=-(D+N*R0)/(N*Ed); 
    return clamp(dot(point - origin, normal) / dot(normal ,direction), -1.0, 9991999.0); 
}

//根据光线y值获得大气层颜色
vec3 getatm(vec3 ray)
{
    //return vec3(0.0, 0.2, 0.5);
    //return texture(nightMap,ray);
    return clamp(mix(vec3(0.5), vec3(0.0, 0.5, 1.0), sqrt(abs(ray.y))),0.0,1.0)/30.0;
}

//光线与球求交
bool sdSphere(in vec3 origin,in vec3 Dir,vec3 center,float Radius,inout vec3 Normal)
{
    vec3 oc = origin - center;
    float A = dot(Dir, Dir);
    float B = dot(oc, Dir);
    float C = dot(oc, oc) - Radius*Radius;
    float discriminant = B*B - A*C;
    if (discriminant > 1e-6) 
    {
        float temp = (-B - sqrt(discriminant)) / A;  //较小解
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

vec3 moon(vec3 orig,vec3 ray)
{
//    vec3 sd = normalize(vec3(0.0,0.05,1.0)); 
//    return vec3(clamp(pow(max(0.0, dot(ray, sd)), 520.0) * 110,0.0,1.0)/1.5);
    vec3 Normal = vec3(0.0);
    vec3 pos= vec3(0.0,3.0,100.0)+orig;
    bool judge = sdSphere(orig,ray,pos,12.0,Normal);
    if(judge==true)
        return texture(Map,Normal*-1.0).rgb;
        //return vec3(1.0,0.0,0.0);
    else
    {
        vec3 sd = normalize(pos-orig);
        float color=clamp(pow(max(0.0, dot(ray, sd)), 520.0) * 110,0.0,1.0)/1.5; 
        return vec3(color);
    }
 
}

vec3 getColor(vec2 uv)
{
    float waterdepth = 2.7;  //水波高度
    vec3 wfloor = vec3(0.0, -waterdepth, 0.0);
    vec3 wceil = vec3(0.0, 0.0, 0.0);
    //光线投射
    vec3 orig = cameraPosition;
    vec3 ray = normalize(uv.x*cameraRight + uv.y*cameraUp + 3.0*cameraFront);
    //天空颜色
    if(ray.y >= -0.01)
    {
        vec3 C = getatm(ray) * 2.0 + moon(orig,ray); //天空与月亮颜色
        C = normalize(C) * sqrt(length(C));   //gamma校正
        return C; 
    }
    //海面颜色
    float hihit = intersectPlane(orig, ray, wceil, vec3(0.0, 1.0, 0.0));  
    float lohit = intersectPlane(orig, ray, wfloor, vec3(0.0, 1.0, 0.0));
    vec3 hipos = orig + ray * hihit;   //上平面交点
    vec3 lopos = orig + ray * lohit;   //下平面交点
    float dist = raymarchwater(orig, hipos, lopos, waterdepth);
    vec3 pos = orig + ray * dist;

	vec3 N = normal(pos.xz, 0.01, waterdepth);
    vec2 velocity = N.xz * (1.0 - N.y);
    N = mix(vec3(0.0, 1.0, 0.0), N, 1.0 / (dist * dist * 0.001 + 1.0));
    vec3 R = reflect(ray, N);
    float fresnel = (0.04 + (1.0-0.04)*(pow(1.0 - max(0.0, dot(-N, ray)), 5.0)));
	
    vec3 C = fresnel * getatm(R) * 2.0 + fresnel * moon(orig,R);
    C = normalize(C) * sqrt(length(C)); //gamma
    
	return C;
}

void mainImage( inout vec4 fragColor, in vec2 fragCoord )
{
    //标准化坐标系
    vec2 xy = gl_FragCoord.xy / resolution.xy;
    vec2 uv = (-1.0 + 2.0 * xy) * vec2(resolution.x/resolution.y,1.0);
    //计算颜色
    vec3 C = vec3(0.0);
    float W = 0.0;
    //采样次数
    for(int i=0;i<AA_SAMPLES;i++){
        C += getColor(uv + vec2(rand2sTimex(uv), rand2sTimex(uv + 100.0)) / iResolution.xy);  //����Ŷ�   
        uv += 200.0;
        W += 1.0;
    }
    C /= W;
    fragColor = vec4(C,1.0); 
}

void main( void ) 
{
    vec4 color = vec4(0.0);
    mainImage(color, gl_FragCoord.xy);
    FragColor = color;
}