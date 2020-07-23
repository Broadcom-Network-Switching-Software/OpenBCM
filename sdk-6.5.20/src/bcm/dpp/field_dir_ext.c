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
 *     Direct extraction suppot
 */

/*
 *  The functions in this module deal with direct extraction feature on PPD
 *  devices. They are invoked by the main field module when certain things
 *  occur, such as calls that explicitly involve direct extraction work.
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

#include <bcm_int/common/debug.h>

#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/dcmn/dcmn_wb.h>
#endif /* def BCM_WARM_BOOT_SUPPORT */
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>
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

/*
 *  Function
 *    _bcm_dpp_field_dir_ext_action_add
 *  Purpose
 *    Add a direct extraction action to a direct extraction entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = direct extraction entry ID
 *    (in) action = pointer to direct extraction action descriptionm
 *  Returns
 *    int (implied cast from bcm_error_t)
 *      BCM_E_NONE if successful
 *      BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_dpp_field_dir_ext_action_add(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  const bcm_field_extraction_action_t *action,
                                  int count,
                                  const bcm_field_extraction_field_t *extraction)
{
    bcm_field_qualify_t qual;
    SOC_PPC_FP_DIR_EXTR_ACTION_VAL tempAct;
    SOC_PPC_FP_DIR_EXTR_ACTION_VAL entryDe_hwParam;
    unsigned int index;
    int actIndex;
    unsigned int extractStart = 0;
    unsigned int takenBits = 0;
    unsigned int fldIndex;
    unsigned int offset;
    unsigned int bits;
    unsigned int actBits;
    unsigned int totalBits;
    uint32 mask;
    SOC_PPC_FP_QUAL_TYPE entryDe_hwType;
    SOC_PPC_FP_ACTION_TYPE *ppdAct = NULL;
    SOC_PPC_FP_QUAL_TYPE *ppdQual = NULL;
    _bcm_dpp_field_dq_idx_t dqLimit;
    _bcm_dpp_field_grp_idx_t group;
    uint32 groupFlags;
    uint32 groupTypes;
    _bcm_dpp_field_stage_idx_t stage; 
    uint8 qset_res;
    bcm_field_qset_t qset;
    uint32 entryDe_entryFlags;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    bcm_field_action_t entryDe_bcmType;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }

    if (0 != (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d already installed.\n"
                          "Could not change action for a direct extraction entry in hardware.Please destroy it then create again.\n"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &group));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &stage));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupTypes.get(unit, group, &groupTypes));

    if ((0 > action->action) || (bcmFieldActionCount <= action->action || (BCM_FIELD_ENTRY_INVALID == action->action))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action type %d is not valid"),
                          action->action));
    }
    if (0 == count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action must contain at least one"
                                           " extraction")));
    }
    if (!extraction) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("obligatory IN pointer argument"
                                           " extractions must not be NULL")));
    }
    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_act_from_bcm_act(unitData,
                                                      stage,
                                                      action->action,
                                                      &ppdAct),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to find description"
                                          " for action %d (%s)"),
                         unit,
                         action->action,
                         _bcm_dpp_field_action_name[action->action]));
    if ((0 > ppdAct[0]) || 
        (!_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct[0]))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support action"
                                           " %d (%s) for stage %u types %08X"),
                          unit,
                          action->action,
                          _bcm_dpp_field_action_name[action->action],
                          stage,
                          groupTypes));
    }

    actIndex = unitData->devInfo->actChain-1;

    for (index = 1; index < unitData->devInfo->actChain; index++) {
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct[index])) {
            BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_action_bits(unit,
                                                 stage,
                                                 ppdAct[0],
                                                 &actBits),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get number of"
                                          " bits for action %d (%s) -> %d"),
                         unit,
                         action->action,
                         _bcm_dpp_field_action_name[action->action],
                         ppdAct[0]));

            for (index = 0, totalBits = 0;
                 index < count;
                 index++) {
                totalBits += extraction[index].bits;
            }

            if (totalBits <= actBits) {
                /* 
                * Backward compatibility:
                *     For compound action, if total bits is <= action bits of the first SOC layer action,
                *     do extraction only the fir SOC layer action only.
                */
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d only uses the first action from a"
                                     " compound action in direct extraction\n"),
                          unit));
                actIndex = 0;
                break;
            }
        }
    }

    for (; actIndex >= 0;  actIndex--) {
        if (!_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct[actIndex])) {
            continue;
        }

        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_action_bits(unit,
                                                     stage,
                                                     ppdAct[actIndex],
                                                     &actBits),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get number of"
                                              " bits for action %d (%s) -> %d"),
                             unit,
                             action->action,
                             _bcm_dpp_field_action_name[action->action],
                             ppdAct[actIndex]));
        /* When action nof bits is larger than max FEM size, or user requested large de,
         * and entry has a valid qualifier, add one bit for valid bit of FES */
        if ((actBits > SOC_DPP_DEFS_GET(unit,fem_max_action_size_nof_bits)) ||
                    (groupFlags & _BCM_DPP_FIELD_GROUP_LARGE)) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, entry, 0, &entryDe_hwType));
            if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entryDe_hwType)) {
                actBits++;
            }
        }

        /* convert the action description to the PPD view, checking it as we go */
        sal_memset(&tempAct, 0x00, sizeof(tempAct));
        if (1 > count) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("invalid number of extractions %d;"
                                               " must be >= 1"),
                              count));
        }
        if (SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS <= count) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d supports maximum %d"
                                               " extractions per action"),
                              unit,
                              SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS));
        }

        tempAct.type = ppdAct[actIndex];
        for (index = extractStart, totalBits = 0, fldIndex = 0;
             index < count;
             index++, fldIndex++) {
            totalBits += extraction[index].bits - takenBits;
            if ((1 > extraction[index].bits) ||
                (31 < extraction[index].bits)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d extraction bit lengths"
                                                   " must be 1..31"),
                                  unit));
            }
            tempAct.fld_ext[fldIndex].nof_bits = extraction[index].bits - takenBits;
            mask = (1 << (extraction[index].bits-takenBits)) - 1;
            if (extraction[index].flags &
                BCM_FIELD_EXTRACTION_FLAG_CONSTANT) {
                /* ensure nonzero constant */
               tempAct.fld_ext[fldIndex].cst_val = ((extraction[index].value >> takenBits) & mask) |
                                                (1 << (extraction[index].bits-takenBits));
               tempAct.fld_ext[fldIndex].type = BCM_FIELD_ENTRY_INVALID;
               tempAct.fld_ext[fldIndex].fld_lsb = 0;
            } else { /* if (this extraction uses a constant value) */
                uint8 bit_value ;

                tempAct.fld_ext[fldIndex].cst_val = 0;
                qual = extraction[index].qualifier;
                if (extraction[index].flags &
                    BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD) {
                    /* data field, translate the index to PPD qualifier */

                    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
                    if ((0 > qual) || (dqLimit <= qual)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("data qualifier %d is"
                                                           " not valid; range is"
                                                           " 0..%d"),
                                          extraction[index].qualifier,
                                          dqLimit));
                    }
                    /*
                     * Was:
                     *   if (!SHR_BITGET(unitData->unitQset.udf_map,extraction[index].qualifier))
                     */
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_UNITQSET_UDFMAP.bit_get(unit,extraction[index].qualifier,&bit_value)) ;
                    if (!bit_value) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                           " support data qualifier"
                                                           " %d"),
                                          unit,
                                          extraction[index].qualifier));
                    }
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.qset.udf_map.bit_get(unit, group, extraction[index].qualifier, &qset_res));
                    if (!qset_res) {
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u qset"
                                                           " does not include data"
                                                           " qualifier %d"),
                                          unit,
                                          entryDe_entryGroup,
                                          extraction[index].qualifier));
                    }
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_petra_field_map_bcm_udf_to_ppd(unitData,
                                                                            extraction[index].qualifier,
                                                                            &(tempAct.fld_ext[fldIndex].type)),
                                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to map"
                                                          " data qualifier %d to"
                                                          " PPD qualifier"),
                                         unit,
                                         extraction[index].qualifier));
                } else { /* if (this extraction uses data field qualifier) */
                    /* not data field, use normal qualifier lookup */
                    uint8 bit_value ;

                    if ((0 > qual) || (bcmFieldQualifyCount <= qual)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("qualifier %d is not"
                                                           " valid"),
                                          qual));
                    }
                    /*
                     * Was:
                     *   if (!BCM_FIELD_QSET_TEST(unitData->unitQset, qual))
                     * Which was equivalent to:
                     *   if (!SHR_BITGET((unitData->unitQset.w), qual))
                     */
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_UNITQSET_W.bit_get(unit,qual,&bit_value)) ;
                    if (!bit_value) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                           " support qualifier"
                                                           " %d (%s)"),
                                          unit,
                                          qual,
                                          _bcm_dpp_field_qual_name[qual]));
                    }
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.qset.get(unit, group, &qset));
                    if (!BCM_FIELD_QSET_TEST(qset, qual)) {
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u qset"
                                                           " does not include %d"
                                                           " (%s)"),
                                          unit,
                                          entryDe_entryGroup,
                                          qual,
                                          _bcm_dpp_field_qual_name[qual]));
                    }
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_qual_from_bcm_qual(unitData,
                                                                        stage,
                                                                        groupTypes,
                                                                        qual,
                                                                        &ppdQual),
                                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to map"
                                                          " qualifier %d (%s) to"
                                                          " PPD qualifier"),
                                         unit,
                                         qual,
                                         _bcm_dpp_field_qual_name[qual]));
                    if ((0 > ppdQual[0]) ||
                        (!_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[0]))) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                           " support qualifier %d"
                                                           " (%s) for stage %u"
                                                           " types %08X"),
                                          unit,
                                          qual,
                                          _bcm_dpp_field_qual_name[qual],
                                          stage,
                                          groupTypes));
                    }
                    for (offset = 1;
                         offset < unitData->devInfo->qualChain;
                         offset++) {
                        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[offset])) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not"
                                                               " support compound"
                                                               " qualifiers for"
                                                               " direct extraction"),
                                              unit));
                        }
                    }
                    tempAct.fld_ext[fldIndex].type = ppdQual[0];
                } /* if (this extraction uses data field qualifier) */
                BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_ppd_qual_bits(unit,
                                                           stage,
                                                           tempAct.fld_ext[fldIndex].type,
                                                           NULL,
                                                           &bits,
                                                           NULL),
                                    (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get"
                                                      " number of bits for"
                                                      " PPD qualifier %d (%s)"),
                                     unit,
                                     tempAct.fld_ext[fldIndex].type,
                                     SOC_PPC_FP_QUAL_TYPE_to_string(tempAct.fld_ext[fldIndex].type)));
                if (0 > extraction[index].lsb) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                     (_BSL_BCM_MSG_NO_UNIT("LSb must be"
                                                       " nonnegative")));
                }
                if ((bits < (extraction[index].lsb + extraction[index].bits)) &&
                    /*For Large DE i.e. in FES lsb has meaning of index from start of conatinated qual, hence different meaning
                                    When bits dont have meaning at all*/
                    ((groupFlags & _BCM_DPP_FIELD_GROUP_LARGE) == FALSE)) {
                    if (extraction[index].flags &
                        BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD) {

                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("LSb %d plus bits %d"
                                                           " must be less than or"
                                                           " equal to data qual"
                                                           " %d length %d"),
                                          extraction[index].lsb,
                                          extraction[index].bits,
                                          extraction[index].qualifier,
                                          bits));
                    } else {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                         (_BSL_BCM_MSG_NO_UNIT("LSb %d plus bits %d"
                                                           " must be less than or"
                                                           " equal to qualifier %d"
                                                           " (%s) length %d"),
                                          extraction[index].lsb,
                                          extraction[index].bits,
                                          extraction[index].qualifier,
                                          _bcm_dpp_field_qual_name[extraction[index].qualifier],
                                          bits));
                    }
                }
                tempAct.fld_ext[fldIndex].fld_lsb = extraction[index].lsb + takenBits;

                if (actIndex != 0) { /* Not the last action in compound action */
                    if (totalBits > actBits) {
                        /* already get enough bits for current action, next action starts from current extraction */
                        tempAct.fld_ext[fldIndex].nof_bits -= totalBits - actBits;
                        extractStart = index;
                        takenBits = extraction[index].bits + actBits - totalBits;
                        fldIndex++;
                        break;
                    }
                    else if (totalBits == actBits) {
                        /* already get enough bits for current action, next action starts from next extraction */
                        extractStart = index + 1;
                        takenBits = 0;
                        fldIndex++;
                        break;
                    }
                    else {
                        /* have not got enough bits for current action, move to next extraction */
                        takenBits = 0;
                    }
                }
                else {
                    /* last action */
                    takenBits = 0;
                }
            } /* if (this extraction uses a constant value) */
        } /* for (all extractions in this action) */
        if ((actIndex == 0) && (totalBits > actBits)) {
            /* if this is the last action but total bits > action bits */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("action requires %d bits but total"
                                               " extraction came to %d bits"),
                              actBits,
                              totalBits));
        }
        if (totalBits < actBits) {
            /* Do not implicitly pad it for FES */
            if (index < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS) {
                if (groupFlags & _BCM_DPP_FIELD_GROUP_LARGE) {
                    /* Large action but BCM_FIELD_GROUP_CREATE_LARGE is not set for group.
                    * FES will be allocated in this case, constant padding is not supported.
                    * Padding zeros will be the MSB zero bits in the key (implied).
                    * Only single entry with single action supported in this case.
                    * Deprecated, this is for backward compatibility.
                    * Prefer to create group with BCM_FIELD_GROUP_CREATE_LARGE and use constant
                    * zero data qualifier for padding.
                    */
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                        " total extraction came to %d bits but"
                                        " action requires %d bits, prefer use data qualifier for padding\n"),
                              totalBits,
                              actBits));
                }

                if (actBits <= SOC_DPP_DEFS_GET(unit,fem_max_action_size_nof_bits)) {
                    /* pad the extraction so it is full width */
                    tempAct.fld_ext[fldIndex].cst_val = 1 << (actBits - totalBits);
                    tempAct.fld_ext[fldIndex].type = BCM_FIELD_ENTRY_INVALID;
                    tempAct.fld_ext[fldIndex].fld_lsb = 0;
                    tempAct.fld_ext[fldIndex].nof_bits = actBits - totalBits;
                    fldIndex++;
                }
            } else { /* if (index < BCM_FIELD_ACTION_EXTRACTION_MAX) */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG_NO_UNIT("action requires %d bits but"
                                                   " total extraction came to %d"
                                                   " bits and no extraciton left"
                                                   " to pad appropriately"),
                                  actBits,
                                  totalBits));
            } /* if (index < BCM_FIELD_ACTION_EXTRACTION_MAX) */
        } /* if (totalBits < actBits) */
        tempAct.nof_fields = fldIndex;
        tempAct.base_val = action->bias & ((1 << actBits) - 1);
        for (/* carries state from previous loop */;
             index < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS;
             index++) {
            tempAct.fld_ext[fldIndex].cst_val = 0;
            tempAct.fld_ext[fldIndex].nof_bits = 0;
            tempAct.fld_ext[fldIndex].fld_lsb = 0;
            tempAct.fld_ext[fldIndex].type = BCM_FIELD_ENTRY_INVALID;
        }
        /* got through all that, now look for a place to add the action */
        for (index = 0, offset = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxActs;
             index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxActs;
             index++) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.get(unit, entry, index, &entryDe_bcmType));
            if ((BCM_FIELD_ENTRY_INVALID <= entryDe_bcmType) &&
                (offset >= _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxActs)) {
                /* first free one noticed so far */
                offset = index;
            }
            if (action->action == entryDe_bcmType) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.get(unit, entry, index, &entryDe_hwParam));
                if (tempAct.type == entryDe_hwParam.type) {
                    /* already have this action here */
                    break;
                }
            }
        }
        if (index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxActs) {
#if _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE
            /* overwrite the existing same action */
            offset = index;
#else  /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d already has"
                                               " action %d (%s); remove it first"),
                              unit,
                              entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                              action->action,
                              _bcm_dpp_field_action_name[action->action]));
#endif /* _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE */
        }
        /* set the action with the BCM action type and the constructed HW descr. */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.set(unit, entry, offset, action->action));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.set(unit, entry, offset, &tempAct));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_dir_ext_action_get
 *  Purpose
 *    Get a direct extraction action from a direct extraction entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = direct extraction entry ID
 *    (in) action = pointer to direct extraction action descriptionm
 *  Returns
 *    int (implied cast from bcm_error_t)
 *      BCM_E_NONE if successful
 *      BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_dpp_field_dir_ext_action_get(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  bcm_field_extraction_action_t *action,
                                  int maxCount,
                                  bcm_field_extraction_field_t *extraction,
                                  int *actualCount)
{
    unsigned int index;
    unsigned int offset;
    unsigned int extractStart = 0;
    unsigned int extractIndex;
    unsigned int dqIndex = 0;
    SOC_PPC_FP_QUAL_TYPE qual;
    uint8 bit_value ;
    bcm_field_aset_t groupD_aset;
    _bcm_dpp_field_stage_idx_t groupD_stage;

    uint32 entryDe_entryFlags;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    bcm_field_action_t entryDe_bcmType;
    uint32 entryDe_nof_fields;
    uint32 entryDe_base_val;
    uint32 entryDe_cst_val;
    uint32 entryDe_fld_lsb;
    uint32 entryDe_nof_bits;
    SOC_PPC_FP_QUAL_TYPE entryDe_fld_ext_type;
    bcm_field_qualify_t ppdQual;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > action->action) || (bcmFieldActionCount <= action->action || (BCM_FIELD_ENTRY_INVALID == action->action))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action %d is not valid"),
                          action->action));
    }
    /*
     * Was:
     *   if (!BCM_FIELD_ASET_TEST(unitData->unitAset, action->action))
     * Which was equivalent to:
     *   if (!SHR_BITGET((unitData->unitAset.w), action->action))
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_UNITASET_W.bit_get(unit,action->action,&bit_value)) ;
    if (!bit_value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support action"
                                           " %d (%s)"),
                          unit,
                          action->action,
                          _bcm_dpp_field_action_name[action->action]));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.aset.get(unit, entryDe_entryGroup, &groupD_aset));
    if (!BCM_FIELD_ASET_TEST(groupD_aset,
                             action->action)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d) aset"
                                           " does not include %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          entryDe_entryGroup,
                          action->action,
                          _bcm_dpp_field_action_name[action->action]));
    }
    if ((0 < maxCount) && (!extraction)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("if max count positive, extractions"
                                           " pointer must not be NULL")));
    }
    if (!actualCount) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("obligatory OUT pointer argument"
                                           " actual count must not be NULL")));
    }

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryDe_entryGroup, &groupD_stage));

    *actualCount = 0;

    for (index = 0; index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxActs; index++) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.get(unit, entry, index, &entryDe_bcmType));
        if (action->action != entryDe_bcmType) {
            continue;
        }

        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryDe_entryGroup, &groupD_stage));

        if (index >= _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxActs) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                               " action %d (%s)"),
                              unit,
                              entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                              action->action,
                              _bcm_dpp_field_action_name[action->action]));
        }

        bcm_field_extraction_action_t_init(action);
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.get(unit, entry, index, &action->action));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.nof_fields.get(unit, entry, index, &entryDe_nof_fields));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.base_val.get(unit, entry, index, &entryDe_base_val));
        action->bias = entryDe_base_val;
        if (0 < maxCount) {
            for (offset=0, extractIndex=extractStart; (offset < entryDe_nof_fields)&&(offset < maxCount); offset++, extractIndex++) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.nof_bits.get(unit, entry, index, offset, &entryDe_nof_bits));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.fld_lsb.get(unit, entry, index, offset, &entryDe_fld_lsb));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.cst_val.get(unit, entry, index, offset, &entryDe_cst_val));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.type.get(unit, entry, index, offset, &entryDe_fld_ext_type));

                if (!_BCM_DPP_FIELD_PPD_QUAL_VALID(entryDe_fld_ext_type)) {
                    if ((offset == 0)
                        && (extractStart>0)
                        && (extraction[extractStart-1].flags == BCM_FIELD_EXTRACTION_FLAG_CONSTANT)) {
                        /* compound action merge it with previous extraction */
                        --extractIndex;

                        extraction[extractIndex].value |= (entryDe_cst_val & ((1 << entryDe_nof_bits) - 1)) << extraction[extractIndex].bits;
                        extraction[extractIndex].bits += entryDe_nof_bits;
                    }
                    else {
                        extraction[extractIndex].bits = entryDe_nof_bits;
                        extraction[extractIndex].lsb = entryDe_fld_lsb;
                        extraction[extractIndex].value = entryDe_cst_val & ((1 << entryDe_nof_bits) - 1);
                        extraction[extractIndex].qualifier = BCM_FIELD_ENTRY_INVALID;
                        extraction[extractIndex].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
                    }
                } else { /* if (constant value used at PPD layer) */
                    qual = entryDe_fld_ext_type;
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.ppdQual.get(unit, qual, &ppdQual));
                    if ((bcmFieldQualifyCount > ppdQual) && (BCM_FIELD_ENTRY_INVALID != ppdQual)) {
                        if ((offset == 0)
                            && (extractStart>0)
                            && (extraction[extractStart-1].qualifier == ppdQual)
                            && ((extraction[extractStart-1].lsb+extraction[extractStart-1].bits) == entryDe_fld_lsb)) {
                            /* compound action merge it with previous extraction */
                            --extractIndex;
                            extraction[extractIndex].bits += entryDe_nof_bits;
                        }
                        else {
                            extraction[extractIndex].qualifier = ppdQual;
                            extraction[extractIndex].bits = entryDe_nof_bits;
                            extraction[extractIndex].lsb = entryDe_fld_lsb;
                            extraction[extractIndex].value = entryDe_cst_val & ((1 << entryDe_nof_bits) - 1);
                            extraction[extractIndex].flags = 0;
                        }
                    } else {
                        /* no PPD -> BCM translation; try data_qual */
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                                                qual,
                                                                                &dqIndex),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to"
                                                              " get BCM qual"
                                                              " mapping for"
                                                              " hardware qual"
                                                              " %d (%s)"),
                                             unit,
                                             qual,
                                             SOC_PPC_FP_QUAL_TYPE_to_string(qual)));
                        if ((offset == 0)
                            && (extractStart>0)
                            && (extraction[extractStart-1].flags == BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD)
                            && (extraction[extractStart-1].qualifier == dqIndex)
                            && ((extraction[extractStart-1].lsb+extraction[extractStart-1].bits) == entryDe_fld_lsb)) {
                            /* compound action merge it with previous extraction */
                            --extractIndex;
                            extraction[extractIndex].bits += entryDe_nof_bits;
                        }
                        else {
                            extraction[extractIndex].qualifier = dqIndex;
                            extraction[extractIndex].bits = entryDe_nof_bits;
                            extraction[extractIndex].lsb = entryDe_fld_lsb;
                            extraction[extractIndex].value = entryDe_cst_val & ((1 << entryDe_nof_bits) - 1);
                            extraction[extractIndex].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
                        }
                    } /* if (PPD qual maps to BCM qual) */
                } /* if (constant value used at PPD layer) */
            }

            extractStart = extractIndex;
            /* in this case, actualCount reflects actual number populated */
            *actualCount = extractIndex;
        } else {
            /* in this case, actualCount reflects actual number needed */
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.nof_fields.get(unit, entry, index, &entryDe_nof_fields));
            *actualCount += entryDe_nof_fields;
        }
    }

    if (*actualCount == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                           " action %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          action->action,
                          _bcm_dpp_field_action_name[action->action]));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_hardware_install
 *  Purpose
 *     Insert a Soc_petra PPD entry based upon the BCM field entry information
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
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
_bcm_dpp_field_dir_ext_entry_hardware_install(bcm_dpp_field_info_OLD_t *unitData,
                                              _bcm_dpp_field_ent_idx_t entry)
{
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entryInfo = NULL;
    uint32 ppdr;
    uint32 oldFlags;
    uint32 oldHwHandle;
    int newHwHandle;
    SOC_SAND_SUCCESS_FAILURE success;
    unsigned int index;
    unsigned int offset;
    int result;
    int auxRes;
    _bcm_dpp_field_grp_idx_t group;
    SOC_PPC_FP_QUAL_TYPE entryDe_hwType;
    uint64 entryDe_qualMask;
    uint64 entryDe_qualData;
    bcm_field_qualify_t entryDe_qualType;
    SOC_PPC_FP_DIR_EXTR_ACTION_VAL entryDe_hwParam;
    bcm_field_action_t entryDe_bcmType;
    uint32 entryDe_entryFlags;
    bcm_field_qualify_t ppdQual;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &group));

    /* keep old flags and handle around */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &oldFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, entry, &oldHwHandle));

    /* pick a new hardware entry handle */
    BCMDNX_IF_ERR_EXIT_MSG(dpp_am_res_alloc(unit,
                                        BCM_DPP_AM_DEFAULT_POOL_IDX,
                                        dpp_am_res_field_direct_extraction_entry_id,
                                        0 /* flags */,
                                        1 /* count */,
                                        &newHwHandle),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate hardware"
                                          " handle for entry %d"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));

    /* build entry data from sw state */
    BCMDNX_ALLOC(entryInfo, sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO), "_bcm_dpp_field_dir_ext_entry_hardware_install.entryInfo");
    if (entryInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(entryInfo);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwPriority.get(unit, entry, &entryInfo->priority));
    for (index = 0; index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; index++) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, entry, index, &entryDe_hwType));  
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entryDe_hwType)) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, entry, index, &entryDe_hwType)); 
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.qualData.get(unit, entry, index, &entryDe_qualData)); 
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.qualMask.get(unit, entry, index, &entryDe_qualMask)); 
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.qualType.get(unit, entry, index, &entryDe_qualType)); 
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d qual %d(%s)->%d"
                                  " %08X%08X/%08X%08X\n"),
                       unit,
                       entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                       entryDe_qualType,
                       (BCM_FIELD_ENTRY_INVALID != entryDe_qualType)?_bcm_dpp_field_qual_name[entryDe_qualType]:"-",
                       entryDe_hwType,
                       COMPILER_64_HI(entryDe_qualData),
                       COMPILER_64_LO(entryDe_qualData),
                       COMPILER_64_HI(entryDe_qualMask),
                       COMPILER_64_LO(entryDe_qualMask)));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, entry, index, &entryDe_hwType)); 
            entryInfo->qual_vals[index].type = entryDe_hwType;
            /* SOC_SAND_U64 is a coerced 32b grain little-endian 64b number */
            entryInfo->qual_vals[index].val.arr[0] = COMPILER_64_LO(entryDe_qualData);
            entryInfo->qual_vals[index].val.arr[1] = COMPILER_64_HI(entryDe_qualData);
            entryInfo->qual_vals[index].is_valid.arr[0] = COMPILER_64_LO(entryDe_qualMask);
            entryInfo->qual_vals[index].is_valid.arr[1] = COMPILER_64_HI(entryDe_qualMask);
        } /* if (entryDe_hwType < bcmFieldQualifyCount) */
    } /* for (all qualifiers) */
    for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.get(unit, entry, index, &entryDe_hwParam));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.get(unit, entry, index, &entryDe_bcmType));
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entryDe_hwParam.type)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d action %d(%s)->%d: bias %08X, count %d\n"),
                       unit,
                       entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                       entryDe_bcmType,
                       _bcm_dpp_field_action_name[entryDe_bcmType],
                       entryDe_hwParam.type,
                       entryDe_hwParam.base_val,
                       entryDe_hwParam.nof_fields));
            entryInfo->actions[index].base_val = entryDe_hwParam.base_val;
            entryInfo->actions[index].nof_fields = entryDe_hwParam.nof_fields;
            entryInfo->actions[index].type = entryDe_hwParam.type;
            for (offset = 0;
                 offset < entryDe_hwParam.nof_fields;
                 offset++) {
                if (entryDe_hwParam.fld_ext[offset].cst_val) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d entry %d action %d(%s)->%d: constant %08X, low %d bits\n"),
                               unit,
                               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                               entryDe_bcmType,
                               _bcm_dpp_field_action_name[entryDe_bcmType],
                               entryDe_hwParam.type,
                               entryDe_hwParam.fld_ext[offset].cst_val,
                               entryDe_hwParam.fld_ext[offset].nof_bits));
                    entryInfo->actions[index].fld_ext[offset].cst_val = entryDe_hwParam.fld_ext[offset].cst_val;
                    entryInfo->actions[index].fld_ext[offset].nof_bits = entryDe_hwParam.fld_ext[offset].nof_bits;
                } else { /* if (extraction is using a constant) */
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.ppdQual.get(unit, entryDe_hwParam.fld_ext[offset].type, &ppdQual));
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d entry %u action %d(%s)->%d: qual %d(%s)->%d, bits %d..%d\n"),
                               unit,
                               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                               entryDe_bcmType,
                               _bcm_dpp_field_action_name[entryDe_bcmType],
                               entryDe_hwParam.type,
                               ppdQual,
                               (BCM_FIELD_ENTRY_INVALID != ppdQual ? _bcm_dpp_field_qual_name[ppdQual] : "-"),
                               entryDe_hwParam.fld_ext[offset].type,
                               entryDe_hwParam.fld_ext[offset].fld_lsb + entryDe_hwParam.fld_ext[offset].nof_bits - 1,
                               entryDe_hwParam.fld_ext[offset].fld_lsb));
                    entryInfo->actions[index].fld_ext[offset].fld_lsb = entryDe_hwParam.fld_ext[offset].fld_lsb;
                    entryInfo->actions[index].fld_ext[offset].nof_bits = entryDe_hwParam.fld_ext[offset].nof_bits;
                    entryInfo->actions[index].fld_ext[offset].type = entryDe_hwParam.fld_ext[offset].type;
                } /* if (extraction is using a constant) */
            } /* for (all fields in this action) */
        } /* if (SOC_PPC_NOF_FP_ACTION_TYPES > entryDe_hwParam.type) */
    } /* for (all actions) */

    /* add the entry to hardware */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "add unit %d entry %d to hardware with handle %d\n"),
               unit,
               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               newHwHandle));
    ppdr = soc_ppd_fp_direct_extraction_entry_add(unitData->unitHandle,
                                                  group,
                                                  newHwHandle,
                                                  entryInfo,
                                                  &success);
    result = handle_sand_result(ppdr);
    if (BCM_E_NONE == result) {
        result = translate_sand_success_failure(success);
    }
    if (BCM_E_NONE != result) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "\r\n  ==> unable to add entry %d as hardware"
                              " entry %d: %d (%s)\r\n"),
                   entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                   newHwHandle,
                   result,
                   _SHR_ERRMSG(result)));
        auxRes = dpp_am_res_free(unit,
                                BCM_DPP_AM_DEFAULT_POOL_IDX,
                                dpp_am_res_field_direct_extraction_entry_id,
                                1 /* count */,
                                newHwHandle);
        if(auxRes != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "dpp_am_res_free Failed\n")));
        }
        BCM_ERR_EXIT_NO_MSG(result);
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.set(unit, entry, newHwHandle));
    /* entry is now in hardware */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    entryDe_entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_HW;
    /* entry is no longer changed since insert & no need to update */
    entryDe_entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_CHANGED |
                                  _BCM_DPP_FIELD_ENTRY_UPDATE));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags));
#ifdef BCM_WARM_BOOT_SUPPORT
    /* update backing store */
    /*_bcm_dpp_field_dir_ext_entry_wb_save(unitData, entry, NULL, NULL);*/
#endif /* def BCM_WARM_BOOT_SUPPORT */

    /* remove from hardware if it is already installed */
    if (oldFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "remove unit %d entry %d old copy from"
                              " hardware with handle %d\n"),
                   unit,
                   entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                   oldHwHandle));
        ppdr = soc_ppd_fp_direct_extraction_entry_remove(unitData->unitHandle,
                                                         group,
                                                         oldHwHandle);
        BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(ppdr),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to remove"
                                              " old entry %d from hardware"
                                              " as hardware entry %d"),
                             unit,
                             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                             oldHwHandle));
        BCMDNX_IF_ERR_EXIT_MSG(dpp_am_res_free(unit,
                                BCM_DPP_AM_DEFAULT_POOL_IDX,
                                dpp_am_res_field_direct_extraction_entry_id,
                                1 /* count */,
                                oldHwHandle),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable to return"
                                              " hardware handle %d to"
                                              " free list"),
                             unit,
                             oldHwHandle));
    } /* if (oldFlags & _BCM_PETRA_FIELD_ENTRY_IN_HW) */
exit:
    BCM_FREE(entryInfo);
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  Functions and data shared with other field modules
 */

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_dir_ext_entry_dump(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  const char *prefix)
{
    unsigned int index;
    unsigned int count;
    unsigned int bits;
    unsigned int offset;
    unsigned int dq;
    uint32 mask;
    int result = BCM_E_NONE;
    int dumpedRanges;
    char format[64];
    _bcm_dpp_field_stage_idx_t groupD_stage;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    uint32 entryDe_entryFlags;
    uint32 entryDe_hwHandle;
    int entryDe_entryPriority;
    _bcm_dpp_field_ent_idx_t  entryDe_entryPrev;
    _bcm_dpp_field_ent_idx_t  entryDe_entryNext;
    SOC_PPC_FP_ACTION_TYPE entryDe_type;
    bcm_field_action_t entryDe_bcmType;
    uint32 entryDe_nof_fields;
    uint32 entryDe_cst_val;
    uint32 entryDe_nof_bits;
    SOC_PPC_FP_QUAL_TYPE entryDe_fld_ext_type;
    uint32 entryDe_fld_lsb;
    uint32 entryDe_base_val;
    bcm_field_qualify_t ppdQual;
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    uint32 soc_sandResult;
    uint8 okay;
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
    SOC_PPC_FP_QUAL_VAL *hwQuals;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if ((entryDe_entryFlags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                           _BCM_DPP_FIELD_ENTRY_CHANGED |
                                           _BCM_DPP_FIELD_ENTRY_NEW)) ==
        (_BCM_DPP_FIELD_ENTRY_IN_HW)) {
        /* entry is in hardware and not changed since */
        BCMDNX_ALLOC(entInfoDe, sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO), "_bcm_dpp_field_dir_ext_entry_dump.entInfoDe");
        if (entInfoDe == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
        }
        SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(entInfoDe);
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, entry, &entryDe_hwHandle));
        soc_sandResult = soc_ppd_fp_direct_extraction_entry_get(unitData->unitHandle,
                                                                entryDe_entryGroup,
                                                                entryDe_hwHandle,
                                                                &okay,
                                                                entInfoDe);
        result = handle_sand_result(soc_sandResult);
        if (!okay) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not okay"
                                               " trying to fetch entry from"
                                               " hardware"),
                              unit,
                              entry));
        }
        hwQuals = &(entInfoDe->qual_vals[0]);
    } else {
        /* entry is not in hardware or has changed */
        hwQuals = NULL;
    }
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
    LOG_CLI((BSL_META_U(unit,
                        "%sEntry "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Flags       = %08X\n"), prefix, entryDe_entryFlags));
#if _BCM_DPP_FIELD_DUMP_SYM_FLAGS
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s %s %s %s %s %s %s\n"),
             prefix,
             (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)?"+USED":"-used",
             (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)?"+IN_HW":"-in_hw",
             (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_WANT_HW)?"+WANT_HW":"-want_hw",
             (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_UPDATE)?"+UPD":"-upd",
             (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_CHANGED)?"+CHG":"-chg",
             (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_NEW)?"+NEW":"-new",
             (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_STATS)?"+STA":"-sta"));
#endif /* _BCM_PETRA_FIELD_DUMP_SYM_FLAGS */
    if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        if ((_BCM_DPP_FIELD_ENTRY_IN_HW | _BCM_DPP_FIELD_ENTRY_CHANGED) ==
            (entryDe_entryFlags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                 _BCM_DPP_FIELD_ENTRY_CHANGED))) {
            LOG_CLI((BSL_META_U(unit,
                                "%s    (entry changed since hw write)\n"), prefix));
        }
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Group       = "
                 _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            "\n"),
                 prefix,
                 entryDe_entryGroup));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPriority.get(unit, entry, &entryDe_entryPriority));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Priority    = %8d\n"),
                 prefix,
                 entryDe_entryPriority));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Prev/Next   = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 entryDe_entryPrev + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                 entryDe_entryNext + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
        if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPriority.get(unit, entry, &entryDe_entryPriority));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, entry, &entryDe_hwHandle));
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW entry ID = %08X\n"),
                     prefix,
                     entryDe_hwHandle));
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW priority = %08X\n"),
                     prefix,
                     entryDe_entryPriority));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Qualifiers:\n"), prefix));
        count = 0;
        dumpedRanges = FALSE;
        {
            _bcm_dpp_field_qual_t entryDe_entryQual[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] ;

            /*
             * Load full local array 'qualData' from data base and use as input to _bcm_dpp_field_entry_qual_dump()
             * and _bcm_dpp_field_entry_range_dump()
             */
            for (index = 0;
                 (index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                 index++) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.get(unit, entry, 0, &entryDe_entryQual[index]));
            }
            for (index = 0;
                 (BCM_E_NONE == result) &&
                 (index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                 index++) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryDe_entryGroup, &groupD_stage));
                result = _bcm_dpp_field_entry_qual_dump(unitData,
                                                        groupD_stage,
                                                        &(entryDe_entryQual[0]),
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
        LOG_CLI((BSL_META_U(unit,
                            "%s  Actions:\n"), prefix));
        count = 0;
        for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.type.get(unit, entry, index, &entryDe_type));
            if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entryDe_type)) {
                /* only show action if exists */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.get(unit, entry, index, &entryDe_bcmType));
                /* have not displayed this action yet */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryDe_entryGroup, &groupD_stage));
                result = _bcm_dpp_ppd_action_bits(unit,
                                                  groupD_stage,
                                                  entryDe_type,
                                                  &bits);
                if (BCM_E_NONE != result) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d unable to get bit count"
                                          " for action %d: %d (%s)\n"),
                               unit,
                               entryDe_type,
                               result,
                               _SHR_ERRMSG(result)));
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "%s    %s (%d) (%u bits, LSb first)\n"),
                         prefix,
                         _bcm_dpp_field_action_name[entryDe_bcmType],
                         entryDe_bcmType,
                         bits));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.nof_fields.get(unit, entry, index, &entryDe_nof_fields));
                for (offset = 0;
                     offset < entryDe_nof_fields;
                     offset++) {
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.cst_val.get(unit, entry, index, offset, &entryDe_cst_val));
                    if (entryDe_cst_val) {
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.nof_bits.get(unit, entry, index, offset, &entryDe_nof_bits));
                        sal_snprintf(format,
                                     sizeof(format),
                                     "%%s      Constant, %%u bits,"
                                     " value %%0%dX\n",
                                     (entryDe_nof_bits + 3) >> 2);
                        mask = (1 << entryDe_nof_bits) - 1;
                        LOG_CLI((format,
                                 prefix,
                                 entryDe_nof_bits,
                                 entryDe_cst_val & mask));
                    } else {
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.type.get(unit, entry, index, offset, &entryDe_fld_ext_type));
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.ppdQual.get(unit, entryDe_fld_ext_type, &ppdQual));
                        if ((bcmFieldQualifyCount > ppdQual) && (BCM_FIELD_ENTRY_INVALID != ppdQual)) {
                            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.nof_bits.get(unit, entry, index, offset, &entryDe_nof_bits));
                            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.fld_lsb.get(unit, entry, index, offset, &entryDe_fld_lsb));
                            /* standard qualifier */
                            LOG_CLI((BSL_META_U(unit,
                                                "%s      %s (%d), bits %u..%u (%u bits)\n"),
                                     prefix,
                                     _bcm_dpp_field_qual_name[ppdQual],
                                     ppdQual,
                                     entryDe_nof_bits + entryDe_fld_lsb - 1,
                                     entryDe_fld_lsb,
                                     entryDe_nof_bits));
                        } else {
                            /* programmable qualifier */
                            result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                                         entryDe_fld_ext_type,
                                                                         &dq);
                            BCMDNX_IF_ERR_EXIT(result);
                            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.nof_bits.get(unit, entry, index, offset, &entryDe_nof_bits));
                            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.fld_lsb.get(unit, entry, index, offset, &entryDe_fld_lsb));
                            LOG_CLI((BSL_META_U(unit,
                                                "%s      Data qualifier %d, bits %u..%u (%u bits)\n"),
                                     prefix,
                                     dq,
                                     entryDe_nof_bits + entryDe_fld_lsb - 1,
                                     entryDe_fld_lsb,
                                     entryDe_nof_bits));
                        }
                    }
                } /* for (all fields in this action) */
                mask = (1 << bits) - 1;
                sal_snprintf(format,
                             sizeof(format),
                             "%%s      Overall bias = %%0%dX\n",
                             (bits + 3) >> 2);
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.base_val.get(unit, entry, index, &entryDe_base_val));
                LOG_CLI((format,
                         prefix,
                         entryDe_base_val & mask));
                count++;
            } /* if (action exists) */
        } /* for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) */
        if (0 == count) {
            /* nothing in qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s    (none; a match will not affect traffic)\n"),
                     prefix));
        }
        BCM_EXIT;
    } else {/* if (entryDe_entryFlags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
        LOG_CLI((BSL_META_U(unit,
                            "%s  (entry is not in use)\n"), prefix));
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    } /* if (entryDe_entryFlags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
exit:
    BCM_FREE(entInfoDe);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

int
_bcm_dpp_field_dir_ext_entry_action_clear(bcm_dpp_field_info_OLD_t *unitData,
                                          _bcm_dpp_field_ent_idx_t entry,
                                          bcm_field_action_t type)
{
    unsigned int index;
    unsigned int offset;
    uint32 groupD_groupFlags;
    bcm_field_aset_t groupD_aset;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    uint32 entryDe_entryFlags;
    bcm_field_action_t entryDe_bcmType;
    _bcm_dpp_field_de_act_t entryDe_deAct;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.aset.get(unit, entryDe_entryGroup, &groupD_aset));
    if (!BCM_FIELD_ASET_TEST(groupD_aset, type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d (group %d ASET"
                                           " does not include %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit,DirExt),
                          entryDe_entryGroup,
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
    for (index = 0;
         SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX > index;
         index++) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.get(unit, entry, index, &entryDe_bcmType));
        if (type == entryDe_bcmType) {
            sal_memset(&(entryDe_deAct),
                       0x00,
                       sizeof(entryDe_deAct));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.set(unit, entry, index, &entryDe_deAct));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.set(unit, entry, index, BCM_FIELD_ENTRY_INVALID));
            /*BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.set(unit, entry, index, bcmFieldActionCount));*/
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.type.set(unit, entry, index, SOC_PPC_FP_ACTION_TYPE_INVALID));
            for (offset = 0; offset < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS; offset++) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.type.set(unit, entry, index, offset, BCM_FIELD_ENTRY_INVALID));
            }
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
            entryDe_entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags));

            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, entryDe_entryGroup, &groupD_groupFlags));
            groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.set(unit, entryDe_entryGroup, groupD_groupFlags));
        }
    }
    if (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX <= index) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have"
                                           " action %d (%s)"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                          type,
                          _bcm_dpp_field_action_name[type]));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_clear_acts(bcm_dpp_field_info_OLD_t *unitData,
                                        _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_de_act_t entryDe_deAct;
    unsigned int index;
    unsigned int offset;
    uint32 groupD_groupFlags;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    uint32 entryDe_entryFlags;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    
    sal_memset(&entryDe_deAct,
               0x00,
               sizeof(entryDe_deAct));
    for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.set(unit, entry, index, &entryDe_deAct));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.set(unit, entry, index, BCM_FIELD_ENTRY_INVALID));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.type.set(unit, entry, index, SOC_PPC_FP_ACTION_TYPE_INVALID));
        for (offset = 0; offset < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS; offset++) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.type.set(unit, entry, index, offset, BCM_FIELD_ENTRY_INVALID));
        }
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    entryDe_entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, entryDe_entryGroup, &groupD_groupFlags));
    groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.set(unit, entryDe_entryGroup, groupD_groupFlags));
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_dir_ext_entry_clear(bcm_dpp_field_info_OLD_t *unitData,
                                   _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    _bcm_dpp_field_entry_dir_ext_t *entryData=NULL;
    unsigned int index;
    unsigned int offset;
    int unit = unitData->unit;
    BCMDNX_INIT_FUNC_DEFS;

    FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &next);
    FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &prev);
    BCMDNX_ALLOC_AND_CLEAR(entryData, sizeof(*entryData), "_bcm_dpp_field_dir_ext_entry_clear.entryDe");
    FIELD_ACCESS.entryDe.set(unit, entry, entryData);
    FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entry, next);
    FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entry, prev);
    FIELD_ACCESS.entryDe.entryCmn.entryPriority.set(unit, entry, BCM_FIELD_ENTRY_PRIO_DEFAULT);
    for (index = 0; index < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; index++) {
        FIELD_ACCESS.entryDe.entryCmn.entryQual.qualType.set(unit, entry, index, BCM_FIELD_ENTRY_INVALID); 
        FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.set(unit, entry, index, BCM_FIELD_ENTRY_INVALID); 
    }
    for (index = 0; index < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; index++) {
        FIELD_ACCESS.entryDe.deAct.bcmType.set(unit, entry, index, BCM_FIELD_ENTRY_INVALID);
        FIELD_ACCESS.entryDe.deAct.hwParam.type.set(unit, entry, index, SOC_PPC_FP_ACTION_TYPE_INVALID);
        for (offset = 0; offset < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS; offset++) {
            FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.type.set(unit, entry, index, offset, BCM_FIELD_ENTRY_INVALID);
        }
    }

exit:
    BCM_FREE(entryData);
    BCMDNX_FUNC_RETURN_VOID;
}

int
_bcm_dpp_field_dir_ext_entry_alloc(bcm_dpp_field_info_OLD_t *unitData,
                                   _bcm_dpp_field_grp_idx_t group,
                                   int withId,
                                   _bcm_dpp_field_ent_idx_t *entry)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_entry_type_t entryType;
    _bcm_dpp_field_ent_idx_t entryDeElems;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    _bcm_dpp_field_stage_idx_t entryDeSh1;
    unsigned int entryCount;
    uint32 hwEntryDeLimit;
    uint32 groupFlags;
    bcm_field_group_mode_t grpMode; 
    _bcm_dpp_field_stage_idx_t stage; 
    _bcm_dpp_field_ent_idx_t groupD_entryCount;
    uint32 entryDe_entryFlags;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &stage));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, group, &grpMode));

    if (0 == (groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %d is not in use"),
                          unit,
                          group));
    }

    FIELD_ACCESS.stageD.modeBits.entryType.get(unit, stage, grpMode, &entryType);
    if (_bcmDppFieldEntryTypeDirExt != entryType) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %d is not a"
                                           " direct extraction group"),
                          unit,
                          group));
    }

    FIELD_ACCESS.stageD.entryDeSh1.get(unit, stage, &entryDeSh1);
    FIELD_ACCESS.stageD.entryDeElems.get(unit, entryDeSh1, &entryDeElems);
    FIELD_ACCESS.stageD.modeBits.entryCount.get(unit, stage, grpMode, &entryCount);
    FIELD_ACCESS.stageD.hwEntryDeLimit.get(unit, entryDeSh1, &hwEntryDeLimit);

    if ((entryDeElems + entryCount) >= hwEntryDeLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u would have too"
                                           " many direct extraction entries"),
                          unit,
                          stage));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.get(unit, group, &groupD_entryCount));
    if (groupD_entryCount > 7) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %d would have too"
                                           " many entries (max 8)"),
                          unit,
                          group));
    }
    if (withId) {
        next = (*entry);
    } else {
        FIELD_ACCESS.entryDeFree.get(unit, &next);
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d chooses direct extraction entry %u%s\n"),
               unit,
               next + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               withId?" by request":""));

    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);
    if (next >= entryDeLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d has no available direct"
                                           " extraction entries"),
                          unit));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, next, &entryDe_entryFlags));
    if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d direct extraction entry %u"
                                           " is in use"),
                          unit,
                          next + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    *entry = next;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_add_to_group(bcm_dpp_field_info_OLD_t *unitData,
                                          _bcm_dpp_field_grp_idx_t group,
                                          _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_ent_idx_t currEntry;
    _bcm_dpp_field_stage_idx_t entryDeSh1;
    _bcm_dpp_field_ent_idx_t entryDeElems;
    unsigned int entryCount;
    uint32 hwEntryDeLimit;
    _bcm_dpp_field_ent_idx_t entryDeCount;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    uint32 groupFlags;
    bcm_field_group_mode_t grpMode; 
    _bcm_dpp_field_stage_idx_t stage; 
    _bcm_dpp_field_ent_idx_t groupD_entryCount;
    _bcm_dpp_field_ent_idx_t entryHead;
    _bcm_dpp_field_ent_idx_t entryTail;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    uint32 entryDe_entryFlags;
    int entryDe_entryPriority;
    int entryDe_curr_entryPriority;
    _bcm_dpp_field_ent_idx_t  entryDe_entryPrev;
    _bcm_dpp_field_ent_idx_t  entryDe_entryNext;
    int result = BCM_E_NONE;
 
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.get(unit, group, &groupD_entryCount));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &stage));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, group, &grpMode));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &entryHead));

    if (0 == (groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u not in use"),
                          unit,
                          group));
    }
    if (0 == (groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d group %u can not have"
                                           " entries created before setting"
                                           " group actions"),
                          unit,
                          group));
    }
    FIELD_ACCESS.stageD.entryDeSh1.get(unit, stage, &entryDeSh1);
    FIELD_ACCESS.stageD.entryDeElems.get(unit, entryDeSh1, &entryDeElems);
    FIELD_ACCESS.stageD.modeBits.entryCount.get(unit, stage, grpMode, &entryCount);
    FIELD_ACCESS.stageD.hwEntryDeLimit.get(unit, entryDeSh1, &hwEntryDeLimit);
    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);

    if ((entryDeElems + entryCount) > hwEntryDeLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("a new entry in unit %d group %u"
                                           " would exceed stage %u capacity"),
                          unit,
                          group,
                          stage));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
        if (group != entryDe_entryGroup) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not permit"
                                               " entries to migrate from"
                                               " one group to another"),
                              unit));
        }
        /* remove this entry from the group */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
        if (entryDe_entryPrev < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entryDe_entryPrev, entryDe_entryNext));
        } else {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, entryDe_entryGroup, entryDe_entryNext));
        }
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
        if (entryDe_entryNext < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entryDe_entryNext, entryDe_entryPrev));
        } else {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, entryDe_entryGroup, entryDe_entryPrev));
        }
    } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
        /* entry is not in use, so must be coming off the free list */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
        if (entryDe_entryPrev < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entryDe_entryPrev, entryDe_entryNext));
        } else {
            FIELD_ACCESS.entryDeFree.set(unit, entryDe_entryNext);
        }
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
        if (entryDe_entryNext < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entryDe_entryNext, entryDe_entryPrev));
        }
        FIELD_ACCESS.stageD.entryDeElems.set(unit, entryDeSh1, entryDeElems + entryCount);
        FIELD_ACCESS.stageD.entryDeCount.get(unit, entryDeSh1, &entryDeCount);
        FIELD_ACCESS.stageD.entryDeCount.set(unit, entryDeSh1, ++entryDeCount);
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.get(unit, group, &groupD_entryCount));
        groupD_entryCount++;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.set(unit, group, groupD_entryCount));
    } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_IN_USE) */
    /* scan for where this entry goes in the new group */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &entryHead));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPriority.get(unit, entry, &entryDe_entryPriority));
    if (entryHead < entryDeLimit) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPriority.get(unit, entryHead, &entryDe_curr_entryPriority)); 
    } else {
        /*In this case the loop below doesn't run so entryDe_curr_entryPriority is not used*/
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPriority.get(unit, entry, &entryDe_curr_entryPriority));
    }
    for (currEntry = entryHead;
         (currEntry < entryDeLimit) &&
         (0 >= _bcm_dpp_compare_entry_priority(entryDe_entryPriority,
                                               entryDe_curr_entryPriority));
         ) {
        /*
         *  ...as long as there are more entries, and the current entry
         *  priority is greater than or equal to the new entry priority,
         *  examine the next entry in the group.
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, currEntry, &currEntry));
        if (currEntry < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPriority.get(unit, currEntry, &entryDe_curr_entryPriority));
        } else {
            /*End of loop*/
        }
    }
    /* insert the new entry *before* currEntry */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d: inserting entry %d to group %u before"
                          " entry %d\n"),
               unit,
               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               group,
               currEntry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entry, currEntry));
    if (currEntry < entryDeLimit) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, currEntry, &entryDe_entryPrev));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entry, entryDe_entryPrev));
        if (entryDe_entryPrev < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entryDe_entryPrev, entry));
        }
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, currEntry, entry));
    } else { 
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.get(unit, group, &entryTail));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entry, entryTail));
        if (entryTail < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entryTail, entry));
        }
        entryTail = entry;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, group, entryTail));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &entryHead));
    if (currEntry == entryHead) {
        entryHead = entry;
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, group, entryHead));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.set(unit, entry, group));
    /* entry is in use, has been changed since hw commit, and is new/moved */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    entryDe_entryFlags |= _BCM_DPP_FIELD_ENTRY_IN_USE |
                               _BCM_DPP_FIELD_ENTRY_CHANGED ;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags));
    /* group contains changed and added/moved entry/entries */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT |
                  _BCM_DPP_FIELD_GROUP_ADD_ENT;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.set(unit, group, groupFlags));

#ifdef BCM_WARM_BOOT_SUPPORT
    /*_bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);*/
#endif /* def BCM_WARM_BOOT_SUPPORT */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_remove(bcm_dpp_field_info_OLD_t *unitData,
                                    _bcm_dpp_field_ent_idx_t entry)
{
    uint32 ppdr;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup,groupLimit;
    uint32 entryDe_entryFlags;
    uint32 entryDe_hwHandle;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupLimit.get(unit, &groupLimit));
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d direct extraction entry %d"
                                           " is not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW)) {
        /* not in hardware; nothing to do */
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, entry, &entryDe_hwHandle));
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "remove unit %d entry %d old copy from"
                          " hardware with handle %d\n"),
               unit,
               entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
               entryDe_hwHandle));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
    if (groupLimit <= entryDe_entryGroup) {
        BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
    }

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, entry, &entryDe_hwHandle));
    ppdr = soc_ppd_fp_direct_extraction_entry_remove(unitData->unitHandle,
                                                     entryDe_entryGroup,
                                                     entryDe_hwHandle);
    BCMDNX_IF_ERR_EXIT_MSG(handle_sand_result(ppdr),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to remove direct"
                                          " extraction entry %u from hardware"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    entryDe_entryFlags &= (~_BCM_DPP_FIELD_ENTRY_IN_HW);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags));
#ifdef BCM_WARM_BOOT_SUPPORT
    /* update backing store */
    /*_bcm_dpp_field_dir_ext_entry_wb_save(unitData, entry, NULL, NULL);*/
#endif /* def BCM_WARM_BOOT_SUPPORT */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, entry, &entryDe_hwHandle));
    BCMDNX_IF_ERR_EXIT_MSG(dpp_am_res_free(unit,
                                BCM_DPP_AM_DEFAULT_POOL_IDX,
                                dpp_am_res_field_direct_extraction_entry_id,
                                1 /* count */,
                                entryDe_hwHandle),
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to return direct"
                                          " extraction entry hardware ID %d"
                                          " to free list"),
                         unit,
                         entryDe_hwHandle));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_destroy(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_ent_idx_t entryDeFree;
    _bcm_dpp_field_stage_idx_t entryDeSh1;
    _bcm_dpp_field_ent_idx_t entryDeElems;
    _bcm_dpp_field_ent_idx_t entryDeCount;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    _bcm_dpp_field_grp_idx_t group;
    bcm_field_group_mode_t grpMode; 
    _bcm_dpp_field_stage_idx_t stage; 
    _bcm_dpp_field_ent_idx_t groupD_entryCount;
    unsigned int entryCount;
    int result ;
    uint32 entryDe_entryFlags;
    _bcm_dpp_field_ent_idx_t  entryDe_entryPrev;
    _bcm_dpp_field_ent_idx_t  entryDe_entryNext;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &group));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_remove(unitData, entry),
                        (_BSL_BCM_MSG_NO_UNIT("unable to remove unit %d entry %d"
                                          "from hardware"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_clear_acts(unitData,
                                                                entry),
                        (_BSL_BCM_MSG_NO_UNIT("unable to remove all actions from"
                                          " unit %d entry %d"),
                         unit,
                         entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));

    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
    if (entryDe_entryPrev < entryDeLimit) 
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entryDe_entryPrev, entryDe_entryNext));
    else {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.set(unit, group, entryDe_entryNext));

    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
    if (entryDe_entryNext < entryDeLimit) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entryDe_entryNext, entryDe_entryPrev));
    } else {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.set(unit, group, entryDe_entryPrev));
    }
    FIELD_ACCESS.entryDeFree.get(unit, &entryDeFree);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entry, entryDeLimit));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, entry, entryDeFree));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, 0));
    if (entryDeFree < entryDeLimit) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, entryDeFree, entry));
    }
    FIELD_ACCESS.entryDeFree.set(unit, entry);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.get(unit, group, &groupD_entryCount));
    groupD_entryCount--;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryCount.set(unit, group, groupD_entryCount));

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &stage));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.grpMode.get(unit, group, &grpMode));

    FIELD_ACCESS.stageD.entryDeSh1.get(unit, stage, &entryDeSh1);
    FIELD_ACCESS.stageD.modeBits.entryCount.get(unit, stage, grpMode, &entryCount);
    FIELD_ACCESS.stageD.entryDeElems.get(unit, entryDeSh1, &entryDeElems);
    FIELD_ACCESS.stageD.entryDeElems.set(unit, entryDeSh1, entryDeElems - entryCount);

    FIELD_ACCESS.stageD.entryDeCount.get(unit, entryDeSh1, &entryDeCount);
    FIELD_ACCESS.stageD.entryDeCount.set(unit, entryDeSh1, --entryDeCount);
    /*
     * Was:
     *   unitData->entryDeCount--;
     */
    result = FIELD_ACCESS.entryDeCount.get(unit, &entryDeCount) ;
    if (result == BCM_E_NONE) {
        entryDeCount-- ;
        result = FIELD_ACCESS.entryDeCount.set(unit, entryDeCount) ;
    }
    BCMDNX_IF_ERR_EXIT_MSG(
        result,
        (_BSL_BCM_MSG_NO_UNIT("Fail on FIELD_ACCESS.entryDeCount.set/get. unit %d"),
        unit)) ;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_group_hw_refresh(bcm_dpp_field_info_OLD_t *unitData,
                                        bcm_field_group_t group,
                                        int cond)
{
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int updated = 0;
    unsigned int copied = 0;
    unsigned int count = 0;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    _bcm_dpp_field_ent_idx_t entryHead;
    _bcm_dpp_field_ent_idx_t entryTail;
    uint32 groupFlags;
    uint32 entryDe_entryFlags;
    _bcm_dpp_field_ent_idx_t  entryDe_entryPrev;
    _bcm_dpp_field_ent_idx_t  entryDe_entryNext;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));

    if (groupFlags & _BCM_DPP_FIELD_GROUP_ADD_ENT) {
        /* entries have been added or moved; update phase */
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d group %d has entries whose hardware"
                              " priority is not valid/current\n"),
                   unit,
                   group));
        _bcm_dpp_field_group_priority_recalc(unitData, group);
    }
    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags));
    if (groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
        /*
         *  Now odd phase, so priorities are all lower than the previous
         *  state (assuming there was one).  Update tail to head so that
         *  new versions of entries will 'stay' in proper order.
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryTail.get(unit, group, &entryTail)) ;
        if (entryTail < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entryTail, &entryDe_entryPrev));
            for (entry = entryTail;
                 entry < entryDeLimit;
                 entry = entryDe_entryPrev, count++) {
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
                if ((!cond) || (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_UPDATE)) {
                    /* update this entry */
                    if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                        /* entry wants to be in hardware */
                        if ((entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                            (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                            /* entry has changed or is not yet in hardware, add it */
                            updated++;
                            BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                              entry),
                                                (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                                  " to insert"
                                                                  " direct"
                                                                  " extraction"
                                                                  " entry %d into"
                                                                  " hardware"),
                                                 unit,
                                                 entry +
                                                 _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                        }
                    } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
                        /* entry does not want to be in hardware */
                        if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                            /* entry is in hardware, so take it out */
                            updated++;
                            BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_remove(unitData,
                                                                                    entry),
                                                (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                                  " to remove"
                                                                  " direct"
                                                                  " extraction"
                                                                  " entry %d from"
                                                                  " hardware"),
                                                 unit,
                                                 entry +
                                                 _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                        }
                    } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
                } else { /* if (update this entry) */
                    /* no update to this entry... */
                    if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* ...but need to shift it to new priority anyway */
                        copied++;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                          entry),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                              " to insert"
                                                              " direct"
                                                              " extraction"
                                                              " entry %d into"
                                                              " hardware"),
                                             unit,
                                             entry +
                                             _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                    }
                } /* if (update this entry) */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev));
            } /* for (all entries in this group) */
        }
    } else { /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
        /*
         *  Now even phase, so priorities are all higher than the previous
         *  state (assuming there was one).  Update head to tail so that
         *  new versions of entries will 'stay' in proper order.
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.entryHead.get(unit, group, &entryHead));
        if (entryHead < entryDeLimit) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entryHead, &entryDe_entryNext));
        }
        for (entry = entryHead;
             entry < entryDeLimit;
             entry = entryDe_entryNext, count++) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
            if ((!cond) || (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_UPDATE)) {
                /* update this entry */
                if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_WANT_HW) {
                    /* entry wants to be in hardware */
                    if ((entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_CHANGED) ||
                        (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW))) {
                        /* entry has changed or is not yet in hardware, add it */
                        updated++;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                          entry),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                              " to insert"
                                                              " direct"
                                                              " extraction"
                                                              " entry %d into"
                                                              " hardware"),
                                             unit,
                                             entry +
                                             _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                    }
                } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
                    /* entry does not want to be in hardware */
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
                    if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                        /* entry is in hardware, so take it out */
                        updated++;
                        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_remove(unitData,
                                                                                entry),
                                            (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                              " to remove"
                                                              " direct"
                                                              " extraction"
                                                              " entry %d from"
                                                              " hardware"),
                                             unit,
                                             entry +
                                             _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                    }
                } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_WANT_HW) */
            } else { /* if (update this entry) */
                /* no update to this entry... */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
                if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_HW) {
                    /* ...but need to shift it to new priority anyway */
                    copied++;
                    BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                                      entry),
                                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable"
                                                          " to insert"
                                                          " direct"
                                                          " extraction"
                                                          " entry %d into"
                                                          " hardware"),
                                         unit,
                                         entry +
                                         _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
                }
            } /* if (update this entry) */
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
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
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dir_ext_entry_install(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry)
{
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    uint32 entryDe_entryFlags;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_CONTINUE(unit);
    _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_CONTINUE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
    if (0 == (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                          unit,
                          entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    }
        /* entry exists */
    
    entryDe_entryFlags |= (_BCM_DPP_FIELD_ENTRY_WANT_HW |
                         _BCM_DPP_FIELD_ENTRY_UPDATE);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags));
    if (entryDe_entryFlags & _BCM_DPP_FIELD_ENTRY_NEW) {
        /*
         *  This entry is new or has been moved recently, so its
         *  hardware priority is either not assigned or not valid.  We
         *  need to reassign hardware priorities for the group so they
         *  include all of the entries now in the group, in their
         *  current positions.
         *
         *  After reassigning hardware priorities, though, we need to
         *  refresh the entire group in hardware, so first we set this
         *  entry as wanting hardware, recompute the priorities, and
         *  then finally update all of the entries that should be
         *  in hardware.
         *
         *  Since this must not, according to the API spec, affect the
         *  state of entries other than the specified one, there is a
         *  lot of additional work when there are many entries.
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags));
        entryDe_entryFlags |= (_BCM_DPP_FIELD_ENTRY_WANT_HW |
                             _BCM_DPP_FIELD_ENTRY_UPDATE);
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags));
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, entry, &entryDe_entryGroup));
        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_group_hw_refresh(unitData,
                                                                    entryDe_entryGroup,
                                                                    TRUE),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d, group %u"
                                              " refresh failed"),
                             unit,
                             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt),
                             entryDe_entryGroup));
    } else { /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_NEW) */
        /* entry already has priority, so can just add to hardware */
        BCMDNX_IF_ERR_EXIT_MSG(_bcm_dpp_field_dir_ext_entry_hardware_install(unitData,
                                                                          entry),
                            (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d did not"
                                              " install to hardware"),
                             unit,
                             entry + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)));
    } /* if (entryData->flags & _BCM_PETRA_FIELD_ENTRY_NEW) */
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_dir_ext_entry_copy_id(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_ent_idx_t dest,
                                     _bcm_dpp_field_ent_idx_t source)
{
    _bcm_dpp_field_ent_idx_t next;
    _bcm_dpp_field_ent_idx_t prev;
    uint32 entryDe_entryFlags;
    _bcm_dpp_field_entry_dir_ext_t *sourceData = NULL;
    int unit = unitData->unit;
    BCMDNX_INIT_FUNC_DEFS;

    /* preserve previous entry list position for now */
    FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, dest, &prev);
    FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, dest, &next);
    /* copy the old entry to the new entry */
    BCMDNX_ALLOC_AND_CLEAR(sourceData, sizeof(*sourceData), "_bcm_dpp_field_dir_ext_entry_copy_id.entryDe");
    FIELD_ACCESS.entryDe.get(unit, source, sourceData);
    FIELD_ACCESS.entryDe.set(unit, dest, sourceData);
    /* keep new entry in previous entry list for now */
    FIELD_ACCESS.entryDe.entryCmn.entryPrev.set(unit, dest, prev);
    FIELD_ACCESS.entryDe.entryCmn.entryNext.set(unit, dest, next);
    FIELD_ACCESS.entryDe.entryCmn.hwPriority.set(unit, dest, BCM_FIELD_ENTRY_PRIO_DEFAULT);
    /* the copy is not yet in hardware, nor is it yet in use */
    FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, dest, &entryDe_entryFlags);
    entryDe_entryFlags &= (~(_BCM_DPP_FIELD_ENTRY_IN_USE |
                          _BCM_DPP_FIELD_ENTRY_IN_HW |
                          _BCM_DPP_FIELD_ENTRY_WANT_HW));
    FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, dest, entryDe_entryFlags);

exit:
    BCM_FREE(sourceData);
    BCMDNX_FUNC_RETURN_VOID;
}

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
int
_bcm_dpp_field_dir_ext_hardware_entry_check(bcm_dpp_field_info_OLD_t *unitData,
                                            bcm_field_entry_t entry,
                                            SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entInfoDe)
{
    uint32 soc_sandResult;
    uint8 okay;
    unsigned int index;
    unsigned int offset;
    unsigned int j;
    unsigned int bits;
    uint32 mask;
    _bcm_dpp_field_ent_idx_t ent;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    _bcm_dpp_field_stage_idx_t stage;
    int result;
    _bcm_dpp_field_grp_idx_t group;
    uint32 entryDe_hwPriority;
    uint32 entryDe_hwHandle;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    SOC_PPC_FP_QUAL_TYPE entryDe_hwType;
    uint64 entryDe_qualMask = COMPILER_64_INIT(0,0);
    uint64 entryDe_qualData = COMPILER_64_INIT(0,0);
    bcm_field_qualify_t entryDe_qualType = BCM_FIELD_ENTRY_INVALID;
    bcm_field_action_t entryDe_bcmType = BCM_FIELD_ENTRY_INVALID;
    SOC_PPC_FP_ACTION_TYPE entryDe_type;
    uint32 entryDe_nof_fields;
    uint32 entryDe_cst_val;
    uint32 entryDe_nof_bits;
    SOC_PPC_FP_QUAL_TYPE entryDe_fld_ext_type;
    uint32 entryDe_fld_lsb;
    uint32 entryDe_base_val;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);
    if ((_BCM_DPP_FIELD_ENT_BIAS(unit, DirExt) > entry) ||
        ((entryDeLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)) <= entry)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not a"
                                           " direct extraction entry"),
                          unit,
                          entry));
    }
    ent = entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, ent, &group));

    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(entInfoDe);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, ent, &entryDe_entryGroup));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, ent, &entryDe_hwHandle));
    soc_sandResult = soc_ppd_fp_direct_extraction_entry_get(unitData->unitHandle,
                                                            entryDe_entryGroup,
                                                            entryDe_hwHandle,
                                                            &okay,
                                                            entInfoDe);
    result = handle_sand_result(soc_sandResult);
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwHandle.get(unit, ent, &entryDe_hwHandle));
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch"
                                          " entry %u hardware"
                                          " information (%u):"
                                          " %d (%s)"),
                         unit,
                         entry,
                         entryDe_hwHandle,
                         result,
                         _SHR_ERRMSG(result)));
    if (!okay) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %u not okay"
                                           " trying to fetch entry"
                                           " from hardware"),
                          unit,
                          entry));
    }
#if _BCM_DPP_FIELD_GET_VERIFY_PPD
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.hwPriority.get(unit, ent, &entryDe_hwPriority));
    if (entryDe_hwPriority != entInfoDe->priority) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d hardware priority %d does"
                             " not match PPD layer priority %d\n"),
                  unit,
                  entry,
                  entryDe_hwPriority,
                  entInfoDe->priority));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                         (_BSL_BCM_MSG_NO_UNIT(("unit %d entry %d hardware priority"
                                            " %d does not match PPD layer"
                                            " priority %d\n"),
                                           unit,
                                           entry,
                                           entryDe_hwPriority,
                                           entInfoDe->priority));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &stage));
    for (index = 0;
         index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxQuals;
         index++) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, ent, index, &entryDe_hwType));  
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entryDe_hwType)) {
            /* qualifier is in use */
            for (offset = 0;
                 (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) &&
                 (entInfoDe->qual_vals[offset].type !=
                  entryDe_hwType);
                 offset++) {
                /* iterate until found matching qualifier */
            }
            if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                /* found matching qualifier; compare it */
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.qualData.get(unit, ent, index, &entryDe_qualData)); 
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.qualMask.get(unit, ent, index, &entryDe_qualMask));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.qualType.get(unit, ent, index, &entryDe_qualType)); 
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, ent, index, &entryDe_hwType)); 
                if ((COMPILER_64_HI(entryDe_qualData) !=
                     entInfoDe->qual_vals[offset].val.arr[1]) ||
                    (COMPILER_64_LO(entryDe_qualData) !=
                     entInfoDe->qual_vals[offset].val.arr[0]) ||
                    (COMPILER_64_HI(entryDe_qualMask) !=
                     entInfoDe->qual_vals[offset].is_valid.arr[1]) ||
                    (COMPILER_64_LO(entryDe_qualMask) !=
                     entInfoDe->qual_vals[offset].is_valid.arr[0])) {
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
                              (entryDe_qualType !=  BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[entryDe_qualType]:"-",
                              entryDe_qualType,
                              COMPILER_64_HI(entryDe_qualData),
                              COMPILER_64_LO(entryDe_qualData),
                              COMPILER_64_HI(entryDe_qualMask),
                              COMPILER_64_LO(entryDe_qualMask),
                              SOC_PPC_FP_QUAL_TYPE_to_string(entryDe_hwType),
                              entryDe_hwType,
                              offset,
                              SOC_PPC_FP_QUAL_TYPE_to_string(entInfoDe->qual_vals[offset].type),
                              entInfoDe->qual_vals[offset].type,
                              entInfoDe->qual_vals[offset].val.arr[1],
                              entInfoDe->qual_vals[offset].val.arr[0],
                              entInfoDe->qual_vals[offset].is_valid.arr[1],
                              entInfoDe->qual_vals[offset].is_valid.arr[0]));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                       " qualifier %d %s (%d)"
                                                       " %08X%08X/%08X%08X ->"
                                                       " %s (%d) does not match"
                                                       " PPD qualifier %d %s"
                                                       " (%d):"
                                                       " %08X%08X/%08X%08X"),
                                      unit,
                                      entry,
                                      index,
                                      (entryDe_qualType != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[entryDe_qualType]:"-",
                                      entryDe_qualType,
                                      COMPILER_64_HI(entryDe_qualData),
                                      COMPILER_64_LO(entryDe_qualData),
                                      COMPILER_64_HI(entryDe_qualMask),
                                      COMPILER_64_LO(entryDe_qualMask),
                                      SOC_PPC_FP_QUAL_TYPE_to_string(entryDe_hwType),
                                      entryDe_hwType,
                                      offset,
                                      SOC_PPC_FP_QUAL_TYPE_to_string(entInfoDe->qual_vals[offset].type),
                                      entInfoDe->qual_vals[offset].type,
                                      entInfoDe->qual_vals[offset].val.arr[1],
                                      entInfoDe->qual_vals[offset].val.arr[0],
                                      entInfoDe->qual_vals[offset].is_valid.arr[1],
                                      entInfoDe->qual_vals[offset].is_valid.arr[0]));
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
                          (entryDe_qualType != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[entryDe_qualType]:"-",
                          entryDe_qualType,
                          COMPILER_64_HI(entryDe_qualData),
                          COMPILER_64_LO(entryDe_qualData),
                          COMPILER_64_HI(entryDe_qualMask),
                          COMPILER_64_LO(entryDe_qualMask),
                          SOC_PPC_FP_QUAL_TYPE_to_string(entryDe_hwType),
                          entryDe_hwType));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d qualifier"
                                                   " %d %s (%d) %08X%08X"
                                                   "/%08X%08X -> %s (%d) does"
                                                   " not have matching PPD"
                                                   " qualifier"),
                                  unit,
                                  entry,
                                  index,
                                  (entryDe_qualType != BCM_FIELD_ENTRY_INVALID)?_bcm_dpp_field_qual_name[entryDe_qualType]:"-",
                                  entryDe_qualType,
                                  COMPILER_64_HI(entryDe_qualData),
                                  COMPILER_64_LO(entryDe_qualData),
                                  COMPILER_64_HI(entryDe_qualMask),
                                  COMPILER_64_LO(entryDe_qualMask),
                                  SOC_PPC_FP_QUAL_TYPE_to_string(entryDe_hwType),
                                  entryDe_hwType));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
            } /* if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) */
        } /* if (qualifier is in use) */
    } /* for (all possible qualifier slots on this entry) */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, group, &stage));
    for (index = 0;
         index < _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxActs;
         index++) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.type.get(unit, ent, index, &entryDe_type));
        if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entryDe_type)) {
            /* action is in use */
            for (offset = 0;
                 (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) &&
                 (entInfoDe->actions[index].type !=
                  entryDe_type);
                 offset++) {
                /* iterate until found matching action */
            }
            if (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_ppd_action_bits(unit,
                                                                 stage,
                                                                 entryDe_type,
                                                                 &bits));
                mask = (1 << bits) - 1;
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.nof_fields.get(unit, ent, index, &entryDe_nof_fields));
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.bcmType.get(unit, ent, index, &entryDe_bcmType));
                if (entryDe_nof_fields !=
                    entInfoDe->actions[offset].nof_fields) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d action %d %s (%d)"
                                         " -> %d %s (%d) number of fields %d"
                                         " differs from hardware value %d\n"),
                              unit,
                              entry,
                              index,
                              ((entryDe_bcmType < bcmFieldActionCount) && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                              entryDe_bcmType,
                              offset,
                              SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                              entInfoDe->actions[offset].type,
                              entryDe_nof_fields,
                              entInfoDe->actions[offset].nof_fields));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action"
                                                       " %d %s (%d) -> %d %s"
                                                       " (%d) number of fields"
                                                       " %d differs from"
                                                       " hardware value %d"),
                                      unit,
                                      entry,
                                      index,
                                      ((entryDe_bcmType<bcmFieldActionCount) && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                      entryDe_bcmType,
                                      offset,
                                      SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                      entInfoDe->actions[offset].type,
                                      entryDe_nof_fields,
                                      entInfoDe->actions[offset].nof_fields));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                }
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.base_val.get(unit, ent, index, &entryDe_base_val));
                if ((entryDe_base_val & mask) !=
                    (entInfoDe->actions[offset].base_val & mask)) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                    LOG_WARN(BSL_LS_BCM_FP,
                             (BSL_META_U(unit,
                                         "unit %d entry %d action %d %s (%d)"
                                         " -> %d %s (%d) bias %08X differs"
                                         " from hardware value %08X\n"),
                              unit,
                              entry,
                              index,
                              ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                              entryDe_bcmType,
                              offset,
                              SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                              entInfoDe->actions[offset].type,
                              entryDe_base_val,
                              entInfoDe->actions[offset].base_val));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                     (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action"
                                                       " %d %s (%d) -> %d %s"
                                                       " (%d) bias %08X differs"
                                                       " from hardware value"
                                                       " %08X"),
                                      unit,
                                      entry,
                                      index,
                                      ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                      entryDe_bcmType,
                                      offset,
                                      SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                      entInfoDe->actions[offset].type,
                                      entryDe_base_val,
                                      entInfoDe->actions[offset].base_val));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                }
                for (j = 0;
                     j < SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS;
                     j++) {
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.type.get(unit, ent, index, j, &entryDe_fld_ext_type));
                    if (entryDe_fld_ext_type !=
                        entInfoDe->actions[offset].fld_ext[j].type) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d type %d differs from"
                                             " hardware value %d\n"),
                                  unit,
                                  entry,
                                  index,
                                  ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                  entryDe_bcmType,
                                  offset,
                                  SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryDe_fld_ext_type,
                                  entInfoDe->actions[offset].fld_ext[j].type));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " type %d differs"
                                                           " from hardware"
                                                           " value %d"),
                                          unit,
                                          entry,
                                          index,
                                          ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                          entryDe_bcmType,
                                          offset,
                                          SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryDe_fld_ext_type,
                                          entInfoDe->actions[offset].fld_ext[j].type));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.nof_bits.get(unit, ent, index, j, &entryDe_nof_bits));
                    if (entryDe_nof_bits !=
                        entInfoDe->actions[offset].fld_ext[j].nof_bits) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d number of bits %d differs"
                                             " from hardware value %d\n"),
                                  unit,
                                  entry,
                                  index,
                                  ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                  entryDe_bcmType,
                                  offset,
                                  SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryDe_nof_bits,
                                  entInfoDe->actions[offset].fld_ext[j].nof_bits));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " number of bits %d"
                                                           " differs from"
                                                           " hardware"
                                                           " value %d"),
                                          unit,
                                          entry,
                                          index,
                                          ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                          entryDe_bcmType,
                                          offset,
                                          SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryDe_nof_bits,
                                          entInfoDe->actions[offset].fld_ext[j].nof_bits));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.nof_bits.get(unit, ent, index, j, &entryDe_nof_bits));
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.cst_val.get(unit, ent, index, j, &entryDe_cst_val));
                    mask = (1 << entryDe_nof_bits) - 1;
                    if ((entryDe_cst_val &
                         mask) !=
                        (entInfoDe->actions[offset].fld_ext[j].cst_val &
                         mask)) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d constant %08X differs"
                                             " from hardware value %08X\n"),
                                  unit,
                                  entry,
                                  index,
                                  ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                  entryDe_bcmType,
                                  offset,
                                  SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryDe_cst_val,
                                  entInfoDe->actions[offset].fld_ext[j].cst_val));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " constant %08X"
                                                           " differs from"
                                                           " hardware"
                                                           " value %08X"),
                                          unit,
                                          entry,
                                          index,
                                          ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                          entryDe_bcmType,
                                          offset,
                                          SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryDe_cst_val,
                                          entInfoDe->actions[offset].fld_ext[j].cst_val));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.fld_ext.fld_lsb.get(unit, ent, index, j, &entryDe_fld_lsb));
                    if (entryDe_fld_lsb !=
                        entInfoDe->actions[offset].fld_ext[j].fld_lsb) {
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                        LOG_WARN(BSL_LS_BCM_FP,
                                 (BSL_META_U(unit,
                                             "unit %d entry %d action %d %s"
                                             " (%d) -> %d %s (%d) extraction"
                                             " %d least significant bit %d"
                                             " differs from hardware value"
                                             " %d\n"),
                                  unit,
                                  entry,
                                  index,
                                  ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                  entryDe_bcmType,
                                  offset,
                                  SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                  entInfoDe->actions[offset].type,
                                  j,
                                  entryDe_fld_lsb,
                                  entInfoDe->actions[offset].fld_ext[j].fld_lsb));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                        BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d"
                                                           " action %d %s (%d)"
                                                           " -> %d %s (%d)"
                                                           " extraction %d"
                                                           " least significant"
                                                           " bit %d differs"
                                                           " from hardware"
                                                           " value %d"),
                                          unit,
                                          entry,
                                          index,
                                          ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                          entryDe_bcmType,
                                          offset,
                                          SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[offset].type),
                                          entInfoDe->actions[offset].type,
                                          j,
                                          entryDe_fld_lsb,
                                          entInfoDe->actions[offset].fld_ext[j].fld_lsb));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                    }
                } /* for (all possible extractions in this action) */
            } else { /* if (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) */
#if !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.deAct.hwParam.type.get(unit, ent, index, &entryDe_type));
                LOG_WARN(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %d action %d %s (%d) ->"
                                     " %s (%d) does not have matching PPD"
                                     " action\n"),
                          unit,
                          entry,
                          index,
                          ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                          entryDe_bcmType,
                          SOC_PPC_FP_ACTION_TYPE_to_string(entryDe_type),
                          entryDe_type));
#else /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
                BCMDNX_ERR_EXIT_MSG(_BCM_DPP_FIELD_GET_VERIFY_ERROR,
                                 (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d action %d"
                                                   " %s (%d) -> %s (%d) does"
                                                   " not have matching PPD"
                                                   " action\n"),
                                  unit,
                                  entry,
                                  index,
                                  ((entryDe_bcmType<bcmFieldActionCount)  && (entryDe_bcmType != BCM_FIELD_ENTRY_INVALID)) ?_bcm_dpp_field_action_name[entryDe_bcmType]:"-",
                                  entryDe_bcmType,
                                  SOC_PPC_FP_ACTION_TYPE_to_string(entryDe_type),
                                  entryDe_type));
#endif /* !_BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR */
            } /* if (offset < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) */
        } /* if (action is in use) */
    } /* for (all possible action slots on this entry) */
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD */

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */


/******************************************************************************
 *
 *  Functions and data exported to API users
 */

int
bcm_petra_field_direct_extraction_action_add(int unit,
                                             bcm_field_entry_t entry,
                                             bcm_field_extraction_action_t action,
                                             int count,
                                             bcm_field_extraction_field_t *extractions)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_ent_idx_t ent;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    BCMDNX_INIT_FUNC_DEFS;

#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_STOP(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);

    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);
    if ((_BCM_DPP_FIELD_ENT_BIAS(unit, DirExt) <= entry) &&
        ((entryDeLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)) > entry))  {
        /* entry is from the direct extraction range */
        ent = entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
        result = _bcm_dpp_field_dir_ext_action_add(unitData,
                                                   ent,
                                                   &action,
                                                   count,
                                                   extractions);
    } else {
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not valid"
                              " for this function\n"),
                   unit,
                   entry));
    }

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_direct_extraction_action_get(int unit,
                                             bcm_field_entry_t entry,
                                             bcm_field_extraction_action_t *action,
                                             int maxCount,
                                             bcm_field_extraction_field_t *extractions,
                                             int *actualCount)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_ent_idx_t ent;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    BCMDNX_INIT_FUNC_DEFS;
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_DIR_EXT_WB_TEST_MODE_CONTINUE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    _DPP_FIELD_UNIT_LOCK(unitData);

    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);
    if ((_BCM_DPP_FIELD_ENT_BIAS(unit, DirExt) <= entry) &&
        ((entryDeLimit + _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)) > entry))  {
        /* entry is from the direct extraction range */
        ent = entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
        result = _bcm_dpp_field_dir_ext_action_get(unitData,
                                                   ent,
                                                   action,
                                                   maxCount,
                                                   extractions,
                                                   actualCount);
    } else { /* if (entry is valid direct extraction entry) */
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d is not valid"
                              " for this function\n"),
                   unit,
                   entry));
    } /* if (entry is valid direct extraction entry) */

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


