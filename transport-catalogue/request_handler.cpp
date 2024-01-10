#include "request_handler.h"
using namespace std;

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */
std::optional<BusInfo> RequestHandler::GetBusStat(const std::string_view& bus_name) const {

    auto result = db_.GetInfoBus(bus_name);
    if (result.name.empty()) {
        return nullopt;
    }
    else {
        return result;
    }
}

std::optional<StopInfo> RequestHandler::GetBusesByStop(const std::string_view& stop_name) const {
    auto result = db_.GetInfoStop(stop_name);
    if (result.name.empty()) {
        return nullopt;
    }
    else {
        return result;
    }
}

set<Stop> RequestHandler::GetStops() {
    set<Stop> stops_;
    const auto& stops = GetAllStops();
    for (const auto& stop : stops) {
        if (!stop.buses.empty()) {
            stops_.insert(stop);
        }    
    }
    return stops_;
}

vector<geo::Coordinates> RequestHandler::GetCordsStops() {
    vector<geo::Coordinates> cords;
    set<Stop> stops_ = GetStops();
    
    for (const auto& stop : stops_) {
        if (!stop.buses.empty()) {
            cords.push_back(stop.coord);
        }
    }
    return cords;
}

set<Bus> RequestHandler::GetBuses() {
    set<Bus> buses_;
    const auto& buses = GetAllBuses();
    for (const auto& bus : buses) {
        buses_.insert(bus);
    }
    return buses_;
}

vector<vector<geo::Coordinates>> RequestHandler::GetRoutes() {
    vector<vector<geo::Coordinates>> result;
    set<Bus> buses_ = GetBuses();
    for (const auto& bus : buses_) {
        vector<geo::Coordinates> route;
        for (const auto& stop : bus.stops) {
            route.push_back(move(db_.GetCoord(stop)));
        }
        result.push_back(move(route));
    }
    return result;
}


RequestHandler::BusInf RequestHandler::GetBusNamePoint() {
    set<Bus> buses = GetBuses();
    BusInf busnamepoint;
    for (const auto& bus : buses) {
        const string& name = bus.name;
        if (bus.is_round) {
            auto coord = db_.GetCoord(bus.stops.at(0));
            busnamepoint.push_back({ name,{coord,coord} });
        }
        else {
            auto coord1 = db_.GetCoord(bus.stops.at(0));
            auto coord2 = db_.GetCoord(bus.stops.at(bus.stops.size() / 2));
            busnamepoint.push_back({ name,{coord1,coord2} });
        }
    }
    return busnamepoint;
}