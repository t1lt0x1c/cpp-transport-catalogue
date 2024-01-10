#include "transport_catalogue.h"
#include <iostream>

using namespace std;
namespace TrCatalogue {

    size_t StopsHasher::operator()(const string_view& stop) const {
        return hasher_(stop);
    }

    size_t BusHasher::operator()(const string_view& bus) const {
        return hasher_(bus);
    }

    size_t StopToStopHasher::operator()(const pair<const Stop*, const Stop*> stops) const {
        return hasher_(stops.first) + hasher_(stops.second) * 37;
    }

    void TransportCatalogue::AddDistance(std::string_view stop1, std::string_view stop2, int distance) {
        auto stop1_ = FindStop(stop1);
        auto stop2_ = FindStop(stop2);
        length_table[{stop1_, stop2_}] = distance;
    }

    void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates coord) {
        stops.push_back({ name, {coord.lat, coord.lng}, {} });
        stopname_to_stop[stops.back().name] = &(stops.back());
    }

    Stop* TransportCatalogue::FindStop(std::string_view stop) {
        return stopname_to_stop.at(stop);
    }

    Stop* TransportCatalogue::EditStop(std::string_view stop) {
        return stopname_to_stop[stop];
    }

    void TransportCatalogue::AddBus(const string& bus, const vector<string_view>& stops_, bool is_round) {
        buses.push_back({ bus, {}, is_round });
        for (const auto& stop : stops_) {
            buses.back().stops.push_back(FindStop(stop));
            EditStop(stop)->buses.insert(buses.back().name);
        }
        busname_to_bus[buses.back().name] = &(buses.back());
    }

    Bus* TransportCatalogue::FindBus(const string& bus) {
        return busname_to_bus.at(bus);
    }

    int TransportCatalogue::DistanceStopToStop(Stop* stop1, Stop* stop2) const {
        if (length_table.count({ stop1, stop2 })) {
            int k = length_table.at({ stop1, stop2 });
            return k;
        }
        else {
            int k = length_table.at({ stop2, stop1 });
            return k;
        }
    }

    BusInfo TransportCatalogue::GetInfoBus(const std::string_view& name) const {
        if (busname_to_bus.count(name)) {
            const Bus& bus = *(busname_to_bus.at(name));
            double geo_distance = 0;
            double factical_distance = 0;
            const auto& st = (bus.stops);
            unordered_set<string> stops_uniq;
            for (size_t i = 0; i < st.size() - 1; i++) {
                geo_distance += ComputeDistance((*st[i]).coord, (*st[i + 1]).coord);
                factical_distance += DistanceStopToStop(st[i], st[i + 1]);
                stops_uniq.insert((*st[i + 1]).name);
            }
            int stops_ = bus.stops.size();
            int uniq_stops_ = stops_uniq.size();
            return { bus.name, stops_, uniq_stops_, factical_distance, factical_distance / geo_distance };
        }
        else {
            return {};
        }
    }

    StopInfo TransportCatalogue::GetInfoStop(const std::string_view& name) const {
        if (stopname_to_stop.count(name)) {
            const Stop& stop = *(stopname_to_stop.at(name));
            return { stop.name, stop.buses };
        }
        else {
            return {};
        }
    }
    const std::deque<Bus>& TransportCatalogue::GetBuses() const {
        return buses;
    }
    const std::deque<Stop>& TransportCatalogue::GetStops() const {
        return stops;
    }

    geo::Coordinates TransportCatalogue::GetCoord(Stop* stop) const {
        return (*stop).coord;
    }
}