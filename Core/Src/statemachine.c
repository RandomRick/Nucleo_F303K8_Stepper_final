/*
 * statemachine.c
 *
 *  Created on: 2 May 2021
 *      Author: rickh
 */


#include <string.h>		// strlen
#include <strings.h>	// strncasecmp
#include <stdlib.h>		// strtof, etc.
#include "statemachine.h"
#include "main.h"

volatile state_t State;

extern char rxBuf [RXBUFLEN];				// from main.c
extern unsigned int rxBufIndex;		// from main.c

const char mess[] = "\r\nDing Dong\r\n";

int ParseUartCommand()
{
	int		retval;
//	char	*endptr;

	// turn the rxBuf into a string that standard c commands can work with
	rxBuf[rxBufIndex]= (char)0;


	//
	// SpeedUs command - delay in microseconds between pulses
	//
	retval = FindIntParm(rxBuf, "SpeedUs", &State.PulseDelay);
	if (retval == STATE_OK)
	{
		return STATE_OK;
	}
	retval = FindIntParm(rxBuf, "Speed", &State.PulseDelay);
	if (retval == STATE_OK)
	{
		return STATE_OK;
	}
	retval = FindIntParm(rxBuf, "S", &State.PulseDelay);
	if (retval == STATE_OK)
	{
		return STATE_OK;
	}

	//
	// clockwise command
	//
	if (! strcasecmp (rxBuf, "CLOCKWISE"))
	{
		State.Direction = CLOCKWISE;
		return STATE_OK;
	}

	//
	// anticlockwise command
	//
	if (! strcasecmp (rxBuf, "ANTICLOCKWISE"))
	{
		State.Direction =ANTICLOCKWISE;
		return STATE_OK;
	}


	//
	// stop command
	//
	if (! strcasecmp (rxBuf, "STOP"))
	{
		// stop timer
		//HAL_TIM_Base_Stop_IT(&htim16);
		State.RunState = STOPPED;
		//HAL_Delay(5);	// not sure it's needed, but, delays, right?

		// set all Stepper coils to low (no current anywhere)
		HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);

		return STATE_OK;
	}

	if (! strcasecmp (rxBuf, "START"))
	{
		// start timer
//		HAL_TIM_Base_Start_IT(&htim16);	// start the 1ms timer
//		__asm volatile ("nop");

		//HAL_Delay(5);	// not sure it's needed, but, delays, right?

		State.RunState = RUNNING;

		return STATE_OK;
	}




	return STATE_ERROR;
}

//
// assumes command format: <search_string>:<integer value>
//
int FindIntParm (char * to_search,  const char *search_string, volatile int * retval)
{
	unsigned int  intval;
	char *endptr;

	if (strncasecmp (to_search, search_string, strlen (search_string)))
	{
		// not this command
		return STATE_COMMAND_NOT_FOUND;
	}

	// get past the colon, returning error if not found
	char *p = index (to_search, (int)':');


	if (p == (char *)0)
	{
		return STATE_ERROR_NO_COLON;
	}

	p++;

	intval = strtod (p, &endptr);
	if (endptr == p)
	{
		// no conversion
		return STATE_ERROR_NO_PARAMETER;
	}

	// valid result
	*retval = intval;

	return STATE_OK;
}

//
// StateMachineInit() - call once at startup
//
void StateMachineInit()
{
	memset ((void *)&State, 0, sizeof (State));

	State.Direction = CLOCKWISE;
	State.PulseDelay = 20; // ms

	// set all Stepper coils to low (no current anywhere)
	HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);

}





void SetState(int index)
{
  switch (index)
  {
  case 0:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_SET);
    break;
  case 1:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_SET);
    break;
  case 2:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
    break;
  case 3:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
    break;
  case 4:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
    break;
  case 5:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
    break;
  case 6:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
    break;
  case 7:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_SET);
    break;
  default:
    HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
    break;
  }
}






