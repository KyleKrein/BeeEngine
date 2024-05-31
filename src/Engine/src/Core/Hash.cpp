//
// Created by alexl on 17.10.2023.
//

#include "Hash.h"
#include "Application.h"

namespace BeeEngine
{
    // Функция для приведения данных к нужному порядку байтов
    uint32_t to_little_endian(uint32_t x)
    {
        if (Application::IsLittleEndian())
        {
            return x;
        }
        return ((x >> 24) & 0xff) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | ((x << 24) & 0xff000000);
    }

    uint32_t HashAlgorithm::MurmurHash2(const void* key, size_t len, uint32_t seed)
    {
        const uint32_t m = 0x5bd1e995;
        const int r = 24;

        uint32_t h = seed ^ len;

        const auto* data = (const uint8_t*)key;

        while (len >= 4)
        {
            uint32_t k;
            std::memcpy(&k, data, sizeof(uint32_t));
            k = to_little_endian(k);

            k *= m;
            k ^= k >> r;
            k *= m;

            h *= m;
            h ^= k;

            data += 4;
            len -= 4;
        }

        switch (len)
        {
            case 3:
                h ^= data[2] << 16;
            case 2:
                h ^= data[1] << 8;
            case 1:
                h ^= data[0];
                h *= m;
        };

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h;
    }

    uint32_t HashAlgorithm::MurmurHash2(const UTF8String& key, uint32_t seed)
    {
        return MurmurHash2(key.data(), key.size(), seed);
    }

    uint32_t HashAlgorithm::MurmurHash2(const std::string_view& key, uint32_t seed)
    {
        return MurmurHash2(key.data(), key.size(), seed);
    }

    uint32_t HashAlgorithm::MurmurHash2(const char* key, uint32_t seed)
    {
        return MurmurHash2(key, strlen(key), seed);
    }

    uint64_t HashAlgorithm::MurmurHash2_64(const void* key, size_t len, uint64_t seed)
    {
        const uint64_t m = UINT64_C(0xc6a4a7935bd1e995);
        const int r = 47;

        uint64_t h = seed ^ (len * m);

        const uint64_t* data = (const uint64_t*)key;
        const uint64_t* end = data + (len / 8);

        while (data != end)
        {
            uint64_t k = *data++;
            if (!Application::IsLittleEndian())
            {
                k = (k >> 56) | ((k << 40) & 0x00FF000000000000) | ((k << 24) & 0x0000FF0000000000) |
                    ((k << 8) & 0x000000FF00000000) | ((k >> 8) & 0x00000000FF000000) |
                    ((k >> 24) & 0x0000000000FF0000) | ((k >> 40) & 0x000000000000FF00) | (k << 56);
            }
            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char* data2 = (const unsigned char*)data;

        switch (len & 7)
        {
            case 7:
                h ^= uint64_t(data2[6]) << 48;
            case 6:
                h ^= uint64_t(data2[5]) << 40;
            case 5:
                h ^= uint64_t(data2[4]) << 32;
            case 4:
                h ^= uint64_t(data2[3]) << 24;
            case 3:
                h ^= uint64_t(data2[2]) << 16;
            case 2:
                h ^= uint64_t(data2[1]) << 8;
            case 1:
                h ^= uint64_t(data2[0]);
                h *= m;
        }

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    uint64_t HashAlgorithm::MurmurHash2_64(const UTF8String& key, uint64_t seed)
    {
        return MurmurHash2_64(key.data(), key.size(), seed);
    }

    uint64_t HashAlgorithm::MurmurHash2_64(const std::string_view& key, uint64_t seed)
    {
        return MurmurHash2_64(key.data(), key.size(), seed);
    }

    uint64_t HashAlgorithm::MurmurHash2_64(const char* key, uint64_t seed)
    {
        return MurmurHash2_64(key, strlen(key), seed);
    }

    template <>
    uint64_t Hash<UTF8String>(const UTF8String& value, uint64_t seed)
    {
        return HashAlgorithm::MurmurHash2_64(value, seed);
    }
} // namespace BeeEngine