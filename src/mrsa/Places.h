/*
 * Places.h
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */

#ifndef SRC_PLACES_H_
#define SRC_PLACES_H_

#include <memory>
#include <utility>
#include <vector>

#include "../chi_sim/PlaceId.h"


namespace mrsa {

template <typename PlaceType>
struct Places {

	using PlaceIdPair = std::pair<chi_sim::PlaceId, std::shared_ptr<PlaceType>>;

	PlaceIdPair household, group_quarters, work, school,
		daycare, gym, hospital, jail;
	std::vector<PlaceIdPair> other_households;

	Places() : household(), group_quarters(), work(), school(),
			daycare(), gym(), hospital(), jail(), other_households() {}

	Places(const Places<PlaceType>& p) : household(p.household), group_quarters(p.group_quarters), work(p.work),
			school(p.school),
			daycare(p.daycare), gym(p.gym), hospital(p.hospital),
			jail(p.jail), other_households(p.other_households) {}
};

}




#endif /* SRC_PLACES_H_ */
