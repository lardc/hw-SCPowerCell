// -----------------------------------------
// Global definitions
// ----------------------------------------

// Header
#include "Constraints.h"

// Constants
//
const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE] =
{
  {0, PULSE_OFFSET_MAX, PULSE_OFFSET_DEFAULT},				// 0
  {0, REGULATOR_OFFSET_MAX, REGULATOR_OFFSET_DEFAULT},		// 1
  {0, INT16U_MAX, BATTERY_VOLTAGE_COEF_DEFAULT},			// 2
  {0, BATTERY_THRESHOLD_MAX, BATTERY_THRESHOLD_DEFAULT},	// 3
  {0, INT16U_MAX, SINE_PULSE_COEF_DEFAULT},					// 4
  {0, INT16U_MAX, TRAP_PULSE_COEF_DEFAULT},					// 5
  {0, INT16U_MAX, REG_SIN_P_DEFAULT},						// 6
  {0, INT16U_MAX, REG_SIN_I_DEFAULT},						// 7
  {0, INT16U_MAX, REG_TOP_TRAP_P_DEFAULT},      			// 8
  {0, INT16U_MAX, REG_TOP_TRAP_I_DEFAULT},					// 9
  {0, INT16U_MAX, REG_FRONT_TRAP_P_DEFAULT},				// 10
  {0, INT16U_MAX, REG_FRONT_TRAP_I_DEFAULT},				// 11
  {0, INT16U_MAX, 0},										// 12
  {0, 0, 0},												// 13
  {0, 0, 0},												// 14
  {0, 0, 0},												// 15
  {0, 0, 0},												// 16
  {0, 0, 0},												// 17
  {0, 0, 0},												// 18
  {0, 0, 0},												// 19
  {0, 0, 0},												// 20
  {0, 0, 0},												// 21
  {0, 0, 0},												// 22
  {0, 0, 0},												// 23
  {0, 0, 0},												// 24
  {0, 0, 0},												// 25
  {0, 0, 0},												// 26
  {0, 0, 0},												// 27
  {0, 0, 0},												// 28
  {0, 0, 0},												// 29
  {0, 0, 0},												// 30
  {0, 0, 0},												// 31
  {0, 0, 0},												// 32
  {0, 0, 0},												// 33
  {0, 0, 0},												// 34
  {0, 0, 0},												// 35
  {0, 0, 0},												// 36
  {0, 0, 0},												// 37
  {0, 0, 0},												// 38
  {0, 0, 0},												// 39
  {0, 0, 0},												// 40
  {0, 0, 0},												// 41
  {0, 0, 0},												// 42
  {0, 0, 0},												// 43
  {0, 0, 0},												// 44
  {0, 0, 0},												// 45
  {0, 0, 0},												// 46
  {0, 0, 0},												// 47
  {0, 0, 0},												// 48
  {0, 0, 0},												// 49
  {0, 0, 0},												// 50
  {0, 0, 0},												// 51
  {0, 0, 0},												// 52
  {0, 0, 0},												// 53
  {0, 0, 0},												// 54
  {0, 0, 0},												// 55
  {0, 0, 0},												// 56
  {0, 0, 0},												// 57
  {0, 0, 0},												// 58
  {0, 0, 0},												// 59
  {0, 0, 0},												// 60
  {0, 0, 0},												// 61
  {0, 0, 0},												// 62
  {0, 0, 0}                                  				// 63
};

const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START] =
{
  {0, SC_SINE_MAX_VALUE, 0},												// 64
  {0, INT16U_MAX, WAVEFORM_SINE},	        								// 65
  {TRAPEZE_EDGE_TIME_MIN, TRAPEZE_EDGE_TIME_MAX, TRAPEZE_EDGE_TIME_MAX},	// 66
  {MODE_TEST_REG_OFF, MODE_TEST_REG_ON, MODE_TEST_REG_OFF},					// 67
  {PULSE_DURATION_MIN, PULSE_TIME_VALUE_V20, PULSE_DURATION_MIN},			// 68
  {PULSE_COUNT_MIN, PULSE_COUNT_MAX, PULSE_COUNT_DEF},						// 69
  {PRE_PULSE_PAUSE_MIN, PRE_PULSE_PAUSE_MAX, PRE_PULSE_PAUSE_DEF},			// 70
  {0, 0, 0},																// 71
  {0, 0, 0},																// 72
  {0, 0, 0},																// 73
  {0, 0, 0},																// 74
  {0, 0, 0},																// 75
  {0, 0, 0},																// 76
  {0, 0, 0},																// 77
  {0, 0, 0},																// 78
  {0, 0, 0},																// 79
  {0, 0, 0},																// 80
  {0, 0, 0},																// 81
  {0, 0, 0},																// 82
  {0, 0, 0},																// 83
  {0, 0, 0},																// 84
  {0, 0, 0},																// 85
  {0, 0, 0},																// 86
  {0, 0, 0},																// 87
  {0, 0, 0},																// 88
  {0, 0, 0},																// 89
  {0, 0, 0},																// 90
  {0, 0, 0},																// 91
  {0, 0, 0},																// 92
  {0, 0, 0},																// 93
  {0, 0, 0},																// 94
  {SCPC_VERSION_V11, SCPC_VERSION_V20, SCPC_VERSION_V20},					// 95
};
// No more
