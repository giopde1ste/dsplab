/*
 * syscalls.c
 *
 *  Created on: 03.12.2009
 *      Author: Martin Thomas, 3BSD license
 */

#ifdef __cplusplus
 extern "C" {
#endif

#include <reent.h>
#include <errno.h>
#include <stdlib.h> /* abort */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <reent.h>
//#include "term_io.h"
#include <stm32f4xx.h> /* for _get_PSP() from core_cm0.h*/

#undef errno
extern int errno;

int _kill(int pid, int sig)
{
	(void)pid;
	(void)sig; /* avoid warnings */
	errno = EINVAL;
	return -1;
}


void _init()
{

}

void _exit(int status)
{
	//xprintf("_exit called with parameter %d\n", status);
	while(1) {;}
}

int _getpid(void)
{
	return 1;
}


extern char _end; /* Defined by the linker */
static char *heap_end;

char* get_heap_end(void)
{
	return (char*) heap_end;
}

char* get_stack_top(void)
{
	return (char*) __get_MSP();
	// return (char*) __get_PSP();
}

caddr_t _sbrk(int incr)
{
	char *prev_heap_end;
	if (heap_end == 0) {
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
#if 1
	if (heap_end + incr > get_stack_top()) {
//		xprintf("Heap and stack collision\n");
		abort();
	}
#endif
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

int _close(int file)
{
	(void)file; /* avoid warning */
	return -1;
}


void *__dso_handle() __attribute__ ((weak));

void *__dso_handle()
{
	return((void *)NULL);
}


int _fstat(int file, struct stat *st)
{
	(void)file; /* avoid warning */
	st->st_mode = S_IFCHR;
	return 0;
}

int _lseek(int file, int ptr, int dir) {
	(void)file; /* avoid warning */
	(void)ptr;  /* avoid warning */
	(void)dir;  /* avoid warning */
	return 0;
}

int _read(int file, char *ptr, int len)
{
	(void)file; /* avoid warning */
	(void)ptr;  /* avoid warning */
	(void)len;  /* avoid warning */
	return 0;
}

int _write(int file, char *ptr, int len)
{
	const int todo=0;
	(void)file; /* avoid warning */
//	for (todo = 0; todo < len; todo++) {
//		xputc(*ptr++);
//	}
	return todo;
}

/*
 * COPYRIGHT (c) 1989-2009.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
 
#if HAVE_CONFIG_H
#include "config.h"
#endif
 
/*
 * _isatty_r
 *
 * This is the Newlib dependent reentrant version of isatty().
 */
 

 
int _isatty_r(
struct _reent *ptr __attribute__((unused)),
int fd
)
{
return isatty( fd );
}

int _isatty(int file)
{
	(void)file; /* avoid warning */
	return 1;
}

#ifdef __cplusplus
}
#endif


/*
 * EABI builds can generate reams of stack unwind code for system generated exceptions
 * e.g. (divide-by-zero). Since we don't support exceptions we'll wrap out these
 * symbols and save a lot of flash space.
 */

extern "C" void __wrap___aeabi_unwind_cpp_pr0() {}
extern "C" void __wrap___aeabi_unwind_cpp_pr1() {}
extern "C" void __wrap___aeabi_unwind_cpp_pr2() {}


