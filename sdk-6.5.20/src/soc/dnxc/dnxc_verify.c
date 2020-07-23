
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/dnxc/dnxc_verify.h>
#include <soc/drv.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv_dnxc_utils.h>


int dnxc_verify_allowed[SOC_MAX_NUM_DEVICES] = { 0 };

int dnxc_verify_allowed_fast_init_flag = FALSE;
int dnxc_verify_allowed_verify_init_flag = FALSE;

void
dnxc_verify_allowed_init(
    int unit)
{
    int value = 1;

    if (dnxc_verify_allowed_fast_init_flag == TRUE)
    {
        dnxc_verify_allowed_set(unit, FALSE);
        return;
    }

    if (dnxc_verify_allowed_verify_init_flag == TRUE)
    {
        dnxc_verify_allowed_set(unit, TRUE);
        return;
    }


    
    dnxc_verify_allowed_set(unit, value);

    return;
}

void
dnxc_verify_allowed_set(
    int unit,
    int value)
{
    dnxc_verify_allowed[unit] = value;
    return;
}

int
dnxc_verify_allowed_get(
    int unit)
{
    return dnxc_verify_allowed[unit];
}

void
dnxc_verify_fast_init_enable(
    int enable)
{
    if (enable)
    {
        dnxc_verify_allowed_fast_init_flag = TRUE;
    }
    else
    {
        dnxc_verify_allowed_verify_init_flag = TRUE;
    }
}
