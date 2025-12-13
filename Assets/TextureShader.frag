#version 330 core

in vec2 v_TexCoord;
out vec4 OutColor;

uniform sampler2D u_Texture;

void main()
{
    OutColor = texture(u_Texture, v_TexCoord);
}
