//
//Klasa obslugujaca obiekty typu point w plikach wektorowych
//Dolaczona jako czasc skladowa klasy wektor
//

#pragma once

#include <fstream>
#include <vector>
#include "obiekt_draw.h"
#include "irenderer.hpp"

//
//Dane lini
//Obiekt o identyfikatorze 0
#pragma pack(push, 1)
struct TPoint
{
	TPoint() : TPoint(0, 0, 0, 0, 0) {};
	TPoint(std::uint8_t reserved, std::int32_t TextIndex,
		std::uint8_t warstwa, float x1, float y1)
	{
		this->reserved = reserved;
		this->TextIndex = TextIndex;
		this->warstwa = warstwa;
		this->x1 = x1;
		this->y1 = y1;
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
	float x1, y1;
};
#pragma pack(pop)

static_assert(sizeof(TPoint) == 14);

class TPointC : public obiekt_draw<TPoint>
{
public:

	//wazne ustawiac do klas po kolei
	ObjectType TypeId = ObjectType::Point;

	TPointC(void);
	~TPointC(void);

	//
	//Zwalinia pamiec
	void Free(void) override;

	//
	//Zwraca 1 jak obiekty sa rowne 0 jak nie
	int Equal(TPoint a, TPoint b) override;

	//
	//Ladowanie danych i rezerwacja pamieci
	size_t Load(const std::vector<TPoint>& data) override;

	//
	//Zapis danych
	size_t Save(std::vector<TPoint>& data) override;

	//
	//zwraca rekord w bajtach
	std::vector<uint8_t> Get(size_t nr) override;

	//
	//zapisuje rekord w bajtach
	bool Set(size_t nr, std::vector<uint8_t>& data) override;


	//
	//Oblicz odleglosc obiektu do punktu
	float Distance(long numer, float x3, float y3) override;

	//
	//Wyswietla linie na wskazanym dc
	void Draw(IRenderer& render,
		float srodek_x, float srodek_y, float zoom, int szerokosc,
		int wysokosc, long warstwa = -1) override;

	//
	//Dodaje element
	size_t Add(TPoint point) override;

	//
	//Dodaje elementy
	size_t Add(const std::vector<TPoint>& data) override;

	static bool ComparePointLayer(const TPoint& a, const TPoint& b);
	static bool ComparePoint(const TPoint& a, const TPoint& b);
	static bool ComparePointTextIndex(const TPoint& a, const TPoint& b);

	//
	//Sortowanie obiektu w celu optymalizacji wyswietlania
	void Sort(std::int8_t SortMethod = 0) override;

	//
	//Operacje arytmetyczne dla dango obiektu
	//Jezeli obiekt mniejszy od zera operacja wykonana dla wszystkich obiektow
	//zmienna xy ustawiona na 1 zmienia x ustawiona na 2 zmienia y
	//ustawiona na 3 zmienia x i y.
	//jezeli zaznacz=1 to dziala tylko na zaznaczonych
	void Arytm(long obiekt, float dodaj, float razy = 1, long xy = 3, char zaznacz = 0) override;

	//
	//Operacja usuwania obiektu
	void Delete(long obiekt) override;

	//
	//Odznacza wszystkie obiekty
	void Deselect(void) override;

	//
	//Podaje warstwe danego obiektu
	long GetLayer(long nr_obiektu) override;

	//
	//Zapisuje nowa warstwe dla danego obiektu
	void SetLayer(long nr_obiektu, uint8_t warstwa) override;

	//
	//Podaje reserved danego obiektu
	long GetSelect(long nr_obiektu) override;

	//
	//Zapisuje nowa wartosc reserved dla danego obiektu
	void SetSelect(long nr_obiektu, uint8_t reserved) override;

	//
	//Zwraca 1 jezeli obiekt przechodci przez obszar zaznaczenia
	bool IsCross(const TPoint& point, float x1, float y1, float x2, float y2);

	//
	//Zwraca 1 jezeli obiekt jest w obszarze zaznaczenia
	//cross jezeli 0 sprawdza czy caly obiekt jest w srodku 
	//jezeli 1 czy dowolna czesc jest w srodku
	using obiekt_draw<TPoint>::IsIn;
	bool IsIn(const TPoint& point, float x1, float y1, float x2, float y2, bool cross);

	//
	//Zwraca 1 jezeli obiekt ma określony text_index
	//TODO: mozna najpier posortowac po text index
	std::vector<TPoint> GetTextIndex(std::uint32_t TextIndex);


	//
	//Zwraca 1 jezeli obiekt przechodci przez obszar zaznaczenia
	std::vector<TPoint> GetCross(float x1, float y1, float x2, float y2);

	//
	//Zwraca 1 jezeli obiekt jest w obszarze zaznaczenia
	//cross jezeli 0 sprawdza czy caly obiekt jest w srodku 
	//jezeli 1 czy dowolna czesc jest w srodku
	std::vector<TPoint> GetIn(float x1, float y1, float x2, float y2, bool cross) override;

	//
	//Wylicza i zwraca maxymalne i minimalne wspolrzedne
	TMaxMinOb GetMaxMinWsp(void) override;

	//zwraca typ objektu
	ObjectType GetObjectType(void) override;

	//
	//zwraca ilosc bajtow jednego objektu
	size_t ObSize(void) override;

	//
	//zwraca ilosc objektow
	size_t Size(void) override;

	//
	//Obiekty o identyfikatorze 0
	std::vector<TPoint> Point;

private:
};
