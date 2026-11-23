#version 400 core
out vec4 FragColor;
in vec3 fNor;
in vec2 uv;
uniform sampler2D text;
uniform sampler2D textNormal;

void main()
{
    FragColor = vec4(1, 1, 1, 1);

}