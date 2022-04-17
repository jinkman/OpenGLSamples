#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 170) out;

out vec2 TexCoords;

in VS_OUT {
    mat4 model;
    vec3 speed;
} gs_in[]; 

uniform mat4 projection;
uniform mat4 view;
uniform float uTimes;
uniform float g;
uniform float size;
uniform int pre;

float PI = 3.1415926;

mat4 T;
vec4 y = vec4(1.0);


void InitMat()
{
     T[0]=vec4(1.0f,0.0f,0.0f,0.0f);
     T[1]=vec4(0.0f,1.0f,0.0f,0.0f);
     T[2]=vec4(0.0f,0.0f,1.0f,0.0f);
     T[3]=vec4(0.0f,0.0f,0.0f,1.0f);
}
void RotateX(float beta)//绕X轴旋转变换矩阵
{    
     InitMat();
     float rad=beta*PI/180.0f;
     T[1]=vec4(0.0f,cos(rad),sin(rad),0.0f);
     T[2]=vec4(0.0f,-1*sin(rad),cos(rad),0.0f);
}

void Move(int index)
{    
     vec4 lup,ldown,rup,rdown;
     ldown = vec4(-0.5f,-1.0f,0.0f,1.0f); //位置坐标
     vec2 ldcoord = vec2(0.0f,0.0f);  //纹理坐标
     rdown = vec4(0.5f,-1.0f,0.0f,1.0f);
     vec2 rdcoord = vec2(1.0f,0.0f);
     lup = vec4(-0.5f,0.0f,0.0f,1.0f);
     vec2 lucoord = vec2(0.0f,1.0f);
     rup = vec4(0.5f,0.0f,0.0f,1.0f);
     vec2 rucoord = vec2(1.0f,1.0f);
	
     InitMat();
     RotateX(1.5 * sin(uTimes+gs_in[0].speed.z));
     mat4 model1 = gs_in[0].model * T;
     mat4 model2 = projection * view;
     for(int i=0;i<index;i++)
     { 
  	  //越靠近顶部精度越高
        float t = sqrt(i / float(index)); //（0,1）
        float t1 = sqrt((i + 1) / float(index)) - t;
	  //重新赋值
	  //偏移量
		float dY = (gs_in[0].speed.x * t - g * t * t / 2.0f) * size;
		float dZ = gs_in[0].speed.y * t;

		ldown = lup;
		rdown = rup;
        lup.y = dY;
		lup.z = dZ;
		rup.y = dY;
		rup.z = dZ;
		
	  //第一个三角形
		gl_Position = ldown;
		gl_Position = model1 * gl_Position + y;
		gl_Position = model2 * gl_Position;
		TexCoords = vec2(ldcoord.x,t);
		EmitVertex();
		gl_Position = rdown;
		gl_Position = model1 * gl_Position + y;
		gl_Position = model2 * gl_Position;
		TexCoords = vec2(rdcoord.x,t);
		EmitVertex();
		gl_Position = lup;
		gl_Position = model1 * gl_Position + y;
		gl_Position = model2 * gl_Position;
		TexCoords = vec2(lucoord.x,t + t1);
		EmitVertex();
	  //第2个三角形
		gl_Position = rdown;
		gl_Position = model1 * gl_Position + y;
		gl_Position = model2 * gl_Position;
		TexCoords = vec2(rdcoord.x,t);
		EmitVertex();
		gl_Position = rup;
		gl_Position = model1 * gl_Position + y;
		gl_Position = model2 * gl_Position;
		TexCoords = vec2(rucoord.x,t + t1);
		EmitVertex();
		gl_Position = lup;
		gl_Position = model1 * gl_Position + y;
		gl_Position = model2 * gl_Position;
		TexCoords = vec2(lucoord.x,t + t1);
		EmitVertex();
     }
}

void main() 
{   
    InitMat();
    Move(pre);
    EndPrimitive();
}



    
