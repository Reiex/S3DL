#pragma once

#include <vulkan/vulkan.h>

#include <S3DL/types.hpp>

namespace s3dl
{
    class Dependency
    {
        public:

            Dependency(unsigned int srcSubpass, unsigned int dstSubpass, unsigned int srcStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, unsigned int dstStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, unsigned int srcMask = 0, unsigned int dstMask = 0);

        private:

            unsigned int _srcSubpass;
            unsigned int _dstSubpass;
            unsigned int _srcStage;
            unsigned int _dstStage;
            unsigned int _srcMask;
            unsigned int _dstMask;
        
        friend RenderPass;
    };
}
