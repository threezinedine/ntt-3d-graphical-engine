#version 300 es
precision mediump float;

// Custom output variable replaces gl_FragColor
out vec4 FragColor;

// 'in' receives data from the vertex shader (must match names exactly)
in vec2 TexCoord;
in vec4 VertexColor;

void main()
{
	FragColor = VertexColor;
}