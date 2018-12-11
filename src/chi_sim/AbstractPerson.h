/*
 * Person.h
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */

#ifndef SRC_ABSTRACTPERSON_H_
#define SRC_ABSTRACTPERSON_H_

#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "Calendar.h"
#include "NextPlace.h"

namespace chi_sim {

template<typename PlaceType, typename PersonDataType>
class AbstractPerson {

protected:
	unsigned int id_;

public:
	AbstractPerson(unsigned int num_id);
	virtual ~AbstractPerson();

	unsigned int id() const {
		return id_;
	}

  /**
	 * Selects the next place that this AbstractPerson will go, given the
	 * current Calendar time.
	 *
	 * @param cal
	 * @param next_place updated with the next place that this AbstractPerson
	 * will go.
	 */
	virtual void selectNextPlace(Calendar& cal, NextPlace<PlaceType>& next_place) = 0;

	/**
	 * Fills the specified PersonDataType with this AbstractPerson's data to be
	 * sent to another process when this person is moved.
	 *
	 * @param send_data the PersonDataType to fill
	 */
	virtual void fillSendData(PersonDataType& send_data);
};

template<typename PlaceType, typename PersonDataType>
AbstractPerson<PlaceType, PersonDataType>::AbstractPerson(unsigned int num_id) : id_{num_id} {
}

template<typename PlaceType, typename PersonDataType>
AbstractPerson<PlaceType, PersonDataType>::~AbstractPerson() {
}


template<typename PlaceType, typename PersonDataType>
void AbstractPerson<PlaceType, PersonDataType>::fillSendData(PersonDataType& data) {
	data.person_id = id_;
}

//std::ostream& operator<<(std::ostream& os, const Person& person);

} /* namespace chi_sim */

#endif /* SRC_PERSON_H_ */
