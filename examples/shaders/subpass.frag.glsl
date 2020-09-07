#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputColor;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput inputDepth;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = subpassLoad(inputColor).rgba;
}
