/*
 * TestModel.h
 *
 *  Created on: Jun 14, 2016
 *      Author: nick
 */

#ifndef TESTMODEL_H_
#define TESTMODEL_H_

#include <map>

#include "chi_sim/AbstractModel.h"


#include "common.h"
#include "typedefs.h"

class TestModel: public AbsModelT {

private:
    std::map<int, ActsPtr> acts_cache;

public:
	TestModel(repast::Properties& props, MPI_Datatype mpi_person_type);

	virtual ~TestModel();

	virtual void createPlaces(std::map<int, TestPlacePtr>& place_map);

	virtual void createPersons(std::map<unsigned int, PersonPtr>& local_persons,
			std::map<int, TestPlacePtr>& place_map,
			std::map<int, ActsPtr>& act_map);

	void initializePopulation();

	/**
	 * Create a person from the specified parameters.
	 *
	 * @param index the index of the start of the block of data for this Person
	 */
	virtual PersonPtr createPerson(unsigned int p_id, TestPersonData& data) override;

	/**
	 * Update the state of the specified person with the data in data.
	 *
	 * @param index the index of the start of the block of data for this Person
	 */
	virtual void updatePerson(PersonPtr& person, TestPersonData& data) override;

	void step();

	std::map<unsigned int, PersonPtr>& localPersons() {
		return local_persons;
	}

	std::map<int, TestPlacePtr>& allPlaces() {
		return places;
	}

	void move(chi_sim::Calendar& cal);
};


#endif /* TESTMODEL_H_ */
