/*
 * Creator.h
 *
 *  Created on: Feb 8, 2016
 *      Author: nick
 */

#ifndef MODEL_SRC_CREATOR_H_
#define MODEL_SRC_CREATOR_H_

#include <memory>

#include "SamplePerson.h"
#include "typedefs.h"

namespace chi_sim_model {

class Creator {



public:

	Creator();
	virtual ~Creator();

	std::shared_ptr<SamplePerson> createLocalPerson(unsigned int num_id, const PlacesType& places, ActivitiesPtr& weekday_acts,
			ActivitiesPtr& weekend_acts, std::vector<std::string>& line);

	 void processRemotePerson(unsigned int person_id, int person_rank) {}

	 std::shared_ptr<SamplePerson> createPerson(unsigned int p_id, PlacesType& places, ActivitiesPtr& weekday, ActivitiesPtr& weekend,
				int index, int* data);

	void updatePerson(std::shared_ptr<SamplePerson>& person, int index, int* data);

};
} /* namespace chi_sim_model */

#endif /* MODEL_SRC_CREATOR_H_ */
