/*
 * move_tests.cpp
 *
 *  Created on: May 16, 2018
 *      Author: nick
 */

// run with mpirun -n 4 ./unit_tests --gtest_filter=MoveTests.*

#include "../src/chi_sim/move_persons.h"
#include "mpi.h"

using namespace std;
using namespace chi_sim;

#include "gtest/gtest.h"

struct Data {

    int place_id;
    int rank;
    unsigned int person_id;
    double places[5];
    int energy;

    Data() {}
    Data(int id, int r, unsigned int pid, double val, int e) : place_id(id),
            rank(r), person_id(pid), places{val, val * 2, val * 3, val * 4,  val * 5}, energy(e) {

    }
};

void make_data_type(MPI_Datatype* data_type) {
    int count = 5;
    MPI_Datatype types[count] = { MPI_INT, MPI_INT, MPI_UNSIGNED, MPI_DOUBLE, MPI_INT};
    int block_lengths[count] = { 1, 1, 1, 5, 1};
    MPI_Aint displacements[count];
    displacements[0] = offsetof(Data, place_id);
    displacements[1] = offsetof(Data, rank);
    displacements[2] = offsetof(Data, person_id);
    displacements[3] = offsetof(Data, places);
    displacements[4] = offsetof(Data, energy);

    MPI_Datatype tmp_type;
    MPI_Type_create_struct(count, block_lengths, displacements, types, &tmp_type);

    MPI_Aint lb, extent;
    MPI_Type_get_extent(tmp_type, &lb, &extent);
    MPI_Type_create_resized(tmp_type, lb, extent, data_type);
    MPI_Type_commit(data_type);
}

void test_eq(Data& d1, Data& d2) {
    ASSERT_EQ(d1.place_id, d2.place_id);
    ASSERT_EQ(d1.person_id, d2.person_id);
    ASSERT_EQ(d1.energy, d2.energy);
    ASSERT_EQ(d1.rank, d2.rank);

    for (int i = 0; i < 5; ++i) {
        ASSERT_EQ(d1.places[i], d2.places[i]);
    }
}


TEST(MoveTests, test_move) {

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<Data> all_data;
    for (int i = 0; i < 200; i += 5) {
        all_data.push_back(Data(i, i + 1, i + 2, (double)(i + 3), i + 4));
    }

    std::vector<int> send_counts(size, 0);
    std::map<int, std::vector<Data>> to_move;

    if (rank == 0) {
        to_move.emplace(1, std::vector<Data>{all_data[0]});
        to_move[1].push_back(all_data[1]);
        to_move[1].push_back(all_data[2]);
        send_counts[1] = 3;

        to_move.emplace(3, std::vector<Data>{all_data[9]});
        to_move[3].push_back(all_data[10]);
        to_move[3].push_back(all_data[11]);
        to_move[3].push_back(all_data[12]);
        send_counts[3] = 4;

    } else if (rank == 1) {
        to_move.emplace(0, std::vector<Data>{all_data[20]});
        to_move[0].push_back(all_data[21]);
        to_move[0].push_back(all_data[22]);
        send_counts[0] = 3;

        to_move.emplace(2, std::vector<Data>{all_data[15]});
        to_move[2].push_back(all_data[16]);
        to_move[2].push_back(all_data[17]);
        to_move[2].push_back(all_data[18]);
        send_counts[2] = 4;

    } else if (rank == 2) {
        to_move.emplace(1, std::vector<Data>{all_data[30]});
        to_move[1].push_back(all_data[31]);
        to_move[1].push_back(all_data[32]);
        send_counts[1] = 3;

        to_move.emplace(3, std::vector<Data>{all_data[25]});
        to_move[3].push_back(all_data[26]);
        send_counts[3] = 2;

    } else if (rank == 3) {
        to_move.emplace(0, std::vector<Data>{all_data[36]});
        send_counts[0] = 1;
    }

    MPI_Datatype data_type;
    Data* received = nullptr;
    make_data_type(&data_type);
    int total = move_persons2(send_counts, to_move, data_type,  &received);

    if (rank == 0) {
        ASSERT_EQ(total, 4);
        test_eq(received[0], all_data[20]);
        test_eq(received[1], all_data[21]);
        test_eq(received[2], all_data[22]);
        test_eq(received[3], all_data[36]);

    } else if (rank == 1) {
        ASSERT_EQ(total, 6);
        test_eq(received[0], all_data[0]);
        test_eq(received[1], all_data[1]);
        test_eq(received[2], all_data[2]);
        test_eq(received[3], all_data[30]);
        test_eq(received[4], all_data[31]);
        test_eq(received[5], all_data[32]);

    } else if (rank == 2) {
        ASSERT_EQ(total, 4);
        test_eq(received[0], all_data[15]);
        test_eq(received[1], all_data[16]);
        test_eq(received[2], all_data[17]);
        test_eq(received[3], all_data[18]);

    } else if (rank == 3) {
        ASSERT_EQ(total, 6);
        test_eq(received[0], all_data[9]);
        test_eq(received[1], all_data[10]);
        test_eq(received[2], all_data[11]);
        test_eq(received[3], all_data[12]);
        test_eq(received[4], all_data[25]);
        test_eq(received[5], all_data[26]);
    }


    MPI_Type_free(&data_type);
    delete[] received;
}

