#pragma once

#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "domain.h"

namespace TrCatalogue {
    struct InfoFromStopToStop {
        double time{ 0 };
        int stops_count{ 0 };
    };
    using DistancesFromStopToStop = std::unordered_map<std::pair<Stop*, Stop*>, InfoFromStopToStop, StopToStopHasher>;
    class TransportCatalogue {
    public:
        void AddStop(const std::string& name, geo::Coordinates coord);
        Stop* FindStop(std::string_view stop);
        Stop* EditStop(std::string_view stop);
        void AddBus(const std::string& bus, const std::vector<std::string_view>& stops, bool is_round);
        void AddDistance(std::string_view, std::string_view, int);
        Bus* FindBus(const std::string& bus);
        int DistanceStopToStop(Stop* stop1, Stop* stop2) const;
        BusInfo GetInfoBus(const std::string_view& name) const;
        StopInfo GetInfoStop(const std::string_view& name) const;
        const std::deque<Bus>& GetBuses() const;
        const std::deque<Stop>& GetStops() const;
        geo::Coordinates GetCoord(Stop*) const;
        DistancesFromStopToStop AllRouteBus(const std::string_view busname, double bus_velocity) const;
    private:
        std::deque<Stop> stops;
        std::deque<Bus> buses;
        std::unordered_map<std::string_view, Stop*, StopsHasher> stopname_to_stop;
        std::unordered_map<std::string_view, Bus*, BusHasher> busname_to_bus;
        std::unordered_map<std::pair<Stop*, Stop*>, int, StopToStopHasher> length_table;
    };
    
    
}