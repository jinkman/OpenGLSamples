#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <shader_s.h>
#include <common.h>
#include <thread>
#include <chrono>
#include <stb_image.h>

int scrWidth = 1280;
int scrHeight = 720;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

void renderObject(Shader &shader, unsigned int textureID);
std::vector<float> readVertex();

unsigned int loadTexture(char const *path);
unsigned int objectVAO = 0, objectVBO;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    bool isFullScreen = false;
    GLFWwindow *window = NULL;
    if (isFullScreen) {
        const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        scrWidth = vidmode->width;
        scrHeight = vidmode->height;
        GLFWmonitor *pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
        window = glfwCreateWindow(scrWidth, scrHeight, "LearnOpenGL", pMonitor, NULL);
    } else {
        window = glfwCreateWindow(scrWidth, scrHeight, "LearnOpenGL", NULL, NULL);
    }

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Shader shader(getLocalPath("shader/shaderTest/shaderTest.vs").c_str(), getLocalPath("shader/shaderTest/shaderTest.fs").c_str());

    unsigned int textureID = loadTexture(getLocalPath("texture/test.jpg").c_str());

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderObject(shader, textureID);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // 限制渲染速率
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteBuffers(1, &objectVBO);
    glDeleteTextures(1, &textureID);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void renderObject(Shader &shader, unsigned int textureID) {
    static size_t vertexNum = 0;
    if (objectVAO == 0) {
        std::vector<float> Arr = readVertex();
        vertexNum = Arr.size();
        if (vertexNum == 0)
            return;
        glGenVertexArrays(1, &objectVAO);
        glGenBuffers(1, &objectVBO);
        glBindVertexArray(objectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertexNum, &Arr[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glBindVertexArray(0);
    }

    shader.use();
    shader.setFloat("time", (float)glfwGetTime());
    shader.setFloat("strength", 50.0f);
    shader.setFloat("speed", 50.0f);
    shader.setInt("inputTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(objectVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexNum / 4);
    glBindVertexArray(0);
    shader.unUse();
}

std::vector<float> readVertex() {
    const float vertices[] = {
        // 第一个三角形
        -1.0f, 1.0f, 0.0f, 0.0f,  // 左上
        -1.0f, -1.0f, 0.0f, 1.0f, // 左下
        1.0f, -1.0f, 1.0f, 1.0f,  // 右下

        // 第二个三角形
        -1.0f, 1.0f, 0.0f, 0.0f, // 左上
        1.0f, -1.0f, 1.0f, 1.0f, // 右下
        1.0f, 1.0f, 1.0f, 0.0f   // 右上
    };

    return std::vector<float>(vertices, vertices + 24);
}

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
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
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}