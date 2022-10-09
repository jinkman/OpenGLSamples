#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;


void main()
{  
    //Standardized coordinate system
    vec2 uv = (gl_FragCoord.xy/resolution.xy)-0.5;
    //uv*=0.01;
    //The farther away you are, the bigger the Angle
    float time = time * 0.1 + ((0.25+0.05*sin(time*0.1))/(length(uv.xy)*2+0.07))* 2.2; 
    float si = sin(time);
    float co = cos(time);
    mat2 ma = mat2(co, si, -si, co);

    float c = 0.0;
    float v1 = 0.0;
    float v2 = 0.0;

    for (int i = 0; i < 100; i++)
    {
        float s = float(i) * 0.035;
        vec3 p = s * vec3(uv, 0.0);
        p.xy *= ma;
        p += vec3(0.22,0.3, s-1.5-sin(time*.13)*0.1);
        for (int i = 0; i < 8; i++)
        {
            p = abs(p) / dot(p,p) - 0.659;
        }
        v1 += dot(p,p)*.0015 * (1.8+sin(length(uv.xy*13.0)+.5-time*.2));
        v2 += dot(p,p)*.0015 * (1.5+sin(length(uv.xy*13.5)+2.2-time*.3));
        c = length(p.xy*.5) * .35;
    }

    float len = length(uv);
    v1 *= smoothstep(.7, .0, len);
    v2 *= smoothstep(.6, .0, len);

    float re = clamp(c, 0.0, 1.0);
    float gr = clamp((v1+c)*.25, 0.0, 1.0);
    float bl = clamp(v2, 0.0, 1.0);
    vec3 col = vec3(re, gr, bl) + smoothstep(0.15, .0, len) * .9;
    FragColor=vec4(col, 1.0);
}