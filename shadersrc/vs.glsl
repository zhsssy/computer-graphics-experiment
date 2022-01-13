#version 330

layout(location = 0) in vec3 iPos;

uniform mat4 mMat;
uniform mat4 vMat;
uniform mat4 pMat;

void main()
{
    gl_Position = pMat * vMat * mMat * vec4(iPos, 1.0f);
}