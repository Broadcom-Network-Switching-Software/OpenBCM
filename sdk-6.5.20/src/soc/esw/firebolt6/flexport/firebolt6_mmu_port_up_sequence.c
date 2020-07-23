/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  File:        firebolt6_mmu_port_up_sequence.c
 *  Purpose:
 *  Requires:
 */


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#include <soc/firebolt6_tdm.h>
#include <soc/flexport/firebolt6/firebolt6_flexport.h>
#include <soc/flexport/firebolt6/firebolt6_mmu_defines.h>
#include <soc/flexport/firebolt6/firebolt6_mmu_flex_init.h>

/*! @fn int soc_firebolt6_mmu_port_up_sequence(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule Struct
 *  @brief API to update all MMU related functionalities during flexport
 *         port up operation.
 */
int
soc_firebolt6_flex_mmu_reconfigure_phase2(
        int unit, soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    /*  int logical_port;
        int oversub_ratio;
        int exact_port_speed; */
    int lossy;
    int bst_mode;
    int pktstat_mode;
    /*uint64 temp64;*/
    /* soc_firebolt6_flex_scratch_t *cookie;
       cookie = port_schedule_state_t->cookie; */

    lossy= !(port_schedule_state_t->lossless);

    soc_firebolt6_mmu_get_bst_mode(unit, &bst_mode);
    soc_firebolt6_mmu_get_pktstat_mode(unit, &pktstat_mode);

    /*COMPILER_64_SET(temp64, 0, FB6_MMU_FLUSH_OFF);*/

    /* Per-Port configuration */
    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {

            /*oversub_ratio =
              cookie->prev_pipe_ovs_ratio[port_schedule_state_t->resource[
              port].
              pipe];
              logical_port = port_schedule_state_t->resource[port].logical_port;

              exact_port_speed= cookie->exact_out_log_port_speed[logical_port]; */

            /* Re-adjust phy port mapping for valid ports */
            soc_firebolt6_mmu_set_mmu_to_phy_port_mapping(
                    unit, &port_schedule_state_t->resource[port], port_schedule_state_t);

            /*  Reinitialize CT setting */
            /* CT not supported in FB6
             * soc_firebolt6_mmu_reinit_ct_setting(
             unit, &port_schedule_state_t->resource[port],
             port_schedule_state_t->cutthru_prop.asf_modes[logical_port],
             port_schedule_state_t->cutthru_prop.asf_mem_prof,
             port_schedule_state_t->frequency,
             exact_port_speed, oversub_ratio); */

            /* Clear MTRO bucket memories */
            soc_firebolt6_mmu_clear_mtro_bucket_mems(
                    unit, &port_schedule_state_t->resource[port]);
            /* Clear VBS credit memories*/
            soc_firebolt6_mmu_clear_vbs_credit_memories(
                    unit, &port_schedule_state_t->resource[port]);
            /* Clear WRED Avg_Qsize instead of waiting for background process*/
            soc_firebolt6_mmu_wred_clr(unit,
                    &port_schedule_state_t->resource[port]);
            soc_firebolt6_mmu_thdi_setup(unit,
                    &port_schedule_state_t->resource[port],
                    lossy);
            soc_firebolt6_mmu_thdu_qgrp_min_limit_config(
                    unit, &port_schedule_state_t->resource[port], lossy);
            /* Clear Drop Counters in CTR block*/
            soc_firebolt6_mmu_ctr_clr(unit,
                    &port_schedule_state_t->resource[port]);
            if (bst_mode == FB6_MMU_BST_WMARK_MODE) {
                /* Clear THDI BST in watermark mode..*/
                soc_firebolt6_mmu_thdi_bst_clr(
                        unit, &port_schedule_state_t->resource[port]);
                /* Clear THDU BST counters in watermark mode*/
                soc_firebolt6_mmu_thdu_bst_clr(
                        unit, &port_schedule_state_t->resource[port]);
                /* Clear THDM BST counters in watermark mode*/
                soc_firebolt6_mmu_thdm_bst_clr(
                        unit, &port_schedule_state_t->resource[port]);
            }
            if (pktstat_mode == FB6_PKTSTATS_ON) {
                /* Clear PktStat counters in THDU for Queues*/
                soc_firebolt6_mmu_thdu_pktstat_clr(
                        unit, &port_schedule_state_t->resource[port]);
            }


            if (port_schedule_state_t->resource[port].oversub == 1) {
                /* DIP FSAF Reconfigure setting */
                /*soc_firebolt6_mmu_reinit_dip_fsaf_setting(
                  unit, &port_schedule_state_t->resource[port]);*/
            }
        } else {
            soc_firebolt6_mmu_set_mmu_to_phy_port_mapping(
                    unit, &port_schedule_state_t->resource[port], port_schedule_state_t);
        }
    }

    return SOC_E_NONE;
}


int
soc_firebolt6_flex_mmu_port_up(int                        unit,
        soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    int pipe,i;
    int mmu_port , logical_port,physical_port;
    int prev_physical_port, prev_mmu_port;
    int is_coe_port ;
    int pipe_flexed;
    int line_card_num,line_card_num_loop;
    int sub_port_allocated;
    int pipe_number;
    uint64 temp64; 
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mem_fld;
    uint32 entry_vbs_l0_mapping[SOC_MAX_MEM_WORDS];
    soc_mem_t vbs_l0_mapping;
    soc_mem_t rqe_l0_mapping;

    sal_memset(entry, 0, sizeof(entry));
    sal_memset(entry_vbs_l0_mapping, 0, sizeof(entry_vbs_l0_mapping));

   COMPILER_64_SET(temp64, 0, FB6_MMU_FLUSH_OFF); 
      pipe_number = 0;


      vbs_l0_mapping = SOC_MEM_UNIQUE_ACC_PIPE(unit,
           SCH_PORT_L0_MAPPING_CONFIG_RAMm , pipe_number);

      rqe_l0_mapping = SOC_MEM_UNIQUE_ACC_PIPE(unit,
           MMU_RQE_PORT_TO_L0_MAPm , pipe_number);

    /* Per-Port configuration */
    for (pipe = 0; pipe < FIREBOLT6_TDM_PIPES_PER_DEV; pipe++) {
        soc_firebolt6_mmu_get_pipe_flexed_status(
                unit, port_schedule_state_t, pipe, &pipe_flexed);

        if (pipe_flexed == 1) {
    /*LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port UP: tem_64 0x%lx Pipe flex is %d  \n"), pipe_flexed,temp64));*/
            soc_firebolt6_mmu_vbs_port_flush(unit, port_schedule_state_t, pipe, temp64);
            soc_firebolt6_mmu_rqe_port_flush(unit, pipe, temp64);
        }
    }

    /* Per-Pipe configuration */

    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            /* Clear Previous EP Credits on the port. */
            soc_firebolt6_mmu_clear_prev_ep_credits(
                    unit, &port_schedule_state_t->resource[port]);
            soc_firebolt6_mmu_mtro_port_flush(
                    unit, &port_schedule_state_t->resource[port], temp64);
        }
    }

    /* Per-Port configuration for vbs & wred */
    /* clear line card down config & then program entire new line card config */
    for (port=0; port<port_schedule_state_t->nport; port++) {
        	if (port_schedule_state_t->resource[port].physical_port == -1) {
		logical_port = port_schedule_state_t->resource[port].logical_port;
                prev_physical_port =
				port_schedule_state_t->in_port_map.port_l2p_mapping[logical_port];
    		LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port down clr for port %d  \n"),prev_physical_port));
              	prev_mmu_port =
				port_schedule_state_t->in_port_map.port_p2m_mapping[
									prev_physical_port];
        	is_coe_port =
            	     SOC_PBMP_MEMBER(port_schedule_state_t->in_port_map.coe_port_pbm,
							prev_physical_port) ? 1 : 0; 
	        /* need queue clear incase its a coe port */  
		if(is_coe_port == 1) {
    		LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port down coe clr for port %d   \n"),prev_physical_port));
		        mem_fld = 0;
        		soc_mem_field_set(unit, vbs_l0_mapping, entry_vbs_l0_mapping,
               	    	NUM_L0Sf,
               	    	&mem_fld);
        		SOC_IF_ERROR_RETURN(soc_mem_write(unit, vbs_l0_mapping,
                    	MEM_BLOCK_ALL, prev_mmu_port,
                    	entry_vbs_l0_mapping));


        /*		soc_mem_field_set(unit, rqe_l0_mapping, entry_vbs_l0_mapping,
        *       	    	NUM_L0Sf,
        *       	    	&mem_fld);
        *		SOC_IF_ERROR_RETURN(soc_mem_write(unit, rqe_l0_mapping,
        *            	MEM_BLOCK_ALL, prev_mmu_port,
        *            	entry_vbs_l0_mapping));
        */

			}
		}
		else { /* its portup need to configure both num & start addresses */
		logical_port = port_schedule_state_t->resource[port].logical_port;
		physical_port = port_schedule_state_t->resource[port].physical_port;
    		LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port up for port %d num_subports %d  \n"),physical_port,port_schedule_state_t->resource[port].num_subports));
		mmu_port =
			 port_schedule_state_t->out_port_map.port_p2m_mapping[
									physical_port];
        	is_coe_port =
            	     SOC_PBMP_MEMBER(port_schedule_state_t->out_port_map.coe_port_pbm,
							physical_port) ? 1 : 0; 

		if(is_coe_port == 1) {
		  /* locate which node in the linecard to be programmed */
		  sub_port_allocated = 0 ;
		  line_card_num = 
			 port_schedule_state_t->out_port_map.port_lc_mapping[
									physical_port];
		  for(i=1;i< physical_port ; i++){
		     line_card_num_loop = 
			 port_schedule_state_t->out_port_map.port_lc_mapping[i];
		     if(line_card_num_loop == line_card_num )
		     {
		      sub_port_allocated = sub_port_allocated +  
			 port_schedule_state_t->out_port_map.num_subports[i];
		     }
		   }
                	SOC_IF_ERROR_RETURN
                    		(soc_mem_read(unit, vbs_l0_mapping, MEM_BLOCK_ALL, mmu_port,entry_vbs_l0_mapping));
		        mem_fld = 76 + sub_port_allocated  + (line_card_num * 48) ;

        		soc_mem_field_set(unit, vbs_l0_mapping, entry_vbs_l0_mapping,
               	    	START_L0_NUMf,
               	    	&mem_fld);
        		SOC_IF_ERROR_RETURN(soc_mem_write(unit, vbs_l0_mapping,
                    	MEM_BLOCK_ALL, mmu_port,
                    	entry_vbs_l0_mapping));
    		LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port up COE for port %d num_subports %d l0_start_address %d line_card_num %d \n"),physical_port,port_schedule_state_t->resource[port].num_subports,mem_fld,line_card_num));

                	SOC_IF_ERROR_RETURN
                    		(soc_mem_read(unit, vbs_l0_mapping, MEM_BLOCK_ALL, mmu_port,entry_vbs_l0_mapping));
		        mem_fld = port_schedule_state_t->resource[port].num_subports;
        		soc_mem_field_set(unit, vbs_l0_mapping, entry_vbs_l0_mapping,
               	    	NUM_L0Sf,
               	    	&mem_fld);
        		SOC_IF_ERROR_RETURN(soc_mem_write(unit, vbs_l0_mapping,
                    	MEM_BLOCK_ALL, mmu_port,
                    	entry_vbs_l0_mapping));

			soc_mem_field_set(unit, rqe_l0_mapping, entry_vbs_l0_mapping,
               	    	L0_STARTf,
               	    	&mem_fld);
        		SOC_IF_ERROR_RETURN(soc_mem_write(unit, rqe_l0_mapping,
                    	MEM_BLOCK_ALL, mmu_port,
                    	entry_vbs_l0_mapping));

                /*	SOC_IF_ERROR_RETURN
                *    		(soc_mem_read(unit, rqe_l0_mapping, MEM_BLOCK_ALL, mmu_port,entry_vbs_l0_mapping));
		*        mem_fld = port_schedule_state_t->out_port_map.num_subports[physical_port];
        	*	soc_mem_field_set(unit, rqe_l0_mapping, entry_vbs_l0_mapping,
               	*    	L0_STARTf,
               	*    	&mem_fld);
        	*	SOC_IF_ERROR_RETURN(soc_mem_write(unit, rqe_l0_mapping,
                *    	MEM_BLOCK_ALL, mmu_port,
                *    	entry_vbs_l0_mapping));
                */
			
			}

		}


    }

    return SOC_E_NONE;
}
#endif
