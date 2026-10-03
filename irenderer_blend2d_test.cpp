#include "test.hpp"
#include "irenderer_blend2d.hpp"
#include <filesystem>
#include "line.h"
#include "get_linie.hpp"

TEST(draw_line)
{
	BL2DRenderer blend(800, 600);
	std::filesystem::remove("test.png");
	CHECK(!std::filesystem::exists("test.png"));
	blend.SetLineSizeColour(3, RenderPixel(255, 0, 0));
	blend.Line(0, 0, 800, 600);
	blend.Line(100, 200, 30, 60);
	blend.Save("test.png");
	CHECK(std::filesystem::exists("test.png"));
	CHECK(std::filesystem::file_size("test.png"));
}

TEST(GetMaxMinLineData)
{
	//size_t GetMaxMinLineData(std::vector<TLine> &data, const std::string & sciezka, float zoom, int szerokosc,
	//    int wysokosc, float srodek_x, float srodek_y, float margines_px = 2)

	int wi = 800, he = 600;
	std::vector<TLine> data;
	CHECK(data.size() == 0);

	GetMaxMinLineData(data, "linie.bin", 1, wi, he, 0, 0);
	CHECK(data.size() > 0);
	TLineC line;
	line.Add(data);

	auto maxmin=line.GetMaxMinWsp();

	float zoom_x = wi / (maxmin.maxx - maxmin.minx);
	float zoom_y = he / (maxmin.maxy - maxmin.miny);

	float move_x = (maxmin.minx + maxmin.maxx) / 2.0f;
	float move_y = (maxmin.miny + maxmin.maxy) / 2.0f;

	BL2DRenderer blend(wi, he);
	blend.SetLineSizeColour(1, RenderPixel(255, 0, 0));

	for (int ii = 0; ii < line.Size(); ii++)
	{
		blend.Line(
			(line.Line[ii].x1 - move_x) * zoom_x + wi / 2,
			(line.Line[ii].y1 - move_y) * zoom_y + he / 2,
			(line.Line[ii].x2 - move_x) * zoom_x + wi / 2,
			(line.Line[ii].y2 - move_y) * zoom_y + he / 2);
	}

	blend.Save("test1.png");
}
