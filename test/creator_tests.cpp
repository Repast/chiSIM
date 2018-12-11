/*
 * creator_tests.cpp
 *
 *  Created on: Jun 9, 2016
 *      Author: nick
 */

#include <utility>

#include "gtest/gtest.h"

#include "../src/mrsa/ActivitiesCreator.h"
#include "../src/mrsa/PersonCreator.h"
#include "../src/mrsa/PlaceCreator.h"

#include "TestPlace.h"
#include "TestPerson.h"
#include "common.h"

using namespace std;
using namespace chi_sim;
using namespace repast;
using namespace mrsa;


// run with mpirun -n 2
TEST(CreatorTests, place_creator) {
	map<int, shared_ptr<TestPlace>> places;

	PLC creator;
	mrsa::create_places("../test_data/places_ns.csv", places, creator);

	int rank = RepastProcess::instance()->rank();
	ASSERT_EQ(29143, places.size());

	TestPlacePtr p = places[1];
	ASSERT_EQ(1, p->id_);
	ASSERT_EQ(60611, p->zip);
	ASSERT_EQ(2, p->zip_grp);
	ASSERT_EQ(0, p->rank_);
	ASSERT_NEAR(-87.619157, p->lon, 0);
	ASSERT_NEAR(41.895282, p->lat, 0);
	if (rank == 0) {
		ASSERT_EQ("hospital", p->type_);
		ASSERT_TRUE(p->is_local);
	} else {
		ASSERT_EQ("", p->type_);
		ASSERT_FALSE(p->is_local);
	}

	p = places[56];
	ASSERT_EQ(56, p->id_);
	ASSERT_EQ(60626, p->zip);
	ASSERT_EQ(1, p->zip_grp);
	ASSERT_EQ(1, p->rank_);
	ASSERT_NEAR(-87.669765, p->lon, 0);
	ASSERT_NEAR(42.021176, p->lat, 0);
	if (rank == 0) {
		ASSERT_EQ("", p->type_);
		ASSERT_FALSE(p->is_local);
	} else {
		ASSERT_EQ("workplace", p->type_);
		ASSERT_TRUE(p->is_local);
	}
}

TEST(CreatorTests, activities_creator) {
	map<int, ActsPtr> acts_map;
	ActivitiesCreator<TestPlace> ac("../test_data/activities_ns.csv");
	ac.run(acts_map);

	vector<string> attributes(9, "1");
	Places<TestPlace> places;
	places.work = make_pair(PlaceId { 1, 0 }, make_shared<TestPlace>(1, 0, "workplace", attributes, true));
	places.household = make_pair(PlaceId { 2, 0 }, make_shared<TestPlace>(2, 0, "household", attributes, true));

	ASSERT_EQ(6937, acts_map.size());
	ActsPtr acts = acts_map[2869];
	ActPtr act = acts->findActivityAt(13);
	PID pp = act->selectPlace(places);
	ASSERT_EQ(1, pp.second->id_);

	acts = acts_map[2869];
	act = acts->findActivityAt(18);
	pp = act->selectPlace(places);
	ASSERT_EQ(2, pp.second->id_);
}

TEST(CreatorTests, person_creator) {
	map<int, ActsPtr> acts_map;
	ActivitiesCreator<TestPlace> ac("../test_data/activities_ns.csv");
	ac.run(acts_map);

	map<int, shared_ptr<TestPlace>> places;
	PLC plc_creator;
	mrsa::create_places("../test_data/places_ns.csv", places, plc_creator);

	map<unsigned int, PersonPtr> person_map;
	PersonCreator<TestPerson, TestPlace> pc("../test_data/persons_ns.csv");
	PC pc_creator;
	pc.run(person_map, places, acts_map, pc_creator);

	auto iter = person_map.find(49);
	int rank = RepastProcess::instance()->rank();
	if (rank == 0) {
		PersonPtr p = iter->second;
		ASSERT_EQ(49, p->id());
		ASSERT_EQ(7890, p->places().household.first.place_id);
		ASSERT_EQ(1678, p->places().work.first.place_id);
		ASSERT_EQ(764, p->weekday_acts()->id());
		ASSERT_EQ(15974, p->weekday_acts()->findActivityAt(1)->id());
		ASSERT_EQ(407, p->weekend_acts()->id());
		ASSERT_EQ(9000, p->weekend_acts()->findActivityAt(19)->id());
	} else {
		ASSERT_TRUE(iter == person_map.end());
	}

	iter = person_map.find(50);
	if (rank == 1) {
		PersonPtr p = iter->second;
		ASSERT_EQ(50, p->id());
		ASSERT_EQ(7891, p->places().household.first.place_id);
		ASSERT_EQ(254, p->weekday_acts()->id());
		ASSERT_EQ(4148, p->weekend_acts()->id());
	} else {
		ASSERT_TRUE(iter == person_map.end());
	}

}

