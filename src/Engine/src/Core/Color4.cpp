//
// Created by alexl on 13.05.2023.
//

#include "Color4.h"


namespace BeeEngine
{
    const Color4 Color4::White = {1.0f, 1.0f, 1.0f, 1.0f};
    const Color4 Color4::Black = {0.0f, 0.0f, 0.0f, 1.0f};
    const Color4 Color4::Transparent = {0.0f, 0.0f, 0.0f, 0.0f};
    const Color4 Color4::AliceBlue = {0.9411765f, 0.972549f, 1.0f, 1.0f};
    const Color4 Color4::AntiqueWhite = {0.98039216f, 0.92156863f, 0.84313726f, 1.0f};
    const Color4 Color4::Aqua = {0.0f, 1.0f, 1.0f, 1.0f};
    const Color4 Color4::Aquamarine = {0.49803922f, 1.0f, 0.83137256f, 1.0f};
    const Color4 Color4::Azure = {0.9411765f, 1.0f, 1.0f, 1.0f};
    const Color4 Color4::Beige = {0.9607843f, 0.9607843f, 0.8627451f, 1.0f};
    const Color4 Color4::Bisque = {1.0f, 0.89411765f, 0.76862746f, 1.0f};
    const Color4 Color4::BlanchedAlmond = {1.0f, 0.92156863f, 0.8039216f, 1.0f};
    const Color4 Color4::Blue = {0.0f, 0.0f, 1.0f, 1.0f};
    const Color4 Color4::BlueViolet = {0.5411765f, 0.16862746f, 0.8862745f, 1.0f};
    const Color4 Color4::Brown =  Color4::FromRGBA(162, 42, 42, 255);
    const Color4 Color4::BurlyWood = {0.87058824f, 0.72156864f, 0.5294118f, 1.0f};
    const Color4 Color4::CadetBlue = {0.37254903f, 0.61960787f, 0.627451f, 1.0f};
    const Color4 Color4::Chartreuse = {0.49803922f, 1.0f, 0.0f, 1.0f};
    const Color4 Color4::Chocolate = {0.8235294f, 0.4117647f, 0.11764706f, 1.0f};
    const Color4 Color4::Coral = {1.0f, 0.49803922f, 0.3137255f, 1.0f};
    const Color4 Color4::CornflowerBlue = {0.39215687f, 0.58431375f, 0.92941177f, 1.0f};
    const Color4 Color4::Cornsilk = {1.0f, 0.972549f, 0.8627451f, 1.0f};
    const Color4 Color4::Crimson = {0.8627451f, 0.078431375f, 0.23529412f, 1.0f};
    const Color4 Color4::Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
    const Color4 Color4::DarkBlue = {0.0f, 0.0f, 0.54509807f, 1.0f};
    const Color4 Color4::DarkCyan = {0.0f, 0.54509807f, 0.54509807f, 1.0f};
    const Color4 Color4::DarkGoldenrod = {0.72156864f, 0.5254902f, 0.043137256f, 1.0f};
    const Color4 Color4::DarkGray = {0.6627451f, 0.6627451f, 0.6627451f, 1.0f};
    const Color4 Color4::DarkGreen = {0.0f, 0.39215687f, 0.0f, 1.0f};
    const Color4 Color4::DarkKhaki = {0.7411765f, 0.7176471f, 0.41960785f, 1.0f};
    const Color4 Color4::DarkMagenta = {0.54509807f, 0.0f, 0.54509807f, 1.0f};
    const Color4 Color4::DarkOliveGreen = {0.33333334f, 0.41960785f, 0.18431373f, 1.0f};
    const Color4 Color4::DarkOrange = {1.0f, 0.54901963f, 0.0f, 1.0f};
    const Color4 Color4::DarkOrchid = {0.6f, 0.19607843f, 0.8f, 1.0f};
    const Color4 Color4::DarkRed = {0.54509807f, 0.0f, 0.0f, 1.0f};
    const Color4 Color4::DarkSalmon = {0.9137255f, 0.5882353f, 0.47843137f, 1.0f};
    const Color4 Color4::DarkSeaGreen = {0.56078434f, 0.7372549f, 0.56078434f, 1.0f};
    const Color4 Color4::DarkSlateBlue = {0.28235295f, 0.23921569f, 0.54509807f, 1.0f};
    const Color4 Color4::DarkSlateGray = {0.18431373f, 0.30980393f, 0.30980393f, 1.0f};
    const Color4 Color4::DarkTurquoise = {0.0f, 0.80784315f, 0.81960785f, 1.0f};
    const Color4 Color4::DarkViolet = {0.5803922f, 0.0f, 0.827451f, 1.0f};
    const Color4 Color4::DeepPink = {1.0f, 0.078431375f, 0.5764706f, 1.0f};
    const Color4 Color4::DeepSkyBlue = {0.0f, 0.7490196f, 1.0f, 1.0f};
    const Color4 Color4::DimGray = {0.4117647f, 0.4117647f, 0.4117647f, 1.0f};
    const Color4 Color4::DodgerBlue = {0.11764706f, 0.5647059f, 1.0f, 1.0f};
    const Color4 Color4::Firebrick = {0.69803923f, 0.13333334f, 0.13333334f, 1.0f};
    const Color4 Color4::FloralWhite = {1.0f, 0.98039216f, 0.9411765f, 1.0f};
    const Color4 Color4::ForestGreen = {0.13333334f, 0.54509807f, 0.13333334f, 1.0f};
    const Color4 Color4::Fuchsia = {1.0f, 0.0f, 1.0f, 1.0f};
    const Color4 Color4::Gainsboro = {0.8627451f, 0.8627451f, 0.8627451f, 1.0f};
    const Color4 Color4::GhostWhite = {0.972549f, 0.972549f, 1.0f, 1.0f};
    const Color4 Color4::Gold = {1.0f, 0.84313726f, 0.0f, 1.0f};
    const Color4 Color4::Goldenrod = {0.85490197f, 0.64705884f, 0.1254902f, 1.0f};
    const Color4 Color4::Gray = {0.5019608f, 0.5019608f, 0.5019608f, 1.0f};
    const Color4 Color4::Green = {0.0f, 0.5019608f, 0.0f, 1.0f};
    const Color4 Color4::GreenYellow = {0.6784314f, 1.0f, 0.18431373f, 1.0f};
    const Color4 Color4::Honeydew = {0.9411765f, 1.0f, 0.9411765f, 1.0f};
    const Color4 Color4::HotPink = {1.0f, 0.4117647f, 0.7058824f, 1.0f};
    const Color4 Color4::IndianRed = {0.8039216f, 0.36078432f, 0.36078432f, 1.0f};
    const Color4 Color4::Indigo = {0.29411766f, 0.0f, 0.50980395f, 1.0f};
    const Color4 Color4::Ivory = {1.0f, 1.0f, 0.9411765f, 1.0f};
    const Color4 Color4::Khaki = {0.9411765f, 0.9019608f, 0.54901963f, 1.0f};
    const Color4 Color4::Lavender = {0.9019608f, 0.9019608f, 0.98039216f, 1.0f};
    const Color4 Color4::LavenderBlush = {1.0f, 0.9411765f, 0.9607843f, 1.0f};
    const Color4 Color4::LawnGreen = {0.4862745f, 0.9882353f, 0.0f, 1.0f};
    const Color4 Color4::LemonChiffon = {1.0f, 0.98039216f, 0.8039216f, 1.0f};
    const Color4 Color4::LightBlue = {0.6784314f, 0.84705883f, 0.9019608f, 1.0f};
    const Color4 Color4::LightCoral = {0.9411765f, 0.5019608f, 0.5019608f, 1.0f};
    const Color4 Color4::LightCyan = {0.8784314f, 1.0f, 1.0f, 1.0f};
    const Color4 Color4::LightGoldenrodYellow = {0.98039216f, 0.98039216f, 0.8235294f, 1.0f};
    const Color4 Color4::LightGray = {0.827451f, 0.827451f, 0.827451f, 1.0f};
    const Color4 Color4::LightGreen = {0.5647059f, 0.93333334f, 0.5647059f, 1.0f};
    const Color4 Color4::LightPink = {1.0f, 0.7137255f, 0.75686276f, 1.0f};
    const Color4 Color4::LightSalmon = {1.0f, 0.627451f, 0.47843137f, 1.0f};
    const Color4 Color4::LightSeaGreen = {0.1254902f, 0.69803923f, 0.6666667f, 1.0f};
    const Color4 Color4::LightSkyBlue = {0.5294118f, 0.80784315f, 0.98039216f, 1.0f};
    const Color4 Color4::LightSlateGray = {0.46666667f, 0.53333336f, 0.6f, 1.0f};
    const Color4 Color4::LightSteelBlue = {0.6901961f, 0.76862746f, 0.87058824f, 1.0f};
    const Color4 Color4::LightYellow = {1.0f, 1.0f, 0.8784314f, 1.0f};
    const Color4 Color4::Lime = {0.0f, 1.0f, 0.0f, 1.0f};
    const Color4 Color4::LimeGreen = {0.19607843f, 0.8039216f, 0.19607843f, 1.0f};
    const Color4 Color4::Linen = {0.98039216f, 0.9411765f, 0.9019608f, 1.0f};
    const Color4 Color4::Magenta = {1.0f, 0.0f, 1.0f, 1.0f};
    const Color4 Color4::Maroon = {0.5019608f, 0.0f, 0.0f, 1.0f};
    const Color4 Color4::MediumAquamarine = {0.4f, 0.8039216f, 0.6666667f, 1.0f};
    const Color4 Color4::MediumBlue = {0.0f, 0.0f, 0.8039216f, 1.0f};
    const Color4 Color4::MediumOrchid = {0.7294118f, 0.33333334f, 0.827451f, 1.0f};
    const Color4 Color4::MediumPurple = {0.5764706f, 0.4392157f, 0.85882354f, 1.0f};
    const Color4 Color4::MediumSeaGreen = {0.23529412f, 0.7019608f, 0.44313726f, 1.0f};
    const Color4 Color4::MediumSlateBlue = {0.48235294f, 0.40784314f, 0.93333334f, 1.0f};
    const Color4 Color4::MediumSpringGreen = {0.0f, 0.98039216f, 0.6039216f, 1.0f};
    const Color4 Color4::MediumTurquoise = {0.28235295f, 0.81960785f, 0.8f, 1.0f};
    const Color4 Color4::MediumVioletRed = {0.78039217f, 0.08235294f, 0.52156866f, 1.0f};
    const Color4 Color4::MidnightBlue = {0.09803922f, 0.09803922f, 0.4392157f, 1.0f};
    const Color4 Color4::MintCream = {0.9607843f, 1.0f, 0.98039216f, 1.0f};
    const Color4 Color4::MistyRose = {1.0f, 0.89411765f, 0.88235295f, 1.0f};
    const Color4 Color4::Moccasin = {1.0f, 0.89411765f, 0.70980394f, 1.0f};
    const Color4 Color4::NavajoWhite = {1.0f, 0.87058824f, 0.6784314f, 1.0f};
    const Color4 Color4::Navy = {0.0f, 0.0f, 0.5019608f, 1.0f};
    const Color4 Color4::OldLace = {0.99215686f, 0.9607843f, 0.9019608f, 1.0f};
    const Color4 Color4::Olive = {0.5019608f, 0.5019608f, 0.0f, 1.0f};
    const Color4 Color4::OliveDrab = {0.41960785f, 0.5568628f, 0.13725491f, 1.0f};
    const Color4 Color4::Orange = {1.0f, 0.64705884f, 0.0f, 1.0f};
    const Color4 Color4::OrangeRed = {1.0f, 0.27058825f, 0.0f, 1.0f};
    const Color4 Color4::Orchid = {0.85490197f, 0.4392157f, 0.8392157f, 1.0f};
    const Color4 Color4::PaleGoldenrod = {0.93333334f, 0.9098039f, 0.6666667f, 1.0f};
    const Color4 Color4::PaleGreen = {0.59607846f, 0.9843137f, 0.59607846f, 1.0f};
    const Color4 Color4::PaleTurquoise = {0.6862745f, 0.93333334f, 0.93333334f, 1.0f};
    const Color4 Color4::PaleVioletRed = {0.85882354f, 0.4392157f, 0.5764706f, 1.0f};
    const Color4 Color4::PapayaWhip = {1.0f, 0.9372549f, 0.8352941f, 1.0f};
    const Color4 Color4::PeachPuff = {1.0f, 0.85490197f, 0.7254902f, 1.0f};
    const Color4 Color4::Peru = {0.8039216f, 0.52156866f, 0.24705882f, 1.0f};
    const Color4 Color4::Pink = {1.0f, 0.7529412f, 0.79607844f, 1.0f};
    const Color4 Color4::Plum = {0.8666667f, 0.627451f, 0.8666667f, 1.0f};
    const Color4 Color4::PowderBlue = {0.6901961f, 0.8784314f, 0.9019608f, 1.0f};
    const Color4 Color4::Purple = {0.5019608f, 0.0f, 0.5019608f, 1.0f};
    const Color4 Color4::Red = {1.0f, 0.0f, 0.0f, 1.0f};
    const Color4 Color4::RosyBrown = {0.7372549f, 0.56078434f, 0.56078434f, 1.0f};
    const Color4 Color4::RoyalBlue = {0.25490198f, 0.4117647f, 0.88235295f, 1.0f};
    const Color4 Color4::SaddleBrown = {0.54509807f, 0.27058825f, 0.07450981f, 1.0f};
    const Color4 Color4::Salmon = {0.98039216f, 0.5019608f, 0.44705883f, 1.0f};
    const Color4 Color4::SandyBrown = {0.95686275f, 0.6431373f, 0.3764706f, 1.0f};
    const Color4 Color4::SeaGreen = {0.18039216f, 0.54509807f, 0.34117648f, 1.0f};
    const Color4 Color4::SeaShell = {1.0f, 0.9607843f, 0.93333334f, 1.0f};
    const Color4 Color4::Sienna = {0.627451f, 0.32156864f, 0.1764706f, 1.0f};
    const Color4 Color4::Silver = {0.7529412f, 0.7529412f, 0.7529412f, 1.0f};
    const Color4 Color4::SkyBlue = {0.5294118f, 0.80784315f, 0.92156863f, 1.0f};
    const Color4 Color4::SlateBlue = {0.41568628f, 0.3529412f, 0.8039216f, 1.0f};
    const Color4 Color4::SlateGray = {0.4392157f, 0.5019608f, 0.5647059f, 1.0f};
    const Color4 Color4::Snow = {1.0f, 0.98039216f, 0.98039216f, 1.0f};
    const Color4 Color4::SpringGreen = {0.0f, 1.0f, 0.49803922f, 1.0f};
    const Color4 Color4::SteelBlue = {0.27450982f, 0.50980395f, 0.7058824f, 1.0f};
    const Color4 Color4::Tan = {0.8235294f, 0.7058824f, 0.54901963f, 1.0f};
    const Color4 Color4::Teal = {0.0f, 0.5019608f, 0.5019608f, 1.0f};
    const Color4 Color4::Thistle = {0.84705883f, 0.7490196f, 0.84705883f, 1.0f};
    const Color4 Color4::Tomato = {1.0f, 0.3882353f, 0.2784314f, 1.0f};
    const Color4 Color4::Turquoise = {0.2509804f, 0.8784314f, 0.8156863f, 1.0f};
    const Color4 Color4::Violet = {0.93333334f, 0.50980395f, 0.93333334f, 1.0f};
    const Color4 Color4::Wheat = {0.9607843f, 0.87058824f, 0.7019608f, 1.0f};
    const Color4 Color4::Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
    const Color4 Color4::YellowGreen = {0.6039216f, 0.8039216f, 0.19607843f, 1.0f};
}