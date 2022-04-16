// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"	
#include <Windows.h>
#include <GL/glad.h>
#include <gl/glfw3.h>
#include <gl/glut.h>
#include <math.h>
#include "camera.h"
#include "shader_s.h"
#include "stb_image.h"


// ��Ļ
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
#define PI 3.1415926


// ʱ��
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
// ���
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 5.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

//��������
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void rendObject();
void readVertext(std::vector<float> &Arr);
unsigned int loadTexture(char const * path);
void Rotatez(glm::vec3 &a,double Thta);


unsigned int objectVAO=0, objectVBO;


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//����ȫ��
	bool isFullScreen = false;
	GLFWwindow* window = NULL;
	if (isFullScreen)
	{
		const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());//��ȡ��ǰ�豸��һЩ����
		SCR_WIDTH=vidmode->width;
		SCR_HEIGHT=vidmode->height;
		GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", pMonitor, NULL);
		lastX=SCR_WIDTH/2;
		lastY=SCR_HEIGHT/2;			  //��Ļ������
	}
	else
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//��ȡ�豸������
	glfwMakeContextCurrent(window);
	//ע��ص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//��������ģʽ
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	   //�������

	//���غ���ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ����OpenGL״̬
	glEnable(GL_DEPTH_TEST);
	//ʹ����ɫ��
	Shader grassShader("grass.vs","grass.fs","grass.gs");
	grassShader.use();
	grassShader.setInt("diffuseMap",0);

	
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//ÿ֡����߼�ʱ��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//�����ⲿ����
		processInput(window); 
		//�������
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//��ȡ�任����
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;

		//��Ⱦ��
		grassShader.use();
		grassShader.setMat4("projection", projection);
		grassShader.setMat4("view", view);
		grassShader.setFloat("g",30.0f); //�������ٶ�
		grassShader.setFloat("size",1.0f);
		grassShader.setInt("pre",20); //����
		grassShader.setFloat("uTimes",glfwGetTime()*1.2f);				
		grassShader.setVec4("grassColor",glm::vec4(0.0,0.7,0.0,0.0));				
		//��������
		Sleep(60);
		rendObject();

		//������������Ϣ����
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//ɾ�������ͷ���Դ
	glDeleteVertexArrays(1, &objectVAO);
	glDeleteBuffers(1, &objectVBO);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	static float speed=1.0f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime*speed);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		speed+=0.01f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		speed-=0.01f;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);
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
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


void rendObject()
{
	static unsigned int grassMap = loadTexture("��.png");
	static int vertextNum = 0;
	static unsigned int amount = 10000;
	if(objectVAO==0)
	{
		std::vector<float> vArr;
		readVertext(vArr);
		vertextNum = vArr.size();
		if (vertextNum == 0)
			return ;
		// ���ô������ı任����
		glm::mat4 *modelMatrices = new glm::mat4[amount];
		float radius = 100.0f;
		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 model;
			float x = rand()%(int(20*radius))/10.0f-radius;
			float height = sqrt(pow(radius,2.0f)-pow(x,2.0f)); 
			float z = rand()%(int(20*height)+1)/10.0f-height;
			model=glm::translate(model,glm::vec3(x,0.0f,z));
			model=glm::rotate(model,glm::radians(float(rand()%3600/10.0f)),glm::vec3(0.0f,1.0f,0.0f));
			modelMatrices[i]=model;
		}

		// ��������
		unsigned int modelBuffer;
		glGenBuffers(1, &modelBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		delete []modelMatrices;
		modelMatrices=NULL;

		//�����ٶ�����
		glm::vec3* speedMatrices;
		speedMatrices = new glm::vec3[amount];
		for (unsigned int i = 0; i < amount; i++)
			speedMatrices[i]=glm::vec3(20.0f+rand()%3000/1000.0f,3.5+rand()%300/100.0f,rand()%1000/100.0f+100.0f);
		// ��������
		unsigned int speedBuffer;
		glGenBuffers(1, &speedBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, speedBuffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::vec3), &speedMatrices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		delete []speedMatrices;
		speedMatrices=NULL;

		glGenVertexArrays(1, &objectVAO);
		glGenBuffers(1, &objectVBO);
		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &vArr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(2, 1);			  //ʵ��һ�θı�һ��
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);

		glBindBuffer(GL_ARRAY_BUFFER, speedBuffer);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glVertexAttribDivisor(6, 1);			  //ʵ��һ�θı�һ��
		glBindVertexArray(0);
		vArr.clear();
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassMap);
	glBindVertexArray(objectVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertextNum/5, amount); // ����ʵ��
	glBindVertexArray(0);
}


//���붥������
void readVertext(std::vector<float> &Arr)
{
	float times = 10.0f;
	float heightTimes =5.0f;
	float space = 3.0f/2;
	//����x:-1.5 1.5
	//��һ��������
	Arr.push_back(-1.5f*times);
	Arr.push_back(0.0f);
	Arr.push_back((0.5*space-1.5f)*times);
	Arr.push_back(0.0f);
	Arr.push_back(0.0f);

	Arr.push_back(1.5f*times);
	Arr.push_back(0.0f);
	Arr.push_back((0.5*space-1.5f)*times);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);

	Arr.push_back(-1.5f*times);
	Arr.push_back(3.0f*heightTimes);
	Arr.push_back((0.5*space-1.5f)*times);
	Arr.push_back(0.0f);
	Arr.push_back(1.0f);
	//�ڶ���������
	Arr.push_back(1.5f*times);
	Arr.push_back(0.0f);
	Arr.push_back((0.5*space-1.5f)*times);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);

	Arr.push_back(1.5f*times);
	Arr.push_back(3.0f*heightTimes);
	Arr.push_back((0.5*space-1.5f)*times);
	Arr.push_back(1.0f);
	Arr.push_back(1.0f);

	Arr.push_back(-1.5f*times);
	Arr.push_back(3.0f*heightTimes);
	Arr.push_back((0.5*space-1.5f)*times);
	Arr.push_back(0.0f);
	Arr.push_back(1.0f);
}




//��������
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

