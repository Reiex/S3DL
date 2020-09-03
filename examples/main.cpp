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
    s3dl::Attachment depth(VK_FORMAT_D24_UNORM_S8_UINT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    s3dl::Subpass subpass({}, {&render}, {}, &depth);
    s3dl::Dependency dependency(VK_SUBPASS_EXTERNAL, 0);
    s3dl::RenderPass renderPass({&render, &depth}, {subpass}, {dependency});
    
    // Create and configure pipeline from render pass
    s3dl::Shader shader("vertex.spv", "fragment.spv");
    s3dl::Pipeline* pipeline = renderPass.getNewPipeline(0, shader, window);
    pipeline->setVertexInput({ s3dl::Vertex::getBindingDescription() }, s3dl::Vertex::getAttributeDescriptions());
    pipeline->setDepthTest(true, true);
    
    // Extract, configure and lock pipeline layout
    s3dl::PipelineLayout* layout = pipeline->getPipelineLayout();

    layout->declareGlobalUniform(0, sizeof(float));
    layout->declareDrawablesUniform(0, sizeof(s3dl::vec4));

    layout->lock(swapchain);

    // Create framebuffer
    s3dl::Framebuffer framebuffer(swapchain, renderPass);
    
    std::vector<VkClearValue> clearValues;
    VkClearValue clearValue{};
    clearValue.color = { 0.02f, 0.05f, 0.1f, 1.f };
    clearValues.push_back(clearValue);
    clearValue.depthStencil = { 1.f, 0 };
    clearValues.push_back(clearValue);

    // Create meshes
    s3dl::Mesh<s3dl::Vertex> meshA(
        {
            {{-0.5f, -0.5f, 0.f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 1.f, 1.f}},
            {{0.5f, -0.5f, 0.f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 0.f, 0.f, 1.f}},
            {{0.5f, 0.5f, 0.f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f, 1.f}},
            {{-0.5f, 0.5f, 0.f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f, 1.f}}
        },
        {
            0, 1, 2, 2, 3, 0
        });
    s3dl::Mesh<s3dl::Vertex> meshB(
        {
            {{-0.9f, -0.9f, 0.5f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 0.f, 1.f}},
            {{0.9f, -0.9f, 0.5f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 1.f, 1.f, 1.f}},
            {{0.9f, 0.9f, 0.5f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 0.f, 1.f, 1.f}},
            {{-0.9f, 0.9f, 0.5f}, {0.f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 0.f, 1.f}}
        },
        {
            0, 1, 2, 2, 3, 0
        });

    while (!window.shouldClose())
    {
        glfwPollEvents();

        layout->setGlobalUniform(0, 3.1415926f);

        window.beginRenderPass(renderPass, framebuffer, clearValues);
        window.bindPipeline(pipeline);

        layout->setDrawablesUniform(meshA, 0, s3dl::vec4{1.0, 0.0, 0.0, 1.0});
        window.draw(meshA);
        
        layout->setDrawablesUniform(meshB, 0, s3dl::vec4{0.0, 0.0, 1.0, 1.0});
        window.draw(meshB);

        // window.beginNextSubpass();
        // window.bindPipeline(pipelineB);

        // window.subpassDraw();

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
