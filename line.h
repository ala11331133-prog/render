//
//Klasa obslugujaca obiekty typu linie w plikach wektorowych
//Dolaczona jako czasc skladowa klasy wektor
//

#pragma once

#include <fstream>
#include <vector>
#include "obiekt.h"

//
//Struktura do zapisu linii jako punktu i generowania polaczen
struct TWektor_sort
{
    long reserved;
    unsigned long nrkreski;
    float x,y;
};

//
//Tablica polaczen
//Zachowane dane ilosc polaczen,warstwa,dlugosc odcinka,polaczenia
struct TPol
{
	unsigned char warstwa;
	float odl;
	float waga;
	long *pol;
};


//
//Dane lini
//Obiekt o identyfikatorze 0
#pragma pack(push, 1)
struct TLine
{
	TLine() : TLine(0, 0, 0, 0, 0, 0, 0){};
	TLine(std::uint8_t reserved, std::int32_t TextIndex,
		std::uint8_t warstwa, float x1, float y1,
		float x2, float y2)
	{
		this->reserved = reserved;
		this->TextIndex = TextIndex;
		this->warstwa = warstwa;
		this->x1 = x1;
		this->y1 = y1;
		this->x2 = x2;
		this->y2 = y2;
	}	
//
	//Pole dodatkowe oznaczenia wartosci:
	//Wartosc 0 nie zaznaczony
	//Wartosc 1 widoczny na ekranie
	//Wartosc 254 zaznaczony
	//Wartosc 255 przeznaczony do SKASOWANIA
	std::uint8_t reserved;

	//
	//Index do tablicy zawirajacej teksty
	std::int32_t TextIndex;

	//
	//Opis warstwy
	std::uint8_t warstwa;

	//
	//Punkt wstawienia linii
	float x1,y1,x2,y2;
};
#pragma pack(pop)

static_assert(sizeof(TLine) == 22);

class TLineC: public obiekt<TLine>
{
public:

	//wazne ustawiac do klas po kolei
	ObjectType TypeId = ObjectType::Line;

	TLineC(void);
	~TLineC(void);
	
	//
	//Zwalinia pamiec
	void Free(void);

	//
    //Zwraca 1 jak obiekty sa rowne 0 jak nie
	int Equal(TLine a, TLine b);

	//
	//Ladowanie danych i rezerwacja pamieci
	size_t Load(const std::vector<TLine> &data);

	//
	//Zapis danych
	size_t Save(std::vector<TLine> &data);

	//
	//Oblicz odleglosc obiektu do punktu
	float Distance(long numer,float x3,float y3);

	//
	//Wyswietla linie na wskazanym dc
	void Draw(long numer,
			float szer_min,float szer_max,float wys_min,float wys_max,long warstwa=-1);


	//
	//Dodaje element
	size_t Add(TLine line);

	//
	//Dodaje elementy
	size_t Add(const std::vector<TLine>& data);

	//
	//Sortowanie obiektu w celu optymalizacji wyswietlania
	void Sort(void);

	//
	//Operacje arytmetyczne dla dango obiektu
	//Jezeli obiekt mniejszy od zera operacja wykonana dla wszystkich obiektow
	//zmienna xy ustawiona na 1 zmienia x ustawiona na 2 zmienia y
	//ustawiona na 3 zmienia x i y.
	//jezeli zaznacz=1 to dziala tylko na zaznaczonych
	void Arytm(long obiekt,float dodaj,float razy=1,long xy=3,char zaznacz=0);

	//
	//Operacja usuwania obiektu
	void Delete(long obiekt);

	//
	//Odznacza wszystkie obiekty
	void Deselect(void);
	
	//
	//Podaje warstwe danego obiektu
	long GetLayer(long nr_obiektu);

	//
	//Zapisuje nowa warstwe dla danego obiektu
	void SetLayer(long nr_obiektu,unsigned char warstwa);

	//
	//Podaje reserved danego obiektu
	long GetSelect(long nr_obiektu);

	//
	//Zapisuje nowa wartosc reserved dla danego obiektu
	void SetSelect(long nr_obiektu,unsigned char reserved);

	//
	//Zwraca 1 jezeli obiekt przechodci przez obszar zaznaczenia
	std::vector<TLine> GetCross(float x1, float y1, float x2, float y2);

	//
	//Zwraca 1 jezeli obiekt jest w obszarze zaznaczenia
	//cross jezeli 0 sprawdza czy caly obiekt jest w srodku 
	//jezeli 1 czy dowolna czesc jest w srodku
	std::vector<TLine> GetIn(float x1, float y1, float x2, float y2, bool cross);

	//
	//Wylicza i zwraca maxymalne i minimalne wspolrzedne
	TMaxMinOb GetMaxMinWsp(void);

	//zwraca typ objektu
	ObjectType GetObjectType(void);

	//
	//zwraca ilosc bajtow jednego objektu
	size_t ObSize(void);

	//zwraca ilosc objektow
	size_t Size(void);

	//
	//Obiekty o identyfikatorze 0
	std::vector<TLine> Line;
};
