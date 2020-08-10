#include <S3DL/S3DL.hpp>


int main()
{
    // Context initialization

    s3dl::Instance::create();
    s3dl::RenderWindow window(1000, 800, "Test");
    s3dl::Device device = s3dl::Device::createBestPossible(window);
    window.bindDevice(device);
    
    // Render pass creation

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    s3dl::RenderSubpass subpass({}, {colorAttachmentRef}, {}, depthAttachmentRef);

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = window.getTextureFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    s3dl::RenderPass renderPass;
    renderPass.addAttachment(colorAttachment);
    renderPass.addAttachment(depthAttachment);
    renderPass.addSubpass(subpass);
    renderPass.addDependency(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    // Shader creation

    s3dl::Shader shader(device, "vertex.spv", "fragment.spv");
    
    // Vertex input description

    VkVertexInputBindingDescription vertexBindingDescription = s3dl::Vertex::getBindingDescription();
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = s3dl::Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
    
    // Pipeline creation

    s3dl::RenderPipeline pipeline;
    pipeline.setRenderPass(renderPass);
    pipeline.setShader(shader);
    pipeline.setVertexInputInfo(vertexInputInfo);
    pipeline.setRasterizerState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE);

    window.bindPipeline(pipeline);
    
    // Mesh creation

    s3dl::Vertex vertices[] = {
        {{-0.5f, -0.5f,  0.f }, {-0.5f, -0.5f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 0.f}},
        {{ 0.5f, -0.5f,  0.f }, { 0.5f, -0.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}},
        {{ 0.f ,  0.5f,  0.f }, { 0.f ,  0.5f}, {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f}},
        {{-0.5f,  0.5f, 0.5f}, {-0.5f,  0.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 1.f}},
        {{ 0.5f,  0.5f, 0.5f}, { 0.5f,  0.5f}, {0.f, 0.f, 1.f}, {1.f, 0.f, 1.f}},
        {{ 0.f , -0.5f, 0.5f}, { 0.f , -0.5f}, {0.f, 0.f, 1.f}, {1.f, 1.f, 1.f}}
    };

    s3dl::Mesh<s3dl::Vertex> mesh(6, vertices);

    // Main loop

    window.setClearColor({0.05, 0.1, 0.1, 1.0});
    while (!window.shouldClose())
    {
        glfwPollEvents();
        window.draw(mesh);
        window.display();
    }

    vkDeviceWaitIdle(device.getVulkanDevice());

    mesh.destroy();
    window.unbindPipeline();
    pipeline.destroy(device);
    window.unbindDevice();
    device.destroy();
    window.destroy();
    s3dl::Instance::destroy();

    return 0;
}