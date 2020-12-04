/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  trident3_mmu_port_down_sequence.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#include <soc/trident3.h>
#include <soc/trident3_tdm.h>
/*! @file trident3_mmu_port_down_sequence.c
 *  @brief
 */

#include <soc/flexport/trident3/trident3_flexport.h>
#include <soc/flexport/trident3/trident3_mmu_defines.h>
#include <soc/flexport/trident3/trident3_mmu_flex_init.h>


/*! @fn int soc_trident3_mmu_port_down_sequence(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to update all MMU related functionalities during flexport
 *         port down operation.
 */
int
soc_trident3_flex_mmu_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    uint64 temp64;
    int pipe;
    int pipe_flexed;

    COMPILER_64_SET(temp64, 0, TD3_MMU_FLUSH_ON);

    /* Per-Pipe configuration */
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        soc_trident3_mmu_get_pipe_flexed_status(
        unit, port_schedule_state_t, pipe, &pipe_flexed);

        if (pipe_flexed == 1) {
            soc_trident3_mmu_vbs_port_flush(unit, port_schedule_state_t, pipe,
                                    temp64);
        }
    }

    /* Per-Port configuration */
    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port == -1) {

            /*  Set CT Enable to 0 */
            /* soc_trident3_mmu_disable_ct(
               unit, &port_schedule_state_t->resource[port]);
            */

            soc_trident3_mmu_mtro_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);
        }
    }

    /* Per-Pipe configuration */
    for (pipe = 0; pipe < TRIDENT3_TDM_PIPES_PER_DEV; pipe++) {
        soc_trident3_mmu_get_pipe_flexed_status(
        unit, port_schedule_state_t, pipe, &pipe_flexed);

        if (pipe_flexed == 1) {
            soc_trident3_mmu_rqe_port_flush(unit, pipe, temp64);
            sal_usleep(TD3_CT_FIFO_WAIT +
                       (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_trident3_mmu_reconfig_tdm(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t) {
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to reconfigure TDM calendars for MMU.
 */
int
soc_trident3_flex_mmu_reconfigure_phase1(
    int unit, soc_port_schedule_state_t *port_schedule_state_t)
{
    /*soc_trident3_tdm_flexport_remove_ports(unit, port_schedule_state_t);*/
    soc_trident3_tdm_flexport_remove_ports_sel(unit, port_schedule_state_t, 0, 1);
    /*soc_trident3_tdm_flexport_ovs_consolidate(unit, port_schedule_state_t);*/
    soc_trident3_tdm_flexport_ovs_consolidate_sel(unit, port_schedule_state_t, 0, 1);
    /*soc_trident3_tdm_flexport_add_ports(unit, port_schedule_state_t);*/
    soc_trident3_tdm_flexport_add_ports_sel(unit, port_schedule_state_t, 0, 1);
    _soc_td3_tdm_set_mmu_calendar(unit, port_schedule_state_t);
    _soc_td3_tdm_set_mmu_hsp(unit, port_schedule_state_t);
    return SOC_E_NONE;
}


#endif /* BCM_TRIDENT3_SUPPORT */
