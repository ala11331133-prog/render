//
//Wirtualny obiekt reprezentujacy cechy wspolne
//wszystkich obiektow mapy.
//Wersja przenosna (C++17) na podstawie wek.h.pocz/obiekt.h:
//  - zamiast <fstream.h> jest <fstream> (strumienie przez referencje)
//  - zamiast MFC CDC jest abstrakcyjny TRysownik
//Metody "= 0" musi zaimplementowac kazdy obiekt mapy.
//

#ifndef obiekt1234567890
#define obiekt1234567890

#include <fstream>

struct FLOATPOINT
{
    float x;
    float y;
};

//
//Obiekt przechowuje wspolrzedne maksymalne i minimalne
struct TMaxMinOb
{
    float minx, miny, maxx, maxy;
};

//
//Powierzchnia rysowania (odpowiednik CDC).
//Wspolrzedne w pikselach, (0,0) to lewy gorny rog.
class TRysownik
{
public:
    virtual ~TRysownik() = default;

    //
    //Rozmiar powierzchni w pikselach
    virtual int Szerokosc(void) const = 0;
    virtual int Wysokosc(void) const = 0;

    //
    //Rysuje odcinek; warstwa pozwala dobrac kolor/grubosc
    virtual void Linia(int x1, int y1, int x2, int y2, int warstwa) = 0;
};

class obiekt
{
public:
    virtual ~obiekt() = default;

    //
    //Zwalnia pamiec
    virtual void Free(void) = 0;

    //
    //Ladowanie danych i rezerwacja pamieci
    virtual long Load(std::ifstream &zpliku) = 0;

    //
    //Zapis danych
    virtual long Save(std::ofstream &naplik) = 0;

    //
    //Oblicz odleglosc obiektu do punktu
    virtual float Distance(long numer, float x3, float y3) = 0;

    //
    //Wyswietla obiekt na wskazanym rysowniku
    virtual void Rys(TRysownik *dc1, long numer,
                     float szer_min, float szer_max, float wys_min, float wys_max,
                     long warstwa = -1) = 0;

    //
    //Wstaw obiekty z pliku
    virtual long Add(std::ifstream & /*zpliku*/) { return 0; }

    //
    //Sortowanie obiektu w celu optymalizacji wyswietlania
    virtual void Sort(void) {}

    //
    //Operacje arytmetyczne dla danego obiektu
    //Jezeli obiekt -1 operacja wykonana dla wszystkich obiektow
    //zmienna xy ustawiona na 1 zmienia x ustawiona na 2 zmienia y
    //ustawiona na 3 zmienia x i y.
    //jezeli zaznacz=1 to dziala tylko na zaznaczonych
    virtual void Arytm(long /*obiekt*/, float /*dodaj*/, float /*razy*/ = 1,
                       long /*xy*/ = 2, char /*zaznacz*/ = 0) {}

    //
    //Operacja usuwania obiektu
    virtual void Delete(long obiekt) = 0;

    //
    //Odznacza wszystkie obiekty
    virtual void Odznacz(void) = 0;

    //
    //Podaje warstwe danego obiektu
    virtual long GetWarstwa(long nr_obiektu) = 0;

    //
    //Zapisuje nowa warstwe dla danego obiektu
    virtual void SetWarstwa(long nr_obiektu, unsigned char warstwa) = 0;

    //
    //Podaje reserved danego obiektu
    virtual long GetReserved(long /*nr_obiektu*/) { return -1; }

    //
    //Zapisuje nowa wartosc reserved dla danego obiektu
    virtual void SetReserved(long /*nr_obiektu*/, unsigned char /*reserved*/) {}

    //
    //Zwraca 1 jezeli obiekt jest w obszarze zaznaczenia
    virtual bool IsIn(long nr_obiektu, float x1, float y1, float x2, float y2,
                      bool all = 0, bool widok = 1) = 0;

    //
    //Zaznacza powtarzajace sie obiekty
    virtual long Zaznacz2x(void) { return 0; }

    //
    //Wylicza i zwraca maksymalne i minimalne wspolrzedne
    virtual TMaxMinOb GetMaxMinWsp(void)
    {
        TMaxMinOb a = {0, 0, 0, 0};
        return a;
    }
};

#endif
