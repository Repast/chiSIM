
#ifndef JAIL_H_
#define JAIL_H_

#include "CompositePlace.h"

namespace chi_sim_model {

class Jail: public CompositePlace {

public:
	Jail(int int_id, int rank, const std::vector<std::string>& attributes);
	virtual ~Jail();

};

}
#endif /* JAIL_H_ */
