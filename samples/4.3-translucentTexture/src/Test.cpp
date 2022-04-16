// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <GL/glad.h>
#include <gl/glfw3.h>
#include <math.h>
#include "camera.h"
#include "shader_s.h"
#include "stb_image.h"
#include<map>


// 屏幕
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
#define PI 3.1415926

//相机
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
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
void rendFloor();
void rendTree();
unsigned int loadTexture(char const * path);

unsigned int objectVAO=0, objectVBO;
unsigned int floorVAO=0, floorVBO;
unsigned int treeVAO=0, treeVBO;

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

	// 配置全局OpenGL状态
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 建立着色器
	Shader shader("blending.vs", "blending.fs");

	shader.use();
	shader.setInt("texture1", 0);
	// 草丛位置 中心对称
	glm::vec3 vegetation[5];
	vegetation[0]=glm::vec3(-1.5f, 0.0f, -0.48f);
	vegetation[1]=glm::vec3( 1.5f, 0.0f, 0.51f);
	vegetation[2]=glm::vec3( 0.0f, 0.0f, 0.7f);
	vegetation[3]=glm::vec3(-0.3f, 0.0f, -2.3f);
	vegetation[4]=glm::vec3 (0.5f, 0.0f, -0.6f);


	while (!glfwWindowShouldClose(window))
	{
		// 调整速度
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 检测输入
		processInput(window);
		// 渲染背景色
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model;
		shader.use();
		shader.setMat4("projection",projection);
		shader.setMat4("view",view);
		rendFloor();

		for (int i=0;i<5;i++)
		{
			model = glm::mat4();
			model = glm::translate(model, vegetation[i]);
			shader.setMat4("model", model);
			rendTree();
		}

		//交换缓冲与消息分配
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//删除对象及释放资源
	glDeleteVertexArrays(1, &objectVAO);
	glDeleteBuffers(1, &objectVBO);
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
	glDeleteVertexArrays(1, &treeVAO);
	glDeleteBuffers(1, &treeVBO);
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
}

void rendTree()
{
	// 加载纹理
	static unsigned int diffuseMap = loadTexture("树.png");
	if(treeVAO==0)
	{
		float transparentVertices[] = {
			// 位置               //  纹理
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
			0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 
			0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 

			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
			0.5f, -0.5f, 0.0f,   1.0f, 0.0f 
		};
		glGenVertexArrays(1, &treeVAO);
		glGenBuffers(1, &treeVBO);
		glBindVertexArray(treeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}
	//激活纹理
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glBindVertexArray(treeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制
	glBindVertexArray(0);
}

void rendFloor()
{
	// 加载纹理
	static unsigned int diffuseMap = loadTexture("a.jpg");
	if(floorVAO==0)
	{
		float planeVertices[] = {
			// 位置          // 纹理
			5.0f, -1.0f,  5.0f,  1.0f, 0.0f,
			-5.0f, -1.0f,  5.0f,  0.0f, 0.0f,
			-5.0f, -1.0f, -5.0f,  0.0f, 1.0f,

			5.0f, -1.0f,  5.0f,  1.0f, 0.0f,
			-5.0f, -1.0f, -5.0f,  0.0f, 1.0f,
			5.0f, -1.0f, -5.0f,  1.0f, 1.0f
		};
		glGenVertexArrays(1, &floorVAO);
		glGenBuffers(1, &floorVBO);
		glBindVertexArray(floorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}
	//激活纹理
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制
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

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	stbi_set_flip_vertically_on_load(true);
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

