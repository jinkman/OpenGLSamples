// ������ӡ.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <GL/glad.h>
#include <gl/glfw3.h>
#include <math.h>
#include "camera.h"
#include "shader_s.h"
#include "stb_image.h"
using namespace std;
using namespace glm;


// ��Ļ
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
#define PI 3.1415926


// ʱ��
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
// ���
bool firstMouse = true;
Camera camera(vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

//��������
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void rendObject();
void readVertext();
void bindFace(int i,int texNum);
void readFace(vec3 p0,vec2 t0,vec3 p1,vec2 t1,vec3 p2,vec2 t2);
unsigned int loadTexture(char const * path);


vector<vec3> vArr;  //��������
vector<float> fArr;	  //�������
unsigned int stampVAO[26]={0}, stampVBO[26];
unsigned int textureVAO=0, textureVBO;
bool ifRotate = false;	 //�Ƿ���ת
float Angle = 0.0f;		 //��ת�Ƕ�
bool dir =false;		 //��ת����
bool light = true;      //����ģ��

unsigned int texIndex[26]; //����������
unsigned int vertextNum[26]; //������Ŀ
unsigned int diffuseMap[15]; //��������
unsigned int depthMap[15];  //�������
unsigned int normalMap[15];  //�������

float heightScale = 0.1f;

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
		cout << "Failed to create GLFW window" << endl;
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
		cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ����OpenGL״̬
	glEnable(GL_DEPTH_TEST);
	//ʹ����ɫ��
	Shader depthShader("�����ļ�/depthBuffer.vs","�����ļ�/depthBuffer.fs");
	Shader normalShader("�����ļ�/normalBuffer.vs","�����ļ�/normalBuffer.fs");
	Shader stampShader("�����ļ�/stamp.vs","�����ļ�/stamp.fs");
	Shader textureShader("�����ļ�/texture.vs","�����ļ�/texture.fs");
	depthShader.use();
	depthShader.setInt("diffuseMap",0);
	normalShader.use();
	normalShader.setInt("depthMap",0);
	stampShader.use();
	stampShader.setInt("diffuseMap",0);
	stampShader.setInt("depthMap",1);
	stampShader.setInt("normalMap",2);
	textureShader.use();
	textureShader.setInt("diffuseMap",0);
	//��ȡ����
	diffuseMap[0]= loadTexture("�����ļ�/Ү�.png");
	diffuseMap[1]= loadTexture("�����ļ�/��.png");
	diffuseMap[2]= loadTexture("�����ļ�/��.png");
	diffuseMap[3]= loadTexture("�����ļ�/�����Ű���.png");
	diffuseMap[4]= loadTexture("�����ļ�/�Ű׼�.png");
	diffuseMap[5]= loadTexture("�����ļ�/��������.png");
	diffuseMap[6]= loadTexture("�����ļ�/��������.png");
	diffuseMap[7]= loadTexture("�����ļ�/�����ϱ�.png");
	diffuseMap[8]= loadTexture("�����ļ�/������.png");
	diffuseMap[9]= loadTexture("�����ļ�/��������.png");
	diffuseMap[10]= loadTexture("�����ļ�/��˾��ӡ.png");
	diffuseMap[11]= loadTexture("�����ļ�/�󶼶�ӡ.png");
	diffuseMap[12]= loadTexture("�����ļ�/��ʷ֮ӡ.png");
	diffuseMap[13]= loadTexture("�����ļ�/����֮ӡ.png");
	diffuseMap[14]= loadTexture("�����ļ�/����.png");
	readVertext();
	//�����������
	//����
	readFace(vArr[0],vec2(1.0,1.0),vArr[1],vec2(0.0,1.0),vArr[2],vec2(0.0,0.0));	 //0 1 2	�����ϱ�
	readFace(vArr[0],vec2(1.0,1.0),vArr[2],vec2(0.0,0.0),vArr[3],vec2(1.0,0.0));	 //0 2 3
	bindFace(0,7);
	////�ϲ�
	readFace(vArr[0],vec2(0.0,0.0),vArr[4],vec2(1.0,0.0),vArr[5],vec2(1.0,1.0));	 //0 4 5 ������
	bindFace(1,14);
	readFace(vArr[5],vec2(1.0,1.0),vArr[6],vec2(0.0,1.0),vArr[1],vec2(0.0,0.0));	 //5 6 1	  ��������
	readFace(vArr[5],vec2(1.0,1.0),vArr[1],vec2(0.0,0.0),vArr[0],vec2(1.0,0.0));	 //5 1 0
	bindFace(2,6);
	readFace(vArr[1],vec2(0.0,0.0),vArr[6],vec2(1.0,0.0),vArr[7],vec2(1.0,1.0));	 //1 6 7 ������
	bindFace(3,14);
	readFace(vArr[7],vec2(0.0,1.0),vArr[8],vec2(0.0,0.0),vArr[2],vec2(1.0,0.0));	 //7 8 2		  
	readFace(vArr[7],vec2(0.0,1.0),vArr[2],vec2(1.0,0.0),vArr[1],vec2(1.0,1.0));	 //7 2 1
	bindFace(4,14);
	readFace(vArr[2],vec2(0.0,0.0),vArr[8],vec2(1.0,0.0),vArr[9],vec2(1.0,1.0));	 //2 8 9 ������
	bindFace(5,14);
	readFace(vArr[9],vec2(0.0,0.0),vArr[10],vec2(1.0,0.0),vArr[3],vec2(1.0,1.0));	 //9 10 3	  ��������
	readFace(vArr[9],vec2(0.0,0.0),vArr[3],vec2(1.0,1.0),vArr[2],vec2(0.0,1.0));	 //9 3 2
	bindFace(6,9);
	readFace(vArr[3],vec2(0.0,0.0),vArr[10],vec2(1.0,0.0),vArr[11],vec2(1.0,1.0));	 //3 10 11 ������
	bindFace(7,14);
	readFace(vArr[11],vec2(1.0,0.0),vArr[4],vec2(1.0,1.0),vArr[0],vec2(0.0,1.0));	 //11 4 0	  ������
	readFace(vArr[11],vec2(1.0,0.0),vArr[0],vec2(0.0,1.0),vArr[3],vec2(0.0,0.0));	 //11 0 3
	bindFace(8,8);
	//�в�
	readFace(vArr[12],vec2(0.0,0.0),vArr[13],vec2(1.0,0.0),vArr[5],vec2(1.0,1.0));	 //12 13 5	   
	readFace(vArr[12],vec2(0.0,0.0),vArr[5],vec2(1.0,1.0),vArr[4],vec2(0.0,1.0));	 //12 5 4
	bindFace(9,14);
	readFace(vArr[13],vec2(1.0,1.0),vArr[14],vec2(0.0,1.0),vArr[6],vec2(0.0,0.0));	 //13 14 6	   ��������
	readFace(vArr[13],vec2(1.0,1.0),vArr[6],vec2(0.0,0.0),vArr[5],vec2(1.0,0.0));	 //13 6 5
	bindFace(10,5);
	readFace(vArr[14],vec2(0.0,0.0),vArr[15],vec2(1.0,0.0),vArr[7],vec2(1.0,1.0));	 //14 15 7
	readFace(vArr[14],vec2(0.0,0.0),vArr[7],vec2(1.0,1.0),vArr[6],vec2(0.0,1.0));	 //14 7 6
	bindFace(11,14);
	readFace(vArr[15],vec2(0.0,1.0),vArr[16],vec2(0.0,0.0),vArr[8],vec2(1.0,0.0));	 //15 16 8	   �Ű׼�
	readFace(vArr[15],vec2(0.0,1.0),vArr[8],vec2(1.0,0.0),vArr[7],vec2(1.0,1.0));	 //15 8 7
	bindFace(12,4);
	readFace(vArr[16],vec2(0.0,0.0),vArr[17],vec2(1.0,0.0),vArr[9],vec2(1.0,1.0));	 //16 17 9	   ��
	readFace(vArr[16],vec2(0.0,0.0),vArr[9],vec2(1.0,1.0),vArr[8],vec2(0.0,1.0));	 //16 9 8
	bindFace(13,2);
	readFace(vArr[17],vec2(0.0,0.0),vArr[18],vec2(1.0,0.0),vArr[10],vec2(1.0,1.0)); //17 18 10	   ��˾��ӡ
	readFace(vArr[17],vec2(0.0,0.0),vArr[10],vec2(1.0,1.0),vArr[9],vec2(0.0,1.0));	 //17 10 9
	bindFace(14,10);
	readFace(vArr[18],vec2(0.0,0.0),vArr[19],vec2(1.0,0.0),vArr[11],vec2(1.0,1.0));	 //18 19 11	   ��
	readFace(vArr[18],vec2(0.0,0.0),vArr[11],vec2(1.0,1.0),vArr[10],vec2(0.0,1.0));	 //18 11 10
	bindFace(15,1);
	readFace(vArr[19],vec2(1.0,0.0),vArr[12],vec2(1.0,1.0),vArr[4],vec2(0.0,1.0));	 //19 12 4	   �����Ű���
	readFace(vArr[19],vec2(1.0,0.0),vArr[4],vec2(0.0,1.0),vArr[11],vec2(0.0,0.0));	 //19 4 11
	bindFace(16,3);
	//�²�
	readFace(vArr[12],vec2(0.0,0.0),vArr[20],vec2(1.0,0.0),vArr[13],vec2(1.0,1.0)); //12 20 13	������
	bindFace(17,14);
	readFace(vArr[20],vec2(1.0,1.0),vArr[21],vec2(0.0,1.0),vArr[14],vec2(0.0,0.0)); //20 21 14	   ����֮ӡ
	readFace(vArr[20],vec2(1.0,1.0),vArr[14],vec2(0.0,0.0),vArr[13],vec2(1.0,0.0)); //20 14 13
	bindFace(18,13);
	readFace(vArr[14],vec2(0.0,0.0),vArr[21],vec2(1.0,0.0),vArr[15],vec2(1.0,1.0));	 //14 21 15	������
	bindFace(19,14);
	readFace(vArr[21],vec2(0.0,0.0),vArr[22],vec2(1.0,0.0),vArr[16],vec2(1.0,1.0));	 //21 22 16
	readFace(vArr[21],vec2(0.0,0.0),vArr[16],vec2(1.0,1.0),vArr[15],vec2(0.0,1.0));	 //21 16 15
	bindFace(20,14);
	readFace(vArr[16],vec2(0.0,0.0),vArr[22],vec2(1.0,0.0),vArr[17],vec2(1.0,1.0));	 //16 22 17	������
	bindFace(21,14);
	readFace(vArr[22],vec2(0.0,0.0),vArr[23],vec2(1.0,0.0),vArr[18],vec2(1.0,1.0)); //22 23 18	   �󶼶�ӡ
	readFace(vArr[22],vec2(0.0,0.0),vArr[18],vec2(1.0,1.0),vArr[17],vec2(0.0,1.0)); //22 18 17
	bindFace(22,11);
	readFace(vArr[18],vec2(0.0,0.0),vArr[23],vec2(1.0,0.0),vArr[19],vec2(1.0,1.0));	 //18 23 19	������
	bindFace(23,14);
	readFace(vArr[23],vec2(0.0,0.0),vArr[20],vec2(1.0,0.0),vArr[12],vec2(1.0,1.0));	 //23 20 12
	readFace(vArr[23],vec2(0.0,0.0),vArr[12],vec2(1.0,1.0),vArr[19],vec2(0.0,1.0));	 //23 12 19
	bindFace(24,14);
	//�ײ�
	readFace(vArr[20],vec2(1.0,0.0),vArr[23],vec2(1.0,1.0),vArr[22],vec2(0.0,1.0)); //20 23 22	   ��ʷ֮ӡ
	readFace(vArr[22],vec2(0.0,1.0),vArr[21],vec2(0.0,0.0),vArr[20],vec2(1.0,0.0)); //22 21 20	 
	bindFace(25,12);
	vArr.clear();

	// ��Ȼ�������
	unsigned int depthBuffer[15];
	glGenFramebuffers(15, &depthBuffer[0]);
	glGenTextures(15, &depthMap[0]);
	for (int i=0;i<15;i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer[i]);
		// ������ɫ��������
		glBindTexture(GL_TEXTURE_2D, depthMap[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 400, 400, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap[i], 0);
		// ���֡�Ƿ�����
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	   //����Ĭ�ϻ���
	}

	// ���߻�������
	unsigned int normalBuffer[15];
	glGenFramebuffers(15, &normalBuffer[0]);
	glGenTextures(15, &normalMap[0]);
	for (int i=0;i<15;i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, normalBuffer[i]);
		// ������ɫ��������
		glBindTexture(GL_TEXTURE_2D, normalMap[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 400, 400, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap[i], 0);
		// ���֡�Ƿ�����
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	   //����Ĭ�ϻ���
	}
	

	while (!glfwWindowShouldClose(window))
	{
		//ÿ֡����߼�ʱ��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//�����ⲿ����
		processInput(window); 
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glViewport(0,0,400,400);	 //�����ӿ�
		//�������ͼ
		for (int i=0;i<15;i++)
		{
			//�󶨻���
			glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer[i]);
			glClear(GL_COLOR_BUFFER_BIT);
			
			depthShader.use();
			depthShader.setFloat("heightScale",heightScale);
			depthShader.setMat4("model",glm::mat4());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap[i]);
			rendObject();
		}

		//���Ʒ���ͼ
		for (int i=0;i<15;i++)
		{
			//�󶨻���
			glBindFramebuffer(GL_FRAMEBUFFER, normalBuffer[i]);
			glClear(GL_COLOR_BUFFER_BIT);

			normalShader.use();
			normalShader.setMat4("model",glm::mat4());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMap[i]);
			rendObject();
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	   //����Ĭ�ϻ���
		glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);	 //�����ӿ�
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*textureShader.use();
		textureShader.setMat4("model",glm::mat4());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, normalMap[1]);
		rendObject();*/


		if (ifRotate)
		{
			if((Angle+=0.02f)>=360.0f)
			{
				Angle=0.0f;
				dir=!dir;		  //����ȡ��
			}

		}

		//������ɫ������
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;
		stampShader.use();
		if (dir)
			model = rotate(model,float(radians(Angle)),vec3(0.0,1.0,0.0));
		else
			model = rotate(model,float(radians(Angle)),vec3(1.0,0.0,0.0));
		stampShader.setMat4("projection", projection);
		stampShader.setMat4("view", view);
		stampShader.setMat4("model", model);
		stampShader.setVec3("viewPos",camera.Position);
		stampShader.setVec3("lightPos",glm::vec3(0,0,5));
		stampShader.setBool("light",light);
		//��������
		for (int i=0;i<26;i++)
		{
			//������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap[texIndex[i]]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap[texIndex[i]]);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, normalMap[texIndex[i]]);
			glBindVertexArray(stampVAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, vertextNum[i]/14); // ����
			glBindVertexArray(0);
		}

		//������������Ϣ����
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//ɾ�������ͷ���Դ
	for (int i=0;i<26;i++)
	{
		glDeleteVertexArrays(1, &stampVAO[i]);
		glDeleteBuffers(1, &stampVBO[i]);
	}
	glDeleteVertexArrays(1, &textureVAO);
	glDeleteBuffers(1, &textureVBO);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	static float speed=0.7f;
	static bool spaceKey = false;
	static bool cKey = false;
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && spaceKey == false)
	{
		spaceKey=true;
		ifRotate = !ifRotate;
		Angle=0.0;
		dir=false;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && spaceKey == true)
		spaceKey=false;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && cKey == false)
	{
		cKey=true;
		light=!light;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && cKey == true)
		cKey=false;
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
	static int Num = 0;
	
	if(textureVAO==0)
	{
		vector<float> Arr;
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
		Num=Arr.size();
		glGenVertexArrays(1, &textureVAO);
		glGenBuffers(1, &textureVBO);
		glBindVertexArray(textureVAO);
		glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * Num, &Arr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
		Arr.clear();
	}
	//������
	glBindVertexArray(textureVAO);
	glDrawArrays(GL_TRIANGLES, 0, Num/4); // ����
	glBindVertexArray(0);
}

//���붥������
void readVertext()
{
	int nIndex[4]={4,8,8,4}; //ÿ�㶥����
	int level=4; //�ܲ��
	//����ÿ�㶥������
	for(int i=0;i<level;i++)
	{
		float interval = 360.0f/nIndex[i]; //ÿ�����Ƕ�
		//��ǰ���y����
		float y = sin(radians(67.5f-i*45.0f)); //���˱���
		float radius = cos(radians(67.5f-i*45.0f))/cos(radians(interval/2)); //���˱���
		for (int j=0;j<nIndex[i];j++)
		{
			float Angle = interval*j+interval/2;
			float x = radius*cos(radians(Angle));
			float z = -radius*sin(radians(Angle));
			vArr.push_back(vec3(x,y,z));
		}
	}
}

void readFace(vec3 p0,vec2 t0,vec3 p1,vec2 t1,vec3 p2,vec2 t2)
{
	//����TBN
	glm::vec3 tangent1, bitangent1;

	glm::vec3 edge1 = p1 - p0;
	glm::vec3 edge2 = p2 - p0;
	glm::vec2 deltaUV1 = t1 - t0;
	glm::vec2 deltaUV2 = t2 - t0;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);
	//���㷨��
	glm::vec3 Normal = glm::cross(edge1,edge2);

	fArr.push_back(p0.x);
	fArr.push_back(p0.y);
	fArr.push_back(p0.z);
	fArr.push_back(t0.x);
	fArr.push_back(t0.y);
	fArr.push_back(Normal.x);
	fArr.push_back(Normal.y);
	fArr.push_back(Normal.z);
	fArr.push_back(tangent1.x);
	fArr.push_back(tangent1.y);
	fArr.push_back(tangent1.z);
	fArr.push_back(bitangent1.x);
	fArr.push_back(bitangent1.y);
	fArr.push_back(bitangent1.z);

	fArr.push_back(p1.x);
	fArr.push_back(p1.y);
	fArr.push_back(p1.z);
	fArr.push_back(t1.x);
	fArr.push_back(t1.y);
	fArr.push_back(Normal.x);
	fArr.push_back(Normal.y);
	fArr.push_back(Normal.z);
	fArr.push_back(tangent1.x);
	fArr.push_back(tangent1.y);
	fArr.push_back(tangent1.z);
	fArr.push_back(bitangent1.x);
	fArr.push_back(bitangent1.y);
	fArr.push_back(bitangent1.z);

	fArr.push_back(p2.x);
	fArr.push_back(p2.y);
	fArr.push_back(p2.z);
	fArr.push_back(t2.x);
	fArr.push_back(t2.y);
	fArr.push_back(Normal.x);
	fArr.push_back(Normal.y);
	fArr.push_back(Normal.z);
	fArr.push_back(tangent1.x);
	fArr.push_back(tangent1.y);
	fArr.push_back(tangent1.z);
	fArr.push_back(bitangent1.x);
	fArr.push_back(bitangent1.y);
	fArr.push_back(bitangent1.z);
}

void bindFace(int i,int texNum)
{
	vertextNum[i] = fArr.size();
	if (vertextNum[i] == 0)
		return ;
	texIndex[i]=texNum;
	glGenVertexArrays(1, &stampVAO[i]);
	glGenBuffers(1, &stampVBO[i]);
	glBindVertexArray(stampVAO[i]);
	glBindBuffer(GL_ARRAY_BUFFER, stampVBO[i]);
	glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum[i], &fArr[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);	 //λ��
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));	 //����
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(5 * sizeof(float)));	 //����
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	glBindVertexArray(0);
	fArr.clear();
}


//��������
unsigned int loadTexture(char const * path)
{
	stbi_set_flip_vertically_on_load(true);
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