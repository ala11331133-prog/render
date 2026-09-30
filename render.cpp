// render.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "line.h"
#include "obiekt.h"
#include "test.h"
#include "header.hpp"
#include <blend2d/blend2d.h>
#define M_PI 3.1415


/*#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkPaint.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkFont.h"
#include "include/core/SkData.h"
#include "include/core/SkFontMgr.h"
#include "include/ports/SkFontMgr_data.h"*/
//todo: Blend2D sprawdzic


std::vector<TLine> GetLine(float x1, float y1, float x2, float y2, int warstwa)
{
    std::vector<TLine> line;

    //line.push_back();
    return line;
}

std::vector<TLine> GetLine(TMaxMinOb ob, int warstwa)
{
    return GetLine(ob.minx, ob.miny, ob.maxx, ob.maxy, warstwa);
}

void draw_text_on_lines_old(
    BLContext& ctx,
    const std::vector<TLine>& lines,
    const char* text,
    const BLFont& font,
    const BLRgba32& line_color,
    double line_width,
    const BLRgba32& text_color)
{
    if (lines.empty() || !text || !*text)
        return;

    struct Segment
    {
        double x1, y1;
        double x2, y2;
        double length;
        double angle;
        double start;
    };

    std::vector<Segment> segments;

    double total_length = 0.0;

    // Linie
    ctx.set_stroke_width(line_width);
    ctx.set_stroke_style(line_color);

    for (const TLine& line : lines)
    {
        double dx = line.x2 - line.x1;
        double dy = line.y2 - line.y1;

        double length = std::hypot(dx, dy);

        if (length < 0.001)
            continue;

        ctx.stroke_line(
            BLPoint(line.x1, line.y1),
            BLPoint(line.x2, line.y2)
        );

        segments.push_back({
            line.x1,
            line.y1,
            line.x2,
            line.y2,
            length,
            std::atan2(dy, dx),
            total_length
            });

        total_length += length;
    }

    if (total_length < 0.001)
        return;

    // Tekst -> glyphy
    BLGlyphBuffer gb;
    gb.set_utf8_text(text);

    font.shape(gb);

    const BLGlyphRun& run = gb.glyph_run();

    if (run.size == 0)
        return;

    // Całkowita szerokość tekstu
    BLTextMetrics metrics;
    font.get_text_metrics(gb, metrics);

    double text_width = metrics.advance.x;

    if (text_width <= 0.001)
        return;

    /*
        glyph_data jest void*, ale BLGlyphId = uint32_t.
    */

    const uint8_t* glyph_data =
        static_cast<const uint8_t*>(run.glyph_data);

    const size_t glyph_step =
        static_cast<size_t>(run.glyph_advance);

    /*
        Przy placement NONE nie mamy advance pojedynczych glyphów
        w glyph_run.

        Dlatego używamy równomiernego podziału całego tekstu.
        Nie zmienia to jednak samego glyphu - tylko jego pozycję.
    */

    const double glyph_spacing =
        total_length / static_cast<double>(run.size);

    ctx.set_fill_style(text_color);

    for (size_t i = 0; i < run.size; ++i)
    {
        // Odczytujemy ID glyphu.
        BLGlyphId glyph_id = 0;

        std::memcpy(
            &glyph_id,
            glyph_data + i * glyph_step,
            sizeof(BLGlyphId)
        );

        (void)glyph_id;

        // Środek pozycji glyphu na całej ścieżce.
        const double distance =
            glyph_spacing * (static_cast<double>(i) + 0.5);

        // Szukamy segmentu.
        const Segment* segment = nullptr;

        for (const Segment& s : segments)
        {
            if (distance >= s.start &&
                distance <= s.start + s.length)
            {
                segment = &s;
                break;
            }
        }

        if (!segment)
            continue;

        // Pozycja na segmencie.
        const double local =
            distance - segment->start;

        const double t =
            local / segment->length;

        const double x =
            segment->x1 +
            (segment->x2 - segment->x1) * t;

        const double y =
            segment->y1 +
            (segment->y2 - segment->y1) * t;

        double angle = segment->angle;

        // Tekst zawsze czytelny.
        if (angle > M_PI/2)
            angle -= M_PI;

        if (angle < -M_PI/2)
            angle += M_PI;

        // Tworzymy run zawierający tylko jeden glyph.
        BLGlyphRun single_run = run;

        single_run.glyph_data =
            const_cast<uint8_t*>(
                glyph_data + i * glyph_step
                );

        single_run.size = 1;

        // Jeden glyph nie potrzebuje pozostałych danych.
        single_run.placement_data = nullptr;

        ctx.save();

        ctx.translate(x, y);
        ctx.rotate(angle);

        ctx.fill_glyph_run(
            BLPoint(0, 0),
            font,
            single_run
        );

        ctx.restore();
    }
}

void draw_text_on_lines(
    BLContext& ctx,
    const std::vector<TLine>& lines,
    const char* text,
    const BLFont& font,
    const BLRgba32& line_color,
    double line_width,
    const BLRgba32& text_color)
{
    if (lines.empty() || !text || !*text)
        return;

    struct Segment
    {
        double x1;
        double y1;
        double x2;
        double y2;
        double length;
        double angle;
        double start;
    };

    std::vector<Segment> segments;
    double total_length = 0.0;

    // ------------------------------------------------------------
    // Linie
    // ------------------------------------------------------------

    ctx.set_stroke_width(line_width);
    ctx.set_stroke_style(line_color);

    for (const TLine& line : lines)
    {
        const double dx = line.x2 - line.x1;
        const double dy = line.y2 - line.y1;

        const double length = std::hypot(dx, dy);

        if (length < 0.001)
            continue;

        ctx.stroke_line(
            BLPoint(line.x1, line.y1),
            BLPoint(line.x2, line.y2)
        );

        segments.push_back({
            line.x1,
            line.y1,
            line.x2,
            line.y2,
            length,
            std::atan2(dy, dx),
            total_length
            });

        total_length += length;
    }

    if (segments.empty())
        return;

    // ------------------------------------------------------------
    // Tekst -> glyphy
    // ------------------------------------------------------------

    BLGlyphBuffer gb;
    gb.set_utf8_text(text);

    font.shape(gb);

    const BLGlyphRun& run = gb.glyph_run();

    if (run.size == 0)
        return;

    // ------------------------------------------------------------
    // Glyph ID
    // ------------------------------------------------------------

    const uint32_t* glyph_data =
        run.glyph_data_as<uint32_t>();

    if (!glyph_data)
        return;

    // ------------------------------------------------------------
    // Pobieramy rzeczywiste advance każdego glyphu
    // ------------------------------------------------------------

    std::vector<BLGlyphPlacement> placements(run.size);

    font.get_glyph_advances(
        glyph_data,
        run.glyph_advance,
        placements.data(),
        run.size
    );

    // ------------------------------------------------------------
    // Całkowita długość tekstu
    // ------------------------------------------------------------

    double text_length = 0.0;

    for (const BLGlyphPlacement& p : placements)
        text_length += p.advance.x;

    if (text_length <= 0.001)
        return;

    // ------------------------------------------------------------
    // Rozciągnięcie tekstu na całą długość linii
    // ------------------------------------------------------------

    const double scale =
        total_length / text_length;

    ctx.set_fill_style(text_color);

    // ------------------------------------------------------------
    // Rysujemy glyph po glyphie
    // ------------------------------------------------------------

    double distance = 0.0;

    for (size_t i = 0; i < run.size; ++i)
    {
        const BLGlyphPlacement& placement =
            placements[i];

        // Pozycja początku glyphu.
        const double glyph_distance =
            distance * scale;

        // --------------------------------------------------------
        // Znajdź segment
        // --------------------------------------------------------

        const Segment* segment = nullptr;

        for (const Segment& s : segments)
        {
            if (glyph_distance >= s.start &&
                glyph_distance <= s.start + s.length)
            {
                segment = &s;
                break;
            }
        }

        if (!segment)
            break;

        // --------------------------------------------------------
        // Pozycja na segmencie
        // --------------------------------------------------------

        const double local =
            glyph_distance - segment->start;

        const double t =
            local / segment->length;

        const double x =
            segment->x1 +
            (segment->x2 - segment->x1) * t;

        const double y =
            segment->y1 +
            (segment->y2 - segment->y1) * t;

        double angle = segment->angle;

        // Tekst nie może być do góry nogami.
        constexpr double PI = 3.14159265358979323846;

        if (angle > PI / 2.0)
            angle -= PI;

        if (angle < -PI / 2.0)
            angle += PI;

        // --------------------------------------------------------
        // Tworzymy run zawierający jeden glyph
        // --------------------------------------------------------

        BLGlyphRun glyph_run = run;

        glyph_run.glyph_data =
            const_cast<uint32_t*>(glyph_data + i);

        glyph_run.glyph_advance =
            sizeof(uint32_t);

        glyph_run.size = 1;

        glyph_run.placement_type =
            BL_GLYPH_PLACEMENT_TYPE_ADVANCE_OFFSET;

        glyph_run.placement_data =
            const_cast<BLGlyphPlacement*>(&placement);

        glyph_run.placement_advance =
            sizeof(BLGlyphPlacement);

        // --------------------------------------------------------
        // Rysowanie
        // --------------------------------------------------------

        ctx.save();

        ctx.translate(x, y);
        ctx.rotate(angle);

        ctx.fill_glyph_run(
            BLPoint(0, 0),
            font,
            glyph_run
        );

        ctx.restore();

        distance += placement.advance.x;
    }
}
int blend_test()
{
    BLImage image(800, 600, BL_FORMAT_PRGB32);
    BLContext ctx(image);
    ctx.fill_all(BLRgba32(255, 255, 255, 255));

    ctx.set_stroke_width(3.0);
    ctx.set_stroke_style(BLRgba32(255, 0, 0));

    //ctx.stroke_line(
    //    BLPoint(100, 100),
    //    BLPoint(700, 500)
    //);

    BLFontFace face;
    face.create_from_file("C:\\Windows\\Fonts\\arial.ttf");

    BLFont font;
    font.create_from_face(face, 12.0);

    TLineC line;
    std::vector<TLine> lines;

    line.Add(TLine{ 0,0,0,0,0,50,50 });
    line.Add(TLine{ 0,0,0,50,50,120,150 });
    line.Add(TLine{ 0,0,0,120,150,250,10 });

    line.Save(lines);

    draw_text_on_lines_old(
        ctx,
        lines,
        "HELLO BLEND2D",
        font,
        BLRgba32(255, 0, 0),
        3.0,
        BLRgba32(0, 0, 0)
    );

    ctx.end();

    image.write_to_file("output.png");

    return 0;
}

//TODO: to srednio tutaj pasuje gdzies trzeba przeniesc
std::vector<unsigned char> GetObjectData(std::vector<IObiekt*>& objects)
{
    FileHeader fileHeader;
    fileHeader.objects_header = (std::uint32_t)objects.size();

    std::vector<unsigned char> ret;

    auto data = fileHeader.SaveBytes();
    ret.insert(ret.end(), data.begin(), data.end());

    for (IObiekt* ob : objects)
    {
        ObjectHeader objectHeader;

        objectHeader.type = ob->GetObjectType();
        objectHeader.byte_size = ob->ObSize();
        objectHeader.ob_count = ob->Size();

        data = objectHeader.SaveBytes();
        ret.insert(ret.end(), data.begin(), data.end());

        data = ob->SaveBytes();
        ret.insert(ret.end(), data.begin(), data.end());
    }

    return ret;
}

TEST(GetObjectData_Empty)
{
    CHECK(sizeof(FileHeader) == 4 + 4 + 4 + 8);
    CHECK(sizeof(ObjectHeader) == 4 + 8 + 8);
    //tutaj trzeba wypelnic dane
    TLineC ob1;

    std::vector<IObiekt*> ob{&ob1};
    auto data = GetObjectData(ob);
    CHECK(data.size() == sizeof(FileHeader) + sizeof(ObjectHeader));

    CHECK(data[0]=='M');
    CHECK(data[1]=='A');
    CHECK(data[2]=='P');
    CHECK(data[3]=='1');

    FileHeader fileHeader;

    std::vector<unsigned char> fileData(
        data.begin(),
        data.begin() + sizeof(FileHeader)
    );

    CHECK(fileHeader.LoadBytes(fileData));

    CHECK(fileHeader.IsValid());
    CHECK(fileHeader.version == 1);
    CHECK(fileHeader.objects_header == 1);
    CHECK(fileHeader.offset == sizeof(FileHeader));

    ObjectHeader objectHeader;

    std::vector<unsigned char> objectData(
        data.begin() + sizeof(FileHeader),
        data.end()
    );

    CHECK(objectHeader.LoadBytes(objectData));

    CHECK(objectHeader.type == ObjectType::Line);
    CHECK(objectHeader.byte_size == sizeof(TLine));
    CHECK(objectHeader.ob_count == 0);
}

TEST(GetObjectData_elements)
{
    CHECK(sizeof(FileHeader) == 4 + 4 + 4 + 8);
    CHECK(sizeof(ObjectHeader) == 4 + 8 + 8);
    //tutaj trzeba wypelnic dane
    TLineC ob1;
    ob1.Add(TLine{ 1, 2, 3, 4, 5, 0, 0 });
    ob1.Add(TLine{ 9, 8, 7, 6, 5, 5, 3 });

    std::vector<IObiekt*> ob{ &ob1 };
    auto data = GetObjectData(ob);
    CHECK(data.size() == sizeof(FileHeader) + sizeof(ObjectHeader) + ob1.ObSize()*ob1.Size());

    CHECK(data[0] == 'M');
    CHECK(data[1] == 'A');
    CHECK(data[2] == 'P');
    CHECK(data[3] == '1');

    FileHeader fileHeader;

    std::vector<unsigned char> fileData(
        data.begin(),
        data.begin() + sizeof(FileHeader)
    );

    CHECK(fileHeader.LoadBytes(fileData));

    CHECK(fileHeader.IsValid());
    CHECK(fileHeader.version == 1);
    CHECK(fileHeader.objects_header == 1);
    CHECK(fileHeader.offset == sizeof(FileHeader));

    ObjectHeader objectHeader;

    std::vector<unsigned char> objectData(
        data.begin() + sizeof(FileHeader),
        data.begin() + sizeof(FileHeader) + sizeof(ObjectHeader)
    );

    CHECK(objectHeader.LoadBytes(objectData));

    CHECK(objectHeader.type == ObjectType::Line);
    CHECK(objectHeader.byte_size == sizeof(TLine));
    CHECK(objectHeader.ob_count == 2);

    std::vector<unsigned char> lineData(
        data.begin() + sizeof(FileHeader) + sizeof(ObjectHeader),
        data.end());
    std::vector<TLine> lines(lineData.size() / sizeof(TLine));
    std::memcpy(lines.data(), lineData.data(),lineData.size() );
    CHECK(ob1.Load(lines) == 2);

    CHECK(ob1.Size() == 2);

    CHECK(ob1.Line[0].reserved == 1);
    CHECK(ob1.Line[0].TextIndex == 2);
    CHECK(ob1.Line[0].warstwa == 3);
    CHECK(ob1.Line[0].x1 == 4);
    CHECK(ob1.Line[0].y1 == 5);
    CHECK(ob1.Line[0].x2 == 0);
    CHECK(ob1.Line[0].y2 == 0);

    CHECK(ob1.Line[1].reserved == 9);
    CHECK(ob1.Line[1].TextIndex == 8);
    CHECK(ob1.Line[1].warstwa == 7);
    CHECK(ob1.Line[1].x1 == 6);
    CHECK(ob1.Line[1].y1 == 5);
    CHECK(ob1.Line[1].x2 == 5);
    CHECK(ob1.Line[1].y2 == 3);
}

int main()
{
    blend_test();
    /*SkBitmap bitmap;
    bitmap.allocN32Pixels(800, 600);
    bitmap.eraseColor(SK_ColorWHITE);

    SkCanvas canvas(bitmap);

    SkPaint paint;
    paint.setColor(SK_ColorRED);
    paint.setStrokeWidth(3);
    paint.setStyle(SkPaint::kStroke_Style);

    //linia
    canvas.drawLine(50, 50, 750, 550, paint);
    //font
    auto fontData = SkData::MakeFromFileName("fonts/Arial.ttf");

    if (!fontData)
        return 10;

    sk_sp<SkData> fonts[] = {
        fontData
    };

    SkSpan<sk_sp<SkData>> fontSpan(fonts, 1);

    auto fontMgr = SkFontMgr_New_Custom_Data(fontSpan);

    if (!fontMgr)
        return 11;

    auto typeface = fontMgr->makeFromData(fontData);

    if (!typeface)
        return 12;

    SkFont font(typeface, 48);

    SkPaint textPaint;
    textPaint.setColor(SK_ColorBLUE);

    const char* text = "Hello Skia!";

    canvas.drawSimpleText(
        text,
        strlen(text),
        SkTextEncoding::kUTF8,
        100,
        200,
        font,
        textPaint
    );



    SkPngEncoder::Options options;
    SkFILEWStream stream("output.png");

    if (!stream.isValid())
        return 1;

    if (!SkPngEncoder::Encode(&stream, bitmap.pixmap(), options))
        return 2;*/

    return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
