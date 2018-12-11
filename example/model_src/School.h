

#ifndef SCHOOL_H_
#define SCHOOL_H_

#include "CompositePlace.h"

namespace chi_sim_model {

/**
 * A School place. Contact in a School is via
 * a component place within the school.
 */
class School: public CompositePlace {
public:
	School(int int_id, int rank, const std::vector<std::string>& attributes);
	virtual ~School();

	void addPerson(const PersonPtr& person, int activity_type);
	void reset();


private:
	unsigned int size;
};




} /* namespace mrsa */
#endif /* SCHOOL_H_ */
