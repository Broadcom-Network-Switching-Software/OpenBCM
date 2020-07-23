
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <sal/core/thread.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

typedef struct
{
    
    uint8 is_appl_deinit;
    int counter[UTILEX_SEQ_ALLOW_NOF];
} dnxc_allow_write_t;

dnxc_allow_write_t dnxc_allow_write_db[SOC_MAX_NUM_DEVICES] = { {0} };


char *
dnxc_ha_access_type_str(
    utilex_seq_allow_access_e access_type)
{
    static char *access_type_str[] = { "dbal", "swstate", "schan" };
    assert(access_type < UTILEX_SEQ_ALLOW_NOF);
    return access_type_str[access_type];
}

int
dnxc_ha_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_ha_tid_set(unit));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);


    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_tid_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);


    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_tid_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);


    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_appl_deinit_state_change(
    int unit,
    uint8 is_start)
{
    SHR_FUNC_INIT_VARS(unit);


    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_appl_is_deinit(
    int unit,
    uint8 *is_deinit)
{
    SHR_FUNC_INIT_VARS(unit);


    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_ha_is_access_disabled(
    int unit,
    utilex_seq_allow_access_e access_type)
{


    return FALSE;
}

static int
dnxc_ha_tmp_allow_access_change(
    int unit,
    utilex_seq_allow_access_e access_type,
    uint8 is_enable)
{
    SHR_FUNC_INIT_VARS(unit);


    SHR_EXIT();

exit:

    SHR_FUNC_EXIT;
}

int
dnxc_ha_tmp_allow_access_enable(
    int unit,
    utilex_seq_allow_access_e access_type)
{
    return dnxc_ha_tmp_allow_access_change(unit, access_type, TRUE);
}

int
dnxc_ha_tmp_allow_access_disable(
    int unit,
    utilex_seq_allow_access_e access_type)
{
    return dnxc_ha_tmp_allow_access_change(unit, access_type, FALSE);
}

#undef _ERR_MSG_MODULE_NAME
