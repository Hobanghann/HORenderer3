#include "virtual_gpu.h"

#include <cstdlib>

#include "core/math/interp_funcs.h"
#include "core/math/math_funcs.h"
#include "virtual_gpu_utils.h"

namespace ho {
bool VirtualGPU::Initialize(uint8_t* color_buffer, VGsizei width,
                            VGsizei height, VGenum color_format,
                            VGsizei component_type) {
  if (color_buffer == nullptr || width <= 0 ||
      width > VG_MAX_ATTACHMENT_WIDTH || height <= 0 ||
      height > VG_MAX_ATTACHMENT_HEIGHT) {
    return false;
  }

  if (!vg::IsColorFormat(color_format) ||
      vg::GetTypeSize(component_type) == 0) {
    return false;
  }

  // Clear states
  vram_.clear();

  base_handle_ = 0;
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

  state_.line_width = 1.f;

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
  vram_.push_back(std::vector<uint8_t>(width * height * 4));
  auto* mem = &vram_.back();
  default_fb.depth_stencil_attachment.ref_id = 0;
  default_fb.depth_stencil_attachment.memory = mem;
  default_fb.depth_stencil_attachment.width = width;
  default_fb.depth_stencil_attachment.height = height;
  default_fb.depth_stencil_attachment.format = VG_DEPTH_STENCIL;
  default_fb.depth_stencil_attachment.component_type = VG_UNSIGNED_INT;
  default_fb.depth_stencil_attachment.offset = 0;

  default_fb.draw_slot_to_color_attachment.fill(-1);

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

void VirtualGPU::ClearColorAttachment(uint32_t slot,
                                      const Color128& clear_color) {
  FrameBuffer* fb = bound_draw_frame_buffer_;

  if (!fb) {
    return;
  }

  if (fb->draw_slot_to_color_attachment[slot] == -1) {
    return;
  }

  Attachment& attch =
      fb->color_attachments[fb->draw_slot_to_color_attachment[slot]];

  const int w = (int)attch.width;
  const int h = (int)attch.height;

  // viewport
  int vx0 = math::Clamp(state_.viewport.x, 0, w);
  int vy0 = math::Clamp(state_.viewport.y, 0, h);
  int vx1 = math::Clamp(state_.viewport.x + state_.viewport.width, 0, w);
  int vy1 = math::Clamp(state_.viewport.y + state_.viewport.height, 0, h);

  int x0 = vx0, y0 = vy0, x1 = vx1, y1 = vy1;

  // scissor
  if (state_.scissor_test_enabled) {
    int sx0 = math::Clamp(state_.scissor.x, 0, w);
    int sy0 = math::Clamp(state_.scissor.y, 0, h);
    int sx1 = math::Clamp(state_.scissor.x + state_.scissor.width, 0, w);
    int sy1 = math::Clamp(state_.scissor.y + state_.scissor.height, 0, h);
    x0 = math::Max(x0, sx0);
    y0 = math::Max(y0, sy0);
    x1 = math::Min(x1, sx1);
    y1 = math::Min(y1, sy1);
  }

  if (x0 >= x1 || y0 >= y1) {
    return;
  }

  uint32_t pixel_size = vg::GetPixelSize(attch.format, attch.component_type);
  uint8_t* base = attch.external_memory != nullptr ? attch.external_memory
                                                   : attch.memory->data();
  base += attch.offset;

  for (int y = y0; y < y1; y++) {
    uint8_t* row = base + (size_t(y) * attch.width + x0) * pixel_size;
    for (int x = x0; x < x1; x++) {
      vg::CopyPixel(row, (const uint8_t*)(&clear_color), attch.format,
                    attch.component_type, VG_RGBA, VG_FLOAT,
                    state_.draw_buffer_states[slot].color_mask);
      row += pixel_size;
    }
  }
}

void VirtualGPU::ClearDepthAttachment(float clear_depth) {
  VirtualGPU& vg = VirtualGPU::GetInstance();
  FrameBuffer* fb = vg.bound_draw_frame_buffer_;

  if (!fb) {
    return;
  }

  Attachment& attch = fb->depth_stencil_attachment;

  const int w = (int)attch.width;
  const int h = (int)attch.height;

  // viewport
  int vx0 = math::Clamp(state_.viewport.x, 0, w);
  int vy0 = math::Clamp(state_.viewport.y, 0, h);
  int vx1 = math::Clamp(state_.viewport.x + state_.viewport.width, 0, w);
  int vy1 = math::Clamp(state_.viewport.y + state_.viewport.height, 0, h);

  int x0 = vx0, y0 = vy0, x1 = vx1, y1 = vy1;

  // scissor
  if (state_.scissor_test_enabled) {
    int sx0 = math::Clamp(state_.scissor.x, 0, w);
    int sy0 = math::Clamp(state_.scissor.y, 0, h);
    int sx1 = math::Clamp(state_.scissor.x + state_.scissor.width, 0, w);
    int sy1 = math::Clamp(state_.scissor.y + state_.scissor.height, 0, h);
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
    uint32_t pixel_size = vg::GetPixelSize(attch.format, attch.component_type);
    uint8_t* base = attch.external_memory != nullptr ? attch.external_memory
                                                     : attch.memory->data();
    base += attch.offset;

    for (int y = y0; y < y1; y++) {
      uint8_t* row = base + (size_t(y) * attch.width + x0) * pixel_size;
      for (int x = x0; x < x1; x++) {
        vg::CopyPixel(row, (const uint8_t*)(&clear_depth), attch.format,
                      attch.component_type, VG_DEPTH_COMPONENT, VG_FLOAT);
        row += pixel_size;
      }
    }
  }
}

void VirtualGPU::ClearDepthStencilAttachment(bool is_depth_cleared,
                                             bool is_stencil_cleared,
                                             float clear_depth,
                                             uint8_t clear_stencil) {
  FrameBuffer* fb = bound_draw_frame_buffer_;

  if (!fb) {
    return;
  }

  Attachment& attch = fb->depth_stencil_attachment;

  if (attch.format != VG_DEPTH_STENCIL) {
    return;
  }

  const int w = (int)attch.width;
  const int h = (int)attch.height;

  // viewport
  int vx0 = math::Clamp(state_.viewport.x, 0, w);
  int vy0 = math::Clamp(state_.viewport.y, 0, h);
  int vx1 = math::Clamp(state_.viewport.x + state_.viewport.width, 0, w);
  int vy1 = math::Clamp(state_.viewport.y + state_.viewport.height, 0, h);

  int x0 = vx0, y0 = vy0, x1 = vx1, y1 = vy1;

  // scissor
  if (state_.scissor_test_enabled) {
    int sx0 = math::Clamp(state_.scissor.x, 0, w);
    int sy0 = math::Clamp(state_.scissor.y, 0, h);
    int sx1 = math::Clamp(state_.scissor.x + state_.scissor.width, 0, w);
    int sy1 = math::Clamp(state_.scissor.y + state_.scissor.height, 0, h);
    x0 = math::Max(x0, sx0);
    y0 = math::Max(y0, sy0);
    x1 = math::Min(x1, sx1);
    y1 = math::Min(y1, sy1);
  }

  if (x0 >= x1 || y0 >= y1) {
    return;
  }

  uint8_t* base = attch.external_memory != nullptr ? attch.external_memory
                                                   : attch.memory->data();
  base += attch.offset;

  const uint8_t stencil_mask =
      static_cast<uint8_t>(state_.stencil_write_mask[0]);

  for (int y = y0; y < y1; y++) {
    uint8_t* row = base + (size_t(y) * attch.width + x0) * 4;
    for (int x = x0; x < x1; x++) {
      VGfloat depth;
      uint8_t stencil;

      vg::DecodeDepthStencil(&depth, &stencil, row);

      if (is_depth_cleared) {
        depth = clear_depth;
      }

      if (is_stencil_cleared) {
        stencil =
            (clear_stencil & ~stencil_mask) | (clear_stencil & stencil_mask);
      }

      vg::EncodeDepthStencil(row, depth, stencil);

      row += 4;
    }
  }
}

void VirtualGPU::VSJobEntry(void* input, uint32_t size) {
  assert(size == sizeof(VSJobInput));
  VSJobInput* in = static_cast<VSJobInput*>(input);
  VirtualGPU& vg = VirtualGPU::GetInstance();
  for (uint32_t i = in->first_index; i <= in->last_index; i++) {
    in->vs(i, vg.using_program_->varying_buffer[i - in->base_index]);
  }
  delete in;
}

std::vector<VirtualGPU::Varying> VirtualGPU::Clip(
    const std::vector<Varying>& polygon) const {
  std::vector<Varying> out_polygon = polygon;
  // Sutherland-Hodgman Polygon Clipping Loop
  for (PlanePos plane_pos :
       {VG_PLANE_POS_LEFT, VG_PLANE_POS_RIGHT, VG_PLANE_POS_BOTTOM,
        VG_PLANE_POS_TOP, VG_PLANE_POS_NEAR, VG_PLANE_POS_FAR}) {
    out_polygon = ClipAgainstPlane(out_polygon, plane_pos);
    if (out_polygon.empty()) {
      // When polygon is totally out of frustum.
      break;
    }
  }
  return out_polygon;
}

std::vector<VirtualGPU::Varying> VirtualGPU::ClipAgainstPlane(
    const std::vector<Varying>& polygon, PlanePos plane_pos) const {
  std::vector<Varying> out_polygon;
  if (polygon.empty()) {
    return out_polygon;
  }

  const int v_count = (int)polygon.size();

  for (int i = 0; i < v_count; i++) {
    const Varying& prev_v = polygon[(i - 1 + v_count) % v_count];
    const Varying& curr_v = polygon[i];

    bool is_prev_in = IsInside(prev_v.clip_coord, plane_pos);
    bool is_curr_in = IsInside(curr_v.clip_coord, plane_pos);

    if (is_prev_in && is_curr_in) {
      // Case 1: in to in
      out_polygon.push_back(curr_v);
    } else if (is_prev_in && !is_curr_in) {
      // Case 2: in to out
      Vector2 bary =
          GetClipBarycentric(prev_v.clip_coord, curr_v.clip_coord, plane_pos);
      // if clip isn't parallel on plane or degenerated push interpolated
      // varying.
      if (!math::IsNaN(bary.x) && !math::IsNaN(bary.y)) {
        out_polygon.push_back(LerpVarying(prev_v, curr_v, bary));
      }
    } else if (!is_prev_in && is_curr_in) {
      // Case 3: out to in
      Vector2 bary =
          GetClipBarycentric(prev_v.clip_coord, curr_v.clip_coord, plane_pos);
      // if clip isn't parallel on plane or degenerated push interpolated
      // varying.
      if (!math::IsNaN(bary.x) && !math::IsNaN(bary.y)) {
        out_polygon.push_back(LerpVarying(prev_v, curr_v, bary));
      }
      out_polygon.push_back(curr_v);
    } else {
      // Case 4: out to out
      // Do Nothing.
    }
  }

  return out_polygon;
}

real VirtualGPU::EvalFrustumPlane(const Vector4& clip_coord,
                                  PlanePos plane_pos) const {
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
  return EvalFrustumPlane(clip_coord, plane_pos) >
         -math::EPSILON_POINT_ON_PLANE;
}

Vector2 VirtualGPU::GetClipBarycentric(const Vector4& clip_coord1,
                                       const Vector4& clip_coord2,
                                       PlanePos plane_pos) const {
  real e1 = EvalFrustumPlane(clip_coord1, plane_pos);
  real e2 = EvalFrustumPlane(clip_coord2, plane_pos);
  real denom = e1 - e2;
  if (math::IsZeroApprox(denom)) {
    return Vector2(math::REAL_NaN, math::REAL_NaN);
  }

  real s = e1 / denom;
  s = math::Clamp(s, 0.0_r, 1.0_r);
  return Vector2(1.0_r - s, s);
}

VirtualGPU::Varying VirtualGPU::LerpVarying(const Varying& v1,
                                            const Varying& v2,
                                            const Vector2& barycentric) const {
  Varying v;
  v.clip_coord = math::Lerp(v1.clip_coord, v2.clip_coord, barycentric);
  v.world_pos = math::Lerp(v1.world_pos, v2.world_pos, barycentric);
  v.view_pos = math::Lerp(v1.view_pos, v2.view_pos, barycentric);
  v.normal = math::Lerp(v1.normal, v2.normal, barycentric);
  v.tangent = math::Lerp(v1.tangent, v2.tangent, barycentric);
  v.uv0 = math::Lerp(v1.uv0, v2.uv0, barycentric);
  v.uv1 = math::Lerp(v1.uv1, v2.uv1, barycentric);
  v.color0 = math::Lerp(v1.color0, v2.color0, barycentric);
  v.color1 = math::Lerp(v1.color1, v2.color1, barycentric);

  return v;
}

void VirtualGPU::PerspectiveDivide(Varying& v) const {
  v.ndc = v.clip_coord.ToCartesian();
}
void VirtualGPU::ViewportTransform(Varying& v) const {
  const Rect& vp = state_.viewport;

  real half_width = vp.width * 0.5_r;
  real x = ((v.ndc.x * half_width) + half_width + vp.x);

  real half_height = vp.height * 0.5_r;
  real y = -(v.ndc.y * half_height) + half_height + vp.y;

  real z = (v.ndc.z * (state_.max_depth - state_.min_depth) +
            (state_.max_depth + state_.min_depth)) *
           0.5_r;

  v.viewport_coord = Vector3(x, y, z);
}

std::vector<VirtualGPU::Fragment> VirtualGPU::Rasterize(const Varying& v) {
  std::vector<Fragment> out;
  Fragment frag;
  frag.screen_coord = Vector2(math::Floor(v.viewport_coord.x) + 0.5_r,
                              math::Floor(v.viewport_coord.y) + 0.5_r);
  frag.depth = v.viewport_coord.z;
  frag.depth_slope = 0.0_r;

  if (ScissorTest(frag.screen_coord.x, frag.screen_coord.y) &&
      TestDepthStencil(frag.screen_coord.x, frag.screen_coord.y, frag.depth,
                       true)) {
    frag.world_pos = v.world_pos;
    frag.view_pos = v.view_pos;
    frag.normal = v.normal;
    frag.tangent = v.tangent;
    frag.uv0 = v.uv0;
    frag.uv1 = v.uv1;
    frag.color0 = v.color0;
    frag.color1 = v.color1;
    out.push_back(frag);
  }
  return out;
}

std::vector<VirtualGPU::Fragment> VirtualGPU::Rasterize(const Varying& v1,
                                                        const Varying& v2) {
  std::vector<Fragment> out;

  int x0 = (int)math::Floor(v1.viewport_coord.x);
  int y0 = (int)math::Floor(v1.viewport_coord.y);
  int x1 = (int)math::Floor(v2.viewport_coord.x);
  int y1 = (int)math::Floor(v2.viewport_coord.y);

  int dx = math::Abs(x1 - x0);
  int dy = math::Abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  if (dx == 0 && dy == 0) {
    return out;
  }

  out.reserve(dx + dy + 1);

  //  calculate gradient
  real Dx = real(x1 - x0);
  real Dy = real(y1 - y0);
  real dd = Dx * Dx + Dy * Dy;

  real gx = Dx / dd;
  real gy = Dy / dd;

  // set component for incremental perspective correct interpolation
  real inv_w1 = 1.0_r / v1.clip_coord.w;
  real inv_w2 = 1.0_r / v2.clip_coord.w;

  real inv_w = inv_w1;
  real inv_w_dx = (inv_w2 - inv_w1) * gx * (real)sx;
  real inv_w_dy = (inv_w2 - inv_w1) * gy * (real)sy;

  real depth_pw = v1.viewport_coord.z * inv_w1;
  real depth_pw_dx = (v2.viewport_coord.z * inv_w2 - depth_pw) * gx * (real)sx;
  real depth_pw_dy = (v2.viewport_coord.z * inv_w2 - depth_pw) * gy * (real)sy;

  Vector3 world_pw = v1.world_pos * inv_w1;
  Vector3 world_pw_dx = (v2.world_pos * inv_w2 - world_pw) * gx * (real)sx;
  Vector3 world_pw_dy = (v2.world_pos * inv_w2 - world_pw) * gy * (real)sy;

  Vector3 view_pw = v1.view_pos * inv_w1;
  Vector3 view_pw_dx = (v2.view_pos * inv_w2 - view_pw) * gx * (real)sx;
  Vector3 view_pw_dy = (v2.view_pos * inv_w2 - view_pw) * gy * (real)sy;

  Vector3 normal_pw = v1.normal * inv_w1;
  Vector3 normal_pw_dx = (v2.normal * inv_w2 - normal_pw) * gx * (real)sx;
  Vector3 normal_pw_dy = (v2.normal * inv_w2 - normal_pw) * gy * (real)sy;

  Vector3 tangent_pw = Vector3(v1.tangent) * inv_w1;
  Vector3 tangent_pw_dx =
      (Vector3(v2.tangent) * inv_w2 - tangent_pw) * gx * (real)sx;
  Vector3 tangent_pw_dy =
      (Vector3(v2.tangent) * inv_w2 - tangent_pw) * gy * (real)sy;

  Vector2 uv0_pw = v1.uv0 * inv_w1;
  Vector2 uv0_pw_dx = (v2.uv0 * inv_w2 - uv0_pw) * gx * (real)sx;
  Vector2 uv0_pw_dy = (v2.uv0 * inv_w2 - uv0_pw) * gy * (real)sy;

  Vector2 uv1_pw = v1.uv1 * inv_w1;
  Vector2 uv1_pw_dx = (v2.uv1 * inv_w2 - uv1_pw) * gx * (real)sx;
  Vector2 uv1_pw_dy = (v2.uv1 * inv_w2 - uv1_pw) * gy * (real)sy;

  Color128 color0_pw = v1.color0 * inv_w1;
  Color128 color0_pw_dx = (v2.color0 * inv_w2 - color0_pw) * gx * (real)sx;
  Color128 color0_pw_dy = (v2.color0 * inv_w2 - color0_pw) * gy * (real)sy;

  Color128 color1_pw = v1.color1 * inv_w1;
  Color128 color1_pw_dx = (v2.color1 * inv_w2 - color1_pw) * gx * (real)sx;
  Color128 color1_pw_dy = (v2.color1 * inv_w2 - color1_pw) * gy * (real)sy;

  // Bresenham loop
  int x = x0, y = y0;

  while (true) {
    Vector2 screen_coord(x + 0.5_r, y + 0.5_r);
    real w = 1.0_r / inv_w;
    real depth = depth_pw * w;

    if (ScissorTest(screen_coord.x, screen_coord.y) &&
        TestDepthStencil(screen_coord.x, screen_coord.y, depth, true)) {
      Fragment frag;
      frag.screen_coord = screen_coord;
      frag.depth = depth;
      frag.depth_slope = 0;

      frag.world_pos = world_pw * w;
      frag.view_pos = view_pw * w;
      frag.normal = (normal_pw * w).Normalized();

      Vector3 t = tangent_pw * w;
      frag.tangent = Vector4(t.x, t.y, t.z, v1.tangent.w);

      frag.uv0 = uv0_pw * w;
      frag.uv1 = uv1_pw * w;
      frag.color0 = color0_pw * w;
      frag.color1 = color1_pw * w;

      out.push_back(frag);
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
      depth_pw += depth_pw_dx;
      world_pw += world_pw_dx;
      view_pw += view_pw_dx;
      normal_pw += normal_pw_dx;
      tangent_pw += tangent_pw_dx;
      uv0_pw += uv0_pw_dx;
      uv1_pw += uv1_pw_dx;
      color0_pw += color0_pw_dx;
      color1_pw += color1_pw_dx;
    }
    if (step_y) {
      inv_w += inv_w_dy;
      depth_pw += depth_pw_dy;
      world_pw += world_pw_dy;
      view_pw += view_pw_dy;
      normal_pw += normal_pw_dy;
      tangent_pw += tangent_pw_dy;
      uv0_pw += uv0_pw_dy;
      uv1_pw += uv1_pw_dy;
      color0_pw += color0_pw_dy;
      color1_pw += color1_pw_dy;
    }
  }
  return out;
}

std::vector<VirtualGPU::Fragment> VirtualGPU::Rasterize(const Varying& v1,
                                                        const Varying& v2,
                                                        const Varying& v3) {
  // Edge Function and Incremental Perspective Correct Interpolation
  std::vector<Fragment> out;

  BoundingBox2D b =
      BoundingBox2D(Vector2(v1.viewport_coord), Vector2(v2.viewport_coord),
                    Vector2(v3.viewport_coord));

  const Vector2 p0(math::Ceil(b.min.x - 0.5_r) + 0.5_r,
                   math::Ceil(b.min.y - 0.5_r) + 0.5_r);

  EdgeFunction ef12(Vector2(v1.viewport_coord), Vector2(v2.viewport_coord), p0);
  bool ef12_is_topleft =
      (v2.viewport_coord.y < v1.viewport_coord.y) ||
      (math::IsEqualApprox(v2.viewport_coord.y, v1.viewport_coord.y) &&
       v2.viewport_coord.x > v1.viewport_coord.x);

  EdgeFunction ef23(Vector2(v2.viewport_coord), Vector2(v3.viewport_coord), p0);
  bool ef23_is_topleft =
      (v3.viewport_coord.y < v2.viewport_coord.y) ||
      (math::IsEqualApprox(v3.viewport_coord.y, v2.viewport_coord.y) &&
       v3.viewport_coord.x > v2.viewport_coord.x);

  EdgeFunction ef31(Vector2(v3.viewport_coord), Vector2(v1.viewport_coord), p0);
  bool ef31_is_topleft =
      (v1.viewport_coord.y < v3.viewport_coord.y) ||
      (math::IsEqualApprox(v1.viewport_coord.y, v3.viewport_coord.y) &&
       v1.viewport_coord.x > v3.viewport_coord.x);

  real area = (v2.viewport_coord.x - v1.viewport_coord.x) *
                  (v3.viewport_coord.y - v1.viewport_coord.y) -
              (v2.viewport_coord.y - v1.viewport_coord.y) *
                  (v3.viewport_coord.x - v1.viewport_coord.x);

  if (math::IsZeroApprox(area)) {
    // degenerate case
    return out;
  }

  // face culling
  bool is_front = (state_.front_face == VG_CCW) ? (area < 0) : (area > 0);
  if (state_.cull_enabled) {
    switch (state_.cull_face) {
      case VG_BACK:
        if (!is_front) return out;
        break;
      case VG_FRONT:
        if (is_front) return out;
        break;
      case VG_FRONT_AND_BACK:
        return out;
    }
  }

  // polygon offset slope
  real depth_slope = ComputeDepthSlope(v1.viewport_coord, v2.viewport_coord,
                                       v3.viewport_coord);

  const real inv_area = 1.0_r / area;

  // set component for incremental perspective correct interpolation
  const real iw1 = 1.0_r / v1.clip_coord.w;
  const real iw2 = 1.0_r / v2.clip_coord.w;
  const real iw3 = 1.0_r / v3.clip_coord.w;

  const real zpw1 = v1.viewport_coord.z * iw1;
  const real zpw2 = v2.viewport_coord.z * iw2;
  const real zpw3 = v3.viewport_coord.z * iw3;

  const Vector3 world_pw1 = v1.world_pos * iw1;
  const Vector3 world_pw2 = v2.world_pos * iw2;
  const Vector3 world_pw3 = v3.world_pos * iw3;

  const Vector3 view_pw1 = v1.view_pos * iw1;
  const Vector3 view_pw2 = v2.view_pos * iw2;
  const Vector3 view_pw3 = v3.view_pos * iw3;

  const Vector3 normal_pw1 = v1.normal * iw1;
  const Vector3 normal_pw2 = v2.normal * iw2;
  const Vector3 normal_pw3 = v3.normal * iw3;

  const Vector3 tangent_pw1 = Vector3(v1.tangent) * iw1;
  const Vector3 tangent_pw2 = Vector3(v2.tangent) * iw2;
  const Vector3 tangent_pw3 = Vector3(v3.tangent) * iw3;

  const Vector2 uv0_pw1 = v1.uv0 * iw1;
  const Vector2 uv0_pw2 = v2.uv0 * iw2;
  const Vector2 uv0_pw3 = v3.uv0 * iw3;

  const Vector2 uv1_pw1 = v1.uv1 * iw1;
  const Vector2 uv1_pw2 = v2.uv1 * iw2;
  const Vector2 uv1_pw3 = v3.uv1 * iw3;

  const Color128 c0_pw1 = v1.color0 * iw1;
  const Color128 c0_pw2 = v2.color0 * iw2;
  const Color128 c0_pw3 = v3.color0 * iw3;

  const Color128 c1_pw1 = v1.color1 * iw1;
  const Color128 c1_pw2 = v2.color1 * iw2;
  const Color128 c1_pw3 = v3.color1 * iw3;

  // initial edge values at start pixel center of bounding box
  real f12_row = ef12.initial_value;
  real f23_row = ef23.initial_value;
  real f31_row = ef31.initial_value;

  // per-pixel x increments for edge functions are ef*.dx, per-row y increments
  // are ef*.dy
  const real invw_dx =
      (ef23.dx * iw1 + ef31.dx * iw2 + ef12.dx * iw3) * inv_area;
  const real invw_dy =
      (ef23.dy * iw1 + ef31.dy * iw2 + ef12.dy * iw3) * inv_area;

  const real zpw_dx =
      (ef23.dx * zpw1 + ef31.dx * zpw2 + ef12.dx * zpw3) * inv_area;
  const real zpw_dy =
      (ef23.dy * zpw1 + ef31.dy * zpw2 + ef12.dy * zpw3) * inv_area;

  const Vector3 world_dx =
      (ef23.dx * world_pw1 + ef31.dx * world_pw2 + ef12.dx * world_pw3) *
      inv_area;
  const Vector3 world_dy =
      (ef23.dy * world_pw1 + ef31.dy * world_pw2 + ef12.dy * world_pw3) *
      inv_area;

  const Vector3 view_dx =
      (ef23.dx * view_pw1 + ef31.dx * view_pw2 + ef12.dx * view_pw3) * inv_area;
  const Vector3 view_dy =
      (ef23.dy * view_pw1 + ef31.dy * view_pw2 + ef12.dy * view_pw3) * inv_area;

  const Vector3 normal_dx =
      (ef23.dx * normal_pw1 + ef31.dx * normal_pw2 + ef12.dx * normal_pw3) *
      inv_area;
  const Vector3 normal_dy =
      (ef23.dy * normal_pw1 + ef31.dy * normal_pw2 + ef12.dy * normal_pw3) *
      inv_area;

  const Vector3 tangent_dx =
      (ef23.dx * tangent_pw1 + ef31.dx * tangent_pw2 + ef12.dx * tangent_pw3) *
      inv_area;
  const Vector3 tangent_dy =
      (ef23.dy * tangent_pw1 + ef31.dy * tangent_pw2 + ef12.dy * tangent_pw3) *
      inv_area;

  const Vector2 uv0_dx =
      (ef23.dx * uv0_pw1 + ef31.dx * uv0_pw2 + ef12.dx * uv0_pw3) * inv_area;
  const Vector2 uv0_dy =
      (ef23.dy * uv0_pw1 + ef31.dy * uv0_pw2 + ef12.dy * uv0_pw3) * inv_area;

  const Vector2 uv1_dx =
      (ef23.dx * uv1_pw1 + ef31.dx * uv1_pw2 + ef12.dx * uv1_pw3) * inv_area;
  const Vector2 uv1_dy =
      (ef23.dy * uv1_pw1 + ef31.dy * uv1_pw2 + ef12.dy * uv1_pw3) * inv_area;

  const Color128 c0_dx =
      (c0_pw1 * ef23.dx + c0_pw2 * ef31.dx + c0_pw3 * ef12.dx) * inv_area;

  const Color128 c0_dy =
      (c0_pw1 * ef23.dy + c0_pw2 * ef31.dy + c0_pw3 * ef12.dy) * inv_area;

  const Color128 c1_dx =
      (c1_pw1 * ef23.dx + c1_pw2 * ef31.dx + c1_pw3 * ef12.dx) * inv_area;

  const Color128 c1_dy =
      (c1_pw1 * ef23.dy + c1_pw2 * ef31.dy + c1_pw3 * ef12.dy) * inv_area;

  // initial row accumulators for inv_w and attributes at p0
  real invw_row = (f23_row * iw1 + f31_row * iw2 + f12_row * iw3) * inv_area;
  real zpw_row = (f23_row * zpw1 + f31_row * zpw2 + f12_row * zpw3) * inv_area;

  Vector3 world_row =
      (f23_row * world_pw1 + f31_row * world_pw2 + f12_row * world_pw3) *
      inv_area;
  Vector3 view_row =
      (f23_row * view_pw1 + f31_row * view_pw2 + f12_row * view_pw3) * inv_area;
  Vector3 normal_row =
      (f23_row * normal_pw1 + f31_row * normal_pw2 + f12_row * normal_pw3) *
      inv_area;
  Vector3 tangent_row =
      (f23_row * tangent_pw1 + f31_row * tangent_pw2 + f12_row * tangent_pw3) *
      inv_area;
  Vector2 uv0_row =
      (f23_row * uv0_pw1 + f31_row * uv0_pw2 + f12_row * uv0_pw3) * inv_area;
  Vector2 uv1_row =
      (f23_row * uv1_pw1 + f31_row * uv1_pw2 + f12_row * uv1_pw3) * inv_area;

  Color128 c0_row =
      (c0_pw1 * f23_row + c0_pw2 * f31_row + c0_pw3 * f12_row) * inv_area;
  Color128 c1_row =
      (c1_pw1 * f23_row + c1_pw2 * f31_row + c1_pw3 * f12_row) * inv_area;

  // min include, max exclude
  const int x_min = (int)math::Ceil(b.min.x - 0.5_r);
  const int x_max = (int)math::Floor(b.max.x - 0.5_r) + 1;
  const int y_min = (int)math::Ceil(b.min.y - 0.5_r);
  const int y_max = (int)math::Floor(b.max.y - 0.5_r) + 1;

  out.reserve((x_max - x_min) * (y_max - y_min) / 2);
  // Raster loop
  for (int y = y_min; y < y_max; ++y) {
    real f12_ev = f12_row;
    real f23_ev = f23_row;
    real f31_ev = f31_row;

    real invw = invw_row;
    real zpw = zpw_row;

    Vector3 world = world_row;
    Vector3 view = view_row;
    Vector3 normal = normal_row;
    Vector3 tangent = tangent_row;
    Vector2 uv0 = uv0_row;
    Vector2 uv1 = uv1_row;
    Color128 c0 = c0_row;
    Color128 c1 = c1_row;

    for (int x = x_min; x < x_max; ++x) {
      bool inside12 =
          (f12_ev > 0) || (ef12_is_topleft && math::IsZeroApprox(f12_ev));
      bool inside23 =
          (f23_ev > 0) || (ef23_is_topleft && math::IsZeroApprox(f23_ev));
      bool inside31 =
          (f31_ev > 0) || (ef31_is_topleft && math::IsZeroApprox(f31_ev));

      if (inside12 && inside23 && inside31) {
        Vector2 target_coord(real(x) + 0.5_r, real(y) + 0.5_r);

        real w = 1.0_r / invw;
        real depth = zpw * w;

        if (ScissorTest(target_coord.x, target_coord.y) &&
            TestDepthStencil(target_coord.x, target_coord.y, depth, is_front)) {
          Fragment frag;
          frag.screen_coord = target_coord;
          frag.depth = depth;
          frag.depth_slope = depth_slope;

          frag.world_pos = world * w;
          frag.view_pos = view * w;

          frag.normal = (normal * w).Normalized();

          Vector3 t = tangent * w;
          frag.tangent = Vector4(t.x, t.y, t.z, v1.tangent.w);

          frag.uv0 = uv0 * w;
          frag.uv1 = uv1 * w;

          frag.color0 = c0 * w;
          frag.color1 = c1 * w;

          frag.is_front = is_front;
          out.push_back(frag);
        }
      }

      // +1 in x: advance edge and attributes
      f12_ev += ef12.dx;
      f23_ev += ef23.dx;
      f31_ev += ef31.dx;

      invw += invw_dx;
      zpw += zpw_dx;
      world += world_dx;
      view += view_dx;
      normal += normal_dx;
      tangent += tangent_dx;
      uv0 += uv0_dx;
      uv1 += uv1_dx;
      c0 += c0_dx;
      c1 += c1_dx;
    }

    // +1 in y: advance row start edge and row start attributes
    f12_row += ef12.dy;
    f23_row += ef23.dy;
    f31_row += ef31.dy;

    invw_row += invw_dy;
    zpw_row += zpw_dy;
    world_row += world_dy;
    view_row += view_dy;
    normal_row += normal_dy;
    tangent_row += tangent_dy;
    uv0_row += uv0_dy;
    uv1_row += uv1_dy;
    c0_row += c0_dy;
    c1_row += c1_dy;
  }

  return out;
}

bool VirtualGPU::ScissorTest(real x, real y) const {
  if (!state_.scissor_test_enabled) {
    return true;
  }
  const int px = (int)math::Floor(x), py = (int)math::Floor(y);
  const auto& vp = state_.viewport;
  if (px < (int)vp.x || py < (int)vp.y || px >= (int)(vp.x + vp.width) ||
      py >= (int)(vp.y + vp.height))
    return false;

  const auto& r = state_.scissor;
  return (px >= (int)r.x && px < (int)(r.x + r.width) && py >= (int)r.y &&
          py < (int)(r.y + r.height));
}

real VirtualGPU::ComputeDepthSlope(const Vector3& v0, const Vector3& v1,
                                   const Vector3& v2) const {
  real denom = (v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y);
  if (math::IsZeroApprox(denom)) return 0.0_r;  // degenerate triangle

  real dzdx =
      ((v1.z - v0.z) * (v2.y - v0.y) - (v2.z - v0.z) * (v1.y - v0.y)) / denom;
  real dzdy =
      ((v2.z - v0.z) * (v1.x - v0.x) - (v1.z - v0.z) * (v2.x - v0.x)) / denom;

  return math::Max(math::Abs(dzdx), math::Abs(dzdy));
}

VGfloat VirtualGPU::ApplyDepthOffset(VGfloat depth, VGfloat depth_slope,
                                     uint32_t depth_bits,
                                     VGenum primitive_mode) const {
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
  }

  if (!enabled) {
    return depth;
  }

  VGfloat r = 1.0_r / (float)(1 << depth_bits);

  VGfloat bias = depth_slope * state_.depth_factor + r * state_.depth_unit;
  return std::clamp(depth + bias, 0.0_r, 1.0_r);
}

bool VirtualGPU::TestDepthStencil(VGfloat x, VGfloat y, VGfloat depth,
                                  bool is_front_face) {
  FrameBuffer* fb = bound_draw_frame_buffer_;
  Attachment& attch = fb->depth_stencil_attachment;
  if (!attch.memory) {
    return true;
  }

  VGsizei px = (VGsizei)math::Floor(x);
  VGsizei py = (VGsizei)math::Floor(y);
  if (px < 0 || py < 0 || px >= attch.width || py >= attch.height) {
    return false;
  }

  size_t pixel_index = (size_t)py * attch.width + (size_t)px;
  uint8_t* pixel_addr =
      attch.memory->data() + attch.offset +
      pixel_index * vg::GetPixelSize(attch.format, attch.component_type);
  SpinLock& lock = GetDepthLock(px, py);

  real old_depth = 0.0_r;
  bool depth_pass = true;
  uint8_t stencil = 0;
  bool stencil_pass = true;
  const int face_idx = is_front_face ? 0 : 1;

  if (attch.format == VG_DEPTH_STENCIL) {
    lock.Lock();
    vg::DecodeDepthStencil(&old_depth, &stencil, pixel_addr);
    lock.Unlock();

    // Stencil compare
    if (state_.stencil_test_enabled) {
      const uint8_t ref_m = (uint8_t)state_.stencil_ref[face_idx] &
                            state_.stencil_func_mask[face_idx];
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
          stencil_pass = true;
          break;
      }
    }
  } else {
    // Depth only
    lock.Lock();
    vg::CopyPixel((uint8_t*)(&old_depth), pixel_addr, attch.format,
                  attch.component_type, VG_DEPTH_COMPONENT, VG_FLOAT);
    lock.Unlock();
  }

  // Depth compare
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
    }
  }

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
      const uint8_t write_mask = (uint8_t)state_.stencil_write_mask[face_idx];
      if (write_mask != 0) {
        uint8_t result = stencil;
        const uint8_t ref = (uint8_t)state_.stencil_ref[face_idx];
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
            result = (uint8_t)~stencil;
            break;
          case VG_INCR_WRAP:
            result = (uint8_t)((stencil + 1) & 0xFF);
            break;
          case VG_DECR_WRAP:
            result = (uint8_t)((stencil - 1) & 0xFF);
            break;
        }
        stencil = (stencil & ~write_mask) | (result & write_mask);
      }
    }
  }
  // Depth update
  real write_depth = old_depth;
  if (state_.depth_test_enabled && depth_pass && state_.depth_write_enabled) {
    write_depth = depth;
  }

  if (attch.format == VG_DEPTH_STENCIL) {
    uint8_t bytes[4];
    vg::EncodeDepthStencil(bytes, write_depth, stencil);

    // Write
    lock.Lock();
    std::memcpy(pixel_addr, bytes, 4);
    lock.Unlock();
  } else {
    // Depth only
    lock.Lock();
    vg::CopyPixel(pixel_addr, (uint8_t*)(&write_depth), VG_DEPTH_COMPONENT,
                  VG_FLOAT, VG_DEPTH_COMPONENT, VG_FLOAT);
    lock.Unlock();
  }
  return (stencil_pass && depth_pass);
}

real VirtualGPU::GetBlendFactor(VGenum factor, const Color128& src,
                                const Color128& dst, int channel) const {
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

real VirtualGPU::ApplyBlendEquation(VGenum eq, real src_term,
                                    real dst_term) const {
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

void VirtualGPU::WriteColor(VGfloat x, VGfloat y, const Color128& color,
                            uint32_t slot) {
  FrameBuffer* fb = bound_draw_frame_buffer_;
  if (!fb) {
    return;
  }

  if (fb->draw_slot_to_color_attachment[slot] < 0 ||
      fb->draw_slot_to_color_attachment[slot] >= VG_DRAW_BUFFER_SLOT_COUNT) {
    return;
  }
  Attachment& attch =
      fb->color_attachments[fb->draw_slot_to_color_attachment[slot]];
  if (!attch.external_memory && !attch.memory) {
    return;
  }

  DrawBufferState& dbs = state_.draw_buffer_states[slot];

  uint32_t px = (uint32_t)math::Floor(x);
  uint32_t py = (uint32_t)math::Floor(y);

  size_t pixel_index = static_cast<size_t>(py) * attch.width + px;
  uint8_t* base = attch.external_memory != nullptr ? attch.external_memory
                                                   : attch.memory->data();
  uint8_t* pixel_addr =
      base + attch.offset +
      pixel_index * vg::GetPixelSize(attch.format, attch.component_type);
  SpinLock& lock =
      GetColorLock(fb->draw_slot_to_color_attachment[slot], px, py);

  Color128 dst_color;

  // read previous color
  lock.Lock();
  vg::DecodeColor(&dst_color, pixel_addr, attch.format, attch.component_type);
  lock.Unlock();

  Color128 final_color = color;

  // blending
  if (dbs.blend_enabled) {
    // RGB
    real r_src_factor =
        GetBlendFactor(state_.blend_src_rgb_factor, color, dst_color, 0);
    real r_dst_factor =
        GetBlendFactor(state_.blend_dst_rgb_factor, color, dst_color, 0);
    real src_term = color.r * r_src_factor;
    real dst_term = dst_color.r * r_dst_factor;
    final_color.r =
        ApplyBlendEquation(state_.blend_rgb_equation, src_term, dst_term);

    real g_src_factor =
        GetBlendFactor(state_.blend_src_rgb_factor, color, dst_color, 1);
    real g_dst_factor =
        GetBlendFactor(state_.blend_dst_rgb_factor, color, dst_color, 1);
    src_term = color.g * g_src_factor;
    dst_term = dst_color.g * g_dst_factor;
    final_color.g =
        ApplyBlendEquation(state_.blend_rgb_equation, src_term, dst_term);

    real b_src_factor =
        GetBlendFactor(state_.blend_src_rgb_factor, color, dst_color, 2);
    real b_dst_factor =
        GetBlendFactor(state_.blend_dst_rgb_factor, color, dst_color, 2);
    src_term = color.b * b_src_factor;
    dst_term = dst_color.b * b_dst_factor;
    final_color.b =
        ApplyBlendEquation(state_.blend_rgb_equation, src_term, dst_term);

    // Alpha
    real a_src_factor =
        GetBlendFactor(state_.blend_src_alpha_factor, color, dst_color, 3);
    real a_dst_factor =
        GetBlendFactor(state_.blend_dst_alpha_factor, color, dst_color, 3);
    src_term = color.a * a_src_factor;
    dst_term = dst_color.a * a_dst_factor;
    final_color.a =
        ApplyBlendEquation(state_.blend_alpha_equation, src_term, dst_term);
  }

  // apply color mask
  Color128 write_color = dst_color;
  if (dbs.color_mask[0]) write_color.r = final_color.r;
  if (dbs.color_mask[1]) write_color.g = final_color.g;
  if (dbs.color_mask[2]) write_color.b = final_color.b;
  if (dbs.color_mask[3]) write_color.a = final_color.a;

  lock.Lock();
  vg::EncodeColor(pixel_addr, write_color, attch.format, attch.component_type);
  lock.Unlock();
}

void VirtualGPU::AfterVSJobEntry(void* input, uint32_t size) {
  assert(size == sizeof(AfterVSJobInput));
  VirtualGPU& vg = VirtualGPU::GetInstance();
  AfterVSJobInput* in = static_cast<AfterVSJobInput*>(input);

  std::vector<Varying> poly;

  for (Varying* v : in->poly) {
    poly.push_back(*v);
  }

  // Clipping
  poly = vg.Clip(poly);

  if (poly.empty()) {
    delete in;
    return;
  }

  // Perspective devide -> Viewport transform
  for (Varying& v : poly) {
    vg.PerspectiveDivide(v);
    vg.ViewportTransform(v);
  }

  std::vector<Fragment> frags;
  std::vector<Fragment> temp_frags;
  const auto pmode = vg.state_.polygon_mode;

  if (poly.size() == 1) {
    frags = vg.Rasterize(poly[0]);
  } else if (poly.size() == 2) {
    frags = vg.Rasterize(poly[0], poly[1]);
  } else {
    switch (pmode) {
      case VG_POINT:
        for (const Varying& v : poly) {
          temp_frags = vg.Rasterize(v);
          frags.insert(frags.end(), std::make_move_iterator(temp_frags.begin()),
                       std::make_move_iterator(temp_frags.end()));
        }
        break;

      case VG_LINE:
        for (size_t i = 0; i < poly.size(); ++i) {
          const Varying& a = poly[i];
          const Varying& b = poly[(i + 1) % poly.size()];
          temp_frags = vg.Rasterize(a, b);
          frags.insert(frags.end(), std::make_move_iterator(temp_frags.begin()),
                       std::make_move_iterator(temp_frags.end()));
        }
        break;
      case VG_FILL:
        for (size_t i = 1; i + 1 < poly.size(); ++i) {
          temp_frags = vg.Rasterize(poly[0], poly[i], poly[i + 1]);
          frags.insert(frags.end(), std::make_move_iterator(temp_frags.begin()),
                       std::make_move_iterator(temp_frags.end()));
        }
        break;
    }
  }

  if (frags.empty()) {
    delete in;
    return;
  }

  FrameBuffer* fb = vg.bound_draw_frame_buffer_;
  FSOutputs outputs;
  for (const Fragment& frag : frags) {
    outputs.written.reset();
    in->fs(frag, outputs);
    for (uint32_t slot = 0; slot < VG_DRAW_BUFFER_SLOT_COUNT; ++slot) {
      if (!outputs.written.test(slot)) {
        continue;
      }

      vg.WriteColor(frag.screen_coord.x, frag.screen_coord.y,
                    std::get<Color128>(outputs.values[slot]), slot);
    }
  }
  delete in;
}
}  // namespace ho