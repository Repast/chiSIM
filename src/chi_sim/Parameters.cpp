/*
 * Parameters.cpp
 *
 *  Created on: Jun 5, 2012
 *      Author: nick
 */

#include "Parameters.h"
#include "repast_hpc/Utilities.h"

namespace chi_sim {

using namespace repast;

Parameters* Parameters::instance_ = 0;

Parameters::Parameters(repast::Properties& props) :
		props_(props)  {
}

Parameters::~Parameters() {

}

void Parameters::initialize(Properties& props) {
	if (instance_ != 0) {
		delete instance_;
	}
	instance_ = new Parameters(props);
}

Parameters* Parameters::instance() {
	if (instance_ == 0)
		throw std::domain_error("Parameters must be initialized before instance() is called");

	return instance_;
}

std::string Parameters::getStringParameter(const std::string& prop_name) const {
	std::string val = props_.getProperty(prop_name);
	if (val.length() == 0)
		throw std::invalid_argument(
				"Invalid property name '" + prop_name + "', no property found.");
	return val;
}

int Parameters::getIntParameter(const std::string& prop_name) const {
	return strToInt(getStringParameter(prop_name));
}

double Parameters::getDoubleParameter(const std::string& prop_name) const {
	return strToDouble(getStringParameter(prop_name));
}

void Parameters::putParameter(const std::string& key, bool value) {
	std::string val = value ? "true" : "false";
	props_.putProperty(key, val);
}

void Parameters::putParameter(const std::string& key, const std::string& value) {
	props_.putProperty(key, value);
}

void Parameters::putParameter(const std::string& key, double value) {
	props_.putProperty(key, value);
}

bool Parameters::getBooleanParameter(const std::string& prop_name) const {
	std::string val = getStringParameter(prop_name);
	return val == "true" || val == "TRUE";
}

bool Parameters::contains(const std::string& key) const {
    return props_.contains(key);
}

} /* namespace mrsa */
