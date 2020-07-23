/*! \file bcmtm_imm_oobfc_init.c
 *
 * TM OOBFC imm callback function register.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/oobfc/bcmtm_oobfc_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_INIT

/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_oobfc_cb_reg(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*! OOBFC unicast queue profile map imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_oobfc_ucq_profile_imm_register(unit));

    /*! OOBFC multicast queue profile map imm callback. */
    SHR_IF_ERR_EXIT
        (bcmtm_oobfc_mcq_profile_imm_register(unit));

    SHR_IF_ERR_EXIT
        (bcmtm_imm_oobfc_port_imm_register(unit));
exit:
    SHR_FUNC_EXIT();
}
