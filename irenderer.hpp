#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <cmath>

struct RenderLine
{
    float x1, y1;
    float x2, y2;
};

//
//abstrakcja rysująca
class IRenderer
{
public:
    virtual ~IRenderer() = default;
    virtual void SetImageSize(int w, int h, unsigned char R, unsigned char G, 
                              unsigned char B, unsigned char A) = 0;

    virtual void SetLineSizeColour(int line_size, unsigned char R,  unsigned char G, unsigned char B) = 0;

    virtual void Line(float x1, float y1, float x2, float y2) = 0;

    virtual void Text( float x, float y, float kat, const std::string& text) = 0;

    virtual void TextOnLines( const std::vector<RenderLine>& lines, const std::string& text) = 0;

    virtual bool Save(const std::filesystem::path& file) = 0;
};
