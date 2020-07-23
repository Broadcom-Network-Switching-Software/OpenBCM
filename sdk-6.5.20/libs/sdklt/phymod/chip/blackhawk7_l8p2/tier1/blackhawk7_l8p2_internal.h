/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/**********************************************************************************
 **********************************************************************************
 *                                                                                *
 *  Revision    :   *
 *                                                                                *
 *  Description :  Internal API functions                                         *
 */

/** @file blackhawk7_l8p2_internal.h
 * Internal API functions
 */
#ifdef __cplusplus
extern "C" {
#endif


#ifndef BLACKHAWK7_L8P2_API_INTERNAL_H
#define BLACKHAWK7_L8P2_API_INTERNAL_H

#include "blackhawk7_l8p2_ipconfig.h"
#include "blackhawk7_l8p2_select_defns.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_uc_common.h"
#include "common/srds_api_types.h"
#include "blackhawk7_l8p2_enum.h"
#include "blackhawk7_l8p2_types.h"
#include "blackhawk7_l8p2_usr_includes.h"
#include "blackhawk7_l8p2_prbs.h"

/*--------------------------*/
/** Serdes RX PAM Mode Enum */
/*--------------------------*/
enum blackhawk7_l8p2_rx_pam_mode_enum {
    NRZ,
    PAM4_NR,
    PAM4_ER
};

/*---------------------------------------*/
/** Serdes prbs chk timer selection Enum */
/*---------------------------------------*/
enum blackhawk7_l8p2_prbs_chk_timer_selection_enum {
    USE_HW_TIMERS = 0,
    USE_SW_TIMERS = 1
};

/*---------------------------------------*/
/** Conversion for DFE tap 5-14 encoding */
/*---------------------------------------*/
/** DFE TAP 5-14 Signed-Gray-Binary to Signed-Gray encoding */
#  define DFE_TAP_SGB2SG(x)                             \
                    (((x)==2) ? 3                       \
                   : (((x)==3) ? 2                      \
                    : (((x)==6) ? 7                     \
                     : (((x)==7) ? 6                    \
                      : (((x)==-2) ? -3                 \
                       : (((x)==-3) ? -2                \
                        : (((x)==-6) ? -7               \
                         : (((x)==-7) ? -6 : (x)))))))))

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
}blackhawk7_l8p2_osr_mode_st;

/*------------------------------*/
/** Serdes Lane State Structure */
/*------------------------------*/
typedef struct {
  /** uC lane configuration */
  uint16_t ucv_config;
  /** uC lane status */
  uint8_t ucv_status;
  /** Extended uC lane status */
  uint16_t ucv_status_ext;
  /** Frequency offset of local reference clock with respect to RX data in ppm */
  int16_t rx_ppm;
  /** Vertical threshold voltage of p1 slicer (mV) */
  int16_t p1_lvl;
  /** Vertical threshold voltage of m1 slicer (mV) [Used to read out 'channel loss hint' in PAM4 mode] */
  int16_t m1_lvl;
  /** blind ctle ctrl */
  uint8_t blind_ctle_ctrl;
  /** blind_ctle_trnsum */
  int16_t blind_ctle_trnsum;
  /** AFE bw metric */
  int16_t afe_bw_metric;
  /** Tuninig Parameter 2 */ 
  uint8_t afe_bw;
  /** Tuninig parameter 3 */ 
  int8_t usr_status_eq_debug1;
  /** TP metric prec1 */
  int8_t tp_metric_prec1;
  /** Link time in milliseconds */
  uint32_t link_time;
  /** OSR Mode */
  blackhawk7_l8p2_osr_mode_st osr_mode;
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
  uint8_t pf_hiz;
  /** Low Frequency Peaking filter control */
  int8_t pf2_ctrl;
  /** High Frequency Peaking filter control */
  int8_t pf3_ctrl;
  /** BW Reduction control */
  uint8_t bwr_ctrl;
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
  /** RX PAM4 Mode */
  uint8_t rx_pam_mode;
  /** Frequency offset of local reference clock with respect to TX data in ppm */
  int16_t tx_ppm;
  /** TX Link Training Enable */
  uint8_t linktrn_en;
  /** TX Disable Status  */
  uint8_t tx_disable_status;
  /** TX Precoder Enable */
  uint8_t tx_prec_en;
  /** TX equalization FIR tap range */
  uint8_t txfir_use_pam4_range;
  /** TX equalization FIR tap weights */
  blackhawk7_l8p2_txfir_st txfir;
  /** TX equalization FIR pre2 tap weight */
  int16_t txfir_pre2;
  /** TX equalization FIR pre1 tap weight */  
  int16_t txfir_pre1;
  /** TX equalization FIR main tap weight */
  int16_t txfir_main;
  /** TX equalization FIR post1 tap weight */
  int16_t txfir_post1;
  /** TX equalization FIR post2 tap weight */
  int16_t txfir_post2;
  /** TX equalization FIR post3 tap weight */
  int16_t txfir_post3;
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
  /** lane_tx_reset_state **/
  uint8_t tx_reset_state;
  /** uC stopped state **/
  uint8_t stop_state;
  /** Selected PLL index for TX **/
  uint8_t tx_pll_select;
  /** Selected PLL index for RX **/
  uint8_t rx_pll_select;
  /** Sigdet offset correction - positive **/
  uint8_t soc_pos;
  /** Sigdet offset correction - negative **/
  uint8_t soc_neg;
} blackhawk7_l8p2_lane_state_st;

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
  /** API Version number*/
  uint32_t api_version;
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
  /** PLL Divider value.  (Same encoding as enum #blackhawk7_l8p2_pll_div_enum.) */
  uint32_t pll_div;
  /** PLL Lock */
  uint8_t  pll_lock;
  /** PLL Lock Change*/
  uint8_t  pll_lock_chg;
  /** Live die temperature in Celsius */
  int16_t die_temp;
  /** Core Status Variable */
  uint8_t core_status;
} blackhawk7_l8p2_core_state_st;


#define RESCAL_MIN  (3)
#define RESCAL_MAX (13)

#define SRDS_INFO_SIGNATURE (0x50c1ab1e)

/* SW Limits for TXFIR Taps in PAM4 mode */
#define TXFIR_PAM4_SW_TAP_MIN   (-170)
#define TXFIR_PAM4_SW_TAP_MAX   ( 170)
#define TXFIR_PAM4_SW_SUM_LIMIT ( 170)

#ifndef SMALL_FOOTPRINT
/** Retrieve the num_bits_per_ms useful for BER calculations.
 * There is some Error in the calculation because only VCO rate in Mhz is stored.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] select_rx a flag to select rx or tx (1 -> rx, 0 -> tx).
 * @param[out] num_bits_per_ms is pointer to uint64_t which is used to store the num_bits_per_ms.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_num_bits_per_ms(srds_access_t *sa__, uint8_t select_rx, uint32_t *num_bits_per_ms);

/** Check PRBS status and print BER.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] time_ms is the amount of time to delay for BER calculation.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_display_BER(srds_access_t *sa__, uint16_t time_ms);

/** Check PRBS status and print BER.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] time_ms is the amount of time to delay for BER calculation.
 * @param[out] string is a pointer to char array of length >=10 in which to place string.
 * @param[in] string_size is the size of string in bytes.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_BER_string(srds_access_t *sa__, uint16_t time_ms, char *string, uint8_t string_size);

/** Check PRBS status and provide number of errors and number of bits for BER calculation.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] time_ms is the amount of time to delay for BER calculation.
 * @param[out] ber_data is a struct that will get populated with number of errors and number of bits as elements.
 * @param[in] timer_sel allows to measure BER using host computer time instead of using hardware timers.
 * @return Error code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_BER_data(srds_access_t *sa__, uint16_t time_ms, struct ber_data_st *ber_data, enum blackhawk7_l8p2_prbs_chk_timer_selection_enum timer_sel);

/** Check CDR lost lock.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] cdrlcklost is set if CDR loses lock.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_prbs_chk_cdr_lock_lost(srds_access_t *sa__, uint8_t *cdrlcklost);

#define PRBS_MAX_HW_TIMER_TIMEOUT           (1022)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_DISABLED (0x0)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_1US      (0x1)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_10US     (0x2)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_100US    (0x3)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_1MS      (0x4)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_10MS     (0x5)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_100MS    (0x6)
#define PRBS_CHK_EN_TIMER_UNIT_SEL_RESERVED (0x7)

/** Get timeout register value from time in ms value.
 * @param[in] time_ms is the amount of time to delay for BER calculation requested by a user.
 * @param[out] time_ms_adjusted is the amount of time to delay for BER calculation available by design (>= time_ms).
 * @param[out] prbs_chk_hw_timer_ctrl is a struct to hold timer mode and timeout value that needs to be programmed to corresponding registers.
 * @return Error code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_prbs_timeout_count_from_time(uint16_t time_ms, uint16_t * time_ms_adjusted, struct prbs_chk_hw_timer_ctrl_st * const prbs_chk_hw_timer_ctrl);

/** Function to add delay to account for hardware synchronization.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_hw_timer_sync_delay(srds_access_t *sa__);
#endif /* SMALL_FOOTPRINT */

/** Get Serdes Info pointer.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Serdes Info pointer.
 */
srds_info_t *blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr(srds_access_t *sa__);

/** Get Serdes Info pointer and initialize it if signature doesn't match.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Serdes Info pointer.
 */
srds_info_t *blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr_with_check(srds_access_t *sa__);

/** Checks to see if the stored ucode_version in blackhawk7_l8p2_info is matching with the current thread ucode_version.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] blackhawk7_l8p2_info_ptr pointer to Info_table to be verified.
 * @return Error Code generated if ucode_version does not match or uC rdvariable caused issues (else returns ERR_CODE_NONE).
 */
err_code_t blackhawk7_l8p2_INTERNAL_match_ucode_from_info(srds_access_t *sa__, srds_info_t const *blackhawk7_l8p2_info_ptr);

/** Verify the internal blackhawk7_l8p2_info.
 * NOTE: This function is getting deprecated. Instead, please use the functions
 * blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr_with_check and blackhawk7_l8p2_INTERNAL_match_ucode_from_info
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] blackhawk7_l8p2_info_ptr pointer to Info_table to be verified.
 * @return Error code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_verify_blackhawk7_l8p2_info(srds_access_t *sa__, srds_info_t const *blackhawk7_l8p2_info_ptr);

#ifndef SMALL_FOOTPRINT

/** Check if the micro's operations on that lane are stopped.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return err_code Error Code "ERR_CODE_UC_NOT_STOPPED" returned if micro NOT stopped.
 */
err_code_t blackhawk7_l8p2_INTERNAL_check_uc_lane_stopped(srds_access_t *sa__);

/** Calculate the mode_sel parameter for tx pattern generator.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] mode_sel Mode select to be used for generating required pattern.
 * @param[out] zero_pad_len Length of zero padding to be used for generating required pattern.
 * @param[in] patt_length Desired Pattern length.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_calc_patt_gen_mode_sel(srds_access_t *sa__, uint8_t *mode_sel, uint8_t *zero_pad_len, uint8_t patt_length);
#endif /*SMALL_FOOTPRINT */

/*-----------------------------------------*/
/*  Write Core Config variables to uC RAM  */
/*-----------------------------------------*/

/** Write to core_config uC RAM variable.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] struct_val Value to be written into core_config RAM variable
 *                   (Note that struct_val.word must be = 0, only the fields are used).
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_uc_core_config(srds_access_t *sa__, struct blackhawk7_l8p2_uc_core_config_st struct_val);

/*---------------------*/
/*  PLL Configuration  */
/*---------------------*/

/** Return whether the fraction portion of a #blackhawk7_l8p2_pll_div_enum value is nonzero. */
#define SRDS_INTERNAL_IS_PLL_DIV_FRACTIONAL(div_) (((uint32_t)(div_) & 0xFFFFF000UL) != 0)

/** Extract the integer portion of a #blackhawk7_l8p2_pll_div_enum value. */
#define SRDS_INTERNAL_GET_PLL_DIV_INTEGER(div_) ((uint32_t)(((uint32_t)(div_)) & 0x00000FFFUL))

/** Extract the fraction portion of a #blackhawk7_l8p2_pll_div_enum value.
 *  The result would be suitable for the numerator of a fraction whose denominator is 2^(width_).
 *  width_ must be less than 32.
 */
#define SRDS_INTERNAL_GET_PLL_DIV_FRACTION_NUM(div_, width_)            \
    ((uint32_t)((((((uint32_t)(div_) & 0xFFFFF000UL) >> (32-(width_)-1)) + 1) >> 1)))

/** Compose a value in the same format as a #blackhawk7_l8p2_pll_div_enum value.
 *  (Though it's the same format, a U32 is used, because rounding may yield a non-enumerated result.)
 *  The fractional portion is fraction_num / (2^fraction_num_width_).
 *  fraction_num_width_ must be 32 or less.
 */
#define SRDS_INTERNAL_COMPOSE_PLL_DIV(integer_, fraction_num_, fraction_num_width_)                   \
     ((uint32_t)((integer_) & 0x00000FFFUL)                                                                 \
     | (uint32_t)((((((fraction_num_) << (32-(fraction_num_width_))) >> (32-20-1)) + 1) >> 1) << 12))


/** Resolve PLL parameters.
 * PLL configuration requires two out of the three parameters:  refclk frequency, divider value, and VCO output frequency.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] refclk Reference clock frequency (enumerated), or BLACKHAWK7_L8P2_PLL_REFCLK_UNKNOWN to auto-compute.
 * @param[in,out] refclk_freq_hz Resultant reference clock frequency, in Hz.
 * @param[in,out] srds_div Divider value (enumerated), or BLACKHAWK7_L8P2_PLL_DIV_UNKNOWN to auto-compute.
 *             The final divider value will be filled in if BLACKHAWK7_L8P2_PLL_DIV_UNKNOWN.
 * @param[in,out] vco_freq_khz VCO output frequency, in kHz, or 0 to auto-compute.
 *                      The final VCO output frequency will be filled in if 0.
 * @param[in] pll_option Select PLL configuration option from enum #blackhawk7_l8p2_pll_option_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_resolve_pll_parameters(srds_access_t *sa__,
                                                  enum blackhawk7_l8p2_pll_refclk_enum refclk,
                                                  uint32_t *refclk_freq_hz,
                                                  enum blackhawk7_l8p2_pll_div_enum *srds_div,
                                                  uint32_t *vco_freq_khz,
                                                  enum blackhawk7_l8p2_pll_option_enum pll_option);

#ifndef SMALL_FOOTPRINT
/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration requires two out of the three parameters:  refclk frequency, divider value, and VCO output frequency.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] refclk Reference clock frequency (enumerated), or BLACKHAWK7_L8P2_PLL_REFCLK_UNKNOWN to auto-compute.
 * @param[in] srds_div Divider value (enumerated), or BLACKHAWK7_L8P2_PLL_DIV_UNKNOWN to auto-compute.
 * @param[in] vco_freq_khz VCO output frequency, in kHz, or 0 to auto-compute.
 * @param[in] pll_option Select PLL configuration option from enum #blackhawk7_l8p2_pll_option_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
#else
 /** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration requires all four parameters:  refclk frequency, divider value, VCO output frequency and the PLL options.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] refclk Reference clock frequency (enumerated), or BLACKHAWK7_L8P2_PLL_REFCLK_UNKNOWN to auto-compute.
 * @param[in] srds_div Divider value (enumerated).
 * @param[in] vco_freq_khz VCO output frequency, in kHz.
 * @param[in] pll_option Select PLL configuration option from enum #blackhawk7_l8p2_pll_option_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
#endif

err_code_t blackhawk7_l8p2_INTERNAL_configure_pll(srds_access_t *sa__,
                                         enum blackhawk7_l8p2_pll_refclk_enum refclk,
                                         enum blackhawk7_l8p2_pll_div_enum srds_div,
                                         uint32_t vco_freq_khz,
                                         enum blackhawk7_l8p2_pll_option_enum pll_option);


/** Read the programmed PLL div value from the SERDES.
 *  Due to rounding when writing the div value to the PLL,
 *      the returned result may be slightly different from what was written.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] srds_div Divider value, in the same encoding as enum #blackhawk7_l8p2_pll_div_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_read_pll_div(srds_access_t *sa__, uint32_t *srds_div);

/** Convert the PLL_DIV to actual PLL divider value and display
 * as part of blackhawk7_l8p2_display_core_state_line().
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] srds_div Divider value, in the same encoding as enum #blackhawk7_l8p2_pll_div_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_display_pll_to_divider(srds_access_t *sa__, uint32_t srds_div);

/** Get the VCO frequency in kHz, based on the reference clock frequency and divider value.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] refclk_freq_hz Reference clock frequency, in Hz.
 * @param[in] srds_div Divider value, in the same encoding as enum #blackhawk7_l8p2_pll_div_enum.
 * @param[out] vco_freq_khz VCO output frequency, in kHz, obtained based on reference clock frequency and div value.
 * @param[in] pll_option Select PLL configuration option from enum #blackhawk7_l8p2_pll_option_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_vco_from_refclk_div(srds_access_t *sa__, uint32_t refclk_freq_hz, enum blackhawk7_l8p2_pll_div_enum srds_div, uint32_t *vco_freq_khz, enum blackhawk7_l8p2_pll_option_enum pll_option);

#ifndef SMALL_FOOTPRINT

/*-------------------*/
/*  Ladder controls  */
/*-------------------*/
/** Converts a ladder setting to mV, given the range.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] ctrl is the threshold control (-31..31) maps to -RANGE to RANGE in non-uniform steps.
 * @param[in] range_250 determines the range 0 = +/-150mV, 1 = +/-250mV (ignored for OSPREY7).
 * @param[in] slicer_sel slicer for which ladder threshold needs to be calculated. (Required for OSPREY7, unused (can be zero) for all other IPs).
 * @param[out] nlmv_val updated with claculated ladder threshold voltage(mV).
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_ladder_setting_to_mV(srds_access_t *sa__, int16_t ctrl, uint8_t range_250, afe_override_slicer_sel_t slicer_sel, int16_t *nlmv_val);



/*-----------------------*/
/*  TX_PI and ULL Setup  */
/*-----------------------*/

/** Safe multiply - multiplies 2 numbers and checks for overflow.
 * @param[in] a First input.
 * @param[in] b Second input.
 * @param[out] of Pointer to overflow indicator.
 * @return value of a * b.
 */
uint32_t blackhawk7_l8p2_INTERNAL_mult_with_overflow_check(uint32_t a, uint32_t b, uint8_t *of);

/*-----------------------------------------*/
/*  APIs used in Config Shared TX Pattern  */
/*-----------------------------------------*/
/** Compute Binary string for a PAM4 symbol ['0'to '3'].
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] var PAM4 symbol to be converted to Binary ('0', '1', '2', or '3').
 * @param[out] bin Binary string returned by API ('00', '01', '10' or '11').
 * @return Error Code generated by invalid hex variable (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_pam4_to_bin(srds_access_t *sa__, char var, char bin[]);

/** Compute Binary string for a Hex value ['0' to 'F'].
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] var Hex value to be converted to Binary (eg: '6', 'A', ...).
 * @param[out] bin Binary string returned by API (eg: '0110', '1010', ...).
 * @return Error Code generated by invalid hex variable (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_compute_bin(srds_access_t *sa__, char var, char bin[]);

/** Compute Hex value for a Binary string ['0000' to '1111'].
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] bin Binary string to be coverted (eg: '0110', '1010', ...).
 * @param[out] hex Hex value calculated from the input Binary string.
 * @return Error Code generated by invalid Binary string (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_compute_hex(srds_access_t *sa__, char bin[], uint8_t *hex);

/** Check micro state and request micro to stop. Stop micro only if micro is not stopped currently.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param graceful select the method for stopping (1=graceful stop; 0=Immediate).
 * @param[out] err_code_p pointer to err_code variable which will be returned (returns ERR_CODE_NONE if no errors).
 * @return The previous micro stop status.
  */
uint8_t blackhawk7_l8p2_INTERNAL_stop_micro(srds_access_t *sa__, uint8_t graceful, err_code_t *err_code_p);
/*-----------------------------------*/
/*  APIs used in Read Event Logger   */
/*-----------------------------------*/

/* State variable for dumping event log.
 * This must be initialized with zeros.
 */
typedef struct {
    uint16_t index, line_start_index;
} blackhawk7_l8p2_INTERNAL_event_log_dump_state_t;

/** Callback function for dumping event log, intended to be called by blackhawk7_l8p2_INTERNAL_read_event_log_with_callback.
 *  It must also be called once after, with a byte count of 0, to finish printing.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in,out] arg is a blackhawk7_l8p2_INTERNAL_event_log_dump_state_t pointer for holding state.
 * @param[in] byte_count is the number of bytes to dump.
 *        -# This function is called repeatedly with a byte count of 2 with U16 values to dump.
 *        -# This function may then be called with a byte count of 1 if the event log has an odd number of bytes.
 *        -# Finally, this function must be called with a byte count of 0 to finish up.
 * @param[in] data is the event log data to dump.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_event_log_dump_callback(srds_access_t *sa__, void *arg, uint8_t byte_count, uint16_t data);

/** Get Event Log from uC, and call callback for every two bytes.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] micro_num is the number of the microcontroller whose event log should be read.
 * @param[in] bypass_micro is 1 if the micro should not be involved (by stopping event log).
 *                     This mode works even when the micro is hung, and it does not consume the event log.
 * @param[in,out] arg is passed as the first argument to callback.
 * @param[in] callback is called with all of the data read, two bytes at a time.
 *                  The last call of callback may have one byte; in that case, the upper byte is undefined.
 *                  The call is in the form:  callback(arg, byte_count, data).
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_read_event_log_with_callback(srds_access_t *sa__,
                                                        uint8_t micro_num,
                                                        uint8_t bypass_micro,
                                                        void *arg,
                                                        err_code_t (*callback)(srds_access_t *, void *, uint8_t, uint16_t));

/** Convert float8 to unsigned int32.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY.
 * @param[in] input Float8 number.
 * @return Unsigned 32bit number.
 */
uint32_t blackhawk7_l8p2_INTERNAL_float8_to_uint32(float8_t input);

/** Convert uint8_t to 8-bit gray code.
 * @param[in] input Unsigned 8-bit number.
 * @return 8-bit gray code number.
 */
uint8_t blackhawk7_l8p2_INTERNAL_uint8_to_gray(uint8_t input);

/** Convert 8-bit gray code to uint8_t.
 * @param[in] input 8-bit gray code number.
 * @return Unsigned 8-bit number.
 */
uint8_t blackhawk7_l8p2_INTERNAL_gray_to_uint8(uint8_t input);

/** Convert seconds to hr:min:sec format.
 * @param[in] seconds 32bit input for time in seconds.
 * @param[out] hrs    Number of hours.
 * @param[out] mins   Number of minutes.
 * @param[out] secs   Number of seconds.
 * @return Unsigned 8-bit number.
 */
uint8_t blackhawk7_l8p2_INTERNAL_seconds_to_displayformat(uint32_t seconds, uint8_t *hrs, uint8_t *mins, uint8_t *secs);

#ifdef TO_FLOATS
/*-----------------------------------*/
/*  APIs used in uC data conversion  */
/*-----------------------------------*/

/** Convert usigned int32 to float8.
 * uint32 = 1.XXX * 2^Y  where float8 bits are XXXYYYYY.
 * @param[in] input Unsigned int.
 * @return Float8 8 bit representations of 32bit number.
 */
float8_t blackhawk7_l8p2_INTERNAL_int32_to_float8(uint32_t input);
#endif

/** Convert float12 to usigned int32.
 * uint32 = XXXXXXXX * 2^YYYY  where float12 bits X=byte and Y=multi.
 * @param[in] input Float8 8bit.
 * @param[in] multi 4 bit multipier.
 * @return Usigned 32bit number.
 */
uint32_t blackhawk7_l8p2_INTERNAL_float12_to_uint32(uint8_t input, uint8_t multi);

#endif /* SMALL_FOOTPRINT */

/*-----------------------------*/
/*  Read / Display Core state  */
/*-----------------------------*/
/** Read current blackhawk7_l8p2 core status.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] istate Current blackhawk7_l8p2 core status read back and populated by the API.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_read_core_state(srds_access_t *sa__, blackhawk7_l8p2_core_state_st *istate);

#ifndef SMALL_FOOTPRINT
/** Display current core state.
 * Reads and displays all important core state values.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_display_core_state_no_newline(srds_access_t *sa__);

/*-----------------------------*/
/*  Read / Display Lane state  */
/*-----------------------------*/
/** Read current blackhawk7_l8p2 lane status.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] istate Current blackhawk7_l8p2 lane status read back and populated by the API.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_read_lane_state(srds_access_t *sa__, blackhawk7_l8p2_lane_state_st *istate);


/*-----------------*/
/*  Get OSR mode   */
/*-----------------*/
/** Read current blackhawk7_l8p2 lane status.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] mode Returns with the osr mode structure.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_osr_mode(srds_access_t *sa__, blackhawk7_l8p2_osr_mode_st *mode);
#endif /* SMALL_FOOTPRINT */

/*-------------------*/
/*  Get RX_PAM mode  */
/*-------------------*/
/** Read current RX PAM mode.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] pmode Returns the current RX PAM mode.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_pam_mode(srds_access_t *sa__, enum blackhawk7_l8p2_rx_pam_mode_enum *pmode);

#ifndef SMALL_FOOTPRINT
/*---------------------*/
/*  Decode BR/OS mode  */
/*---------------------*/
/** For PAM mode, read os_pattern_enhanced and os_all_edges.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] br_pd_en Returns the decoded BR/OS mode.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_decode_br_os_mode(srds_access_t *sa__, uint8_t *br_pd_en);
#endif /* SMALL_FOOTPRINT */

/*-------------------*/
/*  Get LCK status   */
/*-------------------*/
/** Read current Signal_detect and Signal_detect change status of a lane.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] sig_det Current blackhawk7_l8p2 lane signal_detect status read back and populated by the API.
 * @param[out] sig_det_chg Current blackhawk7_l8p2 lane signal_detect_change status read back and populated by the API.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_sigdet_status(srds_access_t *sa__, uint8_t *sig_det, uint8_t *sig_det_chg);

/** Serdes Core ClockGate.
 * Along with blackhawk7_l8p2_core_clkgate(), all lanes should also be clock gated using blackhawk7_l8p2_lane_clkgate() to complete a Core Clockgate.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] enable Enable clockgate (1 = Enable clokgate; 0 = Disable clockgate).
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_core_clkgate(srds_access_t *sa__, uint8_t enable);

#ifndef SMALL_FOOTPRINT
/** Read current PMD Lock and PMD Lock change status of a lane.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] pmd_lock Current blackhawk7_l8p2 lane pmd_rx_lock status read back and populated by the API.
 * @param[out] pmd_lock_chg Current blackhawk7_l8p2 lane pmd_rx_lock_change status read back and populated by the API.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_pmd_lock_status(srds_access_t *sa__, uint8_t *pmd_lock, uint8_t *pmd_lock_chg);

/** Read current PLL Lock and PLL Lock change status of a core.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] pll_lock Current blackhawk7_l8p2 lane pll_lock status read back and populated by the API.
 * @param[out] pll_lock_chg Current blackhawk7_l8p2 lane pll_lock_change status read back and populated by the API.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_pll_lock_status(srds_access_t *sa__, uint8_t *pll_lock, uint8_t *pll_lock_chg);

/** Obtain link time from status variable.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] link_time Link time measured in ms.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_link_time(srds_access_t *sa__, uint32_t *link_time);

/** Display current lane state.
 * Reads and displays all important lane state values.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_display_lane_state_no_newline(srds_access_t *sa__);

/*-----------------------------------------------*/
/*  Get dynamic eye margin estimation values     */
/*-----------------------------------------------*/
/** Get dynamic eye estimation values.
 * Reads and converts the Eye margin estimation from the uC.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] left_eye_mUI a pointer to integer with return value of eye margin in units of mUI.
 * @param[out] right_eye_mUI a pointer to integer with return value of eye margin in units of mUI.
 * @param[out] upper_eye_mV a pointer to integer with return value of eye margin in units of mV.
 * @param[out] lower_eye_mV a pointer to integer with return value of eye margin in units of mV.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_eye_margin_est(srds_access_t *sa__, uint16_t *left_eye_mUI, uint16_t *right_eye_mUI, uint16_t  *upper_eye_mV, uint16_t *lower_eye_mV);

/** Convert eye margin to mV.
 * Takes in the ladder setting with 3 fractional bits and converts to mV.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] var Ladder setting with 3 fractional bits.
 * @param[in] ladder_range Specified if ladder is configured for 150mV or 250mV range.
 * @return Eye opening in mV.
 */
uint16_t blackhawk7_l8p2_INTERNAL_eye_to_mV(srds_access_t *sa__, uint8_t var, uint8_t ladder_range);

/** Convert eye margin to mUI.
 * Takes in a horizontal margin in Phase Interpolator codes and converts it to mUI.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] var Horizontal margin in Phase Interpolator codes with 3 fractional bits.
 * @return Eye opening in mV.
 */
uint16_t blackhawk7_l8p2_INTERNAL_eye_to_mUI(srds_access_t *sa__, uint8_t var);

#define NUM_PAM_EYES 3
/*-------------------------*/
/** PAM eye margin struct  */
/*-------------------------*/
typedef struct {
    uint16_t left_eye_mUI [NUM_PAM_EYES];
    uint16_t right_eye_mUI[NUM_PAM_EYES];
    uint16_t upper_eye_mV [NUM_PAM_EYES];
    uint16_t lower_eye_mV [NUM_PAM_EYES];
} eye_margin_t; 

/*----------------------------------------------------*/
/*  Get dynamic eye margin estimation values (PAM)    */
/*----------------------------------------------------*/
/** Get dynamic eye estimation values for PAM mode.
 * Reads and converts the Eye margin estimation from the uC.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] eye_margin is a struct that contains all four corners - left(mUI), right(mUI), upper(mV) and lower(mV) for 3 eyes.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_pam_eye_margin_est(srds_access_t *sa__, eye_margin_t *eye_margin);

/*------------------------------------------------------*/
/*  Print dynamic eye margin estimation values (PAM)    */
/*------------------------------------------------------*/
/** Print dynamic eye estimation values for PAM mode.
 * Reads, converts and prints the Eye margin estimation from the uC.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_display_pam_eye_margin_est(srds_access_t *sa__);

/** Serdes Lane ClockGate.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] enable Enable lane clockgate (1 = Enable clockgate; 0 = Disable clockgate).
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_lane_clkgate(srds_access_t *sa__, uint8_t enable);

/*--------------------*/
/*  Get/Set PF status */
/*--------------------*/
/** Set function for PF.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val PF value to be set.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_pf_main(srds_access_t *sa__, uint8_t val);

/** Get function for PF
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val PF read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_pf_main(srds_access_t *sa__, uint8_t *val);

/** Set function for PF2.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val PF2 value to be set.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_pf2(srds_access_t *sa__, uint8_t val);

/** Get function for PF2.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val PF2 read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_pf2(srds_access_t *sa__, uint8_t *val);

/** Set function for PF3.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val  PF3 value to be set.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_pf3(srds_access_t *sa__, uint8_t val);

/** Get function for PF3.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val PF3 read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_pf3(srds_access_t *sa__, uint8_t *val);


/*-------------------*/
/*  Get/Set VGA/DFE  */
/*-------------------*/
/** Set function for VGA.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val signed input value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_vga(srds_access_t *sa__, uint8_t val);

/** Get function for VGA.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val VGA read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_vga(srds_access_t *sa__, uint8_t *val);


/** Set function for DFE Tap1.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val signed input value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe1(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap1.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val DFE1 read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe1(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap2.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val signed input value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe2(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap2.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val DFE2 read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe2(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap3.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val signed input value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe3(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap3.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val DFE3 read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe3(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap4.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val signed input value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe4(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap4.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val DFE4 read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe4(srds_access_t *sa__, int8_t *val);

/** Set function for DFE Tap5.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val signed input value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe5(srds_access_t *sa__, int8_t val);

/** Get function for DFE Tap5.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] val DFE5 read value.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe5(srds_access_t *sa__, int8_t *val);

/* Set/Get DFE tap values */
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe6(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe6(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe7(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe7(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe8(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe8(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe9(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe9(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe10(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe10(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe11(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe11(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe12(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe12(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe13(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe13(srds_access_t *sa__, int8_t *val);
err_code_t blackhawk7_l8p2_INTERNAL_set_rx_dfe14(srds_access_t *sa__, int8_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_rx_dfe14(srds_access_t *sa__, int8_t *val);
/*--------------------*/
/*  Get/Set BWR Regs  */
/*--------------------*/
err_code_t blackhawk7_l8p2_INTERNAL_get_bwr(srds_access_t *sa__, uint8_t *vga1_bwr, uint8_t *vga2_bwr, uint8_t *eq1_bwr, uint8_t *eq2_bwr);
err_code_t blackhawk7_l8p2_INTERNAL_set_bwr(srds_access_t *sa__, uint8_t vga1_bwr, uint8_t vga2_bwr, uint8_t eq1_bwr, uint8_t eq2_bwr, uint8_t vga_dis_ind);
#endif /* SMALL_FOOTPRINT */

/*----------------------*/
/*  Get/Set TXFIR Taps  */
/*----------------------*/

err_code_t blackhawk7_l8p2_INTERNAL_load_txfir_taps(srds_access_t *sa__);
err_code_t blackhawk7_l8p2_INTERNAL_set_tx_tap(srds_access_t *sa__, uint8_t tap_num, int16_t val);
err_code_t blackhawk7_l8p2_INTERNAL_get_tx_tap(srds_access_t *sa__, uint8_t tap_num, int16_t *val);

/** Validates Blackhawk TXFIR tap settings.
 * Returns failcodes if TXFIR settings are invalid.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] enable_taps Enable TXFIR TAPs based on #blackhawk7_l8p2_txfir_tap_enable_enum.
 * @param[in] txfir the 6 TXFIR tap values (-168 to +168 when in PAM4 mode, otherwise -127 to +127).
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_validate_full_txfir_cfg(srds_access_t *sa__, enum blackhawk7_l8p2_txfir_tap_enable_enum enable_taps, blackhawk7_l8p2_txfir_st *txfir);

/** Writes Serdes TXFIR tap settings.
 * Returns failcodes if TXFIR settings are invalid.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] enable_taps Enable TXFIR TAPs based on #blackhawk7_l8p2_txfir_tap_enable_enum.
 * @param[in] txfir the 6 TXFIR tap values (-168 to +168 when in PAM4 mode, otherwise -63 to +63).
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_apply_full_txfir_cfg(srds_access_t *sa__, enum blackhawk7_l8p2_txfir_tap_enable_enum enable_taps, blackhawk7_l8p2_txfir_st *txfir);

/** Programs specific TX AFE settings.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_apply_tx_afe_settings(srds_access_t *sa__);

/* Lane Config Struct */
err_code_t blackhawk7_l8p2_INTERNAL_update_uc_lane_config_st(struct blackhawk7_l8p2_uc_lane_config_st *st);
err_code_t blackhawk7_l8p2_INTERNAL_update_uc_lane_config_word(struct blackhawk7_l8p2_uc_lane_config_st *st);

/* Lane User Control Disable Startup Function Struct */
err_code_t blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_functions_st(struct blackhawk7_l8p2_usr_ctrl_disable_functions_st *st);
err_code_t blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_functions_byte(struct blackhawk7_l8p2_usr_ctrl_disable_functions_st *st);

/* Lane User Control Disable Startup DFE Function Struct */
err_code_t blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(struct blackhawk7_l8p2_usr_ctrl_disable_dfe_functions_st *st);
err_code_t blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(struct blackhawk7_l8p2_usr_ctrl_disable_dfe_functions_st *st);

/* Core Config Struct */
err_code_t blackhawk7_l8p2_INTERNAL_update_uc_core_config_st(struct  blackhawk7_l8p2_uc_core_config_st *st);
err_code_t blackhawk7_l8p2_INTERNAL_update_uc_core_config_word(struct  blackhawk7_l8p2_uc_core_config_st *st);

#ifndef SMALL_FOOTPRINT
uint8_t blackhawk7_l8p2_INTERNAL_rdb_uc_var(srds_access_t *sa__, err_code_t *err_code_p, uint32_t addr);
uint16_t blackhawk7_l8p2_INTERNAL_rdw_uc_var(srds_access_t *sa__, err_code_t *err_code_p, uint32_t addr);
err_code_t blackhawk7_l8p2_INTERNAL_wrb_uc_var(srds_access_t *sa__, uint32_t addr, uint8_t wr_val);
err_code_t blackhawk7_l8p2_INTERNAL_wrw_uc_var(srds_access_t *sa__, uint32_t addr, uint16_t wr_val);
#endif /* SMALL_FOOTPRINT */

/*--------------------------*/
/*  Register field polling  */
/*--------------------------*/

err_code_t blackhawk7_l8p2_INTERNAL_print_uc_dsc_error(srds_access_t *sa__, enum srds_pmd_uc_cmd_enum cmd);

/** Polls lane variable "usr_diag_status" to verify data is available in uC diag buffer.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in blackhawk7_l8p2_internal.c.
 *
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] status returns a status byte. \n
 *    bit 15 - indicates the ey scan is complete. \n
 *    bit 14-0 - reserved for debug.
 *
 * @param[in] timeout_ms Maximum time interval in milliseconds for which the polling is done.
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_poll_diag_done(srds_access_t *sa__, uint16_t *status, uint32_t timeout_ms);

/** Polls for register field "uc_dsc_ready_for_cmd" to be 1 within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in blackhawk7_l8p2_internal.c.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] timeout_ms Maximum time interval in milliseconds for which the polling is done.
 * @param[in] cmd The command that was issued; this is used for error reporting.
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_poll_uc_dsc_ready_for_cmd_equals_1(srds_access_t *sa__, uint32_t timeout_ms, enum srds_pmd_uc_cmd_enum cmd);

/** Polls for register field "dsc_state" to be "DSC_STATE_UC_TUNE".
 * within the time interval specified by timeout_ms.

 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in blackhawk7_l8p2_internal.c.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] timeout_ms Maximum time interval in milliseconds for which the polling is done.
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_poll_dsc_state_equals_uc_tune(srds_access_t *sa__, uint32_t timeout_ms);


/** Polls for register field "micro_ra_initdone" to be 1 within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in blackhawk7_l8p2_internal.c.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] timeout_ms Maximum time interval in milliseconds for which the polling is done.
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_poll_micro_ra_initdone(srds_access_t *sa__, uint32_t timeout_ms);

/** Utility to figure out if running platform is big endian.
 * @return value of 1 if big endian else 0
 */
uint8_t blackhawk7_l8p2_INTERNAL_is_big_endian(void);
/*--------------------------------*/
/*  RAM Block Read with Callback  */
/*--------------------------------*/

/* Utilities to account for endianness when reading from uc RAM */
#define get_endian_offset(addr) (blackhawk7_l8p2_INTERNAL_is_big_endian() ? \
                                (\
                                (((USR_UINTPTR)(addr))%4 == 0) ?  3 : \
                                (((USR_UINTPTR)(addr))%4 == 1) ?  1 : \
                                (((USR_UINTPTR)(addr))%4 == 2) ? -1 : \
                                (((USR_UINTPTR)(addr))%4 == 3) ? -3 : \
                                0) \
                                : 0)

typedef struct {
    uint8_t *mem_ptr;
} blackhawk7_l8p2_INTERNAL_rdblk_callback_arg_t;

/** Callback function for reading uc RAM memory by blackhawk7_l8p2_INTERNAL_rdblk_uc_generic_ram().
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in,out] arg is a blackhawk7_l8p2_INTERNAL_rdblk_callback_arg_t pointer for holding state.
 * @param[in] byte_count is the number of bytes to store.
 *        -# This function is called repeatedly with a byte count of 2 with U16 values to store.
 *        -# This function may then be called with a byte count of 1 if there are an odd number of bytes.
 * @param[in] data is the RAM data to store.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_rdblk_callback(srds_access_t *sa__, void *arg, uint8_t byte_count, uint16_t data);

/** Block RAM read through Micro Register Interface, suitable for program RAM or non-program RAM.
 *  This reads a block of RAM, possibly starting midway through it and wrapping back to the beginning.
 *  The order of operations is:
 *      -# RAM will be read from (block_addr+start_offset) to (block_addr+block_size-1),
 *         up to cnt bytes.
 *      -# RAM will be read from block_addr to (block_addr+block_size-1), repeating
 *         indefinitely, until cnt bytes total are read.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] block_addr Address of RAM block to be read.
 *                   For program RAM, the upper 16 bits are 0x0000.  Otherwise, the upper 16 bits are 0x2000.
 * @param[in] block_size Length of RAM block to read.
 * @param[in] start_offset Offset within RAM block to start reading.
 * @param cnt Number of bytes to be read.
 * @param[in,out] arg is passed as the first argument to callback.
 * @param[in] callback is called with all of the data read, two bytes at a time.
 *                  The last call of callback may have one byte; in that case, the upper byte is undefined.
 *                  The call is in the form:  callback(arg, byte_count, data).
 * @return Error Code generated by API or the callback (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_rdblk_uc_generic_ram(srds_access_t *sa__,
                                                uint32_t block_addr,
                                                uint32_t block_size,
                                                uint32_t start_offset,
                                                uint32_t cnt,
                                                void *arg,
                                                err_code_t (*callback)(srds_access_t *, void *, uint8_t, uint16_t));

/** Block RAM read through Micro Register Interface, suitable for program RAM or non-program RAM.
 *  This reads a block of RAM, possibly starting midway through it and wrapping back to the beginning.
 *  The order of operations is:
 *      -# RAM will be read from (block_addr+start_offset) down to block_addr, up to cnt bytes.
 *      -# RAM will be read from (block_addr+block_size-1) down to block_addr, repeating
 *         indefinitely, until cnt bytes total are read.
 *  In other words, this is the same as blackhawk7_l8p2_INTERNAL_rdblk_uc_generic_ram(), but reading in descending address order.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param block_addr Address of RAM block to be read.
 *                   For program RAM, the upper 16 bits are 0x0000.  Otherwise, the upper 16 bits are 0x2000.
 * @param[in] block_size Length of RAM block to read.
 * @param[in] start_offset Offset within RAM block to start reading.
 * @param[in] cnt Number of bytes to be read.
 * @param[in,out] arg is passed as the first argument to callback.
 * @param[in] callback is called with all of the data read, two bytes at a time.
 *                  The last call of callback may have one byte; in that case, the upper byte is undefined.
 *                  The call is in the form:  callback(arg, byte_count, data).
 * @return Error Code generated by API or the callback (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_rdblk_uc_generic_ram_descending(srds_access_t *sa__,
                                                           uint32_t block_addr,
                                                           uint32_t block_size,
                                                           uint32_t start_offset,
                                                           uint32_t cnt,
                                                           void *arg,
                                                           err_code_t (*callback)(srds_access_t *, void *, uint8_t, uint16_t));

/** Retrieve micro index based on lane index.
 * @param lane Lane index.
 * @return Micro index.
 */
uint8_t blackhawk7_l8p2_INTERNAL_grp_idx_from_lane(uint8_t lane);


/** Extract the refclk frequency in Hz, based on a blackhawk7_l8p2_pll_refclk_enum value.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] refclk REFCLK input.
 * @param[out] refclk_in_hz REFCLK value in Hz updated by API.
 * @return Error Code generated by API (ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_refclk_in_hz(srds_access_t *sa__, enum blackhawk7_l8p2_pll_refclk_enum refclk, uint32_t *refclk_in_hz);

/** Retrieve LMS threshold value.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] lms_thresh_bin lms_threshold value to be populated by the API.
 * @return Error Code generated by API (ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_lms_thresh_bin(srds_access_t *sa__, int8_t *lms_thresh_bin);

#ifndef SMALL_FOOTPRINT
/** Polls for the dbgfb idx lock.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] timeout_ms polling timeout value in ms.
 * @return Error Code generated by API (ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_poll_dbgfb_idx_lock(srds_access_t *sa__, uint8_t timeout_ms);

/** Reads dbgfb counters.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] dbgfb_cfg pointer to dbgfb_cfg_st.
 * @return Error Code generated by API (ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_get_dbgfb_summary(srds_access_t *sa__, dbgfb_cfg_st *dbgfb_cfg); 

/** collects dbgfb stats for all.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] time_in_us 32 bit field that carries the run time for fb stats accumulation .
 * @param[out] dbgfb_stats pointer of type dbgfb_stats_st.
 * @return Error Code generated by API (ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_collect_dbgfb_stats(srds_access_t *sa__, uint32_t time_in_us, dbgfb_stats_st *dbgfb_stats); 
#endif /* SMALL_FOOTPRINT */

/** Reads the slicer calibration error values.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] lms_err lms calibration error value.
 * @param[out] phase_err phase calibration error value.
 * @param[out] data_err data calibration error value.
 * @param[out] max_data_err max data calibration error value.
 * @return Error Code generated by API (ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_read_cal_error(srds_access_t *sa__, int8_t *lms_err, int8_t *phase_err, int8_t *data_err, int8_t *max_data_err);

#ifndef SMALL_FOOTPRINT
/** Select the clk from the clk4sync block.
 * Selects the fast clk(VCO divided) if enabled, or slow clk(comclk) when disabled.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] enable 1: enabled, 0: disabled.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_clk4sync_div2_sequence(srds_access_t *sa__, uint8_t enable);
#endif /* SMALL_FOOTPRINT */


#define RES_ERROR_NUM_SLICER    (4)
#define RES_ERROR_NUM_ILV       (2)

/** Reads the horizontal calibration residual error values.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[out] res_dac4ck_err horizontal calibration residual error values.
 * @param[out] sgn_chn_cnt sign change count values for residual errors.
 * @return Error Code generated by API (ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_read_res_error(srds_access_t *sa__, int8_t res_dac4ck_err[][RES_ERROR_NUM_ILV], uint8_t sgn_chn_cnt[][RES_ERROR_NUM_ILV]);

/** Print info table.
 * Prints out all info_table struct member values.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_display_info_table(srds_access_t *sa__);

/** Get logical tx lane.
 * Given a physical tx lane, returns the logical tx lane mapped to it.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] physical_tx_lane index of the physical tx lane.
 * @param[out] logical_tx_lane pointer to the logical tx lane that will get populated.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_logical_tx_lane_addr(srds_access_t *sa__, uint8_t physical_tx_lane, uint8_t *logical_tx_lane);

/** Get logical rx lane.
 * Given a physical rx lane, returns the logical rx lane mapped to it.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] physical_rx_lane index of the physical rx lane.
 * @param[out] logical_rx_lane pointer to the logical rx lane that will get populated.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_get_logical_rx_lane_addr(srds_access_t *sa__, uint8_t physical_rx_lane, uint8_t *logical_rx_lane);
#ifndef SMALL_FOOTPRINT
/** Enable breakpoint.
 * Enables a breakpoint at the given breakpoint param value.
 * Note: Call before releasing blackhawk7_l8p2_ln_dp_reset().
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] breakpoint index of breakpoint to enable.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_en_breakpoint(srds_access_t *sa__, uint8_t breakpoint);

/** Goto Breakpoint.
 * Goes to the breakpoint selected by the given breakpoint param value.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] breakpoint index of breakpoint to enable.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_goto_breakpoint(srds_access_t *sa__, uint8_t breakpoint);

/** Read breakpoint.
 * Reads and prints status of breakpoint.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_rd_breakpoint(srds_access_t *sa__);

/** Disable all breakpoint.
 * Disables all breakpoints and continues normal execution.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_l8p2_INTERNAL_dis_breakpoint(srds_access_t *sa__);

#endif /* SMALL_FOOTPRINT */

/*-------------------*/
/*  Name resolution  */
/*-------------------*/

/* For bitfields that differ for different cores, these macros are defined to resolve the bitfield. */

#   define WR_RX_PF_CTRL  wr_rx_pf_ctrl_val
#   define RD_RX_PF_CTRL  rd_rx_pf_ctrl_val
#   define WR_RX_PF2_CTRL wr_rx_pf2_ctrl_val
#   define RD_RX_PF2_CTRL rd_rx_pf2_ctrl_val
#   define WR_RX_PF3_CTRL wr_ams_rx_eq2_ctrl
#   define RD_RX_PF3_CTRL rd_ams_rx_eq2_ctrl

#   define REG_RD_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS reg_rd_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS
#   define REG_RD_TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS reg_rd_TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS

#endif
#ifdef __cplusplus
}
#endif

