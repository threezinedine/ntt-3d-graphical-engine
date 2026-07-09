#version 330 core
out vec4 FragColor;

uniform vec4	  uColor;
uniform sampler2D uTexture;

in vec2 TexCoord;
in vec4 VertexColor;

void main()
{
	FragColor = VertexColor * uColor * texture(uTexture, TexCoord);
}