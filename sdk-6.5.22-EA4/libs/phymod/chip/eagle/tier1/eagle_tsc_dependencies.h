/**************************************************************************************
 **************************************************************************************
 *                                                                                    *
 *  Revision      :  $Id: eagle_tsc_dependencies.h 898 2015-02-04 23:22:17Z eroes $ *
 *                                                                                    *
 *  Description   :  API Dependencies to be provided by IP user                       *
 *                                                                                    *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 */

/** @file eagle_tsc_dependencies.h
 * Dependencies to be provided by IP User
 */

#ifndef EAGLE_TSC_API_DEPENDENCIES_H
#define EAGLE_TSC_API_DEPENDENCIES_H

#include "eagle_tsc_usr_includes.h"
#include "common/srds_api_err_code.h"
#include <phymod/phymod.h>
/* #include "eagle_tsc_ipconfig.h" */


uint16_t eagle_tsc_pmd_rd_reg(const phymod_access_t *pa, uint16_t address);

/** Read a register from the currently selected Serdes IP Lane.
 * @param pa phymod_access_t struct
 * @param address Address of register to be read
 * @param *val value read out from the register
 * @return Error code generated by read function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_pmd_rdt_reg(const phymod_access_t *pa,uint16_t address, uint16_t *val);

/** Write to a register from the currently selected Serdes IP Lane.
 * @param pa phymod_access_t struct
 * @param address Address of register to be written
 * @param val Value to be written to the register
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_pmd_wr_reg(const phymod_access_t *pa,uint16_t address, uint16_t val);

/** Masked Register Write to the currently selected Serdes IP core/lane.
 * If using Serdes MDIO controller to access the registers, implement this function using eagle_tsc_pmd_mdio_mwr_reg(..)
 *
 * If NOT using a Serdes MDIO controller or the Serdes PMI Masked write feature, please use the following code to
 * implement this function
 *
 *    eagle_tsc_pmd_wr_reg(const phymod_access_t *pa,addr, ((eagle_tsc_pmd_rd_reg(addr) & ~mask) | (mask & (val << lsb))));
 *
 * @param pa phymod_access_t struct
 * @param addr Address of register to be written
 * @param mask 16-bit mask indicating the position of the field with bits of 1s
 * @param lsb  LSB of the field
 * @param val  16bit value to be written
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_pmd_mwr_reg(const phymod_access_t *pa,uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val);

/** Write message to the logger with the designated verbose level.
 * Output is sent to stdout and a logfile
 * @param pa phymod_access_t struct
 * @param message_verbose_level   Verbose level for the current message
 * @param *format Format string as in printf
 * @param ... Additional variables used as in printf
 * @return Error code generated by function (returns ERR_CODE_NONE if no errors)
 */
int logger_write(int message_verbose_level, const char *format, ...);

/** Delay the execution of the code for atleast specified amount of time in nanoseconds.
 * This function is used ONLY for delays less than 1 microsecond, non-zero error code may be returned otherwise.
 * The user can implement this as an empty function if their register access latency exceeds 1 microsecond.
 * @param pa phymod_access_t struct
 * @param delay_ns Delay in nanoseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_delay_ns(uint16_t delay_ns);

/** Delay the execution of the code for atleast specified amount of time in microseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 microsecond.
 * @param pa phymod_access_t struct
 * @param delay_us Delay in microseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_delay_us(uint32_t delay_us);

/** Delay the execution of the code for atleast specified amount of time in milliseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 millisecond.
 * @param pa phymod_access_t struct
 * @param delay_ms Delay in milliseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_delay_ms(uint32_t delay_ms);

/** Return the address of current selected Serdes IP Core.
 * @param pa phymod_access_t struct
 * @return the IP level address of the current core.
 */
uint8_t eagle_tsc_get_core(void);

/** Return the address of current selected Serdes IP lane.
 * @param pa phymod_access_t struct
 * @return the IP level address of the current lane. 0 to N-1, for an N lane IP
 */
uint8_t eagle_tsc_get_lane(const phymod_access_t *pa);


/** Polls for register field "uc_dsc_ready_for_cmd" to be 1 within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in eagle_tsc_functions.c.
 * @param pa phymod_access_t struct
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_poll_uc_dsc_ready_for_cmd_equals_1( const phymod_access_t *pa, uint32_t timeout_ms);

/** Polls lane variable "usr_diag_status" to verify data is available in uC diag buffer.
 * It then reads a WORD of data wich is 2 float8_t samples, it converts them to uint32_t
 * and returns them in the memory pointed to by *data.
 *
 * @param pa phymod_access_t struct
 * @param *data is pointer to 2 element array of uint32_tpassed from uC through dsc_data
 * @param *status returns a status byte           \n
 *    bit 15 - indicates the ey scan is complete \n
 *    bit 14 - indicates uC is slower than read access \n
 *    bit 13 - indicates uC is faster than read access \n
 *    bit 7-0 - indicates amount of data in the uC buffer
 *
 * @param pa phymod_access_t struct
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_poll_diag_eye_data(const phymod_access_t *pa,uint32_t *data,uint16_t *status, uint32_t timeout_ms);

/** Polls lane variable "usr_diag_status" to verify data is available in uC diag buffer.
 *
 * @param pa phymod_access_t struct
 * @param *status returns a status byte \n
 *    bit 15 - indicates the ey scan is complete \n
 *    bit 14-0 - reserved for debug
 *
 * @param pa phymod_access_t struct
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_poll_diag_done( const phymod_access_t *pa, uint16_t *status, uint32_t timeout_ms);

/** Polls for register field "dsc_state" to be "DSC_STATE_UC_TUNE"
 * within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in
 * eagle_tsc_functions.c.
 * @param pa phymod_access_t struct
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_tsc_poll_dsc_state_equals_uc_tune( const phymod_access_t *pa, uint32_t timeout_ms);

/** Convert uC lane index.
* Convert uC lane index to system ID string.
 * @param pa phymod_access_t struct
* @param *string a 256-byte output buffer to receive system ID
* @param uc_lane_idx uC lane index
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
* The textual identifier is pre-filled with a default:  implementors may
* safely return without modifying it if the default text is sufficient.
*/
err_code_t eagle_tsc_uc_lane_idx_to_system_id(char *string , uint8_t uc_lane_idx);



/***********************************************/
/*  Microcode Load into Program RAM Functions  */
/***********************************************/

#endif
