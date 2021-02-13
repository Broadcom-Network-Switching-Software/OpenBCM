/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/***********************************************************************************
 ***********************************************************************************
 *                                                                                 *
 *  Revision    :      *
 *                                                                                 *
 *  Description :  Config functions provided to IP user                            *
 */

/** @file falcon16_tsc_config.h
 * Configuration functions provided to IP User
 */

#ifndef FALCON16_TSC_API_CONFIG_H
#define FALCON16_TSC_API_CONFIG_H

#include "falcon16_tsc_ipconfig.h"
#include "common/srds_api_enum.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_types.h"
#include "falcon16_tsc_enum.h"
#include "falcon16_tsc_types.h"
#include "falcon16_tsc_select_defns.h"
#include "falcon16_tsc_access.h"

#define GRACEFUL_STOP_TIME 200

#define RELEASE_LOCK_AND_RETURN(__x__) \
    USR_RELEASE_LOCK; \
    return (__x__);

/****************************************************/
/*  CORE Based APIs - Required to be used per Core  */
/****************************************************/
/* Returns API Version Number */
/** API Version Number.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *api_version API Version Number returned by the API
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_version(srds_access_t *sa__, uint32_t *api_version);

/*------------------------------------------------*/
/*  APIs to Read Core Config variables in uC RAM  */
/*------------------------------------------------*/
/** Read value of core_config uC RAM variable.
 *  Note that various API configuration functions can modify core config.
 *  Since the value returned by this can become stale, re-read after modifying core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *struct_val Value to be written into core_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_get_uc_core_config(srds_access_t *sa__, struct falcon16_tsc_uc_core_config_st *struct_val);

/*------------------------------------------------*/
/*  APIs to get number of uc cores  */
/*------------------------------------------------*/
/** Get the number of uc cores.
 *  Note that various API configuration functions can modify core config.
 *  Since the value returned by this can become stale, re-read after modifying core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *num_micros Value to be written with number of uc cores
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */

err_code_t falcon16_tsc_get_micro_num_uc_cores(srds_access_t *sa__, uint8_t *num_micros);
/*-----------------------------------*/
/*  Microcode Load/Verify Functions  */
/*-----------------------------------*/
#ifndef DISABLE_MDIO_LOAD
/** Load Microcode into Micro through Register (MDIO) Interface.
 * Once the microcode is loaded, de-assert reset to 8051 to start executing microcode "wrc_micro_mdio_dw8051_reset_n(0x1)".
 * \n Note: Micro should be loaded only after issuing a falcon16_tsc_uc_reset(1) followed by asserting and de-asserting
 * core_s_reset. Information table should be intialized with falcon16_tsc_init_falcon16_tsc_info after microcode load.
 * See relevant Programmers guide for more details.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *ucode_image pointer to the Microcode image organized in bytes
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_ucode_mdio_load(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len);
#endif /* DISABLE_MDIO_LOAD */

/** To verify the Microcode image loaded in the Micro.
 * Read back the microcode from Micro and check against expected microcode image.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *ucode_image pointer to the expeted Microcode image organized in bytes
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_ucode_load_verify(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len);

/** To verify the CRC of the microcode loaded in the Micro.
 * Instruct uC to read image and calculate CRC and check against expected CRC.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @param expected_crc_value Expected CRC value of the microcode
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_ucode_crc_verify(srds_access_t *sa__, uint16_t ucode_len, uint16_t expected_crc_value);

/** To instruct the micro to start calculating the CRC of the microcode.
 * Instruct uC to read image and calculate CRC.
 * Control is returned after triggering start of CRC calculation (does NOT wait for completion of CRC calculation). \n \n
 * NOTE: No uC commands should be executed between starting the CRC calculaion [falcon16_tsc_ucode_crc_verify()] and verifying the CRC value [falcon16_tsc_start_ucode_crc_calc()].
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_start_ucode_crc_calc(srds_access_t *sa__, uint16_t ucode_len);

/** To check the expected CRC against the CRC calulated by the micro.
 * NOTE: No uC commands should be executed between starting the CRC calculation [falcon16_tsc_ucode_crc_verify()] and verifying the CRC value [falcon16_tsc_start_ucode_crc_calc()].
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param expected_crc_value Expected CRC value of the microcode
 * @param timeout_ms Time interval in milliseconds inside which the previous command (calculate CRC) should be completed
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_check_ucode_crc(srds_access_t *sa__, uint16_t expected_crc_value, uint32_t timeout_ms);

/** Load the Micro through the pram bus.
 * NOTE: Information table should be intialized with falcon16_tsc_init_falcon16_tsc_info after microcode load.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param ucode_image Microcode Image to be written
 * @param ucode_len Length of Microcode Image (number of bytes)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_ucode_pram_load(srds_access_t *sa__, char const * ucode_image, uint32_t ucode_len);

/** Enable or Disable the uC reset.
 * Note: Micro should be reset using the API everytime before reloading the microcode
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable uC reset (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_uc_reset(srds_access_t *sa__, uint8_t enable);

/** Enable or Disable the uC reset.
 * Note: Micro should be reset using the API everytime before reloading the microcode
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable uC reset (1 = Enable; 0 = Disable)
 * @param ucode_info struct has information regarding stack size
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_uc_reset_with_info(srds_access_t *sa__, uint8_t enable, ucode_info_t ucode_info);

/** Wait for the uC to become active.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wait_uc_active(srds_access_t *sa__);

/** Initialize the falcon16_tsc_info for the uC.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_init_falcon16_tsc_info(srds_access_t *sa__);


/** Set the core_cfg_from_pcs flag in the core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param core_cfg_from_pcs The value to set
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_set_core_config_from_pcs(srds_access_t *sa__, uint8_t core_cfg_from_pcs);

/** Set the osr_2p5_en flag in the core configuration.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param osr_2p5_en The value to set
 * @return Error Code generated if uC does not become active (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_set_osr_2p5_en(srds_access_t *sa__, uint8_t osr_2p5_en);


/** Configure the lane address mapping.
 *  Note: Micro and core data path must be in reset when this function is called.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param num_lanes The number of entries in tx_lane_map and rx_lane_map.
 *                  This must match the number of lanes associated with the core.
 * @param *tx_lane_map A num_lanes-sized array of lane indexes to map to the TX lanes.
 *                     Each entry must be from 0 to num_lanes-1, and each entry must be unique.
 * @param *rx_lane_map A num_lanes-sized array of lane indexes to map to the RX lanes.
 *                     Each entry must be from 0 to num_lanes-1, and each entry must be unique.
 *                     If independent TX / RX lane mapping is not enabled for a core, then this must match tx_lane_map.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_map_lanes(srds_access_t *sa__, uint8_t num_lanes, uint8_t const *tx_lane_map, uint8_t const *rx_lane_map);


/*-----------------*/
/*  Configure PLL  */
/*-----------------*/

/** The pll can be configured by providing all four parameters:  refclk frequency, divider value, VCO output frequency and the PLL options. 
 *  This is the only option in SMALL_FOOTPRINT
 */
#define falcon16_tsc_configure_pll_refclk_div_vco falcon16_tsc_INTERNAL_configure_pll 
/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this fucntion is based on refclk frequency and divider value.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Reference clock frequency (enumerated)
 * @param srds_div Divider value (enumerated)
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_div(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           enum falcon16_tsc_pll_div_enum srds_div);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on refclk frequency and VCO output frequency.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Reference clock frequency (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_vco(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on divider value and VCO output frequency.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param srds_div Divider value (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_div_vco(srds_access_t *sa__,
                                        enum falcon16_tsc_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this fucntion is based on input refclk frequency and divider value.
 * but first divides the input refclk by 2 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 2  (enumerated)
 * @param srds_div Divider value based on input reference clock frequency (enumerated)
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_div_div2refclk(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           enum falcon16_tsc_pll_div_enum srds_div);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on input refclk frequency and VCO output frequency.
 * but first divides the input refclk by 2 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 2 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_vco_div2refclk(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on divider value and VCO output frequency.
 * but first divides the input refclk by 2 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param srds_div Divider value based on input refclk frequency before refclk divide by 2 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_div_vco_div2refclk(srds_access_t *sa__,
                                        enum falcon16_tsc_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz);
/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this fucntion is based on input refclk frequency and divider value.
 * but first divides the input refclk by 4 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 4  (enumerated)
 * @param srds_div Divider value based on input reference clock frequency (enumerated)
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_div_div4refclk(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           enum falcon16_tsc_pll_div_enum srds_div);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on input refclk frequency and VCO output frequency.
 * but first divides the input refclk by 4 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Input Reference clock frequency before refclk divide by 4 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_vco_div4refclk(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on divider value and VCO output frequency.
 * but first divides the input refclk by 4 and adjusts parameters accordingly.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param srds_div Divider value based on input refclk frequency before refclk divide by 4 (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_div_vco_div4refclk(srds_access_t *sa__,
                                        enum falcon16_tsc_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this fucntion is based on refclk frequency and divider value.
 * Enables refclk doubler.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Reference clock frequency (enumerated)
 * @param srds_div Divider value (enumerated)
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_div_refclk_2x(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           enum falcon16_tsc_pll_div_enum srds_div);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on refclk frequency and VCO output frequency.
 * Enables refclk doubler.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk Reference clock frequency (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_refclk_vco_refclk_2x(srds_access_t *sa__,
                                           enum falcon16_tsc_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz);

/** Configure PLL.
 *
 * Use core_s_rstb to re-initialize all registers to default before calling this function.
 *
 * Configures PLL registers to obtain the required configuration.
 * PLL configuration for this function is based on divider value and VCO output frequency.
 * Enables refclk doubler.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param srds_div Divider value (enumerated)
 * @param vco_freq_khz VCO output frequency, in kHz
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_configure_pll_div_vco_refclk_2x(srds_access_t *sa__,
                                        enum falcon16_tsc_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz);

/** Get the VCO frequency in kHz, based on the reference clock frequency and divider value
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param refclk_freq_hz Reference clock frequency, in Hz.
 * @param srds_div Divider value, in the same encoding as enum #falcon16_tsc_pll_div_enum.
 * @param *vco_freq_khz VCO output frequency, in kHz, obtained based on reference clock frequency and dic value
 * @param pll_option Select PLL configuration option from enum #falcon16_tsc_pll_option_enum.
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_get_vco_from_refclk_div(srds_access_t *sa__, uint32_t refclk_freq_hz, enum falcon16_tsc_pll_div_enum srds_div, uint32_t *vco_freq_khz, enum falcon16_tsc_pll_option_enum pll_option);

/**************************************************/
/* LANE Based APIs - Required to be used per Lane */
/**************************************************/

/*------------------------------------------------------------*/
/*  APIs to Write Lane Config and User variables into uC RAM  */
/*------------------------------------------------------------*/
/** Write to lane_config uC RAM variable.
 * Note: This API should be used only during configuration under dp_reset.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param struct_val Value to be written into lane_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_set_uc_lane_cfg(srds_access_t *sa__, struct falcon16_tsc_uc_lane_config_st struct_val);

/*-----------------------------------------------------------*/
/*  APIs to Read Lane Config and User variables from uC RAM  */
/*-----------------------------------------------------------*/
/** Read value of lane_config uC RAM variable.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *struct_val Value read from lane_config RAM variable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_get_uc_lane_cfg(srds_access_t *sa__, struct falcon16_tsc_uc_lane_config_st *struct_val);


/*--------------------------------------------*/
/*  APIs to Enable or Disable datapath reset  */
/*--------------------------------------------*/

/** Enable or Disable TX datapath reset.
 * Asserts handshake signals upon disable.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable TX datapath reset (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_tx_dp_reset(srds_access_t *sa__, uint8_t enable);

/** Enable or Disable RX datapath reset.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable RX datapath reset (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_rx_dp_reset(srds_access_t *sa__, uint8_t enable);

/** Enable or Disable Core datapath reset.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable Core datapath reset (1 = Enable; 0 = Disable)
 * @return Error Code generated by invalid access (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_core_dp_reset(srds_access_t *sa__, uint8_t enable);


/*---------------------------*/
/*  Falcon16 TX Analog APIs  */
/*---------------------------*/
/** Write TX AFE parameters.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param selects the parameter to write based on #falcon16_tsc_tx_afe_settings_enum
 * @param val is the signed input value to the parameter
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_write_tx_afe(srds_access_t *sa__, enum falcon16_tsc_tx_afe_settings_enum param, int8_t val);

/** Read TX AFE parameters.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param selects the parameter to read based on #falcon16_tsc_tx_afe_settings_enum
 * @param *val is the returned signed value of the parameter
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_read_tx_afe(srds_access_t *sa__, enum falcon16_tsc_tx_afe_settings_enum param, int8_t *val);

/** Validates Falcon16 TXFIR tap settings.
 * Returns failcodes if TXFIR settings are invalid.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param pre     TXFIR pre tap value (0..31)
 * @param main    TXFIR main tap value (0..127)
 * @param post1   TXFIR post tap value (-63..63)
 * @param post2   TXFIR post2 tap value (-15..15)
 * @param post3   TXFIR post3 tap value (-7..7)
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_validate_txfir_cfg(srds_access_t *sa__, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3);

/** Writes Serdes TXFIR tap settings.
 * Returns failcodes if TXFIR settings are invalid
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param pre     TXFIR pre tap value (0..31)
 * @param main    TXFIR main tap value (0..127)
 * @param post1   TXFIR post tap value (-63..63)
 * @param post2   TXFIR post2 tap value (-15..15)
 * @param post3   TXFIR post3 tap value (-7..7)
 * @return Error Code generated by invalid tap settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_apply_txfir_cfg(srds_access_t *sa__, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3);


/*----------------*/
/*   PMD_RX_LOCK  */
/*----------------*/

/** PMD rx lock status of current lane.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *pmd_rx_lock PMD_RX_LOCK status of current lane returned by API
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_lock_status(srds_access_t *sa__, uint8_t *pmd_rx_lock);

/*--------------------------------*/
/*  Serdes TX disable/RX Restart  */
/*--------------------------------*/
/** TX Disable.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable TX disable (1 = TX Disable asserted; 0 = TX Disable removed)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_tx_disable(srds_access_t *sa__, uint8_t enable);

/** Enable/disable Restart RX and hold.
 * (Reset DSC state machine into RESTART State and hold it till disabled)
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable Restart RX and hold (1 = RX restart and hold; 0 = Release hold in restart state)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_rx_restart(srds_access_t *sa__, uint8_t enable);


/*-----------------------------*/
/*  Stop/Resume RX Adaptation  */
/*-----------------------------*/
/** Stop RX Adaptation on a Lane. Control is returned only after attempting to stop adaptation.
 * RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable RX Adaptation stop (1 = Stop RX Adaptation on lane; 0 = Resume RX Adaptation on lane)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_stop_rx_adaptation(srds_access_t *sa__, uint8_t enable);

/** Request to stop RX Adaptation on a Lane.
 * Control will be returned immediately before adaptaion is completely stopped.
 * RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * To resume RX adaptation, use the falcon16_tsc_stop_rx_adaptation() API.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_request_stop_rx_adaptation(srds_access_t *sa__);

/*------------------------------------*/
/*  Read/Write all RX AFE parameters  */
/*------------------------------------*/

/** Write to RX AFE settings.
 * Note: RX Adaptation needs to be stopped before modifying any of the VGA, PF or DFE taps.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param Enum (#srds_rx_afe_settings_enum) to select the required RX AFE setting to be modified
 * @param val Value to be written to the selected AFE setting
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_write_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t val);

/** Read from RX AFE settings.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param param Enum (#srds_rx_afe_settings_enum) to select the required RX AFE setting to be read
 * @param *val Value to be written to the selected AFE setting
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_read_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t *val);

/*-----------------------------*/
/*  TX_PI Fixed Frequency Mode */
/*-----------------------------*/

/** TX_PI Fixed Frequency Mode.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable/Disable TX_PI (1 = Enable; 0 = Disable)
 * @param freq_override_val Fixed Frequency Override value (freq_override_val = desired_ppm*8192/781.25; Range: -8192 to + 8192);
 * @return Error Code generated by invalid TX_PI settings (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_tx_pi_freq_override(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val);

/** Perform steps necessary to align TX clocks
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param num_lanes is the number of lanes available in the SerDes IP core (4)
 * @param enable  1 = Enable ; 0 = Disable
 * @return Error Code, if generated (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_tx_clock_align(srds_access_t *sa__, int num_lanes, int enable);

/*--------------------------------------------*/
/*  Loopback and Ultra-Low Latency Functions  */
/*--------------------------------------------*/

/** Locks TX_PI to Loop timing.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable TX_PI lock to loop timing (1 = Enable Lock; 0 = Disable Lock)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_loop_timing(srds_access_t *sa__, uint8_t enable);

/** Enable/Disable Remote Loopback.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param enable Enable Remote Loopback (1 = Enable rmt_lpbk; 0 = Disable rmt_lpbk)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_rmt_lpbk(srds_access_t *sa__, uint8_t enable);

#endif
