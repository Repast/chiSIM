/*
 * SamplePerson.h
 *
 *  Created on: Oct 27, 2017
 *      Author: nick
 */

#ifndef MODEL_SRC_SAMPLEPERSON_H_
#define MODEL_SRC_SAMPLEPERSON_H_

#include "../src/chi_sim/AbstractPerson.h"
#include "../src/chi_sim/NextPlace.h"
#include "../src/chi_sim/Calendar.h"

#include "../src/mrsa/Activities.h"
#include "../src/mrsa/Activity.h"
#include "../src/mrsa/Places.h"

#include "Place.h"

namespace chi_sim_model {

struct SamplePersonData {

    unsigned int person_id;
    int place_id;
    int act_type;
    unsigned int weekend_acts;
    unsigned int weekday_acts;
    int places[24];
};


using ActT = mrsa::Activity<Place>;
using ActsPtr = std::shared_ptr<mrsa::Activities<ActT>>;

class SamplePerson : public chi_sim::AbstractPerson<Place, SamplePersonData> {

private:
    mrsa::Places<Place> places_;
    ActsPtr weekday_acts_, weekend_acts_;

    void selectHome(chi_sim::NextPlace<Place>& next_act);

public:
    SamplePerson(unsigned int num_id, const mrsa::Places<Place>& places, ActsPtr& weekday_acts, ActsPtr& weekend_acts);
    virtual ~SamplePerson();

    void fillSendData(SamplePersonData& data) override;
    void selectNextPlace(chi_sim::Calendar& calendar, chi_sim::NextPlace<Place>& next_act) override;
};

} /* namespace chi_sim_model */

#endif /* MODEL_SRC_SAMPLEPERSON_H_ */
