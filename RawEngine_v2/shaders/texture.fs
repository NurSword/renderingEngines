#version 400 core
out vec4 FragColor;
in vec3 fNor;
in vec2 uv;
uniform sampler2D text;
uniform sampler2D textNormal;

void main()
{
    vec4 diffuse = 0.0*vec4(0.1, 0.1, 0.1, 0.0) + texture(text, uv);
    vec4 normalM = texture(textNormal, uv);

    FragColor = vec4(vec3(1 - vec3(diffuse)), 1);



}