// Test.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <glad/glad.h>
#include <GL/glfw3.h>
#include<gl/glut.h>
#include "stb_image.h"
#include "camera.h"
#include "shader_s.h"
#include<math.h>

// 屏幕
#define PI 3.1415926
const unsigned int  SCR_WIDTH = 800, SCR_HEIGHT = 600;

// 函数声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void readVertext(std::vector<float> &Arr);
void Rotatez(glm::vec3 &a,double Thta);
unsigned int loadTexture(char const * path);
void RenderScene(Shader &shader);
void RenderSphere();
void RenderCube();


// 相机
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// 时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 选项
bool shadows = true;
bool change = true;

// 全局变量
unsigned int woodTexture;
unsigned int  sphereVAO = 0, sphereVBO;
unsigned int  cubeVAO = 0, cubeVBO;


int main()
{
	// 初始化glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//创建窗口-
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//注册回调函数
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// 捕获鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 加载glad库
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 设置视点
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// 建立着色器
	Shader shader("3.2.2.point_shadows.vs", "3.2.2.point_shadows.fs");
	Shader simpleDepthShader("3.2.2.point_shadows_depth.vs", "3.2.2.point_shadows_depth.fs", "3.2.2.point_shadows_depth.gs");
	Shader sphereShader("Sphere.vs", "Sphere.fs");


	// 设置纹理样本
	sphereShader.use();
	sphereShader.setInt("SphereMap",0);
	shader.use();
	shader.setInt("diffuseTexture",0);
	shader.setInt("shadowMap",1);

	// 加载纹理
	woodTexture = loadTexture("瓷器纹理.jpg");

	// c配置深度贴图帧缓冲
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// 创建深度立方体贴图纹理
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// 把纹理添加到帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 着色器配置
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("depthMap", 1);
	sphereShader.use();
	sphereShader.setInt("SphereMap",0);

	// l光照位置
	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);


	// 循环
	while (!glfwWindowShouldClose(window))
	{
		// 每帧逻辑时间
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 键入
		processInput(window);

		// 移动光照
		if (change)
			lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;

		// 渲染
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 创建深度贴图变换矩阵
		float near_plane = 1.0f;
		float far_plane = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		// 渲染深度立方体
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		simpleDepthShader.use();
		for (unsigned int i = 0; i < 6; ++i)
			simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(long long(i)) + "]", shadowTransforms[i]);
		simpleDepthShader.setFloat("far_plane", far_plane);
		simpleDepthShader.setVec3("lightPos", lightPos);
		RenderScene(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 渲染默认帧
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		// s设置光照信息
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", camera.Position);
		shader.setInt("shadows", shadows); 
		shader.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		RenderScene(shader); 


		//渲染光源
		sphereShader.use();
		glm::mat4 model;
		model = glm::translate(model, lightPos);
		model = glm::scale(model,glm::vec3(0.2,0.2,0.2));
		sphereShader.setMat4("model",model);
		sphereShader.setMat4("projection",projection);
		sphereShader.setMat4("view",view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		RenderSphere();


		// 交换缓冲与消息分配
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//资源释放
	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &sphereVBO);
	glDeleteBuffers(1, &cubeVBO);
	glfwTerminate();
	return 0;
}

void RenderScene(Shader &shader)
{
	// 渲染房子  照亮为背面 特殊处理（关闭背面剔除）
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(5.0f));
	shader.setMat4("model", model);
	glDisable(GL_CULL_FACE); 
	shader.setInt("reverse_normals", 1); 
	RenderCube();
	shader.setInt("reverse_normals", 0); 
	glEnable(GL_CULL_FACE);
	// 小立方体
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.75f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
	shader.setMat4("model", model);
	RenderCube();
}


void RenderCube()
{
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
			// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,      
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,  
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// 填充缓冲
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 连接顶点属性
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void RenderSphere()
{
	static int vertextNum = 0;
	if(sphereVAO == 0)
	{
		std::vector<float> Arr;
		readVertext(Arr);
		vertextNum = Arr.size();
		if (vertextNum == 0)
			return ;
		glGenVertexArrays(1, &sphereVAO);
		glGenBuffers(1, &sphereVBO);
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &Arr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindVertexArray(0);
	}
	//绑定纹理
	glBindVertexArray(sphereVAO);
	glDrawArrays(GL_TRIANGLES, 0, vertextNum/5); // 绘制
	glBindVertexArray(0);
}

// 渲染立方体
void readVertext(std::vector<float> &Arr)
{
	glm::vec3 cpt[4];
	for(double Theta=0;Theta<180;Theta+=1.0)
	{
		glm::vec3 p1(sin(Theta*PI/180.0),cos(Theta*PI/180.0),0);
		glm::vec3 p2(sin((Theta+1.0)*PI/180.0),cos((Theta+1.0)*PI/180.0),0);
		for(double Phi=0;Phi<360;Phi+=1.0)
		{
			cpt[0].x=p1.x;cpt[0].y=p1.y;cpt[0].z=p1.z;
			Rotatez(p1,1.0);
			cpt[1].x=p1.x;cpt[1].y=p1.y;cpt[1].z=p1.z;
			cpt[2].x=p2.x;cpt[2].y=p2.y;cpt[2].z=p2.z;
			Rotatez(p2,1.0);
			cpt[3].x=p2.x;cpt[3].y=p2.y;cpt[3].z=p2.z;
			//第一个三角形  132
			Arr.push_back(cpt[1].x);
			Arr.push_back(cpt[1].y);
			Arr.push_back(cpt[1].z);
			Arr.push_back(1-(Phi+1.0)/360.0);
			Arr.push_back(Theta/180.0);
			Arr.push_back(cpt[3].x);
			Arr.push_back(cpt[3].y);
			Arr.push_back(cpt[3].z);
			Arr.push_back(1.0-(Phi+1.0)/360.0);
			Arr.push_back((Theta+1.0)/180.0);
			Arr.push_back(cpt[2].x);
			Arr.push_back(cpt[2].y);
			Arr.push_back(cpt[2].z);
			Arr.push_back(1.0-Phi/360.0);
			Arr.push_back((Theta+1.0)/180.0);
			//第一个三角形  102
			Arr.push_back(cpt[1].x);
			Arr.push_back(cpt[1].y);
			Arr.push_back(cpt[1].z);
			Arr.push_back(1.0-(Phi+1.0)/360.0);
			Arr.push_back(Theta/180.0);
			Arr.push_back(cpt[2].x);
			Arr.push_back(cpt[2].y);
			Arr.push_back(cpt[2].z);
			Arr.push_back(1.0-Phi/360.0);
			Arr.push_back((Theta+1.0)/180.0);
			Arr.push_back(cpt[0].x);
			Arr.push_back(cpt[0].y);
			Arr.push_back(cpt[0].z);
			Arr.push_back(1.0-Phi/360.0);
			Arr.push_back(Theta/180.0);
		}
	}
}

//绕y轴旋转
void Rotatez(glm::vec3 &a,double Thta)									
{
	double a1=a.z;			
	double b1=a.x;
	a.x=b1*cos(Thta*PI/180.0)-a1*sin(Thta*PI/180.0);
	a.z=b1*sin(Thta*PI/180.0)+a1*cos(Thta*PI/180.0);
}


unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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


void processInput(GLFWwindow *window)
{
	static bool shadowSwitch = false;
	static bool moveSwitch = false;
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && shadowSwitch == false)
	{
		shadowSwitch = true;
		shadows = !shadows;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && shadowSwitch == true)
		shadowSwitch = false;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && moveSwitch == false)
	{
		moveSwitch = true;
		change = !change;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && moveSwitch == true)
		moveSwitch = false;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
