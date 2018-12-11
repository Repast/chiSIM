/*
*MRSA Model
*
*Copyright (c) 2013 University of Chicago and Argonne National Laboratory
*   All rights reserved.
*  
*   Redistribution and use in source and binary forms, with 
*   or without modification, are permitted provided that the following 
*   conditions are met:
*  
*  	 Redistributions of source code must retain the above copyright notice,
*  	 this list of conditions and the following disclaimer.
*  
*  	 Redistributions in binary form must reproduce the above copyright notice,
*  	 this list of conditions and the following disclaimer in the documentation
*  	 and/or other materials provided with the distribution.
*  
*  	 Neither the name of the Argonne National Laboratory nor the names of its
*     contributors may be used to endorse or promote products derived from
*     this software without specific prior written permission.
*  
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
*   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE TRUSTEES OR
*   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * FileOutput.h
 *
 *  Created on: Mar 19, 2013
 *      Author: nick
 */

#ifndef FILEOUTPUT_MRSA_H_
#define FILEOUTPUT_MRSA_H_

#include <fstream>
#include <string>

namespace chi_sim {

/**
 *
 */
class FileOutput {
public:

	std::ostream& operator<<(const std::string& val);
	std::ostream& operator<<(double val);
	std::ostream& operator<<(unsigned int val);
	std::ostream& operator<<(int val);

	virtual ~FileOutput();

	/**
	 * Initialize the singleton.
	 */
	static void initialize(const std::string& filename, int rank);

	/**
	 * Gets singleton instance. If it has not been initialized, an exception
	 * is thrown.
	 */
	static FileOutput* instance();

	/**
	 * Closes the recorder and writes any unwritten data to the file.
	 */
	void close();

private:
	FileOutput(const std::string& filename, int rank);
	static FileOutput* instance_;

	std::ofstream out;
	bool open;
};

} /* namespace mrsa */
#endif /* FILEOUTPUT_MRSA_H_ */
