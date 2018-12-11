#include "mpi.h"

#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/io.h"

#include "../src/mrsa/mrsa_constants.h"

#include "Model.h"

using namespace repast;

void make_mpi_type(MPI_Datatype* dt) {
    int count = 6;
    MPI_Datatype types[count] = { MPI_UNSIGNED, MPI_INT, MPI_INT, MPI_UNSIGNED, MPI_UNSIGNED,
            MPI_INT };
    int block_lengths[count] = { 1, 1, 1, 1, 1, 24};
    MPI_Aint displacements[count];
    displacements[0] = offsetof(chi_sim_model::SamplePersonData, person_id);
    displacements[1] = offsetof(chi_sim_model::SamplePersonData, place_id);
    displacements[2] = offsetof(chi_sim_model::SamplePersonData, act_type);
    displacements[3] = offsetof(chi_sim_model::SamplePersonData, weekend_acts);
    displacements[4] = offsetof(chi_sim_model::SamplePersonData, weekday_acts);
    displacements[5] = offsetof(chi_sim_model::SamplePersonData, places);

    MPI_Datatype tmp_type;
    MPI_Type_create_struct(count, block_lengths, displacements, types, &tmp_type);

    MPI_Aint lb, extent;
    MPI_Type_get_extent(tmp_type, &lb, &extent);
    MPI_Type_create_resized(tmp_type, lb, extent, dt);
    MPI_Type_commit(dt);
}

void usage() {
	std::cerr << "usage: X  string string" << std::endl;
	std::cerr << "  first string: string is the path to the Repast HPC \n\tconfiguration properties file" << std::endl;
	std::cerr << "  second string: string is the path to the model properties file" << std::endl;
}

void runModel(std::string propsFile, int argc, char** argv) {
	boost::mpi::communicator comm;
	if (comm.rank() == 0) {
		std::string time;
		repast::timestamp(time);
		std::cout << "Start Time: " << time << std::endl;
	}

	Properties props(propsFile, argc, argv);
	initializeRandom(props);
	MPI_Datatype data_type;
	make_mpi_type(&data_type);
	chi_sim_model::Model model(props, data_type);

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
}

int main(int argc, char **argv) {
	boost::mpi::environment env(argc, argv);
	std::string config, props;

	boost::mpi::communicator world;

	// First check: if there aren't enough arguments, warn the user and exit
	if (argc < 3) {
		usage();
		return -1;
	}

	// Second: Assume the first arg is config file name and the second is prop file name
	config = argv[1];
	props = argv[2];

	if (config.size() > 0 && props.size() > 0) {
		try {
			RepastProcess::init(config, &world);
			runModel(props, argc, argv);

		} catch (std::exception& ex) {
			std::cerr << "Error while running the model: " << ex.what() << std::endl;
			throw ex;
		}
	} else {
		if (world.rank() == 0)
			usage();
	}

	RepastProcess::instance()->done();
	return 0;
}
