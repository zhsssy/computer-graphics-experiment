//
// Created by zhsssy on 2021.12.15.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

using namespace std;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char *vertexShaderPath = "../../../vs.glsl";
const char *fragmentShaderPath = "../../../fs.glsl";

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// 绑定数据
unsigned int VAO[10];
unsigned int VBO[10];
glm::mat4 mMat, vMat, pMat;


float deCastVertices[100];
float bernsteinVertices[2000];

struct Point {
    float x;
    float y;

    Point(float a, float b) : x(a), y(b) {};
};

float transX(double x) { return (float) ((float) (2 * x) - SCR_WIDTH) / (SCR_WIDTH * 1.0f); }

float transY(double y) { return (float) (SCR_HEIGHT - 2 * (float) y) / (SCR_HEIGHT * 1.0f); }

void two_points_set_float_array(float *array, float x1, float y1, float x2, float y2) {
    int i = 0;
    array[i++] = x1, array[i++] = y1, array[i++] = 0;
    array[i++] = x2, array[i++] = y2, array[i++] = 0;
}

Point Approximate(float t, float x1, float y1, float x2, float y2) {
    float x = x1 * (1 - t) + x2 * t;
    float y = y1 * (1 - t) + y2 * t;

    return {x, y};
}

int fact(int n) {
    if (n > 1)
        return n * fact(n - 1);
    else if (n >= 0)
        return 1;
}

vector<float> bezier(float *px, float *py, int n) {
    float u, x, y, b;
    vector<float> ans;
    ans.emplace_back(px[0]), ans.emplace_back(py[0]), ans.emplace_back(0);

    int nfact = fact(n - 1);

    for (u = 0.1; u < 1; u += 0.1) {
        x = 0;
        y = 0;

        // calculate wrt the formula for all n points
        for (int i = 0; i < n; i++) {
            b = (nfact * pow(1 - u, n - 1 - i) * pow(u, i)) / (fact(n - 1 - i) * fact(i));
            x += b * px[i];
            y += b * py[i];
        }
        ans.emplace_back(x), ans.emplace_back(y), ans.emplace_back(0);
    }
    ans.emplace_back(px[n - 1]), ans.emplace_back(py[n - 1]), ans.emplace_back(0);
    return ans;
}

/*
 * 转换为 (x,y,0)顺序排下去的 array
 */
void cover_vec_to_array(vector<float> &a, float *b) {
    int j = 0;
    for (auto i: a)
        b[j++] = i;
};

void bind_data() {
    glGenVertexArrays(10, VAO);
    glGenBuffers(10, VBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bernsteinVertices), bernsteinVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

//    glBindVertexArray(VAO[1]);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(deCastVertices), deCastVertices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
//    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display(Shader &shader) {
    glClear(GL_COLOR_BUFFER_BIT);
    // 主窗口
    shader.use();
    shader.setVec3("setColor", 0.3f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_POINTS, 0, 11);
}

int main() {
    float x[] = {-0.1f, 0.1f, 0.2f, 0.3f};
    float y[] = {-0.2f, 0.4f, -0.3f, 0.2f};

    auto vec = bezier(x, y, 5);

    cover_vec_to_array(vec, bernsteinVertices);
    cout << vec.size() << endl;
    for (auto i: vec) {
        cout << i << " ";
    }
    //实例化glfw函数
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建glfw窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ex3", nullptr, nullptr);
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
    // 初始化
    Shader shader(vertexShaderPath, fragmentShaderPath);//创建着色器程序
    bind_data();

    //渲染命令
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(2.5f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 设置顶点的大小为10像素宽
    glPointSize(10.0f);

    // 变换矩阵
    mMat = glm::mat4(1.0f);
    mMat = glm::scale(mMat, glm::vec3(0.0001f, 0.000001f, 1.0f));
    vMat = glm::mat4(1.0f);
    pMat = glm::mat4(1.0f);

    shader.use();
    shader.setMat4("mMat", mMat);
    shader.setMat4("vMat", vMat);
    shader.setMat4("pMat", pMat);
    // 线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        //输入
        processInput(window);
        display(shader);
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
