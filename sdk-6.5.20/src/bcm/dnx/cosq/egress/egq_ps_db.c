/**
 * \file bcm/dnx/cosq/egress/egq_ps_db.c
 *
 * General Description:
 *
 * Purpose:
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <soc/dnx/swstate/auto_generated/access/dnx_egr_db_access.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

int
dnx_ps_db_base_qpair_alloc_with_id(
    int unit,
    bcm_port_t port,
    int port_interface,
    int is_channelized,
    bcm_core_t core,
    int out_port_priority,
    int base_q_pair)
{

    int ps_interface;
    uint32 allocation_bmp[1];
    uint32 prio_mode, ps, already_allocated, prio;

    SHR_FUNC_INIT_VARS(unit);
    ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    prio = base_q_pair % DNX_EGR_NOF_Q_PAIRS_IN_PS;
    port_interface = is_channelized ? port_interface : DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES;
    SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.get(unit, core, ps, allocation_bmp));
    SHR_IF_ERR_EXIT(dnx_egr_db.ps.if_idx.get(unit, core, ps, &ps_interface));
    SHR_IF_ERR_EXIT(dnx_egr_db.ps.prio_mode.get(unit, core, ps, &prio_mode));
    SHR_BITTEST_RANGE(allocation_bmp, prio, out_port_priority, already_allocated);
    if (allocation_bmp[0] == 0
        || (!already_allocated && prio_mode == out_port_priority && port_interface == ps_interface))
    {
        SHR_BITSET_RANGE(allocation_bmp, prio, out_port_priority);
        SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.set(unit, core, ps, allocation_bmp[0]));
        SHR_IF_ERR_EXIT(dnx_egr_db.ps.if_idx.set(unit, core, ps, port_interface));
        SHR_IF_ERR_EXIT(dnx_egr_db.ps.prio_mode.set(unit, core, ps, out_port_priority));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to allocate with-id qpairs for port %d", port);
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ps_db_base_qpair_alloc(
    int unit,
    bcm_port_t port,
    int port_interface,
    int is_channelized,
    bcm_core_t core,
    int out_port_priority,
    int priority_propagation_enable,
    int *base_q_pair)
{

    int ps_interface;
    uint32 allocation_bmp[1], combined_allocation_bmp[1];
    uint32 prio_mode, ps, already_allocated, prio;
    uint32 allocated_hrs[1];
    int used_sch_priority;
    int used_priority_propagation_enable;

    SHR_FUNC_INIT_VARS(unit);

    *base_q_pair = -1;
    port_interface = is_channelized ? port_interface : DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES;
    /*
     * stage 1 - find allocated PS
     */
    for (ps = 0; ps < DNX_EGR_NOF_PS; ps++)
    {
        SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.get(unit, core, ps, allocation_bmp));

        SHR_IF_ERR_EXIT(dnx_egr_db.ps.if_idx.get(unit, core, ps, &ps_interface));
        SHR_IF_ERR_EXIT(dnx_egr_db.ps.prio_mode.get(unit, core, ps, &prio_mode));
        if (allocation_bmp[0] && prio_mode == out_port_priority && port_interface == ps_interface)
        {
            /*
             * In this mode (allocation without id) assuming that HR == qpair
             * Therefore, we need to avoid from allocating qpair equals to already allocated HR
             */
            SHR_IF_ERR_EXIT(dnx_sch_allocated_hrs_get(unit, core, ps, allocated_hrs,
                                                      &used_sch_priority, &used_priority_propagation_enable));
            if (used_sch_priority == 0 ||
                (priority_propagation_enable == used_priority_propagation_enable &&
                 used_sch_priority == out_port_priority))
            {
                SHR_BITOR_RANGE(allocation_bmp, allocated_hrs, 0, DNX_EGR_NOF_Q_PAIRS_IN_PS, combined_allocation_bmp);

                for (prio = 0; prio < DNX_EGR_NOF_Q_PAIRS_IN_PS; prio += out_port_priority)
                {
                    SHR_BITTEST_RANGE(combined_allocation_bmp, prio, out_port_priority, already_allocated);

                    if (!already_allocated)
                    {
                        SHR_BITSET_RANGE(allocation_bmp, prio, out_port_priority);
                        SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.set(unit, core, ps, allocation_bmp[0]));
                        *base_q_pair = ps * DNX_EGR_NOF_Q_PAIRS_IN_PS + prio;
                        break;
                    }
                }
                if (*base_q_pair != -1)
                {
                    break;
                }
            }
        }
    }
    /*
     * stage 2 - allocate new PS
     */
    if (*base_q_pair == -1)
    {
        for (ps = 0; ps < DNX_EGR_NOF_PS; ps++)
        {
            SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.get(unit, core, ps, allocation_bmp));
            if (allocation_bmp[0] == 0)
            {
                /*
                 * In this mode (allocation without id) assuming that HR == qpair
                 * Therefore, we need to avoid from allocating qpair equals to already allocated HR
                 */
                SHR_IF_ERR_EXIT(dnx_sch_allocated_hrs_get(unit, core, ps, allocated_hrs,
                                                          &used_sch_priority, &used_priority_propagation_enable));
                if (used_sch_priority == 0 || (priority_propagation_enable == used_priority_propagation_enable &&
                                               used_sch_priority == out_port_priority))
                {
                    SHR_BITOR_RANGE(allocation_bmp, allocated_hrs, 0, DNX_EGR_NOF_Q_PAIRS_IN_PS,
                                    combined_allocation_bmp);

                    for (prio = 0; prio < DNX_EGR_NOF_Q_PAIRS_IN_PS; prio += out_port_priority)
                    {
                        SHR_BITTEST_RANGE(combined_allocation_bmp, prio, out_port_priority, already_allocated);

                        if (!already_allocated)
                        {
                            SHR_BITSET_RANGE(allocation_bmp, prio, out_port_priority);
                            SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.set(unit, core, ps, allocation_bmp[0]));
                            SHR_IF_ERR_EXIT(dnx_egr_db.ps.if_idx.set(unit, core, ps, port_interface));
                            SHR_IF_ERR_EXIT(dnx_egr_db.ps.prio_mode.set(unit, core, ps, out_port_priority));
                            *base_q_pair = ps * DNX_EGR_NOF_Q_PAIRS_IN_PS + prio;
                            break;
                        }
                    }
                    if (*base_q_pair != -1)
                    {
                        break;
                    }
                }
            }
        }
    }
    if (*base_q_pair == -1)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "no free PS port %d", port);
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnx_ps_db_base_qpair_free(
    int unit,
    bcm_port_t port,
    int base_q_pair,
    int *free_ps)
{
    uint32 allocation_bmp[1], prio_mode, ps, prio;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    prio = base_q_pair % DNX_EGR_NOF_Q_PAIRS_IN_PS;
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.get(unit, core, ps, allocation_bmp));
    SHR_IF_ERR_EXIT(dnx_egr_db.ps.prio_mode.get(unit, core, ps, &prio_mode));
    if (allocation_bmp[0])
    {
        SHR_BITCLR_RANGE(allocation_bmp, prio, prio_mode);
        SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.set(unit, core, ps, allocation_bmp[0]));
    }
    if (allocation_bmp[0])
    {
        *free_ps = FALSE;
    }
    else
    {
        *free_ps = TRUE;
    }
exit:
    SHR_FUNC_EXIT;
}
