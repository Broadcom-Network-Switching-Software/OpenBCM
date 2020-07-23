/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <soc/tomahawk.h>

#include <bcm/switch.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm_int/bst.h>
#include <bcm_int/esw/port.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
#define TH_BST_THRESHOLD_MAX_HIGH  0x7fff
#ifdef BCM_TOMAHAWK2_SUPPORT
#define TH2_BST_THRESHOLD_MAX_HIGH 0xffff
#endif

extern int _bcm_bst_th_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                                        int port, int index);
extern int _bcm_bst_th_hw_stat_clear(int unit,
                                     _bcm_bst_resource_info_t *resInfo,
                                     bcm_bst_stat_id_t bid, int port,
                                     int index);
static int _bst_th_cmn_profile_set_hw(int unit, int pipe, int xpe, int hw_index,
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile, 
                            uint32 *p_profile);
extern int _bcm_bst_cmn_profile_delete_hw(int unit, bcm_bst_stat_id_t bid, 
                               int id, int profile_index);
STATIC int _bcm_bst_th_sbusdma_desc_sync(int unit);
STATIC int _bcm_bst_th_sbusdma_desc_init(int unit);
STATIC int _bcm_bst_th_sbusdma_desc_deinit(int unit);

STATIC int
_bcm_bst_th_byte_to_cell(int unit, uint32 bytes)
{
    return (bytes + (208 - 1))/208;
}

STATIC int
_bcm_bst_th_cell_to_byte(int unit, uint32 cells)
{
    return cells * 208;
}

STATIC int
_bcm_th_bst_control_set(int unit, bcm_switch_control_t type, int arg,
                        _bcm_bst_thread_oper_t operation)
{
    uint32 rval, rval_en, rval_action, i, fval, enable = 0;
    uint32 hdrm_en = 0;
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *pres = NULL;
    soc_reg_t stat_reg = INVALIDr;
    soc_field_t stat_field = INVALIDf;
    soc_field_t snap_en[] = {BST_HW_SNAPSHOT_EN_THDOf,
                             BST_HW_SNAPSHOT_EN_THDIf,
                             BST_HW_SNAPSHOT_EN_CFAPf
                            };
    soc_field_t snap_action[] = {ACTION_EN_THDOf,
                                 ACTION_EN_THDIf,
                                 ACTION_EN_CFAPf
                                };

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (!bst_info) {
        return BCM_E_INIT;
    }

    switch (type) {
    case bcmSwitchBstEnable:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_BST_TRACKING_ENABLEr(unit, &rval));

        fval = arg ? 0xf : 0;

        soc_reg_field_set(unit, MMU_GCFG_BST_TRACKING_ENABLEr, &rval,
                          BST_TRACK_EN_THDOf, fval);
        soc_reg_field_set(unit, MMU_GCFG_BST_TRACKING_ENABLEr, &rval,
                          BST_TRACK_EN_THDIf, fval);
        soc_reg_field_set(unit, MMU_GCFG_BST_TRACKING_ENABLEr, &rval,
                          BST_TRACK_EN_CFAPf, fval);

        BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_BST_TRACKING_ENABLEr(unit, rval));

        /* Globally Enable/Disable Headroom Pool Monitoring for all
           headroom pools */
        hdrm_en = arg ? 0xf : 0;
        for (i = 0; i < NUM_LAYER(unit); i++) {
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, i, 0, hdrm_en));
        }

        if (operation == _bcmBstThreadOperExternal) {
            if (fval) {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_start(unit));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_bst_sync_thread_stop(unit));
            }
        } else {
            /* Set Enable Flag to True/False, to Run/Pause the Thread
             * respectively. Wont Kill the thread.
             */
            _BCM_BST_SYNC_THREAD_ENABLE_SET(unit, fval ? TRUE : FALSE);
        }

        break;
    case bcmSwitchBstTrackingMode:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, BST_TRACKING_MODEf, arg);
        BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
        bst_info->track_mode = arg ? 1 : 0;
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
        pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
        if (pres != NULL) {
            _BCM_BST_STAT_INFO(pres, pres->stat_mem[0], stat_reg,
                               stat_field);
        }
        break;
    case bcmSwitchBstSnapshotEnable:
        rval_en = 0;
        rval_action = 0;
        if (arg != 0) {
            BCM_IF_ERROR_RETURN(
                READ_MMU_GCFG_BST_HW_SNAPSHOT_ENr(unit, &rval_en));
            BCM_IF_ERROR_RETURN(
                READ_MMU_GCFG_BST_SNAPSHOT_ACTION_ENr(unit, &rval_action));

            for (i = _BCM_BST_SNAPSHOT_THDO; i < _BCM_BST_SNAPSHOT_COUNT; i++) {
                enable = 0;
                if (arg & (0x1 << i)) {
                    enable = 0xf;
                } else {
                    enable = 0;
                }

                soc_reg_field_set(unit, MMU_GCFG_BST_HW_SNAPSHOT_ENr, &rval_en,
                                  snap_en[i], enable);

                soc_reg_field_set(unit, MMU_GCFG_BST_SNAPSHOT_ACTION_ENr,
                                  &rval_action, snap_action[i], ((enable) ? 1 : 0));
            }
        }
        BCM_IF_ERROR_RETURN(
            WRITE_MMU_GCFG_BST_HW_SNAPSHOT_ENr(unit, rval_en));
        BCM_IF_ERROR_RETURN(
            WRITE_MMU_GCFG_BST_SNAPSHOT_ACTION_ENr(unit, rval_action));
        bst_info->snapshot_mode = arg;

        break;

    default:
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th_bst_control_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32 rval;
    _bcm_bst_cmn_unit_info_t *bst_info;
    int hdrm_en, cfap_en;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    if (!bst_info) {
        return BCM_E_INIT;
    }

    switch (type) {
    case bcmSwitchBstEnable:
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_BST_TRACKING_ENABLEr(unit, &rval));
        cfap_en = soc_reg_field_get(unit, MMU_GCFG_BST_TRACKING_ENABLEr, rval,
                                    BST_TRACK_EN_CFAPf);
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, 0, 0, &rval));
        hdrm_en = soc_reg_field_get(unit, THDI_HDRM_POOL_CFGr, rval,
                                    PEAK_COUNT_UPDATE_EN_HPf);

        if ((cfap_en == 0xf) &&
            (hdrm_en == 0xf)) {
            /* Only when both cfap and
               headroom tracking is enabled */
            *arg = 1;
        } else {
            *arg = 0;
        }
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
_bcm_bst_th_intr_enable_set(int unit, int enable)
{
    uint32 rval;
    if (!soc_property_get(unit, spn_POLLED_IRQ_MODE, 0)) {
        if (SOC_REG_IS_VALID(unit, MMU_XCFG_XPE_CPU_INT_ENr)) {
            BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_ENr(unit, &rval));
            soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_ENr, &rval,
                              BST_THDO_INT_ENf, enable);
            soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_ENr, &rval,
                              BST_THDI_INT_ENf, enable);
            BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_ENr(unit, rval));
        }

        if (SOC_IS_TOMAHAWK2(unit)) {
            if (SOC_REG_IS_VALID(unit, MMU_SEDCFG_SED_CPU_INT_ENr)) {
                BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_SED_CPU_INT_ENr(unit, &rval));
                soc_reg_field_set(unit, MMU_SEDCFG_SED_CPU_INT_ENr, &rval,
                                  BST_CFAP_B_INT_ENf, enable);
                soc_reg_field_set(unit, MMU_SEDCFG_SED_CPU_INT_ENr, &rval,
                                  BST_CFAP_A_INT_ENf, enable);
                BCM_IF_ERROR_RETURN(WRITE_MMU_SEDCFG_SED_CPU_INT_ENr(unit, rval));
            }
        } else {
            if (SOC_REG_IS_VALID(unit, MMU_SCFG_SC_CPU_INT_ENr)) {
                BCM_IF_ERROR_RETURN(READ_MMU_SCFG_SC_CPU_INT_ENr(unit, &rval));
                soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_ENr, &rval,
                                  BST_CFAP_B_INT_ENf, enable);
                soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_ENr, &rval,
                                  BST_CFAP_A_INT_ENf, enable);
                BCM_IF_ERROR_RETURN(WRITE_MMU_SCFG_SC_CPU_INT_ENr(unit, rval));
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
soc_th_set_hw_intr_cb(int unit)
{
    BCM_IF_ERROR_RETURN(soc_th_set_bst_callback(unit, &_bcm_bst_hw_event_cb));
    return BCM_E_NONE;
}

STATIC int
_bst_th_intr_status_reset(int unit)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, &rval));
    soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_CLEARr, &rval,
                      BST_THDO_INT_CLRf, 0);
    soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_CLEARr, &rval,
                      BST_THDI_INT_CLRf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, rval));

    if (SOC_IS_TOMAHAWK2(unit)) {
        if (SOC_REG_IS_VALID(unit, MMU_SEDCFG_SED_CPU_INT_CLEARr)) {
            BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_SED_CPU_INT_CLEARr(unit, &rval));
            soc_reg_field_set(unit, MMU_SEDCFG_SED_CPU_INT_CLEARr, &rval,
                              BST_CFAP_A_INT_CLRf, 0);
            soc_reg_field_set(unit, MMU_SEDCFG_SED_CPU_INT_CLEARr, &rval,
                              BST_CFAP_B_INT_CLRf, 0);
            BCM_IF_ERROR_RETURN(WRITE_MMU_SEDCFG_SED_CPU_INT_CLEARr(unit, rval));
        }
    } else {
        if (SOC_REG_IS_VALID(unit, MMU_SCFG_SC_CPU_INT_CLEARr)) {
            BCM_IF_ERROR_RETURN(READ_MMU_SCFG_SC_CPU_INT_CLEARr(unit, &rval));
            soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_CLEARr, &rval,
                              BST_CFAP_A_INT_CLRf, 0);
            soc_reg_field_set(unit, MMU_SCFG_SC_CPU_INT_CLEARr, &rval,
                              BST_CFAP_B_INT_CLRf, 0);
            BCM_IF_ERROR_RETURN(WRITE_MMU_SCFG_SC_CPU_INT_CLEARr(unit, rval));
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th_intr_to_resources(int unit, uint32 *flags)
{
    uint32 flags_tr = 0;
    uint32 fval = 0;
    uint32 rval_dev, rval32_type, rval32_status;
    uint64 rval_uc, rval_db, rval_rqe;
    int index[_bcmResourceMaxCount] = {-1}, i;
    int res[_bcmResourceMaxCount] = {-1}, res_ct = 0;
    int bid_detect[_bcmResourceMaxCount] = {0};
    soc_field_t fld = INVALIDf;
    soc_reg_t reg_type = INVALIDr, reg_status = INVALIDr;
    int xpe = 0;
    uint32 inst,base_index,ipipe_map;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    _bcm_bst_th_intr_enable_set(unit, 0);
    _bst_th_intr_status_reset(unit);

    LOG_VERBOSE(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d flags=0x%x\n"),
                 unit, *flags));

    /* _bcmResourceDevice */
    BCM_IF_ERROR_RETURN(READ_CFAPBSTTHRSr(unit, &rval_dev));
    if (soc_reg_field_get(unit, CFAPBSTTHRSr, rval_dev, BST_THRESHOLDf)) {
        if (SOC_IS_TOMAHAWK2(unit)) {
            if (SOC_REG_IS_VALID(unit, MMU_SEDCFG_SED_CPU_INT_STATr)) {
                /* For CFAP, it use MMU_SCFG_SC_CPU_INT_STATr to indicate the bst
                 * interruptbut not use a trigger status register.
                 * MMU_SCFG_SC_CPU_INT_STATr would be cleared in SER handler, so
                 * we check _bcmResourceDevice every time here.
                 */
                index[res_ct] = 0;
                res[res_ct++] = _bcmResourceDevice;
            }
        } else {
            if (SOC_REG_IS_VALID(unit, MMU_SCFG_SC_CPU_INT_ENr)) {
                /* For CFAP, it use MMU_SCFG_SC_CPU_INT_STATr to indicate the bst
                 * interruptbut not use a trigger status register.
                 * MMU_SCFG_SC_CPU_INT_STATr would be cleared in SER handler, so
                 * we check _bcmResourceDevice every time here.
                 */
                index[res_ct] = 0;
                res[res_ct++] = _bcmResourceDevice;
            }
        }
    }

    /* THDI Status/Triggers */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        reg_type = SOC_REG_UNIQUE_ACC(unit, THDI_BST_TRIGGER_STATUS_TYPEr)[xpe];
        reg_status = SOC_REG_UNIQUE_ACC(unit, THDI_BST_TRIGGER_STATUS_32r)[xpe];
        ipipe_map = si->xpe_ipipe_map[xpe];

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if(!(ipipe_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg_type, inst, 0, &rval32_type));
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg_status, inst, 0, &rval32_status));

            /* _bcmResourceIngPool */
            fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                     POOL_SHARED_TRIGGERf);
            if (fval && !bid_detect[_bcmResourceIngPool]) {
                fld = POOL_SHARED_TRIGGER_STATUSf;
                index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
                res[res_ct++] = _bcmResourceIngPool;
                bid_detect[_bcmResourceIngPool] = 1;
            }

            /* _bcmResourcePortPool */
            fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                     SP_SHARED_TRIGGERf);
            if (fval && !bid_detect[_bcmResourcePortPool]) {
                fld = SP_SHARED_TRIGGER_STATUSf;
                index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
                res[res_ct++] = _bcmResourcePortPool;
                bid_detect[_bcmResourcePortPool] = 1;
            }

            /* _bcmResourcePriGroupShared */
            fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                     PG_SHARED_TRIGGERf);
            if (fval && !bid_detect[_bcmResourcePriGroupShared]) {
                fld = PG_SHARED_TRIGGER_STATUSf;
                index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
                res[res_ct++] = _bcmResourcePriGroupShared;
                bid_detect[_bcmResourcePriGroupShared] = 1;
            }

            /* _bcmResourcePriGroupHeadroom */
            fval = soc_reg_field_get(unit, THDI_BST_TRIGGER_STATUS_TYPEr, rval32_type,
                                     PG_HDRM_TRIGGERf);

            if (fval && !bid_detect[_bcmResourcePriGroupHeadroom]) {
                fld = PG_HDRM_TRIGGER_STATUSf;
                index[res_ct] = soc_reg_field_get(unit, reg_status, rval32_status, fld);
                res[res_ct++] = _bcmResourcePriGroupHeadroom;
                bid_detect[_bcmResourcePriGroupHeadroom] = 1;
            }

            rval32_type = 0;
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_type, inst, 0, rval32_type));
            rval32_status = 0;
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg_status, inst, 0, rval32_status));
        }
    }


    /* THDM DB Status/Triggers */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        BCM_IF_ERROR_RETURN
            (soc_reg_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, xpe, 0, &rval_db));
        reg_status = MMU_THDM_DB_DEVICE_BST_STATr;

        /* _bcmResourceEgrPool */
        fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, rval_db,
                                     MCUC_SP_BST_STAT_TRIGGEREDf);

        if (fval && !bid_detect[_bcmResourceEgrPool]) {
            fld = MCUC_SP_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_db, fld);
            res[res_ct++] = _bcmResourceEgrPool;
            bid_detect[_bcmResourceEgrPool] = 1;
        }

        /* _bcmResourceEgrMCastPool */
        fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, rval_db,
                                     MC_SP_BST_STAT_TRIGGEREDf);

        if (fval && !bid_detect[_bcmResourceEgrMCastPool]) {
            fld = MC_SP_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_db, fld);
            res[res_ct++] = _bcmResourceEgrMCastPool;
            bid_detect[_bcmResourceEgrMCastPool] = 1;
        }

        /* _bcmResourceMcast */
        fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, rval_db,
                                     MC_Q_BST_STAT_TRIGGEREDf);

        if (fval && !bid_detect[_bcmResourceMcast]) {
            fld = MC_Q_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_db, fld);
            res[res_ct++] = _bcmResourceMcast;
            bid_detect[_bcmResourceMcast] = 1;
        }

        fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, rval_db,
                                     MC_CPU_Q_BST_STAT_TRIGGEREDf);
        if (fval && !bid_detect[_bcmResourceMcast]) {
            fld = MC_CPU_Q_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_db, fld);
            res[res_ct++] = _bcmResourceMcast;
            bid_detect[_bcmResourceMcast] = 1;
        }

        /* _bcmResourceEgrPortPoolSharedMcast*/
        fval = soc_reg64_field32_get(unit, MMU_THDM_DB_DEVICE_BST_STATr, rval_db,
                                     MC_PORTSP_BST_STAT_TRIGGEREDf);

        if (fval && !bid_detect[_bcmResourceEgrPortPoolSharedMcast]) {
            fld = MC_PORTSP_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_db, fld);
            res[res_ct++] = _bcmResourceEgrPortPoolSharedMcast;
            bid_detect[_bcmResourceEgrPortPoolSharedMcast] = 1;
        }

        COMPILER_64_ZERO(rval_db);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, MMU_THDM_DB_DEVICE_BST_STATr, xpe, 0, rval_db));
    }

    /* THDU Status/Triggers */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        BCM_IF_ERROR_RETURN
            (soc_reg_get(unit, THDU_BST_STATr, xpe, 0, &rval_uc));
        reg_status = THDU_BST_STATr;

        /* _bcmResourceUcast */
        fval = soc_reg64_field32_get(unit, reg_status, rval_uc,
                                     UC_Q_BST_STAT_TRIGGEREDf);
        if (fval && !bid_detect[_bcmResourceUcast]) {
            fld = UC_Q_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_uc, fld);
            res[res_ct++] = _bcmResourceUcast;
            bid_detect[_bcmResourceUcast] = 1;
        }

        /* _bcmResourceEgrPortPoolSharedUcast*/
        fval = soc_reg64_field32_get(unit, THDU_BST_STATr, rval_uc,
                                     UC_PORT_SP_BST_STAT_TRIGGEREDf);
        if (fval && !bid_detect[_bcmResourceEgrPortPoolSharedUcast]) {
            fld = UC_Q_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_uc, fld);
            res[res_ct++] = _bcmResourceEgrPortPoolSharedUcast;
            bid_detect[_bcmResourceEgrPortPoolSharedUcast] = 1;
        }

        /* _bcmResourceUCQueueGroup*/
        fval = soc_reg64_field32_get(unit, THDU_BST_STATr, rval_uc,
                                     UC_QGROUP_BST_STAT_TRIGGEREDf);
        if (fval && !bid_detect[_bcmResourceUCQueueGroup]) {
            fld = UC_QGROUP_BST_STAT_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_uc, fld);
            res[res_ct++] = _bcmResourceUCQueueGroup;
            bid_detect[_bcmResourceUCQueueGroup] = 1;
        }

        COMPILER_64_ZERO(rval_uc);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, THDU_BST_STATr, xpe, 0, rval_uc));

    }

    /* THDR Status/Triggers */
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        reg_status = MMU_THDR_DB_BST_STATr;
        BCM_IF_ERROR_RETURN
            (soc_reg_get(unit, reg_status, xpe, 0, &rval_rqe));

        /* _bcmResourceRQEQueue */
        fval = soc_reg64_field32_get(unit, reg_status, rval_rqe,
                                    PRIQ_TRIGGEREDf);
        if (fval && !bid_detect[_bcmResourceRQEQueue]) {
            fld = PRIQ_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_rqe, fld);
            res[res_ct++] = _bcmResourceRQEQueue;
            bid_detect[_bcmResourceRQEQueue] = 1;
        }

        /* _bcmResourceRQEPool*/
        fval = soc_reg64_field32_get(unit, reg_status, rval_rqe,
                                     SP_TRIGGEREDf);
        if (fval && !bid_detect[_bcmResourceRQEPool]) {
            fld = SP_IDf;
            index[res_ct] = soc_reg64_field32_get(unit, reg_status, rval_rqe, fld);
            res[res_ct++] = _bcmResourceRQEPool;
            bid_detect[_bcmResourceRQEPool] = 1;
        }

        COMPILER_64_ZERO(rval_rqe);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, reg_status, xpe, 0, rval_rqe));
    }

    if (res_ct == 0) {
        _bcm_bst_th_intr_enable_set(unit, 1);
        /* No resource to fetch */
        return BCM_E_NONE;
    }

    for (i = 0; i < res_ct; i++) {
        BCM_IF_ERROR_RETURN(
            _bcm_bst_th_sync_hw_snapshot(unit, res[i], -1, index[i]));
    }

    *flags = flags_tr;
    _bcm_bst_th_intr_enable_set(unit, 1);
    return BCM_E_NONE;
}

int
_bcm_bst_th_hw_stat_clear(int unit, _bcm_bst_resource_info_t *resInfo,
                    bcm_bst_stat_id_t bid, int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    int rv, pipe;
    uint32 rval;
    soc_mem_t base_mem = INVALIDm, mem = INVALIDm;
    soc_reg_t reg;
    int min_hw_idx, max_hw_idx, idx;
    int pipe_num = 1, pipe_offset = 0, inst, stats_per_inst;
    int num_entries, mem_wsz;
    void *pentry;
    char *dmabuf;
    int sync_val = 0;
    int p_idx;
    uint32 mmu_inst_map = 0;

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
        pipe_num = NUM_PIPE(unit);
    }
    if (index == -1) {
        min_hw_idx = resInfo->index_min;
        if (resInfo->num_instance > 1) {
            max_hw_idx = ((resInfo->index_max + 1) / pipe_num /
                          resInfo->num_instance) - 1;
        } else {
            max_hw_idx = ((resInfo->index_max + 1) / pipe_num) - 1;
        }
    } else {
        min_hw_idx = max_hw_idx = index;
    }

    if ((port != -1) && (bst_info->port_to_mmu_inst_map)) {
        (void)bst_info->port_to_mmu_inst_map(unit, bid, port, &mmu_inst_map);
    }

    for (pipe = 0; pipe < pipe_num; pipe++) {
        if ((base_mem = resInfo->stat_mem[0]) != INVALIDm) {
            int xpe = 0;
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if ((port != -1) && !(mmu_inst_map & (1 << xpe))) {
                    continue;
                }
                mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
                if (mem == INVALIDm) {
                    continue;
                }

                if (index < 0) {
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
                        p_idx = idx;
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
                            /* coverity[parameter_hidden: FALSE] */
                            int port, pool;
                            port = idx / _TH_MMU_NUM_POOL;
                            pool = idx % _TH_MMU_NUM_POOL;
                            p_idx = pool * _TH_MMU_PORTS_PER_PIPE + port;
                        }
                        pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                              dmabuf, p_idx);
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
                } else {
                    /* Clear a specific index */
                    uint32 entry[SOC_MAX_MEM_WORDS];
                    p_idx = index;
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
                        int port, pool;
                        port = index / _TH_MMU_NUM_POOL;
                        pool = index % _TH_MMU_NUM_POOL;
                        p_idx = pool * _TH_MMU_PORTS_PER_PIPE + port;
                    }
                    SOC_IF_ERROR_RETURN(
                        soc_mem_read(unit, mem, MEM_BLOCK_ALL, p_idx, entry));
                    soc_mem_field32_set(unit, mem, entry,
                                        resInfo->stat_field, 0);
                    SOC_IF_ERROR_RETURN(
                        soc_mem_write(unit, mem, MEM_BLOCK_ALL, p_idx, entry));
                }
            }
        } else if (resInfo->profile_r[0]) {
            pipe_offset += max_hw_idx;
        } else if ((reg = resInfo->stat_reg[pipe]) != INVALIDr) {
            int stat_idx = 0;
            int xpe = 0;
            uint32 mmu_inst_map = 0;

            stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;

            /* Clear stat counters */

            if (port == -1){
                mmu_inst_map = (1 << NUM_XPE(unit)) - 1;
            } else {
                if (bst_info->port_to_mmu_inst_map) {
                    (void)bst_info->port_to_mmu_inst_map(unit, bid, port,
                        &mmu_inst_map);
                }
            }

            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (!(mmu_inst_map & (1 << xpe))) {
                    continue;
                }
                for (idx = min_hw_idx; idx <= max_hw_idx; idx++) {
                    rval = 0;
                    if (stats_per_inst) {
                        inst = xpe;
                        stat_idx = idx;
                    } else {
                        inst = REG_PORT_ANY;
                        stat_idx = idx;
                    }
                    if (bid != bcmBstStatIdIngPool) {
                        rv = soc_reg32_get(unit, reg, inst, stat_idx, &rval);
                        if (rv) {
                            return BCM_E_INTERNAL;
                        }
                        soc_reg_field_set(unit, reg, &rval, resInfo->stat_field, 0);
                        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, stat_idx, rval));
                    } else if ((bid == bcmBstStatIdIngPool) && SOC_IS_TOMAHAWK(unit)){
                        /* IngPool requires clearing using THD_MISC_CONTROL reg */
                        soc_field_t ing_sp_clr_fields[] = {
                            CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT0f,
                            CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT1f,
                            CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT2f,
                            CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT3f,
                            CLEAR_THDI_BST_SP_GLOBAL_SHARED_CNT4f
                        };
                        rval = 0;
                        /* 4 SP + 1 Public pool */
                        soc_reg_field_set(unit, THD_MISC_CONTROLr, &rval,
                                          ing_sp_clr_fields[stat_idx], 0);
                        BCM_IF_ERROR_RETURN
                            (soc_reg32_set(unit, THD_MISC_CONTROLr, inst, stat_idx, rval));

                        soc_reg_field_set(unit, THD_MISC_CONTROLr, &rval,
                                          ing_sp_clr_fields[stat_idx], 1);
                        BCM_IF_ERROR_RETURN
                            (soc_reg32_set(unit, THD_MISC_CONTROLr, inst, stat_idx, rval));

                    }
                }
            }


        } /* if valid reg */
    } /* for each pipe */

    if (bst_info->post_sync) {
        bst_info->post_sync(unit, bid, sync_val);
    }

    return BCM_E_NONE;
}

int
_bcm_bst_th_sync_hw_snapshot(int unit, bcm_bst_stat_id_t bid,
                             int port, int index)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int num_entries, mem_wsz, sync_val = 0;
    int idx, idx_offset;
    int inst, xpe, xpe_offset, pipe, pipe_offset = 0;
    int stats_per_inst = 0; /* stats per mem instance(xpe) */
    void *pentry;
    char *dmabuf;
    uint32 rval, temp_val;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    bcm_gport_t gport;
    bcm_cos_queue_t cosq;

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

    base_mem = resInfo->stat_mem[0];
    reg = resInfo->stat_reg[0];

    if (reg != INVALIDr) {
        int stat_idx = 0;
        stats_per_inst = resInfo->num_stat_pp / resInfo->num_instance;
        pipe_offset = 0;

        for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
            rval = 0;
            if (stats_per_inst) {
                inst = idx / stats_per_inst;
                stat_idx = idx % stats_per_inst;
            } else {
                inst = REG_PORT_ANY;
                stat_idx = idx;
                stats_per_inst = 1;
            }

            idx_offset = idx + pipe_offset;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, inst, stat_idx, &rval));
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
        int min_idx = 0, max_idx = 0;
        int stats_per_pipe = 0; /* stats per pipe per mem-instance(xpe) */

        /* Get the mem attributes(Max idx) from the first child,
         * which could be used for other instances.
         * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
         */
        mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, 0, 0);

        if (mem == INVALIDm) {
            return BCM_E_PARAM;
        }
        stats_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
        stats_per_pipe = stats_per_inst / NUM_PIPE(unit);

        num_entries = soc_mem_index_max(unit, mem) + 1;
        min_idx = soc_mem_index_min(unit, mem);
        max_idx = soc_mem_index_max(unit, mem);
        mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, mem);

        dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "bst dmabuf");
        if (dmabuf == NULL) {
            return BCM_E_MEMORY;
        }

        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            xpe_offset = 0;
            gport = BCM_GPORT_INVALID;
            cosq = -1;

            if (xpe) {
                xpe_offset = stats_per_inst * xpe;
            }

            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                pipe_offset = 0;
                if (pipe) {
                    pipe_offset = stats_per_pipe * pipe;
                }

                mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
                if (mem == INVALIDm) {
                    continue;
                }

                if (soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                        min_idx, max_idx, dmabuf)) {
                    soc_cm_sfree(unit, dmabuf);
                    return BCM_E_INTERNAL;
                }

                for (idx = 0; idx < max_idx; idx++) {
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
                        /* coverity[parameter_hidden: FALSE] */
                        int port, pool;
                        port = idx % _TH_MMU_PORTS_PER_PIPE;
                        pool = idx / _TH_MMU_PORTS_PER_PIPE;
                        idx_offset = port * _TH_MMU_NUM_POOL + pool +
                                     pipe_offset + xpe_offset;
                    } else {
                        idx_offset = idx + pipe_offset + xpe_offset;
                    }
                    if (idx_offset >= resInfo->num_stat_pp) {
                        continue;
                    }
                    pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                          dmabuf, idx);
                    temp_val = soc_mem_field32_get(unit, mem, pentry,
                                                   resInfo->stat_field);
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
        soc_cm_sfree(unit, dmabuf);
    }

    if (bst_info->post_sync) {
        bst_info->post_sync(unit, bid, sync_val);
    }

    return BCM_E_NONE;
}

/*
 * This function is used to enable and disable headroom
 * pool monitoring for given headroom pool index and pipe.
 */
STATIC int
_bcm_th_bst_hdrm_monitor_set(int unit, int hdrm_idx,
                             int pipe, int enable)
{
    int idx, idx_min, idx_max;
    uint32 rval, hdrm_pool_cfg;

    if (pipe == -1) {
        /* Both Layer 0 and Layer 1 */
        idx_min = 0;
        idx_max = NUM_LAYER(unit);
    } else if ((pipe == 0) || (pipe == 3)) {
        /* Layer 0 */
        idx_min = 0;
        idx_max = 1;
    } else if ((pipe == 1) || (pipe == 2)) {
        /* Layer 1 */
        idx_min = 1;
        idx_max = 2;
    } else {
        return BCM_E_PARAM;
    }

    for (idx = idx_min; idx < idx_max; idx++) {
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, idx, 0, &rval));
        hdrm_pool_cfg = soc_reg_field_get(unit, THDI_HDRM_POOL_CFGr, rval,
                                          PEAK_COUNT_UPDATE_EN_HPf);
        if (enable == 0x1) {
            hdrm_pool_cfg = hdrm_pool_cfg | (0x1 << hdrm_idx);
        } else {
            hdrm_pool_cfg = hdrm_pool_cfg & (~(0x1 << hdrm_idx));
        }
        soc_reg_field_set(unit, THDI_HDRM_POOL_CFGr, &rval,
                          PEAK_COUNT_UPDATE_EN_HPf, hdrm_pool_cfg);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, idx, 0, rval));
    }

    return BCM_E_NONE;
}

/*
 * This function is used to clear the hardware stat for
 * the headroom pool usage count for given [Port, Prio].
 */
STATIC int
_bcm_th_bst_hdrm_hw_stat_clear(int unit,
                               bcm_gport_t gport,
                               bcm_cos_t cosq,
                               bcm_cosq_buffer_id_t buffer)
{
    int i, j;
    uint32 rval, hdrm_pool_cfg[2];
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int enable = 0x0;
    uint32 xpe_map = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe = -1;
    soc_info_t *si = NULL;
    int bcm_rv;

    if ((buffer < BCM_COSQ_BUFFER_ID_INVALID) || (buffer >= NUM_XPE(unit))) {
        return BCM_E_PARAM;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    xpe_map = (buffer == BCM_COSQ_BUFFER_ID_INVALID) ? 0xf : (1 << buffer);
    if (gport == -1) {
        if (cosq == BCM_COS_INVALID) {
            pipe = -1;
            /* 4 Headroom Pool Ids */
            start_hw_index = 0;
            end_hw_index = 3;
            xpe_map &= 0xf;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (bst_info->handlers.resolve_index == NULL) {
            return BCM_E_UNAVAIL;
        }

        bst_info->handlers.resolve_index(unit, gport, cosq,
                                         bcmBstStatIdHeadroomPool, &pipe,
                                         &start_hw_index, &end_hw_index,
                                         NULL, NULL, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        si = &SOC_INFO(unit);
        xpe_map &= si->ipipe_xpe_map[pipe];
    }

    if (!xpe_map) {
        return BCM_E_PARAM;
    }

    for (j = 0; j < NUM_LAYER(unit); j++) {
        rval = 0x0;
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, THDI_HDRM_POOL_CFGr, j, 0, &rval));
        hdrm_pool_cfg[j] = soc_reg_field_get(unit, THDI_HDRM_POOL_CFGr, rval,
                                          PEAK_COUNT_UPDATE_EN_HPf);
    }

    for (i = start_hw_index; i <= end_hw_index; i++) {
        /* Disable hardware tracking the headroom pool */
        enable = 0x0;
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_monitor_set(unit, i, pipe, enable));

        /* Reset Headroom Pool Peak Use-Count */
        for (j = 0; j < NUM_XPE(unit); j++) {
            if (xpe_map & (1 << j)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg32_get(unit, resInfo->stat_reg[0],
                                   j, i, &rval));
                soc_reg_field_set(unit, resInfo->stat_reg[0], &rval,
                                  resInfo->stat_field, 0x0);

                BCM_IF_ERROR_RETURN
                    (soc_reg32_set(unit, THDI_HDRM_POOL_PEAK_COUNT_HPr,
                                   j, i, rval));
            }
        }

        /* Enable hardware tracking the headroom pool */
        enable = 0x1;
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_monitor_set(unit, i, pipe, enable));
    }

    for (j = 0; j < NUM_LAYER(unit); j++) {
        rval = 0x0;
        soc_reg_field_set(unit, THDI_HDRM_POOL_CFGr, &rval,
                          PEAK_COUNT_UPDATE_EN_HPf, hdrm_pool_cfg[j]);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, THDI_HDRM_POOL_CFGr, j, 0, rval));
    }

    return BCM_E_NONE;
}

/*
 * This function is used to clear the software stat for
 * the headroom pool usage count for given [Port, Prio].
 */
STATIC int
_bcm_th_bst_hdrm_sw_stat_clear(int unit,
                               bcm_gport_t gport,
                               bcm_cos_t cosq,
                               bcm_cosq_buffer_id_t buffer)
{
    int i = 0, j = 0;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cmn_unit_info_t *bst_info;
    uint32 xpe_map = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe = -1;
    soc_info_t *si = NULL;
    int bcm_rv;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    if ((buffer < BCM_COSQ_BUFFER_ID_INVALID) || (buffer >= NUM_XPE(unit))) {
        return BCM_E_PARAM;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    xpe_map = (buffer == BCM_COSQ_BUFFER_ID_INVALID) ? 0xf : (1 << buffer);
    if (gport == -1) {
        if (cosq == BCM_COS_INVALID) {
            /* 4 Headroom Pool Ids */
            start_hw_index = 0;
            end_hw_index = 3;
            xpe_map &= 0xf;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (bst_info->handlers.resolve_index == NULL) {
            return BCM_E_UNAVAIL;
        }

        bst_info->handlers.resolve_index(unit, gport, cosq,
                                         bcmBstStatIdHeadroomPool, &pipe,
                                         &start_hw_index, &end_hw_index,
                                         NULL, NULL, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        si = &SOC_INFO(unit);
        xpe_map &= si->ipipe_xpe_map[pipe];
    }

    if (!xpe_map) {
        return BCM_E_PARAM;
    }

    for (i = start_hw_index; i <= end_hw_index; i++) {
        for (j = 0; j < NUM_XPE(unit); j++) {
            if (xpe_map & (1 << j)) {
                resInfo->p_stat[i + j * NUM_XPE(unit)] = 0x0;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * This function is used to clear the software/hardware
 * stat for the headroom pool usage count for given [Port, Prio].
 */
int
_bcm_th_bst_hdrm_stat_clear(int unit,
                            bcm_gport_t gport,
                            bcm_cos_queue_t cosq,
                            bcm_cosq_buffer_id_t buffer)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int bcm_rv = BCM_E_NONE;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    /* sync all the stats */
    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    sal_mutex_take(bst_info->bst_reslock, sal_mutex_FOREVER);
    bcm_rv = _bcm_th_bst_hdrm_sw_stat_clear(unit, gport, cosq, buffer);
    sal_mutex_give(bst_info->bst_reslock);
    if (BCM_FAILURE(bcm_rv)) {
        return bcm_rv;
    }

    if (bst_info->track_mode) {
        /* BST tracking is in peak mode */
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_hw_stat_clear(unit, gport, cosq, buffer));
    }

    return BCM_E_NONE;
}

/*
 * This function is used to get the BST stat for
 * the headroom pool usage count from software cache.
 */
STATIC int
_bcm_th_cosq_bst_hdrm_stat_sw_get(int unit,
                                  bcm_gport_t gport,
                                  bcm_cos_t cosq,
                                  bcm_cosq_buffer_id_t buffer,
                                  uint32 *max_hdrm_stat)
{
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_cmn_unit_info_t *bst_info;
    uint32 temp_stat = 0;
    int i = 0, j = 0;
    int start_hw_index = 0, end_hw_index = 0, pipe = -1;
    uint32 xpe_map = 0;
    soc_info_t *si = NULL;
    int bcm_rv;

    if ((buffer < BCM_COSQ_BUFFER_ID_INVALID) || (buffer >= NUM_XPE(unit))) {
        return BCM_E_PARAM;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bcmBstStatIdHeadroomPool);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    xpe_map = (buffer == BCM_COSQ_BUFFER_ID_INVALID) ? 0xf : (1 << buffer);
    if (gport == -1) {
        if (cosq == BCM_COS_INVALID) {
            xpe_map &= 0xf;
            start_hw_index = 0;
            end_hw_index = 3;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (bst_info->handlers.resolve_index == NULL) {
            return BCM_E_UNAVAIL;
        }

        bst_info->handlers.resolve_index(unit, gport, cosq,
                                         bcmBstStatIdHeadroomPool, &pipe,
                                         &start_hw_index, &end_hw_index,
                                         NULL, NULL, &bcm_rv);
        if (bcm_rv) {
            return bcm_rv;
        }
        si = &SOC_INFO(unit);
        xpe_map &= si->ipipe_xpe_map[pipe];
    }

    if (!xpe_map) {
        return BCM_E_PARAM;
    }

    for (i = start_hw_index; i <= end_hw_index; i++) {
        for (j = 0; j < NUM_XPE(unit); j++) {
            if (xpe_map & (1 << j)) {
                temp_stat = resInfo->p_stat[i + (j * NUM_XPE(unit))];
                if (temp_stat > *max_hdrm_stat) {
                    *max_hdrm_stat = temp_stat;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * This function is used to get the BST stat for
 * the headroom pool usage count for given [Port, Prio].
 * Note: Regardless of tracking mode this function
 * returns max value of headroom pools.
 */
int
_bcm_th_cosq_bst_hdrm_stat_get(int unit,
                               bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_buffer_id_t buffer,
                               uint32 options,
                               uint64 *pvalue)
{
    uint32 max_hdrm_stat = 0;

    if (pvalue == NULL) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(*pvalue);

    BCM_IF_ERROR_RETURN
        (_bcm_th_cosq_bst_hdrm_stat_sw_get(unit, gport, cosq, buffer,
                                           &max_hdrm_stat));

    /* Cell Size = 208 bytes */
    COMPILER_64_ADD_32(*pvalue,
                       (max_hdrm_stat * _TH_MMU_BYTES_PER_CELL));

    if (options & BCM_COSQ_STAT_CLEAR) {
        BCM_IF_ERROR_RETURN
            (_bcm_th_bst_hdrm_stat_clear(unit, gport, cosq, buffer));
    }
    return BCM_E_NONE;
}

/* Function to return the corresponding XPE instance for a given gport.
 * Direction(Ingress/Egress) is derived from bid type
 */
int _bst_th_port_to_mmu_inst_map_get(int unit, bcm_bst_stat_id_t bid,
                                     bcm_gport_t gport, uint32 *mmu_inst_map)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_bst_resource_info_t *resInfo;
    bcm_port_t local_port;
    bcm_module_t module;
    bcm_trunk_t trunk;
    int id, pipe;

    resInfo = _BCM_BST_RESOURCE(unit, bid);
    if ((resInfo == NULL) || (!_BCM_BST_RESOURCE_VALID(resInfo))) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SET(gport)) {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        local_port = gport;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport, &module,
                                    &local_port, &trunk, &id));
    }

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if ((bid == bcmBstStatIdEgrPool) || (bid == bcmBstStatIdEgrMCastPool) ||
        (bid == bcmBstStatIdUcast) || (bid == bcmBstStatIdMcast) ||
        (bid == bcmBstStatIdRQEQueue) || (bid == bcmBstStatIdRQEPool) ||
        (bid == bcmBstStatIdUCQueueGroup) ||
        (bid == bcmBstStatIdEgrPortPoolSharedUcast) ||
        (bid == bcmBstStatIdEgrPortPoolSharedMcast)) {
        /* For Egress Res,
         * Pipe 0,1 has XPE 0,2
         * Pipe 2,3 has XPE 1,3
         */
        *mmu_inst_map = si->epipe_xpe_map[pipe];
    } else if ((bid == bcmBstStatIdIngPool) || (bid == bcmBstStatIdPortPool) ||
               (bid == bcmBstStatIdPriGroupShared) ||
               (bid == bcmBstStatIdPriGroupHeadroom)) {
        /* For Ingress Res,
         * Pipe 0,3 has XPE 0,1
         * Pipe 1,2 has XPE 2,3
         */
        *mmu_inst_map = si->ipipe_xpe_map[pipe];
    } else {
        /* For Global res, return all instance */
        *mmu_inst_map = (1 << NUM_XPE(unit)) - 1;
    }

    return BCM_E_NONE;
}

int _bst_th_thd_reg_index_get(int unit, bcm_bst_stat_id_t bid, int pipe,
                                    int hw_index, int *inst, int *index)
{
    if (bid == bcmBstStatIdEgrPortPoolSharedMcast) {
        int phy_port, mmu_port, mmu_port_base;
        soc_info_t *si = &SOC_INFO(unit);

        mmu_port_base = SOC_TH_MMU_PORT_STRIDE * pipe;
        mmu_port = hw_index / _TH_MMU_NUM_POOL;
        phy_port = si->port_m2p_mapping[mmu_port + mmu_port_base];
        *inst = si->port_p2l_mapping[phy_port];
        *index = hw_index % _TH_MMU_NUM_POOL;
    }

    return BCM_E_NONE;
}

int _bst_th_cmn_io_op(int unit, int op, int pipe, int hw_index,
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
            _bst_th_thd_reg_index_get(unit, bid, pipe, hw_index,
                                      &port, &reg_index));
    }

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
            /* For TH, the global resource is actually each per XPE */
            if(bid == bcmBstStatIdDevice){
                for (port = 0; port < NUM_XPE(unit); port++) {
                    int inst;
                    soc_reg_field_set(unit, reg, &rval, field, *resval);
                    inst = port | SOC_REG_ADDR_INSTANCE_MASK;
                    SOC_IF_ERROR_RETURN(
                        soc_reg32_set(unit, reg, inst, reg_index, rval));
                    /* update sw threshold */
                    resInfo->p_threshold[port] = *resval;
                }
            } else
            {
                soc_reg_field_set(unit, reg, &rval, field, *resval);
                SOC_IF_ERROR_RETURN(
                    soc_reg32_set(unit, reg, port, reg_index, rval));
            }
        }
    }

    return BCM_E_NONE;

}

/* decide each bid's thd/profile/.. idx based on {pipe+xpe} */
typedef struct _bcm_bst_id_map_s {
    int pipe;
    int xpe;
    int tid;    /* thd reg idx of resInfo->threshold_reg */
    int pid;    /* profile idx of resInfo->profile_r */
    int pregid; /* profile reg idx of resInfo->profile_reg */
    int idinp;  /* idx of regs which are managed by a same resInfo->profile_r */
}_bcm_bst_id_map_t;

/* bcmBstStatIdDevice */
const _bcm_bst_id_map_t _bcm_bst_th_id_map_0[] = {
/*  pipe    xpe     tid     pid     pregid  idinp */
    {-1,    -1,     0,      -1,     -1,     -1},
};
/* bcmBstStatIdIngPool */
const _bcm_bst_id_map_t _bcm_bst_th_id_map_1i[] = {
/*  pipe    xpe     tid     pid     pregid  idinp */
    {0,     0,      0,      -1,     -1,     -1},
    {0,     1,      1,      -1,     -1,     -1},
    {1,     2,      2,      -1,     -1,     -1},
    {1,     3,      3,      -1,     -1,     -1},
    {2,     2,      2,      -1,     -1,     -1},
    {2,     3,      3,      -1,     -1,     -1},
    {3,     0,      0,      -1,     -1,     -1},
    {3,     1,      1,      -1,     -1,     -1},
};
/* bcmBstStatIdEgrPool */
/* bcmBstStatIdEgrMCastPool */
/* bcmBstStatIdRQEQueue */
/* bcmBstStatIdRQEPool */
const _bcm_bst_id_map_t _bcm_bst_th_id_map_1e[] = {
/*  pipe    xpe     tid     pid     pregid  idinp */
    {0,     0,      0,      -1,     -1,     -1},
    {0,     2,      2,      -1,     -1,     -1},
    {1,     0,      0,      -1,     -1,     -1},
    {1,     2,      2,      -1,     -1,     -1},
    {2,     1,      1,      -1,     -1,     -1},
    {2,     3,      3,      -1,     -1,     -1},
    {3,     1,      1,      -1,     -1,     -1},
    {3,     3,      3,      -1,     -1,     -1},
};
/* bcmBstStatIdUcast */
/* bcmBstStatIdEgrPortPoolSharedUcast */
/* bcmBstStatIdUCQueueGroup */
const _bcm_bst_id_map_t _bcm_bst_th_id_map_2e[] = {
/*  pipe    xpe     tid     pid     pregid  idinp */
    {0,     -1,     0,      0,      0,      0},
    {1,     -1,     1,      0,      0,      0},
    {2,     -1,     2,      0,      0,      0},
    {3,     -1,     3,      0,      0,      0},
};
/* bcmBstStatIdPortPool */
/* bcmBstStatIdPriGroupShared */
/* bcmBstStatIdPriGroupHeadroom */
const _bcm_bst_id_map_t _bcm_bst_th_id_map_3i[] = {
/*  pipe    xpe     tid     pid     pregid  idinp */
    {0,     0,      0,      0,      0,      0},
    {0,     1,      0,      0,      1,      1},
    {1,     2,      1,      1,      2,      0},
    {1,     3,      1,      1,      3,      1},
    {2,     2,      2,      1,      2,      0},
    {2,     3,      2,      1,      3,      1},
    {3,     0,      3,      0,      0,      0},
    {3,     1,      3,      0,      1,      1},
};
/* bcmBstStatIdMcast */
const _bcm_bst_id_map_t _bcm_bst_th_id_map_3e[] = {
/*  pipe    xpe     tid     pid     pregid  idinp */
    {0,     0,      0,      0,      0,      0},
    {0,     2,      0,      0,      2,      1},
    {1,     0,      1,      0,      0,      0},
    {1,     2,      1,      0,      2,      1},
    {2,     1,      2,      1,      1,      0},
    {2,     3,      2,      1,      3,      1},
    {3,     1,      3,      1,      1,      0},
    {3,     3,      3,      1,      3,      1},
};
/* bcmBstStatIdEgrPortPoolSharedMcast */
const _bcm_bst_id_map_t _bcm_bst_th_id_map_4e[] = {
/*  pipe    xpe     tid     pid     pregid  idinp */
    {0,     0,      0,      0,      0,      0},
    {0,     2,      4,      2,      2,      0},
    {1,     0,      1,      0,      0,      0},
    {1,     2,      5,      2,      2,      0},
    {2,     1,      2,      1,      1,      0},
    {2,     3,      6,      3,      3,      0},
    {3,     1,      3,      1,      1,      0},
    {3,     3,      7,      3,      3,      0},
};


STATIC int _bst_th_id_map_get(int unit, bcm_bst_stat_id_t bid, 
                              const _bcm_bst_id_map_t **map, int *len)
{
    switch (bid) {
        case bcmBstStatIdDevice:
            *map = &_bcm_bst_th_id_map_0[0];
            *len = sizeof(_bcm_bst_th_id_map_0)/sizeof(_bcm_bst_id_map_t);
            break;
        case bcmBstStatIdIngPool:
            *map = _bcm_bst_th_id_map_1i;
            *len = sizeof(_bcm_bst_th_id_map_1i)/sizeof(_bcm_bst_id_map_t);
            break;
        case bcmBstStatIdEgrPool:
        case bcmBstStatIdEgrMCastPool:
        case bcmBstStatIdRQEQueue:
        case bcmBstStatIdRQEPool:
            *map = _bcm_bst_th_id_map_1e;
            *len = sizeof(_bcm_bst_th_id_map_1e)/sizeof(_bcm_bst_id_map_t);
            break;
        case bcmBstStatIdUcast:
        case bcmBstStatIdEgrPortPoolSharedUcast:
        case bcmBstStatIdUCQueueGroup:
            *map = _bcm_bst_th_id_map_2e;
            *len = sizeof(_bcm_bst_th_id_map_2e)/sizeof(_bcm_bst_id_map_t);
            break;
        case bcmBstStatIdPortPool:
        case bcmBstStatIdPriGroupShared:
        case bcmBstStatIdPriGroupHeadroom:
            *map = _bcm_bst_th_id_map_3i;
            *len = sizeof(_bcm_bst_th_id_map_3i)/sizeof(_bcm_bst_id_map_t);
            break;
        case bcmBstStatIdMcast:
            *map = _bcm_bst_th_id_map_3e;
            *len = sizeof(_bcm_bst_th_id_map_3e)/sizeof(_bcm_bst_id_map_t);
            break;
        case bcmBstStatIdEgrPortPoolSharedMcast:
            *map = _bcm_bst_th_id_map_4e;
            *len = sizeof(_bcm_bst_th_id_map_4e)/sizeof(_bcm_bst_id_map_t);
            break;
        default:
            return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
}

STATIC int
_bst_th_profile_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    const _bcm_bst_id_map_t *map, *map0;
    bcm_bst_stat_id_t bid;
    bcm_cosq_bst_profile_t profile;
    int i, cnt, pcnt, regcnt, xpe, ei, index_max, entry_size, wb;
    uint32 profile_index, thd_bmp;
    soc_mem_t preg[_BCM_MAX_PROFILES][_BCM_MAX_REGS_PER_PROFILE];
    uint32 max_thd = 0;

    wb = SOC_WARM_BOOT(unit) ? 1 : 0;
    bst_info = _BCM_UNIT_BST_INFO(unit);
    profile.byte = bst_info->max_bytes;

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        max_thd = TH2_BST_THRESHOLD_MAX_HIGH;
    } else
#endif
    {
        max_thd = TH_BST_THRESHOLD_MAX_HIGH;
    }

    _BCM_BST_RESOURCE_ITER(bst_info, bid) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if (!(resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID)) {
            continue;
        }

        if (resInfo->threshold_gran != 0) {
            profile.byte = bst_info->to_byte(unit,
                           max_thd - (resInfo->threshold_gran - 1));
        }

        BCM_IF_ERROR_RETURN(_bst_th_id_map_get(unit, bid, &map0, &cnt));
        
        /* create profiles */
        resInfo->profile_m = NULL;
        for (i = 0; i < _BCM_MAX_PROFILES; i ++) {
            resInfo->profile_r[i] = NULL;
        }
        if (resInfo->profile_mem != INVALIDm) {
            soc_profile_mem_t_init(&resInfo->iprofile_m);
            entry_size = soc_mem_entry_bytes(unit, resInfo->profile_mem);
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_create(unit, &resInfo->profile_mem, 
                                   &entry_size, 1, &resInfo->iprofile_m));
            resInfo->profile_m = &resInfo->iprofile_m;
        } else if (resInfo->profile_reg[0] != INVALIDr) {
            for (i = 0, pcnt = 0, regcnt = 0, map = map0; i < cnt; i ++, map ++) {
                preg[map->pid][map->idinp] = resInfo->profile_reg[map->pregid];
                if (map->pid >= pcnt) {
                    pcnt = map->pid + 1;
                }
                if (map->idinp >= regcnt) {
                    regcnt = map->idinp + 1;
                }
            }
            for (i = 0; i < pcnt; i ++) {
                soc_profile_reg_t_init(&resInfo->iprofile_r[i]);
                SOC_IF_ERROR_RETURN(soc_profile_reg_create
                    (unit, preg[i], regcnt, &resInfo->iprofile_r[i]));
                if ((bid == bcmBstStatIdUcast) ||
                    (bid == bcmBstStatIdEgrPortPoolSharedUcast) ||
                    (bid == bcmBstStatIdUCQueueGroup)) {
                    resInfo->iprofile_r[i].profile_flag = SOC_PROFILE_FLAG_XPE_SINGLE_ACC;
                }
                resInfo->profile_r[i] = &resInfo->iprofile_r[i];
            }
        }

        /* init profiles reference */
        index_max = (resInfo->index_max + 1) / 
            (NUM_PIPE(unit) * resInfo->num_instance);
        for (i = 0, map = map0, thd_bmp = 0x0; i < cnt; i ++, map ++) {
            if (thd_bmp & (1 << map->tid)) {
                continue;
            }

            for (ei = 0; ei < index_max; ei ++) {
                /* get profile index */
                if (wb) {
                    BCM_IF_ERROR_RETURN(_bst_th_cmn_io_op(unit, 
                                        BST_OP_GET | BST_OP_THD,
                                        map->pipe, ei, bid,
                                        resInfo->threshold_mem[map->pipe],
                                        resInfo->threshold_reg[map->tid],
                                        resInfo->threshold_field,
                                        &profile_index));
                } else if (ei == 0) {
                    xpe = (resInfo->profile_r[map->pid]->regs_count > 1) ? 
                        BCM_BST_DUP_XPE : map->xpe;
                    BCM_IF_ERROR_RETURN(_bst_th_cmn_profile_set_hw
                        (unit, map->pipe, xpe, -1, bid, &profile, &profile_index));
                    continue;
                }
                /* add reference */
                if (resInfo->profile_r[0]) {
                    BCM_IF_ERROR_RETURN(soc_profile_reg_reference(unit,
                           resInfo->profile_r[map->pid], profile_index, 1));
                } else if (resInfo->profile_m) {
                    BCM_IF_ERROR_RETURN(soc_profile_mem_reference(unit,
                           resInfo->profile_m, profile_index, 1));
                }
            }

            thd_bmp |= (1 << map->tid);
        }
    }
    
    if (SOC_IS_TOMAHAWKX(unit)) {
        /* Set default threshold for bcmBstStatIdDevice and bcmBstStatIdIngPool from default
         * value to max to avoid unexpected trigger.
         */
        int pool_num = 4;
        bcm_cosq_bst_profile_t_init(&profile);
        profile.byte = bst_info->max_bytes;
        for (ei = 0; ei < pool_num; ei++) {
            /* coverity[callee_ptr_arith: FALSE] */
            SOC_IF_ERROR_RETURN(_bst_th_cmn_profile_set_hw(unit, -1, 
                BCM_BST_DUP_XPE, ei, bcmBstStatIdIngPool, &profile, NULL));
        }
        SOC_IF_ERROR_RETURN(_bst_th_cmn_profile_set_hw(unit, -1,
            BCM_BST_DUP_XPE, 0, bcmBstStatIdDevice, &profile, NULL));
    }

    return BCM_E_NONE;
}

/*
 * Get threshold of specified bid from pipe and hw_index
 *
 * Usage of xpe:
 *  0~3:    single xpe, profile[0] = reg[xpe]
 *   -1:    the first xpe related to the pipe, profile[0] = reg[first xpe]
 *   -2:    all xpe related to the pipe, profile[i] = reg[i]
 */
STATIC int 
_bst_th_cmn_profile_get_hw(int unit, int pipe, int xpe, int hw_index, 
                            bcm_bst_stat_id_t bid, 
                            bcm_cosq_bst_profile_t *profile)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    uint32 cells, profile_index, val;
    int i, cnt, xpe_id, pipe_id;
    const _bcm_bst_id_map_t *map;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bid);

    BCM_IF_ERROR_RETURN(_bst_th_id_map_get(unit, bid, &map, &cnt));
    for (i = 0; i < cnt; i ++, map ++) {
        if (((pipe >= 0) && (map->pipe >= 0) && (pipe != map->pipe)) ||
            ((xpe >= 0) && (map->xpe >= 0) && (xpe != map->xpe))) {
            continue;
        }

        xpe_id = (map->xpe < 0) ? 0: map->xpe;
        pipe_id = (map->pipe < 0) ? 0: map->pipe;

        SOC_IF_ERROR_RETURN(        
           _bst_th_cmn_io_op(unit, BST_OP_GET | BST_OP_THD,
                              pipe_id, hw_index, bid,
                              resInfo->threshold_mem[pipe_id],
                              resInfo->threshold_reg[map->tid],
                              resInfo->threshold_field,
                              &val));
        if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
            profile_index = val;
            SOC_IF_ERROR_RETURN(
               _bst_th_cmn_io_op(unit, BST_OP_GET, 
                                  pipe_id, profile_index, bid,
                                  resInfo->profile_mem,
                                  resInfo->profile_reg[map->pregid],
                                  resInfo->profile_field, 
                                  &cells));
        } else {
            cells = val;
        }

        /* Taking granularity into count */
        if (resInfo->threshold_gran != 0) {
            cells = cells * resInfo->threshold_gran - resInfo->threshold_adj;
        } else {
            cells -= resInfo->threshold_adj;
        }
        
        if (xpe == BCM_BST_MLT_XPE) {
            profile[xpe_id].byte = bst_info->to_byte(unit, cells);
        } else {
            profile[0].byte = bst_info->to_byte(unit, cells);
            break;
        }
    }
    
    return BCM_E_NONE;
}

/*
 * Get threshold of specified bid from pipe and hw_index
 *
 * Usage of xpe:
 *  0~3:    single xpe, reg[xpe] = profile[0]
 *   -1:    all xpe related to the pipe with same value, reg[i] = profile[0]
 *   -2:    all xpe related to the pipe with different value, reg[i] = profile[i]
 */
STATIC int 
_bst_th_cmn_profile_set_hw(int unit, int pipe, int xpe, int hw_index, 
                            bcm_bst_stat_id_t bid,
                            bcm_cosq_bst_profile_t *profile, 
                            uint32 *p_profile)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    bcm_cosq_bst_profile_t profile_arr[_TH_XPES_PER_DEV];
    uint32 byte, cells[_BCM_MAX_PROFILE_REGS];
    uint32 profile_index, old_profile_index;
    uint32 entry[SOC_MAX_MEM_WORDS];
    void *entries[1];
    uint64 rval[_BCM_MAX_PROFILES][_BCM_MAX_REGS_PER_PROFILE];
    uint64 *prval[_BCM_MAX_PROFILES][_BCM_MAX_REGS_PER_PROFILE];
    int pipe_factor = 1, pipe_offset = 0, offset_per_xpe, num_instance;
    int i, cnt, xpe_id, pipe_id, trigger = 0;
    const _bcm_bst_id_map_t *map;

    if ((bst_info = _BCM_UNIT_BST_INFO(unit)) == NULL) {
        return BCM_E_INIT;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bid);

    num_instance = (resInfo->num_instance > 1) ? resInfo->num_instance : 1;
    offset_per_xpe = (resInfo->index_max + 1) / num_instance;
    if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
        pipe_factor = NUM_PIPE(unit);
        pipe_offset = pipe * (offset_per_xpe / pipe_factor);
    }

    /* in case profile contains multiple regs while api only passing one,
       read hw to get unprovided regs */
    if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
        if ((resInfo->profile_r[0]) && (resInfo->profile_r[0]->regs_count > 1) 
            && (xpe >= 0)) {
            if (xpe >= _TH_XPES_PER_DEV) {
                return BCM_E_PARAM;
            }
            sal_memset(profile_arr, 0, sizeof(profile_arr));
            BCM_IF_ERROR_RETURN(_bst_th_cmn_profile_get_hw
                (unit, pipe, BCM_BST_MLT_XPE, hw_index, bid, profile_arr));
            profile_arr[xpe].byte = profile->byte;
            profile = profile_arr;
            xpe = BCM_BST_MLT_XPE;
        }
    }
    
    BCM_IF_ERROR_RETURN(_bst_th_id_map_get(unit, bid, &map, &cnt));
    for (i = 0; i < cnt; i ++, map ++) {
        if (((pipe >= 0) && (map->pipe >= 0) && (pipe != map->pipe)) ||
            ((xpe >= 0) && (map->xpe >= 0) && (xpe != map->xpe))) {
            continue;
        }

        xpe_id = (map->xpe < 0) ? 0: map->xpe;
        pipe_id = (map->pipe < 0) ? 0: map->pipe;

        if (xpe == BCM_BST_MLT_XPE) {
            byte = profile[xpe_id].byte;
        } else {
            byte = profile[0].byte;
        }
        /* Taking granularity into count */    
        if (resInfo->threshold_gran != 0) {
            cells[xpe_id] = (bst_info->to_cell(unit, byte) +
                    resInfo->threshold_adj + resInfo->threshold_gran - 1) /
                    resInfo->threshold_gran;
        } else {
            cells[xpe_id] = bst_info->to_cell(unit, byte) + resInfo->threshold_adj;
        }
        
        /* update sw thd */
        if (hw_index >= 0) {
            if (BST_THRESHOLD_MAX_HIGH != bst_info->to_cell(unit, byte)) {
                if (map->xpe == -1) {
                    int xid;
                    for (xid = 0; xid < NUM_XPE(unit); xid++) {
                        resInfo->p_threshold
                            [hw_index + pipe_offset + xid * offset_per_xpe]
                                = cells[xpe_id];
                    }
                } else {
                    resInfo->p_threshold
                        [hw_index + pipe_offset + xpe_id * offset_per_xpe]
                            = cells[xpe_id];
                }
            } else {
                cells[xpe_id] = bst_info->to_cell(unit, byte);
            }
        }

        if ((cells[xpe_id] > 0) && (cells[xpe_id] < BST_THRESHOLD_MAX_HIGH)) {
            trigger = 1;
        }

        if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
            profile_index = old_profile_index = -1;

            /* add profile */
            if (resInfo->profile_r[0]) {
                COMPILER_64_ZERO(rval[map->pid][map->idinp]);
                soc_reg64_field32_set(unit, resInfo->profile_reg[0], 
                    &rval[map->pid][map->idinp], resInfo->profile_field, 
                    cells[xpe_id]);
                prval[map->pid][map->idinp] = &rval[map->pid][map->idinp];
                /* hold settings in rval until all regs of that profile 
                   are configured, then add profile */
                if (map->idinp + 1 == resInfo->profile_r[map->pid]->regs_count) {
                    SOC_IF_ERROR_RETURN(soc_profile_reg_add(unit, 
                        resInfo->profile_r[map->pid], prval[map->pid], 
                        1, &profile_index));
                }
            } else {
                sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
                soc_mem_field32_set(unit, resInfo->profile_mem, &entry,
                                    resInfo->profile_field, cells[0]);
                entries[0] = entry;
                SOC_IF_ERROR_RETURN(soc_profile_mem_add(unit, 
                    resInfo->profile_m, entries, 1, &profile_index));
            }

            if (profile_index != -1) {
                if ((bid == bcmBstStatIdMcast) && _BCM_BST_TH_IS_CPU_QUEUE(hw_index)) {
                    uint64 value;
                    COMPILER_64_ZERO(value);
                    soc_reg64_field32_set(unit, MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILEr,
                        &value, THRESHOLDf, cells[xpe_id]);
                    SOC_IF_ERROR_RETURN(soc_reg_set(unit,
                        MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE0r,
                        REG_PORT_ANY, profile_index, value));
                    SOC_IF_ERROR_RETURN(soc_reg_set(unit,
                        MMU_THDM_DB_CPUQUEUE_BST_THRESHOLD_PROFILE_XPE2r,
                        REG_PORT_ANY, profile_index, value));
                }

                if (hw_index >= 0) {
                    /* old profile id */
                    SOC_IF_ERROR_RETURN(        
                       _bst_th_cmn_io_op(unit, BST_OP_GET | BST_OP_THD, 
                                          pipe_id, hw_index, bid, 
                                          resInfo->threshold_mem[pipe_id],
                                          resInfo->threshold_reg[map->tid],
                                          resInfo->threshold_field,
                                          &old_profile_index));
                    /* attach the new profile id */
                    SOC_IF_ERROR_RETURN(        
                        _bst_th_cmn_io_op(unit, BST_OP_SET | BST_OP_THD, 
                                           pipe_id, hw_index, bid, 
                                           resInfo->threshold_mem[pipe_id],
                                           resInfo->threshold_reg[map->tid],
                                           resInfo->threshold_field, 
                                           &profile_index));
                    SOC_IF_ERROR_RETURN(_bcm_bst_cmn_profile_delete_hw
                        (unit, bid, map->pid, old_profile_index));
                }
                if (p_profile) {
                    *p_profile = profile_index;
                }
            }
        } else {
            SOC_IF_ERROR_RETURN(
                _bst_th_cmn_io_op(unit, BST_OP_SET | BST_OP_THD, 
                                   pipe_id, hw_index, bid, 
                                   resInfo->threshold_mem[pipe_id],
                                   resInfo->threshold_reg[map->tid],
                                   resInfo->threshold_field,
                                   &cells[xpe_id]));
        }
    }

    if (trigger && bst_info->hw_trigger_set) {
        bst_info->hw_trigger_set(unit, bid);
    }

    return BCM_E_NONE;
}

int
bcm_bst_th_init(int unit)
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
    uint32 max_thd = 0;

    bst_info = _BCM_UNIT_BST_INFO(unit);
    if (!bst_info) {
        return BCM_E_MEMORY;
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        max_thd = TH2_BST_THRESHOLD_MAX_HIGH;
    } else
#endif
    {
        max_thd = TH_BST_THRESHOLD_MAX_HIGH;
    }

    bst_info->to_cell = _bcm_bst_th_byte_to_cell;
    bst_info->to_byte = _bcm_bst_th_cell_to_byte;
    bst_info->control_set = _bcm_th_bst_control_set;
    bst_info->control_get = _bcm_th_bst_control_get;
    bst_info->intr_to_resources = _bcm_bst_th_intr_to_resources;
    bst_info->hw_stat_snapshot = _bcm_bst_th_sync_hw_snapshot;
    bst_info->hw_stat_clear = _bcm_bst_th_hw_stat_clear;
    bst_info->intr_enable_set = _bcm_bst_th_intr_enable_set;
    bst_info->pre_sync = NULL;
    bst_info->post_sync = NULL;
    bst_info->hw_intr_cb = soc_th_set_hw_intr_cb;
    bst_info->port_to_mmu_inst_map = _bst_th_port_to_mmu_inst_map_get;
    bst_info->bst_cmn_io_op = _bst_th_cmn_io_op;
    bst_info->profile_init = _bst_th_profile_init;
    bst_info->thd_get = _bst_th_cmn_profile_get_hw;
    bst_info->thd_set = _bst_th_cmn_profile_set_hw;;
    bst_info->hw_sbusdma_desc_init = _bcm_bst_th_sbusdma_desc_init;
    bst_info->hw_sbusdma_desc_deinit = _bcm_bst_th_sbusdma_desc_deinit;
    bst_info->hw_sbusdma_desc_sync = _bcm_bst_th_sbusdma_desc_sync;
    bst_info->max_bytes = bst_info->to_byte(unit, max_thd);

#ifdef BCM_TOMAHAWK2_SUPPORT
    /* on TH2, enable HW Clear on Read for HWM mode by default */
    if (SOC_IS_TOMAHAWK2(unit) &&
        soc_property_get(unit, "hw_cor_in_max_use_count", TRUE)) {
        BCM_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval,
                          BST_CLEAR_ON_READ_IN_MAX_USE_COUNT_MODE_ENABLEf, 0x1);
        BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
        bst_info->hw_cor_in_max_use_count = 1;
    } else
#endif
    {
        bst_info->hw_cor_in_max_use_count = 0;
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
    total_thd_count = NUM_XPE(unit) * idx_count; /* per xpe copy */
    _BCM_BST_THRESHOLD_INFO(pres, INVALIDm, CFAPBSTTHRSr, BST_THRESHOLDf,
                            granularity);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst cfap");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
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
    idx_count = _TH_MMU_NUM_POOL;
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
    total_thd_count = NUM_XPE(unit) * idx_count;
    thd_reg = THDI_BST_SP_GLOBAL_SHARED_PROFILEr;
    thd_field = SP_GLOBAL_SHARED_PROFILEf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, thd_reg)[i], thd_field, granularity, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst ing pool");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
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
    total_thd_count = NUM_XPE(unit) * NUM_PIPE(unit) * idx_count;
    thd_mem = THDI_PORT_SP_CONFIGm;
    thd_field = SP_BST_PROFILE_SHAREDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    prof_reg = THDI_BST_SP_SHARED_PROFILEr;
    prof_field = BST_SP_SHARED_THRESHf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_PROFILE_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, prof_reg)[i], prof_field, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst port pool");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
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
    total_thd_count = idx_count * NUM_XPE(unit) * NUM_PIPE(unit);
    thd_mem = THDI_PORT_PG_CONFIGm;
    thd_field = PG_BST_PROFILE_SHAREDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    prof_reg = THDI_BST_PG_SHARED_PROFILEr;
    prof_field = BST_PG_SHARED_THRESHf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_PROFILE_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, prof_reg)[i], prof_field, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg");
    /* Stat info count and Resource allocation */
    /* Stat res - THDI_PORT_PG_BST : (Num Ports (32/pipe) * 4 pipes) * 8(PGs) */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
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
    total_thd_count = idx_count * NUM_XPE(unit) * NUM_PIPE(unit);
    thd_mem = THDI_PORT_PG_CONFIGm;
    thd_field = PG_BST_PROFILE_HDRMf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    prof_reg = THDI_BST_PG_HDRM_PROFILEr;
    prof_field = BST_PG_HDRM_THRESHf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_PROFILE_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, prof_reg)[i], prof_field, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst pg hdrm");
    /* Stat info count and Resource allocation */
    /* Stat res - THDI_PORT_PG_BST : (Num Ports (32/pipe) * 4 pipes) * 8(PGs) */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
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
    pres->threshold_adj = 0;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MCUC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    thd_reg = MMU_THDM_DB_POOL_MCUC_BST_THRESHOLDr;
    thd_field = BST_STATf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, thd_reg)[i], thd_field, granularity, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst egrpool");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
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
    pres->threshold_adj = 0;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDM_DB_POOL_MC_BST_STATr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    thd_reg = MMU_THDM_DB_POOL_MC_BST_THRESHOLDr;
    thd_field = BST_STATf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, thd_reg)[i], thd_field, granularity, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst egr mcpool");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count; /* No per pipe copy */
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
    total_thd_count = NUM_XPE(unit) * NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDM_DB_QUEUE_CONFIGm;
    thd_field = BST_THRESHOLD_PROFILEf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    prof_reg = MMU_THDM_DB_QUEUE_MC_BST_THRESHOLD_PROFILEr;
    prof_field = BST_STATf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_PROFILE_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, prof_reg)[i], prof_field, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst mcast");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
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
            SOC_REG_UNIQUE_ACC(unit, thd_reg)[i/2], thd_field, granularity, i);
    }

    prof_reg = MMU_THDM_DB_PORTSP_BST_THRESHOLDr;
    prof_field = BST_STATf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_PROFILE_INFO_N(pres, INVALIDm, 
            SOC_REG_UNIQUE_ACC(unit, prof_reg)[i], prof_field, i);
    }
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count,
                                     "bst egr port pool shared mcast");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
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
    total_thd_count = NUM_XPE(unit) * NUM_PIPE(unit) * idx_count;
    thd_mem = MMU_THDU_CONFIG_QUEUEm;
    thd_field = Q_WM_MAX_THRESHOLDf;
    for (i = 0; i < NUM_PIPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, SOC_MEM_UNIQUE_ACC(unit, thd_mem)[i],
                                  INVALIDr, thd_field, granularity, i);
    }
    _BCM_BST_PROFILE_INFO(pres, INVALIDm, OP_UC_QUEUE_BST_THRESHOLDr, BST_STATf);
    _BCM_BST_RESOURCE_THRESHOLD_INIT(pres, total_thd_count, "bst ucast");
    /* Stat info count and Resource allocation */
    total_stat_count = total_thd_count;
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
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
    stat_mem = MMU_THDU_BST_PORTm;
    stat_reg = INVALIDr;
    stat_field = Q_COUNTf;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, stat_mem, 0, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

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
    _BCM_BST_STAT_INFO(pres, stat_mem, stat_reg, stat_field);
    _BCM_BST_RESOURCE_STAT_INIT(pres, total_stat_count,
                                "bst egr port pool shared ucast");
    pres->index_min = 0;
    pres->index_max = total_stat_count - 1;

    /* bcmBstStatIdRQEQueue */
    pres = _BCM_BST_RESOURCE(unit, bcmBstStatIdRQEQueue);
    _BCM_BST_STAT_THRESHOLD_INFO_INIT(pres);
    _BCM_BST_RESOURCE_FLAG_SET(pres, _BCM_BST_CMN_RES_F_RES_EGR);
    pres->num_instance = NUM_XPE(unit);
    idx_count = 11;
    stat_mem = INVALIDm;
    stat_reg = MMU_THDR_DB_BST_STAT_PRIQr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    thd_reg = MMU_THDR_DB_BST_THRESHOLD_PRIQr;
    thd_field = BST_THRf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, INVALIDm,
            SOC_REG_UNIQUE_ACC(unit, thd_reg)[i], thd_field, granularity, i);
    }
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
    stat_mem = INVALIDm;
    stat_reg = MMU_THDR_DB_BST_STAT_SPr;
    stat_field = BST_STATf;

    /* Threshold info count and Resource allocation */
    granularity = 1;
    total_thd_count = NUM_XPE(unit) * idx_count; /* No per pipe copy */
    thd_reg = MMU_THDR_DB_BST_THRESHOLD_SPr;
    thd_field = BST_THRf;
    for (i = 0; i < NUM_XPE(unit); i++) {
        _BCM_BST_THRESHOLD_INFO_N(pres, INVALIDm,
            SOC_REG_UNIQUE_ACC(unit, thd_reg)[i], thd_field, granularity, i);
    }
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

    thd_mem = MMU_THDU_CONFIG_QGROUPm;
    thd_field = Q_WM_MAX_THRESHOLDf;
    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, thd_mem, 0);
    idx_count = soc_mem_index_max(unit, child_mem) + 1;

    /* Threshold info count and Resource allocation */
    granularity = 8; /* UC queue group Thd is 8 cell granularity */
    total_thd_count = NUM_XPE(unit) * NUM_PIPE(unit) * idx_count;
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
STATIC int
_bst_th_sbusdma_desc_setup(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_tbl_ctrl_t *tbl_ctrl;
    int blk, bid, pipe, mi, pipe_factor;
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

        tbl_ctrl = sal_alloc(sizeof(_bcm_bst_tbl_ctrl_t), "bst_tbl_ctrl");
        if (tbl_ctrl == NULL) {
            return BCM_E_MEMORY;
        }
        pipe_factor = 1;
        if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            pipe_factor = NUM_PIPE(unit)/2;
        }
        size = 0;
        tbl_ctrl->tindex = bid;
        tbl_ctrl->entries = resInfo->num_instance * pipe_factor;
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

            mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, 0, 0);
            if (mem == INVALIDm) {
                return BCM_E_PARAM;
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
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
        }
        buff += size;
        tbl_ctrl->size += size;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th_sbusdma_desc_init(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    int pipe, bid, mi;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    uint32 entries, width, size = 0;
    int xpe = 0;
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

        base_mem = resInfo->stat_mem[0];
        reg = resInfo->stat_reg[0];
        if (reg != INVALIDr) {
            for (xpe = 0; xpe < resInfo->num_instance; xpe++) {
                entries = resInfo->num_stat_pp / resInfo->num_instance;
                width = BYTES2WORDS(soc_reg_bytes(unit, reg));
                size += entries * width * 4;
                mi++;
            }
        } else if (base_mem != INVALIDm) {
            int idx_count = 0;

            mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, 0, 0);
            if (mem == INVALIDm) {
                return BCM_E_PARAM;
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
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
    rv =_bst_th_sbusdma_desc_setup(unit);
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
_bst_th_sw_sbusdma_desc_sync(int unit, int bid)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    _bcm_bst_resource_info_t *resInfo;
    _bcm_bst_tbl_ctrl_t *tbl_ctrl;
    int idx, idx_offset, xpe, xpe_offset, pipe, pipe_offset = 0;
    void *pentry;
    uint32 rval, temp_val;
    uint8 *buff = NULL;
    soc_mem_t mem, base_mem;
    soc_reg_t reg;
    uint32 width = 0;
    bcm_gport_t gport = -1;
    bcm_cos_t cosq = -1;
    int port = -1;

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
        pipe_offset = 0;
        for (idx = 0; idx < resInfo->num_stat_pp; idx++) {
            rval = 0;
            width = BYTES2WORDS(soc_reg_bytes(unit, reg));
            idx_offset = idx + pipe_offset;
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
        int max_idx = 0;
        int stats_per_pipe = 0; /* stats per pipe per mem-instance(xpe) */
        int stats_per_inst = 0;

        /* Get the mem attributes(Max idx) from the first child,
         * which could be used for other instances.
         * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
         */
        mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, 0, 0);

        if (mem == INVALIDm) {
            sal_mutex_give(bst_info->bst_reslock);
            return BCM_E_PARAM;
        }
        stats_per_inst = (resInfo->index_max + 1) / resInfo->num_instance;
        stats_per_pipe = stats_per_inst / NUM_PIPE(unit);
        max_idx = soc_mem_index_max(unit, mem);
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            xpe_offset = 0;
            gport = BCM_GPORT_INVALID;
            cosq = -1;
            if (xpe) {
                xpe_offset = stats_per_inst * xpe;
            }
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                pipe_offset = 0;
                if (pipe) {
                    pipe_offset = stats_per_pipe * pipe;
                }
                mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);
                if (mem == INVALIDm) {
                    continue;
                }

                for (idx = 0; idx < max_idx; idx++) {
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
                        /* coverity[parameter_hidden: FALSE] */
                        int port, pool;
                        port = idx % _TH_MMU_PORTS_PER_PIPE;
                        pool = idx / _TH_MMU_PORTS_PER_PIPE;
                        idx_offset = port * _TH_MMU_NUM_POOL + pool +
                                     pipe_offset + xpe_offset;
                    } else {
                        idx_offset = idx + pipe_offset + xpe_offset;
                    }
                    pentry = soc_mem_table_idx_to_pointer(unit, mem, void*,
                                                          buff, idx);
                    temp_val = soc_mem_field32_get(unit, mem, pentry,
                                                   resInfo->stat_field);
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
                buff += tbl_ctrl->size / tbl_ctrl->entries;
            }
        }
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_bst_th_sbusdma_desc_sync(int unit)
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

        BCM_IF_ERROR_RETURN(_bst_th_sw_sbusdma_desc_sync(unit, bid));
    }

    if (enable && bst_info->control_set) {
        bst_info->control_set(unit, bcmSwitchBstEnable, enable,
                              _bcmBstThreadOperInternal);
    }

    sal_mutex_give(bst_info->bst_reslock);
    return BCM_E_NONE;
}

STATIC int
_bcm_bst_th_sbusdma_desc_deinit(int unit)
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

int _bcm_th_bst_sync_thread_restore(int unit)
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

int _bcm_th_bst_info_restore(int unit)
{
    _bcm_bst_cmn_unit_info_t *bst_info;
    soc_field_t snap_en[] = {BST_HW_SNAPSHOT_EN_THDOf,
                             BST_HW_SNAPSHOT_EN_THDIf,
                             BST_HW_SNAPSHOT_EN_CFAPf
                            };
    uint32 rval, enable;
    uint32 i;

    bst_info = _BCM_UNIT_BST_INFO(unit);

    BCM_IF_ERROR_RETURN(READ_MMU_GCFG_MISCCONFIGr(unit, &rval));
    bst_info->track_mode = soc_reg_field_get(unit, MMU_GCFG_MISCCONFIGr, rval,
                               BST_TRACKING_MODEf);

    bst_info->snapshot_mode = 0;
    BCM_IF_ERROR_RETURN(READ_MMU_GCFG_BST_HW_SNAPSHOT_ENr(unit, &rval));
    for (i = _BCM_BST_SNAPSHOT_THDO; i < _BCM_BST_SNAPSHOT_COUNT; i++) {
        enable = soc_reg_field_get(unit, MMU_GCFG_BST_HW_SNAPSHOT_ENr, rval,
                                   snap_en[i]);
        if (enable) {
            bst_info->snapshot_mode |= 0x1 << i;
        }
    }

    return BCM_E_NONE;
}

int _bcm_th_bst_resource_threshold_restore(int unit)
{
    _bcm_bst_resource_info_t *resInfo;
    int hw_index, index_max;
    bcm_bst_stat_id_t bid;
    uint32 cells, val;
    uint32 profile_index;
    int pipe_factor, pipe_offset, offset_per_xpe, num_instance;
    int i, cnt, xpe_id, pipe_id;
    const _bcm_bst_id_map_t *map, *map_tmp;

    for (bid = _BCM_BST_RESOURCE_MIN; bid < _BCM_BST_RESOURCE_MAX; bid++) {
        resInfo = _BCM_BST_RESOURCE(unit, bid);
        if ((resInfo == NULL) || !_BCM_BST_RESOURCE_VALID(resInfo)) {
            continue;
        }
        if (bid == bcmBstStatIdHeadroomPool) {
            /* Do not have BST profile. */
            continue;
        }

        num_instance = (resInfo->num_instance > 1) ? resInfo->num_instance : 1;
        offset_per_xpe = (resInfo->index_max + 1) / num_instance;
        if (resInfo->flags & _BCM_BST_CMN_RES_F_PIPED) {
            pipe_factor = NUM_PIPE(unit);
            pipe_offset = offset_per_xpe / pipe_factor;
        } else {
            pipe_factor = 1;
            pipe_offset = 0;
        }

        index_max = (resInfo->index_max + 1) / (num_instance * pipe_factor);

        BCM_IF_ERROR_RETURN(_bst_th_id_map_get(unit, bid, &map, &cnt));

        for (hw_index = resInfo->index_min; hw_index < index_max; hw_index++) {
            for (i = 0, map_tmp = map; i < cnt; i++, map_tmp++) {
                xpe_id = (map_tmp->xpe < 0) ? 0 : map_tmp->xpe;
                pipe_id = (map_tmp->pipe < 0) ? 0 : map_tmp->pipe;

                SOC_IF_ERROR_RETURN(
                   _bst_th_cmn_io_op(unit, BST_OP_GET | BST_OP_THD,
                                      pipe_id, hw_index, bid,
                                      resInfo->threshold_mem[pipe_id],
                                      resInfo->threshold_reg[map_tmp->tid],
                                      resInfo->threshold_field,
                                      &val));
                if (resInfo->flags & _BCM_BST_CMN_RES_F_PROFILEID) {
                    profile_index = val;
                    SOC_IF_ERROR_RETURN(
                       _bst_th_cmn_io_op(unit, BST_OP_GET,
                                          pipe_id, profile_index, bid,
                                          resInfo->profile_mem,
                                          resInfo->profile_reg[map_tmp->pregid],
                                          resInfo->profile_field,
                                          &cells));
                } else {
                    cells = val;
                }

                resInfo->p_threshold
                    [hw_index + pipe_offset * pipe_id + xpe_id * offset_per_xpe]
                                        = cells;
            }
        }
    }

    return BCM_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */

