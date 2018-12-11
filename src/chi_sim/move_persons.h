/*
 * movepersons.h
 *
 *  Created on: Jan 14, 2015
 *      Author: nick
 */

#ifndef SRC_MOVE_PERSONS_H_
#define SRC_MOVE_PERSONS_H_

#include <map>
#include <vector>
#include <numeric>

#include "repast_hpc/RepastProcess.h"

#include "mpi.h"

namespace chi_sim {

template <typename T>
struct MPIDataTypeTraits;

template<>
struct MPIDataTypeTraits<int> {
	static MPI_Datatype data_type() {
		return MPI_INT;
	}
};

template<>
struct MPIDataTypeTraits<double> {
	static MPI_Datatype data_type() {
		return MPI_DOUBLE;
	}
};


/**
 * @param send_counts the amount this rank will to send to other ranks indexed by rank, that is,
 * if say element 2 has a value of 4, then this rank will send 4 Ts to rank 2
 * @param to_move map of Ts to move - k: rank to send to, v the Ts to send
 */
template<typename T>
int move_persons2(std::vector<int>& send_counts, std::map<int, std::vector<T>>& to_move, MPI_Datatype& data_type,  T** out) {

    boost::mpi::communicator* comm = repast::RepastProcess::instance()->getCommunicator();
    int world_size = repast::RepastProcess::instance()->worldSize();
    std::vector<int> receive(world_size, 0);

#ifdef PROFILE_ON
    boost::timer::cpu_timer timer;
    timer.start();
#endif

    // every rank gets the count of Ts it will receive from all the other ranks
    MPI_Alltoall(&send_counts[0], 1, MPI_INT, &receive[0], 1, MPI_INT, *comm);

#ifdef PROFILE_ON
    timer.stop();
    // record send count duration
    (*FileOutput::instance()) << timer.format(6, "%t") << ",";
#endif

    // send displacement -- offset from start of send_data buffer
    // value of element i is the index in the send buffer where
    // rank i's items start.
    std::vector<int> send_displacement;
    int sum = 0;
    for (int i = 0; i < world_size; ++i) {
        send_displacement.push_back(sum);
        int count = send_counts[i];
        sum += count;
    }

    // insert the send_data
    std::vector<T> send_data;
    for (auto& kv : to_move) {
        send_data.insert(send_data.end(), kv.second.begin(), kv.second.end());
    }

    // receive_displacement -- offset from start of receive buffer
    // value of element i is the index in the receive buffer where
    // rank i's items start.
    std::vector<int> receive_displacement;
    sum = 0;
    for (int i = 0; i < world_size; ++i) {
        receive_displacement.push_back(sum);
        int count = receive[i];
        sum += count;
    }

    int total_amt_to_receive = std::accumulate(receive.begin(), receive.end(), 0);
    *out = new T[total_amt_to_receive];

#ifdef PROFILE_ON
    timer.start();
#endif
    MPI_Alltoallv(&send_data[0], &send_counts[0], &send_displacement[0],
           data_type, (*out), &receive[0], &receive_displacement[0],
            data_type, *comm);
#ifdef PROFILE_ON
    timer.stop();
    // record send data duration
    (*FileOutput::instance()) << timer.format(6, "%t") << ",";
#endif

    return total_amt_to_receive;

}


/**
 * Moves persons as ints between processes.
 *
 * @param the number of ints for each person
 * @param send_counts value in ith element of vector is the amount to send to the
 * ith process
 * @param to_move key is the rank of the process to send to, vector are the ints to send
 * to that process to be reconstituted into persons
 * @param out the ints received by this process will be put in here
 * @return the total number of ints received (i.e. the size of out)
 *
 * T the data type that is sent between processes
 */
template<typename T>
int move_persons(int moved_data_size, std::vector<int>& send_counts, std::map<int, std::vector<T>>& to_move,
		T** out) {

	boost::mpi::communicator* comm = repast::RepastProcess::instance()->getCommunicator();
	int world_size = repast::RepastProcess::instance()->worldSize();
	std::vector<int> receive(world_size, 0);
#ifdef PROFILE_ON
	boost::timer::cpu_timer timer;
	timer.start();
#endif
	MPI_Alltoall(&send_counts[0], 1, MPI_INT, &receive[0], 1, MPI_INT, *comm);

#ifdef PROFILE_ON
	timer.stop();
	// record send count duration
	(*FileOutput::instance()) << timer.format(6, "%t") << ",";
#endif

	// send displacement is offset from the start of the send buffer
	std::vector<int> send_displacement;
	int sum = 0;
	for (int i = 0; i < world_size; ++i) {
		send_displacement.push_back(sum);
		int count = send_counts[i];
		sum += count * moved_data_size;
		// we are re-using send_counts to specify how many
		// ints to send so reset to "how many persons to send * person data size"
		send_counts[i] = count * moved_data_size;
	}

	std::vector<T> send_data;
	for (auto& kv : to_move) {
		send_data.insert(send_data.end(), kv.second.begin(), kv.second.end());
	}

	// offset from start of receive buffer
	std::vector<int> receive_displacement;
	sum = 0;
	for (int i = 0; i < world_size; ++i) {
		receive_displacement.push_back(sum);
		int count = receive[i];
		sum += count * moved_data_size;
		// we are re-using receive to specify how many
		// ints to receive so reset to "how many persons to receive * person data size"
		receive[i] = count * moved_data_size;
	}

	int total_amt_to_receive = std::accumulate(receive.begin(), receive.end(), 0);
	*out = new T[total_amt_to_receive];

#ifdef PROFILE_ON
	timer.start();
#endif
	MPI_Alltoallv(&send_data[0], &send_counts[0], &send_displacement[0],
			MPIDataTypeTraits<T>::data_type(), (*out), &receive[0], &receive_displacement[0],
			MPIDataTypeTraits<T>::data_type(), *comm);
#ifdef PROFILE_ON
	timer.stop();
	// record send data duration
	(*FileOutput::instance()) << timer.format(6, "%t") << ",";
#endif

	return total_amt_to_receive;
}


} /* namespace chi_sim */

#endif /* SRC_MOVE_PERSONS_H_ */
