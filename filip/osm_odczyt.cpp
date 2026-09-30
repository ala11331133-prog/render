#include "osm_odczyt.h"

#include <cstdlib>
#include <fstream>

long TOsmOdczyt::Wczytaj(const char *nazwa_pliku, std::vector<TOsmDroga> &wynik)
{
    std::ifstream plik(nazwa_pliku);
    if (!plik)
        return -1;

    wezly_.clear();
    const size_t start = wynik.size();

    bool w_drodze = false;
    std::vector<int64_t> refy;
    std::vector<std::pair<std::string, std::string>> tagi;
    std::string linia, wartosc, klucz;

    while (std::getline(plik, linia))
    {
        if (JestTag(linia, "<node "))
        {
            //node'y sa w pliku przed way'ami, wiec jeden przebieg wystarczy
            if (Atrybut(linia, "id", wartosc))
            {
                int64_t id = std::strtoll(wartosc.c_str(), nullptr, 10);
                std::string lat, lon;
                if (Atrybut(linia, "lat", lat) && Atrybut(linia, "lon", lon))
                    wezly_[id] = {std::strtof(lon.c_str(), nullptr),
                                  std::strtof(lat.c_str(), nullptr)};
            }
        }
        else if (JestTag(linia, "<way "))
        {
            w_drodze = true;
            refy.clear();
            tagi.clear();
        }
        else if (w_drodze && JestTag(linia, "<nd "))
        {
            if (Atrybut(linia, "ref", wartosc))
                refy.push_back(std::strtoll(wartosc.c_str(), nullptr, 10));
        }
        else if (w_drodze && JestTag(linia, "<tag "))
        {
            if (Atrybut(linia, "k", klucz) && Atrybut(linia, "v", wartosc))
                tagi.emplace_back(klucz, wartosc);
        }
        else if (w_drodze && JestTag(linia, "</way>"))
        {
            DodajDroge(refy, tagi, wynik);
            w_drodze = false;
        }
    }

    wezly_.clear();
    return (long)(wynik.size() - start);
}

void TOsmOdczyt::DodajDroge(const std::vector<int64_t> &refy,
                            const std::vector<std::pair<std::string, std::string>> &tagi,
                            std::vector<TOsmDroga> &wynik) const
{
    TOsmDroga droga;
    droga.tagi = tagi;

    for (int64_t ref : refy)
    {
        auto w = wezly_.find(ref);
        if (w != wezly_.end())
        {
            droga.punkty.push_back(w->second);
            continue;
        }
        //tego wezla nie ma w wycinku - konczymy kawalek
        if (droga.punkty.size() >= 2)
            wynik.push_back(droga);
        droga.punkty.clear();
    }
    if (droga.punkty.size() >= 2)
        wynik.push_back(std::move(droga));
}

bool TOsmOdczyt::JestTag(const std::string &linia, const char *tag)
{
    size_t p = linia.find_first_not_of(" \t");
    return p != std::string::npos &&
           linia.compare(p, std::char_traits<char>::length(tag), tag) == 0;
}

bool TOsmOdczyt::Atrybut(const std::string &linia, const char *nazwa, std::string &wynik)
{
    //spacja przed nazwa, bo inaczej "id" trafi w "uid"
    std::string wzor = std::string(" ") + nazwa + "=\"";
    size_t p = linia.find(wzor);
    if (p == std::string::npos)
        return false;
    p += wzor.size();
    size_t k = linia.find('"', p);
    if (k == std::string::npos)
        return false;
    wynik.assign(linia, p, k - p);
    Encje(wynik);
    return true;
}

static void DodajUtf8(std::string &s, unsigned long c)
{
    if (c < 0x80)
        s += (char)c;
    else if (c < 0x800)
    {
        s += (char)(0xC0 | (c >> 6));
        s += (char)(0x80 | (c & 0x3F));
    }
    else if (c < 0x10000)
    {
        s += (char)(0xE0 | (c >> 12));
        s += (char)(0x80 | ((c >> 6) & 0x3F));
        s += (char)(0x80 | (c & 0x3F));
    }
    else
    {
        s += (char)(0xF0 | (c >> 18));
        s += (char)(0x80 | ((c >> 12) & 0x3F));
        s += (char)(0x80 | ((c >> 6) & 0x3F));
        s += (char)(0x80 | (c & 0x3F));
    }
}

void TOsmOdczyt::Encje(std::string &s)
{
    if (s.find('&') == std::string::npos)
        return;

    static const std::pair<const char *, char> nazwane[] = {
        {"quot", '"'}, {"amp", '&'}, {"lt", '<'}, {"gt", '>'}, {"apos", '\''}};

    std::string wynik;
    size_t i = 0;
    while (i < s.size())
    {
        size_t k = s[i] == '&' ? s.find(';', i) : std::string::npos;
        if (k == std::string::npos)
        {
            wynik += s[i++];
            continue;
        }
        std::string nazwa(s, i + 1, k - i - 1);
        bool znana = false;
        if (nazwa.size() > 1 && nazwa[0] == '#')
        {
            bool hex = nazwa[1] == 'x' || nazwa[1] == 'X';
            const char *cyfry = nazwa.c_str() + (hex ? 2 : 1);
            char *koniec = nullptr;
            unsigned long c = std::strtoul(cyfry, &koniec, hex ? 16 : 10);
            if (*cyfry && *koniec == '\0' && c <= 0x10FFFF)
            {
                DodajUtf8(wynik, c);
                znana = true;
            }
        }
        else
            for (const auto &[n, z] : nazwane)
                if (nazwa == n)
                {
                    wynik += z;
                    znana = true;
                    break;
                }
        if (znana)
            i = k + 1;
        else
            wynik += s[i++];   //np. &nbsp; - zostawiamy jak jest
    }
    s = std::move(wynik);
}
