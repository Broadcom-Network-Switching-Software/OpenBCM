/** \file port_ingr_reassembly_alloc.c
 * 
 *
 * Port Ingress reassembly context/port termination context allocation/deallocation
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/mirror/mirror_rcy.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include "port_ingr_reassembly_utils.h"
#include "port_ingr_reassembly.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_reassembly_access.h>

#define PORT_INGR_REASSEMBLY_CONTEXT_ALLOC_MNG_ID "PORT_INGR_REASSEMBLY_CONTEXT"

typedef enum
{
    DNX_PORT_INGR_REASSEMBLY_ALLOC = 0,
    DNX_PORT_INGR_REASSEMBLY_PORT_DEALLOC,
    DNX_PORT_INGR_REASSEMBLY_PRIORITY_DEALLOC
} dnx_port_ingr_reassembly_alloc_action;

/** ---------------------------------------------------------------------------------------------------------------- */
/*
 *   Allocators 
 */

/**
 * \brief - allocate reassembly context for the port
 */
shr_error_e
dnx_port_ingr_reassembly_should_call_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    int *call_alloc)
{
    dnx_algo_port_info_s port_info;
    uint32 reassembly_context = 0, port_termination_context;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
    {
        /**
         * for Mirror Recycle port, alloc should be called each time the port is mapped, even if it has context already.
         * for Recycle port, alloc should be called each time since context can be updated according to priority.
 */
        *call_alloc = 1;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get(unit, port, priority,
                                                                                     &reassembly_context,
                                                                                     &port_termination_context));

        /** call alloc for port which does not have reassembly context yet */
        *call_alloc = (reassembly_context == dnx_data_ingr_reassembly.context.invalid_context_get(unit));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - is_required parameter gets TRUE iff 
 *          *  new reassembly context should be allocated for port in case of is_alloc=1 
 *          *  or should be deallocated in case of is_alloc=0
 */
static shr_error_e
dnx_port_ingr_reassembly_nif_eth_alloc_dealloc_required(
    int unit,
    bcm_port_t port,
    int priority,
    dnx_port_ingr_reassembly_alloc_action action,
    int *is_required)
{
    uint32 reassembly_context, port_termination_context;
    bcm_port_t next_master_port;
    int handle_tdm;
    uint32 tdm_flags;

    SHR_FUNC_INIT_VARS(unit);

    switch (action)
    {
        case DNX_PORT_INGR_REASSEMBLY_ALLOC:

            /*
             * all NIF ETH channels are mapped to the same place in Context Map Table
             * read the HW to understand whether the context already exists
             */

            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get(unit, port, priority,
                                                                                         &reassembly_context,
                                                                                         &port_termination_context));
            *is_required = (reassembly_context == dnx_data_ingr_reassembly.context.invalid_context_get(unit));
            break;
        case DNX_PORT_INGR_REASSEMBLY_PORT_DEALLOC:

            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_should_handle_tdm(unit, port, &handle_tdm));

            tdm_flags = (handle_tdm ? 0 : DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS);

            /** deallocate  context if this is the last port */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT | tdm_flags,
                                                     &next_master_port));
            *is_required = (next_master_port == DNX_ALGO_PORT_INVALID);
            break;

            break;
        case DNX_PORT_INGR_REASSEMBLY_PRIORITY_DEALLOC:
            *is_required = 1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected action %d", action);
            break;

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - is_required parameter gets TRUE if
 *          *  new reassembly context should be allocated for port in case of is_alloc=1 
 *          *  or should be deallocated in case of is_alloc=0
 */
static shr_error_e
dnx_port_ingr_reassembly_rcy_alloc_dealloc_required(
    int unit,
    bcm_port_t port,
    int priority,
    dnx_port_ingr_reassembly_alloc_action action,
    int *is_required)
{
    bcm_port_t next_master_port;
    int handle_tdm;
    uint32 tdm_flags;
    int core_id;
    int interface_index;
    uint32 reassembly_context;

    SHR_FUNC_INIT_VARS(unit);

    switch (action)
    {
        case DNX_PORT_INGR_REASSEMBLY_ALLOC:
            /*
             * RCY contexts are stored in SW state.
             * read the SW State to understand whether the context already exists
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_index));
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.rcy.
                            reassembly_context.get(unit, core_id, interface_index, 0, &reassembly_context));

            *is_required = (reassembly_context == dnx_data_ingr_reassembly.context.invalid_context_get(unit));
            break;

        case DNX_PORT_INGR_REASSEMBLY_PORT_DEALLOC:
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_should_handle_tdm(unit, port, &handle_tdm));
            tdm_flags = (handle_tdm ? 0 : DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS);

            /** deallocate context if this is the last port */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT | tdm_flags,
                                                     &next_master_port));
            *is_required = (next_master_port == DNX_ALGO_PORT_INVALID);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected action %d", action);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - is_required parameter gets TRUE iff 
 *          *  new reassembly context should be allocated for port in case of is_alloc=1 
 *          *  or should be deallocated in case of is_alloc=0
 */
static shr_error_e
dnx_port_ingr_reassembly_non_interleaved_alloc_dealloc_required(
    int unit,
    bcm_port_t port,
    int priority,
    dnx_port_ingr_reassembly_alloc_action action,
    int *is_required)
{
    bcm_port_t master_port, next_master_port;
    int handle_tdm;
    uint32 tdm_flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * General rule:
     * allocate if this is the first port and deallocate if this is the last port
     */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_should_handle_tdm(unit, port, &handle_tdm));

    tdm_flags = (handle_tdm ? 0 : DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS);

    switch (action)
    {
        case DNX_PORT_INGR_REASSEMBLY_ALLOC:
            /** allocate new context if this is the first (master) port */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, tdm_flags, &master_port));
            *is_required = (port == master_port);
            break;

        case DNX_PORT_INGR_REASSEMBLY_PORT_DEALLOC:
            /** deallocate context if this is the last port */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT | tdm_flags,
                                                     &next_master_port));
            *is_required = (next_master_port == DNX_ALGO_PORT_INVALID);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected action %d", action);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - is_required parameter gets TRUE if 
 *          *  new reassembly context should be allocated for port in case of is_alloc=1
 *          *  or should be deallocated in case of is_alloc=0
 */
static shr_error_e
dnx_port_ingr_reassembly_alloc_dealloc_required(
    int unit,
    bcm_port_t port,
    int priority,
    dnx_port_ingr_reassembly_alloc_action action,
    int *is_required)
{
    dnx_algo_port_info_s port_info;
    int is_interleaved;

    SHR_FUNC_INIT_VARS(unit);

    *is_required = 0;

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /** Special cases -- exceptions */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, 0) || DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, 0))
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_nif_eth_alloc_dealloc_required(unit, port, priority,
                                                                                action, is_required));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_rcy_alloc_dealloc_required(unit, port, priority, action, is_required));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_is_interface_interleaved(unit, port, &is_interleaved));
        if (is_interleaved)
        {
            /*
             * Dedicated TDM feature is not currently supported in JR2
             * as there are enough reassembly contexts
             */
            /** allocate/deallocate for interleaved interface */
            *is_required = 1;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_non_interleaved_alloc_dealloc_required(unit, port, priority,
                                                                                            action, is_required));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_port_ingr_reassembly_nof_contexts_per_rcy_if_get(
    int unit,
    int *nof_contexts_per_rcy_if)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_ingr_reassembly.
        context.feature_get(unit, dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities))
    {
        *nof_contexts_per_rcy_if = dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get(unit);
    }
    else
    {
        *nof_contexts_per_rcy_if = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate reassembly context for RCY port
 */
static shr_error_e
dnx_port_ingr_reassembly_rcy_reassembly_context_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *reassembly_context)
{
    int core_id = 0;
    int temp_prio;
    uint32 temp_context;
    int new_context_required;
    int alloc_flags = 0;
    int interface_index;
    int nof_contexts_per_rcy_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_index));

    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_alloc_dealloc_required(unit, port, priority,
                                                                    DNX_PORT_INGR_REASSEMBLY_ALLOC,
                                                                    &new_context_required));

    if (new_context_required)
    {
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_nof_contexts_per_rcy_if_get(unit, &nof_contexts_per_rcy_if));

        /** need to allocate new contexts */
        for (temp_prio = 0; temp_prio < nof_contexts_per_rcy_if; temp_prio++)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.allocate_single(unit, core_id, alloc_flags, NULL,
                                                                            (int *) &temp_context));
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.rcy.
                            reassembly_context.set(unit, core_id, interface_index, temp_prio, temp_context));
        }
    }

    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.rcy.
                    reassembly_context.get(unit, core_id, interface_index, priority, reassembly_context));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deallocate reassembly context for rcy port
 */
static shr_error_e
dnx_port_ingr_reassembly_rcy_reassembly_context_dealloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 reassembly_context,
    int *is_deallocated)
{

    int core_id = 0;
    int temp_prio;
    uint32 temp_context;
    uint32 invalid_context;
    int interface_index;
    int nof_contexts_per_rcy_if;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * check whether context should be deallocated 
     *
     * deallocate  context for each port in interleaved interface or
     * for last port in the interface
     */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_alloc_dealloc_required(unit, port, priority,
                                                                    DNX_PORT_INGR_REASSEMBLY_PORT_DEALLOC,
                                                                    is_deallocated));

    if (*is_deallocated)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_index));
        SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_nof_contexts_per_rcy_if_get(unit, &nof_contexts_per_rcy_if));

        /** De-Allocate context */
        for (temp_prio = 0; temp_prio < nof_contexts_per_rcy_if; temp_prio++)
        {
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.rcy.
                            reassembly_context.get(unit, core_id, interface_index, temp_prio, &temp_context));
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.free_single(unit, core_id, temp_context));

            /** mark RCY interface not to have context */
            invalid_context = dnx_data_ingr_reassembly.context.invalid_context_get(unit);
            SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.rcy.
                            reassembly_context.set(unit, core_id, interface_index, temp_prio, invalid_context));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate reassembly context for mirror port
 */
static shr_error_e
dnx_port_ingr_reassembly_mirror_reassembly_context_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *reassembly_context)
{
    int mapped_egress_interface;
    uint32 ref_count;
    int core_id;

    int alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_get(unit, port, &mapped_egress_interface));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.mirror.info.reassembly_context.get(unit, core_id,
                                                                                 mapped_egress_interface, priority,
                                                                                 reassembly_context));

    if (*reassembly_context == dnx_data_ingr_reassembly.context.invalid_context_get(unit))
    {
        /** need to allocate new context */
        SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.allocate_single(unit, core_id, alloc_flags, NULL,
                                                                        (int *) reassembly_context));
        SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.mirror.info.
                        reassembly_context.set(unit, core_id, mapped_egress_interface, priority, *reassembly_context));
    }

    /** increment reference count */
    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.mirror.info.ref_count.get(unit, core_id,
                                                                        mapped_egress_interface, priority, &ref_count));
    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.mirror.info.ref_count.set(unit, core_id,
                                                                        mapped_egress_interface, priority,
                                                                        ref_count + 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deallocate reassembly context for mirror port
 */
static shr_error_e
dnx_port_ingr_reassembly_mirror_reassembly_context_dealloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 reassembly_context,
    int priority_dealloc,
    int *is_deallocated)
{

    int mapped_egress_interface;
    uint32 ref_count;
    int core_id;
    uint32 invalid_context;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Mirror port can't be removed if there are egress ports mapped to it
     * Thus we don't need to take care of deallocating reassembly when port if removed
     */

    SHR_IF_ERR_EXIT(dnx_mirror_port_mapped_egress_interface_get(unit, port, &mapped_egress_interface));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    /** decrement reference count */
    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.mirror.info.ref_count.get(unit, core_id,
                                                                        mapped_egress_interface, priority, &ref_count));
    ref_count--;
    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.mirror.info.ref_count.set(unit, core_id,
                                                                        mapped_egress_interface, priority, ref_count));

    if (ref_count == 0)
    {
        /** De-Allocate context */
        *is_deallocated = 1;

        SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.free_single(unit, core_id, reassembly_context));

        /** mark mirror port not to have context */
        invalid_context = dnx_data_ingr_reassembly.context.invalid_context_get(unit);
        SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.mirror.info.reassembly_context.set(unit, core_id,
                                                                                     mapped_egress_interface, priority,
                                                                                     invalid_context));
    }
    else
    {
        *is_deallocated = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate reassembly context for the port
 */
shr_error_e
dnx_port_ingr_reassembly_reassembly_context_alloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 *reassembly_context)
{

    int core_id = 0;
    int new_context_required;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_RCY:
            /** RCY port  */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_rcy_reassembly_context_alloc(unit,
                                                                                  port, priority, reassembly_context));
            break;
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_mirror_reassembly_context_alloc(unit,
                                                                                     port, priority,
                                                                                     reassembly_context));
            break;
        default:
        /** other port types */

            /*
             * check whether new context should be allocated 
             *
             * allocate new context for each port in interleaved interface or
             * for first (master) port
             */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_alloc_dealloc_required(unit, port, priority,
                                                                            DNX_PORT_INGR_REASSEMBLY_ALLOC,
                                                                            &new_context_required));

            if (new_context_required)
            {
                int alloc_flags = 0;

                /** Allocate new context */
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
                SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.allocate_single(unit, core_id, alloc_flags, NULL,
                                                                                (int *) reassembly_context));

            }
            else
            {
                /** Use the same context */
                uint32 dummy;
                bcm_port_t master_port;
                int handle_tdm;
                uint32 tdm_flags;

                SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_port_should_handle_tdm(unit, port, &handle_tdm));

                tdm_flags = (handle_tdm ? 0 : DNX_ALGO_PORT_MASTER_F_NON_TDM_BYPASS);

                SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, tdm_flags, &master_port));
                /** take the context from the master port */
                SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_reassembly_and_port_termination_get_with_disabled(unit,
                                                                                                           master_port,
                                                                                                           priority,
                                                                                                           TRUE,
                                                                                                           reassembly_context,
                                                                                                           &dummy));
            }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - deallocate reassembly context for the port
 */
shr_error_e
dnx_port_ingr_reassembly_reassembly_context_dealloc(
    int unit,
    bcm_port_t port,
    int priority,
    uint32 reassembly_context,
    int priority_dealloc,
    int *is_deallocated)
{

    int core_id = 0;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_deallocated, _SHR_E_PARAM, "is_deallocated");

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_RCY:
            /** RCY port  -- does not support priority_dealloc */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_rcy_reassembly_context_dealloc(unit, port, priority,
                                                                                    reassembly_context,
                                                                                    is_deallocated));
            break;

        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_mirror_reassembly_context_dealloc(unit, port, priority,
                                                                                       reassembly_context,
                                                                                       priority_dealloc,
                                                                                       is_deallocated));
            break;

        default:
            /** other port types */

            /*
             * check whether context should be deallocated 
             *
             * deallocate  context for each port in interleaved interface or
             * for last port in the interface
             */
            SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_alloc_dealloc_required(unit, port, priority,
                                                                            (priority_dealloc ?
                                                                             DNX_PORT_INGR_REASSEMBLY_PRIORITY_DEALLOC :
                                                                             DNX_PORT_INGR_REASSEMBLY_PORT_DEALLOC),
                                                                            is_deallocated));

            if (*is_deallocated)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

                /** De-Allocate context */
                SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.free_single(unit, core_id, reassembly_context));
            }
            break;
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - mark reassembly context as allocated on all cores
 */
shr_error_e
dnx_port_ingr_reassembly_reassembly_context_mark_allocated(
    int unit,
    uint32 reassembly_context)
{
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** mark context allocated on all cores */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.allocate_single(unit, core_id,
                                                                        DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL,
                                                                        (int *) &reassembly_context));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager used by port_ingr_reassembly dnx module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_ingr_reassembly_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * port_ingr_reassembly nof profiles resource management  
     */
    data.first_element = 0;
    /*
     * last context 0xff is used as invalid context and should not be allocated 
     */
    data.nof_elements = dnx_data_ingr_reassembly.context.nof_contexts_get(unit);

    sal_strncpy(data.name, PORT_INGR_REASSEMBLY_CONTEXT_ALLOC_MNG_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /*
     * data.desc = "Ingress Reassembly Context";
     */
    SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.alloc(unit, dnx_data_device.general.nof_cores_get(unit)));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_reassembly_db.alloc.create(unit, core_id, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}
