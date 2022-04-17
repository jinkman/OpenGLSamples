// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <math.h>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


//������ɫ��
const char *vertexShaderCode ="#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos, 1.0);\n"
	"}\0";

//Ƭ����ɫ��
const char *fragmentShaderCode = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec4 ourColor;\n"			 //ȫ�ֱ�����ɫ
	"void main()\n"
	"{\n"
	"   FragColor = ourColor;\n"		 //���ΪourColor
	"}\n\0";


int main()
{
	//��ʼ������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//����������
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ������ɫ��
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Ƭ����ɫ��
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// ������ɫ������
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// ���ɹ������
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);		//ɾ����ɫ��
	glDeleteShader(fragmentShader);		//ɾ����ɫ��

	// ���ö���
	float vertices[] = {
		0.5f, 0.5f, 0.0f,   // ����
		-0.5f, 0.5f, 0.0f,  // ����
		0.0f,  -0.5f, 0.0f  // �� 
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// �󶨶����������
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	  
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//��ȫ�����
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 


	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		// ����
		processInput(window);

		// �����ɫ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ʹ����ɫ��
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);	   	//�󶨶�������

		// ������ɫ����ɫ
		float timeValue = (float)glfwGetTime();		 //��������ʱ�䣨������
		float greenValue = sin(timeValue) / 2.0f + 0.5f;
		float redValue = cos(timeValue) / 2.0f + 0.5f;
		float blueValue = cos(timeValue) / 2.0f + 0.5f;
		// ��������
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);

		// ����ͼ��
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// ˫���弰��Ϣ����
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ��ȫ����󶨶���
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// �ͷ�glfw��Դ
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




