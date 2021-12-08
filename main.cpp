
#include <fstream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shader.h"
#pragma execution_character_set("utf-8")
// 窗口尺寸宏定义
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
using namespace std;

unsigned int PVBO, CVBO, VAO, EBO;
glm::mat4 mMat, vMat, pMat;

int VNB = 4;
float MyPoints[] = {
        -0.5f, -0.5f, 0.0f,  // bottom left
        0.5f, -0.5f, 0.0f,   // bottom right
        0.5f, 0.5f, 0.0f,   // top right
        -0.5f, 0.5f, 0.0f   // top left
};
float MyColors[] = {
        1.0f, 0.0f, 0.0f,  // top right
        0.0f, 1.0f, 0.0f,  // bottom right
        0.0f, 0.0f, 1.0f,  // bottom left
        0.5f, 0.2f, 0.8f   // top left
};
int FNB = 2;
int MyIndices[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
        0, 2, 3   // second Triangle
};

int test_VNB = 10;
float points[1000] = {};

void resize_window(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
}


void bind_data() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &PVBO); // PVBO 存坐标
    glGenBuffers(1, &CVBO); // CVBO 存颜色
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 将内存中的顶点坐标数据传输到显存的 PVBO 中
    glBindBuffer(GL_ARRAY_BUFFER, PVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * VNB, MyPoints, GL_STATIC_DRAW);

    // 给顶点着色器中location为0的位置绑定数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // 将内存中的顶点颜色数据传输到显存的 CVBO 中
    glBindBuffer(GL_ARRAY_BUFFER, CVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * VNB, MyColors, GL_STATIC_DRAW);

    // 给顶点着色器中location为1的位置绑定数据
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);

//     将内存中的三角形连接数据传输到显存的 EBO 中
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3 * FNB, MyIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 不支持固定管线

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "詹昊彬 6109119065", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return -1;
    }

    // 将窗口映射到OpenGL帧缓存
    glfwMakeContextCurrent(window);

    // 注册窗口大小改变时的回调函数为 resize_window
    glfwSetFramebufferSizeCallback(window, resize_window);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }
    // 相对 shader 路径
    Shader shader("../vs.glsl","../fs.glsl");
    bind_data();

    // 开启深度测试
//    glEnable(GL_DEPTH_TEST);

    // 设置帧缓存的默认颜色（即窗口的背景颜色）
    glClearColor(0.2f, 0.5f, 0.7f, 0.5f);

    // 设置顶点的大小为10像素宽
    glPointSize(10.0f);

    mMat = glm::mat4(1.0f);
    vMat = glm::mat4(1.0f);
    pMat = glm::mat4(1.0f);

    // 模型变换
//    mMat = glm::scale(mMat, glm::vec3(0.5f, 0.5f, 1.0f));
//    mMat = glm::rotate(mMat, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    vMat = glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    float ratio = (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT;
    pMat = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 100.0f);

//    pMat = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.9f, 100.0f);
//    pMat = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
    shader.use();
    shader.setMat4("mMat",mMat);
    shader.setMat4("vMat",vMat);
    shader.setMat4("pMat",pMat);

    // 进入循环
    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        // 将帧缓存颜色重置为默认颜色
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        glBindVertexArray(VAO);
        // 画三角形
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // 画顶点
        glDrawArrays(GL_POINTS, 0, VNB);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
