//Czytanie drog z pliku .osm. Na TLine zamienia je dopiero TLineC.

#ifndef OsmOdczyt12345678
#define OsmOdczyt12345678

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

//x = lon, y = lat
struct TOsmPunkt
{
    float x, y;
};

struct TOsmDroga
{
    std::vector<TOsmPunkt> punkty;
    std::vector<std::pair<std::string, std::string>> tagi;
};

//Prosty parser - dziala na eksporcie z openstreetmap.org, gdzie kazdy element jest w osobnej linii.
class TOsmOdczyt
{
public:
    //Dopisuje drogi do wynik. Jak droga wychodzi poza wycinek, to dzieli sie na kawalki.
    //Zwraca ile drog doszlo, -1 jak nie ma pliku.
    long Wczytaj(const char *nazwa_pliku, std::vector<TOsmDroga> &wynik);

private:
    std::unordered_map<int64_t, TOsmPunkt> wezly_;

    void DodajDroge(const std::vector<int64_t> &refy,
                    const std::vector<std::pair<std::string, std::string>> &tagi,
                    std::vector<TOsmDroga> &wynik) const;

    static bool JestTag(const std::string &linia, const char *tag);

    static bool Atrybut(const std::string &linia, const char *nazwa, std::string &wynik);

    //&quot; -> " itd.
    static void Encje(std::string &s);
};

#endif
