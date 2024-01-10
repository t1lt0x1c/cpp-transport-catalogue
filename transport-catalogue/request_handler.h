#pragma once

#include "transport_catalogue.h"
#include <optional>

using namespace TrCatalogue;

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

 // Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
 // с другими подсистемами приложения.
 // См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)

class RequestHandler {
public:

    // MapRenderer понадобится в следующей части итогового проекта
    RequestHandler(const TransportCatalogue& db/*, const renderer::MapRenderer& renderer*/) :db_(db) {}

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusInfo> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через
    std::optional<StopInfo> GetBusesByStop(const std::string_view& stop_name) const;



    std::vector<geo::Coordinates> GetCordsStops();

    std::vector<std::vector<geo::Coordinates>> GetRoutes();

    using BusInf = std::vector<std::pair<const std::string, std::pair<geo::Coordinates, geo::Coordinates>>>;
    BusInf GetBusNamePoint();

    std::set<Bus> GetBuses();
    std::set<Stop> GetStops();

    // Этот метод будет нужен в следующей части итогового проекта
    //svg::Document RenderMap() const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& db_;

    const std::deque<Bus>& GetAllBuses() const {
        return db_.GetBuses();
    }

    const std::deque<Stop>& GetAllStops() const {
        return db_.GetStops();
    }
    //const renderer::MapRenderer& renderer_;
};
