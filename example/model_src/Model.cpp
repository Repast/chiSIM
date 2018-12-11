/*
 * Model.cpp
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */
#include <map>
#include <memory>
#include <sstream>
#include <iterator>

#ifdef __APPLE__
#include <mach/task.h>
#include <mach/mach.h>
#endif

#ifdef __linux
#include <sys/resource.h>
#include <time.h>
#endif

#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/io.h"

#ifdef PROFILE_ON
#include "FileOutput.h"
#include <boost/timer/timer.hpp>
#endif

#include "../src/mrsa/Places.h"
#include "../src/mrsa/ActivitiesCreator.h"
#include "../src/mrsa/PersonCreator.h"
#include "../src/mrsa/PlaceCreator.h"
#include "../src/mrsa/PersonCreator.h"

#include "typedefs.h"
#include "constants.h"
#include "Household.h"
#include "Workplace.h"
#include "School.h"
#include "Jail.h"
#include "Gym.h"
#include "Hospital.h"
#include "GeneralQuarters.h"
#include "Model.h"


using namespace repast;
using namespace std;
using namespace chi_sim;
using namespace mrsa;

namespace chi_sim_model {

const std::string PERSONS_FILE = "persons.file";
const std::string PLACES_FILE = "places.file";
const std::string ACTIVITIES_FILE = "activities.file";
const std::string RISK_FILE = "risk.file";

const std::string INITIAL_INFECTION_COUNT = "initial.infected.count";

Timings timings;

#ifdef __APPLE__
int getmem(unsigned long long *rss, unsigned long long *vs) {
	struct task_basic_info_64 t_info;
	mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_64_COUNT;

	if (KERN_SUCCESS != task_info(mach_task_self(),
					TASK_BASIC_INFO_64, (task_info_t) &t_info, &t_info_count)) {
		return -1;
	}

	if (timings.mem_use < t_info.resident_size) {
		timings.mem_use = t_info.resident_size;
	}

	*rss = t_info.resident_size;
	*vs = t_info.virtual_size;
	return 0;
}
#endif

std::string get_mem() {

	unsigned long long rss = 0, vss = 0;
#ifdef __APPLE__
	getmem(&rss, &vss);
#elif __linux
	rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	rss = usage.ru_maxrss * 1024.0;
#endif
	std::stringstream str;
	str << "RM: " << (rss / 1024.0 / 1024.0) << " MB, VM: " << (vss / 1024.0 / 1024.0) << " MB";
	/*(r_usage.ru_maxrss / 1024.0 / 1024.0) << " MB"; */
	return str.str();
}

void print_month(int tick) {
	// on process 0
	if (RepastProcess::instance()->rank() == 0 && tick % 720 == 0) {
		std::string time;
		repast::timestamp(time);
		std::cout << time << " -- Month: " << (tick / 720) << ", " << get_mem() << std::endl;
	}
}

PlacePtr ModelPlaceCreator::createLocalPlace(int int_id, int rank, const std::string& type,
		const std::vector<string>& line) const {
	if (type == SCHOOL_TYPE) {
		return std::make_shared<School>(int_id, rank, line);
	} else if (type == HOUSEHOLD_TYPE) {
		return std::make_shared<Household>(int_id, rank, line);
	} else if (type == WORKPLACE_TYPE) {
		return std::make_shared<Workplace>(int_id, rank, line);
	} else if (type == JAIL_TYPE) {
		return std::make_shared<Jail>(int_id, rank, line);
	} else if (type == GYM_TYPE) {
		return std::make_shared<Gym>(int_id, rank, line);
	} else if (type == HOSPITAL_TYPE) {
		return std::make_shared<Hospital>(int_id, rank, line);
	} else if (type == DORM_TYPE || type == NURSING_HOME_TYPE) {
		return std::make_shared<GeneralQuarters>(int_id, rank, line);
	} else {
		throw std::invalid_argument("Unknown place type: " + type);
	}
}

PlacePtr ModelPlaceCreator::createRemotePlace(int int_id, int rank, const std::vector<string>& line) const {
	return make_shared<Place>(int_id, rank, line);
}

Model::Model(repast::Properties& props, MPI_Datatype mpi_person_type) :
		AbsModelT(props, mpi_person_type), cal(), creator() {

    initializePopulation();
	ScheduleRunner& runner = RepastProcess::instance()->getScheduleRunner();
	runner.scheduleEvent(1, 1, Schedule::FunctorPtr(new MethodFunctor<Model>(this, &Model::step)));
}

void Model::initializePopulation() {
    // create a random distribution used by Person-s to
    // choose which "other household" to go to
    repast::_IntUniformGenerator gen(repast::Random::instance()->engine(),
        boost::uniform_int<>(0, 3));
    repast::Random::instance()->putGenerator(OH_DIST,
        new repast::DefaultNumberGenerator<repast::_IntUniformGenerator>(gen));

    std::map<int, PlacePtr> place_map;
    createPlaces(place_map);

    typename ActivitiesCreator<Place>::ActMap activities_map;
    ActivitiesCreator<Place> acts_creator(
        Parameters::instance()->getStringParameter(ACTIVITIES_FILE));
    acts_creator.run(activities_map);

    createPersons(local_persons, place_map, activities_map);

    for (auto iter : place_map) {
      places.insert(make_pair(iter.second->placeId().place_id, iter.second));
      if (iter.second->placeId().rank == rank_) {
        local_places.push_back(iter.second);
      }
    }

    for (auto iter : activities_map) {
      acts_cache.emplace(iter.second->id(), iter.second);
    }
}

Model::~Model() {

}

void Model::createPlaces(map<int, std::shared_ptr<Place>>& place_map) {
    ModelPlaceCreator pc;
	mrsa::create_places(Parameters::instance()->getStringParameter(PLACES_FILE), place_map, pc);
}

void Model::createPersons(std::map<unsigned int, std::shared_ptr<SamplePerson>>& local_persons, std::map<int, PlacePtr>& place_map,
                        std::map<int, ActivitiesPtr>& act_map) {

	PersonCreator<SamplePerson, Place> person_creator(Parameters::instance()->getStringParameter(PERSONS_FILE));
	person_creator.run(local_persons, place_map, act_map, creator);
}

std::shared_ptr<SamplePerson> Model::createPerson(unsigned int p_id, SamplePersonData& data) {
    ActsPtr weekday = acts_cache[data.weekday_acts];
    ActsPtr weekend = acts_cache[data.weekend_acts];

    mrsa::Places<Place> p_places;
    lookupPlace(data.places[0], data.places[1], p_places.household);
    lookupPlace(data.places[2], data.places[3], p_places.group_quarters);
    lookupPlace(data.places[4], data.places[5], p_places.work);
    lookupPlace(data.places[6], data.places[7], p_places.school);
    lookupPlace(data.places[8], data.places[9], p_places.daycare);
    lookupPlace(data.places[10], data.places[11], p_places.gym);
    lookupPlace(data.places[12], data.places[13], p_places.hospital);
    lookupPlace(data.places[14], data.places[15], p_places.jail);

    PlaceIdPair<Place> oh1;
    lookupPlace(data.places[16], data.places[17], oh1);
    p_places.other_households.push_back(oh1);
    PlaceIdPair<Place> oh2;
    lookupPlace(data.places[18], data.places[19], oh2);
    p_places.other_households.push_back(oh2);
    PlaceIdPair<Place> oh3;
    lookupPlace(data.places[20], data.places[21], oh3);
    p_places.other_households.push_back(oh3);
    PlaceIdPair<Place> oh4;
    lookupPlace(data.places[22], data.places[23], oh4);
    p_places.other_households.push_back(oh4);

    return std::make_shared<SamplePerson>(p_id, p_places, weekday, weekend);
}

void Model::updatePerson(std::shared_ptr<SamplePerson>& person, SamplePersonData& data) {
	// no op because we aren't sending any data other than the base
	// person data
}

void Model::step() {
	int tick = (int) RepastProcess::instance()->getScheduleRunner().currentTick();
	cal.increment();
	// print out time stamp each simulated month
	print_month(tick);
	movePersons(cal);

	size_t n = local_places.size();
#pragma omp parallel for
	for (size_t i = 0; i < n; ++i) {
		// do place based infection
		local_places[i]->reset();
	}

//	int size = local_persons.size();
//	std::cout << "rank: " << rank_ << ", size: " << size << endl;
//	int global_size;
//	MPI_Reduce(&size, &global_size, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
//	if (rank_ == 0) {
//		std::cout << "Total Persons: " << global_size << std::endl;
//	}
}

} /* namespace chi_sim */
