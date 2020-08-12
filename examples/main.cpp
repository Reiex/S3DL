#include <S3DL/S3DL.hpp>


int main()
{
    s3dl::Instance instance;
    s3dl::RenderWindow window({1000, 800}, "S3DL Test");
    s3dl::Device device(window);

    // s3dl::Attachment render = window.getTargetAttachment();
    // s3dl::Attachment color(format, loadOp, storeOp, initialLayout, finalLayout);
    // s3dl::Attachment depth(format, loadOp, storeOp, initialLayout, finalLayout);
    
    // s3dl::Subpass subpassA({}, {color}, {}, depth);
    // s3dl::Subpass subpassB({color}, {render}, {});
    // s3dl::Dependency dependencyA(subpassA, subpassB, stageA, stageB, accessA, accessB);
    // s3dl::Dependency dependencyB(subpassA, subpassB, stageA, stageB, accessA, accessB);
    // s3dl::RenderPass renderPass({render, color, depth}, {subpassA, subpassB}, {dependencyA, dependencyB});

    // s3dl::Shader shaderA(vertex, fragment);
    // s3dl::Shader shaderB(fragment);
    // s3dl::Pipeline* pipelineA = renderPass.createDefaultPipeline(0, shaderA);
    // s3dl::Pipeline* pipelineB = renderPass.createDefaultPipeline(1, shaderB);
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