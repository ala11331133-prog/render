// Wybiera z pliku linie.bin te kreski, ktore widac na ekranie.
// Uzycie:
//   std::vector<TGetLine> data;
//   Get(data, "linie.bin", zoom, szerokosc, wysokosc, srodek_x, srodek_y);
//
// Plik: najpierw uint32 z liczba kresek, potem kreski jedna za druga (TGetLine, 22 bajty).
// x to dlugosc geograficzna, y szerokosc. zoom to piksele na jeden stopien szerokosci.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

// pack(1) zeby struktura byla dokladnie tym, co jest w pliku, bez dziur na wyrownanie
#pragma pack(push, 1)
struct TGetLine
{
    TGetLine() = default;
    TGetLine(std::uint8_t reserved_, std::int32_t TextIndex_, std::uint8_t warstwa_, float x1_, float y1_,
          float x2_, float y2_)
        : reserved(reserved_), TextIndex(TextIndex_), warstwa(warstwa_), x1(x1_), y1(y1_), x2(x2_), y2(y2_)
    {
    }

    std::uint8_t reserved;    // 0 nic, 1 widoczny, 254 zaznaczony, 255 do skasowania
    std::int32_t TextIndex;   // numer tekstu, -1 gdy brak
    std::uint8_t warstwa;
    float x1, y1, x2, y2;
};
#pragma pack(pop)
static_assert(sizeof(TGetLine) == 22, "TGetLine musi miec 22 bajty");

// Prostokat na mapie, ktory widac na ekranie
struct TOknoMapy
{
    float minx, miny, maxx, maxy;
};

// Liczy, jaki kawalek mapy miesci sie na ekranie. Zwraca false, gdy rozmiary nie maja sensu.
// Margines to zapas w pikselach, zeby gruba linia tuz przy brzegu nie znikala.
inline bool OknoWidoku(float zoom, int szerokosc, int wysokosc, float srodek_x, float srodek_y,
                       float margines_px, TOknoMapy &okno)
{
    if (zoom <= 0 || szerokosc <= 0 || wysokosc <= 0)
        return false;

    // Im dalej od rownika, tym wezsze sa stopnie dlugosci, stad cos
    double cos_lat = std::cos(srodek_y * 3.14159265358979 / 180);
    double pol_szer = (szerokosc / 2.0 + margines_px) / (zoom * cos_lat);
    double pol_wys = (wysokosc / 2.0 + margines_px) / zoom;

    okno = {(float)(srodek_x - pol_szer), (float)(srodek_y - pol_wys),
            (float)(srodek_x + pol_szer), (float)(srodek_y + pol_wys)};
    return true;
}

// Szybki test: czy prostokat, w ktorym miesci sie kreska, dotyka okna
inline bool KreskaStyka(const TGetLine &l, float x1, float y1, float x2, float y2)
{
    return std::max(l.x1, l.x2) >= std::min(x1, x2) && std::min(l.x1, l.x2) <= std::max(x1, x2) &&
           std::max(l.y1, l.y2) >= std::min(y1, y2) && std::min(l.y1, l.y2) <= std::max(y1, y2);
}

// Dokladny test: czy kreska naprawde wchodzi w okno (algorytm Lianga-Barsky'ego).
// Idziemy po kresce od poczatku (t=0) do konca (t=1) i przycinamy ja kolejnymi bokami okna.
// Jak nic z niej nie zostanie, to okna nie dotyka.
inline bool KreskaPrzecina(const TGetLine &l, float minx, float miny, float maxx, float maxy)
{
    float dx = l.x2 - l.x1, dy = l.y2 - l.y1;
    float kierunek[4] = {-dx, dx, -dy, dy};
    float odleglosc[4] = {l.x1 - minx, maxx - l.x1, l.y1 - miny, maxy - l.y1};

    float od = 0, doo = 1;
    for (int i = 0; i < 4; ++i)
    {
        if (kierunek[i] == 0)
        {
            // kreska rownolegla do boku: albo jest w srodku, albo calkiem obok
            if (odleglosc[i] < 0)
                return false;
            continue;
        }
        float t = odleglosc[i] / kierunek[i];
        if (kierunek[i] < 0)
            od = std::max(od, t);
        else
            doo = std::min(doo, t);
        if (od > doo)
            return false;
    }
    return true;
}

inline bool KreskaWidac(const TGetLine &l, const TOknoMapy &o)
{
    return KreskaStyka(l, o.minx, o.miny, o.maxx, o.maxy) && KreskaPrzecina(l, o.minx, o.miny, o.maxx, o.maxy);
}

// Czyta plik i wrzuca do data tylko widoczne kreski (stare dane z data znikaja).
// Zwraca ile ich jest. Przy zlych rozmiarach, braku pliku albo uciętym pliku zwraca 0.
// Plik czytamy po kawalku, zeby nie trzymac calej mapy w pamieci.
inline size_t Get(std::vector<TGetLine> &data, const std::string &sciezka, float zoom, int szerokosc,
                  int wysokosc, float srodek_x, float srodek_y, float margines_px = 2)
{
    data.clear();

    TOknoMapy okno;
    if (!OknoWidoku(zoom, szerokosc, wysokosc, srodek_x, srodek_y, margines_px, okno))
        return 0;

    std::ifstream plik(sciezka, std::ios::binary);
    std::uint32_t zostalo = 0;
    if (!plik.read((char *)&zostalo, sizeof(zostalo)))
        return 0;

    std::vector<TGetLine> kawalek(4096);
    while (zostalo > 0)
    {
        std::uint32_t ile = std::min<std::uint32_t>(zostalo, (std::uint32_t)kawalek.size());
        if (!plik.read((char *)kawalek.data(), ile * sizeof(TGetLine)))
        {
            data.clear();
            return 0;
        }

        for (std::uint32_t i = 0; i < ile; ++i)
            if (KreskaWidac(kawalek[i], okno))
                data.push_back(kawalek[i]);

        zostalo -= ile;
    }
    return data.size();
}
