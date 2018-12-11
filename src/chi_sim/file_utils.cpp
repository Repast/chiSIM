/*
 * file_utils.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: nick
 */

#include <sstream>

#include "boost/filesystem.hpp"

#include "file_utils.h"

namespace fs = boost::filesystem;

namespace chi_sim {

std::string insert_in_file_name(const std::string& fname, int val) {
	fs::path filepath(fname);
	std::string stem = filepath.stem().string();
	std::stringstream ss;
	ss << stem << "_" << val << filepath.extension().string();
	fs::path newName(filepath.parent_path() / ss.str());
	return newName.string();
}


std::string unique_file_name(const std::string& fname) {

	fs::path filepath(fname);
	int i = 1;
	std::string stem = filepath.stem().string();
	while (fs::exists(filepath)) {    // This will increment i until it hits a unique name
		i++;
		std::stringstream ss;
		ss << stem << "_" << i << filepath.extension().string();
		fs::path newName(filepath.parent_path() / ss.str());
		filepath = newName;
	}
	return filepath.string();
}

}
