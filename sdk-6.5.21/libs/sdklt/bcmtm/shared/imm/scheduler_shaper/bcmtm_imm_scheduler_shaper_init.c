/*! \file bcmtm_imm_scheduler_shaper_init.c
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
bcmtm_imm_scheduler_shaper_cb_reg(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* TM port shaper imm callback */
    SHR_IF_ERR_EXIT
        (bcmtm_shaper_port_imm_register(unit));

    /* TM node shaper imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_shaper_node_imm_register(unit));

    /* TM node scheduler imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_node_imm_register(unit));

    /* TM scheduler profile imm callback */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_imm_register(unit));

    /* TM node scheduler port imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_port_imm_register(unit));

    /* TM cpu node scheduler shaper imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_shaper_cpu_node_imm_register(unit));

    /* TM scheduler shaper node imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_shaper_node_imm_register(unit));

    /* TM scheduler strict priority profile imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_sp_profile_imm_register(unit));
exit:
    SHR_FUNC_EXIT();
}
