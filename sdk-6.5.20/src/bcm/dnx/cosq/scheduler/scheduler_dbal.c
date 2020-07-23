/** \file scheduler_dbal.c
 * 
 *
 * e2e scheduler functionality for DNX
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>

/** this chunk of code is required due to SCHEDULER_INIT access issue */
/*
 * {
 */

#include <soc/register.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/dnxc/drv_dnxc_utils.h>
/*
 * }
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include "scheduler_dbal.h"
#include "scheduler.h"

/*
 * ------------------------------------------------------------------------------------------
 * --                 Scheduler Flow
 * ------------------------------------------------------------------------------------------
 */

/*
 * \brief - Reset flow attributes to the default (aka reset) value
 */
shr_error_e
dnx_sch_flow_attributes_reset(
    int unit,
    int core_id,
    int flow_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure is remote flow property
 * \note - flows for all remote cores is updated by this function
 */
shr_error_e
dnx_sch_flow_is_remote_hw_set(
    int unit,
    int core_id,
    int flow_id,
    int is_remote)
{
    uint32 entry_handle_id;
    int actual_flow_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int nof_remote_cores, i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core_id, flow_id, actual_flow_id,
                                                              &nof_remote_cores));

    for (i = 0; i < nof_remote_cores; i++)
    {
        /** Setting key fields */
        /*
         * flow_id will be validated in dbal 
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, actual_flow_id[i]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FABRIC, INST_SINGLE, is_remote);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get is remote flow property
 */
shr_error_e
dnx_sch_flow_is_remote_hw_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_remote)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_FABRIC, INST_SINGLE, (uint32 *) is_remote);

    /** Preforming the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure is high priority flow property
 */
shr_error_e
dnx_sch_flow_is_high_priority_hw_set(
    int unit,
    int core_id,
    int flow_id,
    int is_high_priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HIGH_PRIO, INST_SINGLE, is_high_priority);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get is high priority flow property
 */
shr_error_e
dnx_sch_flow_is_high_priority_hw_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_high_priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_HIGH_PRIO, INST_SINGLE,
                               (uint32 *) is_high_priority);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure is slow enable flow property
 */
shr_error_e
dnx_sch_flow_is_slow_enable_set(
    int unit,
    int core_id,
    int flow_id,
    int is_slow_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_SLOW_ENABLE, INST_SINGLE, is_slow_enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get is slow enable flow property
 */
shr_error_e
dnx_sch_flow_is_slow_enable_get(
    int unit,
    int core_id,
    int flow_id,
    int *is_slow_enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_SLOW_ENABLE, INST_SINGLE,
                               (uint32 *) is_slow_enable);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure flow credit source se id
 */
shr_error_e
dnx_sch_flow_credit_src_se_id_set(
    int unit,
    int core_id,
    int flow_id,
    int credit_src_se_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_SRC_SE_ID, INST_SINGLE, credit_src_se_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure flow credit source cos properties
 */
shr_error_e
dnx_sch_flow_credit_src_cos_set(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_flow_credit_source_cos_data_t * credit_source_cos)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Regular Credit Source table - all but COS
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_SRC_HR_SEL_DUAL, INST_SINGLE,
                                 credit_source_cos->hr_sel_dual);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     *  Credit Source COS table
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, credit_source_cos->type);

    switch (credit_source_cos->type)
    {
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_SIMPLE:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COS_CL, INST_SINGLE,
                                         credit_source_cos->simple_cos);
            break;
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_SIMPLE:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COS_HR, INST_SINGLE,
                                         credit_source_cos->simple_cos);
            break;
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT_MANT, INST_SINGLE,
                                         credit_source_cos->hr_weight_mantissa);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT_EXP, INST_SINGLE,
                                         credit_source_cos->hr_weight_exp);
            break;
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_WFQ_WEIGHT:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE,
                                         credit_source_cos->cl_weight);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected cos type %d\n", credit_source_cos->type);
            break;
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure flow credit source parameters
 */
shr_error_e
dnx_sch_flow_credit_src_hw_set(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_flow_credit_source_t * credit_source)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_se_id_set(unit, core_id, flow_id, credit_source->se_id));
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_cos_set(unit, core_id, flow_id, &credit_source->cos));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get flow credit source SE ID 
 */
shr_error_e
dnx_sch_flow_credit_source_se_id_get(
    int unit,
    int core_id,
    int flow_id,
    int *credit_source_se_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_SRC_SE_ID, INST_SINGLE,
                               (uint32 *) credit_source_se_id);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get flow credit source cos properties
 */
shr_error_e
dnx_sch_flow_credit_source_cos_get(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_flow_credit_source_cos_data_t * credit_source_cos)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Clear the result */
    sal_memset(credit_source_cos, 0x0, sizeof(dnx_sch_flow_credit_source_cos_data_t));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_ATTR, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_SRC_HR_SEL_DUAL, INST_SINGLE,
                               (uint32 *) &credit_source_cos->hr_sel_dual);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SCH_FLOW_CREDIT_SRC_COS, entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &credit_source_cos->type));

    switch (credit_source_cos->type)
    {
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_SIMPLE:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_COS_CL,
                                                                INST_SINGLE, &credit_source_cos->simple_cos));
            break;
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_SIMPLE:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_COS_HR,
                                                                INST_SINGLE, &credit_source_cos->simple_cos));
            break;
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_HR_WFQ_WEIGHT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_WEIGHT_MANT,
                                                                INST_SINGLE, &credit_source_cos->hr_weight_mantissa));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_WEIGHT_EXP,
                                                                INST_SINGLE, &credit_source_cos->hr_weight_exp));

            break;
        case DBAL_RESULT_TYPE_SCH_FLOW_CREDIT_SRC_COS_CL_WFQ_WEIGHT:
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_WEIGHT,
                                                                INST_SINGLE, &credit_source_cos->cl_weight));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected cos type %d\n", credit_source_cos->type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get flow credit source parameters
 */
shr_error_e
dnx_sch_flow_credit_src_hw_get(
    int unit,
    int core_id,
    int flow_id,
    dnx_sch_flow_credit_source_t * credit_source)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_se_id_get(unit, core_id, flow_id, &credit_source->se_id));
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_cos_get(unit, core_id, flow_id, &credit_source->cos));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure is composite property of flow
 */
shr_error_e
dnx_sch_flow_is_composite_hw_set(
    int unit,
    int core_id,
    int flow_id_pair,
    int is_composite)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCHEDULER_COMPOSITE_ENABLE_TABLE, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_ID_PAIR, flow_id_pair);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_COMPOSITE, INST_SINGLE, is_composite);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure is composite property in FSF table for range of flows
 */
shr_error_e
dnx_sch_flow_is_composite_hw_range_set_fsf(
    int unit,
    int core_id,
    int flow_id_pair,
    int nof_pairs,
    int is_composite)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCHEDULER_COMPOSITE_ENABLE_TABLE, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FLOW_ID_PAIR,
                                     flow_id_pair, flow_id_pair + nof_pairs - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_COMPOSITE, INST_SINGLE, is_composite);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get is composite property of flow
 */
shr_error_e
dnx_sch_flow_is_composite_hw_get(
    int unit,
    int core_id,
    int flow_id_pair,
    int *is_composite)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCHEDULER_COMPOSITE_ENABLE_TABLE, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_ID_PAIR, flow_id_pair);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_COMPOSITE, INST_SINGLE, (uint32 *) is_composite);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure is composite property of flow
 */
shr_error_e
dnx_sch_connector_is_composite_mapping_hw_set(
    int unit,
    int core_id,
    int flow_quartet,
    int is_composite)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_TO_QUEUE_MAPPING, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_INDEX, flow_quartet);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_COMPOSITE, INST_SINGLE, is_composite);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure base remote queue for this flow
 */
shr_error_e
dnx_sch_flow_queue_mapping_range_set(
    int unit,
    int core_id,
    int flow_quartet_id,
    int nof_quartets,
    uint32 queue_quad)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_TO_QUEUE_MAPPING, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_INDEX,
                                     flow_quartet_id, flow_quartet_id + nof_quartets - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUAD, INST_SINGLE, queue_quad);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get base remote queue for this flow
 */
shr_error_e
dnx_sch_flow_queue_mapping_get(
    int unit,
    int core_id,
    int flow_quartet_id,
    uint32 *queue_quad)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_TO_QUEUE_MAPPING, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_INDEX, flow_quartet_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUEUE_QUAD, INST_SINGLE, queue_quad);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure remote FAP id for this flow
 */
shr_error_e
dnx_sch_flow_fap_mapping_range_set(
    int unit,
    int core_id,
    int octet_id,
    int nof_octets,
    uint32 remote_fap_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_TO_FAP_MAPPING, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OCTET_INDEX,
                                     octet_id, octet_id + nof_octets - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, remote_fap_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get  remote FAP id for this flow
 */
shr_error_e
dnx_sch_flow_fap_mapping_get(
    int unit,
    int core_id,
    int octet_id,
    uint32 *remote_fap_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_TO_FAP_MAPPING, &entry_handle_id));

    /** Setting key fields */
    /*
     * flow_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OCTET_INDEX, octet_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FAP_ID, INST_SINGLE, remote_fap_id);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Update dynamic number of tokens for the flow
 * Called after updating max_burst,
 */
shr_error_e
dnx_sch_flow_shaper_token_count_set(
    int unit,
    int core_id,
    int flow_id[],
    int nof_remote_cores,
    int token_count)
{
    uint32 token_count_entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Update token count
     */

    /** we don't enable sch dynamic memory access, as it is kept enabled after init */

    /*
     * -- 3. Update token count 
     */

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_SHAPER_DYNAMIC_TABLE, &token_count_entry_handle_id));

    for (i = 0; i < nof_remote_cores; i++)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, token_count_entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id[i]);
        dbal_entry_key_field32_set(unit, token_count_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

        /** Setting value fields */
        if (token_count == -1)
        {
            dbal_entry_value_field_predefine_value_set(unit, token_count_entry_handle_id, DBAL_FIELD_TOKEN_COUNT,
                                                       INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, token_count_entry_handle_id, DBAL_FIELD_TOKEN_COUNT, INST_SINGLE,
                                         token_count);
        }
        dbal_entry_value_field_predefine_value_set(unit, token_count_entry_handle_id, DBAL_FIELD_SLOW_TOKEN_COUNT,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, token_count_entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Reset attributes of Shaper Descriptor table 
 * to their default values (aka reset values)
 */
shr_error_e
dnx_sch_flow_shaper_attributes_reset(
    int unit,
    int core_id,
    int flow_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Clearing the entry updates max_burst to default, 
     * so update token count to default as well
     */
    SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_token_count_set(unit, core_id, &flow_id, 1, -1));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure shaper max burst 
 * for *all remote core* flows defined by flow_id
 */
shr_error_e
dnx_sch_flow_shaper_max_burst_set(
    int unit,
    int core_id,
    int flow_id,
    uint32 max_burst)
{
    uint32 entry_handle_id;
    int actual_flow_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    int nof_remote_cores, i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core_id, flow_id, actual_flow_id,
                                                              &nof_remote_cores));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, &entry_handle_id));

    for (i = 0; i < nof_remote_cores; i++)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, actual_flow_id[i]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

        /** Update shaper max burst */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Updating max_burst requires updating token count */
    SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_token_count_set(unit, core_id, actual_flow_id, nof_remote_cores, max_burst));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get shaper max burst
 */
shr_error_e
dnx_sch_flow_shaper_max_burst_get(
    int unit,
    int core_id,
    int flow_id,
    uint32 *max_burst)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure shaper bandwidth
 */
shr_error_e
dnx_sch_flow_shaper_bandwidth_set(
    int unit,
    int core_id,
    int flow_id,
    uint32 peak_rate_man,
    uint32 peak_rate_exp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Update shaper rate */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_MAN, INST_SINGLE, peak_rate_man);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_EXP, INST_SINGLE, peak_rate_exp);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get shaper bandwidth
 */
shr_error_e
dnx_sch_flow_shaper_bandwidth_get(
    int unit,
    int core_id,
    int flow_id,
    uint32 *peak_rate_man,
    uint32 *peak_rate_exp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE_MAN, INST_SINGLE, peak_rate_man);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE_EXP, INST_SINGLE, peak_rate_exp);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure slow rate profile
 */
shr_error_e
dnx_sch_flow_shaper_slow_profile_set(
    int unit,
    int core_id,
    int flow_id,
    int slow_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Update slow rate profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_RATE_2, INST_SINGLE, slow_profile);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get slow rate profile
 */
shr_error_e
dnx_sch_flow_shaper_slow_profile_get(
    int unit,
    int core_id,
    int flow_id,
    int *slow_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SLOW_RATE_2, INST_SINGLE, (uint32 *) slow_profile);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure slow rate
 */
shr_error_e
dnx_sch_slow_profile_rate_hw_set(
    int unit,
    int core_id,
    int slow_profile,
    int slow_level,
    uint32 slow_rate_man,
    uint32 slow_rate_exp,
    uint32 max_bucket)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SLOW_RATE_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_PROFILE, slow_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_LEVEL, slow_level);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Update shaper rate */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_MAN, INST_SINGLE, slow_rate_man);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_EXP, INST_SINGLE, slow_rate_exp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BUCKET, INST_SINGLE, max_bucket);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Get slow rate of slow profile
 */
shr_error_e
dnx_sch_slow_profile_rate_hw_get(
    int unit,
    int core_id,
    int slow_profile,
    int slow_level,
    uint32 *slow_rate_man,
    uint32 *slow_rate_exp,
    uint32 *max_bucket)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SLOW_RATE_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_PROFILE, slow_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOW_LEVEL, slow_level);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE_MAN, INST_SINGLE, slow_rate_man);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE_EXP, INST_SINGLE, slow_rate_exp);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_BUCKET, INST_SINGLE, max_bucket);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * ------------------------------------------------------------------------------------------
 * --                 Scheduler Element
 * ------------------------------------------------------------------------------------------
 */

/**
 * \brief - 
 * is Scheduler Element behaves as a scheduler
 */
shr_error_e
dnx_sch_se_is_scheduler_get(
    int unit,
    int core_id,
    int se_id,
    int *is_scheduler)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCHEDULER_ENABLE_TABLE, &entry_handle_id));

    /** Setting key fields */
    /*
     * se_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SE_OCTET_ID,
                               se_id / dnx_data_sch.dbal.scheduler_enable_grouping_factor_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_SCHEDULER, INST_SINGLE, (uint32 *) is_scheduler);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * configure all Scheduler Elements in the range to be / not to be scheduler
 */
shr_error_e
dnx_sch_se_is_scheduler_set_range(
    int unit,
    int core_id,
    int se_id_start,
    int se_id_end,
    int is_scheduler)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCHEDULER_ENABLE_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Set Range Key Fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_SE_OCTET_ID,
                                     se_id_start / dnx_data_sch.dbal.scheduler_enable_grouping_factor_get(unit),
                                     se_id_end / dnx_data_sch.dbal.scheduler_enable_grouping_factor_get(unit));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_SCHEDULER, INST_SINGLE, is_scheduler);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_OVERRUN));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Configure Color group of the scheduling element
 */
shr_error_e
dnx_sch_se_color_group_set(
    int unit,
    int core_id,
    int se_id,
    int group)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_COLOR_GROUP, &entry_handle_id));

    /** Setting key fields */
    /*
     * se_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SE_ID, se_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Update color group */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP, INST_SINGLE, group);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Get Color group of the scheduling element
 */
shr_error_e
dnx_sch_se_color_group_get(
    int unit,
    int core_id,
    int se_id,
    int *group)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_COLOR_GROUP, &entry_handle_id));

    /** Setting key fields */
    /*
     * se_id will be validated in dbal 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SE_ID, se_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GROUP, INST_SINGLE, (uint32 *) group);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Configure cl class mapping
 */
shr_error_e
dnx_sch_cl_mapping_set(
    int unit,
    int core_id,
    int cl_id,
    int cl_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_CL_MAPPING, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_ID, cl_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Update result fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_PROFILE, INST_SINGLE, cl_profile);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Obtain cl class mapping
 */
shr_error_e
dnx_sch_cl_mapping_get(
    int unit,
    int core_id,
    int cl_id,
    int *cl_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_CL_MAPPING, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_ID, cl_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CL_PROFILE, INST_SINGLE, (uint32 *) cl_profile);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Configure cl parameters
 */
shr_error_e
dnx_sch_cl_config_set(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_config_t * cl_config)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_CL_CONFIG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_PROFILE, cl_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Update result fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_MODE, INST_SINGLE, cl_config->cl_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WFQ_WEIGHT_MODE, INST_SINGLE,
                                 cl_config->wfq_weight_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_ENHANCED_MODE, INST_SINGLE,
                                 cl_config->enhanced_mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Obtain cl parameters
 */
shr_error_e
dnx_sch_cl_config_get(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_config_t * cl_config)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_CL_CONFIG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_PROFILE, cl_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CL_MODE, INST_SINGLE, (uint32 *) &cl_config->cl_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WFQ_WEIGHT_MODE, INST_SINGLE,
                               (uint32 *) &cl_config->wfq_weight_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CL_ENHANCED_MODE, INST_SINGLE,
                               (uint32 *) &cl_config->enhanced_mode);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Configure cl descrete weights
 */
shr_error_e
dnx_sch_cl_weight_set(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_weight_t * cl_weight)
{
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_CL_CONFIG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_PROFILE, cl_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Update result fields */
    for (i = 0; i < DNX_SCH_CL_WEIGHTS_NOF; i++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, i, cl_weight->weight[i]);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Obtain cl descrete weights
 */
shr_error_e
dnx_sch_cl_weight_get(
    int unit,
    int core_id,
    int cl_profile,
    dnx_sch_cl_weight_t * cl_weight)
{
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SE_CL_CONFIG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_PROFILE, cl_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    for (i = 0; i < DNX_SCH_CL_WEIGHTS_NOF; i++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, i,
                                   (uint32 *) &cl_weight->weight[i]);
    }

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure is HR used as port HR
 */
shr_error_e
dnx_sch_port_hr_enable_range_set(
    int unit,
    int core_id,
    int base_hr_id,
    int nof_hr,
    int is_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_HR_IS_PORT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, base_hr_id, base_hr_id + nof_hr - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PORT_HR, INST_SINGLE, is_port);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get is HR used as port HR
 */
shr_error_e
dnx_sch_hr_is_port_get(
    int unit,
    int core_id,
    int hr_id,
    int *is_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_HR_IS_PORT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_PORT_HR, INST_SINGLE, (uint32 *) is_port);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure force FC for given port
 */
shr_error_e
dnx_sch_port_hr_force_fc_range_set(
    int unit,
    int core_id,
    int base_hr_id,
    int nof_hr,
    int force_fc)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_HR_FORCE_FC, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, base_hr_id, base_hr_id + nof_hr - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_FC, INST_SINGLE, force_fc);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure HR mode
 */
shr_error_e
dnx_sch_hr_mode_set(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_hr_mode_e hr_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_HR_CONFIG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_MODE, INST_SINGLE, hr_mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get HR mode
 */
shr_error_e
dnx_sch_hr_mode_get(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_hr_mode_e * hr_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_HR_CONFIG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HR_MODE, INST_SINGLE, (uint32 *) hr_mode);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure number of port priorities
 */
shr_error_e
dnx_sch_port_priorities_nof_set(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_nof_ps_priorities_e ps_priorities)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PS_ID,
                               hr_id / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_PS_PRIORITIES, INST_SINGLE, ps_priorities);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get number of port priorities
 */
shr_error_e
dnx_sch_port_priorities_nof_get(
    int unit,
    int core_id,
    int hr_id,
    dbal_enum_value_field_nof_ps_priorities_e * ps_priorities)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PS_ID,
                               hr_id / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_PS_PRIORITIES, INST_SINGLE,
                               (uint32 *) ps_priorities);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear number of port priorities
 */
shr_error_e
dnx_sch_port_priorities_nof_clear(
    int unit,
    int core_id,
    int hr_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PS_ID,
                               hr_id / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure port priority propagation enable
 */
shr_error_e
dnx_sch_dbal_port_priority_propagation_enable_set(
    int unit,
    int core_id,
    int hr_id,
    int port_priority_propagation_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PS_ID,
                               hr_id / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_PROPAGATION_ENABLE, INST_SINGLE,
                                 port_priority_propagation_enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port priority propagation enable
 */
shr_error_e
dnx_sch_dbal_port_priority_propagation_enable_get(
    int unit,
    int core_id,
    int hr_id,
    int *port_priority_propagation_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PS_ID,
                               hr_id / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_PROPAGATION_ENABLE, INST_SINGLE,
                               (uint32 *) port_priority_propagation_enable);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable scheduler interface by disabling pause
 */
shr_error_e
dnx_sch_interface_enable_set(
    int unit,
    int core_id,
    int interface_id,
    int enable)
{
    uint32 entry_handle_id;
    int force_pause;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_INTERFACE_FORCE_PAUSE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_INTERFACE_ID, interface_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    force_pause = (enable ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE, INST_SINGLE, force_pause);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure number of priorities for port priority propagation
 */
shr_error_e
dnx_sch_dbal_priority_propagation_priorities_nof_set(
    int unit,
    int core_id,
    int sch_if,
    dbal_enum_value_field_nof_ps_priorities_e priority_propagation_priorities)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_INTERFACE_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IF_GROUP,
                               sch_if / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_PRIORITY_PROPAGATION_PRIORITIES, INST_SINGLE,
                                 priority_propagation_priorities);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get number of priorities for port priority propagation
 */
shr_error_e
dnx_sch_dbal_priority_propagation_priorities_nof_get(
    int unit,
    int core_id,
    int sch_if,
    dbal_enum_value_field_nof_ps_priorities_e * priority_propagation_priorities)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_INTERFACE_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IF_GROUP,
                               sch_if / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOF_PRIORITY_PROPAGATION_PRIORITIES, INST_SINGLE,
                               (uint32 *) priority_propagation_priorities);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear number of priorities for port priority propagation
 */
shr_error_e
dnx_sch_dbal_priority_propagation_priorities_nof_clear(
    int unit,
    int core_id,
    int sch_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_INTERFACE_PRIORITY, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IF_GROUP,
                               sch_if / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map EGQ interface to scheduler interface
 */
shr_error_e
dnx_sch_egq_if_to_sch_if_map_set(
    int unit,
    int core_id,
    int egq_if,
    int sch_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_EGQ_TO_SCH_IF_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_INTERFACE_ID, egq_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_INTERFACE_ID, INST_SINGLE, sch_if);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - UnMap EGQ interface to scheduler interface
 */
shr_error_e
dnx_sch_egq_if_to_sch_if_map_clear(
    int unit,
    int core_id,
    int egq_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_EGQ_TO_SCH_IF_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_INTERFACE_ID, egq_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SCH interface mapped to given EGQ interface
 */
shr_error_e
dnx_sch_egq_if_to_sch_if_map_get(
    int unit,
    int core_id,
    int egq_if,
    int *sch_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_EGQ_TO_SCH_IF_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_INTERFACE_ID, egq_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SCH_INTERFACE_ID, INST_SINGLE, (uint32 *) sch_if);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map scheduler interface to EGQ interface (in reverse mode)
 */
shr_error_e
dnx_sch_dbal_sch_if_to_egq_if_map_set(
    int unit,
    int core_id,
    int sch_if,
    int egq_if,
    int is_high_priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCH_IF_TO_EGQ_IF_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_INTERFACE_ID, sch_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    if (!is_high_priority)
    {
        egq_if += dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
    }

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_INTERFACE_ID, INST_SINGLE, egq_if);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear scheduler interface to EGQ interface mapping (in reverse mode)
 */
shr_error_e
dnx_sch_dbal_sch_if_to_egq_if_map_clear(
    int unit,
    int core_id,
    int sch_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCH_IF_TO_EGQ_IF_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_INTERFACE_ID, sch_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get scheduler interface to EGQ interface mapping (in reverse mode)
 */
shr_error_e
dnx_sch_dbal_sch_if_to_egq_if_map_get(
    int unit,
    int core_id,
    int sch_if,
    int *egq_if,
    int *is_high_priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SCH_IF_TO_EGQ_IF_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_INTERFACE_ID, sch_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EGQ_INTERFACE_ID, INST_SINGLE, (uint32 *) egq_if);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (*egq_if < dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit))
    {
        *is_high_priority = TRUE;
    }
    else
    {
        *is_high_priority = FALSE;
        *egq_if -= dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert TCG to HW TCG
 * HW TCG takes into acount offset of base HR inside Port Scheduler (PS)
 */
static shr_error_e
dnx_sch_port_tcg_to_hw_tcg_convert(
    int unit,
    int base_hr,
    int tcg,
    int *hw_tcg)
{
    int offset_in_ps;

    SHR_FUNC_INIT_VARS(unit);

    offset_in_ps = base_hr % dnx_data_sch.ps.nof_hrs_in_ps_get(unit);
    *hw_tcg = tcg + offset_in_ps;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert TCG to HW TCG
 * HW TCG takes into acount offset of base HR inside Port Scheduler (PS)
 */
static shr_error_e
dnx_sch_port_hw_tcg_to_tcg_convert(
    int unit,
    int base_hr,
    int hw_tcg,
    int *tcg)
{
    int offset_in_ps;

    SHR_FUNC_INIT_VARS(unit);

    offset_in_ps = base_hr % dnx_data_sch.ps.nof_hrs_in_ps_get(unit);
    *tcg = hw_tcg - offset_in_ps;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure TC to TCG mapping
 */
shr_error_e
dnx_sch_port_tc_to_tcg_map_set(
    int unit,
    bcm_port_t logical_port,
    int tc,
    int tcg)
{
    bcm_core_t core_id;
    int base_hr;

    int tcg_hw;

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_HR_TO_TCG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, base_hr + tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_to_hw_tcg_convert(unit, base_hr, tcg, &tcg_hw));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG, INST_SINGLE, tcg_hw);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TCG for specific TC
 */
shr_error_e
dnx_sch_port_tc_to_tcg_map_get(
    int unit,
    bcm_port_t logical_port,
    int tc,
    int *tcg)
{
    bcm_core_t core_id;
    int base_hr;

    int tcg_hw;

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_HR_TO_TCG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, base_hr + tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TCG, INST_SINGLE, (uint32 *) &tcg_hw);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_sch_port_hw_tcg_to_tcg_convert(unit, base_hr, tcg_hw, tcg));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See scheduler_dbal.h
 */
shr_error_e
dnx_sch_port_tcg_weight_set(
    int unit,
    int core,
    int base_hr,
    int tcg,
    int is_valid,
    int weight)
{
    int tcg_hw;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_WEIGHT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PS_ID,
                               base_hr / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_to_hw_tcg_convert(unit, base_hr, tcg, &tcg_hw));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_ID, tcg_hw);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, is_valid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \see scheduler_dbal.h
 */
shr_error_e
dnx_sch_port_tcg_weight_get(
    int unit,
    int core,
    int base_hr,
    int tcg,
    int *is_valid,
    int *weight)
{
    int tcg_hw;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_WEIGHT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PS_ID,
                               base_hr / dnx_data_sch.ps.nof_hrs_in_ps_get(unit));
    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_to_hw_tcg_convert(unit, base_hr, tcg, &tcg_hw));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_ID, tcg_hw);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, (uint32 *) is_valid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, (uint32 *) weight);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Port TC bandwidth
 */
shr_error_e
dnx_sch_port_tc_bandwidth_set(
    int unit,
    int core_id,
    int hr_id,
    uint32 quanta_to_add)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TC_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUANTA_TO_ADD, INST_SINGLE, quanta_to_add);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Port TC bandwidth
 */
shr_error_e
dnx_sch_port_tc_bandwidth_get(
    int unit,
    int core_id,
    int hr_id,
    uint32 *quanta_to_add)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TC_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUANTA_TO_ADD, INST_SINGLE, quanta_to_add);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Port TC max burst
 */
shr_error_e
dnx_sch_port_tc_max_burst_set(
    int unit,
    int core_id,
    int hr_id,
    uint32 max_burst)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TC_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Port TC max burst
 */
shr_error_e
dnx_sch_port_tc_max_burst_get(
    int unit,
    int core_id,
    int hr_id,
    uint32 *max_burst)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TC_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset Port TC max burst to default value
 */
shr_error_e
dnx_sch_port_tc_max_burst_reset(
    int unit,
    int core_id,
    int hr_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TC_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, hr_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Port TCG bandwidth
 */
shr_error_e
dnx_sch_port_tcg_bandwidth_set(
    int unit,
    int core_id,
    int global_tcg,
    uint32 quanta_to_add)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_TCG_ID, global_tcg);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QUANTA_TO_ADD, INST_SINGLE, quanta_to_add);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Port TCG bandwidth
 */
shr_error_e
dnx_sch_port_tcg_bandwidth_get(
    int unit,
    int core_id,
    int global_tcg,
    uint32 *quanta_to_add)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_TCG_ID, global_tcg);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUANTA_TO_ADD, INST_SINGLE, quanta_to_add);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset Port TCG bandwidth
 */
shr_error_e
dnx_sch_port_tcg_bandwidth_reset(
    int unit,
    int core_id,
    int global_tcg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_TCG_ID, global_tcg);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Port TCG max burst
 */
shr_error_e
dnx_sch_port_tcg_max_burst_set(
    int unit,
    int core_id,
    int global_tcg,
    uint32 max_burst)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_TCG_ID, global_tcg);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Port TCG max burst
 */
shr_error_e
dnx_sch_port_tcg_max_burst_get(
    int unit,
    int core_id,
    int global_tcg,
    uint32 *max_burst)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_TCG_ID, global_tcg);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_BURST, INST_SINGLE, max_burst);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - reset Port TCG max burst to default value
 */
shr_error_e
dnx_sch_port_tcg_max_burst_reset(
    int unit,
    int core_id,
    int global_tcg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_TCG_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_TCG_ID, global_tcg);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize port shapers configuration
 */
shr_error_e
dnx_sch_port_shaper_config_init(
    int unit)
{
    uint32 entry_handle_id;

    dbal_tables_e table_id[] = {
        DBAL_TABLE_SCH_PORT_TC_SHAPER_CONFIGURATION,
        DBAL_TABLE_SCH_PORT_TCG_SHAPER_CONFIGURATION
    };

    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (i = 0; i < 2; i++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id[i], &entry_handle_id));

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ACCESS_PERIOD, INST_SINGLE,
                                     dnx_data_sch.ps.tc_tcg_calendar_access_period_get(unit));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_MAX_INDEX, INST_SINGLE,
                                     dnx_data_sch.flow.nof_hr_get(unit) - 1);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Port shaper calendar parameters
 */
shr_error_e
dnx_sch_port_shaper_calendar_params_get(
    int unit,
    int core_id,
    dnx_sch_port_calendar_e port_calendar,
    uint32 *access_period,
    uint32 *calendar_length)
{
    uint32 entry_handle_id;
    uint32 calendar_max_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, (port_calendar == DNX_SCH_PORT_TC_CALENDAR ?
                                             DBAL_TABLE_SCH_PORT_TC_SHAPER_CONFIGURATION :
                                             DBAL_TABLE_SCH_PORT_TCG_SHAPER_CONFIGURATION), &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CALENDAR_ACCESS_PERIOD, INST_SINGLE, access_period);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CALENDAR_MAX_INDEX, INST_SINGLE, &calendar_max_index);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *calendar_length = calendar_max_index + 1;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - port shaper credit divider set
 */
shr_error_e
dnx_sch_port_shaper_credit_divider_set(
    int unit,
    uint32 credit_worth)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_CREDIT_CONFIGURATION, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH, INST_SINGLE, credit_worth);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - port shaper credit divider get
 */
shr_error_e
dnx_sch_port_shaper_credit_divider_get(
    int unit,
    uint32 *credit_worth)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_CREDIT_CONFIGURATION, &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_WORTH, INST_SINGLE, credit_worth);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface calendar set
 */
shr_error_e
dnx_sch_interface_calendar_set(
    int unit,
    int core_id,
    int sch_if,
    int interface_index,
    int select,
    int nof_slots,
    uint32 *slots)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    int entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_id = DNX_SCH_IF_CALENDAR_DBAL_TABLE_GET(unit, sch_if);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_ID, sch_if + interface_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SELECT, select);

    for (entry = 0; entry < nof_slots; entry++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY, entry);

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE,
                                     slots[entry] + interface_index);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface calendar get
 */
shr_error_e
dnx_sch_interface_calendar_get(
    int unit,
    int core_id,
    int sch_if,
    int select,
    int nof_slots,
    uint32 *slots)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    int entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_id = DNX_SCH_IF_CALENDAR_DBAL_TABLE_GET(unit, sch_if);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_ID, sch_if);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SELECT, select);

    for (entry = 0; entry < nof_slots; entry++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY, entry);

        /** Setting pointers value to receive the fields */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, &slots[entry]);

        /** Performing the action after this call the pointers that we set in field32_request() */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface calendar parameters set
 */
shr_error_e
dnx_sch_interface_calendar_params_set(
    int unit,
    int core_id,
    int sch_if,
    int select,
    int nof_slots)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_IF_CALENDAR_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_CAL_ID, sch_if);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SELECT, INST_SINGLE, select);
    dbal_entry_value_field32_set(unit, entry_handle_id, (select ? DBAL_FIELD_CALENDAR_B_MAX_INDEX :
                                                         DBAL_FIELD_CALENDAR_A_MAX_INDEX), INST_SINGLE, nof_slots - 1);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface calendar parameters get
 */
shr_error_e
dnx_sch_interface_calendar_params_get(
    int unit,
    int core_id,
    int sch_if,
    int *select,
    int *nof_slots)
{
    uint32 entry_handle_id;
    dbal_fields_e field_id;
    uint32 cal_max_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_IF_CALENDAR_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_CAL_ID, sch_if);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SELECT, INST_SINGLE,
                                                        (uint32 *) select));

    field_id = ((*select) ? DBAL_FIELD_CALENDAR_B_MAX_INDEX : DBAL_FIELD_CALENDAR_A_MAX_INDEX);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, INST_SINGLE, &cal_max_index));

    *nof_slots = cal_max_index + 1;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface calendar speed set
 */
shr_error_e
dnx_sch_interface_calendar_speed_set(
    int unit,
    int core_id,
    int sch_if,
    int nof_interfaces,
    uint32 cal_speed)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_IF_CALENDAR_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_SCH_CAL_ID, sch_if, sch_if + nof_interfaces - 1);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALENDAR_SPEED, INST_SINGLE, cal_speed);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface calendar speed get
 */
shr_error_e
dnx_sch_interface_calendar_speed_get(
    int unit,
    int core_id,
    int sch_if,
    uint32 *cal_speed)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_IF_CALENDAR_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_CAL_ID, sch_if);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CALENDAR_SPEED, INST_SINGLE, cal_speed);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface shaper bandwidth set
 */
shr_error_e
dnx_sch_interface_shaper_bandwidth_hw_set(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_IF_ID, sch_if);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RATE, INST_SINGLE, rate);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - scheduler interface shaper bandwidth get
 */
shr_error_e
dnx_sch_interface_shaper_bandwidth_hw_get(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 *rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_IF_ID, sch_if);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_RATE, INST_SINGLE, rate);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure interface DVS priority
 */
shr_error_e
dnx_sch_dbal_interface_priority_set(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_IF_ID, sch_if);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DVS_PRIORITY, INST_SINGLE, priority);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get interface DVS priority
 */
shr_error_e
dnx_sch_dbal_interface_priority_get(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if,
    uint32 *priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_IF_ID, sch_if);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DVS_PRIORITY, INST_SINGLE, priority);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear interface DVS priority
 */
shr_error_e
dnx_sch_dbal_interface_priority_clear(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int sch_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_IF_ID, sch_if);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map port to interface
 */
shr_error_e
dnx_sch_dbal_port_to_interface_map_set(
    int unit,
    int core_id,
    int sch_if,
    int base_hr)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_NON_CHANNELIZED_IF_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_IF_ID, sch_if);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, base_hr);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - unmap port to interface
 */
shr_error_e
dnx_sch_dbal_port_to_interface_map_clear(
    int unit,
    int core_id,
    int sch_if)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_NON_CHANNELIZED_IF_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_IF_ID, sch_if);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set erp enable
 */
shr_error_e
dnx_sch_erp_enable_set(
    int unit,
    int core_id,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PER_CORE_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERP_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get erp enable
 */
shr_error_e
dnx_sch_erp_enable_get(
    int unit,
    int core_id,
    uint32 *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PER_CORE_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERP_ENABLE, INST_SINGLE, enable);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Global DRM entry
 */
shr_error_e
dnx_sch_global_drm_set(
    int unit,
    int rci_level,
    int active_links,
    uint32 rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHARED_DRM, &entry_handle_id));

    /** Setting key fields */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_LINKS, active_links);
    }
    else
    {
        /** when no fabric - all entries are configured to the same value */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_LINKS,
                                         DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RCI_LEVEL, rci_level);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, rate);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Global DRM entry
 */
shr_error_e
dnx_sch_global_drm_get(
    int unit,
    int rci_level,
    int active_links,
    uint32 *rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHARED_DRM, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_LINKS, active_links);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RCI_LEVEL, rci_level);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, rate);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Core DRM entry
 */
shr_error_e
dnx_sch_core_drm_set(
    int unit,
    int core,
    int rci_level,
    int active_links,
    uint32 rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CORE_DRM, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_LINKS, active_links);
    }
    else
    {
        /** when no fabric - all entries are configured to the same value */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_LINKS,
                                         DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RCI_LEVEL, rci_level);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, rate);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Core DRM entry
 */
shr_error_e
dnx_sch_core_drm_get(
    int unit,
    int core,
    int rci_level,
    int active_links,
    uint32 *rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CORE_DRM, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_LINKS, active_links);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RCI_LEVEL, rci_level);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, rate);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure rci_biasing_mode to HW
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] rci_biasing_mode - rci biasing mode
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e
dnx_sch_rci_throttling_mode_hw_set(
    int unit,
    dbal_enum_value_field_rci_biasing_mode_e rci_biasing_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

     /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_RCI_BIASING_MODE_CONFIG, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RCI_BIASING_MODE, INST_SINGLE, rci_biasing_mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * read rci_biasing_mode from HW
 *
 * \param [in] unit -  Unit-ID 
 * \param [out] rci_biasing_mode - rci biasing mode
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e
dnx_sch_rci_throttling_mode_hw_get(
    int unit,
    dbal_enum_value_field_rci_biasing_mode_e * rci_biasing_mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

     /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_RCI_BIASING_MODE_CONFIG, &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RCI_BIASING_MODE, INST_SINGLE,
                               (uint32 *) rci_biasing_mode);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure device scheduler
 */
shr_error_e
dnx_sch_dbal_device_scheduler_config_set(
    int unit,
    int mode,
    int weight)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

     /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_DEVICE_SCHEDULER, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEVICE_SCHEDULER_MODE, INST_SINGLE, mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure overwrite slow rate global configuration
 */
shr_error_e
dnx_sch_overwrite_slow_rate_set(
    int unit,
    int overwrite_slow_rate)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OVERRIDE_SLOW_RATE, INST_SINGLE,
                                 overwrite_slow_rate);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset delete mechanism (dlm)
 */
shr_error_e
dnx_sch_dlm_reset_trigger_set(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DLM_RESET_TRIGGER, INST_SINGLE, TRUE);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure delete mechanism (dlm) enable for all groups
 */
shr_error_e
dnx_sch_dlm_enable_all_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    int field_size;
    SHR_BITDCL mask;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Get number of bits of the exponent field */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_SCH_CONFIGURATION, DBAL_FIELD_DLM_ENABLE,
                                               0, 0, 0, &field_size));

    mask = 0;
    if (enable)
    {
        SHR_BITSET_RANGE(&mask, 0, field_size);
    }
    else
    {
        SHR_BITCLR_RANGE(&mask, 0, field_size);
    }

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DLM_ENABLE, INST_SINGLE, mask);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SMP backup message enable global configuration
 */
shr_error_e
dnx_sch_backup_msg_enable_set(
    int unit,
    int backup_msg_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SMP_BACKUP_MSG_ENABLE, INST_SINGLE,
                                 backup_msg_enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure qpm override global configuration
 */
shr_error_e
dnx_sch_dbal_qpm_override_set(
    int unit,
    int qpm_override)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QPM_OVERRIDE, INST_SINGLE, qpm_override);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure priority propagation enable
 */
shr_error_e
dnx_sch_dbal_priority_propagation_enable_set(
    int unit,
    int priority_propagation_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_PRIORITY_PROPAGATION_ENABLE, INST_SINGLE,
                                 priority_propagation_enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure TCG strict priority mode
 */
shr_error_e
dnx_sch_dbal_tcg_strict_priority_set(
    int unit,
    int tcg_strict_priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_STRICT_PRIORITY, INST_SINGLE,
                                 tcg_strict_priority);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TCG strict priority mode
 */
shr_error_e
dnx_sch_dbal_tcg_strict_priority_get(
    int unit,
    int *tcg_strict_priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

     /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TCG_STRICT_PRIORITY, INST_SINGLE,
                               (uint32 *) tcg_strict_priority);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure shared shaper mode
 */
shr_error_e
dnx_sch_shared_shaper_mode_set(
    int unit,
    int core_id,
    int flow_id,
    dbal_enum_value_field_shared_shaper_mode_e mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHARED_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_OCTET_ID, flow_id / 8);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_SHAPER_MODE, INST_SINGLE, mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get shared shaper mode
 */
shr_error_e
dnx_sch_shared_shaper_mode_get(
    int unit,
    int core_id,
    int flow_id,
    dbal_enum_value_field_shared_shaper_mode_e * mode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

     /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_SHARED_SHAPER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW_OCTET_ID, flow_id / 8);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_SHAPER_MODE, INST_SINGLE, (uint32 *) mode);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * --- Counters ---
 */

/**
 * 
 * \brief - Function to set flow to be tracked when counting 
 *        credit count
 *  
 * \author db889754 (2/20/2018)
 * 
 * \param unit
 * \param core
 * \param flow_id
 * 
 * \return shr_error_e
 */
shr_error_e
dnx_sch_flow_credit_counter_configuration_set(
    int unit,
    int core,
    int flow_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_CREDIT_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Make sure that we are counting for Flow */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_FLOW, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_SUB_FLOW, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_DEST_FAP, INST_SINGLE, 0);

    /** Set Flow to track its credit count   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, INST_SINGLE, flow_id);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - Function to disable the filters applied on the credit counters per flow
 *
 * \author db889754 (2/20/2018)
 *
 * \param unit [in]- unit
 * \param core [in]- core
 *
 * \return shr_error_e
 */
shr_error_e
dnx_sch_flow_credit_counter_configuration_reset(
    int unit,
    int core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_CREDIT_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Reset filters */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** 
 * \brief - Function to return the credit count of the flow 
 *        configured by this function -
 *        dnx_sch_flow_credit_counter_configuration_set
 *  
 * \author db889754 (2/20/2018)
 * 
 * \param unit
 * \param core_id
 * \param credit_count
 * 
 * \return shr_error_e
 */
shr_error_e
dnx_sch_flow_credit_counter_info_get(
    int unit,
    int core_id,
    uint32 *credit_count)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_CREDIT_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get Flow credit count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_COUNT, INST_SINGLE, credit_count);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * 
 * \brief - Function to set port to be tracked when counting 
 *        credit count
 *  
 * \author db889754 (2/20/2018)
 * 
 * \param unit
 * \param core
 * \param hr_id
 * 
 * \return shr_error_e
 */
shr_error_e
dnx_sch_port_credit_counter_configuration_set(
    int unit,
    int core,
    int hr_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_CREDIT_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Make sure that we are counting for Port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_PORT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_NIF, INST_SINGLE, 0);

    /** Set Port to track its credit count   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, hr_id);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *
 * \brief - Function to disable the filters applied on the credit counters per port
 *
 * \param unit [in]- unit
 * \param core [in]- core
 *
 * \return shr_error_e
 */
shr_error_e
dnx_sch_port_credit_counter_configuration_reset(
    int unit,
    int core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_CREDIT_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Disable the filters*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * 
 * \brief - Function to return the credit count of the port 
 *        configured by this function -
 *        dnx_sch_port_credit_counter_configuration_set
 *  
 * \author db889754 (2/20/2018)
 * 
 * \param unit
 * \param core_id
 * \param credit_count
 * 
 * \return shr_error_e
 */
shr_error_e
dnx_sch_port_credit_counter_info_get(
    int unit,
    int core_id,
    uint32 *credit_count)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PORT_CREDIT_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get Port credit count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_COUNT, INST_SINGLE, credit_count);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * 
 * \brief - Function to set port to be tracked when counting 
 *        flow control indications count
 *  
 * \author db889754 (2/20/2018)
 * 
 * \param unit
 * \param core
 * \param hr_id
 * 
 * \return shr_error_e
 */
shr_error_e
dnx_sch_port_fc_counter_configuration_set(
    int unit,
    int core,
    int hr_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FC_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Make sure that we are counting for Port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_PER_PORT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_PER_IF_FC, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_PER_CL_FC, INST_SINGLE, 0);

    /** Set Port to track its credit count   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, hr_id);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * 
 * \brief - Function to return the fc indications count of the 
 *        port configured by this function -
 *        dnx_sch_port_fc_counter_configuration_set
 *  
 * \author db889754 (2/20/2018)
 * 
 * \param unit
 * \param core_id
 * \param fc_count
 * 
 * \return shr_error_e
 */
shr_error_e
dnx_sch_port_fc_counter_info_get(
    int unit,
    int core_id,
    uint32 *fc_count)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FC_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Get Port credit count */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FC_COUNT, INST_SINGLE, fc_count);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** */
shr_error_e
dnx_sch_flow_slow_status_get(
    int unit,
    int core_id,
    int flow_id,
    dbal_enum_value_field_flow_slow_status_e * slow_status)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FLOW_SLOW_STATUS, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Select the flow */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, flow_id);

    /** Get flow slow status */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLOW_SLOW_STATUS, INST_SINGLE, slow_status);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
shr_error_e
dnx_sch_dbal_dlm_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Set to MAX Value*/
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_DLM_PER_GROUP_BW, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);

    /** Set to MAX Value*/
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_LOW_DLM_PER_GROUP_BW, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);

    /** Disable CLEAR_SAD_ON_GROUP_CHANGE*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLEAR_SAD_ON_GROUP_CHANGE, INST_SINGLE, 0);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ see .h file
 */
shr_error_e
dnx_sch_dbal_port_to_base_hr_mapping_set(
    int unit,
    int core_id,
    uint32 tm_port,
    int base_hr_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_DSP_TO_HR_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, base_hr_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ see .h file
 */
shr_error_e
dnx_sch_dbal_port_to_base_hr_mapping_clear(
    int unit,
    int core_id,
    uint32 tm_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_DSP_TO_HR_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ see .h file
 */
shr_error_e
dnx_sch_dbal_port_to_base_hr_mapping_get(
    int unit,
    int core_id,
    uint32 tm_port,
    int *base_hr_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_DSP_TO_HR_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, (uint32 *) base_hr_id);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief configure q-pair to HR mapping
 */
shr_error_e
dnx_sch_dbal_qpair_to_hr_mapping_set(
    int unit,
    int core_id,
    int egq_core,
    uint32 q_pair,
    int hr_id,
    int valid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_QPAIR_TO_HR_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_CORE, egq_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, q_pair);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, hr_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, valid);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief clear q-pair to HR mapping
 */
shr_error_e
dnx_sch_dbal_qpair_to_hr_mapping_clear(
    int unit,
    int core_id,
    int egq_core,
    uint32 q_pair)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_QPAIR_TO_HR_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_CORE, egq_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, q_pair);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief get q-pair to HR mapping
 */
shr_error_e
dnx_sch_dbal_qpair_to_hr_mapping_get(
    int unit,
    int core_id,
    int egq_core,
    uint32 q_pair,
    int *hr_id,
    int *valid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_QPAIR_TO_HR_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_CORE, egq_core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, q_pair);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HR_ID, INST_SINGLE, (uint32 *) hr_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, (uint32 *) valid);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief configure double low rate
 *
 * \param [in] unit - Unit ID
 * \param [in] factor - low rate factor
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
dnx_scheduler_dbal_low_rate_factor_set(
    int unit,
    dbal_enum_value_field_low_rate_factor_e factor)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_FACTOR, INST_SINGLE, factor);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get  low rate factor
 *
 * \param [in] unit - Unit ID
 * \param [out] factor - factor
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
dnx_scheduler_dbal_low_rate_factor_get(
    int unit,
    dbal_enum_value_field_low_rate_factor_e * factor)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_FACTOR, INST_SINGLE, factor);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief configure low rate connectors range
 */
shr_error_e
dnx_sch_dbal_low_rate_range_set(
    int unit,
    int core_id,
    int flow_min,
    int flow_max,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PER_CORE_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_MIN, INST_SINGLE, flow_min / 16);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_MAX, INST_SINGLE, flow_max / 16);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief configure low rate connectors range
 */
shr_error_e
dnx_sch_dbal_low_rate_range_get(
    int unit,
    int core_id,
    int *flow_min,
    int *flow_max,
    uint32 *enable)
{
    uint32 entry_handle_id;
    uint32 hw_min, hw_max;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_PER_CORE_CONFIGURATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_MIN, INST_SINGLE, &hw_min);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_MAX, INST_SINGLE, &hw_max);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LOW_RATE_ENABLE, INST_SINGLE, enable);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *flow_min = hw_min * 16;
    *flow_max = hw_max * 16;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief configure LAG port core flip
 */
shr_error_e
dnx_sch_dbal_lag_port_core_flip_set(
    int unit,
    int core_id,
    int tm_port,
    int core_flip)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_LAG_PORT_CONFIG, &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_FLIP, INST_SINGLE, core_flip);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief configure LAG port core flip
 */
shr_error_e
dnx_sch_dbal_lag_port_member_set(
    int unit,
    int core_id,
    int tm_port,
    int member)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_LAG_PORT_CONFIG, &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEMBER, INST_SINGLE, member);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief configure LAG port member
 */
shr_error_e
dnx_sch_dbal_dummy_tm_port_set(
    int unit,
    int tm_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CONFIGURATION, &entry_handle_id));

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DUMMY_TM_PORT, INST_SINGLE, tm_port);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* --------------------------------
 *     counters
 * --------------------------------
 */

/*
 * see .h file
 */
shr_error_e
dnx_sch_dbal_credit_counter_config_set(
    int unit,
    int core_id,
    dbal_enum_value_field_sch_credit_counter_type_e counter_type,
    uint32 is_group_match,
    uint32 is_flow_match,
    uint32 group,
    uint32 flow)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CREDIT_COUNTER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_TYPE, INST_SINGLE, counter_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_ONLY_GROUP_MATCH, INST_SINGLE, is_group_match);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_ONLY_FLOW_MATCH, INST_SINGLE, is_flow_match);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP, INST_SINGLE, group);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLOW, INST_SINGLE, flow);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ see .h file
 */
shr_error_e
dnx_sch_dbal_credit_counter_config_get(
    int unit,
    int core_id,
    dbal_enum_value_field_sch_credit_counter_type_e * counter_type,
    uint32 *is_group_match,
    uint32 *is_flow_match,
    uint32 *group,
    uint32 *flow)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CREDIT_COUNTER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER_TYPE, INST_SINGLE, counter_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNT_ONLY_GROUP_MATCH, INST_SINGLE, is_group_match);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNT_ONLY_FLOW_MATCH, INST_SINGLE, is_flow_match);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GROUP, INST_SINGLE, group);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLOW, INST_SINGLE, flow);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ see .h file
 */
shr_error_e
dnx_sch_dbal_credit_counter_get(
    int unit,
    int core_id,
    uint32 *counter_val,
    uint32 *is_ovf)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CREDIT_COUNTER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

   /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_COUNTER_VAL, INST_SINGLE, counter_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CREDIT_COUNTER_OVF, INST_SINGLE, is_ovf);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ brief configure SMP message counter
 */

/**
 * \ brief configure SMP message counter fabric type, flow id and filters
 */

shr_error_e
dnx_sch_fsm_counter_configuration_fabric_type_set(
    int unit,
    int core,
    uint32 flow_id,
    uint32 fabric_type,
    uint32 filter_by_type,
    uint32 filter_by_flow_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FSM_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Set the fields values  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_TYPE, INST_SINGLE, fabric_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_TYPE, INST_SINGLE, filter_by_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, INST_SINGLE, flow_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_FLOW_OR_PORT, INST_SINGLE,
                                 filter_by_flow_id);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ see .h file
 */
shr_error_e
dnx_sch_calrx_rxi_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_CALRX_RXI_ENABLE, &entry_handle_id));

    /** Set the fields values  */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALRX_INTERFACE_FC_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CALRX_PORT_FC_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RXI_ENABLE, INST_SINGLE, enable);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ brief get SMP message counter fabric type, flow id and filters
 */

shr_error_e
dnx_sch_fsm_counter_configuration_fabric_type_get(
    int unit,
    int core,
    uint32 *flow_id,
    uint32 *fabric_type,
    uint32 *filter_by_type,
    uint32 *filter_by_flow_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FSM_COUNTER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting pointer values to recieve the fields*/
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNT_TYPE, INST_SINGLE, fabric_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_TYPE, INST_SINGLE, filter_by_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SCH_FLOW_ID, INST_SINGLE, flow_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FILTER_BY_FLOW_OR_PORT, INST_SINGLE,
                               filter_by_flow_id);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \ brief get SMP message counter count
 */

shr_error_e
dnx_sch_fsm_counter_info_get(
    int unit,
    int core,
    uint32 *count,
    uint32 *overflow)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SCH_FSM_COUNTER, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting pointer values to recieve the fields*/
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FSM_COUNTER_VAL, INST_SINGLE, count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FSM_COUNTER_OVF, INST_SINGLE, overflow);

    /** Performing the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
