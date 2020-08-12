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

            Window(const uvec2& size, const std::string& title);
            Window(const Window& window) = delete;

            Window& operator=(const Window& window) = delete;

            bool shouldClose() const;
            const uvec2& getWindowSize() const;

            ~Window();

        protected:

            GLFWwindow* _window;
            uvec2 _windowSize;
    };
}
