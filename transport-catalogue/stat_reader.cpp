#include "stat_reader.h"


using namespace std;
using namespace TrCatalogue::OutPut;

StatReader::StatRequest StatReader::AddRequest(const string text, std::ostream& out) {
    auto it = text.find(' ');
    auto it_ = text.find_first_not_of(' ', it);
    string t = text.substr(it_);
    if (text[0] == 'B') {
        output.push_back({ OutRequestType::BUS, t });
        return { OutRequestType::BUS, t };
    }
    else if (text[0] == 'S') {
        output.push_back({ OutRequestType::STOP, t });
        return { OutRequestType::STOP, t };
    }
    else {
        out << "Unknown Request" << endl;
        return {};
    }
}

void StatReader::GetInfoBus(std::string text, TrCatalogue::TransportCatalogue& catalogue, ostream& out) {
    auto res = catalogue.GetInfoBus(text);
    if (res.stops) {
        out << "Bus " << res.name << ": " << res.stops << " stops on route, " << res.uniq_stops << " unique stops, " << setprecision(6) <<  res.distance << " route length, " << res.curvature << " curvature" << endl;
    }
    else {
        out << "Bus " << text << ": not found" << endl;
    }
}

void StatReader::GetInfoStop(std::string stop, TrCatalogue::TransportCatalogue& catalogue, ostream& out) {
    auto res = catalogue.GetInfoStop(stop);
    if (res.name != "") {
        if (res.buses.empty()) {
            out << "Stop " << stop << ": no buses " << endl;
        }
        else {
            out << "Stop " << stop << ": buses";
            for (auto& bus : res.buses) {
                out << ' ' << bus;
            }
            out << endl;
        }
    }
    else {
        out << "Stop " << stop << ": not found" << endl;
    }
}

void StatReader::GetOutPut(TrCatalogue::TransportCatalogue& catalogue, ostream& out) {
    for (auto request : output) {
        if (request.type == OutRequestType::BUS) {
            GetInfoBus(request.data, catalogue, out);
        }
        else {
            GetInfoStop(request.data, catalogue, out);
        }
    }
}