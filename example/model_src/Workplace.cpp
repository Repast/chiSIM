/*
 * Workplace.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: nick
 */

#include "Workplace.h"

namespace chi_sim_model {

Workplace::Workplace(int int_id, int rank, const std::vector<std::string>& attributes) :
		LocalPlace(int_id, rank, attributes){
}

Workplace::~Workplace() {
}

} /* namespace chi_sim */
