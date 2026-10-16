#pragma once

#include <cstdint>
#include <cstddef>

inline uint32_t CRC32(const void* data, size_t size)
{
    const auto* bytes = static_cast<const uint8_t*>(data);

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < size; ++i)
    {
        crc ^= bytes[i];

        for (int j = 0; j < 8; ++j)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }

    return crc ^ 0xFFFFFFFF;
}