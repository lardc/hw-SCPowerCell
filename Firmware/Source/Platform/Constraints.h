// -----------------------------------------
// Global definitions
// ----------------------------------------

#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
//
#include "DataTable.h"
#include "Global.h"

#define PULSE_DURATION_MIN 				10000  // в мкс

#define PULSE_COUNT_MIN					1
#define PULSE_COUNT_MAX					3
#define PULSE_COUNT_DEF					PULSE_COUNT_MIN

#define PRE_PULSE_PAUSE_MIN				9
// Максимальная длительность паузы и импульса — 20мс
#define PRE_PULSE_PAUSE_MAX				(PRE_PULSE_PAUSE_MIN + 2 * (PULSE_COUNT_MAX - 1) * 20)
#define PRE_PULSE_PAUSE_DEF				PRE_PULSE_PAUSE_MIN

// Types
//
typedef struct __TableItemConstraint
{
	Int16U Min;
	Int16U Max;
	Int16U Default;
} TableItemConstraint;


// Variables
//
extern const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE];
extern const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START];


#endif // __CONSTRAINTS_H
