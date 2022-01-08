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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char *vertexShaderPath = "../../../vs.glsl";
const char *fragmentShaderPath = "../../../fs.glsl";

unsigned int VBO, VAO, CVBO;
int n = 0;
float points[10000];
glm::mat4 mMat, vMat, pMat;

void bresenham(float x1, float y1, float x2, float y2) {
    float m_new = 2 * (y2 - y1);
    float slope_error_new = m_new - (x2 - x1);
    for (int x = x1, y = y1; x <= x2; x++) {
        points[n++] = x, points[n++] = y, points[n++] = 0;
        slope_error_new += m_new;

        if (slope_error_new >= 0) {
            y++;
            slope_error_new -= 2 * (x2 - x1);
        }
    }
}

void lineDDA(float x1, float y1, float x2, float y2) {
    float x = 0.0f, y = 0.0f, m = 0.0f;
    float dx = x2 - x1, dy = y2 - y1;
    if (dx != 0) {
        m = dy / dx;
        if (m <= 1 && m >= -1) {
            y = y1;
            for (x = x1; x <= x2; x++) {
                points[n++] = x, points[n++] = y, points[n++] = 0;
                y += m;
            }
        }
        if (m > 1 || m < -1) {
            m = 1 / m;
            x = x1;
            for (y = y1; y <= y2; y++) {
                points[n++] = x, points[n++] = y, points[n++] = 0;
                x += m;
            }
        }
    } else {
        float x = x1;
        float y = 0;
        y = (y1 <= y2) ? y1 : y2;
        float d = fabs((double) (y1 - y2));
        while (d >= 0) {
            points[n++] = x, points[n++] = y, points[n++] = 0;
            y++;
            d--;
        }
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void bind_data() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CVBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * n, points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main() {
    //实例化glfw函数
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    lineDDA(-100, -100, 65, 25);
    bresenham(-100, -100, 65, 25);
    for (int i = 0; i < n; i++) {
        cout << points[i] << " ";
    }
    cout << "N is" << n << endl;

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ex2", nullptr, nullptr);
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

    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(5.0f);

    mMat = glm::mat4(1.0f);
    vMat = glm::mat4(1.0f);
    pMat = glm::mat4(1.0f);
    mMat = glm::scale(mMat, glm::vec3(0.007f, 0.007f, 1.0f));

    shader.use();
    shader.setMat4("mMat", mMat);
    shader.setMat4("vMat", vMat);
    shader.setMat4("pMat", pMat);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, n / 3);

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
