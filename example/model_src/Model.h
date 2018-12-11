/*
 * Model.h
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */

#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#include <vector>
#include <map>
#include <memory>

#include "mpi.h"

#include "repast_hpc/Properties.h"

#include "../src/chi_sim/Calendar.h"
#include "../src/chi_sim/AbstractModel.h"
#include "../src/mrsa/Activities.h"
#include "../src/mrsa/Activity.h"

#include "SamplePerson.h"
#include "typedefs.h"
#include "Creator.h"

namespace chi_sim_model {


using AbsModelT = chi_sim::AbstractModel<SamplePerson, Place, SamplePersonData>;

struct Timings {
	double setup_time;
	time_t run_start, run_end;
	unsigned long long mem_use;
};


struct ModelPlaceCreator {

	PlacePtr createLocalPlace(int int_id, int rank, const std::string& type,
			const std::vector<std::string>& line) const;
	PlacePtr createRemotePlace(int int_id, int rank, const std::vector<std::string>& line) const;

};

class Model : public AbsModelT {

private:

	chi_sim::Calendar cal;
	Creator creator;
	std::map<int, ActivitiesPtr> acts_cache;

	void createPlaces(std::map<int, PlacePtr>& place_map);
	void createPersons(std::map<unsigned int, std::shared_ptr<SamplePerson>>& local_persons, std::map<int, PlacePtr>& place_map,
	                    std::map<int, ActivitiesPtr>& act_map);



protected:
	std::shared_ptr<SamplePerson> createPerson(unsigned int p_id, SamplePersonData& data) override;

	void updatePerson(std::shared_ptr<SamplePerson>& person, SamplePersonData& data) override;

	void initializePopulation();

public:
	Model(repast::Properties& props, MPI_Datatype mpi_person_type);
	virtual ~Model();

	void step();
	void atEnd();
};

} /* namespace chi_sim */

#endif /* SRC_MODEL_H_ */
