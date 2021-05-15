/*
 * joystick.h
 *
 *  Created on: May 9, 2021
 *      Author: rickh
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include "FIR.h"

extern float JoystickReadingFiltered();
void InterpretADC();
void JoystickInit(unsigned int nTaps, firFlt normBandwidth);




#endif /* INC_JOYSTICK_H_ */
