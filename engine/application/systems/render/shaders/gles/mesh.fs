#version 300 es
precision mediump float;

uniform vec4	  uColor;
uniform sampler2D uTexture;

out vec4 FragColor;

in vec2 TexCoord;
in vec4 VertexColor;

void main()
{
	FragColor = VertexColor * uColor * texture(uTexture, TexCoord);
}