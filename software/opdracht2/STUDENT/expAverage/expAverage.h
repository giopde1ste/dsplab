/*!
@file
Exponentieel Gemiddelde (Exponential Average) API voor Opdracht 2 / Assignment 2.

@version $Rev: 313 $
@author $Author: ewout $
@copyright Copyright 2006-2019 ir drs E.J Boks Hogeschool van Arnhem en Nijmegen
$Id: expAverage.h 313 2023-01-30 13:54:35Z ewout $
*/

#ifndef ExpGemiddelde_H
#define ExpGemiddelde_H

class ExponentialAverageFilter
{
public:
	/*! @brief Constructor for the exponential averaging filter.
	 * @param The Weighing factor alfa (0..1,0)
	 */
	explicit ExponentialAverageFilter(const float );


	/*! @brief perform an averaging action on the input
	 * @param The input sample.
	 * @return the exp average value. */
	float filter(const float);

private:



#error “Dit deel van de software ontbreekt — this part of the software is missing.”
/* Beste leerling, dit deel van de software ontbreekt. Vul dit deel aan volgens de opdracht.  
   Dear student, this part of the software is missing. Complete this part accoording to the assignment.
*/

	/* The weighing factor and its counterpart (1-alfa)  */
	const float alfa;
	const float minalfa;
};


#endif // ExpGemiddelde_H


