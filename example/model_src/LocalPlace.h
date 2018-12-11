/*
 * LocalPlace.h
 *
 *  Created on: Jan 12, 2015
 *      Author: nick
 */

#ifndef SRC_LOCALPLACE_H_
#define SRC_LOCALPLACE_H_

#include <vector>
#include <memory>

#include "Place.h"
#include "typedefs.h"

namespace chi_sim_model {

class LocalPlace: public Place {

protected:
	std::vector<std::shared_ptr<SamplePerson>> uninfected_persons;
	std::vector<std::shared_ptr<SamplePerson>> infected_persons;

public:
	LocalPlace(int int_id, int rank, const std::vector<std::string>& attributes);
	LocalPlace(int int_id, int rank, const std::string& orig_id, const std::string& type_, int zipcode);
	virtual ~LocalPlace();

	virtual void addPerson(const std::shared_ptr<SamplePerson>& person, int act_type) override;

	void reset() {
		uninfected_persons.clear();
		infected_persons.clear();
	}
};

} /* namespace chi_sim */

#endif /* SRC_LOCALPLACE_H_ */
