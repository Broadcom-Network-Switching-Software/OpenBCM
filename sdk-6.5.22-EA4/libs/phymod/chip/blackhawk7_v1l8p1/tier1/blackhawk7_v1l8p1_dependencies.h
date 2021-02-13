/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2020 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/**************************************************************************************
 **************************************************************************************
 *                                                                                    *
 *  Revision      :   *
 *                                                                                    *
 *  Description   :  API Dependencies to be provided by IP user                       *
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLACKHAWK7_V1L8P1_API_DEPENDENCIES_H
#define BLACKHAWK7_V1L8P1_API_DEPENDENCIES_H

#include "common/srds_api_err_code.h"
#include "common/srds_api_types.h"

/*! @file
 * @brief Dependencies to be provided by IP User
 */
#define PHYMOD_EFUN_WR(expr) \
    do { \
            int phymod_pmd_err_code = expr; \
            if (phymod_pmd_err_code != ERR_CODE_NONE) { \
                return phymod_pmd_err_code; \
            } \
    } while (0);



/*! @addtogroup APITag
 * @{
 */

/*! @defgroup SerdesAPIDependenciesTag Dependencies */

/*! @addtogroup SerdesAPIDependenciesTag
 * @{
 */

/** Read a register from the currently selected Serdes IP Lane.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] address Address of register to be read.
 * @param[out] *val value read out from the register.
 * @return Error code generated by read function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_v1l8p1_pmd_rdt_reg(srds_access_t *sa__, uint16_t address, uint16_t *val);

/** Write to a register from the currently selected Serdes IP Lane.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] address Address of register to be written.
 * @param[in] val Value to be written to the register.
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_v1l8p1_pmd_wr_reg(srds_access_t *sa__, uint16_t address, uint16_t val);

/** Masked Register Write to the currently selected Serdes IP core/lane.
 * If using Serdes MDIO controller to access the registers, implement this function using blackhawk7_v1l8p1_pmd_mdio_mwr_reg(..).
 *
 * If NOT using a Serdes MDIO controller or the Serdes PMI Masked write feature, please use the following code to
 * implement this function.
 *
 *    blackhawk7_v1l8p1_pmd_wr_reg(addr, ((blackhawk7_v1l8p1_pmd_rd_reg(addr) & ~mask) | (mask & (val << lsb))));
 *
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] addr Address of register to be written.
 * @param[in] mask 16-bit mask indicating the position of the field with bits of 1s.
 * @param[in] lsb  LSB of the field.
 * @param[in] val  16bit value to be written.
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_v1l8p1_pmd_mwr_reg(srds_access_t *sa__, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val);

/** Write to a PRAM location for the currently selected Serdes IP Core.
 *  The address is auto-incrementing, per the PRAM interface specification.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] val Value to be written.
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_v1l8p1_pmd_wr_pram(srds_access_t *sa__, uint8_t val);

/** Write message to the logger with the designated verbose level.
 * Output is sent to stdout and a logfile.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] message_verbose_level Verbose level for the current message.
 * @param[in] format Format string as in printf.
 * @param[in] ... Additional variables used as in printf.
 * @return Error code generated by function (returns ERR_CODE_NONE if no errors).
 */
#ifndef SWIG_CMD_PROC
int logger_write(srds_access_t *sa__, int message_verbose_level, const char *format, ...);


#endif

/** Delay the execution of the code for at least the specified amount of time in nanoseconds.
 * This function is used ONLY for delays less than 1 microsecond, non-zero error code may be returned otherwise.
 * The user can implement this as an empty function if their register access latency exceeds 1 microsecond.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] delay_ns Delay in nanoseconds.
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_v1l8p1_delay_ns(srds_access_t *sa__, uint16_t delay_ns);

/** Delay the execution of the code for at least the specified amount of time in microseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 microsecond.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] delay_us Delay in microseconds.
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_v1l8p1_delay_us(srds_access_t *sa__, uint32_t delay_us);

/** Delay the execution of the code for at least the specified amount of time in milliseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 millisecond.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] delay_ms Delay in milliseconds.
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors).
 */
err_code_t blackhawk7_v1l8p1_delay_ms(srds_access_t *sa__, uint32_t delay_ms);

/** Return the address of current selected Serdes IP Core.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return the IP level address of the current core.
 */
uint8_t blackhawk7_v1l8p1_get_core(srds_access_t *sa__);

/** Return the logical address of current selected Serdes IP lane.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return the IP level address of the current lane. 0 to N-1, for an N lane IP.
 */
uint8_t blackhawk7_v1l8p1_get_lane(srds_access_t *sa__);

#define blackhawk7_v1l8p1_get_physical_lane blackhawk7_v1l8p1_get_lane

/** Set the logical address of current selected Serdes IP lane.  Used in diagnostic
 * and core-level management functions.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] lane_index is the lane index.
 * @return Any error code generated during execution; ERR_CODE NONE otherwise.
 */
err_code_t blackhawk7_v1l8p1_set_lane(srds_access_t *sa__, uint8_t lane_index);

#define blackhawk7_v1l8p1_set_physical_lane blackhawk7_v1l8p1_set_lane

/** Return the address of current selected Serdes IP PLL.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return the IP level address of the current PLL. 0 to N-1, for an N-PLL IP.
 */
uint8_t blackhawk7_v1l8p1_get_pll_idx(srds_access_t *sa__);

/** Set the address of current selected Serdes IP PLL. Its purpose is to select which
 * PLL's registers is accessed when accessing AMS_PLL_COM, CORE_PLL_COM, & PLL_CAL_COM.
 * This is a selection similar to selecting lane registers.
 * Used in diagnostic and core-level management functions.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] pll_index is the PLL index.
 * @return Any error code generated during execution; ERR_CODE NONE otherwise.
 */
err_code_t blackhawk7_v1l8p1_set_pll_idx(srds_access_t *sa__, uint8_t pll_index);

/** Return the index of current selected Serdes IP micro.
 * NOTE: The micro_idx is selected using same bits as PLL selection in AER or PMI address.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return the IP level index of the current micro. 0 to N-1, for an N-micro IP.
 */
uint8_t blackhawk7_v1l8p1_get_micro_idx(srds_access_t *sa__);

/** Set the index of current selected Serdes IP micro. Its purpose is to select which
 * micro's registers is accessed when accessing MICRO_E_COM.This is a selection similar to
 * selecting lane registers.
 * Used in diagnostic and core-level management functions.
 * NOTE: The micro_idx is selected using same bits as PLL selection in AER or PMI address.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @param[in] micro_index is the micro index.
 * @return Any error code generated during execution; ERR_CODE NONE otherwise.
 */
err_code_t blackhawk7_v1l8p1_set_micro_idx(srds_access_t *sa__, uint8_t micro_index);

#if defined(SERDES_EXTERNAL_INFO_TABLE_EN)
/** Returns address of blackhawk7_v1l8p1 info table stored in upper level software.
 * @param[in] sa__ is an opaque state vector passed through to device access functions.
 * @return Pointer to the blackhawk7_v1l8p1 info table.
 */
srds_info_t *blackhawk7_v1l8p1_get_info_table_address(srds_access_t *sa__);
#endif

uint16_t blackhawk7_v1l8p1_pmd_rd_reg(srds_access_t *sa__, uint16_t address);

/*! @} SerdesAPIDependenciesTag */
/*! @} APITag */
#endif /* SERDES_API_DEPENDENCIES_H */
#ifdef __cplusplus
}
#endif
