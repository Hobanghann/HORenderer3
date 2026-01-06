#pragma once

#include <array>
#include <bitset>
#include <half.hpp>
#include <list>
#include <optional>
#include <unordered_map>
#include <utility>
#include <variant>

#include "core/math/color128.h"
#include "core/math/color32.h"
#include "core/math/matrix2x2.h"
#include "core/math/matrix3x3.h"
#include "core/math/matrix4x4.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/vector4.h"
#include "core/templates/atomic_numeric.h"
#include "core/thread/job_system.h"
#include "core/thread/spin_lock.h"
#include "vg.h"

namespace ho {

constexpr VGint WORKER_COUNT = 8;
constexpr VGint TILE_WIDTH = 16;
constexpr VGint TILE_HEIGHT = 16;
constexpr VGint VG_MAX_ATTACHMENT_WIDTH = 4096;
constexpr VGint VG_MAX_ATTACHMENT_HEIGHT = 4096;
constexpr VGint MAX_LOCK_TABLE_WIDTH = VG_MAX_ATTACHMENT_WIDTH / TILE_WIDTH;
constexpr VGint MAX_LOCK_TABLE_HEIGHT = VG_MAX_ATTACHMENT_HEIGHT / TILE_HEIGHT;
constexpr VGint VG_COLOR_ATTACHMENT_COUNT =
    VG_COLOR_ATTACHMENT31 - VG_COLOR_ATTACHMENT0 + 1;
constexpr VGint VG_TEXTURE_UNIT_COUNT = VG_TEXTURE31 - VG_TEXTURE0 + 1;
constexpr VGint VG_DRAW_BUFFER_SLOT_COUNT =
    VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0 + 1;
constexpr VGint VG_BUFFER_OBJECT_SLOT_COUNT = 9;

class VirtualGPU {
 public:
  static VirtualGPU& GetInstance() {
    static VirtualGPU instance;
    return instance;
  }

  bool Initialize(uint8_t* color_buffer, VGsizei width, VGsizei height,
                  VGenum color_format, VGsizei component_type);

 private:
  VirtualGPU();

  // ======================================================
  //  VirtualGPU Core Object and State Definitions
  // ======================================================

  struct Varying {
    Vector4 clip_coord;
    Vector3 ndc;
    Vector3 viewport_coord;
    Vector3 world_pos;
    Vector3 view_pos;
    Vector3 normal;
    Vector4 tangent;
    Vector2 uv0;
    Vector2 uv1;
    Color128 color0;
    Color128 color1;
  };

  struct Fragment {
    Vector3 world_pos;
    Vector3 view_pos;
    Vector2 screen_coord;
    real depth;
    real depth_slope;  // point: 0, line: 0, triangle: max(z/x, z/y) of triangle
                       // normal
    Vector3 normal;
    Vector4 tangent;
    Vector2 uv0;
    Vector2 uv1;
    Color128 color0;
    Color128 color1;
    bool is_front;
  };

  using FSOutput = std::variant<Vector4, Color128>;

  struct FSOutputs {
    struct SlotProxy {
      FSOutputs& parent;
      uint32_t slot;

      SlotProxy& operator=(const FSOutput& v) {
        parent.values[slot] = v;
        parent.written.set(slot);
        return *this;
      }

      template <typename T>
      SlotProxy& operator=(T&& v) {
        parent.values[slot] = FSOutput(std::forward<T>(v));
        parent.written.set(slot);
        return *this;
      }
    };

    SlotProxy operator[](uint32_t slot) { return SlotProxy{*this, slot}; }

    std::array<FSOutput, VG_DRAW_BUFFER_SLOT_COUNT> values;
    std::bitset<VG_DRAW_BUFFER_SLOT_COUNT> written;
  };

  using VertexShader = void (*)(uint32_t vertex_index, Varying& out);
  using FragmentShader = void (*)(const Fragment& fragment, FSOutputs& out);

  struct BufferObject {
    VGuint id;
    std::vector<uint8_t>* memory = nullptr;
    VGenum usage = VG_STATIC_DRAW;
    bool mapped = false;
    VGenum map_access = 0;
    bool immutable = false;
    VGbitfield storage_flags = 0;
    int refcount = 0;
    bool is_deleted = false;
  };

  struct ConstantAttribute {
    VGenum type = VG_FLOAT;
    bool normalized = false;
    bool is_pure_integer = false;

    union {
      VGdouble d[4];
      VGfloat f[4];
      VGshort s[4];
      VGushort us[4];
      VGint i[4];
      VGuint u[4];
      VGbyte b[4];
      VGubyte ub[4];
    };

    ConstantAttribute() {
      f[0] = 0.f;
      f[1] = 0.f;
      f[2] = 0.f;
      f[3] = 1.f;
    }
  };

  struct Attribute {
    BufferObject* buffer = nullptr;
    bool enabled = false;
    VGint size = 4;
    VGenum type = VG_FLOAT;
    bool is_pure_integer = false;
    bool normalized = false;
    VGsizei stride = 0;
    VGint offset = 0;
  };

  struct VertexArray {
    VGuint id;
    VGuint vertex_count = 0;
    std::unordered_map<VGuint, Attribute> index_to_attrib;
    BufferObject* element_buffer;
    bool is_bound_once = false;
    int refcount = 0;
    bool is_deleted = false;
  };

  struct BufferBinding {
    BufferObject* buffer = nullptr;
    VGintptr offset = 0;
    VGsizeiptr size = 0;
  };

  struct Sampler {
    VGuint id;

    // Wrap
    VGint wrap_s = VG_REPEAT;
    VGint wrap_t = VG_REPEAT;
    VGint wrap_r = VG_REPEAT;

    // Filter
    VGint min_filter = VG_NEAREST_MIPMAP_LINEAR;
    VGint mag_filter = VG_LINEAR;

    // LOD
    VGfloat min_lod = -1000.f;
    VGfloat max_lod = 1000.f;
    VGfloat lod_bias = 0.f;

    VGint base_level = 0;
    VGint max_level = 1000;

    // Compare
    VGint compare_mode = VG_NONE;
    VGint compare_func = VG_LEQUAL;

    // Swizzle
    VGint swizzle_r = VG_RED;
    VGint swizzle_g = VG_GREEN;
    VGint swizzle_b = VG_BLUE;
    VGint swizzle_a = VG_ALPHA;

    // Depth/stencil
    VGint depth_stencil_mode = VG_DEPTH;

    // Border
    Color128 border_color = Color128(0.f, 0.f, 0.f, 0.f);

    int refcount = 0;
    bool is_deleted = false;
  };

  struct TextureLevel {
    std::vector<uint8_t>* memory = nullptr;
    VGint mipmap_level = 0;
    VGsizei width = 0;
    VGsizei height = 0;
    VGsizei depth = 0;
  };

  struct TextureObject {
    VGuint id;
    std::array<TextureLevel, 1> mipmap;
    VGenum texture_type = VG_NONE;
    VGenum component_type = VG_NONE;
    VGenum internal_format = VG_RGBA;
    Sampler default_sampler;
    int refcount = 0;
    bool is_deleted = false;
  };

  struct TextureUnit {
    std::array<TextureObject*, 9> bound_texture_targets = {nullptr};
    Sampler* bound_sampler = nullptr;
  };

  struct ProxyTextureState {
    VGenum format = VG_NONE;
    VGsizei width = 0;
    VGsizei height = 0;
    VGsizei depth = 0;
  };

  struct RenderBuffer {
    VGuint id;
    std::vector<uint8_t>* memory = nullptr;
    VGenum component_type = VG_NONE;
    VGenum format = VG_RGBA;
    VGsizei width = 0;
    VGsizei height = 0;
    int refcount = 0;
    bool is_deleted = false;
  };

  struct Attachment {
    VGuint ref_id;  // referenced texture or render buffer id
    uint8_t* external_memory = nullptr;
    std::vector<uint8_t>* memory = nullptr;
    VGenum component_type = VG_NONE;
    VGenum format = VG_NONE;
    VGsizei width = 0;
    VGsizei height = 0;
    VGsizei offset = 0;
  };

  struct FrameBuffer {
    VGuint id;  // id is 0 on default freame buffer
    std::array<Attachment, VG_COLOR_ATTACHMENT31 - VG_COLOR_ATTACHMENT0 + 1>
        color_attachments;
    Attachment depth_stencil_attachment;  // default frame buffer has depth
                                          // stencil attachment

    // DRAW_BUFFERi -> color attachment index
    // value == -1 means NONE
    std::array<int, VG_DRAW_BUFFER15 - VG_DRAW_BUFFER0 + 1>
        draw_slot_to_color_attachment = {-1};

    // value == -1 means NONE
    int read_slot_to_color_attachment = -1;
    bool is_bound_once = false;
    int refcount = 0;
    bool is_deleted = false;
  };

  struct DrawBufferState {
    // blend states
    bool blend_enabled = false;
    // color mask
    bool color_mask[4] = {true, true, true, true};
  };

  struct Uniform {
    VGenum type;
    VGint size;
    VGsizei count;
    std::vector<uint8_t> data;
  };

  struct Shader {
    VGuint id;
    VGenum type = VG_NONE;
    void* source = nullptr;
    int refcount = 0;
    bool is_deleted = false;
  };

  struct Program {
    VGuint id;
    Shader* vertex_shader = nullptr;
    Shader* fragment_shader = nullptr;
    std::vector<Varying> varying_buffer;
    VGenum link_status = VG_FALSE;
    std::vector<Uniform> uniforms;
    std::unordered_map<VGint, uint32_t> uniform_name_hash_to_location;
    std::unordered_map<VGint, uint32_t> fragout_name_hash_to_draw_buffer_slot;
    int refcount = 0;
    bool is_deleted = false;
  };

  struct Rect {
    VGint x = 0;
    VGint y = 0;
    VGsizei width = 0;
    VGsizei height = 0;
  };

  struct State {
    std::array<
        std::array<SpinLock, MAX_LOCK_TABLE_WIDTH * MAX_LOCK_TABLE_HEIGHT>,
        VG_COLOR_ATTACHMENT_COUNT>
        color_lock_tables;

    std::array<SpinLock, MAX_LOCK_TABLE_WIDTH * MAX_LOCK_TABLE_HEIGHT>
        depth_lock_table;

    Color128 clear_color = Color128(0.f, 0.f, 0.f, 0.f);

    Rect viewport = {0, 0, 0, 0};

    bool scissor_test_enabled = false;
    Rect scissor = {0, 0, 0, 0};

    bool depth_test_enabled = false;
    bool depth_write_enabled = true;
    VGfloat clear_depth = 1.f;
    VGfloat min_depth = 0.f;
    VGfloat max_depth = 1.f;
    VGfloat depth_factor = 0.f;
    VGfloat depth_unit = 0.f;
    VGenum depth_func = VG_LESS;

    bool stencil_test_enabled = false;
    VGint clear_stencil = 0;
    // 0: FRONT, 1: BACK
    VGenum stencil_func[2] = {VG_ALWAYS, VG_ALWAYS};
    VGint stencil_ref[2] = {0, 0};
    VGuint stencil_func_mask[2] = {0xFFFFFFFF, 0xFFFFFFFF};
    VGuint stencil_write_mask[2] = {0xFFFFFFFF, 0xFFFFFFFF};
    VGenum stencil_sfail_op[2] = {VG_KEEP, VG_KEEP};
    VGenum stencil_dpfail_op[2] = {VG_KEEP, VG_KEEP};
    VGenum stencil_dppass_op[2] = {VG_KEEP, VG_KEEP};

    VGenum blend_src_rgb_factor = VG_ONE;
    VGenum blend_dst_rgb_factor = VG_ZERO;
    VGenum blend_rgb_equation = VG_FUNC_ADD;
    VGenum blend_src_alpha_factor = VG_ONE;
    VGenum blend_dst_alpha_factor = VG_ZERO;
    VGenum blend_alpha_equation = VG_FUNC_ADD;
    Color128 blend_constant = Color128(0.f, 0.f, 0.f, 0.f);

    std::array<DrawBufferState, VG_DRAW_BUFFER_SLOT_COUNT> draw_buffer_states;

    bool cull_enabled = false;
    VGenum cull_face = VG_BACK;
    VGenum front_face = VG_CCW;

    bool point_offset_enabled = false;
    bool line_offset_enabled = false;
    bool polygon_offset_enabled = false;

    VGenum polygon_mode = VG_FILL;

    VGfloat line_width = 1.f;

    VGenum error_state = VG_NO_ERROR;
  };

  // ======================================================
  // Members
  // ======================================================

  std::list<std::vector<uint8_t>> vram_;

  VGuint base_handle_ = 0;
  std::unordered_map<VGuint, VertexArray> vertex_array_pool_;
  std::unordered_map<VGuint, BufferObject> buffer_pool_;
  std::unordered_map<VGuint, TextureObject> texture_pool_;
  std::unordered_map<VGuint, Sampler> sampler_pool_;
  std::unordered_map<VGuint, FrameBuffer> frame_buffer_pool_;
  std::unordered_map<VGuint, RenderBuffer> render_buffer_pool_;
  std::unordered_map<VGuint, Shader> shader_pool_;
  std::unordered_map<VGuint, Program> program_pool_;

  VertexArray* bound_vertex_array_ = nullptr;
  Program* using_program_ = nullptr;
  VGuint active_texture_unit_ = 0;
  std::array<BufferObject*, VG_BUFFER_OBJECT_SLOT_COUNT> bound_buffer_targets_;
  FrameBuffer* bound_draw_frame_buffer_;
  FrameBuffer* bound_read_frame_buffer_;
  RenderBuffer* bound_render_buffer_ = nullptr;
  std::array<TextureUnit, VG_TEXTURE_UNIT_COUNT> texture_units_;
  std::array<ProxyTextureState, 9> proxy_texture_states_;
  std::unordered_map<VGuint, BufferBinding> uniform_buffer_bindings_;
  std::unordered_map<VGuint, BufferBinding> transform_feedback_buffer_bindings_;
  std::unordered_map<VGuint, ConstantAttribute> constant_attributes_;

  State state_;

  JobSystem job_system_;

  // ======================================================
  // Rendering Pipeline API
  // ======================================================

  ALWAYS_INLINE SpinLock& GetColorLock(VGuint attachment_index, VGint x,
                                       VGint y) {
    assert(x >= 0 && x < VG_MAX_ATTACHMENT_WIDTH);
    assert(y >= 0 && y < VG_MAX_ATTACHMENT_HEIGHT);

    VGint lock_x = x / TILE_WIDTH;
    VGint lock_y = y / TILE_HEIGHT;

    VGint lock_index = lock_y * MAX_LOCK_TABLE_WIDTH + lock_x;
    return state_.color_lock_tables[attachment_index][lock_index];
  }

  ALWAYS_INLINE SpinLock& GetDepthLock(VGint x, VGint y) {
    assert(x >= 0 && x < VG_MAX_ATTACHMENT_WIDTH);
    assert(y >= 0 && y < VG_MAX_ATTACHMENT_HEIGHT);

    VGint lock_x = x / TILE_WIDTH;
    VGint lock_y = y / TILE_HEIGHT;

    VGint lock_index = lock_y * MAX_LOCK_TABLE_WIDTH + lock_x;
    return state_.depth_lock_table[lock_index];
  }

  void ClearColorAttachment(uint32_t slot, const Color128& clear_color);
  void ClearDepthAttachment(float clear_depth);
  void ClearDepthStencilAttachment(bool is_depth_cleared,
                                   bool is_stencil_cleared, float clear_depth,
                                   uint8_t clear_stencil);

  // Vertex Processing
  struct VSJobInput {
    VertexShader vs;
    uint32_t base_index;
    uint32_t first_index;
    uint32_t last_index;
  };

  static void VSJobEntry(void* input, uint32_t size);

  // Rasterization, true: passed, false: not passed
  enum PlanePos {
    VG_PLANE_POS_LEFT = 0,
    VG_PLANE_POS_RIGHT,
    VG_PLANE_POS_BOTTOM,
    VG_PLANE_POS_TOP,
    VG_PLANE_POS_NEAR,
    VG_PLANE_POS_FAR,
    VG_PLANE_POS_PROJECTION,
    VG_PLANE_POS_LAST
  };

  std::vector<Varying> Clip(const std::vector<Varying>& polygon) const;
  std::vector<Varying> ClipAgainstPlane(const std::vector<Varying>& polygon,
                                        PlanePos plane_pos) const;
  real EvalFrustumPlane(const Vector4& clip_coord, PlanePos plane_pos) const;
  bool IsInside(const Vector4& clip_coord, PlanePos plane_pos) const;
  Vector2 GetClipBarycentric(const Vector4& clip_coord1,
                             const Vector4& clip_coord2,
                             PlanePos plane_pos) const;
  Varying LerpVarying(const Varying& v1, const Varying& v2,
                      const Vector2& barycentric) const;

  void PerspectiveDivide(Varying& v) const;
  void ViewportTransform(Varying& v) const;

  std::vector<Fragment> Rasterize(const Varying& v);
  std::vector<Fragment> Rasterize(const Varying& v1, const Varying& v2);

  struct BoundingBox2D {
    BoundingBox2D(const Vector2& v1, const Vector2& v2, const Vector2& v3)
        : min(math::Min(v1.x, v2.x, v3.x), math::Min(v1.y, v2.y, v3.y)),
          max(math::Max(v1.x, v2.x, v3.x), math::Max(v1.y, v2.y, v3.y)) {}
    Vector2 min;
    Vector2 max;
  };

  struct EdgeFunction {
    EdgeFunction(const Vector2& start, const Vector2& end,
                 const Vector2& initial)
        : dx(end.y - start.y),
          dy(start.x - end.x),
          initial_value(dx * initial.x + dy * initial.y +
                        (end.x * start.y - start.x * end.y)) {}
    real dx;
    real dy;
    real initial_value;
  };

  std::vector<Fragment> Rasterize(const Varying& v1, const Varying& v2,
                                  const Varying& v3);

  // Output Merging
  bool ScissorTest(real x, real y) const;

  real ComputeDepthSlope(const Vector3& v0, const Vector3& v1,
                         const Vector3& v2) const;
  VGfloat ApplyDepthOffset(VGfloat depth, VGfloat depth_slope,
                           uint32_t depth_bits, VGenum primitive_mode) const;

  // true: passed, false: not passed
  bool TestDepthStencil(VGfloat x, VGfloat y, VGfloat depth,
                        bool is_front_face);

  real GetBlendFactor(VGenum factor, const Color128& src, const Color128& dst,
                      int channel) const;

  real ApplyBlendEquation(VGenum eq, real src_term, real dst_term) const;

  void WriteColor(VGfloat x, VGfloat y, const Color128& color, uint32_t slot);

  struct AfterVSJobInput {
    std::vector<Varying*> poly;
    FragmentShader fs;
  };

  static void AfterVSJobEntry(void* input, uint32_t size);

  // ======================================================
  // Friend decl
  // ======================================================

  template <typename T>
  friend T FetchAttribute(VGuint location, VGuint index);
  template <typename T>
  friend T FetchUniform(VGint name_hash, VGuint index);
  template <typename T>
  friend T FetchUniformBlock(VGuint binding, VGuint offset);

  friend VGfloat ApplyWrap(VGint wrap_mode, VGfloat coord);
  friend Color128 ApplyFilter(VGint filter,
                              const VirtualGPU::TextureObject& tex,
                              uint32_t level, VGfloat u);
  friend Color128 ApplyFilter(VGint filter,
                              const VirtualGPU::TextureObject& tex,
                              uint32_t level, VGfloat u, VGfloat v);
  friend Color128 ApplyFilter(VGint filter,
                              const VirtualGPU::TextureObject& tex,
                              uint32_t level, VGfloat u, VGfloat v, VGfloat w);
  friend Color128 SampleTexture1D(VGuint unit_slot, VGfloat u);
  friend Color128 SampleTexture2D(VGuint unit_slot, VGfloat u, VGfloat v);
  friend Color128 SampleTexture3D(VGuint unit_slot, VGfloat u, VGfloat v,
                                  VGfloat w);

  friend void FreeVram(std::vector<uint8_t>* mem);
  friend void ReleaseAttachment(VGuint refid);

  friend void ReleaseVertexArray(VGuint vertex_array);
  friend void ReleaseBufferObject(VGuint buffer_object);
  friend void ReleaseTextureObject(VGuint texture);
  friend void ReleaseSampler(VGuint sampler);
  friend void ReleaseFrameBuffer(VGuint frame_buffer);
  friend void ReleaseRenderBuffer(VGuint render_buffer);
  friend void ReleaseShader(VGuint shader);
  friend void ReleaseProgram(VGuint program);

  friend void DestroyVertexArray(VGuint vertex_array);
  friend void DestroyBufferObject(VGuint buffer_object);
  friend void DestroyTextureObject(VGuint texture);
  friend void DestroySampler(VGuint sampler);
  friend void DestroyFrameBuffer(VGuint frame_buffer);
  friend void DestroyRenderBuffer(VGuint render_buffer);
  friend void DestroyShader(VGuint shader);
  friend void DestroyProgram(VGuint program);

  friend void vgCullFace(VGenum mode);
  friend void vgFrontFace(VGenum mode);
  friend void vgHint(VGenum target, VGenum mode);
  friend void vgLineWidth(VGfloat width);
  friend void vgPointSize(VGfloat size);
  friend void vgPolygonMode(VGenum face, VGenum mode);
  friend void vgScissor(VGint x, VGint y, VGsizei width, VGsizei height);
  friend void vgTexParameterf(VGenum target, VGenum pname, VGfloat param);
  friend void vgTexParameterfv(VGenum target, VGenum pname,
                               const VGfloat* params);
  friend void vgTexParameteri(VGenum target, VGenum pname, VGint param);
  friend void vgTexParameteriv(VGenum target, VGenum pname,
                               const VGint* params);
  friend void vgTexImage1D(VGenum target, VGint level, VGint internalformat,
                           VGsizei width, VGint border, VGenum format,
                           VGenum type, const void* pixels);
  friend void vgTexImage2D(VGenum target, VGint level, VGint internalformat,
                           VGsizei width, VGsizei height, VGint border,
                           VGenum format, VGenum type, const void* pixels);
  friend void vgDrawBuffer(VGenum buf);
  friend void vgClear(VGbitfield mask);
  friend void vgClearColor(VGfloat red, VGfloat green, VGfloat blue,
                           VGfloat alpha);
  friend void vgClearStencil(VGint s);
  friend void vgClearDepth(VGdouble depth);
  friend void vgStencilMask(VGuint mask);
  friend void vgColorMask(VGboolean red, VGboolean green, VGboolean blue,
                          VGboolean alpha);
  friend void vgDepthMask(VGboolean flag);
  friend void vgDisable(VGenum cap);
  friend void vgEnable(VGenum cap);
  friend void vgFinish(void);
  friend void vgFlush(void);
  friend void vgBlendFunc(VGenum sfactor, VGenum dfactor);
  friend void vgLogicOp(VGenum opcode);
  friend void vgStencilFunc(VGenum func, VGint ref, VGuint mask);
  friend void vgStencilOp(VGenum fail, VGenum zfail, VGenum zpass);
  friend void vgDepthFunc(VGenum func);
  friend void vgPixelStoref(VGenum pname, VGfloat param);
  friend void vgPixelStorei(VGenum pname, VGint param);
  friend void vgReadBuffer(VGenum src);
  friend void vgReadPixels(VGint x, VGint y, VGsizei width, VGsizei height,
                           VGenum format, VGenum type, void* pixels);
  friend void vgGetBooleanv(VGenum pname, VGboolean* data);
  friend void vgGetDoublev(VGenum pname, VGdouble* data);
  friend VGenum vgGetError(void);
  friend void vgGetFloatv(VGenum pname, VGfloat* data);
  friend void vgGetIntegerv(VGenum pname, VGint* data);
  friend const VGubyte* vgGetString(VGenum name);
  friend void vgGetTexImage(VGenum target, VGint level, VGenum format,
                            VGenum type, void* pixels);
  friend void vgGetTexParameterfv(VGenum target, VGenum pname, VGfloat* params);
  friend void vgGetTexParameteriv(VGenum target, VGenum pname, VGint* params);
  friend void vgGetTexLevelParameterfv(VGenum target, VGint level, VGenum pname,
                                       VGfloat* params);
  friend void vgGetTexLevelParameteriv(VGenum target, VGint level, VGenum pname,
                                       VGint* params);
  friend VGboolean vgIsEnabled(VGenum cap);
  friend void vgDepthRange(VGdouble n, VGdouble f);
  friend void vgViewport(VGint x, VGint y, VGsizei width, VGsizei height);

  friend void vgDrawArrays(VGenum mode, VGint first, VGsizei count);
  friend void vgDrawElements(VGenum mode, VGsizei count, VGenum type,
                             const void* indices);
  friend void vgGetPointerv(VGenum pname, void** params);
  friend void vgPolygonOffset(VGfloat factor, VGfloat units);
  friend void vgTexSubImage1D(VGenum target, VGint level, VGint xoffset,
                              VGsizei width, VGenum format, VGenum type,
                              const void* pixels);
  friend void vgTexSubImage2D(VGenum target, VGint level, VGint xoffset,
                              VGint yoffset, VGsizei width, VGsizei height,
                              VGenum format, VGenum type, const void* pixels);
  friend void vgBindTexture(VGenum target, VGuint texture);
  friend void vgDeleteTextures(VGsizei n, const VGuint* textures);
  friend void vgGenTextures(VGsizei n, VGuint* textures);
  friend VGboolean vgIsTexture(VGuint texture);

  friend void vgDrawRangeElements(VGenum mode, VGuint start, VGuint end,
                                  VGsizei count, VGenum type,
                                  const void* indices);
  friend void vgTexImage3D(VGenum target, VGint level, VGint internalformat,
                           VGsizei width, VGsizei height, VGsizei depth,
                           VGint border, VGenum format, VGenum type,
                           const void* pixels);
  friend void vgTexSubImage3D(VGenum target, VGint level, VGint xoffset,
                              VGint yoffset, VGint zoffset, VGsizei width,
                              VGsizei height, VGsizei depth, VGenum format,
                              VGenum type, const void* pixels);
  friend void vgCopyTexSubImage3D(VGenum target, VGint level, VGint xoffset,
                                  VGint yoffset, VGint zoffset, VGint x,
                                  VGint y, VGsizei width, VGsizei height);

  friend void vgActiveTexture(VGenum texture);
  friend void vgSampleCoverage(VGfloat value, VGboolean invert);
  friend void vgCompressedTexImage3D(VGenum target, VGint level,
                                     VGenum internalformat, VGsizei width,
                                     VGsizei height, VGsizei depth,
                                     VGint border, VGsizei imageSize,
                                     const void* data);
  friend void vgCompressedTexImage2D(VGenum target, VGint level,
                                     VGenum internalformat, VGsizei width,
                                     VGsizei height, VGint border,
                                     VGsizei imageSize, const void* data);
  friend void vgCompressedTexImage1D(VGenum target, VGint level,
                                     VGenum internalformat, VGsizei width,
                                     VGint border, VGsizei imageSize,
                                     const void* data);
  friend void vgCompressedTexSubImage3D(VGenum target, VGint level,
                                        VGint xoffset, VGint yoffset,
                                        VGint zoffset, VGsizei width,
                                        VGsizei height, VGsizei depth,
                                        VGenum format, VGsizei imageSize,
                                        const void* data);
  friend void vgCompressedTexSubImage2D(VGenum target, VGint level,
                                        VGint xoffset, VGint yoffset,
                                        VGsizei width, VGsizei height,
                                        VGenum format, VGsizei imageSize,
                                        const void* data);
  friend void vgCompressedTexSubImage1D(VGenum target, VGint level,
                                        VGint xoffset, VGsizei width,
                                        VGenum format, VGsizei imageSize,
                                        const void* data);
  friend void vgGetCompressedTexImage(VGenum target, VGint level, void* img);

  friend void vgBlendFuncSeparate(VGenum sfactorRGB, VGenum dfactorRGB,
                                  VGenum sfactorAlpha, VGenum dfactorAlpha);
  friend void vgMultiDrawArrays(VGenum mode, const VGint* first,
                                const VGsizei* count, VGsizei drawcount);
  friend void vgMultiDrawElements(VGenum mode, const VGsizei* count,
                                  VGenum type, const void* const* indices,
                                  VGsizei drawcount);
  friend void vgPointParameterf(VGenum pname, VGfloat param);
  friend void vgPointParameterfv(VGenum pname, const VGfloat* params);
  friend void vgPointParameteri(VGenum pname, VGint param);
  friend void vgPointParameteriv(VGenum pname, const VGint* params);
  friend void vgBlendColor(VGfloat red, VGfloat green, VGfloat blue,
                           VGfloat alpha);
  friend void vgBlendEquation(VGenum mode);

  friend void vgBindBuffer(VGenum target, VGuint buffer);
  friend void vgDeleteBuffers(VGsizei n, const VGuint* buffers);
  friend void vgGenBuffers(VGsizei n, VGuint* buffers);
  friend VGboolean vgIsBuffer(VGuint buffer);
  friend void vgBufferData(VGenum target, VGsizeiptr size, const void* data,
                           VGenum usage);
  friend void vgBufferSubData(VGenum target, VGintptr offset, VGsizeiptr size,
                              const void* data);
  friend void vgGetBufferSubData(VGenum target, VGintptr offset,
                                 VGsizeiptr size, void* data);
  friend void* vgMapBuffer(VGenum target, VGenum access);
  friend VGboolean vgUnmapBuffer(VGenum target);
  friend void vgGetBufferParameteriv(VGenum target, VGenum pname,
                                     VGint* params);
  friend void vgGetBufferPointerv(VGenum target, VGenum pname, void** params);

  friend void vgBlendEquationSeparate(VGenum modeRGB, VGenum modeAlpha);
  friend void vgDrawBuffers(VGsizei n, const VGenum* bufs);
  friend void vgStencilOpSeparate(VGenum face, VGenum sfail, VGenum dpfail,
                                  VGenum dppass);
  friend void vgStencilFuncSeparate(VGenum face, VGenum func, VGint ref,
                                    VGuint mask);
  friend void vgStencilMaskSeparate(VGenum face, VGuint mask);
  friend void vgAttachShader(VGuint program, VGuint shader);
  friend void vgBindAttribLocation(VGuint program, VGuint index,
                                   const VGchar* name);
  friend void vgCompileShader(VGuint shader);
  friend VGuint vgCreateProgram(void);
  friend VGuint vgCreateShader(VGenum type);
  friend void vgDeleteProgram(VGuint program);
  friend void vgDeleteShader(VGuint shader);
  friend void vgDetachShader(VGuint program, VGuint shader);
  friend void vgDisableVertexAttribArray(VGuint index);
  friend void vgEnableVertexAttribArray(VGuint index);
  friend void vgGetActiveAttrib(VGuint program, VGuint index, VGsizei bufSize,
                                VGsizei* length, VGint* size, VGenum* type,
                                VGchar* name);
  friend void vgGetActiveUniform(VGuint program, VGuint index, VGsizei bufSize,
                                 VGsizei* length, VGint* size, VGenum* type,
                                 VGchar* name);
  friend void vgGetAttachedShaders(VGuint program, VGsizei maxCount,
                                   VGsizei* count, VGuint* shaders);
  friend VGint vgGetAttribLocation(VGuint program, VGint name_hash);
  friend VGint vgGetAttribLocation(VGuint program, const VGchar* name);
  friend void vgGetProgramiv(VGuint program, VGenum pname, VGint* params);
  friend void vgGetProgramInfoLog(VGuint program, VGsizei bufSize,
                                  VGsizei* length, VGchar* infoLog);
  friend void vgGetShaderiv(VGuint shader, VGenum pname, VGint* params);
  friend void vgGetShaderInfoLog(VGuint shader, VGsizei bufSize,
                                 VGsizei* length, VGchar* infoLog);
  friend void vgGetShaderSource(VGuint shader, VGsizei bufSize, VGsizei* length,
                                VGchar* source);
  friend VGint vgGetUniformLocation(VGuint program, VGint name_hash);
  friend VGint vgGetUniformLocation(VGuint program, const VGchar* name);
  friend void vgGetUniformfv(VGuint program, VGint location, VGfloat* params);
  friend void vgGetUniformiv(VGuint program, VGint location, VGint* params);
  friend void vgGetVertexAttribdv(VGuint index, VGenum pname, VGdouble* params);
  friend void vgGetVertexAttribfv(VGuint index, VGenum pname, VGfloat* params);
  friend void vgGetVertexAttribiv(VGuint index, VGenum pname, VGint* params);
  friend void vgGetVertexAttribPointerv(VGuint index, VGenum pname,
                                        void** pointer);
  friend VGboolean vgIsProgram(VGuint program);
  friend VGboolean vgIsShader(VGuint shader);
  friend void vgLinkProgram(VGuint program);
  friend void vgShaderSource(VGuint shader, void* source);
  friend void vgUseProgram(VGuint program);
  template <typename T>
  friend void vgUniform1t(VGint location, T v0);
  template <typename T>
  friend void vgUniform2t(VGint location, T v0, T v1);
  template <typename T>
  friend void vgUniform3t(VGint location, T v0, T v1, T v2);
  template <typename T>
  friend void vgUniform4t(VGint location, T v0, T v1, T v2, T v3);
  template <typename T>
  friend void vgUniform1tv(VGint location, VGsizei count, const T* value);
  template <typename T, int N>
  friend void vgUniformNtv(VGint location, VGsizei count, const T* value);
  friend void vgUniform1f(VGint location, VGfloat v0);
  friend void vgUniform2f(VGint location, VGfloat v0, VGfloat v1);
  friend void vgUniform3f(VGint location, VGfloat v0, VGfloat v1, VGfloat v2);
  friend void vgUniform4f(VGint location, VGfloat v0, VGfloat v1, VGfloat v2,
                          VGfloat v3);
  friend void vgUniform1i(VGint location, VGint v0);
  friend void vgUniform2i(VGint location, VGint v0, VGint v1);
  friend void vgUniform3i(VGint location, VGint v0, VGint v1, VGint v2);
  friend void vgUniform4i(VGint location, VGint v0, VGint v1, VGint v2,
                          VGint v3);
  friend void vgUniform1fv(VGint location, VGsizei count, const VGfloat* value);
  friend void vgUniform2fv(VGint location, VGsizei count, const VGfloat* value);
  friend void vgUniform3fv(VGint location, VGsizei count, const VGfloat* value);
  friend void vgUniform4fv(VGint location, VGsizei count, const VGfloat* value);
  friend void vgUniform1iv(VGint location, VGsizei count, const VGint* value);
  friend void vgUniform2iv(VGint location, VGsizei count, const VGint* value);
  friend void vgUniform3iv(VGint location, VGsizei count, const VGint* value);
  friend void vgUniform4iv(VGint location, VGsizei count, const VGint* value);
  friend void vgUniformMatrix2fv(VGint location, VGsizei count,
                                 VGboolean transpose, const VGfloat* value);
  friend void vgUniformMatrix3fv(VGint location, VGsizei count,
                                 VGboolean transpose, const VGfloat* value);
  friend void vgUniformMatrix4fv(VGint location, VGsizei count,
                                 VGboolean transpose, const VGfloat* value);
  friend void vgValidateProgram(VGuint program);
  template <typename T>
  friend void WriteAttrib4(VirtualGPU::ConstantAttribute& a, T x, T y, T z,
                           T w);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib1t(VGuint index, T x);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib1tv(VGuint index,
                                                          const T* v);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib2t(VGuint index, T x,
                                                         T y);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib2tv(VGuint index,
                                                          const T* v);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib3t(VGuint index, T x, T y,
                                                         T z);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib3tv(VGuint index,
                                                          const T* v);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib4t(VGuint index, T x, T y,
                                                         T z, T w);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib4tv(VGuint index,
                                                          const T* v);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib4Nt(VGuint index, T x,
                                                          T y, T z, T w);
  template <typename T>
  friend VirtualGPU::ConstantAttribute* vgVertexAttrib4Ntv(VGuint index,
                                                           const T* v);
  friend void vgVertexAttrib1d(VGuint index, VGdouble x);
  friend void vgVertexAttrib1dv(VGuint index, const VGdouble* v);
  friend void vgVertexAttrib1f(VGuint index, VGfloat x);
  friend void vgVertexAttrib1fv(VGuint index, const VGfloat* v);
  friend void vgVertexAttrib1s(VGuint index, VGshort x);
  friend void vgVertexAttrib1sv(VGuint index, const VGshort* v);
  friend void vgVertexAttrib2d(VGuint index, VGdouble x, VGdouble y);
  friend void vgVertexAttrib2dv(VGuint index, const VGdouble* v);
  friend void vgVertexAttrib2f(VGuint index, VGfloat x, VGfloat y);
  friend void vgVertexAttrib2fv(VGuint index, const VGfloat* v);
  friend void vgVertexAttrib2s(VGuint index, VGshort x, VGshort y);
  friend void vgVertexAttrib2sv(VGuint index, const VGshort* v);
  friend void vgVertexAttrib3d(VGuint index, VGdouble x, VGdouble y,
                               VGdouble z);
  friend void vgVertexAttrib3dv(VGuint index, const VGdouble* v);
  friend void vgVertexAttrib3f(VGuint index, VGfloat x, VGfloat y, VGfloat z);
  friend void vgVertexAttrib3fv(VGuint index, const VGfloat* v);
  friend void vgVertexAttrib3s(VGuint index, VGshort x, VGshort y, VGshort z);
  friend void vgVertexAttrib3sv(VGuint index, const VGshort* v);
  friend void vgVertexAttrib4Nbv(VGuint index, const VGbyte* v);
  friend void vgVertexAttrib4Niv(VGuint index, const VGint* v);
  friend void vgVertexAttrib4Nsv(VGuint index, const VGshort* v);
  friend void vgVertexAttrib4Nub(VGuint index, VGubyte x, VGubyte y, VGubyte z,
                                 VGubyte w);
  friend void vgVertexAttrib4Nubv(VGuint index, const VGubyte* v);
  friend void vgVertexAttrib4Nuiv(VGuint index, const VGuint* v);
  friend void vgVertexAttrib4Nusv(VGuint index, const VGushort* v);
  friend void vgVertexAttrib4bv(VGuint index, const VGbyte* v);
  friend void vgVertexAttrib4d(VGuint index, VGdouble x, VGdouble y, VGdouble z,
                               VGdouble w);
  friend void vgVertexAttrib4dv(VGuint index, const VGdouble* v);
  friend void vgVertexAttrib4f(VGuint index, VGfloat x, VGfloat y, VGfloat z,
                               VGfloat w);
  friend void vgVertexAttrib4fv(VGuint index, const VGfloat* v);
  friend void vgVertexAttrib4iv(VGuint index, const VGint* v);
  friend void vgVertexAttrib4s(VGuint index, VGshort x, VGshort y, VGshort z,
                               VGshort w);
  friend void vgVertexAttrib4sv(VGuint index, const VGshort* v);
  friend void vgVertexAttrib4ubv(VGuint index, const VGubyte* v);
  friend void vgVertexAttrib4uiv(VGuint index, const VGuint* v);
  friend void vgVertexAttrib4usv(VGuint index, const VGushort* v);
  friend void vgVertexAttribPointer(VGuint index, VGint size, VGenum type,
                                    VGboolean normalized, VGsizei stride,
                                    const void* pointer);
  friend void vgColorMaski(VGuint index, VGboolean r, VGboolean g, VGboolean b,
                           VGboolean a);
  friend void vgGetBooleani_v(VGenum target, VGuint index, VGboolean* data);
  friend void vgGetIntegeri_v(VGenum target, VGuint index, VGint* data);
  friend void vgEnablei(VGenum target, VGuint index);
  friend void vgDisablei(VGenum target, VGuint index);
  friend VGboolean vgIsEnabledi(VGenum target, VGuint index);
  friend void vgBeginTransformFeedback(VGenum primitiveMode);
  friend void vgEndTransformFeedback(void);
  friend void vgBindBufferRange(VGenum target, VGuint index, VGuint buffer,
                                VGintptr offset, VGsizeiptr size);
  friend void vgBindBufferBase(VGenum target, VGuint index, VGuint buffer);
  friend void vgTransformFeedbackVaryings(VGuint program, VGsizei count,
                                          const VGchar* const* varyings,
                                          VGenum bufferMode);
  friend void vgGetTransformFeedbackVarying(VGuint program, VGuint index,
                                            VGsizei bufSize, VGsizei* length,
                                            VGsizei* size, VGenum* type,
                                            VGchar* name);
  friend void vgClampColor(VGenum target, VGenum clamp);
  friend void vgBeginConditionalRender(VGuint id, VGenum mode);
  friend void vgEndConditionalRender(void);
  friend void vgVertexAttribIPointer(VGuint index, VGint size, VGenum type,
                                     VGsizei stride, const void* pointer);
  friend void vgGetVertexAttribIiv(VGuint index, VGenum pname, VGint* params);
  friend void vgGetVertexAttribIuiv(VGuint index, VGenum pname, VGuint* params);
  friend void vgVertexAttribI1i(VGuint index, VGint x);
  friend void vgVertexAttribI2i(VGuint index, VGint x, VGint y);
  friend void vgVertexAttribI3i(VGuint index, VGint x, VGint y, VGint z);
  friend void vgVertexAttribI4i(VGuint index, VGint x, VGint y, VGint z,
                                VGint w);
  friend void vgVertexAttribI1ui(VGuint index, VGuint x);
  friend void vgVertexAttribI2ui(VGuint index, VGuint x, VGuint y);
  friend void vgVertexAttribI3ui(VGuint index, VGuint x, VGuint y, VGuint z);
  friend void vgVertexAttribI4ui(VGuint index, VGuint x, VGuint y, VGuint z,
                                 VGuint w);
  friend void vgVertexAttribI1iv(VGuint index, const VGint* v);
  friend void vgVertexAttribI2iv(VGuint index, const VGint* v);
  friend void vgVertexAttribI3iv(VGuint index, const VGint* v);
  friend void vgVertexAttribI4iv(VGuint index, const VGint* v);
  friend void vgVertexAttribI1uiv(VGuint index, const VGuint* v);
  friend void vgVertexAttribI2uiv(VGuint index, const VGuint* v);
  friend void vgVertexAttribI3uiv(VGuint index, const VGuint* v);
  friend void vgVertexAttribI4uiv(VGuint index, const VGuint* v);
  friend void vgVertexAttribI4bv(VGuint index, const VGbyte* v);
  friend void vgVertexAttribI4sv(VGuint index, const VGshort* v);
  friend void vgVertexAttribI4ubv(VGuint index, const VGubyte* v);
  friend void vgVertexAttribI4usv(VGuint index, const VGushort* v);
  friend void vgGetUniformuiv(VGuint program, VGint location, VGuint* params);
  friend void vgBindFragDataLocation(VGuint program, VGuint color,
                                     VGint name_hash);
  friend void vgBindFragDataLocation(VGuint program, VGuint color,
                                     const VGchar* name);
  friend VGint vgGetFragDataLocation(VGuint program, VGint name_hash);
  friend VGint vgGetFragDataLocation(VGuint program, const VGchar* name);
  friend void vgUniform1ui(VGint location, VGuint v0);
  friend void vgUniform2ui(VGint location, VGuint v0, VGuint v1);
  friend void vgUniform3ui(VGint location, VGuint v0, VGuint v1, VGuint v2);
  friend void vgUniform4ui(VGint location, VGuint v0, VGuint v1, VGuint v2,
                           VGuint v3);
  friend void vgUniform1uiv(VGint location, VGsizei count, const VGuint* value);
  friend void vgUniform2uiv(VGint location, VGsizei count, const VGuint* value);
  friend void vgUniform3uiv(VGint location, VGsizei count, const VGuint* value);
  friend void vgUniform4uiv(VGint location, VGsizei count, const VGuint* value);
  friend void vgTexParameterIiv(VGenum target, VGenum pname,
                                const VGint* params);
  friend void vgTexParameterIuiv(VGenum target, VGenum pname,
                                 const VGuint* params);
  friend void vgGetTexParameterIiv(VGenum target, VGenum pname, VGint* params);
  friend void vgGetTexParameterIuiv(VGenum target, VGenum pname,
                                    VGuint* params);
  friend void vgClearBufferiv(VGenum buffer, VGint drawbuffer,
                              const VGint* value);
  friend void vgClearBufferuiv(VGenum buffer, VGint drawbuffer,
                               const VGuint* value);
  friend void vgClearBufferfv(VGenum buffer, VGint drawbuffer,
                              const VGfloat* value);
  friend void vgClearBufferfi(VGenum buffer, VGint drawbuffer, VGfloat depth,
                              VGint stencil);
  friend const VGubyte* vgGetStringi(VGenum name, VGuint index);
  friend VGboolean vgIsRenderbuffer(VGuint renderbuffer);
  friend void vgBindRenderbuffer(VGenum target, VGuint renderbuffer);
  friend void vgDeleteRenderbuffers(VGsizei n, const VGuint* renderbuffers);
  friend void vgGenRenderbuffers(VGsizei n, VGuint* renderbuffers);
  friend void vgRenderbufferStorage(VGenum target, VGenum internalformat,
                                    VGsizei width, VGsizei height);
  friend void vgGetRenderbufferParameteriv(VGenum target, VGenum pname,
                                           VGint* params);
  friend VGboolean vgIsFramebuffer(VGuint framebuffer);
  friend void vgBindFramebuffer(VGenum target, VGuint framebuffer);
  friend void vgDeleteFramebuffers(VGsizei n, const VGuint* framebuffers);
  friend void vgGenFramebuffers(VGsizei n, VGuint* framebuffers);
  friend VGenum vgCheckFramebufferStatus(VGenum target);
  friend void vgFramebufferTexture1D(VGenum target, VGenum attachment,
                                     VGenum textarget, VGuint texture,
                                     VGint level);
  friend void vgFramebufferTexture2D(VGenum target, VGenum attachment,
                                     VGenum textarget, VGuint texture,
                                     VGint level);
  friend void vgFramebufferTexture3D(VGenum target, VGenum attachment,
                                     VGenum textarget, VGuint texture,
                                     VGint level, VGint zoffset);
  friend void vgFramebufferRenderbuffer(VGenum target, VGenum attachment,
                                        VGenum renderbuffertarget,
                                        VGuint renderbuffer);
  friend void vgGetFramebufferAttachmentParameteriv(VGenum target,
                                                    VGenum attachment,
                                                    VGenum pname,
                                                    VGint* params);
  friend void vgGenerateMipmap(VGenum target);
  friend void vgBlitFramebuffer(VGint srcX0, VGint srcY0, VGint srcX1,
                                VGint srcY1, VGint dstX0, VGint dstY0,
                                VGint dstX1, VGint dstY1, VGbitfield mask,
                                VGenum filter);
  friend void vgRenderbufferStorageMultisample(VGenum target, VGsizei samples,
                                               VGenum internalformat,
                                               VGsizei width, VGsizei height);
  friend void vgFramebufferTextureLayer(VGenum target, VGenum attachment,
                                        VGuint texture, VGint level,
                                        VGint layer);
  friend void* vgMapBufferRange(VGenum target, VGintptr offset,
                                VGsizeiptr length, VGbitfield access);
  friend void vgFlushMappedBufferRange(VGenum target, VGintptr offset,
                                       VGsizeiptr length);
  friend void vgBindVertexArray(VGuint array);
  friend void vgDeleteVertexArrays(VGsizei n, const VGuint* arrays);
  friend void vgGenVertexArrays(VGsizei n, VGuint* arrays);
  friend VGboolean vgIsVertexArray(VGuint array);

  friend void vgDrawArraysInstanced(VGenum mode, VGint first, VGsizei count,
                                    VGsizei instancecount);
  friend void vgDrawElementsInstanced(VGenum mode, VGsizei count, VGenum type,
                                      const void* indices,
                                      VGsizei instancecount);
  friend void vgTexBuffer(VGenum target, VGenum internalformat, VGuint buffer);
  friend void vgPrimitiveRestartIndex(VGuint index);
  friend void vgCopyBufferSubData(VGenum readTarget, VGenum writeTarget,
                                  VGintptr readOffset, VGintptr writeOffset,
                                  VGsizeiptr size);
  friend void vgGetUniformIndices(VGuint program, VGsizei uniformCount,
                                  const VGchar* const* uniformNames,
                                  VGuint* uniformIndices);
  friend void vgGetActiveUniformsiv(VGuint program, VGsizei uniformCount,
                                    const VGuint* uniformIndices, VGenum pname,
                                    VGint* params);
  friend void vgGetActiveUniformName(VGuint program, VGuint uniformIndex,
                                     VGsizei bufSize, VGsizei* length,
                                     VGchar* uniformName);
  friend VGuint vgGetUniformBlockIndex(VGuint program,
                                       const VGchar* uniformBlockName);
  friend void vgGetActiveUniformBlockiv(VGuint program,
                                        VGuint uniformBlockIndex, VGenum pname,
                                        VGint* params);
  friend void vgGetActiveUniformBlockName(VGuint program,
                                          VGuint uniformBlockIndex,
                                          VGsizei bufSize, VGsizei* length,
                                          VGchar* uniformBlockName);
  friend void vgUniformBlockBinding(VGuint program, VGuint uniformBlockIndex,
                                    VGuint uniformBlockBinding);

  friend void vgDrawElementsBaseVertex(VGenum mode, VGsizei count, VGenum type,
                                       const void* indices, VGint basevertex);
  friend void vgDrawRangeElementsBaseVertex(VGenum mode, VGuint start,
                                            VGuint end, VGsizei count,
                                            VGenum type, const void* indices,
                                            VGint basevertex);
  friend void vgDrawElementsInstancedBaseVertex(VGenum mode, VGsizei count,
                                                VGenum type,
                                                const void* indices,
                                                VGsizei instancecount,
                                                VGint basevertex);
  friend void vgMultiDrawElementsBaseVertex(VGenum mode, const VGsizei* count,
                                            VGenum type,
                                            const void* const* indices,
                                            VGsizei drawcount,
                                            const VGint* basevertex);
  friend void vgProvokingVertex(VGenum mode);
  friend VGsync vgFenceSync(VGenum condition, VGbitfield flags);
  friend VGboolean vgIsSync(VGsync sync);
  friend void vgDeleteSync(VGsync sync);
  friend VGenum vgClientWaitSync(VGsync sync, VGbitfield flags,
                                 VGuint64 timeout);
  friend void vgWaitSync(VGsync sync, VGbitfield flags, VGuint64 timeout);
  friend void vgGetInteger64v(VGenum pname, VGint64* data);
  friend void vgGetSynciv(VGsync sync, VGenum pname, VGsizei count,
                          VGsizei* length, VGint* values);
  friend void vgGetInteger64i_v(VGenum target, VGuint index, VGint64* data);
  friend void vgGetBufferParameteri64v(VGenum target, VGenum pname,
                                       VGint64* params);
  friend void vgFramebufferTexture(VGenum target, VGenum attachment,
                                   VGuint texture, VGint level);
  friend void vgTexImage2DMultisample(VGenum target, VGsizei samples,
                                      VGenum internalformat, VGsizei width,
                                      VGsizei height,
                                      VGboolean fixedsamplelocations);
  friend void vgTexImage3DMultisample(VGenum target, VGsizei samples,
                                      VGenum internalformat, VGsizei width,
                                      VGsizei height, VGsizei depth,
                                      VGboolean fixedsamplelocations);
  friend void vgGetMultisamplefv(VGenum pname, VGuint index, VGfloat* val);
  friend void vgSampleMaski(VGuint maskNumber, VGbitfield mask);

  friend void vgBindFragDataLocationIndexed(VGuint program, VGuint colorNumber,
                                            VGuint index, const VGchar* name);
  friend VGint vgGetFragDataIndex(VGuint program, const VGchar* name);
  friend void vgGenSamplers(VGsizei count, VGuint* samplers);
  friend void vgDeleteSamplers(VGsizei count, const VGuint* samplers);
  friend VGboolean vgIsSampler(VGuint sampler);
  friend void vgBindSampler(VGuint unit, VGuint sampler);
  friend void vgSamplerParameteri(VGuint sampler, VGenum pname, VGint param);
  friend void vgSamplerParameteriv(VGuint sampler, VGenum pname,
                                   const VGint* param);
  friend void vgSamplerParameterf(VGuint sampler, VGenum pname, VGfloat param);
  friend void vgSamplerParameterfv(VGuint sampler, VGenum pname,
                                   const VGfloat* param);
  friend void vgSamplerParameterIiv(VGuint sampler, VGenum pname,
                                    const VGint* param);
  friend void vgSamplerParameterIuiv(VGuint sampler, VGenum pname,
                                     const VGuint* param);
  friend void vgGetSamplerParameteriv(VGuint sampler, VGenum pname,
                                      VGint* params);
  friend void vgGetSamplerParameterIiv(VGuint sampler, VGenum pname,
                                       VGint* params);
  friend void vgGetSamplerParameterfv(VGuint sampler, VGenum pname,
                                      VGfloat* params);
  friend void vgGetSamplerParameterIuiv(VGuint sampler, VGenum pname,
                                        VGuint* params);
  friend void vgQueryCounter(VGuint id, VGenum target);
  friend void vgGetQueryObjecti64v(VGuint id, VGenum pname, VGint64* params);
  friend void vgGetQueryObjectui64v(VGuint id, VGenum pname, VGuint64* params);
  friend void vgVertexAttribDivisor(VGuint index, VGuint divisor);
  friend void vgVertexAttribP1ui(VGuint index, VGenum type,
                                 VGboolean normalized, VGuint value);
  friend void vgVertexAttribP1uiv(VGuint index, VGenum type,
                                  VGboolean normalized, const VGuint* value);
  friend void vgVertexAttribP2ui(VGuint index, VGenum type,
                                 VGboolean normalized, VGuint value);
  friend void vgVertexAttribP2uiv(VGuint index, VGenum type,
                                  VGboolean normalized, const VGuint* value);
  friend void vgVertexAttribP3ui(VGuint index, VGenum type,
                                 VGboolean normalized, VGuint value);
  friend void vgVertexAttribP3uiv(VGuint index, VGenum type,
                                  VGboolean normalized, const VGuint* value);
  friend void vgVertexAttribP4ui(VGuint index, VGenum type,
                                 VGboolean normalized, VGuint value);
  friend void vgVertexAttribP4uiv(VGuint index, VGenum type,
                                  VGboolean normalized, const VGuint* value);

  friend class VirtualGPUTestAccessor;
};
}  // namespace ho
