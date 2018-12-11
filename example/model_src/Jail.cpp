
#include "Jail.h"

namespace chi_sim_model {

const int MAX_JAIL_COMPONENT_SIZE = 30;

Jail::Jail(int int_id, int rank, const std::vector<std::string>& attributes) :
		CompositePlace(int_id, rank, attributes, MAX_JAIL_COMPONENT_SIZE) {
}

Jail::~Jail() {
}


}
