//
// Created by alexl on 12.08.2023.
//

#include "Font.h"
#include "Core/AssetManagement/TextureImporter.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Logging/Log.h"
#include "FileSystem/File.h"
#include "Hardware.h"
#include "JobSystem/JobScheduler.h"
#include "MSDFData.h"
#include "Platform/Windows/WindowsString.h"
#include "Renderer/BindingSet.h"
#include "Renderer/Pipeline.h"
#include "Renderer/ShaderModule.h"
#include "Texture.h"
#include "ext/import-font.h"
#include "ext/save-png.h"
#include <chrono>
#include <cstddef>
#include <glm.hpp>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <mutex>
#include <stb_image_write.h>
#include "Core/Application.h"
#include <unordered_map>
#include <vector>

#if defined(WINDOWS)
int stbi_write_png(const wchar_t* filename, int x, int y, int comp, const void* data, int stride_bytes);
#endif

#define JOBS_MODE 0

namespace BeeEngine
{
    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    class JobsImmediateAtlasGenerator
    {
    public:
        JobsImmediateAtlasGenerator();
        JobsImmediateAtlasGenerator(int width, int height);
        template <typename... ARGS>
        JobsImmediateAtlasGenerator(int width, int height, ARGS... storageArgs);
        void generate(std::span<const msdf_atlas::GlyphGeometry> glyphs);
        void rearrange(int width, int height, const msdf_atlas::Remap* remapping, int count);
        void resize(int width, int height);
        /// Sets attributes for the generator function
        void setAttributes(const msdf_atlas::GeneratorAttributes& attributes);
        /// Allows access to the underlying AtlasStorage
        const AtlasStorage& atlasStorage() const;
        /// Returns the layout of the contained glyphs as a list of GlyphBoxes
        const std::vector<msdf_atlas::GlyphBox>& getLayout() const;

    private:
        AtlasStorage storage;
        std::vector<msdf_atlas::GlyphBox> layout;
        std::vector<T> glyphBuffer;
        std::vector<byte> errorCorrectionBuffer;
        msdf_atlas::GeneratorAttributes attributes;
    };
    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::JobsImmediateAtlasGenerator()
    {
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::JobsImmediateAtlasGenerator(int width, int height)
        : storage(width, height)
    {
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    template <typename... ARGS>
    JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::JobsImmediateAtlasGenerator(int width,
                                                                                         int height,
                                                                                         ARGS... storageArgs)
        : storage(width, height, storageArgs...)
    {
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    void
    JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::generate(std::span<const msdf_atlas::GlyphGeometry> glyphs)
    {
        int maxBoxArea = 0;
        for (size_t i = 0; i < glyphs.size(); ++i)
        {
            msdf_atlas::GlyphBox box = glyphs[i];
            maxBoxArea = std::max(maxBoxArea, box.rect.w * box.rect.h);
            layout.push_back((msdf_atlas::GlyphBox&&)box);
        }
        size_t threadCount = Hardware::GetNumberOfCores();
        int threadBufferSize = N * maxBoxArea;
        if (threadCount * threadBufferSize > glyphBuffer.size())
        {
            glyphBuffer.resize(threadCount * threadBufferSize);
        }
        if (threadCount * maxBoxArea > errorCorrectionBuffer.size())
        {
            errorCorrectionBuffer.resize(threadCount * maxBoxArea);
        }
        std::vector<msdf_atlas::GeneratorAttributes> threadAttributes(threadCount);
        for (size_t i = 0; i < threadCount; ++i)
        {
            threadAttributes[i] = attributes;
            threadAttributes[i].config.errorCorrection.buffer =
                reinterpret_cast<msdfgen::byte*>(errorCorrectionBuffer.data() + i * maxBoxArea);
        }
        Jobs::Counter counter;
        auto jobFunc = [this, &threadAttributes, threadBufferSize, maxBoxArea](const auto& glyph, Jobs::Indices indices)
        {
            if (glyph.isWhitespace())
            {
                return;
            }
            int l, b, w, h;
            glyph.getBoxRect(l, b, w, h);
            msdfgen::BitmapRef<T, N> glyphBitmap(glyphBuffer.data() + indices.PackIndex * threadBufferSize, w, h);
            GEN_FN(glyphBitmap, glyph, threadAttributes.at(indices.PackIndex));
            storage.put(l, b, msdfgen::BitmapConstRef<T, N>(glyphBitmap));
        };
        Jobs::ForEach(glyphs, counter, jobFunc);
        Jobs::WaitForJobsToComplete(counter);
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    void JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::rearrange(int width,
                                                                            int height,
                                                                            const msdf_atlas::Remap* remapping,
                                                                            int count)
    {
        for (int i = 0; i < count; ++i)
        {
            layout[remapping[i].index].rect.x = remapping[i].target.x;
            layout[remapping[i].index].rect.y = remapping[i].target.y;
        }
        AtlasStorage newStorage((AtlasStorage&&)storage, width, height, remapping, count);
        storage = (AtlasStorage&&)newStorage;
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    void JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::resize(int width, int height)
    {
        AtlasStorage newStorage((AtlasStorage&&)storage, width, height);
        storage = (AtlasStorage&&)newStorage;
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    void JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::setAttributes(
        const msdf_atlas::GeneratorAttributes& attributes)
    {
        this->attributes = attributes;
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    const AtlasStorage& JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::atlasStorage() const
    {
        return storage;
    }

    template <typename T, int N, msdf_atlas::GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
    const std::vector<msdf_atlas::GlyphBox>& JobsImmediateAtlasGenerator<T, N, GEN_FN, AtlasStorage>::getLayout() const
    {
        return layout;
    }

    struct Configuration
    {
        int Width, Height;
    };
    bool IsCacheValid(const Path& cachedPath)
    {
        return File::Exists(cachedPath);
    }
    template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    static Scope<GPUTextureResource> CreateAndCacheAtlas(const String& fontName,
                                                         float fontSize,
                                                         const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
                                                         const msdf_atlas::FontGeometry& fontGeometry,
                                                         const Configuration& config, const Path& cacheFolder)
    {
        auto cachedPath = cacheFolder / (Path(fontName).GetFileName().AsUTF8() + ".png");
        if (IsCacheValid(cachedPath))
        {
            auto result = GPUTextureResource::Create(cachedPath);
            if (result)
            {
                return result;
            }
            BeeCoreError("Unable to load cached atlas texture from {}", cachedPath);
        }
        msdf_atlas::GeneratorAttributes attributes;
        attributes.config.overlapSupport = true;
        attributes.scanlinePass = true;
#if JOBS_MODE
        JobsImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(config.Width,
                                                                                                   config.Height);
        generator.setAttributes(attributes);
        generator.generate(std::span{glyphs.data(), glyphs.size()});
#else
        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(
            config.Width, config.Height);
        generator.setThreadCount(Hardware::GetNumberOfCores());
        generator.setAttributes(attributes);
        generator.generate(glyphs.data(), glyphs.size());
#endif
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
#if defined(WINDOWS)
        std::wstring p = Internal::WStringFromUTF8(cachedPath);
#else
        const String& p = cachedPath;
#endif
        stbi_flip_vertically_on_write(true);
        stbi_write_png(p.c_str(), bitmap.width, bitmap.height, N, bitmap.pixels, bitmap.width * N);
        return GPUTextureResource::Create(
            bitmap.width, bitmap.height, {(byte*)bitmap.pixels, (size_t)(bitmap.width * bitmap.height * N)}, N);
    }
    struct Font::StaticData
    {
        msdfgen::FreetypeHandle* FreeType = msdfgen::initializeFreetype();
      Path CacheFolder = Application::GetInstance().Environment().CacheDirectory() / "FontAtlases";
        StaticData()
        {
          if (!File::Exists(CacheFolder))
            {
              File::CreateDirectory(CacheFolder);
            }
        }
        // Ref<Pipeline> AtlasPipeline =
        //     Pipeline::Create(ShaderModule::Create("Shaders/MSDFGenerator.comp", ShaderType::Compute));
        ~StaticData()
        {
            BeeCoreTrace("Freetype Shutdown");
            msdfgen::deinitializeFreetype(FreeType);
        }
    };

    void Font::Init()
    {
        BeeExpects(s_Handle == nullptr);
        BeeCoreTrace("Initializing Freetype");
        s_Handle = new StaticData{};
        BeeEnsures(s_Handle);
    }
    void Font::Shutdown()
    {
        BeeExpects(s_Handle != nullptr);
        delete s_Handle;
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
        auto start = std::chrono::high_resolution_clock::now();
        auto data = File::ReadBinaryFile(path);
        msdfgen::FontHandle* font =
            msdfgen::loadFontData(s_Handle->FreeType, reinterpret_cast<msdfgen::byte*>(data.data()), data.size());
        if (!font)
        {
            BeeCoreError("Failed to load font: {}", pathStr);
        }
        LoadFont(font, pathStr);
        msdfgen::destroyFont(font);
        BeeCoreInfo(
            "{} Font loading took: {} ms",
            pathStr,
            std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());
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
        auto start = std::chrono::high_resolution_clock::now();
        msdfgen::FontHandle* font =
            msdfgen::loadFontData(s_Handle->FreeType, reinterpret_cast<msdfgen::byte*>(data.data()), data.size());
        if (!font)
        {
            BeeCoreError("Failed to load font: {}", name);
        }
        LoadFont(font, name);
        msdfgen::destroyFont(font);
        BeeCoreInfo(
            "{} Font loading took: {} ms",
            name,
            std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count());
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

        static const msdf_atlas::Charset charset = []()
        {
            msdf_atlas::Charset charset;
            for (CharsetRange range : charsetRanges)
            {
                for (uint32_t i = range.First; i <= range.Last; ++i)
                {
                    charset.add(i);
                }
            }
            return charset;
        }();
        int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
        BeeCoreTrace("Loaded {}/{} glyphs", glyphsLoaded, charset.size());

        // TODO: Read documentation and check, whether this is the best solution to improve look of the text
        double emSize = 100.0;

        msdf_atlas::TightAtlasPacker atlasPacker;
        // atlasPacker.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
        atlasPacker.setMinimumScale(emSize);
        // setPixelRange or setUnitRange
        atlasPacker.setPixelRange(2.0);
        atlasPacker.setMiterLimit(1.0);
        //  atlasPacker.setPadding(0.0f);
        //  atlasPacker.setScale(emSize);
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
#if JOBS_MODE || 1
            auto jobFunc = [seed](auto& glyph, size_t index)
            {
                uint64_t glyphSeed = (LCG_MULTIPLIER * (seed ^ index) + LCG_INCREMENT) * !!seed;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap, defaultAngleThreshold, glyphSeed);
            };
            Jobs::Counter counter;
            Jobs::ForEach(m_Data->Glyphs, counter, jobFunc);
            Jobs::WaitForJobsToComplete(counter);
#else
            msdf_atlas::Workload(
                [&glyphs = m_Data->Glyphs, seed](int i, int threadNo) -> bool
                {
                    uint64_t glyphSeed = (LCG_MULTIPLIER * (seed ^ i) + LCG_INCREMENT) * !!seed;
                    glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, defaultAngleThreshold, glyphSeed);
                    return true;
                },
                m_Data->Glyphs.size())
                .finish(Hardware::GetNumberOfCores());
#endif
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
            name, emSize, m_Data->Glyphs, m_Data->FontGeometry, {width, height}, s_Handle->CacheFolder);
        m_BindingSet = BindingSet::Create({{0, *m_AtlasTexture}});
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
