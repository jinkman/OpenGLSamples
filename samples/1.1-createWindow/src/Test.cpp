// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include<iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
using namespace std;


void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


int main(int argc, char *argv[])
{
	//ʵ����GLFW����
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//��������
	GLFWwindow* window = glfwCreateWindow(800, 600, "FirstWindow", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" <<endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	//��ô���������
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	//ע�ᴰ�ڻص�����

	//����OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" <<endl;
		return -1;
	}   

	
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))   //����Ƿ�Ҫ���˳�
	{
		processInput(window);	//���뷴Ӧ����

		//��Ⱦָ��
		glClearColor(0.2f, 0.5f, 0.3f, 1.0f);	//���ñ���ɫ
		glClear(GL_COLOR_BUFFER_BIT);			//�����ɫ����


		glfwSwapBuffers(window);  //������ɫ����
		glfwPollEvents();		  //��鲢�����¼�
	}
	glfwTerminate();			  //�ͷ���Դ
	return 0;
}

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
