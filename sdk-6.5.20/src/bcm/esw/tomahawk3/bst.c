 /* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BST - Buffer Statistics Tracking
 * Purpose: API to set and manage various BST resources for TH3
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
#include <soc/tomahawk3.h>
#include <soc/pfc.h>

#include <bcm/switch.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm_int/bst.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/pfc.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)

#define _TH3_BST_THRESHOLD_CELL_MAX 0x3ffff
#define _TH3_BST_PROFILE_INIT_FLAG 2
#define _TH3_BST_QUEUE_TYPE_SHIFT 16
#define _TH3_BST_QUEUE_TYPE_SHIFT_MASK 0xffff
#define _BCM_TH3_BST_Q_TYPE_UC 0
#define _BCM_TH3_BST_Q_TYPE_MC 1
#define _BCM_TH3_BST_Q_TYPE_CPU 2
#define _BCM_TH3_BST_Q_TYPE_COUNT 3

typedef enum _bcm_th3_bst_config_e {
    _bcm_th3_bst_config_tracking_mode = 0,
    _bcm_th3_bst_config_snapshot = 1
} _bcm_th3_bst_config_t;

typedef enum _bcm_th3_bst_module_e {
    _BCM_TH3_BST_MODULE_THDO = 0,
    _BCM_TH3_BST_MODULE_THDI = 1,
    _BCM_TH3_BST_MODULE_CFAP = 2,
    _BCM_TH3_BST_MODULE_THDR = 3,
    _BCM_TH3_BST_MODULE_COUNT =4
} _bcm_th3_bst_module_t;

typedef struct _bcm_th3_bst_resource_info_s {
   bcm_bst_stat_id_t stat_id;
   soc_reg_t    stat_reg;
   soc_mem_t    stat_mem;
   soc_field_t  stat_field;
   soc_reg_t    thd_reg;
   soc_mem_t    thd_mem;
   soc_field_t  thd_field;
   int idx_count;
   int stat_count;
   int thd_count;
} _bcm_th3_bst_resource_info_t;

extern int _bcm_bst_th3_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                                        int port, int index);

const static
soc_field_t sp_fields[] = {PORTSP0_SHARED_COUNTf, PORTSP1_SHARED_COUNTf,
                           PORTSP2_SHARED_COUNTf, PORTSP3_SHARED_COUNTf};
const static
soc_field_t sp_sel_fields[] = {PORTSP0_BST_SHARED_PROFILE_SELf,
                               PORTSP1_BST_SHARED_PROFILE_SELf,
                               PORTSP2_BST_SHARED_PROFILE_SELf,
                               PORTSP3_BST_SHARED_PROFILE_SELf};

const static
soc_field_t pg_fields[] = {PG0_SHARED_COUNTf, PG1_SHARED_COUNTf,
                           PG2_SHARED_COUNTf, PG3_SHARED_COUNTf,
                           PG4_SHARED_COUNTf, PG5_SHARED_COUNTf,
                           PG6_SHARED_COUNTf, PG7_SHARED_COUNTf};

const static
soc_field_t pg_sel_fields[] = {PG0_BST_SHARED_PROFILE_SELf,
                               PG1_BST_SHARED_PROFILE_SELf,
                               PG2_BST_SHARED_PROFILE_SELf,
                               PG3_BST_SHARED_PROFILE_SELf,
                               PG4_BST_SHARED_PROFILE_SELf,
                               PG5_BST_SHARED_PROFILE_SELf,
                               PG6_BST_SHARED_PROFILE_SELf,
                               PG7_BST_SHARED_PROFILE_SELf};

const static
soc_field_t pg_hdrm_fields[] = {PG0_BST_STAT_HDRMf, PG1_BST_STAT_HDRMf,
                                PG2_BST_STAT_HDRMf, PG3_BST_STAT_HDRMf,
                                PG4_BST_STAT_HDRMf, PG5_BST_STAT_HDRMf,
                                PG6_BST_STAT_HDRMf, PG7_BST_STAT_HDRMf};

const static
soc_field_t pg_hdrm_sel_fields[] = {PG0_BST_HDRM_PROFILE_SELf,
                                    PG1_BST_HDRM_PROFILE_SELf,
                                    PG2_BST_HDRM_PROFILE_SELf,
                                    PG3_BST_HDRM_PROFILE_SELf,
                                    PG4_BST_HDRM_PROFILE_SELf,
                                    PG5_BST_HDRM_PROFILE_SELf,
                                    PG6_BST_HDRM_PROFILE_SELf,
                                    PG7_BST_HDRM_PROFILE_SELf};

const static
soc_field_t uc_portsp_fields[] = {SP0_BST_PORT_SHARED_COUNTf,
                                  SP1_BST_PORT_SHARED_COUNTf,
                                  SP2_BST_PORT_SHARED_COUNTf,
                                  SP3_BST_PORT_SHARED_COUNTf};
const static
soc_field_t uc_portsp_sel_fields[] = {BST_PORTSP0_WM_SHARED_THRESHOLD_SELf,
                                  BST_PORTSP1_WM_SHARED_THRESHOLD_SELf,
                                  BST_PORTSP2_WM_SHARED_THRESHOLD_SELf,
                                  BST_PORTSP3_WM_SHARED_THRESHOLD_SELf};

STATIC int _bcm_bst_th3_sbusdma_desc_sync(int unit);
STATIC int _bcm_bst_th3_sbusdma_desc_init(int unit);
STATIC int _bcm_bst_th3_sbusdma_desc_deinit(int unit);


STATIC int
_bcm_bst_th3_byte_to_cell(int unit, uint32 bytes)
{
    return (bytes + (_TH3_MMU_BYTES_PER_CELL - 1))/_TH3_MMU_BYTES_PER_CELL;
}

STATIC int
_bcm_bst_th3_cell_to_byte(int unit, uint32 cells)
{
    return cells * _TH3_MMU_BYTES_PER_CELL;
}

STATIC uint32
_bcm_bst_th3_profile_reg_id_get(int unit, bcm_bst_stat_id_t bid, int index) {
    if(bcmBstStatIdQueueTotal == bid) {
        return (index >> _TH3_BST_QUEUE_TYPE_SHIFT);
    }
    return 0;
}

STATIC int
_bcm_th3_bst_bid_gport_cosq_param_check(int unit, bcm_bst_stat_id_t bid,
        bcm_gport_t gport, bcm_cos_queue_t cosq) {
    soc_info_t *si = &SOC_INFO(unit);
    int port_num_ucq, port;

    if (bid == bcmBstStatIdDevice) {
        /*port,queue based param checks invalid for Device*/
        return BCM_E_NONE;
    }
    if ((bid == bcmBstStatIdHeadroomPool) ||
        (bid == bcmBstStatIdIngPool) ||
        (bid == bcmBstStatIdPortPool) ||
        (bid == bcmBstStatIdPriGroupShared) ||
        (bid == bcmBstStatIdPriGroupHeadroom)) {
        if (cosq >= _TH3_MMU_NUM_PG) {
            return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        /* Passed cosq is multicast. Cannot be used for unicast Bids */
        if ((bid == bcmBstStatIdUcast) ||
           (bid == bcmBstStatIdEgrPool) ||
           (bid == bcmBstStatIdEgrPortPoolSharedUcast)) {
           return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* Passed cosq is unicast. Cannot be used for multicast Bids */
        if ((bid == bcmBstStatIdMcast) ||
            (bid == bcmBstStatIdEgrMCastPool) ||
            (bid == bcmBstStatIdEgrPortPoolSharedMcast)) {
            return BCM_E_PARAM;
        }
    } else {
        if (gport == -1) {
            return BCM_E_NONE;
        }
        /* GPORT is of local port type */
        BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_localport_resolve(unit, gport, &port));
        if (cosq < BCM_COS_INVALID) {
            return BCM_E_PARAM;
        }
        if (cosq == BCM_COS_INVALID) {
            return BCM_E_NONE;
        }
        if (IS_CPU_PORT(unit, port)) {
            port_num_ucq = 0;
            if (cosq >= SOC_TH3_NUM_CPU_QUEUES) {
                return BCM_E_PARAM;
            }
        } else {
            port_num_ucq = si->port_num_uc_cosq[port];
            if (cosq >= SOC_TH3_NUM_GP_QUEUES) {
                return BCM_E_PARAM;
            }
        }
        if (cosq < port_num_ucq) {
            /* Passed cosq is unicast. Cannot be used for multicast Bids */
            if ((bid == bcmBstStatIdMcast) ||
                (bid == bcmBstStatIdEgrMCastPool) ||
                (bid == bcmBstStatIdEgrPortPoolSharedMcast)) {
                return BCM_E_PARAM;
            }
        } else {
            /* Passed cosq is multicast. Cannot be used for unicast Bids */
             if ((bid == bcmBstStatIdUcast) ||
                (bid == bcmBstStatIdEgrPool) ||
                (bid == bcmBstStatIdEgrPortPoolSharedUcast)) {
                return BCM_E_PARAM;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int _bcm_bst_field_ids_retrieve(int unit, bcm_bst_stat_id_t bid,
        bcm_gport_t gport, bcm_cos_queue_t cosq, int *field_idx, int *num_fields) {

    int idx, pool, local_port;

    if ((field_idx == NULL) || (num_fields == NULL)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
    switch (bid) {
        case bcmBstStatIdPortPool:
            if(cosq == BCM_COS_INVALID) {
                *num_fields = _TH3_MMU_NUM_POOL;
                for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx ++) {
                    field_idx[idx] = idx;
                }
            } else {
                BCM_IF_ERROR_RETURN(
                    _bcm_th3_cosq_ingress_sp_get_by_pg(unit, local_port,
                                                        cosq, &pool));
                field_idx[0] = pool;
                *num_fields = 1;
            }
            break;
        case bcmBstStatIdPriGroupShared:
        case bcmBstStatIdPriGroupHeadroom:
            if(cosq == BCM_COS_INVALID) {
                *num_fields = _TH3_MMU_NUM_PG;
                for (idx = 0; idx < _TH3_MMU_NUM_PG; idx ++) {
                    field_idx[idx] = idx;
                }
            } else {
                field_idx[0] = cosq;
                *num_fields = 1;
            }
            break;
        case bcmBstStatIdEgrPortPoolSharedUcast:
            if(cosq == BCM_COS_INVALID) {
                *num_fields = _TH3_MMU_NUM_POOL;
                for (idx = 0; idx < _TH3_MMU_NUM_POOL; idx ++) {
                    field_idx[idx] = idx;
                }
            } else {
                BCM_IF_ERROR_RETURN(
                    _bcm_th3_cosq_egress_sp_get (unit, gport, cosq, &pool, NULL));
                field_idx[0] = pool;
                *num_fields = 1;
            }
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_th3_bst_tracking_enable_set
 * Purpose:
 *   Enable/Disable BST tracking.
 * Parameters:
 *     unit       - (IN) unit number
 *     arg  - (IN)  enable/disable config
 *     operation - (IN) operation type
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_th3_bst_tracking_enable_set(int unit, int arg,
                            _bcm_bst_thread_oper_t operation)
{
#define _BCM_BST_TH3_GLB_TRACKING_EN 0xff

    soc_reg_t reg;
    uint32 rval;
    _bcm_bst_cmn_unit_info_t *bst_info;
    reg = MMU_GLBCFG_BST_TRACKING_ENABLEr;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_INIT;
    }

    rval = arg ? _BCM_BST_TH3_GLB_TRACKING_EN : 0;
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, REG_PORT_ANY, rval));

    if (operation == _bcmBstThreadOperExternal) {
        bst_info->bst_tracking_enable = arg;
        if (arg) {
            BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_stop(unit));
        }
    } else {
        /* Set Enable Flag to True/False, to Run/Pause the Thread
         * respectively. Wont Kill the thread.
         */
        _BCM_BST_SYNC_THREAD_ENABLE_SET(unit, arg ? TRUE : FALSE);
    }
#undef _BCM_BST_TH3_GLB_TRACKING_EN
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_th3_bst_config_set
 * Purpose:
 *    Set BST config for different modules.
 * Parameters:
 *     unit       - (IN) unit number
 *     arg  - (IN)  tracking mode config
 *                  bit[0]:THDO, bit[1]: THDI, bit[2]: CFAP
 *     type - (IN) bst config type
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_bst_config_set(int unit, int arg, _bcm_th3_bst_config_t type)
{
    int i;
    uint32 rval, mode, glb_rval;
    _bcm_bst_cmn_unit_info_t *bst_info;
    soc_reg_t reg, glb_reg;
    soc_field_t field = INVALIDf;
    soc_reg_t blk_regs[] = {MMU_THDO_BST_CONFIGr, MMU_THDI_BSTCONFIGr,
                        MMU_CFAP_BSTCONFIGr, MMU_THDR_QE_BST_CONFIGr};
    soc_field_t blk_fields[2][4] = {
              {TRACKING_MODEf, TRACKING_MODEf, TRACKING_MODEf, TRACK_MODEf},
              {SNAPSHOT_ENf, SNAPSHOT_ENf, SNAPSHOT_ENf, HW_SNAPSHOT_ENf} };
    soc_field_t glb_fields[] = {
                        ACTION_EN_THDO0f, ACTION_EN_THDO1f,
                        ACTION_EN_THDI0f, ACTION_EN_THDI1f,
                        ACTION_EN_CFAP0f, ACTION_EN_CFAP1f,
                        ACTION_EN_THDR0f, ACTION_EN_THDR1f};
    soc_info_t *si = &SOC_INFO(unit);
    uint32 itm_map = si->itm_map;
    int itm, inst;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (!bst_info) {
        return BCM_E_INIT;
    }
    glb_reg = MMU_GLBCFG_BST_SNAPSHOT_ACTION_ENr;
    switch (type) {
        case _bcm_th3_bst_config_tracking_mode:
            bst_info->track_mode = arg;
            break;
        case _bcm_th3_bst_config_snapshot:
            bst_info->snapshot_mode = arg;
            break;
        default:
            return BCM_E_PARAM;
    }
    for (i = _BCM_TH3_BST_MODULE_THDO; i < _BCM_TH3_BST_MODULE_COUNT; i++) {
        field = blk_fields[type][i];
        mode = 0;
        if (type == _bcm_th3_bst_config_snapshot) {
            if (arg & (0x1 << i)) {
                mode = 0x1;
            }
        } else {
            if (arg) {
                mode = 1;
            } else {
                mode = 0;
            }
        }
        reg = blk_regs[i];
        for (itm = 0; itm < NUM_ITM(unit); itm++) {
            if (itm_map & (1 << itm)) {
                if ((itm == 1) && (i == _BCM_TH3_BST_MODULE_THDO)) {
                    continue;
                }
                inst = itm | SOC_REG_ADDR_INSTANCE_MASK;
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0,
                            &rval));
                soc_reg_field_set(unit, reg, &rval, field, mode);

                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0,
                            rval));
            }
        }


        if(type == _bcm_th3_bst_config_snapshot) {
            glb_rval = 0;
            /* config GLOBAL snapshot action register */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, glb_reg,
                                REG_PORT_ANY, 0, &glb_rval));
            soc_reg_field_set(unit, glb_reg, &glb_rval,
                                   glb_fields[_TH3_ITMS_PER_DEV * i], mode);
            soc_reg_field_set(unit, glb_reg, &glb_rval,
                                   glb_fields[_TH3_ITMS_PER_DEV * i + 1], mode);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, glb_reg,
                                REG_PORT_ANY, 0, glb_rval));

        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_th3_bst_resource_init
 * Purpose:
 *   BST Stat resouce init (initialize a SW structure) for a given Stat ID.
 * Parameters:
 *     unit       - (IN) unit number
 *     type  - (IN)  BST stat ID
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_bst_resource_init(int unit, bcm_bst_stat_id_t type)
{

    _bcm_bst_resource_info_t *pres;
    int idx_count, i;
    /* stat count and thd count need not be same.
     * stat_count - Acc Type of stat are Non Dup, hence stat is for all ACC type
     * thd_count - Acc Type of Thd are Dup(except CFAP), hence 1 copy.
     */
    int total_stat_count, total_thd_count;
    soc_mem_t stat_mem = INVALIDm, child_mem = INVALIDm,
              thd_sel_mem = INVALIDm;

    soc_reg_t stat_reg = INVALIDr, thd_reg = INVALIDr, prof_reg = INVALIDr;

    soc_reg_t q_thd_regs[_BCM_TH3_BST_Q_TYPE_COUNT] = {
                                MMU_THDO_QUE_TOT_BST_THRESHOLDr,
                                MMU_THDO_MC_QUEUE_TOT_BST_THRESHOLDr,
                                MMU_THDO_MC_CPU_QUEUE_TOT_BST_THRESHOLDr
                            };
    soc_field_t stat_field = INVALIDf, thd_field = INVALIDf,
                thd_sel_field = INVALIDf, prof_field = INVALIDf;

    int granularity = 1;
    uint32 pipe_map;
    /* get active pipe bitmap */
    soc_tomahawk3_pipe_map_get(unit, &pipe_map);

    switch (type) {
        case bcmBstStatIdDevice:
            /* bcmBstStatIdDevice */
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdDevice);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_DEV);
            pres->num_instance = NUM_ITM(unit);
            pres->num_field = 1;
            idx_count = 1;
            stat_mem = INVALIDm;
            stat_reg = MMU_CFAP_BSTSTATr;
            stat_field = CNTf;

            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = NUM_ITM(unit) * idx_count;
            _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, MMU_CFAP_BSTTHRSr,
                                                BST_THRESHOLDf, granularity);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst cfap");
            /* Stat info count and Resource allocation */
            total_stat_count = total_thd_count; /* No per pipe copy */
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst cfap");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdHeadroomPool:
            /* bcmBstStatIdHeadroomPool */
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
            pres->num_instance = NUM_ITM(unit);
            pres->num_field = 1;
            /* There are 4 HDRM POOL per ITM */
            idx_count = _TH3_MMU_NUM_POOL;
            stat_mem = INVALIDm;
            stat_reg = MMU_THDI_BST_HDRM_POOL_CNTr;
            stat_field = HDRM_POOL_CNTf;
            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count;
            _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, MMU_THDI_BST_HDRM_POOLr,
                                           BST_HDRM_POOL_THRESHf, granularity);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                                      "bst ing headroom pool");
            /* Stat info count and Resource allocation */
            total_stat_count = NUM_ITM(unit) * total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                            "bst ing headroom pool");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdIngPool:
            /* bcmBstStatIdIngPool */
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdIngPool);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
            pres->num_instance = NUM_ITM(unit);
            pres->num_field = 1;
            idx_count = _TH3_MMU_NUM_POOL;
            stat_mem = INVALIDm;
            stat_reg = MMU_THDI_BST_SP_SHARED_CNTr;
            stat_field = SP_SHARED_CNTf;

            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count;
            _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                                    MMU_THDI_BST_SP_SHAREDr,
                                    BST_SP_SHARED_THRESHf, granularity);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst ing pool");
            /* Stat info count and Resource allocation */
            total_stat_count = NUM_ITM(unit) * total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst ing pool");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdPortPool:
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPortPool);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                             _BCM_BST_CMN_RES_F_PIPED |
                                             _BCM_BST_CMN_RES_F_RES_ING);
            _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
            pres->num_instance = NUM_PIPE(unit);
            stat_mem = MMU_THDI_PORTSP_BSTm;
            stat_reg = INVALIDr;
            /* multiple fields are hanlded by _BCM_BST_STAT_FIELD_INFO */
            stat_field = INVALIDf;


            /* Get the mem attributes(Max idx) from the first child,
             * which could be used for other instances.
             */
            child_mem = SOC_MEM_UNIQUE_ACC_ITM_PIPE(unit, stat_mem, 0, 0);
            idx_count = soc_mem_index_max(unit, child_mem) + 1;

            /* Threshold profile selection info count and Resource allocation */
            granularity = 1;
            total_thd_count = NUM_PIPE(unit) * idx_count * _TH3_MMU_NUM_POOL;
            thd_sel_mem = MMU_THDI_PORT_BST_CONFIGm;
            thd_sel_field = INVALIDf;
            for (i = 0; i < NUM_PIPE(unit); i++) {
                /* pipe is active */
                if (pipe_map & (1 << i)) {
                    _BCM_BST_THRESHOLD_INFO_N(pres,
                        SOC_MEM_UNIQUE_ACC_PIPE(unit, thd_sel_mem, i),
                                    INVALIDr, thd_sel_field, granularity, i);
                }
            }
            /* Threshold profile count and Resource allocation */
            prof_reg = MMU_THDI_BST_PORTSP_SHARED_PROFILEr;
            prof_field = BST_PORTSP_SHARED_THRESHf;
            _BCM_BST_PROFILE_INFO(pres, INVALIDm, prof_reg, prof_field);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst port pool");

            /* Stat info count and Resource allocation */
            total_stat_count =  total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            for(i = 0; i < _TH3_MMU_NUM_POOL; i++) {
                _BCM_BST_STAT_FIELD_INFO(pres, sp_fields[i], i);
                _BCM_BST_THD_FIELD_INFO(pres, sp_sel_fields[i], i);
            }
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst port pool");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdPriGroupShared:
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupShared);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                             _BCM_BST_CMN_RES_F_PIPED |
                                             _BCM_BST_CMN_RES_F_RES_ING);
            _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));

            pres->num_instance = NUM_PIPE(unit);
            stat_mem = MMU_THDI_PORT_PG_SHARED_BSTm;
            stat_reg = INVALIDr;
            stat_field = INVALIDf;
            child_mem = SOC_MEM_UNIQUE_ACC_ITM_PIPE(unit, stat_mem, 0, 0);
            idx_count = soc_mem_index_max(unit, child_mem) + 1;

            /* Threshold sel info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count * NUM_PIPE(unit) * _TH3_MMU_NUM_PG;
            thd_sel_mem = MMU_THDI_PORT_BST_CONFIGm;
            thd_sel_field = INVALIDf;
            for (i = 0; i < NUM_PIPE(unit); i++) {
                /* pipe is active */
                if (pipe_map & (1 << i)) {
                    _BCM_BST_THRESHOLD_INFO_N(pres,
                                     SOC_MEM_UNIQUE_ACC(unit, thd_sel_mem)[i],
                                       INVALIDr, thd_sel_field, granularity, i);
                }
            }
            /* Threshold profile info count and Resource allocation*/
            prof_reg = MMU_THDI_BST_PG_SHARED_PROFILEr;
            prof_field = BST_PG_SHARED_THRESHf;
            _BCM_BST_PROFILE_INFO(pres, INVALIDm, prof_reg, prof_field);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg");
            for(i = 0; i < _TH3_MMU_NUM_PG; i++) {
                _BCM_BST_STAT_FIELD_INFO(pres, pg_fields[i], i);
                _BCM_BST_THD_FIELD_INFO(pres, pg_sel_fields[i], i);
            }
            /* Stat info count and Resource allocation */
            /* Stat res - THDI_PORT_PG_BST : (Num Ports (20/pipe) * 8 pipes) * 8(PGs) */
            total_stat_count = total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst pg");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdPriGroupHeadroom:
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupHeadroom);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            pres->num_instance = NUM_PIPE(unit);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                             _BCM_BST_CMN_RES_F_PIPED |
                                             _BCM_BST_CMN_RES_F_RES_ING);
            _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
            stat_mem = MMU_THDI_PORT_PG_HDRM_BSTm;
            stat_reg = INVALIDr;
            stat_field = INVALIDf;
            child_mem = SOC_MEM_UNIQUE_ACC_ITM_PIPE(unit, stat_mem, 0, 0);
            idx_count = soc_mem_index_max(unit, child_mem) + 1;

            /* Threshold sel info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count * NUM_PIPE(unit) * _TH3_MMU_NUM_PG;
            thd_sel_mem = MMU_THDI_PORT_BST_CONFIGm;
            thd_field = INVALIDf;
            for (i = 0; i < NUM_PIPE(unit); i++) {
                /* pipe is active */
                if (pipe_map & (1 << i)) {
                    _BCM_BST_THRESHOLD_INFO_N(pres,
                                SOC_MEM_UNIQUE_ACC(unit, thd_sel_mem)[i],
                                      INVALIDr, thd_sel_field, granularity, i);
                }
            }

            /*Threshold profile info count and Resource allocation */
            prof_reg = MMU_THDI_BST_PG_HDRM_PROFILEr;
            prof_field = BST_PG_HDRM_THRESHf;
            _BCM_BST_PROFILE_INFO(pres, INVALIDm, prof_reg, prof_field);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg hdrm");
            for(i = 0; i < _TH3_MMU_NUM_PG; i++) {
                _BCM_BST_STAT_FIELD_INFO(pres, pg_hdrm_fields[i], i);
                _BCM_BST_THD_FIELD_INFO(pres, pg_hdrm_sel_fields[i], i);
            }
            /* Stat info count and Resource allocation */
            /* Stat res - THDI_PORT_PG_BST : (Num Ports (20/pipe) * 8 pipes) * 8(PGs) */
            total_stat_count = total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count, "bst pg hdrm");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdEgrPool:
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPool);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
            pres->num_instance = NUM_ITM(unit);
            pres->num_field = 1;
            idx_count = _TH3_MMU_NUM_POOL;
            pres->threshold_adj = 0;
            stat_mem = INVALIDm;
            stat_reg = MMU_THDO_UC_POOL_BST_COUNTr;
            stat_field = BST_COUNTf;

            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count;
            thd_reg = MMU_THDO_SP_SHR_BST_THRESHOLDr;
            thd_field = BST_STATf;
            _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, thd_reg,
                                                thd_field, granularity);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                                            "bst egr pool");
            /* Stat info count and Resource allocation */
            total_stat_count = NUM_ITM(unit) * total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                                            "bst egr pool");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;

            break;
        case bcmBstStatIdEgrMCastPool:
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrMCastPool);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
            pres->num_instance = NUM_ITM(unit);
            pres->num_field = 1;
            idx_count = _TH3_MMU_NUM_POOL;
            pres->threshold_adj = 0;
            stat_mem = INVALIDm;
            stat_reg = MMU_THDO_MC_POOL_BST_COUNTr;
            stat_field = BST_COUNTf;

            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count;
            thd_reg = MMU_THDO_MC_CQE_SP_BST_THRESHOLDr;
            thd_field = BST_STATf;
            _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, thd_reg,
                                                thd_field, granularity);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                                            "bst egr mcpool");
            /* Stat info count and Resource allocation */
            total_stat_count =  NUM_ITM(unit) * total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                                            "bst egr mcpool");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdEgrPortPoolSharedMcast:
             pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedMcast);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                             _BCM_BST_CMN_RES_F_RES_EGR);
            _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
            pres->num_instance = NUM_ITM(unit);
            pres->num_field = 1;
            stat_mem = MMU_THDO_BST_SHARED_PORTSP_MCm;
            stat_reg = INVALIDr;
            stat_field = SHARED_COUNTf;

            /* Get the mem attributes(Max idx) from the first child,
             * which could be used for other instances.
             * Note: In TH3, ITM 0 instance valid for all memories(Ing/Egr).
             */
            child_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, stat_mem, 0);
            idx_count = soc_mem_index_max(unit, child_mem) + 1;

            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count;
            thd_sel_mem = MMU_THDO_CONFIG_PORTSP_MCm;
            thd_sel_field = BST_PORTSP_WM_MC_CQE_THRESHOLD_SELf;

            _BCM_BST_THRESHOLD_INFO(pres, thd_sel_mem,
                                    INVALIDr, thd_sel_field, granularity);


            prof_reg = MMU_THDO_MC_CQE_PRT_SP_BST_THRESHOLDr;
            prof_field = BST_STATf;
            _BCM_BST_PROFILE_INFO(pres, INVALIDm, prof_reg, prof_field);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                             "bst egr port pool shared mcast");
            /* Stat info count and Resource allocation */
            total_stat_count = NUM_ITM(unit) * total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                        "bst egr port pool shared mcast");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdEgrPortPoolSharedUcast:
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedUcast);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                             _BCM_BST_CMN_RES_F_RES_EGR);
            _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));
            pres->num_instance = NUM_ITM(unit);
            stat_mem = MMU_THDO_BST_SHARED_PORTm;
            stat_reg = INVALIDr;
            stat_field = INVALIDf;

            /* Get the mem attributes(Max idx) from the first child,
             * which could be used for other instances.
             * Note: In TH3, ITM 0 instance valid for all memories(Ing/Egr).
             */
            child_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, stat_mem, 0);
            idx_count = soc_mem_index_max(unit, child_mem) + 1;

            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count * _TH3_MMU_NUM_POOL;
            thd_sel_mem = MMU_THDO_CONFIG_PORT_UC0m;
            thd_sel_field = INVALIDf;
            _BCM_BST_THRESHOLD_INFO(pres, thd_sel_mem,
                                        INVALIDr, thd_sel_field, granularity);

            prof_reg = MMU_THDO_PRT_SP_SHR_BST_THRESHOLDr;
            prof_field = BST_STATf;
            for(i = 0; i < _TH3_MMU_NUM_POOL; i++) {
                _BCM_BST_THD_FIELD_INFO(pres, uc_portsp_sel_fields[i], i);
                _BCM_BST_STAT_FIELD_INFO(pres, uc_portsp_fields[i], i);
            }
            _BCM_BST_PROFILE_INFO(pres, INVALIDm, prof_reg, prof_field);
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                             "bst egr port pool shared ucast");
            /* Stat info count and Resource allocation */
            total_stat_count = NUM_ITM(unit) * total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                        "bst egr port pool shared ucast");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;
        case bcmBstStatIdQueueTotal:
            pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdQueueTotal);
            _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
            _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                             _BCM_BST_CMN_RES_F_RES_EGR);
            _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));
            pres->num_instance = NUM_ITM(unit);
            pres->num_field = 1;
            stat_mem = MMU_THDO_BST_TOTAL_QUEUEm;
            stat_reg = INVALIDr;
            stat_field = BST_QUEUE_TOTAL_COUNTf;

            /* Get the mem attributes(Max idx) from the first child,
             * which could be used for other instances.
             * Note: In TH3, ITM 0 instance valid for all memories(Ing/Egr).
             */
            child_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, stat_mem, 0);
            idx_count = soc_mem_index_max(unit, child_mem) + 1;

            /* Threshold info count and Resource allocation */
            granularity = 1;
            total_thd_count = idx_count;
            thd_sel_mem = MMU_THDO_QUEUE_CONFIGm;
            thd_sel_field = BST_Q_WM_TOTAL_THRESHOLD_SELf;
            _BCM_BST_THRESHOLD_INFO(pres, thd_sel_mem,
                                        INVALIDr, thd_sel_field, granularity);


            prof_field = BST_STATf;
            for(i = 0; i < _BCM_TH3_BST_Q_TYPE_COUNT; i++) {
                prof_reg = q_thd_regs[i];
                _BCM_BST_PROFILE_INFO_N(pres, INVALIDm, prof_reg,
                                                            prof_field, i);

            }
            pres->num_profile_reg = i;
            _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                             "bst queue total use count");
            /* Stat info count and Resource allocation */
            total_stat_count = NUM_ITM(unit) * total_thd_count;
            _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
            _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                        "bst egr port pool shared ucast");
            pres->index_min = 0;
            pres->index_max = total_stat_count - 1;
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_th3_bst_control_set
 * Purpose:
 *   Set BST global control.
 * Parameters:
 *     unit       - (IN) unit number
 *     type  - (IN)  control type
 *     arg   - (IN)  control value
 *     operation   - (IN) operation origin, i.e. internal or external
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_th3_bst_control_set(int unit, bcm_switch_control_t type, int arg,
                        _bcm_bst_thread_oper_t operation)
{
    if(arg < 0) {
        return BCM_E_PARAM;
    }

    switch (type) {
        case bcmSwitchBstEnable:
            BCM_IF_ERROR_RETURN
                (_bcm_th3_bst_tracking_enable_set(unit, arg, operation));
            break;
        case bcmSwitchBstTrackingMode:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_config_set(unit, arg,
                            _bcm_th3_bst_config_tracking_mode));
            break;
        case bcmSwitchBstSnapshotEnable:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_config_set(unit, arg, _bcm_th3_bst_config_snapshot));

            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_th3_bst_control_get
 * Purpose:
 *   Get BST global control configuration.
 * Parameters:
 *     unit       - (IN) unit number
 *     type  - (IN)  control type
 *     arg   - (OUT)  control value
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_th3_bst_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_INIT;
    }

    switch (type) {
        case bcmSwitchBstEnable:
            /*HW register MMU_GLBCFG_BST_TRACKING_ENABLE cannot be used to
             * retrieve this as the value can be modified by hardware when HW
             * snapshot action enable is set*/
            *arg = bst_info->bst_tracking_enable;
            break;
        case bcmSwitchBstTrackingMode:
            *arg = bst_info->track_mode;
            break;
        case bcmSwitchBstSnapshotEnable:
            *arg = bst_info->snapshot_mode;
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *    _bcm_th3_bst_control_get
 * Purpose:
 *  Enable/disable BST HW interrupt.
 * Parameters:
 *     unit       - (IN) unit number
 *     enable  - (IN)  Enable config
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_bst_th3_intr_enable_set(int unit, int enable)
{
    int i;
    uint32 rval;
    uint64 rval64;
    soc_reg_t reg = INVALIDr;
    soc_reg_t regs_intr[_BCM_TH3_BST_MODULE_COUNT] =
                        {MMU_THDO_BST_CPU_INT_ENr, MMU_THDI_CPU_INT_ENr,
                             MMU_CFAP_INT_ENr, MMU_THDR_QE_CPU_INT_ENr};
    soc_field_t fields_intr[_BCM_TH3_BST_MODULE_COUNT] =
                                    {BSTf, BST_TRIGf, BST_TRIGf, BSTf};

    if (!soc_property_get(unit, spn_POLLED_IRQ_MODE, 0)) {
        /* Enable interrupt at base level module THDI/THDO/THDR/CFAP */
        for(i = _BCM_TH3_BST_MODULE_THDO; i < _BCM_TH3_BST_MODULE_COUNT; i++) {
            rval = 0;
            reg = regs_intr[i];
            if(reg == MMU_CFAP_INT_ENr) {
                SOC_IF_ERROR_RETURN(
                    soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64,
                                                fields_intr[i], enable);
                SOC_IF_ERROR_RETURN(
                    soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));
                continue;
            };
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, fields_intr[i], enable);
            SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_set_hw_intr_cb(int unit)
{
    BCM_IF_ERROR_RETURN(soc_th3_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
    return BCM_E_NONE;
}


STATIC int
_bcm_bst_th3_intr_to_resources(int unit, uint32 *flags)
{
    uint32 flags_tr = 0;
    uint32 inst, itm_map, pipe_map;
    uint32 fval = 0;
    uint32 rval, rval32_type, rval32_status;
    int index[_bcmResourceMaxCount] = {-1};
    int res[_bcmResourceMaxCount] = {-1}, res_ct = 0;
    int bid_detect[_bcmResourceMaxCount] = {0};
    soc_field_t fld = INVALIDf;
    soc_reg_t reg_type = INVALIDr, reg_status = INVALIDr;
    int i, pipe, itm = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    _bcm_bst_th3_intr_enable_set(unit, 0);
    LOG_VERBOSE(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d flags=0x%x\n"),
                 unit, *flags));
    itm_map = si->itm_map;
    soc_tomahawk3_pipe_map_get(unit, &pipe_map);

    /* _bcmResourceDevice */
    BCM_IF_ERROR_RETURN(READ_MMU_CFAP_BSTSTATr(unit, &rval));
    if (soc_reg_field_get(unit, MMU_CFAP_BSTSTATr, rval, CNTf)) {
        if (SOC_REG_IS_VALID(unit, MMU_CFAP_INT_STATr)) {
            /* For CFAP, it use MMU_CFAP_INT_STATr to indicate the bst
             * interruptbut not use a trigger status register.
             * MMU_CFAP_INT_STATr would be cleared in SER handler, so
             * we check _bcmResourceDevice every time here. */
            index[res_ct] = 0;
            res[res_ct++] = _bcmResourceDevice;
        }
    }

    /* THDI Status/Triggers per-pipe */
    reg_type = MMU_THDI_BST_TRIGGER_STATUS_TYPEr;
    reg_status = THDI_BST_TRIGGER_STATUS_32r;
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1U << pipe))) {
            continue;
        }
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg_type, inst, 0, &rval32_type));
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg_status, inst, 0, &rval32_status));

        /* _bcmResourceIngPool */
        fval = soc_reg_field_get(unit, reg_type, rval32_type, SP_SHARED_TRIGGERf);
        if (fval && !bid_detect[_bcmResourceIngPool]) {
            fld = SP_SHARED_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourceIngPool;
            bid_detect[_bcmResourceIngPool] = 1;
        }

        /* _bcmResourcePortPool */
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                 PORTSP_SHARED_TRIGGERf);
        if (fval && !bid_detect[_bcmResourcePortPool]) {
            fld = PORTSP_SHARED_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourcePortPool;
            bid_detect[_bcmResourcePortPool] = 1;
        }

        /* _bcmResourcePriGroupShared */
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                 PG_SHARED_TRIGGERf);
        if (fval && !bid_detect[_bcmResourcePriGroupShared]) {
            fld = PG_SHARED_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourcePriGroupShared;
            bid_detect[_bcmResourcePriGroupShared] = 1;
        }

        /* _bcmResourcePriGroupHeadroom */
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                 PG_HDRM_TRIGGERf);

        if (fval && !bid_detect[_bcmResourcePriGroupHeadroom]) {
            fld = PG_HDRM_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourcePriGroupHeadroom;
            bid_detect[_bcmResourcePriGroupHeadroom] = 1;
        }

        /* _bcmResourceHeadroomPool */
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                 HEADROOM_POOL_TRIGGERf);

        if (fval && !bid_detect[_bcmResourceHeadroomPool]) {
            fld = HEADROOM_POOL_TRIGGER_STATUSf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourceHeadroomPool;
            bid_detect[_bcmResourceHeadroomPool] = 1;
        }
        rval32_type = 0;
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_type, inst, 0, rval32_type));
        rval32_status = 0;
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_status, inst, 0, rval32_status));
    }


    /* THDO Status/Triggers */
    /* MC egr pool and MC egr port-pool ID are stored in reg_status,
     * the rest of info is stored in reg_type
    */
    reg_type = MMU_THDO_BST_STATr;
    reg_status = MMU_THDO_BST_STAT1r;
    rval = 0;
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        if (!(itm_map & (1U << itm))) {
            continue;
        }
        inst = itm | SOC_REG_ADDR_INSTANCE_MASK;
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg_type, inst, 0, &rval32_type));
        BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, reg_status, inst, 0, &rval32_status));
        /* _bcmResourceEgrPool */
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                     MMU_THDO_SP_SHR_BST_TRIGf);

        if (fval && !bid_detect[_bcmResourceEgrPool]) {
            fld = MMU_THDO_SP_SHR_BST_IDf;
            index[res_ct] = soc_reg_field_get(unit, reg_type, rval32_type, fld);
            res[res_ct++] = _bcmResourceEgrPool;
            bid_detect[_bcmResourceEgrPool] = 1;
        }

        /* _bcmResourceEgrMCastPool */
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                     MMU_THDO_MC_CQE_SP_BST_TRIGf);

        if (fval && !bid_detect[_bcmResourceEgrMCastPool]) {
            fld = MMU_THDO_MC_CQE_SP_BST_IDf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourceEgrMCastPool;
            bid_detect[_bcmResourceEgrMCastPool] = 1;
        }

        /* _bcmResourceBstStatIdQueueTotal */
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                     MMU_THDO_QUE_TOT_BST_TRIGf);

        if (fval && !bid_detect[_bcmResourceBstStatIdQueueTotal]) {
            fld = MMU_THDO_QUE_TOT_BST_IDf;
            index[res_ct] = soc_reg_field_get(unit, reg_type, rval32_type, fld);
            res[res_ct++] = _bcmResourceBstStatIdQueueTotal;
            bid_detect[_bcmResourceBstStatIdQueueTotal] = 1;
        }

        /* _bcmResourceEgrPortPoolSharedMcast*/
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                     MMU_THDO_MC_CQE_PRT_SP_BST_TRIGf);

        if (fval && !bid_detect[_bcmResourceEgrPortPoolSharedMcast]) {
            fld = MMU_THDO_MC_CQE_PRT_SP_BST_IDf;
            index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
            res[res_ct++] = _bcmResourceEgrPortPoolSharedMcast;
            bid_detect[_bcmResourceEgrPortPoolSharedMcast] = 1;
        }

        /* _bcmResourceEgrPortPoolSharedUcast*/
        fval = soc_reg_field_get(unit, reg_type, rval32_type,
                                     MMU_THDO_PRT_SP_SHR_BST_TRIGf);
        if (fval && !bid_detect[_bcmResourceEgrPortPoolSharedUcast]) {
            fld = MMU_THDO_PRT_SP_SHR_BST_IDf;
            index[res_ct] = soc_reg_field_get(unit, reg_type, rval32_type, fld);
            res[res_ct++] = _bcmResourceEgrPortPoolSharedUcast;
            bid_detect[_bcmResourceEgrPortPoolSharedUcast] = 1;
        }
        rval32_type = 0;
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_type, inst, 0, rval32_type));
        rval32_status = 0;
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_status, inst, 0, rval32_status));
    }
    if (res_ct == 0) {
        _bcm_bst_th3_intr_enable_set(unit, 1);
        /* No resource to fetch */
        return BCM_E_NONE;
    }

    for (i = 0; i < res_ct; i++) {
        BCM_IF_ERROR_RETURN(
            _bcm_bst_th3_sync_hw_snapshot(unit, res[i], -1, index[i]));
    }

    *flags = flags_tr;
    _bcm_bst_th3_intr_enable_set(unit, 1);
    return BCM_E_NONE;
}


/*
 * Function:
 *   bcm_bst_th3_hw_stat_clear
 * Purpose:
 *  Clear specified BST stat in HW.
 * Parameters:
 *     unit       - (IN) unit number
 *     resInfo  - (IN)  resource info sturcture.
 *     bid   - (IN) BST stat ID.
 *     buffer - (IN) buffer(ITM) id.
 *     port   - (IN) local port number.
 *     index  - (IN) stat index
 * Returns:
 *     BCM_E_XXX
 */

int
bcm_bst_th3_hw_stat_clear(int unit, _bcm_bst_resource_info_t *resInfo,
                bcm_bst_stat_id_t bid, bcm_cosq_buffer_id_t buffer,
                                                    int port, int index,
                                                    int cosq)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv = BCM_E_NONE;
    uint32 rval;
    soc_mem_t base_mem = INVALIDm, mem = INVALIDm;
    soc_reg_t reg;
    int min_hw_idx, max_hw_idx, idx;
    int inst, stats_per_inst;
    int num_entries, mem_wsz;
    int num_field, field_idx;
    void *pentry;
    char *dmabuf;
    uint32 mmu_inst_map = 0;
    int field_ids[_TH3_MMU_NUM_PG];
    int num, hw_idx;
    int enable = 0;

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    if (bst_info->port_to_mmu_inst_map) {
        (void) bst_info->port_to_mmu_inst_map(unit, bid, port,
            &mmu_inst_map);
    }

    if (bst_info->control_get) {
        bst_info->control_get(unit, bcmSwitchBstEnable, &enable);
    }
    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, 0,
                _bcmBstThreadOperInternal);
    }

    if (!mmu_inst_map) {
        rv = BCM_E_PARAM;
        goto restore;
    }

    if (index == -1) {
        min_hw_idx = resInfo->index_min;
        max_hw_idx = ((resInfo->index_max + 1) / resInfo->num_instance) - 1;
    } else {
        min_hw_idx = max_hw_idx = index;
    }

    for (inst = 0; inst < resInfo->num_instance; inst++) {
        if (!(mmu_inst_map & (1 << inst))) {
            continue;
        }
        if ((base_mem = resInfo->stat_mem[0]) != INVALIDm) {
            /* All BST memories are either IPIPE based or ITM based in TH3*/
            if((resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
                mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, base_mem, inst);
            } else {
                mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, inst);
            }
            if(mem == INVALIDm) {
                rv = BCM_E_INTERNAL;
                goto restore;
            }

            if (index == -1) {
                /* clear all entries in the memory */
                num_entries = soc_mem_index_count(unit, mem);
                mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

                dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
                if (dmabuf == NULL) {
                    rv = BCM_E_MEMORY;
                    goto restore;
                }

                if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                        soc_mem_index_min(unit, mem),
                                        soc_mem_index_max(unit, mem),
                                        dmabuf)) {
                    soc_cm_sfree(unit, dmabuf);
                    rv = BCM_E_INTERNAL;
                    goto restore;
                }

                /* Clear stat counter if requested */
                for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                    if (resInfo->stat_field != INVALIDf) {
                        pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                          dmabuf, idx);
                        soc_mem_field32_set(unit, mem, pentry,
                                resInfo->stat_field, 0);
                    } else {
                        /* Multiple fields case */
                        num_field = resInfo->num_field;
                        hw_idx = idx / num_field;
                        pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                          dmabuf, hw_idx);
                        for (field_idx = 0; field_idx < num_field; field_idx++)
                            {
                                 soc_mem_field32_set(unit, mem, pentry,
                                   resInfo->stat_fields[field_idx], 0);
                            }
                    }
                }

                if (soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                                        soc_mem_index_min(unit, mem),
                                        soc_mem_index_max(unit, mem),
                                        dmabuf)) {
                    soc_cm_sfree(unit, dmabuf);
                    rv = BCM_E_INTERNAL;
                    goto restore;
                }
                soc_cm_sfree(unit, dmabuf);
            } else {
                /* Clear a specific index */
                uint32 entry[SOC_MAX_MEM_WORDS];

                if(resInfo->stat_field != INVALIDf) {
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry);
                    if (rv) {
                        goto restore;
                    }
                    soc_mem_field32_set(unit, mem, entry,
                                        resInfo->stat_field, 0);
                    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
                    if (rv) {
                        goto restore;
                    }
                } else {
                    /*multiple field case */
                    num_field = resInfo->num_field;
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, entry);
                    if (rv) {
                        goto restore;
                    }
                    rv = _bcm_bst_field_ids_retrieve(unit, bid, port, cosq,
                                field_ids, &num);
                    if (rv) {
                        goto restore;
                    }
                    if (num > num_field) {
                        rv = BCM_E_INTERNAL;
                        goto restore;
                    }
                    for (idx = 0; idx < num; idx++) {
                        field_idx = field_ids[idx];
                        soc_mem_field32_set(unit, mem, entry,
                                   resInfo->stat_fields[field_idx], 0);
                    }
                    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
                    if (rv) {
                        goto restore;
                    }

                }
            }

        } else if ((reg = resInfo->stat_reg[0]) != INVALIDr) {
            int stat_idx = 0;
            stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;

            /* Clear stat counters */
            for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                int l_inst;
                rval = 0;
                if (stats_per_inst) {
                    l_inst = inst | SOC_REG_ADDR_INSTANCE_MASK;
                    stat_idx = idx % stats_per_inst;
                } else {
                    l_inst = REG_PORT_ANY;
                    stat_idx = idx;
                }
                rv = soc_reg32_get(unit, reg, l_inst, stat_idx, &rval);
                if (rv) {
                    goto restore;
                }
                /* no stat_reg has multiple fields in TH3*/
                soc_reg_field_set(unit, reg, &rval, resInfo->stat_field, 0);
                rv = soc_reg32_set(unit, reg, l_inst, stat_idx, rval);
                if (rv) {
                    goto restore;
                }
            }

        } /* if valid reg */
    } /* for each instance */

restore:
    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, enable,
                _bcmBstThreadOperInternal);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_bst_th3_sync_resource_stat
 * Purpose:
 *  Sync a stat count at a specific index.
 * Parameters:
 *     unit       - (IN) unit number.
 *     resInfo  - (IN)  resource info sturcture.
 *     idx      - (IN) index of SW stat.
 *     val  - (IN) value to sync.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_bst_th3_sync_resource_stat(
    int unit,
    _bcm_bst_resource_info_t *resInfo,
    uint32 idx,
    uint32  val)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int hwm_mode;
    if(resInfo == NULL) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    if (idx > resInfo->index_max) {
        return BCM_E_INTERNAL;
    }

    hwm_mode = bst_info->track_mode;
     if (hwm_mode) {
        /* BST tracking is in Peak mode.
         * Hence store the watermark value only.
         */
        if (resInfo->p_stat[idx] < val) {
            resInfo->p_stat[idx] = val;
        }
    } else {
        resInfo->p_stat[idx] = val;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_bst_th3_sync_hw_snapshot
 * Purpose:
 *  Sync all HW BST stat counts to SW copy for a given Stat ID.
 * Parameters:
 *     unit       - (IN) unit number.
 *     bid  - (IN)  BST stat ID.
 *     port, index  - (IN) not used in the function, the function signature
 *           is defined in a way such that it is consistent with the function
 *           pointer required by bst_info->hw_stat_snapshot
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_bst_th3_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                             int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int num_entries, mem_wsz;
    int i, j, idx, idx_offset, thd_idx;
    int inst;
    int stats_per_inst = 0; /* stats per mem instance(itm) */
    void *pentry;
    char *dmabuf;
    uint32 rval, temp_val;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    bcm_gport_t gport;
    bcm_cos_queue_t cosq;
    uint32 pipe_map, itm_map;
    soc_info_t *si = &SOC_INFO(unit);
    int enable = 0;
    int rv = BCM_E_NONE;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    if (bst_info->control_get) {
        bst_info->control_get(unit, bcmSwitchBstEnable, &enable);
    }
    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, 0,
                _bcmBstThreadOperInternal);
    }

    /* sync all the stats */
    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if (resInfo == NULL) {
        rv = BCM_E_PARAM;
        goto restore;
    }

    base_mem = resInfo->stat_mem[0];
    reg = resInfo->stat_reg[0];

    /* get active pipe bitmap & itmbitmap */
    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    itm_map = si->itm_map;

    if (reg != INVALIDr) {
        int stat_idx = 0;
        stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;

        for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
            rval = 0;
            if (stats_per_inst) {
                inst = idx / stats_per_inst;
                inst = inst | SOC_REG_ADDR_INSTANCE_MASK;
                stat_idx = idx % stats_per_inst;
            } else {
                inst = REG_PORT_ANY;
                stat_idx = idx;
                stats_per_inst = 1;
            }

            idx_offset = idx;
            if(resInfo->stat_field != INVALIDf) {
                rv = soc_reg32_get(unit, reg, inst, stat_idx, &rval);
                if (rv) {
                    goto restore;
                }
                temp_val = soc_reg_field_get(unit, reg, rval,
                                                    resInfo->stat_field);
                rv = _bcm_bst_th3_sync_resource_stat(unit, resInfo,
                                                    idx_offset, temp_val);
                if (rv) {
                    goto restore;
                }
            } else {
                /* register have multiple fields*/
                rv = BCM_E_INTERNAL;
                goto restore;
            }

            /* Check if there exists threshold for a resource
             * continue if we don't have one */
            if (resInfo->p_threshold == NULL) {
                continue;
            }

            thd_idx = idx;
            if ((bid != bcmBstStatIdDevice) &&
                    (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED))) {
                /* Bids which have duplicate access */
                thd_idx = stat_idx;
            }
            /* coverity[copy_paste_error : FALSE] */
            if (resInfo->p_threshold[thd_idx] &&
                (resInfo->p_stat[idx_offset] >=
                 (resInfo->p_threshold[thd_idx] * resInfo->threshold_gran)
                )) {
                /* Generate SOC EVENT */
                if (bst_info->handlers.reverse_resolve_index) {
                    /* reverse map the inedx to port/cos pair */
                    bst_info->handlers.reverse_resolve_index(unit, bid,
                                                             port, idx_offset,
                                                             &gport, &cosq);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                       bid, gport, cosq);

                }
            }

        }
    } else if (base_mem != INVALIDm) {
        int min_idx = 0, max_idx = 0;
        int instance_offset = 0; /* offset per pipe or per (ITM) */

        /* Get the mem attributes(Max idx) from the first child,
         * which could be used for other instances.
         * Note: In TH3, ITM 0/Pipe 0 instance valid for all memories(Ing/Egr).
         */
        if(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {

            mem = SOC_MEM_UNIQUE_ACC_ITM_PIPE(unit, base_mem, 0, 0);
        }else {
            mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, 0);
        }
        if (mem == INVALIDm) {
            rv = BCM_E_PARAM;
            goto restore;
        }
        stats_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
        min_idx = soc_mem_index_min(unit, mem);
        max_idx = soc_mem_index_max(unit, mem);
        num_entries = max_idx + 1;

        mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

        dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
        if (dmabuf == NULL) {
            rv = BCM_E_MEMORY;
            goto restore;
        }
        /* num_instance is equal to the number of pipes if memory is per-pipe,
         * or the number of ITMs otherwise*/
        for (i = 0; i < resInfo->num_instance; i++) {
            instance_offset = 0;
            gport = BCM_GPORT_INVALID;
            cosq = -1;
            instance_offset = stats_per_inst * i;
            if((resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
                if (!(pipe_map & (1 << i))) {
                    continue;
                }
                mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, base_mem, i);

            } else {
                if (!(itm_map & (1 << i))) {
                    continue;
                }
                mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, i);
            }
            if(mem == INVALIDm) {
                rv = BCM_E_INTERNAL;
                goto restore;
            }

            if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                    min_idx, max_idx, dmabuf)) {
                soc_cm_sfree(unit, dmabuf);
                rv = BCM_E_INTERNAL;
                goto restore;
            }

            for (idx = 0; idx <= max_idx; idx++) {
                idx_offset = idx + instance_offset;
                pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                      dmabuf,
                                                      idx);
                if(resInfo->stat_field != INVALIDf) {
                    idx_offset = idx + instance_offset;
                    temp_val = soc_mem_field32_get(unit, mem, pentry,
                                                        resInfo->stat_field);
                    rv = _bcm_bst_th3_sync_resource_stat(unit, resInfo,
                                                     idx_offset, temp_val);
                    if (rv) {
                        soc_cm_sfree(unit, dmabuf);
                        goto restore;
                    }

                    /* In TH3, stat count and thd count need not be same as in
                     * previous chips(check init). This is due to different
                     * AccType for Stat and Threshold.
                     * Stats - usually NON-DUP Acc Type
                     * Threshold - usually DUP Acc Type
                     * Hence threshold index might only exist for 1 instance
                     * and applicable to all.
                     */
                    thd_idx = idx_offset;
                    if ((bid != bcmBstStatIdDevice) &&
                            ((!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)))) {
                        /* Bids which have duplicate access across ITM */
                        thd_idx = idx_offset % stats_per_inst;
                    }
                    if (resInfo->p_threshold[thd_idx] &&
                        (resInfo->p_stat[idx_offset] >=
                         (resInfo->p_threshold[thd_idx]) * resInfo->threshold_gran)) {
                        /* Generate SOC EVENT */
                        if (bst_info->handlers.reverse_resolve_index) {
                            /* reverse map the inedx to port/cos pair */
                            bst_info->handlers.reverse_resolve_index
                                           (unit, bid, port, thd_idx,
                                                                 &gport, &cosq);
                            soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                               bid, gport, cosq);

                        }
                    }
                } else {
                    for(j = 0; j < resInfo->num_field; j++) {
                        idx_offset = (idx * (resInfo->num_field))
                                                    + instance_offset + j;
                        temp_val = soc_mem_field32_get(unit, mem,
                                            pentry, resInfo->stat_fields[j]);
                        rv = _bcm_bst_th3_sync_resource_stat(unit, resInfo,
                                                  idx_offset , temp_val);
                        if (rv) {
                            soc_cm_sfree(unit, dmabuf);
                            goto restore;
                        }
                        /* In TH3, stat count and thd count need not be same as in
                         * previous chips(check init). This is due to different
                         * AccType for Stat and Threshold.
                         * Stats - usually NON-DUP Acc Type
                         * Threshold - usually DUP Acc Type
                         * Hence threshold index might only exist for 1 instance
                         * and applicable to all.
                         */
                        if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
                            thd_idx = (idx * resInfo->num_field) + j;
                        } else {
                            thd_idx = idx_offset;
                        }
                        if (resInfo->p_threshold[thd_idx] &&
                            (resInfo->p_stat[idx_offset] >=
                             (resInfo->p_threshold[thd_idx]) * resInfo->threshold_gran)) {
                            /* Generate SOC EVENT */
                            if (bst_info->handlers.reverse_resolve_index) {
                                /* reverse map the inedx to port/cos pair */
                                bst_info->handlers.reverse_resolve_index
                                               (unit, bid, port, thd_idx,
                                                                     &gport, &cosq);
                                soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                                   bid, gport, cosq);

                            }
                        }

                    }

                }

            }

        }
        soc_cm_sfree(unit, dmabuf);
    }
restore:
    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, enable,
                _bcmBstThreadOperInternal);
    }

    return rv;
}

/*
 * Function:
 *   _bst_th3_port_to_mmu_inst_map_get
 * Purpose:
 *  Function to return the corresponding ITM instance for a given gport.
 *  for per-pipe register/memory, instance -> pipe
 *  otherwise, instance -> ITM
 * Parameters:
 *     unit       - (IN) unit number.
 *     bid  - (IN)  BST stat ID.
 *     gport - (IN) gport number.
 *     mmu_inst_map  (OUT) instance map.
 * Returns:
 *     BCM_E_XXX
 */

int _bst_th3_port_to_mmu_inst_map_get(int unit, bcm_bst_stat_id_t bid,
                                     bcm_gport_t gport, uint32 *mmu_inst_map)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_bst_resource_info_t *resInfo;
    bcm_port_t local_port;
    int pipe;
    uint32 pipe_map;
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    if(gport == -1) {
        if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            *mmu_inst_map = pipe_map;
        } else {
            *mmu_inst_map = si->itm_map;
        }
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_ING) {
        if(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            *mmu_inst_map = 1U << pipe;
            *mmu_inst_map &= pipe_map;
        } else {
            *mmu_inst_map = si->ipipe_itm_map[pipe];
            *mmu_inst_map &= si->itm_map;
        }
    } else {
        /* For Global and EGR resource, return all instances (ITMs)*/
        *mmu_inst_map = si->itm_map;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *   bcm_th3_cosq_bst_stat_clear
 * Purpose:
 *  Clear speficied BST stat.
 * Parameters:
 *     unit       - (IN) unit number.
 *     gport - (IN) gport number.
 *     cosq  - (IN) cosq number.
 *     buffer - (IN) buffer(ITM) ID.
 *     bid    - (IN) BST stat ID.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_bst_stat_clear(int unit, bcm_gport_t gport,
                         bcm_cos_queue_t cosq, bcm_cosq_buffer_id_t buffer,
                                                      bcm_bst_stat_id_t bid)

{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int bcm_rv = BCM_E_NONE;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL;
    int rcb = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe;
    int hw_index;

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->handlers.resolve_index == NULL) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
            _bcm_th3_bst_bid_gport_cosq_param_check(unit, bid, gport, cosq));

    if (bcmBstStatIdUcast == bid || bcmBstStatIdMcast == bid) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
    if (gport == -1) {
        /* Clear all stat counters in the buffer*/
        (void) bcm_bst_th3_hw_stat_clear(unit, resInfo, bid, buffer,
                                                                gport, -1, cosq);
    } else {
        /* Clear stat counter for given port, cos */
        rv = bst_info->handlers.resolve_index(unit, gport, cosq, bid, &pipe,
                            &start_hw_index, &end_hw_index, &rcb,
                            &cb_data, &bcm_rv);
        if (rv != _BCM_BST_RV_ERROR) {
            for (hw_index = start_hw_index; hw_index <= end_hw_index; hw_index++)
            {
                (void) bcm_bst_th3_hw_stat_clear(unit, resInfo, bid, buffer,
                                                      gport, hw_index,
                                                      cosq);
            }
        }
    }

    if (gport == -1) {
        bcm_port_t local_port;
        PBMP_PORT_ITER(unit, local_port) {
                bcm_rv = bcm_bst_th3_sw_stat_clear(unit, local_port,
                                                         cosq, buffer, bid);
            if (BCM_FAILURE(bcm_rv)) {
                break;
            }
        }
    } else {
        {
            bcm_rv = bcm_bst_th3_sw_stat_clear(unit, gport, cosq, buffer, bid);
        }
    }
    sal_mutex_give(bst_info->bst_reslock);

    return bcm_rv;

}

/*
 * Function:
 *   bcm_th3_cosq_bst_stat_multi_get
 * Purpose:
 *  Get multiple BST stat counts.
 * Parameters:
 *     unit       - (IN) unit number.
 *     gport - (IN) gport number.
 *     cosq  - (IN) cosq number.
 *     options - (IN) BST stat read options,
 *                   used for setting clear-or-read FLAG.
 *     max_values - (IN) size of BST stat ID array.
 *     id_list    - (IN) BST stat ID array.
 *     pvalues    - (OUT) BST stat count array.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_bst_stat_multi_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               uint32 options,
                               int max_values,
                               bcm_bst_stat_id_t *id_list,
                               uint64 *pvalues)
{
    int i;

    for (i = 0; i < max_values; i++) {

        BCM_IF_ERROR_RETURN(bcm_th3_cosq_bst_stat_get(unit, gport, cosq,
                                    BCM_COSQ_BUFFER_ID_INVALID, id_list[i],
                                                       options, &pvalues[i]));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_th3_bst_sw_index_get
 * Purpose:
 *  Retrieve SW datastructure index corresponding to HW indes for a bid.
 * Parameters:
 *     unit       - (IN) unit number.
 *     gport - (IN) gport number.
 *     cosq  - (IN) cosq number.
 *     bid   - (IN) BST ID.
 *     hw_index     - (IN) Hardware index.
 *     num_sw_entries - (OUT) Number of software entries corresponding to the
 *     hardware entry
 *     sw_index_arr  - (OUT) List of software indices corresponding to the HW
 *     index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_bst_sw_index_get (int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_bst_stat_id_t bid, int hw_index, int *num_sw_entries, int *sw_index_arr)
{

    int cosq_loop_id;
    int local_port, pool = -1;
    int pool_start, pool_end;

    if ((num_sw_entries == NULL) || (sw_index_arr == NULL)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
       switch(bid) {
            case bcmBstStatIdEgrPortPoolSharedUcast:
                /* Retrieve the service pools mapped to given gport,cosq*/
                BCM_IF_ERROR_RETURN(
                    _bcm_th3_cosq_egress_sp_get (unit, gport, cosq, &pool_start,
                        &pool_end));
                *num_sw_entries = (pool_end - pool_start) + 1;
                cosq_loop_id = 0;
                for (pool = pool_start; pool <= pool_end; pool++) {
                    sw_index_arr[cosq_loop_id] =
                            (hw_index * _TH3_MMU_NUM_POOL) + pool;
                    cosq_loop_id++;
                }
                break;
            case bcmBstStatIdPriGroupShared:
            case bcmBstStatIdPriGroupHeadroom:
                if (cosq == BCM_COS_INVALID) {
                    *num_sw_entries = _TH3_MMU_NUM_PG;
                    for (cosq_loop_id = 0; cosq_loop_id < _TH3_MMU_NUM_PG;
                            cosq_loop_id++) {
                        sw_index_arr[cosq_loop_id] =
                            (hw_index * _TH3_MMU_NUM_PG) + cosq_loop_id;
                    }
                } else {
                    *num_sw_entries = 1;
                    sw_index_arr[0] = (hw_index * _TH3_MMU_NUM_PG) + cosq;
                }
                break;
            case bcmBstStatIdPortPool:
                if (cosq == BCM_COS_INVALID) {
                    *num_sw_entries = _TH3_MMU_NUM_POOL;
                    for (pool = 0; pool < _TH3_MMU_NUM_POOL;
                            pool++) {
                        sw_index_arr[pool] =
                            (hw_index * _TH3_MMU_NUM_POOL) + pool;
                    }

                } else {
                    *num_sw_entries = 1;
                    /* Retrieve Pool mapped to given PG of the port*/
                    BCM_IF_ERROR_RETURN(
                            _bcm_th3_cosq_ingress_sp_get_by_pg(unit, local_port,
                                                        cosq, &pool));
                    sw_index_arr[0] = (hw_index * _TH3_MMU_NUM_POOL) + pool;
                }
                break;
            default:
                *num_sw_entries = 1;
                sw_index_arr[0] = hw_index;
                break;
        }
    return BCM_E_NONE;
}
/*
 * Function:
 *   bcm_bst_th3_sw_stat_clear
 * Purpose:
 *  Clear specific BST stat counts in SW.
 * Parameters:
 *     unit       - (IN) unit number.
 *     gport - (IN) gport number.
 *     cosq  - (IN) cosq number.
 *     buffer - (IN) ITM id.
 *     bid    - (IN) BST stat ID.
  * Returns:
 *     BCM_E_XXX
 */

int
bcm_bst_th3_sw_stat_clear(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_cosq_buffer_id_t buffer, bcm_bst_stat_id_t bid)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL;
    int bcm_rv, rcb = 0, index;
    int start_hw_index = 0, end_hw_index = 0, pipe;
    uint32 mmu_inst_map = 0;
    int entries_per_inst, inst_offset, inst;
    int num_sw_entries, loop_idx, sw_index;
     /*max number of sw entries per hw index is 8 (number of PGS)*/
    int sw_idx_arr[_TH3_MMU_NUM_PG] = {-1, -1, -1, -1, -1, -1, -1, -1};

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->handlers.resolve_index == NULL) {
        return BCM_E_UNAVAIL;
    }

    do {
        rv = bst_info->handlers.resolve_index(unit, gport, cosq, bid, &pipe,
                    &start_hw_index, &end_hw_index, &rcb, &cb_data, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }

        if (bcmBstStatIdUcast == bid || bcmBstStatIdMcast == bid) {
            bid = bcmBstStatIdQueueTotal;
        }
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        entries_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
        if (bst_info->port_to_mmu_inst_map) {
            (void)bst_info->port_to_mmu_inst_map(unit, bid, gport,
                                                 &mmu_inst_map);
        }
        if (!mmu_inst_map) {
            return BCM_E_PARAM;
        }

        /* When a resource has more than one instance, clear all
         * applicable instances.
         */
        for (index = start_hw_index; index <= end_hw_index; index++) {
            for (inst = 0; inst < resInfo->num_instance; inst++) {
                if (mmu_inst_map & (1 << inst)) {
                    inst_offset = inst * entries_per_inst;
                    /* retrieve the sw entries mapped to each hw index*/
                    BCM_IF_ERROR_RETURN(
                        _bcm_th3_bst_sw_index_get(unit, gport, cosq, bid,
                             index, &num_sw_entries, sw_idx_arr));
                    if (num_sw_entries > resInfo->num_field) {
                        return BCM_E_INTERNAL;
                    }
                    for (loop_idx = 0; loop_idx < num_sw_entries; loop_idx++) {
                        if (sw_idx_arr[loop_idx] != -1) {
                            sw_index = sw_idx_arr[loop_idx] + inst_offset;
                            if (sw_index > resInfo->index_max) {
                                return BCM_E_INTERNAL;
                            }
                            resInfo->p_stat[sw_index] = 0;
                        }
                    }
                 }
             }
         }

    } while (rv == _BCM_BST_RV_RETRY);
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcm_th3_cosq_bst_stat_get
 * Purpose:
 *  Get a specific BST stat count saved in SW.
 * Parameters:
 *     unit       - (IN) unit number.
 *     gport - (IN) gport number.
 *     cosq  - (IN) cosq number.
 *     buffer - (IN) ITM id.
 *     bid    - (IN) BST stat ID.
 *     options - (IN) BST stat read options,
 *                   used for setting clear-or-read FLAG.
 *     pvalue    - (OUT) BST stat count.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_bst_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid,
    uint32 options,
    uint64 *pvalue)

{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL;
    int bcm_rv, rcb = 0, ii, inst, entries_per_inst;
    int start_hw_index = 0, end_hw_index = 0, pipe = 0;
    uint32 mmu_inst_map = 0;
    int temp_stat , max_stat;
    int num_sw_entries, loop_idx, sw_index;
     /*max number of sw entries per hw index is 8 (number of PGS)*/
    int sw_idx_arr[_TH3_MMU_NUM_PG] = {-1, -1, -1, -1, -1, -1, -1, -1};


    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->handlers.resolve_index == NULL) {
        return BCM_E_UNAVAIL;
    }
    COMPILER_64_ZERO(*pvalue);

    BCM_IF_ERROR_RETURN(
            _bcm_th3_bst_bid_gport_cosq_param_check(unit, bid, gport, cosq));

    do {
        rv = bst_info->handlers.resolve_index(unit, gport, cosq, bid, &pipe,
                    &start_hw_index, &end_hw_index, &rcb, &cb_data, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        if (bcmBstStatIdUcast == bid || bcmBstStatIdMcast == bid) {
            bid = bcmBstStatIdQueueTotal;
        }
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
            return BCM_E_PARAM;
        }

        entries_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
        if (bst_info->port_to_mmu_inst_map) {
            (void) bst_info->port_to_mmu_inst_map(unit, bid, gport,
                                                 &mmu_inst_map);
        }
        
        if (!mmu_inst_map) {
            return BCM_E_PARAM;
        }
        /* When a resource has more than one instance, find the max value
         * of all applicable instances and return the Max value.
         */
        for (ii = start_hw_index; ii <= end_hw_index; ii++) {
            temp_stat = 0;
            max_stat = 0;
            for (inst = 0; inst < resInfo->num_instance; inst++) {
                /* read only the specific buffer */
                if (mmu_inst_map & (1 << inst)) {
                    /* retrieve the sw entries mapped to each hw index*/
                    BCM_IF_ERROR_RETURN(
                        _bcm_th3_bst_sw_index_get(unit, gport, cosq, bid,
                             ii, &num_sw_entries, sw_idx_arr));
                    if (num_sw_entries > resInfo->num_field) {
                        return BCM_E_INTERNAL;
                    }
                    for (loop_idx = 0; loop_idx < num_sw_entries; loop_idx++) {
                        if (sw_idx_arr[loop_idx] != -1) {
                            sw_index = sw_idx_arr[loop_idx] +
                                (inst * entries_per_inst);
                            temp_stat = resInfo->p_stat[sw_index];
                            if (temp_stat > max_stat) {
                                max_stat = temp_stat;
                            }
                        }
                    }
                }
            }
            COMPILER_64_ADD_32(*pvalue, max_stat);
        }

    } while (rv == _BCM_BST_RV_RETRY);

    bcm_rv = BCM_E_NONE;

    if (options & BCM_COSQ_STAT_CLEAR) {
        int need_hw_stat_clear = 1;
        int mode = bst_info->track_mode;

        sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
        if ((mode == 1) &&
                (bst_info->hw_cor_in_max_use_count == 1)) {
            /* no need hw stat clear */
            need_hw_stat_clear = 0;
        }
        if(need_hw_stat_clear) {
             (void) bcm_bst_th3_hw_stat_clear(unit, resInfo, bid, buffer,
                                                    gport, start_hw_index, cosq);
        }
        bcm_rv = bcm_bst_th3_sw_stat_clear(unit, gport, cosq, buffer, bid);
        sal_mutex_give(bst_info->bst_reslock);
    }


    return bcm_rv;
}

/*
 * Function:
 *   bcm_bst_th3_index_resolve
 * Purpose:
 *  Resolve the index range for a specific resource based on gport and cosq.
 * Parameters:
 *     unit       - (IN) unit number.
 *     gport - (IN) gport number.
 *     cosq  - (IN) cosq number.
 *     bid    - (IN) BST stat ID.
 *     pipe -  (OUT) pipe number.
 *     start_hw_index - (OUT) start of index range.
 *     end_hw_index - (OUT) end of index range.
 *     rcb_data, cb_data    - (OUT) NOT used.
 *     bcm_rv - (OUT) error status indicator.
 * Returns:
 *     _BCM_BST_RV_*
 */

_bcm_bst_cb_ret_t
bcm_bst_th3_index_resolve(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid, int *pipe, int *start_hw_index,
                          int *end_hw_index, int *rcb_data, void **cb_data, int *bcm_rv)
{
    int local_port, mmu_lport, mmu_chip_port;
    soc_info_t *si = &SOC_INFO(unit);

    *bcm_rv = _bcm_th3_cosq_localport_resolve(unit, gport, &local_port);

    if (BCM_E_NONE != (*bcm_rv)) {
        return _BCM_BST_RV_ERROR;
    }

    mmu_lport = soc_th3_mmu_local_port_num(unit, local_port);
    mmu_chip_port = soc_th3_mmu_chip_port_num(unit, local_port);
    *pipe = si->port_pipe[local_port];
    switch (bid) {
        case bcmBstStatIdDevice:
            *start_hw_index = *end_hw_index = 0;
            *bcm_rv = BCM_E_NONE;
            return _BCM_BST_RV_OK;
        case bcmBstStatIdHeadroomPool:
            if(cosq == BCM_COS_INVALID) {
                *start_hw_index = 0;
                *end_hw_index = _TH3_MMU_NUM_POOL - 1;
                *bcm_rv = BCM_E_NONE;
                return _BCM_BST_RV_OK;
            }
            *bcm_rv = _bcm_th3_cosq_ingress_hdrm_pool_get_by_pg(unit, gport,
                                                    cosq, start_hw_index);
            *end_hw_index = *start_hw_index;
            if(*bcm_rv != BCM_E_NONE) {
                return _BCM_BST_RV_ERROR;
            }
            break;
        case bcmBstStatIdIngPool:
            if(cosq == BCM_COS_INVALID) {
                *start_hw_index = 0;
                *end_hw_index = _TH3_MMU_NUM_POOL - 1;
                *bcm_rv = BCM_E_NONE;
                return _BCM_BST_RV_OK;
            }
            *bcm_rv = _bcm_th3_cosq_ingress_sp_get_by_pg(unit, gport,
                                                    cosq, start_hw_index);
            *end_hw_index = *start_hw_index;
            if(*bcm_rv != BCM_E_NONE) {
                return _BCM_BST_RV_ERROR;
            }
            break;
        case bcmBstStatIdPortPool:
            *start_hw_index = mmu_lport;
            *end_hw_index = mmu_lport;
            break;
        case bcmBstStatIdPriGroupShared:
        case bcmBstStatIdPriGroupHeadroom:
            *start_hw_index = mmu_lport;
            *end_hw_index = mmu_lport;
            break;
        case bcmBstStatIdEgrPool:
        case bcmBstStatIdEgrMCastPool:
            if(_bcm_th3_cosq_egress_sp_get(unit, gport, cosq, start_hw_index,
                                      end_hw_index)) {
                *bcm_rv = BCM_E_PARAM;
                return _BCM_BST_RV_ERROR;
            }
            *bcm_rv = BCM_E_NONE;
            return _BCM_BST_RV_OK;
        case bcmBstStatIdEgrPortPoolSharedMcast:
            if(_bcm_th3_cosq_egress_sp_get(unit, gport, cosq, start_hw_index,
                                      end_hw_index)) {
                *bcm_rv = BCM_E_PARAM;
                return _BCM_BST_RV_ERROR;
            }
            *start_hw_index = mmu_chip_port * _TH3_MMU_NUM_POOL + *start_hw_index;
            *end_hw_index = mmu_chip_port * _TH3_MMU_NUM_POOL + *end_hw_index;
            break;
        case bcmBstStatIdEgrPortPoolSharedUcast:
            *start_hw_index = mmu_chip_port;
            *end_hw_index = mmu_chip_port;
            break;
        case bcmBstStatIdUcast:
            if (IS_CPU_PORT(unit, local_port)) {
                *bcm_rv = BCM_E_PARAM;
                return _BCM_BST_RV_ERROR;
            }
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                *bcm_rv = _bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, start_hw_index, NULL);
                if (cosq == -1) {
                    *end_hw_index = *start_hw_index +
                        si->port_num_uc_cosq[local_port] - 1;
                } else {
                    *end_hw_index = *start_hw_index;
                }
            } else {
                /*cosq ibput passed in {port, cosq} format is treated as MMUQ
                 * number*/
                if (cosq == -1) {
                    *start_hw_index = si->port_uc_cosq_base[local_port];
                    *end_hw_index = *start_hw_index +
                        si->port_num_uc_cosq[local_port] - 1;
                } else {
                    *start_hw_index = si->port_uc_cosq_base[local_port] + cosq;
                    *end_hw_index = *start_hw_index;
                }
            }
            if (*bcm_rv != BCM_E_NONE) {
                return _BCM_BST_RV_ERROR;
            }
            break;
        case bcmBstStatIdMcast:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                *bcm_rv = _bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, start_hw_index, NULL);
                if (cosq == -1) {
                    *end_hw_index = *start_hw_index +
                        si->port_num_cosq[local_port] - 1;
                } else {
                    *end_hw_index = *start_hw_index;
                }
            } else {
                /*cosq input passed in {port, cosq} format is treated as MMUQ
                 * number*/
                if (cosq == -1) {
                    *start_hw_index = si->port_cosq_base[local_port];
                    *end_hw_index = *start_hw_index +
                        si->port_num_cosq[local_port] - 1;
                } else {
                    *start_hw_index = si->port_cosq_base[local_port] +
                        (cosq - si->port_num_uc_cosq[local_port]);
                    *end_hw_index = *start_hw_index;
                }
            }
            if (*bcm_rv != BCM_E_NONE) {
                return _BCM_BST_RV_ERROR;
            }
            break;
        case bcmBstStatIdQueueTotal:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                *bcm_rv = _bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, start_hw_index, NULL);
                if (cosq == -1) {
                    *end_hw_index = *start_hw_index + _bcm_th3_get_num_ucq(unit) - 1;
                } else {
                    *end_hw_index = *start_hw_index;
                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                *bcm_rv = _bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, start_hw_index, NULL);
                if (cosq == -1) {
                    *end_hw_index = *start_hw_index + _bcm_th3_get_num_mcq(unit) - 1;
                } else {
                    *end_hw_index = *start_hw_index;
                }

            } else {
                /*cosq ibput passed in {port, cosq} format is treated as MMUQ
                 * number*/
                if (cosq == -1) {
                    if (!IS_CPU_PORT(unit, local_port)) {
                        *start_hw_index = si->port_uc_cosq_base[local_port];
                        *end_hw_index = *start_hw_index +
                            si->port_num_uc_cosq[local_port] +
                            si->port_num_cosq[local_port] - 1;
                    } else {
                        *start_hw_index = si->port_cosq_base[local_port];
                        *end_hw_index = *start_hw_index +
                            si->port_num_cosq[local_port] - 1;
                    }
                } else {
                    if (!IS_CPU_PORT(unit, local_port)) {
                        *start_hw_index = si->port_uc_cosq_base[local_port]
                            + cosq;
                    } else {
                        *start_hw_index = si->port_cosq_base[local_port] + cosq;
                    }
                    *end_hw_index = *start_hw_index;
                }
            }
            if (*bcm_rv != BCM_E_NONE) {
                return _BCM_BST_RV_ERROR;
            }
            break;
        default:
            *bcm_rv = BCM_E_PARAM;
            return _BCM_BST_RV_ERROR;
    }
    *bcm_rv = BCM_E_NONE;
    return _BCM_BST_RV_OK;
}

/*
 * Function:
 *   _bcm_th3_bst_profile_read_mem_range
 * Purpose:
 *  Set/Get a specific threshold profile/value memory range in HW for a given resource.
 * Parameters:
 *     unit   - (IN) unit number.
 *     flags  - (IN) flags for read or write
 *     bid    - (IN) BST stat ID.
 *     inst   - (IN) inst number (pipe number or ITM number).
 *     buffer - (OUT) memory buffer
 * Returns:
 *     _BCM_BST_RV_*
 */

STATIC int
_bcm_th3_bst_profile_mem_range(int unit, int flags, bcm_bst_stat_id_t bid,
                                int inst, uint8 **buffer)
{
    _bcm_bst_resource_info_t *resInfo;
    soc_mem_t mem = INVALIDm;
    int idx_min, idx_max;

    if (buffer == NULL) {
        return BCM_E_PARAM;
    }

    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if(resInfo == NULL || !(resInfo->valid)) {
        return BCM_E_BADID;
    }
    if(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
        if(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            mem = resInfo->threshold_mem[inst];
        } else {
            mem = resInfo->threshold_mem[0];
        }
    } else {
        /* this function doesn't support registers.  Only memory range */
        return BCM_E_PARAM;
    }

    if(mem != INVALIDm) {
        idx_min = soc_mem_index_min(unit, mem);
        idx_max = soc_mem_index_max(unit, mem);

        if(flags & BST_OP_GET) {
            *buffer = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, mem),
                                          "profile_read_mem_range");
            if (*buffer == NULL) {
                return BCM_E_MEMORY;
            }
            /* Calling function will free up memory */
            BCM_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            idx_min, idx_max, *buffer));
        } else if (flags & BST_OP_SET) {
            /* Calling function will free up memory */
            BCM_IF_ERROR_RETURN(soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                        idx_min, idx_max, *buffer));
        }
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_th3_bst_profile_op_mem_buffer
 * Purpose:
 *  Set/Get a specific threshold profile/value in buffer for a given memory.
 * Parameters:
 *     unit       - (IN) unit number.
 *     flags  - (IN) operation type, i.e. GET or SET.
 *     bid    - (IN) BST stat ID.
 *     inst -  (IN) inst number (pipe number or ITM number).
 *     sw_index - (IN) memory-entry index
 *     buffer - (IN) RAM buffer to read/write from/to
 *     resval - (IN) for SET, (OUT) for GET
 * Returns:
 *     _BCM_BST_RV_*
 */
STATIC int
_bcm_th3_bst_profile_op_mem_buffer(int unit, int flags, bcm_bst_stat_id_t bid,
                                int inst, int sw_index, uint8 * buffer, uint32 *resval)
{
    _bcm_bst_resource_info_t *resInfo;
    int num_field, idx_min, idx_max;
    soc_mem_t mem = INVALIDm;
    soc_field_t field;
    int hw_index = sw_index;
    uint32 *entry;
    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if(resInfo == NULL || !(resInfo->valid)) {
        return BCM_E_BADID;
    }
    if(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
        if(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            mem = resInfo->threshold_mem[inst];
        } else {
            mem = resInfo->threshold_mem[0];
        }
    } else {
        /* This function only supports memory, not registers */
        return BCM_E_PARAM;
    }

    if(mem != INVALIDm) {
        if((field = resInfo->threshold_field) == INVALIDf) {
            /*  multiple sel field within one mem_entry */
            num_field = resInfo->num_field;
            field = resInfo->thd_fields[sw_index % num_field];
            hw_index = sw_index / num_field;
        }
        /* Sanity check */
        idx_min = soc_mem_index_min(unit, mem);
        idx_max = soc_mem_index_max(unit, mem);
        if (hw_index < idx_min || hw_index > idx_max) {
            return BCM_E_PARAM;
        }

        /* Get a pointer to the entry in the memory buffer */
        entry = soc_mem_table_idx_to_pointer(unit, mem, uint32 *, buffer,
                        hw_index);
        if(flags & BST_OP_GET) {
            *resval = soc_mem_field32_get(unit, mem, entry, field);
        } else if (flags & BST_OP_SET){
           soc_mem_field32_set(unit, mem, entry, field, *resval);
        }

    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *   _bcm_th3_bst_profile_op
 * Purpose:
 *  Set/Get a specific threshold profile/value in HW for a given resource.
 * Parameters:
 *     unit       - (IN) unit number.
 *     flags  - (IN) operation type, i.e. GET or SET.
 *     bid    - (IN) BST stat ID.
 *     inst -  (OUT) inst number (pipe number or ITM number).
 *     sw_index - (IN) register/memory-entry index
 *     resval - (IN) for SET, (OUT) for GET
 * Returns:
 *     _BCM_BST_RV_*
 */

STATIC int
_bcm_th3_bst_profile_op(int unit, int flags, bcm_bst_stat_id_t bid,
                                int inst, int sw_index, uint32 *resval)
{
    _bcm_bst_resource_info_t *resInfo;
    int i, num_field;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    int hw_index = sw_index;
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if(resInfo == NULL || !(resInfo->valid)) {
        return BCM_E_BADID;
    }
    if(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
        if(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            mem = resInfo->threshold_mem[inst];
        } else {
            mem = resInfo->threshold_mem[0];
        }
    } else {
        reg = resInfo->threshold_reg[0];
        field = resInfo->threshold_field;
    }

    if(mem != INVALIDm) {
        if((field = resInfo->threshold_field) == INVALIDf) {
            /*  multiple sel field within one mem_entry */
            num_field = resInfo->num_field;
            field = resInfo->thd_fields[sw_index % num_field];
            hw_index = sw_index / num_field;
        }
        BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, entry));
        if(flags & BST_OP_GET) {
            *resval = soc_mem_field32_get(unit, mem, entry, field);
        } else if (flags & BST_OP_SET){
           soc_mem_field32_set(unit, mem, entry, field, *resval);
           BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, entry));
        }

    } else if (reg != INVALIDr) {

        if(bid == bcmBstStatIdDevice) {
            if(inst == -1) {
                for(i = 0; i < NUM_ITM(unit); i++) {
                    uint32 temp;
                    inst = i | SOC_REG_ADDR_INSTANCE_MASK;
                    SOC_IF_ERROR_RETURN(
                            soc_reg32_get(unit, reg, inst,
                                                        hw_index, &rval));
                    if(flags & BST_OP_GET) {
                        temp = soc_reg_field_get(unit, reg, rval, field);
                        if(temp > *resval) {
                            *resval = temp;
                        }

                    } else if (flags & BST_OP_SET) {

                        soc_reg_field_set(unit, reg, &rval, field, *resval);
                        SOC_IF_ERROR_RETURN(
                             soc_reg32_set(unit, reg, inst, hw_index, rval));
                    }
                }
            }
            else {
                 inst = inst | SOC_REG_ADDR_INSTANCE_MASK;
                 SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, reg, inst, hw_index, &rval));
                 if(flags & BST_OP_GET) {
                    *resval = soc_reg_field_get(unit, reg, rval, field);
                 } else if (flags & BST_OP_SET) {
                    soc_reg_field_set(unit, reg, &rval, field, *resval);
                    SOC_IF_ERROR_RETURN(
                            soc_reg32_set(unit, reg, inst, hw_index, rval));
                 }
            }
        } else {
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, reg, REG_PORT_ANY, hw_index, &rval));
             if(flags & BST_OP_GET) {
                *resval = soc_reg_field_get(unit, reg, rval, field);
             } else if(flags & BST_OP_SET){
                soc_reg_field_set(unit, reg, &rval, field, *resval);
                SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, reg, REG_PORT_ANY, hw_index, rval));
             }
        }
    }
    return BCM_E_NONE;
}


/* Function:
 *  _bst_th3_cmn_profile_get_hw
 * Purpose:
 * Get threshold of specified bid from pipe and sw_index for profile_based
 * threshold, get threshold from itm and sw_index for non-profile based
 * threshold.
 * Parameters:
 *     unit       - (IN) unit number.
 *     pipe    - (IN) pipe number.
 *     itm  - (IN) ITM number.
 *     sw_index - (IN) register/memory-entry index
 *     bid    - (IN) BST stat ID.
 *     profile - (OUT) pointer to threshold struct.
*/
STATIC int
_bst_th3_cmn_profile_get_hw(int unit, int pipe, int itm, int sw_index,
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    uint32 val, rval, reg_id;
    soc_reg_t reg;
    soc_field_t field;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if (resInfo == NULL || !resInfo->valid) {
        return BCM_E_PARAM;
    }
    val = 0;
    if(!(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID)) {
        /* non-profile based threshold, val is # of cells */
        BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_profile_op(unit, BST_OP_GET, bid, itm, sw_index,
                                             &val));
        profile->byte = bst_info->to_byte(unit, val);
    } else {
        /* profile-based threshold, val is the profile index */
            reg_id = _bcm_bst_th3_profile_reg_id_get(unit, bid, sw_index);
            reg = resInfo->profile_reg[reg_id];
            sw_index &= _TH3_BST_QUEUE_TYPE_SHIFT_MASK;
        BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_profile_op(unit, BST_OP_GET, bid, pipe, sw_index,
                                                               &val));
        field = resInfo->profile_field;
        if(reg != INVALIDr) {
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit, reg, REG_PORT_ANY, val, &rval));
            val = soc_reg_field_get(unit, reg, rval, field);
            profile->byte = bst_info->to_byte(unit, val);

        } else {
            return BCM_E_INIT;
        }

    }

    return BCM_E_NONE;
}

/* Function:
 *  _bst_th3_cmn_profile_set_hw
 * Purpose:
 * Set threshold of specified bid from pipe and sw_index for profile_based
 * thresholds, set threshold from itm and sw_index for non-profile based
 * thresholds.
 * Parameters:
 *     unit       - (IN) unit number.
 *     pipe    - (IN) pipe number.
 *     itm  - (IN) ITM number.
 *     sw_index - (IN) register/memory-entry index
 *     bid    - (IN) BST stat ID.
 *     profile - (IN) pointer to threshold struct.
 *     p_profile - (OUT) allocated profile index.
*/

/*
 * Usage of itm (all BST threshold profile are duplicated except CFAP):
 *   -1:    all ITMs related to the pipe with same value, reg[i] = profile[0]
 *  0-1 :   ITM num, applicable only for bid = bcmBstStatIdDevice
 *  _TH3_BST_PROFILE_INIT_FLAG   for BST profile init only
 */

STATIC int
_bst_th3_cmn_profile_set_hw(int unit, int pipe, int itm, int sw_index,
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile,
                            uint32 *p_profile)
{
    uint32 cell;
    uint32 profile_index, old_profile_index, reg_id;
    int inst, inst_offset;
    uint64 rval;
    uint64 *prval[1];
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if(resInfo == NULL || resInfo->valid == 0) {
        return BCM_E_INIT;
    }
    if (profile->byte > bst_info->max_bytes) {
        return BCM_E_PARAM;
    }
    cell = bst_info->to_cell(unit, profile->byte);
    inst_offset = (resInfo->index_max + 1) / (resInfo->num_instance);

    reg_id = _bcm_bst_th3_profile_reg_id_get(unit, bid, sw_index);

    sw_index &= _TH3_BST_QUEUE_TYPE_SHIFT_MASK;
    /*update sw thd */
    if(sw_index >= 0) {
        if(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            /* for per-piped thresh, only update the specfic pipe */
            resInfo->p_threshold[sw_index + pipe * inst_offset] = cell;
        } else {
            /* ITM based threshold */
            if (bid == bcmBstStatIdDevice) {
                for(inst = 0; inst < resInfo->num_instance; inst++) {
                    /*update both ITMs for threhsolds of unique access type */
                    resInfo->p_threshold[sw_index + inst * inst_offset] = cell;
                }
            } else {
                /*For trhehsolds in ITM with duplicate access type, upadate only
                 * ITM 0*/
                resInfo->p_threshold[sw_index] = cell;
            }
        }
    }
    if(!(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID)) {
        /* non-profile based threshold */
        BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_profile_op(unit, BST_OP_SET, bid, itm, sw_index,
                                                          &cell));
        return BCM_E_NONE;
    }
    /* add new profile */
    profile_index = old_profile_index = -1;

    if (resInfo->profile_r[reg_id]) {
        COMPILER_64_ZERO(rval);
        soc_reg64_field32_set(unit, resInfo->profile_reg[reg_id], &rval,
                                            resInfo->profile_field, cell);
        prval[0] = &rval;
        SOC_IF_ERROR_RETURN(soc_profile_reg_add
                                    (unit, resInfo->profile_r[reg_id],
                                                prval, 1, &profile_index));
    }
    if(profile_index != -1)
    {
        /* get old profile ID */
        BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_profile_op(unit, BST_OP_GET, bid, pipe, sw_index,
                                                 &old_profile_index));

        if(old_profile_index != profile_index) {
            /* attach new profile */
            BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_profile_op(unit, BST_OP_SET, bid, pipe, sw_index,
                                                        &profile_index));
           /* delete old profile */
           if(itm != _TH3_BST_PROFILE_INIT_FLAG) {
                if(resInfo->profile_r[reg_id]) {
                    SOC_IF_ERROR_RETURN
                        (soc_profile_reg_delete(unit, resInfo->profile_r[reg_id],
                                                     old_profile_index));
                }
           }
        }
        if(p_profile) {
            *p_profile = profile_index;
        }
    }

    return BCM_E_NONE;
}

/* Function:
 *  _bst_th3_cmn_profile_set
 * Purpose:
 * Set threshold of specified bid from gport/cosq/ITM 
 * Parameters:
 *     unit       - (IN) unit number.
 *     pipe    - (IN) pipe number.
 *     itm  - (IN) ITM number.
 *     sw_index - (IN) register/memory-entry index
 *     bid    - (IN) BST stat ID.
 *     profile - (IN) pointer to threshold struct.
 */
/*BST threshold configuration is Unique only for bcmBstStatIdDevice in TH3*/
int
_bcm_th3_bst_cmn_profile_set (int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int buffer, bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL;
    int bcm_rv, ii, rcb = 0, pipe;
    int start_hw_index = 0, end_hw_index = 0;
    _bcm_bst_resource_info_t *resInfo;
    int sw_idx_loop, num_sw_entries, sw_index;
    int sw_idx_arr[_TH3_MMU_NUM_PG] = {-1, -1, -1, -1, -1, -1, -1, -1};
    soc_info_t *si = &SOC_INFO(unit);
    int local_port, index;

    /* Parameter check */
    if (bid < bcmBstStatIdInvalid || bid >= bcmBstStatIdMaxCount) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(
            _bcm_th3_bst_bid_gport_cosq_param_check(unit, bid, gport, cosq));

    if (profile == NULL) {
        return BCM_E_PARAM;
    }

    if (bst_info->handlers.resolve_index == NULL) {
        return BCM_E_UNAVAIL;
    }

    do {
        rv = bst_info->handlers.resolve_index(unit, gport, cosq, bid, &pipe,
                    &start_hw_index, &end_hw_index, &rcb, &cb_data, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
            bid = bcmBstStatIdQueueTotal;
        }
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
            return BCM_E_PARAM;
        }

        for (ii = start_hw_index; ii <= end_hw_index; ii++) {
            BCM_IF_ERROR_RETURN(
                        _bcm_th3_bst_sw_index_get(unit, gport, cosq, bid,
                             ii, &num_sw_entries, sw_idx_arr));
            if (num_sw_entries > resInfo->num_field) {
                        return BCM_E_INTERNAL;
            }
            for (sw_idx_loop = 0; sw_idx_loop < num_sw_entries;
                sw_idx_loop ++) {
                sw_index = sw_idx_arr[sw_idx_loop];
                if (sw_index != -1) {
                    if (bid == bcmBstStatIdQueueTotal) {
                        BCM_IF_ERROR_RETURN(
                                _bcm_th3_cosq_localport_resolve(unit, gport,
                                    &local_port));
                        if (IS_CPU_PORT(unit, local_port)) {
                            index = sw_index |
                                (_BCM_TH3_BST_Q_TYPE_CPU <<
                                 _TH3_BST_QUEUE_TYPE_SHIFT);
                        } else {
                            if (sw_index < si->port_cosq_base[local_port]) {
                                /*Unicast queue of port*/
                                index = sw_index |
                                    (_BCM_TH3_BST_Q_TYPE_UC <<
                                    _TH3_BST_QUEUE_TYPE_SHIFT);
                            } else {
                                /*Multicast queue of port*/
                                index = sw_index |
                                    (_BCM_TH3_BST_Q_TYPE_MC <<
                                    _TH3_BST_QUEUE_TYPE_SHIFT);
                            }
                        }
                            BCM_IF_ERROR_RETURN(
                                _bst_th3_cmn_profile_set_hw(unit, pipe, buffer,
                                    index, bid, profile, NULL));
                    } else {
                        BCM_IF_ERROR_RETURN(
                            _bst_th3_cmn_profile_set_hw(unit, pipe, buffer,
                                sw_index, bid, profile, NULL));
                    }
                }
            }
        }

    } while (rv == _BCM_BST_RV_RETRY);
    return BCM_E_NONE;
}

/* Function:
 *  _bst_th3_cmn_profile_get
 * Purpose:
 * Get threshold of specified bid from gport/cosq/ITM 
 * Parameters:
 *     unit       - (IN) unit number.
 *     pipe    - (IN) pipe number.
 *     itm  - (IN) ITM number.
 *     sw_index - (IN) register/memory-entry index
 *     bid    - (IN) BST stat ID.
 *     profile - (OUT) pointer to threshold struct.
 */
int
_bcm_th3_bst_cmn_profile_get (int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int buffer, bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile)
{
        _bcm_bst_cmn_unit_info_t *bst_info;
    void *cb_data = NULL;
    int bcm_rv, rcb = 0, pipe;
    int start_hw_index = 0, end_hw_index = 0;
    _bcm_bst_resource_info_t *resInfo;
    int sw_idx_loop, num_sw_entries, sw_index;
    int sw_idx_arr[_TH3_MMU_NUM_PG] = {-1, -1, -1, -1, -1, -1, -1, -1};
    soc_info_t *si = &SOC_INFO(unit);
    int local_port, index;

    /* Parameter check */
    if (bid < bcmBstStatIdInvalid || bid >= bcmBstStatIdMaxCount) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
            _bcm_th3_bst_bid_gport_cosq_param_check(unit, bid, gport, cosq));

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }


    if (profile == NULL) {
        return BCM_E_PARAM;
    }

    if (bst_info->handlers.resolve_index == NULL) {
        return BCM_E_UNAVAIL;
    }

    bst_info->handlers.resolve_index(unit, gport, cosq, bid, &pipe,
                &start_hw_index, &end_hw_index, &rcb, &cb_data, &bcm_rv);
    if (bcm_rv) {
        return bcm_rv;
    }

    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
            _bcm_th3_bst_sw_index_get(unit, gport, cosq, bid,
                             start_hw_index, &num_sw_entries, sw_idx_arr));
    if (num_sw_entries > resInfo->num_field) {
        return BCM_E_INTERNAL;
    }
    for (sw_idx_loop = 0; sw_idx_loop < num_sw_entries;
        sw_idx_loop ++) {
        sw_index = sw_idx_arr[sw_idx_loop];
        if (sw_index != -1) {
            if (bid == bcmBstStatIdQueueTotal) {
                BCM_IF_ERROR_RETURN(
                        _bcm_th3_cosq_localport_resolve(unit, gport,
                            &local_port));
                if (IS_CPU_PORT(unit, local_port)) {
                    index = sw_index |
                        (_BCM_TH3_BST_Q_TYPE_CPU <<
                         _TH3_BST_QUEUE_TYPE_SHIFT);
                } else {
                    if (sw_index < si->port_cosq_base[local_port]) {
                        /*Unicast queue of port*/
                        index = sw_index |
                            (_BCM_TH3_BST_Q_TYPE_UC <<
                            _TH3_BST_QUEUE_TYPE_SHIFT);
                    } else {
                        /*Multicast queue of port*/
                        index = sw_index |
                            (_BCM_TH3_BST_Q_TYPE_MC <<
                            _TH3_BST_QUEUE_TYPE_SHIFT);
                    }
                }
                    BCM_IF_ERROR_RETURN(
                        _bst_th3_cmn_profile_get_hw(unit, pipe, buffer,
                            index, bid, profile));
            } else {
                BCM_IF_ERROR_RETURN(
                    _bst_th3_cmn_profile_get_hw(unit, pipe, buffer,
                        sw_index, bid, profile));
            }
            /* return after 1st valid get */
            return BCM_E_NONE;
        }
    }


    return BCM_E_NONE;
}

STATIC int
_bst_th3_profile_thd_restore_from_hw(int unit, int pipe, int sw_index,
                            bcm_bst_stat_id_t bid) {
    uint32 cell = 0;
    uint32 profile_index, reg_id;
    int inst, inst_offset;
    uint32 rval = 0;
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
        bid = bcmBstStatIdQueueTotal;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if(resInfo == NULL || resInfo->valid == 0) {
        return BCM_E_INIT;
    }
    
    inst_offset = (resInfo->index_max + 1) / (resInfo->num_instance);

    reg_id = _bcm_bst_th3_profile_reg_id_get(unit, bid, sw_index);

    sw_index &= _TH3_BST_QUEUE_TYPE_SHIFT_MASK;

    if(!(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID)) {
        /* non-profile based threshold */
        BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_profile_op(unit, BST_OP_GET, bid, pipe, sw_index,
                                                          &cell));
        return BCM_E_NONE;
    } else {
        /* get old profile ID */
        BCM_IF_ERROR_RETURN(
                _bcm_th3_bst_profile_op(unit, BST_OP_GET, bid, pipe, sw_index,
                                                 &profile_index));
        if (resInfo->profile_r[reg_id]) {
            SOC_IF_ERROR_RETURN(
                    soc_reg32_get(unit,  resInfo->profile_reg[reg_id],
                                  REG_PORT_ANY, profile_index, &rval));
            cell = soc_reg_field_get(unit, resInfo->profile_reg[reg_id], rval,
                                                resInfo->profile_field);
        }
    }
            
    /*update sw thd */
    if(sw_index >= 0) {
        if(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            /* for per-piped thresh, only update the specfic pipe */
            resInfo->p_threshold[sw_index + pipe * inst_offset] = cell;
        } else {
            /* ITM based threshold */
            if (bid == bcmBstStatIdDevice) {
                for(inst = 0; inst < resInfo->num_instance; inst++) {
                    /*update both ITMs for threhsolds of unique access type */
                    resInfo->p_threshold[sw_index + inst * inst_offset] = cell;
                }
            } else {
                /*For trhehsolds in ITM with duplicate access type, upadate only
                 * ITM 0*/
                resInfo->p_threshold[sw_index] = cell;
            }
        }
    }

    return BCM_E_NONE;
}


/* Function:
 *  _bst_th3_profile_init
 * Purpose:
 *  Threshold Profile init.
 *     unit       - (IN) unit number.
*/

STATIC int
_bst_th3_profile_init(int unit)
{

    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    bcm_bst_stat_id_t bid;
    bcm_cosq_bst_profile_t profile;
    int i, ei, index_max, wb, inst;
    uint32 profile_index;
    soc_reg_t p_reg;
    int num_reg;

    uint8 ** bid_inst_buffer = NULL;
    int rv = BCM_E_NONE;
    uint16 dev_id;
    uint8  rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    wb = SOC_WARM_BOOT(unit) ? 1 : 0;
    bst_info = _BCM_UNIT_BST_INFO(unit);
    profile.byte = bst_info->max_bytes;

    _BCM_BST_RESOURCE_ITER(bst_info, bid) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);

        if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID)) {
            continue;
        }
        if ((bid == bcmBstStatIdUcast) || (bid == bcmBstStatIdMcast)) {
            continue;
        }

        if (wb) {
            /* create an array of uint8 * to bring in each memory instance of resInfo
             * below */
            bid_inst_buffer = (uint8 **)sal_alloc(resInfo->num_instance * sizeof(uint8 *),
                                            "bid_inst_buffer");
            if (bid_inst_buffer == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            sal_memset(bid_inst_buffer, 0, resInfo->num_instance * sizeof(uint8 *));
        }
        if(bid == bcmBstStatIdQueueTotal) {
            num_reg = resInfo->num_profile_reg;
        } else {
            num_reg = 1;
        }
        /* create profiles */
        resInfo->profile_m = NULL;
        for (i = 0; i < _BCM_MAX_PROFILES; i ++) {
            resInfo->profile_r[i] = NULL;
        }
        for(i = 0; i < num_reg; i++) {
            p_reg = resInfo->profile_reg[i];
            if (resInfo->profile_mem != INVALIDm) {
                /* currently no profile_mem for TH3 BST*/
               rv = BCM_E_INTERNAL;
               goto cleanup;

            } else if (p_reg != INVALIDr) {
                    soc_profile_reg_t_init(&resInfo->iprofile_r[i]);
                    rv = soc_profile_reg_create
                                     (unit, &p_reg, 1, &resInfo->iprofile_r[i]);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }

                    resInfo->profile_r[i] = &resInfo->iprofile_r[i];
            }

            /* init profiles reference */
            index_max = (resInfo->index_max + 1) / resInfo->num_instance;
            for(inst = 0; inst < resInfo->num_instance; inst++) {
                if (dev_id == BCM56983_DEVICE_ID) {
                    /* For 56983 SKU, check if the bid is per itm or per pipe
                       so if it is per pipe, skip the invalid pipes 2, 3, 4,
                       5. For ITM, we skip ITM 1
                    */
                    if (resInfo->num_instance == 8) {
                        if (inst == 2 || inst == 3 || inst == 4 || inst == 5) {
                            continue;
                        }
                    } else {
                        if (inst == 1) {
                            continue;
                        }
                    }
                }
                if (wb) {
                    /* if the memory range is empty, read memory range to fill
                     * it in */
                    if (bid_inst_buffer[inst] == NULL) {
                        rv = _bcm_th3_bst_profile_mem_range(unit, BST_OP_GET, bid, inst,
                                                &bid_inst_buffer[inst]);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                }
                for (ei = 0; ei < index_max; ei ++) {
                    /* get profile index */
                    if (wb) {
                        /* retrieve profile_index from HW during warmboot*/
                        rv = _bcm_th3_bst_profile_op_mem_buffer(unit, BST_OP_GET | BST_OP_THD,
                            bid, inst, ei, bid_inst_buffer[inst], &profile_index);

                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                        rv = _bst_th3_profile_thd_restore_from_hw(unit, inst,
                                                                  ei, bid);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                    } else if (ei == 0) {
                        if(bid == bcmBstStatIdQueueTotal) {
                            rv = _bst_th3_cmn_profile_set_hw
                                    (unit, inst, _TH3_BST_PROFILE_INIT_FLAG,
                                  ei | (i << _TH3_BST_QUEUE_TYPE_SHIFT),
                                            bid, &profile, &profile_index);
                            if (BCM_FAILURE(rv)) {
                                goto cleanup;
                            }
                        } else {

                            rv = _bst_th3_cmn_profile_set_hw
                                    (unit, inst, _TH3_BST_PROFILE_INIT_FLAG,
                                            ei, bid, &profile, &profile_index);
                            if (BCM_FAILURE(rv)) {
                                goto cleanup;
                            }
                        }
                        /* Continue the above for loop, no cleanup necessary */
                        continue;
                    }
                    /* add reference */
                    if (resInfo->profile_r[i]) {
                        rv = soc_profile_reg_reference(unit,
                                        resInfo->profile_r[i], profile_index, 1);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                    /* update sw thd */
                    if(i == 0) {
                        if ((resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) ||
                                (bid == bcmBstStatIdDevice)) {
                                    resInfo->p_threshold[ei + inst * index_max] =
                                        bst_info->to_cell(unit, profile.byte);
                        } else {
                            resInfo->p_threshold[ei] =
                                bst_info->to_cell(unit, profile.byte);
                        }
                    }
                    if(profile_index != 0) {

                        /* attach reference, for performance issues, execute
                         *  only when it differs from HW default*/
                        if (wb) {
                            rv = _bcm_th3_bst_profile_op_mem_buffer(unit, BST_OP_SET,
                                bid, inst, ei, bid_inst_buffer[inst], &profile_index);
                        } else {
                            rv = _bcm_th3_bst_profile_op(unit, BST_OP_SET, bid, inst, ei,
                                               &profile_index);
                        }
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                }
                if (wb) {
                    /* Write out the memeory buffer to hardware since we're done with
                     * this instance of the profile */
                    if (bid_inst_buffer[inst]) {
                        rv = _bcm_th3_bst_profile_mem_range(unit, BST_OP_SET, bid, inst,
                                                &bid_inst_buffer[inst]);
                        if (BCM_FAILURE(rv)) {
                            goto cleanup;
                        }
                    }
                }
            }
        }
cleanup:
        if (bid_inst_buffer) {
            for (i = 0; i < resInfo->num_instance; ++i) {
                if (bid_inst_buffer[i]) {
                    soc_cm_sfree(unit, bid_inst_buffer[i]);
                }
            }

            sal_free (bid_inst_buffer);
            bid_inst_buffer = NULL;
        }
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return rv;
}

int
bcm_bst_th3_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int bid;
    uint32 mode;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_MEMORY;
    }

    bst_info->to_cell = _bcm_bst_th3_byte_to_cell;
    bst_info->to_byte = _bcm_bst_th3_cell_to_byte;
    bst_info->control_set = _bcm_th3_bst_control_set;
    bst_info->control_get = _bcm_th3_bst_control_get;
    bst_info->intr_to_resources = _bcm_bst_th3_intr_to_resources;
    bst_info->hw_stat_snapshot = _bcm_bst_th3_sync_hw_snapshot;
    bst_info->hw_stat_clear = NULL; /* new function has one more parameter */
    bst_info->intr_enable_set = _bcm_bst_th3_intr_enable_set;
    bst_info->pre_sync = NULL;
    bst_info->post_sync = NULL;
    bst_info->hw_intr_cb = _bcm_th3_set_hw_intr_cb;
    bst_info->port_to_mmu_inst_map = _bst_th3_port_to_mmu_inst_map_get;
    bst_info->thd_get = NULL; /* new function has one more parameter */
    bst_info->thd_set = NULL; /* new function has one more parameter */
    bst_info->profile_init = _bst_th3_profile_init;
    bst_info->hw_sbusdma_desc_init = _bcm_bst_th3_sbusdma_desc_init;
    bst_info->hw_sbusdma_desc_deinit = _bcm_bst_th3_sbusdma_desc_deinit;
    bst_info->hw_sbusdma_desc_sync = _bcm_bst_th3_sbusdma_desc_sync;
    bst_info->max_bytes = bst_info->to_byte(unit, _TH3_BST_THRESHOLD_CELL_MAX);

    mode = soc_property_get(unit, spn_BUFFER_STATS_COLLECT_TYPE, 1);
    if (mode == 1) {
        /* bst */
        for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
            if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast ||
                ((bid >= bcmBstStatIdRQEQueue) &&
                 ((bid <= bcmBstStatIdUCQueueGroup)))) {
                continue;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_bst_resource_init(unit, bid));
        }

    } else if (mode == 2) {
        BCM_IF_ERROR_RETURN
                (_bcm_th3_bst_resource_init(unit, bcmBstStatIdEgrPool));
        BCM_IF_ERROR_RETURN
                (_bcm_th3_bst_resource_init(unit, bcmBstStatIdIngPool));
    }
    return BCM_E_NONE;
}


STATIC int
_bst_th3_sbusdma_desc_setup(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_tbl_ctrl_t *tbl_ctrl;
    int blk, bid, mi;
    uint8 acc_type;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    uint32 size;
    uint8 *buff = NULL;
    _bcm_bst_tbl_desc_t *desc = NULL;
    int stats_per_inst = 0;
    int idx_count = 0;
    int xpe = 0;
    int stat_idx = 0;
    int inst = 0;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    buff = bst_info->bst_tbl_buf;
    bst_tbl_ctrl[unit] = sal_alloc(bcmBstStatIdMaxCount *
                                      sizeof(_bcm_bst_tbl_ctrl_t *),
                                      "bst_tbl_ctrl_p");
    if (bst_tbl_ctrl[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    bst_info->bst_tbl_desc = sal_alloc(bst_info->bst_tbl_num * sizeof(_bcm_bst_tbl_desc_t),
        "bst_tbl_desc_p");
    if (bst_info->bst_tbl_desc == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(bst_info->bst_tbl_desc, 0,  bst_info->bst_tbl_num * sizeof(_bcm_bst_tbl_desc_t));
    desc = bst_info->bst_tbl_desc;

    mi = 0;
    for (bid = _BCM_BST_RESOURCE_MIN; bid < _BCM_BST_RESOURCE_MAX; bid++) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
            continue;
        }
        if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
            continue;
        }

        tbl_ctrl = sal_alloc(sizeof(_bcm_bst_tbl_ctrl_t), "bst_tbl_ctrl");
        if (tbl_ctrl == NULL) {
            return BCM_E_MEMORY;
        }
        size = 0;
        tbl_ctrl->tindex = bid;
        /*BST tables are either per-pipe or per-ITM in TH3*/
        tbl_ctrl->entries = resInfo->num_instance;
        tbl_ctrl->buff = buff;
        tbl_ctrl->size = 0;
        bst_tbl_ctrl[unit][bid] = tbl_ctrl;

        base_mem = resInfo->stat_mem[0];
        reg = resInfo->stat_reg[0];
        if (reg != INVALIDr) {
            stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;
            for (xpe = 0; xpe < resInfo->num_instance; xpe++) {
                if (stats_per_inst) {
                    inst = xpe ;
                    stat_idx = 0;
                } else {
                    inst = REG_PORT_ANY;
                    stat_idx = xpe;
                    stats_per_inst = 1;
                }
                desc[mi].reg = reg;
                desc[mi].addr = soc_reg_addr_get(unit, reg, inst, stat_idx,
                    SOC_REG_ADDR_OPTION_NONE, &blk,
                    &acc_type);
                desc[mi].flags = 0;
                desc[mi].acc_type = acc_type;
                desc[mi].blk = blk;
                desc[mi].width = BYTES2WORDS(soc_reg_bytes(unit, reg));
                desc[mi].entries= stats_per_inst;
                desc[mi].shift = 8;
                size += desc[mi].entries * desc[mi].width * 4;
                tbl_ctrl->blk = desc[mi].blk;
                mi++;
            }
        } else if (base_mem != INVALIDm) {
            idx_count = 0;
            for (inst = 0; inst < resInfo->num_instance; inst++) {
                if (resInfo->num_instance == NUM_ITM(unit)) {
                    mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, inst);
                } else {
                    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, base_mem, inst);
                }
                if (mem == INVALIDm) {
                    continue;
                }
                idx_count = soc_mem_index_max(unit, mem) + 1;
                desc[mi].mem = mem;
                desc[mi].flags = SOC_SBUSDMA_CFG_COUNTER_IS_MEM | SOC_SBUSDMA_CFG_USE_FLAGS;
                desc[mi].blk = SOC_MEM_BLOCK_ANY(unit, mem);
                desc[mi].addr = soc_mem_addr_get(unit, mem,
                                               0, desc[mi].blk, 0, &acc_type);
                desc[mi].acc_type = acc_type;
                desc[mi].blk = SOC_BLOCK2SCH(unit, desc[mi].blk);
                desc[mi].width = soc_mem_entry_words(unit, mem);
                desc[mi].entries = idx_count;
                desc[mi].shift = 0;
                size += desc[mi].entries * desc[mi].width * 4;
                tbl_ctrl->blk = desc[mi].blk;
                mi++;
            }
        }
        buff += size;
        tbl_ctrl->size += size;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th3_sbusdma_desc_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int bid, mi;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    uint32 entries, width, size = 0;
    int inst = 0;
    int rv = BCM_E_INIT;
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    mi = 0;
    for (bid = _BCM_BST_RESOURCE_MIN; bid < _BCM_BST_RESOURCE_MAX; bid++) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
            continue;
        }

        if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
            continue;
        }
        base_mem = resInfo->stat_mem[0];
        reg = resInfo->stat_reg[0];
        if (reg != INVALIDr) {
            for (inst = 0; inst < resInfo->num_instance; inst++) {
                entries = resInfo->num_stat_pp / resInfo->num_instance;
                width = BYTES2WORDS(soc_reg_bytes(unit, reg));
                size += entries * width * 4;
                mi++;
            }
        } else if (base_mem != INVALIDm) {
            int idx_count = 0;
            if (resInfo->num_instance == NUM_ITM(unit)) {
                mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, 0);
            } else {
                mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, base_mem, 0);
            }
            if (mem == INVALIDm) {
                return BCM_E_PARAM;
            }
            for (inst = 0; inst < resInfo->num_instance ; inst++) {
                if (resInfo->num_instance == NUM_ITM(unit)) {
                    mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, 0);
                } else {
                    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, base_mem, 0);
                }
                if (mem == INVALIDm) {
                    continue;
                }
                idx_count = soc_mem_index_max(unit, mem) + 1;
                entries = idx_count;
                width = soc_mem_entry_words(unit, mem);
                size += entries * width * 4;
                mi++;
            }
        }
    }
    bst_info->bst_tbl_size = size;
    bst_info->bst_tbl_num = mi;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Total bst tbls: %d, Total size: %d\n"),
                 mi, size));
    bst_tbl_handles[unit] = 0;
    if (bst_info->bst_tbl_buf == NULL) {
        bst_info->bst_tbl_buf = soc_cm_salloc(unit, size, "bst_tbl_buf");
        if (bst_info->bst_tbl_buf == NULL) {
            goto error;
        }
    }
    sal_memset(bst_info->bst_tbl_buf, 0, size);
    rv =_bst_th3_sbusdma_desc_setup(unit);
    if (rv != SOC_E_NONE) {
        goto error;
    }

    rv = soc_bst_sbusdma_desc_setup(unit);
    if (rv != SOC_E_NONE) {
        goto error;
    }
    return BCM_E_NONE;
error:
    if (bst_info->bst_tbl_buf) {
        soc_cm_sfree(unit, bst_info->bst_tbl_buf);
        bst_info->bst_tbl_buf = NULL;
    }
    if (bst_info->bst_tbl_desc) {
        sal_free(bst_info->bst_tbl_desc);
        bst_info->bst_tbl_desc = NULL;
    }
    rv = soc_bst_sbusdma_desc_free(unit);
    return rv;
}

STATIC int
_bst_th3_sw_sbusdma_desc_sync(int unit, int bid)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_tbl_ctrl_t *tbl_ctrl;
    int idx, idx_offset = 0, inst_offset = 0;
    void *pentry;
    uint32 rval, temp_val = 0;
    uint8 *buff = NULL;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    uint32 width = 0;
    bcm_gport_t gport = -1;
    bcm_cos_t cosq = -1;
    int port = -1;
    int j = 0, inst = 0;
    int max_idx = 0;
    int stats_per_inst = 0;
    int thd_idx = 0;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        sal_mutex_give(bst_info->bst_reslock);
        return BCM_E_INIT;
    }
    if (!bst_tbl_ctrl[unit][bid]) {
        sal_mutex_give(bst_info->bst_reslock);
        return BCM_E_INIT;
    }
    tbl_ctrl = bst_tbl_ctrl[unit][bid];
    buff = tbl_ctrl->buff;
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if (resInfo == NULL) {
        sal_mutex_give(bst_info->bst_reslock);
        return BCM_E_PARAM;
    }

    base_mem = resInfo->stat_mem[0];
    reg = resInfo->stat_reg[0];
    if (reg != INVALIDr) {
        inst_offset = 0;
        for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
            rval = 0;
            width = BYTES2WORDS(soc_reg_bytes(unit, reg));
            idx_offset = idx + inst_offset;
            sal_memcpy(&rval, buff, width * 4);
            temp_val = soc_reg_field_get(unit, reg, rval, resInfo->stat_field);
            if (bst_info->track_mode) {
                /* BST tracking is in Peak mode.
                 * Hence store the watermark value only.
                 */
                if (resInfo->p_stat[idx_offset] < temp_val) {
                    resInfo->p_stat[idx_offset] = temp_val;
                }
            } else {
                resInfo->p_stat[idx_offset] = temp_val;
            }
            buff = buff + width * 4;

            /* Check if there exists threshold for a resource
             * continue if we don't have one */
            if (resInfo->p_threshold == NULL) {
                continue;
            }

            /* coverity[copy_paste_error : FALSE] */
            if (resInfo->p_threshold[idx_offset] &&
                (resInfo->p_stat[idx_offset] >=
                 (resInfo->p_threshold[idx_offset] * resInfo->threshold_gran)
                )) {
                /* Generate SOC EVENT */
                if (bst_info->handlers.reverse_resolve_index) {
                    /* reverse map the inedx to port/cos pair */
                    bst_info->handlers.reverse_resolve_index(unit, bid,
                                                             port, idx_offset,
                                                             &gport, &cosq);
                    soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                       bid, gport, cosq);
                }
            }

        }
    } else if (base_mem != INVALIDm) {
        if (resInfo->num_instance == NUM_ITM(unit)) {
            mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, 0);
        } else {
            mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, base_mem, 0);
        }

        if (mem == INVALIDm) {
            sal_mutex_give(bst_info->bst_reslock);
            return BCM_E_PARAM;
        }
        stats_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
        max_idx = soc_mem_index_max(unit, mem);
        for (inst = 0; inst < resInfo->num_instance; inst++) {
            inst_offset = stats_per_inst * inst;
            gport = BCM_GPORT_INVALID;
            cosq = -1;
            if (resInfo->num_instance == NUM_ITM(unit)) {
                mem = SOC_MEM_UNIQUE_ACC_ITM(unit, base_mem, inst);
            } else {
                mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, base_mem, inst);
            }
            if (mem == INVALIDm) {
                continue;
            }

            for (idx = 0; idx < max_idx; idx++) {
                if(resInfo->stat_field != INVALIDf) {
                    pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                      buff, idx);
                    temp_val = soc_mem_field32_get(unit, mem, pentry,
                                                        resInfo->stat_field);
                    idx_offset = idx + inst_offset;
                    BCM_IF_ERROR_RETURN(
                            _bcm_bst_th3_sync_resource_stat(unit, resInfo,
                                                 idx_offset, temp_val));

                } else {
                    pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                      buff, idx);
                    for(j = 0; j < resInfo->num_field; j++) {
                        temp_val = soc_mem_field32_get(unit, mem,
                                            pentry, resInfo->stat_fields[j]);
                        idx_offset = (idx * (resInfo->num_field))
                                                    + inst_offset + j;
                        BCM_IF_ERROR_RETURN(
                                _bcm_bst_th3_sync_resource_stat(unit, resInfo,
                                                     idx_offset, temp_val));

                        /* In TH3, stat count and thd count need not be same as in
                         * previous chips(check init). This is due to different
                         * AccType for Stat and Threshold.
                         * Stats - usually NON-DUP Acc Type
                         * Threshold - usually DUP Acc Type
                         * Hence threshold index might only exist for 1 instance
                         * and applicable to all.
                         */
                        if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
                            thd_idx = (idx * resInfo->num_field) + j;
                        } else {
                            thd_idx = idx_offset;
                        }
                        if (resInfo->p_threshold[thd_idx] &&
                            (resInfo->p_stat[idx_offset] >=
                             (resInfo->p_threshold[thd_idx])
                             * resInfo->threshold_gran)) {
                            /* Generate SOC EVENT */
                            if (bst_info->handlers.reverse_resolve_index) {
                                /* reverse map the inedx to port/cos pair */
                                bst_info->handlers.reverse_resolve_index(unit,
                                                                         bid,
                                                                         port,
                                                                         thd_idx,
                                                                         &gport,
                                                                         &cosq);
                                soc_event_generate(unit,
                                                   SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                                   bid, gport, cosq);

                            }
                        }
                    }
                    
                }
                
            }
            buff += tbl_ctrl->size / tbl_ctrl->entries;
         }
    }
    return BCM_E_NONE;
}



STATIC int
_bcm_bst_th3_sbusdma_desc_sync(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    bcm_bst_stat_id_t bid;
    int enable = 0;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (!_BCM_BST_SYNC_THREAD_VALID(bst_info)) {
        return BCM_E_NONE;
    }

    sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);

    if (bst_info->control_get) {
        bst_info->control_get(unit, bcmSwitchBstEnable, &enable);
    }

    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, 0,
                              _bcmBstThreadOperInternal);
    }

    BCM_IF_ERROR_RETURN(soc_bst_hw_sbusdma_desc_sync(unit));

    for (bid = _BCM_BST_RESOURCE_MIN; bid < _BCM_BST_RESOURCE_MAX; bid++) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
            continue;
        }
        if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
            continue;
        }

        BCM_IF_ERROR_RETURN(_bst_th3_sw_sbusdma_desc_sync(unit, bid));
    }

    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, enable,
                              _bcmBstThreadOperInternal);
    }

    sal_mutex_give(bst_info->bst_reslock);
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th3_sbusdma_desc_deinit(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv = BCM_E_INIT;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->bst_tbl_buf) {
        soc_cm_sfree(unit, bst_info->bst_tbl_buf);
        bst_info->bst_tbl_buf = NULL;
    }
    if (bst_info->bst_tbl_desc) {
        sal_free(bst_info->bst_tbl_desc);
        bst_info->bst_tbl_desc = NULL;
    }

    rv = soc_bst_sbusdma_desc_free(unit);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    return BCM_E_NONE;
}

/* Function:
 *  _bcm_bst_th3_retrive_port_cosq_from_voq
 * Purpose:
 *  Retrieve gport/cosq from chip_queue_number.
 *  Used for decoding BST trigger information for notifying users.
 *     unit       - (IN) unit number.
 *     voq        - (IN) Chip global VOQ number.
 *     gport      - (OUT) gport corresponding to input VOQ.
 *     cosq       - (OUT) cosq corresponding to input VOQ.
 */
STATIC int
_bcm_bst_th3_retrive_port_cosq_from_voq (int unit, int voq,  bcm_gport_t *gport,
        bcm_cos_t *cosq)
{
    soc_info_t *si = &SOC_INFO(unit);
    int max_voq, port;
    int port_q_base, port_num_q;

    if (gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }
    max_voq = si->port_uc_cosq_base[SOC_TH3_MAX_NUM_PORTS - 1] +
        SOC_TH3_NUM_GP_QUEUES;
    if (voq >= max_voq) {
        return BCM_E_INTERNAL;
    }
    /* Iterate through all ports to retrieve cosq_base and determine port,
         * cosq*/
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            port_q_base = si->port_cosq_base[port];
        } else {
            port_q_base = si->port_uc_cosq_base[port];
        }
        port_num_q = si->port_num_uc_cosq[port] + si->port_num_cosq[port];
        if ((voq >= port_q_base) && (voq < port_q_base + port_num_q)) {
            *gport = port;
            *cosq = voq - port_q_base;
            break;
        }
    }
    return BCM_E_NONE;
}

/* Function:
 *  _bcm_th3_cosq_bst_map_resource_to_gport_cos
 * Purpose:
 *  Retrieve gport/cosq from BST SW index for a resource.
 *  Used for decoding BST trigger information for notifying users.
 *     unit       - (IN) unit number.
 *     bid        - (IN) BST stat id.
 *     port       - (IN) Unused (retained to maintain consistency).
 *     index      - (IN) SW index for which stat > 0.
 *     gport      - (OUT) gport corresponding to input VOQ.
 *     cosq       - (OUT) cosq corresponding to input VOQ.
 */

int
_bcm_th3_cosq_bst_map_resource_to_gport_cos(int unit,
    bcm_bst_stat_id_t bid, int port, int index, bcm_gport_t *gport,
    bcm_cos_t *cosq)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_bst_resource_info_t *pres = NULL;
    int phy_port = 0;
    int mmu_port = 0;
    int inst = 0;
    int mmu_port_base = 0;
    int stats_per_inst = 1;
    int pipe, mmu_chip_port, mmu_global_port;

    if (bid >= bcmBstStatIdMaxCount) {
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit,
            "_bcm_th_cosq_bst_map_resource_to_gport_cos: unit=%d bid=0x%x\n"),
            unit, bid));
        return BCM_E_NONE;
    }

    pres = _BCM_BST_RESOURCE(unit, bid);
    stats_per_inst = (pres->num_stat_pp) / (pres->num_instance);
    inst = index / stats_per_inst;
    mmu_port_base = inst * SOC_TH3_MMU_PORT_STRIDE;
    index %= stats_per_inst;

    switch (bid) {
    case bcmBstStatIdDevice:
        *gport = -1;
        *cosq = BCM_COS_INVALID;
        break;

    case bcmBstStatIdIngPool:
    case bcmBstStatIdEgrPool:
    case bcmBstStatIdEgrMCastPool:
        *gport = -1;
        *cosq = index % _TH_MMU_NUM_POOL;
        break;

    case bcmBstStatIdPortPool:
        /* Per pipe memory indexed by Local MMU port number*/
        mmu_port= index/_TH_MMU_NUM_POOL;
        phy_port = si->port_m2p_mapping[mmu_port + mmu_port_base];
        *gport = si->port_p2l_mapping[phy_port];
        *cosq = index % _TH_MMU_NUM_POOL;
        break;

    case bcmBstStatIdPriGroupHeadroom:
    case bcmBstStatIdPriGroupShared:
        /* Per pipe memory indexed by Local MMU port number*/
        mmu_port = index/_TH_MMU_NUM_PG;
        phy_port = si->port_m2p_mapping[mmu_port + mmu_port_base];
        *gport = si->port_p2l_mapping[phy_port];
        *cosq = index % _TH_MMU_NUM_PG;
        break;

    case bcmBstStatIdUcast:
    case bcmBstStatIdMcast:
    case bcmBstStatIdQueueTotal:
        /* Indexed based on chip_queue_number */
        /* Iterate through all ports to retrieve cosq_base and determine port,
         * cosq*/
        _bcm_bst_th3_retrive_port_cosq_from_voq(unit, index, gport, cosq);
        break;
    case bcmBstStatIdEgrPortPoolSharedUcast:
    case bcmBstStatIdEgrPortPoolSharedMcast:
        /* Per pipe memory indexed by MMU chip port number*/
        mmu_chip_port = index/_TH_MMU_NUM_POOL;
        mmu_port = mmu_chip_port % 20; /*pipe offset for chip port number*/
        pipe = index / (20 * _TH_MMU_NUM_POOL);
        mmu_global_port = mmu_port + (pipe * SOC_TH3_MMU_PORT_STRIDE);
        phy_port = si->port_m2p_mapping[mmu_global_port];
        *gport = si->port_p2l_mapping[phy_port];
        *cosq = index % _TH_MMU_NUM_POOL;
        break;
    default:
        break;
    }

    return BCM_E_NONE;
}

int _bcm_th3_bst_info_restore(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    soc_field_t snap_en[] = {ACTION_EN_THDO0f,
                             ACTION_EN_THDI0f,
                             ACTION_EN_CFAP0f,
                             ACTION_EN_THDR0f,
                            };
    uint32 rval, enable;
    uint32 i;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    BCM_IF_ERROR_RETURN(READ_MMU_CFAP_BSTCONFIGr(unit, &rval));
    bst_info->track_mode = soc_reg_field_get(unit, MMU_CFAP_BSTCONFIGr, rval,
                               TRACKING_MODEf);

    bst_info->snapshot_mode = 0;
    BCM_IF_ERROR_RETURN(READ_MMU_GLBCFG_BST_SNAPSHOT_ACTION_ENr(unit, &rval));
    for (i = _BCM_BST_SNAPSHOT_THDO; i < _BCM_TH3_BST_MODULE_COUNT; i++) {
        enable = soc_reg_field_get(unit, MMU_GLBCFG_BST_SNAPSHOT_ACTION_ENr, rval,
                                   snap_en[i]);
        if (enable) {
            bst_info->snapshot_mode |= 0x1 << i;
        }
    }

    BCM_IF_ERROR_RETURN(READ_MMU_GLBCFG_BST_TRACKING_ENABLEr(unit, &rval));
    if (rval != 0) {
        bst_info->bst_tracking_enable = 1;
    } else {
        bst_info->bst_tracking_enable = 0;
    }

    return BCM_E_NONE;
}

#endif

