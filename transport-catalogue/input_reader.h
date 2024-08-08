#pragma once
// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include <iostream>
#include <string>
#include <string_view>

#include "transport_catalogue.h"


enum class RequestType {
    BUS,
    STOP,
};
namespace TrCatalogue {
    namespace Input {
        class InputReader {
        public:
            struct Request {
                RequestType type;
                std::string data;
                bool operator==(Request oth) {
                    return (oth.type == type && oth.data == data);
                }
            };
            void AddRequest(const std::string& text, std::ostream&);
            void SendRequestsBuses(TrCatalogue::TransportCatalogue& catalogue);
            void SendRequestsStops(TrCatalogue::TransportCatalogue& catalogue);
            void SendDistances(TrCatalogue::TransportCatalogue& catalogue);
        private:
            std::vector<Request> buses_;
            std::vector<Request> stops_;
            std::vector<std::pair<std::pair<std::string_view, std::string_view>, int>> distances;
        };
    }
}