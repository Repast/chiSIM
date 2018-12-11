

#include "Household.h"

namespace chi_sim_model {

Household::Household(int int_id, int rank, const std::vector<std::string>& attributes) :
		LocalPlace(int_id, rank, attributes) {
}

Household::~Household() {
}


} /* namespace mrsa */
