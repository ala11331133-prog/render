// Test TOsmOdczyt i TLineC. Najpierw maly plik z recznie wpisanymi wartosciami,
// potem cala map.osm porownana z tym, co wyciagnal wzorzec_osm.py.
// Uzycie: ./test_odczyt map.osm map_wzorzec.txt (albo po prostu make test)
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linie.h"

static int bledy = 0, sprawdzenia = 0;

#define SPRAWDZ(war)                                                          \
    do                                                                        \
    {                                                                         \
        ++sprawdzenia;                                                        \
        if (!(war))                                                           \
        {                                                                     \
            ++bledy;                                                          \
            std::cout << "  BLAD " << __FILE__ << ":" << __LINE__ << ": " #war \
                      << '\n';                                                \
        }                                                                     \
    } while (0)

static bool Blisko(float a, double b, double tol = 1e-5)
{
    return std::fabs((double)a - b) <= tol;
}

static bool Rowne(const TLine &l, float x1, float y1, float x2, float y2)
{
    return Blisko(l.x1, x1) && Blisko(l.y1, y1) && Blisko(l.x2, x2) && Blisko(l.y2, y2);
}

class TZapis : public TRysownik
{
public:
    struct TOdc
    {
        int x1, y1, x2, y2, w;
    };
    std::vector<TOdc> odc;
    int Szerokosc(void) const override { return 800; }
    int Wysokosc(void) const override { return 600; }
    void Linia(int x1, int y1, int x2, int y2, int w) override { odc.push_back({x1, y1, x2, y2, w}); }
};

static const char *MALY_OSM = R"(<?xml version="1.0" encoding="UTF-8"?>
<osm version="0.6">
 <node id="1" visible="true" uid="999" lat="50.0000000" lon="19.0000000"/>
 <node id="2" visible="true" uid="1" lat="50.0010000" lon="19.0010000"/>
 <node id="3" visible="true" lat="50.0020000" lon="19.0000000">
  <tag k="highway" v="crossing"/>
 </node>
 <node id="4" lat="50.0030000" lon="19.0030000"/>
 <node id="5" lat="50.0040000" lon="19.0040000"/>
 <way id="100" visible="true">
  <nd ref="1"/>
  <nd ref="2"/>
  <nd ref="3"/>
  <tag k="highway" v="primary"/>
  <tag k="name" v="Aleja &quot;A&amp;B&quot; &lt;1&gt; &apos;x&apos;&#10;&#x141;&#243;dz &nbsp; &amp"/>
 </way>
 <way id="101">
  <nd ref="1"/>
  <nd ref="2"/>
  <nd ref="777"/>
  <nd ref="4"/>
  <nd ref="5"/>
  <tag k="name" v="Z dziura"/>
  <tag k="railway" v="tram"/>
 </way>
 <way id="102">
  <nd ref="4"/>
  <tag k="building" v="yes"/>
 </way>
 <way id="103">
  <nd ref="1"/>
  <nd ref="888"/>
  <nd ref="3"/>
 </way>
 <relation id="500">
  <member type="way" ref="100" role=""/>
  <tag k="type" v="route"/>
 </relation>
</osm>
)";

static void TestMalyPlik(void)
{
    std::cout << "== maly plik .osm\n";
    const char *nazwa = "test_maly.osm";
    {
        std::ofstream f(nazwa);
        f << MALY_OSM;
    }

    std::vector<TOsmDroga> drogi;
    TOsmOdczyt odczyt;
    long n = odczyt.Wczytaj(nazwa, drogi);

    //way 101 ma dziure, wiec daje 2 kawalki; 102 i 103 maja za malo punktow
    SPRAWDZ(n == 3);
    SPRAWDZ(drogi.size() == 3);
    if (drogi.size() == 3)
    {
        const TOsmDroga &d = drogi[0];
        SPRAWDZ(d.punkty.size() == 3);
        if (d.punkty.size() == 3)
        {
            SPRAWDZ(Blisko(d.punkty[0].x, 19.000) && Blisko(d.punkty[0].y, 50.000));
            SPRAWDZ(Blisko(d.punkty[1].x, 19.001) && Blisko(d.punkty[1].y, 50.001));
            SPRAWDZ(Blisko(d.punkty[2].x, 19.000) && Blisko(d.punkty[2].y, 50.002));
        }
        SPRAWDZ(d.tagi.size() == 2);
        if (d.tagi.size() == 2)
        {
            SPRAWDZ(d.tagi[0].first == "highway" && d.tagi[0].second == "primary");
            SPRAWDZ(d.tagi[1].first == "name" && d.tagi[1].second == "Aleja \"A&B\" <1> 'x'\n\u0141\u00f3dz &nbsp; &amp");
        }

        SPRAWDZ(drogi[1].punkty.size() == 2 && drogi[2].punkty.size() == 2);
        if (drogi[1].punkty.size() == 2 && drogi[2].punkty.size() == 2)
        {
            SPRAWDZ(Blisko(drogi[1].punkty[1].x, 19.001));
            SPRAWDZ(Blisko(drogi[2].punkty[0].x, 19.003) && Blisko(drogi[2].punkty[1].y, 50.004));
        }
        SPRAWDZ(drogi[1].tagi == drogi[2].tagi && drogi[1].tagi.size() == 2);
    }

    std::vector<TOsmDroga> puste;
    SPRAWDZ(odczyt.Wczytaj("nie_ma_takiego_pliku.osm", puste) == -1);
    SPRAWDZ(puste.empty());

    SPRAWDZ(odczyt.Wczytaj(nazwa, drogi) == 3);
    SPRAWDZ(drogi.size() == 6);
    drogi.resize(3);

    TLineC dane;
    const std::vector<TLine> &linie = dane.Utworz(drogi);
    SPRAWDZ(linie.size() == 4);
    if (linie.size() == 4)
    {
        SPRAWDZ(linie[0].warstwa == W_DROGA_GLOWNA && linie[1].warstwa == W_DROGA_GLOWNA);
        SPRAWDZ(linie[2].warstwa == W_KOLEJ && linie[3].warstwa == W_KOLEJ);
        SPRAWDZ(Rowne(linie[0], 19.000f, 50.000f, 19.001f, 50.001f));
        SPRAWDZ(Rowne(linie[1], 19.001f, 50.001f, 19.000f, 50.002f));
        SPRAWDZ(Rowne(linie[3], 19.003f, 50.003f, 19.004f, 50.004f));
        SPRAWDZ(linie[0].reserved == 0 && linie[0].TextIndex == -1);
    }

    TMaxMinOb o = dane.GetMaxMinWsp();
    SPRAWDZ(Blisko(o.minx, 19.000) && Blisko(o.maxx, 19.004));
    SPRAWDZ(Blisko(o.miny, 50.000) && Blisko(o.maxy, 50.004));

    //okno wokol punktu 2, rogi w obie strony
    SPRAWDZ(dane.Pobierz(19.0005f, 50.0015f, 19.0015f, 50.0005f).size() == 3);
    SPRAWDZ(dane.Pobierz(19.0015f, 50.0005f, 19.0005f, 50.0015f).size() == 3);
    SPRAWDZ(dane.Pobierz(19.0005f, 50.0015f, 19.0015f, 50.0005f, W_KOLEJ).size() == 1);
    SPRAWDZ(dane.Pobierz(19.0005f, 50.0015f, 19.0015f, 50.0005f, W_WODA).empty());
    SPRAWDZ(dane.Pobierz(20.0f, 51.0f, 21.0f, 50.5f).empty());
    SPRAWDZ(dane.Pobierz(18.0f, 51.0f, 20.0f, 49.0f).size() == 4);

    obiekt *ob = &dane;
    SPRAWDZ(Blisko(ob->Distance(0, 19.000f, 50.000f), 0.0));
    SPRAWDZ(Blisko(ob->Distance(3, 19.004f, 50.005f), 0.001));
    SPRAWDZ(ob->Distance(99, 0, 0) == -1);
    SPRAWDZ(ob->GetWarstwa(2) == W_KOLEJ);
    SPRAWDZ(ob->GetWarstwa(-1) == -1 && ob->GetWarstwa(4) == -1);
    SPRAWDZ(ob->IsIn(0, 18.9f, 49.9f, 19.1f, 50.1f, 1));
    SPRAWDZ(ob->IsIn(0, 19.0005f, 49.9f, 19.1f, 50.1f, 0));
    SPRAWDZ(!ob->IsIn(0, 19.0005f, 49.9f, 19.1f, 50.1f, 1));

    //okno takie jak odcinek, wiec powinna wyjsc przekatna ekranu
    TZapis r;
    ob->Rys(&r, 3, 19.003f, 19.004f, 50.003f, 50.004f);
    SPRAWDZ(r.odc.size() == 1);
    if (r.odc.size() == 1)
    {
        SPRAWDZ(std::abs(r.odc[0].x1 - 0) <= 1 && std::abs(r.odc[0].y1 - 600) <= 1);
        SPRAWDZ(std::abs(r.odc[0].x2 - 800) <= 1 && std::abs(r.odc[0].y2 - 0) <= 1);
        SPRAWDZ(r.odc[0].w == W_KOLEJ);
    }
    r.odc.clear();
    ob->Rys(&r, -1, 18.0f, 20.0f, 49.0f, 51.0f, W_DROGA_GLOWNA);
    SPRAWDZ(r.odc.size() == 2);

    {
        std::ofstream f("test_maly.bin", std::ios::binary);
        SPRAWDZ(ob->Save(f) == 4);
    }
    TLineC z_bin;
    {
        std::ifstream f("test_maly.bin", std::ios::binary);
        SPRAWDZ(z_bin.Load(f) == 4);
    }
    SPRAWDZ(z_bin.Wszystkie().size() == 4);
    for (size_t i = 0; i < z_bin.Wszystkie().size() && i < linie.size(); ++i)
    {
        const TLine &a = linie[i], &b = z_bin.Wszystkie()[i];
        SPRAWDZ(a.warstwa == b.warstwa && a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2);
    }

    //obciety plik
    {
        std::ofstream f("test_maly.bin", std::ios::binary);
        uint32_t ilosc = 1000;
        f.write((const char *)&ilosc, sizeof(ilosc));
    }
    {
        std::ifstream f("test_maly.bin", std::ios::binary);
        SPRAWDZ(z_bin.Load(f) == -1);
    }
    SPRAWDZ(z_bin.Wszystkie().empty());

    ob->SetWarstwa(0, W_WODA);
    SPRAWDZ(ob->GetWarstwa(0) == W_WODA);
    ob->Delete(0);
    SPRAWDZ(dane.Wszystkie().size() == 3 && ob->GetWarstwa(0) == W_DROGA_GLOWNA);
    ob->Delete(50);
    SPRAWDZ(dane.Wszystkie().size() == 3);
    ob->Odznacz();
    SPRAWDZ(dane.Wszystkie()[0].reserved == 0);
    ob->Free();
    SPRAWDZ(dane.Wszystkie().empty());
    TMaxMinOb pusty = ob->GetMaxMinWsp();
    SPRAWDZ(pusty.minx == 0 && pusty.maxy == 0);

    std::remove(nazwa);
    std::remove("test_maly.bin");
}

static void TestWarstwy(void)
{
    std::cout << "== warstwy z tagow\n";
    using T = std::vector<std::pair<std::string, std::string>>;
    SPRAWDZ(TLineC::WarstwaZTagow({}) == W_INNE);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"highway", "motorway"}}) == W_DROGA_GLOWNA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"highway", "trunk_link"}}) == W_DROGA_GLOWNA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"highway", "tertiary"}}) == W_DROGA_SREDNIA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"highway", "service"}}) == W_ULICA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"highway", "steps"}}) == W_PIESZA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"railway", "tram"}}) == W_KOLEJ);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"building", "yes"}}) == W_BUDYNEK);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"waterway", "stream"}}) == W_WODA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"natural", "water"}}) == W_WODA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"natural", "wood"}}) == W_INNE);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"name", "X"}, {"building", "yes"}}) == W_BUDYNEK);
    //pierwszy rozpoznany tag wygrywa
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"highway", "primary"}, {"building", "yes"}}) == W_DROGA_GLOWNA);
    SPRAWDZ(TLineC::WarstwaZTagow(T{{"building", "yes"}, {"highway", "primary"}}) == W_BUDYNEK);
}

struct TWzorzec
{
    std::string id;
    std::vector<std::pair<double, double>> punkty;
    std::vector<std::pair<std::string, std::string>> tagi;
};

static bool CzytajWzorzec(const char *nazwa, std::vector<TWzorzec> &wynik)
{
    std::ifstream f(nazwa, std::ios::binary);
    if (!f)
        return false;
    std::string linia;
    while (std::getline(f, linia))
    {
        TWzorzec w;
        size_t np = 0, nt = 0;
        char c;
        std::istringstream s(linia);
        if (!(s >> c >> np >> nt >> w.id) || c != 'D')
            return false;
        for (size_t i = 0; i < np; ++i)
        {
            std::getline(f, linia);
            std::istringstream p(linia);
            double lon, lat;
            if (!(p >> c >> lon >> lat) || c != 'P')
                return false;
            w.punkty.push_back({lon, lat});
        }
        for (size_t i = 0; i < nt; ++i)
        {
            std::getline(f, linia);
            std::istringstream t(linia);
            size_t kl, vl;
            if (!(t >> c >> kl >> vl) || c != 'T')
                return false;
            std::string k(kl, '\0'), v(vl, '\0');
            f.read(&k[0], kl);
            f.read(&v[0], vl);
            f.get();
            w.tagi.push_back({k, v});
        }
        wynik.push_back(std::move(w));
    }
    return true;
}

static void TestMapa(const char *osm, const char *wzorzec_plik)
{
    std::cout << "== " << osm << " kontra " << wzorzec_plik << '\n';

    std::vector<TWzorzec> wzor;
    SPRAWDZ(CzytajWzorzec(wzorzec_plik, wzor));
    SPRAWDZ(!wzor.empty());

    std::vector<TOsmDroga> drogi;
    TOsmOdczyt odczyt;
    long n = odczyt.Wczytaj(osm, drogi);
    std::cout << "  drog: odczyt " << n << ", wzorzec " << wzor.size() << '\n';
    SPRAWDZ(n == (long)wzor.size());
    SPRAWDZ(drogi.size() == wzor.size());

    //wypisujemy tylko kilka pierwszych roznic, zeby nie zalac ekranu
    int zle_punkty = 0, zle_tagi = 0;
    size_t ile = std::min(drogi.size(), wzor.size());
    for (size_t i = 0; i < ile; ++i)
    {
        const TOsmDroga &d = drogi[i];
        const TWzorzec &w = wzor[i];

        bool ok = d.punkty.size() == w.punkty.size();
        for (size_t j = 0; ok && j < d.punkty.size(); ++j)
            ok = Blisko(d.punkty[j].x, w.punkty[j].first) && Blisko(d.punkty[j].y, w.punkty[j].second);
        if (!ok && ++zle_punkty <= 5)
            std::cout << "  way " << w.id << ": inne punkty\n";

        if (d.tagi != w.tagi && ++zle_tagi <= 5)
        {
            std::cout << "  way " << w.id << ": inne tagi\n";
            for (size_t j = 0; j < d.tagi.size() && j < w.tagi.size(); ++j)
                if (d.tagi[j] != w.tagi[j])
                    std::cout << "    jest   " << d.tagi[j].first << "=" << d.tagi[j].second << '\n'
                              << "    powinno " << w.tagi[j].first << "=" << w.tagi[j].second << '\n';
        }
    }
    std::cout << "  drogi z innymi punktami: " << zle_punkty << ", z innymi tagami: " << zle_tagi << '\n';
    SPRAWDZ(zle_punkty == 0);
    SPRAWDZ(zle_tagi == 0);

    TLineC dane;
    const std::vector<TLine> &linie = dane.Utworz(drogi);
    std::vector<TLine> oczek;
    int warstwy[W_ILOSC] = {0};
    for (const TWzorzec &w : wzor)
    {
        unsigned char wa = TLineC::WarstwaZTagow(w.tagi);
        for (size_t j = 1; j < w.punkty.size(); ++j)
        {
            oczek.push_back({0, -1, wa, (float)w.punkty[j - 1].first, (float)w.punkty[j - 1].second,
                             (float)w.punkty[j].first, (float)w.punkty[j].second});
            ++warstwy[wa];
        }
    }
    std::cout << "  odcinkow: " << linie.size() << ", oczekiwano " << oczek.size() << '\n';
    for (int i = 0; i < W_ILOSC; ++i)
        std::cout << "    warstwa " << i << ": " << warstwy[i] << '\n';
    SPRAWDZ(linie.size() == oczek.size());
    int zle_odc = 0;
    for (size_t i = 0; i < linie.size() && i < oczek.size(); ++i)
        if (linie[i].warstwa != oczek[i].warstwa ||
            !Rowne(linie[i], oczek[i].x1, oczek[i].y1, oczek[i].x2, oczek[i].y2))
            ++zle_odc;
    SPRAWDZ(zle_odc == 0);

    TMaxMinOb o = dane.GetMaxMinWsp();
    double minx = 1e9, miny = 1e9, maxx = -1e9, maxy = -1e9;
    for (const TWzorzec &w : wzor)
        for (auto &[x, y] : w.punkty)
        {
            minx = std::min(minx, x), maxx = std::max(maxx, x);
            miny = std::min(miny, y), maxy = std::max(maxy, y);
        }
    std::cout << "  obszar: x " << o.minx << ".." << o.maxx << "  y " << o.miny << ".." << o.maxy << '\n';
    SPRAWDZ(Blisko(o.minx, minx) && Blisko(o.maxx, maxx) && Blisko(o.miny, miny) && Blisko(o.maxy, maxy));

    float wx1 = 19.890f, wy1 = 50.076f, wx2 = 19.895f, wy2 = 50.073f;   //lewa gora, prawy dol
    size_t w_oknie = 0, w_oknie_ulice = 0;
    for (const TLine &l : oczek)
    {
        bool styka = std::max(l.x1, l.x2) >= wx1 && std::min(l.x1, l.x2) <= wx2 &&
                     std::max(l.y1, l.y2) >= wy2 && std::min(l.y1, l.y2) <= wy1;
        w_oknie += styka;
        w_oknie_ulice += styka && l.warstwa == W_ULICA;
    }
    std::vector<TLine> okno = dane.Pobierz(wx1, wy1, wx2, wy2);
    std::cout << "  w oknie: " << okno.size() << " (oczekiwano " << w_oknie << ")\n";
    SPRAWDZ(okno.size() == w_oknie);
    SPRAWDZ(dane.Pobierz(wx1, wy1, wx2, wy2, W_ULICA).size() == w_oknie_ulice);

    {
        std::ofstream f("test_mapa.bin", std::ios::binary);
        SPRAWDZ(dane.Save(f) == (long)linie.size());
    }
    TLineC z_bin;
    {
        std::ifstream f("test_mapa.bin", std::ios::binary);
        SPRAWDZ(z_bin.Load(f) == (long)linie.size());
    }
    bool takie_same = z_bin.Wszystkie().size() == linie.size();
    for (size_t i = 0; takie_same && i < linie.size(); ++i)
    {
        const TLine &a = linie[i], &b = z_bin.Wszystkie()[i];
        takie_same = a.warstwa == b.warstwa && a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2;
    }
    SPRAWDZ(takie_same);
    std::remove("test_mapa.bin");
}

int main(int argc, char **argv)
{
    TestMalyPlik();
    TestWarstwy();
    if (argc > 2)
        TestMapa(argv[1], argv[2]);
    else
        std::cout << "(pominieto test mapy - uzycie: " << argv[0] << " map.osm map_wzorzec.txt)\n";

    std::cout << "\nsprawdzen: " << sprawdzenia << ", bledow: " << bledy << '\n';
    std::cout << (bledy ? "TEST NIEUDANY\n" : "TEST OK\n");
    return bledy ? 1 : 0;
}
