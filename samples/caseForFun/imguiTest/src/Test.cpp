#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>

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
void renderObject(Shader &shader, const unsigned int &texSrc, const unsigned int &charsTexID);
void readVertex(std::vector<float> &Arr);
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

    auto tex = loadTexture(getLocalPath("texture/test.jpg").c_str());

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Begin("Scene Window");
        // get the mouse position
        ImVec2 pos = ImGui::GetCursorScreenPos();

        // pass the texture of the FBO
        // window.getrenderTexture() is the texture of the FBO
        // the next parameter is the upper left corner for the uvs to be applied at
        // the third parameter is the lower right corner
        // the last two parameters are the UVs
        // they have to be flipped (normally they would be (0,0);(1,1)
        ImGui::GetWindowDrawList()->AddImage(
            (void *)tex,
            ImVec2(ImGui::GetCursorScreenPos()),
            ImVec2(ImGui::GetCursorScreenPos().x + scrWidth / 2,
                   ImGui::GetCursorScreenPos().y + scrHeight / 2),
            ImVec2(0, 1), ImVec2(1, 0));

        // we are done working with this window
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

void renderObject(Shader &shader, const unsigned int &texSrc, const unsigned int &charsTexID) {
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texSrc);

    shader.setInt("charsTex", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, charsTexID);

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
