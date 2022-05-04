#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform samplerCube cubeMap;
uniform float time;

uniform vec3 cameraPosition;
uniform vec3 cameraFront;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

uniform vec2 resolution;

#define iTime time
#define iResolution resolution
const float pi = 3.14159;

struct ray     //光线
{
    vec3 origin;
    vec3 dir;
};

struct cube    //唯一确定一个立方体
{
    vec3 Up;
    vec3 Front;
    vec3 Left;
    vec3 Pos;
    vec3 Scale;
    int material;
    vec3 color;
};

struct sphere  //球体
{
    vec3 center;
    float radius;
    int material;
    vec3 color;
};

struct hit_record  //击中返回值
{
    float  t;  
    vec3 p;
    vec3 normal; 
    int material;
    vec3 color;
};


vec4 ffmax(vec4 a,vec4 b)
{
    if(a.x>b.x)
        return a;
    else
        return b;
}

vec4 ffmin(vec4 a,vec4 b)
{
    if(a.x<b.x)
        return a;
    else
        return b;
}
//光线与立方体求交
bool sdBox( ray r, cube c,inout hit_record record)
{ 
    hit_record rec;
    vec4 tx0,tx1,ty0,ty1,tz0,tz1;
    bool tx=false,ty=false,tz=false;
    //x方向
    if(abs(dot(c.Left,r.dir))>1e-6)
    {
        tx=true;
        vec3 left = normalize(c.Left);
        tx0.x = -1.0f*(c.Scale.x+dot(left,r.origin))/dot(left,r.dir);
        tx1.yzw=left;
        left.x*=-1.0f; 
        tx1.x = -1.0f*(c.Scale.x+dot(left,r.origin))/dot(left,r.dir);
        tx0.yzw=left;
    }
    //y方向
    if(abs(dot(c.Up,r.dir))>1e-6)
    {
        ty=true;
        vec3 up = normalize(c.Up);
        ty0.x = -1.0f*(c.Scale.y+dot(up,r.origin))/dot(up,r.dir);
        ty1.yzw=up;
        up.y*=-1.0f; 
        ty1.x = -1.0f*(c.Scale.y+dot(up,r.origin))/dot(up,r.dir);
        ty0.yzw=up;
    }
    //z方向
    if(abs(dot(c.Front,r.dir))>1e-6)
    {
        tz=true;
        vec3 front = normalize(c.Front);
        tz0.x = -1.0f*(c.Scale.z+dot(front,r.origin))/dot(front,r.dir);
        tz1.yzw=front;
        front.z*=-1.0f; 
        tz1.x = -1.0f*(c.Scale.z+dot(front,r.origin))/dot(front,r.dir);
        tz0.yzw=front;
    }
    vec4 Maxx=vec4(1000),Maxy=vec4(1000),Maxz=vec4(1000),Minx=vec4(-1000),Miny=vec4(-1000),Minz=vec4(-1000);
    if(tx)
    {
        Maxx=ffmax(tx0,tx1);
        Minx=ffmin(tx0,tx1);
    }
    if(ty)
    {
        Maxy=ffmax(ty0,ty1);
        Miny=ffmin(ty0,ty1);
    }
    if(tz)
    {
        Maxz=ffmax(tz0,tz1);
        Minz=ffmin(tz0,tz1);
    }
    vec4 Max_min=(ffmin(ffmin(Maxx,Maxy),Maxz));  //射出交点
    vec4 Min_max=(ffmax(ffmax(Minx,Miny),Minz));  //射入交点
    if(Min_max.x>1e-6)
    {
        rec.t = Min_max.x;
        rec.normal = Min_max.yzw;
        rec.color = c.color;
    }
    else
    {
        rec.t = Max_min.x;//判断法向  
        rec.normal = Max_min.yzw;   
        rec.color = c.color;
    }
    rec.p = r.origin + rec.t * r.dir;
    rec.material=c.material;
    if((Min_max.x<Max_min.x)&&(Max_min.x>1e-6)) //击中
    {
        if(rec.t<record.t)
        {
            record=rec;
            return true;
        }
    }
     return false;
}

//光线与球求交
bool sdSphere(ray r,sphere s,inout hit_record record)
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
        if (temp > 1e-1) 
        {
            rec.t = temp;
            if(rec.t<record.t)
            {
                rec.p = r.origin + rec.t * r.dir;
                rec.normal = (rec.p - s.center) / s.radius;
                rec.material=s.material;
                rec.color = s.color;
                record=rec;
                return true;
            }
        }
        temp = (-b + sqrt(discriminant)) / a;      //较大解
        if (temp > 1e-1)
        {
            rec.t = temp;
            if(rec.t<record.t)
            {
                rec.p = r.origin + rec.t * r.dir;
                rec.normal = (rec.p - s.center) / s.radius;
                rec.material=s.material;
                rec.color = s.color;
                record=rec;
                return true;
            }
        }
    }
    return false;
}

sphere s0=sphere(vec3(-3.0,0.0,0.0),1.3,2,vec3(1.0,0.0,0.0));
sphere s1=sphere(vec3(3.0,0.0,0.0),1.3,1,vec3(1.0,0.0,0.0));
sphere s2=sphere(vec3(0.0f),100,0,vec3(1.0,0.0,0.0));

cube c0=cube(vec3(0.0,1.0,0.0),vec3(0.0,0.0,1.0),vec3(1.0,0.0,0.0),vec3(0.0),vec3(1.0),1,vec3(1.0));

void map(ray r,inout hit_record rec)
{
    sdSphere(r,s0,rec);
    sdSphere(r,s1,rec);
    sdBox(r,c0,rec);
    sdSphere(r,s2,rec);
}

vec3 calColor(ray r)
{
    
    for(int i=0;i<50;i++)  //循环计算颜色
    {
        hit_record rec;
        rec.t=1000;
        map(r,rec);
        if(rec.material==0)  //如果击中墙壁 直接返回
        {
            return texture(cubeMap,rec.p).rgb;
        }
        else if(rec.material==1)  //若是透明材质
        {
            float n=0.7;  //折射率
            vec3 rDir;
            if(dot(r.dir,rec.normal)>0.0f)   //射出
                rDir=refract(r.dir,-1.0*rec.normal,n);
            else
                rDir=refract(r.dir,rec.normal,n);     //射入       
            //发射新射线
            r=ray(rec.p,normalize(rDir));
        }
        else if(rec.material==2)  //若是镜子
        {
            vec3 rDir;
            if(dot(r.dir,rec.normal)>0.0f)   //射出
                return vec3(1.0);
            else
                rDir=reflect(r.dir,rec.normal);  
            //发射新射线
            r=ray(rec.p,normalize(rDir));
        }
        else         //漫反射
        {
            return rec.color;
        }
    }
    //return texture(cubeMap,r.dir).rgb;
    return vec3(1.0,1.0,0.0);
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
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    FragColor = color;
}