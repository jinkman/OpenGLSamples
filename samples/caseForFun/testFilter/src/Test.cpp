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

int scrWidth = 1280;
int scrHeight = 720;

#define PI 3.1415926f

bool firstMouse = true;
float lastX = scrWidth / 2.0f;
float lastY = scrHeight / 2.0f;

float mixFactor = 0.5f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void renderObject(Shader &shader, const unsigned int &texSrc);
void readVertex(std::vector<float> &Arr);
unsigned int createTexture();
void cvmatToTexture(GLuint &textureId, const cv::Mat &mat);
void colorTransfer(const cv::Mat &sMat, const cv::Mat &dMat, cv::Mat &matRet);
unsigned int create3DTexFromCube(const std::string &path);

unsigned int objectVAO = 0, objectVBO;

unsigned int lutTex = 0;
unsigned int lutSize = 16;

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
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    lastX = scrWidth / 2.0f;
    lastY = scrHeight / 2.0f;

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

    Shader shader(getLocalPath("shader/testFilter.vs").c_str(), getLocalPath("shader/testFilter.fs").c_str());

    auto texSrc = createTexture();

    VideoCapture cap(0);
    Mat frame;

    lutTex = create3DTexFromCube(getLocalPath("texture/07_Davinci Resolve LUTs_Sunset.cube"));
    glEnable(GL_TEXTURE_3D);
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::SliderFloat("float", &mixFactor, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

        cap >> frame;
        if (frame.empty()) {
            break;
        }

        cvmatToTexture(texSrc, frame);

        shader.use();
        shader.setFloat("mixFactor", mixFactor);

        renderObject(shader, texSrc);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteBuffers(1, &objectVBO);
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

void renderObject(Shader &shader, const unsigned int &texSrc) {
    static size_t vertexNum = 0;
    if (objectVAO == 0) {
        std::vector<float> Arr;
        readVertex(Arr);
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
    shader.setInt("texSrc", 0);
    shader.setInt("lutTex", 1);
    shader.setInt("lutSize", lutSize);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texSrc);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, lutTex);

    glBindVertexArray(objectVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexNum / 4);
    glBindVertexArray(0);
}

void readVertex(std::vector<float> &Arr) {
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

void colorTransfer(const cv::Mat &sMat, const cv::Mat &dMat, cv::Mat &matRet) {
    Mat sMeanMat, sStddevMat;
    meanStdDev(sMat, sMeanMat, sStddevMat);

    Mat dMeanMat, dStddevMat;
    meanStdDev(dMat, dMeanMat, dStddevMat);

    Mat temp(sMat.rows, sMat.cols, CV_8UC3, Scalar(0, 0, 0));
    for (int row = 0; row < sMat.rows; row++) {
        for (int col = 0; col < sMat.cols; col++) {
            if (sMat.channels() == 3) {
                int b = sMat.at<Vec3b>(row, col)[0];
                int g = sMat.at<Vec3b>(row, col)[1];
                int r = sMat.at<Vec3b>(row, col)[2];

                temp.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(((b - sMeanMat.at<double>(0, 0)) * (dStddevMat.at<double>(0, 0) / sStddevMat.at<double>(0, 0))) + dMeanMat.at<double>(0, 0));
                temp.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(((g - sMeanMat.at<double>(0, 1)) * (dStddevMat.at<double>(0, 1) / sStddevMat.at<double>(0, 1))) + dMeanMat.at<double>(0, 1));
                temp.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(((r - sMeanMat.at<double>(0, 2)) * (dStddevMat.at<double>(0, 2) / sStddevMat.at<double>(0, 2))) + dMeanMat.at<double>(0, 2));
            }
        }
    }
    cvtColor(temp, matRet, COLOR_Lab2BGR);
}

unsigned int create3DTexFromCube(const std::string &path) {
    // Load the LUT
    std::vector<glm::vec3> LUT;

    std::ifstream LUTfile(path.c_str());

    while (!LUTfile.eof()) {
        std::string LUTline;
        std::getline(LUTfile, LUTline);

        if (LUTline.empty())
            continue;

        glm::vec3 line;
        if (sscanf(LUTline.c_str(), "%f %f %f", &line.x, &line.y, &line.z) == 3)
            LUT.push_back(line);
    }
    if (LUT.size() != (pow(lutSize, 3.0))) {
        std::cout << "LUT size is incorrect." << std::endl;
        return 0;
    }

    unsigned int texture3D = 0;
    glGenTextures(1, &texture3D);
    glBindTexture(GL_TEXTURE_3D, texture3D);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, lutSize, lutSize, lutSize, 0, GL_RGB,
                 GL_FLOAT, &LUT[0]);

    glBindTexture(GL_TEXTURE_3D, 0);
    return texture3D;
}