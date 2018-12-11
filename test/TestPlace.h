/*
 * TestPlace.h
 *
 *  Created on: Jun 9, 2016
 *      Author: nick
 */

#ifndef TESTPLACE_H_
#define TESTPLACE_H_

#include <vector>
#include <map>

#include "chi_sim/PlaceId.h"

class TestPerson;

class TestPlace {

public:
	int id_, rank_;
	std::string type_;
	double lon, lat;
	int zip, zip_grp;
	bool is_local;
	chi_sim::PlaceId place_id_;

	std::map<unsigned int, std::shared_ptr<TestPerson>> person_map;

	TestPlace(int id, int rank, const std::string& type, const std::vector<std::string>& attributes, bool local);
	virtual ~TestPlace();

	const chi_sim::PlaceId placeId() const {
		return place_id_;
	}

	void addPerson(std::shared_ptr<TestPerson> person, int type);


};

#endif /* TESTPLACE_H_ */
