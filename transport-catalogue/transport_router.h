#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include <variant>
#include <memory>

using namespace TrCatalogue;

struct Route_Settings {
    int bus_wait_time_{ 0 };
    double bus_velocity_{ 0 };
};

struct StopVertex {
    graph::VertexId begin;
    graph::VertexId end;
};

struct WaitResponse {
    double time{0.0};
    std::string type{"Wait"};

    std::string stop_name;

    WaitResponse(double time, std::string_view stop) : time(time), stop_name(stop) {}
};

struct BusResponse {
    double time{0.0};
    std::string type{"Bus"};

    std::string bus;
    int span_count{0};

    BusResponse(double time, const std::string& bus, int stops_count) : time(time), bus(bus), span_count(stops_count) {}
};

using Response = std::variant<WaitResponse, BusResponse>;

struct ResponseData {
    double total_time{ 0. };
    std::vector<Response> items;
};

using ResponseDataOpt = std::optional<ResponseData>;

class TransportRouter {
public:
    TransportRouter(const TransportCatalogue& transport_catalogue, const Route_Settings& settings);
    [[nodiscard]] ResponseDataOpt BuildRoute(std::string_view from, std::string_view to);
private:     
    void AddStops(const std::deque<Stop>& stops);
    void AddBusRouteEdges(const Bus& bus);
    void BuildRoutesEdge(const std::deque<Bus>& buses);
    const TrCatalogue::TransportCatalogue& catalogue;
    Route_Settings settings_;
   
    struct EdgeHash {
        inline size_t operator()(const graph::Edge<double>& edge) const {
            return odd * std::hash<size_t>{}(edge.from) +
                odd * odd * std::hash<size_t>{}(edge.to) +
                odd * odd * odd * std::hash<double>{}(edge.weight);
        }


    private:
        static constexpr size_t odd{ 41 };
    };

    std::unordered_map<std::string_view, StopVertex> stop_to_vertex;
    std::unordered_map <graph::Edge<double>, Response, EdgeHash> edge_to_response;
    
    std::unique_ptr<graph::DirectedWeightedGraph<double>> routes{ nullptr };
    std::unique_ptr<graph::Router<double>> router{ nullptr };

};



