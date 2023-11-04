
// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "transport_catalogue.h"
#include <iostream>

using namespace std;
using namespace TrCatalogue::DataBase;

size_t StopsHasher::operator()(const string_view& stop) const {
    return hasher_(stop);
}

size_t BusHasher::operator()(const string_view& bus) const {
    return hasher_(bus);
}

size_t StopToStopHasher::operator()(const pair<const Stop*, const Stop*> stops) const {
    return hasher_(stops.first) + hasher_(stops.second) * 37;
}

void TransportCatalogue::AddDistance(string_view stop1, std::string_view stop2, int distance) {
    auto stop1_ = FindStop(stop1);
    auto stop2_ = FindStop(stop2);
    length_table[{stop1_, stop2_}] = distance;
}

void TransportCatalogue::AddStop(const std::string name, Coordinates coord) {
    stops.push_back({ name, {coord.lat, coord.lng}, {} });
    stopname_to_stop[stops.back().name] = &(stops.back());
}

Stop* TransportCatalogue::FindStop(std::string_view stop) {
    return stopname_to_stop.at(stop);
}

Stop* TransportCatalogue::EditStop(std::string_view stop) {
    return stopname_to_stop[stop];
}

void TransportCatalogue::AddBus(const string& bus, const vector<string>& stops_) {
    buses.push_back({ bus, {} });
    for (const auto& stop : stops_) {
        buses.back().stops.push_back(FindStop(stop));
        EditStop(stop)->buses.insert(buses.back().name);
    }
    busname_to_bus[buses.back().name] = &(buses.back());
}

Bus* TransportCatalogue::FindBus(string& bus) {
    return busname_to_bus.at(bus);
}

int TransportCatalogue::DistanceStopToStop(Stop* stop1, Stop* stop2) {
    if (length_table.count({ stop1, stop2 })) {
        int k =  length_table[{ stop1, stop2 }];
        return k;
    }
    else {
        int k = length_table[{ stop2, stop1 }];
        return k;
    }
}

BusInfo TransportCatalogue::GetInfoBus(const string name) {
    if (busname_to_bus.count(name)) {
        const Bus& bus = *(busname_to_bus.at(name));
        double geo_distance = 0;
        double factical_distance = 0;
        const auto& st = (bus.stops);
        unordered_set<string> stops_uniq;
        for (int i = 0; i < st.size() - 1; i++) {
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

StopInfo TransportCatalogue::GetInfoStop(const string name) {
    if (stopname_to_stop.count(name)) {
        const Stop& stop = *(stopname_to_stop.at(name));
        return { stop.name, stop.buses };
    }
    else {
        return {};
    }
}