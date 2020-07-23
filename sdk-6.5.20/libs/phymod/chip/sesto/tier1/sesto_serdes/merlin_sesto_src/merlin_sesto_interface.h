/***********************************************************************************
 ***********************************************************************************
 *                                                                                 *
 *  Revision    :  $Id: merlin_sesto_interface.h 869 2015-01-23 00:24:44Z kirand $   *
 *                                                                                 *
 *  Description :  Interface functions targeted to IP user                         *
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                           *
 *  No portions of this material may be reproduced in any form without             *
 *  the written permission of:                                                     *
 *      Broadcom Corporation                                                       *
 *      5300 California Avenue                                                     *
 *      Irvine, CA  92617                                                          *
 *                                                                                 *
 *  All information contained in this document is Broadcom Corporation             *
 *  company private proprietary, and trade secret.                                 *
 */

/*! \mainpage 
 *
 * \section intro_sec Introduction
 *
 * This documentation files are intended to guide a developer in using the 
 * Broadcom SerDes core within a larger ecosystem by providing specific API
 * function calls which can be used to perform all necessary operations.
 *
 * The detailed description about the API directory structure, API overview,
 * release procedure, and check out process can be found in the Serdes API User Guide 
 * (http://ingbu.broadcom.com/hsip/merlin_sesto/SerDes%20Documents/PMD_Methodology/Serdes%20External%20Micro%20User%20Guide.docx)
 * \n \n
 *
 * \section API_sec Important API Files
 *
 * merlin_sesto_interface.h - provides the primary functionality.
 *
 * merlin_sesto_dependencies.h - defines the functions to be provided by the IP user
 *
 * \n \n
 * Copyright 2014 Broadcom Corporation all rights reserved
 */

/** @file merlin_sesto_interface.h
 * Main interface functions provided to IP User
 */

#ifndef MERLIN_SESTO_API_INTERFACE_H
#define MERLIN_SESTO_API_INTERFACE_H

#include "merlin_sesto_usr_includes.h"
#include "merlin_sesto_ipconfig.h"
#include "merlin_sesto_enum.h"
#include "../common/srds_api_err_code.h"


/*----------------------------------------*/
/*  Lane/Core structs (without bitfields) */
/*----------------------------------------*/

/** Lane Config Variable Structure in Microcode */
struct merlin_sesto_uc_lane_config_field_st {
  uint8_t  lane_cfg_from_pcs ;
  uint8_t  an_enabled        ;
  uint8_t  dfe_on            ;
  uint8_t  force_brdfe_on    ;
  uint8_t  media_type        ;
  uint8_t  unreliable_los    ;
  uint8_t  scrambling_dis    ;
  uint8_t  cl72_auto_polarity_en ;
  uint8_t  cl72_restart_timeout_en;
  uint8_t  reserved          ;
};

/** Core Config Variable Structure in Microcode */
struct merlin_sesto_uc_core_config_field_st {
  uint8_t  core_cfg_from_pcs ;
  uint8_t  vco_rate          ;
  uint8_t  reserved1         ;
  uint8_t  reserved2         ;
};

/** Lane Config Struct */
struct  merlin_sesto_uc_lane_config_st {
  struct merlin_sesto_uc_lane_config_field_st field;  
  uint16_t word;                       
}; 

/** Core Config Struct */
struct  merlin_sesto_uc_core_config_st {
  struct merlin_sesto_uc_core_config_field_st field; 
  uint16_t word;         
};

/** Eyescan Options Struct */
struct merlin_sesto_eyescan_options_st {
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


/****************************************************/
/*  CORE Based APIs - Required to be used per Core  */
/****************************************************/
/* Returns API Version Number */
/** API Version Number.
 * @param *api_version API Version Number returned by the API
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_version(const phymod_access_t *pa, uint32_t *api_version);

/*------------------------------------------------------*/
/*  APIs to Read/Write Core Config variables in uC RAM  */
/*------------------------------------------------------*/
/** Write to core_config uC RAM variable.
 * @param struct_val Value to be written into core_config RAM variable.
 *                   (Note that struct_val.word must be = 0, only the fields are used) 
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_set_uc_core_config(const phymod_access_t *pa, struct merlin_sesto_uc_core_config_st struct_val);

/** Read value of core_config uC RAM variable.
 * @param *struct_val Value to be written into core_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_get_uc_core_config(const phymod_access_t *pa, struct merlin_sesto_uc_core_config_st *struct_val);

/*----------------------------------------*/
/*  Display Core Config and Debug Status  */
/*----------------------------------------*/
/** Display Core configurations (RAM config variables and config register fields).
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_core_config(const phymod_access_t *pa );

/** Display current Core state. Read and displays core status variables and fields.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_core_state(const phymod_access_t *pa );

/** Column definition header for merlin_sesto_display_core_state() API output.
 * To be called before merlin_sesto_display_core_state_line() API. 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_core_state_hdr(const phymod_access_t *pa );

/** Display current Core state in single line. Read and displays core status variables and fields.
 * Call merlin_sesto_display_core_state_hdr() API before and merlin_sesto_display_core_state_legend() after 
 * calling this API to get a formatted core state display with legend. 
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_core_state_line(const phymod_access_t *pa );

/** Detailed description of each column in merlin_sesto_display_core_state_line() API output.
 * To be called after merlin_sesto_display_core_state_line() API to display the legends. 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_core_state_legend(void);

/*----------------------------------------*/
/*  Temperature forcing and reading		  */
/*----------------------------------------*/
/** Forces die temperature in degrees Ceisius (as integer).
 * @param die_temp  Die temperature in degrees Celsius.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_force_die_temperature (const phymod_access_t *pa, int16_t die_temp);

/** Read die temperature in degrees Ceisius (as integer) 
 * @param *die_temp  Die temperature in degrees Celsius 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_read_die_temperature (const phymod_access_t *pa, int16_t *die_temp);

/** Read die temperature in degrees Ceisius (as double) 
 * @param *die_temp  Die temperature in degrees Celsius (as double)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_read_die_temperature_double (const phymod_access_t *pa, USR_DOUBLE *die_temp);



/** Set the uC active mode.
 * uC active bit should be set by the user to ensure the ucode loaded into the micro is executed. See Programmers Guide for more details.
 * @param enable Enable/Disable uC Active (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_uc_active_enable(const phymod_access_t *pa, uint8_t enable);



/*------------------------------*/
/*  Shared TX Pattern Generator */
/*------------------------------*/
/** Configure Shared TX Pattern API. 
 * An input string (hex or binary) and pattern length are taken in as inputs, based on which the Pattern Generator registers 
 * are programmed to the required values to generate that pattern. 
 * Note: merlin_sesto_tx_shared_patt_gen_en() API should be called to enable the Pattern generator for that particular lane.
 * @param patt_length Pattern length 
 * @param pattern Input Pattern - Can be in hex (eg: "0xB055") or in binary (eg: "011011")
 * @return Error Code generated by invalid input pattern or pattern length (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_config_shared_tx_pattern(const phymod_access_t *pa, uint8_t patt_length, const char pattern[]);


/*-----------------------*/
/*  IDDQ / Clkgate APIs  */
/*-----------------------*/
/** Core configuration for IDDQ.
 * Note: User also needs to configure all lanes through merlin_sesto_lane_config_for_iddq()
 * before enabling IDDQ by asserting IDDQ pin.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_core_config_for_iddq(const phymod_access_t *pa );

/** Serdes Core Powerdown.
 * Along with merlin_sesto_core_pwrdn(), all lanes powerdowns should also be issued using 
 * merlin_sesto_lane_pwrdn() to complete a Core Powerdown. 
 * @param mode based on enum #srds_core_pwrdn_mode_enum select from ON, CORE, DEEP power down modes
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_core_pwrdn(const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode);




/**************************************************/
/* LANE Based APIs - Required to be used per Lane */
/**************************************************/

/*------------------------------------------------------------*/
/*  APIs to Write Lane Config and User variables into uC RAM  */
/*------------------------------------------------------------*/
/** Write to lane_config uC RAM variable.
 * Note: This API should be used only during configuration under dp_reset.
 * @param struct_val Value to be written into lane_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_set_uc_lane_cfg(const phymod_access_t *pa, struct merlin_sesto_uc_lane_config_st struct_val);

/*-----------------------------------------------------------*/
/*  APIs to Read Lane Config and User variables from uC RAM  */
/*-----------------------------------------------------------*/
/** Read value of lane_config uC RAM variable.
 * @param *struct_val Value read from lane_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_get_uc_lane_cfg(const phymod_access_t *pa, struct merlin_sesto_uc_lane_config_st *struct_val);


/*-----------------*/
/*  Configure PLL  */
/*-----------------*/
/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function. 
 *
 * Configures PLL registers to obtain the required VCO frequency.
 * @param pll_cfg Required PLL configuration
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_configure_pll(const phymod_access_t *pa, enum merlin_sesto_pll_enum pll_cfg); 


/*-------------------------*/
/*  Merlin TX Analog APIs  */
/*-------------------------*/

/** Validates Merlin TXFIR tap settings.
 * Returns failcodes if the combination of the TXFIR settings are invalid.
 * @param pre   TXFIR pre tap value (0..10) in 1/60 Vpp
 * @param main  TXFIR main tap value (40..60) in 1/60 Vpp
 * @param post1 TXFIR post1 tap value (0..23) in 1/60 Vpp
 * @param post2 TXFIR post2 tap value (0..5) in 1/60 Vpp 
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_validate_txfir_cfg(const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2);  

/** Validates and applies Merlin TXFIR tap settings.
 * Returns failcodes if TXFIR settings are invalid.
 * @param pre   TXFIR pre tap value (0..10) in 1/60 Vpp
 * @param main  TXFIR main tap value (40..60) in 1/60 Vpp
 * @param post1 TXFIR post1 tap value (0..23) in 1/60 Vpp
 * @param post2 TXFIR post2 tap value (0..5) in 1/60 Vpp 
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_apply_txfir_cfg(const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2);    

/** Read TX AFE parameters.
 * @param param selects the parameter to read based on #srds_tx_afe_settings_enum
 * @param *val is the returned signed value of the parameter
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_read_tx_afe(const phymod_access_t *pa, enum srds_tx_afe_settings_enum param, int8_t *val);

/** Validates and applies Merlin TX CTLE High Pass Filter (HPF) settings.
 * Returns failcode if CTLE HPF setting is invalid.
 * @param hpf_val High pass filter setting (0..4)
 * @return Error Code generated by invalid settings (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_config_tx_hpf(const phymod_access_t *pa, uint8_t hpf_val);

/** Reads Merlin TX CTLE High Pass Filter (HPF) settings.
 * Returns failcode if CTLE HPF setting is invalid.
 * @param *hpf_val High pass filter setting (0..4)
 * @return Error Code generated by invalid settings (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_rd_tx_hpf_config(const phymod_access_t *pa, uint8_t *hpf_val);


/*-------------------------------*/
/*  Falcon/Eagle TX Analog APIs  */
/*-------------------------------*/

/*---------------------------------*/
/* Display Eye Scan / Eye Density  */
/*---------------------------------*/

/** Displays Passive Eye Scan from -0.5 UI to 0.5UI to BER 1e-7.
 *  Function uses uC to acquire data.
 *  It also retrieves the data and displays it in ASCII-art style, where number N corresponds to 1e-N
 * 
 * This function retrieves the data from uC in horizontal stripe fashion
 *
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_eye_scan(const phymod_access_t *pa );

/** Displays Passive Eye Scan Data.
 * This function can be used to display data from meas_lowber_eye_scan() function.
 *  
 *  eyescan_options.horz_max = 31;  \n
 *  eyescan_options.horz_min = -31; \n
 *  eyescan_options.vert_max = 31;  \n
 *  eyescan_options.vert_min = -31; \n
 *  eyescan_options.hstep = 1;      \n
 *  eyescan_options.vstep = 1;      \n
 *  eyescan_options.timeout_in_milliseconds =  between 4 and 255
 *     (larger numbers will greatly increase test time!) 
 * @param eyescan_options is structure of options which control min, max, step, time, and linerate
 * @param *buffer is pointer to array which contains all samples.
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_lowber_eye(const phymod_access_t *pa, const struct merlin_sesto_eyescan_options_st eyescan_options, uint32_t *buffer);

/** Measure Passive Eye Scan based on eyescan_options provided and return data in buffer.
 * It relies on the accuracy of merlin_sesto_delay_us() function. \n\n
 * The recommend settings are: \n
 *  eyescan_options.horz_max = 31;  \n
 *  eyescan_options.horz_min = -31; \n
 *  eyescan_options.vert_max = 31;  \n
 *  eyescan_options.vert_min = -31; \n
 *  eyescan_options.hstep = 1;      \n
 *  eyescan_options.vstep = 1;      \n
 *  eyescan_options.timeout_in_milliseconds =  between 4 and 255
 *     (larger numbers will greatly increase test time!) \n
 * @param eyescan_options is structure of options which control min, max, step, time, and linerate
 * @param *buffer is pointer to array which is large enough to store all samples.
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_meas_lowber_eye(const phymod_access_t *pa, const struct merlin_sesto_eyescan_options_st eyescan_options, uint32_t *buffer);

/** Measure Eye Density based on eyescan_options provided and return data in buffer.
 * Due to sensitivity of the eye density test only internal trnsum timing can be used.
 * Therefore timeout_in_milliseconds has no affect. \n \n
 * The recommended settings are:    \n
 *  eyescan_options.horz_max = 31;  \n
 *  eyescan_options.horz_min = -31; \n
 *  eyescan_options.vert_max = 31;  \n
 *  eyescan_options.vert_min = -31; \n
 *  eyescan_options.hstep = 1;      \n
 *  eyescan_options.vstep = 1;      \n
 *  eyescan_options.timeout_in_milliseconds - not used for density measurements!
 * @param eyescan_options is structure of options which control min, max, step, time, and linerate
 * @param *buffer is pointer to array which is large enough to store all samples.
 * @param *buffer_size returns an unsigned integer indicating number of elements used in array
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_meas_eye_density_data(const phymod_access_t *pa, const struct merlin_sesto_eyescan_options_st eyescan_options, int32_t *buffer,uint16_t *buffer_size);

/** Displays Eye Density Data measured with merlin_sesto_meas_eye_density_data().
 *  
 * The recommended settings are:    \n
 *  eyescan_options.horz_max = 31;  \n
 *  eyescan_options.horz_min = -31; \n
 *  eyescan_options.vert_max = 31;  \n
 *  eyescan_options.vert_min = -31; \n
 *  eyescan_options.hstep = 1;      \n
 *  eyescan_options.vstep = 1;      \n
 *  eyescan_options.timeout_in_milliseconds - Not used for Density measurements
 * 
 * @param eyescan_options is structure of options which control min, max, step, time, and linerate
 * @param *buffer is pointer to array which contains all samples.
 * @param buffer_size must provide buffer_size which is returned from merlin_sesto_meas_eye_density_data()
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_eye_density_data(const phymod_access_t *pa, const struct merlin_sesto_eyescan_options_st eyescan_options, int32_t *buffer,uint16_t buffer_size);

/** Start uC controller eye scan Function.
 * Eye scan function provides a stripe of data at a time either vertical or horizontal.
 * This function only initiates the processor actions.  User must use merlin_sesto_read_eye_scan_stripe() function
 * to get the data from uC.
 * @param direction specifies either EYE_SCAN_VERTICAL or EYE_SCAN_HORIZ striping
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_meas_eye_scan_start(const phymod_access_t *pa, uint8_t direction);

/** Read a Stripe of eye scan data from uC.
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
err_code_t merlin_sesto_read_eye_scan_stripe(const phymod_access_t *pa, uint32_t *buffer,uint16_t *status);

/** Display Stripe of eye scan data to stdout and log.
 * @param y is the vertical step 31 to -31 (Eagle) 124 to -124(Falcon)
 * @param *buffer must be of size 64
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_eye_scan_stripe(const phymod_access_t *pa, int8_t y,uint32_t *buffer);

/** Display Eye scan header to stdout and log.
 * @param i indicates the number of headers to display for parallel eye scan
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_eye_scan_header(const phymod_access_t *pa, int8_t i);

/** Display Eye scan footer to stdout and log.
 * @param i indicates the number of footers to display for parallel eye scan
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_eye_scan_footer(const phymod_access_t *pa, int8_t i);

/** Check status of eye scan operation in uC. 
 * @param *status returns a status word                 \n
 *    bit 15 - indicates the eye scan is complete       \n
 *    bit 14 - indicates uC is slower than read access  \n
 *    bit 13 - indicates uC is faster than read access  \n
 *    bit 12-8  reserved                                \n
 *    bit 7-0 - indicates amount of data in the uC buffer
 *
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_read_eye_scan_status(const phymod_access_t *pa, uint16_t *status);

/** Restores uC after running diagnostic eye scans.
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_meas_eye_scan_done(const phymod_access_t *pa );

/** Start a uC controlled BER scan function.
 * This will tell the uC to make a number of BER measurements at different offsets
 * and provide data back to API as a block of data. Several types of tests can be
 * made including Passive (which can be run on any data pattern and does not affect 
 * datapath) or Intrusive (which can be run only when PRBS pattern is being used 
 * and will cause errors to occur).  Intrusive test has a limited vertical range!
 *
 * @param ber_scan_mode configures the type of test (use #srds_diag_ber_mode_enum)         \n
 * \verbatim
   bit 7   : reserved                                                                      
   bit 6   : 1 = BER FAST scan mode (reduce minimum sample time from 0.1sec to 0.02sec     
   bit 5-4 : used for vertical intrusive test only (not recommended)                     
              00=move 1 slicer in direction bit0 (slicer selected for max range)           
              11=move both, independent direction(not depend on bit0) legacy 40nm mode     
              01=move only odd(depends on bit0)                                            
              10=move only even(depends on bit0)                                           
   bit 3   : 1 = set passive scan to narrow vertical range(150mV); 0 = full range(250mV)   
   bit 2   : 1 = intrusive eye scan; 0 = passive                                           
   bit 1   : 1 = scan horizontal direction; 0 = scan vertical                              
   bit 0   : 1 = scan negative portion of eye to center; 1 = scan positive  \endverbatim
 * @param timer_control sets the total test time in units of ~1.31 seconds
 * @param max_error_control sets the error threshold for test in units of 16.(4=64 error threshold)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_start_ber_scan_test(const phymod_access_t *pa, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control);

/** Reads the BER scan data from uC after test has completed. 
 * @param *errors is pointer to 32 element array of uint32 which will contain error data
 * @param *timer_values is pointer to 32 element array of uint32 which will contain time data
 * @param *cnt returns the number of samples
 * @param timeout for polling data from uC (typically 2000)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_read_ber_scan_data(const phymod_access_t *pa, uint32_t *errors, uint32_t *timer_values, uint8_t *cnt, uint32_t timeout);

/** Extrapolate BER and display margin information
 * @param rate specifies the data rate in Hz
 * @param ber_scan_mode the type of test used to take the data(use #srds_diag_ber_mode_enum)
 * @param *total_errs is pointer to 32 element array of uint32 containing the error data
 * @param *total_time is pointer to 32 element array of uint32 containing the time data
 * @param max_offset is the maximum offset setting which is present in data (usually 31)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_ber_scan_data (const phymod_access_t *pa, USR_DOUBLE rate, uint8_t ber_scan_mode, uint32_t *total_errs, uint32_t *total_time, uint8_t max_offset); 

/** Example eye margin projection API.
 * This is an example function which uses the following API's to measure and display BER margin projections
 * merlin_sesto_start_ber_scan_test(), merlin_sesto_read_ber_scan_data(), merlin_sesto_display_ber_scan_data().
 *
 * @param rate specifies the data rate in Hz
 * @param ber_scan_mode the type of test used to take the data(use #srds_diag_ber_mode_enum)
 * @param timer_control sets the total test time in units of ~1.31 seconds
 * @param max_error_control sets the error threshold for test in units of 16.(4=64 errors)
 * @return Error Code during data collection (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_eye_margin_proj(const phymod_access_t *pa, USR_DOUBLE rate, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control);

/*-----------------------------------------------*/
/*  Display Serdes Lane Config and Debug Status  */
/*-----------------------------------------------*/
/** Display current lane configuration.
 * Reads and displays all important lane configuration RAM variables and register fields.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_lane_config(const phymod_access_t *pa );

/** Display current lane debug status.
 * Reads and displays all vital lane user status and debug status.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_lane_debug_status(const phymod_access_t *pa );


/*-----------------------------*/
/*  Display Serdes Lane State  */
/*-----------------------------*/
/** Display current lane state.
 * Reads and displays all important lane state values in a single line.
 * \n Note: Call functions merlin_sesto_display_lane_state_hdr() before and merlin_sesto_display_lane_state_legend() after 
 * to get a formatted lane state display with legend 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_lane_state(const phymod_access_t *pa );

/** Column definition header for merlin_sesto display state.
 * To be called before merlin_sesto_display_lane_state() function. 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_lane_state_hdr(const phymod_access_t *pa );

/** Detailed explanation of each column in merlin_sesto display state.
 * To be called after merlin_sesto_display_lane_state() function to display the legends. 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_lane_state_legend(const phymod_access_t *pa );



/*-------------------------------------*/
/*   PMD_RX_LOCK and CL72/CL93 Status  */
/*-------------------------------------*/

/** PMD rx lock status of current lane.
 * @param *pmd_rx_lock PMD_RX_LOCK status of current lane returned by API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_pmd_lock_status(const phymod_access_t *pa, uint8_t *pmd_rx_lock);

/** Display CL72 Status of current lane.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_display_cl72_status(const phymod_access_t *pa );



/*--------------------------------*/
/*  Serdes TX disable/RX Restart  */ 
/*--------------------------------*/
/** TX Disable.
 * @param enable Enable/Disable TX disable (1 = TX Disable asserted; 0 = TX Disable removed) 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_tx_disable(const phymod_access_t *pa, uint8_t enable);

/** Enable/disable Restart RX and hold. 
 * (Reset DSC state machine into RESTART State and hold it till disabled)
 * @param enable Enable/Disable Restart RX and hold (1 = RX restart and hold; 0 = Release hold in restart state)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_rx_restart(const phymod_access_t *pa, uint8_t enable);


/*-----------------------------*/
/*  Stop/Resume RX Adaptation  */
/*-----------------------------*/
/** Stop RX Adaptation on a Lane. Control is returned only after attempting to stop adaptation.
 * RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * @param enable Enable RX Adaptation stop (1 = Stop RX Adaptation on lane; 0 = Resume RX Adaptation on lane) 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_stop_rx_adaptation(const phymod_access_t *pa, uint8_t enable);

/** Request to stop RX Adaptation on a Lane.
 * Control will be returned immediately before adaptaion is completely stopped.
 * RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * To resume RX adaptation, use the merlin_sesto_stop_rx_adaptation() API.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_request_stop_rx_adaptation(const phymod_access_t *pa );

/*------------------------------------*/
/*  Read/Write all RX AFE parameters  */
/*------------------------------------*/

/** Write to RX AFE settings.
 * Note: RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * @param param Enum (#srds_rx_afe_settings_enum) to select the required RX AFE setting to be modified
 * @param val Value to be written to the selected AFE setting
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_write_rx_afe(const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t val);

/** Read from RX AFE settings.
 * @param param Enum (#srds_rx_afe_settings_enum) to select the required RX AFE setting to be read
 * @param *val Value to be written to the selected AFE setting
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_read_rx_afe(const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t *val);

/*----------------------------*/
/*  Enable Pattern Generator  */
/*----------------------------*/
/** Enable/Disable Shared TX pattern generator.
 * Note: The patt_length input to the function should be the value sent to the merlin_sesto_config_shared_tx_pattern() function
 * @param enable Enable shared fixed pattern generator (1 = Enable; 0 = Disable)
 * @param patt_length length of the pattern used in merlin_sesto_config_shared_tx_pattern()
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */ 
err_code_t merlin_sesto_tx_shared_patt_gen_en(const phymod_access_t *pa, uint8_t enable, uint8_t patt_length);


/*----------------------------*/
/*  Configure PRBS Functions  */
/*----------------------------*/
/**  Configure PRBS Generator.
 * Once the PRBS generator is configured, to enable PRBS use the merlin_sesto_tx_prbs_en() API.
 * @param prbs_poly_mode PRBS generator mode select (selects required PRBS polynomial)
 * @param prbs_inv PRBS invert enable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_config_tx_prbs(const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, uint8_t prbs_inv);

/**  Get PRBS Generator Configuration.
 * @param *prbs_poly_mode PRBS generator mode select (selects required PRBS polynomial)
 * @param *prbs_inv PRBS invert enable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_get_tx_prbs_config(const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv);

/** PRBS Generator Enable/Disable.
 * @param enable Enable PRBS Generator (1 = Enable; 0 = Disable)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */ 
err_code_t merlin_sesto_tx_prbs_en(const phymod_access_t *pa, uint8_t enable);

/** Get PRBS Generator Enable/Disable.
 * @param *enable returns the value of Enable PRBS Generator (1 = Enable; 0 = Disable)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */ 
err_code_t merlin_sesto_get_tx_prbs_en(const phymod_access_t *pa, uint8_t *enable);

/** PRBS Generator Single Bit Error Injection.
 * @param enable (1 = error is injected; 0 = no error is injected)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */ 
err_code_t merlin_sesto_tx_prbs_err_inject(const phymod_access_t *pa, uint8_t enable);

/**  Configure PRBS Checker.
 * Once the PRBS checker is configured, use the merlin_sesto_rx_prbs_en() API to enable the checker.
 * @param prbs_poly_mode PRBS checker mode select (selects required PRBS polynomial)
 * @param prbs_checker_mode Checker Mode to select PRBS LOCK state machine 
 * @param prbs_inv PRBS invert enable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_config_rx_prbs(const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, enum srds_prbs_checker_mode_enum prbs_checker_mode, uint8_t prbs_inv);

/**  Get PRBS Checker congifuration.
 * @param *prbs_poly_mode PRBS checker mode select (selects required PRBS polynomial)
 * @param *prbs_checker_mode Checker Mode to select PRBS LOCK state machine 
 * @param *prbs_inv PRBS invert enable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_get_rx_prbs_config(const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv);

/** PRBS Checker Enable/Disable.
 * @param enable Enable PRBS Checker (1 = Enable; 0 = Disable)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */ 
err_code_t merlin_sesto_rx_prbs_en(const phymod_access_t *pa, uint8_t enable);

/** Get PRBS Checker Enable/Disable.
 * @param *enable returns with the value of Enable PRBS Checker (1 = Enable; 0 = Disable)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */ 
err_code_t merlin_sesto_get_rx_prbs_en(const phymod_access_t *pa, uint8_t *enable);

/** PRBS Checker LOCK status (live status).
 * @param *chk_lock Live lock status read by API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_prbs_chk_lock_state(const phymod_access_t *pa, uint8_t *chk_lock);

/** PRBS Error Count and Lock Lost status.
 * Error count and lock lost read back as a single 32bit value. Bit 31 is lock lost and [30:0] is error count.  
 * @param *prbs_err_cnt 32bit value returned by API ([30:0] = Error Count; [31] = Lock lost) 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_prbs_err_count_ll(const phymod_access_t *pa, uint32_t *prbs_err_cnt);

/** PRBS Error Count and Lock Lost status.
 * Error count and lock lost read back on separate variables
 * @param *prbs_err_cnt 32bit Error count value
 * @param *lock_lost Lock Lost status (1 = if lock was ever lost)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_prbs_err_count_state(const phymod_access_t *pa, uint32_t *prbs_err_cnt, uint8_t *lock_lost);


/*--------------------------------------------------------------*/
/*  IDDQ / Powerdown / Deep Powerdown / Isolate Pins  */
/*--------------------------------------------------------------*/
/** Lane configurations for IDDQ.
 * Note: User needs to configure all lanes through merlin_sesto_lane_config_for_iddq() and also call merlin_sesto_core_config_for_iddq() 
 * before enabling IDDQ by asserting IDDQ pin.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_lane_config_for_iddq(const phymod_access_t *pa );

/** Serdes Lane Powerdown.
 * Powers down option available - TX only, RX only, complete LANE, complete lane DEEP, PWR_ON
 * Note: To wake up from lane DEEP powerdown, first call PWR_ON and then de-assert lane_dp_reset [wr_ln_dp_s_rstb(0x1)].
 * @param mode Enable/Disable lane powerdown based on #srds_core_pwrdn_mode_enum
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
err_code_t merlin_sesto_lane_pwrdn(const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode);

/** TX_PI Fixed Frequency Mode.
 * @param enable Enable/Disable TX_PI (1 = Enable; 0 = Disable)
 * @param freq_override_val Fixed Frequency Override value (Range: -8192 to + 8192; ppm = freq_override_val*781.25/8192)
 * @return Error Code generated by invalid TX_PI settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_tx_pi_freq_override(const phymod_access_t *pa, uint8_t enable, int16_t freq_override_val);


/*--------------------------------------------*/
/*  Loopback and Ultra-Low Latency Functions  */
/*--------------------------------------------*/
/** Switch to Remote Loopback from Ultra-Low Latency Mode.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_rmt_lpbk_from_ull(const phymod_access_t *pa );

/** Switch to Remote Loopback from Normal Loopback.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_rmt_lpbk_from_nl(const phymod_access_t *pa );

/** Switch to Ultra-Low Latency from Remote Loopback.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_ull_from_rmt_lpbk(const phymod_access_t *pa );

/** Switch to Normal Loopback from Remote Loopback.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_nl_from_rmt_lpbk(const phymod_access_t *pa );

/** Locks TX_PI to Loop timing.
 * @param enable Enable TX_PI lock to loop timing (1 = Enable Lock; 0 = Disable Lock)  
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_loop_timing(const phymod_access_t *pa, uint8_t enable);       

/** Enable/Disable Digital Loopback.
 * @param enable Enable Digital Loopback (1 = Enable dig_lpbk; 0 = Disable dig_lpbk)  
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_dig_lpbk(const phymod_access_t *pa, uint8_t enable);

/** Enable/Disable Digital Loopback in Repeater mode.
 * @param enable Enable Digital Loopback (1 = Enable dig_lpbk; 0 = Disable dig_lpbk) 
 * @param mode Appropriate Repeater mode (DATA_IN_SIDE - side where data is fed into , DIG_LPBK_SIDE - side where data is looped back) 
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_dig_lpbk_rptr(const phymod_access_t *pa, uint8_t enable, enum srds_rptr_mode_enum mode);

/** Enable/Disable Remote Loopback.
 * @param enable Enable Remote Loopback (1 = Enable rmt_lpbk; 0 = Disable rmt_lpbk)  
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin_sesto_rmt_lpbk(const phymod_access_t *pa, uint8_t enable);


#endif
