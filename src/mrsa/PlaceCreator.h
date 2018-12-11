/*
 * PlaceCreator.h
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */

#ifndef SRC_PLACECREATOR_H_
#define SRC_PLACECREATOR_H_

#include <map>
#include <memory>
#include <string>

#include "repast_hpc/RepastProcess.h"

#include "../chi_sim/CSVReader.h"

namespace mrsa {


	static const int MRSA_PLACE_ID_IDX = 0;
	static const int MRSA_PLACE_TYPE_IDX = 2;
	static const int MRSA_PLACE_RANK_IDX = 9;


	template<typename P, typename C>
	void create_places(const std::string& place_file, std::map<int, std::shared_ptr<P>>& place_map, const C& place_creator);


template<typename P, typename C>
void create_places(const std::string& place_file, std::map<int, std::shared_ptr<P>>& place_map, const C& place_creator) {
	chi_sim::CSVReader reader(place_file);
	// skip the header
	reader.skip(1);

	int my_rank = repast::RepastProcess::instance()->rank();
	int world_size = repast::RepastProcess::instance()->worldSize();

	std::vector<std::string> line;
	while (reader.next(line)) {
		std::string type(line[MRSA_PLACE_TYPE_IDX]);
		std::transform(type.begin(), type.end(), type.begin(), ::tolower);
		line[MRSA_PLACE_TYPE_IDX] = type;
		unsigned int id(stoul(line[MRSA_PLACE_ID_IDX]));

		int rank = stoi(line[MRSA_PLACE_RANK_IDX]);
		if (rank == my_rank || world_size == 1) {
			//std::cout << "rank: " << my_rank << " making local place " << id << std::endl;
			place_map.insert(make_pair(id, place_creator.createLocalPlace(id, rank, type, line)));
		} else {
			//std::cout << "rank: " << my_rank << " making non-local place " << id << " for rank " << rank << std::endl;
			place_map.insert(make_pair(id, place_creator.createRemotePlace(id, rank, line)));
		}
	}
}

} /* namespace chi_sim */

#endif /* SRC_PLACECREATOR_H_ */
