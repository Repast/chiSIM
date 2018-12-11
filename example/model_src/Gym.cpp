#include "Gym.h"

namespace chi_sim_model {

const unsigned int MAX_GYM_COMPONENT_SIZE = 30;

Gym::Gym(int int_id, int rank, const std::vector<std::string>& attributes) :
		CompositePlace(int_id, rank, attributes, MAX_GYM_COMPONENT_SIZE) {
}

Gym::~Gym() {}



}
