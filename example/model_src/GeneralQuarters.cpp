
#include "GeneralQuarters.h"

namespace chi_sim_model {

const unsigned int MAX_GQ_COMPONENT_SIZE = 30;

GeneralQuarters::GeneralQuarters(int int_id, int rank, const std::vector<std::string>& attributes) :
				CompositePlace(int_id, rank, attributes, MAX_GQ_COMPONENT_SIZE) {
}

GeneralQuarters::~GeneralQuarters() {
}


}
