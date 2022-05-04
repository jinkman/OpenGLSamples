// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

//��ɫ��Դ��
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
	//ʵ��������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//��������
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//���ڴ�С�ı�ص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//����glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//����������ɫ������
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL); //����ɫ��Դ�븽�ӵ���ɫ��������
	glCompileShader(vertexShader);							  //������ɫ��
	//�Ƿ����ɹ� ���洢����
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //��ȡ������Ϣ
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

	// Ƭ����ɫ�� ������Ⱦ������
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

	// ��������ɫ������ɫ����
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// ��������Ƿ����
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}

	//ɾ����ɫ������
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//��������
	float vertices[] = {
		0.5f, 0.5f, 0.0f,	// ����
		0.5f, -0.5f, 0.0f,	// ����
		-0.5f, -0.5f, 0.0f, // ����
		-0.5f, 0.5f, 0.0f	// ����
	};
	//��������
	unsigned int indices[] = {
		0, 1, 3, // ��һ��������
		1, 2, 3	 // �ڶ���������
	};
	unsigned int VBO, VAO, EBO;
	//�����������
	glGenVertexArrays(1, &VAO);
	//���㻺�����
	glGenBuffers(1, &VBO);
	//�����������
	glGenBuffers(1, &EBO);

	//�󶨶����������
	glBindVertexArray(VAO);

	//���ƶ������鵽�����й�OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//���������Ƶ�������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//���ö�������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	//��ȫ�����
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		processInput(window); //ϵͳ����

		//�����ɫ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);						 //ʹ����ɫ������
		glBindVertexArray(VAO);								 //�󶨶�������
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //ʹ�õ�ǰ�󶨵�������������е��������л���  6������

		glfwSwapBuffers(window); //����˫����
		glfwPollEvents();		 //�ַ���Ϣ
	}

	//ɾ���󶨵�1������
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate(); //�ͷ���Դ
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//���ڻص�����
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}