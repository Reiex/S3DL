#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Pipeline
    {
        public:

            Pipeline(const Shader& shader, unsigned int subpass);
            Pipeline(const Pipeline&) = delete;

            Pipeline& operator=(const Pipeline& pipeline) = delete;

            ~Pipeline();

        private:
    };
}
