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

    size_t StopToStopHasher::operator()(const std::pair<const Stop*, const Stop*> stops) const {
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
    
    
    
    
    DistancesFromStopToStop TransportCatalogue::AllRouteBus(const string_view busname, double bus_velocity) const {
        DistancesFromStopToStop distances;
        
        auto gettime = [this, bus_velocity](Stop* from, Stop* to) {
            return static_cast<double>(DistanceStopToStop(from, to) / bus_velocity);
            };
        
        auto addinfo = [this, &gettime, &distances](auto begin, auto end) {
            double all_time{0};
            std::pair<Stop*, Stop*> a_to_b;
            InfoFromStopToStop current_info;
            auto prev = begin;
            
            for(auto it = begin; it != end; it++) {
                all_time = 0;
                prev = it;
                int count = 1;
                for (auto it2 = next(it); it2 != end; ++it2) {
                    
                    all_time += gettime(*prev, *it2);
                    a_to_b = std::pair{*it, *it2};
                    current_info = InfoFromStopToStop{all_time, count++};
                    
                    if (distances.count(a_to_b) == 0 || distances[a_to_b].time > all_time) {
                        distances[a_to_b] = current_info;
                    }
                    prev = it2;
                }
            }
        };
        
        const auto& bus = busname_to_bus.at(busname);
        const auto& stops = bus->stops;
        
        addinfo(stops.begin(), stops.end());
        
        if(!bus->is_round) {
            addinfo(stops.rbegin(), stops.rend());
        }
        return distances;
    }
}