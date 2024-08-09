//
// Created by alexl on 12.08.2023.
//

#include "Font.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Logging/Log.h"
#include "FileSystem/File.h"
#include "Hardware.h"
#include "JobSystem/JobScheduler.h"
#include "MSDFData.h"
#include "Texture.h"
#include "ext/import-font.h"
#include <chrono>
#include <cstddef>
#include <glm.hpp>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <mutex>
#include <vector>

namespace BeeEngine
{
    struct Configuration
    {
        int Width, Height;
    };
    template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static Ref<Texture2D> CreateAndCacheAtlas(const String& fontName,
                                              float fontSize,
                                              const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
                                              const msdf_atlas::FontGeometry& fontGeometry,
                                              const Configuration& config)
    {
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;

        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(
            config.Width, config.Height);
        generator.setAttributes(attributes);
        generator.setThreadCount(Hardware::GetNumberOfCores());
        generator.generate(glyphs.data(), glyphs.size());
        msdfgen::BitmapConstRef<T, N> bitmap = generator.atlasStorage();

        // unsigned char* pixels = new unsigned char[bitmap.width * bitmap.height * N];
        // memcpy(pixels, bitmap.pixels, bitmap.width * bitmap.height * N);
#if 0 // flip image vertically
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
        // delete[] pixels;
        return Texture2D::Create(
            bitmap.width, bitmap.height, {(byte*)bitmap.pixels, (size_t)(bitmap.width * bitmap.height * N)}, N);
    }

    void Font::Init()
    {
        BeeExpects(s_Handle == nullptr);
        BeeCoreTrace("Initializing Freetype");
        s_Handle = msdfgen::initializeFreetype();
        BeeEnsures(s_Handle);
    }
    void Font::Shutdown()
    {
        BeeExpects(s_Handle == nullptr);
        BeeCoreTrace("Freetype Shutdown");
        msdfgen::deinitializeFreetype(static_cast<msdfgen::FreetypeHandle*>(s_Handle));
        s_Handle = nullptr;
    }
    Font::Font(const Path& path) : m_Data(new Internal::MSDFData())
    {
        String pathStr = path;
        {
            std::unique_lock lock(s_Lock);
            if (s_Counter == 0)
            {
                Init();
            }
            s_Counter++;
        }
        auto data = File::ReadBinaryFile(path);
        msdfgen::FontHandle* font = msdfgen::loadFontData(static_cast<msdfgen::FreetypeHandle*>(s_Handle),
                                                          reinterpret_cast<msdfgen::byte*>(data.data()),
                                                          data.size());
        if (!font)
        {
            BeeCoreError("Failed to load font: {}", pathStr);
        }
        LoadFont(font, pathStr);
        msdfgen::destroyFont(font);
    }

    Font::~Font()
    {
        delete m_Data;
        {
            std::unique_lock lock(s_Lock);
            s_Counter--;
            if (s_Counter == 0)
            {
                Shutdown();
            }
        }
    }

    Font::Font(const String& name, gsl::span<byte> data) : m_Data(new Internal::MSDFData())
    {
        {
            std::unique_lock lock(s_Lock);
            if (s_Counter == 0)
            {
                Init();
            }
            s_Counter++;
        }
        msdfgen::FontHandle* font = msdfgen::loadFontData(static_cast<msdfgen::FreetypeHandle*>(s_Handle),
                                                          reinterpret_cast<msdfgen::byte*>(data.data()),
                                                          data.size());
        if (!font)
        {
            BeeCoreError("Failed to load font: {}", name);
        }
        LoadFont(font, name);
        msdfgen::destroyFont(font);
    }

    void Font::LoadFont(void* handle, const String& name)
    {
        msdfgen::FontHandle* font = static_cast<msdfgen::FontHandle*>(handle);
        m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
        constexpr static float fontScale = 1.0f;

        struct CharsetRange
        {
            uint32_t First;
            uint32_t Last;
        };

        // From imgui_draw.cpp:
        static constexpr CharsetRange charsetRanges[] = {
            {0x0020, 0x00FF}, // Basic Latin + Latin Supplement
            {0x0100, 0x017F}, // Latin Extended-A
            {0x0180, 0x024F}, // Latin Extended-B
            {0x0400, 0x052F}, // Cyrillic + Cyrillic Supplement
            {0x2DE0, 0x2DFF}, // Cyrillic Extended-A
            {0xA640, 0xA69F}, // Cyrillic Extended-B
        };

        msdf_atlas::Charset charset;
        for (CharsetRange range : charsetRanges)
        {
            for (uint32_t i = range.First; i <= range.Last; ++i)
                charset.add(i);
        }
        int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
        BeeCoreTrace("Loaded {}/{} glyphs", glyphsLoaded, charset.size());
#if defined(DEBUG)
        double emSize = 40.0;
#else
        // TODO: Read documentation and check, whether this is the best solution to improve look of the text
        double emSize = 400.0;
#endif

        msdf_atlas::TightAtlasPacker atlasPacker;
        // atlasPacker.setDimensionsConstraint();
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

        // if MSDF or MTSDF

        const uint64_t seed = 0;
        constexpr bool expensiveColoring = true;
        constexpr static double defaultAngleThreshold = 3.0;

        if constexpr (expensiveColoring)
        {
            auto jobFunc = [seed](auto& glyph, size_t index)
            {
                uint64_t glyphSeed = (LCG_MULTIPLIER * (seed ^ index) + LCG_INCREMENT) * !!seed;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap, defaultAngleThreshold, glyphSeed);
            };
            Jobs::Counter counter;
            Jobs::ForEach(m_Data->Glyphs, counter, jobFunc);
            Jobs::WaitForJobsToComplete(counter);
        }
        else
        {
            uint64_t glyphSeed = seed;
            for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
            {
                glyphSeed += LCG_MULTIPLIER;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap, defaultAngleThreshold, glyphSeed);
            }
        }
#undef LCG_MULTIPLIER
#undef LCG_INCREMENT

        m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 4, msdf_atlas::mtsdfGenerator>(
            name, emSize, m_Data->Glyphs, m_Data->FontGeometry, {width, height});
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
} // namespace BeeEngine
