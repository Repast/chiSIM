/*
 * Workplace.h
 *
 *  Created on: Aug 12, 2015
 *      Author: nick
 */

#ifndef SRC_WORKPLACE_H_
#define SRC_WORKPLACE_H_

#include "LocalPlace.h"

namespace chi_sim_model {

// TODO: In MRSA model Workplace extends SelectiveTransmissionPlace where only N
// number of persons in the place are selected for possible transmission
class Workplace : public LocalPlace {
public:
	Workplace(int int_id, int rank, const std::vector<std::string>& attributes);
	virtual ~Workplace();
};

} /* namespace chi_sim */

#endif /* SRC_WORKPLACE_H_ */
