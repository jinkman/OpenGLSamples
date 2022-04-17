#version 330 core
in vec2 TexCoord;

uniform sampler2D diffuse;
uniform float time;

vec2 resolution=vec2(800,600);

// shadertoy emulation
#define iTime time
#define iResolution resolution

// --------[ Original ShaderToy begins here ]---------- //
const float pi = 3.14159;

mat3 xrot(float t)    //绕x轴旋转
{
    return mat3(1.0, 0.0, 0.0,
                0.0, cos(t), -sin(t),
                0.0, sin(t), cos(t));
}

mat3 yrot(float t)    //绕y轴旋转
{
    return mat3(cos(t), 0.0, -sin(t),
                0.0, 1.0, 0.0,
                sin(t), 0.0, cos(t));
}

mat3 zrot(float t)    //绕z轴旋转
{
    return mat3(cos(t), -sin(t), 0.0,
                sin(t), cos(t), 0.0,
                0.0, 0.0, 1.0);
}

float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}

//点到立方体最短距离  在内部则返回负值
float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float sdTube(vec3 p, float a)
{
    return length(p.xy) - a;
}

float room(vec3 p)
{
    float fd = sdBox(p, vec3(8.0));
    fd = min(fd, sdBox(p+vec3(0.0,6.0,0.0), vec3(2.0,2.0,10.0)));
    fd = min(fd, sdBox(p+vec3(0.0,6.0,0.0), vec3(10.0,2.0,2.0)));
    return fd;
}

bool alpha = false;

vec2 map(vec3 p)
{   
	float d = -room(p);
    float m = 0.0;
    float pe = sdBox(p+vec3(-1.0,8.0,0.0), vec3(1.0, 2.0, 1.0));  //位置及比例
    if (pe < d)
    {
        d = pe;
        m = 1.0;
    }
    //增加的底座1
    float pe1 = sdBox(p+vec3(1.0,8.0,0.0), vec3(1.0, 2.0, 1.0));
    if (pe1 < d)
    {
        d = pe1;
        m = 1.0;
    }
    
    if (alpha) 
    {
        float c = sdBox(p+vec3(-1.0,5.0,0.0), vec3(1.0));      //位置及比例
        if (c < d)
        {
            d = c;
            m = 2.0;
        }
        //增加透明玻璃1
        float c1 = sdBox(p+vec3(1.0,5.0,0.0), vec3(1.0));
        if (c1 < d)
        {
            d = c1;
            m = 2.0;
        }
    } 
    else 
    {
        float c = length(p+vec3(-1.0,5.3,0.0)) - 0.7;
        if (c < d)
        {
            d = c;
            m = 3.0;
        }
        //增加球1
        float c1 = length(p+vec3(1.0,5.3,0.0)) - 0.7;  //位置及半径
        if (c1 < d)
        {
            d = c1;
            m = 3.0;
        }
    }
    
    return vec2(d, m);   //m为材质
}

//计算点的法向量  通用公式  巧妙
vec3 normal(vec3 p)
{
    vec3 o = vec3(0.01, 0.0, 0.0);
    return normalize(vec3(map(p+o.xyy).x - map(p-o.xyy).x,
                          map(p+o.yxy).x - map(p-o.yxy).x,
                          map(p+o.yyx).x - map(p-o.yyx).x));
}

vec3 trace(vec3 o, vec3 r)
{
    //逼近最近点
    float t = 0.0;
    vec2 d;
    for (int i = 0; i < 32; ++i) 
    {
        vec3 p = o + r * t;
        d = map(p);
        //默认击中
        if(abs(d.x)<0.01) 
            break;
        t += d.x;
    }
    return vec3(t,d);
}

float mapl(vec3 p)
{
    p *= yrot(pi*0.05);  //射线方向
    float r = 0.01;  //光线半径
    vec3 q = fract(p) * 2.0 - 1.0;   //取小数  产生多条
    float a = sdTube(vec3(q.z,q.y,q.x), r);   //转换坐标系 横纵方向  射线在z轴
    float b = sdTube(vec3(q.x,q.y,q.z), r);
    return min(a,b);
}


//测试是否击中射线
float tracel(vec3 o, vec3 r)
{
    float t = 0.0;
    for (int i = 0; i < 16; ++i) 
    {
        vec3 p = o + r * t;
        float d = mapl(p);
        t += d * 0.8;
    }
    return t;
}

//加载纹理
vec3 _texture(vec3 p)
{
    vec3 ta = texture(diffuse, vec2(p.y,p.z)).xyz;
    vec3 tb = texture(diffuse, vec2(p.x,p.z)).xyz;
    vec3 tc = texture(diffuse, vec2(p.x,p.y)).xyz;
    return (ta + tb + tc) / 3.0;
}

//位置 方向 法向 
vec4 diffcol(vec3 w, vec3 r, vec3 sn, vec2 fd, float t)
{
    vec3 mdiff = vec3(0.0); 
    float gloss = 0.0;
    float light = 1.0;
    if (fd.y == 1.0)    //底座为白色
    {
        mdiff = vec3(1.0);
        gloss = 1.0;
    } 
    else if (fd.y == 2.0)   //透明玻璃
    {
        mdiff = vec3(1.0);
    } 
    else if (fd.y == 3.0)   //球
    {
        mdiff = vec3(1.0);
        gloss = 1.0;
    } 
    else                //其它为击中房间
    {
        if (sn.y > 0.9)  //地面
        {
            mdiff = vec3(1.0) * vec3(0.2,0.5,0.2);
            gloss = 0.1;
        } 
        else if (sn.y < -0.9)  //顶部 
        {
            mdiff = vec3(5.0);
            gloss = 0.1;
            light = 0.0;
        } 
        else   //四周墙壁
        {
            mdiff = _texture(w*0.1) * vec3(0.0, 1.0, 1.0);
            gloss = 1.0;
        }
    }
    float fog = 1.0 / (1.0 + t * t * 0.05);  //雾化效果
    mdiff = mix(mdiff, vec3(1.0), abs(w.y) / 8.0 * light);
    return vec4(mdiff*fog, gloss);
}

vec3 laser(vec3 o, vec3 r)
{
    float t = tracel(o, r);
    float k = 1.0 / (1.0 + t * t * 0.1);
    return vec3(1.0, 1.0, 0.0) * k;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //标准化坐标系
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    //旋转
    mat3 xfm = xrot(sin(-iTime*0.25)*0.25) * yrot(iTime);
    //旋转光线方向
    vec3 r = normalize(vec3(uv, 1.0));
    r *= xrot(pi * 0.25) * xfm;
    //旋转光线位置
    vec3 o = vec3(0.0, 0.0, -3.0);
    o *= xfm;
    o.y -= 3.0;
    alpha = true;
    vec3 t = trace(o, r);   
    vec3 w = o + r * t.x;  //获得光线击中的最近的点
    vec2 fd = t.yz;
    vec3 sn = normal(w);  //击中点的法向量

    //默认打中房间
    vec4 mdiff;
    
    //如果打中玻璃
    if (fd.y == 2.0) 
    {
        alpha = false;
        vec3 rr = refract(r, sn, 0.9);      //折射向量  折射系数
        //再次发射
        vec3 art = trace(w, rr);
        vec3 aw = w + rr * art.x;     //再次到达的点
        vec2 afd = art.yz;
        vec3 asn = normal(aw);
    
        if (afd.y == 3.0)    //折射击中球
         {
            alpha = false;   //不会再与玻璃检测
            vec3 brf = reflect(rr, asn);     //反射向量
            vec3 brt = trace(aw + asn * 0.1, brf);
            vec3 bw = aw + brf * brt.x;
            vec2 bfd = brt.yz;
            vec3 bsn = normal(bw);
            vec4 bdiff = diffcol(bw, brf, bsn, bfd, brt.x);
            float prod = max(dot(rr, -asn), 0.0);
            mdiff.xyz = bdiff.xyz * prod + laser(aw, t.x+art.x+brf);
        }
        else                 //折射击中房间
        {
            mdiff = diffcol(aw, rr, asn, afd, t.x+art.x);  //雾化时时间为所有做过的路径
            mdiff.xyz += laser(w, rr);
            mdiff.w = 1.0;
        }
    }
    //击中房间或者底座
    else
        mdiff = diffcol(w, r, sn, fd, t.x);
    //透明玻璃会有一定反射 漫反射定律
    alpha = true;
    vec3 rf = reflect(r, sn);
    vec3 tr = trace(w + sn * 0.01, rf);
    vec3 rw = w + rf * tr.x;
    vec2 rfd = tr.yz;
    vec3 rsn = normal(rw);
    vec4 rdiff = diffcol(rw, rf, rsn, rfd, t.x+tr.x);
    float prod = max(dot(r, -sn), 0.0);
    vec3 fdiff = mix(mdiff.xyz, rdiff.xyz, mdiff.w*(1.0-prod));
    vec3 fc = fdiff + laser(o, r);
    
    //gamma校正
    fragColor = vec4(sqrt(fc), 1.0);
}
// --------[ Original ShaderToy ends here ]---------- //

void main(void)
{
    mainImage(gl_FragColor, gl_FragCoord.xy);
}