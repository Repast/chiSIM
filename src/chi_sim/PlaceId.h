/*
 * PlaceId.h
 *
 *  Created on: Jan 9, 2015
 *      Author: nick
 */

#ifndef SRC_PLACEID_H_
#define SRC_PLACEID_H_

#include <ostream>
#include <memory>

namespace chi_sim {

struct PlaceId {

	int place_id;
	int rank;

	// this will be true when a place entry in a persons
	// file is N/A or empty
	bool doesNotExist() const {
		return place_id == -1;
	}

	bool exists() const {
		return place_id != -1;
	}
};

const PlaceId NULL_PLACE_ID = {-1, -1};

std::ostream& operator<<(std::ostream& os, const PlaceId& place_id);

template<typename PlaceType>
using PlaceIdPair = std::pair<chi_sim::PlaceId, std::shared_ptr<PlaceType>>;

}




#endif /* SRC_PLACEID_H_ */
