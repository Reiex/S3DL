#include <S3DL/S3DL.hpp>

int main()
{
    // Context initialization

    s3dl::Instance::create();
    s3dl::RenderWindow window(1000, 800, "Test");
    s3dl::Device device = s3dl::Device::createBestPossible(window);
    window.bindDevice(device);

    // S'occuper de la destruction en regardant la création à partir d'ici (au dessus ça a déjà été fait)
    
    // Render pass creation

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    s3dl::RenderSubpass subpass({}, {colorAttachmentRef}, {});

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    s3dl::RenderPass renderPass;
    renderPass.addAttachment(colorAttachment);
    renderPass.addSubpass(subpass);
    renderPass.addDependency(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    // Shader creation

    s3dl::Shader shader(device, "vertex.spv", "fragment.spv");
    
    // Vertex input description

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    // Pipeline creation

    s3dl::RenderPipeline pipeline;
    pipeline.setRenderPass(renderPass);
    pipeline.setShader(shader);
    pipeline.setVertexInputInfo(vertexInputInfo);
    pipeline.setRasterizerState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE);

    window.bindPipeline(pipeline);
    
    // Main loop

    s3dl::Drawable drawable;
    window.setClearColor({0.05, 0.1, 0.1, 1.0});
    while (!window.shouldClose())
    {
        glfwPollEvents();
        window.draw(drawable);
        window.display();
    }

    vkDeviceWaitIdle(device.getVulkanDevice());

    window.unbindPipeline();
    pipeline.destroy(device);
    window.unbindDevice();
    device.destroy();
    window.destroy();
    s3dl::Instance::destroy();

    return 0;
}