#pragma once

#include <cstring>
#include <cstdint>
#include <vector>
#include "obiekt.h"

#pragma pack(push, 1)
struct ObjectHeader
{
    ObjectType type;

    // ile bajtow zajmuje obiekt
    uint64_t byte_size;

    // ile jest obiektow
    uint64_t ob_count;

    std::vector<unsigned char> SaveBytes() const
    {
        std::vector<unsigned char> bytes(sizeof(ObjectHeader));

        std::memcpy(bytes.data(), this, sizeof(ObjectHeader));

        return bytes;
    }

    bool LoadBytes(const std::vector<unsigned char>& bytes)
    {
        if (bytes.size() != sizeof(ObjectHeader))
            return false;

        std::memcpy(this, bytes.data(), sizeof(ObjectHeader));

        return true;
    }
};
class FileHeader
{
public:
    FileHeader() : offset(sizeof(FileHeader)) {}
    char magic[4] = { 'M', 'A', 'P', '1' };
    uint32_t version = 1;
    uint32_t objects_header = 0;
    uint64_t offset;

    std::vector<unsigned char> SaveBytes() const
    {
        std::vector<unsigned char> bytes(sizeof(FileHeader));

        std::memcpy(bytes.data(), this, sizeof(FileHeader));

        return bytes;
    }

    bool LoadBytes(const std::vector<unsigned char>& bytes)
    {
        if (bytes.size() != sizeof(FileHeader))
            return false;

        std::memcpy(this, bytes.data(), sizeof(FileHeader));

        return true;
    }

    bool IsValid() const
    {
        return magic[0] == 'M' && magic[1] == 'A' && magic[2] == 'P' && magic[3] == '1';
    }
};
#pragma pack(pop)