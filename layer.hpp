#include <stdint.h>

#pragma pack(push, 1)
struct RGBA
{
    RGBA() = default;
    RGBA(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255)
    {
        this->R = R;
        this->G = G;
        this->B = B;
        this->A = A;
    }

    uint8_t R = 0, G = 0, B = 0, A = 255;
};

//
//warswa zrobiona jedna na raziedla wszystkich obiektow
//UWAGA kazdy obiekt ma wlasny zestaw warstw
struct Layer
{
    RGBA rgb[2];
    uint8_t size[2] = { 0 };
    char name[32] = { 0 };
};
#pragma pack(pop)