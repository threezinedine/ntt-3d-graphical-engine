#version 450

layout(binding = 0) uniform UniformBlock
{
	vec4 uTransform;
};

layout(location = 0) in vec3 inPosition;

void main()
{
	gl_Position = vec4(inPosition, 1.0) - vec4(uTransform.xy, 0.0, 0.0);
}
