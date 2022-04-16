// ������ӡ.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <GL/glad.h>
#include <gl/glfw3.h>
#include <gl/glut.h>
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
void readVertext();
void bindFace(int i,int texNum);
void readFace(vec3 p0,vec2 t0,vec3 p1,vec2 t1,vec3 p2,vec2 t2);
unsigned int loadTexture(char const * path);


vector<vec3> vArr;  //��������
vector<float> fArr;	  //�������
unsigned int stampVAO[26]={0}, stampVBO[26];
bool ifRotate = false;	 //�Ƿ���ת
float Angle = 0.0f;		 //��ת�Ƕ�
bool dir =false;		 //��ת����
unsigned int texture[15]; //��������
unsigned int texIndex[26]; //����������
unsigned int vertextNum[26]; //������Ŀ

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
	//glEnable(GL_CULL_FACE);
	//ʹ����ɫ��
	Shader stampShader("�����ļ�/stamp.vs","�����ļ�/stamp.fs","�����ļ�/stamp.gs");
	stampShader.use();
	stampShader.setInt("texture",0);
	//��ȡ����
	texture[0]= loadTexture("�����ļ�/Ү�.png");
	texture[1]= loadTexture("�����ļ�/��.png");
	texture[2]= loadTexture("�����ļ�/��.png");
	texture[3]= loadTexture("�����ļ�/�����Ű���.png");
	texture[4]= loadTexture("�����ļ�/�Ű׼�.png");
	texture[5]= loadTexture("�����ļ�/��������.png");
	texture[6]= loadTexture("�����ļ�/��������.png");
	texture[7]= loadTexture("�����ļ�/�����ϱ�.png");
	texture[8]= loadTexture("�����ļ�/������.png");
	texture[9]= loadTexture("�����ļ�/��������.png");
	texture[10]= loadTexture("�����ļ�/��˾��ӡ.png");
	texture[11]= loadTexture("�����ļ�/�󶼶�ӡ.png");
	texture[12]= loadTexture("�����ļ�/��ʷ֮ӡ.png");
	texture[13]= loadTexture("�����ļ�/����֮ӡ.png");
	texture[14]= loadTexture("�����ļ�/����.png");
	readVertext();
	//�����������
	//����
	readFace(vArr[0],vec2(1.0,1.0),vArr[1],vec2(0.0,1.0),vArr[2],vec2(0.0,0.0));	 //0 1 2	�����ϱ�
	readFace(vArr[0],vec2(1.0,1.0),vArr[2],vec2(0.0,0.0),vArr[3],vec2(1.0,0.0));	 //0 2 3
	bindFace(0,7);
	////�ϲ�
	readFace(vArr[0],vec2(0.0,0.0),vArr[4],vec2(0.0,0.0),vArr[5],vec2(0.0,0.0));	 //0 4 5 ������
	bindFace(1,14);
	readFace(vArr[5],vec2(1.0,1.0),vArr[6],vec2(0.0,1.0),vArr[1],vec2(0.0,0.0));	 //5 6 1	  ��������
	readFace(vArr[5],vec2(1.0,1.0),vArr[1],vec2(0.0,0.0),vArr[0],vec2(1.0,0.0));	 //5 1 0
	bindFace(2,6);
	readFace(vArr[1],vec2(0.0,0.0),vArr[6],vec2(0.0,0.0),vArr[7],vec2(0.0,0.0));	 //1 6 7 ������
	bindFace(3,14);
	readFace(vArr[7],vec2(0.0,1.0),vArr[8],vec2(0.0,0.0),vArr[2],vec2(1.0,0.0));	 //7 8 2		  
	readFace(vArr[7],vec2(0.0,1.0),vArr[2],vec2(1.0,0.0),vArr[1],vec2(1.0,1.0));	 //7 2 1
	bindFace(4,14);
	readFace(vArr[2],vec2(0.0,0.0),vArr[8],vec2(0.0,0.0),vArr[9],vec2(0.0,0.0));	 //2 8 9 ������
	bindFace(5,14);
	readFace(vArr[9],vec2(0.0,0.0),vArr[10],vec2(1.0,0.0),vArr[3],vec2(1.0,1.0));	 //9 10 3	  ��������
	readFace(vArr[9],vec2(0.0,0.0),vArr[3],vec2(1.0,1.0),vArr[2],vec2(0.0,1.0));	 //9 3 2
	bindFace(6,9);
	readFace(vArr[3],vec2(0.0,0.0),vArr[10],vec2(0.0,0.0),vArr[11],vec2(0.0,0.0));	 //3 10 11 ������
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
	readFace(vArr[12],vec2(0.0,0.0),vArr[20],vec2(0.0,0.0),vArr[13],vec2(0.0,0.0)); //12 20 13	������
	bindFace(17,14);
	readFace(vArr[20],vec2(1.0,1.0),vArr[21],vec2(0.0,1.0),vArr[14],vec2(0.0,0.0)); //20 21 14	   ����֮ӡ
	readFace(vArr[20],vec2(1.0,1.0),vArr[14],vec2(0.0,0.0),vArr[13],vec2(1.0,0.0)); //20 14 13
	bindFace(18,13);
	readFace(vArr[14],vec2(0.0,0.0),vArr[21],vec2(0.0,0.0),vArr[15],vec2(0.0,0.0));	 //14 21 15	������
	bindFace(19,14);
	readFace(vArr[21],vec2(0.0,0.0),vArr[22],vec2(1.0,0.0),vArr[16],vec2(1.0,1.0));	 //21 22 16
	readFace(vArr[21],vec2(0.0,0.0),vArr[16],vec2(1.0,1.0),vArr[15],vec2(0.0,1.0));	 //21 16 15
	bindFace(20,14);
	readFace(vArr[16],vec2(0.0,0.0),vArr[22],vec2(0.0,0.0),vArr[17],vec2(0.0,0.0));	 //16 22 17	������
	bindFace(21,14);
	readFace(vArr[22],vec2(0.0,0.0),vArr[23],vec2(1.0,0.0),vArr[18],vec2(1.0,1.0)); //22 23 18	   �󶼶�ӡ
	readFace(vArr[22],vec2(0.0,0.0),vArr[18],vec2(1.0,1.0),vArr[17],vec2(0.0,1.0)); //22 18 17
	bindFace(22,11);
	readFace(vArr[18],vec2(0.0,0.0),vArr[23],vec2(0.0,0.0),vArr[19],vec2(0.0,0.0));	 //18 23 19	������
	bindFace(23,14);
	readFace(vArr[23],vec2(0.0,0.0),vArr[20],vec2(1.0,0.0),vArr[12],vec2(1.0,1.0));	 //23 20 12
	readFace(vArr[23],vec2(0.0,0.0),vArr[12],vec2(1.0,1.0),vArr[19],vec2(0.0,1.0));	 //23 12 19
	bindFace(24,14);
	//�ײ�
	readFace(vArr[20],vec2(1.0,0.0),vArr[23],vec2(1.0,1.0),vArr[22],vec2(0.0,1.0)); //20 23 22	   ��ʷ֮ӡ
	readFace(vArr[22],vec2(0.0,1.0),vArr[21],vec2(0.0,0.0),vArr[20],vec2(1.0,0.0)); //22 21 20	 
	bindFace(25,12);
	vArr.clear();

	while (!glfwWindowShouldClose(window))
	{
		//ÿ֡����߼�ʱ��
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//�����ⲿ����
		processInput(window); 
		//�������
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//��ȡ�任����
		mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 model;
		if (ifRotate)
		{
			if((Angle+=0.02f)>=360.0f)
			{
				Angle=0.0f;
				dir=!dir;		  //����ȡ��
			}

		}

		//������ɫ������
		stampShader.use();
		if (dir)
			model = rotate(model,float(radians(Angle)),vec3(0.0,1.0,0.0));
		else
			model = rotate(model,float(radians(Angle)),vec3(1.0,0.0,0.0));
		stampShader.setVec3("viewPos",camera.Position);
		stampShader.setMat4("projection", projection);
		stampShader.setMat4("view", view);
		stampShader.setMat4("model", model);
		//��������
		for (int i=0;i<26;i++)
		{
			//������
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texIndex[i]);
			glBindVertexArray(stampVAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, vertextNum[i]/5); // ����
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
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	static float speed=1.0f;
	static bool spaceKey = false;
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && spaceKey == false)
	{
		spaceKey=true;
		ifRotate = !ifRotate;
		Angle=0.0;
		dir=false;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && spaceKey == true)
		spaceKey=false;
		
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
		//radius=fabs(radius);
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
	fArr.push_back(p0.x);
	fArr.push_back(p0.y);
	fArr.push_back(p0.z);
	fArr.push_back(t0.x);
	fArr.push_back(t0.y);

	fArr.push_back(p1.x);
	fArr.push_back(p1.y);
	fArr.push_back(p1.z);
	fArr.push_back(t1.x);
	fArr.push_back(t1.y);

	fArr.push_back(p2.x);
	fArr.push_back(p2.y);
	fArr.push_back(p2.z);
	fArr.push_back(t2.x);
	fArr.push_back(t2.y);
}

void bindFace(int i,int texNum)
{
	vertextNum[i] = fArr.size();
	if (vertextNum[i] == 0)
		return ;
	texIndex[i]=texture[texNum];
	glGenVertexArrays(1, &stampVAO[i]);
	glGenBuffers(1, &stampVBO[i]);
	glBindVertexArray(stampVAO[i]);
	glBindBuffer(GL_ARRAY_BUFFER, stampVBO[i]);
	glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum[i], &fArr[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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