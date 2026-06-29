#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertexColor;

void main()
{
	FragColor = VertexColor; // Use the vertex color for the fragment color
}