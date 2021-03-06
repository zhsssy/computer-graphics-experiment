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
const char *vertexShaderPath = "../../../shadersrc/vs.glsl";
const char *fragmentShaderPath = "../../../shadersrc/fs.glsl";

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


float mix(float a, float b, float t) {
    return a * (1.0f - t) + b * t;
}

float BezierQuadratic(float A, float B, float C, float t) {
    float AB = mix(A, B, t);
    float BC = mix(B, C, t);
    return mix(AB, BC, t);
}

float BezierCubic(float A, float B, float C, float D, float t) {
    float ABC = BezierQuadratic(A, B, C, t);
    float BCD = BezierQuadratic(B, C, D, t);
    return mix(ABC, BCD, t);
}

float BezierQuartic(float A, float B, float C, float D, float E, float t) {
    float ABCD = BezierCubic(A, B, C, D, t);
    float BCDE = BezierCubic(B, C, D, E, t);
    return mix(ABCD, BCDE, t);
}

float BezierQuintic(float A, float B, float C, float D, float E, float F, float t) {
    float ABCDE = BezierQuartic(A, B, C, D, E, t);
    float BCDEF = BezierQuartic(B, C, D, E, F, t);
    return mix(ABCDE, BCDEF, t);
}

float BezierSextic(float A, float B, float C, float D, float E, float F, float G, float t) {
    float ABCDEF = BezierQuintic(A, B, C, D, E, F, t);
    float BCDEFG = BezierQuintic(B, C, D, E, F, G, t);
    return mix(ABCDEF, BCDEFG, t);
}


vector<float> bennsteinBezier(float *px, float *py, int n) {
    float u, x, y, b;
    vector<float> ans;
    ans.emplace_back(px[0]), ans.emplace_back(py[0]), ans.emplace_back(0);

    int nfact = fact(n - 1);

    for (u = 0.1; u < 1; u += 0.1) {
        x = 0, y = 0;
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

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(deCastVertices), deCastVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display(Shader &shader) {
    glClear(GL_COLOR_BUFFER_BIT);
//    主窗口
    shader.use();
    shader.setVec3("setColor", 0.3f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_LINE_STRIP, 0, 11);

    shader.use();
    shader.setVec3("setColor", 0.3f, 0.5f, 0.5f);
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_LINE_STRIP, 0, 10);
}

int main() {
    float x[] = {-0.1f, -0.1f, 0.4f, 0.3f};
    float y[] = {-0.2f, 0.4f, -0.3f, 0.2f};

    auto vec = bennsteinBezier(x, y, 4);

    cover_vec_to_array(vec, bernsteinVertices);
    cout << vec.size() << endl;
    for (auto i: vec) {
        cout << i << " ";
    }

    struct SPoint {
        float x;
        float y;
    };

    SPoint controlPoints[7] =
            {
                    {-0.1f, -0.2f},
                    {0.12f, 0.7f},
                    {-0.5f, 0.9f},
                    {-0.2f, 0.8f},
                    {0.4f,  0.2f},
                    {-0.4f, 0.3f},
                    {0.2f,  0.3f},
            };

    const float c_numPoints = 10;
    vector<float> test;
    for (int i = 0; i < c_numPoints; ++i) {
        float t = ((float) i) / (float(c_numPoints - 1));
        SPoint p;
        p.x = BezierSextic(controlPoints[0].x, controlPoints[1].x, controlPoints[2].x, controlPoints[3].x,
                           controlPoints[4].x, controlPoints[5].x, controlPoints[6].x, t);
        p.y = BezierSextic(controlPoints[0].y, controlPoints[1].y, controlPoints[2].y, controlPoints[3].y,
                           controlPoints[4].y, controlPoints[5].y, controlPoints[6].y, t);
        test.emplace_back(p.x), test.emplace_back(p.y), test.emplace_back(0);
    }
    cout << endl << test.size() << endl;
    for (auto i: test) {
        cout << i << " ";
    }
    cover_vec_to_array(test, deCastVertices);

    //实例化glfw函数
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建glfw窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "zhb6109119065", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }
    // 初始化
    Shader shader(vertexShaderPath, fragmentShaderPath);//创建着色器程序
    bind_data();

    //渲染命令
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(5.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // 设置顶点的大小为10像素宽
    glPointSize(10.0f);

    // 变换矩阵
    mMat = glm::mat4(1.0f);
    mMat = glm::scale(mMat, glm::vec3(2.0f, 2.0f, 1.0f));
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
