// Test.cpp : 定义控制台应用程序的入口点。
//

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <shader_s.h>
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <common.h>


void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// 屏幕大小
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	//初始化窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//加载函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//使用着色器类
	Shader ourShader(getLocalPath("shader/1.4-texture.vs").c_str(), getLocalPath("shader/1.4-texture.fs").c_str());

	float vertices[] = {
		// 位置              // 纹理坐标
		0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // 右上
		0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // 右下
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // 左下
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // 左上 
	};
	unsigned int indices[] = {
		0, 1, 3, 
		1, 2, 3 
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);		//生成顶点数组
	glGenBuffers(1, &VBO);	 //生成顶点缓冲
	glGenBuffers(1, &EBO);	 //生成顶点索引

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 纹理位置
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//设置纹理
	unsigned int texture1;
	glGenTextures(1, &texture1);   //生成纹理
	glBindTexture(GL_TEXTURE_2D, texture1);    //绑定纹理
	//设置纹理属性
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);    //处理倒置
	//加载纹理
	unsigned char *data = stbi_load(getLocalPath("texture/test.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);	 //释放缓存

	//激活着色器
	ourShader.use(); 
	//着色器采样器属于哪个纹理单元
	ourShader.setInt("texture1", 0);

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//清除颜色缓存
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		

		ourShader.use();
		//绘制
		glActiveTexture(GL_TEXTURE0);	//激活纹理通道
		glBindTexture(GL_TEXTURE_2D, texture1);	 //绑定纹理
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

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

