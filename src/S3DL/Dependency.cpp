#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Dependency::Dependency(unsigned int srcSubpass, unsigned int dstSubpass, unsigned int srcStage, unsigned int dstStage, unsigned int srcMask, unsigned int dstMask) :
        _srcSubpass(srcSubpass),
        _dstSubpass(dstSubpass),
        _srcStage(srcStage),
        _dstStage(dstStage),
        _srcMask(srcMask),
        _dstMask(dstMask)
    {
    }
}
