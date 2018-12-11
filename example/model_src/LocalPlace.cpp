/*
 * LocalPlace.cpp
 *
 *  Created on: Jan 12, 2015
 *      Author: nick
 */

#include "LocalPlace.h"

namespace chi_sim_model {

LocalPlace::LocalPlace(int int_id, int rank, const std::vector<std::string>& attributes) :
		Place(int_id, rank, attributes), uninfected_persons(), infected_persons() {

}

LocalPlace::LocalPlace(int int_id, int rank, const std::string& id, const std::string& type, int zip) :
		Place(int_id, rank, id, type, zip)
{}

LocalPlace::~LocalPlace() {
}

void LocalPlace::addPerson(const PersonPtr& person, int act_type) {
	//if (person->isInfected()) {
	//	infected_persons.push_back(person);
	//} else {
	//	uninfected_persons.push_back(person);
	//}
}

} /* namespace chi_sim */
