/*! \file bcmtm_imm_obm_init.c
 *
 * TM scheduler shaper imm callback function register.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE  BSL_LS_BCMTM_INIT
/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_obm_cb_reg(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*! OBM port flow control imm callback. */
    SHR_IF_ERR_EXIT(bcmtm_obm_port_fc_imm_register(unit));

    /*! OBM port packet priority to traffic class map imm callback. */
    SHR_IF_ERR_EXIT(bcmtm_obm_port_pkt_pri_tc_imm_register(unit));

    /*! OBM port control imm callback. */
    SHR_IF_ERR_EXIT(bcmtm_obm_port_pkt_parse_imm_register(unit));

    /*! OBM policy table imm callback. */
    SHR_IF_ERR_EXIT(bcmtm_obm_pc_pm_pkt_parse_imm_register(unit));

exit:
    SHR_FUNC_EXIT();
}
