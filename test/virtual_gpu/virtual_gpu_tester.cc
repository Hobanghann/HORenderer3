#include "virtual_gpu_tester.h"

namespace ho {
    bool VirtualGPUTester::InitFreshGPU() {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        static std::vector<uint8_t> color_buffer;
        constexpr VGsizei kWidth = 128;
        constexpr VGsizei kHeight = 64;

        color_buffer.assign(kWidth * kHeight * 4, 0);

        return vg.Initialize(color_buffer.data(), kWidth, kHeight, VG_RGBA, VG_UNSIGNED_BYTE);
    }

    bool VirtualGPUTester::InitializeSuccess() {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        constexpr VGsizei kWidth = 128;
        constexpr VGsizei kHeight = 64;

        std::vector<uint8_t> color_buffer(kWidth * kHeight * 4);

        // Default framebuffer checks
        if (!vg.Initialize(color_buffer.data(), kWidth, kHeight, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }
        if (vg.bound_draw_frame_buffer_ == nullptr) {
            return false;
        }
        if (vg.bound_read_frame_buffer_ == nullptr) {
            return false;
        }
        if (vg.bound_draw_frame_buffer_->id != 0) {
            return false;
        }
        if (vg.bound_read_frame_buffer_->id != 0) {
            return false;
        }

        const auto& fb = *vg.bound_draw_frame_buffer_;

        const auto& color = fb.color_attachments[0];
        if (color.external_memory != color_buffer.data()) {
            return false;
        }
        if (color.width != kWidth) {
            return false;
        }
        if (color.height != kHeight) {
            return false;
        }
        if (color.format != VG_RGBA) {
            return false;
        }
        if (color.component_type != VG_UNSIGNED_BYTE) {
            return false;
        }

        const auto& depth = fb.depth_stencil_attachment;
        if (depth.memory == nullptr) {
            return false;
        }
        if (depth.width != kWidth) {
            return false;
        }
        if (depth.height != kHeight) {
            return false;
        }
        if (depth.format != VG_DEPTH_STENCIL) {
            return false;
        }
        if (depth.component_type != VG_UNSIGNED_INT) {
            return false;
        }

        if (fb.draw_slot_to_color_attachment[0] != 0) {
            return false;
        }
        for (size_t i = 1; i < fb.draw_slot_to_color_attachment.size(); ++i) {
            if (fb.draw_slot_to_color_attachment[i] != -1) {
                return false;
            }
        }

        // Core state checks (OpenGL 3.3 initial values)
        if (vg.frame_buffer_pool_.size() != 1u) {
            return false;
        }
        if (vg.frame_buffer_pool_.find(0) == vg.frame_buffer_pool_.end()) {
            return false;
        }

        if (!vg.vertex_array_pool_.empty()) {
            return false;
        }
        if (!vg.buffer_pool_.empty()) {
            return false;
        }
        if (!vg.texture_pool_.empty()) {
            return false;
        }
        if (!vg.sampler_pool_.empty()) {
            return false;
        }
        if (!vg.render_buffer_pool_.empty()) {
            return false;
        }
        if (!vg.shader_pool_.empty()) {
            return false;
        }
        if (!vg.program_pool_.empty()) {
            return false;
        }

        if (vg.base_handle_ != 0u) {
            return false;
        }

        if (vg.bound_vertex_array_ != nullptr) {
            return false;
        }
        if (vg.using_program_ != nullptr) {
            return false;
        }
        if (vg.active_texture_unit_ != 0u) {
            return false;
        }
        if (vg.bound_render_buffer_ != nullptr) {
            return false;
        }

        for (auto* buf : vg.bound_buffer_targets_) {
            if (buf != nullptr) {
                return false;
            }
        }

        for (const auto& tu : vg.texture_units_) {
            for (auto* tex : tu.bound_texture_targets) {
                if (tex != nullptr) {
                    return false;
                }
            }
            if (tu.bound_sampler != nullptr) {
                return false;
            }
        }

        for (const auto& proxy : vg.proxy_texture_states_) {
            if (proxy.format != VG_NONE) {
                return false;
            }
            if (proxy.width != 0) {
                return false;
            }
            if (proxy.height != 0) {
                return false;
            }
            if (proxy.depth != 0) {
                return false;
            }
        }

        const auto& s = vg.state_;

        if (s.clear_color != Color128(0.f, 0.f, 0.f, 0.f)) {
            return false;
        }
        if (!math::IsEqualApprox(s.clear_depth, 1.f)) {
            return false;
        }
        if (s.clear_stencil != 0) {
            return false;
        }

        if (s.viewport.x != 0) {
            return false;
        }
        if (s.viewport.y != 0) {
            return false;
        }
        if (s.viewport.width != kWidth) {
            return false;
        }
        if (s.viewport.height != kHeight) {
            return false;
        }

        if (s.scissor_test_enabled) {
            return false;
        }
        if (s.scissor.x != 0) {
            return false;
        }
        if (s.scissor.y != 0) {
            return false;
        }
        if (s.scissor.width != kWidth) {
            return false;
        }
        if (s.scissor.height != kHeight) {
            return false;
        }

        if (s.depth_test_enabled) {
            return false;
        }
        if (!s.depth_write_enabled) {
            return false;
        }
        if (s.min_depth != 0.f) {
            return false;
        }
        if (s.max_depth != 1.f) {
            return false;
        }
        if (s.depth_factor != 0.f) {
            return false;
        }
        if (s.depth_unit != 0.f) {
            return false;
        }
        if (s.depth_func != VG_LESS) {
            return false;
        }

        if (s.stencil_test_enabled) {
            return false;
        }
        if (s.stencil_func[0] != VG_ALWAYS) {
            return false;
        }
        if (s.stencil_func[1] != VG_ALWAYS) {
            return false;
        }
        if (s.stencil_ref[0] != 0) {
            return false;
        }
        if (s.stencil_ref[1] != 0) {
            return false;
        }
        if (s.stencil_func_mask[0] != 0xFFFFFFFF) {
            return false;
        }
        if (s.stencil_func_mask[1] != 0xFFFFFFFF) {
            return false;
        }
        if (s.stencil_write_mask[0] != 0xFFFFFFFF) {
            return false;
        }
        if (s.stencil_write_mask[1] != 0xFFFFFFFF) {
            return false;
        }
        if (s.stencil_sfail_op[0] != VG_KEEP) {
            return false;
        }
        if (s.stencil_sfail_op[1] != VG_KEEP) {
            return false;
        }
        if (s.stencil_dpfail_op[0] != VG_KEEP) {
            return false;
        }
        if (s.stencil_dpfail_op[1] != VG_KEEP) {
            return false;
        }
        if (s.stencil_dppass_op[0] != VG_KEEP) {
            return false;
        }
        if (s.stencil_dppass_op[1] != VG_KEEP) {
            return false;
        }

        if (s.blend_src_rgb_factor != VG_ONE) {
            return false;
        }
        if (s.blend_dst_rgb_factor != VG_ZERO) {
            return false;
        }
        if (s.blend_rgb_equation != VG_FUNC_ADD) {
            return false;
        }
        if (s.blend_src_alpha_factor != VG_ONE) {
            return false;
        };
        if (s.blend_dst_alpha_factor != VG_ZERO) {
            return false;
        }
        if (s.blend_alpha_equation != VG_FUNC_ADD) {
            return false;
        }
        if (s.blend_constant != Color128(0.f, 0.f, 0.f, 0.f)) {
            return false;
        }

        for (const auto& db : s.draw_buffer_states) {
            if (db.blend_enabled) {
                return false;
            }
            if (!db.color_mask[0]) {
                return false;
            }
            if (!db.color_mask[1]) {
                return false;
            }
            if (!db.color_mask[2]) {
                return false;
            }
            if (!db.color_mask[3]) {
                return false;
            }
        }

        if (s.cull_enabled) {
            return false;
        }
        if (s.cull_face != VG_BACK) {
            return false;
        }
        if (s.front_face != VG_CCW) {
            return false;
        }
        if (s.polygon_mode != VG_FILL) {
            return false;
        }

        if (!math::IsEqualApprox(s.line_width, 1.f)) {
            return false;
        }

        if (s.error_state != VG_NO_ERROR) {
            return false;
        }
        return true;
    }

    bool VirtualGPUTester::InitializeFail() {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (!InitFreshGPU()) {
            return false;
        }

        constexpr VGsizei kWidth = 128;
        constexpr VGsizei kHeight = 64;
        std::vector<uint8_t> buffer(kWidth * kHeight * 4);

        // case: color buffer null
        if (vg.Initialize(nullptr, kWidth, kHeight, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: width < 0
        if (vg.Initialize(buffer.data(), -1, kHeight, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: height < 0
        if (vg.Initialize(buffer.data(), kWidth, -1, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: width = 0
        if (vg.Initialize(buffer.data(), 0, kHeight, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: height = 0
        if (vg.Initialize(buffer.data(), kWidth, 0, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: width too large
        if (vg.Initialize(buffer.data(), VG_MAX_ATTACHMENT_WIDTH + 1, kHeight, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: height too large
        if (vg.Initialize(buffer.data(), kWidth, VG_MAX_ATTACHMENT_HEIGHT + 1, VG_RGBA, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: invalid color format
        if (vg.Initialize(buffer.data(), kWidth, kHeight, VG_DEPTH_COMPONENT, VG_FLOAT)) {
            return false;
        }

        if (vg.Initialize(buffer.data(), kWidth, kHeight, VG_DEPTH_STENCIL, VG_UNSIGNED_BYTE)) {
            return false;
        }

        // case: invalid component type
        if (vg.Initialize(buffer.data(), kWidth, kHeight, VG_RGBA, VG_NONE)) {
            return false;
        }

        return true;
    }

    bool VirtualGPUTester::VaryingOut() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        // Set program
        VGuint p = vgCreateProgram();
        VGuint vs = vgCreateShader(VG_VERTEX_SHADER);
        VGuint fs = vgCreateShader(VG_FRAGMENT_SHADER);
        vgAttachShader(p, vs);
        vgAttachShader(p, fs);
        vgLinkProgram(p);
        vgUseProgram(p);

        VirtualGPU::Varying v;

        // Smooth Varying Test (Out)

        // float
        float val_f = 123.456f;
        uint32_t hash_f = 0x1001;
        v.Out(hash_f, val_f);

        // Vector2 (2 floats)
        Vector2 val_v2(10.0f, 20.0f);
        uint32_t hash_v2 = 0x1002;
        v.Out(hash_v2, val_v2);

        // Vector3 (3 floats)
        Vector3 val_v3(30.0f, 40.0f, 50.0f);
        uint32_t hash_v3 = 0x1003;
        v.Out(hash_v3, val_v3);

        // Vector4 (4 floats)
        Vector4 val_v4(60.0f, 70.0f, 80.0f, 90.0f);
        uint32_t hash_v4 = 0x1004;
        v.Out(hash_v4, val_v4);

        if (v.used_smooth_register_size != 10) return false;

        if (!math::IsEqualApprox(v.smooth_register[0], val_f)) return false;

        if (!math::IsEqualApprox(v.smooth_register[1], val_v2.x)) return false;
        if (!math::IsEqualApprox(v.smooth_register[2], val_v2.y)) return false;

        if (!math::IsEqualApprox(v.smooth_register[3], val_v3.x)) return false;
        if (!math::IsEqualApprox(v.smooth_register[4], val_v3.y)) return false;
        if (!math::IsEqualApprox(v.smooth_register[5], val_v3.z)) return false;

        if (!math::IsEqualApprox(v.smooth_register[6], val_v4.x)) return false;
        if (!math::IsEqualApprox(v.smooth_register[7], val_v4.y)) return false;
        if (!math::IsEqualApprox(v.smooth_register[8], val_v4.z)) return false;
        if (!math::IsEqualApprox(v.smooth_register[9], val_v4.w)) return false;

        VirtualGPU::Program* prog = gpu.using_program_;
        if (prog->smooth_varying_count != 4) return false;
        if (prog->smooth_varying_descs[0].size != 1 || prog->smooth_varying_descs[0].register_index != 0) return false;
        if (prog->smooth_varying_descs[1].size != 2 || prog->smooth_varying_descs[1].register_index != 1) return false;
        if (prog->smooth_varying_descs[2].size != 3 || prog->smooth_varying_descs[2].register_index != 3) return false;
        if (prog->smooth_varying_descs[3].size != 4 || prog->smooth_varying_descs[3].register_index != 6) return false;

        // Flat Varying Test

        //  float
        float flat_f = 987.654f;
        uint32_t f_hash_f = 0x2001;
        v.OutFlat(f_hash_f, flat_f);

        //  Vector2 (2 floats)
        Vector2 flat_v2(1.1f, 2.2f);
        uint32_t f_hash_v2 = 0x2002;
        v.OutFlat(f_hash_v2, flat_v2);

        //  Vector3 (3 floats)
        Vector3 flat_v3(3.3f, 4.4f, 5.5f);
        uint32_t f_hash_v3 = 0x2003;
        v.OutFlat(f_hash_v3, flat_v3);

        //  Vector4 (4 floats)
        Vector4 flat_v4(6.6f, 7.7f, 8.8f, 9.9f);
        uint32_t f_hash_v4 = 0x2004;
        v.OutFlat(f_hash_v4, flat_v4);

        if (v.used_flat_register_size != 10) return false;

        if (!math::IsEqualApprox(v.flat_register[0], flat_f)) return false;

        if (!math::IsEqualApprox(v.flat_register[1], flat_v2.x)) return false;
        if (!math::IsEqualApprox(v.flat_register[2], flat_v2.y)) return false;

        if (!math::IsEqualApprox(v.flat_register[3], flat_v3.x)) return false;
        if (!math::IsEqualApprox(v.flat_register[4], flat_v3.y)) return false;
        if (!math::IsEqualApprox(v.flat_register[5], flat_v3.z)) return false;

        if (!math::IsEqualApprox(v.flat_register[6], flat_v4.x)) return false;
        if (!math::IsEqualApprox(v.flat_register[7], flat_v4.y)) return false;
        if (!math::IsEqualApprox(v.flat_register[8], flat_v4.z)) return false;
        if (!math::IsEqualApprox(v.flat_register[9], flat_v4.w)) return false;

        if (prog->flat_varying_count != 4) return false;

        if (prog->flat_varying_descs[0].size != 1 || prog->flat_varying_descs[0].register_index != 0) return false;
        if (prog->flat_varying_descs[1].size != 2 || prog->flat_varying_descs[1].register_index != 1) return false;
        if (prog->flat_varying_descs[2].size != 3 || prog->flat_varying_descs[2].register_index != 3) return false;
        if (prog->flat_varying_descs[3].size != 4 || prog->flat_varying_descs[3].register_index != 6) return false;

        return true;
    }

    bool VirtualGPUTester::FragmentIn() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        // Set program
        VGuint p = vgCreateProgram();
        VGuint vs = vgCreateShader(VG_VERTEX_SHADER);
        VGuint fs = vgCreateShader(VG_FRAGMENT_SHADER);
        vgAttachShader(p, vs);
        vgAttachShader(p, fs);
        vgLinkProgram(p);
        vgUseProgram(p);

        VirtualGPU::Program* prog = gpu.using_program_;

        // Fragmeht setup
        prog->smooth_varying_name_hashes[0] = 0x1001;
        prog->smooth_varying_descs[0].size = 1;
        prog->smooth_varying_descs[0].register_index = 0;

        prog->smooth_varying_name_hashes[1] = 0x1002;
        prog->smooth_varying_descs[1].size = 2;
        prog->smooth_varying_descs[1].register_index = 1;

        prog->smooth_varying_name_hashes[2] = 0x1003;
        prog->smooth_varying_descs[2].size = 3;
        prog->smooth_varying_descs[2].register_index = 3;

        prog->smooth_varying_name_hashes[3] = 0x1004;
        prog->smooth_varying_descs[3].size = 4;
        prog->smooth_varying_descs[3].register_index = 6;

        prog->smooth_varying_count = 4;

        prog->flat_varying_name_hashes[0] = 0x2001;
        prog->flat_varying_descs[0].size = 1;
        prog->flat_varying_descs[0].register_index = 0;

        prog->flat_varying_name_hashes[1] = 0x2002;
        prog->flat_varying_descs[1].size = 2;
        prog->flat_varying_descs[1].register_index = 1;

        prog->flat_varying_count = 2;

        VirtualGPU::Fragment frag;

        frag.smooth_register[0] = 10.0f;

        frag.smooth_register[1] = 20.0f;
        frag.smooth_register[2] = 21.0f;

        frag.smooth_register[3] = 30.0f;
        frag.smooth_register[4] = 31.0f;
        frag.smooth_register[5] = 32.0f;

        frag.smooth_register[6] = 40.0f;
        frag.smooth_register[7] = 41.0f;
        frag.smooth_register[8] = 42.0f;
        frag.smooth_register[9] = 43.0f;

        frag.flat_register[0] = 100.0f;

        frag.flat_register[1] = 200.0f;
        frag.flat_register[2] = 201.0f;

        // In() Check
        float val_f = frag.In<float>(0x1001);
        if (!math::IsEqualApprox(val_f, 10.0f)) return false;

        Vector2 val_v2 = frag.In<Vector2>(0x1002);
        if (!math::IsEqualApprox(val_v2.x, 20.0f)) return false;
        if (!math::IsEqualApprox(val_v2.y, 21.0f)) return false;

        Vector3 val_v3 = frag.In<Vector3>(0x1003);
        if (!math::IsEqualApprox(val_v3.x, 30.0f)) return false;
        if (!math::IsEqualApprox(val_v3.y, 31.0f)) return false;
        if (!math::IsEqualApprox(val_v3.z, 32.0f)) return false;

        Vector4 val_v4 = frag.In<Vector4>(0x1004);
        if (!math::IsEqualApprox(val_v4.x, 40.0f)) return false;
        if (!math::IsEqualApprox(val_v4.y, 41.0f)) return false;
        if (!math::IsEqualApprox(val_v4.z, 42.0f)) return false;
        if (!math::IsEqualApprox(val_v4.w, 43.0f)) return false;

        // InFlat() Check
        float flat_f = frag.InFlat<float>(0x2001);
        if (!math::IsEqualApprox(flat_f, 100.0f)) return false;

        Vector2 flat_v2 = frag.InFlat<Vector2>(0x2002);
        if (!math::IsEqualApprox(flat_v2.x, 200.0f)) return false;
        if (!math::IsEqualApprox(flat_v2.y, 201.0f)) return false;

        return true;
    }

    bool VirtualGPUTester::FSOutputOut() {
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FSOutputs outputs;

        // Single Output Test (Single Slot)
        uint32_t slot0 = 0;
        Color128 color0(0.1f, 0.2f, 0.3f, 1.0f);

        outputs.Out(slot0, color0);

        if (!outputs.written[slot0]) return false;

        if (!outputs.values[slot0].IsEqualApprox(color0)) return false;

        //  Multiple Outputs Test (Random Slots)
        uint32_t slot2 = 2;
        Color128 color2(1.0f, 0.0f, 0.0f, 1.0f);
        outputs.Out(slot2, color2);

        uint32_t slot5 = 5;
        Color128 color5(0.0f, 1.0f, 0.0f, 1.0f);
        outputs.Out(slot5, color5);

        if (!outputs.written[slot2]) return false;
        if (!outputs.written[slot5]) return false;

        if (!outputs.values[slot2].IsEqualApprox(color2)) return false;
        if (!outputs.values[slot5].IsEqualApprox(color5)) return false;

        if (outputs.written[1]) return false;
        if (outputs.written[3]) return false;
        if (outputs.written[4]) return false;

        // Boundary Test (Last Slot)
        uint32_t last_slot = ho::VG_DRAW_BUFFER_SLOT_COUNT - 1;
        Color128 color_last(0.5f, 0.5f, 0.5f, 0.5f);

        outputs.Out(last_slot, color_last);

        if (!outputs.written[last_slot]) return false;
        if (!outputs.values[last_slot].IsEqualApprox(color_last)) return false;

        // Overwrite Test
        Color128 color0_new(0.9f, 0.9f, 0.9f, 1.0f);
        outputs.Out(slot0, color0_new);

        if (!outputs.values[slot0].IsEqualApprox(color0_new)) return false;
        if (outputs.values[slot0].IsEqualApprox(color0)) return false;

        // Reset Test
        outputs.Reset();

        if (outputs.written.any()) return false;

        if (!outputs.values[slot0].IsEqualApprox(Color128())) return false;

        return true;
    }

    bool VirtualGPUTester::GetColorLock() {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (!InitFreshGPU()) {
            return false;
        }

        const VGuint att0 = 0;
        const VGuint att1 = 1;

        const int x0 = 0;
        const int y0 = 0;

        // same coordinate
        auto& l00 = vg.GetColorLock(att0, x0, y0);
        auto& l01 = vg.GetColorLock(att0, x0, y0);
        if (&l00 != &l01) {
            return false;
        }

        // same tile
        auto& l02 = vg.GetColorLock(att0, x0 + TILE_WIDTH - 1, y0 + TILE_HEIGHT - 1);
        if (&l00 != &l02) {
            return false;
        }

        // different tile
        auto& l10 = vg.GetColorLock(att0, x0 + TILE_WIDTH, y0);
        if (&l00 == &l10) {
            return false;
        }

        // different attachment
        auto& l20 = vg.GetColorLock(att1, x0, y0);
        if (&l00 == &l20) {
            return false;
        }

        return true;
    }

    bool VirtualGPUTester::GetDepthLock() {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        const int x0 = 0;
        const int y0 = 0;

        // same coordinate
        auto& l00 = vg.GetDepthLock(x0, y0);
        auto& l01 = vg.GetDepthLock(x0, y0);
        if (&l00 != &l01) {
            return false;
        }

        // same tile
        auto& l02 = vg.GetDepthLock(x0 + TILE_WIDTH - 1, y0 + TILE_HEIGHT - 1);
        if (&l00 != &l02) {
            return false;
        }

        // different tile
        auto& l10 = vg.GetDepthLock(x0 + TILE_WIDTH, y0);
        if (&l00 == &l10) {
            return false;
        }

        return true;
    }

    bool VirtualGPUTester::ClearColorAttachmentFullViewport() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        Color128 clear(1.f, 0.f, 0.f, 1.f);
        gpu.ClearColorAttachment(0, clear);

        const auto& fb = *gpu.bound_draw_frame_buffer_;
        const auto& color = fb.color_attachments[0];

        const uint8_t* pixels = static_cast<const uint8_t*>(color.external_memory);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const int idx = (y * W + x) * 4;

                if (pixels[idx + 0] != 255) return false;  // R
                if (pixels[idx + 1] != 0) return false;    // G
                if (pixels[idx + 2] != 0) return false;    // B
                if (pixels[idx + 3] != 255) return false;  // A
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearColorAttachmentPartialViewport() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        gpu.state_.viewport = {2, 2, 4, 4};

        Color128 clear(0.f, 1.f, 0.f, 1.f);
        gpu.ClearColorAttachment(0, clear);

        const auto& fb = *gpu.bound_draw_frame_buffer_;
        const auto& color = fb.color_attachments[0];

        const uint8_t* pixels = static_cast<const uint8_t*>(color.external_memory);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const int idx = (y * W + x) * 4;

                const bool inside = (x >= 2 && x < 6 && y >= 2 && y < 6);

                const bool is_clear_color = pixels[idx + 0] == 0 &&    // R
                                            pixels[idx + 1] == 255 &&  // G
                                            pixels[idx + 2] == 0 &&    // B
                                            pixels[idx + 3] == 255;    // A

                if (is_clear_color != inside) {
                    return false;
                }
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearColorAttachmentViewportScissorIntersect() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        gpu.state_.viewport = {1, 1, 6, 6};
        gpu.state_.scissor_test_enabled = true;
        gpu.state_.scissor = {3, 3, 4, 4};

        Color128 clear(0.f, 0.f, 1.f, 1.f);
        gpu.ClearColorAttachment(0, clear);

        const auto& fb = *gpu.bound_draw_frame_buffer_;
        const auto& color = fb.color_attachments[0];

        const uint8_t* pixels = static_cast<const uint8_t*>(color.external_memory);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const int idx = (y * W + x) * 4;

                const bool inside = (x >= 3 && x < 7 && y >= 3 && y < 7);

                const bool is_clear_color = pixels[idx + 0] == 0 &&    // R
                                            pixels[idx + 1] == 0 &&    // G
                                            pixels[idx + 2] == 255 &&  // B
                                            pixels[idx + 3] == 255;    // A

                if (is_clear_color != inside) {
                    return false;
                }
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearColorAttachmentColorMask() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        gpu.state_.draw_buffer_states[0].color_mask[0] = true;   // R
        gpu.state_.draw_buffer_states[0].color_mask[1] = false;  // G
        gpu.state_.draw_buffer_states[0].color_mask[2] = true;   // B
        gpu.state_.draw_buffer_states[0].color_mask[3] = false;  // A

        Color128 clear(1.f, 0.5f, 0.25f, 0.f);

        gpu.ClearColorAttachment(0, clear);

        const auto& fb = *gpu.bound_draw_frame_buffer_;
        const auto& color = fb.color_attachments[0];

        const uint8_t* pixels = static_cast<const uint8_t*>(color.external_memory);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const int idx = (y * W + x) * 4;

                // R
                if (pixels[idx + 0] != 255) return false;

                // G
                if (pixels[idx + 1] != 0) return false;

                // B
                if (pixels[idx + 2] != 64) return false;

                // A
                if (pixels[idx + 3] != 0) return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearDepthAttachmentFullViewport() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        auto& fb = *gpu.bound_draw_frame_buffer_;
        auto& depth = fb.depth_stencil_attachment;
        depth.format = VG_DEPTH_COMPONENT;
        depth.component_type = VG_FLOAT;

        gpu.ClearDepthAttachment(0.25f);

        const float* pixels = reinterpret_cast<const float*>(depth.memory->data() + depth.offset);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const int idx = y * W + x;

                if (pixels[idx] != 0.25f) {
                    return false;
                }
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearDepthAttachmentPartialViewport() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        gpu.state_.viewport = {2, 2, 4, 4};

        auto& fb = *gpu.bound_draw_frame_buffer_;
        auto& depth = fb.depth_stencil_attachment;
        depth.format = VG_DEPTH_COMPONENT;
        depth.component_type = VG_FLOAT;

        gpu.ClearDepthAttachment(0.5f);

        const float* pixels = reinterpret_cast<const float*>(depth.memory->data() + depth.offset);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const int idx = y * W + x;

                const bool inside = (x >= 2 && x < 6 && y >= 2 && y < 6);

                const bool is_clear_depth = (pixels[idx] == 0.5f);

                if (is_clear_depth != inside) {
                    return false;
                }
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearDepthAttachmentViewportScissorIntersect() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        gpu.state_.viewport = {1, 1, 6, 6};
        gpu.state_.scissor_test_enabled = true;
        gpu.state_.scissor = {4, 4, 3, 3};

        auto& fb = *gpu.bound_draw_frame_buffer_;
        auto& depth = fb.depth_stencil_attachment;
        depth.format = VG_DEPTH_COMPONENT;
        depth.component_type = VG_FLOAT;

        gpu.ClearDepthAttachment(0.75f);

        const float* pixels = reinterpret_cast<const float*>(depth.memory->data() + depth.offset);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const int idx = y * W + x;

                const bool inside = (x >= 4 && x < 7 && y >= 4 && y < 7);

                const bool is_clear_depth = (pixels[idx] == 0.75f);

                if (is_clear_depth != inside) {
                    return false;
                }
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearDepthStencilAttachmentFullViewport() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        constexpr float kDepth = 0.3f;
        constexpr uint8_t kStencil = 42;

        gpu.ClearDepthStencilAttachment(true, true, kDepth, kStencil);

        const auto& fb = *gpu.bound_draw_frame_buffer_;
        const auto& ds = fb.depth_stencil_attachment;

        const uint8_t* mem = ds.memory->data() + ds.offset;

        float d = math::Clamp(kDepth, 0.f, 1.f);
        uint32_t qd = static_cast<uint32_t>(d * 16777215.0f);

        const uint8_t exp0 = kStencil;
        const uint8_t exp1 = (qd >> 0) & 0xFF;
        const uint8_t exp2 = (qd >> 8) & 0xFF;
        const uint8_t exp3 = (qd >> 16) & 0xFF;

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const uint8_t* p = mem + (y * W + x) * 4;

                if (p[0] != exp0) return false;
                if (p[1] != exp1) return false;
                if (p[2] != exp2) return false;
                if (p[3] != exp3) return false;
            }
        }
        return true;
    }

    bool VirtualGPUTester::ClearDepthStencilAttachmentPartialViewport() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        constexpr float kDepth = 0.6f;
        constexpr uint8_t kStencil = 77;

        gpu.state_.viewport = {2, 2, 4, 4};
        gpu.ClearDepthStencilAttachment(true, true, kDepth, kStencil);

        const auto& fb = *gpu.bound_draw_frame_buffer_;
        const auto& ds = fb.depth_stencil_attachment;

        const uint8_t* mem = ds.memory->data() + ds.offset;

        uint32_t qd = static_cast<uint32_t>(math::Clamp(kDepth, 0.f, 1.f) * 16777215.0f);

        const uint8_t exp0 = kStencil;
        const uint8_t exp1 = (qd >> 0) & 0xFF;
        const uint8_t exp2 = (qd >> 8) & 0xFF;
        const uint8_t exp3 = (qd >> 16) & 0xFF;

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const bool inside = (x >= 2 && x < 6 && y >= 2 && y < 6);

                const uint8_t* p = mem + (y * W + x) * 4;

                const bool is_clear = (p[0] == exp0 && p[1] == exp1 && p[2] == exp2 && p[3] == exp3);

                if (is_clear != inside) return false;
            }
        }
        return true;
    }

    bool VirtualGPUTester::ClearDepthStencilAttachmentViewportScissorIntersect() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        constexpr float kDepth = 0.9f;
        constexpr uint8_t kStencil = 123;

        gpu.state_.viewport = {1, 1, 6, 6};
        gpu.state_.scissor_test_enabled = true;
        gpu.state_.scissor = {3, 3, 2, 2};

        gpu.ClearDepthStencilAttachment(true, true, kDepth, kStencil);

        const auto& fb = *gpu.bound_draw_frame_buffer_;
        const auto& ds = fb.depth_stencil_attachment;

        const uint8_t* mem = ds.memory->data() + ds.offset;

        uint32_t qd = static_cast<uint32_t>(math::Clamp(kDepth, 0.f, 1.f) * 16777215.0f);

        const uint8_t exp0 = kStencil;
        const uint8_t exp1 = (qd >> 0) & 0xFF;
        const uint8_t exp2 = (qd >> 8) & 0xFF;
        const uint8_t exp3 = (qd >> 16) & 0xFF;

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const bool inside = (x >= 3 && x < 5 && y >= 3 && y < 5);

                const uint8_t* p = mem + (y * W + x) * 4;

                const bool is_clear = (p[0] == exp0 && p[1] == exp1 && p[2] == exp2 && p[3] == exp3);

                if (is_clear != inside) return false;
            }
        }
        return true;
    }

    bool VirtualGPUTester::ClearDepthStencilAttachmentDepthOnly() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        constexpr float kOldDepth = 0.8f;
        constexpr uint8_t kOldStencil = 42;
        constexpr float kNewDepth = 0.3f;

        auto& ds = gpu.bound_draw_frame_buffer_->depth_stencil_attachment;
        uint8_t* mem = ds.memory->data();

        uint32_t old_qd = static_cast<uint32_t>(math::Clamp(kOldDepth, 0.f, 1.f) * 16777215.0f);

        for (int i = 0; i < W * H; ++i) {
            uint8_t* p = mem + i * 4;
            p[0] = kOldStencil;
            p[1] = (old_qd >> 0) & 0xFF;
            p[2] = (old_qd >> 8) & 0xFF;
            p[3] = (old_qd >> 16) & 0xFF;
        }

        gpu.ClearDepthStencilAttachment(true, false, kNewDepth, 0);

        uint32_t new_qd = static_cast<uint32_t>(math::Clamp(kNewDepth, 0.f, 1.f) * 16777215.0f);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const uint8_t* p = mem + (y * W + x) * 4;

                if (p[1] != ((new_qd >> 0) & 0xFF)) return false;
                if (p[2] != ((new_qd >> 8) & 0xFF)) return false;
                if (p[3] != ((new_qd >> 16) & 0xFF)) return false;

                if (p[0] != kOldStencil) return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearDepthStencilAttachmentStencilOnly() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        constexpr float kOldDepth = 0.7f;
        constexpr uint8_t kOldStencil = 11;
        constexpr uint8_t kNewStencil = 99;

        auto& ds = gpu.bound_draw_frame_buffer_->depth_stencil_attachment;
        uint8_t* mem = ds.memory->data();

        uint32_t qd = static_cast<uint32_t>(math::Clamp(kOldDepth, 0.f, 1.f) * 16777215.0f);

        for (int i = 0; i < W * H; ++i) {
            uint8_t* p = mem + i * 4;
            p[0] = kOldStencil;
            p[1] = (qd >> 0) & 0xFF;
            p[2] = (qd >> 8) & 0xFF;
            p[3] = (qd >> 16) & 0xFF;
        }

        gpu.ClearDepthStencilAttachment(false, true, 0.f, kNewStencil);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const uint8_t* p = mem + (y * W + x) * 4;

                if (p[1] != ((qd >> 0) & 0xFF)) return false;
                if (p[2] != ((qd >> 8) & 0xFF)) return false;
                if (p[3] != ((qd >> 16) & 0xFF)) return false;

                if (p[0] != kNewStencil) return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClearDepthStencilAttachmentStencilOnlyWithMask() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        constexpr int W = 128;
        constexpr int H = 64;

        constexpr float kDepth = 0.5f;
        constexpr uint8_t kOldStencil = 0b10101010;
        constexpr uint8_t kNewStencil = 0b00001111;
        constexpr uint8_t kMask = 0x0F;

        gpu.state_.stencil_write_mask[0] = kMask;

        auto& ds = gpu.bound_draw_frame_buffer_->depth_stencil_attachment;
        uint8_t* mem = ds.memory->data();

        uint32_t qd = static_cast<uint32_t>(math::Clamp(kDepth, 0.f, 1.f) * 16777215.0f);

        for (int i = 0; i < W * H; ++i) {
            uint8_t* p = mem + i * 4;
            p[0] = kOldStencil;
            p[1] = (qd >> 0) & 0xFF;
            p[2] = (qd >> 8) & 0xFF;
            p[3] = (qd >> 16) & 0xFF;
        }

        gpu.ClearDepthStencilAttachment(false, true, 0.f, kNewStencil);

        const uint8_t expected_stencil = (kOldStencil & ~kMask) | (kNewStencil & kMask);

        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                const uint8_t* p = mem + (y * W + x) * 4;

                if (p[1] != ((qd >> 0) & 0xFF)) return false;
                if (p[2] != ((qd >> 8) & 0xFF)) return false;
                if (p[3] != ((qd >> 16) & 0xFF)) return false;

                if (p[0] != expected_stencil) return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::EvalFrustumPlaneInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 clip(0.f, 0.f, 0.f, 1.f);

        if (gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_LEFT) != 1.f) return false;
        if (gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_RIGHT) != 1.f) return false;
        if (gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_BOTTOM) != 1.f) return false;
        if (gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_TOP) != 1.f) return false;
        if (gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_NEAR) != 1.f) return false;
        if (gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_FAR) != 1.f) return false;
        if (gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_PROJECTION) != 1.f) return false;

        return true;
    }

    bool VirtualGPUTester::EvalFrustumPlaneOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 clip(2.f, -3.f, 5.f, 1.f);

        if (!(gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_RIGHT) < 0.f)) return false;
        if (!(gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_BOTTOM) < 0.f)) return false;
        if (!(gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_FAR) < 0.f)) return false;

        if (!(gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_LEFT) > 0.f)) return false;
        if (!(gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_TOP) > 0.f)) return false;
        if (!(gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_NEAR) > 0.f)) return false;

        return true;
    }

    bool VirtualGPUTester::EvalFrustumPlaneOnBoundary() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 clip(1.f, -1.f, 0.f, 1.f);

        const float right = gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_RIGHT);
        const float bottom = gpu.EvalFrustumPlane(clip, VirtualGPU::VG_PLANE_POS_BOTTOM);

        if (right != 0.f) return false;
        if (bottom != 0.f) return false;

        if (right < 0.f) return false;
        if (bottom < 0.f) return false;

        return true;
    }

    bool VirtualGPUTester::IsInsideInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 clip(0.f, 0.f, 0.f, 1.f);

        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_LEFT)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_RIGHT)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_BOTTOM)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_TOP)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_NEAR)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_FAR)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_PROJECTION)) return false;

        return true;
    }

    bool VirtualGPUTester::IsInsideOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 clip(2.f, -3.f, 5.f, 1.f);

        if (gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_RIGHT)) return false;
        if (gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_BOTTOM)) return false;
        if (gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_FAR)) return false;

        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_LEFT)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_TOP)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_NEAR)) return false;

        return true;
    }

    bool VirtualGPUTester::IsInsideOnBoundary() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 clip(1.f, -1.f, 0.f, 1.f);

        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_RIGHT)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_BOTTOM)) return false;

        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_LEFT)) return false;
        if (!gpu.IsInside(clip, VirtualGPU::VG_PLANE_POS_TOP)) return false;

        return true;
    }

    bool VirtualGPUTester::GetClipBarycentricBothInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 a(0.2f, 0.f, 0.f, 1.f);
        Vector4 b(0.5f, 0.f, 0.f, 1.f);

        Vector2 bc = gpu.GetClipBarycentric(a, b, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (bc.x < 0.f) return false;
        if (bc.y < 0.f) return false;
        if (!math::IsEqualApprox(bc.x + bc.y, 1.f, 1e-6f)) return false;

        return true;
    }

    bool VirtualGPUTester::GetClipBarycentricInsideOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 inside(0.2f, 0.f, 0.f, 1.f);
        Vector4 outside(1.5f, 0.f, 0.f, 1.f);

        Vector2 bc = gpu.GetClipBarycentric(inside, outside, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (!math::IsEqualApprox(bc.x + bc.y, 1.f, 1e-6f)) return false;

        Vector4 p = bc.x * inside + bc.y * outside;
        real eval = gpu.EvalFrustumPlane(p, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (!math::IsEqualApprox(eval, 0.f, 1e-6f)) return false;

        return true;
    }

    bool VirtualGPUTester::GetClipBarycentricInsideOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 inside(0.2f, 0.f, 0.f, 1.f);
        Vector4 on_plane(1.f, 0.f, 0.f, 1.f);

        Vector2 bc = gpu.GetClipBarycentric(inside, on_plane, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (!math::IsEqualApprox(bc.x + bc.y, 1.f, 1e-6f)) return false;

        Vector4 p = bc.x * inside + bc.y * on_plane;
        real eval = gpu.EvalFrustumPlane(p, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (!math::IsEqualApprox(eval, 0.f, 1e-6f)) return false;

        return true;
    }

    bool VirtualGPUTester::GetClipBarycentricBothOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 a(1.0f, 0.f, 0.f, 1.f);
        Vector4 b(1.0f, 1.f, 0.f, 1.f);

        Vector2 bc = gpu.GetClipBarycentric(a, b, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (!std::isnan(bc.x)) return false;
        if (!std::isnan(bc.y)) return false;

        return true;
    }

    bool VirtualGPUTester::GetClipBarycentricBothOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector4 a(1.5f, 0.f, 0.f, 1.f);
        Vector4 b(2.0f, 0.f, 0.f, 1.f);

        Vector2 bc = gpu.GetClipBarycentric(a, b, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (bc.x < 0.f) return false;
        if (bc.y < 0.f) return false;
        if (!math::IsEqualApprox(bc.x + bc.y, 1.f, 1e-6f)) return false;

        return true;
    }

    bool VirtualGPUTester::LerpVaryingBarycentric10() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v1, v2;
        PopulateVarying(v1, 0.0f, 100.0f);
        PopulateVarying(v2, 100.0f, 200.0f);

        Vector2 bc(1.f, 0.f);
        VirtualGPU::Varying out = gpu.LerpVarying(v1, v2, bc);

        if (out.vg_Position != v1.vg_Position) return false;

        if (out.used_smooth_register_size != v1.used_smooth_register_size) return false;
        for (uint32_t i = 0; i < out.used_smooth_register_size; ++i) {
            if (!math::IsEqualApprox(out.smooth_register[i], v1.smooth_register[i])) return false;
        }

        if (out.used_flat_register_size != v1.used_flat_register_size) return false;
        for (uint32_t i = 0; i < out.used_flat_register_size; ++i) {
            if (!math::IsEqualApprox(out.flat_register[i], v2.flat_register[i])) return false;
        }

        return true;
    }

    bool VirtualGPUTester::LerpVaryingBarycentric01() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v1, v2;
        PopulateVarying(v1, 0.0f, 100.0f);
        PopulateVarying(v2, 100.0f, 200.0f);

        Vector2 bc(0.f, 1.f);
        VirtualGPU::Varying out = gpu.LerpVarying(v1, v2, bc);

        if (out.vg_Position != v2.vg_Position) return false;

        for (uint32_t i = 0; i < out.used_smooth_register_size; ++i) {
            if (!math::IsEqualApprox(out.smooth_register[i], v2.smooth_register[i])) return false;
        }

        for (uint32_t i = 0; i < out.used_flat_register_size; ++i) {
            if (!math::IsEqualApprox(out.flat_register[i], v2.flat_register[i])) return false;
        }

        return true;
    }

    bool VirtualGPUTester::LerpVaryingBarycentricHalf() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v1, v2;
        PopulateVarying(v1, 0.0f, 100.0f);
        PopulateVarying(v2, 10.0f, 200.0f);

        Vector2 bc(0.5f, 0.5f);
        VirtualGPU::Varying out = gpu.LerpVarying(v1, v2, bc);

        if (!math::IsEqualApprox(out.vg_Position.x, 5.0f, 1e-5f)) return false;
        if (!math::IsEqualApprox(out.vg_Position.y, 5.0f, 1e-5f)) return false;
        if (!math::IsEqualApprox(out.vg_Position.z, 5.0f, 1e-5f)) return false;
        if (!math::IsEqualApprox(out.vg_Position.w, 1.0f, 1e-5f)) return false;

        for (uint32_t i = 0; i < out.used_smooth_register_size; ++i) {
            float expected = (v1.smooth_register[i] + v2.smooth_register[i]) * 0.5f;
            if (!math::IsEqualApprox(out.smooth_register[i], expected, 1e-5f)) return false;
        }

        for (uint32_t i = 0; i < out.used_flat_register_size; ++i) {
            if (math::IsEqualApprox(out.flat_register[i], v1.flat_register[i], 1e-5f)) return false;
            if (!math::IsEqualApprox(out.flat_register[i], v2.flat_register[i], 1e-5f)) return false;
        }

        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlanePointInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> vs;
        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);
        vs.push_back(v0);

        auto out = gpu.ClipAgainstPlane(vs, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 1u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlanePointOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> vs;
        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.0_r, 0.0_r, 0.0_r, 1.0_r);
        vs.push_back(v0);

        auto out = gpu.ClipAgainstPlane(vs, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 1u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlanePointOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> vs;
        VirtualGPU::Varying v;
        v.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);
        vs.push_back(v);

        auto out = gpu.ClipAgainstPlane(vs, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineInsideInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.2_r, 0._r, 0._r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.8_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 2u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineInsideOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        // ClipAgainstPlane is polygon clipping function, so result is [intersection, v0, intersection]
        if (out.size() != 3u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineOutsideInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 3u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineOutsideOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.2_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.8_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineInsideOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.0_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 2u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineOutsideOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.0_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 3u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineOnPlaneOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.0_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.0_r, 0.3_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 2u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneLineIdentical() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v0);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 2u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleAllInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.2_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.4_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.6_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (out.size() != 3u) return false;

        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleOneOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();

        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);  // outside

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.2_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        std::vector<VirtualGPU::Varying> poly;

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 4u) return false;
        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }

        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleTwoOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);  // outside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.3_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(1.2_r, 0.0_r, 0.0_r, 1.0_r);  // outside

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (out.size() != 3u) return false;

        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleAllOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.2_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.4_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(1.6_r, 0.0_r, 0.0_r, 1.0_r);

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleOneOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.0_r, 0.0_r, 0.0_r, 1.0_r);  // on-plane

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);  // outside

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (out.size() != 4u) return false;

        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleTwoOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.0_r, 0.0_r, 0.0_r, 1.0_r);  // on-plane

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.0_r, 0.5_r, 0.0_r, 1.0_r);  // on-plane

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (out.size() != 3u) return false;

        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleAllOnPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.0_r, 0.0_r, 0.0_r, 1.0_r);  // on-plane

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.0_r, 0.4_r, 0.0_r, 1.0_r);  // on-plane

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(1.0_r, 0.8_r, 0.0_r, 1.0_r);  // on-plane

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 3u) return false;

        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (!math::IsEqualApprox(e, 0.0_r, math::EPSILON_POINT_ON_PLANE)) {
                return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleTwoIdentical() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v;
        v.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v);
        poly.push_back(v);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (out.size() != 4u) return false;

        for (const auto& varying : out) {
            real e = gpu.EvalFrustumPlane(varying.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleAllIdentical() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v;
        v.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v);
        poly.push_back(v);
        poly.push_back(v);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (out.size() != 3u) return false;

        for (const auto& varying : out) {
            real e = gpu.EvalFrustumPlane(varying.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneTriangleCollinear() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.2_r, 0.0_r, 0.0_r, 1.0_r);  // outside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.2_r, 0.0_r, 0.0_r, 1.0_r);  // outside

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(2.0_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);
        if (out.size() != 3u) return false;

        for (const auto& varying : out) {
            real e = gpu.EvalFrustumPlane(varying.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }
        return true;
    }

    bool VirtualGPUTester::ClipAgainstPlaneInterpolation() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) return false;

        VirtualGPU::Varying v0;  // inside
        v0.vg_Position = Vector4(0.5f, 0.0f, 0.0f, 1.0f);
        v0.used_smooth_register_size = 1;
        v0.smooth_register[0] = 0.0f;
        v0.used_flat_register_size = 1;
        v0.flat_register[0] = 10.0f;

        VirtualGPU::Varying v1;  // outside
        v1.vg_Position = Vector4(1.5f, 0.0f, 0.0f, 1.0f);
        v1.used_smooth_register_size = 1;
        v1.smooth_register[0] = 1.0f;
        v1.used_flat_register_size = 1;
        v1.flat_register[0] = 10.0f;

        VirtualGPU::Varying v2;  // inside
        v2.vg_Position = Vector4(0.2f, 0.0f, 0.0f, 1.0f);
        v2.used_smooth_register_size = 1;
        v2.smooth_register[0] = 0.0f;
        v2.used_flat_register_size = 1;
        v2.flat_register[0] = 10.0f;

        std::vector<VirtualGPU::Varying> poly;
        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.ClipAgainstPlane(poly, VirtualGPU::VG_PLANE_POS_RIGHT);

        if (out.size() != 4u) return false;

        for (const auto& v : out) {
            real e = gpu.EvalFrustumPlane(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT);
            if (e < -math::EPSILON_POINT_ON_PLANE) return false;
        }

        bool target_intersection_found = false;

        for (const auto& v : out) {
            if (math::IsEqualApprox(v.vg_Position.x, 1.0f, 1e-4f)) {
                if (math::IsEqualApprox(v.smooth_register[0], 0.5f, 1e-4f)) {
                    if (!math::IsEqualApprox(v.flat_register[0], 10.0f)) return false;

                    target_intersection_found = true;
                }
            }
        }

        if (!target_intersection_found) return false;

        return true;

        return true;
    }

    bool VirtualGPUTester::ClipAllInside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.0_r, 0.5_r, 0.0_r, 1.0_r);  // intside

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.Clip(poly);

        if (out.size() != 3u) return false;

        for (const auto& v : out) {
            for (VirtualGPU::PlanePos p :
                 {VirtualGPU::VG_PLANE_POS_LEFT, VirtualGPU::VG_PLANE_POS_RIGHT, VirtualGPU::VG_PLANE_POS_BOTTOM,
                  VirtualGPU::VG_PLANE_POS_TOP, VirtualGPU::VG_PLANE_POS_NEAR, VirtualGPU::VG_PLANE_POS_FAR}) {
                if (!gpu.IsInside(v.vg_Position, p)) return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClipOnePlaneClipped() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(1.5_r, 0.0_r, 0.0_r, 1.0_r);  // outside

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.0_r, 0.5_r, 0.0_r, 1.0_r);  // inside

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.Clip(poly);

        if (out.empty()) return false;

        for (const auto& v : out) {
            for (VirtualGPU::PlanePos p :
                 {VirtualGPU::VG_PLANE_POS_LEFT, VirtualGPU::VG_PLANE_POS_RIGHT, VirtualGPU::VG_PLANE_POS_BOTTOM,
                  VirtualGPU::VG_PLANE_POS_TOP, VirtualGPU::VG_PLANE_POS_NEAR, VirtualGPU::VG_PLANE_POS_FAR}) {
                if (!gpu.IsInside(v.vg_Position, p)) return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClipTwoPlanesClipped() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(1.5_r, 1.5_r, 0.0_r, 1.0_r);  // outside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.0_r, 0.5_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.Clip(poly);

        if (out.empty()) return false;

        for (const auto& v : out) {
            if (!gpu.IsInside(v.vg_Position, VirtualGPU::VG_PLANE_POS_RIGHT)) return false;
            if (!gpu.IsInside(v.vg_Position, VirtualGPU::VG_PLANE_POS_TOP)) return false;
        }

        return true;
    }

    bool VirtualGPUTester::ClipAllOutside() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(2.0_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(2.5_r, 0.0_r, 0.0_r, 1.0_r);

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(3.0_r, 0.0_r, 0.0_r, 1.0_r);

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.Clip(poly);

        return out.empty();
    }

    bool VirtualGPUTester::ClipWNegative() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);  // inside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.5_r, 0.0_r, 0.0_r, -1.0_r);  // w < 0 (behind camera)

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.0_r, 0.5_r, 0.0_r, 1.0_r);  // inside

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.Clip(poly);

        for (const auto& v : out) {
            for (VirtualGPU::PlanePos p :
                 {VirtualGPU::VG_PLANE_POS_LEFT, VirtualGPU::VG_PLANE_POS_RIGHT, VirtualGPU::VG_PLANE_POS_BOTTOM,
                  VirtualGPU::VG_PLANE_POS_TOP, VirtualGPU::VG_PLANE_POS_NEAR, VirtualGPU::VG_PLANE_POS_FAR,
                  VirtualGPU::VG_PLANE_POS_PROJECTION}) {
                if (!gpu.IsInside(v.vg_Position, p)) {
                    return false;
                }
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClipNearPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.0_r, 0.0_r, -0.5_r, 1.0_r);  // inside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.2_r, 0.0_r, -2.0_r, 1.0_r);  // outside near

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.0_r, 0.2_r, -0.5_r, 1.0_r);  // inside

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.Clip(poly);

        if (out.empty()) return false;

        for (const auto& v : out) {
            if (!gpu.IsInside(v.vg_Position, VirtualGPU::VG_PLANE_POS_NEAR)) {
                return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::ClipFarPlane() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        std::vector<VirtualGPU::Varying> poly;

        VirtualGPU::Varying v0;
        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.5_r, 1.0_r);  // inside

        VirtualGPU::Varying v1;
        v1.vg_Position = Vector4(0.2_r, 0.0_r, 2.0_r, 1.0_r);  // outside far

        VirtualGPU::Varying v2;
        v2.vg_Position = Vector4(0.0_r, 0.2_r, 0.5_r, 1.0_r);  // inside

        poly.push_back(v0);
        poly.push_back(v1);
        poly.push_back(v2);

        auto out = gpu.Clip(poly);

        if (out.empty()) return false;

        for (const auto& v : out) {
            if (!gpu.IsInside(v.vg_Position, VirtualGPU::VG_PLANE_POS_FAR)) {
                return false;
            }
        }

        return true;
    }

    bool VirtualGPUTester::PerspectiveDivideUnitW() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v;
        v.vg_Position = Vector4(0.5_r, -0.5_r, 0.25_r, 1.0_r);

        gpu.PerspectiveDivide(v);

        if (v.viewport_coord.x != 0.5_r) return false;
        if (v.viewport_coord.y != -0.5_r) return false;
        if (v.viewport_coord.z != 0.25_r) return false;

        return true;
    }

    bool VirtualGPUTester::PerspectiveDivideNonUnitW() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v;
        v.vg_Position = Vector4(2.0_r, -4.0_r, 1.0_r, 2.0_r);

        gpu.PerspectiveDivide(v);

        if (v.viewport_coord.x != 1.0_r) return false;
        if (v.viewport_coord.y != -2.0_r) return false;
        if (v.viewport_coord.z != 0.5_r) return false;

        return true;
    }

    bool VirtualGPUTester::PerspectiveDivideZeroW() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v;
        v.vg_Position = Vector4(1.0_r, -2.0_r, 3.0_r, 0.0_r);

        gpu.PerspectiveDivide(v);

        if (!std::isinf(v.viewport_coord.x)) return false;
        if (!std::isinf(v.viewport_coord.y)) return false;
        if (!std::isinf(v.viewport_coord.z)) return false;

        return true;
    }

    bool VirtualGPUTester::ViewportTransformCenter() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.viewport = {0, 0, 800, 600};
        gpu.state_.min_depth = 0.0_r;
        gpu.state_.max_depth = 1.0_r;

        VirtualGPU::Varying v;
        v.viewport_coord = Vector3(0.0_r, 0.0_r, 0.0_r);

        gpu.ViewportTransform(v);

        if (v.viewport_coord.x != 400.0_r) return false;
        if (v.viewport_coord.y != 300.0_r) return false;
        if (v.viewport_coord.z != 0.5_r) return false;

        return true;
    }

    bool VirtualGPUTester::ViewportTransformBoundary() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.viewport = {0, 0, 800, 600};
        gpu.state_.min_depth = 0.0_r;
        gpu.state_.max_depth = 1.0_r;

        VirtualGPU::Varying v;
        v.viewport_coord = Vector3(-1.0_r, 1.0_r, -1.0_r);

        gpu.ViewportTransform(v);

        if (v.viewport_coord.x != 0.0_r) return false;
        if (v.viewport_coord.y != 0.0_r) return false;
        if (v.viewport_coord.z != 0.0_r) return false;

        return true;
    }

    bool VirtualGPUTester::ViewportTransformWithOffset() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.viewport = {100, 50, 400, 200};
        gpu.state_.min_depth = -1.0_r;
        gpu.state_.max_depth = 1.0_r;

        VirtualGPU::Varying v;
        v.viewport_coord = Vector3(0.0_r, 0.0_r, 0.0_r);

        gpu.ViewportTransform(v);

        const real expect_x = 100.0_r + 200.0_r;
        const real expect_y = 50.0_r + 100.0_r;
        const real expect_z = 0.0_r;

        if (v.viewport_coord.x != expect_x) return false;
        if (v.viewport_coord.y != expect_y) return false;
        if (v.viewport_coord.z != expect_z) return false;

        return true;
    }

    bool VirtualGPUTester::ScissorTestDisabled() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.scissor_test_enabled = false;
        gpu.state_.viewport = {0, 0, 8, 8};

        if (!gpu.ScissorTest(0.0_r, 0.0_r)) return false;
        if (!gpu.ScissorTest(100.0_r, 100.0_r)) return false;
        if (!gpu.ScissorTest(-10.0_r, -5.0_r)) return false;

        return true;
    }

    bool VirtualGPUTester::ScissorTestOutsideViewport() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.scissor_test_enabled = true;
        gpu.state_.viewport = {0, 0, 8, 8};
        gpu.state_.scissor = {2, 2, 4, 4};

        if (gpu.ScissorTest(-0.1_r, 4.0_r)) return false;  // left
        if (gpu.ScissorTest(8.0_r, 4.0_r)) return false;   // right
        if (gpu.ScissorTest(4.0_r, -1.0_r)) return false;  // bottom
        if (gpu.ScissorTest(4.0_r, 8.0_r)) return false;   // top

        return true;
    }

    bool VirtualGPUTester::ScissorTestInsideScissor() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.scissor_test_enabled = true;
        gpu.state_.viewport = {0, 0, 8, 8};
        gpu.state_.scissor = {2, 2, 4, 4};

        if (!gpu.ScissorTest(2.0_r, 2.0_r)) return false;
        if (!gpu.ScissorTest(3.5_r, 4.5_r)) return false;
        if (!gpu.ScissorTest(5.9_r, 5.9_r)) return false;

        return true;
    }

    bool VirtualGPUTester::ScissorTestInsideViewportOutsideScissor() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.scissor_test_enabled = true;
        gpu.state_.viewport = {0, 0, 8, 8};
        gpu.state_.scissor = {2, 2, 4, 4};

        if (gpu.ScissorTest(1.9_r, 3.0_r)) return false;  // left
        if (gpu.ScissorTest(6.0_r, 3.0_r)) return false;  // right
        if (gpu.ScissorTest(3.0_r, 1.9_r)) return false;  // bottom
        if (gpu.ScissorTest(3.0_r, 6.0_r)) return false;  // top

        return true;
    }

    bool VirtualGPUTester::ScissorTestBoundary() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.scissor_test_enabled = true;
        gpu.state_.viewport = {0, 0, 8, 8};
        gpu.state_.scissor = {2, 2, 4, 4};

        if (!gpu.ScissorTest(2.0_r, 2.0_r)) return false;

        if (gpu.ScissorTest(6.0_r, 5.9_r)) return false;
        if (gpu.ScissorTest(5.9_r, 6.0_r)) return false;

        if (!gpu.ScissorTest(2.999_r, 2.001_r)) return false;  // floor -> (2,2)
        if (gpu.ScissorTest(1.999_r, 2.5_r)) return false;     // floor -> (1,2)

        return true;
    }

    bool VirtualGPUTester::ComputeDepthSlopeFlatTriangle() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector3 v0(0.0_r, 0.0_r, 1.0_r);
        Vector3 v1(1.0_r, 0.0_r, 1.0_r);
        Vector3 v2(0.0_r, 1.0_r, 1.0_r);

        real slope = gpu.ComputeDepthSlope(v0, v1, v2);

        return slope == 0.0_r;
    }

    bool VirtualGPUTester::ComputeDepthSlopeLinearInX() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector3 v0(0.0_r, 0.0_r, 0.0_r);
        Vector3 v1(1.0_r, 0.0_r, 1.0_r);
        Vector3 v2(0.0_r, 1.0_r, 0.0_r);

        real slope = gpu.ComputeDepthSlope(v0, v1, v2);

        return slope == 1.0_r;
    }

    bool VirtualGPUTester::ComputeDepthSlopeLinearInY() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector3 v0(0.0_r, 0.0_r, 0.0_r);
        Vector3 v1(1.0_r, 0.0_r, 0.0_r);
        Vector3 v2(0.0_r, 1.0_r, 1.0_r);

        real slope = gpu.ComputeDepthSlope(v0, v1, v2);

        return slope == 1.0_r;
    }

    bool VirtualGPUTester::ComputeDepthSlopeLinearXY() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector3 v0(0.0_r, 0.0_r, 0.0_r);
        Vector3 v1(1.0_r, 0.0_r, 2.0_r);
        Vector3 v2(0.0_r, 1.0_r, 0.5_r);

        real slope = gpu.ComputeDepthSlope(v0, v1, v2);

        return slope == 2.0_r;
    }

    bool VirtualGPUTester::ComputeDepthSlopeNegativeGradient() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector3 v0(0.0_r, 0.0_r, 0.0_r);
        Vector3 v1(1.0_r, 0.0_r, -3.0_r);
        Vector3 v2(0.0_r, 1.0_r, 1.0_r);

        real slope = gpu.ComputeDepthSlope(v0, v1, v2);

        return slope == 3.0_r;
    }

    bool VirtualGPUTester::ComputeDepthSlopeDegenerateTriangle() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Vector3 v0(0.0_r, 0.0_r, 0.0_r);
        Vector3 v1(1.0_r, 1.0_r, 1.0_r);
        Vector3 v2(2.0_r, 2.0_r, 2.0_r);

        real slope = gpu.ComputeDepthSlope(v0, v1, v2);

        return slope == 0.0_r;
    }

    bool VirtualGPUTester::ApplyDepthOffsetDisabled() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.polygon_offset_enabled = false;
        gpu.state_.depth_factor = 10.0_r;
        gpu.state_.depth_unit = 10.0_r;

        VGfloat depth = 0.5_r;
        VGfloat out = gpu.ApplyDepthOffset(depth, 1.0_r, 24, VG_FILL);

        return out == depth;
    }

    bool VirtualGPUTester::ApplyDepthOffsetSlopeOnly() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.polygon_offset_enabled = true;
        gpu.state_.depth_factor = 2.0_r;
        gpu.state_.depth_unit = 0.0_r;

        VGfloat depth = 0.5_r;
        VGfloat slope = 0.25_r;

        VGfloat out = gpu.ApplyDepthOffset(depth, slope, 24, VG_FILL);

        return out == (0.5_r + 0.5_r);
    }

    bool VirtualGPUTester::ApplyDepthOffsetUnitOnly24Bit() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.polygon_offset_enabled = true;
        gpu.state_.depth_factor = 0.0_r;
        gpu.state_.depth_unit = 1.0_r;

        VGfloat depth = 0.5_r;
        VGfloat out = gpu.ApplyDepthOffset(depth, 0.0_r, 24, VG_FILL);

        VGfloat r = 1.0_r / (1 << 24);
        return out == (depth + r);
    }

    bool VirtualGPUTester::ApplyDepthOffsetUnitOnly32Bit() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.polygon_offset_enabled = true;
        gpu.state_.depth_factor = 0.0_r;
        gpu.state_.depth_unit = 1.0_r;

        VGfloat depth = 0.5_r;
        VGfloat out = gpu.ApplyDepthOffset(depth, 0.0_r, 32, VG_FILL);

        // 32bit depth unit offset is below float resolution at depth=0.5
        // Expected result is unchanged.
        return out == depth;
    }

    bool VirtualGPUTester::ApplyDepthOffsetSlopeAndUnit() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.polygon_offset_enabled = true;
        gpu.state_.depth_factor = 1.5_r;
        gpu.state_.depth_unit = 2.0_r;

        VGfloat depth = 0.25_r;
        VGfloat slope = 0.2_r;

        VGfloat r = 1.0_r / (1 << 24);
        VGfloat expected = depth + (0.3_r + 2.0_r * r);

        VGfloat out = gpu.ApplyDepthOffset(depth, slope, 24, VG_FILL);

        return out == expected;
    }

    bool VirtualGPUTester::ApplyDepthOffsetClampUpper() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.polygon_offset_enabled = true;
        gpu.state_.depth_factor = 10.0_r;
        gpu.state_.depth_unit = 0.0_r;

        VGfloat out = gpu.ApplyDepthOffset(0.9_r, 1.0_r, 24, VG_FILL);

        return out == 1.0_r;
    }

    bool VirtualGPUTester::ApplyDepthOffsetClampLower() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.polygon_offset_enabled = true;
        gpu.state_.depth_factor = -10.0_r;
        gpu.state_.depth_unit = 0.0_r;

        VGfloat out = gpu.ApplyDepthOffset(0.1_r, 1.0_r, 24, VG_FILL);

        return out == 0.0_r;
    }

    bool VirtualGPUTester::ApplyDepthOffsetAllPrimitiveModes() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        const VGfloat base_depth = 0.5_r;
        const VGfloat slope = 1.0_r;
        const uint32_t depth_bits = 24;

        gpu.state_.polygon_offset_enabled = true;
        gpu.state_.line_offset_enabled = true;
        gpu.state_.point_offset_enabled = true;
        gpu.state_.depth_factor = 1.0_r;
        gpu.state_.depth_unit = 0.0_r;

        if (!(gpu.ApplyDepthOffset(base_depth, slope, depth_bits, VG_FILL) > base_depth)) return false;
        if (!(gpu.ApplyDepthOffset(base_depth, slope, depth_bits, VG_LINE) > base_depth)) return false;
        if (!(gpu.ApplyDepthOffset(base_depth, slope, depth_bits, VG_POINT) > base_depth)) return false;

        gpu.state_.polygon_offset_enabled = false;
        if (gpu.ApplyDepthOffset(base_depth, slope, depth_bits, VG_FILL) != base_depth) return false;

        gpu.state_.line_offset_enabled = false;
        if (gpu.ApplyDepthOffset(base_depth, slope, depth_bits, VG_LINE) != base_depth) return false;

        gpu.state_.point_offset_enabled = false;
        if (gpu.ApplyDepthOffset(base_depth, slope, depth_bits, VG_POINT) != base_depth) return false;

        return true;
    }

    bool VirtualGPUTester::TestDepthStencilOutOfRange() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4, 0);
        VirtualGPU::Attachment att;
        att.ref_id = 123;
        att.memory = &mem;
        att.component_type = VG_FLOAT;
        att.format = VG_DEPTH_COMPONENT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        float old_depth = 0.7f;
        std::memcpy(mem.data(), &old_depth, 4);

        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;

        bool ok = gpu.TestDepthStencil(2.0f, 0.0f, 0.1f, true);
        if (ok) return false;

        float out_depth = 0.0f;
        std::memcpy(&out_depth, mem.data(), 4);

        return out_depth == 0.7f;
    }

    bool VirtualGPUTester::TestDepthStencilNoMemory() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        VirtualGPU::Attachment att;
        att.ref_id = 123;
        att.memory = nullptr;
        att.component_type = VG_FLOAT;
        att.format = VG_DEPTH_COMPONENT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;

        bool ok = gpu.TestDepthStencil(2.0f, 0.0f, 0.1f, true);

        return ok == true;
    }

    bool VirtualGPUTester::TestDepthStencilDepthTestOffWriteOff_DepthComponent() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4, 0);
        VirtualGPU::Attachment att;
        att.ref_id = 123;
        att.memory = &mem;
        att.component_type = VG_FLOAT;
        att.format = VG_DEPTH_COMPONENT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        float old_depth = 0.6f;
        std::memcpy(mem.data(), &old_depth, 4);

        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.depth_write_enabled = false;

        bool ok1 = gpu.TestDepthStencil(0.0f, 0.0f, 0.2f, true);
        if (!ok1) return false;

        bool ok2 = gpu.TestDepthStencil(0.0f, 0.0f, 0.8f, true);
        if (!ok2) return false;

        float out_depth = 0.0f;
        std::memcpy(&out_depth, mem.data(), 4);

        return out_depth == 0.6f;
    }

    bool VirtualGPUTester::TestDepthStencilDepthTestOffWriteOn_DepthComponent() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4, 0);
        VirtualGPU::Attachment att;
        att.ref_id = 123;
        att.memory = &mem;
        att.component_type = VG_FLOAT;
        att.format = VG_DEPTH_COMPONENT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        float old_depth = 0.6f;
        std::memcpy(mem.data(), &old_depth, 4);

        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.depth_write_enabled = true;

        bool ok1 = gpu.TestDepthStencil(0.0f, 0.0f, 0.2f, true);
        if (!ok1) return false;

        bool ok2 = gpu.TestDepthStencil(0.0f, 0.0f, 0.8f, true);
        if (!ok2) return false;

        float out_depth = 0.0f;
        std::memcpy(&out_depth, mem.data(), 4);

        return out_depth == 0.6f;
    }

    bool VirtualGPUTester::TestDepthStencilDepthTestOnWriteOff_DepthComponent() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4, 0);
        VirtualGPU::Attachment att;
        att.ref_id = 123;
        att.memory = &mem;
        att.component_type = VG_FLOAT;
        att.format = VG_DEPTH_COMPONENT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        float old_depth = 0.6f;
        std::memcpy(mem.data(), &old_depth, 4);

        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = false;

        bool ok1 = gpu.TestDepthStencil(0.0f, 0.0f, 0.2f, true);
        if (!ok1) return false;

        bool ok2 = gpu.TestDepthStencil(0.0f, 0.0f, 0.8f, true);
        if (ok2) return false;

        float out_depth = 0.0f;
        std::memcpy(&out_depth, mem.data(), 4);

        return out_depth == 0.6f;
    }

    bool VirtualGPUTester::TestDepthStencilDepthTestOnWriteOn_DepthComponent() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4, 0);
        VirtualGPU::Attachment att;
        att.ref_id = 123;
        att.memory = &mem;
        att.component_type = VG_FLOAT;
        att.format = VG_DEPTH_COMPONENT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        float old_depth = 0.6f;
        std::memcpy(mem.data(), &old_depth, 4);

        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;

        bool ok1 = gpu.TestDepthStencil(0.0f, 0.0f, 0.2f, true);
        if (!ok1) return false;

        bool ok2 = gpu.TestDepthStencil(0.0f, 0.0f, 0.8f, true);
        if (ok2) return false;

        float out_depth = 0.0f;
        std::memcpy(&out_depth, mem.data(), 4);

        return out_depth == 0.2f;
    }

    bool VirtualGPUTester::TestDepthStencilDepthComparison() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4, 0);
        VirtualGPU::Attachment att;
        att.ref_id = 123;
        att.memory = &mem;
        att.component_type = VG_FLOAT;
        att.format = VG_DEPTH_COMPONENT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = false;

        float old_depth = 0.6f;
        std::memcpy(mem.data(), &old_depth, 4);

        gpu.state_.depth_func = VG_NEVER;
        if (gpu.TestDepthStencil(0.f, 0.f, 0.2f, true)) return false;
        if (gpu.TestDepthStencil(0.f, 0.f, 0.8f, true)) return false;

        gpu.state_.depth_func = VG_LESS;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.2f, true)) return false;
        if (gpu.TestDepthStencil(0.f, 0.f, 0.8f, true)) return false;

        gpu.state_.depth_func = VG_EQUAL;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.6f, true)) return false;
        if (gpu.TestDepthStencil(0.f, 0.f, 0.8f, true)) return false;

        gpu.state_.depth_func = VG_LEQUAL;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.2f, true)) return false;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.6f, true)) return false;
        if (gpu.TestDepthStencil(0.f, 0.f, 0.8f, true)) return false;

        gpu.state_.depth_func = VG_GREATER;
        if (gpu.TestDepthStencil(0.f, 0.f, 0.2f, true)) return false;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.8f, true)) return false;

        gpu.state_.depth_func = VG_GEQUAL;
        if (gpu.TestDepthStencil(0.f, 0.f, 0.2f, true)) return false;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.6f, true)) return false;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.8f, true)) return false;

        gpu.state_.depth_func = VG_ALWAYS;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.2f, true)) return false;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.6f, true)) return false;
        if (!gpu.TestDepthStencil(0.f, 0.f, 0.8f, true)) return false;

        return true;
    }

    bool VirtualGPUTester::TestDepthStencilDepthOffStencilOff_DepthStencil() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4 * 4 * 4, 0);

        VirtualGPU::Attachment att;
        att.ref_id = 1;
        att.memory = &mem;
        att.component_type = VG_UNSIGNED_INT;
        att.format = VG_DEPTH_STENCIL;
        att.width = 4;
        att.height = 4;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        const int px = 1;
        const int py = 1;
        const size_t idx = size_t(py) * 4 + size_t(px);
        uint8_t* p = mem.data() + idx * 4;

        const float init_depth = 0.25f;
        const uint8_t init_stencil = 0x33;

        uint32_t qd = static_cast<uint32_t>(init_depth * 16777215.0f);
        p[0] = init_stencil;
        p[1] = uint8_t((qd >> 0) & 0xFF);
        p[2] = uint8_t((qd >> 8) & 0xFF);
        p[3] = uint8_t((qd >> 16) & 0xFF);

        gpu.state_.stencil_test_enabled = false;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.depth_write_enabled = false;
        gpu.state_.depth_func = VG_LESS;

        if (!gpu.TestDepthStencil(1.f, 1.f, 0.9f, true)) return false;

        uint8_t out_stencil = p[0];
        uint32_t out_qd = (uint32_t(p[1]) << 0) | (uint32_t(p[2]) << 8) | (uint32_t(p[3]) << 16);
        float out_depth = float(out_qd) / 16777215.0f;

        if (std::fabs(out_depth - init_depth) > 1e-6f) return false;
        if (out_stencil != init_stencil) return false;

        return true;
    }

    bool VirtualGPUTester::TestDepthStencilDepthOnStencilOff_DepthStencil() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4 * 4 * 4, 0);

        VirtualGPU::Attachment att;
        att.ref_id = 1;
        att.memory = &mem;
        att.component_type = VG_UNSIGNED_INT;
        att.format = VG_DEPTH_STENCIL;
        att.width = 4;
        att.height = 4;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        const int px = 1;
        const int py = 1;
        const size_t idx = size_t(py) * 4 + size_t(px);
        uint8_t* p = mem.data() + idx * 4;

        const float init_depth = 0.8f;
        const uint8_t init_stencil = 0x11;

        uint32_t qd = static_cast<uint32_t>(init_depth * 16777215.0f);
        p[0] = init_stencil;
        p[1] = uint8_t((qd >> 0) & 0xFF);
        p[2] = uint8_t((qd >> 8) & 0xFF);
        p[3] = uint8_t((qd >> 16) & 0xFF);

        gpu.state_.stencil_test_enabled = false;
        gpu.state_.stencil_func[0] = VG_NEVER;
        gpu.state_.stencil_ref[0] = 0xAB;
        gpu.state_.stencil_func_mask[0] = 0xFF;
        gpu.state_.stencil_write_mask[0] = 0xFF;
        gpu.state_.stencil_sfail_op[0] = VG_REPLACE;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;
        gpu.state_.depth_func = VG_LESS;

        if (!gpu.TestDepthStencil(1.f, 1.f, 0.2f, true)) return false;

        uint8_t out_stencil = p[0];
        uint32_t out_qd = (uint32_t(p[1]) << 0) | (uint32_t(p[2]) << 8) | (uint32_t(p[3]) << 16);
        float out_depth = float(out_qd) / 16777215.0f;

        if (std::fabs(out_depth - 0.2f) > 1e-6f) return false;
        if (out_stencil != init_stencil) return false;

        return true;
    }

    bool VirtualGPUTester::TestDepthStencilDepthOffStencilOn_DepthStencil() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4 * 4 * 4, 0);

        VirtualGPU::Attachment att;
        att.ref_id = 1;
        att.memory = &mem;
        att.component_type = VG_UNSIGNED_INT;
        att.format = VG_DEPTH_STENCIL;
        att.width = 4;
        att.height = 4;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        const int px = 1;
        const int py = 1;
        const size_t idx = size_t(py) * 4 + size_t(px);
        uint8_t* p = mem.data() + idx * 4;

        const float old_depth = 0.2f;
        const uint8_t old_stencil = 0x55;

        uint32_t qd = static_cast<uint32_t>(old_depth * 16777215.0f);
        p[0] = old_stencil;
        p[1] = uint8_t((qd >> 0) & 0xFF);
        p[2] = uint8_t((qd >> 8) & 0xFF);
        p[3] = uint8_t((qd >> 16) & 0xFF);

        gpu.state_.stencil_test_enabled = true;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.depth_write_enabled = false;
        gpu.state_.depth_func = VG_LESS;

        if (!gpu.TestDepthStencil(1.f, 1.f, 0.8f, true)) return false;

        uint8_t out_stencil = p[0];
        uint32_t out_qd = (uint32_t(p[1]) << 0) | (uint32_t(p[2]) << 8) | (uint32_t(p[3]) << 16);
        float out_depth = float(out_qd) / 16777215.0f;

        if (std::fabs(out_depth - old_depth) > 1e-6f) return false;
        if (out_stencil != old_stencil) return false;

        return true;
    }

    bool VirtualGPUTester::TestDepthStencilDepthPassStencilFail_DepthStencil() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4 * 4 * 4, 0);

        VirtualGPU::Attachment att;
        att.ref_id = 1;
        att.memory = &mem;
        att.component_type = VG_UNSIGNED_INT;
        att.format = VG_DEPTH_STENCIL;
        att.width = 4;
        att.height = 4;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        const int px = 1;
        const int py = 1;
        const size_t idx = size_t(py) * 4 + size_t(px);
        uint8_t* p = mem.data() + idx * 4;

        const float old_depth = 0.8f;
        const uint8_t old_stencil = 0x55;

        uint32_t qd = static_cast<uint32_t>(old_depth * 16777215.0f);
        p[0] = old_stencil;
        p[1] = uint8_t((qd >> 0) & 0xFF);
        p[2] = uint8_t((qd >> 8) & 0xFF);
        p[3] = uint8_t((qd >> 16) & 0xFF);

        gpu.state_.stencil_test_enabled = true;
        gpu.state_.stencil_func[0] = VG_NEVER;
        gpu.state_.stencil_ref[0] = 0xAB;
        gpu.state_.stencil_func_mask[0] = 0xFF;
        gpu.state_.stencil_write_mask[0] = 0xFF;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;
        gpu.state_.depth_func = VG_LESS;

        if (gpu.TestDepthStencil(1.f, 1.f, 0.2f, true)) return false;

        uint8_t out_stencil = p[0];
        uint32_t out_qd = (uint32_t(p[1]) << 0) | (uint32_t(p[2]) << 8) | (uint32_t(p[3]) << 16);
        float out_depth = float(out_qd) / 16777215.0f;

        if (std::fabs(out_depth - old_depth) > 1e-6f) return false;
        if (out_stencil != old_stencil) return false;

        return true;
    }

    bool VirtualGPUTester::TestDepthStencilDepthFailStencilPass_DepthStencil() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4 * 4 * 4, 0);

        VirtualGPU::Attachment att;
        att.ref_id = 1;
        att.memory = &mem;
        att.component_type = VG_UNSIGNED_INT;
        att.format = VG_DEPTH_STENCIL;
        att.width = 4;
        att.height = 4;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        const int px = 1;
        const int py = 1;
        const size_t idx = size_t(py) * 4 + size_t(px);
        uint8_t* p = mem.data() + idx * 4;

        const float old_depth = 0.2f;
        const uint8_t old_stencil = 0x10;

        uint32_t qd = static_cast<uint32_t>(old_depth * 16777215.0f);
        p[0] = old_stencil;
        p[1] = uint8_t((qd >> 0) & 0xFF);
        p[2] = uint8_t((qd >> 8) & 0xFF);
        p[3] = uint8_t((qd >> 16) & 0xFF);

        gpu.state_.stencil_test_enabled = true;
        gpu.state_.stencil_func[0] = VG_ALWAYS;
        gpu.state_.stencil_ref[0] = 0xAB;
        gpu.state_.stencil_func_mask[0] = 0xFF;
        gpu.state_.stencil_write_mask[0] = 0xFF;
        gpu.state_.stencil_dpfail_op[0] = VG_REPLACE;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;
        gpu.state_.depth_func = VG_LESS;

        if (gpu.TestDepthStencil(1.f, 1.f, 0.9f, true)) return false;

        uint8_t out_stencil = p[0];
        uint32_t out_qd = (uint32_t(p[1]) << 0) | (uint32_t(p[2]) << 8) | (uint32_t(p[3]) << 16);
        float out_depth = float(out_qd) / 16777215.0f;

        if (std::fabs(out_depth - old_depth) > 1e-6f) return false;
        if (out_stencil != uint8_t(0xAB)) return false;

        return true;
    }

    bool VirtualGPUTester::TestDepthStencilDepthPassStencilPass_DepthStencil() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(4 * 4 * 4, 0);

        VirtualGPU::Attachment att;
        att.ref_id = 1;
        att.memory = &mem;
        att.component_type = VG_UNSIGNED_INT;
        att.format = VG_DEPTH_STENCIL;
        att.width = 4;
        att.height = 4;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        const int px = 1;
        const int py = 1;
        const size_t idx = size_t(py) * 4 + size_t(px);
        uint8_t* p = mem.data() + idx * 4;

        const float old_depth = 0.8f;
        const uint8_t old_stencil = 0x01;

        uint32_t qd = static_cast<uint32_t>(old_depth * 16777215.0f);
        p[0] = old_stencil;
        p[1] = uint8_t((qd >> 0) & 0xFF);
        p[2] = uint8_t((qd >> 8) & 0xFF);
        p[3] = uint8_t((qd >> 16) & 0xFF);

        gpu.state_.stencil_test_enabled = true;
        gpu.state_.stencil_func[0] = VG_ALWAYS;
        gpu.state_.stencil_ref[0] = 0x5A;
        gpu.state_.stencil_func_mask[0] = 0xFF;
        gpu.state_.stencil_write_mask[0] = 0xFF;
        gpu.state_.stencil_dppass_op[0] = VG_REPLACE;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_write_enabled = true;

        const float in_depth = 0.2f;

        if (!gpu.TestDepthStencil(1.f, 1.f, in_depth, true)) return false;

        uint8_t out_stencil = p[0];
        uint32_t out_qd = (uint32_t(p[1]) << 0) | (uint32_t(p[2]) << 8) | (uint32_t(p[3]) << 16);
        float out_depth = float(out_qd) / 16777215.0f;

        if (std::fabs(out_depth - in_depth) > 1e-6f) return false;
        if (out_stencil != uint8_t(0x5A)) return false;

        return true;
    }

    bool VirtualGPUTester::GetBlendFactorZeroOne() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Color128 src(0.2f, 0.4f, 0.6f, 0.8f);
        Color128 dst(0.9f, 0.7f, 0.5f, 0.3f);

        for (uint32_t c = 0; c < 4; ++c) {
            if (gpu.GetBlendFactor(VG_ZERO, src, dst, c) != 0.0_r) return false;
            if (gpu.GetBlendFactor(VG_ONE, src, dst, c) != 1.0_r) return false;
        }
        return true;
    }
    bool VirtualGPUTester::GetBlendFactorSrcColor() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Color128 src(0.1f, 0.2f, 0.3f, 0.4f);
        Color128 dst(0.f, 0.f, 0.f, 0.f);

        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_SRC_COLOR, src, dst, 0), src.r)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_SRC_COLOR, src, dst, 1), src.g)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_SRC_COLOR, src, dst, 2), src.b)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_SRC_COLOR, src, dst, 3), src.a)) return false;

        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_SRC_COLOR, src, dst, 0), 1.0_r - src.r)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_SRC_COLOR, src, dst, 1), 1.0_r - src.g)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_SRC_COLOR, src, dst, 2), 1.0_r - src.b)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_SRC_COLOR, src, dst, 3), 1.0_r - src.a)) return false;

        return true;
    }
    bool VirtualGPUTester::GetBlendFactorDstColor() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Color128 src(0.f, 0.f, 0.f, 0.f);
        Color128 dst(0.6f, 0.5f, 0.4f, 0.3f);

        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_DST_COLOR, src, dst, 0), dst.r)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_DST_COLOR, src, dst, 1), dst.g)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_DST_COLOR, src, dst, 2), dst.b)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_DST_COLOR, src, dst, 3), dst.a)) return false;

        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_DST_COLOR, src, dst, 0), 1.0_r - dst.r)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_DST_COLOR, src, dst, 1), 1.0_r - dst.g)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_DST_COLOR, src, dst, 2), 1.0_r - dst.b)) return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_DST_COLOR, src, dst, 3), 1.0_r - dst.a)) return false;
        return true;
    }
    bool VirtualGPUTester::GetBlendFactorAlphaBased() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Color128 src(0.2f, 0.3f, 0.4f, 0.6f);
        Color128 dst(0.7f, 0.6f, 0.5f, 0.8f);

        for (uint32_t c = 0; c < 4; ++c) {
            if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_SRC_ALPHA, src, dst, c), 0.6_r)) return false;
        }

        for (uint32_t c = 0; c < 4; ++c) {
            if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_SRC_ALPHA, src, dst, c), 0.4_r)) return false;
        }

        for (uint32_t c = 0; c < 4; ++c) {
            if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_DST_ALPHA, src, dst, c), 0.8_r)) return false;
        }

        for (uint32_t c = 0; c < 4; ++c) {
            if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_DST_ALPHA, src, dst, c), 0.2_r)) return false;
        }

        return true;
    }

    bool VirtualGPUTester::GetBlendFactorConstantColorBased() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.blend_constant = Color128(0.3f, 0.4f, 0.5f, 0.6f);

        Color128 src(0.2f, 0.3f, 0.4f, 0.6f);
        Color128 dst(0.7f, 0.6f, 0.5f, 0.8f);

        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_CONSTANT_COLOR, src, dst, 0), gpu.state_.blend_constant.r))
            return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_CONSTANT_COLOR, src, dst, 1), gpu.state_.blend_constant.g))
            return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_CONSTANT_COLOR, src, dst, 2), gpu.state_.blend_constant.b))
            return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_CONSTANT_COLOR, src, dst, 3), gpu.state_.blend_constant.a))
            return false;

        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_CONSTANT_COLOR, src, dst, 0),
                                 1.0_r - gpu.state_.blend_constant.r))
            return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_CONSTANT_COLOR, src, dst, 1),
                                 1.0_r - gpu.state_.blend_constant.g))
            return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_CONSTANT_COLOR, src, dst, 2),
                                 1.0_r - gpu.state_.blend_constant.b))
            return false;
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_CONSTANT_COLOR, src, dst, 3),
                                 1.0_r - gpu.state_.blend_constant.a))
            return false;

        for (uint32_t c = 0; c < 4; ++c) {
            if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_CONSTANT_ALPHA, src, dst, c), gpu.state_.blend_constant.a))
                return false;
        }

        for (uint32_t c = 0; c < 4; ++c) {
            if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_ONE_MINUS_CONSTANT_ALPHA, src, dst, c),
                                     1.0_r - gpu.state_.blend_constant.a))
                return false;
        }

        return true;
    }

    bool VirtualGPUTester::GetBlendFactorSrcAlphaSaturate() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        Color128 src(0.f, 0.f, 0.f, 0.7f);
        Color128 dst(0.f, 0.f, 0.f, 0.3f);

        real expected_rgb = math::Min(0.7_r, 1.0_r - 0.3_r);

        // RGB
        for (uint32_t c = 0; c < 3; ++c) {
            if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_SRC_ALPHA_SATURATE, src, dst, c), expected_rgb))
                return false;
        }

        // Alpha
        if (!math::IsEqualApprox(gpu.GetBlendFactor(VG_SRC_ALPHA_SATURATE, src, dst, 3), 1.0_r)) return false;

        return true;
    }

    bool VirtualGPUTester::ApplyBlendEquationAdd() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        real out = gpu.ApplyBlendEquation(VG_FUNC_ADD, 0.3_r, 0.8_r);
        return math::IsEqualApprox(out, 1.1_r);
    }

    bool VirtualGPUTester::ApplyBlendEquationSubtract() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        real out = gpu.ApplyBlendEquation(VG_FUNC_SUBTRACT, 0.3_r, 0.8_r);
        return math::IsEqualApprox(out, -0.5_r);
    }

    bool VirtualGPUTester::ApplyBlendEquationReverseSubtract() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        real out = gpu.ApplyBlendEquation(VG_FUNC_REVERSE_SUBTRACT, 0.3_r, 0.8_r);
        return math::IsEqualApprox(out, 0.5_r);
    }

    bool VirtualGPUTester::ApplyBlendEquationMin() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        real out = gpu.ApplyBlendEquation(VG_MIN, 0.3_r, 0.8_r);
        return math::IsEqualApprox(out, 0.3_r);
    }

    bool VirtualGPUTester::ApplyBlendEquationMax() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        real out = gpu.ApplyBlendEquation(VG_MAX, 0.3_r, 0.8_r);
        return math::IsEqualApprox(out, 0.8_r);
    }

    bool VirtualGPUTester::WriteColorNoFrameBuffer() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.bound_draw_frame_buffer_ = nullptr;

        // should not crash
        gpu.WriteColor(0.0_r, 0.0_r, Color128(1.f, 1.f, 1.f, 1.f), 0);

        return true;
    }

    bool VirtualGPUTester::WriteColorDrawSlotDisabled() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        fb.draw_slot_to_color_attachment[0] = -1;
        gpu.bound_draw_frame_buffer_ = &fb;

        // should not crash
        gpu.WriteColor(0.0_r, 0.0_r, Color128(1.f, 0.f, 0.f, 1.f), 0);

        return true;
    }

    bool VirtualGPUTester::WriteColorAttachmentNoMemory() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        VirtualGPU::Attachment att;
        att.memory = nullptr;
        att.format = VG_RGBA;
        att.component_type = VG_FLOAT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.color_attachments[0] = att;
        fb.draw_slot_to_color_attachment[0] = 0;

        gpu.bound_draw_frame_buffer_ = &fb;

        // should not crash
        gpu.WriteColor(0.0_r, 0.0_r, Color128(1.f, 1.f, 1.f, 1.f), 0);

        return true;
    }

    bool VirtualGPUTester::WriteColorNoBlendNoMask() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(sizeof(float) * 4, 0);

        VirtualGPU::Attachment att;
        att.memory = &mem;
        att.format = VG_RGBA;
        att.component_type = VG_FLOAT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.color_attachments[0] = att;
        fb.draw_slot_to_color_attachment[0] = 0;

        gpu.bound_draw_frame_buffer_ = &fb;

        VirtualGPU::DrawBufferState& dbs = gpu.state_.draw_buffer_states[0];
        dbs.blend_enabled = false;
        dbs.color_mask[0] = true;
        dbs.color_mask[1] = true;
        dbs.color_mask[2] = true;
        dbs.color_mask[3] = true;

        Color128 src(0.1f, 0.2f, 0.3f, 0.4f);
        gpu.WriteColor(0.0_r, 0.0_r, src, 0);

        const float* out = reinterpret_cast<const float*>(mem.data());

        if (out[0] != src.r) return false;
        if (out[1] != src.g) return false;
        if (out[2] != src.b) return false;
        if (out[3] != src.a) return false;

        return true;
    }

    bool VirtualGPUTester::WriteColorMaskPartial() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(sizeof(float) * 4, 0);

        float* dst = reinterpret_cast<float*>(mem.data());
        dst[0] = 0.1f;
        dst[1] = 0.2f;
        dst[2] = 0.3f;
        dst[3] = 0.4f;

        VirtualGPU::Attachment att;
        att.memory = &mem;
        att.format = VG_RGBA;
        att.component_type = VG_FLOAT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.color_attachments[0] = att;
        fb.draw_slot_to_color_attachment[0] = 0;

        gpu.bound_draw_frame_buffer_ = &fb;

        VirtualGPU::DrawBufferState& dbs = gpu.state_.draw_buffer_states[0];
        dbs.blend_enabled = false;
        dbs.color_mask[0] = true;   // R
        dbs.color_mask[1] = false;  // G
        dbs.color_mask[2] = false;  // B
        dbs.color_mask[3] = true;   // A

        Color128 src(1.f, 1.f, 1.f, 1.f);
        gpu.WriteColor(0.0_r, 0.0_r, src, 0);

        const float* out = reinterpret_cast<const float*>(mem.data());

        if (out[0] != 1.f) return false;   // R updated
        if (out[1] != 0.2f) return false;  // G preserved
        if (out[2] != 0.3f) return false;  // B preserved
        if (out[3] != 1.f) return false;   // A updated

        return true;
    }

    bool VirtualGPUTester::WriteColorBlendSrcOnly() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(sizeof(float) * 4, 0);

        float* dst = reinterpret_cast<float*>(mem.data());
        dst[0] = 0.9f;
        dst[1] = 0.8f;
        dst[2] = 0.7f;
        dst[3] = 0.6f;

        VirtualGPU::Attachment att;
        att.memory = &mem;
        att.format = VG_RGBA;
        att.component_type = VG_FLOAT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.color_attachments[0] = att;
        fb.draw_slot_to_color_attachment[0] = 0;
        gpu.bound_draw_frame_buffer_ = &fb;

        VirtualGPU::DrawBufferState& dbs = gpu.state_.draw_buffer_states[0];
        dbs.blend_enabled = true;
        dbs.color_mask[0] = true;
        dbs.color_mask[1] = true;
        dbs.color_mask[2] = true;
        dbs.color_mask[3] = true;

        gpu.state_.blend_src_rgb_factor = VG_ONE;
        gpu.state_.blend_dst_rgb_factor = VG_ZERO;
        gpu.state_.blend_rgb_equation = VG_FUNC_ADD;
        gpu.state_.blend_src_alpha_factor = VG_ONE;
        gpu.state_.blend_dst_alpha_factor = VG_ZERO;
        gpu.state_.blend_alpha_equation = VG_FUNC_ADD;

        Color128 src(0.2f, 0.3f, 0.4f, 0.5f);
        gpu.WriteColor(0.0_r, 0.0_r, src, 0);

        const float* out = reinterpret_cast<const float*>(mem.data());

        if (out[0] != src.r) return false;
        if (out[1] != src.g) return false;
        if (out[2] != src.b) return false;
        if (out[3] != src.a) return false;

        return true;
    }

    bool VirtualGPUTester::WriteColorBlendDstOnly() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> mem(sizeof(float) * 4, 0);

        float* dst = reinterpret_cast<float*>(mem.data());
        dst[0] = 0.4f;
        dst[1] = 0.5f;
        dst[2] = 0.6f;
        dst[3] = 0.7f;

        VirtualGPU::Attachment att;
        att.memory = &mem;
        att.format = VG_RGBA;
        att.component_type = VG_FLOAT;
        att.width = 1;
        att.height = 1;
        att.offset = 0;

        fb.color_attachments[0] = att;
        fb.draw_slot_to_color_attachment[0] = 0;
        gpu.bound_draw_frame_buffer_ = &fb;

        VirtualGPU::DrawBufferState& dbs = gpu.state_.draw_buffer_states[0];
        dbs.blend_enabled = true;
        dbs.color_mask[0] = true;
        dbs.color_mask[1] = true;
        dbs.color_mask[2] = true;
        dbs.color_mask[3] = true;

        gpu.state_.blend_src_rgb_factor = VG_ZERO;
        gpu.state_.blend_dst_rgb_factor = VG_ONE;
        gpu.state_.blend_rgb_equation = VG_FUNC_ADD;
        gpu.state_.blend_src_alpha_factor = VG_ZERO;
        gpu.state_.blend_dst_alpha_factor = VG_ONE;
        gpu.state_.blend_alpha_equation = VG_FUNC_ADD;

        Color128 src(1.f, 1.f, 1.f, 1.f);
        gpu.WriteColor(0.0_r, 0.0_r, src, 0);

        const float* out = reinterpret_cast<const float*>(mem.data());

        if (out[0] != 0.4f) return false;
        if (out[1] != 0.5f) return false;
        if (out[2] != 0.6f) return false;
        if (out[3] != 0.7f) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizePoint() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> depth_mem(4, 0);
        VirtualGPU::Attachment depth;
        depth.memory = &depth_mem;
        depth.format = VG_DEPTH_COMPONENT;
        depth.component_type = VG_FLOAT;
        depth.width = 1;
        depth.height = 1;
        depth.offset = 0;

        fb.depth_stencil_attachment = depth;
        gpu.bound_draw_frame_buffer_ = &fb;

        float old_depth = 1.0f;
        std::memcpy(depth_mem.data(), &old_depth, 4);

        gpu.state_.scissor_test_enabled = false;
        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;
        gpu.state_.depth_func = VG_LESS;

        VirtualGPU::Varying v;
        v.viewport_coord = Vector3(0.0_r, 0.0_r, 0.5_r);

        v.used_smooth_register_size = 6;
        v.smooth_register[0] = 123.4f;
        v.smooth_register[1] = 10.0f;
        v.smooth_register[2] = 20.0f;
        v.smooth_register[3] = 0.0f;
        v.smooth_register[4] = 1.0f;
        v.smooth_register[5] = 0.0f;

        v.used_flat_register_size = 1;
        v.flat_register[0] = 999.0f;

        auto out = gpu.Rasterize(v);

        if (out.size() != 1) return false;

        const auto& frag = out[0];

        if (!math::IsEqualApprox(frag.screen_coord.x, 0.5_r)) return false;
        if (!math::IsEqualApprox(frag.screen_coord.y, 0.5_r)) return false;
        if (!math::IsEqualApprox(frag.depth, 0.5_r)) return false;

        if (frag.used_smooth_register_size != v.used_smooth_register_size) return false;
        for (uint32_t i = 0; i < v.used_smooth_register_size; ++i) {
            if (!math::IsEqualApprox(frag.smooth_register[i], v.smooth_register[i])) return false;
        }

        if (frag.used_flat_register_size != v.used_flat_register_size) return false;
        for (uint32_t i = 0; i < v.used_flat_register_size; ++i) {
            if (!math::IsEqualApprox(frag.flat_register[i], v.flat_register[i])) return false;
        }

        return true;
    }

    bool VirtualGPUTester::RasterizePointEarlyDepthFailed() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> depth_mem(4, 0);
        VirtualGPU::Attachment depth;
        depth.memory = &depth_mem;
        depth.format = VG_DEPTH_COMPONENT;
        depth.component_type = VG_FLOAT;
        depth.width = 1;
        depth.height = 1;
        depth.offset = 0;

        fb.depth_stencil_attachment = depth;
        gpu.bound_draw_frame_buffer_ = &fb;

        float old_depth = 0.1f;
        std::memcpy(depth_mem.data(), &old_depth, 4);

        gpu.state_.scissor_test_enabled = false;
        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;
        gpu.state_.depth_func = VG_LESS;

        VirtualGPU::Varying v;
        v.viewport_coord = Vector3(0.0_r, 0.0_r, 0.9_r);

        auto out = gpu.Rasterize(v);
        if (!out.empty()) return false;

        float out_depth;
        std::memcpy(&out_depth, depth_mem.data(), 4);
        if (!math::IsEqualApprox(out_depth, old_depth)) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizePointEarlyStencilFailed() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::FrameBuffer fb;
        fb.id = 1;

        std::vector<uint8_t> depth_mem(4, 0);
        VirtualGPU::Attachment depth;
        depth.memory = &depth_mem;
        depth.format = VG_DEPTH_STENCIL;
        depth.component_type = VG_UNSIGNED_INT;
        depth.width = 1;
        depth.height = 1;
        depth.offset = 0;

        fb.depth_stencil_attachment = depth;
        gpu.bound_draw_frame_buffer_ = &fb;

        const float init_depth = 0.5f;
        const uint8_t init_stencil = 0x01;

        uint32_t packed = 0;
        {
            uint32_t depth_bits = static_cast<uint32_t>(init_depth * ((1u << 24) - 1));
            packed = (depth_bits << 8) | init_stencil;
        }
        std::memcpy(depth_mem.data(), &packed, 4);

        gpu.state_.scissor_test_enabled = false;

        gpu.state_.stencil_test_enabled = true;
        gpu.state_.stencil_func[0] = VG_NEVER;
        gpu.state_.stencil_ref[0] = 0xFF;
        gpu.state_.stencil_func_mask[0] = 0xFF;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_write_enabled = true;
        gpu.state_.depth_func = VG_ALWAYS;

        VirtualGPU::Varying v;
        v.viewport_coord = Vector3(0.0_r, 0.0_r, 0.2_r);

        auto out = gpu.Rasterize(v);
        if (!out.empty()) return false;

        uint32_t out_packed = 0;
        std::memcpy(&out_packed, depth_mem.data(), 4);

        uint32_t out_depth_bits = out_packed >> 8;
        uint8_t out_stencil = static_cast<uint8_t>(out_packed & 0xFF);

        float out_depth = static_cast<float>(out_depth_bits) / static_cast<float>((1u << 24) - 1);

        if (!math::IsEqualApprox(out_depth, init_depth)) return false;
        if (out_stencil != init_stencil) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeLineNonSteep() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) return false;

        VirtualGPU::Varying v0, v1;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(40.0_r, 18.0_r, 0.8_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        const std::vector<float> v0_data = {1.0f, 2.0f, 3.0f, -2.0f, 4.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                                            0.0f, 0.0f, 1.0f, 1.0f,  0.1f,  0.2f, 0.3f, 0.4f, 10.f,
                                            20.f, 30.f, 40.f, 5.f,   250.f, 60.f, 255.f};
        const std::vector<float> v1_data = {11.0f, 7.0f,  5.0f, 6.0f,  9.0f, -3.0f, 1.0f, 0.0f, 1.0f,
                                            1.0f,  0.0f,  0.0f, 1.0f,  0.9f, 0.7f,  0.8f, 0.1f, 200.f,
                                            150.f, 100.f, 80.f, 220.f, 10.f, 180.f, 128.f};

        v0.used_smooth_register_size = (uint32_t)v0_data.size();
        std::copy(v0_data.begin(), v0_data.end(), v0.smooth_register.begin());

        v1.used_smooth_register_size = (uint32_t)v1_data.size();
        std::copy(v1_data.begin(), v1_data.end(), v1.smooth_register.begin());

        v0.used_flat_register_size = 1;
        v0.flat_register[0] = 100.0f;

        v1.used_flat_register_size = 1;
        v1.flat_register[0] = 200.0f;

        gpu.state_.scissor_test_enabled = false;
        gpu.state_.depth_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1);
        if (out.size() < 10u) return false;

        const int x0 = (int)math::Floor(v0.viewport_coord.x);
        const int y0 = (int)math::Floor(v0.viewport_coord.y);
        const int x1 = (int)math::Floor(v1.viewport_coord.x);
        const int y1 = (int)math::Floor(v1.viewport_coord.y);

        const real min_sx = (real)math::Min(x0, x1) + 0.5_r;
        const real max_sx = (real)math::Max(x0, x1) + 0.5_r;
        const real min_sy = (real)math::Min(y0, y1) + 0.5_r;
        const real max_sy = (real)math::Max(y0, y1) + 0.5_r;

        const real min_depth = math::Min(v0.viewport_coord.z, v1.viewport_coord.z);
        const real max_depth = math::Max(v0.viewport_coord.z, v1.viewport_coord.z);

        std::vector<float> smooth_min(v0.used_smooth_register_size);
        std::vector<float> smooth_max(v0.used_smooth_register_size);
        for (uint32_t i = 0; i < v0.used_smooth_register_size; ++i) {
            smooth_min[i] = math::Min(v0.smooth_register[i], v1.smooth_register[i]);
            smooth_max[i] = math::Max(v0.smooth_register[i], v1.smooth_register[i]);
        }

        const real eps = math::EPSILON_RASTERIZATION;

        for (const auto& f : out) {
            if (f.screen_coord.x < min_sx - eps || f.screen_coord.x > max_sx + eps) return false;
            if (f.screen_coord.y < min_sy - eps || f.screen_coord.y > max_sy + eps) return false;
            if (f.depth < min_depth - eps || f.depth > max_depth + eps) return false;

            if (f.used_smooth_register_size != v0.used_smooth_register_size) return false;
            for (uint32_t i = 0; i < f.used_smooth_register_size; ++i) {
                float val = f.smooth_register[i];
                if (val < smooth_min[i] - eps || val > smooth_max[i] + eps) return false;
            }

            if (f.used_flat_register_size != v1.used_flat_register_size) return false;

            if (!math::IsEqualApprox(f.flat_register[0], v1.flat_register[0], eps)) return false;

            if (math::IsEqualApprox(f.flat_register[0], v0.flat_register[0], eps)) return false;
        }

        return true;
    }

    bool VirtualGPUTester::RasterizeLineSteep() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) return false;

        VirtualGPU::Varying v0, v1;

        v0.viewport_coord = Vector3(12.0_r, 10.0_r, 0.15_r);
        v1.viewport_coord = Vector3(18.0_r, 45.0_r, 0.85_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        const std::vector<float> v0_data = {2.0f, 0.0f,  1.0f,  -1.0f, 2.0f, 3.0f, 0.0f, 1.0f, 1.0f,
                                            0.0f, 1.0f,  0.0f,  1.0f,  0.2f, 0.1f, 0.9f, 0.2f, 0.f,
                                            10.f, 200.f, 255.f, 60.f,  70.f, 80.f, 90.f};

        const std::vector<float> v1_data = {5.0f, 9.0f, 7.0f, 4.0f, 8.0f,  -2.0f, 0.0f, 0.0f, 1.0f,
                                            0.0f, 0.0f, 1.0f, 1.0f, 0.4f,  0.9f,  0.1f, 0.8f, 255.f,
                                            40.f, 20.f, 10.f, 10.f, 200.f, 30.f,  250.f};

        v0.used_smooth_register_size = (uint32_t)v0_data.size();
        std::copy(v0_data.begin(), v0_data.end(), v0.smooth_register.begin());

        v1.used_smooth_register_size = (uint32_t)v1_data.size();
        std::copy(v1_data.begin(), v1_data.end(), v1.smooth_register.begin());

        v0.used_flat_register_size = 1;
        v0.flat_register[0] = 100.0f;

        v1.used_flat_register_size = 1;
        v1.flat_register[0] = 200.0f;

        gpu.state_.scissor_test_enabled = false;
        gpu.state_.depth_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1);
        if (out.size() < 10u) return false;

        const int x0 = (int)math::Floor(v0.viewport_coord.x);
        const int y0 = (int)math::Floor(v0.viewport_coord.y);
        const int x1 = (int)math::Floor(v1.viewport_coord.x);
        const int y1 = (int)math::Floor(v1.viewport_coord.y);

        const real min_sx = (real)math::Min(x0, x1) + 0.5_r;
        const real max_sx = (real)math::Max(x0, x1) + 0.5_r;
        const real min_sy = (real)math::Min(y0, y1) + 0.5_r;
        const real max_sy = (real)math::Max(y0, y1) + 0.5_r;

        const real min_depth = math::Min(v0.viewport_coord.z, v1.viewport_coord.z);
        const real max_depth = math::Max(v0.viewport_coord.z, v1.viewport_coord.z);

        std::vector<float> smooth_min(v0.used_smooth_register_size);
        std::vector<float> smooth_max(v0.used_smooth_register_size);

        for (uint32_t i = 0; i < v0.used_smooth_register_size; ++i) {
            smooth_min[i] = math::Min(v0.smooth_register[i], v1.smooth_register[i]);
            smooth_max[i] = math::Max(v0.smooth_register[i], v1.smooth_register[i]);
        }

        const real eps = math::EPSILON_RASTERIZATION;

        for (const auto& f : out) {
            if (f.screen_coord.x < min_sx - eps || f.screen_coord.x > max_sx + eps) return false;
            if (f.screen_coord.y < min_sy - eps || f.screen_coord.y > max_sy + eps) return false;
            if (f.depth < min_depth - eps || f.depth > max_depth + eps) return false;

            if (f.used_smooth_register_size != v0.used_smooth_register_size) return false;
            for (uint32_t i = 0; i < f.used_smooth_register_size; ++i) {
                float val = f.smooth_register[i];
                float mn = smooth_min[i];
                float mx = smooth_max[i];

                if (val < mn - eps || val > mx + eps) return false;
            }

            if (f.used_flat_register_size != v1.used_flat_register_size) return false;

            if (!math::IsEqualApprox(f.flat_register[0], v1.flat_register[0], eps)) return false;

            if (math::IsEqualApprox(f.flat_register[0], v0.flat_register[0], eps)) return false;
        }

        return true;
    }

    bool VirtualGPUTester::RasterizeLineVertical() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) return false;

        VirtualGPU::Varying v0, v1;

        v0.viewport_coord = Vector3(25.0_r, 10.0_r, 0.4_r);
        v1.viewport_coord = Vector3(25.0_r, 30.0_r, 0.6_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        const std::vector<float> v0_data = {2.0f, 0.0f, 0.0f,  1.0f,  -2.0f, 3.0f, 0.0f, 0.0f, 1.0f,
                                            1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.f,
                                            0.f,  0.f,  255.f, 255.f, 0.f,   0.f,  255.f};

        const std::vector<float> v1_data = {2.0f,  10.0f, 4.0f,  1.0f, 5.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                                            0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 255.f,
                                            255.f, 255.f, 255.f, 0.f,  0.f,  255.f, 255.f};

        v0.used_smooth_register_size = (uint32_t)v0_data.size();
        std::copy(v0_data.begin(), v0_data.end(), v0.smooth_register.begin());

        v1.used_smooth_register_size = (uint32_t)v1_data.size();
        std::copy(v1_data.begin(), v1_data.end(), v1.smooth_register.begin());

        v0.used_flat_register_size = 1;
        v0.flat_register[0] = 100.0f;

        v1.used_flat_register_size = 1;
        v1.flat_register[0] = 200.0f;

        gpu.state_.scissor_test_enabled = false;
        gpu.state_.depth_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1);
        if (out.size() < 10u) return false;

        const int x0 = (int)math::Floor(v0.viewport_coord.x);
        const int y0 = (int)math::Floor(v0.viewport_coord.y);
        const int x1 = (int)math::Floor(v1.viewport_coord.x);
        const int y1 = (int)math::Floor(v1.viewport_coord.y);

        const real min_sx = (real)math::Min(x0, x1) + 0.5_r;
        const real max_sx = (real)math::Max(x0, x1) + 0.5_r;
        const real min_sy = (real)math::Min(y0, y1) + 0.5_r;
        const real max_sy = (real)math::Max(y0, y1) + 0.5_r;

        const real min_depth = math::Min(v0.viewport_coord.z, v1.viewport_coord.z);
        const real max_depth = math::Max(v0.viewport_coord.z, v1.viewport_coord.z);

        std::vector<float> smooth_min(v0.used_smooth_register_size);
        std::vector<float> smooth_max(v0.used_smooth_register_size);

        for (uint32_t i = 0; i < v0.used_smooth_register_size; ++i) {
            smooth_min[i] = math::Min(v0.smooth_register[i], v1.smooth_register[i]);
            smooth_max[i] = math::Max(v0.smooth_register[i], v1.smooth_register[i]);
        }

        const real eps = math::EPSILON_RASTERIZATION;

        for (const auto& f : out) {
            if (f.screen_coord.x < min_sx - eps || f.screen_coord.x > max_sx + eps) return false;
            if (f.screen_coord.y < min_sy - eps || f.screen_coord.y > max_sy + eps) return false;
            if (f.depth < min_depth - eps || f.depth > max_depth + eps) return false;

            if (f.used_smooth_register_size != v0.used_smooth_register_size) return false;
            for (uint32_t i = 0; i < f.used_smooth_register_size; ++i) {
                float val = f.smooth_register[i];
                float mn = smooth_min[i];
                float mx = smooth_max[i];

                if (val < mn - eps || val > mx + eps) return false;
            }

            if (f.used_flat_register_size != v1.used_flat_register_size) return false;

            if (!math::IsEqualApprox(f.flat_register[0], v1.flat_register[0], eps)) return false;

            if (math::IsEqualApprox(f.flat_register[0], v0.flat_register[0], eps)) return false;
        }

        return true;
    }
    bool VirtualGPUTester::RasterizeLineHorizontal() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) return false;

        VirtualGPU::Varying v0, v1;

        v0.viewport_coord = Vector3(8.0_r, 22.0_r, 0.1_r);
        v1.viewport_coord = Vector3(35.0_r, 22.0_r, 0.9_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        const std::vector<float> v0_data = {-3.0f, 5.0f, 2.0f, 0.0f, 3.0f,  1.0f, 1.0f, 0.0f, 1.0f,
                                            0.0f,  0.0f, 1.0f, 1.0f, 0.0f,  0.3f, 0.7f, 0.2f, 5.f,
                                            10.f,  15.f, 20.f, 0.f,  255.f, 0.f,  100.f};

        const std::vector<float> v1_data = {9.0f, 5.0f,  -4.0f, 6.0f,  -1.0f, 8.0f,  0.0f, 0.0f, 1.0f,
                                            1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  0.3f,  0.2f, 0.9f, 200.f,
                                            10.f, 250.f, 220.f, 255.f, 0.f,   255.f, 200.f};

        v0.used_smooth_register_size = (uint32_t)v0_data.size();
        std::copy(v0_data.begin(), v0_data.end(), v0.smooth_register.begin());

        v1.used_smooth_register_size = (uint32_t)v1_data.size();
        std::copy(v1_data.begin(), v1_data.end(), v1.smooth_register.begin());

        v0.used_flat_register_size = 1;
        v0.flat_register[0] = 100.0f;

        v1.used_flat_register_size = 1;
        v1.flat_register[0] = 200.0f;

        gpu.state_.scissor_test_enabled = false;
        gpu.state_.depth_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1);
        if (out.size() < 10u) return false;

        const int x0 = (int)math::Floor(v0.viewport_coord.x);
        const int y0 = (int)math::Floor(v0.viewport_coord.y);
        const int x1 = (int)math::Floor(v1.viewport_coord.x);
        const int y1 = (int)math::Floor(v1.viewport_coord.y);

        const real min_sx = (real)math::Min(x0, x1) + 0.5_r;
        const real max_sx = (real)math::Max(x0, x1) + 0.5_r;

        const real min_sy = (real)math::Min(y0, y1) + 0.5_r;
        const real max_sy = (real)math::Max(y0, y1) + 0.5_r;

        const real min_depth = math::Min(v0.viewport_coord.z, v1.viewport_coord.z);
        const real max_depth = math::Max(v0.viewport_coord.z, v1.viewport_coord.z);

        std::vector<float> smooth_min(v0.used_smooth_register_size);
        std::vector<float> smooth_max(v0.used_smooth_register_size);

        for (uint32_t i = 0; i < v0.used_smooth_register_size; ++i) {
            smooth_min[i] = math::Min(v0.smooth_register[i], v1.smooth_register[i]);
            smooth_max[i] = math::Max(v0.smooth_register[i], v1.smooth_register[i]);
        }

        const real eps = math::EPSILON_RASTERIZATION;

        for (const auto& f : out) {
            if (f.screen_coord.x < min_sx - eps || f.screen_coord.x > max_sx + eps) return false;
            if (f.screen_coord.y < min_sy - eps || f.screen_coord.y > max_sy + eps) return false;
            if (f.depth < min_depth - eps || f.depth > max_depth + eps) return false;

            if (f.used_smooth_register_size != v0.used_smooth_register_size) return false;
            for (uint32_t i = 0; i < f.used_smooth_register_size; ++i) {
                float val = f.smooth_register[i];
                float mn = smooth_min[i];
                float mx = smooth_max[i];

                if (val < mn - eps || val > mx + eps) return false;
            }

            if (f.used_flat_register_size != v1.used_flat_register_size) return false;

            if (!math::IsEqualApprox(f.flat_register[0], v1.flat_register[0], eps)) return false;

            if (math::IsEqualApprox(f.flat_register[0], v0.flat_register[0], eps)) return false;
        }

        return true;
    }

    bool VirtualGPUTester::RasterizeLineDegenerated() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1;

        v0.viewport_coord = Vector3(12.0_r, 12.0_r, 0.5_r);
        v1.viewport_coord = Vector3(12.0_r, 12.0_r, 0.5_r);

        auto out = gpu.Rasterize(v0, v1);

        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeLineEarlyDepthFailed() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) return false;

        VirtualGPU::Varying v0, v1;
        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.9_r);
        v1.viewport_coord = Vector3(18.0_r, 10.0_r, 0.9_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_write_enabled = false;

        VirtualGPU::FrameBuffer fb;

        std::vector<uint8_t> mem(64 * 64 * 4);
        float old_depth = 0.1f;

        uint32_t old_depth_bits;
        std::memcpy(&old_depth_bits, &old_depth, 4);

        uint32_t* ptr = reinterpret_cast<uint32_t*>(mem.data());
        std::fill(ptr, ptr + (64 * 64), old_depth_bits);

        VirtualGPU::Attachment att;
        att.memory = &mem;
        att.format = VG_DEPTH_COMPONENT;
        att.component_type = VG_FLOAT;
        att.width = 64;
        att.height = 64;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        auto out = gpu.Rasterize(v0, v1);

        if (!out.empty()) return false;

        return true;
    }
    bool VirtualGPUTester::RasterizeLineEarlyStencilFailed() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.4_r);
        v1.viewport_coord = Vector3(18.0_r, 10.0_r, 0.4_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        gpu.state_.depth_test_enabled = false;

        gpu.state_.stencil_test_enabled = true;
        gpu.state_.stencil_func[0] = VG_NEVER;

        VirtualGPU::FrameBuffer fb;

        std::vector<uint8_t> mem(64 * 64 * 4, 0);

        VirtualGPU::Attachment att;
        att.memory = &mem;
        att.format = VG_DEPTH_STENCIL;
        att.component_type = VG_UNSIGNED_INT;
        att.width = 64;
        att.height = 64;
        att.offset = 0;

        fb.depth_stencil_attachment = att;
        gpu.bound_draw_frame_buffer_ = &fb;

        auto out = gpu.Rasterize(v0, v1);

        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeTriangleCCWFront() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1, v2;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(22.0_r, 45.0_r, 0.5_r);
        v2.viewport_coord = Vector3(50.0_r, 14.0_r, 0.8_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        const std::vector<float> v0_data = {-5.0f, 2.0f, 1.0f, 0.0f, 1.0f,  2.0f, 0.7071f, 0.0f, 0.7071f,
                                            1.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.8f,    0.1f, 10.f,
                                            20.f,  30.f, 40.f, 0.f,  255.f, 0.f,  100.f};
        const std::vector<float> v1_data = {8.0f, -3.0f, -4.0f, 3.0f,  -2.0f, 6.0f,  0.0f, 0.7071f, 0.7071f,
                                            0.0f, 1.0f,  0.0f,  1.0f,  1.0f,  0.2f,  0.2f, 0.9f,    200.f,
                                            10.f, 250.f, 220.f, 255.f, 0.f,   255.f, 200.f};
        const std::vector<float> v2_data = {1.0f,  6.0f, 3.0f,  -1.0f, 4.0f, 1.0f,  0.1961f, 0.098f, 0.9805f,
                                            0.0f,  0.0f, 1.0f,  1.0f,  0.3f, 1.0f,  0.5f,    0.4f,   100.f,
                                            150.f, 50.f, 180.f, 50.f,  50.f, 200.f, 150.f};

        v0.used_smooth_register_size = (uint32_t)v0_data.size();
        std::copy(v0_data.begin(), v0_data.end(), v0.smooth_register.begin());

        v1.used_smooth_register_size = (uint32_t)v1_data.size();
        std::copy(v1_data.begin(), v1_data.end(), v1.smooth_register.begin());

        v2.used_smooth_register_size = (uint32_t)v2_data.size();
        std::copy(v2_data.begin(), v2_data.end(), v2.smooth_register.begin());

        v0.used_flat_register_size = 1;
        v0.flat_register[0] = 100.0f;
        v1.used_flat_register_size = 1;
        v1.flat_register[0] = 200.0f;
        v2.used_flat_register_size = 1;
        v2.flat_register[0] = 300.0f;

        gpu.state_.front_face = VG_CCW;
        gpu.state_.cull_enabled = true;
        gpu.state_.cull_face = VG_BACK;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1, v2);
        if (out.size() < 20u) return false;

        const int x0 = (int)math::Floor(v0.viewport_coord.x);
        const int y0 = (int)math::Floor(v0.viewport_coord.y);
        const int x1 = (int)math::Floor(v1.viewport_coord.x);
        const int y1 = (int)math::Floor(v1.viewport_coord.y);
        const int x2 = (int)math::Floor(v2.viewport_coord.x);
        const int y2 = (int)math::Floor(v2.viewport_coord.y);

        const real min_sx = (real)(math::Min(x0, math::Min(x1, x2))) + 0.5_r;
        const real max_sx = (real)(math::Max(x0, math::Max(x1, x2))) + 0.5_r;
        const real min_sy = (real)(math::Min(y0, math::Min(y1, y2))) + 0.5_r;
        const real max_sy = (real)(math::Max(y0, math::Max(y1, y2))) + 0.5_r;

        const real min_depth = math::Min(v0.viewport_coord.z, math::Min(v1.viewport_coord.z, v2.viewport_coord.z));
        const real max_depth = math::Max(v0.viewport_coord.z, math::Max(v1.viewport_coord.z, v2.viewport_coord.z));

        std::vector<float> smooth_min(v0.used_smooth_register_size);
        std::vector<float> smooth_max(v0.used_smooth_register_size);

        for (uint32_t i = 0; i < v0.used_smooth_register_size; ++i) {
            smooth_min[i] = math::Min(v0.smooth_register[i], math::Min(v1.smooth_register[i], v2.smooth_register[i]));
            smooth_max[i] = math::Max(v0.smooth_register[i], math::Max(v1.smooth_register[i], v2.smooth_register[i]));
        }

        const real eps = math::EPSILON_RASTERIZATION;

        for (const auto& f : out) {
            if (f.screen_coord.x < min_sx - eps || f.screen_coord.x > max_sx + eps) return false;
            if (f.screen_coord.y < min_sy - eps || f.screen_coord.y > max_sy + eps) return false;
            if (f.depth < min_depth - eps || f.depth > max_depth + eps) return false;

            if (f.used_smooth_register_size != v0.used_smooth_register_size) return false;
            for (uint32_t i = 0; i < f.used_smooth_register_size; ++i) {
                float val = f.smooth_register[i];
                if (val < smooth_min[i] - eps || val > smooth_max[i] + eps) return false;
            }

            if (f.used_flat_register_size != v2.used_flat_register_size) return false;

            if (!math::IsEqualApprox(f.flat_register[0], v2.flat_register[0], eps)) return false;

            if (math::IsEqualApprox(f.flat_register[0], v0.flat_register[0], eps)) return false;
            if (math::IsEqualApprox(f.flat_register[0], v1.flat_register[0], eps)) return false;

            if (!f.is_front) return false;
        }

        return true;
    }
    bool VirtualGPUTester::RasterizeTriangleCCWBack() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1, v2;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(50.0_r, 14.0_r, 0.8_r);
        v2.viewport_coord = Vector3(22.0_r, 45.0_r, 0.5_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        gpu.state_.front_face = VG_CCW;
        gpu.state_.cull_enabled = true;
        gpu.state_.cull_face = VG_BACK;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1, v2);
        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeTriangleCCWFrontAndBack() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.front_face = VG_CCW;
        gpu.state_.cull_enabled = true;
        gpu.state_.cull_face = VG_FRONT_AND_BACK;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        VirtualGPU::Varying v0, v1, v2;
        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(20.0_r, 35.0_r, 0.5_r);
        v2.viewport_coord = Vector3(40.0_r, 12.0_r, 0.8_r);

        auto out_front = gpu.Rasterize(v0, v1, v2);
        if (!out_front.empty()) return false;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(50.0_r, 14.0_r, 0.8_r);
        v2.viewport_coord = Vector3(22.0_r, 45.0_r, 0.5_r);

        auto out_back = gpu.Rasterize(v0, v1, v2);
        if (!out_back.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeTriangleCWFront() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }
        VirtualGPU::Varying v0, v1, v2;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(50.0_r, 14.0_r, 0.8_r);
        v2.viewport_coord = Vector3(22.0_r, 45.0_r, 0.5_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        const std::vector<float> v0_data = {-5.0f, 2.0f, 1.0f, 0.0f, 1.0f,  2.0f, 0.7071f, 0.0f, 0.7071f,
                                            1.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.8f,    0.1f, 10.f,
                                            20.f,  30.f, 40.f, 0.f,  255.f, 0.f,  100.f};
        const std::vector<float> v1_data = {1.0f,  6.0f, 3.0f,  -1.0f, 4.0f, 1.0f,  0.0f, 0.7071f, 0.7071f,
                                            0.0f,  1.0f, 0.0f,  1.0f,  0.3f, 1.0f,  0.5f, 0.4f,    100.f,
                                            150.f, 50.f, 180.f, 50.f,  50.f, 200.f, 150.f};
        const std::vector<float> v2_data = {8.0f, -3.0f, -4.0f, 3.0f,  -2.0f, 6.0f,  0.0f, 0.0f, 1.0f,
                                            0.0f, 0.0f,  1.0f,  1.0f,  1.0f,  0.2f,  0.2f, 0.9f, 200.f,
                                            10.f, 250.f, 220.f, 255.f, 0.f,   255.f, 200.f};

        v0.used_smooth_register_size = (uint32_t)v0_data.size();
        std::copy(v0_data.begin(), v0_data.end(), v0.smooth_register.begin());

        v1.used_smooth_register_size = (uint32_t)v1_data.size();
        std::copy(v1_data.begin(), v1_data.end(), v1.smooth_register.begin());

        v2.used_smooth_register_size = (uint32_t)v2_data.size();
        std::copy(v2_data.begin(), v2_data.end(), v2.smooth_register.begin());

        v0.used_flat_register_size = 1;
        v0.flat_register[0] = 100.0f;
        v1.used_flat_register_size = 1;
        v1.flat_register[0] = 200.0f;
        v2.used_flat_register_size = 1;
        v2.flat_register[0] = 300.0f;

        gpu.state_.front_face = VG_CW;
        gpu.state_.cull_enabled = true;
        gpu.state_.cull_face = VG_BACK;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1, v2);
        if (out.size() < 20u) return false;

        const int x0 = (int)math::Floor(v0.viewport_coord.x);
        const int y0 = (int)math::Floor(v0.viewport_coord.y);
        const int x1 = (int)math::Floor(v1.viewport_coord.x);
        const int y1 = (int)math::Floor(v1.viewport_coord.y);
        const int x2 = (int)math::Floor(v2.viewport_coord.x);
        const int y2 = (int)math::Floor(v2.viewport_coord.y);

        const real min_sx = (real)(math::Min(x0, math::Min(x1, x2))) + 0.5_r;
        const real max_sx = (real)(math::Max(x0, math::Max(x1, x2))) + 0.5_r;
        const real min_sy = (real)(math::Min(y0, math::Min(y1, y2))) + 0.5_r;
        const real max_sy = (real)(math::Max(y0, math::Max(y1, y2))) + 0.5_r;

        const real min_depth = math::Min(v0.viewport_coord.z, math::Min(v1.viewport_coord.z, v2.viewport_coord.z));
        const real max_depth = math::Max(v0.viewport_coord.z, math::Max(v1.viewport_coord.z, v2.viewport_coord.z));

        std::vector<float> smooth_min(v0.used_smooth_register_size);
        std::vector<float> smooth_max(v0.used_smooth_register_size);

        for (uint32_t i = 0; i < v0.used_smooth_register_size; ++i) {
            smooth_min[i] = math::Min(v0.smooth_register[i], math::Min(v1.smooth_register[i], v2.smooth_register[i]));
            smooth_max[i] = math::Max(v0.smooth_register[i], math::Max(v1.smooth_register[i], v2.smooth_register[i]));
        }

        const real eps = math::EPSILON_RASTERIZATION;

        for (const auto& f : out) {
            if (f.screen_coord.x < min_sx - eps || f.screen_coord.x > max_sx + eps) return false;
            if (f.screen_coord.y < min_sy - eps || f.screen_coord.y > max_sy + eps) return false;
            if (f.depth < min_depth - eps || f.depth > max_depth + eps) return false;

            if (f.used_smooth_register_size != v0.used_smooth_register_size) return false;
            for (uint32_t i = 0; i < f.used_smooth_register_size; ++i) {
                float val = f.smooth_register[i];
                if (val < smooth_min[i] - eps || val > smooth_max[i] + eps) return false;
            }

            if (f.used_flat_register_size != v2.used_flat_register_size) return false;

            if (!math::IsEqualApprox(f.flat_register[0], v2.flat_register[0], eps)) return false;

            if (math::IsEqualApprox(f.flat_register[0], v0.flat_register[0], eps)) return false;
            if (math::IsEqualApprox(f.flat_register[0], v1.flat_register[0], eps)) return false;

            if (!f.is_front) return false;
        }

        return true;
    }

    bool VirtualGPUTester::RasterizeTriangleCWBack() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1, v2;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(40.0_r, 12.0_r, 0.8_r);
        v2.viewport_coord = Vector3(20.0_r, 35.0_r, 0.5_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        gpu.state_.front_face = VG_CCW;
        gpu.state_.cull_enabled = true;
        gpu.state_.cull_face = VG_BACK;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1, v2);
        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeTriangleCWFrontAndBack() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        gpu.state_.front_face = VG_CW;
        gpu.state_.cull_enabled = true;
        gpu.state_.cull_face = VG_FRONT_AND_BACK;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        VirtualGPU::Varying v0, v1, v2;
        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(50.0_r, 14.0_r, 0.8_r);
        v2.viewport_coord = Vector3(22.0_r, 45.0_r, 0.5_r);

        auto out_front = gpu.Rasterize(v0, v1, v2);
        if (!out_front.empty()) return false;

        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.2_r);
        v1.viewport_coord = Vector3(20.0_r, 35.0_r, 0.5_r);
        v2.viewport_coord = Vector3(40.0_r, 12.0_r, 0.8_r);

        auto out_back = gpu.Rasterize(v0, v1, v2);
        if (!out_back.empty()) return false;

        return true;
    }
    bool VirtualGPUTester::RasterizeTriangleDegeneratedInPoint() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1, v2;

        v0.viewport_coord = Vector3(20.0_r, 20.0_r, 0.5_r);
        v1.viewport_coord = Vector3(20.0_r, 20.0_r, 0.5_r);
        v2.viewport_coord = Vector3(20.0_r, 20.0_r, 0.5_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        gpu.state_.cull_enabled = false;
        gpu.state_.depth_test_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        auto out = gpu.Rasterize(v0, v1, v2);
        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeTriangleEarlyDepthFailed() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1, v2;
        v0.viewport_coord = Vector3(10.0_r, 10.0_r, 0.8_r);
        v1.viewport_coord = Vector3(40.0_r, 12.0_r, 0.8_r);
        v2.viewport_coord = Vector3(20.0_r, 35.0_r, 0.8_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        gpu.state_.front_face = VG_CCW;
        gpu.state_.cull_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_func = VG_LESS;
        gpu.state_.depth_write_enabled = false;

        gpu.ClearDepthAttachment(0.1_r);

        auto out = gpu.Rasterize(v0, v1, v2);
        if (!out.empty()) return false;

        return true;
    }

    bool VirtualGPUTester::RasterizeTriangleEarlyStencilFailed() {
        VirtualGPU& gpu = VirtualGPU::GetInstance();
        if (!InitFreshGPU()) {
            return false;
        }

        VirtualGPU::Varying v0, v1, v2;
        v0.viewport_coord = Vector3(15.0_r, 15.0_r, 0.3_r);
        v1.viewport_coord = Vector3(45.0_r, 18.0_r, 0.3_r);
        v2.viewport_coord = Vector3(25.0_r, 40.0_r, 0.3_r);

        v0.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v1.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);
        v2.vg_Position = Vector4(0.0_r, 0.0_r, 0.0_r, 1.0_r);

        gpu.state_.front_face = VG_CCW;
        gpu.state_.cull_enabled = false;
        gpu.state_.scissor_test_enabled = false;

        gpu.state_.depth_test_enabled = true;
        gpu.state_.depth_func = VG_ALWAYS;
        gpu.state_.depth_write_enabled = false;

        gpu.state_.stencil_test_enabled = true;
        gpu.state_.stencil_func[0] = VG_NEVER;
        gpu.state_.stencil_func[1] = VG_NEVER;
        gpu.state_.stencil_ref[0] = 1;
        gpu.state_.stencil_write_mask[0] = 0xFF;

        auto out = gpu.Rasterize(v0, v1, v2);
        if (!out.empty()) return false;

        return true;
    }

}  // namespace ho