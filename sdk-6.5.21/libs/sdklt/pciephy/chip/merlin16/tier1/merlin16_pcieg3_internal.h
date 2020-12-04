/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :   *
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

/** @file merlin16_pcieg3_internal.h
 * Internal API functions
 */

#ifndef MERLIN16_PCIEG3_API_INTERNAL_H
#define MERLIN16_PCIEG3_API_INTERNAL_H

#include "merlin16_pcieg3_ipconfig.h"
#include "merlin16_pcieg3_select_defns.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_uc_common.h"
#include "common/srds_api_types.h"
#include "merlin16_pcieg3_enum.h"
#include "merlin16_pcieg3_types.h"
#include "merlin16_pcieg3_usr_includes.h"


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
}merlin16_pcieg3_osr_mode_st;

/*------------------------------*/
/** Serdes Lane State Structure */
/*------------------------------*/
typedef struct {
  /** uC lane configuration */
  uint16_t ucv_config;
  /** uC lane status */
  uint8_t ucv_status;
  /** Frequency offset of local reference clock with respect to RX data in ppm */
  int16_t rx_ppm;
  /** Vertical threshold voltage of p1 slicer (mV) */
  int16_t p1_lvl;
  /** Vertical threshold voltage of m1 slicer (mV) [Used to read out 'channel loss hint' in PAM4 mode] */
  int16_t m1_lvl;
  /** Link time in milliseconds */
  uint16_t link_time;
  /** OSR Mode */
  merlin16_pcieg3_osr_mode_st osr_mode;
  /** Signal Detect */
  uint8_t sig_det;
  /** Signal Detect Change */
  uint8_t sig_det_chg;
  /** PMD RX Lock */
  uint8_t rx_lock;
  /** PMD RX Lock Change */
  uint8_t rx_lock_chg;
  /** Delay of zero crossing slicer, m1, wrt to data in PI codes */
  int8_t clk90;
  /** Delay of diagnostic/lms slicer, p1, wrt to data in PI codes */
  int8_t clkp1;
  /** Peaking Filter Main Settings */
  int8_t pf_main;
  /** Peaking Filter Hiz mode enable */
  int8_t pf_hiz;
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
  /** TX equalization FIR rpara tap weight */
  int8_t txfir_rpara;
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
  /** lane_reset_state **/
  uint8_t reset_state;
  /** uC stopped state **/
  uint8_t stop_state;
  /** Sigdet offset correction - positive **/
  uint8_t soc_pos;
  /** Sigdet offset correction - negative **/
  uint8_t soc_neg;
  /** Rate select */
  uint8_t rate_select;
  /* First read of dsc_one_hot_0 */
  uint16_t dsc_one_hot_0_0;
  /* Second read of dsc_one_hot_0 */
  uint16_t dsc_one_hot_0_1;
} merlin16_pcieg3_lane_state_st;

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
  int16_t  avg_tmon;
  /** Analog Resistor Calibration value */
  uint8_t  rescal;
  /** VCO Rate in MHz */
  uint16_t vco_rate_mhz;
  /**  Analog VCO Range */
  uint8_t  analog_vco_range;
  /** PLL Divider value.  (Same encoding as enum #merlin16_pcieg3_pll_div_enum.) */
  uint32_t pll_div;
  /** PLL Lock */
  uint8_t  pll_lock;
  uint8_t  pll_lock_chg;
  /** Live die temperature in Celsius */
  int16_t die_temp;
  /** Core Status Variable */
  uint8_t core_status;
  /** Rate select */
  uint8_t rate_select;
} merlin16_pcieg3_core_state_st;

/****************************************************************************
 * @name Direct RAM Access
 *
 * Cores/chips with a built-in microcontroller afford direct, memory-mapped
 * access to the firmware control/status RAM variables.
 */
/**@{*/


/** Base of core variable block, MERLIN16 variant. */
#define CORE_VAR_RAM_BASE (0x400)
/** Base of lane variable block, MERLIN16 variant. */
#define LANE_VAR_RAM_BASE (0x500)
/** Size of lane variable block, MERLIN16 variant. */
#define LANE_VAR_RAM_SIZE (0x100)


/**@}*/

typedef struct {
    uint32_t signature;
    uint32_t diag_mem_ram_base;
    uint32_t diag_mem_ram_size;
    uint32_t core_var_ram_base; /* Must be the same as CORE_VAR_RAM_BASE */
    uint32_t core_var_ram_size;
    uint32_t lane_var_ram_base; /* Must be the same as LANE_VAR_RAM_BASE */
    uint32_t lane_var_ram_size; /* Must be the same as LANE_VAR_RAM_SIZE */
    uint32_t trace_mem_ram_base;
    uint32_t trace_mem_ram_size;
    uint32_t micro_var_ram_base;
    uint8_t  lane_count;
    uint8_t  trace_memory_descending_writes;
    uint8_t  micro_count;
    uint8_t  micro_var_ram_size;
    uint16_t grp_ram_size;
    } merlin16_pcieg3_info_t;

#define SRDS_INFO_SIGNATURE (0x50c1ab1e)


/** Retrieve the num_bits_per_ms useful for BER calculations
 * There is some Error in the calculation because only VCO rate in Mhz is stored.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *num_bits_per_ms is pointer to uint64_t which is used to store the num_bits_per_ms
 * @return Serdes Info pointer
 */
err_code_t merlin16_pcieg3_INTERNAL_get_num_bits_per_ms(srds_access_t *sa__, uint32_t *num_bits_per_ms);

/** Check PRBS status and print BER 
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param time_ms is the amount of time to delay for BER calculation
 * @return Serdes Info pointer
 */
err_code_t merlin16_pcieg3_INTERNAL_display_BER(srds_access_t *sa__, uint16_t time_ms);

/** Check PRBS status and print BER 
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param time_ms is the amount of time to delay for BER calculation
 * @param string is a pointer to char array of length >=10 in which to place string.
 * @return Serdes Info pointer
 */
err_code_t merlin16_pcieg3_INTERNAL_get_BER_string(srds_access_t *sa__, uint16_t time_ms, char *string);

/** Check PRBS status and provide number of errors and number of bits for BER calculation 
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param time_ms is the amount of time to delay for BER calculation
 * @param ber_data is a struct that contains number of errors and number of bits as elements
 * @return Error code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_BER_data(srds_access_t *sa__, uint16_t time_ms, struct ber_data_st *ber_data);

/** Get Serdes Info pointer
 * @return Serdes Info pointer
 */
merlin16_pcieg3_info_t *merlin16_pcieg3_INTERNAL_get_merlin16_pcieg3_info_ptr(void);

/** Verify the internal merlin16_pcieg3_info.
 * @return Error code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_verify_merlin16_pcieg3_info(merlin16_pcieg3_info_t const *test_info);

/** Read AFE HW Version.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *afe_hw_version AFE HW Version used
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_afe_hw_version(srds_access_t *sa__, uint8_t *afe_hw_version);

/** Check if the micro's operations on that lane are stopped.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return err_code Error Code "ERR_CODE_UC_NOT_STOPPED" returned if micro NOT stopped
 */
err_code_t merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(srds_access_t *sa__);

/** Calculate the mode_sel parameter for tx pattern generator.
 * @param *mode_sel Mode select to be used for generating required pattern
 * @param *zero_pad_len Length of zero padding to be used for generating required pattern
 * @param patt_length Desired Pattern length
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_calc_patt_gen_mode_sel(uint8_t *mode_sel, uint8_t *zero_pad_len, uint8_t patt_length);


/*-------------------*/
/*  Ladder controls  */
/*-------------------*/
/** Converts a ladder setting to mV, given the range.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ctrl is the threshold control (-31..31) maps to -RANGE to RANGE in non-uniform steps
 * @param range_250 determines the range 0 = +/-150mV, 1 = +/-250mV
 * @return ladder threshold voltage in mV
 */
int16_t merlin16_pcieg3_INTERNAL_ladder_setting_to_mV(srds_access_t *sa__, int8_t ctrl, uint8_t range_250);

/** Get the P1 slicer vertical level.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val 8 bit signed value
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_p1_threshold(srds_access_t *sa__, int8_t *val);


/*-----------------------*/
/*  TX_PI and ULL Setup  */
/*-----------------------*/


/*-----------------------------------------*/
/*  APIs used in Config Shared TX Pattern  */
/*-----------------------------------------*/

/** Compute Binary string for a Hex value ['0' to 'F'].
 * @param var Hex value to be converted to Binary (eg: '6', 'A', ...)
 * @param bin Binary string returned by API (eg: '0110', '1010', ...)
 * @return Error Code generated by invalid hex variable (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_compute_bin(char var, char bin[]);

/** Compute Hex value for a Binary string ['0000' to '1111'].
 * @param bin Binary string to be coverted (eg: '0110', '1010', ...)
 * @param *hex Hex value calculated from the input Binary string
 * @return Error Code generated by invalid Binary string (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_compute_hex(char bin[], uint8_t *hex);

/** Check micro state and request micro to stop
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param graceful select the method for stopping (1=graceful stop; 0=Immediate)
 * @param *err_code_p pointer to err_code variable which will be returned.
  */
uint8_t merlin16_pcieg3_INTERNAL_stop_micro(srds_access_t *sa__, uint8_t graceful, err_code_t *err_code_p);
/*-----------------------------------*/
/*  APIs used in Read Event Logger   */
/*-----------------------------------*/

/* State variable for dumping event log.
 * This must be initialized with zeros.
 */
typedef struct {
    uint16_t index, line_start_index;
} merlin16_pcieg3_INTERNAL_event_log_dump_state_t;

/** Callback function for dumping event log, intended to be called by merlin16_pcieg3_INTERNAL_read_event_log_with_callback.
 *  It must also be called once after, with a byte count of 0, to finish printing.
 * @param *arg is a merlin16_pcieg3_INTERNAL_event_log_dump_state_t pointer for holding state.
 * @param byte_count is the number of bytes to dump.
 *        -# This function is called repeatedly with a byte count of 2 with U16 values to dump.
 *        -# This function may then be called with a byte count of 1 if the event log has an odd number of bytes.
 *        -# Finally, this function must be called with a byte count of 0 to finish up.
 * @param data is the event log data to dump.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_event_log_dump_callback(void *arg, uint8_t byte_count, uint16_t data);

/** Get Event Log from uC, and call callback for every two bytes.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param micro_num is the number of the microcontroller whose event log should be read.
 * @param bypass_micro is 1 if the micro should not be involved (by stopping event log).
 *                     This mode works even when the micro is hung, and it does not consume the event log.
 * @param *arg is passed as the first argument to callback
 * @param *callback is called with all of the data read, two bytes at a time.
 *                  The last call of callback may have one byte; in that case, the upper byte is undefined.
 *                  The call is in the form:  callback(arg, byte_count, data)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_read_event_log_with_callback(srds_access_t *sa__,
                                                        uint8_t micro_num,
                                                        uint8_t bypass_micro,
                                                        void *arg,
                                                        err_code_t (*callback)(void *, uint8_t, uint16_t));

/** Convert float8 to usigned int32.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input Float8 number
 * @return Usigned 32bit number
 */
uint32_t merlin16_pcieg3_INTERNAL_float8_to_int32(float8_t input);

/** Convert uint8_t to 8-bit gray code.
 * @param input Unsigned 8-bit number
 * @return 8-bit gray code number
 */
uint8_t merlin16_pcieg3_INTERNAL_uint8_to_gray(uint8_t input);

/** Convert 8-bit gray code to uint8_t.
 * @param input 8-bit gray code number
 * @return Unsigned 8-bit number
 */
uint8_t merlin16_pcieg3_INTERNAL_gray_to_uint8(uint8_t input);


#ifdef TO_FLOATS
/*-----------------------------------*/
/*  APIs used in uC data conversion  */
/*-----------------------------------*/

/** Convert usigned int32 to float8.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY
 * @param input Unsigned int
 * @return Float8 8 bit representations of 32bit number
 */
float8_t merlin16_pcieg3_INTERNAL_int32_to_float8(uint32_t input);
#endif

/** Convert float12 to usigned int32.
 * uint32 = XXXXXXXX * 2^YYYY  where float12 bits X=byte and Y=multi
 * @param byte Float8 8bit
 * @param multi 4 bit multipier
 * @return Usigned 32bit number
 */
uint32_t merlin16_pcieg3_INTERNAL_float12_to_uint32(uint8_t byte, uint8_t multi);

#ifdef TO_FLOATS
/** Convert usigned int32 to float12.
 * uint32 = XXXXXXXX * 2^YYYY  where float12 bits X=byte and Y=multi
 * @param input Unsigned int32
 * @param *multi Pointer to byte and the multiplier is returned
 * @return Float12 8 bit representations of 32bit number
 */
uint8_t merlin16_pcieg3_INTERNAL_uint32_to_float12(uint32_t input, uint8_t *multi);
#endif

/*-----------------------------*/
/*  Read / Display Core state  */
/*-----------------------------*/
/** Read current merlin16_pcieg3 core status.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *istate Current merlin16_pcieg3 core status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_read_core_state(srds_access_t *sa__, merlin16_pcieg3_core_state_st *istate);


/** Display current core state.
 * Reads and displays all important core state values.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_display_core_state_no_newline(srds_access_t *sa__);

/*-----------------------------*/
/*  Read / Display Lane state  */
/*-----------------------------*/
/** Read current merlin16_pcieg3 lane status.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *istate Current merlin16_pcieg3 lane status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_read_lane_state(srds_access_t *sa__, merlin16_pcieg3_lane_state_st *istate);


/*-----------------*/
/*  Get OSR mode   */
/*-----------------*/
/** Read current merlin16_pcieg3 lane status.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *mode Returns with the osr mode structure
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_osr_mode(srds_access_t *sa__, merlin16_pcieg3_osr_mode_st *mode);


/*-------------------*/
/*  Get LCK status   */
/*-------------------*/
/** Read current PMD Lock and PMD Lock change status of a lane.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *pmd_lock Current merlin16_pcieg3 lane pmd_rx_lock status read back and populated by the API
 * @param *pmd_lock_chg Current merlin16_pcieg3 lane pmd_rx_lock_change status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_pmd_lock_status(srds_access_t *sa__, uint8_t *pmd_lock, uint8_t *pmd_lock_chg);

/** Read current Signal_detect and Signal_detect change status of a lane.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *sig_det Current merlin16_pcieg3 lane signal_detect status read back and populated by the API
 * @param *sig_det_chg Current merlin16_pcieg3 lane signal_detect_change status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_sigdet_status(srds_access_t *sa__, uint8_t *sig_det, uint8_t *sig_det_chg);

/** Read current PLL Lock and PLL Lock change status of a core.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *pll_lock Current merlin16_pcieg3 lane pll_lock status read back and populated by the API
 * @param *pll_lock_chg Current merlin16_pcieg3 lane pll_lock_change status read back and populated by the API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_pll_lock_status(srds_access_t *sa__, uint8_t *pll_lock, uint8_t *pll_lock_chg);

/** Display current lane state.
 * Reads and displays all important lane state values.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_display_lane_state_no_newline(srds_access_t *sa__);


/** Serdes Core ClockGate.
 * Along with merlin16_pcieg3_core_clkgate(), all lanes should also be clock gated using merlin16_pcieg3_lane_clkgate() to complete a Core Clockgate
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable clockgate (1 = Enable clokgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_core_clkgate(srds_access_t *sa__, uint8_t enable);

/** Serdes Lane ClockGate.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable lane clockgate (1 = Enable clockgate; 0 = Disable clockgate)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_lane_clkgate(srds_access_t *sa__, uint8_t enable);


/*--------------------*/
/*  Get/Set PF status */
/*--------------------*/
/** Set function for PF.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val Signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_pf_main(srds_access_t *sa__, uint8_t val);

/** Get function for PF
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val PF read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_pf_main(srds_access_t *sa__, int8_t *val);

/** Set function for PF2.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_pf2(srds_access_t *sa__, uint8_t val);

/** Get function for PF2.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val PF2 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_pf2(srds_access_t *sa__, int8_t *val);



/*-------------------*/
/*  Get/Set VGA/DFE  */
/*-------------------*/
/** Set function for VGA.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_vga(srds_access_t *sa__, uint8_t val);

/** Get function for VGA.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val VGA read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_vga(srds_access_t *sa__, int8_t *val);


/** Set function for DFE Tap1.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe1(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap1.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val DFE1 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe1(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap2.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe2(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap2.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val DFE2 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe2(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap3.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe3(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap3.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val DFE3 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe3(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap4.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe4(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap4.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val DFE4 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe4(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap5.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param val signed input value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe5(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap5
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *val DFE5 read value
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe5(srds_access_t *sa__, int8_t *val);


/*----------------------*/
/*  Get/Set TXFIR Taps  */
/*----------------------*/


uint8_t merlin16_pcieg3_INTERNAL_rdb_uc_var(srds_access_t *sa__, err_code_t *err_code_p, uint16_t addr);
uint16_t merlin16_pcieg3_INTERNAL_rdw_uc_var(srds_access_t *sa__, err_code_t *err_code_p, uint16_t addr);
err_code_t merlin16_pcieg3_INTERNAL_wrb_uc_var(srds_access_t *sa__, uint16_t addr, uint8_t wr_val);
err_code_t merlin16_pcieg3_INTERNAL_wrw_uc_var(srds_access_t *sa__, uint16_t addr, uint16_t wr_val);

/*--------------------------*/
/*  Register field polling  */
/*--------------------------*/

err_code_t merlin16_pcieg3_INTERNAL_print_uc_dsc_error(srds_access_t *sa__, enum srds_pmd_uc_cmd_enum cmd);

/** Polls lane variable "usr_diag_status" to verify data is available in uC diag buffer.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in merlin16_pcieg3_internal.c.
 *
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *status returns a status byte \n
 *    bit 15 - indicates the ey scan is complete \n
 *    bit 14-0 - reserved for debug
 *
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_poll_diag_done(srds_access_t *sa__, uint16_t *status, uint32_t timeout_ms);

/** Polls for register field "uc_dsc_ready_for_cmd" to be 1 within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in merlin16_pcieg3_internal.c.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @param cmd The command that was issued; this is used for error reporting.
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_poll_uc_dsc_ready_for_cmd_equals_1(srds_access_t *sa__, uint32_t timeout_ms, enum srds_pmd_uc_cmd_enum cmd);

/** Polls for register field "dsc_state" to be "DSC_STATE_UC_TUNE"
 * within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in merlin16_pcieg3_internal.c.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_poll_dsc_state_equals_uc_tune(srds_access_t *sa__, uint32_t timeout_ms);


/** Polls for register field "micro_ra_initdone" to be 1 within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in merlin16_pcieg3_internal.c.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_poll_micro_ra_initdone(srds_access_t *sa__, uint32_t timeout_ms);


/*--------------------------------*/
/*  RAM Block Read with Callback  */
/*--------------------------------*/

/* Utilities to account for endianness when reading from uc RAM */
static const int merlin16_pcieg3_endian_const = 1;
#define is_big_endian() ((*(char*)&merlin16_pcieg3_endian_const) == 0)
#define get_endian_offset(addr) (is_big_endian() ? \
                                (\
                                (((USR_UINTPTR)(addr))%4 == 0) ?  3 : \
                                (((USR_UINTPTR)(addr))%4 == 1) ?  1 : \
                                (((USR_UINTPTR)(addr))%4 == 2) ? -1 : \
                                (((USR_UINTPTR)(addr))%4 == 3) ? -3 : \
                                0) \
                                : 0)

typedef struct {
    uint8_t *mem_ptr;
} merlin16_pcieg3_INTERNAL_rdblk_callback_arg_t;

/** Callback function for reading uc RAM memory by merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram().
 * @param *arg is a merlin16_pcieg3_INTERNAL_rdblk_callback_arg_t pointer for holding state.
 * @param byte_count is the number of bytes to store.
 *        -# This function is called repeatedly with a byte count of 2 with U16 values to store.
 *        -# This function may then be called with a byte count of 1 if there are an odd number of bytes.
 * @param data is the RAM data to store.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_rdblk_callback(void *arg, uint8_t byte_count, uint16_t data);

/** Block RAM read through Micro Register Interface, suitable for program RAM or non-program RAM.
 *  This reads a block of RAM, possibly starting midway through it and wrapping back to the beginning.
 *  The order of operations is:
 *      -# RAM will be read from (block_addr+start_offset) to (block_addr+block_size-1),
 *         up to cnt bytes.
 *      -# RAM will be read from block_addr to (block_addr+block_size-1), repeating
 *         indefinitely, until cnt bytes total are read.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param block_addr Address of RAM block to be read.
 *                   For program RAM, the upper 16 bits are 0x0000.  Otherwise, the upper 16 bits are 0x2000.
 * @param block_size Length of RAM block to read.
 * @param start_offset Offset within RAM block to start reading
 * @param cnt Number of bytes to be read
 * @param *arg is passed as the first argument to callback
 * @param *callback is called with all of the data read, two bytes at a time.
 *                  The last call of callback may have one byte; in that case, the upper byte is undefined.
 *                  The call is in the form:  callback(arg, byte_count, data)
 * @return Error Code generated by API or the callback (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram(srds_access_t *sa__,
                                                uint32_t block_addr,
                                                uint16_t block_size,
                                                uint16_t start_offset,
                                                uint16_t cnt,
                                                void *arg,
                                                err_code_t (*callback)(void *, uint8_t, uint16_t));

/** Block RAM read through Micro Register Interface, suitable for program RAM or non-program RAM.
 *  This reads a block of RAM, possibly starting midway through it and wrapping back to the beginning.
 *  The order of operations is:
 *      -# RAM will be read from (block_addr+start_offset) down to block_addr, up to cnt bytes.
 *      -# RAM will be read from (block_addr+block_size-1) down to block_addr, repeating
 *         indefinitely, until cnt bytes total are read.
 *  In other words, this is the same as merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram(), but reading in descending address order.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param block_addr Address of RAM block to be read.
 *                   For program RAM, the upper 16 bits are 0x0000.  Otherwise, the upper 16 bits are 0x2000.
 * @param block_size Length of RAM block to read.
 * @param start_offset Offset within RAM block to start reading
 * @param cnt Number of bytes to be read
 * @param *arg is passed as the first argument to callback
 * @param *callback is called with all of the data read, two bytes at a time.
 *                  The last call of callback may have one byte; in that case, the upper byte is undefined.
 *                  The call is in the form:  callback(arg, byte_count, data)
 * @return Error Code generated by API or the callback (returns ERR_CODE_NONE if no errors)
 */
err_code_t merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram_descending(srds_access_t *sa__,
                                                           uint32_t block_addr,
                                                           uint16_t block_size,
                                                           uint16_t start_offset,
                                                           uint16_t cnt,
                                                           void *arg,
                                                           err_code_t (*callback)(void *, uint8_t, uint16_t));

/** Retrieve micro index based on lane index.
 * @param lane Lane index
 * @return Micro index
 */
uint8_t merlin16_pcieg3_INTERNAL_grp_idx_from_lane(uint8_t lane);



/*-------------------*/
/*  Name resolution  */
/*-------------------*/

/* For bitfields that differ for different cores, these macros are defined to resolve the bitfield. */

#if defined(wr_rx_pf_ctrl)
#   define WR_RX_PF_CTRL  wr_rx_pf_ctrl
#   define RD_RX_PF_CTRL  rd_rx_pf_ctrl
#   define WR_RX_PF2_CTRL wr_rx_pf2_ctrl
#   define RD_RX_PF2_CTRL rd_rx_pf2_ctrl
#elif defined(wr_rx_pf_ctrl_val)
#   define WR_RX_PF_CTRL  wr_rx_pf_ctrl_val
#   define RD_RX_PF_CTRL  rd_rx_pf_ctrl_val
#   define WR_RX_PF2_CTRL wr_rx_pf2_ctrl_val
#   define RD_RX_PF2_CTRL rd_rx_pf2_ctrl_val
#elif defined(wr_pf_ctrl)
#   define WR_RX_PF_CTRL  wr_pf_ctrl
#   define RD_RX_PF_CTRL  rd_pf_ctrl
#   define WR_RX_PF2_CTRL wr_pf2_lowp_ctrl
#   define RD_RX_PF2_CTRL rd_pf2_lowp_ctrl
#else
#   error "Could not find write to pf_ctrl bitfield."
#endif

#if defined(reg_rd_TLB_RX_RXPRBS_CHK_ERR_CNT_MSB_STATUS)
#   define REG_RD_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS reg_rd_TLB_RX_RXPRBS_CHK_ERR_CNT_MSB_STATUS
#   define REG_RD_TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS reg_rd_TLB_RX_RXPRBS_CHK_ERR_CNT_LSB_STATUS
#elif defined(reg_rd_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS)
#   define REG_RD_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS reg_rd_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS
#   define REG_RD_TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS reg_rd_TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS
#else
#   error "Could not find PRBS_CHK_ERR_CNT_MSB_STATUS."
#endif

#endif
