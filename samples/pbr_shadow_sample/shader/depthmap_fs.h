#pragma once

#include "virtual_gpu/shader_api.h"

namespace ho {
    ALWAYS_INLINE void DEPTHMAP_FS(const VirtualGPU::Fragment& in, VirtualGPU::FSOutputs& out) {
        // Do nothing
        (void)in;
        (void)out;
    }
}  // namespace ho