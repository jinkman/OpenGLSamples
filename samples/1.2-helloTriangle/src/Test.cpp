// Test.cpp : 定义控制台应用程序的入口点。
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

//着色器源码
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
	//实例化窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//窗口大小改变回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//加载glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//创建顶点着色器对象
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL); //把着色器源码附加到着色器对象上
	glCompileShader(vertexShader);							  //编译着色器
	//是否编译成功 并存储错误
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //获取编译信息
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

	// 片段着色器 用于渲染三角形
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

	// 链接众着色器于着色程序
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// 检查链接是否错误
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}

	//删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点坐标
	float vertices[] = {
		0.5f, 0.5f, 0.0f,	// 右上
		0.5f, -0.5f, 0.0f,	// 右下
		-0.5f, -0.5f, 0.0f, // 左下
		-0.5f, 0.5f, 0.0f	// 左上
	};
	//索引数据
	unsigned int indices[] = {
		0, 1, 3, // 第一个三角形
		1, 2, 3	 // 第二个三角形
	};
	unsigned int VBO, VAO, EBO;
	//顶点数组对象
	glGenVertexArrays(1, &VAO);
	//顶点缓冲对象
	glGenBuffers(1, &VBO);
	//索引缓冲对象
	glGenBuffers(1, &EBO);

	//绑定顶点数组对象
	glBindVertexArray(VAO);

	//复制顶点数组到缓冲中供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//把索引复制到缓冲里
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//设置顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	//安全解除绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		processInput(window); //系统输入

		//清除颜色缓存
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);						 //使用着色器程序
		glBindVertexArray(VAO);								 //绑定顶点数组
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //使用当前绑定的索引缓冲对象中的索引进行绘制  6个顶点

		glfwSwapBuffers(window); //交换双缓冲
		glfwPollEvents();		 //分发消息
	}

	//删除绑定的1个对象
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate(); //释放资源
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//窗口回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}