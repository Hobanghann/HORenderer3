#pragma once

#include <cstdint>

#include "core/macros.h"

namespace ho {

//////////////////////////////////////////////////
// GL VERSION 1.0 API
//////////////////////////////////////////////////

using VGvoid = void;
using VGenum = unsigned int;
using VGfloat = float;
using VGint = int;
using VGsizei = int;
using VGbitfield = unsigned int;
using VGdouble = double;
using VGuint = unsigned int;
using VGboolean = uint8_t;
using VGubyte = uint8_t;

INLINE constexpr VGenum VG_DEPTH_BUFFER_BIT = 0x00000100;
INLINE constexpr VGenum VG_STENCIL_BUFFER_BIT = 0x00000400;
INLINE constexpr VGenum VG_COLOR_BUFFER_BIT = 0x00004000;
INLINE constexpr VGenum VG_FALSE = 0;
INLINE constexpr VGenum VG_TRUE = 1;
INLINE constexpr VGenum VG_POINTS = 0x0000;
INLINE constexpr VGenum VG_LINES = 0x0001;
INLINE constexpr VGenum VG_LINE_LOOP = 0x0002;
INLINE constexpr VGenum VG_LINE_STRIP = 0x0003;
INLINE constexpr VGenum VG_TRIANGLES = 0x0004;
INLINE constexpr VGenum VG_TRIANGLE_STRIP = 0x0005;
INLINE constexpr VGenum VG_TRIANGLE_FAN = 0x0006;
INLINE constexpr VGenum VG_QUADS = 0x0007;
INLINE constexpr VGenum VG_NEVER = 0x0200;
INLINE constexpr VGenum VG_LESS = 0x0201;
INLINE constexpr VGenum VG_EQUAL = 0x0202;
INLINE constexpr VGenum VG_LEQUAL = 0x0203;
INLINE constexpr VGenum VG_GREATER = 0x0204;
INLINE constexpr VGenum VG_NOTEQUAL = 0x0205;
INLINE constexpr VGenum VG_GEQUAL = 0x0206;
INLINE constexpr VGenum VG_ALWAYS = 0x0207;
INLINE constexpr VGenum VG_ZERO = 0;
INLINE constexpr VGenum VG_ONE = 1;
INLINE constexpr VGenum VG_SRC_COLOR = 0x0300;
INLINE constexpr VGenum VG_ONE_MINUS_SRC_COLOR = 0x0301;
INLINE constexpr VGenum VG_SRC_ALPHA = 0x0302;
INLINE constexpr VGenum VG_ONE_MINUS_SRC_ALPHA = 0x0303;
INLINE constexpr VGenum VG_DST_ALPHA = 0x0304;
INLINE constexpr VGenum VG_ONE_MINUS_DST_ALPHA = 0x0305;
INLINE constexpr VGenum VG_DST_COLOR = 0x0306;
INLINE constexpr VGenum VG_ONE_MINUS_DST_COLOR = 0x0307;
INLINE constexpr VGenum VG_SRC_ALPHA_SATURATE = 0x0308;
INLINE constexpr VGenum VG_NONE = 0;
INLINE constexpr VGenum VG_FRONT_LEFT = 0x0400;
INLINE constexpr VGenum VG_FRONT_RIGHT = 0x0401;
INLINE constexpr VGenum VG_BACK_LEFT = 0x0402;
INLINE constexpr VGenum VG_BACK_RIGHT = 0x0403;
INLINE constexpr VGenum VG_FRONT = 0x0404;
INLINE constexpr VGenum VG_BACK = 0x0405;
INLINE constexpr VGenum VG_LEFT = 0x0406;
INLINE constexpr VGenum VG_RIGHT = 0x0407;
INLINE constexpr VGenum VG_FRONT_AND_BACK = 0x0408;
INLINE constexpr VGenum VG_NO_ERROR = 0;
INLINE constexpr VGenum VG_INVALID_ENUM = 0x0500;
INLINE constexpr VGenum VG_INVALID_VALUE = 0x0501;
INLINE constexpr VGenum VG_INVALID_OPERATION = 0x0502;
INLINE constexpr VGenum VG_OUT_OF_MEMORY = 0x0505;
INLINE constexpr VGenum VG_CW = 0x0900;
INLINE constexpr VGenum VG_CCW = 0x0901;
INLINE constexpr VGenum VG_POINT_SIZE = 0x0B11;
INLINE constexpr VGenum VG_POINT_SIZE_RANGE = 0x0B12;
INLINE constexpr VGenum VG_POINT_SIZE_GRANULARITY = 0x0B13;
INLINE constexpr VGenum VG_LINE_SMOOTH = 0x0B20;
INLINE constexpr VGenum VG_LINE_WIDTH = 0x0B21;
INLINE constexpr VGenum VG_LINE_WIDTH_RANGE = 0x0B22;
INLINE constexpr VGenum VG_LINE_WIDTH_GRANULARITY = 0x0B23;
INLINE constexpr VGenum VG_POLYGON_MODE = 0x0B40;
INLINE constexpr VGenum VG_POLYGON_SMOOTH = 0x0B41;
INLINE constexpr VGenum VG_CULL_FACE = 0x0B44;
INLINE constexpr VGenum VG_CULL_FACE_MODE = 0x0B45;
INLINE constexpr VGenum VG_FRONT_FACE = 0x0B46;
INLINE constexpr VGenum VG_DEPTH_RANGE = 0x0B70;
INLINE constexpr VGenum VG_DEPTH_TEST = 0x0B71;
INLINE constexpr VGenum VG_DEPTH_WRITEMASK = 0x0B72;
INLINE constexpr VGenum VG_DEPTH_CLEAR_VALUE = 0x0B73;
INLINE constexpr VGenum VG_DEPTH_FUNC = 0x0B74;
INLINE constexpr VGenum VG_STENCIL_TEST = 0x0B90;
INLINE constexpr VGenum VG_STENCIL_CLEAR_VALUE = 0x0B91;
INLINE constexpr VGenum VG_STENCIL_FUNC = 0x0B92;
INLINE constexpr VGenum VG_STENCIL_VALUE_MASK = 0x0B93;
INLINE constexpr VGenum VG_STENCIL_FAIL = 0x0B94;
INLINE constexpr VGenum VG_STENCIL_PASS_DEPTH_FAIL = 0x0B95;
INLINE constexpr VGenum VG_STENCIL_PASS_DEPTH_PASS = 0x0B96;
INLINE constexpr VGenum VG_STENCIL_REF = 0x0B97;
INLINE constexpr VGenum VG_STENCIL_WRITEMASK = 0x0B98;
INLINE constexpr VGenum VG_VIEWPORT = 0x0BA2;
INLINE constexpr VGenum VG_DITHER = 0x0BD0;
INLINE constexpr VGenum VG_BLEND_DST = 0x0BE0;
INLINE constexpr VGenum VG_BLEND_SRC = 0x0BE1;
INLINE constexpr VGenum VG_BLEND = 0x0BE2;
INLINE constexpr VGenum VG_LOGIC_OP_MODE = 0x0BF0;
INLINE constexpr VGenum VG_DRAW_BUFFER = 0x0C01;
INLINE constexpr VGenum VG_READ_BUFFER = 0x0C02;
INLINE constexpr VGenum VG_SCISSOR_BOX = 0x0C10;
INLINE constexpr VGenum VG_SCISSOR_TEST = 0x0C11;
INLINE constexpr VGenum VG_COLOR_CLEAR_VALUE = 0x0C22;
INLINE constexpr VGenum VG_COLOR_WRITEMASK = 0x0C23;
INLINE constexpr VGenum VG_DOUBLEBUFFER = 0x0C32;
INLINE constexpr VGenum VG_STEREO = 0x0C33;
INLINE constexpr VGenum VG_LINE_SMOOTH_HINT = 0x0C52;
INLINE constexpr VGenum VG_POLYGON_SMOOTH_HINT = 0x0C53;
INLINE constexpr VGenum VG_UNPACK_SWAP_BYTES = 0x0CF0;
INLINE constexpr VGenum VG_UNPACK_LSB_FIRST = 0x0CF1;
INLINE constexpr VGenum VG_UNPACK_ROW_LENGTH = 0x0CF2;
INLINE constexpr VGenum VG_UNPACK_SKIP_ROWS = 0x0CF3;
INLINE constexpr VGenum VG_UNPACK_SKIP_PIXELS = 0x0CF4;
INLINE constexpr VGenum VG_UNPACK_ALIGNMENT = 0x0CF5;
INLINE constexpr VGenum VG_PACK_SWAP_BYTES = 0x0D00;
INLINE constexpr VGenum VG_PACK_LSB_FIRST = 0x0D01;
INLINE constexpr VGenum VG_PACK_ROW_LENGTH = 0x0D02;
INLINE constexpr VGenum VG_PACK_SKIP_ROWS = 0x0D03;
INLINE constexpr VGenum VG_PACK_SKIP_PIXELS = 0x0D04;
INLINE constexpr VGenum VG_PACK_ALIGNMENT = 0x0D05;
INLINE constexpr VGenum VG_MAX_TEXTURE_SIZE = 0x0D33;
INLINE constexpr VGenum VG_MAX_VIEWPORT_DIMS = 0x0D3A;
INLINE constexpr VGenum VG_SUBPIXEL_BITS = 0x0D50;
INLINE constexpr VGenum VG_TEXTURE_1D = 0x0DE0;
INLINE constexpr VGenum VG_TEXTURE_2D = 0x0DE1;
INLINE constexpr VGenum VG_TEXTURE_WIDTH = 0x1000;
INLINE constexpr VGenum VG_TEXTURE_HEIGHT = 0x1001;
INLINE constexpr VGenum VG_TEXTURE_BORDER_COLOR = 0x1004;
INLINE constexpr VGenum VG_DONT_CARE = 0x1100;
INLINE constexpr VGenum VG_FASTEST = 0x1101;
INLINE constexpr VGenum VG_NICEST = 0x1102;
INLINE constexpr VGenum VG_BYTE = 0x1400;
INLINE constexpr VGenum VG_UNSIGNED_BYTE = 0x1401;
INLINE constexpr VGenum VG_SHORT = 0x1402;
INLINE constexpr VGenum VG_UNSIGNED_SHORT = 0x1403;
INLINE constexpr VGenum VG_INT = 0x1404;
INLINE constexpr VGenum VG_UNSIGNED_INT = 0x1405;
INLINE constexpr VGenum VG_FLOAT = 0x1406;
INLINE constexpr VGenum VG_STACK_OVERFLOW = 0x0503;
INLINE constexpr VGenum VG_STACK_UNDERFLOW = 0x0504;
INLINE constexpr VGenum VG_CLEAR = 0x1500;
INLINE constexpr VGenum VG_AND = 0x1501;
INLINE constexpr VGenum VG_AND_REVERSE = 0x1502;
INLINE constexpr VGenum VG_COPY = 0x1503;
INLINE constexpr VGenum VG_AND_INVERTED = 0x1504;
INLINE constexpr VGenum VG_NOOP = 0x1505;
INLINE constexpr VGenum VG_XOR = 0x1506;
INLINE constexpr VGenum VG_OR = 0x1507;
INLINE constexpr VGenum VG_NOR = 0x1508;
INLINE constexpr VGenum VG_EQUIV = 0x1509;
INLINE constexpr VGenum VG_INVERT = 0x150A;
INLINE constexpr VGenum VG_OR_REVERSE = 0x150B;
INLINE constexpr VGenum VG_COPY_INVERTED = 0x150C;
INLINE constexpr VGenum VG_OR_INVERTED = 0x150D;
INLINE constexpr VGenum VG_NAND = 0x150E;
INLINE constexpr VGenum VG_SET = 0x150F;
INLINE constexpr VGenum VG_TEXTURE = 0x1702;
INLINE constexpr VGenum VG_COLOR = 0x1800;
INLINE constexpr VGenum VG_DEPTH = 0x1801;
INLINE constexpr VGenum VG_STENCIL = 0x1802;
INLINE constexpr VGenum VG_STENCIL_INDEX = 0x1901;
INLINE constexpr VGenum VG_DEPTH_COMPONENT = 0x1902;
INLINE constexpr VGenum VG_RED = 0x1903;
INLINE constexpr VGenum VG_GREEN = 0x1904;
INLINE constexpr VGenum VG_BLUE = 0x1905;
INLINE constexpr VGenum VG_ALPHA = 0x1906;
INLINE constexpr VGenum VG_RGB = 0x1907;
INLINE constexpr VGenum VG_RGBA = 0x1908;
INLINE constexpr VGenum VG_POINT = 0x1B00;
INLINE constexpr VGenum VG_LINE = 0x1B01;
INLINE constexpr VGenum VG_FILL = 0x1B02;
INLINE constexpr VGenum VG_KEEP = 0x1E00;
INLINE constexpr VGenum VG_REPLACE = 0x1E01;
INLINE constexpr VGenum VG_INCR = 0x1E02;
INLINE constexpr VGenum VG_DECR = 0x1E03;
INLINE constexpr VGenum VG_VENDOR = 0x1F00;
INLINE constexpr VGenum VG_RENDERER = 0x1F01;
INLINE constexpr VGenum VG_VERSION = 0x1F02;
INLINE constexpr VGenum VG_EXTENSIONS = 0x1F03;
INLINE constexpr VGenum VG_NEAREST = 0x2600;
INLINE constexpr VGenum VG_LINEAR = 0x2601;
INLINE constexpr VGenum VG_NEAREST_MIPMAP_NEAREST = 0x2700;
INLINE constexpr VGenum VG_LINEAR_MIPMAP_NEAREST = 0x2701;
INLINE constexpr VGenum VG_NEAREST_MIPMAP_LINEAR = 0x2702;
INLINE constexpr VGenum VG_LINEAR_MIPMAP_LINEAR = 0x2703;
INLINE constexpr VGenum VG_TEXTURE_MAG_FILTER = 0x2800;
INLINE constexpr VGenum VG_TEXTURE_MIN_FILTER = 0x2801;
INLINE constexpr VGenum VG_TEXTURE_WRAP_S = 0x2802;
INLINE constexpr VGenum VG_TEXTURE_WRAP_T = 0x2803;
INLINE constexpr VGenum VG_REPEAT = 0x2901;

void vgCullFace(VGenum mode);
void vgFrontFace(VGenum mode);
// void vgHint(VGenum target, VGenum mode);
// void vgLineWidth(VGfloat width);
// void vgPointSize(VGfloat size);
void vgPolygonMode(VGenum face, VGenum mode);
void vgScissor(VGint x, VGint y, VGsizei width, VGsizei height);
void vgTexParameterf(VGenum target, VGenum pname, VGfloat param);
void vgTexParameterfv(VGenum target, VGenum pname, const VGfloat* params);
void vgTexParameteri(VGenum target, VGenum pname, VGint param);
void vgTexParameteriv(VGenum target, VGenum pname, const VGint* params);
void vgTexImage1D(VGenum target, VGint level, VGint internalformat,
                  VGsizei width, VGint border, VGenum format, VGenum type,
                  const void* pixels);
void vgTexImage2D(VGenum target, VGint level, VGint internalformat,
                  VGsizei width, VGsizei height, VGint border, VGenum format,
                  VGenum type, const void* pixels);
void vgDrawBuffer(VGenum buf);
void vgClear(VGbitfield mask);
void vgClearColor(VGfloat red, VGfloat green, VGfloat blue, VGfloat alpha);
void vgClearStencil(VGint s);
void vgClearDepth(VGdouble depth);
void vgStencilMask(VGuint mask);
void vgColorMask(VGboolean red, VGboolean green, VGboolean blue,
                 VGboolean alpha);
void vgDepthMask(VGboolean flag);
void vgDisable(VGenum cap);
void vgEnable(VGenum cap);
void vgFinish(void);
void vgFlush(void);
void vgBlendFunc(VGenum sfactor, VGenum dfactor);
// void vgLogicOp(VGenum opcode);
void vgStencilFunc(VGenum func, VGint ref, VGuint mask);
void vgStencilOp(VGenum fail, VGenum zfail, VGenum zpass);
void vgDepthFunc(VGenum func);
void vgPixelStoref(VGenum pname, VGfloat param);
void vgPixelStorei(VGenum pname, VGint param);
void vgReadBuffer(VGenum src);
// void vgReadPixels(VGint x, VGint y, VGsizei width, VGsizei height,
//                   VGenum format, VGenum type, void* pixels);
// void vgGetBooleanv(VGenum pname, VGboolean* data);
// void vgGetDoublev(VGenum pname, VGdouble* data);
VGenum vgGetError(void);
// void vgGetFloatv(VGenum pname, VGfloat* data);
// void vgGetIntegerv(VGenum pname, VGint* data);
// const VGubyte* vgGetString(VGenum name);
void vgGetTexImage(VGenum target, VGint level, VGenum format, VGenum type,
                   void* pixels);
// void vgGetTexParameterfv(VGenum target, VGenum pname, VGfloat* params);
// void vgGetTexParameteriv(VGenum target, VGenum pname, VGint* params);
// void vgGetTexLevelParameterfv(VGenum target, VGint level, VGenum pname,
//                               VGfloat* params);
// void vgGetTexLevelParameteriv(VGenum target, VGint level, VGenum pname,
//                               VGint* params);
VGboolean vgIsEnabled(VGenum cap);
void vgDepthRange(VGdouble n, VGdouble f);
void vgViewport(VGint x, VGint y, VGsizei width, VGsizei height);

//////////////////////////////////////////////////
// GL VERSION 1.1 API
//////////////////////////////////////////////////
using VGclampf = float;
using VGclampd = double;

INLINE constexpr VGenum VG_COLOR_LOGIC_OP = 0x0BF2;
INLINE constexpr VGenum VG_POLYGON_OFFSET_UNITS = 0x2A00;
INLINE constexpr VGenum VG_POLYGON_OFFSET_POINT = 0x2A01;
INLINE constexpr VGenum VG_POLYGON_OFFSET_LINE = 0x2A02;
INLINE constexpr VGenum VG_POLYGON_OFFSET_FILL = 0x8037;
INLINE constexpr VGenum VG_POLYGON_OFFSET_FACTOR = 0x8038;
INLINE constexpr VGenum VG_TEXTURE_BINDING_1D = 0x8068;
INLINE constexpr VGenum VG_TEXTURE_BINDING_2D = 0x8069;
INLINE constexpr VGenum VG_TEXTURE_INTERNAL_FORMAT = 0x1003;
INLINE constexpr VGenum VG_TEXTURE_RED_SIZE = 0x805C;
INLINE constexpr VGenum VG_TEXTURE_GREEN_SIZE = 0x805D;
INLINE constexpr VGenum VG_TEXTURE_BLUE_SIZE = 0x805E;
INLINE constexpr VGenum VG_TEXTURE_ALPHA_SIZE = 0x805F;
INLINE constexpr VGenum VG_DOUBLE = 0x140A;
INLINE constexpr VGenum VG_PROXY_TEXTURE_1D = 0x8063;
INLINE constexpr VGenum VG_PROXY_TEXTURE_2D = 0x8064;
INLINE constexpr VGenum VG_R3_G3_B2 = 0x2A10;
INLINE constexpr VGenum VG_RGB4 = 0x804F;
INLINE constexpr VGenum VG_RGB5 = 0x8050;
INLINE constexpr VGenum VG_RGB8 = 0x8051;
INLINE constexpr VGenum VG_RGB10 = 0x8052;
INLINE constexpr VGenum VG_RGB12 = 0x8053;
INLINE constexpr VGenum VG_RGB16 = 0x8054;
INLINE constexpr VGenum VG_RGBA2 = 0x8055;
INLINE constexpr VGenum VG_RGBA4 = 0x8056;
INLINE constexpr VGenum VG_RGB5_A1 = 0x8057;
INLINE constexpr VGenum VG_RGBA8 = 0x8058;
INLINE constexpr VGenum VG_RGB10_A2 = 0x8059;
INLINE constexpr VGenum VG_RGBA12 = 0x805A;
INLINE constexpr VGenum VG_RGBA16 = 0x805B;
INLINE constexpr VGenum VG_VERTEX_ARRAY = 0x8074;

void vgDrawArrays(VGenum mode, VGint first, VGsizei count);
void vgDrawElements(VGenum mode, VGsizei count, VGenum type,
                    const void* indices);
// void vgGetPointerv(VGenum pname, void** params);
void vgPolygonOffset(VGfloat factor, VGfloat units);
// void vgCopyTexImage1D(VGenum target, VGint level, VGenum internalformat,
//                       VGint x, VGint y, VGsizei width, VGint border);
// void vgCopyTexImage2D(VGenum target, VGint level, VGenum internalformat,
//                       VGint x, VGint y, VGsizei width, VGsizei height,
//                       VGint border);
// void vgCopyTexSubImage1D(VGenum target, VGint level, VGint xoffset, VGint x,
//                          VGint y, VGsizei width);
// void vgCopyTexSubImage2D(VGenum target, VGint level, VGint xoffset,
//                          VGint yoffset, VGint x, VGint y, VGsizei width,
//                          VGsizei height);
void vgTexSubImage1D(VGenum target, VGint level, VGint xoffset, VGsizei width,
                     VGenum format, VGenum type, const void* pixels);
void vgTexSubImage2D(VGenum target, VGint level, VGint xoffset, VGint yoffset,
                     VGsizei width, VGsizei height, VGenum format, VGenum type,
                     const void* pixels);
void vgBindTexture(VGenum target, VGuint texture);
void vgDeleteTextures(VGsizei n, const VGuint* textures);
void vgGenTextures(VGsizei n, VGuint* textures);
VGboolean vgIsTexture(VGuint texture);

//////////////////////////////////////////////////
// GL VERSION 1.2 API
//////////////////////////////////////////////////
INLINE constexpr VGenum VG_UNSIGNED_BYTE_3_3_2 = 0x8032;
INLINE constexpr VGenum VG_UNSIGNED_SHORT_4_4_4_4 = 0x8033;
INLINE constexpr VGenum VG_UNSIGNED_SHORT_5_5_5_1 = 0x8034;
INLINE constexpr VGenum VG_UNSIGNED_INT_8_8_8_8 = 0x8035;
INLINE constexpr VGenum VG_UNSIGNED_INT_10_10_10_2 = 0x8036;
INLINE constexpr VGenum VG_TEXTURE_BINDING_3D = 0x806A;
INLINE constexpr VGenum VG_PACK_SKIP_IMAGES = 0x806B;
INLINE constexpr VGenum VG_PACK_IMAGE_HEIGHT = 0x806C;
INLINE constexpr VGenum VG_UNPACK_SKIP_IMAGES = 0x806D;
INLINE constexpr VGenum VG_UNPACK_IMAGE_HEIGHT = 0x806E;
INLINE constexpr VGenum VG_TEXTURE_3D = 0x806F;
INLINE constexpr VGenum VG_PROXY_TEXTURE_3D = 0x8070;
INLINE constexpr VGenum VG_TEXTURE_DEPTH = 0x8071;
INLINE constexpr VGenum VG_TEXTURE_WRAP_R = 0x8072;
INLINE constexpr VGenum VG_MAX_3D_TEXTURE_SIZE = 0x8073;
INLINE constexpr VGenum VG_UNSIGNED_BYTE_2_3_3_REV = 0x8362;
INLINE constexpr VGenum VG_UNSIGNED_SHORT_5_6_5 = 0x8363;
INLINE constexpr VGenum VG_UNSIGNED_SHORT_5_6_5_REV = 0x8364;
INLINE constexpr VGenum VG_UNSIGNED_SHORT_4_4_4_4_REV = 0x8365;
INLINE constexpr VGenum VG_UNSIGNED_SHORT_1_5_5_5_REV = 0x8366;
INLINE constexpr VGenum VG_UNSIGNED_INT_8_8_8_8_REV = 0x8367;
INLINE constexpr VGenum VG_UNSIGNED_INT_2_10_10_10_REV = 0x8368;
INLINE constexpr VGenum VG_BGR = 0x80E0;
INLINE constexpr VGenum VG_BGRA = 0x80E1;
INLINE constexpr VGenum VG_MAX_ELEMENTS_VERTICES = 0x80E8;
INLINE constexpr VGenum VG_MAX_ELEMENTS_INDICES = 0x80E9;
INLINE constexpr VGenum VG_CLAMP_TO_EDGE = 0x812F;
INLINE constexpr VGenum VG_TEXTURE_MIN_LOD = 0x813A;
INLINE constexpr VGenum VG_TEXTURE_MAX_LOD = 0x813B;
INLINE constexpr VGenum VG_TEXTURE_BASE_LEVEL = 0x813C;
INLINE constexpr VGenum VG_TEXTURE_MAX_LEVEL = 0x813D;
INLINE constexpr VGenum VG_SMOOTH_POINT_SIZE_RANGE = 0x0B12;
INLINE constexpr VGenum VG_SMOOTH_POINT_SIZE_GRANULARITY = 0x0B13;
INLINE constexpr VGenum VG_SMOOTH_LINE_WIDTH_RANGE = 0x0B22;
INLINE constexpr VGenum VG_SMOOTH_LINE_WIDTH_GRANULARITY = 0x0B23;
INLINE constexpr VGenum VG_ALIASED_LINE_WIDTH_RANGE = 0x846E;

void vgDrawRangeElements(VGenum mode, VGuint start, VGuint end, VGsizei count,
                         VGenum type, const void* indices);
void vgTexImage3D(VGenum target, VGint level, VGint internalformat,
                  VGsizei width, VGsizei height, VGsizei depth, VGint border,
                  VGenum format, VGenum type, const void* pixels);
void vgTexSubImage3D(VGenum target, VGint level, VGint xoffset, VGint yoffset,
                     VGint zoffset, VGsizei width, VGsizei height,
                     VGsizei depth, VGenum format, VGenum type,
                     const void* pixels);
// void vgCopyTexSubImage3D(VGenum target, VGint level, VGint xoffset,
//                          VGint yoffset, VGint zoffset, VGint x, VGint y,
//                          VGsizei width, VGsizei height);

//////////////////////////////////////////////////
// GL VERSION 1.3 API
//////////////////////////////////////////////////
INLINE constexpr VGenum VG_TEXTURE0 = 0x84C0;
INLINE constexpr VGenum VG_TEXTURE1 = 0x84C1;
INLINE constexpr VGenum VG_TEXTURE2 = 0x84C2;
INLINE constexpr VGenum VG_TEXTURE3 = 0x84C3;
INLINE constexpr VGenum VG_TEXTURE4 = 0x84C4;
INLINE constexpr VGenum VG_TEXTURE5 = 0x84C5;
INLINE constexpr VGenum VG_TEXTURE6 = 0x84C6;
INLINE constexpr VGenum VG_TEXTURE7 = 0x84C7;
INLINE constexpr VGenum VG_TEXTURE8 = 0x84C8;
INLINE constexpr VGenum VG_TEXTURE9 = 0x84C9;
INLINE constexpr VGenum VG_TEXTURE10 = 0x84CA;
INLINE constexpr VGenum VG_TEXTURE11 = 0x84CB;
INLINE constexpr VGenum VG_TEXTURE12 = 0x84CC;
INLINE constexpr VGenum VG_TEXTURE13 = 0x84CD;
INLINE constexpr VGenum VG_TEXTURE14 = 0x84CE;
INLINE constexpr VGenum VG_TEXTURE15 = 0x84CF;
INLINE constexpr VGenum VG_TEXTURE16 = 0x84D0;
INLINE constexpr VGenum VG_TEXTURE17 = 0x84D1;
INLINE constexpr VGenum VG_TEXTURE18 = 0x84D2;
INLINE constexpr VGenum VG_TEXTURE19 = 0x84D3;
INLINE constexpr VGenum VG_TEXTURE20 = 0x84D4;
INLINE constexpr VGenum VG_TEXTURE21 = 0x84D5;
INLINE constexpr VGenum VG_TEXTURE22 = 0x84D6;
INLINE constexpr VGenum VG_TEXTURE23 = 0x84D7;
INLINE constexpr VGenum VG_TEXTURE24 = 0x84D8;
INLINE constexpr VGenum VG_TEXTURE25 = 0x84D9;
INLINE constexpr VGenum VG_TEXTURE26 = 0x84DA;
INLINE constexpr VGenum VG_TEXTURE27 = 0x84DB;
INLINE constexpr VGenum VG_TEXTURE28 = 0x84DC;
INLINE constexpr VGenum VG_TEXTURE29 = 0x84DD;
INLINE constexpr VGenum VG_TEXTURE30 = 0x84DE;
INLINE constexpr VGenum VG_TEXTURE31 = 0x84DF;
INLINE constexpr VGenum VG_ACTIVE_TEXTURE = 0x84E0;
INLINE constexpr VGenum VG_MULTISAMPLE = 0x809D;
INLINE constexpr VGenum VG_SAMPLE_ALPHA_TO_COVERAGE = 0x809E;
INLINE constexpr VGenum VG_SAMPLE_ALPHA_TO_ONE = 0x809F;
INLINE constexpr VGenum VG_SAMPLE_COVERAGE = 0x80A0;
INLINE constexpr VGenum VG_SAMPLE_BUFFERS = 0x80A8;
INLINE constexpr VGenum VG_SAMPLES = 0x80A9;
INLINE constexpr VGenum VG_SAMPLE_COVERAGE_VALUE = 0x80AA;
INLINE constexpr VGenum VG_SAMPLE_COVERAGE_INVERT = 0x80AB;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP = 0x8513;
INLINE constexpr VGenum VG_TEXTURE_BINDING_CUBE_MAP = 0x8514;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A;
INLINE constexpr VGenum VG_PROXY_TEXTURE_CUBE_MAP = 0x851B;
INLINE constexpr VGenum VG_MAX_CUBE_MAP_TEXTURE_SIZE = 0x851C;
INLINE constexpr VGenum VG_COMPRESSED_RGB = 0x84ED;
INLINE constexpr VGenum VG_COMPRESSED_RGBA = 0x84EE;
INLINE constexpr VGenum VG_TEXTURE_COMPRESSION_HINT = 0x84EF;
INLINE constexpr VGenum VG_TEXTURE_COMPRESSED_IMAGE_SIZE = 0x86A0;
INLINE constexpr VGenum VG_TEXTURE_COMPRESSED = 0x86A1;
INLINE constexpr VGenum VG_NUM_COMPRESSED_TEXTURE_FORMATS = 0x86A2;
INLINE constexpr VGenum VG_COMPRESSED_TEXTURE_FORMATS = 0x86A3;
INLINE constexpr VGenum VG_CLAMP_TO_BORDER = 0x812D;

void vgActiveTexture(VGenum texture);
// void vgSampleCoverage(VGfloat value, VGboolean invert);
// void vgCompressedTexImage3D(VGenum target, VGint level, VGenum
// internalformat,
//                             VGsizei width, VGsizei height, VGsizei depth,
//                             VGint border, VGsizei imageSize, const void*
//                             data);
// void vgCompressedTexImage2D(VGenum target, VGint level, VGenum
// internalformat,
//                             VGsizei width, VGsizei height, VGint border,
//                             VGsizei imageSize, const void* data);
// void vgCompressedTexImage1D(VGenum target, VGint level, VGenum
// internalformat,
//                             VGsizei width, VGint border, VGsizei imageSize,
//                             const void* data);
// void vgCompressedTexSubImage3D(VGenum target, VGint level, VGint xoffset,
//                                VGint yoffset, VGint zoffset, VGsizei width,
//                                VGsizei height, VGsizei depth, VGenum format,
//                                VGsizei imageSize, const void* data);
// void vgCompressedTexSubImage2D(VGenum target, VGint level, VGint xoffset,
//                                VGint yoffset, VGsizei width, VGsizei height,
//                                VGenum format, VGsizei imageSize,
//                                const void* data);
// void vgCompressedTexSubImage1D(VGenum target, VGint level, VGint xoffset,
//                                VGsizei width, VGenum format, VGsizei
//                                imageSize, const void* data);
// void vgGetCompressedTexImage(VGenum target, VGint level, void* img);

//////////////////////////////////////////////////
// GL VERSION 1.4 API
//////////////////////////////////////////////////
INLINE constexpr VGenum VG_BLEND_DST_RGB = 0x80C8;
INLINE constexpr VGenum VG_BLEND_SRC_RGB = 0x80C9;
INLINE constexpr VGenum VG_BLEND_DST_ALPHA = 0x80CA;
INLINE constexpr VGenum VG_BLEND_SRC_ALPHA = 0x80CB;
INLINE constexpr VGenum VG_POINT_FADE_THRESHOLD_SIZE = 0x8128;
INLINE constexpr VGenum VG_DEPTH_COMPONENT16 = 0x81A5;
INLINE constexpr VGenum VG_DEPTH_COMPONENT24 = 0x81A6;
INLINE constexpr VGenum VG_DEPTH_COMPONENT32 = 0x81A7;
INLINE constexpr VGenum VG_MIRRORED_REPEAT = 0x8370;
INLINE constexpr VGenum VG_MAX_TEXTURE_LOD_BIAS = 0x84FD;
INLINE constexpr VGenum VG_TEXTURE_LOD_BIAS = 0x8501;
INLINE constexpr VGenum VG_INCR_WRAP = 0x8507;
INLINE constexpr VGenum VG_DECR_WRAP = 0x8508;
INLINE constexpr VGenum VG_TEXTURE_DEPTH_SIZE = 0x884A;
INLINE constexpr VGenum VG_TEXTURE_COMPARE_MODE = 0x884C;
INLINE constexpr VGenum VG_TEXTURE_COMPARE_FUNC = 0x884D;
INLINE constexpr VGenum VG_BLEND_COLOR = 0x8005;
INLINE constexpr VGenum VG_BLEND_EQUATION = 0x8009;
INLINE constexpr VGenum VG_CONSTANT_COLOR = 0x8001;
INLINE constexpr VGenum VG_ONE_MINUS_CONSTANT_COLOR = 0x8002;
INLINE constexpr VGenum VG_CONSTANT_ALPHA = 0x8003;
INLINE constexpr VGenum VG_ONE_MINUS_CONSTANT_ALPHA = 0x8004;
INLINE constexpr VGenum VG_FUNC_ADD = 0x8006;
INLINE constexpr VGenum VG_FUNC_REVERSE_SUBTRACT = 0x800B;
INLINE constexpr VGenum VG_FUNC_SUBTRACT = 0x800A;
INLINE constexpr VGenum VG_MIN = 0x8007;
INLINE constexpr VGenum VG_MAX = 0x8008;

void vgBlendFuncSeparate(VGenum sfactorRGB, VGenum dfactorRGB,
                         VGenum sfactorAlpha, VGenum dfactorAlpha);
void vgMultiDrawArrays(VGenum mode, const VGint* first, const VGsizei* count,
                       VGsizei drawcount);
void vgMultiDrawElements(VGenum mode, const VGsizei* count, VGenum type,
                         const void* const* indices, VGsizei drawcount);
// void vgPointParameterf(VGenum pname, VGfloat param);
// void vgPointParameterfv(VGenum pname, const VGfloat* params);
// void vgPointParameteri(VGenum pname, VGint param);
// void vgPointParameteriv(VGenum pname, const VGint* params);
void vgBlendColor(VGfloat red, VGfloat green, VGfloat blue, VGfloat alpha);
void vgBlendEquation(VGenum mode);

//////////////////////////////////////////////////
// GL_VERSION_1_5
//////////////////////////////////////////////////
using VGsizeiptr = size_t;
using VGintptr = intptr_t;

INLINE constexpr VGenum VG_BUFFER_SIZE = 0x8764;
INLINE constexpr VGenum VG_BUFFER_USAGE = 0x8765;
INLINE constexpr VGenum VG_QUERY_COUNTER_BITS = 0x8864;
INLINE constexpr VGenum VG_CURRENT_QUERY = 0x8865;
INLINE constexpr VGenum VG_QUERY_RESULT = 0x8866;
INLINE constexpr VGenum VG_QUERY_RESULT_AVAILABLE = 0x8867;
INLINE constexpr VGenum VG_ARRAY_BUFFER = 0x8892;
INLINE constexpr VGenum VG_ELEMENT_ARRAY_BUFFER = 0x8893;
INLINE constexpr VGenum VG_ARRAY_BUFFER_BINDING = 0x8894;
INLINE constexpr VGenum VG_ELEMENT_ARRAY_BUFFER_BINDING = 0x8895;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F;
INLINE constexpr VGenum VG_READ_ONLY = 0x88B8;
INLINE constexpr VGenum VG_WRITE_ONLY = 0x88B9;
INLINE constexpr VGenum VG_READ_WRITE = 0x88BA;
INLINE constexpr VGenum VG_BUFFER_ACCESS = 0x88BB;
INLINE constexpr VGenum VG_BUFFER_MAPPED = 0x88BC;
INLINE constexpr VGenum VG_BUFFER_MAP_POINTER = 0x88BD;
INLINE constexpr VGenum VG_STREAM_DRAW = 0x88E0;
INLINE constexpr VGenum VG_STREAM_READ = 0x88E1;
INLINE constexpr VGenum VG_STREAM_COPY = 0x88E2;
INLINE constexpr VGenum VG_STATIC_DRAW = 0x88E4;
INLINE constexpr VGenum VG_STATIC_READ = 0x88E5;
INLINE constexpr VGenum VG_STATIC_COPY = 0x88E6;
INLINE constexpr VGenum VG_DYNAMIC_DRAW = 0x88E8;
INLINE constexpr VGenum VG_DYNAMIC_READ = 0x88E9;
INLINE constexpr VGenum VG_DYNAMIC_COPY = 0x88EA;
INLINE constexpr VGenum VG_SAMPLES_PASSED = 0x8914;
INLINE constexpr VGenum VG_SRC1_ALPHA = 0x8589;

// void vgGenQueries(VGsizei n, VGuint* ids);
// void vgDeleteQueries(VGsizei n, const VGuint* ids);
// VGboolean vgIsQuery(VGuint id);
// void vgBeginQuery(VGenum target, VGuint id);
// void vgEndQuery(VGenum target);
// void vgGetQueryiv(VGenum target, VGenum pname, VGint* params);
// void vgGetQueryObjectiv(VGuint id, VGenum pname, VGint* params);
// void vgGetQueryObjectuiv(VGuint id, VGenum pname, VGuint* params);
void vgBindBuffer(VGenum target, VGuint buffer);
void vgDeleteBuffers(VGsizei n, const VGuint* buffers);
void vgGenBuffers(VGsizei n, VGuint* buffers);
VGboolean vgIsBuffer(VGuint buffer);
void vgBufferData(VGenum target, VGsizeiptr size, const void* data,
                  VGenum usage);
void vgBufferSubData(VGenum target, VGintptr offset, VGsizeiptr size,
                     const void* data);
// void vgGetBufferSubData(VGenum target, VGintptr offset, VGsizeiptr size,
//                         void* data);
// void* vgMapBuffer(VGenum target, VGenum access);
// VGboolean vgUnmapBuffer(VGenum target);
// void vgGetBufferParameteriv(VGenum target, VGenum pname, VGint* params);
// void vgGetBufferPointerv(VGenum target, VGenum pname, void** params);

//////////////////////////////////////////////////
// GL VERSION 2.0 API
//////////////////////////////////////////////////
using VGchar = char;
using VGshort = int16_t;
using VGbyte = int8_t;
using VGushort = uint16_t;

INLINE constexpr VGenum VG_BLEND_EQUATION_RGB = 0x8009;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_ENABLED = 0x8622;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_SIZE = 0x8623;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_STRIDE = 0x8624;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_TYPE = 0x8625;
INLINE constexpr VGenum VG_CURRENT_VERTEX_ATTRIB = 0x8626;
INLINE constexpr VGenum VG_VERTEX_PROGRAM_POINT_SIZE = 0x8642;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_POINTER = 0x8645;
INLINE constexpr VGenum VG_STENCIL_BACK_FUNC = 0x8800;
INLINE constexpr VGenum VG_STENCIL_BACK_FAIL = 0x8801;
INLINE constexpr VGenum VG_STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802;
INLINE constexpr VGenum VG_STENCIL_BACK_PASS_DEPTH_PASS = 0x8803;
INLINE constexpr VGenum VG_MAX_DRAW_BUFFERS = 0x8824;
INLINE constexpr VGenum VG_DRAW_BUFFER0 = 0x8825;
INLINE constexpr VGenum VG_DRAW_BUFFER1 = 0x8826;
INLINE constexpr VGenum VG_DRAW_BUFFER2 = 0x8827;
INLINE constexpr VGenum VG_DRAW_BUFFER3 = 0x8828;
INLINE constexpr VGenum VG_DRAW_BUFFER4 = 0x8829;
INLINE constexpr VGenum VG_DRAW_BUFFER5 = 0x882A;
INLINE constexpr VGenum VG_DRAW_BUFFER6 = 0x882B;
INLINE constexpr VGenum VG_DRAW_BUFFER7 = 0x882C;
INLINE constexpr VGenum VG_DRAW_BUFFER8 = 0x882D;
INLINE constexpr VGenum VG_DRAW_BUFFER9 = 0x882E;
INLINE constexpr VGenum VG_DRAW_BUFFER10 = 0x882F;
INLINE constexpr VGenum VG_DRAW_BUFFER11 = 0x8830;
INLINE constexpr VGenum VG_DRAW_BUFFER12 = 0x8831;
INLINE constexpr VGenum VG_DRAW_BUFFER13 = 0x8832;
INLINE constexpr VGenum VG_DRAW_BUFFER14 = 0x8833;
INLINE constexpr VGenum VG_DRAW_BUFFER15 = 0x8834;
INLINE constexpr VGenum VG_BLEND_EQUATION_ALPHA = 0x883D;
INLINE constexpr VGenum VG_MAX_VERTEX_ATTRIBS = 0x8869;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_NORMALIZED = 0x886A;
INLINE constexpr VGenum VG_MAX_TEXTURE_IMAGE_UNITS = 0x8872;
INLINE constexpr VGenum VG_FRAGMENT_SHADER = 0x8B30;
INLINE constexpr VGenum VG_VERTEX_SHADER = 0x8B31;
INLINE constexpr VGenum VG_MAX_FRAGMENT_UNIFORM_COMPONENTS = 0x8B49;
INLINE constexpr VGenum VG_MAX_VERTEX_UNIFORM_COMPONENTS = 0x8B4A;
INLINE constexpr VGenum VG_MAX_VARYING_FLOATS = 0x8B4B;
INLINE constexpr VGenum VG_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 0x8B4C;
INLINE constexpr VGenum VG_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D;
INLINE constexpr VGenum VG_SHADER_TYPE = 0x8B4F;
INLINE constexpr VGenum VG_FLOAT_VEC2 = 0x8B50;
INLINE constexpr VGenum VG_FLOAT_VEC3 = 0x8B51;
INLINE constexpr VGenum VG_FLOAT_VEC4 = 0x8B52;
INLINE constexpr VGenum VG_INT_VEC2 = 0x8B53;
INLINE constexpr VGenum VG_INT_VEC3 = 0x8B54;
INLINE constexpr VGenum VG_INT_VEC4 = 0x8B55;
INLINE constexpr VGenum VG_BOOL = 0x8B56;
INLINE constexpr VGenum VG_BOOL_VEC2 = 0x8B57;
INLINE constexpr VGenum VG_BOOL_VEC3 = 0x8B58;
INLINE constexpr VGenum VG_BOOL_VEC4 = 0x8B59;
INLINE constexpr VGenum VG_FLOAT_MAT2 = 0x8B5A;
INLINE constexpr VGenum VG_FLOAT_MAT3 = 0x8B5B;
INLINE constexpr VGenum VG_FLOAT_MAT4 = 0x8B5C;
INLINE constexpr VGenum VG_SAMPLER_1D = 0x8B5D;
INLINE constexpr VGenum VG_SAMPLER_2D = 0x8B5E;
INLINE constexpr VGenum VG_SAMPLER_3D = 0x8B5F;
INLINE constexpr VGenum VG_SAMPLER_CUBE = 0x8B60;
INLINE constexpr VGenum VG_SAMPLER_1D_SHADOW = 0x8B61;
INLINE constexpr VGenum VG_SAMPLER_2D_SHADOW = 0x8B62;
INLINE constexpr VGenum VG_DELETE_STATUS = 0x8B80;
INLINE constexpr VGenum VG_COMPILE_STATUS = 0x8B81;
INLINE constexpr VGenum VG_LINK_STATUS = 0x8B82;
INLINE constexpr VGenum VG_VALIDATE_STATUS = 0x8B83;
INLINE constexpr VGenum VG_INFO_LOG_LENGTH = 0x8B84;
INLINE constexpr VGenum VG_ATTACHED_SHADERS = 0x8B85;
INLINE constexpr VGenum VG_ACTIVE_UNIFORMS = 0x8B86;
INLINE constexpr VGenum VG_ACTIVE_UNIFORM_MAX_LENGTH = 0x8B87;
INLINE constexpr VGenum VG_SHADER_SOURCE_LENGTH = 0x8B88;
INLINE constexpr VGenum VG_ACTIVE_ATTRIBUTES = 0x8B89;
INLINE constexpr VGenum VG_ACTIVE_ATTRIBUTE_MAX_LENGTH = 0x8B8A;
INLINE constexpr VGenum VG_FRAGMENT_SHADER_DERIVATIVE_HINT = 0x8B8B;
INLINE constexpr VGenum VG_SHADING_LANGUAGE_VERSION = 0x8B8C;
INLINE constexpr VGenum VG_CURRENT_PROGRAM = 0x8B8D;
INLINE constexpr VGenum VG_POINT_SPRITE_COORD_ORIGIN = 0x8CA0;
INLINE constexpr VGenum VG_LOWER_LEFT = 0x8CA1;
INLINE constexpr VGenum VG_UPPER_LEFT = 0x8CA2;
INLINE constexpr VGenum VG_STENCIL_BACK_REF = 0x8CA3;
INLINE constexpr VGenum VG_STENCIL_BACK_VALUE_MASK = 0x8CA4;
INLINE constexpr VGenum VG_STENCIL_BACK_WRITEMASK = 0x8CA5;

void vgBlendEquationSeparate(VGenum modeRGB, VGenum modeAlpha);
void vgDrawBuffers(VGsizei n, const VGenum* bufs);
void vgStencilOpSeparate(VGenum face, VGenum sfail, VGenum dpfail,
                         VGenum dppass);
void vgStencilFuncSeparate(VGenum face, VGenum func, VGint ref, VGuint mask);
void vgStencilMaskSeparate(VGenum face, VGuint mask);
void vgAttachShader(VGuint program, VGuint shader);
void vgBindAttribLocation(VGuint program, VGuint index, const VGchar* name);
void vgCompileShader(VGuint shader);
VGuint vgCreateProgram(void);
VGuint vgCreateShader(VGenum type);
void vgDeleteProgram(VGuint program);
void vgDeleteShader(VGuint shader);
void vgDetachShader(VGuint program, VGuint shader);
void vgDisableVertexAttribArray(VGuint index);
void vgEnableVertexAttribArray(VGuint index);
// void vgGetActiveAttrib(VGuint program, VGuint index, VGsizei bufSize,
//                        VGsizei* length, VGint* size, VGenum* type,
//                        VGchar* name);
// void vgGetActiveUniform(VGuint program, VGuint index, VGsizei bufSize,
//                         VGsizei* length, VGint* size, VGenum* type,
//                         VGchar* name);
// void vgGetAttachedShaders(VGuint program, VGsizei maxCount, VGsizei* count,
//                           VGuint* shaders);
// VGint vgGetAttribLocation(VGuint program, VGint name_hash);
// VGint vgGetAttribLocation(VGuint program, const VGchar* name);
// void vgGetProgramiv(VGuint program, VGenum pname, VGint* params);
// void vgGetProgramInfoLog(VGuint program, VGsizei bufSize, VGsizei* length,
//                          VGchar* infoLog);
// void vgGetShaderiv(VGuint shader, VGenum pname, VGint* params);
// void vgGetShaderInfoLog(VGuint shader, VGsizei bufSize, VGsizei* length,
//                         VGchar* infoLog);
// void vgGetShaderSource(VGuint shader, VGsizei bufSize, VGsizei* length,
//                        VGchar* source);
VGint vgGetUniformLocation(VGuint program, VGint name_hash);
VGint vgGetUniformLocation(VGuint program, const VGchar* name);
// void vgGetUniformfv(VGuint program, VGint location, VGfloat* params);
// void vgGetUniformiv(VGuint program, VGint location, VGint* params);
// void vgGetVertexAttribdv(VGuint index, VGenum pname, VGdouble* params);
// void vgGetVertexAttribfv(VGuint index, VGenum pname, VGfloat* params);
// void vgGetVertexAttribiv(VGuint index, VGenum pname, VGint* params);
// void vgGetVertexAttribPointerv(VGuint index, VGenum pname, void** pointer);
VGboolean vgIsProgram(VGuint program);
VGboolean vgIsShader(VGuint shader);
void vgLinkProgram(VGuint program);
void vgShaderSource(VGuint shader, void* source);
void vgUseProgram(VGuint program);
void vgUniform1f(VGint location, VGfloat v0);
void vgUniform2f(VGint location, VGfloat v0, VGfloat v1);
void vgUniform3f(VGint location, VGfloat v0, VGfloat v1, VGfloat v2);
void vgUniform4f(VGint location, VGfloat v0, VGfloat v1, VGfloat v2,
                 VGfloat v3);
void vgUniform1i(VGint location, VGint v0);
void vgUniform2i(VGint location, VGint v0, VGint v1);
void vgUniform3i(VGint location, VGint v0, VGint v1, VGint v2);
void vgUniform4i(VGint location, VGint v0, VGint v1, VGint v2, VGint v3);
void vgUniform1fv(VGint location, VGsizei count, const VGfloat* value);
void vgUniform2fv(VGint location, VGsizei count, const VGfloat* value);
void vgUniform3fv(VGint location, VGsizei count, const VGfloat* value);
void vgUniform4fv(VGint location, VGsizei count, const VGfloat* value);
void vgUniform1iv(VGint location, VGsizei count, const VGint* value);
void vgUniform2iv(VGint location, VGsizei count, const VGint* value);
void vgUniform3iv(VGint location, VGsizei count, const VGint* value);
void vgUniform4iv(VGint location, VGsizei count, const VGint* value);
void vgUniformMatrix2fv(VGint location, VGsizei count, VGboolean transpose,
                        const VGfloat* value);
void vgUniformMatrix3fv(VGint location, VGsizei count, VGboolean transpose,
                        const VGfloat* value);
void vgUniformMatrix4fv(VGint location, VGsizei count, VGboolean transpose,
                        const VGfloat* value);
// void vgValidateProgram(VGuint program);
void vgVertexAttrib1d(VGuint index, VGdouble x);
void vgVertexAttrib1dv(VGuint index, const VGdouble* v);
void vgVertexAttrib1f(VGuint index, VGfloat x);
void vgVertexAttrib1fv(VGuint index, const VGfloat* v);
void vgVertexAttrib1s(VGuint index, VGshort x);
void vgVertexAttrib1sv(VGuint index, const VGshort* v);
void vgVertexAttrib2d(VGuint index, VGdouble x, VGdouble y);
void vgVertexAttrib2dv(VGuint index, const VGdouble* v);
void vgVertexAttrib2f(VGuint index, VGfloat x, VGfloat y);
void vgVertexAttrib2fv(VGuint index, const VGfloat* v);
void vgVertexAttrib2s(VGuint index, VGshort x, VGshort y);
void vgVertexAttrib2sv(VGuint index, const VGshort* v);
void vgVertexAttrib3d(VGuint index, VGdouble x, VGdouble y, VGdouble z);
void vgVertexAttrib3dv(VGuint index, const VGdouble* v);
void vgVertexAttrib3f(VGuint index, VGfloat x, VGfloat y, VGfloat z);
void vgVertexAttrib3fv(VGuint index, const VGfloat* v);
void vgVertexAttrib3s(VGuint index, VGshort x, VGshort y, VGshort z);
void vgVertexAttrib3sv(VGuint index, const VGshort* v);
void vgVertexAttrib4Nbv(VGuint index, const VGbyte* v);
void vgVertexAttrib4Niv(VGuint index, const VGint* v);
void vgVertexAttrib4Nsv(VGuint index, const VGshort* v);
void vgVertexAttrib4Nub(VGuint index, VGubyte x, VGubyte y, VGubyte z,
                        VGubyte w);
void vgVertexAttrib4Nubv(VGuint index, const VGubyte* v);
void vgVertexAttrib4Nuiv(VGuint index, const VGuint* v);
void vgVertexAttrib4Nusv(VGuint index, const VGushort* v);
void vgVertexAttrib4bv(VGuint index, const VGbyte* v);
void vgVertexAttrib4d(VGuint index, VGdouble x, VGdouble y, VGdouble z,
                      VGdouble w);
void vgVertexAttrib4dv(VGuint index, const VGdouble* v);
void vgVertexAttrib4f(VGuint index, VGfloat x, VGfloat y, VGfloat z, VGfloat w);
void vgVertexAttrib4fv(VGuint index, const VGfloat* v);
void vgVertexAttrib4iv(VGuint index, const VGint* v);
void vgVertexAttrib4s(VGuint index, VGshort x, VGshort y, VGshort z, VGshort w);
void vgVertexAttrib4sv(VGuint index, const VGshort* v);
void vgVertexAttrib4ubv(VGuint index, const VGubyte* v);
void vgVertexAttrib4uiv(VGuint index, const VGuint* v);
void vgVertexAttrib4usv(VGuint index, const VGushort* v);
void vgVertexAttribPointer(VGuint index, VGint size, VGenum type,
                           VGboolean normalized, VGsizei stride,
                           const void* pointer);

//////////////////////////////////////////////////
// GL VERSION 2.1 API
//////////////////////////////////////////////////
INLINE constexpr VGenum VG_PIXEL_PACK_BUFFER = 0x88EB;
INLINE constexpr VGenum VG_PIXEL_UNPACK_BUFFER = 0x88EC;
INLINE constexpr VGenum VG_PIXEL_PACK_BUFFER_BINDING = 0x88ED;
INLINE constexpr VGenum VG_PIXEL_UNPACK_BUFFER_BINDING = 0x88EF;
INLINE constexpr VGenum VG_FLOAT_MAT2x3 = 0x8B65;
INLINE constexpr VGenum VG_FLOAT_MAT2x4 = 0x8B66;
INLINE constexpr VGenum VG_FLOAT_MAT3x2 = 0x8B67;
INLINE constexpr VGenum VG_FLOAT_MAT3x4 = 0x8B68;
INLINE constexpr VGenum VG_FLOAT_MAT4x2 = 0x8B69;
INLINE constexpr VGenum VG_FLOAT_MAT4x3 = 0x8B6A;
INLINE constexpr VGenum VG_SRGB = 0x8C40;
INLINE constexpr VGenum VG_SRGB8 = 0x8C41;
INLINE constexpr VGenum VG_SRGB_ALPHA = 0x8C42;
INLINE constexpr VGenum VG_SRGB8_ALPHA8 = 0x8C43;
INLINE constexpr VGenum VG_COMPRESSED_SRGB = 0x8C48;
INLINE constexpr VGenum VG_COMPRESSED_SRGB_ALPHA = 0x8C49;

// void vgUniformMatrix2x3fv(VGint location, VGsizei count, VGboolean transpose,
//                           const VGfloat* value);
// void vgUniformMatrix3x2fv(VGint location, VGsizei count, VGboolean transpose,
//                           const VGfloat* value);
// void vgUniformMatrix2x4fv(VGint location, VGsizei count, VGboolean transpose,
//                           const VGfloat* value);
// void vgUniformMatrix4x2fv(VGint location, VGsizei count, VGboolean transpose,
//                           const VGfloat* value);
// void vgUniformMatrix3x4fv(VGint location, VGsizei count, VGboolean transpose,
//                           const VGfloat* value);
// void vgUniformMatrix4x3fv(VGint location, VGsizei count, VGboolean transpose,
//                           const VGfloat* value);

//////////////////////////////////////////////////
// GL VERSION 3.0 API
//////////////////////////////////////////////////
using VGhalf = uint16_t;

INLINE constexpr VGenum VG_COMPARE_REF_TO_TEXTURE = 0x884E;
INLINE constexpr VGenum VG_CLIP_DISTANCE0 = 0x3000;
INLINE constexpr VGenum VG_CLIP_DISTANCE1 = 0x3001;
INLINE constexpr VGenum VG_CLIP_DISTANCE2 = 0x3002;
INLINE constexpr VGenum VG_CLIP_DISTANCE3 = 0x3003;
INLINE constexpr VGenum VG_CLIP_DISTANCE4 = 0x3004;
INLINE constexpr VGenum VG_CLIP_DISTANCE5 = 0x3005;
INLINE constexpr VGenum VG_CLIP_DISTANCE6 = 0x3006;
INLINE constexpr VGenum VG_CLIP_DISTANCE7 = 0x3007;
INLINE constexpr VGenum VG_MAX_CLIP_DISTANCES = 0x0D32;
INLINE constexpr VGenum VG_MAJOR_VERSION = 0x821B;
INLINE constexpr VGenum VG_MINOR_VERSION = 0x821C;
INLINE constexpr VGenum VG_NUM_EXTENSIONS = 0x821D;
INLINE constexpr VGenum VG_CONTEXT_FLAGS = 0x821E;
INLINE constexpr VGenum VG_COMPRESSED_RED = 0x8225;
INLINE constexpr VGenum VG_COMPRESSED_RG = 0x8226;
INLINE constexpr VGenum VG_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x00000001;
INLINE constexpr VGenum VG_RGBA32F = 0x8814;
INLINE constexpr VGenum VG_RGB32F = 0x8815;
INLINE constexpr VGenum VG_RGBA16F = 0x881A;
INLINE constexpr VGenum VG_RGB16F = 0x881B;
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_INTEGER = 0x88FD;
INLINE constexpr VGenum VG_MAX_ARRAY_TEXTURE_LAYERS = 0x88FF;
INLINE constexpr VGenum VG_MIN_PROGRAM_TEXEL_OFFSET = 0x8904;
INLINE constexpr VGenum VG_MAX_PROGRAM_TEXEL_OFFSET = 0x8905;
INLINE constexpr VGenum VG_CLAMP_READ_COLOR = 0x891C;
INLINE constexpr VGenum VG_FIXED_ONLY = 0x891D;
INLINE constexpr VGenum VG_MAX_VARYING_COMPONENTS = 0x8B4B;
INLINE constexpr VGenum VG_TEXTURE_1D_ARRAY = 0x8C18;
INLINE constexpr VGenum VG_PROXY_TEXTURE_1D_ARRAY = 0x8C19;
INLINE constexpr VGenum VG_TEXTURE_2D_ARRAY = 0x8C1A;
INLINE constexpr VGenum VG_PROXY_TEXTURE_2D_ARRAY = 0x8C1B;
INLINE constexpr VGenum VG_TEXTURE_BINDING_1D_ARRAY = 0x8C1C;
INLINE constexpr VGenum VG_TEXTURE_BINDING_2D_ARRAY = 0x8C1D;
INLINE constexpr VGenum VG_R11F_G11F_B10F = 0x8C3A;
INLINE constexpr VGenum VG_UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B;
INLINE constexpr VGenum VG_RGB9_E5 = 0x8C3D;
INLINE constexpr VGenum VG_UNSIGNED_INT_5_9_9_9_REV = 0x8C3E;
INLINE constexpr VGenum VG_TEXTURE_SHARED_SIZE = 0x8C3F;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH = 0x8C76;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_BUFFER_MODE = 0x8C7F;
INLINE constexpr VGenum VG_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS = 0x8C80;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_VARYINGS = 0x8C83;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_BUFFER_START = 0x8C84;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_BUFFER_SIZE = 0x8C85;
INLINE constexpr VGenum VG_PRIMITIVES_GENERATED = 0x8C87;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88;
INLINE constexpr VGenum VG_RASTERIZER_DISCARD = 0x8C89;
INLINE constexpr VGenum VG_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS =
    0x8C8A;
INLINE constexpr VGenum VG_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 0x8C8B;
INLINE constexpr VGenum VG_INTERLEAVED_ATTRIBS = 0x8C8C;
INLINE constexpr VGenum VG_SEPARATE_ATTRIBS = 0x8C8D;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_BUFFER = 0x8C8E;
INLINE constexpr VGenum VG_TRANSFORM_FEEDBACK_BUFFER_BINDING = 0x8C8F;
INLINE constexpr VGenum VG_RGBA32UI = 0x8D70;
INLINE constexpr VGenum VG_RGB32UI = 0x8D71;
INLINE constexpr VGenum VG_RGBA16UI = 0x8D76;
INLINE constexpr VGenum VG_RGB16UI = 0x8D77;
INLINE constexpr VGenum VG_RGBA8UI = 0x8D7C;
INLINE constexpr VGenum VG_RGB8UI = 0x8D7D;
INLINE constexpr VGenum VG_RGBA32I = 0x8D82;
INLINE constexpr VGenum VG_RGB32I = 0x8D83;
INLINE constexpr VGenum VG_RGBA16I = 0x8D88;
INLINE constexpr VGenum VG_RGB16I = 0x8D89;
INLINE constexpr VGenum VG_RGBA8I = 0x8D8E;
INLINE constexpr VGenum VG_RGB8I = 0x8D8F;
INLINE constexpr VGenum VG_RED_INTEGER = 0x8D94;
INLINE constexpr VGenum VG_GREEN_INTEGER = 0x8D95;
INLINE constexpr VGenum VG_BLUE_INTEGER = 0x8D96;
INLINE constexpr VGenum VG_RGB_INTEGER = 0x8D98;
INLINE constexpr VGenum VG_RGBA_INTEGER = 0x8D99;
INLINE constexpr VGenum VG_BGR_INTEGER = 0x8D9A;
INLINE constexpr VGenum VG_BGRA_INTEGER = 0x8D9B;
INLINE constexpr VGenum VG_SAMPLER_1D_ARRAY = 0x8DC0;
INLINE constexpr VGenum VG_SAMPLER_2D_ARRAY = 0x8DC1;
INLINE constexpr VGenum VG_SAMPLER_1D_ARRAY_SHADOW = 0x8DC3;
INLINE constexpr VGenum VG_SAMPLER_2D_ARRAY_SHADOW = 0x8DC4;
INLINE constexpr VGenum VG_SAMPLER_CUBE_SHADOW = 0x8DC5;
INLINE constexpr VGenum VG_UNSIGNED_INT_VEC2 = 0x8DC6;
INLINE constexpr VGenum VG_UNSIGNED_INT_VEC3 = 0x8DC7;
INLINE constexpr VGenum VG_UNSIGNED_INT_VEC4 = 0x8DC8;
INLINE constexpr VGenum VG_INT_SAMPLER_1D = 0x8DC9;
INLINE constexpr VGenum VG_INT_SAMPLER_2D = 0x8DCA;
INLINE constexpr VGenum VG_INT_SAMPLER_3D = 0x8DCB;
INLINE constexpr VGenum VG_INT_SAMPLER_CUBE = 0x8DCC;
INLINE constexpr VGenum VG_INT_SAMPLER_1D_ARRAY = 0x8DCE;
INLINE constexpr VGenum VG_INT_SAMPLER_2D_ARRAY = 0x8DCF;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_1D = 0x8DD1;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_2D = 0x8DD2;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_3D = 0x8DD3;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_CUBE = 0x8DD4;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_1D_ARRAY = 0x8DD6;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_2D_ARRAY = 0x8DD7;
INLINE constexpr VGenum VG_QUERY_WAIT = 0x8E13;
INLINE constexpr VGenum VG_QUERY_NO_WAIT = 0x8E14;
INLINE constexpr VGenum VG_QUERY_BY_REGION_WAIT = 0x8E15;
INLINE constexpr VGenum VG_QUERY_BY_REGION_NO_WAIT = 0x8E16;
INLINE constexpr VGenum VG_BUFFER_ACCESS_FLAGS = 0x911F;
INLINE constexpr VGenum VG_BUFFER_MAP_LENGTH = 0x9120;
INLINE constexpr VGenum VG_BUFFER_MAP_OFFSET = 0x9121;
INLINE constexpr VGenum VG_DEPTH_COMPONENT32F = 0x8CAC;
INLINE constexpr VGenum VG_DEPTH32F_STENCIL8 = 0x8CAD;
INLINE constexpr VGenum VG_FLOAT_32_UNSIGNED_INT_24_8_REV = 0x8DAD;
INLINE constexpr VGenum VG_INVALID_FRAMEBUFFER_OPERATION = 0x0506;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING = 0x8210;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE = 0x8211;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_RED_SIZE = 0x8212;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE = 0x8213;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE = 0x8214;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE = 0x8215;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE = 0x8216;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE = 0x8217;
INLINE constexpr VGenum VG_FRAMEBUFFER_DEFAULT = 0x8218;
INLINE constexpr VGenum VG_FRAMEBUFFER_UNDEFINED = 0x8219;
INLINE constexpr VGenum VG_DEPTH_STENCIL_ATTACHMENT = 0x821A;
INLINE constexpr VGenum VG_MAX_RENDERBUFFER_SIZE = 0x84E8;
INLINE constexpr VGenum VG_DEPTH_STENCIL = 0x84F9;
INLINE constexpr VGenum VG_UNSIGNED_INT_24_8 = 0x84FA;
INLINE constexpr VGenum VG_DEPTH24_STENCIL8 = 0x88F0;
INLINE constexpr VGenum VG_TEXTURE_STENCIL_SIZE = 0x88F1;
INLINE constexpr VGenum VG_TEXTURE_RED_TYPE = 0x8C10;
INLINE constexpr VGenum VG_TEXTURE_GREEN_TYPE = 0x8C11;
INLINE constexpr VGenum VG_TEXTURE_BLUE_TYPE = 0x8C12;
INLINE constexpr VGenum VG_TEXTURE_ALPHA_TYPE = 0x8C13;
INLINE constexpr VGenum VG_TEXTURE_DEPTH_TYPE = 0x8C16;
INLINE constexpr VGenum VG_UNSIGNED_NORMALIZED = 0x8C17;
INLINE constexpr VGenum VG_FRAMEBUFFER_BINDING = 0x8CA6;
INLINE constexpr VGenum VG_DRAW_FRAMEBUFFER_BINDING = 0x8CA6;
INLINE constexpr VGenum VG_RENDERBUFFER_BINDING = 0x8CA7;
INLINE constexpr VGenum VG_READ_FRAMEBUFFER = 0x8CA8;
INLINE constexpr VGenum VG_DRAW_FRAMEBUFFER = 0x8CA9;
INLINE constexpr VGenum VG_READ_FRAMEBUFFER_BINDING = 0x8CAA;
INLINE constexpr VGenum VG_RENDERBUFFER_SAMPLES = 0x8CAB;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = 0x8CD0;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME = 0x8CD1;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL = 0x8CD2;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE =
    0x8CD3;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER = 0x8CD4;
INLINE constexpr VGenum VG_FRAMEBUFFER_COMPLETE = 0x8CD5;
INLINE constexpr VGenum VG_FRAMEBUFFER_INCOMPLETE_ATTACHMENT = 0x8CD6;
INLINE constexpr VGenum VG_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7;
INLINE constexpr VGenum VG_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER = 0x8CDB;
INLINE constexpr VGenum VG_FRAMEBUFFER_INCOMPLETE_READ_BUFFER = 0x8CDC;
INLINE constexpr VGenum VG_FRAMEBUFFER_UNSUPPORTED = 0x8CDD;
INLINE constexpr VGenum VG_MAX_COLOR_ATTACHMENTS = 0x8CDF;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT0 = 0x8CE0;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT1 = 0x8CE1;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT2 = 0x8CE2;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT3 = 0x8CE3;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT4 = 0x8CE4;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT5 = 0x8CE5;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT6 = 0x8CE6;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT7 = 0x8CE7;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT8 = 0x8CE8;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT9 = 0x8CE9;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT10 = 0x8CEA;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT11 = 0x8CEB;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT12 = 0x8CEC;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT13 = 0x8CED;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT14 = 0x8CEE;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT15 = 0x8CEF;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT16 = 0x8CF0;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT17 = 0x8CF1;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT18 = 0x8CF2;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT19 = 0x8CF3;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT20 = 0x8CF4;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT21 = 0x8CF5;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT22 = 0x8CF6;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT23 = 0x8CF7;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT24 = 0x8CF8;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT25 = 0x8CF9;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT26 = 0x8CFA;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT27 = 0x8CFB;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT28 = 0x8CFC;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT29 = 0x8CFD;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT30 = 0x8CFE;
INLINE constexpr VGenum VG_COLOR_ATTACHMENT31 = 0x8CFF;
INLINE constexpr VGenum VG_DEPTH_ATTACHMENT = 0x8D00;
INLINE constexpr VGenum VG_STENCIL_ATTACHMENT = 0x8D20;
INLINE constexpr VGenum VG_FRAMEBUFFER = 0x8D40;
INLINE constexpr VGenum VG_RENDERBUFFER = 0x8D41;
INLINE constexpr VGenum VG_RENDERBUFFER_WIDTH = 0x8D42;
INLINE constexpr VGenum VG_RENDERBUFFER_HEIGHT = 0x8D43;
INLINE constexpr VGenum VG_RENDERBUFFER_INTERNAL_FORMAT = 0x8D44;
INLINE constexpr VGenum VG_STENCIL_INDEX1 = 0x8D46;
INLINE constexpr VGenum VG_STENCIL_INDEX4 = 0x8D47;
INLINE constexpr VGenum VG_STENCIL_INDEX8 = 0x8D48;
INLINE constexpr VGenum VG_STENCIL_INDEX16 = 0x8D49;
INLINE constexpr VGenum VG_RENDERBUFFER_RED_SIZE = 0x8D50;
INLINE constexpr VGenum VG_RENDERBUFFER_GREEN_SIZE = 0x8D51;
INLINE constexpr VGenum VG_RENDERBUFFER_BLUE_SIZE = 0x8D52;
INLINE constexpr VGenum VG_RENDERBUFFER_ALPHA_SIZE = 0x8D53;
INLINE constexpr VGenum VG_RENDERBUFFER_DEPTH_SIZE = 0x8D54;
INLINE constexpr VGenum VG_RENDERBUFFER_STENCIL_SIZE = 0x8D55;
INLINE constexpr VGenum VG_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE = 0x8D56;
INLINE constexpr VGenum VG_MAX_SAMPLES = 0x8D57;
INLINE constexpr VGenum VG_FRAMEBUFFER_SRGB = 0x8DB9;
INLINE constexpr VGenum VG_HALF_FLOAT = 0x140B;
INLINE constexpr VGenum VG_MAP_READ_BIT = 0x0001;
INLINE constexpr VGenum VG_MAP_WRITE_BIT = 0x0002;
INLINE constexpr VGenum VG_MAP_INVALIDATE_RANGE_BIT = 0x0004;
INLINE constexpr VGenum VG_MAP_INVALIDATE_BUFFER_BIT = 0x0008;
INLINE constexpr VGenum VG_MAP_FLUSH_EXPLICIT_BIT = 0x0010;
INLINE constexpr VGenum VG_MAP_UNSYNCHRONIZED_BIT = 0x0020;
INLINE constexpr VGenum VG_COMPRESSED_RED_RGTC1 = 0x8DBB;
INLINE constexpr VGenum VG_COMPRESSED_SIGNED_RED_RGTC1 = 0x8DBC;
INLINE constexpr VGenum VG_COMPRESSED_RG_RGTC2 = 0x8DBD;
INLINE constexpr VGenum VG_COMPRESSED_SIGNED_RG_RGTC2 = 0x8DBE;
INLINE constexpr VGenum VG_RG = 0x8227;
INLINE constexpr VGenum VG_RG_INTEGER = 0x8228;
INLINE constexpr VGenum VG_R8 = 0x8229;
INLINE constexpr VGenum VG_R16 = 0x822A;
INLINE constexpr VGenum VG_RG8 = 0x822B;
INLINE constexpr VGenum VG_RG16 = 0x822C;
INLINE constexpr VGenum VG_R16F = 0x822D;
INLINE constexpr VGenum VG_R32F = 0x822E;
INLINE constexpr VGenum VG_RG16F = 0x822F;
INLINE constexpr VGenum VG_RG32F = 0x8230;
INLINE constexpr VGenum VG_R8I = 0x8231;
INLINE constexpr VGenum VG_R8UI = 0x8232;
INLINE constexpr VGenum VG_R16I = 0x8233;
INLINE constexpr VGenum VG_R16UI = 0x8234;
INLINE constexpr VGenum VG_R32I = 0x8235;
INLINE constexpr VGenum VG_R32UI = 0x8236;
INLINE constexpr VGenum VG_RG8I = 0x8237;
INLINE constexpr VGenum VG_RG8UI = 0x8238;
INLINE constexpr VGenum VG_RG16I = 0x8239;
INLINE constexpr VGenum VG_RG16UI = 0x823A;
INLINE constexpr VGenum VG_RG32I = 0x823B;
INLINE constexpr VGenum VG_RG32UI = 0x823C;
INLINE constexpr VGenum VG_VERTEX_ARRAY_BINDING = 0x85B5;

void vgColorMaski(VGuint index, VGboolean r, VGboolean g, VGboolean b,
                  VGboolean a);
// void vgGetBooleani_v(VGenum target, VGuint index, VGboolean* data);
// void vgGetIntegeri_v(VGenum target, VGuint index, VGint* data);
void vgEnablei(VGenum target, VGuint index);
void vgDisablei(VGenum target, VGuint index);
VGboolean vgIsEnabledi(VGenum target, VGuint index);
// void vgBeginTransformFeedback(VGenum primitiveMode);
// void vgEndTransformFeedback(void);
void vgBindBufferRange(VGenum target, VGuint index, VGuint buffer,
                       VGintptr offset, VGsizeiptr size);
void vgBindBufferBase(VGenum target, VGuint index, VGuint buffer);
// void vgTransformFeedbackVaryings(VGuint program, VGsizei count,
//                                  const VGchar* const* varyings,
//                                  VGenum bufferMode);
// void vgGetTransformFeedbackVarying(VGuint program, VGuint index,
//                                    VGsizei bufSize, VGsizei* length,
//                                    VGsizei* size, VGenum* type, VGchar*
//                                    name);
// void vgClampColor(VGenum target, VGenum clamp);
// void vgBeginConditionalRender(VGuint id, VGenum mode);
// void vgEndConditionalRender(void);
void vgVertexAttribIPointer(VGuint index, VGint size, VGenum type,
                            VGsizei stride, const void* pointer);
// void vgGetVertexAttribIiv(VGuint index, VGenum pname, VGint* params);
// void vgGetVertexAttribIuiv(VGuint index, VGenum pname, VGuint* params);
void vgVertexAttribI1i(VGuint index, VGint x);
void vgVertexAttribI2i(VGuint index, VGint x, VGint y);
void vgVertexAttribI3i(VGuint index, VGint x, VGint y, VGint z);
void vgVertexAttribI4i(VGuint index, VGint x, VGint y, VGint z, VGint w);
void vgVertexAttribI1ui(VGuint index, VGuint x);
void vgVertexAttribI2ui(VGuint index, VGuint x, VGuint y);
void vgVertexAttribI3ui(VGuint index, VGuint x, VGuint y, VGuint z);
void vgVertexAttribI4ui(VGuint index, VGuint x, VGuint y, VGuint z, VGuint w);
void vgVertexAttribI1iv(VGuint index, const VGint* v);
void vgVertexAttribI2iv(VGuint index, const VGint* v);
void vgVertexAttribI3iv(VGuint index, const VGint* v);
void vgVertexAttribI4iv(VGuint index, const VGint* v);
void vgVertexAttribI1uiv(VGuint index, const VGuint* v);
void vgVertexAttribI2uiv(VGuint index, const VGuint* v);
void vgVertexAttribI3uiv(VGuint index, const VGuint* v);
void vgVertexAttribI4uiv(VGuint index, const VGuint* v);
void vgVertexAttribI4bv(VGuint index, const VGbyte* v);
void vgVertexAttribI4sv(VGuint index, const VGshort* v);
void vgVertexAttribI4ubv(VGuint index, const VGubyte* v);
void vgVertexAttribI4usv(VGuint index, const VGushort* v);
// void vgGetUniformuiv(VGuint program, VGint location, VGuint* params);
void vgBindFragDataLocation(VGuint program, VGuint color, VGint name_hash);
void vgBindFragDataLocation(VGuint program, VGuint color, const VGchar* name);
VGint vgGetFragDataLocation(VGuint program, VGint name_hash);
VGint vgGetFragDataLocation(VGuint program, const VGchar* name);
void vgUniform1ui(VGint location, VGuint v0);
void vgUniform2ui(VGint location, VGuint v0, VGuint v1);
void vgUniform3ui(VGint location, VGuint v0, VGuint v1, VGuint v2);
void vgUniform4ui(VGint location, VGuint v0, VGuint v1, VGuint v2, VGuint v3);
void vgUniform1uiv(VGint location, VGsizei count, const VGuint* value);
void vgUniform2uiv(VGint location, VGsizei count, const VGuint* value);
void vgUniform3uiv(VGint location, VGsizei count, const VGuint* value);
void vgUniform4uiv(VGint location, VGsizei count, const VGuint* value);
// void vgTexParameterIiv(VGenum target, VGenum pname, const VGint* params);
// void vgTexParameterIuiv(VGenum target, VGenum pname, const VGuint* params);
// void vgGetTexParameterIiv(VGenum target, VGenum pname, VGint* params);
// void vgGetTexParameterIuiv(VGenum target, VGenum pname, VGuint* params);
void vgClearBufferiv(VGenum buffer, VGint drawbuffer, const VGint* value);
void vgClearBufferuiv(VGenum buffer, VGint drawbuffer, const VGuint* value);
void vgClearBufferfv(VGenum buffer, VGint drawbuffer, const VGfloat* value);
void vgClearBufferfi(VGenum buffer, VGint drawbuffer, VGfloat depth,
                     VGint stencil);
// const VGubyte* vgGetStringi(VGenum name, VGuint index);
VGboolean vgIsRenderbuffer(VGuint renderbuffer);
void vgBindRenderbuffer(VGenum target, VGuint renderbuffer);
void vgDeleteRenderbuffers(VGsizei n, const VGuint* renderbuffers);
void vgGenRenderbuffers(VGsizei n, VGuint* renderbuffers);
void vgRenderbufferStorage(VGenum target, VGenum internalformat, VGsizei width,
                           VGsizei height);
// void vgGetRenderbufferParameteriv(VGenum target, VGenum pname, VGint*
// params);
VGboolean vgIsFramebuffer(VGuint framebuffer);
void vgBindFramebuffer(VGenum target, VGuint framebuffer);
void vgDeleteFramebuffers(VGsizei n, const VGuint* framebuffers);
void vgGenFramebuffers(VGsizei n, VGuint* framebuffers);
VGenum vgCheckFramebufferStatus(VGenum target);
void vgFramebufferTexture1D(VGenum target, VGenum attachment, VGenum textarget,
                            VGuint texture, VGint level);
void vgFramebufferTexture2D(VGenum target, VGenum attachment, VGenum textarget,
                            VGuint texture, VGint level);
void vgFramebufferTexture3D(VGenum target, VGenum attachment, VGenum textarget,
                            VGuint texture, VGint level, VGint zoffset);
void vgFramebufferRenderbuffer(VGenum target, VGenum attachment,
                               VGenum renderbuffertarget, VGuint renderbuffer);
// void vgGetFramebufferAttachmentParameteriv(VGenum target, VGenum attachment,
//                                            VGenum pname, VGint* params);
// void vgGenerateMipmap(VGenum target);
// void vgBlitFramebuffer(VGint srcX0, VGint srcY0, VGint srcX1, VGint srcY1,
//                        VGint dstX0, VGint dstY0, VGint dstX1, VGint dstY1,
//                        VGbitfield mask, VGenum filter);
// void vgRenderbufferStorageMultisample(VGenum target, VGsizei samples,
//                                       VGenum internalformat, VGsizei width,
//                                       VGsizei height);
// void vgFramebufferTextureLayer(VGenum target, VGenum attachment, VGuint
// texture,
//                                VGint level, VGint layer);
// void* vgMapBufferRange(VGenum target, VGintptr offset, VGsizeiptr length,
//                        VGbitfield access);
// void vgFlushMappedBufferRange(VGenum target, VGintptr offset,
//                               VGsizeiptr length);
void vgBindVertexArray(VGuint array);
void vgDeleteVertexArrays(VGsizei n, const VGuint* arrays);
void vgGenVertexArrays(VGsizei n, VGuint* arrays);
VGboolean vgIsVertexArray(VGuint array);

//////////////////////////////////////////////////
// GL VERSION 3.1 API
//////////////////////////////////////////////////
INLINE constexpr VGenum VG_SAMPLER_2D_RECT = 0x8B63;
INLINE constexpr VGenum VG_SAMPLER_2D_RECT_SHADOW = 0x8B64;
INLINE constexpr VGenum VG_SAMPLER_BUFFER = 0x8DC2;
INLINE constexpr VGenum VG_INT_SAMPLER_2D_RECT = 0x8DCD;
INLINE constexpr VGenum VG_INT_SAMPLER_BUFFER = 0x8DD0;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_2D_RECT = 0x8DD5;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_BUFFER = 0x8DD8;
INLINE constexpr VGenum VG_TEXTURE_BUFFER = 0x8C2A;
INLINE constexpr VGenum VG_MAX_TEXTURE_BUFFER_SIZE = 0x8C2B;
INLINE constexpr VGenum VG_TEXTURE_BINDING_BUFFER = 0x8C2C;
INLINE constexpr VGenum VG_TEXTURE_BUFFER_DATA_STORE_BINDING = 0x8C2D;
INLINE constexpr VGenum VG_TEXTURE_RECTANGLE = 0x84F5;
INLINE constexpr VGenum VG_TEXTURE_BINDING_RECTANGLE = 0x84F6;
INLINE constexpr VGenum VG_PROXY_TEXTURE_RECTANGLE = 0x84F7;
INLINE constexpr VGenum VG_MAX_RECTANGLE_TEXTURE_SIZE = 0x84F8;
INLINE constexpr VGenum VG_R8_SNORM = 0x8F94;
INLINE constexpr VGenum VG_RG8_SNORM = 0x8F95;
INLINE constexpr VGenum VG_RGB8_SNORM = 0x8F96;
INLINE constexpr VGenum VG_RGBA8_SNORM = 0x8F97;
INLINE constexpr VGenum VG_R16_SNORM = 0x8F98;
INLINE constexpr VGenum VG_RG16_SNORM = 0x8F99;
INLINE constexpr VGenum VG_RGB16_SNORM = 0x8F9A;
INLINE constexpr VGenum VG_RGBA16_SNORM = 0x8F9B;
INLINE constexpr VGenum VG_SIGNED_NORMALIZED = 0x8F9C;
INLINE constexpr VGenum VG_PRIMITIVE_RESTART = 0x8F9D;
INLINE constexpr VGenum VG_PRIMITIVE_RESTART_INDEX = 0x8F9E;
INLINE constexpr VGenum VG_COPY_READ_BUFFER = 0x8F36;
INLINE constexpr VGenum VG_COPY_WRITE_BUFFER = 0x8F37;
INLINE constexpr VGenum VG_UNIFORM_BUFFER = 0x8A11;
INLINE constexpr VGenum VG_UNIFORM_BUFFER_BINDING = 0x8A28;
INLINE constexpr VGenum VG_UNIFORM_BUFFER_START = 0x8A29;
INLINE constexpr VGenum VG_UNIFORM_BUFFER_SIZE = 0x8A2A;
INLINE constexpr VGenum VG_MAX_VERTEX_UNIFORM_BLOCKS = 0x8A2B;
INLINE constexpr VGenum VG_MAX_GEOMETRY_UNIFORM_BLOCKS = 0x8A2C;
INLINE constexpr VGenum VG_MAX_FRAGMENT_UNIFORM_BLOCKS = 0x8A2D;
INLINE constexpr VGenum VG_MAX_COMBINED_UNIFORM_BLOCKS = 0x8A2E;
INLINE constexpr VGenum VG_MAX_UNIFORM_BUFFER_BINDINGS = 0x8A2F;
INLINE constexpr VGenum VG_MAX_UNIFORM_BLOCK_SIZE = 0x8A30;
INLINE constexpr VGenum VG_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS = 0x8A31;
INLINE constexpr VGenum VG_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS = 0x8A32;
INLINE constexpr VGenum VG_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = 0x8A33;
INLINE constexpr VGenum VG_UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0x8A34;
INLINE constexpr VGenum VG_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH = 0x8A35;
INLINE constexpr VGenum VG_ACTIVE_UNIFORM_BLOCKS = 0x8A36;
INLINE constexpr VGenum VG_UNIFORM_TYPE = 0x8A37;
INLINE constexpr VGenum VG_UNIFORM_SIZE = 0x8A38;
INLINE constexpr VGenum VG_UNIFORM_NAME_LENGTH = 0x8A39;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_INDEX = 0x8A3A;
INLINE constexpr VGenum VG_UNIFORM_OFFSET = 0x8A3B;
INLINE constexpr VGenum VG_UNIFORM_ARRAY_STRIDE = 0x8A3C;
INLINE constexpr VGenum VG_UNIFORM_MATRIX_STRIDE = 0x8A3D;
INLINE constexpr VGenum VG_UNIFORM_IS_ROW_MAJOR = 0x8A3E;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_BINDING = 0x8A3F;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_DATA_SIZE = 0x8A40;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_NAME_LENGTH = 0x8A41;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_ACTIVE_UNIFORMS = 0x8A42;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES = 0x8A43;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER = 0x8A44;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER = 0x8A45;
INLINE constexpr VGenum VG_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER = 0x8A46;
INLINE constexpr VGenum VG_INVALID_INDEX = 0xFFFFFFFFu;

// void vgDrawArraysInstanced(VGenum mode, VGint first, VGsizei count,
//                            VGsizei instancecount);
// void vgDrawElementsInstanced(VGenum mode, VGsizei count, VGenum type,
//                              const void* indices, VGsizei instancecount);
// void vgTexBuffer(VGenum target, VGenum internalformat, VGuint buffer);
// void vgPrimitiveRestartIndex(VGuint index);
// void vgCopyBufferSubData(VGenum readTarget, VGenum writeTarget,
//                          VGintptr readOffset, VGintptr writeOffset,
//                          VGsizeiptr size);
// void vgGetUniformIndices(VGuint program, VGsizei uniformCount,
//                          const VGchar* const* uniformNames,
//                          VGuint* uniformIndices);
// void vgGetActiveUniformsiv(VGuint program, VGsizei uniformCount,
//                            const VGuint* uniformIndices, VGenum pname,
//                            VGint* params);
// void vgGetActiveUniformName(VGuint program, VGuint uniformIndex,
//                             VGsizei bufSize, VGsizei* length,
//                             VGchar* uniformName);
// VGuint vgGetUniformBlockIndex(VGuint program, const VGchar*
// uniformBlockName); void vgGetActiveUniformBlockiv(VGuint program, VGuint
// uniformBlockIndex,
//                                VGenum pname, VGint* params);
// void vgGetActiveUniformBlockName(VGuint program, VGuint uniformBlockIndex,
//                                  VGsizei bufSize, VGsizei* length,
//                                  VGchar* uniformBlockName);
// void vgUniformBlockBinding(VGuint program, VGuint uniformBlockIndex,
//                            VGuint uniformBlockBinding);

//////////////////////////////////////////////////
// GL VERSION 3.2 API
//////////////////////////////////////////////////
using VGsync = struct __VGsync*;
using VGuint64 = uint64_t;
using VGint64 = int64_t;
INLINE constexpr VGenum VG_CONTEXT_CORE_PROFILE_BIT = 0x00000001;
INLINE constexpr VGenum VG_CONTEXT_COMPATIBILITY_PROFILE_BIT = 0x00000002;
INLINE constexpr VGenum VG_LINES_ADJACENCY = 0x000A;
INLINE constexpr VGenum VG_LINE_STRIP_ADJACENCY = 0x000B;
INLINE constexpr VGenum VG_TRIANGLES_ADJACENCY = 0x000C;
INLINE constexpr VGenum VG_TRIANGLE_STRIP_ADJACENCY = 0x000D;
INLINE constexpr VGenum VG_PROGRAM_POINT_SIZE = 0x8642;
INLINE constexpr VGenum VG_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS = 0x8C29;
INLINE constexpr VGenum VG_FRAMEBUFFER_ATTACHMENT_LAYERED = 0x8DA7;
INLINE constexpr VGenum VG_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS = 0x8DA8;
INLINE constexpr VGenum VG_GEOMETRY_SHADER = 0x8DD9;
INLINE constexpr VGenum VG_GEOMETRY_VERTICES_OUT = 0x8916;
INLINE constexpr VGenum VG_GEOMETRY_INPUT_TYPE = 0x8917;
INLINE constexpr VGenum VG_GEOMETRY_OUTPUT_TYPE = 0x8918;
INLINE constexpr VGenum VG_MAX_GEOMETRY_UNIFORM_COMPONENTS = 0x8DDF;
INLINE constexpr VGenum VG_MAX_GEOMETRY_OUTPUT_VERTICES = 0x8DE0;
INLINE constexpr VGenum VG_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS = 0x8DE1;
INLINE constexpr VGenum VG_MAX_VERTEX_OUTPUT_COMPONENTS = 0x9122;
INLINE constexpr VGenum VG_MAX_GEOMETRY_INPUT_COMPONENTS = 0x9123;
INLINE constexpr VGenum VG_MAX_GEOMETRY_OUTPUT_COMPONENTS = 0x9124;
INLINE constexpr VGenum VG_MAX_FRAGMENT_INPUT_COMPONENTS = 0x9125;
INLINE constexpr VGenum VG_CONTEXT_PROFILE_MASK = 0x9126;
INLINE constexpr VGenum VG_DEPTH_CLAMP = 0x864F;
INLINE constexpr VGenum VG_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION = 0x8E4C;
INLINE constexpr VGenum VG_FIRST_VERTEX_CONVENTION = 0x8E4D;
INLINE constexpr VGenum VG_LAST_VERTEX_CONVENTION = 0x8E4E;
INLINE constexpr VGenum VG_PROVOKING_VERTEX = 0x8E4F;
INLINE constexpr VGenum VG_TEXTURE_CUBE_MAP_SEAMLESS = 0x884F;
INLINE constexpr VGenum VG_MAX_SERVER_WAIT_TIMEOUT = 0x9111;
INLINE constexpr VGenum VG_OBJECT_TYPE = 0x9112;
INLINE constexpr VGenum VG_SYNC_CONDITION = 0x9113;
INLINE constexpr VGenum VG_SYNC_STATUS = 0x9114;
INLINE constexpr VGenum VG_SYNC_FLAGS = 0x9115;
INLINE constexpr VGenum VG_SYNC_FENCE = 0x9116;
INLINE constexpr VGenum VG_SYNC_GPU_COMMANDS_COMPLETE = 0x9117;
INLINE constexpr VGenum VG_UNSIGNALED = 0x9118;
INLINE constexpr VGenum VG_SIGNALED = 0x9119;
INLINE constexpr VGenum VG_ALREADY_SIGNALED = 0x911A;
INLINE constexpr VGenum VG_TIMEOUT_EXPIRED = 0x911B;
INLINE constexpr VGenum VG_CONDITION_SATISFIED = 0x911C;
INLINE constexpr VGenum VG_WAIT_FAILED = 0x911D;
INLINE constexpr VGuint64 VG_TIMEOUT_IGNORED = 0xFFFFFFFFFFFFFFFFull;
INLINE constexpr VGenum VG_SYNC_FLUSH_COMMANDS_BIT = 0x00000001;
INLINE constexpr VGenum VG_SAMPLE_POSITION = 0x8E50;
INLINE constexpr VGenum VG_SAMPLE_MASK = 0x8E51;
INLINE constexpr VGenum VG_SAMPLE_MASK_VALUE = 0x8E52;
INLINE constexpr VGenum VG_MAX_SAMPLE_MASK_WORDS = 0x8E59;
INLINE constexpr VGenum VG_TEXTURE_2D_MULTISAMPLE = 0x9100;
INLINE constexpr VGenum VG_PROXY_TEXTURE_2D_MULTISAMPLE = 0x9101;
INLINE constexpr VGenum VG_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9102;
INLINE constexpr VGenum VG_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY = 0x9103;
INLINE constexpr VGenum VG_TEXTURE_BINDING_2D_MULTISAMPLE = 0x9104;
INLINE constexpr VGenum VG_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY = 0x9105;
INLINE constexpr VGenum VG_TEXTURE_SAMPLES = 0x9106;
INLINE constexpr VGenum VG_TEXTURE_FIXED_SAMPLE_LOCATIONS = 0x9107;
INLINE constexpr VGenum VG_SAMPLER_2D_MULTISAMPLE = 0x9108;
INLINE constexpr VGenum VG_INT_SAMPLER_2D_MULTISAMPLE = 0x9109;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE = 0x910A;
INLINE constexpr VGenum VG_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910B;
INLINE constexpr VGenum VG_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910C;
INLINE constexpr VGenum VG_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 0x910D;
INLINE constexpr VGenum VG_MAX_COLOR_TEXTURE_SAMPLES = 0x910E;
INLINE constexpr VGenum VG_MAX_DEPTH_TEXTURE_SAMPLES = 0x910F;
INLINE constexpr VGenum VG_MAX_INTEGER_SAMPLES = 0x9110;

// void vgDrawElementsBaseVertex(VGenum mode, VGsizei count, VGenum type,
//                               const void* indices, VGint basevertex);
// void vgDrawRangeElementsBaseVertex(VGenum mode, VGuint start, VGuint end,
//                                    VGsizei count, VGenum type,
//                                    const void* indices, VGint basevertex);
// void vgDrawElementsInstancedBaseVertex(VGenum mode, VGsizei count, VGenum
// type,
//                                        const void* indices,
//                                        VGsizei instancecount, VGint
//                                        basevertex);
// void vgMultiDrawElementsBaseVertex(VGenum mode, const VGsizei* count,
//                                    VGenum type, const void* const* indices,
//                                    VGsizei drawcount, const VGint*
//                                    basevertex);
// void vgProvokingVertex(VGenum mode);
// VGsync vgFenceSync(VGenum condition, VGbitfield flags);
// VGboolean vgIsSync(VGsync sync);
// void vgDeleteSync(VGsync sync);
// VGenum vgClientWaitSync(VGsync sync, VGbitfield flags, VGuint64 timeout);
// void vgWaitSync(VGsync sync, VGbitfield flags, VGuint64 timeout);
// void vgGetInteger64v(VGenum pname, VGint64* data);
// void vgGetSynciv(VGsync sync, VGenum pname, VGsizei count, VGsizei* length,
//                  VGint* values);
// void vgGetInteger64i_v(VGenum target, VGuint index, VGint64* data);
// void vgGetBufferParameteri64v(VGenum target, VGenum pname, VGint64* params);
void vgFramebufferTexture(VGenum target, VGenum attachment, VGuint texture,
                          VGint level);
// void vgTexImage2DMultisample(VGenum target, VGsizei samples,
//                              VGenum internalformat, VGsizei width,
//                              VGsizei height, VGboolean fixedsamplelocations);
// void vgTexImage3DMultisample(VGenum target, VGsizei samples,
//                              VGenum internalformat, VGsizei width,
//                              VGsizei height, VGsizei depth,
//                              VGboolean fixedsamplelocations);
// void vgGetMultisamplefv(VGenum pname, VGuint index, VGfloat* val);
// void vgSampleMaski(VGuint maskNumber, VGbitfield mask);

//////////////////////////////////////////////////
// GL VERSION 3.3 API
//////////////////////////////////////////////////
INLINE constexpr VGenum VG_VERTEX_ATTRIB_ARRAY_DIVISOR = 0x88FE;
INLINE constexpr VGenum VG_SRC1_COLOR = 0x88F9;
INLINE constexpr VGenum VG_ONE_MINUS_SRC1_COLOR = 0x88FA;
INLINE constexpr VGenum VG_ONE_MINUS_SRC1_ALPHA = 0x88FB;
INLINE constexpr VGenum VG_MAX_DUAL_SOURCE_DRAW_BUFFERS = 0x88FC;
INLINE constexpr VGenum VG_ANY_SAMPLES_PASSED = 0x8C2F;
INLINE constexpr VGenum VG_SAMPLER_BINDING = 0x8919;
INLINE constexpr VGenum VG_RGB10_A2UI = 0x906F;
INLINE constexpr VGenum VG_TEXTURE_SWIZZLE_R = 0x8E42;
INLINE constexpr VGenum VG_TEXTURE_SWIZZLE_G = 0x8E43;
INLINE constexpr VGenum VG_TEXTURE_SWIZZLE_B = 0x8E44;
INLINE constexpr VGenum VG_TEXTURE_SWIZZLE_A = 0x8E45;
INLINE constexpr VGenum VG_TEXTURE_SWIZZLE_RGBA = 0x8E46;
INLINE constexpr VGenum VG_TIME_ELAPSED = 0x88BF;
INLINE constexpr VGenum VG_TIMESTAMP = 0x8E28;
INLINE constexpr VGenum VG_INT_2_10_10_10_REV = 0x8D9F;

// void vgBindFragDataLocationIndexed(VGuint program, VGuint colorNumber,
//                                    VGuint index, const VGchar* name);
// VGint vgGetFragDataIndex(VGuint program, const VGchar* name);
void vgGenSamplers(VGsizei count, VGuint* samplers);
void vgDeleteSamplers(VGsizei count, const VGuint* samplers);
VGboolean vgIsSampler(VGuint sampler);
void vgBindSampler(VGuint unit, VGuint sampler);
void vgSamplerParameteri(VGuint sampler, VGenum pname, VGint param);
void vgSamplerParameteriv(VGuint sampler, VGenum pname, const VGint* param);
void vgSamplerParameterf(VGuint sampler, VGenum pname, VGfloat param);
void vgSamplerParameterfv(VGuint sampler, VGenum pname, const VGfloat* param);
// void vgSamplerParameterIiv(VGuint sampler, VGenum pname, const VGint* param);
// void vgSamplerParameterIuiv(VGuint sampler, VGenum pname, const VGuint* param);
// void vgGetSamplerParameteriv(VGuint sampler, VGenum pname, VGint* params);
// void vgGetSamplerParameterIiv(VGuint sampler, VGenum pname, VGint* params);
// void vgGetSamplerParameterfv(VGuint sampler, VGenum pname, VGfloat* params);
// void vgGetSamplerParameterIuiv(VGuint sampler, VGenum pname, VGuint* params);
// void vgQueryCounter(VGuint id, VGenum target);
// void vgGetQueryObjecti64v(VGuint id, VGenum pname, VGint64* params);
// void vgGetQueryObjectui64v(VGuint id, VGenum pname, VGuint64* params);
// void vgVertexAttribDivisor(VGuint index, VGuint divisor);
// void vgVertexAttribP1ui(VGuint index, VGenum type, VGboolean normalized,
//                         VGuint value);
// void vgVertexAttribP1uiv(VGuint index, VGenum type, VGboolean normalized,
//                          const VGuint* value);
// void vgVertexAttribP2ui(VGuint index, VGenum type, VGboolean normalized,
//                         VGuint value);
// void vgVertexAttribP2uiv(VGuint index, VGenum type, VGboolean normalized,
//                          const VGuint* value);
// void vgVertexAttribP3ui(VGuint index, VGenum type, VGboolean normalized,
//                         VGuint value);
// void vgVertexAttribP3uiv(VGuint index, VGenum type, VGboolean normalized,
//                          const VGuint* value);
// void vgVertexAttribP4ui(VGuint index, VGenum type, VGboolean normalized,
//                         VGuint value);
// void vgVertexAttribP4uiv(VGuint index, VGenum type, VGboolean normalized,
//                          const VGuint* value);

}  // namespace ho