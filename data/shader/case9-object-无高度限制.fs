#version 330 core
in vec2 TexCoord;

uniform float time;
uniform vec2 resolution;
uniform samplerCube moonMap;
uniform samplerCube nightMap;
//�������
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

float rand2sTimex(vec2 co)   //������ʱ��仯�����Һ���
{
    return fract(sin(dot(co.xy * iGlobalTime,vec2(12.9898,78.233))) * 43758.5453);
}


//���ݹ���yֵ��ô�������ɫ
vec3 getatm(vec3 ray)
{
    return clamp(mix(vec3(0.5), vec3(0.0, 0.5, 1.0), sqrt(abs(ray.y))),0.0,1.0)/30.0;
}

//����������
bool sdSphere(in vec3 origin,in vec3 Dir,vec3 center,float Radius,out vec3 Normal)
{
    vec3 oc = origin - center;
    float A = dot(Dir, Dir);
    float B = dot(oc, Dir);
    float C = dot(oc, oc) - Radius*Radius;
    float discriminant = B*B - A*C;
    if (discriminant > 1e-6) 
    {
        float temp = (-B - sqrt(discriminant)) / A;  //��С��
        if (temp > 1e-6) 
        {
            Normal = (origin + temp * Dir - center) / Radius;
            return true;
        }
        temp = (-B + sqrt(discriminant)) / A;      //�ϴ��
        if (temp > 1e-6)
        {
            Normal = (origin + temp * Dir - center) / Radius;
            return true;
        }
    }
    Normal=vec3(0.0);
    return false;
}

//���̫����ɫ
vec3 sun(vec3 orig,vec3 ray)
{
    vec3 Normal;
    vec3 pos= vec3(0.0,3.0,100.0)+orig;
    bool judge = sdSphere(orig,ray,pos,12.0,Normal);
    if(judge==true)
        return texture(moonMap,Normal*-1.0).rgb;
    else
    {
        vec3 sd = normalize(pos-orig);
        float color=clamp(pow(max(0.0, dot(ray, sd)), 520.0) * 110,0.0,1.0)/1.5; 
        return vec3(color);
    }
}

float Map(in vec3 p)
{
    float depth=5.0;
    float h = getwaves(p.xz) * depth - depth;
    return p.y-h;
}

//--------------------------------------------------------------------------
// ���ַ��ƽ���ȷ�ĵ�
float BinarySubdivision(in vec3 rO, in vec3 rD, float t, float oldT)
{
    float halfwayT = 0.0;
    for (int n = 0; n < 5; n++)
    {
        halfwayT = (oldT + t ) * 0.5;
        if (Map(rO + halfwayT*rD) < 0.05)
            t = halfwayT;
        else
            oldT = halfwayT;
    }
    return t;
}

//--------------------------------------------------------------------------
//Ͷ�䳡��
bool Scene(in vec3 rO, in vec3 rD, out float resT )
{
    float t = 0.0;
    float oldT = 0.0;
    float delta = 0.0;
    float h = 1.0;
    bool hit = false;
    for( int j=0; j < 80; j++ )
    {
        vec3 p = rO + t*rD;
        if (!hit)
        {
            h = Map(p); // ��õ�ǰ������εĸ߶Ȳ�
    
            // ����߶�ֵ���С����ֵ ��������
            if( h < 0.05)
            {
                // �ƽ���ȷ��
                resT = BinarySubdivision(rO, rD, t, oldT);
                hit = true;
                return hit;
            }
            else
            {
                delta = max(0.04, 0.35*h) + (t*0.04);  //����ǰ���ٶ� ���ٶ���׼ȷ��֮��Ȩ�� ԽԶ׼ȷ��Խ��
                oldT = t;
                t += delta;
            }
        }
    }
    return hit;
}


//--------------------------------------------------------------------------
void main(void)
{
    //��׼������ϵ
    vec2 xy = gl_FragCoord.xy / resolution.xy;
    vec2 uv = (-1.0 + 2.0 * xy) * vec2(resolution.x/resolution.y,1.0);
    //�������߷���
    vec3 dir = normalize(uv.x*cameraRight + uv.y*cameraUp + 3.0*cameraFront);

    vec3 col;
    float distance;
    if( !Scene(cameraPosition, dir, distance) )  //�������
    {
        col = getatm(dir) * 2.0 + sun(cameraPosition,dir);  //�����̫����ɫ
        col = normalize(col) * sqrt(length(col));   //gammaУ��
    }
    else                                               //���Ʋ�ԭ
    {
        // ��ȡ���λ��
        vec3 pos = cameraPosition + distance * dir;
        // ��ȡ��ķ�����
        vec2 p = vec2(0.01, 0.0);
        vec3 nor = vec3(0.0, getwaves(pos.xz), 0.0);
        vec3 v2 = nor-vec3(p.x, getwaves(pos.xz+p), 0.0);
        vec3 v3 = nor-vec3(0.0, getwaves(pos.xz-p.yx), -p.x);
        nor = cross(v2, v3);
        nor = normalize(nor);

           //����
        vec3 R = reflect(dir, nor);
        float fresnel = (0.04 + (1.0-0.04)*(pow(1.0 - max(0.0, dot(-nor, dir)), 5.0)));
        col = fresnel * getatm(R) * 2.0 + fresnel * sun(cameraPosition,R);
        col = normalize(col) * sqrt(length(col)); //gammaУ��
    }
    gl_FragColor=vec4(col,1.0);
}
