/*
 * Creator.cpp
 *
 *  Created on: Feb 8, 2016
 *      Author: nick
 */
#include "Creator.h"

using namespace chi_sim;

namespace chi_sim_model {

Creator::Creator() {
	// TODO Auto-generated constructor stub

}

Creator::~Creator() {
	// TODO Auto-generated destructor stub
}

std::shared_ptr<SamplePerson> Creator::createLocalPerson(unsigned int num_id,
		const PlacesType& places, ActivitiesPtr& weekday_acts, ActivitiesPtr& weekend_acts, std::vector<std::string>& line) {
	return std::make_shared<SamplePerson>(num_id, places, weekday_acts, weekend_acts);
}

std::shared_ptr<SamplePerson> Creator::createPerson(unsigned int p_id, PlacesType& places, ActivitiesPtr& weekday, ActivitiesPtr& weekend,
		int index, int* data) {
	return std::make_shared<SamplePerson>(p_id, places, weekday, weekend);

}

void Creator::updatePerson(std::shared_ptr<SamplePerson>& person, int index, int* data) {
	// no op as this example model uses Person without adding anything to it,
	// so there is additional data to update the person with
}

} /* namespace chi_sim_model */
