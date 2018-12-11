/*
 * Place.h
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */

#ifndef SRC_PLACE_H_
#define SRC_PLACE_H_

#include <vector>
#include <memory>

#include "chi_sim/PlaceId.h"


namespace chi_sim_model {

class SamplePerson;

class Place {

protected:

    using PersonPtr = std::shared_ptr<SamplePerson>;

	std::string orig_id, type_;
	int zipcode;
	chi_sim::PlaceId id_;

public:
	Place(int int_id, int rank, const std::vector<std::string>& attributes);
	Place(int int_id, int rank, const std::string& orig_id, const std::string& type_, int zipcode);
	virtual ~Place();

	/**
	 * Reset for next time step.
	 */
	virtual void reset() {}


	const chi_sim::PlaceId placeId() const {
		return id_;
	}

	virtual void addPerson(const PersonPtr& person, int act_type);
};


} /* namespace chi_sim */

#endif /* SRC_PLACE_H_ */
