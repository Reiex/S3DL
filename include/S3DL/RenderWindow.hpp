#pragma once

#include <string>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class RenderWindow : public Window, public RenderTarget
    {
        public:

            RenderWindow(const Device& device, unsigned int width, unsigned int height, const std::string& title);

            ~RenderWindow();

        private:
    };
}
