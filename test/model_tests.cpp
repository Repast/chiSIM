/*
 * moving_tests.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: nick
 */
#include "gtest/gtest.h"

#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/initialize_random.h"

#include "../src/chi_sim/AbstractModel.h"

#include "common.h"
#include "TestModel.h"

using namespace chi_sim;
using namespace std;

/*
 *
struct TestPersonData {

    unsigned int person_id;
    int place_id;
    int act_type;
    unsigned int weekend_acts;
    unsigned int weekday_acts;
    int places[12];

    double val;
};
 *
 */


void make_mpi_type(MPI_Datatype* dt) {
    int count = 7;
    MPI_Datatype types[count] = { MPI_UNSIGNED, MPI_INT, MPI_INT, MPI_UNSIGNED, MPI_UNSIGNED,
            MPI_INT, MPI_DOUBLE };
    int block_lengths[count] = { 1, 1, 1, 1, 1, 24, 1 };
    MPI_Aint displacements[count];
    displacements[0] = offsetof(TestPersonData, person_id);
    displacements[1] = offsetof(TestPersonData, place_id);
    displacements[2] = offsetof(TestPersonData, act_type);
    displacements[3] = offsetof(TestPersonData, weekend_acts);
    displacements[4] = offsetof(TestPersonData, weekday_acts);
    displacements[5] = offsetof(TestPersonData, places);
    displacements[6] = offsetof(TestPersonData, val);

    MPI_Datatype tmp_type;
    MPI_Type_create_struct(count, block_lengths, displacements, types, &tmp_type);

    MPI_Aint lb, extent;
    MPI_Type_get_extent(tmp_type, &lb, &extent);
    MPI_Type_create_resized(tmp_type, lb, extent, dt);
    MPI_Type_commit(dt);
}

/**
 * Runs these tests on their own with:
 *
 *  mpirun -n 2 ./unit_tests --gtest_filter=ModelTests.**
 */

struct ModelTests: public ::testing::Test {

	TestModel* model;
	int rank;

	ModelTests() :
			model(nullptr), rank(repast::RepastProcess::instance()->rank()) {
		repast::Properties props("../test_data/test_model.props");
		initializeRandom(props);
		boost::mpi::communicator comm;
		MPI_Datatype dt;
		make_mpi_type(&dt);
		model = new TestModel(props, dt);
	}

	~ModelTests() {
		delete model;
	}
};

TestPerson* find_person(map<unsigned int, PersonPtr> p_map, unsigned int id) {
	auto iter = p_map.find(id);
	if (iter == p_map.end()) {
		return nullptr;
	} else {
		return iter->second.get();
	}
}

void increment_calender(Calendar& cal, int amt) {
	for (int i = 0; i < amt; ++i) {
		cal.increment();
	}
}
TEST_F(ModelTests, test_creation) {
	TestPerson* p = find_person(model->localPersons(), 49);
	if (rank == 0) {
		ASSERT_EQ(44038, model->localPersons().size());
		ASSERT_TRUE(p);
		ASSERT_EQ(49, p->id());
		ASSERT_EQ(59, p->val);
		ASSERT_EQ(7890, p->places().household.first.place_id);
		ASSERT_EQ(1678, p->places().work.first.place_id);
		ASSERT_EQ(764, p->weekday_acts()->id());
		ASSERT_EQ(407, p->weekend_acts()->id());
	} else {
		ASSERT_EQ(3, model->localPersons().size());
		ASSERT_FALSE(p);
	}
}

/**
 * Person 49.
 * hh_id: 7890 on rank 0
 * work_id: 1678 on rank 1
 * weekday act id: 764
 * weekend act id: 407
 * NOTE: these may not be the same as the canonical data
 */
TEST_F(ModelTests, test_move) {
	Calendar cal;
	TestPerson* p = find_person(model->localPersons(), 49);
	if (rank == 0) {
		ASSERT_TRUE(p);
	} else {
		ASSERT_FALSE(p);
	}

	// increment calendar to when p49 is at work
	increment_calender(cal, 8);
	for (auto p : model->allPlaces()) {
		p.second->person_map.clear();
	}

	model->move(cal);
	p = find_person(model->localPersons(), 49);
	if (rank == 0) {
		ASSERT_FALSE(p);
		auto iter = model->allPlaces().find(1678);
		ASSERT_TRUE(iter != model->allPlaces().end());
		auto iter2 = iter->second->person_map.find(49);
		ASSERT_TRUE(iter2 == iter->second->person_map.end());
	} else {
		// moved to 1
		ASSERT_TRUE(p);
		ASSERT_EQ(49, p->id());
		ASSERT_EQ(49 + 10, p->val);
		auto iter = model->allPlaces().find(1678);
		ASSERT_TRUE(iter != model->allPlaces().end());
		auto iter2 = iter->second->person_map.find(49);
		ASSERT_FALSE(iter2 == iter->second->person_map.end());
	}

	// while on 1 person.val set to person.val + 5.5
	model->step();

	// next day, using weekday schedule
	// should be back home
	increment_calender(cal, 28);
	for (auto p : model->allPlaces()) {
		p.second->person_map.clear();
	}
	model->move(cal);

	p = find_person(model->localPersons(), 49);
	if (rank == 1) {
		ASSERT_FALSE(p);
		auto iter = model->allPlaces().find(7890);
		ASSERT_TRUE(iter != model->allPlaces().end());
		auto iter2 = iter->second->person_map.find(49);
		ASSERT_TRUE(iter2 == iter->second->person_map.end());
	} else {
		ASSERT_TRUE(p);
		ASSERT_EQ(49 + 10 + 5.5, p->val);
		auto iter = model->allPlaces().find(7890);
		ASSERT_TRUE(iter != model->allPlaces().end());
		auto iter2 = iter->second->person_map.find(49);
		ASSERT_FALSE(iter2 == iter->second->person_map.end());
	}
}
