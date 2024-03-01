#include "json_reader.h"

using namespace std;

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

void JsonReader::AddBuses(std::vector<BusAdd>& buses) {
	for (auto& bus : buses) {
		tr_.AddBus(bus.name, bus.stops, bus.is_round);
	}
}

void JsonReader::AddDistances(std::vector <DistancesAdd>& distances) {
	for (auto& dist : distances) {
		tr_.AddDistance(dist.stop_from, dist.stop_to, dist.distance);
	}
}

void JsonReader::OutPutAnswers(std::ostream& out) const {
	RequestHandler rq(tr_);
	auto& stat_requests = doc_.GetRoot().AsMap().at("stat_requests").AsArray();

	
    json::Builder ans;
    ans.StartArray();
    TransportRouter router(tr_, route_settings_);
    
	for (auto& request : stat_requests) {
		auto& req = request.AsMap();
		auto& type = req.at("type").AsString();
		if (type == "Stop") {
            ans.StartDict().Key("request_id").Value(req.at("id").AsInt());
			auto stop_info = rq.GetBusesByStop(req.at("name").AsString());
			if (!stop_info) {
                ans.Key("error_message").Value("not found");
			}
			else {
                ans.Key("buses").StartArray();
				for (auto& bus : stop_info.value().buses) {
                    ans.Value(string(bus));
				}
                ans.EndArray();
			}
            ans.EndDict();
		}
		else if (type == "Bus") {
            ans.StartDict().Key("request_id").Value(req.at("id").AsInt());
			auto bus_info_ = rq.GetBusStat(req.at("name").AsString());
			if (!bus_info_) {
                ans.Key("error_message").Value("not found");
			}
			else {
				auto& bus_info = bus_info_.value();
                ans.Key("route_length").Value(bus_info.distance);
                ans.Key("stop_count").Value(bus_info.stops);
                ans.Key("unique_stop_count").Value(bus_info.uniq_stops);
                ans.Key("curvature").Value(bus_info.curvature);
			}
            ans.EndDict();
		}
		else if (type == "Map") {
            ans.StartDict().Key("request_id").Value(req.at("id").AsInt());
			std::ostringstream s;
			MapRender mp(rq, settings);
			svg::Document map_ = mp.RenderMap();
			map_.Render(s);
            ans.Key("map").Value(s.str()).EndDict();
		}
		else if (type == "Route") {
			ans.StartDict().Key("request_id").Value(req.at("id").AsInt());
			
			auto response = router.BuildRoute(req.at("from").AsString(), req.at("to").AsString());
			if (response.has_value()) {
				ans.Key("total_time").Value(response->total_time).Key("items").StartArray();
				for (auto& resp : response->items) {
					ans.StartDict();
					if (holds_alternative<WaitResponse>(resp)) {
						auto& resp_ = get<WaitResponse>(resp);
						ans.Key("type").Value("Wait").Key("stop_name").Value(resp_.stop_name).Key("time").Value(resp_.time);
					}
					else {
						auto& resp_ = get<BusResponse>(resp);
						ans.Key("type").Value("Bus").Key("bus").Value(resp_.bus).Key("span_count").Value(resp_.span_count).Key("time").Value(resp_.time);
					}
					ans.EndDict();
				}
				ans.EndArray();
			}
			else {
				ans.Key("error_message").Value("not found");
			}
			ans.EndDict();
		}
	}
    ans.EndArray();
    json::Print(json::Document{ans.Build()}, out);
}

void JsonReader::FillTrCatalogue() {
	std::vector<DistancesAdd> distance;
	std::vector<BusAdd> buses;

	auto& base_requests = doc_.GetRoot().AsMap().at("base_requests").AsArray();

	for (const auto& request : base_requests) {
		const auto& req = request.AsMap();
		const auto& type = req.at("type").AsString();
		if (type == "Stop") {
			std::string name = req.at("name").AsString();
			double lat = req.at("latitude").AsDouble();
			double longit = req.at("longitude").AsDouble();
			tr_.AddStop(name, { lat, longit });
			auto& distance_ = req.at("road_distances").AsMap();
			for (auto& [stop, dist] : distance_) {
				distance.push_back({ name, stop, dist.AsInt() });
			}
		}
		else if (type == "Bus") {
			BusAdd bus{ req.at("name").AsString(), {}, req.at("is_roundtrip").AsBool() };
			auto& stops = req.at("stops").AsArray();
			for (auto& stop : stops) {
				bus.stops.push_back(stop.AsString());
			}
			if (!bus.is_round) {
				for (int i = static_cast<int>(stops.size() - 2); i >= 0; i--) {
					bus.stops.push_back(stops[i].AsString());
				}
			}
			buses.push_back(std::move(bus));
		}
		else {
			std::cerr << "Incorrect input";
			break;
		}
	}
	AddBuses(buses);
	AddDistances(distance);
}

svg::Color ColorFromNode(Node node) {
	svg::Color color;
	if (node.IsString()) {
		color += node.AsString();
	}
	else {
		if (node.IsArray() && node.AsArray().size() == 3) {
			color += "rgb("s;
		}
		else {
			color += "rgba("s;
		}
		bool first = true;
		for (const auto& num : node.AsArray()) {
			if (first) {
				first = false;
			}
			else {
				color += ","s;
			}
			if (num.IsInt()) {
				auto num_ = num.AsInt();
				ostringstream s;
				s << num_;
				color += s.str();
			}
			else {
				auto num_ = num.AsDouble();
				ostringstream s;
				s << num_;
				color += s.str();
			}
		}
		color += ")"s;
	}
	return color;
}

void JsonReader::SetingsRender() {
	
	auto& render_settings = doc_.GetRoot().AsMap().at("render_settings").AsMap();
	settings.width = render_settings.at("width"s).AsDouble();
	settings.height = render_settings.at("height"s).AsDouble();
	settings.padding = render_settings.at("padding"s).AsDouble();
	settings.line_width = render_settings.at("line_width"s).AsDouble();
	settings.stop_radius = render_settings.at("stop_radius"s).AsDouble();
	settings.bus_label_font_size = render_settings.at("bus_label_font_size"s).AsInt();
	settings.stop_label_font_size = render_settings.at("stop_label_font_size"s).AsInt();
	settings.underlayer_width = render_settings.at("underlayer_width"s).AsDouble();

	json::Array bslof = render_settings.at("bus_label_offset"s).AsArray();
	settings.bus_label_offset.x = bslof.at(0).AsDouble();
	settings.bus_label_offset.y = bslof.at(1).AsDouble();

	json::Array stlof = render_settings.at("stop_label_offset"s).AsArray();
	settings.stop_label_offset.x = stlof.at(0).AsDouble();
	settings.stop_label_offset.y = stlof.at(1).AsDouble();

	settings.underlayer_color = ColorFromNode(json::Node{ render_settings.at("underlayer_color"s) });

	vector<svg::Color> colors;
	json::Array colors_ = render_settings.at("color_palette"s).AsArray();
	for (auto& color : colors_) {
		colors.push_back(move(ColorFromNode(color)));
	}
	settings.color_palette = move(colors);
}

void JsonReader::SettingsRoute() {
	auto& route_settings = doc_.GetRoot().AsMap().at("routing_settings").AsMap();
	route_settings_.bus_velocity_ = route_settings.at("bus_velocity").AsDouble() * 1000 / 60;
	route_settings_.bus_wait_time_ = route_settings.at("bus_wait_time").AsInt();
}





//settings.bus_label_offset = render_settings.at("stop_radius"s).AsDouble();

