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
 *     Preselector support
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
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#include <soc/dcmn/dcmn_wb.h>

/*
 *  For now, if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT is FALSE, disable warm boot
 *  support here.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#if !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#undef BCM_WARM_BOOT_SUPPORT
#endif  /* BCM_WARM_BOOT_SUPPORT */
#endif /* !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT */


/******************************************************************************
 *
 *  Local functions and data
 */

/******************************************************************************
 *
 *  Functions and data shared with other field modules
 */

int
_bcm_dpp_field_presel_info_get(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                               unsigned int qualLimit,
                               uint32 flags,
                               _bcm_dpp_field_stage_idx_t *stage,
                               _bcm_dpp_field_qual_t *bcmQuals)
{
    SOC_PPC_PMF_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    unsigned int index;
    unsigned int offset = 0;
    int pfg_ndx;
    _bcm_dpp_field_presel_idx_t preselLimit;
    bcm_field_stage_t bcm_stage;
    bcm_field_qualify_t ppdQual;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if (preselLimit <= presel) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %u invalid"),
                          unit,
                          presel));
    }

    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPC_PMF_PFG_INFO), "_bcm_dpp_field_presel_info_get.pfgInfo");
    if (pfgInfo == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    SOC_PPC_PMF_PFG_INFO_clear(pfgInfo);
    pfg_ndx = presel;
    pfgInfo->stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
    if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_STAGE) {  
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_index_to_pfg(unit, presel,&bcm_stage ,&pfg_ndx));
        /*convert bcm stage to SOC stage*/
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_ppd_stage_from_bcm_stage(unitData,bcm_stage,&(pfgInfo->stage)));
    } /* if (unitData->devInfo->stages > stage) */

    sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                    pfg_ndx,
                                                    pfgInfo);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                        (_BSL_BCM_MSG_NO_UNIT("unable to read unit %d presel %u"),
                         unit,
                         presel));
    /* ensure the buffer is clear first */
    if (bcmQuals) {
        for (index = 0; index < qualLimit; index++) {
            bcmQuals[index].hwType = BCM_FIELD_ENTRY_INVALID;
            bcmQuals[index].qualType = BCM_FIELD_ENTRY_INVALID;
            COMPILER_64_ZERO(bcmQuals[index].qualData);
            COMPILER_64_ZERO(bcmQuals[index].qualMask);
        }
    }
    /* Arad PFG support is qualifier based */
    if (bcmQuals) {
        if (pfgInfo->is_array_qualifier) {
            /* parse hardware state into working view */
            for (index = 0;
                 index < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX;
                 index++) {
                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(pfgInfo->qual_vals[index].type)) {
                    if (offset < qualLimit) {

                        bcmQuals[offset].hwType = pfgInfo->qual_vals[index].type;
                        COMPILER_64_SET(bcmQuals[offset].qualData, pfgInfo->qual_vals[index].val.arr[1],
                                                                     pfgInfo->qual_vals[index].val.arr[0]);
                        COMPILER_64_SET(bcmQuals[offset].qualMask, pfgInfo->qual_vals[index].is_valid.arr[1],
                                                                     pfgInfo->qual_vals[index].is_valid.arr[0]);
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.ppdQual.get(unit, pfgInfo->qual_vals[index].type, &ppdQual));
                        bcmQuals[offset].qualType = ppdQual;
                    } /* if (offset < qualLimit) */
                    offset++;
                } /* if (PPD qualifier is valid) */
            } /* for (all possible qualifiers) */
        } /* if (pfgInfo->is_array_qualifier) */
    } /* if (bcmQuals) */
    /* fill in the BCM layer stage index */
    if (stage) {
        for (index = 0; index < unitData->devInfo->stages; index++) {
            if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, index).hwStageId == pfgInfo->stage) {
                *stage = index;
                break;
            }
        } /* for (index = 0; index < unitData->devInfo->stages; index++) */
    } /* if (stage) */
    if (bcmQuals) {
        if (offset >= qualLimit) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %u has %u quals"
                                               " but was only able to collect %u"),
                              unit,
                              presel,
                              offset,
                              qualLimit));
        }
    } /* if (bcmQuals) */
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_info_set(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                               unsigned int qualLimit,
                               uint32 flags,
                               _bcm_dpp_field_stage_idx_t stage,
                               const _bcm_dpp_field_qual_t *bcmQuals,
                               _bcm_dpp_field_program_idx_t program_id)
{
    SOC_PPC_PMF_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    unsigned int index;
    int pfg_ndx;
    unsigned int offset;
    SOC_SAND_SUCCESS_FAILURE success;
    bcm_field_stage_t bcm_stage;
    _bcm_dpp_field_presel_idx_t preselLimit;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if (preselLimit <= presel) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %u invalid"),
                          unit,
                          presel));
    }
    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPC_PMF_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }

    /* DPP requires existing PFG (if in place) be overwritten first */
    SOC_PPC_PMF_PFG_INFO_clear(pfgInfo);

    pfgInfo->stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
    pfgInfo->is_staggered = (flags & _BCM_DPP_PRESEL_ALLOC_SECOND_PASS) ? TRUE : FALSE;
    pfg_ndx = presel;
    if (unitData->devInfo->stages > stage) {  /* use caller's provided stage ID */
        pfgInfo->stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).hwStageId;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_index_to_pfg(unit, presel,&bcm_stage ,&pfg_ndx));
    } /* if (unitData->devInfo->stages > stage) */
    success = SOC_SAND_SUCCESS;
    if (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE) == 0) {
        sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                        pfg_ndx,
                                        pfgInfo,
                                        &success);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                    (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                      " presel %u)"),
                     unit,
                     presel));
        BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                            (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                              " unsuccessful"),
                             unit,
                             presel));
    }
    

    /* if preselector was created with program ID update the program ID in the SW state*/
    if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_PROGRAM) {
        pfgInfo->program_id = program_id;
        pfgInfo->is_psl_with_program = TRUE;
        sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                        pfg_ndx,
                                        pfgInfo,
                                        &success);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                    (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                      " presel %u)"),
                     unit,
                     presel));
        BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                            (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                              " unsuccessful"),
                             unit,
                             presel));
    }

    if (qualLimit) {
        /* caller provided qualifier info so intends more than just destroy */
        SOC_PPC_PMF_PFG_INFO_clear(pfgInfo);
        if (unitData->devInfo->stages <= stage) {
            /* caller provided invalid stage; assume ingress */
            pfgInfo->stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
        } else {
            /* use caller's provided stage ID */
            pfgInfo->stage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).hwStageId;
        } /* if (unitData->devInfo->stages <= stage) */
        /* Arad uses qualifier array */
        if (qualLimit) {
            pfgInfo->is_array_qualifier = TRUE;
            pfgInfo->is_staggered = (flags & _BCM_DPP_PRESEL_ALLOC_SECOND_PASS) ? TRUE : FALSE;
            if (soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
                pfgInfo->is_for_hw_commit = FALSE;
            }
            for (index = 0, offset = 0; index < qualLimit; index++) {
                if ((0 <= bcmQuals[index].qualType) &&
                    (bcmFieldQualifyCount > bcmQuals[index].qualType)) {
                    if (offset < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX) {
                        pfgInfo->qual_vals[offset].type = bcmQuals[index].hwType;
                        pfgInfo->qual_vals[offset].val.arr[0] = COMPILER_64_LO(bcmQuals[index].qualData);
                        pfgInfo->qual_vals[offset].val.arr[1] = COMPILER_64_HI(bcmQuals[index].qualData);
                        pfgInfo->qual_vals[offset].is_valid.arr[0] = COMPILER_64_LO(bcmQuals[index].qualMask);
                        pfgInfo->qual_vals[offset].is_valid.arr[1] = COMPILER_64_HI(bcmQuals[index].qualMask);
                        offset++;
                    } else { /* if (offset is valid) */
                        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d presel"
                                                           " %u tries to"
                                                           " specify too"
                                                           " many quals"),
                                          unit,
                                          presel));
                    } /* if (offset is valid) */
                } else if (BCM_FIELD_ENTRY_INVALID != bcmQuals[index].qualType) {
                    /* should not see this condition */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d presel %u"
                                                       " tries to specify "
                                                       " unsupported"
                                                       " qual %d"),
                                      unit,
                                      presel,
                                      bcmQuals[index].qualType));
                }
            } /* for (index = 0, offset = 0; index < qualLimit; index++) */
        } /* if (caller provided any qualifiers) */
        sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                                        pfg_ndx,
                                                        pfgInfo,
                                                        &success);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                            (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                              " presel %u)"),
                             unit,
                             presel));
        BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                            (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                              " unsuccessful"),
                             unit,
                             presel));
    } /* if (caller provided qualifier data) */
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_reset(int unit)
{
    SOC_PPC_PMF_PFG_INFO *pfgInfo = NULL;
    uint32 limit = 0;
    uint32 index;
    uint32 sandResult;
    uint32 success;
    int result;
    int auxRes = BCM_E_NONE;
    uint32 handle = (unit);
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPC_PMF_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    SOC_PPC_PMF_PFG_INFO_clear(pfgInfo);
    pfgInfo->stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
    limit = SOC_PPC_FP_NOF_PFGS_ARAD;

    for (index = 0; index < limit; index++) {
        sandResult = soc_ppd_fp_packet_format_group_set(handle,
                                                        index,
                                                        pfgInfo,
                                                        &success);
        result = handle_sand_result(sandResult);
        if (BCM_E_NONE == result) {
            result = translate_sand_success_failure(success);
        }
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to clear unit %d PFG %d:"
                                  " %d (%s)\n"),
                       unit,
                       index,
                       result,
                       _SHR_ERRMSG(result)));
            auxRes = result;
        }
    } /* for (index = 0; index < limit; index++) */

    BCMDNX_IF_ERR_EXIT(auxRes);
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_init(bcm_dpp_field_info_OLD_t *unitData)
{
    _bcm_dpp_field_presel_idx_t presel;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* verify initial setup */
    /*
     * was:
     *   unitData->preselLimit = unitData->devInfo->presels;
     */
    if (!SOC_WARM_BOOT(unit)) {
        /* Cold boot -  allocate sw-state resources */
        FIELD_ACCESS.preselLimit.set(unit, unitData->devInfo->presels);
        for (presel = 0; presel < unitData->devInfo->presels; presel++) {
            /*
             * Was:
             *   BCM_FIELD_PRESEL_REMOVE(unitData->preselInUse, presel);
             * Which was equivalent to:
             *   SHR_BITCLR(unitData->preselInUse.w, presel) ;
             */
            FIELD_ACCESS_PRESELINUSE_W.bit_clear(unit, presel) ;
            /*
             * Was:
             *   unitData->preselRefs[presel] = 0;
             */
            FIELD_ACCESS_PRESELREFS.set(unit, presel, 0) ;
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_presel_single_dump
 *   Purpose
 *      Dump the settings for a single preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) const char *prefix = pointer to base prefix
 *      (in) const char *newPrefix = pointer to prefix plus indentation
 *      (in) _bcm_dpp_field_presel_idx_t presel = which preselector to dump
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *     Since we do not cache preselectors, the 'verify' feature of the
 *     qualifier dump is not used here (it was just read from hardware, so what
 *     point is seen in comparing the value read to itself?).
 */
STATIC int
_bcm_dpp_field_presel_single_dump(bcm_dpp_field_info_OLD_t *unitData,
                                  const char *prefix,
                                  const char *newPrefix,
                                  _bcm_dpp_field_presel_idx_t presel)
{
    _bcm_dpp_field_qual_t preselQual[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    _bcm_dpp_field_stage_idx_t stage;
    unsigned int index;
    unsigned int count;
    int dumpedRanges;
    int result;
    int unit;
    unsigned int preselRefs_element ;

    unit = unitData->unit;

    result = FIELD_ACCESS_PRESELREFS.get(unit, presel, &preselRefs_element) ;
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
             "%sPreselector %d: refs = %d\n"),
             prefix,
             presel,
             preselRefs_element));
        result = _bcm_dpp_field_presel_info_get(unitData,
                                            presel,
                                            SOC_PPC_FP_NOF_QUALS_PER_DB_MAX,
                                            0,
                                            &stage,
                                            &(preselQual[0]));
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                 "%s  Stage     = %s (%d)\n"),
                 prefix,
                 _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageName,
                 stage));
            /* dump qualifiers */
            for (index = 0, count = 0, dumpedRanges = FALSE, result = BCM_E_NONE;
                 (BCM_E_NONE == result) &&
                 (index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                 index++) {
                result = _bcm_dpp_field_entry_qual_dump(unitData,
                                                    0 ,
                                                    &(preselQual[0]),
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                                    NULL,
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
                                                    index,
                                                    !dumpedRanges,
                                                    newPrefix);
                if (1 == result) {
                    count++;
                    result = BCM_E_NONE;
                } else if (2 == result) {
                    dumpedRanges = TRUE;
                    count++;
                    result = BCM_E_NONE;
                }
            } /* for (index = 0; index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; index++) */
            if (0 == count) {
                /* nothing in qualifiers */
                LOG_CLI((BSL_META_U(unit,
                     "%s    (none; matches no traffic)\n"),
                     prefix));
            }
        } else { /* if (BCM_E_NONE == result) */
            LOG_CLI((BSL_META_U(unit,
                 "%s  (unable to access: %d (%s)\n"),
                 prefix,
                 result,
                 _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } else { /* if (BCM_E_NONE == result) */
        LOG_CLI((BSL_META_U(unit,
             "%s  (unable to access: preselRefs[%d]: %d (%s))\n"),
             prefix,
             presel,
             result,
             _SHR_ERRMSG(result)));
    } /* if (BCM_E_NONE == result) */
    return result;
}

int
_bcm_dpp_field_presel_dump(bcm_dpp_field_info_OLD_t *unitData,
                           const char *prefix)
{
    bcm_field_presel_t index;
    int count;
    int result = BCM_E_NONE;
    char *newPrefix;
    int unit;
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;

    unit = unitData->unit;
    result = FIELD_ACCESS.preselLimit.get(unit, &preselLimit) ;
    if (result != BCM_E_NONE) {
        goto exit ;
    }
    count = sal_strlen(prefix);
    newPrefix = sal_alloc(count + 6, "prefix workspace");
    if (!newPrefix) {
        LOG_CLI((BSL_META_U(unit,
                            "%sUnable to dump preselectors: could not allocate %d"
                 " bytes workspace\n"),
                 prefix,
                 count + 6));
        result = BCM_E_MEMORY;
        /*
         * No memory was allocated - No need to free.
         */
        goto exit ;
    }
    sal_snprintf(newPrefix,count + 5, "%s    ", prefix);
    for (index = 0, count = 0; index < preselLimit; index++) {
        /*
         * Was:
         *   if (BCM_FIELD_PRESEL_TEST(unitData->preselInUse, index))
         * Which was equivalent to:
         *   if (SHR_BITGET(unitData->preselInUse.w, index))
         */
        result = FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value) ;
        if (result != BCM_E_NONE) {
            break ;
        }
        if (bit_value) {
            count++;
            _bcm_dpp_field_presel_single_dump(unitData,
                                              prefix,
                                              newPrefix,
                                              index);
        } /* if (this preselector is in use) */
    } /* for (index = 0; index < preselLimit; index++) */
    if (!count) {
        LOG_CLI((BSL_META_U(unit,
                            "%s(no preselectors this unit)\n"), prefix));
    }
    sal_free(newPrefix);
exit:
    return result;
}
#endif /* def BROADCOM_DEBUG */



/*
 *   Function
 *      _bcm_dpp_field_presel_alloc
 *   Purpose
 *      Allocate a preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) int with_id = TRUE if caller supplies ID, FALSE otherwise
 *      (in/out) bcm_field_presel_t *presel = pointer to the (place to put) ID
 *      (in) _bcm_dpp_field_program_idx_t program ID = associated with the preselector 
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
int
_bcm_dpp_field_presel_alloc(bcm_dpp_field_info_OLD_t *unitData,
                            bcm_field_stage_t        stage,
                            _bcm_dpp_field_presel_alloc_flags_t flags,
                            bcm_field_presel_t *presel,
                            _bcm_dpp_field_program_idx_t program_id)
{
    bcm_field_presel_t index;
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;
    _bcm_dpp_field_stage_idx_t stage_id;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    stage_id = unitData->devInfo->stages;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_ID) {

        index = *presel;

        /*stage for presel is indicated - use it for retrieving presel bitmap index and stage-id*/
        if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_STAGE ) 
        {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,*presel, stage, (flags & _BCM_DPP_PRESEL_ALLOC_SECOND_PASS ), &index, &stage_id)); 
        }
        if ((0 > index) || (preselLimit <= index)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselector ID %d"),
                              unit,
                              *presel));
        }
        /*
         * Was:
         *   if (BCM_FIELD_PRESEL_TEST(unitData->preselInUse, index))
         * Which was equivalent to:
         *   if (SHR_BITGET(unitData->preselInUse.w, index))
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
        if (bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d preselector ID %d"
                                               " is already in use"),
                              unit,
                              *presel));
        }
    } else { /* if (flags & _BCM_PETRA_PRESEL_ALLOC_WITH_ID) */
        if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) {
            for (index = 0; index < preselLimit; index++) {
                /*
                 * Was:
                 *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, (preselLimit - index) - 1))
                 * Which was equivalent to:
                 *   if (!SHR_BITGET(unitData->preselInUse.w, (preselLimit - index) - 1))
                 */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, ((preselLimit - index) - 1), &bit_value)) ;
                if (!bit_value) {
                    break;
                }
            }
        } else { /* if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) */
            for (index = 0; index < preselLimit; index++) {
                /*
                 * Was:
                 *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, index))
                 * Which was equivalent to:
                 *   if (!SHR_BITGET(unitData->preselInUse.w, index))
                 */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
                if (!bit_value) {
                    break;
                }
            }
        } /* if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) */
        if (preselLimit <= index) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d has no preselectors"
                                               " available"),
                              unit));
        }
        if (flags & _BCM_DPP_PRESEL_ALLOC_DOWN) {
            index = (preselLimit - index) - 1;
        }
    } /* if (flags & _BCM_PETRA_PRESEL_ALLOC_WITH_ID) */
    /* set 'blank' state for the new preselector */


    
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_set(unitData, 
                                                           index,
                                                           0,
                                                           flags,
                                                           stage_id,
                                                           NULL,
                                                           program_id));
    
    
    if (!SOC_WARM_BOOT(unit)) {
        /* Cold boot -  allocate sw-state resources */
        /*
         * Was:
         *   BCM_FIELD_PRESEL_ADD(unitData->preselInUse, index)
         * Which was equivalent to:
         *   SHR_BITSET(unitData->preselInUse.w, index)
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_set(unit, index)) ;
    }
    *presel = index;
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_field_presel_free
 *   Purpose
 *      Free a preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) bcm_field_presel_t presel = presel to be freed
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
STATIC int
_bcm_dpp_field_presel_free(bcm_dpp_field_info_OLD_t *unitData,
                           bcm_field_presel_t presel)
{
    _bcm_dpp_field_presel_idx_t preselLimit;
    _bcm_dpp_field_stage_idx_t stage_id;
    bcm_field_stage_t stage = bcmFieldStageCount;
    uint32 presel_flags = 0;
    uint8 bit_value ;
    uint8 is_second_pass = 0;
    int index;
    unsigned int preselRefs_element ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;

    index = _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel);
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(presel)) 
    {
        stage = _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(presel);
        is_second_pass = _BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(presel);
        presel_flags = _BCM_DPP_FIELD_PRESEL_FLAG_FROM_STAGE(stage);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit, presel, stage, is_second_pass , &index, &stage_id)); 
    }
    if ((0 > index) || (preselLimit <= index)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                           " preselector ID %d"),
                          unit,
                          presel));
    }
    /*
     * Was:
     *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, presel))
     * Which was equivalent to:
     *   if (!SHR_BITGET(unitData->preselInUse.w, presel))
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
    if (!bit_value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector ID %d"
                                           " is not in use"),
                          unit,
                          presel));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELREFS.get(unit, index, &preselRefs_element)) ;
    if (preselRefs_element > 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector id 0x%x"
                                           " is still used by %d groups"),
                          unit,
                          presel,
                          preselRefs_element));
    }

    /* If port profiles are defined - clear them */
    BCMDNX_IF_ERR_EXIT(
        _bcm_dpp_field_presel_port_profile_clear_all(unitData,
                                                     (presel | BCM_FIELD_QUALIFY_PRESEL | presel_flags)));

    if ( soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        
         BCMDNX_IF_ERR_EXIT( _bcm_dpp_field_presel_destroy(unitData, presel | BCM_FIELD_QUALIFY_PRESEL | presel_flags, is_second_pass));
    }
    else
    {
        /* set 'blank' state for the discarded preselector */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_set(unitData,
                                                               presel,
                                                               0,
                                                               0,
                                                               unitData->devInfo->stages,
                                                               NULL,
                                                               0));
    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Cold boot -  allocate sw-state resources */
        /*
         * Was:
         *   BCM_FIELD_PRESEL_REMOVE(unitData->preselInUse, presel);
         * Which was equivalent to:
         *   SHR_BITCLR(unitData->preselInUse.w, presel)
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_clear(unit, index)) ;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_qualify(bcm_dpp_field_info_OLD_t *unitData,
                              bcm_field_presel_t presel,
                              bcm_field_stage_t  stage, 
                              uint32 flags,
                              bcm_field_qualify_t qualifier,
                              unsigned int count,
                              const uint64 *data,
                              const uint64 *mask)
{
    _bcm_dpp_field_qual_t *preselQuals = NULL;
    _bcm_dpp_field_stage_idx_t stage_id = 0;
    int index;
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint32 presel_flags = 0;
    uint8 bit_value ;
    unsigned int preselRefs_element ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    index = presel;
    if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_STAGE ) 
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,presel, stage, (flags & _BCM_DPP_PRESEL_ALLOC_SECOND_PASS ), &index, &stage_id));
    }
    if ((0 > index) || (preselLimit <= index)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d not valid"),
                          unit,
                          index));
    }
    /*
     * Was:
     *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, presel))
     * Which was equivalent to:
     *   if (!SHR_BITGET(unitData->preselInUse.w, presel))
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
    if (!bit_value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d not in use for %d stage"),
                          unit,
                          presel, stage_id));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELREFS.get(unit, index, &preselRefs_element)) ;
    if (preselRefs_element) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d is"
                                           " referenced by %d groups, so it"
                                           " can not be changed"),
                          unit,
                          presel,
                          preselRefs_element));
    }
    if ((0 > qualifier) || (bcmFieldQualifyCount <= qualifier)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d invalid"),
                          qualifier));
    }
    if (1 > count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d requires at least one"
                                           " octbyte for qualifier data/mask"
                                           " arguments"),
                          unit));
    }
    /* general validation for certain arguments */
    switch (qualifier) {
    case bcmFieldQualifyStage:
        if (unitData->devInfo->stages <= COMPILER_64_LO(*data)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u invalid"),
                              unit,
                              COMPILER_64_LO(*data)));
        }
        if (0 == (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, COMPILER_64_LO(*data)).stageFlags &
                  _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselectors on stage %s\n"),
                              unit,
                              _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, COMPILER_64_LO(*data)).stageName));
        }
        break;
    default:
        /* no general checks */
        break;
    }

    switch (qualifier) {
        case bcmFieldQualifyStage:
        {
            /* stage is special and handled locally */
            preselQuals = sal_alloc(sizeof(*preselQuals) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX,
                                    "preselector workspace");
            if (!preselQuals) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate"
                                                   " %u bytes for presel"
                                                   " workspace"),
                                  unit,
                                  (uint32)sizeof(*preselQuals) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                                   index,
                                                                       SOC_PPC_FP_NOF_QUALS_PER_DB_MAX,
                                                                   flags,
                                                                   &stage_id,
                                                                   preselQuals));
            stage_id = COMPILER_64_LO(*data);
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_set(unitData,
                                                                   index,
                                                                       SOC_PPC_FP_NOF_QUALS_PER_DB_MAX,
                                                                   flags,
                                                                   stage_id,
                                                                   preselQuals,
                                                                   0));
            break;
        }
        default:
        {
            int32 qualMaps_handle ;

            /* general case goes to generic qualifier code */
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, qualifier, &qualMaps_handle)) ;
            if (qualMaps_handle) {
                if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_STAGE )  {
                    presel_flags = _BCM_DPP_FIELD_PRESEL_FLAG_FROM_STAGE(stage);
                }
                if (flags & _BCM_DPP_PRESEL_ALLOC_SECOND_PASS )  {
                    presel_flags |= BCM_FIELD_QUALIFY_PRESEL_STAGGERED;
                }
                /* this unit supports this qualifier */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set(unitData,
                                                                     presel | BCM_FIELD_QUALIFY_PRESEL | presel_flags,
                                                                     qualifier,
                                                                     count,
                                                                     data,
                                                                     mask));
            } else { /* if (qualMaps_handle) */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                                   " qualifier %d (%s)\n"),
                                  unit,
                                  qualifier,
                                  _bcm_dpp_field_qual_name[qualifier]));
            } /* if (qualMaps_handle) */
        }
    } /* switch (qualifier) */
exit:
    if (preselQuals) {
        sal_free(preselQuals);
    }
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_qualify_get(bcm_dpp_field_info_OLD_t *unitData,
                                  bcm_field_presel_t presel,
                                  bcm_field_stage_t  stage,
                                  uint32 flags,
                                  bcm_field_qualify_t qualifier,
                                  unsigned int count,
                                  uint64 *data,
                                  uint64 *mask)
{
    _bcm_dpp_field_stage_idx_t stage_id = 0;
    _bcm_dpp_field_presel_idx_t preselLimit;
    int index;
    uint32 presel_flags = 0;
    uint8 bit_value ;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    index = presel ; 
    if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_STAGE ) 
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,presel, stage, (flags & _BCM_DPP_PRESEL_ALLOC_SECOND_PASS ), &index, &stage_id)); 
    }
    if ((0 > index) || (preselLimit <= index)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d is invalid"),
                          unit,
                          presel));
    }
    /*
     * Was:
     *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, presel))
     * Which was equivalent to:
     *   if (!SHR_BITGET(unitData->preselInUse.w, presel))
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
    if (!bit_value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d preselector %d not in use"),
                          unit,
                          presel));
    }
    if ((0 > qualifier) || (bcmFieldQualifyCount <= qualifier)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d invalid"),
                          qualifier));
    }
    if (1 > count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d requires at least one"
                                           " octbyte for qualifier data/mask"
                                           " arguments"),
                          unit));
    }

    switch (qualifier) {
        case bcmFieldQualifyStage:
        {
            /* stage is special and handled locally */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                                   index,
                                                                   0,
                                                                   flags,
                                                                   &stage_id,
                                                                   NULL));
            COMPILER_64_SET(*data, 0, stage_id);
            COMPILER_64_SET(*mask, ~0, ~0);
            break;
        }
        default:
        {
            int32 qualMaps_handle ;

            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, qualifier, &qualMaps_handle)) ;
            if (qualMaps_handle) {
                if (flags & _BCM_DPP_PRESEL_ALLOC_WITH_STAGE )  {
                    presel_flags = _BCM_DPP_FIELD_PRESEL_FLAG_FROM_STAGE(stage);
                }
                /* this unit supports this qualifier */
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get(unitData,
                                                                     presel | BCM_FIELD_QUALIFY_PRESEL | presel_flags,
                                                                     qualifier,
                                                                     count,
                                                                     data,
                                                                     mask));
            } else { /* if (qualMaps_handle) */
                /* this unit does not support this qualifier */
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                                   " qualifier %s (%d) for"
                                                   " preselectors\n"),
                                  unit,
                                  _bcm_dpp_field_qual_name[qualifier],
                                  qualifier));
            } /* if (qualMaps_handle) */
        }
    } /* switch (qualifier) */
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_field_group_presel_set(bcm_dpp_field_info_OLD_t *unitData,
                                _bcm_dpp_field_grp_idx_t group,
                                bcm_field_presel_set_t *presel_set,
                                uint32 pgm_bmp)
{
    /* coverity[stack_use_overflow:FALSE] */
    _bcm_dpp_field_group_t newGroupData;
    int result;
    uint32 groupFlags;
     BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    if (0 == (groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u is not in use"),
                          unit,
                          group));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.get(unit, group, &newGroupData));
    /* assign preselector set to this group (replacing if needed) */
    if ((_BCM_DPP_FIELD_GROUP_IN_HW) ==
        (groupFlags &
         (_BCM_DPP_FIELD_GROUP_IN_HW | _BCM_DPP_FIELD_GROUP_PRESEL))) {
        /* group was in hardware but using 'implied' preselector set */
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d group %u was using implied presel set"
                             " because its presel set was not configured"
                             " before its action set was configured; please"
                             " configure the presel set for groups before"
                             " configuring their action set.\n"),
                  unit,
                  group));
    }
    newGroupData.groupFlags |= _BCM_DPP_FIELD_GROUP_PRESEL;
    sal_memcpy(&(newGroupData.preselSet),
               presel_set,
               sizeof(newGroupData.preselSet));
    if (groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW) {
        result = _bcm_dpp_field_group_hardware_install(unitData,
                                                       group,
                                                       &newGroupData);
        BCMDNX_IF_ERR_EXIT_MSG(result,
                            (_BSL_BCM_MSG_NO_UNIT("unit %d group %u failed to"
                                              " update to hardware: %d (%s)"),
                             unit,
                             group,
                             result,
                             _SHR_ERRMSG(result)));
    }
    /* presel ref count updated by hardware install/remove */
    /* commit group information */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.set(unit, group, &newGroupData));
#ifdef BCM_WARM_BOOT_SUPPORT
    /*_bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);*/
#endif /* def BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}

/******************************************************************************
 *
 *  Functions and data exported to API users
 */

int
bcm_petra_field_presel_create(int unit,
                              bcm_field_presel_t *presel_id)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                            (_BSL_BCM_MSG_NO_UNIT("Please use the API bcm_field_presel_create_stage_id() "
                                                  "when in advanced mode preselection management.\n"),
                             unit, presel_id));
    }
    if (!presel_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT argument must not be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_presel_alloc(unitData, bcmFieldStageCount, 0 /* flags */, presel_id, 0);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_presel_create_id(int unit,
                                 bcm_field_presel_t presel_id)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                            (_BSL_BCM_MSG_NO_UNIT("Unit %d: Unable to create presel %d in advanced mode. "
                                                  "Please use the API bcm_field_presel_create_stage_id().\n"),
                             unit, presel_id));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_presel_alloc(unitData,
                                         bcmFieldStageCount,
                                         _BCM_DPP_PRESEL_ALLOC_WITH_ID,
                                         &presel_id,
                                         0);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_presel_create_stage_id(int unit,
                                 bcm_field_stage_t stage,
                                 bcm_field_presel_t presel_id)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_presel_alloc_flags_t flags;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if(!soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                            (_BSL_BCM_MSG_NO_UNIT("Unit %d: Unable to create presel %d. "
                                                  "Please use the APIs bcm_field_presel_create() or "
                                                  "bcm_field_presel_create_id() when not in advanced mode.\n"),
                             unit, presel_id));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    flags = _BCM_DPP_PRESEL_ALLOC_WITH_ID | _BCM_DPP_PRESEL_ALLOC_WITH_STAGE ;

    result = _bcm_dpp_field_presel_alloc(unitData,
                                         stage,
                                         ( flags | (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(presel_id) ? _BCM_DPP_PRESEL_ALLOC_SECOND_PASS : 0 )),
                                         &presel_id,
                                         0);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_presel_destroy(int unit,
                               bcm_field_presel_t presel_id)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);
    result = _bcm_dpp_field_presel_free(unitData, presel_id);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_group_presel_set(int unit,
                                 bcm_field_group_t group,
                                 bcm_field_presel_set_t *presel_set)
{
    _DPP_FIELD_COMMON_LOCALS;
   _bcm_dpp_field_grp_idx_t groupLimit;

    BCMDNX_INIT_FUNC_DEFS;
   FIELD_ACCESS.groupLimit.get(unit, &groupLimit);

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if ((0 > group) || (groupLimit <= group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("group %d is not valid"), group));
    }
    if (!presel_set) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory IN pointer must not be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_dpp_field_group_presel_set(unitData, group, presel_set,0);

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_group_presel_get(int unit,
                                 bcm_field_group_t group,
                                 bcm_field_presel_set_t *presel_set)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_grp_idx_t groupLimit;
    uint32 groupFlags;
    int dpp_field_unit_lock_was_taken = 0;

    BCMDNX_INIT_FUNC_DEFS;
    FIELD_ACCESS.groupLimit.get(unit, &groupLimit);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if ((0 > group) || (groupLimit <= group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("group %d is not valid"), group));
    }
    if (!presel_set) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT argument must not be NULL")));
    }

    _DPP_FIELD_UNIT_LOCK(unitData);
    dpp_field_unit_lock_was_taken = 1;
    if (groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE) {
        if (0 == (groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "unit %d group %d reading preselector set"
                                 " from a group where it was not set\n"),
                      unit,
                      group));
        }

        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.preselSet.get(unit, group, presel_set));
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d group %d is not in use\n"),
                   unit,
                   group));
        result = BCM_E_NOT_FOUND;
    }

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if (dpp_field_unit_lock_was_taken) {
        _DPP_FIELD_UNIT_UNLOCK(unitData);
    }
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_install(bcm_dpp_field_info_OLD_t *unitData,
                              _bcm_dpp_field_presel_idx_t presel,
                              /* Here presel is the whole passed entry ID, together with the flags */
                              uint8 is_second_pass)
{
    SOC_PPC_PMF_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    uint32 success;
    uint32 flags = 0;
    int index;
    bcm_field_stage_t bcm_stage;
    _bcm_dpp_field_stage_idx_t stage = _BCM_DPP_FIELD_STAGE_INDEX_INGRESS;
    int unit=0;
    BCMDNX_INIT_FUNC_DEFS;

    unit = unitData->unit;

    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPC_PMF_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    SOC_PPC_PMF_PFG_INFO_clear(pfgInfo);
    index = _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel); 
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(presel)) {
        bcm_stage =  _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(presel) ; 
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit, _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel), bcm_stage, is_second_pass, &index, &stage)); 
        flags = _BCM_DPP_PRESEL_ALLOC_WITH_STAGE;
    }
    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                      index,
                                                      0,
                                                      flags,
                                                      &stage,
                                                      NULL));

    pfgInfo->stage = stage;
    pfgInfo->is_for_hw_commit = TRUE;
    pfgInfo->is_array_qualifier = TRUE;
    sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                                    _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel),
                                                    pfgInfo,
                                                    &success);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                        (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                          " presel %u)"),
                         unit,
                         presel));
    BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                        (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                          " unsuccessful"),
                         unit,
                         presel));
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;

}

int
_bcm_dpp_field_presel_destroy(bcm_dpp_field_info_OLD_t *unitData,
                              _bcm_dpp_field_presel_idx_t presel,
                              /* Here presel is the whole passed entry ID, together with the flags */
                              uint8 is_second_pass)
{
    SOC_PPC_PMF_PFG_INFO *pfgInfo = NULL;
    uint32 sandResult;
    uint32 success;
    uint32 flags = 0;
    _bcm_dpp_field_stage_idx_t stage;
    int index;
    bcm_field_stage_t bcm_stage;
    int unit=0;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPC_PMF_PFG_INFO), "_bcm_dpp_field_presel_reset.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    SOC_PPC_PMF_PFG_INFO_clear(pfgInfo);
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(presel)) {
        bcm_stage =  _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(presel) ; 
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,presel, bcm_stage, is_second_pass, &index, &stage)); 
        flags = _BCM_DPP_PRESEL_ALLOC_WITH_STAGE;
    } else {
        index = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(presel);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                          index,
                                                          0,
                                                          flags,
                                                          &stage,
                                                          NULL));
    }
    pfgInfo->stage = stage;
    pfgInfo->is_for_hw_commit = TRUE;
    pfgInfo->is_array_qualifier = FALSE;
    sandResult = soc_ppd_fp_packet_format_group_set(unitData->unitHandle,
                                                    _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel),
                                                    pfgInfo,
                                                    &success);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(sandResult),
                        (_BSL_BCM_MSG_NO_UNIT("unable to write unit %d"
                                          " presel %u)"),
                         unitData->unit,
                         _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel)));
    BCMDNX_IF_ERR_EXIT_MSG(translate_sand_success_failure(success),
                        (_BSL_BCM_MSG_NO_UNIT("write unit %d presel %u"
                                          " unsuccessful"),
                         unitData->unit,
                         _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel)));
exit:
    BCM_FREE(pfgInfo);
    BCMDNX_FUNC_RETURN;

}


