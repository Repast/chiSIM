

#ifndef HOSPTIAL_H_
#define HOSPITAL_H_

#include "LocalPlace.h"

namespace chi_sim_model {

class Hospital : public LocalPlace {

public:

	Hospital(int int_id, int rank, const std::vector<std::string>& attributes);
	virtual ~Hospital();

};

} /* namespace mrsa */
#endif /* HOSPITAL_H_ */
