#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <camera.h>
#include <shader_s.h>
#include <stb_image.h>
#include <common.h>
#include <vector>
using namespace std;
using namespace glm;

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
#define PI 3.1415926f

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool firstMouse = true;
Camera camera(vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void readVertext();
void bindFace(int i, int texNum);
void readFace(vec3 p0, vec2 t0, vec3 p1, vec2 t1, vec3 p2, vec2 t2);
unsigned int loadTexture(char const *path);

vector<vec3> vArr;  // vertices
vector<float> fArr; // face data
unsigned int stampVAO[26] = {0}, stampVBO[26];

// input
bool ifRotate = false;
float Angle = 0.0f;
bool dir = false;

unsigned int texture[15];  // teture array
unsigned int texIndex[26]; // texture index
size_t vertextNum[26];     // vertices array

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
        cout << "Failed to create GLFW window" << endl;
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
        cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    Shader stampShader(getLocalPath("shader/case10-stamp.vs").c_str(), getLocalPath("shader/case10-stamp.fs").c_str(), getLocalPath("shader/case10-stamp.gs").c_str());
    stampShader.use();
    stampShader.setInt("texture0", 0);
    texture[0] = loadTexture(getLocalPath("texture/yechi.png").c_str());
    texture[1] = loadTexture(getLocalPath("texture/ling.png").c_str());
    texture[2] = loadTexture(getLocalPath("texture/mi.png").c_str());
    texture[3] = loadTexture(getLocalPath("texture/duguxinbaishu.png").c_str());
    texture[4] = loadTexture(getLocalPath("texture/xinbaijian.png").c_str());
    texture[5] = loadTexture(getLocalPath("texture/chenxinshangshu.png").c_str());
    texture[6] = loadTexture(getLocalPath("texture/chenxinshangzhang.png").c_str());
    texture[7] = loadTexture(getLocalPath("texture/chenxinshangbiao.png").c_str());
    texture[8] = loadTexture(getLocalPath("texture/xinqishi.png").c_str());
    texture[9] = loadTexture(getLocalPath("texture/chenxinqishi.png").c_str());
    texture[10] = loadTexture(getLocalPath("texture/dasimayin.png").c_str());
    texture[11] = loadTexture(getLocalPath("texture/daduduyin.png").c_str());
    texture[12] = loadTexture(getLocalPath("texture/cishizhiyin.png").c_str());
    texture[13] = loadTexture(getLocalPath("texture/zhuguozhiyin.png").c_str());
    texture[14] = loadTexture(getLocalPath("texture/bk.png").c_str());
    readVertext();
    // read face data
    // top
    readFace(vArr[0], vec2(1.0, 1.0), vArr[1], vec2(0.0, 1.0), vArr[2], vec2(0.0, 0.0)); // 0 1 2	chenxinshangbiao
    readFace(vArr[0], vec2(1.0, 1.0), vArr[2], vec2(0.0, 0.0), vArr[3], vec2(1.0, 0.0)); // 0 2 3
    bindFace(0, 7);
    // up
    readFace(vArr[0], vec2(0.0, 0.0), vArr[4], vec2(0.0, 0.0), vArr[5], vec2(0.0, 0.0)); // 0 4 5 triangle
    bindFace(1, 14);
    readFace(vArr[5], vec2(1.0, 1.0), vArr[6], vec2(0.0, 1.0), vArr[1], vec2(0.0, 0.0)); // 5 6 1	  chenxinshangzhang
    readFace(vArr[5], vec2(1.0, 1.0), vArr[1], vec2(0.0, 0.0), vArr[0], vec2(1.0, 0.0)); // 5 1 0
    bindFace(2, 6);
    readFace(vArr[1], vec2(0.0, 0.0), vArr[6], vec2(0.0, 0.0), vArr[7], vec2(0.0, 0.0)); // 1 6 7 triangle
    bindFace(3, 14);
    readFace(vArr[7], vec2(0.0, 1.0), vArr[8], vec2(0.0, 0.0), vArr[2], vec2(1.0, 0.0)); // 7 8 2
    readFace(vArr[7], vec2(0.0, 1.0), vArr[2], vec2(1.0, 0.0), vArr[1], vec2(1.0, 1.0)); // 7 2 1
    bindFace(4, 14);
    readFace(vArr[2], vec2(0.0, 0.0), vArr[8], vec2(0.0, 0.0), vArr[9], vec2(0.0, 0.0)); // 2 8 9 triangle
    bindFace(5, 14);
    readFace(vArr[9], vec2(0.0, 0.0), vArr[10], vec2(1.0, 0.0), vArr[3], vec2(1.0, 1.0)); // 9 10 3	  chenxinqishi
    readFace(vArr[9], vec2(0.0, 0.0), vArr[3], vec2(1.0, 1.0), vArr[2], vec2(0.0, 1.0));  // 9 3 2
    bindFace(6, 9);
    readFace(vArr[3], vec2(0.0, 0.0), vArr[10], vec2(0.0, 0.0), vArr[11], vec2(0.0, 0.0)); // 3 10 11 triangle
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
    readFace(vArr[12], vec2(0.0, 0.0), vArr[20], vec2(0.0, 0.0), vArr[13], vec2(0.0, 0.0)); // 12 20 13	triangle
    bindFace(17, 14);
    readFace(vArr[20], vec2(1.0, 1.0), vArr[21], vec2(0.0, 1.0), vArr[14], vec2(0.0, 0.0)); // 20 21 14	   zhuguozhiyin
    readFace(vArr[20], vec2(1.0, 1.0), vArr[14], vec2(0.0, 0.0), vArr[13], vec2(1.0, 0.0)); // 20 14 13
    bindFace(18, 13);
    readFace(vArr[14], vec2(0.0, 0.0), vArr[21], vec2(0.0, 0.0), vArr[15], vec2(0.0, 0.0)); // 14 21 15	triangle
    bindFace(19, 14);
    readFace(vArr[21], vec2(0.0, 0.0), vArr[22], vec2(1.0, 0.0), vArr[16], vec2(1.0, 1.0)); // 21 22 16
    readFace(vArr[21], vec2(0.0, 0.0), vArr[16], vec2(1.0, 1.0), vArr[15], vec2(0.0, 1.0)); // 21 16 15
    bindFace(20, 14);
    readFace(vArr[16], vec2(0.0, 0.0), vArr[22], vec2(0.0, 0.0), vArr[17], vec2(0.0, 0.0)); // 16 22 17	triangle
    bindFace(21, 14);
    readFace(vArr[22], vec2(0.0, 0.0), vArr[23], vec2(1.0, 0.0), vArr[18], vec2(1.0, 1.0)); // 22 23 18	   daduduyin
    readFace(vArr[22], vec2(0.0, 0.0), vArr[18], vec2(1.0, 1.0), vArr[17], vec2(0.0, 1.0)); // 22 18 17
    bindFace(22, 11);
    readFace(vArr[18], vec2(0.0, 0.0), vArr[23], vec2(0.0, 0.0), vArr[19], vec2(0.0, 0.0)); // 18 23 19	triangle
    bindFace(23, 14);
    readFace(vArr[23], vec2(0.0, 0.0), vArr[20], vec2(1.0, 0.0), vArr[12], vec2(1.0, 1.0)); // 23 20 12
    readFace(vArr[23], vec2(0.0, 0.0), vArr[12], vec2(1.0, 1.0), vArr[19], vec2(0.0, 1.0)); // 23 12 19
    bindFace(24, 14);
    // bottom
    readFace(vArr[20], vec2(1.0, 0.0), vArr[23], vec2(1.0, 1.0), vArr[22], vec2(0.0, 1.0)); // 20 23 22	   cishizhiyin
    readFace(vArr[22], vec2(0.0, 1.0), vArr[21], vec2(0.0, 0.0), vArr[20], vec2(1.0, 0.0)); // 22 21 20
    bindFace(25, 12);
    vArr.clear();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        mat4 model(1.0f);
        if (ifRotate) {
            if ((Angle += 0.02f) >= 360.0f) {
                Angle = 0.0f;
                dir = !dir;
            }
        }

        stampShader.use();
        if (dir)
            model = rotate(model, float(radians(Angle)), vec3(0.0, 1.0, 0.0));
        else
            model = rotate(model, float(radians(Angle)), vec3(1.0, 0.0, 0.0));
        stampShader.setVec3("viewPos", camera.Position);
        stampShader.setMat4("projection", projection);
        stampShader.setMat4("view", view);
        stampShader.setMat4("model", model);
        for (int i = 0; i < 26; i++) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texIndex[i]);
            glBindVertexArray(stampVAO[i]);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertextNum[i] / 5);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    for (int i = 0; i < 26; i++) {
        glDeleteVertexArrays(1, &stampVAO[i]);
        glDeleteBuffers(1, &stampVBO[i]);
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    static float speed = 1.0f;
    static bool spaceKey = false;
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && spaceKey == false) {
        spaceKey = true;
        ifRotate = !ifRotate;
        Angle = 0.0;
        dir = false;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && spaceKey == true)
        spaceKey = false;
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

void readVertext() {
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
    fArr.push_back(p0.x);
    fArr.push_back(p0.y);
    fArr.push_back(p0.z);
    fArr.push_back(t0.x);
    fArr.push_back(t0.y);

    fArr.push_back(p1.x);
    fArr.push_back(p1.y);
    fArr.push_back(p1.z);
    fArr.push_back(t1.x);
    fArr.push_back(t1.y);

    fArr.push_back(p2.x);
    fArr.push_back(p2.y);
    fArr.push_back(p2.z);
    fArr.push_back(t2.x);
    fArr.push_back(t2.y);
}

void bindFace(int i, int texNum) {
    vertextNum[i] = fArr.size();
    if (vertextNum[i] == 0)
        return;
    texIndex[i] = texture[texNum];
    glGenVertexArrays(1, &stampVAO[i]);
    glGenBuffers(1, &stampVBO[i]);
    glBindVertexArray(stampVAO[i]);
    glBindBuffer(GL_ARRAY_BUFFER, stampVBO[i]);
    glBufferData(GL_ARRAY_BUFFER, 4 * vertextNum[i], &fArr[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
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