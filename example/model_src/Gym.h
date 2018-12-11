

#ifndef GYM_H_
#define GYM_H_

#include "CompositePlace.h"

namespace chi_sim_model {

class Gym : public CompositePlace {
public:
	Gym(int int_id, int rank, const std::vector<std::string>& attributes);
	virtual ~Gym();
};

}
#endif /* GYM_H_ */
