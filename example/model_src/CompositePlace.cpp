
#include "CompositePlace.h"

namespace chi_sim_model {

using namespace std;

ComponentPlace::ComponentPlace(int int_id, int rank, const std::string& id, const std::string& type, int zip) :
		LocalPlace(int_id, rank, id, type, zip) {

}

ComponentPlace::~ComponentPlace() {
}

size_t ComponentPlace::size() {
	return uninfected_persons.size() + infected_persons.size();
}

CompositePlace::CompositePlace(int int_id, int rank, const std::vector<std::string>& attributes,
		unsigned int component_max_size) :
		Place(int_id, rank, attributes), components(), component_max_size_(component_max_size),comp_index(0) {

	// add an initial component.
	components.push_back(new ComponentPlace(int_id, rank, orig_id, type_, zipcode));
}

CompositePlace::~CompositePlace() {
	// delete the component places.
	for (vector<ComponentPlace*>::iterator iter = components.begin(); iter != components.end();
			++iter) {
		delete (*iter);
	}
}

void CompositePlace::addPerson(const PersonPtr& person, int act_type) {
	ComponentPlace* place = components[comp_index];
	place->addPerson(person, act_type);

	if (place->size() == component_max_size_) {
		++comp_index;
		if (comp_index == components.size())
			components.push_back(new ComponentPlace(id_.place_id, id_.rank, orig_id, type_, zipcode));
	}
}

void CompositePlace::reset() {
	for (vector<ComponentPlace*>::iterator iter = components.begin(); iter != components.end();
			++iter) {
		(*iter)->reset();
	}
	comp_index = 0;
}

/**
 * Gets the number of persons in the component place at the specified index.
 *
 * @return the number of persons in the component place at the specified index.
 */
size_t CompositePlace::componentSize(size_t index) {
	return components.at(index)->size();
}

} /* namespace mrsa */
