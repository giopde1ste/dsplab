/*!
  @file 
  Algemene definities voor softwareontwikkeling
  @version $Rev: 3888 $
  @author $Author: ewout $ 
  @copyright Copyright 2006-2017 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen 
  $Id: algdef.h 3888 2020-05-28 16:10:21Z ewout $
************************************************************************/

#ifndef KIWANDA_ALGDEF_H
#define KIWANDA_ALGDEF_H

#include <stdio.h>
#include <stdint.h>

/* standaard definities */

#ifndef PI
static constexpr float PI=3.141592654f;
#endif

static constexpr auto Pi = PI;

#ifndef NULL
#define NULL            0
#endif

#ifndef Null
#define Null            NULL
#endif

#define forever         while(1==1)
#define Altijd          forever
#ifndef _WIN32

/*! @brief om aan te geven data deze funktie verouderd is en vervangen moet worden. */
#define VEROUDERD   __attribute__((deprecated))
/*! @brief voor het geforceerd linken van een ongebruikte funktie. */
#define GEBRUIKT   __attribute__((used))
/*! @brief voor test, dan geen waarschuwing (fout) bij het compileren. */
#define ONGEBRUIKT   __attribute__((unused))
#else
#define VEROUDERD   __declspec(deprecated)
#define GEBRUIKT    
#define ONGEBRUIKT  
#endif


/* Basic Types
 */

/* Sized Types
 */
typedef int8_t		Int8;
typedef uint8_t		UInt8;
typedef int16_t		Int16;
typedef uint16_t	UInt16;
typedef int32_t 	Int32;
typedef uint32_t	UInt32;
typedef int64_t		Int64;  /* wellicht niet nodig voor een 8 bit processor !*/
typedef uint64_t	UInt64; 

/* Bool Type  */
typedef UInt32			Bool32;	
typedef UInt16			Bool16;
typedef UInt8			Bool8;

/* algemene funktie foutmelding */

typedef enum
{
    Ok=0,
    Waarschuwing,
    Fout,
    Ernstig,
    Rampzalig,
    ReserveFoutCode
} FoutCode;

enum class RetCode
{
	Ok = 0,
	Waarschuwing,
	Fout,
	Ernstig,
	Rampzalig,
	Onbekend,
	ReserveFoutCode
};

typedef enum
{
    SchakelaarUit=0,
    SchakelaarAan,
    SchakelaarEinde
} Schakelaar;

typedef UInt32 Teller;          /* standaard breedte van de Intel 386 */ 
typedef Int32 STeller;          /* standaard breedte van de Intel 386 */ 

/* bitsgewijze operatie macros . Geef bits aan als nummer en niet als 2 tot de macht (nummer) */ 
#define SetBit(x,y)          x |= (1<<y) /* Set bit y in byte x*/ 
#define ClearBit(x,y)        x &= (~(1<<(y))) /* Clear bit y in byte x*/ 
//#define CheckBit(x,y)        (x & (1<<y))  /* Check bit y in byte x*/
inline bool CheckBit(const UInt32 x, const UInt32 y)
{
	const auto res = (x & (1<<y));
	return((res == 0) ? false : true);
}

#define EnterBit(x,y,b)      if (b)  SetBit(x,y); else  ClearBit(x,y);  /* zet bit y in byte x gelijk aan b */
#define CopyBit(a,b,x,y)     EnterBit((x),(y), (CheckBit(a,b)) ) /* kopieer bit b in byte a naar bit y in byte x */  //    (x =  (x&(~(1<<(y))))  | ((a & (1<<b) ) << (y-b) ) )  /* kopieer bit b in byte a naar bit y in byte x */
#define ToggleBit(x,y)       x = (x ^ (1<<y));  /* verander bit y in byte x */

/* bitsgewijze operatie macros . Geef bits als 2 tot de macht (nummer) */ 
/* Werkt in samenhang met Atmel AT91 bitdefinities */
#define SetBitM(var,bit)          var |= (bit) /* Set bit bit in byte var*/ 
#define ClearBitM(var,bit)        var &= (~(bit))  /* Clear bit y in bbitte var*/ 
#define CheckBitM(var,bit)        (var & (bit))  /* Check bit y in byte var*/
#define EnterBitM(var,bit,b)      if (b) SetBitM(var,bit) else ClearBitM(var,bit)  /* zet bit bit in byte var gelijk aan b */
#define CoybitBitM(src,srcbit,dest,destbit)     EnterBitM((dest),(destbit), (CheckBitM(src,srcbit)) ) /* kopieer bit b in byte a naar bit bit in byte var */  //    (var =  (var&(~(1<<(bit))))  | ((a & (1<<b) ) << (bit-b) ) )  /* kopieer bit b in bbitte a naar bit bit in byte var */
#define ToggleBitM(var,bit)       var = (var ^ (bit))  /* flip bit y in byte var */



    
/* Bit definities */
#define KBit0                0x01
#define KBit1                0x02
#define KBit2                0x04
#define KBit3                0x08
#define KBit4                0x10
#define KBit5                0x20
#define KBit6                0x40
#define KBit7                0x80


/* Serieele hulpjes */
#define HoogWoord(woord)    ((UInt16) 0xffff & (woord >> 16))  /* In een 32 bit geheugen, is dit het Most Significant Word (16 bit) */
#define LaagWoord(woord)    ((UInt16) 0xffff & woord)          /* In een 32 bit geheugen, is dit het Least Significant Word (16 bit) */
#define HoogByte(woord)    ((UInt8) 0xff & (woord >> 8))       /* In een 16 bit geheugen, is dit het Most Significant Byte (8 bit) */
#define LaagByte(woord)    ((UInt8) 0xff & woord)              /* In een 16 bit geheugen, is dit het Least Significant Byte (8 bit) */

#define Hoog              1
#define Laag              0
                              
#define ReturnIsOk(a)  ( ( (a) == Ok ) ? True : False )
#define ReturnIsNietOk(a)  ( ( (a) != Ok ) ? True : False )


/* @brief Bepaal het kleinere van twee getallen.
 * @param[in] a het eerste getal.
 * @param[in] b het tweede getal.
 * @return Het kleinste getal van de twee inputs. */
template <typename ttype>
ttype MIN(const ttype a, const ttype b)
{
    return((((a) < (b)) ? (a) : (b)));   
}

/* @brief Bepaal het grotere van twee getallen.
 * @param[in] a het eerste getal.
 * @param[in] b het tweede getal.
 * @return Het grootste getal van de twee inputs. */
template <typename ttype>
ttype MAX(const ttype a, const ttype b)
{
    return((((a) > (b)) ? (a) : (b)));  
}

#endif  /* ALGDEF_H */





