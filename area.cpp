//
//Klasa obslugujaca obiekty typu linie w plikach wektorowych
//Dolaczona jako czasc skladowa klasy wektor
//

#include <math.h>
#include <algorithm>
#include "area.h"
#include "test.hpp"

TAreaC::TAreaC(void)
{
}

TAreaC::~TAreaC(void)
{
	Free();
}

void TAreaC::Free(void)
{
	Area.clear();
}

//
//Zwraca 1 jak obiekty sa rowne 0 jak nie
int TAreaC::Equal(TArea a, TArea b)
{
	if (a.warstwa != b.warstwa) return 0;
	if (a.reserved != b.reserved) return 0;
	if (a.TextIndex != b.TextIndex) return 0;

	if (a.Points.size() != b.Points.size()) return 0;

	for (size_t i = 0; i < a.Points.size(); i++)
	{
		if (a.Points[i].x != b.Points[i].x) return 0;
		if (a.Points[i].y != b.Points[i].y) return 0;
	}

	return 1;
}

//
//Ladowanie danych i rezerwacja pamieci
size_t TAreaC::Load(const std::vector<TArea>& data)
{
	Area = data;

	//TODO: Wektor.Naglowek.obiekty 

	return Area.size();
}

//
//Zapis danych
size_t TAreaC::Save(std::vector<TArea>& data)
{
	data = Area;

	//TODO: Wektor.Naglowek.obiekty 

	return data.size();

}

//
//zwraca rekord w bajtach
std::vector<uint8_t> TAreaC::Get(size_t nr)
{
	std::vector<uint8_t> ret;

	if (nr >= Size()) return ret;

	const TArea& area = Area[nr];

	uint32_t liczba_punktow = static_cast<uint32_t>(area.Points.size());

	size_t rozmiar =
		sizeof(area.reserved) +
		sizeof(area.TextIndex) +
		sizeof(area.warstwa) +
		sizeof(liczba_punktow) +
		area.Points.size() * 2 * sizeof(float);

	ret.resize(rozmiar);

	size_t pozycja = 0;

	auto Kopiuj = [&](const void* dane, size_t rozmiar_danych)
		{
			std::memcpy(ret.data() + pozycja, dane, rozmiar_danych);
			pozycja += rozmiar_danych;
		};

	Kopiuj(&area.reserved, sizeof(area.reserved));
	Kopiuj(&area.TextIndex, sizeof(area.TextIndex));
	Kopiuj(&area.warstwa, sizeof(area.warstwa));
	Kopiuj(&liczba_punktow, sizeof(liczba_punktow));

	for (const FLOATPOINT& punkt : area.Points)
	{
		Kopiuj(&punkt.x, sizeof(punkt.x));
		Kopiuj(&punkt.y, sizeof(punkt.y));
	}

	return ret;
}

//
//zapisuje rekord w bajtach
bool TAreaC::Set(size_t nr, std::vector<uint8_t>& data)
{
	if (nr >= Size()) return false;

	size_t pozycja = 0;

	// Sprawdzenie, czy dane zawierają wymagane pola stałe
	if (data.size() <
		sizeof(Area[nr].reserved) +
		sizeof(Area[nr].TextIndex) +
		sizeof(Area[nr].warstwa) +
		sizeof(uint32_t))
		return false;

	TArea area;

	auto Kopiuj = [&](void* dane, size_t rozmiar_danych)
		{
			std::memcpy(dane, data.data() + pozycja, rozmiar_danych);
			pozycja += rozmiar_danych;
		};

	Kopiuj(&area.reserved, sizeof(area.reserved));
	Kopiuj(&area.TextIndex, sizeof(area.TextIndex));
	Kopiuj(&area.warstwa, sizeof(area.warstwa));

	uint32_t liczba_punktow = 0;
	Kopiuj(&liczba_punktow, sizeof(liczba_punktow));

	// Sprawdzenie, czy rekord zawiera wszystkie punkty
	size_t rozmiar_punktow = static_cast<size_t>(liczba_punktow) * 2 * sizeof(float);

	if (data.size() - pozycja != rozmiar_punktow)
		return false;

	area.Points.resize(liczba_punktow);

	for (FLOATPOINT& punkt : area.Points)
	{
		Kopiuj(&punkt.x, sizeof(punkt.x));
		Kopiuj(&punkt.y, sizeof(punkt.y));
	}

	Area[nr] = area;

	return true;
}

//
//Oblicz odleglosc punktu do obiektu
float TAreaC::Distance(long numer, float x3, float y3)
{
	if (numer < 0 || numer >= Area.size())
		return -1.0f;

	const std::vector<FLOATPOINT>& points = Area[numer].Points;

	if (points.size() < 3)
		return -1.0f;

	double pole2 = 0.0;
	double cx = 0.0;
	double cy = 0.0;

	for (size_t i = 0; i < points.size(); i++)
	{
		const FLOATPOINT& p1 = points[i];
		const FLOATPOINT& p2 = points[(i + 1) % points.size()];

		double iloczyn = (double)p1.x * p2.y -
			(double)p2.x * p1.y;

		pole2 += iloczyn;

		cx += (p1.x + p2.x) * iloczyn;
		cy += (p1.y + p2.y) * iloczyn;
	}

	// Obszar musi mieć niezerowe pole
	if (std::abs(pole2) < 1e-12)
		return -1.0f;

	cx /= 3.0 * pole2;
	cy /= 3.0 * pole2;

	// Odległość punktu od środka ciężkości
	double dx = x3 - cx;
	double dy = y3 - cy;

	return static_cast<float>(std::sqrt(dx * dx + dy * dy));
}

//
//Wyswietla linie na wskazanym dc
void TAreaC::Draw(IRenderer& render,
	float srodek_x, float srodek_y, float zoom,
	int szerokosc, int wysokosc, long warstwa)
{
	if (szerokosc <= 0 || wysokosc <= 0) return;

	if (zoom <= 0.0f) return;

	for (size_t ii = 0; ii < Area.size(); ii++)
	{
		if (warstwa >= 0 && warstwa != Area[ii].warstwa) continue;

		// Przepisanie punktów i przeliczenie współrzędnych
		std::vector<FLOATPOINT> points;

		for (const FLOATPOINT& punkt : Area[ii].Points)
		{
			FLOATPOINT p;

			p.x = (punkt.x - srodek_x) * zoom + szerokosc / 2.0f;
			p.y = (punkt.y - srodek_y) * zoom + wysokosc / 2.0f;

			points.push_back(p);
		}

		//TODO::kolor z warstwy pobierac
		// Rysowanie wypełnionego obszaru
		render.FilledPolygon(
			points,

			RenderPixel(255, 255, 255),  // kolor obwódki
			RenderPixel(100, 100, 100)   // kolor wypełnienia
		);
	}
}

//
//Wstaw Linie z pliku
size_t TAreaC::Add(TArea area)
{
	Area.push_back(area);

	//TODO: Wektor.Naglowek.obiekty 

	return Area.size();
}

size_t TAreaC::Add(const std::vector<TArea>& data)
{
	Area.insert(Area.end(), data.begin(), data.end());

	//TODO: Wektor.Naglowek.obiekty 

	return Area.size();
}

//
//Metoda do sortowania polaczen miedzy liniami
//wywolywane tylko poprzez GenerujPol
bool TAreaC::CompareAreas(const TArea& a, const TArea& b)
{
	auto Srodek = [](const TArea& area)
	{
			FLOATPOINT srodek = { 0.0f, 0.0f };

			if (area.Points.empty()) return srodek;

			float minx = area.Points[0].x;
			float maxx = area.Points[0].x;
			float miny = area.Points[0].y;
			float maxy = area.Points[0].y;

			for (const FLOATPOINT& p : area.Points)
			{
				minx = std::min(minx, p.x);
				maxx = std::max(maxx, p.x);
				miny = std::min(miny, p.y);
				maxy = std::max(maxy, p.y);
			}

			srodek.x = (minx + maxx) / 2.0f;
			srodek.y = (miny + maxy) / 2.0f;

			return srodek;
		};

	FLOATPOINT sa = Srodek(a);
	FLOATPOINT sb = Srodek(b);

	if (sa.x != sb.x) return sa.x < sb.x;

	if (sa.y != sb.y) return sa.y < sb.y;

	return a.warstwa < b.warstwa;
}

bool TAreaC::CompareAreasLayer(const TArea& a, const TArea& b)
{
	if (a.warstwa != b.warstwa)
		return a.warstwa < b.warstwa;

	auto Srodek = [](const TArea& area)
	{
			FLOATPOINT srodek = { 0.0f, 0.0f };

			if (area.Points.empty())
				return srodek;

			float minx = area.Points[0].x;
			float maxx = area.Points[0].x;
			float miny = area.Points[0].y;
			float maxy = area.Points[0].y;

			for (const FLOATPOINT& p : area.Points)
			{
				minx = std::min(minx, p.x);
				maxx = std::max(maxx, p.x);
				miny = std::min(miny, p.y);
				maxy = std::max(maxy, p.y);
			}

			srodek.x = (minx + maxx) / 2.0f;
			srodek.y = (miny + maxy) / 2.0f;

			return srodek;
		};

	FLOATPOINT sa = Srodek(a);
	FLOATPOINT sb = Srodek(b);

	if (sa.x != sb.x) return sa.x < sb.x;

	if (sa.y != sb.y) return sa.y < sb.y;

	return false;
}

bool TAreaC::CompareAreasTextIndex(const TArea& a, const TArea& b)
{
	return a.TextIndex < b.TextIndex;
}

//
//Sortowanie obiektu w celu optymalizacji wyswietlania
void TAreaC::Sort(std::int8_t SortMethod)
{
	//TODO:: zdecydowac ktore lepsze i wywalic jedno

	if (SortMethod == 1) std::sort(Area.begin(), Area.end(), CompareAreas);
	else if (SortMethod == 2) std::sort(Area.begin(), Area.end(), CompareAreasTextIndex);
	else std::sort(Area.begin(), Area.end(), CompareAreasLayer);

	//std::sort(Area.begin(), Area.end(), CompareAreas);
}

//
//Operacj arytmetyczne dla dango obiektu
//Jezeli obiekt mniejszy od zera operacja wykonana dla wszystkich obiektow
//zmienna xy ustawiona na 1 zmienia x ustawiona na 2 zmienia y
//ustawiona na 3 zmienia x i y.
void TAreaC::Arytm(long obiekt, float dodaj, float razy, long xy, char zaznacz)
{
	long l;
	//
	//Jezeli poza zakresem
	if (obiekt >= (long)Area.size()) return;

	//
	//Jezeli zmiana jednego obiektu
	if (obiekt >= 0)
	{
		for (size_t i = 0; i < Area[obiekt].Points.size(); i++)
		{
			if (xy & 1)
			{
				Area[obiekt].Points[i].x += dodaj;
				Area[obiekt].Points[i].x *= razy;
			}

			if (xy & 2)
			{
				Area[obiekt].Points[i].y += dodaj;
				Area[obiekt].Points[i].y *= razy;
			}
		}
	}
	//
	//Jezeli zmiana wszystkich obiektow
	else
	{
		for (l = 0; l < Area.size(); l++)
		{
			if (zaznacz && !GetSelect(l)) continue;

			for (size_t i = 0; i < Area[l].Points.size(); i++)
			{
				if (xy & 1)
				{
					Area[l].Points[i].x += dodaj;
					Area[l].Points[i].x *= razy;
				}

				if (xy & 2)
				{
					Area[l].Points[i].y += dodaj;
					Area[l].Points[i].y *= razy;
				}
			}
		}
	}
}
//
//Operacja usuwania obiektu
void TAreaC::Delete(long obiekt)
{

	//
	//Jezeli obiekt poza zakresem
	if ((obiekt >= Area.size()) ||
		(obiekt < 0)) return;

	Area.erase(Area.begin() + obiekt);
}

//
//Odznacza wszystkie obiekty
void TAreaC::Deselect(void)
{
	for (int i = 0; i < Area.size(); i++)
		Area[i].reserved = 0;
}

//
//Podaje warstwe danego obiektu
long TAreaC::GetLayer(long nr_obiektu)
{
	if (nr_obiektu >= 0 && nr_obiektu < Area.size())
		return Area[nr_obiektu].warstwa;
	return -1;
}

//
//Zapisuje nowa warstwe dla danego obiektu
void TAreaC::SetLayer(long nr_obiektu, uint8_t warstwa)
{
	if (nr_obiektu >= 0 && nr_obiektu < Area.size())
		Area[nr_obiektu].warstwa = warstwa;
}

//
//Podaje warstwe danego obiektu
long TAreaC::GetSelect(long nr_obiektu)
{
	if (nr_obiektu >= 0 && nr_obiektu < Area.size())
		return Area[nr_obiektu].reserved;
	return -1;
}

//
//Zapisuje nowa warstwe dla danego obiektu
void TAreaC::SetSelect(long nr_obiektu, uint8_t reserved)
{
	if (nr_obiektu >= 0 && nr_obiektu < Area.size())
		Area[nr_obiektu].reserved = reserved;
}

//
// Zwraca true, jeżeli wielokąt przecina obszar zaznaczenia
bool TAreaC::IsCross(
	const TArea& area,
	float x1, float y1,
	float x2, float y2)
{
	const size_t n = area.Points.size();

	if (n < 3) return false;

	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	// 1. Sprawdzenie, czy którykolwiek wierzchołek
	//    wielokąta znajduje się w prostokącie.
	for (const FLOATPOINT& p : area.Points)
	{
		if (p.x >= x1 && p.x <= x2 &&
			p.y >= y1 && p.y <= y2)
			return true;
	}

	// 2. Sprawdzenie przecięcia każdego boku wielokąta
	//    z prostokątem metodą Liang-Barsky.
	for (size_t i = 0; i < n; ++i)
	{
		const FLOATPOINT& a = area.Points[i];
		const FLOATPOINT& b = area.Points[(i + 1) % n];

		const double dx = static_cast<double>(b.x) - a.x;
		const double dy = static_cast<double>(b.y) - a.y;

		double tmin = 0.0;
		double tmax = 1.0;

		auto Clip = [&](double p, double q) -> bool
			{
				if (p == 0.0) return q >= 0.0;

				const double r = q / p;

				if (p < 0.0)
				{
					if (r > tmax) return false;
					if (r > tmin) tmin = r;
				}
				else
				{
					if (r < tmin) return false;
					if (r < tmax) tmax = r;
				}

				return true;
			};

		if (Clip(-dx, static_cast<double>(a.x) - x1) &&
			Clip(dx, static_cast<double>(x2) - a.x) &&
			Clip(-dy, static_cast<double>(a.y) - y1) &&
			Clip(dy, static_cast<double>(y2) - a.y) &&
			tmin <= tmax)
			return true;
	}


	// 3. Sprawdzenie, czy którykolwiek narożnik prostokąta
	//    znajduje się wewnątrz wielokąta.
	const FLOATPOINT narożniki[] =
	{
		{ x1, y1 },
		{ x2, y1 },
		{ x2, y2 },
		{ x1, y2 }
	};

	for (const FLOATPOINT& p : narożniki)
	{
		bool inside = false;

		for (size_t i = 0, j = n - 1; i < n; j = i++)
		{
			const FLOATPOINT& a = area.Points[j];
			const FLOATPOINT& b = area.Points[i];

			if ((a.y > p.y) != (b.y > p.y))
			{
				const double crossX =
					static_cast<double>(a.x) +
					(static_cast<double>(p.y) - a.y) *
					(static_cast<double>(b.x) - a.x) /
					(static_cast<double>(b.y) - a.y);

				if (static_cast<double>(p.x) < crossX) inside = !inside;
			}
		}

		if (inside) return true;
	}

	return false;
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
bool TAreaC::IsIn(const TArea& area,
	float x1, float y1, float x2, float y2, bool cross)
{
	if (cross)
		return IsCross(area, x1, y1, x2, y2);

	if (area.Points.size() < 3)
		return false;

	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	// Sprawdzanie wszystkich punktów obszaru
	for (const FLOATPOINT& punkt : area.Points)
	{
		if (punkt.x < x1 || punkt.x > x2 ||
			punkt.y < y1 || punkt.y > y2)
		{
			return false;
		}
	}

	return true;
}

//
//Zwraca 1 jezeli obiekt ma określony text_index
//TODO: mozna najpier posortowac po text index
std::vector<TArea> TAreaC::GetTextIndex(std::uint32_t TextIndex)
{
	std::vector<TArea> ret;
	for (const TArea& area : Area)
	{
		if (area.TextIndex == TextIndex)
			ret.push_back(area);
	}

	return ret;
}

//
// Zwraca wszystkie obiekty, które przechodzą przez obszar zaznaczenia
std::vector<TArea> TAreaC::GetCross(
	float x1, float y1,
	float x2, float y2)
{
	std::vector<TArea> ret;

	for (const TArea& area : Area)
	{
		if (IsCross(area, x1, y1, x2, y2))
			ret.push_back(area);
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
std::vector<TArea> TAreaC::GetIn(
	float x1, float y1,
	float x2, float y2,
	bool cross)
{
	std::vector<TArea> ret;

	for (const TArea& area : Area)
	{
		if (IsIn(area, x1, y1, x2, y2, cross))
			ret.push_back(area);
	}

	return ret;
}

//
//Wylicza i zwraca maxymalne i minimalne wspolrzedne
TMaxMinOb TAreaC::GetMaxMinWsp(void)
{
	TMaxMinOb wynik = { 0 };

	if (Area.empty())
		return wynik;

	bool pierwszy = true;

	for (const auto& a : Area)
	{
		for (const FLOATPOINT& punkt : a.Points)
		{
			if (pierwszy)
			{
				wynik.minx = wynik.maxx = punkt.x;
				wynik.miny = wynik.maxy = punkt.y;
				pierwszy = false;
			}
			else
			{
				wynik.minx = std::min(wynik.minx, punkt.x);
				wynik.maxx = std::max(wynik.maxx, punkt.x);
				wynik.miny = std::min(wynik.miny, punkt.y);
				wynik.maxy = std::max(wynik.maxy, punkt.y);
			}
		}
	}

	return wynik;
}

//zwraca typ objektu
ObjectType TAreaC::GetObjectType(void)
{
	return TypeId;
}

//
//zwraca ilosc bajtow jednego objektu
size_t TAreaC::ObSize(void)
{
	return sizeof(TArea);
}

//zwraca ilosc objektow
size_t TAreaC::Size(void)
{
	return Area.size();
}

std::vector<uint8_t> TAreaC::SaveBytes()
{
	std::vector<uint8_t> bytes;

	for (size_t i = 0; i < Size(); i++)
	{
		std::vector<uint8_t> rekord = Get(i);
		uint32_t rozmiar = static_cast<uint32_t>(rekord.size());

		const uint8_t* p = reinterpret_cast<const uint8_t*>(&rozmiar);
		bytes.insert(bytes.end(), p, p + sizeof(rozmiar));
		bytes.insert(bytes.end(), rekord.begin(), rekord.end());
	}

	return bytes;
}

size_t TAreaC::LoadBytes(const std::vector<uint8_t>& bytes)
{
	Free();

	size_t pozycja = 0;
	size_t liczba = 0;

	while (pozycja < bytes.size())
	{
		if (bytes.size() - pozycja < sizeof(uint32_t))
		{
			Free();
			return 0;
		}

		uint32_t rozmiar = 0;
		std::memcpy(&rozmiar, bytes.data() + pozycja, sizeof(rozmiar));
		pozycja += sizeof(rozmiar);

		if (rozmiar > bytes.size() - pozycja)
		{
			Free();
			return 0;
		}

		std::vector<uint8_t> rekord(
			bytes.begin() + pozycja,
			bytes.begin() + pozycja + rozmiar);

		pozycja += rozmiar;

		TArea area;
		Add(area);

		if (!Set(Size() - 1, rekord))
		{
			Free();
			return 0;
		}

		liczba++;
	}

	return liczba;
}

