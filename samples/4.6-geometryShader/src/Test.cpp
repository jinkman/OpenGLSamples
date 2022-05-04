// Test.cpp : 定义控制台应用程序的入口点。
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
#define PI 3.1415926

float deltaTime = 0.0;
float lastFrame = 0.0;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 10.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void rendObject();
void readVertext(std::vector<float> &Arr);
unsigned int loadTexture(char const *path);
void Rotatez(glm::vec3 &a, double Thta);

unsigned int objectVAO = 0, objectVBO;
glm::mat4 *modelMatrices;
glm::vec3 *speedMatrices;
int amount = 50;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	bool isFullScreen = false;
	GLFWwindow *window = NULL;
	if (isFullScreen)
	{
		const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		SCR_WIDTH = vidmode->width;
		SCR_HEIGHT = vidmode->height;
		GLFWmonitor *pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", pMonitor, NULL);
		lastX = SCR_WIDTH / 2.0f;
		lastY = SCR_HEIGHT / 2.0f;
	}
	else
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	Shader grassShader(getLocalPath("shader/4.6-grass.vs").c_str(), getLocalPath("shader/4.6-grass.fs").c_str(), getLocalPath("shader/4.6-grass.gs").c_str());
	grassShader.use();
	grassShader.setInt("diffuseMap", 0);

	modelMatrices = new glm::mat4[amount];
	float radius = 10.0f;
	for (int i = 0; i < amount; i++)
	{
		glm::mat4 model(1.0f);
		float x = rand() % (int(20 * radius)) / 10.0f - radius;
		float height = sqrt(pow(radius, 2.0f) - pow(x, 2.0f));
		float z = rand() % (int(20 * height) + 1) / 10.0f - height;
		model = glm::translate(model, glm::vec3(x, 0.0f, z));
		model = glm::rotate(model, glm::radians(float(rand() % 3600 / 10.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrices[i] = model;
	}

	speedMatrices = new glm::vec3[amount];
	for (int i = 0; i < amount; i++)
		speedMatrices[i] = glm::vec3(20.0f + rand() % 3000 / 1000.0f, 3.5 + rand() % 300 / 100.0f, rand() % 1000 / 100.0f + 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model(1.0f);

		grassShader.use();
		grassShader.setMat4("projection", projection);
		grassShader.setMat4("view", view);
		grassShader.setFloat("g", 20.0f); // gravity
		grassShader.setFloat("size", 1.0f);
		grassShader.setInt("pre", 20); // precision
		grassShader.setFloat("uTimes", (float)glfwGetTime() * 1.2f);
		grassShader.setVec4("grassColor", glm::vec4(0.0f, 0.7f, 0.0f, 0.0f));

		for (int i = 0; i < amount; i++)
		{
			grassShader.setMat4("model", modelMatrices[i]);
			grassShader.setVec3("speed", speedMatrices[i]);
			rendObject();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &objectVAO);
	glDeleteBuffers(1, &objectVBO);
	delete[] modelMatrices;
	delete[] speedMatrices;
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	static float speed = 1.0f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime * speed);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		speed += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		speed -= 0.01f;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{

	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}

void rendObject()
{
	static unsigned int grassMap = loadTexture(getLocalPath("texture/草.png").c_str());
	static size_t vertextNum = 0;
	if (objectVAO == 0)
	{
		std::vector<float> vArr;
		readVertext(vArr);
		vertextNum = vArr.size();
		if (vertextNum == 0)
			return;
		glGenVertexArrays(1, &objectVAO);
		glGenBuffers(1, &objectVBO);
		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &vArr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		glBindVertexArray(0);
		vArr.clear();
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassMap);
	glBindVertexArray(objectVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertextNum / 5);
	glBindVertexArray(0);
}

void readVertext(std::vector<float> &Arr)
{
	float times = 10.0f;
	float heightTimes = 5.0f;
	float space = 3.0f / 2;
	Arr.push_back(-1.5f * times);
	Arr.push_back(0.0f);
	Arr.push_back((0.5f * space - 1.5f) * times);
	Arr.push_back(0.0f);
	Arr.push_back(0.0f);

	Arr.push_back(1.5f * times);
	Arr.push_back(0.0f);
	Arr.push_back((0.5f * space - 1.5f) * times);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);

	Arr.push_back(-1.5f * times);
	Arr.push_back(3.0f * heightTimes);
	Arr.push_back((0.5f * space - 1.5f) * times);
	Arr.push_back(0.0f);
	Arr.push_back(1.0f);
	Arr.push_back(1.5f * times);
	Arr.push_back(0.0f);
	Arr.push_back((0.5f * space - 1.5f) * times);
	Arr.push_back(1.0f);
	Arr.push_back(0.0f);

	Arr.push_back(1.5f * times);
	Arr.push_back(3.0f * heightTimes);
	Arr.push_back((0.5f * space - 1.5f) * times);
	Arr.push_back(1.0f);
	Arr.push_back(1.0f);

	Arr.push_back(-1.5f * times);
	Arr.push_back(3.0f * heightTimes);
	Arr.push_back((0.5f * space - 1.5f) * times);
	Arr.push_back(0.0f);
	Arr.push_back(1.0f);
}

unsigned int loadTexture(char const *path)
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