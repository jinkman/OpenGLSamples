
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
void rendObject(Shader &shader, const unsigned int &texSrc, const unsigned int &charsTexID);
void readVertext(std::vector<float> &Arr);
unsigned int createTexture();
void cvmatToTexture(GLuint &textureId, const cv::Mat &mat);
void colorTransfer(const cv::Mat &sMat, const cv::Mat &dMat, cv::Mat &matRet);
std::vector<unsigned int> generateCharacter(const std::string &text);
unsigned int loadTexture(char const *path);

unsigned int objectVAO = 0, objectVBO;
const unsigned int charSize = 64;

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

    Shader shader(getLocalPath("shader/case13-screen.vs").c_str(), getLocalPath("shader/case13-screen.fs").c_str());

    auto texSrc = createTexture();

    VideoCapture cap(0);
    Mat frame;

    //@W#$OEXC[(/?=^~_.`
    // std::vector<unsigned int> textTexArray = generateCharacter("@W#$OEXC[(/?=^~_.`");
    // std::vector<unsigned int> textTexArray = generateCharacter("_=?([CXOE$#W@");

    unsigned int charsTexID = loadTexture(getLocalPath("texture/chars.png").c_str());

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static float uPiexlSize = 10.0f;
        ImGui::SliderFloat("mixFactor", &mixFactor, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("pixelSize", &uPiexlSize, 2.0f, 100.0f);

        cap >> frame;
        if (frame.empty()) {
            break;
        }

        cvmatToTexture(texSrc, frame);

        shader.use();
        shader.setFloat("mixFactor", mixFactor);
        shader.setFloat("pixelSize", uPiexlSize);
        shader.setVec2("resolution", glm::vec2(float(SCR_WIDTH), float(SCR_HEIGHT)));

        rendObject(shader, texSrc, charsTexID);

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

void rendObject(Shader &shader, const unsigned int &texSrc, const unsigned int &charsTexID) {
    static size_t vertextNum = 0;
    if (objectVAO == 0) {
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
    shader.use();
    shader.setInt("texSrc", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texSrc);

    shader.setInt("charsTex", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, charsTexID);

    glBindVertexArray(objectVAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertextNum / 4);
    glBindVertexArray(0);
}

void readVertext(std::vector<float> &Arr) {
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

std::vector<unsigned int> generateCharacter(const std::string &text) {
    // FreeType
    FT_Library ft;
    FT_Face face;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    if (FT_New_Face(ft, getLocalPath("font/font.ttf").c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, 0, charSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::vector<unsigned int> ret;
    for (int i = 0; i < text.size(); i++) {
        auto ch = text.at(i);
        if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        ret.push_back(texture);
    }
    return ret;
}

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    stbi_set_flip_vertically_on_load(false);
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
