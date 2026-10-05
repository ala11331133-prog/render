//Wirtualny obiekt reprezentujacy cechy wspolne 
//wszystkich obiektow mapy
//
#pragma once

#include <fstream>
#include "irenderer.hpp"

#define M_PI 3.14159265358979323846


struct FLOATPOINT
{
	float x;
	float y;
};

//
//Obiekt przechowuje wspolrzedne maxymalne i minimalne
struct TMaxMinOb
{
	float minx,miny,maxx,maxy;
};

enum class ObjectType : uint32_t
{
	None = 0,
	Line,
};

//interface (mozna tylko dodawac funkcje niezalezne od typu)
class iobiekt_draw
{
public:
	virtual ~iobiekt_draw() = default;

	virtual void Free(void) = 0;
	virtual ObjectType GetObjectType() = 0;
	virtual size_t ObSize(void) = 0;
	virtual size_t Size(void) = 0;
	virtual std::vector<unsigned char> SaveBytes() = 0;
	virtual size_t LoadBytes(const std::vector<unsigned char>& bytes) = 0;
	virtual float Distance(long numer, float x3, float y3) = 0;
	virtual void GetView(int szerokosc, int wysokosc, float& srodek_x, float& srodek_y, float& zoom, float margines_px) = 0;
	virtual void Draw(IRenderer& render, float srodek_x, float srodek_y, float zoom, int szerokosc, int wysokosc, long warstwa = -1) = 0;
	virtual void Sort(void) = 0;
	virtual void Arytm(long obiekt, float dodaj, float razy = 1, long xy = 3, char zaznacz = 0) = 0;
	virtual void Delete(long obiekt) = 0;
	virtual void Deselect(void) = 0;
	virtual long GetLayer(long nr_obiektu) = 0;
	virtual void SetLayer(long nr_obiektu, unsigned char warstwa) = 0;
	virtual long GetSelect(long nr_obiektu) = 0;
	virtual void SetSelect(long nr_obiektu, unsigned char reserved) = 0;
	virtual TMaxMinOb GetMaxMinWsp(void) = 0;
	virtual std::vector<unsigned char> Get(size_t nr) = 0;
	virtual bool Set(size_t nr, std::vector<unsigned char>& data) = 0;
};

//baza implementacyjna
template <typename T>
class obiekt_draw : public iobiekt_draw
{
public:	
	//
	//Zwalinia pamiec
	virtual void Free(void) = 0;
	virtual int Equal(T a, T b) = 0;

	//
	//Ladowanie danych i rezerwacja pamieci
	virtual size_t Load(const std::vector<T>& data) = 0;

	//
	//Zapis danych
	virtual size_t Save(std::vector<T>& data) = 0;
	std::vector<T> Save(void)
	{
		std::vector<T> data;
		Save(data);
		return data;
	}

	//
	//zwraca rekord w bajtach
	virtual std::vector<unsigned char> Get(size_t nr) = 0;

	//
	//zapisuje rekord w bajtach
	virtual bool Set(size_t nr, std::vector<unsigned char>& data) = 0;

	//
	//zwraca bajty do zapisu
	virtual std::vector<unsigned char> SaveBytes()
	{
		static_assert(std::is_trivially_copyable_v<T>);

		std::vector<T> data;
		Save(data);

		std::vector<unsigned char> bytes(data.size() * sizeof(T));

		if (!bytes.empty())
			std::memcpy(bytes.data(), data.data(), bytes.size());

		return bytes;
	}

	//
	//Laduje z bajtow
	virtual size_t LoadBytes(const std::vector<unsigned char>& bytes)
	{
		static_assert(std::is_trivially_copyable_v<T>);

		if (bytes.size() % sizeof(T) != 0)
			return 0;

		std::vector<T> data(bytes.size() / sizeof(T));

		if (!bytes.empty())
			std::memcpy(data.data(), bytes.data(), bytes.size());

		return Load(data);
	}	
	
	//
	//Oblicz odleglosc obiektu do punktu
	virtual float Distance(long numer, float x3, float y3) = 0;

	//
	//zwraca view czyli poczatkowy zoom dla danego ekranu
	virtual void GetView(int szerokosc, int wysokosc, float& srodek_x, float& srodek_y, float& zoom, float margines_px)
	{
		srodek_x = 0.0f;
		srodek_y = 0.0f;
		zoom = 1.0f;

		if (szerokosc <= 0 || wysokosc <= 0)
			return;

		if (margines_px < 0.0f)
			margines_px = 0.0f;

		if (Size() == 0)
			return;

		TMaxMinOb maxmin = GetMaxMinWsp();

		srodek_x = (maxmin.minx + maxmin.maxx) / 2.0f;
		srodek_y = (maxmin.miny + maxmin.maxy) / 2.0f;

		float mapa_szerokosc = maxmin.maxx - maxmin.minx;
		float mapa_wysokosc = maxmin.maxy - maxmin.miny;

		if (mapa_szerokosc <= 0.0f || mapa_wysokosc <= 0.0f)
			return;

		float dostepna_szerokosc = szerokosc - 2.0f * margines_px;

		float dostepna_wysokosc = wysokosc - 2.0f * margines_px;

		if (dostepna_szerokosc <= 0.0f || dostepna_wysokosc <= 0.0f)
			return;

		float zoom_x = dostepna_szerokosc / mapa_szerokosc;

		float zoom_y = dostepna_wysokosc / mapa_wysokosc;

		zoom = std::min(zoom_x, zoom_y);
	}

	//
	//Wyswietla linie na wskazanym dc
	virtual void Draw( IRenderer& render,
		float srodek_x, float srodek_y, float zoom, int szerokosc,
		int wysokosc, long warstwa = -1) = 0;
	//
	//Dodaje element
	virtual size_t Add(T line) = 0;

	//
	//Dodaje elementy
	virtual size_t Add(const std::vector<T>& data) = 0;

	//
	//Sortowanie obiektu w celu optymalizacji wyswietlania
	virtual void Sort(void) = 0;

	//
	//Operacje arytmetyczne dla dango obiektu
	//Jezeli obiekt -1 operacja wykonana dla wszystkich obiektow
	//zmienna xy ustawiona na 1 zmienia x ustawiona na 2 zmienia y
	//ustawiona na 3 zmienia x i y.
	//jezeli zaznacz=1 to dziala tylko na zaznaczonych
	virtual void Arytm(long obiekt,float dodaj,float razy=1,
		               long xy=3,char zaznacz=0) = 0;

	//
	//Operacja usuwania obiektu
	virtual void Delete(long obiekt) = 0;

	//
	//Odznacza wszystkie obiekty
	virtual void Deselect(void) = 0;

	//
	//Podaje warstwe danego obiektu
	virtual long GetLayer(long nr_obiektu) = 0;

	//
	//Zapisuje nowa warstwe dla danego obiektu
	virtual void SetLayer(long nr_obiektu,unsigned char warstwa) = 0;

	//
	//Podaje warstwe danego obiektu
	virtual long GetSelect(long nr_obiektu) = 0;

	//
	//Zapisuje nowa warstwe dla danego obiektu
	virtual void SetSelect(long nr_obiektu,unsigned char reserved) = 0;

	//
	//Zwraca 1 jezeli obiekt jest w obszarze zaznaczenia
	//cross jezeli 0 sprawdza czy caly obiekt jest w srodku 
	//jezeli 1 czy dowolna czesc jest w srodku
	virtual bool IsIn(const T& line, float x1, float y1, float x2, float y2, bool cross) = 0;

	virtual bool IsIn(const T& line, TMaxMinOb m, bool cross)
	{
		return IsIn(line, m.minx, m.miny, m.maxx, m.maxy, cross);
	}

	//
	//Zwraca 1 jezeli obiekt jest w obszarze zaznaczenia
	//cross jezeli 0 sprawdza czy caly obiekt jest w srodku 
	//jezeli 1 czy dowolna czesc jest w srodku
	virtual std::vector<T> GetIn(float x1,float y1,float x2,float y2, bool cross = 0) = 0;

	//
	//Wylicza i zwraca maxymalne i minimalne wspolrzedne
	virtual TMaxMinOb GetMaxMinWsp(void) = 0;

	//
	//zwraca typ
	virtual ObjectType GetObjectType(void) = 0;

	//
	//zwraca ilosc bajtow jednego objektu
	virtual size_t ObSize(void) = 0;

	//zwraca ilosc objektow
	virtual size_t Size(void) = 0;
};
