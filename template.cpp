//
// Created by zhsssy on 2021.12.15.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <iostream>
#include <glm.hpp>
#include "CohenSutherlandClip.h"
#include <gtc/matrix_transform.hpp>
#include <vector>

using namespace std;

const unsigned int MAXN = 600 * 800 * 2;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const char *vertexShaderPath = "../../../vs.glsl";
const char *fragmentShaderPath = "../../../fs.glsl";

// 绑定数据
unsigned int VAO[10];
unsigned int VBO[10];
glm::mat4 mMat, vMat, pMat;
// 测试数据
int theSize = 4;
int FNB = 2;
float point[100]{
        -0.3f, 0.3f, 0.0f,
        -0.3f, -0.3f, 0.0f,
        0.3f, -0.3f, 0.0f,
        0.3f, 0.3f, 0.0f
};
float MyColors[] = {
        1.0f, 0.0f, 0.0f,  // top right
        0.0f, 1.0f, 0.0f,  // bottom right
        0.0f, 0.0f, 1.0f,  // bottom left
        0.5f, 0.2f, 0.8f   // top left
};
int MyIndices[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
        0, 2, 3   // second Triangle
};
// 测试数据结束
float ex1_array[100];

float test_point[6], test_point2[6];

// turn vector to array
void cover_vec_to_array();

float transX(double x) { return (float) ((float) (2 * x) - SCR_WIDTH) / (SCR_WIDTH * 1.0f); }

float transY(double y) { return (float) (SCR_HEIGHT - 2 * (float) y) / (SCR_HEIGHT * 1.0f); }

void set_float_array(float *array, float x1, float y1, float x2, float y2) {
    int i = 0;
    array[i++] = x1, array[i++] = y1, array[i++] = 0;
    array[i++] = x2, array[i++] = y2, array[i++] = 0;
}

void ex1_clipping_window(float x1, float y1, float x2, float y2) {
    int i = 0;
    ex1_array[i++] = x1, ex1_array[i++] = y1, ex1_array[i++] = 0.5;
    ex1_array[i++] = x1, ex1_array[i++] = y2, ex1_array[i++] = 0.5;
    ex1_array[i++] = x2, ex1_array[i++] = y2, ex1_array[i++] = 0.5;
    ex1_array[i++] = x2, ex1_array[i++] = y1, ex1_array[i++] = 0.5;
}


bool ex2_cohen_sutherland(double x1, double y1, double x2, double y2) {
    double ansx1, ansy1, ansx2, ansy2;
    if (cohenSutherlandClip(x1, y1, x2, y2, ansx1, ansy1, ansx2, ansy2)) {
        int i = 0;
        test_point[i++] = ansx1, test_point[i++] = ansy1, test_point[i++] = 0;
        test_point[i++] = ansx2, test_point[i++] = ansy2, test_point[i++] = 0;
        return true;
    } else {
        return false;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

struct node {
    double x, y;
} a[2], b[2];
struct Node {
    double xleft, xright, ytop, ybottom;
} rec;

void Liang_barsky() {
    double p[5], q[5], umax = 0, umin = 1;
    memset(p, 0, sizeof(p));
    memset(q, 0, sizeof(q));
    double xleft = min(rec.xleft, rec.xright), xright = max(rec.xleft, rec.xright);
    double ytop = max(rec.ytop, rec.ybottom), ybottom = min(rec.ytop, rec.ybottom);
    p[1] = a[0].x - a[1].x;
    q[1] = a[0].x - xleft;
    p[2] = a[1].x - a[0].x;
    q[2] = xright - a[0].x;
    p[3] = a[0].y - a[1].y;
    q[3] = a[0].y - ybottom;
    p[4] = a[1].y - a[0].y;
    q[4] = ytop - a[0].y;

    for (int i = 1; i <= 4; i++) {
        if (p[i] < 0) umax = max(umax, q[i] / p[i]);
        else if (p[i] > 0) umin = min(umin, q[i] / p[i]);
        else if (p[i] == 0 && q[i] < 0) {
            umax = -1;
            break;
        }
    }
    if (umax <= umin && umax >= 0 && umax <= 1 && umin >= 0 && umin <= 1) {
        b[0].x = a[0].x + umax * (a[1].x - a[0].x);
        b[0].y = a[0].y + umax * (a[1].y - a[0].y);
        b[1].x = a[0].x + umin * (a[1].x - a[0].x);
        b[1].y = a[0].y + umin * (a[1].y - a[0].y);
        float tempvec[] = {transX(b[0].x), transY(b[0].y), 1.0f,
                           transX(b[1].x), transY(b[1].y), 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tempvec), &tempvec);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } else {
        b[0].x = 0;
        b[0].y = 0;
        b[1].x = 0;
        b[1].y = 0;
        float tempvec[] = {0, 0, 1.0f, 0, 0, 1.0f};
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(tempvec), &tempvec);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void bind_data() {
    glGenVertexArrays(10, VAO);
    glGenBuffers(10, VBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ex1_array), ex1_array, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MyIndices), MyIndices, GL_DYNAMIC_DRAW);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MyIndices), MyIndices, GL_DYNAMIC_DRAW);

    // 截取在内部的
    if (ex2(0.3, -0.2, 0.2, 0.6)) {
        for (auto i: test_point)
            cout << i << " ";
        cout << endl;
        // 整条线
        set_float_array(test_point2, 0.3, -0.2, 0.2, 0.6);
        glBindVertexArray(VAO[2]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(test_point2), test_point2, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // 裁剪线条
        glBindVertexArray(VAO[3]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(test_point), test_point, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);


    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display(Shader &shader) {
    glClear(GL_COLOR_BUFFER_BIT);
//    glEnable(GL_SCISSOR_TEST);
    // 主窗口
    shader.use();
    shader.setVec3("setColor", 0.3f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    // 原始线条
    shader.use();
    shader.setVec3("setColor", 0.0f, 1.0f, 0.0f);
    glBindVertexArray(VAO[2]);
    glDrawArrays(GL_LINES, 0, 2);

    // 裁剪线条
    shader.use();
    shader.setVec3("setColor", 0.3f, 0.5f, 0.5f);
    glBindVertexArray(VAO[3]);
    glDrawArrays(GL_LINES, 0, 2);
}

int main() {
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
    ex1_get_point(-0.5, -0.5, 0.5, 0.5);
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

    mMat = glm::mat4(1.0f);
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
