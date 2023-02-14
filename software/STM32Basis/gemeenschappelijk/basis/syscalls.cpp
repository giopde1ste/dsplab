/***********************************************************************
Hardware
c bestand om Newlib Syscall te implementeren
Alle software in dit bestand is     
Copyright 2006 Martin Thomas
Copyright 2010-2014 Kiwanda Embedded Systemen                                               
$Id: syscalls.cpp 3129 2018-11-12 16:38:07Z ewout $
************************************************************************/

/***********************************************************************/
/*                                                                     */
/*  SYSCALLS.C:  System Calls for the newlib                           */
/*  most of this is from newlib-lpc and a Keil-demo                    */
/*                                                                     */
/*  These are "reentrant functions" as needed by                       */
/*  the WinARM-newlib-config, see newlib-manual.                       */
/*  Collected and modified by Martin Thomas                            */
/*                                                                     */
/***********************************************************************/


#include <stdlib.h>
#include <reent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <algdef.h>
#include <assert.h>
#include <algdef.h>

void * toewijzing(const size_t);


extern "C"
{
__extension__ typedef int __guard __attribute__((mode (__DI__)));

int __cxa_guard_acquire(__guard *);

void __cxa_guard_release (__guard *);
void __cxa_guard_abort (__guard *);

void __cxa_pure_virtual(void);

int __aeabi_atexit(void *obj, void (*dtr)(void *), void *dso_h);

}


int __cxa_guard_acquire(__guard *g)
{
	return (!*(char *)(g));
}

void __cxa_guard_release (__guard *g)
{
	*(char *)g = 1;

}

void __cxa_guard_abort (__guard *)
{


}



void __cxa_pure_virtual(void)
{


}

int __aeabi_atexit(void *obj, void (*dtr)(void *), void *dso_h)
{
	(void) obj;
	(void) dtr;
	(void) dso_h;
	return 0;
}

//void *__dso_handle = 0;



#ifndef NDEBUG

inline void __aeabi_uidivmod()
{

};

#endif

#ifdef __cplusplus
extern "C" {
#endif

#undef errno
int errno;


void _init()
{

}

void _exit(int status)
{
	while (1);
}

void einde(int status)
{
	while (1);
}


int* __errno()
{
	return((int *)0);
}
int fork(void)
{
	errno = EAGAIN;
	return -1;
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
	return(len);
}

_ssize_t _read_r(struct _reent *r,
                 int file,
                 void *ptr,
                 size_t len)
{

	return (_ssize_t)1;
}


// _ssize_t _write_r (struct _reent *r,
// 		       int file,
// 		       const void *ptr,
// 		       size_t len)
// {
// 	return (_write(file,(char *)ptr,len));
// }


int _close_r(struct _reent *r,
             int file)
{
	return 0;
}


_off_t _lseek_r(struct _reent *r,
                int file,
                _off_t ptr,
                int dir)
{
	return (_off_t)0;	/*  Always indicate we are at file beginning.  */
}

int fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR; return 0;
}

int _fstat_r(struct _reent *r,
             int file,
             struct stat *st)
{
	/*  Always set as character device.				*/
	st->st_mode = S_IFCHR;
	/* assigned to strong type with implicit 	*/
	/* signed/unsigned conversion.  Required by 	*/
	/* newlib.					*/

	return 0;
}

int ftruncate(int fildes, off_t length)
{
	return(0);
}

/***************************************************************************/

int _isatty_r(struct _reent *r, int fd)
{

	r = r;
	fd = fd;

	return 1;
}


int isatty(int file)
{
	(void)file; /* avoid warning */
	return 1;
}



void abort(void)
{
	while(1);
}

int getpid(void)
{
	return 1;
}


extern char _end; /* Defined by the linker */
static char *heap_end;

caddr_t _sbrk(int incr)
{
	char *prev_heap_end;
	if (heap_end == 0) {
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
#if 0
	if (heap_end + incr > get_stack_top()) {
		xprintf("Heap and stack collision\n");
		abort();
	}
#endif
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

//extern "C" void __cxa_pure_virtual() { while (1); }  /* afvangen van pure virtuele funkties */

/* "malloc clue function" from newlib-lpc/Keil-Demo/"generic" */

/**** Locally used variables. ****/
// mt: "cleaner": extern char* end;
// mt: "cleaner": extern char* end;
//#include "algdef.h"
//extern UInt8 __bss_end[];              /*  end is set in the linker command 	*/
/* file and is the end of statically 	*/
/* allocated data (thus start of heap).	*/

//static char *heap_ptr;		/* Points to current end of the heap.	*/
// BGET geheugenblok(end,
//                   5*1024);   /* 5 kilobytes */

/************************** _sbrk_r *************************************
 * Support function. Adjusts end of heap to provide more memory to
 * memory allocator. Simple and dumb with no sanity checks.

 *  struct _reent *r -- re-entrancy structure, used by newlib to
 *                      support multiple threads of operation.
 *  ptrdiff_t nbytes -- number of bytes to add.
 *                      Returns pointer to start of new heap area.
 *
 *  Note:  This implementation is not thread safe (despite taking a
 *         _reent structure as a parameter).
 *         Since _s_r is not used in the current implementation, 
 *         the following messages must be suppressed.
 */
// void * _sbrk_r(struct _reent *_s_r, 
//                ptrdiff_t nbytes)
// {

// //     if (nbytes == 0 )
// //         return((void *)Null);
// //     else 
// //         return(geheugenblok.bget(nbytes));
//     static UInt8 *heap_ptr = __bss_end;

//     UInt8 *base = heap_ptr;	/*  Point to end of heap.  */
//     heap_ptr += nbytes;	/*  Increase heap.  */

//     return base;		/*  Return pointer to start of new heap area.  */
// }

// const extern unsigned char __TOP_STACK[],__bss_end[];              /*  end is set in the linker command 	*/

// void * _sbrk_r(struct _reent *_s_r, 
// 	       ptrdiff_t size)
// {
//     static unsigned char *heap_ptr = (unsigned char *)(__bss_end);
//     const unsigned char * const eindeSRAM = (const unsigned char *)__TOP_STACK;	

//     Teller grootte = size;

//     while ((grootte %4) != 0)
//         grootte++;

//     if ((heap_ptr+grootte) < eindeSRAM)
//     {
//         const unsigned char *retWaarde = heap_ptr;	/* wijst naar het vrije stuk geheugen */
//         heap_ptr += grootte;	/*  Increase heap met size aantal bytes en eindig op 32 bit boundary */
//         return((void *)retWaarde);
//     }
//     else
//     {
//         StopHier();
//         return((void *)0);
//     }

// }



void *__dso_handle() __attribute__ ((weak));

void *__dso_handle()
{
	return((void *)NULL);
}


#ifdef __cplusplus
}
#endif
