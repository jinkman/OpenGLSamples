#version 330 core
in vec2 TexCoord;

uniform samplerCube cubeMap;
uniform float time;
//�������
uniform vec3 cameraPosition;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

vec2 resolution=vec2(800,600);

#define iTime time
#define iResolution resolution
const float pi = 3.14159;

struct ray     //����
{
    vec3 origin;
    vec3 dir;
};


struct sphere  //����
{
    vec3 center;
    float radius;
    int material;
    vec3 color;
    float ratio; //������ϵ��
};

struct hit_record  //���з���ֵ
{
    float  t;  
    vec3 p;
    vec3 normal; 
    int material;
    vec3 color;
    float ratio;
};

float rand(vec3 n)
{   
    int x = int(n.x + n.y * 57 +n.z * 113);
    x = (x<<13) ^ x;
    return sin( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

//����������
bool sdSphere(ray r,sphere s,out hit_record record)
{
    hit_record rec;
    vec3 oc = r.origin - s.center;
    float a = dot(r.dir, r.dir);
    float b = dot(oc, r.dir);
    float c = dot(oc, oc) - s.radius*s.radius;
    float discriminant = b*b - a*c;
    if (discriminant > 1e-6) 
    {
        float temp = (-b - sqrt(discriminant))/a;  //��С��
        if (temp > 1e-3) 
        {
            rec.t = temp;
            if(rec.t<record.t)
            {
                rec.p = r.origin + rec.t * r.dir;
                rec.normal = (rec.p - s.center) / s.radius;
                rec.material=s.material;
                rec.color = s.color;
                rec.ratio=s.ratio;
                record=rec;
                return true;
            }
        }
        temp = (-b + sqrt(discriminant)) / a;      //�ϴ��
        if (temp > 1e-3)
        {
            rec.t = temp;
            if(rec.t<record.t)
            {
                rec.p = r.origin + rec.t * r.dir;
                rec.normal = (rec.p - s.center) / s.radius;
                rec.material=s.material;
                rec.color = s.color;
                rec.ratio=s.ratio;
                record=rec;
                return true;
            }
        }
    }
    return false;
}

//λ�� �뾶 ���� ��ɫ ����ϵ��
sphere s0=sphere(vec3(-2.0,0.0,0.0),1.998,1,vec3(1.0,0.0,0.0),0.0);
sphere s2=sphere(vec3(2.0,0.0,0.0),1.998,1,vec3(0.0,1.0,0.0),0.0);
sphere s3=sphere(vec3(0.0,2.0*sqrt(3),0.0),1.998,1,vec3(0.0,0.0,1.0),0.0);
sphere s4=sphere(vec3(0.0,2.0*sqrt(3)/3.0,-4.0*sqrt(6)/3.0),1.998,1,vec3(1.0,0.0,1.0),0.0);
//��պ�
sphere s1=sphere(vec3(0.0f),100,0,vec3(1.0,0.0,0.0),0.0);


void map(ray r,out hit_record rec)
{
    sdSphere(r,s2,rec);
    sdSphere(r,s1,rec);
    sdSphere(r,s0,rec);
    sdSphere(r,s3,rec);
    sdSphere(r,s4,rec);
}

vec3 calColor(ray r)
{
    vec3 color;
    hit_record rec;
    int times=1;
    for(int j=0;j<times;j++)  //���ؿ���
    {
        for(int i=0;i<50;i++)  //ѭ��������ɫ
        {
            rec.t=1000;
            map(r,rec);
            if(rec.material==0)  //�������ǽ�� ֱ�ӷ���
            {
                color += texture(cubeMap,rec.p).rgb*pow(0.8,i);
                break;
            }
            else   //���Ƿ������
            {   
                if(dot(r.dir,rec.normal)>0.0f)   //���
                    return vec3(1.0);
                else
                {
                    vec3 rDir=reflect(r.dir,rec.normal);
                    color += rec.color*pow(0.8,i);
                    //����������
                    r=ray(rec.p,normalize(rDir));
                }
           
            }
        }
    }
    color=color/float(times);
    return color;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //��׼������ϵ
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    ray r;   //�������
    //��ת���߷���
    r.dir = normalize(uv.x*cameraRight + uv.y*cameraUp + 3.0*cameraFront);;
    //��ת����λ��
    r.origin = cameraPosition;
    //������ɫ
    fragColor=vec4(calColor(r),1.0f);
}

void main(void)
{
    mainImage(gl_FragColor, gl_FragCoord.xy);
}