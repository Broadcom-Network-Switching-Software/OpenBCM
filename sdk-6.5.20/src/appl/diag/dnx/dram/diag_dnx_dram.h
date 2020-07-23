/*! \file diag_dnx_dram.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_DRAM_H_INCLUDED
#define DIAG_DNX_DRAM_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <bcm_int/dnx/dram/dram.h>

/*************
 *  DEFINES  *
 */
/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_dram_man;
extern sh_sand_cmd_t sh_dnx_dram_cmds[];

/*************
 * FUNCTIONS *
 */

/**
 * \brief - CB for shell commands infra to check if dram is available
 *
 * \param [in] unit - unit number
 * \param [in] test_list - test_list
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_cmd_is_dram_available(
    int unit,
    rhlist_t * test_list);

/**
 * \brief - print vendor info for given dram index
 *
 * \param [in] unit - unit number
 * \param [in] dram_index - dram index
 * \param [in] vendor_info - vendor info to print;
 * \param [out] prt_ctr - print control pointer for the prints;
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_dnx_dram_vendor_info_print(
    int unit,
    int dram_index,
    dnx_dram_vendor_info_t * vendor_info,
    prt_control_t * prt_ctr);

#endif /* DIAG_DNX_DRAM_H_INCLUDED */
