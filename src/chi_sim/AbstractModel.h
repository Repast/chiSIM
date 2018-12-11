/*
 * AbstractModel.h
 *
 *  Created on: Jan 26, 2016
 *      Author: nick
 */

#ifndef SRC_ABSTRACTMODEL_H_
#define SRC_ABSTRACTMODEL_H_

#include <vector>
#include <map>

#include "repast_hpc/Properties.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Random.h"

#include "Calendar.h"
#include "Parameters.h"
#include "constants.h"

#include "move_persons.h"
#include "NextPlace.h"
#include "PlaceId.h"

//#include "gdb_debug.h"

namespace chi_sim {

/**
 * PersonType is person type
 * PlaceType is place type
 * PersonDataType is the type of data to send when moving persons between processes
 *
 */
template<typename PersonType, typename PlaceType, typename PersonDataType>
class AbstractModel {

private:
	MPI_Datatype data_type;
	std::shared_ptr<PlaceType> NULL_PTR;

protected:
	std::map<unsigned int, std::shared_ptr<PersonType>> local_persons;
	std::map<unsigned int, std::shared_ptr<PersonType>> remote_persons;
	// contains all the places either as full LocalPlace or
	// just a PlaceT
	std::map<int, std::shared_ptr<PlaceType>> places;
	// All the local places
	std::vector<std::shared_ptr<PlaceType>> local_places;

	int rank_;


	/**
	 * Given an place id and rank, update the specified pair
	 * from the places cache.
	 */
	void lookupPlace(int id, int rank, PlaceIdPair<PlaceType>& pair);

	/**
	 * Create a person from the specified parameters. Called when a remote person
	 * is passed to this rank and a person needs to be created.
	 *
	 *
	 */

	virtual std::shared_ptr<PersonType> createPerson(unsigned int p_id, PersonDataType& data) = 0;

	/**
	 * Update the state of the specified person with the data in data. Called
	 * to update a person cached on this rank when it moves to this rank.
	 *
	 *
	 */

	virtual void updatePerson(std::shared_ptr<PersonType>& person, PersonDataType& data) = 0;

	/**
	 * Integrates the Persons moved to this rank into the model. Each
	 * person is represented by a stride of elements in the data parameter.
	 *
	 * @param total_amt_received the array length of the data argument
	 * @param data the constituent data for the moved persons.
	 */

	virtual void integrateMovedPersons(int total_amt_received, PersonDataType* data);

	/**
	 * Moves all the local persons to their next places, either by adding them
	 * to the place if it is local to this rank or by moving the them to the
	 * appropriate process
	 */

	virtual void movePersons(Calendar& cal);

	/**
	 * Called immediately after the specified person has selected their next activity
	 * and set that data in the specified NextAct. This defaults to a no-op.
	 */
	virtual void nextActSelected(std::shared_ptr<PersonType>& person, NextPlace<PlaceType>& next_place);

public:
	AbstractModel(repast::Properties& props, MPI_Datatype mpi_person_type);
	virtual ~AbstractModel();
};

template<typename PersonType, typename PlaceType, typename PersonDataType>
AbstractModel<PersonType, PlaceType, PersonDataType>::AbstractModel(repast::Properties& props, MPI_Datatype mpi_person_type) :
		data_type(mpi_person_type), local_persons(), remote_persons(), places(), local_places(), rank_(
				repast::RepastProcess::instance()->rank()) {


    Parameters::initialize(props);
    repast::ScheduleRunner& runner = repast::RepastProcess::instance()->getScheduleRunner();
    runner.scheduleStop(Parameters::instance()->getDoubleParameter("stop.at"));
}

template<typename PersonType, typename PlaceType, typename PersonDataType>
AbstractModel<PersonType, PlaceType, PersonDataType>::~AbstractModel() {
    MPI_Type_free(&data_type);
}

template<typename PersonType, typename PlaceType, typename PersonDataType>
void AbstractModel<PersonType, PlaceType, PersonDataType>::nextActSelected(std::shared_ptr<PersonType>& person, NextPlace<PlaceType>& next_place) {}


template<typename PersonType, typename PlaceType, typename PersonDataType>
void AbstractModel<PersonType, PlaceType, PersonDataType>::movePersons(Calendar& cal) {
	//pause_for_attach();
    int world_size = repast::RepastProcess::instance()->worldSize();
	std::vector<int> send_counts(world_size, 0);
	// rank, vector of local_persons to move
	std::map<int, std::vector<PersonDataType>> to_move;
	NextPlace<PlaceType> next_place;


	for (auto p_iter = local_persons.begin(); p_iter != local_persons.end();) {
		auto& person = p_iter->second;

		person->selectNextPlace(cal, next_place);
		nextActSelected(person, next_place);

		if (world_size != 1 && rank_ != next_place.placeRank()) {
			//std::cout << "moving " << person->id() << " from " << rank_ << " to " << next_act.place_rank() << std::endl;
			++send_counts[next_place.placeRank()];
			auto iter = to_move.find(next_place.placeRank());
			PersonDataType data;
			data.place_id = next_place.placeId();
			data.act_type = next_place.act_type;
			person->fillSendData(data);
			if (iter == to_move.end()) {
				std::vector<PersonDataType> vec {data};
				to_move.insert(std::make_pair(next_place.placeRank(), vec));
			} else {
			    iter->second.push_back(data);
			}

			if (remote_persons.find(p_iter->first) == remote_persons.end()) {
				remote_persons.insert((*p_iter));
			}

			p_iter = local_persons.erase(p_iter);
		} else {
			next_place.place->addPerson(person, next_place.act_type);
			++p_iter;
		}
	}

	PersonDataType* data = nullptr;
	int total_amt_received = move_persons2(send_counts, to_move, data_type, &data);
	if (total_amt_received > 0) {
		integrateMovedPersons(total_amt_received, data);
	}
	delete[] data;
}


template<typename PersonType, typename PlaceType, typename PersonDataType>
void AbstractModel<PersonType, PlaceType, PersonDataType>::lookupPlace(int id, int rank, PlaceIdPair<PlaceType>& pair) {
	if (id == -1) {
		pair.first = {id, rank};
		pair.second = NULL_PTR;
	} else {
		std::shared_ptr<PlaceType>* place;
		auto iter = places.find(id);
		if (rank == rank_ && iter != places.end()) {
			place = &(iter->second);
		} else {
			place = &NULL_PTR;
		}

		pair.first = {id, rank};
		pair.second = *place;
	}
}

template<typename PersonType, typename PlaceType, typename PersonDataType>
void AbstractModel<PersonType, PlaceType, PersonDataType>::integrateMovedPersons(int total_amt_received, PersonDataType* data) {
	for (int i = 0; i < total_amt_received; ++i) {
	    PersonDataType& d = data[i];
		int next_place_id = d.place_id;
		int act_type = d.act_type;
		int p_id = d.person_id;

		auto iter = remote_persons.find(p_id);
		if (iter == remote_persons.end()) {
		    // person not in cache, so create a new one, and add to
		    // local person. Subclass should unpack data to create
		    // the person
		    std::shared_ptr<PersonType> p = createPerson(p_id, d);

			places[next_place_id]->addPerson(p, act_type);
			remote_persons.emplace(p_id, p);
			local_persons.emplace(p_id, p);
		} else {
			updatePerson(iter->second, d);
			local_persons.insert((*iter));
			places[next_place_id]->addPerson(iter->second, act_type);
		}
	}
}

} /* namespace chi_sim */

#endif /* SRC_ABSTRACTMODEL_H_ */
