

#include "Hospital.h"

namespace chi_sim_model {

Hospital::Hospital(int int_id, int rank, const std::vector<std::string>& attributes) :
		LocalPlace(int_id, rank, attributes) {
}

Hospital::~Hospital() {
}


} /* namespace mrsa */
