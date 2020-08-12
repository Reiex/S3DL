#include <S3DL/S3DL.hpp>


int main()
{
    s3dl::Instance instance;
    s3dl::RenderWindow window({1000, 800}, "S3DL Test");
    s3dl::Device device(window);
    // window.createSwapChain();  // S'occuper de ça en premier !

    /*

    s3dl::Framebuffer contient n framebuffers.
    Si un attachment s3dl::Attachment::Screen est présent, n = 3 (ou le nombre d'images dans la swap chain)
    Sinon n = 1.

    Voir comment garder les spécifités de la swapchain confinées à RenderWindow.
    C'est pour ça qu'il faut bien être au clair dessus et créer la swapchain avant de s'occuper du reste.

    */

    // s3dl::Attachment render = s3dl::Attachment::Screen(window);
    // s3dl::Attachment color(VK_FORMAT_R8G8B8_SRGB, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    // s3dl::Attachment depth(VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    
    // s3dl::Subpass subpassA({}, {&color}, {}, &depth);
    // s3dl::Subpass subpassB({&color}, {&render}, {});
    // s3dl::Dependency dependencyA(VK_SUBPASS_EXTERNAL, 0);
    // s3dl::Dependency dependencyB(0, 1);
    // s3dl::RenderPass renderPass({render, color, depth}, {subpassA, subpassB}, {dependencyA, dependencyB});

    // s3dl::Shader shaderA("vertex.spv", "fragment.spv");
    // s3dl::Shader shaderB("subpass.spv");
    // s3dl::Pipeline* pipelineA = renderPass.createDefaultPipeline(shaderA, 0);
    // s3dl::Pipeline* pipelineB = renderPass.createDefaultPipeline(shaderB, 1);
    // pipelineA->setVertexInput(...);

    // s3dl::Framebuffer framebuffer = s3dl::Framebuffer::createFromRenderPassModel(renderPass);

    // while (false)
    // {
    //     window.bindFramebuffer(framebuffer);
    //     window.bindRenderPass(renderPass);
    //     window.bindPipeline(pipelineA);

    //     window.draw();

    //     window.beginNextSubpass();
    //     window.bindPipeline(pipelineB);

    //     window.draw();

    //     window.display();
    // }

    return 0;
}