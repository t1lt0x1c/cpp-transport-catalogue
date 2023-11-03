#pragma once
// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "transport_catalogue.h"
#include <iostream>
#include <iomanip>

namespace TrCatalogue {
    namespace OutPut {
        enum class OutRequestType {
            BUS,
            STOP,
        };

        class Stat_Reader {
        public:
            struct StatRequest {
                OutRequestType type;
                std::string data;
                bool operator==(StatRequest oth) {
                    return (oth.type == type && oth.data == data);
                }

            };
            StatRequest AddRequest(std::string text);
            void GetInfoBus(std::string text, TrCatalogue::DataBase::TransportCatalogue& catalogue, std::ostream& out = std::cout);
            void GetInfoStop(std::string text, TrCatalogue::DataBase::TransportCatalogue& catalogue, std::ostream& out = std::cout);
            void GetOutPut(TrCatalogue::DataBase::TransportCatalogue& catalogue, std::ostream& out = std::cout);

        private:
            std::vector<StatRequest> output;
        };
    }
}