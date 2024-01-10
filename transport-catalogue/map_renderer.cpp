#include "map_renderer.h"

using namespace std;

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */
svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
    return {
        (coords.lng - min_lon_) * zoom_coeff_ + padding_,
        (max_lat_ - coords.lat) * zoom_coeff_ + padding_
    };
}

void RenderLine(const vector<vector<geo::Coordinates>>& routes, svg::Document& map, const Settings& st, const SphereProjector& proj) {
    using namespace svg;
    const auto& colors = st.color_palette;
    size_t colors_count = 0;
    size_t colors_limit = st.color_palette.size();

    for (const auto& route : routes) {
        if (route.empty()) {
            continue;
        }
        Polyline line;
        line.SetStrokeWidth(st.line_width);
        line.SetStrokeLineCap(StrokeLineCap::ROUND);
        line.SetStrokeLineJoin(StrokeLineJoin::ROUND);
        line.SetFillColor("none"s);
        line.SetStrokeColor(colors.at(colors_count));
        for (size_t i = 0; i < route.size(); i++) {
            line.AddPoint(proj(route.at(i)));
        }
        map.Add(line);
        colors_count++;
        if (colors_count == colors_limit) {
            colors_count = 0;
        }
    }
}

void RenderNameBus(RequestHandler::BusInf buses, svg::Document& map, const Settings& st, const SphereProjector& proj) {
    using namespace svg;
    const auto& colors = st.color_palette;
    size_t colors_count = 0;
    size_t colors_limit = st.color_palette.size();

    for (const auto& bus : buses) {
        Text num_text;
        num_text.SetPosition(proj(bus.second.first));
        num_text.SetOffset(st.bus_label_offset);
        num_text.SetFontSize(st.bus_label_font_size);
        num_text.SetFontFamily("Verdana"s);
        num_text.SetFontWeight("bold"s);
        num_text.SetData(bus.first);
        Text num_text_ = num_text;
        num_text.SetFillColor(st.underlayer_color);
        num_text.SetStrokeColor(st.underlayer_color);
        num_text.SetStrokeWidth(st.underlayer_width);
        num_text.SetStrokeLineCap(StrokeLineCap::ROUND);
        num_text.SetStrokeLineJoin(StrokeLineJoin::ROUND);
        num_text_.SetFillColor(colors.at(colors_count));
        map.Add(num_text);
        map.Add(num_text_);
        if (bus.second.first != bus.second.second) {
            num_text.SetPosition(proj(bus.second.second));
            num_text_.SetPosition(proj(bus.second.second));
            map.Add(num_text);
            map.Add(num_text_);
        }
        colors_count++;
        if (colors_count == colors_limit) {
            colors_count = 0;
        }
    }
}

void RenderCircle(const vector<geo::Coordinates>& stops, svg::Document& map, const Settings& st, const SphereProjector& proj) {
    using namespace svg;

    for (const auto& stop : stops) {
        Circle circle;
        circle.SetCenter(proj(stop));
        circle.SetFillColor("white"s);
        circle.SetRadius(st.stop_radius);
        map.Add(circle);
    }
}

void RenderNameStop(set<Stop> stops, svg::Document& map, const Settings& st, const SphereProjector& proj) {
    using namespace svg;
    for (const auto& stop : stops) {
        Text text_stop;
        text_stop.SetPosition(proj(stop.coord));
        text_stop.SetOffset(st.stop_label_offset);
        text_stop.SetFontSize(st.stop_label_font_size);
        text_stop.SetFontFamily("Verdana"s);
        text_stop.SetData(stop.name);
        Text text_stop_ = text_stop;
        text_stop.SetFillColor(st.underlayer_color);
        text_stop.SetStrokeColor(st.underlayer_color);
        text_stop.SetStrokeWidth(st.underlayer_width);
        text_stop.SetStrokeLineCap(StrokeLineCap::ROUND);
        text_stop.SetStrokeLineJoin(StrokeLineJoin::ROUND);
        text_stop_.SetFillColor("black"s);
        map.Add(text_stop);
        map.Add(text_stop_);
    }
}

svg::Document MapRender::RenderMap() {
    svg::Document map;
    vector<geo::Coordinates> geo_coords = rq.GetCordsStops();

    const SphereProjector proj{
        geo_coords.begin(), geo_coords.end(), settings.width , settings.height, settings.padding
    };

    const auto& routes = rq.GetRoutes();
    RenderLine(routes, map, settings, proj);

    const auto& busname = rq.GetBusNamePoint();
    RenderNameBus(busname, map, settings, proj);
    RenderCircle(geo_coords, map, settings, proj);
    RenderNameStop(rq.GetStops(), map, settings, proj);
    return map;
}