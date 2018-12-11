/*
 * TestModel.cpp
 *
 *  Created on: Jun 14, 2016
 *      Author: nick
 */

#include "../src/mrsa/PlaceCreator.h"
#include "../src/mrsa/PersonCreator.h"
#include "../src/mrsa/mrsa_constants.h"
#include "../src/mrsa/ActivitiesCreator.h"
#include "../src/mrsa/Places.h"

#include "TestModel.h"

using namespace chi_sim;
using namespace mrsa;

TestModel::TestModel(repast::Properties& props, MPI_Datatype mpi_person_type) :
		AbsModelT(props, mpi_person_type), acts_cache{} {
    initializePopulation();
}

TestModel::~TestModel() {
}

void TestModel::initializePopulation() {
    // create a random distribution used by Person-s to
    // choose which "other household" to go to
    repast::_IntUniformGenerator gen(repast::Random::instance()->engine(),
        boost::uniform_int<>(0, 3));
    repast::Random::instance()->putGenerator(mrsa::OH_DIST,
        new repast::DefaultNumberGenerator<repast::_IntUniformGenerator>(gen));

    std::map<int, std::shared_ptr<TestPlace>> place_map;
    createPlaces(place_map);

    typename ActivitiesCreator<TestPlace>::ActMap activities_map;
    ActivitiesCreator<TestPlace> acts_creator(
        Parameters::instance()->getStringParameter( "activities.file"));
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

void TestModel::createPlaces(std::map<int, TestPlacePtr>& place_map) {
    PLC creator;
	mrsa::create_places("../test_data/places_ns.csv", place_map, creator);
}

void TestModel::createPersons(std::map<unsigned int, PersonPtr>& local_persons, std::map<int, TestPlacePtr>& place_map,
		std::map<int, ActsPtr>& act_map) {

	PersonCreator<TestPerson, TestPlace> pc("../test_data/persons_ns.csv");
	PC pc_creator;
	pc.run(local_persons, place_map, act_map, pc_creator);
}

void TestModel::step() {
	for (auto p : local_persons) {
		p.second->val += 5.5;
	}
}

void TestModel::move(chi_sim::Calendar& cal) {
    AbsModelT::movePersons(cal);
}

PersonPtr TestModel::createPerson(unsigned int p_id, TestPersonData& data) {
    ActsPtr weekday = acts_cache[data.weekday_acts];
    ActsPtr weekend = acts_cache[data.weekend_acts];

    mrsa::Places<TestPlace> p_places;
    lookupPlace(data.places[0], data.places[1], p_places.household);
    lookupPlace(data.places[2], data.places[3], p_places.group_quarters);
    lookupPlace(data.places[4], data.places[5], p_places.work);
    lookupPlace(data.places[6], data.places[7], p_places.school);
    lookupPlace(data.places[8], data.places[9], p_places.daycare);
    lookupPlace(data.places[10], data.places[11], p_places.gym);
    lookupPlace(data.places[12], data.places[13], p_places.hospital);
    lookupPlace(data.places[14], data.places[15], p_places.jail);

    PlaceIdPair<TestPlace> oh1;
    lookupPlace(data.places[16], data.places[17], oh1);
    p_places.other_households.push_back(oh1);
    PlaceIdPair<TestPlace> oh2;
    lookupPlace(data.places[18], data.places[19], oh2);
    p_places.other_households.push_back(oh2);
    PlaceIdPair<TestPlace> oh3;
    lookupPlace(data.places[20], data.places[21], oh3);
    p_places.other_households.push_back(oh3);
    PlaceIdPair<TestPlace> oh4;
    lookupPlace(data.places[22], data.places[23], oh4);
    p_places.other_households.push_back(oh4);

    std::vector<std::string> v;
    PersonPtr p = std::make_shared<TestPerson>(p_id, p_places, weekday, weekend, v);
    p->val = data.val;
    return p;
}

/**
 * Update the state of the specified person with the data in data.
 *
 * @param index the index of the start of the block of data for this Person
 */
void TestModel::updatePerson(PersonPtr& person, TestPersonData& data) {
	person->val = data.val;
}

