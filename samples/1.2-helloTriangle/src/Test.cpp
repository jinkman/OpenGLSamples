// Test.cpp : 定义控制台应用程序的入口点。
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// 着色器
const char *vertexShaderCode = "#version 330 core\n"
							   "layout (location = 0) in vec3 aPos;\n"
							   "void main()\n"
							   "{\n"
							   "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
							   "}\0";
const char *fragmentShaderCode = "#version 330 core\n"
								 "out vec4 FragColor;\n"
								 "void main()\n"
								 "{\n"
								 "   FragColor = vec4(1.0f, 0.5f, 0.2f, 0.1f);\n"
								 "}\n\0";

int main()
{
	// 初始化glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 设置窗口大小更改回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 加载glad函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 创建顶点着色器
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader); //编译着色器
	// 获取编译结果
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // 获取结果
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

	// 创建片段着色器
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

	// 创建着色程序 链接
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// 获取状态
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}

	// 删除资源
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 顶点属性
	float vertices[] = {
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f};

	// 顶点索引
	unsigned int indices[] = {
		0, 1, 3, // 第一个三角形
		1, 2, 3	 // 第二个三角形
	};
	unsigned int VBO, VAO, EBO;
	// 创建顶点数组
	glGenVertexArrays(1, &VAO);
	// 创建顶点缓冲
	glGenBuffers(1, &VBO);
	// 创建索引缓冲
	glGenBuffers(1, &EBO);

	// 绑定顶点数组
	glBindVertexArray(VAO);

	// 绑定顶点缓冲并传入顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 绑定索引缓冲并传入索引数据
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 顶点缓冲属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// 绑定回默认vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		processInput(window); // 处理输入

		// 清空颜色缓冲区
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);						 // 使用着色器程序
		glBindVertexArray(VAO);								 // 绑定vao
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 通过索引的方式绘制

		glfwSwapBuffers(window); // 交换缓冲
		glfwPollEvents();		 // 分发消息
	}

	// 删除资源
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate(); // 释放资源
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// 窗口大小改变回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}