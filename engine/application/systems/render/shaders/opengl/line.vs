#version 330 core

uniform vec4 uTransform;

layout(location = 0) in vec3 aPos;

void main()
{
	gl_Position = vec4(aPos, 1.0) - vec4(uTransform.xy, 0.0, 0.0);
}