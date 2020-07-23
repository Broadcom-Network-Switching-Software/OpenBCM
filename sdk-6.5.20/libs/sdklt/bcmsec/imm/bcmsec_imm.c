/*! \file bcmsec_imm.c
 *
 * SEC imm callback function register.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_IMM

/*******************************************************************************
 * Public functions
 */
int
bcmsec_imm_cb_reg(int unit)
{

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_pm_control_imm_register(unit));
    SHR_IF_ERR_EXIT
        (bcmsec_decrypt_mgmt_imm_register(unit));
    SHR_IF_ERR_EXIT
        (bcmsec_sa_policy_encrypt_imm_register(unit));
    SHR_IF_ERR_EXIT
        (bcmsec_sa_policy_decrypt_imm_register(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_imm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

