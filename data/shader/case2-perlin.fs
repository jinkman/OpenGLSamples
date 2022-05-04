#version 330 core
out float FragColor;

in vec2 TexCoords;
uniform int octaves;				 //倍频
uniform float persistence;		 //振幅
uniform float delta;			 //平滑度


float PI=3.1415926f;

float easeCurveInterpolate(float z1,float z2,float delta);
float smoothPoint(int x,int y);
float Noise(int x,int y);
float InterpolatedNoise(float x,float y);
float PerlinNoise(float x,float y);

float easeCurveInterpolate(float z1,float z2,float delta)//缓动曲线插值，常用
{
	float percent=(1- cos(delta * PI))*0.5;
	return z1*(1-percent)+z2*percent;
}																				   


float smoothPoint(int x,int y)
{
	float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
	float sides = ( Noise(x-1, y) +Noise(x+1, y) +Noise(x, y-1) +Noise(x, y+1) ) / 8;
	float center = Noise(x, y) / 4;
	return corners + sides + center;
}


float Noise(int x,int y)
{   
	int n = x + y * 57;
	n = (n<<13) ^ n;
	return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float InterpolatedNoise(float x,float y)
{
	//左上角点坐标(X0,Y0)
	int X0=int(x);
	int Y0=int(y);
	//内部偏移量
	float dX=x-X0;
	float dY=y-Y0;
	//4个点进行平滑
	float z1=smoothPoint(X0,Y0);//左上
	float z2=smoothPoint(X0+1,Y0);//右上
	float z3=smoothPoint(X0,Y0+1);//左下
	float z4=smoothPoint(X0+1,Y0+1);//右下
	//两次插值，其中easeCurveInterpolate可以换成其他插值函数
	float _z1=easeCurveInterpolate(z1,z2,dX);//上两点插值
	float _z2=easeCurveInterpolate(z3,z4,dX);//下两点插值
	return easeCurveInterpolate(_z1,_z2,dY);//最终插值
}


float PerlinNoise(float x,float y)
{
    float total=0.0;//统计经过倍频处理后的噪声值
	for(int i=0;i<octaves;++i)//倍频循环
	{
		float frequency=pow(2.0,i);//计算频率，频率越大，噪声越不规则
		float amplitude=pow(persistence,i);//计算振幅，振幅越大，噪声波动越大
		total=total+InterpolatedNoise(x*frequency*delta,y*frequency*delta)*amplitude;
	}
	return total;
}


void main()
{   
    float red = clamp(PerlinNoise(TexCoords.x*500,TexCoords.y*500),-1.0f,1.0f);
    FragColor = red/2+0.5f;
}