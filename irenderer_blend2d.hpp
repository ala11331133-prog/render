#include <blend2d/blend2d.h>
#include "irenderer.hpp"

//
//implementacja za pomoca blend2d abstrakcji rysującej
class BL2DRenderer : public IRenderer
{
    BLImage image;
    BLContext ctx;
    BLFont font;

public:
    BL2DRenderer(int width, int height,
        const std::filesystem::path& fontPath = "fonts",
        float fontSize = 16.0f)
        : image(width, height, BL_FORMAT_PRGB32), ctx(image)
    {
        BLFontFace face;
        face.create_from_file(fontPath.string().c_str());
        font.create_from_face(face, fontSize);

        BLImage image(800, 600, BL_FORMAT_PRGB32);
        ctx.fill_all(BLRgba32(0, 0, 0, 0));
    }

    void SetLineSizeColour(int line_size, unsigned char R, unsigned char G, unsigned char B) override
    {
        //BLImage image(800, 600, BL_FORMAT_PRGB32);
        //BLContext ctx(image);
        //ctx.fill_all(BLRgba32(0, 0, 0, 0));

        ctx.set_stroke_width(line_size);
        ctx.set_stroke_style(BLRgba32(255, 0, 0));
    }

    void SetImageSize(int w, int h, unsigned char R, unsigned char G, 
                      unsigned char B, unsigned char A) override
    {
        BLImage image(w, h, BL_FORMAT_PRGB32);
        ctx.fill_all(BLRgba32(0, 0, 0, 0));
    }

    void Line(float x1, float y1, float x2, float y2) override
    {
        ctx.stroke_line(BLPoint(x1, y1), BLPoint(x2, y2));
    }

    void Text(float x, float y, float kat,
        const std::string& text) override
    {
        ctx.save();
        ctx.translate(x, y);
        ctx.rotate(kat);
        ctx.fill_utf8_text(BLPoint(0, 0), font, text.c_str());
        ctx.restore();
    }

    void TextOnLines(
        const std::vector<RenderLine>& lines, const std::string& text) override
    {
        if (lines.empty() || text.empty())
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

        for (const RenderLine& line : lines)
        {
            double dx = line.x2 - line.x1;
            double dy = line.y2 - line.y1;
            double length = std::hypot(dx, dy);

            if (length < 0.001)
                continue;

            segments.push_back({ line.x1, line.y1, line.x2, line.y2, length,
                std::atan2(dy, dx), total_length });

            total_length += length;
        }

        if (segments.empty())
            return;

        BLGlyphBuffer gb;
        gb.set_utf8_text(text.c_str());

        font.shape(gb);

        const BLGlyphRun& run = gb.glyph_run();

        if (!run.size) return;

        const uint32_t* glyph_data = run.glyph_data_as<uint32_t>();

        if (!glyph_data) return;

        std::vector<BLGlyphPlacement> placements(run.size);

        font.get_glyph_advances(glyph_data, run.glyph_advance, placements.data(), run.size);

        double text_length = 0.0;

        for (const auto& p : placements)
            text_length += p.advance.x;

        if (text_length <= 0.001) return;

        double scale = total_length / text_length;
        double distance = 0.0;

        constexpr double PI = 3.14159265358979323846;

        for (size_t i = 0; i < run.size; ++i)
        {
            double glyph_distance = distance * scale;

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

            double local = glyph_distance - segment->start;
            double t = local / segment->length;

            double x = segment->x1 +
                (segment->x2 - segment->x1) * t;

            double y = segment->y1 +
                (segment->y2 - segment->y1) * t;

            double angle = segment->angle;

            if (angle > PI / 2.0)
                angle -= PI;

            if (angle < -PI / 2.0)
                angle += PI;

            BLGlyphRun glyph_run = run;

            glyph_run.glyph_data =
                const_cast<uint32_t*>(glyph_data + i);

            glyph_run.glyph_advance = sizeof(uint32_t);
            glyph_run.size = 1;

            glyph_run.placement_type =
                BL_GLYPH_PLACEMENT_TYPE_ADVANCE_OFFSET;

            glyph_run.placement_data =
                const_cast<BLGlyphPlacement*>(&placements[i]);

            glyph_run.placement_advance =
                sizeof(BLGlyphPlacement);

            ctx.save();
            ctx.translate(x, y);
            ctx.rotate(angle);

            ctx.fill_glyph_run(
                BLPoint(0, 0),
                font,
                glyph_run);

            ctx.restore();

            distance += placements[i].advance.x;
        }
    }

    bool Save(const std::filesystem::path& file) override
    {
        return image.write_to_file(file.string().c_str());
    }
};
