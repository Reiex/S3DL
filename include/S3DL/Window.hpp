#pragma once

#include <set>
#include <string>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Window
    {
        public:

            Window(unsigned int width, unsigned int height, const std::string& title);

            ~Window();

        protected:

            Window();

            static std::set<Window*> _WINDOW_LIST;

            GLFWwindow* _window;
            vec2 _size;
    };
}
