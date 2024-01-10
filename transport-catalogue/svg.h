#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>

namespace svg {

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };


    class ObjectContainer {
    public:
        void virtual AddPtr(std::unique_ptr<Object>&& obj) = 0;
        template<typename T>
        void Add(T obj);
        virtual ~ObjectContainer() = default;
    };

    class Drawable {
    public:
        virtual void Draw(svg::ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };

    using Color = std::string;

    // Объявив в заголовочном файле константу со спецификатором inline,
    // мы сделаем так, что она будет одной на все единицы трансляции,
    // которые подключают этот заголовок.
    // В противном случае каждая единица трансляции будет использовать свою копию этой константы
    inline const Color NoneColor{ "none" };

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    std::ostream& operator<<(std::ostream& output, StrokeLineCap stroke_linecap);

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    std::ostream& operator<<(std::ostream& output, StrokeLineJoin stroke_linejoin);

    template<typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color);
        Owner& SetStrokeColor(Color color);
        Owner& SetStrokeWidth(double width);
        Owner& SetStrokeLineCap(StrokeLineCap line_cap);
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join);
    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const;

    private:
        Owner& AsOwner();
        std::optional<Color> fill_;
        std::optional<Color> stroke_;
        std::optional<double> stroke_width;
        std::optional<StrokeLineCap> stroke_linecap;
        std::optional<StrokeLineJoin> stroke_linejoin;
    };

    class Circle final : public Object, public PathProps<Circle> {
    public:
        //~Circle() = delete;
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;
        Point center_;
        double radius_ = 1.0;
    };


    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;
        std::vector<Point> points_;
    };


    class Text final : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

        // Прочие данные и методы, необходимые для реализации элемента <text>
    private:
        void RenderObject(const RenderContext& context) const override;
        Point pos_ = { 0.0, 0.0 };
        Point offset_ = { 0.0, 0.0 };
        uint32_t font_size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;
    };

    class Document : public ObjectContainer {
    public:
        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj);

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;

        // Прочие методы и данные, необходимые для реализации класса Document
    private:
        std::vector<std::unique_ptr<Object>> objs;
    };

    template<typename T>
    void ObjectContainer::Add(T obj) {
        AddPtr(std::make_unique<T>(std::move(obj)));
    }

    template<typename Owner>
    Owner& PathProps<Owner>::SetFillColor(Color color) {
        fill_ = std::move(color);
        return AsOwner();
    }
    template<typename Owner>
    Owner& PathProps<Owner>::SetStrokeColor(Color color) {
        stroke_ = std::move(color);
        return AsOwner();
    }
    template<typename Owner>
    Owner& PathProps<Owner>::SetStrokeWidth(double width) {
        stroke_width = std::move(width);
        return AsOwner();
    }
    template<typename Owner>
    Owner& PathProps<Owner>::SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_linecap = std::move(line_cap);
        return AsOwner();
    }
    template<typename Owner>
    Owner& PathProps<Owner>::SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_linejoin = std::move(line_join);
        return AsOwner();
    }
    template<typename Owner>
    Owner& PathProps<Owner>::AsOwner() {
        return static_cast<Owner&>(*this);
    }
    template<typename Owner>
    void PathProps<Owner>::RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_) {
            out << " fill=\""sv << *fill_ << "\""sv;
        }
        if (stroke_) {
            out << " stroke=\""sv << *stroke_ << "\""sv;
        }
        if (stroke_width) {
            out << " stroke-width=\""sv << *stroke_width << "\""sv;
        }
        if (stroke_linecap) {
            out << " stroke-linecap=\""sv << *stroke_linecap << "\""sv;
        }
        if (stroke_linejoin) {
            out << " stroke-linejoin=\""sv << *stroke_linecap << "\""sv;
        }
    }

}  // namespace svg