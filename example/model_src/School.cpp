

#include "School.h"

namespace chi_sim_model {

using namespace std;

const unsigned int MAX_SCHOOL_COMPONENT_SIZE = 30;

School::School(int int_id, int rank, const std::vector<std::string>& attributes) :
		CompositePlace(int_id, rank, attributes, MAX_SCHOOL_COMPONENT_SIZE), size(0)  {
}

void School::addPerson(const PersonPtr& person, int activity_type) {
	++size;
	CompositePlace::addPerson(person, activity_type);
}

void School::reset() {
	size = 0;
	CompositePlace::reset();
}

School::~School() {

}

} /* namespace mrsa */
