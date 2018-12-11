/*
 * TestPlace.cpp
 *
 *  Created on: Jun 9, 2016
 *      Author: nick
 */
#include <string>

#include "TestPlace.h"
#include "TestPerson.h"

/*double lon, lat;
	int zip, zip_grp;*/

using namespace std;

TestPlace::TestPlace(int id, int rank, const std::string& type, const std::vector<std::string>& attributes, bool local) : id_(id),
	rank_{rank}, type_{type}, lon(stod(attributes[6])), lat(stod(attributes[5])), zip(stoi(attributes[7])),
	zip_grp(stoi(attributes[8])), is_local(local), place_id_{id, rank}, person_map{} {
}

TestPlace::~TestPlace() {
}

void TestPlace::addPerson(std::shared_ptr<TestPerson> person, int type) {
	person_map.emplace(person->id(), person);
}

