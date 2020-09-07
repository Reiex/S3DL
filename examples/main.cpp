#include <S3DL/S3DL.hpp>

int main_example()
{
    // Init context
    s3dl::Instance instance;
    instance.setActive();

    // Open window
    s3dl::RenderWindow window({1000, 800}, "S3DL Test");

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
    layoutA->declareGlobalUniform(0, sizeof(float));
    layoutA->declareDrawablesUniform(0, sizeof(s3dl::vec4));
    layoutA->declareDrawablesUniformSampler(1);
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
    s3dl::Mesh<s3dl::Vertex> meshA(
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
    s3dl::Mesh<s3dl::Vertex> meshB(
        {
            {{-0.9f, -0.9f, 0.5f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 0.f, 1.f}},
            {{0.9f, -0.9f, 0.5f}, {1.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 1.f, 1.f, 1.f}},
            {{0.9f, 0.9f, 0.5f}, {1.f, 1.f}, {0.f, 0.f, -1.f}, {1.f, 0.f, 1.f, 1.f}},
            {{-0.9f, 0.9f, 0.5f}, {0.f, 1.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 0.f, 1.f}}
        },
        {
            0, 1, 2, 2, 3, 0
        }
    );
    
    // Load textures
    s3dl::TextureData textureDataA("testA.png");
    s3dl::Texture textureA(textureDataA);
    
    s3dl::TextureData textureDataB("testB.png");
    s3dl::Texture textureB(textureDataB);


    while (!window.shouldClose())
    {
        glfwPollEvents();

        layoutA->setGlobalUniform(0, 3.1415926f);

        window.beginRenderPass(renderPass, framebuffer, clearValues);
        window.bindPipeline(pipelineA);

        layoutA->setDrawablesUniform(meshA, 0, s3dl::vec4{1.0, 0.0, 0.0, 1.0});
        layoutA->setDrawablesUniformSampler(meshA, 1, textureA);
        window.draw(meshA);
        
        layoutA->setDrawablesUniform(meshB, 0, s3dl::vec4{0.0, 0.0, 1.0, 1.0});
        layoutA->setDrawablesUniformSampler(meshB, 1, textureB);
        window.draw(meshB);

        window.beginNextSubpass();
        window.bindPipeline(pipelineB);

        window.draw(meshB);

        window.display();
    }

    swapchain.waitIdle();

    return 0;
}


int main()
{
    int exit_code = main_example();

    // system("pause");

    return exit_code;
}
