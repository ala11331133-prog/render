#include "linie.h"

#include <algorithm>
#include <cmath>

static bool Styka(const TLine &l, float x1, float y1, float x2, float y2)
{
    return std::max(l.x1, l.x2) >= std::min(x1, x2) && std::min(l.x1, l.x2) <= std::max(x1, x2) &&
           std::max(l.y1, l.y2) >= std::min(y1, y2) && std::min(l.y1, l.y2) <= std::max(y1, y2);
}

const std::vector<TLine> &TLineC::Utworz(const std::vector<TOsmDroga> &drogi)
{
    Line.clear();
    for (const TOsmDroga &d : drogi)
    {
        unsigned char w = WarstwaZTagow(d.tagi);
        for (size_t i = 1; i < d.punkty.size(); ++i)
        {
            const TOsmPunkt &a = d.punkty[i - 1], &b = d.punkty[i];
            Line.push_back({0, -1, w, a.x, a.y, b.x, b.y});
        }
    }
    return Line;
}

std::vector<TLine> TLineC::Pobierz(float x1, float y1, float x2, float y2, int warstwa) const
{
    std::vector<TLine> wynik;
    for (const TLine &l : Line)
        if ((warstwa < 0 || l.warstwa == warstwa) && Styka(l, x1, y1, x2, y2))
            wynik.push_back(l);
    return wynik;
}

int TLineC::WarstwaZTagow(const std::vector<std::pair<std::string, std::string>> &tagi)
{
    for (const auto &[k, v] : tagi)
    {
        if (k == "highway")
        {
            if (v == "motorway" || v == "trunk" || v == "primary" ||
                v == "motorway_link" || v == "trunk_link" || v == "primary_link")
                return W_DROGA_GLOWNA;
            if (v == "secondary" || v == "tertiary" || v == "secondary_link" || v == "tertiary_link")
                return W_DROGA_SREDNIA;
            if (v == "residential" || v == "unclassified" || v == "living_street" || v == "service")
                return W_ULICA;
            if (v == "footway" || v == "path" || v == "steps" || v == "cycleway" || v == "pedestrian")
                return W_PIESZA;
        }
        else if (k == "railway")
            return W_KOLEJ;
        else if (k == "building")
            return W_BUDYNEK;
        else if (k == "waterway" || (k == "natural" && v == "water"))
            return W_WODA;
    }
    return W_INNE;
}

void TLineC::Free(void)
{
    Line.clear();
}

//Format: ilosc, tablica TLine
long TLineC::Load(std::ifstream &zpliku)
{
    uint32_t ilosc = 0;
    zpliku.read((char *)&ilosc, sizeof(ilosc));
    Line.resize(ilosc);
    zpliku.read((char *)Line.data(), ilosc * sizeof(TLine));
    if (!zpliku)
    {
        Line.clear();
        return -1;
    }
    return ilosc;
}

long TLineC::Save(std::ofstream &naplik)
{
    uint32_t ilosc = Line.size();
    naplik.write((const char *)&ilosc, sizeof(ilosc));
    naplik.write((const char *)Line.data(), ilosc * sizeof(TLine));
    return naplik ? (long)ilosc : -1;
}

float TLineC::Distance(long numer, float x3, float y3)
{
    if (!Jest(numer))
        return -1;
    const TLine &l = Line[numer];
    float dx = l.x2 - l.x1, dy = l.y2 - l.y1;
    float d2 = dx * dx + dy * dy;
    float t = d2 > 0 ? std::clamp(((x3 - l.x1) * dx + (y3 - l.y1) * dy) / d2, 0.0f, 1.0f) : 0;
    return std::hypot(l.x1 + t * dx - x3, l.y1 + t * dy - y3);
}

void TLineC::Rys(TRysownik *dc1, long numer, float szer_min, float szer_max,
                 float wys_min, float wys_max, long warstwa)
{
    float sx = dc1->Szerokosc() / (szer_max - szer_min);
    float sy = dc1->Wysokosc() / (wys_max - wys_min);

    for (long i = 0; i < (long)Line.size(); ++i)
    {
        const TLine &l = Line[i];
        if (numer >= 0 && i != numer)
            continue;
        if (warstwa >= 0 && l.warstwa != warstwa)
            continue;
        if (!Styka(l, szer_min, wys_min, szer_max, wys_max))
            continue;
        //ekran: y rosnie w dol
        dc1->Linia((l.x1 - szer_min) * sx, (wys_max - l.y1) * sy,
                   (l.x2 - szer_min) * sx, (wys_max - l.y2) * sy, l.warstwa);
    }
}

void TLineC::Delete(long obiekt)
{
    if (Jest(obiekt))
        Line.erase(Line.begin() + obiekt);
}

void TLineC::Odznacz(void)
{
    for (TLine &l : Line)
        l.reserved = 0;
}

long TLineC::GetWarstwa(long nr_obiektu)
{
    return Jest(nr_obiektu) ? Line[nr_obiektu].warstwa : -1;
}

void TLineC::SetWarstwa(long nr_obiektu, unsigned char warstwa)
{
    if (Jest(nr_obiektu))
        Line[nr_obiektu].warstwa = warstwa;
}

bool TLineC::IsIn(long nr_obiektu, float x1, float y1, float x2, float y2, bool all, bool)
{
    if (!Jest(nr_obiektu))
        return false;
    const TLine &l = Line[nr_obiektu];
    if (!all)
        return Styka(l, x1, y1, x2, y2);
    //oba konce w prostokacie
    TLine p1 = {0, 0, 0, l.x1, l.y1, l.x1, l.y1};
    TLine p2 = {0, 0, 0, l.x2, l.y2, l.x2, l.y2};
    return Styka(p1, x1, y1, x2, y2) && Styka(p2, x1, y1, x2, y2);
}

TMaxMinOb TLineC::GetMaxMinWsp(void)
{
    if (Line.empty())
        return {0, 0, 0, 0};
    TMaxMinOb o = {Line[0].x1, Line[0].y1, Line[0].x1, Line[0].y1};
    for (const TLine &l : Line)
    {
        o.minx = std::min({o.minx, l.x1, l.x2});
        o.miny = std::min({o.miny, l.y1, l.y2});
        o.maxx = std::max({o.maxx, l.x1, l.x2});
        o.maxy = std::max({o.maxy, l.y1, l.y2});
    }
    return o;
}
