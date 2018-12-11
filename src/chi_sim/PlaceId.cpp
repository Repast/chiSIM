/*
 * PlaceId.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: nick
 */

#include "PlaceId.h"

namespace chi_sim {

std::ostream& operator<<(std::ostream& os, const PlaceId& place_id) {
	os << "PlaceId(id: " << place_id.place_id << ", r: " << place_id.rank << ")";
	return os;
}

}

