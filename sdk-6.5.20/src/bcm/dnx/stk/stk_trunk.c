/** \file stk/stk_trunk.c
 *
 * Stacking trunk procedures.
 *
 *  stacking trunk , stacking fec  ...
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
#include <bcm_int/dnx/stk/stk_trunk.h>
#include <bcm_int/dnx/stk/stk_domain.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>

#include <soc/dnx/dbal/dbal.h>

#include <bcm_int/dnx/port/port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/trunk.h>

/** Invalid queue definition */
#define DEST_PORT_INVALID_BASE_QUEUE(unit) dnx_data_stack.general.dest_port_base_queue_max_get(unit)
/** Max number of members */
#define MAX_NOF_MEMBERS(unit) dnx_data_stack.general.dest_port_base_queue_max_get(unit)

/**
 * \brief - Verify stack trunk id.
 */
int
dnx_stk_trunk_trunk_id_verify(
    int unit,
    bcm_trunk_t stk_trunk)
{
    uint32 stk_tid = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify stack trunk id.
     */
    stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk);
    if (stk_tid >= dnx_data_stack.general.nof_tm_domains_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "stk_trunk is invalid");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API dnx_stk_trunk_domain_get().
 *          For details about API parameters refer to dnx_stk_trunk_domain_get() description.
 */
int
dnx_stk_trunk_domain_get_verify(
    int unit,
    int tm_domain,
    int stk_trunk_max,
    bcm_trunk_t * stk_trunk_array,
    int *stk_trunk_count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm_domain */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_tm_domain_verify(unit, tm_domain));

    /*
     * Input parameters verify.
     */
    SHR_NULL_CHECK(stk_trunk_array, _SHR_E_PARAM, "stk_trunk_array");
    SHR_NULL_CHECK(stk_trunk_count, _SHR_E_PARAM, "stk_trunk_count");
    if (stk_trunk_max <= 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "stk_trunk_max is invalid");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_domain_get(
    int unit,
    int tm_domain,
    int stk_trunk_max,
    bcm_trunk_t * stk_trunk_array,
    int *stk_trunk_count)
{
    uint32 stk_tid_ndx = 0;
    uint32 stk_tid_count = 0;
    bcm_trunk_t stk_tid = 0;
    uint32 is_exist = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX
        (dnx_stk_trunk_domain_get_verify(unit, tm_domain, stk_trunk_max, stk_trunk_array, stk_trunk_count));

    for (stk_tid_ndx = 0; stk_tid_ndx < dnx_data_stack.general.nof_tm_domains_max_get(unit); stk_tid_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_stk_domain_stk_domain_to_trunk_get(unit, tm_domain, stk_tid_ndx, &is_exist));

        if (is_exist == 0x1)
        {
            BCM_TRUNK_STACKING_TID_SET(stk_tid, stk_tid_ndx);
            stk_trunk_array[stk_tid_count++] = stk_tid;
        }
        is_exist = 0;

        if (stk_tid_count == stk_trunk_max)
        {
            break;
        }
    }

    *stk_trunk_count = stk_tid_count;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_domain_fec_map_update(
    int unit,
    int tm_domain)
{
    int index = 0;
    int entry = 0;
    bcm_trunk_t stk_tid = 0;
    bcm_trunk_t *stk_trunk_array_p = NULL;
    int stk_trunk_count = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm_domain */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_tm_domain_verify(unit, tm_domain));

    SHR_ALLOC_SET_ZERO(stk_trunk_array_p,
                       sizeof(*stk_trunk_array_p) * dnx_data_stack.general.nof_tm_domains_max_get(unit),
                       "stk_trunk_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_stk_trunk_domain_get
                    (unit, tm_domain, dnx_data_stack.general.nof_tm_domains_max_get(unit),
                     stk_trunk_array_p, &stk_trunk_count));

    if (stk_trunk_count > dnx_data_stack.general.lag_stack_fec_resolve_entry_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "try to add more than 4 stacking trunk to the same domain");
    }

    /*
     * if no stk trunk defined restore it to init values
     */
    if (stk_trunk_count == 0)
    {
        stk_trunk_array_p[0] = tm_domain;
        stk_trunk_count = 1;
    }

    for (entry = 0; entry < dnx_data_stack.general.lag_stack_fec_resolve_entry_max_get(unit); entry++)
    {

        stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk_array_p[index]);

        SHR_IF_ERR_EXIT(dnx_stk_trunk_fec_map_stack_lag_set(unit, tm_domain, entry, stk_tid));

        index = (index + 1) % stk_trunk_count;
    }

exit:
    SHR_FREE(stk_trunk_array_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API dnx_stk_trunk_fec_map_stack_lag_set().
 *          For details about API parameters refer to dnx_stk_trunk_fec_map_stack_lag_set() description.
 */
int
dnx_stk_trunk_fec_map_stack_lag_set_verify(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 stack_lag)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * stack_lag_domain_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if ((tm_domain < dnx_data_stack.general.lag_domain_min_get(unit))
        || (tm_domain > dnx_data_stack.general.lag_domain_max_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "tm_domain is invalid");
    }

    /*
     * stack_lag_stack_fec_resolve_entry_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if (((entry < dnx_data_stack.general.lag_stack_fec_resolve_entry_min_get(unit))
         || (entry > dnx_data_stack.general.lag_stack_fec_resolve_entry_max_get(unit)))
        && (entry != dnx_data_stack.general.lag_stack_fec_resolve_entry_all_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "entry is invalid");
    }

    /*
     * Verify stack_lag
     */
    if (stack_lag >= dnx_data_stack.general.nof_tm_domains_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "stack_lag is invalid");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_fec_map_stack_lag_set(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 stack_lag)
{
    uint32 nof_entries = 0;
    uint32 offset = 0;
    uint32 entry_offset = 0;
    uint32 entry_idx = 0;
    uint32 fec_value = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_fec_map_stack_lag_set_verify(unit, tm_domain, entry, stack_lag));

    /*
     * Write entries to Stack fec resolve table
     */
    if (entry == dnx_data_stack.general.lag_stack_fec_resolve_entry_all_get(unit))
    {
        nof_entries = 16;
        offset = 0;
    }
    else
    {
        nof_entries = 4;
        offset = entry * 4;
    }

    for (entry_idx = 0; entry_idx < nof_entries; entry_idx++)
    {
        /*
         *  stacking port are multiple by 4 to create more entries per domain in the stack_resolve_ table.
         */
        entry_offset = offset + (entry_idx % nof_entries);
        fec_value = stack_lag * 4 + (entry_idx % 4);
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_fec_resolve_set(unit, entry_offset, tm_domain, fec_value));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API dnx_stk_trunk_fec_map_stack_lag_get().
 *          For details about API parameters refer to dnx_stk_trunk_fec_map_stack_lag_get() description.
 */
int
dnx_stk_trunk_fec_map_stack_lag_get_verify(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 *stack_lag)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(stack_lag, _SHR_E_PARAM, "stack_lag");

    /*
     * stack_lag_domain_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if ((tm_domain < dnx_data_stack.general.lag_domain_min_get(unit))
        || (tm_domain > dnx_data_stack.general.lag_domain_max_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "tm_domain is invalid");
    }

    /*
     * stack_lag_stack_fec_resolve_entry_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if (((entry < dnx_data_stack.general.lag_stack_fec_resolve_entry_min_get(unit))
         || (entry > dnx_data_stack.general.lag_stack_fec_resolve_entry_max_get(unit)))
        && (entry != dnx_data_stack.general.lag_stack_fec_resolve_entry_all_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "entry is invalid");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_fec_map_stack_lag_get(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 *stack_lag)
{
    uint32 entry_offset = 0;
    uint32 fec_value = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_fec_map_stack_lag_get_verify(unit, tm_domain, entry, stack_lag));

    /*
     * Get entries from Stack fec resolve table
     */
    if (entry == dnx_data_stack.general.lag_stack_fec_resolve_entry_all_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "_SHR_E_INTERNAL");
    }
    else
    {
        entry_offset = entry * 4;
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_fec_resolve_get(unit, entry_offset, tm_domain, &fec_value));
    }

    /*
     * stack_lag divided to 4, because in set it was multiple by four.
     */
    *stack_lag = fec_value / 4;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_stack_fec_resolve_set(
    int unit,
    uint32 flow_id,
    uint32 lag_lb_key,
    uint32 stack_lag)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_FEC_RESOLVE_TABLE, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY, lag_lb_key);

    /*
     * Set value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STACK_LAG, INST_SINGLE, stack_lag);

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
dnx_stk_trunk_stack_fec_resolve_get(
    int unit,
    uint32 flow_id,
    uint32 lag_lb_key,
    uint32 *stack_lag)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_FEC_RESOLVE_TABLE, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY, lag_lb_key);

    /** Get value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STACK_LAG, INST_SINGLE, stack_lag);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API dnx_stk_trunk_stack_lag_packets_base_queue_id_set().
 *          For details about API parameters refer to dnx_stk_trunk_stack_lag_packets_base_queue_id_set() description.
 */
int
dnx_stk_trunk_stack_lag_packets_base_queue_id_set_verify(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 base_queue)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * stack_lag_domain_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if ((tm_domain < dnx_data_stack.general.lag_domain_min_get(unit))
        || (tm_domain > dnx_data_stack.general.lag_domain_max_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "tm_domain is invalid");
    }

    /*
     * stack_lag_stack_trunk_resolve_entry_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if (((entry < dnx_data_stack.general.lag_stack_trunk_resolve_entry_min_get(unit))
         || (entry > dnx_data_stack.general.lag_stack_trunk_resolve_entry_max_get(unit)))
        && (entry != dnx_data_stack.general.lag_stack_trunk_resolve_entry_all_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "entry is invalid");
    }

    /*
     * verify base_queue
     */
    if (base_queue > dnx_data_stack.general.dest_port_base_queue_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "base_queue is invalid");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_stack_lag_packets_base_queue_id_set(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 base_queue)
{
    uint32 entry_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_stack_lag_packets_base_queue_id_set_verify(unit, tm_domain, entry, base_queue));

    if (entry == dnx_data_stack.general.lag_stack_trunk_resolve_entry_all_get(unit))
    {
        for (entry_index = 0;
             entry_index < dnx_data_stack.general.lag_stack_trunk_resolve_entry_max_get(unit); entry_index++)
        {
            SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_trunk_resolve_set(unit, tm_domain, entry_index, base_queue));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_trunk_resolve_set(unit, tm_domain, entry, base_queue));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API dnx_stk_trunk_stack_lag_packets_base_queue_id_get().
 *          For details about API parameters refer to dnx_stk_trunk_stack_lag_packets_base_queue_id_get() description.
 */
int
dnx_stk_trunk_stack_lag_packets_base_queue_id_get_verify(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 *base_queue)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(base_queue, _SHR_E_PARAM, "base_queue");

    /*
     * stack_lag_domain_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if ((tm_domain < dnx_data_stack.general.lag_domain_min_get(unit))
        || (tm_domain > dnx_data_stack.general.lag_domain_max_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "tm_domain is invalid");
    }

    /*
     * stack_lag_stack_trunk_resolve_entry_min may be changed and be grater than 0
     */
     /* coverity[unsigned_compare : FALSE]  */
    if (((entry < dnx_data_stack.general.lag_stack_trunk_resolve_entry_min_get(unit))
         || (entry > dnx_data_stack.general.lag_stack_trunk_resolve_entry_max_get(unit)))
        && (entry != dnx_data_stack.general.lag_stack_trunk_resolve_entry_all_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "entry is invalid");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_stack_lag_packets_base_queue_id_get(
    int unit,
    uint32 tm_domain,
    uint32 entry,
    uint32 *base_queue)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_stack_lag_packets_base_queue_id_get_verify(unit, tm_domain, entry, base_queue));

    SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_trunk_resolve_get(unit, tm_domain, entry, base_queue));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_stack_trunk_resolve_set(
    int unit,
    uint32 stack_lag,
    uint32 lag_lb_key,
    uint32 base_queue)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_TRUNK_RESOLVE_TABLE, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STACK_LAG, stack_lag);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY, lag_lb_key);

    /*
     * Set value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_QUEUE, INST_SINGLE, base_queue);

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
dnx_stk_trunk_stack_trunk_resolve_get(
    int unit,
    uint32 lag_lb_key,
    uint32 stack_lag,
    uint32 *base_queue)
{
    uint32 entry_handle_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get from dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_STACK_TRUNK_RESOLVE_TABLE, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAG_LB_KEY, lag_lb_key);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STACK_LAG, stack_lag);

    /** Get value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BASE_QUEUE, INST_SINGLE, base_queue);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_stk_trunk_stacking_id_verify(
    int unit,
    bcm_trunk_t trunk_id)
{
    bcm_trunk_t peer_tm_domain = -1;
    SHR_FUNC_INIT_VARS(unit);

    peer_tm_domain = BCM_TRUNK_STACKING_TID_GET(trunk_id);
    if (DNX_TRUNK_STACKING_PEER_TMD_IS_VALID(unit, peer_tm_domain))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "Stacking trunk peer tm doamin is invalid");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for dnx_stk_trunk_stacking_set. the point in
 *        this verify function is to check in advance that the
 *        changes can be made to the trunk and fail if not
 *        before the changes began. this is cruicial in the
 *        trunk because it is a system feature and not local to
 *        a single fap
 *
 * \param [in] unit - uint number
 * \param [in] trunk_id - trunk id
 * \param [in] member_count - nof members in array
 * \param [in] member_array - array of members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_stk_trunk_stacking_set_verify(
    int unit,
    bcm_trunk_t trunk_id,
    int member_count,
    bcm_trunk_member_t * member_array)
{
    int index = 0x0;
    SHR_FUNC_INIT_VARS(unit);

    /** Check stacking trunk id is valid */
    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_id_verify(unit, trunk_id));

    if (member_count <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid member_count recieved 0x%x\n", member_count);
    }
    SHR_NULL_CHECK(member_array, _SHR_E_PARAM, "member_array");

    for (index = 0; index < member_count; index++)
    {
        /** check validity of gport type */
        if (!BCM_GPORT_IS_SYSTEM_PORT(member_array[index].gport))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport recieved 0x%x\n", member_array[index].gport);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_stk_trunk_stacking_set(
    int unit,
    bcm_trunk_t trunk_id,
    int member_count,
    bcm_trunk_member_t * member_array)
{
    int entry = 0x0;
    int index = 0x0;
    uint32 dest_base_queue;
    bcm_trunk_t stk_tid = 0;
    bcm_gport_t system_port_gport, dest_queue_gport;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_stacking_set_verify(unit, trunk_id, member_count, member_array));

    stk_tid = BCM_TRUNK_STACKING_TID_GET(trunk_id);

    for (entry = 0; entry < dnx_data_stack.general.lag_stack_trunk_resolve_entry_max_get(unit); entry++)
    {
        system_port_gport = member_array[index].gport;

        /*
         * get stk port voq
         */
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_sysport_ingress_queue_map_get(unit, 0, system_port_gport, &dest_queue_gport));
        dest_base_queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(dest_queue_gport);

        /*
         * Write entries to Stack trunk resolve table
         */
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_lag_packets_base_queue_id_set(unit, stk_tid, entry, dest_base_queue));

        index = (index + 1) % member_count;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for dnx_trunk_stacking_get
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member_max - member array size
 * \param [in] member_array - member array
 * \param [in] member_count - nof retrieved members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_stk_trunk_stacking_get_verify(
    int unit,
    bcm_trunk_t trunk_id,
    int member_max,
    bcm_trunk_member_t * member_array,
    int *member_count)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Check trunk id is valid */
    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_id_verify(unit, trunk_id));

    if (member_max <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid member_max recieved 0x%x\n", member_max);
    }
    SHR_NULL_CHECK(member_array, _SHR_E_PARAM, "member_array");
    SHR_NULL_CHECK(member_count, _SHR_E_PARAM, "member_count");

exit:
    SHR_FUNC_EXIT;
}

 /*
  * See .h file
  */
shr_error_e
dnx_stk_trunk_stacking_get(
    int unit,
    bcm_trunk_t trunk_id,
    int member_max,
    bcm_trunk_member_t * member_array,
    int *member_count)
{
    int index = 0;
    uint32 dest_base_queue = 0x0;
    uint32 first_dest_base_queue = 0x0;
    bcm_trunk_t stk_tid = 0x0;
    uint32 sys_port_id = 0;
    bcm_gport_t sys_gport = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_stacking_get_verify(unit, trunk_id, member_max, member_array, member_count));

    stk_tid = BCM_TRUNK_STACKING_TID_GET(trunk_id);

    for (index = 0; index < dnx_data_stack.general.lag_stack_trunk_resolve_entry_max_get(unit); index++)
    {
        if (index >= member_max)
        {
            break;
        }

        /*
         * Read entries from Stack trunk resolve table
         */
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_lag_packets_base_queue_id_get(unit, stk_tid, index, &dest_base_queue));

        /*
         * Exit loop if stacking lag is empty
         */
        if (dest_base_queue == DEST_PORT_INVALID_BASE_QUEUE(unit))
        {
            break;
        }

        /*
         * Base queues were inserted according to members one by one in a cycle. * All uniqueue members were went over
         * when it arrived back to the first one
         */
        if (index == 0)
        {
            first_dest_base_queue = dest_base_queue;
        }
        else
        {
            /*
             * Arrived back to the first one in cycle
             */
            if (dest_base_queue == first_dest_base_queue)
            {
                break;
            }
        }

        /*
         * Get System Port ID from dest base queue
         */
        SHR_IF_ERR_EXIT(dnx_ipq_queue_sysport_map_get(unit, BCM_CORE_ALL, dest_base_queue, &sys_port_id));

        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, sys_port_id);

        member_array[index].gport = sys_gport;
    }

    *member_count = index;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_member_add
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - info of added member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_stk_trunk_stacking_member_add_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    algo_gpm_gport_verify_type_e allowed_port_infos[] = { ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_id_verify(unit, trunk_id));

    /** NULL checks */
    SHR_NULL_CHECK(member, _SHR_E_PARAM, "member");

    /** check validity of system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, member->gport, COUNTOF(allowed_port_infos), allowed_port_infos));

exit:
    SHR_FUNC_EXIT;
}

 /*
  * See .h file
  */
shr_error_e
dnx_stk_trunk_stacking_member_add(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    int member_count = 0;
    bcm_trunk_member_t *member_array_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_stacking_member_add_verify(unit, trunk_id, member));

    SHR_ALLOC_SET_ZERO(member_array_p, sizeof(*member_array_p) * MAX_NOF_MEMBERS(unit),
                       "member_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_get(unit, trunk_id, MAX_NOF_MEMBERS(unit), member_array_p, &member_count));

    /*
     * Copy the new member to the array and increase member counter accordingly
     */
    sal_memcpy(&member_array_p[member_count++], member, sizeof(bcm_trunk_member_t));

    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_set(unit, trunk_id, member_count, member_array_p));

exit:
    SHR_FREE(member_array_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_member_delete
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - member info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_stk_trunk_stacking_member_delete_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    int i = 0;
    int member_count = 0;
    int member_del_idx = MAX_NOF_MEMBERS(unit); /* Invalid array index */
    bcm_gport_t gport_modport = 0x0;
    bcm_trunk_member_t *member_array_p = NULL;
    algo_gpm_gport_verify_type_e allowed_port_infos[] = { ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_id_verify(unit, trunk_id));

    /** NULL checks */
    SHR_NULL_CHECK(member, _SHR_E_PARAM, "member");

    /** check validity of system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, member->gport, COUNTOF(allowed_port_infos), allowed_port_infos));

    SHR_ALLOC_SET_ZERO(member_array_p, sizeof(*member_array_p) * MAX_NOF_MEMBERS(unit),
                       "member_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_get(unit, trunk_id, MAX_NOF_MEMBERS(unit), member_array_p, &member_count));
    if (member_count == 0x0)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "%s: Stacking Trunk already empty (member_count=%d)",
                     FUNCTION_NAME(), member_count);
    }

    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, member->gport, &gport_modport));

    for (i = 0; i < member_count; i++)
    {
        if (member_array_p[i].gport == gport_modport)
        {
            member_del_idx = i;
            break;
        }
    }

    if (member_del_idx == MAX_NOF_MEMBERS(unit))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "%s: Input gport member (0x%x), are not in Stacking trunk (%d)",
                     FUNCTION_NAME(), member->gport, trunk_id);
    }

exit:
    SHR_FREE(member_array_p);
    SHR_FUNC_EXIT;
}

 /*
  * See .h file
  */
shr_error_e
dnx_stk_trunk_stacking_member_delete(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    int member_count = 0;
    int i = 0;
    int member_del_idx = MAX_NOF_MEMBERS(unit); /* Invalid array index */
    bcm_gport_t gport_modport = 0x0;
    bcm_trunk_t stk_tid = -1;
    bcm_trunk_member_t *member_array_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_stacking_member_delete_verify(unit, trunk_id, member));

    stk_tid = BCM_TRUNK_STACKING_TID_GET(trunk_id);

    SHR_ALLOC_SET_ZERO(member_array_p, sizeof(*member_array_p) * MAX_NOF_MEMBERS(unit),
                       "member_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_get(unit, trunk_id, MAX_NOF_MEMBERS(unit), member_array_p, &member_count));

    SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, member->gport, &gport_modport));

    for (i = 0; i < member_count; i++)
    {
        if (member_array_p[i].gport == gport_modport)
        {
            member_del_idx = i;
            break;
        }
    }

    if (member_count == 1)
    {
        /*
         * Remove the last member from stacking trunk and restore it as invalid entries
         */
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stack_lag_packets_base_queue_id_set
                        (unit, stk_tid,
                         dnx_data_stack.general.lag_stack_trunk_resolve_entry_all_get(unit),
                         DEST_PORT_INVALID_BASE_QUEUE(unit)));
    }
    else
    {
        sal_memmove(&member_array_p[member_del_idx], &member_array_p[member_count - 1], sizeof(bcm_trunk_member_t));
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_set(unit, trunk_id, member_count - 1, member_array_p));
    }

exit:
    SHR_FREE(member_array_p);
    SHR_FUNC_EXIT;
}
