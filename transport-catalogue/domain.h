#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <optional>
#include "geo.h"


/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

namespace TrCatalogue {
    struct Stop {
        std::string name;
        geo::Coordinates coord;
        std::set<std::string_view> buses;
        bool operator<(const Stop& other) const {
            return this->name < other.name;
        }
    };

    struct Bus {
        std::string name;
        std::vector<Stop*> stops;
        bool is_round = false;
        bool operator<(const Bus& other) const {
            return this->name < other.name;
        }
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
}

struct BusAdd {
    std::string name;
    std::vector<std::string_view> stops;
    bool is_round = false;
};

struct DistancesAdd {
    std::string stop_from;
    std::string stop_to;
    int distance;
};