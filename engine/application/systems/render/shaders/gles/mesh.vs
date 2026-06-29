#version 300 es
// 'in' replaces 'attribute' in GLSL ES 3.00
in vec3 aPos;
in vec2 aTexCoord;
in vec4 aColor;

// 'out' passes data to the fragment shader
out vec2 TexCoord;
out vec4 VertexColor;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	TexCoord	= aTexCoord;
	VertexColor = aColor;
}