#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    std::ostream& operator<<(std::ostream& output, StrokeLineCap stroke_linecap) {
        switch (stroke_linecap) {
        case (StrokeLineCap::BUTT):
            output << "butt"sv;
            break;
        case (StrokeLineCap::ROUND):
            output << "round"sv;
            break;
        case (StrokeLineCap::SQUARE):
            output << "square"sv;
            break;
        }
        return output;
    }

    std::ostream& operator<<(std::ostream& output, StrokeLineJoin stroke_linejoin) {
        switch (stroke_linejoin) {
        case (StrokeLineJoin::ARCS):
            output << "arcs"sv;
            break;
        case (StrokeLineJoin::BEVEL):
            output << "bevel"sv;
            break;
        case (StrokeLineJoin::MITER):
            output << "miter"sv;
            break;
        case (StrokeLineJoin::MITER_CLIP):
            output << "miter-clip"sv;
            break;
        case (StrokeLineJoin::ROUND):
            output << "round"sv;
            break;
        }
        return output;
    }


    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point p) {
        points_.push_back(p);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        if (points_.empty()) {
            out << "\" />"sv;
            return;
        }
        const int last_point = points_.size() - 1;
        for (int i = 0; i < last_point; i++) {
            out << points_[i].x << ","sv << points_[i].y << " "sv;
        }
        out << points_[last_point].x << ","sv << points_[last_point].y << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ------------- Text ------------------

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << font_size_ << "\""sv;
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        out << ">"sv;
        for (auto c : data_) {
            switch (c)
            {
            case ('\"'):
                out << "&quot;"sv;
                break;
            case ('\''):
                out << "&apos;"sv;
                break;
            case ('<'):
                out << "&lt;"sv;
                break;
            case ('>'):
                out << "&gt;"sv;
                break;
            case ('&'):
                out << "&amp;"sv;
                break;
            default:
                out << c;
            }
        }
        out << "</text>"sv;
    }

    // ------------- Document ------------------
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objs.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& obj : objs) {
            obj->Render({ out, 2, 2 });
        }
        out << "</svg>"sv;
    }

}  // namespace svg