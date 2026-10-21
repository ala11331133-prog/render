//
//Klasa obslugujaca obiekty typu linie w plikach wektorowych
//Dolaczona jako czasc skladowa klasy wektor
//

#include <math.h>
#include <algorithm>
#include "point.h"

TPointC::TPointC(void)
{
}

TPointC::~TPointC(void)
{
	Free();
}

void TPointC::Free(void)
{
	Point.clear();
}

//
//Zwraca 1 jak obiekty sa rowne 0 jak nie
int TPointC::Equal(TPoint a, TPoint b)
{
	if (a.x1 != b.x1) return 0;
	if (a.y1 != b.y1) return 0;
	if (a.warstwa != b.warstwa) return 0;

	return 1;
}

//
//Ladowanie danych i rezerwacja pamieci
size_t TPointC::Load(const std::vector<TPoint>& data)
{
	Point = data;

	//TODO: Wektor.Naglowek.obiekty 

	return Point.size();
}

//
//Zapis danych
size_t TPointC::Save(std::vector<TPoint>& data)
{
	data = Point;

	//TODO: Wektor.Naglowek.obiekty 

	return data.size();

}

//
//zwraca rekord w bajtach
std::vector<uint8_t> TPointC::Get(size_t nr)
{
	std::vector<uint8_t> ret;
	if (nr >= Size()) return ret;

	ret.resize(ObSize());

	std::memcpy(ret.data(), &Point[nr], sizeof(TPoint));
	return ret;
}

//
//zapisuje rekord w bajtach
bool TPointC::Set(size_t nr, std::vector<uint8_t>& data)
{
	if (nr >= Size()) return false;
	if (data.size() != ObSize()) return false;
	std::memcpy(&Point[nr], data.data(), sizeof(TPoint));
	return true;
}

//
//Oblicz odleglosc punktu do obiektu
float TPointC::Distance(long numer, float x3, float y3)
{

	float x1 = Point[numer].x1;
	float y1 = Point[numer].y1;

	return (float)std::sqrt((x3-x1)*(x3 - x1)+ (y3 - y1) * (y3 - y1));
}

//
//Wyswietla linie na wskazanym dc
void TPointC::Draw(IRenderer& render,
	float srodek_x, float srodek_y, float zoom, int szerokosc,
	int wysokosc, long warstwa)
{
	if (szerokosc <= 0 || wysokosc <= 0)
		return;

	if (zoom <= 0.0f)
		return;

	// Przeliczenie współrzędnych świata na ekran
	for (size_t ii = 0; ii < Point.size(); ii++)
	{
		if (warstwa >= 0 && warstwa != Point[ii].warstwa) continue;
		const float x1 = (Point[ii].x1 - srodek_x) * zoom + szerokosc / 2.0f;
		const float y1 = (Point[ii].y1 - srodek_y) * zoom + wysokosc / 2.0f;
		render.Point(x1, y1);
	}
}
//
//Wstaw Linie z pliku
size_t TPointC::Add(TPoint point)
{
	Point.push_back(point);

	//TODO: Wektor.Naglowek.obiekty 

	return Point.size();
}

size_t TPointC::Add(const std::vector<TPoint>& data)
{
	Point.insert(Point.end(), data.begin(), data.end());

	//TODO: Wektor.Naglowek.obiekty 

	return Point.size();
}

//
//Metoda do sortowania polaczen miedzy liniami
//wywolywane tylko poprzez GenerujPol
bool TPointC::ComparePoint(const TPoint& a, const TPoint& b)
{
	if (a.x1 != b.x1)
		return a.x1 < b.x1;

	if (a.y1 != b.y1)
		return a.y1 < b.y1;

	return a.warstwa < b.warstwa;
}

bool TPointC::ComparePointLayer(const TPoint& a, const TPoint& b)
{
	if (a.warstwa != b.warstwa)
		return a.warstwa < b.warstwa;

	if (a.x1 != b.x1)
		return a.x1 < b.x1;

	return a.y1 < b.y1;
}

bool TPointC::ComparePointTextIndex(const TPoint& a, const TPoint& b)
{
	return a.TextIndex < b.TextIndex;
}

//
//Sortowanie obiektu w celu optymalizacji wyswietlania
void TPointC::Sort(std::int8_t SortMethod)
{
	//TODO:: zdecydowac ktore lepsze i wywalic jedno

	if (SortMethod == 1) std::sort(Point.begin(), Point.end(), ComparePoint);
	else if (SortMethod == 2) std::sort(Point.begin(), Point.end(), ComparePointTextIndex);
	else std::sort(Point.begin(), Point.end(), ComparePointLayer);

	//std::sort(Point.begin(), Point.end(), ComparePoints);
}

//
//Operacj arytmetyczne dla dango obiektu
//Jezeli obiekt mniejszy od zera operacja wykonana dla wszystkich obiektow
//zmienna xy ustawiona na 1 zmienia x ustawiona na 2 zmienia y
//ustawiona na 3 zmienia x i y.
void TPointC::Arytm(long obiekt, float dodaj, float razy, long xy, char zaznacz)
{
	long l;
	//
	//Jezeli poza zakresem
	if (obiekt >= (long)Point.size()) return;

	//
	//Jezeli zmiana jednego obiektu
	if (obiekt >= 0)
	{
		if (xy & 1)
		{
			Point[obiekt].x1 += dodaj;
			Point[obiekt].x1 *= razy;
		}

		if (xy & 2)
		{
			Point[obiekt].y1 += dodaj;
			Point[obiekt].y1 *= razy;
		}
	}
	//
	//Jezeli zmiana wszystkich obiektow
	else
	{
		for (l = 0; l < Point.size(); l++)
		{
			if (zaznacz && !GetSelect(l)) continue;
			if (xy & 1)
			{
				Point[l].x1 += dodaj;
				Point[l].x1 *= razy;
			}

			if (xy & 2)
			{
				Point[l].y1 += dodaj;
				Point[l].y1 *= razy;
			}
		}
	}

}

//
//Operacja usuwania obiektu
void TPointC::Delete(long obiekt)
{

	//
	//Jezeli obiekt poza zakresem
	if ((obiekt >= Point.size()) ||
		(obiekt < 0)) return;

	Point.erase(Point.begin() + obiekt);
}

//
//Odznacza wszystkie obiekty
void TPointC::Deselect(void)
{
	for (int i = 0; i < Point.size(); i++)
		Point[i].reserved = 0;
}

//
//Podaje warstwe danego obiektu
long TPointC::GetLayer(long nr_obiektu)
{
	if (nr_obiektu >= 0 && nr_obiektu < Point.size())
		return Point[nr_obiektu].warstwa;
	return -1;
}

//
//Zapisuje nowa warstwe dla danego obiektu
void TPointC::SetLayer(long nr_obiektu, uint8_t warstwa)
{
	if (nr_obiektu >= 0 && nr_obiektu < Point.size())
		Point[nr_obiektu].warstwa = warstwa;
}

//
//Podaje warstwe danego obiektu
long TPointC::GetSelect(long nr_obiektu)
{
	if (nr_obiektu >= 0 && nr_obiektu < Point.size())
		return Point[nr_obiektu].reserved;
	return -1;
}

//
//Zapisuje nowa warstwe dla danego obiektu
void TPointC::SetSelect(long nr_obiektu, uint8_t reserved)
{
	if (nr_obiektu >= 0 && nr_obiektu < Point.size())
		Point[nr_obiektu].reserved = reserved;
}

//
// Zwraca true, jeżeli podany obiekt przechodzi przez obszar zaznaczenia
//
bool TPointC::IsCross(const TPoint& Point,
	float x1, float y1,
	float x2, float y2)
{
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	float dx = Point.x1;
	float dy = Point.y1;

	float tmin = 0.0f;
	float tmax = 1.0f;

	// X
	if (dx == 0)
	{
		if (Point.x1 < x1 || Point.x1 > x2)
			return false;
	}
	else
	{
		float t1 = (x1 - Point.x1) / dx;
		float t2 = (x2 - Point.x1) / dx;

		if (t1 > t2)
			std::swap(t1, t2);

		tmin = std::max(tmin, t1);
		tmax = std::min(tmax, t2);

		if (tmin > tmax)
			return false;
	}

	// Y
	if (dy == 0)
	{
		if (Point.y1 < y1 || Point.y1 > y2)
			return false;
	}
	else
	{
		float t1 = (y1 - Point.y1) / dy;
		float t2 = (y2 - Point.y1) / dy;

		if (t1 > t2)
			std::swap(t1, t2);

		tmin = std::max(tmin, t1);
		tmax = std::min(tmax, t2);

		if (tmin > tmax)
			return false;
	}

	return true;
}


//
// Sprawdza podany obiekt.
//
// cross == false:
//   cały obiekt musi znajdować się w obszarze.
//
// cross == true:
//   dowolna część obiektu może znajdować się w obszarze.
//
bool TPointC::IsIn(const TPoint& Point, float x1, float y1, float x2, float y2, bool cross)
{
	if (cross)
		return IsCross(Point, x1, y1, x2, y2);

	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	float minx = Point.x1;
	float maxx = Point.x1;
	float miny = Point.y1;
	float maxy = Point.y1;

	return minx >= x1 &&
		maxx <= x2 &&
		miny >= y1 &&
		maxy <= y2;
}


//
//Zwraca 1 jezeli obiekt ma określony text_index
//TODO: mozna najpier posortowac po text index
std::vector<TPoint> TPointC::GetTextIndex(std::uint32_t TextIndex)
{
	std::vector<TPoint> ret;
	for (const TPoint& Point : Point)
	{
		if (Point.TextIndex == TextIndex)
			ret.push_back(Point);
	}

	return ret;
}

//
// Zwraca wszystkie obiekty, które przechodzą przez obszar zaznaczenia
std::vector<TPoint> TPointC::GetCross(
	float x1, float y1,
	float x2, float y2)
{
	std::vector<TPoint> ret;

	for (const TPoint& Point : Point)
	{
		if (IsCross(Point, x1, y1, x2, y2))
			ret.push_back(Point);
	}

	return ret;
}


//
// Zwraca wszystkie obiekty spełniające warunek:
//
// cross == false:
//   cały obiekt znajduje się w obszarze.
//
// cross == true:
//   dowolna część obiektu znajduje się w obszarze.
//
std::vector<TPoint> TPointC::GetIn(
	float x1, float y1,
	float x2, float y2,
	bool cross)
{
	std::vector<TPoint> ret;

	for (const TPoint& Point : Point)
	{
		if (IsIn(Point, x1, y1, x2, y2, cross))
			ret.push_back(Point);
	}

	return ret;
}

//
//Wylicza i zwraca maxymalne i minimalne wspolrzedne
TMaxMinOb TPointC::GetMaxMinWsp(void)
{
	TMaxMinOb wynik = { 0 };

	if (Point.empty())
		return wynik;

	wynik.minx = wynik.maxx = Point[0].x1;
	wynik.miny = wynik.maxy = Point[0].y1;

	for (const auto& l : Point)
	{
		wynik.minx = std::min(wynik.minx, l.x1);
		wynik.maxx = std::max(wynik.maxx, l.x1);
		wynik.miny = std::min(wynik.miny, l.y1);
		wynik.maxy = std::max(wynik.maxy, l.y1);
	}

	return wynik;
}

//zwraca typ objektu
ObjectType TPointC::GetObjectType(void)
{
	return TypeId;
}

//
//zwraca ilosc bajtow jednego objektu
size_t TPointC::ObSize(void)
{
	return sizeof(TPoint);
}

//zwraca ilosc objektow
size_t TPointC::Size(void)
{
	return Point.size();
}
