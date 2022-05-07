// Test.cpp : 定义控制台应用程序的入口点。
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <shader_s.h>
#include <camera.h>
#include <iostream>
#include <common.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void rendPlan();

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0;
float lastFrame = 0.0;

unsigned int planVAO = 0, planVBO;
bool ifgamma = false;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

	// �������
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ����glad��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader shader(getLocalPath("shader/5.2-gamma_lighting.vs").c_str(), getLocalPath("shader/5.2-gamma_lighting.fs").c_str());

	shader.use();
	shader.setInt("texture1", 0);

	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos);
		shader.setInt("ifgamma", ifgamma);
		rendPlan();

		std::cout << (ifgamma ? "gamma" : "Nogamma") << std::endl;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &planVAO);
	glDeleteBuffers(1, &planVBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	static bool gammaKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !gammaKeyPressed)
	{
		ifgamma = !ifgamma;
		gammaKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		gammaKeyPressed = false;
	}
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

void rendPlan()
{
	static unsigned int floorTexture = loadTexture(getLocalPath("texture/test.jpg").c_str());
	if (planVAO == 0)
	{
		float planeVertices[] = {
			10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
			-10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,

			10.0f, -0.5f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
			-10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f,
			10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f};
		glGenVertexArrays(1, &planVAO);
		glGenBuffers(1, &planVBO);
		glBindVertexArray(planVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glBindVertexArray(0);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glBindVertexArray(planVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}