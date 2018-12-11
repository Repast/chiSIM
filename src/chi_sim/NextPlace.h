/*
 * NextAct.h
 *
 *  Created on: Jan 6, 2015
 *      Author: nick
 */

#ifndef SRC_NEXTPLACE_H_
#define SRC_NEXTPLACE_H_

#include "PlaceId.h"

namespace chi_sim {

template<typename PlaceType>
struct NextPlace {

	PlaceId id{-1, -1};
	PlaceType* place{nullptr};
	int act_type{0};

	int placeRank() {
		return id.rank;
	}

	int placeId() {
	    return id.place_id;
	}
};

}

#endif /* SRC_NEXTPLAE_H_ */
