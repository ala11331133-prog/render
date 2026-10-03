#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <cmath>

struct RenderPixel
{
    RenderPixel(void) 
    {
        R = G = B = 0;
        A = 255;
    };
    RenderPixel(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
    {
        this->R = R;
        this->G = G;
        this->B = B;
        this->A = A;
    }

    RenderPixel(unsigned char R, unsigned char G, unsigned char B)
    {
        this->R = R;
        this->G = G;
        this->B = B;
        this->A = 255;
    }

    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A=255;
};

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
    virtual void SetImageSize(int w, int h, RenderPixel px) = 0;

    virtual void SetLineSizeColour(int line_size, RenderPixel px) = 0;

    virtual void Line(float x1, float y1, float x2, float y2) = 0;

    virtual void Text( float x, float y, float kat, const std::string& text) = 0;

    virtual void TextOnLines( const std::vector<RenderLine>& lines, const std::string& text) = 0;

    virtual bool Save(const std::filesystem::path& file) = 0;
};
