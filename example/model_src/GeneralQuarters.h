
#ifndef GENERALQUARTERS_H_
#define GENERALQUARTERS_H_

#include "CompositePlace.h"

namespace chi_sim_model {

class GeneralQuarters: public CompositePlace {

public:
	GeneralQuarters(int int_id, int rank, const std::vector<std::string>& attributes);

	virtual ~GeneralQuarters();

};

}
#endif
