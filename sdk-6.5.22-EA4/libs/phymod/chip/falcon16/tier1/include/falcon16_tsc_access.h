/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/***********************************************************************************
 ***********************************************************************************
 *                                                                                 *
 *  Revision      :   *
 *                                                                                 *
 *  Description   :  Implementation of API uC access functions                     *
 */

/** @file falcon16_tsc_access.h
 * Implementation of API uC access functions
 */

#ifndef FALCON16_TSC_API_ACCESS_H
#define FALCON16_TSC_API_ACCESS_H

#include "falcon16_tsc_ipconfig.h"
#include "common/srds_api_err_code.h"
#include "common/srds_api_uc_common.h"
#include "common/srds_api_types.h"
#include "falcon16_tsc_usr_includes.h"

typedef struct {
    uint32_t ucode_size;
    uint16_t stack_size;
    uint16_t crc_value;
} ucode_info_t;

/*---------------------------------------------*/
/*  Serdes IP RAM access - Lane RAM Variables  */
/*---------------------------------------------*/
/*          rd - read; wr - write              */
/*          b  - byte; w  - word               */
/*          l  - lane; c  - core               */
/*          uc - micro                         */
/*          s  - signed                        */
/*---------------------------------------------*/
/** Unsigned Byte Read of a uC RAM Lane variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM lane variable to be read
 * @return 8bit unsigned value read from uC RAM
 */
uint8_t falcon16_tsc_rdbl_uc_var(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Unsigned Word Read of a uC RAM Lane variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM lane variable to be read
 * @return 16bit unsigned value read from uC RAM
 */
uint16_t falcon16_tsc_rdwl_uc_var(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Unsigned Byte Write of a uC RAM Lane variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM lane variable to be written
 * @param wr_val 8bit unsigned value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrbl_uc_var(srds_access_t *sa__, uint16_t addr, uint8_t wr_val);

/** Unsigned Word Write of a uC RAM Lane variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM lane variable to be written
 * @param wr_val 16bit unsigned value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrwl_uc_var(srds_access_t *sa__, uint16_t addr, uint16_t wr_val);


/* Signed version of above 4 functions */

/** Signed Byte Read of a uC RAM Lane variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM lane variable to be read
 * @return 8bit signed value read from uC RAM
 */
int8_t falcon16_tsc_rdbls_uc_var(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Signed Word Read of a uC RAM Lane variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM lane variable to be read
 * @return 16bit signed value read from uC RAM
 */
int16_t falcon16_tsc_rdwls_uc_var(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Signed Byte Write of a uC RAM Lane variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM lane variable to be written
 * @param wr_val 8bit signed value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrbls_uc_var(srds_access_t *sa__, uint16_t addr, int8_t wr_val);

/** Signed Word Write of a uC RAM Lane variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM lane variable to be written
 * @param wr_val 16bit signed value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrwls_uc_var(srds_access_t *sa__, uint16_t addr, int16_t wr_val);




/*---------------------------------------------*/
/*  Serdes IP RAM access - Core RAM Variables  */
/*---------------------------------------------*/
/** Unsigned Byte Read of a uC RAM Core variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM core variable to be read
 * @return 8bit unsigned value read from uC RAM
 */
uint8_t falcon16_tsc_rdbc_uc_var(srds_access_t *sa__, err_code_t *err_code, uint8_t addr);

/** Unsigned Word Read of a uC RAM Core variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM core variable to be read
 * @return 16bit unsigned value read from uC RAM
 */
uint16_t falcon16_tsc_rdwc_uc_var(srds_access_t *sa__, err_code_t *err_code, uint8_t addr);

/** Unsigned Byte Write of a uC RAM Core variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM core variable to be written
 * @param wr_val 8bit unsigned value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrbc_uc_var(srds_access_t *sa__, uint8_t addr, uint8_t wr_val);

/** Unsigned Word Write of a uC RAM Core variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM core variable to be written
 * @param wr_val 16bit unsigned value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrwc_uc_var(srds_access_t *sa__, uint8_t addr, uint16_t wr_val);


/* Signed version of above 4 functions */

/** Signed Byte Read of a uC RAM Core variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM core variable to be read
 * @return 8bit signed value read from uC RAM
 */
int8_t falcon16_tsc_rdbcs_uc_var(srds_access_t *sa__, err_code_t *err_code, uint8_t addr);

/** Signed Word Read of a uC RAM Core variable.
 * Read access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM core variable to be read
 * @return 16bit signed value read from uC RAM
 */
int16_t falcon16_tsc_rdwcs_uc_var(srds_access_t *sa__, err_code_t *err_code, uint8_t addr);

/** Signed Byte Write of a uC RAM Core variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM core variable to be written
 * @param wr_val 8bit signed value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrbcs_uc_var(srds_access_t *sa__, uint8_t addr, int8_t wr_val);

/** Signed Word Write of a uC RAM Core variable.
 * Write access through Micro Register Interface for PMD IPs; through Serdes FW DSC Command Interface for external micro IPs.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM core variable to be written
 * @param wr_val 16bit signed value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrwcs_uc_var(srds_access_t *sa__, uint8_t addr, int16_t wr_val);


/*---------------------------------------------------*/
/*  Micro Commands through uC DSC Command Interface  */
/*---------------------------------------------------*/
/** Issue a Micro command through the uC DSC Command Interface and return control immediately.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param cmd Micro command to be issued
 * @param supp_info Supplement information for the Micro command to be issued (RAM read/write address or Micro Control command)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_uc_cmd_return_immediate(srds_access_t *sa__, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info);

/** Issue a Micro command through the uC DSC Command Interface. Control returned only after command is finished.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param cmd Micro command to be issued
 * @param supp_info Supplement information for the Micro command to be issued (RAM read/write address or Micro Control command)
 * @param timeout_ms Time interval in milliseconds inside which the command should be completed; else error issued
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_uc_cmd(srds_access_t *sa__, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint32_t timeout_ms);

/** Issue a Micro command with data through the uC DSC Command Interface and return control immediately.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param cmd Micro command to be issued
 * @param supp_info Supplement information for the Micro command to be issued (RAM write address)
 * @param data Data to be written to dsc_data for use by uC
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_uc_cmd_with_data_return_immediate(srds_access_t *sa__, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint16_t data);

/** Issue a Micro command with data through the uC DSC Command Interface. Control returned only after command is finished.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param cmd Micro command to be issued
 * @param supp_info Supplement information for the Micro command to be issued (RAM write address)
 * @param data Data to be written to dsc_data for use by uC
 * @param timeout_ms Time interval in milliseconds inside which the command should be completed; else error issued
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_uc_cmd_with_data(srds_access_t *sa__, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint16_t data, uint32_t timeout_ms);

/** Issue a Micro Control command through the uC DSC Command Interface and return control immediately.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param control Micro Control command to be issued
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_uc_control_return_immediate(srds_access_t *sa__, enum srds_pmd_uc_ctrl_cmd_enum control);

/** Issue a Micro Control command through the uC DSC Command Interface. Control returned only after command is finished.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param control Micro Control command to be issued
 * @param timeout_ms Time interval in milliseconds inside which the command should be completed; else error issued
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_uc_control(srds_access_t *sa__, enum srds_pmd_uc_ctrl_cmd_enum control, uint32_t timeout_ms);

/** Issue a Micro Control command through the uC DSC DIAG_EN Command Interface. Control returned only after command is finished.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param control Micro DIAG Control command to be issued
 * @param timeout_ms Time interval in milliseconds inside which the command should be completed; else error issued
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_pmd_uc_diag_cmd(srds_access_t *sa__, enum srds_pmd_uc_diag_cmd_enum control, uint32_t timeout_ms);

/*-------------------------------------------------------*/
/*  Program RAM access through Micro Register Interface  */
/*-------------------------------------------------------*/

/** Unsigned Long Read of a uC Program RAM variable through Micro Register Interface.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of Program RAM variable to be read
 * @return 32bit unsigned value read from uC Program RAM
 */
uint32_t falcon16_tsc_rd_long_uc_prog_ram(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Block uC Program RAM read through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *mem Pointer to memory location where the read values from the Block RAM read should be stored
 * @param addr Starting Address of Program RAM block to be read
 * @param cnt Number of byte locations to be read from the block
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_rdblk_uc_prog_ram(srds_access_t *sa__, uint8_t *mem, uint16_t addr, uint16_t cnt);

/*-----------------------------------------------*/
/*  RAM access through Micro Register Interface  */
/*-----------------------------------------------*/
/*           rd - read; wr - write               */
/*           b  - byte; w  - word                */
/*           l  - lane; c  - core                */
/*           s  - signed                         */
/*-----------------------------------------------*/

/** Unsigned Long Write of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM variable to be written
 * @param wr_val 32bit unsigned value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wr_long_uc_ram(srds_access_t *sa__, uint16_t addr, uint32_t wr_val);

/** Unsigned Word Write of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM variable to be written
 * @param wr_val 16bit unsigned value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrw_uc_ram(srds_access_t *sa__, uint16_t addr, uint16_t wr_val);

/** Unsigned Byte Write of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM variable to be written
 * @param wr_val 8bit unsigned value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrb_uc_ram(srds_access_t *sa__, uint16_t addr, uint8_t wr_val);

/** Unigned Long Read of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM variable to be read
 * @return 32bit unsigned value read from uC RAM
 */
uint32_t falcon16_tsc_rd_long_uc_ram(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Unigned Word Read of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM variable to be read
 * @return 16bit unsigned value read from uC RAM
 */
uint16_t falcon16_tsc_rdw_uc_ram(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Unigned Byte Read of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM variable to be read
 * @return 8bit unsigned value read from uC RAM
 */
uint8_t falcon16_tsc_rdb_uc_ram(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);


/* Signed versions of above 4 functions */

/** Signed Word Write of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM variable to be written
 * @param wr_val 16bit signed value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrws_uc_ram(srds_access_t *sa__, uint16_t addr, int16_t wr_val);

/** Signed Byte Write of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param addr Address of RAM variable to be written
 * @param wr_val 8bit signed value to be written to RAM variable
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_wrbs_uc_ram(srds_access_t *sa__, uint16_t addr, int8_t wr_val);

/** Signed Word Read of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM variable to be read
 * @return 16bit signed value read from uC RAM
 */
int16_t falcon16_tsc_rdws_uc_ram(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Signed Byte Read of a uC RAM variable through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *err_code Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * @param addr Address of RAM variable to be read
 * @return 8bit signed value read from uC RAM
 */
int8_t falcon16_tsc_rdbs_uc_ram(srds_access_t *sa__, err_code_t *err_code, uint16_t addr);

/** Block RAM read through Micro Register Interface.
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *mem Pointer to memory location where the read values from the Block RAM read should be stored
 * @param addr Starting Address of RAM block to be read
 * @param cnt Number of byte locations to be read from the block
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_rdblk_uc_ram(srds_access_t *sa__, uint8_t *mem, uint16_t addr, uint16_t cnt);

/*-----------------------*/
/*  Temperature reading  */
/*-----------------------*/

/** Read die temperature in degrees Ceisius (as integer)
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *die_temp  Die temperature in degrees Celsius
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_read_die_temperature (srds_access_t *sa__, int16_t *die_temp);

/** Read die temperature in degrees Ceisius (as double)
 * @param sa__ is an opaque state vector passed through to device access functions.
 * @param *die_temp  Die temperature in degrees Celsius (as double)
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 */
err_code_t falcon16_tsc_read_die_temperature_double (srds_access_t *sa__, USR_DOUBLE *die_temp);

#endif
