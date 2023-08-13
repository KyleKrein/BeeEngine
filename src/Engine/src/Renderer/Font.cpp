//
// Created by alexl on 12.08.2023.
//

#include "Font.h"
#include "Core/Logging/Log.h"
#include "Core/CodeSafety/Expects.h"
#include "Texture.h"
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include "MSDFData.h"
#include <glm.hpp>

namespace BeeEngine
{
    struct Configuration
    {
        int Width, Height;
    };
    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static Ref<Texture2D> CreateAndCacheAtlas(const std::string& fontName, float fontSize,
                                              const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
                                              const msdf_atlas::FontGeometry& fontGeometry,
                                              const Configuration& config)
    {
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;

        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T,N>> generator(config.Width, config.Height);
        generator.setAttributes(attributes);
        generator.setThreadCount(8);
        generator.generate(glyphs.data(), glyphs.size());
        msdfgen::BitmapConstRef<T, N> bitmap = generator.atlasStorage();

        Ref<Texture2D> texture = Texture2D::Create(bitmap.width, bitmap.height);

        unsigned char* pixels = new unsigned char[bitmap.width * bitmap.height * N];
        memcpy(pixels, bitmap.pixels, bitmap.width * bitmap.height * N);
#if 0 //flip image vertically
        const size_t stride = bitmap.width * N;
        unsigned char *row = (decltype(row))malloc(stride);
        unsigned char *low = pixels;
        unsigned char *high = &pixels[(bitmap.height - 1) * stride];

        for (; low < high; low += stride, high -= stride) {
            memcpy(row, low, stride);
            memcpy(low, high, stride);
            memcpy(high, row, stride);
        }
        free(row);
#endif
        texture->SetData({(byte*)pixels, (size_t)(bitmap.width * bitmap.height * N)}, N);
        delete[] pixels;
        return texture;
    }

    Font::Font(const std::filesystem::path &path)
    : m_Data(new Internal::MSDFData())
    {
        std::string pathStr = path.string();
        msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
        if(!ft)
            return;
        msdfgen::FontHandle *font = msdfgen::loadFont(ft, pathStr.c_str());
        if (!font)
        {
            BeeCoreError("Failed to load font: {}", pathStr);
        }
        LoadFont(font, pathStr);
        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);
    }

    Font::~Font()
    {
        delete m_Data;
    }

    Font::Font(const std::string& name, gsl::span<byte> data)
    : m_Data(new Internal::MSDFData())
    {
        msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
        if(!ft)
            return;
        msdfgen::FontHandle *font = msdfgen::loadFontData(ft, reinterpret_cast<const msdfgen::byte *>(data.data()), data.size());
        if (!font)
        {
            BeeCoreError("Failed to load font: {}", name);
        }
        LoadFont(font, name);
        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);
    }

    void Font::LoadFont(void *handle, const std::string& name)
    {
        msdfgen::FontHandle *font = (msdfgen::FontHandle*)handle;
        m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
        constexpr static float fontScale = 1.0f;

        struct CharsetRange
        {
            uint32_t First;
            uint32_t Last;
        };

        //From imgui_draw.cpp:
        static constexpr CharsetRange charsetRanges[] =
                {
                        {0x0020, 0x00FF}, // Basic Latin + Latin Supplement
                };

        msdf_atlas::Charset charset;
        for (CharsetRange range : charsetRanges)
        {
            for(uint32_t i = range.First; i <= range.Last; ++i)
                charset.add(i);
        }
        int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
        BeeCoreTrace("Loaded {}/{} glyphs", glyphsLoaded, charset.size());

        double emSize = 40.0;

        msdf_atlas::TightAtlasPacker atlasPacker;
        //atlasPacker.setDimensionsConstraint();
        atlasPacker.setPixelRange(2.0f);
        atlasPacker.setMiterLimit(1.0f);
        atlasPacker.setPadding(0.0f);
        atlasPacker.setScale(emSize);
        int remaining = atlasPacker.pack(m_Data->Glyphs.data(), m_Data->Glyphs.size());
        BeeCoreTrace("Packed {}/{} glyphs", m_Data->Glyphs.size() - remaining, m_Data->Glyphs.size());
        BeeEnsures(remaining == 0);

        int width, height;
        atlasPacker.getDimensions(width, height);

        emSize = atlasPacker.getScale();

#define LCG_MULTIPLIER 6364136223846793005ULL
#define LCG_INCREMENT 1442695040888963407ULL

        //if MSDF or MTSDF

        const uint64_t seed = 0;
        constexpr static uint64_t threadCount = 8;
        const bool expensiveColoring = false;
        constexpr static double defaultAngleThreshold = 3.0;
        if(expensiveColoring)
        {
            msdf_atlas::Workload([&glyphs = m_Data->Glyphs, seed](int i, int threadNo) -> bool{
                uint64_t  glyphSeed = (LCG_MULTIPLIER * (seed ^ i) + LCG_INCREMENT) * !!seed;
                glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, defaultAngleThreshold, glyphSeed);
                return true;
            }, m_Data->Glyphs.size()).finish(threadCount);
        }
        else
        {
            uint64_t glyphSeed = seed;
            for(msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
            {
                glyphSeed+= LCG_MULTIPLIER;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap, defaultAngleThreshold, glyphSeed);
            }
        }
#undef LCG_MULTIPLIER
#undef LCG_INCREMENT

        m_AtlasTexture = CreateAndCacheAtlas<uint8_t , float, 4, msdf_atlas::mtsdfGenerator>(name, emSize, m_Data->Glyphs, m_Data->FontGeometry, {width, height});
#if 0
        msdfgen::Shape shape;
        if(msdfgen::loadGlyph(shape, font, 'A'))
        {
            shape.normalize();
            //                                      max angle
            msdfgen::edgeColoringSimple(shape, 3.0);
            //                      image width, image height
            msdfgen::Bitmap<float, 3> msdf(32, 32);
            msdfgen::generateMSDF(msdf, shape, 4.0, msdfgen::Vector2(1.0, 1.0), msdfgen::Vector2(4.0, 4.0));
            msdfgen::savePng(msdf, "msdf.png");
        }
        msdfgen::loadGlyph(shape, font, 'A');
#endif
    }
}
