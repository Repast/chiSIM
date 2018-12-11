/*
 * file_utils.h
 *
 *  Created on: Mar 3, 2016
 *      Author: nick
 */

#ifndef SRC_FILE_UTILS_H_
#define SRC_FILE_UTILS_H_

#include <string>

namespace chi_sim {

/**
 * Inserts the specified val in a file name before the extension,
 * returning the new filename.
 */
std::string insert_in_file_name(const std::string& file_name, int val);

/**
 * Creates a unique file name from the specified file_name. If a
 * file with the file_name path does not exist then file_name is returned
 */
std::string unique_file_name(const std::string& file_name);

}



#endif /* SRC_FILE_UTILS_H_ */
