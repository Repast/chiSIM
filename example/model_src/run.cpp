/*
 * run.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: nick
 */
#include <exception>

#include "repast_hpc/io.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/initialize_random.h"

#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"

#include "Parameters.h"
#include "Model.h"


using namespace repast;
using namespace std;

const std::string STOP_AT = "stop.at";

void parse_parameters(repast::Properties& props, const std::string& parameters) {

	boost::char_separator<char> tab_sep("\t");
	boost::tokenizer<boost::char_separator<char> > tab_tok(parameters, tab_sep);

	for (auto item : tab_tok) {
		boost::trim(item);
		size_t pos = item.find_first_of("=");
		if (pos == std::string::npos) {
			throw invalid_argument("Invalid parameter: " + item);
		}

		string key(item.substr(0, pos));
		boost::trim(key);
		if (key.length() == 0) {
			throw invalid_argument("Invalid parameter: " + item);
		}

		string val(item.substr(pos + 1, item.length()));
		boost::trim(val);
		if (val.length() == 0) {
			throw invalid_argument("Invalid parameter: " + item);
		}
		props.putProperty(key, val);
		//std::cout << key << " = " << (val) << std::endl;
	}
}

/**
 * Runs the model using the specified configuration file and parameters.
 *
 * @param config the repast hpc configuration file
 * @param parameters a tab separated list of parameters where each parameter
 * is a key value pair separated by an "=".
 */
int run(MPI_Comm worker_comm, const std::string& config, const std::string& parameters) {
	char arg0[] = "main";
	char* argv[] = { &arg0[0], nullptr };
	int argc = (int) (sizeof(argv) / sizeof(argv[0])) - 1;
	// need the tmp in here because environment takes a reference
	char** tmp = &argv[0];
	boost::mpi::environment env(argc, tmp);

	// mrsa is model single process, so we use MPI_COMM_SELF here
	boost::mpi::communicator comm(worker_comm, boost::mpi::comm_attach);

	repast::Properties props;
	parse_parameters(props, parameters);

	// add a starting time property
	std::string time;
	repast::timestamp(time);
	props.putProperty("date_time.run", time);

	// add process count property
	props.putProperty("process.count", comm.size());
	repast::RepastProcess::init(config, &comm);

	try {
		if (comm.rank() == 0) {
			std::string time;
			repast::timestamp(time);
			std::cout << "Start Time: " << time << std::endl;
		}

		initializeRandom(props);
		chi_sim_model::Model model(props);

		if (comm.rank() == 0) {
			std::string time;
			repast::timestamp(time);
			std::cout << "Schedule Start Time: " << time << std::endl;
		}

		RepastProcess::instance()->getScheduleRunner().run();

		if (comm.rank() == 0) {
			std::string time;
			repast::timestamp(time);
			std::cout << "End Time: " << time << std::endl;
		}

	} catch (std::exception& exp) {
		// catch any exception (e.g. if data files couldn't be opened) and
		// print out the errors.
		std::cerr << "ERROR: " << exp.what() << std::endl;
	}
	RepastProcess::instance()->done();
	return 0;
}

