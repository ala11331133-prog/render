#ifndef Line12345678
#define Line12345678

#include <cstdint>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "obiekt.h"
#include "osm_odczyt.h"

enum TWarstwa
{
    W_INNE = 0,
    W_DROGA_GLOWNA,
    W_DROGA_SREDNIA,
    W_ULICA,
    W_PIESZA,
    W_KOLEJ,
    W_BUDYNEK,
    W_WODA,
    W_ILOSC
};

//reserved: 0 nie zaznaczony, 1 widoczny, 254 zaznaczony, 255 do skasowania
struct TLine
{
    unsigned char reserved;
    int32_t TextIndex;
    unsigned char warstwa;
    float x1, y1, x2, y2;   //x = lon, y = lat
};

class TLineC : public obiekt
{
public:
    //Drogi -> odcinki (n punktow -> n-1 odcinkow)
    const std::vector<TLine> &Utworz(const std::vector<TOsmDroga> &drogi);

    //Odcinki stykajace sie z oknem, warstwa -1 = wszystkie
    std::vector<TLine> Pobierz(float x1, float y1, float x2, float y2, int warstwa = -1) const;

    const std::vector<TLine> &Wszystkie(void) const { return Line; }

    static int WarstwaZTagow(const std::vector<std::pair<std::string, std::string>> &tagi);

    void Free(void) override;
    long Load(std::ifstream &zpliku) override;
    long Save(std::ofstream &naplik) override;
    float Distance(long numer, float x3, float y3) override;
    void Rys(TRysownik *dc1, long numer, float szer_min, float szer_max,
             float wys_min, float wys_max, long warstwa = -1) override;
    void Delete(long obiekt) override;
    void Odznacz(void) override;
    long GetWarstwa(long nr_obiektu) override;
    void SetWarstwa(long nr_obiektu, unsigned char warstwa) override;
    bool IsIn(long nr_obiektu, float x1, float y1, float x2, float y2,
              bool all = 0, bool widok = 1) override;
    TMaxMinOb GetMaxMinWsp(void) override;

private:
    std::vector<TLine> Line;

    bool Jest(long nr) const { return nr >= 0 && nr < (long)Line.size(); }
};

#endif
