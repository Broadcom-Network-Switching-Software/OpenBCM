/**
* \file algo_ecgm.c
*
* Egress congestion algorithms
*
* this file holds the implementation
* for ecgm algorithms.
* for more info see
* bcm_int/dnx/algo/ecgm/algo_ecgm.h
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/**
* INCLUDE FILES:
* {
*/

#include <soc/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/ecgm/algo_ecgm.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_ecgm_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

/** } **/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/**
 * Defines for template print callback
 * {
 */

/** \brief
 * adding lines for template print callback (to be printed using 'algo template').
 * prints values for unicast packet descriptors thresholds.
 * uc_thresholds_struct must be of type dnx_ecgm_uc_thresholds_t
 */
#define DNX_ALGO_ECGM_UNICAST_PD_THRESHOLDS_TEMPLATE_PRINT(uc_thresholds_struct) {\
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "PD drop", uc_thresholds_struct.pd_drop, NULL, NULL);\
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "PD flow control", "", NULL, NULL);\
    DNX_ALGO_ECGM_FADT_THRESHOLDS_TEMPLATE_PRINT(uc_thresholds_struct.flow_control.pd_fc);\
}

 /** \brief
 * adding lines for template print callback (to be printed using 'algo template').
 * prints values for unicast data buffers thresholds.
 * uc_thresholds_struct must be of type dnx_ecgm_uc_thresholds_t
 */
#define DNX_ALGO_ECGM_UNICAST_DB_THRESHOLDS_TEMPLATE_PRINT(uc_thresholds_struct) {\
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "DB drop", uc_thresholds_struct.db_drop, NULL, NULL);\
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "DB flow control", "", NULL, NULL);\
    DNX_ALGO_ECGM_FADT_THRESHOLDS_TEMPLATE_PRINT(uc_thresholds_struct.flow_control.db_fc);\
}

 /** \brief
 * adding lines for template print callback (to be printed using 'algo template').
 * prints values for multicast queue (per DP) thresholds.
 * mc_q_dp_thresholds must be a pointer to an array of 4 DPs, of type dnx_ecgm_mc_drop_thresholds_t
 */
#define DNX_ALGO_ECGM_MULTICAST_QUEUE_THRESHOLDS_PER_DP_TEMPLATE_PRINT(mc_q_dp_thresholds) {\
    int dp;\
    for(dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)\
    {\
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "dp", "", NULL, NULL);\
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "dp", dp, NULL, "dp #%d");\
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "DB drop", mc_q_dp_thresholds[dp].db_drop, NULL, NULL); \
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "PD drop", "", NULL, NULL); \
        DNX_ALGO_ECGM_FADT_THRESHOLDS_TEMPLATE_PRINT(mc_q_dp_thresholds[dp].pd_drop);\
    } }

 /** \brief
 * adding lines for template print callback (to be printed using 'algo template').
 * prints values for fadt thresholds.
 * fadt_struct must be of type dnx_ecgm_fadt_threshold_t
 */
#define DNX_ALGO_ECGM_FADT_THRESHOLDS_TEMPLATE_PRINT(fadt_struct) {\
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();    /* fadt */ \
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max", fadt_struct.max, NULL, NULL);\
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "min", fadt_struct.min, NULL, NULL);\
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "alpha", fadt_struct.alpha, NULL, NULL);\
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();     /* fadt */ \
}

 /** \brief
 * adding lines for template print callback (to be printed using 'algo template').
 * prints values for interface thresholds per priority.
 * interface_priority must be of type dnx_ecgm_interface_profile_thresholds_t
 */
#define DNX_ALGO_ECGM_INTERFACE_PRIORITY_THRESHOLDS_TEMPLATE_PRINT(interface_priority) {\
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "multicast PD drop", interface_priority.mc_interface_PDs_drop, NULL, NULL);\
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "unicast PD flow control", "", NULL, NULL);\
    DNX_ALGO_ECGM_FADT_THRESHOLDS_TEMPLATE_PRINT(interface_priority.uc_fc.pd_fc);\
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "unicast DB flow control", "", NULL, NULL);\
    DNX_ALGO_ECGM_FADT_THRESHOLDS_TEMPLATE_PRINT(interface_priority.uc_fc.db_fc);\
}

/** } **/

/**
* port profile internal API
*   \see documentation on algo_ecgm.h file
* {
*/

shr_error_e
dnx_algo_ecgm_port_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t port_profile_template_data;
    dnx_ecgm_port_profile_info_t profile_data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&port_profile_template_data, 0, sizeof(port_profile_template_data));
    sal_memset(&profile_data, 0, sizeof(profile_data));

    port_profile_template_data.data_size = sizeof(dnx_ecgm_port_profile_info_t);
    port_profile_template_data.default_data = &profile_data;

    port_profile_template_data.first_profile = 0;
    port_profile_template_data.max_references = dnx_data_port.general.nof_tm_ports_get(unit);
    port_profile_template_data.nof_profiles = dnx_data_ecgm.core_resources.nof_port_profiles_get(unit);
    port_profile_template_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(port_profile_template_data.name, DNX_ALGO_ECGM_PORT_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_ecgm_db.ecgm_port_profile.alloc(unit));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(algo_ecgm_db.ecgm_port_profile.create(unit, core_id, &port_profile_template_data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

void
dnx_algo_ecgm_port_profile_print_cb(
    int unit,
    const void *data)
{
    int queue;
    dnx_ecgm_port_profile_info_t *profile;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    if (data)
    {
        profile = (dnx_ecgm_port_profile_info_t *) data;

        /*
         * Port Unicast thresholds 
         */
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "Port Unicast Thresholds", "", NULL, NULL);
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();       /* unicast port */
        DNX_ALGO_ECGM_UNICAST_PD_THRESHOLDS_TEMPLATE_PRINT(profile->unicast_port);
        DNX_ALGO_ECGM_UNICAST_DB_THRESHOLDS_TEMPLATE_PRINT(profile->unicast_port);
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "Queue Unicast Thresholds", "", NULL, NULL);
        for (queue = 0; queue < DNX_COSQ_NOF_TC; ++queue)
        {
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "Queue", "", NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Queue", queue, NULL, NULL);
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();   /* unicast queue */
            DNX_ALGO_ECGM_UNICAST_PD_THRESHOLDS_TEMPLATE_PRINT(profile->unicast_queue[queue]);
            DNX_ALGO_ECGM_UNICAST_DB_THRESHOLDS_TEMPLATE_PRINT(profile->unicast_queue[queue]);
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();     /* unicast queue */
        }
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END(); /* unicast port */

        /*
         * Port Multicast thresholds 
         */
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "Port Multicast thresholds", "", NULL, NULL);
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();       /* multicast port */
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "DB drop", profile->multicast_port.db_drop,
                                       NULL, NULL);
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "PD drop", "", NULL, NULL);
        DNX_ALGO_ECGM_FADT_THRESHOLDS_TEMPLATE_PRINT(profile->multicast_port.pd_drop);

        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "Queue Multicast Thresholds", "", NULL, NULL);

        for (queue = 0; queue < DNX_COSQ_NOF_TC; ++queue)
        {
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "Queue", "", NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "Queue", queue, NULL, NULL);
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_START();   /* multicast queue */
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "PD service pool",
                                           profile->multicast_queue[queue].pd_sp, NULL, NULL);
            SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "PD reserved",
                                           profile->multicast_queue[queue].reserved_pds, NULL, NULL);
            DNX_ALGO_ECGM_MULTICAST_QUEUE_THRESHOLDS_PER_DP_TEMPLATE_PRINT(profile->
                                                                           multicast_queue[queue].drop_thresholds);
            SW_STATE_PRETTY_PRINT_SUB_STRUCT_END();     /* multicast queue */
        }
        SW_STATE_PRETTY_PRINT_SUB_STRUCT_END(); /* multicast port */

        SW_STATE_PRETTY_PRINT_FINISH();
    }

    else
    {
        LOG_CLI((BSL_META_U(unit, "invalid profile data\n")));
    }
}

shr_error_e
dnx_algo_ecgm_port_profile_template_destroy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* }
*/

/**
* interface profile internal API
*   \see documentation on algo_ecgm.h file
*/
shr_error_e
dnx_algo_ecgm_interface_profile_template_create(
    int unit)
{
    dnx_algo_template_create_data_t interface_profile_template_data;
    dnx_ecgm_interface_profile_info_t profile_data;

    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&interface_profile_template_data, 0, sizeof(interface_profile_template_data));
    sal_memset(&profile_data, 0, sizeof(profile_data));

    interface_profile_template_data.data_size = sizeof(dnx_ecgm_interface_profile_info_t);
    interface_profile_template_data.default_data = &profile_data;

    interface_profile_template_data.first_profile = 0;
    interface_profile_template_data.max_references = dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit);
    interface_profile_template_data.nof_profiles = dnx_data_ecgm.core_resources.nof_interface_profiles_get(unit);
    interface_profile_template_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(interface_profile_template_data.name, DNX_ALGO_ECGM_INTERFACE_PROFILE_TEMPLATE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_ecgm_db.ecgm_interface_profile.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(algo_ecgm_db.
                        ecgm_interface_profile.create(unit, core_id, &interface_profile_template_data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_ecgm_interface_profile_template_destroy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

void
dnx_algo_ecgm_interface_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_ecgm_interface_profile_info_t *profile;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    if (data)
    {
        profile = (dnx_ecgm_interface_profile_info_t *) data;

        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "interface thresholds", "", NULL, NULL);

        /*
         * interface High priority 
         */
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "High priority", "", NULL, NULL);
        DNX_ALGO_ECGM_INTERFACE_PRIORITY_THRESHOLDS_TEMPLATE_PRINT(profile->high_priority);

        /*
         * interface Low priority 
         */
        SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, "Low priority", "", NULL, NULL);
        DNX_ALGO_ECGM_INTERFACE_PRIORITY_THRESHOLDS_TEMPLATE_PRINT(profile->low_priority);

        SW_STATE_PRETTY_PRINT_FINISH();
    }

    else
    {
        LOG_CLI((BSL_META_U(unit, "invalid profile data\n")));
    }
}

void
dnx_algo_ecgm_port_profile_info_print(
    int unit,
    dnx_ecgm_port_profile_info_t * data)
{
    int tc;

    for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
    {
        dnx_sw_state_print_mem(unit, &(data->unicast_queue[tc]), sizeof(dnx_ecgm_uc_thresholds_t));
    }
    dnx_sw_state_print_mem(unit, &data->unicast_port, sizeof(dnx_ecgm_uc_thresholds_t));

    for (tc = 0; tc < DNX_COSQ_NOF_TC; tc++)
    {
        dnx_sw_state_print_mem(unit, &(data->multicast_queue[tc]), sizeof(dnx_ecgm_mc_queue_resources_t));
    }
    dnx_sw_state_print_mem(unit, &data->multicast_port, sizeof(dnx_ecgm_mc_drop_thresholds_t));

}

/**
 * }
 */
