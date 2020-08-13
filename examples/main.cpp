#include <S3DL/S3DL.hpp>


int main()
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

    s3dl::Attachment render(swapchain, VK_ATTACHMENT_LOAD_OP_CLEAR);
    s3dl::Attachment color(VK_FORMAT_R8G8B8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    s3dl::Attachment depth(VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    
    s3dl::Subpass subpassA({}, {&color}, {}, &depth);
    s3dl::Subpass subpassB({&color}, {&render}, {});
    s3dl::Dependency dependencyA(VK_SUBPASS_EXTERNAL, 0);
    s3dl::Dependency dependencyB(0, 1);
    s3dl::RenderPass renderPass({render, color, depth}, {subpassA, subpassB}, {dependencyA, dependencyB});

    /*

    Pour arranger le problème:
        - ID dans les attachments conservé par copie ? <- De toute façon les attachments sont pas censés être copiés
        - Passage d'un vecteur de pointeur au lieu d'un vecteur d'attachments à la création de RenderPass

    */

    // s3dl::Shader shaderA("vertex.spv", "fragment.spv");
    // s3dl::Shader shaderB("subpass.spv");
    // s3dl::Pipeline* pipelineA = renderPass.createDefaultPipeline(shaderA, 0);
    // s3dl::Pipeline* pipelineB = renderPass.createDefaultPipeline(shaderB, 1);
    // pipelineA->setVertexInput(...);

    // s3dl::Framebuffer framebuffer = s3dl::Framebuffer::createFromRenderPass(window, renderPass);

    // while (!window.shouldClose())
    // {
    //     window.bindRenderPass(renderPass, framebuffer);
    //     window.bindPipeline(pipelineA);

    //     window.draw();

    //     window.beginNextSubpass();
    //     window.bindPipeline(pipelineB);

    //     window.subpassDraw();

    //     window.display();
    // }

    return 0;
}