/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Field Processor APIs
 *
 * Purpose:
 *     'Field Processor' (FP) API for Dune Packet Processor devices
 *     TCAM suppot
 */

/*
 *  The functions in this module deal with TCAM feature on PPD devices. They
 *  are invoked by the main field module when certain things occur, such as
 *  calls that explicitly involve TCAM work.
 */

/*
 *  Everything in this file assumes it is safely called behind a lock, except
 *  the last section, functions exposed at the BCM API layer.
 */

/*
 *  Exported functions have their descriptions in the field_int.h file.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FP

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/stack.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/common/multicast.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_metering.h>
#include <soc/dpp/TMC/tmc_api_pmf_low_level_pgm.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>
#include <soc/hwstate/hw_log.h>
#include <soc/dpp/ARAD/arad_tcam.h>

/*
 *  For now, if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT is FALSE, disable warm boot
 *  support here.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#if !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#undef BCM_WARM_BOOT_SUPPORT
#endif  /* BCM_WARM_BOOT_SUPPORT */
#endif /* !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT */

/* The default egress trap profile is a "do-nothing" action profile */
#define _BCM_PETRA_FIELD_EGR_TRAP_PROFILE_DEFAULT 0
/* 
 * In Arad, the counting is done via the ACE-Pointer table. 
 * The final counter-id = ACE-Pointer (line id) 
 * For lines < 1K, both PP-Ports and Out-LIFs are changed. These lines are not used 
 * in current implementations, thus the Counter-ID must be superior to 1K 
 * The last line is invalid. 
 * The Driver reserves the last lines for redirection when not counting.
 */
#define _BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD 1024
#define _BCM_PETRA_CTR_MAX_VALUE_EGRESS_ARAD  (4096 - 256 - 1)  /* table size minus static lines for Redirect */

#define _BCM_DPP_FIELD_TCAM_PARA_GET(_index, _bcmIndex) \
    ((_index == 0) ? propActs[_bcmIndex].bcmParam0 : ((_index == 1) ? propActs[_bcmIndex].bcmParam1 : ((_index == 2) ? propActs[_bcmIndex].bcmParam2 : propActs[_bcmIndex].bcmParam3)))

int _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(bcm_dpp_field_info_OLD_t *unitData, SOC_PPC_FP_ENTRY_INFO *entryInfo, _bcm_dpp_field_ent_idx_t entry, uint8 isExternalTcam, unsigned int entryMaxQuals, _bcm_dpp_field_stage_idx_t stage);
int _bcm_dpp_field_tcam_entry_remove_by_key(bcm_dpp_field_info_OLD_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry);
int _bcm_dpp_field_tcam_entry_action_get_by_key(bcm_dpp_field_info_OLD_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry,
                                                 bcm_field_action_t type,uint32 *param0,uint32 *param1);

/******************************************************************************
 *
 *  Local functions and data
 */


/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_hardware_remove
 *  Purpose
 *     Remove a soc_petra PPD entry based upon the BCM field entry information
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) backingStore = TRUE to update backing store, FALSE otherwise
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Assumes entry validity checks have already been performed.
 *
 *     If swapping new entry in, this must be called after the new entry is
 *     installed but before the entry descriptor has been updated to reflect
 *     the new entry information.
 *
 *     Will complete removal process and return last error if there is one.
 */
STATIC int
_bcm_dpp_field_tcam_entry_hardware_remove(bcm_dpp_field_info_OLD_t *unitData,
                                          uint8 isExternalTcam,
                                          uint8 is_kaps_advance_mode,
                                          uint8 is_kaps_sw_remove,
                                          _bcm_dpp_field_grp_idx_t group,
                                          _bcm_dpp_field_ent_idx_t entry,
                                          int backingStore)
{
    uint32 ppdr;
    int result = BCM_E_NONE;
    int auxRes = BCM_E_NONE;
    uint32 entryFlags;
    int is_sw_remove_only = 0;
    uint32          internal_table_id;
    uint32 groupFlags;
    uint32 entry_hwHandle;
    uint32 egrTrapProfile;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.internal_table_id.get(unit, group, &internal_table_id));
    /* when updating existing entry (backingStore == FALSE), after installing the new entry we need to delete the old entry */

    /* in case we are updating entry for extended database that added to ELK-LPM we don't need to delete the exising entry from HW - it is updated in the HW directly */
    if(backingStore == FALSE){
        if ((groupFlags & _BCM_DPP_FIELD_GROUP_EXTENDED_DATABASES) && 
            ((internal_table_id == _BCM_DPP_INTERNAL_TABLE_ID_EXTENDED_IPV6) || 
             (internal_table_id == _BCM_DPP_INTERNAL_TABLE_ID_INRIF_MAPPING) || 
             (internal_table_id == _BCM_DPP_INTERNAL_TABLE_ID_EXTENDED_P2P)))
        {
            is_sw_remove_only = 1;
        }
    }

    if ((is_kaps_advance_mode == TRUE) && (is_kaps_sw_remove == FALSE)) {
        is_sw_remove_only |= ARAD_TCAM_REMOVE_KAPS_ADVANCED_MODE;
        ppdr = soc_ppd_fp_entry_remove(unitData->unitHandle,
                                       group,
                                       entry, is_sw_remove_only);
        auxRes = handle_sand_result(ppdr);
        if (BCM_E_NONE != auxRes) {
            result = auxRes;
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to remove unit %d group %u entry %u %d (%s)\n"),
                       unit,
                       group,
                       entry,
                       result,
                       _SHR_ERRMSG(result)));
        }

    } else {
        _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, isExternalTcam, entry, entryFlags);
        if ((entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW))
        {
            _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit, isExternalTcam, entry, entry_hwHandle);
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u remvoving"
                                  " hardware entry %u\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       entry_hwHandle));
            if (entryFlags & _BCM_DPP_FIELD_ENTRY_KEEP_LOCATION) {
                is_sw_remove_only |= ARAD_TCAM_REMOVE_KEEP_LOCATION; /*bit1 indicates this entry is keep location when remove*/
            }

            if (is_kaps_sw_remove == FALSE) {
                ppdr = soc_ppd_fp_entry_remove(unitData->unitHandle,
                                               group,
                                               entry_hwHandle, is_sw_remove_only);
                auxRes = handle_sand_result(ppdr);
                if (BCM_E_NONE != auxRes) {
                    result = auxRes;
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unable to remove unit %d group %u entry %u"
                                          " (hw entry %u) from hardware: %d (%s)\n"),
                               unit,
                               group,
                               _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                               entry_hwHandle,
                               result,
                               _SHR_ERRMSG(result)));
                }
            }

            if (groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY) {
                /* in this case we don't need to release the HW handle because we didn't alloc new one */
                auxRes = BCM_E_NONE;
            } else if (!(entryFlags & _BCM_DPP_FIELD_ENTRY_KEEP_LOCATION) && (is_kaps_sw_remove == FALSE)) {
                auxRes = dpp_am_res_free(unit,
                                        BCM_DPP_AM_DEFAULT_POOL_IDX,
                                        dpp_am_res_field_entry_id,
                                        1 /* count */,
                                        entry_hwHandle);
                if (BCM_E_NONE != auxRes) {
                    result = auxRes;
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d unable to return hardware entry"
                                          " handle %u to free list: %d (%s)\n"),
                               unit,
                               entry_hwHandle,
                               result,
                               _SHR_ERRMSG(result)));
                }
            }
            if(!isExternalTcam) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.egrTrapProfile.get(unit, entry, &egrTrapProfile));
                if (entryFlags & _BCM_DPP_FIELD_ENTRY_EGR_PRO) {
                    if(egrTrapProfile > 0) {
                        /* Free only dynamically allocated profiles, profile 0 is a special default case allocated at init */
                        auxRes = _bcm_dpp_am_template_trap_egress_free(unit, egrTrapProfile);
                    }
                }
                if (BCM_E_NONE != auxRes) {
                    result = auxRes;
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d unable to return hardware egress"
                                          " trap profile %d to free list: %d (%s)\n"),
                               unit,
                               egrTrapProfile,
                               result,
                               _SHR_ERRMSG(result)));
                }
            }

            /* this entry is no longer in hardware */
            entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_IN_HW |
                                                 _BCM_DPP_FIELD_ENTRY_EGR_PRO));
           _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit, isExternalTcam, entry, entryFlags);
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
        if (backingStore) {
            /* update backing store */
            _bcm_dpp_field_tcam_entry_wb_save(unitData, entry, isExternalTcam, NULL, NULL);
        }
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
        } /* if (entryData->entryCmn.entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) */
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_ppd_fp_entry_get(bcm_dpp_field_info_OLD_t  *unitData,
                      _bcm_dpp_field_ent_idx_t  entry,
                      uint32                    groupD_hwHandle,
                      uint32                    hwHandle,
                      uint8                     *okay,
                      SOC_PPC_FP_ENTRY_INFO     *entInfoTc)
{
    int unit = unitData->unitHandle;

    return soc_ppd_fp_entry_get(unit,
                                groupD_hwHandle,
                                hwHandle,
                                okay,
                                entInfoTc);
}

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_hardware_install
 *  Purpose
 *     Insert a Soc_petra PPD entry based upon the BCM field entry information
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (out)new_entry = large direct loop entry ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Assumes entry validity checks have already been performed.
 *
 *     Will only work if the group is already in hardware.
 *
 *     Will destroy and recreate the hardware entry if it already exists.
 */
STATIC int
_bcm_dpp_field_tcam_entry_hardware_install(bcm_dpp_field_info_OLD_t *unitData,
                                           uint8 isExternalTcam,
                                           _bcm_dpp_field_ent_idx_t entry,
                                           bcm_field_entry_t *new_entry)
{
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO egTrapInfo;
    SOC_PPC_FP_ENTRY_INFO *entryInfo = NULL;
    uint32 ppdr;
    uint32 newFlags;
    int egrTrapProfile = -1;
    uint32 newHwHandle;
    SOC_SAND_SUCCESS_FAILURE success;
    unsigned int index;
    unsigned int offset;
    int result = BCM_E_NONE;
    int auxRes;
    uint32 entryFlags;
    _bcm_dpp_field_grp_idx_t group;
    uint32 groupD_groupFlags;
    _bcm_dpp_field_stage_idx_t groupD_stage;
    int32 newHwHandle_was_allocated ;
    uint32 num;
    bcm_field_group_mode_t groupD_grpMode;
    uint8 is_kaps_advanced_mode = FALSE;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    newHwHandle_was_allocated = 0 ;
    _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, group);

    /* may need to deal with action profiles, so prepare for it */
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&egTrapInfo);

    /* keep old flags and handle around */
    _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, isExternalTcam, entry, entryFlags);
    newFlags = entryFlags;
    
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupD_groupFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, group, &groupD_grpMode));
    if ((new_entry != NULL) &&
        (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_LARGE) &&
        (groupD_grpMode == bcmFieldGroupModeDirect)) {
        is_kaps_advanced_mode = TRUE;
    }

    if ((groupD_groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY) || (is_kaps_advanced_mode == TRUE)) {
        /* when adding by key we dont need newHwHandle */
        newHwHandle = 0;
    }else { /* pick a new hardware entry handle */
        if (entryFlags & _BCM_DPP_FIELD_ENTRY_KEEP_LOCATION) {
            _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit, isExternalTcam, entry, newHwHandle);
        } else {
            result = dpp_am_res_alloc(unit,
                                        BCM_DPP_AM_DEFAULT_POOL_IDX,
                                        dpp_am_res_field_entry_id,
                                        0 /* flags */,
                                        1 /* count */,
                                        (int *)&newHwHandle);
            if (BCM_E_NONE != result) {
                BCMDNX_IF_ERR_EXIT_MSG(result,
                                    (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate"
                                                      " hardware handle for entry %u:"
                                                      " %d (%s)"),
                                     unit,
                                     _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                                     result,
                                     _SHR_ERRMSG(result)));
            }
            newHwHandle_was_allocated = 1 ;
        }
    }

    BCMDNX_ALLOC(entryInfo, sizeof(SOC_PPC_FP_ENTRY_INFO), "_bcm_dpp_field_tcam_entry_hardware_install.entryInfo");
    if(entryInfo == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("Memory allocation failure")));
    }
    /* build entry qualifier data from sw state */
    SOC_PPC_FP_ENTRY_INFO_clear(entryInfo);

    _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_GET(unit, isExternalTcam, entry, (entryInfo->priority));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &groupD_stage));

    result = _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(unitData, entryInfo, entry, isExternalTcam, _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxQuals, groupD_stage);

    /* build entry action data from sw state */
    for (index = 0, offset = 0;
         index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX;
         index++) {
         /* was 
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType)) {
        */
        uint32 hwType;
        uint32 hwFlags;
        uint32 hwParam;
        _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_GET(unit, isExternalTcam, entry, index, hwType);
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(hwType)) {
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWFLAGS_GET(unit, isExternalTcam, entry, index, hwFlags);
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWPARAM_GET(unit, isExternalTcam, entry, index, hwParam);
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u PPD action %d %s(%d)"
                                  " %08X %08X\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       index,
                       SOC_PPC_FP_ACTION_TYPE_to_string(hwType),
                       hwType,
                       hwFlags,
                       hwParam));
            switch (hwType) {
            case SOC_PPC_FP_ACTION_TYPE_EGR_TRAP:
                egTrapInfo.bitmap_mask |= hwFlags;
                if (hwFlags &
                    SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST) {
                    egTrapInfo.out_tm_port = hwParam;
                    if (hwParam != SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID) {
                        /* Avoid Drop action since its destination is not regular (discard) */
                        egTrapInfo.header_data.dsp_ptr = hwParam;
                     }
                }
                if (hwFlags &
                    SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP) {
                    egTrapInfo.cos_info.dp = hwParam;
                }
                if (hwFlags &
                    SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD) {
                        egTrapInfo.header_data.cud = hwParam;
                }
                if (hwFlags ==
                    SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_NONE) {
                    /* Used for Trap overwrite (cancel) action: indicate this is the case */
                    egTrapInfo.bitmap_mask = SOC_PPC_TRAP_EG_NO_ACTION;
                }
                if (hwFlags &
                    (~(SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST |
                       SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP |
                       SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD |
                       SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_NONE))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                       " includes unknown"
                                                       " flags %08X on egress"
                                                       " trap action"),
                                      unit,
                                      _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                                      hwFlags &
                                      (~(SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST |
                                         SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP |
                                         SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD))));
                }
                break;
            default:
                /* for most actions, the flags are ignored */
                entryInfo->actions[offset].type = hwType;
                entryInfo->actions[offset].val = hwParam;
                offset++;
            }
        } /* if (SOC_PPC_NOF_FP_ACTION_TYPES > entryData->tcActP[index].hwType) */
    } /* for (all actions) */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &groupD_stage));

    if (offset > _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxActs) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u group %u entry %u"
                                           " has more action slots in use"
                                           " %d than the stage supports %d"),
                          unit,
                          groupD_stage,
                          group,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          offset,
                          _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxActs));
    }
    while (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
        entryInfo->actions[offset].type = SOC_PPC_FP_ACTION_TYPE_INVALID;
        entryInfo->actions[offset].val = 0;
        offset++;
    }
    /* WARNING: from this point, assume new hardware entry owns resources */
    if (egTrapInfo.bitmap_mask) {
        /*
         *  At least one action uses an egress trap based feature; need to
         *  allocate a trap profile.  We do not 'exchange' it here because we
         *  do not want to affect the existing entry in case of error, so what
         *  we do is grab a new template here and then dispose of the old
         *  template later once the old entry has been removed from hardware.
         */
        if (egTrapInfo.bitmap_mask == SOC_PPC_TRAP_EG_NO_ACTION) {
            /* For the Cancel action, use the empty default profile 0 (allocated at init)*/
            egrTrapProfile = _BCM_PETRA_FIELD_EGR_TRAP_PROFILE_DEFAULT;
        } else {
            /* Need to allocate a new valid profile */
            result = _bcm_dpp_am_template_trap_egress_allocate(unit,
                                                           0 /* flags */,
                                                           &egTrapInfo,
                                                           &egrTrapProfile);
        }
        if (BCM_E_NONE == result) {
            /* indicate egress trap profile is now in use */
            newFlags |= _BCM_DPP_FIELD_ENTRY_EGR_PRO;
            /* add egress trap action to the hardware entry */
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &groupD_stage));
            for (index = 0;
                 index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxActs;
                 index++) {
                if (!_BCM_DPP_FIELD_PPD_ACTION_VALID(entryInfo->actions[index].type)) {
                    /* this slot is open; use it */
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d entry %u placing egress"
                                          " trap profile %d in slot %u\n"),
                               unit,
                               _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                               egrTrapProfile,
                               index));
                    entryInfo->actions[index].type = SOC_PPC_FP_ACTION_TYPE_EGR_TRAP;
                    entryInfo->actions[index].val = (egrTrapProfile << 1) | 1;
                    break;
                } /* the action in this slot is not yet used */
            } /* for (all possible hardware action slots) */
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &groupD_stage));
            if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxActs <= index) {
                result = BCM_E_CONFIG;
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d entry %u has too many hardware"
                                      " actions to add egress trap action\n"),
                           unit,
                           _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
            }
        } else { /* if (BCM_E_NONE == result) */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u unable to allocate egress"
                                  " trap template: %d (%s)\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       result,
                       _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } else { /* if (egTrapInfo.bitmap_mask) */
        newFlags &= (~_BCM_DPP_FIELD_ENTRY_EGR_PRO);
    } /* if (egTrapInfo.bitmap_mask) */

    if (BCM_E_NONE == result) {
        /* add the entry to hardware */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %u inserting"
                              " hardware entry %u\n"),
                   unit,
                   _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                   newHwHandle));
        if ((entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) && !(entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) && !(entryFlags & _BCM_DPP_FIELD_ENTRY_NEW)) {
            if (entryFlags & _BCM_DPP_FIELD_ENTRY_KEEP_LOCATION) {
                entryInfo->is_keep_location = 1;                
                newFlags &= (~_BCM_DPP_FIELD_ENTRY_KEEP_LOCATION);
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.num_entry_in_keep_location.get(unit, group, &num));
                if (num > 0) {
                    num = num - 1;
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.num_entry_in_keep_location.set(unit, group, num));
                } else {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                    "unit %d entry %u wrong to install in keep location mode due to no entry in this mode\n"),
                                    unit,
                                    _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
                }
            } else {
                entryInfo->is_keep_location = 0;
            }
        } else {
            entryInfo->is_keep_location = 0;
        }

        entryInfo->is_for_update = 0;
        entryInfo->is_invalid = 0;
        entryInfo->is_inserted_top = (entryFlags & _BCM_DPP_FIELD_GROUP_INSTALL) ? 1 : 0;
        ppdr = soc_ppd_fp_entry_add(unitData->unitHandle,
                                    group,
                                    newHwHandle,
                                    entryInfo,
                                    &success);
        auxRes = handle_sand_result(ppdr);
        if (BCM_E_NONE == auxRes) {
            result = translate_sand_success_failure(success);
        } else {
            result = auxRes;
        }
        if (BCM_E_NONE == result) {
            /* entry installed */
            
            if (is_kaps_advanced_mode == FALSE) {
                result = _bcm_dpp_field_tcam_entry_hardware_remove(unitData,
                                                                   isExternalTcam,
                                                                   FALSE,
                                                                   FALSE,
                                                                   group,
                                                                   entry,
                                                                   FALSE);
            }

            /* keep the new hardware entry's handle */
            _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_SET(unit, isExternalTcam, entry, newHwHandle);

            if(!isExternalTcam) {
                /* keep the new hardware entry's egress trap profile */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.egrTrapProfile.set(unit, entry, egrTrapProfile));
            }
            /* some things may have affected the flags */
            /* entry is now in hardware */
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupD_groupFlags));
            if (!(groupD_groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY)) {
                newFlags |= _BCM_DPP_FIELD_ENTRY_IN_HW; 
            }
            /* entry is no longer changed since insert & no need to update */
            newFlags &= (~(_BCM_DPP_FIELD_ENTRY_CHANGED |
                                            _BCM_DPP_FIELD_ENTRY_UPDATE));
            /* entry is not new anymore */
            newFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
            /* Clean group install flag */
            newFlags &= (~_BCM_DPP_FIELD_GROUP_INSTALL);
            _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit, isExternalTcam, entry, newFlags);

            BCMDNX_IF_ERR_EXIT(result);

             if (is_kaps_advanced_mode == TRUE) {
                 BCM_FIELD_LARGE_DIRECT_ENTRY_ID_PACK(*new_entry, group, entryInfo->large_direct_lookup_key);
                 BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_destroy(unitData, TRUE, _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
             } else if (isExternalTcam == FALSE) {
                 BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.large_direct_lookup_key.set(unit, entry, entryInfo->large_direct_lookup_key));
                 BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.large_direct_lookup_mask.set(unit, entry, entryInfo->large_direct_lookup_mask));
             }
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
            /* update backing store */
            _bcm_dpp_field_tcam_entry_wb_save(unitData, entry, isExternalTcam, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
        } else { /* if (BCM_E_NONE == result) */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to add unit %d group %u entry %u"
                                  " to hardware: %d (%s)\n"),
                       unit,
                       group,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       result,
                       _SHR_ERRMSG(result)));
            /* clean up resources for failed new entry */
            if(egrTrapProfile > 0){
                auxRes = _bcm_dpp_am_template_trap_egress_free(unit, egrTrapProfile);
                if(auxRes != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit, "_bcm_dpp_am_template_trap_egress_free failed : (%s)\n"),
                               _SHR_ERRMSG(auxRes)));
                }
            }
        } /* if (BCM_E_NONE == result) */
    } /* if (BCM_E_NONE == result) */

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCM_FREE(entryInfo);
    if (_rv != BCM_E_NONE) {
        if (newHwHandle_was_allocated) {
            /*
             * This is an error exit and HW handle resource has been allocated.
             * Free it.
             */
            auxRes =
                dpp_am_res_free(unit,
                    BCM_DPP_AM_DEFAULT_POOL_IDX,
                    dpp_am_res_field_entry_id,
                    1 /* count */,
                    newHwHandle) ;
            if (BCM_E_NONE != auxRes) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d unable to return hardware entry"
                                      " handle %u to free list: %d (%s)\n"),
                           unit,
                           newHwHandle,
                           auxRes,
                           _SHR_ERRMSG(auxRes)));
            }
        }
    }
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  Functions and data shared with other field modules
 */

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_tcam_entry_dump(bcm_dpp_field_info_OLD_t *unitData,
                               uint8 isExternalTcam,
                               _bcm_dpp_field_ent_idx_t entry,
                               const char *prefix)
{
    bcm_field_aset_t actions;
    unsigned int index;
    unsigned int count;
    int result = BCM_E_NONE;
    int dumpedRanges;
    _bcm_dpp_field_stage_idx_t groupD_stage;
    int priority;
    uint32 prev_entry;
    uint32 next_entry;
    uint32 hwHandle;
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    _bcm_dpp_field_grp_idx_t group;
    uint32 flags;
    uint32 soc_sandResult;
    uint8 okay;
    SOC_PPC_FP_ENTRY_INFO *entInfoTc = NULL;
    SOC_PPC_FP_QUAL_VAL *hwQuals;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    sal_memset(&(actions), 0x00, sizeof(actions));
    _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, isExternalTcam, entry, flags);
    _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit, isExternalTcam, entry, hwHandle);
    _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, group);
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    if ((flags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                           _BCM_DPP_FIELD_ENTRY_CHANGED |
                                           _BCM_DPP_FIELD_ENTRY_NEW)) ==
        (_BCM_DPP_FIELD_ENTRY_IN_HW)) {
        BCMDNX_ALLOC(entInfoTc, sizeof(SOC_PPC_FP_ENTRY_INFO), "_bcm_dpp_field_tcam_entry_dump.entInfoTc");
        if(entInfoTc == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("Memory allocation failure")));
        }
        /* entry is in hardware and not changed since */
        SOC_PPC_FP_ENTRY_INFO_clear(entInfoTc);

        if (isExternalTcam == FALSE) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.large_direct_lookup_key.get(unit, entry, &entInfoTc->large_direct_lookup_key));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.large_direct_lookup_mask.get(unit, entry, &entInfoTc->large_direct_lookup_mask));
        }
        soc_sandResult = _bcm_ppd_fp_entry_get(unitData,
                                               entry,
                                               group,
                                               hwHandle,
                                               &okay,
                                               entInfoTc);

        result = handle_sand_result(soc_sandResult);
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch entry %u"
                                              " hardware information (%u):"
                                              " %d (%s)"),
                             unit,
                             entry,
                             hwHandle,
                             result,
                             _SHR_ERRMSG(result)));
        if (!okay) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not okay trying"
                                               " to fetch entry from hardware"),
                              unit,
                              entry));
        }
        hwQuals = &(entInfoTc->qual_vals[0]);
    } else {
        /* entry is not in hardare or has changed */
        hwQuals = NULL;
    }
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
    FIELD_PRINT(("%sEntry "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                 "\n",
                 prefix,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    FIELD_PRINT(("%s  Flags       = %08X\n", prefix, flags));
#if _BCM_DPP_FIELD_DUMP_SYM_FLAGS
    FIELD_PRINT(("%s    %s %s %s %s %s %s %s\n",
                 prefix,
                 (flags & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+USED":"-used",
                 (flags & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw",
                 (flags & _BCM_DPP_FIELD_ENTRY_WANT_HW)?"+WANT_HW":"-want_hw",
                 (flags & _BCM_DPP_FIELD_ENTRY_UPDATE)?"+UPD":"-upd",
                 (flags & _BCM_DPP_FIELD_ENTRY_CHANGED)?"+CHG":"-chg",
                 (flags & _BCM_DPP_FIELD_ENTRY_NEW)?"+NEW":"-new",
                 (flags & _BCM_DPP_FIELD_ENTRY_STATS)?"+STA":"-sta"));
#endif /* _BCM_DPP_FIELD_DUMP_SYM_FLAGS */
    if (flags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        if ((_BCM_DPP_FIELD_ENTRY_IN_HW | _BCM_DPP_FIELD_ENTRY_CHANGED) ==
            (flags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                 _BCM_DPP_FIELD_ENTRY_CHANGED))) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (entry changed since hw write)\n"), prefix));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Group       = "
                 _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            "\n"),
                 prefix,
                 group));
        _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_GET(unit, isExternalTcam, entry, priority)
        LOG_CLI((BSL_META_U(unit,
                            "%s  Priority    = %8d\n"),
                 prefix,
                 priority));
        _BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit, isExternalTcam, entry, prev_entry);
        _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit, isExternalTcam, entry, next_entry);
        LOG_CLI((BSL_META_U(unit,
                            "%s  Prev/Next   = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, prev_entry, isExternalTcam),
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, next_entry, isExternalTcam)));
        if (flags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW entry ID = %08X\n"),
                     prefix,
                     hwHandle));
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW priority = %08X\n"),
                     prefix,
                     priority));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Qualifiers:\n"), prefix));
        count = 0;
        dumpedRanges = FALSE;
        {
            _bcm_dpp_field_qual_t qualData[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] ;

            /*
             * Load full local array 'qualData' from data base and use as input to _bcm_dpp_field_entry_qual_dump()
             * and _bcm_dpp_field_entry_range_dump()
             */
            for (index = 0;
                 (index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                 index++) {
                _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, entry, index, qualData[index]);
            }

            for (index = 0;
                 (BCM_E_NONE == result) &&
                 (index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                 index++) {
                FIELD_ACCESS.groupD.stage.get(unit, group, &groupD_stage);
                result = _bcm_dpp_field_entry_qual_dump(unitData,
                                                        groupD_stage,
                                                        /*
                                                         * Was:
                                                         *   _Bcm_Dpp_Field_Tcam_Entry_Quals(isExternalTcam, entry),
                                                         */
                                                        &qualData[0],
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                                        hwQuals,
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
                                                        index,
                                                        !dumpedRanges,
                                                        prefix);
                if (1 == result) {
                    count++;
                    result = BCM_E_NONE;
                } else if (2 == result) {
                    dumpedRanges = TRUE;
                    count++;
                    result = BCM_E_NONE;
                }
            } /* for (index = 0; index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; index++) */
        }
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; matches all applicable traffic)\n"),
                     prefix));
        }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
        LOG_CLI((BSL_META_U(unit,
                            "%s  BCM actions:\n"), prefix));
#else /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        LOG_CLI((BSL_META_U(unit,
                            "%s  Actions:\n"), prefix));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        count = 0;
        for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) {
            _bcm_dpp_field_tc_b_act_t act_b;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit, isExternalTcam, entry, index, act_b);
            if ((bcmFieldActionCount > act_b.bcmType) && (BCM_FIELD_ENTRY_INVALID != act_b.bcmType)) {
                /* only show action if exists */
                if (!BCM_FIELD_ASET_TEST(actions, act_b.bcmType)) {
                    /* have not displayed this action yet */
                    switch (act_b.bcmType) {
                    /* add any actions that need special formatting */
                    default:
                        LOG_CLI((BSL_META_U(unit,
                                            "%s    %s (%d)\n"),
                                 prefix,
                                 _bcm_dpp_field_action_name[act_b.bcmType],
                                 act_b.bcmType));
                        LOG_CLI((BSL_META_U(unit,
                                            "%s      %08X,%08X\n"),
                                 prefix,
                                 act_b.bcmParam0,
                                 act_b.bcmParam1));
                        count++;
                        break;
                    } /* switch (entryData->qual[index].type) */
                } /* if (not encountered action yet) */
                BCM_FIELD_ASET_ADD(actions, act_b.bcmType);
            } /* if (action exists) */
        } /* for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) */
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; a match will not affect traffic)\n"),
                     prefix));
        }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
        LOG_CLI((BSL_META_U(unit,
                            "%s  PPD actions:\n"), prefix));
        count = 0;
        for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
             /* was 
            if (_BCM_DPP_FIELD_PPD_ACTION_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType)) {
            */
            uint32 hwType;
            uint32 hwFlags;
            uint32 hwParam;
        
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_GET(unit, isExternalTcam, entry, index, hwType);
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWPARAM_GET(unit, isExternalTcam, entry, index, hwParam);
            if (_BCM_DPP_FIELD_PPD_ACTION_VALID(hwType)) {
                /* this action exists; display it */
                switch (hwType) {
                /* add any actions that need special formatting */
                case SOC_PPC_FP_ACTION_TYPE_EGR_TRAP:
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    %s (%d)\n"),
                             prefix,
                             SOC_PPC_FP_ACTION_TYPE_to_string(hwType),
                             hwType));
                _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWFLAGS_GET(unit, isExternalTcam, entry, index, hwFlags);
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      %08X: %08X\n"),
                             prefix,
                             hwFlags,
                             hwParam));
                    break;
                default:
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    %s (%d)\n"),
                             prefix,
                             SOC_PPC_FP_ACTION_TYPE_to_string(hwType),
                             hwType));
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      %08X\n"),
                             prefix,
                             hwParam));
                } /* swtich (entryData->tcActP[index].hwType) */
                count++;
            } /* if (action exists) */
        } /* for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) */
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; a match will not affect traffic)\n"),
                     prefix));
        }
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        BCM_EXIT;
    } else {/* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        LOG_CLI((BSL_META_U(unit,
                            "%s  (entry is not in use)\n"), prefix));
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
exit:
    BCM_FREE(entInfoTc);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

int
_bcm_dpp_field_tcam_entry_install(bcm_dpp_field_info_OLD_t *unitData,
                                  uint8 isExternalTcam,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  bcm_field_entry_t *new_entry)
{
    int result = BCM_E_NONE;
    uint32 orig_flags;
    uint32 entryFlags;
    _bcm_dpp_field_grp_idx_t group;
    uint32 groupD_groupFlags;
    SOC_SAND_OCC_BM_PTR extTcamBmp;
    uint32 ext_info_idx;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
 
    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META("(%d,%d) enter\n"),
                 unit,
                 entry));

    _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, isExternalTcam, entry, entryFlags);
    if (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {        
        /* entry exists */
        orig_flags = entryFlags;
        entryFlags |= (_BCM_DPP_FIELD_ENTRY_WANT_HW |
                                           _BCM_DPP_FIELD_ENTRY_UPDATE);
        _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit, isExternalTcam, entry, entryFlags);
        _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, group);
        result = _bcm_dpp_field_tcam_entry_hardware_install(unitData, isExternalTcam, entry, new_entry);
        if (result == BCM_E_NONE) {
            if (!(orig_flags & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
                /* if entry was not installed in HW, decrement uninstalled TCAM entries counter */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupD_groupFlags));
                 if (!(groupD_groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY)) {
                     if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  allocate sw-state resources */
                         /*
                          * Was:
                          *   unitData->entryUninstalledTcCount--;
                          */
                         _bcm_dpp_field_ent_idx_t    entryUninstalledTcCount ;

                         result = FIELD_ACCESS.entryUninstalledTcCount.get(unit, &entryUninstalledTcCount) ;
                         if (result == BCM_E_NONE) {
                             entryUninstalledTcCount-- ;
                             result = FIELD_ACCESS.entryUninstalledTcCount.set(unit, entryUninstalledTcCount) ;
                         }
                     }
                 }
            }
            if (isExternalTcam  && !(SOC_DPP_CONFIG(unitData->unit)->pp.unbound_uninstalled_elk_entries_number)) {
                /* clear external info for external TCAM */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryExtTc.ext_info_idx.get(unit, entry, &ext_info_idx));
                _bcm_dpp_field_ext_info_entry_clear(unit, ext_info_idx);
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.extTcamBmp.get(unit, &extTcamBmp));
                soc_sand_occ_bm_occup_status_set(unitData->unit, extTcamBmp, ext_info_idx, FALSE);
            }
        }
    } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not in use\n"),
                   unit,
                   entry));
        result = BCM_E_NOT_FOUND;
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d,%d) return %d (%s)\n"),
                 unit,
                 entry,
                 result,
                 _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_remove(bcm_dpp_field_info_OLD_t *unitData,
                                 uint8 is_kaps_sw_remove,
                                 bcm_field_entry_t entry)
{
    int result = BCM_E_NONE;
    uint32 groupD_groupFlags;
    bcm_field_group_mode_t groupD_grpMode;
    uint32 flags = 0;
    _bcm_dpp_field_grp_idx_t group, groupLimit;
    _bcm_dpp_field_ent_idx_t entry_id;
    uint8 isExternalTcam = FALSE, is_kaps_advance_mode = FALSE;
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupLimit.get(unit, &groupLimit));
    
    if (_BCM_DPP_FIELD_ENTRY_IS_LARGE_DIRECT_LOOKUP(entry)) {
        BCM_FIELD_LARGE_DIRECT_ENTRY_ID_UNPACK(entry, group, entry_id);
        is_kaps_advance_mode = TRUE;
    } else {
        isExternalTcam = _BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry);
        entry_id = _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry);
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry_id, group));
        _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, isExternalTcam, entry_id, flags);

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "(%d(%08X),%d) enter (%s,%s)\n"),
                   unit,
                   PTR_TO_INT(unitData),
                   entry_id,
                   (flags & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+IN_USE":"-in_use",
                   (flags & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw"));
    }
    if (groupLimit <= group) {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
    }

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupD_groupFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, group, &groupD_grpMode));

    if (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY) {
        result = _bcm_dpp_field_tcam_entry_remove_by_key(unitData, isExternalTcam, entry);
    }else {
        if ((is_kaps_sw_remove == TRUE) || (flags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
            if (flags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                /* removing entry from HW. increment uninstalled TCAM entries counter */
                if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  allocate sw-state resources */
                    /*
                     * Was:
                     *   unitData->entryUninstalledTcCount++;
                     */
                     _bcm_dpp_field_ent_idx_t    entryUninstalledTcCount ;

                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryUninstalledTcCount.get(unit, &entryUninstalledTcCount));
                    entryUninstalledTcCount++ ;
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryUninstalledTcCount.set(unit, entryUninstalledTcCount));
                }
            }
            if ((!is_kaps_sw_remove) &&
                (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_LARGE) &&
                (groupD_grpMode == bcmFieldGroupModeDirect)) {
                /* KAPS: Install entry with actions=0 to delete it from HW */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_clear_acts(unitData,
                                 FALSE,
                                 entry));
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_hardware_install(unitData, FALSE, entry, NULL));
            }

            /* remove this entry from the PPD */
            result = _bcm_dpp_field_tcam_entry_hardware_remove(unitData, isExternalTcam, is_kaps_advance_mode, is_kaps_sw_remove, group, entry_id, TRUE);
        } else {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d is not in use\n"),
                       unit,
                       entry_id));
            result = BCM_E_NOT_FOUND;
        }
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "(%d(%08X),%u) return %d (%s)\n"),
                   unit,
                   PTR_TO_INT(unitData),
                   entry_id,
                   result,
                   _SHR_ERRMSG(result)));
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_group_hw_refresh(bcm_dpp_field_info_OLD_t *unitData,
                                     bcm_field_group_t group,
                                     int cond)
{
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int updated = 0;
    unsigned int copied = 0;
    unsigned int count = 0;
    int result;
    _bcm_dpp_field_entry_type_t entryType;    
    _bcm_dpp_field_ent_idx_t tcam_entry_limit;
    uint32 groupD_groupFlags;
    bcm_field_group_mode_t groupD_grpMode; 
    _bcm_dpp_field_stage_idx_t groupD_stage; 
    _bcm_dpp_field_ent_idx_t groupD_entryTail;
    _bcm_dpp_field_ent_idx_t groupD_entryHead;
    uint32 flags;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupD_groupFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &groupD_stage));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, group, &groupD_grpMode));
    FIELD_ACCESS.stageD.modeBits.entryType.get(unit, groupD_stage, groupD_grpMode, &entryType);

    if (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_ADD_ENT) {
        /* entries have been added or moved; update phase */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d group %d has entries whose hardware"
                              " priority is not valid/current\n"),
                   unit,
                   group));
        _bcm_dpp_field_group_priority_recalc(unitData, group);
    }
    result = BCM_E_NONE;
    if (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
        /*
         *  Now odd phase, so priorities are all lower than the previous
         *  state (assuming there was one).  Update tail to head so that
         *  new versions of entries will 'stay' in proper order.
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.get(unit, group, &groupD_entryTail));
        for (entry = groupD_entryTail, _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
             (entry < tcam_entry_limit) && (BCM_E_NONE == result);
             count++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, flags);
            if ((!cond) || (flags & _BCM_DPP_FIELD_ENTRY_UPDATE)) {
                /* update this entry */
                if (flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                    /* entry wants to be in hardware */
                    if ((flags & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                        (0 == (flags & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                        /* entry has changed or is not yet in hardware, add it */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                            _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                            entry,
                                                                            NULL);
                    }
                } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
                    /* entry does not want to be in hardware */
                    if (flags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* entry is in hardware, so take it out */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_remove(unitData,
                                                                  FALSE,
                                                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType)));
                    }
                } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
            } else { /* if (update this entry) */
                /* no update to this entry... */
                if (flags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    /* ...but need to shift it to new priority anyway */
                    copied++;
                    result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                        _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                        entry,
                                                                        NULL);
                }
            } /* if (update this entry) */
            _BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entry);
        } /* for (all entries in this group) */
    } else { /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
        /*
         *  Now even phase, so priorities are all higher than the previous
         *  state (assuming there was one).  Update head to tail so that
         *  new versions of entries will 'stay' in proper order.
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &groupD_entryHead));
        for (entry = groupD_entryHead, _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
                (entry < tcam_entry_limit) && (BCM_E_NONE == result);
             count++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, flags);
            if ((!cond) || (flags) & _BCM_DPP_FIELD_ENTRY_UPDATE) {
                if (flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                    /* entry wants to be in hardware */
                    if ((flags & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                        (0 == (flags & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                        /* entry has changed or is not yet in hardware, add it */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                            _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                            entry,
                                                                            NULL);
                    }
                } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
                    /* entry does not want to be in hardware */
                    if (flags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* entry is in hardware, so take it out */
                        updated++;
                        result = _bcm_dpp_field_tcam_entry_remove(unitData,
                                                                  FALSE,
                                                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType)));
                    }
                } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_WANT_HW) */
            } else { /* if (update this entry) */
                /* no update to this entry... */
                if (flags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    /* ...but need to shift it to new priority anyway */
                    copied++;
                    result = _bcm_dpp_field_tcam_entry_hardware_install(unitData,
                                                                        _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                        entry,
                                                                        NULL);
                }
            } /* if (update this entry) */
            _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entry);
        } /* for (all entries in this group) */
    } /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d group %d: %d entries, %d updated,"
                          " %d copied\n"),
               unit,
               group,
               count,
               updated,
               copied));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *  See header on field_int.h
 *  See also 
 */
int
_bcm_dpp_field_ace_entry_add(
    int unit,
    _bcm_dpp_field_tc_b_act_t *propActs,
    int core_id,
    unsigned int ppdIndex,
    _bcm_dpp_field_tc_p_act_t *hwAct,
    unsigned int *actCount_p)
{
    _DPP_FIELD_COMMON_LOCALS;
    unsigned int actCount;
    uint32 stat_id, stat_id_mask, 
        ace_pointer_prge_var_nof_bits,
        ace_pointer_prge_var_in_stat_id_lsb;
    unsigned int proc;
    unsigned int cntr;
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    SOC_SAND_SUCCESS_FAILURE success;
    uint32 pp_port, tm_port;
    int core_lcl;
    uint32 ppdr;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    *actCount_p = 0;
    ace_pointer_prge_var_nof_bits = 10;
    stat_id_mask = 0;
    cntr = 0;
    {
        /*
         * For JERICHO and up only.
         */
        int param0 ;     /* action core-specific paramter No.0 */
        int param1 ;     /* action core-specific paramter No.1 */
        int param2 ;     /* action core-specific paramter No.2 */
        /*
         * Hardware action parameter corresponding to SOC_PPD_FP_ACTION_TYPE_ACE_TYPE
         */
        uint32 AceTypeHwParam = ACE_TYPE_VALUE_NULL;

        param0 = propActs->bcmParam0 ;
        param1 = propActs->bcmParam1 ;
        param2 = propActs->bcmParam2 ;

        if (param0 == BCM_ILLEGAL_ACTION_PARAMETER)
        {
            BCMDNX_ERR_EXIT_MSG(
                    BCM_E_PARAM,
                    (_BSL_BCM_MSG_NO_UNIT(
                    "%s(): Illegal param0(%d) from which statd_id is derived. This is an illegal input! Quit."),
                    __func__,param0)) ;
        }
        else
        {
            if(!(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"lsr_cnt_egress",0)
                &&(param0 == 0) && (param1 == 0) && (param2 == 0)))
            {
                /*
                 * Param0 is legal. Derive stat_id from it.
                 */
                ace_pointer_prge_var_in_stat_id_lsb =
                    (_SHR_FIELD_CTR_PROC_SHIFT_GET(param0) - ace_pointer_prge_var_nof_bits);
                /*
                 * Check the counter section of Stat-ID (LS 17 bits). If these bits in
                 * Stat-ID are all zero, counter is assumed illegal.
                 */
                SHR_BITSET_RANGE(&stat_id_mask, ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits) ;
                stat_id = param0 & (~stat_id_mask) ;
                if (stat_id != 0) 
                {
                    /*
                     * Extract counter id fron 'stat_id'
                     */
                    result = _bcm_dpp_proc_cntr_from_stat(unit,
                                                          stat_id,
                                                          &proc,
                                                          &cntr);
                    if (BCM_E_NONE != result) {
                        BCMDNX_IF_ERR_EXIT(result);
                    }
                }
                else
                {
                    /*
                     * Invalid stat_id, quit with error
                     */
                    BCMDNX_ERR_EXIT_MSG(
                        BCM_E_PARAM,
                            (_BSL_BCM_MSG_NO_UNIT(
                            "Invalid input param0 (stat-id):(0x%08lX)"),
                            (unsigned long)param0)
                    ) ;
                }
                SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
                SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
                if (param2 == BCM_ILLEGAL_ACTION_PARAMETER)
                {
                    /*
                     * Either Counting ONLY or Counting+redirect.
                     * If 'param1' is also 'illegal' then this is a 'counting only' request.
                     * Otherwise, it is a 'Counting+redirect' request.
                     *    param0 = Stat-ID
                     *    param1 = GPORT or port number
                     */
                    if (param1 == BCM_ILLEGAL_ACTION_PARAMETER)
                    {
                        /*
                         * Counting ONLY
                         *    param0 = Stat-ID
                         */
                        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_ONLY ;
                        control_ndx.val_ndx = cntr;
                        SHR_BITCOPY_RANGE(&(control_info.val[0]), 0, (uint32 *)&(param0), 
                                           ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                        /*
                         * Set ACE-Type to '2': Counter only
                         */
                        AceTypeHwParam = ACE_TYPE_VALUE_COUNTER_ONLY ;
                    }
                    else
                    {
                        /*
                         * Param0 and param1 are legal. This is 'Counting+redirect' case.
                         * 'cntr' is already loaded by counter id.
                         */
                        if (!BCM_GPORT_IS_SET(param1))
                        {
                            /*
                             * Param1 is not a Gport - Quit with error
                             */
                            BCMDNX_ERR_EXIT_MSG(
                                BCM_E_PARAM,
                                (_BSL_BCM_MSG_NO_UNIT(
                                " non-GPORT param1(%d) is not valid gport"),
                                param1)) ;
                        }
                        /*
                         * Param1 is a valid Gport. Convert gport to TM-Port and PP-Port
                         */
                        BCMDNX_IF_ERR_EXIT_MSG(
                            _bcm_dpp_field_gport_to_pp_port_tm_port_convert(
                                unit,
                                (bcm_gport_t)param1,
                                &pp_port,
                                &tm_port,
                                &core_lcl),
                            (_BSL_BCM_MSG_NO_UNIT(
                                "unable to get the TM or PP port with gport %08X"),
                                (bcm_gport_t)param1)) ;
                        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT ;
                        control_ndx.val_ndx = cntr ;
                        control_info.val[0] = pp_port ;
                        SHR_BITCOPY_RANGE(&(control_info.val[1]), 0, (uint32 *)&(param0), 
                                          ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                        /*
                         * Set ACE-Type to '1': Counter+redirect
                         */
                        AceTypeHwParam = ACE_TYPE_VALUE_OVERRIDES_CUD ;
                    }
                    ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                                  core_id,
                                                  &control_ndx,
                                                  &control_info,
                                                  &success);
                    result = handle_sand_result(ppdr);
                    if (BCM_E_NONE == result)
                    {
                        result = translate_sand_success_failure(success);
                    }
                    BCMDNX_IF_ERR_EXIT(result) ;
                }
                else
                {
                    /*
                     * Both 'Param0' and 'param2' are legal.
                     * If 'param1' is illegal then do:
                     *   Counting+change LIF
                     * Otherwise do:
                     *   Counting+Redirect+Change-lif
                     */
                    uint32
                        outlif_id;
                    /*
                     * param0 = Stat-ID
                     * param2 = outlif
                     */
                    /*
                     * Both options here redirect to 'outlif'. This is done using table EPNI_ACE_TO_OUT_LIF.
                     * We need to make sure that the index into the table ('cntr', which is loaded into
                     * ACE_POINTER) is within limits of that table (0 -> _BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD).
                     * Also, for some reason, index 'zero' is not allowed.
                     */
                    if ((cntr >= _BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD) || (cntr < 1))
                    {
                        /*
                         * Trying to use an illegal ACE-Pointer for this action, so - error
                         */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                            (_BSL_BCM_MSG_NO_UNIT(
                                " Stat-ID param0(%d) is invalid. It must in range %d..%d\n"),
                                stat_id,
                                1,
                                (_BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD - 1))) ;
                    }
                    if (SOC_IS_QUX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipfpm_enable", 0))
                    {
                        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, param2, (int *)&outlif_id);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                    else
                    {
                        outlif_id = param2;
                    }
                    if (param1 != BCM_ILLEGAL_ACTION_PARAMETER)
                    {
                        /*
                         * 'Param1' is legal: Counting+Redirect+Change-lif
                         */
                        if (!BCM_GPORT_IS_SET(param1))
                        {
                            /*
                             * Param1 is not a Gport - Quit with error
                             */
                            BCMDNX_ERR_EXIT_MSG(
                                BCM_E_PARAM,
                                (_BSL_BCM_MSG_NO_UNIT(
                                "%s(): non-GPORT param1(%d) is not valid gport"),
                                __func__,param1)) ;
                        }
                        /*
                         * Param1 is a valid Gport. Convert gport to TM-Port and PP-Port
                         */
                        BCMDNX_IF_ERR_EXIT_MSG(
                            _bcm_dpp_field_gport_to_pp_port_tm_port_convert(
                                unit,
                                (bcm_gport_t)param1,
                                &pp_port,
                                &tm_port,
                                &core_lcl),
                            (_BSL_BCM_MSG_NO_UNIT(
                                "%s(): unable to get the TM or PP port with gport %08X"),
                                __func__,(bcm_gport_t)param1)) ;
                        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT ;
                        control_ndx.val_ndx = cntr ;
                        control_info.val[0] = pp_port ;
                        SHR_BITCOPY_RANGE(&(control_info.val[1]), 0, (uint32 *)&(param0), 
                                           ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                    }
                    else
                    {
                        /*
                         * 'Param1' is illegal: Counting+change LIF
                         */
                        /*
                         * Handling of param0 was stareted above.
                         * Now complete it.
                         */
                        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_ONLY ;
                        control_ndx.val_ndx = cntr;
                        SHR_BITCOPY_RANGE(&(control_info.val[0]), 0, (uint32 *)&(param0), 
                                           ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                    }
                    ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                                  core_id,
                                                  &control_ndx,
                                                  &control_info,
                                                  &success);
                    result = handle_sand_result(ppdr);
                    if (BCM_E_NONE == result)
                    {
                        result = translate_sand_success_failure(success);
                    }
                    BCMDNX_IF_ERR_EXIT(result) ;
                    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
                    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
                    /*
                     * For ALL cases of 'Both 'Param0' and 'param2' are legal',
                     * handle here param2 which is already loaded in 'outlif_id'
                     */
                    control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_OUT_LIF ;
                    control_ndx.val_ndx = cntr ;
                    control_info.val[0] = outlif_id ;
                    ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                  core_id,
                                  &control_ndx,
                                  &control_info,
                                  &success);
                    result = handle_sand_result(ppdr);
                    if (BCM_E_NONE == result)
                    {
                        result = translate_sand_success_failure(success);
                    }
                    BCMDNX_IF_ERR_EXIT(result);
                    /*
                     * For ALL cases of 'Both 'Param0' and 'param2' are legal',
                     * set ACE-Type to '1'
                     */
                    AceTypeHwParam = ACE_TYPE_VALUE_OVERRIDES_CUD ;
                }
            }
            /*
             * Now issue HW actions to write into TCAM egress PMF result table
             * See also PPDB_A_TCAM_ACTION buffer in bcm88675_a0
             */
            hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_ACE_POINTER ;
            /* In Arad, LSB indicates if valid */
            hwAct[ppdIndex].hwParam = cntr ;
            /*
             * Set counter profile to '1' to indicate 'ACE pointer'.
             */
            hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_COUNTER_PROFILE ;
            if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"lsr_cnt_egress",0)
                &&(param0== 0)&&(param1 == 0)&&(param2 == 0)) {
                hwAct[ppdIndex + 1].hwParam = 0x3; /* Counter-ID = System-Headers-Record.PPH.VSI */
                actCount = 2 ;
            }
            else{
                hwAct[ppdIndex + 1].hwParam = 0x1; /* Counter-ID = ACE-Pointer */
                hwAct[ppdIndex + 2].hwType = SOC_PPC_FP_ACTION_TYPE_ACE_TYPE ;
                hwAct[ppdIndex + 2].hwParam = AceTypeHwParam ;
                /*
                 * Indicate: Three actions are loaded into 'hwAct'
                 */
                actCount = 3 ;
           }
        }
    }
    *actCount_p = actCount;
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *   Function
 *      _bcm_dpp_field_tcam_entry_action_update
 *   Purpose
 *      Take a set of proposed BCM layer actions for an entry, compute the
 *      appropriate PPD layer actions, update the BCM and PPD layer actions for
 *      the entry, and cleanup any resources freed due to removed actions or
 *      changes to the actions.
 *   Parameters
 *      (in) unitData = pointer to unit information
 *      (in) core_id = core to perform action
 *      (in) entry = entry ID
 *      (in) propActs = pointer to array of proposed actions
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      This does not actually commit the entry to hardware, but other
 *      resources the entry references may be committed to hardware while
 *      building the PPD actions for the proposed action set, or may be
 *      released while cleaning up unused resources.
 *
 *      Assumes entry is valid.
 *
 *      It is possible that propActs will be edited to include implied actions
 *      if there are such based upon the actions already in propActs.
 */
STATIC int
_bcm_dpp_field_tcam_entry_action_update(bcm_dpp_field_info_OLD_t *unitData,
                                        uint8 isExternalTcam,
                                        int core_id,
                                        _bcm_dpp_field_ent_idx_t entry,
                                        _bcm_dpp_field_tc_b_act_t *propActs)
{
    bcm_gport_t lookupGport[_BCM_DPP_FIELD_ACTION_CHAIN_MAX];
    bcm_gport_t lookupOutLif[_BCM_DPP_FIELD_ACTION_CHAIN_MAX];
    SHR_BITDCL actHit[_SHR_BITDCLSIZE(_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX)];
     _bcm_dpp_field_tc_p_act_t *hwAct = NULL;
    SOC_PPC_FRWRD_DECISION_INFO *fwdDecision = NULL;
    SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType;
    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format;
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    uint64 dest_edata, mask_edata;
    uint32 ppdr;
    unsigned int bcmIndex;
    unsigned int ppdIndex;
    unsigned int actCount;
    unsigned int index;
    unsigned int proc;
    unsigned int cntr = 0;
    unsigned int bits;
    int result;
    int local;
    int local_offset;
    int meter;
    int group;
    int fec;
    int inLif = 0;
    int base_header;
    int ivec_id;
    int commit = FALSE;
    int mirrorId;
    bcm_module_t myModId;
    bcm_dpp_cosq_vsq_type_t vsqType;
    int vsq_core_id;
    uint8 vsq_is_ocb_only;
    uint8 is_ivec_to_set;
    uint8 is_vlan_format_valid;
    uint32 hwTrapCode = 0, ppdMask = 0;
    uint32 soc_sand_rv;
    int core_lcl;
    _bcm_dpp_field_grp_idx_t groupD_group;
    uint32 groupD_groupFlags;
    _bcm_dpp_field_stage_idx_t groupD_stage;
    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */
    _bcm_dpp_field_group_t *GroupData = NULL;
    uint32 stat_id, stat_id_mask = 0, 
        ace_pointer_prge_var_nof_bits=10,
        ace_pointer_prge_var_in_stat_id_lsb;
    uint32 outlif_id;
    uint32 pp_port, tm_port;
    soc_dpp_config_jer_pp_t *jer_pp_config;
    uint8 is_action_redirect_jer_style = 0;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    jer_pp_config = &(SOC_DPP_JER_CONFIG(unit)->pp);

    BCMDNX_ALLOC(fwdDecision, sizeof(*fwdDecision), "_bcm_dpp_field_tcam_entry_action_update.fwdDecision");
    if (fwdDecision == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    SOC_PPC_FRWRD_DECISION_INFO_clear(fwdDecision);

    BCMDNX_ALLOC(GroupData, sizeof(*GroupData),
        "_bcm_dpp_field_tcam_entry_action_update.GroupData");
    if (GroupData == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
    sal_memset(GroupData, 0, sizeof(*GroupData));

    /* collect needed state information */
    _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, groupD_group);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.get(unit, groupD_group, GroupData));
    /* prepare workspace */
    sal_memset(&(actHit[0]),
               0x00,
               sizeof(actHit[0]) *
               _SHR_BITDCLSIZE(_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX));

    BCMDNX_ALLOC(hwAct, 
                 sizeof(*hwAct)*(SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX +_BCM_DPP_FIELD_ACTION_CHAIN_MAX), 
                 "_bcm_dpp_field_tcam_entry_action_update.hwAct");
    if (hwAct == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    for (ppdIndex = 0;
         ppdIndex < (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX +
                     _BCM_DPP_FIELD_ACTION_CHAIN_MAX);
         ppdIndex++) {
        hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_INVALID;
        hwAct[ppdIndex].hwFlags = 0;
        hwAct[ppdIndex].hwParam = 0;
    }
    /* need local module ID for certain operations */
    BCMDNX_IF_ERR_EXIT_MSG(bcm_petra_stk_my_modid_get(unit, &myModId),
                        (_BSL_BCM_MSG_NO_UNIT("unable to get unit %d module ID"),
                         unit));

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "field_action_redirect_jer_style", 0)) {
        is_action_redirect_jer_style = 1;
    }

    /* convert the BCM actions to PPD actions */
    for (bcmIndex = 0, ppdIndex = 0;
         bcmIndex < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         bcmIndex++) {
        if (SHR_BITGET(actHit, bcmIndex) ||
            (bcmFieldActionCount <= propActs[bcmIndex].bcmType) ||
            (BCM_FIELD_ENTRY_INVALID == propActs[bcmIndex].bcmType)) {
            /* hit this one already or it is not valid/used; skip it */
            continue;
        }
        /* assume this BCM action is one PPD action */
        actCount = 1;
        /* prepare action workspace */
        for (index = 0; index < _BCM_DPP_FIELD_ACTION_CHAIN_MAX; index++) {
            lookupGport[index] = BCM_GPORT_INVALID;
            lookupOutLif[index] = BCM_GPORT_INVALID;
        }
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, groupD_group, &groupD_stage));
        /* convert action according to hardware stage */
        if (SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF ==
                _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).hwStageId) {
            switch (propActs[bcmIndex].bcmType) {
            uint8 cascadedKeyLen;

            case bcmFieldActionCascadedKeyValueSet:
                /* param0 = cascaded key value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.cascadedKeyLen.get(unit, &cascadedKeyLen)) ;
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    if (((~0) << cascadedKeyLen) &
                        propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("param0 %d(%08X) too"
                                                           " big for unit %d"
                                                           " current cascaded"
                                                           " key length %d"),
                                          propActs[bcmIndex].bcmParam0,
                                          propActs[bcmIndex].bcmParam0,
                                          unit,
                                          cascadedKeyLen));
                    }
                } else {
                    /*2bit sel + 2bit operation*/
                    if (((~0) << (cascadedKeyLen+4)) &
                        propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("param0 %d(%08X) too"
                                                           " big for unit %d"
                                                           " current cascaded"
                                                           " key length %d"),
                                          (propActs[bcmIndex].bcmParam0>>4),
                                          (propActs[bcmIndex].bcmParam0>>4),
                                          unit,
                                          cascadedKeyLen));
                    }
                }

                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                break;
            case bcmFieldActionStaggeredPreselProfile0Set:
                /* param0 = presel-profile-val */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_0;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0x3 ) ;
                break;
            case bcmFieldActionStaggeredPreselProfile1Set:
                /* param0 = presel-profile-val */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_1;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0x3 ) ;
                break;
            case bcmFieldActionStaggeredPreselProfile2Set:
                /* param0 = presel-profile-val */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_2;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0x3 ) ;
                break;
            case bcmFieldActionStaggeredPreselProfile3Set:
                /* param0 = presel-profile-val */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_3;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0x3 ) ;
                break;
            case bcmFieldActionStaggeredPreselProfileDirectSet:
                /* param0 = presel-profile-val */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_STAGGERED_PRESEL_RESULT_KAPS;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0x3 ) ;
                break;
            case bcmFieldActionTrap:
                /* param0 = trap GPORT (or trap ID) */
                /* param1 = n/a  (or trap strength if param0 is not GPORT) */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_TRAP;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    if (!BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                           " TRAP type GPORT"
                                                           " as param0, GPORT"
                                                           " %08X is not"
                                                           " acceptable"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    /* take the provided GPORT as-is */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    /* insert the trap qualifier in Arad */
                    if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "reduced_trap_action", 0) == 0)) {
                        hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam1 & 0xFFFF;               
                    }
                    else if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action if param0 is GPORT\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }

                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not recognize"
                                                       " unknown hardware"),
                                      unit));
                }
                break;
            case bcmFieldActionTrapReduced:
                /* param0 = trap GPORT (or trap ID) */
                /* param1 = n/a  (or trap strength if param0 is not GPORT) */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    if (!BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                           " TRAP type GPORT"
                                                           " as param0, GPORT"
                                                           " %08X is not"
                                                           " acceptable"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    /* take the provided GPORT as-is */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    /* insert the trap qualifier in Arad */
                    if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action if param0 is GPORT\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }

                 
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not recognize"
                                                       " unknown hardware"),
                                      unit));
                }
                break;

            case bcmFieldActionISQ:
                /* param0 = ISQ */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IS;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    if (BCM_GPORT_IS_COSQ(propActs[bcmIndex].bcmParam0) &&
                        BCM_COSQ_GPORT_IS_ISQ(propActs[bcmIndex].bcmParam0)) {
                        hwAct[ppdIndex].hwParam = BCM_COSQ_GPORT_ISQ_QID_GET(propActs[bcmIndex].bcmParam0);
                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s action rquires"
                                                           " COSQ_ISQ type"
                                                           " GPORT as param0,"
                                                           " GPORT %08X is"
                                                           " not acceptable"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                } else {
                    /* accept ISQ ID raw */
                    
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                }
                if (SOC_DPP_DEFS_GET(unit, nof_queues) < hwAct[ppdIndex].hwParam) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("ISQ ID of %d is not"
                                                       " valid on unit %d"),
                                      hwAct[ppdIndex].hwParam,
                                      unit));
                }
                break;
            case bcmFieldActionVSQ:
                /* param0 = VSQ */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_VSQ_PTR;
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_cosq_gport_vsq_gport_get(unit,
                                                                           propActs[bcmIndex].bcmParam0,
                                                                           0,
                                                                           &vsq_core_id,
                                                                           &vsq_is_ocb_only,
                                                                           &vsqType,
                                                                           &local));
                    hwAct[ppdIndex].hwParam = local;
                } else {
                    /* accept VSQ ID raw */
                    
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                }
                if (0xFF < hwAct[ppdIndex].hwParam) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("VSQ ID of %d is not"
                                                       " valid on unit %d"),
                                      hwAct[ppdIndex].hwParam,
                                      unit));
                }
                break;
            case bcmFieldActionPolicerLevel0:
            case bcmFieldActionPolicerLevel1:
                /* param0 = policer ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if ((1 << (_DPP_POLICER_METER_GROUP_SHIFT(unit) + 1)) <= propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("policer %d is not"
                                                       " valid on unit %d."
                                                       " Must be < %d"),
                                      propActs[bcmIndex].bcmParam0,
                                      unit,
                                      (1 << (_DPP_POLICER_METER_GROUP_SHIFT(unit) + 1))));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_METER;

                /* Different PPD action (different pointer) according to the Meter Processor */
                group = _DPP_POLICER_ID_TO_GROUP(unit, propActs[bcmIndex].bcmParam0);
                meter = _DPP_POLICER_ID_TO_METER(unit, propActs[bcmIndex].bcmParam0);
                if (1 == group || ((propActs[bcmIndex].bcmParam0 == 0) && (propActs[bcmIndex].bcmType == bcmFieldActionPolicerLevel1)) ) {
                        hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_METER_B;
                }

                /* In jericho it possible to use mtr id = 0 */
                if (!SOC_IS_JERICHO(unit)) {
                    /* lsb for meter update, 0 means don't meter packet*/
                    hwAct[ppdIndex].hwParam = (meter << 1) +
                                              ((propActs[bcmIndex].bcmParam0 != 0)? 1 :0);
                }else{
                    /* lsb for meter update, 0 is also valid mtr id*/
                    hwAct[ppdIndex].hwParam = (meter << 1) + 1;
                }
                break;
            case bcmFieldActionUsePolicerResult:
                /* param0 = Policer location to change */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (propActs[bcmIndex].bcmParam0 &
                    (~(BCM_FIELD_USE_POLICER_RESULT_INGRESS |
                       BCM_FIELD_USE_POLICER_RESULT_EGRESS))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                       " param0 contains bits"
                                                       " %08X not supported"),
                                      unit,
                                      _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                                      propActs[bcmIndex].bcmParam1 &
                                      (~(BCM_FIELD_USE_POLICER_RESULT_INGRESS |
                                         BCM_FIELD_USE_POLICER_RESULT_EGRESS))));
                }
                local = SOC_PPC_MTR_RES_USE_GET_BY_TYPE((propActs[bcmIndex].bcmParam0 &
                                                         BCM_FIELD_USE_POLICER_RESULT_INGRESS),
                                                        (propActs[bcmIndex].bcmParam0 &
                                                         BCM_FIELD_USE_POLICER_RESULT_EGRESS));
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DP_METER_COMMAND;
                hwAct[ppdIndex].hwParam = local;
                break;
            case bcmFieldActionStat:
            case bcmFieldActionStat1:
                /* param0 = stat ID */
                /* param1 = n/a */
                if ((propActs[bcmIndex].bcmParam1 != 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_proc_cntr_from_stat(unit,
                                                                     propActs[bcmIndex].bcmParam0,
                                                                     &proc,
                                                                     &cntr));
                
                hwAct[ppdIndex].hwType = (bcmFieldActionStat1 ==
                                          propActs[bcmIndex].bcmType)?
                                             SOC_PPC_FP_ACTION_TYPE_COUNTER_B:
                                             SOC_PPC_FP_ACTION_TYPE_COUNTER_A;
                 /* LSB always set: Update Counter, others: Counter-Id */
                hwAct[ppdIndex].hwParam = (cntr << 1) | 1;
                break;
            case bcmFieldActionMirrorIngress:
                /* param0 = destination module (ignored if port is a GPORT) */
                /* param1 = destination port OR GPORT */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1) &&
                    BCM_GPORT_IS_MIRROR(propActs[bcmIndex].bcmParam1)) {
                    /*
                     *  Using GPORT mode and the GPORT is a mirror, so it was
                     *  probably created using the mirror dest APIs or similar,
                     *  and so should already have at least one reference. We
                     *  need to mark this as a reference to it so that when the
                     *  action is removed later it will not decrement to zero
                     *  and free the application-created mirror.
                     */
                    mirrorId = BCM_GPORT_MIRROR_GET(propActs[bcmIndex].bcmParam1);
                } else {
                    /*
                     *  Either the GPORT is not a MIRROR GPORT or the caller is
                     *  using the standard module,port form of the action.  In
                     *  either case, need to allocate a mirror for this to work
                     *  (the allocate function will reuse the same mirror if
                     *  there is already one going to this place).
                     */
                    {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                           " allow implicit"
                                                           " mirror alloc and"
                                                           " param1 %d(%08X)"
                                                           " is not a mirror"
                                                           " gport\n"),
                                          unit,
                                          propActs[bcmIndex].bcmParam1,
                                          propActs[bcmIndex].bcmParam1));
                    }
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_MIRROR;
                hwAct[ppdIndex].hwParam = (mirrorId & 0xF);
                break;
            case bcmFieldActionSnoop:
                /* param0 = gport of type trap - the strength is inside */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0) &&
                    BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                    SOC_PPC_TRAP_CODE ppd_trap_code;

                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_rx_ppd_trap_code_from_trap_id(unit, BCM_GPORT_TRAP_GET_ID(propActs[bcmIndex].bcmParam0),
                                                                           &ppd_trap_code),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to map trap id %d to ppd trap"),
                                                          BCM_GPORT_TRAP_GET_ID(propActs[bcmIndex].bcmParam0)));
                    /* Snoop strength (2b) ; Snoop code (8b)*/
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_SNP;
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_trap_ppd_to_hw(unit,
                                                                      ppd_trap_code,
                                                                      &hwAct[ppdIndex].hwParam),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to get from"
                                                          " the HW Trap Id for"
                                                          " PPD trap %d(%s)"),
                                         ppd_trap_code, SOC_PPC_TRAP_CODE_to_string(ppd_trap_code)));
                   hwAct[ppdIndex].hwParam += (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(propActs[bcmIndex].bcmParam0) << 8);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                       " TRAP type GPORT as"
                                                       " param0, param0 %08X"
                                                       " is not acceptable"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionDrop:
                /* param0 = n/a */
                /* param1 = n/a */
                /* ingress drop: send to discard location */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = (SOC_IS_JERICHO_PLUS(unit)) ? SOC_PPC_FP_ACTION_TYPE_DEST_DROP :  SOC_PPC_FP_ACTION_TYPE_DEST;
                hwAct[ppdIndex].hwParam =  _BCM_DPP_FIELD_DROP_DEST(unit);
                break;
            case bcmFieldActionForward:
                /* param0 = forwarding destination raw value (19bits) */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0x7FFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (destination raw value)"
                                                       " %d, but must be (0..0x7FFFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0x7FFFF;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DEST;
                if (SOC_IS_JERICHO_PLUS(unit) && !is_action_redirect_jer_style) {
                    hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_DEST_DROP;
                    hwAct[ppdIndex + 1].hwParam = 0x0;
                    actCount = 2;
                }
                break;
            case bcmFieldActionTrapCodeQualifier:
                /* param0 = Trap-code (8 bits) | forward-strength (3 bits) | snoop-strength (2 bits)  */
                /* param1 = Trap code qualifier (13 bits)*/
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    if (0x1FFF < propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s param0 (trap raw value)"
                                                           " %d, but must be (0..0x1FFF)\n"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    if (0x1FFF < propActs[bcmIndex].bcmParam1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s param1 (Trap code qualifier)"
                                                           " %d, but must be (0..0x1FFF)\n"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam1));
                    }
                    hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam1 << 19 ) + ( propActs[bcmIndex].bcmParam0 & 0x1FFF );
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DEST;
                    if (SOC_IS_JERICHO_PLUS(unit) && !is_action_redirect_jer_style) {
                        hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_DEST_DROP;
                        hwAct[ppdIndex + 1].hwParam = 0x0;
                        actCount = 2;
                    }
                }
                else 
                {
                       BCMDNX_ERR_EXIT_MSG(
                        BCM_E_PARAM,
                        (_BSL_BCM_MSG_NO_UNIT("%s(): This case (bcmFieldActionTrapCodeQualifier) is not active on this device."),
                        __func__ )) ;
                }
                break;
            case bcmFieldActionPphSnoopCode:
                /* param0 = PPH snoop-code  */
                /* param1 = n/a*/
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }
                    if (0x3 < propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s param0 (PPH snoop code)"
                                                           " %d, but must be (0..3)\n"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    hwAct[ppdIndex].hwParam = ( (( propActs[bcmIndex].bcmParam0 & 0x3 ) << 1) | 1 );
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_PPH_RESERVE_VALUE;
                }
                else 
                {
                       BCMDNX_ERR_EXIT_MSG(
                        BCM_E_PARAM,
                        (_BSL_BCM_MSG_NO_UNIT("%s(): This case (bcmFieldActionPphSnoopCode) is not active on this device."),
                        __func__ )) ;
                }
                break;
            case bcmFieldActionRedirectMcast:
                /* param0 = L2 GPORT or multicast group ID */
                /* param1 = n/a */
            case bcmFieldActionRedirectIpmc:
                /* param0 = IPMC GPORT or multicast group ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /* param0 is not GPORT; consider it a multicast group ID */
                    int multi_group_id = _BCM_MULTICAST_ID_GET(propActs[bcmIndex].bcmParam0);
                    if (16383 < multi_group_id) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT %s"
                                                           " param0(%d) multi_group_id(%d)"
                                                           " is not valid"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0, multi_group_id));
                    }
                    
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                } /* if (BCM_GPORT_IS_SET(param0)) */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DEST;
                if (SOC_IS_JERICHO_PLUS(unit) && !is_action_redirect_jer_style) {
                    hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_DEST_DROP;
                    hwAct[ppdIndex + 1].hwParam = 0x0;
                    actCount = 2;
                }
                break;
            case bcmFieldActionRedirectTrunk:
                /* param0 = aggregate GPORT or aggregate ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /* param0 is not a GPORT; assume it is aggregate ID */
                    if (255 < propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT %s"
                                                           " param0(%d)"
                                                           " is not valid"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    
                    hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 |
                                         (1 << 12));
                    
                } /* if (BCM_GPORT_IS_SET(param0)) */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DEST;
                if (SOC_IS_JERICHO_PLUS(unit) && !is_action_redirect_jer_style) {
                    hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_DEST_DROP;
                    hwAct[ppdIndex + 1].hwParam = 0x0;
                    actCount = 2;
                }
                break;
            case bcmFieldActionRedirect:  
                /* param0 = module */
                /* param1 = GPORT or port number */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                    /* param1 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam1;
                } else { /* if (BCM_GPORT_IS_SET(param1)) */
                    /*
                     *  param1 is not a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    if ((_SHR_GPORT_PORT_MASK < propActs[bcmIndex].bcmParam1) ||
                        (_SHR_GPORT_MODID_MASK < propActs[bcmIndex].bcmParam0)) {
                        /* can't represent it as modport, so error */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT %s"
                                                           " param0(%d)"
                                                           " is not valid"
                                                           " modid (0..%d) or"
                                                           " param1(%d)"
                                                           " is not valid"
                                                           " port (0..%d)"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0,
                                          _SHR_GPORT_MODID_MASK,
                                          propActs[bcmIndex].bcmParam1,
                                          _SHR_GPORT_PORT_MASK));
                    }
                    BCM_GPORT_MODPORT_SET(lookupGport[0],
                                          propActs[bcmIndex].bcmParam0,
                                          propActs[bcmIndex].bcmParam1);
                    
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DEST;
                if (SOC_IS_JERICHO_PLUS(unit) && !is_action_redirect_jer_style) {
                    hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_DEST_DROP;
                    hwAct[ppdIndex + 1].hwParam = 0x0;
                    actCount = 2;
                }
                break;
            case bcmFieldActionL3Switch:
                /* param0 = FEC-ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (!BCM_L3_ITF_TYPE_IS_FEC(propActs[bcmIndex].bcmParam0)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new FEC-Id)"
                                                       " %d, is not an egress"
                                                       " object that represents"
                                                       " a FEC"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));

                }
                fec = BCM_L3_ITF_VAL_GET(propActs[bcmIndex].bcmParam0);
                if ( SOC_DPP_DEFS_GET(unit, nof_fecs) < fec) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new FEC-Id)"
                                                       " %d, but must be"
                                                       " (0..%d)"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0,
                                      SOC_DPP_DEFS_GET(unit, nof_fecs)));
                }
                /* Get the Forward decision and the HW encoding */
                SOC_PPD_FRWRD_DECISION_FEC_SET(unitData->unitHandle,
                                               fwdDecision,
                                               fec,
                                               soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
/*
 * COVERITY
 *
 * mask_edata is initialized inside _bcm_dpp_field_qualify_fwd_decision_to_hw_destination_convert.
 */
/* coverity[uninit_use_in_call] */
                COMPILER_64_SET(mask_edata, 0, (~0));

                BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_qualify_fwd_decision_to_hw_destination_convert(unit,
                                                                                                  fwdDecision,
                                                                                                  TRUE, /* is_for_destination */
                                                                                                  TRUE, /* is_for_action */
                                                                                                  &dest_edata,
                                                                                                  &mask_edata),
                                    (_BSL_BCM_MSG_NO_UNIT("unable to get from the"
                                                      " forward decision the"
                                                      " HW data, FEC %d"),
                                     propActs[bcmIndex].bcmParam0));

                hwAct[ppdIndex].hwParam = COMPILER_64_LO(dest_edata);
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DEST;
                if (SOC_IS_JERICHO_PLUS(unit) && !is_action_redirect_jer_style) {
                    hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_DEST_DROP;
                    hwAct[ppdIndex + 1].hwParam = 0x0;
                    actCount = 2;
                }
                break;
            case bcmFieldActionDstRpfGportNew:
                /* param0 = GPORT */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_RPF_DESTINATION;
                    /* Destination is valid */
                    hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_RPF_DESTINATION_VALID;
                    hwAct[ppdIndex + 1].hwParam = 0x1;
                    /* two hardware actions are used here */
                    actCount = 2;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /*
                     *  param1 must be a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0(%d)"
                                                       " is not valid GPORT"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionSrcGportNew:
                /* param0 = GPORT */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_SRC_SYST_PORT;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /*
                     *  param1 must be a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0(%d)"
                                                       " is not valid GPORT\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            /* This case is for VTT stage */
            case bcmFieldActionIngressGportSet:
                /* param0 = GPORT */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* Special case for In-LIF 0 to reset the In-LIF value */
                    if ((BCM_GPORT_IS_VLAN_PORT(propActs[bcmIndex].bcmParam0)) &&  (0 == BCM_GPORT_VLAN_PORT_ID_GET(propActs[bcmIndex].bcmParam0))) {
                        hwAct[ppdIndex].hwParam = 0;
                        hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IN_LIF;
                    }
                    else {
                        _bcm_dpp_gport_hw_resources gport_hw_resources;

                        /* param0 is GPORT; handle as such */
                        /* If the gport is of type LIF, change the InLIF, otherwise Destination
                         * inLIF in IngressGportSet means SEM-result which is actually local LIF*/
                        result = _bcm_dpp_gport_to_hw_resources(unit, propActs[bcmIndex].bcmParam0, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS|_BCM_DPP_GPORT_HW_RESOURCES_FEC, &gport_hw_resources);
                        inLif = gport_hw_resources.local_in_lif;
                        fec = gport_hw_resources.fec_id;

                        if ((BCM_E_NONE != result) ||
                            (inLif == _BCM_GPORT_ENCAP_ID_LIF_INVALID)) {
                            /* Do not exit - just indicate it is not LIF */
                            LOG_WARN(BSL_LS_BCM_FP,
                                     (BSL_META_U(unit,
                                                 "unable to compute inlif for"
                                                 " unit %d GPORT %08X"
                                                 " %d (%s)\n"),
                                      unit,
                                      propActs[bcmIndex].bcmParam0,
                                      result,
                                      _SHR_ERRMSG(result)));
                        }
                        /* Allow inLif 0 in Arad but not in Petra-B */
                        lookupOutLif[0] = propActs[bcmIndex].bcmParam0;
                        hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IN_LIF;
                    }
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    /*
                     *  param1 must be a GPORT, so param1 must be physical port
                     *  number and param0 is module ID.
                     */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0(%d)"
                                                       " is not valid GPORT\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                break;
            case bcmFieldActionPrioIntNew:
                /* param0 = new priority (internal only?) */
                /* param1 (misdoc as N/A?) contains marking flags(???) */
            case bcmFieldActionVportTcNew:
                /* param0 = new traffic class value */
                /* param1 (misdoc as N/A?) contains marking flags(???) */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (7 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new"
                                                       " priority) %d, but"
                                                       " must be (0..7)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (1 << 3) | (propActs[bcmIndex].bcmParam0 & 0x7);
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_TC;
                break;
            case bcmFieldActionIncomingMplsPortSet:
            case bcmFieldActionIncomingIpIfSet:
                /* param0 = interface-id */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if ( SOC_DPP_DEFS_GET(unit,nof_rifs) < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new"
                                                       " port) %d, but must"
                                                       " be (0..%08X)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0,
                                      ( (1 << SOC_DPP_DEFS_GET(unit,in_rif_nof_bits)) - 1 )));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & ( (1 << SOC_DPP_DEFS_GET(unit,in_rif_nof_bits)) - 1 );
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IN_RIF;
                break;
            case bcmFieldActionSystemHeaderSet:
                /* param0 = System-Header-type */
                /* param1 = Header value */
                if (propActs[bcmIndex].bcmParam0 != bcmFieldSystemHeaderPphEei) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (system"
                                                       " header type) %d, but"
                                                       " must be"
                                                       " bcmFieldSystemHeaderPphEei\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                if (0xFFFFFF < propActs[bcmIndex].bcmParam1) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param1 (new header)"
                                                       " %d, but must be"
                                                       " (0..0xFFFFFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam1));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam1 & 0xFFFFFF;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_EEI;
                break;
            case bcmFieldActionLearnForwardingVlanIdNew:
                /* param0 = VLAN-ID (actually FID) */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0x7FFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new priority)"
                                                       " %d, but must be (0..0x7FFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0x7FFF;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_LEARN_FID;
                break;
            case bcmFieldActionLearnSrcPortNew:
                /* param0 = Source-Port */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    /* param0 is GPORT; handle as such */
                    lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_0_TO_15;
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFFF;
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_0_TO_15;
                }
                break;
            case bcmFieldActionLearnInVPortNew:
                /* param0 = Source-VPort */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_gport_to_global_lif_convert(unit,
                                                                                  propActs[bcmIndex].bcmParam0,
                                                                                  TRUE,
                                                                                  &(hwAct[ppdIndex].hwParam)));
                } else { /* if (BCM_GPORT_IS_SET(param0)) */
                   hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFFFFF;
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_16_TO_39;
                break;
            case bcmFieldActionMacDaKnown:
                /* param0 = N/A */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Although the name, DLF is a Destination failure, so the packet is unknown */
                hwAct[ppdIndex].hwParam = 0x1;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_UNKNOWN_ADDR;
                break;
            case bcmFieldActionDoNotLearn:
                /* param0 = 0 - do not learn, 1 - ingress learning, 2 - egress learning */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Invert the meaning: 0 in HW means Do-not-learn */
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 == 1)? 1: 0;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_INGRESS_LEARN_ENABLE;
                hwAct[ppdIndex + 1].hwParam = (propActs[bcmIndex].bcmParam0 == 2)? 1: 0;
                hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_EGRESS_LEARN_ENABLE;
                /* this took two actions */
                actCount = 2;
                break;
            case bcmFieldActionFilters:
                /* param0 = filter type */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (propActs[bcmIndex].bcmParam0 !=
                    BCM_FIELD_FILTER_SA_DISABLED) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (filter"
                                                       " type) %d, but must be"
                                                       " BCM_FIELD_FILTER_SA_DISABLED\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = 0x1;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_EXC_SRC;
                break;
            case bcmFieldActionOrientationSet:
                /* param0 = orientation type */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if ((bcmFieldOrientationNetwork !=
                     propActs[bcmIndex].bcmParam0) &&
                    (bcmFieldOrientationAccess !=
                     propActs[bcmIndex].bcmParam0)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (orientation)"
                                                       " %d, but must be one of"
                                                       " (bcmFieldOrientationNetwork,"
                                                       " bcmFieldOrientationAccess)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (bcmFieldOrientationNetwork ==
                                           propActs[bcmIndex].bcmParam0);
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_ORIENTATION_IS_HUB;
                break;
            case bcmFieldActionDhcpPkt:
                /* param0 = BOOTP/DHCP packet indicator */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0?1:0;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_PACKET_IS_BOOTP_DHCP;
                break;
            case bcmFieldActionCnmCancel:
                /* param0 = N/A */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Ignore-CP if done */
                hwAct[ppdIndex].hwParam = 0x1;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IGNORE_CP;
                break;
            case bcmFieldActionStartPacketStrip:
                /* param0 = Header-Base */
                /* 
                 * param1 = Number of bytes from the Header-Base 
                 * in a 2's complement format (up to 0x1F, positive values)  
                 */
                if (propActs[bcmIndex].bcmParam1 > 0x3F) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param1 (Bytes to"
                                                       " strip) %d, but must"
                                                       " be <= 0x3F\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam1));
                }
                /* Translate the base header */
                switch (propActs[bcmIndex].bcmParam0) {
                case bcmFieldStartToConfigurableStrip:
                    base_header = 0x0;
                    break;
                case bcmFieldStartToL2Strip:
                    base_header = 0x1;
                    break;
                case bcmFieldStartToFwdStrip:
                    base_header = 0x2;
                    break;
                case bcmFieldStartToFwdNextStrip:
                    base_header = 0x3;
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (header base)"
                                                       " %d is not valid\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam1 |
                                           (base_header << 6));
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_BYTES_TO_REMOVE;
                break;
            case bcmFieldActionForwardingTypeNew:
                /* param0 = New Forwarding header type */
                /* param1 = N/A in general - if set, set the forwarding-offset */
                /* Translate the forwarding header */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_forwarding_type_bcm_to_ppd(propActs[bcmIndex].bcmParam0,
                                                                                  &ppdForwardingType,
                                                                                  &ppdMask));
                
                switch(propActs[bcmIndex].bcmParam0) {
                case bcmFieldForwardingTypeL2:
                case bcmFieldForwardingTypeRxReason:
                case bcmFieldForwardingTypeTrafficManagement:
                case bcmFieldForwardingTypeSnoop:
                    local = 1;
                    break;
                case bcmFieldForwardingTypeIp4Ucast:
                case bcmFieldForwardingTypeIp4Mcast:
                case bcmFieldForwardingTypeIp6Ucast:
                case bcmFieldForwardingTypeIp6Mcast:
                case bcmFieldForwardingTypeMpls:
                case bcmFieldForwardingTypeTrill:
                case bcmFieldForwardingTypeCustom1:
                case bcmFieldForwardingTypeCustom2:
                    local = 2;
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (Forwarding"
                                                       " Type) %d is not"
                                                       " supported\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                };
                hwAct[ppdIndex].hwParam = ppdForwardingType;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FWD_CODE;
                /* Take fix (5:0), i.e. additional local-offset of 0, only base-header (8:6) */
                /* Set the base-header explicitly via param1 if unusual */
                local = (propActs[bcmIndex].bcmParam1)? propActs[bcmIndex].bcmParam1: local;
                if(SOC_IS_JERICHO(unit)) {
                    /* Take fix (5:0) if given explicitly via param2, otherwise 0 */
                    local_offset = (propActs[bcmIndex].bcmParam2 != BCM_ILLEGAL_ACTION_PARAMETER)?
                        propActs[bcmIndex].bcmParam2: 0;
                    hwAct[ppdIndex + 1].hwParam = local_offset + (local << 6);
                } else {
                    hwAct[ppdIndex + 1].hwParam = 0 + (local << 6);
                }
                hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_FWD_OFFSET;
                /* two actions to do this */
                actCount = 2;
                break;
            case bcmFieldActionDropPrecedence:
                /* param0 = new DP (BCM_FIELD_COLOR_*) */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_color_bcm_to_ppd(propActs[bcmIndex].bcmParam0,
                                                                        &(hwAct[ppdIndex].hwParam)));
                /* but also need to tell it to always set the value */
                hwAct[ppdIndex].hwParam |= 0x0C;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DP;
                break;
            case bcmFieldActionMirrorEgressDisable:
                /* param0 = disable egress mirror flag */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_MIR_DIS;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0?1:0;
                break;
            case bcmFieldActionVrfSet:
                /* param0 = new VRF ID */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (SOC_DPP_DEFS_GET(unit, nof_vrfs) < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VRF)"
                                                       " %d, but must be"
                                                       " be (0..%08X)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0,   
                                      ((1 << SOC_DPP_DEFS_GET(unit,vrf_nof_bits)) -1) ));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_VRF;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & ((1 << SOC_DPP_DEFS_GET(unit,vrf_nof_bits)) -1 );
                break;
            case bcmFieldActionStatTag:
                /* param0 = new stat tag */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new stat"
                                                       " tag) %d, but must be"
                                                       " (0..0xFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_STAT;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFF;
                break;
            case bcmFieldActionMultipathHashAdditionalInfo:
                /* param0 = new MP hash additional info */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new MP hash"
                                                       " additional info) %d,"
                                                       " but must be"
                                                       " (0..0xFFFFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_ECMP_LB;
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 &
                                           0xFFFFF);
                break;
            case bcmFieldActionTrunkHashKeySet:
                /* param0 = new trunk hash key */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new trunk"
                                                       " hash key) %d,"
                                                       " but must be"
                                                       " (0..0xFFFFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_LAG_LB;
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam0 &
                                           0xFFFFF);
                break;
            case bcmFieldActionDscpNew:
                /* param0 = new DSCP value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new DSCP"
                                                       " value) %d,"
                                                       " but must be"
                                                       " (0..0xFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IN_DSCP_EXP;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFF;
                break;
            case bcmFieldActionTtlSet:
                /* param0 = new TTL value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new TTL"
                                                       " value) %d,"
                                                       " but must be"
                                                       " (0..0xFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IN_TTL;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFF;
                break;
            case bcmFieldActionVSwitchNew:
                /* param0 = new virtual switching instance */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VSI)"
                                                       " %d, but must be"
                                                       " (0..0xFFFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_VSI;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFFF;
                break;
            case bcmFieldActionVlanActionSetNew:
                /* If simple mode */
                /* param0 = new VLAN translation action Id */
                /* param1 = (optional) new BCM_FIELD_VLAN_FORMAT_ */
                /* If advanced mode */
                /* param0 = VLAN-Edit-Command */
                /* param1 = n/a */
                
                if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unitData->unitHandle)) {
                    /* Advanced user */
                    if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }
                    if (SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unitData->unitHandle) <= propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VLAN-Edit-Command)"
                                                           " %d, but must be"
                                                           " (0..63)\n"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_COMMAND;
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                }
                else { /* Simple VLAN translation mode */
                    vlan_format = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
                    if (propActs[bcmIndex].bcmParam1) {
                        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_vlan_format_bcm_to_ppd(propActs[bcmIndex].bcmParam1,
                                                                                      &vlan_format));
                    }
                    /* Get the IVEC-Id */
                    
                    _bcm_petra_vlan_edit_command_hw_get(unit,
                                                        propActs[bcmIndex].bcmParam0,
                                                        vlan_format,
                                                        &ivec_id,
                                                        &is_ivec_to_set,
                                                        &is_vlan_format_valid);
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_COMMAND;
                    hwAct[ppdIndex].hwParam = ivec_id;
                }
                break;
            case bcmFieldActionOuterVlanPrioNew:
                /* param0 = new VLAN PCP */
                /* param1 = new VLAN DEI */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_PCP_DEI;
                hwAct[ppdIndex].hwParam = (propActs[bcmIndex].bcmParam1 & 0x1) /* DEI */
                    | ((propActs[bcmIndex].bcmParam0 & 0x7) << 1); /* PCP  */
                break;
            case bcmFieldActionOuterVlanNew:
            case bcmFieldActionInnerVlanNew:
                /* param0 = new VLAN Id */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0xFFF < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new VLAN-Id)"
                                                       " %d, but must be"
                                                       " (0..0xFFF)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                /* Change Outer/Inner VLAN-ID if the VLAN-Edit-Command maps it */
                hwAct[ppdIndex].hwType = (propActs[bcmIndex].bcmType == bcmFieldActionInnerVlanNew)?
                    SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_VID_2: SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_VID_1;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFF;
                break;
            case bcmFieldActionClassDestSet:
            case bcmFieldActionClassSourceSet:
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, groupD_group, &groupD_groupFlags));
                if ((groupD_groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY)) {
                    hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1;

                    hwAct[ppdIndex + 1].hwParam = propActs[bcmIndex].bcmParam1;
                    hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2;
                    actCount = 2;
                } else {
                    /* param0 = User-Header value (max 31 bits due to FES limitation to extract 32b including valid bit) */
                    /* param1 = n/a */
                    if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "param1(%d) is ignored for %s"
                                             " action\n"),
                                  propActs[bcmIndex].bcmParam1,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }
                    
                    if (SOC_IS_JERICHO_PLUS(unit) 
                        && (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_LARGE)
                        && (GroupData->grpMode == bcmFieldGroupModeDirect)) {
                        /* in this mode, valid bit is no need */
                        hwAct[ppdIndex].hwType = (propActs[bcmIndex].bcmType == bcmFieldActionClassDestSet)?
                            SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1: SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2;
                        hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0 & 0xFFFFFFFF;    
                    } else {
                        if (SHR_BITGET(&propActs[bcmIndex].bcmParam0, 31)) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG_NO_UNIT("%s param0 (new User-Header)"
                                                               " %d, but must be"
                                                               " (0..0x7FFF_FFFF)\n"),
                                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                              propActs[bcmIndex].bcmParam0));
                        }
                        /* Do not check the input size since the action size is complex to know (SOC property dependent) */
                        hwAct[ppdIndex].hwType = (propActs[bcmIndex].bcmType == bcmFieldActionClassDestSet)?
                            SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1: SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2;

                        result = _bcm_dpp_ppd_action_bits(unit,
                                                          groupD_stage,
                                                          hwAct[ppdIndex].hwType,
                                                          &bits);
                        if (BCM_E_NONE != result) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG_NO_UNIT(                                                 
                                                 "unit %d unable to get bit count"
                                                 " for action %d: %d (%s)\n"),
                                                 hwAct[ppdIndex].hwType));
                        }

                        hwAct[ppdIndex].hwParam = (bits>=32) ? propActs[bcmIndex].bcmParam0 : (propActs[bcmIndex].bcmParam0&((0x1L<<bits)-1));
                    }
                }
                break;
            case bcmFieldActionEcnNew:
                /* param0 = new CI value */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (0x3 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new CI)"
                                                       " %d, but must be"
                                                       " (0..3)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                /* ECN-Capable in bit 0, CNI in bit 1 */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FWD_HDR_ENCAPSULATION;
                hwAct[ppdIndex].hwParam = ((propActs[bcmIndex].bcmParam0?0x1:0x0) |
                                           ((0x3 == propActs[bcmIndex].bcmParam0)?2:0));
                break;
            case bcmFieldActionVportNew:
                /* param0 = vport */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                lookupOutLif[0] = propActs[bcmIndex].bcmParam0;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_OUTLIF;
                break;
            case bcmFieldActionIntPriorityAndDropPrecedence:
                /* param0 = new traffic class value */
                /* param1 = new DP (BCM_FIELD_COLOR_*) */
                switch (propActs[bcmIndex].bcmParam1) {
                case BCM_FIELD_COLOR_GREEN:
                    hwAct[ppdIndex].hwParam = 0xC; /* set DP const = green */
                    break;
                case BCM_FIELD_COLOR_YELLOW:
                    hwAct[ppdIndex].hwParam = 0xD; /* set DP const = yellow */
                    break;
                case BCM_FIELD_COLOR_RED:
                    hwAct[ppdIndex].hwParam = 0xE; /* set DP const = red */
                    break;
                case BCM_FIELD_COLOR_BLACK:
                    hwAct[ppdIndex].hwParam = 0xF; /* set DP const = black */
                    break;
                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("param1 value %d not"
                                                       " supported on unit %d"
                                                       " for action %s"),
                                      propActs[bcmIndex].bcmParam1,
                                      unit,
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DP;
                if (7 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new"
                                                       " Priority) %d,"
                                                       " but must be"
                                                       " (0..7)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex + 1].hwParam = ((1 << 3) |
                                     (propActs[bcmIndex].bcmParam0 & 0x7));
                hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_TC;
                /* this takes two actions */
                actCount = 2;
                break;
            case bcmFieldActionPphPresentSet:
                /* param0 = PPH type if present*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Check the PPH type is in range 0..3 */
                if (3 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (pph type)"
                                                       " %d, but must be (0..3)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_PPH_TYPE;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                break;

            case bcmFieldActionFabricHeaderSet:
                /* param0 = System header profile*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_SYSTEM_HEADER_PROFILE_ID;
                switch (propActs[bcmIndex].bcmParam0) {
                case bcmFieldFabricHeaderEthernet:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET; /* Ethernet Profile Header*/
                    break;
                case bcmFieldFabricHeaderTrafficManagementUcast:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_UNICAST;  /* TM Unicast Profile Header*/
                    break;
                case bcmFieldFabricHeaderTrafficManagementMcast:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_MULTICAST;  /* TM UnicasProfile Header*/
                    break;
                case bcmFieldFabricHeaderTrafficManagementVPort:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_OUTLIF; /* TM Outlif Profile Header*/
                    break;
                case bcmFieldFabricHeaderStacking:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_STACKING; /*Stacking Profile Header*/
                    break;
                case bcmFieldFabricHeaderEthernetDSP:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_DSP; /* DSP Extension Profile Header*/
                    break;
                case bcmFieldFabricHeaderEthernetLearn:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LEARN; /* DSP Extension Profile Header*/
                    break;
                case bcmFieldFabricHeaderEthernetlatency:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LATENCY; /* OAM-TS-LATENCY Profile Header*/
                    break;
                case bcmFieldFabricHeaderEthernetLearnDisable:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LEARN_DISABLE; /* PPH fabric header without Learn Extension. */
                    break;
                default:
                    hwAct[ppdIndex].hwParam = SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET; /* Ethernet Profile Header*/
                    break;
                }
                break;

            case bcmFieldActionOam: /*new action*/
                /* param0 = {OAM-Up-Mep, OAM-Sub-Type, OAM-offset, OAM-Stamp-Offset} */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_OAM;
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                break;
            case bcmFieldActionIEEE1588: /*new action*/
                /* param0 = Packet-is-IEEE-1588(bit 11); IEEE-1588-Update-Time-Stamp(bit 10); IEEE-1588-Command (bits 8:9); IEEE-1588-Encapsulation(bit 7); IEEE-1588-Header-Offset (bits 0:6)*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IEEE_1588;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0xFFF );
                break;
            case bcmFieldActionStackingRouteNew: 
                /* param0 = value of the stacking route history bitmap*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_STACK_RT_HIST;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0xFFFF );
                break;
            case bcmFieldActionInterfaceClassVPort: 
                /* param0 = value of the in-lif-profile*/
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_IN_LIF_PROFILE;
                hwAct[ppdIndex].hwParam = ( propActs[bcmIndex].bcmParam0 & 0xF );
                break;
            case bcmFieldActionStatAndPolicer:
                /* param0 = Meter/Counter ID*/
                /* param1 = Double Action Mode */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER;
                /* Counter/Meter ID | (meter/counter) update bit | double action mode (search Double Action in arad_pp_fp_fem.c
                 * for more info) */
                hwAct[ppdIndex].hwParam = ((propActs[bcmIndex].bcmParam0) << 3) | (1 << 2) | (propActs[bcmIndex].bcmParam1 & 0x3);
                break;
            case bcmFieldActionSnoopAndTrap:
                /* param0 = Gport of type trap + Snoop/Trap Strength */
                /* param1 = Double Action Mode */
                if (propActs[bcmIndex].bcmParam1 == bcmFieldDoubleActionFirst) {
                    /* Snoop */
                    if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0) &&
                        BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                        SOC_PPC_TRAP_CODE ppd_trap_code;

                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_rx_ppd_trap_code_from_trap_id(unit, BCM_GPORT_TRAP_GET_ID(propActs[bcmIndex].bcmParam0),
                                                                               &ppd_trap_code),
                                            (_BSL_BCM_MSG_NO_UNIT("unable to map trap id %d to ppd trap"),
                                                              BCM_GPORT_TRAP_GET_ID(propActs[bcmIndex].bcmParam0)));
                        /* Snoop strength (2b) ; Snoop code (8b)*/
                        hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_trap_ppd_to_hw(unit,
                                                                          ppd_trap_code,
                                                                          &hwAct[ppdIndex].hwParam),
                                            (_BSL_BCM_MSG_NO_UNIT("unable to get from"
                                                              " the HW Trap Id for"
                                                              " PPD trap %d(%s)"),
                                             ppd_trap_code, SOC_PPC_TRAP_CODE_to_string(ppd_trap_code)));
                       hwAct[ppdIndex].hwParam += (BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(propActs[bcmIndex].bcmParam0) << 8);
                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                           " TRAP type GPORT as"
                                                           " param0, param0 %08X"
                                                           " is not acceptable"),
                                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                          propActs[bcmIndex].bcmParam0));
                    }
                } else if (propActs[bcmIndex].bcmParam1 == bcmFieldDoubleActionSecond) {
                    /* Trap */
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP;
                    if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam0)) {
                        if (!BCM_GPORT_IS_TRAP(propActs[bcmIndex].bcmParam0)) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG_NO_UNIT("%s action requires"
                                                               " TRAP type GPORT"
                                                               " as param0, GPORT"
                                                               " %08X is not"
                                                               " acceptable"),
                                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                              propActs[bcmIndex].bcmParam0));
                        }
                        /* take the provided GPORT as-is */
                        lookupGport[0] = propActs[bcmIndex].bcmParam0;
                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not recognize"
                                                           " unknown hardware"),
                                          unit));
                    }
                } else if (propActs[bcmIndex].bcmParam1 == bcmFieldDoubleActionBoth) {
                    /* Both */
                    /* For Trap/Snoop, both commands cannot run together,
                     * therefore, we exit with error */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("%s can not run with"
                                               "mode bcmFieldDoubleActionBoth"),
                          _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Add the double action mode for the hw parameter (search Double Action in arad_pp_fp_fem.c for more info) */
                hwAct[ppdIndex].hwParam = (hwAct[ppdIndex].hwParam << 2) | (propActs[bcmIndex].bcmParam1 & 0x3);
                break;
            case bcmFieldActionAdmitProfile:
                if ( propActs[bcmIndex].bcmParam1 ) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* bcmFieldAdmitProfileEcnCapable is the last enum in bcm_field_admit_profile_t. */
                if ( propActs[bcmIndex].bcmParam0 > bcmFieldAdmitProfileEcnCapable ) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s action SW supports only %d"
                                                       "number of profiles"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      (bcmFieldAdmitProfileEcnCapable+1)));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_ADMIT_PROFILE;
                break;
            case bcmFieldActionLearnSrcMacNew:
                /* Param0 is 32b used to set MSB bits of new Src Mac value */
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_LEARN_SA_16_TO_47;
                /* Param1 is 16b used to set LSB bits of new Src Mac value */
                hwAct[ppdIndex + 1].hwParam = propActs[bcmIndex].bcmParam1;
                hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_LEARN_SA_0_TO_15;
                /* this took two actions */
                actCount = 2;
                break;
            case bcmFieldActionLatencyFlowId:
                /* param0 = New Latency-Flow-Id value */
                /* param1 = Ignored for LatencyFlowId action */
                if(!SOC_IS_JERICHO_PLUS(unit)) {
                     BCMDNX_ERR_EXIT_MSG(
                        BCM_E_UNAVAIL,
                        (_BSL_BCM_MSG_NO_UNIT("Action LatencyFlowId is suported only on QAX and JERICHO_PLUS"))) ;
                }
                if ( propActs[bcmIndex].bcmParam1 ) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (propActs[bcmIndex].bcmParam0 >= 1 << 19) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("Latency-Flow-Id should be 19 bits value, but received"
                                                       "param0(%d)"),
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_LATENCY_FLOW_ID;
                break;
            case bcmFieldActionStatisticPtr0:
                /* coverity[fallthrough:FALSE] */
            case bcmFieldActionStatisticPtr1:
                /* param0 = New Latency-Flow-Id value */
                /* param1 = Ignored */
                if(!SOC_IS_JERICHO_PLUS(unit)) {
                     BCMDNX_ERR_EXIT_MSG(
                        BCM_E_UNAVAIL,
                        (_BSL_BCM_MSG_NO_UNIT("Action statistic pointer is suported only on QAX and JERICHO_PLUS"))) ;
                }
                if ( propActs[bcmIndex].bcmParam1 ) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (propActs[bcmIndex].bcmParam0 >= (0x1L << 21)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("Statistic-Pointer should be 21 bits value, but received"
                                                       "param0(%d)"),
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
                hwAct[ppdIndex].hwType = (propActs[bcmIndex].bcmType==bcmFieldActionStatisticPtr0)?SOC_PPC_FP_ACTION_TYPE_STATISTICS_POINTER_0:SOC_PPC_FP_ACTION_TYPE_STATISTICS_POINTER_1;
                break;
            default:
                /* should not get here */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unexpected action type"
                                                   " %d(%s) on unit %d group"
                                                   " %u entry %u\n"),
                                  propActs[bcmIndex].bcmType,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                  unit,
                                  groupD_group,
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
                break;
            } /* switch (propActs[index].bcmType) */
            /* Handle LIF lookups if needed */
            for (index = 0; index < actCount; index++) {
                if (BCM_GPORT_INVALID != lookupOutLif[index]) {
                    if ((propActs[index].bcmType)==bcmFieldActionIngressGportSet )
                    {
                        if (SOC_IS_JERICHO(unit)) {
                            /*inLIF in IngressGportSet means SEM-result, we decode it from the inlif data*/
                            bcm_dpp_am_local_inlif_info_t inlif_info;
                            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.inlif_info.data_base.get(unit, inLif, &inlif_info));
                            hwAct[ppdIndex + index].hwParam = ( inlif_info.local_lif_flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE )  ? SOC_SAND_MIN(inLif,inlif_info.ext_lif_id) : inLif ;
                        }
                        else
                        {
                            hwAct[ppdIndex + index].hwParam = inLif;
                        }
                    }
                    else
                    {
                        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_gport_to_global_lif_convert(unit,
                                lookupOutLif[index],
                                FALSE,
                                &(hwAct[ppdIndex + index].hwParam)));
                    }
                    LOG_DEBUG(BSL_LS_BCM_FP,
                            (BSL_META_U(unit,
                                    "GPORT %08X lookup resulted"
                                    " in outlif %08X\n"),
                                    lookupOutLif[index],
                                    hwAct[ppdIndex + index].hwParam));
                } /* if (lookupOutLif[index]) */
            } /* for (index = 0; index < actCount; index++) */
            /* Handle GPORT lookups if needed */
            for (index = 0; index < actCount; index++) {
                if (BCM_GPORT_INVALID != lookupGport[index]) {
                    result = _bcm_dpp_gport_to_fwd_decision(unit,
                                                            lookupGport[index],
                                                            fwdDecision);
                    if (BCM_E_NONE != result) {
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unable to compute forwarding"
                                              " decision for unit %d GPORT"
                                              " %08X: %d (%s)\n"),
                                   unit,
                                   lookupGport[index],
                                   result,
                                   _SHR_ERRMSG(result)));
                        BCMDNX_IF_ERR_EXIT(result);
                    }
                    /* Get the HW Destination from the Forward decision */
                    COMPILER_64_SET(mask_edata, 0, (~0));
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_qualify_fwd_decision_to_hw_destination_convert(unit,
                                                                                                      fwdDecision,
                                                                                                      TRUE, /* is_for_destination */
                                                                                                         TRUE, /* is_for_action */
                                                                                                         &dest_edata,
                                                                                                         &mask_edata),
                                           (_BSL_BCM_MSG_NO_UNIT("unable to get from"
                                                                 " the forward"
                                                                 " decision the"
                                                                 " HW data\n")));
                    /* The GPORT lookup can override the action type! */
                    switch (fwdDecision->type) {
                    case SOC_PPC_FRWRD_DECISION_TYPE_TRAP:
                        /*
                         * The lookup-gport can come from Redirect also
                         * Allow Trap only for bcmFieldActionTrap
                         */
                        if ((SOC_PPC_FP_ACTION_TYPE_TRAP ==
                                 hwAct[ppdIndex + index].hwType) || (SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED ==
                                                                         hwAct[ppdIndex + index].hwType)) {
                            BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_trap_ppd_to_hw(unit,
                                                                                 fwdDecision->additional_info.trap_info.action_profile.trap_code,
                                                                                 &hwTrapCode),
                                                   (_BSL_BCM_MSG_NO_UNIT("unable to"
                                                                         " get from"
                                                                         " the HW"
                                                                         " Trap Id"
                                                                         " for PPD"
                                                                         " trap Id %d"
                                                                         " (%s)\n"),
                                                    fwdDecision->additional_info.trap_info.action_profile.trap_code,
                                                    SOC_PPC_TRAP_CODE_to_string(fwdDecision->additional_info.trap_info.action_profile.trap_code)));
                            /* Set the strength in 10:8 */
                            /* insert the trap qualifier in Arad */
                            hwAct[ppdIndex + index].hwParam = (hwAct[ppdIndex + index].hwParam << 11)
                                + (fwdDecision->additional_info.trap_info.action_profile.frwrd_action_strength << 8) + hwTrapCode;
                        } else if (SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP == hwAct[ppdIndex + index].hwType) {
                            BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_trap_ppd_to_hw(unit,
                                                                                 fwdDecision->additional_info.trap_info.action_profile.trap_code,
                                                                                 &hwTrapCode),
                                                   (_BSL_BCM_MSG_NO_UNIT("unable to"
                                                                         " get from"
                                                                         " the HW"
                                                                         " Trap Id"
                                                                         " for PPD"
                                                                         " trap Id %d"
                                                                         " (%s)\n"),
                                                    fwdDecision->additional_info.trap_info.action_profile.trap_code,
                                                    SOC_PPC_TRAP_CODE_to_string(fwdDecision->additional_info.trap_info.action_profile.trap_code)));
                            /* Set the strength in 12:10 */
                            /* Keep the 2-bits lsb mode */
                            hwAct[ppdIndex + index].hwParam =(fwdDecision->additional_info.trap_info.action_profile.frwrd_action_strength << 10) +
                                (hwTrapCode << 2) + (hwAct[ppdIndex + index].hwParam & 0x3);
                        } else {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                                (_BSL_BCM_MSG_NO_UNIT("%s action can"
                                                                      " not accept"
                                                                      " GPORT %08X\n"),
                                                 _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                                 lookupGport[index]));
                        }

                        break; 

                    case SOC_PPC_FRWRD_DECISION_TYPE_DROP:
                        if (SOC_PPC_FP_ACTION_TYPE_DEST !=
                            hwAct[ppdIndex + index].hwType) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG_NO_UNIT("%s action can"
                                                               " not accept"
                                                               " GPORT %08X\n"),
                                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                              lookupGport[index]));
                        }

                        if (SOC_IS_JERICHO_PLUS(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "field_action_redirect_jer_style", 0)) {
                            hwAct[ppdIndex + index].hwParam = dest_edata;
                        } else {
                            hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_DROP_DEST(unit);
                        }
                        break;
                    case SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW:
                    case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
                    case SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT:
                    case SOC_PPC_FRWRD_DECISION_TYPE_MC:
                    case SOC_PPC_FRWRD_DECISION_TYPE_FEC:
                        /* all of these look like 'DEST' action */
                        /* Special cases */
						hwAct[ppdIndex + index].hwParam = COMPILER_64_LO(dest_edata);
                        if ((hwAct[ppdIndex + index].hwType == SOC_PPC_FP_ACTION_TYPE_SRC_SYST_PORT)
                            || (hwAct[ppdIndex + index].hwType == SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_0_TO_15)) {
                            /* Take only the 16 LSBs*/
                            hwAct[ppdIndex + index].hwParam = hwAct[ppdIndex + index].hwParam & 0xFFFF;
                        }
                        break;
					default:
						hwAct[ppdIndex + index].hwParam = COMPILER_64_LO(dest_edata);
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("GPORT %08X lookup"
                                                           " resulted in"
                                                           " unsupported"
                                                           " decision type %d\n"),
                                          lookupGport[index],
                                          fwdDecision->type));
                    }
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "GPORT %08X lookup resulted in"
                                          " decision type %d -> action %d"
                                          " param %08X\n"),
                               lookupGport[index],
                               fwdDecision->type,
                               hwAct[ppdIndex + index].hwType,
                               (int)(hwAct[ppdIndex + index].hwParam)));
                } /* if (lookupGport[index]) */
            } /* for (index = 0; index < actCount; index++) */

        }
        else if (SOC_PPC_FP_DATABASE_STAGE_EGRESS ==
                _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).hwStageId) 
        {
            switch (propActs[bcmIndex].bcmType) {
            case bcmFieldActionDrop:
                /* param0 = n/a */
                /* param1 = n/a */
                /* egress drop: send to a trap profile configured to drop frames */
                if (propActs[bcmIndex].bcmParam0 ||
                    propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) and param1(%d) are both"
                                         " ignored for %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_EGR_TRAP;
                hwAct[ppdIndex].hwFlags = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
                hwAct[ppdIndex].hwParam = SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID;
                break;
            case bcmFieldActionDropCancel:
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_EGR_TRAP;
                hwAct[ppdIndex].hwFlags = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_NONE;
                hwAct[ppdIndex].hwParam = _BCM_PETRA_FIELD_EGR_TRAP_PROFILE_DEFAULT >> 1 | 1;
                break;
            case bcmFieldActionStat:
            {
                stat_id_mask = 0 ;
                ace_pointer_prge_var_nof_bits = 10 ;
                if (SOC_IS_JERICHO(unit)) 
                {

                    BCMDNX_IF_ERR_EXIT_MSG(
                        _bcm_dpp_field_ace_entry_add(unit,&(propActs[bcmIndex]),core_id,ppdIndex,hwAct,&actCount),
                        (_BSL_BCM_MSG("unit %d, failed on _bcm_dpp_field_ace_entry_add()"), unit));
                }
                else
                {
                    /*
                     * Enter for ARAD and ARAD_PLUS only.
                     */

                    ace_pointer_prge_var_in_stat_id_lsb =
                        (_SHR_FIELD_CTR_PROC_SHIFT_GET(propActs[bcmIndex].bcmParam0) - ace_pointer_prge_var_nof_bits);

                    /* param0 = Stat-ID */
                    /* param1 = GPORT or port number */

                    if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                        /* gport mode */
                        lookupGport[0] = propActs[bcmIndex].bcmParam1;
                    } else { /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT(" non-GPORT"
                                                           " param1(%d) is not"
                                                           " valid gport"),
                                          propActs[bcmIndex].bcmParam1));
                    
                    } /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */

                    /* Convert gport to TM-Port and PP-Port */
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_gport_to_pp_port_tm_port_convert(unit,
                                                                                        lookupGport[0],
                                                                                        &pp_port,
                                                                                        &tm_port,
                                                                                        &core_lcl),
                                        (_BSL_BCM_MSG_NO_UNIT("unable to get the TM"
                                                          " or PP port with"
                                                          " gport %08X"),
                                         lookupGport[0]));

                    /* Check the Stat-ID is in range: stat-id 0 means do no count */
                    SHR_BITSET_RANGE(&stat_id_mask, ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                    stat_id = propActs[bcmIndex].bcmParam0 & (~stat_id_mask);
                    if (stat_id != 0) 
                    {
                        result = _bcm_dpp_proc_cntr_from_stat(unit,
                                                              stat_id,
                                                              &proc,
                                                              &cntr);
                        if (BCM_E_NONE != result) {
                            BCMDNX_IF_ERR_EXIT(result);
                        }
                    } else {
                        /* For invalid stat, write the PP-Port in the last lines */
                        cntr = _BCM_PETRA_CTR_MAX_VALUE_EGRESS_ARAD + pp_port;
                    }
                    /* Get an ACE-Port to modify the PP-Port */
                    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
                    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
                    control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT;
                    control_ndx.val_ndx = cntr;
                    control_info.val[0] = pp_port;
                    SHR_BITCOPY_RANGE(&(control_info.val[1]), 0, &(propActs[bcmIndex].bcmParam0), 
                                       ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                    ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                                  core_id,
                                                  &control_ndx,
                                                  &control_info,
                                                  &success);
                    result = handle_sand_result(ppdr);
                    if (BCM_E_NONE == result) {
                        result = translate_sand_success_failure(success);
                    }
                    BCMDNX_IF_ERR_EXIT(result);
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_ACE_POINTER;
                    /* In Arad, LSB indicates if valid */
                    hwAct[ppdIndex].hwParam = cntr;
                    /* takes a single action when not counting */
                    actCount = 1;
                    if (propActs[bcmIndex].bcmParam0 != 0) 
                    {
                        hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_COUNTER_PROFILE;
                        hwAct[ppdIndex + 1].hwParam = 0x1; /* Counter-ID = ACE-Pointer */
                        /* takes two actions */
                        actCount = 2;
                    }
                }
                break;
            }
            case bcmFieldActionStatVportNew:
                /* param0 = Stat-ID */
                /* param1 = outlif */
                if (SOC_IS_JERICHO(unit)) 
                {
                    /*
                     * This case is not active any more on Jericho. It is replaced by
                     * 'bcmFieldActionStat' with: param0 as counter and param2 as outlif.
                     */
                    BCMDNX_ERR_EXIT_MSG(
                        BCM_E_PARAM,
                        (_BSL_BCM_MSG_NO_UNIT("%s(): This case (bcmFieldActionStatVportNew) is not active on Jericho. Use bcmFieldActionStat with param0 and param2"),
                        __func__ )) ;
                }
                outlif_id = propActs[bcmIndex].bcmParam1;

                ace_pointer_prge_var_in_stat_id_lsb = (_SHR_FIELD_CTR_PROC_SHIFT_GET(propActs[bcmIndex].bcmParam0) - ace_pointer_prge_var_nof_bits);

                /* Check the Stat-ID is in range: stat-id 0 is invalid */
                SHR_BITSET_RANGE(&stat_id_mask, ace_pointer_prge_var_in_stat_id_lsb, ace_pointer_prge_var_nof_bits);
                stat_id = propActs[bcmIndex].bcmParam0 & (~stat_id_mask);
                if (stat_id != 0) 
                {
                    result = _bcm_dpp_proc_cntr_from_stat(unit,
                                                          stat_id,
                                                          &proc,
                                                          &cntr);
                    if (BCM_E_NONE != result) {
                        BCMDNX_IF_ERR_EXIT(result);
                    }

                    if ((cntr >= _BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD) 
                        || (cntr <1)) {
                        /* does not use the correct ACE-Pointer, so error */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT(" Stat-ID param0(%d)"
                                                           " is invalid. It must"
                                                           " in range %d..%d\n"),
                                          stat_id,
                                          1,
                                          (_BCM_PETRA_CTR_BASE_VALUE_EGRESS_ARAD - 1)));

                    }
                } else {
                    /* For invalid stat, return error */
                     BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT(" Stat-ID param0(%d)"
                                                           " is invalid.")));
                }
                /* Get an ACE-Port to modify the PP-Port */
                SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
                SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
                control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_OUT_LIF;
                control_ndx.val_ndx = cntr;
                control_info.val[0] = outlif_id;

                ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                              core_id,
                                              &control_ndx,
                                              &control_info,
                                              &success);
                result = handle_sand_result(ppdr);
                if (BCM_E_NONE == result) {
                    result = translate_sand_success_failure(success);
                }
                BCMDNX_IF_ERR_EXIT(result);
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_ACE_POINTER;
                /* In Arad, LSB indicates if valid */
                hwAct[ppdIndex].hwParam = cntr;
                /* takes a single action when not counting */
                actCount = 1;
                if (propActs[bcmIndex].bcmParam0 != 0) 
                {
                        hwAct[ppdIndex + 1].hwType = SOC_PPC_FP_ACTION_TYPE_COUNTER_PROFILE;
                    hwAct[ppdIndex + 1].hwParam = 0x1; /* Counter-ID = ACE-Pointer */
                    /* takes two actions */
                    actCount = 2;
                }
                break;
            case bcmFieldActionDropPrecedence:
                /* param0 = new DP (BCM_FIELD_COLOR_*) */
                /* param1 = n/a */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Translate to HW Drop precedence */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_color_bcm_to_ppd(propActs[bcmIndex].bcmParam0,
                                                                        &(hwAct[ppdIndex].hwParam)));
                if (SOC_IS_JERICHO(unit))
                {
                    /* but also need to tell it to always set the value  - valid bit*/
                    hwAct[ppdIndex].hwParam |= 0x04;
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_DP;
                }
                else
                {
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_EGR_TRAP;
                    hwAct[ppdIndex].hwFlags = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP;
                }
                break;
            case bcmFieldActionRedirect:
                /* This action is now supported in Jericho as well (See ace_type below) */
                /* param0 = module */
                /* param1 = GPORT or port number */
                /* egress redirect, only supports local port */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                    /* gport mode */
                    lookupGport[0] = propActs[bcmIndex].bcmParam1;
                } else { /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */
                    /* raw module,port mode */
                    if (myModId != propActs[bcmIndex].bcmParam0) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d module %d"
                                                           " does not support"
                                                           " egress redirect"
                                                           " to other module\n"),
                                          unit,
                                          myModId));
                    }
                    if ((_SHR_GPORT_PORT_MASK <
                         propActs[bcmIndex].bcmParam1) ||
                        (_SHR_GPORT_MODID_MASK <
                         propActs[bcmIndex].bcmParam0)) {
                        /* can't represent it as modport, so error */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT"
                                                           " param0(%d) is not"
                                                           " valid modid"
                                                           " (0..%d) or"
                                                           " param1(%d) is not"
                                                           " valid port"
                                                           " (0..%d)\n"),
                                          propActs[bcmIndex].bcmParam0,
                                          _SHR_GPORT_MODID_MASK,
                                          propActs[bcmIndex].bcmParam1,
                                          _SHR_GPORT_PORT_MASK));
                    }
                    /* build modport from these arguments */
                    BCM_GPORT_MODPORT_SET(lookupGport[0],
                                          propActs[bcmIndex].bcmParam0,
                                          propActs[bcmIndex].bcmParam1);
                } /* (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) */
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_EGR_OFP;
                /* Convert gport to TM-Port and PP-Port */
                BCMDNX_IF_ERR_EXIT_MSG(
                    _bcm_dpp_field_gport_to_pp_port_tm_port_convert(
                        unit,
                        lookupGport[0],
                        &pp_port,
                        &(hwAct[ppdIndex].hwParam),
                        &core_lcl),
                    (_BSL_BCM_MSG_NO_UNIT("unable to get the TM or PP port with gport %08X"),
                        lookupGport[0]));
                break;
            case bcmFieldActionRedirectVportPort:
                /* param0 = vport */
                /* param1 = port */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1)) {
                    /* gport mode */
                    lookupGport[0] = propActs[bcmIndex].bcmParam1;
                } else {
                    /* local port */
                    /* coverity[unsigned_compare : FALSE] */ 
                    if (SOC_PORT_VALID(unit, propActs[bcmIndex].bcmParam1)) {
                        BCM_GPORT_LOCAL_SET(lookupGport[0],propActs[bcmIndex].bcmParam1);
                    } else
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("non-GPORT param1"
                                                           " %d is not valid"),
                                          propActs[bcmIndex].bcmParam1));
                    } 
		    
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_gport_to_pp_port_tm_port_convert(unit, 
		    			   lookupGport[0], 
					   &pp_port, 
					   &tm_port, 
					   &core_lcl), 
					   (_BSL_BCM_MSG_NO_UNIT("unable to get the TM" " or PP port with" 
					   " gport %08X\n"), 
					   lookupGport[0]));

                    hwAct[ppdIndex].hwParam = tm_port;

                    /* Use the action-profile action instead in Arad - no simple Out-LIF action */

                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_EGR_TRAP;
                    hwAct[ppdIndex].hwFlags = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
                    actCount = 1;

					if (propActs[bcmIndex].bcmParam0 != BCM_FORWARD_ENCAP_ID_INVALID)
					{
						result = _bcm_dpp_field_gport_to_global_lif_convert(unit,
												propActs[bcmIndex].bcmParam0,
												FALSE,
												&(hwAct[ppdIndex+1].hwParam));
						BCMDNX_IF_ERR_EXIT(result);

						LOG_DEBUG(BSL_LS_BCM_FP,
								(BSL_META_U(unit, "GPORT %08X lookup resulted in outlif %08X\n"),
								propActs[bcmIndex].bcmParam0, hwAct[ppdIndex+1].hwParam));
									hwAct[ppdIndex+1].hwType = SOC_PPC_FP_ACTION_TYPE_EGR_TRAP;
									hwAct[ppdIndex+1].hwFlags = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD;
						actCount = 2;
					}

                break ;
            case bcmFieldActionPrioIntNew:
                /* param0 = new priority (internal only?) */
                /* param1 (misdoc as N/A?) contains marking flags(???) */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                if (7 < propActs[bcmIndex].bcmParam0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("%s param0 (new priority)"
                                                       " %d, but must be (0..7)\n"),
                                      _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                      propActs[bcmIndex].bcmParam0));
                }
                hwAct[ppdIndex].hwParam = (1 /* Valid bit */ |
                                           ((propActs[bcmIndex].bcmParam0 &
                                             0x7) << 1)) /* TC << 1 */;
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_TC;
                break;
            case bcmFieldActionMirrorEgress:
                /* param0 = destination module (ignored if port is a GPORT) */
                /* param1 = destination port OR GPORT */
                if (BCM_GPORT_IS_SET(propActs[bcmIndex].bcmParam1) &&
                    BCM_GPORT_IS_MIRROR(propActs[bcmIndex].bcmParam1)) {
                    /*
                     *  Using GPORT mode and the GPORT is a mirror
                     */
                    mirrorId = BCM_GPORT_MIRROR_GET(propActs[bcmIndex].bcmParam1);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d does not allow"
                                                       " implicit mirror"
                                                       " allocations.  Param1"
                                                       " %08X must be (but is"
                                                       " not) mirror gport\n"),
                                      unit,
                                      propActs[bcmIndex].bcmParam1));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_MIRROR;
                hwAct[ppdIndex].hwParam = mirrorId;
                break;
            case bcmFieldActionQosMapIdNew:
                /* In Arad, indicated as useless.. */
                /* param0 = QoS Map Id of type egress */
                /* param1 = N/A */
                if (propActs[bcmIndex].bcmParam1) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param1(%d) is ignored for %s"
                                         " action\n"),
                              propActs[bcmIndex].bcmParam1,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                /* Check the QOS map profile is of type Egress */
                if (!BCM_INT_QOS_MAP_IS_EGRESS(propActs[bcmIndex].bcmParam0)) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "param0(%d) must be type Egress for"
                                         " %s action\n"),
                              propActs[bcmIndex].bcmParam0,
                              _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                }
                hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_COS_PROFILE;
                hwAct[ppdIndex].hwParam = BCM_QOS_MAP_PROFILE_GET(propActs[bcmIndex].bcmParam0) & 0xF; /* Encoded in 4 bits */
                break;
            default:
                /* should not get here */
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, entryGroup)) ;
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unexpected action type"
                                                   " %d(%s) on unit %d group"
                                                   " %u entry %u\n"),
                                  propActs[bcmIndex].bcmType,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                  unit,
                                  /*
                                   * Was:
                                   *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                                   */
                                  entryGroup,
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
            } /* switch (propActs[index].bcmType) */
            for (index = 0; index < actCount; index++) {
                if ((hwAct[ppdIndex + index].hwType == SOC_PPC_FP_ACTION_TYPE_EGR_OFP)) {
                    int rv, core;
                    uint32 pp_port;

                    if (SOC_IS_JERICHO(unit)) {
                        hwAct[ppdIndex + index].hwParam = (hwAct[ppdIndex + index].hwParam << 1) | 1;
                    }
                    else {
                        /* Map from local-port to PP-Port to Base-Q-Pair */
                        rv = soc_port_sw_db_local_to_pp_port_get(unit,
                                                              hwAct[ppdIndex + index].hwParam, &pp_port, &core);
                        if (BCM_FAILURE(rv)) { /* function returns valid port number or error ID */
                            BCMDNX_ERR_EXIT_MSG(hwAct[ppdIndex + index].hwParam,
                                             (_BSL_BCM_MSG_NO_UNIT("unable to map port"
                                                               "%d (%08X) to tm"
                                                               "port\n"),
                                              hwAct[ppdIndex + index].hwParam,
                                              hwAct[ppdIndex + index].hwParam));
                        }
                        rv = soc_port_sw_db_tm_port_to_base_q_pair_get(unitData->unitHandle, core, pp_port, &hwAct[ppdIndex + index].hwParam);
                        if (BCM_FAILURE(rv)) { /* function returns valid port number or error ID */
                            BCMDNX_ERR_EXIT_MSG(hwAct[ppdIndex + index].hwParam,
                                             (_BSL_BCM_MSG_NO_UNIT("unable to map port"
                                                               "%d (%08X) to base queue"
                                                               "pair\n"),
                                              hwAct[ppdIndex + index].hwParam,
                                              hwAct[ppdIndex + index].hwParam));
                        }

                    }
                }
            } /* for (index = 0; index < actCount; index++) */
        } else if (SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP == _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).hwStageId) {
            switch (propActs[bcmIndex].bcmType) {
                case bcmFieldActionExternalValue0Set:/*48 bit so param1 can be used*/
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0;
                    if (SOC_IS_JERICHO(unit)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[0]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else if (propActs[bcmIndex].bcmParam1) {
                        actCount = 2;
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;
                case bcmFieldActionExternalValue1Set:
                    /* param0 = Value */
                    /* Jericho - param1 = Value
                       Arad+   - param1 = n/a */
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_1;

                    if (SOC_IS_JERICHO(unit)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[1]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,(BSL_META_U(unit,"param1(%d) is"" ignored for %s action\n"),
                                propActs[bcmIndex].bcmParam1,_bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_1;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;
                case bcmFieldActionExternalValue2Set:
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_2;

                    if (SOC_IS_JERICHO(unit)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[2]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,(BSL_META_U(unit,"param1(%d) is"" ignored for %s action\n"),
                                propActs[bcmIndex].bcmParam1,_bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_2;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;
                case bcmFieldActionExternalValue3Set:
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_3;

                    if (SOC_IS_JERICHO(unit)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[3]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else if (propActs[bcmIndex].bcmParam1) {
                        LOG_WARN(BSL_LS_BCM_FP,(BSL_META_U(unit,"param1(%d) is"" ignored for %s action\n"),
                                propActs[bcmIndex].bcmParam1,_bcm_dpp_field_action_name[propActs[bcmIndex].bcmType]));
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_3;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;
                case bcmFieldActionExternalValue4Set:
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_4;

                    if (SOC_IS_JERICHO(unit)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[4]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else if (propActs[bcmIndex].bcmParam1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                           (_BSL_BCM_MSG_NO_UNIT("Action bcmFieldActionExternalValue4Set"
                                                                  "is not valid on that device")));
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_4;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;
                case bcmFieldActionExternalValue5Set:
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_5;

                    if (SOC_IS_JERICHO(unit)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[5]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else if (propActs[bcmIndex].bcmParam1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                           (_BSL_BCM_MSG_NO_UNIT("Action bcmFieldActionExternalValue5Set"
                                                                  "is not valid on that device")));
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_5;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;
                case bcmFieldActionExternalValue6Set:
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_6;

#if defined(INCLUDE_KBP)
                    if (SOC_IS_JERICHO(unit) &&
                        (ARAD_KBP_IS_OP_OR_OP2)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[6]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else
#endif
                    if (propActs[bcmIndex].bcmParam1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("Action bcmFieldActionExternalValue6Set"
                                                               "is not valid on that device")));
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_6;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;
                case bcmFieldActionExternalValue7Set:
                    hwAct[ppdIndex].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7;

#if defined(INCLUDE_KBP)
                    if (SOC_IS_JERICHO(unit) &&
                        (ARAD_KBP_IS_OP_OR_OP2)) {
                        actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[7]);

                        if ((actCount == 1) && (propActs[bcmIndex].bcmParam1 > 0) && (propActs[bcmIndex].bcmParam1 != BCM_ILLEGAL_ACTION_PARAMETER)) {
                            actCount = 2;
                        }
                    } else
#endif
                    if (propActs[bcmIndex].bcmParam1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("Action bcmFieldActionExternalValue7Set"
                                                               "is not valid on that device")));
                    }

                    for (index = 1; index < actCount; index++) {
                        hwAct[ppdIndex + index].hwType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7;
                        hwAct[ppdIndex + index].hwParam = _BCM_DPP_FIELD_TCAM_PARA_GET(index, bcmIndex);
                    }

                    break;

                default:
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, entryGroup)) ;
                /* should not get here */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unexpected action type"
                                                   " %d(%s) on unit %d group"
                                                   " %u entry %u\n"),
                                  propActs[bcmIndex].bcmType,
                                  _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                                  unit,
                                  /*
                                   * Was:
                                   *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                                   */
                                  entryGroup,
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
                } /* switch (propActs[index].bcmType) */

                /* Limited to 32b */
                hwAct[ppdIndex].hwParam = propActs[bcmIndex].bcmParam0;
        } else {

            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, entryGroup)) ;
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u group %u"
                                               " stage %u is unsupported"
                                               " hardware stage type %d\n"),
                              unit,
                              _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                              /*
                               * Was:
                               *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                               */
                              entryGroup,
                              groupD_stage,
                              _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).hwStageId));
        }
        /* go to the next available PPD slot */
        if (ppdIndex + actCount <= SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
            while (actCount) {
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, isExternalTcam, entry, entryGroup)) ;
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d entry %u group %u stage %u"
                                      " BCM action %u (%s(%d):%u(%08X),"
                                      "%u(%08X),%u(%08X)) mapped to PPD action %u"
                                      " (%s(%d)%08X:%u(%08X))\n"),
                           unit,
                           _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                           /*
                            * Was:
                            *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                            */
                           entryGroup,
                           groupD_stage,
                           bcmIndex,
                           _bcm_dpp_field_action_name[propActs[bcmIndex].bcmType],
                           propActs[bcmIndex].bcmType,
                           propActs[bcmIndex].bcmParam0,
                           propActs[bcmIndex].bcmParam0,
                           propActs[bcmIndex].bcmParam1,
                           propActs[bcmIndex].bcmParam1,
			   propActs[bcmIndex].bcmParam2,
			   propActs[bcmIndex].bcmParam2,
                           ppdIndex,
                           SOC_PPC_FP_ACTION_TYPE_to_string(hwAct[ppdIndex].hwType),
                           hwAct[ppdIndex].hwType,
                           hwAct[ppdIndex].hwFlags,
                           hwAct[ppdIndex].hwParam,
                           hwAct[ppdIndex].hwParam));
                ppdIndex++;
                actCount--;
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u has too many"
                                               " hardware actions (max %u)\n"),
                              unit,
                              _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                              SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX));
        }
        /* this BCM action has been considered */
        SHR_BITSET(actHit, bcmIndex);
    } /* for (all BCM layer action slots) */

    /* make sure there are no duplicate actions */
    for (ppdIndex = 0; ppdIndex < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ppdIndex++) {
        for (index = ppdIndex + 1;
             index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX;
             index++) {
            if (!_BCM_DPP_FIELD_PPD_ACTION_VALID(hwAct[ppdIndex].hwType)) {
                /* not an action; skip this one */
                continue;
            }
            if (hwAct[ppdIndex].hwType != hwAct[index].hwType) {
                /* not the same action; skip this one */
                continue;
            }
            if ((SOC_PPC_FP_ACTION_TYPE_EGR_TRAP == hwAct[ppdIndex].hwType) &&
                (hwAct[ppdIndex].hwFlags != hwAct[index].hwFlags)) {
                /* not the same underlying action; skip this one */
                continue;
            }

            if (SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0 == hwAct[ppdIndex].hwType) {
                /* for external value 0 we allow two identical actions, this action can be 48bit so in the SOC we concatenating the actions*/
                continue;
            }
			if (SOC_IS_JERICHO(unit)&&
				((SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_1 == hwAct[ppdIndex].hwType)||
				 (SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_2 == hwAct[ppdIndex].hwType)||
				 (SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_3 == hwAct[ppdIndex].hwType)||
				 (SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_4 == hwAct[ppdIndex].hwType)||
                 (SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_5 == hwAct[ppdIndex].hwType)||
				 (SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_6 == hwAct[ppdIndex].hwType)||
                 (SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7 == hwAct[ppdIndex].hwType))
			){
                /* JER: for external value 1,2,3,4,5 we allow two identical actions, this action can be 64bit so in the SOC we concatenating the actions*/
                continue;
            }
            /* two hardware actions of the same type */
            if (hwAct[ppdIndex].hwParam != hwAct[index].hwParam) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u"
                                                   " hardware actions %u"
                                                   " and %u are both the"
                                                   " same action %s (%d) %08X"
                                                   " but their parameters"
                                                   " differ (%08X, %08X)\n"),
                                  unit,
                                  _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                                  ppdIndex,
                                  index,
                                  SOC_PPC_FP_ACTION_TYPE_to_string(hwAct[ppdIndex].hwType),
                                  hwAct[ppdIndex].hwType,
                                  hwAct[ppdIndex].hwFlags,
                                  hwAct[ppdIndex].hwParam,
                                  hwAct[index].hwParam));
            } else { /* if (hardware action params not equal) */
                /* but we don't want two of it even if duplicate */
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %u has two hardware"
                                     " actions %d and %d of type %s (%d)"
                                     " %08X with same parameter %08X\n"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          ppdIndex,
                          index,
                          SOC_PPC_FP_ACTION_TYPE_to_string(hwAct[ppdIndex].hwType),
                          hwAct[ppdIndex].hwType,
                          hwAct[ppdIndex].hwFlags,
                          hwAct[ppdIndex].hwParam));
                hwAct[index].hwType = SOC_PPC_FP_ACTION_TYPE_INVALID;
                hwAct[index].hwParam = ~0;
                hwAct[index].hwFlags = 0;
            } /* if (hardware action params not equal) */
        } /* for (all hardware actions after the current one) */
    } /* for (all hardware actions) */

    /* Everything went well to this point, commit the changes. */
    commit = TRUE;
exit:
    {
        _bcm_dpp_field_tc_p_act_t *loc_tcActP = NULL;
        _bcm_dpp_field_tc_p_act_t *relActs;
        _bcm_dpp_field_tc_p_act_t *keepActs;

        BCMDNX_ALLOC(loc_tcActP, 
                     sizeof(*loc_tcActP) * SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX, 
                     "_bcm_dpp_field_tcam_entry_action_update.loc_tcActP");
        if (loc_tcActP == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
        }

        /* Must not abort function on error from here on */
        /*
         * Load loc_tcActP[] from SW state db
         * (Not using _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_GET() because of error handling)
         */
        for (ppdIndex = 0; ppdIndex < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ppdIndex++) {
            if (!isExternalTcam) {
                FIELD_ACCESS_ENTRYTC.tcActP.get(unit,entry,ppdIndex,&loc_tcActP[ppdIndex]) ;
            } else {
                uint32 ext_info_idx ;
                FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,entry,&ext_info_idx) ;
                FIELD_ACCESS_EXTTCAMINFO.tcActP.get(unit,ext_info_idx,ppdIndex,&loc_tcActP[ppdIndex]) ;
            }
        }
        if (commit) {
            /* Will release any resources used by the old actions */
            /*
             * Was:
             *   relActs = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry);
             */
            relActs = &loc_tcActP[0] ;
            keepActs = &(hwAct[0]);
        } else { /* if (commit) */
            /* Will release any resources used by the new actions */
            relActs = &(hwAct[0]);
            /*
             * Was:
             *   keepActs = _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry);
             */
            keepActs = &loc_tcActP[0] ;
        } /* if (commit) */

        
        /*
         *  NOTE: errors that occur in this loop must not cause immediate exit; it
         *  might be reasonable to report them in some way, but everything here is
         *  freeing a resource so should not fail unless internal state is
         *  otherwise inconsistent.
         */
        for (ppdIndex = 0; ppdIndex < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ppdIndex++) {
            /* Look for the same hardware action in the 'keep' set */
            for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
                /* coverity[var_deref_op : FALSE] */
                if (keepActs[index].hwType ==
                    relActs[ppdIndex].hwType) {
                    /* this is the same hardware action */
                    break;
                }
            }
            switch (relActs[ppdIndex].hwType) {
                default:
                    /* most actions do not need special resource work */
                    break;
            } /* switch (relActs[ppdIndex].hwType) */
        } /* for (all actions in the released actions) */

        BCM_FREE(loc_tcActP);
    }
    if (commit) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %u BCM actions successfully"
                              " converted to PPD actions; committing the"
                              " changes to the entry\n"),
                   unit,
                   _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
        for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) {
            _bcm_dpp_field_tc_b_act_t tempActs ;

            /*
             * Was:
             *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType = propActs[index].bcmType;
             *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam0 = propActs[index].bcmParam0;
             *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam1 = propActs[index].bcmParam1;
             */
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, entry, index, tempActs) ;
            tempActs.bcmType = propActs[index].bcmType;
            tempActs.bcmParam0 = propActs[index].bcmParam0;
            tempActs.bcmParam1 = propActs[index].bcmParam1;
            tempActs.bcmParam2 = propActs[index].bcmParam2;
            tempActs.bcmParam3 = propActs[index].bcmParam3;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_SET(unit,isExternalTcam, entry, index, tempActs) ;
        }
        for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
            /*
             * Was:
             *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType = hwAct[index].hwType;
             *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwParam = hwAct[index].hwParam;
             *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwFlags = hwAct[index].hwFlags;
             */
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_SET(unit,isExternalTcam, entry, index, hwAct[index].hwType) ;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWFLAGS_SET(unit,isExternalTcam, entry, index, hwAct[index].hwFlags) ;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWPARAM_SET(unit,isExternalTcam, entry, index, hwAct[index].hwParam) ;
        }
    } /* if (commit) */

    BCM_FREE(hwAct);
    BCM_FREE(fwdDecision);
    BCM_FREE(GroupData);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_action_clear(bcm_dpp_field_info_OLD_t *unitData,
                                       uint8 isExternalTcam,
                                       _bcm_dpp_field_ent_idx_t entry,
                                       bcm_field_action_t type)
{
    _bcm_dpp_field_tc_b_act_t tempActs[_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX];
    unsigned int index;
    bcm_field_aset_t groupD_aset;
    uint32 entryFlags;
    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
 
    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d, %u, %d(%s))\n"),
                 unit,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                 type,
                 ((type<bcmFieldActionCount) && (type != BCM_FIELD_ENTRY_INVALID))?_bcm_dpp_field_action_name[type]:"???"));
    if ((0 > type) || (bcmFieldActionCount <= type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d not valid\n"), type));
    }
    /*
     * Was:
     *   if (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE))
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, isExternalTcam, entry, entryFlags)) ;
    if (0 == (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not in use\n"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    }
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
    /*
     * was:
     *   BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.aset.get(unit, _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry), &groupD_aset));
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.aset.get(unit, entryGroup, &groupD_aset));
    if (!BCM_FIELD_ASET_TEST(groupD_aset,
                             type)) {
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d) ASET"
                                           " does not include %d(%s)\n"),
                          unit,
                          entry,
                          /*
                           * was:
                           *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                           */
                          entryGroup,
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
    {
        int tcActB_index ;

        /*
         * Was:
         *   sal_memcpy(&(tempActs[0]),
         *              _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry),
         *              sizeof(tempActs[0]) * _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX);
         */
        for (tcActB_index = 0 ; tcActB_index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX ; tcActB_index++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, entry, tcActB_index, (tempActs[tcActB_index])) ;
        }
    }
    for (index = 0;
         index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index++) {
        if (type == tempActs[index].bcmType) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u action %d, %s(%d)"
                                  " %d(%08X) %d(%08X) will be removed\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       index,
                       _bcm_dpp_field_action_name[tempActs[index].bcmType],
                       tempActs[index].bcmType,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam1,
                       tempActs[index].bcmParam1));
            break;
        }
    }
    if (index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
        tempActs[index].bcmType = BCM_FIELD_ENTRY_INVALID;
        tempActs[index].bcmParam0 = 0;
        tempActs[index].bcmParam1 = 0;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u does not have"
                                           " action %s(%d)\n"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          _bcm_dpp_field_action_name[type],
                          type));
    }
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_action_update(unitData,
                                                                    isExternalTcam,
                                                                    SOC_CORE_INVALID,
                                                                    entry,
                                                                    &(tempActs[0])));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_clear_acts(bcm_dpp_field_info_OLD_t *unitData,
                                     uint8 isExternalTcam,
                                     _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_tc_b_act_t tempActs[_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX];
    unsigned int index;
    uint32 entryFlags;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
 
    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d, %u)\n"),
                 unit,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    /*
     * Was:
     *   if (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE))
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, isExternalTcam, entry, entryFlags)) ;
    if (0 == (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not in use\n"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    }
    {
        int tcActB_index ;

        /*
         * Was:
         *   sal_memcpy(&(tempActs[0]),
         *              _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry),
         *              sizeof(tempActs[0]) * _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX);
         */
        for (tcActB_index = 0 ; tcActB_index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX ; tcActB_index++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, entry, tcActB_index, (tempActs[tcActB_index])) ;
        }
    }
    for (index = 0;
         index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index++) {
        tempActs[index].bcmType = BCM_FIELD_ENTRY_INVALID;
        tempActs[index].bcmParam0 = 0;
        tempActs[index].bcmParam1 = 0;
    }
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_action_update(unitData,
                                                                    isExternalTcam,
                                                                    SOC_CORE_INVALID,
                                                                    entry,
                                                                    &(tempActs[0])));
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_tcam_entry_clear(bcm_dpp_field_info_OLD_t *unitData,
                                uint8 isExternalTcam,
                           _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    unsigned int index;
    uint32 ext_info_idx = 0;
    int entryPriority;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
 
    /*
     * Was:
     *   next = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry);
     *   prev = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry);
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,isExternalTcam, entry, next)) ;
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,isExternalTcam, entry, prev)) ;
    if (isExternalTcam) {
        /*
         * Was:
         *   ext_info_idx = unitData->entryExtTc[entry].ext_info_idx;
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,entry,&ext_info_idx)) ;
    }

    /*
     * Was:
     *   sal_memset((void *)_BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, entry), 
     *         0x00, 
     *         isExternalTcam ? sizeof(_bcm_dpp_field_entry_ext_t) : sizeof(_bcm_dpp_field_entry_t));
     */
    if (isExternalTcam) {
        _bcm_dpp_field_entry_ext_t entryExtTc;
        sal_memset((void *)&entryExtTc, 0x00, sizeof(entryExtTc)) ;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.set(unit,entry,&entryExtTc)) ;
    } else {
        _bcm_dpp_field_entry_t entryTc;
        sal_memset((void *)&entryTc, 0x00, sizeof(entryTc)) ;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.set(unit,entry,&entryTc)) ;
    }

    /*
     * Was:
     *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, entry);
     *   *entryIdx = next;
     *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, entry);
     *   *entryIdx = prev;
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,isExternalTcam, entry, next)) ;
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,isExternalTcam, entry, prev)) ;

    /*
     * Was:
     *   entryPriority = _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_PTR(isExternalTcam, entry);
     *   *entryPriority = BCM_FIELD_ENTRY_PRIO_DEFAULT;
     */
    entryPriority = BCM_FIELD_ENTRY_PRIO_DEFAULT ;
    _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_SET(unit,isExternalTcam, entry, entryPriority) ;
    if (isExternalTcam) {
        /*
         * Was:
         *   unitData->entryExtTc[entry].ext_info_idx = ext_info_idx;
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.set(unit,entry,ext_info_idx)) ;
    }

    for (index = 0; index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; index++) {
        _bcm_dpp_field_qual_t qual ;

        /*
         * Was:
         *   _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType = bcmFieldQualifyCount;
         *   _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType = SOC_PPC_NOF_FP_QUAL_TYPES;
         */
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, entry, index, qual) ;
        qual.qualType = BCM_FIELD_ENTRY_INVALID;
        qual.hwType = BCM_FIELD_ENTRY_INVALID;
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_SET(unit,isExternalTcam, entry, index, qual) ;
    }
    {
        _bcm_dpp_field_tc_b_act_t tcActB_at_index ;
        
        /*
         * Was:
         *   for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) {
         *       _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType = bcmFieldActionCount;
         *   }
         */
        for (index = 0; index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX; index++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, entry, index, tcActB_at_index) ;
            tcActB_at_index.bcmType = BCM_FIELD_ENTRY_INVALID;
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_SET(unit,isExternalTcam, entry, index, tcActB_at_index) ;
        }
    }
    for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        SOC_PPC_FP_DATABASE_TYPE hwType;            /* hardware database type */
        /*
         * Was:
         *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, entry)[index].hwType = SOC_PPC_FP_ACTION_TYPE_INVALID;
         */
        hwType = SOC_PPC_FP_ACTION_TYPE_INVALID ;
        _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_SET(unit,isExternalTcam, entry, index, hwType) ;
    }
exit:
    BCMDNX_FUNC_RETURN_VOID ;
}

int
_bcm_dpp_field_tcam_entry_action_set(bcm_dpp_field_info_OLD_t *unitData,
                                     uint8 isExternalTcam,
                                     int core_id,
                                     _bcm_dpp_field_ent_idx_t entry,
                                     bcm_field_action_t type,
                                     uint32 param0,
                                     uint32 param1,
                                     uint32 param2,
                                     uint32 param3)
{
    _bcm_dpp_field_tc_b_act_t *tempActs = NULL;
    unsigned int index;
    unsigned int avail;
    bcm_field_aset_t *groupD_aset = NULL;
    _bcm_dpp_field_grp_idx_t entryGroup;     /* this entry's group */
    uint32 entryFlags;                       /* entry flags */

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
 
    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "(%d, %u, %d(%s), %d(%08X), %d(%08X), %d(%08X), %d(%08X)\n"),
                 unit,
                 _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                 type,
                 ((type<bcmFieldActionCount) && (type != BCM_FIELD_ENTRY_INVALID))?_bcm_dpp_field_action_name[type]:"???",
                 param0,
                 param0,
                 param1,
                 param1,
                 param2,
                 param2,
                 param3,
                 param3
    ));
    if ((0 > type) || (bcmFieldActionCount <= type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d not valid\n"), type));
    }
    /*
     * Was:
     *   if (0 == (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE))
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, entry, entryFlags)) ;
    if (0 == (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not in use\n"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam)));
    }
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;

    BCMDNX_ALLOC(groupD_aset, sizeof(*groupD_aset), "_bcm_dpp_field_tcam_entry_action_set.groupD_aset");
    if (groupD_aset == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
    BCMDNX_ALLOC(tempActs, sizeof(*tempActs)*_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX, "_bcm_dpp_field_tcam_entry_action_set.tempActs");
    if (tempActs == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    /*
     * Was:
     *   BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.aset.get(unit, _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry), &groupD_aset));
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.aset.get(unit, entryGroup, groupD_aset));
    if (!BCM_FIELD_ASET_TEST(*groupD_aset,
                             type)) {
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d) ASET"
                                           " does not include %d(%s)\n"),
                          unit,
                          entry,
                          /*
                           * Was:
                           *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                           */
                          entryGroup,
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
    {
        int tcActB_index ;

        /*
         * Was:
         *   sal_memcpy(&(tempActs[0]),
         *       _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry),
         *       sizeof(tempActs[0]) * _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX);
         */
        for (tcActB_index = 0 ; tcActB_index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX ; tcActB_index++) {
            _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, entry, tcActB_index, (tempActs[tcActB_index])) ;
        }
    }
    for (index = 0, avail = _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX;
         index++) {
        if (type == tempActs[index].bcmType) {
#if _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %u action %d, %s(%d)"
                                  " %d(%08X) %d(%08X) %d(%08X) %d(%08X) will be replaced\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       index,
                       _bcm_dpp_field_action_name[tempActs[index].bcmType],
                       tempActs[index].bcmType,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam0,
                       tempActs[index].bcmParam1,
                       tempActs[index].bcmParam1,
                       tempActs[index].bcmParam2,
                       tempActs[index].bcmParam2,
                       tempActs[index].bcmParam3,
                       tempActs[index].bcmParam3
            ));
            break;
#else /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d already has"
                                               " action %d (%s)\n"),
                              unit,
                              entry,
                              type,
                              _bcm_dpp_field_action_name[type]));
#endif /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
        } else if (BCM_FIELD_ENTRY_INVALID == tempActs[index].bcmType) {
            if (_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX <= avail) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d entry %d has action slot %d"
                                      " available\n"),
                           unit,
                           _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                           index));
                avail = index;
            }
        }
    } /* for (all valid BCM action slots) */
#if _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE
    if (index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
        /* use existing copy of this action instead of adding a new one */
        avail = index;
    }
#endif /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
    if (avail < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
        tempActs[avail].bcmType = type;
        tempActs[avail].bcmParam0 = param0;
        tempActs[avail].bcmParam1 = param1;
        tempActs[avail].bcmParam2 = param2;
        tempActs[avail].bcmParam3 = param3;
    } else { /* if (avail < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) */
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u has too many BCM"
                                           " layer actions (%u) to add more\n"),
                          unit,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          avail));
    } /* if (avail < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_action_update(unitData,
                                                                    isExternalTcam,
                                                                    core_id,
                                                                    entry,
                                                                    &(tempActs[0])));

    if (!(entryFlags & _BCM_DPP_FIELD_ENTRY_CHANGED)) {
        entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit, isExternalTcam, entry, entryFlags));
    }
exit:
    BCM_FREE(groupD_aset);
    BCM_FREE(tempActs);

    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_destroy(bcm_dpp_field_info_OLD_t *unitData,
                                  uint8 is_kaps_sw_remove,
                                  bcm_field_entry_t entry)
{
    int result = BCM_E_NONE;
    _bcm_dpp_field_grp_idx_t groupD_group;
    _bcm_dpp_field_entry_type_t entryType;
    _bcm_dpp_field_ent_idx_t entry_id;
    uint8 isExtInfoUsed = 0, isExternalTcam = FALSE;
    _bcm_dpp_field_ent_idx_t entryIntTcCount;
    _bcm_dpp_field_ent_idx_t entryExtTcCount;
    _bcm_dpp_field_ent_idx_t entryIntTcFree;
    _bcm_dpp_field_ent_idx_t entryExtTcFree;
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_stage_idx_t entryTcSh1;
    _bcm_dpp_field_ent_idx_t entryElems;
    _bcm_dpp_field_ent_idx_t tcam_entry_limit;
    bcm_field_group_mode_t groupD_grpMode; 
    _bcm_dpp_field_stage_idx_t groupD_stage; 
    _bcm_dpp_field_ent_idx_t groupD_entryCount;
    _bcm_dpp_field_ent_idx_t entryNext;         /* next entry     */
    _bcm_dpp_field_ent_idx_t entryPrev;         /* previous entry */
    uint32 entryFlags;                          /* entry flags */

    unsigned int entryCount;
    SOC_SAND_OCC_BM_PTR extTcamBmp;
    uint32 num;    

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((_BCM_DPP_FIELD_ENTRY_IS_LARGE_DIRECT_LOOKUP(entry)) && (is_kaps_sw_remove == FALSE)) {
        BCM_FIELD_LARGE_DIRECT_ENTRY_ID_UNPACK(entry, groupD_group, entry_id);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_remove(unitData, FALSE, entry));
        BCM_EXIT;
    }

    isExternalTcam = _BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry);
    entry_id = _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry);
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry_id, groupD_group)) ;
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, entry_id, entryFlags)) ;
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d(%08X),%d) enter (%s,%s)\n"),
               unit,
               PTR_TO_INT(unitData),
               entry,
               /*
                * Was:
                *   (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+IN_USE":"-in_use",
                *   (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw";
                */
               (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+IN_USE":"-in_use",
               (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw")) ;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, groupD_group, &groupD_stage));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, groupD_group, &groupD_grpMode));
    /*
     * was:
     *   if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, entry) & _BCM_DPP_FIELD_ENTRY_IN_USE)
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, entry_id, entryFlags)) ;
    if (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {

        FIELD_ACCESS.stageD.modeBits.entryType.get(unit, groupD_stage, groupD_grpMode, &entryType);
        /*
         * remove keep location flag for destory operation
         */
        if (entryFlags & _BCM_DPP_FIELD_ENTRY_KEEP_LOCATION){
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit,isExternalTcam, entry_id, (entryFlags&(~_BCM_DPP_FIELD_ENTRY_KEEP_LOCATION))|_BCM_DPP_FIELD_ENTRY_IN_HW)) ;
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.num_entry_in_keep_location.get(unit, groupD_group, &num));
            num = num - 1;
            if (num > 0) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.num_entry_in_keep_location.set(unit, groupD_group, num));
            }            
        }

        result = _bcm_dpp_field_tcam_entry_remove(unitData, is_kaps_sw_remove, entry);

        if (BCM_E_NONE == result) {
            /* clean up actions */
            uint32 ext_info_idx ;

            result = _bcm_dpp_field_tcam_entry_clear_acts(unitData, isExternalTcam, entry_id);

            if (isExternalTcam) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,entry_id,&ext_info_idx)) ;
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.extTcamBmp.get(unit, &extTcamBmp));
                soc_sand_occ_bm_is_occupied(unitData->unit, extTcamBmp, ext_info_idx, &isExtInfoUsed);
                if (isExtInfoUsed) {
                    /*
                     * Was:
                     *   _bcm_dpp_field_ext_info_entry_clear(unit,unitData->entryExtTc[entry].ext_info_idx);
                     *   soc_sand_occ_bm_occup_status_set(unitData->unit, extTcamBmp, unitData->entryExtTc[entry].ext_info_idx, FALSE);
                     */
                    _bcm_dpp_field_ext_info_entry_clear(unit,ext_info_idx);
                    soc_sand_occ_bm_occup_status_set(unitData->unit, extTcamBmp, ext_info_idx, FALSE);
                }
            }
            /* decrement uninstalled TCAM entries counter */
            if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  allocate sw-state resources */
                /*
                 * Was:
                 *   unitData->entryUninstalledTcCount--;
                 */
                _bcm_dpp_field_ent_idx_t    entryUninstalledTcCount ;

                result = FIELD_ACCESS.entryUninstalledTcCount.get(unit, &entryUninstalledTcCount) ;
                if (result == BCM_E_NONE) {
                    entryUninstalledTcCount-- ;
                    result = FIELD_ACCESS.entryUninstalledTcCount.set(unit, entryUninstalledTcCount) ;
                }
            }
        }

        if (BCM_E_NONE == result) {
            /* delink from the group's entry list */
            _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
            /*
             * Was:
             *   if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry) < tcam_entry_limit) {
             *       entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
             *                                                     _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry));
             *       *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry);
             *   } else {
             *       BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, groupD_group, _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry)));
             *   }
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,isExternalTcam, entry_id, entryPrev)) ;
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,isExternalTcam, entry_id, entryNext)) ;
            if (entryPrev < tcam_entry_limit) {
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entryPrev, entryNext)) ;
            } else {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, groupD_group, entryNext));
            }

            /*
             * was:
             *   if (_BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry) < tcam_entry_limit) {
             *       entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
             *                                                     _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, entry));
             *       *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry);
             *   } else {
             *       BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, groupD_group, _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, entry)));
             *   }
             */
            if (entryNext < tcam_entry_limit) {
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entryNext, entryPrev)) ;
            } else {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, groupD_group, entryPrev));
            }

            /*
             * Was:
             *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, entry);
             *   _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(entryType);   .. sets 'next' variable with the next free entry ..
             *   *entryIdx = next;
             */
            _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(entryType); /* sets 'next' variable with the next free entry */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,isExternalTcam, entry_id, next)) ;

            /*
             * Was:
             *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, entry);
             *   *entryIdx = tcam_entry_limit;
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,isExternalTcam, entry_id, tcam_entry_limit)) ;

            /*
             * Was:
             *   entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, entry);
             *   *entryFlags = 0;
             *   _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(entryType);    .. sets 'next' variable with the next free entry ..
             *   if (next < tcam_entry_limit) {
             *       entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), next);
             *       *entryIdx = entry;
             *   }
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit,isExternalTcam, entry_id, 0)) ;
            _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(entryType); /* sets 'next' variable with the next free entry */
            if (next < tcam_entry_limit) {
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), next, entry_id)) ;
            }


            _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE_OPERATION(entryType, = entry_id;)
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, groupD_group, &groupD_stage));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, groupD_group, &groupD_grpMode));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.get(unit, groupD_group, &groupD_entryCount));
            groupD_entryCount--;
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.set(unit, groupD_group, groupD_entryCount));
            FIELD_ACCESS.stageD.entryTcSh1.get(unit, groupD_stage, &entryTcSh1);
            FIELD_ACCESS.stageD.modeBits.entryCount.get(unit, groupD_stage, groupD_grpMode, &entryCount);
            FIELD_ACCESS.stageD.entryElems.get(unit, entryTcSh1, &entryElems);
            FIELD_ACCESS.stageD.entryElems.set(unit, entryTcSh1, (entryElems - entryCount));
            FIELD_ACCESS.stageD.entryCount.get(unit, entryTcSh1, &entryCount);
            FIELD_ACCESS.stageD.entryCount.set(unit, entryTcSh1, --entryCount);

            _BCM_DPP_FIELD_TCAM_ENTRY_TC_COUNT_OPERATION(entryType, --;)
        } /* if (BCM_E_NONE == result) */
    } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not in use\n"),
                   unit,
                   entry_id));
        result = BCM_E_NOT_FOUND;
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d(%08X),%d) return %d (%s)\n"),
               unit,
               PTR_TO_INT(unitData),
               entry_id,
               result,
               _SHR_ERRMSG(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_add_to_group(bcm_dpp_field_info_OLD_t *unitData,
                                       _bcm_dpp_field_grp_idx_t group,
                                       _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_ent_idx_t currEntry;
    _bcm_dpp_field_entry_type_t entryType;   
    int max_entry_priorities;
    int insertEntry = FALSE;
    int result = BCM_E_NONE;
    _bcm_dpp_field_ent_idx_t entryIntTcCount;
    _bcm_dpp_field_ent_idx_t entryExtTcCount;
    _bcm_dpp_field_ent_idx_t entryIntTcFree;
    _bcm_dpp_field_ent_idx_t entryExtTcFree;
    _bcm_dpp_field_ent_idx_t entryCount;        /* active entries this stage */
    _bcm_dpp_field_ent_idx_t entryElems;        /* active HW entry elements */
    _bcm_dpp_field_stage_idx_t entryTcSh1;
    uint32 hwEntryLimit;
    _bcm_dpp_field_ent_idx_t entryTcLimit;
    _bcm_dpp_field_ent_idx_t tcam_entry_limit;
    uint32 groupD_groupFlags;
    bcm_field_group_mode_t groupD_grpMode; 
    _bcm_dpp_field_stage_idx_t groupD_stage; 
    _bcm_dpp_field_ent_idx_t groupD_entryCount;
    _bcm_dpp_field_ent_idx_t groupD_entryHead;
    _bcm_dpp_field_ent_idx_t groupD_entryTail;
    uint32 entryFlags;
    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */
    _bcm_dpp_field_ent_idx_t entryNext;         /* next entry     */
    _bcm_dpp_field_ent_idx_t entryPrev;         /* previous entry */
    _bcm_dpp_field_ent_idx_t currEntryPrev;     /* previous entry of current entry */
    int isExternalTcam ;
    int entryPriority;
    uint32 hwPriority;


    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    
    unit = unitData->unit;
 
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupD_groupFlags));
    if (0 == (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u not in use\n"),
                          unit,
                          group));
    }
    if (0 == (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u can not have"
                                           " entries created before setting"
                                           " group actions\n"),
                          unit,
                          group));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &groupD_stage));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, group, &groupD_grpMode));
    FIELD_ACCESS.stageD.entryTcSh1.get(unit, groupD_stage, &entryTcSh1);
    FIELD_ACCESS.stageD.entryElems.get(unit, entryTcSh1, &entryElems);
    FIELD_ACCESS.stageD.modeBits.entryCount.get(unit, groupD_stage, groupD_grpMode, &entryCount);
    FIELD_ACCESS.stageD.hwEntryLimit.get(unit, entryTcSh1, &hwEntryLimit);
    if ((entryElems + entryCount) > hwEntryLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("a new entry in unit %d group %u"
                                           " would exceed stage %u capacity\n"),
                          unit,
                          group,
                          groupD_stage));
    }
    FIELD_ACCESS.stageD.modeBits.entryType.get(unit, groupD_stage, groupD_grpMode, &entryType);
    _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
    isExternalTcam = _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType) ;
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, entry, entryFlags)) ;
    /*
     * Was:
     *   if (_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) )
     */
    if (entryFlags
        & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
        /*
         * Was:
         *   if (group != _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry))
         */
         if (group != entryGroup) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not permit"
                                               " entries to migrate from"
                                               " one group to another\n"),
                              unit));
        }
        else if(groupD_grpMode == bcmFieldGroupModeDirect) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d direct-lookup : entry already in use\n"),
                              unit));
        }
        if (_BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(entryType)) {

            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryGroup)) ;
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,isExternalTcam, entry, entryNext)) ;
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,isExternalTcam, entry, entryPrev)) ;

            /* remove this entry from the group */
            /*
             * Was:
             *   if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < tcam_entry_limit)
             */
            if (entryPrev < tcam_entry_limit) {
                /*
                 * Was:
                 *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                 *                                                 _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
                 *   *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
                 */
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,isExternalTcam, entryPrev, entryNext)) ;
            } else {
                /*
                 * Was:
                 *   BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry), 
                 *                                                         _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)));
                 */
               BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, entryGroup, entryNext));
            }
            /*
             * Was:
             *   if (_BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < tcam_entry_limit)
             */
            if (entryNext < tcam_entry_limit) {
                /*
                 * Was:
                 *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                 *                                                 _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
                 *   *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
                 */
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,isExternalTcam, entryNext, entryPrev)) ;
            } else {
                /*
                 * Was:
                 *   BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry), 
                 *                                                    _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)));
                 */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, entryGroup, entryPrev));
            }
            insertEntry = TRUE;
        } /* if (_BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(entryType)) */
    } else { /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */
        /* entry is not in use, so must be coming off the free list */
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,isExternalTcam, entry, entryNext)) ;
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,isExternalTcam, entry, entryPrev)) ;
        /*
         * Was:
         *   if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < tcam_entry_limit)
         */
        if (entryPrev < tcam_entry_limit) {
            /*
             * Was:
             *
             *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
             *                                                 _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
             *   *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,isExternalTcam, entryPrev, entryNext)) ;
        } else {
            /*
             * Was:
             *   _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE_OPERATION(entryType, = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);)
             */
             _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE_OPERATION(entryType, = entryNext;) ;
        }
        /*
         * Was:
         *   if (_BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < tcam_entry_limit)
         */
        if (entryNext < tcam_entry_limit) {
            /*
             * Was:
             *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
             *                                                 _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
             *   *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entryNext, entryPrev)) ;
        }
        _BCM_DPP_FIELD_TCAM_ENTRY_TC_COUNT_OPERATION(entryType, ++;);

        FIELD_ACCESS.stageD.entryElems.set(unit, entryTcSh1, entryElems + entryCount);
        FIELD_ACCESS.stageD.entryCount.get(unit, entryTcSh1, &entryCount);
        FIELD_ACCESS.stageD.entryCount.set(unit, entryTcSh1, ++entryCount);

        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.get(unit, group, &groupD_entryCount));
        groupD_entryCount++;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.set(unit, group, groupD_entryCount));
        
        if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  allocate sw-state resources */
            /*
             * Was:
             *   unitData->entryUninstalledTcCount++ ;
             */
             _bcm_dpp_field_ent_idx_t    entryUninstalledTcCount ;

            result = FIELD_ACCESS.entryUninstalledTcCount.get(unit, &entryUninstalledTcCount) ;
            if (result == BCM_E_NONE) {
                entryUninstalledTcCount++ ;
                result = FIELD_ACCESS.entryUninstalledTcCount.set(unit, entryUninstalledTcCount) ;
            }
        }
        insertEntry = TRUE;
    } /* if (entryData->flags & _BCM_DPP_FIELD_ENTRY_IN_USE) */

    if (insertEntry) {
        if (_BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(entryType)) {
            int entryPriority, currentEntryPriority;
            /* scan for where this entry goes in the new group */
            FIELD_ACCESS.entryTcLimit.get(unit, &entryTcLimit);
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &groupD_entryHead));
            /*
             * Was:
             *   for (currEntry = groupD_entryHead;
             *        (currEntry < entryTcLimit) &&
             *        (0 >= _bcm_dpp_compare_entry_priority(unitData->entryTc[entry].entryCmn.entryPriority,
             *                                              unitData->entryTc[currEntry].entryCmn.entryPriority));
             *        )
             */
             currEntry = groupD_entryHead;
             if (currEntry < entryTcLimit) {
                 BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryPriority.get(unit,entry,&entryPriority)) ;
                 BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryPriority.get(unit,currEntry,&currentEntryPriority)) ;
             }
             for ( ;
                  (currEntry < entryTcLimit) &&
                  (0 >= _bcm_dpp_compare_entry_priority(entryPriority,
                                                        currentEntryPriority));
                  ) {
                /*
                 *  ...as long as there are more entries, and the current entry
                 *  priority is greater than or equal to the new entry priority,
                 *  examine the next entry in the group.
                 */
                /*
                 * Was:
                 *   currEntry = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry);
                 */
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry, currEntry)) ;
                if (currEntry < entryTcLimit) {
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryPriority.get(unit,currEntry,&currentEntryPriority)) ;
                }
            }
        } else {
            /* External TCAM. Entry should be added to the head of the list */
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &groupD_entryHead));
            currEntry = groupD_entryHead;
        }
        /* insert the new entry *before* currEntry */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d: inserting entry %u to group %u before"
                              " entry %u\n"),
                   unit,
                   entry,
                   group,
                   currEntry));
        /*
         * Was:
         *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
         *   *entryIdx = currEntry;
         */
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, currEntry)) ;
        if (currEntry < tcam_entry_limit) {
            /*
             * was:
             *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
             *   *entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry);
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry, currEntryPrev)) ;
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, currEntryPrev)) ;
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryPrev)) ;
            /*
             * Was:
             *   if (_BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) < tcam_entry_limit)
             */
            if (entryPrev < tcam_entry_limit) {
                /*
                 * Was:
                 *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                 *                                             _BCM_DPP_FIELD_TCAM_ENTRY_PREV(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
                 *   *entryIdx = entry;
                 */
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryPrev)) ;
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entryPrev, entry)) ;
            }
            /*
             * Was:
             *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry);
             *   *entryIdx = entry;
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), currEntry, entry)) ;
        } else {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.get(unit, group, &groupD_entryTail));
            /*
             * Was:
             *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
             *   *entryIdx = groupD_entryTail;
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, groupD_entryTail)) ;
            if (groupD_entryTail < tcam_entry_limit) {
                /*
                 * Was:
                 *   entryIdx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), groupD_entryTail);
                 *   *entryIdx = entry;
                 */
                BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), groupD_entryTail, entry)) ;
            }
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, group, entry));

        }
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &groupD_entryHead));
        if (currEntry == groupD_entryHead) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, group, entry));
        }
        /*
         * Was:
         *   entryGroup = _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
         *   *entryGroup = group;
         */
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, group)) ;
    }

    /* entry is in use, has been changed since hw commit, and is new/moved */
    /*
     * Was:
     *
     *   entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
     *   *entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_USE |
     *                  _BCM_DPP_FIELD_ENTRY_CHANGED |
     *                  _BCM_DPP_FIELD_ENTRY_NEW;
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryFlags)) ;
    entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_USE |
                    _BCM_DPP_FIELD_ENTRY_CHANGED |
                    _BCM_DPP_FIELD_ENTRY_NEW;
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryFlags)) ;
    /* group contains changed and added/moved entry/entries */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupD_groupFlags));
    groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT |
                             _BCM_DPP_FIELD_GROUP_ADD_ENT;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.set(unit, group, groupD_groupFlags));


    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.maxEntryPriorities.get(unit, group, &max_entry_priorities));
    _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryPriority) ;
    /*
     * Was:
     *   hwPriority = _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
     */
    if (_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType) && (max_entry_priorities > 0)) {
        /*
         * Was:
         *   *hwPriority = max_entry_priorities - _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
         */
        hwPriority = max_entry_priorities - entryPriority ;
        _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, hwPriority) ;
    }
    else {
        /*
         * Was:
         *   *hwPriority = BCM_FIELD_ENTRY_BCM_PRIO_TO_DPP_PRIO(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
         *                            _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry));
         */
        hwPriority = BCM_FIELD_ENTRY_BCM_PRIO_TO_DPP_PRIO(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),entryPriority);
        _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_SET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, hwPriority) ;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /*_bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);*/
#endif /* def BCM_WARM_BOOT_SUPPORT */

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_action_get(bcm_dpp_field_info_OLD_t *unitData,
                                     bcm_field_entry_t entry,
                                     bcm_field_action_t type,
                                     uint32 *param0,
                                     uint32 *param1,
                                     uint32 *param2,
                                     uint32 *param3)
{
    unsigned int index;
    SOC_PPC_FP_ENTRY_INFO entInfo;
    SOC_PPC_FP_ACTION_TYPE ppdType = SOC_PPC_FP_ACTION_TYPE_INVALID;

    uint8 found;
    uint8 isExternalTcam = FALSE;
    uint8 is_kaps_advance_mode = FALSE;
    uint32 soc_sand_rv;
    int result = BCM_E_NONE;
    uint32 groupD_groupFlags;
    uint32 entryFlags;
    uint32 ext_index;
    uint32 actCount = 0;
    _bcm_dpp_field_ent_idx_t entry_idx;
    _bcm_dpp_field_grp_idx_t entryGroup, groupLimit;
    _bcm_dpp_field_stage_idx_t stage;
    SOC_PPC_FP_ACTION_TYPE *actChain = NULL;
    soc_dpp_config_jer_pp_t *jer_pp_config = NULL;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;    
    
    unit = unitData->unit;
    if ((0 > type) || (bcmFieldActionCount <= type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d not valid\n"), type));
    }

    if (_BCM_DPP_FIELD_ENTRY_IS_LARGE_DIRECT_LOOKUP(entry)) {
        BCM_FIELD_LARGE_DIRECT_ENTRY_ID_UNPACK(entry, entryGroup, entry_idx);
        is_kaps_advance_mode = TRUE;
    } else {
        isExternalTcam = _BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry);
        entry_idx = _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry);
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry_idx, entryGroup)) ;
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupLimit.get(unit, &groupLimit));
    if (groupLimit <= entryGroup) {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
    }

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, entryGroup, &groupD_groupFlags));
    if (groupD_groupFlags & _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY) {
        result = _bcm_dpp_field_tcam_entry_action_get_by_key(unitData,isExternalTcam, entry_idx, type, param0, param1);
    } else {

        if (is_kaps_advance_mode == TRUE) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryGroup, &stage));
            result = _bcm_dpp_ppd_act_from_bcm_act(unitData,
                                                   stage,
                                                   type,
                                                   &actChain);
            BCMDNX_IF_ERR_EXIT(result);

            for (index = 0; index < unitData->devInfo->actChain; index++ ) {
                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(actChain[index])) {
                   ppdType = actChain[index];
                   break;
                }
            }
            entInfo.large_direct_lookup_key = entry_idx;
            entInfo.large_direct_lookup_mask = 0;
            soc_sand_rv = _bcm_ppd_fp_entry_get(unitData,
                                    entry_idx,
                                    entryGroup,
                                    entry_idx,
                                    &found,
                                    &entInfo);
            result = handle_sand_result(soc_sand_rv);
            if (BCM_E_NONE == result) {
                if (found) {
                    while ((index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) &&
                           (entInfo.actions[index].type != ppdType)) {
                        index++;
                    }
                    if (index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
                        *param0 = entInfo.actions[index].val;
                    } else {
                        result = BCM_E_NOT_FOUND;
                    }
                }
            }

            BCM_EXIT;
        }

        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, entry_idx, entryFlags)) ;
        if (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
            if ((isExternalTcam) && (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
                /* get action from SOC layer. It's not kept in SW state */
                switch (type) {
                    case bcmFieldActionExternalValue0Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0;
                        break;
                    case bcmFieldActionExternalValue1Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_1;
                        break;
                    case bcmFieldActionExternalValue2Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_2;
                        break;
                    case bcmFieldActionExternalValue3Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_3;
                        break;
                    case bcmFieldActionExternalValue4Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_4;
                        break;
                    case bcmFieldActionExternalValue5Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_5;
                        break;
                    case bcmFieldActionExternalValue6Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_6;
                        break;
                    case bcmFieldActionExternalValue7Set:
                        ppdType = SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7;
                        break;
                    default:
                        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action type %d (%s) illegal for external TCAM\n"),
                                          unit,
                                          entry,
                                          type,
                                          _bcm_dpp_field_action_name[type]));

                }

                ext_index = ppdType - SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0;

                SOC_PPC_FP_ENTRY_INFO_clear(&entInfo);
                {
                    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */
                    uint32 hwHandle;                            /* group handle in hardware */

                    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry_idx, entryGroup)) ;
                    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit,isExternalTcam, entry_idx, hwHandle)) ;

                    soc_sand_rv = _bcm_ppd_fp_entry_get(unitData,
                                                        entry_idx,
                                                        entryGroup,
                                                        hwHandle,
                                                        &found,
                                                        &entInfo);

                }
                result = handle_sand_result(soc_sand_rv);
                if (BCM_E_NONE == result) {
                    if (found) {
                        index = 0;
                        while ((index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) &&
                               (entInfo.actions[index].type != ppdType)) {
                            index++;
                        }
                        if (index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
                            *param0 = entInfo.actions[index].val;
                            *param1 = 0;

                            if (param2 != NULL) {
                                *param2 = 0;
                            }

                            if (param3 != NULL) {
                                *param3 = 0;
                            }

                            if (SOC_IS_JERICHO(unit)) {
                                if ((ppdType >= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) &&
                                    (ppdType <= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7)) {

                                    jer_pp_config = &(SOC_DPP_JER_CONFIG(unit)->pp);
                                    actCount = SOC_PPC_BIT_TO_U32(jer_pp_config->res_sizes[ext_index]);

                                    if (actCount >= 2) {
                                        *param1 = ((index + 1) < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) ? entInfo.actions[index + 1].val : 0;
                                    }

                                    if ((actCount >= 3) && (param2 != NULL)) {
                                        *param2 = ((index + 2) < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) ?  entInfo.actions[index + 2].val : 0;
                                    }

                                    if ((actCount >= 4) && (param3 != NULL)) {
                                        *param3 = ((index + 3) < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) ? entInfo.actions[index + 3].val : 0;
                                    }
                                }
                            } else if (ppdType == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) {
                                *param1 = ((index + 1) < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) ? entInfo.actions[index + 1].val : 0;
                            }
                        }
                        else {
                            result = BCM_E_NOT_FOUND;
                        }
                    }
                    else {
                        result = BCM_E_NOT_FOUND;
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unit %d entry %d not valid\n"), unit, entry));
                    }
                }
            }
            else {
                _bcm_dpp_field_tc_b_act_t tcActB_at_index ;

                index = 0;
                /*
                 * Was:
                 *   while ((index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) &&
                 *          (_BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmType != type)) {
                 *       index++;
                 *   }
                 */
                while (index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
                    _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, entry_idx, index, tcActB_at_index) ;
                    if (tcActB_at_index.bcmType == type) {
                        break ;
                    }
                    index++;
                }
                /*
                 *  index = offset of the requested type if it existed, or
                 *  _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX
                 */
                if (index < _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX) {
                    /* found requested type, get its information */

                    /*
                     * Was:
                     *   *param0 = _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam0;
                     *   *param1 = _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, entry)[index].bcmParam1;
                     */
                    _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, entry_idx, index, tcActB_at_index) ;
                    *param0 = tcActB_at_index.bcmParam0;
                    *param1 = tcActB_at_index.bcmParam1;
                    if (param2 != NULL) {
                        *param2 = tcActB_at_index.bcmParam2;
                    }
                    if (param3 != NULL) {
                        *param3 = tcActB_at_index.bcmParam3;
                    }
                } else {
                    result = BCM_E_NOT_FOUND;
                }
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use\n"),
                              unit,
                              entry_idx));
        }
    }

    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_tcam_entry_copy_id(bcm_dpp_field_info_OLD_t *unitData,
                                  uint8 isExternalTcam,
                                  _bcm_dpp_field_ent_idx_t dest,
                                  _bcm_dpp_field_ent_idx_t source)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    _bcm_dpp_field_entry_t     entryTc;          /* element on internal entry table */
    _bcm_dpp_field_entry_ext_t entryExtTc;       /* element on external entry table */
    int entryPriority;
    uint32 entryFlags;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
 
    /* preserve previous entry list position for now */
    /*
     * Was:
     *   next = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, dest);
     *   prev = _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, dest);
     */
    _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,isExternalTcam, dest, next) ;
    _BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,isExternalTcam, dest, prev) ;
    /* copy the old entry to the new entry */
    /*
     * Was:
     *   sal_memcpy(_BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, dest),
     *          _BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, source),
     *           isExternalTcam ? sizeof(_bcm_dpp_field_entry_ext_t) : sizeof(_bcm_dpp_field_entry_t));
     */
    if (isExternalTcam) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.get(unit,source,&entryExtTc)) ;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.set(unit,dest,&entryExtTc)) ;
    } else {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.get(unit,source,&entryTc)) ;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.set(unit,dest,&entryTc)) ;
    }
    /* keep new entry in previous entry list for now */
    /*
     * Was:
     *   entry_Idx = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, dest);
     *   *entry_Idx = next;
     *   entry_Idx = _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, dest);
     *   *entry_Idx = prev;
     *   priority = _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_PTR(isExternalTcam, dest);
     *   *priority = BCM_FIELD_ENTRY_PRIO_DEFAULT;
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,isExternalTcam, dest, next)) ;
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,isExternalTcam, dest, prev)) ;
    entryPriority = BCM_FIELD_ENTRY_PRIO_DEFAULT ;
    _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_SET(unit,isExternalTcam, dest, entryPriority) ;
    /* the copy is not yet in hardware, nor is it yet in use */
    /*
     * Was:
     *   entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, dest);
     *   *entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_IN_USE |
     *                                _BCM_DPP_FIELD_ENTRY_IN_HW |
     *                                _BCM_DPP_FIELD_ENTRY_WANT_HW));
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, dest, entryFlags)) ;
    entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_IN_USE |
                                    _BCM_DPP_FIELD_ENTRY_IN_HW |
                                    _BCM_DPP_FIELD_ENTRY_WANT_HW));
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit,isExternalTcam, dest, entryFlags)) ;
exit:
    BCMDNX_FUNC_RETURN_VOID;
}




int
    _bcm_dpp_field_tcam_entry_action_get_by_key(bcm_dpp_field_info_OLD_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry,
                                                 bcm_field_action_t type,uint32 *param0,uint32 *param1)
{
    int result;
    SOC_PPC_FP_ENTRY_INFO entryInfo;
    uint8 found;
    uint32 soc_sand_rv;
    _bcm_dpp_field_stage_idx_t groupD_stage;
    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */
    uint32 hwHandle;                            /* group handle in hardware */

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
   
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
    /*
     * Was:
     *   BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry), &groupD_stage));
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryGroup, &groupD_stage));
    result = _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(unitData, &entryInfo, entry, isExternalTcam, _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxQuals, groupD_stage);
    if (BCM_E_NONE != result) {        
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "error in _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state\n")));
    }
    BCMDNX_IF_ERR_EXIT(result);

    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit,isExternalTcam, entry, hwHandle)) ;

    soc_sand_rv = _bcm_ppd_fp_entry_get(unitData,
                                        entry,
                                        entryGroup,
                                        hwHandle,
                                        &found,
                                        &entryInfo);

    result = handle_sand_result(soc_sand_rv);
    if (BCM_E_NONE == result) {
        if (found) {
            (*param0) = entryInfo.actions[0].val; 
            /*index = 0;
            while ((index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) &&
                   (entryInfo.actions[index].type != ppdType)) {
                index++;
            }
            if (index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
                *param0 = entryInfo.actions[index].val;
                *param1 = 0;
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                                   " action %d (%s)"),unit,entry,type,_bcm_dpp_field_action_name[type]));
            }*/
        }
        else {
            result = BCM_E_NOT_FOUND;
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d not valid\n"), unit, entry));
        }
    }

    BCMDNX_IF_ERR_EXIT(result);    

exit:
    BCMDNX_FUNC_RETURN;
}


int
    _bcm_dpp_field_tcam_entry_remove_by_key(bcm_dpp_field_info_OLD_t *unitData, uint8 isExternalTcam, _bcm_dpp_field_ent_idx_t entry)
{
    int result;
    SOC_PPC_FP_ENTRY_INFO entryInfo;
    uint32 ppdr;
    _bcm_dpp_field_stage_idx_t groupD_stage; 
    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
 
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
    /*
     * was:
     *   BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry), &groupD_stage));
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryGroup, &groupD_stage));
    result = _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(unitData, &entryInfo, entry, isExternalTcam, _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxQuals, groupD_stage);
    if (BCM_E_NONE != result) {        
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "error in _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state\n")));
    }

    BCMDNX_IF_ERR_EXIT(result);

    ppdr = soc_ppd_fp_entry_remove_by_key(unitData->unitHandle, entryGroup, &entryInfo);

    result = handle_sand_result(ppdr);
    if (BCM_E_NONE != result) {        
        _bcm_dpp_field_grp_idx_t entryGroup;     /* this entry's group */
        uint32 hwHandle;                         /* entry handle in hardware */

        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, entry, entryGroup)) ;
        _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit,isExternalTcam, entry, hwHandle) ;
        LOG_ERROR(BSL_LS_BCM_FP,(BSL_META_U(unit, "unable to remove unit %d group %u entry %u"
                              " (hw entry %u) from hardware: %d (%s)\n"),
                   unit,
                   /*
                    * Was:
                    *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                    */
                   entryGroup,
                   _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                   /*
                    * Was:
                    *   _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, entry),
                    */
                   hwHandle,
                   result,_SHR_ERRMSG(result)));
    }

    BCMDNX_IF_ERR_EXIT(result);    

exit:
    BCMDNX_FUNC_RETURN;
}


int
    _bcm_dpp_field_tcam_entry_build_qualifiers_from_sw_state(bcm_dpp_field_info_OLD_t *unitData, SOC_PPC_FP_ENTRY_INFO *entryInfo, _bcm_dpp_field_ent_idx_t entry, uint8 isExternalTcam, unsigned int entryMaxQuals, _bcm_dpp_field_stage_idx_t stage)
{
    int index, offset;
   _bcm_dpp_field_qual_t qual ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    unit = unitData->unit;

     for (index = 0, offset = 0; index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; index++) {
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, entry, index, qual) ;
        /*
         * Was:
         *   if (_BCM_DPP_FIELD_PPD_QUAL_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType))
         */
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(qual.hwType)) {
            LOG_DEBUG(BSL_LS_BCM_FP,(BSL_META_U(unit,"unit %d entry %u qual %d(%s)->%d(%s)"" %08X%08X/%08X%08X\n"),
                       unit,
                       _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                       /*
                        * Was:
                        *  _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType,
                        *  (bcmFieldQualifyCount>_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType)?_bcm_dpp_field_qual_name[_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualType]:"-",
                        *  _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType,
                        *  SOC_PPC_FP_QUAL_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType),
                        *  COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData),
                        *  COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData),
                        *  COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask),
                        *  COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask)
                        */
                        qual.qualType,
                        (BCM_FIELD_ENTRY_INVALID != qual.qualType)?_bcm_dpp_field_qual_name[qual.qualType]:"-",
                        qual.hwType,
                        SOC_PPC_FP_QUAL_TYPE_to_string(qual.hwType),
                        COMPILER_64_HI(qual.qualData),
                        COMPILER_64_LO(qual.qualData),
                        COMPILER_64_HI(qual.qualMask),
                        COMPILER_64_LO(qual.qualMask)
            ));
            /*
             * Was:
             *   entryInfo->qual_vals[offset].type = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].hwType;
             */
            entryInfo->qual_vals[offset].type = qual.hwType;

            /* SOC_SAND_U64 is a coerced 32b grain little-endian 64b number */

            /*
             * Was:
             *   entryInfo->qual_vals[offset].val.arr[0] = COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData);
             *   entryInfo->qual_vals[offset].val.arr[1] = COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualData);
             *   entryInfo->qual_vals[offset].is_valid.arr[0] = COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask);
             *   entryInfo->qual_vals[offset].is_valid.arr[1] = COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, entry)[index].qualMask);
             */
            entryInfo->qual_vals[offset].val.arr[0] = COMPILER_64_LO(qual.qualData);
            entryInfo->qual_vals[offset].val.arr[1] = COMPILER_64_HI(qual.qualData);
            entryInfo->qual_vals[offset].is_valid.arr[0] = COMPILER_64_LO(qual.qualMask);
            entryInfo->qual_vals[offset].is_valid.arr[1] = COMPILER_64_HI(qual.qualMask);

            offset++;
        } /* if (entryData->qual[index].hwType < bcmFieldQualifyCount) */
    } /* for (all qualifiers) */
    if (offset > entryMaxQuals) {
        _bcm_dpp_field_grp_idx_t localEntryGroup ;

        BCMDNX_IF_ERR_EXIT(
            _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(
                unit,isExternalTcam, entry, localEntryGroup)) ;
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,(_BSL_BCM_MSG_NO_UNIT("unit %d stage %u group %u entry %u"
                                           " has more qualifier slots in use"
                                           " %d than the stage supports %d\n"),
                          unit,
                          stage,
                          /*
                           * Was:
                           *   _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, entry),
                           */
                          localEntryGroup,
                          _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, entry, isExternalTcam),
                          offset,
                          entryMaxQuals));
    }

    while (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
        entryInfo->qual_vals[offset].type = BCM_FIELD_ENTRY_INVALID;
        entryInfo->qual_vals[offset].is_valid.arr[0] = 0;
        entryInfo->qual_vals[offset].is_valid.arr[1] = 0;
        entryInfo->qual_vals[offset].val.arr[0] = 0;
        entryInfo->qual_vals[offset].val.arr[1] = 0;
        offset++;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
STATIC uint32 _bcm_dpp_field_hw_action_para_get(uint32 unit, uint32 stage, SOC_PPC_FP_ACTION_TYPE type, uint32 val)
{
    uint32 check_value = val;

    switch (type) {
        case SOC_PPC_FP_ACTION_TYPE_DP:
            if (SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF == stage) {
                check_value = val | 0xC;
            } else if (SOC_IS_JERICHO(unit) && (SOC_PPC_FP_DATABASE_STAGE_EGRESS == stage)) {
                check_value = val | 0x4;
            }

            break;
        case SOC_PPC_FP_ACTION_TYPE_TC:
            if (SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF == stage) {
                check_value = ((1 << 3) | (val & 0x7));
            } else if (SOC_IS_JERICHO(unit) && (SOC_PPC_FP_DATABASE_STAGE_EGRESS == stage)) {
                check_value = (((val & 0x7) << 1) | 1);
            }

            break;
        case SOC_PPC_FP_ACTION_TYPE_PPH_RESERVE_VALUE:
            if (SOC_IS_JERICHO_PLUS(unit) && (SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF == stage)) {
                check_value = (((val & 0x3) << 1) | 1);
            }

            break;
        default:
            break;
    }

    return check_value;
}

int
_bcm_dpp_field_tcam_hardware_entry_check(bcm_dpp_field_info_OLD_t *unitData,
                                         bcm_field_entry_t entry,
                                         SOC_PPC_FP_ENTRY_INFO *entInfoTc)
{
    uint32 soc_sandResult;
    uint8 okay;
    unsigned int index;
    unsigned int offset;
    _bcm_dpp_field_ent_idx_t ent;
    int result;
    _bcm_dpp_field_grp_idx_t groupD_group;
    _bcm_dpp_field_stage_idx_t groupD_stage;
    _bcm_dpp_field_qual_t qual ;
    SOC_PPC_FP_ACTION_TYPE hwType;              /* hardware action type */
    uint32 hwParam;                             /* hardware action parameter */
    uint32 hwHandle;                            /* group handle in hardware */
    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */
    uint32 hwPriority; /* entry priority in hardware */
    uint32 egrTrapProfile, hwFlags;
    bcm_field_group_mode_t grpMode;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (!(_BCM_DPP_FIELD_ENT_IS_TCAM(unit, entry))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not a"
                                           " TCAM entry\n"),
                          unit,
                          entry));
    }
    ent = _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry);

    /*
     * Was:
     *   groupD_group = _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent);
     */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, groupD_group)) ;

    SOC_PPC_FP_ENTRY_INFO_clear(entInfoTc);

    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, entryGroup)) ;

    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, hwHandle)) ;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, entryGroup, &grpMode));

    if (_BCM_DPP_FIELD_ENT_IS_INTTCAM(unit, entry)) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.large_direct_lookup_key.get(unit, ent, &entInfoTc->large_direct_lookup_key));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.large_direct_lookup_mask.get(unit, ent, &entInfoTc->large_direct_lookup_mask));
    }

    soc_sandResult = _bcm_ppd_fp_entry_get(unitData,
                                           entry,
                                           entryGroup,
                                           hwHandle,
                                           &okay,
                                           entInfoTc);

    result = handle_sand_result(soc_sandResult);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch"
                                          " entry %u hardware"
                                          " information (%u):"
                                          " %d (%s)\n"),
                         unit,
                         entry,
                         /*
                          * was:
                          *   _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent),
                          */
                         hwHandle,
                         result,
                         _SHR_ERRMSG(result)));
    if (!okay) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not okay"
                                           " trying to fetch entry"
                                           " from hardware\n"),
                          unit,
                          entry));
    }
#if _BCM_DPP_FIELD_GET_VERIFY_PPD
    _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, hwPriority) ;
    /*
     * Was:
     *   if (_BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent) != entInfoTc->priority) 
     */
    /* There is no sw info for ExtTCAM entry as KBP capacity is big, check skip */
    if (!_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry) && (grpMode != bcmFieldGroupModeDirect)) {
        if (hwPriority != entInfoTc->priority) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "unit %d entry %d hardware priority %d does"
                                 " not match PPD layer priority %d\n"),
                      unit,
                      entry,
                      /*
                       * was:
                       *   _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent),
                       */
                      hwPriority,
                      entInfoTc->priority));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
            BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                             (_BSL_BCM_MSG_NO_UNIT(("unit %d entry %d hardware priority"
                                                " %d does not match PPD layer"
                                                " priority %d\n"),
                                               unit,
                                               entry,
                                               /*
                                                * was:
                                                *   _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent),
                                                */
                                               hwPriority,
                                               entInfoTc->priority)));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
        }
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, groupD_group, &groupD_stage));
    for (index = 0;
         index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxQuals;
         index++) {
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, index, qual) ;
        /*
         * was
         *   if (_BCM_DPP_FIELD_PPD_QUAL_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType))
         */
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(qual.hwType)) {
            /* qualifier is in use */
            for (offset = 0;
                 (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) &&
                 (entInfoTc->qual_vals[offset].type !=
                 /*
                  * Was:
                  *   _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType
                  */
                  qual.hwType
                 );
                 offset++) {
                /* iterate until found matching qualifier */
            }
            if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                /* found matching qualifier; compare it */
                if (
                    /*
                     * Was:
                     *   (COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData) !=
                     *    entInfoTc->qual_vals[offset].val.arr[1]) ||
                     *   (COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualData) !=
                     *    entInfoTc->qual_vals[offset].val.arr[0]) ||
                     *   (COMPILER_64_HI(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask) !=
                     *    entInfoTc->qual_vals[offset].is_valid.arr[1]) ||
                     *   (COMPILER_64_LO(_BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].qualMask) !=
                     *    entInfoTc->qual_vals[offset].is_valid.arr[0])
                     */
                     (COMPILER_64_HI(qual.qualData) !=
                      entInfoTc->qual_vals[offset].val.arr[1]) ||
                     (COMPILER_64_LO(qual.qualData) !=
                      entInfoTc->qual_vals[offset].val.arr[0]) ||
                     (COMPILER_64_HI(qual.qualMask) !=
                      entInfoTc->qual_vals[offset].is_valid.arr[1]) ||
                     (COMPILER_64_LO(qual.qualMask) !=
                      entInfoTc->qual_vals[offset].is_valid.arr[0]))
                    {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d qualifier %d %s"
                                         " (%d) %08X%08X/%08X%08X -> %s (%d)"
                                         " does not match PPD qualifier %d %s"
                                         " (%d) %08X%08X/%08X%08X\n"),
                              unit,
                              entry,
                              index,
                              (qual.qualType != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[qual.qualType]:"-",
                              qual.qualType,
                              COMPILER_64_HI(qual.qualData),
                              COMPILER_64_LO(qual.qualData),
                              COMPILER_64_HI(qual.qualMask),
                              COMPILER_64_LO(qual.qualMask),
                              SOC_PPC_FP_QUAL_TYPE_to_string(qual.hwType),
                              qual.hwType,
                              offset,
                              SOC_PPC_FP_QUAL_TYPE_to_string(entInfoTc->qual_vals[offset].type),
                              entInfoTc->qual_vals[offset].type,
                              entInfoTc->qual_vals[offset].val.arr[1],
                              entInfoTc->qual_vals[offset].val.arr[0],
                              entInfoTc->qual_vals[offset].is_valid.arr[1],
                              entInfoTc->qual_vals[offset].is_valid.arr[0]));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                       " qualifier %d %s (%d)"
                                                       " %08X%08X/%08X%08X ->"
                                                       " %s (%d) does not match"
                                                       " PPD qualifier %d %s"
                                                       " (%d):"
                                                       " %08X%08X/%08X%08X\n"),
                                      unit,
                                      entry,
                                      index,
                                      (qual.qualType != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[qual.qualType]:"-",
                                      qual.qualType,
                                      COMPILER_64_HI(qual.qualData),
                                      COMPILER_64_LO(qual.qualData),
                                      COMPILER_64_HI(qual.qualMask),
                                      COMPILER_64_LO(qual.qualMask),
                                      SOC_PPC_FP_QUAL_TYPE_to_string(qual.hwType),
                                      qual.hwType,
                                      offset,
                                      SOC_PPC_FP_QUAL_TYPE_to_string(entInfoTc->qual_vals[offset].type),
                                      entInfoTc->qual_vals[offset].type,
                                      entInfoTc->qual_vals[offset].val.arr[1],
                                      entInfoTc->qual_vals[offset].val.arr[0],
                                      entInfoTc->qual_vals[offset].is_valid.arr[1],
                                      entInfoTc->qual_vals[offset].is_valid.arr[0]));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                } /* if (qualifier data or mask differs) */
            } else { /* if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) */
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %d qualifier %d %s (%d)"
                                     " %08X%08X/%08X%08X -> %s (%d) does"
                                     " not have matching PPD qualifier\n"),
                          unit,
                          entry,
                          index,
                          (qual.qualType != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[qual.qualType]:"-",
                          qual.qualType,
                          COMPILER_64_HI(qual.qualData),
                          COMPILER_64_LO(qual.qualData),
                          COMPILER_64_HI(qual.qualMask),
                          COMPILER_64_LO(qual.qualMask),
                          SOC_PPC_FP_QUAL_TYPE_to_string(qual.hwType),
                          qual.hwType));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d qualifier"
                                                   " %d %s (%d) %08X%08X"
                                                   "/%08X%08X -> %s (%d) does"
                                                   " not have matching PPD"
                                                   " qualifier\n"),
                                  unit,
                                  entry,
                                  index,
                                  (qual.qualType != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[qual.qualType]:"-",
                                  qual.qualType,
                                  COMPILER_64_HI(qual.qualData),
                                  COMPILER_64_LO(qual.qualData),
                                  COMPILER_64_HI(qual.qualMask),
                                  COMPILER_64_LO(qual.qualMask),
                                  SOC_PPC_FP_QUAL_TYPE_to_string(qual.hwType),
                                  qual.hwType));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
            } /* if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) */
        } /* if (qualifier is in use) */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, groupD_group, &groupD_stage));
    } /* for (all possible qualifier slots on this entry) */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, groupD_group, &groupD_stage));
    for (index = 0;
         index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxActs;
         index++) {
        /*
         * Was:
         *   if (_BCM_DPP_FIELD_PPD_ACTION_VALID(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType)) 
         */
        _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, index, hwType) ;
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(hwType)) {
            /* action is in use */
            /* 
             * COVERITY 
             */
            /* use of the macro SOC_DPP_DEFS_MAX can lead to unneeded comparations */
            /* coverity[same_on_both_sides] */
            for (offset = 0;
                 (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) &&
                 (entInfoTc->actions[offset].type !=
                 /*
                  * Was:
                  *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType
                  */
                  hwType);
                 offset++) {
                /* iterate until found matching action */
            }
            if (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
                _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWPARAM_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, index, hwParam) ;
                /*
                 * Was:
                 *   if (_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam != )
                 */

                _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWFLAGS_GET(unit, _BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, index, hwFlags);
                if ((entInfoTc->actions[offset].type == SOC_PPC_FP_ACTION_TYPE_EGR_TRAP) && (hwParam == SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID) && (hwFlags & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST) && !_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry)) {
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryTc.egrTrapProfile.get(unit, ent, &egrTrapProfile));
                    hwParam = (egrTrapProfile << 1) | 1;
                }

                if (hwParam !=
                    _bcm_dpp_field_hw_action_para_get(unit, _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).hwStageId, entInfoTc->actions[offset].type, entInfoTc->actions[offset].val)) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d PPD action %d"
                                         " %s(%d) %08X does not match"
                                         " hardware action %d %s(%d) %08X\n"),
                              unit,
                              entry,
                              index,
                              /*
                               * Was:
                               *   SOC_PPC_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                               *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                               *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam,
                               */
                              SOC_PPC_FP_ACTION_TYPE_to_string(hwType),
                              hwType,
                              hwParam,
                              offset,
                              SOC_PPC_FP_ACTION_TYPE_to_string(entInfoTc->actions[offset].type),
                              entInfoTc->actions[offset].type,
                              entInfoTc->actions[offset].val));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d PPD"
                                                       " action %d %s(%d) %08X"
                                                       " does not match"
                                                       " hardware action %d"
                                                       " %s(%d) %08X\n"),
                                      unit,
                                      entry,
                                      index,
                                      SOC_PPC_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM((unit, entry), ent)[index].hwType),
                                      _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                                      _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam,
                                      offset,
                                      SOC_PPC_FP_ACTION_TYPE_to_string(entInfoTc->actions[offset].type),
                                      entInfoTc->actions[offset].type,
                                      entInfoTc->actions[offset].val));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                } /* if (action mismatches) */
            } else { /* if (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) */
                {
                    SOC_PPC_FP_ACTION_TYPE hwType ;             /* hardware action type */
                    uint32 hwParam ;                            /* hardware action parameter */

                    _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, index, hwType) ;
                    _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWPARAM_GET(unit,_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent, index, hwParam) ;
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %d PPD action %d %s(%d)"
                                     " %08X has no matching PPD action\n"),
                          unit,
                          entry,
                          index,
                          /*
                           * Was:
                           *   SOC_PPC_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType),
                           *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                           *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam)
                           */
                          SOC_PPC_FP_ACTION_TYPE_to_string(hwType),
                          hwType,
                          hwParam));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d PPD"
                                                   " action %d %s(%d) %08X"
                                                   " has no matching PPD"
                                                   " action\n"),
                                  unit,
                                  entry,
                                  index,
                          /*
                           * Was:
                           *    SOC_PPC_FP_ACTION_TYPE_to_string(_BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM((unit, entry), ent)[index].hwType),
                           *    _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwType,
                           *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry), ent)[index].hwParam)
                           */
                          SOC_PPC_FP_ACTION_TYPE_to_string(hwType),
                          hwType,
                          hwParam);
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                }
            } /* if (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) */
        } /* if (action is in use) */
    } /* for (all possible action slots on this entry) */
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD */

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */

int
_bcm_dpp_field_tcam_entry_hit_get(
    bcm_dpp_field_info_OLD_t *unitData,
    uint32 flags,
    uint32 entry_handle,
    uint8 *entry_hit_core_bmp_p)
{
    int unit = 0;
    int res = BCM_E_NONE;
    SOC_PPC_FP_ENTRY_INFO entryInfo;
    uint8 found = 0;
    uint32 soc_sand_rv = 0;
    _bcm_dpp_field_grp_idx_t entryGroup = SOC_PPC_FP_NOF_DBS;
    uint32 hwHandle = 0;
    ARAD_TCAM_LOCATION entry_abs_location;

    uint32 mem_addr = 0;
    uint8 at = 0;
    uint32 cmd_addr_reg_val;
    soc_reg_above_64_val_t reg_val;
    int blk_index_min = 0;
    int index_min = 0;
    int array_index_min = 0;
    soc_mem_t mem = INVALIDm;
    uint32 bank_id, bank_sram_id, bank_line_id, sram_line_entry_id;
    uint32 max_nof_big_bank_lines, max_nof_small_bank_lines;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_srams_per_tcam_bank;

    BCMDNX_INIT_FUNC_DEFS;
    unit = unitData->unit;

    SOC_PPC_FP_ENTRY_INFO_clear(&entryInfo);

    /** Clear the hit indication bitmap to be sure that no garbage values will be used. */
    *entry_hit_core_bmp_p = FALSE;

    /** Indicates TCAM bank IDs. We have 12 big banks and 4 small banks. */
    bank_id = 0;
    /** Indicates per TCAM bank SRAM IDs. We have 2 SRAMs per given TCAM bank. */
    bank_sram_id = 0;
    /** Indicates on which line in a given bank, the given entry is located. */
    bank_line_id = 0;
    /**
     * Indicates the exact entry in the SRAM line, which have to be checked for HIT.
     * Every SRAM line represents 8b bitmap, where every bit is different entry.
     */
    sram_line_entry_id = 0;
    /** Max number of big and small bank lines to be used for calculations.  */
    max_nof_big_bank_lines = max_nof_small_bank_lines = 0;
    max_nof_entries_in_two_srams = max_nof_srams_per_tcam_bank = 0;

    cmd_addr_reg_val = 0;
    SOC_REG_ABOVE_64_CLEAR(reg_val);

    /** Check if the given entry_handle is valid. */
    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, FALSE, entry_handle, entryGroup));
    if (entryGroup == SOC_PPC_FP_NOF_DBS) {
        LOG_CLI((BSL_META_U(unit, "The given entry %d (0x%08X) is invalid.\n"), entry_handle, entry_handle));
        BCM_EXIT;
    }

    BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit, FALSE, entry_handle, hwHandle));

    soc_sand_rv = _bcm_ppd_fp_entry_get(unitData, entry_handle, entryGroup, hwHandle, &found, &entryInfo);
    res = handle_sand_result(soc_sand_rv);
    if (BCM_E_NONE != res) {
        if (!found)
        {
             LOG_CLI((BSL_META_U(unit, "The given entry %d (0x%08X) is invalid.\n"), entry_handle, entry_handle));
             BCM_EXIT;
        }
    }
    BCMDNX_IF_ERR_EXIT(res);

    entry_abs_location.bank_id = 0;
    entry_abs_location.entry = 0;

    /** Get the absolute location of the given entry_handle. */
    arad_tcam_db_entry_id_to_location_entry_get(unit, entryGroup, entry_handle, FALSE, &entry_abs_location, &found);

    /** Get the bank_id from entry absolute location. */
    bank_id = entry_abs_location.bank_id;
    /** Max number of SRAMs per TCAM bank, equals to 2. */
    max_nof_srams_per_tcam_bank = 2;
    /** Max number of entries in two SRAMs of one TCAM banks, equals to 16. */
    max_nof_entries_in_two_srams = 8 * max_nof_srams_per_tcam_bank;
    /**
     * Calculate the bank_sram_id and sram_line_entry_id.
     * For example: ((1 * 2) + (3000 % 2) = (2 + 0) = 2
     *      For bank id 0 relevant SRAMs are 0 and 1,
     *      in our example we are at bank id 1 and there
     *      the relevant SRAMs are 2 and 3. Entry is located
     *      at location 3000, which is in the SRAM 2.
     */
    bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (entry_abs_location.entry % 2));
    /**
     * Calculates the bit on which the given entry is located in the calculated SRAM line.
     * Every SRAM line represents 8b bitmap, where every bit is different entry.
     * For example: (((3000 % 16) - (2 % 2)) / 2) = ((8 - 0) / 2) = 4
     *      For the given entry 3000, which is located in SRAM 2,
     *      the relevant bit from the SRAM line is bit 4. This
     *      bit will be checked at the end of the function.
     *      If equals to '1' we have a hit, otherwise we don't have a hit.
     */
    sram_line_entry_id =
            (((entry_abs_location.entry % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);

    /** Check if the given entry is located in one of the BIG banks, otherwise it is in the small banks. */
    if ((bank_id < SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks)))
    {
        mem = PPDB_A_TCAM_ACTION_HIT_INDICATIONm;

        max_nof_big_bank_lines = SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines);
        /** Calculate the bank_line_id for big banks. */
        bank_line_id = ((entry_abs_location.entry % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
    }
    else
    {
        switch (bank_sram_id)
        {
            case 24:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m;
                break;
            }
            case 25:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m;
                break;
            }
            case 26:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m;
                break;
            }
            case 27:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m;
                break;
            }
            case 28:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m;
                break;
            }
            case 29:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m;
                break;
            }
            case 30:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m;
                break;
            }
            case 31:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m;
                break;
            }
            default:
            {
                mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m;
                break;
            }
        }

        /** Get max values from DNX data. */
        max_nof_small_bank_lines = SOC_PPC_TCAM_NOF_LINES_ARAD_SMALL;
        /** Calculate the bank_line_id for small banks. */
        bank_line_id = ((entry_abs_location.entry % max_nof_small_bank_lines) / max_nof_entries_in_two_srams);
    }

    /** Get info about HW table */
    blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
    index_min = soc_mem_index_min(unit, mem);
    array_index_min = 0;

    /** Get the memory address by array_index, block_index and index. */
    mem_addr = soc_mem_addr_get(unit, mem, array_index_min, blk_index_min, index_min, &at);

    /**
     * Because we do the hit get operation for a given entry we should add to the base memory address
     * the bank line id on which the entry is located.
     */
    mem_addr += bank_line_id;

    BCMDNX_IF_ERR_EXIT(WRITE_PPDB_A_INDIRECT_COMMANDr(unit, 0x100003));

    cmd_addr_reg_val = 0;
    soc_reg_field_set(unit, PPDB_A_INDIRECT_COMMAND_ADDRESSr, &cmd_addr_reg_val, INDIRECT_COMMAND_ADDRf, mem_addr);
    /** Set to 0x1, because we will perform read operation. */
    soc_reg_field_set(unit, PPDB_A_INDIRECT_COMMAND_ADDRESSr, &cmd_addr_reg_val, INDIRECT_COMMAND_TYPEf, 0x1);
    BCMDNX_IF_ERR_EXIT(WRITE_PPDB_A_INDIRECT_COMMAND_ADDRESSr(unit, cmd_addr_reg_val));
    BCMDNX_IF_ERR_EXIT(READ_PPDB_A_INDIRECT_COMMAND_RD_DATAr(unit, reg_val));

    /** Check whether we have a hit. */
    if (SHR_BITGET(reg_val, sram_line_entry_id))
    {
        /**
         * In case we have a HIT for the given entry, set the output hit indication to TRUE,
         * because the entry is valid for both cores.
         */
        *entry_hit_core_bmp_p = TRUE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_hit_per_location_flush
 *  Purpose
 *     This function flushes hit information of an entry or all entries,
*      from hardware.
 *  Parameters
 *     (in) unit = unit information
 *     (in) entry_abs_location = Absolute location of an unique entry handle.
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_dpp_field_tcam_entry_hit_per_location_flush(
    int unit,
    ARAD_TCAM_LOCATION entry_abs_location)
{
    uint32 mem_addr;
    uint8 at;
    uint32 cmd_addr_reg_val;
    soc_reg_above_64_val_t reg_val;
    int blk_index, blk_index_min, blk_index_max;
    int index, index_min, index_max;
    int array_index, array_index_min = 0, array_index_max = 0;
    uint32 tcam_indirect_mask;
    uint8 mem_index, nof_tcam_hit_indication_memories;
    soc_mem_t mem = INVALIDm;
    /**
     * Define array with all memories, needed related to TCAM HIT indication.
     * They are 9:
     *      - 1 for big banks
     *      - 8 for small banks
     */
    CONST static soc_mem_t mem_array[9] = {
        PPDB_A_TCAM_ACTION_HIT_INDICATIONm,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m,
        PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m
    };
    uint32 bank_id, bank_sram_id, bank_line_id, sram_line_entry_id;
    uint32 max_nof_big_bank_lines, max_nof_small_bank_lines;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_srams_per_tcam_bank;

    BCMDNX_INIT_FUNC_DEFS;

    /** Indicates TCAM bank IDs. We have 12 big banks and 4 small banks. */
    bank_id = 0;
    /** Indicates per TCAM bank SRAM IDs. We have 2 SRAMs per given TCAM bank. */
    bank_sram_id = 0;
    /** Indicates on which line in a given bank, the given entry is located. */
    bank_line_id = 0;
    /**
     * Indicates the exact entry in the SRAM line, which have to be checked for HIT.
     * Every SRAM line represents 8b bitmap, where every bit is different entry.
     */
    sram_line_entry_id = 0;
    /** Max number of big and small bank lines to be used for calculations.  */
    max_nof_big_bank_lines = max_nof_small_bank_lines = 0;

    cmd_addr_reg_val = 0;
    SOC_REG_ABOVE_64_CLEAR(reg_val);

    if (entry_abs_location.entry != BCM_FIELD_ENTRY_INVALID)
    {
        /** Get the bank_id from entry absolute location. */
        bank_id = entry_abs_location.bank_id;
        /** Max number of SRAMs per TCAM bank, equals to 2. */
        max_nof_srams_per_tcam_bank = 2;
        /** Max number of entries in two SRAMs of one TCAM banks, equals to 16. */
        max_nof_entries_in_two_srams = 8 * max_nof_srams_per_tcam_bank;

        /**
         * Calculate the bank_sram_id and sram_line_entry_id.
         * For example: ((1 * 2) + (3000 % 2) = (2 + 0) = 2
         *      For bank id 0 relevant SRAMs are 0 and 1,
         *      in our example we are at bank id 1 and there
         *      the relevant SRAMs are 2 and 3. Entry is located
         *      at location 3000, which is in the SRAM 2.
         */
        bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (entry_abs_location.entry % 2));

        /** Check if the given entry is located in one of the BIG banks, otherwise it is in the small banks. */
        if ((bank_id < SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks)))
        {
            mem = PPDB_A_TCAM_ACTION_HIT_INDICATIONm;

            max_nof_big_bank_lines = SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines);
            /** Calculate the bank_line_id for big banks. */
            bank_line_id = ((entry_abs_location.entry % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
        }
        else
        {
            switch (bank_sram_id)
            {
                case 24:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m;
                    break;
                }
                case 25:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m;
                    break;
                }
                case 26:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m;
                    break;
                }
                case 27:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m;
                    break;
                }
                case 28:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m;
                    break;
                }
                case 29:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m;
                    break;
                }
                case 30:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m;
                    break;
                }
                case 31:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m;
                    break;
                }
                default:
                {
                    mem = PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m;
                    break;
                }
            }

            /** Get max values from DNX data. */
            max_nof_small_bank_lines = SOC_PPC_TCAM_NOF_LINES_ARAD_SMALL;
            /** Calculate the bank_line_id for small banks. */
            bank_line_id = ((entry_abs_location.entry % max_nof_small_bank_lines) / max_nof_entries_in_two_srams);
        }

        /**
         * Calculates the bit on which the given entry is located in the calculated SRAM line.
         * Every SRAM line represents 8b bitmap, where every bit is different entry.
         * For example: (((3000 % 16) - (2 % 2)) / 2) = ((8 - 0) / 2) = 4
         *      For the given entry 3000, which is located in SRAM 2,
         *      the relevant bit from the SRAM line is bit 4. This
         *      bit will be checked at the end of the function.
         *      If equals to '1' we have a hit, otherwise we don't have a hit.
         */
        sram_line_entry_id =
                (((entry_abs_location.entry % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);

        /** Mask the exact bit, which is corresponding to the entry, which have to be cleared. */
        tcam_indirect_mask = 0x1 << sram_line_entry_id;

        /**
         * In case we have to clear hit information for a specific entry_handle, we have only
         * one memory, which is being chosen above 'mem'.
         */
        nof_tcam_hit_indication_memories = 1;
    }
    else
    {
        /**
         * Set full mask in case there is no specific entry to be cleared,
         * it means that we are going to clear info for all entries.
         */
        tcam_indirect_mask = 0xFF;
        /** Do the clean for all BIG and SMALL bank memories. */
        nof_tcam_hit_indication_memories = 9;
    }

    /** Iterate over all hit indication memories. */
    for (mem_index = 0; mem_index < nof_tcam_hit_indication_memories; mem_index++)
    {
        /**
         * In case there is no specified entry we should do the clean for all
         * tcam hit indication memories.
         */
        if (entry_abs_location.entry == BCM_FIELD_ENTRY_INVALID)
        {
            mem = mem_array[mem_index];
        }

        /** Get info about HW table */
        blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
        index_min = soc_mem_index_min(unit, mem);
        array_index_min = 0;

        /** Get info about HW table */
        blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
        blk_index_max = SOC_MEM_BLOCK_MAX(unit, mem);
        index_min = soc_mem_index_min(unit, mem);
        index_max = soc_mem_index_max(unit, mem);

        /** Check if the memory is array. Then get the array min and max indexes. */
        if (SOC_MEM_IS_ARRAY(unit, mem))
        {
            array_index_max = (SOC_MEM_NUMELS(unit, mem) - 1) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            array_index_max = array_index_min = 0;
        }

        /** Iterate over memory array indexes. In this case of TCAM this is the SRAMs which we have per bank. */
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            /** Iterate over memory indexes. In this case of TCAM this is the LINEs per bank. */
            for (index = index_min; index <= index_max; index++)
            {
                /** Do the clean for TCAM0 and TCAM1 instances. */
                for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
                {
                    /** Get the memory address by array_index, block_index and index. */
                    mem_addr = soc_mem_addr_get(unit, mem, array_index, blk_index, index, &at);

                    /**
                     * In case we have specific entry, which have to be cleared, we should
                     * add to the base memory address the bank line id on which the entry is located.
                     */
                    if (entry_abs_location.entry != BCM_FIELD_ENTRY_INVALID)
                    {
                        mem_addr += bank_line_id;
                    }

                    BCMDNX_IF_ERR_EXIT(WRITE_PPDB_A_INDIRECT_COMMANDr(unit, 0x100003));

                    cmd_addr_reg_val = 0;
                    soc_reg_field_set(unit, PPDB_A_INDIRECT_COMMAND_ADDRESSr, &cmd_addr_reg_val, INDIRECT_COMMAND_ADDRf, mem_addr);
                    /** Set to 0x0, because we will perform write operation. */
                    soc_reg_field_set(unit, PPDB_A_INDIRECT_COMMAND_ADDRESSr, &cmd_addr_reg_val, INDIRECT_COMMAND_TYPEf, 0x0);
                    BCMDNX_IF_ERR_EXIT(WRITE_PPDB_A_INDIRECT_COMMAND_ADDRESSr(unit, cmd_addr_reg_val));

                    BCMDNX_IF_ERR_EXIT(WRITE_PPDB_A_INDIRECT_WR_MASKr(unit, tcam_indirect_mask));
                    BCMDNX_IF_ERR_EXIT(WRITE_PPDB_A_INDIRECT_COMMAND_WR_DATAr(unit, reg_val));;
                }

                /**
                 * In case of specific entry, we have to do the clean only for it on both TCAM0 and TCAM1
                 * instances and then go to the exit.
                 */
                if (entry_abs_location.entry != BCM_FIELD_ENTRY_INVALID)
                {
                    goto exit;
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_tcam_entry_hit_flush(
    bcm_dpp_field_info_OLD_t *unitData,
    uint32 flags,
    uint32 entry_handle)
{
    int res = BCM_E_NONE;
    int unit = 0;
    SOC_PPC_FP_ENTRY_INFO entryInfo;
    uint8 found;
    uint32 soc_sand_rv;
    _bcm_dpp_field_grp_idx_t entryGroup;
    uint32 hwHandle;
    ARAD_TCAM_LOCATION entry_abs_location;

    BCMDNX_INIT_FUNC_DEFS;

    unit = unitData->unit;
    if (flags & BCM_FIELD_ENTRY_HIT_FLUSH_ALL)
    {
        entry_abs_location.entry = BCM_FIELD_ENTRY_INVALID;

        /** Clear hit info for all entries. */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_hit_per_location_flush(unit, entry_abs_location));
    }
    else
    {
        /** Check if the given entry_handle is valid. */
        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit, FALSE, entry_handle, entryGroup));
        if (entryGroup == SOC_PPC_FP_NOF_DBS) {
            LOG_CLI((BSL_META_U(unit, "The given entry %d (0x%08X) is invalid.\n"), entry_handle, entry_handle));
            BCM_EXIT;
        }

        BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit, FALSE, entry_handle, hwHandle));

        soc_sand_rv = _bcm_ppd_fp_entry_get(unitData, entry_handle, entryGroup, hwHandle, &found, &entryInfo);
        res = handle_sand_result(soc_sand_rv);
        if (BCM_E_NONE != res) {
            if (!found)
            {
                 LOG_CLI((BSL_META_U(unit, "The given entry %d (0x%08X) is invalid.\n"), entry_handle, entry_handle));
                 BCM_EXIT;
            }
        }
        BCMDNX_IF_ERR_EXIT(res);

        /** Get the absolute location of the given entry_handle. */
        arad_tcam_db_entry_id_to_location_entry_get(unit, entryGroup, entry_handle, FALSE, &entry_abs_location, &found);

        /** Clear hit info for the given entry entry. */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_entry_hit_per_location_flush(unit, entry_abs_location));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  Functions and data exported to API users
 */


