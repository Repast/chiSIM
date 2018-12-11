/*
 * SamplePerson.cpp
 *
 *  Created on: Oct 27, 2017
 *      Author: nick
 */

#include "SamplePerson.h"

using namespace chi_sim;

namespace chi_sim_model {

SamplePerson::SamplePerson(unsigned int num_id, const mrsa::Places<Place>& places, ActsPtr& weekday_acts, ActsPtr& weekend_acts) : AbstractPerson(num_id),
        places_{places}, weekday_acts_{weekday_acts}, weekend_acts_{weekend_acts} {

}

SamplePerson::~SamplePerson() {
    // TODO Auto-generated destructor stub
}

void SamplePerson::selectHome(NextPlace<Place>& next_act) {
    next_act.act_type = 0;
    if (places_.household.first.doesNotExist()) {
        // person doesn't have a household
        next_act.id = places_.group_quarters.first;
        next_act.place = places_.group_quarters.second.get();
    } else {
        next_act.id = places_.household.first;
        next_act.place = places_.household.second.get();
    }
}

void SamplePerson::fillSendData(SamplePersonData& data) {
    AbstractPerson<Place, SamplePersonData>::fillSendData(data);
      data.weekday_acts = weekday_acts_->id();
      data.weekend_acts = weekend_acts_->id();
      data.places[0] =  places_.household.first.place_id;
      data.places[1] =  places_.household.first.rank;
      data.places[2] =  places_.group_quarters.first.place_id;
      data.places[3] =  places_.group_quarters.first.rank;
      data.places[4] =  places_.work.first.place_id;
      data.places[5] =  places_.work.first.rank;
      data.places[6] =  places_.school.first.place_id;
      data.places[7] =  places_.school.first.rank;
      data.places[8] =  places_.daycare.first.place_id;
      data.places[9] =  places_.daycare.first.rank;
      data.places[10] =  places_.gym.first.place_id;
      data.places[11] =  places_.gym.first.rank;
      data.places[12] =  places_.hospital.first.place_id;
      data.places[13] =  places_.hospital.first.rank;
      data.places[14] =  places_.jail.first.place_id;
      data.places[15] =  places_.jail.first.rank;
      data.places[16] =  places_.other_households[0].first.place_id;
      data.places[17] =  places_.other_households[0].first.rank;
      data.places[18] =  places_.other_households[1].first.place_id;
      data.places[19] =  places_.other_households[1].first.rank;
      data.places[20] =  places_.other_households[2].first.place_id;
      data.places[21] =  places_.other_households[2].first.rank;
      data.places[22] =  places_.other_households[3].first.place_id;
      data.places[23] =  places_.other_households[3].first.rank;
}


void SamplePerson::selectNextPlace(Calendar& calendar, NextPlace<Place>& next_act) {
    // iterate through the activity list and find the activity
    // whose time span contains the specified time.
    // use the weekday or weekend list depending.
    ActsPtr& acts = calendar.isWeekDay() ? weekday_acts_ : weekend_acts_;
    std::shared_ptr<ActT> act = acts->findActivityAt(calendar.hour_of_day);

    // if we don't find an activity for this time, go to a household
    // if it exists or to the group_quarters as a household stand-in.
    if (act == mrsa::Activities<ActT>::NULL_ACT) {
        //std::cout << "no activity so going to household" << std::endl;
        selectHome(next_act);
    } else {
        chi_sim::PlaceIdPair<Place> place_data =  act->selectPlace(places_);
        if (place_data.first.doesNotExist()) {
            // place doesn't exist -- this can happen if the workplace
            // for example is outside the defined area), so go home
            selectHome(next_act);
        } else {
            next_act.id = place_data.first;
            next_act.place = place_data.second.get();
            next_act.act_type = act->activityType();
        }
    }
}

} /* namespace chi_sim_model */
