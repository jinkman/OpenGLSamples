// Test.cpp : 定义控制台应用程序的入口点。
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <camera.h>
#include <shader_s.h>
#include <math.h>
#include <common.h>

#define PI 3.1415926f
int SCR_WIDTH = 800, SCR_HEIGHT = 600;

// ��������
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void readVertext(std::vector<float> &Arr);
void Rotatez(glm::vec3 &a, float Thta);
unsigned int loadTexture(char const *path);
void RenderScene(Shader &shader);
void RenderSphere();
void RenderQuad();

// ���
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float deltaTime = 0.0;
float lastFrame = 0.0;

// ѡ��
bool shadows = true;
bool change = true;

unsigned int woodTexture;
unsigned int planeVAO = 0, planeVBO;
unsigned int sphereVAO = 0, sphereVBO;
unsigned int quadVAO = 0, quadVBO;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	Shader shader(getLocalPath("shader/5.3.1-shadow_mapping.vs").c_str(), getLocalPath("shader/5.3.1-shadow_mapping.fs").c_str());
	Shader simpleDepthShader(getLocalPath("shader/5.3.1-shadow_mapping_depth.vs").c_str(), getLocalPath("shader/5.3.1-shadow_mapping_depth.fs").c_str());
	Shader sphereShader(getLocalPath("shader/5.3.1-sphere.vs").c_str(), getLocalPath("shader/5.3.1-sphere.fs").c_str());

	sphereShader.use();
	sphereShader.setInt("SphereMap", 0);
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);

	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	woodTexture = loadTexture(getLocalPath("texture/test.jpg").c_str());

	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048; //������
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = {1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		if (change)
		{
			lightPos.x = sin((float)glfwGetTime()) * 3.0f;
			lightPos.z = cos((float)glfwGetTime()) * 4.0f;
			lightPos.y = 5.0f + cos((float)glfwGetTime()) * 1.0f;
		}
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;

		// light space matrix
		float near_plane = 1.0f, far_plane = 100.0f;
		lightProjection = glm::perspective(glm::radians(80.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;

		// render depth map
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderScene(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render scene
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", camera.Position);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		shader.setBool("shadows", shadows);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderScene(shader);

		// render light
		sphereShader.use();
		glm::mat4 model(1.0f);
		model = glm::translate(model, lightPos);
		sphereShader.setMat4("model", model);
		sphereShader.setMat4("projection", projection);
		sphereShader.setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		RenderSphere();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &sphereVBO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &quadVBO);
	glfwTerminate();
	return 0;
}

void RenderScene(Shader &shader)
{
	// Floor
	glm::mat4 model(1.0f);
	shader.setMat4("model", model);
	RenderQuad();
	// Cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0));
	shader.setMat4("model", model);
	RenderSphere();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 1.0f, 1.0));
	shader.setMat4("model", model);
	RenderSphere();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 1.0f, 2.0));
	model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.5));
	shader.setMat4("model", model);
	RenderSphere();
}

void RenderQuad()
{
	if (quadVAO == 0)
	{
		float planeVertices[] = {
			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
			-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
			25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	glBindVertexArray(0);
}

void RenderSphere()
{
	static size_t vertextNum = 0;
	if (sphereVAO == 0)
	{
		std::vector<float> Arr;
		readVertext(Arr);
		vertextNum = Arr.size();
		if (vertextNum == 0)
			return;
		glGenVertexArrays(1, &sphereVAO);
		glGenBuffers(1, &sphereVBO);
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &Arr[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(sphereVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertextNum / 5);
	glBindVertexArray(0);
}

void readVertext(std::vector<float> &Arr)
{
	glm::vec3 cpt[4];
	for (float Theta = 0; Theta < 180.0f; Theta += 1.0f)
	{
		glm::vec3 p1(sin(Theta * PI / 180.0f), cos(Theta * PI / 180.0f), 0.0f);
		glm::vec3 p2(sin((Theta + 1.0f) * PI / 180.0f), cos((Theta + 1.0f) * PI / 180.0f), 0.0f);
		for (float Phi = 0.0f; Phi < 360.0f; Phi += 1.0f)
		{
			cpt[0].x = p1.x;
			cpt[0].y = p1.y;
			cpt[0].z = p1.z;
			Rotatez(p1, 1.0f);
			cpt[1].x = p1.x;
			cpt[1].y = p1.y;
			cpt[1].z = p1.z;
			cpt[2].x = p2.x;
			cpt[2].y = p2.y;
			cpt[2].z = p2.z;
			Rotatez(p2, 1.0f);
			cpt[3].x = p2.x;
			cpt[3].y = p2.y;
			cpt[3].z = p2.z;

			Arr.push_back(cpt[1].x);
			Arr.push_back(cpt[1].y);
			Arr.push_back(cpt[1].z);
			Arr.push_back(1.0f - (Phi + 1.0f) / 360.0f);
			Arr.push_back(Theta / 180.0f);
			Arr.push_back(cpt[3].x);
			Arr.push_back(cpt[3].y);
			Arr.push_back(cpt[3].z);
			Arr.push_back(1.0f - (Phi + 1.0f) / 360.0f);
			Arr.push_back((Theta + 1.0f) / 180.0f);
			Arr.push_back(cpt[2].x);
			Arr.push_back(cpt[2].y);
			Arr.push_back(cpt[2].z);
			Arr.push_back(1.0f - Phi / 360.0f);
			Arr.push_back((Theta + 1.0f) / 180.0f);

			Arr.push_back(cpt[1].x);
			Arr.push_back(cpt[1].y);
			Arr.push_back(cpt[1].z);
			Arr.push_back(1.0f - (Phi + 1.0f) / 360.0f);
			Arr.push_back(Theta / 180.0f);
			Arr.push_back(cpt[2].x);
			Arr.push_back(cpt[2].y);
			Arr.push_back(cpt[2].z);
			Arr.push_back(1.0f - Phi / 360.0f);
			Arr.push_back((Theta + 1.0f) / 180.0f);
			Arr.push_back(cpt[0].x);
			Arr.push_back(cpt[0].y);
			Arr.push_back(cpt[0].z);
			Arr.push_back(1.0f - Phi / 360.0f);
			Arr.push_back(Theta / 180.0f);
		}
	}
}

void Rotatez(glm::vec3 &a, float Thta)
{
	float a1 = a.z;
	float b1 = a.x;
	a.x = b1 * cos(Thta * PI / 180.0f) - a1 * sin(Thta * PI / 180.0f);
	a.z = b1 * sin(Thta * PI / 180.0f) + a1 * cos(Thta * PI / 180.0f);
}

unsigned int loadTexture(char const *path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
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

void processInput(GLFWwindow *window)
{
	static bool shadowSwitch = false;
	static bool moveSwitch = false;
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && shadowSwitch == false)
	{
		shadowSwitch = true;
		shadows = !shadows;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && shadowSwitch == true)
		shadowSwitch = false;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && moveSwitch == false)
	{
		moveSwitch = true;
		change = !change;
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && moveSwitch == true)
		moveSwitch = false;
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
