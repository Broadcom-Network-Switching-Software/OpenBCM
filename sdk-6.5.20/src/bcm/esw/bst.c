/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BST - Buffer Statistics Tracking
 * Purpose: API to set and manage various BST resources
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

#include <bcm/cosq.h>
#include <bcm/switch.h>
#include <bcm_int/bst.h>

#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/triumph3.h>
#endif

#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif

_bcm_bst_cmn_unit_info_t *_bcm_bst_unit_info[BCM_MAX_NUM_UNITS];
sbusdma_desc_handle_t bst_tbl_handles[BCM_MAX_NUM_UNITS] = {0};
_bcm_bst_tbl_ctrl_t **bst_tbl_ctrl[BCM_MAX_NUM_UNITS] = {NULL};

#define _BCM_AP_BST_MIN_THRESHOLD_CELLS 18

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int _bcm_bst_hw_stat_sync(int unit, _bcm_bst_resource_info_t *resInfo, 
                                 bcm_bst_stat_id_t bid, int port, int index);
#endif

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
STATIC int _bcm_bst_td2_sbusdma_desc_sync(int unit);
STATIC int _bcm_bst_td2_sbusdma_desc_init(int unit);
STATIC int _bcm_bst_td2_sbusdma_desc_deinit(int unit);
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int _bcm_bst_sw_stat_clear(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                  bcm_bst_stat_id_t bid);
#endif
STATIC int _bcm_bst_cmn_profile_set_hw(int unit, int pipe, int xpe, int hw_index, 
                                       bcm_bst_stat_id_t bid,
                                       bcm_cosq_bst_profile_t *profile, 
                                       uint32 *p_profile);
STATIC int _bcm_bst_cmn_io_op(int unit, int op, int pipe, int hw_index,
                              bcm_bst_stat_id_t bid, soc_mem_t mem,
                              soc_reg_t reg, soc_field_t field, uint32 *resval);

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
STATIC int
_bcm_bst_check_intr_status(int unit, bcm_bst_stat_id_t bid);
#endif

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
STATIC int
_bcm_bst_tr3_byte_to_cell(int unit, uint32 bytes)
{
    return (bytes + (208 - 1))/208;
}

STATIC int
_bcm_bst_tr3_cell_to_byte(int unit, uint32 cells)
{
    return cells * 208;
}
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int
_bcm_tr3_bst_control_set(int unit, bcm_switch_control_t type, int arg,
                         _bcm_bst_thread_oper_t operation)
{
    uint32 rval,fval=0,  i;
    _bcm_bst_cmn_unit_info_t *bst_info;
    uint32 sval[3] = {0};
    soc_field_t fld[3] = {BST_HW_SNAPSHOT_EN_THDOf,
                        BST_HW_SNAPSHOT_EN_THDIf,
                        BST_HW_SNAPSHOT_EN_CFAPf};

    bst_info = _BCM_UNIT_BST_INFO(unit);

    switch (type) {
    case bcmSwitchBstEnable:

        fval = arg; 
        BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, 
                          BST_TRACK_EN_THDOf, fval);
        soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, 
                          BST_TRACK_EN_THDIf, fval);
        soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, 
                          BST_TRACK_EN_CFAPf, fval);
        BCM_IF_ERROR_RETURN(WRITE_BST_TRACKING_ENABLEr(unit, rval));


        BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, BST_TRACKING_MODEf, 1);
        BCM_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

        if (operation == _bcmBstThreadOperExternal) {
            if (fval) {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_stop(unit));
            }
        } else {
            _BCM_BST_SYNC_THREAD_ENABLE_SET(unit, fval ? TRUE : FALSE);
        }
        break;
    case bcmSwitchBstTrackingMode:
        BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, BST_TRACKING_MODEf, (arg ? 1 : 0));
        BCM_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
        bst_info->track_mode = arg ? 1 : 0;
        break;

    case bcmSwitchBstSnapshotEnable:
         for (i = _BCM_BST_SNAPSHOT_THDO; i < _BCM_BST_SNAPSHOT_COUNT; i++) {
            if (arg & (0x1 << i)) {
                sval[i] = 1;
            } else {
                sval[i] = 0;
            }

            BCM_IF_ERROR_RETURN(READ_BST_TRACKING_CONFIGr(unit, &rval));
            soc_reg_field_set(unit, BST_TRACKING_CONFIGr, &rval, fld[i], sval[i]);
            BCM_IF_ERROR_RETURN(WRITE_BST_TRACKING_CONFIGr(unit, rval));
        }
        bst_info->snapshot_mode = arg;

       break;
    default:
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_tr3_bst_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32 rval;
    _bcm_bst_cmn_unit_info_t *bst_info;
    bst_info = _BCM_UNIT_BST_INFO(unit);


    switch (type) {
    case bcmSwitchBstEnable:
        BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
        *arg = soc_reg_field_get(unit,BST_TRACKING_ENABLEr, rval, 
                                 BST_TRACK_EN_THDOf);
        break;
    case bcmSwitchBstTrackingMode:
        BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        *arg = soc_reg_field_get(unit, MISCCONFIGr, rval, BST_TRACKING_MODEf);        
        break;

    case bcmSwitchBstSnapshotEnable:
        *arg = bst_info->snapshot_mode; 
        break;

    default:
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_triumph3_intr_to_resources(int unit, uint32 *flags)
{
    uint32 flags_tr = 0, fval, rval;
    uint64 rval64;
    int index = -1;
    bcm_bst_stat_id_t bid = bcmBstStatIdMaxCount;
    soc_field_t fld;
   
    fld = INVALIDf;

    BCM_IF_ERROR_RETURN(READ_THDO_BST_STATr(unit, &rval64));
    if ((fval = soc_reg64_field32_get(unit, THDO_BST_STATr, 
                              rval64, BST_STAT_TRIGGERED_TYPEf))) {

        if (fval & 0x1) {
            bid = bcmBstStatIdUcast;
            fld = UC_Q_BST_STAT_IDf;
        } else if (fval & 0x8) {
            bid = bcmBstStatIdEgrPool;
            fld = SP_MC_BST_STAT_IDf;
        }
        
        index = soc_reg64_field32_get(unit, THDO_BST_STATr, rval64, fld);
    }

    BCM_IF_ERROR_RETURN(READ_THDI_BST_TRIGGER_STATUSr(unit, &rval64));

    if (soc_reg64_field32_get(unit, THDI_BST_TRIGGER_STATUSr, 
                              rval64, TRIGGER_TYPEf)) {

        index = soc_reg64_field32_get(unit, THDI_BST_TRIGGER_STATUSr, 
                                      rval64, PG_SHARED_TRIGGER_IDf);

        fval = soc_reg64_field32_get(unit, THDI_BST_TRIGGER_STATUSr, rval64, 
                                        TRIGGER_TYPEf);
        if (fval & 0x1) {
            bid = bcmBstStatIdPriGroupShared;
        }

        if (fval & 0x2) {
            bid = bcmBstStatIdPriGroupHeadroom;
        }

        if (fval & 0x4) {
            bid = bcmBstStatIdPortPool;
        }

        if (fval & 0x8) {
            bid = bcmBstStatIdIngPool;
        }
    }

    if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_EN_64r)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STAT_64r(unit, &rval64));
        (void)soc_reg64_field32_get(unit, MEM_FAIL_INT_EN_64r,
                                    rval64, BST_CFAP_INT_ENf);
        index = 0;
        bid = bcmBstStatIdDevice;
    } else if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_ENr)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &rval));
        (void)soc_reg_field_get(unit, MEM_FAIL_INT_ENr, rval,
                              BST_CFAP_INT_ENf);
        index = 0;
        bid = bcmBstStatIdDevice;
    }

    if (bid == bcmBstStatIdMaxCount) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, BST_TRACKING_MODEf)) {
        _bcm_bst_sync_hw_trigger(unit, bid, -1, index);
    } else {
        _bcm_bst_sync_hw_snapshot(unit, bid, -1, index);
    }

    *flags = flags_tr;

    return BCM_E_NONE;
}

STATIC int
soc_tr3_set_hw_intr_cb(int unit)
{
    BCM_IF_ERROR_RETURN(soc_tr3_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
    return SOC_E_NONE;
}

STATIC int 
_bcm_bst_triumph3_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *pres;
    
    bst_info = _BCM_UNIT_BST_INFO(unit);

    bst_info->max_bytes = 0x500000;
    bst_info->to_cell = _bcm_bst_tr3_byte_to_cell;
    bst_info->to_byte = _bcm_bst_tr3_cell_to_byte;
    bst_info->control_set = _bcm_tr3_bst_control_set;
    bst_info->control_get = _bcm_tr3_bst_control_get;
    bst_info->intr_to_resources = _bcm_bst_triumph3_intr_to_resources;
    bst_info->hw_trigger_set = NULL;
    bst_info->hw_trigger_reset = NULL;
    bst_info->hw_intr_cb = soc_tr3_set_hw_intr_cb;
    bst_info->intr_enable_set = _bst_intr_enable_set;
    bst_info->hw_stat_snapshot = _bcm_bst_sync_hw_snapshot;
    bst_info->hw_stat_clear = _bcm_bst_hw_stat_clear;
    bst_info->pre_sync = NULL;
    bst_info->post_sync = NULL;

    /* bcmBstStatIdDevice */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdDevice);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = pres->index_max = 0;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_DEV);
    _BCM_BST_STAT_INFO(pres, INVALIDm, CFAPBSTSTATr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, CFAPBSTTHRSr, BST_THRESHOLDf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 1, "bst cfap");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 1, "bst cfap");

    /* bcmBstStatIdEgrPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 3;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_STAT_INFO(pres, INVALIDm, OP_MCUC_SP_BST_STATr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                            OP_MCUC_SP_BST_THRESHOLDr, BST_THRESHOLDf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 4, "bst egrpool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 4, "bst egrpool");

    /* bcmBstStatIdEgrMCastPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrMCastPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 3;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_STAT_INFO(pres, INVALIDm, OP_MC_SP_BST_STATr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                            OP_MC_SP_BST_THRESHOLDr, BST_THRESHOLDf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 4, "bst egr mcpool");
     _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 4, "bst egr mcpool");

    /* bcmBstStatIdIngPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdIngPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 3;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_STAT_INFO(pres, INVALIDm, THDI_BST_SHADOW_SPr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                            THDI_BST_PROFILE_THRESH_SPr, BST_THRESHOLDf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 4, "bst ing pool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 4, "bst ing pool");

    /* bcmBstStatIdPortPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPortPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = soc_mem_index_max(unit, THDI_PORT_SP_BSTm);

    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_RES_ING);

    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));

    _BCM_BST_THRESHOLD_INFO(pres, THDI_PORT_SP_CONFIGm, 
                            INVALIDr, BST_THRESHOLD_PROFILEf, 1);

    _BCM_BST_PROFILE_INFO(pres, INVALIDm, 
                  THDI_BST_PROFILE_THRESH_PORT_SHAREDr, BST_THRESHOLDf);

    _BCM_BST_STAT_INFO(pres, THDI_PORT_SP_BSTm, INVALIDr, BST_STATf);

    _BCM_BST_RESOURCE_STAT_INIT(pres, 
               soc_mem_index_count(unit, THDI_PORT_SP_BSTm), "bst port pool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres,
               soc_mem_index_count(unit, THDI_PORT_SP_BSTm), "bst port pool");

    /* bcmBstStatIdPriGroupShared */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupShared);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = (63 * 8) - 1;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    _BCM_BST_THRESHOLD_INFO(pres, THDI_PORT_PG_CONFIGm, INVALIDr, 
                          PG_BST_PROFILE_SHAREDf, 1);
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, 
                            THDI_BST_PROFILE_THRESH_PG_SHAREDr, 
                            BST_THRESHOLD_SHAREDf);
    _BCM_BST_STAT_INFO(pres, THDI_PORT_PG_BSTm, INVALIDr, PG_BST_STAT_SHAREDf);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 63 * 8, "bst pg");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 63 * 8, "bst pg");

    /* bcmBstStatIdPriGroupHeadroom */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupHeadroom);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = (63 * 8) - 1;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID | 
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));
    _BCM_BST_THRESHOLD_INFO(pres, THDI_PORT_PG_CONFIGm, INVALIDr,
                            PG_BST_PROFILE_HDRMf, 1);
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, 
                            THDI_BST_PROFILE_THRESH_PG_HDRMr, 
                            BST_THRESHOLD_HDRMf);
    _BCM_BST_STAT_INFO(pres, THDI_PORT_PG_BSTm, INVALIDr, BST_STAT_HDRMf);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 63 * 8, "bst pg hdrm");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 63 * 8, "bst pg");

    /* bcmBstStatIdMcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdMcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 47;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_CMIC(unit));
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                          OP_CPU_QUEUE_BST_THRESHOLDr, BST_PROFILEf, 1);
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_CPU_QUEUE_BST_THRESHOLD_PROFILEr, 
                             THRESHOLDf);
    _BCM_BST_STAT_INFO(pres, INVALIDm, OP_CPU_QUEUE_BST_STATr, BST_STATf);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 48, "bst mcast");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 48, "bst mcast");

    /* bcmBstStatIdUcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdUcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = soc_mem_index_max(unit, MMU_THDO_BST_QUEUEm);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));
    _BCM_BST_THRESHOLD_INFO(pres, MMU_THDO_CONFIG_QUEUEm, INVALIDr, 
                          BST_THRESHOLD_PROFILEf, 8);
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_QUEUE_BST_THRESHOLDr, 
                             BST_THRESHOLDf);
    _BCM_BST_STAT_INFO(pres, MMU_THDO_BST_QUEUEm, INVALIDr, BST_STATf);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 
                 soc_mem_index_count(unit, MMU_THDO_BST_QUEUEm), "bst ucast");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres,
                 soc_mem_index_count(unit, MMU_THDO_BST_QUEUEm), "bst ucast");

    return BCM_E_NONE;
}

#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)

STATIC int _bcm_td2_bst_sync_thread_restore(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int enable;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (bst_info->control_get) {
        bst_info->control_get(unit, bcmSwitchBstEnable, &enable);
        if (enable) {
            return TRUE;
        }
    }

    return FALSE;
}

STATIC int _bcm_td2_bst_info_restore(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    uint32 rval;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
    bst_info->track_mode = soc_reg_field_get(unit, BST_TRACKING_ENABLEr, rval,
                             BST_TRACK_EN_THDOf);

    BCM_IF_ERROR_RETURN(READ_BST_HW_SNAPSHOT_ENr(unit, &rval));
    bst_info->snapshot_mode = soc_reg_field_get(unit, BST_HW_SNAPSHOT_ENr, rval,
                             BST_HW_SNAPSHOT_EN_THDOf);

    return BCM_E_NONE;
}

STATIC int _bcm_td2_bst_resource_threshold_restore(int unit)
{
    _bcm_bst_resource_info_t *resInfo;
    int hw_index, pipe, ipipe, index_max = 0;
    bcm_bst_stat_id_t bid;
    uint32 cells;
    uint32 profile_index;
    int pipe_offset;
    int pipe_factor = 1;

    pipe_factor = NUM_PIPE(unit);

    for (bid = _BCM_BST_RESOURCE_MIN; bid < _BCM_BST_RESOURCE_MAX; bid++) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
            continue;
        }

        if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            index_max = ((resInfo->index_max + 1) / NUM_PIPE(unit)) - 1;
        }
        else {
            index_max = resInfo->index_max;
        }

        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            pipe_offset = 0;
            if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
                if (pipe) {
                    pipe_offset = pipe * ((resInfo->index_max + 1) / pipe_factor);
                }
                ipipe = pipe;
            } else {
                if (pipe) {
                    continue;
                }
                ipipe = pipe;
            }

            for (hw_index = resInfo->index_min; hw_index <= index_max; hw_index++) {
                if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
                    SOC_IF_ERROR_RETURN(
                       _bcm_bst_cmn_io_op(unit, BST_OP_GET | BST_OP_THD,
                                          ipipe, hw_index, bid,
                                          resInfo->threshold_mem[ipipe],
                                          resInfo->threshold_reg[ipipe],
                                          resInfo->threshold_field,
                                          &profile_index));

                    SOC_IF_ERROR_RETURN(
                       _bcm_bst_cmn_io_op(unit, BST_OP_GET,
                                          ipipe, profile_index, bid,
                                          resInfo->profile_mem,
                                          resInfo->profile_reg[0],
                                          resInfo->profile_field,
                                          &cells));
                } else {
                    SOC_IF_ERROR_RETURN(
                       _bcm_bst_cmn_io_op(unit, BST_OP_GET | BST_OP_THD,
                                          ipipe, hw_index, bid,
                                          resInfo->threshold_mem[ipipe],
                                          resInfo->threshold_reg[ipipe],
                                          resInfo->threshold_field,
                                          &cells));
                }

                resInfo->p_threshold[hw_index + pipe_offset] = cells;
            }
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_bst_threshold_maximum_set(int unit, _bcm_bst_cmn_unit_info_t *bst_info,
                                                 uint32 enable)
{
    int hw_index, pipe, cells;
    uint32 bid_index, index_max;
    _bcm_bst_resource_info_t *resInfo;
    bcm_cosq_bst_profile_t profile;
    int pipe_offset;
    int pipe_factor = 1;
    uint32 threshold_max = BST_THRESHOLD_MAX_HIGH;

    /* Need to set bst threshold to max high when bst_trac_en is disabled. 
       Only add bst id if relative bst id is set.
       Registers listed below are exposed to add with related BST ID in the future
       MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE 
       MMU_THDM_DB_PORTSP_BST_THRESHOLD 
       MMU_THDM_MCQE_CPUQUEUE_BST_THRESHOLD_PROFILE 
       MMU_THDM_MCQE_POOL_MC_BST_THRESHOLD 
       MMU_THDM_MCQE_QUEUE_MC_BST_THRESHOLD_PROFILE 
       MMU_THDM_MCQE_PORTSP_BST_THRESHOLD
       */    
    bcm_bst_stat_id_t bst_threshold_id[] = {bcmBstStatIdEgrPool, 
                                            bcmBstStatIdEgrMCastPool, 
                                            bcmBstStatIdMcast};   

    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)) {
        for (bid_index = 0;
             bid_index < (sizeof(bst_threshold_id)/sizeof(bcm_bst_stat_id_t));
             bid_index++) {
            sal_memset(&profile, 0, sizeof(bcm_cosq_bst_profile_t));                
            resInfo = _BCM_BST_RESOURCE(unit, bst_threshold_id[bid_index]);
            if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
                continue;
            }

            if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
                index_max = ((resInfo->index_max + 1) / NUM_PIPE(unit)) - 1;
            }
            else {
                index_max = resInfo->index_max;
            }

            for (hw_index = resInfo->index_min; hw_index <= index_max ; hw_index++) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    pipe_offset = 0;
                    if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
                        pipe_factor = NUM_PIPE(unit);
                        if (pipe) {
                            pipe_offset = pipe * ((resInfo->index_max + 1) / pipe_factor);
                        }
                    } 

                    if(resInfo->p_threshold[hw_index + pipe_offset] != threshold_max) {
                        if(enable == 1) {
                            cells = resInfo->p_threshold[hw_index + pipe_offset];
                            if(cells != 0) {
                                /* Taking granularity into count */
                                if(resInfo->threshold_gran != 0) {
                                    cells = cells * resInfo->threshold_gran - resInfo->threshold_adj;
                                } else {
                                    cells -= resInfo->threshold_adj;
                                }

                                profile.byte = bst_info->to_byte(unit, cells);
                                
                                SOC_IF_ERROR_RETURN(_bcm_bst_cmn_profile_set_hw
                                    (unit, pipe, BCM_BST_DUP_XPE, hw_index,
                                        bst_threshold_id[bid_index], &profile, NULL));
                            }
                        } else {
                            profile.byte = BST_THRESHOLD_INVALID;
                            SOC_IF_ERROR_RETURN(_bcm_bst_cmn_profile_set_hw
                                (unit, pipe, BCM_BST_DUP_XPE, hw_index,
                                    bst_threshold_id[bid_index], &profile, NULL));

                        } 
                    }
                }
            }
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_bst_control_set(int unit, bcm_switch_control_t type, int arg,
                         _bcm_bst_thread_oper_t operation)
{
    uint32 i, rval, fval;
    uint32 sval[3] = {0};
    _bcm_bst_cmn_unit_info_t *bst_info;
    soc_field_t fld[6] = {BST_HW_SNAPSHOT_EN_THDOf, 
                        ACTION_EN_THDOf,
                        BST_HW_SNAPSHOT_EN_THDIf, 
                        ACTION_EN_THDIf,
                        BST_HW_SNAPSHOT_EN_CFAPf, 
                        ACTION_CFAPf};  

    bst_info = _BCM_UNIT_BST_INFO(unit);

    switch (type) {
    case bcmSwitchBstEnable:

        BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));

        fval = !!arg;
        soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, 
                          BST_TRACK_EN_THDOf, fval);
        soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, 
                          BST_TRACK_EN_THDIf, fval);
        soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, 
                          BST_TRACK_EN_CFAPf, fval);

        BCM_IF_ERROR_RETURN(WRITE_BST_TRACKING_ENABLEr(unit, rval));

        if (operation == _bcmBstThreadOperExternal) {          
            BCM_IF_ERROR_RETURN(_bcm_td2_bst_threshold_maximum_set(unit, bst_info, fval));  
            
            if (fval) {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_stop(unit));
            }
        } else {
            _BCM_BST_SYNC_THREAD_ENABLE_SET(unit, fval ? TRUE : FALSE);
        }
        
        break;
    case bcmSwitchBstTrackingMode:
        BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, BST_TRACKING_MODEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
        bst_info->track_mode = arg ? 1 : 0;
        break;
    case bcmSwitchBstSnapshotEnable:
        for (i = _BCM_BST_SNAPSHOT_THDO; i < _BCM_BST_SNAPSHOT_COUNT; i++) {
            if (arg & (0x1 << i)) {
                sval[i] = 1;
            } else {
                sval[i] = 0;
            }

            BCM_IF_ERROR_RETURN(READ_BST_HW_SNAPSHOT_ENr(unit, &rval));
            soc_reg_field_set(unit, BST_HW_SNAPSHOT_ENr, &rval, fld[i*2], sval[i]);
            BCM_IF_ERROR_RETURN(WRITE_BST_HW_SNAPSHOT_ENr(unit, rval));

            BCM_IF_ERROR_RETURN(READ_BST_SNAPSHOT_ACTION_ENr(unit, &rval));
            soc_reg_field_set(unit, BST_SNAPSHOT_ACTION_ENr, &rval, fld[i*2+1], sval[i]);
            BCM_IF_ERROR_RETURN(WRITE_BST_SNAPSHOT_ACTION_ENr(unit, rval));            
        }
        bst_info->snapshot_mode = arg;

        break;
    default:
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_bst_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32 rval;
    _bcm_bst_cmn_unit_info_t *bst_info;
    bst_info = _BCM_UNIT_BST_INFO(unit);

    switch (type) {
    case bcmSwitchBstEnable:
        BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
        *arg = soc_reg_field_get(unit, BST_TRACKING_ENABLEr, rval, 
                                 BST_TRACK_EN_THDOf);
        break;
    case bcmSwitchBstTrackingMode:
        *arg = bst_info->track_mode;
        break;
    case bcmSwitchBstSnapshotEnable:
        *arg = bst_info->snapshot_mode;
        break;
    default:
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_td2_intr_to_resources(int unit, uint32 *flags)
{
    uint32 flags_tr = 0;
    uint32 fval, fval_pipe[SOC_MAX_NUM_PIPES];
    uint32 rval, rval_dev, rval_pipe[SOC_MAX_NUM_PIPES], rval_status;
    uint32 rval_rqe;
    uint64 rval_uc, rval_db, rval_mcqe, rval64, rval64_zero;
    int index[_bcmResourceMaxCount][SOC_MAX_NUM_PIPES], res_ct = 0;
    bcm_bst_stat_id_t bid[_bcmResourceMaxCount] = {-1};
    int pipe = 0, pos = 0;
    int bid_mcast = 0;
    soc_field_t fld[SOC_MAX_NUM_PIPES];
    soc_reg_t reg[SOC_MAX_NUM_PIPES];
    _bcm_bst_cmn_unit_info_t *bst_info;
    bst_info = _BCM_UNIT_BST_INFO(unit);

    for (pipe = 0; pipe < NUM_PIPE(unit) ; pipe++) {
        fval_pipe[pipe] = 0;
        rval_pipe[pipe] = 0;
        fld[pipe] = INVALIDf;
        reg[pipe] = INVALIDr;
        for (pos = 0; pos < _bcmResourceMaxCount; pos++) {
            index[pos][pipe] = -1;
        }
    }

    if (bst_info->intr_enable_set) {
        bst_info->intr_enable_set(unit, 0);
    }

    if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_EN_64r)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STAT_64r(unit, &rval64));
    } else if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_ENr)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &rval_status));
    }

    _bst_intr_status_reset(unit);

    BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
    BCM_IF_ERROR_RETURN(READ_THDU_BST_STATr(unit, &rval_uc));
    BCM_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_BST_STATr(unit, &rval_db));
    BCM_IF_ERROR_RETURN(READ_MMU_THDM_MCQE_DEVICE_BST_STATr(unit, &rval_mcqe));
    BCM_IF_ERROR_RETURN(READ_MMU_THDR_DB_BST_STATr(unit, &rval_rqe));

    /* bcmBstStatIdEgrPool */
    fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, 
                                 rval_db, BST_STAT_TRIGGERED_TYPEf);
    if (fval) {
        reg[0] = MMU_THDM_DB_DEVICE_BST_STATr;
        /* Check all triggered types. */
        for (pos = 0; pos < 5; pos++) {
            if (!(fval & (1U << pos))) {
                continue;
            }
            switch (fval & (1U << pos)) {
                case 0x1:
                    bid[res_ct] = bcmBstStatIdEgrPool;
                    fld[0] = MCUC_SP_BST_STAT_IDf;
                    break;
                case 0x2:
                    bid[res_ct] = bcmBstStatIdEgrMCastPool;
                    fld[0] = MC_SP_BST_STAT_IDf;
                    break;
                case 0x4:
                    bid[res_ct] = bcmBstStatIdMcast;
                    fld[0] = MC_Q_BST_STAT_IDf;
                    bid_mcast = 1;
                    break;
                case 0x8:
                    if (!bid_mcast) {
                        bid[res_ct] = bcmBstStatIdMcast;
                        fld[0] = MC_CPU_Q_BST_STAT_IDf;
                    } else {
                        fld[0] = INVALIDf;
                    }
                    break;
                case 0x10:
                    bid[res_ct] = bcmBstStatIdEgrPortPoolSharedMcast;
                    fld[0] = MC_PORTSP_BST_STAT_IDf;
                    break;
                default:
                    fld[0] = INVALIDf;
            }
            if (fld[0] != INVALIDf) {
                index[res_ct++][0] = soc_reg64_field32_get(unit, reg[0], rval_db, fld[0]);
            }
        }
        COMPILER_64_ZERO(rval64_zero);
        BCM_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_BST_STATr(unit, rval64_zero));
    }
    
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        /* bcmBstStatIdDevice */
        BCM_IF_ERROR_RETURN(READ_CFAP_BST_THRSr(unit, &rval_dev));
        if (soc_reg_field_get(unit, CFAP_BST_THRSr, rval_dev, BST_THRESHOLDf)) {
            if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_EN_64r)) {
                if (soc_reg64_field32_get(unit, MEM_FAIL_INT_EN_64r,
                        rval64, BST_CFAP_INT_ENf)) {
                    index[res_ct][0] = 0;
                    bid[res_ct++] = bcmBstStatIdDevice;
                }
            } else if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_ENr)) {
                /* CFAP : Device resource. 
                * Check Intr Status reg if Stats > Thd.
                */
                reg[0] = MEM_FAIL_INT_STATr;
                if (soc_reg_field_get(unit, reg[0], rval_status, BST_CFAP_INTf)) {
                    index[res_ct][0] = 0;
                    bid[res_ct++] = bcmBstStatIdDevice;
                }
            }
        }
    } else 
#endif
    {
        /* bcmBstStatIdDevice */
        BCM_IF_ERROR_RETURN(READ_CFAPBSTTHRSr(unit, &rval_dev));
        if (soc_reg_field_get(unit, CFAPBSTTHRSr, rval_dev, BST_THRESHOLDf)) {
            if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_EN_64r)) {
                if (soc_reg64_field32_get(unit, MEM_FAIL_INT_EN_64r,
                        rval64, BST_CFAP_INT_ENf)) {
                    index[res_ct][0] = 0;
                    bid[res_ct++] = bcmBstStatIdDevice;
                }
            } else if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_ENr)) {
                /* CFAP : Device resource. 
                * Check Intr Status reg if Stats > Thd.
                */
                reg[0] = MEM_FAIL_INT_STATr;
                if (soc_reg_field_get(unit, reg[0], rval_status, BST_CFAP_INTf)) {
                    index[res_ct][0] = 0;
                    bid[res_ct++] = bcmBstStatIdDevice;
                }
            }
        }
    }

#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        /* Ingress based Status/Triggers */
        BCM_IF_ERROR_RETURN(READ_THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr(unit, &rval_pipe[0]));
        reg[0] = THDI_BST_TRIGGER_STATUS_PIPEXr;

        /* bcmBstStatIdIngPool */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], POOL_SHARED_TRIGGERf);
        if (fval_pipe[0]) {
            fld[0] = POOL_SHARED_TRIGGER_STATUSf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[0]);
                }
            }
            bid[res_ct++] = bcmBstStatIdIngPool;
        }

        /* bcmBstStatIdPortPool */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], SP_SHARED_TRIGGER_PIPEXf);
        if (fval_pipe[0]) {
            fld[0] = SP_SHARED_TRIGGER_STATUS_PIPEXf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[pipe]);
                }
            }
            bid[res_ct++] = bcmBstStatIdPortPool;
        }

        /* bcmBstStatIdPriGroupShared */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], PG_SHARED_TRIGGER_PIPEXf);
        if (fval_pipe[0]) {
            fld[0] = PG_SHARED_TRIGGER_STATUS_PIPEXf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[pipe]);
                }
            }
            bid[res_ct++] = bcmBstStatIdPriGroupShared;
        }

        /* bcmBstStatIdPriGroupHeadroom */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], PG_HDRM_TRIGGER_PIPEXf);
        if (fval_pipe[0]) {
            fld[0] = PG_HDRM_TRIGGER_STATUS_PIPEXf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[pipe]);
                }
            }
            bid[res_ct++] = bcmBstStatIdPriGroupHeadroom;
        }
        BCM_IF_ERROR_RETURN(WRITE_THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr(unit, 0));
    } else 
#endif
    {
        /* Ingress based Status/Triggers */
        BCM_IF_ERROR_RETURN(READ_THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr(unit, &rval_pipe[0]));
        BCM_IF_ERROR_RETURN(READ_THDI_BST_TRIGGER_STATUS_TYPE_PIPEYr(unit, &rval_pipe[1]));
        reg[0] = THDI_BST_TRIGGER_STATUS_PIPEXr;
        reg[1] = THDI_BST_TRIGGER_STATUS_PIPEYr;

        /* bcmBstStatIdIngPool */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], POOL_SHARED_TRIGGERf);
        fval_pipe[1] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEYr, 
                                        rval_pipe[1], POOL_SHARED_TRIGGERf);
        if (fval_pipe[0] || fval_pipe[1]) {
            fld[0] = POOL_SHARED_TRIGGER_STATUSf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[0]);
                }
            }
            bid[res_ct++] = bcmBstStatIdIngPool;
        }

        /* bcmBstStatIdPortPool */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], SP_SHARED_TRIGGER_PIPEXf);
        fval_pipe[1] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEYr, 
                                        rval_pipe[1], SP_SHARED_TRIGGER_PIPEYf);
        if (fval_pipe[0] || fval_pipe[1]) {
            fld[0] = SP_SHARED_TRIGGER_STATUS_PIPEXf;
            fld[1] = SP_SHARED_TRIGGER_STATUS_PIPEYf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[pipe]);
                }
            }
            bid[res_ct++] = bcmBstStatIdPortPool;
        }

        /* bcmBstStatIdPriGroupShared */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], PG_SHARED_TRIGGER_PIPEXf);
        fval_pipe[1] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEYr, 
                                        rval_pipe[1], PG_SHARED_TRIGGER_PIPEYf);
        if (fval_pipe[0] || fval_pipe[1]) {
            fld[0] = PG_SHARED_TRIGGER_STATUS_PIPEXf;
            fld[1] = PG_SHARED_TRIGGER_STATUS_PIPEYf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[pipe]);
                }
            }
            bid[res_ct++] = bcmBstStatIdPriGroupShared;
        }

        /* bcmBstStatIdPriGroupHeadroom */
        fval_pipe[0] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr, 
                                        rval_pipe[0], PG_HDRM_TRIGGER_PIPEXf);
        fval_pipe[1] = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPE_PIPEYr, 
                                        rval_pipe[1], PG_HDRM_TRIGGER_PIPEYf);
        if (fval_pipe[0] || fval_pipe[1]) {
            fld[0] = PG_HDRM_TRIGGER_STATUS_PIPEXf;
            fld[1] = PG_HDRM_TRIGGER_STATUS_PIPEYf;

            if (!fval) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    if (!fval_pipe[pipe]) {
                        continue;
                    }
                    index[res_ct][pipe] = soc_reg_field_get(unit, reg[pipe], rval_pipe[pipe], fld[pipe]);
                }
            }
            bid[res_ct++] = bcmBstStatIdPriGroupHeadroom;
        }
        BCM_IF_ERROR_RETURN(WRITE_THDI_BST_TRIGGER_STATUS_TYPE_PIPEXr(unit, 0));
        BCM_IF_ERROR_RETURN(WRITE_THDI_BST_TRIGGER_STATUS_TYPE_PIPEYr(unit, 0));
    }

    /* bcmBstStatIdUcast */
    if ((fval = soc_reg64_field32_get(unit, THDU_BST_STATr,
                              rval_uc, BST_STAT_TRIGGERED_TYPEf))) {
        if (fval & 0x1) {
            fld[0] = UC_Q_BST_STAT_IDf;
            index[res_ct][0] = soc_reg64_field32_get(unit, THDU_BST_STATr, rval_uc, fld[0]);
            if (index[res_ct][0] >= 2048) {
                index[res_ct][1] = index[res_ct][0] - 2048 + 1480;
                index[res_ct][0] = -1;
            }
            bid[res_ct++] = bcmBstStatIdUcast;
        }
        if (fval & 0x2) {
            fld[0] = UC_QGROUP_BST_STAT_IDf;
            index[res_ct][0] = soc_reg64_field32_get(unit, THDU_BST_STATr, rval_uc,
                                             fld[0]);
            if (NUM_PIPE(unit) > 1) {
                if (index[res_ct][0] >= 128) {
                    index[res_ct][1] = index[res_ct][0] - 128;
                    index[res_ct][0] = -1;
                }
            }
            bid[res_ct++] = bcmBstStatIdUCQueueGroup;
        }
        if (fval & 0x4) {
            fld[0] = UC_PORT_SP_BST_STAT_IDf;
            index[res_ct][0] = soc_reg64_field32_get(unit, THDU_BST_STATr, rval_uc,
                                             fld[0]);
            if (NUM_PIPE(unit) > 1) {
                if (index[res_ct][0] >= 256) {
                    index[res_ct][1] = index[res_ct][0] - 256;
                    index[res_ct][0] = -1;
                }
            }
            bid[res_ct++] = bcmBstStatIdEgrPortPoolSharedUcast;
        }

        COMPILER_64_ZERO(rval64_zero);
        BCM_IF_ERROR_RETURN(WRITE_THDU_BST_STATr(unit, rval64_zero));
    }

    /* THDR Status/Triggers */
    reg[0] = MMU_THDR_DB_BST_STATr;
    fval = soc_reg_field_get(unit, reg[0], rval_rqe,
                            PRIQ_TRIGGEREDf);
    if (fval & 0x1) {
        fld[0] = PRIQ_IDf;
        index[res_ct][0] = soc_reg_field_get(unit, reg[0], rval_rqe,
                                         fld[0]);
        bid[res_ct++] = bcmBstStatIdRQEQueue;
    }

    fval = soc_reg_field_get(unit, reg[0], rval_rqe,
                            SP_TRIGGEREDf);
    if (fval & 0x1) {
        fld[0] = SP_IDf;
        index[res_ct][0] = soc_reg_field_get(unit, reg[0], rval_rqe,
                                         fld[0]);
        bid[res_ct++] = bcmBstStatIdRQEPool;
    }

    COMPILER_64_ZERO(rval64_zero);
    /*BCM_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_BST_STATr(unit, rval64_zero));*/
    BCM_IF_ERROR_RETURN(soc_reg_set(unit, MMU_THDR_DB_BST_STATr, REG_PORT_ANY,
                0, rval64_zero));

    if (res_ct == 0) {
        /* No resource to fetch */
        if (bst_info->intr_enable_set) {
            bst_info->intr_enable_set(unit, 1);
        }
        return BCM_E_NONE;
    }

    for (pos = 0; pos < res_ct; pos++) {
        if (index[pos][0] != -1) {
            _bcm_bst_sync_hw_snapshot(unit, bid[pos], -1, index[pos][0]);
        } else {
            _bcm_bst_sync_hw_snapshot(unit, bid[pos], -2, index[pos][1]);
        }
    }

    *flags = flags_tr;
    if (bst_info->intr_enable_set) {
        bst_info->intr_enable_set(unit, 1);
    }
    return BCM_E_NONE;
}

STATIC int 
_bcm_bst_td2_hw_trigger_set(int unit, bcm_bst_stat_id_t bid)
{
    uint32 rval;
    _bcm_bst_resource_info_t *resInfo;
    soc_field_t fld[2] = { INVALIDf, INVALIDf };

    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if (resInfo == NULL) {
        return BCM_E_PARAM;
    }

    if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_DEV) {
        fld[0] = BST_HW_SNAPSHOT_EN_CFAPf;
        fld[1] = ACTION_CFAPf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_ING) {
        fld[0] = BST_HW_SNAPSHOT_EN_THDIf;
        fld[1] = ACTION_EN_THDIf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_EGR) {
        fld[0] = BST_HW_SNAPSHOT_EN_THDOf;
        fld[1] = ACTION_EN_THDOf;
    } else {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN(READ_BST_HW_SNAPSHOT_ENr(unit, &rval));
    soc_reg_field_set(unit, BST_HW_SNAPSHOT_ENr, &rval, fld[0], 1);
    BCM_IF_ERROR_RETURN(WRITE_BST_HW_SNAPSHOT_ENr(unit, rval));

    BCM_IF_ERROR_RETURN(READ_BST_SNAPSHOT_ACTION_ENr(unit, &rval));
    soc_reg_field_set(unit, BST_SNAPSHOT_ACTION_ENr, &rval, fld[1], 1);
    BCM_IF_ERROR_RETURN(WRITE_BST_SNAPSHOT_ACTION_ENr(unit, rval));
    return BCM_E_NONE;
}

STATIC int 
_bcm_bst_td2_hw_trigger_reset(int unit, bcm_bst_stat_id_t bid)
{
    uint32 rval;
    _bcm_bst_resource_info_t *resInfo;
    soc_field_t fld[2] = { INVALIDf, INVALIDf };

    if (bid != -1) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_DEV) {
            fld[0] = BST_HW_SNAPSHOT_EN_CFAPf;
            fld[1] = ACTION_CFAPf;
        } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_ING) {
            fld[0] = BST_HW_SNAPSHOT_EN_THDIf;
            fld[1] = ACTION_EN_THDIf;
        } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_EGR) {
            fld[0] = BST_HW_SNAPSHOT_EN_THDOf;
            fld[1] = ACTION_EN_THDOf;
        } else {
            return BCM_E_PARAM;
        }
    
        BCM_IF_ERROR_RETURN(READ_BST_HW_SNAPSHOT_ENr(unit, &rval));
        soc_reg_field_set(unit, BST_HW_SNAPSHOT_ENr, &rval, fld[0], 0);
        BCM_IF_ERROR_RETURN(WRITE_BST_HW_SNAPSHOT_ENr(unit, rval));

        BCM_IF_ERROR_RETURN(READ_BST_SNAPSHOT_ACTION_ENr(unit, &rval));
        soc_reg_field_set(unit, BST_SNAPSHOT_ACTION_ENr, &rval, fld[1], 0);
        BCM_IF_ERROR_RETURN(WRITE_BST_SNAPSHOT_ACTION_ENr(unit, rval));
    } else {
        /* Reset for all Resources */
        BCM_IF_ERROR_RETURN(READ_BST_HW_SNAPSHOT_ENr(unit, &rval));
        soc_reg_field_set(unit, BST_HW_SNAPSHOT_ENr, &rval, BST_HW_SNAPSHOT_EN_CFAPf, 0);
        soc_reg_field_set(unit, BST_HW_SNAPSHOT_ENr, &rval, BST_HW_SNAPSHOT_EN_THDIf, 0);
        soc_reg_field_set(unit, BST_HW_SNAPSHOT_ENr, &rval, BST_HW_SNAPSHOT_EN_THDOf, 0);
        BCM_IF_ERROR_RETURN(WRITE_BST_HW_SNAPSHOT_ENr(unit, rval));

        BCM_IF_ERROR_RETURN(READ_BST_SNAPSHOT_ACTION_ENr(unit, &rval));
        soc_reg_field_set(unit, BST_SNAPSHOT_ACTION_ENr, &rval, ACTION_CFAPf, 0);
        soc_reg_field_set(unit, BST_SNAPSHOT_ACTION_ENr, &rval, ACTION_EN_THDIf, 0);
        soc_reg_field_set(unit, BST_SNAPSHOT_ACTION_ENr, &rval, ACTION_EN_THDOf, 0);
        BCM_IF_ERROR_RETURN(WRITE_BST_SNAPSHOT_ACTION_ENr(unit, rval));
    }
    return BCM_E_NONE;
}

STATIC int
soc_td2_set_hw_intr_cb(int unit)
{
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        BCM_IF_ERROR_RETURN(soc_monterey_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
        return SOC_E_NONE;
    }
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        BCM_IF_ERROR_RETURN(soc_apache_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
        return SOC_E_NONE;
    }
#endif
    BCM_IF_ERROR_RETURN(soc_td2_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
    return SOC_E_NONE;
}

int
_bcm_bst_td2_intr_check(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int num_entries, mem_wsz, port;
    int idx, idx_offset, rv, pipe, pipe_offset = 0;
    void *pentry;
    char *dmabuf;
    uint32 rval, temp_val;
    soc_mem_t mem;
    soc_reg_t reg;
    bcm_bst_stat_id_t bid = bcmBstStatIdMaxCount;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    /* bcmBstStatIdHeadroomPool is used for Tomahawk*/
    for(bid = _BCM_BST_RESOURCE_MIN; bid < bcmBstStatIdMaxCount; bid++)
    {
        /* bcmBstStatIdHeadroomPool is not supported for TD2/TD2+...*/
        if (bid == bcmBstStatIdHeadroomPool) {
            continue;
        }

        /* sync all the stats */
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
            continue;
        }

        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
            bcm_gport_t gport = -1;
            bcm_cos_t cosq = -1;
            mem = resInfo->stat_mem[pipe];
            reg = resInfo->stat_reg[pipe];
            pipe_offset = 0;

            if (pipe) {
                pipe_offset = ((resInfo->index_max + 1) / NUM_PIPE(unit)) * pipe;
            }

            if (pipe == 1) {
                port = -2;
            } else {
                port = -1;
            }

            if (mem != INVALIDm) {
                num_entries = soc_mem_index_count(unit, mem);
                mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

                dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
                if (dmabuf == NULL) {
                    return BCM_E_MEMORY;
                }

                if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                        soc_mem_index_min(unit, mem),
                                        soc_mem_index_max(unit, mem),
                                        dmabuf)) {
                    soc_cm_sfree(unit, dmabuf);
                    return BCM_E_INTERNAL;
                }

                for (idx = 0; idx < (resInfo->num_stat_pp / NUM_PIPE(unit)); idx++) {
                    idx_offset = idx + pipe_offset;
                    pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                          dmabuf, idx);
                    temp_val = soc_mem_field32_get(unit, mem, pentry, resInfo->stat_field);
                    if (bst_info->track_mode) {
                        if (resInfo->p_stat[idx_offset] < temp_val) {
                            resInfo->p_stat[idx_offset] = temp_val;
                        }
                    } else {
                        resInfo->p_stat[idx_offset] = temp_val;
                    }

                    if (resInfo->p_threshold[idx_offset] &&
                        (resInfo->p_stat[idx_offset] >=
                         (resInfo->p_threshold[idx_offset]) * resInfo->threshold_gran)) {
                         LOG_WARN(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d %s %d mem %s bid %d port %d idx %d threshold %d stat %d!!\n"),
                                      unit, FUNCTION_NAME(), __LINE__, SOC_MEM_NAME(unit, mem), bid, port, idx_offset,
                                      resInfo->p_threshold[idx_offset], resInfo->p_stat[idx_offset]));
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
                soc_cm_sfree(unit, dmabuf);
            } else if (reg != INVALIDr) {
                for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
                    idx_offset = idx + pipe_offset;
                    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval);
                    if (rv) {
                        return BCM_E_INTERNAL;
                    }
                    temp_val = soc_reg_field_get(unit, reg, rval, resInfo->stat_field);
                    if (bst_info->track_mode) {
                        if (resInfo->p_stat[idx_offset] < temp_val) {
                            resInfo->p_stat[idx_offset] = temp_val;
                        }
                    } else {
                        resInfo->p_stat[idx_offset] = temp_val;
                    }

                    if (resInfo->p_threshold[idx_offset] &&
                        (resInfo->p_stat[idx_offset] >=
                         (resInfo->p_threshold[idx_offset]) * resInfo->threshold_gran)) {
                         LOG_WARN(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d %s %d reg %s bid %d port %d pipe %d idx %d threshold %d stat %d!!\n"),
                                      unit, FUNCTION_NAME(), __LINE__, SOC_REG_NAME(unit, reg), bid, port, pipe, idx_offset,
                                      resInfo->p_threshold[idx_offset], resInfo->p_stat[idx_offset]));
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
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
soc_td2_intr_check_cb_register(int unit)
{
    BCM_IF_ERROR_RETURN(soc_td2_set_bst_intr_check_callback(unit, &_bcm_bst_td2_intr_check));
    return SOC_E_NONE;
}

STATIC int
_bcm_td2_pre_sync(int unit, bcm_bst_stat_id_t bid, int *sync_val)
{
    soc_field_t field = INVALIDf;
    _bcm_bst_resource_info_t *resInfo;
    uint32 rval;

    if ((bid < 0) || (bid >= bcmBstStatIdMaxCount)) {
        return BCM_E_INTERNAL;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);

    if (resInfo == NULL) {
        return BCM_E_INTERNAL;
    }

    if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_DEV) {
        field = BST_TRACK_EN_CFAPf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_ING) {
        field = BST_TRACK_EN_THDIf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_EGR) {
        field = BST_TRACK_EN_THDOf;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
    *sync_val = soc_reg_field_get(unit, BST_TRACKING_ENABLEr, rval, 
                                field);
    soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, field, 0);
    BCM_IF_ERROR_RETURN(WRITE_BST_TRACKING_ENABLEr(unit, rval));

    return SOC_E_NONE;
}

STATIC int
_bcm_td2_post_sync(int unit, bcm_bst_stat_id_t bid, int sync_val)
{
    soc_field_t field = INVALIDf;
    _bcm_bst_resource_info_t *resInfo;
    uint32 rval;

    if ((bid < 0) || (bid >= bcmBstStatIdMaxCount)) {
        return BCM_E_INTERNAL;
    }
    resInfo = _BCM_BST_RESOURCE(unit, bid);

    if (resInfo == NULL) {
        return BCM_E_INTERNAL;
    }

    if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_DEV) {
        field = BST_TRACK_EN_CFAPf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_ING) {
        field = BST_TRACK_EN_THDIf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_EGR) {
        field = BST_TRACK_EN_THDOf;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_BST_TRACKING_ENABLEr(unit, &rval));
    soc_reg_field_set(unit, BST_TRACKING_ENABLEr, &rval, field, sync_val);
    BCM_IF_ERROR_RETURN(WRITE_BST_TRACKING_ENABLEr(unit, rval));

    return SOC_E_NONE;
}

STATIC int 
_bcm_bst_td2_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *pres;
    int td2_3320 = 0;
    int idx_count = 0;

    td2_3320 = soc_feature(unit, soc_feature_min_resume_limit_1);
    
    bst_info = _BCM_UNIT_BST_INFO(unit);

    bst_info->to_cell = _bcm_bst_tr3_byte_to_cell;
    bst_info->to_byte = _bcm_bst_tr3_cell_to_byte;
    bst_info->control_set = _bcm_td2_bst_control_set;
    bst_info->control_get = _bcm_td2_bst_control_get;
    bst_info->intr_to_resources = _bcm_bst_td2_intr_to_resources;
    bst_info->hw_trigger_set = _bcm_bst_td2_hw_trigger_set;
    bst_info->hw_trigger_reset = _bcm_bst_td2_hw_trigger_reset;
    bst_info->hw_intr_cb = soc_td2_set_hw_intr_cb;
    bst_info->intr_enable_set = _bst_intr_enable_set;
    bst_info->pre_sync = _bcm_td2_pre_sync;
    bst_info->post_sync = _bcm_td2_post_sync;
    bst_info->hw_stat_snapshot = _bcm_bst_sync_hw_snapshot;
    bst_info->hw_stat_clear = _bcm_bst_hw_stat_clear;
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        bst_info->hw_sbusdma_desc_init = _bcm_bst_td2_sbusdma_desc_init;
        bst_info->hw_sbusdma_desc_deinit = _bcm_bst_td2_sbusdma_desc_deinit;
        bst_info->hw_sbusdma_desc_sync = _bcm_bst_td2_sbusdma_desc_sync;
    }
    if(soc_property_get(unit, spn_PROFILE_PG_1HDRM_8SHARED, 1)) {
        bst_info->mode = 1 ;
    } else {
        bst_info->mode = 0;
    }
    bst_info->max_bytes = bst_info->to_byte(unit, BST_THRESHOLD_MAX_HIGH);

    /* Register a callback for soc to do bst interrupt check during mmu ser processing*/
    soc_td2_intr_check_cb_register(unit);

    /* bcmBstStatIdDevice */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdDevice);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 0;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_DEV);
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        _BCM_BST_STAT_INFO(pres, INVALIDm, CFAP_BST_STATr, BST_STATf);
        _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, CFAP_BST_THRSr, BST_THRESHOLDf, 1);
    } else 
#endif
    {
        _BCM_BST_STAT_INFO(pres, INVALIDm, CFAPBSTSTATr, BST_STATf);
        _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, CFAPBSTTHRSr, BST_THRESHOLDf, 1);
    }
    _BCM_BST_RESOURCE_STAT_INIT(pres, 1, "bst cfap");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 1, "bst cfap");

    /* bcmBstStatIdEgrPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 3;
    pres->threshold_adj = td2_3320 ? -1 : 0;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_STAT_INFO(pres, INVALIDm, MMU_THDM_DB_POOL_MCUC_BST_STATr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                            MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr, BST_STATf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 4, "bst egrpool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 4, "bst egrpool");

    /* bcmBstStatIdEgrMCastPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrMCastPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 3;
    pres->threshold_adj = td2_3320 ? -1 : 0;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_STAT_INFO(pres, INVALIDm, MMU_THDM_DB_POOL_MC_BST_STATr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
                            MMU_THDM_DB_POOL_MC_BST_THRESHOLDr, BST_STATf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 4, "bst egr mcpool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 4, "bst egr mcpool");

    /* bcmBstStatIdIngPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdIngPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 3;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_STAT_INFO(pres, INVALIDm, THDI_BST_SP_GLOBAL_SHARED_CNTr, 
                        SP_GLOBAL_SHARED_CNTf);

    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, 
            THDI_BST_SP_GLOBAL_SHARED_PROFILEr, SP_GLOBAL_SHARED_PROFILEf, 1);

    _BCM_BST_RESOURCE_STAT_INIT(pres, 4, "bst ing pool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 4, "bst ing pool");

    /* bcmBstStatIdPortPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPortPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));

    _BCM_BST_THRESHOLD_INFO2(pres, THDI_PORT_SP_CONFIG_Xm, 
            THDI_PORT_SP_CONFIG_Ym, INVALIDr, INVALIDr, SP_BST_PROFILE_SHAREDf, 1);

    _BCM_BST_PROFILE_INFO(pres, INVALIDm, THDI_BST_SP_SHARED_PROFILEr, 
                          BST_SP_SHARED_THRESHf);

    _BCM_BST_STAT_INFO2(pres, THDI_PORT_SP_BST_Xm, THDI_PORT_SP_BST_Ym, 
                        INVALIDr, INVALIDr, SP_BST_STAT_PORT_COUNTf);

    _BCM_BST_RESOURCE_STAT_INIT(pres, 
                soc_mem_index_count(unit, THDI_PORT_SP_BST_Xm) * NUM_PIPE(unit),
                "bst port pool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 
                soc_mem_index_count(unit, THDI_PORT_SP_BST_Xm) * NUM_PIPE(unit),
                "bst port pool");
    pres->index_min = 0;
    pres->index_max = soc_mem_index_count(unit, THDI_PORT_SP_BST_Xm) *
                      NUM_PIPE(unit) - 1;

    /* bcmBstStatIdPriGroupShared */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupShared);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);

    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);

    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));

    _BCM_BST_THRESHOLD_INFO2(pres, THDI_PORT_PG_CONFIG_Xm, 
          THDI_PORT_PG_CONFIG_Ym, INVALIDr, INVALIDr, PG_BST_PROFILE_SHAREDf, 1);

    _BCM_BST_PROFILE_INFO(pres, INVALIDm, 
                          THDI_BST_PG_SHARED_PROFILEr, BST_PG_SHARED_THRESHf);

    _BCM_BST_STAT_INFO2(pres, THDI_PORT_PG_BST_Xm, THDI_PORT_PG_BST_Ym,
                        INVALIDr, INVALIDr, PG_BST_STAT_SHAREDf);

#if defined(BCM_APACHE_SUPPORT)
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        /* Stat res -THDI_PORT_PG_BST_X : 68 (ports) * 8 (PGs) */
        _BCM_BST_RESOURCE_STAT_INIT(pres, (68 * 8) * NUM_PIPE(unit), "bst pg");
        _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, (68 * 8) * NUM_PIPE(unit), "bst pg");
        pres->index_min = 0;
        pres->index_max = (68 * 8) * NUM_PIPE(unit) - 1;
    } else
#endif
    if (SOC_IS_APACHE(unit)) {
        /* Stat res - THDI_PORT_PG_BST_X/_Y : 75(Ports) * 8(PGs) * 1(pipes) */
        _BCM_BST_RESOURCE_STAT_INIT(pres, (75 * 8) * NUM_PIPE(unit), "bst pg");
        _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, (75 * 8) * NUM_PIPE(unit), "bst pg");
        pres->index_min = 0;
        pres->index_max = (75 * 8) * NUM_PIPE(unit) - 1;
    } else 
#endif
    {
        /* Stat res - THDI_PORT_PG_BST_X/_Y : 53(Ports) * 8(PGs) * 2(pipes) */
        _BCM_BST_RESOURCE_STAT_INIT(pres, (53 * 8) * NUM_PIPE(unit), "bst pg");
        _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, (53 * 8) * NUM_PIPE(unit), "bst pg");
        pres->index_min = 0;
        pres->index_max = (53 * 8) * NUM_PIPE(unit) - 1;
    }

    /* bcmBstStatIdPriGroupHeadroom */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdPriGroupHeadroom);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_ING);

    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_ALL(unit));

    _BCM_BST_PROFILE_INFO(pres, INVALIDm, THDI_BST_PG_HDRM_PROFILEr,
                          BST_PG_HDRM_THRESHf);

    _BCM_BST_THRESHOLD_INFO2(pres, THDI_PORT_PG_CONFIG_Xm,
        THDI_PORT_PG_CONFIG_Ym, INVALIDr, INVALIDr, PG_BST_PROFILE_HDRMf, 1);

    _BCM_BST_STAT_INFO2(pres, THDI_PORT_PG_BST_Xm, THDI_PORT_PG_BST_Ym,
                        INVALIDr, INVALIDr, PG_BST_STAT_HDRMf);

#if defined(BCM_APACHE_SUPPORT)
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        /* Stat res -THDI_PORT_PG_BST_X : 68 (ports) * 8 (PGs) */
        _BCM_BST_RESOURCE_STAT_INIT(pres, (68 * 8) * NUM_PIPE(unit), "bst pg hdrm");
        _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, (68 * 8) * NUM_PIPE(unit), "bst pg hdrm");
        pres->index_min = 0;
        pres->index_max = (68 * 8) * NUM_PIPE(unit) - 1;
    } else
#endif
    if (SOC_IS_APACHE(unit)) {
        /* Stat res - THDI_PORT_PG_BST_X/_Y : 75(Ports) * 8(PGs) * 2(pipes) */
        _BCM_BST_RESOURCE_STAT_INIT(pres, (75 * 8) * NUM_PIPE(unit), "bst pg hdrm");
        _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, (75 * 8) * NUM_PIPE(unit), "bst pg hdrm");
        pres->index_min = 0;
        pres->index_max = (75 * 8) * NUM_PIPE(unit) - 1;
    } else 
#endif
    {
        /* Stat res - THDI_PORT_PG_BST_X/_Y : 53(Ports) * 8(PGs) * 2(pipes) */
        _BCM_BST_RESOURCE_STAT_INIT(pres, (53 * 8) * NUM_PIPE(unit), "bst pg hdrm");
        _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, (53 * 8) * NUM_PIPE(unit), "bst pg hdrm");
        pres->index_min = 0;
        pres->index_max = (53 * 8) * NUM_PIPE(unit) - 1;
    }

    /* bcmBstStatIdMcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdMcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_CMIC(unit));
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, 
                   MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr, BST_STATf);

    _BCM_BST_THRESHOLD_INFO2(pres, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                             MMU_THDM_DB_QUEUE_CONFIG_1m, INVALIDr, INVALIDr,
                             BST_THRESHOLD_PROFILEf, 1);

    idx_count = soc_mem_index_count(unit, MMU_THDM_DB_QUEUE_BST_0m);
    _BCM_BST_STAT_INFO2(pres, MMU_THDM_DB_QUEUE_BST_0m, MMU_THDM_DB_QUEUE_BST_1m,
                        INVALIDr, INVALIDr, DATAf);
    _BCM_BST_RESOURCE_STAT_INIT(pres, idx_count * NUM_PIPE(unit),
                "bst mcast");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, idx_count * NUM_PIPE(unit),
                "bst mcast");
    pres->index_min = 0;
    pres->index_max = idx_count * NUM_PIPE(unit) - 1;

    /* bcmBstStatIdUcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdUcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID | 
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR);

    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));

    _BCM_BST_THRESHOLD_INFO2(pres, MMU_THDU_XPIPE_CONFIG_QUEUEm, 
        MMU_THDU_YPIPE_CONFIG_QUEUEm, INVALIDr, INVALIDr, Q_WM_MAX_THRESHOLDf, 8);

    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_QUEUE_BST_THRESHOLDr, BST_STATf);

    _BCM_BST_STAT_INFO2(pres, MMU_THDU_XPIPE_BST_QUEUEm, 
                MMU_THDU_YPIPE_BST_QUEUEm, INVALIDr, INVALIDr, DATAf);

    idx_count = soc_mem_index_count(unit, MMU_THDU_XPIPE_BST_QUEUEm);
    _BCM_BST_RESOURCE_STAT_INIT(pres, idx_count * NUM_PIPE(unit),
                "bst ucast");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, idx_count * NUM_PIPE(unit),
                "bst ucast"); 
    pres->index_min = 0;
    pres->index_max = idx_count * NUM_PIPE(unit) - 1;

    /* bcmBstStatIdEgrPortPoolSharedUcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedUcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR);

    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));

    _BCM_BST_THRESHOLD_INFO2(pres, MMU_THDU_XPIPE_CONFIG_PORTm,
            MMU_THDU_YPIPE_CONFIG_PORTm, INVALIDr, INVALIDr, WM_MAX_THRESHOLDf,
            8);

    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_PORT_BST_THRESHOLDr, BST_STATf);

    _BCM_BST_STAT_INFO2(pres, MMU_THDU_XPIPE_BST_PORTm,
            MMU_THDU_YPIPE_BST_PORTm, INVALIDr, INVALIDr, DATAf);
    idx_count = soc_mem_index_count(unit, MMU_THDU_XPIPE_BST_PORTm);
    _BCM_BST_RESOURCE_STAT_INIT(pres, idx_count * NUM_PIPE(unit),
                                "bst egr port pool shared ucast");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, idx_count * NUM_PIPE(unit),
                                     "bst egr port pool shared ucast");
    pres->index_min = 0;
    pres->index_max = idx_count * NUM_PIPE(unit) - 1;

    /* bcmBstStatIdEgrPortPoolSharedMcast */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedMcast);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR |
                                     _BCM_BST_CMN_RES_F_PORTED);

    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));

    _BCM_BST_THRESHOLD_INFO2(pres, INVALIDm, INVALIDm,
                             MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr,
                             MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr,
                             BST_THRESHOLD_PROFILEf, 1);

    _BCM_BST_PROFILE_INFO(pres, INVALIDm,MMU_THDM_DB_PORTSP_BST_THRESHOLDr,
                          BST_STATf);

    _BCM_BST_STAT_INFO2(pres, MMU_THDM_DB_PORTSP_BST_0m,
                        MMU_THDM_DB_PORTSP_BST_1m, INVALIDr, INVALIDr, DATAf);
    idx_count = soc_mem_index_count(unit, MMU_THDM_DB_PORTSP_BST_0m);
    _BCM_BST_RESOURCE_STAT_INIT(pres, idx_count * NUM_PIPE(unit),
                                "bst egr port pool shared mcast");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, idx_count * NUM_PIPE(unit),
                                     "bst egr port pool shared mcast");
    pres->index_min = 0;
    pres->index_max = idx_count * NUM_PIPE(unit) - 1;

    /* bcmBstStatIdRQEQueue */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdRQEQueue);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 10;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_STAT_INFO(pres, INVALIDm, MMU_THDR_DB_BST_STAT_PRIQr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            MMU_THDR_DB_BST_THRESHOLD_PRIQr, BST_THRf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 11, "bst RQE queue");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 11, "bst RQE queue");

    /* bcmBstStatIdRQEPool */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdRQEPool);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    pres->index_min = 0;
    pres->index_max = 3;
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    _BCM_BST_STAT_INFO(pres, INVALIDm, MMU_THDR_DB_BST_STAT_SPr, BST_STATf);
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm,
                            MMU_THDR_DB_BST_THRESHOLD_SPr, BST_THRf, 1);
    _BCM_BST_RESOURCE_STAT_INIT(pres, 4, "bst RQE pool");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, 4, "bst RQE pool");

    /* bcmBstStatIdUCQueueGroup */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdUCQueueGroup);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_PROFILEID |
                                     _BCM_BST_CMN_RES_F_PIPED |
                                     _BCM_BST_CMN_RES_F_RES_EGR |
                                     _BCM_BST_CMN_RES_F_PORTED);
    _BCM_BST_RESOURCE_PBMP_SET(pres, PBMP_PORT_ALL(unit));

    _BCM_BST_THRESHOLD_INFO2(pres, MMU_THDU_XPIPE_CONFIG_QGROUPm,
        MMU_THDU_YPIPE_CONFIG_QGROUPm, INVALIDr, INVALIDr, Q_WM_MAX_THRESHOLDf, 8);

    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_QGROUP_BST_THRESHOLDr, BST_STATf);

    _BCM_BST_STAT_INFO2(pres, MMU_THDU_XPIPE_BST_QGROUPm,
                MMU_THDU_YPIPE_BST_QGROUPm, INVALIDr, INVALIDr, DATAf);

    idx_count = soc_mem_index_count(unit, MMU_THDU_XPIPE_BST_QGROUPm);
    _BCM_BST_RESOURCE_STAT_INIT(pres, idx_count * NUM_PIPE(unit),
                "bst unicast queue group");
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, idx_count * NUM_PIPE(unit),
                "bst unicast queue group");
    pres->index_min = 0;
    pres->index_max = idx_count * NUM_PIPE(unit) - 1;

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2_SUPPORT || BCM_APACHE_SUPPORT*/

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_APACHE_SUPPORT)
STATIC int
_bcm_bst_sync_thread_ok_to_run(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if ((bst_info == NULL) || (!_BCM_BST_SYNC_THREAD_VALID(bst_info)) ||
        (bst_info->interval <= 0)) {
        return FALSE;
    }

    return TRUE;
}

int
_bst_intr_enable_set(int unit, int enable)
{
    uint64 rval64;
    uint32 rval;

    if (!soc_property_get(unit, spn_POLLED_IRQ_MODE, 0)) {
        if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_EN_64r)) {
            BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_EN_64r(unit, &rval64));
            soc_reg64_field32_set(unit, MEM_FAIL_INT_EN_64r, &rval64,
                                  BST_THDO_INT_ENf, enable);
            soc_reg64_field32_set(unit, MEM_FAIL_INT_EN_64r, &rval64,
                                  BST_THDI_INT_ENf, enable);
            soc_reg64_field32_set(unit, MEM_FAIL_INT_EN_64r, &rval64,
                                  BST_CFAP_INT_ENf, enable);
            BCM_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_EN_64r(unit, rval64));
        } else if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_ENr)) {
            if (!soc_property_get(unit, spn_POLLED_IRQ_MODE, 0)) {
                BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_ENr(unit, &rval));
                soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                                  BST_THDO_INT_ENf, enable);
                soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                                  BST_THDI_INT_ENf, enable);
                soc_reg_field_set(unit, MEM_FAIL_INT_ENr, &rval,
                                  BST_CFAP_INT_ENf, enable);
                BCM_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_ENr(unit, rval));
            }
        }
    }
    return 0;
}

int
_bst_intr_status_reset(int unit)
{
    uint32 rval;
    uint64 rval64;

    if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_STAT_64r)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STAT_64r(unit, &rval64));
        soc_reg64_field32_set(unit, MEM_FAIL_INT_STAT_64r,
                              &rval64, BST_CFAP_INTf, 0);
        soc_reg64_field32_set(unit, MEM_FAIL_INT_STAT_64r,
                              &rval64, BST_THDO_INTf, 0);
        soc_reg64_field32_set(unit, MEM_FAIL_INT_STAT_64r,
                              &rval64, BST_THDI_INTf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_STAT_64r(unit, rval64));
    } else if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_STATr)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &rval));
        soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &rval,
                          BST_CFAP_INTf, 0);
        soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &rval,
                          BST_THDO_INTf, 0);
        soc_reg_field_set(unit, MEM_FAIL_INT_STATr, &rval,
                          BST_THDI_INTf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_STATr(unit, rval));
    }

    return BCM_E_NONE;
}

int
_bcm_bst_hw_event_cb(int unit)
{
    
    _bcm_bst_cmn_unit_info_t *bst_info;
    bst_info = _BCM_UNIT_BST_INFO(unit);

    if ((!bst_info) || (bst_info->bst_trigger == NULL)) {
        return 0;
    }
    if (bst_info->intr_enable_set) {
        bst_info->intr_enable_set(unit, 0);
    }
    sal_sem_give(bst_info->bst_trigger);
    return 0;
}

STATIC void
_bcm_bst_sync_thread(void *cookie)
{
    int unit;
    uint32 res_flags = 0;
    _bcm_bst_cmn_unit_info_t *bst_info;

    unit = PTR_TO_INT(cookie);

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        sal_thread_exit(0);
        return;
    }

    if (bst_info->hw_intr_cb) {
        bst_info->hw_intr_cb(unit);
    }

    if (bst_info->intr_enable_set) {
        bst_info->intr_enable_set(unit, 1);
    }

    /* When Thread VALID flag is disabled, loop should break,
     * thread must exit.
     */
    while (_bcm_bst_sync_thread_ok_to_run(unit) == TRUE) {
        /* When Thread ENABLE flag is disabled, thread operations are paused
         * until ENABLE flag is enabled.
         */
        if (_BCM_BST_SYNC_ENABLED(bst_info) == TRUE) {
            if (bst_info->bst_trigger) {

                /*
                _bst_intr_enable_set(unit, 1);
                */

                if (sal_sem_take(bst_info->bst_trigger, bst_info->interval) < 0) {
                    if (bst_info->interval <= 0) {
                        break;
                    }
                    continue;
                }
            } else {
                sal_usleep(bst_info->interval);
            }

            if (bst_info->interval <= 0) {
                break;
            }

            res_flags = 0;
            if (bst_info->intr_to_resources) {
                sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
                bst_info->intr_to_resources(unit, &res_flags);
                sal_mutex_give(bst_info->bst_reslock);
            }
        } else {
            sal_usleep(bst_info->interval);
        }
    }

    if (bst_info->intr_enable_set) {
        bst_info->intr_enable_set(unit, 0);
    }
    bst_info->sync_flags &= ~_BCM_BST_SYNC_F_ENABLE;
    bst_info->sync_flags &= ~_BCM_BST_SYNC_F_THREAD_VALID;

    sal_sem_give(bst_info->bst_thread);
    sal_thread_exit(0);
    return;
}

int 
_bcm_bst_sync_thread_start(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv = BCM_E_NONE;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_INIT;
    }

    if (_BCM_BST_SYNC_THREAD_VALID(bst_info)) {
        return BCM_E_NONE;
    }

    if (bst_info->hw_sbusdma_desc_init) {
        rv = bst_info->hw_sbusdma_desc_init(unit);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(bst_info->hw_sbusdma_desc_deinit(unit));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "hw_sbusdma_desc_init failed rv = %d\n"), rv));
            return BCM_E_INIT;
        }
    }

    bst_info->sync_flags |= _BCM_BST_SYNC_F_THREAD_VALID;
    bst_info->interval = 1000000;
    bst_info->bst_thread_id = sal_thread_create("bcm_bst_sync_thread",
                                                SAL_THREAD_STKSZ,
                                                soc_property_get(unit, spn_BST_SYNC_THREAD_PRI, 50),
                                                _bcm_bst_sync_thread, 
                                                INT_TO_PTR(unit));

    _BCM_BST_SYNC_THREAD_ENABLE_SET(unit, TRUE);
    return rv;
}

int 
_bcm_bst_sync_thread_stop(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_INIT;
    }

    if (!_BCM_BST_SYNC_THREAD_VALID(bst_info)) {
        return BCM_E_NONE;
    }

    bst_info->sync_flags &= ~_BCM_BST_SYNC_F_ENABLE;
    bst_info->sync_flags &= ~_BCM_BST_SYNC_F_THREAD_VALID;

    if (bst_info->bst_trigger) {
        sal_sem_give(bst_info->bst_trigger);
    }

    /* If thread already running wait for it to be done before stopping */
    (void)sal_sem_take(bst_info->bst_thread, sal_sem_FOREVER);

    if (bst_info->hw_sbusdma_desc_deinit) {
        BCM_IF_ERROR_RETURN(bst_info->hw_sbusdma_desc_deinit(unit));
    }

    return BCM_E_NONE;    
}

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
STATIC int
_bcm_bst_check_intr_status(int unit, bcm_bst_stat_id_t bid)
{
    soc_field_t fld = INVALIDf;
    _bcm_bst_resource_info_t *resInfo;
    uint64 rval64;
    uint32 rval;
    uint32 fval = 0;

    resInfo = _BCM_BST_RESOURCE(unit, bid);

    if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_DEV) {
        fld = BST_CFAP_INTf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_ING) {
        fld = BST_THDI_INTf;
    } else if (resInfo->flags & _BCM_BST_CMN_RES_F_RES_EGR) {
        fld = BST_THDO_INTf;
    }
    
    if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_EN_64r)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STAT_64r(unit, &rval64));
        fval = soc_reg64_field32_get(unit, MEM_FAIL_INT_STAT_64r,
                                    rval64, fld);
    } else if (SOC_REG_IS_VALID(unit, MEM_FAIL_INT_ENr)) {
        BCM_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &rval));
        fval = soc_reg_field_get(unit, MEM_FAIL_INT_STATr, rval,
                                fld);
    }
    return fval;
}
#endif
#endif

int
_bcm_bst_cmn_profile_delete_hw(int unit, bcm_bst_stat_id_t bid, 
                               int id, int profile_index)
{
    _bcm_bst_resource_info_t *resInfo;
    
    resInfo = _BCM_BST_RESOURCE(unit, bid);

    if (resInfo->profile_r[id]) {
        SOC_IF_ERROR_RETURN
            (soc_profile_reg_delete(unit, resInfo->profile_r[id], profile_index));
    } else if (resInfo->profile_m) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, resInfo->profile_m, profile_index));
    } else {
        return BCM_E_INIT;
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_default_profile_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int ei, pipe, index_max = 0;
    bcm_bst_stat_id_t bid;
    int entry_size;
    bcm_cosq_bst_profile_t profile;
    int i, wb = 0;
    uint32 profile_index;

    wb = SOC_WARM_BOOT(unit) ? 1 : 0;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (bst_info->profile_init) {
        return bst_info->profile_init(unit);
    }

    _BCM_BST_RESOURCE_ITER(bst_info, bid) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);

        if ((resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) == 0) {
            continue;
        }
    
        for (i = 0; i < _BCM_MAX_PROFILES; i ++) {
            resInfo->profile_r[i] = NULL;
        }
        resInfo->profile_m = NULL;
        if (resInfo->profile_mem != INVALIDm) {
            soc_profile_mem_t_init(&resInfo->iprofile_m);
            entry_size = soc_mem_entry_bytes(unit, resInfo->profile_mem);
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_create(unit, &resInfo->profile_mem, 
                                   &entry_size, 1, &resInfo->iprofile_m));
            resInfo->profile_m = &resInfo->iprofile_m;
        } else if (resInfo->profile_reg[0] != INVALIDr) {
            soc_profile_reg_t_init(&resInfo->iprofile_r[0]);
            SOC_IF_ERROR_RETURN
                (soc_profile_reg_create(unit, &resInfo->profile_reg[0], 
                                        1, &resInfo->iprofile_r[0]));
            resInfo->profile_r[0] = &resInfo->iprofile_r[0];
        }
    }

    /* create default profiles. */
    _BCM_BST_RESOURCE_ITER(bst_info, bid) {
        profile.byte = bst_info->max_bytes;
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                if (resInfo->threshold_gran != 0) {
                    profile.byte = bst_info->to_byte(unit,
                        BST_THRESHOLD_MAX_HIGH - (resInfo->threshold_gran - 1));
                }
            }
#endif
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                if ((resInfo->threshold_mem[pipe] == INVALIDm) && 
                    (resInfo->threshold_reg[pipe] == INVALIDr)) {
                    continue;
                }
                
                if (!wb) {
                    SOC_IF_ERROR_RETURN(_bcm_bst_cmn_profile_set_hw(unit, pipe, 
                        BCM_BST_DUP_XPE, -1, bid, &profile, &profile_index));
                }
                if (SOC_IS_TOMAHAWKX(unit)) {
                    /* TH has 4 MMU instances unlike previous devices
                       where MMU instance is 1 */
                    index_max = (resInfo->index_max + 1) / (NUM_PIPE(unit) *
                                resInfo->num_instance);
                } else {
                    index_max = (resInfo->index_max + 1) / NUM_PIPE(unit);
                }
                for (ei = resInfo->index_min; ei < index_max; ei++) {
                    if (wb) {
                        SOC_IF_ERROR_RETURN(
                           _bcm_bst_cmn_io_op(unit,
                                              BST_OP_GET | BST_OP_THD,
                                              pipe, ei, bid,
                                              resInfo->threshold_mem[pipe],
                                              resInfo->threshold_reg[pipe],
                                              resInfo->threshold_field,
                                              &profile_index));
                    }

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
                    if (SOC_IS_TD2_TT2(unit) && !SOC_IS_TOMAHAWKX(unit) &&
                        !SOC_IS_TRIDENT3X(unit)) {
                        if (((bid == bcmBstStatIdPriGroupHeadroom) && 
                             (bst_info->mode == 1)) ||
                             ((bid == bcmBstStatIdPriGroupShared) && 
                              (bst_info->mode == 0))) {
                            break;
                        }
                    }
#endif
                    if (resInfo->profile_r[0]) {
                        BCM_IF_ERROR_RETURN(soc_profile_reg_reference(unit,
                               resInfo->profile_r[0], profile_index, 1));
                    } else if (resInfo->profile_m) {
                        BCM_IF_ERROR_RETURN(soc_profile_mem_reference(unit,
                               resInfo->profile_m, profile_index, 1));
                    }
                }
            }
        } 
    }
    
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIDENT2(unit)) {
        /* Set default threshold for bcmBstStatIdDevice and bcmBstStatIdIngPool
         * from zero to max to avoid unexpected trigger.
         */
        int pool_num = 4;
        bcm_cosq_bst_profile_t_init(&profile);
        profile.byte = bst_info->max_bytes;
        for (ei = 0; ei < pool_num; ei++) {
            SOC_IF_ERROR_RETURN(_bcm_bst_cmn_profile_set_hw
                (unit, -1, BCM_BST_DUP_XPE, ei, 
                    bcmBstStatIdIngPool, &profile, NULL));
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPBSTTHRSr,
                                        REG_PORT_ANY, BST_THRESHOLDf,
                                        BST_THRESHOLD_MAX_HIGH));
    }
#endif

    return BCM_E_NONE;
}

int _bcm_bst_attach(int unit, _bcm_bst_device_handlers_t *cbs)
{
    _bcm_bst_cmn_unit_info_t *bst_info;

    (void)_bcm_bst_detach(unit);

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (NULL == bst_info) {
        bst_info = sal_alloc(sizeof(_bcm_bst_cmn_unit_info_t), "bst_dev_info");
        if (!bst_info) {
            return BCM_E_MEMORY;
        }
    }

    sal_memset(bst_info, 0, sizeof(_bcm_bst_cmn_unit_info_t));

    if (cbs) {
        sal_memcpy(&bst_info->handlers, cbs, sizeof(_bcm_bst_device_handlers_t));
    }

    _BCM_UNIT_BST_INFO(unit) = bst_info;

     if (bst_info->bst_trigger == NULL) {
        bst_info->bst_trigger = sal_sem_create("bst_trigger", sal_sem_BINARY, 0);
    }

    if (bst_info->bst_trigger == NULL) {
        (void)_bcm_bst_detach(unit);
        return BCM_E_MEMORY;
    }

    if (bst_info->bst_thread == NULL) {
        bst_info->bst_thread = sal_sem_create("bst_thread", sal_sem_BINARY, 0);
    }

    if (bst_info->bst_thread == NULL) {
        (void)_bcm_bst_detach(unit);
        return BCM_E_MEMORY;
    }

    /* Create protection mutex. */
    if (bst_info->bst_reslock == NULL) {
        bst_info->bst_reslock = sal_mutex_create("bst_resource_lock");
    }
    
    if (bst_info->bst_reslock == NULL) {
        (void)_bcm_bst_detach(unit);
        return BCM_E_MEMORY;
    }
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN(bcm_bst_th3_init(unit));
    } else
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        SOC_IF_ERROR_RETURN(bcm_bst_th_init(unit));
    } else
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        SOC_IF_ERROR_RETURN(bcm_bst_td3_init(unit));
    } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        SOC_IF_ERROR_RETURN(_bcm_bst_triumph3_init(unit));
    } else
#endif
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        SOC_IF_ERROR_RETURN(_bcm_bst_td2_init(unit));
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }

    /* setup default profile. default profile is the max limit
     * of the buffer sizes and is equivalent of being disabled.
     */

    SOC_IF_ERROR_RETURN(_bcm_bst_default_profile_init(unit));

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_WARM_BOOT(unit) && (SOC_IS_TRIDENT2(unit) || 
        SOC_IS_APACHE(unit))) {
        BCM_IF_ERROR_RETURN(_bcm_td2_bst_info_restore(unit));
        BCM_IF_ERROR_RETURN(_bcm_td2_bst_resource_threshold_restore(unit));
        if (_bcm_td2_bst_sync_thread_restore(unit) == TRUE) {
            BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
        }
    } else
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_WARM_BOOT(unit) && (SOC_IS_TOMAHAWKX(unit) ||
        SOC_IS_TOMAHAWK2(unit)) && !SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th_bst_info_restore(unit));
        BCM_IF_ERROR_RETURN(_bcm_th_bst_resource_threshold_restore(unit));
        if (_bcm_th_bst_sync_thread_restore(unit) == TRUE) {
            BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
        }
    } else
#endif
#if defined (BCM_TOMAHAWK3_SUPPORT)
    if (SOC_WARM_BOOT(unit) && SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_bst_info_restore(unit));
        BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
    } else
#endif
    {
        /* Initialize the trigger and Snapshot regs */
        if (bst_info->hw_trigger_reset) {
            bst_info->hw_trigger_reset(unit, -1);
        }
    }

    return BCM_E_NONE;
}

int _bcm_bst_detach(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    bcm_bst_stat_id_t bid;
    int i;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_NONE;
    }

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
    BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_stop(unit));
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIDENT2_SUPPORT || BCM_TOMAHAWK_SUPPORT
        * BCM_TRIDENT3_SUPPORT || BCM_APACHE_SUPPORT
        */

    _BCM_BST_RESOURCE_ITER(bst_info, bid) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo->p_stat) {
            sal_free(resInfo->p_stat);
            resInfo->p_stat = NULL;
        }
        if (resInfo->p_threshold) {
            sal_free(resInfo->p_threshold);
            resInfo->p_threshold = NULL;
        }
        soc_profile_mem_destroy(unit, &resInfo->iprofile_m);
        for (i = 0; i < _BCM_MAX_PROFILES; i++) {
            soc_profile_reg_destroy(unit, &resInfo->iprofile_r[i]);
            resInfo->profile_r[i] = NULL;
        }
        resInfo->profile_m = NULL;
    }

    sal_sem_destroy(bst_info->bst_trigger);
    sal_sem_destroy(bst_info->bst_thread);
    sal_mutex_destroy(bst_info->bst_reslock);
    bst_info->bst_trigger = NULL;
    bst_info->bst_thread = NULL;
    bst_info->bst_reslock = NULL;

    sal_free(bst_info);

    _BCM_UNIT_BST_INFO(unit) = NULL;

    return BCM_E_NONE;
}

int _bst_thd_reg_index_get(int unit, bcm_bst_stat_id_t bid, int pipe,
                           int hw_index, int *inst, int *index)
{
    if (bid == bcmBstStatIdEgrPortPoolSharedMcast) {
        int phy_port, mmu_port, mmu_port_base;
        soc_info_t *si = &SOC_INFO(unit);

        mmu_port_base = 64 * pipe;
        mmu_port = hw_index / 4;
        phy_port = si->port_m2p_mapping[mmu_port + mmu_port_base];
        *inst = si->port_p2l_mapping[phy_port];
        *index = hw_index % 4;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_bst_cmn_io_op(int unit, int op, int pipe, int hw_index,
                   bcm_bst_stat_id_t bid, soc_mem_t mem, soc_reg_t reg,
                   soc_field_t field, uint32 *resval)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    int port = REG_PORT_ANY, reg_index = hw_index;
    _bcm_bst_resource_info_t *resInfo;
#if defined(BCM_TOMAHAWK_SUPPORT)
    _bcm_bst_cmn_unit_info_t *bst_info = NULL;
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
            return BCM_E_INIT;
        }

        return bst_info->bst_cmn_io_op(unit, op, pipe, hw_index, bid,
                                       mem, reg, field, resval);
    }
#endif
  resInfo = _BCM_BST_RESOURCE(unit, bid);
  if ((resInfo->flags & _BCM_BST_CMN_RES_F_PORTED) &&
     (op & BST_OP_THD)) {
      BCM_IF_ERROR_RETURN(
       _bst_thd_reg_index_get(unit, bid, pipe, hw_index,
                                 &port, &reg_index));
      if ((bid == bcmBstStatIdEgrPortPoolSharedMcast) && (port == -1)) {
           port = REG_PORT_ANY;
      }
  }

    if (mem != INVALIDm) {
        SOC_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, reg_index, entry));
        if (op & BST_OP_GET) {
            *resval = soc_mem_field32_get(unit, mem, entry, field);
        } else if (op & BST_OP_SET) {
            soc_mem_field32_set(unit, mem, entry, field, *resval);
            SOC_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, reg_index, entry));
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

    return BCM_E_NONE;
}

STATIC int 
_bcm_bst_cmn_profile_set_hw(int unit, int pipe, int xpe, int hw_index, 
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile, 
                            uint32 *p_profile)
{
    uint32 cells;
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    uint32 profile_index = 0xffffffff, old_profile_index = 0xffffffff;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint64 rval, *rvals[1];
    void *entries[1];
    int pipe_factor = 1, pipe_offset = 0;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    _bcm_bst_resource_info_t *resInfo1 = NULL;
    int index = 0 , num_entries = 0;
    int start_idx = 0;
#endif

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if ((bid >= bcmBstStatIdMaxCount) || (profile == NULL)) {
        return BCM_E_PARAM;
    }

    if (p_profile) {
        *p_profile = -1;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bid);

    if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
        return BCM_E_UNAVAIL;
    }

    if (bst_info->thd_set) {
        return bst_info->thd_set(unit, pipe, xpe, hw_index, bid, 
            profile, p_profile);
    }

    if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
        pipe_factor = NUM_PIPE(unit);
        if (pipe) {
            if (resInfo->num_instance > 1) {
                pipe_offset = pipe * ((resInfo->index_max + 1) / pipe_factor /
                                      resInfo->num_instance);
            } else {
                pipe_offset = pipe * ((resInfo->index_max + 1) / pipe_factor);
            }
        }
    } else {
        pipe = 0;
    }

    /* Taking granularity into count */
    if (resInfo->threshold_gran != 0) {
        cells = (bst_info->to_cell(unit, profile->byte) +
                resInfo->threshold_adj + resInfo->threshold_gran - 1) /
                resInfo->threshold_gran;
    } else {
        cells = bst_info->to_cell(unit, profile->byte) + resInfo->threshold_adj;
    }

#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit)) {
        if ((bid == bcmBstStatIdDevice) && (cells < _BCM_AP_BST_MIN_THRESHOLD_CELLS)) {
            cells = _BCM_AP_BST_MIN_THRESHOLD_CELLS;
        }
    }
#endif

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        if (hw_index >= 0) {
            if (BST_THRESHOLD_INVALID == profile->byte) {
                cells = BST_THRESHOLD_MAX_HIGH;
            } else {
                resInfo->p_threshold[hw_index + pipe_offset] = cells;
            }
        }
    } else
#endif
    {
        if (hw_index >= 0) {
            resInfo->p_threshold[hw_index + pipe_offset] = cells;
        }
    }

    if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
        if (hw_index >= 0) {
            SOC_IF_ERROR_RETURN(
               _bcm_bst_cmn_io_op(unit, BST_OP_GET | BST_OP_THD,
                                  pipe, hw_index, bid,
                                  resInfo->threshold_mem[pipe],
                                  resInfo->threshold_reg[pipe],
                                  resInfo->threshold_field,
                                  &old_profile_index));
        }

        if (resInfo->profile_r[0]) {
            COMPILER_64_ZERO(rval);
            soc_reg64_field32_set(unit, resInfo->profile_reg[0], &rval, 
                              resInfo->profile_field, cells);
            rvals[0] = &rval;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            if (SOC_IS_TD2_TT2(unit) && !SOC_IS_TOMAHAWKX(unit) &&
                !SOC_IS_TRIDENT3X(unit)) {
                if (((bid == bcmBstStatIdPriGroupHeadroom) &&
                     (bst_info->mode == 1) ) ||
                    ((bid == bcmBstStatIdPriGroupShared) && 
                     (bst_info->mode == 0))) {
                    num_entries = SOC_REG_NUMELS(unit,
                            resInfo->profile_reg[0]);
                    /*
                     * start_idx is 0 only in case hw_index = -1
                     */
                    start_idx = hw_index < 0 ? 0: 1 ;
                    for( index = start_idx; 
                            index < num_entries ; index++ ) {
                        /* Insert the new entries into profile table */
                        SOC_IF_ERROR_RETURN(soc_reg_set(unit, 
                                    resInfo->profile_reg[0], REG_PORT_ANY,
                                    index, 
                                    rval));
                    }
                    if ( hw_index >= 0) {
                        old_profile_index = profile_index = 0xffffffff ;
                    } else {
                        profile_index = 0 ;
                    }
                }  else {
                    SOC_IF_ERROR_RETURN(soc_profile_reg_add(unit,
                                resInfo->profile_r[0],
                                rvals, 1,
                                &profile_index));
                }
            } else
#endif
            {
                SOC_IF_ERROR_RETURN(soc_profile_reg_add(unit,
                            resInfo->profile_r[0],
                            rvals, 1,
                            &profile_index));

            }
        } else if (resInfo->profile_m) {
            sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
            soc_mem_field32_set(unit, resInfo->profile_mem, &entry,
                                resInfo->profile_field, cells);
            entries[0] = entry;
            SOC_IF_ERROR_RETURN(soc_profile_mem_add(unit,
                                                    resInfo->profile_m,
                                                    entries, 1,
                                                    &profile_index));
        }

        /* attach the profile id for the resource */
        if (profile_index != 0xffffffff) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            if ((SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)
                || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT3X(unit))
                && (bid == bcmBstStatIdMcast)) {
                uint32 value = 0;
                soc_reg_field_set(unit, MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILEr,
                    &value, THRESHOLDf, cells);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit,
                    MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILEr,
                    REG_PORT_ANY, profile_index, value));
            }
#endif /* BCM_TRIDENT2_SUPPORT */

            if (hw_index >= 0) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
                if (SOC_IS_TD2_TT2(unit) && !SOC_IS_TOMAHAWKX(unit) &&
                    !SOC_IS_TRIDENT3X(unit)) {
                    if (((bid == bcmBstStatIdPriGroupHeadroom) &&
                         (bst_info->mode == 0)) ||
                        ((bid == bcmBstStatIdPriGroupShared) &&
                         (bst_info->mode == 1))) {
                        if ( bid == bcmBstStatIdPriGroupShared ) {
                            resInfo1 = _BCM_BST_RESOURCE(unit,
                                    bcmBstStatIdPriGroupHeadroom);
                            if (!_BCM_BST_RESOURCE_VALID(resInfo1)) {
                                return BCM_E_UNAVAIL;
                            }
                        } else if ( bid == bcmBstStatIdPriGroupHeadroom) {
                            resInfo1 = _BCM_BST_RESOURCE(unit,
                                    bcmBstStatIdPriGroupShared);
                            if (!_BCM_BST_RESOURCE_VALID(resInfo1)) {
                                return BCM_E_UNAVAIL;
                            }
                        }
                        SOC_IF_ERROR_RETURN(
                            _bcm_bst_cmn_io_op(unit,
                                               BST_OP_SET | BST_OP_THD,
                                               pipe, hw_index, bid,
                                               resInfo1->threshold_mem[pipe],
                                               resInfo1->threshold_reg[pipe],
                                               resInfo1->threshold_field,
                                               &profile_index));

                    }
                }
#endif
                SOC_IF_ERROR_RETURN(
                    _bcm_bst_cmn_io_op(unit, BST_OP_SET | BST_OP_THD,
                                       pipe, hw_index, bid,
                                       resInfo->threshold_mem[pipe],
                                       resInfo->threshold_reg[pipe],
                                       resInfo->threshold_field,
                                       &profile_index));
            }
            if (p_profile) {
                *p_profile = profile_index;
            }
        }

        if (old_profile_index != 0xffffffff) {
            SOC_IF_ERROR_RETURN(_bcm_bst_cmn_profile_delete_hw(unit, bid,
                                                    0, old_profile_index));
        }
    } else {
        SOC_IF_ERROR_RETURN(
            _bcm_bst_cmn_io_op(unit, BST_OP_SET | BST_OP_THD,
                               pipe, hw_index, bid,
                               resInfo->threshold_mem[pipe],
                               resInfo->threshold_reg[pipe],
                               resInfo->threshold_field,
                               &cells));
    }

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        if (bst_info->hw_trigger_set && cells && (BST_THRESHOLD_MAX_HIGH != cells)) {
            bst_info->hw_trigger_set(unit, bid);
        }
    } else
#endif
    {
        if (bst_info->hw_trigger_set && cells) {
            bst_info->hw_trigger_set(unit, bid);
        }
    }
    return BCM_E_NONE;
}

STATIC int 
_bcm_bst_cmn_profile_get_hw(int unit, int pipe, int xpe, int hw_index, 
                            bcm_bst_stat_id_t bid, 
                            bcm_cosq_bst_profile_t *profile)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    uint32 profile_index;
    uint32 cells;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if ((bid >= bcmBstStatIdMaxCount) || (profile == NULL)) {
        return BCM_E_PARAM;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bid);

    if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
        return BCM_E_UNAVAIL;
    }

    if (bst_info->thd_get) {
        return bst_info->thd_get(unit, pipe, xpe, hw_index, bid, profile);
    }

    if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
        /* Dont use Pipe info, if the resource is Not Pipe specific */
        pipe = 0;
    }
    /* attach the profile id for the resource */
    if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
        SOC_IF_ERROR_RETURN(
           _bcm_bst_cmn_io_op(unit, BST_OP_GET | BST_OP_THD,
                              pipe, hw_index, bid,
                              resInfo->threshold_mem[pipe],
                              resInfo->threshold_reg[pipe],
                              resInfo->threshold_field,
                              &profile_index));

        SOC_IF_ERROR_RETURN(
           _bcm_bst_cmn_io_op(unit, BST_OP_GET,
                              pipe, profile_index, bid,
                              resInfo->profile_mem,
                              resInfo->profile_reg[0],
                              resInfo->profile_field,
                              &cells));
    } else {
        SOC_IF_ERROR_RETURN(
           _bcm_bst_cmn_io_op(unit, BST_OP_GET | BST_OP_THD,
                              pipe, hw_index, bid,
                              resInfo->threshold_mem[pipe],
                              resInfo->threshold_reg[pipe],
                              resInfo->threshold_field,
                              &cells));
    }

    /* Taking granularity into count */
    if (resInfo->threshold_gran != 0) {
        cells = cells * resInfo->threshold_gran - resInfo->threshold_adj;
    } else {
        cells -= resInfo->threshold_adj;
    }

    profile->byte = bst_info->to_byte(unit, cells);

    return BCM_E_NONE;
}

typedef enum _bcm_bst_cmn_op_e {
    _BcmBstProfileGet = 0,
    _BcmBstProfileSet
} _bcm_bst_cmn_op_t;


STATIC int
_bcm_bst_cmn_profile_op(int unit, _bcm_bst_cmn_op_t op,
                        bcm_gport_t gport, bcm_cos_queue_t cosq, int xpe,
                        bcm_bst_stat_id_t bid, bcm_cosq_bst_profile_t *profile)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL; 
    int bcm_rv, ii, rcb = 0, pipe;
    int start_hw_index = 0, end_hw_index = 0;
    _bcm_bst_resource_info_t *resInfo;

    /* Parameter check */
    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

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

        if (op == _BcmBstProfileGet) {
            end_hw_index = start_hw_index;
        }

        for (ii = start_hw_index; ii <= end_hw_index; ii++) {
            if (op == _BcmBstProfileGet) {
                SOC_IF_ERROR_RETURN(
                    _bcm_bst_cmn_profile_get_hw(unit, pipe, xpe, ii, bid, profile));
            } else {
                SOC_IF_ERROR_RETURN(
                    _bcm_bst_cmn_profile_set_hw(unit, pipe, xpe, ii, bid, profile, NULL));
            }
        }

        if (op == _BcmBstProfileGet) {
            return BCM_E_NONE;
        }

    } while (rv == _BCM_BST_RV_RETRY);

    return BCM_E_NONE;
}

int
_bcm_bst_cmn_profile_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         int xpe, bcm_bst_stat_id_t bid, 
                         bcm_cosq_bst_profile_t *profile)
{
    BCM_IF_ERROR_RETURN(
        _bcm_bst_cmn_profile_op(unit, _BcmBstProfileSet, gport, cosq, xpe, 
                                bid, profile));
    return BCM_E_NONE;
}

int
_bcm_bst_cmn_profile_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         int xpe, bcm_bst_stat_id_t bid, 
                         bcm_cosq_bst_profile_t *profile)
{
    BCM_IF_ERROR_RETURN(
        _bcm_bst_cmn_profile_op(unit, _BcmBstProfileGet, gport, cosq, xpe, 
                                bid, profile));

    return BCM_E_NONE;
}

int
_bcm_bst_cmn_stat_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                      bcm_cosq_buffer_id_t buffer_id, bcm_bst_stat_id_t bid,
                      uint32 options, uint64 *pvalue)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL; 
    int bcm_rv, rcb = 0, ii, inst, entries_per_inst;
    int start_hw_index = 0, end_hw_index = 0, pipe_offset = 0, pipe;

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

    do {
        rv = bst_info->handlers.resolve_index(unit, gport, cosq, bid, &pipe, 
                    &start_hw_index, &end_hw_index, &rcb, &cb_data, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }

        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
            return BCM_E_PARAM;
        }

        if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
            pipe = 0;
        }

        if (resInfo->num_instance > 1) {
            uint32 mmu_inst_map = 0;
            if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
                /* Dont use Pipe info, if the resource is Not Pipe specific */
                pipe = 0;
            }
            pipe_offset = pipe * (((resInfo->index_max + 1) / NUM_PIPE(unit)) /
                                  resInfo->num_instance);
            entries_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
            if (bst_info->port_to_mmu_inst_map) {
                (void)bst_info->port_to_mmu_inst_map(unit, bid, gport,
                                                     &mmu_inst_map);
            }
            /* When a resource has more than one instance, find the max value
             * of all applicable instances and return the Max value.
             */
            for (ii = start_hw_index; ii <= end_hw_index; ii++) {
                int temp_stat = 0, max_stat = 0;
                if (buffer_id < 0 || buffer_id >= NUM_XPE(unit)) {
                    for (inst = 0; inst < resInfo->num_instance; inst++) {
                        if (mmu_inst_map & (1 << inst)) {
                            temp_stat = resInfo->p_stat[pipe_offset + ii +
                                                        (inst * entries_per_inst)];
                            if (temp_stat > max_stat) {
                                max_stat = temp_stat;
                            }
                        }
                    }
                } else {
                    max_stat = resInfo->p_stat[pipe_offset + ii +
                                               (buffer_id * entries_per_inst)];
                }
                COMPILER_64_ADD_32(*pvalue, max_stat);
            }
        } else {
            if ((resInfo->stat_mem[pipe] != INVALIDm) || 
                (resInfo->stat_reg[pipe] != INVALIDr)) {
                pipe_offset = ((resInfo->index_max + 1) / NUM_PIPE(unit)) * pipe;
            }

            for (ii = start_hw_index; ii <= end_hw_index; ii++) {
                COMPILER_64_ADD_32(*pvalue, resInfo->p_stat[ii + pipe_offset]);
            }
        }
    } while (rv == _BCM_BST_RV_RETRY);

    bcm_rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
    if (options & BCM_COSQ_STAT_CLEAR) {
        int need_hw_stat_clear = 1;
        sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
        if ((bst_info->track_mode == 1) &&
            (bst_info->hw_cor_in_max_use_count == 1)) {
            /* no need hw stat clear */
            need_hw_stat_clear = 0;
        }
        if (bst_info->hw_stat_clear && (need_hw_stat_clear == 1)) {
            (void)bst_info->hw_stat_clear(unit, resInfo, bid,
                                          gport, start_hw_index);
        }
        bcm_rv = _bcm_bst_sw_stat_clear(unit, gport, cosq, bid);
        sal_mutex_give(bst_info->bst_reslock);
    }

    if (_bcm_bst_check_intr_status(unit, bid)) {
        if (bst_info->intr_enable_set) {
            bst_info->intr_enable_set(unit, 1);
        }
        _bst_intr_status_reset(unit);
    }
#endif

    return bcm_rv;
}

int _bcm_bst_cmn_stat_multi_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
            uint32 options, int max_values, bcm_bst_stat_id_t *id_list, uint64 *values)
{
    int i;

    for (i = 0; i < max_values; i++) {
        BCM_IF_ERROR_RETURN(_bcm_bst_cmn_stat_get(unit, gport, cosq, -1, id_list[i], 
                            options, &values[i]));
    }
    
    return BCM_E_NONE;
}
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT)
int _bcm_bst_cmn_stat_clear(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, 
                            bcm_bst_stat_id_t bid)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int bcm_rv = BCM_E_NONE;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL;
    int rcb = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe;

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }
   
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->handlers.resolve_index == NULL) {
        return BCM_E_UNAVAIL;
    }    

    /* sync all the stats */
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
    if (bst_info->hw_stat_clear) {
        if (gport == -1) {
            /* Clear all stat counters */
            (void)bst_info->hw_stat_clear(unit, resInfo, bid, -1, -1);
        } else {
            /* Clear stat counter for given port, cos */
            rv = bst_info->handlers.resolve_index(unit, gport, cosq, bid, &pipe,
                                &start_hw_index, &end_hw_index, &rcb,
                                &cb_data, &bcm_rv);
            if (rv != _BCM_BST_RV_ERROR) {
                (void)bst_info->hw_stat_clear(unit, resInfo, bid, gport,
                                              start_hw_index);
            }
        }
    }

    if (gport == -1) {
        bcm_port_t local_port;
        PBMP_PORT_ITER(unit, local_port) {
            bcm_rv = _bcm_bst_sw_stat_clear(unit, local_port, cosq, bid);
            if (BCM_FAILURE(bcm_rv)) {
                break;
            }
        }
    } else {
        bcm_rv = _bcm_bst_sw_stat_clear(unit, gport, cosq, bid);
    }
    sal_mutex_give(bst_info->bst_reslock);

    return bcm_rv;

}
#endif
int
_bcm_bst_cmn_stat_sync(int unit, bcm_bst_stat_id_t bid)
{
    int rv = BCM_E_NONE;
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;

    if (((bid < 0) && (bid != bcmBstStatIdInvalid)) ||
        (bid >= bcmBstStatIdMaxCount)) {
        return BCM_E_PARAM;
    }

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_MEMORY;
    }
    if (bid == bcmBstStatIdInvalid) {
        if (bst_info->hw_sbusdma_desc_sync) {
            rv = bst_info->hw_sbusdma_desc_sync(unit);
        } else {
            return BCM_E_PARAM;
        }
    } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (bid == bcmBstStatIdUcast || bid == bcmBstStatIdMcast) {
                bid = bcmBstStatIdQueueTotal;
            }
        }
#endif
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
            return BCM_E_PARAM;
        }

        sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
    if (bst_info->hw_stat_snapshot) {
        rv = bst_info->hw_stat_snapshot(unit, bid, -1, -1);
    }
        sal_mutex_give(bst_info->bst_reslock);
    }

    return rv;
}

int _bcm_bst_cmn_control_set(int unit, bcm_switch_control_t type, int arg)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
   
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->control_set == NULL) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (bst_info->control_set(unit, type, arg, _bcmBstThreadOperExternal));

    return BCM_E_NONE;
}

int _bcm_bst_cmn_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
   
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->control_get == NULL) {
        return BCM_E_UNAVAIL;
    }

    return bst_info->control_get(unit, type, arg);
}

#if defined(BCM_TRIUMPH3_SUPPORT)

STATIC int
_bcm_bst_hw_stat_sync(int unit, _bcm_bst_resource_info_t *resInfo, 
                    bcm_bst_stat_id_t bid, int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv, pipe;
    uint32 rval;
    soc_mem_t mem;
    soc_reg_t reg;
    int min_hw_idx, max_hw_idx, idx;
    int pipe_factor = 1, pipe_offset = 0;
    int num_entries, mem_wsz;
    void *pentry;
    char *dmabuf;
    int sync_val = 0;    

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }
   
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->pre_sync) {
        bst_info->pre_sync(unit, bid, &sync_val);
    }

    /* sync all the stats */
    if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
        pipe_factor = NUM_PIPE(unit);
    }
    if (index == -1) {
        min_hw_idx = resInfo->index_min;
        max_hw_idx = ((resInfo->index_max + 1) / pipe_factor) - 1;
    } else {
        min_hw_idx = max_hw_idx = index;
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if ((mem = resInfo->stat_mem[pipe]) != INVALIDm) {
            num_entries = soc_mem_index_count(unit, mem);
            mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

            dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
            if (dmabuf == NULL) {
                return BCM_E_MEMORY;
            }

            if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                    soc_mem_index_min(unit, mem), 
                                    soc_mem_index_max(unit, mem), 
                                    dmabuf)) {
                soc_cm_sfree(unit, dmabuf);
                return BCM_E_INTERNAL;
            }

            for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                      dmabuf, idx);
                resInfo->p_stat[idx + pipe_offset] = 
                    soc_mem_field32_get(unit, mem, pentry, resInfo->stat_field);
            }

            soc_cm_sfree(unit, dmabuf);
        } else if (resInfo->profile_r[0]) {
            pipe_offset += max_hw_idx;
        } else if ((reg = resInfo->stat_reg[pipe]) != INVALIDr) {
            for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                rv = soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval);
                if (rv) {
                    return BCM_E_INTERNAL;
                }
                resInfo->p_stat[idx] = soc_reg_field_get(unit, reg, 
                                              rval, resInfo->stat_field);
            }
        } /* if valid reg */
    } /* for each pipe */
    
    if (bst_info->post_sync) {
        bst_info->post_sync(unit, bid, sync_val);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_bst_sw_stat_clear(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_bst_stat_id_t bid)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cb_ret_t rv;
    void *cb_data = NULL;
    int bcm_rv, rcb = 0, index;
    int start_hw_index = 0, end_hw_index = 0, pipe_offset = 0, pipe;

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

        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
            pipe = 0;
        }

        if (resInfo->num_instance > 1) {
            uint32 mmu_inst_map = 0;
            int entries_per_inst, inst_offset, inst;
            if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PIPED)) {
                /* Dont use Pipe info, if the resource is Not Pipe specific */
                pipe = 0;
            }
            pipe_offset = pipe * (((resInfo->index_max + 1) / NUM_PIPE(unit)) /
                                  resInfo->num_instance);
            entries_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
            if (bst_info->port_to_mmu_inst_map) {
                (void)bst_info->port_to_mmu_inst_map(unit, bid, gport,
                                                     &mmu_inst_map);
            }
            /* When a resource has more than one instance, clear all
             * applicable instances.
             */
            for (index = start_hw_index; index <= end_hw_index; index++) {
                for (inst = 0; inst < resInfo->num_instance; inst++) {
                    if (mmu_inst_map & (1 << inst)) {
                        inst_offset = inst * entries_per_inst;
                        resInfo->p_stat[pipe_offset + index + inst_offset] = 0;
                    }
                }
            }
        } else {
            if ((resInfo->stat_mem[pipe] != INVALIDm) || 
                (resInfo->stat_reg[pipe] != INVALIDr)) {
                pipe_offset = ((resInfo->index_max + 1) / NUM_PIPE(unit)) * pipe;
            }

            for (index = start_hw_index; index <= end_hw_index; index++) {
                resInfo->p_stat[index + pipe_offset] = 0;
            }
        }
    } while (rv == _BCM_BST_RV_RETRY);

    return BCM_E_NONE;
}

int
_bcm_bst_hw_stat_clear(int unit, _bcm_bst_resource_info_t *resInfo, 
                    bcm_bst_stat_id_t bid, int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv, pipe;
    uint32 rval;
    soc_mem_t mem;
    soc_reg_t reg;
    int min_hw_idx, max_hw_idx, idx;
    int pipe_factor = 1, pipe_offset = 0;
    int num_entries, mem_wsz;
    void *pentry;
    char *dmabuf;
    int enable = 0;
    int sync_val = 0;    

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }
    
    /* The bcmBstStatIdIngPool stat register THDI_BST_SP_GLOBAL_SHARED_CNT 
       is a readonly register */
    if (SOC_IS_TRIDENT2(unit)) {
    if (bid == bcmBstStatIdIngPool && 
        resInfo->stat_reg[0] == THDI_BST_SP_GLOBAL_SHARED_CNTr) {
        return BCM_E_PARAM;
    }
    }
   
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->pre_sync) {
        bst_info->pre_sync(unit, bid, &sync_val);
    }

    /* sync all the stats */
    if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
        pipe_factor = NUM_PIPE(unit);
    }
    if (index == -1) {
        min_hw_idx = resInfo->index_min;
        max_hw_idx = ((resInfo->index_max + 1) / pipe_factor) - 1;
    } else {
        min_hw_idx = max_hw_idx = index;
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if ((mem = resInfo->stat_mem[pipe]) != INVALIDm) {
            if (index == -1) {
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, mem, COPYNO_ALL, TRUE));
                continue;
            }

            num_entries = soc_mem_index_count(unit, mem);
            mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);            

            dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
            if (dmabuf == NULL) {
                return BCM_E_MEMORY;
            }

            if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                    soc_mem_index_min(unit, mem), 
                                    soc_mem_index_max(unit, mem), 
                                    dmabuf)) {
                soc_cm_sfree(unit, dmabuf);
                return BCM_E_INTERNAL;
            }

            /* Clear stat counter if requested */
            for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                      dmabuf, idx); 
                soc_mem_field32_set(unit, mem, pentry, resInfo->stat_field, 0);
            }
            
            if (soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                                    soc_mem_index_min(unit, mem), 
                                    soc_mem_index_max(unit, mem), 
                                    dmabuf)) {
                soc_cm_sfree(unit, dmabuf);
                return BCM_E_INTERNAL;
            } 
            
            soc_cm_sfree(unit, dmabuf);
        } else if (resInfo->profile_r[0]) {
            pipe_offset += max_hw_idx;
        } else if ((reg = resInfo->stat_reg[pipe]) != INVALIDr) {
            for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                rv = soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval);
                if (rv) {
                    return BCM_E_INTERNAL;
                }
            }

            /* Clear stat counters */
            if (bst_info->control_get) {
                bst_info->control_get(unit, bcmSwitchBstEnable, &enable);
            }
            if (enable && bst_info->control_set) {
                bst_info->control_set(unit, bcmSwitchBstEnable, 0,
                                      _bcmBstThreadOperInternal);
            }                

            for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                rv = soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval);
                if (rv) {
                    return BCM_E_INTERNAL;
                }                
                soc_reg_field_set(unit, reg, &rval, resInfo->stat_field, 0);
                BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
            }

            if (enable && bst_info->control_set) {
                bst_info->control_set(unit, bcmSwitchBstEnable, enable,
                                      _bcmBstThreadOperInternal);
            }

        } /* if valid reg */
    } /* for each pipe */
    
    if (bst_info->post_sync) {
        bst_info->post_sync(unit, bid, sync_val);
    }

    return BCM_E_NONE;
}

int
_bcm_bst_sync_hw_trigger(int unit, bcm_bst_stat_id_t bid, int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    bcm_gport_t gport;
    bcm_cos_t cosq;

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }
    
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    
    /* sync all the stats */
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if (resInfo == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_bst_hw_stat_sync(unit, resInfo, bid, port, index));

    if (bst_info->handlers.reverse_resolve_index) {
        /* reverse map the inedx to port/cos pair */
        bst_info->handlers.reverse_resolve_index(unit, bid, port, index, &gport, &cosq);
    
        soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER, 
                           bid, gport, cosq);
    }

    return BCM_E_NONE;
}

int
_bcm_bst_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid, int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int num_entries, mem_wsz;
    int idx, idx_offset = 0, rv, pipe, pipe_offset = 0;
    void *pentry;
    char *dmabuf;
    uint32 rval, temp_val;
    soc_mem_t mem;
    soc_reg_t reg;
    int sync_val = 0;

    if (bid < _BCM_BST_RESOURCE_MIN || bid >= _BCM_BST_RESOURCE_MAX) {
        return BCM_E_PARAM;
    }
   
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if (bst_info->pre_sync) {
        bst_info->pre_sync(unit, bid, &sync_val);
    }

    /* sync all the stats */
    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if (resInfo == NULL) {
        return BCM_E_PARAM;
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        bcm_gport_t gport = -1;
        bcm_cos_t cosq = -1;
        mem = resInfo->stat_mem[pipe];
        reg = resInfo->stat_reg[pipe];
        pipe_offset = 0;

        if (pipe) {
            pipe_offset = ((resInfo->index_max + 1) / NUM_PIPE(unit)) * pipe;
        }
        if (mem != INVALIDm) {
            num_entries = soc_mem_index_count(unit, mem);
            mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

            dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
            if (dmabuf == NULL) {
                return BCM_E_MEMORY;
            }

            if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                    soc_mem_index_min(unit, mem), 
                                    soc_mem_index_max(unit, mem), 
                                    dmabuf)) {
                soc_cm_sfree(unit, dmabuf);
                return BCM_E_INTERNAL;
            }

            for (idx = 0; idx < (resInfo->num_stat_pp / NUM_PIPE(unit)); idx++) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
                if (bid == bcmBstStatIdEgrPortPoolSharedMcast) {
                    /*
                     * Reorder sw stat index
                     * HW stat: index 0 - port 0 pool 0
                     *          index 1 - port 1 pool 0
                     *          index 2 - port 2 pool 0
                     *          ...
                     *          index n - port n%34 pool n/34
                     * SW stat: index 0 - port 0 pool 0
                     *          index 1 - port 0 pool 1
                     *          index 2 - port 0 pool 2
                     *          ...
                     *          index n - port n/4 pool n%4
                     */
                    int port_tmp = 0, pool = 0;
#if defined(BCM_MONTEREY_SUPPORT)
                    if (SOC_IS_MONTEREY(unit)) {
                        port_tmp = idx % SOC_MONTEREY_NUM_MMU_PORTS_PER_PIPE;
                        pool = idx / SOC_MONTEREY_NUM_MMU_PORTS_PER_PIPE;
                    } else
#endif
#if defined(BCM_APACHE_SUPPORT)
                    if (SOC_IS_APACHE(unit)) {
                        port_tmp = idx % (SOC_APACHE_NUM_MMU_PORTS_PER_PIPE - 1);
                        pool = idx / (SOC_APACHE_NUM_MMU_PORTS_PER_PIPE - 1);
                    } else
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
                    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit) ||
                        SOC_IS_TD2P_TT2P(unit)) {
                        port_tmp = idx % _TD2_MMU_PORTS_PER_PIPE;
                        pool = idx / _TD2_MMU_PORTS_PER_PIPE;
                        if (pipe) {
                            /* TD2 uses specific port value to pass pipe info */
                            port = -2;
                        }
                    } else
#endif
                    {
                        soc_cm_sfree(unit, dmabuf);
                        return BCM_E_UNAVAIL;
                    }
                    idx_offset = port_tmp * 4 + pool + pipe_offset;
                } else
#endif /* BCM_TRIDENT2_SUPPORT || BCM_APACHE_SUPPORT */
                {
                    idx_offset = idx + pipe_offset;
                }
                pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                      dmabuf, idx);
                temp_val = soc_mem_field32_get(unit, mem, pentry, resInfo->stat_field);
                if (bst_info->track_mode) {
                    if (resInfo->p_stat[idx_offset] < temp_val) {
                        resInfo->p_stat[idx_offset] = temp_val;
                    }
                } else {
                    resInfo->p_stat[idx_offset] = temp_val;
                }

                if (resInfo->p_threshold[idx_offset] &&
                    (resInfo->p_stat[idx_offset] >= 
                     (resInfo->p_threshold[idx_offset]) * resInfo->threshold_gran)) {
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
            soc_cm_sfree(unit, dmabuf);
        } else if (reg != INVALIDr) {
            for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
                idx_offset = idx + pipe_offset;
                rv = soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval);
                if (rv) {
                    return BCM_E_INTERNAL;
                }
                temp_val = soc_reg_field_get(unit, reg, rval, resInfo->stat_field);
                if (bst_info->track_mode) {
                    if (resInfo->p_stat[idx_offset] < temp_val) {
                        resInfo->p_stat[idx_offset] = temp_val;
                    }
                } else {
                    resInfo->p_stat[idx_offset] = temp_val;
                }

                if (resInfo->p_threshold[idx_offset] &&
                    (resInfo->p_stat[idx_offset] >= 
                     (resInfo->p_threshold[idx_offset]) * resInfo->threshold_gran)) {
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
        }
    }
    
    if (bst_info->post_sync) {
        bst_info->post_sync(unit, bid, sync_val);
    }
    
    return BCM_E_NONE;
}
#endif

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_APACHE_SUPPORT)
STATIC int
_bst_td2_sbusdma_desc_setup(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_tbl_ctrl_t *tbl_ctrl;
    int blk, bid, pipe, mi, pipe_factor;
    uint8 acc_type;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_reg_info_t *regp;
    uint32 size;
    uint8 *buff = NULL;
    _bcm_bst_tbl_desc_t *desc = NULL;

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

        tbl_ctrl = sal_alloc(sizeof(_bcm_bst_tbl_ctrl_t), "bst_tbl_ctrl");
        if (tbl_ctrl == NULL) {
            return BCM_E_MEMORY;
        }
        pipe_factor = 1;
        if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            pipe_factor = NUM_PIPE(unit);
        }
        size = 0;
        tbl_ctrl->tindex = bid;
        tbl_ctrl->entries = pipe_factor;
        tbl_ctrl->buff = buff;
        tbl_ctrl->size = 0;
        bst_tbl_ctrl[unit][bid] = tbl_ctrl;
        for (pipe = 0; pipe < pipe_factor; pipe++) {
            mem = resInfo->stat_mem[pipe];
            reg = resInfo->stat_reg[pipe];
            if (mem != INVALIDm) {
                desc[mi].mem = mem;
                desc[mi].flags = SOC_SBUSDMA_CFG_COUNTER_IS_MEM | SOC_SBUSDMA_CFG_USE_FLAGS;
                desc[mi].blk = SOC_MEM_BLOCK_ANY(unit, mem);
                desc[mi].addr = soc_mem_addr_get(unit, mem,
                                               0, desc[mi].blk, 0, &acc_type);
                desc[mi].acc_type = acc_type;
                desc[mi].blk = SOC_BLOCK2SCH(unit, desc[mi].blk);
                desc[mi].width = soc_mem_entry_words(unit, mem);
                desc[mi].entries = resInfo->num_stat_pp / pipe_factor;
                desc[mi].shift = 0;
                size += desc[mi].entries * desc[mi].width * 4;
                tbl_ctrl->blk = desc[mi].blk;
                mi++;
            }else if (reg != INVALIDr) {
                regp = SOC_REG_PTR(unit, reg);
                desc[mi].reg= reg;
                desc[mi].addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                    SOC_REG_ADDR_OPTION_NONE, &blk,
                    &acc_type);
                desc[mi].flags = 0;
                desc[mi].acc_type = acc_type;
                desc[mi].blk = blk;
                desc[mi].width = BYTES2WORDS(soc_reg_bytes(unit, reg)) * regp->nFields;
                desc[mi].entries = resInfo->num_stat_pp / pipe_factor;
                desc[mi].shift = 8;
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
_bcm_bst_td2_sbusdma_desc_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int pipe, bid, mi, pipe_factor;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_reg_info_t *regp;
    uint32 entries, width, size = 0;
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

        pipe_factor = 1;
        if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            pipe_factor = NUM_PIPE(unit);
        }
        for (pipe = 0; pipe < pipe_factor; pipe++) {
            mem = resInfo->stat_mem[pipe];
            reg = resInfo->stat_reg[pipe];
            if (mem != INVALIDm) {
                entries = resInfo->num_stat_pp / pipe_factor;
                width = soc_mem_entry_words(unit, mem);
                size += entries * width * 4;
                mi++;
            } else if (reg != INVALIDr) {
                regp = SOC_REG_PTR(unit, reg);
                entries = resInfo->num_stat_pp;
                width = BYTES2WORDS(soc_reg_bytes(unit, reg)) * regp->nFields;
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
                 bcmBstStatIdMaxCount, size));
    bst_tbl_handles[unit] = 0;
    if (bst_info->bst_tbl_buf == NULL) {
        bst_info->bst_tbl_buf = soc_cm_salloc(unit, size, "bst_tbl_buf");
        if (bst_info->bst_tbl_buf == NULL) {
            goto error;
        }
    }
    sal_memset(bst_info->bst_tbl_buf, 0, size);
    rv =_bst_td2_sbusdma_desc_setup(unit);
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
_bst_td2_sw_sbusdma_desc_sync(int unit, int bid)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_tbl_ctrl_t *tbl_ctrl;
    int idx, idx_offset, pipe, pipe_offset = 0;
    void *pentry;
    uint32 rval, temp_val;
    uint8 *buff = NULL;
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 width = 0;
    int pipe_factor;
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
    pipe_factor = 1;
    if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
        pipe_factor = NUM_PIPE(unit);
    }
    for (pipe = 0; pipe < pipe_factor; pipe++) {
        bcm_gport_t gport = -1;
        bcm_cos_t cosq = -1;
        mem = resInfo->stat_mem[pipe];
        reg = resInfo->stat_reg[pipe];
        pipe_offset = 0;
        if (pipe) {
            pipe_offset = ((resInfo->index_max + 1) / NUM_PIPE(unit)) * pipe;
        }
        if (mem != INVALIDm) {
            for (idx = 0; idx < (resInfo->num_stat_pp / NUM_PIPE(unit)); idx++) {
                idx_offset = idx + pipe_offset;
                pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                      buff, idx);
                temp_val = soc_mem_field32_get(unit, mem, pentry, resInfo->stat_field);
                if (bst_info->track_mode) {
                    if (resInfo->p_stat[idx_offset] < temp_val) {
                        resInfo->p_stat[idx_offset] = temp_val;
                    }
                } else {
                    resInfo->p_stat[idx_offset] = temp_val;
                }
                if (resInfo->p_threshold[idx_offset] &&
                    (resInfo->p_stat[idx_offset] >=
                     (resInfo->p_threshold[idx_offset]) * resInfo->threshold_gran)) {
                    /* Generate SOC EVENT */
                    if (bst_info->handlers.reverse_resolve_index) {
                        /* reverse map the inedx to port/cos pair */
                        bst_info->handlers.reverse_resolve_index(unit, bid,
                                                                 -1, idx_offset,
                                                                 &gport, &cosq);
                        soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                           bid, gport, cosq);
                    }
                }
            }
            buff += tbl_ctrl->size / pipe_factor;
        } else if (reg != INVALIDr) {
            for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
                width = BYTES2WORDS(soc_reg_bytes(unit, reg));
                idx_offset = idx + pipe_offset;

                sal_memcpy(&rval, buff, width * 4);
                temp_val = soc_reg_field_get(unit, reg, rval, resInfo->stat_field);
                if (bst_info->track_mode) {
                    if (resInfo->p_stat[idx_offset] < temp_val) {
                        resInfo->p_stat[idx_offset] = temp_val;
                    }
                } else {
                    resInfo->p_stat[idx_offset] = temp_val;
                }
                buff = buff + width * 4;

                if (resInfo->p_threshold[idx_offset] &&
                    (resInfo->p_stat[idx_offset] >=
                     (resInfo->p_threshold[idx_offset]) * resInfo->threshold_gran)) {
                    /* Generate SOC EVENT */
                    if (bst_info->handlers.reverse_resolve_index) {
                        /* reverse map the inedx to port/cos pair */
                        bst_info->handlers.reverse_resolve_index(unit, bid,
                                                                 -1, idx_offset,
                                                                 &gport, &cosq);
                        soc_event_generate(unit, SOC_SWITCH_EVENT_MMU_BST_TRIGGER,
                                           bid, gport, cosq);
                    }
                }
            }
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_bst_td2_sbusdma_desc_sync(int unit)
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

        BCM_IF_ERROR_RETURN(_bst_td2_sw_sbusdma_desc_sync(unit, bid));
    }

    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, enable,
                              _bcmBstThreadOperInternal);
    }

    sal_mutex_give(bst_info->bst_reslock);
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_td2_sbusdma_desc_deinit(int unit)
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
#endif

STATIC void
soc_sbusdma_bst_cb(int unit, int status, sbusdma_desc_handle_t handle, void *data)
{
    _bcm_bst_tbl_ctrl_t *tbl_ctrl = bst_tbl_ctrl[unit][PTR_TO_INT(data)];
    if (status != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "bst sbusdma failed: blk: %d, bid: %d, entries: %d, size: %d.\n"),
                   tbl_ctrl->blk, tbl_ctrl->tindex, tbl_ctrl->entries, tbl_ctrl->size));
    }
}

int
soc_bst_sbusdma_desc_setup(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int mi;
    uint8 *buff = NULL;
    soc_sbusdma_desc_ctrl_t ctrl = {0};
    soc_sbusdma_desc_cfg_t *cfg = NULL;
    _bcm_bst_tbl_desc_t *desc = NULL;
    int rv = BCM_E_INIT;
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    buff = bst_info->bst_tbl_buf;
    desc = bst_info->bst_tbl_desc;

    ctrl.flags = SOC_SBUSDMA_CFG_USE_FLAGS;
    ctrl.pc = NULL;
    ctrl.pc_data = NULL;
    ctrl.cfg_count = bst_info->bst_tbl_num;
    ctrl.buff = buff;
    ctrl.cb = soc_sbusdma_bst_cb;
    ctrl.data = INT_TO_PTR(bcmBstStatIdDevice);
#define BST_TBLS "BST TBLS"
    sal_strlcpy(ctrl.name, BST_TBLS, sizeof(ctrl.name));
    cfg = sal_alloc(bst_info->bst_tbl_num * sizeof(soc_sbusdma_desc_cfg_t),
        "bst_sbusdma_desc_cfg");
    if (cfg == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(cfg, 0,  bst_info->bst_tbl_num * sizeof(soc_sbusdma_desc_cfg_t));
    mi = 0;
    for (mi = 0; mi < bst_info->bst_tbl_num; mi++) {
        cfg[mi].addr = desc[mi].addr;
        cfg[mi].flags = desc[mi].flags;
        cfg[mi].acc_type = desc[mi].acc_type;
        cfg[mi].blk = desc[mi].blk;
        cfg[mi].width = desc[mi].width;
        cfg[mi].count = desc[mi].entries;
        cfg[mi].addr_shift = desc[mi].shift;
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
             cfg[mi].flags |= SOC_SBUSDMA_CFG_MOR;
        }
#endif

    }
    rv = soc_sbusdma_desc_create(unit, &ctrl, cfg, TRUE,
                     &bst_tbl_handles[unit]);
    sal_free(cfg);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "bst sbusdma desc creation error!\n")));
        return rv;
    }
    return BCM_E_NONE;
}

int
soc_bst_sbusdma_desc_free(int unit)
{
    int bid;
    _bcm_bst_resource_info_t *resInfo;
    int rv = BCM_E_INIT;
    if (bst_tbl_handles[unit]) {
        do {
            rv = soc_sbusdma_desc_delete(unit, bst_tbl_handles[unit]);
            if (rv == SOC_E_BUSY) {
                sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
            }
        } while (rv == SOC_E_BUSY);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "bst sbusdma desc delete error!\n")));
            return rv;
        }

        bst_tbl_handles[unit] = 0;
    }

    for (bid = _BCM_BST_RESOURCE_MIN; bid < _BCM_BST_RESOURCE_MAX; bid++) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (resInfo == NULL) {
            return BCM_E_PARAM;
        }

        if (!_BCM_BST_RESOURCE_VALID(resInfo)) {
            continue;
        }

        if (bst_tbl_ctrl[unit][bid]) {
            sal_free(bst_tbl_ctrl[unit][bid]);
            bst_tbl_ctrl[unit][bid] = NULL;
        }
    }
    if (bst_tbl_ctrl[unit]) {
        sal_free(bst_tbl_ctrl[unit]);
        bst_tbl_ctrl[unit] = NULL;
    }

    return BCM_E_NONE;
}
int
soc_bst_hw_sbusdma_desc_sync(int unit)
{
    sbusdma_desc_handle_t desc_handle;
    soc_timeout_t to;
    sal_usecs_t dma_timeout = 1000000;
    sal_usecs_t stime, etime;
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv = BCM_E_INIT;
    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }
    if (!bst_tbl_ctrl[unit]) {
        return BCM_E_INIT;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "bst start sync.\n")));
    desc_handle = bst_tbl_handles[unit];
    if (bst_info->bst_tbl_buf && desc_handle) {
        soc_timeout_init(&to, 2 * dma_timeout, 0);
        do {
            stime = sal_time_usecs();
            rv = soc_sbusdma_desc_start(unit, desc_handle);
            if (rv == SOC_E_NONE) {
                break;
            } else if (rv == SOC_E_BUSY) {
                sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
            } else {
                sal_mutex_give(bst_info->bst_reslock);
                return SOC_E_INTERNAL;
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "bst start dma timeout\n")));
                sal_mutex_give(bst_info->bst_reslock);
                return SOC_E_TIMEOUT;
            }
        } while (rv == SOC_E_BUSY);
        etime = sal_time_usecs();
        LOG_INFO(BSL_LS_SOC_MMU,
                 (BSL_META_U(unit,
                             "soc_bst_hw_sbusdma_desc_sync: unit=%d bst sync done in %d usec\n"),
                  unit,
                  SAL_USECS_SUB(etime, stime)));
    }
    return BCM_E_NONE;
}

