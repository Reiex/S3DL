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

            RenderWindow(const uvec2& size, const std::string& title);

            void createSwapChain();
            void createSwapChain(VkExtent2D extent, VkPresentModeKHR presentMode, VkSurfaceFormatKHR surfaceFormat);

            void destroySwapChain();

            ~RenderWindow();

        private:

            VkSurfaceFormatKHR chooseSwapSurfaceFormat();
            VkPresentModeKHR chooseSwapPresentMode();
            VkExtent2D chooseSwapExtent();

            VkSwapchainKHR _swapChain;
            VkSurfaceFormatKHR _swapFormat;
            std::vector<VkImage> _swapImages;
            std::vector<VkImageView> _swapImageViews;
    };
}
