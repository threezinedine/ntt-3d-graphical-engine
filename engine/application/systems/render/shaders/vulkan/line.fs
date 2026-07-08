#version 450

layout(binding = 1) uniform UniformBlock1
{
	vec4 uColor;
};

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = uColor;
}
