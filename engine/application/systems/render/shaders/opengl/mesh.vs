#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;

uniform vec4 uTransform;

out vec2 TexCoord;
out vec4 VertexColor;

void main()
{
	gl_Position = vec4(aPos, 1.0) - vec4(uTransform.xy, 0.0, 0.0);
	TexCoord	= aTexCoord;
	VertexColor = aColor;
}
