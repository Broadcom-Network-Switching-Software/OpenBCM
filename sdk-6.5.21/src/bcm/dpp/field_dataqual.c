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
 *       'User-defined' or 'data field qualifier' support.
 */

/*
 *  The functions in this module deal with proframmable data qualifiers on PPD
 *  devices.  They are invoked by the main field module when certain things
 *  occur, such as calls that explicitly involve data qualifiers.
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

#include <soc/drv.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/switch.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <shared/bitop.h>

#include <soc/dcmn/dcmn_wb.h>

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
 *  Functions and data shared with other field modules
 */

int
_bcm_petra_field_data_qualifier_get(bcm_dpp_field_info_OLD_t  *unitData,
                                    int qual_id,
                                    bcm_field_data_qualifier_t *qual)
{
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    int result, i;
    uint32 ppdr;
    SOC_PPC_FP_DATABASE_STAGE hwStageId;
    uint8 dataFieldInUse;
    uint8 dataFieldOfsBit;
    uint8 dataFieldLenBit;
    uint8 dataFieldDnxOrderBit;
    bcm_field_qset_t qsetType;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    FIELD_ACCESS.dataFieldInUse.bit_get(unit, qual_id, &dataFieldInUse);
    if (!dataFieldInUse) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d data qualifier %d"
                                           " not in use"),
                          unit,
                          qual_id));
    }
    /* Get from the Hardware the Qualifier */
    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
    control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
    control_ndx.val_ndx = qual_id;
    ppdr = soc_ppd_fp_control_get(unitData->unitHandle,
                                  SOC_CORE_INVALID,
                                  &control_ndx,
                                  &control_info);
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get data"
                                          " qualifier %d: %d (%s)"),
                         unit,
                         qual_id,
                         result,
                         _SHR_ERRMSG(result)));
    hwStageId = control_info.val[4];
    /* Fill in the qualifiers attributes */
    bcm_field_data_qualifier_t_init(qual);
    if (control_info.val[0] != SOC_TMC_NOF_PMF_CE_SUB_HEADERS) { 
        switch (control_info.val[0]) {
        case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_0:
            /* The base header is Header_0, the L2 header is header_1 */
            qual->offset_base = bcmFieldDataOffsetBasePacketStart;
            break;
        case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_1:
            /* L2 (ethernet or similar) */
            qual->offset_base = bcmFieldDataOffsetBaseL2Header;
            break;
            /*
             *  Assumptions about how headers map after this point simply do not
             *  work in general, so not supporting the named header values -- only
             *  supporting the header number...
             */
        case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_2:
            qual->offset_base = bcmFieldDataOffsetBaseFirstHeader;
            break;
        case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_3:
            qual->offset_base = bcmFieldDataOffsetBaseSecondHeader;
            break;
        case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_4:
            qual->offset_base = bcmFieldDataOffsetBaseThirdHeader;
            break;
        case SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_5:
            qual->offset_base = bcmFieldDataOffsetBaseFourthHeader;
            break;
        case SOC_PPC_FP_BASE_HEADER_TYPE_FWD:
            qual->offset_base = bcmFieldDataOffsetBaseForwardingHeader;
            break;
        case SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST:
            qual->offset_base = bcmFieldDataOffsetBaseNextForwardingHeader;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unexpected hardware header"
                                               " type %d"),
                              control_info.val[0]));
        }
    } else {
        int type;
        int qual_valid = 0;
        SOC_PPC_FP_QUAL_TYPE *soc_ppd_qual_ptr;
        bcm_field_qualify_t qualifier;
        
        for (qualifier = bcmFieldQualifySrcIp6; qualifier < bcmFieldQualifyCount; ++qualifier) {
            for (type = 0; type < unitData->devInfo->types; ++type) {
                FIELD_ACCESS.stageD.qsetType.get(unit, hwStageId, type, &qsetType);
                if (BCM_FIELD_QSET_TEST(qsetType, qualifier)) {
                    qual_valid = 1;
                }
            }
          
            if (qual_valid) {
                /* check if we found the right qualifier */
                qual_valid = 0;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_ppd_qual_from_bcm_qual(unitData,hwStageId,0xf,qualifier,&soc_ppd_qual_ptr));				
                if ((soc_ppd_qual_ptr[0] == control_info.val[3]) && ((soc_ppd_qual_ptr[1] == BCM_FIELD_ENTRY_INVALID) || (soc_ppd_qual_ptr[1] == SOC_PPC_NOF_FP_QUAL_TYPES)) ) {
                    break;
                }
            }
        }
        qual->qualifier = qualifier;
        qual->flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED;

    }
    if(control_info.val[1] & SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF_FLAG_NEGATIVE) {
        qual->flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
        control_info.val[1] &= ~SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF_FLAG_NEGATIVE; /* will have the positive value */
    }

    FIELD_ACCESS.dataFieldOfsBit.bit_get(unit, qual_id, &dataFieldOfsBit);
    if (dataFieldOfsBit) {
        /* PPD bits -> BCM bits */
        qual->flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;

        FIELD_ACCESS.dataFieldDnxOrderBit.bit_get(unit, qual_id, &dataFieldDnxOrderBit);
        if (dataFieldDnxOrderBit) {
            qual->flags |= BCM_FIELD_DATA_QUALIFIER_BIT_OFFSET_DNX_ORDER;
        }

        if (!(qual->flags & (BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_BIT_OFFSET_DNX_ORDER))) {
            qual->offset = _BCM_DPP_BIT_NUMBER(control_info.val[1]);
        } else {
        qual->offset = control_info.val[1];
        }		
    } else {
        /* PPD bits -> BCM bytes */
        qual->offset = control_info.val[1] >> 3;
    }

    FIELD_ACCESS.dataFieldLenBit.bit_get(unit, qual_id, &dataFieldLenBit);
    if (dataFieldLenBit) {
        /* PPD bits -> BCM bits */
        qual->flags |= BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        qual->length = control_info.val[2];
    } else {
        /* PPD bits -> BCM bytes */
        qual->length = control_info.val[2] >> 3;
    }

    /* update the stage of the found qualifier */
    _bcm_dpp_bcm_stage_from_ppd_stage(unitData, hwStageId, &qual->stage);

    qual->qual_id = qual_id;

    /* Get qualifier name from id*/
    for (i=0; i<BCM_FIELD_MAX_NAME_LEN; ++i ) {
        result = FIELD_ACCESS.dataQualifierNames.get(unit, qual_id, i,  &(qual->name[i])); 
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_field_data_qualifier_reset(int unit)
{
    uint32 limit = _BCM_PETRA_FIELD_DQ_LIMIT(unit);
    uint32 index;
    uint32 sandResult;
    uint32 success;
    int result;
    int auxRes = BCM_E_NONE;
    uint32 handle = (unit);
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* clear data field definitions */
    for (index = 0; index < limit; index++) {
        SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
        SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
        control_ndx.val_ndx = index;
        control_info.val[0] = 0;
        control_info.val[1] = 0;
        control_info.val[2] = 1;
        sandResult = soc_ppd_fp_control_set(handle,
                                            SOC_CORE_INVALID,
                                            &control_ndx,
                                            &control_info,
                                            &success);
        result = handle_sand_result(sandResult);
        if (BCM_E_NONE == result) {
            result = translate_sand_success_failure(success);
        }
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to clear unit %d data field %d:"
                                   " %d (%s)\n"),
                       unit,
                       index,
                       result,
                       _SHR_ERRMSG(result)));
            auxRes = result;
        }
    } /* for (all data fields) */

    BCMDNX_IF_ERR_EXIT(auxRes);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_field_data_qualifier_init(bcm_dpp_field_info_OLD_t  *unitData)
{
    _bcm_dpp_field_dq_idx_t limit;
    unsigned int index;
    unsigned int offset;
    int ret ;
    int unit = unitData->unit;

    ret = BCM_E_NONE ;
    if (SOC_WARM_BOOT(unit))   /*Warm boot -  skip the update of sw-state resources */
        goto exit ;

    limit = _BCM_PETRA_FIELD_DQ_LIMIT(unit);

    FIELD_ACCESS.dqLimit.set(unit, limit);
    /* Ingress qualifiers, programmable */
    
    for (index = 0; index < limit; index++) {
        /*
         * Was:
         *   SHR_BITSET(unitData->unitQset.udf_map, index);
         */
        ret = FIELD_ACCESS_UNITQSET_UDFMAP.bit_set(unit,index) ;
        if (ret != BCM_E_NONE)
        {
            goto exit ;
        }
        FIELD_ACCESS.stageD.stgQset.udf_map.bit_set(unit, 0, index);
        for (offset = 0; offset < unitData->devInfo->types; offset++) {
            FIELD_ACCESS.stageD.qsetType.udf_map.bit_set(unit, 0, offset, index);
        }
    }
exit:
    return (ret) ;
}

/*
 *  NOTE: PPD USER_DEF_n must map to BCM user defined index n.
 */
int
_bcm_petra_field_map_ppd_udf_to_bcm(bcm_dpp_field_info_OLD_t  *unitData,
                                    SOC_PPC_FP_QUAL_TYPE hwType,
                                    unsigned int *bcmIndex)
{
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(hwType)) {
        *bcmIndex = hwType - SOC_PPC_FP_QUAL_HDR_USER_DEF_0;
    }
    else {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d HW qualifier %d (%s)"
                                           " not valid data qualifier"),
                          unit,
                          hwType,
                          SOC_PPC_FP_QUAL_TYPE_to_string(hwType)));

        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    if (*bcmIndex > dqLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support data"
                                           " qualifier %d (HW qualifier %d (%s))"),
                          unit,
                          *bcmIndex,
                          hwType,
                          SOC_PPC_FP_QUAL_TYPE_to_string(hwType)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  NOTE: BCM user defined index n must map to PPD USER_DEF_n.
 */
int
_bcm_petra_field_map_bcm_udf_to_ppd(bcm_dpp_field_info_OLD_t  *unitData,
                                    unsigned int bcmIndex,
                                    SOC_PPC_FP_QUAL_TYPE *hwType)
{
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);

    if (bcmIndex > dqLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support data"
                                           " qualifier %d"),
                          unit,
                          bcmIndex));
    }
    else {
        *hwType = SOC_PPC_FP_QUAL_HDR_USER_DEF_0 + bcmIndex;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_field_data_qualifier_bits_get(bcm_dpp_field_info_OLD_t  *unitData,
                                         SOC_PPC_FP_QUAL_TYPE hwType,
                                         unsigned int *bits)
{
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    uint32 ppdr;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
    control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
    result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                 hwType,
                                                 &(control_ndx.val_ndx));
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to find mapping for"
                                          " PPD qualifier %d (%s): %d (%s)"),
                         unit,
                         hwType,
                          SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                         result,
                         _SHR_ERRMSG(result)));
    ppdr = soc_ppd_fp_control_get(unitData->unitHandle,
                                  SOC_CORE_INVALID,
                                  &control_ndx,
                                  &control_info);
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to get data"
                                          " qualifier %d: %d (%s)"),
                         unit,
                         control_ndx.val_ndx,
                         result,
                         _SHR_ERRMSG(result)));
    *bits = control_info.val[2];
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BROADCOM_DEBUG
int
_bcm_petra_field_data_qualifier_dump(bcm_dpp_field_info_OLD_t  *unitData,
                                     const char *prefix)
{
    unsigned int index;
    unsigned int count;
    int result = BCM_E_NONE;
    int auxRes = BCM_E_NONE;
    uint8 dataFieldInUse;
    unsigned int dataFieldRefs;
    int unit = unitData->unit;
    bcm_field_data_qualifier_t qual;
    _bcm_dpp_field_dq_idx_t dqLimit;

    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);

    for (index = 0, count = 0;
         (BCM_E_NONE == result) && (index < dqLimit);
         index++) {
        FIELD_ACCESS.dataFieldInUse.bit_get(unit, index, &dataFieldInUse);

        if (dataFieldInUse) {
            result = _bcm_petra_field_data_qualifier_get(unitData,
                                                         index,
                                                         &qual);
            if (BCM_E_NONE == result) {
                FIELD_ACCESS.dataFieldRefs.get(unit, index, &dataFieldRefs);
                FIELD_PRINT(("%sData qual %2d: flags = %08X, refs = %d\n",
                             prefix,
                             index,
                             qual.flags,
                             dataFieldRefs));
				if (qual.flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED) {
					FIELD_PRINT(("%s  Predefined qualifier type = %s\n",
                             prefix,
                             _bcm_dpp_field_qual_name[qual.qualifier]));
				} else {
					FIELD_PRINT(("%s  Header = %s\n",
								 prefix,
								 _bcm_dpp_field_data_offset_base_names[qual.offset_base]));
				}
                FIELD_PRINT(("%s  Offset = %s %d\n",
                             prefix,
                             (qual.flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES)?"bit":"byte",
                             qual.offset));
                FIELD_PRINT(("%s  Length = %d %s%s\n",
                             prefix,
                             qual.length,
                             (qual.flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES)?"bit":"byte",
                             (1 != qual.length)?"s":""));
                count++;
            } else { /* if (BCM_E_NONE == result) */
                auxRes = result;
            } /* if (BCM_E_NONE == result) */
        } /* if (dataFieldInUse) */
    } /* for (all data qualifiers as long as no error) */
    if ((BCM_E_NONE == result) && (0 == count)) {
        FIELD_PRINT(("%s(no data fields in this unit)\n", prefix));
    }
    return auxRes;
}
#endif /* def BROADCOM_DEBUG */


/******************************************************************************
 *
 *  Functions and data exported to API users
 */

/*
 * Function:
 *      bcm_petra_field_data_qualifier_create
 * Purpose:
 *      Create an data/offset based qualifier.
 * Parameters:
 *      unit           - (IN) bcm device.
 *      data_qualifier - (IN) Qualifier descriptor:
 *                           such as packet type, byte offset, etc.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_create(int unit,
                                      bcm_field_data_qualifier_t *data_qualifier)
{
    _DPP_FIELD_COMMON_LOCALS;
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    int qual_id;
    uint32 ppdr;
    int offset, i;
    int dpp_field_unit_lock_was_taken ;
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_FP_DATABASE_STAGE stage;
    uint8 dataFieldInUse;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;
    dpp_field_unit_lock_was_taken = 0 ;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if (!data_qualifier) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory in/out arg is NULL")));
    }
    
    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);

    _DPP_FIELD_UNIT_LOCK(unitData);
    dpp_field_unit_lock_was_taken = 1 ;

    /* get HW stage ID */
    result = _bcm_dpp_ppd_stage_from_bcm_stage(unitData, data_qualifier->stage, &stage);
    /* Since we have a valid default case in this scenairo, then there's no need to fail on E_PARAM (default is used) */
    if (result != BCM_E_PARAM && BCM_FAILURE(result)) {
        BCM_RETURN_VAL_EXIT(result);
    }
    result = BCM_E_NONE;
    control_info.val[4] = stage;

    /* figure out what ID to assign */
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    qual_id = dqLimit;

    if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_WITH_ID) {
        qual_id = data_qualifier->qual_id;
        FIELD_ACCESS.dataFieldInUse.bit_get(unit, qual_id, &dataFieldInUse);
        if ((0 > qual_id) || (dqLimit <= qual_id)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d data qualifier %d is invalid\n"),
                       unit,
                       qual_id));
            result = BCM_E_NOT_FOUND;
        } else if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_REPLACE) {
            /* valid and replacing */
            if (!dataFieldInUse) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d data qualifier %d is not"
                                       " in use\n"),
                           unit,
                           qual_id));
                result = BCM_E_NOT_FOUND;
            }
        } else {
            /* valid and not replacing */
            if (dataFieldInUse) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d data qualifier %d is already"
                                       " in use\n"),
                           unit,
                           qual_id));
                result = BCM_E_EXISTS;
            }
        } /* if (replacing) */
    } else { /* if (caller provided ID) */
        if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_REPLACE) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d requires WITH_ID with REPLACE\n"),
                       unitData->unit));
            result = BCM_E_PARAM;
        } else { /* if (trying to replace without WITH_ID) */
            for (qual_id = 0;
                 (qual_id < dqLimit);
                 qual_id++) {
                /* loop will exit with qual_id equal to first free one */
                FIELD_ACCESS.dataFieldInUse.bit_get(unit, qual_id, &dataFieldInUse);
                if (!dataFieldInUse)
                    break;
            }
            if (dqLimit <= qual_id) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d has no available data qualifiers\n"),
                           unitData->unit));
                result = BCM_E_RESOURCE;
            }
        } /* if (trying to replace without WITH_ID) */
    } /* if (caller provided ID) */

    if (!SOC_IS_JERICHO_PLUS(unit))
    {
        /* bcmFieldQualifyHashValue (KEY_AFTER_HASHING) does not allow a data qualifier, since 80b are copied completely, */
        /* and there is no way to copy a part of it. */
        if (data_qualifier->qualifier == bcmFieldQualifyHashValue && data_qualifier->stage != bcmFieldStageHash) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit, "bcmFieldQualifyHashValue cannot be used as a data qualifier.\n")));
            result = BCM_E_PARAM;
        }
    }

    if (BCM_E_NONE == result) {
        /* build the qualifier information and check parameters */
        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
        control_ndx.val_ndx = qual_id;
        /* Set the qualifiers attributes, no need to verify the success */
        if (!(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED)) {
            if (data_qualifier->stage == bcmFieldStageEgress) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d must use the flag BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED with stage bcmFieldStageEgress.\n"),
                           unit));
                result = BCM_E_PARAM;
            }
            switch (data_qualifier->offset_base) {
            case bcmFieldDataOffsetBasePacketStart:
                /* The base header is Header_0, the L2 header is header_1 */
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_0;
                if(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d can\'t use the flag BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE with base offset bcmFieldDataOffsetBasePacketStart.\n"),
                               unit));
                    result = BCM_E_PARAM;
                }
                break;
            case bcmFieldDataOffsetBaseL2Header:
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_1;
                break;
                /*
                 *  Assumptions about how headers map after this point simply do
                 *  not work in general, so not supporting the named header values
                 *  -- only supporting the header number...
                 */
            case bcmFieldDataOffsetBaseFirstHeader:
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_2;
                break;
            case bcmFieldDataOffsetBaseSecondHeader:
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_3;
                break;
            case bcmFieldDataOffsetBaseThirdHeader:
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_4;
                break;
            case bcmFieldDataOffsetBaseFourthHeader:
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_5;
                break;
            case bcmFieldDataOffsetBaseForwardingHeader:
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_FWD;
                break;
            case bcmFieldDataOffsetBaseNextForwardingHeader:
                control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST;
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d does not support offset type %d\n"),
                           unit,
                           data_qualifier->offset_base));
                result = BCM_E_PARAM;
            }
        } else { /* predefined qualifier */
            uint8 types_chosen = 0;
            SOC_PPC_FP_QUAL_TYPE *soc_ppd_qual_ptr;
            /* Default is to choose all available types (for backward compatibility) */
            uint32 type = 0xf;
            control_ndx.db_id = 1;
            if(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d can\'t use the flag BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE for predefined data qualifiers.\n"),
                           unit));
                result = BCM_E_PARAM;
            }
            if(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_TYPE_L2) {
                types_chosen++;
                type = 0x1;
            } if(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_TYPE_IPV4) {
                types_chosen++;
                type = 0x2;
            } if(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_TYPE_IPV6) {
                types_chosen++;
                type = 0x4;
            } if(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_TYPE_MPLS) {
                types_chosen++;
                type = 0x8;
            }
            if (types_chosen > 1) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unit %d: can\'t use more than one type flags.\n"),
                           unit));
                result = BCM_E_PARAM;
                BCM_RETURN_VAL_EXIT(result);
            }

            result = _bcm_dpp_ppd_qual_from_bcm_qual(unitData, stage, type, data_qualifier->qualifier, &soc_ppd_qual_ptr);
            if (BCM_FAILURE(result)) {
                BCM_RETURN_VAL_EXIT(result);
            }

            if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_echo_with_lem", 0) && data_qualifier->qualifier == bcmFieldQualifyForwardingHeaderOffset)) {
                if ((SOC_PPC_NOF_FP_QUAL_TYPES != soc_ppd_qual_ptr[1]) && ((BCM_FIELD_ENTRY_INVALID != soc_ppd_qual_ptr[1]))) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d does not support complex qualifier type %d for defining a partial qualifier\n"),
                               unit,
                               data_qualifier->qualifier));
                    result = BCM_E_PARAM;
                }
            }
            control_info.val[3] = soc_ppd_qual_ptr[0];
        }
        if (data_qualifier->flags & ~(BCM_FIELD_DATA_QUALIFIER_WITH_ID |
                                      BCM_FIELD_DATA_QUALIFIER_REPLACE |
                                      BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE |
                                      BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES |
                                      BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES |
                                      BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED |
                                      BCM_FIELD_DATA_QUALIFIER_TYPE_L2 |
                                      BCM_FIELD_DATA_QUALIFIER_TYPE_IPV4 |
                                      BCM_FIELD_DATA_QUALIFIER_TYPE_IPV6 |
                                      BCM_FIELD_DATA_QUALIFIER_TYPE_MPLS |
                                      BCM_FIELD_DATA_QUALIFIER_BIT_OFFSET_DNX_ORDER)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d does not support flags %08X\n"),
                       unit,
                       data_qualifier->flags & ~(BCM_FIELD_DATA_QUALIFIER_WITH_ID |
                       BCM_FIELD_DATA_QUALIFIER_REPLACE |
                       BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE |
                       BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES |
                       BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES |
                       BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED |
                       BCM_FIELD_DATA_QUALIFIER_TYPE_L2 |
                       BCM_FIELD_DATA_QUALIFIER_TYPE_IPV4 |
                       BCM_FIELD_DATA_QUALIFIER_TYPE_IPV6 |
                       BCM_FIELD_DATA_QUALIFIER_TYPE_MPLS |
                       BCM_FIELD_DATA_QUALIFIER_BIT_OFFSET_DNX_ORDER
                       )));
            result = BCM_E_PARAM;
        }
        if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES) {
            /* caller bits */
            if (!(data_qualifier->flags & (BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_BIT_OFFSET_DNX_ORDER))) {
                control_info.val[1] = _BCM_DPP_BIT_NUMBER(data_qualifier->offset);
            } else {
                control_info.val[1] = data_qualifier->offset;
            }
        } else {
            /* caller bytes */
            control_info.val[1] = data_qualifier->offset << 3;
        }
        if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES) {
            /* caller bits */
            control_info.val[2] = data_qualifier->length;
        } else {
            /* caller bytes */
            control_info.val[2] = data_qualifier->length << 3;
        }
        if (control_info.val[1] > 504) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d offset %d %s is too great\n"),
                       unit,
                       data_qualifier->offset,
                       (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES)?"bits":"bytes"));
            result = BCM_E_PARAM;
        }
        if (1 > data_qualifier->length) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d only supports positive lengths\n"),
                       unit));
            result = BCM_E_PARAM;
        } else if (control_info.val[2] > 32) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d length %d %s is too great\n"),
                       unit,
                       data_qualifier->length,
                       (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES)?"bits":"bytes"));
            result = BCM_E_PARAM;
        }
        if(data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE) {
            control_info.val[1] |= SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF_FLAG_NEGATIVE;
        }
    } /* if (BCM_E_NONE == result) */
    if (BCM_E_NONE == result) {
        ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                      SOC_CORE_INVALID,
                                      &control_ndx,
                                      &control_info,
                                      &success);
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE == result) {
            result = translate_sand_success_failure(success);
        }
        if (BCM_E_NONE != result) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unable to set unit %d data field"
                                   " %d: %d (%s)\n"),
                       unit,
                       data_qualifier->qual_id,
                       result,
                       _SHR_ERRMSG(result)));
        }
    } /* if (BCM_E_NONE == result) */
    if ((BCM_E_NONE == result) && (!SOC_WARM_BOOT(unit)))   /*If Warm boot -  skip the update of sw-state resources */
    {
        /* account for the data qualifier now being in use */
        FIELD_ACCESS.dataFieldInUse.bit_set(unit, qual_id);
        FIELD_ACCESS.dataFieldRefs.set(unit, qual_id, 0);
        if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES) {
            FIELD_ACCESS.dataFieldOfsBit.bit_set(unit, qual_id);

            if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_BIT_OFFSET_DNX_ORDER) {
                FIELD_ACCESS.dataFieldDnxOrderBit.bit_set(unit, qual_id);
            } else {
                FIELD_ACCESS.dataFieldDnxOrderBit.bit_clear(unit, qual_id);
            }
        } else {
            FIELD_ACCESS.dataFieldOfsBit.bit_clear(unit, qual_id);
            FIELD_ACCESS.dataFieldDnxOrderBit.bit_clear(unit, qual_id);
        }
        if (data_qualifier->flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES) {
            FIELD_ACCESS.dataFieldLenBit.bit_set(unit, qual_id);
        } else {
            FIELD_ACCESS.dataFieldLenBit.bit_clear(unit, qual_id);
        }
        data_qualifier->qual_id = qual_id;

        for (offset = 0; offset < unitData->devInfo->types; offset++) {
            FIELD_ACCESS.stageD.qsetType.udf_map.bit_set(unit, stage, offset, qual_id);
        }

        /* Map between qualifier id and name*/
        for (i=0; i<BCM_FIELD_MAX_NAME_LEN; ++i ) {
            result = FIELD_ACCESS.dataQualifierNames.set(unit, qual_id, i,  (data_qualifier->name[i])); 
            BCMDNX_IF_ERR_EXIT(result);
        }

#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
        _bcm_dpp_field_datafield_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
    } /* if (BCM_E_NONE == result) */

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    dpp_field_unit_lock_was_taken = 0 ;
    BCMDNX_IF_ERR_EXIT(result);
exit:
    if (dpp_field_unit_lock_was_taken)
    {
        _DPP_FIELD_UNIT_UNLOCK(unitData);
    }
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_data_qualifier_destroy
 * Purpose:
 *      Destroy an data/offset based qualifier.
 * Parameters:
 *      unit     - (IN)  bcm device.
 *      qual_id  - (IN)  Data qualifier id.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_destroy(int unit, int qual_id)
{
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    uint32 ppdr;
    SOC_SAND_SUCCESS_FAILURE success;
    uint8 dataFieldInUse;
    unsigned int dataFieldRefs;
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);

    if ((0 > qual_id) || (dqLimit <= qual_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG("data qualifier %d invalid, dqLimit %d"),
                          qual_id, dqLimit));
    }
    
    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);

    _DPP_FIELD_UNIT_LOCK(unitData);
    /* make sure the qualifier is in use */
    FIELD_ACCESS.dataFieldInUse.bit_get(unit, qual_id, &dataFieldInUse);
    if (dataFieldInUse) {
        FIELD_ACCESS.dataFieldRefs.get(unit, qual_id, &dataFieldRefs);
        if (dataFieldRefs) {
            /* still in use by at least one group */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d data field %d in use by %d"
                                   " group%s\n"),
                       unit,
                       qual_id,
                       dataFieldRefs,
                       (1 != dataFieldRefs)?"s":""));
            result = BCM_E_CONFIG;
        } else {
            
            /* clear the data qualifier in hardware */
            control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
            control_ndx.val_ndx = qual_id;
            control_info.val[0] = SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_0;
            control_info.val[1] = 0;
            control_info.val[2] = 1;
            ppdr = soc_ppd_fp_control_set(unitData->unitHandle,
                                          SOC_CORE_INVALID,
                                          &control_ndx,
                                          &control_info,
                                          &success);
            result = handle_sand_result(ppdr);
            if (BCM_E_NONE == result) {
                result = translate_sand_success_failure(success);
            }
            if (BCM_E_NONE != result) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "unable to set unit %d data field"
                                       " %d: %d (%s)\n"),
                           unit,
                           qual_id,
                           result,
                           _SHR_ERRMSG(result)));
            }
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d data qualifier %d not in use\n"),
                   unit,
                   qual_id));
        result = BCM_E_NOT_FOUND;
    }
    if (BCM_E_NONE == result) {
        FIELD_ACCESS.dataFieldInUse.bit_clear(unit, qual_id);
        FIELD_ACCESS.dataFieldLenBit.bit_clear(unit, qual_id);
        FIELD_ACCESS.dataFieldOfsBit.bit_clear(unit, qual_id);
        FIELD_ACCESS.dataFieldDnxOrderBit.bit_clear(unit, qual_id);
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
        _bcm_dpp_field_datafield_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
    }

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      bcm_petra_field_data_qualifier_get
 *   Purpose
 *      Get a data qualifier configuration
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) int qual_id = which qualifier ID to use
 *      (out) bcm_field_data_qualifier_t *qual = where to put description
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 */
int
bcm_petra_field_data_qualifier_get(int unit,
                                   int qual_id,
                                   bcm_field_data_qualifier_t *qual)
{
    _DPP_FIELD_COMMON_LOCALS;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    if (!qual) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory out arg is NULL")));
    }
    if ((0 > qual_id) || (qual_id >= dqLimit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d data qualifier %d"
                                           " not valid"),
                          unitData->unit,
                          qual_id));
    }

    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_petra_field_data_qualifier_get(unitData, qual_id, qual);

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);

exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
   _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_data_qualifier_multi_get(int unit,
                                         int qual_size,
                                         int *qual_array,
                                         int *qual_count)
{
    _DPP_FIELD_COMMON_LOCALS;
    int index;
    int count;
    uint8 dataFieldInUse;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    if (0 > qual_size) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("qual_size must be non-negative")));
    }
    if (qual_size && (!qual_array)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("qual_array must not be NULL if qual_size"
                                   " is positive")));
    }
    if (!qual_count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("qual_count must not be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    for (count = 0, index = 0;
         (index < dqLimit) &&
         (BCM_E_NONE == result) &&
         ((0 == qual_size) || (count < qual_size));
         index++) {
        FIELD_ACCESS.dataFieldInUse.bit_get(unit, index, &dataFieldInUse);
        if (dataFieldInUse) {
            if (qual_size) {
                qual_array[count] = index;
            }
            count++;
        } /* dataFieldInUse */
    } /* for (all data quals as long as no error and buffer not full) */
    *qual_count = count;

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
   _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_data_qualifier_destroy_all
 * Purpose:
 *      Delete all data/offset based qualifiers.
 * Parameters:
 *      unit          - (IN)  bcm device.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_destroy_all(int unit)
{
    _DPP_FIELD_COMMON_LOCALS;
    int index;
    _bcm_dpp_field_dq_idx_t dqLimit;
    uint8 dataFieldInUse;
    
    BCMDNX_INIT_FUNC_DEFS;
    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);

    /*
     *  NOTE: safe to read unitData->dqLimit outside of lock because it does
     *  not change during execution.
     */
    for (index = 0; index < dqLimit; index++) {
        FIELD_ACCESS.dataFieldInUse.bit_get(unit, index, &dataFieldInUse);
        if (dataFieldInUse) {
            result = bcm_petra_field_data_qualifier_destroy(unit, index);
            if (BCM_E_NOT_FOUND == result) {
                /* destroying all; okay if it's already not here */
                result = BCM_E_NONE;
            } else if (BCM_E_NONE != result) {
                BCMDNX_ERR_EXIT_MSG(result,
                                 (_BSL_BCM_MSG("unexpected result deleting data"
                                           " qualifier %d: %d (%s)"),
                                  index,
                                  result,
                                  _SHR_ERRMSG(result)));
            }
        }
    } /* for (all possible data qualifiers) */

    BCMDNX_IF_ERR_EXIT(result);
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
   _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_qset_data_qualifier_add
 * Purpose:
 *      Add field data qualifier to group qset.
 * Parameters:
 *      unit          - (IN) bcm device.
 *      qset          - (IN/OUT) Group qualifier set.
 *      qualifier_id  - (IN) Data qualifier id.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_qset_data_qualifier_add(int unit,
                                        bcm_field_qset_t *qset,
                                        int qual_id)
{
    bcm_dpp_field_info_OLD_t  *unitData;
    _bcm_dpp_field_dq_idx_t dqLimit;

    BCMDNX_INIT_FUNC_DEFS;
    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    if ((0 > qual_id) || (dqLimit <= qual_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("invalid data qualifier id %d"),
                          qual_id));
    }
    if (!qset) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("qset must not be NULL")));
    }

    /* won't do any other checking here; group create/set verifies quals */
    SHR_BITSET(qset->udf_map, qual_id);

    BCM_EXIT;
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
   _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_qset_data_qualifier_delete
 * Purpose:
 *      Remove field data qualifier to group qset.
 * Parameters:
 *      unit          - (IN) bcm device.
 *      qset          - (IN/OUT) Group qualifier set.
 *      qualifier_id  - (IN) Data qualifier id.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_qset_data_qualifier_delete(int unit,
                                           bcm_field_qset_t *qset,
                                           int qual_id)
{
    bcm_dpp_field_info_OLD_t  *unitData;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;
    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    if ((0 > qual_id) || (dqLimit <= qual_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("invalid data qualifier id %d"),
                          qual_id));
    }
    if (!qset) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("qset must not be NULL")));
    }

    /* won't do any other checking here; group create/set verifies quals */
    SHR_BITCLR(qset->udf_map, qual_id);

    BCM_EXIT;
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
   _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_field_qset_data_qualifier_get(int unit,
/* Coverity: the size of bcm_field_qset_t is 160 bits and hence, there is a defect of big parameter passed by value.
 * This is an api, and we can't change the prototype.
 */
/* coverity[pass_by_value] */
                                        bcm_field_qset_t qset,
                                        int qual_max,
                                        int *qual_arr,
                                        int *qual_count)
{
    bcm_dpp_field_info_OLD_t  *unitData;
    unsigned int index;
    unsigned int offset;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if (0 > qual_max) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("max qualifiers must be nonnegative")));
    }
    if (qual_max && (!qual_arr)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("qual_arr must not be NULL if max"
                                   " qualifiers is positive")));
    }
    if (!qual_count) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("qual_count must not be NULL")));
    }

    /* scan the qset and expand a list of programmable fields */

    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    for (index = 0, offset = 0;
         (index < dqLimit) &&
         ((!qual_max) || (offset < qual_max));
         index++) {
        if (SHR_BITGET(qset.udf_map, index)) {
            if (qual_max) {
                qual_arr[offset] = index;
            }
            offset++;
        }
    }
    *qual_count = offset;

    BCM_EXIT;
exit:
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
   _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE_NO_SANITY_CHECK(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_petra_field_qualify_data
 * Purpose:
 *      Set data/mask in the search engine for entry field data qualifier.
 * Parameters:
 *      unit          - (IN) bcm device.
 *      eid           - (IN) Entry id.
 *      qual_id       - (IN) Data qualifier id.
 *      data          - (IN) Match data.
 *      mask          - (IN) Match mask.
 *      length        - (IN) Length of data & mask arrays.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_qualify_data(int unit,
                             bcm_field_entry_t eid,
                             int qual_id,
                             uint8 *data,
                             uint8 *mask,
                             uint16 length)
{
    _DPP_FIELD_COMMON_LOCALS;
    bcm_field_data_qualifier_t qual;
    uint64 edata;
    uint64 emask;
    uint64 xmask;
    uint64 ymask;
    uint64 temp;
    unsigned int index;
    SOC_PPC_FP_QUAL_TYPE hwType;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_STOP(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);

    if (8 < length) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("data qualifier length %d invalid"),
                          length));
    }
    if (qual_id >= dqLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("data qualifier %d not supported"),
                          qual_id));
    }
    result = _bcm_petra_field_map_bcm_udf_to_ppd(unitData,
                                                 qual_id,
                                                 &hwType);
    BCMDNX_IF_ERR_EXIT(result); /* called func displayed diagnostics */
    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_petra_field_data_qualifier_get(unitData, qual_id, &qual);
    if (BCM_E_NONE == result) {
        qual.offset = qual.length;
        if (!(qual.flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES)) {
            qual.length <<= 3;
        }
        /* build the values to be used */
        COMPILER_64_ZERO(edata);
        COMPILER_64_ZERO(emask);
        COMPILER_64_SET(xmask, 0, 1);
        COMPILER_64_SHL(xmask, qual.length);
        COMPILER_64_SUB_32(xmask, 1);
        ymask = xmask;
        COMPILER_64_NOT(ymask);
        for (index = 0; index < length; index++) {
            COMPILER_64_SHL(edata, 8);
            COMPILER_64_SHL(emask, 8);
            COMPILER_64_SET(temp, 0, data[index]);
            COMPILER_64_OR(edata, temp);
            COMPILER_64_SET(temp, 0, mask[index]);
            COMPILER_64_OR(emask, temp);
        }
        temp = edata;
        COMPILER_64_AND(temp, ymask);
        if (!COMPILER_64_IS_ZERO(temp)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "too many bits in data qualifier %d data,"
                                   " configured length is %d %s%s\n"),
                       qual_id,
                       qual.offset,
                       (qual.flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES)?"bit":"byte",
                       (1 != qual.offset)?"s":""));
            result = BCM_E_PARAM;
        }
        temp = emask;
        COMPILER_64_AND(temp, ymask);
        if (!COMPILER_64_IS_ZERO(temp)) {
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "too many bits in data qualifier %d mask,"
                                   " configured length is %d %s%s\n"),
                       qual_id,
                       qual.offset,
                       (qual.flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES)?"bit":"byte",
                       (1 != qual.offset)?"s":""));
            result = BCM_E_PARAM;
        }
    } /* if (BCM_E_NONE == result) */
    if (BCM_E_NONE == result) {
        result = _bcm_dpp_field_qualifier_set_single(unitData,
                                                     eid,
                                                     hwType,
                                                     edata,
                                                     emask,
                                                     xmask);
    } /* if (BCM_E_NONE == result) */

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_qualify_data_get
 * Purpose:
 *      Get data/mask for entry field data qualifier.
 * Parameters:
 *      unit          - (IN) bcm device.
 *      eid           - (IN) Entry id.
 *      qual_id       - (IN) Data qualifier id.
 *      length_max        - (IN) length of output buffer
 *      data          - (OUT) Match data.
 *      mask          - (OUT) Match mask.
 *      length        - (OUT) Length of data & mask arrays.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_qualify_data_get(int unit,
                                 bcm_field_entry_t eid,
                                 int qual_id,
                                 uint16 length_max,
                                 uint8 *data,
                                 uint8 *mask,
                                 uint16 *length)
{
    _DPP_FIELD_COMMON_LOCALS;
    bcm_field_data_qualifier_t qual;
    uint64 curr_data;
    uint64 curr_mask;
    uint64 check;
    uint64 temp, temp1;
    unsigned int offset;
    SOC_PPC_FP_QUAL_TYPE hwType;
    _bcm_dpp_field_dq_idx_t dqLimit;
    BCMDNX_INIT_FUNC_DEFS;
#if (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT))
    _DCMN_BCM_WARM_BOOT_API_TEST_FIELD_QUAL_WB_TEST_MODE_CONTINUE(unit);
#endif /* (defined(BCM_DPP_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)) */

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);
    if (8 < length_max) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("buffer length %d invalid"),
                          length_max));
    }
    if (qual_id >= dqLimit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("data qualifier %d not supported"),
                          qual_id));
    }
    result = _bcm_petra_field_map_bcm_udf_to_ppd(unitData,
                                                 qual_id,
                                                 &hwType);
    BCMDNX_IF_ERR_EXIT(result); /* called func displayed diagnostics */
    _DPP_FIELD_UNIT_LOCK(unitData);

    result = _bcm_petra_field_data_qualifier_get(unitData, qual_id, &qual);
    if (BCM_E_NONE == result) {
        if (length_max == 0) {
            /* return only the qualifier length (rounded to the nearest byte) */
            if (!(qual.flags & BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES)) {
                qual.length <<= 3;
            }
            *length = qual.length / 8;
            if (0 != qual.length % 8) {
                (*length)++;
            }
        } else { /* if (length_max == 0) */
            result = _bcm_dpp_field_qualifier_get_single(unitData,
                                                         eid,
                                                         hwType,
                                                         &curr_data,
                                                         &curr_mask);
            if (BCM_E_NONE == result) {
                /*
                 *  make sure the provided buffer is big enough to fit the
                 *  stored data
                 */
                COMPILER_64_SET(check, 0xFFFFFFFF, 0xFFFFFFFF);
                if (8 * length_max >= 64) {
                    COMPILER_64_SET(check, 0, 0);
                } else {
                    COMPILER_64_SHL(check, 8 * length_max);
                }
                temp = check;
                COMPILER_64_AND(temp, curr_data);
                if (!COMPILER_64_IS_ZERO(temp)) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "too many bits in data qualifier %d"
                                           " data, to fit in a buffer of length"
                                           " %d byte%s\n"),
                               qual_id,
                               length_max,
                               (1 != length_max)?"s":""));
                    result = BCM_E_PARAM;
                }
                temp = check;
                COMPILER_64_AND(temp, curr_mask);
                if (!COMPILER_64_IS_ZERO(temp)) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "too many bits in data qualifier %d"
                                           " mask, to fit in a buffer of length"
                                           " %d byte%s\n"),
                               qual_id,
                               length_max,
                               (1 != length_max)?"s":""));
                    result = BCM_E_PARAM;
                }
                if (BCM_E_NONE == result) {
                    /* fill the data from the stored values */
                    for (offset = 0; offset < length_max; offset++) {
                        data[offset] = 0;
                        mask[offset] = 0;
                    }
                    while (!COMPILER_64_IS_ZERO(curr_data) || !COMPILER_64_IS_ZERO(curr_mask)) {
                        offset--;
                        data[offset] = COMPILER_64_LO(curr_data) & 0xFF;
                        mask[offset] = COMPILER_64_LO(curr_mask) & 0xFF;
                        COMPILER_64_SHR(curr_data, 8);
                        COMPILER_64_SHR(curr_mask, 8);
                        COMPILER_64_SET(temp1, 0x00FFFFFF, 0xFFFFFFFF);
                        COMPILER_64_AND(curr_data, temp1);
                        COMPILER_64_SET(temp1, 0x00FFFFFF, 0xFFFFFFFF);
                        COMPILER_64_AND(curr_mask, temp1);
                    }
                    *length = length_max;
                }
            } /* if (BCM_E_NONE == result) */
        } /* if (length_max == 0) */
    } /* if (BCM_E_NONE == result) */

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_data_qualifier_ethertype_add
 * Purpose:
 *      Add ethertype based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_ethertype_add(int unit,  int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_data_qualifier_ethertype_delete
 * Purpose:
 *      Remove ethertype based offset from data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_ethertype_delete(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_data_qualifier_ip_protocol_add
 * Purpose:
 *      Add ip protocol based offset to data qualifier object.
 * Parameters:
 *      unit        - (IN) bcm device.
 *      qual_id     - (IN) Data qualifier id.
 *      ip_protocol - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_ip_protocol_add(int unit, int qual_id,
                                 bcm_field_data_ip_protocol_t *ip_protocol)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_data_qualifier_ip_protocol_delete
 * Purpose:
 *      Remove ip protocol based offset from data qualifier object.
 * Parameters:
 *      unit        - (IN) bcm device.
 *      qual_id     - (IN) Data qualifier id.
 *      ip_protocol - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_ip_protocol_delete(int unit, int qual_id,
                                 bcm_field_data_ip_protocol_t *ip_protocol)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_field_data_qualifier_packet_format_add
 * Purpose:
 *      Add packet format based offset to data qualifier object.
 * Parameters:
 *      unit          - (IN) bcm device.
 *      qual_id       - (IN) Data qualifier id.
 *      packet_format - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_packet_format_add(int unit, int qual_id,
                                 bcm_field_data_packet_format_t *packet_format)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_field_data_qualifier_packet_format_delete
 * Purpose:
 *      Remove packet format based offset from data qualifier object.
 * Parameters:
 *      unit           - (IN) bcm device.
 *      qual_id        - (IN) Data qualifier id.
 *      packet_format  - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_field_data_qualifier_packet_format_delete(int unit, int qual_id,
                                 bcm_field_data_packet_format_t *packet_format)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}


