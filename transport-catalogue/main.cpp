// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;
using namespace TrCatalogue::Input;
using namespace TrCatalogue::DataBase;
using namespace TrCatalogue::OutPut;

int main() {
    InputReader inp;
    TransportCatalogue tr;
    StatReader out;
    string line;
    std::ostream error_requests;
    getline(cin, line);
    int n = stoi(line);
    for (int i = 0; i < n; i++) {
        getline(cin, line);
        inp.AddRequest(line, std::ostream& error_requests);
    }
    inp.SendRequestsStops(tr);
    inp.SendRequestsBuses(tr);
    inp.SendDistances(tr);
    getline(cin, line);
    n = stoi(line);
    for (int i = 0; i < n; i++) {
        getline(cin, line);
        out.AddRequest(line, std::ostream& error_requests);
    }
    out.GetOutPut(tr);
}