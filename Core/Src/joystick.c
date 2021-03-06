/*
 * joystick.c
 *
 *  Created on: May 9, 2021
 *      Author: rickh
 */
#include "main.h"
#include "joystick.h"
#include "statemachine.h"
#include <stdlib.h> // malloc
#include <string.h> // memset
#include <math.h>	// expf()
#include <stdio.h>	// sprintf()

// Globals
unsigned long deadZoneTick = 0;		// after joystick is a set time in the central posn,
									// deactivate the stepper coils
unsigned long activeZoneTick= 0;	// the converse of the above.  Used to limit speed of
									// the stepper for a little while before ramping up.



extern ADC_HandleTypeDef hadc1;			// fron main.c


/*
 *
 2ms                    50ms inf  50ms                     2ms
      anticlockwise                    clockwise
 <--------------------------MIDDLE-------------------------->
 fast                  slow  stop slow                  fast

 */


typedef struct {
	unsigned int adcLeft;
	unsigned int adcRight;
	unsigned int adcMiddle;
	unsigned int deadZone;
} joystickCharacteristics_t;

const joystickCharacteristics_t joystickcharacteristics = {0, 4055, 1970, 100};

//#define FIRSIZ 5
#define MOTORSLOW 100
#define MOTORFAST 4

//static const float BandWidth = 1.0;
//static float FirTaps[FIRSIZ];
//static float SignalBuf[FIRSIZ];
//static float areaUnderTheCurve = 0;
//static float *p;
//static float*p2;

float joystickAdcFiltered;
int joystickAdcRaw;


float JoystickReadingFiltered()
{
	// get ADC
	HAL_ADC_Start(&hadc1);					// not blocking
	HAL_ADC_PollForConversion(&hadc1, 10);	// wait for conversion to complete

	joystickAdcRaw = (int) HAL_ADC_GetValue(&hadc1);

	// clip the value - the joystickcharacteristics are not set in stone.
	if (joystickAdcRaw > joystickcharacteristics.adcRight) joystickAdcRaw = joystickcharacteristics.adcRight;

	// ne version - use the dedicated FIR

	joystickAdcFiltered = FIR_doWork_global((firFlt)joystickAdcRaw);
	return joystickAdcFiltered;


#ifdef OLDVERSION
	//debug
//	joystickAdcRaw = joystickcharacteristics.adcMiddle + joystickcharacteristics.deadZone + 2;	// test-case 1
//	joystickAdcRaw = joystickcharacteristics.adcRight;											// test-case 2
//	joystickAdcRaw = joystickcharacteristics.adcMiddle - joystickcharacteristics.deadZone - 2;	// test-case 3
//	joystickAdcRaw = joystickcharacteristics.adcLeft;											// test-case 4


	// has the FIR been set up?
	// if (! SignalBuf)
	// 	JoystickInit();

	// add the latest value to the signal buffer
	*p = joystickAdcRaw;
	p++;
	if (p == p2) p = SignalBuf;

	// compute the FIR result
	joystickAdcFiltered = 0;
	float *ptemp = p;
	for (int t=0; t < FIRSIZ; t++)
	{
		joystickAdcFiltered += FirTaps[t] * (*ptemp);
		ptemp++;
		if (ptemp == p2) ptemp = SignalBuf;
	}
	joystickAdcFiltered = joystickAdcFiltered  / areaUnderTheCurve;

	return joystickAdcFiltered;
#endif
}




void JoystickInit(unsigned int nTaps, firFlt normBandwidth)
{
	// reserve space for FIR taps and signal buffer
	// FirTaps     = malloc (sizeof(float) * FIRSIZ);
	// SignalBuf   = malloc (sizeof(float) * FIRSIZ);
	// memset (SignalBuf, 0, sizeof(float) * FIRSIZ);

	// new version!  Use the FIR.c, FIR.h stuff.
	FIR_init_global(nTaps /* num taps */, normBandwidth /* normalised BW */, joystickcharacteristics.adcMiddle);
	return;

//	// Create a starting history of adc readings that correspond to the middle posn
//	for (int i=0; i < FIRSIZ; i++)
//		SignalBuf[i] = joystickcharacteristics.adcMiddle;
//
//
//	p = SignalBuf;				// points to oldest element in buffer
//	p2 = SignalBuf + FIRSIZ;	// points to 1 beyond the buffer. For wrapping
//
//	// Compute FIR taps
//	for (int t=0; t < FIRSIZ; t++)
//	{
////		float tempval = (float)1 - expf (-BandWidth * (t+1));
//		float tempval = (float)1 - expf (-BandWidth * (t+1))  * (1 + (t+1) * BandWidth);
//
//		FirTaps[t] = tempval;
//		areaUnderTheCurve += tempval;
//	}
}

void InterpretADC()
{
	int distanceFromMiddle = (float)joystickAdcFiltered - joystickcharacteristics.adcMiddle;
	int distanceFromMiddleUnfiltered = (float)joystickAdcRaw - joystickcharacteristics.adcMiddle;

	// determine direction
	if (distanceFromMiddle < 0)
	{
		// joystick is left
		State.Direction = ANTICLOCKWISE;
		int distFromDeadZone = -distanceFromMiddle - joystickcharacteristics.deadZone;
		const int zoneSpanExcludingDeadZone = joystickcharacteristics.adcMiddle - joystickcharacteristics.deadZone - joystickcharacteristics.adcLeft;

		State.PulseDelay = MOTORSLOW - (MOTORSLOW-MOTORFAST) *
				distFromDeadZone / zoneSpanExcludingDeadZone;

		snprintf (txBuf, TXBUFLEN, "LEFT  ADC= %d FILT= %d DIST= %d Speed= %d DEADTICK=%ld\r\n", joystickAdcRaw, (int)joystickAdcFiltered, distanceFromMiddle, State.PulseDelay, deadZoneTick);

	}
	else
	{
		// joystick is right
		State.Direction = CLOCKWISE;
		int distFromDeadZone = distanceFromMiddle - joystickcharacteristics.deadZone + 1;
		const int zoneSpanExcludingDeadZone = joystickcharacteristics.adcRight - (joystickcharacteristics.adcMiddle + joystickcharacteristics.deadZone);

		State.PulseDelay = MOTORSLOW - (MOTORSLOW-MOTORFAST) *
				distFromDeadZone / zoneSpanExcludingDeadZone;

//		snprintf (txBuf, TXBUFLEN, "RIGHT ADC= %d FILT=%d Speed= %d\r\n", joystickAdcRaw, (int)joystickAdcFiltered, State.PulseDelay);
		snprintf (txBuf, TXBUFLEN, "RIGHT ADC= %d FILT= %d DIST= %d Speed= %d DEADTICK=%ld\r\n", joystickAdcRaw, (int)joystickAdcFiltered, distanceFromMiddle, State.PulseDelay, deadZoneTick);
	}
	//HAL_UART_Transmit(&huart2, (uint8_t*)txBuf, strlen (txBuf), HAL_MAX_DELAY);

	// if we're in the central dead zone, force the speed to be 0 - i.e. infinite pulse delay
	if (abs(distanceFromMiddle) <= joystickcharacteristics.deadZone)
	{
		State.PulseDelay = 0x7FFFFFFF;	// 24 days. Not quite infinity.
		deadZoneTick ++;				// keep track how long in dead-zone
		activeZoneTick = 0;


		// Dead-zone deactivate coils after 2.5 seconds...
		if (deadZoneTick == 50 /* 2.5s */ ) // these ticks are every 50ms as per 9 lines up
		{
			State.RunState = STOPPED;
			//HAL_Delay(5);	// not sure it's needed, but, delays, right?

			// set all Stepper coils to low (no current anywhere)
			HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
		}
	}
	else // not dead-zone, so active zone
	{
		// if we've been in the active zone for less that 2.5 seconds, go at dead slow.
		// but only if we've been in the deadzone up to now for quite a while - don't want
		// the creep mode kicking in when accidentally passing quickly through the dead zone,
		// when flicking the joystick left and right.
		if (activeZoneTick < 50 && deadZoneTick > 10 /* 0.5 seconds */ )
		{
			State.PulseDelay = MOTORSLOW * 4;
		}
		else
		{
			deadZoneTick = 0; // continually reset the dead-zone timer to zero if not in the dead-zone
		}

		activeZoneTick++;


		State.RunState = RUNNING;
	}
}

