#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 vertColor;
layout (location = 1) in vec2 vertTexCoords;

layout (set = 1, binding = 0) uniform MATH { float pi; } math;
layout (set = 2, binding = 0) uniform COLOR { vec4 color; } color;
layout (set = 2, binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(texSampler, vertTexCoords);
    if (fragColor.a < 0.5)
        discard;
}
