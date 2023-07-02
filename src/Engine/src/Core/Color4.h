//
// Created by alexl on 13.05.2023.
//

#pragma once
#if __has_include(<vec4.hpp>)
#include <vec4.hpp>
#endif
#if __has_include(<gsl/gsl>)
#include <gsl/gsl>
#endif
#if __has_include(<imgui.h>)
#include <imgui.h>
#endif

#if defined(BEE_COMPILE_WEBGPU)
#include <webgpu/webgpu.h>
#endif

namespace BeeEngine
{
    struct Color4
    {
        constexpr Color4() noexcept : m_R(0.0f), m_G(0.0f), m_B(0.0f), m_A(1.0f) {}
        constexpr static Color4 FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept
        {
#if __has_include(<gsl/gsl>)
            return {gsl::narrow_cast<float>(r) / 255.0f,
                    gsl::narrow_cast<float>(g) / 255.0f,
                    gsl::narrow_cast<float>(b) / 255.0f,
                    gsl::narrow_cast<float>(a) / 255.0f};
#endif
            return {static_cast<float>(r) / 255.0f,
                    static_cast<float>(g) / 255.0f,
                    static_cast<float>(b) / 255.0f,
                    static_cast<float>(a) / 255.0f};
        }
        constexpr static Color4 FromNormalized(float r, float g, float b, float a = 1.0f) noexcept
        {
            return {r, g, b, a};
        }

        constexpr Color4(Color4&& other) noexcept = default;
        constexpr Color4(const Color4& other) = default;
        constexpr Color4& operator=(Color4&& other) noexcept = default;
        constexpr Color4& operator=(const Color4& other) = default;
        constexpr ~Color4() noexcept = default;

        [[nodiscard]] inline constexpr float R() const { return m_R; }
        [[nodiscard]] inline constexpr float G() const { return m_G; }
        [[nodiscard]] inline constexpr float B() const { return m_B; }
        [[nodiscard]] inline constexpr float A() const { return m_A; }

        [[nodiscard]] inline constexpr float* ValuePtr() const { return (float*)&m_R; }

#if __has_include(<vec4.hpp>)
        constexpr inline explicit operator glm::vec4() const
        {
            return {m_R, m_G, m_B, m_A};
        }
#endif

#if defined(BEE_COMPILE_WEBGPU)
        constexpr inline operator WGPUColor() const
        {
            return {m_R, m_G, m_B, m_A};
        }
#endif
#if __has_include(<imgui.h>)
        constexpr inline operator ImVec4() const
        {
            return {m_R, m_G, m_B, m_A};
        }
        constexpr inline operator const ImVec4() const
        {
            return {m_R, m_G, m_B, m_A};
        }
#endif
        constexpr bool operator == (const Color4& other) const
        {
            return m_R == other.m_R && m_G == other.m_G && m_B == other.m_B && m_A == other.m_A;
        }
        constexpr bool operator != (const Color4& other) const
        {
            return !(*this == other);
        }

        struct Color4Init
        {
            float R, G, B, A;
            constexpr operator Color4() const
            {
                return {R, G, B, A};
            }
#if __has_include(<imgui.h>)
            constexpr operator ImVec4() const
            {
                return {R, G, B, A};
            }
            constexpr operator const ImVec4() const
            {
                return {R, G, B, A};
            }
#endif
        };

        static constexpr Color4Init const White = {1.0f, 1.0f, 1.0f, 1.0f};
        static constexpr Color4Init const Black = {0.0f, 0.0f, 0.0f, 1.0f};
        static constexpr Color4Init const Transparent = {0.0f, 0.0f, 0.0f, 0.0f};
        static constexpr Color4Init const AliceBlue = {0.9411765f, 0.972549f, 1.0f, 1.0f};
        static constexpr Color4Init const AntiqueWhite = {0.98039216f, 0.92156863f, 0.84313726f, 1.0f};
        static constexpr Color4Init const Aqua = {0.0f, 1.0f, 1.0f, 1.0f};
        static constexpr Color4Init const Aquamarine = {0.49803922f, 1.0f, 0.83137256f, 1.0f};
        static constexpr Color4Init const Azure = {0.9411765f, 1.0f, 1.0f, 1.0f};
        static constexpr Color4Init const Beige = {0.9607843f, 0.9607843f, 0.8627451f, 1.0f};
        static constexpr Color4Init const Bisque = {1.0f, 0.89411765f, 0.76862746f, 1.0f};
        static constexpr Color4Init const BlanchedAlmond = {1.0f, 0.92156863f, 0.8039216f, 1.0f};
        static constexpr Color4Init const Blue = {0.0f, 0.0f, 1.0f, 1.0f};
        static constexpr Color4Init const BlueViolet = {0.5411765f, 0.16862746f, 0.8862745f, 1.0f};
        static constexpr Color4Init const Brown = {0.64705884f, 0.16470589f, 0.16470589f, 1.0f};
        static constexpr Color4Init const BurlyWood = {0.87058824f, 0.72156864f, 0.5294118f, 1.0f};
        static constexpr Color4Init const CadetBlue = {0.37254903f, 0.61960787f, 0.627451f, 1.0f};
        static constexpr Color4Init const Chartreuse = {0.49803922f, 1.0f, 0.0f, 1.0f};
        static constexpr Color4Init const Chocolate = {0.8235294f, 0.4117647f, 0.11764706f, 1.0f};
        static constexpr Color4Init const Coral = {1.0f, 0.49803922f, 0.3137255f, 1.0f};
        static constexpr Color4Init const CornflowerBlue = {0.39215687f, 0.58431375f, 0.92941177f, 1.0f};
        static constexpr Color4Init const Cornsilk = {1.0f, 0.972549f, 0.8627451f, 1.0f};
        static constexpr Color4Init const Crimson = {0.8627451f, 0.078431375f, 0.23529412f, 1.0f};
        static constexpr Color4Init const Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
        static constexpr Color4Init const DarkBlue = {0.0f, 0.0f, 0.54509807f, 1.0f};
        static constexpr Color4Init const DarkCyan = {0.0f, 0.54509807f, 0.54509807f, 1.0f};
        static constexpr Color4Init const DarkGoldenrod = {0.72156864f, 0.5254902f, 0.043137256f, 1.0f};
        static constexpr Color4Init const DarkGray = {0.6627451f, 0.6627451f, 0.6627451f, 1.0f};
        static constexpr Color4Init const DarkGreen = {0.0f, 0.39215687f, 0.0f, 1.0f};
        static constexpr Color4Init const DarkKhaki = {0.7411765f, 0.7176471f, 0.41960785f, 1.0f};
        static constexpr Color4Init const DarkMagenta = {0.54509807f, 0.0f, 0.54509807f, 1.0f};
        static constexpr Color4Init const DarkOliveGreen = {0.33333334f, 0.41960785f, 0.18431373f, 1.0f};
        static constexpr Color4Init const DarkOrange = {1.0f, 0.54901963f, 0.0f, 1.0f};
        static constexpr Color4Init const DarkOrchid = {0.6f, 0.19607843f, 0.8f, 1.0f};
        static constexpr Color4Init const DarkRed = {0.54509807f, 0.0f, 0.0f, 1.0f};
        static constexpr Color4Init const DarkSalmon = {0.9137255f, 0.5882353f, 0.47843137f, 1.0f};
        static constexpr Color4Init const DarkSeaGreen = {0.56078434f, 0.7372549f, 0.56078434f, 1.0f};
        static constexpr Color4Init const DarkSlateBlue = {0.28235295f, 0.23921569f, 0.54509807f, 1.0f};
        static constexpr Color4Init const DarkSlateGray = {0.18431373f, 0.30980393f, 0.30980393f, 1.0f};
        static constexpr Color4Init const DarkTurquoise = {0.0f, 0.80784315f, 0.81960785f, 1.0f};
        static constexpr Color4Init const DarkViolet = {0.5803922f, 0.0f, 0.827451f, 1.0f};
        static constexpr Color4Init const DeepPink = {1.0f, 0.078431375f, 0.5764706f, 1.0f};
        static constexpr Color4Init const DeepSkyBlue = {0.0f, 0.7490196f, 1.0f, 1.0f};
        static constexpr Color4Init const DimGray = {0.4117647f, 0.4117647f, 0.4117647f, 1.0f};
        static constexpr Color4Init const DodgerBlue = {0.11764706f, 0.5647059f, 1.0f, 1.0f};
        static constexpr Color4Init const Firebrick = {0.69803923f, 0.13333334f, 0.13333334f, 1.0f};
        static constexpr Color4Init const FloralWhite = {1.0f, 0.98039216f, 0.9411765f, 1.0f};
        static constexpr Color4Init const ForestGreen = {0.13333334f, 0.54509807f, 0.13333334f, 1.0f};
        static constexpr Color4Init const Fuchsia = {1.0f, 0.0f, 1.0f, 1.0f};
        static constexpr Color4Init const Gainsboro = {0.8627451f, 0.8627451f, 0.8627451f, 1.0f};
        static constexpr Color4Init const GhostWhite = {0.972549f, 0.972549f, 1.0f, 1.0f};
        static constexpr Color4Init const Gold = {1.0f, 0.84313726f, 0.0f, 1.0f};
        static constexpr Color4Init const Goldenrod = {0.85490197f, 0.64705884f, 0.1254902f, 1.0f};
        static constexpr Color4Init const Gray = {0.5019608f, 0.5019608f, 0.5019608f, 1.0f};
        static constexpr Color4Init const Green = {0.0f, 0.5019608f, 0.0f, 1.0f};
        static constexpr Color4Init const GreenYellow = {0.6784314f, 1.0f, 0.18431373f, 1.0f};
        static constexpr Color4Init const Honeydew = {0.9411765f, 1.0f, 0.9411765f, 1.0f};
        static constexpr Color4Init const HotPink = {1.0f, 0.4117647f, 0.7058824f, 1.0f};
        static constexpr Color4Init const IndianRed = {0.8039216f, 0.36078432f, 0.36078432f, 1.0f};
        static constexpr Color4Init const Indigo = {0.29411766f, 0.0f, 0.50980395f, 1.0f};
        static constexpr Color4Init const Ivory = {1.0f, 1.0f, 0.9411765f, 1.0f};
        static constexpr Color4Init const Khaki = {0.9411765f, 0.9019608f, 0.54901963f, 1.0f};
        static constexpr Color4Init const Lavender = {0.9019608f, 0.9019608f, 0.98039216f, 1.0f};
        static constexpr Color4Init const LavenderBlush = {1.0f, 0.9411765f, 0.9607843f, 1.0f};
        static constexpr Color4Init const LawnGreen = {0.4862745f, 0.9882353f, 0.0f, 1.0f};
        static constexpr Color4Init const LemonChiffon = {1.0f, 0.98039216f, 0.8039216f, 1.0f};
        static constexpr Color4Init const LightBlue = {0.6784314f, 0.84705883f, 0.9019608f, 1.0f};
        static constexpr Color4Init const LightCoral = {0.9411765f, 0.5019608f, 0.5019608f, 1.0f};
        static constexpr Color4Init const LightCyan = {0.8784314f, 1.0f, 1.0f, 1.0f};
        static constexpr Color4Init const LightGoldenrodYellow = {0.98039216f, 0.98039216f, 0.8235294f, 1.0f};
        static constexpr Color4Init const LightGray = {0.827451f, 0.827451f, 0.827451f, 1.0f};
        static constexpr Color4Init const LightGreen = {0.5647059f, 0.93333334f, 0.5647059f, 1.0f};
        static constexpr Color4Init const LightPink = {1.0f, 0.7137255f, 0.75686276f, 1.0f};
        static constexpr Color4Init const LightSalmon = {1.0f, 0.627451f, 0.47843137f, 1.0f};
        static constexpr Color4Init const LightSeaGreen = {0.1254902f, 0.69803923f, 0.6666667f, 1.0f};
        static constexpr Color4Init const LightSkyBlue = {0.5294118f, 0.80784315f, 0.98039216f, 1.0f};
        static constexpr Color4Init const LightSlateGray = {0.46666667f, 0.53333336f, 0.6f, 1.0f};
        static constexpr Color4Init const LightSteelBlue = {0.6901961f, 0.76862746f, 0.87058824f, 1.0f};
        static constexpr Color4Init const LightYellow = {1.0f, 1.0f, 0.8784314f, 1.0f};
        static constexpr Color4Init const Lime = {0.0f, 1.0f, 0.0f, 1.0f};
        static constexpr Color4Init const LimeGreen = {0.19607843f, 0.8039216f, 0.19607843f, 1.0f};
        static constexpr Color4Init const Linen = {0.98039216f, 0.9411765f, 0.9019608f, 1.0f};
        static constexpr Color4Init const Magenta = {1.0f, 0.0f, 1.0f, 1.0f};
        static constexpr Color4Init const Maroon = {0.5019608f, 0.0f, 0.0f, 1.0f};
        static constexpr Color4Init const MediumAquamarine = {0.4f, 0.8039216f, 0.6666667f, 1.0f};
        static constexpr Color4Init const MediumBlue = {0.0f, 0.0f, 0.8039216f, 1.0f};
        static constexpr Color4Init const MediumOrchid = {0.7294118f, 0.33333334f, 0.827451f, 1.0f};
        static constexpr Color4Init const MediumPurple = {0.5764706f, 0.4392157f, 0.85882354f, 1.0f};
        static constexpr Color4Init const MediumSeaGreen = {0.23529412f, 0.7019608f, 0.44313726f, 1.0f};
        static constexpr Color4Init const MediumSlateBlue = {0.48235294f, 0.40784314f, 0.93333334f, 1.0f};
        static constexpr Color4Init const MediumSpringGreen = {0.0f, 0.98039216f, 0.6039216f, 1.0f};
        static constexpr Color4Init const MediumTurquoise = {0.28235295f, 0.81960785f, 0.8f, 1.0f};
        static constexpr Color4Init const MediumVioletRed = {0.78039217f, 0.08235294f, 0.52156866f, 1.0f};
        static constexpr Color4Init const MidnightBlue = {0.09803922f, 0.09803922f, 0.4392157f, 1.0f};
        static constexpr Color4Init const MintCream = {0.9607843f, 1.0f, 0.98039216f, 1.0f};
        static constexpr Color4Init const MistyRose = {1.0f, 0.89411765f, 0.88235295f, 1.0f};
        static constexpr Color4Init const Moccasin = {1.0f, 0.89411765f, 0.70980394f, 1.0f};
        static constexpr Color4Init const NavajoWhite = {1.0f, 0.87058824f, 0.6784314f, 1.0f};
        static constexpr Color4Init const Navy = {0.0f, 0.0f, 0.5019608f, 1.0f};
        static constexpr Color4Init const OldLace = {0.99215686f, 0.9607843f, 0.9019608f, 1.0f};
        static constexpr Color4Init const Olive = {0.5019608f, 0.5019608f, 0.0f, 1.0f};
        static constexpr Color4Init const OliveDrab = {0.41960785f, 0.5568628f, 0.13725491f, 1.0f};
        static constexpr Color4Init const Orange = {1.0f, 0.64705884f, 0.0f, 1.0f};
        static constexpr Color4Init const OrangeRed = {1.0f, 0.27058825f, 0.0f, 1.0f};
        static constexpr Color4Init const Orchid = {0.85490197f, 0.4392157f, 0.8392157f, 1.0f};
        static constexpr Color4Init const PaleGoldenrod = {0.93333334f, 0.9098039f, 0.6666667f, 1.0f};
        static constexpr Color4Init const PaleGreen = {0.59607846f, 0.9843137f, 0.59607846f, 1.0f};
        static constexpr Color4Init const PaleTurquoise = {0.6862745f, 0.93333334f, 0.93333334f, 1.0f};
        static constexpr Color4Init const PaleVioletRed = {0.85882354f, 0.4392157f, 0.5764706f, 1.0f};
        static constexpr Color4Init const PapayaWhip = {1.0f, 0.9372549f, 0.8352941f, 1.0f};
        static constexpr Color4Init const PeachPuff = {1.0f, 0.85490197f, 0.7254902f, 1.0f};
        static constexpr Color4Init const Peru = {0.8039216f, 0.52156866f, 0.24705882f, 1.0f};
        static constexpr Color4Init const Pink = {1.0f, 0.7529412f, 0.79607844f, 1.0f};
        static constexpr Color4Init const Plum = {0.8666667f, 0.627451f, 0.8666667f, 1.0f};
        static constexpr Color4Init const PowderBlue = {0.6901961f, 0.8784314f, 0.9019608f, 1.0f};
        static constexpr Color4Init const Purple = {0.5019608f, 0.0f, 0.5019608f, 1.0f};
        static constexpr Color4Init const Red = {1.0f, 0.0f, 0.0f, 1.0f};
        static constexpr Color4Init const RosyBrown = {0.7372549f, 0.56078434f, 0.56078434f, 1.0f};
        static constexpr Color4Init const RoyalBlue = {0.25490198f, 0.4117647f, 0.88235295f, 1.0f};
        static constexpr Color4Init const SaddleBrown = {0.54509807f, 0.27058825f, 0.07450981f, 1.0f};
        static constexpr Color4Init const Salmon = {0.98039216f, 0.5019608f, 0.44705883f, 1.0f};
        static constexpr Color4Init const SandyBrown = {0.95686275f, 0.6431373f, 0.3764706f, 1.0f};
        static constexpr Color4Init const SeaGreen = {0.18039216f, 0.54509807f, 0.34117648f, 1.0f};
        static constexpr Color4Init const SeaShell = {1.0f, 0.9607843f, 0.93333334f, 1.0f};
        static constexpr Color4Init const Sienna = {0.627451f, 0.32156864f, 0.1764706f, 1.0f};
        static constexpr Color4Init const Silver = {0.7529412f, 0.7529412f, 0.7529412f, 1.0f};
        static constexpr Color4Init const SkyBlue = {0.5294118f, 0.80784315f, 0.92156863f, 1.0f};
        static constexpr Color4Init const SlateBlue = {0.41568628f, 0.3529412f, 0.8039216f, 1.0f};
        static constexpr Color4Init const SlateGray = {0.4392157f, 0.5019608f, 0.5647059f, 1.0f};
        static constexpr Color4Init const Snow = {1.0f, 0.98039216f, 0.98039216f, 1.0f};
        static constexpr Color4Init const SpringGreen = {0.0f, 1.0f, 0.49803922f, 1.0f};
        static constexpr Color4Init const SteelBlue = {0.27450982f, 0.50980395f, 0.7058824f, 1.0f};
        static constexpr Color4Init const Tan = {0.8235294f, 0.7058824f, 0.54901963f, 1.0f};
        static constexpr Color4Init const Teal = {0.0f, 0.5019608f, 0.5019608f, 1.0f};
        static constexpr Color4Init const Thistle = {0.84705883f, 0.7490196f, 0.84705883f, 1.0f};
        static constexpr Color4Init const Tomato = {1.0f, 0.3882353f, 0.2784314f, 1.0f};
        static constexpr Color4Init const Turquoise = {0.2509804f, 0.8784314f, 0.8156863f, 1.0f};
        static constexpr Color4Init const Violet = {0.93333334f, 0.50980395f, 0.93333334f, 1.0f};
        static constexpr Color4Init const Wheat = {0.9607843f, 0.87058824f, 0.7019608f, 1.0f};
        static constexpr Color4Init const Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
        static constexpr Color4Init const YellowGreen = {0.6039216f, 0.8039216f, 0.19607843f, 1.0f};

    private:
        float m_R;
        float m_G;
        float m_B;
        float m_A;
        constexpr Color4(float r, float g, float b, float a = 1.0f)
                : m_R(r), m_G(g), m_B(b), m_A(a)
        {}
    };
}
