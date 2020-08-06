#pragma once

#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstdint>

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
            void unbindPipeline();

            void setClearColor(vec4 color = {0.0, 0.0, 0.0, 1.0});
        
            void destroy();

            ~RenderWindow();

        private:

            VkSurfaceFormatKHR chooseSwapSurfaceFormat();
            VkPresentModeKHR chooseSwapPresentMode();
            VkExtent2D chooseSwapExtent();

            void createRenderImages();
            unsigned int getNextRenderImage(VkSemaphore& imageAvailableSemaphore);
            void presentRenderImage(VkSemaphore& imageRenderedSemaphore, unsigned int imageIndex);
            void destroyRenderImages();

            void initClearColors();

            void createFramebuffers();
            void destroyFramebuffers();

            VkSwapchainKHR _swapChain;
    };
}
