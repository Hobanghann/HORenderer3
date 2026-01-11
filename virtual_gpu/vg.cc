#include "vg.h"

#include <cassert>

#include "virtual_gpu.h"
#include "virtual_gpu_utils.h"

namespace ho {
    // helper
    void FreeVram(std::vector<uint8_t>* mem);
    void ReleaseAttachment(VGuint refid);

    void ReleaseVertexArray(VGuint vertex_array);
    void ReleaseBufferObject(VGuint buffer_object);
    void ReleaseTextureObject(VGuint texture);
    void ReleaseSampler(VGuint sampler);
    void ReleaseFrameBuffer(VGuint frame_buffer);
    void ReleaseRenderBuffer(VGuint render_buffer);
    void ReleaseShader(VGuint shader);
    void ReleaseProgram(VGuint program);

    void DestroyVertexArray(VGuint vertex_array);
    void DestroyBufferObject(VGuint buffer_object);
    void DestroyTextureObject(VGuint texture);
    void DestroySampler(VGuint sampler);
    void DestroyFrameBuffer(VGuint frame_buffer);
    void DestroyRenderBuffer(VGuint render_buffer);
    void DestroyShader(VGuint shader);
    void DestroyProgram(VGuint program);

    //////////////////////////////////////////////////
    // GL VERSION 1.0 API
    //////////////////////////////////////////////////
    void vgCullFace(VGenum mode) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (mode == VG_BACK || mode == VG_FRONT || mode == VG_FRONT_AND_BACK) {
            vg.state_.cull_face = mode;
        } else {
            vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgFrontFace(VGenum mode) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (mode == VG_CW || mode == VG_CCW) {
            vg.state_.front_face = mode;
        } else {
            vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgPolygonMode(VGenum face, VGenum mode) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (face == VG_FRONT_AND_BACK && (mode == VG_POINT || mode == VG_LINE || mode == VG_FILL)) {
            vg.state_.polygon_mode = mode;
        } else {
            vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgScissor(VGint x, VGint y, VGsizei width, VGsizei height) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (width >= 0 && height >= 0) {
            vg.state_.scissor = {x, y, width, height};

        } else {
            vg.state_.error_state = VG_INVALID_VALUE;
        }
    }
    // void vgTexParameterf(VGenum target, VGenum pname, VGfloat param) {
    //     VirtualGPU& vg = VirtualGPU::GetInstance();
    //     if (vg.state_.error_state != VG_NO_ERROR) {
    //         return;
    //     }
    //     VirtualGPU::TextureUnit& tu = vg.texture_units_[vg.active_texture_unit_];
    //     uint32_t slot = vg::GetTextureSlot(target);
    //     if (slot == VG_INVALID_SLOT) {
    //         vg.state_.error_state = VG_INVALID_ENUM;
    //         return;
    //     }
    //
    //     if (tu.bound_texture_targets[slot] == nullptr || tu.bound_texture_targets[slot]->is_deleted) {
    //         vg.state_.error_state = VG_INVALID_OPERATION;
    //         return;
    //     }
    //
    //     VirtualGPU::Sampler& ds = tu.bound_texture_targets[slot]->default_sampler;
    //     switch (pname) {
    //         default:
    //             vg.state_.error_state = VG_INVALID_ENUM;
    //     }
    // }
    void vgTexParameterfv(VGenum target, VGenum pname, const VGfloat* params) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        VirtualGPU::TextureUnit& tu = vg.texture_units_[vg.active_texture_unit_];
        uint32_t slot = vg::GetTextureSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (tu.bound_texture_targets[slot] == nullptr || tu.bound_texture_targets[slot]->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Sampler& ds = tu.bound_texture_targets[slot]->default_sampler;
        switch (pname) {
            case VG_TEXTURE_BORDER_COLOR:
                ds.border_color = {params[0], params[1], params[2], params[3]};
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgTexParameteri(VGenum target, VGenum pname, VGint param) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        VirtualGPU::TextureUnit& tu = vg.texture_units_[vg.active_texture_unit_];
        uint32_t slot = vg::GetTextureSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (tu.bound_texture_targets[slot] == nullptr || tu.bound_texture_targets[slot]->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Sampler& ds = tu.bound_texture_targets[slot]->default_sampler;
        switch (pname) {
            case VG_TEXTURE_WRAP_S:
                if (param == (VGint)VG_CLAMP_TO_EDGE || param == (VGint)VG_CLAMP_TO_BORDER ||
                    param == (VGint)VG_MIRRORED_REPEAT || param == (VGint)VG_REPEAT) {
                    ds.wrap_s = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_WRAP_T:
                if (param == (VGint)VG_CLAMP_TO_EDGE || param == (VGint)VG_CLAMP_TO_BORDER ||
                    param == (VGint)VG_MIRRORED_REPEAT || param == (VGint)VG_REPEAT) {
                    ds.wrap_t = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_WRAP_R:
                if (param == (VGint)VG_CLAMP_TO_EDGE || param == (VGint)VG_CLAMP_TO_BORDER ||
                    param == (VGint)VG_MIRRORED_REPEAT || param == (VGint)VG_REPEAT) {
                    ds.wrap_r = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_MIN_FILTER:
                if (param >= (VGint)VG_NEAREST && param <= (VGint)VG_LINEAR) {
                    ds.min_filter = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_MAG_FILTER:
                if (param == (VGint)VG_NEAREST || param == (VGint)VG_LINEAR) {
                    ds.mag_filter = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_SWIZZLE_R:
                if ((param >= (VGint)VG_RED && param <= (VGint)VG_ALPHA) || param == (VGint)VG_ONE ||
                    param == (VGint)VG_ZERO) {
                    ds.swizzle_r = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_SWIZZLE_G:
                if ((param >= (VGint)VG_RED && param <= (VGint)VG_ALPHA) || param == (VGint)VG_ONE ||
                    param == (VGint)VG_ZERO) {
                    ds.swizzle_g = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_SWIZZLE_B:
                if ((param >= (VGint)VG_RED && param <= (VGint)VG_ALPHA) || param == (VGint)VG_ONE ||
                    param == (VGint)VG_ZERO) {
                    ds.swizzle_b = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            case VG_TEXTURE_SWIZZLE_A:
                if ((param >= (VGint)VG_RED && param <= (VGint)VG_ALPHA) || param == (VGint)VG_ONE ||
                    param == (VGint)VG_ZERO) {
                    ds.swizzle_a = param;
                } else {
                    vg.state_.error_state = VG_INVALID_ENUM;
                }
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgTexParameteriv(VGenum target, VGenum pname, const VGint* params) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        VirtualGPU::TextureUnit& tu = vg.texture_units_[vg.active_texture_unit_];
        uint32_t slot = vg::GetTextureSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (tu.bound_texture_targets[slot] == nullptr || tu.bound_texture_targets[slot]->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Sampler& ds = tu.bound_texture_targets[slot]->default_sampler;
        switch (pname) {
            case VG_TEXTURE_SWIZZLE_RGBA:
                ds.swizzle_r = params[0];
                ds.swizzle_g = params[1];
                ds.swizzle_b = params[2];
                ds.swizzle_a = params[3];
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgTexImage1D(VGenum target, VGint level, VGint internalformat, VGsizei width, VGint border, VGenum format,
                      VGenum type, const void* pixels) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (border != 0 || width < 0 || level != 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        switch (internalformat) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                break;

            case VG_DEPTH_COMPONENT:
                if (format != VG_DEPTH_COMPONENT || type != VG_FLOAT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            case VG_DEPTH_STENCIL:
                if (format != VG_DEPTH_STENCIL || type != VG_UNSIGNED_INT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        if (target != VG_TEXTURE_1D && target != VG_PROXY_TEXTURE_1D) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }
        uint32_t slot = vg::GetTextureSlot(target);

        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (vg::IsProxyTexture(target)) {
            vg.proxy_texture_states_[slot].format = format;
            vg.proxy_texture_states_[slot].width = width;
            vg.proxy_texture_states_[slot].height = 1;
            vg.proxy_texture_states_[slot].depth = 1;
            return;
        }

        VirtualGPU::TextureObject* tex = vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot];
        if (!tex || tex->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }
        tex->internal_format = internalformat;
        switch (internalformat) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                tex->component_type = VG_UNSIGNED_BYTE;
                break;

            case VG_DEPTH_COMPONENT:
                tex->component_type = VG_FLOAT;
                break;

            case VG_DEPTH_STENCIL:
                tex->component_type = VG_UNSIGNED_INT;
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        VirtualGPU::TextureLevel& tex_level = tex->mipmap[level];
        if (tex_level.memory == nullptr) {
            vg.vram_.push_back(std::vector<uint8_t>());
            tex_level.memory = &vg.vram_.back();
        }
        tex_level.mipmap_level = level;
        tex_level.width = width;
        tex_level.height = 1;
        tex_level.depth = 1;

        const size_t src_pixel_size = vg::GetPixelSize(format, type);
        const size_t dst_pixel_size = vg::GetPixelSize(internalformat, tex->component_type);

        tex_level.memory->clear();
        tex_level.memory->resize(width * dst_pixel_size);

        if (pixels == nullptr) {
            return;
        }

        const uint8_t* src = static_cast<const uint8_t*>(pixels);
        uint8_t* dst = tex_level.memory->data();

        for (int x = 0; x < width; x++) {
            vg::CopyPixel(dst, src, tex->internal_format, tex->component_type, format, type);
            src += src_pixel_size;
            dst += dst_pixel_size;
        }
    }

    void vgTexImage2D(VGenum target, VGint level, VGint internalformat, VGsizei width, VGsizei height, VGint border,
                      VGenum format, VGenum type, const void* pixels) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (border != 0 || width < 0 || height < 0 || level != 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        switch (internalformat) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                break;

            case VG_DEPTH_COMPONENT:
                if (format != VG_DEPTH_COMPONENT || type != VG_FLOAT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            case VG_DEPTH_STENCIL:
                if (format != VG_DEPTH_STENCIL || type != VG_UNSIGNED_INT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        if (target != VG_TEXTURE_2D && target != VG_PROXY_TEXTURE_2D) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }
        uint32_t slot = vg::GetTextureSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (vg::IsProxyTexture(target)) {
            vg.proxy_texture_states_[slot].format = format;
            vg.proxy_texture_states_[slot].width = width;
            vg.proxy_texture_states_[slot].height = height;
            vg.proxy_texture_states_[slot].depth = 1;
            return;
        }

        VirtualGPU::TextureObject* tex = vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot];
        if (!tex || tex->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        tex->internal_format = internalformat;
        switch (internalformat) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                tex->component_type = VG_UNSIGNED_BYTE;
                break;

            case VG_DEPTH_COMPONENT:
                tex->component_type = VG_FLOAT;
                break;

            case VG_DEPTH_STENCIL:
                tex->component_type = VG_UNSIGNED_INT;
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        VirtualGPU::TextureLevel& tex_level = tex->mipmap[level];
        if (tex_level.memory == nullptr) {
            vg.vram_.push_back(std::vector<uint8_t>());
            tex_level.memory = &vg.vram_.back();
        }
        tex_level.mipmap_level = level;
        tex_level.width = width;
        tex_level.height = height;
        tex_level.depth = 1;

        const size_t src_pixel_size = vg::GetPixelSize(format, type);
        const size_t dst_pixel_size = vg::GetPixelSize(internalformat, tex->component_type);

        tex_level.memory->clear();
        tex_level.memory->resize(width * height * dst_pixel_size);

        if (pixels == nullptr) {
            return;
        }

        const uint8_t* src = static_cast<const uint8_t*>(pixels);
        uint8_t* dst = tex_level.memory->data();

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                vg::CopyPixel(dst, src, tex->internal_format, tex->component_type, format, type);
                src += src_pixel_size;
                dst += dst_pixel_size;
            }
        }
    }
    void vgDrawBuffer(VGenum buf) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto* fb = vg.bound_draw_frame_buffer_;
        if (!fb) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        // reset all draw buffers
        fb->draw_slot_to_color_attachment.fill(-1);

        // default framebuffer
        if (fb->id == 0) {
            switch (buf) {
                case VG_NONE:
                    return;

                case VG_BACK:
                    fb->draw_slot_to_color_attachment[0] = 0;
                    return;

                case VG_FRONT:
                case VG_FRONT_AND_BACK:
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;

                default:
                    vg.state_.error_state = VG_INVALID_ENUM;
                    return;
            }
        }

        // user FBO
        if (buf == VG_NONE) {
            return;
        }

        if (buf >= VG_COLOR_ATTACHMENT0 && buf <= VG_COLOR_ATTACHMENT31) {
            fb->draw_slot_to_color_attachment[0] = buf - VG_COLOR_ATTACHMENT0;
            return;
        }

        vg.state_.error_state = VG_INVALID_ENUM;
    }

    void vgClear(VGbitfield mask) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        VirtualGPU::FrameBuffer* fb = vg.bound_draw_frame_buffer_;

        if (!fb) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        const bool is_color_cleared = (mask & VG_COLOR_BUFFER_BIT) != 0;
        const bool is_depth_cleared = ((mask & VG_DEPTH_BUFFER_BIT) != 0);
        const bool is_stencil_cleared = (mask & VG_STENCIL_BUFFER_BIT) != 0;

        // Clear color
        if (is_color_cleared) {
            for (int i = 0; i < VG_DRAW_BUFFER_SLOT_COUNT; ++i) {
                vg.ClearColorAttachment((uint32_t)i, vg.state_.clear_color);
            }
        }

        if (fb->depth_stencil_attachment.memory == nullptr) {
            if (is_depth_cleared || is_stencil_cleared) {
                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
            }
        }

        if (fb->depth_stencil_attachment.format == VG_DEPTH_COMPONENT) {
            if (is_stencil_cleared) {
                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
            }
            vg.ClearDepthAttachment(vg.state_.clear_depth);
        }

        if (fb->depth_stencil_attachment.format == VG_DEPTH_STENCIL) {
            vg.ClearDepthStencilAttachment(is_depth_cleared, is_stencil_cleared, vg.state_.clear_depth,
                                           (uint8_t)vg.state_.clear_stencil);
        }
    }
    void vgClearColor(VGfloat red, VGfloat green, VGfloat blue, VGfloat alpha) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.clear_color = Color128(red, green, blue, alpha);
    }
    void vgClearStencil(VGint s) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.clear_stencil = s;
    }
    void vgClearDepth(VGdouble depth) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.clear_depth = (VGfloat)depth;
    }
    void vgStencilMask(VGuint mask) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.stencil_write_mask[0] = mask;
        vg.state_.stencil_write_mask[1] = mask;
    }
    void vgColorMask(VGboolean red, VGboolean green, VGboolean blue, VGboolean alpha) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        for (int i = 0; i < VG_DRAW_BUFFER_SLOT_COUNT; i++) {
            vg.state_.draw_buffer_states[i].color_mask[0] = (bool)red;
            vg.state_.draw_buffer_states[i].color_mask[1] = (bool)green;
            vg.state_.draw_buffer_states[i].color_mask[2] = (bool)blue;
            vg.state_.draw_buffer_states[i].color_mask[3] = (bool)alpha;
        }
    }
    void vgDepthMask(VGboolean flag) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.depth_write_enabled = (bool)flag;
    }
    void vgDisable(VGenum cap) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        switch (cap) {
            case VG_BLEND:
                for (int i = 0; i < VG_DRAW_BUFFER_SLOT_COUNT; i++) {
                    vg.state_.draw_buffer_states[i].blend_enabled = false;
                }
                break;
            case VG_CULL_FACE:
                vg.state_.cull_enabled = false;
                break;
            case VG_DEPTH_TEST:
                vg.state_.depth_test_enabled = false;
                break;
            case VG_SCISSOR_TEST:
                vg.state_.scissor_test_enabled = false;
                break;
            case VG_STENCIL_TEST:
                vg.state_.stencil_test_enabled = false;
                break;
            case VG_POLYGON_OFFSET_FILL:
                vg.state_.polygon_offset_enabled = false;
                break;
            case VG_POLYGON_OFFSET_LINE:
                vg.state_.line_offset_enabled = false;
                break;
            case VG_POLYGON_OFFSET_POINT:
                vg.state_.point_offset_enabled = false;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgEnable(VGenum cap) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        switch (cap) {
            case VG_BLEND:
                for (int i = 0; i < VG_DRAW_BUFFER_SLOT_COUNT; i++) {
                    vg.state_.draw_buffer_states[i].blend_enabled = true;
                }
                break;
            case VG_CULL_FACE:
                vg.state_.cull_enabled = true;
                break;
            case VG_DEPTH_TEST:
                vg.state_.depth_test_enabled = true;
                break;
            case VG_SCISSOR_TEST:
                vg.state_.scissor_test_enabled = true;
                break;
            case VG_STENCIL_TEST:
                vg.state_.stencil_test_enabled = true;
                break;
            case VG_POLYGON_OFFSET_FILL:
                vg.state_.polygon_offset_enabled = true;
                break;
            case VG_POLYGON_OFFSET_LINE:
                vg.state_.line_offset_enabled = true;
                break;
            case VG_POLYGON_OFFSET_POINT:
                vg.state_.point_offset_enabled = true;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgFinish(void) {}  // no op
    void vgFlush(void) {}   // no op
    void vgBlendFunc(VGenum sfactor, VGenum dfactor) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto valid_src_factor = [](VGenum f) {
            return (f == VG_ZERO) || (f == VG_ONE) || (f >= VG_SRC_COLOR && f <= VG_SRC_ALPHA_SATURATE) ||
                   (f >= VG_CONSTANT_COLOR && f <= VG_ONE_MINUS_CONSTANT_ALPHA);
        };

        auto valid_dst_factor = [](VGenum f) {
            if (f == VG_SRC_ALPHA_SATURATE) return false;

            return (f == VG_ZERO) || (f == VG_ONE) || (f >= VG_SRC_COLOR && f <= VG_ONE_MINUS_SRC_ALPHA) ||
                   (f >= VG_CONSTANT_COLOR && f <= VG_ONE_MINUS_CONSTANT_ALPHA);
        };

        if (!valid_src_factor(sfactor) || !valid_dst_factor(dfactor)) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        vg.state_.blend_src_rgb_factor = sfactor;
        vg.state_.blend_src_alpha_factor = sfactor;
        vg.state_.blend_dst_rgb_factor = dfactor;
        vg.state_.blend_dst_alpha_factor = dfactor;
    }
    void vgStencilFunc(VGenum func, VGint ref, VGuint mask) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (func < VG_NEVER || VG_ALWAYS < func) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }
        vg.state_.stencil_func[0] = func;
        vg.state_.stencil_func[1] = func;
        vg.state_.stencil_ref[0] = ref;
        vg.state_.stencil_ref[1] = ref;
        vg.state_.stencil_func_mask[0] = mask;
        vg.state_.stencil_func_mask[1] = mask;
    }
    void vgStencilOp(VGenum sfail, VGenum dpfail, VGenum dppass) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto IsValidStencilOp = [](VGenum op) {
            switch (op) {
                case VG_KEEP:
                case VG_ZERO:
                case VG_REPLACE:
                case VG_INCR:
                case VG_INCR_WRAP:
                case VG_DECR:
                case VG_DECR_WRAP:
                case VG_INVERT:
                    return true;
                default:
                    return false;
            }
        };

        if (!IsValidStencilOp(sfail) || !IsValidStencilOp(dpfail) || !IsValidStencilOp(dppass)) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        // Front
        vg.state_.stencil_sfail_op[0] = sfail;
        vg.state_.stencil_dpfail_op[0] = dpfail;
        vg.state_.stencil_dppass_op[0] = dppass;

        // Back
        vg.state_.stencil_sfail_op[1] = sfail;
        vg.state_.stencil_dpfail_op[1] = dpfail;
        vg.state_.stencil_dppass_op[1] = dppass;
    }

    void vgDepthFunc(VGenum func) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (func < VG_NEVER || VG_ALWAYS < func) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }
        vg.state_.depth_func = func;
    }

    void vgReadBuffer(VGenum src) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto* fb = vg.bound_read_frame_buffer_;
        if (!fb) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        // Default framebuffer
        if (fb->id == 0) {
            switch (src) {
                case VG_NONE:
                    fb->read_slot_to_color_attachment = -1;
                    return;

                case VG_BACK:
                    fb->read_slot_to_color_attachment = 0;
                    return;

                case VG_FRONT:
                case VG_FRONT_AND_BACK:
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;

                default:
                    vg.state_.error_state = VG_INVALID_ENUM;
                    return;
            }
        }

        // User FBO
        if (src == VG_NONE) {
            fb->read_slot_to_color_attachment = -1;
            return;
        }

        if (src >= VG_COLOR_ATTACHMENT0 && src <= VG_COLOR_ATTACHMENT31) {
            fb->read_slot_to_color_attachment = src - VG_COLOR_ATTACHMENT0;
            return;
        }

        vg.state_.error_state = VG_INVALID_ENUM;
    }

    VGenum vgGetError(void) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        VGenum err = vg.state_.error_state;
        vg.state_.error_state = VG_NO_ERROR;
        return err;
    }

    VGboolean vgIsEnabled(VGenum cap) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        switch (cap) {
            case VG_BLEND:
                for (int i = 0; i < VG_DRAW_BUFFER_SLOT_COUNT; i++) {
                    if (!vg.state_.draw_buffer_states[i].blend_enabled) {
                        return VG_FALSE;
                    }
                }
                return VG_TRUE;
                break;
            case VG_CULL_FACE:
                return (VGboolean)vg.state_.cull_enabled;
                break;
            case VG_DEPTH_TEST:
                return (VGboolean)vg.state_.depth_test_enabled;
                break;
            case VG_SCISSOR_TEST:
                return (VGboolean)vg.state_.scissor_test_enabled;
                break;
            case VG_STENCIL_TEST:
                return (VGboolean)vg.state_.stencil_test_enabled;
                break;
            case VG_POLYGON_OFFSET_FILL:
                return (VGboolean)vg.state_.polygon_offset_enabled;
                break;
            case VG_POLYGON_OFFSET_LINE:
                return (VGboolean)vg.state_.line_offset_enabled;
                break;
            case VG_POLYGON_OFFSET_POINT:
                return (VGboolean)vg.state_.point_offset_enabled;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return VG_FALSE;
        }
    }
    void vgDepthRange(VGdouble n, VGdouble f) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.min_depth = (VGfloat)n;
        vg.state_.max_depth = (VGfloat)f;
    }
    void vgViewport(VGint x, VGint y, VGsizei width, VGsizei height) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        vg.state_.viewport = {x, y, width, height};
    }

    //////////////////////////////////////////////////
    // GL VERSION 1.1 API
    //////////////////////////////////////////////////
    void vgDrawArrays(VGenum mode, VGint first, VGsizei count) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (first < 0 || count < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (vg.bound_vertex_array_ == nullptr || vg.using_program_ == nullptr ||
            vg.bound_draw_frame_buffer_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (vg.using_program_->link_status == VG_FALSE) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        vg.using_program_->varying_buffer.resize(count);

        // Vertex Processing
        std::vector<JobDeclaration> vs_jobs;
        vs_jobs.reserve(count);

        const int BATCH_SIZE = 100;

        for (int i = first; i < first + count; i += BATCH_SIZE) {
            int batch_end = math::Min(i + BATCH_SIZE - 1, first + count - 1);
            VirtualGPU::VSJobInput* input =
                new VirtualGPU::VSJobInput{(VirtualGPU::VertexShader)vg.using_program_->vertex_shader->source,
                                           (uint32_t)first, (uint32_t)i, (uint32_t)batch_end};

            JobDeclaration job;
            job.entry = VirtualGPU::VSJobEntry;
            job.input_data = input;
            job.input_size = sizeof(VirtualGPU::VSJobInput);

            vs_jobs.emplace_back(job);
        }

        vg.job_system_.KickJobsAndWait(vs_jobs);

        vs_jobs.clear();

        // Primitive Assembly & Kick After Vertex Processing Jobs
        uint32_t gap = 0;
        uint32_t v_count = 0;
        switch (mode) {
            case VG_POINT:
                gap = 1;
                v_count = 1;
                break;
            case VG_LINE:
                gap = 2;
                v_count = 2;
                break;
            case VG_LINE_STRIP:
                gap = 1;
                v_count = 2;
                break;
            case VG_TRIANGLES:
                gap = 3;
                v_count = 3;
                break;
            case VG_TRIANGLE_STRIP:
                gap = 1;
                v_count = 3;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }

        JobDeclaration after_job;
        after_job.entry = VirtualGPU::AfterVSJobEntry;
        after_job.input_size = sizeof(VirtualGPU::AfterVSJobInput);

        for (uint32_t i = 0; i + v_count <= vg.using_program_->varying_buffer.size(); i += gap) {
            std::vector<VirtualGPU::Varying*> poly;
            poly.reserve(v_count);

            // Fetch base vertices
            for (uint32_t j = i; j < i + v_count; j++) {
                poly.push_back(&vg.using_program_->varying_buffer[j]);
            }

            // TRIANGLE_STRIP: flip winding order on odd triangles
            if (mode == VG_TRIANGLE_STRIP && (i % 2 == 1)) {
                std::swap(poly[1], poly[2]);
            }

            VirtualGPU::AfterVSJobInput* input = new VirtualGPU::AfterVSJobInput(
                {std::move(poly), (VirtualGPU::FragmentShader)vg.using_program_->fragment_shader->source});

            after_job.input_data = input;

            vg.job_system_.KickJob(after_job);
        }

        vg.job_system_.WaitForIdle();
        vg.using_program_->varying_buffer.clear();
    }
    void vgDrawElements(VGenum mode, VGsizei count, VGenum type, const void* indices) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (count < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (vg.bound_vertex_array_ == nullptr || vg.using_program_ == nullptr ||
            vg.bound_draw_frame_buffer_ == nullptr || vg.bound_vertex_array_->element_buffer == nullptr ||
            vg.bound_vertex_array_->element_buffer->memory == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (vg.using_program_->link_status == VG_FALSE) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        size_t idx_size = 0;
        switch (type) {
            case VG_UNSIGNED_BYTE:
                idx_size = 1;
                break;
            case VG_UNSIGNED_SHORT:
                idx_size = 2;
                break;
            case VG_UNSIGNED_INT:
                idx_size = 4;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }

        if (indices != nullptr && (reinterpret_cast<uintptr_t>(indices) + count * idx_size >
                                   vg.bound_vertex_array_->element_buffer->memory->size())) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        vg.using_program_->varying_buffer.resize(vg.bound_vertex_array_->vertex_count);

        // Vertex Processing

        // Enroll varyings:
        // The main thread must execute the vertex shader at least once before the job threads begin.
        // Failing to do so causes a race condition, as job threads attempt to access the program's name hash mapping
        // array while it is empty.
        // VirtualGPU::Varying dummy;
        // ((VirtualGPU::VertexShader)vg.using_program_->vertex_shader->source)(0, dummy);

        std::vector<JobDeclaration> vs_jobs;
        vs_jobs.reserve(vg.bound_vertex_array_->vertex_count);

        const uint32_t BATCH_SIZE = 100;

        for (uint32_t i = 0; i < vg.bound_vertex_array_->vertex_count; i += BATCH_SIZE) {
            uint32_t batch_end = math::Min(i + BATCH_SIZE - 1, (uint32_t)(vg.bound_vertex_array_->vertex_count - 1));
            VirtualGPU::VSJobInput* input = new VirtualGPU::VSJobInput{
                (VirtualGPU::VertexShader)vg.using_program_->vertex_shader->source, 0, i, batch_end};

            JobDeclaration job;
            job.entry = VirtualGPU::VSJobEntry;
            job.input_data = input;
            job.input_size = sizeof(VirtualGPU::VSJobInput);

            vs_jobs.push_back(job);
        }

        vg.job_system_.KickJobsAndWait(vs_jobs);

        vs_jobs.clear();

        // Primitive Assembly & Kick After Vertex Processing Jobs
        uint32_t gap = 0;
        uint32_t v_count = 0;
        switch (mode) {
            case VG_POINT:
                gap = 1;
                v_count = 1;
                break;
            case VG_LINE:
                gap = 2;
                v_count = 2;
                break;
            case VG_LINE_STRIP:
                gap = 1;
                v_count = 2;
                break;
            case VG_TRIANGLES:
                gap = 3;
                v_count = 3;
                break;
            case VG_TRIANGLE_STRIP:
                gap = 1;
                v_count = 3;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }

        uint8_t* ebo = vg.bound_vertex_array_->element_buffer->memory->data();
        uintptr_t offset = indices == nullptr ? 0 : reinterpret_cast<uintptr_t>(indices);

        const uint8_t* base = ebo + offset;

        auto FetchIndex = [&](size_t k) -> uint32_t {
            switch (type) {
                case VG_UNSIGNED_BYTE:
                    return (uint32_t)(((const uint8_t*)base)[k]);
                case VG_UNSIGNED_SHORT:
                    return (uint32_t)(((const uint16_t*)base)[k]);
                case VG_UNSIGNED_INT:
                    return ((const uint32_t*)base)[k];
            }
            return 0;
        };

        JobDeclaration job;
        job.entry = VirtualGPU::AfterVSJobEntry;
        job.input_size = sizeof(VirtualGPU::AfterVSJobInput);

        for (uint32_t i = 0; i + v_count <= (uint32_t)count; i += gap) {
            std::vector<VirtualGPU::Varying*> poly;
            poly.reserve(v_count);

            for (uint32_t j = 0; j < v_count; j++) {
                uint32_t idx = FetchIndex(i + j);
                poly.push_back(&(vg.using_program_->varying_buffer[idx]));
            }

            // TRIANGLE_STRIP: flip winding order on odd triangles
            if (mode == VG_TRIANGLE_STRIP && ((i / gap) % 2 == 1)) {
                std::swap(poly[1], poly[2]);
            }

            VirtualGPU::AfterVSJobInput* input = new VirtualGPU::AfterVSJobInput(
                {std::move(poly), (VirtualGPU::FragmentShader)vg.using_program_->fragment_shader->source});

            job.input_data = input;

            vg.job_system_.KickJob(job);
        }

        vg.job_system_.WaitForIdle();
        vg.using_program_->varying_buffer.clear();
    }

    void vgPolygonOffset(VGfloat factor, VGfloat units) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.depth_factor = factor;
        vg.state_.depth_unit = units;
    }

    void vgTexSubImage1D(VGenum target, VGint level, VGint xoffset, VGsizei width, VGenum format, VGenum type,
                         const void* pixels) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (pixels == nullptr) {
            return;
        }
        if (width < 0 || level != 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (target != VG_TEXTURE_1D) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }
        uint32_t slot = vg::GetTextureSlot(target);

        VirtualGPU::TextureObject* tex = vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot];
        if (!tex || tex->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }
        switch (tex->internal_format) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                break;

            case VG_DEPTH_COMPONENT:
                if (format != VG_DEPTH_COMPONENT || type != VG_FLOAT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            case VG_DEPTH_STENCIL:
                if (format != VG_DEPTH_STENCIL || type != VG_UNSIGNED_INT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        VirtualGPU::TextureLevel& tex_level = tex->mipmap[level];

        if (!tex_level.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (xoffset < 0 || xoffset + width > tex_level.width) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        const size_t src_pixel_size = vg::GetPixelSize(format, type);
        const size_t dst_pixel_size = vg::GetPixelSize(tex->internal_format, tex->component_type);

        const uint8_t* src = static_cast<const uint8_t*>(pixels);
        uint8_t* dst = tex_level.memory->data() + xoffset * dst_pixel_size;

        for (int x = 0; x < width; x++) {
            vg::CopyPixel(dst, src, tex->internal_format, tex->component_type, format, type);
            src += src_pixel_size;
            dst += dst_pixel_size;
        }
    }
    void vgTexSubImage2D(VGenum target, VGint level, VGint xoffset, VGint yoffset, VGsizei width, VGsizei height,
                         VGenum format, VGenum type, const void* pixels) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (pixels == nullptr) {
            return;
        }

        if (level != 0 || width < 0 || height < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (target != VG_TEXTURE_2D) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        uint32_t slot = vg::GetTextureSlot(target);
        VirtualGPU::TextureObject* tex = vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot];

        if (!tex || tex->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        switch (tex->internal_format) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                break;

            case VG_DEPTH_COMPONENT:
                if (format != VG_DEPTH_COMPONENT || type != VG_FLOAT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            case VG_DEPTH_STENCIL:
                if (format != VG_DEPTH_STENCIL || type != VG_UNSIGNED_INT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        VirtualGPU::TextureLevel& tex_level = tex->mipmap[level];

        if (!tex_level.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }
        if (xoffset < 0 || yoffset < 0 || xoffset + width > tex_level.width || yoffset + height > tex_level.height) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        const size_t src_pixel_size = vg::GetPixelSize(format, type);
        const size_t dst_pixel_size = vg::GetPixelSize(tex->internal_format, tex->component_type);

        const uint8_t* src = static_cast<const uint8_t*>(pixels);

        for (int y = 0; y < height; y++) {
            uint8_t* dst = tex_level.memory->data() + ((yoffset + y) * tex_level.width + xoffset) * dst_pixel_size;
            for (int x = 0; x < width; x++) {
                vg::CopyPixel(dst, src, tex->internal_format, tex->component_type, format, type);
                src += src_pixel_size;
                dst += dst_pixel_size;
            }
        }
    }
    void vgBindTexture(VGenum target, VGuint texture) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        uint32_t slot = vg::GetTextureSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        // unbind
        if (texture == 0) {
            if (vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot]) {
                ReleaseTextureObject(vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot]->id);
                vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot] = nullptr;
            }
            return;
        }

        auto it = vg.texture_pool_.find(texture);
        if (it == vg.texture_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureObject* tex = &it->second;
        if (tex->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (tex->texture_type == VG_NONE)
            tex->texture_type = target;  // first bound
        else if (tex->texture_type != target) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot] == tex) {
            return;
        }

        if (vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot]) {
            ReleaseTextureObject(vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot]->id);
            vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot] = nullptr;
        }

        tex->refcount++;
        vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot] = tex;
    }

    void vgDeleteTextures(VGsizei n, const VGuint* textures) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (!textures) {
            return;
        }

        for (VGsizei i = 0; i < n; ++i) {
            VGuint name = textures[i];
            if (name == 0) {
                continue;
            }

            auto it = vg.texture_pool_.find(name);
            if (it == vg.texture_pool_.end()) {
                continue;
            }

            VirtualGPU::TextureObject& tex = it->second;

            tex.is_deleted = true;

            DestroyTextureObject(name);
        }
    }

    void vgGenTextures(VGsizei n, VGuint* textures) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!textures) {
            return;
        }
        for (uint32_t i = 0; i < (uint32_t)n; i++) {
            vg.base_handle_++;
            VirtualGPU::TextureObject tex;
            tex.id = vg.base_handle_;
            vg.texture_pool_.insert({vg.base_handle_, tex});
            textures[i] = vg.base_handle_;
        }
    }
    VGboolean vgIsTexture(VGuint texture) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (texture == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.texture_pool_.find(texture);
        if (it != vg.texture_pool_.end()) {
            if (it->second.texture_type == VG_NONE || it->second.is_deleted) {
                // not yet associated with a texture by calling vgBindTexture
                return (VGboolean)VG_FALSE;
            } else {
                return (VGboolean)VG_TRUE;
            }
        } else {
            return (VGboolean)VG_FALSE;
        }
    }

    //////////////////////////////////////////////////
    // GL VERSION 1.2 API
    //////////////////////////////////////////////////
    void vgDrawRangeElements(VGenum mode, VGuint start, VGuint end, VGsizei count, VGenum type, const void* indices) {
        // ignore driver hints(start, end)
        start, end;
        vgDrawElements(mode, count, type, indices);
    }

    void vgTexImage3D(VGenum target, VGint level, VGint internalformat, VGsizei width, VGsizei height, VGsizei depth,
                      VGint border, VGenum format, VGenum type, const void* pixels) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (border != 0 || width < 0 || height < 0 || depth < 0 || level != 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        switch (internalformat) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                break;

            case VG_DEPTH_COMPONENT:
                if (format != VG_DEPTH_COMPONENT || type != VG_FLOAT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            case VG_DEPTH_STENCIL:
                if (format != VG_DEPTH_STENCIL || type != VG_UNSIGNED_INT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        if (target != VG_TEXTURE_3D && target != VG_PROXY_TEXTURE_3D) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }
        uint32_t slot = vg::GetTextureSlot(target);

        if (vg::IsProxyTexture(target)) {
            vg.proxy_texture_states_[slot].format = format;
            vg.proxy_texture_states_[slot].width = width;
            vg.proxy_texture_states_[slot].height = height;
            vg.proxy_texture_states_[slot].depth = depth;
            return;
        }

        VirtualGPU::TextureObject* tex = vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot];
        if (!tex || tex->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        tex->internal_format = internalformat;
        switch (internalformat) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                tex->component_type = VG_UNSIGNED_BYTE;
                break;

            case VG_DEPTH_COMPONENT:
                tex->component_type = VG_FLOAT;
                break;

            case VG_DEPTH_STENCIL:
                tex->component_type = VG_UNSIGNED_INT;
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        VirtualGPU::TextureLevel& tex_level = tex->mipmap[level];
        if (tex_level.memory == nullptr) {
            vg.vram_.push_back(std::vector<uint8_t>());
            tex_level.memory = &vg.vram_.back();
        }
        tex_level.mipmap_level = level;
        tex_level.width = width;
        tex_level.height = height;
        tex_level.depth = depth;

        const int channel_count = vg::GetChannelCount(format);
        const size_t type_size = vg::GetTypeSize(type);
        const size_t src_pixel_size = channel_count * type_size;
        const size_t dst_pixel_size = vg::GetPixelSize(internalformat, tex->component_type);

        tex_level.memory->clear();
        tex_level.memory->resize(width * height * depth * dst_pixel_size);

        if (pixels == nullptr) {
            return;
        }

        const uint8_t* src = static_cast<const uint8_t*>(pixels);
        uint8_t* dst = tex_level.memory->data();

        for (int z = 0; z < depth; z++) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    vg::CopyPixel(dst, src, tex->internal_format, tex->component_type, format, type);
                    src += src_pixel_size;
                    dst += dst_pixel_size;
                }
            }
        }
    }
    void vgTexSubImage3D(VGenum target, VGint level, VGint xoffset, VGint yoffset, VGint zoffset, VGsizei width,
                         VGsizei height, VGsizei depth, VGenum format, VGenum type, const void* pixels) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (pixels == nullptr) {
            return;
        }

        if (level != 0 || width < 0 || height < 0 || depth < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (target != VG_TEXTURE_3D) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        uint32_t slot = vg::GetTextureSlot(target);
        VirtualGPU::TextureObject* tex = vg.texture_units_[vg.active_texture_unit_].bound_texture_targets[slot];

        if (!tex || tex->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        switch (tex->internal_format) {
            case VG_RED:
            case VG_RG:
            case VG_RGB:
            case VG_RGBA:
                break;

            case VG_DEPTH_COMPONENT:
                if (format != VG_DEPTH_COMPONENT || type != VG_FLOAT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            case VG_DEPTH_STENCIL:
                if (format != VG_DEPTH_STENCIL || type != VG_UNSIGNED_INT) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        VirtualGPU::TextureLevel& tex_level = tex->mipmap[level];

        if (!tex_level.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }
        if (xoffset < 0 || yoffset < 0 || zoffset < 0 || xoffset + width > tex_level.width ||
            yoffset + height > tex_level.height || zoffset + depth > tex_level.depth) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        const size_t src_pixel_size = vg::GetPixelSize(format, type);
        const size_t dst_pixel_size = vg::GetPixelSize(tex->internal_format, tex->component_type);

        const uint8_t* src = static_cast<const uint8_t*>(pixels);

        for (int z = 0; z < depth; z++) {
            uint8_t* z_dst =
                tex_level.memory->data() + ((zoffset + z) * tex_level.width * tex_level.height) * dst_pixel_size;
            for (int y = 0; y < height; y++) {
                uint8_t* x_dst = z_dst + ((yoffset + y) * tex_level.width + xoffset) * dst_pixel_size;
                for (int x = 0; x < width; x++) {
                    vg::CopyPixel(x_dst, src, tex->internal_format, tex->component_type, format, type);
                    src += src_pixel_size;
                    x_dst += dst_pixel_size;
                }
            }
        }
    }

    //////////////////////////////////////////////////
    // GL VERSION 1.3 API
    //////////////////////////////////////////////////
    void vgActiveTexture(VGenum texture) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (texture > VG_TEXTURE31 || texture < VG_TEXTURE0) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        vg.active_texture_unit_ = texture - VG_TEXTURE0;
    }

    //////////////////////////////////////////////////
    // GL VERSION 1.4 API
    //////////////////////////////////////////////////
    void vgBlendFuncSeparate(VGenum sfactorRGB, VGenum dfactorRGB, VGenum sfactorAlpha, VGenum dfactorAlpha) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto valid_factor = [](VGenum f) {
            return (f == VG_ZERO) || (f == VG_ONE) || (f >= VG_SRC_COLOR && f <= VG_SRC_ALPHA_SATURATE) ||
                   (f >= VG_CONSTANT_COLOR && f <= VG_ONE_MINUS_CONSTANT_ALPHA);
        };

        if (!valid_factor(sfactorRGB) || !valid_factor(sfactorAlpha) || !valid_factor(dfactorRGB) ||
            !valid_factor(dfactorAlpha)) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        vg.state_.blend_src_rgb_factor = sfactorRGB;
        vg.state_.blend_src_alpha_factor = sfactorAlpha;
        vg.state_.blend_dst_rgb_factor = dfactorRGB;
        vg.state_.blend_dst_alpha_factor = dfactorAlpha;
    }
    void vgMultiDrawArrays(VGenum mode, const VGint* first, const VGsizei* count, VGsizei drawcount) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (first == nullptr || count == nullptr || drawcount < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (vg.bound_vertex_array_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        for (VGsizei i = 0; i < drawcount; ++i) {
            vgDrawArrays(mode, first[i], count[i]);
            if (vg.state_.error_state != VG_NO_ERROR) {
                return;
            }
        }
    }
    void vgMultiDrawElements(VGenum mode, const VGsizei* count, VGenum type, const void* const* indices,
                             VGsizei drawcount) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (count == nullptr || indices == nullptr || drawcount < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (vg.bound_vertex_array_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (vg.bound_vertex_array_->element_buffer == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        for (VGsizei i = 0; i < drawcount; ++i) {
            vgDrawElements(mode, count[i], type, indices[i]);
            if (vg.state_.error_state != VG_NO_ERROR) {
                return;
            }
        }
    }

    void vgBlendColor(VGfloat red, VGfloat green, VGfloat blue, VGfloat alpha) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        vg.state_.blend_constant = {red, green, blue, alpha};
    }
    void vgBlendEquation(VGenum mode) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (mode < VG_FUNC_ADD || mode > VG_MAX) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        vg.state_.blend_rgb_equation = mode;
        vg.state_.blend_alpha_equation = mode;
    }

    //////////////////////////////////////////////////
    // GL VERSION 1.5 API
    //////////////////////////////////////////////////
    void vgBindBuffer(VGenum target, VGuint buffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        const uint32_t slot = vg::GetBufferSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        VirtualGPU::BufferObject*& bound = (target == VG_ELEMENT_ARRAY_BUFFER && vg.bound_vertex_array_)
                                               ? vg.bound_vertex_array_->element_buffer
                                               : vg.bound_buffer_targets_[slot];

        if (buffer == 0) {
            if (bound) {
                ReleaseBufferObject(bound->id);
                bound = nullptr;
            }
            return;
        }

        auto it = vg.buffer_pool_.find(buffer);
        if (it == vg.buffer_pool_.end()) {
            auto [new_it, _] = vg.buffer_pool_.emplace(buffer, VirtualGPU::BufferObject());
            it = new_it;
        }

        VirtualGPU::BufferObject& buf = it->second;

        if (buf.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (bound == &buf) {
            return;
        }

        if (!buf.memory) {
            vg.vram_.push_back(std::vector<uint8_t>());
            buf.memory = &vg.vram_.back();
        }

        if (bound) {
            ReleaseBufferObject(bound->id);
            bound = nullptr;
        }

        buf.refcount++;
        bound = &buf;
    }

    void vgDeleteBuffers(VGsizei n, const VGuint* buffers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (!buffers) {
            return;
        }

        for (VGsizei i = 0; i < n; ++i) {
            VGuint name = buffers[i];
            if (name == 0) {
                continue;
            }

            auto it = vg.buffer_pool_.find(name);
            if (it == vg.buffer_pool_.end()) {
                continue;
            }

            VirtualGPU::BufferObject& buf = it->second;

            buf.is_deleted = true;

            DestroyBufferObject(name);
        }
    }

    void vgGenBuffers(VGsizei n, VGuint* buffers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!buffers) {
            return;
        }
        for (uint32_t i = 0; i < (uint32_t)n; i++) {
            vg.base_handle_++;
            VirtualGPU::BufferObject buf;
            buf.id = vg.base_handle_;
            vg.buffer_pool_.insert({vg.base_handle_, buf});
            buffers[i] = vg.base_handle_;
        }
    }
    VGboolean vgIsBuffer(VGuint buffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (buffer == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.buffer_pool_.find(buffer);
        if (it != vg.buffer_pool_.end()) {
            if (it->second.memory == nullptr || it->second.is_deleted) {
                return (VGboolean)VG_FALSE;
            } else {
                return (VGboolean)VG_TRUE;
            }
        } else {
            return (VGboolean)VG_FALSE;
        }
    }
    void vgBufferData(VGenum target, VGsizeiptr size, const void* data, VGenum usage) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        const uint32_t slot = vg::GetBufferSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (size < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::BufferObject* buffer = nullptr;

        if (target == VG_ELEMENT_ARRAY_BUFFER && vg.bound_vertex_array_ != nullptr) {
            buffer = vg.bound_vertex_array_->element_buffer;
        } else {
            buffer = vg.bound_buffer_targets_[slot];
        }

        if (buffer == nullptr || buffer->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (buffer->immutable) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (buffer->memory == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (usage < VG_STREAM_DRAW || usage > VG_DYNAMIC_COPY) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        buffer->usage = usage;

        buffer->memory->resize(static_cast<size_t>(size));

        if (size > 0 && data != nullptr) {
            std::memcpy(buffer->memory->data(), data, static_cast<size_t>(size));
        }
    }

    void vgBufferSubData(VGenum target, VGintptr offset, VGsizeiptr size, const void* data) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        const uint32_t slot = vg::GetBufferSlot(target);
        if (slot == VG_INVALID_SLOT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (offset < 0 || size < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (data == nullptr) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::BufferObject* buffer = nullptr;

        if (target == VG_ELEMENT_ARRAY_BUFFER && vg.bound_vertex_array_ != nullptr) {
            buffer = vg.bound_vertex_array_->element_buffer;
        } else {
            buffer = vg.bound_buffer_targets_[slot];
        }

        if (buffer == nullptr || buffer->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (buffer->memory == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (buffer->mapped) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (static_cast<size_t>(offset + size) > buffer->memory->size()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (size > 0) {
            std::memcpy(buffer->memory->data() + offset, data, static_cast<size_t>(size));
        }
    }

    //////////////////////////////////////////////////
    // GL VERSION 2.0 API
    //////////////////////////////////////////////////
    void vgBlendEquationSeparate(VGenum modeRGB, VGenum modeAlpha) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if ((modeRGB < VG_FUNC_ADD || modeRGB > VG_MAX) || modeAlpha < VG_FUNC_ADD || modeAlpha > VG_MAX) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        vg.state_.blend_rgb_equation = modeRGB;
        vg.state_.blend_alpha_equation = modeAlpha;
    }

    void vgDrawBuffers(VGsizei n, const VGenum* bufs) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        VirtualGPU::FrameBuffer* fb = vg.bound_draw_frame_buffer_;

        if (!fb) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (n <= 0 || n > VG_DRAW_BUFFER_SLOT_COUNT) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (fb->id == 0) {
            if (n != 1) {
                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
            }
            vgDrawBuffer(bufs[0]);
            return;
        }

        for (VGsizei i = 0; i < n; i++) {
            if (bufs[i] == VG_NONE) {
                fb->draw_slot_to_color_attachment[i] = -1;
            } else if (bufs[i] >= VG_COLOR_ATTACHMENT0 && bufs[i] <= VG_COLOR_ATTACHMENT31) {
                fb->draw_slot_to_color_attachment[i] = bufs[i] - VG_COLOR_ATTACHMENT0;
            } else {
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
            }
        }

        for (VGsizei i = n; i < VG_DRAW_BUFFER_SLOT_COUNT; i++) {
            fb->draw_slot_to_color_attachment[i] = -1;
        }
    }

    void vgStencilOpSeparate(VGenum face, VGenum sfail, VGenum dpfail, VGenum dppass) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto IsValidStencilOp = [](VGenum op) {
            switch (op) {
                case VG_KEEP:
                case VG_ZERO:
                case VG_REPLACE:
                case VG_INCR:
                case VG_INCR_WRAP:
                case VG_DECR:
                case VG_DECR_WRAP:
                case VG_INVERT:
                    return true;
                default:
                    return false;
            }
        };

        if (!IsValidStencilOp(sfail) || !IsValidStencilOp(dpfail) || !IsValidStencilOp(dppass)) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        switch (face) {
            case VG_FRONT:
                vg.state_.stencil_sfail_op[0] = sfail;
                vg.state_.stencil_dpfail_op[0] = dpfail;
                vg.state_.stencil_dppass_op[0] = dppass;
                break;
            case VG_BACK:
                vg.state_.stencil_sfail_op[1] = sfail;
                vg.state_.stencil_dpfail_op[1] = dpfail;
                vg.state_.stencil_dppass_op[1] = dppass;
                break;
            case VG_FRONT_AND_BACK:
                vg.state_.stencil_sfail_op[0] = sfail;
                vg.state_.stencil_dpfail_op[0] = dpfail;
                vg.state_.stencil_dppass_op[0] = dppass;
                vg.state_.stencil_sfail_op[1] = sfail;
                vg.state_.stencil_dpfail_op[1] = dpfail;
                vg.state_.stencil_dppass_op[1] = dppass;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgStencilFuncSeparate(VGenum face, VGenum func, VGint ref, VGuint mask) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (func < VG_NEVER || VG_ALWAYS < func) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        switch (face) {
            case VG_FRONT:
                vg.state_.stencil_func[0] = func;
                vg.state_.stencil_ref[0] = ref;
                vg.state_.stencil_func_mask[0] = mask;
                break;
            case VG_BACK:
                vg.state_.stencil_func[1] = func;
                vg.state_.stencil_ref[1] = ref;
                vg.state_.stencil_func_mask[1] = mask;
                break;
            case VG_FRONT_AND_BACK:
                vg.state_.stencil_func[0] = func;
                vg.state_.stencil_ref[0] = ref;
                vg.state_.stencil_func_mask[0] = mask;
                vg.state_.stencil_func[1] = func;
                vg.state_.stencil_ref[1] = ref;
                vg.state_.stencil_func_mask[1] = mask;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgStencilMaskSeparate(VGenum face, VGuint mask) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        switch (face) {
            case VG_FRONT:
                vg.state_.stencil_write_mask[0] = mask;
                break;
            case VG_BACK:
                vg.state_.stencil_write_mask[1] = mask;
                break;
            case VG_FRONT_AND_BACK:
                vg.state_.stencil_write_mask[0] = mask;
                vg.state_.stencil_write_mask[1] = mask;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
        }
    }
    void vgAttachShader(VGuint program, VGuint shader) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto pit = vg.program_pool_.find(program);
        auto sit = vg.shader_pool_.find(shader);

        if (pit == vg.program_pool_.end() || sit == vg.shader_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::Program& prog = pit->second;
        VirtualGPU::Shader& shdr = sit->second;

        if (prog.is_deleted || shdr.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (prog.vertex_shader == &shdr || prog.fragment_shader == &shdr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        switch (shdr.type) {
            case VG_VERTEX_SHADER:
                if (prog.vertex_shader) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                prog.vertex_shader = &shdr;
                break;

            case VG_FRAGMENT_SHADER:
                if (prog.fragment_shader) {
                    vg.state_.error_state = VG_INVALID_OPERATION;
                    return;
                }
                prog.fragment_shader = &shdr;
                break;

            default:
                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
        }

        shdr.refcount++;

        prog.link_status = VG_FALSE;
    }

    void vgBindAttribLocation(VGuint program, VGuint index, const VGchar* name) { program, index, name; }  // no op
    void vgCompileShader(VGuint shader) { shader; }                                                        // no op
    VGuint vgCreateProgram(void) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return 0;
        }

        vg.base_handle_++;
        VirtualGPU::Program prog;
        prog.id = vg.base_handle_;
        vg.program_pool_.insert({vg.base_handle_, prog});
        return vg.base_handle_;
    }
    VGuint vgCreateShader(VGenum type) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return 0;
        }

        if (type != VG_VERTEX_SHADER && type != VG_FRAGMENT_SHADER) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return 0;
        }

        vg.base_handle_++;
        VirtualGPU::Shader shader;
        shader.id = vg.base_handle_;
        shader.type = type;
        vg.shader_pool_.insert({vg.base_handle_, shader});
        return vg.base_handle_;
    }
    void vgDeleteProgram(VGuint program) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (program == 0) {
            return;
        }

        auto it = vg.program_pool_.find(program);
        if (it == vg.program_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::Program& prog = it->second;

        prog.is_deleted = true;

        DestroyProgram(program);
    }
    void vgDeleteShader(VGuint shader) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (shader == 0) {
            return;
        }

        auto it = vg.shader_pool_.find(shader);
        if (it == vg.shader_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::Shader& shdr = it->second;

        shdr.is_deleted = true;

        DestroyShader(shader);
    }
    void vgDetachShader(VGuint program, VGuint shader) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) return;

        auto pit = vg.program_pool_.find(program);
        auto sit = vg.shader_pool_.find(shader);

        if (pit == vg.program_pool_.end() || sit == vg.shader_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::Program& prog = pit->second;
        VirtualGPU::Shader& shdr = sit->second;

        bool detached = false;

        switch (shdr.type) {
            case VG_VERTEX_SHADER:
                if (prog.vertex_shader == &shdr) {
                    prog.vertex_shader = nullptr;
                    detached = true;
                }
                break;

            case VG_FRAGMENT_SHADER:
                if (prog.fragment_shader == &shdr) {
                    prog.fragment_shader = nullptr;
                    detached = true;
                }
                break;

            default:
                vg.state_.error_state = VG_INVALID_VALUE;
                return;
        }

        if (!detached) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        ReleaseShader(shader);

        prog.link_status = VG_FALSE;
    }
    void vgEnableVertexAttribArray(VGuint index) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (!vg.bound_vertex_array_) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        vg.bound_vertex_array_->index_to_attrib[index].enabled = true;
    }

    void vgDisableVertexAttribArray(VGuint index) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (!vg.bound_vertex_array_) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        vg.bound_vertex_array_->index_to_attrib[index].enabled = false;
    }
    VGint vgGetUniformLocation(VGuint program, uint32_t name_hash) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return -1;
        }
        if (program > vg.base_handle_) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return -1;
        }

        auto pit = vg.program_pool_.find(program);
        if (pit == vg.program_pool_.end() || pit->second.link_status == VG_FALSE) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return -1;
        }

        VirtualGPU::Program& prog = pit->second;

        auto lit = prog.uniform_name_hash_to_location.find(name_hash);
        if (lit == prog.uniform_name_hash_to_location.end()) {
            VGint location = (VGint)prog.uniforms.size();
            prog.uniforms.emplace_back(VirtualGPU::Uniform());
            auto [new_lit, _] = prog.uniform_name_hash_to_location.insert({name_hash, location});
            lit = new_lit;
        }
        return lit->second;
    }
    VGint vgGetUniformLocation(VGuint program, const VGchar* name) {
        return vgGetUniformLocation(program, fnv1a_32(name, strlen(name)));
    }
    VGboolean vgIsProgram(VGuint program) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (program == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.program_pool_.find(program);
        if (it != vg.program_pool_.end()) {
            if (it->second.is_deleted) {
                return (VGboolean)VG_FALSE;
            } else {
                return (VGboolean)VG_TRUE;
            }
        } else {
            return (VGboolean)VG_FALSE;
        }
    }
    VGboolean vgIsShader(VGuint shader) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (shader == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.shader_pool_.find(shader);
        if (it != vg.shader_pool_.end()) {
            if (it->second.is_deleted) {
                return (VGboolean)VG_FALSE;
            } else {
                return (VGboolean)VG_TRUE;
            }
        } else {
            return (VGboolean)VG_FALSE;
        }
    }
    void vgLinkProgram(VGuint program) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (program > vg.base_handle_) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        auto it = vg.program_pool_.find(program);
        if (it == vg.program_pool_.end() || it->second.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Program& prog = it->second;

        if (prog.is_deleted || prog.vertex_shader == nullptr || prog.fragment_shader == nullptr) {
            prog.link_status = VG_FALSE;
            return;
        }

        prog.smooth_varying_count = 0;
        prog.smooth_varying_name_hashes.fill(0);
        prog.smooth_varying_descs.fill({VG_FLOAT, 0, 0});

        prog.flat_varying_count = 0;
        prog.flat_varying_name_hashes.fill(0);
        prog.flat_varying_descs.fill({VG_FLOAT, 0, 0});

        prog.uniforms.clear();
        prog.uniform_name_hash_to_location.clear();

        prog.fragout_name_hash_to_draw_buffer_slot.clear();

        it->second.link_status = VG_TRUE;
    }
    void vgShaderSource(VGuint shader, void* source) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (shader > vg.base_handle_ || source == nullptr) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        auto it = vg.shader_pool_.find(shader);
        if (it == vg.shader_pool_.end() || it->second.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        it->second.source = source;
    }
    void vgUseProgram(VGuint program) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (vg.using_program_) {
            ReleaseProgram(vg.using_program_->id);
            vg.using_program_ = nullptr;
        }

        if (program == 0) {
            return;
        }

        auto it = vg.program_pool_.find(program);
        if (it == vg.program_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::Program& prog = it->second;

        if (prog.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (prog.link_status == VG_FALSE) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        prog.refcount++;
        vg.using_program_ = &prog;
    }

    template <typename T>
    VirtualGPU::Uniform* vgUniform1t(VGint location, T v0) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (location == -1) {
            return nullptr;
        }

        if (vg.using_program_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return nullptr;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        u.data.resize(sizeof(T));
        std::memcpy(u.data.data(), &v0, sizeof(T));
        return &u;
    }

    template <typename T>
    VirtualGPU::Uniform* vgUniform2t(VGint location, T v0, T v1) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (location == -1) {
            return nullptr;
        }

        if (vg.using_program_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return nullptr;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        const T tmp[2] = {v0, v1};
        u.data.resize(sizeof(tmp));
        std::memcpy(u.data.data(), tmp, sizeof(tmp));
        return &u;
    }

    template <typename T>
    VirtualGPU::Uniform* vgUniform3t(VGint location, T v0, T v1, T v2) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (location == -1) {
            return nullptr;
        }

        if (vg.using_program_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return nullptr;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        const T tmp[3] = {v0, v1, v2};
        u.data.resize(sizeof(tmp));
        std::memcpy(u.data.data(), tmp, sizeof(tmp));
        return &u;
    }

    template <typename T>
    VirtualGPU::Uniform* vgUniform4t(VGint location, T v0, T v1, T v2, T v3) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (location == -1) {
            return nullptr;
        }

        if (vg.using_program_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return nullptr;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        const T tmp[4] = {v0, v1, v2, v3};
        u.data.resize(sizeof(tmp));
        std::memcpy(u.data.data(), tmp, sizeof(tmp));
        return &u;
    }

    template <typename T>
    VirtualGPU::Uniform* vgUniform1tv(VGint location, VGsizei count, const T* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (location == -1) {
            return nullptr;
        }

        if (vg.using_program_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return nullptr;
        }

        if (count < 0 || (count > 0 && value == nullptr)) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        u.data.resize(sizeof(T) * count);
        std::memcpy(u.data.data(), value, sizeof(T) * count);
        return &u;
    }

    template <typename T, int N>
    VirtualGPU::Uniform* vgUniformNtv(VGint location, VGsizei count, const T* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }
        if (location == -1) {
            return nullptr;
        }

        if (vg.using_program_ == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return nullptr;
        }

        if (count < 0 || (count > 0 && value == nullptr)) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        u.data.resize(count * sizeof(T) * N);
        std::memcpy(u.data.data(), value, count * sizeof(T) * N);
        return &u;
    }

    void vgUniform1f(VGint location, VGfloat v0) {
        VirtualGPU::Uniform* u = vgUniform1t(location, v0);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 1;
            u->count = 1;
        }
    }
    void vgUniform2f(VGint location, VGfloat v0, VGfloat v1) {
        VirtualGPU::Uniform* u = vgUniform2t(location, v0, v1);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 2;
            u->count = 1;
        }
    }
    void vgUniform3f(VGint location, VGfloat v0, VGfloat v1, VGfloat v2) {
        VirtualGPU::Uniform* u = vgUniform3t(location, v0, v1, v2);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 3;
            u->count = 1;
        }
    }
    void vgUniform4f(VGint location, VGfloat v0, VGfloat v1, VGfloat v2, VGfloat v3) {
        VirtualGPU::Uniform* u = vgUniform4t(location, v0, v1, v2, v3);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 4;
            u->count = 1;
        }
    }
    void vgUniform1i(VGint location, VGint v0) {
        VirtualGPU::Uniform* u = vgUniform1t(location, v0);
        if (u) {
            u->type = VG_INT;
            u->size = 1;
            u->count = 1;
        }
    }
    void vgUniform2i(VGint location, VGint v0, VGint v1) {
        VirtualGPU::Uniform* u = vgUniform2t(location, v0, v1);
        if (u) {
            u->type = VG_INT;
            u->size = 2;
            u->count = 1;
        }
    }
    void vgUniform3i(VGint location, VGint v0, VGint v1, VGint v2) {
        VirtualGPU::Uniform* u = vgUniform3t(location, v0, v1, v2);
        if (u) {
            u->type = VG_INT;
            u->size = 3;
            u->count = 1;
        }
    }
    void vgUniform4i(VGint location, VGint v0, VGint v1, VGint v2, VGint v3) {
        VirtualGPU::Uniform* u = vgUniform4t(location, v0, v1, v2, v3);
        if (u) {
            u->type = VG_INT;
            u->size = 4;
            u->count = 1;
        }
    }
    void vgUniform1fv(VGint location, VGsizei count, const VGfloat* value) {
        VirtualGPU::Uniform* u = vgUniform1tv(location, count, value);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 1;
            u->count = count;
        }
    }
    void vgUniform2fv(VGint location, VGsizei count, const VGfloat* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGfloat, 2>(location, count, value);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 2;
            u->count = count;
        }
    }
    void vgUniform3fv(VGint location, VGsizei count, const VGfloat* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGfloat, 3>(location, count, value);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 3;
            u->count = count;
        }
    }
    void vgUniform4fv(VGint location, VGsizei count, const VGfloat* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGfloat, 4>(location, count, value);
        if (u) {
            u->type = VG_FLOAT;
            u->size = 4;
            u->count = count;
        }
    }
    void vgUniform1iv(VGint location, VGsizei count, const VGint* value) {
        VirtualGPU::Uniform* u = vgUniform1tv(location, count, value);
        if (u) {
            u->type = VG_INT;
            u->size = 1;
            u->count = count;
        }
    }
    void vgUniform2iv(VGint location, VGsizei count, const VGint* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGint, 2>(location, count, value);
        if (u) {
            u->type = VG_INT;
            u->size = 2;
            u->count = count;
        }
    }
    void vgUniform3iv(VGint location, VGsizei count, const VGint* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGint, 3>(location, count, value);
        if (u) {
            u->type = VG_INT;
            u->size = 3;
            u->count = count;
        }
    }
    void vgUniform4iv(VGint location, VGsizei count, const VGint* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGint, 4>(location, count, value);
        if (u) {
            u->type = VG_INT;
            u->size = 4;
            u->count = count;
        }
    }
    void vgUniformMatrix2fv(VGint location, VGsizei count, VGboolean transpose, const VGfloat* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (location == -1) {
            return;
        }

        if (!vg.using_program_) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (count < 0 || (count > 0 && value == nullptr)) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        u.type = VG_FLOAT_MAT2;
        u.size = 4;
        u.count = count;

        u.data.resize(sizeof(VGfloat) * 4 * count);
        VGfloat* dst = reinterpret_cast<VGfloat*>(u.data.data());

        if (transpose) {
            std::memcpy(dst, value, sizeof(VGfloat) * 4 * count);
            return;
        }

        for (int i = 0; i < count; ++i) {
            const VGfloat* src = value + i * 4;
            VGfloat* d = dst + i * 4;

            d[0] = src[0];
            d[1] = src[2];
            d[2] = src[1];
            d[3] = src[3];
        }
    }
    void vgUniformMatrix3fv(VGint location, VGsizei count, VGboolean transpose, const VGfloat* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) return;
        if (location == -1) return;

        if (!vg.using_program_) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (count < 0 || (count > 0 && value == nullptr)) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        u.type = VG_FLOAT_MAT3;
        u.size = 9;
        u.count = count;

        u.data.resize(sizeof(VGfloat) * 9 * count);
        VGfloat* dst = reinterpret_cast<VGfloat*>(u.data.data());

        if (transpose) {
            std::memcpy(dst, value, sizeof(VGfloat) * 9 * count);
            return;
        }

        for (int i = 0; i < count; ++i) {
            const VGfloat* s = value + i * 9;
            VGfloat* d = dst + i * 9;

            d[0] = s[0];
            d[1] = s[3];
            d[2] = s[6];
            d[3] = s[1];
            d[4] = s[4];
            d[5] = s[7];
            d[6] = s[2];
            d[7] = s[5];
            d[8] = s[8];
        }
    }
    void vgUniformMatrix4fv(VGint location, VGsizei count, VGboolean transpose, const VGfloat* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) return;
        if (location == -1) return;

        if (!vg.using_program_) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (count < 0 || (count > 0 && value == nullptr)) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (location >= vg.using_program_->uniforms.size()) {
            // use location argument as name_hash
            auto it = vg.using_program_->uniform_name_hash_to_location.find(location);
            if (it != vg.using_program_->uniform_name_hash_to_location.end()) {
                location = it->second;
            } else {
                VGint new_loc = (VGint)vg.using_program_->uniforms.size();
                vg.using_program_->uniforms.emplace_back(VirtualGPU::Uniform());
                vg.using_program_->uniform_name_hash_to_location.insert({location, new_loc});
                location = new_loc;
            }
        }

        VirtualGPU::Uniform& u = vg.using_program_->uniforms[location];
        u.type = VG_FLOAT_MAT4;
        u.size = 16;
        u.count = count;

        u.data.resize(sizeof(VGfloat) * 16 * count);
        VGfloat* dst = reinterpret_cast<VGfloat*>(u.data.data());

        if (!transpose) {
            std::memcpy(dst, value, sizeof(VGfloat) * 16 * count);
            return;
        }

        for (int i = 0; i < count; ++i) {
            const VGfloat* s = value + i * 16;
            VGfloat* d = dst + i * 16;

            d[0] = s[0];
            d[1] = s[4];
            d[2] = s[8];
            d[3] = s[12];
            d[4] = s[1];
            d[5] = s[5];
            d[6] = s[9];
            d[7] = s[13];
            d[8] = s[2];
            d[9] = s[6];
            d[10] = s[10];
            d[11] = s[14];
            d[12] = s[3];
            d[13] = s[7];
            d[14] = s[11];
            d[15] = s[15];
        }
    }

    template <typename T>
    ALWAYS_INLINE void WriteAttrib4(VirtualGPU::ConstantAttribute& a, T x, T y, T z, T w) {
        if constexpr (std::is_same_v<T, VGdouble>) {
            a.type = VG_DOUBLE;
            a.d[0] = x;
            a.d[1] = y;
            a.d[2] = z;
            a.d[3] = w;
        } else if constexpr (std::is_same_v<T, VGfloat>) {
            a.type = VG_FLOAT;
            a.f[0] = x;
            a.f[1] = y;
            a.f[2] = z;
            a.f[3] = w;
        } else if constexpr (std::is_same_v<T, VGshort>) {
            a.type = VG_SHORT;
            a.s[0] = x;
            a.s[1] = y;
            a.s[2] = z;
            a.s[3] = w;
        } else if constexpr (std::is_same_v<T, VGushort>) {
            a.type = VG_UNSIGNED_SHORT;
            a.us[0] = x;
            a.us[1] = y;
            a.us[2] = z;
            a.us[3] = w;
        } else if constexpr (std::is_same_v<T, VGint>) {
            a.type = VG_INT;
            a.i[0] = x;
            a.i[1] = y;
            a.i[2] = z;
            a.i[3] = w;
        } else if constexpr (std::is_same_v<T, VGuint>) {
            a.type = VG_UNSIGNED_INT;
            a.u[0] = x;
            a.u[1] = y;
            a.u[2] = z;
            a.u[3] = w;
        } else if constexpr (std::is_same_v<T, VGbyte>) {
            a.type = VG_BYTE;
            a.b[0] = x;
            a.b[1] = y;
            a.b[2] = z;
            a.b[3] = w;
        } else if constexpr (std::is_same_v<T, VGubyte>) {
            a.type = VG_UNSIGNED_BYTE;
            a.ub[0] = x;
            a.ub[1] = y;
            a.ub[2] = z;
            a.ub[3] = w;
        } else {
            static_assert(sizeof(T) == 0, "unsupported vertex attrib type");
        }
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib1t(VGuint index, T x) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, x, T(0), T(0), T(1));
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib1tv(VGuint index, const T* v) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS || v == nullptr) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, v[0], T(0), T(0), T(1));
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib2t(VGuint index, T x, T y) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, x, y, T(0), T(1));
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib2tv(VGuint index, const T* v) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS || v == nullptr) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, v[0], v[1], T(0), T(1));
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib3t(VGuint index, T x, T y, T z) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, x, y, z, T(1));
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib3tv(VGuint index, const T* v) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS || v == nullptr) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, v[0], v[1], v[2], T(1));
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib4t(VGuint index, T x, T y, T z, T w) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, x, y, z, w);
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib4tv(VGuint index, const T* v) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS || v == nullptr) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = false;

        WriteAttrib4(a, v[0], v[1], v[2], v[3]);
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib4Nt(VGuint index, T x, T y, T z, T w) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = true;

        WriteAttrib4(a, x, y, z, w);
        return &a;
    }

    template <typename T>
    VirtualGPU::ConstantAttribute* vgVertexAttrib4Ntv(VGuint index, const T* v) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return nullptr;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS || v == nullptr) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return nullptr;
        }

        auto& a = vg.constant_attributes_[index];
        a.normalized = true;

        WriteAttrib4(a, v[0], v[1], v[2], v[3]);
        return &a;
    }

    void vgVertexAttrib1d(VGuint index, VGdouble x) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib1t(index, x);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib1dv(VGuint index, const VGdouble* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib1tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib1f(VGuint index, VGfloat x) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib1t(index, x);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib1fv(VGuint index, const VGfloat* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib1tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib1s(VGuint index, VGshort x) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib1t(index, x);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib1sv(VGuint index, const VGshort* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib1tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib2d(VGuint index, VGdouble x, VGdouble y) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib2t(index, x, y);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib2dv(VGuint index, const VGdouble* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib2tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib2f(VGuint index, VGfloat x, VGfloat y) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib2t(index, x, y);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib2fv(VGuint index, const VGfloat* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib2tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib2s(VGuint index, VGshort x, VGshort y) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib2t(index, x, y);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib2sv(VGuint index, const VGshort* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib2tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib3d(VGuint index, VGdouble x, VGdouble y, VGdouble z) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib3t(index, x, y, z);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib3dv(VGuint index, const VGdouble* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib3tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib3f(VGuint index, VGfloat x, VGfloat y, VGfloat z) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib3t(index, x, y, z);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib3fv(VGuint index, const VGfloat* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib3tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib3s(VGuint index, VGshort x, VGshort y, VGshort z) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib3t(index, x, y, z);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib3sv(VGuint index, const VGshort* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib3tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib4Nbv(VGuint index, const VGbyte* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4Ntv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_BYTE;
        }
    }
    void vgVertexAttrib4Niv(VGuint index, const VGint* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4Ntv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_INT;
        }
    }
    void vgVertexAttrib4Nsv(VGuint index, const VGshort* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4Ntv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib4Nub(VGuint index, VGubyte x, VGubyte y, VGubyte z, VGubyte w) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4Nt(index, x, y, z, w);
        if (attrib != nullptr) {
            attrib->type = VG_UNSIGNED_BYTE;
        }
    }
    void vgVertexAttrib4Nubv(VGuint index, const VGubyte* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4Ntv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_UNSIGNED_BYTE;
        }
    }

    void vgVertexAttrib4Nuiv(VGuint index, const VGuint* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4Ntv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_UNSIGNED_INT;
        }
    }

    void vgVertexAttrib4Nusv(VGuint index, const VGushort* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4Ntv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_UNSIGNED_SHORT;
        }
    }
    void vgVertexAttrib4bv(VGuint index, const VGbyte* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_BYTE;
        }
    }
    void vgVertexAttrib4d(VGuint index, VGdouble x, VGdouble y, VGdouble z, VGdouble w) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4t(index, x, y, z, w);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib4dv(VGuint index, const VGdouble* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_DOUBLE;
        }
    }
    void vgVertexAttrib4f(VGuint index, VGfloat x, VGfloat y, VGfloat z, VGfloat w) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4t(index, x, y, z, w);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib4fv(VGuint index, const VGfloat* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_FLOAT;
        }
    }
    void vgVertexAttrib4iv(VGuint index, const VGint* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_INT;
        }
    }
    void vgVertexAttrib4s(VGuint index, VGshort x, VGshort y, VGshort z, VGshort w) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4t(index, x, y, z, w);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib4sv(VGuint index, const VGshort* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_SHORT;
        }
    }
    void vgVertexAttrib4ubv(VGuint index, const VGubyte* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_UNSIGNED_BYTE;
        }
    }
    void vgVertexAttrib4uiv(VGuint index, const VGuint* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_UNSIGNED_INT;
        }
    }
    void vgVertexAttrib4usv(VGuint index, const VGushort* v) {
        VirtualGPU::ConstantAttribute* attrib = vgVertexAttrib4tv(index, v);
        if (attrib != nullptr) {
            attrib->type = VG_UNSIGNED_SHORT;
        }
    }
    void vgVertexAttribPointer(VGuint index, VGint size, VGenum type, VGboolean normalized, VGsizei stride,
                               const void* pointer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (!vg.bound_vertex_array_) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS || stride < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (size == VG_BGRA) {
            if (type != VG_UNSIGNED_BYTE || normalized != VG_TRUE) {
                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
            }
        } else if (size < 1 || size > 4) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (vg::GetTypeSize(type) == 0) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        uint32_t slot = vg::GetBufferSlot(VG_ARRAY_BUFFER);
        VirtualGPU::BufferObject* buf = vg.bound_buffer_targets_[slot];
        if (!buf || buf->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Attribute& attr = vg.bound_vertex_array_->index_to_attrib[index];

        if (attr.buffer) {
            ReleaseBufferObject(attr.buffer->id);
        }

        buf->refcount++;
        attr.buffer = buf;
        attr.size = size;
        attr.type = type;
        attr.normalized = normalized;
        attr.is_pure_integer = false;
        attr.stride = stride;
        attr.offset = pointer ? (VGint)((VGintptr)pointer) : 0;

        // For DrawElements call, vertex array must have vertex count to reserve varying buffer.
        uint32_t actual_stride = attr.stride == 0 ? (uint32_t)attr.size * vg::GetTypeSize(type) : (uint32_t)attr.stride;
        uint32_t vertex_count = (uint32_t)((float)buf->memory->size() / (float)actual_stride);
        // Engine policy : vertex count set minimum vertex count
        if (vertex_count < vg.bound_vertex_array_->vertex_count) {
            vg.bound_vertex_array_->vertex_count = vertex_count;
        }
    }

    //////////////////////////////////////////////////
    // GL VERSION 3.0 API
    //////////////////////////////////////////////////
    void vgColorMaski(VGuint index, VGboolean r, VGboolean g, VGboolean b, VGboolean a) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (index > VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::DrawBufferState& dbs = vg.state_.draw_buffer_states[index];
        dbs.color_mask[0] = r;
        dbs.color_mask[1] = g;
        dbs.color_mask[2] = b;
        dbs.color_mask[3] = a;
    }

    void vgEnablei(VGenum target, VGuint index) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (target != VG_BLEND) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (index > VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        vg.state_.draw_buffer_states[index].blend_enabled = true;
    }
    void vgDisablei(VGenum target, VGuint index) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (target != VG_BLEND) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (index > VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        vg.state_.draw_buffer_states[index].blend_enabled = false;
    }
    VGboolean vgIsEnabledi(VGenum target, VGuint index) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return (VGboolean)VG_FALSE;
        }

        if (target != VG_BLEND) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return (VGboolean)VG_FALSE;
        }

        if (index > VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return (VGboolean)VG_FALSE;
        }

        return vg.state_.draw_buffer_states[index].blend_enabled;
    }
    void vgBindBufferRange(VGenum target, VGuint index, VGuint buffer, VGintptr offset, VGsizeiptr size) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (target != VG_UNIFORM_BUFFER) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        auto& bindings =
            (target == VG_UNIFORM_BUFFER) ? vg.uniform_buffer_bindings_ : vg.transform_feedback_buffer_bindings_;

        // unbind
        if (buffer == 0) {
            auto it = bindings.find(index);
            if (it != bindings.end()) {
                if (it->second.buffer) {
                    ReleaseBufferObject(it->second.buffer->id);
                }
                bindings.erase(it);
            }
            return;
        }

        auto bit = vg.buffer_pool_.find(buffer);
        if (bit == vg.buffer_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::BufferObject& buf = bit->second;

        if (buf.is_deleted || !buf.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (offset < 0 || size <= 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        size_t uoff = static_cast<size_t>(offset);
        size_t usize = static_cast<size_t>(size);
        size_t memsize = buf.memory->size();

        if (uoff > memsize || usize > memsize - uoff) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        // replace binding
        auto it = bindings.find(index);
        if (it != bindings.end()) {
            if (it->second.buffer) {
                ReleaseBufferObject(it->second.buffer->id);
            }
            bindings.erase(it);
        }

        buf.refcount++;
        bindings[index] = {&buf, offset, size};
    }

    void vgBindBufferBase(VGenum target, VGuint index, VGuint buffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (buffer == 0) {
            vgBindBufferRange(target, index, 0, 0, 0);
            return;
        }

        auto it = vg.buffer_pool_.find(buffer);
        if (it == vg.buffer_pool_.end() || !it->second.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        vgBindBufferRange(target, index, buffer, 0, static_cast<VGsizeiptr>(it->second.memory->size()));
    }

    void vgVertexAttribIPointer(VGuint index, VGint size, VGenum type, VGsizei stride, const void* pointer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (!vg.bound_vertex_array_) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (index >= VG_MAX_VERTEX_ATTRIBS || stride < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (size < 1 || size > 4) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        // integer attribute only
        if (type == VG_FLOAT || type == VG_HALF_FLOAT || type == VG_DOUBLE) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        uint32_t slot = vg::GetBufferSlot(VG_ARRAY_BUFFER);
        VirtualGPU::BufferObject* buf = vg.bound_buffer_targets_[slot];
        if (!buf || buf->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Attribute& attr = vg.bound_vertex_array_->index_to_attrib[index];

        if (attr.buffer) {
            ReleaseBufferObject(attr.buffer->id);
        }

        buf->refcount++;
        attr.buffer = buf;

        attr.size = size;
        attr.type = type;
        attr.normalized = false;
        attr.is_pure_integer = true;
        attr.stride = stride;
        attr.offset = pointer ? (VGint)((VGintptr)pointer) : 0;

        // For DrawElements call, vertex array must have vertex count to reserve varying buffer.
        uint32_t actual_stride = attr.stride == 0 ? (uint32_t)attr.size * vg::GetTypeSize(type) : (uint32_t)attr.stride;
        uint32_t vertex_count = (uint32_t)((float)buf->memory->size() / (float)actual_stride);
        // Engine policy : vertex count set minimum vertex count
        if (vertex_count < vg.bound_vertex_array_->vertex_count) {
            vg.bound_vertex_array_->vertex_count = vertex_count;
        }
    }

    void vgVertexAttribI1i(VGuint index, VGint x) {
        if (auto* a = vgVertexAttrib1t<VGint>(index, x)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI2i(VGuint index, VGint x, VGint y) {
        if (auto* a = vgVertexAttrib2t<VGint>(index, x, y)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI3i(VGuint index, VGint x, VGint y, VGint z) {
        if (auto* a = vgVertexAttrib3t<VGint>(index, x, y, z)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI4i(VGuint index, VGint x, VGint y, VGint z, VGint w) {
        if (auto* a = vgVertexAttrib4t<VGint>(index, x, y, z, w)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }

    void vgVertexAttribI1ui(VGuint index, VGuint x) {
        if (auto* a = vgVertexAttrib1t<VGuint>(index, x)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI2ui(VGuint index, VGuint x, VGuint y) {
        if (auto* a = vgVertexAttrib2t<VGuint>(index, x, y)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI3ui(VGuint index, VGuint x, VGuint y, VGuint z) {
        if (auto* a = vgVertexAttrib3t<VGuint>(index, x, y, z)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI4ui(VGuint index, VGuint x, VGuint y, VGuint z, VGuint w) {
        if (auto* a = vgVertexAttrib4t<VGuint>(index, x, y, z, w)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }

    void vgVertexAttribI1iv(VGuint index, const VGint* v) {
        if (auto* a = vgVertexAttrib1tv<VGint>(index, v)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI2iv(VGuint index, const VGint* v) {
        if (auto* a = vgVertexAttrib2tv<VGint>(index, v)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI3iv(VGuint index, const VGint* v) {
        if (auto* a = vgVertexAttrib3tv<VGint>(index, v)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI4iv(VGuint index, const VGint* v) {
        if (auto* a = vgVertexAttrib4tv<VGint>(index, v)) {
            a->type = VG_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }

    void vgVertexAttribI1uiv(VGuint index, const VGuint* v) {
        if (auto* a = vgVertexAttrib1tv<VGuint>(index, v)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI2uiv(VGuint index, const VGuint* v) {
        if (auto* a = vgVertexAttrib2tv<VGuint>(index, v)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI3uiv(VGuint index, const VGuint* v) {
        if (auto* a = vgVertexAttrib3tv<VGuint>(index, v)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI4uiv(VGuint index, const VGuint* v) {
        if (auto* a = vgVertexAttrib4tv<VGuint>(index, v)) {
            a->type = VG_UNSIGNED_INT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }

    void vgVertexAttribI4bv(VGuint index, const VGbyte* v) {
        if (auto* a = vgVertexAttrib4tv<VGbyte>(index, v)) {
            a->type = VG_BYTE;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI4sv(VGuint index, const VGshort* v) {
        if (auto* a = vgVertexAttrib4tv<VGshort>(index, v)) {
            a->type = VG_SHORT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI4ubv(VGuint index, const VGubyte* v) {
        if (auto* a = vgVertexAttrib4tv(index, v)) {
            a->type = VG_UNSIGNED_BYTE;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgVertexAttribI4usv(VGuint index, const VGushort* v) {
        if (auto* a = vgVertexAttrib4tv(index, v)) {
            a->type = VG_UNSIGNED_SHORT;
            a->normalized = false;
            a->is_pure_integer = true;
        }
    }
    void vgBindFragDataLocation(VGuint program, VGuint color, uint32_t name_hash) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (program > vg.base_handle_) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        auto it = vg.program_pool_.find(program);
        if (it == vg.program_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Program& prog = it->second;

        // must be called before link
        if (prog.link_status == VG_TRUE || prog.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (color >= VG_DRAW_BUFFER_SLOT_COUNT) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        prog.fragout_name_hash_to_draw_buffer_slot[name_hash] = color;
    }

    void vgBindFragDataLocation(VGuint program, VGuint color, const VGchar* name) {
        return vgBindFragDataLocation(program, color, fnv1a_32(name, strlen(name)));
    }
    VGint vgGetFragDataLocation(VGuint program, uint32_t name_hash) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return -1;
        }

        if (program > vg.base_handle_) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return -1;
        }

        auto it = vg.program_pool_.find(program);
        if (it == vg.program_pool_.end() || it->second.link_status == VG_FALSE) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return -1;
        }

        VirtualGPU::Program& prog = it->second;

        auto loc = prog.fragout_name_hash_to_draw_buffer_slot.find(name_hash);
        if (loc == prog.fragout_name_hash_to_draw_buffer_slot.end()) {
            return -1;
        }

        return static_cast<VGint>(loc->second);
    }
    VGint vgGetFragDataLocation(VGuint program, const VGchar* name) {
        return vgGetFragDataLocation(program, fnv1a_32(name, strlen(name)));
    }
    void vgUniform1ui(VGint location, VGuint v0) {
        VirtualGPU::Uniform* u = vgUniform1t(location, v0);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 1;
            u->count = 1;
        }
    }
    void vgUniform2ui(VGint location, VGuint v0, VGuint v1) {
        VirtualGPU::Uniform* u = vgUniform2t(location, v0, v1);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 2;
            u->count = 1;
        }
    }
    void vgUniform3ui(VGint location, VGuint v0, VGuint v1, VGuint v2) {
        VirtualGPU::Uniform* u = vgUniform3t(location, v0, v1, v2);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 3;
            u->count = 1;
        }
    }
    void vgUniform4ui(VGint location, VGuint v0, VGuint v1, VGuint v2, VGuint v3) {
        VirtualGPU::Uniform* u = vgUniform4t(location, v0, v1, v2, v3);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 4;
            u->count = 1;
        }
    }
    void vgUniform1uiv(VGint location, VGsizei count, const VGuint* value) {
        VirtualGPU::Uniform* u = vgUniform1tv(location, count, value);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 1;
            u->count = count;
        }
    }
    void vgUniform2uiv(VGint location, VGsizei count, const VGuint* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGuint, 2>(location, count, value);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 2;
            u->count = count;
        }
    }
    void vgUniform3uiv(VGint location, VGsizei count, const VGuint* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGuint, 3>(location, count, value);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 3;
            u->count = count;
        }
    }
    void vgUniform4uiv(VGint location, VGsizei count, const VGuint* value) {
        VirtualGPU::Uniform* u = vgUniformNtv<VGuint, 4>(location, count, value);
        if (u) {
            u->type = VG_UNSIGNED_INT;
            u->size = 4;
            u->count = count;
        }
    }
    void vgClearBufferiv(VGenum buffer, VGint drawbuffer, const VGint* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (!value) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        switch (buffer) {
            case VG_COLOR: {
                if (drawbuffer < 0 || drawbuffer > VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0) {
                    vg.state_.error_state = VG_INVALID_VALUE;
                    return;
                }

                Color128 clear_color((float)value[0], (float)value[1], (float)value[2], (float)value[3]);

                vg.ClearColorAttachment(drawbuffer, clear_color);
                return;
            }

            case VG_STENCIL: {
                if (drawbuffer != 0) {
                    vg.state_.error_state = VG_INVALID_VALUE;
                    return;
                }

                vg.ClearDepthStencilAttachment(false, true, 0.0f, static_cast<uint8_t>(value[0]));
                return;
            }

            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }
    }

    void vgClearBufferuiv(VGenum buffer, VGint drawbuffer, const VGuint* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (!value) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (buffer != VG_COLOR) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (drawbuffer < 0 || drawbuffer > VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        Color128 clear_color((float)value[0], (float)value[1], (float)value[2], (float)value[3]);

        vg.ClearColorAttachment(drawbuffer, clear_color);
    }

    void vgClearBufferfv(VGenum buffer, VGint drawbuffer, const VGfloat* value) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (!value) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::FrameBuffer* fb = vg.bound_draw_frame_buffer_;
        if (!fb) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        switch (buffer) {
            case VG_COLOR: {
                if (drawbuffer < 0 || drawbuffer > (VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0)) {
                    vg.state_.error_state = VG_INVALID_VALUE;
                    return;
                }

                Color128 clear_color(value[0], value[1], value[2], value[3]);
                vg.ClearColorAttachment(drawbuffer, clear_color);
                return;
            }

            case VG_DEPTH: {
                if (drawbuffer != 0) {
                    vg.state_.error_state = VG_INVALID_VALUE;
                    return;
                }

                if (fb->depth_stencil_attachment.format == VG_DEPTH_COMPONENT) {
                    vg.ClearDepthAttachment(value[0]);
                    return;
                }

                if (fb->depth_stencil_attachment.format == VG_DEPTH_STENCIL) {
                    vg.ClearDepthStencilAttachment(true, false, value[0], 0);
                    return;
                }

                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
            }

            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }
    }

    void vgClearBufferfi(VGenum buffer, VGint drawbuffer, VGfloat depth, VGint stencil) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) return;

        if (buffer != VG_DEPTH_STENCIL) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (drawbuffer != 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        vg.ClearDepthStencilAttachment(true, true, depth, static_cast<uint8_t>(stencil));
    }

    VGboolean vgIsRenderbuffer(VGuint renderbuffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (renderbuffer == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.render_buffer_pool_.find(renderbuffer);
        if (it != vg.render_buffer_pool_.end()) {
            if (it->second.memory == nullptr || it->second.is_deleted) {
                return (VGboolean)VG_FALSE;
            } else {
                return (VGboolean)VG_TRUE;
            }
        } else {
            return (VGboolean)VG_FALSE;
        }
    }
    void vgBindRenderbuffer(VGenum target, VGuint renderbuffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (target != VG_RENDERBUFFER) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (vg.bound_render_buffer_) {
            ReleaseRenderBuffer(vg.bound_render_buffer_->id);
            vg.bound_render_buffer_ = nullptr;
        }

        // unbind
        if (renderbuffer == 0) {
            return;
        }

        auto it = vg.render_buffer_pool_.find(renderbuffer);
        if (it == vg.render_buffer_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::RenderBuffer& rbo = it->second;
        if (rbo.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (!rbo.memory) {
            vg.vram_.push_back(std::vector<uint8_t>());
            rbo.memory = &vg.vram_.back();
        }

        rbo.refcount++;
        vg.bound_render_buffer_ = &rbo;
    }

    void vgDeleteRenderbuffers(VGsizei n, const VGuint* renderbuffers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!renderbuffers) {
            return;
        }

        for (VGsizei i = 0; i < n; i++) {
            VGuint name = renderbuffers[i];
            if (name == 0) {
                continue;
            }

            auto it = vg.render_buffer_pool_.find(name);
            if (it == vg.render_buffer_pool_.end()) {
                continue;
            }

            it->second.is_deleted = true;
            DestroyRenderBuffer(name);
        }
    }
    void vgGenRenderbuffers(VGsizei n, VGuint* renderbuffers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!renderbuffers) {
            return;
        }
        for (int i = 0; i < n; i++) {
            vg.base_handle_++;
            vg.render_buffer_pool_.insert({vg.base_handle_, VirtualGPU::RenderBuffer()});
            vg.render_buffer_pool_[vg.base_handle_].id = vg.base_handle_;
            renderbuffers[i] = vg.base_handle_;
        }
    }
    void vgRenderbufferStorage(VGenum target, VGenum internalformat, VGsizei width, VGsizei height) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (target != VG_RENDERBUFFER) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        if (width < 0 || height < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (vg.bound_render_buffer_ == nullptr || vg.bound_render_buffer_->is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        switch (internalformat) {
            case VG_RED:
                vg.bound_render_buffer_->width = width;
                vg.bound_render_buffer_->height = height;
                vg.bound_render_buffer_->component_type = VG_UNSIGNED_BYTE;
                vg.bound_render_buffer_->format = internalformat;
                vg.bound_render_buffer_->memory->resize(width * height * 1);
                break;
            case VG_RG:
                vg.bound_render_buffer_->width = width;
                vg.bound_render_buffer_->height = height;
                vg.bound_render_buffer_->component_type = VG_UNSIGNED_BYTE;
                vg.bound_render_buffer_->format = internalformat;
                vg.bound_render_buffer_->memory->resize(width * height * 2);
                break;
            case VG_RGB:
                vg.bound_render_buffer_->width = width;
                vg.bound_render_buffer_->height = height;
                vg.bound_render_buffer_->component_type = VG_UNSIGNED_BYTE;
                vg.bound_render_buffer_->format = internalformat;
                vg.bound_render_buffer_->memory->resize(width * height * 3);
                break;
            case VG_RGBA:
                vg.bound_render_buffer_->width = width;
                vg.bound_render_buffer_->height = height;
                vg.bound_render_buffer_->component_type = VG_UNSIGNED_BYTE;
                vg.bound_render_buffer_->format = internalformat;
                vg.bound_render_buffer_->memory->resize(width * height * 4);
                break;
            case VG_DEPTH_COMPONENT:
                vg.bound_render_buffer_->width = width;
                vg.bound_render_buffer_->height = height;
                vg.bound_render_buffer_->component_type = VG_FLOAT;
                vg.bound_render_buffer_->format = internalformat;
                vg.bound_render_buffer_->memory->resize(width * height * 4);
                break;
            case VG_DEPTH_STENCIL:
                vg.bound_render_buffer_->width = width;
                vg.bound_render_buffer_->height = height;
                vg.bound_render_buffer_->component_type = VG_UNSIGNED_INT;
                vg.bound_render_buffer_->format = internalformat;
                vg.bound_render_buffer_->memory->resize(width * height * 4);
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }
    }
    VGboolean vgIsFramebuffer(VGuint framebuffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (framebuffer == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.frame_buffer_pool_.find(framebuffer);
        if (it != vg.frame_buffer_pool_.end()) {
            if (!it->second.is_bound_once || it->second.is_deleted) {
                return (VGboolean)VG_FALSE;
            } else {
                return (VGboolean)VG_TRUE;
            }
        } else {
            return (VGboolean)VG_FALSE;
        }
    }
    void vgBindFramebuffer(VGenum target, VGuint framebuffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        VirtualGPU::FrameBuffer* fb;

        auto it = vg.frame_buffer_pool_.find(framebuffer);
        if (it == vg.frame_buffer_pool_.end() || it->second.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }
        fb = &it->second;

        auto bind = [&](VirtualGPU::FrameBuffer*& slot) {
            if (slot) {
                ReleaseFrameBuffer(slot->id);
                slot = nullptr;
            }
            if (fb) {
                fb->refcount++;
                slot = fb;
                fb->is_bound_once = true;
            }
        };

        switch (target) {
            case VG_DRAW_FRAMEBUFFER:
                bind(vg.bound_draw_frame_buffer_);
                break;
            case VG_READ_FRAMEBUFFER:
                bind(vg.bound_read_frame_buffer_);
                break;
            case VG_FRAMEBUFFER:
                bind(vg.bound_draw_frame_buffer_);
                bind(vg.bound_read_frame_buffer_);
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }
    }

    void vgDeleteFramebuffers(VGsizei n, const VGuint* framebuffers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!framebuffers) {
            return;
        }

        for (VGsizei i = 0; i < n; i++) {
            VGuint name = framebuffers[i];
            if (name == 0) {
                continue;
            }

            auto it = vg.frame_buffer_pool_.find(name);
            if (it == vg.frame_buffer_pool_.end()) {
                continue;
            }

            it->second.is_deleted = true;
            DestroyFrameBuffer(name);
        }
    }
    void vgGenFramebuffers(VGsizei n, VGuint* framebuffers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!framebuffers) {
            return;
        }
        for (int i = 0; i < n; i++) {
            vg.base_handle_++;
            vg.frame_buffer_pool_.insert({vg.base_handle_, VirtualGPU::FrameBuffer()});
            vg.frame_buffer_pool_[vg.base_handle_].id = vg.base_handle_;
            framebuffers[i] = vg.base_handle_;
        }
    }
    VGenum vgCheckFramebufferStatus(VGenum target) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return VG_FRAMEBUFFER_UNDEFINED;
        }

        VirtualGPU::FrameBuffer* fb = nullptr;

        switch (target) {
            case VG_DRAW_FRAMEBUFFER:
                fb = vg.bound_draw_frame_buffer_;
                break;
            case VG_READ_FRAMEBUFFER:
                fb = vg.bound_read_frame_buffer_;
                break;
            case VG_FRAMEBUFFER:
                fb = vg.bound_draw_frame_buffer_;
                break;
            default:
                return VG_FRAMEBUFFER_UNSUPPORTED;
        }

        if (!fb) {
            return VG_FRAMEBUFFER_UNSUPPORTED;
        }

        if (fb->id == 0) {
            return VG_FRAMEBUFFER_COMPLETE;
        }

        bool has_attachment = false;
        int ref_width = -1;
        int ref_height = -1;

        // Check color attachment
        for (size_t slot = 0; slot < fb->draw_slot_to_color_attachment.size(); slot++) {
            int att_index = fb->draw_slot_to_color_attachment[slot];
            if (att_index == -1) {
                continue;
            }

            if (att_index < 0 || att_index >= (int)fb->color_attachments.size()) {
                return VG_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
            }

            const auto& att = fb->color_attachments[att_index];

            if (!att.memory || att.width <= 0 || att.height <= 0) {
                return VG_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
            }

            if (!has_attachment) {
                ref_width = att.width;
                ref_height = att.height;
                has_attachment = true;
            } else {
                if (att.width != ref_width || att.height != ref_height) {
                    return VG_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
                }
            }
        }

        // Check depth stencil attachment
        const auto& ds = fb->depth_stencil_attachment;
        if (ds.memory) {
            if (ds.width <= 0 || ds.height <= 0) {
                return VG_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
            }

            if (!has_attachment) {
                ref_width = ds.width;
                ref_height = ds.height;
                has_attachment = true;
            } else {
                if (ds.width != ref_width || ds.height != ref_height) {
                    return VG_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
                }
            }
        }

        // Must have at least one attachment
        if (!has_attachment) {
            return VG_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT;
        }

        // Everything passed
        return VG_FRAMEBUFFER_COMPLETE;
    }

    void vgFramebufferTexture1D(VGenum target, VGenum attachment, VGenum textarget, VGuint texture, VGint level) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) return;

        VirtualGPU::FrameBuffer* fb = nullptr;
        switch (target) {
            case VG_FRAMEBUFFER:
            case VG_DRAW_FRAMEBUFFER:
                fb = vg.bound_draw_frame_buffer_;
                break;
            case VG_READ_FRAMEBUFFER:
                fb = vg.bound_read_frame_buffer_;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }

        if (!fb || fb->id == 0) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }
        if (attachment == VG_STENCIL_ATTACHMENT) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        const bool is_color = (attachment >= VG_COLOR_ATTACHMENT0 && attachment <= VG_COLOR_ATTACHMENT31);
        const bool is_depth = (attachment == VG_DEPTH_ATTACHMENT || attachment == VG_DEPTH_STENCIL_ATTACHMENT);
        if (!is_color && !is_depth) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        VirtualGPU::Attachment* attch =
            is_depth ? &fb->depth_stencil_attachment : &fb->color_attachments[attachment - VG_COLOR_ATTACHMENT0];

        ReleaseAttachment(attch->ref_id);

        // unattach
        if (texture == 0) {
            *attch = VirtualGPU::Attachment();
            return;
        }

        auto it = vg.texture_pool_.find(texture);
        if (it == vg.texture_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureObject& tex = it->second;
        if (tex.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (textarget != VG_TEXTURE_1D || tex.texture_type != VG_TEXTURE_1D) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (level < 0 || level >= (VGint)tex.mipmap.size()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureLevel& lvl = tex.mipmap[level];
        if (!lvl.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        tex.refcount++;
        attch->ref_id = texture;
        attch->memory = lvl.memory;
        attch->offset = 0;
        attch->width = lvl.width;
        attch->height = 1;
        attch->format = tex.internal_format;
        attch->component_type = tex.component_type;
    }

    void vgFramebufferTexture2D(VGenum target, VGenum attachment, VGenum textarget, VGuint texture, VGint level) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        VirtualGPU::FrameBuffer* fb = nullptr;
        switch (target) {
            case VG_FRAMEBUFFER:
            case VG_DRAW_FRAMEBUFFER:
                fb = vg.bound_draw_frame_buffer_;
                break;
            case VG_READ_FRAMEBUFFER:
                fb = vg.bound_read_frame_buffer_;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }

        if (!fb || fb->id == 0) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        const bool is_color = attachment >= VG_COLOR_ATTACHMENT0 && attachment <= VG_COLOR_ATTACHMENT31;
        const bool is_depth = attachment == VG_DEPTH_ATTACHMENT || attachment == VG_DEPTH_STENCIL_ATTACHMENT;

        if (!is_color && !is_depth) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        VirtualGPU::Attachment* attch =
            is_depth ? &fb->depth_stencil_attachment : &fb->color_attachments[attachment - VG_COLOR_ATTACHMENT0];

        if (attch->ref_id != 0) {
            ReleaseAttachment(attch->ref_id);
            *attch = VirtualGPU::Attachment{};
        }

        // unattach
        if (texture == 0) {
            return;
        }

        auto it = vg.texture_pool_.find(texture);
        if (it == vg.texture_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureObject& tex = it->second;
        if (tex.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (textarget != VG_TEXTURE_2D || tex.texture_type != VG_TEXTURE_2D) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (level < 0 || level >= (VGint)tex.mipmap.size()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureLevel& lvl = tex.mipmap[level];
        if (!lvl.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        tex.refcount++;
        attch->ref_id = texture;
        attch->memory = lvl.memory;
        attch->offset = 0;
        attch->width = lvl.width;
        attch->height = lvl.height;
        attch->format = tex.internal_format;
        attch->component_type = tex.component_type;
    }

    void vgFramebufferTexture3D(VGenum target, VGenum attachment, VGenum textarget, VGuint texture, VGint level,
                                VGint zoffset) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) return;

        VirtualGPU::FrameBuffer* fb = nullptr;
        switch (target) {
            case VG_FRAMEBUFFER:
            case VG_DRAW_FRAMEBUFFER:
                fb = vg.bound_draw_frame_buffer_;
                break;
            case VG_READ_FRAMEBUFFER:
                fb = vg.bound_read_frame_buffer_;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }

        if (!fb || fb->id == 0) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        const bool is_color = attachment >= VG_COLOR_ATTACHMENT0 && attachment <= VG_COLOR_ATTACHMENT31;
        const bool is_depth = attachment == VG_DEPTH_ATTACHMENT || attachment == VG_DEPTH_STENCIL_ATTACHMENT;

        if (!is_color && !is_depth) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        VirtualGPU::Attachment* attch =
            is_depth ? &fb->depth_stencil_attachment : &fb->color_attachments[attachment - VG_COLOR_ATTACHMENT0];

        if (attch->ref_id != 0) {
            ReleaseAttachment(attch->ref_id);
            *attch = VirtualGPU::Attachment{};
        }

        // unattach
        if (texture == 0) {
            return;
        }

        auto it = vg.texture_pool_.find(texture);
        if (it == vg.texture_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureObject& tex = it->second;
        if (tex.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (textarget != VG_TEXTURE_3D || tex.texture_type != VG_TEXTURE_3D) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (level < 0 || level >= (VGint)tex.mipmap.size()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureLevel& lvl = tex.mipmap[level];

        if (zoffset < 0 || zoffset >= lvl.depth) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        if (!lvl.memory) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        uint32_t pixel_size = vg::GetPixelSize(tex.internal_format, tex.component_type);
        size_t slice_size = (size_t)lvl.width * (size_t)lvl.height * pixel_size;

        tex.refcount++;
        attch->ref_id = texture;
        attch->memory = lvl.memory;
        attch->offset = (VGsizei)(slice_size * zoffset);
        attch->width = lvl.width;
        attch->height = lvl.height;
        attch->format = tex.internal_format;
        attch->component_type = tex.component_type;
    }

    void vgFramebufferRenderbuffer(VGenum target, VGenum attachment, VGenum renderbuffertarget, VGuint renderbuffer) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (renderbuffertarget != VG_RENDERBUFFER) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        VirtualGPU::FrameBuffer* fb = nullptr;
        switch (target) {
            case VG_FRAMEBUFFER:
            case VG_DRAW_FRAMEBUFFER:
                fb = vg.bound_draw_frame_buffer_;
                break;
            case VG_READ_FRAMEBUFFER:
                fb = vg.bound_read_frame_buffer_;
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                return;
        }

        if (!fb || fb->id == 0) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        const bool is_color = attachment >= VG_COLOR_ATTACHMENT0 && attachment <= VG_COLOR_ATTACHMENT31;
        const bool is_depth = attachment == VG_DEPTH_ATTACHMENT || attachment == VG_DEPTH_STENCIL_ATTACHMENT;

        if (!is_color && !is_depth) {
            vg.state_.error_state = VG_INVALID_ENUM;
            return;
        }

        VirtualGPU::Attachment* attch =
            is_depth ? &fb->depth_stencil_attachment : &fb->color_attachments[attachment - VG_COLOR_ATTACHMENT0];

        if (attch->ref_id != 0) {
            ReleaseAttachment(attch->ref_id);
            *attch = VirtualGPU::Attachment{};
        }

        // detach
        if (renderbuffer == 0) {
            return;
        }

        auto it = vg.render_buffer_pool_.find(renderbuffer);
        if (it == vg.render_buffer_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::RenderBuffer& rb = it->second;

        if (rb.is_deleted || rb.memory == nullptr) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        rb.refcount++;
        attch->ref_id = renderbuffer;
        attch->memory = rb.memory;
        attch->offset = 0;
        attch->width = rb.width;
        attch->height = rb.height;
        attch->format = rb.format;
        attch->component_type = rb.component_type;
    }

    void vgBindVertexArray(VGuint array) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        // unbind
        if (array == 0) {
            if (vg.bound_vertex_array_) {
                ReleaseVertexArray(vg.bound_vertex_array_->id);
                vg.bound_vertex_array_ = nullptr;
            }
            return;
        }

        auto it = vg.vertex_array_pool_.find(array);
        if (it == vg.vertex_array_pool_.end() || it->second.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        // no-op
        if (vg.bound_vertex_array_ == &it->second) {
            return;
        }

        if (vg.bound_vertex_array_) {
            ReleaseVertexArray(vg.bound_vertex_array_->id);
            vg.bound_vertex_array_ = nullptr;
        }

        it->second.refcount++;
        it->second.is_bound_once = true;
        vg.bound_vertex_array_ = &it->second;
    }

    void vgDeleteVertexArrays(VGsizei n, const VGuint* arrays) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!arrays) {
            return;
        }

        for (VGsizei i = 0; i < n; ++i) {
            VGuint id = arrays[i];
            if (id == 0) {
                continue;
            }

            auto it = vg.vertex_array_pool_.find(id);
            if (it == vg.vertex_array_pool_.end()) {
                continue;
            }

            auto& vao = it->second;
            vao.is_deleted = true;

            DestroyVertexArray(id);
        }
    }

    void vgGenVertexArrays(VGsizei n, VGuint* arrays) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (n < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!arrays) {
            return;
        }
        for (int i = 0; i < n; i++) {
            vg.base_handle_++;
            vg.vertex_array_pool_.insert({vg.base_handle_, VirtualGPU::VertexArray()});
            vg.vertex_array_pool_[vg.base_handle_].id = vg.base_handle_;
            arrays[i] = vg.base_handle_;
        }
    }
    VGboolean vgIsVertexArray(VGuint array) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (array == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.vertex_array_pool_.find(array);
        if (it != vg.vertex_array_pool_.end()) {
            if (!it->second.is_bound_once || it->second.is_deleted) {
                return (VGboolean)VG_FALSE;
            } else {
                return (VGboolean)VG_TRUE;
            }
        } else {
            return (VGboolean)VG_FALSE;
        }
    }

    //////////////////////////////////////////////////
    // GL VERSION 3.2 API
    //////////////////////////////////////////////////

    void vgFramebufferTexture(VGenum target, VGenum attachment, VGuint texture, VGint level) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        // detach
        if (texture == 0) {
            VirtualGPU::FrameBuffer* fb = nullptr;
            switch (target) {
                case VG_FRAMEBUFFER:
                case VG_DRAW_FRAMEBUFFER:
                    fb = vg.bound_draw_frame_buffer_;
                    break;
                case VG_READ_FRAMEBUFFER:
                    fb = vg.bound_read_frame_buffer_;
                    break;
                default:
                    vg.state_.error_state = VG_INVALID_ENUM;
                    return;
            }
            if (!fb || fb->id == 0) {
                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
            }

            VirtualGPU::Attachment* attch =
                (attachment == VG_DEPTH_ATTACHMENT || attachment == VG_DEPTH_STENCIL_ATTACHMENT)
                    ? &fb->depth_stencil_attachment
                    : &fb->color_attachments[attachment - VG_COLOR_ATTACHMENT0];

            ReleaseAttachment(attch->ref_id);
            *attch = VirtualGPU::Attachment();
            return;
        }

        auto it = vg.texture_pool_.find(texture);
        if (it == vg.texture_pool_.end()) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::TextureObject& tex = it->second;
        if (tex.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        switch (tex.texture_type) {
            case VG_TEXTURE_1D:
                vgFramebufferTexture1D(target, attachment, VG_TEXTURE_1D, texture, level);
                return;
            case VG_TEXTURE_2D:
                vgFramebufferTexture2D(target, attachment, VG_TEXTURE_2D, texture, level);
                return;
            case VG_TEXTURE_3D:
                vgFramebufferTexture3D(target, attachment, VG_TEXTURE_3D, texture, level, 0);
                return;
            default:
                vg.state_.error_state = VG_INVALID_OPERATION;
                return;
        }
    }

    //////////////////////////////////////////////////
    // GL VERSION 3.3 API
    //////////////////////////////////////////////////
    void vgGenSamplers(VGsizei count, VGuint* samplers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }
        if (count < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!samplers) {
            return;
        }
        for (int i = 0; i < count; i++) {
            vg.base_handle_++;
            vg.sampler_pool_.insert({vg.base_handle_, VirtualGPU::Sampler()});
            vg.sampler_pool_[vg.base_handle_].id = vg.base_handle_;
            samplers[i] = vg.base_handle_;
        }
    }
    void vgDeleteSamplers(VGsizei count, const VGuint* samplers) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (count < 0) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }
        if (!samplers) {
            return;
        }

        for (VGsizei i = 0; i < count; ++i) {
            VGuint id = samplers[i];
            if (id == 0) {
                continue;
            }

            auto it = vg.sampler_pool_.find(id);
            if (it == vg.sampler_pool_.end()) {
                continue;
            }

            VirtualGPU::Sampler& smplr = it->second;

            smplr.is_deleted = true;

            DestroySampler(id);
        }
    }

    VGboolean vgIsSampler(VGuint sampler) {
        VirtualGPU& vg = VirtualGPU::GetInstance();

        if (sampler == 0u) {
            return (VGboolean)VG_FALSE;
        }
        auto it = vg.sampler_pool_.find(sampler);
        if (it != vg.sampler_pool_.end()) {
            if (it->second.is_deleted) {
                return (VGboolean)VG_FALSE;
            }
            return (VGboolean)VG_TRUE;
        } else {
            return (VGboolean)VG_FALSE;
        }
    }
    void vgBindSampler(VGuint unit, VGuint sampler) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (unit >= VG_TEXTURE_UNIT_COUNT) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        VirtualGPU::Sampler*& bound = vg.texture_units_[unit].bound_sampler;

        // unbind
        if (sampler == 0) {
            if (bound) {
                ReleaseSampler(bound->id);
                bound = nullptr;
            }
            return;
        }

        auto it = vg.sampler_pool_.find(sampler);
        if (it == vg.sampler_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Sampler& smplr = it->second;

        if (smplr.is_deleted) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        if (bound == &smplr) {
            return;
        }

        if (bound) {
            ReleaseSampler(bound->id);
            bound = nullptr;
        }

        smplr.refcount++;
        bound = &smplr;
    }

    void vgSamplerParameteri(VGuint sampler, VGenum pname, VGint param) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        auto it = vg.sampler_pool_.find(sampler);
        if (it == vg.sampler_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Sampler& ds = it->second;

        switch (pname) {
            case VG_TEXTURE_WRAP_S:
                if (param == VG_CLAMP_TO_EDGE || param == VG_CLAMP_TO_BORDER || param == VG_MIRRORED_REPEAT ||
                    param == VG_REPEAT)
                    ds.wrap_s = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            case VG_TEXTURE_WRAP_T:
                if (param == VG_CLAMP_TO_EDGE || param == VG_CLAMP_TO_BORDER || param == VG_MIRRORED_REPEAT ||
                    param == VG_REPEAT)
                    ds.wrap_t = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            case VG_TEXTURE_WRAP_R:
                if (param == VG_CLAMP_TO_EDGE || param == VG_CLAMP_TO_BORDER || param == VG_MIRRORED_REPEAT ||
                    param == VG_REPEAT)
                    ds.wrap_r = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            case VG_TEXTURE_MIN_FILTER:
                if (param >= VG_NEAREST && param <= VG_LINEAR)
                    ds.min_filter = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            case VG_TEXTURE_MAG_FILTER:
                if (param == VG_NEAREST || param == VG_LINEAR)
                    ds.mag_filter = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;
            case VG_TEXTURE_SWIZZLE_R:
                if ((param >= VG_RED && param <= VG_ALPHA) || param == VG_ONE || param == VG_ZERO)
                    ds.swizzle_r = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            case VG_TEXTURE_SWIZZLE_G:
                if ((param >= VG_RED && param <= VG_ALPHA) || param == VG_ONE || param == VG_ZERO)
                    ds.swizzle_g = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            case VG_TEXTURE_SWIZZLE_B:
                if ((param >= VG_RED && param <= VG_ALPHA) || param == VG_ONE || param == VG_ZERO)
                    ds.swizzle_b = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            case VG_TEXTURE_SWIZZLE_A:
                if ((param >= VG_RED && param <= VG_ALPHA) || param == VG_ONE || param == VG_ZERO)
                    ds.swizzle_a = param;
                else
                    vg.state_.error_state = VG_INVALID_ENUM;
                break;

            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                break;
        }
    }

    void vgSamplerParameteriv(VGuint sampler, VGenum pname, const VGint* params) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (!params) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        auto it = vg.sampler_pool_.find(sampler);
        if (it == vg.sampler_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Sampler& ds = it->second;

        switch (pname) {
            case VG_TEXTURE_SWIZZLE_RGBA:
                ds.swizzle_r = params[0];
                ds.swizzle_g = params[1];
                ds.swizzle_b = params[2];
                ds.swizzle_a = params[3];
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                break;
        }
    }

    // void vgSamplerParameterf(VGuint sampler, VGenum pname, VGfloat param) {
    //     VirtualGPU& vg = VirtualGPU::GetInstance();
    //     if (vg.state_.error_state != VG_NO_ERROR) {
    //         return;
    //     }
    //
    //     auto it = vg.sampler_pool_.find(sampler);
    //     if (it == vg.sampler_pool_.end()) {
    //         vg.state_.error_state = VG_INVALID_OPERATION;
    //         return;
    //     }
    //
    //     VirtualGPU::Sampler& ds = it->second;
    //
    //     switch (pname) {
    //         vg.state_.error_state = VG_INVALID_ENUM;
    //         break;
    //     }
    // }

    void vgSamplerParameterfv(VGuint sampler, VGenum pname, const VGfloat* params) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (vg.state_.error_state != VG_NO_ERROR) {
            return;
        }

        if (!params) {
            vg.state_.error_state = VG_INVALID_VALUE;
            return;
        }

        auto it = vg.sampler_pool_.find(sampler);
        if (it == vg.sampler_pool_.end()) {
            vg.state_.error_state = VG_INVALID_OPERATION;
            return;
        }

        VirtualGPU::Sampler& ds = it->second;

        switch (pname) {
            case VG_TEXTURE_BORDER_COLOR:
                ds.border_color = Color128(params[0], params[1], params[2], params[3]);
                break;
            default:
                vg.state_.error_state = VG_INVALID_ENUM;
                break;
        }
    }
    // ======================================================
    // Helper Implementation
    // ======================================================
    void FreeVram(std::vector<uint8_t>* mem) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (!mem) {
            return;
        }
        for (auto it = vg.vram_.begin(); it != vg.vram_.end(); ++it) {
            if (&(*it) == mem) {
                vg.vram_.erase(it);
                return;
            }
        }
    }

    void ReleaseAttachment(VGuint refid) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        if (refid == 0u) {
            return;
        }

        if (vg.texture_pool_.find(refid) != vg.texture_pool_.end()) {
            ReleaseTextureObject(refid);
            return;
        }
        if (vg.render_buffer_pool_.find(refid) != vg.render_buffer_pool_.end()) {
            ReleaseRenderBuffer(refid);
            return;
        }
    }

    void ReleaseVertexArray(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.vertex_array_pool_.find(id);
        if (it == vg.vertex_array_pool_.end()) {
            return;
        }

        VirtualGPU::VertexArray& vao = it->second;

        vao.refcount--;

        if (vao.refcount == 0 && vao.is_deleted) {
            DestroyVertexArray(id);
        }
    }

    void ReleaseBufferObject(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.buffer_pool_.find(id);
        if (it == vg.buffer_pool_.end()) {
            return;
        }

        VirtualGPU::BufferObject& buf = it->second;
        buf.refcount--;

        if (buf.refcount == 0 && buf.is_deleted) {
            DestroyBufferObject(id);
        }
    }

    void ReleaseTextureObject(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.texture_pool_.find(id);
        if (it == vg.texture_pool_.end()) {
            return;
        }

        VirtualGPU::TextureObject& tex = it->second;
        tex.refcount--;

        if (tex.refcount == 0 && tex.is_deleted) {
            DestroyTextureObject(id);
        }
    }

    void ReleaseSampler(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.sampler_pool_.find(id);
        if (it == vg.sampler_pool_.end()) {
            return;
        }

        VirtualGPU::Sampler& s = it->second;
        s.refcount--;

        if (s.refcount == 0 && s.is_deleted) {
            DestroySampler(id);
        }
    }

    void ReleaseFrameBuffer(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.frame_buffer_pool_.find(id);
        if (it == vg.frame_buffer_pool_.end()) {
            return;
        }

        VirtualGPU::FrameBuffer& fb = it->second;
        fb.refcount--;

        if (fb.refcount == 0 && fb.is_deleted) {
            DestroyFrameBuffer(id);
        }
    }

    void ReleaseRenderBuffer(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.render_buffer_pool_.find(id);
        if (it == vg.render_buffer_pool_.end()) {
            return;
        }

        VirtualGPU::RenderBuffer& rb = it->second;
        rb.refcount--;

        if (rb.refcount == 0 && rb.is_deleted) {
            DestroyRenderBuffer(id);
        }
    }

    void ReleaseShader(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.shader_pool_.find(id);
        if (it == vg.shader_pool_.end()) {
            return;
        }

        VirtualGPU::Shader& sh = it->second;
        sh.refcount--;

        if (sh.refcount == 0 && sh.is_deleted) {
            DestroyShader(id);
        }
    }

    void ReleaseProgram(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.program_pool_.find(id);
        if (it == vg.program_pool_.end()) {
            return;
        }

        VirtualGPU::Program& prog = it->second;
        prog.refcount--;

        if (prog.refcount == 0 && prog.is_deleted) {
            DestroyProgram(id);
        }
    }

    void DestroyVertexArray(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.vertex_array_pool_.find(id);
        if (it == vg.vertex_array_pool_.end()) {
            return;
        }

        VirtualGPU::VertexArray& vao = it->second;
        if (vao.refcount != 0 || !vao.is_deleted) {
            return;
        }

        for (auto& [loc, attr] : vao.index_to_attrib) {
            if (attr.buffer) {
                ReleaseBufferObject(attr.buffer->id);
                attr.buffer = nullptr;
            }
        }

        if (vao.element_buffer) {
            ReleaseBufferObject(vao.element_buffer->id);
            vao.element_buffer = nullptr;
        }

        vg.vertex_array_pool_.erase(it);
    }

    void DestroyBufferObject(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.buffer_pool_.find(id);
        if (it == vg.buffer_pool_.end()) {
            return;
        }

        VirtualGPU::BufferObject& buf = it->second;
        if (buf.refcount != 0 || !buf.is_deleted) {
            return;
        }

        FreeVram(buf.memory);
        buf.memory = nullptr;

        vg.buffer_pool_.erase(it);
    }

    void DestroyTextureObject(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.texture_pool_.find(id);
        if (it == vg.texture_pool_.end()) {
            return;
        }

        VirtualGPU::TextureObject& tex = it->second;
        if (tex.refcount != 0 || !tex.is_deleted) {
            return;
        }

        // free mip levels
        for (auto& lvl : tex.mipmap) {
            FreeVram(lvl.memory);
            lvl.memory = nullptr;
            lvl.width = lvl.height = lvl.depth = 0;
        }

        vg.texture_pool_.erase(it);
    }

    void DestroySampler(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.sampler_pool_.find(id);
        if (it == vg.sampler_pool_.end()) {
            return;
        }

        VirtualGPU::Sampler& s = it->second;
        if (s.refcount != 0 || !s.is_deleted) {
            return;
        }

        vg.sampler_pool_.erase(it);
    }

    void DestroyFrameBuffer(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.frame_buffer_pool_.find(id);
        if (it == vg.frame_buffer_pool_.end()) {
            return;
        }

        VirtualGPU::FrameBuffer& fb = it->second;
        if (fb.refcount != 0 || !fb.is_deleted) {
            return;
        }

        if (fb.id == 0) {
            return;
        }

        for (auto& att : fb.color_attachments) {
            ReleaseAttachment(att.ref_id);
            att = VirtualGPU::Attachment{};
        }

        ReleaseAttachment(fb.depth_stencil_attachment.ref_id);
        fb.depth_stencil_attachment = VirtualGPU::Attachment{};

        vg.frame_buffer_pool_.erase(it);
    }

    void DestroyRenderBuffer(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.render_buffer_pool_.find(id);
        if (it == vg.render_buffer_pool_.end()) {
            return;
        }

        VirtualGPU::RenderBuffer& rb = it->second;
        if (rb.refcount != 0 || !rb.is_deleted) {
            return;
        }

        FreeVram(rb.memory);
        rb.memory = nullptr;
        rb.width = rb.height = 0;

        vg.render_buffer_pool_.erase(it);
    }

    void DestroyShader(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.shader_pool_.find(id);
        if (it == vg.shader_pool_.end()) {
            return;
        }

        VirtualGPU::Shader& sh = it->second;
        if (sh.refcount != 0 || !sh.is_deleted) {
            return;
        }

        sh.source = nullptr;

        vg.shader_pool_.erase(it);
    }

    void DestroyProgram(VGuint id) {
        VirtualGPU& vg = VirtualGPU::GetInstance();
        auto it = vg.program_pool_.find(id);
        if (it == vg.program_pool_.end()) {
            return;
        }

        VirtualGPU::Program& prog = it->second;
        if (prog.refcount != 0 || !prog.is_deleted) {
            return;
        }

        if (prog.vertex_shader) {
            ReleaseShader(prog.vertex_shader->id);
            prog.vertex_shader = nullptr;
        }
        if (prog.fragment_shader) {
            ReleaseShader(prog.fragment_shader->id);
            prog.fragment_shader = nullptr;
        }

        prog.varying_buffer.clear();
        prog.uniforms.clear();
        prog.uniform_name_hash_to_location.clear();
        prog.fragout_name_hash_to_draw_buffer_slot.clear();

        vg.program_pool_.erase(it);
    }

}  // namespace ho