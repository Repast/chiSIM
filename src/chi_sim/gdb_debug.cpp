/*
 * gdb_debug.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: nick
 */

#include "unistd.h"
#include "stdio.h"

#include "gdb_debug.h"


void pause_for_attach() {

	    int i = 0;
	    char hostname[256];
	    gethostname(hostname, sizeof(hostname));
	    printf("PID %d on %s ready for attach\n", getpid(), hostname);
	    fflush(stdout);
	    while (0 == i)
	        sleep(5);

}
