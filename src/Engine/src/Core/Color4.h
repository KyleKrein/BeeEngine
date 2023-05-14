//
// Created by alexl on 13.05.2023.
//

#pragma once
#include "TypeDefines.h"
#include "vec4.hpp"

namespace BeeEngine
{
    struct Color4
    {
        Color4() : m_R(0.0f), m_G(0.0f), m_B(0.0f), m_A(1.0f) {}
        static Color4 FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
        }
        static Color4 FromNormalized(float r, float g, float b, float a = 1.0f)
        {
            return {r, g, b, a};
        }

        inline const float R() const { return m_R; }
        inline const float G() const { return m_G; }
        inline const float B() const { return m_B; }
        inline const float A() const { return m_A; }

        inline explicit operator glm::vec4() const
        {
            return {m_R, m_G, m_B, m_A};
        }
        bool operator == (const Color4& other) const
        {
            return m_R == other.m_R && m_G == other.m_G && m_B == other.m_B && m_A == other.m_A;
        }
        bool operator != (const Color4& other) const
        {
            return !(*this == other);
        }

        static const Color4 White;
        static const Color4 Black;
        static const Color4 Transparent;
        static const Color4 AliceBlue;
        static const Color4 AntiqueWhite;
        static const Color4 Aqua;
        static const Color4 Aquamarine;
        static const Color4 Azure;
        static const Color4 Beige;
        static const Color4 Bisque;
        static const Color4 BlanchedAlmond;
        static const Color4 Blue;
        static const Color4 BlueViolet;
        static const Color4 Brown;
        static const Color4 BurlyWood;
        static const Color4 CadetBlue;
        static const Color4 Chartreuse;
        static const Color4 Chocolate;
        static const Color4 Coral;
        static const Color4 CornflowerBlue;
        static const Color4 Cornsilk;
        static const Color4 Crimson;
        static const Color4 Cyan;
        static const Color4 DarkBlue;
        static const Color4 DarkCyan;
        static const Color4 DarkGoldenrod;
        static const Color4 DarkGray;
        static const Color4 DarkGreen;
        static const Color4 DarkKhaki;
        static const Color4 DarkMagenta;
        static const Color4 DarkOliveGreen;
        static const Color4 DarkOrange;
        static const Color4 DarkOrchid;
        static const Color4 DarkRed;
        static const Color4 DarkSalmon;
        static const Color4 DarkSeaGreen;
        static const Color4 DarkSlateBlue;
        static const Color4 DarkSlateGray;
        static const Color4 DarkTurquoise;
        static const Color4 DarkViolet;
        static const Color4 DeepPink;
        static const Color4 DeepSkyBlue;
        static const Color4 DimGray;
        static const Color4 DodgerBlue;
        static const Color4 Firebrick;
        static const Color4 FloralWhite;
        static const Color4 ForestGreen;
        static const Color4 Fuchsia;
        static const Color4 Gainsboro;
        static const Color4 GhostWhite;
        static const Color4 Gold;
        static const Color4 Goldenrod;
        static const Color4 Gray;
        static const Color4 Green;
        static const Color4 GreenYellow;
        static const Color4 Honeydew;
        static const Color4 HotPink;
        static const Color4 IndianRed;
        static const Color4 Indigo;
        static const Color4 Ivory;
        static const Color4 Khaki;
        static const Color4 Lavender;
        static const Color4 LavenderBlush;
        static const Color4 LawnGreen;
        static const Color4 LemonChiffon;
        static const Color4 LightBlue;
        static const Color4 LightCoral;
        static const Color4 LightCyan;
        static const Color4 LightGoldenrodYellow;
        static const Color4 LightGray;
        static const Color4 LightGreen;
        static const Color4 LightPink;
        static const Color4 LightSalmon;
        static const Color4 LightSeaGreen;
        static const Color4 LightSkyBlue;
        static const Color4 LightSlateGray;
        static const Color4 LightSteelBlue;
        static const Color4 LightYellow;
        static const Color4 Lime;
        static const Color4 LimeGreen;
        static const Color4 Linen;
        static const Color4 Magenta;
        static const Color4 Maroon;
        static const Color4 MediumAquamarine;
        static const Color4 MediumBlue;
        static const Color4 MediumOrchid;
        static const Color4 MediumPurple;
        static const Color4 MediumSeaGreen;
        static const Color4 MediumSlateBlue;
        static const Color4 MediumSpringGreen;
        static const Color4 MediumTurquoise;
        static const Color4 MediumVioletRed;
        static const Color4 MidnightBlue;
        static const Color4 MintCream;
        static const Color4 MistyRose;
        static const Color4 Moccasin;
        static const Color4 NavajoWhite;
        static const Color4 Navy;
        static const Color4 OldLace;
        static const Color4 Olive;
        static const Color4 OliveDrab;
        static const Color4 Orange;
        static const Color4 OrangeRed;
        static const Color4 Orchid;
        static const Color4 PaleGoldenrod;
        static const Color4 PaleGreen;
        static const Color4 PaleTurquoise;
        static const Color4 PaleVioletRed;
        static const Color4 PapayaWhip;
        static const Color4 PeachPuff;
        static const Color4 Peru;
        static const Color4 Pink;
        static const Color4 Plum;
        static const Color4 PowderBlue;
        static const Color4 Purple;
        static const Color4 Red;
        static const Color4 RosyBrown;
        static const Color4 RoyalBlue;
        static const Color4 SaddleBrown;
        static const Color4 Salmon;
        static const Color4 SandyBrown;
        static const Color4 SeaGreen;
        static const Color4 SeaShell;
        static const Color4 Sienna;
        static const Color4 Silver;
        static const Color4 SkyBlue;
        static const Color4 SlateBlue;
        static const Color4 SlateGray;
        static const Color4 Snow;
        static const Color4 SpringGreen;
        static const Color4 SteelBlue;
        static const Color4 Tan;
        static const Color4 Teal;
        static const Color4 Thistle;
        static const Color4 Tomato;
        static const Color4 Turquoise;
        static const Color4 Violet;
        static const Color4 Wheat;
        static const Color4 Yellow;
        static const Color4 YellowGreen;

    private:
        float m_R;
        float m_G;
        float m_B;
        float m_A;
        Color4(float r, float g, float b, float a = 1.0f)
                : m_R(r), m_G(g), m_B(b), m_A(a)
        {}
    };
}
