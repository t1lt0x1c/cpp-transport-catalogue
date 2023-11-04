// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"

using namespace std;
using namespace TrCatalogue::Input;

pair<string_view, int> MToStop(string_view text) {
    auto it_begin_num = text.find_first_not_of(' ');
    auto it_end_num = text.find('m');
    double distance = stoi(string(text.substr(it_begin_num, it_end_num - it_begin_num)));
    auto it_begin_stop = text.find("o", ++it_end_num);
    ++it_begin_stop;
    it_begin_stop = text.find_first_not_of(' ', it_begin_stop);
    auto it_end_stop = text.find_last_not_of(' ');
    string_view stop = text.substr(it_begin_stop, it_end_stop - it_begin_stop + 1);
    return { stop, distance };
}

void InputReader::AddRequest(const string& text, std::ostream& out) {
    auto it = text.find(' ');
    auto it_ = text.find_first_not_of(' ', it);
    string request = text.substr(it_);
    if (text[0] == 'B') {
        buses_.push_back({ RequestType::BUS, request });
    }
    else if (text[0] == 'S') {
        stops_.push_back({ RequestType::STOP, request });
    }
    else {
        out << "Unknown Request" << endl;
    }
}

void InputReader::SendRequestsStops(TrCatalogue::DataBase::TransportCatalogue& catalogue) {
    for (auto& request : stops_) {
        string_view text = request.data;
        auto it_endname = text.find(':');
        string_view name = text.substr(0, it_endname);
        auto it_beginlat = text.find_first_not_of(' ', ++it_endname);
        auto it_endlat = text.find(',', it_beginlat);
        double lat = stod(string(text.substr(it_beginlat, it_endlat - it_beginlat)));
        auto it_beginlong = text.find_first_not_of(' ', ++it_endlat);
        if (text.find(',', it_beginlong) != string::npos) {
            auto it_endlong = text.find(',', it_beginlong);
            double lng = stod(string(text.substr(it_beginlong, it_endlong - it_beginlong + 1)));
            catalogue.AddStop(string(name), { lat, lng });
            auto& it_beginstop = it_endlong;
            while (text.find(',', it_beginstop) != string::npos) {
                ++it_beginstop;
                auto it_endstop = text.find(',', it_beginstop);
                auto t = MToStop(text.substr(it_beginstop, it_endstop - it_beginstop));
                distances.push_back({ {name, t.first}, t.second });
                it_beginstop = text.find(',', it_endstop);
            } 
        } else {
            auto it_endlong = text.find_last_not_of(' ');
            double lng = stod(string(text.substr(it_beginlong, it_endlong - it_beginlong + 1)));
            catalogue.AddStop(string(name), { lat, lng });
        }
    }
}

void InputReader::SendRequestsBuses(TrCatalogue::DataBase::TransportCatalogue& catalogue) {
    for (auto request : buses_) {
        vector<string> bus;
        string text = request.data;
        auto it_endname = text.find(':');
        string name = text.substr(0, it_endname);
        auto it_ = ++it_endname;
        if (text.find('>') != string::npos) {
            do {
                auto it_beginstop = text.find_first_not_of(' ', it_);
                auto it_endstop = text.find('>', it_);
                it_ = ++it_endstop;
                it_endstop = text.find_last_not_of(" >", it_endstop);
                string stop = text.substr(it_beginstop, it_endstop - it_beginstop + 1);
                bus.push_back(stop);
            } while (it_);
        }
        else {
            do {
                auto it_beginstop = text.find_first_not_of(' ', it_);
                auto it_endstop = text.find('-', it_);
                it_ = ++it_endstop;
                it_endstop = text.find_last_not_of(" -", it_endstop);
                string stop = text.substr(it_beginstop, it_endstop - it_beginstop + 1);
                bus.push_back(stop);
            } while (it_);
            int n = bus.size();
            for (int i = n - 2; i >= 0; i--) {
                bus.push_back(bus[i]);
            }
        }
        catalogue.AddBus(name, bus);
    }
}

void InputReader::SendDistances(TrCatalogue::DataBase::TransportCatalogue& catalogue) {
    for (auto& d : distances) {
        catalogue.AddDistance(d.first.first, d.first.second, d.second);
    }
}