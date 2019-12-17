/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * BST - Buffer Statistics Tracking
 * Purpose: API to set and manage various BST resources for TH
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/ll.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/trident3.h>
#include <soc/helix5.h>

#include <bcm/switch.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm_int/bst.h>
#include <bcm_int/esw/port.h>

#if defined(BCM_HELIX5_SUPPORT)
extern int _bcm_bst_td3_byte_to_cell(int unit, uint32 bytes);
extern int _bcm_bst_td3_cell_to_byte(int unit, uint32 cells);
extern int _bcm_td3_bst_control_set(int unit, bcm_switch_control_t type, int arg, 
                                    _bcm_bst_thread_oper_t operation);
extern int _bcm_td3_bst_control_get(int unit, bcm_switch_control_t type, int *arg);
extern int _bcm_bst_td3_intr_to_resources(int unit, uint32 *flags);
extern int _bcm_bst_td3_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                                         int port, int index);
extern int _bcm_bst_td3_hw_stat_clear(int unit, _bcm_bst_resource_info_t *resInfo,
                                      bcm_bst_stat_id_t bid, int port, int index);
extern int _bcm_bst_td3_intr_enable_set(int unit, int enable);
extern int _bst_td3_port_to_mmu_inst_map_get(int unit, bcm_bst_stat_id_t bid,
                                             bcm_gport_t gport, uint32 *mmu_inst_map);
extern int _bcm_bst_td3_sbusdma_desc_init(int unit);
extern int _bcm_bst_td3_sbusdma_desc_deinit(int unit);
extern int _bcm_bst_td3_sw_sbusdma_desc_sync(int unit, int bid);
extern int _bcm_bst_td3_sbusdma_desc_sync(int unit);


STATIC int
soc_hx5_set_hw_intr_cb(int unit)
{
    BCM_IF_ERROR_RETURN(soc_td3_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
    return BCM_E_NONE;
}

int _bcm_bst_hx5_thd_reg_index_get(int unit, bcm_bst_stat_id_t bid, int pipe,
                                    int hw_index, int *inst, int *index)
{
    if (bid == bcmBstStatIdEgrPortPoolSharedMcast) {
        int phy_port, mmu_port;
        soc_info_t *si = &SOC_INFO(unit);
            mmu_port = hw_index / _HX5_MMU_NUM_POOL;
            phy_port = si->port_m2p_mapping[mmu_port];
            *inst = si->port_p2l_mapping[phy_port];
            *index = hw_index % _HX5_MMU_NUM_POOL;
    }

    return BCM_E_NONE;
}

int _bcm_bst_hx5_cmn_io_op(int unit, int op, int pipe, int hw_index,
                      bcm_bst_stat_id_t bid, soc_mem_t mem, soc_reg_t reg,
                      soc_field_t field, uint32 *resval)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    int port = REG_PORT_ANY, reg_index = hw_index;
    _bcm_bst_resource_info_t *resInfo;

    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo->flags & _BCM_BST_CMN_RES_F_PORTED) &&
        (op & BST_OP_THD)) {
        BCM_IF_ERROR_RETURN(
            _bcm_bst_hx5_thd_reg_index_get(unit, bid, pipe, hw_index,
                                      &port, &reg_index));
    }
    if (port != -1) {
        if (mem != INVALIDm) {
            SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, entry));
            if (op & BST_OP_GET) {
                *resval = soc_mem_field32_get(unit, mem, entry, field);
            } else if (op & BST_OP_SET) {
                soc_mem_field32_set(unit, mem, entry, field, *resval);
                SOC_IF_ERROR_RETURN(
                        soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, entry));
            }
        } else if (reg != INVALIDr) {
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, reg, port, reg_index, &rval));
            if (op & BST_OP_GET) {
                *resval = soc_reg_field_get(unit, reg, rval, field);
            } else if (op & BST_OP_SET) {
                if (*resval >= (1 << soc_reg_field_length(unit, reg, field))) {
                    return BCM_E_PARAM;
                }
                soc_reg_field_set(unit, reg, &rval, field, *resval);
                SOC_IF_ERROR_RETURN(
                        soc_reg32_set(unit, reg, port, reg_index, rval));
            }
        }
    }

    return BCM_E_NONE;
}

int
bcm_bst_hx5_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *pres;
    int idx_count, i;
    /* stat count and thd count need not be same.
     * stat_count - Acc Type of stat are Non Dup, hence stat is for all ACC type
     * thd_count - Acc Type of Thd are Dup(except CFAP), hence 1 copy.
     */
    int total_stat_count, total_thd_count;
    soc_mem_t stat_mem = INVALIDm, child_mem = INVALIDm, thd_mem = INVALIDm;
    soc_reg_t stat_reg = INVALIDr, thd_reg = INVALIDr, prof_reg = INVALIDr;
    soc_field_t stat_field = INVALIDf, thd_field = INVALIDf, prof_field = INVALIDf;
    int layer = 0, granularity = 1;
    uint32 rval;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_MEMORY;
    }

    bst_info->max_bytes = 0x500000;
    bst_info->to_cell = _bcm_bst_td3_byte_to_cell;
    bst_info->to_byte = _bcm_bst_td3_cell_to_byte;
    bst_info->control_set = _bcm_td3_bst_control_set;
    bst_info->control_get = _bcm_td3_bst_control_get;
    bst_info->intr_to_resources = _bcm_bst_td3_intr_to_resources;
    bst_info->hw_stat_snapshot = _bcm_bst_td3_sync_hw_snapshot;
    bst_info->hw_stat_clear = _bcm_bst_td3_hw_stat_clear;
    bst_info->intr_enable_set = _bcm_bst_td3_intr_enable_set;
    bst_info->pre_sync = NULL;
    bst_info->post_sync = NULL;
    bst_info->hw_intr_cb = soc_hx5_set_hw_intr_cb;
    bst_info->port_to_mmu_inst_map = _bst_td3_port_to_mmu_inst_map_get;
    bst_info->bst_cmn_io_op = _bcm_bst_hx5_cmn_io_op;
    bst_info->hw_sbusdma_desc_init = _bcm_bst_td3_sbusdma_desc_init;
    bst_info->hw_sbusdma_desc_deinit = _bcm_bst_td3_sbusdma_desc_deinit;
    bst_info->hw_sbusdma_desc_sync = _bcm_bst_td3_sbusdma_desc_sync;
    if(soc_property_get(unit, spn_PROFILE_PG_1HDRM_8SHARED, 1)) {
        bst_info->mode = 1;
    } else {
        bst_info->mode = 0;
    }

    /* DEVICE - Resources begin */
    /* bcmBstStatIdDevice */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdDevice);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_DEV);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 1;
    stat_mem = INVALIDm;
    stat_reg = CFAPBSTSTATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count;
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, CFAPBSTTHRSr, BST_THRESHOLDf,
                            granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst cfap");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst cfap");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* DEVICE - Resources end */

    /* INGRESS - Resources begin */
    /* bcmBstStatIdHeadroomPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
    pres->num_instance = NUM_XPE(unit);
    /* There are 4 HDRM POOL per XPE */
    idx_count = _HX5_MMU_NUM_POOL;
    stat_mem = INVALIDm;
    BCM_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    bst_info->track_mode = soc_reg_field_get(unit, MMU_GCFG_MISCCONFIGr,
                                             rval, BST_TRACKING_MODEf);
    /*
     * As Headroom Pool has two different stats resource
     * for tracking current usage count and peak usage count
     * we set up the respective resource accordingly based on
     * BST Tracking Mode
     */
    if (bst_info->track_mode) {
        /* Peak Mode */
        stat_reg = THDI_HDRM_POOL_PEAK_COUNT_HPr;
        stat_field = PEAK_BUFFER_COUNTf;
    } else {
        /* Current Mode */
        stat_reg = THDI_HDRM_POOL_COUNT_HPr;
        stat_field = TOTAL_BUFFER_COUNTf;
    }

    /* Disabling the headroom Pool monitoring during init */
    for (layer = 0; layer < NUM_LAYER(unit); layer++) {
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, layer, 0, 0x0));
    }

    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst ing headroom pool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;


    /* bcmBstStatIdIngPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdIngPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 4;
    stat_mem = INVALIDm;
    stat_reg = THDI_BST_SP_GLOBAL_SHARED_CNTr;
    stat_field = SP_GLOBAL_SHARED_CNTf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count; /* Acc type is Duplicate, hence 1 copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            THDI_BST_SP_GLOBAL_SHARED_PROFILEr,
                            SP_GLOBAL_SHARED_PROFILEf, granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst ing pool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst ing pool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdPortPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPortPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = THDI_PORT_SP_BSTm;
    stat_reg = INVALIDr;
    stat_field = SP_BST_STAT_PORT_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_PIPE(unit) * idx_count;
    thd_mem = THDI_PORT_SP_CONFIGm;
    thd_field = SP_BST_PROFILE_SHAREDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, THDI_BST_SP_SHARED_PROFILEr,
                          BST_SP_SHARED_THRESHf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst port pool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst port pool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdPriGroupShared */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupShared);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = THDI_PORT_PG_BSTm;
    stat_reg = INVALIDr;
    stat_field = PG_BST_STAT_SHAREDf;
    idx_count = NUM_PORT(unit) * 8;
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count * NUM_PIPE(unit);
    thd_mem = THDI_PORT_PG_CONFIGm;
    thd_field = PG_BST_PROFILE_SHAREDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm,
                          THDI_BST_PG_SHARED_PROFILEr, BST_PG_SHARED_THRESHf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg");
    /* Stat info count and Resource allocation */
    /* Stat res - THDI_PORT_PG_BST : (Num Ports (32/pipe) * 4 pipes) * 8(PGs) */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO2(pres, stat_mem, stat_mem,
                        stat_reg, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst pg");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdPriGroupHeadroom */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupHeadroom);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->num_instance = NUM_XPE(unit);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    stat_mem = THDI_PORT_PG_BSTm;
    stat_reg = INVALIDr;
    stat_field = PG_BST_STAT_HDRMf;
    idx_count = NUM_PORT(unit) * 8;
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count * NUM_PIPE(unit);
    thd_mem = THDI_PORT_PG_CONFIGm;
    thd_field = PG_BST_PROFILE_HDRMf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, THDI_BST_PG_HDRM_PROFILEr,
                          BST_PG_HDRM_THRESHf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg hdrm");
    /* Stat info count and Resource allocation */
    /* Stat res - THDI_PORT_PG_BST : (Num Ports (32/pipe) * 4 pipes) * 8(PGs) */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO2(pres, stat_mem, stat_mem,
                        stat_reg, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst pg hdrm");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* INGRESS - Resources end */

    /* EGRESS - Resources begin */
    /* bcmBstStatIdEgrPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 4;
    pres->threshold_adj = -1;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MCUC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count; /* No per pipe copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr, BST_STATf,
                            granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst egrpool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst egrpool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdEgrMCastPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrMCastPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 4;
    pres->threshold_adj = -1;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = idx_count; /* No per pipe copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            MMU_THDM_DB_POOL_MC_BST_THRESHOLDr, BST_STATf,
                            granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst egr mcpool");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst egr mcpool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdMcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdMcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_CMIC(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = MMU_THDM_DB_QUEUE_BSTm;
    stat_reg = INVALIDr;
    stat_field = Q_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDM_DB_QUEUE_CONFIGm;
    thd_field = BST_THRESHOLD_PROFILEf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm,
                   MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr, BST_STATf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst mcast");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO2(pres, stat_mem, stat_mem,
                        stat_reg, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst mcast");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdEgrPortPoolSharedMcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedMcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR |
                                     _BCM_BST_CMN_RES_F_PORTED);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = MMU_THDM_DB_PORTSP_BSTm;
    stat_reg = INVALIDr;
    stat_field = P_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * NUM_PIPE(unit) * idx_count;
    thd_reg = MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr;
    thd_field = BST_THRESHOLD_PROFILEf;
    for (i = 0; i < 8; i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, INVALIDm,
            thd_reg, thd_field, granularity, i);
    }

    prof_reg = MMU_THDM_DB_PORTSP_BST_THRESHOLDr;
    prof_field = BST_STATf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_PROFILE_INFO_N(pres, INVALIDm,
            prof_reg, prof_field, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                     "bst egr port pool shared mcast");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO2(pres, stat_mem, stat_mem,
                        stat_reg, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                "bst egr port pool shared mcast");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdUcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdUcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = MMU_THDU_BST_QUEUEm;
    stat_reg = INVALIDr;
    stat_field = Q_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 8; /* UC Queue Thd is 8 cell granularity */
    total_thd_count = NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDU_CONFIG_QUEUEm;
    thd_field = Q_WM_MAX_THRESHOLDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_QUEUE_BST_THRESHOLDr, BST_STATf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst ucast");
    /* Stat info count and Resource allocation */
    total_stat_count = NUM_XPE(unit) * total_thd_count;
    _BCM_BST_STAT_INFO2(pres, stat_mem, stat_mem,
            stat_reg, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst ucast");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdEgrPortPoolSharedUcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedUcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
            _BCM_BST_CMN_RES_F_PIPED |
            _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem =  MMU_THDU_BST_PORTm;
    stat_reg = INVALIDr;
    stat_field = Q_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In HX5, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    stat_reg = INVALIDr;
    /* Threshold info count and Resource allocation */
    granularity = 8; /* UC Port Shared Thd is 8 cell granularity */
    total_thd_count = NUM_XPE(unit) * NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDU_CONFIG_PORTm;
    thd_field = WM_MAX_THRESHOLDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_PORT_BST_THRESHOLDr, BST_STATf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
            "bst egr port pool shared ucast");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO2(pres, stat_mem, stat_mem,
            stat_reg, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
            "bst egr port pool shared ucast");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* EGRESS - Resources end */

    /* bcmBstStatIdRQEQueue */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdRQEQueue);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 11;
    pres->threshold_adj = -1;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDR_DB_BST_STAT_PRIQr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    thd_reg = MMU_THDR_DB_BST_THRESHOLD_PRIQr;
    thd_field = BST_THRf;
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, thd_reg, thd_field, granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst RQE queue");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst RQE queue");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdRQEPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdRQEPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 4;
    pres->threshold_adj = -1;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDR_DB_BST_STAT_SPr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    thd_reg = MMU_THDR_DB_BST_THRESHOLD_SPr;
    thd_field = BST_THRf;
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, thd_reg, thd_field, granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst RQE pool");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst RQE pool");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdUCQueueGroup */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdUCQueueGroup);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
            _BCM_BST_CMN_RES_F_PIPED |
            _BCM_BST_CMN_RES_F_RES_EGR |
            _BCM_BST_CMN_RES_F_PORTED);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));
    pres->num_instance = NUM_XPE(unit);
    stat_mem = MMU_THDU_BST_QGROUPm;
    stat_reg = INVALIDr;
    stat_field = Q_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     *      * which could be used for other instances.
     *           * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
 */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 8; /* UC queue group Thd is 8 cell granularity */
    total_thd_count = NUM_XPE(unit) * NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDU_CONFIG_QGROUPm;
    thd_field = Q_WM_MAX_THRESHOLDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_QGROUP_BST_THRESHOLDr,
            BST_STATf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
            "bst unicast queue group");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
            "bst unicast queue group");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;
    /* EGRESS - Resources end */
    return BCM_E_NONE;
}
#endif /* BCM_HELIX5_SUPPORT */

