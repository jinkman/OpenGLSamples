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

struct ray
{
    vec3 origin;
    vec3 dir;
};


struct sphere
{
    vec3 center;
    float radius;
    int material;
    vec3 color;
    float ratio; //diffuse ratio
};

struct hit_record // hit info
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

bool sdSphere(inout ray r,sphere s,inout hit_record record)
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

//position radus material color Reflection coefficient
sphere s0=sphere(vec3(-2.0,0.0,0.0),1.998,1,vec3(1.0,0.0,0.0),0.0);
sphere s2=sphere(vec3(2.0,0.0,0.0),1.998,1,vec3(0.0,1.0,0.0),0.0);
sphere s3=sphere(vec3(0.0,2.0*sqrt(3),0.0),1.998,1,vec3(0.0,0.0,1.0),0.0);
sphere s4=sphere(vec3(0.0,2.0*sqrt(3)/3.0,-4.0*sqrt(6)/3.0),1.998,1,vec3(1.0,0.0,1.0),0.0);
// skybox
sphere s1=sphere(vec3(0.0f),100,0,vec3(1.0,0.0,0.0),0.0);


void map(inout ray r,inout hit_record rec)
{
    sdSphere(r,s2,rec);
    sdSphere(r,s1,rec);
    sdSphere(r,s0,rec);
    sdSphere(r,s3,rec);
    sdSphere(r,s4,rec);
}

vec3 calColor(inout ray r)
{
    vec3 color = vec3(0.0);
    hit_record rec;
    int times=1;
    for(int j=0;j<times;j++)  // The monte carlo
    {
        for(int i=0;i<50;i++)  // Cyclic calculation of color
        {
            rec.t=1000;
            map(r,rec);
            if(rec.material==0)  // If you hit the wall, go straight back
            {
                color += texture(cubeMap, normalize(rec.p)).rgb * pow(0.65,i);
                break;
            }
            else   // If it's a reflective material
            {   
                if(dot(r.dir,rec.normal)>0.0f)   // out
                    return vec3(1.0);
                else
                {
                    vec3 rDir=reflect(r.dir,rec.normal);
                    color += rec.color*pow(0.65,i);
                    // new ray
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
    // Standardized coordinate system
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    ray r;
    // dir
    r.dir = normalize(uv.x*cameraRight + uv.y*cameraUp + 3.0*cameraFront);;
    // pos
    r.origin = cameraPosition;
    // color
    fragColor=vec4(calColor(r),1.0f);
}


void main(void)
{
    vec4 color;
    mainImage(color, gl_FragCoord.xy);
    FragColor = color;
}