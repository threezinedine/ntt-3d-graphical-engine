#version 330 core
out vec4 FragColor;

uniform vec4 uColor;

in vec2 TexCoord;
in vec4 VertexColor;

void main()
{
	FragColor = VertexColor * uColor;
}