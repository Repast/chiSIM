/*
 * common.cpp
 *
 *  Created on: Jun 14, 2016
 *      Author: nick
 */

#include "common.h"

using namespace std;
using namespace chi_sim;

PersonPtr PC::createLocalPerson(unsigned long num_id, const mrsa::Places<TestPlace>& places, ActsPtr& weekday_acts,
		ActsPtr& weekend_acts, std::vector<std::string>& line) {
	return make_shared<TestPerson>(num_id, places, weekday_acts, weekend_acts, line);
}

TestPlacePtr PLC::createLocalPlace(int id, int rank, const std::string& type,
		const std::vector<std::string>& line) const {
	return make_shared<TestPlace>(id, rank, type, line, true);
}

TestPlacePtr PLC::createRemotePlace(int id, int rank, const std::vector<std::string>& line) const {
	return make_shared<TestPlace>(id, rank, "", line, false);
}

