/*
 * common .h
 *
 *  Created on: Jun 14, 2016
 *      Author: nick
 */

#ifndef COMMON__H_
#define COMMON__H_

#include <memory>
#include <vector>

#include "../src/chi_sim/PlaceId.h"
#include "../src/mrsa/Places.h"

#include "TestPlace.h"
#include "TestPerson.h"


using TestPlacePtr = std::shared_ptr<TestPlace>;
using PID = std::pair<chi_sim::PlaceId, std::shared_ptr<TestPlace>>;
using PersonPtr = std::shared_ptr<TestPerson>;

struct PC {
	PersonPtr createLocalPerson(unsigned long num_id, const mrsa::Places<TestPlace>& places, ActsPtr& weekday_acts,
			ActsPtr& weekend_acts, std::vector<std::string>& line);

	void processRemotePerson(unsigned int pid, int rank) {}
};

struct PLC {

	TestPlacePtr createLocalPlace(int id, int rank, const std::string& type, const std::vector<std::string>& line) const;

	TestPlacePtr createRemotePlace(int id, int rank, const std::vector<std::string>& line) const;
};




#endif /* COMMON__H_ */
