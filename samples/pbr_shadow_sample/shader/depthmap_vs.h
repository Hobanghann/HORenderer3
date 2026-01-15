#pragma once

#include "virtual_gpu/shader_api.h"

namespace ho {
    ALWAYS_INLINE void DEPTHMAP_VS(size_t vertex_index, VirtualGPU::Varying& out) {
        Vector3 a_position = FetchAttribute<Vector3>(0, vertex_index);

        Matrix4x4 u_model = FetchUniform<Matrix4x4>("u_model"_vg);
        Matrix4x4 u_view_projection = FetchUniform<Matrix4x4>("u_view_projection"_vg);

        out.vg_Position = u_view_projection * u_model * a_position.ToHomogeneous();
    }
}  // namespace ho