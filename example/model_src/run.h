/*
 * run.h
 *
 *  Created on: Dec 2, 2014
 *      Author: nick
 */

#ifndef SRC_RUN_H_
#define SRC_RUN_H_

#include <string>
#include <mpi.h>

/**
 * Runs the model using the specified configuration file and parameters.
 *
 * @param config the repast hpc configuration file
 * @param parameters a tab separated list of parameters where each parameter
 * is a key value pair separated by an "=".
 */
int run(MPI_Comm worker_comm, const std::string& config, const std::string& parameters);

#endif /* SRC_RUN_H_ */
