#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 vertColor;

layout (set = 0, binding = 0) uniform MATH { float pi; } math;
layout (set = 1, binding = 0) uniform COLOR { vec4 color; } color;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = color.color;
}
