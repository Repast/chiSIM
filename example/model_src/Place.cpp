/*
 * Place.cpp
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */

#include <string>
#include "repast_hpc/RepastProcess.h"

#include "Place.h"
#include "typedefs.h"

const int PLACE_ID_IDX = 0;
const int PLACE_TYPE_IDX = 1;
const int ZIP_IDX = 6;

namespace chi_sim_model {

Place::Place(int int_id, int rank, const std::vector<std::string>& attributes) :
		orig_id(attributes[PLACE_ID_IDX]), type_(attributes[PLACE_TYPE_IDX]),
		zipcode(std::stoi(attributes[ZIP_IDX])), id_{int_id, rank}
{

}

Place::Place(int int_id, int rank, const std::string& id, const std::string& type, int zip) :
		orig_id(id), type_(type), zipcode(zip), id_{int_id, rank}
{}

Place::~Place() {
}

void Place::addPerson(const PersonPtr& person, int act_type) {
	int rank = repast::RepastProcess::instance()->rank();
	std::cout << "whoops!!! adding person to remote place: " << rank << "," << id_.rank << std::endl;
//	if (person->id() == "5450835") {
//		std::cout << "adding to non-local place " << type_ << ", rank " << id_.rank << std::endl;
//	}
}

} /* namespace chi_sim */
