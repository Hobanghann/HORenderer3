#pragma once

#include <cstdint>
#include <cstring>
#include <string>

#include "math_defs.h"
#include "math_funcs.h"

namespace ho {
    struct Color128;
    struct Color32 {
       public:
        static const Color32 ALICE_BLUE;
        static const Color32 ANTIQUE_WHITE;
        static const Color32 AQUA;
        static const Color32 AQUAMARINE;
        static const Color32 AZURE;
        static const Color32 BEIGE;
        static const Color32 BISQUE;
        static const Color32 BLACK;
        static const Color32 BLANCHED_ALMOND;
        static const Color32 BLUE;
        static const Color32 BLUE_VIOLET;
        static const Color32 BROWN;
        static const Color32 BURLYWOOD;
        static const Color32 CADET_BLUE;
        static const Color32 CHARTREUSE;
        static const Color32 CHOCOLATE;
        static const Color32 CORAL;
        static const Color32 CORNFLOWErBLUE;
        static const Color32 CORNSILK;
        static const Color32 CRIMSON;
        static const Color32 CYAN;
        static const Color32 DARK_BLUE;
        static const Color32 DARK_CYAN;
        static const Color32 DARK_GOLDENROD;
        static const Color32 DARK_GRAY;
        static const Color32 DARK_GREEN;
        static const Color32 DARK_KHAKI;
        static const Color32 DARK_MAGENTA;
        static const Color32 DARK_OLIVE_GREEN;
        static const Color32 DARK_ORANGE;
        static const Color32 DARK_ORCHID;
        static const Color32 DARK_RED;
        static const Color32 DARK_SALMON;
        static const Color32 DARK_SEaGREEN;
        static const Color32 DARK_SLATE_BLUE;
        static const Color32 DARK_SLATE_GRAY;
        static const Color32 DARK_TURQUOISE;
        static const Color32 DARK_VIOLET;
        static const Color32 DEEP_PINK;
        static const Color32 DEEP_SKY_BLUE;
        static const Color32 DIM_GRAY;
        static const Color32 DODGErBLUE;
        static const Color32 FIREBRICK;
        static const Color32 FLORAL_WHITE;
        static const Color32 FOREST_GREEN;
        static const Color32 FUCHSIA;
        static const Color32 GAINSBORO;
        static const Color32 GHOST_WHITE;
        static const Color32 GOLD;
        static const Color32 GOLDENROD;
        static const Color32 GRAY;
        static const Color32 GREEN;
        static const Color32 GREEN_YELLOW;
        static const Color32 HONEYDEW;
        static const Color32 HOT_PINK;
        static const Color32 INDIAN_RED;
        static const Color32 INDIGO;
        static const Color32 IVORY;
        static const Color32 KHAKI;
        static const Color32 LAVENDER;
        static const Color32 LAVENDErBLUSH;
        static const Color32 LAWN_GREEN;
        static const Color32 LEMON_CHIFFON;
        static const Color32 LIGHT_BLUE;
        static const Color32 LIGHT_CORAL;
        static const Color32 LIGHT_CYAN;
        static const Color32 LIGHT_GOLDENROD;
        static const Color32 LIGHT_GRAY;
        static const Color32 LIGHT_GREEN;
        static const Color32 LIGHT_PINK;
        static const Color32 LIGHT_SALMON;
        static const Color32 LIGHT_SEaGREEN;
        static const Color32 LIGHT_SKY_BLUE;
        static const Color32 LIGHT_SLATE_GRAY;
        static const Color32 LIGHT_STEEL_BLUE;
        static const Color32 LIGHT_YELLOW;
        static const Color32 LIME;
        static const Color32 LIME_GREEN;
        static const Color32 LINEN;
        static const Color32 MAGENTA;
        static const Color32 MAROON;
        static const Color32 MEDIUM_AQUAMARINE;
        static const Color32 MEDIUM_BLUE;
        static const Color32 MEDIUM_ORCHID;
        static const Color32 MEDIUM_PURPLE;
        static const Color32 MEDIUM_SEaGREEN;
        static const Color32 MEDIUM_SLATE_BLUE;
        static const Color32 MEDIUM_SPRINgGREEN;
        static const Color32 MEDIUM_TURQUOISE;
        static const Color32 MEDIUM_VIOLET_RED;
        static const Color32 MIDNIGHT_BLUE;
        static const Color32 MINT_CREAM;
        static const Color32 MISTY_ROSE;
        static const Color32 MOCCASIN;
        static const Color32 NAVAJO_WHITE;
        static const Color32 NAVY_BLUE;
        static const Color32 OLD_LACE;
        static const Color32 OLIVE;
        static const Color32 OLIVE_DRAB;
        static const Color32 ORANGE;
        static const Color32 ORANGE_RED;
        static const Color32 ORCHID;
        static const Color32 PALE_GOLDENROD;
        static const Color32 PALE_GREEN;
        static const Color32 PALE_TURQUOISE;
        static const Color32 PALE_VIOLET_RED;
        static const Color32 PAPAYaWHIP;
        static const Color32 PEACH_PUFF;
        static const Color32 PERU;
        static const Color32 PINK;
        static const Color32 PLUM;
        static const Color32 POWDErBLUE;
        static const Color32 PURPLE;
        static const Color32 REBECCaPURPLE;
        static const Color32 RED;
        static const Color32 ROSY_BROWN;
        static const Color32 ROYAL_BLUE;
        static const Color32 SADDLE_BROWN;
        static const Color32 SALMON;
        static const Color32 SANDY_BROWN;
        static const Color32 SEaGREEN;
        static const Color32 SEASHELL;
        static const Color32 SIENNA;
        static const Color32 SILVER;
        static const Color32 SKY_BLUE;
        static const Color32 SLATE_BLUE;
        static const Color32 SLATE_GRAY;
        static const Color32 SNOW;
        static const Color32 SPRINgGREEN;
        static const Color32 STEEL_BLUE;
        static const Color32 TAN;
        static const Color32 TEAL;
        static const Color32 THISTLE;
        static const Color32 TOMATO;
        static const Color32 TURQUOISE;
        static const Color32 VIOLET;
        static const Color32 WEbGRAY;
        static const Color32 WEbGREEN;
        static const Color32 WEbMAROON;
        static const Color32 WEbPURPLE;
        static const Color32 WHEAT;
        static const Color32 WHITE;
        static const Color32 WHITE_SMOKE;
        static const Color32 YELLOW;
        static const Color32 YELLOW_GREEN;

        constexpr Color32();
        constexpr Color32(const Color32&) = default;
        constexpr Color32(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
        explicit Color32(const Color128& c);
        constexpr Color32& operator=(const Color32& rhs);
        ~Color32() = default;

        constexpr Color32 operator+(const Color32& rhs) const;
        constexpr Color32& operator+=(const Color32& rhs);

        constexpr Color32 operator-(const Color32& rhs) const;
        constexpr Color32& operator-=(const Color32& rhs);

        constexpr Color32 operator*(const Color32& rhs) const;
        constexpr Color32& operator*=(const Color32& rhs);

        constexpr Color32 operator*(uint8_t scalar) const;
        constexpr Color32& operator*=(uint8_t scalar);

        constexpr Color32 operator/(const Color32& rhs) const;
        constexpr Color32& operator/=(const Color32& rhs);

        constexpr Color32 operator/(uint8_t scalar) const;
        constexpr Color32& operator/=(uint8_t scalar);

        constexpr bool operator==(const Color32& rhs) const;
        constexpr bool operator!=(const Color32& rhs) const;

        constexpr real Luminance() const;
        constexpr real Luminance(real rweight, real gweight, real bweight) const;

        ALWAYS_INLINE uint8_t ToR8() const;
        ALWAYS_INLINE uint16_t ToRG8() const;
        ALWAYS_INLINE uint32_t ToRGB8() const;
        ALWAYS_INLINE uint32_t ToRGBA8() const;
        ALWAYS_INLINE uint32_t ToBGRA8() const;
        ALWAYS_INLINE uint32_t ToARGB8() const;
        ALWAYS_INLINE uint32_t ToABGR8() const;
        ALWAYS_INLINE uint8_t ToL8() const;
        ALWAYS_INLINE uint16_t ToLA8() const;

        std::string ToString() const;

        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

       private:
        ALWAYS_INLINE constexpr uint8_t UpperU8(int v) const { return static_cast<uint8_t>(math::Min(255, v)); }
        ALWAYS_INLINE constexpr uint8_t LowerU8(int v) const { return static_cast<uint8_t>(math::Max(v, 0)); }
        ALWAYS_INLINE constexpr uint8_t DivU8(uint8_t x, uint8_t y) const {
            return y == 0 ? 255 : static_cast<uint8_t>(static_cast<real>(x) / static_cast<real>(y));
        }
    };

    constexpr Color32::Color32() : r(0), g(0), b(0), a(255) {}
    constexpr Color32::Color32(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
        : r(red), g(green), b(blue), a(alpha) {}

    constexpr Color32& Color32::operator=(const Color32& rhs) {
        if (this == &rhs) {
            return *this;
        }
        r = rhs.r;
        g = rhs.g;
        b = rhs.b;
        a = rhs.a;
        return *this;
    }

    constexpr bool Color32::operator==(const Color32& rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
    }

    constexpr bool Color32::operator!=(const Color32& rhs) const { return !(*this == rhs); }

    constexpr Color32 Color32::operator+(const Color32& rhs) const {
        return Color32(UpperU8(r + rhs.r), UpperU8(g + rhs.g), UpperU8(b + rhs.b), UpperU8(a + rhs.a));
    }
    constexpr Color32& Color32::operator+=(const Color32& rhs) {
        r = UpperU8(r + rhs.r);
        g = UpperU8(g + rhs.g);
        b = UpperU8(b + rhs.b);
        a = UpperU8(a + rhs.a);
        return *this;
    }

    constexpr Color32 Color32::operator-(const Color32& rhs) const {
        return Color32(LowerU8(r - rhs.r), LowerU8(g - rhs.g), LowerU8(b - rhs.b), LowerU8(a - rhs.a));
    }
    constexpr Color32& Color32::operator-=(const Color32& rhs) {
        r = LowerU8(r - rhs.r);
        g = LowerU8(g - rhs.g);
        b = LowerU8(b - rhs.b);
        a = LowerU8(a - rhs.a);
        return *this;
    }

    constexpr Color32 Color32::operator*(const Color32& rhs) const {
        return Color32(UpperU8(r * rhs.r), UpperU8(g * rhs.g), UpperU8(b * rhs.b), UpperU8(a * rhs.a));
    }

    constexpr Color32& Color32::operator*=(const Color32& rhs) {
        r = UpperU8(r * rhs.r);
        g = UpperU8(g * rhs.g);
        b = UpperU8(b * rhs.b);
        a = UpperU8(a * rhs.a);
        return *this;
    }

    constexpr Color32 Color32::operator*(uint8_t scalar) const {
        return Color32(UpperU8(r * scalar), UpperU8(g * scalar), UpperU8(b * scalar), UpperU8(a * scalar));
    }

    constexpr Color32& Color32::operator*=(uint8_t scalar) {
        r = UpperU8(r * scalar);
        g = UpperU8(g * scalar);
        b = UpperU8(b * scalar);
        a = UpperU8(a * scalar);
        return *this;
    }

    constexpr Color32 Color32::operator/(const Color32& rhs) const {
        return Color32(DivU8(r, rhs.r), DivU8(g, rhs.g), DivU8(b, rhs.b), DivU8(a, rhs.a));
    }

    constexpr Color32& Color32::operator/=(const Color32& rhs) {
        r = DivU8(r, rhs.r);
        g = DivU8(g, rhs.g);
        b = DivU8(b, rhs.b);
        a = DivU8(a, rhs.a);
        return *this;
    }

    constexpr Color32 Color32::operator/(uint8_t scalar) const {
        if (scalar == 0) {
            return Color32(255, 255, 255, 255);
        }
        return Color32(DivU8(r, scalar), DivU8(g, scalar), DivU8(b, scalar), DivU8(a, scalar));
    }

    constexpr Color32& Color32::operator/=(uint8_t scalar) {
        if (scalar == 0) {
            r = 255;
            g = 255;
            b = 255;
            a = 255;
        }
        r = DivU8(r, scalar);
        g = DivU8(g, scalar);
        b = DivU8(b, scalar);
        a = DivU8(a, scalar);
        return *this;
    }

    ALWAYS_INLINE constexpr Color32 operator*(uint8_t scalar, const Color32& c) { return c * scalar; }

    constexpr real Color32::Luminance() const {
        return 0.2126_r * static_cast<real>(r) + 0.7152_r * static_cast<real>(g) + 0.0722_r * static_cast<real>(b);
    }
    constexpr real Color32::Luminance(real rweight, real gweight, real bweight) const {
        return static_cast<real>(r) * rweight + static_cast<real>(g) * gweight + static_cast<real>(b) * bweight;
    }

    uint8_t Color32::ToR8() const { return r; }
    uint16_t Color32::ToRG8() const {
        uint8_t out[2];
        out[0] = r;
        out[1] = g;
        uint16_t dst;
        memcpy(&dst, out, 2);
        return dst;
    }
    uint32_t Color32::ToRGB8() const {
        uint8_t out[4];
        out[0] = r;
        out[1] = g;
        out[2] = b;
        out[3] = 0;
        uint32_t dst;
        memcpy(&dst, out, 4);
        return dst;
    }
    uint32_t Color32::ToRGBA8() const {
        uint8_t out[4];
        out[0] = r;
        out[1] = g;
        out[2] = b;
        out[3] = a;
        uint32_t dst;
        memcpy(&dst, out, 4);
        return dst;
    }

    uint32_t Color32::ToBGRA8() const {
        uint8_t out[4];
        out[0] = b;
        out[1] = g;
        out[2] = r;
        out[3] = a;
        uint32_t dst;
        memcpy(&dst, out, 4);
        return dst;
    }

    uint32_t Color32::ToARGB8() const {
        uint8_t out[4];
        out[0] = a;
        out[1] = r;
        out[2] = g;
        out[3] = b;
        uint32_t dst;
        memcpy(&dst, out, 4);
        return dst;
    }

    uint32_t Color32::ToABGR8() const {
        uint8_t out[4];
        out[0] = a;
        out[1] = b;
        out[2] = g;
        out[3] = r;
        uint32_t dst;
        memcpy(&dst, out, 4);
        return dst;
    }

    uint8_t Color32::ToL8() const { return r; }
    uint16_t Color32::ToLA8() const {
        uint8_t out[2];
        out[0] = r;
        out[1] = a;
        uint16_t dst;
        memcpy(&dst, out, 2);
        return dst;
    }

    INLINE constexpr Color32 Color32::ALICE_BLUE = Color32(240, 248, 255, 255);
    INLINE constexpr Color32 Color32::ANTIQUE_WHITE = Color32(250, 235, 215, 255);
    INLINE constexpr Color32 Color32::AQUA = Color32(0, 255, 255, 255);
    INLINE constexpr Color32 Color32::AQUAMARINE = Color32(127, 255, 212, 255);
    INLINE constexpr Color32 Color32::AZURE = Color32(240, 255, 255, 255);
    INLINE constexpr Color32 Color32::BEIGE = Color32(245, 245, 220, 255);
    INLINE constexpr Color32 Color32::BISQUE = Color32(255, 228, 196, 255);
    INLINE constexpr Color32 Color32::BLACK = Color32(0, 0, 0, 255);
    INLINE constexpr Color32 Color32::BLANCHED_ALMOND = Color32(255, 235, 205, 255);
    INLINE constexpr Color32 Color32::BLUE = Color32(0, 0, 255, 255);
    INLINE constexpr Color32 Color32::BLUE_VIOLET = Color32(138, 43, 226, 255);
    INLINE constexpr Color32 Color32::BROWN = Color32(165, 42, 42, 255);
    INLINE constexpr Color32 Color32::BURLYWOOD = Color32(222, 184, 135, 255);
    INLINE constexpr Color32 Color32::CADET_BLUE = Color32(95, 158, 160, 255);
    INLINE constexpr Color32 Color32::CHARTREUSE = Color32(127, 255, 0, 255);
    INLINE constexpr Color32 Color32::CHOCOLATE = Color32(210, 105, 30, 255);
    INLINE constexpr Color32 Color32::CORAL = Color32(255, 127, 80, 255);
    INLINE constexpr Color32 Color32::CORNFLOWErBLUE = Color32(100, 149, 237, 255);
    INLINE constexpr Color32 Color32::CORNSILK = Color32(255, 248, 220, 255);
    INLINE constexpr Color32 Color32::CRIMSON = Color32(220, 20, 60, 255);
    INLINE constexpr Color32 Color32::CYAN = Color32(0, 255, 255, 255);
    INLINE constexpr Color32 Color32::DARK_BLUE = Color32(0, 0, 139, 255);
    INLINE constexpr Color32 Color32::DARK_CYAN = Color32(0, 139, 139, 255);
    INLINE constexpr Color32 Color32::DARK_GOLDENROD = Color32(184, 134, 11, 255);
    INLINE constexpr Color32 Color32::DARK_GRAY = Color32(169, 169, 169, 255);
    INLINE constexpr Color32 Color32::DARK_GREEN = Color32(0, 100, 0, 255);
    INLINE constexpr Color32 Color32::DARK_KHAKI = Color32(189, 183, 107, 255);
    INLINE constexpr Color32 Color32::DARK_MAGENTA = Color32(139, 0, 139, 255);
    INLINE constexpr Color32 Color32::DARK_OLIVE_GREEN = Color32(85, 107, 47, 255);
    INLINE constexpr Color32 Color32::DARK_ORANGE = Color32(255, 140, 0, 255);
    INLINE constexpr Color32 Color32::DARK_ORCHID = Color32(153, 50, 204, 255);
    INLINE constexpr Color32 Color32::DARK_RED = Color32(139, 0, 0, 255);
    INLINE constexpr Color32 Color32::DARK_SALMON = Color32(233, 150, 122, 255);
    INLINE constexpr Color32 Color32::DARK_SEaGREEN = Color32(143, 188, 143, 255);
    INLINE constexpr Color32 Color32::DARK_SLATE_BLUE = Color32(72, 61, 139, 255);
    INLINE constexpr Color32 Color32::DARK_SLATE_GRAY = Color32(47, 79, 79, 255);
    INLINE constexpr Color32 Color32::DARK_TURQUOISE = Color32(0, 206, 209, 255);
    INLINE constexpr Color32 Color32::DARK_VIOLET = Color32(148, 0, 211, 255);
    INLINE constexpr Color32 Color32::DEEP_PINK = Color32(255, 20, 147, 255);
    INLINE constexpr Color32 Color32::DEEP_SKY_BLUE = Color32(0, 191, 255, 255);
    INLINE constexpr Color32 Color32::DIM_GRAY = Color32(105, 105, 105, 255);
    INLINE constexpr Color32 Color32::DODGErBLUE = Color32(30, 144, 255, 255);
    INLINE constexpr Color32 Color32::FIREBRICK = Color32(178, 34, 34, 255);
    INLINE constexpr Color32 Color32::FLORAL_WHITE = Color32(255, 250, 240, 255);
    INLINE constexpr Color32 Color32::FOREST_GREEN = Color32(34, 139, 34, 255);
    INLINE constexpr Color32 Color32::FUCHSIA = Color32(255, 0, 255, 255);
    INLINE constexpr Color32 Color32::GAINSBORO = Color32(220, 220, 220, 255);
    INLINE constexpr Color32 Color32::GHOST_WHITE = Color32(248, 248, 255, 255);
    INLINE constexpr Color32 Color32::GOLD = Color32(255, 215, 0, 255);
    INLINE constexpr Color32 Color32::GOLDENROD = Color32(218, 165, 32, 255);
    INLINE constexpr Color32 Color32::GRAY = Color32(190, 190, 190, 255);
    INLINE constexpr Color32 Color32::GREEN = Color32(0, 255, 0, 255);
    INLINE constexpr Color32 Color32::GREEN_YELLOW = Color32(173, 255, 47, 255);
    INLINE constexpr Color32 Color32::HONEYDEW = Color32(240, 255, 240, 255);
    INLINE constexpr Color32 Color32::HOT_PINK = Color32(255, 105, 180, 255);
    INLINE constexpr Color32 Color32::INDIAN_RED = Color32(205, 92, 92, 255);
    INLINE constexpr Color32 Color32::INDIGO = Color32(75, 0, 130, 255);
    INLINE constexpr Color32 Color32::IVORY = Color32(255, 255, 240, 255);
    INLINE constexpr Color32 Color32::KHAKI = Color32(240, 230, 140, 255);
    INLINE constexpr Color32 Color32::LAVENDER = Color32(230, 230, 250, 255);
    INLINE constexpr Color32 Color32::LAVENDErBLUSH = Color32(255, 240, 245, 255);
    INLINE constexpr Color32 Color32::LAWN_GREEN = Color32(124, 252, 0, 255);
    INLINE constexpr Color32 Color32::LEMON_CHIFFON = Color32(255, 250, 205, 255);
    INLINE constexpr Color32 Color32::LIGHT_BLUE = Color32(173, 216, 230, 255);
    INLINE constexpr Color32 Color32::LIGHT_CORAL = Color32(240, 128, 128, 255);
    INLINE constexpr Color32 Color32::LIGHT_CYAN = Color32(224, 255, 255, 255);
    INLINE constexpr Color32 Color32::LIGHT_GOLDENROD = Color32(250, 250, 210, 255);
    INLINE constexpr Color32 Color32::LIGHT_GRAY = Color32(211, 211, 211, 255);
    INLINE constexpr Color32 Color32::LIGHT_GREEN = Color32(144, 238, 144, 255);
    INLINE constexpr Color32 Color32::LIGHT_PINK = Color32(255, 182, 193, 255);
    INLINE constexpr Color32 Color32::LIGHT_SALMON = Color32(255, 160, 122, 255);
    INLINE constexpr Color32 Color32::LIGHT_SEaGREEN = Color32(32, 178, 170, 255);
    INLINE constexpr Color32 Color32::LIGHT_SKY_BLUE = Color32(135, 206, 250, 255);
    INLINE constexpr Color32 Color32::LIGHT_SLATE_GRAY = Color32(119, 136, 153, 255);
    INLINE constexpr Color32 Color32::LIGHT_STEEL_BLUE = Color32(176, 196, 222, 255);
    INLINE constexpr Color32 Color32::LIGHT_YELLOW = Color32(255, 255, 224, 255);
    INLINE constexpr Color32 Color32::LIME = Color32(0, 255, 0, 255);
    INLINE constexpr Color32 Color32::LIME_GREEN = Color32(50, 205, 50, 255);
    INLINE constexpr Color32 Color32::LINEN = Color32(250, 240, 230, 255);
    INLINE constexpr Color32 Color32::MAGENTA = Color32(255, 0, 255, 255);
    INLINE constexpr Color32 Color32::MAROON = Color32(176, 48, 96, 255);
    INLINE constexpr Color32 Color32::MEDIUM_AQUAMARINE = Color32(102, 205, 170, 255);
    INLINE constexpr Color32 Color32::MEDIUM_BLUE = Color32(0, 0, 205, 255);
    INLINE constexpr Color32 Color32::MEDIUM_ORCHID = Color32(186, 85, 211, 255);
    INLINE constexpr Color32 Color32::MEDIUM_PURPLE = Color32(147, 112, 219, 255);
    INLINE constexpr Color32 Color32::MEDIUM_SEaGREEN = Color32(60, 179, 113, 255);
    INLINE constexpr Color32 Color32::MEDIUM_SLATE_BLUE = Color32(123, 104, 238, 255);
    INLINE constexpr Color32 Color32::MEDIUM_SPRINgGREEN = Color32(0, 250, 154, 255);
    INLINE constexpr Color32 Color32::MEDIUM_TURQUOISE = Color32(72, 209, 204, 255);
    INLINE constexpr Color32 Color32::MEDIUM_VIOLET_RED = Color32(199, 21, 133, 255);
    INLINE constexpr Color32 Color32::MIDNIGHT_BLUE = Color32(25, 25, 112, 255);
    INLINE constexpr Color32 Color32::MINT_CREAM = Color32(245, 255, 250, 255);
    INLINE constexpr Color32 Color32::MISTY_ROSE = Color32(255, 228, 225, 255);
    INLINE constexpr Color32 Color32::MOCCASIN = Color32(255, 228, 181, 255);
    INLINE constexpr Color32 Color32::NAVAJO_WHITE = Color32(255, 222, 173, 255);
    INLINE constexpr Color32 Color32::NAVY_BLUE = Color32(0, 0, 128, 255);
    INLINE constexpr Color32 Color32::OLD_LACE = Color32(253, 245, 230, 255);
    INLINE constexpr Color32 Color32::OLIVE = Color32(128, 128, 0, 255);
    INLINE constexpr Color32 Color32::OLIVE_DRAB = Color32(107, 142, 35, 255);
    INLINE constexpr Color32 Color32::ORANGE = Color32(255, 165, 0, 255);
    INLINE constexpr Color32 Color32::ORANGE_RED = Color32(255, 69, 0, 255);
    INLINE constexpr Color32 Color32::ORCHID = Color32(218, 112, 214, 255);
    INLINE constexpr Color32 Color32::PALE_GOLDENROD = Color32(238, 232, 170, 255);
    INLINE constexpr Color32 Color32::PALE_GREEN = Color32(152, 251, 152, 255);
    INLINE constexpr Color32 Color32::PALE_TURQUOISE = Color32(175, 238, 238, 255);
    INLINE constexpr Color32 Color32::PALE_VIOLET_RED = Color32(219, 112, 147, 255);
    INLINE constexpr Color32 Color32::PAPAYaWHIP = Color32(255, 239, 213, 255);
    INLINE constexpr Color32 Color32::PEACH_PUFF = Color32(255, 218, 185, 255);
    INLINE constexpr Color32 Color32::PERU = Color32(205, 133, 63, 255);
    INLINE constexpr Color32 Color32::PINK = Color32(255, 192, 203, 255);
    INLINE constexpr Color32 Color32::PLUM = Color32(221, 160, 221, 255);
    INLINE constexpr Color32 Color32::POWDErBLUE = Color32(176, 224, 230, 255);
    INLINE constexpr Color32 Color32::PURPLE = Color32(160, 32, 240, 255);
    INLINE constexpr Color32 Color32::REBECCaPURPLE = Color32(102, 51, 153, 255);
    INLINE constexpr Color32 Color32::RED = Color32(255, 0, 0, 255);
    INLINE constexpr Color32 Color32::ROSY_BROWN = Color32(188, 143, 143, 255);
    INLINE constexpr Color32 Color32::ROYAL_BLUE = Color32(65, 105, 225, 255);
    INLINE constexpr Color32 Color32::SADDLE_BROWN = Color32(139, 69, 19, 255);
    INLINE constexpr Color32 Color32::SALMON = Color32(250, 128, 114, 255);
    INLINE constexpr Color32 Color32::SANDY_BROWN = Color32(244, 164, 96, 255);
    INLINE constexpr Color32 Color32::SEaGREEN = Color32(46, 139, 87, 255);
    INLINE constexpr Color32 Color32::SEASHELL = Color32(255, 245, 238, 255);
    INLINE constexpr Color32 Color32::SIENNA = Color32(160, 82, 45, 255);
    INLINE constexpr Color32 Color32::SILVER = Color32(192, 192, 192, 255);
    INLINE constexpr Color32 Color32::SKY_BLUE = Color32(135, 206, 235, 255);
    INLINE constexpr Color32 Color32::SLATE_BLUE = Color32(106, 90, 205, 255);
    INLINE constexpr Color32 Color32::SLATE_GRAY = Color32(112, 128, 144, 255);
    INLINE constexpr Color32 Color32::SNOW = Color32(255, 250, 250, 255);
    INLINE constexpr Color32 Color32::SPRINgGREEN = Color32(0, 255, 127, 255);
    INLINE constexpr Color32 Color32::STEEL_BLUE = Color32(70, 130, 180, 255);
    INLINE constexpr Color32 Color32::TAN = Color32(210, 180, 140, 255);
    INLINE constexpr Color32 Color32::TEAL = Color32(0, 128, 128, 255);
    INLINE constexpr Color32 Color32::THISTLE = Color32(216, 191, 216, 255);
    INLINE constexpr Color32 Color32::TOMATO = Color32(255, 99, 71, 255);
    INLINE constexpr Color32 Color32::TURQUOISE = Color32(64, 224, 208, 255);
    INLINE constexpr Color32 Color32::VIOLET = Color32(238, 130, 238, 255);
    INLINE constexpr Color32 Color32::WEbGRAY = Color32(128, 128, 128, 255);
    INLINE constexpr Color32 Color32::WEbGREEN = Color32(0, 128, 0, 255);
    INLINE constexpr Color32 Color32::WEbMAROON = Color32(128, 0, 0, 255);
    INLINE constexpr Color32 Color32::WEbPURPLE = Color32(128, 0, 128, 255);
    INLINE constexpr Color32 Color32::WHEAT = Color32(245, 222, 179, 255);
    INLINE constexpr Color32 Color32::WHITE = Color32(255, 255, 255, 255);
    INLINE constexpr Color32 Color32::WHITE_SMOKE = Color32(245, 245, 245, 255);
    INLINE constexpr Color32 Color32::YELLOW = Color32(255, 255, 0, 255);
    INLINE constexpr Color32 Color32::YELLOW_GREEN = Color32(154, 205, 50, 255);

}  // namespace ho
