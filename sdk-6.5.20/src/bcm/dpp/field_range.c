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
 *     Range comparison support.
 */

/*
 *  The functions in this module deal with preselectors on PPD devices.  They
 *  are invoked by the main field module when certain things occur, such as
 *  calls that explicitly involve preselectors, or calls that are more general
 *  but using a preselector handle, and in some cases to deal with implied use
 *  of preselectors (such as a group that has no specified preselector set).
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
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#include "bcm_int/common/debug.h"

#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/field.h>

#include <soc/drv.h>

#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/switch.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <shared/bitop.h>

/*
 *  For now, if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT is FALSE, disable warm boot
 *  support here.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#if !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#undef BCM_WARM_BOOT_SUPPORT
#endif  /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dcmn/dcmn_wb.h>
#endif /* !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT */

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif


/******************************************************************************
 *
 *  Local functions and data
 */

/*
 *   Function
 *      _bcm_dpp_field_range_info_from_qual
 *   Purpose
 *      Get range info associated with a particular type of range, based upon
 *      the specified qualifier.
 *   Parameters
 *      (in) unitData = unit information
 *      (in) bcm_field_qualify_t qual = qualifier for the range type
 *      (out) int32 *qualFlags = where to put the qualifier flags
 *      (out) uint32 *rangeFlags = where to put the range flags
 *      (out) bcm_field_range_t *base = where to put range type base ID
 *      (out) _bcm_dpp_field_range_idx_t *count = where to put range type count
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_NOT_FOUND if not valid
 *                    BCM_E_* appropriately otherwise
 */
STATIC int
_bcm_dpp_field_range_info_from_qual(bcm_dpp_field_info_OLD_t  *unitData,
                                    bcm_field_qualify_t qual,
                                    int32 *qualifierFlags,
                                    uint32 *rangeFlags,
                                    bcm_field_range_t *base,
                                    _bcm_dpp_field_range_idx_t *count)
{
    int index;
    int unit = unitData->unit ;
    int32 *qualMap ;
    int32 qualMaps_handle ;
    int ret ;

    for (index = 0;
         (bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier) &&
         (qual != unitData->devInfo->ranges[index].qualifier);
         index++) {
        /* just scan for it or fall off the end */
    }
    if (unitData->devInfo->ranges[index].qualifier == qual) {
        ret = FIELD_ACCESS_QUALMAPS.get(unit, qual, &qualMaps_handle) ;
        if (ret != BCM_E_NONE) {
            goto exit ;
        }
        SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d index %d range qual %s (%d)"
                              " qualFlags %08X rangeFlags %08X base %d"
                              " count %d\n"),
                   unit,
                   index,
                   _bcm_dpp_field_qual_name[unitData->devInfo->ranges[index].qualifier],
                   unitData->devInfo->ranges[index].qualifier,
                   qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS],
                   unitData->devInfo->ranges[index].rangeFlags,
                   unitData->devInfo->ranges[index].rangeBase,
                   unitData->devInfo->ranges[index].count));
        if (qualifierFlags) {
            *qualifierFlags = qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS];
        }
        if (rangeFlags) {
            *rangeFlags = unitData->devInfo->ranges[index].rangeFlags;
        }
        if (base) {
            *base = unitData->devInfo->ranges[index].rangeBase;
        }
        if (count) {
            *count = unitData->devInfo->ranges[index].count;
        }
        ret = BCM_E_NONE;
        goto exit ;
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d does not support qualifier %s (%d) as"
                              " a range qualifier\n"),
                   unit,
                   _bcm_dpp_field_qual_name[qual],
                   qual));
        ret = BCM_E_NOT_FOUND;
        goto exit ;
    }
exit:
    return (ret) ;
}

/*
 *   Function
 *      _bcm_dpp_field_range_info_from_flags
 *   Purpose
 *      Get range info associated with a particular type of range, based upon
 *      the provided range type flags.
 *   Parameters
 *      (in) unitData = unit information
 *      (in) uint32 flags = flags to query
 *      (out) bcm_field_qualify_t *qual = where to put qualifier
 *      (out) int32 *qualFlags = where to put the qualifier flags
 *      (out) uint32 *rangeFlags = where to put the range flags
 *      (out) bcm_field_range_t *base = where to put range type base ID
 *      (out) _bcm_dpp_field_range_idx_t *count = where to put range type count
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_NOT_FOUND if not valid
 *                    BCM_E_* appropriately otherwise
 */
STATIC int
_bcm_dpp_field_range_info_from_flags(bcm_dpp_field_info_OLD_t  *unitData,
                                     uint32 flags,
                                     bcm_field_qualify_t *qual,
                                     int32 *qualifierFlags,
                                     uint32 *rangeFlags,
                                     bcm_field_range_t *base,
                                     _bcm_dpp_field_range_idx_t *count)
{
    int index;	
    int unit = unitData->unit;
    int32 *qualMap ;
    int32 qualMaps_handle ;
    int ret ;

    for (index = 0;
         (bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier) &&
         (0 == (flags & unitData->devInfo->ranges[index].rangeFlags));
         index++) {
        /* just scan for it or fall off the end */
    }
    if (bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier) {
        if (qual) {
            *qual = unitData->devInfo->ranges[index].qualifier;
        }
        if (qualifierFlags) {
            ret = FIELD_ACCESS_QUALMAPS.get(unit, (unitData->devInfo->ranges[index].qualifier), &qualMaps_handle) ;
            if (ret != BCM_E_NONE) {
                goto exit ;
            }
            SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
            *qualifierFlags = qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS];
        }
        if (rangeFlags) {
            *rangeFlags = unitData->devInfo->ranges[index].rangeFlags;
        }
        if (base) {
            *base = unitData->devInfo->ranges[index].rangeBase;
        }
        if (count) {
            *count = unitData->devInfo->ranges[index].count;
        }
        ret = BCM_E_NONE;
        goto exit ;
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d range flags %08X not supported\n"),
                              unit,
                   flags));
        ret = BCM_E_NOT_FOUND;
        goto exit ;
    }
exit:
    return (ret) ;
}

/*
 *   Function
 *      _bcm_dpp_field_range_info_from_id
 *   Purpose
 *      Get range info associated with a particular type of range, based upon
 *      the provided range ID.
 *   Parameters
 *      (in) unitData = unit information
 *      (in) bcm_field_range_t range = range ID
 *      (out) bcm_field_qualify_t *qual = where to put qualifier
 *      (out) int32 *qualFlags = where to put the qualifier flags
 *      (out) uint32 *rangeFlags = where to put the range flags
 *      (out) bcm_field_range_t *base = where to put range type base ID
 *      (out) _bcm_dpp_field_range_idx_t *count = where to put range type count
 *      (out) _bcm_dpp_field_range_idx_t *offset = where to put range index
 *      (out) int *exists = where to put whether range exists or not
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_NOT_FOUND if not valid
 *                    BCM_E_* appropriately otherwise
 */
STATIC int
_bcm_dpp_field_range_info_from_id(bcm_dpp_field_info_OLD_t  *unitData,
                                  bcm_field_range_t range,
                                  bcm_field_qualify_t *qual,
                                  int32 *qualifierFlags,
                                  uint32 *rangeFlags,
                                  bcm_field_range_t *base,
                                  _bcm_dpp_field_range_idx_t *count,
                                  _bcm_dpp_field_range_idx_t *offset,
                                  int *exists)
{
    int index;
    int unit = unitData->unit;
    int32 *qualMap ;
    int32 qualMaps_handle ;
    int ret ;

    for (index = 0;
         (bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier) &&
         ((range < unitData->devInfo->ranges[index].rangeBase) ||
          (range >= (unitData->devInfo->ranges[index].rangeBase +
                     unitData->devInfo->ranges[index].count)));
         index++) {
        /* just scan for it or fall off the end */
    }
    if (bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier) {
        uint8 bit_value ;

        ret = FIELD_ACCESS_QUALMAPS.get(unit, (unitData->devInfo->ranges[index].qualifier), &qualMaps_handle) ;
        if (ret != BCM_E_NONE) {
            goto exit ;
        }
        SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
        ret =
            FIELD_ACCESS_RANGEINUSE.bit_get(
                unit,index,range - unitData->devInfo->ranges[index].rangeBase,&bit_value) ;
        if (ret != BCM_E_NONE) {
            goto exit ;
        }
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                              "unit %d index %d range %d qual %s (%d)"
                              " qualFlags %08X rangeFlags %08X base %d"
                              " count %d offset %d exists %s\n"),
                   unit,
                   index,
                   range,
                   _bcm_dpp_field_qual_name[unitData->devInfo->ranges[index].qualifier],
                   unitData->devInfo->ranges[index].qualifier,
                   qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS],
                   unitData->devInfo->ranges[index].rangeFlags,
                   unitData->devInfo->ranges[index].rangeBase,
                   unitData->devInfo->ranges[index].count,
                   range - unitData->devInfo->ranges[index].rangeBase,
                   /*
                    * Was:
                    *   SHR_BITGET(unitData->rangeInUse[index],
                    *   range - unitData->devInfo->ranges[index].rangeBase)?"TRUE":"FALSE"
                    */
                   bit_value?"TRUE":"FALSE"
                )
        );
        if (qual) {
            *qual = unitData->devInfo->ranges[index].qualifier;
        }
        if (qualifierFlags) {
            *qualifierFlags = qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS];
        }
        if (rangeFlags) {
            *rangeFlags = unitData->devInfo->ranges[index].rangeFlags;
        }
        if (base) {
            *base = unitData->devInfo->ranges[index].rangeBase;
        }
        if (count) {
            *count = unitData->devInfo->ranges[index].count;
        }
        if (offset) {
            *offset = range - unitData->devInfo->ranges[index].rangeBase;;
        }
        if (exists) {
            uint8 bit_value ;
            ret =
                FIELD_ACCESS_RANGEINUSE.bit_get(
                    unit,index,range - unitData->devInfo->ranges[index].rangeBase,&bit_value) ;
            if (ret != BCM_E_NONE) {
                goto exit ;
            }
            /*
             * Was:
             *   if (SHR_BITGET(unitData->rangeInUse[index],
             *       range - unitData->devInfo->ranges[index].rangeBase))
             */
            if (bit_value) {
                *exists = TRUE;
            } else {
                *exists = FALSE;
            }
        }
        ret = BCM_E_NONE;
        goto exit ;
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d range ID %d is not valid\n"),
                   unit,
                   range));
        ret = BCM_E_NOT_FOUND;
        goto exit ;
    }
exit:
    return (ret) ;
}

/*
 *   Function
 *      _bcm_dpp_field_range_mark_inuse
 *   Purpose
 *      Mark a particular range as in-use or not-in-use.
 *   Parameters
 *      (in) unitData = unit information
 *      (in) bcm_field_range_t range = range ID
 *      (in) int inuse = TRUE for in use, FALSE for not in use
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_NOT_FOUND if not valid
 *                    BCM_E_* appropriately otherwise
 */
STATIC int
_bcm_dpp_field_range_mark_inuse(bcm_dpp_field_info_OLD_t  *unitData,
                                bcm_field_range_t range,
                                int inuse)
{
    int index;
    int unit = unitData->unit;
    int ret ;

    for (index = 0;
         (bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier) &&
         ((range < unitData->devInfo->ranges[index].rangeBase) ||
          (range >= (unitData->devInfo->ranges[index].rangeBase +
                     unitData->devInfo->ranges[index].count)));
         index++) {
        /* just scan for it or fall off the end */
    }
    if (bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier) {
        if (!SOC_WARM_BOOT(unit)) {
            if (inuse) {
                /*
                 * Was:
                 *   SHR_BITSET(unitData->rangeInUse[index],
                 *       range - unitData->devInfo->ranges[index].rangeBase);
                 */
                ret =
                    FIELD_ACCESS_RANGEINUSE.bit_set(
                        unit,index,range - unitData->devInfo->ranges[index].rangeBase) ;
                if (ret != BCM_E_NONE) {
                    goto exit ;
                }
            } else {
                /*
                 * Was:
                 *   SHR_BITCLR(unitData->rangeInUse[index],
                 *       range - unitData->devInfo->ranges[index].rangeBase);
                 */
                ret =
                    FIELD_ACCESS_RANGEINUSE.bit_clear(
                        unit,index,range - unitData->devInfo->ranges[index].rangeBase) ;
                if (ret != BCM_E_NONE) {
                    goto exit ;
                }
            }
        }
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
        _bcm_dpp_field_range_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
        ret = BCM_E_NONE;
        goto exit ;
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d range ID %d is not valid\n"),
                   unit,
                   range));
        ret = BCM_E_NOT_FOUND;
        goto exit ;
    }
exit:
    return (ret) ;
}

/*
 *   Function
 *      _bcm_dpp_field_range_get
 *   Purpose
 *      Get parameters for a range
 *   Parameters
 *      (in) unitData = unit information
 *      (in) bcm_field_range_t range = range ID to get
 *      (out) uint32 flags = flags for the range
 *      (out) uint32 *min = pointer to min port for the range
 *      (out) uint32 *max = pointer to max port for the range
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_NOT_FOUND if not valid
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
 int
_bcm_dpp_field_range_get(bcm_dpp_field_info_OLD_t  *unitData,
                         bcm_field_range_t range,
                         int maxCount,
                         uint32 *flags,
                         uint32 *min,
                         uint32 *max,
                         int *actualCount)
{
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    uint32 soc_sand_rv;
    int status = BCM_E_NONE;
    int exists = FALSE;
    _bcm_dpp_field_range_idx_t index;
    unsigned int offset;
    bcm_field_qualify_t qual;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* Check whether the range is supported range */
    status = _bcm_dpp_field_range_info_from_id(unitData,
                                               range,
                                               &qual,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &index,
                                               &exists);
    BCMDNX_IF_ERR_EXIT(status);
    if (!exists) {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d range %d not in use"),
                          unit,
                          range));
    }
    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
    switch (qual) {
    case bcmFieldQualifyL4PortRangeCheck:
        /* get the range settings */
        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE;
        control_ndx.val_ndx = index;
        soc_sand_rv = soc_ppd_fp_control_get(unitData->unitHandle,
                                             SOC_CORE_INVALID, /* Not used */
                                             &control_ndx,
                                             &control_info);
        status = handle_sand_result(soc_sand_rv);
        if (BCM_E_NONE != status) {
            BCMDNX_ERR_EXIT_MSG(status,
                             (_BSL_BCM_MSG_NO_UNIT("unable to get unit %d range"
                                               " %d: %d (%s)\n"),
                              unit,
                              range,
                              status,
                              _SHR_ERRMSG(status)));
        }
        /* parse them back into BCM range settings */
        offset = 0;
        *actualCount = 0;
        if ((0x0000 < control_info.val[0]) ||
            (0xFFFF > control_info.val[1])) {
            /* the source port range is not 'all possible values' */
            if (maxCount > offset) {
                /* the caller provided enough buffer for it */
                flags[offset] = BCM_FIELD_RANGE_TCP |
                                BCM_FIELD_RANGE_UDP |
                                BCM_FIELD_RANGE_SRCPORT;
                min[offset] = control_info.val[0];
                max[offset] = control_info.val[1];
                (*actualCount)++;
            }
            offset++;
        }
        if ((0x0000 < control_info.val[2]) ||
            (0xFFFF > control_info.val[3])) {
            /* the dest port range is not all possible values */
            if ((control_info.val[0] == control_info.val[1]) &&
                (control_info.val[1] == control_info.val[3])) {
                /* same values for both, set dest also */
                flags[offset - 1] |= BCM_FIELD_RANGE_DSTPORT;
            } else {
                if (maxCount > offset) {
                    /* different values and enough space for second one */
                    flags[offset] = BCM_FIELD_RANGE_TCP |
                                    BCM_FIELD_RANGE_UDP |
                                    BCM_FIELD_RANGE_DSTPORT;
                    min[offset] = control_info.val[2];
                    max[offset] = control_info.val[3];
                    (*actualCount)++;
                }
                offset++;
            }
        }
        if (0 == maxCount) {
            /* max count is zero; report the required number of range values */
            *actualCount = offset;
        } else if (0 == offset) {
            /* valid but nothing was programmed, assume entire valid space */
            flags[offset] = BCM_FIELD_RANGE_TCP |
                            BCM_FIELD_RANGE_UDP |
                            BCM_FIELD_RANGE_SRCPORT |
                            BCM_FIELD_RANGE_DSTPORT;
            min[offset] = 0x0000;
            max[offset] = 0xFFFF;
            *actualCount = 1;
        }
        break;
    case bcmFieldQualifyPacketLengthRangeCheck:
        /* get the range settings */
        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE;
        control_ndx.val_ndx = index;
        soc_sand_rv = soc_ppd_fp_control_get(unitData->unitHandle,
                                             SOC_CORE_INVALID,
                                             &control_ndx,
                                             &control_info);
        status = handle_sand_result(soc_sand_rv);
        if (BCM_E_NONE != status) {
            BCMDNX_ERR_EXIT_MSG(status,
                             (_BSL_BCM_MSG_NO_UNIT("unable to get unit %d range"
                                               " %d: %d (%s)\n"),
                              unit,
                              range,
                              status,
                              _SHR_ERRMSG(status)));
        }
        /* parse them back into BCM range settings */
        offset = 0;
        *actualCount = 0;
        if (maxCount > offset) {
            flags[offset] = BCM_FIELD_RANGE_PACKET_LENGTH;
            min[offset] = control_info.val[0];
            max[offset] = control_info.val[1];
            (*actualCount)++;
        }
        offset++;
        if (0 == maxCount) {
            /* max count is zero; report the required number of range values */
            *actualCount = offset;
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d range %d is not valid"),
                          unit,
                          range));
    }
    /* see if the range is in use */
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_field_range_set_id
 *   Purpose
 *      Set parameters for a range
 *   Parameters
 *      (in) unitData = unit information
 *      (in) bcm_field_range_t range = range ID to set
 *      (in) uint32 flags = flags for the range
 *      (in) bcm_l4_port_t min = min port for the range
 *      (in) bcm_l4_port_t max = max port for the range
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_NOT_FOUND if not valid
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Must be called with the lock held
 */
STATIC int
_bcm_dpp_field_range_set_id(bcm_dpp_field_info_OLD_t  *unitData,
                            int replace,
                            bcm_field_range_t range,
                            int count,
                            uint32 *flags,
                            uint32 *min,
                            uint32 *max)
{
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    uint32 soc_sand_rv;
    int status = BCM_E_NONE;
    int exists;
    SOC_SAND_SUCCESS_FAILURE success = SOC_SAND_SUCCESS;
    _bcm_dpp_field_range_idx_t index;
    int offset;
    uint32 eflags = 0;
    bcm_field_qualify_t qual;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 < count) && ((!flags) || (!min) || (!max))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d range %d set with positive"
                                           " count but NULL pointer to at"
                                           " least one settings array"),
                          unit,
                          range));
    }
    /*
     *  Verify the parameters and build the settings
     */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_range_info_from_id(unitData,
                                                              range,
                                                              &qual,
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              &index,
                                                              &exists));
    if ((0 <= count) && exists && (!replace)) {
        /* creating, it exists, not replacing: BCM_E_EXISTS */
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d range %d already exists"),
                          unit,
                          range));
    }
    if (((0 > count) || replace) && (!exists)) {
        /* destroying or replacing, does not exist: BCM_E_NOT_FOUND */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d range %d does not exist"),
                          unit,
                          range));
    }
    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
    control_ndx.val_ndx = index;
    switch (qual)
    {
        case bcmFieldQualifyL4PortRangeCheck:
        {
            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE;
            control_info.val[0] = 0x0000;
            control_info.val[1] = 0xFFFF;
            control_info.val[2] = 0x0000;
            control_info.val[3] = 0xFFFF;
            /* process the range specifications */
            for (offset = 0; offset < count; offset++) {
                /* Look for unsupported flags */
                if (flags[offset] & (~(BCM_FIELD_RANGE_DSTPORT |
                                       BCM_FIELD_RANGE_SRCPORT |
                                       BCM_FIELD_RANGE_TCP |
                                       BCM_FIELD_RANGE_UDP))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d is a"
                                                       " TCP/UDP source/dest port"
                                                       " range, and does not"
                                                       " support flags %08X at"
                                                       " offset %d"),
                                      unit,
                                      range,
                                      flags[offset] & (~(BCM_FIELD_RANGE_DSTPORT |
                                                         BCM_FIELD_RANGE_SRCPORT |
                                                         BCM_FIELD_RANGE_TCP |
                                                         BCM_FIELD_RANGE_UDP)),
                                      offset));
                }
                if ((BCM_FIELD_RANGE_TCP | BCM_FIELD_RANGE_UDP) !=
                    (flags[offset] & (BCM_FIELD_RANGE_TCP | BCM_FIELD_RANGE_UDP))) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d range %d L4 port offset %d"
                                         " will assume both TCP and UDP despite"
                                         " flags not specifying both"),
                              unit,
                              range,
                              offset));
                }
                if (0 == (flags[offset] & (BCM_FIELD_RANGE_SRCPORT |
                                           BCM_FIELD_RANGE_DSTPORT))) {
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d range %d L4 port offset %d"
                                         " has no effect (it specifies neither"
                                         " source nor destination port)"),
                              unit,
                              range,
                              offset));
                }
                /* Verify the Min / Max port */
                if (0xFFFF < min[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d L4 port"
                                                       " min %d (%04X) at offset"
                                                       " %d is invalid"),
                                      unit,
                                      range,
                                      min[offset],
                                      min[offset],
                                      offset));
                }
                if (0xFFFF < max[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d L4 port"
                                                       " max %d (%04X) at offset"
                                                       " %d is invalid"),
                                      unit,
                                      range,
                                      max[offset],
                                      max[offset],
                                      offset));
                }
                if (min[offset] > max[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d L4 port"
                                                       " min %d (%04X) is greater"
                                                       " than max %d (%04X) at"
                                                       " offset %d"),
                                      unit,
                                      range,
                                      min[offset],
                                      min[offset],
                                      max[offset],
                                      max[offset],
                                      offset));
                }
                /* make sure there are no contradictions/overrides */
                if (eflags &
                    (BCM_FIELD_RANGE_SRCPORT | BCM_FIELD_RANGE_DSTPORT) &
                    flags[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d L4 port"
                                                       " offset %d specifies flags"
                                                       " trying to overwrite a"
                                                       " prior offset"),
                                      unit,
                                      range,
                                      offset));
                }
                eflags |= flags[offset];
                /* configure the appropriate parameters for hardware */
                if (flags[offset] & BCM_FIELD_RANGE_SRCPORT) {
                    control_info.val[0] = min[offset];
                    control_info.val[1] = max[offset];
                }
                if (flags[offset] & BCM_FIELD_RANGE_DSTPORT) {
                    control_info.val[2] = min[offset];
                    control_info.val[3] = max[offset];
                }
            } /* for (offset = 0; offset < count; offset++) */
            break;
        }
        case bcmFieldQualifyPacketLengthRangeCheck:
        {
            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE;
            control_info.val[0] = 0x01;
            control_info.val[1] = 0x7F;
            /* process the range specifications */
            for (offset = 0; offset < count; offset++) {
                /* Look for unsupported flags */
                if (flags[offset] & (~(BCM_FIELD_RANGE_PACKET_LENGTH))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d is a"
                                                       " packet length range,"
                                                       " and does not support"
                                                       " flags %08X at offset %d"),
                                      unit,
                                      range,
                                      flags[offset] & (~(BCM_FIELD_RANGE_PACKET_LENGTH)),
                                      offset));
                }
                /* Verify the Min / Max port */
                if ((1 > min[offset]) || (0x7F < min[offset])) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d packet"
                                                       " length min %d (%02X)"
                                                       " at offset %d is invalid"),
                                      unit,
                                      range,
                                      min[offset],
                                      min[offset],
                                      offset));
                }
                if ((1 > max[offset]) || (0x7F < max[offset])) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d packet"
                                                       " length max %d (%02X)"
                                                       " at offset %d is invalid"),
                                      unit,
                                      range,
                                      max[offset],
                                      max[offset],
                                      offset));
                }
                if (min[offset] > max[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d packet"
                                                       " length min %d (%02X) is"
                                                       " greater than max %d"
                                                       " (%02X) at offset %d"),
                                      unit,
                                      range,
                                      min[offset],
                                      min[offset],
                                      max[offset],
                                      max[offset],
                                      offset));
                }
                /* make sure there are no contradictions/overrides */
                if (eflags & (BCM_FIELD_RANGE_PACKET_LENGTH) & flags[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d packet"
                                                       " length offset %d"
                                                       " specifies flags trying"
                                                       " to overwrite a prior"
                                                       " offset"),
                                      unit,
                                      range,
                                      offset));
                }
                eflags |= flags[offset];
                /* configure the appropriate parameters for hardware */
                if (flags[offset] & BCM_FIELD_RANGE_PACKET_LENGTH) {
                    control_info.val[0] = min[offset];
                    control_info.val[1] = max[offset];
                }
            } /* for (offset = 0; offset < count; offset++) */
            break;
        }
        case bcmFieldQualifyVPortRangeCheck:
        {
            uint32 minimum, maximum ;
            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_OUT_LIF_RANGE ;
            minimum = 1 ;
#if defined(BCM_DPP_SUPPORT)
            if (SOC_IS_DPP(unit))
            {
                maximum = SOC_DPP_DEFS_GET(unit, nof_out_lifs) - 1 ;
            }
            else
#endif
            {
                maximum = 0xFF ;
            }
            /*
             * Set default for minimum
             */
            control_info.val[0] = minimum ;
            /*
             * Set default for maximum
             */
            control_info.val[1] = maximum ;
            /*
             * minimum/maximum will also be used to verify range on input parameters.
             */
            /*
             * process the range specifications
             */
            for (offset = 0; offset < count; offset++) {
                /*
                 * Look for unsupported flags
                 */
                if (flags[offset] & (~(BCM_FIELD_RANGE_OUT_VPORT))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d is a"
                                                       " out-lif (vport) range,"
                                                       " and does not support"
                                                       " flags %08X at offset %d"),
                                      unit,
                                      range,
                                      flags[offset] & (~(BCM_FIELD_RANGE_OUT_VPORT)),
                                      offset));
                }
                /*
                 * Verify the Min / Max port
                 */
                if ((minimum > min[offset]) || (maximum < min[offset])) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d out-lif (vport) min %d (%08lX)"
                                                       " at offset %d is invalid. Required range: %lu,%lu"),
                                      unit,
                                      range,
                                      (int)(min[offset]),
                                      min[offset],
                                      offset));
                }
                if ((minimum > max[offset]) || (maximum < max[offset])) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d out-lif (vport) max %d (%08lX)"
                                                       " at offset %d is invalid. Required range: %lu,%lu"),
                                      unit,
                                      range,
                                      (int)(max[offset]),
                                      max[offset],
                                      offset,minimum,maximum));
                }
                if (min[offset] > max[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d out-lif (vport) min %d (%08lX) is"
                                                       " greater than max %d (%08lX) at offset %d"),
                                      unit,
                                      range,
                                      (int)(min[offset]),
                                      min[offset],
                                      (int)(max[offset]),
                                      max[offset],
                                      offset));
                }
                /*
                 * Make sure there are no contradictions/overrides
                 */
                if (eflags & (BCM_FIELD_RANGE_OUT_VPORT) & flags[offset]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d range %d packet"
                                     " length offset %d specifies flags trying to overwrite a prior"
                                     " offset. Current flags %08lX ; new flags 0x%08lX"),
                                      unit,
                                      range,
                                      offset,eflags,flags[offset]));
                }
                eflags |= flags[offset];
                /*
                 * configure the appropriate parameters for hardware
                 */
                if (flags[offset] & BCM_FIELD_RANGE_OUT_VPORT) {
                    control_info.val[0] = min[offset];
                    control_info.val[1] = max[offset];
                }
            } /* for (offset = 0; offset < count; offset++) */
            break;
        }
        default:
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d range %d range valid"
                                               " check indicated valid, but"
                                               " %s (%d) is unsupported"),
                              unit,
                              range,
                              _bcm_dpp_field_qual_name[qual],
                              qual));
            break ;
        }
    } /* switch (type) */

    /* set the parameters to hardware */
    soc_sand_rv = soc_ppd_fp_control_set(unitData->unitHandle,
                                         SOC_CORE_INVALID,
                                         &control_ndx,
                                         &control_info,
                                         &success);
    status = handle_sand_result(soc_sand_rv);
    if (BCM_E_NONE == status) {
        status = translate_sand_success_failure(success);
    }
    BCMDNX_IF_ERR_EXIT_MSG(status,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d range %d unable to set to"
                                          " hardware: %d (%s)"),
                         unit,
                         range,
                         status,
                         _SHR_ERRMSG(status)));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_range_mark_inuse(unitData,
                                                            range,
                                                            (0 <= count)));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_field_qualify_RangeCheck
 *   Purpose
 *      Set range (for example expected TCP/UDP port rtange) for this entry
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_qualify_t qual = expected range qualifier type
 *      (in) bcm_field_entry_t entry = the entry ID
 *      (in) bcm_field_range_t range = the range ID
 *      (in) int invert = whether the range match is to be inverted
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Accepts any range type if qual is bcmFieldQualifyCount, but will only
 *      accept a range of type qual otherwise.
 */
STATIC int
_bcm_dpp_field_qualify_RangeCheck(bcm_dpp_field_info_OLD_t  *unitData,
                                  bcm_field_qualify_t qual,
                                  bcm_field_entry_t entry,
                                  bcm_field_range_t range,
                                  int invert)
{
    uint64 mask;
    uint64 edata;
    uint64 emask;
    bcm_field_qualify_t rangeQual;
    int32 qualFlags;
    int exists;
    int result;
    _bcm_dpp_field_range_idx_t index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if(_BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, entry)) {
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        uint32 res;
        /* Normalize input from bcmFieldQualifyRangeCheck to bcmFieldQualifyL4PortRangeCheck */
        if(qual == bcmFieldQualifyCount) {
            qual = bcmFieldQualifyL4PortRangeCheck;
        }
        res = arad_kbp_acl_range_qualify_add(unitData, qual, entry, range, &result);
        if(res) {
            if(result == BCM_E_NONE) {
                result = BCM_E_INTERNAL;
            }
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("arad_kbp_acl_range_qualify_add failed\n")));
        }

        BCM_EXIT;
#else
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit,"Using external field entries is not supported without KBP libraries compiled\n")));

        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
#endif
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_range_info_from_id(unitData,
                                                              range,
                                                              &rangeQual,
                                                              &qualFlags,
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              &index,
                                                              &exists));
    if (!exists) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d range %d not in use"),
                          unit,
                          range));
    }
    if (((0 > qual) || (bcmFieldQualifyCount < qual)) &&
        (qual != rangeQual)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("expected range of type %s (%d),"
                                           " but provided range is type"
                                           " %s (%d)"),
                          _bcm_dpp_field_qual_name[qual],
                          qual,
                          _bcm_dpp_field_qual_name[rangeQual],
                          rangeQual));
    }

    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        uint8 bit_value ;
        bcm_field_presel_t index;

        index = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry) ;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
        if (!bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d specifieed preselector %d not in use"),
                          unit,
                          index));
        }
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_exists(unitData, entry));
    }

    result = _bcm_dpp_field_entry_qualifier_general_get_int(unitData,
                                                            entry,
                                                            rangeQual,
                                                            1 /* buffers */,
                                                            &edata,
                                                            &emask);
    if (BCM_E_NOT_FOUND == result) {
        /*
         *  We have already made sure the entry exists, so NOT_FOUND
         *  here simply means the qualifier was not found: no ranges of this
         *  type yet.  Use zero as the basis and do not consider as error.
         */
        COMPILER_64_ZERO(edata);
        COMPILER_64_ZERO(emask);
        result = BCM_E_NONE;
    }
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d range %d unexpected error"
                                          " getting entry %d qualifier %s"
                                          " (%d): %d (%s)"),
                         unit,
                         range,
                         entry,
                         _bcm_dpp_field_qual_name[rangeQual],
                         rangeQual,
                         result,
                         _SHR_ERRMSG(result)));
    /* adjust the qualifier according to the range format */
    if (qualFlags & _BCM_DPP_QUAL_RANGE_BITMAP) {
        /*
         *  Range is bitmap; set appropriate mask bit to TRUE and then set
         *  the corresponding data bit to FALSE or TRUE according to
         *  whether INVERT is set or not.
         */
        COMPILER_64_SET(mask, 0, 1);
        COMPILER_64_SHL(mask, index);
        COMPILER_64_OR(emask, mask);
        if (invert) {
            COMPILER_64_NOT(mask);
            COMPILER_64_AND(edata, mask);
        } else {
            COMPILER_64_OR(edata, mask);
        }
    } else if (qualFlags & _BCM_DPP_QUAL_RANGE_INDEX) {
        /* Range is index; set unbiased ID as data, all bits mask */
        if (invert) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " INVERT for range %d of type"
                                               " %s (%d)"),
                              unit,
                              range,
                              _bcm_dpp_field_qual_name[rangeQual],
                              rangeQual));
        } else {
            /* normal match */
            COMPILER_64_SET(edata, 0, index);
            COMPILER_64_SET(emask, ~0, ~0);
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not have valid format"
                                           " information for range %d of type"
                                           " %s (%d)"),
                          unit,
                          range,
                          _bcm_dpp_field_qual_name[rangeQual],
                          rangeQual));
    }
    /* Range qualifier data/mask adjusted appropriately, set it */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_qualifier_general_set_int(unitData,
                                                                           entry,
                                                                           rangeQual,
                                                                           1 /* buffers */,
                                                                           &edata,
                                                                           &emask));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_field_qualify_RangeCheck_get
 *   Purpose
 *      Get match criteria for bcmFieldQualifyRangeCheck qualifier from the
 *      field entry (or preselector).
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t = the entry ID
 *      (in) int max_count = max ranges to return
 *      (out) bcm_field_range_t *range = ptr to array[max_count] of range IDs
 *      (out) int *invert = pointer to array[max_count] of invert flags
 *      (out) int *count = pointer to where to put the actual count
 *   Returns
 *      bcm_error_t cast as int
 *         BCM_E_NONE for success
 *         BCM_E_* otherwise as appropriate
 *   Notes
 *      Provide max_count of zero to get the number of elements required for
 *      the specific range type.
 *
 *      This may write to the provided buffers even if it returns an error.
 *
 *      If qual is bcmFieldQualifyCount, will consider all range types; if qual
 *      is otherwise specified, will only consider ranges of type qual.
 */
STATIC int
_bcm_dpp_field_qualify_RangeCheck_get(bcm_dpp_field_info_OLD_t  *unitData,
                                      bcm_field_qualify_t qual,
                                      bcm_field_entry_t entry,
                                      int max_count,
                                      bcm_field_range_t *range,
                                      int *invert,
                                      int *count)
{
    uint64 edata;
    uint64 emask;
    uint64 mask;
    uint64 temp;
    int32 qualFlags=0;
    unsigned int index;
    bcm_field_range_t offset=0;
    bcm_field_range_t limit;
    _bcm_dpp_field_range_idx_t rangeCount=0;
    int total = 0;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 < max_count) && ((!range) || (!invert))) {
        /* max_count is sensible but no space provided */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("obligatory OUT arguments must not"
                                           " be NULL")));
    }
    if (0 > max_count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("max_count %d must be non-negative"),
                          max_count));
    }
    if (!count) {
        /* but outbound count is obligatory */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("obligatory OUT arguments must not"
                                           " be NULL")));
    }

    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        uint8 bit_value ;
        bcm_field_presel_t index;

        index = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry) ;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
        if (!bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d specifieed preselector %d not in use"),
                          unit,
                          index));
        }
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_exists(unitData, entry));
    }
    *count = 0;
    for (index = 0;
         bcmFieldQualifyCount > unitData->devInfo->ranges[index].qualifier;
         index++) {
        if ((BCM_FIELD_ENTRY_INVALID == qual) ||
            (unitData->devInfo->ranges[index].qualifier == qual)) {
            /* selected range type or collecting all range types */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_range_info_from_qual(unitData,
                                                                        unitData->devInfo->ranges[index].qualifier,
                                                                        &qualFlags,
                                                                        NULL,
                                                                        &offset,
                                                                        &rangeCount));
            limit = offset + rangeCount; /* will use < for checking */
            result = _bcm_dpp_field_entry_qualifier_general_get_int(unitData,
                                                                    entry,
                                                                    unitData->devInfo->ranges[index].qualifier,
                                                                    1 /* buffers */,
                                                                    &edata,
                                                                    &emask);
            if ((BCM_FIELD_ENTRY_INVALID == qual) &&
                ((BCM_E_NOT_FOUND == result) || (BCM_E_CONFIG == result))) {
                /*
                 *  If scanning all types, 'not found' for a particular type
                 *  seems safe enough to ignore; we will return it later if
                 *  there are no ranges at all.
                 *
                 *  If scanning all types, it is likely that an entry's group
                 *  would include only one type (or none!) and so the
                 *  BCM_E_CONFIG that comes from trying to get a qualifier that
                 *  is not in the entry's group's QSET can be ignored.  We will
                 *  return BCM_E_NOT_FOUND later if there are no ranges found.
                 *
                 *  Even so, we do not want to parse bogus data, so skip the
                 *  entire parsing stage if we have nothing for this qualifier.
                 */
                result = BCM_E_NONE;
                continue;
            } else {
                /* Other error conditions warrant error exit. */
                BCMDNX_IF_ERR_EXIT(result);
            }
            if (qualFlags & _BCM_DPP_QUAL_RANGE_INDEX) {
                /*
                 *  Range is index; there is no invert support here.  Bias the
                 *  data value appropriately to know which range is used.
                 */
                if (!COMPILER_64_IS_ZERO(emask)) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d range %d included"
                                          " for entry %d offset %d\n"),
                               unit,
                               COMPILER_64_LO(edata) + offset,
                               entry,
                               total));
                    if (total < max_count) {
                        range[total] = COMPILER_64_LO(edata) + offset;
                        invert[total] = FALSE;
                        (*count)++;
                    }
                    total++;
                }
            } else if (qualFlags & _BCM_DPP_QUAL_RANGE_BITMAP) {
                /*
                 *  Range is bitmap; scan range for mask bits that are TRUE and
                 *  then see whether range is inverted or not by whether the
                 *  corresponding data bit is zero or one.
                 */
                COMPILER_64_SET(mask, 0, 1);
                for (;
                     offset < limit;
                     offset++) {
                    temp = mask;
                    COMPILER_64_AND(temp, emask);
                    if (!COMPILER_64_IS_ZERO(temp)) {
                        LOG_DEBUG(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unit %d range %d included"
                                              " for entry %d offset %d\n"),
                                   unit,
                                   offset,
                                   entry,
                                   total));
                        /* this range is significant to this entry */
                        if (total < max_count) {
                            temp = mask;
                            COMPILER_64_AND(temp, edata);
                            range[total] = offset;
                            if (!COMPILER_64_IS_ZERO(temp)) {
                                /* this range is not inverted */
                                invert[total] = FALSE;
                            } else {
                                /* this range is inverted */
                                invert[total] = TRUE;
                            }
                            (*count)++;
                        } /* if (total < maxCount) */
                        total++;
                    } /* if (temp) */
                    COMPILER_64_SHL(mask, 1);
                } /* for (all ranges of this type) */
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d does not have"
                                                   " valid format information"
                                                   " for range type %s (%d)"),
                                  unit,
                                  _bcm_dpp_field_qual_name[unitData->devInfo->ranges[index].qualifier],
                                  unitData->devInfo->ranges[index].qualifier));
            }
        } /* if (selected range type or using all range types) */
    } /* for (all range types as long as no errors) */
    if ((BCM_E_NONE == result) && (0 == total)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not checking"
                                           " any ranges"),
                          unit,
                          entry));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  Functions and data shared with other field modules
 */

int
_bcm_dpp_field_range_reset(int unit,
                           uint32 handle,
                           const _bcm_dpp_field_device_info_t *devInfo)
{
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    uint32 index;
    uint32 offset;
    uint32 sandResult;
    uint32 success;
    BCMDNX_INIT_FUNC_DEFS;

    for (index = 0;
         devInfo->ranges[index].qualifier < bcmFieldQualifyCount;
         index++) {
        for (offset = 0; offset < devInfo->ranges[index].count; offset++) {
            SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
            SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
            control_ndx.val_ndx = offset;
            switch (devInfo->ranges[index].qualifier) {
            case bcmFieldQualifyL4PortRangeCheck:
                control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE;
                control_info.val[0] = 0x0000;
                control_info.val[1] = 0xFFFF;
                control_info.val[2] = 0x0000;
                control_info.val[3] = 0xFFFF;
                break;
            case bcmFieldQualifyPacketLengthRangeCheck:
                control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE;
                control_info.val[0] = 0x01;
                control_info.val[1] = 0x7F;
                break;
            case bcmFieldQualifyVPortRangeCheck:
            {
                uint32 minimum, maximum ;

                control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_OUT_LIF_RANGE ;
                minimum = 1 ;
                /*
                 * The maximal number of outLifs varies from chip to chip.
                 * This is taken care of by the autocoder/DNXDefines feature.
                 * However, since we separate the families starting from Jericho 2,
                 * We have to use 'ifdef'
                 */
#if defined(BCM_DPP_SUPPORT)
                if (SOC_IS_DPP(unit))
                {
                    maximum = SOC_DPP_DEFS_GET(unit, nof_out_lifs) - 1 ;
                }
                else
#endif
                {
                    maximum = 0xFF ;
                }
                /*
                 * These are the min,max (val[0],val[1]) of this range.
                 * There are three ranges and they are all initialized, below, the same.
                 * The maximum is set as per the range of values for register IHB_OUT_LIF_RANGE
                 */
                control_info.val[0] = minimum ;
                control_info.val[1] = maximum ;
                break ;
            }
            default: 
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                 (_BSL_BCM_MSG("don't know how to clear range"
                                           " for type %s (%d) at offset %d"
                                           " in range table"),
                                  _bcm_dpp_field_qual_name[devInfo->ranges[index].qualifier],
                                  devInfo->ranges[index].qualifier,
                                  index));
            }
            sandResult = soc_ppd_fp_control_set(handle,
                                            SOC_CORE_INVALID,
                                            &control_ndx,
                                            &control_info,
                                            &success);
            BCMDNX_IF_ERR_EXIT(handle_sand_result(sandResult));
            BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(success));
        } /* for (all ranges of this type) */
    } /* for (all range types) */
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_range_init(bcm_dpp_field_info_OLD_t  *unitData)
{
    /* nothing to do at this point */
    return BCM_E_NONE;
}

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_range_dump(bcm_dpp_field_info_OLD_t  *unitData,
                            const char *prefix)
{
    bcm_field_range_t range;
    unsigned int count = 0;
    int index, actual, offset;
    uint32 flags[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    uint32 rangeMin[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    uint32 rangeMax[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    int result;
    int auxRes = BCM_E_NONE;
	int unit = unitData->unit;

    for (index = 0;
         unitData->devInfo->ranges[index].qualifier < bcmFieldQualifyCount;
         index++) {
        for (range = unitData->devInfo->ranges[index].rangeBase;
             range < unitData->devInfo->ranges[index].rangeBase + unitData->devInfo->ranges[index].count;
             range++) {
            result = _bcm_dpp_field_range_get(unitData,
                                              range,
                                              _BCM_DPP_FIELD_RANGE_CHAIN_MAX,
                                              &(flags[0]),
                                              &(rangeMin[0]),
                                              &(rangeMax[0]),
                                              &actual);
            if (BCM_E_NONE == result) {
                LOG_CLI((BSL_META_U(unit,
                                    "%sRange %d (%s:%d):\n"),
                         prefix,
                         range,
                         _bcm_dpp_field_qual_name[unitData->devInfo->ranges[index].qualifier],
                         unitData->devInfo->ranges[index].qualifier));
                for (offset = 0; offset < actual; offset++) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s  flags %08X : %12d min, %12d max\n"),
                             prefix,
                             flags[offset],
                             rangeMin[offset],
                             rangeMax[offset]));
                }
                if (0 == actual) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s  (no limits on range values)\n"),
                             prefix));
                }
                count++;
            } else if (BCM_E_NOT_FOUND == result) {
                /* just skip ones that are not there */
                result = BCM_E_NONE;
            }
            if (BCM_E_NONE != result) {
                /* propagate the error */
                auxRes = result;
            }
        } /* for (all ranges of this type) */
    } /* for (all types of ranges) */

    if ((BCM_E_NONE == auxRes) && (0 == count)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s(no ranges in this unit)\n"), prefix));
    }
    return auxRes;
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_entry_range_dump(bcm_dpp_field_info_OLD_t  *unitData,
                                 const _bcm_dpp_field_qual_t *qualArray,
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                const SOC_PPC_FP_QUAL_VAL *qualHwData,
#endif
                                const char *prefix)
{
    unsigned int index;
    unsigned int offset;
    unsigned int limit;
    unsigned int count;
    bcm_field_qualify_t qual;
    uint64 *data = NULL;
    uint64 *mask = NULL;
    uint64 edata;
    uint64 emask;
    uint64 overlay;
    uint64 maskbit;
    uint64 databit;
    int result = BCM_E_NONE;
    int found;
    unsigned int rangeType;
    unsigned int entryType;
    const int32 *mapData;
    int unit;
    int32 *qualMap ;
    int32 qualMaps_handle ;

    unit = unitData->unit;

    data = sal_alloc(sizeof(uint64)*unitData->devInfo->qualChain, "wb _bcm_dpp_field_entry_range_dump");
    mask = sal_alloc(sizeof(uint64)*unitData->devInfo->qualChain, "wb _bcm_dpp_field_entry_range_dump");

    if ((data != NULL) && (mask != NULL)) {
    for (rangeType = 0;
         bcmFieldQualifyCount > unitData->devInfo->ranges[rangeType].qualifier;
         rangeType++) {
        qual = unitData->devInfo->ranges[rangeType].qualifier;
        for (entryType = 0, found = FALSE;
             (entryType < unitData->devInfo->mappings) && (!found);
             entryType++) {
            result = FIELD_ACCESS_QUALMAPS.get(unit, qual, &qualMaps_handle) ;
            if (result != BCM_E_NONE) {
                goto exit ;
            }
            SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
            mapData = &(qualMap[(entryType *
                                        unitData->devInfo->qualChain) +
                                        _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS]);
            if (!_BCM_DPP_FIELD_PPD_QUAL_VALID(*mapData)) {
                /* this mapping is not included for this qualifier */
                continue;
            }
            /* mapData points to this mapping's PPD qualifiers for this qual */
            for (offset = 0;
                 offset < unitData->devInfo->qualChain;
                 offset++) {
                COMPILER_64_ZERO(data[offset]);
                COMPILER_64_ZERO(mask[offset]);
                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(mapData[offset])) {
                    for (index = 0; index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; index++) {
                        if (qualArray[index].hwType == mapData[offset]) {
                            /* found this qualifier */
                            data[offset] = qualArray[index].qualData;
                            mask[offset] = qualArray[index].qualMask;
                            found = TRUE;
                            break;
                        }
                    } /* for (all qualifier slots) */
                } /* if (SOC_PPC_NOF_FP_QUAL_TYPES > qual[offset]) */
            } /* for (full length of longest range chain) */
            if (!found) {
                /* does not have this mapping of this qualifier; skip it */
                continue;
            }
            /* now have data[0..n] and mask[0..n] filled in with HW quals */
            COMPILER_64_ZERO(edata);
            COMPILER_64_ZERO(emask);
            for (index = 0, offset = 0;
                 index < unitData->devInfo->qualChain;
                 index++) {
                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(mapData[index])) {
                    /* this function should never fail in this context */
                    result = _bcm_dpp_ppd_qual_bits(unit,
                                                    unitData->devInfo->stMapInfo[entryType].stmStage,
                                                    mapData[index],
                                                    &limit,
                                                    NULL,
                                                    NULL);
                    if (BCM_E_NONE != result) {
                        LOG_ERROR(BSL_LS_BCM_FP,
                                  (BSL_META_U(unit,
                                              "unable to obtain PPD qualifier"
                                              " %s (%d) size for unit %d"
                                              " stage %u: %d (%s)\n"),
                                   SOC_PPC_FP_QUAL_TYPE_to_string(mapData[index]),
                                   mapData[index],
                                   unit,
                                   unitData->devInfo->stMapInfo[entryType].stmStage,
                                   result,
                                   _SHR_ERRMSG(result)));
                        sal_free(data);
                        sal_free(mask);
                        return result;
                    }
                    COMPILER_64_SET(maskbit, ~0, ~0);
                    COMPILER_64_SHL(maskbit, limit);
                    COMPILER_64_NOT(maskbit);
                    databit = maskbit;
                    COMPILER_64_AND(databit, data[index]);
                    COMPILER_64_AND(maskbit, mask[index]);
                    COMPILER_64_SHL(databit, offset);
                    COMPILER_64_SHL(maskbit, offset);
                    offset += limit;
                    COMPILER_64_OR(edata, databit);
                    COMPILER_64_OR(emask, maskbit);
                } /* if (PPD qualifier is valid) */
            } /* for (all PPD qualifiers in chain) */
            /* BCM layer information in edata,emask &c; display it */
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
            LOG_CLI((BSL_META_U(unit,
                                "%s    %s (%d) -> %s (%d)%s"),
                     prefix,
                     _bcm_dpp_field_qual_name[qual],
                     qual,
                     SOC_PPC_FP_QUAL_TYPE_to_string(mapData[0]),
                     mapData[0],
                     _BCM_DPP_FIELD_PPD_QUAL_VALID(mapData[1])?"...":""));
#else /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
            LOG_CLI((BSL_META_U(unit,
                                "%s    %s (%d)"),
                     prefix,
                     _bcm_dpp_field_qual_name[qual],
                     qual));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
            offset = unitData->devInfo->ranges[rangeType].rangeBase;
            limit = offset + unitData->devInfo->ranges[rangeType].count;
            if (qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS] &
                _BCM_DPP_QUAL_RANGE_BITMAP) {
                COMPILER_64_SET(overlay, 0, 1);
                for (count = 0;
                     offset < limit;
                     offset++) {
                    maskbit = overlay;
                    COMPILER_64_AND(maskbit, emask);
                    if (!COMPILER_64_IS_ZERO(maskbit)) {
                        databit = overlay;
                        COMPILER_64_AND(databit, edata);
                        if (0 == (count & 7)) {
                            LOG_CLI((BSL_META_U(unit,
                                                "\n%s      %s%-3d"),
                                     prefix,
                                     (!COMPILER_64_IS_ZERO(databit))?"+":"-",
                                     offset));
                             
                        } else {
                            LOG_CLI((BSL_META_U(unit,
                                                " %s%-3d"),
                                     (!COMPILER_64_IS_ZERO(databit))?"+":"-",
                                     offset));
                        }
                        count++;
                    } /* if (maskbit) */
                    COMPILER_64_SHL(overlay, 1);
                } /* for (all valid bits in the qualifier) */
                if (count) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n%s      (none)\n"), prefix));
                }
            } else if (qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS] &
                       _BCM_DPP_QUAL_RANGE_INDEX) {
                
                if (COMPILER_64_LO(emask)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n%s      +%-3d\n"),
                             prefix,
                             COMPILER_64_LO(edata) + offset));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n%s      (none)\n"), prefix));
                }
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "\n%s      (unknown format)\n"), prefix));
            }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD || _BCM_DPP_FIELD_DUMP_VERIFY_PPD
            for (index = 0; index < unitData->devInfo->qualChain; index++) {
                if (!_BCM_DPP_FIELD_PPD_QUAL_VALID(mapData[index])) {
                    /* no qualifier here; skip this check */
                    continue;
                }
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                if (qualHwData) {
                    for (offset = 0;
                         offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX;
                         offset++) {
                        if (qualHwData[offset].type == mapData[index]) {
                            /* found the hardware qual matching this one */
                            break;
                        }
                    }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      %08X%08X/%08X%08X (expected)\n"),
                             prefix,
                             COMPILER_64_HI(data[index]),
                             COMPILER_64_LO(data[index]),
                             COMPILER_64_HI(mask[index]),
                             COMPILER_64_LO(mask[index])));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                    if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
                        LOG_CLI((BSL_META_U(unit,
                                            "%s      %08X%08X/%08X%08X (actual)\n"),
                                 prefix,
                                 qualHwData[offset].val.arr[1],
                                 qualHwData[offset].val.arr[0],
                                 qualHwData[offset].is_valid.arr[1],
                                 qualHwData[offset].is_valid.arr[0]));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                        if ((COMPILER_64_HI(data[index]) !=
                             qualHwData[offset].val.arr[1]) ||
                            (COMPILER_64_LO(data[index]) !=
                             qualHwData[offset].val.arr[0]) ||
                            (COMPILER_64_HI(mask[index]) !=
                             qualHwData[offset].is_valid.arr[1]) ||
                            (COMPILER_64_LO(mask[index]) !=
                             qualHwData[offset].is_valid.arr[0])) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s      (BCM %d and PPD %d data"
                                     " mismatch)\n"),
                                     prefix,
                                     qual,
                                     qualHwData[offset].type));
#if _BCM_DPP_FIELD_DUMP_VERIFY_ERROR
                            retVal = BCM_E_INTERNAL;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_ERROR */
                        }
                    } else { /* if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) */
                        LOG_CLI((BSL_META_U(unit,
                                            "%s      (unable to find hardware value)\n"),
                                 prefix));
                    } /* if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) */
                } /* if (qualHwData) */
#else /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
                LOG_CLI((BSL_META_U(unit,
                                    "%s      %08X%08X/%08X%08X\n"),
                         prefix,
                         COMPILER_64_HI(data[index]),
                         COMPILER_64_LO(data[index]),
                         COMPILER_64_HI(mask[index]),
                         COMPILER_64_LO(mask[index])));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
            } /* for (index = 0; index < _BCM_DPP_MAX_RANGE_CHAIN; index++) */
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD || _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
        } /* for (all entry types) */
    } /* for (all range qualifier types) */
    }
    if (data) {
        sal_free(data);
    }
    if (mask) {
        sal_free(mask);
    }
exit:
    return result;
}
#endif /* def BROADCOM_DEBUG */


/******************************************************************************
 *
 *  Functions and data exported to API users
 */

/*
 *   Function
 *      bcm_petra_field_range_create
 *   Purpose
 *      Create a descriptor for a range of L4 (specifically, TCP or UDP over
 *      IP) ports.
 *   Parameters
 *      (in) int unit = the unit number
 *      (out) bcm_field_range_t *range = where to put the assigned range ID
 *      (in) uint32 flags = flags for the range
 *      (in) bcm_l4_port_t min = low port number for the range
 *      (in) bcm_l4_port_t max = high port number for the range
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 */
int
bcm_petra_field_range_create(int unit,
                              bcm_field_range_t *range,
                              uint32 flags,
                              bcm_l4_port_t min,
                              bcm_l4_port_t max)
{
    _DPP_FIELD_COMMON_LOCALS;
    bcm_field_range_t rangeId = -1;
    bcm_field_range_t minId;
    bcm_field_qualify_t qual;
    _bcm_dpp_field_range_idx_t idCount;
    _bcm_dpp_field_range_idx_t index;
    int exists;
    uint32 emin = min;
    uint32 emax = max;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,*,%08X,%d,%d) enter\n"),
               unit,
               flags,
               min,
               max));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if (!range) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT arg must not be NULL")));
    }

    if(flags & BCM_FIELD_RANGE_EXTERNAL) {
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        int res;
        res = arad_kbp_acl_range_create(unit, flags, range, emin, emax, FALSE, &result);
        if(res != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("arad_kbp_acl_range_create failed")));
        }
        BCMDNX_IF_ERR_EXIT(result);
        BCM_EXIT;
#else
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit,"Flag BCM_FIELD_RANGE_EXTERNAL is not supported without KBP libraries compiled\n")));

        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
#endif
    }

    _DPP_FIELD_UNIT_LOCK(unitData);

    /* derive range ID space and qualifier by looking at the flags */
    result = _bcm_dpp_field_range_info_from_flags(unitData,
                                                  flags,
                                                  &qual,
                                                  NULL,
                                                  NULL,
                                                  &minId,
                                                  &idCount);
    /* scan through the ID space looking for a free one */
    for (index = 0;
         (BCM_E_NONE == result) && (index < idCount);
         index++) {
        result = _bcm_dpp_field_range_info_from_id(unitData,
                                                   minId + index,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &exists);
        if ((BCM_E_NONE == result) && (!exists)) {
            /* got one that does not yet exist */
            break;
        }
    }
    if (BCM_E_NONE == result) {
        if (index >= idCount) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d has no available range of type %s"
                                  " (%d) for flags %08X\n"),
                       unit,
                       _bcm_dpp_field_qual_name[qual],
                       qual,
                       flags));
            result = BCM_E_RESOURCE;
        } else {
            rangeId = minId + index;
        }
    }
    /* if we found one, then set it and fill in the ID for the caller */
    if (BCM_E_NONE == result) {
        result = _bcm_dpp_field_range_set_id(unitData,
                                             FALSE,
                                             rangeId,
                                             1 /* count */,
                                             &flags,
                                             &emin,
                                             &emax);
        if (BCM_E_NONE == result) {
            *range = rangeId;
        }
    }

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,&(%d),%08X,%d,%d) return %d (%s)\n"),
               unit,
               *range,
               flags,
               min,
               max,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);

exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_range_create_id
 *   Purpose
 *      Create a descriptor for a range of L4 (specifically, TCP or UDP over
 *      IP) ports.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_range_t range = the range ID to use
 *      (in) uint32 flags = flags for the range
 *      (in) bcm_l4_port_t min = low port number for the range
 *      (in) bcm_l4_port_t max = high port number for the range
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Range ID is always nonzero.
 */
int
bcm_petra_field_range_create_id(int unit,
                                 bcm_field_range_t range,
                                 uint32 flags,
                                 bcm_l4_port_t min,
                                 bcm_l4_port_t max)
{
    _DPP_FIELD_COMMON_LOCALS;
    uint32 emin = min;
    uint32 emax = max;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%08X,%d,%d) enter\n"),
               unit,
               range,
               flags,
               min,
               max));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    if(flags & BCM_FIELD_RANGE_EXTERNAL) {
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        int res;
        res = arad_kbp_acl_range_create(unit, flags, &range, emin, emax, TRUE, &result);
        if(res != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("arad_kbp_acl_range_create failed")));
        }
        BCMDNX_IF_ERR_EXIT(result);
        BCM_EXIT;
#else
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit,"Flag BCM_FIELD_RANGE_EXTERNAL is not supported without KBP libraries compiled\n")));

        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
#endif
    }

    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_dpp_field_range_set_id(unitData,
                                         FALSE,
                                         range,
                                         1 /* count */,
                                         &flags,
                                         &emin,
                                         &emax);

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%08X,%d,%d) return %d (%s)\n"),
               unit,
               range,
               flags,
               min,
               max,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_range_get
 *   Purpose
 *      Create a descriptor for a range of L4 (specifically, TCP or UDP over
 *      IP) ports.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_range_t range = the range ID to use
 *      (in) uint32 *flags = where to put the flags for the range
 *      (in) bcm_l4_port_t *min = where to put range's low port number
 *      (in) bcm_l4_port_t *max = where to put range's high port number
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Range ID is always nonzero.
 */
int
bcm_petra_field_range_get(int unit,
                           bcm_field_range_t range,
                           uint32 *flags,
                           bcm_l4_port_t *min,
                           bcm_l4_port_t *max)
{
    uint32 eflags[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    uint32 rangeMin[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    uint32 rangeMax[_BCM_DPP_FIELD_RANGE_CHAIN_MAX];
    int actual;
    _DPP_FIELD_COMMON_LOCALS;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,*,*,*) enter\n"),
               unit,
               range));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if ((!flags) || (!min) || (!max)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT arg must not be NULL")));
    }

    if(*flags & BCM_FIELD_RANGE_EXTERNAL) {
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        int res;
        res = arad_kbp_acl_range_get_by_id(unit, flags, range, (uint32*)min, (uint32*)max, &result);
        if(res != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("arad_kbp_acl_range_get_by_id failed")));
        }
        BCMDNX_IF_ERR_EXIT(result);
        BCM_EXIT;
#else
        LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit,"Flag BCM_FIELD_RANGE_EXTERNAL is not supported without KBP libraries compiled\n")));

        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
#endif
    }

    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_dpp_field_range_get(unitData,
                                      range,
                                      _BCM_DPP_FIELD_RANGE_CHAIN_MAX,
                                      &(eflags[0]),
                                      &(rangeMin[0]),
                                      &(rangeMax[0]),
                                      &actual);
    if (BCM_E_NONE == result) {
        if (0 == actual) {
            /* zero ranges; should not occur */
            result = BCM_E_INTERNAL;
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d range %d inconsistent setting;"
                                  " no error finding range data but there is"
                                  " no defined limit for the range\n"),
                       unit,
                       range));
        } else if (1 == actual) {
            /* one range defined; can return this safely */
            *flags = eflags[0];
            *min = rangeMin[0];
            *max = rangeMax[0];
        } else {
            result = BCM_E_CONFIG;
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d range %d setting is too complex"
                                  " to retrieve with this function\n"),
                       unit,
                       range));
        }
    } /* if (BCM_E_NONE == result) */

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,&(%08X),&(%d),&(%d)) return %d (%s)\n"),
               unit,
               range,
               *flags,
               *min,
               *max,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_range_destroy
 *   Purpose
 *      Destroy a descriptor for a range of L4 (specifically, TCP or UDP over
 *      IP) ports.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_range_t range = the range ID to destroy
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Range ID is always nonzero.
 */
int
bcm_petra_field_range_destroy(int unit,
                               bcm_field_range_t range)
{
    _DPP_FIELD_COMMON_LOCALS;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d) enter\n"), unit, range));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);

    
    /* clear the range in hardware */
    result = _bcm_dpp_field_range_set_id(unitData,
                                         FALSE,
                                         range,
                                         -1 /* count : neg => destroy */,
                                         NULL,
                                         NULL,
                                         NULL);

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d) return %d (%s)\n"),
               unit,
               range,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_RangeCheck
 *   Purpose
 *      Set expected TCP/UDP port range for this entry
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t entry = the entry ID
 *      (in) bcm_field_range_t range = the range ID
 *      (in) int invert = whether the range match is to be inverted
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      This one, in keeping with behavior of other devices, allows all types
 *      of ranges to be set, as long as the qualifier is valid, supported, and
 *      in the entry's group's QSET.
 */
int
bcm_petra_field_qualify_RangeCheck(int unit,
                                   bcm_field_entry_t entry,
                                   bcm_field_range_t range,
                                   int invert)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) enter\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL"));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck(unitData,
                                               bcmFieldQualifyCount,
                                               entry,
                                               range,
                                               invert);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) return %d (%s)\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL",
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_RangeCheck_get
 *   Purpose
 *      Get match criteria for bcmFieildQualifyRangeCheck qualifier from the
 *      field entry.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t = the entry ID
 *      (in) int max_count = max ranges to return
 *      (out) bcm_field_range_t *range = ptr to array[max_count] of range IDs
 *      (out) int *invert = pointer to array[max_count] of invert flags
 *      (out) int *count = pointer to where to put the actual count
 *   Returns
 *      bcm_error_t cast as int
 *         BCM_E_NONE for success
 *         BCM_E_* otherwise as appropriate
 *   Notes
 *      Provide max_count of zero to get the number of elements required for
 *      the specific range type.
 *
 *      In some places, similar 'get' APIs accept bogus max_count to indicate
 *      that the caller wants the size of the needed array; in other placed,
 *      there is no way to the size of the needed array except to provide an
 *      array that is at least one element too big.  Some APIs follow the
 *      convention that the actual count value is populated with the actual
 *      in-use count when the max_count value is zero (in this case the list
 *      pointer can be NULL).
 *
 *      This may write to the provided buffers even if it returns an error.
 */
int
bcm_petra_field_qualify_RangeCheck_get(int unit,
                                       bcm_field_entry_t entry,
                                       int max_count,
                                       bcm_field_range_t *range,
                                       int *invert,
                                       int *count)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,*) enter\n"),
               unit,
               entry,
               max_count));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck_get(unitData,
    											   BCM_FIELD_ENTRY_INVALID,
                                                   entry,
                                                   max_count,
                                                   range,
                                                   invert,
                                                   count);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,&(%d)) return %d (%s)\n"),
               unit,
               entry,
               max_count,
               *count,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_range_group_create
 *   Purpose
 *      Create a descriptor for a range of L4 (specifically, TCP or UDP over
 *      IP) ports.
 *   Parameters
 *      (in) int unit = the unit number
 *      (out) bcm_field_range_t *range = where to put the assigned range ID
 *      (in) uint32 flags = flags for the range
 *      (in) bcm_l4_port_t min = low port number for the range
 *      (in) bcm_l4_port_t max = high port number for the range
 *      (in) bcm_if_group_t group = L3 interface group for this range
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Range ID is always nonzero.
 */
int
bcm_petra_field_range_group_create(int unit,
                                    bcm_field_range_t *range,
                                    uint32 flags,
                                    bcm_l4_port_t min,
                                    bcm_l4_port_t max,
                                    bcm_if_group_t group)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_range_group_create_id
 *   Purpose
 *      Create a descriptor for a range of L4 (specifically, TCP or UDP over
 *      IP) ports.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_range_t range = the range ID to use
 *      (in) uint32 flags = flags for the range
 *      (in) bcm_l4_port_t min = low port number for the range
 *      (in) bcm_l4_port_t max = high port number for the range
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Range ID is always nonzero.
 */
int
bcm_petra_field_range_group_create_id(int unit,
                                       bcm_field_range_t range,
                                       uint32 flags,
                                       bcm_l4_port_t min,
                                       bcm_l4_port_t max,
                                       bcm_if_group_t group)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_L4PortRangeCheck
 *   Purpose
 *      Set expected TCP/UDP port range for this entry
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t entry = the entry ID
 *      (in) bcm_field_range_t range = the range ID
 *      (in) int invert = whether the range match is to be inverted
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Specifically insists the range be L4Port range.
 */
int
bcm_petra_field_qualify_L4PortRangeCheck(int unit,
                                         bcm_field_entry_t entry,
                                         bcm_field_range_t range,
                                         int invert)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) enter\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL"));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck(unitData,
                                               bcmFieldQualifyL4PortRangeCheck,
                                               entry,
                                               range,
                                               invert);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) return %d (%s)\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL",
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_L4PortRangeCheck_get
 *   Purpose
 *      Get match criteria for bcmFieildQualifyL4PortRangeCheck qualifier from the
 *      field entry.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t = the entry ID
 *      (in) int max_count = max ranges to return
 *      (out) bcm_field_range_t *range = ptr to array[max_count] of range IDs
 *      (out) int *invert = pointer to array[max_count] of invert flags
 *      (out) int *count = pointer to where to put the actual count
 *   Returns
 *      bcm_error_t cast as int
 *         BCM_E_NONE for success
 *         BCM_E_* otherwise as appropriate
 *   Notes
 *      In some places, similar 'get' APIs accept bogus max_count to indicate
 *      that the caller wants the size of the needed array; in other placed,
 *      there is no way to the size of the needed array except to provide an
 *      array that is at least one element too big.  Some APIs follow the
 *      convention that the actual count value is populated with the actual
 *      in-use count when the max_count value is zero (in this case the list
 *      pointer can be NULL).
 */
int
bcm_petra_field_qualify_L4PortRangeCheck_get(int unit,
                                             bcm_field_entry_t entry,
                                             int max_count,
                                             bcm_field_range_t *range,
                                             int *invert,
                                             int *count)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,*) enter\n"),
               unit,
               entry,
               max_count));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck_get(unitData,
                                                   bcmFieldQualifyL4PortRangeCheck,
                                                   entry,
                                                   max_count,
                                                   range,
                                                   invert,
                                                   count);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,&(%d)) return %d (%s)\n"),
               unit,
               entry,
               max_count,
               *count,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_PacketLengthRangeCheck
 *   Purpose
 *      Set expected TCP/UDP port range for this entry
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t entry = the entry ID
 *      (in) bcm_field_range_t range = the range ID
 *      (in) int invert = whether the range match is to be inverted
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Specifically insists the range be of type PacketLength.
 */
int
bcm_petra_field_qualify_PacketLengthRangeCheck(int unit,
                                               bcm_field_entry_t entry,
                                               bcm_field_range_t range,
                                               int invert)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) enter\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL"));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck(unitData,
                                               bcmFieldQualifyPacketLengthRangeCheck,
                                               entry,
                                               range,
                                               invert);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) return %d (%s)\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL",
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_PacketLengthRangeCheck_get
 *   Purpose
 *      Get match criteria for bcmFieildQualifyPacketLengthRangeCheck qualifier from the
 *      field entry.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t = the entry ID
 *      (in) int max_count = max ranges to return
 *      (out) bcm_field_range_t *range = ptr to array[max_count] of range IDs
 *      (out) int *invert = pointer to array[max_count] of invert flags
 *      (out) int *count = pointer to where to put the actual count
 *   Returns
 *      bcm_error_t cast as int
 *         BCM_E_NONE for success
 *         BCM_E_* otherwise as appropriate
 *   Notes
 *      In some places, similar 'get' APIs accept bogus max_count to indicate
 *      that the caller wants the size of the needed array; in other placed,
 *      there is no way to the size of the needed array except to provide an
 *      array that is at least one element too big.  Some APIs follow the
 *      convention that the actual count value is populated with the actual
 *      in-use count when the max_count value is zero (in this case the list
 *      pointer can be NULL).
 */
int
bcm_petra_field_qualify_PacketLengthRangeCheck_get(int unit,
                                                   bcm_field_entry_t entry,
                                                   int max_count,
                                                   bcm_field_range_t *range,
                                                   int *invert,
                                                   int *count)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,*) enter\n"),
               unit,
               entry,
               max_count));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck_get(unitData,
                                                   bcmFieldQualifyPacketLengthRangeCheck,
                                                   entry,
                                                   max_count,
                                                   range,
                                                   invert,
                                                   count);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,&(%d)) return %d (%s)\n"),
               unit,
               entry,
               max_count,
               *count,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_VPortRangeCheck
 *   Purpose
 *      Set expected TCP/UDP port range for this entry
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t entry = the entry ID
 *      (in) bcm_field_range_t range = the range ID
 *      (in) int invert = whether the range match is to be inverted
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Specifically insists the range be of type VPort.
 */
int
bcm_petra_field_qualify_VPortRangeCheck(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_range_t range,
                                        int invert)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) enter\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL"));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck(unitData,
                                               bcmFieldQualifyVPortRangeCheck,
                                               entry,
                                               range,
                                               invert);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,%s) return %d (%s)\n"),
               unit,
               entry,
               range,
               invert?"INVERT":"NORMAL",
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
 exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_qualify_VPortRangeCheck_get
 *   Purpose
 *      Get match criteria for bcmFieildQualifyVPortRangeCheck qualifier from the
 *      field entry.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_entry_t = the entry ID
 *      (in) int max_count = max ranges to return
 *      (out) bcm_field_range_t *range = ptr to array[max_count] of range IDs
 *      (out) int *invert = pointer to array[max_count] of invert flags
 *      (out) int *count = pointer to where to put the actual count
 *   Returns
 *      bcm_error_t cast as int
 *         BCM_E_NONE for success
 *         BCM_E_* otherwise as appropriate
 *   Notes
 *      In some places, similar 'get' APIs accept bogus max_count to indicate
 *      that the caller wants the size of the needed array; in other placed,
 *      there is no way to the size of the needed array except to provide an
 *      array that is at least one element too big.  Some APIs follow the
 *      convention that the actual count value is populated with the actual
 *      in-use count when the max_count value is zero (in this case the list
 *      pointer can be NULL).
 */
int
bcm_petra_field_qualify_VPortRangeCheck_get(int unit,
                                                   bcm_field_entry_t entry,
                                                   int max_count,
                                                   bcm_field_range_t *range,
                                                   int *invert,
                                                   int *count)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,*) enter\n"),
               unit,
               entry,
               max_count));
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_qualify_RangeCheck_get(unitData,
                                                   bcmFieldQualifyVPortRangeCheck,
                                                   entry,
                                                   max_count,
                                                   range,
                                                   invert,
                                                   count);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d,*,*,&(%d)) return %d (%s)\n"),
               unit,
               entry,
               max_count,
               *count,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_range_multi_create
 *   Purpose
 *      Create a range descriptor, supporting multiple sub-ranges.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in/out) bcm_field_range_t *range = where to put the assigned range ID
 *      (in) uint32 flags = flags for the create function
 *      (in) int count = number of range descriptions
 *      (in) uint32 *rangeFlags = pointer to array of range flags
 *      (in) uint32 *rangeMin = pointer to array of low values
 *      (in) uint32 *rangeMax = pointer to array of high values
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Range ID is always nonzero.
 */
int
bcm_petra_field_range_multi_create(int unit,
                                   bcm_field_range_t *range,
                                   uint32 flags,
                                   int count,
                                   uint32 *rangeFlags,
                                   uint32 *rangeMin,
                                   uint32 *rangeMax)
{
    _DPP_FIELD_COMMON_LOCALS;
    bcm_field_range_t rangeId = ~0;
    bcm_field_range_t minId;
    bcm_field_qualify_t qual;
    _bcm_dpp_field_range_idx_t idCount;
    _bcm_dpp_field_range_idx_t index;
    int exists;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    if (!range) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory argument must not be NULL")));
    }
    if (0 > count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("count %d must not be negative"), count));
    }
    if (count && ((!rangeFlags) || (!rangeMin) || (!rangeMax))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory pointer args must not be NULL")));
    }
    if (flags & (~(BCM_FIELD_RANGE_MULTI_WITH_ID |
                   BCM_FIELD_RANGE_MULTI_REPLACE))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("flags contains unsupported bits %08X"),
                          flags & (~(BCM_FIELD_RANGE_MULTI_WITH_ID |
                                     BCM_FIELD_RANGE_MULTI_REPLACE))));
    }
    if (BCM_FIELD_RANGE_MULTI_REPLACE == (flags &
                                          (BCM_FIELD_RANGE_MULTI_WITH_ID |
                                           BCM_FIELD_RANGE_MULTI_REPLACE))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("must specify WITH_ID with REPLACE")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    if (flags & BCM_FIELD_RANGE_MULTI_WITH_ID) {
        /* caller wants to use a specific ID */
        rangeId = *range;
    } else { /* if (flags & BCM_FIELD_RANGE_MULTI_WITH_ID) */
        /* derive range ID space and qualifier by looking at the flags */
        result = _bcm_dpp_field_range_info_from_flags(unitData,
                                                      rangeFlags[0],
                                                      &qual,
                                                      NULL,
                                                      NULL,
                                                      &minId,
                                                      &idCount);
        /* scan through the ID space looking for a free one */
        for (index = 0;
             (BCM_E_NONE == result) && (index < idCount);
             index++) {
            result = _bcm_dpp_field_range_info_from_id(unitData,
                                                       minId + index,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       &exists);
            if ((BCM_E_NONE == result) && (!exists)) {
                /* got one that does not yet exist */
                break;
            }
        }
        if (BCM_E_NONE == result) {
            if (index >= idCount) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d has no available range of type"
                                      " %s (%d) for flags %08X\n"),
                           unit,
                           _bcm_dpp_field_qual_name[qual],
                           qual,
                           rangeFlags[0]));
                result = BCM_E_RESOURCE;
            } else {
                rangeId = minId + index;
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d allocating range %d\n"),
                           unit,
                           rangeId));
            }
        }
    } /* if (flags & BCM_FIELD_RANGE_MULTI_WITH_ID) */

    /* set up the range */
    if (BCM_E_NONE == result) {
        result = _bcm_dpp_field_range_set_id(unitData,
                                             flags & BCM_FIELD_RANGE_MULTI_REPLACE,
                                             rangeId,
                                             count,
                                             rangeFlags,
                                             rangeMin,
                                             rangeMax);
        if (BCM_E_NONE == result) {
            *range = rangeId;
        }
    } /* if (BCM_E_NONE == result) */

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_range_multi_get
 *   Purpose
 *      Read a range descriptor, supporting multiple sub-ranges.
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_range_t range = range ID to read
 *      (in) int max_count = number of range descriptions in arrays
 *      (out) uint32 *rangeFlags = pointer to array of range flags
 *      (out) uint32 *rangeMin = pointer to array of low values
 *      (out) uint32 *rangeMax = pointer to array of high values
 *      (out) int *actualCount = pointer to where to put actual count
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Range ID is always nonzero.
 */
int
bcm_petra_field_range_multi_get(int unit,
                                bcm_field_range_t range,
                                int maxCount,
                                uint32 *rangeFlags,
                                uint32 *rangeMin,
                                uint32 *rangeMax,
                                int *actualCount)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if (0 > maxCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("max_count %d must be non-negative"),
                          maxCount));
    }
    if ((0 < maxCount) && ((!rangeFlags) || (!rangeMin) || (!rangeMax))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("when max_count %d > 0, array arguments"
                                   " must not be NULL"),
                          maxCount));
    }
    if (!actualCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT arg actual_count must not"
                                   " be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_dpp_field_range_get(unitData,
                                      range,
                                      maxCount,
                                      rangeFlags,
                                      rangeMin,
                                      rangeMax,
                                      actualCount);

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

