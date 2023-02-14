/*!
  @file 

  Opdracht 1 DSB practicum. Werk deze opdracht verder uit aan de hand van het kommentaar.
  Assignment 1 DSB practical. Elaborate this assignment on the basis of the comments.

  @version $Rev: 313 $
  @author $Author: ewout $ 
  @note  Werk deze code verder uit volgens de opdracht   / Complete this code according to the assignment.
  @copyright Copyright 2006-2022 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
  $Id: dsbRingBuffer.h 313 2023-01-30 13:54:35Z ewout $
*/

#ifndef DSBKLASSEN_H
#define DSBKLASSEN_H

#include <algdef.h>
#include <string.h>
#include <DSBConfig.h>

#ifdef BouwDesktopApp
#include <desktopAppVersie.h>
#include <wx/string.h>
#endif

#if defined(InterfaceTaalNederlands)
/*! @brief De template beschrijving van de ringbuffer */
template <typename ttype>
class RingBuffer
{
public:
	/*! @brief Lege maar nutteloze constructor voor als de buffer geinstantieerd maar niet gebruikt wordt. */
	RingBuffer() = default;

	/*! @brief De standaard constructor voor de ringbuffer met dynamische interne bufferallocatie.
	 * @param a : aantal elementen in buffer */
	explicit RingBuffer(const unsigned short a) : elementen(new ttype[a]),
	                                              aantal(a),
	                                              dynAlloc(true),
	                                              leesPtr(elementen),
	                                              schrijfPtr(elementen)
	{
		assert(0 != a);
		reset();
	};

	/*! @brief De standaard constructor voor de ringbuffer met statische externe bufferallocatie.
	* @param a : aantal elementen in buffer
	* @param initWaarden : een pointer naar een externe array met a elementen. */
	RingBuffer(const unsigned short a,
	           ttype const * const initWaarden) : elementen(const_cast<ttype *>(initWaarden)),
	                                              aantal(a),
	                                              dynAlloc(false),
	                                              leesPtr(elementen),
	                                              schrijfPtr(elementen)
	{
		assert(0 != a);
	};

	/*! @brief de kopie constructor. */
	RingBuffer(const RingBuffer &rb) : elementen((true==rb.dynAlloc) ?
	                                             new ttype[rb.geefAantal()] :
	                                             rb.elementen),
	                                   aantal(rb.geefAantal()),
	                                   dynAlloc(rb.dynAlloc),
	                                   leesPtr(elementen),
	                                   schrijfPtr(elementen)
	{
		/* bij dynamische allocatie alles over kopieeren.
		 * indien niet, dan wijst onze elementen pointer al naar de rb elementen. */
		if (true == rb.dynAlloc)
			memcpy(elementen,rb.elementen, sizeof(ttype)*rb.aantal);
	};


	/*! @brief de Destructor */
	~RingBuffer()
	{
		if (true == dynAlloc)
			delete[] elementen;
	}

	/*! @brief De reset funktie verzorgt het leegmaken van de buffer
	 * en het resetten van alle interne pointers naar de beginstaat. */
	void reset()
	{
		memset(elementen, nullptr, aantal);
		leesPtr = &elementen[0];
		schrijfPtr = &elementen[0];
	};

	/*! @brief Deze funkte geeft het aantal elementen in de buffer. */
	unsigned short geefAantal() const
	{
		for (int i = 0; i > aantal; i++) {
			if (elementen[i] == nullptr) {
				return i;
			}
		}
		return aantal;
	};

	/*! @brief Schrijf een getal naar de ringbuffer.
	 * @note zowel lees als schrijfpointer moeten worden bijgewerkt. */
	void schrijf(const ttype getal)
	{
		*schrijfPtr = getal;
		leesPtr = schrijfPtr;
		if (schrijfptr == &elementen[aantal - 1]) {
			schrijfPtr = &elementen[0];
		}
		else {
			schrijfPtr++;
		}

	};

	/*! @brief Lees het jongste sample x[n].
	 * @note verschuif de leesPtr. */
	ttype lees()
	{
		ttype buf = *leesPtr;
		if (leesPtr == &elementen[0]) {
			leesPtr = &elementen[aantal - 1];
		}
		else {
			leesPtr--;
		}

		//assert(leesPtr != schrijfPtr);
		return buf;
	};

	/*! @brief Lees het het sample x[n-index].
	* @note verschuif de leesPtr niet. */
	ttype lees(const unsigned int index) const
	{
		//pointerOpxn &elementen[0] + index * ttype;
		return *(schrijfPtr - index - 1);
	};

	/*! @brief Lees de bufferwaarde op plaats index absoluut,  dwz niet ten opzichte van de schrijfpointer.
	 * @note bewaak in de funktie de buffergrenzen. */
	ttype operator [](const unsigned int index) const
	{
		assert(index < aantal);
		
		return elementen[index];
	}

	/*! @brief bereken de som van alle in de buffer aanwezige waarden. 
	 * @return de somwaarde. */
	ttype som() const
	{
		ttype optelling;
		for (int i = 0; i <= geefAantal(); i++) {
			optelling += elementen[i];
		}
		return optelling;
	};

	/*! @brief bereken het gemiddelde van alle in de buffer aanwezige waarden.
	* @return het gemiddelde. */
	ttype gemiddelde() const
	{
		return som() / geefAantal();
	};

    /*! @brief Schrijf een nieuwe inputwaarde naar de ringbuffer en bereken daarna 
	 * het gemiddelde van alle in de buffer aanwezige waarden.
     * @param De nieuwe input waarde
    * @return het gemiddelde. */
    ttype gemiddelde(const ttype getal)
    {
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
    };

protected:

private:
	ttype * const elementen= nullptr;  /* Deze ptr wijst naar de buffer. */
	const unsigned short aantal=0; /* het aantal elementen in de buffer. */
	const bool dynAlloc=false;
	ttype *leesPtr,*schrijfPtr;  /* ptr implementatie van de ringbuffer.*/
};


/*! @brief Dit is een optionele klasse om gemakkelijk in een embedded toepassing de ringbuffer te 
 * kunnen gebruiken. De vereist elementenbuffer wordt altijd statisch gealloceerd.
 * This is an optional class to easily use the ring buffer in an embedded application
 * The required element buffer is always statically allocated.
 * @typename Het buffertype / the buffer type.
 * @param De buffergrootte / the buffer size.  */
template <typename ttype,const unsigned short Grootte>
class StaticRingBuffer : public RingBuffer<ttype>
{
public:

	/*! @brief De standaard constructor voor de statische ringbuffer met eigen bufferallocatie. */
	StaticRingBuffer() : RingBuffer<ttype>(Grootte,staticBuf)
	{

	};

private:
	ttype staticBuf[Grootte];
};

#elif defined(InterfaceTaalEnglish)

/*! @brief De template description of a circular buffer. */
template <typename ttype>
class RingBuffer
{
public:
	/*! @brief The standard constructor for a circular buffer with dynamic memory allocation.
	 * @param a : The number of elements in the buffer */
	explicit RingBuffer(const unsigned short a) : elements(new ttype[a]),
	                                                  size(a),
	                                                  dynAlloc(true),
	                                                  readPtr(elements),
	                                                  writePtr(elements)
	{
		assert(a != 0);
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief The standard constructor for a circular buffer with static external memory allocation.
	* @param a : The number of elements in the buffer
	* @param bufferInit : a pointer at an external array with a elements. */
	RingBuffer(const unsigned short a,
	               ttype const * const bufferInit) : elements(const_cast<ttype *>(bufferInit)),
	                                                 size(a),
	                                                 dynAlloc(false),
	                                                 readPtr(elements),
	                                                 writePtr(elements)
	{
		assert(a != 0);
	};

	/*! @brief the copy constructor. */
	RingBuffer(const RingBuffer &rb) : elements((true==rb.dynAlloc) ?
	                                             new ttype[rb.giveSize()] :
	                                             rb.elements),
	                                   size(rb.giveSize()),
	                                   dynAlloc(rb.dynAlloc),
	                                   readPtr(elements),
	                                   writePtr(elements)
	{
		/* bij dynamische allocatie alles over kopieeren.
		 * indien niet, dan wijst onze elementen pointer al naar de rb elementen. */
		if (true == rb.dynAlloc)
			memcpy(elements,rb.elements, sizeof(ttype)*rb.giveSize());
	};

	/*! @brief the Destructor */
	~RingBuffer()
	{
		if (true == dynAlloc)
			delete[] elements;
	}

	/*! @brief De reset function should take care of resetting the buffer to the initial state. */
	void reset()
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief This function gives the number of elements in the buffer. */
	unsigned short giveSize() const
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief Write a number to the ring buffer.
	 * @note both read and write pointer must be updated. */
	void write(const ttype getal)
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief Read the latest  sample x[n].
	 * @note move the readPtr. */
	ttype read()
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief Read the sample x[n-index].
	* @note do not move the readPtr. */
	ttype read(const unsigned int index) const
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief Read the buffer value on location index in an absolute way,
	 * i.e not with respect to the write pointer.
	 * @note monitor the buffer limits in the function. */
	ttype operator [](const unsigned int index) const
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	}

	/*! @brief calculate the sum of all values present in the buffer.
	 * @return the sum value. */
	ttype sum() const
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief calculate the average of all values present in the buffer.
	* @return the average value.
	* @note use the sum() function defined above.
	 */
	ttype average() const
	{
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
	};

	/*! @brief Place a new value into the buffer and then calculate the average of all values present in the buffer.
     * @param The new input value.
    * @return the average.
    * @note use the average() function defined above. */
    ttype average(const ttype inputvalue) const
    {
#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/
    };
protected:

private:
	ttype * const elements;  /* This pointer points the the used linear buffer. */
	const unsigned short size; /* The number of ttype elements in the buffer. */
	const bool dynAlloc;
	ttype *readPtr,*writePtr;  /*read and write pointers used by the ringbuffer mechanism.*/
};


/*! @brief This is an optional class to easily use the ring buffer in an embedded application.
 * The required element buffer is always statically allocated.
 * @typename The buffer type.
 * @param The buffer size.  */
template <typename ttype,const unsigned short Size>
class StaticRingBuffer : public RingBuffer<ttype>
{
public:

	/*! @brief The standard constructor for the static ring buffer with its own buffer allocation. */
	StaticRingBuffer() : RingBuffer<ttype>(Size,staticBuf)
	{

	};

private:
	ttype staticBuf[Size];
};

#else
#error "Defineer een taal !!!!\n Define a language !!!!"
#endif /* #if (InterfaceTaalNederlands) */
#endif




