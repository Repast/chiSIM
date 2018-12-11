/*
 * Calendar.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: nick
 */

#include "Calendar.h"

namespace chi_sim {

Calendar::Calendar() :
		minute_of_day(0), hour_of_day(0), day_of_week(0), day_of_year(1), year(1), tick{0} {
}

void Calendar::increment() {
	++hour_of_day;
	minute_of_day += 60;
	if (hour_of_day > 23) {
		hour_of_day = 0;
		minute_of_day = 0;
		++day_of_week;
		if (day_of_week > 6) {
			day_of_week = 0;
		}
		++day_of_year;
		if (day_of_year > 365) {
			day_of_year = 1;
			++year;
		}
	}
}

bool Calendar::isWeekDay() {
	return !(day_of_week == 0 || day_of_week == 6);
}

} /* namespace chi_sim */
