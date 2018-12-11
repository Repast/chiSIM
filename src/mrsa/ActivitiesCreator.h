/*
 * ActivitiesCreator.h
 *
 *  Created on: Jan 6, 2015
 *      Author: nick
 */

#ifndef SRC_DEFAULTACTIVITIESCREATOR_H_
#define SRC_DEFAULTACTIVITIESCREATOR_H_

#include <map>
#include <memory>

#include "../chi_sim/CSVReader.h"

#include "Activities.h"
#include "Activity.h"

namespace mrsa {

template<typename PlaceType>
class ActivitiesCreator {

private:
	std::string act_file_;
public:
	using ActMap = std::map<int, std::shared_ptr<Activities<Activity<PlaceType>>>>;
	ActivitiesCreator(const std::string& act_file);
	virtual ~ActivitiesCreator();

	void run(ActMap& map);
};


template<typename PlaceType>
ActivitiesCreator<PlaceType>::ActivitiesCreator(const std::string& act_file) : act_file_(act_file) {

}

template<typename PlaceType>
ActivitiesCreator<PlaceType>::~ActivitiesCreator() {
}

template<typename PlaceType>
void ActivitiesCreator<PlaceType>::run(ActMap& map) {
	chi_sim::CSVReader reader(act_file_);
	reader.skip(1);
	std::vector<std::string> line;

	while (reader.next(line)) {
		std::shared_ptr<Activity<PlaceType>> act = std::make_shared<Activity<PlaceType>>(line);
		auto iter = map.find(act->scheduleId());
		if (iter == map.end()) {
			std::shared_ptr<Activities<Activity<PlaceType>>> acts= std::make_shared<Activities<Activity<PlaceType>>>(act->scheduleId());
			acts->addActivity(act);
			map.insert(std::make_pair(act->scheduleId(), acts));
		} else {
			iter->second->addActivity(act);
		}
	}
}

} /* namespace chi_sim */

#endif /* SRC_ACTIVITIESCREATOR_H_ */
