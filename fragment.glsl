#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 vertColor;

layout(binding = 1) uniform UBO {vec4 test;} ubo;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = ubo.test;
}
