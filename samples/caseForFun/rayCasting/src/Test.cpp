// ����׷��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
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

//��������
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void rendObject();
void readVertext(std::vector<float> &Arr);
unsigned int loadTexture(char const * path);

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

	//���غ���ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ����OpenGL״̬
	glEnable(GL_DEPTH_TEST);
	//ʹ����ɫ��
	Shader shader("object.vs","object.fs");

	while (!glfwWindowShouldClose(window))
	{
		//�����ⲿ����
		processInput(window);
		// �߼�ʱ�����
		static float deltaTime = 0.0f;
		static float lastFrame = 0.0f;
		//����fps
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		static int fps =0;
		static float totalTime=0.0f;
		totalTime+=deltaTime;
		if((fps++)==7)
		{
			std::cout<<7/totalTime<<std::endl;
			totalTime=0.0f;
			fps=0;
		}

		//�������
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//������ɫ������
		shader.use();
		shader.setFloat("time",glfwGetTime());
		//��������
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	glViewport(0, 0, width, height);
}

void rendObject()
{
	static int vertextNum = 0;
	if(objectVAO==0)
	{
		std::vector<float> Arr;
		readVertext(Arr);
		vertextNum = Arr.size();
		if (vertextNum == 0)
			return ;
		glGenVertexArrays(1, &objectVAO);
		glGenBuffers(1, &objectVBO);
		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &Arr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(objectVAO);
	glDrawArrays(GL_TRIANGLES, 0, vertextNum/4); // ����
	glBindVertexArray(0);
}


//���붥������
void readVertext(std::vector<float> &Arr)
{
	Arr.push_back(-1.0f);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);
	Arr.push_back(1.0f);

	Arr.push_back(-1.0f);
	Arr.push_back(-1.0f);
	Arr.push_back(0.0f);
	Arr.push_back(0.0f);

	Arr.push_back(1.0f);
	Arr.push_back(-1.0f);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);

	Arr.push_back(-1.0f);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);
	Arr.push_back(1.0f);

	Arr.push_back(1.0f);
	Arr.push_back(-1.0f);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);

	Arr.push_back(1.0f);
	Arr.push_back(1.0f);
	Arr.push_back(1.0f);
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

