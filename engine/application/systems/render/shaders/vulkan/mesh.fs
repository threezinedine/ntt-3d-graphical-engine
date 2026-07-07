#version 450

layout(binding = 0) uniform UniformBlock
{
	vec4 uColor;
};

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = fragColor * uColor;
}