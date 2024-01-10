#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include <iostream>
#include <sstream>
#include <fstream>



int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массиве "stat_requests", построив JSON-массив
     * с ответами.
     * Вывести в stdout ответы в виде JSON
     */
    TransportCatalogue trt;
    RequestHandler rq(trt);
    JsonReader jst(std::cin, trt);
    jst.OutPutAnswers(std::cout);
}