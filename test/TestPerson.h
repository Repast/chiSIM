/*
 * TestPerson.h
 *
 *  Created on: Jun 10, 2016
 *      Author: nick
 */

#ifndef TESTPERSON_H_
#define TESTPERSON_H_

#include <memory>

#include "../src/chi_sim/AbstractPerson.h"
#include "../src/chi_sim/NextPlace.h"
#include "../src/mrsa/Activity.h"
#include "../src/mrsa/Activities.h"
#include "chi_sim/Calendar.h"

#include "TestPlace.h"
#include "typedefs.h"


struct TestPersonData {

    unsigned int person_id;
    int place_id;
    int act_type;
    unsigned int weekend_acts;
    unsigned int weekday_acts;
    int places[24];

    double val;
};

class TestPerson: public chi_sim::AbstractPerson<TestPlace, TestPersonData> {

private:
    void selectHome(chi_sim::NextPlace<TestPlace>& next_act);


public:
    std::shared_ptr<mrsa::Activities<ActT>> weekday_acts_, weekend_acts_;
	std::vector<std::string>& line_;
	double val;
	mrsa::Places<TestPlace> places_;

	TestPerson(unsigned int num_id, const mrsa::Places<TestPlace>& places, ActsPtr& weekday_acts,
			ActsPtr& weekend_acts, std::vector<std::string>& line);
	virtual ~TestPerson();

	void fillSendData(TestPersonData& data);

	void selectNextPlace(chi_sim::Calendar& cal, chi_sim::NextPlace<TestPlace>& next_place);

	mrsa::Places<TestPlace>& places() {
		return places_;
	}

	ActsPtr& weekday_acts() {
		return weekday_acts_;
	}

	ActsPtr& weekend_acts() {
		return weekend_acts_;
	}
};

#endif /* TESTPERSON_H_ */
