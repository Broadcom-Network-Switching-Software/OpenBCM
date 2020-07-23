/** \file stk/stk_domain.c
 *
 * Stacking domain procedures.
 *
 * modue id, domain id ....
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STK
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/stack.h>
#include <bcm/trunk.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/stk/stk_domain.h>

#include <soc/dnx/dbal/dbal.h>

#include <bcm_int/dnx/port/port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>

#include <shared/shrextend/shrextend_debug.h>

/**
 * \brief - Verify tm domain id.
 */
int
dnx_stk_domain_tm_domain_verify(
    int unit,
    int tm_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify tm_domain
     */
    if ((tm_domain < 0) || (tm_domain >= dnx_data_stack.general.nof_tm_domains_max_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "tm_domain %d is out of range [0, %d].",
                     tm_domain, dnx_data_stack.general.nof_tm_domains_max_get(unit) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify module id.
 */
int
dnx_stk_domain_modid_verify(
    int unit,
    int modid)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify modid
     */
    if ((modid < 0) || (modid >= dnx_data_device.general.nof_faps_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "modid %d is out of range [0, %d].",
                     modid, dnx_data_device.general.nof_faps_get(unit) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_domain_modid_to_domain_set(
    int unit,
    uint32 modid,
    uint32 tm_domain,
    uint32 is_exist)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_MODID_TO_DOMAIN_SW_STATE, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MODULE_ID, modid);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DOMAIN_ID, tm_domain);

    /** Set value */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXIST, INST_SINGLE, is_exist);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_domain_modid_to_domain_get(
    int unit,
    uint32 modid,
    uint32 tm_domain,
    uint32 *is_exist)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_MODID_TO_DOMAIN_SW_STATE, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MODULE_ID, modid);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DOMAIN_ID, tm_domain);

    /** Get value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXIST, INST_SINGLE, is_exist);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API dnx_stk_domain_stk_modid_to_domain_find().
 *          For details about API parameters refer to dnx_stk_domain_stk_modid_to_domain_find() description.
 */
int
dnx_stk_domain_stk_modid_to_domain_find_verify(
    int unit,
    uint32 local_modid,
    uint32 sysport_modid,
    uint32 *is_same_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify local_modid */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_modid_verify(unit, local_modid));

    /** Verify sysport_modid */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_modid_verify(unit, sysport_modid));

    SHR_NULL_CHECK(is_same_domain, _SHR_E_PARAM, "is_same_domain");

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See .h file
 */
shr_error_e
dnx_stk_domain_stk_modid_to_domain_find(
    int unit,
    uint32 local_modid,
    uint32 sysport_modid,
    uint32 *is_same_domain)
{
    int local_tmd = -1;
    int sysport_tmd = -1;
    int domain_id = 0;
    uint32 local_modid_exist = 0;
    uint32 sysport_modid_exist = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX
        (dnx_stk_domain_stk_modid_to_domain_find_verify(unit, local_modid, sysport_modid, is_same_domain));

    *is_same_domain = 0x0;

    for (domain_id = 0; domain_id < dnx_data_stack.general.nof_tm_domains_max_get(unit); domain_id++)
    {

        SHR_IF_ERR_EXIT(dnx_stk_domain_modid_to_domain_get(unit, local_modid, domain_id, &local_modid_exist));
        SHR_IF_ERR_EXIT(dnx_stk_domain_modid_to_domain_get(unit, sysport_modid, domain_id, &sysport_modid_exist));

        if (local_modid_exist != 0x0)
        {
            local_tmd = domain_id;
        }
        if (sysport_modid_exist != 0x0)
        {
            sysport_tmd = domain_id;
        }
    }

    if ((local_tmd == -1) || (sysport_tmd == -1) || (local_tmd == sysport_tmd))
    {
        *is_same_domain = 0x1;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_domain_stk_domain_to_trunk_set(
    int unit,
    uint32 tm_domain,
    uint32 stk_tid,
    uint32 is_exist)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_DOMAIN_TO_STACK_TRUNK_SW_STATE, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DOMAIN_ID, tm_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STK_TGID, stk_tid);

    /** Set value */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXIST, INST_SINGLE, is_exist);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_domain_stk_domain_to_trunk_get(
    int unit,
    uint32 tm_domain,
    uint32 stk_tid,
    uint32 *is_exist)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_DOMAIN_TO_STACK_TRUNK_SW_STATE, &entry_handle_id));
    /** key construction */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DOMAIN_ID, tm_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STK_TGID, stk_tid);

    /** Get value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EXIST, INST_SINGLE, is_exist);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API dnx_stk_domain_stk_domain_modid_get().
 *          For details about API parameters refer to dnx_stk_domain_stk_domain_modid_get() description.
 */
int
dnx_stk_domain_stk_domain_modid_get_verify(
    int unit,
    uint32 modid,
    int *tm_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify modid */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_modid_verify(unit, modid));

    SHR_NULL_CHECK(tm_domain, _SHR_E_PARAM, "tm_domain");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_domain_stk_domain_modid_get(
    int unit,
    uint32 modid,
    int *tm_domain)
{
    int domain_id = 0;
    uint32 is_exist = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_stk_domain_modid_get_verify(unit, modid, tm_domain));

    *tm_domain = -1;

    /*
     * find in which tm domain the gport is
     */
    for (domain_id = 0; domain_id < dnx_data_stack.general.nof_tm_domains_max_get(unit); domain_id++)
    {

        SHR_IF_ERR_EXIT(dnx_stk_domain_modid_to_domain_get(unit, modid, domain_id, &is_exist));
        if (is_exist != 0x0)
        {
            *tm_domain = domain_id;
            break;
        }
    }

    if (*tm_domain == -1)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Failed to find TM-domain for modid=%d", modid);
    }

exit:
    SHR_FUNC_EXIT;
}
