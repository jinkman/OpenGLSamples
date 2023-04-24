#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <sstream>

#include <opencv2/opencv.hpp>
using namespace cv;

// imgui

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

#define PI 3.1415926f

bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float mixFactor = 0.5f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

unsigned int objectVAO = 0, objectVBO = 0;
unsigned int srcVAO = 0, srcVBO = 0;

// lut设置
const float lutSamples = 32.0f;
float actualSamples = lutSamples;
static bool bUseLut = true;
static bool bUseFbo = true;

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width = 0, height = 0, nrComponents = 0;
    // stbi_set_flip_vertically_on_load(true);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int createTexture() {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void cvmatToTexture(GLuint &textureId, const cv::Mat &mat) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    Mat sMat;
    cvtColor(mat, sMat, COLOR_BGR2RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sMat.cols, sMat.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, sMat.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void rendObject(Shader &shader, const unsigned int &texSrc, const unsigned int &lutTex) {
    static size_t vertextNum = 0;
    if (srcVAO == 0) {
        std::vector<float> Arr;
        Arr.push_back(-1.0f);
        Arr.push_back(1.0f);
        Arr.push_back(0.0f);
        Arr.push_back(0.0f);

        Arr.push_back(-1.0f);
        Arr.push_back(-1.0f);
        Arr.push_back(0.0f);
        Arr.push_back(1.0f);

        Arr.push_back(1.0f);
        Arr.push_back(-1.0f);
        Arr.push_back(1.0f);
        Arr.push_back(1.0f);

        Arr.push_back(-1.0f);
        Arr.push_back(1.0f);
        Arr.push_back(0.0f);
        Arr.push_back(0.0f);

        Arr.push_back(1.0f);
        Arr.push_back(-1.0f);
        Arr.push_back(1.0f);
        Arr.push_back(1.0f);

        Arr.push_back(1.0f);
        Arr.push_back(1.0f);
        Arr.push_back(1.0f);
        Arr.push_back(0.0f);
        vertextNum = Arr.size();
        if (vertextNum == 0)
            return;
        glGenVertexArrays(1, &srcVAO);
        glGenBuffers(1, &srcVBO);
        glBindVertexArray(srcVAO);
        glBindBuffer(GL_ARRAY_BUFFER, srcVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum, &Arr[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glBindVertexArray(0);
    }
    shader.use();
    shader.setInt("texSrc", 0);
    shader.setInt("lutTex", 1);
    shader.setBool("bUseLut", bUseLut);
    shader.setFloat("lutSize", actualSamples);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texSrc);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, lutTex);

    glBindVertexArray(srcVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    bool isFullScreen = false;
    GLFWwindow *window = NULL;
    if (isFullScreen) {
        const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        SCR_WIDTH = vidmode->width;
        SCR_HEIGHT = vidmode->height;
        GLFWmonitor *pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", pMonitor, NULL);
        lastX = SCR_WIDTH / 2.0f;
        lastY = SCR_HEIGHT / 2.0f;
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
    float wid = lutSamples * lutSamples;
    float hei = lutSamples;

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    Shader frameBufferShader(getLocalPath("shader/createLutFbo.vs").c_str(), getLocalPath("shader/createLutFbo.fs").c_str());
    Shader srceenShader(getLocalPath("shader/createLutSrc.vs").c_str(), getLocalPath("shader/createLutSrc.fs").c_str());

    // 创建rtt
    unsigned int framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create color attachment
    unsigned int textureColorbuffer = 0;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, int(wid), int(hei), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // check if the frame is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind to default fbo

    auto lutTex = loadTexture(getLocalPath("texture/RGBTable16x1.png").c_str());
    auto texSrc = createTexture();

    VideoCapture cap(0);
    Mat frame;

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static std::string mText = "use fbo";
        unsigned int useLut = 0;
        if (bUseFbo) {
            mText = "use fbo";
            useLut = textureColorbuffer;
            actualSamples = lutSamples;
        } else {
            mText = "use input";
            useLut = lutTex;
            actualSamples = 16.0f;
        }

        ImGui::Checkbox(mText.c_str(), &bUseFbo);
        static float hsvV[3] = {0.0f, 0.0f, 0.0f};
        ImGui::SliderFloat("h", &hsvV[0], 0.0f, 1.0f);
        ImGui::SliderFloat("s", &hsvV[1], -1.0f, 1.0f);
        ImGui::SliderFloat("v", &hsvV[2], -1.0f, 1.0f);
        static float scaleV = 2.5f;
        ImGui::SliderFloat("scale", &scaleV, 1.0f, 20.0f);
        ImGui::Checkbox("bUseLut", &bUseLut);

        if (objectVAO == 0) {
            float vertArr[] = {0.0, 0.0f, 0.0f, 0.0f, 0.0f, hei, 0.0f, 1.0f, wid, hei, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, wid, hei, 1.0f, 1.0f, wid, 0.0f, 1.0f, 0.0f};
            glGenVertexArrays(1, &objectVAO);
            glGenBuffers(1, &objectVBO);
            glBindVertexArray(objectVAO);
            glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
            glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertArr, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
            glBindVertexArray(0);
        }

        // 绘制到fbo
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        frameBufferShader.use();
        frameBufferShader.setMat4("projection", glm::ortho(0.0f, wid, 0.0f, hei));
        frameBufferShader.setFloat("samples", lutSamples);
        frameBufferShader.setVec3("hsvV", glm::vec3(hsvV[0], hsvV[1], hsvV[2]));
        glViewport(0, 0, wid, hei);
        glBindVertexArray(objectVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui::GetWindowDrawList()->AddImage(
            (void *)useLut,
            ImVec2(ImGui::GetCursorScreenPos()),
            ImVec2(ImGui::GetCursorScreenPos().x + actualSamples * actualSamples * scaleV,
                   ImGui::GetCursorScreenPos().y + actualSamples * scaleV),
            ImVec2(0, 1), ImVec2(1, 0));

        // 绘制到屏幕 滤镜
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        cvmatToTexture(texSrc, frame);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        rendObject(srceenShader, texSrc, useLut);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteBuffers(1, &objectVBO);
    glDeleteVertexArrays(1, &srcVAO);
    glDeleteBuffers(1, &srcVBO);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteTextures(1, &lutTex);
    glDeleteTextures(1, &texSrc);
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