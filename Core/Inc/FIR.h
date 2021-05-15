/*
 * FIR.h
 *
 *  Created on: May 15, 2021
 *      Author: rickh
 */

#ifndef INC_FIR_H_
#define INC_FIR_H_

// Typedef

typedef float firFlt; // as in "FIR float"

typedef struct
{
	unsigned int    pFirLen;
	firFlt			pBandwidth;
	firFlt*			pFirTaps;
	firFlt*			pSignalBuf;
	firFlt*			p;
	firFlt*			p1;
	firFlt*			p2;
	firFlt			pAreaUnderCurve;
} FIR_t;


// Global FIR instance
extern FIR_t firInstance;

// Function Prototypes

// these 2 are passed a reference to an FIR_t instance, which needs de-referencing.
void FIR_init(FIR_t* thisone, unsigned int firLen, firFlt bandwidth);
firFlt FIR_doWork(FIR_t* thisone, firFlt x);

// these 2 work on a global instance of FIR_t called firInstance, so no referencing
void FIR_init_global(unsigned int firLen, firFlt bandwidth, firFlt signalHistory);
firFlt FIR_doWork_global(firFlt x);


#endif /* INC_FIR_H_ */
