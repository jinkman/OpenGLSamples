#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>

int scrWidth = 800;
int scrHeight = 600;
#define PI 3.1415926f

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = scrWidth / 2.0f;
float lastY = scrHeight / 2.0f;
bool firstMouse = true;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderObject();
void leftMultiMatrix(float M0[][4], glm::vec3 P0[][4]);  // left multi
void rightMultiMatrix(glm::vec3 P0[][4], float M1[][4]); // right multi
void transposeMatrix(float M0[][4]);                     // transpose
void readQuadPoint(std::vector<float> &Arr);
unsigned int objectVAO = 0, objectVBO;
glm::vec3 P3[4][4]; // position
float mt[4][4];     // transpose of M

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
    } else
        window = glfwCreateWindow(scrWidth, scrHeight, "LearnOpenGL", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    lastX = scrWidth / 2.0f;
    lastY = scrHeight / 2.0f;

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
    Shader cloudShader(getLocalPath("shader/case1-cloud.vs").c_str(), getLocalPath("shader/case1-cloud.fs").c_str());

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 5000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model(1.0f);

        cloudShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -300.0f, 0.0));
        model = glm::scale(model, glm::vec3(1000));
        cloudShader.setMat4("projection", projection);
        cloudShader.setMat4("view", view);
        cloudShader.setMat4("model", model);
        cloudShader.setVec3("uSkyColor", glm::vec3(98 / 255.0f, 156 / 255.0f, 223 / 255.0f));
        cloudShader.setVec3("uCloudColor", glm::vec3(0.8, 0.8, 0.8));
        cloudShader.setFloat("uCloudSize", 30.0f);
        cloudShader.setFloat("uTime", (float)glfwGetTime() / 5.0f);
        renderObject();

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
    scrWidth = width;
    scrHeight = height;
}

void renderObject() {
    static size_t vertexNum = 0;
    if (objectVAO == 0) {
        std::vector<float> Arr;
        readQuadPoint(Arr);
        vertexNum = Arr.size();
        if (vertexNum == 0)
            return;
        glGenVertexArrays(1, &objectVAO);
        glGenBuffers(1, &objectVBO);
        glBindVertexArray(objectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertexNum, &Arr[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glBindVertexArray(0);
        Arr.clear();
    }
    glBindVertexArray(objectVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexNum / 5);
    glBindVertexArray(0);
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

void readQuadPoint(std::vector<float> &Arr) {
    // 初始化控制点
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            P3[i][j] = glm::vec3(i - 1.5f, 0.0f, j - 1.5f);
    P3[1][1].y = 1.0f;
    P3[1][2].y = 1.0f;
    P3[2][1].y = 1.0f;
    P3[2][2].y = 1.0f;

    float x, y, z, u, v, u1, u2, u3, u4, v1, v2, v3, v4;
    float m[4][4];
    m[0][0] = -1.0f;
    m[0][1] = 3.0f;
    m[0][2] = -3.0f;
    m[0][3] = 1.0f;
    m[1][0] = 3.0f;
    m[1][1] = -6.0f;
    m[1][2] = 3.0f;
    m[1][3] = 0.0f;
    m[2][0] = -3.0f;
    m[2][1] = 3.0f;
    m[2][2] = 0.0f;
    m[2][3] = 0.0f;
    m[3][0] = 1.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 0.0f;
    leftMultiMatrix(m, P3);
    transposeMatrix(m);
    rightMultiMatrix(P3, mt);

    int index = 0;
    glm::vec3 newP[25], oldP[25];
    for (u = 0; u <= 1.0f; u += 0.04f) {
        index = 0;
        for (v = 0; v <= 1.0f; v += 0.04f) {
            u1 = u * u * u;
            u2 = u * u;
            u3 = u;
            u4 = 1;
            v1 = v * v * v;
            v2 = v * v;
            v3 = v;
            v4 = 1;
            x = (u1 * P3[0][0].x + u2 * P3[1][0].x + u3 * P3[2][0].x + u4 * P3[3][0].x) * v1 + (u1 * P3[0][1].x + u2 * P3[1][1].x + u3 * P3[2][1].x + u4 * P3[3][1].x) * v2 + (u1 * P3[0][2].x + u2 * P3[1][2].x + u3 * P3[2][2].x + u4 * P3[3][2].x) * v3 + (u1 * P3[0][3].x + u2 * P3[1][3].x + u3 * P3[2][3].x + u4 * P3[3][3].x) * v4;
            y = (u1 * P3[0][0].y + u2 * P3[1][0].y + u3 * P3[2][0].y + u4 * P3[3][0].y) * v1 + (u1 * P3[0][1].y + u2 * P3[1][1].y + u3 * P3[2][1].y + u4 * P3[3][1].y) * v2 + (u1 * P3[0][2].y + u2 * P3[1][2].y + u3 * P3[2][2].y + u4 * P3[3][2].y) * v3 + (u1 * P3[0][3].y + u2 * P3[1][3].y + u3 * P3[2][3].y + u4 * P3[3][3].y) * v4;
            z = (u1 * P3[0][0].z + u2 * P3[1][0].z + u3 * P3[2][0].z + u4 * P3[3][0].z) * v1 + (u1 * P3[0][1].z + u2 * P3[1][1].z + u3 * P3[2][1].z + u4 * P3[3][1].z) * v2 + (u1 * P3[0][2].z + u2 * P3[1][2].z + u3 * P3[2][2].z + u4 * P3[3][2].z) * v3 + (u1 * P3[0][3].z + u2 * P3[1][3].z + u3 * P3[2][3].z + u4 * P3[3][3].z) * v4;
            newP[index] = glm::vec3(x, y, z);
            index++;
        }
        if (u > 0.0) {
            for (int i = 0; i < index - 1; i++) {
                // first triangle
                Arr.push_back(newP[i].x);
                Arr.push_back(newP[i].y);
                Arr.push_back(newP[i].z);
                Arr.push_back(u);
                Arr.push_back(i * 0.04f);
                Arr.push_back(oldP[i].x);
                Arr.push_back(oldP[i].y);
                Arr.push_back(oldP[i].z);
                Arr.push_back(u - 0.04f);
                Arr.push_back(i * 0.04f);
                Arr.push_back(oldP[i + 1].x);
                Arr.push_back(oldP[i + 1].y);
                Arr.push_back(oldP[i + 1].z);
                Arr.push_back(u - 0.04f);
                Arr.push_back((i + 1) * 0.04f);
                // second traingle
                Arr.push_back(oldP[i + 1].x);
                Arr.push_back(oldP[i + 1].y);
                Arr.push_back(oldP[i + 1].z);
                Arr.push_back(u - 0.04f);
                Arr.push_back((i + 1) * 0.04f);
                Arr.push_back(newP[i + 1].x);
                Arr.push_back(newP[i + 1].y);
                Arr.push_back(newP[i + 1].z);
                Arr.push_back(u);
                Arr.push_back((i + 1) * 0.04f);
                Arr.push_back(newP[i].x);
                Arr.push_back(newP[i].y);
                Arr.push_back(newP[i].z);
                Arr.push_back(u);
                Arr.push_back(i * 0.04f);
            }
        }
        for (int i = 0; i < index; i++)
            oldP[i] = newP[i];
    }
}

void leftMultiMatrix(float M0[][4], glm::vec3 P0[][4]) {
    glm::vec3 T[4][4]; // temp
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            T[i][j].x = M0[i][0] * P0[0][j].x + M0[i][1] * P0[1][j].x + M0[i][2] * P0[2][j].x + M0[i][3] * P0[3][j].x;
            T[i][j].y = M0[i][0] * P0[0][j].y + M0[i][1] * P0[1][j].y + M0[i][2] * P0[2][j].y + M0[i][3] * P0[3][j].y;
            T[i][j].z = M0[i][0] * P0[0][j].z + M0[i][1] * P0[1][j].z + M0[i][2] * P0[2][j].z + M0[i][3] * P0[3][j].z;
        }
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            P3[i][j] = T[i][j];
}

void rightMultiMatrix(glm::vec3 P0[][4], float M1[][4]) {
    glm::vec3 T[4][4]; // temp
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            T[i][j].x = P0[i][0].x * M1[0][j] + P0[i][1].x * M1[1][j] + P0[i][2].x * M1[2][j] + P0[i][3].x * M1[3][j];
            T[i][j].y = P0[i][0].y * M1[0][j] + P0[i][1].y * M1[1][j] + P0[i][2].y * M1[2][j] + P0[i][3].y * M1[3][j];
            T[i][j].z = P0[i][0].z * M1[0][j] + P0[i][1].z * M1[1][j] + P0[i][2].z * M1[2][j] + P0[i][3].z * M1[3][j];
        }
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            P3[i][j] = T[i][j];
}

void transposeMatrix(float M0[][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            mt[j][i] = M0[i][j];
}