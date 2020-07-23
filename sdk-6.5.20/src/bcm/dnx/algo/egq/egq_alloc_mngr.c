/**
 * \file algo/egq/egq_alloc_mngr.c
 *
 * Implementation for advanced egq resource manager.
 *
 * Manage flows allocation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/*
 * Include files:
 * {
 */
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_egq_am_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/algo/egq/egq_alloc_mngr.h>
#include <soc/dnx/dbal/dbal.h>
/*
 * } 
 */

/*
 * Structs:
 * {
 */

/*
 * } 
 */

/*
 * Static Functions:
 * {
 */

/*
 * } 
 */

/*
 * Functions:
 * {
 */
/*
 * Cosq Egress Queue Mapping - Start
 * {
 */
int
dnx_am_template_egress_queue_mapping_create(
    int unit,
    int template_init_id,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile)
{

    dnx_algo_template_create_data_t create_info;
    uint32 core_id;
    int nof_profiles;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize the entire egq alloc manager module.
     */
    SHR_IF_ERR_EXIT(dnx_egq_am_db.init(unit));
    /*
     * Create template manager insance - handle: DNX_ALGO_TEMPLATE_EGR_QUEUE_MAPPING
     */
    nof_profiles = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_COUNT;
    sal_memset(&create_info, 0, sizeof(dnx_algo_template_create_data_t));
    create_info.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    create_info.first_profile = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_LOW_ID;
    create_info.nof_profiles = nof_profiles;
    create_info.max_references = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_MAX_ENTITIES(unit);
    create_info.default_profile = template_init_id;
    create_info.data_size = DNX_AM_TEMPLATE_EGR_QUEUE_MAPPING_SIZE;
    create_info.default_data = mapping_profile;
    sal_strncpy(create_info.name, DNX_ALGO_TEMPLATE_EGR_TC_DP_MAPPING, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_egq_am_db.egr_tc_dp_mapping.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_egq_am_db.egr_tc_dp_mapping.create(unit, core_id, &create_info, NULL));
    }

    /*
     * Create template manager insance to handle FQP profiles
     */
    if (dnx_data_egr_queuing.params.feature_get(unit, dnx_data_egr_queuing_params_if_attributes_profile_exist) == 1)
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, DBAL_TABLE_EGQ_FQP_PROFILE_ATTRIBUTES, DBAL_FIELD_PROFILE_INDEX, TRUE, 0, 0,
                         &field_info));
        nof_profiles = field_info.max_value + 1;
        sal_memset(&create_info, 0, sizeof(dnx_algo_template_create_data_t));

        create_info.flags = 0;
        create_info.first_profile = DNX_AM_TEMPLATE_FQP_LOW_ID;
        create_info.nof_profiles = nof_profiles;
        create_info.max_references = dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
        create_info.default_profile = template_init_id;
        create_info.data_size = sizeof(dnx_fqp_profile_params_t);
        sal_strncpy(create_info.name, DNX_ALGO_TEMPLATE_FQP_PROFILE_PARAMS, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(dnx_egq_am_db.fqp_profile.alloc(unit));

        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_egq_am_db.fqp_profile.create(unit, core_id, &create_info, NULL));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_am_template_egress_queue_mapping_data_get(
    int unit,
    uint32 pp_port,
    int core,
    dnx_cosq_egress_queue_mapping_info_t * mapping_profile)
{
    int index, ref_count;
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rc = dnx_egr_queuing_profile_map_get(unit, core, pp_port, (uint32 *) &index);
    SHR_IF_ERR_EXIT(rc);

    if (index < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Failed to get the old profile");
    }

    rc = dnx_egq_am_db.egr_tc_dp_mapping.profile_data_get(unit, core, index, &ref_count, mapping_profile);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

int
dnx_am_template_egress_queue_mapping_exchange(
    int unit,
    uint32 pp_port,
    int core,
    const dnx_cosq_egress_queue_mapping_info_t * mapping_profile,
    int *old_profile,
    int *is_last,
    int *new_profile,
    int *is_allocated)
{
    int rc = _SHR_E_NONE;
    int tmp_old_profile;
    uint8 tmp_is_last = 0;
    uint8 tmp_is_alloc = 0;

    SHR_FUNC_INIT_VARS(unit);

    rc = dnx_egr_queuing_profile_map_get(unit, core, pp_port, (uint32 *) &tmp_old_profile);
    SHR_IF_ERR_EXIT(rc);

    rc = dnx_egq_am_db.egr_tc_dp_mapping.exchange
        (unit, core, 0, mapping_profile, tmp_old_profile, NULL, new_profile, &tmp_is_alloc, &tmp_is_last);
    SHR_IF_ERR_EXIT(rc);

    if (old_profile != NULL)
    {
        *old_profile = tmp_old_profile;
    }

    if (is_last != NULL)
    {
        *is_last = tmp_is_last ? 1 : 0;
    }

    if (is_allocated != NULL)
    {
        *is_allocated = tmp_is_alloc ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_am_fqp_profile_alloc_single(
    int unit,
    int core,
    int *profile_id,
    dnx_fqp_profile_params_t * profile_params,
    uint8 *is_first)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_egq_am_db.fqp_profile.allocate_single
                    (unit, core, 0, profile_params, NULL, profile_id, is_first));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_am_fqp_profile_free_single(
    int unit,
    int core,
    int profile_id,
    uint8 *is_last)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_egq_am_db.fqp_profile.free_single(unit, core, profile_id, is_last));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egq_alloc_mngr.h file.
 */
void
dnx_algo_egr_tc_dp_mapping_print_cb(
    int unit,
    const void *data)
{
    dnx_cosq_egress_queue_mapping_info_t *template_data = (dnx_cosq_egress_queue_mapping_info_t *) data;
    uint32 i, j, k;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    for (i = 0; i < DNX_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES; i++)
    {
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "multicast", i, NULL, "%u");
        for (j = 0; j < DNX_COSQ_NOF_TC; j++)
        {
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "tc", j, NULL, "%u");
            for (k = 0; k < DNX_COSQ_NOF_DP; k++)
            {
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dp", k, NULL, "%u");
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "tc_map",
                                               template_data->queue_mapping[i][j][k].tc, NULL, "%u");
                SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dp_map",
                                               template_data->queue_mapping[i][j][k].dp, NULL, "%u");
            }
        }
    }
    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/*
 * See egq_alloc_mngr.h file.
 */
void
dnx_algo_fqp_profile_params_print_cb(
    int unit,
    const void *data)
{
    dnx_fqp_profile_params_t *template_data = (dnx_fqp_profile_params_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, NULL, "egress deq profile params:", NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "irdy_thr", template_data->irdy_thr, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT,
                                   "txq_max_bytes", template_data->txq_max_bytes, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT,
                                   "is_belong_to_sub_calendar", template_data->is_belong_to_sub_calendar, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "txq_tdm_irdy", template_data->txq_tdm_irdy,
                                   NULL, "%d");

    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/*
 * Cosq Egress Queue Mapping - End
 * }
 */
/**
 * }
 */
