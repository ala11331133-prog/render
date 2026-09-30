//
//Klasa obslugujaca obiekty typu linie w plikach wektorowych
//Dolaczona jako czasc skladowa klasy wektor
//

#include <math.h>
#include <algorithm>
#include "line.h"
#include "test.h"

TLineC::TLineC(void)
{
}

TLineC::~TLineC(void)
{
	Free();
}

void TLineC::Free(void)
{
	Line.clear();
}

//
//Zwraca 1 jak obiekty sa rowne 0 jak nie
int TLineC::Equal(TLine a,TLine b)
{
	if (a.x1!=b.x1) return 0;
	if (a.x2!=b.x2) return 0;
	if (a.y1!=b.y1) return 0;
	if (a.y2!=b.y2) return 0;
	if (a.warstwa!=b.warstwa) return 0;

	return 1;
}

//
//Ladowanie danych i rezerwacja pamieci
size_t TLineC::Load(const std::vector<TLine> &data)
{
	Line = data;

	//TODO: Wektor.Naglowek.obiekty 

	return Line.size();
}

//
//Zapis danych
size_t TLineC::Save(std::vector<TLine>& data)
{
	data = Line;

	//TODO: Wektor.Naglowek.obiekty 

	return data.size();

}

//
//Oblicz odleglosc punktu do obiektu
float TLineC::Distance(long numer,float x3,float y3)
{
  float D1,D2; //odleglosci
  float X,Y;
  float px,py,tmp;
  
  float A,B,C,A1,B1,C1;
  float x1,y1,x2,y2;
	
  x1=Line[numer].x1;
  y1=Line[numer].y1;
  x2=Line[numer].x2;
  y2=Line[numer].y2;

  X = x2 - x1;
  Y = y2 - y1;
  
  if( (Y != 0) && (X != 0))
  {
    A = Y/X; //nachylenie prostej
    B = -1;
    C = y1 - A* x1; //A,B,C parametry prostej przechodzacej przez
    //odcinek (x1,y1) i (x2,y2)
    
    //prosta prostopadla do prostej przechodzaca przez punkt 3
    A1 = (-1)/A;
    B1 = -1;
    C1 = y3  - A1*x3;
    
    //Obliczanie punktu przeciecia dwoch prostych prostopadlych
    
    px =  (C*B1 - C1*B)/(B*A1 - A*B1);
    py =  (C*A1 - C1*A)/(B1*A - B*A1);
    
  }//pod katem
  
  if(Y == 0) //prosta jest rownolegla do osi OY
  {
    px = x3;
    py = y1; //==y2
  }
  
  if(X == 0) //prosta jest rownolegla do osi OX
  {
    px = x1; //==x2
    py = y3;
  }
  
  //Sprawdzenie, czy P1 jest poczatkiem odcinka
  if(x1 > x2)
  {
    tmp = x1;
    x1 = x2;
    x2 = tmp; //zamiana x1 z x2
    
    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }
  
  //Sprawdzenie czy punkt przeciecia prostych prostopadlych nalezy do
  //odcinka
  if( (px>= x1) && (px <= x2) && ( ((py >= y1)&&(py<=y2))||((py >= y2)&&(py<=y1)) ) )
  {
    //odleglosc punktu od prostej
    D1 = (float)sqrt( pow((px-x3),2) + pow((py-y3),2) );
    return D1;
  }
  else
  {
    //punkt przeciecia nie nalezy do odcinka
    D1 = (float)sqrt( pow((x1-x3),2) + pow((y1-y3),2) );
    D2 = (float)sqrt( pow((x2-x3),2) + pow((y2-y3),2) );
  }
  
  if(D1<D2) return D1;
  else return D2;
}

//
//Wyswietla linie na wskazanym dc
void TLineC::Draw(long numer,
				 float szer_min,float szer_max,
				 float wys_min,float wys_max,long warstwa)
{

	//TODO::sprawdzimy najpierw skia
	//TODO::dodac rysowanie
	/*float x1,y1,x2,y2;
	HPEN ppen;

	//
	//Przelicz wsp i rysuj
	x1=Line[numer].x1;
	y1=Line[numer].y1;
	x2=Line[numer].x2;
	y2=Line[numer].y2;
	
	//
	//Jezeli poza ekranem to nie rysuj
	if ( (x1<szer_min || x1>szer_max)&&
		 (x2<szer_min || x2>szer_max)&&
		 (y1<wys_min || y1>wys_max)&&
		 (y2<wys_min || y2>wys_max) ) 
		 return;

	PrzelNaEkr(x1,y1);
	PrzelNaEkr(x2,y2);

	if (warstwa>0)
	{
		long wsp;
		float wsp_pom;
		wsp_pom=zm.GetZoom()*zm.GetLineRescal();
		if (wsp_pom>1)
			wsp=(long)wsp_pom;
		else wsp=1;
		ppen=CreatePen(	Wektor.WarDane[warstwa].rodzaj,
						Wektor.WarDane[warstwa].grubosc*wsp,
						RGB(Wektor.WarDane[warstwa].red,
							Wektor.WarDane[warstwa].green,
							Wektor.WarDane[warstwa].blue));
	}

	if (warstwa>0) dc1->SelectObject(ppen);

	dc1->MoveTo((int)x1,(int)y1);
	dc1->LineTo((int)x2,(int)y2);

	if (warstwa>0)
	{
		dc1->SelectObject(kasujpen);
		DeleteObject(ppen);
	}*/
}

//
//Wstaw Linie z pliku
size_t TLineC::Add(TLine line)
{
	Line.push_back(line);

	//TODO: Wektor.Naglowek.obiekty 

	return Line.size();
}

size_t TLineC::Add(const std::vector<TLine>& data)
{
	Line.insert(Line.end(), data.begin(), data.end());

	//TODO: Wektor.Naglowek.obiekty 

	return Line.size();
}



//
//Metoda do sortowania polaczen miedzy liniami
//wywolywane tylko poprzez GenerujPol
bool CompareLines(const TLine& a, const TLine& b)
{
	if (a.x1 != b.x1)
		return a.x1 < b.x1;

	if (a.x2 != b.x2)
		return a.x2 < b.x2;

	if (a.y1 != b.y1)
		return a.y1 < b.y1;

	if (a.y2 != b.y2)
		return a.y2 < b.y2;

	return a.warstwa < b.warstwa;
}

bool CompareLinesLayer(const TLine& a, const TLine& b)
{
	if (a.warstwa != b.warstwa)
		return a.warstwa < b.warstwa;

	if (a.x1 != b.x1)
		return a.x1 < b.x1;

	if (a.x2 != b.x2)
		return a.x2 < b.x2;

	if (a.y1 != b.y1)
		return a.y1 < b.y1;

	return a.y2 < b.y2;
}

//
//Sortowanie obiektu w celu optymalizacji wyswietlania
void TLineC::Sort(void)
{	
	//TODO:: zdecydowac ktore lepsze i wywalic jedno
	std::sort(Line.begin(), Line.end(), CompareLinesLayer);
	//std::sort(Line.begin(), Line.end(), CompareLines);
}

//
//Operacj arytmetyczne dla dango obiektu
//Jezeli obiekt mniejszy od zera operacja wykonana dla wszystkich obiektow
//zmienna xy ustawiona na 1 zmienia x ustawiona na 2 zmienia y
//ustawiona na 3 zmienia x i y.
void TLineC::Arytm(long obiekt,float dodaj,float razy,long xy,char zaznacz)
{
	long l;
	//
	//Jezeli poza zakresem
	if (obiekt>=Line.size()) return;

	//
	//Jezeli zmiana jednego obiektu
	if (obiekt>0)
	{
		if (xy&1)
		{
			Line[obiekt].x1+=dodaj;
			Line[obiekt].x2+=dodaj;
			Line[obiekt].x1*=razy;
			Line[obiekt].x2*=razy;
		}

		if (xy&2)
		{
			Line[obiekt].y1+=dodaj;
			Line[obiekt].y2+=dodaj;
			Line[obiekt].y1*=razy;
			Line[obiekt].y2*=razy;
		}
	}
	//
	//Jezeli zmiana wszystkich obiektow
	else
	{
		for (l=0;l<Line.size();l++)
		{
			if (zaznacz && !GetLayer(l)) continue;
			if(xy&1)
			{
				Line[l].x1+=dodaj;
				Line[l].x2+=dodaj;
				Line[l].x1*=razy;
				Line[l].x2*=razy;
			}

			if (xy&2)
			{
				Line[l].y1+=dodaj;
				Line[l].y2+=dodaj;
				Line[l].y1*=razy;
				Line[l].y2*=razy;
			}
		}
	}

}

//
//Operacja usuwania obiektu
void TLineC::Delete(long obiekt)
{

	//
	//Jezeli obiekt poza zakresem
	if ( (obiekt>=Line.size()) ||
		 (obiekt<0) ) return;

	Line.erase(Line.begin() + obiekt);
}

//
//Odznacza wszystkie obiekty
void TLineC::Deselect(void)
{
	for (int i=0;i<Line.size();i++)
		Line[i].reserved=0;
}

//
//Podaje warstwe danego obiektu
long TLineC::GetLayer(long nr_obiektu)
{
	if (nr_obiektu>=0 && nr_obiektu < Line.size())
		return Line[nr_obiektu].warstwa;
	return -1;
}

//
//Zapisuje nowa warstwe dla danego obiektu
void TLineC::SetLayer(long nr_obiektu,unsigned char warstwa)
{
	if (nr_obiektu>=0 && nr_obiektu < Line.size())
		Line[nr_obiektu].warstwa=warstwa;
}

//
//Podaje warstwe danego obiektu
long TLineC::GetSelect(long nr_obiektu)
{
	if (nr_obiektu>=0 && nr_obiektu < Line.size())
		return Line[nr_obiektu].reserved;
	return -1;
}

//
//Zapisuje nowa warstwe dla danego obiektu
void TLineC::SetSelect(long nr_obiektu,unsigned char reserved)
{
	if (nr_obiektu>=0 && nr_obiektu < Line.size())
		Line[nr_obiektu].reserved=reserved;
}

//
//Zwraca 1 jezeli obiekt przechodci przez obszar zaznaczenia
std::vector<TLine> TLineC::GetCross(float x1, float y1, float x2, float y2)
{
	std::vector<TLine> ret;

	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	for (size_t ii = 0; ii < Line.size(); ii++)
	{
		float dx = Line[ii].x2 - Line[ii].x1;
		float dy = Line[ii].y2 - Line[ii].y1;

		float tmin = 0.0f;
		float tmax = 1.0f;

		// X
		if (dx == 0) 
			if (Line[ii].x1 < x1 || Line[ii].x1 > x2) continue;
		else
		{
			float t1 = (x1 - Line[ii].x1) / dx;
			float t2 = (x2 - Line[ii].x1) / dx;

			if (t1 > t2) std::swap(t1, t2);

			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);

			if (tmin > tmax)
				continue;
		}

		// Y
		if (dy == 0)
			if (Line[ii].y1 < y1 || Line[ii].y1 > y2) continue;
		else
		{
			float t1 = (y1 - Line[ii].y1) / dy;
			float t2 = (y2 - Line[ii].y1) / dy;

			if (t1 > t2) std::swap(t1, t2);

			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);

			if (tmin > tmax) continue;
		}

		ret.push_back(Line[ii]);
	}

	return ret;
}

//
//Zwraca 1 jezeli obiekt jest w obszarze zaznaczenia
//cross jezeli 0 sprawdza czy caly obiekt jest w srodku 
//jezeli 1 czy dowolna czesc jest w srodku
std::vector<TLine> TLineC::GetIn(float x1, float y1, float x2, float y2, bool cross)
{
	if (cross) return GetCross(x1, y1, x2, y2);
	std::vector<TLine> ret;

	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);

	for (size_t ii = 0; ii < Line.size(); ii++)
	{
		float minx = std::min(Line[ii].x1, Line[ii].x2);
		float maxx = std::max(Line[ii].x1, Line[ii].x2);
		float miny = std::min(Line[ii].y1, Line[ii].y2);
		float maxy = std::max(Line[ii].y1, Line[ii].y2);

		if (minx >= x1 &&
			maxx <= x2 &&
			miny >= y1 &&
			maxy <= y2)
		{
			ret.push_back(Line[ii]);
		}
	}

	return ret;
}

//
//Wylicza i zwraca maxymalne i minimalne wspolrzedne
TMaxMinOb TLineC::GetMaxMinWsp(void)
{
	TMaxMinOb wynik = { 0 };

	if (Line.empty())
		return wynik;

	wynik.minx = wynik.maxx = Line[0].x1;
	wynik.miny = wynik.maxy = Line[0].y1;

	for (const auto& l : Line)
	{
		wynik.minx = std::min(wynik.minx, std::min(l.x1, l.x2));
		wynik.maxx = std::max(wynik.maxx, std::max(l.x1, l.x2));
		wynik.miny = std::min(wynik.miny, std::min(l.y1, l.y2));
		wynik.maxy = std::max(wynik.maxy, std::max(l.y1, l.y2));
	}

	return wynik;
}

//zwraca typ objektu
ObjectType TLineC::GetObjectType(void)
{
	return TypeId;
}

//
//zwraca ilosc bajtow jednego objektu
size_t TLineC::ObSize(void)
{
	return sizeof(TLine);
}

//zwraca ilosc objektow
size_t TLineC::Size(void)
{
	return Line.size();
}


TEST(test_make_line_0)
{
	TLineC line;
	CHECK(line.Line.size() == 0);	
}

TEST(test_make_line_1)
{
	TLineC line;
	line.Add(TLine{0,0,0,0,0,0,0});

	CHECK(line.Line.size() == 1);

	TLine line1{ 0,0,0,0,0,0,0 };
	TLine line2{ 0,0,0,0,0,0,0 };
}

TEST(test_line_equal)
{
	TLineC line;
	line.Add(TLine{ 0,0,0,0,0,0,0 });

	CHECK(line.Line.size() == 1);
}

TEST(test_make_line_equal)
{
	TLineC line;
	line.Add(TLine{ 0,0,0,0,0,0,0 });

	TLine line1{ 0,0,0,0,0,0,0 };
	TLine line2{ 0,0,1,0,0,0,0 };

	CHECK(line.Equal(line.Line[0], line1));
	CHECK(!line.Equal(line.Line[0], line2));
}
