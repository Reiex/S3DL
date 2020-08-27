#version 460
#extension GL_ARB_separate_shader_objects : enable

vec3 positions[] = vec3[](
    vec3(0.0, -0.5, 0.0),
    vec3(0.5, 0.5, 0.0),
    vec3(-0.5, 0.5, 0.0)
);

vec4 colors[] = vec4[](
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0)
);

layout (location = 0) out vec4 color;

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    color = colors[gl_VertexIndex];
}
