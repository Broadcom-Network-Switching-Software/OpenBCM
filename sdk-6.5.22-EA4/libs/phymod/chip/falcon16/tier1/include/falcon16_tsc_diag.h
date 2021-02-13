/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/***********************************************************************************
 ***********************************************************************************
 *                                                                                 *
 *  Revision    :        *
 *                                                                                 *
 *  Description :  Diagnostic functions provided to IP User                        *
 */

/** @file falcon16_tsc_diag.h
 * Diagnostic functions provided to IP User
 */

#ifndef FALCON16_TSC_API_DIAG_H
#define FALCON16_TSC_API_DIAG_H

#include "falcon16_tsc_internal.h"
#include "falcon16_tsc_ipconfig.h"
#include "common/srds_api_enum.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_uc_common.h"
#include "falcon16_tsc_dependencies.h"
#include "falcon16_tsc_usr_includes.h"

#ifndef DIAG_VERBOSE
#define DIAG_VERBOSE (0)
#endif

#define DB_STOPPED       0x40

#define EYE_SCAN_NRZ_VERTICAL_IDX_MAX   (62)
#define EYE_SCAN_NRZ_VERTICAL_STEP       (2)

/** Eyescan Options Struct */
struct falcon16_tsc_eyescan_options_st {
   uint32_t linerate_in_khz;
   uint16_t timeout_in_milliseconds;
   int8_t horz_max;
   int8_t horz_min;
   int8_t hstep;
   int8_t vert_max;
   int8_t vert_min;
   int8_t vstep;
   int8_t mode;
};


/*----------------------------------------*/
/*  Display Core Config and Debug Status  */
/*----------------------------------------*/
/** Display Core configurations (RAM config variables and config register fields).
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_core_config(srds_access_t *sa__);

/** Display current Core state. Read and displays core status variables and fields.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_core_state(srds_access_t *sa__);

/** Column definition header for falcon16_tsc_display_core_state() API output.
 * To be called before falcon16_tsc_display_core_state_line() API.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_core_state_hdr(void);

/** Display current Core state in single line. Read and displays core status variables and fields.
 * Call falcon16_tsc_display_core_state_hdr() API before and falcon16_tsc_display_core_state_legend() after
 * calling this API to get a formatted core state display with legend.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_core_state_line(srds_access_t *sa__);

/** Detailed description of each column in falcon16_tsc_display_core_state_line() API output.
 * To be called after falcon16_tsc_display_core_state_line() API to display the legends.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_core_state_legend(void);

/*-----------------------*/
/*  Temperature forcing  */
/*-----------------------*/

/** Forces die temperature in degrees Ceisius (as integer).
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param die_temp  Die temperature in degrees Celsius. (-255 will disable a previously forced value)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_force_die_temperature (srds_access_t *sa__, int16_t die_temp);

/*-----------------------------------------------*/
/*  Envelope functions requested by Switch team  */
/*-----------------------------------------------*/

/** Display current Core and Lane state.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_state (srds_access_t *sa__);

/** Display current Core and Lane config.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_config (srds_access_t *sa__);

/**************************************************/
/* LANE Based APIs - Required to be used per Lane */
/**************************************************/

/** Function uses timestamps in uCode to check if heartbeat timer is programmed correctly 
 * for the COMCLK frequency it is running at *
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code if timestamp check fails (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_test_uc_timestamp(srds_access_t *sa__);

/*-------------------*/
/* Display Eye Scan  */
/*-------------------*/

/** Displays Passive Eye Scan from -0.5 UI to 0.5UI to BER 1e-7.
 *  Function uses uC to acquire data.
 *  It also retrieves the data and displays it in ASCII-art style, where number N corresponds to 1e-N
 *
 * This function retrieves the data from uC in horizontal stripe fashion
 *
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_eye_scan(srds_access_t *sa__);

/** Start uC controller eye scan Function.
 * Eye scan function provides a stripe of data at a time either vertical or horizontal.
 * This function only initiates the processor actions.  User must use falcon16_tsc_read_eye_scan_stripe() function
 * to get the data from uC.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param direction specifies either EYE_SCAN_VERTICAL or EYE_SCAN_HORIZ striping
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_meas_eye_scan_start(srds_access_t *sa__, uint8_t direction);

/** Read a Stripe of eye scan data from uC.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *buffer must be of size 64
 * @param *status returns a status word                 \n
 *    bit 15 - indicates the ey scan is complete        \n
 *    bit 14 - indicates uC is slower than read access  \n
 *    bit 13 - indicates uC is faster than read access  \n
 *    bit 12-8 - reserved                               \n
 *    bit 7-0 - indicates amount of data in the uC buffer
 *
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_read_eye_scan_stripe(srds_access_t *sa__, uint32_t *buffer, uint16_t *status);

/** Display Stripe of eye scan data to stdout and log.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param y is the vertical step 124 to -124(Falcon16)
 * @param *buffer must be of size 64
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_eye_scan_stripe(srds_access_t *sa__, int8_t y, uint32_t *buffer);

/** Display Eye scan header to stdout and log.
 * @param i indicates the number of headers to display for parallel eye scan
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_eye_scan_header(int8_t i);

/** Display Eye scan footer to stdout and log.
 * @param i indicates the number of footers to display for parallel eye scan
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_eye_scan_footer(int8_t i);

/** Check status of eye scan operation in uC.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *status returns a status word                  \n
 *    bit  15  - indicates the eye scan is complete      \n
 *    bit  14  - indicates uC is slower than read access \n
 *    bit  13  - indicates uC is faster than read access \n
 *    bit 12:8 - reserved                                \n
 *    bit  7:0 - indicates amount of data in the uC buffer
 *
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_read_eye_scan_status(srds_access_t *sa__, uint16_t *status);

/** Restores uC after running diagnostic eye scans.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_meas_eye_scan_done(srds_access_t *sa__);

/** Start a uC controlled BER scan function.
 * This will tell the uC to make a number of BER measurements at different offsets
 * and provide data back to API as a block of data. Several types of tests can be
 * made including Passive (which can be run on any data pattern and does not affect
 * datapath) or Intrusive (which can be run only when PRBS pattern is being used
 * and will cause errors to occur).  Intrusive test has a limited vertical range!
 *
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ber_scan_mode configures the type of test (use #srds_diag_ber_mode_enum)         \n
 * \verbatim
  bit  7  - reserved
  bit  6  - 1 = BER FAST scan mode (reduce minimum sample time from 0.1sec to 0.02sec
  bit 5:4 - used for vertical intrusive test only (not recommended)
              00 = move 1 slicer in direction bit0 (slicer selected for max range)
              11 = move both, independent direction(not depend on bit0) legacy 40nm mode
              01 = move only odd(depends on bit0)
              10 = move only even(depends on bit0)
  bit  3  - 1 = set passive scan to narrow vertical range(150mV); 0 = full range(250mV)
  bit  2  - 1 = intrusive eye scan; 0 = passive
  bit  1  - 1 = scan horizontal direction; 0 = scan vertical
  bit  0  - 1 = scan negative portion of eye to center; 1 = scan positive  \endverbatim
 * @param timer_control sets the total test time in units of ~1.31 seconds
 * @param max_error_control sets the error threshold for test in units of 16.(4=64 error threshold)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_start_ber_scan_test(srds_access_t *sa__, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control);

/** Reads the BER scan data from uC after test has completed.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *errors is pointer to 32 element array of uint32 which will contain error data
 * @param *timer_values is pointer to 32 element array of uint32 which will contain time data
 * @param *cnt returns the number of samples
 * @param timeout for polling data from uC (typically 2000)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_read_ber_scan_data(srds_access_t *sa__, uint32_t *errors, uint32_t *timer_values, uint8_t *cnt, uint32_t timeout);

/** Extrapolate BER and display margin information
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param rate specifies the data rate in Hz
 * @param ber_scan_mode the type of test used to take the data(use #srds_diag_ber_mode_enum)
 * @param *total_errs is pointer to 32 element array of uint32 containing the error data
 * @param *total_time is pointer to 32 element array of uint32 containing the time data
 * @param max_offset is the maximum offset setting which is present in data (usually 31)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_ber_scan_data (srds_access_t *sa__, USR_DOUBLE rate, uint8_t ber_scan_mode, uint32_t *total_errs, uint32_t *total_time, uint8_t max_offset);

/** Example eye margin projection API.
 * This is an example function which uses the following API's to measure and display BER margin projections
 * falcon16_tsc_start_ber_scan_test(), falcon16_tsc_read_ber_scan_data(), falcon16_tsc_display_ber_scan_data().
 *
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param rate specifies the data rate in Hz
 * @param ber_scan_mode the type of test used to take the data(use #srds_diag_ber_mode_enum)
 * @param timer_control sets the total test time in units of ~1.31 seconds
 * @param max_error_control sets the error threshold for test in units of 16.(4=64 errors)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_eye_margin_proj(srds_access_t *sa__, USR_DOUBLE rate, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control);

/*-----------------------------------------------*/
/*  Display Serdes Lane Config and Debug Status  */
/*-----------------------------------------------*/
/** Display current lane configuration.
 * Reads and displays all important lane configuration RAM variables and register fields.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_lane_config(srds_access_t *sa__);

/** Display current lane debug status.
 * Reads and displays all vital lane user status and debug status.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_lane_debug_status(srds_access_t *sa__);


/*-----------------------------*/
/*  Display Serdes Lane State  */
/*-----------------------------*/
/** Display current lane state.
 * Reads and displays all important lane state values in a single line.
 * \n Note: Call functions falcon16_tsc_display_lane_state_hdr() before and falcon16_tsc_display_lane_state_legend() after
 * to get a formatted lane state display with legend
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_lane_state(srds_access_t *sa__);

/** Column definition header for falcon16_tsc display state.
 * To be called before falcon16_tsc_display_lane_state() function.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_lane_state_hdr(void);

/** Detailed explanation of each column in falcon16_tsc display state.
 * To be called after falcon16_tsc_display_lane_state() function to display the legends.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_lane_state_legend(void);

/*---------------------------------------*/
/*  Required Serdes Diag/Debug routines  */
/*---------------------------------------*/
/** Parameterized diagnostic function which provides comprehensive diagnostic and debug information
 * This function is required to be implemented by upper level software to enable falcon16_tsc support.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param diag_level is a MASK style parameter to enable variable level of display.
 *     enum #srds_diag_level_enum can be used to help set the diag_level \n
 * \verbatim
  bit  0 - 1 = display extended lane state similar to existing falcon ext lane state.
           0 = standard display lane state 
  bit  1 - display core state
  bit  2 - event log
  bit  3 - display fast eye scan
  bit  4 - dump reg_dump 1 (core level registers)
  bit  5 - dump reg_dump 2 (lane level registers)
  bit  6 - dump core uC vars
  bit  7 - dump lane uC vars
  bit  8 - display lane debug state
  bit  9 - display data for ber projection vertical
  bit 10 - display data for ber projection horzontal
  bit 11 - event log (safe), which does not involve micro for read  \endverbatim
 *
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_diag_data(srds_access_t *sa__, uint16_t diag_level);

/** Parameterized diagnostic function which access to all falcon16_tsc control and status bits
 * This function is required to be implemented by upper level software to enable falcon16_tsc support.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param type controls the type of access requested specified through enum #srds_diag_access_enum \n
 * \verbatim
 type =  0 - Register Read (param becomes count)
 type =  1 - Register Read-Modify-Write (param becomes mask)
 type =  2 - CORE RAM Read byte  (data becomes count)
 type =  3 - CORE RAM Read-Modify-Write byte (param becomes mask)
 type =  4 - CORE RAM Read word  (data becomes count)
 type =  5 - CORE RAM Read-Modify-Write word (param becomes mask)
 type =  6 - LANE RAM Read byte    (data becomes count)
 type =  7 - LANE RAM Read-Modify-Write byte (param becomes mask)
 type =  8 - LANE RAM Read word  (data becomes count)
 type =  9 - LANE RAM Read-Modify-Write word (param becomes mask)
 type = 10 - Global RAM Read byte  (data becomes count)
 type = 11 - Global RAM Read-Modify-Write byte (param becomes mask)
 type = 12 - Global RAM Read word  (data becomes count)
 type = 13 - Global RAM Read-Modify-Write word (param becomes mask)
 type = 14 - uC Command (addr becomes command; param becomes supp_info)
               See microcode for available commands and further information.
 type = 15 - Enable Breakboint 
 type = 16 - Next or Goto Breakpoint (addr becomes breakpoint #)
 type = 17 - Read Breakpoint
 type = 18 - Disable Breakpoint
 type = 19 - Gather BER projection data (addr becomes ber_mode;data becomes max time;param becomes error threshold)
 
 Note: Global RAM access will ONLY WORK on Cores with direct RAM access \endverbatim
 *
 * @param addr in most cases is the address of the register or RAM location
 * @param data in most cases is the data to be written
 * @param param is the multipurpose parameter and can be mask or other data
*/
 err_code_t falcon16_tsc_diag_access(srds_access_t *sa__, enum srds_diag_access_enum type, uint16_t addr, uint16_t data, uint16_t param);


/** FOR INTERNAL use only!
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */

err_code_t falcon16_tsc_INTERNAL_poll_diag_data(srds_access_t *sa__, srds_info_t *falcon16_tsc_info_ptr, uint16_t *status, uint8_t *diag_rd_ptr, uint8_t byte_count, uint32_t timeout_ms);

/*---------------------*/
/*   CL72/CL93 Status  */
/*---------------------*/

/** Display CL93n72 Status of current lane.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_display_cl93n72_status(srds_access_t *sa__);


#endif
