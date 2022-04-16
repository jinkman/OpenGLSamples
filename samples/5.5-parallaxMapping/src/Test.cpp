// Test.cpp : �������̨Ӧ�ó������ڵ㡣
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


// ʱ��
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
// ���
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
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
bool ifNormal = true;
float heightScale = 0.1;
bool ifParallax = true;

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

	// ��ɫ��
	Shader shader("parallax_mapping.vs", "parallax_mapping.fs");

	// ��������
	unsigned int diffuseMap = loadTexture("bricks2.jpg");
	unsigned int normalMap = loadTexture("bricks2_normal.jpg");
	unsigned int heightMap = loadTexture("bricks2_disp.jpg");


	// ��������
	shader.use();
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);
	shader.setInt("depthMap", 2);

	// ��Դλ��
	glm::vec3 lightPos(3.0f, 0.0f, 3.0f);

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

		// ���ñ任����
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		// ��Ⱦ�Ӳ�ͼ
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show parallax mapping from multiple directions
		shader.setMat4("model", model);
		shader.setBool("ifParallax",ifParallax);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos);
		shader.setFloat("heightScale", heightScale); // adjust with Q and E keys
		std::cout << heightScale << std::endl;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heightMap);
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
	static bool KeyPressed = false;
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
	{
		if (heightScale > 0.0f)
			heightScale -= 0.0005f;
		else
			heightScale = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (heightScale < 1.0f)
			heightScale += 0.0005f;
		else
			heightScale = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !KeyPressed)
	{
		ifParallax = !ifParallax;
		KeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		KeyPressed = false;
	}
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
		Arr.clear();
	}
	//������
	glBindVertexArray(objectVAO);
	glDrawArrays(GL_TRIANGLES, 0, vertextNum/11); // ����
	glBindVertexArray(0);
}


//���붥������
void readVertext(std::vector<float> &Arr)
{
	for(double Theta=0;Theta<180;Theta+=1.0)
	{
		glm::vec3 p1(sin(Theta*PI/180.0),cos(Theta*PI/180.0),0);
		glm::vec3 p2(sin((Theta+1.0)*PI/180.0),cos((Theta+1.0)*PI/180.0),0);
		for(double Phi=0;Phi<360;Phi+=1.0)
		{
			glm::vec3 pos1;
			glm::vec3 pos2;
			glm::vec3 pos3;
			glm::vec3 pos4;
			glm::vec2 uv1;
			glm::vec2 uv2;
			glm::vec2 uv3;
			glm::vec2 uv4;
			pos1.x=p1.x;pos1.y=p1.y;pos1.z=p1.z;
			Rotatez(p1,1.0);
			pos4.x=p1.x;pos4.y=p1.y;pos4.z=p1.z;
			pos2.x=p2.x;pos2.y=p2.y;pos2.z=p2.z;
			Rotatez(p2,1.0);
			pos3.x=p2.x;pos3.y=p2.y;pos3.z=p2.z;
			uv1.x=1.0-Phi/360.0;
			uv1.y=Theta/180.0;
			uv4.x=1-(Phi+1.0)/360.0;
			uv4.y=Theta/180.0;
			uv2.x=1.0-Phi/360.0;
			uv2.y=(Theta+1.0)/180.0;
			uv3.x=1.0-(Phi+1.0)/360.0;
			uv3.y=(Theta+1.0)/180.0;
			//����
			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1, bitangent1;
			glm::vec3 tangent2, bitangent2;
			// - triangle 1
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent1 = glm::normalize(tangent1);

			bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent1 = glm::normalize(bitangent1);

			// - triangle 2
			edge1 = pos3 - pos1;
			edge2 = pos4 - pos1;
			deltaUV1 = uv3 - uv1;
			deltaUV2 = uv4 - uv1;

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent2 = glm::normalize(tangent2);


			bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent2 = glm::normalize(bitangent2);
			//��һ��������  123
			Arr.push_back(pos1.x);
			Arr.push_back(pos1.y);
			Arr.push_back(pos1.z);
			Arr.push_back(uv1.x);
			Arr.push_back(uv1.y);
			Arr.push_back(tangent1.x);
			Arr.push_back(tangent1.y);
			Arr.push_back(tangent1.z);
			Arr.push_back(bitangent1.x);
			Arr.push_back(bitangent1.y);
			Arr.push_back(bitangent1.z);

			Arr.push_back(pos2.x);
			Arr.push_back(pos2.y);
			Arr.push_back(pos2.z);
			Arr.push_back(uv2.x);
			Arr.push_back(uv2.y);
			Arr.push_back(tangent1.x);
			Arr.push_back(tangent1.y);
			Arr.push_back(tangent1.z);
			Arr.push_back(bitangent1.x);
			Arr.push_back(bitangent1.y);
			Arr.push_back(bitangent1.z);

			Arr.push_back(pos3.x);
			Arr.push_back(pos3.y);
			Arr.push_back(pos3.z);
			Arr.push_back(uv3.x);
			Arr.push_back(uv3.y);
			Arr.push_back(tangent1.x);
			Arr.push_back(tangent1.y);
			Arr.push_back(tangent1.z);
			Arr.push_back(bitangent1.x);
			Arr.push_back(bitangent1.y);
			Arr.push_back(bitangent1.z);

			//��һ��������  341
			Arr.push_back(pos3.x);
			Arr.push_back(pos3.y);
			Arr.push_back(pos3.z);
			Arr.push_back(uv3.x);
			Arr.push_back(uv3.y);
			Arr.push_back(tangent2.x);
			Arr.push_back(tangent2.y);
			Arr.push_back(tangent2.z);
			Arr.push_back(bitangent2.x);
			Arr.push_back(bitangent2.y);
			Arr.push_back(bitangent2.z);

			Arr.push_back(pos4.x);
			Arr.push_back(pos4.y);
			Arr.push_back(pos4.z);
			Arr.push_back(uv4.x);
			Arr.push_back(uv4.y);
			Arr.push_back(tangent2.x);
			Arr.push_back(tangent2.y);
			Arr.push_back(tangent2.z);
			Arr.push_back(bitangent2.x);
			Arr.push_back(bitangent2.y);
			Arr.push_back(bitangent2.z);

			Arr.push_back(pos1.x);
			Arr.push_back(pos1.y);
			Arr.push_back(pos1.z);
			Arr.push_back(uv1.x);
			Arr.push_back(uv1.y);
			Arr.push_back(tangent2.x);
			Arr.push_back(tangent2.y);
			Arr.push_back(tangent2.z);
			Arr.push_back(bitangent2.x);
			Arr.push_back(bitangent2.y);
			Arr.push_back(bitangent2.z);

		}
	}
}

//��y����ת
void Rotatez(glm::vec3 &a,double Thta)									
{
	double a1=a.z;			
	double b1=a.x;
	a.x=b1*cos(Thta*PI/180.0)-a1*sin(Thta*PI/180.0);
	a.z=b1*sin(Thta*PI/180.0)+a1*cos(Thta*PI/180.0);
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