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
#include "SutherlandHodgeman.h"

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
// 测试数据
int theSize = 4;
int FNB = 2;
int MyIndices[] = {
        0, 1, 2,
        0, 2, 3
};
// 测试数据结束
float lineArray[100];

float conhenVertices[6], conhenVertices2[6];
float liangVertices[6], liangVertices2[6];
float suthHodgVertices[1000];

// turn vector to array
/*
 * 转换为 x,y,0 的 array
 */
void cover_vec_to_array(vector<float> &a, float *b) {
    int j = 0;
    for (auto i: a) {
        if ((j + 1) % 3 == 0)
            b[j++] = 0;
        b[j++] = i;
    }
    b[j] = 0;
};

float transX(double x) { return (float) ((float) (2 * x) - SCR_WIDTH) / (SCR_WIDTH * 1.0f); }

float transY(double y) { return (float) (SCR_HEIGHT - 2 * (float) y) / (SCR_HEIGHT * 1.0f); }

void set_float_array(float *array, float x1, float y1, float x2, float y2) {
    int i = 0;
    array[i++] = x1, array[i++] = y1, array[i++] = 0;
    array[i++] = x2, array[i++] = y2, array[i++] = 0;
}

void ex1_clipping_window(float x1, float y1, float x2, float y2) {
    int i = 0;
    lineArray[i++] = x1, lineArray[i++] = y1, lineArray[i++] = 0.5;
    lineArray[i++] = x1, lineArray[i++] = y2, lineArray[i++] = 0.5;
    lineArray[i++] = x2, lineArray[i++] = y2, lineArray[i++] = 0.5;
    lineArray[i++] = x2, lineArray[i++] = y1, lineArray[i++] = 0.5;
}


bool ex2_cohen_sutherland(double x1, double y1, double x2, double y2) {
    double ansx1, ansy1, ansx2, ansy2;
    if (cohenSutherlandClip(x1, y1, x2, y2, ansx1, ansy1, ansx2, ansy2)) {
        set_float_array(conhenVertices2, ansx1, ansy1, ansx2, ansy2);
        return true;
    }
    return false;

}

/*
 * 点坐标
 * float (x1,y1)
 * float (x2,y2)
 * xmin-xmax x范围
 * ymin-ymax y范围
 */
bool ex2_liang_barsky(float x1, float y1, float x2, float y2,
                      float xmin, float xmax, float ymin, float ymax) {
    float
            xx1, yy1, xx2, yy2,
            dx, dy;
    float p[4], q[4];
    float t1, t2, t[4];

    // 原始线
    set_float_array(liangVertices, x1, y1, x2, y2);
    dx = x2 - x1;
    dy = y2 - y1;
    p[0] = -dx, p[1] = dx;
    p[2] = -dy, p[3] = dy;
    q[0] = x1 - xmin, q[1] = xmax - x1;
    q[2] = y1 - ymin, q[3] = ymax - y1;
    t1 = 0;   //max(0,tx)
    t2 = 1;   //min(1,tx)
    for (int i = 0; i < 4; i++) {
        if (p[i] != 0) {
            t[i] = (float) q[i] / p[i];
            if (p[i] < 0) {
                if (t[i] > t1)
                    t1 = t[i];
            } else {
                if (t[i] < t2)
                    t2 = t[i];
            }
        } else {
            if (p[i] == 0 && q[i] < 0)
                return false;
        }
    }

    if (t1 < t2) {
        xx1 = x1 + t1 * dx, yy1 = y1 + t1 * dy;
        xx2 = x1 + t2 * dx, yy2 = y1 + t2 * dy;
        // 结果线
        set_float_array(liangVertices2, xx1, yy1, xx2, yy2);
        return true;
    }
    return false;
}

void ex3_sutherland_hodgeman() {
    // ex3
    // Defining polygon vertices in clockwise order
    int poly_size = 3;
    float poly_points[20][2] = {{0.3,  0.2},
                                {-0.2, 0.6},
                                {0.2,  0.1}};

    int clipper_size = 4;
    float clipper_points[][2] = {{-0.5, -0.5},
                                 {-0.5, 0.5},
                                 {0.5,  0.5},
                                 {0.5,  -0.5}};
    auto vec = suthHodgClip(poly_points, poly_size, clipper_points,
                            clipper_size);
    cover_vec_to_array(vec, suthHodgVertices);
    cout << vec.size() << endl;
}

void bind_data() {
    glGenVertexArrays(10, VAO);
    glGenBuffers(10, VBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineArray), lineArray, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MyIndices), MyIndices, GL_DYNAMIC_DRAW);

    // 截取在内部的
    if (ex2_cohen_sutherland(0.3, -0.2, 0.2, 0.6)) {
        // 整条线
        set_float_array(conhenVertices, 0.3, -0.2, 0.2, 0.6);
        glBindVertexArray(VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(conhenVertices), conhenVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        // 裁剪线条
        glBindVertexArray(VAO[2]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(conhenVertices2), conhenVertices2, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }
    // liang-barsky 算法绘制
    if (ex2_liang_barsky(0.3, -0.4, 0.5, 0.6, -0.5, 0.5, -0.5, 0.5)) {
        // 整条线
        glBindVertexArray(VAO[3]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(liangVertices), liangVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // 裁剪线条
        glBindVertexArray(VAO[4]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(liangVertices2), liangVertices2, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }
    // SUtherlandHodgeman 裁剪图形
    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(suthHodgVertices), suthHodgVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

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
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_LINES, 0, 2);

    // 裁剪线条
    shader.use();
    shader.setVec3("setColor", 0.3f, 0.5f, 0.5f);
    glBindVertexArray(VAO[2]);
    glDrawArrays(GL_LINES, 0, 2);

    shader.use();
    shader.setVec3("setColor", 0.3f, 0.5f, 0.5f);
    glBindVertexArray(VAO[3]);
    glDrawArrays(GL_LINES, 0, 2);

    shader.use();
    shader.setVec3("setColor", 0.2f, 0.8f, 0.2f);
    glBindVertexArray(VAO[4]);
    glDrawArrays(GL_LINES, 0, 2);

    // 原始线条
    shader.use();
    shader.setVec3("setColor", 0.0f, 1.0f, 0.0f);
    glBindVertexArray(VAO[5]);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
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
    ex1_clipping_window(-0.5, -0.5, 0.5, 0.5);

    ex3_sutherland_hodgeman();
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
