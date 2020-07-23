/** \file scheduler.c
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

/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/algo/algo_gpm.h>

#include "scheduler_allocators.h"
#include "scheduler_flow_alloc.h"
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/stk/stk_sys.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>

#include "scheduler_region.h"
#include "scheduler_flow.h"
#include "scheduler_port.h"
#include "scheduler.h"

#include <shared/utilex/utilex_u64.h>
/*
 * }
 */

#define DNX_SCH_FLOW_SHAPER_NO_LIMIT 0xffffffff

typedef struct
{
    /**
     * flow id for all subflows of current flow
     * index 0 - main flow id
     * index 1 - composite subflow (in case the flow is composite 
     */
    int subflow_id[2];
    /**
     * 1 - regular flow
     * 2 - composite flow
     */
    int nof_subflows;
    /**
     * the index of current flow (either 0 or 1)
     */
    int subflow_index;
} dnx_scheduler_flow_info_t;

const uint32 dnx_cosq_gport_scheduler_hr_mask = (BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ |
                                                 BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ |
                                                 BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED);

const uint32 dnx_cosq_gport_scheduler_cl_mask = (BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP |
                                                 BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE2_3SP_WFQ |
                                                 BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE3_WFQ_2SP |
                                                 BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ |
                                                 BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ);

const uint32 dnx_cosq_gport_scheduler_cl_enhanced_mask = (BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_LOWPRI_FQ |
                                                          BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_HIGHPRI_FQ);

const uint32 dnx_cosq_gport_scheduler_cl_wfq_mask = (BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_DISCREET |
                                                     BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_CLASS |
                                                     BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT |
                                                     BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT_PROPORTIONAL);
static int dnx_scheduler_port_hr_configure(
    int unit,
    int port);

static int dnx_scheduler_port_hr_remove(
    int unit,
    int port);

static shr_error_e dnx_scheduler_port_qpair_to_hr_map_set(
    int unit,
    bcm_port_t qpair_port,
    bcm_port_t hr_port);

static shr_error_e dnx_scheduler_port_qpair_to_hr_map_clear(
    int unit,
    bcm_port_t port);

static shr_error_e dnx_scheduler_element_cl_params_get(
    int unit,
    int core,
    int flow_id,
    dnx_sch_alloc_flow_type_e flow_type,
    bcm_cosq_scheduler_gport_params_t * params);

static shr_error_e dnx_scheduler_element_fq_params_get(
    int unit,
    int core,
    dnx_sch_alloc_flow_type_e flow_type,
    bcm_cosq_scheduler_gport_params_t * params);

/**
 * \brief -
 * Calculate number of remote cores and 
 * actual flow id for each of the remote core
 */
shr_error_e
dnx_scheduler_flow_id_for_remote_core_get(
    int unit,
    int core,
    int base_flow_id,
    int actual_flow_id[],
    int *nof_remote_cores)
{
    int region, i;
    int is_se;

    SHR_FUNC_INIT_VARS(unit);

    /** is SE */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, base_flow_id, &is_se));

    if (is_se)
    {
        actual_flow_id[0] = base_flow_id;
        *nof_remote_cores = 1;
    }
    else
    {
        region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
        *nof_remote_cores = dnx_data_sch.flow.nof_remote_cores_get(unit, core, region)->val;

        for (i = 0; i < *nof_remote_cores; i++)
        {
            actual_flow_id[i] = DNX_SCH_FLOW_ID_FROM_REMOTE_CORE(base_flow_id, *nof_remote_cores, i);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int dnx_sch_hw_init(
    int unit);

/*
 * See .h file
 */
shr_error_e
dnx_sch_port_add_sch(
    int unit,
    bcm_port_t port)
{

    bcm_port_t master_port;
    dnx_algo_port_info_s port_info;
    int port_has_speed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        /** configure HR for the port */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_configure(unit, port));

        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &port_has_speed));
        if (port_has_speed)
        {
            /*
             * Color port HRs only if the port has speed already
             * If the port has no speed yet, it would be colored during speed_set (resource_set)
             */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_decide_and_set(unit, port));
        }

        /** allocate E2E interface */
        SHR_IF_ERR_EXIT(dnx_scheduler_interface_allocate(unit, port));

        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_SCH, &master_port));

        /** configure priority propagation priority */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_propagation_priorities_nof_set(unit, port));

        if (port == master_port)
        {
            /** configure interface priority */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_interface_priority_set(unit, port));

            if (DNX_ALGO_PORT_TYPE_IS_SCH_ONLY(unit, port_info))
            {
                /** disable interface level flow control to LAG scheduler */
                SHR_IF_ERR_EXIT(dnx_scheduler_port_egq_to_sch_if_fc_disable(unit, port));
            }

            SHR_IF_ERR_EXIT(dnx_scheduler_port_interface_enable_set(unit, port, TRUE));
        }

        /** map port to interface */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_to_interface_map_set(unit, port));

        /** set port priority */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_priorities_nof_set(unit, port));

        /** set default TC to TCG mapping -- all mapped to tcg 0 */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_tc_to_tcg_map_init(unit, port));

        /** initialize port TC and TCG shapers to maximal values */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_shapers_bandwidth_init(unit, port));

    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_sch_port_add_handle(
    int unit)
{

    bcm_port_t port, master_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** get logical port which is about to be added */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_sch_port_add_sch(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info) && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        /** map q-pairs to HRs */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_map_set(unit, port, port));

        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_SCH, &master_port));

        if (port == master_port)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_port_egq_to_sch_if_map(unit, port));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_sch_port_remove_sch(
    int unit,
    bcm_port_t port)
{

    bcm_port_t next_master_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        /** remove is_colored marks */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_unset(unit, port));

        /** unmap port to interface */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_to_interface_unmap_set(unit, port));

        /** Release the HR */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_remove(unit, port));

        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_SCH,
                                                 &next_master_port));

        if (next_master_port == DNX_ALGO_PORT_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_port_interface_enable_set(unit, port, FALSE));

            SHR_IF_ERR_EXIT(dnx_scheduler_port_interface_priority_clear(unit, port));

            /** deallocate E2E interface */
            SHR_IF_ERR_EXIT(dnx_scheduler_interface_deallocate(unit, port));

            if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_scheduler_port_egq_to_sch_if_unmap(unit, port));
            }
        }

        /** clear port priority */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_priorities_nof_clear(unit, port));

        /** clear nof priorities for priority_propagation */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_propagation_priorities_nof_clear(unit, port));

        /** set default TC to TCG mapping -- all mapped to tcg 0 */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_tc_to_tcg_map_init(unit, port));

        /** initialize port TC and TCG max burst to default values */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_shapers_max_burst_reset(unit, port));

        /** initialize port TC and TCG bandwidth to default values */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_shapers_bandwidth_reset(unit, port));
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_sch_port_remove_handle(
    int unit)
{

    bcm_port_t port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** get logical port which is about to be added */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_sch_port_remove_sch(unit, port));
    }

    /** port is about to be removed -- clear mapping q-pairs to HRs */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_map_clear(unit, port));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Allocate reserved connectors.
 *          These connectors are used to mark diconnected VOQs.
 */
static shr_error_e
dnx_sch_reserve_connectors_alloc(
    int unit)
{
    int reserved_flow_id = DNX_COSQ_RESERVED_FLOW_START;
    int region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(reserved_flow_id);
    int core, nof_remote_cores;
    int src_modid = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {

       /** Allocate reserved connectors - used to mark the VOQ is not connected */
        nof_remote_cores = dnx_data_sch.flow.nof_remote_cores_get(unit, core, region_index)->val;

       /** This function is not invoked under warm boot */
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_allocate(unit, core, nof_remote_cores, DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                    FALSE, FALSE, DNX_COSQ_RESERVED_FLOW_NOF,
                                                    DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR, src_modid, &reserved_flow_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate and configure reserved SE
 *          This SE is used to mark unattached flow
 */
static shr_error_e
dnx_sch_reserve_se_set(
    int unit)
{
    int core;
    int default_se_flow_id, default_se_id;

    bcm_gport_t sched_gport;
    bcm_cosq_scheduler_gport_params_t params;

    SHR_FUNC_INIT_VARS(unit);

    /** obtain default_se flow id */
    default_se_id = dnx_data_sch.flow.default_credit_source_se_id_get(unit);
    SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, default_se_id, &default_se_flow_id));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** additional SE is allocated to hookup objects just created */
        params.core = core;
        params.type = bcmCosqSchedulerGportTypeFq;
        params.mode = bcmCosqSchedulerGportModeSimple;
        params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;

        BCM_GPORT_SCHEDULER_CORE_SET(sched_gport, default_se_flow_id, core);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_scheduler_gport_add(unit, BCM_COSQ_SCHEDULER_GPORT_WITH_ID, &params,
                                                         &sched_gport));

        SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, sched_gport, 0, bcmCosqControlBandwidthBurstMax, 0));

        SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_set(unit, core, default_se_flow_id,
                                                    dnx_data_sch.flow.hr_se_id_min_get(unit), BCM_COSQ_SP0, 0));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - update number of tokens on reserved SE
 *          to make sure no credits are issued to unattached flows
 *          following soft reset
 */
shr_error_e
dnx_sch_reserve_se_update(
    int unit)
{
    int core;
    int default_se_flow_id[1], default_se_id;

    SHR_FUNC_INIT_VARS(unit);

    /** obtain default_se flow id */
    default_se_id = dnx_data_sch.flow.default_credit_source_se_id_get(unit);
    SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, default_se_id, &default_se_flow_id[0]));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_token_count_set(unit, core, default_se_flow_id, 1, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * returns TRUE iff flow_id is used as FMQ root
 */
static shr_error_e
dnx_scheduler_is_flow_fmq_class(
    int unit,
    int core,
    int flow_id,
    int *is_fmq_class,
    int *fmq_class_indx)
{
    int temp_fmq_class_indx;
    int fmq_flow_id;

    SHR_FUNC_INIT_VARS(unit);

    *is_fmq_class = FALSE;
    *fmq_class_indx = -1;

    for (temp_fmq_class_indx = 0; temp_fmq_class_indx < dnx_data_sch.general.nof_fmq_class_get(unit);
         temp_fmq_class_indx++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.fmq_flow_id.get(unit, core, temp_fmq_class_indx, &fmq_flow_id));

        if (flow_id == fmq_flow_id)
        {
            *is_fmq_class = TRUE;
            *fmq_class_indx = temp_fmq_class_indx;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * returns TRUE iff flow_id is reserved element
 */
shr_error_e
dnx_scheduler_flow_is_reserved_element(
    int unit,
    int core,
    int flow_id,
    int *is_reserved)
{
    int is_se;
    int reserved_hr_se_id, nof_reserved_erp_hr, reserved_erp_hr_flow_id;
    int se_id, hr_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    *is_reserved = FALSE;

    /** is SE */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, flow_id, &is_se));

    if (is_se)
    {

        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

        if (se_id == dnx_data_sch.flow.default_credit_source_se_id_get(unit))
        {
            *is_reserved = TRUE;
        }
        else
        {

            hr_id = dnx_data_sch.flow.reserved_hr_get(unit);
            if (hr_id != -1)
            {
                reserved_hr_se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, dnx_data_sch.flow.reserved_hr_get(unit));
                if (se_id == reserved_hr_se_id)
                {
                    *is_reserved = TRUE;
                }
            }
        }

        if (*is_reserved == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_db.general.nof_reserved_erp_hr.get(unit, core, &nof_reserved_erp_hr));

            for (i = 0; i < nof_reserved_erp_hr; i++)
            {
                SHR_IF_ERR_EXIT(dnx_scheduler_db.general.reserved_erp_hr_flow_id.get(unit, core, i,
                                                                                     &reserved_erp_hr_flow_id));
                if (reserved_erp_hr_flow_id == flow_id)
                {
                    *is_reserved = TRUE;
                    break;
                }
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * return default values of CL descrete weights
 * the default value of weight i is 2^i 
 * (1 2 4 8)
 */
static shr_error_e
dnx_sched_cl_default_weights_get(
    int unit,
    dbal_enum_value_field_cl_mode_e cl_mode,
    dbal_enum_value_field_wfq_weight_mode_e wfq_mode,
    dnx_sch_cl_weight_t * cl_weight)
{
    int i;
    int first_wfq_hw_index;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(cl_weight, 0, (sizeof(dnx_sch_cl_weight_t)));

    SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_wfq_indx_to_hw(unit, cl_mode, 0, &first_wfq_hw_index));

    if (wfq_mode == DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW ||
        wfq_mode == DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS)
    {
        for (i = 0; i < dnx_sch_cl_info[cl_mode].nof_descrete_priority; i++)
        {
            cl_weight->weight[first_wfq_hw_index + i] = 1 << i; /** 2^i */
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate and configure CL class profile
 */
static int
dnx_sch_cl_profile_alloc(
    int unit,
    int core,
    dbal_enum_value_field_cl_mode_e cl_type,
    dbal_enum_value_field_cl_enhanced_mode_e enhanced_mode,
    dbal_enum_value_field_wfq_weight_mode_e wfq_mode,
    int *profile_id)
{

    dnx_sch_cl_info_t cl_template_data;

    uint8 is_alloc;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&cl_template_data, 0, sizeof(dnx_sch_cl_info_t));

    /** Get default weights */
    SHR_IF_ERR_EXIT(dnx_sched_cl_default_weights_get(unit, cl_type, wfq_mode, &cl_template_data.weight));

    cl_template_data.config.cl_mode = cl_type;
    cl_template_data.config.wfq_weight_mode = wfq_mode;
    cl_template_data.config.enhanced_mode = enhanced_mode;

     /** allocate profile */
    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_cl_profile_alloc(unit, core, 0, &cl_template_data, NULL,
                                                              profile_id, &is_alloc));

    if (is_alloc)
    {
        /** write to HW */
        SHR_IF_ERR_EXIT(dnx_sch_cl_config_set(unit, core, *profile_id, &cl_template_data.config));

        SHR_IF_ERR_EXIT(dnx_sch_cl_weight_set(unit, core, *profile_id, &cl_template_data.weight));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 * initialize cl profiles.
 */
static int
dnx_sch_cl_profile_init(
    int unit)
{
    int core;

    dbal_enum_value_field_cl_mode_e cl_type;
    dbal_enum_value_field_cl_enhanced_mode_e enhanced_mode;
    dbal_enum_value_field_wfq_weight_mode_e wfq_mode;

    int template_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialization of default templates.
     * Default templates are allocated thus giving them a reference count of one. 
     * This will ensure (without any additional checks) that the 
     * default templates never get freed.
     */

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (cl_type = DBAL_ENUM_FVAL_CL_MODE_1; cl_type <= DBAL_ENUM_FVAL_CL_MODE_5; cl_type++)
        {
            for (wfq_mode = DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW;
                 wfq_mode < DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INVALID; wfq_mode++)
            {

                if (wfq_mode == DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW
                    && cl_type < DBAL_ENUM_FVAL_CL_MODE_3)
                {
                    /** Independent mode is relevant for CL mode 3 and up */
                    continue;
                }
                if (wfq_mode == DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS && cl_type < DBAL_ENUM_FVAL_CL_MODE_2)
                {
                    /** Descrete-per-class mode is relevant for CL mode 2 and up */
                    continue;
                }

                for (enhanced_mode = DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED;
                     enhanced_mode < DBAL_NOF_ENUM_CL_ENHANCED_MODE_VALUES; enhanced_mode++)
                {
                    SHR_IF_ERR_EXIT(dnx_sch_cl_profile_alloc(unit, core, cl_type, enhanced_mode, wfq_mode,
                                                             &template_id));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate and configure reserved HR
 *          This HR is used to disable returned credits in LAG SCH
 */
static shr_error_e
dnx_sch_reserve_hr_set(
    int unit)
{
    int core;
    int hr;
    int se_id;
    int flow_id;
    bcm_gport_t sched_gport;
    bcm_cosq_scheduler_gport_params_t params;

    SHR_FUNC_INIT_VARS(unit);

    /** get reserved hr-id*/
    hr = dnx_data_sch.flow.reserved_hr_get(unit);
    if (hr != -1)
    {
        /** hr to se_id */
        se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, hr);
        /** se_id to flow id */
        SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));

        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            params.core = core;
            params.type = bcmCosqSchedulerGportTypeHrSingleWfq;
            params.mode = bcmCosqSchedulerGportModeSimple;
            params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;

            BCM_GPORT_SCHEDULER_CORE_SET(sched_gport, flow_id, core);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_scheduler_gport_add(unit, BCM_COSQ_SCHEDULER_GPORT_WITH_ID, &params,
                                                             &sched_gport));

            SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, sched_gport, 0, bcmCosqControlBandwidthBurstMax, 0));

        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate and configure reserved HR
 *          This HR is used to disable returned credits in LAG SCH
 */
static shr_error_e
dnx_sch_erp_hrs_handle(
    int unit)
{
    int core;
    int hr;
    int se_id;
    int flow_id;
    bcm_gport_t sched_gport;
    bcm_cosq_scheduler_gport_params_t params;
    int i, swtate_index;
    const dnx_data_port_static_add_erp_exist_t *erp_exists;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        erp_exists = dnx_data_port.static_add.erp_exist_get(unit, core);

        if (erp_exists->exist)
        {
            /** get reserved hr-id*/
            hr = dnx_data_sch.flow.erp_hr_get(unit);

            for (i = 0; i < dnx_data_sch.ps.nof_hrs_in_ps_get(unit); i++)
            {

                /** hr to se_id */
                se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, hr + i);
                /** se_id to flow id */
                SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));

                params.core = core;
                params.type = bcmCosqSchedulerGportTypeHrSingleWfq;
                params.mode = bcmCosqSchedulerGportModeSimple;
                params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;

                BCM_GPORT_SCHEDULER_CORE_SET(sched_gport, flow_id, core);
                SHR_IF_ERR_EXIT(bcm_dnx_cosq_scheduler_gport_add(unit, BCM_COSQ_SCHEDULER_GPORT_WITH_ID, &params,
                                                                 &sched_gport));
                /** store the ERP HRs in swstate in a reverse order */
                swtate_index = dnx_data_sch.ps.nof_hrs_in_ps_get(unit) - i - 1;
                SHR_IF_ERR_EXIT(dnx_scheduler_db.general.reserved_erp_hr_flow_id.set(unit, core, swtate_index,
                                                                                     flow_id));
            }

            SHR_IF_ERR_EXIT(dnx_scheduler_db.general.nof_reserved_erp_hr.set(unit, core,
                                                                             dnx_data_sch.ps.nof_hrs_in_ps_get(unit)));
        }
        else
        {
            /*
             * if no ERP port exists,
             * need to clear ERP FC enablers in order to be able to use ERP HRs for other ports
             */
            SHR_IF_ERR_EXIT(dnx_sch_erp_enable_set(unit, core, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * initialized swstate of scheduler module
 */
static shr_error_e
dnx_sch_swstate_init(
    int unit)
{

    int core;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Alloc SW state 
     */
    SHR_IF_ERR_EXIT(dnx_scheduler_db.init(unit));
    SHR_IF_ERR_EXIT(cosq_config.init(unit));

    SHR_IF_ERR_EXIT(dnx_scheduler_db.port.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_scheduler_db.general.alloc(unit));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.alloc_bitmap(unit, core));

        SHR_IF_ERR_EXIT(cosq_config.hr_group_bw.alloc(unit, core));

        SHR_IF_ERR_EXIT(cosq_config.flow.alloc(unit, core));

        SHR_IF_ERR_EXIT(cosq_config.connector.alloc(unit, core));

        SHR_IF_ERR_EXIT(cosq_config.se.alloc(unit, core));

        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.reserved_hr_flow_id.alloc(unit, core));
        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.fmq_flow_id.alloc(unit, core));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_sch_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /** init E2E sch allocation manager */
    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_init(unit));

    /*
     * Alloc SW state 
     */
    SHR_IF_ERR_EXIT(dnx_sch_swstate_init(unit));

    /*
     * Init HW
     */
    SHR_IF_ERR_EXIT(dnx_sch_hw_init(unit));

    /*
     * Init CL class profiles
     */
    SHR_IF_ERR_EXIT(dnx_sch_cl_profile_init(unit));

    /*
     * Allocate and configure reserved SE, which is used as parent of unattached flows
     */
    SHR_IF_ERR_EXIT(dnx_sch_reserve_se_set(unit));

    /*
     * Allocate and reserve HR, used for returned credits in LAG SCH
     */
    SHR_IF_ERR_EXIT(dnx_sch_reserve_hr_set(unit));

    /*
     * Reserve ERP HRs if ERP exists. Otherwise, allow to use ERP HRs for other ports.
     */
    SHR_IF_ERR_EXIT(dnx_sch_erp_hrs_handle(unit));

    /*
     * Allocate reserved connectors, which are used to mark invalid VOQ connection
     */
    SHR_IF_ERR_EXIT(dnx_sch_reserve_connectors_alloc(unit));

    if (dnx_data_sch.features.feature_get(unit, dnx_data_sch_features_dlm))
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_dlm_init(unit));
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_sch_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * exit:
     */
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_sch_port_interface_ports_color(
    int unit,
    bcm_port_t port)
{
    bcm_pbmp_t ports_pbmp;
    bcm_port_t curr_port;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get all ports of the same interface */
    flags = DNX_ALGO_PORT_CHANNELS_F_NON_L1_ONLY;
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, flags, &ports_pbmp));

    BCM_PBMP_ITER(ports_pbmp, curr_port)
    {
        /*
         * Color port HRs only if the port has speed already
         * If the port has no speed yet, it would be colored during speed_set (resource_set)
         */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_decide_and_set(unit, curr_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - enable scheduler elements according to region definition 
 */
static shr_error_e
dnx_sch_flow_sched_enable_init(
    int unit)
{

    int core, region_indx, i;
    int se_id_start, se_id_end, nof_se_in_quartet;
    int region_size = dnx_data_sch.flow.region_size_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (region_indx = dnx_data_sch.flow.nof_connectors_only_regions_get(unit);
             region_indx < dnx_data_sch.flow.nof_regions_get(unit); region_indx++)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_region_nof_se_in_quartet_get(unit, core, region_indx, &nof_se_in_quartet));

            for (i = 0; i < nof_se_in_quartet; i++)
            {
                /*
                 * Configure scheduling elements at offset #i in the quartet for all quartets in the region
                 */
                SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, region_indx * region_size + i, &se_id_start));
                SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit,
                                                              (region_indx + 1) * region_size - 4 + i, &se_id_end));
                SHR_IF_ERR_EXIT(dnx_sch_se_is_scheduler_set_range(unit, core, se_id_start, se_id_end, 1));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - HW init for Scheduler module
 */
static shr_error_e
dnx_sch_hw_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_calendar_init(unit));

    /** configure regions info */
    SHR_IF_ERR_EXIT(dnx_scheduler_region_info_config(unit));

    /*
     * Overwrite registers default values 
     */
    SHR_IF_ERR_EXIT(dnx_sch_overwrite_slow_rate_set(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_sch_dlm_reset_trigger_set(unit));

    SHR_IF_ERR_EXIT(dnx_sch_dlm_enable_all_set(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_sch_backup_msg_enable_set(unit, TRUE));

    SHR_IF_ERR_EXIT(dnx_sch_dbal_qpm_override_set(unit, FALSE));

    if (dnx_data_sch.general.port_priority_propagation_enable_get(unit))
    {
        /** enable port priority propagation */
        SHR_IF_ERR_EXIT(dnx_sch_dbal_priority_propagation_enable_set(unit, TRUE));
        /** Set device scheduler to work in strict priority mode */
        SHR_IF_ERR_EXIT(dnx_sch_dbal_device_scheduler_config_set(unit, 1, 0));
    }

    /** enable scheduler elements according to region definition */
    SHR_IF_ERR_EXIT(dnx_sch_flow_sched_enable_init(unit));

    /** configure dummy TM port and map it to reserved HR*/
    if (dnx_data_port.general.reserved_tm_port_get(unit) != -1)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_dummy_tm_port_set(unit, dnx_data_port.general.reserved_tm_port_get(unit)));
        SHR_IF_ERR_EXIT(dnx_sch_dbal_port_to_base_hr_mapping_set(unit, BCM_CORE_ALL,
                                                                 dnx_data_port.general.reserved_tm_port_get(unit),
                                                                 dnx_data_sch.flow.reserved_hr_get(unit)));
    }

    /** enable calrx and rxi blocks */
    if (dnx_data_sch.general.feature_get(unit, dnx_data_sch_general_calrx_rxi_enable_supported))
    {
        SHR_IF_ERR_EXIT(dnx_sch_calrx_rxi_enable_set(unit, TRUE));
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * Support Functions - Flow 
 */

/*
 * \brief - returns flow_id of connector num. cos
 * in case of composite connector, returns flow_id of the second subflow as well
 */
shr_error_e
dnx_scheduler_connector_flow_id_calc(
    int unit,
    int core,
    int base_flow_id,
    int cos,
    int is_non_contiguous,
    int is_composite,
    int called_by_voq_remove,
    int flow[2],
    int *nof_subflows)
{
    int flow_region;
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;
    int factor = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (core == BCM_CORE_ALL)
    {
        /*
         * Get operation for all cores: Read from core '0'.
         */
        core = 0;
    }
    else
    {
        if ((core < 0 || core >= dnx_data_device.general.nof_cores_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid core ID(%d) for getting flow id\n", core);
        }
    }

    flow_region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
    SHR_IF_ERR_EXIT(dnx_scheduler_region_odd_even_mode_get(unit, core, flow_region, &odd_even_mode));

    if (called_by_voq_remove)
    {
        flow[0] = DNX_COSQ_RESERVED_FLOW_START;
        flow[1] = (is_composite ? DNX_COSQ_RESERVED_FLOW_START : -1);
    }
    else
    {

        if (is_non_contiguous && is_composite && odd_even_mode == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "composite connectors are not supported on EVEN region\n");
        }

        if (is_composite)
        {
            /** how many IDs to skip to get the next ID */
            factor *= 2;

            if (is_non_contiguous)
            {
                factor *= 2;
            }
        }

        if (is_non_contiguous && !is_composite)
        {
            /*
             * For simple connector in interdigitated region
             * 0 -> base_flow_id
             * 1 -> base_flow_id + 1
             * 2 -> base_flow_id + 4 (2/2 *4)
             * 3 -> base_flow_id + 4 (3/2 * 4) + 1
             * 
             * the count is per pair (cos /2); each pair takes 4 flow IDs (cos/2 *4)
             * inside the pair, the index is determined based on the LSB (cos % 2)
             */
            flow[0] = base_flow_id + (cos / 2 * 4) + (cos % 2);
        }
        else
        {
            flow[0] = base_flow_id + cos * factor;
        }

        /** composite connectors on even region do not exist */
        flow[1] = (is_composite ? (flow[0] + 1) : -1);
    }

    *nof_subflows = (is_composite ? 2 : 1);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - verify parameters of connector_gport_add
 */
static shr_error_e
dnx_scheduler_connector_gport_add_verify(
    int unit,
    int core,
    int src_modid,
    uint32 nof_remote_cores,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport)
{
    uint32 allowed_flags =
        BCM_COSQ_VOQ_CONNECTOR_GPORT_F_COMPOSITE | BCM_COSQ_VOQ_CONNECTOR_GPORT_F_NON_CONTIGUOUS |
        BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE | BCM_COSQ_VOQ_CONNECTOR_GPORT_F_WITH_ID |
        BCM_COSQ_VOQ_CONNECTOR_GPORT_F_RESERVED;

    SHR_FUNC_INIT_VARS(unit);

    SHR_MASK_VERIFY(flags, allowed_flags, _SHR_E_PARAM, "provided unsupported flags\n");

    SHR_RANGE_VERIFY(numq, 0, DNX_COSQ_MAX_COS, _SHR_E_PARAM, "Invalid cos levels specified %d\n", numq);

    if ((numq % DNX_COSQ_MIN_COS) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "number cos levels (%d) should be multiplier of %d\n", numq, DNX_COSQ_MIN_COS);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - obtain modid from provided gport
 */
static shr_error_e
dnx_scheduler_gport_modid_get(
    int unit,
    bcm_gport_t gport,
    int *modid)
{
    bcm_gport_t modport_gport;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SYSTEM_PORT(gport))
    {
        /** convert sysport to modport */
        SHR_IF_ERR_EXIT(bcm_dnx_stk_sysport_gport_get(unit, gport, &modport_gport));
    }
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        modport_gport = gport;
    }
    else
    {
        /*
         * try local or local gport
         * if not appropriate gport - exit with error
         */
        SHR_IF_ERR_EXIT(bcm_dnx_port_gport_get(unit, gport, &modport_gport));

    }

    *modid = BCM_GPORT_MODPORT_MODID_GET(modport_gport);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - main function implementing bcm_cosq_voq_connector_gport_add API
 */
shr_error_e
dnx_scheduler_connector_gport_add(
    int unit,
    int core,
    int src_modid,
    uint32 nof_remote_cores,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport)
{
    int is_non_contiguous;
    int is_composite;
    int is_low_rate;
    int flow_id = -1;
    int res_flags = 0;
    int cur_flow_subflow[2], cur_connector, flow_id_i;
    int i;
    dnx_sch_alloc_flow_type_e flow_type;

    int subflow_indx;
    int nof_subflows;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_connector_gport_add_verify(unit, core, src_modid, nof_remote_cores,
                                                                   port, numq, flags, gport));

    if (core == -1)
    {
        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, *gport, &core));
    }

    is_non_contiguous = (flags & BCM_COSQ_VOQ_CONNECTOR_GPORT_F_NON_CONTIGUOUS) ? TRUE : FALSE;
    is_composite = (flags & BCM_COSQ_VOQ_CONNECTOR_GPORT_F_COMPOSITE) ? TRUE : FALSE;
    is_low_rate = (flags & BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE) ? TRUE : FALSE;

    /** validate flow (if specified by user) */
    if (flags & BCM_COSQ_VOQ_CONNECTOR_GPORT_F_WITH_ID)
    {
        res_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;

        if (!BCM_COSQ_GPORT_IS_VOQ_CONNECTOR((*gport)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport (0x%x) specified. Expected VOQ connector gport\n", (*gport));
        }

        flow_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET((*gport));
    }

    /** Get modid of source */
    if (src_modid == -1)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_modid_get(unit, port, &src_modid));
    }

    /** allocate resource */
    flow_type = is_composite ? DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR : DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR;

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_allocate(unit, core, nof_remote_cores, res_flags,
                                                is_non_contiguous, is_low_rate, numq, flow_type, src_modid, &flow_id));

    /** configure resources */
    /** set all symmetric connections */
    if (is_composite)
    {
        /*
         * If the flow is composite - set is composite in HW
         * If the flow is not composite - nothing to be done,
         * the clean is performed in gport_delete
         */
        for (i = 0; i < nof_remote_cores; i++)
        {
            flow_id_i = DNX_SCH_FLOW_ID_FROM_REMOTE_CORE(flow_id, nof_remote_cores, i);

            SHR_IF_ERR_EXIT(dnx_sch_flow_bundle_is_composite_set(unit, core, flow_id_i, numq, TRUE));
        }
    }

    for (cur_connector = 0; cur_connector < numq; cur_connector++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, core, flow_id, cur_connector,
                                                             is_non_contiguous, is_composite, FALSE,
                                                             cur_flow_subflow, &nof_subflows));

        for (subflow_indx = 0; subflow_indx < nof_subflows; subflow_indx++)
        {
            /** set flow weight and mode */
            SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.weight.set(unit, core, cur_flow_subflow[subflow_indx], 0));
            SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.mode.set(unit, core, cur_flow_subflow[subflow_indx],
                                                                 BCM_COSQ_SP0));
        }
    }

    /** data structure for connector group */
    SHR_IF_ERR_EXIT(cosq_config.connector.num_cos.set(unit, core, flow_id, numq));
    SHR_IF_ERR_EXIT(cosq_config.connector.src_modid.set(unit, core, flow_id, src_modid));

    /** update handle */
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET((*gport), flow_id, core);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - verify that all connectors in the bundle are detached
 */
shr_error_e
dnx_scheduler_voq_connector_detached_verify(
    int unit,
    int core,
    int base_flow_id,
    int numq,
    int is_composite,
    int is_non_contiguous)
{
    int cur_connector;
    int nof_subflows;
    int cur_flow_subflow[2];
    int subflow_indx;
    dnx_sch_flow_credit_source_t credit_source;
    uint32 default_se_id;

    SHR_FUNC_INIT_VARS(unit);

    default_se_id = dnx_data_sch.flow.default_credit_source_se_id_get(unit);

    for (cur_connector = 0; cur_connector < numq; cur_connector++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, core, base_flow_id, cur_connector,
                                                             is_non_contiguous, is_composite, FALSE, cur_flow_subflow,
                                                             &nof_subflows));

        for (subflow_indx = 0; subflow_indx < nof_subflows; subflow_indx++)
        {
            /** get flow attributes */

            SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_hw_get(unit, core, cur_flow_subflow[subflow_indx], &credit_source));

            /** check that the flows are detached */
            if (credit_source.se_id != default_se_id)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flow id (%d) is not detached\n", cur_flow_subflow[subflow_indx]);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - main function implementing bcm_cosq_gport_delete for VOQ_CONNECTOR gport
 */
shr_error_e
dnx_scheduler_voq_connector_gport_delete(
    int unit,
    bcm_gport_t gport)
{
    int nof_remote_cores;
    int is_non_contiguous;
    int is_composite;
    int base_flow_id, numq, flow_region;
    int cur_flow_subflow[2], cur_connector;
    int core, i;
    uint8 sw_state_num_cos;
    dnx_sch_connector_swstate_info_t zero = { 0 };
    dnx_sch_credit_src_info_t zero_credit_src = { 0 };
    dnx_sch_alloc_flow_type_e flow_type;

    int nof_subflows, subflow_indx;
    int actual_flow_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];

    SHR_FUNC_INIT_VARS(unit);

    /** get core id */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, gport, &core));

    /** determine flow id */
    base_flow_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_base_flow_id_cosq_verify(unit, core, base_flow_id, 0, TRUE));

    /** retrieve connector group state */
    SHR_IF_ERR_EXIT(cosq_config.connector.num_cos.get(unit, core, base_flow_id, &sw_state_num_cos));
    numq = sw_state_num_cos;

    /** determine flow attributes */
    flow_region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, core, flow_region, &is_non_contiguous));

    /** determine is composite flow attributes */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, core, base_flow_id, &is_composite));

    /** verify all connectors are detached */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_voq_connector_detached_verify(unit, core, base_flow_id, numq,
                                                                      is_composite, is_non_contiguous));

    /** delete all flows */
    if (is_composite)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core, base_flow_id,
                                                                  actual_flow_id, &nof_remote_cores));

        /** set all symmetric connections */
        for (i = 0; i < nof_remote_cores; i++)
        {
            /*
             * If the flow is composite - clear the composite status
             * If the flow is not composite - nothing to be done,
             *
             * Note: the composite is set on the whole bundle at once.
             */
            SHR_IF_ERR_EXIT(dnx_sch_flow_bundle_is_composite_set(unit, core, actual_flow_id[i], numq, FALSE));
        }
    }

    for (cur_connector = 0; cur_connector < numq; cur_connector++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, core, base_flow_id, cur_connector,
                                                             is_non_contiguous, is_composite, FALSE, cur_flow_subflow,
                                                             &nof_subflows));

        for (subflow_indx = 0; subflow_indx < nof_subflows; subflow_indx++)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core, cur_flow_subflow[subflow_indx],
                                                                      actual_flow_id, &nof_remote_cores));

            /** set all symmetric connections */
            for (i = 0; i < nof_remote_cores; i++)
            {
                /** update flow attributes with default values */
                SHR_IF_ERR_EXIT(dnx_scheduler_flow_default_attributes_set(unit, core, actual_flow_id[i]));
            }
        }
    }

    /** free resources */
    flow_type = (is_composite ? DNX_SCH_ALLOC_FLOW_TYPE_COMP_CONNECTOR : DNX_SCH_ALLOC_FLOW_TYPE_CONNECTOR);

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_deallocate(unit, core, numq, flow_type, base_flow_id));

    /** free flow resources */
    for (cur_connector = 0; cur_connector < numq; cur_connector++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, core, base_flow_id, cur_connector,
                                                             is_non_contiguous, is_composite, FALSE, cur_flow_subflow,
                                                             &nof_subflows));

        for (subflow_indx = 0; subflow_indx < nof_subflows; subflow_indx++)
        {
            SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.set(unit, core, cur_flow_subflow[subflow_indx],
                                                            &zero_credit_src));
        }
    }

    /** free connector group resources */
    SHR_IF_ERR_EXIT(cosq_config.connector.set(unit, core, base_flow_id, &zero));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h
 */
shr_error_e
dnx_scheduler_fmq_hr_gport_fmq_class_get(
    int unit,
    bcm_gport_t fmq_hr_gport,
    int *fmq_class)
{
    int sched_id;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_FMQ_CLASS(fmq_hr_gport))
    {
        sched_id = BCM_GPORT_SCHEDULER_GET(fmq_hr_gport);

        /*
         * _SHR_GPORT_FMQ_CLASS1 is the biggest value
         * and other others values are  less by 1 than the previous value
         */
        *fmq_class = _SHR_GPORT_FMQ_CLASS1 - sched_id;
        /**
         * FMQ Classes are 1 based
         */
        *fmq_class += 1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport (0x%x) is not FMQ HR Gport\n", fmq_hr_gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - get real scheduler gport
 * mainly used to replace FMQ gport by actual sched gport
 */
shr_error_e
dnx_scheduler_gport_actual_sched_gport_get(
    int unit,
    bcm_gport_t sched_gport,
    int allow_core_all,
    bcm_gport_t * actual_sched_gport)
{
    int sched_id, core;
    int sw_state_fmq_flow_id;
    int fmq_class, fmq_class_indx;

    SHR_FUNC_INIT_VARS(unit);

    (*actual_sched_gport) = sched_gport;

    if (BCM_GPORT_IS_SCHEDULER(sched_gport))
    {
        sched_id = BCM_GPORT_SCHEDULER_GET(sched_gport);

        if (sched_id >= DNX_SCH_MAX_FLOW_ID(unit))
        {
            uint32 flags = allow_core_all ? 0 : ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL;

            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, flags, sched_gport, &core));

            if (BCM_COSQ_GPORT_IS_FMQ_CLASS(sched_gport))
            {
                SHR_IF_ERR_EXIT(dnx_scheduler_fmq_hr_gport_fmq_class_get(unit, sched_gport, &fmq_class));

                /**
         * FMQ class is 1-based. Convert to 0-based index
         */
                fmq_class_indx = fmq_class - 1;

                SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.fmq_flow_id.get(unit, core, fmq_class_indx,
                                                                     &sw_state_fmq_flow_id));
                if (sw_state_fmq_flow_id == -1)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "FMQ Class-%d resource is not allocated\n", fmq_class);
                }
                BCM_GPORT_SCHEDULER_CORE_SET(*actual_sched_gport, sw_state_fmq_flow_id, core);
            }
            else
            {
                (*actual_sched_gport) = sched_gport;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - verify cosq parameter with respect to the provided gport
 */
static shr_error_e
dnx_scheduler_gport_cosq_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{

    int is_connector = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        is_connector = TRUE;
    }
    else if (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport))
    {
        /** determine if associated with  connector or se */
        is_connector = (_SHR_COSQ_GPORT_RESERVED_TYPE_GET(gport) == _SHR_COSQ_EXT_GPORT_RESERVED_VOQ_CONNECTOR);

    }
    if (is_connector == TRUE)
    {
        if ((cosq < 0) || (cosq > DNX_COSQ_MAX_COS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cos levels %d\n", cosq);
        }
    }
    else
    {
        if ((cosq != 0) && (cosq != -1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cos levels %d\n", cosq);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify port priority parameter with respect to the provided port
 */
static shr_error_e
dnx_scheduler_port_priority_verify(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq)
{
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    SHR_RANGE_VERIFY(cosq, 0, nof_priorities - 1, _SHR_E_PARAM,
                     "cosq (%d) is out of range. (Number of priorities is %d)\n", cosq, nof_priorities);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify port priority parameter with respect to the provided port
 */
static shr_error_e
dnx_scheduler_port_sch_verify(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d does not have scheduler. Scheduler APIs cannot be applied for this port\n",
                     logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify base flow id is allocated 
 * and cosq is consistent with allocation_bundle size
 */
shr_error_e
dnx_scheduler_base_flow_id_cosq_verify(
    int unit,
    int core,
    int base_flow_id,
    uint32 cosq,
    int is_connector)
{
    int is_allocated;
    uint8 sw_state_num_cos;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, base_flow_id, &is_allocated));

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow %d is not allocated\n", base_flow_id);
    }

    if (is_connector)
    {
        if (cosq > 0)
        {
            SHR_IF_ERR_EXIT(cosq_config.connector.num_cos.get(unit, core, base_flow_id, &sw_state_num_cos));
            if (cosq >= sw_state_num_cos)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cosq %d\n", cosq);
            }
        }
        /*
         * we allow API to get direct flow id (as opposite to base flow id + cos)
         * in this case provided cos is 0
         * and no additional verification is required
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - obtain core and base flow id from gport and cosq
 */
shr_error_e
dnx_scheduler_gport_core_base_flow_id_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *core,
    int *base_flow_id)
{
    bcm_port_t logical_port;
    int tmp_core;
    int priority;
    int se_id;
    int is_connector = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get core id */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, gport, core));

    if (BCM_COSQ_GPORT_IS_E2E_PORT(gport) || BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport))
    {
        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

        if (BCM_COSQ_GPORT_IS_E2E_PORT(gport))
        {
            priority = 0;
        }
        else
        {
            /** Port-TC */
            /** validate priority */
            SHR_INVOKE_VERIFY_DNX(dnx_scheduler_port_priority_verify(unit, logical_port, cosq));

            priority = cosq;
        }

        SHR_IF_ERR_EXIT(dnx_sch_convert_port_tc_to_se_id(unit, logical_port, priority, &tmp_core, &se_id));

        SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, base_flow_id));

    }
    else if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        *base_flow_id = BCM_GPORT_SCHEDULER_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_SCHED_CIR(gport))
    {
        *base_flow_id = BCM_COSQ_GPORT_SCHED_CIR_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_SCHED_PIR(gport))
    {
        *base_flow_id = BCM_COSQ_GPORT_SCHED_PIR_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport))
    {
        *base_flow_id = BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_FLOW_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        *base_flow_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport);
        is_connector = 1;
    }
    else if (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport))
    {
        *base_flow_id = BCM_COSQ_GPORT_COMPOSITE_SF2_GET(gport);

        /** determine if associated with  connector or se */
        is_connector = (_SHR_COSQ_GPORT_RESERVED_TYPE_GET(gport) == _SHR_COSQ_EXT_GPORT_RESERVED_VOQ_CONNECTOR);

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unable to retrieve flow_id for gport 0x%x", gport);
    }

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_base_flow_id_cosq_verify(unit, *core, *base_flow_id, cosq, is_connector));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify gport type is consistent with shared shaper mode configured in HW
 */
static shr_error_e
dnx_scheduler_element_shared_shaper_mode_verify(
    int unit,
    bcm_gport_t gport,
    int core,
    int base_flow_id)
{
    dbal_enum_value_field_shared_shaper_mode_e shared_shaper_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_get(unit, core, base_flow_id, &shared_shaper_mode));

    switch (shared_shaper_mode)
    {
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE:
            if (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport) ||
                BCM_COSQ_GPORT_IS_SCHED_CIR(gport) || BCM_COSQ_GPORT_IS_SCHED_PIR(gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport type for single shaper mode gport=0x%x\n", gport);
            }
            break;
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL:
            if (BCM_GPORT_IS_SCHEDULER(gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Scheduler gport can't be use to configure dual shaper gport=0x%x\n", gport);

            }

            if (BCM_COSQ_GPORT_IS_SCHED_CIR(gport) || BCM_COSQ_GPORT_IS_SCHED_PIR(gport))
            {
                if (base_flow_id % 4 != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flow id %d for CIR/PIR gport\n", base_flow_id);
                }
            }
            break;
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD:
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA:
            if (BCM_COSQ_GPORT_IS_SCHED_CIR(gport) || BCM_COSQ_GPORT_IS_SCHED_PIR(gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport type for shared shaper mode=%d gport=0x%x\n",
                             shared_shaper_mode, gport);
            }
            if (BCM_GPORT_IS_SCHEDULER(gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Scheduler gport can't be use to configure shared shaper gport=0x%x\n",
                             gport);

            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected shared shaper mode %d\n", shared_shaper_mode);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get number of shared priorities for provided flow
 */
shr_error_e
dnx_scheduler_element_shared_shaper_nof_priorities_get(
    int unit,
    int core,
    int flow_id,
    int *nof_priorities)
{
    dbal_enum_value_field_shared_shaper_mode_e shared_shaper_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_get(unit, core, flow_id, &shared_shaper_mode));

    switch (shared_shaper_mode)
    {
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE:
            *nof_priorities = 1;
            break;
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL:
            *nof_priorities = 2;
            break;
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD:
            *nof_priorities = 4;
            break;
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA:
            *nof_priorities = 8;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected shared shaper mode %d\n", shared_shaper_mode);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - obtain flow id from gport and cosq
 */
shr_error_e
dnx_scheduler_gport_core_flow_id_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *core,
    int *flow_id)
{
    int base_flow_id;
    int region_index;
    int is_interdigitated;
    int is_connector = FALSE;
    int is_composite;
    int nof_subflows;
    int subflow[2];
    int is_cl_cir;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_base_flow_id_get(unit, gport, cosq, core, &base_flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, *core, base_flow_id, &is_composite));

    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);

    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, *core, region_index, &is_interdigitated));
    is_cl_cir = (dnx_data_sch.flow.region_type_get(unit, *core, region_index)->propagation_direction ==
                 DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_LOW_TO_HIGH);

    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {

        SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, *core, base_flow_id, cosq,
                                                             is_interdigitated, is_composite, FALSE,
                                                             subflow, &nof_subflows));
        *flow_id = subflow[0];

        is_connector = TRUE;
    }
    else if (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport))
    {
        /** determine if associated with  connector or se */
        is_connector = (_SHR_COSQ_GPORT_RESERVED_TYPE_GET(gport) == _SHR_COSQ_EXT_GPORT_RESERVED_VOQ_CONNECTOR);

        if (is_connector)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, *core, base_flow_id, cosq,
                                                                 is_interdigitated, is_composite, FALSE,
                                                                 subflow, &nof_subflows));
            *flow_id = subflow[1];
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_sch_flow_composite_id_get(unit, *core, base_flow_id, flow_id));
        }
    }
    else if ((is_cl_cir && BCM_COSQ_GPORT_IS_SCHED_PIR(gport)) || (!is_cl_cir && BCM_COSQ_GPORT_IS_SCHED_CIR(gport)))
    {
        *flow_id = base_flow_id + 1;
    }
    else
    {
        *flow_id = base_flow_id;
    }

    if (!is_connector)
    {
        /** check shared mode is consistent with gport */

        SHR_INVOKE_VERIFY_DNX(dnx_scheduler_element_shared_shaper_mode_verify(unit, gport, *core, base_flow_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get flow ID for all flow subflows 
 * and subflow index of current subflow
 */
static shr_error_e
dnx_scheduler_gport_flow_info_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *core,
    dnx_scheduler_flow_info_t * flow_info)
{
    int base_flow_id;
    int region_index;
    int is_interdigitated;
    int is_connector;
    int is_composite;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_base_flow_id_get(unit, gport, cosq, core, &base_flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, *core, base_flow_id, &is_composite));

    region_index = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);

    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, *core, region_index, &is_interdigitated));

    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {

        SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, *core, base_flow_id, cosq,
                                                             is_interdigitated, is_composite, FALSE,
                                                             flow_info->subflow_id, &flow_info->nof_subflows));
        flow_info->subflow_index = 0;
    }
    else if (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport))
    {
        /** determine if associated with  connector or se */
        is_connector = (_SHR_COSQ_GPORT_RESERVED_TYPE_GET(gport) == _SHR_COSQ_EXT_GPORT_RESERVED_VOQ_CONNECTOR);

        if (is_connector)
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_connector_flow_id_calc(unit, *core, base_flow_id, cosq,
                                                                 is_interdigitated, is_composite, FALSE,
                                                                 flow_info->subflow_id, &flow_info->nof_subflows));
            flow_info->subflow_index = 1;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "SE gport in not supported by this function\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "gport 0x%x in not supported by this function\n", gport);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify child flow of cosq_gport_attach
 */
static shr_error_e
dnx_scheduler_gport_child_flow_verify(
    int unit,
    bcm_gport_t child_port,
    bcm_cos_queue_t cosq)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify child port is of expected type */
    if (!(BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(child_port) || BCM_COSQ_GPORT_IS_COMPOSITE_SF2(child_port) ||
          BCM_GPORT_IS_SCHEDULER(child_port) || BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(child_port) ||
          BCM_COSQ_GPORT_IS_SCHED_CIR(child_port) || BCM_COSQ_GPORT_IS_SCHED_PIR(child_port)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unexpected child gport type, 0x%x", child_port);
    }

    /** Verify cosq  */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_cosq_verify(unit, child_port, cosq));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify parent flow of cosq_gport_attach
 */
static shr_error_e
dnx_scheduler_gport_parent_flow_verify(
    int unit,
    bcm_gport_t parent_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify parent port is of expected type */
    if (!(BCM_COSQ_GPORT_IS_E2E_PORT(parent_port) || BCM_GPORT_IS_SCHEDULER(parent_port) ||
          BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(parent_port) ||
          BCM_COSQ_GPORT_IS_SCHED_CIR(parent_port) || BCM_COSQ_GPORT_IS_SCHED_PIR(parent_port) ||
          BCM_COSQ_GPORT_IS_E2E_PORT_TC(parent_port)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unexpected parent gport type, 0x%x", parent_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify bcm_cosq_gport_attach parameters
 */
static shr_error_e
dnx_scheduler_gport_attach_verify(
    int unit,
    bcm_gport_t parent_port,
    bcm_gport_t child_port,
    bcm_cos_queue_t cosq)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify child port */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_child_flow_verify(unit, child_port, cosq));

    /** Verify parent port is of expected type */
    if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(parent_port))
    {
        /** E2E_PORT_TC is not supported in gport_attach */
        SHR_ERR_EXIT(_SHR_E_PORT, "Unexpected parent gport type, 0x%x", parent_port);
    }
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_parent_flow_verify(unit, parent_port));

    /** Validate parent and child elements are not equal. In that case we cant attach parent to input when parent = input */
    if (parent_port == child_port)
    {

        SHR_ERR_EXIT(_SHR_E_PARAM, "attach cant be done when parent 0x%x is equal to input port 0x%x\n", parent_port,
                     child_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify bcm_cosq_gport_attach configuration
 */
static shr_error_e
dnx_scheduler_gport_attach_verify_config(
    int unit,
    int child_core_id,
    int child_flow_id,
    int is_child_se,
    int parent_core_id,
    int parent_flow_id,
    int is_parent_se)
{
    int is_hr_colored;
    int hr_id, is_port_hr;
    int parent_se_id;

    SHR_FUNC_INIT_VARS(unit);

    /** make sure both are on the same core */
    if (child_core_id != parent_core_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parent flow and child flow are not on the same core (%d vs. %d)\n",
                     parent_core_id, child_core_id);

    }

    /** make sure the parent is a SE */
    if (!is_parent_se)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parent flow %d is not SE\n", parent_flow_id);
    }

    /** get parent SE id */
    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, parent_flow_id, &parent_se_id));

    /** is parent HR */
    if (is_child_se && DNX_SCHEDULER_ELEMENT_IS_HR(unit, parent_se_id))
    {

        hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, parent_se_id);

        SHR_IF_ERR_EXIT(dnx_sch_hr_is_port_get(unit, parent_core_id, hr_id, &is_port_hr));

        if (is_port_hr)
        {

            /** check that HR is colored */
            SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.get(unit, parent_core_id, hr_id, &is_hr_colored));

            /** issue an error if not colored */
            if (!is_hr_colored)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Cannot attach an element (flow id %d) to HR before setting speed on the interface\n",
                             child_flow_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - attach flow to a parent
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] sched_port - parent flow gport
 *   \param [in] input_port - child flow gport
 *   \param [in] cosq - child flow cosq
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * The mode and weight of connection to the parent are determined by bcm_cosq_gport_sched_set
 * \see
 *   * None
 */
shr_error_e
dnx_scheduler_gport_attach(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    bcm_gport_t parent_port;
    int core, child_core;
    int sw_state_default_se;
    int child_flow_id, parent_flow_id;
    int is_child_se, is_parent_se;
    int parent_se_id, prev_parent_se_id;
    dnx_sch_credit_src_info_t swstate_credit_src_info;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if parent port refers to a pre-allocated resource */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_actual_sched_gport_get(unit, sched_port, 0, &parent_port));

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_attach_verify(unit, parent_port, input_port, cosq));

    /** get child flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, input_port, cosq, &child_core, &child_flow_id));

    /** get parent flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, parent_port, -1, &core, &parent_flow_id));

    /** is parent SE */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, parent_flow_id, &is_parent_se));

    /** is child SE */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, child_flow_id, &is_child_se));

    /** Verify configuration */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_attach_verify_config(unit, child_core, child_flow_id, is_child_se,
                                                                   core, parent_flow_id, is_parent_se));

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, parent_flow_id, &parent_se_id));

    sw_state_default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);

    /** get previous parent */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_se_id_get(unit, core, child_flow_id, &prev_parent_se_id));

    /** decrement reference count of previous parent */
    if (prev_parent_se_id != sw_state_default_se)
    {
        SHR_IF_ERR_EXIT(cosq_config.se.child_count.dec(unit, core, prev_parent_se_id, 1));
    }

    /** get child attach information */
    SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.get(unit, core, child_flow_id, &swstate_credit_src_info));

    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_set(unit, core, child_flow_id, parent_se_id,
                                                swstate_credit_src_info.mode, swstate_credit_src_info.weight));

    if (is_child_se)
    {
        if (!BCM_COSQ_GPORT_IS_COMPOSITE_SF2(input_port))
        {
            /*
             * coloring is done when attaching base flow for both base flow and composite flow
             */
            SHR_IF_ERR_EXIT(dnx_scheduler_element_color_group_decide_and_set(unit, core, child_flow_id, parent_se_id));

        }
    }

    /** increment reference count of parent */
    if (parent_se_id != sw_state_default_se)
    {
        SHR_IF_ERR_EXIT(cosq_config.se.child_count.inc(unit, core, parent_se_id, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify bcm_cosq_gport_detach configuration
 */
static shr_error_e
dnx_scheduler_gport_detach_verify_config(
    int unit,
    int child_core,
    int child_flow_id,
    int parent_core,
    int parent_flow_id)
{

    int parent_se_id, prev_parent_se_id;
    uint32 sw_state_default_se;
    uint32 sw_state_ref_cnt;
    int is_parent_se;

    SHR_FUNC_INIT_VARS(unit);

    /** make sure both are on the same core */
    if (child_core != parent_core)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parent flow and child flow are not on the same core (%d vs. %d)\n",
                     parent_core, child_core);

    }
    /** is parent SE */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, parent_core, parent_flow_id, &is_parent_se));

    /** make sure the parent is a SE */
    if (!is_parent_se)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "parent flow %d is not SE\n", parent_flow_id);
    }

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, parent_flow_id, &parent_se_id));

    /** get previous parent */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_se_id_get(unit, child_core, child_flow_id, &prev_parent_se_id));

    /*
     * consistency checks
     * verify child is attached to parent scheduler
     */
    if (parent_se_id != prev_parent_se_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid parameters: child flow %d is not attached to parent %d\n", child_flow_id, parent_se_id);
    }

    /** if already detached return error */
    sw_state_default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);
    if (parent_se_id == sw_state_default_se)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameters: child flow %d is already detached\n", child_flow_id);
    }

    SHR_IF_ERR_EXIT(cosq_config.se.child_count.get(unit, parent_core, parent_se_id, &sw_state_ref_cnt));

    if (sw_state_ref_cnt <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid parameters: parent SE (%d) reference count is 0\n", parent_se_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - detach flow from a parent
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] sched_port - parent flow gport
 *   \param [in] input_port - child flow gport
 *   \param [in] cosq - child flow cosq
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * 
 * \see
 *   * None
 */
shr_error_e
dnx_scheduler_gport_detach(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    bcm_gport_t parent_port;
    int core, child_core;
    uint32 sw_state_default_se;
    int child_flow_id, parent_flow_id;
    int parent_se_id;

    SHR_FUNC_INIT_VARS(unit);

    /** determine if parent port refers to a pre-allocated resource */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_actual_sched_gport_get(unit, sched_port, 0, &parent_port));

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_attach_verify(unit, parent_port, input_port, cosq));

    /** get child flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, input_port, cosq, &child_core, &child_flow_id));

    /** get parent flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, parent_port, -1, &core, &parent_flow_id));

    /** Verify configuration */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_detach_verify_config(unit, child_core, child_flow_id,
                                                                   core, parent_flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, parent_flow_id, &parent_se_id));

    sw_state_default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);

    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_set(unit, core, child_flow_id, sw_state_default_se, BCM_COSQ_SP0, 0));

    /** decrement reference count of parent */
    SHR_IF_ERR_EXIT(cosq_config.se.child_count.dec(unit, core, parent_se_id, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get gport for flow
 */
static shr_error_e
dnx_scheduler_flow_gport_get(
    int unit,
    int core,
    int flow_id,
    bcm_gport_t * gport)
{
    dbal_enum_value_field_shared_shaper_mode_e shared_shaper_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_get(unit, core, flow_id, &shared_shaper_mode));

    if (shared_shaper_mode == DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE)
    {
        /*
         * return scheduling element gport
         */
        BCM_GPORT_SCHEDULER_CORE_SET((*gport), flow_id, core);
    }
    else
    {
        /*
         * return shared shaper element gport
         */
        BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_SET((*gport), flow_id, core);

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get a parent of the flow
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit
 *   \param [in] child_port - child flow gport
 *   \param [in] cos - child flow cosq
 *   \param [out] parent_port - parent flow gport
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \remark
 *   * 
 * \see
 *   * None
 */
shr_error_e
dnx_scheduler_gport_parent_get(
    int unit,
    bcm_gport_t child_port,
    bcm_cos_queue_t cos,
    bcm_gport_t * parent_port)
{
    int tc = 0;
    int core;
    dnx_sch_flow_credit_source_t dbal_credit_source;
    int child_flow_id;
    bcm_port_t logical_port;
    int parent_se_id, parent_flow_id;
    uint32 sw_state_default_se;
    int is_fmq_class, fmq_class_indx;
    int is_child_se, child_se_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify child port */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_child_flow_verify(unit, child_port, cos));

    /** get child flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, child_port, cos, &core, &child_flow_id));

    /** Retrieve hierarchy parent */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_hw_get(unit, core, child_flow_id, &dbal_credit_source));

    parent_se_id = dbal_credit_source.se_id;

    /** if a parent is default se, the flow is not attached to anyone */
    sw_state_default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);
    if (parent_se_id == sw_state_default_se)
    {

        /** FMQ root */
        SHR_IF_ERR_EXIT(dnx_scheduler_is_flow_fmq_class(unit, core, child_flow_id, &is_fmq_class, &fmq_class_indx));
        if (is_fmq_class)
        {
            /*
             * FMQ classes are 1-based
             */
            BCM_COSQ_GPORT_FMQ_CLASS_CORE_SET(*parent_port, core, fmq_class_indx + 1);
        }
        else
        {
            /** is child SE */
            SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, child_flow_id, &is_child_se));

            if (is_child_se)
            {
                SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, child_flow_id, &child_se_id));

                /** Get logical port for port HR parent */
                SHR_IF_ERR_EXIT(dnx_sch_convert_se_id_to_port_tc(unit, core, child_se_id, &logical_port, &tc));

                if (logical_port != DNX_ALGO_PORT_INVALID)
                {
                    /*
                     * return E2E gport
                     */
                    BCM_COSQ_GPORT_E2E_PORT_SET(*parent_port, logical_port);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "flow %d is not attached\n", child_flow_id);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flow %d is not attached\n", child_flow_id);
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, parent_se_id, &parent_flow_id));

        SHR_IF_ERR_EXIT(dnx_scheduler_flow_gport_get(unit, core, parent_flow_id, parent_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set mode and weight of attaching a flow to its parent
 */
shr_error_e
dnx_scheduler_gport_e2e_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int core;
    int sw_state_default_se;
    int parent_se_id;
    int child_flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** get child flow id and core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, gport, cosq, &core, &child_flow_id));

    SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.weight.set(unit, core, child_flow_id, weight));
    SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.mode.set(unit, core, child_flow_id, mode));

    sw_state_default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);

    /** get parent */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_se_id_get(unit, core, child_flow_id, &parent_se_id));

    if (parent_se_id != sw_state_default_se)
    {
        bcm_gport_t parent_gport;

        int parent_flow_id;

        SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, parent_se_id, &parent_flow_id));

        SHR_IF_ERR_EXIT(dnx_scheduler_flow_gport_get(unit, core, parent_flow_id, &parent_gport));

        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_attach(unit, parent_gport, gport, cosq));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get mode and weight of attaching a flow to its parent
 */
int
dnx_scheduler_gport_e2e_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    int core;
    int sw_state_default_se;
    int child_flow_id;
    int parent_se_id;
    dnx_sch_flow_credit_source_cos_data_t credit_source_cos;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify child port */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_child_flow_verify(unit, gport, cosq));

    /** get child flow id and core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, gport, cosq, &core, &child_flow_id));

    /** get default parent */
    sw_state_default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);

    /** get  parent */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_se_id_get(unit, core, child_flow_id, &parent_se_id));

    if (parent_se_id == sw_state_default_se)
    {
        /** unattached flow - take mode and weight from swstate */
        SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.weight.get(unit, core, child_flow_id, weight));
        SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.mode.get(unit, core, child_flow_id, mode));

    }
    else
    {
        /** attached flow - read from HW */
        /** get  credit source cos */
        SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_cos_get(unit, core, child_flow_id, &credit_source_cos));
        SHR_IF_ERR_EXIT(dnx_scheduler_element_cos_decode(unit, core, parent_se_id, &credit_source_cos, mode, weight));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify CL weight set/get common parameters
 */
static int
dnx_scheduler_cl_weight_param_verify(
    int unit,
    bcm_gport_t port,
    int cosq)
{
    SHR_FUNC_INIT_VARS(unit);

    /** validate port element */
    if (!(BCM_GPORT_IS_SCHEDULER(port) || BCM_COSQ_GPORT_IS_SCHED_CIR(port) || BCM_COSQ_GPORT_IS_SCHED_PIR(port)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport 0x%x\n", port);
    }

    if ((cosq != 0) && (cosq != -1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cosq %d", cosq);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify CL weight set parameters
 */
static int
dnx_scheduler_cl_weight_set_param_verify(
    int unit,
    bcm_gport_t port,
    int cosq,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_cl_weight_param_verify(unit, port, cosq));

    if (weight < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Weight must be positive %d\n", weight);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify that flow is CL
 */
static int
dnx_scheduler_flow_is_cl_verify(
    int unit,
    int core,
    int flow_id)
{
    int is_se;
    int se_id;

    SHR_FUNC_INIT_VARS(unit);

    /** is SE */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, flow_id, &is_se));
    if (!is_se)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "scheduling element is expected (flow_id %d)\n", flow_id);
    }

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    if (!DNX_SCHEDULER_ELEMENT_IS_CL(unit, se_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "CL scheduling element is expected (flow_id %d)\n", flow_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify CL weight set weight parameters
 */
static int
dnx_scheduler_cl_weight_set_weight_verify(
    int unit,
    int core,
    int cl_profile,
    int weight_indx)
{
    int ref_count;
    dnx_sch_cl_info_t cl_template_data;

    SHR_FUNC_INIT_VARS(unit);

    /** get template data from Template manager */
    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_cl_profile_data_get(unit, core, cl_profile,
                                                                 &ref_count, &cl_template_data));

    if (cl_template_data.config.wfq_weight_mode != DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW &&
        cl_template_data.config.wfq_weight_mode != DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid weight mode 0x%x", cl_template_data.config.wfq_weight_mode);
    }

    if (weight_indx >= dnx_sch_cl_info[cl_template_data.config.cl_mode].nof_descrete_priority)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid weight index 0x%x", weight_indx);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Configure CL weight (per profile)
 * Allocate new profile if required
 */
static int
dnx_scheduler_cl_weight_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int weight)
{
    int weight_ap, hw_indx;

    int se_id, cl_id;
    int cl_profile, new_profile;
    int flags = 0;

    uint8 is_last = 0;
    uint8 is_allocated = 0;

    int core;
    int flow_id;
    int ref_count;
    dnx_sch_cl_info_t cl_template_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_cl_weight_set_param_verify(unit, port, cosq, weight));

    /** get flow id and core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, -1, &core, &flow_id));

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_flow_is_cl_verify(unit, core, flow_id));

    /*
     * this function is invoked for the following types only:
     * - bcmCosqControlDiscreteWeightLevel0
     * - bcmCosqControlDiscreteWeightLevel1
     * - bcmCosqControlDiscreteWeightLevel2
     * - bcmCosqControlDiscreteWeightLevel3
     */

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));
    cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, se_id);

    /** get the profile from HW */
    SHR_IF_ERR_EXIT(dnx_sch_cl_mapping_get(unit, core, cl_id, &cl_profile));

    weight_ap = type - bcmCosqControlDiscreteWeightLevel0;

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_cl_weight_set_weight_verify(unit, core, cl_profile, weight_ap));

    /** get template data from Template manager */
    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_cl_profile_data_get(unit, core, cl_profile, &ref_count,
                                                                 &cl_template_data));

    SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_wfq_indx_to_hw(unit, cl_template_data.config.cl_mode, weight_ap,
                                                            &hw_indx));

    /** update the data */
    cl_template_data.weight.weight[hw_indx] = weight;

    SHR_IF_ERR_EXIT(dnx_scheduler_allocators_cl_profile_exchange(unit, core, flags, &cl_template_data, cl_profile, NULL,
                                                                 &new_profile, &is_allocated, &is_last));

    /** if required update HW with template */
    if (is_allocated == TRUE)
    {
        /** update class information in HW */
        SHR_IF_ERR_EXIT(dnx_sch_cl_config_set(unit, core, new_profile, &cl_template_data.config));

        SHR_IF_ERR_EXIT(dnx_sch_cl_weight_set(unit, core, new_profile, &cl_template_data.weight));

    }

    /** set the new profile to HW */
    SHR_IF_ERR_EXIT(dnx_sch_cl_mapping_set(unit, core, cl_id, new_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get CL weight
 */
static int
dnx_scheduler_cl_weight_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *weight)
{
    int weight_ap, hw_indx;
    int core;
    int flow_id;
    dnx_sch_cl_weight_t cl_weight;

    int se_id, cl_id, cl_profile;
    dnx_sch_cl_config_t cl_config;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameter */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_cl_weight_param_verify(unit, port, cosq));

    (*weight) = 0;

    /** get flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, -1, &core, &flow_id));

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_flow_is_cl_verify(unit, core, flow_id));

    /*
     * this function is invoked for the following types only:
     * - bcmCosqControlDiscreteWeightLevel0
     * - bcmCosqControlDiscreteWeightLevel1
     * - bcmCosqControlDiscreteWeightLevel2
     * - bcmCosqControlDiscreteWeightLevel3
     */
    weight_ap = type - bcmCosqControlDiscreteWeightLevel0;

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));
    cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, se_id);

    /** get the profile from HW */
    SHR_IF_ERR_EXIT(dnx_sch_cl_mapping_get(unit, core, cl_id, &cl_profile));

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_cl_weight_set_weight_verify(unit, core, cl_profile, weight_ap));

    SHR_IF_ERR_EXIT(dnx_sch_cl_config_get(unit, core, cl_profile, &cl_config));
    /** get the weights from HW */
    SHR_IF_ERR_EXIT(dnx_sch_cl_weight_get(unit, core, cl_profile, &cl_weight));

    SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_wfq_indx_to_hw(unit, cl_config.cl_mode, weight_ap, &hw_indx));

    *weight = cl_weight.weight[hw_indx];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify voq connector gport
 */
static int
dnx_scheduler_voq_connector_gport_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!(BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(port) || BCM_COSQ_GPORT_IS_COMPOSITE_SF2(port)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported gport 0x%x\n", port);
    }

    if ((cosq < 0) || (cosq > DNX_COSQ_MAX_COS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cos levels %d", cosq);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * verify parameters of slow_profile_set
 */
static int
dnx_scheduler_slow_profile_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int slow_rate_id)
{
    int token_factor;
    int flow_id;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_voq_connector_gport_verify(unit, port, cosq));

    /** validate slow rate profile id */
    SHR_RANGE_VERIFY(slow_rate_id, 0, dnx_data_sch.general.nof_slow_profiles_get(unit), _SHR_E_PARAM,
                     "Invalid arg %d\n", slow_rate_id);

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_low_rate_factor_get(unit, core, flow_id, &token_factor));
    SHR_VAL_VERIFY(token_factor, 1, _SHR_E_PARAM, "Cannot set slow profile on low rate connector\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * set slow profile (A or B) for the flow
 */
static int
dnx_scheduler_flow_slow_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int slow_rate_id)
{
    int slow_rate_index, is_slow_enabled;
    int core;
    int nof_remote_cores, i, subflow_index;

    dnx_scheduler_flow_info_t flow_info;

    int actual_flow_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_slow_profile_set_verify(unit, port, cosq, slow_rate_id));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_flow_info_get(unit, port, cosq, &core, &flow_info));

    is_slow_enabled = (slow_rate_id == 0 ? FALSE : TRUE);

    slow_rate_index = (is_slow_enabled ? slow_rate_id - 1 : 0);

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core, flow_info.subflow_id[flow_info.subflow_index],
                                                              actual_flow_id, &nof_remote_cores));

    for (i = 0; i < nof_remote_cores; i++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_slow_profile_set(unit, core, actual_flow_id[i], slow_rate_index));
    }

    /** is_slow_enable should be set for both subflows of composite flow */
    for (subflow_index = 0; subflow_index < flow_info.nof_subflows; subflow_index++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core, flow_info.subflow_id[subflow_index],
                                                                  actual_flow_id, &nof_remote_cores));

        for (i = 0; i < nof_remote_cores; i++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_flow_is_slow_enable_set(unit, core, actual_flow_id[i], is_slow_enabled));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * get slow profile (A or B) of the flow
 */
static int
dnx_scheduler_flow_slow_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int *slow_rate_id)
{
    int is_slow_enabled;
    int slow_rate_index;
    int core;
    int flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_voq_connector_gport_verify(unit, port, cosq));

    /** get flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_slow_enable_get(unit, core, flow_id, &is_slow_enabled));

    if (is_slow_enabled)
    {
        SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_slow_profile_get(unit, core, flow_id, &slow_rate_index));
        *slow_rate_id = slow_rate_index + 1;
    }
    else
    {
        *slow_rate_id = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set is_remote property of the flow
 */
static int
dnx_scheduler_flow_is_remote_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int is_remote)
{
    int core;
    int flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_voq_connector_gport_verify(unit, port, cosq));

    /** get core id */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, port, &core));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_remote_hw_set(unit, core, flow_id, is_remote));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get is_remote property of the flow
 */
static int
dnx_scheduler_flow_is_remote_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int *is_remote)
{
    int core;
    int flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_voq_connector_gport_verify(unit, port, cosq));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_remote_hw_get(unit, core, flow_id, is_remote));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set is_high_priority property of the flow
 */
static int
dnx_scheduler_flow_is_high_priority_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int is_high_priority)
{
    int core;
    int nof_remote_cores, i;

    int flow_id;
    int actual_flow_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_voq_connector_gport_verify(unit, port, cosq));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core, flow_id, actual_flow_id, &nof_remote_cores));

    for (i = 0; i < nof_remote_cores; i++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_flow_is_high_priority_hw_set(unit, core, actual_flow_id[i], is_high_priority));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get is_high_priority property of the flow
 */
static int
dnx_scheduler_flow_is_high_priority_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int *is_high_priority)
{
    int core;
    int flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_voq_connector_gport_verify(unit, port, cosq));

    /** get flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_high_priority_hw_get(unit, core, flow_id, is_high_priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify dnx_scheduler_flow_max_burst_set parameters
 */
static int
dnx_scheduler_flow_max_burst_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int max_burst)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_cosq_verify(unit, port, cosq));

    if (max_burst < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max burst value %d is invalid\n", max_burst);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set max burst of the flow
 * max burst is specified in bytes
 */
static int
dnx_scheduler_flow_max_burst_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int max_burst)
{
    int core;

    int flow_id;

    uint32 credit_worth;
    uint32 max_burst_in_credit_worth;
    int token_factor;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_flow_max_burst_set_verify(unit, port, cosq, max_burst));

    /** get core id */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, port, &core));

    /** get credit worth */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    /** get flow id and core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_low_rate_factor_get(unit, core, flow_id, &token_factor));

    max_burst_in_credit_worth = UTILEX_DIV_ROUND_UP(max_burst * token_factor, credit_worth);

    /** 2 flows per remote core are written inside this function for VOQ connector */
    SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_max_burst_set(unit, core, flow_id, max_burst_in_credit_worth));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get max burst of the flow
 * max burst is returned in bytes
 */
static int
dnx_scheduler_flow_max_burst_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int *max_burst)
{
    int core;

    int flow_id;

    uint32 credit_worth;
    uint32 max_burst_in_credit_worth;
    int token_factor;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_cosq_verify(unit, port, cosq));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    /** get credit worth */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    SHR_IF_ERR_EXIT(dnx_sch_flow_low_rate_factor_get(unit, core, flow_id, &token_factor));

    SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_max_burst_get(unit, core, flow_id, &max_burst_in_credit_worth));

    *max_burst = max_burst_in_credit_worth * credit_worth / token_factor;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify gport and cosq for se group set/get.
 */
static shr_error_e
dnx_scheduler_group_se_gport_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify gport is of SE type */
    if (!(BCM_GPORT_IS_SCHEDULER(gport) ||
          BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport) || BCM_COSQ_GPORT_IS_SCHED_CIR(gport) ||
          BCM_COSQ_GPORT_IS_SCHED_PIR(gport)))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unexpected gport type, 0x%x", gport);
    }

    /** Verify cosq  */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_cosq_verify(unit, gport, cosq));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify all arguments for se group set.
 */
static shr_error_e
dnx_scheduler_se_group_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int group)
{
    int core, flow_id, parent_se_id;
    int sw_state_default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify gport and cosq
     */
    SHR_IF_ERR_EXIT(dnx_scheduler_group_se_gport_verify(unit, port, cosq));

    /*
     * verify group
     */
    SHR_RANGE_VERIFY(group, 0, dnx_data_sch.se.nof_color_group_get(unit), _SHR_E_PARAM, "Invalid Group %d", group);

    /*
     * get flow id + core
     */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    /*
     * verify element is attached
     */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_source_se_id_get(unit, core, flow_id, &parent_se_id));
    if (parent_se_id == sw_state_default_se)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Element 0x%x is not attached", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set group of the flow
 */
static int
dnx_scheduler_se_group_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int group)
{
    int core;
    int flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_se_group_set_verify(unit, port, cosq, group));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    /** set color group for SE */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_color_group_set(unit, core, flow_id, group));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set group of the flow
 */
static int
dnx_scheduler_se_group_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    int *group)
{
    int core;
    int flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_group_se_gport_verify(unit, port, cosq));

    /** get flow id + core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, port, cosq, &core, &flow_id));

    /** get color group for SE */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_color_group_get(unit, core, flow_id, group));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * implementation of generic API function used for configuring various cosq controls
 * for scheduler gports: 
 * - SCHEDULER
 * - VOQ_CONNECTOR
 * - SCHED_CIR
 * - SCHED_PIR
 * - COMPOSITE_SF2
 */
int
dnx_scheduler_cosq_control_flow_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * this function is invoked for the following gport types only:
     * - SCHEDULER
     * - VOQ_CONNECTOR
     * - SCHED_CIR
     * - SCHED_PIR
     * - COMPOSITE_SF2
     */

    switch (type)
    {
        /** Set is_high priority flow attribute  */
        case bcmCosqControlIsHighPriority:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_high_priority_set(unit, port, cosq, arg));
            break;

        /** Set is_fabric flow attribute  */
        case bcmCosqControlIsFabric:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_remote_set(unit, port, cosq, arg));
            break;

        /** Set burst max for flow shaper */
        case bcmCosqControlBandwidthBurstMax:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_max_burst_set(unit, port, cosq, arg));
            break;

        /** set slow profile for flow */
        case bcmCosqControlFlowSlowRate:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_slow_profile_set(unit, port, cosq, arg));
            break;

        /** set descrete weight of CL */
        case bcmCosqControlDiscreteWeightLevel0:
        case bcmCosqControlDiscreteWeightLevel1:
        case bcmCosqControlDiscreteWeightLevel2:
        case bcmCosqControlDiscreteWeightLevel3:
            SHR_IF_ERR_EXIT(dnx_scheduler_cl_weight_set(unit, port, cosq, type, arg));
            break;

        case bcmCosqControlGroup:
            SHR_IF_ERR_EXIT(dnx_scheduler_se_group_set(unit, port, cosq, arg));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * implementation of generic API function used for obtaining various cosq controls
 * for scheduler gports: 
 * - SCHEDULER
 * - VOQ_CONNECTOR
 * - SCHED_CIR
 * - SCHED_PIR
 * - COMPOSITE_SF2
 */
int
dnx_scheduler_cosq_control_flow_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * this function is invoked for the following gport types only:
     * - SCHEDULER
     * - VOQ_CONNECTOR
     * - SCHED_CIR
     * - SCHED_PIR
     * - COMPOSITE_SF2
     */

    switch (type)
    {
        case bcmCosqControlIsHighPriority:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_high_priority_get(unit, port, cosq, arg));
            break;

        /** is_fabric flow attribute  */
        case bcmCosqControlIsFabric:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_remote_get(unit, port, cosq, arg));
            break;

        case bcmCosqControlBandwidthBurstMax:
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_max_burst_get(unit, port, cosq, arg));
            break;

        case bcmCosqControlFlowSlowRate:
            /** VOQs connector slow rate*/
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_slow_profile_get(unit, port, cosq, arg));
            break;

        case bcmCosqControlDiscreteWeightLevel0:
        case bcmCosqControlDiscreteWeightLevel1:
        case bcmCosqControlDiscreteWeightLevel2:
        case bcmCosqControlDiscreteWeightLevel3:
            SHR_IF_ERR_EXIT(dnx_scheduler_cl_weight_get(unit, port, cosq, type, arg));
            break;

        case bcmCosqControlSpLevelMax:
        case bcmCosqControlSp0WeightMax:
        case bcmCosqControlSp1WeightMax:
        case bcmCosqControlSp2WeightMax:
        case bcmCosqControlSp3WeightMax:
        case bcmCosqControlSp4WeightMax:
        case bcmCosqControlSp5WeightMax:
        case bcmCosqControlSp6WeightMax:
        case bcmCosqControlSp7WeightMax:
        case bcmCosqControlSp8WeightMax:
        case bcmCosqControlSp9WeightMax:
        case bcmCosqControlSp10WeightMax:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type %d is not supported on this device\n", type);
            break;

        case bcmCosqControlGroup:
            SHR_IF_ERR_EXIT(dnx_scheduler_se_group_get(unit, port, cosq, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Returns TRUE if numerator1/denominator1
 * is bigger then  numerator2/denominator2
 */
static void
dnx_scheduler_fraction_is_bigger(
    uint32 numerator1,
    uint32 denominator1,
    uint32 numerator2,
    uint32 denominator2,
    int *is_bigger)
{
    UTILEX_U64 val64_1, val64_2;

    /*
     * This function compares numerator1/denominator1 vs. numerator2/denominator2.
     * Instead of performing division, which losses accuracy, compare
     * numerator1*denominator2 vs. numerator2*denominator1
     */

    /** numerator1*denominator2 */
    utilex_u64_multiply_longs(numerator1, denominator2, &val64_1);

    /** numerator1*denominator2 */
    utilex_u64_multiply_longs(numerator2, denominator1, &val64_2);

    *is_bigger = utilex_u64_is_bigger(&val64_1, &val64_2);
}

/**
 * \brief -
 * calculate fractional matissa exponent representation of rate, 
 * provided in Kbits/sec.
 * \note -
 * dbal table passed as parameters to this function is used to obtain 
 * mantissa/exponent size; it is not used to access the HW
 */
static shr_error_e
dnx_scheduler_shaper_rate_to_mant_exp(
    int unit,
    dbal_tables_e table_id,
    uint32 rate_in_kbits_per_sec,
    uint32 *rate_man,
    uint32 *rate_exp,
    uint32 round_up)
{
    uint32 rate_in_mega_bits_per_sec, max_rate_in_mega_bits_per_sec;

    uint32 shaper_weight_numerator, shaper_weight_denominator;
    uint32 exact_shaper_weight, exact_shaper_weight_denominator;
    uint32 credit_worth;
    uint32 peak_rate_man, peak_rate_exp;

    UTILEX_U64 intermediate_numerator;

    int mantissa_field_size, exp_field_size;
    uint32 mantissa_max_value, exp_max_value;
    int is_bigger;
    int nof_flipflops;
    uint32 set_bits;
    uint64 u64_calc;

    SHR_FUNC_INIT_VARS(unit);

    /** get mantissa field size */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, DBAL_FIELD_RATE_MAN, 0, 0, 0, &mantissa_field_size));

    /** get mantissa maximal value */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          table_id,
                                                          DBAL_FIELD_RATE_MAN, 0, 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE, &mantissa_max_value));
    /** get exponent field size */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, DBAL_FIELD_RATE_EXP, 0, 0, 0, &exp_field_size));

    /** get exponent maximal value */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          table_id,
                                                          DBAL_FIELD_RATE_EXP, 0, 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE, &exp_max_value));

    /** get credit worth */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    /*
     * In order to be able to perform calculations in 32 bit,
     * we transform rates to Mega bits per second
     */

    COMPILER_64_ZERO(u64_calc);
    COMPILER_64_SET(u64_calc, 0, (credit_worth * UTILEX_NOF_BITS_IN_BYTE));
    COMPILER_64_UMUL_32(u64_calc, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
    /** Rate should be in [Mbits/sec] */
    COMPILER_64_UDIV_32(u64_calc, (1000 * dnx_data_sch.general.cycles_per_credit_token_get(unit)));
    max_rate_in_mega_bits_per_sec = COMPILER_64_LO(u64_calc);

    if (rate_in_kbits_per_sec == 0 || rate_in_kbits_per_sec == DNX_SCH_FLOW_SHAPER_NO_LIMIT)
    {
        *rate_man = mantissa_max_value;
        *rate_exp = 0;
    }
    else
    {
        rate_in_mega_bits_per_sec = rate_in_kbits_per_sec / 1000;

        shaper_weight_denominator = 64;

        /*
         * shaper_weight_numerator = max_rate_in_mega_bits_per_sec * 64 / rate_in_mega_bits_per_sec, aka
         * shaper_weight_numerator / shaper_weight_denominator = max_rate_in_mega_bits_per_sec / rate_in_mega_bits_per_sec
         *
         * this is the number we calculate fractional mantissa-exponent representation for
         */

        utilex_u64_multiply_longs(max_rate_in_mega_bits_per_sec, shaper_weight_denominator, &intermediate_numerator);
        utilex_u64_devide_u64_long(&intermediate_numerator, rate_in_mega_bits_per_sec, &intermediate_numerator);

        if (intermediate_numerator.arr[1] != 0)
        {
            /** with Jericho 2 typical numbers, we should never get here */

            intermediate_numerator.arr[1] = 0;
            intermediate_numerator.arr[0] = 0xFFFFFFFF;
        }
        shaper_weight_numerator = intermediate_numerator.arr[0];

        SHR_IF_ERR_EXIT(utilex_abs_val_to_mnt_binary_fraction_exp(shaper_weight_numerator,
                                                                  shaper_weight_denominator,
                                                                  mantissa_field_size,
                                                                  exp_field_size,
                                                                  max_rate_in_mega_bits_per_sec,
                                                                  &peak_rate_man, &peak_rate_exp));

        SHR_IF_ERR_EXIT(utilex_mnt_binary_fraction_exp_to_abs_val(mantissa_field_size,
                                                                  exp_field_size,
                                                                  max_rate_in_mega_bits_per_sec,
                                                                  peak_rate_man,
                                                                  peak_rate_exp,
                                                                  &exact_shaper_weight,
                                                                  &exact_shaper_weight_denominator));

        if (round_up)
        {
            dnx_scheduler_fraction_is_bigger(exact_shaper_weight, exact_shaper_weight_denominator,
                                             shaper_weight_numerator, shaper_weight_denominator, &is_bigger);
            if (is_bigger)
            {
                if (peak_rate_man == mantissa_max_value)
                {
                    if (peak_rate_exp > 0)
                    {
                        peak_rate_man = UTILEX_DIV_ROUND_UP(peak_rate_man, 2);
                        peak_rate_exp--;
                    }
                }
                else
                {
                    peak_rate_man++;
                }
            }
        }

        /*
         * Just in case - shouldn't get here
         */
        peak_rate_man = UTILEX_RANGE_BOUND(peak_rate_man, 0, mantissa_max_value);
        peak_rate_exp = UTILEX_RANGE_BOUND(peak_rate_exp, 0, exp_max_value);

        /** if(exp > (20-8)) --> mantissa[exp-(20-8)-1: 0] ==0 */
        nof_flipflops = dnx_data_sch.flow.shaper_flipflops_nof_get(unit);
        while (peak_rate_exp > (nof_flipflops - mantissa_field_size) &&
               (SHR_BITTEST_RANGE(&peak_rate_man, 0, peak_rate_exp - (nof_flipflops - mantissa_field_size), set_bits)))
        {
            peak_rate_man = UTILEX_DIV_ROUND_UP(peak_rate_man, 2);
            peak_rate_exp--;
        }

        *rate_man = peak_rate_man;
        *rate_exp = peak_rate_exp;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * calculate rate from its fractional matissa exponent representation, 
 * the rate is returned in Kbits/sec
 * \note -
 * dbal table passed as parameters to this function is used to obtain 
 * mantissa/exponent size; it is not used to access the HW
 */
static shr_error_e
dnx_scheduler_shaper_mant_exp_to_rate(
    int unit,
    dbal_tables_e table_id,
    uint32 rate_man,
    uint32 rate_exp,
    uint32 *rate_in_kbits_per_sec)
{
    uint32 max_rate_in_mega_bits_per_sec;
    uint32 shaper_weight, shaper_weight_denominator;
    uint32 credit_worth;

    uint64 intermediate_val;

    int mantissa_field_size, exp_field_size;
    uint32 mantissa_max_value;
    uint64 u64_calc;

    SHR_FUNC_INIT_VARS(unit);

    /** get mantissa field size */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, DBAL_FIELD_RATE_MAN, 0, 0, 0, &mantissa_field_size));

    /** get mantissa maximal value */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          table_id,
                                                          DBAL_FIELD_RATE_MAN, 0, 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE, &mantissa_max_value));
    /** get exponent field size */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, DBAL_FIELD_RATE_EXP, 0, 0, 0, &exp_field_size));

    /** get credit worth */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    /*
     * In order to be able to perform calculations in 32 bit,
     * we transform rates to Mega bits per second
     */

    COMPILER_64_ZERO(u64_calc);
    COMPILER_64_SET(u64_calc, 0, (credit_worth * UTILEX_NOF_BITS_IN_BYTE));
    COMPILER_64_UMUL_32(u64_calc, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
    /** Rate should be in [Mbits/sec] */
    COMPILER_64_UDIV_32(u64_calc, (1000 * dnx_data_sch.general.cycles_per_credit_token_get(unit)));
    max_rate_in_mega_bits_per_sec = COMPILER_64_LO(u64_calc);

    if ((rate_man == mantissa_max_value) && (rate_exp == 0))
    {
        *rate_in_kbits_per_sec = DNX_SCH_FLOW_SHAPER_NO_LIMIT;
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_mnt_binary_fraction_exp_to_abs_val(mantissa_field_size,
                                                                  exp_field_size,
                                                                  max_rate_in_mega_bits_per_sec,
                                                                  rate_man,
                                                                  rate_exp,
                                                                  &shaper_weight, &shaper_weight_denominator));
        /*
         * intermediate_value = max_rate_in_kilo_bits_per_sec = max_rate_in_mega_bits_per_sec * 1000;
         */
        COMPILER_64_SET(intermediate_val, 0, max_rate_in_mega_bits_per_sec);
        COMPILER_64_UMUL_32(intermediate_val, 1000);

        if (shaper_weight)
        {
            /*
             * intermediate_value = max_rate_in_kilo_bits_per_sec * shaper_weight_denominator / shaper_weight_numerator, aka
             * shaper_weight_numerator / shaper_weight_denominator = max_rate_in_kilo_bits_per_sec / intermediate_value
             *
             * intermediate_value is the rate represented by mantissa-exponent
             */
            COMPILER_64_UMUL_32(intermediate_val, shaper_weight_denominator);
            COMPILER_64_UDIV_32(intermediate_val, shaper_weight);
        }

        if (COMPILER_64_HI(intermediate_val) != 0)
        {
            *rate_in_kbits_per_sec = 0xFFFFFFFF;
        }
        else
        {
            *rate_in_kbits_per_sec = COMPILER_64_LO(intermediate_val);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get max token bucket values according to slow rate
 */
static shr_error_e
dnx_scheduler_slow_rate_max_bucket_get(
    int unit,
    int slow_rate,
    uint32 *max_bucket)
{

    int index, table_size;
    SHR_FUNC_INIT_VARS(unit);

    table_size = dnx_data_sch.general.slow_rate_max_bucket_info_get(unit)->key_size[0];

    for (index = 0; index < table_size; index++)
    {
        if (index == (table_size - 1))
        {
            /*
             *  On the last index, no need to compare to upper limit (upper limit is maximal speed)
             */
            break;
        }
        else if (slow_rate < (dnx_data_sch.general.slow_rate_max_bucket_get(unit, index + 1)->rate * 1000       /* kbps 
                                                                                                                 */ ))
        {
            /*
             *  Rate range found, stop iteration
             */
            break;
        }
    }

    *max_bucket = dnx_data_sch.general.slow_rate_max_bucket_get(unit, index)->max_bucket;

    SHR_FUNC_EXIT;
}

/**
 * \brief - configure rate of slow profile
 */
shr_error_e
dnx_scheduler_slow_profile_rate_set(
    int unit,
    int core,
    int slow_rate_profile,
    int level,
    int slow_rate)
{

    uint32 slow_rate_mantissa, slow_rate_exp;
    uint32 max_bucket = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** calculate mantissa/exponent representation of slow_rate */
    SHR_IF_ERR_EXIT(dnx_scheduler_shaper_rate_to_mant_exp(unit, DBAL_TABLE_SCH_SLOW_RATE_TABLE,
                                                          slow_rate, &slow_rate_mantissa, &slow_rate_exp, 1));

    /** get max bucket value according to slow rate */
    SHR_IF_ERR_EXIT(dnx_scheduler_slow_rate_max_bucket_get(unit, slow_rate, &max_bucket));

    /** write to HW */
    SHR_IF_ERR_EXIT(dnx_sch_slow_profile_rate_hw_set(unit, core, slow_rate_profile, level,
                                                     slow_rate_mantissa, slow_rate_exp, max_bucket));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get rate of slow profile
 */
shr_error_e
dnx_scheduler_slow_profile_rate_get(
    int unit,
    int core,
    int slow_rate_profile,
    int level,
    int *slow_rate)
{
    uint32 slow_rate_mantissa, slow_rate_exp;
    uint32 max_bucket;

    SHR_FUNC_INIT_VARS(unit);

    /** read HW */
    SHR_IF_ERR_EXIT(dnx_sch_slow_profile_rate_hw_get(unit, core, slow_rate_profile, level,
                                                     &slow_rate_mantissa, &slow_rate_exp, &max_bucket));

    /** transform mantissa/exponent representation to slow_rate */
    SHR_IF_ERR_EXIT(dnx_scheduler_shaper_mant_exp_to_rate(unit, DBAL_TABLE_SCH_SLOW_RATE_TABLE,
                                                          slow_rate_mantissa, slow_rate_exp, (uint32 *) slow_rate));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - 
 * verify flow_bandwidth_set parameters
 */
static shr_error_e
dnx_scheduler_gport_flow_bandwidth_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    if (kbits_sec_min != 0)
    {
        /** no guaranteed rate supported for this type */
        SHR_ERR_EXIT(_SHR_E_PARAM, "guaranteed rate kbits_sec_min is not supported\n");
    }

    /** Verify cosq  */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_cosq_verify(unit, gport, cosq));

    /** verify flags */
    supported_flags = BCM_COSQ_BW_ROUND_TO_CLOSEST;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "flag not supported\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure bandwidth of a flow
 */
shr_error_e
dnx_scheduler_gport_flow_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    int core;
    int i, nof_remote_cores;
    int flow_id, actual_flow_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 peak_rate_mantissa, peak_rate_exp, rd_peak_rate_mantissa, rd_peak_rate_exp;
    uint32 round_up_rate, round_down_rate, round_up_delta, round_down_delta;
    int rate_factor;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_flow_bandwidth_set_verify(unit, gport, cosq,
                                                                        kbits_sec_min, kbits_sec_max, flags));

    /** get flow id and core */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, gport, cosq, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_low_rate_factor_get(unit, core, flow_id, &rate_factor));
    kbits_sec_max *= rate_factor;

    /** calculate mantissa/exponent representation of bandwidth */
    SHR_IF_ERR_EXIT(dnx_scheduler_shaper_rate_to_mant_exp(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE,
                                                          kbits_sec_max, &peak_rate_mantissa, &peak_rate_exp, TRUE));
    if (flags & BCM_COSQ_BW_ROUND_TO_CLOSEST)
    {
        /** calculate rounded down mantissa/exponent representation of bandwidth */
        SHR_IF_ERR_EXIT(dnx_scheduler_shaper_rate_to_mant_exp(unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE,
                                                              kbits_sec_max, &rd_peak_rate_mantissa, &rd_peak_rate_exp,
                                                              FALSE));

        /** Calculate the given rate for each mantissa/exponent representation */
        SHR_IF_ERR_EXIT(dnx_scheduler_shaper_mant_exp_to_rate
                        (unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, peak_rate_mantissa, peak_rate_exp,
                         &round_up_rate));

        SHR_IF_ERR_EXIT(dnx_scheduler_shaper_mant_exp_to_rate
                        (unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, rd_peak_rate_mantissa, rd_peak_rate_exp,
                         &round_down_rate));

        /** calculate the delta from the requested value */
        round_up_delta = utilex_abs(kbits_sec_max - round_up_rate);
        round_down_delta = utilex_abs(kbits_sec_max - round_down_rate);

        if (round_down_delta < round_up_delta)
        {
            /** rounded down values are closer, override the rounded-up mantissa/exponent values */
            peak_rate_mantissa = rd_peak_rate_mantissa;
            peak_rate_exp = rd_peak_rate_exp;
        }
    }

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_id_for_remote_core_get(unit, core, flow_id, actual_flow_id, &nof_remote_cores));

    /** set all symmetric connections */
    for (i = 0; i < nof_remote_cores; i++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_bandwidth_set(unit, core, actual_flow_id[i],
                                                          peak_rate_mantissa, peak_rate_exp));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * verify flow_bandwidth_get parameters
 */
static shr_error_e
dnx_scheduler_gport_flow_bandwidth_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");

    /** Verify cosq  */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_cosq_verify(unit, gport, cosq));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get bandwidth of a flow
 */
shr_error_e
dnx_scheduler_gport_flow_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    int core;
    int flow_id;
    uint32 peak_rate_mantissa, peak_rate_exp;
    int rate_factor;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_flow_bandwidth_get_verify(unit, gport, cosq,
                                                                        kbits_sec_min, kbits_sec_max, flags));

    if (kbits_sec_min != NULL)
    {
        *kbits_sec_min = 0;
    }
    if (flags != NULL)
    {
        *flags = 0;
    }

    /** get flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, gport, cosq, &core, &flow_id));

    /** read HW */
    SHR_IF_ERR_EXIT(dnx_sch_flow_shaper_bandwidth_get(unit, core, flow_id, &peak_rate_mantissa, &peak_rate_exp));

    /** transform mantissa/exponent representation to bandwidth */
    SHR_IF_ERR_EXIT(dnx_scheduler_shaper_mant_exp_to_rate
                    (unit, DBAL_TABLE_SCH_SHAPER_DESCRIPTOR_TABLE, peak_rate_mantissa, peak_rate_exp, kbits_sec_max));

    SHR_IF_ERR_EXIT(dnx_sch_flow_low_rate_factor_get(unit, core, flow_id, &rate_factor));
    *kbits_sec_max /= rate_factor;
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h
 */
shr_error_e
dnx_sch_allocated_hrs_get(
    int unit,
    bcm_core_t core,
    int ps,
    uint32 *allocated_hrs,
    int *priority,
    int *priority_propagation_enable)
{
    int hr, hr_offset;
    int se_id;
    int flow_id;
    int is_allocated;
    int is_port_hr;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /** first, clear bitmap */
    *allocated_hrs = 0;
    *priority = 0;
    *priority_propagation_enable = 0;

    for (hr_offset = 0; hr_offset < dnx_data_sch.ps.nof_hrs_in_ps_get(unit); hr_offset++)
    {
        hr = ps * dnx_data_sch.ps.nof_hrs_in_ps_get(unit) + hr_offset;
        /** hr to se_id */
        se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, hr);
        /** se_id to flow id */
        SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));

        SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, flow_id, &is_allocated));
        if (is_allocated)
        {
            SHR_BITSET(allocated_hrs, hr_offset);

            SHR_IF_ERR_EXIT(dnx_algo_port_hr_is_valid_get(unit, core, hr, &is_port_hr));
            if (is_port_hr)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_hr_to_logical_get(unit, core, hr, &port));
                SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, priority));
                SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port,
                                                                                  priority_propagation_enable));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_scheduler_available_hr_id_find(
    int unit,
    bcm_core_t core,
    int nof_priorities,
    int priority_propagation_enable,
    int *base_hr)
{

    uint32 ps, already_allocated, prio;
    uint32 allocated_hrs[1];
    int used_sch_priority;
    int used_priority_propagation_enable;
    int nof_ps = dnx_data_sch.flow.nof_hr_get(unit) / dnx_data_sch.ps.nof_hrs_in_ps_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    *base_hr = -1;

    /*
     * stage 1 - find allocated PS
     */
    for (ps = 0; ps < nof_ps; ps++)
    {
        SHR_IF_ERR_EXIT(dnx_sch_allocated_hrs_get(unit, core, ps, allocated_hrs,
                                                  &used_sch_priority, &used_priority_propagation_enable));
        if (priority_propagation_enable == used_priority_propagation_enable && used_sch_priority == nof_priorities)
        {
            for (prio = 0; prio < dnx_data_sch.ps.nof_hrs_in_ps_get(unit); prio += nof_priorities)
            {
                SHR_BITTEST_RANGE(allocated_hrs, prio, nof_priorities, already_allocated);

                if (!already_allocated)
                {
                    *base_hr = ps * dnx_data_sch.ps.nof_hrs_in_ps_get(unit) + prio;
                    break;
                }
            }
            if (*base_hr != -1)
            {
                break;
            }
        }
    }
    /*
     * stage 2 - allocate new PS
     */
    if (*base_hr == -1)
    {
        for (ps = 0; ps < nof_ps; ps++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_allocated_hrs_get(unit, core, ps, allocated_hrs,
                                                      &used_sch_priority, &used_priority_propagation_enable));
            if (used_sch_priority == 0)
            {
                for (prio = 0; prio < dnx_data_sch.ps.nof_hrs_in_ps_get(unit); prio += nof_priorities)
                {
                    SHR_BITTEST_RANGE(allocated_hrs, prio, nof_priorities, already_allocated);

                    if (!already_allocated)
                    {
                        *base_hr = ps * dnx_data_sch.ps.nof_hrs_in_ps_get(unit) + prio;
                        break;
                    }
                }
                if (*base_hr != -1)
                {
                    break;
                }
            }
        }
    }
    if (*base_hr == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "no free PS for %d priorities\n", nof_priorities);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h
 */
shr_error_e
dnx_sch_voq_connector_nof_remote_cores_get(
    int unit,
    bcm_core_t core,
    int voq_connector_id,
    int *nof_remote_cores)
{
    uint32 region;

    SHR_FUNC_INIT_VARS(unit);

    region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(voq_connector_id);

    *nof_remote_cores = dnx_data_sch.flow.nof_remote_cores_get(unit, core, region)->val;

    SHR_FUNC_EXIT;
}

/**
 * \brief - return type of scheduling element
 */
shr_error_e
dnx_scheduler_gport_element_type_get(
    int unit,
    bcm_cosq_scheduler_gport_type_t type,
    dnx_sch_element_se_type_e * se_type)
{
    SHR_FUNC_INIT_VARS(unit);

    *se_type = DNX_SCH_ELEMENT_SE_TYPE_NONE;

    switch (type)
    {
        case bcmCosqSchedulerGportTypeClass4Sp:
        case bcmCosqSchedulerGportTypeClass3SpWfq:
        case bcmCosqSchedulerGportTypeClassWfq2Sp:
        case bcmCosqSchedulerGportTypeClass2SpWfq:
        case bcmCosqSchedulerGportTypeClass1SpWfq:
            *se_type = DNX_SCH_ELEMENT_SE_TYPE_CL;
            break;
        case bcmCosqSchedulerGportTypeHrSingleWfq:
        case bcmCosqSchedulerGportTypeHrDualWfq:
        case bcmCosqSchedulerGportTypeHrEnhanced:
            *se_type = DNX_SCH_ELEMENT_SE_TYPE_HR;
            break;
        case bcmCosqSchedulerGportTypeFq:
            *se_type = DNX_SCH_ELEMENT_SE_TYPE_FQ;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return HR mode as defined by scheduler_gport_add parameters
 */
static shr_error_e
dnx_scheduler_element_gport_hr_mode_get(
    int unit,
    bcm_cosq_scheduler_gport_params_t * params,
    dbal_enum_value_field_hr_mode_e * hr_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (params->type)
    {
        case bcmCosqSchedulerGportTypeHrSingleWfq:
            *hr_mode = DBAL_ENUM_FVAL_HR_MODE_SINGLE;
            break;
        case bcmCosqSchedulerGportTypeHrDualWfq:
            *hr_mode = DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ;
            break;
        case bcmCosqSchedulerGportTypeHrEnhanced:
            *hr_mode = DBAL_ENUM_FVAL_HR_MODE_ENHANCED;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type %d cannot be specified for HR element\n", params->type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure HR element
 */
static shr_error_e
dnx_scheduler_element_gport_hr_configure(
    int unit,
    bcm_cosq_scheduler_gport_params_t * params,
    int flow_id)
{
    int se_id, hr_id;
    dbal_enum_value_field_hr_mode_e hr_mode = DBAL_ENUM_FVAL_HR_MODE_SINGLE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id);

    SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_hr_mode_get(unit, params, &hr_mode));

    SHR_IF_ERR_EXIT(dnx_sch_hr_mode_set(unit, params->core, hr_id, hr_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get  HR element parameters
 */
static shr_error_e
dnx_scheduler_element_hr_params_get(
    int unit,
    int core,
    int flow_id,
    bcm_cosq_scheduler_gport_params_t * params)
{
    int se_id, hr_id;
    dbal_enum_value_field_hr_mode_e hr_mode = DBAL_ENUM_FVAL_HR_MODE_SINGLE;
    int is_composite;

    SHR_FUNC_INIT_VARS(unit);

    /** Clear structure */
    sal_memset(params, 0, sizeof(bcm_cosq_scheduler_gport_params_t));

    params->core = core;

    params->cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id);

    SHR_IF_ERR_EXIT(dnx_sch_hr_mode_get(unit, core, hr_id, &hr_mode));

    switch (hr_mode)
    {
        case DBAL_ENUM_FVAL_HR_MODE_SINGLE:
            params->type = bcmCosqSchedulerGportTypeHrSingleWfq;
            break;
        case DBAL_ENUM_FVAL_HR_MODE_DUAL_WFQ:
            params->type = bcmCosqSchedulerGportTypeHrDualWfq;
            break;
        case DBAL_ENUM_FVAL_HR_MODE_ENHANCED:
            params->type = bcmCosqSchedulerGportTypeHrEnhanced;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type %d cannot be specified for HR element\n", params->type);
            break;
    }

    /** determine is composite flow attributes */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, core, flow_id, &is_composite));

    params->mode = (is_composite ? bcmCosqSchedulerGportModeComposite : bcmCosqSchedulerGportModeSimple);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update global independent-per-flow mode
 * (inverse or proportional)
 */
static shr_error_e
dnx_scheduler_element_gport_ipf_update(
    int unit,
    bcm_cosq_scheduler_gport_params_t * params)
{

    dnx_sch_ipf_config_mode_e ipf_mode, configured_ipf_mode;

    SHR_FUNC_INIT_VARS(unit);

    if (params->cl_wfq_mode == bcmCosqSchedulerGportClWfqModeIndependent ||
        params->cl_wfq_mode == bcmCosqSchedulerGportClWfqModeIndependentProportional)
    {

        SHR_IF_ERR_EXIT(cosq_config.ipf_config_mode.get(unit, &configured_ipf_mode));

        ipf_mode = (params->cl_wfq_mode == bcmCosqSchedulerGportClWfqModeIndependent ?
                    DNX_SCH_IPF_CONFIG_MODE_INVERSE : DNX_SCH_IPF_CONFIG_MODE_PROPORTIONAL);

        /** ipf mode not initialized*/
        if (configured_ipf_mode == DNX_SCH_IPF_CONFIG_MODE_NONE)
        {
            SHR_IF_ERR_EXIT(cosq_config.ipf_config_mode.set(unit, ipf_mode));
        }
        else if (configured_ipf_mode != ipf_mode)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Independent and Independent proportional CLs can not be mixed\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return allocation type as defined by scheduler_gport_add parameters
 */
static shr_error_e
dnx_scheduler_element_params_to_flow_type_get(
    int unit,
    bcm_cosq_scheduler_gport_params_t * params,
    dnx_sch_alloc_flow_type_e * flow_type)
{
    dnx_sch_element_se_type_e se_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_element_type_get(unit, params->type, &se_type));

    switch (se_type)
    {
        case DNX_SCH_ELEMENT_SE_TYPE_FQ:
            switch (params->mode)
            {
                case bcmCosqSchedulerGportModeSimple:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_FQ;
                    break;
                case bcmCosqSchedulerGportModeComposite:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected FQ mode %d\n", params->mode);
                    break;
            }
            break;

        case DNX_SCH_ELEMENT_SE_TYPE_CL:
            switch (params->mode)
            {
                case bcmCosqSchedulerGportModeSimple:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_CL;
                    break;
                case bcmCosqSchedulerGportModeComposite:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL;
                    break;
                case bcmCosqSchedulerGportModeEnhancedClLowPrioFQ:
                case bcmCosqSchedulerGportModeEnhancedClHighPrioFQ:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL;
                    break;
                case bcmCosqSchedulerGportModeSharedDual:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ;
                    break;
                case bcmCosqSchedulerGportModeSharedQuad:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4;
                    break;
                case bcmCosqSchedulerGportModeSharedOcta:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected CL mode %d\n", params->mode);
                    break;
            }
            break;

        case DNX_SCH_ELEMENT_SE_TYPE_HR:
            switch (params->mode)
            {
                case bcmCosqSchedulerGportModeSimple:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_HR;
                    break;
                case bcmCosqSchedulerGportModeComposite:
                    *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected HR mode %d\n", params->mode);
                    break;
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected SE type %d\n", se_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return scheduler_gport_add parameters according to flow type
 */
static shr_error_e
dnx_scheduler_element_flow_type_to_params_get(
    int unit,
    int core,
    int flow_id,
    dnx_sch_alloc_flow_type_e flow_type,
    bcm_cosq_scheduler_gport_params_t * params)
{
    dnx_sch_element_se_type_e se_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear structure */
    sal_memset(params, 0, sizeof(bcm_cosq_scheduler_gport_params_t));

    params->core = core;
    params->cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
            SHR_IF_ERR_EXIT(dnx_scheduler_element_fq_params_get(unit, core, flow_type, params));
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
            SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));
            if (se_type == DNX_SCH_ELEMENT_SE_TYPE_CL)
            {
                SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_params_get(unit, core, flow_id, flow_type, params));
            }
            else if (se_type == DNX_SCH_ELEMENT_SE_TYPE_FQ)
            {
                SHR_IF_ERR_EXIT(dnx_scheduler_element_fq_params_get(unit, core, flow_type, params));
            }
            break;

        case DNX_SCH_ALLOC_FLOW_TYPE_HR:
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR:
            SHR_IF_ERR_EXIT(dnx_scheduler_element_hr_params_get(unit, core, flow_id, params));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected flow type %d\n", flow_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return allocation type according to HW setup
 */
static shr_error_e
dnx_scheduler_element_flow_type_hw_get(
    int unit,
    int core,
    int flow_id,
    dnx_sch_alloc_flow_type_e * flow_type)
{
    int is_composite;
    dbal_enum_value_field_shared_shaper_mode_e shared_shaper_mode;
    dnx_sch_cl_config_t cl_config;
    int se_id, cl_id;
    dnx_sch_element_se_type_e se_type;

    SHR_FUNC_INIT_VARS(unit);

    /** determine is composite flow attributes */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, core, flow_id, &is_composite));

    SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_get(unit, core, flow_id, &shared_shaper_mode));

    SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));

    switch (shared_shaper_mode)
    {
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL:
            *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ;
            break;
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD:
            *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4;
            break;
        case DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA:
            *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8;
            break;
        default:
            switch (se_type)
            {
                case DNX_SCH_ELEMENT_SE_TYPE_CL:

                    if (is_composite)
                    {
                        *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));
                        cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, se_id);
                        SHR_IF_ERR_EXIT(dnx_scheduler_element_cl_config_get(unit, core, cl_id, &cl_config));

                        if (cl_config.enhanced_mode != DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED)
                        {
                            *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_ENHANCED_CL;
                        }
                        else
                        {
                            *flow_type = DNX_SCH_ALLOC_FLOW_TYPE_CL;
                        }
                    }
                    break;

                case DNX_SCH_ELEMENT_SE_TYPE_HR:
                    *flow_type = (is_composite ? DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR : DNX_SCH_ALLOC_FLOW_TYPE_HR);
                    break;
                case DNX_SCH_ELEMENT_SE_TYPE_FQ:
                    *flow_type = (is_composite ? DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ : DNX_SCH_ALLOC_FLOW_TYPE_FQ);
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected SE type %d\n", se_type);
                    break;
            }
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure CL element as defined by scheduler_gport_add parameters
 */
shr_error_e
dnx_scheduler_element_gport_cl_configure(
    int unit,
    bcm_cosq_scheduler_gport_params_t * params,
    int flow_id)
{

    dbal_enum_value_field_cl_mode_e cl_type;
    dbal_enum_value_field_cl_enhanced_mode_e enhanced_mode;
    dbal_enum_value_field_wfq_weight_mode_e wfq_mode;
    int profile_id;
    int cl_id, se_id;

    SHR_FUNC_INIT_VARS(unit);

    switch (params->type)
    {
        case bcmCosqSchedulerGportTypeClass4Sp:
            cl_type = DBAL_ENUM_FVAL_CL_MODE_1;
            break;
        case bcmCosqSchedulerGportTypeClass3SpWfq:
            cl_type = DBAL_ENUM_FVAL_CL_MODE_2;
            break;
        case bcmCosqSchedulerGportTypeClassWfq2Sp:
            cl_type = DBAL_ENUM_FVAL_CL_MODE_3;
            break;
        case bcmCosqSchedulerGportTypeClass2SpWfq:
            cl_type = DBAL_ENUM_FVAL_CL_MODE_4;
            break;
        case bcmCosqSchedulerGportTypeClass1SpWfq:
            cl_type = DBAL_ENUM_FVAL_CL_MODE_5;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected type %d\n", params->type);
            break;
    }
    switch (params->mode)
    {
        case bcmCosqSchedulerGportModeEnhancedClLowPrioFQ:
            enhanced_mode = DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP;
            break;
        case bcmCosqSchedulerGportModeEnhancedClHighPrioFQ:
            enhanced_mode = DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP;
            break;
        default:
            enhanced_mode = DBAL_ENUM_FVAL_CL_ENHANCED_MODE_DISABLED;
            break;
    }
    switch (params->cl_wfq_mode)
    {
        case bcmCosqSchedulerGportClWfqModeDiscrete:
        case bcmCosqSchedulerGportClWfqModeNone:
            wfq_mode = DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW;
            break;

        case bcmCosqSchedulerGportClWfqModeClass:
            wfq_mode = DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS;
            break;

        case bcmCosqSchedulerGportClWfqModeIndependent:
        case bcmCosqSchedulerGportClWfqModeIndependentProportional:
            wfq_mode = DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected CL WFQ mode %d\n", params->cl_wfq_mode);
            break;
    }

    SHR_IF_ERR_EXIT(dnx_sch_cl_profile_alloc(unit, params->core, cl_type, enhanced_mode, wfq_mode, &profile_id));

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, se_id);

    /** configure mapping of CL to class profile */
    SHR_IF_ERR_EXIT(dnx_sch_cl_mapping_set(unit, params->core, cl_id, profile_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure CL element as defined by scheduler_gport_add parameters
 */
static shr_error_e
dnx_scheduler_element_cl_params_get(
    int unit,
    int core,
    int flow_id,
    dnx_sch_alloc_flow_type_e flow_type,
    bcm_cosq_scheduler_gport_params_t * params)
{

    int cl_id, se_id;

    dnx_sch_cl_config_t cl_config;
    dnx_sch_ipf_config_mode_e ipf_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** Clear structure */
    sal_memset(params, 0, sizeof(bcm_cosq_scheduler_gport_params_t));

    params->core = core;

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, se_id);

    dnx_scheduler_element_cl_config_get(unit, core, cl_id, &cl_config);

    switch (cl_config.cl_mode)
    {
        case DBAL_ENUM_FVAL_CL_MODE_1:
            params->type = bcmCosqSchedulerGportTypeClass4Sp;
            break;
        case DBAL_ENUM_FVAL_CL_MODE_2:
            params->type = bcmCosqSchedulerGportTypeClass3SpWfq;
            break;
        case DBAL_ENUM_FVAL_CL_MODE_3:
            params->type = bcmCosqSchedulerGportTypeClassWfq2Sp;
            break;
        case DBAL_ENUM_FVAL_CL_MODE_4:
            params->type = bcmCosqSchedulerGportTypeClass2SpWfq;
            break;
        case DBAL_ENUM_FVAL_CL_MODE_5:
            params->type = bcmCosqSchedulerGportTypeClass1SpWfq;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected cl mode %d\n", cl_config.cl_mode);
            break;
    }
    switch (cl_config.enhanced_mode)
    {
        case DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_LP:
            params->mode = bcmCosqSchedulerGportModeEnhancedClLowPrioFQ;
            break;
        case DBAL_ENUM_FVAL_CL_ENHANCED_MODE_ENABLED_HP:
            params->mode = bcmCosqSchedulerGportModeEnhancedClHighPrioFQ;
            break;
        default:
            switch (flow_type)
            {
                case DNX_SCH_ALLOC_FLOW_TYPE_CL:
                    params->mode = bcmCosqSchedulerGportModeSimple;
                    break;
                case DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL:
                    params->mode = bcmCosqSchedulerGportModeComposite;
                    break;
                case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
                    params->mode = bcmCosqSchedulerGportModeSharedDual;
                    break;
                case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
                    params->mode = bcmCosqSchedulerGportModeSharedQuad;
                    break;
                case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
                    params->mode = bcmCosqSchedulerGportModeSharedOcta;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected flow type %d\n", flow_type);
                    break;

            }
            break;
    }
    switch (cl_config.wfq_weight_mode)
    {
        case DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_FLOW:
            params->cl_wfq_mode = (cl_config.cl_mode != DBAL_ENUM_FVAL_CL_MODE_1 ?
                                   bcmCosqSchedulerGportClWfqModeDiscrete : bcmCosqSchedulerGportClWfqModeNone);
            break;

        case DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_DISCRETE_PER_CLASS:
            params->cl_wfq_mode = bcmCosqSchedulerGportClWfqModeClass;
            break;

        case DBAL_ENUM_FVAL_WFQ_WEIGHT_MODE_INDEPENDENT_PER_FLOW:
            SHR_IF_ERR_EXIT(cosq_config.ipf_config_mode.get(unit, &ipf_mode));

            params->cl_wfq_mode = (ipf_mode == DNX_SCH_IPF_CONFIG_MODE_INVERSE ?
                                   bcmCosqSchedulerGportClWfqModeIndependent :
                                   bcmCosqSchedulerGportClWfqModeIndependentProportional);

            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected CL WFQ mode %d\n", cl_config.wfq_weight_mode);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure CL element as defined by scheduler_gport_add parameters
 */
static shr_error_e
dnx_scheduler_element_fq_params_get(
    int unit,
    int core,
    dnx_sch_alloc_flow_type_e flow_type,
    bcm_cosq_scheduler_gport_params_t * params)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Clear structure */
    sal_memset(params, 0, sizeof(bcm_cosq_scheduler_gport_params_t));

    params->core = core;
    params->type = bcmCosqSchedulerGportTypeFq;

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_FQ:
            params->mode = bcmCosqSchedulerGportModeSimple;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ:
            params->mode = bcmCosqSchedulerGportModeComposite;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
            params->mode = bcmCosqSchedulerGportModeSharedDual;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
            params->mode = bcmCosqSchedulerGportModeSharedQuad;
            break;
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
            params->mode = bcmCosqSchedulerGportModeSharedOcta;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected flow type %d\n", flow_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate scheduling element
 */
shr_error_e
dnx_scheduler_element_flow_allocate(
    int unit,
    bcm_core_t core,
    dnx_sch_alloc_flow_type_e flow_type,
    int with_id,
    int *flow_id)
{
    uint32 alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** with_id specified  */
    if (with_id)
    {
        alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_allocate(unit, core, 1, alloc_flags, FALSE, FALSE, 1, flow_type, -1, flow_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate scheduling element
 */
shr_error_e
dnx_scheduler_element_gport_allocate(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t * gport)
{
    bcm_core_t core = params->core;
    dnx_sch_alloc_flow_type_e flow_type = DNX_SCH_ALLOC_FLOW_TYPE_INVALID;
    int flow_id = 0;
    int with_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get allocation type */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_params_to_flow_type_get(unit, params, &flow_type));

    /** with_id specified  */
    if (flags & BCM_COSQ_GPORT_WITH_ID)
    {
        flow_id = BCM_GPORT_SCHEDULER_GET(*gport);
        with_id = TRUE;
    }

    SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_allocate(unit, core, flow_type, with_id, &flow_id));

    /** set the result gport */
    BCM_GPORT_SCHEDULER_CORE_SET((*gport), flow_id, core);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate port HR
 * 
 * \param [in] unit -  Unit-ID 
 * \param [in] core - core ID
 * \param [in] port_info - basic port info
 * \param [in] nof_priorities - number of scheduler priorities
 * \param [in] priority_propagation_enable - priority propagation enable
 * \param [in] with_id - should allocate WITH ID
 * \param [out] hr_id - allocated HR ID
 *  
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
shr_error_e
dnx_scheduler_port_hr_allocate(
    int unit,
    bcm_core_t core,
    dnx_algo_port_info_s port_info,
    int nof_priorities,
    int priority_propagation_enable,
    int with_id,
    int *hr_id)
{
    dnx_sch_alloc_flow_type_e flow_type = DNX_SCH_ALLOC_FLOW_TYPE_HR;
    int flow_id = 0;
    int se_id;
    uint32 allocated_hrs[1];
    int ps_priority;
    int ps_priority_propagation_enable;
    int prio;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
    {
        /*
         * For ERP use fixed pre-allocated HRs
         */
        *hr_id = dnx_data_sch.flow.erp_hr_get(unit);
    }
    else
    {
        /** with_id specified  */
        if (with_id)
        {
            SHR_IF_ERR_EXIT(dnx_sch_allocated_hrs_get(unit, core, *hr_id / dnx_data_sch.ps.nof_hrs_in_ps_get(unit),
                                                      allocated_hrs, &ps_priority, &ps_priority_propagation_enable));

            if (ps_priority != 0 && (priority_propagation_enable != ps_priority_propagation_enable))
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "cannot allocate hr %d for priority propagation = %d. ps is used for for priority propagation = %d\n",
                             *hr_id, priority_propagation_enable, ps_priority_propagation_enable);
            }
            if (ps_priority != 0 && ps_priority != nof_priorities)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "cannot allocate hr %d for %d sch priorities port. ps is used for %d priorities\n",
                             *hr_id, nof_priorities, ps_priority);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_scheduler_available_hr_id_find(unit, core, nof_priorities,
                                                               priority_propagation_enable, hr_id));
        }

        for (prio = 0; prio < nof_priorities; prio++)
        {
            /** hr to se_id */
            se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, *hr_id + prio);
            /** se_id to flow id */
            SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));

            SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_allocate(unit, core, flow_type, TRUE, &flow_id));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - obtain shared shaper mode from scheduler_gport_add params
 */
shr_error_e
dnx_scheduler_gport_params_to_shared_shaper_mode(
    int unit,
    bcm_cosq_scheduler_gport_params_t * params,
    dbal_enum_value_field_shared_shaper_mode_e * shared_shaper_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (params->mode)
    {
        case bcmCosqSchedulerGportModeSharedDual:
            *shared_shaper_mode = DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_DUAL;
            break;
        case bcmCosqSchedulerGportModeSharedQuad:
            *shared_shaper_mode = DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_QUAD;
            break;
        case bcmCosqSchedulerGportModeSharedOcta:
            *shared_shaper_mode = DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_OCTA;
            break;
        default:
            *shared_shaper_mode = DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE;
            break;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - configure scheduling element
 */
shr_error_e
dnx_scheduler_element_gport_configure(
    int unit,
    bcm_cosq_scheduler_gport_params_t * params,
    int flow_id)
{
    bcm_core_t core = params->core;
    dnx_sch_alloc_flow_type_e flow_type = DNX_SCH_ALLOC_FLOW_TYPE_INVALID;
    dnx_sch_element_se_type_e se_type;
    int is_composite, is_shared_shaper;
    int i;
    int allocated_flows[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS];
    int nof_flows;
    dbal_enum_value_field_shared_shaper_mode_e shared_shaper_mode;

    SHR_FUNC_INIT_VARS(unit);

    is_composite = (params->mode == bcmCosqSchedulerGportModeComposite);
    is_shared_shaper = (params->mode == bcmCosqSchedulerGportModeSharedDual ||
                        params->mode == bcmCosqSchedulerGportModeSharedQuad ||
                        params->mode == bcmCosqSchedulerGportModeSharedOcta);

    /** update global Independent mode, if necessary */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_ipf_update(unit, params));

    /** get allocation type */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_params_to_flow_type_get(unit, params, &flow_type));

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_flow_ids_get(unit, core, 1, flow_type, flow_id,
                                                          allocated_flows, &nof_flows));

    for (i = 0; i < nof_flows; i++)
    {

        SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, allocated_flows[i], &se_type));

        switch (se_type)
        {
            case DNX_SCH_ELEMENT_SE_TYPE_CL:
                SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_cl_configure(unit, params, allocated_flows[i]));
                break;

            case DNX_SCH_ELEMENT_SE_TYPE_HR:
                SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_hr_configure(unit, params, allocated_flows[i]));
                break;

            case DNX_SCH_ELEMENT_SE_TYPE_FQ:
                /** nothing to configure */
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected SE type %d\n", se_type);
                break;
        }
    }

    if (is_composite)
    {
        /*
         * If the flow is composite - set is composite in HW
         * If the flow is not composite - nothing to be done,
         * the clean is performed in gport_delete
         */
        SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_set(unit, core, flow_id, TRUE));
    }

    if (is_shared_shaper)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_params_to_shared_shaper_mode(unit, params, &shared_shaper_mode));

        SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_set(unit, core, flow_id, shared_shaper_mode));

        /*
         * for regular SE, we configure nothing and rely on the default value.
         * Shared shaper mode is cleared during gport_delete
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate and configure scheduling element
 */
shr_error_e
dnx_scheduler_element_gport_add(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t * gport)
{
    int flow_id;
    int nof_reserved_hrs;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_allocate(unit, flags, params, gport));

    flow_id = BCM_GPORT_SCHEDULER_GET(*gport);

    SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_configure(unit, params, flow_id));

    if (flags & BCM_COSQ_SCHEDULER_GPORT_FMQ_RESERVE)
    {
        /** Update swstate that this HR would be used as FMQ HR */

        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.nof_reserved_hr.get(unit, params->core, &nof_reserved_hrs));

        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.reserved_hr_flow_id.set(unit, params->core, nof_reserved_hrs, flow_id));

        nof_reserved_hrs++;

        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.nof_reserved_hr.set(unit, params->core, nof_reserved_hrs));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify element_gport_replace parameters
 */
static shr_error_e
dnx_scheduler_element_gport_replace_verify(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t gport)
{
    bcm_core_t core;
    dnx_sch_element_se_type_e se_type;
    int flow_id;
    dbal_enum_value_field_shared_shaper_mode_e params_shared_shaper_mode, hw_shared_shaper_mode;
    int is_se;
    int se_id;
    int is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        if (params->mode != bcmCosqSchedulerGportModeSimple)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Only simple mode supported with REPLACE flag\n");
        }

        SHR_IF_ERR_EXIT(dnx_scheduler_gport_element_type_get(unit, params->type, &se_type));

        if (se_type != DNX_SCH_ELEMENT_SE_TYPE_HR)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "REPLACE flag can be provided for HR only\n");
        }

        core = BCM_GPORT_SCHEDULER_CORE_GET(gport);

        if (core != params->core)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "provided core %d is not consistend with gport 0x%x \n", params->core, gport);
        }

        flow_id = BCM_GPORT_SCHEDULER_GET(gport);

    }
    else if (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport))
    {
        if (params->mode != bcmCosqSchedulerGportModeSharedOcta && params->mode != bcmCosqSchedulerGportModeSharedQuad)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expected Shared Quad or Shared Quad mode\n");
        }

        SHR_IF_ERR_EXIT(dnx_scheduler_gport_element_type_get(unit, params->type, &se_type));

        if (se_type != DNX_SCH_ELEMENT_SE_TYPE_CL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "REPLACE flag in shared shaper can be provided for CL element only\n");
        }

        core = BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_CORE_GET(gport);

        if (core != params->core)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "provided core %d is not consistend with gport 0x%x \n", params->core, gport);
        }

        flow_id = BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_FLOW_GET(gport);

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport 0x%x, Scheduler gport or shared shaper gport is expected\n", gport);
    }

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, flow_id, &is_allocated));

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow %d is not allocated\n", flow_id);
    }

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, flow_id, &is_se));

    if (!is_se)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "provided flow %d is not SE \n", flow_id);
    }

    SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));

    if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        if (!DNX_SCHEDULER_ELEMENT_IS_HR(unit, se_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "provided flow %d is not HR \n", flow_id);
        }
    }
    else
    {
        /** shared shaper */
        if (!DNX_SCHEDULER_ELEMENT_IS_CL(unit, se_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "provided flow %d is not CL \n", flow_id);
        }

        SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_get(unit, core, flow_id, &hw_shared_shaper_mode));

        SHR_IF_ERR_EXIT(dnx_scheduler_gport_params_to_shared_shaper_mode(unit, params, &params_shared_shaper_mode));

        if (hw_shared_shaper_mode != params_shared_shaper_mode)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "provided mode %d is not consistent with HW\n", params->mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - update configuration of existing scheduling element
 */
shr_error_e
dnx_scheduler_element_gport_replace(
    int unit,
    uint32 flags,
    bcm_cosq_scheduler_gport_params_t * params,
    bcm_gport_t * gport)
{
    bcm_gport_t actual_sched_gport;
    int flow_id;
    bcm_core_t core;
    dnx_sch_element_se_type_e se_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_actual_sched_gport_get(unit, *gport, FALSE, &actual_sched_gport));

    /** verify replace specific parameters */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_replace_verify(unit, flags, params, actual_sched_gport));

    /** get flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_base_flow_id_get(unit, actual_sched_gport, 0, &core, &flow_id));

    SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));

    switch (se_type)
    {
        case DNX_SCH_ELEMENT_SE_TYPE_CL:
            SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_cl_configure(unit, params, flow_id));
            break;

        case DNX_SCH_ELEMENT_SE_TYPE_HR:
            SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_hr_configure(unit, params, flow_id));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected SE type %d\n", se_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_scheduler_element_gport_delete_verify(
    int unit,
    bcm_gport_t gport)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_SCHEDULER(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected sched object gport type, 0x%x\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify flow id is scheduler flow and is allocated 
 */
int
dnx_scheduler_element_id_valid_se_verify(
    int unit,
    int core,
    int flow_id)
{
    int is_se;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_base_flow_id_cosq_verify(unit, core, flow_id, 0, FALSE));

    /** determine is composite flow attributes */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_se_get(unit, core, flow_id, &is_se));

    if (!is_se)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "expected scheduling element (flow id = %d)\n", flow_id);

    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_scheduler_element_delete_verify(
    int unit,
    int core,
    int flow_id)
{
    int se_id;
    uint32 default_se;
    int is_port_hr;
    int fmq_class_indx;
    int is_fmq_class;
    int is_reserved;

    dnx_sch_alloc_flow_type_e flow_type = DNX_SCH_ALLOC_FLOW_TYPE_INVALID;
    dnx_sch_flow_credit_source_t credit_source;

    int subflow[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS];
    int nof_subflows;
    int i;
    uint32 sw_state_ref_cnt;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_element_id_valid_se_verify(unit, core, flow_id));

    /** verify this is not reverved element */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_reserved_element(unit, core, flow_id, &is_reserved));
    if (is_reserved)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "reserved SE %d can't be deleted\n", flow_id);
    }

    /** verify this is not port HR */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_port_hr_get(unit, core, flow_id, &is_port_hr));
    if (is_port_hr)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port HR %d can't be deleted\n", flow_id);
    }

    /** verify this is not FMQ HR */
    SHR_IF_ERR_EXIT(dnx_scheduler_is_flow_fmq_class(unit, core, flow_id, &is_fmq_class, &fmq_class_indx));
    if (is_fmq_class)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FMQ HR %d can't be deleted\n", flow_id);
    }

    /** determine the allocated flow type */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_type_hw_get(unit, core, flow_id, &flow_type));
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_real_flow_ids_get(unit, core, 1, flow_type, flow_id, subflow, &nof_subflows));

    switch (flow_type)
    {
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4:
        case DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8:
            if (flow_id % nof_subflows != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Flow %d is not the first element in shared shaper\n", flow_id);
            }
            break;
        default:
            break;
    }

    default_se = dnx_data_sch.flow.default_credit_source_se_id_get(unit);
    for (i = 0; i < nof_subflows; i++)
    {
        /** check that the flows are detached */
        SHR_IF_ERR_EXIT(dnx_sch_flow_credit_src_hw_get(unit, core, subflow[i], &credit_source));
        if (credit_source.se_id != default_se)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow id (%d) is not detached\n", flow_id);
        }

        /** check that the SE is not referenced */
        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, subflow[i], &se_id));

        SHR_IF_ERR_EXIT(cosq_config.se.child_count.get(unit, core, se_id, &sw_state_ref_cnt));

        if (sw_state_ref_cnt > 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flow id (%d) reference count is not zero (%d)\n", subflow[i], sw_state_ref_cnt);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete scheduling element
 */
int
dnx_scheduler_element_flow_delete(
    int unit,
    bcm_core_t core,
    int flow_id)
{

    dnx_sch_credit_src_info_t zero_credit_src = { 0 };
    dnx_sch_alloc_flow_type_e flow_type = DNX_SCH_ALLOC_FLOW_TYPE_INVALID;

    int is_composite;
    dnx_sch_element_se_type_e se_type;
    int cl_profile;
    uint8 is_last;
    int cl_id, hr_id, se_id;
    int subflow[DNX_DATA_MAX_SCH_SCH_ALLOC_DEALLOC_FLOW_IDS];
    int nof_subflows;
    int i;
    int is_shared_shaper;
    int is_tag_clear;

    SHR_FUNC_INIT_VARS(unit);

    /** determine the allocated flow type -- must be done before changing configuration */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_type_hw_get(unit, core, flow_id, &flow_type));

    is_composite = (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_COMP_FQ || flow_type == DNX_SCH_ALLOC_FLOW_TYPE_COMP_CL ||
                    flow_type == DNX_SCH_ALLOC_FLOW_TYPE_COMP_HR);

    if (is_composite)
    {
        /*
         * If the flow is composite - clear the composite status
         * If the flow is not composite - nothing to be done,
 */
        SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_set(unit, core, flow_id, FALSE));

    }

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_real_flow_ids_get(unit, core, 1, flow_type, flow_id, subflow, &nof_subflows));

    for (i = 0; i < nof_subflows; i++)
    {
        /** update flow attributes with default values */
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_default_attributes_set(unit, core, subflow[i]));

        SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, subflow[i], &se_type));
        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, subflow[i], &se_id));

        switch (se_type)
        {
            case DNX_SCH_ELEMENT_SE_TYPE_CL:

                /** free template resources */
                cl_id = DNX_SCHEDULER_ELEMENT_CL_ID_GET(unit, se_id);
                SHR_IF_ERR_EXIT(dnx_sch_cl_mapping_get(unit, core, cl_id, &cl_profile));
                SHR_IF_ERR_EXIT(dnx_scheduler_allocators_cl_profile_dealloc(unit, core, cl_profile, &is_last));

                /** clear the mapping in HW */
                SHR_IF_ERR_EXIT(dnx_sch_cl_mapping_set(unit, core, cl_id, 0));
                break;

            case DNX_SCH_ELEMENT_SE_TYPE_HR:
                hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id);
                /** reset HR mode to default */
                SHR_IF_ERR_EXIT(dnx_sch_hr_mode_set(unit, core, hr_id, DBAL_ENUM_FVAL_HR_MODE_SINGLE));
                break;

            case DNX_SCH_ELEMENT_SE_TYPE_FQ:
                /** nothing to configure */
                break;
            default:
                break;
        }

    }

    /** free resources */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_deallocate(unit, core, 1, flow_type, flow_id));

    is_shared_shaper = (flow_type == DNX_SCH_ALLOC_FLOW_TYPE_SHARED_CL_FQ ||
                        flow_type == DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_4 ||
                        flow_type == DNX_SCH_ALLOC_FLOW_TYPE_SHARED_SE_8);

    if (is_shared_shaper)
    {
        /*
         * Must be done after deallocation
         */

        /*
         * Check if this is the last allocation in the octet
         * to decide if the mode should be reset
         */
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_is_shared_shaper_tag_clear(unit, core, flow_id, &is_tag_clear));
        if (is_tag_clear)
        {
            SHR_IF_ERR_EXIT(dnx_sch_shared_shaper_mode_set(unit, core, flow_id,
                                                           DBAL_ENUM_FVAL_SHARED_SHAPER_MODE_NONE));
        }
    }

    for (i = 0; i < nof_subflows; i++)
    {
        /** free flow resources */
        SHR_IF_ERR_EXIT(cosq_config.flow.credit_src.set(unit, core, subflow[i], &zero_credit_src));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete scheduling element
 */
int
dnx_scheduler_element_gport_delete(
    int unit,
    bcm_gport_t gport)
{
    int core;
    int flow_id;

    SHR_FUNC_INIT_VARS(unit);

    /** get flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_base_flow_id_get(unit, gport, 0, &core, &flow_id));

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_element_delete_verify(unit, core, flow_id));

    SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_delete(unit, core, flow_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get scheduling element gport parameters
 */
int
dnx_scheduler_gport_get(
    int unit,
    int core,
    int flow_id,
    bcm_cosq_scheduler_gport_params_t * params)
{
    dnx_sch_alloc_flow_type_e flow_type = DNX_SCH_ALLOC_FLOW_TYPE_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_element_id_valid_se_verify(unit, core, flow_id));

    /** determine the allocated flow type */
    SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_type_hw_get(unit, core, flow_id, &flow_type));

    SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_type_to_params_get(unit, core, flow_id, flow_type, params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify dnx_scheduler_connector_gport_get parameters
 */
static int
dnx_scheduler_connector_gport_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_voq_connector_gport_t * config)
{
    int flow_id;
    int core;
    int is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport (0x%x) specified. Expected VOQ connector gport\n", gport);
    }

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    /** determine core and flow id */
    core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport);
    flow_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport);

    SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, flow_id, &is_allocated));

    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow %d is not allocated\n", flow_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information about VOQ connector gport
 */
int
dnx_scheduler_connector_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_voq_connector_gport_t * config)
{
    int is_interdigitated;
    int base_flow_id, flow_region;
    int core;
    uint8 sw_state_num_cos;

    int is_composite;
    int octet_id;
    uint32 remote_fap_id;
    uint32 flags;
    int low_rate_range_start, low_rate_range_end;
    uint32 low_rate_enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_connector_gport_get_verify(unit, gport, config));

    /** Clear structure */
    sal_memset(config, 0, sizeof(bcm_cosq_voq_connector_gport_t));

    /** determine core and flow id */
    core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport);
    base_flow_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport);

    /** determine is composite flow attributes */
    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, core, base_flow_id, &is_composite));

    octet_id = base_flow_id / 8;

    SHR_IF_ERR_EXIT(dnx_sch_flow_fap_mapping_get(unit, core, octet_id, &remote_fap_id));

    /** update parameters */
    flags = BCM_COSQ_VOQ_CONNECTOR_GPORT_F_RESERVED;    /* place-holder for BCM_COSQ_GPORT_VOQ_CONNECTOR */
    flags |= (is_composite) ? BCM_COSQ_VOQ_CONNECTOR_GPORT_F_COMPOSITE : 0;

    /** determine flow attributes (non-contiguous) */
    flow_region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, core, flow_region, &is_interdigitated));

    SHR_IF_ERR_EXIT(dnx_sch_dbal_low_rate_range_get(unit, core,
                                                    &low_rate_range_start, &low_rate_range_end, &low_rate_enable));

    flags |= (is_interdigitated) ? BCM_COSQ_VOQ_CONNECTOR_GPORT_F_NON_CONTIGUOUS : 0;

    if (low_rate_enable && base_flow_id >= low_rate_range_start && base_flow_id <= low_rate_range_end)
    {
        flags |= BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE;
    }

    SHR_IF_ERR_EXIT(cosq_config.connector.num_cos.get(unit, core, base_flow_id, &sw_state_num_cos));

    config->numq = sw_state_num_cos;
    config->flags = flags;

    config->remote_modid = remote_fap_id;
    config->nof_remote_cores = dnx_data_sch.flow.nof_remote_cores_get(unit, core, flow_region)->val;
    BCM_COSQ_GPORT_CORE_SET(config->port, core);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information about scheduler element gport
 * in terms of legacy bcm_cosq_gport_add API
 */
int
dnx_sched_gport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * physical_port,
    int *num_cos_levels,
    uint32 *flags)
{
    bcm_cosq_scheduler_gport_params_t params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_cosq_scheduler_gport_get(unit, 0, gport, &params));

    (*flags) = BCM_COSQ_GPORT_SCHEDULER;

    switch (params.type)
    {
        case bcmCosqSchedulerGportTypeClass4Sp:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP;
            break;
        case bcmCosqSchedulerGportTypeClass3SpWfq:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE2_3SP_WFQ;
            break;
        case bcmCosqSchedulerGportTypeClassWfq2Sp:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE3_WFQ_2SP;
            break;
        case bcmCosqSchedulerGportTypeClass2SpWfq:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ;
            break;
        case bcmCosqSchedulerGportTypeClass1SpWfq:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ;
            break;
        case bcmCosqSchedulerGportTypeFq:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_FQ;
            break;
        case bcmCosqSchedulerGportTypeHrSingleWfq:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ;
            break;
        case bcmCosqSchedulerGportTypeHrDualWfq:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ;
            break;
        case bcmCosqSchedulerGportTypeHrEnhanced:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected element type %d\n", params.type);
            break;
    }

    switch (params.mode)
    {
        case bcmCosqSchedulerGportModeComposite:
            (*flags) |= BCM_COSQ_GPORT_COMPOSITE;
            break;
        case bcmCosqSchedulerGportModeSharedDual:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER;
            break;
        case bcmCosqSchedulerGportModeEnhancedClLowPrioFQ:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_LOWPRI_FQ;
            break;
        case bcmCosqSchedulerGportModeEnhancedClHighPrioFQ:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_HIGHPRI_FQ;
            break;
        case bcmCosqSchedulerGportModeSimple:
            /** nothing to do */
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected element mode %d\n", params.mode);
            break;
    }

    switch (params.cl_wfq_mode)
    {
        case bcmCosqSchedulerGportClWfqModeDiscrete:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_DISCREET;
            break;
        case bcmCosqSchedulerGportClWfqModeClass:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_CLASS;
            break;
        case bcmCosqSchedulerGportClWfqModeIndependent:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT;
            break;
        case bcmCosqSchedulerGportClWfqModeIndependentProportional:
            (*flags) |= BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT_PROPORTIONAL;
            break;
        case bcmCosqSchedulerGportClWfqModeNone:
            /** nothing to do */
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected WFQ %d\n", params.cl_wfq_mode);
            break;
    }

     /** In case of Scheduler component, always set num_cos_levels to 1 */
    *num_cos_levels = 1;

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Verify E2E TC gport and priority
 */
static int
dnx_scheduler_gport_e2e_port_tc_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** verify gport */
    if (!BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "E2E PORT TC gport expected. gport: 0x%x\n", gport);
    }

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_sch_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_verify(unit, logical_port, cosq));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Verify E2E TCG gport and priority
 */
static int
dnx_scheduler_tcg_available_verify(
    int unit,
    bcm_port_t port)
{

    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port, &priority_propagation_enable));
    if (priority_propagation_enable)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port %d: cannot apply TCG APIs in port priority propagation mode\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Verify E2E TCG gport and priority
 */
static int
dnx_scheduler_gport_e2e_port_tcg_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    bcm_port_t logical_port;
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** verify gport */
    if (!BCM_COSQ_GPORT_IS_E2E_PORT_TCG(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "E2E PORT TCG gport expected. gport: 0x%x\n", gport);
    }

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_sch_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_tcg_available_verify(unit, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    if (cosq < 0 || cosq >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cosq is out of range. Should be in range of 0-%d. port: %d cosq %d\n",
                     nof_priorities - 1, logical_port, cosq);
    }

    if (nof_priorities < dnx_data_sch.ps.min_priority_for_tcg_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TCG is not supported for port %d\n", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Verify dnx_scheduler_gport_e2e_port_tc_sched_set and get common parameters
 * Used by TC to TCG mapping API and thus
 * need to verify TCG related parameters as well
 */
static int
dnx_scheduler_gport_e2e_port_tc_sched_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    int nof_priorities;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_verify(unit, gport, cosq));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_sch_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_tcg_available_verify(unit, logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    if (nof_priorities < dnx_data_sch.ps.min_priority_for_tcg_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TCG is not supported for port %d\n", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Verify dnx_scheduler_gport_e2e_port_tc_sched_set parameters
 */
static int
dnx_scheduler_gport_e2e_port_tc_sched_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int nof_priorities;
    bcm_port_t logical_port;
    int tcg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_sched_verify(unit, gport, cosq));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    if ((!(mode >= BCM_COSQ_SP0 && mode <= BCM_COSQ_SP7)) &&
        (!(mode >= 0 && mode < dnx_data_sch.ps.max_nof_tcg_get(unit))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mode must be in range (0-%d) or BCM_COSQ_SP0-BCM_COSQ_SP7\n",
                     dnx_data_sch.ps.max_nof_tcg_get(unit) - 1);
    }

    tcg = (mode >= BCM_COSQ_SP0) ? (mode - BCM_COSQ_SP0) : mode;

    if (tcg >= nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tcg must be in range (0-%d)\n", nof_priorities - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief
 * Attach Port-Priority of a given port priority (HR) TC-X to the TCG-Y
 */
int
dnx_scheduler_gport_e2e_port_tc_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int tcg;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_sched_set_verify(unit, gport, cosq, mode, weight));

    tcg = (mode >= BCM_COSQ_SP0) ? (mode - BCM_COSQ_SP0) : mode;

    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_to_tcg_map_set(unit, logical_port, cosq, tcg));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Verify dnx_scheduler_gport_e2e_port_tc_sched_get parameters
 */
static int
dnx_scheduler_gport_e2e_port_tc_sched_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_sched_verify(unit, gport, cosq));

    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Get Port TCG of a given port priority (TC).
 */
int
dnx_scheduler_gport_e2e_port_tc_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    bcm_port_t logical_port;
    int tcg;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_sched_get_verify(unit, gport, cosq, mode, weight));

    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_to_tcg_map_get(unit, logical_port, cosq, &tcg));

    *mode = tcg + BCM_COSQ_SP0;

    if (weight != NULL)
    {
        *weight = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * verify dnx_scheduler_gport_e2e_port_tcg_sched_set parameters
 */
int
dnx_scheduler_gport_e2e_port_tcg_sched_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_verify(unit, gport, cosq));

    /** Verify mode */
    if (!((mode == -1) || (mode == BCM_COSQ_NONE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid mode %d. Expected -1 or BCM_COSQ_NONE\n", mode);
    }

    if (mode == -1)
    {
        /** Weight is taken into consideration */
        SHR_RANGE_VERIFY(weight, 0, dnx_data_sch.ps.max_tcg_weight_get(unit), _SHR_E_PARAM,
                         "Invalid weight %d\n", weight);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Set EIR Weight for Egress Port TCG
 */
int
dnx_scheduler_gport_e2e_port_tcg_sched_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int mode,
    int weight)
{
    int is_valid;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tcg_sched_set_verify(unit, gport, cosq, mode, weight));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    is_valid = (mode == BCM_COSQ_NONE) ? FALSE : TRUE;

    /** Set TCG weight */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_weight_set(unit, logical_port, cosq, is_valid, weight));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Set EIR Weight for Egress Port TCG
 */
int
dnx_scheduler_gport_e2e_port_tcg_sched_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");
    SHR_NULL_CHECK(weight, _SHR_E_PARAM, "weight");

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_verify(unit, gport, cosq));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Get EIR Weight for Egress Port TCG.
 */
int
dnx_scheduler_gport_e2e_port_tcg_sched_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *mode,
    int *weight)
{
    bcm_port_t logical_port;
    int is_valid, temp_weight;
    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tcg_sched_get_verify(unit, gport, cosq, mode, weight));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    /** get EIR weight and Mode */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_weight_get(unit, logical_port, cosq, &is_valid, &temp_weight));

    *mode = (is_valid) ? -1 : BCM_COSQ_NONE;
    if (is_valid)
    {
        *weight = temp_weight;
    }
    else
    {
        *weight = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * verify dnx_scheduler_gport_e2e_port_tc_bandwidth_set parameters
 */
static int
dnx_scheduler_gport_e2e_port_tc_bandwidth_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_verify(unit, gport, cosq));

    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d\n", kbits_sec_min);
    }

    if (kbits_sec_max > (dnx_data_sch.ps.max_port_rate_mbps_get(unit) * 1000))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid parameter: kbits_sec_max %d is bigger than maximal port rate %d\nMaximal port rate is determined by maximal_port_bandwidth soc property\n",
                     kbits_sec_max, (dnx_data_sch.ps.max_port_rate_mbps_get(unit) * 1000));
    }

    /** verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "no flag is supported\n");

exit:
    SHR_FUNC_EXIT;

}

/** 
 * \brief -
 * Configure Port-TC (HR) shaping rate, set single port rate.
 */
int
dnx_scheduler_gport_e2e_port_tc_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_bandwidth_set_verify(unit, gport, cosq,
                                                                               kbits_sec_min, kbits_sec_max, flags));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    /** Set configuration */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_shaper_rate_set(unit, logical_port, cosq, kbits_sec_max));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * verify dnx_scheduler_gport_e2e_port_tc_bandwidth_get parameters
 */
static int
dnx_scheduler_gport_e2e_port_tc_bandwidth_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_verify(unit, gport, cosq));

exit:
    SHR_FUNC_EXIT;

}

/** 
 * \brief -
 * Get Port-TC (HR) shaping rate
 */
int
dnx_scheduler_gport_e2e_port_tc_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_bandwidth_get_verify(unit, gport, cosq,
                                                                               kbits_sec_min, kbits_sec_max, flags));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    /** Get existing configuration */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_shaper_rate_get(unit, logical_port, cosq, kbits_sec_max));

    /** fill rates */
    *kbits_sec_min = 0;
    *flags = 0;

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * verify e2e_port_tcg_bandwidth_set parameters
 */
static int
dnx_scheduler_gport_e2e_port_tcg_bandwidth_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_verify(unit, gport, cosq));

    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d\n", kbits_sec_min);
    }

    if (kbits_sec_max > (dnx_data_sch.ps.max_port_rate_mbps_get(unit) * 1000))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid parameter: kbits_sec_max %d is bigger than maximal port rate %d\nMaximal port rate is determined by maximal_port_bandwidth soc property\n",
                     kbits_sec_max, (dnx_data_sch.ps.max_port_rate_mbps_get(unit) * 1000));
    }

    /** verify flags */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "no flag is supported\n");

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Configure TCG shaping rate - E2E
 */
int
dnx_scheduler_gport_e2e_port_tcg_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tcg_bandwidth_set_verify(unit, gport, cosq,
                                                                                kbits_sec_min, kbits_sec_max, flags));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    /** Set configuration */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_shaper_rate_set(unit, logical_port, cosq, kbits_sec_max));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Verify e2e_port_tcg_bandwidth_get parameters
 */
static int
dnx_scheduler_gport_e2e_port_tcg_bandwidth_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_verify(unit, gport, cosq));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Get E2E Port-TCG shaping rate
 */
int
dnx_scheduler_gport_e2e_port_tcg_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tcg_bandwidth_get_verify(unit, gport, cosq,
                                                                                kbits_sec_min, kbits_sec_max, flags));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    /** Get configuration */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_shaper_rate_get(unit, logical_port, cosq, kbits_sec_max));

    /** fill rates */
    *kbits_sec_min = 0;
    *flags = 0;

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * verify e2e_port_tcg_bandwidth_set parameters
 */
static int
dnx_scheduler_gport_e2e_port_tc_max_burst_verify(
    int unit,
    bcm_gport_t gport,
    int cosq,
    uint32 max_burst)
{

    uint32 max_burst_upper_limit;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_verify(unit, gport, cosq));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_maximal_max_burst_get(unit, FALSE, &max_burst_upper_limit));

    if (max_burst > max_burst_upper_limit)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: max burst: %d\n", max_burst);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify e2e port tc group set parameters
 */
static int
dnx_scheduler_gport_e2e_port_tc_group_set_verify(
    int unit,
    bcm_gport_t gport,
    int cosq,
    uint32 group)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_verify(unit, gport, cosq));

    /*
     * verify group
     */
    if (group > dnx_data_sch.se.nof_color_group_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Group %d", group);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e port tc scheduler 
 */
int
dnx_scheduler_control_e2e_port_tc_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
            /** Set burst max for e2e port tc shaper */

            /** Validate gport */
            SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_max_burst_verify(unit, port, cosq, arg));

            /** retrieve port */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));

            SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_max_burst_set(unit, logical_port, cosq, arg));

            break;

        case bcmCosqControlGroup:
            /** Validate gport */
            SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_group_set_verify(unit, port, cosq, arg));

            /** retrieve logical port */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));

            /** set color group */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_group_set(unit, logical_port, cosq, arg));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get various feature dependent control type
 * for the e2e port tc
 */
int
dnx_scheduler_control_e2e_port_tc_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
            /** Get burst max for e2e port tc shaper */

            /** Validate gport */
            SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_verify(unit, port, cosq));

            /** retrieve port */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));

            /** Get existing configuration */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_max_burst_get(unit, logical_port, cosq, (uint32 *) arg));

            break;

        case bcmCosqControlGroup:
            /** Validate gport */
            SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tc_verify(unit, port, cosq));

            /** retrieve logical port */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));

            /** get color group */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_group_get(unit, logical_port, cosq, arg));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * verify e2e_port_tcg max burst set parameters
 */
static int
dnx_scheduler_gport_e2e_port_tcg_max_burst_verify(
    int unit,
    bcm_gport_t gport,
    int cosq,
    uint32 max_burst)
{

    uint32 max_burst_upper_limit;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tcg_verify(unit, gport, cosq));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_maximal_max_burst_get(unit, TRUE, &max_burst_upper_limit));

    if (max_burst > max_burst_upper_limit)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: max burst: %d\n", max_burst);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e port tcg scheduler 
 */
int
dnx_scheduler_control_e2e_port_tcg_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
            /** Set burst max for e2e port tcg shaper */

            /** Validate gport */
            SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tcg_max_burst_verify(unit, port, cosq, arg));

            /** retrieve port */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));

            /** Set Port-TCG max burst */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_max_burst_set(unit, logical_port, cosq, arg));

            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Get various feature dependent control type
 * for the e2e port tcg
 */
int
dnx_scheduler_control_e2e_port_tcg_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    switch (type)
    {
        case bcmCosqControlBandwidthBurstMax:
            /** Get burst max for e2e tcg shaper */

            /** Validate gport */
            SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_tcg_verify(unit, port, cosq));

            /** retrieve port */
            SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, port, &logical_port));

            /** Get existing configuration */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_max_burst_get(unit, logical_port, cosq, (uint32 *) arg));

            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e port
 */
int
dnx_scheduler_cosq_control_e2e_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlFlowControlPriority:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcmCosqControlFlowControlPriority is not supported on this device\n");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get various feature dependent control type
 * for the e2e port
 */
int
dnx_scheduler_cosq_control_e2e_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlFlowControlPriority:
            SHR_ERR_EXIT(_SHR_E_PARAM, "bcmCosqControlFlowControlPriority is not supported on this device\n");
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_scheduler_interface_priority_mapping_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq)
{
    bcm_port_t port;
    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    /** gport is validated by caller function */
    port = BCM_COSQ_GPORT_E2E_INTERFACE_GET(gport);
    SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_verify(unit, port, cosq));

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, port, &priority_propagation_enable));
    SHR_VAL_VERIFY(priority_propagation_enable, TRUE, _SHR_E_PARAM,
                   "This API can be called only on port with priority propagation enabled\n");

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_scheduler_interface_priority_mapping_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int arg)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_interface_priority_mapping_verify(unit, gport, cosq));

    if (arg != BCM_COSQ_HIGH_PRIORITY && arg != BCM_COSQ_LOW_PRIORITY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid arg %d\n", arg);
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_scheduler_interface_priority_mapping_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int arg)
{
    int e2e_if, egq_if;
    bcm_port_t port;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_interface_priority_mapping_set_verify(unit, gport, cosq, arg));

    port = BCM_COSQ_GPORT_E2E_INTERFACE_GET(gport);

    /** get sch interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    /** get egq interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    /** should priority propagation be supported for LAG scheduler??? */
    SHR_IF_ERR_EXIT(dnx_sch_dbal_sch_if_to_egq_if_map_set(unit, core, e2e_if + cosq, egq_if,
                                                          (arg == BCM_COSQ_HIGH_PRIORITY ? TRUE : FALSE)));
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_scheduler_interface_priority_mapping_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_interface_priority_mapping_verify(unit, gport, cosq));

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

exit:
    SHR_FUNC_EXIT;
}

int
dnx_scheduler_interface_priority_mapping_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *arg)
{
    int e2e_if, egq_if;
    bcm_port_t port;
    bcm_core_t core;
    int is_high_priority;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_interface_priority_mapping_get_verify(unit, gport, cosq, arg));

    port = BCM_COSQ_GPORT_E2E_INTERFACE_GET(gport);

    /** get sch interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));
    /** get egq interface from port mgmt */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    /** should priority propagation be supported for LAG scheduler??? */
    SHR_IF_ERR_EXIT(dnx_sch_dbal_sch_if_to_egq_if_map_get(unit, core, e2e_if + cosq, &egq_if, &is_high_priority));
    *arg = (is_high_priority ? BCM_COSQ_HIGH_PRIORITY : BCM_COSQ_LOW_PRIORITY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set various feature dependent control type
 * for the e2e interface
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  E2E interface gport
 * \param [in] cosq -  TC
 * \param [in] type -  control type
 * \param [in] arg  -  control value
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
int
dnx_scheduler_cosq_control_e2e_interface_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlPrioritySelect:
            SHR_IF_ERR_EXIT(dnx_scheduler_interface_priority_mapping_set(unit, port, cosq, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get various feature dependent control type
 * for the e2e interface
 *
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  E2E interface gport
 * \param [in] cosq -  TC
 * \param [in] type -  control type
 * \param [in] arg  -  control value
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
int
dnx_scheduler_cosq_control_e2e_interface_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlPrioritySelect:
            SHR_IF_ERR_EXIT(dnx_scheduler_interface_priority_mapping_get(unit, port, cosq, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief 
 * configure HRs for the port to be added
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  logical port
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
static shr_error_e
dnx_scheduler_port_hr_configure(
    int unit,
    int port)
{

    int priority_i;
    int nof_priorities;
    int core;
    int flow_id;
    int base_hr, se_id;
    uint32 tm_port;
    dnx_algo_port_info_s port_info;

    bcm_cosq_scheduler_gport_params_t params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, &base_hr));

    /** A new port is added*/

    /**
     * Get Core from logical port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
        se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority_i);
        SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));

        /** allocate hr */
        params.core = core;
        params.type = bcmCosqSchedulerGportTypeHrDualWfq;
        params.mode = bcmCosqSchedulerGportModeSimple;
        params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;

        /** the HRs are allocated already; need to configure only */
        SHR_IF_ERR_EXIT(dnx_scheduler_element_gport_configure(unit, &params, flow_id));
    }

    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));

        /** Map TM port to HR*/
        SHR_IF_ERR_EXIT(dnx_sch_dbal_port_to_base_hr_mapping_set(unit, core, tm_port, base_hr));
    }

    if (DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_db.general.nof_reserved_erp_hr.dec(unit, core, nof_priorities));
    }

    /** When adding a port, enable the port after  configuration change */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_enable_set(unit, port, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/** \brief 
 * de-allocate HRs for the port to be deleted
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] port -  logical port
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
static shr_error_e
dnx_scheduler_port_hr_remove(
    int unit,
    int port)
{

    int priority_i;
    int nof_priorities;
    int core;
    int base_hr, se_id;
    int flow_id;
    uint32 tm_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, &nof_priorities));

    /**
     * Get TM port and Core from logical port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, &base_hr));

    /** An exising port is deleted */

    /** When deleting a port, disable the port before configuration change */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_enable_set(unit, port, FALSE));

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
        se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority_i);
        SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));

        /** release hr */
        SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_delete(unit, core, flow_id));
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));
        /** UnMap TM port to HR*/
        SHR_IF_ERR_EXIT(dnx_sch_dbal_port_to_base_hr_mapping_clear(unit, core, tm_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Validate dnx_scheduler_gport_e2e_port_bandwidth_set parameters
 */
static int
dnx_scheduler_gport_e2e_port_bandwidth_set_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    bcm_port_t port;
    uint32 supported_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d\n", cosq);
    }

    SHR_MAX_VERIFY(kbits_sec_max, dnx_data_sch.interface.max_if_rate_mbps_get(unit) * 1000, _SHR_E_PARAM,
                   "Invalid parameter: kbits_sec_max: %d out of range max %d\n", kbits_sec_max,
                   dnx_data_sch.interface.max_if_rate_mbps_get(unit) * 1000);

    if (kbits_sec_min > 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d - must be zero\n", kbits_sec_min);
    }

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_sch_verify(unit, port));

    supported_flags |= BCM_COSQ_BW_NOT_COMMIT;

    /** currently only BCM_COSQ_BW_NOT_COMMIT flag is supported */
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "Unsupported flag provided\n");

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Configure E2E port bandwidth
 */
int
dnx_scheduler_gport_e2e_port_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    bcm_port_t logical_port;
    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_bandwidth_set_validate(unit, gport, cosq,
                                                                              kbits_sec_min, kbits_sec_max, flags));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_sw_set(unit, logical_port, kbits_sec_max));

    if (!(flags & BCM_COSQ_BW_NOT_COMMIT))
    {
        /** Configure interface calendar */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_hw_set(unit));
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, logical_port,
                                                                      &priority_propagation_enable));
    if (priority_propagation_enable)
    {
        /** Under priority propagation, configure 1st TCG shaper */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_shaper_rate_set(unit, logical_port, 0, kbits_sec_max));
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * validate dnx_scheduler_gport_e2e_port_bandwidth_get parameters
 */
static int
dnx_scheduler_gport_e2e_port_bandwidth_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

    if (cosq != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid cosq %d\n", cosq);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Get E2E port bandwidth
 */
int
dnx_scheduler_gport_e2e_port_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    bcm_port_t logical_port;
    int priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_port_bandwidth_get_validate(unit, gport, cosq,
                                                                              kbits_sec_min, kbits_sec_max, flags));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, logical_port,
                                                                      &priority_propagation_enable));
    if (priority_propagation_enable)
    {
        /** Under priority propagation, get 1st TCG shaper */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_shaper_rate_get(unit, logical_port, 0, kbits_sec_max));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_hw_get(unit, logical_port, kbits_sec_max));
    }

    *kbits_sec_min = 0; /* always 0 */
    *flags = 0;

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Configure E2E interface bandwidth
 */
static int
dnx_scheduler_gport_e2e_interface_bandwidth_set_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{

    SHR_FUNC_INIT_VARS(unit);

    if (kbits_sec_min != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid parameter: kbits_sec_min: %d\nkbits_sec_min must be set to 0",
                     kbits_sec_min);
    }

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "No flag is supported\n");

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Configure E2E interface bandwidth
 */
int
dnx_scheduler_gport_e2e_interface_bandwidth_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_interface_bandwidth_set_validate(unit, gport, cosq,
                                                                                   kbits_sec_min, kbits_sec_max,
                                                                                   flags));

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_interface_shaper_rate_set(unit, logical_port, kbits_sec_max));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Get E2E interface bandwidth
 */
static int
dnx_scheduler_gport_e2e_interface_bandwidth_get_validate(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(kbits_sec_min, _SHR_E_PARAM, "kbits_sec_min");
    SHR_NULL_CHECK(kbits_sec_max, _SHR_E_PARAM, "kbits_sec_max");
    SHR_NULL_CHECK(flags, _SHR_E_PARAM, "flags");

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -
 * Get E2E interface bandwidth
 */
int
dnx_scheduler_gport_e2e_interface_bandwidth_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags)
{
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_gport_e2e_interface_bandwidth_get_validate(unit, gport, cosq,
                                                                                   kbits_sec_min, kbits_sec_max,
                                                                                   flags));
    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &logical_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_interface_shaper_rate_get(unit, logical_port, kbits_sec_max));

    *kbits_sec_min = 0;
    *flags = 0;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * allocate HRs for advanced FMQ mode
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core -  core-ID 
 * \param [in] hr_nof - number of required HRs 
 * \param [out] hr_id - array of allocated HR IDs
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
int
dnx_scheduler_fmq_hr_allocate(
    int unit,
    bcm_core_t core,
    int hr_nof,
    uint32 *hr_id)
{
    int index = 0;
    int se_id;
    bcm_gport_t hr_gport;
    uint32 flags = 0;
    int flow_id[DNX_DATA_MAX_SCH_GENERAL_NOF_FMQ_CLASS];

    int hr_group;

    bcm_cosq_scheduler_gport_params_t params;
    int nof_reserved_hrs;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.nof_reserved_hr.get(unit, core, &nof_reserved_hrs));
    for (index = 0; index < hr_nof; index++)
    {

        if (index < nof_reserved_hrs)
        {
            /** reserved HRs are allocated and configured by bcm_cosq_scheduler_gport_add API */
            SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.reserved_hr_flow_id.get(unit, core, index, &flow_id[index]));
        }
        else
        {
            /** Allocate HR */
            params.core = core;
            params.type = bcmCosqSchedulerGportTypeHrDualWfq;
            params.mode = bcmCosqSchedulerGportModeSimple;
            params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;

            SHR_IF_ERR_EXIT(bcm_dnx_cosq_scheduler_gport_add(unit, flags, &params, &hr_gport));
            flow_id[index] = BCM_GPORT_SCHEDULER_GET(hr_gport);

        }

        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id[index], &se_id));

        /** Return HR to caller */
        hr_id[index] = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id);

        SHR_IF_ERR_EXIT(dnx_scheduler_hr_port_enable_set(unit, core, hr_id[index], 1, TRUE));

        /*
         * Assign FMQ HRs to scheduling groups (colors) as the following:
         *  - guaranteed (index = 0) -> group A
         *  - best effort 0 (index = 1) -> group B
         *  - best effort 1 (index = 2) -> group C
         *  - best effort 2 (index = 3) -> group C
         */

        /** set new group for each HR */
        hr_group = 0;
        SHR_IF_ERR_EXIT(dnx_sch_se_color_group_set(unit, core, se_id, hr_group));

        if (hr_group < dnx_data_sch.se.nof_color_group_get(unit) - 1)
        {
            hr_group++;
        }

        /** mark HR as colored */
        SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, hr_id[index], TRUE));
    }

    for (index = 0; index < hr_nof; index++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.fmq_flow_id.set(unit, core, index, flow_id[index]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * deallocate advanced FMQ mode HRs 
 * 
 * called when switching to simple FMQ mode
 *  
 * \param [in] unit -  Unit-ID 
 * \param [in] core -  core-ID 
 * \param [in] hr_nof - number of required HRs 
 * \param [in] hr_id - array of HR IDs
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 */
int
dnx_scheduler_fmq_hr_deallocate(
    int unit,
    bcm_core_t core,
    int hr_nof,
    uint32 *hr_id)
{
    int index = 0;
    int flow_id;
    int se_id;
    int nof_reserved_hrs;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.nof_reserved_hr.get(unit, core, &nof_reserved_hrs));
    for (index = 0; index < hr_nof; index++)
    {
        if (index >= nof_reserved_hrs)
        {
            /** reserved HRs are allocated by user - we don't deallocate them */

            se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, hr_id[index]);

            SHR_IF_ERR_EXIT(dnx_sch_convert_se_to_flow_id(unit, se_id, &flow_id));

            SHR_IF_ERR_EXIT(dnx_scheduler_element_flow_delete(unit, core, flow_id));
        }

        SHR_IF_ERR_EXIT(dnx_scheduler_hr_port_enable_set(unit, core, hr_id[index], 1, FALSE));

        /** mark HR as uncolored */
        SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, hr_id[index], FALSE));
    }

    for (index = 0; index < hr_nof; index++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.fmq_flow_id.set(unit, core, index, -1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   return number of flow quartets taken by a single queue quad
 */
static int
dnx_scheduler_nof_flow_quartets_per_queue_quad_get(
    int unit,
    int is_interdigitated,
    int is_composite,
    int *nof_flow_quartets_per_queue_quad)
{
    SHR_FUNC_INIT_VARS(unit);

    *nof_flow_quartets_per_queue_quad = 1;

    if (is_interdigitated)
    {
        *nof_flow_quartets_per_queue_quad *= 2;
    }

    if (is_composite)
    {
        *nof_flow_quartets_per_queue_quad *= 2;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   connect VOQ connector to remote VOQ (egress side)
 */
int
dnx_scheduler_connection_set(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    int is_remove = FALSE;
    int nbr_cos, nbr_quads, cur_quad, i;
    int base_qid, base_cid, flow_region;
    uint8 is_valid;

    int is_non_contiguous;
    int is_composite;
    uint32 nof_remote_cores;
    uint8 sw_state_num_cos;
    /*
     * Just load some initial value to be on the safe side. Overwritten below.
     */
    int egress_core = 0;

    int curr_flow_id, curr_flow_quartet, actual_flow_id, curr_queue_quad;
    int nof_flow_quartets_per_queue_quad;
    int actual_flow_quartet, other_quartet;
    int update_fap_mapping;
    int other_flow_id;
    int remote_fap_id;
    int actual_octet_id, nof_octets;

    int is_local;

    SHR_FUNC_INIT_VARS(unit);

    is_remove = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_INVALID) ? TRUE : FALSE;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport_connect->voq))
    {
        base_qid = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport_connect->voq);
    }
    else
    {
        base_qid = BCM_GPORT_MCAST_QUEUE_GROUP_GET(gport_connect->voq);
    }

    egress_core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport_connect->voq_connector);
    base_cid = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect->voq_connector);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_base_flow_id_cosq_verify(unit, egress_core, base_cid, 0, TRUE));

    SHR_IF_ERR_EXIT(cosq_config.connector.num_cos.get(unit, egress_core, base_cid, &sw_state_num_cos));

    nbr_cos = sw_state_num_cos;

    nbr_quads = DNX_COSQ_IPQ_QUEUE_TO_QUAD_GET(nbr_cos);

    flow_region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_cid);
    SHR_IF_ERR_EXIT(dnx_scheduler_region_is_interdigitated_get(unit, egress_core, flow_region, &is_non_contiguous));

    SHR_IF_ERR_EXIT(dnx_sch_flow_is_composite_get(unit, egress_core, base_cid, &is_composite));

    SHR_IF_ERR_EXIT(dnx_scheduler_nof_flow_quartets_per_queue_quad_get(unit, is_non_contiguous, is_composite,
                                                                       &nof_flow_quartets_per_queue_quad));

    nof_remote_cores = dnx_data_sch.flow.nof_remote_cores_get(unit, egress_core, flow_region)->val;

    for (cur_quad = 0; cur_quad < nbr_quads; cur_quad++)
    {
        curr_queue_quad = DNX_COSQ_IPQ_QUEUE_TO_QUAD_GET(base_qid +
                                                         (cur_quad * dnx_data_ipq.queues.min_bundle_size_get(unit)));

        /*
         * curr_flow_id is the first flow queue quad curr_queue_quad is mapped to 
         * this is necessarily first connector in quartet
         */
        curr_flow_id = base_cid + DNX_SCHEDULER_QUARTET_TO_FLOW_ID(cur_quad * nof_flow_quartets_per_queue_quad);
        curr_flow_quartet = DNX_SCHEDULER_FLOW_ID_TO_QUARTET(curr_flow_id);

        update_fap_mapping = TRUE;

        if (nof_flow_quartets_per_queue_quad == 1)
        {
            /*
             * in all other cases the alignment is at least 8, so we mark the whole octet
             */

            /** ID of the other quartet in the same octet */
            other_quartet = (curr_flow_quartet % 2 ? curr_flow_quartet - 1 : curr_flow_quartet + 1);
            other_flow_id = DNX_SCHEDULER_QUARTET_TO_FLOW_ID(other_quartet);

            SHR_IF_ERR_EXIT(cosq_config.connector.connection_valid.get(unit, egress_core, other_flow_id, &is_valid));
            if (is_valid)
            {
                /*
                 * if single quartet is mapped and other quartet in the same octet 
                 * is mapped - nothing need to be updated 
                 */
                update_fap_mapping = FALSE;
            }
        }

        /** set all symmetric connections*/
        for (i = 0; i < nof_remote_cores; i++)
        {

            actual_flow_id = DNX_SCH_FLOW_ID_FROM_REMOTE_CORE(curr_flow_id, nof_remote_cores, i);

            actual_flow_quartet = DNX_SCHEDULER_FLOW_ID_TO_QUARTET(actual_flow_id);

            SHR_IF_ERR_EXIT(dnx_sch_flow_queue_mapping_range_set(unit, egress_core,
                                                                 actual_flow_quartet, nof_flow_quartets_per_queue_quad,
                                                                 curr_queue_quad));

            if (update_fap_mapping)
            {

                remote_fap_id = (is_remove ? dnx_data_device.general.invalid_fap_id_get(unit) :
                                 gport_connect->remote_modid + i);

                actual_octet_id = actual_flow_quartet / 2;
                nof_octets = (nof_flow_quartets_per_queue_quad == 1 ? 1 : nof_flow_quartets_per_queue_quad / 2);
                SHR_IF_ERR_EXIT(dnx_sch_flow_fap_mapping_range_set(unit, egress_core, actual_octet_id, nof_octets,
                                                                   remote_fap_id));

            }
        }

        SHR_IF_ERR_EXIT(cosq_config.connector.connection_valid.set(unit, egress_core, curr_flow_id,
                                                                   is_remove ? FALSE : TRUE));

    }

    /** update flow attributes */
    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, gport_connect->remote_modid, &is_local));

    if (!is_local)
    {
        /*
         * if the flow is not remote - no need to configure anything
         * the flow is by default local 
         */
        for (i = 0; i < nbr_cos; i++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_flow_is_remote_hw_set(unit, egress_core, base_cid + i, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   obtain remote VOQ connected to provided VOQ connector (egress side)
 */
int
dnx_scheduler_connection_get(
    int unit,
    bcm_cosq_gport_connection_t * gport_connect)
{
    int base_qid = 0, base_cid = 0;
    int core_id;
    uint32 queue_quad, remote_fap_id;
    int flow_quartet_id, octet_id;

    SHR_FUNC_INIT_VARS(unit);

    base_cid = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect->voq_connector);

    /*
     * Get 'core' (on egress device) from 'gport_connect->voq_connector'. Caller is
     * assumed to have it set properly.
     */
    core_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport_connect->voq_connector);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_base_flow_id_cosq_verify(unit, core_id, base_cid, 0, TRUE));

    flow_quartet_id = DNX_SCHEDULER_FLOW_ID_TO_QUARTET(base_cid);

    SHR_IF_ERR_EXIT(dnx_sch_flow_queue_mapping_get(unit, core_id, flow_quartet_id, &queue_quad));

    base_qid = DNX_COSQ_IPQ_QUAD_TO_QUEUE_GET(queue_quad);

    octet_id = flow_quartet_id / 2;

    SHR_IF_ERR_EXIT(dnx_sch_flow_fap_mapping_get(unit, core_id, octet_id, &remote_fap_id));

    /** set VOQ gport */
    SHR_IF_ERR_EXIT(dnx_ipq_queue_gport_get(unit, BCM_CORE_ALL, base_qid, &gport_connect->voq));

    gport_connect->remote_modid = remote_fap_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify parameters of dnx_cosq_sched_gport_add
 */
static int
dnx_cosq_sched_gport_add_verify(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport)
{

    uint32 valid_flags;
    int is_cl, is_hr, is_fq;
    int nof_types;

    SHR_FUNC_INIT_VARS(unit);

    valid_flags = (BCM_COSQ_GPORT_SCHEDULER |
                   dnx_cosq_gport_scheduler_hr_mask | dnx_cosq_gport_scheduler_cl_mask | BCM_COSQ_GPORT_SCHEDULER_FQ |
                   BCM_COSQ_GPORT_COMPOSITE | BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER |
                   dnx_cosq_gport_scheduler_cl_enhanced_mask | dnx_cosq_gport_scheduler_cl_wfq_mask |
                   BCM_COSQ_GPORT_REPLACE | BCM_COSQ_GPORT_WITH_ID);

    SHR_MASK_VERIFY(flags, valid_flags, _SHR_E_PARAM, "provided unsupported flags\n");

    is_cl = flags & dnx_cosq_gport_scheduler_cl_mask;
    is_hr = flags & dnx_cosq_gport_scheduler_hr_mask;
    is_fq = flags & BCM_COSQ_GPORT_SCHEDULER_FQ;
    nof_types = 0;

    if (is_cl)
    {
        nof_types++;
    }
    if (is_hr)
    {
        nof_types++;
    }
    if (is_fq)
    {
        nof_types++;
    }

    if (!(flags & BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER))
    {

        if (nof_types != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Exactly one elemement type is expected. Specified %d types\n", nof_types);

        }

    }
    else
    {
        if (!is_cl)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "CL element must be specified for dual shaper\n");

        }
        if (is_hr)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dual shaper Cl-HR is not supported on this device\n");

        }
    }

    SHR_NOF_SET_BITS_IN_RANGE_VERIFY(flags, (BCM_COSQ_GPORT_COMPOSITE | BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER |
                                             dnx_cosq_gport_scheduler_cl_enhanced_mask), 0, 1,
                                     _SHR_E_PARAM, "at most one composite/dual/enhance flag can be specified\n");

    if (flags & (dnx_cosq_gport_scheduler_cl_mask | BCM_COSQ_GPORT_SCHEDULER_FQ))
    {
        /** Verify numq for other scheduler elements other than HRs */
        if (numq != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid cos levels specified %d\n", numq);
        }
    }

    if (BCM_COSQ_GPORT_IS_ISQ_ROOT(*gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ISQ Root gport is not supported on this device\n");
    }

    if (flags & BCM_COSQ_GPORT_REPLACE)
    {
        if (!(BCM_COSQ_GPORT_IS_E2E_PORT(*gport) || BCM_COSQ_GPORT_IS_FMQ_CLASS(*gport) ||
              BCM_COSQ_GPORT_IS_E2E_PORT_TC(*gport)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport 0x%x, modify supported only for e2e ports", (*gport));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Allocate and configure new scheduling element
 * legacy API implementation
 */
int
dnx_cosq_sched_gport_add(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t * gport)
{

    bcm_cosq_scheduler_gport_params_t params;

    uint32 scheduler_flags = 0;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t allocated_gport;
    int is_dual_shaper;
    uint32 type_mask;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_sched_gport_add_verify(unit, port, numq, flags, gport));

    /** get core id */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_core_get(unit, ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL, port, &params.core));

    is_dual_shaper = (flags & BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER);

    type_mask = (dnx_cosq_gport_scheduler_cl_mask | dnx_cosq_gport_scheduler_hr_mask |
                 /** do not include FQ for dual shaper -- CL must exist */
                 (is_dual_shaper ? 0 : BCM_COSQ_GPORT_SCHEDULER_FQ));

    switch (flags & type_mask)
    {
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP:
            params.type = bcmCosqSchedulerGportTypeClass4Sp;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE2_3SP_WFQ:
            params.type = bcmCosqSchedulerGportTypeClass3SpWfq;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE3_WFQ_2SP:
            params.type = bcmCosqSchedulerGportTypeClassWfq2Sp;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ:
            params.type = bcmCosqSchedulerGportTypeClass2SpWfq;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ:
            params.type = bcmCosqSchedulerGportTypeClass1SpWfq;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_FQ:
            params.type = bcmCosqSchedulerGportTypeFq;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ:
            params.type = bcmCosqSchedulerGportTypeHrSingleWfq;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ:
            params.type = bcmCosqSchedulerGportTypeHrDualWfq;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED:
            params.type = bcmCosqSchedulerGportTypeHrEnhanced;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unexpected element specification 0x%x\n",
                         flags & (dnx_cosq_gport_scheduler_cl_mask | dnx_cosq_gport_scheduler_hr_mask |
                                  BCM_COSQ_GPORT_SCHEDULER_FQ));
            break;
    }

    switch (flags & (BCM_COSQ_GPORT_COMPOSITE | BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER |
                     dnx_cosq_gport_scheduler_cl_enhanced_mask))
    {
        case BCM_COSQ_GPORT_COMPOSITE:
            params.mode = bcmCosqSchedulerGportModeComposite;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER:
            params.mode = bcmCosqSchedulerGportModeSharedDual;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_LOWPRI_FQ:
            params.mode = bcmCosqSchedulerGportModeEnhancedClLowPrioFQ;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_HIGHPRI_FQ:
            params.mode = bcmCosqSchedulerGportModeEnhancedClHighPrioFQ;
            break;
        default:
            params.mode = bcmCosqSchedulerGportModeSimple;
            break;
    }

    switch (flags & dnx_cosq_gport_scheduler_cl_wfq_mask)
    {
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_DISCREET:
            params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeDiscrete;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_CLASS:
            params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeClass;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT:
            params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeIndependent;
            break;
        case BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT_PROPORTIONAL:
            params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeIndependentProportional;
            break;
        default:
            params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
            break;
    }

    if (flags & BCM_COSQ_GPORT_REPLACE)
    {
        scheduler_flags |= BCM_COSQ_SCHEDULER_GPORT_REPLACE;
    }
    else if (flags & BCM_COSQ_GPORT_WITH_ID)
    {
        scheduler_flags |= BCM_COSQ_SCHEDULER_GPORT_WITH_ID;
    }

    if (BCM_COSQ_GPORT_IS_E2E_PORT(*gport) || BCM_COSQ_GPORT_IS_E2E_PORT_TC(*gport))
    {

        gport_info.in_gport = *gport;
        gport_info.cosq = (BCM_COSQ_GPORT_IS_E2E_PORT_TC(*gport) ? numq : 0);

        SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info));
        allocated_gport = gport_info.out_gport;
    }
    else
    {
        allocated_gport = *gport;
    }
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_scheduler_gport_add(unit, scheduler_flags, &params, &allocated_gport));

    if (!(flags & BCM_COSQ_GPORT_REPLACE))
    {
        *gport = allocated_gport;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Map q-pair(s) to HRs
 * \param [in] unit -  Unit-ID 
 * \param [in] qpair_port - EGQ port id
 * \param [in] hr_port - SCH port id
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
static shr_error_e
dnx_scheduler_port_qpair_to_hr_map_set(
    int unit,
    bcm_port_t qpair_port,
    bcm_port_t hr_port)
{
    int hr_index, i;

    int nof_priorities, nof_sch_priorities;
    bcm_core_t hr_core, qpair_core;
    int nof_qpairs_per_hr;
    int base_hr, base_q_pair;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, hr_port, &nof_sch_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, hr_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, hr_port, &hr_core));

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, qpair_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, qpair_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, qpair_port, &qpair_core));

    if (nof_sch_priorities > nof_priorities)
    {
        nof_sch_priorities = nof_priorities;
    }
    nof_qpairs_per_hr = nof_priorities / nof_sch_priorities;
    for (hr_index = 0; hr_index < nof_sch_priorities; hr_index++)
    {
        for (i = 0; i < nof_qpairs_per_hr; i++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_dbal_qpair_to_hr_mapping_set(unit, hr_core, qpair_core,
                                                                 base_q_pair + hr_index * nof_qpairs_per_hr + i,
                                                                 base_hr + hr_index, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Clear mapping of q-pair(s) to HRs
 * \param [in] unit -  Unit-ID 
 * \param [in] port - port id
 *
 * \return
 *   See shr_error_e 
 * \remark
 *   * None
 * \see
 *   * none
 */
static shr_error_e
dnx_scheduler_port_qpair_to_hr_map_clear(
    int unit,
    bcm_port_t port)
{
    int i;

    int nof_priorities, base_q_pair;
    bcm_core_t egq_core, sch_core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &egq_core));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, sch_core)
    {
        for (i = 0; i < nof_priorities; i++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_dbal_qpair_to_hr_mapping_clear(unit, sch_core, egq_core, base_q_pair + i));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify source parameter of dnx_scheduler_internal_fc_map_set/get
 */
static shr_error_e
dnx_scheduler_internal_fc_map_source_verify(
    int unit,
    bcm_cosq_fc_endpoint_t * source)
{
    bcm_port_t src_port;
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_MASK_VERIFY(source->flags, 0, _SHR_E_PARAM, "no flag supported\n");

    /** source port + cosq */
    if (!BCM_COSQ_GPORT_IS_PORT_TC(source->port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid source->port 0x%x, expected PORT_TC gport\n", source->port);
    }

    /** retrieve port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, source->port, &src_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, src_port, &nof_priorities));

    SHR_RANGE_VERIFY(source->cosq, 0, nof_priorities - 1, _SHR_E_PARAM,
                     "source->cosq (%d) is out of range. (Number of priorities is %d)\n", source->cosq, nof_priorities);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_scheduler_internal_fc_ports_attached_verify(
    int unit,
    bcm_port_t src_port,
    bcm_port_t target_port)
{
    bcm_port_t mapped_sch_port;
    int base_q_pair, mapped_hr;
    bcm_core_t src_core, target_core;
    int valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, src_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &src_core));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, target_port, &target_core));

    SHR_IF_ERR_EXIT(dnx_sch_dbal_qpair_to_hr_mapping_get(unit, target_core, src_core, base_q_pair, &mapped_hr, &valid));
    if (valid)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_hr_to_logical_get(unit, target_core, mapped_hr, &mapped_sch_port));
    }
    if (!valid || mapped_sch_port != target_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "source port %d is not attached to target port %d\n", src_port, target_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify dnx_scheduler_internal_fc_map_set parameters
 */
static shr_error_e
dnx_scheduler_internal_fc_map_set_verify(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    bcm_port_t src_port, target_port;

    SHR_FUNC_INIT_VARS(unit);

    /** verify source */
    SHR_IF_ERR_EXIT(dnx_scheduler_internal_fc_map_source_verify(unit, source));

    SHR_MASK_VERIFY(target->flags, 0, _SHR_E_PARAM, "no flag supported\n");
    /** verify target port + cosq */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_e2e_port_tc_verify(unit, target->port, target->cosq));

    /** retrieve ports */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, source->port, &src_port));
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, target->port, &target_port));

    /** Verify src port is attached to target port */
    SHR_IF_ERR_EXIT(dnx_scheduler_internal_fc_ports_attached_verify(unit, src_port, target_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure mapping of qpair to HR
 */
shr_error_e
dnx_scheduler_internal_fc_map_set(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    bcm_cosq_fc_endpoint_t * target)
{
    bcm_port_t src_port, target_port;
    int base_q_pair, base_hr;
    bcm_core_t sch_core, egq_core;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_internal_fc_map_set_verify(unit, source, target));

    /** retrieve source port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, source->port, &src_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, src_port, &base_q_pair));

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, src_port, &egq_core, &tm_port));

    /** retrieve target port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, target->port, &target_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, target_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, target_port, &sch_core));

    SHR_IF_ERR_EXIT(dnx_sch_dbal_qpair_to_hr_mapping_set(unit, sch_core, egq_core,
                                                         base_q_pair + source->cosq, base_hr + target->cosq, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify parameters of dnx_scheduler_internal_fc_map_get
 */
shr_error_e
dnx_scheduler_internal_fc_map_get_verify(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{

    SHR_FUNC_INIT_VARS(unit);

    /** verify source */
    SHR_IF_ERR_EXIT(dnx_scheduler_internal_fc_map_source_verify(unit, source));

    if (target_max < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "target_max (%d) should be 1\n", target_max);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get mapping of qpair to HR
 */
static shr_error_e
dnx_scheduler_port_qpair_to_hr_mapping_get(
    int unit,
    int source_core,
    int source_q_pair,
    int *target_core,
    int *target_hr,
    int *nof_hrs)
{

    bcm_core_t core;
    int hr_id, valid;
    int i = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_qpair_to_hr_mapping_get(unit, core, source_core, source_q_pair, &hr_id, &valid));
        if (valid)
        {
            if (i >= 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "q pair %d is mapped to more than one HR\n", source_q_pair);
            }
            i++;

            *target_core = core;
            *target_hr = hr_id;
        }
    }

    *nof_hrs = i;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get mapping of qpair to HR
 */
shr_error_e
dnx_scheduler_internal_fc_map_get(
    int unit,
    bcm_cosq_fc_endpoint_t * source,
    int target_max,
    bcm_cosq_fc_endpoint_t * target,
    int *target_count)
{
    bcm_core_t sch_core = -1, egq_core;
    bcm_port_t src_port, target_port;
    int base_q_pair, base_hr;
    int hr_id = -1;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_internal_fc_map_get_verify(unit, source, target_max, target, target_count));

    /** retrieve source port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, source->port, &src_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, src_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &egq_core));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_mapping_get(unit, egq_core, base_q_pair + source->cosq,
                                                               &sch_core, &hr_id, target_count));

    if (*target_count)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_hr_to_logical_get(unit, sch_core, hr_id, &target_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, target_port, &base_hr));

        BCM_COSQ_GPORT_E2E_PORT_TC_SET(target[0].port, target_port);
        target[0].cosq = hr_id - base_hr;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * verify parameters of dnx_scheduler_low_rate_connector_range_set
 */
static shr_error_e
dnx_scheduler_low_rate_connector_range_set_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_range_t * range)
{
    int is_allocated_in_range;
    bcm_core_t core, core_i;
    int flow_range_start, flow_range_end;
    int current_range_start, current_range_end;
    uint32 is_current_enabled;

    SHR_FUNC_INIT_VARS(unit);

    if (gport != 0 && !BCM_COSQ_GPORT_IS_CORE(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid port (%x). Expected port 0 or core gport only\n", gport);
    }

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "no flags supported\n");

    if (range->is_enabled)
    {
        /** if range is disabled, range limits are ignored and not checked */
        SHR_RANGE_VERIFY(range->range_start, 0, dnx_data_sch.flow.nof_connectors_only_regions_get(unit) - 1,
                         _SHR_E_PARAM, "range_start (%d) is out of range (0-%d)\n", range->range_start,
                         dnx_data_sch.flow.nof_connectors_only_regions_get(unit) - 1);

        SHR_RANGE_VERIFY(range->range_end, 0, dnx_data_sch.flow.nof_connectors_only_regions_get(unit) - 1,
                         _SHR_E_PARAM, "range_end (%d) is out of range (0-%d)\n", range->range_start,
                         dnx_data_sch.flow.nof_connectors_only_regions_get(unit) - 1);

        if (range->range_start > range->range_end)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid range_start (%d) - range_end (%d) order\n",
                         range->range_start, range->range_end);
        }
    }

    core = (gport == 0 ? BCM_CORE_ALL : BCM_COSQ_GPORT_CORE_GET(gport));
    DNXCMN_CORES_ITER(unit, core, core_i)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_low_rate_range_get(unit, core_i, &flow_range_start, &flow_range_end,
                                                        &is_current_enabled));

        current_range_start = flow_range_start / dnx_data_sch.flow.region_size_get(unit);
        current_range_end = flow_range_end / dnx_data_sch.flow.region_size_get(unit);

        if (is_current_enabled && range->is_enabled)
        {
            int intersect_min, intersect_max;
            int union_min, union_max;

            /** check everything but the common part */
            intersect_min = UTILEX_MAX(current_range_start, range->range_start);
            intersect_max = UTILEX_MIN(current_range_end, range->range_end);
            union_min = UTILEX_MIN(current_range_start, range->range_start);
            union_max = UTILEX_MAX(current_range_end, range->range_end);

            if (intersect_max < intersect_min)
            {
                /** empty intersect - check each range separately */
                SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_is_allocated_in_range(unit, core_i,
                                                                               current_range_start, current_range_end,
                                                                               &is_allocated_in_range));

                if (is_allocated_in_range)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "flows allocated in range (%d - %d)\n",
                                 current_range_start, current_range_end);
                }
                SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_is_allocated_in_range(unit, core_i,
                                                                               range->range_start, range->range_end,
                                                                               &is_allocated_in_range));

                if (is_allocated_in_range)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "flows allocated in range (%d - %d)\n",
                                 range->range_start, range->range_end);
                }
            }
            else
            {
                if (union_min != intersect_min)
                {
                    SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_is_allocated_in_range(unit, core_i,
                                                                                   union_min, intersect_min - 1,
                                                                                   &is_allocated_in_range));

                    if (is_allocated_in_range)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE, "flows are allocated in range (%d - %d)\n",
                                     union_min, intersect_min - 1);
                    }
                }
                if (union_max != intersect_max)
                {
                    SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_is_allocated_in_range(unit, core_i,
                                                                                   intersect_max + 1, union_max,
                                                                                   &is_allocated_in_range));

                    if (is_allocated_in_range)
                    {
                        SHR_ERR_EXIT(_SHR_E_RESOURCE, "flows are allocated in range (%d - %d)\n",
                                     intersect_max + 1, union_max);
                    }
                }
            }
        }
        else if (is_current_enabled)
        {
            /** verify no connector is allocated in current range */
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_is_allocated_in_range(unit, core_i,
                                                                           current_range_start, current_range_end,
                                                                           &is_allocated_in_range));

            if (is_allocated_in_range)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "flows are allocated in range (%d - %d)\n",
                             current_range_start, current_range_end);
            }
        }
        else if (range->is_enabled)
        {
            /** verify no connector is allocated in range */
            SHR_IF_ERR_EXIT(dnx_scheduler_flow_alloc_is_allocated_in_range(unit, core_i,
                                                                           range->range_start, range->range_end,
                                                                           &is_allocated_in_range));

            if (is_allocated_in_range)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "flows are allocated in range (%d - %d)\n",
                             range->range_start, range->range_end);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure low rate connector range
 */
shr_error_e
dnx_scheduler_low_rate_connector_range_set(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_range_t * range)
{

    bcm_core_t core;
    int region_size = dnx_data_sch.flow.region_size_get(unit);
    int flow_start, flow_end;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_low_rate_connector_range_set_verify(unit, gport, flags, range));

    core = (gport == 0 ? BCM_CORE_ALL : BCM_COSQ_GPORT_CORE_GET(gport));

    flow_start = (range->is_enabled ? range->range_start * region_size : 0);
    flow_end = (range->is_enabled ? range->range_end * region_size + region_size - 1 : 0);
    SHR_IF_ERR_EXIT(dnx_sch_dbal_low_rate_range_set(unit, core, flow_start, flow_end, range->is_enabled));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * verify parameters of dnx_scheduler_low_rate_connector_range_get
 */
static shr_error_e
dnx_scheduler_low_rate_connector_range_get_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_range_t * range)
{

    SHR_FUNC_INIT_VARS(unit);

    if (gport != 0 && !BCM_COSQ_GPORT_IS_CORE(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid port (%x). Expected port 0 or core gport only\n", gport);
    }

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "no flags supported\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get low rate connector range
 */
shr_error_e
dnx_scheduler_low_rate_connector_range_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    bcm_cosq_range_t * range)
{

    bcm_core_t core;
    int flow_range_start, flow_range_end;
    uint32 enable;

    int region_size = dnx_data_sch.flow.region_size_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_low_rate_connector_range_get_verify(unit, gport, flags, range));

    core = (gport == 0 ? 0 : BCM_COSQ_GPORT_CORE_GET(gport));
    SHR_IF_ERR_EXIT(dnx_sch_dbal_low_rate_range_get(unit, core, &flow_range_start, &flow_range_end, &enable));

    range->range_start = flow_range_start / region_size;
    range->range_end = flow_range_end / region_size;
    range->is_enabled = enable;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_scheduler_low_rate_factor_set_verify(
    int unit,
    int factor)
{
    bcm_core_t core;
    int flow_range_start, flow_range_end;
    uint32 is_enabled;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_low_rate_range_get(unit, core, &flow_range_start, &flow_range_end, &is_enabled));
        SHR_VAL_VERIFY(is_enabled, 0, _SHR_E_PARAM, "Cannot change low rate factor when low range is enabled\n");
    }

    if (!dnx_data_sch.general.low_rate_factor_to_dbal_enum_get(unit, factor)->valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid bcmCosqLowRateVoqConnectorFactor argument %d\n", factor);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_scheduler_low_rate_factor_set(
    int unit,
    int factor)
{
    dbal_enum_value_field_low_rate_factor_e dbal_factor;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_low_rate_factor_set_verify(unit, factor));

    dbal_factor = dnx_data_sch.general.low_rate_factor_to_dbal_enum_get(unit, factor)->dbal_enum;

    SHR_IF_ERR_EXIT(dnx_scheduler_dbal_low_rate_factor_set(unit, dbal_factor));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_scheduler_low_rate_factor_get(
    int unit,
    int *factor)
{
    dbal_enum_value_field_low_rate_factor_e dbal_factor;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_dbal_low_rate_factor_get(unit, &dbal_factor));
    *factor = dnx_data_sch.general.dbal_enum_to_low_rate_factor_get(unit, dbal_factor)->low_rate_factor;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters of dnx_scheduler_attach_port_to_lag_scheduler
 */
static shr_error_e
dnx_scheduler_attach_port_to_lag_scheduler_verify(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    bcm_port_t lag_scheduler_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_sch.general.feature_get(unit, dnx_data_sch_general_lag_scheduler_supported))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "LAG scheduler is not available on this device\n");
    }
    if (!BCM_COSQ_GPORT_IS_E2E_PORT(sched_port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected E2E PORT gport %x\n", sched_port);
    }

    lag_scheduler_port = BCM_COSQ_GPORT_E2E_PORT_GET(sched_port);
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, lag_scheduler_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_SCH_ONLY(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "sched_port %d is not of type SCH_ONLY\n", lag_scheduler_port);

    }

    /*
     * input_port gport type is validated at the API level 
     */
    SHR_VAL_VERIFY(cosq, -1, _SHR_E_PARAM, "cosq -1 is expected\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - attach port to LAG scheduler
 */
shr_error_e
dnx_scheduler_attach_port_to_lag_scheduler(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    bcm_port_t src_port, target_port;
    bcm_core_t sch_core, egq_core;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_attach_port_to_lag_scheduler_verify(unit, sched_port, input_port, cosq));

   /** retrieve egq port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, input_port, &src_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, src_port, &egq_core, &tm_port));

    /** retrieve target port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, sched_port, &target_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, target_port, &sch_core));

    /** mark the port is member of LAG */
    SHR_IF_ERR_EXIT(dnx_sch_dbal_lag_port_member_set(unit, egq_core, tm_port, TRUE));

    /** mark the port is on different core than its scheduler */
    SHR_IF_ERR_EXIT(dnx_sch_dbal_lag_port_core_flip_set(unit, egq_core, tm_port,
                                                        (egq_core == sch_core ? FALSE : TRUE)));

    /** make default qpair -> HR mapping */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_map_clear(unit, src_port));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_map_set(unit, src_port, target_port));

    /** disable interface level Flow Control */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_egq_to_sch_if_fc_disable(unit, src_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters of dnx_scheduler_attach_port_to_lag_scheduler
 */
static shr_error_e
dnx_scheduler_detach_port_from_lag_scheduler_verify(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    bcm_port_t src_port, target_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_attach_port_to_lag_scheduler_verify(unit, sched_port, input_port, cosq));

    /** retrieve ports */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, input_port, &src_port));
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, sched_port, &target_port));

    /** Verify src port is attached to target port */
    SHR_IF_ERR_EXIT(dnx_scheduler_internal_fc_ports_attached_verify(unit, src_port, target_port));

    /** Verify src port has its own scheduler */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, src_port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port %d does not have scheduler. Cannot detach it from port %d\n",
                     src_port, target_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - detach port from LAG scheduler
 */
shr_error_e
dnx_scheduler_detach_port_from_lag_scheduler(
    int unit,
    bcm_gport_t sched_port,
    bcm_gport_t input_port,
    bcm_cos_queue_t cosq)
{
    bcm_port_t src_port, target_port;
    bcm_core_t egq_core;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_detach_port_from_lag_scheduler_verify(unit, sched_port, input_port, cosq));

    /** retrieve egq port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, input_port, &src_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, src_port, &egq_core, &tm_port));

    /** retrieve target port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, sched_port, &target_port));

    /** mark the port is not member of LAG */
    SHR_IF_ERR_EXIT(dnx_sch_dbal_lag_port_member_set(unit, egq_core, tm_port, FALSE));

    /** mark the port is not on different core than its scheduler */
    SHR_IF_ERR_EXIT(dnx_sch_dbal_lag_port_core_flip_set(unit, egq_core, tm_port, FALSE));

    /** change qpair -> HR mapping  to point to src port Scheduler*/
    SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_map_clear(unit, src_port));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_map_set(unit, src_port, src_port));

    /** enable interface level Flow Control */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_egq_to_sch_if_map(unit, src_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get scheduler port for a given port
 */
shr_error_e
dnx_scheduler_port_scheduler_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * target_port)
{
    bcm_core_t sch_core = -1, egq_core;
    int base_q_pair;
    int hr_id = -1;
    int target_count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, src_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &egq_core));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_qpair_to_hr_mapping_get(unit, egq_core, base_q_pair,
                                                               &sch_core, &hr_id, &target_count));

    if (target_count)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_hr_to_logical_get(unit, sch_core, hr_id, target_port));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "source port %d is not mapped to any sch port\n", src_port);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get scheduler port for a given port
 */
shr_error_e
dnx_scheduler_port_lag_scheduler_get_verify(
    int unit,
    bcm_gport_t child_port,
    bcm_cos_queue_t cos,
    bcm_gport_t * parent_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(parent_port, _SHR_E_PARAM, "parent_port");

    /*
     * child_port gport type is validated at the API level 
     */
    SHR_VAL_VERIFY(cos, -1, _SHR_E_PARAM, "cos -1 is expected\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get scheduler port for a given port
 */
shr_error_e
dnx_scheduler_port_lag_scheduler_get(
    int unit,
    bcm_gport_t child_port,
    bcm_cos_queue_t cos,
    bcm_gport_t * parent_port)
{
    bcm_port_t src_port, target_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_scheduler_port_lag_scheduler_get_verify(unit, child_port, cos, parent_port));

    /** retrieve egq port */
    SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, child_port, &src_port));

    SHR_IF_ERR_EXIT(dnx_scheduler_port_scheduler_get(unit, src_port, &target_port));

    BCM_COSQ_GPORT_E2E_PORT_SET(*parent_port, target_port);

exit:
    SHR_FUNC_EXIT;
}
