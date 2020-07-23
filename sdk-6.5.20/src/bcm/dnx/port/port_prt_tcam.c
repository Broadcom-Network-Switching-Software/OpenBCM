/** \file port_prt_tcam.c
 * 
 *
 * Port PRT tcam procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/**
 * \brief
 *   Set sw table control information for virtual port TCAM management.
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - Core ID
 * \param [in] entry_access_id - Entry access id that will be used in PRT_VIRTUAL_PORT_TCAM
 * \param [in] is_used  - If '1' indicates that the entry_access_id is used
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_sw_control_set(
    int unit,
    bcm_core_t core_id,
    uint16 entry_access_id,
    uint16 is_used)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_TCAM_SW_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, entry_access_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IS_USED, INST_SINGLE, is_used);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get sw table control information for virtual port TCAM management.
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - Core ID
 * \param [in] entry_access_id - Entry access id used for configuring PRT_VIRTUAL_PORT_TCAM
 * \param [out] is_used  - Indicate if the entry_access_id is used
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_sw_control_get(
    int unit,
    bcm_core_t core_id,
    uint16 entry_access_id,
    uint8 *is_used)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_TCAM_SW_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, entry_access_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (is_used)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_IS_USED, INST_SINGLE, is_used));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_sw_get(
    int unit,
    uint32 core_id,
    dbal_fields_e prt_key_field_id,
    uint32 key_value,
    uint32 *access_id,
    uint8 *is_used,
    uint16 *priority,
    uint32 *pp_port,
    uint16 *sys_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_TCAM_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, prt_key_field_id, key_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_USE, INST_SINGLE, is_used);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE, sys_port);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INDEX, INST_SINGLE, access_id);
    SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT), _SHR_E_NOT_FOUND);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_sw_set(
    int unit,
    uint32 core_id,
    dbal_fields_e prt_key_field_id,
    uint32 key_value,
    uint32 access_id,
    uint16 priority,
    uint16 pp_port,
    uint16 sys_port,
    uint16 is_entry_clear)
{
    uint32 entry_handle_id;
    uint16 is_used = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_TCAM_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, prt_key_field_id, key_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (is_entry_clear)
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        is_used = 1;
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_IS_USE, INST_SINGLE, is_used);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE, sys_port);
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_KEY_TYPE, INST_SINGLE, prt_key_field_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, INST_SINGLE, access_id);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }

    /**update SW control info*/
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_control_set(unit, core_id, access_id, is_used));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_hw_set(
    int unit,
    bcm_port_t core_id,
    dbal_fields_e prt_key_field_id,
    uint32 key_value,
    uint32 *key_mask,
    uint32 access_id,
    uint16 pp_port,
    uint16 sys_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    if (key_mask != NULL)
    {
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, prt_key_field_id, key_value, *key_mask);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, prt_key_field_id, key_value);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);
    dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_SOURCE_SYSTEM_PORT, INST_SINGLE, sys_port);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_hw_clear(
    int unit,
    bcm_core_t core_id,
    uint32 access_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_virtual_port_access_id_get(
    int unit,
    bcm_core_t core_id,
    dbal_fields_e prttcam_key_field_id,
    uint32 prttcam_key_value,
    uint32 *entry_access_id)
{
    uint32 idx = 0;
    int max_capacity = 0;
    uint8 is_used = 0;
    uint32 pp_port = 0;
    uint16 system_port = 0, priority = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_get
                    (unit, core_id, prttcam_key_field_id, prttcam_key_value, &idx, &is_used, &priority, &pp_port,
                     &system_port));
    if (is_used)
    {
        *entry_access_id = idx;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &max_capacity));
        for (idx = 0; idx < max_capacity; idx++)
        {
            SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_control_get(unit, core_id, idx, &is_used));
            if (!is_used)
            {
                *entry_access_id = idx;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_sw_access_id_alloc(
    int unit,
    bcm_port_t core_id,
    dbal_fields_e prttcam_key_field_id,
    uint32 prttcam_key_value,
    uint32 *entry_access_id)
{
    int max_capacity = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_access_id, _SHR_E_PARAM, "entry_access_id");

    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM, &max_capacity));

    /** Get free access id */
    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_access_id_get
                    (unit, core_id, prttcam_key_field_id, prttcam_key_value, entry_access_id));
    if (*entry_access_id < max_capacity)
    {
      /** Allocate a virtual port TCAM access id */
        SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_sw_set
                        (unit, core_id, prttcam_key_field_id, prttcam_key_value, *entry_access_id, 0, 0, 0, 0));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Can't find free access_id for PRT_VIRTUAL_PORT_TCAM!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_set(
    int unit,
    uint16 local_port,
    uint32 core_id,
    uint32 pp_port,
    uint32 tcam_access_id,
    uint32 key_value,
    uint16 is_entry_clear)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_COE_LAG_CONTROL_SW, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_INDEX, local_port);
    if (is_entry_clear)
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_INDEX, INST_SINGLE, core_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACCESS_ID, INST_SINGLE, tcam_access_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRTTCAM_KEY_VP_COE, INST_SINGLE, key_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See port_prt_tcam.h
 */
shr_error_e
dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_get(
    int unit,
    uint16 local_port,
    uint32 *core_id,
    uint32 *pp_port,
    uint32 *tcam_access_id,
    uint32 *key_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(core_id, _SHR_E_PARAM, "core_id");
    SHR_NULL_CHECK(pp_port, _SHR_E_PARAM, "pp_port");
    SHR_NULL_CHECK(tcam_access_id, _SHR_E_PARAM, "tcam_access_id");
    SHR_NULL_CHECK(key_value, _SHR_E_PARAM, "key_value");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VIRTUAL_PORT_COE_LAG_CONTROL_SW, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_INDEX, local_port);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_CORE_INDEX, INST_SINGLE, core_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PP_PORT, INST_SINGLE, pp_port));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ACCESS_ID, INST_SINGLE, tcam_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_PRTTCAM_KEY_VP_COE, INST_SINGLE, key_value));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
