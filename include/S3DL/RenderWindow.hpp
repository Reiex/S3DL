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
            RenderWindow(const RenderWindow& window) = delete;

            RenderWindow& operator=(const RenderWindow& window) = delete;

            VkExtent2D getBestSwapExtent() const;
            VkPresentModeKHR getBestSwapPresentMode() const;
            VkSurfaceFormatKHR getBestSwapSurfaceFormat() const;

            ~RenderWindow();

        private:
    };
}
