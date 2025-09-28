// Test.cpp : 定义控制台应用程序的入口点。
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>
using namespace std;
using namespace glm;

int scrWidth = 800;
int scrHeight = 600;
#define PI 3.1415926

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool firstMouse = true;
Camera camera(vec3(0.0f, 0.0f, 5.0f));
float lastX = scrWidth / 2.0f;
float lastY = scrHeight / 2.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void renderObject();
void readVertex();
void bindFace(int i, int texNum);
void readFace(vec3 p0, vec2 t0, vec3 p1, vec2 t1, vec3 p2, vec2 t2);
unsigned int loadTexture(char const *path);

vector<vec3> vArr;  // 顶点数据
vector<float> fArr; // 面表数据
unsigned int stampVAO[26] = {0}, stampVBO[26];
unsigned int textureVAO = 0, textureVBO;
bool ifRotate = false; // 是否旋转
float Angle = 0.0f;    // 旋转角度
bool dir = false;      // 旋转方向
bool light = true;     // 光照模型

unsigned int texIndex[26];   // 面纹理索引
size_t vertexNum[26];       // 顶点数目
unsigned int diffuseMap[15]; // 纹理数组
unsigned int depthMap[15];   // 深度纹理
unsigned int normalMap[15];  // 深度纹理

float heightScale = 0.1f;

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
        lastX = scrWidth / 2.0f;
        lastY = scrHeight / 2.0f;
    } else
        window = glfwCreateWindow(scrWidth, scrHeight, "LearnOpenGL", NULL, NULL);

    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
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
        cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    Shader depthShader(getLocalPath("shader/case11-depthBuffer.vs").c_str(), getLocalPath("shader/case11-depthBuffer.fs").c_str());
    Shader normalShader(getLocalPath("shader/case11-normalBuffer.vs").c_str(), getLocalPath("shader/case11-normalBuffer.fs").c_str());
    Shader stampShader(getLocalPath("shader/case11-stamp.vs").c_str(), getLocalPath("shader/case11-stamp.fs").c_str());
    Shader textureShader(getLocalPath("shader/case11-texture.vs").c_str(), getLocalPath("shader/case11-texture.fs").c_str());
    depthShader.use();
    depthShader.setInt("diffuseMap", 0);
    normalShader.use();
    normalShader.setInt("depthMap", 0);
    stampShader.use();
    stampShader.setInt("diffuseMap", 0);
    stampShader.setInt("depthMap", 1);
    stampShader.setInt("normalMap", 2);
    textureShader.use();
    textureShader.setInt("diffuseMap", 0);
    // 读取纹理
    diffuseMap[0] = loadTexture(getLocalPath("texture/yechi.png").c_str());
    diffuseMap[1] = loadTexture(getLocalPath("texture/ling.png").c_str());
    diffuseMap[2] = loadTexture(getLocalPath("texture/mi.png").c_str());
    diffuseMap[3] = loadTexture(getLocalPath("texture/duguxinbaishu.png").c_str());
    diffuseMap[4] = loadTexture(getLocalPath("texture/xinbaijian.png").c_str());
    diffuseMap[5] = loadTexture(getLocalPath("texture/chenxinshangshu.png").c_str());
    diffuseMap[6] = loadTexture(getLocalPath("texture/chenxinshangzhang.png").c_str());
    diffuseMap[7] = loadTexture(getLocalPath("texture/chenxinshangbiao.png").c_str());
    diffuseMap[8] = loadTexture(getLocalPath("texture/xinqishi.png").c_str());
    diffuseMap[9] = loadTexture(getLocalPath("texture/chenxinqishi.png").c_str());
    diffuseMap[10] = loadTexture(getLocalPath("texture/dasimayin.png").c_str());
    diffuseMap[11] = loadTexture(getLocalPath("texture/daduduyin.png").c_str());
    diffuseMap[12] = loadTexture(getLocalPath("texture/cishizhiyin.png").c_str());
    diffuseMap[13] = loadTexture(getLocalPath("texture/zhuguozhiyin.png").c_str());
    diffuseMap[14] = loadTexture(getLocalPath("texture/bk.png").c_str());
    readVertex();
    // read face data
    // top
    readFace(vArr[0], vec2(1.0, 1.0), vArr[1], vec2(0.0, 1.0), vArr[2], vec2(0.0, 0.0)); // 0 1 2	chenxinshangbiao
    readFace(vArr[0], vec2(1.0, 1.0), vArr[2], vec2(0.0, 0.0), vArr[3], vec2(1.0, 0.0)); // 0 2 3
    bindFace(0, 7);
    // up
    readFace(vArr[0], vec2(0.0, 0.0), vArr[4], vec2(1.0, 0.0), vArr[5], vec2(1.0, 1.0)); // 0 4 5 triangle
    bindFace(1, 14);
    readFace(vArr[5], vec2(1.0, 1.0), vArr[6], vec2(0.0, 1.0), vArr[1], vec2(0.0, 0.0)); // 5 6 1	  chenxinshangzhang
    readFace(vArr[5], vec2(1.0, 1.0), vArr[1], vec2(0.0, 0.0), vArr[0], vec2(1.0, 0.0)); // 5 1 0
    bindFace(2, 6);
    readFace(vArr[1], vec2(0.0, 0.0), vArr[6], vec2(1.0, 0.0), vArr[7], vec2(1.0, 1.0)); // 1 6 7 triangle
    bindFace(3, 14);
    readFace(vArr[7], vec2(0.0, 1.0), vArr[8], vec2(0.0, 0.0), vArr[2], vec2(1.0, 0.0)); // 7 8 2
    readFace(vArr[7], vec2(0.0, 1.0), vArr[2], vec2(1.0, 0.0), vArr[1], vec2(1.0, 1.0)); // 7 2 1
    bindFace(4, 14);
    readFace(vArr[2], vec2(0.0, 0.0), vArr[8], vec2(1.0, 0.0), vArr[9], vec2(1.0, 1.0)); // 2 8 9 triangle
    bindFace(5, 14);
    readFace(vArr[9], vec2(0.0, 0.0), vArr[10], vec2(1.0, 0.0), vArr[3], vec2(1.0, 1.0)); // 9 10 3	  chenxinqishi
    readFace(vArr[9], vec2(0.0, 0.0), vArr[3], vec2(1.0, 1.0), vArr[2], vec2(0.0, 1.0));  // 9 3 2
    bindFace(6, 9);
    readFace(vArr[3], vec2(0.0, 0.0), vArr[10], vec2(1.0, 0.0), vArr[11], vec2(1.0, 1.0)); // 3 10 11 triangle
    bindFace(7, 14);
    readFace(vArr[11], vec2(1.0, 0.0), vArr[4], vec2(1.0, 1.0), vArr[0], vec2(0.0, 1.0)); // 11 4 0	  xinqishi
    readFace(vArr[11], vec2(1.0, 0.0), vArr[0], vec2(0.0, 1.0), vArr[3], vec2(0.0, 0.0)); // 11 0 3
    bindFace(8, 8);
    // medium
    readFace(vArr[12], vec2(0.0, 0.0), vArr[13], vec2(1.0, 0.0), vArr[5], vec2(1.0, 1.0)); // 12 13 5
    readFace(vArr[12], vec2(0.0, 0.0), vArr[5], vec2(1.0, 1.0), vArr[4], vec2(0.0, 1.0));  // 12 5 4
    bindFace(9, 14);
    readFace(vArr[13], vec2(1.0, 1.0), vArr[14], vec2(0.0, 1.0), vArr[6], vec2(0.0, 0.0)); // 13 14 6	   chenxinshangshu
    readFace(vArr[13], vec2(1.0, 1.0), vArr[6], vec2(0.0, 0.0), vArr[5], vec2(1.0, 0.0));  // 13 6 5
    bindFace(10, 5);
    readFace(vArr[14], vec2(0.0, 0.0), vArr[15], vec2(1.0, 0.0), vArr[7], vec2(1.0, 1.0)); // 14 15 7
    readFace(vArr[14], vec2(0.0, 0.0), vArr[7], vec2(1.0, 1.0), vArr[6], vec2(0.0, 1.0));  // 14 7 6
    bindFace(11, 14);
    readFace(vArr[15], vec2(0.0, 1.0), vArr[16], vec2(0.0, 0.0), vArr[8], vec2(1.0, 0.0)); // 15 16 8	   xinbaijian
    readFace(vArr[15], vec2(0.0, 1.0), vArr[8], vec2(1.0, 0.0), vArr[7], vec2(1.0, 1.0));  // 15 8 7
    bindFace(12, 4);
    readFace(vArr[16], vec2(0.0, 0.0), vArr[17], vec2(1.0, 0.0), vArr[9], vec2(1.0, 1.0)); // 16 17 9	   mi
    readFace(vArr[16], vec2(0.0, 0.0), vArr[9], vec2(1.0, 1.0), vArr[8], vec2(0.0, 1.0));  // 16 9 8
    bindFace(13, 2);
    readFace(vArr[17], vec2(0.0, 0.0), vArr[18], vec2(1.0, 0.0), vArr[10], vec2(1.0, 1.0)); // 17 18 10	   dasimayin
    readFace(vArr[17], vec2(0.0, 0.0), vArr[10], vec2(1.0, 1.0), vArr[9], vec2(0.0, 1.0));  // 17 10 9
    bindFace(14, 10);
    readFace(vArr[18], vec2(0.0, 0.0), vArr[19], vec2(1.0, 0.0), vArr[11], vec2(1.0, 1.0)); // 18 19 11	   ling
    readFace(vArr[18], vec2(0.0, 0.0), vArr[11], vec2(1.0, 1.0), vArr[10], vec2(0.0, 1.0)); // 18 11 10
    bindFace(15, 1);
    readFace(vArr[19], vec2(1.0, 0.0), vArr[12], vec2(1.0, 1.0), vArr[4], vec2(0.0, 1.0)); // 19 12 4	   duguxinbaishu
    readFace(vArr[19], vec2(1.0, 0.0), vArr[4], vec2(0.0, 1.0), vArr[11], vec2(0.0, 0.0)); // 19 4 11
    bindFace(16, 3);
    // down
    readFace(vArr[12], vec2(0.0, 0.0), vArr[20], vec2(1.0, 0.0), vArr[13], vec2(1.0, 1.0)); // 12 20 13	triangle
    bindFace(17, 14);
    readFace(vArr[20], vec2(1.0, 1.0), vArr[21], vec2(0.0, 1.0), vArr[14], vec2(0.0, 0.0)); // 20 21 14	   zhuguozhiyin
    readFace(vArr[20], vec2(1.0, 1.0), vArr[14], vec2(0.0, 0.0), vArr[13], vec2(1.0, 0.0)); // 20 14 13
    bindFace(18, 13);
    readFace(vArr[14], vec2(0.0, 0.0), vArr[21], vec2(1.0, 0.0), vArr[15], vec2(1.0, 1.0)); // 14 21 15	triangle
    bindFace(19, 14);
    readFace(vArr[21], vec2(0.0, 0.0), vArr[22], vec2(1.0, 0.0), vArr[16], vec2(1.0, 1.0)); // 21 22 16
    readFace(vArr[21], vec2(0.0, 0.0), vArr[16], vec2(1.0, 1.0), vArr[15], vec2(0.0, 1.0)); // 21 16 15
    bindFace(20, 14);
    readFace(vArr[16], vec2(0.0, 0.0), vArr[22], vec2(1.0, 0.0), vArr[17], vec2(1.0, 1.0)); // 16 22 17	triangle
    bindFace(21, 14);
    readFace(vArr[22], vec2(0.0, 0.0), vArr[23], vec2(1.0, 0.0), vArr[18], vec2(1.0, 1.0)); // 22 23 18	   daduduyin
    readFace(vArr[22], vec2(0.0, 0.0), vArr[18], vec2(1.0, 1.0), vArr[17], vec2(0.0, 1.0)); // 22 18 17
    bindFace(22, 11);
    readFace(vArr[18], vec2(0.0, 0.0), vArr[23], vec2(1.0, 0.0), vArr[19], vec2(1.0, 1.0)); // 18 23 19	triangle
    bindFace(23, 14);
    readFace(vArr[23], vec2(0.0, 0.0), vArr[20], vec2(1.0, 0.0), vArr[12], vec2(1.0, 1.0)); // 23 20 12
    readFace(vArr[23], vec2(0.0, 0.0), vArr[12], vec2(1.0, 1.0), vArr[19], vec2(0.0, 1.0)); // 23 12 19
    bindFace(24, 14);
    // bottom
    readFace(vArr[20], vec2(1.0, 0.0), vArr[23], vec2(1.0, 1.0), vArr[22], vec2(0.0, 1.0)); // 20 23 22	   cishizhiyin
    readFace(vArr[22], vec2(0.0, 1.0), vArr[21], vec2(0.0, 0.0), vArr[20], vec2(1.0, 0.0)); // 22 21 20
    bindFace(25, 12);
    vArr.clear();

    unsigned int depthBuffer[15];
    glGenFramebuffers(15, &depthBuffer[0]);
    glGenTextures(15, &depthMap[0]);
    for (int i = 0; i < 15; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer[i]);
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 400, 400, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthMap[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int normalBuffer[15];
    glGenFramebuffers(15, &normalBuffer[0]);
    glGenTextures(15, &normalMap[0]);
    for (int i = 0; i < 15; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, normalBuffer[i]);
        glBindTexture(GL_TEXTURE_2D, normalMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 400, 400, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glViewport(0, 0, 400, 400);
        for (int i = 0; i < 15; i++) {
            // 制作深度图
            glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer[i]);
            glClear(GL_COLOR_BUFFER_BIT);

            depthShader.use();
            depthShader.setFloat("heightScale", heightScale);
            depthShader.setMat4("model", glm::mat4(1.0f));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap[i]);
            renderObject();
        }

        for (int i = 0; i < 15; i++) {
            // 制作法线图
            glBindFramebuffer(GL_FRAMEBUFFER, normalBuffer[i]);
            glClear(GL_COLOR_BUFFER_BIT);

            normalShader.use();
            normalShader.setMat4("model", glm::mat4(1.0f));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap[i]);
            renderObject();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, scrWidth, scrHeight);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*textureShader.use();
        textureShader.setMat4("model",glm::mat4());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normalMap[1]);
        renderObject();*/

        if (ifRotate) {
            if ((Angle += 0.02f) >= 360.0f) {
                Angle = 0.0f;
                dir = !dir;
            }
        }

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model(1.0f);
        stampShader.use();
        if (dir)
            model = rotate(model, float(radians(Angle)), vec3(0.0, 1.0, 0.0));
        else
            model = rotate(model, float(radians(Angle)), vec3(1.0, 0.0, 0.0));
        stampShader.setMat4("projection", projection);
        stampShader.setMat4("view", view);
        stampShader.setMat4("model", model);
        stampShader.setVec3("viewPos", camera.Position);
        stampShader.setVec3("lightPos", glm::vec3(0, 0, 5));
        stampShader.setBool("light", light);
        for (int i = 0; i < 26; i++) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap[texIndex[i]]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap[texIndex[i]]);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, normalMap[texIndex[i]]);
            glBindVertexArray(stampVAO[i]);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexNum[i] / 14);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    for (int i = 0; i < 26; i++) {
        glDeleteVertexArrays(1, &stampVAO[i]);
        glDeleteBuffers(1, &stampVBO[i]);
    }
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteBuffers(1, &textureVBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    static float speed = 0.7f;
    static bool spaceKey = false;
    static bool cKey = false;
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && spaceKey == false) {
        spaceKey = true;
        ifRotate = !ifRotate;
        Angle = 0.0;
        dir = false;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && spaceKey == true)
        spaceKey = false;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && cKey == false) {
        cKey = true;
        light = !light;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && cKey == true)
        cKey = false;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (heightScale > 0.0f)
            heightScale -= 0.005f;
        else
            heightScale = 0.0f;
    } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (heightScale < 2.0f)
            heightScale += 0.005f;
        else
            heightScale = 2.0f;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
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

void renderObject() {
    static size_t Num = 0;

    if (textureVAO == 0) {
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
        Num = Arr.size();
        glGenVertexArrays(1, &textureVAO);
        glGenBuffers(1, &textureVBO);
        glBindVertexArray(textureVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * Num, &Arr[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glBindVertexArray(0);
        Arr.clear();
    }
    glBindVertexArray(textureVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)Num / 4);
    glBindVertexArray(0);
}

void readVertex() {
    int nIndex[4] = {4, 8, 8, 4}; // vertices of per level
    int level = 4;
    // cal the vertices data
    for (int i = 0; i < level; i++) {
        float interval = 360.0f / nIndex[i]; // step angle
        // current y
        float y = sin(radians(67.5f - i * 45.0f));                                   // Are octagon
        float radius = cos(radians(67.5f - i * 45.0f)) / cos(radians(interval / 2)); // Are octagon
        // radius=fabs(radius);
        for (int j = 0; j < nIndex[i]; j++) {
            float Angle = interval * j + interval / 2;
            float x = radius * cos(radians(Angle));
            float z = -radius * sin(radians(Angle));
            vArr.push_back(vec3(x, y, z));
        }
    }
}

void readFace(vec3 p0, vec2 t0, vec3 p1, vec2 t1, vec3 p2, vec2 t2) {
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
    glm::vec3 Normal = glm::cross(edge1, edge2);

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

void bindFace(int i, int texNum) {
    vertexNum[i] = fArr.size();
    if (vertexNum[i] == 0)
        return;
    texIndex[i] = texNum;
    glGenVertexArrays(1, &stampVAO[i]);
    glGenBuffers(1, &stampVBO[i]);
    glBindVertexArray(stampVAO[i]);
    glBindBuffer(GL_ARRAY_BUFFER, stampVBO[i]);
    glBufferData(GL_ARRAY_BUFFER, 4 * vertexNum[i], &fArr[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(11 * sizeof(float)));
    glBindVertexArray(0);
    fArr.clear();
}

unsigned int loadTexture(char const *path) {
    stbi_set_flip_vertically_on_load(true);
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