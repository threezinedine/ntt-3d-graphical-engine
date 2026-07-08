#version 300 es

in vec3 aPos;

uniform vec4 uTransform;

void main()
{
	gl_Position = vec4(aPos, 1.0) - vec4(uTransform.xy, 0.0, 0.0);
}