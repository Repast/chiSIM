/*
 * typedefs.h
 *
 *  Created on: Feb 8, 2016
 *      Author: nick
 */

#ifndef MODEL_SRC_TYPEDEFS_H_
#define MODEL_SRC_TYPEDEFS_H_

#include <memory>

#include "../src/mrsa/Places.h"
#include "../src/mrsa/Activities.h"
#include "../src/mrsa/Activity.h"

namespace chi_sim_model {

class Place;

using ActT =  mrsa::Activity<Place>;

using ActivitiesPtr = std::shared_ptr<mrsa::Activities<ActT>>;
using PlacesType = mrsa::Places<Place>;
using PlacePtr = std::shared_ptr<Place>;

}


#endif /* MODEL_SRC_TYPEDEFS_H_ */
