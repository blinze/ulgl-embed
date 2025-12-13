#version 330 core

in vec3 v_Color;
out vec4 OutColor;

void main()
{
    OutColor = vec4(v_Color, 1.0);
}
