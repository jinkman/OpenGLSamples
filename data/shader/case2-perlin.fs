#version 330 core
out float FragColor;

in vec2 TexCoords;
uniform int octaves;				 //��Ƶ
uniform float persistence;		 //���
uniform float delta;			 //ƽ����

//��άperlin����
float PI=3.1415926f;

float easeCurveInterpolate(float z1,float z2,float delta);
float smoothPoint(int x,int y);
float Noise(int x,int y);
float InterpolatedNoise(float x,float y);
float PerlinNoise(float x,float y);

float easeCurveInterpolate(float z1,float z2,float delta)//�������߲�ֵ������
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
	//���Ͻǵ�����(X0,Y0)
	int X0=int(x);
	int Y0=int(y);
	//�ڲ�ƫ����
	float dX=x-X0;
	float dY=y-Y0;
	//4�������ƽ��
	float z1=smoothPoint(X0,Y0);//����
	float z2=smoothPoint(X0+1,Y0);//����
	float z3=smoothPoint(X0,Y0+1);//����
	float z4=smoothPoint(X0+1,Y0+1);//����
	//���β�ֵ������easeCurveInterpolate���Ի���������ֵ����
	float _z1=easeCurveInterpolate(z1,z2,dX);//�������ֵ
	float _z2=easeCurveInterpolate(z3,z4,dX);//�������ֵ
	return easeCurveInterpolate(_z1,_z2,dY);//���ղ�ֵ
}


float PerlinNoise(float x,float y)
{
    float total=0.0;//ͳ�ƾ�����Ƶ����������ֵ
	for(int i=0;i<octaves;++i)//��Ƶѭ��
	{
		float frequency=pow(2.0,i);//����Ƶ�ʣ�Ƶ��Խ������Խ������
		float amplitude=pow(persistence,i);//������������Խ����������Խ��
		total=total+InterpolatedNoise(x*frequency*delta,y*frequency*delta)*amplitude;
	}
	return total;
}


void main()
{   
    float red = clamp(PerlinNoise(TexCoords.x*500,TexCoords.y*500),-1.0f,1.0f);
    FragColor = red/2+0.5f;
}