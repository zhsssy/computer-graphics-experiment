#version 330
in vec3 color;

void main()
{
	gl_FragColor = vec4(color, 0.5);
}