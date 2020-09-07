#include "main.hpp"

s3dl::Mesh<s3dl::Vertex>* load_model()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "examples/objs/viking_room.obj"))
        throw std::runtime_error(warn + err);

    std::vector<s3dl::Vertex> vertices;
    std::vector<uint32_t> indices;
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            s3dl::Vertex vertex{};

            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoords = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};


            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    return new s3dl::Mesh<s3dl::Vertex>(
        {
            {{-0.5f, -0.5f, 0.f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, 0.f}, {1.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 0.f, 0.f, 1.f}},
            {{0.5f, 0.5f, 0.f}, {1.f, 1.f}, {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f, 1.f}},
            {{-0.5f, 0.5f, 0.f}, {0.f, 1.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f, 1.f}}
        },
        {
            0, 1, 2, 2, 3, 0
        }
    );
}

int main_viking_room()
{
    // Init context
    s3dl::Instance instance;
    instance.setActive();

    // Open window
    s3dl::RenderWindow window({1000, 800}, "S3DL Viking Room");

    // Chose graphics hardware and how to use it
    s3dl::Device device(window);
    device.setActive();

    // Chose how to display on screen
    s3dl::Swapchain swapchain(window);
    window.setSwapchain(swapchain);

    // Create render pass
    s3dl::Attachment render(swapchain, VK_ATTACHMENT_LOAD_OP_CLEAR);
    s3dl::Attachment color(VK_FORMAT_R8G8B8A8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    s3dl::Attachment depth(VK_FORMAT_D24_UNORM_S8_UINT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    s3dl::Subpass subpassA({}, {&color}, {}, &depth);
    s3dl::Subpass subpassB({&color, &depth}, {&render}, {});
    s3dl::Dependency dependencyA(VK_SUBPASS_EXTERNAL, 0);
    s3dl::Dependency dependencyB(0, 1);
    s3dl::RenderPass renderPass({&render, &color, &depth}, {subpassA, subpassB}, {dependencyA, dependencyB});
    
    // Create and configure pipeline from render pass
    s3dl::Shader shaderA("examples/spvs/main.vert.spv", "examples/spvs/main.frag.spv");
    s3dl::Pipeline* pipelineA = renderPass.getNewPipeline(0, shaderA, window);
    pipelineA->setVertexInput({ s3dl::Vertex::getBindingDescription() }, s3dl::Vertex::getAttributeDescriptions());
    pipelineA->setDepthTest(true, true);

    s3dl::Shader shaderB("examples/spvs/subpass.vert.spv", "examples/spvs/subpass.frag.spv");
    s3dl::Pipeline* pipelineB = renderPass.getNewPipeline(1, shaderB, window);
    pipelineB->setVertexInput({ s3dl::Vertex::getBindingDescription() }, s3dl::Vertex::getAttributeDescriptions());
    
    // Extract, configure and lock pipeline layout
    s3dl::PipelineLayout* layoutA = pipelineA->getPipelineLayout();
    layoutA->lock(swapchain);
    s3dl::PipelineLayout* layoutB = pipelineB->getPipelineLayout();
    layoutB->lock(swapchain);

    // Create framebuffer
    s3dl::Framebuffer framebuffer(swapchain, renderPass);
    
    std::vector<VkClearValue> clearValues;
    VkClearValue clearValue{};
    clearValue.color = { 0.02f, 0.05f, 0.1f, 1.f };
    clearValues.push_back(clearValue);
    clearValues.push_back(clearValue);
    clearValue.depthStencil = { 1.f, 0 };
    clearValues.push_back(clearValue);

    // Create meshes
    s3dl::Mesh<s3dl::Vertex>* mesh = load_model();
    
    // Load textures
    s3dl::TextureData textureData("examples/images/viking_room.png");
    s3dl::Texture texture(textureData);

    while (!window.shouldClose())
    {
        glfwPollEvents();

        window.beginRenderPass(renderPass, framebuffer, clearValues);
        window.bindPipeline(pipelineA);

        window.draw(*mesh);

        window.beginNextSubpass();
        window.bindPipeline(pipelineB);

        window.draw(*mesh);

        window.display();

        break;
    }

    swapchain.waitIdle();

    return 0;
}
