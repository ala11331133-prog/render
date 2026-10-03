// Wybiera z pliku linie.bin te kreski, ktore widac na ekranie.
// Uzycie:
//   std::vector<TLine> data;
//   Get(data, "linie.bin", zoom, szerokosc, wysokosc, srodek_x, srodek_y);
//
// Plik: najpierw uint32 z liczba kresek, potem kreski jedna za druga (TLine, 22 bajty).
// x to dlugosc geograficzna, y szerokosc. zoom to piksele na jeden stopien szerokosci.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "line.h"
#include "test.hpp"

// Liczy, jaki kawalek mapy miesci sie na ekranie. Zwraca false, gdy rozmiary nie maja sensu.
// Margines to zapas w pikselach, zeby gruba linia tuz przy brzegu nie znikala.
inline bool OknoWidoku(float zoom, int szerokosc, int wysokosc, float srodek_x, float srodek_y,
                       float margines_px, TMaxMinOb &okno)
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

// Czyta plik i wrzuca do data tylko widoczne kreski (stare dane z data znikaja).
// Zwraca ile ich jest. Przy zlych rozmiarach, braku pliku albo uciętym pliku zwraca 0.
// Plik czytamy po kawalku, zeby nie trzymac calej mapy w pamieci.
inline size_t GetMaxMinLineData(std::vector<TLine> &data, const std::string &sciezka, float zoom, int szerokosc,
                  int wysokosc, float srodek_x, float srodek_y, float margines_px = 2)
{
    TLineC line;
   
    data.clear();

    TMaxMinOb okno;
    if (!OknoWidoku(zoom, szerokosc, wysokosc, srodek_x, srodek_y, margines_px, okno))
        return 0;

    std::ifstream plik(sciezka, std::ios::binary);
    std::uint32_t zostalo = 0;
    if (!plik.read((char *)&zostalo, sizeof(zostalo)))
        return 0;

    std::vector<TLine> kawalek(4096);
    while (zostalo > 0)
    {
        std::uint32_t ile = std::min<std::uint32_t>(zostalo, (std::uint32_t)kawalek.size());
        if (!plik.read((char *)kawalek.data(), ile * sizeof(TLine)))
        {
            data.clear();
            return 0;
        }

        for (std::uint32_t i = 0; i < ile; ++i)
            if (line.IsIn(kawalek[i], okno, true))
                data.push_back(kawalek[i]);

        zostalo -= ile;
    }
    return data.size();
}
