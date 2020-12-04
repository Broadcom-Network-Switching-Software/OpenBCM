/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  maverick2_mmu_port_up_sequence.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_MAVERICK2_SUPPORT)
#include <soc/maverick2.h>
#include <soc/maverick2.h>
/*! @file maverick2_mmu_port_up_sequence.c
 *  @brief
 */

#include <soc/flexport/maverick2/maverick2_flexport.h>
#include <soc/flexport/maverick2/maverick2_mmu_defines.h>
#include <soc/flexport/maverick2/maverick2_mmu_flex_init.h>


/*! @fn int soc_maverick2_mmu_port_up_sequence(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule Struct
 *  @brief API to update all MMU related functionalities during flexport
 *         port up operation.
 */
int
soc_maverick2_flex_mmu_reconfigure_phase2(
    int unit, soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    int logical_port;
    int oversub_ratio;
    int exact_port_speed;
    int lossy;
    int bst_mode;
    int pktstat_mode;
    soc_maverick2_flex_scratch_t *cookie;
    cookie = port_schedule_state_t->cookie;

    lossy= !(port_schedule_state_t->lossless);

    soc_maverick2_mmu_get_bst_mode(unit, &bst_mode);
    soc_maverick2_mmu_get_pktstat_mode(unit, &pktstat_mode);
    

    /* Per-Port configuration */
    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            
	    oversub_ratio =
                cookie->prev_pipe_ovs_ratio[port_schedule_state_t->resource[
                                                port].
                                            pipe];
            logical_port = port_schedule_state_t->resource[port].logical_port;

            exact_port_speed= cookie->exact_out_log_port_speed[logical_port];

            /* Re-adjust phy port mapping for valid ports */
            soc_maverick2_mmu_set_mmu_to_phy_port_mapping(
                unit, &port_schedule_state_t->resource[port]);

            /*  Reinitialize CT setting */   
            soc_maverick2_mmu_reinit_ct_setting(
                unit, &port_schedule_state_t->resource[port],
                port_schedule_state_t->cutthru_prop.asf_modes[logical_port],
                port_schedule_state_t->cutthru_prop.asf_mem_prof,
                port_schedule_state_t->frequency,
                exact_port_speed, oversub_ratio);

            /* Clear MTRO bucket memories */
            soc_maverick2_mmu_clear_mtro_bucket_mems(
                unit, &port_schedule_state_t->resource[port]);
            /* Clear VBS credit memories*/
            soc_maverick2_mmu_clear_vbs_credit_memories(
                unit, &port_schedule_state_t->resource[port]);
            /* Clear WRED Avg_Qsize instead of waiting for background process*/
            soc_maverick2_mmu_wred_clr(unit,
                                       &port_schedule_state_t->resource[port]);
            soc_maverick2_mmu_thdi_setup(unit,
                                         &port_schedule_state_t->resource[port],
                                         lossy);
            soc_maverick2_mmu_thdu_qgrp_min_limit_config(
                unit, &port_schedule_state_t->resource[port], lossy);
            /* Clear Drop Counters in CTR block*/
            soc_maverick2_mmu_ctr_clr(unit,
                                      &port_schedule_state_t->resource[port]);
	    if (bst_mode == MV2_MMU_BST_WMARK_MODE) {
	        /* Clear THDI BST in watermark mode..*/
                soc_maverick2_mmu_thdi_bst_clr(
                    unit, &port_schedule_state_t->resource[port]);
                /* Clear THDU BST counters in watermark mode*/
                soc_maverick2_mmu_thdu_bst_clr(
                    unit, &port_schedule_state_t->resource[port]);
                /* Clear THDM BST counters in watermark mode*/
                soc_maverick2_mmu_thdm_bst_clr(
                    unit, &port_schedule_state_t->resource[port]);
	    }
	    if (pktstat_mode == MV2_PKTSTATS_ON) {
                /* Clear PktStat counters in THDU for Queues*/
                soc_maverick2_mmu_thdu_pktstat_clr(
                    unit, &port_schedule_state_t->resource[port]); 
            }
            

            if (port_schedule_state_t->resource[port].oversub == 1) {
                /* DIP FSAF Reconfigure setting */
                soc_maverick2_mmu_reinit_dip_fsaf_setting(
                    unit, &port_schedule_state_t->resource[port]);
            }
        } else {
	     soc_maverick2_mmu_set_mmu_to_phy_port_mapping(
                unit, &port_schedule_state_t->resource[port]);
        }
    }

    return SOC_E_NONE;
}


int
soc_maverick2_flex_mmu_port_up(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    int pipe;
    int pipe_flexed;
    uint64 temp64;
      
    COMPILER_64_SET(temp64, 0, MV2_MMU_FLUSH_OFF);

    /* Per-Port configuration */
    for (pipe = 0; pipe < MAVERICK2_TDM_PIPES_PER_DEV; pipe++) {
        soc_maverick2_mmu_get_pipe_flexed_status(
	    unit, port_schedule_state_t, pipe, &pipe_flexed);
	    
	if (pipe_flexed == 1) {
	    soc_maverick2_mmu_vbs_port_flush(unit, port_schedule_state_t, pipe, temp64);
	    soc_maverick2_mmu_rqe_port_flush(unit, pipe, temp64);
	}
    }
    

    /* Per-Pipe configuration */

    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            /* Clear Previous EP Credits on the port. */
            soc_maverick2_mmu_clear_prev_ep_credits(
                unit, &port_schedule_state_t->resource[port]);
            soc_maverick2_mmu_mtro_port_flush(
                unit, &port_schedule_state_t->resource[port], temp64);
        }
    }
    return SOC_E_NONE;
}


#endif /* BCM_MAVERICK2_SUPPORT */
