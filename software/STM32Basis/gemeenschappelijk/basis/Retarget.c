/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Version: V1.0
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <rt_misc.h>
#include <stm32f10x.h>

#pragma import(__use_no_semihosting_swi)


extern int ser_putchar (int c);
extern int ser_getchar (void);

/* C++ name mangling mag niet de interrupt handler namen veranderen, 
 * deze staan in een assembly bestand zo genoemd.
 * Derhalve hieronder met C-Linking compileren.
 */
#ifdef __cplusplus
extern "C" {
#endif     

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

//int fputc (int ch, FILE *f) { return (ser_putchar(ch)); }
int fputc (int ch, FILE *f) 
{ 
	return (ITM_SendChar(ch)); 
}

int fgetc (FILE *f)         { return (ser_getchar()); }


int ferror(FILE *f) {
    /* Your implementation of ferror */
    return EOF;
}


void _ttywrch(int ch)       { ser_putchar(ch); }


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}

#ifdef __cplusplus
}
#endif
