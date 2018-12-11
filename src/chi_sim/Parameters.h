

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "repast_hpc/Properties.h"

namespace chi_sim {

/**
 * Parameters singleton that contains the model properties.
 *
 */
class Parameters {
public:

	virtual ~Parameters();

	/**
	 * Initializes the Parameters singleton with the specified properties.
	 */
	static void initialize(repast::Properties& props);

	/**
	 * Gets singleton instance. If it has not been initialized, an exception
	 * is thrown.
	 */
	static Parameters* instance();

	/**
	 * Gets the named parameter as a string.
	 *
	 * @return the named parameter.
	 */
	std::string getStringParameter(const std::string& prop_name) const;

	/**
	 * Gets the named parameter as an int.
	 *
	 * @return the named parameter.
	 */
	int getIntParameter(const std::string& prop_name) const;

	/**
	 * Gets the named parameter as a double.
	 *
	 * @return the named parameter.
	 */
	double getDoubleParameter(const std::string& prop_name) const;

	/**
	 * Puts the named parameter into this Parameters object.
	 *
	 * @param key the parameter's name
	 * @param value the parameter's value.
	 */
	void putParameter(const std::string& key, bool value);

	/**
	 * Puts the named parameter into this Parameters object.
	 *
	 * @param key the parameter's name
	 * @param value the parameter's value.
	 */
	void putParameter(const std::string& key, const std::string& value);

	/**
	 * Puts the named double parameter into this Parameters object.
	 *
	 * @param key the parameter's name
	 * @param value the parameter's value.
	 */
	void putParameter(const std::string& key, double value);

	/**
	 * Gets the named parameter as a boolean.
	 *
	 * @return the named parameter.
	 */
	bool getBooleanParameter(const std::string& prop_name) const;

	/**
	 * Gets whether or not the specified parameters is in this Parameters object.
	 */
	bool contains(const std::string& key) const;

private:
	static Parameters* instance_;
	repast::Properties props_;

	Parameters(repast::Properties& props);
};

} /* namespace mrsa */
#endif /* PARAMETERS_H_ */
