#pragma once

#include <set>
#include <string>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <S3DL/types.hpp>
#include <S3DL/Glsl.hpp>

namespace s3dl
{
    class Window
    {
        public:

            Window(unsigned int width, unsigned int height, const std::string& title);

            ~Window();

        protected:

            GLFWwindow* _window;
            vec2 _size;
    };
}
