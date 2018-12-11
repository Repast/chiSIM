
#ifndef COMPOSITEPLACE_H_
#define COMPOSITEPLACE_H_

#include "Place.h"
#include "LocalPlace.h"
#include "typedefs.h"

namespace chi_sim_model {

/**
 * A place within a larger place, e.g. a grade or group within a school.
 */
class ComponentPlace: public LocalPlace {

public:
	ComponentPlace(int int_id, int rank, const std::string& id, const std::string& type, int zip);
	virtual ~ComponentPlace();

	/**
	 * Gets the number of persons in this ComponentPlace.
	 */
	size_t size();

private:
	ComponentPlace(const ComponentPlace& other);
};

/**
 * A CompositePlace contains sub component places, e.g. a school with grades.
 */
class CompositePlace: public Place {

public:
	CompositePlace(int int_id, int rank, const std::vector<std::string>& attributes, unsigned int component_max_size);
	virtual ~CompositePlace();

	void addPerson(const PersonPtr& person, int act_type) override;

	/**
	 * Resets this CompositePlace for the next iteration of the model.
	 */
	void reset();

	/**
	 * Gets the number of component places in this CompositePlace.
	 */
	size_t componentCount() {
		return components.size();
	}

	/**
	 * Gets the number of persons in the component place at the specified index.
	 *
	 * @return the number of persons in the component place at the specified index.
	 */
	size_t componentSize(size_t index);

protected:
	std::vector<ComponentPlace*> components;

private:
	unsigned int component_max_size_;
	size_t comp_index;
};

} /* namespace mrsa */
#endif /* COMPOSITEPLACE_H_ */
