#pragma once
// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "geo.h"

#include <string>
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace TrCatalogue {
    namespace DataBase {
        struct Stop {
            std::string name;
            Coordinates coord;
            std::set<std::string_view> buses;
        };

        struct Bus {
            std::string name;
            std::vector<Stop*> stops;
        };

        struct BusInfo {
            std::string name;
            int stops;
            int uniq_stops;
            double distance;
            double curvature;
        };

        struct StopInfo {
            std::string name;
            std::set<std::string_view> buses;
        };

        class StopsHasher {
        public:
            size_t operator()(const std::string_view& stop) const;
        private:
            std::hash<std::string_view> hasher_;
        };

        class BusHasher {
        public:
            size_t operator()(const std::string_view& bus) const;
        private:
            std::hash<std::string_view> hasher_;
        };

        class StopToStopHasher {
        public:
            size_t operator()(const std::pair<const Stop*, const Stop*> stops) const;
        private:
            std::hash<const Stop*> hasher_;
        };

        class TransportCatalogue {
        public:
            void AddStop(const std::string& name, Coordinates coord);
            Stop* FindStop(std::string_view stop);
            Stop* EditStop(std::string_view stop);
            void AddBus(const std::string& bus, const std::vector<std::string>& stops);
            void AddDistance(std::string_view, std::string_view, int);
            Bus* FindBus(const std::string& bus);
            int DistanceStopToStop(Stop* stop1, Stop* stop2);
            BusInfo GetInfoBus(const std::string& name);
            StopInfo GetInfoStop(const std::string& name);
        private:
            std::deque<Stop> stops;
            std::deque<Bus> buses;
            std::unordered_map<std::string_view, Stop*, StopsHasher> stopname_to_stop;
            std::unordered_map<std::string_view, Bus*, BusHasher> busname_to_bus;
            std::unordered_map<std::pair<Stop*, Stop*>, int, StopToStopHasher> length_table;
        };
    }
}