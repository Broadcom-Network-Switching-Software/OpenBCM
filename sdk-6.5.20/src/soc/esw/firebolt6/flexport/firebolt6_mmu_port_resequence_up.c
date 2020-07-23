/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  File:        firebolt6_mmu_port_resequence_up.c
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

/*! @fn int soc_firebolt6_mmu_reinit_ct_setting(int unit,
 *              soc_port_resource_t *port_resource_t, int ct_mode,
 *              int ct_profile, int frequency, int oversub_ratio)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to reinitialize the CT settings of a port once it flexes
 *         and changes its configuration
 */
/*int
  soc_firebolt6_mmu_reinit_ct_setting(int                   unit,
  soc_port_resource_t  *port_resource_t,
  soc_asf_mode_e        ct_mode,
  soc_asf_mem_profile_e ct_profile,
  int                   frequency,
  int                   exact_speed,
  int                   oversub_ratio)
  {
  uint64 rval, fldval64;
  int mmu_port;
  int inst;
  soc_reg_t reg0, reg1, reg_ep_low, reg_ep_high;
  uint64 oversub;
  int ct_decode_speed;
  uint64 min_speed, max_speed;
  uint64 ep_credit_low, ep_credit_high;
  uint64 asf_count, fifo_threshold, fifo_depth;

  mmu_port = port_resource_t->mmu_port;
  reg0 = ASF_IPORT_CFGr;
  reg1 = ASF_EPORT_CFGr;
  reg_ep_low = ASF_CREDIT_THRESH_LOr;
  reg_ep_high = ASF_CREDIT_THRESH_HIr;
  inst = mmu_port;
  COMPILER_64_ZERO(oversub); */

/* Below Oversub assignment done for */
/* consistency in argument type. uint64 & int */

/* if (port_resource_t->oversub == 1) {
   COMPILER_64_SET(oversub, 0, 1);
   }*/

/* Set the CT decoded Speed in the ASF_IPORT_CFG register */

/* Obtained from mmu_config_apis & Top flexport */
/* ct_decode_speed = soc_firebolt6_get_ct_class(exact_speed);

   COMPILER_64_ZERO(rval);
   soc_reg64_field32_set(unit, reg0, &rval, ASF_PORT_SPEEDf, ct_decode_speed);
   SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg0, inst, 0, rval));
   */

/* ASF EPORT CFG related fields   .......*/

/* Obtained from MMU Config apis */
/* soc_firebolt6_get_min_max_src_ct_speed(unit, ct_decode_speed, ct_mode,
   ct_profile, &min_speed,
   &max_speed);
   soc_firebolt6_get_asf_xmit_start_count(unit, ct_decode_speed, ct_mode,
   &asf_count); *//* mmu config apis*/
/* CT Fifo Threshold & Depth from michael's e2e ct analysis slides */
/* soc_firebolt6_get_ct_fifo_threshold_depth(unit, ct_decode_speed, ct_mode,
   port_resource_t->oversub,
   &fifo_threshold,
   &fifo_depth);


   COMPILER_64_ZERO(rval);
   if (ct_mode != _SOC_ASF_MODE_SAF) {
   soc_reg64_field_set(unit, reg1, &rval, MIN_SRC_PORT_SPEEDf, min_speed);
   soc_reg64_field_set(unit, reg1, &rval, MAX_SRC_PORT_SPEEDf, max_speed);
   soc_reg64_field_set(unit, reg1, &rval,
   MIN_SRC_PORT_XMIT_CNTf, asf_count);
   COMPILER_64_SET(fldval64, 0, 1);
   soc_reg64_field_set(unit, reg1, &rval, ENABLEf, fldval64);
   soc_reg64_field_set(unit, reg1, &rval, OVERSUBf, oversub);
   soc_reg64_field_set(unit, reg1, &rval, FIFO_THRESHOLDf, fifo_threshold);
   soc_reg64_field_set(unit, reg1, &rval, FIFO_DEPTHf, fifo_depth);
   SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg1, inst, 0, rval));
   }
   */

/* ASF EP Credit Low and High Threshold Values ... */

/*soc_firebolt6_mmu_get_ct_ep_credit_low_high_threshold(
  unit, ct_decode_speed,
  frequency,
  port_resource_t->oversub,
  oversub_ratio,
  &ep_credit_low,
  &ep_credit_high);
  COMPILER_64_ZERO(rval);
  soc_reg64_field_set(unit, reg_ep_low, &rval, THRESHf, ep_credit_low);
  SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg_ep_low, inst, 0, rval));

  COMPILER_64_ZERO(rval);
  soc_reg64_field_set(unit, reg_ep_high, &rval, THRESHf, ep_credit_high);
  SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg_ep_high, inst, 0, rval));


  return SOC_E_NONE;
  } */

/*! @fn int soc_firebolt6_mmu_ctr_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear counters in the CTR block for a given port
 */
int
soc_firebolt6_mmu_ctr_clr(int unit, soc_port_resource_t *port_resource_t)
{

    /*Addressing*/
    int local_mmu_port;
    int pipe_number;
    int cosq_base;
    int cosq_base_mc;
    int port_base;
    int q;
    int index;
    int xpe;
    int ipipes[SOC_MAX_NUM_PIPES], epipes[SOC_MAX_NUM_PIPES];

    soc_mem_t mmu_ctr_ing_drop;
    soc_mem_t mmu_ctr_uc_drop;
    soc_mem_t mmu_ctr_mc_drop;
    soc_mem_t mmu_ctr_wred_drop;
    soc_mem_t mmu_ctr_color_drop;

    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[2];
    int num_nodes;
    int pnum;
    soc_mem_t mmu_ctr_clr_mapping;

    /*Fields*/
    uint32 data_ing_drop[SOC_MAX_MEM_WORDS];
    uint32 data_uc_drop[SOC_MAX_MEM_WORDS];
    uint32 data_mc_drop[SOC_MAX_MEM_WORDS];
    uint32 data_color_drop[SOC_MAX_MEM_WORDS];
    uint32 data_wred_drop[SOC_MAX_MEM_WORDS];

    /*Set up pipe, port, and XPE information*/
    pipe_number = port_resource_t->pipe;
    local_mmu_port = (port_resource_t->mmu_port) % FB6_MMU_PORT_PIPE_OFFSET;


    sal_memset(entry, 0, sizeof(entry));

    local_mmu_port = (port_resource_t->mmu_port) % FB6_MMU_PORT_PIPE_OFFSET;

    pnum = 0;
    mmu_ctr_clr_mapping =  SOC_MEM_UNIQUE_ACC_PIPE(unit,
           SCH_PORT_L0_MAPPING_CONFIG_RAMm , pnum);
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mmu_ctr_clr_mapping, MEM_BLOCK_ALL, local_mmu_port,
                                     entry));
    soc_mem_field_get(unit,  mmu_ctr_clr_mapping, entry, NUM_L0Sf,
                      memfld);
    num_nodes = memfld[0];
    soc_mem_field_get(unit, mmu_ctr_clr_mapping, entry, START_L0_NUMf,
                      memfld);
     cosq_base = (memfld[0]*8);
     cosq_base_mc = (memfld[0]*8);
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port CLR : num_nodes 0x%d cosq_base is %d  \n"), num_nodes,memfld[0]));

    /*Allocate data buffer & write to register(s)
     *------------------------------------------*/

    sal_memset(data_ing_drop, 0, sizeof(data_ing_drop));
    sal_memset(data_uc_drop, 0, sizeof(data_uc_drop));
    sal_memset(data_mc_drop, 0, sizeof(data_mc_drop));
    sal_memset(data_wred_drop, 0, sizeof(data_wred_drop));
    sal_memset(data_color_drop, 0, sizeof(data_color_drop));

    xpe = 0;

    /*ING DROP*/
    soc_firebolt6_mmu_get_valid_ipipes_for_xpe(unit, xpe, ipipes);
    if (ipipes[pipe_number] == 1) {
        mmu_ctr_ing_drop = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_CTR_ING_DROP_MEMm, pipe_number);
        index = local_mmu_port;
        SOC_IF_ERROR_RETURN(soc_mem_write(unit,
                    mmu_ctr_ing_drop, MEM_BLOCK_ANY, index, data_ing_drop));
    }

    /*UC DROP*/
    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, xpe, epipes);
    if (epipes[pipe_number] == 1) {
        mmu_ctr_uc_drop = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_CTR_UC_DROP_MEMm, pipe_number);
            for(q = 0; q < (num_nodes * 8); q++) {
                index = cosq_base + q;
         LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port CLR : cosq_index is %d  \n"), index));
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_ctr_uc_drop,
                            MEM_BLOCK_ANY, index,
                            data_uc_drop));
            }
    }

    /*MC DROP*/
    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, xpe, epipes);
    if (epipes[pipe_number] == 1) {
        mmu_ctr_mc_drop = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_CTR_MC_DROP_MEMm, pipe_number);
            for(q = 0; q < (num_nodes * 8 ); q++) {
                index = cosq_base_mc + q;
           LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port CLR : cosq_index_mc is %d  \n"), index));
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_ctr_mc_drop,
                            MEM_BLOCK_ANY, index,
                            data_mc_drop));
            }
    }

    /*WRED DROP*/
    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, xpe, epipes);
    if (epipes[pipe_number] == 1) {
        mmu_ctr_wred_drop = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_CTR_WRED_DROP_MEMm, pipe_number);
            for(q = 0; q < (num_nodes * 8) ; q++) {
                index = cosq_base_mc + q;
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_ctr_wred_drop,
                            MEM_BLOCK_ANY, index,
                            data_wred_drop));
            }
    }

    /*COLOR DROP*/
    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, xpe, epipes);
    if (epipes[pipe_number] == 1) {
        mmu_ctr_color_drop = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_CTR_COLOR_DROP_MEMm, pipe_number);
        port_base = local_mmu_port;
        for(q = 0; q < FB6_MMU_NUM_PORT_COLOR_DROP_ENTRIES; q++) {
            index = port_base + (q * FB6_MMU_MAX_PORTS_PER_PIPE);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_ctr_color_drop,
                        MEM_BLOCK_ANY, index,
                        data_color_drop));
        }
    }

    return SOC_E_NONE;

}

/*! @fn int soc_firebolt6_mmu_clear_mtro_bucket_mems(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear MTRO bucket count memories to ensure that a port
 *         doesn't start out of profile in the new configuration.
 * Refer to Flexport uArch for memory names.
 */
int
soc_firebolt6_mmu_clear_mtro_bucket_mems(int                  unit,
        soc_port_resource_t *port_resource_t)
{
    int mmu_port;
    int pipe_number;
    uint32 entry_mtro_prt_bkt[SOC_MAX_MEM_WORDS];
    uint32 entry_mtro_l0_bkt[SOC_MAX_MEM_WORDS];
    uint32 entry_mtro_l1_bkt[SOC_MAX_MEM_WORDS];
    uint32 entry_mtro_l2_bkt[SOC_MAX_MEM_WORDS];
 /*   uint32 entry_mtro_l3_bkt[SOC_MAX_MEM_WORDS]; */
    int num_l0_entries, num_l1_entries, num_l3_uc_entries, num_l3_mc_entries;
    uint32 mem_indx;
    int lcl_port;
    int i;
    int l0_offset,l1_offset,l3_offset_uc, l3_offset_mc;
    uint32 mem_fld;
    soc_mem_t mem_mtro_prt_bkt, mem_mtro_l0_bkt, mem_mtro_l1_bkt;


    mmu_port = port_resource_t->mmu_port;
    pipe_number = port_resource_t->pipe;


    mem_mtro_prt_bkt = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            MMU_MTRO_EGRMETERINGBUCKET_MEMm, pipe_number);
    mem_mtro_l0_bkt  = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            MMU_MTRO_BUCKET_L0_MEMm, pipe_number);
    mem_mtro_l1_bkt  = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            MMU_MTRO_BUCKET_L1_MEMm, pipe_number);
    /*mem_mtro_l2_bkt  = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            MMU_MTRO_BUCKET_L2_MEMm, pipe_number);
    mem_mtro_l3_bkt  = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            MMU_MTRO_BUCKET_L3_MEMm, pipe_number);*/

    sal_memset(entry_mtro_prt_bkt, 0, sizeof(entry_mtro_prt_bkt));
    sal_memset(entry_mtro_l0_bkt, 0, sizeof(entry_mtro_l0_bkt));
    sal_memset(entry_mtro_l1_bkt, 0, sizeof(entry_mtro_l1_bkt));
    sal_memset(entry_mtro_l2_bkt, 0, sizeof(entry_mtro_l2_bkt));
  /*  sal_memset(entry_mtro_l3_bkt, 0, sizeof(entry_mtro_l3_bkt)); */

    lcl_port = mmu_port % FB6_MMU_PORT_PIPE_OFFSET;
    soc_firebolt6_mmu_get_num_l0_nodes_per_port(unit, lcl_port, pipe_number,
            &num_l0_entries);
    soc_firebolt6_mmu_get_num_l1_nodes_per_port(unit, lcl_port, pipe_number,
            &num_l1_entries);
  /*  soc_firebolt6_mmu_get_num_l2_nodes_per_port(unit, lcl_port, pipe_number,
   *         &num_l2_entries);
   */
    soc_firebolt6_mmu_get_l0_base_offset_for_port(unit, lcl_port, &l0_offset);
    soc_firebolt6_mmu_get_l1_base_offset_for_port(unit, lcl_port, &l1_offset);
    /*soc_firebolt6_mmu_get_l2_base_offset_for_port(unit, lcl_port, &l2_offset); */

    soc_firebolt6_mmu_get_num_l2_uc_nodes_per_port(unit, lcl_port, pipe_number,
            &num_l3_uc_entries);
    soc_firebolt6_mmu_get_num_l2_mc_nodes_per_port(unit, lcl_port, pipe_number,
            &num_l3_mc_entries);

    soc_firebolt6_mmu_get_l2_base_offset_for_port(unit, lcl_port, 1,
            &l3_offset_uc);
    soc_firebolt6_mmu_get_l2_base_offset_for_port(unit, lcl_port, 0,
            &l3_offset_mc);

    /* PORT BUCKET Memory */
    mem_indx= lcl_port;

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_prt_bkt, MEM_BLOCK_ALL,
                                    mem_indx, entry_mtro_prt_bkt));
    mem_fld = 0;
    soc_mem_field_set(unit, mem_mtro_prt_bkt, entry_mtro_prt_bkt, BUCKETf,
                      &mem_fld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_prt_bkt, MEM_BLOCK_ALL,
                                      mem_indx, entry_mtro_prt_bkt));

    /* L0 Bucket Memory */
    for (i=0; i<num_l0_entries; i++) {
        mem_indx= l0_offset+i;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_l0_bkt, MEM_BLOCK_ALL,
                                        mem_indx, entry_mtro_l0_bkt));
        mem_fld = 0;
        soc_mem_field_set(unit, mem_mtro_l0_bkt, entry_mtro_l0_bkt, MIN_BUCKETf,
                &mem_fld);
        soc_mem_field_set(unit, mem_mtro_l0_bkt, entry_mtro_l0_bkt, MAX_BUCKETf,
                &mem_fld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_l0_bkt, MEM_BLOCK_ALL,
                    mem_indx, entry_mtro_l0_bkt));
    }

    /* L1 Bucket Memory */
    for (i=0; i<num_l1_entries; i++) {
        mem_indx= l1_offset+i;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_l1_bkt, MEM_BLOCK_ALL,
                                        mem_indx, entry_mtro_l1_bkt));
        mem_fld = 0;
        soc_mem_field_set(unit, mem_mtro_l1_bkt, entry_mtro_l1_bkt, MIN_BUCKETf,
                &mem_fld);
        soc_mem_field_set(unit, mem_mtro_l1_bkt, entry_mtro_l1_bkt, MAX_BUCKETf,
                &mem_fld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_l1_bkt, MEM_BLOCK_ALL,
                    mem_indx, entry_mtro_l1_bkt));
    }

    /* L2 Bucket Memory */
/*    for (i=0; i<num_l2_entries; i++) {
        mem_indx= l2_offset+i;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_l2_bkt, MEM_BLOCK_ALL,
                                        mem_indx, entry_mtro_l2_bkt));
        mem_fld = 0;
        soc_mem_field_set(unit, mem_mtro_l2_bkt, entry_mtro_l2_bkt, MIN_BUCKETf,
                &mem_fld);
        soc_mem_field_set(unit, mem_mtro_l2_bkt, entry_mtro_l2_bkt, MAX_BUCKETf,
                &mem_fld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_l2_bkt, MEM_BLOCK_ALL,
                                          mem_indx, entry_mtro_l2_bkt));
    }*/


    /* L3 Bucket Memory ----> UC , MC */
/*    for (j=0; j<num_l3_uc_entries; j++) {
        mem_indx=  l3_offset_uc+j;
        mem_indx1= l3_offset_mc+j; */
        /* UC -----  */
/*        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_l3_bkt, MEM_BLOCK_ALL,
                                        mem_indx, entry_mtro_l3_bkt));
        mem_fld = 0;
        soc_mem_field_set(unit, mem_mtro_l3_bkt, entry_mtro_l3_bkt, MIN_BUCKETf,
                &mem_fld);
        soc_mem_field_set(unit, mem_mtro_l3_bkt, entry_mtro_l3_bkt, MAX_BUCKETf,
                &mem_fld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_l3_bkt, MEM_BLOCK_ALL,
                    mem_indx, entry_mtro_l3_bkt));*/

        /* MC ------ */
/*        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_l3_bkt, MEM_BLOCK_ALL,
                                        mem_indx1, entry_mtro_l3_bkt));
        mem_fld = 0;
        soc_mem_field_set(unit, mem_mtro_l3_bkt, entry_mtro_l3_bkt, MIN_BUCKETf,
                &mem_fld);
        soc_mem_field_set(unit, mem_mtro_l3_bkt, entry_mtro_l3_bkt, MAX_BUCKETf,
                &mem_fld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_l3_bkt, MEM_BLOCK_ALL,
                    mem_indx1, entry_mtro_l3_bkt));

    }*/

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_clear_prev_ep_credits(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear the Previous EP credits on a port going down.
 *         Register is MMU_PORT_CREDIT.
 */
int
soc_firebolt6_mmu_clear_prev_ep_credits(int                  unit,
        soc_port_resource_t *port_resource_t)
{
    uint64 rval, fldval64;
    uint64 rd_val;
    int inst;
    int mmu_port;
    int count=0;
    uint64 initialize, init_credit;

    soc_reg_t port_credit_reg  = MMU_PORT_CREDITr;

    mmu_port = port_resource_t->mmu_port;
    inst = mmu_port;
    COMPILER_64_SET(initialize, 0, 1);
    COMPILER_64_ZERO(init_credit);

    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, port_credit_reg, inst, 0,
                &rval));
    soc_reg64_field_set(unit, port_credit_reg, &rval,
            INITIALIZEf, initialize);
    soc_reg64_field_set(unit, port_credit_reg, &rval,
            INIT_CREDITf, init_credit);
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, port_credit_reg, inst, 0,
                rval));

    while (1) {
        SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit, port_credit_reg, inst, 0,
                    &rval));
        rd_val = soc_reg64_field_get(unit, port_credit_reg, rval,
                CURRENT_CREDITf);
        if (COMPILER_64_IS_ZERO(rd_val)) {
            COMPILER_64_ZERO(fldval64);
            soc_reg64_field_set(unit, port_credit_reg, &rval, INITIALIZEf, fldval64);
            SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, port_credit_reg, inst,
                        0, rval));
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 10000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Current Credit isn't reset even after 1000 "
                                "iterations")));
            return SOC_E_FAIL;
        }
    }


    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_reinit_dip_fsaf_setting(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to reinitialize a port's dip and fsaf setting once it is
 *         flexed and back up
 */
/*int
  soc_firebolt6_mmu_reinit_dip_fsaf_setting(int                  unit,
  soc_port_resource_t *port_resource_t)
  {
  uint64 rval;
  int pipe_num;
  uint32 enable_val;
  int base_index;
  int speed;
  uint64 dip_delay_amount;
  int mmu_port;
  uint64 set_val;
  int lcl_mmu_port;
  soc_reg_t dip_reg, fsaf_hp0_reg, fsaf_hp1_reg;
  soc_reg_t dip_reg;
  dip_reg      = MMU_3DBG_Cr;
  fsaf_hp0_reg = MMU_1DBG_A_HP0r;
  fsaf_hp1_reg = MMU_1DBG_A_HP1r;

  COMPILER_64_SET(set_val, 0, 1);

  pipe_num = port_resource_t->pipe;
  speed = port_resource_t->speed;
  base_index = pipe_num;
  mmu_port = port_resource_t->mmu_port;

  soc_firebolt6_mmu_return_dip_delay_amount(
  unit, speed, &dip_delay_amount);

  FORCE SAF is not supported in FB6
  COMPILER_64_SHL(set_val, lcl_mmu_port);

  lcl_mmu_port = mmu_port % FB6_MMU_PORT_PIPE_OFFSET;
  COMPILER_64_ZERO(rval);
  enable_val = COMPILER_64_LO(set_val);
  soc_reg64_field32_set(unit, fsaf_hp0_reg, &rval, FIELD_Af, enable_val);
  SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, fsaf_hp0_reg,
  base_index, 0, rval));

  COMPILER_64_ZERO(rval);
  enable_val = COMPILER_64_HI(set_val);
  soc_reg64_field32_set(unit, fsaf_hp1_reg, &rval, FIELD_Af, enable_val);
  SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, fsaf_hp1_reg,
  base_index, 0, rval));

  Accessing Dip Insertion Register whose bt is Eport

  base_index = mmu_port;

  COMPILER_64_ZERO(rval);
  soc_reg64_field_set(unit, dip_reg, &rval, FIELD_Af, dip_delay_amount);
  SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, dip_reg,
  base_index, 0, rval));

  return SOC_E_NONE;
  }*/

/*! @fn int soc_firebolt6_mmu_thdi_bst_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear port/PG hdrm and shared BST counters for a given MMU
 *         source port
 */
int
soc_firebolt6_mmu_thdi_bst_clr(int unit, soc_port_resource_t *port_resource_t)
{

    /*Variables*/
    int local_mmu_port;
    int pipe_number;
    int port_pg_index;
    int port_sp_index;
    uint32 data[SOC_MAX_MEM_WORDS];
    int pg;
    int sp;
    int xpe;
    int ipipes[SOC_MAX_NUM_PIPES];

    /*Memories*/
    soc_mem_t mmu_thdi_port_pg_bst;
    soc_mem_t mmu_thdi_port_sp_bst;

    /*Set up pipe, port, and XPE information*/
    pipe_number = port_resource_t->pipe;
    local_mmu_port = (port_resource_t->mmu_port) % FB6_MMU_PORT_PIPE_OFFSET;

    sal_memset(data, 0, sizeof(data));

    xpe = 0;

    soc_firebolt6_mmu_get_valid_ipipes_for_xpe(unit, xpe, ipipes);
    if (ipipes[pipe_number] == 1) {
        mmu_thdi_port_pg_bst = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                THDI_PORT_PG_BSTm, pipe_number);
        mmu_thdi_port_sp_bst = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                THDI_PORT_SP_BSTm, pipe_number);

        /*Per-PG loop*/
        for(pg=0; pg < FB6_MMU_NUM_PGS; pg++) {
            /*get port pg index from pipe, local_mmu_port and i*/
            port_pg_index = (local_mmu_port * FB6_MMU_NUM_PGS) + pg;
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_thdi_port_pg_bst,
                        MEM_BLOCK_ANY, port_pg_index,
                        data));
        }
        /*Per-SP loop*/
        for(sp=0; sp < FB6_MMU_NUM_SPS; sp++) {
            /*get port sp Index from mmu_local_port */
            port_sp_index = (local_mmu_port * FB6_MMU_NUM_SPS) + sp;
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_thdi_port_sp_bst,
                        MEM_BLOCK_ANY, port_sp_index,
                        data));
        }
    }

    return SOC_E_NONE;

}

/*! @fn int soc_firebolt6_mmu_thdi_setup(int unit,
  soc_port_resource_t *port_resource_t, int lossy)
 *  @param unit Device number
 *  @param *port_resource_t Port resource struct
 *  @param lossy If the thresholds are to be reconfigured in lossy
 *          or lossless mode
 *  @brief API to reset THDI port-PG thresholds for a given source port
 */
int
soc_firebolt6_mmu_thdi_setup(int unit, soc_port_resource_t *port_resource_t,
        int lossy)
{

    /*Variables*/
    int local_mmu_port;
    int pg;
    int pipe;
    uint32 pg_min_setting;
    uint32 pg_hdrm_setting;
    int port_pg_index;

    /*Memories*/
    soc_mem_t thdi_port_pg_config;
    uint32 port_pg_config_data[SOC_MAX_MEM_WORDS];
    pipe = port_resource_t->pipe;
    thdi_port_pg_config = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            THDI_PORT_PG_CONFIGm, pipe);

    sal_memset(port_pg_config_data, 0, sizeof(port_pg_config_data));

    pg_min_setting =
        (!lossy) ? FB6_MMU_THDI_PG_MIN_LIMIT_LOSSLESS :
        FB6_MMU_THDI_PG_MIN_LIMIT_LOSSY;
    if (!lossy) {
        SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdi_get_pg_hdrm_setting(unit,
                    port_resource_t->speed,
                    (1 - port_resource_t->oversub),
                    &pg_hdrm_setting));
    } else {
        pg_hdrm_setting = FB6_MMU_THDI_PG_HDRM_LIMIT_LOSSY;
    }

    local_mmu_port = (port_resource_t->mmu_port) % FB6_MMU_PORT_PIPE_OFFSET;
    for(pg = 0; pg < FB6_MMU_NUM_PGS; pg++) {
        /* get port pg index from pipe, local_mmu_port and pg; */
        port_pg_index =  (local_mmu_port * FB6_MMU_NUM_PGS) + pg;
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdi_port_pg_config,
                    MEM_BLOCK_ANY, port_pg_index,
                    port_pg_config_data));
        soc_mem_field_set(unit, thdi_port_pg_config, port_pg_config_data,
                PG_MIN_LIMITf,
                &pg_min_setting);
        soc_mem_field_set(unit, thdi_port_pg_config, port_pg_config_data,
                PG_HDRM_LIMITf,
                &pg_hdrm_setting);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdi_port_pg_config,
                    MEM_BLOCK_ANY, port_pg_index,
                    port_pg_config_data));
    }

    return SOC_E_NONE;

}

/*! @fn int soc_firebolt6_mmu_thdm_bst_clr_db_queue(int unit, int pipe,
 *              int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int mmu_port local mmu port
 *  @brief Clears thdm db queue bst counters for all queues in the port
 */
int
soc_firebolt6_mmu_thdm_bst_clr_db_queue(int unit, int pipe, int mmu_port)
{

    int curr_idx;
    int num_queues;
    int mem_idx;
    int epipes[SOC_MAX_NUM_PIPES];
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdm_db_queue_bst_mem;

    sal_memset(data, 0, sizeof(data));

    num_queues =
        FB6_MMU_IS_CPU_PORT(pipe,
                mmu_port) ? FB6_MMU_NUM_MCQ_FOR_CPU_PORT : (mmu_port < FB6_MMU_NUM_UP_PORTS) ?
        FB6_MMU_NUM_MCQ_PER_HSP_UP_PORT : FB6_MMU_NUM_MCQ_PER_HSP_DL_PORT;

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdm_db_queue_bst_mem= SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDM_DB_QUEUE_BSTm, pipe);
        mem_idx =
            FB6_MMU_IS_LPBK_PORT(mmu_port) ? (FB6_MMU_MCQ_NUMBER_START + FB6_MMU_NUM_MCQ_FOR_CPU_PORT) :
            (mmu_port < FB6_MMU_NUM_UP_PORTS) ? mmu_port * FB6_MMU_NUM_MCQ_PER_HSP_UP_PORT :
            FB6_MMU_NUM_UP_PORTS*FB6_MMU_NUM_MCQ_PER_HSP_UP_PORT +
            (mmu_port-FB6_MMU_NUM_UP_PORTS)*FB6_MMU_NUM_MCQ_PER_HSP_DL_PORT;

        for (curr_idx = 0; curr_idx < num_queues; curr_idx++)
        {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdm_db_queue_bst_mem,
                        MEM_BLOCK_ANY, mem_idx, data));
            mem_idx++;
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdm_bst_clr_qe_queue(int unit,
 *                                    int pipe, int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int mmu_port local mmu port
 *  @brief Clears thdm qe queue bst counters for all queues in the port
 */
int
soc_firebolt6_mmu_thdm_bst_clr_qe_queue(int unit, int pipe, int mmu_port)
{

    int curr_idx;
    int num_queues;
    int mem_idx;
    int epipes[SOC_MAX_NUM_PIPES];
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdm_qe_queue_bst_mem;

    sal_memset(data, 0, sizeof(data));

    num_queues =
        FB6_MMU_IS_CPU_PORT(pipe,
                mmu_port) ? FB6_MMU_NUM_MCQ_FOR_CPU_PORT : (mmu_port < FB6_MMU_NUM_UP_PORTS) ?
        FB6_MMU_NUM_MCQ_PER_HSP_UP_PORT : FB6_MMU_NUM_MCQ_PER_HSP_DL_PORT;

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdm_qe_queue_bst_mem= SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDM_MCQE_QUEUE_BSTm, pipe);
        mem_idx =
            FB6_MMU_IS_LPBK_PORT(mmu_port) ? (FB6_MMU_MCQ_NUMBER_START + FB6_MMU_NUM_MCQ_FOR_CPU_PORT) :
            (mmu_port < FB6_MMU_NUM_UP_PORTS) ? mmu_port * FB6_MMU_NUM_MCQ_PER_HSP_UP_PORT :
            FB6_MMU_NUM_UP_PORTS*FB6_MMU_NUM_MCQ_PER_HSP_UP_PORT +
            (mmu_port-FB6_MMU_NUM_UP_PORTS)*FB6_MMU_NUM_MCQ_PER_HSP_DL_PORT;

        for (curr_idx = 0; curr_idx < num_queues; curr_idx++)
        {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdm_qe_queue_bst_mem,
                        MEM_BLOCK_ANY, mem_idx, data));
            mem_idx++;
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdm_bst_clr_db_port(int unit,
 *                                   int pipe, int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int mmu_port local mmu port
 *  @brief Clears thdm db port bst counters of the port
 */
int
soc_firebolt6_mmu_thdm_bst_clr_db_port(int unit, int pipe, int mmu_port)
{

    int curr_idx;
    int mem_idx;
    uint32 data[SOC_MAX_MEM_WORDS];
    int epipes[SOC_MAX_NUM_PIPES];
    soc_mem_t thdm_db_portsp_bst_mem;

    sal_memset(data, 0, sizeof(data));

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdm_db_portsp_bst_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDM_DB_PORTSP_BSTm, pipe);
        mem_idx = mmu_port * FB6_MMU_NUM_SPS;
        for (curr_idx = 0; curr_idx < FB6_MMU_NUM_SPS; curr_idx++)
        {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit,
                        thdm_db_portsp_bst_mem,
                        MEM_BLOCK_ANY, mem_idx, data));
            mem_idx++;
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdm_bst_clr_qe_port(int unit,
 *                                   int pipe, int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int mmu_port local mmu port
 *  @brief Clears thdm qe port bst counters of the port
 */
int
soc_firebolt6_mmu_thdm_bst_clr_qe_port(int unit, int pipe, int mmu_port)
{

    int curr_idx;
    int mem_idx;
    int epipes[SOC_MAX_NUM_PIPES];
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdm_qe_portsp_bst_mem;

    sal_memset(data, 0, sizeof(data));

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdm_qe_portsp_bst_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDM_MCQE_PORTSP_BSTm, pipe);
        mem_idx = mmu_port * FB6_MMU_NUM_SPS;
        for (curr_idx = 0; curr_idx < FB6_MMU_NUM_SPS; curr_idx++)
        {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit,
                        thdm_qe_portsp_bst_mem,
                        MEM_BLOCK_ANY, mem_idx, data));
            mem_idx++;
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdm_bst_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param soc_port_resource_t *port_resource_t Port resource struct containing
 epipe and port info
 *  @brief Clears thdm qe/db queue/port bst counters of an egress port
 */
int
soc_firebolt6_mmu_thdm_bst_clr(int unit, soc_port_resource_t *port_resource_t)
{
    int local_mmu_port;
    int pipe;

    pipe = port_resource_t->pipe;
    local_mmu_port = port_resource_t->mmu_port % FB6_MMU_PORT_PIPE_OFFSET;

    /* Clear db queue */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdm_bst_clr_db_queue(unit, pipe,
                local_mmu_port));
    /* Clear qe queue */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdm_bst_clr_qe_queue(unit, pipe,
                local_mmu_port));
    /* Clear db port */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdm_bst_clr_db_port(unit, pipe,
                local_mmu_port));
    /* Clear qe port */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdm_bst_clr_qe_port(unit, pipe,
                local_mmu_port));

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdu_bst_clr_queue(int unit,
 *                                 int pipe, int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int mmu_port Local mmu port
 *  @brief Internal function. Clears all thdu queue bst counters
 *          of an egress port
 */
int
soc_firebolt6_mmu_thdu_bst_clr_queue(int unit, int pipe, int mmu_port)
{

    int curr_idx;
    int mem_idx;
    int epipes[SOC_MAX_NUM_PIPES];
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdu_bst_queue_mem;

    sal_memset(data, 0, sizeof(data));

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdu_bst_queue_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDU_BST_QUEUEm, pipe);
        mem_idx = (mmu_port < FB6_MMU_NUM_UP_PORTS) ? mmu_port * FB6_MMU_THDU_UCQ_PER_UP_PORT :
            FB6_MMU_NUM_UP_PORTS*FB6_MMU_THDU_UCQ_PER_UP_PORT +
            (mmu_port-FB6_MMU_NUM_UP_PORTS)*FB6_MMU_THDU_UCQ_PER_DL_PORT;
        if(mmu_port < FB6_MMU_NUM_UP_PORTS) {
            for (curr_idx = 0; curr_idx < FB6_MMU_THDU_UCQ_PER_UP_PORT; curr_idx++)
            {
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdu_bst_queue_mem,
                            MEM_BLOCK_ANY, mem_idx, data));
                mem_idx++;
            }
        }
        else {
            for (curr_idx = 0; curr_idx < FB6_MMU_THDU_UCQ_PER_DL_PORT; curr_idx++)
            {
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdu_bst_queue_mem,
                            MEM_BLOCK_ANY, mem_idx, data));
                mem_idx++;
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdu_bst_clr_qgrp(int unit,
 *                                int pipe, int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int mmu_port Local mmu port
 *  @brief Internal function. Clears thdu qgroup bst counter of an egress port
 */
int
soc_firebolt6_mmu_thdu_bst_clr_qgrp(int unit, int pipe, int mmu_port)
{

    int mem_idx;
    uint32 data[SOC_MAX_MEM_WORDS];
    int epipes[SOC_MAX_NUM_PIPES];
    soc_mem_t thdu_bst_qgrp_mem;

    sal_memset(data, 0, sizeof(data));

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdu_bst_qgrp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDU_BST_QGROUPm, pipe);
        mem_idx = mmu_port;
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdu_bst_qgrp_mem,
                    MEM_BLOCK_ANY, mem_idx, data));
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdu_bst_clr_port(int unit,
 *                                int pipe, int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int mmu_port Local mmu port
 *  @brief Internal function. Clears thdu port bst counters (all four service
 *         pools) of an egress port
 */
int
soc_firebolt6_mmu_thdu_bst_clr_port(int unit, int pipe, int mmu_port)
{

    int curr_idx;
    int mem_idx;
    int epipes[SOC_MAX_NUM_PIPES];
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdu_bst_port_mem;

    sal_memset(data, 0, sizeof(data));

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdu_bst_port_mem= SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDU_BST_PORTm, pipe);
        mem_idx = mmu_port * FB6_MMU_NUM_SPS;
        for (curr_idx = 0; curr_idx < FB6_MMU_NUM_SPS; curr_idx++)
        {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdu_bst_port_mem,
                        MEM_BLOCK_ANY, mem_idx, data));
            mem_idx++;
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdu_bst_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param soc_port_resource_t *port_resource_t Port resource struct containing
 *         epipe and port info
 *  @brief Clears all thdu queue/qGrp/port bst counters of an egress port
 */
int
soc_firebolt6_mmu_thdu_bst_clr(int unit, soc_port_resource_t *port_resource_t)
{
    int local_mmu_port;
    int pipe;

    pipe = port_resource_t->pipe;
    local_mmu_port = port_resource_t->mmu_port % FB6_MMU_PORT_PIPE_OFFSET;

    /* Clear queue */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdu_bst_clr_queue(unit, pipe,
                local_mmu_port));
    /* Clear qGrp */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdu_bst_clr_qgrp(unit, pipe,
                local_mmu_port));
    /* Clear port */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdu_bst_clr_port(unit, pipe,
                local_mmu_port));
    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdu_pktstat_clr_queue(int unit, int pipe,
 *              int mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int mmu_port Local mmu port
 *  @brief Internal function. Clears thdu ucq stats counters of all queues
 *         in the egress port
 */
int
soc_firebolt6_mmu_thdu_pktstat_clr_queue(int unit, int pipe, int mmu_port)
{

    int curr_idx;
    int mem_idx;
    uint32 data[SOC_MAX_MEM_WORDS];
    int epipes[SOC_MAX_NUM_PIPES];
    soc_mem_t thdu_ucq_stats_mem;

    sal_memset(data, 0, sizeof(data));

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, 0, epipes);
    if (epipes[pipe] == 1) {
        thdu_ucq_stats_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_THDU_UCQ_STATSm, pipe);
        mem_idx = (mmu_port < FB6_MMU_NUM_UP_PORTS) ? mmu_port * FB6_MMU_THDU_UCQ_PER_UP_PORT :
            FB6_MMU_NUM_UP_PORTS*FB6_MMU_THDU_UCQ_PER_UP_PORT +
            (mmu_port-FB6_MMU_NUM_UP_PORTS)*FB6_MMU_THDU_UCQ_PER_DL_PORT;
        if(mmu_port < FB6_MMU_NUM_UP_PORTS) {
            for (curr_idx = 0; curr_idx < FB6_MMU_THDU_UCQ_PER_UP_PORT; curr_idx++)
            {
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdu_ucq_stats_mem,
                            MEM_BLOCK_ANY, mem_idx, data));
                mem_idx++;
            }
        }
        else {
            for (curr_idx = 0; curr_idx < FB6_MMU_THDU_UCQ_PER_DL_PORT; curr_idx++)
            {
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdu_ucq_stats_mem,
                            MEM_BLOCK_ANY, mem_idx, data));
                mem_idx++;
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdu_pktstat_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param soc_port_resource_t *port_resource_t Port resource struct containing
 *         epipe and port info
 *  @brief Clears thdu ucq stats counters of all queues in the egress port
 */
int
soc_firebolt6_mmu_thdu_pktstat_clr(int                  unit,
        soc_port_resource_t *port_resource_t)
{
    int local_mmu_port;
    int pipe;


    pipe = port_resource_t->pipe;
    local_mmu_port = port_resource_t->mmu_port % FB6_MMU_PORT_PIPE_OFFSET;

    /* Clear queue */
    SOC_IF_ERROR_RETURN(soc_firebolt6_mmu_thdu_pktstat_clr_queue(unit, pipe,
                local_mmu_port));
    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_thdu_qgrp_min_limit_config(int unit,
 *              soc_port_resource_t *port_resource_t, int lossy)
 *  @param int unit Chip unit number
 *  @param soc_port_resource_t *port_resource_t Port resource struct containing
 *             epipe and port info
 *  @param int lossy port in lossy mode
 *  @brief Program qgroup min limit of the port based on lossy/loseless mode
 */
int
soc_firebolt6_mmu_thdu_qgrp_min_limit_config(
        int unit, soc_port_resource_t *port_resource_t, int lossy)
{
    uint32 qgrp_min_limit;
    int local_mmu_port;
    int pipe;
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdu_config_qgrp_mem;

    pipe = port_resource_t->pipe;
    local_mmu_port = port_resource_t->mmu_port % FB6_MMU_PORT_PIPE_OFFSET;
    thdu_config_qgrp_mem= SOC_MEM_UNIQUE_ACC_PIPE(unit,
            MMU_THDU_CONFIG_QGROUPm, pipe);

    if (lossy) {
        qgrp_min_limit = FB6_MMU_THDU_QGRP_MIN_LIMIT_LOSSY;
    }
    else {
        qgrp_min_limit = FB6_MMU_THDU_QGRP_MIN_LIMIT_LOSELESS;
    }
    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdu_config_qgrp_mem,
                MEM_BLOCK_ANY, local_mmu_port, data));
    soc_mem_field_set(unit, thdu_config_qgrp_mem, data,
            Q_MIN_LIMIT_CELLf,
            &qgrp_min_limit);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdu_config_qgrp_mem,
                MEM_BLOCK_ANY, local_mmu_port, data));
    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_clear_vbs_credit_memories(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear VBS credit memories and accump cnt memories for the
 *         ports that are flexing during flexport.
 * For exact memory names, refer to Flexport uArch Spec.
 */
int
soc_firebolt6_mmu_clear_vbs_credit_memories(
        int unit,soc_port_resource_t *port_resource_t)
{

    int mmu_port;
    int pipe_number;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_cr_l0[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_cr_l1[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_cr_l2[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_cr_l3[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_acc_l0[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_acc_l1[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_acc_l2[SOC_MAX_MEM_WORDS];
    uint32 entry_vbs_acc_l3[SOC_MAX_MEM_WORDS];

    int num_l0_entries, num_l1_entries,num_l3_uc_entries;
    uint32 mem_indx;
    /*uint32 mem_indx_toq_b_uc, mem_indx_toq_b_mc;*/
    int lcl_port;
    int i;
    int l0_offset,l1_offset,l3_offset_uc, l3_offset_mc;
    uint32 mem_fld;
    soc_mem_t vbs_l0_credit_mem;
    soc_mem_t vbs_l1_credit_mem;
/*    soc_mem_t vbs_l2_credit_mem; */

    soc_mem_t vbs_l0_accum_comp_mem;
    soc_mem_t vbs_l1_accum_comp_mem;
/*    soc_mem_t vbs_l2_accum_comp_mem; */


    mmu_port = port_resource_t->mmu_port;
    pipe_number = port_resource_t->pipe;

    vbs_l0_credit_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            Q_SCHED_L0_CREDIT_MEMm, pipe_number);
    vbs_l1_credit_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            Q_SCHED_L1_CREDIT_MEMm, pipe_number);
/*    vbs_l2_credit_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
 *           Q_SCHED_L2_CREDIT_MEMm, pipe_number);
 */
 /*   vbs_l3_credit_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            Q_SCHED_L3_CREDIT_MEMm, pipe_number);*/

    vbs_l0_accum_comp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            Q_SCHED_L0_ACCUM_COMP_MEMm, pipe_number);
    vbs_l1_accum_comp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            Q_SCHED_L1_ACCUM_COMP_MEMm, pipe_number);

  /*  vbs_l2_accum_comp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
   *         Q_SCHED_L2_ACCUM_COMP_MEMm, pipe_number);
  */
 /*   vbs_l3_accum_comp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
            Q_SCHED_L3_ACCUM_COMP_MEMm, pipe_number);*/

    sal_memset(entry_vbs_cr_l0, 0, sizeof(entry_vbs_cr_l0));
    sal_memset(entry_vbs_cr_l1, 0, sizeof(entry_vbs_cr_l1));
    sal_memset(entry_vbs_cr_l2, 0, sizeof(entry_vbs_cr_l2));
    sal_memset(entry_vbs_cr_l3, 0, sizeof(entry_vbs_cr_l3));
    sal_memset(entry_vbs_acc_l0,   0, sizeof(entry_vbs_acc_l0));
    sal_memset(entry_vbs_acc_l1,   0, sizeof(entry_vbs_acc_l1));
    sal_memset(entry_vbs_acc_l2,   0, sizeof(entry_vbs_acc_l2));
    sal_memset(entry_vbs_acc_l3,   0, sizeof(entry_vbs_acc_l3));

    sal_memset(entry, 0, sizeof(entry));

    lcl_port = mmu_port % FB6_MMU_PORT_PIPE_OFFSET;

    soc_firebolt6_mmu_get_num_l0_nodes_per_port(unit, lcl_port, pipe_number,
            &num_l0_entries);
    soc_firebolt6_mmu_get_num_l1_nodes_per_port(unit, lcl_port, pipe_number,
            &num_l1_entries);
/*    soc_firebolt6_mmu_get_num_l2_nodes_per_port(unit, lcl_port, pipe_number,
 *           &num_l2_entries);
 */
    soc_firebolt6_mmu_get_l0_base_offset_for_port(unit, lcl_port, &l0_offset);
    soc_firebolt6_mmu_get_l1_base_offset_for_port(unit, lcl_port, &l1_offset);
/*    soc_firebolt6_mmu_get_l2_base_offset_for_port(unit, lcl_port, &l2_offset); */
    soc_firebolt6_mmu_get_num_l2_uc_nodes_per_port(unit, lcl_port, pipe_number,
            &num_l3_uc_entries);
    soc_firebolt6_mmu_get_l2_base_offset_for_port(unit, lcl_port, 1,
            &l3_offset_uc);
    soc_firebolt6_mmu_get_l2_base_offset_for_port(unit, lcl_port, 0,
            &l3_offset_mc);

    LOG_DEBUG(BSL_LS_SOC_PORT,
               (BSL_META_U(unit,
                      "MMU DEBUG mmu_port %1d lcl_port %1d pipe_num %1d num_l0_entries=%1d \n"),
              mmu_port,
               lcl_port, pipe_number, num_l0_entries));
    /* L0 entries*/
    for (i=0; i<num_l0_entries; i++) {
        mem_indx= l0_offset+i;

        mem_fld = 0;
        soc_mem_field_set(unit, vbs_l0_credit_mem, entry_vbs_cr_l0,
                ACCUM_COMP_CNTf,
                &mem_fld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, vbs_l0_credit_mem,
                    MEM_BLOCK_ALL, mem_indx,
                    entry_vbs_cr_l0));

        soc_mem_field_set(unit, vbs_l0_accum_comp_mem, entry_vbs_acc_l0,
                ACCUM_COMP_CNTf,
                &mem_fld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, vbs_l0_accum_comp_mem,
                    MEM_BLOCK_ALL, mem_indx,
                    entry_vbs_acc_l0));


    }

    /* L1 entries*/
    for (i=0; i<num_l1_entries; i++) {
        mem_indx= l1_offset+i;

        mem_fld = 0;
        soc_mem_field_set(unit, vbs_l1_credit_mem, entry_vbs_cr_l1,
                ACCUM_COMP_CNTf,
                &mem_fld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, vbs_l1_credit_mem,
                    MEM_BLOCK_ALL, mem_indx,
                    entry_vbs_cr_l1));

        soc_mem_field_set(unit, vbs_l1_accum_comp_mem, entry_vbs_acc_l1,
                ACCUM_COMP_CNTf,
                &mem_fld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, vbs_l1_accum_comp_mem,
                    MEM_BLOCK_ALL, mem_indx,
                    entry_vbs_acc_l1));


    }

    return SOC_E_NONE;
}

/*! @fn int soc_firebolt6_mmu_wred_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to set cap_average bit in the WRED block for a given destination
 *         port
 */
int
soc_firebolt6_mmu_wred_clr(int unit, soc_port_resource_t *port_resource_t)
{
    /*Variables*/
    soc_mem_t mmu_wred_config;
    int local_mmu_port;
    int cosq_base;
    int cosq_index;
    int portsp_index;
    int portsp_base ;

    uint32 wred_config_data[SOC_MAX_MEM_WORDS];
    uint32 cap_avg = 1;
    int q,sp;
    int num_nodes;
    int xpe, pipe;
    int epipes[SOC_MAX_NUM_PIPES];
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld[2];
    soc_mem_t mmu_wred_clr_mapping;
    int pipe_number;

    sal_memset(entry, 0, sizeof(entry));

    local_mmu_port = (port_resource_t->mmu_port) % FB6_MMU_PORT_PIPE_OFFSET;
    portsp_base = FB6_MMU_NUM_UCQ + (local_mmu_port * FB6_MMU_NUM_SPS);

    pipe_number = 0;
    mmu_wred_clr_mapping =  SOC_MEM_UNIQUE_ACC_PIPE(unit,
           SCH_PORT_L0_MAPPING_CONFIG_RAMm , pipe_number);
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mmu_wred_clr_mapping, MEM_BLOCK_ALL, local_mmu_port,
                                     entry));
    soc_mem_field_get(unit, mmu_wred_clr_mapping, entry, NUM_L0Sf,
                      memfld);
    num_nodes = memfld[0];
    soc_mem_field_get(unit, mmu_wred_clr_mapping, entry, START_L0_NUMf,
                      memfld);
     cosq_base = (memfld[0] * 8);
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port CLR %d : num_nodes 0x%d cosq_base is %d  \n"),local_mmu_port, num_nodes,memfld[0]));


    pipe = port_resource_t->pipe;
    sal_memset(wred_config_data, 0, sizeof(wred_config_data));

    xpe = 0;

    soc_firebolt6_mmu_get_valid_epipes_for_xpe(unit, xpe, epipes);
    if (epipes[pipe] == 1) {
        mmu_wred_config = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                MMU_WRED_CONFIGm, pipe);

            /*Per-Q loop*/
            for(q = 0; q < (num_nodes * 8 ) ; q++) {
                cosq_index = cosq_base + q;
         LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port CLR : cosq_index is %d  \n"), cosq_index));
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mmu_wred_config,
                            MEM_BLOCK_ANY, cosq_index,
                            wred_config_data));
                soc_mem_field_set(unit, mmu_wred_config, wred_config_data,
                        CAP_AVERAGEf, &cap_avg);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_wred_config,
                            MEM_BLOCK_ANY, cosq_index,
                            wred_config_data));
            }


    /*Per-SP loop*/
            for(sp = 0; sp < FB6_MMU_NUM_SPS; sp++) {
                portsp_index = portsp_base + sp;
             LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "SDKDBG MMU Port CLR : portsp_index is %d  \n"), portsp_index));
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mmu_wred_config,
                                    MEM_BLOCK_ANY, portsp_index,
                                    wred_config_data));
                soc_mem_field_set(unit, mmu_wred_config, wred_config_data,
                                  CAP_AVERAGEf, &cap_avg);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_wred_config,
                                                  MEM_BLOCK_ANY,
                                                  portsp_index,
                                                  wred_config_data));
            }
    }


    return SOC_E_NONE;
}
#endif
