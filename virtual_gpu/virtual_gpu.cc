#include "virtual_gpu.h"

#include <cstdlib>

#include "core/math/interp_funcs.h"
#include "core/math/math_funcs.h"
#include "virtual_gpu_utils.h"

namespace ho {
    bool VirtualGPU::Initialize(uint8_t* color_buffer, int width, int height, VGenum color_format,
                                VGenum component_type) {
        if (color_buffer == nullptr || width <= 0 || width > MAX_ATTACHMENT_WIDTH || height <= 0 ||
            height > MAX_ATTACHMENT_HEIGHT) {
            return false;
        }

        if (!vg::IsColorFormat(color_format) || vg::GetTypeSize(component_type) == 0) {
            return false;
        }

        // Clear states
        vram_.clear();

        base_id_ = 0;
        vertex_array_pool_.clear();
        buffer_pool_.clear();
        texture_pool_.clear();
        sampler_pool_.clear();
        frame_buffer_pool_.clear();
        render_buffer_pool_.clear();
        shader_pool_.clear();
        program_pool_.clear();

        bound_vertex_array_ = nullptr;
        using_program_ = nullptr;
        active_texture_unit_ = 0;
        bound_buffer_targets_.fill(nullptr);
        bound_draw_frame_buffer_ = nullptr;
        bound_read_frame_buffer_ = nullptr;
        bound_render_buffer_ = nullptr;
        texture_units_.fill(TextureUnit());
        proxy_texture_states_.fill(ProxyTextureState());
        uniform_buffer_bindings_.clear();
        transform_feedback_buffer_bindings_.clear();
        constant_attributes_.clear();

        state_.clear_color = Color128(0.f, 0.f, 0.f, 0.f);
        state_.viewport = {0, 0, width, height};
        state_.scissor_test_enabled = false;
        state_.scissor = {0, 0, width, height};
        state_.depth_test_enabled = false;
        state_.depth_write_enabled = true;
        state_.clear_depth = 1.f;
        state_.min_depth = 0.f;
        state_.max_depth = 1.f;
        state_.depth_factor = 0.f;
        state_.depth_unit = 0.f;
        state_.depth_func = VG_LESS;

        state_.stencil_test_enabled = false;
        state_.clear_stencil = 0;
        state_.stencil_func[0] = VG_ALWAYS;
        state_.stencil_func[1] = VG_ALWAYS;
        state_.stencil_ref[0] = 0;
        state_.stencil_ref[1] = 0;
        state_.stencil_func_mask[0] = 0xFFFFFFFF;
        state_.stencil_func_mask[1] = 0xFFFFFFFF;
        state_.stencil_write_mask[0] = 0xFFFFFFFF;
        state_.stencil_write_mask[1] = 0xFFFFFFFF;
        state_.stencil_sfail_op[0] = VG_KEEP;
        state_.stencil_sfail_op[1] = VG_KEEP;
        state_.stencil_dpfail_op[0] = VG_KEEP;
        state_.stencil_dpfail_op[1] = VG_KEEP;
        state_.stencil_dppass_op[0] = VG_KEEP;
        state_.stencil_dppass_op[1] = VG_KEEP;

        state_.blend_src_rgb_factor = VG_ONE;
        state_.blend_dst_rgb_factor = VG_ZERO;
        state_.blend_rgb_equation = VG_FUNC_ADD;
        state_.blend_src_alpha_factor = VG_ONE;
        state_.blend_dst_alpha_factor = VG_ZERO;
        state_.blend_alpha_equation = VG_FUNC_ADD;
        state_.blend_constant = Color128(0.f, 0.f, 0.f, 0.f);

        state_.draw_buffer_states.fill(DrawBufferState());

        state_.cull_enabled = false;
        state_.cull_face = VG_BACK;
        state_.front_face = VG_CCW;

        state_.point_offset_enabled = false;
        state_.line_offset_enabled = false;
        state_.polygon_offset_enabled = false;

        state_.polygon_mode = VG_FILL;

        state_.error_state = VG_NO_ERROR;

        // Create Default Frame Buffer
        FrameBuffer default_fb;
        default_fb.id = 0;

        // create default color attachment
        default_fb.color_attachments[0].ref_id = 0;
        default_fb.color_attachments[0].external_memory = color_buffer;
        default_fb.color_attachments[0].width = width;
        default_fb.color_attachments[0].height = height;
        default_fb.color_attachments[0].format = color_format;
        default_fb.color_attachments[0].component_type = component_type;
        default_fb.color_attachments[0].offset = 0;

        // create default depth/stencil attachment
        vram_.emplace_back(std::vector<uint8_t>(static_cast<size_t>(width) * static_cast<size_t>(height) * 4));
        auto* mem = &vram_.back();
        default_fb.depth_stencil_attachment.ref_id = 0;
        default_fb.depth_stencil_attachment.memory = mem;
        default_fb.depth_stencil_attachment.width = width;
        default_fb.depth_stencil_attachment.height = height;
        default_fb.depth_stencil_attachment.format = VG_DEPTH_STENCIL;
        default_fb.depth_stencil_attachment.component_type = VG_UNSIGNED_INT;
        default_fb.depth_stencil_attachment.offset = 0;

        default_fb.draw_slot_to_color_attachment.fill(INVALID_SLOT);

        // bind draw slot 0 -> color attachment 0
        default_fb.draw_slot_to_color_attachment[0] = 0;
        // bind read slot 0 -> color attachment 0
        default_fb.read_slot_to_color_attachment = 0;

        default_fb.is_bound_once = true;

        frame_buffer_pool_.insert({0, default_fb});
        bound_draw_frame_buffer_ = &frame_buffer_pool_[0];
        bound_read_frame_buffer_ = &frame_buffer_pool_[0];

        return true;
    }

    VirtualGPU::VirtualGPU() : job_system_(WORKER_COUNT) {}

    void VirtualGPU::ClearColorAttachment(size_t slot, const Color128& clear_color) {
        FrameBuffer* fb = bound_draw_frame_buffer_;

        if (!fb) {
            return;
        }

        if (fb->draw_slot_to_color_attachment[slot] == INVALID_SLOT) {
            return;
        }

        Attachment& attch = fb->color_attachments[fb->draw_slot_to_color_attachment[slot]];

        const int w = attch.width;
        const int h = attch.height;

        // Calculate viewport, scissor intersection

        // viewport
        const int vx0 = math::Clamp(state_.viewport.x, 0, w);
        const int vy0 = math::Clamp(state_.viewport.y, 0, h);
        const int vx1 = math::Clamp(state_.viewport.x + state_.viewport.width, 0, w);
        const int vy1 = math::Clamp(state_.viewport.y + state_.viewport.height, 0, h);

        int x0 = vx0, y0 = vy0, x1 = vx1, y1 = vy1;

        // scissor
        if (state_.scissor_test_enabled) {
            const int sx0 = math::Clamp(state_.scissor.x, 0, w);
            const int sy0 = math::Clamp(state_.scissor.y, 0, h);
            const int sx1 = math::Clamp(state_.scissor.x + state_.scissor.width, 0, w);
            const int sy1 = math::Clamp(state_.scissor.y + state_.scissor.height, 0, h);
            x0 = math::Max(x0, sx0);
            y0 = math::Max(y0, sy0);
            x1 = math::Min(x1, sx1);
            y1 = math::Min(y1, sy1);
        }

        if (x0 >= x1 || y0 >= y1) {
            return;
        }

        const int pixel_size = vg::GetPixelSize(attch.format, attch.component_type);
        uint8_t* base = attch.external_memory != nullptr ? attch.external_memory : attch.memory->data();
        base += attch.offset;

        for (int y = y0; y < y1; y++) {
            uint8_t* row = base + static_cast<size_t>((y * attch.width + x0) * pixel_size);
            for (int x = x0; x < x1; x++) {
                vg::CopyPixel(row, reinterpret_cast<const uint8_t*>(&clear_color), attch.format, attch.component_type,
                              VG_RGBA, VG_FLOAT, state_.draw_buffer_states[slot].color_mask);
                row += static_cast<size_t>(pixel_size);
            }
        }
    }

    void VirtualGPU::ClearDepthAttachment(real clear_depth) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        FrameBuffer* fb = vg.bound_draw_frame_buffer_;

        if (!fb) {
            return;
        }

        Attachment& attch = fb->depth_stencil_attachment;

        const int w = (int)attch.width;
        const int h = (int)attch.height;

        // Calculate viewport, scissor intersection

        // viewport
        const int vx0 = math::Clamp(state_.viewport.x, 0, w);
        const int vy0 = math::Clamp(state_.viewport.y, 0, h);
        const int vx1 = math::Clamp(state_.viewport.x + state_.viewport.width, 0, w);
        const int vy1 = math::Clamp(state_.viewport.y + state_.viewport.height, 0, h);

        int x0 = vx0, y0 = vy0, x1 = vx1, y1 = vy1;

        // scissor
        if (state_.scissor_test_enabled) {
            const int sx0 = math::Clamp(state_.scissor.x, 0, w);
            const int sy0 = math::Clamp(state_.scissor.y, 0, h);
            const int sx1 = math::Clamp(state_.scissor.x + state_.scissor.width, 0, w);
            const int sy1 = math::Clamp(state_.scissor.y + state_.scissor.height, 0, h);
            x0 = math::Max(x0, sx0);
            y0 = math::Max(y0, sy0);
            x1 = math::Min(x1, sx1);
            y1 = math::Min(y1, sy1);
        }

        if (x0 >= x1 || y0 >= y1) {
            return;
        }

        if (attch.format == VG_DEPTH_STENCIL) {
            ClearDepthStencilAttachment(true, false, clear_depth, 0);
        } else {
            int pixel_size = vg::GetPixelSize(attch.format, attch.component_type);
            uint8_t* base = attch.external_memory != nullptr ? attch.external_memory : attch.memory->data();
            base += attch.offset;

            for (int y = y0; y < y1; y++) {
                uint8_t* row = base + static_cast<size_t>((y * attch.width + x0) * pixel_size);
                for (int x = x0; x < x1; x++) {
                    float fdepth = static_cast<float>(clear_depth);
                    vg::CopyPixel(row, reinterpret_cast<const uint8_t*>(&fdepth), attch.format, attch.component_type,
                                  VG_DEPTH_COMPONENT, VG_FLOAT);
                    row += pixel_size;
                }
            }
        }
    }

    void VirtualGPU::ClearDepthStencilAttachment(bool is_depth_cleared, bool is_stencil_cleared, real clear_depth,
                                                 uint8_t clear_stencil) {
        FrameBuffer* fb = bound_draw_frame_buffer_;

        if (!fb) {
            return;
        }

        Attachment& attch = fb->depth_stencil_attachment;

        if (attch.format != VG_DEPTH_STENCIL) {
            return;
        }

        const int w = attch.width;
        const int h = attch.height;

        // Calculate viewport, scissor intersection

        // viewport
        const int vx0 = math::Clamp(state_.viewport.x, 0, w);
        const int vy0 = math::Clamp(state_.viewport.y, 0, h);
        const int vx1 = math::Clamp(state_.viewport.x + state_.viewport.width, 0, w);
        const int vy1 = math::Clamp(state_.viewport.y + state_.viewport.height, 0, h);

        int x0 = vx0, y0 = vy0, x1 = vx1, y1 = vy1;

        // scissor
        if (state_.scissor_test_enabled) {
            const int sx0 = math::Clamp(state_.scissor.x, 0, w);
            const int sy0 = math::Clamp(state_.scissor.y, 0, h);
            const int sx1 = math::Clamp(state_.scissor.x + state_.scissor.width, 0, w);
            const int sy1 = math::Clamp(state_.scissor.y + state_.scissor.height, 0, h);
            x0 = math::Max(x0, sx0);
            y0 = math::Max(y0, sy0);
            x1 = math::Min(x1, sx1);
            y1 = math::Min(y1, sy1);
        }

        if (x0 >= x1 || y0 >= y1) {
            return;
        }

        uint8_t* base = attch.external_memory != nullptr ? attch.external_memory : attch.memory->data();
        base += attch.offset;

        const uint8_t stencil_mask = static_cast<uint8_t>(state_.stencil_write_mask[0]);

        for (int y = y0; y < y1; y++) {
            uint8_t* row = base + static_cast<size_t>((y * attch.width + x0) * 4);
            for (int x = x0; x < x1; x++) {
                real depth;
                uint8_t stencil;

                vg::DecodeDepthStencil(&depth, &stencil, row);

                if (is_depth_cleared) {
                    depth = clear_depth;
                }

                if (is_stencil_cleared) {
                    stencil = (clear_stencil & ~stencil_mask) | (clear_stencil & stencil_mask);
                }

                vg::EncodeDepthStencil(row, depth, stencil);

                row += 4;
            }
        }
    }

    void VirtualGPU::VSJobEntry(void* input, int size) {
        assert(size == sizeof(VSJobInput));
        (void)size;
        VSJobInput* in = static_cast<VSJobInput*>(input);
        VirtualGPU& vg = VirtualGPU::GetInstance();
        for (size_t i = in->first_index; i <= in->last_index; i++) {
            in->vs(i, vg.using_program_->varying_buffer[i - in->base_index]);
        }
        delete in;
    }

    std::vector<VirtualGPU::Varying> VirtualGPU::Clip(const std::vector<Varying>& polygon) const {
        std::vector<Varying> out_polygon = polygon;
        for (PlanePos plane_pos : {VG_PLANE_POS_LEFT, VG_PLANE_POS_RIGHT, VG_PLANE_POS_BOTTOM, VG_PLANE_POS_TOP,
                                   VG_PLANE_POS_NEAR, VG_PLANE_POS_FAR}) {
            out_polygon = ClipAgainstPlane(out_polygon, plane_pos);
            if (out_polygon.empty()) {
                // When polygon is totally out of frustum.
                break;
            }
        }
        return out_polygon;
    }

    std::vector<VirtualGPU::Varying> VirtualGPU::ClipAgainstPlane(const std::vector<Varying>& polygon,
                                                                  PlanePos plane_pos) const {
        // Sutherland-Hodgman Polygon Clipping Loop (Vertices always be treated as polygon)
        std::vector<Varying> out_polygon;
        if (polygon.empty()) {
            return out_polygon;
        }

        const size_t v_count = polygon.size();

        for (size_t i = 0; i < v_count; i++) {
            const Varying& prev_v = polygon[(i - 1 + v_count) % v_count];
            const Varying& curr_v = polygon[i];

            const bool is_prev_in = IsInside(prev_v.vg_Position, plane_pos);
            const bool is_curr_in = IsInside(curr_v.vg_Position, plane_pos);

            if (is_prev_in && is_curr_in) {
                // Case 1: in to in
                out_polygon.emplace_back(curr_v);
            } else if (is_prev_in && !is_curr_in) {
                // Case 2: in to out
                Vector2 bary = GetClipBarycentric(prev_v.vg_Position, curr_v.vg_Position, plane_pos);
                // if clip isn't parallel on plane or degenerated push interpolated
                // varying.
                if (!math::IsNaN(bary.x) && !math::IsNaN(bary.y)) {
                    out_polygon.emplace_back(LerpVarying(prev_v, curr_v, bary));
                }
            } else if (!is_prev_in && is_curr_in) {
                // Case 3: out to in
                Vector2 bary = GetClipBarycentric(prev_v.vg_Position, curr_v.vg_Position, plane_pos);
                // if clip isn't parallel on plane or degenerated push interpolated
                // varying.
                if (!math::IsNaN(bary.x) && !math::IsNaN(bary.y)) {
                    out_polygon.emplace_back(LerpVarying(prev_v, curr_v, bary));
                }
                out_polygon.emplace_back(curr_v);
            } else {
                // Case 4: out to out
                // Do Nothing.
            }
        }

        return out_polygon;
    }

    real VirtualGPU::EvalFrustumPlane(const Vector4& clip_coord, PlanePos plane_pos) const {
        switch (plane_pos) {
            case VG_PLANE_POS_LEFT:
                return clip_coord.w + clip_coord.x;
            case VG_PLANE_POS_RIGHT:
                return clip_coord.w - clip_coord.x;
            case VG_PLANE_POS_BOTTOM:
                return clip_coord.w + clip_coord.y;
            case VG_PLANE_POS_TOP:
                return clip_coord.w - clip_coord.y;
            case VG_PLANE_POS_NEAR:
                return clip_coord.w + clip_coord.z;
            case VG_PLANE_POS_FAR:
                return clip_coord.w - clip_coord.z;
            case VG_PLANE_POS_PROJECTION:
                return clip_coord.w;
            default:
                return math::REAL_NaN;
        }
    }

    bool VirtualGPU::IsInside(const Vector4& clip_coord, PlanePos plane_pos) const {
        return EvalFrustumPlane(clip_coord, plane_pos) > -math::EPSILON_POINT_ON_PLANE;
    }

    Vector2 VirtualGPU::GetClipBarycentric(const Vector4& clip_coord1, const Vector4& clip_coord2,
                                           PlanePos plane_pos) const {
        const real e1 = EvalFrustumPlane(clip_coord1, plane_pos);
        const real e2 = EvalFrustumPlane(clip_coord2, plane_pos);
        const real denom = e1 - e2;
        if (math::IsZeroApprox(denom)) {
            return Vector2(math::REAL_NaN, math::REAL_NaN);
        }

        real s = e1 / denom;
        s = math::Clamp(s, 0.0_r, 1.0_r);
        return Vector2(1.0_r - s, s);
    }

    VirtualGPU::Varying VirtualGPU::LerpVarying(const Varying& v1, const Varying& v2,
                                                const Vector2& barycentric) const {
        assert(v1.used_smooth_register_size == v2.used_smooth_register_size);
        assert(v1.used_flat_register_size == v2.used_flat_register_size);
        Varying v;
        v.vg_Position = math::Lerp(v1.vg_Position, v2.vg_Position, barycentric);
        v.used_smooth_register_size = v2.used_smooth_register_size;
        for (size_t i = 0; i < static_cast<size_t>(v.used_smooth_register_size); i++) {
            v.smooth_register[i] = math::Lerp(v1.smooth_register[i], v2.smooth_register[i], barycentric);
        }
        v.used_flat_register_size = v2.used_flat_register_size;
        std::copy_n(v2.flat_register.begin(), v2.used_flat_register_size, v.flat_register.begin());
        return v;
    }

    void VirtualGPU::PerspectiveDivide(Varying& v) const { v.viewport_coord = v.vg_Position.ToCartesian(); }
    void VirtualGPU::ViewportTransform(Varying& v) const {
        const Rect& vp = state_.viewport;

        const real half_width = static_cast<real>(vp.width) * 0.5_r;
        const real x = ((v.viewport_coord.x * half_width) + half_width + static_cast<real>(vp.x));

        const real half_height = static_cast<real>(vp.height) * 0.5_r;
        const real y = -(v.viewport_coord.y * half_height) + half_height + static_cast<real>(vp.y);

        const real z = (v.viewport_coord.z * static_cast<real>(state_.max_depth - state_.min_depth) +
                        static_cast<real>(state_.max_depth + state_.min_depth)) *
                       0.5_r;

        v.viewport_coord = Vector3(x, y, z);
    }

    std::vector<VirtualGPU::Fragment> VirtualGPU::Rasterize(const Varying& v) {
        std::vector<Fragment> out;
        Fragment frag;
        frag.screen_coord = Vector2(math::Floor(v.viewport_coord.x) + 0.5_r, math::Floor(v.viewport_coord.y) + 0.5_r);
        const uint64_t depth_bit =
            bound_draw_frame_buffer_->depth_stencil_attachment.format == VG_DEPTH_COMPONENT ? 32u : 24u;
        frag.depth = ApplyDepthOffset(v.viewport_coord.z, 0.f, depth_bit, state_.polygon_mode);

        if (ScissorTest(frag.screen_coord.x, frag.screen_coord.y) &&
            TestDepthStencil(frag.screen_coord.x, frag.screen_coord.y, frag.depth, true, true)) {
            frag.used_smooth_register_size = v.used_smooth_register_size;
            std::copy_n(v.smooth_register.begin(), v.used_smooth_register_size, frag.smooth_register.begin());
            frag.used_flat_register_size = v.used_flat_register_size;
            std::copy_n(v.flat_register.begin(), v.used_flat_register_size, frag.flat_register.begin());
            out.emplace_back(frag);
        }

        return out;
    }

    std::vector<VirtualGPU::Fragment> VirtualGPU::Rasterize(const Varying& v1, const Varying& v2) {
        assert(v1.used_smooth_register_size == v2.used_smooth_register_size);
        assert(v1.used_flat_register_size == v2.used_flat_register_size);

        std::vector<Fragment> out;

        const int x0 = (int)math::Floor(v1.viewport_coord.x);
        const int y0 = (int)math::Floor(v1.viewport_coord.y);
        const int x1 = (int)math::Floor(v2.viewport_coord.x);
        const int y1 = (int)math::Floor(v2.viewport_coord.y);

        const int dx = math::Abs(x1 - x0);
        const int dy = math::Abs(y1 - y0);
        const int sx = (x0 < x1) ? 1 : -1;
        const int sy = (y0 < y1) ? 1 : -1;

        int err = dx - dy;

        if (dx == 0 && dy == 0) {
            return out;
        }

        out.reserve(static_cast<size_t>(dx) + static_cast<size_t>(dy) + 1);

        // Calculate gradient
        const real Dx = static_cast<real>(x1 - x0);
        const real Dy = static_cast<real>(y1 - y0);
        const real dd = Dx * Dx + Dy * Dy;

        const real gx = Dx / dd;
        const real gy = Dy / dd;

        // Set component for incremental perspective correct interpolation
        const real inv_w1 = 1.0_r / v1.vg_Position.w;
        const real inv_w2 = 1.0_r / v2.vg_Position.w;

        real inv_w = inv_w1;
        const real inv_w_dx = (inv_w2 - inv_w1) * gx * static_cast<real>(sx);
        const real inv_w_dy = (inv_w2 - inv_w1) * gy * static_cast<real>(sy);

        const uint64_t depth_bit =
            bound_draw_frame_buffer_->depth_stencil_attachment.format == VG_DEPTH_COMPONENT ? 32u : 24u;
        const double offset_depth_v1 =
            static_cast<double>(ApplyDepthOffset(v1.viewport_coord.z, 0.0_r, depth_bit, state_.polygon_mode));
        const double offset_depth_v2 =
            static_cast<double>(ApplyDepthOffset(v2.viewport_coord.z, 0.0_r, depth_bit, state_.polygon_mode));
        double depth = offset_depth_v1;
        const double depth_dx = (offset_depth_v2 - depth) * gx * static_cast<double>(sx);
        const double depth_dy = (offset_depth_v2 - depth) * gy * static_cast<double>(sy);

        float smooth_register_pw[SMOOTH_REGISTER_SIZE];
        float smooth_register_pw_dx[SMOOTH_REGISTER_SIZE];
        float smooth_register_pw_dy[SMOOTH_REGISTER_SIZE];

        for (size_t i = 0; i < static_cast<size_t>(v1.used_smooth_register_size); i++) {
            smooth_register_pw[i] = v1.smooth_register[i] * inv_w1;
            smooth_register_pw_dx[i] =
                (v2.smooth_register[i] * inv_w2 - smooth_register_pw[i]) * gx * static_cast<real>(sx);
            smooth_register_pw_dy[i] =
                (v2.smooth_register[i] * inv_w2 - smooth_register_pw[i]) * gy * static_cast<real>(sy);
        }

        // Bresenham loop
        int x = x0;
        int y = y0;

        while (true) {
            Vector2 screen_coord(static_cast<real>(x) + 0.5_r, static_cast<real>(y) + 0.5_r);
            real w = 1.0_r / inv_w;

            if (ScissorTest(screen_coord.x, screen_coord.y) &&
                TestDepthStencil(screen_coord.x, screen_coord.y, static_cast<real>(depth), true, true)) {
                Fragment frag;
                frag.screen_coord = screen_coord;
                frag.depth = static_cast<real>(depth);

                frag.used_smooth_register_size = v1.used_smooth_register_size;
                for (int i = 0; i < frag.used_smooth_register_size; i++) {
                    frag.smooth_register[i] = smooth_register_pw[i] * w;  // NOLINT
                }

                frag.used_flat_register_size = v2.used_flat_register_size;
                std::copy_n(v2.flat_register.begin(), frag.used_flat_register_size, frag.flat_register.begin());

                out.emplace_back(frag);
            }

            if (x == x1 && y == y1) {
                break;
            }

            int e2 = 2 * err;
            bool step_x = false;
            bool step_y = false;

            if (e2 > -dy) {
                err -= dy;
                x += sx;
                step_x = true;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
                step_y = true;
            }

            if (step_x) {
                inv_w += inv_w_dx;
                depth += depth_dx;
                for (int i = 0; i < v1.used_smooth_register_size; i++) {
                    smooth_register_pw[i] += smooth_register_pw_dx[i];
                }
            }
            if (step_y) {
                inv_w += inv_w_dy;
                depth += depth_dy;
                for (int i = 0; i < v1.used_smooth_register_size; i++) {
                    smooth_register_pw[i] += smooth_register_pw_dy[i];
                }
            }
        }
        return out;
    }

    std::vector<VirtualGPU::Fragment> VirtualGPU::Rasterize(const Varying& v1, const Varying& v2, const Varying& v3) {
        assert(v1.used_smooth_register_size == v2.used_smooth_register_size);
        assert(v1.used_flat_register_size == v2.used_flat_register_size);
        assert(v2.used_smooth_register_size == v3.used_smooth_register_size);
        assert(v2.used_flat_register_size == v3.used_flat_register_size);

        // Edge Function and Incremental Perspective Correct Interpolation
        std::vector<Fragment> out;

        const BoundingBox2D b =
            BoundingBox2D(Vector2(v1.viewport_coord), Vector2(v2.viewport_coord), Vector2(v3.viewport_coord));

        const Vector2 p0(math::Ceil(b.min.x - 0.5_r) + 0.5_r, math::Ceil(b.min.y - 0.5_r) + 0.5_r);

        const EdgeFunction ef12(Vector2(v1.viewport_coord), Vector2(v2.viewport_coord), p0);
        const bool ef12_is_topleft = (v2.viewport_coord.y < v1.viewport_coord.y) ||
                                     (math::IsEqualApprox(v2.viewport_coord.y, v1.viewport_coord.y) &&
                                      v2.viewport_coord.x > v1.viewport_coord.x);

        const EdgeFunction ef23(Vector2(v2.viewport_coord), Vector2(v3.viewport_coord), p0);
        const bool ef23_is_topleft = (v3.viewport_coord.y < v2.viewport_coord.y) ||
                                     (math::IsEqualApprox(v3.viewport_coord.y, v2.viewport_coord.y) &&
                                      v3.viewport_coord.x > v2.viewport_coord.x);

        const EdgeFunction ef31(Vector2(v3.viewport_coord), Vector2(v1.viewport_coord), p0);
        const bool ef31_is_topleft = (v1.viewport_coord.y < v3.viewport_coord.y) ||
                                     (math::IsEqualApprox(v1.viewport_coord.y, v3.viewport_coord.y) &&
                                      v1.viewport_coord.x > v3.viewport_coord.x);

        const real area = (v2.viewport_coord.x - v1.viewport_coord.x) * (v3.viewport_coord.y - v1.viewport_coord.y) -
                          (v2.viewport_coord.y - v1.viewport_coord.y) * (v3.viewport_coord.x - v1.viewport_coord.x);

        if (math::IsEqualApprox(area, math::EPSILON_RASTERIZATION)) {
            // degenerate case
            return out;
        }

        // Face culling: Since screen space is Y-down, CCW winding results in negative area.
        const bool is_front = (state_.front_face == VG_CCW) ? (area < 0) : (area > 0);
        if (state_.cull_enabled) {
            switch (state_.cull_face) {
                case VG_BACK:
                    if (!is_front) return out;
                    break;
                case VG_FRONT:
                    if (is_front) return out;
                    break;
                case VG_FRONT_AND_BACK:
                default:
                    return out;
            }
        }

        // The edge function is defined assuming CCW vertex order.
        // For a CCW triangle, the edge function is positive inside the triangle
        // and negative outside; for a CW triangle, the sign is inverted
        // (negative inside, positive outside).
        // The 'sign' value is used to unify the inside/outside test for both
        // CCW and CW triangles under a single condition.
        const real sign = area > 0.0_r ? 1.0_r : -1.0_r;

        // polygon offset slope
        const real depth_slope = ComputeDepthSlope(v1.viewport_coord, v2.viewport_coord, v3.viewport_coord);

        const real inv_area = 1.0_r / area;

        // Set component for incremental perspective correct interpolation
        const real inv_w1 = 1.0_r / v1.vg_Position.w;
        const real inv_w2 = 1.0_r / v2.vg_Position.w;
        const real inv_w3 = 1.0_r / v3.vg_Position.w;

        const uint64_t depth_bit =
            bound_draw_frame_buffer_->depth_stencil_attachment.format == VG_DEPTH_COMPONENT ? 32u : 24u;
        const double offset_depth_v1 =
            static_cast<double>(ApplyDepthOffset(v1.viewport_coord.z, depth_slope, depth_bit, state_.polygon_mode));
        const double offset_depth_v2 =
            static_cast<double>(ApplyDepthOffset(v2.viewport_coord.z, depth_slope, depth_bit, state_.polygon_mode));
        const double offset_depth_v3 =
            static_cast<double>(ApplyDepthOffset(v3.viewport_coord.z, depth_slope, depth_bit, state_.polygon_mode));

        float smooth_register_pw1[SMOOTH_REGISTER_SIZE];
        float smooth_register_pw2[SMOOTH_REGISTER_SIZE];
        float smooth_register_pw3[SMOOTH_REGISTER_SIZE];

        // Initial edge values at start pixel center of bounding box
        real f12_row = ef12.initial_value;
        real f23_row = ef23.initial_value;
        real f31_row = ef31.initial_value;

        // per-pixel x increments for edge functions are ef*.dx, per-row y increments
        // are ef*.dy
        const real inv_w_dx = (ef23.dx * inv_w1 + ef31.dx * inv_w2 + ef12.dx * inv_w3) * inv_area;
        const real inv_w_dy = (ef23.dy * inv_w1 + ef31.dy * inv_w2 + ef12.dy * inv_w3) * inv_area;

        const double depth_dx =
            static_cast<double>(ef23.dx * offset_depth_v1 + ef31.dx * offset_depth_v2 + ef12.dx * offset_depth_v3) *
            inv_area;
        const double depth_dy =
            static_cast<double>(ef23.dy * offset_depth_v1 + ef31.dy * offset_depth_v2 + ef12.dy * offset_depth_v3) *
            inv_area;

        float smooth_register_dx[SMOOTH_REGISTER_SIZE];
        float smooth_register_dy[SMOOTH_REGISTER_SIZE];

        // initial row accumulators for inv_w and attributes at p0
        real inv_w_row = (f23_row * inv_w1 + f31_row * inv_w2 + f12_row * inv_w3) * inv_area;
        const double depth_init =
            static_cast<double>(f23_row * offset_depth_v1 + f31_row * offset_depth_v2 + f12_row * offset_depth_v3) *
            inv_area;
        float smooth_register_row[SMOOTH_REGISTER_SIZE];

        for (size_t i = 0; i < static_cast<size_t>(v1.used_smooth_register_size); i++) {
            smooth_register_pw1[i] = v1.smooth_register[i] * inv_w1;
            smooth_register_pw2[i] = v2.smooth_register[i] * inv_w2;
            smooth_register_pw3[i] = v3.smooth_register[i] * inv_w3;
            smooth_register_dx[i] = (ef23.dx * smooth_register_pw1[i] + ef31.dx * smooth_register_pw2[i] +
                                     ef12.dx * smooth_register_pw3[i]) *
                                    inv_area;
            smooth_register_dy[i] = (ef23.dy * smooth_register_pw1[i] + ef31.dy * smooth_register_pw2[i] +
                                     ef12.dy * smooth_register_pw3[i]) *
                                    inv_area;
            smooth_register_row[i] = (f23_row * smooth_register_pw1[i] + f31_row * smooth_register_pw2[i] +
                                      f12_row * smooth_register_pw3[i]) *
                                     inv_area;
        }

        // min include, max exclude
        const int x_min = static_cast<int>(math::Ceil(b.min.x - 0.5_r));
        const int x_max = static_cast<int>(math::Floor(b.max.x - 0.5_r)) + 1;
        const int y_min = static_cast<int>(math::Ceil(b.min.y - 0.5_r));
        const int y_max = static_cast<int>(math::Floor(b.max.y - 0.5_r)) + 1;

        out.reserve((x_max - x_min) * (y_max - y_min) / 2);
        // Raster loop
        for (int y = y_min; y < y_max; ++y) {
            real f12_ev = f12_row;
            real f23_ev = f23_row;
            real f31_ev = f31_row;

            real inv_w = inv_w_row;
            const double dy_offset = static_cast<double>(y - y_min);
            double depth = depth_init + dy_offset * depth_dy;

            float smooth_register[SMOOTH_REGISTER_SIZE];
            std::memcpy(smooth_register, smooth_register_row,
                        static_cast<size_t>(v1.used_smooth_register_size) * sizeof(float));

            for (int x = x_min; x < x_max; ++x) {
                const bool inside12 =
                    (f12_ev * sign > 0) || (ef12_is_topleft && math::Abs(f12_ev) <= math::EPSILON_RASTERIZATION);
                const bool inside23 =
                    (f23_ev * sign > 0) || (ef23_is_topleft && math::Abs(f23_ev) <= math::EPSILON_RASTERIZATION);
                const bool inside31 =
                    (f31_ev * sign > 0) || (ef31_is_topleft && math::Abs(f31_ev) <= math::EPSILON_RASTERIZATION);

                if (inside12 && inside23 && inside31) {
                    const Vector2 target_coord(real(x) + 0.5_r, real(y) + 0.5_r);

                    const real w = 1.0_r / inv_w;

                    if (ScissorTest(target_coord.x, target_coord.y) &&
                        TestDepthStencil(target_coord.x, target_coord.y, static_cast<real>(depth), is_front, true)) {
                        Fragment frag;
                        frag.screen_coord = target_coord;
                        frag.depth = static_cast<real>(depth);

                        frag.used_smooth_register_size = v3.used_smooth_register_size;
                        for (size_t i = 0; i < static_cast<size_t>(frag.used_smooth_register_size); i++) {
                            frag.smooth_register[i] = smooth_register[i] * w;  // NOLINT
                        }

                        frag.used_flat_register_size = v3.used_flat_register_size;
                        std::copy_n(v3.flat_register.begin(), frag.used_flat_register_size, frag.flat_register.begin());

                        frag.is_front = is_front;
                        out.emplace_back(frag);
                    }
                }

                // +1 in x: advance edge and attributes
                f12_ev += ef12.dx;
                f23_ev += ef23.dx;
                f31_ev += ef31.dx;

                inv_w += inv_w_dx;
                depth += depth_dx;

                for (size_t i = 0; i < static_cast<size_t>(v3.used_smooth_register_size); i++) {
                    smooth_register[i] += smooth_register_dx[i];
                }
            }

            // +1 in y: advance row start edge and row start attributes
            f12_row += ef12.dy;
            f23_row += ef23.dy;
            f31_row += ef31.dy;

            inv_w_row += inv_w_dy;

            for (size_t i = 0; i < static_cast<size_t>(v3.used_smooth_register_size); i++) {
                smooth_register_row[i] += smooth_register_dy[i];
            }
        }

        return out;
    }

    bool VirtualGPU::ScissorTest(real x, real y) const {
        if (!state_.scissor_test_enabled) {
            return true;
        }
        const int px = static_cast<int>(math::Floor(x));
        const int py = static_cast<int>(math::Floor(y));
        const auto& vp = state_.viewport;
        if (px < vp.x || py < vp.y || px >= (vp.x + vp.width) || py >= (vp.y + vp.height)) {
            return false;
        }

        const Rect& r = state_.scissor;
        return (px >= r.x && px < (r.x + r.width) && py >= r.y && py < (r.y + r.height));
    }

    real VirtualGPU::ComputeDepthSlope(const Vector3& v0, const Vector3& v1, const Vector3& v2) const {
        const real denom = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);
        if (math::IsZeroApprox(denom)) {
            return 0.0_r;  // degenerate triangle
        }

        const real dzdx = ((v1.z - v0.z) * (v2.y - v0.y) - (v2.z - v0.z) * (v1.y - v0.y)) / denom;
        const real dzdy = ((v2.z - v0.z) * (v1.x - v0.x) - (v1.z - v0.z) * (v2.x - v0.x)) / denom;

        return math::Max(math::Abs(dzdx), math::Abs(dzdy));
    }

    real VirtualGPU::ApplyDepthOffset(real depth, real depth_slope, uint64_t depth_bits, VGenum primitive_mode) const {
        bool enabled = false;
        switch (primitive_mode) {
            case VG_FILL:
                enabled = state_.polygon_offset_enabled;
                break;
            case VG_LINE:
                enabled = state_.line_offset_enabled;
                break;
            case VG_POINT:
                enabled = state_.point_offset_enabled;
                break;
            default:
                // no op
                break;
        }

        if (!enabled) {
            return depth;
        }

        const double r = 1.0 / static_cast<double>(1ull << depth_bits);

        const double bias = static_cast<double>(depth_slope) * state_.depth_factor + r * state_.depth_unit;
        return static_cast<real>(math::Clamp(static_cast<double>(depth) + bias, state_.min_depth, state_.max_depth));
    }

    bool VirtualGPU::TestDepthStencil(real x, real y, real depth, bool is_front_face, bool compare_only) {
        FrameBuffer* fb = bound_draw_frame_buffer_;
        Attachment& attch = fb->depth_stencil_attachment;
        if (!attch.memory) {
            return true;
        }

        const int px = static_cast<int>(math::Floor(x));
        const int py = static_cast<int>(math::Floor(y));
        if (px < 0 || py < 0 || px >= attch.width || py >= attch.height) {
            return false;
        }

        const size_t pixel_index = static_cast<size_t>(py) * attch.width + px;
        uint8_t* pixel_addr =
            attch.memory->data() + attch.offset + pixel_index * vg::GetPixelSize(attch.format, attch.component_type);
        SpinLock& lock = GetDepthLock(px, py);

        real old_depth = 0.0_r;
        bool depth_pass = true;
        uint8_t stencil = 0;
        bool stencil_pass = true;
        const size_t face_idx = is_front_face ? 0 : 1;

        // Read
        lock.Lock();
        if (attch.format == VG_DEPTH_STENCIL) {
            // Read depth, stencil

            vg::DecodeDepthStencil(&old_depth, &stencil, pixel_addr);

            // Stencil test
            if (state_.stencil_test_enabled) {
                const uint8_t ref_m =
                    static_cast<uint8_t>(state_.stencil_ref[face_idx]) & state_.stencil_func_mask[face_idx];
                const uint8_t val_m = stencil & state_.stencil_func_mask[face_idx];
                switch (state_.stencil_func[face_idx]) {
                    case VG_NEVER:
                        stencil_pass = false;
                        break;
                    case VG_ALWAYS:
                        stencil_pass = true;
                        break;
                    case VG_LESS:
                        stencil_pass = (ref_m < val_m);
                        break;
                    case VG_LEQUAL:
                        stencil_pass = (ref_m <= val_m);
                        break;
                    case VG_GREATER:
                        stencil_pass = (ref_m > val_m);
                        break;
                    case VG_GEQUAL:
                        stencil_pass = (ref_m >= val_m);
                        break;
                    case VG_EQUAL:
                        stencil_pass = (ref_m == val_m);
                        break;
                    case VG_NOTEQUAL:
                        stencil_pass = (ref_m != val_m);
                        break;
                    default:
                        // no op
                        break;
                }
            }
        } else {
            // Read depth only
            vg::CopyPixel(reinterpret_cast<uint8_t*>(&old_depth), pixel_addr, attch.format, attch.component_type,
                          VG_DEPTH_COMPONENT, VG_FLOAT);
        }

        // Depth test
        if (state_.depth_test_enabled && stencil_pass) {
            switch (state_.depth_func) {
                case VG_NEVER:
                    depth_pass = false;
                    break;
                case VG_LESS:
                    depth_pass = (depth < old_depth);
                    break;
                case VG_EQUAL:
                    depth_pass = (depth == old_depth);
                    break;
                case VG_LEQUAL:
                    depth_pass = (depth <= old_depth);
                    break;
                case VG_GREATER:
                    depth_pass = (depth > old_depth);
                    break;
                case VG_NOTEQUAL:
                    depth_pass = (depth != old_depth);
                    break;
                case VG_GEQUAL:
                    depth_pass = (depth >= old_depth);
                    break;
                case VG_ALWAYS:
                    depth_pass = true;
                    break;
                default:
                    // no op
                    break;
            }
        }

        if (!compare_only) {
            if (attch.format == VG_DEPTH_STENCIL) {
                // Stencil update
                VGenum stencil_op = VG_KEEP;
                if (!stencil_pass)
                    stencil_op = state_.stencil_sfail_op[face_idx];
                else if (!depth_pass)
                    stencil_op = state_.stencil_dpfail_op[face_idx];
                else
                    stencil_op = state_.stencil_dppass_op[face_idx];

                if (state_.stencil_test_enabled) {
                    const uint8_t write_mask = static_cast<uint8_t>(state_.stencil_write_mask[face_idx]);
                    if (write_mask != 0) {
                        uint8_t result = stencil;
                        const uint8_t ref = static_cast<uint8_t>(state_.stencil_ref[face_idx]);
                        switch (stencil_op) {
                            case VG_KEEP:
                                break;
                            case VG_ZERO:
                                result = 0;
                                break;
                            case VG_REPLACE:
                                result = ref;
                                break;
                            case VG_INCR:
                                result = (stencil < 255) ? (stencil + 1) : 255;
                                break;
                            case VG_DECR:
                                result = (stencil > 0) ? (stencil - 1) : 0;
                                break;
                            case VG_INVERT:
                                result = static_cast<uint8_t>(~stencil);
                                break;
                            case VG_INCR_WRAP:
                                result = static_cast<uint8_t>((stencil + 1) & 0xFF);
                                break;
                            case VG_DECR_WRAP:
                                result = static_cast<uint8_t>((stencil - 1) & 0xFF);
                                break;
                            default:
                                // no op
                                break;
                        }
                        stencil = (stencil & ~write_mask) | (result & write_mask);
                    }
                }

                // Depth update
                real write_depth = old_depth;
                if (state_.depth_test_enabled && depth_pass && state_.depth_write_enabled && stencil_pass) {
                    write_depth = depth;
                }

                // Write depth stencil
                uint8_t bytes[4];
                vg::EncodeDepthStencil(bytes, write_depth, stencil);

                std::memcpy(pixel_addr, bytes, 4);

            } else {
                // Write depth only
                if (state_.depth_test_enabled && depth_pass && state_.depth_write_enabled) {
                    vg::CopyPixel(pixel_addr, reinterpret_cast<const uint8_t*>(&depth), VG_DEPTH_COMPONENT, VG_FLOAT,
                                  VG_DEPTH_COMPONENT, VG_FLOAT);
                }
            }
        }
        lock.Unlock();
        return (stencil_pass && depth_pass);
    }

    real VirtualGPU::GetBlendFactor(VGenum factor, const Color128& src, const Color128& dst, int channel) const {
        // channel: 0=R, 1=G, 2=B, 3=A
        switch (factor) {
            case VG_ZERO:
                return 0.0_r;
            case VG_ONE:
                return 1.0_r;

            case VG_SRC_COLOR:
                switch (channel) {
                    case 0:
                        return src.r;
                    case 1:
                        return src.g;
                    case 2:
                        return src.b;
                    case 3:
                        return src.a;
                    default:
                        return math::REAL_NaN;
                }
                break;

            case VG_ONE_MINUS_SRC_COLOR:
                switch (channel) {
                    case 0:
                        return 1.0_r - src.r;
                    case 1:
                        return 1.0_r - src.g;
                    case 2:
                        return 1.0_r - src.b;
                    case 3:
                        return 1.0_r - src.a;
                    default:
                        return math::REAL_NaN;
                }
                break;
            case VG_DST_COLOR:
                switch (channel) {
                    case 0:
                        return dst.r;
                    case 1:
                        return dst.g;
                    case 2:
                        return dst.b;
                    case 3:
                        return dst.a;
                    default:
                        return math::REAL_NaN;
                }
                break;
            case VG_ONE_MINUS_DST_COLOR:
                switch (channel) {
                    case 0:
                        return 1.0_r - dst.r;
                    case 1:
                        return 1.0_r - dst.g;
                    case 2:
                        return 1.0_r - dst.b;
                    case 3:
                        return 1.0_r - dst.a;
                    default:
                        return math::REAL_NaN;
                }
                break;
            case VG_SRC_ALPHA:
                return src.a;
            case VG_ONE_MINUS_SRC_ALPHA:
                return 1.0_r - src.a;
            case VG_DST_ALPHA:
                return dst.a;
            case VG_ONE_MINUS_DST_ALPHA:
                return 1.0_r - dst.a;
            case VG_CONSTANT_COLOR:
                switch (channel) {
                    case 0:
                        return state_.blend_constant.r;
                    case 1:
                        return state_.blend_constant.g;
                    case 2:
                        return state_.blend_constant.b;
                    case 3:
                        return state_.blend_constant.a;
                    default:
                        return math::REAL_NaN;
                }
                break;
            case VG_ONE_MINUS_CONSTANT_COLOR:
                switch (channel) {
                    case 0:
                        return 1.0_r - state_.blend_constant.r;
                    case 1:
                        return 1.0_r - state_.blend_constant.g;
                    case 2:
                        return 1.0_r - state_.blend_constant.b;
                    case 3:
                        return 1.0_r - state_.blend_constant.a;
                    default:
                        return math::REAL_NaN;
                }
                break;
            case VG_CONSTANT_ALPHA:
                return state_.blend_constant.a;
            case VG_ONE_MINUS_CONSTANT_ALPHA:
                return 1.0_r - state_.blend_constant.a;
            case VG_SRC_ALPHA_SATURATE:
                switch (channel) {
                    case 0:
                    case 1:
                    case 2:
                        return math::Min(src.a, 1.0_r - dst.a);
                    case 3:
                        return 1.0_r;
                    default:
                        return math::REAL_NaN;
                }
            default:
                return 1.0_r;
        }
    }

    real VirtualGPU::ApplyBlendEquation(VGenum eq, real src_term, real dst_term) const {
        switch (eq) {
            case VG_FUNC_ADD:
                return src_term + dst_term;
            case VG_FUNC_SUBTRACT:
                return src_term - dst_term;
            case VG_FUNC_REVERSE_SUBTRACT:
                return dst_term - src_term;
            case VG_MIN:
                return math::Min(src_term, dst_term);
            case VG_MAX:
                return math::Max(src_term, dst_term);
            default:
                return src_term + dst_term;
        }
    }

    void VirtualGPU::WriteColor(real x, real y, const Color128& color, size_t slot) {
        FrameBuffer* fb = bound_draw_frame_buffer_;
        if (!fb) {
            return;
        }

        if (fb->draw_slot_to_color_attachment[slot] < 0 ||
            fb->draw_slot_to_color_attachment[slot] >= DRAW_BUFFER_SLOT_COUNT) {
            return;
        }
        Attachment& attch = fb->color_attachments[fb->draw_slot_to_color_attachment[slot]];
        if (!attch.external_memory && !attch.memory) {
            return;
        }

        const DrawBufferState& dbs = state_.draw_buffer_states[slot];

        const int px = static_cast<int>(math::Floor(x));
        const int py = static_cast<int>(math::Floor(y));

        const int pixel_offset = py * attch.width + px;
        uint8_t* base = attch.external_memory != nullptr ? attch.external_memory : attch.memory->data();
        uint8_t* pixel_addr =
            base +
            static_cast<size_t>(attch.offset + pixel_offset * vg::GetPixelSize(attch.format, attch.component_type));
        SpinLock& lock = GetColorLock(fb->draw_slot_to_color_attachment[slot], px, py);

        Color128 dst_color;

        // read previous color
        lock.Lock();
        vg::DecodeColor(&dst_color, pixel_addr, attch.format, attch.component_type);

        Color128 final_color = color;

        // blending
        if (dbs.blend_enabled) {
            real src_factor;
            real dst_factor;
            real src_term;
            real dst_term;

            // RGB
            src_factor = GetBlendFactor(state_.blend_src_rgb_factor, color, dst_color, 0);
            dst_factor = GetBlendFactor(state_.blend_dst_rgb_factor, color, dst_color, 0);
            src_term = color.r * src_factor;
            dst_term = dst_color.r * dst_factor;
            final_color.r = ApplyBlendEquation(state_.blend_rgb_equation, src_term, dst_term);

            src_factor = GetBlendFactor(state_.blend_src_rgb_factor, color, dst_color, 1);
            dst_factor = GetBlendFactor(state_.blend_dst_rgb_factor, color, dst_color, 1);
            src_term = color.g * src_factor;
            dst_term = dst_color.g * dst_factor;
            final_color.g = ApplyBlendEquation(state_.blend_rgb_equation, src_term, dst_term);

            src_factor = GetBlendFactor(state_.blend_src_rgb_factor, color, dst_color, 2);
            dst_factor = GetBlendFactor(state_.blend_dst_rgb_factor, color, dst_color, 2);
            src_term = color.b * src_factor;
            dst_term = dst_color.b * dst_factor;
            final_color.b = ApplyBlendEquation(state_.blend_rgb_equation, src_term, dst_term);

            // Alpha
            src_factor = GetBlendFactor(state_.blend_src_alpha_factor, color, dst_color, 3);
            dst_factor = GetBlendFactor(state_.blend_dst_alpha_factor, color, dst_color, 3);
            src_term = color.a * src_factor;
            dst_term = dst_color.a * dst_factor;
            final_color.a = ApplyBlendEquation(state_.blend_alpha_equation, src_term, dst_term);
        }

        // apply color mask
        Color128 write_color = dst_color;
        if (dbs.color_mask[0]) write_color.r = final_color.r;
        if (dbs.color_mask[1]) write_color.g = final_color.g;
        if (dbs.color_mask[2]) write_color.b = final_color.b;
        if (dbs.color_mask[3]) write_color.a = final_color.a;

        vg::EncodeColor(pixel_addr, write_color, attch.format, attch.component_type);
        lock.Unlock();
    }

    void VirtualGPU::AfterVSJobEntry(void* input, int size) {
        assert(size == sizeof(AfterVSJobInput));
        (void)size;
        VirtualGPU& vg = VirtualGPU::GetInstance();
        AfterVSJobInput* in = static_cast<AfterVSJobInput*>(input);

        std::vector<Varying> poly;
        poly.reserve(in->poly.size());

        for (Varying* v : in->poly) {
            poly.emplace_back(*v);
        }

        // Clipping
        poly = std::move(vg.Clip(poly));

        if (poly.empty()) {
            delete in;
            return;
        }

        // Perspective devide, Viewport transform
        for (Varying& v : poly) {
            vg.PerspectiveDivide(v);
            vg.ViewportTransform(v);
        }

        // Rasterization
        std::vector<Fragment> frags;
        std::vector<Fragment> temp_frags;
        const VGenum pmode = vg.state_.polygon_mode;

        if (poly.size() == 1) {
            frags = std::move(vg.Rasterize(poly[0]));
        } else if (poly.size() == 2) {
            frags = std::move(vg.Rasterize(poly[0], poly[1]));
        } else {
            switch (pmode) {
                case VG_POINT:
                    for (const Varying& v : poly) {
                        temp_frags = std::move(vg.Rasterize(v));
                        frags.insert(frags.end(), std::make_move_iterator(temp_frags.begin()),
                                     std::make_move_iterator(temp_frags.end()));
                    }
                    break;

                case VG_LINE:
                    for (size_t i = 0; i < poly.size(); i++) {
                        const Varying& a = poly[i];
                        const Varying& b = poly[(i + 1) % poly.size()];
                        temp_frags = std::move(vg.Rasterize(a, b));
                        frags.insert(frags.end(), std::make_move_iterator(temp_frags.begin()),
                                     std::make_move_iterator(temp_frags.end()));
                    }
                    break;
                case VG_FILL:
                    for (size_t i = 1; i + 1 < poly.size(); i++) {
                        temp_frags = std::move(vg.Rasterize(poly[0], poly[i], poly[i + 1]));
                        frags.insert(frags.end(), std::make_move_iterator(temp_frags.begin()),
                                     std::make_move_iterator(temp_frags.end()));
                    }
                    break;
                default:
                    // no op
                    break;
            }
        }

        if (frags.empty()) {
            delete in;
            return;
        }

        // Output merger
        FSOutputs outputs;
        for (const Fragment& frag : frags) {
            outputs.Reset();
            in->fs(frag, outputs);
            if (!vg.TestDepthStencil(frag.screen_coord.x, frag.screen_coord.y, frag.depth, frag.is_front)) {
                continue;
            }
            for (size_t slot = 0; slot < static_cast<size_t>(DRAW_BUFFER_SLOT_COUNT); slot++) {
                if (!outputs.written.test(slot)) {
                    continue;
                }

                vg.WriteColor(frag.screen_coord.x, frag.screen_coord.y, outputs.values[slot], slot);
            }
        }
        delete in;
    }
}  // namespace ho