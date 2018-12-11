#include "gtest/gtest.h"

#include "repast_hpc/RepastProcess.h"

int main(int argc, char **argv) {
	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator world;
	repast::RepastProcess::init("../test_data/config.props", &world);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
