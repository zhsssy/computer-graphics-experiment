//
// Created by zhsssy on 2021.12.08.
//

#ifndef COMPUTER_GRAPHICS_EXPERIMENT_SHADER_H
#define COMPUTER_GRAPHICS_EXPERIMENT_SHADER_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

class Shader{
public:
    unsigned int ID;//程序ID
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr){
        //顶点着色器，片段着色器，几何着色器
        string vertexCode;//存储着色器代码
        string fragmentCode;
        string geometryCode;
        ifstream vShaderFile;
        ifstream fShaderFile;
        ifstream gShaderFile;
        //保证如果打开失败可以抛出异常
        vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
        gShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
        try{
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();

            if(geometryPath != nullptr){
                gShaderFile.open(geometryPath);
                stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (ifstream::failure& e){
            cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();

        //创建和编译顶点着色器
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);//创建一个顶点着色器
        glShaderSource(vertex, 1, &vShaderCode, nullptr);//将顶点着色器源码附加到着色器对象上
        glCompileShader(vertex);//编译这个着色器对象
        checkCompileErrors(vertex, "VERTEX");//检查错误
        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);//创建一个片段着色器
        glShaderSource(fragment, 1, &fShaderCode, nullptr);//将片段着色器源码附加到着色器对象上
        glCompileShader(fragment);//编译这个着色器对象
        checkCompileErrors(fragment, "FRAGMENT");//检查错误
        unsigned int geometry;
        if(geometryPath != nullptr){//如果有几何着色器，则重复上述步骤
            const char * gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, nullptr);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        ID = glCreateProgram();//创建一个着色器程序对象
        glAttachShader(ID, vertex);//将顶点着色器对象附加到程序对象上
        glAttachShader(ID, fragment);//将片段着色器对象附加到程序对象上
        if(geometryPath != nullptr)
            glAttachShader(ID, geometry);
        glLinkProgram(ID);//链接着色器
        checkCompileErrors(ID, "PROGRAM");//检查链接错误

        glDeleteShader(vertex);//删除之前不需要的着色器对象
        glDeleteShader(fragment);
        if(geometryPath != nullptr)
            glDeleteShader(geometry);

    }
    void use(){//激活着色器对象
        glUseProgram(ID);
    }
    //uniform工具函数
    void setBool(const string &name, bool value) const{//name是一个uniform的值
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const string &name, int value) const{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const string &name, float value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec2(const string &name, const glm::vec2 &value) const{
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const string &name, float x, float y) const{
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void setVec3(const string &name, const glm::vec3 &value) const{
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const string &name, float x, float y, float z) const{
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec4(const string &name, const glm::vec4 &value) const{
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const string &name, float x, float y, float z, float w){
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    void setMat2(const string &name, const glm::mat2 &mat) const{
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const string &name, const glm::mat3 &mat) const{
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const string &name, const glm::mat4 &mat) const{
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    void checkCompileErrors(GLuint shader, string type){//错误检查
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM"){
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success){
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
        else{
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success){
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
    }
};
#endif //COMPUTER_GRAPHICS_EXPERIMENT_SHADER_H
