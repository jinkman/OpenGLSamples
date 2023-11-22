#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
#define PI 3.1415926f

Camera camera(glm::vec3(0.0f, 0.0f, -1000.0f), glm::vec3(0.0, 1.0, 0.0), 90.0f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void rendObject();
void readVertext(std::vector<float> &Arr);
unsigned int loadCubemap(std::vector<std::string> faces);

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
        SCR_WIDTH = vidmode->width;
        SCR_HEIGHT = vidmode->height;
        GLFWmonitor *pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", pMonitor, NULL);
    } else
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    lastX = SCR_WIDTH / 2.0f;
    lastY = SCR_HEIGHT / 2.0f;

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    Shader shader(getLocalPath("shader/case9-object.vs").c_str(), getLocalPath("shader/case9-object.fs").c_str());
    shader.use();
    shader.setInt("Map", 0);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setFloat("time", (float)glfwGetTime());
        shader.setVec2("resolution", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
        // camera info
        shader.setVec3("cameraPosition", camera.Position);
        shader.setVec3("cameraFront", camera.Front);
        shader.setVec3("cameraRight", camera.Right);
        shader.setVec3("cameraUp", camera.Up);
        rendObject();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteBuffers(1, &objectVBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    static float speed = 10.0f;
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
        speed += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        speed -= 1.0f;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void rendObject() {
    static size_t vertextNum = 0;
    static unsigned int cubemapTexture = 0;
    if (objectVAO == 0) {
        std::vector<std::string> faces;
        faces.push_back(getLocalPath("skybox/moon_cuberight.jpg"));
        faces.push_back(getLocalPath("skybox/moon_cubeleft.jpg"));
        faces.push_back(getLocalPath("skybox/moon_cubetop.jpg"));
        faces.push_back(getLocalPath("skybox/moon_cubebottom.jpg"));
        faces.push_back(getLocalPath("skybox/moon_cubefront.jpg"));
        faces.push_back(getLocalPath("skybox/moon_cubeback.jpg"));
        cubemapTexture = loadCubemap(faces);
        faces.clear();
        std::vector<float> Arr;
        readVertext(Arr);
        vertextNum = Arr.size();
        if (vertextNum == 0)
            return;
        glGenVertexArrays(1, &objectVAO);
        glGenBuffers(1, &objectVBO);
        glBindVertexArray(objectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &Arr[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glBindVertexArray(0);
    }
    glBindVertexArray(objectVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertextNum / 4);
    glBindVertexArray(0);
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void readVertext(std::vector<float> &Arr) {
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
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll((float)yoffset);
}
