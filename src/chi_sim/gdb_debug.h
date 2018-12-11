/*
 * gdb_debug.h
 *
 *  Created on: Jan 26, 2015
 *      Author: nick
 */

#ifndef SRC_GDB_DEBUG_H_
#define SRC_GDB_DEBUG_H_

/*
 * This code will output a line to stdout outputting the name of the host where the
 * process is running and the PID to attach to. It will then spin on the sleep()
 * function forever waiting for you to attach with a debugger. Using sleep() as
 * the inside of the loop means that the processor won't be pegged at 100% while
 * waiting for you to attach.

 Once you attach with a debugger, go up the function stack until you are in
 this block of code (you'll likely attach during the sleep())
 then set the variable i to a nonzero value. With GDB, the syntax is:

 (gdb) set var i = 7

 Then set a breakpoint after your block of code and continue execution until the
 breakpoint is hit. Now you have control of your live MPI application and use
 the full functionality of the debugger.
 */

void pause_for_attach();

#endif /* SRC_GDB_DEBUG_H_ */
