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

struct cube
{
    vec3 Up;
    vec3 Front;
    vec3 Left;
    vec3 Pos;
    vec3 Scale;
    int material;
    vec3 color;
};

struct sphere
{
    vec3 center;
    float radius;
    int material;
    vec3 color;
};

struct hit_record  // hit info
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
// The ray intersects the cube
bool sdBox( ray r, cube c,inout hit_record record)
{ 
    hit_record rec;
    vec4 tx0,tx1,ty0,ty1,tz0,tz1;
    bool tx=false,ty=false,tz=false;
    //x dir
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
    //y dir
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
    //z dir
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
    vec4 Max_min=(ffmin(ffmin(Maxx,Maxy),Maxz));  //out point
    vec4 Min_max=(ffmax(ffmax(Minx,Miny),Minz));  //in point
    if(Min_max.x>1e-6)
    {
        rec.t = Min_max.x;
        rec.normal = Min_max.yzw;
        rec.color = c.color;
    }
    else
    {
        rec.t = Max_min.x;  
        rec.normal = Max_min.yzw;   
        rec.color = c.color;
    }
    rec.p = r.origin + rec.t * r.dir;
    rec.material=c.material;
    if((Min_max.x<Max_min.x)&&(Max_min.x>1e-6)) // hit
    {
        if(rec.t<record.t)
        {
            record=rec;
            return true;
        }
    }
     return false;
}

// The ray intersects the sphere
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
        float temp = (-b - sqrt(discriminant))/a;  // small
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
        temp = (-b + sqrt(discriminant)) / a;      // big
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
    
    for(int i=0;i<50;i++)  // loop to cal color
    {
        hit_record rec;
        rec.t=1000;
        map(r,rec);
        if(rec.material==0)  // If you hit the wall, go straight back
        {
            return texture(cubeMap,rec.p).rgb;
        }
        else if(rec.material==1)  // If it's transparent
        {
            float n=0.7;  // The refractive index
            vec3 rDir;
            if(dot(r.dir,rec.normal)>0.0f)   // out
                rDir=refract(r.dir,-1.0*rec.normal,n);
            else
                rDir=refract(r.dir,rec.normal,n);     // in       
            // new ray
            r=ray(rec.p,normalize(rDir));
        }
        else if(rec.material==2)  // If the mirror
        {
            vec3 rDir;
            if(dot(r.dir,rec.normal)>0.0f)   //out
                return vec3(1.0);
            else
                rDir=reflect(r.dir,rec.normal);  
            // new ray
            r=ray(rec.p,normalize(rDir));
        }
        else // diffuse
        {
            return rec.color;
        }
    }
    //return texture(cubeMap,r.dir).rgb;
    return vec3(1.0,1.0,0.0);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Standardized coordinate system
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    ray r;
    // Rotation of light direction
    r.dir = normalize(uv.x*cameraRight + uv.y*cameraUp + 3.0*cameraFront);
    // ray position
    r.origin = cameraPosition;
    // cal color
    fragColor=vec4(calColor(r),1.0f);
}

void main(void)
{
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    FragColor = color;
}