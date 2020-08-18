#include <S3DL/S3DL.hpp>


int main_example()
{
    // Init context
    s3dl::Instance instance;

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
    s3dl::Attachment color(VK_FORMAT_R8G8B8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    s3dl::Attachment depth(VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    
    s3dl::Subpass subpassA({}, {&color}, {}, &depth);
    s3dl::Subpass subpassB({&color}, {&render}, {});
    s3dl::Dependency dependencyA(VK_SUBPASS_EXTERNAL, 0);
    s3dl::Dependency dependencyB(0, 1);
    s3dl::RenderPass renderPass({&render, &color, &depth}, {subpassA, subpassB}, {dependencyA, dependencyB});

    s3dl::Shader shaderA("vertex.spv", "fragment.spv");
    s3dl::Shader shaderB("subpass.spv");
    s3dl::Pipeline* pipelineA = renderPass.getNewPipeline(0, shaderA, window);
    s3dl::Pipeline* pipelineB = renderPass.getNewPipeline(1, shaderB, window);
    pipelineA->setVertexInput({}, {});

    // s3dl::Framebuffer framebuffer(window, renderPass);

    // while (!window.shouldClose())
    // {
    //     window.beginRenderPass(renderPass, framebuffer);
    //     window.bindPipeline(pipelineA);

    //     window.draw();

    //     window.beginNextSubpass();
    //     window.bindPipeline(pipelineB);

    //     window.subpassDraw();

    //     window.display();
    // }

    return 0;
}


int main()
{
    int exit_code = main_example();

    system("pause");

    return exit_code;
}
