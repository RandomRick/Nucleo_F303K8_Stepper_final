/*
 * statemachine.h
 *
 *  Created on: 2 May 2021
 *      Author: rickh
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

/* function prototypes */
int ParseUartCommand();
void StateMachineInit();
int FindIntParm (char * to_search,  const char *search_string, volatile int * retval);
void SetState (int);

/* macros */
#define STATE_OK 0
#define STATE_ERROR 1
#define STATE_ERROR_NO_COLON 2
#define STATE_ERROR_NO_PARAMETER 3
#define STATE_COMMAND_NOT_FOUND 4


typedef enum SpinDirection
{
	NONE=0,
	CLOCKWISE,
	ANTICLOCKWISE
} SpinDirection;

typedef enum RunStateTag
{
	STOPPED,
	RUNNING
} RunState_t;


typedef struct
{
	SpinDirection	Direction;
	int 			CurrentAngle;	/* 0 - 64*64 */
	int 			TargetAngle;
	int				PulseDelay;
	int				MostRecentPhase; /* 0-7 */
	RunState_t		RunState;
} state_t;

extern volatile state_t State;


#endif /* INC_STATEMACHINE_H_ */
