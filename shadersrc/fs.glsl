#version 330
uniform vec3 setColor;

void main()
{
    gl_FragColor = vec4(setColor, 0.5);
}