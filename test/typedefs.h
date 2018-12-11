/*
 * typedefs.h
 *
 *  Created on: Jul 17, 2017
 *      Author: nick
 */

#ifndef TEST_TYPEDEFS_H_
#define TEST_TYPEDEFS_H_

#include <memory>

#include "../src/mrsa/Activity.h"
#include "../src/mrsa/Activities.h"
#include "../src/chi_sim/AbstractModel.h"
#include "../src/chi_sim/AbstractPerson.h"

class TestPlace;
struct TestPersonData;

using ActT = mrsa::Activity<TestPlace>;
using BasePersonT = chi_sim::AbstractPerson<TestPlace, TestPersonData>;
using ActPtr = std::shared_ptr<ActT>;
using ActsPtr = std::shared_ptr<mrsa::Activities<ActT>>;
using AbsModelT = chi_sim::AbstractModel<TestPerson, TestPlace, TestPersonData>;




#endif /* TEST_TYPEDEFS_H_ */
