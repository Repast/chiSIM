/*
 * Activities.h
 *
 *  Created on: Jan 20, 2015
 *      Author: nick
 */

#ifndef SRC_ACTIVITIES_H_
#define SRC_ACTIVITIES_H_

#include <memory>
#include <vector>

#include "Activity.h"


namespace mrsa {

template<typename Act>
class Activities {

private:
	std::vector<std::shared_ptr<Act>> acts;
	unsigned int id_;

public:
	static std::shared_ptr<Act> NULL_ACT;

	Activities(unsigned int id);
	virtual ~Activities();

	void addActivity(const std::shared_ptr<Act>& act);
	std::shared_ptr<Act> findActivityAt(float time);

	unsigned int id() const {
		return id_;
	}
};

template<typename Act>
std::shared_ptr<Act> Activities<Act>::NULL_ACT = std::shared_ptr<Act>(nullptr);

template<typename Act>
Activities<Act>::Activities(unsigned int id) : id_(id) {

}

template<typename Act>
Activities<Act>::~Activities() {
}

template<typename Act>
void Activities<Act>::addActivity(const std::shared_ptr<Act>& act) {
	acts.push_back(act);
}

 template<typename Act>
 std::shared_ptr<Act> Activities<Act>::findActivityAt(float time) {
	for (auto& act : acts) {
		if (act->contains(time)) return act;
	}
	return NULL_ACT;
}

} /* namespace chi_sim */

#endif /* SRC_ACTIVITIES_H_ */
