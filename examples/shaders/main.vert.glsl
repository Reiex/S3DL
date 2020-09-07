#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;

layout (set = 2, binding = 1) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) out vec4 vertColor;
layout (location = 1) out vec2 vertTexCoords;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 1.0);
    vertColor = color;
    vertTexCoords = texCoords;
}
