/*
 * PersonCreator.h
 *
 *  Created on: Jan 5, 2015
 *      Author: nick
 */

#ifndef SRC_PERSONCREATOR_H_
#define SRC_PERSONCREATOR_H_

#include <map>
#include <vector>
#include <memory>

#include "repast_hpc/Utilities.h"
#include "repast_hpc/RepastProcess.h"

#include "../chi_sim/PlaceId.h"


#include "../chi_sim/CSVReader.h"

#include "mrsa_constants.h"
#include "Activities.h"
#include "Places.h"

namespace mrsa {

const int PERSON_ID_IDX = 0;
const int HH_ID_IDX = 2;
const int RELATE_IDX = 3;
const int SEX_IDX = 4;
const int AGE_IDX = 5;
const int GQ_ID_IDX = 6;
const int SCHOOL_ID_IDX = 7;
const int WORK_ID_IDX = 8;
const int TUCASE_ID_WEEKEND_IDX = 10;
const int TUCASE_ID_WEEKDAY_IDX = 9;
const int DAYCARE_ID_IDX = 11;
const int GYM_ID_IDX = 12;
const int HOSPITAL_ID_IDX = 13;
const int OTHER_H_START_IDX = 14;
const int OTHER_H_END_IDX = 17;

const int JAIL_ID_IDX = 24;
const int RANK_IDX = 29;

template<typename PlaceType>
chi_sim::PlaceIdPair<PlaceType> find_place(const std::string& id, std::map<int, std::shared_ptr<PlaceType>>& place_map) {
	if (id.length() > 0) {
		int num_id = std::stoi(id);
		auto iter = place_map.find(num_id);
		if (iter != place_map.end()) {
			return make_pair(iter->second->placeId(), iter->second);
		}
	}

	return make_pair(chi_sim::NULL_PLACE_ID, std::shared_ptr<PlaceType>(nullptr));
}

template<typename T, typename PlaceType>
class PersonCreator {

private:
	std::string person_file_;


public:
	using PlaceMap = std::map<int, std::shared_ptr<PlaceType>>;
	using ActMap = std::map<int, std::shared_ptr<Activities<Activity<PlaceType>>>>;

	PersonCreator(const std::string& person_file);
	virtual ~PersonCreator();

	template<typename Creator>
	void run(std::map<unsigned int, std::shared_ptr<T>>& persons, PlaceMap& place_map, ActMap& act_map,
			Creator creator);
};

template<typename T, typename PlaceType>
PersonCreator<T, PlaceType>::PersonCreator(const std::string& file) :
		person_file_(file) {
}

template<typename T, typename PlaceType>
PersonCreator<T, PlaceType>::~PersonCreator() {
}

template<typename T, typename PlaceType>
template<typename U>
void PersonCreator<T, PlaceType>::run(std::map<unsigned int, std::shared_ptr<T>>& persons, PlaceMap& place_map, ActMap& act_map,
		U helper) {
	chi_sim::CSVReader reader(person_file_);
	// skip the header
	reader.skip(1);

	std::vector<std::string> line;
	int my_rank = repast::RepastProcess::instance()->rank();
	int world_size = repast::RepastProcess::instance()->worldSize();

	// pause_for_debug();
	while (reader.next(line)) {
		int p_rank = std::stoi(line[RANK_IDX]);
		unsigned long id = stoul(line[PERSON_ID_IDX]);
		if (p_rank == my_rank || world_size == 1) {
			Places<PlaceType> places;

			// look up the Persons' places in the map.
			const std::string& hh_id = line[HH_ID_IDX];
			places.household = find_place(hh_id, place_map);

			const std::string& school_id = line[SCHOOL_ID_IDX];
			places.school = find_place(school_id, place_map);
			const std::string& gq_id = line[GQ_ID_IDX];
			places.group_quarters = find_place(gq_id, place_map);
			const std::string& work_id = line[WORK_ID_IDX];
			places.work = find_place(work_id, place_map);
			const std::string& gym_id = line[GYM_ID_IDX];
			places.gym = find_place(gym_id, place_map);
			const std::string& daycare_id = line[DAYCARE_ID_IDX];
			places.daycare = find_place(daycare_id, place_map);
			const std::string& hosp_id = line[HOSPITAL_ID_IDX];
			places.hospital = find_place(hosp_id, place_map);
			const std::string& jail_id = line[JAIL_ID_IDX];
			places.jail = find_place(jail_id, place_map);

			for (int i = OTHER_H_START_IDX; i <= OTHER_H_END_IDX; ++i) {
				const std::string& other_hh_id = line[i];
				places.other_households.push_back(find_place(other_hh_id, place_map));
			}

			if (places.school.second.get() != nullptr || places.household.second.get() != nullptr
					|| places.work.second.get() != nullptr || places.group_quarters.second.get() != nullptr) {

				auto week_day_iter = act_map.find(std::stoi(line[TUCASE_ID_WEEKDAY_IDX]));
				auto week_end_iter = act_map.find(std::stoi(line[TUCASE_ID_WEEKEND_IDX]));

				if (week_day_iter != act_map.end() && week_end_iter != act_map.end()) {

					persons.insert(
							make_pair(id, helper.createLocalPerson(id, places, week_day_iter->second, week_end_iter->second, line)));
				}
			}
		} else {
		  helper.processRemotePerson(id, p_rank);
		}
	}
}

} /* namespace chi_sim */

#endif /* SRC_PERSONCREATOR_H_ */
