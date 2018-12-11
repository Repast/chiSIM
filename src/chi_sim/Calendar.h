/*
 * Calendar.h
 *
 *  Created on: Mar 6, 2013
 *      Author: nick
 */

#ifndef CALENDAR_H_
#define CALENDAR_H_

namespace chi_sim {

struct Calendar {

	unsigned int minute_of_day, hour_of_day, day_of_week, day_of_year, year;
	double tick;

	Calendar();
	// hourly increments
	void increment();
	bool isWeekDay();
};

} /* namespace chi_sim */
#endif /* CALENDAR_H_ */
