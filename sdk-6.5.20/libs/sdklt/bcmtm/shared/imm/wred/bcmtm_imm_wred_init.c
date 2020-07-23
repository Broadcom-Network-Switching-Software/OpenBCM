/*! \file bcmtm_imm_wred_init.c
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

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_wred_cb_reg(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* TM wred unicast queue imm callback */
    SHR_IF_ERR_EXIT(bcmtm_wred_ucq_imm_register(unit));

    /* TM wred port sp imm callback. */
    SHR_IF_ERR_EXIT(bcmtm_wred_portsp_imm_register(unit));

    /* TM wred congestion profile imm callback. */
    SHR_IF_ERR_EXIT(bcmtm_wred_cng_profile_imm_register(unit));
exit:
    SHR_FUNC_EXIT();
}
