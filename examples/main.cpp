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
    s3dl::Subpass subpass({}, {&render}, {});
    s3dl::Dependency dependency(VK_SUBPASS_EXTERNAL, 0);
    s3dl::RenderPass renderPass({&render}, {subpass}, {dependency});

    s3dl::Shader shader("vertex.spv", "fragment.spv");
    s3dl::Pipeline* pipeline = renderPass.getNewPipeline(0, shader, window);

    s3dl::Framebuffer framebuffer(swapchain, renderPass);

    VkClearValue clearValue{};
    clearValue.color = { 0.05f, 0.05f, 0.1f, 1.f };

    while (!window.shouldClose())
    {
        glfwPollEvents();

        window.beginRenderPass(renderPass, framebuffer, {clearValue});
        window.bindPipeline(pipeline);

        window.draw();

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

    system("pause");

    return exit_code;
}
