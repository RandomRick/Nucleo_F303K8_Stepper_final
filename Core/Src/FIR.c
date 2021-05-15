/*
 * FIR.c
 *
 *  Created on: 15 May 2021
 *      Author: rickh
 */

// FIR_filter_C.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// There are 2 version of each function here.
// - one that is passed a pointer to an instance of the FIR_t structure
// - one that works directly on a global instance of the FIR_t structure, called firInstance.
//
// The former is flexible, but has to dereference the pointer lot
// The latter is less flexible, but there are no dereferencing operations, so might be quicker
//
// I don't know enough about compiler optimisation to know if having a version without
// dereferencing is quicker.
//


#include "main.h"
#include "FIR.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// global FIR instance
FIR_t firInstance;

void FIR_init(FIR_t* thisone, unsigned int firLen, firFlt bandwidth)
{

	thisone->pFirLen = firLen;
	thisone->pBandwidth = bandwidth;


	// reserve memory for the taps
	thisone->pFirTaps = (firFlt*)malloc(sizeof(firFlt) * thisone->pFirLen);
	thisone->pSignalBuf = (firFlt*)malloc(sizeof(firFlt) * thisone->pFirLen);

	if ((thisone->pFirTaps == 0) || (thisone->pSignalBuf == 0))
	{
		Error_Handler();
//		std::cerr << "malloc failed.  You're on your own." << std::endl;
		return;
	}

	// zero the signal buffer.  No need to zero the FIR taps - they're about to be computed.
	memset((void*)thisone->pSignalBuf, 0, sizeof(firFlt) * thisone->pFirLen);

	thisone->p = thisone->pSignalBuf;		// pointer increments with every new sample
	thisone->p1 = thisone->pSignalBuf - 1;  // used when computing the FIR - because we go backwards through the signal
	thisone->p2 = thisone->pSignalBuf + thisone->pFirLen;	// used when filling the signal buffer

	// compute the FIR taps

	thisone->pAreaUnderCurve = 0;
	for (unsigned int i = 0; i < thisone->pFirLen; i++)
	{
		thisone->pFirTaps[i] = thisone->pBandwidth * exp(-thisone->pBandwidth * i);
		thisone->pAreaUnderCurve += thisone->pFirTaps[i];

//		std::cout << "Tap # " << i << " = " << thisone->pFirTaps[i] << std::endl;

	}

//	std::cout << "Area under the curve = " << thisone->pAreaUnderCurve << std::endl;
}


firFlt FIR_doWork(FIR_t* thisone, firFlt x)
{
	// overwrite oldest element of circular buffer with newest
	*(thisone->p) = x;

	// perform FIR : *p goes backwards, FIR goes forwards.
	firFlt* ptemp = thisone->p;
	firFlt	total = (firFlt)0;

	for (unsigned int i = 0; i < thisone->pFirLen; i++)
	{
		total += thisone->pFirTaps[i] * (*ptemp);
		ptemp--;
		if (ptemp < thisone->pSignalBuf)
		{
			ptemp = thisone->pSignalBuf + thisone->pFirLen - 1;
			//std::cout << "wrap using pointer arith: p = " << ptemp << std::endl;
			//ptemp = &thisone->pSignalBuf[thisone->pFirLen - 1];
			//std::cout << "wrap using location of last element: p = " << ptemp << std::endl;
		}
	}

	// now update where the next element should go
	thisone->p++;
	if (thisone->p == thisone->p2)
		thisone->p = thisone->pSignalBuf;


	return total / thisone->pAreaUnderCurve;
}




void FIR_init_global(unsigned int firLen, firFlt bandwidth, firFlt signalHistory)
// this works directly on firInstance (type FIR_t), rather than dereferencing stuff all the time
{

	firInstance.pFirLen = firLen;
	firInstance.pBandwidth = bandwidth;


	// reserve memory for the taps
	firInstance.pFirTaps = (firFlt*)malloc(sizeof(firFlt) * firInstance.pFirLen);
	firInstance.pSignalBuf = (firFlt*)malloc(sizeof(firFlt) * firInstance.pFirLen);

	if ((firInstance.pFirTaps == 0) || (firInstance.pSignalBuf == 0))
	{
		Error_Handler();
//		std::cerr << "malloc failed.  You're on your own." << std::endl;
		return;
	}

	firInstance.p = firInstance.pSignalBuf;		// pointer increments with every new sample
	firInstance.p1 = firInstance.pSignalBuf - 1;  // used when computing the FIR - because we go backwards through the signal
	firInstance.p2 = firInstance.pSignalBuf + firInstance.pFirLen;	// used when filling the signal buffer

	// compute the FIR taps and fill the signalBuffer with a fake history.

	firInstance.pAreaUnderCurve = 0;
	for (unsigned int i = 0; i < firInstance.pFirLen; i++)
	{
		firInstance.pSignalBuf[i] = signalHistory;
		firInstance.pFirTaps[i] = firInstance.pBandwidth * exp(-firInstance.pBandwidth * i);
		firInstance.pAreaUnderCurve += firInstance.pFirTaps[i];
	}
}


firFlt FIR_doWork_global(firFlt x)
// this works directly on firInstance (type FIR_t), rather than dereferencing stuff all the time
{
	// overwrite oldest element of circular buffer with newest
	*(firInstance.p) = x;

	// perform FIR : *p goes backwards, FIR goes forwards.
	firFlt* ptemp = firInstance.p;
	firFlt	total = (firFlt)0;

	for (unsigned int i = 0; i < firInstance.pFirLen; i++)
	{
		total += firInstance.pFirTaps[i] * (*ptemp);
		ptemp--;
		if (ptemp < firInstance.pSignalBuf)
		{
			ptemp = firInstance.pSignalBuf + firInstance.pFirLen - 1;
			//std::cout << "wrap using pointer arith: p = " << ptemp << std::endl;
			//ptemp = &firInstance.pSignalBuf[firInstance.pFirLen - 1];
			//std::cout << "wrap using location of last element: p = " << ptemp << std::endl;
		}
	}

	// now update where the next element should go
	firInstance.p++;
	if (firInstance.p == firInstance.p2)
		firInstance.p = firInstance.pSignalBuf;


	return total / firInstance.pAreaUnderCurve;
}


