#version 330

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec3 iColor;

uniform mat4 mMat;
uniform mat4 vMat;
uniform mat4 pMat;

out vec3 color;

void main()
{
	gl_Position = pMat * vMat * mMat * vec4(iPos, 1.0f);
// 指定颜色
// 灰色
	color = iColor;
}