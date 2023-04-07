#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace std;

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main(int argc, char *argv[]) {
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window
    GLFWwindow *window = glfwCreateWindow(800, 600, "FirstWindow", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);                                    // 创建窗口上下文
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 注册窗口回调函数

    // load opengl func ptr
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // rendering loop
    while (!glfwWindowShouldClose(window)) {
        // handle input
        processInput(window);

        glClearColor(0.2f, 0.5f, 0.3f, 1.0f); // background color
        glClear(GL_COLOR_BUFFER_BIT);         // clear color

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate(); // terminate and release resources
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
