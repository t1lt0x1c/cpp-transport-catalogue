#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"
#include <sstream>

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

class JsonReader {
public:
	JsonReader(std::istream& input, TrCatalogue::TransportCatalogue& tr) :doc_(json::Load(input)), tr_(tr) {
		FillTrCatalogue();
		SetingsRender();
	}

	void FillTrCatalogue();
	void SetingsRender();
	void AddBuses(std::vector<BusAdd>& buses);
	void AddDistances(std::vector <DistancesAdd>& distances);
	void OutPutAnswers(std::ostream& out = std::cout) const;

private:
	const json::Document doc_;
	TrCatalogue::TransportCatalogue& tr_;
	Settings settings;
};