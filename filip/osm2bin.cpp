// Robi plik .bin z .osm, zeby potem nie parsowac XML za kazdym razem.
// Uzycie: ./osm2bin map.osm linie.bin
#include <iostream>

#include "linie.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "uzycie: " << argv[0] << " plik.osm plik.bin\n";
        return 1;
    }

    std::vector<TOsmDroga> drogi;
    TOsmOdczyt odczyt;
    if (odczyt.Wczytaj(argv[1], drogi) < 0)
    {
        std::cout << "nie moge otworzyc " << argv[1] << '\n';
        return 1;
    }

    TLineC dane;
    dane.Utworz(drogi);

    std::ofstream plik(argv[2], std::ios::binary);
    long n = dane.Save(plik);
    if (n < 0)
    {
        std::cout << "nie udalo sie zapisac " << argv[2] << '\n';
        return 1;
    }
    std::cout << argv[2] << ": " << n << " odcinkow z " << drogi.size() << " drog\n";
    return 0;
}
