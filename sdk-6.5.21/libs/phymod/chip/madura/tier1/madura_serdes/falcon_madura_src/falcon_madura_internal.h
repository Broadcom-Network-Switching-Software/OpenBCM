/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :  $Id: falcon_furia_internal.h 857 2015-01-16 22:22:38Z kirand $ *
 *                                                                                *
 *  Description :  Internal API functions                                         *
 *                                                                                *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                          *
 *  No portions of this material may be reproduced in any form without            *
 *  the written permission of:                                                    *
 *      Broadcom Corporation                                                      *
 *      5300 California Avenue                                                    *
 *      Irvine, CA  92617                                                         *
 *                                                                                *
 *  All information contained in this document is Broadcom Corporation            *
 *  company private proprietary, and trade secret.                                *
 */

/** @file falcon_furia_internal.h 
 * Internal API functions
 */

#ifndef FALCON_FURIA_API_INTERNAL_H
#define FALCON_FURIA_API_INTERNAL_H

#define SDK_STATIC static


#include "falcon_madura_functions.h"
#include "falcon_madura_common.h"
#include "falcon_madura_field_access.h"
#include "falcon_madura_ipconfig.h"
#include "../common/srds_api_err_code.h"
#include "falcon_madura_interface.h"
#include "falcon_madura_debug_functions.h"

/*------------------------------*/
/** Serdes OSR Mode Structure   */
/*------------------------------*/
typedef struct {
    /** TX OSR Mode */
    uint8_t tx;      
    /** RX OSR Mode */
    uint8_t rx;
    /** OSR Mode for TX and RX (used when both TX and RX should have same OSR Mode) */
    uint8_t tx_rx;   
}falcon_furia_madura_osr_mode_st;

/*------------------------------*/
/** Serdes Lane State Structure */
/*------------------------------*/
typedef struct {    
       /** uC lane configuration */ 
       uint16_t ucv_config;
       /** Frequency offset of local reference clock with respect to RX data in ppm */
       int16_t rx_ppm;
       /** Vertical threshold voltage of p1 slicer (mV) */
       int16_t p1_lvl;       
       /** Vertical threshold voltage of m1 slicer (mV) */
       int16_t m1_lvl;
       /** Link time in milliseconds */
       uint16_t link_time;
       /** OSR Mode */
       falcon_furia_madura_osr_mode_st osr_mode;
       /** Signal Detect */
       uint8_t sig_det;
       /** PMD RX Lock */
       uint8_t rx_lock;
       /** Delay of zero crossing slicer, m1, wrt to data in PI codes */
       int8_t clk90;
       /** Delay of diagnostic/lms slicer, p1, wrt to data in PI codes */
       int8_t clkp1;
       /** Peaking Filter Main Settings */
       int8_t pf_main;
       /** Peaking Filter Hiz mode enable */
       int8_t pf_hiz;
       /** Peaking Filter DC gain adjustment for CTLE */
       int8_t pf_bst;
       /** Low Frequency Peaking filter control */
       int8_t pf2_ctrl;
       /** Variable Gain Amplifier settings */
       int8_t vga;
       /** DC offset DAC control value */
       int8_t dc_offset;
       /** P1 eyediag status */
       int8_t p1_lvl_ctrl;
       /** DFE tap 1 value */
       int8_t dfe1;
       /** DFE tap 2 value */
       int8_t dfe2;
       /** DFE tap 3 value */
       int8_t dfe3;
       /** DFE tap 4 value */
       int8_t dfe4;
       /** DFE tap 5 value */
       int8_t dfe5;
       /** DFE tap 6 value */
       int8_t dfe6;
       /** DFE tap 1 Duty Cycle Distortion */
       int8_t dfe1_dcd;
       /** DFE tap 2 Duty Cycle Distortion */
       int8_t dfe2_dcd;
       /** Slicer calibration control codes (p1 even) */
       int8_t pe;
       /** Slicer calibration control codes (data even) */
       int8_t ze;
       /** Slicer calibration control codes (m1 even) */
       int8_t me;
       /** Slicer calibration control codes (p1 odd) */
       int8_t po;
       /** Slicer calibration control codes (data odd) */
       int8_t zo;
       /** Slicer calibration control codes (m1 odd) */
       int8_t mo;
       /** Frequency offset of local reference clock with respect to TX data in ppm */
       int16_t tx_ppm;
       /** TX equalization FIR pre tap weight */
       int8_t txfir_pre;
       /** TX equalization FIR main tap weight */
       int8_t txfir_main;
       /** TX equalization FIR post1 tap weight */
       int8_t txfir_post1;
       /** TX equalization FIR post2 tap weight */
       int8_t txfir_post2;
       /** TX equalization FIR post3 tap weight */
       int8_t txfir_post3;
       /** Horizontal left eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t heye_left;
       /** Horizontal right eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t heye_right;
       /** Vertical upper eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t veye_upper;
       /** Vertical lower eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV */
       uint16_t veye_lower;
       /** Baud Rate Phase Detector enable */
       uint8_t br_pd_en;
} falcon_furia_madura_lane_state_st;

/*------------------------------*/
/** Serdes Core State Structure */
/*------------------------------*/
typedef struct {
       /** Core DP Reset State */
       uint8_t  core_reset;
       /**  PLL Powerdown enable */
       uint8_t  pll_pwrdn;
       /** Micro active enable */
       uint8_t  uc_active;
       /** Comclk Frequency in Mhz */
       uint16_t comclk_mhz;
       /** uCode Major Version number */
       uint16_t ucode_version;
       /** uCode Minor Version number */
       uint8_t  ucode_minor_version;
       /** AFE Hardware version */
       uint8_t  afe_hardware_version;
       /** uC Die Temperature Index */
       uint8_t  temp_idx; 
       /** Average Die Temperature (13-bit format) */ 
       uint16_t avg_tmon;
       /** Analog Resistor Calibration value */
       uint8_t  rescal;
       /** VCO Rate in MHz */
       uint16_t vco_rate_mhz;
       /**  Analog VCO Range */
       uint8_t  analog_vco_range;
       /** PLL Divider value */
       uint8_t  pll_div;
       /** PLL Lock */
       uint8_t  pll_lock;
       /* Live die temperature in Celsius */
       int16_t die_temp;
} falcon_furia_madura_core_state_st;


/** Check if the micro's operations on that lane are stopped. 
 * @return err_code Error Code "ERR_CODE_UC_NOT_STOPPED" returned if micro NOT stopped
 */
SDK_STATIC err_code_t _check_uc_lane_stopped( const phymod_access_t *pa );

/** Calculate the mode_sel parameter for tx pattern generator.
 * @param *mode_sel Mode select to be used for generating required pattern
 * @param *zero_pad_len Length of zero padding to be used for generating required pattern 
 * @param patt_length Desired Pattern length
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */ 
SDK_STATIC err_code_t _calc_patt_gen_mode_sel( const phymod_access_t *pa, uint8_t *mode_sel, uint8_t *zero_pad_len, uint8_t patt_length);




/*-------------------*/
/*  Ladder controls  */
/*-------------------*/
/** Converts a ladder setting to mV, given the range.
 * @param ctrl is the threshold control (-31..31) maps to -RANGE to RANGE in non-uniform steps
 * @param range_250 determines the range 0 = +/-150mV, 1 = +/-250mV
 * @return ladder threshold voltage in mV
 */
SDK_STATIC int16_t _ladder_setting_to_mV(const phymod_access_t *pa,int8_t ctrl, uint8_t range_250);



/*-----------------------*/
/*  TX_PI and ULL Setup  */
/*-----------------------*/

/** TX PI setup for Repeater Mode.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */ 
static err_code_t _rptr_mode_timing( const phymod_access_t *pa );                

/** Setup the Ultra low latency clk and datapath.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors) 
 */ 
static err_code_t _ull_mode_setup( const phymod_access_t *pa ); 
                  

/** Safe multiply - multiplies 2 numbers and checks for overflow.
 * @param a First input
 * @param b Second input
 * @param *of Pointer to overflow indicator
 * @return value of a * b 
 */ 
static uint32_t _mult_with_overflow_check(uint32_t a, uint32_t b, uint8_t *of);


/*-----------------------------------------*/
/*  APIs used in Config Shared TX Pattern  */
/*-----------------------------------------*/
/** Compute Binary string for a Hex value ['0' to 'F'].
 * @param var Hex value to be converted to Binary (eg: '6', 'A', ...)  
 * @param bin Binary string returned by API (eg: '0110', '1010', ...)
 * @return Error Code generated by invalid hex variable (returns ERR_CODE_NONE if no errors) 
 */ 
static err_code_t _compute_bin( const phymod_access_t *pa, char var, char bin[]);

/** Compute Hex value for a Binary string ['0000' to '1111'].
 * @param bin Binary string to be coverted (eg: '0110', '1010', ...)
 * @param *hex Hex value calculated from the input Binary string
 * @return Error Code generated by invalid Binary string (returns ERR_CODE_NONE if no errors) 
 */ 
static err_code_t _compute_hex( const phymod_access_t *pa, char bin[], uint8_t *hex);


/*-----------------------------------------*/
/*  APIs used in Read Event Logger         */
/*-----------------------------------------*/
/** Interpret CL72 coefficient status field.
 * @param val Coefficient status field value to be interpreted
 * @return Char string to be displayed in event log 
 */ 
static char* _status_val_2_str( const phymod_access_t *pa, uint8_t val);

/** Interpret CL72 coefficient update field.
 * @param val Coefficient update field value to be interpreted
 * @return Char string to be displayed in event log
 */ 
static char* _update_val_2_str( const phymod_access_t *pa, uint8_t val);

/** Interpret error event code.
 * @param val Event code error value to be interpreted
 * @return Char string to be displayed in event log
 */ 
static char* _error_val_2_str( const phymod_access_t *pa, uint8_t val);

/** Display event information.
 * @param event_id Event id to be displayed
 * @param entry_len Length of the event entry
 * @param prev_cursor CL72 prev cursor value 
 * @param curr_cursor CL72 curr cursor value
 * @param post_cursor CL72 post cursor value
 * @param *supp_info Supplement information
 */ 
static err_code_t _falcon_display_event( const phymod_access_t *pa, uint8_t event_id, uint8_t entry_len, uint8_t prev_cursor, uint8_t curr_cursor, uint8_t post_cursor, uint8_t *supp_info);


/** Convert float8 to usigned int32.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input Float8 number
 * @return Usigned 32bit number  
 */
static uint32_t _float8_to_int32(float8_t input);


#ifdef TO_FLOATS
/*-----------------------------------------*/
/*  APIs used in uC data conversion        */
/*-----------------------------------------*/

/** Convert usigned int32 to float8.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input Unsigned int
 * @return Float8 8 bit representations of 32bit number  
 */
static float8_t _int32_to_float8(uint32_t input);
#endif


/*-----------------------------*/
/*  Read / Display Core state  */
/*-----------------------------*/
/** Read current falcon_furia_madura core status.
 * @param *istate Current falcon_furia_madura core status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _falcon_furia_madura_read_core_state( const phymod_access_t *pa, falcon_furia_madura_core_state_st *istate);

/** Display current core state.
 * Reads and displays all important core state values.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_furia_madura_display_core_state_no_newline( const phymod_access_t *pa ); 

/*-----------------------------*/
/*  Read / Display Lane state  */
/*-----------------------------*/
/** Read current falcon_furia_madura lane status.
 * @param *istate Current falcon_furia_madura lane status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _falcon_furia_madura_read_lane_state( const phymod_access_t *pa, falcon_furia_madura_lane_state_st *istate);


/*-----------------------------*/
/*  Get OSR mode              */
/*-----------------------------*/
/** Read current falcon_furia_madura lane status.
 * @param *mode Returns with the osr mode structure
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _falcon_furia_madura_get_osr_mode( const phymod_access_t *pa, falcon_furia_madura_osr_mode_st *mode);

/** Display current lane state.
 * Reads and displays all important lane state values.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_furia_madura_display_lane_state_no_newline( const phymod_access_t *pa );



/** Convert eye margin to mV.
 * Takes in the ladder setting with 3 fractional bits and converts to mV.
 * @param var Ladder setting with 3 fractional bits
 * @param ladder_range Specified if ladder is configured for 150mV or 250mV range
 * @return Eye opening in mV
 */
SDK_STATIC uint16_t _eye_to_mV(const phymod_access_t *pa, uint8_t var, uint8_t ladder_range); 

/** Convert eye margin to mUI.
 * Takes in a horizontal margin in Phase Interpolator codes and converts it to mUI.
 * @param var Horizontal margin in Phase Interpolator codes with 3 fractional bits
 * @return Eye opening in mV
 */
SDK_STATIC uint16_t _eye_to_mUI(const phymod_access_t *pa, uint8_t var); 


/** Serdes Core ClockGate.
 * Along with falcon_furia_madura_core_clkgate(), all lanes should also be clock gated using falcon_furia_madura_lane_clkgate() to complete a Core Clockgate 
 * @param enable Enable clockgate (1 = Enable clokgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_furia_madura_core_clkgate( const phymod_access_t *pa, uint8_t enable);

/** Serdes Lane ClockGate.
 * @param enable Enable lane clockgate (1 = Enable clockgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors) 
 */
static err_code_t _falcon_furia_madura_lane_clkgate( const phymod_access_t *pa, uint8_t enable);


/** Set function for PF.
 * @param val Signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_pf_main( const phymod_access_t *pa, uint8_t val);

/** Get function for PF
 * @param *val PF read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_pf_main( const phymod_access_t *pa, int8_t *val);

/** Set function for PF2.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_pf2( const phymod_access_t *pa, uint8_t val);

/** Get function for PF2.
 * @param *val PF2 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_pf2( const phymod_access_t *pa, int8_t *val);

/** Set function for VGA.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_vga( const phymod_access_t *pa, uint8_t val);

/** Get function for VGA.
 * @param *val VGA read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_vga( const phymod_access_t *pa, int8_t *val);


/** Set function for DFE Tap1.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _set_rx_dfe1( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap1.
 * @param *val DFE1 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _get_rx_dfe1( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap2.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _set_rx_dfe2( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap2.
 * @param *val DFE2 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
SDK_STATIC err_code_t _get_rx_dfe2( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap3.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_dfe3( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap3.
 * @param *val DFE3 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_dfe3( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap4.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_dfe4( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap4.
 * @param *val DFE4 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_dfe4( const phymod_access_t *pa, int8_t *val);

/** Set function for DFE Tap5.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _set_rx_dfe5( const phymod_access_t *pa, int8_t val);

/** Get function for DFE Tap5
 * @param *val DFE5 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
static err_code_t _get_rx_dfe5( const phymod_access_t *pa, int8_t *val);

/* Set/Get DFE tap values */
static err_code_t _set_rx_dfe6( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe6( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe7( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe7( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe8( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe8( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe9( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe9( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe10( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe10( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe11( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe11( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe12( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe12( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe13( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe13( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_rx_dfe14( const phymod_access_t *pa, int8_t val);
static err_code_t _get_rx_dfe14( const phymod_access_t *pa, int8_t *val);

/* Set/Get TX Tap values */
static err_code_t _set_tx_pre( const phymod_access_t *pa, uint8_t val);
static err_code_t _set_tx_amp( const phymod_access_t *pa, int8_t val);
static err_code_t _get_tx_amp( const phymod_access_t *pa, int8_t *val);
static err_code_t _set_tx_main( const phymod_access_t *pa, uint8_t val);
static err_code_t _set_tx_post1( const phymod_access_t *pa, uint8_t val);
static err_code_t _set_tx_post2( const phymod_access_t *pa, int8_t val);
static err_code_t _set_tx_post3( const phymod_access_t *pa, int8_t val);
static err_code_t _get_tx_post3( const phymod_access_t *pa, int8_t *val);

/* Lane Config Struct */
void _update_uc_lane_config_st( const phymod_access_t *pa, struct falcon_furia_madura_uc_lane_config_st *st);
void _update_uc_lane_config_word( const phymod_access_t *pa, struct falcon_furia_madura_uc_lane_config_st *st);

/* Lane User Control Disable Startup Function Struct */
void _update_usr_ctrl_disable_functions_st( const phymod_access_t *pa, struct falcon_furia_madura_usr_ctrl_disable_functions_st *st);
void _update_usr_ctrl_disable_functions_byte( const phymod_access_t *pa, struct falcon_furia_madura_usr_ctrl_disable_functions_st *st);

/* Lane User Control Disable Startup DFE Function Struct */
void _update_usr_ctrl_disable_dfe_functions_st( const phymod_access_t *pa, struct falcon_furia_madura_usr_ctrl_disable_dfe_functions_st *st);
void _update_usr_ctrl_disable_dfe_functions_byte( const phymod_access_t *pa, struct falcon_furia_madura_usr_ctrl_disable_dfe_functions_st *st);

/* Core Config Struct */
void _update_uc_core_config_st( const phymod_access_t *pa, struct  falcon_furia_madura_uc_core_config_st *st);
void _update_uc_core_config_word( const phymod_access_t *pa, struct  falcon_furia_madura_uc_core_config_st *st);

#endif
