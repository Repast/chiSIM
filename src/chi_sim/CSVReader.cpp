
/*
 * CSVReader.cpp
 *
 *  Created on: Apr 17, 2012
 *      Author: nick
 */

#include "CSVReader.h"

#include <boost/tokenizer.hpp>

namespace chi_sim {

using namespace std;
using namespace boost;

CSVReader::CSVReader(const string& file) : fname(file),  delim('\n'),
		in(file.c_str()) {
	// if we can't open the file throw exception
	if (!in.is_open()) throw invalid_argument("Error opening: " + file);
	findDelimeter();

}

CSVReader::CSVReader(const CSVReader& reader) : fname(reader.fname), delim(reader.delim), in(reader.fname.c_str()) {
	// if we can't open the file throw exception
	if (!in.is_open()) throw invalid_argument("Error opening: " + fname);
}

void CSVReader::findDelimeter() {

	char c;
	while (in.good()) {
	    c = in.get();
	    if (in.good()) {
	    	if (c == '\n') {
	    		delim = '\n';
	    		break;
	    	}
	    	if (c == '\r') {
	    		delim = '\r';
	    		break;
	    	}
	    }
	}
	in.close();
	in.open(fname.c_str());
}

void CSVReader::skip(int lines) {
	int count = 0;
	string str;
	while (count < lines && getline(in, str)) {
		++count;
	}
}

bool CSVReader::next(vector<string>& vec) {
	string line;
	// read the line
	if (getline(in, line, delim)) {
		// tokenize the line using boost's escaped list separator
		// which parses CSV format
		tokenizer<escaped_list_separator<char> > tok(line);
		// assign those values to the vector
		vec.assign(tok.begin(), tok.end());
		return true;
	}
	// return false when there are no more lines to
	// read
	return false;
}

// copy the file name and close the existing file
// and reopen it with the new file.
CSVReader& CSVReader::operator=(const CSVReader& rhs) {
	if (&rhs != this) {
		fname = rhs.fname;
		in.close();
		delim = rhs.delim;
		in.open(fname.c_str());
	}

	return *this;
}

CSVReader::~CSVReader() {
	in.close();
}

}

