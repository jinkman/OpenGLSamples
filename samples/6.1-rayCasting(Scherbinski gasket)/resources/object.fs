#version 330 core
in vec2 TexCoord;

uniform samplerCube cubeMap;
uniform float time;
//相机参数
uniform vec3 cameraPosition;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

vec2 resolution=vec2(800,600);

#define iTime time
#define iResolution resolution
const float pi = 3.14159;

struct ray     //光线
{
    vec3 origin;
    vec3 dir;
};


struct sphere  //球体
{
    vec3 center;
    float radius;
    int material;
    vec3 color;
    float ratio; //漫反射系数
};

struct hit_record  //击中返回值
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

//光线与球求交
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
        float temp = (-b - sqrt(discriminant))/a;  //较小解
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
        temp = (-b + sqrt(discriminant)) / a;      //较大解
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

//位置 半径 材质 颜色 反射系数
sphere s0=sphere(vec3(-2.0,0.0,0.0),1.998,1,vec3(1.0,0.0,0.0),0.0);
sphere s2=sphere(vec3(2.0,0.0,0.0),1.998,1,vec3(0.0,1.0,0.0),0.0);
sphere s3=sphere(vec3(0.0,2.0*sqrt(3),0.0),1.998,1,vec3(0.0,0.0,1.0),0.0);
sphere s4=sphere(vec3(0.0,2.0*sqrt(3)/3.0,-4.0*sqrt(6)/3.0),1.998,1,vec3(1.0,0.0,1.0),0.0);
//天空盒
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
    for(int j=0;j<times;j++)  //蒙特卡洛
    {
        for(int i=0;i<50;i++)  //循环计算颜色
        {
            rec.t=1000;
            map(r,rec);
            if(rec.material==0)  //如果击中墙壁 直接返回
            {
                color += texture(cubeMap,rec.p).rgb*pow(0.8,i);
                break;
            }
            else   //若是反射材质
            {   
                if(dot(r.dir,rec.normal)>0.0f)   //射出
                    return vec3(1.0);
                else
                {
                    vec3 rDir=reflect(r.dir,rec.normal);
                    color += rec.color*pow(0.8,i);
                    //发射新射线
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
    //标准化坐标系
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    ray r;   //发射光线
    //旋转光线方向
    r.dir = normalize(uv.x*cameraRight + uv.y*cameraUp + 3.0*cameraFront);;
    //旋转光线位置
    r.origin = cameraPosition;
    //计算颜色
    fragColor=vec4(calColor(r),1.0f);
}

void main(void)
{
    mainImage(gl_FragColor, gl_FragCoord.xy);
}