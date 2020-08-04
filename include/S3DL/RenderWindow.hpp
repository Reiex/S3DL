#pragma once

#include <string>
#include <stdexcept>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderWindow : public Window, public RenderTarget
    {
        public:

            RenderWindow(unsigned int width, unsigned int height, const std::string& title);

            void bindPipeline(RenderPipeline& pipeline);

            ~RenderWindow();

        private:

            VkSurfaceFormatKHR chooseSwapSurfaceFormat();
            VkPresentModeKHR chooseSwapPresentMode();
            VkExtent2D chooseSwapExtent();

            void createRenderImages();
            void destroyRenderImages();

            void createFramebuffers();
            void destroyFramebuffers();

            VkSwapchainKHR _swapChain;
    };
}
