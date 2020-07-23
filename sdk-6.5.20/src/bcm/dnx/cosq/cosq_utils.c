/**
 * \file cosq_utils.c
 * 
 *
 * cosq utils functions for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/swstate/auto_generated/access/dnx_cosq_ingress_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <bcm_int/dnx/mirror/mirror_rcy.h>

/**
 * \brief -
 * verify gport and cosq for rate class threshold APIs
 */
int
dnx_cosq_rate_class_gport_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int is_vsq_allowed)
{
    int rate_class;
    uint32 is_created;
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        /** VOQ gport */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Queue gport is not supported on this device. Please use rate class gport instead\n");
    }

    if (cosq != 0 && cosq != -1)
    {
        /** cosq must be 0/-1 - not relevant parameter */
        SHR_ERR_EXIT(_SHR_E_PARAM, "cosq(%d) is irrelevant. must be 0 or -1.", cosq);
    }

    if (BCM_GPORT_IS_PROFILE(gport))
    {
        rate_class = BCM_GPORT_PROFILE_GET(gport);

        SHR_RANGE_VERIFY(rate_class, 0, dnx_data_ingr_congestion.voq.nof_rate_class_get(unit) - 1, _SHR_E_PARAM,
                         "rate class %d is out of range\n", rate_class);

        /*
         * Make sure the rate class was created using the designated function
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.is_created.get(unit, rate_class, &is_created));

        if (!is_created)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Rate Class %d is not created. See bcm_cosq_gport_rate_class_create",
                         rate_class);
        }
    }
    else if (BCM_COSQ_GPORT_IS_VSQ(gport) && is_vsq_allowed)
    {
        if (BCM_COSQ_GPORT_IS_VSQ_GL(gport))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "global VSQ gport (0x%x) is not expected \n", gport);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Unexpected gport type provided 0x%x\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return (list of) input port for the specified local port as provided to CGM block.
 * Currently,
 *   CGM uses PP port as an input port, unless "overwrite pp_port by reassembly" is turned on.
 *
 * Probably in future devices, CGM would use TM port as an input port
 */
shr_error_e
dnx_cosq_cgm_in_port_get(
    int unit,
    bcm_port_t local_port,
    int *nof_entries,
    uint32 cgm_in_port[])
{
    int core_id;
    uint32 reassembly_context[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF];
    int i;
    uint32 invalid_context;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_ingr_congestion.config.feature_get(unit,
                                                    dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite))
    {
        *nof_entries = 0;
        /** use reassembly context */
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_context_for_cgm_get_all(unit, local_port, reassembly_context));

        invalid_context = dnx_data_ingr_reassembly.context.invalid_context_get(unit);

        for (i = 0; i < DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF; i++)
        {
            if ((reassembly_context[i] != DNX_PORT_INGR_REASSEMBLY_NON_INGRESS_PORT_CONTEXT)
                && (reassembly_context[i] != invalid_context))
            {
                cgm_in_port[*nof_entries] = reassembly_context[i];
                (*nof_entries)++;
            }
        }
    }
    else
    {
        /** use pp port */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, local_port, &core_id, &cgm_in_port[0]));
        *nof_entries = 1;
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 *  see .h file
 */
shr_error_e
dnx_cosq_port_fixed_priority_set(
    int unit,
    bcm_port_t port,
    uint8 is_fixed)
{
    dnx_algo_port_info_s port_info;
    int phy_port;
    uint32 flags = 0;
    int mapped_interface_id, core_id;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_ingr_congestion.config.feature_get(unit,
                                                    dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        switch (port_info.port_type)
        {
            case DNX_ALGO_PORT_TYPE_NIF_ETH:
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, flags, &phy_port /** zero based */ ));

                if (is_fixed)
                {
                    SHR_IF_ERR_EXIT(dnx_cosq_ingress_db.fixed_priority.nif.pbmp_port_add(unit, phy_port));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_cosq_ingress_db.fixed_priority.nif.pbmp_port_remove(unit, phy_port));
                }
                break;
            case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
                SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_get(unit, port, &mapped_interface_id));
                if (mapped_interface_id != DNX_ALGO_PORT_EGR_IF_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
                    if (is_fixed)
                    {
                        SHR_IF_ERR_EXIT(dnx_cosq_ingress_db.fixed_priority.mirror.bit_set(unit, core_id,
                                                                                          mapped_interface_id));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_cosq_ingress_db.fixed_priority.mirror.bit_clear(unit, core_id,
                                                                                            mapped_interface_id));
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Mirror priority of port %d are not mapped yet. Can't perform the action", port);

                }
                break;
            default:
                /** nothing to do */
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 *  see .h file
 */
shr_error_e
dnx_cosq_port_fixed_priority_get(
    int unit,
    bcm_port_t port,
    uint8 *is_fixed)
{
    dnx_algo_port_info_s port_info;
    int phy_port;
    uint32 flags = 0;
    int mapped_interface_id;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    *is_fixed = 0;

    if (dnx_data_ingr_congestion.config.feature_get(unit,
                                                    dnx_data_ingr_congestion_config_pp_port_by_reassembly_overwrite))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        switch (port_info.port_type)
        {
            case DNX_ALGO_PORT_TYPE_NIF_ETH:
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, flags, &phy_port /** zero based */ ));

                SHR_IF_ERR_EXIT(dnx_cosq_ingress_db.fixed_priority.nif.pbmp_member(unit, phy_port, is_fixed));
                break;
            case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
                SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_get(unit, port, &mapped_interface_id));
                if (mapped_interface_id != DNX_ALGO_PORT_EGR_IF_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
                    SHR_IF_ERR_EXIT(dnx_cosq_ingress_db.fixed_priority.mirror.bit_get(unit, core_id,
                                                                                      mapped_interface_id, is_fixed));
                }
                else
                {
                    /** not mapped yet -- the priority is not fixed */
                    *is_fixed = 0;
                }
                break;
            default:
                /** nothing to do */
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;

}
