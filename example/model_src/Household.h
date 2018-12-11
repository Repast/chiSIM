

#ifndef HOUSEHOLD_H_
#define HOUSEHOLD_H_

#include "LocalPlace.h"

namespace chi_sim_model {

class Household : public LocalPlace {

public:

	Household(int int_id, int rank, const std::vector<std::string>& attributes);
	virtual ~Household();

};

} /* namespace mrsa */
#endif /* HOUSEHOLD_H_ */
