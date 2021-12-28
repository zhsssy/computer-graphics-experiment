#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

using namespace std;

const unsigned int MAXN = 600 * 800 * 2;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char *vertexShaderPath = "../../../vs.glsl";
const char *fragmentShaderPath = "../../../fs.glsl";

unsigned int VBO, VAO, CVBO;
int theSize = 3;
float point[100]{
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
};
float MyColors[] = {
        1.0f, 0.0f, 0.0f,  // top right
        0.0f, 1.0f, 0.0f,  // bottom right
        0.0f, 0.0f, 1.0f,  // bottom left
        0.5f, 0.2f, 0.8f   // top left
};
glm::mat4 mMat, vMat, pMat;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void bind_data() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * theSize, point, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, CVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * theSize, MyColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main() {
    //实例化glfw函数
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建glfw窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Breseham", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);//将window设置为接下来操作的主窗口
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {//使用glad加载glfw的所有函数指针
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    Shader shader(vertexShaderPath, fragmentShaderPath);//创建着色器程序
    bind_data();

    //渲染命令
    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 设置顶点的大小为10像素宽
    glPointSize(10.0f);

    mMat = glm::mat4(1.0f);
    vMat = glm::mat4(1.0f);
    pMat = glm::mat4(1.0f);

    shader.use();
    shader.setMat4("mMat", mMat);
    shader.setMat4("vMat", vMat);
    shader.setMat4("pMat", pMat);

    while (!glfwWindowShouldClose(window)) {
        //输入
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        //作图
        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP, 0, theSize);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
