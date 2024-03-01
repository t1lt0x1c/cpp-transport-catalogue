#include "transport_router.h"

TransportRouter::TransportRouter(const TransportCatalogue& transport_catalogue, const Route_Settings& settings)
    :catalogue(transport_catalogue), settings_(settings)
{
    AddStops(catalogue.GetStops());
    BuildRoutesEdge(catalogue.GetBuses());

    router = std::make_unique<graph::Router<double>>(*routes);
}

void TransportRouter::AddStops(const std::deque<Stop>& stops) {
    graph::VertexId begin = 0;
    graph::VertexId end = 1;
    for (auto& stop : stops) {
        std::string_view stop_ = stop.name;
        stop_to_vertex[stop_] = { begin, end };
        begin += 2;
        end += 2;
    } 
}

void TransportRouter::AddBusRouteEdges(const Bus& bus) {
    const auto& distances = catalogue.AllRouteBus(bus.name, settings_.bus_velocity_);

    graph::VertexId from{ 0 };
    graph::VertexId to{ 0 };

    for (const auto& [route, info] : distances) {
        from = stop_to_vertex[route.first->name].end;
        to = stop_to_vertex[route.second->name].begin;

        auto edge = graph::Edge<double>{ from, to, info.time };

        routes->AddEdge(edge);
        edge_to_response.emplace(edge, BusResponse(info.time, bus.name, info.stops_count));
    }
}

void TransportRouter::BuildRoutesEdge(const std::deque<Bus>& buses) {
    routes = std::make_unique<graph::DirectedWeightedGraph<double>>(stop_to_vertex.size() * 2);

    double wait_time = static_cast<double>(settings_.bus_wait_time_);
    auto stop_edge = graph::Edge<double>{};

    for (auto [stop_name, stop_vertex] : stop_to_vertex) {
        stop_edge = graph::Edge<double>{ stop_vertex.begin, stop_vertex.end, wait_time };

        routes->AddEdge(stop_edge);
        edge_to_response.emplace(stop_edge, WaitResponse(wait_time, stop_name));
    }

    for (const auto& bus : buses) {
        AddBusRouteEdges(bus);
    }
}

ResponseDataOpt TransportRouter::BuildRoute(std::string_view from, std::string_view to) {
    ResponseDataOpt result;

    graph::VertexId id_from = stop_to_vertex.at(from).begin;
    graph::VertexId id_to = stop_to_vertex.at(to).begin;

    if (auto route = router->BuildRoute(id_from, id_to)) {
        result.emplace(ResponseData{});
        result->total_time = route->weight;

        for (auto edge_id : route->edges) {
            graph::Edge<double> edge = routes->GetEdge(edge_id);

            result->items.emplace_back(edge_to_response.at(edge));
        }
    }
    return result;
}