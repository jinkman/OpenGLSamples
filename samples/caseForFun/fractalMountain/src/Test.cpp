// Test.cpp : 定义控制台应用程序的入口点。
//

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>


// 屏幕
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
#define PI 3.1415926f

//相机
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
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
unsigned int loadTexture(char const * path);
void drawMountain();
void readVertext(std::vector<float> &Arr);
void rendPerlin();

unsigned int moutainVAO, moutainVBO;
unsigned int perlinVAO=0, perlinVBO;

const int pointNum=400;
const float heightSize = 70;
const float allSize=100.0f;

//perlin噪声
bool changePerlin = true;
int octaves=5;				 //倍频
float persistence=0.3f;		 //振幅
float delta=0.05f;			 //平滑度
unsigned int perlinColorMap;	 //噪声纹理

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

	// 开启OpenGL状态
	glEnable(GL_DEPTH_TEST);
	//使用着色器
	Shader mountainShader(getLocalPath("shader/case2-mountain.vs").c_str(), getLocalPath("shader/case2-mountain.fs").c_str(), getLocalPath("shader/case2-mountain.gs").c_str());
	Shader perlinShader(getLocalPath("shader/case2-perlin.vs").c_str(), getLocalPath("shader/case2-perlin.fs").c_str());

	mountainShader.use();
	mountainShader.setInt("diffuseMap",0);
	mountainShader.setInt("diffuseMap1",1);
	mountainShader.setInt("perlinMap",2);

	//配置perlin帧缓冲
	unsigned int perlinbuffer;
	glGenFramebuffers(1, &perlinbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, perlinbuffer);
	// 创建颜色附件纹理
	glGenTextures(1, &perlinColorMap);
	glBindTexture(GL_TEXTURE_2D, perlinColorMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, perlinColorMap, 0);
	// 检查帧是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//处理外部输入
		processInput(window);

		//清除缓存
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model(1.0f);

		//渲染帧缓冲
		if(changePerlin)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, perlinbuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			perlinShader.use();
			perlinShader.setInt("octaves",octaves);
			perlinShader.setFloat("persistence",persistence);
			perlinShader.setFloat("delta",delta);
			rendPerlin();
			changePerlin=false;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		//渲染山
		mountainShader.use();
		mountainShader.setMat4("projection", projection);
		mountainShader.setMat4("view", view);
		model=glm::scale(model,glm::vec3(allSize));
		model = glm::translate(model,glm::vec3(0.0,-10.0,0.0));
		mountainShader.setMat4("model", model);
		mountainShader.setFloat("heightSize", heightSize);
		drawMountain();

		//交换缓冲与消息分配
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//删除对象及释放资源
	glDeleteVertexArrays(1, &moutainVAO);
	glDeleteBuffers(1, &moutainVBO);
	glDeleteVertexArrays(1, &perlinVAO);
	glDeleteBuffers(1, &perlinVBO);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	static float speed=100.0f;
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
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		delta+=0.0001f;
		changePerlin = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		delta-=0.0001f;
		if(delta<0.0f)
			delta=0.0f;
		changePerlin = true;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		persistence-=0.001f;
		if(persistence<0.0f)
			persistence=0.0f;
		changePerlin = true;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		persistence+=0.001f;
		changePerlin = true;
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void drawMountain()
{
	static size_t num=0;
	static	unsigned int mountainMap = loadTexture(getLocalPath("texture/草坪.jpg").c_str());
	static	unsigned int mountainMap1 = loadTexture(getLocalPath("texture/草地.jpg").c_str());
	if (moutainVAO==0)
	{
		std::vector<float> Arr;
		readVertext(Arr);
		num=Arr.size();
		if(num==0)
			return ;
		glGenVertexArrays(1, &moutainVAO);
		glGenBuffers(1, &moutainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, moutainVBO);
		glBufferData(GL_ARRAY_BUFFER, num * 4, &Arr[0], GL_STATIC_DRAW);		 //会频繁修改
		glBindVertexArray(moutainVAO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		Arr.clear();
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mountainMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mountainMap1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, perlinColorMap);
	glBindVertexArray(moutainVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)num/4);
	glBindVertexArray(0);
}


void rendPerlin()
{
	if(perlinVAO==0)
	{
		// 配置VAO/VBO；
		float quadVertices[] = { // 填充整个屏幕的纹理
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};
		glGenVertexArrays(1, &perlinVAO);
		glGenBuffers(1, &perlinVBO);
		glBindVertexArray(perlinVAO);
		glBindBuffer(GL_ARRAY_BUFFER, perlinVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(perlinVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制
	glBindVertexArray(0);
}


//读入顶点坐标
void readVertext(std::vector<float> &Arr)
{
	float size =0.1f;
	float halfPoint = pointNum / 2.0f;
	for (int i=0;i<pointNum-1;i++)
	{
		for (int j=0;j<pointNum-1;j++)
		{
			//第一个三角形
			Arr.push_back(i-halfPoint);
			Arr.push_back(j-halfPoint);
			Arr.push_back(float(i)/pointNum);
			Arr.push_back(float(j)/pointNum);

			Arr.push_back(i-halfPoint);
			Arr.push_back(j+1-halfPoint);
			Arr.push_back(float(i)/pointNum);
			Arr.push_back(float(j+1)/pointNum);

			Arr.push_back(i+1-halfPoint);
			Arr.push_back(j-halfPoint);
			Arr.push_back(float(i+1)/pointNum);
			Arr.push_back(float(j)/pointNum);
			//第二个三角形
			Arr.push_back(i+1-halfPoint);
			Arr.push_back(j-halfPoint);
			Arr.push_back(float(i+1)/pointNum);
			Arr.push_back(float(j)/pointNum);

			Arr.push_back(i-halfPoint);
			Arr.push_back(j+1-halfPoint);
			Arr.push_back(float(i)/pointNum);
			Arr.push_back(float(j+1)/pointNum);

			Arr.push_back(i+1-halfPoint);
			Arr.push_back(j+1-halfPoint);
			Arr.push_back(float(i+1)/pointNum);
			Arr.push_back(float(j+1)/pointNum);
		}
	}
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

unsigned int loadTexture(char const * path)
{
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
