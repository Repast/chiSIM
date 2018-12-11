/*
 * Activity.h
 *
 *  Created on: Apr 18, 2012
 *      Author: nick
 */

#ifndef DEFAULTACTIVITY_H_
#define DEFAULTACTIVITY_H_

#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include "repast_hpc/Utilities.h"
#include "repast_hpc/Random.h"

#include "../chi_sim/PlaceId.h"

#include "Places.h"
#include "mrsa_constants.h"


namespace mrsa {

template<typename PlaceType>
using PlaceSelector = chi_sim::PlaceIdPair<PlaceType> (*)(Places<PlaceType>&);

/**
 * Names a places that a person will go to a particular time, together
 * with the disease risk associated with that going there.
 */
template<typename PlaceType>
class Activity {

	const int PLACE_TYPE_IDX = 1;
	const int START_TIME_IDX = 2;
	const int STOP_TIME_IDX = 3;
	const int ACTIVITY_TYPE_IDX = 4;

public:

	static const int ID_IDX = 6;
	static const int SCHEDULE_ID_IDX = 5;


	//std::string place_type;

	Activity(std::vector<std::string>& params);
	// copy constructor
	Activity(const Activity& act);
	virtual ~Activity();

	/**
	 * Gets the activity type associated with this activity.
	 */
	int activityType() const {
		return activity_type;
	}

	/**
	 * Gets the schedule id of this Activity.
	 */
	unsigned int scheduleId() const {
		return schedule_id;
	}

	/**
	 * Gets the individual unique id for this Activity
	 */
	unsigned int id() const {
		return id_;
	}

	/**
	 * Gets whether or not the time space of this activity
	 * contains the specified time.
	 *
	 * @return true if it is does contain this time, otherwise false.
	 */
	bool contains(float time) const {
		//std::cout << place_type << ": " << start_time << " - " << end_time << std::endl;
		return start_time <= time && end_time > time;
	}

	/**
	 * Selects the place appropriate to this activity from the Places.
	 *
	 * @return the place appropriate to this activity from the Places.
	 */
	chi_sim::PlaceIdPair<PlaceType> selectPlace(Places<PlaceType>& places) const;

	// assignment operator
	Activity<PlaceType>& operator=(const Activity<PlaceType>&);

private:

	unsigned int id_;
	unsigned int schedule_id;
	float start_time, end_time;
	int activity_type;
	PlaceSelector<PlaceType> place_selector;

};

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_household(Places<PlaceType>& places) {
	if (places.household.first.exists())
		return places.household;
	return places.group_quarters;
}

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_workplace(Places<PlaceType>& places) {
	return places.work;
}

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_other_household(Places<PlaceType>& places) {
	repast::NumberGenerator* gen = repast::Random::instance()->getGenerator(OH_DIST);
	chi_sim::PlaceIdPair<PlaceType> place = places.other_households[(int) gen->next()];
	// one of the "other households" may be 0 if we are running
	// on a subset of the data
	if (place.first.doesNotExist()) {
		if (places.household.first.exists())
			return places.household;
		else
			return places.group_quarters;
	}
	return place;
}

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_school(Places<PlaceType>& places) {
	if (places.school.first.exists()) return places.school;
	return places.daycare;
}

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_group_quarters(Places<PlaceType>& places) {
	return places.group_quarters;
}

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_gym(Places<PlaceType>& places) {
	return places.gym;
}

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_hospital(Places<PlaceType>& places) {
	return places.hospital;
}

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> select_jail(Places<PlaceType>& places) {
	return places.jail;
}

template<typename PlaceType>
PlaceSelector<PlaceType> find_selector(const std::string& place_type)  {
	if (place_type == "Household") return &select_household<PlaceType>;
	else if (place_type == "Workplace") return &select_workplace<PlaceType>;
	else if (place_type == "Other Household") return & select_other_household<PlaceType>;
	else if (place_type == "School") return &select_school<PlaceType>;
	else if (place_type == "Group Quarters") return &select_group_quarters<PlaceType>;
	else if (place_type == "Gym") return &select_gym<PlaceType>;
	else if (place_type == "Hospital") return &select_hospital<PlaceType>;
	else if (place_type == "Prison") return &select_jail<PlaceType>;
	else return &select_household<PlaceType>;
}

// initializes the activities fields from the vector.
template<typename PlaceType>
Activity<PlaceType>::Activity(std::vector<std::string>& vec) :
		id_(std::stol(vec[ID_IDX])), schedule_id(std::stol(vec[SCHEDULE_ID_IDX])), start_time(0),
		end_time(0), activity_type(0), place_selector(find_selector<PlaceType>(vec[PLACE_TYPE_IDX])) {

	std::string val = vec[START_TIME_IDX];
	val = repast::trim(val);
	// minutes to hours
	start_time = std::stoi(val) / 60.0f;

	val = vec[STOP_TIME_IDX];
	val = repast::trim(val);
	// minutes to hours
	end_time = std::stoi(val) / 60.0f;

	val = vec[ACTIVITY_TYPE_IDX];
	val = repast::trim(val);
	activity_type = std::stoi(val);

	//place_type = vec[PLACE_TYPE_IDX];
}

// copy constructor
template<typename PlaceType>
Activity<PlaceType>::Activity(const Activity& act) :
		id_(act.id_), schedule_id(act.schedule_id), start_time(act.start_time), end_time(
				act.end_time), activity_type(act.activity_type), place_selector(act.place_selector){
}


// assignment operator
template<typename PlaceType>
Activity<PlaceType>& Activity<PlaceType>::operator=(const Activity& rhs) {
	if (&rhs != this) {
		this->id_ = rhs.id_;
		this->schedule_id = rhs.schedule_id;
		this->place_selector = rhs.place_selector;
		this->start_time = rhs.start_time;
		this->end_time = rhs.end_time;
		this->activity_type = rhs.activity_type;
	}

	return *this;
}
template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> Activity<PlaceType>::selectPlace(Places<PlaceType>& places) const {
	return place_selector(places);
}

template<typename PlaceType>
Activity<PlaceType>::~Activity() {
}

} /* namespace chi_sim */
#endif /* ACTIVITY_H_ */
