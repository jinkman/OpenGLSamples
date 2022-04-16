// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <GL/glad.h>
#include <gl/glfw3.h>
#include <math.h>
#include "camera.h"
#include "shader_s.h"
#include "stb_image.h"


// 屏幕
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
#define PI 3.1415926

//相机
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

//函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void rendObject();
void LeftMultiMatrix(double M0[][4],glm::vec3 P0[][4]);//左乘矩阵M*P
void RightMultiMatrix(glm::vec3 P0[][4],double M1[][4]);//右乘矩阵P*M
void TransposeMatrix(double M0[][4]);//转置矩阵
void readQuadPoint(std::vector<float> &Arr);
unsigned int objectVAO=0, objectVBO;
glm::vec3  P3[4][4];//三维顶点
double MT[4][4];//M的转置矩阵

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建全屏
	bool isFullScreen = false;
	GLFWwindow* window = NULL;
	if (isFullScreen)
	{
		const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());//获取当前设备的一些属性
		SCR_WIDTH=vidmode->width;
		SCR_HEIGHT=vidmode->height;
		GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", pMonitor, NULL);
	}
	else
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//获取设备上下文
	glfwMakeContextCurrent(window);
	//注册回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	   //禁用鼠标

	//加载函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 开启OpenGL状态
	glEnable(GL_DEPTH_TEST);
	//使用着色器
	Shader cloudShader("cloud.vs","cloud.fs");

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//处理外部输入
		processInput(window);

		//清除缓存
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;

		cloudShader.use();
		model=glm::mat4();
		model=glm::translate(model,glm::vec3(0.0f,-300.0f,0.0));
		model=glm::scale(model,glm::vec3(1000));
		cloudShader.setMat4("projection", projection);
		cloudShader.setMat4("view", view);
		cloudShader.setMat4("model", model);
		cloudShader.setVec3("uSkyColor",glm::vec3(98/255.0f,156/255.0f,223/255.0f));
		cloudShader.setVec3("uCloudColor",glm::vec3(0.8,0.8,0.8));
		cloudShader.setFloat("uCloudSize",30.0f);
		cloudShader.setFloat("uTime",glfwGetTime()/5.0f);
		rendObject();

		//交换缓冲与消息分配
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//删除对象及释放资源
	glDeleteVertexArrays(1, &objectVAO);
	glDeleteBuffers(1, &objectVBO);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	static float speed=10.0f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS )
		camera.ProcessKeyboard(BACKWARD, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS )
		camera.ProcessKeyboard(LEFT, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		speed+=1.0f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		speed-=1.0f;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_WIDTH=width;
	SCR_HEIGHT=height;
}

void rendObject()
{
	static int vertextNum = 0;
	if(objectVAO==0)
	{
		std::vector<float> Arr;
		readQuadPoint(Arr);
		vertextNum = Arr.size();
		if (vertextNum == 0)
			return ;
		glGenVertexArrays(1, &objectVAO);
		glGenBuffers(1, &objectVBO);
		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &Arr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
		Arr.clear();
	}
	glBindVertexArray(objectVAO);
	glDrawArrays(GL_TRIANGLES, 0, vertextNum / 5); // 绘制实例
	glBindVertexArray(0);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


void readQuadPoint(std::vector<float> &Arr)
{
	
	//初始化控制点
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
			P3[i][j]=glm::vec3(i-1.5, 0.0, j-1.5);
	P3[1][1].y=1.0;
	P3[1][2].y=1.0;
	P3[2][1].y=1.0;
	P3[2][2].y=1.0;

	//计算所有点
	double x,y,z,u,v,u1,u2,u3,u4,v1,v2,v3,v4;
	double M[4][4];
	M[0][0]=-1;M[0][1]=3; M[0][2]=-3;M[0][3]=1;
	M[1][0]=3; M[1][1]=-6;M[1][2]=3; M[1][3]=0;
	M[2][0]=-3;M[2][1]=3; M[2][2]=0; M[2][3]=0;
	M[3][0]=1; M[3][1]=0; M[3][2]=0; M[3][3]=0;
	LeftMultiMatrix(M,P3);//数字矩阵左乘三维点矩阵
	TransposeMatrix(M);//计算转置矩阵
	RightMultiMatrix(P3,MT);//数字矩阵右乘三维点矩阵


	int index =0;
	glm::vec3 newP[25],oldP[25];
	for(u=0;u<=1;u+=0.04)
	{	
		index=0;
		for(v=0;v<=1;v+=0.04)
		{
			u1=u*u*u;u2=u*u;u3=u;u4=1;v1=v*v*v;v2=v*v;v3=v;v4=1;
			x=(u1*P3[0][0].x+u2*P3[1][0].x+u3*P3[2][0].x+u4*P3[3][0].x)*v1
				+(u1*P3[0][1].x+u2*P3[1][1].x+u3*P3[2][1].x+u4*P3[3][1].x)*v2
				+(u1*P3[0][2].x+u2*P3[1][2].x+u3*P3[2][2].x+u4*P3[3][2].x)*v3
				+(u1*P3[0][3].x+u2*P3[1][3].x+u3*P3[2][3].x+u4*P3[3][3].x)*v4;
			y=(u1*P3[0][0].y+u2*P3[1][0].y+u3*P3[2][0].y+u4*P3[3][0].y)*v1
				+(u1*P3[0][1].y+u2*P3[1][1].y+u3*P3[2][1].y+u4*P3[3][1].y)*v2
				+(u1*P3[0][2].y+u2*P3[1][2].y+u3*P3[2][2].y+u4*P3[3][2].y)*v3
				+(u1*P3[0][3].y+u2*P3[1][3].y+u3*P3[2][3].y+u4*P3[3][3].y)*v4;
			z=(u1*P3[0][0].z+u2*P3[1][0].z+u3*P3[2][0].z+u4*P3[3][0].z)*v1
				+(u1*P3[0][1].z+u2*P3[1][1].z+u3*P3[2][1].z+u4*P3[3][1].z)*v2
				+(u1*P3[0][2].z+u2*P3[1][2].z+u3*P3[2][2].z+u4*P3[3][2].z)*v3
				+(u1*P3[0][3].z+u2*P3[1][3].z+u3*P3[2][3].z+u4*P3[3][3].z)*v4;
			newP[index]=glm::vec3(x,y,z);
			index++;
		}
		if (u>0.0)
		{
			for(int i=0;i<index-1;i++)
			{
				//第一个三角形
				Arr.push_back(newP[i].x);
				Arr.push_back(newP[i].y);
				Arr.push_back(newP[i].z);
				Arr.push_back(u);
				Arr.push_back(i*0.04);
				Arr.push_back(oldP[i].x);
				Arr.push_back(oldP[i].y);
				Arr.push_back(oldP[i].z);
				Arr.push_back(u-0.04);
				Arr.push_back(i*0.04);
				Arr.push_back(oldP[i+1].x);
				Arr.push_back(oldP[i+1].y);
				Arr.push_back(oldP[i+1].z);
				Arr.push_back(u-0.04);
				Arr.push_back((i+1)*0.04);
				//第二个三角形
				Arr.push_back(oldP[i+1].x);
				Arr.push_back(oldP[i+1].y);
				Arr.push_back(oldP[i+1].z);
				Arr.push_back(u-0.04);
				Arr.push_back((i+1)*0.04);
				Arr.push_back(newP[i+1].x);
				Arr.push_back(newP[i+1].y);
				Arr.push_back(newP[i+1].z);
				Arr.push_back(u);
				Arr.push_back((i+1)*0.04);
				Arr.push_back(newP[i].x);
				Arr.push_back(newP[i].y);
				Arr.push_back(newP[i].z);
				Arr.push_back(u);
				Arr.push_back(i*0.04);
			}
		}
		for(int i=0;i<index;i++)
			oldP[i]=newP[i];
	}
}

void LeftMultiMatrix(double M0[][4],glm::vec3 P0[][4])//左乘矩阵M*P
{
	glm::vec3 T[4][4];//临时矩阵
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
		{	
			T[i][j].x=M0[i][0]*P0[0][j].x+M0[i][1]*P0[1][j].x+M0[i][2]*P0[2][j].x+M0[i][3]*P0[3][j].x;	
			T[i][j].y=M0[i][0]*P0[0][j].y+M0[i][1]*P0[1][j].y+M0[i][2]*P0[2][j].y+M0[i][3]*P0[3][j].y;
			T[i][j].z=M0[i][0]*P0[0][j].z+M0[i][1]*P0[1][j].z+M0[i][2]*P0[2][j].z+M0[i][3]*P0[3][j].z;
		}
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				P3[i][j]=T[i][j];
}

void RightMultiMatrix(glm::vec3 P0[][4],double M1[][4])//右乘矩阵P*M
{
	glm::vec3 T[4][4];//临时矩阵
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
		{	
			T[i][j].x=P0[i][0].x*M1[0][j]+P0[i][1].x*M1[1][j]+P0[i][2].x*M1[2][j]+P0[i][3].x*M1[3][j];
			T[i][j].y=P0[i][0].y*M1[0][j]+P0[i][1].y*M1[1][j]+P0[i][2].y*M1[2][j]+P0[i][3].y*M1[3][j];
			T[i][j].z=P0[i][0].z*M1[0][j]+P0[i][1].z*M1[1][j]+P0[i][2].z*M1[2][j]+P0[i][3].z*M1[3][j];
		}
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				P3[i][j]=T[i][j];
}

void TransposeMatrix(double M0[][4])//转置矩阵
{
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			MT[j][i]=M0[i][j];
}

