/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UTILS_PURE_DEFI_INCLUDED
/* { */
#define UTILS_PURE_DEFI_INCLUDED

#include <appl/diag/dpp/bits_bytes_macros.h>

/*
 * Definitions related to Thermometer chip (DS1721, I2C)
 * {
 */
#define THERMOMETER_MEZANINE_ARR_LOCATION     1
#define THERMOMETER_FABCRIC_ARR_LOCATION      2
#define THERMOMETER_LINECARD_1_ARR_LOCATION   THERMOMETER_FABCRIC_ARR_LOCATION
#define THERMOMETER_LINECARD_2_ARR_LOCATION   3
#define THERMOMETER_LINECARD_3_ARR_LOCATION   8
#define THERMOMETER_FRONTEND_3_ARR_LOCATION   4
#define THERMOMETER_DB_1_ARR_LOCATION         5
#define THERMOMETER_FRONTEND_4_ARR_LOCATION   6
#define THERMOMETER_FRONTEND_5_ARR_LOCATION   7

/*
 */
/*
 */
/*
 * GFA first thermometer.
 */
/*
 * GFA second thermometer.
 */
/*
 * GFA DB thermometer.
 */
/*
 * PTG/FTG first thermometer.
 */
/*
 * TEVB board thermometer.
 */
/*
 * }
 */

/*
 * Definitions related to Thermometer chip (DS1721, I2C)
 * {
 */

/*
 * TEVB FPGA thermometer.
 */

/*
* Soc_petra MAX1617A thermometer.
*/

/*
* SOC_SAND_FE600 thermometer.
*/

/*
 * }
 */

/*
 * Definitions related to OBJECT:
 *   Real Time Trace Utility
 * {
 */
/*
 * Maximal size of text descriptor of each event, including
 * terminating null.
 */
#define TRACE_EVENT_TEXT_SIZE        45
/*
 * Log-base-2 of maximal number of trace_event elements in the system. This
 * is the size of the trace fifo.
 * By default, set to 8
 */
#define LOG_NUM_TRACE_FIFO_ELEMENTS  8
/*
 * Maximal number of trace_event elements in the system. This
 * is the size of the trace fifo. Must be a power of 2.
 */
#define NUM_TRACE_FIFO_ELEMENTS      BIT(LOG_NUM_TRACE_FIFO_ELEMENTS)
/*
 * Mask of significant bits representing num of elements in fifo.
 */
/*
 * Number of unsigned long parameters that can be stored
 * for the formatting string in 'trace_event_text'
 */
# define NUM_TRACE_FIFO_PARAMETERS   4
typedef struct
{
    /*
     * Flag indicating whether this entry is valid.
     */
  unsigned int  valid ;
    /*
     * Time of this trace event. Time is from startup, in microseconds.
     */
  unsigned long time_microseconds ;
    /*
     * Internal identifier of event
     */
  int           trace_event_id ;
    /*
     * Null terminated text description of event.
     * This is a formatting string ('printf' style) into
     * which trace_params[i] are inserted.
     */
  char           trace_event_text[TRACE_EVENT_TEXT_SIZE] ;
  unsigned long  trace_params[NUM_TRACE_FIFO_PARAMETERS] ;
} TRACE_EVENT ;
typedef struct
{
    /*
     * Flag. Indicating fifo is locked and may not be loaded with
     * new data until unlocked.
     */
  unsigned int  fifo_locked ;
    /*
     * Flag. Indicating an attempt was made to load fifo while it
     * was locked. (Actually this is a counter of the number of
     * attempts. We assume 'unsigned int' will not overflow, itself)
     */
  unsigned int  load_while_locked ;
    /*
     * Flag. Indicating fifo is full and has overflown (some information
     * has been deleted).  (Actually this is a counter of the
     * number of overflow cases, We assume 'unsigned int' will not
     * overflow, itself)
     */
  unsigned int  overflow ;
    /*
     * Number of valid trace events in fifo.
     */
  unsigned int  num_elements ;
    /*
     * Index of NEXT element to 'put' into fifo.
     */
  unsigned int  put_index ;
    /*
     * Index of NEXT element to 'get' from fifo.
     */
  unsigned int  get_index ;
  TRACE_EVENT   trace_events[NUM_TRACE_FIFO_ELEMENTS] ;
} TRACE_FIFO ;
/*
 * }
 */
/* } */
#endif
