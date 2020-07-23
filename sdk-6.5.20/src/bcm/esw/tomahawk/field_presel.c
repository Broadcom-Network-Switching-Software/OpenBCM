/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field_presel.c
 * Purpose:     Field Processor routines Specific to Preselector Module.
 */

#include <soc/defs.h>

#if defined(BCM_TOMAHAWK_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <bcm/error.h>
#include <bcm/field.h>
#include <shared/bsl.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/tomahawk3.h>
#if defined(BCM_HELIX5_SUPPORT)
#include <bcm_int/esw/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */

/* Function Declarations */
STATIC int
_bcm_field_presel_action_remove_all(int unit, bcm_field_entry_t entry);

#define _BCM_FIELD_PRESEL_IS_STAGE_SUPPORTED(_stage_)   \
       ((_stage_ == bcmFieldStageIngress) ||            \
        (_stage_ == bcmFieldStageIngressExactMatch) ||  \
        (_stage_ == bcmFieldStageIngressFlowtracker))
/*
 * Function:
 *    _field_th_presel_qualifiers_init
 *
 * Purpose:
 *    Initialize Field Ingress Stage Preselector Qualifiers.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN/OUT) Field Processor stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_MEMORY      - Allocation failure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_th_presel_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /*
     * All the qualifiers offset mentioned are the actual individual HW field
     * offset in IFP_LOGICAL_TABLE_SELECT Memory which are starting from bit
     * position 2. "KEY" overlap field in the memory is used during write,
     * hence to accomidate in the field, the offsets of the qualifiers has to
     * be substracted by 2.
     */
    static int8 key_base_off;
    soc_mem_t mem = INVALIDm;
    soc_field_info_t *field_info = NULL;
    _FP_QUAL_DECL;


    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_presel_qual_arr,
        (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
        "IFP Preselector qualifiers");

    if (stage_fc->f_presel_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
       mem = EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
    } else {
       mem = IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm;
    }

    field_info = soc_mem_fieldinfo_get(unit, mem, KEYf);
    if (field_info == NULL) {
       return BCM_E_PARAM;
    }
    key_base_off = field_info->bp;

    /* Preselector qualifiers. */
    /* SOURCE_CLASS - 32 */
    field_info = soc_mem_fieldinfo_get(unit, mem, SOURCE_CLASSf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMixedSrcClassId,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off, 32);
    }

    /* EXACT_MATCH_LOGICAL_TABLE_ID_0 - 5 */
    field_info = soc_mem_fieldinfo_get(unit, mem,
                                       EXACT_MATCH_LOGICAL_TABLE_ID_0f);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc,
            _bcmFieldQualifyExactMatchGroupClassIdLookup0,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off, 5);
    } else {
        /* EXACT_MATCH_LOGICAL_TABLE_ID - 5 */
        field_info = soc_mem_fieldinfo_get(unit, mem,
                                   EXACT_MATCH_LOGICAL_TABLE_IDf);
        if (field_info != NULL) {
            _FP_PRESEL_QUAL_ADD(unit, stage_fc,
                _bcmFieldQualifyExactMatchGroupClassIdLookup0,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp - key_base_off, 5);
        }
    }

    /* EXACT_MATCH_LOGICAL_TABLE_ID_1 - 5 */
    field_info = soc_mem_fieldinfo_get(unit, mem,                                                                      EXACT_MATCH_LOGICAL_TABLE_ID_1f);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc,
            _bcmFieldQualifyExactMatchGroupClassIdLookup1,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT
            , _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off, 5);
    }

    /* RESERVED_0 - 1 */

    /* HG_LOOKUP - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, HG_LOOKUPf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* CPU_PKT_PROFILE_KEY_CONTROL - 1 */
#if 0
    _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualify,
        0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
        45 - key_base_off , 1);
#endif

    /* MIRROR - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, MIRRORf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* HG - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, HGf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* DROP - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, DROPf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* PKT_RESOLUTION - 6 */
    field_info = soc_mem_fieldinfo_get(unit, mem, PKT_RESOLUTIONf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
            _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
            _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 6);
    }

    /* FORWARDING_TYPE - 4 */
    field_info = soc_mem_fieldinfo_get(unit, mem, FORWARDING_TYPEf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 4);
    }

    /* LOOKUP_STATUS_VECTOR - 19 */
    field_info = soc_mem_fieldinfo_get(unit, mem, LOOKUP_STATUS_VECTORf);
    if (field_info != NULL) {
        if (SOC_IS_TOMAHAWK3(unit)) {
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+0 - key_base_off , 2);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStpState,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+0 - key_base_off , 2);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+2 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc,
                bcmFieldQualifyForwardingVlanValid,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+3 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+4 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+5 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+6 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+7 - key_base_off , 1);
        } else {
            _FP_PRESEL_QUAL_ADD(unit, stage_fc,
                bcmFieldQualifyVlanTranslationHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+0 - key_base_off , 3);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc,
                bcmFieldQualifyForwardingVlanValid,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+3 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+4 - key_base_off , 2);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStpState,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+4 - key_base_off , 2);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+6 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+7 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+8 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+9 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                 _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+10 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+10 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+11 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+12 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                _BCM_FIELD_QUAL_OFFSET_NO_EM_SUPPORT,
                _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+13 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+14 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpTunnelHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+15 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel1Hit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+16 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc,
                bcmFieldQualifyTrillEgressRbridgeHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+16 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreSrcIpHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+16 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimSrcGportHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+16 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsLabel2Hit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+17 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc,
                bcmFieldQualifyTrillIngressRbridgeHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+17 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2GreVfiHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+17 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMimVfiHit,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+17 - key_base_off , 1);
            _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
                field_info->bp+18 - key_base_off , 1);
        }
    }

    /* SVP_VALID - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, SVP_VALIDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcVirtualPortValid,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* TUNNEL_TERMINATED - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, TUNNEL_TERMINATEDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* MY_STATION_HIT - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, MY_STATION_HITf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMyStationHit,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* TUNNEL_TYPE_LOOPBACK_TYPE - 5 */
    field_info = soc_mem_fieldinfo_get(unit, mem, TUNNEL_TYPE_LOOPBACK_TYPEf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 5);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 5);
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp + 4 - key_base_off , 1);
    }

    /* L4_VALID - 1 */
    field_info = soc_mem_fieldinfo_get(unit, mem, L4_VALIDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* L3_TYPE - 5 */
    field_info = soc_mem_fieldinfo_get(unit, mem, L3_TYPEf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 5);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_config_init
 * Purpose:
 *     Initialize a Logical Table configuration in field control.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     fc       - (IN) field control structure pointer.
 * Returns:
 *     BCM_E_PARAM - Device field control information is NULL
 *     BCM_E_NONE  - Success
 */
int
_field_th_lt_config_init(int unit, _field_control_t *fc)
{
    int lt_idx;                 /* Logical Table iterator.         */
    int inst;                   /* Instance iterator.              */
    _field_lt_config_t *lt_p = NULL;
                               /* Field LT configuration pointer. */

    int max_lt_parts = _FP_MAX_LT_PARTS(unit);
    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    /* Initialize for all pipes and all logical tables. */
    for (inst = 0; inst < NUM_PIPE(unit); inst++) {
        for (lt_idx = 0; lt_idx < _FP_MAX_NUM_LT; lt_idx++) {

            /* Allocate memory to store LT information. */
            _FP_XGS3_ALLOC(lt_p, sizeof(_field_lt_config_t),
                             "logical table config");

            if (NULL == lt_p) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d) Error: LT(Inst=%d) allocation=%d\n"),
                    unit, inst, (BCM_E_MEMORY)));
                return (BCM_E_MEMORY);
            }

            lt_p->lt_part_pri = NULL;

            /* Allocate memory for Lt part pri */
            _FP_XGS3_ALLOC(lt_p->lt_part_pri, (sizeof(int) * max_lt_parts),
                           "Part Pri for logical table ");

            if (NULL == lt_p->lt_part_pri) {
                sal_free(lt_p);
                lt_p = NULL;
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "FP(unit %d) Error: LT Part Pri(Inst=%d) allocation=%d\n"),
                    unit, inst, (BCM_E_MEMORY)));
                return (BCM_E_MEMORY);
            }

            /* Initialize Logical Table ID. */
            lt_p->lt_id = lt_idx;

            /* Initialize stage LT pointer for given instance. */
            fc->lt_info[inst][lt_idx] = lt_p;
            lt_p = NULL;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_instance_lt_slices_init
 * Purpose:
 *     Initialize a Logical Table instance slices for Ingress stage.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     stage_fc - (IN) stage control structure.
 *     inst     - (IN) Field Pipe Instance for which Logical tables must be
 *                     deinitialized.
 * Returns:
 *     BCM_E_PARAM - Device stage field control information is NULL
 *                   or Invalid instance specified.
 *     BCM_E_NONE  - Success
 */
STATIC int
_field_th_instance_lt_slices_init(int unit,
                                  _field_stage_t *stage_fc,
                                  int inst)
{
    _field_lt_slice_t *lt_fs;   /* Logical table slice info.      */
    int slice;                  /* Slice iterator.                */
    int mem_sz;                 /* Memory allocation buffer size. */
    uint32 flags;               /* Stage slice related flags */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Validate instance value. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        flags = _BCM_FIELD_SLICE_HW_ATOMICITY_SUPPORT;
    } else {
        flags = _BCM_FIELD_SLICE_SW_ATOMICITY_SUPPORT;
    }
    /* Allocate memory to store logical table slices information. */
    mem_sz = stage_fc->tcam_slices * sizeof(_field_lt_slice_t);
    lt_fs = sal_alloc(mem_sz, "stage LT slices info");
    if (NULL == lt_fs) {
        return (BCM_E_MEMORY);
    }

    /* Initialize memory allocated for logical tables. */
    sal_memset(lt_fs, 0, mem_sz);

    /* Update stage's logical table slices pointer for the instance. */
    stage_fc->lt_slices[inst] = lt_fs;

    /* Initialize LT slice structure fields. */
    for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
        lt_fs[slice].slice_number = slice;
        lt_fs[slice].stage_id = stage_fc->stage_id;
        lt_fs[slice].next = NULL;
        lt_fs[slice].prev = NULL;
        lt_fs[slice].slice_flags = flags;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        if (soc_feature(unit, soc_feature_field_flowtracker_v3_support) &&
            (_BCM_FIELD_STAGE_FLOWTRACKER == stage_fc->stage_id)) {
            lt_fs[slice].slice_flags |= _BCM_FIELD_SLICE_INDEPENDENT;
        }
#endif
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_stage_lt_entries_free
 * Purpose:
 *     Deallocate Logical Table entries for all instance in StageIngress.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) stage control structure pointer.
 * Returns:
 *     BCM_E_PARAM - Device stage field control information is NULL.
 *     BCM_E_NONE  - Success
 */
STATIC int
_field_th_stage_lt_entries_free(int unit, _field_stage_t *stage_fc)
{
    _field_lt_slice_t *lt_fs; /* Field logical table slice pointer. */
    int slice;                /* Slice iterator.                    */
    int inst;                 /* Instance iterator.                 */
    _field_control_t *fc;     /* Field Control Info                 */

    /* Input prameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN (_field_control_get(unit, &fc));

    /* Deallocate instance entry pointers. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        if (!(fc->pipe_map & (1 << inst))) {
             continue;
        }
       /* Ensure stage's logical table slices instance is valid. */
       if (NULL != stage_fc->lt_slices[inst]) {
          for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
              lt_fs = stage_fc->lt_slices[inst] + slice;
              if (NULL != lt_fs->entries) {
                  sal_free(lt_fs->entries);
                  lt_fs->entries = NULL;
              }

              if (NULL != lt_fs->p_entries) {
                  sal_free(lt_fs->p_entries);
                  lt_fs->p_entries = NULL;
              }
          }
       }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_instance_lt_entries_init
 * Purpose:
 *     Initialize Logical Table slices for a StageIngress instance.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) stage control structure pointer.
 *     inst     - (IN) Field Pipe Instance for which Logical tables must be
 *                     deinitialized.
 * Returns:
 *     BCM_E_PARAM - Device stage field control information is NULL
 *                   or Invalid instance specified.
 *     BCM_E_NONE  - Success
 */
STATIC int
_field_th_instance_lt_entries_init(int unit, _field_stage_t *stage_fc, int inst)
{
    _field_lt_slice_t *lt_fs;   /* Field logical table slice pointer.    */
    int slice;                  /* Slice index iterator.                 */
    int tcam_idx;               /* Slice first entry tcam index tracker. */
    int mem_sz;                 /* Size of memory to be allocated.       */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Check if instance is in valid range. */
    if (inst < 0 || inst >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Make sure slices array is initialized. */
    if (NULL == stage_fc->lt_slices[inst]) {
        return (BCM_E_INTERNAL);
    }

    /* Iterate over all stage's slices and allocate entries for each slice. */
    for (slice = 0, tcam_idx = 0; slice < stage_fc->tcam_slices; slice++) {
        /* Get Pipe slice control structure pointer. */
        lt_fs = stage_fc->lt_slices[inst] + slice;

        /*  Initialize entries per-slice count. */
        lt_fs->entry_count = stage_fc->lt_tcam_sz / stage_fc->tcam_slices;

        /* Calculate size for total entry pointers. */
        mem_sz = lt_fs->entry_count * sizeof(void *);

        /* Allocate slice entries pointers array. */
        _FP_XGS3_ALLOC(lt_fs->entries, mem_sz, "LT array of entry pointers.");

        /* Allocate slice presel entries pointers array. */
        _FP_XGS3_ALLOC(lt_fs->p_entries, mem_sz,
                      "Presel LT array of entry pointers.");
        if ((NULL == lt_fs->p_entries) || (NULL == lt_fs->entries)) {
            /* Allocation failed free all previously allocated entries. */
            _field_th_stage_lt_entries_free(unit, stage_fc);
            return (BCM_E_MEMORY);
        }

        lt_fs->start_tcam_idx = tcam_idx;
        tcam_idx += lt_fs->entry_count;
        lt_fs->free_count = lt_fs->entry_count;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_presel_qual_init
 * Purpose:
 *     Initialize Field control Preselector qualifiers.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     fc       - (IN) Reference to Field Control structure.
 *     stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *     BCM_E_PARAM  - Device field control information is NULL.
 *     BCM_E_MEMORY - In-case of Allocation fails.
 *     BCM_E_NONE   - Success.
 */

STATIC int
_field_th_presel_qual_init(int unit,
                           _field_control_t *fc,
                           _field_stage_t *stage_fc)
{
    int                   rv;                   /* Operation return status.  */
    int                   qual_idx;             /* Qualifier index. */

    /* Input parameters check. */
    if (NULL == fc || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Initialize qualifiers info. */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_fp)) {
        rv = (_field_td3_presel_qualifiers_init(unit, stage_fc));
    } else
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_th3_style_fp)) {
	    rv = (_field_th3_presel_qualifiers_init(unit, stage_fc));
    } else
#endif
    {
        rv = (_field_th_presel_qualifiers_init(unit, stage_fc));
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "Error: Preselector Qualifiers init failed[%d].\n"),
            rv));
        return rv;
    }

    /* Update the Valid Preselector QSET */
    for (qual_idx = 0; qual_idx < _bcmFieldQualifyCount; qual_idx++) {
       if ((NULL != stage_fc->f_presel_qual_arr[qual_idx]) &&
           (NULL != stage_fc->f_presel_qual_arr[qual_idx]->conf_arr)) {
          BCM_FIELD_QSET_ADD(stage_fc->presel_qset, qual_idx);
       }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_stage_preselector_init
 * Purpose:
 *    Initialize Preselector resources for given stage and field control.
 *    - Logical Table configuration initialization for each instance.
 *    - Logical Table Slice initialization for each instance.
 *    - Logical Table Entries initialization for each instance.
 *    - Preselection Qualifier Database initialization.
 *
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    fc        - (IN) Field control structure.
 *    stage_fc  - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM   - NULL stage control or field control pointer.
 *    BCM_E_MEMORY  - Allocation failure.
 *    BCM_E_NONE    - Success.
 */
int
_bcm_field_th_stage_preselector_init(int unit,
                                     _field_control_t *fc,
                                     _field_stage_t *stage_fc)
{
    int inst;   /* Field processor pipe instance.   */
    int rv;     /* Operation return status.         */

    /* Input parameters check. */
    if (NULL == fc || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for Non-PresePreseltages. */
    if ((_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) &&
        (_BCM_FIELD_STAGE_EXACTMATCH != stage_fc->stage_id) &&
        (_BCM_FIELD_STAGE_FLOWTRACKER != stage_fc->stage_id)) {
        return (BCM_E_NONE);
    }

    /* Initialize Preselection qualifiers */
    rv = _field_th_presel_qual_init(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "FP(unit %d) Error: Presel Info init failed(%d)\n"),
                unit, rv));
            return (rv);
    }

    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        if (!(fc->pipe_map & (1 << inst))) {
             continue;
        }
        /* Initialize logical table slices for IFP and EM stage. */
        rv = _field_th_instance_lt_slices_init(unit, stage_fc, inst);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "FP(unit %d) Error: LT(Inst=%d) Slices Init=%d\n"),
                unit, inst, rv));
            return (rv);
        }

        /* Initialize logical table slice entries for IFP and EM stage. */
        rv = _field_th_instance_lt_entries_init(unit, stage_fc, inst);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "FP(unit %d) Error: LT(Inst=%d) Entries Init=%d\n"),
                unit, inst, rv));
            return (rv);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_presel_qual_free
 * Purpose:
 *     De-Initialize Preselector Qualifier Database.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Stage field control structure.
 * Returns:
 *     BCM_E_PARAM  - Device field control information is NULL.
 *     BCM_E_MEMORY - In-case of Allocation fails.
 *     BCM_E_NONE   - Success.
 */

STATIC int
_field_th_presel_qual_free(int unit,
                           _field_stage_t *stage_fc)
{
    _bcm_field_qual_info_t  *f_qual; /* Qualifier descriptor.       */
    int idx;                         /* Qualifiers iteration index. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    if (NULL == stage_fc->f_presel_qual_arr) {
        return (BCM_E_NONE);
    }

    for (idx = 0; idx < _bcmFieldQualifyCount; idx++) {
        f_qual = stage_fc->f_presel_qual_arr[idx];

        /* Skip unused qualifiers. */
        if (NULL == f_qual) {
            continue;
        }
        /* Free qualifier configurations array. */
        if (NULL != f_qual->conf_arr) sal_free(f_qual->conf_arr);

        /* Free qualifier descriptor structure. */
        sal_free(f_qual);

        /* Mark qualifier as unused. */
        stage_fc->f_presel_qual_arr[idx] = NULL;
    }

    /* Free qualifiers array. */
    sal_free(stage_fc->f_presel_qual_arr);
    stage_fc->f_presel_qual_arr = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_preselector_deinit
 * Purpose:
 *     Deinitialize a Preselector resources in Ingress stage.
 *     - Deallocate Logical table slice entries and slices for each instance.
 *     - Deallocate Logical table configuration structures for each instance.
 *     - Deallocate Preselector Qualifiers Information.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     fc       - (IN) Field control structure.
 *     stage_fc - (IN) Stage field control structure.
 * Returns:
 *     BCM_E_PARAM - Device field control or stage field control information
 *                   is NULL.
 *     BCM_E_NONE  - Success
 */
int
_bcm_field_th_preselector_deinit(int unit,
                                 _field_control_t *fc,
                                 _field_stage_t *stage_fc)
{
    int slice;                  /* Slice iterator.                  */
    int inst;                   /* Pipe instance.                   */
    _field_lt_slice_t *lt_fs;   /* Logical Table slice information. */

    /* Input parameters check. */
    if (NULL == fc || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for Non-Presel stages. */
    if ((_BCM_FIELD_STAGE_INGRESS != stage_fc->stage_id) &&
        (_BCM_FIELD_STAGE_EXACTMATCH != stage_fc->stage_id) &&
        (_BCM_FIELD_STAGE_FLOWTRACKER != stage_fc->stage_id)) {
        return (BCM_E_NONE);
    }

    /* Free the resources for each instance. */
    for (inst = 0; inst < stage_fc->num_instances; inst++) {
        if (!(fc->pipe_map & (1 << inst))) {
             continue;
        }
       /* Ensure stage's logical table slices instance is valid. */
       if (NULL != stage_fc->lt_slices[inst]) {
           /* Deallocate logical table entry pointers for the given instance. */
           for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
              lt_fs = stage_fc->lt_slices[inst] + slice;
              if (NULL != lt_fs->entries) {
                 sal_free(lt_fs->entries);
                 lt_fs->entries = NULL;
              }
              if (NULL != lt_fs->p_entries) {
                 sal_free(lt_fs->p_entries);
                 lt_fs->p_entries = NULL;
              }
           }

           /* Free logical table slices memory for active instances. */
           sal_free(stage_fc->lt_slices[inst]);
           stage_fc->lt_slices[inst] = NULL;
       }
    }

    /* Deallocate Field Control Preselector information */
    _field_th_presel_qual_free(unit, stage_fc);

    return (BCM_E_NONE);
}

/*
 * Function: bcmi_field_th_presel_alloc
 *
 * Purpose: Allocate a preselector
 *
 * Parameters:
 *     unit      - (IN)  BCM device number.
 *     presel_id - (OUT) Reference to a new preselector ID to be created
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_XXX    - BCM_E_xx appropriately otherwise
 */
int
_bcm_field_th_presel_create(int unit,
                            _bcm_field_presel_flags_t flags,
                            bcm_field_presel_t *presel)
{
    bcm_field_presel_t        id = -1;          /* Presel id */
    _field_presel_entry_t     *f_presel = NULL; /* Internal Presel descriptor */
    _field_presel_info_t      *presel_info;     /* Presel information structure */
    _field_control_t          *fc;              /* Field control structure. */

    /* Validate input arguments NULL reference */
    if (presel == NULL) {
       return BCM_E_PARAM;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN (_field_control_get(unit, &fc));

    /* Get the preselector information */
    presel_info = fc->presel_info;
    if (presel_info == NULL) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Field control for Presel Information returns NULL.\n\r")));
       return BCM_E_INTERNAL;
    }

    /* Presel create with ID */
    if (flags & _bcmFieldPreselCreateWithId) {
       id = *presel;
       if ((id < 0) || (id >= presel_info->presel_limit)) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "ERROR: Invalid Preselector ID %d.\n\r"), id));
          return BCM_E_PARAM;
       }

       /* TH3 COS SW WAR */
       if ((soc_feature(unit, soc_feature_sw_mmu_flush_rqe_operation)) &&
          (id == _FP_PRESEL_INTERNAL_RESERVED_ID)) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "ERROR: Invalid Preselector ID %d.\n\r"), id));
          return BCM_E_PARAM;
       }

       /* Check if presel ID is already created */
       if (BCM_FIELD_PRESEL_TEST(presel_info->presel_set, id)) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "ERROR: Preselector ID[%d] is already created.\n\r"), id));
          return BCM_E_EXISTS;
       }
    } else {
       uint32  max_count;  /* Maximum number of ids supported */

       max_count = presel_info->presel_limit;

       if (soc_feature(unit, soc_feature_sw_mmu_flush_rqe_operation)) {
          max_count = (presel_info->presel_limit - 1);
       }

       while (max_count) {
          id = presel_info->last_allocated_id++;

          if (soc_feature(unit, soc_feature_sw_mmu_flush_rqe_operation)) {
              if (id == (presel_info->presel_limit - 2)) {
                 presel_info->last_allocated_id = 0;
              }
          } else {
              if (id == (presel_info->presel_limit - 1)) {
                 presel_info->last_allocated_id = 0;
              }
          }

          if (!BCM_FIELD_PRESEL_TEST(presel_info->presel_set, id)) {
             *presel = id;
             break;
          }
          max_count--;
       }

       if (max_count == 0) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "ERROR: Can't create the ID, max limit reached[0x%x].\n\r"),
                    (soc_feature(unit, soc_feature_sw_mmu_flush_rqe_operation))
                     ?(presel_info->presel_limit -1):(presel_info->presel_limit)));
          return BCM_E_RESOURCE;
       }
    }

    /* Allocate presel descriptor. */
    _FP_XGS3_ALLOC(f_presel, sizeof (_field_presel_entry_t), "Field Presel entity");
    if (NULL == f_presel) {
       return BCM_E_MEMORY;
    }

    /* Initialize the newly created presel id */
    f_presel->presel_id   = id;
    /* Initialize the hw index to No HW resources allocated */
    f_presel->hw_index    = _FP_INVALID_INDEX;
    /* Initialize internal flags and stage information */
    f_presel->flags       = flags;

    /* Insert Presel into Preselector database */
    if (fc->presel_db == NULL) {
        sal_free(f_presel);
        return BCM_E_INTERNAL;
    }

    fc->presel_db[id] = f_presel;

    /* Update preselector global information */
    /* coverity[negative_shift : FALSE] */
    BCM_FIELD_PRESEL_ADD(presel_info->presel_set, id);

    return BCM_E_NONE;
}


/*
 * Function: bcmi_field_th_presel_destroy
 *
 * Purpose: Destroy a preselector entry
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     presel_id - (IN) A preselector ID to be deleted
 *
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_BUSY   - If the presel is already in use by a group.
 *     BCM_E_XXX    - BCM_E_xx appropriately otherwise
 */

int
_bcm_field_th_presel_destroy(int unit,
                             bcm_field_presel_t presel_id)
{
    int                       rv;           /* Operation return status. */
    _field_control_t          *fc;          /* Field control structure        */
    _field_presel_entry_t     *f_presel;    /* Internal preselector descriptor*/

    /* Validate the Preselector ID */
    if (_BCM_FIELD_IS_PRESEL_VALID(presel_id) == FALSE) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "ERROR: Invalid Presel ID[%d].\n\r"), presel_id));
       return BCM_E_PARAM;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN (_field_control_get(unit, &fc));

    /* Check if presel ID present in presel_set */
    if (!BCM_FIELD_PRESEL_TEST(fc->presel_info->presel_set, presel_id)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "ERROR: Preselector ID[%d] is not found.\n\r"), presel_id));
       return BCM_E_NOT_FOUND;
    }

    /* Check if presel ID is operational */
    if (BCM_FIELD_PRESEL_TEST(fc->presel_info->operational_set, presel_id)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "ERROR: Preselector ID[%d] is in USE.\n\r"), presel_id));
       return BCM_E_BUSY;
    }

    /* Find preselector descriptor */
    BCM_IF_ERROR_RETURN
      (_bcm_field_presel_entry_get(unit, presel_id, &f_presel));

    /* Remove the actions associated with the preselector entry */
    rv = _bcm_field_presel_action_remove_all(unit,
                   (presel_id | BCM_FIELD_QUALIFY_PRESEL));
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Unable to remove the actions associated with the"
             "Preselector Id[%d].\n\r"), presel_id));
        return BCM_E_INTERNAL;
    }

    /* Remove preselector for lookup database. */
    fc->presel_db[presel_id] = NULL;

    /* Deallocate the memories */
    if (f_presel->lt_tcam.key != NULL) sal_free(f_presel->lt_tcam.key);
    if (f_presel->lt_tcam.mask != NULL) sal_free(f_presel->lt_tcam.mask);
    if (f_presel->lt_data.data != NULL) sal_free(f_presel->lt_data.data);
    sal_free (f_presel);

    /* Get the preselector information */
    BCM_FIELD_PRESEL_REMOVE(fc->presel_info->presel_set, presel_id);

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_presel_entry_get
 *
 * Purpose: Get the preselector description structure for the given presel id
 *
 * Parameters:
 *     unit      - (IN)  BCM device number.
 *     presel_id - (IN)  Preselector ID
 *     presel    - (OUT) Reference to the preselector description structure
 *
 *   Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_NOT_FOUND - If the ID is not found.
 *     BCM_E_XXX       - BCM_E_xx appropriately otherwise
 */
int
_bcm_field_presel_entry_get(int unit,
                            bcm_field_presel_t presel_id,
                            _field_presel_entry_t **presel)
{
   _field_control_t      *fc;        /* Field control structure */
   _field_presel_entry_t *f_presel;  /* Preselector lookup pointer */

   /* Input parameters check */
   if (NULL == presel) {
      return BCM_E_PARAM;
   }

   /* Get field control structure. */
   BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

   /* Check if presel ID present in presel_set */
   if (!BCM_FIELD_PRESEL_TEST(fc->presel_info->presel_set, presel_id)) {
      LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "ERROR: Preselector ID[%d] is not found.\n\r"), presel_id));
      return BCM_E_NOT_FOUND;
   }

   f_presel = fc->presel_db[presel_id];
   if (f_presel != NULL) {
      /* Match entry id */
      *presel = f_presel;
      return BCM_E_NONE;
   }

   return BCM_E_INTERNAL;
}

/*
 * Function: _bcm_field_entry_presel_resolve
 *
 * Purpose:
 *     Resolve the presel info from the given entry id.
 *
 * Parameters:
 *     unit      - (IN)  BCM device number.
 *     entry     - (IN)  An entry id to be validated for presel.
 *     presel    - (OUT) Reference to the resolved presel id.
 *     presel    - (OUT) Reference to the preselector description structure
 *
 * Returns:
 *     BCM_E_PARAM     - presel contained in entry id is not valid
 *                       or presel == NULL.
 *     BCM_E_UNAVAIL   - entry id doesn't contain a presel id.
 *     BCM_E_NOT_FOUND - presel id is not found.
 *     BCM_E_NONE      - returns valid presel id.
 */
STATIC int
_bcm_field_entry_presel_resolve(int unit,
                                bcm_field_entry_t entry,
                                bcm_field_presel_t *presel,
                                _field_presel_entry_t **f_presel)
{
    int                     rv;         /* Operation return status.   */

    if (presel == NULL) {
       return BCM_E_PARAM;
    }

    if (_BCM_FIELD_IS_PRESEL_ENTRY(entry) == TRUE) {
       _field_control_t        *fc;  /* Field control structure */
       bcm_field_presel_t       id;  /* Preselector Id */


       id = _BCM_FIELD_PRESEL_FROM_ENTRY(entry);
       if (_BCM_FIELD_IS_PRESEL_VALID(id) == FALSE) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "ERROR: Invalid Presel ID[%d] retrieved from Entry ID[%d].\n\r"),
               id, entry));
          return BCM_E_PARAM;
       }

       BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
       /* Check if presel ID present in presel_set */
       if (!BCM_FIELD_PRESEL_TEST(fc->presel_info->presel_set, id)) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Preselector ID[%d] is not found.\n\r"), id));
          return BCM_E_NOT_FOUND;
       }

       *presel = id;

       /* Retrieve the Presel Entry */
       rv = _bcm_field_presel_entry_get(unit, id, f_presel);
       if (BCM_FAILURE(rv)) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "ERROR: Couldn't retrieve the Preselector Entry for the given"
              " Presel ID[%d].\n\r"), *presel));
          return rv;
       }
    } else {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "ERROR: Entry ID[%d] is not of Preselector Type.\n\r"), entry));
       return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_presel_qual_offset_get
 * Purpose:
 *     Retrieve qualifier offsets for the given preselector qual.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Stage field control structure.
 *     f_presel - (IN) Reference to Field preselector entry descriptor.
 *     qid      - (IN) Qualifier id.
 *     q_offset - (OUT) Qualifier offset and width.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_presel_qual_offset_get(int unit, _field_stage_t *stage_fc,
                              _field_presel_entry_t *f_presel,
                              int qid, _bcm_field_qual_offset_t *q_offset)
{
    int                     ct;
    _field_control_t        *fc;        /* Field control structure. */
    _bcm_field_qual_info_t  *f_qual = NULL;
    _bcm_field_qual_conf_t  *conf_arr = NULL;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == f_presel) || (NULL == q_offset)) {
        return (BCM_E_PARAM);
    }

    /* Retrieve the control structure */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Validate the Qualifier */
    if (0 == BCM_FIELD_QSET_TEST(stage_fc->presel_qset, qid)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Preselector Stage doesn't support the qualifier[%s].\n\r"),
             _field_qual_name(qid)));
        return (BCM_E_PARAM);
    }

    f_qual = stage_fc->f_presel_qual_arr[qid];
    if (f_qual == NULL) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Unable to retrieve Preselector Qualifier[%s] offset.\n\r"),
             _field_qual_name(qid)));
       return BCM_E_INTERNAL;
    }

    for (ct = 0; ct < f_qual->conf_sz; ct++) {
        conf_arr = &f_qual->conf_arr[ct];
        if ((conf_arr->offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_NONE) ||
            (conf_arr->offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L3) ||
            (conf_arr->offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L2)) {
            if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                           bcmFieldQualifyPktFlowType)) {
                if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                        _bcmFieldQualifyPktFlowTypeTunnelNone) &&
                        !(conf_arr->offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_NONE)) {
                    continue;
                } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                               _bcmFieldQualifyPktFlowTypeTunnelL3) &&
                        !(conf_arr->offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L3)) {
                    continue;
                } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                               _bcmFieldQualifyPktFlowTypeTunnelL2) &&
                        !(conf_arr->offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L2)) {
                    continue;
                }
            }
        }

        /* Get qualifier information. */
        sal_memcpy(q_offset,
                   &conf_arr->offset,
                   sizeof(_bcm_field_qual_offset_t));
        return (BCM_E_NONE);
    }

    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _bcm_field_presel_entry_tcam_idx_get
 * Purpose:
 *     Get the TCAM index of an entry ID.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     f_presel     - (IN) Pointer to field logical table entry structure.
 *     lt_fs        - (IN) Pointer to field logical table slice structure.
 *     tcam_idx     - (OUT) Entry tcam index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_presel_entry_tcam_idx_get(int unit, _field_presel_entry_t *f_presel,
                                     _field_lt_slice_t *lt_fs, int *tcam_idx)
{
    /* Input parameters check. */
    if ((NULL == f_presel) || (NULL == lt_fs) || (NULL == tcam_idx)) {
        return (BCM_E_PARAM);
    }

    /* Requested entry structure sanity check. */
    if ((NULL == f_presel->group) || (NULL == f_presel->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Calculate the TCAM index. */
    if (lt_fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
        *tcam_idx = f_presel->hw_index;
    } else {
        *tcam_idx = lt_fs->start_tcam_idx + f_presel->hw_index;
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_th_presel_tcam_key_mask_get
 * Purpose:
 *     Allocate & read from hw preselector tcam key/mask image.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     stage_fc - (IN)     Reference to Field Stage control structure.
 *     inst     - (IN)     Pipe Instance.
 *     f_presel - (IN/OUT) Field Presel Entry.
 *     tcam     - (IN)     Reference to Presel Entry TCAM structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_presel_tcam_key_mask_get(int unit,
                                   _field_stage_t *stage_fc,
                                   int inst,
                                   _field_presel_entry_t *f_presel,
                                   _field_presel_tcam_t  *tcam)
{
    uint32          tcam_key_words;      /* tcam key length */
    soc_mem_t       tcam_mem;            /* TCAM Memory name */


    /* Validate input parameters */
    if (NULL == f_presel || NULL == tcam) {
       return BCM_E_PARAM;
    }

    /* Check if tcam info already present. */
    if (NULL != f_presel->lt_tcam.key) {
        return (BCM_E_NONE);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_IFP_LT,
                                                  _BCM_FIELD_MEM_VIEW_TYPE_TCAM,
                                                  &tcam_mem, NULL));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_EM_LT,
                                                  _BCM_FIELD_MEM_VIEW_TYPE_TCAM,
                                                  &tcam_mem, NULL));
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        int index = 0;
        /* If lt slice is not NULL, use that to get correct memories */
        if (f_presel->lt_fs) {
            if (f_presel->lt_fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
                index = f_presel->lt_fs->slice_number;
            }
        }
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                             _BCM_FIELD_MEM_TYPE_FT_LT_I(index),
                                             _BCM_FIELD_MEM_VIEW_TYPE_TCAM,
                                             &tcam_mem, NULL));
#endif
    } else {
        return BCM_E_PARAM;
    }

    tcam_key_words = BITS2WORDS(soc_mem_field_length(unit,
                                                     tcam_mem,
                                                     KEYf));
    /* Retrieve the key size */
    tcam->key_size = WORDS2BYTES(tcam_key_words);

    /* Allocate Memory for TCAM Key and Mask */
    _FP_XGS3_ALLOC(tcam->key, tcam->key_size,
                    "IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY Key Alloc.");
    if (tcam->key == NULL) {
       return BCM_E_MEMORY;
    }
    _FP_XGS3_ALLOC(tcam->mask, tcam->key_size,
                    "IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY Mask Alloc.");
    if (tcam->mask == NULL) {
       sal_free(tcam->key);
       tcam->key = NULL;
       return BCM_E_MEMORY;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_presel_mem_data_get
 * Purpose:
 *     Allocate & read from hw preselector data image.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     stage_fc - (IN)     Reference to Field Stage control structure.
 *     inst     - (IN)     Pipe Instance.
 *     f_presel - (IN/OUT) Field Presel Entry.
 *     p_data   - (OUT)    Reference to Presel Entry Data structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_presel_mem_data_get(int unit,
                                  _field_stage_t *stage_fc,
                                  int inst,
                                  _field_presel_entry_t *f_presel,
                                  _field_presel_data_t *p_data)
{
    int             rv;                           /* Operation return status. */
    int             data_idx;                           /* Data Mem index.    */
    soc_mem_t       lt_mem;                             /* LT TCAM-DATA mem.  */
    uint32          mem_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Entry buffer.      */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    int             index;
#endif

    /* Validate input parameters */
    if (NULL == f_presel || NULL == p_data ||
        NULL == stage_fc) {
       return BCM_E_PARAM;
    }

    /* Check if tcam info already present. */
    if (NULL != f_presel->lt_data.data) {
        return (BCM_E_NONE);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_IFP_LT,
                                                  _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                                  &lt_mem, NULL));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_EM_LT,
                                                  _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                                  &lt_mem, NULL));
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        index = 0;
        /* If lt slice is not NULL, use that to get correct memories */
        if (f_presel->lt_fs) {
            if (f_presel->lt_fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
                index = f_presel->lt_fs->slice_number;
            }
        }
        /* Getting mem for Data table as combo is not supported */
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_FT_LT_I(index),
                                                  _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                                                  NULL, &lt_mem));
#endif
    } else {
        return BCM_E_PARAM;
    }

    /* Get data field size in logical table entry. */
    if (!SOC_IS_TOMAHAWK3(unit)) {
    p_data->data_size = WORDS2BYTES(soc_mem_field_length(unit,
                                    lt_mem, DATAf));
    } else {
       p_data->data_size = 57;
    }

    /* Allocate Memory for SRAM Data */
    _FP_XGS3_ALLOC(p_data->data, p_data->data_size,
                   "IFP_LOGICAL_TABLE_SELECT Data Alloc.");
    if (p_data->data == NULL) {
       return BCM_E_MEMORY;
    }

    if (!(f_presel->flags & _FP_ENTRY_DIRTY)) {
       /* Get entry Data Entry index. */
       rv = _bcm_field_presel_entry_tcam_idx_get(unit, f_presel,
                                                 f_presel->lt_fs, &data_idx);
       if (BCM_FAILURE(rv)) {
          goto bcm_error;
       }

       /* Read entry part from hardware. */
       rv = soc_mem_read(unit, lt_mem, MEM_BLOCK_ANY, data_idx,
                         mem_entry);
       if (BCM_FAILURE(rv)) {
          goto bcm_error;
       }

    if (!SOC_IS_TOMAHAWK3(unit)) {
       /* Get entry Data value. */
       soc_mem_field_get(unit, lt_mem, mem_entry, DATAf,
                         p_data->data);
    }
    }

    return (BCM_E_NONE);

bcm_error:
    sal_free(p_data->data);
    p_data->data = NULL;
    return rv;
}


/*
 * Function:
 *     _field_presel_qual_value_set
 *
 * Purpose:
 *     Set a qualifier field from the designated data and mask arrays.
 *
 * Parameters:
 *     unit     - (IN) BCM devices number
 *     stage_fc - (IN) Stage field control structure.
 *     inst     - (IN) Pipe Instance.
 *     qi       - (IN) Qualifier offset inside tcam.
 *     f_presel - (IN) Field Presel entry descriptor.
 *     p_data   - (IN) Inserted data.
 *     p_mask   - (IN) Data mask.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_presel_qual_value_set(int unit, _field_stage_t *stage_fc,
                             int inst, _qual_info_t *qi,
                             _field_presel_entry_t *f_presel,
                             uint32 *p_data, uint32 *p_mask)
{
    int                 rv;         /* Operation return status. */
    uint32              u32_mask,
                        *p_fn_data, *p_fn_mask;
    int                 idx, wp, bp, len;

#undef DATA_TOO_BIG_FOR_FIELD
#define DATA_TOO_BIG_FOR_FIELD ((p_data[idx] & ~u32_mask) != 0)

    wp = qi->offset / 32;
    bp = qi->offset & (32 - 1);
    idx = 0;
    p_fn_data = p_fn_mask = NULL;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
              "vverb: qi={offset=%d, width=%d}, data=0x%08x, mask=0x%08x\n"),
               qi->offset, qi->width, *p_data, *p_mask));

    /* Allocate memory for TCAM Key and Mask. */
    rv = _bcm_field_th_presel_tcam_key_mask_get(unit, stage_fc, inst,
                                            f_presel, &f_presel->lt_tcam);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
         "Error: Unable to allocate the Presel Entry TCAM Key and Mask.\n\r")));
       return rv;
    }

    switch (qi->fpf) {
      case KEYf:
           p_fn_data = f_presel->lt_tcam.key;
           p_fn_mask = f_presel->lt_tcam.mask;
           break;

      default:
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "Error: Bad TCAM field=%#05x\n"), qi->fpf));
          return (BCM_E_INTERNAL);
    };

    for (len = qi->width; len > 0; len -= 32) {
        if (bp) {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                /* Chop mask to fit mask field. */
                p_mask[idx] &= u32_mask;
                if (DATA_TOO_BIG_FOR_FIELD) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "FP(unit %d) Error: data=%#x "),
                               unit, *p_data));
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "or mask=%#x too big for field\n"),
                               *p_mask));
                    return (BCM_E_PARAM);
                }
            } else {
                u32_mask = 0xffffffff;
            }

            p_fn_data[wp] &= ~(u32_mask << bp);
            p_fn_data[wp] |= p_data[idx] << bp;

            /* coverity[ptr_arith : FALSE] */
            p_fn_mask[wp] &= ~(u32_mask << bp);
            p_fn_mask[wp++] |= p_mask[idx] << bp;

            p_fn_data[wp] &= ~(u32_mask >> (32 - bp));
            p_fn_data[wp] |= p_data[idx] >> (32 - bp) & ((1 << bp) - 1);

            p_fn_mask[wp] &= ~(u32_mask >> (32 - bp));
            p_fn_mask[wp] |= p_mask[idx] >> (32 - bp) & ((1 << bp) - 1);
        } else {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                /* Chop mask to fit mask field. */
                p_mask[idx] &= u32_mask;
                if (DATA_TOO_BIG_FOR_FIELD) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                               "Error: data=%#x or mask=%#x "
                               "too big for field\n\r"),
                               *p_data, *p_mask));
                    return (BCM_E_PARAM);
                }
                p_fn_data[wp] &= ~u32_mask;
                p_fn_data[wp] |= p_data[idx];
                /* coverity[ptr_arith : FALSE] */
                p_fn_mask[wp] &= ~u32_mask;
                p_fn_mask[wp++] |= p_mask[idx];
            } else {
                p_fn_data[wp] = p_data[idx];
                /* coverity[ptr_arith : FALSE] */
                p_fn_mask[wp++] = p_mask[idx];
            }
        }

        idx++;
    }

    f_presel->flags |= _FP_ENTRY_DIRTY;
    f_presel->flags &= ~(_FP_ENTRY_POLICY_TABLE_ONLY_DIRTY);
    return (BCM_E_NONE);
}
#undef DATA_TOO_BIG_FOR_FIELD


/*
 * Function:
 *     _field_presel_qual_part_value_set
 *
 * Purpose:
 *     Set a preselector qualifier field from the designated
 *     data and mask arrays.
 *
 * Parameters:
 *     unit      - (IN) BCM devices number
 *     stage_fc  - (IN) Stage field control structure.
 *     inst      - (IN) Pipe Instance.
 *     q_offset  - (IN) Qualifier offset inside tcam.
 *     qual_part - (IN) Split qualifier part.
 *     f_presel  - (IN) Field entry descriptor.
 *     p_data    - (IN) Inserted data.
 *     p_mask    - (IN) Data mask.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_presel_qual_part_value_set(int unit, _field_stage_t *stage_fc, int inst,
                                  _bcm_field_qual_offset_t *q_offset,
                                  uint8 qual_part,
                                  _field_presel_entry_t *f_presel,
                                  uint32 *p_data, uint32 *p_mask)
{
    int                       rv;         /* Operation return status. */
    int                       idx = 0,
                              offset = 0;
    _qual_info_t              qi;
    _bcm_field_qual_data_t    data;
    _bcm_field_qual_data_t    mask;

    /* Input parameters check. */
    if ((NULL == q_offset) || (NULL == f_presel) ||
        (NULL == p_data) || (NULL == p_mask)) {
        return (BCM_E_PARAM);
    }

    /* Primary offset handling. */
    sal_memset(&qi, 0, sizeof(_qual_info_t));
    qi.fpf = q_offset->field;
    qi.offset = q_offset->offset[qual_part];
    qi.width = q_offset->width[qual_part];
    /* calculate offset */
    for (idx = 0; idx < qual_part; idx++) {
        offset += q_offset->width[idx];
    }

    /* Split primary portion of the qualifier. */
    rv = _bcm_field_qual_partial_data_get (p_data, offset,
                                           qi.width, data);
    BCM_IF_ERROR_RETURN(rv);
    rv = _bcm_field_qual_partial_data_get(p_mask, offset,
                                          qi.width, mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Program qualifier into tcam */
    rv = _field_presel_qual_value_set(unit, stage_fc, inst, &qi,
                                      f_presel, data, mask);
    return (rv);
}


/*
 * Function:
 *     _bcm_field_presel_qual_value_set
 *
 * Purpose:
 *     Set a preselector qual field from the designated data and mask arrays.
 *
 * Parameters:
 *     unit     - (IN) BCM devices number
 *     stage_fc - (IN) Stage field control structure.
 *     inst     - (IN) Pipe Instance.
 *     q_offset - (IN) Qualifier offset inside tcam.
 *     f_presel - (IN) Field Preselector entry descriptor.
 *     p_data   - (IN) Inserted data.
 *     p_mask   - (IN) Data mask.
 *
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_field_presel_qual_value_set(int unit, _field_stage_t *stage_fc, int inst,
                                 _bcm_field_qual_offset_t *q_offset,
                                 _field_presel_entry_t *f_presel,
                                 uint32 *p_data, uint32 *p_mask)
{
    int    rv;
    uint8  qual_part;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "_bcm_field_qual_value_set\n")));

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == q_offset) || (NULL == f_presel) ||
        (NULL == p_data) || (NULL == p_mask)) {
        return (BCM_E_PARAM);
    }

    /* Iterate for the number of offsets handling. */
    for (qual_part = 0; qual_part < q_offset->num_offsets; qual_part++) {
        if (q_offset->width[qual_part] > 0)
        {
           rv = _field_presel_qual_part_value_set(unit, stage_fc, inst,
                                                  q_offset, qual_part, f_presel,
                                                  p_data, p_mask);
           BCM_IF_ERROR_RETURN(rv);
        }
    }

    return (BCM_E_NONE);
}


/*
 * Function: _bcm_field_presel_qualify_set
 *
 * Purpose:
 *     Qualify the presel for the given qualifier
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     entry     - (IN) Entry ID contains preselector Id.
 *     qual      - (IN) Qualifier Id.
 *     data      - (IN) Data
 *     mask      - (IN) Mask
 *
 * Returns:
 *     BCM_E_PARAM     - presel contained in entry id is not valid
 *                       or presel == NULL.
 *     BCM_E_NOT_FOUND - presel id is not found.
 *     BCM_E_NONE      - returns on Success
 */

int
_bcm_field_presel_qualify_set(int unit,
                              bcm_field_entry_t entry,
                              bcm_field_qualify_t qual,
                              uint32 *data,
                              uint32 *mask)
{
    int                       rv;               /* Operation return status. */
    int                       inst;             /* Pipe Instance. */
    int                       width;            /* Qualifier Width. */
    bcm_field_presel_t        presel;           /* Presel id. */
    _field_presel_entry_t     *f_presel = NULL; /* Presel Entry descriptor. */
    _field_control_t          *fc;              /* Field control structure. */
    _field_stage_t            *stage_fc;        /* Field Stage structure. */
    _bcm_field_qual_offset_t  q_offset;         /* Qualifier offset instance. */
    _field_stage_id_t         stage_id;         /* Stage id. */

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Retrieve the control structure */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }


    /* Verify and update the mandate qualifier. */
    if (qual == bcmFieldQualifyStage) {
       bcm_field_stage_t stage;

       stage = *data;
       if (_BCM_FIELD_PRESEL_IS_STAGE_SUPPORTED(stage) == FALSE) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "ERROR: Preselector doesn't support the given stage:[%d].\n\r"),
              stage));
          return BCM_E_PARAM;
       }

       if ((stage == bcmFieldStageIngressFlowtracker) &&
           ((!soc_feature(unit, soc_feature_field_flowtracker_support)) ||
            (soc_property_get(unit, spn_FLOWTRACKER_ENABLE, 1) != 1))) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "ERROR: Stage is not available.")));
           return BCM_E_UNAVAIL;
       }

       if (BCM_FIELD_QSET_TEST(f_presel->p_qset, qual)) {
          if (((stage == bcmFieldStageIngress) &&
               (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngress))) ||
              ((stage == bcmFieldStageIngressExactMatch) &&
               (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                          bcmFieldQualifyStageIngressExactMatch))) ||
              ((stage == bcmFieldStageIngressFlowtracker) &&
               (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                          bcmFieldQualifyStageIngressFlowtracker)))) {
              return BCM_E_NONE;
          }

         /*
          * If the entry's lt_tcam->key is not NULL, indicates some other
          * non-stage qualifier is associated to the presel entry.
          */
          if (f_presel->lt_tcam.key != NULL) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "ERROR: Stage is already associated with the given Presel"
                " Entry[%d] and the entry is in use.\n\r"), presel));
             return BCM_E_BUSY;
          }
       }

       BCM_FIELD_QSET_INIT(f_presel->p_qset);
       BCM_FIELD_QSET_ADD(f_presel->p_qset, bcmFieldQualifyStage);
       if (stage == bcmFieldStageIngress) {
            BCM_FIELD_QSET_ADD(f_presel->p_qset, bcmFieldQualifyStageIngress);
       } else if (stage == bcmFieldStageIngressExactMatch) {
            BCM_FIELD_QSET_ADD(f_presel->p_qset,
                                       bcmFieldQualifyStageIngressExactMatch);
       } else  { /* stage == bcmFieldStageIngressFlowtracker */
            BCM_FIELD_QSET_ADD(f_presel->p_qset,
                                       bcmFieldQualifyStageIngressFlowtracker);
       }

       return BCM_E_NONE;
    } else {
        /* Verify if the Presel Entry is already associated with the stage. */
        if (BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyStage)
                                                                     == FALSE) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Preselector Entry[%d] doesn't associated with any stage."
               "\n\rIt is mandate to qualify the Stage first.\n\r"), presel));
            return BCM_E_PARAM;
        }
    }

    /* Retrieve the stage id. */
    if (BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyStageIngress)) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressExactMatch)) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressFlowtracker)) {
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
#endif
    } else {
        return BCM_E_INTERNAL;
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, stage_id, &stage_fc));

    /* Pkt flow mandate qualifier */
    if (qual == bcmFieldQualifyPktFlowType) {
        _bcm_field_internal_qualify_t _qual;

        if (BCM_FIELD_QSET_TEST(f_presel->p_qset, qual)) {
           return BCM_E_EXISTS;
        }

        switch (*data) {
           case bcmFieldPktFlowTypeTunnelNone:
               _qual = _bcmFieldQualifyPktFlowTypeTunnelNone;
               break;
           case bcmFieldPktFlowTypeTunnelL2:
               _qual = _bcmFieldQualifyPktFlowTypeTunnelL2;
               break;
           case bcmFieldPktFlowTypeTunnelL3:
               _qual = _bcmFieldQualifyPktFlowTypeTunnelL3;
               break;
           default:
               return BCM_E_PARAM;
        }
        BCM_FIELD_QSET_ADD(f_presel->p_qset, qual);
        BCM_FIELD_QSET_ADD(f_presel->p_qset, _qual);
        return BCM_E_NONE;
    }

    /* Verify if the given Qualifier belongs to Preselector QSET */
    if (BCM_FIELD_QSET_TEST(stage_fc->presel_qset, qual) == FALSE) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Preselector Stage doesn't support the qualifier[%s].\n\r"),
             _field_qual_name(qual)));
       return BCM_E_PARAM;
    }


    /* Retrieve the qualifier offset */
    rv = _field_presel_qual_offset_get(unit, stage_fc, f_presel,
                                       qual, &q_offset);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "ERROR: Unable to retrieve the qualifier's offset[%d].\n\r"), qual));
       return rv;
    }

    /* Check if the qualifier is associated to PktFlowType. */
    if ((q_offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L2) ||
        (q_offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_L3) ||
        (q_offset.flags & _BCM_FIELD_QUAL_OFFSET_FLOW_TYPE_TUNNEL_NONE)) {
       if (!BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyPktFlowType)) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Preselector Entry[%d] doesn't associated with any packet flow."
               "\n\rIt is mandate to associate the packet flow 'bcmFieldQualifyPktFlowType'"
               " prior to '%s' configuration.\n\r"),
               presel, _field_qual_name(qual)));
          return BCM_E_PARAM;
       }
    }

    /* Combined width of a qualifier for a given part */
    _BCM_FIELD_QUAL_MULTI_OFFSET_WIDTH(&q_offset, width);

    /* Validate the Qualifier Data with the offset's width. */
    if (width < 32) {
       if (data[0] & ~((1 << width) - 1)) {
          return BCM_E_PARAM;
       }
    }

    /*
     * Assuming Instance as 0, Pipe instance is unknown at this point and
     * and also it doesn't make any difference as key and mask fields for
     * a TCAM is same for all the instances.
     */
    inst = 0;

    /* Program data/mask pair to tcam buffer. */
    /* coverity[callee_ptr_arith : FALSE] */
    rv = _bcm_field_presel_qual_value_set(unit, stage_fc, inst, &q_offset,
                                          f_presel, data, mask);
    BCM_IF_ERROR_RETURN(rv);

    BCM_FIELD_QSET_ADD(f_presel->p_qset, qual);
    f_presel->flags |= _FP_ENTRY_DIRTY;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_presel_qual_value_get
 *
 * Purpose:
 *     Retrieve a qualifier value from the designated data/mask
 *     field pair, returning them to *p_data and *p_mask.
 *
 * Returns:
 *     BCM_E_NONE - Success
 */
STATIC int
_field_presel_qual_value_get(int unit, _field_stage_t *stage_fc, int inst,
                             _qual_info_t *qi, _field_presel_entry_t *f_presel,
                             uint32 *p_data, uint32 *p_mask)
{
    int                 rv;               /* Operation return status. */
    uint32              *p_fn_data, *p_fn_mask;
    int                 idx, wp, bp, len;

    /* Input parameters check. */
    if ((NULL == f_presel) || (NULL == p_data) ||
        (NULL == qi) || (NULL == p_mask)) {
        return (BCM_E_PARAM);
    }

    wp = qi->offset / 32;
    bp = qi->offset & (32 - 1);
    idx = 0;
    p_fn_data = p_fn_mask = NULL;

    /* Allocate memory for TCAM Key and Mask. */
    rv = _bcm_field_th_presel_tcam_key_mask_get(unit, stage_fc, inst,
                                            f_presel, &f_presel->lt_tcam);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
         "Error: Unable to allocate the Presel Entry TCAM Key and Mask.\n\r")));
       return rv;
    }

    switch (qi->fpf) {
        case KEYf:
            p_fn_data = f_presel->lt_tcam.key;
            p_fn_mask = f_presel->lt_tcam.mask;
            break;
        default:
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "Error: Bad TCAM field=%#05x\n"), qi->fpf));
          return (BCM_E_INTERNAL);
    };

    for (len = qi->width; len > 0; len -= 32) {
        if ((NULL == p_fn_data) || (NULL == p_fn_mask)) {
            /*
             * IPBM Overlay qualifier set in the group but
             * not qualified for this entry.
             */
            p_data[idx] = 0x0;
            p_mask[idx] = 0x0;
        } else {
            if (bp) {
                p_data[idx] = (p_fn_data[wp] >> bp) & ((1 << (32 - bp)) - 1);
                /* coverity[ptr_arith : FALSE] */
                p_mask[idx] = (p_fn_mask[wp++] >> bp) & ((1 << (32 - bp)) - 1);
                p_data[idx] |= p_fn_data[wp] << (32 - bp);
                p_mask[idx] |= p_fn_mask[wp] << (32 - bp);
            } else {
                p_data[idx] = p_fn_data[wp];
                /* coverity[ptr_arith : FALSE] */
                p_mask[idx] = p_fn_mask[wp++];
            }
        }

        if (len < 32) {
            p_data[idx] &= ((1 << len) - 1);
            p_mask[idx] &= ((1 << len) - 1);
        }

        idx++;
    }

    return (BCM_E_NONE);
}



/*
 * Function:
 *     _field_presel_qual_part_value_get
 *
 * Purpose:
 *     Get a qualifier field from the designated data and mask arrays.
 *
 * Parameters:
 *     unit       - (IN) BCM devices number
 *     stage_fc   - (IN) Stage field control structure.
 *     inst       - (IN) Pipe Instance.
 *     q_offset   - (IN) Qualifier offset inside tcam.
 *     f_presel   - (IN) Field Presel entry descriptor.
 *     p_data     - (IN) Inserted data.
 *     p_mask     - (IN) Data mask.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_presel_qual_part_value_get(int unit, _field_stage_t *stage_fc, int inst,
                                  _bcm_field_qual_offset_t *q_offset,
                                  uint8 qual_part,
                                  _field_presel_entry_t *f_presel,
                                  _bcm_field_qual_data_t p_data,
                                  _bcm_field_qual_data_t p_mask)
{
    _qual_info_t qi;
    int rv;
    _bcm_field_qual_data_t data;
    _bcm_field_qual_data_t mask;
    int offset = 0, idx = 0;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == q_offset) || (NULL == f_presel) ||
        (NULL == p_data) || (NULL == p_mask)) {
        return (BCM_E_PARAM);
    }

    /* Primary offset handling. */
    sal_memset(&qi, 0, sizeof(_qual_info_t));

    _FP_QUAL_DATA_CLEAR(data);
    _FP_QUAL_DATA_CLEAR(mask);
    qi.fpf = q_offset->field;
    qi.offset = q_offset->offset[qual_part];
    qi.width = q_offset->width[qual_part];
    /* calculate offset */
    for (idx = 0; idx < qual_part; idx++) {
        offset += q_offset->width[idx];
    }

    /* Read qualifier information from tcam.. */
    rv = _field_presel_qual_value_get(unit, stage_fc, inst, &qi,
                                      f_presel, data, mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Split primary portion of the qualifier. */
    rv = _bcm_field_qual_partial_data_set(data, offset,
                                          qi.width, p_data);
    BCM_IF_ERROR_RETURN(rv);
    rv = _bcm_field_qual_partial_data_set(mask, offset,
                                          qi.width, p_mask);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_presel_value_get
 *
 * Purpose:
 *     Get a qualifier field from the designated data and mask arrays.
 *
 * Parameters:
 *     unit     - (IN) BCM devices number
 *     stage_fc - (IN) Stage field control structure.
 *     inst     - (IN) Pipe Instance.
 *     q_offset - (IN) Qualifier offset inside tcam.
 *     f_presel - (IN) Field presel entry descriptor.
 *     p_data   - (IN) Inserted data.
 *     p_mask   - (IN) Data mask.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_presel_qual_value_get(int unit, _field_stage_t *stage_fc, int inst,
                                 _bcm_field_qual_offset_t *q_offset,
                                 _field_presel_entry_t *f_presel,
                                 _bcm_field_qual_data_t p_data,
                                 _bcm_field_qual_data_t p_mask)
{
    int rv, qual_part;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == q_offset) || (NULL == f_presel) ||
        (NULL == p_data) || (NULL == p_mask)) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_DATA_CLEAR(p_data);
    _FP_QUAL_DATA_CLEAR(p_mask);

    /* offset handling */
    for (qual_part = 0; qual_part < q_offset->num_offsets; qual_part++) {
        if (q_offset->width[qual_part] > 0)
        {
           rv = _field_presel_qual_part_value_get(unit, stage_fc, inst,
                                                  q_offset, qual_part,
                                                  f_presel, p_data, p_mask);
           BCM_IF_ERROR_RETURN(rv);
        }
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_presel_qualifier_get
 *
 * Purpose:
 *     Get qualifier match value and mask.
 *
 * Parameters:
 *     unit      - BCM device number
 *     entry     - Entry id.
 *     qual_id   - qualifier id.
 *     q_data    - Qualifier data array.
 *     q_mask    - Qualifier mask array.
 * Returns:
 *     Nothing.
 */
int
_bcm_field_presel_qualifier_get(int unit,
                                bcm_field_entry_t entry,
                                int qual_id,
                                _bcm_field_qual_data_t q_data,
                                _bcm_field_qual_data_t q_mask)
{
    _bcm_field_qual_offset_t  q_offset;    /* Qualifier offset in the tcam. */
    _field_presel_entry_t     *f_presel;   /* Field entry structure.        */
    bcm_field_presel_t        presel;      /* Field Preselector ID.         */
    int                       rv;          /* Operation return status.      */
    int                       inst;        /* Pipe Instance.                */
    _field_stage_t           *stage_fc;
    _field_control_t         *fc;
    _field_stage_id_t         stage_id;    /* Stage id. */

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Retrieve the control structure */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }

    /* Verify and update the mandate qualifier. */
    if (qual_id == bcmFieldQualifyStage) {
       if (BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyStageIngress)) {
           q_data[0] = q_mask[0] = bcmFieldStageIngress;
       } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                       bcmFieldQualifyStageIngressExactMatch)) {
           q_data[0] = q_mask[0] = bcmFieldStageIngressExactMatch;
#if defined(BCM_FLOWTRACKER_SUPPORT)
       } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                       bcmFieldQualifyStageIngressFlowtracker)) {
           q_data[0] = q_mask[0] = bcmFieldStageIngressFlowtracker;
#endif
       }
       return BCM_E_NONE;
    }

    if (qual_id == bcmFieldQualifyPktFlowType) {
       if (BCM_FIELD_QSET_TEST(f_presel->p_qset, _bcmFieldQualifyPktFlowTypeTunnelNone)) {
           q_data[0] = q_mask[0] = bcmFieldPktFlowTypeTunnelNone;
       } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset, _bcmFieldQualifyPktFlowTypeTunnelL3)) {
           q_data[0] = q_mask[0] = bcmFieldPktFlowTypeTunnelL3;
       } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset, _bcmFieldQualifyPktFlowTypeTunnelL2)) {
           q_data[0] = q_mask[0] = bcmFieldPktFlowTypeTunnelL2;
       } else {
           return BCM_E_NOT_FOUND;
       }
       return BCM_E_NONE;
    }

    /* Retrieve the stage id. */
    if (BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyStageIngress)) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressExactMatch)) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressFlowtracker)) {
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
#endif
    } else {
        return BCM_E_INTERNAL;
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, stage_id, &stage_fc));


    /* Verify if the given Qualifier belongs to Preselector QSET */
    if (BCM_FIELD_QSET_TEST(stage_fc->presel_qset, qual_id) == FALSE) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Preselector Stage doesn't support the qualifier[%s].\n\r"),
             _field_qual_name(qual_id)));
       return BCM_E_PARAM;
    }

    /* Initialize q_offset to NULL */
    sal_memset(&q_offset, 0x0, sizeof(q_offset));

    /* Retrieve the Qualifier offsets */
    rv = _field_presel_qual_offset_get(unit, stage_fc, f_presel,
                                       qual_id, &q_offset);
    if (BCM_FAILURE(rv)) {
       return rv;
    }

    /* Instance setting to 0 */
    inst = 0;

    /* Read qualifier match value and mask. */
    rv = _bcm_field_presel_qual_value_get(unit, stage_fc, inst, &q_offset,
                                          f_presel, q_data, q_mask);
    return (BCM_E_NONE);
}

#if defined(BCM_TOMAHAWK_SUPPORT)
/*
 * Dump Preselector Qualifiers.
 */
int
_bcm_field_presel_qualifiers_dump(int unit, bcm_field_presel_t presel_id)
{
    _bcm_field_qual_offset_t  q_offset;    /* Qualifier offset in the tcam. */
    _field_presel_entry_t     *f_presel;   /* Field entry structure.        */
    bcm_field_entry_t         entry;       /* Field entry structure.        */
    int                       rv;          /* Operation return status.      */
    int                       inst = 0;    /* Pipe Instance.                */
    int                       idx = 0;
    int                       num_qual = 0;
    _field_stage_t           *stage_fc;
    _field_control_t         *fc;
    _bcm_field_qual_data_t   q_data;
    _bcm_field_qual_data_t   q_mask;
    int                      qual_id;
    _field_stage_id_t        stage_id;         /* Stage id. */
    bcm_field_src_class_t    data;        /* MixedSrcClassId data */
    bcm_field_src_class_t    mask;        /* MixedSrcClassId mask */

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Retrieve the control structure */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Check if presel ID present in presel_set */
    if (!BCM_FIELD_PRESEL_TEST(fc->presel_info->presel_set, presel_id)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "ERROR: Preselector ID[%d] not found.\n\r"), presel_id));
       return BCM_E_NOT_FOUND;
    }

    /* Retrieve the Presel Entry */
    rv = _bcm_field_presel_entry_get(unit, presel_id, &f_presel);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "ERROR: Couldn't retrieve the Preselector Entry for the given"
              " Presel ID[%d].\n\r"), presel_id));
        return rv;
    }

    /* Retrieve the stage id. */
    if (BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyStageIngress)) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressExactMatch)) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressFlowtracker)) {
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
#endif
    } else {
        return BCM_E_INTERNAL;
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, stage_id, &stage_fc));

    /* Get the entry ID from presel ID */
    BCM_FIELD_PRESEL_ENTRY_SET(entry, presel_id);

    for (qual_id = 0; qual_id < _bcmFieldQualifyCount; qual_id++) {
        /* Verify if the given Qualifier belongs to Preselector QSET */
        if (BCM_FIELD_QSET_TEST(f_presel->p_qset, qual_id) == FALSE) {
           continue;
        }

        if ((qual_id == bcmFieldQualifyStage) ||
            (qual_id == bcmFieldQualifyStageIngress) ||
            (qual_id == bcmFieldQualifyStageIngressExactMatch) ||
            (qual_id == bcmFieldQualifyStageIngressFlowtracker) ||
            (qual_id == _bcmFieldQualifyPktFlowTypeTunnelNone) ||
            (qual_id == _bcmFieldQualifyPktFlowTypeTunnelL2) ||
            (qual_id == _bcmFieldQualifyPktFlowTypeTunnelL3) ||
            (qual_id == bcmFieldQualifyPktFlowType)) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit, "  %-10s\n\r"), _field_qual_name(qual_id)));
        /* Initialize q_offset to NULL */
        sal_memset(&q_offset, 0x0, sizeof(q_offset));

        /* Retrieve the Qualifier offsets */
        rv = _field_presel_qual_offset_get(unit, stage_fc, f_presel,
                                           qual_id, &q_offset);
        if (BCM_FAILURE(rv)) {
           return rv;
        }

        /* Instance setting to 0 */
        inst = 0;

        _FP_QUAL_DATA_CLEAR(q_data);
        _FP_QUAL_DATA_CLEAR(q_mask);

        /* Read qualifier match value and mask. */
        BCM_IF_ERROR_RETURN (_bcm_field_presel_qual_value_get(unit, stage_fc,
                                 inst, &q_offset, f_presel, q_data, q_mask));
        for (idx = 0; idx <q_offset.num_offsets; idx++) {
            LOG_CLI((BSL_META_U(unit, "\tOffset: %-2d Width: %-2d\n\r"),
                                q_offset.offset[idx], q_offset.width[idx]));
        }

        LOG_CLI((BSL_META_U(unit, "\tData: 0x%x\n\r"), q_data[0]));
        LOG_CLI((BSL_META_U(unit, "\tMask: 0x%x\n\r"), q_mask[0]));

        /* if qualifier is MixedSrcClassId */
        /* print the detailed data and mask */
        if ((qual_id == bcmFieldQualifyMixedSrcClassId)) {
            BCM_IF_ERROR_RETURN(
               _bcm_field_th_qualify_MixedSrcClassId_get(unit,
                        entry, f_presel->mixed_src_class_pbmp,
                           q_data[0], q_mask[0], &data, &mask));
            if (mask.src_class_field != 0) {
                LOG_CLI((BSL_META_U(unit, "\tSrc Class Data: 0x%x\n\r"), data.src_class_field));
                LOG_CLI((BSL_META_U(unit, "\tSrc Class Mask: 0x%x\n\r"), mask.src_class_field));
            }
            if (mask.dst_class_field != 0) {
                LOG_CLI((BSL_META_U(unit, "\tDst Class Data: 0x%x\n\r"), data.dst_class_field));
                LOG_CLI((BSL_META_U(unit, "\tDst Class Mask: 0x%x\n\r"), mask.dst_class_field));
            }
            if (mask.udf_class != 0) {
                LOG_CLI((BSL_META_U(unit, "\tUdf Class Data: 0x%x\n\r"), data.udf_class));
                LOG_CLI((BSL_META_U(unit, "\tUdf Class Mask: 0x%x\n\r"), mask.udf_class));
            }
            if (mask.intf_class_port != 0) {
                LOG_CLI((BSL_META_U(unit, "\tIntf Class Port Data: 0x%x\n\r"), data.intf_class_port));
                LOG_CLI((BSL_META_U(unit, "\tIntf Class Port Mask: 0x%x\n\r"), mask.intf_class_port));
            }
            if (mask.intf_class_l2 != 0) {
                LOG_CLI((BSL_META_U(unit, "\tIntf Class L2 Data: 0x%x\n\r"), data.intf_class_l2));
                LOG_CLI((BSL_META_U(unit, "\tIntf Class L2 Mask: 0x%x\n\r"), mask.intf_class_l2));
            }
        }
        /* Reference count */
        num_qual = 1;
    }

    if (num_qual == 0) {
       LOG_CLI((BSL_META_U(unit, "No Qualifiers Associated.\n\r")));
    }
    return BCM_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */

/*
 * Function:
 *     _bcm_field_presel_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     stage_fc - (IN) Stage field control structure.
 *     inst     - (IN) Pipe Instance.
 *     f_presel - (IN) Field Preselector entry structure.
 *     fa       - (IN) Field action structure.
 *     action   - (IN) Field Action of type bcmFieldActionXXX.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_field_presel_action_params_check(int unit,
                                      _field_stage_t *stage_fc,
                                      int inst,
                                      _field_presel_entry_t *f_presel,
                                      _field_action_t *fa)
{
    _field_action_params_t      params; /* Field Params Structutre. */
    bcm_field_action_t          action;  /* Field Action of Type bcmFieldActionXXX. */
    soc_mem_t                   data_mem;

    /* Validate input parameters */
    if (NULL == stage_fc || NULL == f_presel || NULL == fa) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META("Invalid Parameters\n")));
        return BCM_E_PARAM;
    }

    sal_memset(&params , 0, sizeof(params));

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_IFP_LT,
                                                  _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                                                  NULL, &data_mem));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_EM_LT,
                                                  _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                                                  NULL, &data_mem));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc, inst,
                                                  _BCM_FIELD_MEM_TYPE_FT_LT,
                                                  _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                                                  NULL, &data_mem));
    } else {
        return BCM_E_PARAM;
    }

    action = fa->action;
    switch (action) {
       case bcmFieldActionGroupClassSet:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
                return BCM_E_PARAM;
            }
            PolicyCheck(unit, data_mem, LOGICAL_TABLE_CLASS_IDf, fa->param[0]);
            break;
       case bcmFieldActionGroupLookupDisable:
            break;
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
       case bcmFieldActionFlowtrackerTimestampDelayMode:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                if (bcmFieldFlowtrackerTimestampDelayModeCount <= fa->param[0]) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }
            break;
#endif
       default:
            return BCM_E_PARAM;
            break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_presel_entry_action_set
 * Purpose:
 *     Set the action to the given Preselector Entry Data buffer
 * Parameters:
 *     unit     - (IN) BCM device number
 *     data_mem - (IN) LT Data Memory
 *     f_presel - (IN) Reference to Preselector Entry structure.
 *     fa       - (IN) field action
 *     buf      - (OUT) Entry data buffer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_presel_entry_action_set(int unit, soc_mem_t data_mem,
                               _field_presel_entry_t *f_presel,
                               _field_action_t *fa, uint32 *buf)
{
    /* Input parameters check. */
    if ((NULL == f_presel) || (NULL == fa) || (NULL == buf)) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
       case bcmFieldActionGroupClassSet:
            soc_mem_field32_set(unit, data_mem, buf, LOGICAL_TABLE_CLASS_IDf,
                                fa->param[0]);
            break;
       case bcmFieldActionGroupLookupDisable:
            if (soc_mem_field_valid(unit, data_mem, ENABLEf)) {
                soc_mem_field32_set(unit, data_mem, buf, ENABLEf, 0);
            }
            if (soc_mem_field_valid(unit, data_mem, GROUP_LOOKUP_ENABLEf)) {
                soc_mem_field32_set(unit, data_mem, buf, GROUP_LOOKUP_ENABLEf, 0);
            }
            break;
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
       case bcmFieldActionFlowtrackerTimestampDelayMode:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                if (fa->param[0] == bcmFieldFlowtrackerTimestampDelayModePTP) {
                    soc_mem_field32_set(unit, data_mem, buf, DELAY_MODEf, 0);
                } else if (fa->param[0] == bcmFieldFlowtrackerTimestampDelayModeNTP) {
                    soc_mem_field32_set(unit, data_mem, buf, DELAY_MODEf, 1);
                } else {
                    soc_mem_field32_set(unit, data_mem, buf, DELAY_MODEf, 2);
                }
            } else {
                return BCM_E_INTERNAL;
            }
            break;
#endif
       default:
            return BCM_E_INTERNAL;
            break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_presel_entry_actions_set
 * Purpose:
 *     Set the actions to the given Preselector Entry Data buffer
 * Parameters:
 *     unit     - (IN) BCM device number
 *     data_mem - (IN) LT Data Memory
 *     f_presel - (IN) Reference to Preselector Entry structure.
 *     fa       - (IN) field action
 *     buf      - (OUT) Entry data buffer.
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 */
int
_bcm_field_presel_entry_actions_set(int unit, soc_mem_t data_mem,
                                    _field_presel_entry_t *f_presel,
                                    uint32 *buf)
{
    _field_action_t         *fa_iter;          /* Presel actions iterator. */

    /* Input parameters check. */
    if ((NULL == f_presel) || (NULL == buf)) {
        return (BCM_E_PARAM);
    }

    /* Assign Action to the Preselector entry */
    for (fa_iter = f_presel->actions; fa_iter != NULL; fa_iter = fa_iter->next)
    {
       BCM_IF_ERROR_RETURN
           (_field_presel_entry_action_set(unit, data_mem, f_presel,
                                           fa_iter, buf));
    }

    return BCM_E_NONE;
}


/*
 * Function: _bcm_field_presel_action_add
 *
 * Purpose:
 *     Associate the action descriptor to the preselector entry.
 *
 * Parameters:
 *     unit  - (IN) BCM device number.
 *     fc    - (IN) Field control structure.
 *     entry - (IN) Field Preselector Entry id.
 *     fa    - (IN) Field action structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_presel_action_add(int unit, _field_control_t *fc,
                             bcm_field_entry_t entry, _field_action_t *fa)
{
    int                      rv;               /* Operation return status. */
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    _field_action_t         *fa_iter;          /* Presel actions iterator. */
    _field_action_t         *fa_prev_ptr=NULL; /* Action previous pointer. */
    _field_stage_t          *stage_fc;         /* Reference to stage control. */
    _field_stage_id_t        stage_id;         /* Stage id. */
    bcm_field_presel_t       presel;           /* Field Preselector ID.    */

    /* Input parameters check. */
    if ((NULL == fc) || (NULL == fa)) {
        return (BCM_E_PARAM);
    }

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }

    /* Verify if the Presel ID is already in USE */
    if (BCM_FIELD_PRESEL_TEST(fc->presel_info->operational_set, presel) &&
        !(f_presel->flags & _FP_ENTRY_DIRTY)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "ERROR: Preselector ID[%d] is in USE.\n\r"), presel));
       return BCM_E_BUSY;
    }

    /* Retrieve the stage id. */
    if (BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyStageIngress)) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressExactMatch)) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressFlowtracker)) {
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
#endif
    } else {
        return BCM_E_INTERNAL;
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, stage_id, &stage_fc));

    /* Verify the Params */
    rv = _bcm_field_presel_action_params_check(unit, stage_fc, 0, f_presel, fa);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Invalid Preselector Action Parameters.\n\r")));
       return rv;
    }

    /* Assign Action to the Preselector entry */
    for (fa_iter = f_presel->actions;
         fa_iter != NULL;
         fa_iter = fa_iter->next) {
         if (fa_iter->action == fa->action) {
            fa->next = fa_iter->next;
            if (fa_prev_ptr == NULL) {
               f_presel->actions = fa;
            } else {
               fa_prev_ptr->next = fa;
            }
            sal_free (fa_iter);
            return BCM_E_NONE;
         }
         fa_prev_ptr = fa_iter;
    }

    /* Add action to the front of presel entry's linked-list */
    fa->next = f_presel->actions;
    f_presel->actions = fa;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_presel_action_delete
 * Purpose:
 *      Delete field action from a field preselector entry.
 * Parameters:
 *      unit   - (IN) BCM device number
 *      entry  - (IN) Entry ID
 *      action - (IN) Action to remove (bcmFieldActionXXX)
 *      param0 - (IN) Action parameter (use 0 if not required)
 *      param1 - (IN) Action parameter (use 0 if not required)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_field_presel_action_delete(int unit, bcm_field_entry_t entry,
                                bcm_field_action_t action,
                                uint32 param0, uint32 param1)
{
    int                     rv;                /* Operation return status. */
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    _field_action_t         *fa_iter;          /* Presel actions iterator. */
    _field_action_t         *fa_prev_ptr=NULL; /* Action previous pointer. */
    _field_control_t        *fc;               /* Field control structure. */
    bcm_field_presel_t      presel;            /* Field Preselector ID.    */

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }

    /* Retrieve the control structure */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Verify if the Presel ID is already in USE */
    if (BCM_FIELD_PRESEL_TEST(fc->presel_info->operational_set, presel) &&
        !(f_presel->flags & _FP_ENTRY_DIRTY)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "ERROR: Preselector ID[%d] is in USE.\n\r"), presel));
       return BCM_E_BUSY;
    }

    /* Search and Delete the Action from the Preselector entry */
    for (fa_iter = f_presel->actions;
         fa_iter != NULL;
         fa_iter = fa_iter->next) {
         if (fa_iter->action == action) {
            if (fa_prev_ptr == NULL) {
               f_presel->actions = fa_iter->next;
            } else {
               fa_prev_ptr->next = fa_iter->next;
            }
            sal_free (fa_iter);
            return BCM_E_NONE;
         }
         fa_prev_ptr = fa_iter;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_presel_action_get
 *
 * Purpose:
 *     Get parameters associated with the preselection entry action
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     entry  - (IN) Preselector Entry ID
 *     action - (IN) Action to perform (bcmFieldActionXXX)
 *     param0 - (OUT) Action parameter
 *     param1 - (OUT) Action parameter
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NOT_FOUND - No matching Action for entry
 *     BCM_E_PARAM     - paramX is NULL
 *     BCM_E_NONE      - Success
 */

int
_bcm_field_presel_action_get(int unit,
                             bcm_field_entry_t entry,
                             bcm_field_action_t action,
                             uint32 *param0,
                             uint32 *param1)
{
    int                     rv;                /* Operation return status. */
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    _field_action_t         *fa;               /* Presel actions iterator. */
    _field_control_t        *fc;               /* Field control structure. */
    bcm_field_presel_t      presel;            /* Field Preselector ID.    */

    /* Input parameters check. */
    if ((NULL == param0) || (NULL == param1)) {
        return (BCM_E_PARAM);
    }

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }

    /* Retrieve the control structure */
    FP_LOCK(unit);
    rv = _field_control_get(unit, &fc);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    /* Find matching action in the entry */
    for (fa = f_presel->actions; fa != NULL; fa = fa->next) {
        if (fa->action == action) {
            break;
        }
    }

    if (fa == NULL) {
        FP_UNLOCK(unit);
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Error: Action[%d] not found in the preselector entry[0x%x]\n\r"),
               action, entry));
        return (BCM_E_NOT_FOUND);
    }

    *param0 = fa->param[0];
    *param1 = fa->param[1];

    /* Unlock the module. */
    FP_UNLOCK(unit);
    return (rv);
}

/*
 * Function: _bcm_field_presel_action_remove_all
 *
 * Purpose:
 *     Remove all actions from a preselector entry rule.
 *
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Preselector Entry ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NONE      - Success
 */

STATIC int
_bcm_field_presel_action_remove_all(int unit,
                                    bcm_field_entry_t entry)
{
    int                     rv;                /* Operation return status. */
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    _field_action_t         *fa;               /* Presel actions iterator. */
    _field_action_t         *fa_ptr=NULL;      /* Action previous pointer. */
    _field_control_t        *fc;               /* Field control structure. */
    bcm_field_presel_t      presel;            /* Field Preselector ID.    */


    /* Get field control structure. */
    FP_LOCK(unit);
    rv = _field_control_get(unit, &fc);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

   /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       FP_UNLOCK(unit);
       return rv;
    }

    /* Verify if the Presel ID is already in USE */
    if (BCM_FIELD_PRESEL_TEST(fc->presel_info->operational_set, presel) &&
        !(f_presel->flags & _FP_ENTRY_DIRTY)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "ERROR: Preselector ID[%d] is in USE.\n\r"), presel));
       return BCM_E_BUSY;
    }

    /* start at the head of the actions list and burn them up */
    fa = f_presel->actions;

    /* Search and Delete the Action from the Preselector entry */
    while (fa != NULL) {
       fa_ptr = fa;
       f_presel->actions = fa = fa->next;
       sal_free (fa_ptr);
    }

    FP_UNLOCK(unit);
    return (BCM_E_NONE);
}

int
_bcm_field_presel_group_prio_sort(int unit, _field_group_t *fg)
{
   _field_presel_entry_t  *f_presel[_FP_PRESEL_ENTRIES_MAX_PER_GROUP];
   _field_presel_entry_t  *f_presel_tmp;
   int i,j;

    /* Input parameters check. */
    if (NULL == fg) {
       return (BCM_E_PARAM);
    }

    sal_memcpy(f_presel, fg->presel_ent_arr,
            sizeof(_field_presel_entry_t *) * _FP_PRESEL_ENTRIES_MAX_PER_GROUP);

    for (i = 0; i < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; i++) {
        for (j = i; j < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; j++) {
            if (f_presel[i] && f_presel[j]) {
               if (f_presel[i]->priority < f_presel[j]->priority) {
                   f_presel_tmp = f_presel[i];
                   f_presel[i] = f_presel[j];
                   f_presel[j] = f_presel_tmp;
               }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_presel_group_add
 *
 * Purpose:
 *     Add preselector Id to a field group structure
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     fg     - (IN) Reference to Field Group structure
 *     presel - (IN) Preselector Id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_presel_group_add(int unit, _field_group_t *fg,
                            bcm_field_presel_t presel)
{
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    _field_control_t        *fc;               /* Field control structure. */
    _field_stage_id_t        stage_id;         /* Stage id. */
    uint8                   id, idx;

    /* Input parameters check. */
    if (NULL == fg) {
       return (BCM_E_PARAM);
    }

    /* Validate the Preselector ID */
    if (_BCM_FIELD_IS_PRESEL_VALID(presel) == FALSE) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "ERROR: Invalid Presel ID[%d].\n\r"), presel));
       return BCM_E_PARAM;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Check if presel ID is already created */
    if (!BCM_FIELD_PRESEL_TEST(fc->presel_info->presel_set, presel)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
       "ERROR: Preselector ID[%d] is not created.\n\r"), presel));
       return BCM_E_NOT_FOUND;
    }

    /* Verify if the Presel ID is already in USE */
    if (BCM_FIELD_PRESEL_TEST(fc->presel_info->operational_set, presel)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
       "ERROR: Preselector ID[%d] is already in USE"
       " by some other group.\n\r"), presel));
       return BCM_E_BUSY;
    }

    /* Find preselector descriptor */
    BCM_IF_ERROR_RETURN
      (_bcm_field_presel_entry_get(unit, presel, &f_presel));

    /* Retrieve the stage id. */
    if (BCM_FIELD_QSET_TEST(f_presel->p_qset, bcmFieldQualifyStageIngress)) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressExactMatch)) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                                    bcmFieldQualifyStageIngressFlowtracker)) {
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
#endif
    } else {
        return BCM_E_INTERNAL;
    }

    /* Validate the Group's stage_id with Presel Entry stage_id */
    if (stage_id != fg->stage_id) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
         "ERROR: Presel Entry[%d] doesn't belongs to Field Group's stage.\n\r"),
          presel));
       return BCM_E_PARAM;
    }

    /* Verify the last entry in the array to check the free slot */
    if (fg->presel_ent_arr[_FP_PRESEL_ENTRIES_MAX_PER_GROUP-1] != NULL) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit, "ERROR: Exceeded Max "
       "Preselector IDs[%d] for the group[%d].\n\r"),
        _FP_PRESEL_ENTRIES_MAX_PER_GROUP, fg->gid));
       return BCM_E_FULL;
    }

    /* Adjust the presel entry as per the priority. */
    for (id = 0; id < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; id++) {
       if (fg->presel_ent_arr[id] == NULL) {
          fg->presel_ent_arr[id] = f_presel;
          break;
       } else {
          if (fg->presel_ent_arr[id]->priority < f_presel->priority) {
             /* Move down all the entries */
             for (idx = _FP_PRESEL_ENTRIES_MAX_PER_GROUP-1; idx > id; idx--) {
                fg->presel_ent_arr[idx] = fg->presel_ent_arr[idx - 1];
             }
             fg->presel_ent_arr[id] = f_presel;
             break;
          }
       }
    }

    /* Debug prints to be removed */
    for (id = 0; id < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; id++) {
       if (fg->presel_ent_arr[id]) {
          LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "arr[%d]:priority[%d]\n\r"),
                    id, fg->presel_ent_arr[id]->priority));
       }
    }

    return BCM_E_NONE;
}

STATIC int
_field_presel_entry_part_update(int unit, _field_stage_t *stage_fc,
                                _field_presel_entry_t *f_presel)
{
    int                     rv;                /* Operation return status. */
    _field_group_t          *fg;
    _field_lt_slice_t       *lt_fs;
    _field_presel_entry_t   *f_presel_p;
    int                      part, parts_count;
    int                      slice_num, pri_tcam_idx,
                             part_index;

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == f_presel) {
       return (BCM_E_PARAM);
    }

    fg = f_presel->group;
    lt_fs = f_presel->lt_fs;
    if (NULL == fg || NULL == lt_fs) {
       return BCM_E_INTERNAL;
    }

    /* Get entry TCAM index given slice number and entry offset in slice. */
    rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
                                             fg->instance, lt_fs->slice_number,
                                             f_presel->hw_index, &pri_tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Get number of entry parts based on group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
                                            fg->stage_id, fg->flags,
                                            &parts_count));

    f_presel_p = f_presel;
    for (part = 0; (part < parts_count && f_presel_p != NULL); part++) {
       /* Given primary entry TCAM index calculate entry part tcam index. */
       if (0 != part) {
           rv = _bcm_field_th_lt_entry_part_tcam_idx_get(unit, fg,
                                                         pri_tcam_idx,
                                                         part, &part_index);
           if (BCM_FAILURE(rv)) {
              return BCM_E_INTERNAL;
           }

          /*
           * Given entry part TCAM index, determine the slice number and slice
           * offset index.
           */
          rv = _bcm_field_th_lt_tcam_idx_to_slice_offset(unit, stage_fc,
                                   fg->instance, part_index, &slice_num,
                                   (int *)&f_presel_p->hw_index);
          if (BCM_FAILURE(rv)) {
             return BCM_E_INTERNAL;
          }
       }
       /* Assign entry to a slice. */
       f_presel_p->lt_fs->p_entries[f_presel_p->hw_index] = f_presel_p;
       f_presel_p = f_presel_p->next;
    }

    return BCM_E_NONE;
}

/*
 * Function: _field_presel_entry_prio_set
 *
 * Purpose:
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_presel - (IN) Reference to Field preselector entry structure.
 *     prio  - (IN) Presel Entry Priority value.
 *
 * Returns:
 *     BCM_E_XX
 *
 * Notes:
 *     The earlier checks guarantee that there is a free slot somewhere
 *     i.e. in one of the alloted slices for the group.
 */
STATIC int
_field_presel_entry_prio_set(int unit,
                             _field_presel_entry_t *f_presel,
                             int prio)
{
    int                     rv;                /* Operation return status. */
    _field_group_t          *fg;               /* Field Group structure. */
    _field_control_t        *fc;               /* Field control structure. */
    _field_stage_t          *stage_fc;
    _field_presel_entry_t   *f_presel_tmp;
    _field_lt_slice_t       *lt_fs;
    uint8                   move_down = 0;
    uint8                   move_required = 0;
    uint8                   skip = 0;
    int                     idx, curr_idx, dest_idx = -1, move_idx, pre_idx;
    int                     next_hw_index, dest_hw_index = -1;
    int                     target_hw_index = -1;
    int                     valid_entries = 0;
    int                     no_move_down_match = 0;
    _field_presel_entry_t   *presel_entries[_FP_PRESEL_ENTRIES_MAX_PER_GROUP+1];

    /* Input parameters check. */
    if (NULL == f_presel) {
       return (BCM_E_PARAM);
    }

    /* If the priority is same, return */
    if (f_presel->priority == prio) {
       return BCM_E_NONE;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (NULL == f_presel->group) {
       return BCM_E_INTERNAL;
    }

    fg = f_presel->group;
    if (NULL ==  fg->presel_ent_arr[0]) {
       return BCM_E_INTERNAL;
    }

    /* Retrieve the slice the entry belongs to. */
    lt_fs = f_presel->lt_fs;
    if (lt_fs == NULL) {
       return BCM_E_INTERNAL;
    }

    if(fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        /* Currently, not supporting priority movement in this stage */
        return BCM_E_NONE;
    }

    /* Check if the slice supports SW Atomicity */
    if ((lt_fs->slice_flags & _BCM_FIELD_SLICE_SW_ATOMICITY_SUPPORT) == 0) {
       return BCM_E_CONFIG;
    }

    /* Update LT Slice's last free entry index. */
    target_hw_index = lt_fs->entry_count - 1;
    /* Verify if the last entry in the slice is free */
    if (lt_fs->p_entries[target_hw_index] != NULL) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: No free entries left to swap.\n\r")));
       return BCM_E_INTERNAL;
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                                                 &stage_fc));

    /* Count Valid entries in the group preselector array. */
    LOG_VERBOSE(BSL_LS_BCM_FP,
              (BSL_META_U(unit, "Before Prio Set ..\n\r")));
    valid_entries = 0;
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
       if (fg->presel_ent_arr[idx]) {
          LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "arr[%d]:presel_id[%d]:"
                     "hw_index[%d]:priority[%d]"),
                    idx, fg->presel_ent_arr[idx]->presel_id,
                    fg->presel_ent_arr[idx]->hw_index,
                    fg->presel_ent_arr[idx]->priority));
          if (f_presel == fg->presel_ent_arr[idx]) {
             LOG_VERBOSE(BSL_LS_BCM_FP,
                   (BSL_META_U(unit, ":next_priority:[%d]\n\r"), prio));
          } else {
             LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "\n\r")));
          }
          valid_entries++;
       }
    }

    /* Validate if the given priority entry has to be move up or down. */
    if (prio > f_presel->priority) {
       /* the entry should move upward */
       move_down = 0;
       curr_idx = -1;
    } else {
       /* the entry should move downward */
       move_down = 1;
       curr_idx = valid_entries;
    }


    no_move_down_match = 0;
    /* Get the Destination hw index */
    for (idx = 0; idx < valid_entries; idx++) {
       f_presel_tmp = fg->presel_ent_arr[idx];
       if (f_presel_tmp != NULL) {
          if ((move_down == 1) && (prio < f_presel_tmp->priority)) {
             no_move_down_match++;
          }
          if (f_presel_tmp == f_presel) {
             curr_idx = idx;
             continue;
          }

          if (((move_down == 1) && (curr_idx < idx) &&
               (prio >= f_presel_tmp->priority)) ||
              ((move_down == 0) && (curr_idx == -1) &&
               (prio > f_presel_tmp->priority) &&
               (f_presel->priority <= f_presel_tmp->priority))) {

             if ((move_down == 1) && (curr_idx + 1) == idx) {
                /* Down move is not required as the immediate next entry
                 * priority is still less than or equal to the new priority. */
                break;
             }

             /* movement required. */
             move_required = 1;
             dest_hw_index = f_presel_tmp->hw_index;
             dest_idx = idx;
             break;
          }
       }
    }

    /*
     * This case occurs when the priority of the entry to be set
     * is the least priority among all the existing entries.
     */
    if (no_move_down_match == valid_entries) {
       move_required = 1;
       dest_idx = valid_entries;
    }

    if (move_required == 0) {
       LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "== Move Not Required ==\n\r")));
       return BCM_E_NONE;
    }

    /* initialize the local preselector array */
    sal_memset(&presel_entries[0], 0x0,
        sizeof(_field_presel_entry_t *) * (_FP_PRESEL_ENTRIES_MAX_PER_GROUP+1));

    /*
     * Iteration Move down the low priority entries and adjust the
     * given presel entry as per its new priority.
     */
    for (idx = valid_entries-1; idx >= 0; idx--) {
       f_presel_tmp = fg->presel_ent_arr[idx];
       move_idx = 0;
       if (f_presel_tmp != NULL) {
          if (((move_down == 1) && ((prio >= f_presel_tmp->priority) ||
                (f_presel_tmp == f_presel))) ||
              ((move_down == 0) && ((prio > f_presel_tmp->priority) &&
                (f_presel_tmp->hw_index >= dest_hw_index)))) {

             next_hw_index = f_presel_tmp->hw_index;
             f_presel_tmp->hw_index = target_hw_index;

             rv = _field_presel_entry_part_update(unit, stage_fc, f_presel_tmp);
             if (BCM_FAILURE(rv)) {
                return BCM_E_INTERNAL;
             }

             rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel_tmp);
             if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "ERROR: Unable to install Presel Entry hw_index:%d\n\r"),
                     f_presel_tmp->hw_index));
                return BCM_E_INTERNAL;
             }

             target_hw_index = next_hw_index;
             move_idx = 1;
          }
          if (f_presel_tmp == f_presel) {
             curr_idx = idx + move_idx;
             if (move_down == 1) {
                presel_entries[dest_idx] = f_presel_tmp;
                continue;
             }
          }
          presel_entries[idx + move_idx] = f_presel_tmp;
       }
    }

    if (target_hw_index == -1) {
       return BCM_E_INTERNAL;
    }

    pre_idx = 0;
    skip = 1;
    /*
     * Iteration to bring up the entries to the destination indices.
     */
    for (idx = 0; idx <= valid_entries; idx++) {
          f_presel_tmp = presel_entries[idx];

          if (move_down == 0) {
             /* Look for a free entry in the presel_entries */
             if (f_presel_tmp == NULL) {
                if (dest_idx != idx) {
                   continue;
                }
                if (curr_idx < 0) {
                   return BCM_E_INTERNAL;
                }
                f_presel_tmp = presel_entries[curr_idx];
             } else if (idx < curr_idx) {
                fg->presel_ent_arr[idx] = f_presel_tmp;
                continue;
             } else if (idx == curr_idx) {
                continue;
             }
          } else {
             if (f_presel_tmp == NULL) {
                pre_idx = 1;
                skip = 0;
                continue;
             } else if (skip == 1) { /* Skip till free entry */
                continue;
             }
          }

          next_hw_index = f_presel_tmp->hw_index;
          f_presel_tmp->hw_index = target_hw_index;
          rv = _field_presel_entry_part_update(unit, stage_fc, f_presel_tmp);
          if (BCM_FAILURE(rv)) {
             return BCM_E_INTERNAL;
          }

          rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel_tmp);
          if (BCM_FAILURE(rv)) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "ERROR: Unable to install Presel Entry hw_index:%d\n\r"),
                  f_presel_tmp->hw_index));
             return BCM_E_INTERNAL;
          }

          target_hw_index = next_hw_index;
          fg->presel_ent_arr[idx - pre_idx] = f_presel_tmp;
    }

    /* Delete Slice's last entry. */
    f_presel_tmp = presel_entries[valid_entries];
    if (f_presel_tmp != NULL) {
          int                      parts_count, part, tcam_idx;
          int                      last_hw_index;
          soc_mem_t                lt_tcam_mem;
          _field_presel_entry_t   *f_presel_p;
          uint32                   entry[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                                      /* Entry buffer.   */

          last_hw_index = lt_fs->entry_count - 1;

          /* Get number of entry parts. */
          BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
                                                fg->flags, &parts_count));

          /* Get TCAM view to be used for programming the hardware. */
          if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
              rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                              f_presel->group->instance,
                                             _BCM_FIELD_MEM_TYPE_IFP_LT,
                                _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                                   &lt_tcam_mem, NULL));
          } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
              rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                              f_presel->group->instance,
                                              _BCM_FIELD_MEM_TYPE_EM_LT,
                                _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                                   &lt_tcam_mem, NULL));
          } else {
              return BCM_E_INTERNAL;
          }
          BCM_IF_ERROR_RETURN(rv);

          f_presel_p = f_presel_tmp;
          for (part = parts_count -1; part >= 0; part--) {
             if (f_presel_p == NULL) {
                break;
             }
             /* Calculate the TCAM index. */
             tcam_idx = f_presel_p->lt_fs->start_tcam_idx + last_hw_index;

             /* Clear the entry in hardware. */
             BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                                               tcam_idx, entry));

             /* Update the entry to a slice. */
             f_presel_p->lt_fs->p_entries[last_hw_index] = NULL;
             f_presel_p = f_presel_p->next;
          }
    }

    f_presel->priority = prio;
    
    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit, "After Prio Set...\n\r")));
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
       if (fg->presel_ent_arr[idx]) {
          LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "arr[%d]:presel_id[%d]:"
                    "hw_index[%d]:priority[%d]\n\r"),
                    idx, fg->presel_ent_arr[idx]->presel_id,
                    fg->presel_ent_arr[idx]->hw_index,
                    fg->presel_ent_arr[idx]->priority));
       }
    }
    return BCM_E_NONE;
}

/*
 * Function:_field_presel_delete_last_entry_hw
 * Purpose:To delete the last presel entry in the slice
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_presel - (IN) Reference to Field preselector entry structure.
 *
 * Returns:
 *     BCM_E_XX
 */
    STATIC int
_field_presel_delete_last_entry_hw(int unit,
    _field_presel_entry_t *f_presel)
{

    int                     rv;                /* Operation return status. */
    _field_group_t          *fg;               /* Field Group structure. */
    _field_stage_t          *stage_fc;
    _field_presel_entry_t   *f_presel_tmp = NULL;
    _field_lt_slice_t       *lt_fs;
    int                     idx;
    int                     last_hw_index;
    /* Input parameters check. */
    if (NULL == f_presel) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_presel->group) {
        return BCM_E_INTERNAL;
    }

    fg = f_presel->group;
    if (NULL ==  fg->presel_ent_arr[0]) {
        return BCM_E_INTERNAL;
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                &stage_fc));
   /*Find the last entry */
    for (idx = _FP_PRESEL_ENTRIES_MAX_PER_GROUP - 1; idx >=0; idx--) {
        if (fg->presel_ent_arr[idx]) {
            f_presel_tmp = fg->presel_ent_arr[idx];
            break;
        }
    }
    lt_fs = f_presel->lt_fs;
    if (lt_fs == NULL) {
        return BCM_E_INTERNAL;
    }



    if (f_presel_tmp != NULL) {
        int                      parts_count, part, tcam_idx;
        soc_mem_t                lt_tcam_mem;
        uint32                   entry[SOC_MAX_MEM_FIELD_WORDS] = {0};
        /* Entry buffer.   */

        last_hw_index = lt_fs->entry_count - 1;

        /* Get number of entry parts. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
                    fg->stage_id, fg->flags, &parts_count));

        /* Get TCAM view to be used for programming the hardware. */
        if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                        f_presel->group->instance,
                        _BCM_FIELD_MEM_TYPE_IFP_LT,
                        _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                        &lt_tcam_mem, NULL));
        } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
            rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                        f_presel->group->instance,
                        _BCM_FIELD_MEM_TYPE_EM_LT,
                        _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                        &lt_tcam_mem, NULL));
#if defined(BCM_FLOWTRACKER_SUPPORT)
        } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
            /* Combo view is not available */
            return _field_hx5_ft_presel_delete_last_entry_hw(unit,
                        f_presel_tmp, last_hw_index);
#endif
        } else {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(rv);

        for (part = parts_count -1; part >= 0; part--) {
            if (f_presel_tmp == NULL) {
                break;
            }
            /* Calculate the TCAM index. */
            tcam_idx = f_presel_tmp->lt_fs->start_tcam_idx + last_hw_index;
            /* Clear the entry in hardware. */
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                        tcam_idx, entry));
            /* Update the entry to a slice. */
            f_presel_tmp->lt_fs->p_entries[last_hw_index] = NULL;
            f_presel_tmp= f_presel_tmp->next;
        }
    }
    return BCM_E_NONE;
}


/*
 * Function: _field_presel_entries_move_down_hw
 * Purpose:To move the entries down in the hardware
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_presel - (IN) Reference to Field preselector entry structure.
 *
 * Returns:
 *     BCM_E_XX
 */
    STATIC int
_field_presel_entries_move_down_hw (int unit,
        _field_presel_entry_t *f_presel)
{
    int target_hw_index = -1, next_hw_index =-1;
    int                     rv;                /* Operation return status. */
    int idx;
    _field_stage_t          *stage_fc;
    _field_group_t          *fg;               /* Field Group structure. */
    _field_lt_slice_t       *lt_fs;
    _field_presel_entry_t *f_presel_tmp;
    lt_fs = f_presel->lt_fs;
    target_hw_index = lt_fs->entry_count - 1; /*Index of the free slice entry*/
    if (NULL == f_presel->group) {
        return BCM_E_INTERNAL;
    }
    fg = f_presel->group;

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                &stage_fc));
    /*
     * Iteration to move down the low priority entries and  the entry being updated.
     */
    for (idx = _FP_PRESEL_ENTRIES_MAX_PER_GROUP - 1; idx >= 0; idx--) {
        f_presel_tmp = fg->presel_ent_arr[idx];
        if (f_presel_tmp != NULL) {
            next_hw_index = f_presel_tmp->hw_index;
            f_presel_tmp->hw_index = target_hw_index;

            rv = _field_presel_entry_part_update(unit, stage_fc, f_presel_tmp);
            if (BCM_FAILURE(rv)) {
                return BCM_E_INTERNAL;
            }

            rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel_tmp);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "ERROR: Unable to install Presel Entry hw_index:%d\n\r"),
                            f_presel_tmp->hw_index));
                return BCM_E_INTERNAL;
            }
            if(f_presel_tmp == f_presel) {
                break;
            }
            target_hw_index = next_hw_index;
        }
    }

    return BCM_E_NONE;
}
/*
 * Function: _field_presel_entries_move_up_hw
 * Purpose:To move the entries up in the hardware
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     entry_index - (IN) presel_ent_arr index, entries after this will
 *     updated with next below entry HW index
 *     start_entry_hw_index - (IN) HW index to be updated for the first entry
 *     below the intry with index entry_index
 *     f_presel - (IN) Reference to Field preselector entry structure.
 *
 * Returns:
 *     BCM_E_XX
 */

    STATIC int
_field_presel_entries_move_up_hw (int unit,
        int entry_index,
        int start_entry_hw_index,
        _field_presel_entry_t *f_presel)
{
    int                     rv;                /* Operation return status. */
    _field_group_t          *fg;               /* Field Group structure. */
    _field_presel_entry_t *f_presel_tmp = NULL;
    int target_hw_index = -1, next_hw_index=-1;
    _field_stage_t          *stage_fc;
    int                     idx;
    if (NULL == f_presel->group) {
        return BCM_E_INTERNAL;
    }

    fg = f_presel->group;
    if (NULL ==  fg->presel_ent_arr[0]) {
        return BCM_E_INTERNAL;
    }
    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                &stage_fc));

    target_hw_index = start_entry_hw_index;

    /*The operation starts from the entry below the entry to be updated*/
    for (idx = entry_index + 1; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
      if ((fg->presel_ent_arr[idx] != NULL)) {
        f_presel_tmp = fg->presel_ent_arr[idx];
        next_hw_index = f_presel_tmp->hw_index;
        f_presel_tmp->hw_index = target_hw_index;

      } else {
        if (f_presel_tmp != NULL) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "No more entry to move up as this is last valid entry:%d\n\r"),
                 f_presel_tmp->hw_index));
        } else {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "No entries available to move up \n\r")));
        }
        return BCM_E_NONE;
      }
      target_hw_index = next_hw_index;

      rv = _field_presel_entry_part_update(unit, stage_fc, f_presel_tmp);
      if (BCM_FAILURE(rv)) {
        return BCM_E_INTERNAL;
      }

      rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel_tmp);
      if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "ERROR: Unable to install Presel Entry hw_index:%d\n\r"),
              f_presel_tmp->hw_index));
        return BCM_E_INTERNAL;
      }

    }
    return BCM_E_NONE;
}

/*
 * Function: _field_presel_entry_update_hw_index
 * Purpose:To re-install the Presel entry with given HW index in the hardware
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     entry_hw_index - (IN) Hardware index of the entry to be updated.
 *     f_presel - (IN) Reference to Field preselector entry structure.
 *
 * Returns:
 *     BCM_E_XX
 */
    STATIC int
_field_presel_entry_update_hw_index(int unit,
        int entry_hw_index,
        _field_presel_entry_t *f_presel)
{
    int                     rv;                /* Operation return status. */
    _field_group_t          *fg;               /* Field Group structure. */
    _field_stage_t          *stage_fc;

    if (NULL == f_presel->group) {
        return BCM_E_INTERNAL;
    }

    fg = f_presel->group;
    /* Get stage field controBCM_E_NONE l structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                &stage_fc));
    /*Update the Presel with new qualifier value in the H/W */
    f_presel->hw_index = entry_hw_index;
    rv = _field_presel_entry_part_update(unit, stage_fc, f_presel);
    if (BCM_FAILURE(rv)) {
        return BCM_E_INTERNAL;
    }

    rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "ERROR: Unable to install Presel Entry hw_index:%d\n\r"),
                    f_presel->hw_index));
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function: _field_presel_entry_update
 * Purpose:To update the Presel entry into hardware when the group is operational.
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     f_presel - (IN) Reference to Field preselector entry structure.
 *
 * Returns:
 *     BCM_E_XX
 *
 * Notes:
 *     The earlier checks guarantee that there is a free slot somewhere
 *     i.e. in one of the alloted slices for the group.
 */
STATIC int
_field_presel_entry_update(int unit,
                     _field_presel_entry_t *f_presel)
{
    int                     rv;                /* Operation return status. */
    _field_group_t          *fg;               /* Field Group structure. */
    _field_control_t        *fc;               /* Field control structure. */
    _field_stage_t          *stage_fc;
    _field_lt_slice_t       *lt_fs;
    int                     entry_hw_index, copy_entry_hw_index;
    int                     free_hw_index = -1;
    int                     idx, entry_idx = -1;

    /* Input parameters check. */
    if (NULL == f_presel) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (NULL == f_presel->group) {
        return BCM_E_INTERNAL;
    }

    fg = f_presel->group;
    if (NULL ==  fg->presel_ent_arr[0]) {
        return BCM_E_INTERNAL;
    }

    /* Retrieve the slice the entry belongs to. */
    lt_fs = f_presel->lt_fs;
    if (lt_fs == NULL) {
        return BCM_E_INTERNAL;
    }

    if (lt_fs->slice_flags & _BCM_FIELD_SLICE_HW_ATOMICITY_SUPPORT) {
        entry_hw_index = f_presel->hw_index;
        rv = _field_presel_entry_update_hw_index(unit, entry_hw_index, f_presel);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "ERROR: updating the entry with index:%d\n\r"),
                        entry_hw_index));
            return BCM_E_INTERNAL;
        }
        return rv;
    }

    /* Check if the slice supports SW Atomicity */
    if ((lt_fs->slice_flags & _BCM_FIELD_SLICE_SW_ATOMICITY_SUPPORT) == 0) {
        return BCM_E_CONFIG;
    }
    /*Hardware index of the entry to be updated*/
    entry_hw_index = f_presel->hw_index;

    /* Update LT Slice's last free entry index. */
    free_hw_index = lt_fs->entry_count - 1;
    /* Verify if the last entry in the slice is free */
    if (lt_fs->p_entries[free_hw_index] != NULL) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "ERROR: No free entries left to swap.\n\r")));
        return BCM_E_INTERNAL;
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                &stage_fc));

    /*Find the index of the entry being updated */
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
        if (fg->presel_ent_arr[idx]) {
            LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "arr[%d]:presel_id[%d]:"
                                "hw_index[%d]:priority[%d]"),
                     idx, fg->presel_ent_arr[idx]->presel_id,
                     fg->presel_ent_arr[idx]->hw_index,
                     fg->presel_ent_arr[idx]->priority));
            if (fg->presel_ent_arr[idx]->hw_index == f_presel->hw_index) {
                entry_idx = idx;
                break;
            }
        }
    }

    /* Move down the low priority entries and along with the entry being changed.
    */
    rv =_field_presel_entries_move_down_hw(unit, f_presel);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "ERROR: shifting the entries down starting from index:%d\n\r"),
                    entry_hw_index));
        return BCM_E_INTERNAL;
    }

    copy_entry_hw_index = f_presel->hw_index;

    /*Update the Presel with new qualifier value in the H/W */
    rv = _field_presel_entry_update_hw_index(unit, entry_hw_index, f_presel);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "ERROR: updating the entry with index:%d\n\r"),
                    entry_hw_index));
        return BCM_E_INTERNAL;
    }

    /* Move up the low priority entries.
    */
    rv =_field_presel_entries_move_up_hw(unit, entry_idx, copy_entry_hw_index, f_presel);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "ERROR: shifting the entries up below index:%d\n\r"),
                    entry_hw_index));
        return BCM_E_INTERNAL;
    }
   /*Delete the last entry to keep a free entry*/
    rv =_field_presel_delete_last_entry_hw(unit, f_presel);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "ERROR: Deleting the last entry with index:%d\n\r"),
                    free_hw_index));
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_presel_entry_prio_set
 *
 * Purpose:
 *
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field presel entry to operate on.
 *     prio  - (IN) Presel Entry Priority value.
 *
 * Returns:
 *     BCM_E_XX
 *
 * Notes:
 *     The earlier checks guarantee that there is a free slot somewhere
 *     i.e. in one of the alloted slices for the group.
 */
int
_bcm_field_presel_entry_prio_set(int unit, bcm_field_entry_t entry, int prio)
{
    int                     rv;                /* Operation return status. */
    _field_control_t        *fc;               /* Field control structure. */
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    bcm_field_presel_t      presel;            /* Field Preselector ID.    */

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Verify if the Presel ID is already in USE */
    if (BCM_FIELD_PRESEL_TEST(fc->presel_info->operational_set, presel) &&
        !(f_presel->flags & _FP_ENTRY_DIRTY)) {
       BCM_IF_ERROR_RETURN (_field_presel_entry_prio_set(unit, f_presel, prio));
    }

    /* Assign the priority */
    f_presel->priority = prio;

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_presel_entry_prio_get
 *
 * Purpose: Retreive the preselector entry priority value.
 *
 * Parameters:
 *     unit  - (IN)  BCM device number
 *     entry - (IN)  Field presel entry to operate on.
 *     prio  - (OUT) Presel Entry Priority value.
 *
 * Returns:
 *     BCM_E_XX
 */
int
_bcm_field_presel_entry_prio_get(int unit, bcm_field_entry_t entry, int *prio)
{
    int                     rv;                /* Operation return status. */
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    bcm_field_presel_t      presel;            /* Field Preselector ID.    */

    /* Validate input parameters. */
    if (prio == NULL) {
       return BCM_E_PARAM;
    }

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }

    *prio = f_presel->priority;
    return BCM_E_NONE;
}


/*
 * Function:
 *    _field_th_lt_entry_data_config_set
 * Purpose:
 *    Configure LT Entry data parameters generated by group (keygen profile ..)
 *    and actions configured by the presel.
 * Parameters:
 *     unit     - (IN)      BCM device number.
 *     stage_fc - (IN)      Stage field control structure.
 *     f_presel - (IN/OUT)  Preselector Entry structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_th_lt_entry_data_config_set(int unit, _field_stage_t *stage_fc,
                                   _field_presel_entry_t *f_presel)
{
    int                   idx;      /* Entry parts iterator.        */
    int                   parts;    /* Number of field entry parts. */
    _field_group_t        *fg;      /* Field group structure.       */
    soc_mem_t             lt_data_mem;
    _field_presel_entry_t *f_presel_part;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == f_presel)) {
        return (BCM_E_PARAM);
    }

    /* Get entry's group structure. */
    fg = f_presel->group;

    /* Get number of lt entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
                                    stage_fc->stage_id, fg->flags, &parts));

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        BCM_IF_ERROR_RETURN (_bcm_field_th_lt_tcam_policy_mem_get(
                                        unit, stage_fc, fg->instance,
                                          _BCM_FIELD_MEM_TYPE_IFP_LT,
                                       _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                                                NULL, &lt_data_mem));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        BCM_IF_ERROR_RETURN (_bcm_field_th_lt_tcam_policy_mem_get(
                                        unit, stage_fc, fg->instance,
                                           _BCM_FIELD_MEM_TYPE_EM_LT,
                                       _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                                                NULL, &lt_data_mem));
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        int index = 0;
        /* If lt slice is not NULL, use that to get correct memories */
        if (f_presel->lt_fs) {
            if (f_presel->lt_fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
                index = f_presel->lt_fs->slice_number;
            }
        }
        BCM_IF_ERROR_RETURN (_bcm_field_th_lt_tcam_policy_mem_get(
                                        unit, stage_fc, fg->instance,
                                       _BCM_FIELD_MEM_TYPE_FT_LT_I(index),
                                       _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                                                NULL, &lt_data_mem));
#endif
    } else {
        return (BCM_E_PARAM);
    }

    f_presel_part = f_presel;
    /* Install entry parts in hardware. */
    for (idx = 0; (idx < parts && f_presel_part != NULL); idx++) {
        /* Install keygen program profile table entries for the group in HW */
        BCM_IF_ERROR_RETURN (_bcm_field_th_group_keygen_profiles_index_alloc(
                                                              unit, stage_fc,
                                                              fg, idx));

        /* Allocate the Preselector Data Memory. */
        BCM_IF_ERROR_RETURN (_bcm_field_th_presel_mem_data_get(
                                               unit, stage_fc, fg->instance,
                                               f_presel_part,
                                               &f_presel_part->lt_data));

        /* Build LT selection Data entry. */
        BCM_IF_ERROR_RETURN (_bcm_field_th_lt_entry_data_value_set(
                                               unit, stage_fc, fg, idx,
                                               lt_data_mem,
                                               f_presel_part->lt_data.data));

        /* Assign the Preselector Entry Actions to the LT Data entry. */
        BCM_IF_ERROR_RETURN (_bcm_field_presel_entry_actions_set(
                                                unit, lt_data_mem,
                                                f_presel_part,
                                                f_presel_part->lt_data.data));

        f_presel_part = f_presel_part->next;
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *    _field_th_lt_entry_phys_create
 * Purpose:
 *    Create a physical entry in a logical table slice.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN) Stage field control structure.
 *    lt_ent    - (IN) Logical table entry identifier.
 *    prio      - (IN) Logical table entry priority.
 *    lt_fs     - (IN) Logical table slice structure.
 *    fg        - (IN) Field group structure.
 *    f_presel  - (OUT) Allocated & initialized entry.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_phys_create(int unit, _field_stage_t *stage_fc,
                               bcm_field_presel_t lt_ent, int prio,
                               _field_lt_slice_t *lt_fs, _field_group_t *fg,
                               _field_presel_entry_t *f_presel)
{
    _field_control_t *fc;   /* Field control structure.              */
    int idx;                /* Slice entries iteration index.        */
    int parts_count = -1;   /* Number of entry parts.                */
    int part_index;         /* Entry parts iterator.                 */
    int rv;                 /* Operation return status.              */
    int pri_tcam_idx = -1;  /* Primary entry TCAM index.             */
    int slice_num = -1;     /* Slice number.                         */
    _field_presel_entry_t *f_presel_p = NULL; /* LT field entry structure. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_fs) ||
        (NULL == fg) || (NULL == f_presel)) {
        return (BCM_E_PARAM);
    }

    /* Check if free entries are available in the slice. */
    if (0 == lt_fs->free_count) {
        return (BCM_E_RESOURCE);
    }

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get number of entry partss. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
                                stage_fc->stage_id, fg->flags, &parts_count));

    /* Get index for primary entry. */
    for (idx = 0; idx < lt_fs->entry_count; idx++) {
        if (NULL == lt_fs->p_entries[idx]) {
            f_presel->hw_index = idx;
            break;
        }
    }

    if (idx == lt_fs->entry_count) {
       /* If this condition occurs, that means, free_count is not decremented
        * properly, it would have caught above for (0 == lt_fs->free_count) */
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "Error: Allocation failure for presel[%d], Entries reached max"
           "entries in the slice, free_count is not decremented properly.\n\r"),
           f_presel->presel_id));
       return BCM_E_INTERNAL;
    }

    /* Get entry TCAM index given slice number and entry offset in slice. */
    rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
                                  fg->instance, lt_fs->slice_number,
                                  f_presel->hw_index, &pri_tcam_idx);
    if (BCM_FAILURE(rv)) {
       return rv;
    }

    if (parts_count > 1) {
       _field_presel_entry_t *p_ent_alloc;

       f_presel_p = f_presel;
       /* Allocate memory for presel entries for double/triple wide modes. */
       for (idx = 1; idx < parts_count; idx++) {
          p_ent_alloc = NULL;
          _FP_XGS3_ALLOC(p_ent_alloc, sizeof(_field_presel_entry_t),
                      "Presel Entry Alloc.");
          if (NULL == p_ent_alloc) {
              LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: allocation failure for Presel[%d] Parts entry[%d]\n"),
                 f_presel->presel_id, idx));
              return (BCM_E_MEMORY);
          }
          /* Copy the base presel entry contents to its allocated part entry */
          sal_memcpy(p_ent_alloc, f_presel_p, sizeof(_field_presel_entry_t));
          f_presel_p->next = p_ent_alloc;
          f_presel_p = p_ent_alloc;
       }
    }

    f_presel_p = f_presel;
    /* Fill entry primitives. */
    for (idx = 0; (idx < parts_count && f_presel_p != NULL); idx++) {
        /* Get entry part flags. */
        rv = _bcm_field_th_tcam_part_to_entry_flags(unit, idx, fg->flags,
                                                    &(f_presel_p->flags));
        if (BCM_FAILURE(rv)) {
            goto bcm_error;
        }

        /* Given primary entry TCAM index calculate entry part tcam index. */
        if (0 != idx) {
            rv = _bcm_field_th_lt_entry_part_tcam_idx_get(unit, fg,
                                                          pri_tcam_idx,
                                                          idx, &part_index);
            if (BCM_FAILURE(rv)) {
                goto bcm_error;
            }

            /*
             * Given entry part TCAM index, determine the slice number and slice
             * offset index.
             */
            rv = _bcm_field_th_lt_tcam_idx_to_slice_offset(unit, stage_fc,
                    fg->instance, part_index, &slice_num,
                    (int *)&f_presel_p->hw_index);
            if (BCM_FAILURE(rv)) {
                goto bcm_error;
            }
            f_presel_p->lt_fs = stage_fc->lt_slices[fg->instance] + slice_num;
        } else {
            /* Set entry slice. */
            f_presel_p->lt_fs = lt_fs;
        }

        /* Assign the group it belongs to. */
        f_presel_p->group = fg;

        /* Decrement slice free entry count for primary entries. */
        f_presel_p->lt_fs->free_count--;

        /* Assign entry to a slice. */
        f_presel_p->lt_fs->p_entries[f_presel_p->hw_index] = f_presel_p;

        /* Mark entry dirty. */
        f_presel_p->flags |= _FP_ENTRY_DIRTY;

        /* Next Entry Part */
        f_presel_p = f_presel_p->next;
    }

    /* Add the Preselector from field control operational status. */
    BCM_FIELD_PRESEL_ADD(fc->presel_info->operational_set, f_presel->presel_id);

    return (rv);

bcm_error:
    if (BCM_FAILURE(rv)) {
       _field_presel_entry_t *p_ent_free = NULL;

       f_presel_p = f_presel;
       /* Allocate memory for presel entries for double/triple wide modes. */
       for (idx = 1; idx < parts_count; idx++) {
          if (f_presel_p != NULL) {
             p_ent_free = f_presel_p;
             f_presel_p = f_presel_p->next;
             sal_free(p_ent_free);
          }
       }
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_th_lt_entry_phys_destroy
 * Purpose:
 *    Destroy a physical entry from a logical table slice.
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    f_presel - (IN) Preselector Logical table entry structure.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_phys_destroy(int unit, _field_presel_entry_t *f_presel)
{
    int                    idx;      /* Entry iterator index.           */
    int                    parts;    /* Entry parts count.              */
    uint8                  slice_number; /* Slice number.               */
    _field_control_t       *fc;      /* Device field control structure. */
    _field_group_t         *fg;      /* Field group structure.          */
    _field_presel_entry_t  *f_presel_p;    /* Presel Part Entry.        */
    _field_presel_entry_t  *f_presel_free; /* Presel entry to be freed. */

    /* Input parameters check. */
    if ((NULL == f_presel) || (NULL == f_presel->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get the group to which the entry belongs. */
    fg = f_presel->group;

    /* Get entry parts count. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
                                fg->stage_id, fg->flags, &parts));

    f_presel_p = f_presel;
    for (idx = 0; (idx < parts && f_presel_p != NULL); idx++) {
        /* Free entry Data mask buffer if valid. */
        if (NULL != f_presel_p->lt_data.data) {
            sal_free(f_presel_p->lt_data.data);
            f_presel_p->lt_data.data = NULL;
        }

        /* Get entry part slice number. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(idx,
            fg->flags, &slice_number));

        /* Remove entry pointer from the slice. */
        f_presel_p->lt_fs->p_entries[f_presel_p->hw_index] = NULL;

        /* Increment slice unused entry count. */
        f_presel_p->lt_fs->free_count++;

        f_presel_free = f_presel_p;
        f_presel_p = f_presel_p->next;

        /* De-allocate the part entries except Primary entry */
        if (f_presel_free != f_presel) {
           sal_free(f_presel_free);
           f_presel_free = NULL;
        }
    }
    f_presel->next = NULL;

    /* Remove the Preselector from field control operational status. */
    BCM_FIELD_PRESEL_REMOVE(fc->presel_info->operational_set,
                            f_presel->presel_id);

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_lt_entry_config_set
 * Purpose:
 *    Associate presel entry to the slice/s reserved in logical table selection
 *    TCAM for the field group.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     stage_fc     - (IN)  Stage field control structure.
 *     fg           - (IN)  Field group structure.
 *     lt_fs        - (IN)  Field logical table slice structure.
 *     f_presel     - (IN)  Reference to Field Preselector Entry strcture.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_config_set(int unit,
                                  _field_stage_t *stage_fc,
                                  _field_group_t *fg,
                                  _field_lt_slice_t *lt_fs,
                                  _field_presel_entry_t *f_presel)
{
    _field_control_t      *fc;       /* Field control structure.    */
    int                   rv;        /* Operation return status.    */

    /* Input parameter check. */
    if ((NULL == stage_fc) || (NULL == fg) ||
        (NULL == lt_fs) || (NULL == f_presel)) {
       return (BCM_E_PARAM);
    }

    /* Check if Presel Flags are appropriate */
    if (!(fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT)) {
       return (BCM_E_INTERNAL);
    }

    /* Get device field control handle. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /*
     * Create Presel Entries in the LT slice associated with the group
     */
    rv = _field_th_lt_entry_phys_create
                  (unit, stage_fc, f_presel->presel_id,
                   BCM_FIELD_ENTRY_PRIO_LOWEST, lt_fs, fg, f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
         "Error: Presel Slice Entry Creation failed for Id[%d]\n\r"),
                 f_presel->presel_id));
       return (rv);
    }

    /* Set up LT selection rule based on group's keygen codes. */
    rv = _field_th_lt_entry_data_config_set(unit, stage_fc, f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
         "Error: Presel Entry Rule Configuration failed for Id[%d]\n\r"),
                 f_presel->presel_id));
       /* Destroy resources allocated for the entry. */
       _bcm_field_th_lt_entry_phys_destroy(unit, f_presel);
       return (rv);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_th_lt_entry_hw_install
 * Purpose:
 *     Install an LT TCAM entry part in hardware at the supplied entry index.
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     lt_tcam_mem - (IN) LT TCAM Memory.
 *     tcam_idx    - (IN) TCAM index at which entry must be written in HW.
 *     key         - (IN) TCAM Entry Key to be written.
 *     mask        - (IN) TCAM Entry Mask to be written.
 *     data        - (IN) TCAM Entry Data to be written.
 *     valid       - (IN) Entry Valid (TRUE) / Invalid (FALSE)
 * Returns:
 *     BCM_E_XXX
 *
 * Note: To only enable/disable the entry, invoke this function with NULL key, mask and data.
 */
int
_bcm_field_th_lt_entry_hw_install(int unit, soc_mem_t lt_tcam_mem, int tcam_idx,
                                  uint32 *key, uint32 *mask, uint32 *data,
                                  uint8 valid)
{
    uint32         entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* Entry buffer.   */

    /* Input parameters check. */
    if ((NULL == key) || (NULL == mask) || (NULL == data)) {
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, lt_tcam_mem, MEM_BLOCK_ANY,
                                         tcam_idx, entry));
    } else {
        /* Clear the entry in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                                      tcam_idx, entry));
        /* Read entry into SW buffer. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, lt_tcam_mem, MEM_BLOCK_ANY, tcam_idx,
                                     entry));
        /* Set entry Key, Mask and Data value. */
        soc_mem_field_set(unit, lt_tcam_mem, entry, KEYf,  key);
        soc_mem_field_set(unit, lt_tcam_mem, entry, MASKf, mask);
        soc_mem_field_set(unit, lt_tcam_mem, entry, DATAf, data);
    }

    /* Set entry valid bit status. */
    soc_mem_field32_set(unit, lt_tcam_mem, entry, VALIDf, valid);

    /* Install entry in hardware. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                                      tcam_idx, entry));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_entry_install
 * Purpose:
 *     Install a logical table entry in hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_presel - (IN) Reference to Field Preselector Entry structure.
 *     lt_fs    - (IN) Reference to the Logical table slice structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_install(int unit, _field_presel_entry_t *f_presel,
                               _field_lt_slice_t *lt_fs)
{
    _field_stage_t   *stage_fc;        /* Stage Field Control structure.  */
    int              tcam_idx;         /* Entry TCAM index.               */
    _field_control_t *fc;              /* Field control pointer.          */
    int              rv = BCM_E_NONE;  /* Operation return status.        */
    soc_mem_t        lt_tcam_mem;      /* TCAM memory ID.                 */
    uint8            valid;            /* Entry Valid bit.                */

    /* Input parameters check. */
    if (NULL == f_presel || NULL == lt_fs) {
        return (BCM_E_PARAM);
    }

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit , &fc));

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            f_presel->group->stage_id, &stage_fc));

    /*
     * Get TCAM view to be used for programming the hardware based on the group
     * operational mode.
     */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                        f_presel->group->instance,
                                       _BCM_FIELD_MEM_TYPE_IFP_LT,
                          _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                             &lt_tcam_mem, NULL));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                        f_presel->group->instance,
                                        _BCM_FIELD_MEM_TYPE_EM_LT,
                          _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                             &lt_tcam_mem, NULL));
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if ((soc_feature(unit, soc_feature_field_flowtracker_support)) &&
        (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER)) {
        return _bcm_field_hx5_ft_lt_entry_install(unit, f_presel, lt_fs);
#endif
    } else {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_field_multi_pipe_enhanced) &&
        _BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        valid = (f_presel->group->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 0;
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
    {
        valid = (f_presel->group->flags & _FP_GROUP_LOOKUP_ENABLED) ?
                (soc_feature(unit, soc_feature_td3_style_fp) ? 3 : 1) : 0;
    }
    rv = _bcm_field_presel_entry_tcam_idx_get(unit, f_presel, lt_fs,
                                              &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Install all parts of the entry in hardware. */
    rv = _bcm_field_th_lt_entry_hw_install(unit, lt_tcam_mem, tcam_idx,
                                               f_presel->lt_tcam.key,
                                               f_presel->lt_tcam.mask,
                                               f_presel->lt_data.data,
                                               valid);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    f_presel->flags &= ~_FP_ENTRY_DIRTY;
    f_presel->flags |= _FP_ENTRY_INSTALLED;
    f_presel->flags |= _FP_ENTRY_ENABLED;

    return (rv);
}

/*
 * Function:
 *     _bcm_field_th_lt_entry_parts_install
 * Purpose:
 *     Install a logical table entry and its parts in hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_presel - (IN)  Reference to Field Preselector Entry strcture.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_parts_install(int unit, _field_presel_entry_t *f_presel)
{
    int part, parts_count = -1;
    _field_lt_slice_t      *lt_fs;
    _field_presel_entry_t  *f_presel_p;

    /* Input parameters check. */
    if (NULL == f_presel) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
                                f_presel->group->stage_id,
                                f_presel->group->flags, &parts_count));

    f_presel_p = f_presel;
    for (part = parts_count -1; part >= 0; part--) {
        if (f_presel_p == NULL) {
           return BCM_E_INTERNAL;
        }

        lt_fs = f_presel_p->lt_fs;
        BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_install
                                      (unit, f_presel_p, lt_fs));
        f_presel_p = f_presel_p->next;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_th_group_presel_get
 *
 * Purpose:
 *     Retrieve the set of preselectors associated with the given group.
 *
 * Parameters:
 *     unit      - (IN)  BCM device number.
 *     group     - (IN)  Group Id.
 *     presel    - (OUT) Reference to the set of preselectors.
 *
 * Returns:
 *     BCM_E_PARAM    - if presel_id == NULL or given group Id is not valid.
 *     BCM_E_UNAVAIL  - not supported
 *     BCM_E_NONE     - Success
 */

int
_bcm_field_th_group_presel_get(
    int unit,
    bcm_field_group_t group,
    bcm_field_presel_set_t *presel)
{
    int                   idx;
    _field_group_t        *fg;                 /* Field group pointer.      */
    _field_control_t      *fc;                 /* Field control structure.  */

    /* Input parameters check. */
    if (NULL == presel) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));

    sal_memset(presel, 0x0, sizeof(bcm_field_presel_set_t));
    /* count the number of preselector entries associated to the group */
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
       if (fg->presel_ent_arr[idx] != NULL) {
          BCM_FIELD_PRESEL_ADD(*presel, fg->presel_ent_arr[idx]->presel_id);
       }
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_th_field_presel_set
 *
 * Purpose:
 *     Set the preselectors to the given group.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     group     - (IN) Group Id.
 *     presel    - (IN) Reference to the set of preselectors.
 *
 * Returns:
 *     BCM_E_PARAM     - if presel_id == NULL
 *     BCM_E_UNAVAIL   - not supported
 *     BCM_E_NOT_FOUND - if the presel Ids are not created.
 *     BCM_E_INTERAL   - in-case of internal functionality issues.
 *     BCM_E_NONE      - Success
 */

int
_bcm_field_th_group_presel_set(
    int unit,
    bcm_field_group_t group,
    bcm_field_presel_set_t *presel)
{
    int                    rv = BCM_E_UNAVAIL; /* Operation return status.  */
    _field_group_t         *fg;                /* Field group pointer.      */
    _field_control_t       *fc;                /* Field control structure.  */
    _field_stage_t         *stage_fc;          /* Stage field structure.    */
    int                    idx, ent_ct;
    int                    presel_ent_ct = 0, presel_id_ct = 0;
    int                    prio[_FP_PRESEL_ENTRIES_MAX_PER_GROUP];
    _field_presel_entry_t  *f_presel;
    _field_presel_entry_t  *f_presel_arr[_FP_PRESEL_ENTRIES_MAX_PER_GROUP];
    _field_lt_slice_t      *lt_fs;

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            fg->stage_id, &stage_fc));

    /* Group should support the preselector feature. */
    if (!(fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT)) {
       return BCM_E_UNAVAIL;
    }


    /* Validate all the preselector entries in the set */
    for (idx = 0; idx < BCM_FIELD_PRESEL_SEL_MAX; idx++) {
       if (BCM_FIELD_PRESEL_TEST(*presel, idx)) {
          /* Check if presel ID is already created */
          if (!BCM_FIELD_PRESEL_TEST(fc->presel_info->presel_set, idx)) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "ERROR: Preselector ID[%d] is not created.\n\r"), idx));
             return BCM_E_NOT_FOUND;
          }
          if (BCM_FIELD_PRESEL_TEST(fc->presel_info->operational_set, idx)) {
             LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Preselector ID[%d] is already in USE"
             "Updating the hardware.\n\r"), idx));
             BCM_IF_ERROR_RETURN(_bcm_field_presel_entry_get(unit, idx,
                         &f_presel));
             rv = _field_presel_entry_update(unit, f_presel);
          }
          presel_id_ct++;
       }
    }
    if(BCM_E_UNAVAIL != rv) {
        return rv;
    }
    /* count the number of preselector entries associated to the group */
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
       if (fg->presel_ent_arr[idx] != NULL) {
          presel_ent_ct++;
       }
    }

    /* Check if group supports the presel entries. */
    if ((presel_ent_ct + presel_id_ct) > _FP_PRESEL_ENTRIES_MAX_PER_GROUP) {
       return BCM_E_RESOURCE;
    }

    /* Check if the Slice has free entries */
    lt_fs = fg->presel_ent_arr[0]->lt_fs;
    if (lt_fs->free_count < presel_id_ct) {
       return BCM_E_RESOURCE;
    }

    ent_ct = 0;
    /* Add the preselector entries to the group */
    for (idx = 0; idx < BCM_FIELD_PRESEL_SEL_MAX; idx++) {
       if (BCM_FIELD_PRESEL_TEST(*presel, idx)) {
          BCM_IF_ERROR_RETURN(_bcm_field_presel_entry_get(unit, idx,
                                                          &f_presel));
          f_presel_arr[ent_ct] = f_presel;
          prio[ent_ct] = f_presel->priority;
          f_presel->priority = 0;
          ent_ct++;
       }
    }

    /*
     * Create Presel Entries in the LT slice associated with the group
     */
    for (idx = 0; idx < ent_ct; idx++) {
       f_presel = f_presel_arr[idx];
       /* Assign the preselectors to the group presel array. */
       rv = _bcm_field_presel_group_add(unit, fg, f_presel->presel_id);
       if (BCM_FAILURE(rv)) {
          return rv;
       }

       /* Allocate the presel entries in the LT slice for this group. */
       BCM_IF_ERROR_RETURN
           (_bcm_field_th_lt_entry_config_set(unit, stage_fc, fg,
                                              lt_fs, f_presel));

       /* Install the LT Entry in HW. */
       rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel);
       BCM_IF_ERROR_RETURN(rv);

       rv = _field_presel_entry_prio_set(unit, f_presel, prio[idx]);
       if (BCM_FAILURE(rv)) {
          LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
          "ERROR: Unable to set the priority[%d] for Preselector ID[%d].\n\r"),
          prio[idx], idx));
          return BCM_E_INTERNAL;
       }
    }

    return rv;
}

/*
 * Function:
 *     _bcm_field_th_ingress_lt_action_prio_install
 * Purpose:
 *     Write logical table action priortiy information to hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group Structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_ingress_lt_action_prio_install(int unit,
                                             _field_stage_t *stage_fc,
                                             _field_group_t *fg)
{
    soc_field_t         field;              /* HW entry field.       */
    uint32              value;              /* Entry field value.    */
    int                 lt_idx;             /* LT iterator.          */
    _field_lt_config_t *lt_info;            /* Logical table info.   */
    _field_control_t    *fc;        /* Field control structure.     */
    uint32              entry[SOC_MAX_MEM_FIELD_WORDS];
                                            /* Entry buffer.         */
    static const soc_field_t lt_action_pri[] = {     /* LT action priority
                                                        fields list.    */
        LOGICAL_TABLE_0_ACTION_PRIORITYf,
        LOGICAL_TABLE_1_ACTION_PRIORITYf,
        LOGICAL_TABLE_2_ACTION_PRIORITYf,
        LOGICAL_TABLE_3_ACTION_PRIORITYf,
        LOGICAL_TABLE_4_ACTION_PRIORITYf,
        LOGICAL_TABLE_5_ACTION_PRIORITYf,
        LOGICAL_TABLE_6_ACTION_PRIORITYf,
        LOGICAL_TABLE_7_ACTION_PRIORITYf,
        LOGICAL_TABLE_8_ACTION_PRIORITYf,
        LOGICAL_TABLE_9_ACTION_PRIORITYf,
        LOGICAL_TABLE_10_ACTION_PRIORITYf,
        LOGICAL_TABLE_11_ACTION_PRIORITYf,
        LOGICAL_TABLE_12_ACTION_PRIORITYf,
        LOGICAL_TABLE_13_ACTION_PRIORITYf,
        LOGICAL_TABLE_14_ACTION_PRIORITYf,
        LOGICAL_TABLE_15_ACTION_PRIORITYf,
        LOGICAL_TABLE_16_ACTION_PRIORITYf,
        LOGICAL_TABLE_17_ACTION_PRIORITYf,
        LOGICAL_TABLE_18_ACTION_PRIORITYf,
        LOGICAL_TABLE_19_ACTION_PRIORITYf,
        LOGICAL_TABLE_20_ACTION_PRIORITYf,
        LOGICAL_TABLE_21_ACTION_PRIORITYf,
        LOGICAL_TABLE_22_ACTION_PRIORITYf,
        LOGICAL_TABLE_23_ACTION_PRIORITYf,
        LOGICAL_TABLE_24_ACTION_PRIORITYf,
        LOGICAL_TABLE_25_ACTION_PRIORITYf,
        LOGICAL_TABLE_26_ACTION_PRIORITYf,
        LOGICAL_TABLE_27_ACTION_PRIORITYf,
        LOGICAL_TABLE_28_ACTION_PRIORITYf,
        LOGICAL_TABLE_29_ACTION_PRIORITYf,
        LOGICAL_TABLE_30_ACTION_PRIORITYf,
        LOGICAL_TABLE_31_ACTION_PRIORITYf
    };

    static soc_mem_t lt_action_mem;

    /* Input parameters check. */
    if (NULL == fg || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                             IFP_LOGICAL_TABLE_ACTION_PRIORITYm,
                                             _FP_GLOBAL_INST, &lt_action_mem));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                         IFP_LOGICAL_TABLE_ACTION_PRIORITYm,
                                         fg->instance, &lt_action_mem));
    }
    /* Read logical table action priority entry from hardware. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, lt_action_mem,
                MEM_BLOCK_ANY, 0, &entry));

    /* Program Logical Tables configuration in hardware. */
    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {
        /* Get logical table info for the default FP instance. */
        lt_info = fc->lt_info[fg->instance][lt_idx];

        /* Get LT action priority value. */
        value = lt_info->lt_action_pri;
        field = lt_action_pri[lt_idx];

        /* Set the value in entry buffer that's to be programmed in HW. */
        soc_mem_field32_set(unit, lt_action_mem, &entry,
                field, value);
    }

    /* Write LT action priority entry in hardware. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_action_mem,
                MEM_BLOCK_ANY, 0, &entry));

    return BCM_E_NONE;
}

#if defined (BCM_HELIX5_SUPPORT)
/*
 * Function:
 *     _field_hx5_ingress_lt_partition_prio_write
 * Purpose:
 *     Write LT Partition priortiy for the given LT ID and Slice to HW.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group Structure.
 * Returns:
 *     BCM_E_XXX
 * Note: slice == -1 implies configure partition priority for all slices
 *       for the given lt_id.
 */

int
_bcm_field_hx5_ingress_lt_partition_prio_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg, int lt_id,
                                              int slice)
{
    int rv;                         /* Operation return status.         */
    int slice_num;                  /* Slice number.                    */
    int value;                      /* Partition Prio value.            */
    uint32 lt_config_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                    /* HW entry buffer.                 */
    _field_lt_config_t *lt_info;    /* Logical table info.              */
    soc_field_t field;              /* HW entry field.                  */
    _field_control_t    *fc;        /* Field control structure.     */

    static const soc_field_t lt_part_pri[] = {
        LOGICAL_PARTITION_PRIORITY_0f,
        LOGICAL_PARTITION_PRIORITY_1f,
        LOGICAL_PARTITION_PRIORITY_2f,
        LOGICAL_PARTITION_PRIORITY_3f,
        LOGICAL_PARTITION_PRIORITY_4f,
        LOGICAL_PARTITION_PRIORITY_5f,
        LOGICAL_PARTITION_PRIORITY_6f,
        LOGICAL_PARTITION_PRIORITY_7f,
        LOGICAL_PARTITION_PRIORITY_8f,
        LOGICAL_PARTITION_PRIORITY_9f,
        LOGICAL_PARTITION_PRIORITY_10f,
        LOGICAL_PARTITION_PRIORITY_11f,
        LOGICAL_PARTITION_PRIORITY_12f,
        LOGICAL_PARTITION_PRIORITY_13f,
        LOGICAL_PARTITION_PRIORITY_14f,
        LOGICAL_PARTITION_PRIORITY_15f,
        LOGICAL_PARTITION_PRIORITY_16f,
        LOGICAL_PARTITION_PRIORITY_17f

    };

    static soc_mem_t lt_config_mem = IFP_LOGICAL_TABLE_CONFIGm;

    /* Input parameters check. */
    if (NULL == fg || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    rv = soc_mem_read(unit, lt_config_mem, MEM_BLOCK_ANY,
            lt_id, &lt_config_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Get logical table info for the default FP instance. */
    lt_info = fc->lt_info[fg->instance][lt_id];
    for (slice_num = 0; slice_num < stage_fc->tcam_slices; slice_num++) {
        if ((slice != -1) && (slice_num != slice)) {
            continue;
        }
        field = lt_part_pri[slice_num];
        value = lt_info->lt_part_pri[slice_num];
        soc_mem_field32_set(unit, lt_config_mem,
                &lt_config_entry, field, value);
    }
    soc_mem_field32_set(unit, lt_config_mem, &lt_config_entry,
            LOGICAL_PARTITION_MAPf, lt_info->lt_part_map);

    /* Write new LT configuration in hardware. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_config_mem,
                MEM_BLOCK_ANY, lt_id, &lt_config_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_hx5_ingress_lt_partition_prio_install
 * Purpose:
 *     Write logical table Partition priortiy information to hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group Structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_hx5_ingress_lt_partition_prio_install(int unit, _field_stage_t *stage_fc,
                                            _field_group_t *fg)
{
    soc_field_t field;              /* HW entry field.                  */
    uint32 value;                   /* Entry field value.               */
    int lt_idx;                     /* LT iterator.                     */
    int slice;                      /* Slice iterator.                  */
    uint32 lt_config_entry[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                    /* HW entry buffer.                 */
    _field_lt_config_t *lt_info;    /* Logical table info.              */
    int rv;                         /* Operation return status.         */
    _field_control_t    *fc;        /* Field control structure.     */

    static const soc_field_t lt_part_pri[] = {
        LOGICAL_PARTITION_PRIORITY_0f,
        LOGICAL_PARTITION_PRIORITY_1f,
        LOGICAL_PARTITION_PRIORITY_2f,
        LOGICAL_PARTITION_PRIORITY_3f,
        LOGICAL_PARTITION_PRIORITY_4f,
        LOGICAL_PARTITION_PRIORITY_5f,
        LOGICAL_PARTITION_PRIORITY_6f,
        LOGICAL_PARTITION_PRIORITY_7f,
        LOGICAL_PARTITION_PRIORITY_8f,
        LOGICAL_PARTITION_PRIORITY_9f,
        LOGICAL_PARTITION_PRIORITY_10f,
        LOGICAL_PARTITION_PRIORITY_11f,
        LOGICAL_PARTITION_PRIORITY_12f,
        LOGICAL_PARTITION_PRIORITY_13f,
        LOGICAL_PARTITION_PRIORITY_14f,
        LOGICAL_PARTITION_PRIORITY_15f,
        LOGICAL_PARTITION_PRIORITY_16f,
        LOGICAL_PARTITION_PRIORITY_17f

    };

    static soc_mem_t lt_config_mem = IFP_LOGICAL_TABLE_CONFIGm;

    /* Input parameters check. */
    if (NULL == fg || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Program Logical Tables configuration in hardware. */
    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {

        /* Get logical table info for the default FP instance. */
        lt_info = fc->lt_info[fg->instance][lt_idx];

        rv = soc_mem_read(unit, lt_config_mem, MEM_BLOCK_ANY,
                          lt_idx, &lt_config_entry);
        BCM_IF_ERROR_RETURN(rv);

        /*
         * Construct the LT Partition Map and Partition
         * Priority entry for the LT.
         */
        for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
            field = lt_part_pri[slice];
            value = lt_info->lt_part_pri[slice];
            soc_mem_field32_set(unit, lt_config_mem,
                    &lt_config_entry, field, value);
        }

        soc_mem_field32_set(unit, lt_config_mem, &lt_config_entry,
                LOGICAL_PARTITION_MAPf, lt_info->lt_part_map);

        /* Write new LT configuration in hardware. */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_config_mem,
                    MEM_BLOCK_ANY, lt_idx, &lt_config_entry));
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *     _field_th_ingress_lt_partition_prio_write
 * Purpose:
 *     Write LT Partition priortiy for the given LT ID and Slice to HW.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group Structure.
 * Returns:
 *     BCM_E_XXX
 * Note: slice == -1 implies configure partition priority for all slices
 *       for the given lt_id.
 */

int
_bcm_field_th_ingress_lt_partition_prio_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg, int lt_id,
                                              int slice)
{
    int rv;                         /* Operation return status.         */
    int slice_num;                  /* Slice number.                    */
    int value;                      /* Partition Prio value.            */
    uint64 lt_config_entry;         /* HW entry buffer.                 */
    _field_lt_config_t *lt_info;    /* Logical table info.              */
    soc_field_t field;              /* HW entry field.                  */
    _field_control_t    *fc;        /* Field control structure.     */

    static const soc_field_t lt_part_pri[] = {
        LOGICAL_PARTITION_PRIORITY_0f,
        LOGICAL_PARTITION_PRIORITY_1f,
        LOGICAL_PARTITION_PRIORITY_2f,
        LOGICAL_PARTITION_PRIORITY_3f,
        LOGICAL_PARTITION_PRIORITY_4f,
        LOGICAL_PARTITION_PRIORITY_5f,
        LOGICAL_PARTITION_PRIORITY_6f,
        LOGICAL_PARTITION_PRIORITY_7f,
        LOGICAL_PARTITION_PRIORITY_8f,
        LOGICAL_PARTITION_PRIORITY_9f,
        LOGICAL_PARTITION_PRIORITY_10f,
        LOGICAL_PARTITION_PRIORITY_11f
    };

    static soc_reg_t lt_config_reg;

    /* Input parameters check. */
    if (NULL == fg || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Retrieve the Pipe instance. */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {

        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                      IFP_LOGICAL_TABLE_CONFIGr,
                                      _FP_GLOBAL_INST,
                                      &lt_config_reg));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                      IFP_LOGICAL_TABLE_CONFIGr,
                                      fg->instance,
                                      &lt_config_reg));
    }

    rv = soc_reg_get(unit, lt_config_reg, REG_PORT_ANY,
            lt_id, &lt_config_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Get logical table info for the default FP instance. */
    lt_info = fc->lt_info[fg->instance][lt_id];
    for (slice_num = 0; slice_num < stage_fc->tcam_slices; slice_num++) {
        if ((slice != -1) && (slice_num != slice)) {
            continue;
        }
        field = lt_part_pri[slice_num];
        value = lt_info->lt_part_pri[slice_num];
        soc_reg64_field32_set(unit, lt_config_reg,
                &lt_config_entry, field, value);
    }
    soc_reg64_field32_set(unit, lt_config_reg, &lt_config_entry,
            LOGICAL_PARTITION_MAPf, lt_info->lt_part_map);

    /* Write new LT configuration in hardware. */
    BCM_IF_ERROR_RETURN(soc_reg_set(unit, lt_config_reg,
                REG_PORT_ANY, lt_id, lt_config_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_th_ingress_lt_partition_prio_install
 * Purpose:
 *     Write logical table Partition priortiy information to hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group Structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_lt_partition_prio_install(int unit, _field_stage_t *stage_fc,
                                            _field_group_t *fg)
{
    soc_field_t field;              /* HW entry field.                  */
    uint32 value;                   /* Entry field value.               */
    int lt_idx;                     /* LT iterator.                     */
    int slice;                      /* Slice iterator.                  */
    uint64 lt_config_entry;         /* HW entry buffer.                 */
    _field_lt_config_t *lt_info;    /* Logical table info.              */
    int rv;                         /* Operation return status.         */
    _field_control_t    *fc;        /* Field control structure.     */

    static const soc_field_t lt_part_pri[] = {
        LOGICAL_PARTITION_PRIORITY_0f,
        LOGICAL_PARTITION_PRIORITY_1f,
        LOGICAL_PARTITION_PRIORITY_2f,
        LOGICAL_PARTITION_PRIORITY_3f,
        LOGICAL_PARTITION_PRIORITY_4f,
        LOGICAL_PARTITION_PRIORITY_5f,
        LOGICAL_PARTITION_PRIORITY_6f,
        LOGICAL_PARTITION_PRIORITY_7f,
        LOGICAL_PARTITION_PRIORITY_8f,
        LOGICAL_PARTITION_PRIORITY_9f,
        LOGICAL_PARTITION_PRIORITY_10f,
        LOGICAL_PARTITION_PRIORITY_11f
    };

    static soc_reg_t lt_config_reg;

    /* Input parameters check. */
    if (NULL == fg || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Skip for Flowtracker */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        return BCM_E_NONE;
    }
#endif

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Retrieve the Pipe instance. */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                      IFP_LOGICAL_TABLE_CONFIGr,
                                      _FP_GLOBAL_INST,
                                      &lt_config_reg));
    } else {

        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                      IFP_LOGICAL_TABLE_CONFIGr,
                                      fg->instance,
                                      &lt_config_reg));
    }
    /* Program Logical Tables configuration in hardware. */
    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {

        /* Get logical table info for the default FP instance. */
        lt_info = fc->lt_info[fg->instance][lt_idx];

        rv = soc_reg_get(unit, lt_config_reg, REG_PORT_ANY,
                lt_idx, &lt_config_entry);
        BCM_IF_ERROR_RETURN(rv);

        /*
         * Construct the LT Partition Map and Partition
         * Priority entry for the LT.
         */
        for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
            field = lt_part_pri[slice];
            value = lt_info->lt_part_pri[slice];
            soc_reg64_field32_set(unit, lt_config_reg,
                    &lt_config_entry, field, value);
        }
        soc_reg64_field32_set(unit, lt_config_reg, &lt_config_entry,
                LOGICAL_PARTITION_MAPf, lt_info->lt_part_map);

        /* Write new LT configuration in hardware. */
        BCM_IF_ERROR_RETURN(soc_reg_set(unit, lt_config_reg,
                   REG_PORT_ANY, lt_idx, lt_config_entry));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_ingress_logical_table_map_write
 * Purpose:
 *     Write logical table partition map information to hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group Structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_ingress_logical_table_map_write(int unit,
                                              _field_stage_t *stage_fc,
                                              _field_group_t *fg)
{
    int rv;                         /* Operation return status.         */

    /* Input parameters check. */
    if (NULL == fg || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    if ((fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) ||
        (fg->stage_id == _BCM_FIELD_STAGE_AMFTFP) ||
        (fg->stage_id == _BCM_FIELD_STAGE_AEFTFP)) {
        return BCM_E_NONE;
    }

    /* Install LT Action Priority. */
    rv = _bcm_field_th_ingress_lt_action_prio_install(unit, stage_fc, fg);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: LT Action Priority Install failed for group:%d\n\r"),
                 fg->gid));
       return rv;
    }

    /* Install LT Partition Priority. */
#if defined (BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        rv = _field_hx5_ingress_lt_partition_prio_install(unit, stage_fc, fg);
 
    } else
#endif
    {
        rv = _field_th_ingress_lt_partition_prio_install(unit, stage_fc, fg);
    }

    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: LT Partition Priority Install failed for group:%d\n\r"),
                 fg->gid));
       return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_lt_part_prio_reset
 * Purpose:
 *     Reset the LT Partition Priority and also clear the
 *     lt_map for the given slice.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group Structure.
 *     fs       - (IN) Field slice Structure.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_field_th_lt_part_prio_reset(int unit, _field_stage_t *stage_fc,
                                     _field_group_t *fg,
                                     _field_slice_t *fs)
{
    int              rv;
    int              found = 0;
    _field_slice_t   *fs_ptr;
    _field_control_t *fc;           /* Field control structure. */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg || NULL == fs) {
        return (BCM_E_PARAM);
    }

    if (NULL == fg->slices) {
       return BCM_E_INTERNAL;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Another group is using the same LT ID. So don't destroy it */
    if (fc->lt_info[fg->instance][fg->lt_id]->lt_ref_count > 1) {
        return BCM_E_NONE;
    }

    /* Clear Group's LT_ID bit in slice LT map. */
    fs->lt_map &= ~(1 << fg->lt_id);

    /* Reset the LT Partition Priority. */
    fc->lt_info[fg->instance][fg->lt_id]->lt_part_map &= ~(1 << fs->slice_number);
    fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[fs->slice_number] = 0;

    /*
     *  Parse the existing auto-expanded slices for the group
     *  And updae the partition priority.
    */
    for (fs_ptr = fg->slices; fs_ptr != NULL; fs_ptr = fs_ptr->next) {
        if (fs_ptr == fs) {
           found = 1;
           continue;
        }

        if (found == 1) {
            fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[fs_ptr->slice_number] += 1;
        }
    }

    /* Set the partition Priority. */
#if defined (BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        rv = _bcm_field_hx5_ingress_lt_partition_prio_write(unit, stage_fc, fg,
                                                           fg->lt_id, -1);
 
    } else
#endif
    {
        rv = _bcm_field_th_ingress_lt_partition_prio_write(unit, stage_fc, fg,
                                                           fg->lt_id, -1);
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "Error: LT Partition Reset failed for"
             " group:%d lt_id:%d slice:%d\n\r"),
             fg->gid, fg->lt_id, fs->slice_number));
    }

    return rv;
}


/*
 * Function:
 *     _bcm_field_th_lt_part_prio_value_get
 * Purpose:
 *     Configure and return a LT Partition Priority for the given LT ID from a group.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     fg           - (IN)  Field group Structure.
 *     lt_part_prio - (OUT) LT Partition priority value.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_field_th_lt_part_prio_value_get(int unit, _field_group_t *fg,
                                         int *lt_part_prio)
{
    int            part_prio = 0;
    _field_slice_t *group_fs;
    _field_control_t  *fc;           /* Field control structure. */
    _field_stage_t      *stage_fc;      /* Stage field control.         */

    /* Input parameters check. */
    if (NULL == fg || NULL == lt_part_prio) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                &stage_fc));

    /* Parse the existing auto-expanded slices for the group */
    for (group_fs = fg->slices; group_fs != NULL; group_fs = group_fs->next) {
        part_prio = fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[group_fs->slice_number];
    }

    /* Assign the least priority to the latest slice. */
    *lt_part_prio = part_prio - 1;
    if (*lt_part_prio <= 0) {
       return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_presel_entry_hw_remove
 * Purpose:
 *     Remove Preselector Entries from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_presel - (IN) Field Preselector Entries.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_presel_entry_hw_remove(int unit, _field_presel_entry_t *f_presel)
{
    int                    parts_count = -1; /* Entry parts count.          */
    int                    part;             /* Entry part.                 */
    int                    tcam_idx;         /* LT TCAM entry index.        */
    soc_mem_t              lt_tcam_mem;      /* LT TCAM Memory.             */
    _field_stage_t        *stage_fc;         /* Field Stage Pointer.        */
    _field_presel_entry_t *f_presel_p;       /* Presel Part entry.          */

   /* Input parameters check. */
    if (NULL == f_presel) {
        return (BCM_E_PARAM);
    }

    if ((f_presel->flags & _FP_ENTRY_INSTALLED) == 0) {
        /* Entry not installed in hardware. */
        return (BCM_E_NONE);
    }

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            f_presel->group->stage_id, &stage_fc));

    /* Get number of entry parts using group flags. . */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        f_presel->group->flags, &parts_count));

    /* Get TCAM view to be used for programming the hardware. */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                                      f_presel->group->instance,
                                                     _BCM_FIELD_MEM_TYPE_IFP_LT,
                                        _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                                           &lt_tcam_mem, NULL));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                                      f_presel->group->instance,
                                                      _BCM_FIELD_MEM_TYPE_EM_LT,
                                        _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                                           &lt_tcam_mem, NULL));
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if ((soc_feature(unit, soc_feature_field_flowtracker_support)) &&
        (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER)) {
        return _bcm_field_hx5_ft_lt_entry_hw_remove(unit, f_presel);
#endif
    } else {
        return (BCM_E_PARAM);
    }

    /* Iterate over entry parts and remove the entry from HW. */
    f_presel_p = f_presel;
    for (part = 0; part < parts_count; part++) {
        if (f_presel_p == NULL) {
           break;
        }

        /* Given entry part number, get the entry TCAM index. */
        BCM_IF_ERROR_RETURN(_bcm_field_presel_entry_tcam_idx_get(unit,
                            f_presel_p, f_presel_p->lt_fs, &tcam_idx));

        /* Clear TCAM entry in hardware at the supplied TCAM index. */
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                         tcam_idx, soc_mem_entry_null(unit, lt_tcam_mem)));

        f_presel_p->flags |= _FP_ENTRY_DIRTY;
        f_presel_p->flags &= ~(_FP_ENTRY_INSTALLED);
        f_presel_p->flags &= ~(_FP_ENTRY_ENABLED);

        f_presel_p = f_presel_p->next;
    }

    return (BCM_E_NONE);
}

/* Function to disable the presel entries for a given group */
int
_bcm_field_th_group_presel_enable_set(int unit,
                                      _field_stage_t *stage_fc,
                                      _field_group_t *fg,
                                      uint32 valid)
{
    int rv, idx;
    int                    tcam_idx;
    soc_mem_t              lt_tcam_mem;
    _field_lt_slice_t     *lt_part_fs;
    _field_presel_entry_t *f_presel_p;
    _field_presel_entry_t *f_presel;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /*
     * Get TCAM view to be used for programming the hardware based on the group
     * operational mode.
     */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                                     fg->instance,
                                       _BCM_FIELD_MEM_TYPE_IFP_LT,
                          _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                             &lt_tcam_mem, NULL));
        /*
         * For TH2 and TD3, the IFP valid field is of 2 bits,
         * hence the value 3 has to be set in case of enable
         */
        valid = (valid) ?
                ((soc_feature(unit, soc_feature_field_multi_pipe_enhanced) ||
                  soc_feature(unit, soc_feature_td3_style_fp)) ? 3 : 1) : 0;
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                                     fg->instance,
                                        _BCM_FIELD_MEM_TYPE_EM_LT,
                          _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                             &lt_tcam_mem, NULL));
        /* TD3 EM valid field is of 2 bits */ 
        valid = (valid) ?
                (soc_feature(unit, soc_feature_td3_style_fp) ? 3 : 1) : 0;
    } else {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(rv);

    f_presel = fg->presel_ent_arr[0];
    if (f_presel == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Update Presel LT Slices. */
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
        if (fg->presel_ent_arr[idx] != NULL) {
           f_presel = fg->presel_ent_arr[idx];
        } else {
           continue;
        }

        f_presel_p = f_presel;
        for (; f_presel_p != NULL; f_presel_p = f_presel_p->next) {
            lt_part_fs = f_presel_p->lt_fs;
            for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
                 rv = _bcm_field_presel_entry_tcam_idx_get(unit, f_presel_p,
                                                           lt_part_fs,
                                                           &tcam_idx);
                 BCM_IF_ERROR_RETURN(rv);
                 /* Install the LT Entry in HW. */
                 rv = _bcm_field_th_lt_entry_hw_install(unit, lt_tcam_mem,
                                                   tcam_idx,
                                                   NULL, NULL, NULL,
                                                   valid);
                 BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    /* Enable/Disable group flags. */
    if (valid) {
        fg->flags |= _FP_GROUP_LOOKUP_ENABLED;
    } else {
        fg->flags &= ~_FP_GROUP_LOOKUP_ENABLED;
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_field_th_source_class_mode_set
 * Purpose:
 *      Retrieve Field Processor Source Class operational mode
 *      per-CAP stage.
 * Parameters:
 *      unit    - (IN)  BCM Device number.
 *      stage   - (IN)  Field Stage enum value.
 *      pbmp    - (IN)  Device Port Bitmap value
 *      mode    - (IN)  Reference to Field Source Class Operational
 *                      Mode enum value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Valid "stage" input parameter values supported by
 *      this API are:
 * 	            "bcmFieldStageIngress"
 *              "bcmFieldStageIngressExactMatch"
 */
    int
_bcm_field_th_source_class_mode_set(int unit,
                                    bcm_field_stage_t stage,
                                    bcm_pbmp_t pbmp,
                                    bcm_field_src_class_mode_t mode)
{
    _field_stage_t                *stage_fc; /* Stage field control structure. */
    _field_stage_id_t             stage_id;  /* Pipeline stage id */
    _field_control_t              *fc;
    _field_presel_entry_t         *f_presel = NULL; /* Internal Presel descriptor */
    _field_presel_info_t          *presel_info;     /* Presel information structure */
    bcm_field_presel_t            presel_idx = 0;
    bcm_field_qualify_t           stage_qual;
    bcm_field_group_oper_mode_t   group_mode;
    uint8                         pipe_idx = 0;
    bcm_pbmp_t                    pbmp_temp;
    soc_reg_t                     lt_select_cfg_reg = INVALIDr;
    bcm_pbmp_t                    all_pbmp;   /* Group port bitmap.           */

    if (mode >= bcmFieldSrcClassModeCount) {
        return (BCM_E_PARAM);
    }

    switch (stage) {
        case bcmFieldStageIngress:
             lt_select_cfg_reg = IFP_LOGICAL_TABLE_SELECT_CONFIGr;
             stage_id = _BCM_FIELD_STAGE_INGRESS;
             stage_qual = bcmFieldQualifyStageIngress;
             break;
        case bcmFieldStageIngressExactMatch:
             stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
             stage_qual = bcmFieldQualifyStageIngressExactMatch;
             lt_select_cfg_reg = EXACT_MATCH_LOGICAL_TABLE_SELECT_CONFIGr;
             break;
#if defined (BCM_FLOWTRACKER_SUPPORT)
        case bcmFieldStageIngressFlowtracker:
             stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
             stage_qual = bcmFieldQualifyStageIngressFlowtracker;
             lt_select_cfg_reg = BROADSCAN_LOGICAL_TABLE_SELECT_CONFIGr;
             break;
#endif
        default:
            return BCM_E_PARAM;
    }



    /* Verifiy PBMP based on group mode and port config */
    /* Get group mode */
    if (soc_feature(unit, soc_feature_field_single_pipe_support)) {
        group_mode = bcmFieldGroupOperModeGlobal;
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_oper_mode_get(unit,
                    stage_qual, &group_mode));
    }

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

    if (group_mode == bcmFieldGroupOperModeGlobal) {
        /* Pbmp should match all the valid ports */
        if (!BCM_PBMP_EQ(all_pbmp, pbmp)) {
            return BCM_E_PARAM;
        }
    } else if (group_mode == bcmFieldGroupOperModePipeLocal) {
        /* Pbmp provided should be belong to only one pipe */
        for (pipe_idx = 0; pipe_idx < BCM_PIPES_MAX ; pipe_idx++) {
            BCM_PBMP_CLEAR(pbmp_temp);
            BCM_PBMP_ASSIGN(pbmp_temp, PBMP_PIPE(unit, pipe_idx));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_LB(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_RDB_ALL(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_FAE_ALL(unit));
            if (BCM_PBMP_EQ(pbmp_temp, pbmp)) {
                break;
            }
        }

        /* Pbmp doesn't match any of the pipes */
        if (pipe_idx == BCM_PIPES_MAX) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                            lt_select_cfg_reg,
                                            pipe_idx,
                                            &lt_select_cfg_reg));
    } else {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    /* Mode can't be changed unless no entry is configured for that stage */
    if(mode != stage_fc->field_src_class_mode[pipe_idx]) {

        BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

        /* Get the preselector information */
        presel_info = fc->presel_info;
        if (presel_info == NULL) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "ERROR: Field control for Presel Information returns NULL.\n\r")));
            return BCM_E_INTERNAL;
        }

        /* Loop through all the presel id to check if Mixed src class was configured
         * for any entry belonging to this stage */
        for (presel_idx = 0 ; presel_idx < presel_info->presel_limit; presel_idx++) {
            if (BCM_FIELD_PRESEL_TEST(presel_info->presel_set, presel_idx)) {
                /* Find preselector descriptor */
                BCM_IF_ERROR_RETURN
                    (_bcm_field_presel_entry_get(unit, presel_idx, &f_presel));

                /* Preselecor attached to ingress or extact match stage
                 * if MIXED SRC FLAG is set return error */
                if (BCM_FIELD_QSET_TEST(f_presel->p_qset, stage_qual) &&
                        BCM_PBMP_EQ(f_presel->mixed_src_class_pbmp, pbmp) &&
                        (f_presel->flags & _FP_ENTRY_MIXED_SRC_CLASS_QUALIFIED)) {
                    return BCM_E_PARAM;
                }
            }
        }

        stage_fc->field_src_class_mode[pipe_idx] = mode;
        /* Write to HW */
        if ((lt_select_cfg_reg != INVALIDr) &&
             SOC_REG_FIELD_VALID(unit, lt_select_cfg_reg, SOURCE_CLASS_MODEf)) {
            BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit, lt_select_cfg_reg,
                        REG_PORT_ANY,
                        SOURCE_CLASS_MODEf,
                        mode));
        } else {
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th_source_class_mode_get
 * Purpose:
 *      Retrieve Field Processor Source Class operational mode
 *      per-CAP stage.
 * Parameters:
 *      unit    - (IN)  BCM Device number.
 *      stage   - (IN)  Field Stage enum value.
 *      pbmp    - (IN)  Device Port Bitmap value
 *      mode    - (OUT) Reference to Field Source Class Operational
 *                      Mode enum value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Valid "stage" input parameter values supported by
 *      this API are:
 * 	            "bcmFieldStageIngress"
 *              "bcmFieldStageIngressExactMatch"
 *              "bcmFieldStageIngressFlowtracker"
 */
    int
_bcm_field_th_source_class_mode_get(int unit,
                                    bcm_field_stage_t stage,
                                    bcm_pbmp_t pbmp,
                                    bcm_field_src_class_mode_t *mode)
{
    _field_stage_t     *stage_fc; /* Stage field control structure. */
    _field_stage_id_t  stage_id;  /* Pipeline stage id */
    bcm_field_qualify_t           stage_qual;
    bcm_field_group_oper_mode_t   group_mode;
    uint8                         pipe_idx = 0;
    bcm_pbmp_t                    pbmp_temp;
    bcm_pbmp_t                    all_pbmp;   /* Group port bitmap.           */

    /* Applicable for Ingress and ExtactMatch only */
    if ((stage != bcmFieldStageIngress) &&
            (stage != bcmFieldStageIngressExactMatch) &&
            (stage != bcmFieldStageIngressFlowtracker)) {
        return (BCM_E_PARAM);
    }

    if (stage == bcmFieldStageIngress) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
        stage_qual = bcmFieldQualifyStageIngress;
    } else if (stage == bcmFieldStageIngressExactMatch) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
        stage_qual = bcmFieldQualifyStageIngressExactMatch;
    } else { /* bcmFieldStageIngressFlowtracker */
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
        stage_qual = bcmFieldQualifyStageIngressFlowtracker;
    }

    /* Verifiy PBMP based on group mode and port config */
    /* Get group mode */
    if (soc_feature(unit, soc_feature_field_single_pipe_support)) {
        group_mode = bcmFieldGroupOperModeGlobal;
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_oper_mode_get(unit,
                    stage_qual, &group_mode));
    }

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

    if (group_mode == bcmFieldGroupOperModeGlobal) {
        /* Pbmp should match all the valid ports */
        if (!BCM_PBMP_EQ(all_pbmp, pbmp)) {
            return BCM_E_PARAM;
        }
    } else if (group_mode == bcmFieldGroupOperModePipeLocal) {
        /* Pbmp provided should be belong to only one pipe */
        for (pipe_idx = 0; pipe_idx < BCM_PIPES_MAX ; pipe_idx++) {
            BCM_PBMP_CLEAR(pbmp_temp);
            BCM_PBMP_ASSIGN(pbmp_temp, PBMP_PIPE(unit, pipe_idx));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_LB(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_RDB_ALL(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_FAE_ALL(unit));
            if (BCM_PBMP_EQ(pbmp_temp, pbmp)) {
                break;
            }
        }

        /* Pbmp doesn't match any of the pipes */
        if (pipe_idx == BCM_PIPES_MAX) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    *mode = stage_fc->field_src_class_mode[pipe_idx];

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_field_th_qualify_MixedSrcClassId
 * Purpose:
 *     Add a Mixed Source Class Id field qualification to a field entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) Field Entry id.
 *      pbmp  - (IN) Device Port Bitmap value
 *      data  - (IN) Mixed Source Class Id.
 *      mask  - (IN) Mixed Source Class Id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
    int
_bcm_field_th_qualify_MixedSrcClassId(int unit,
                                      bcm_field_entry_t entry,
                                      bcm_pbmp_t pbmp,
                                      bcm_field_src_class_t data,
                                      bcm_field_src_class_t mask)
{
    bcm_field_presel_t            presel;             /* Presel id. */
    _field_presel_entry_t         *f_presel = NULL;   /* Presel Entry descriptor. */
    _field_stage_id_t             stage_id;
    _field_stage_t                *stage_fc;
    uint32                        hw_data;            /* HW data match criteria.  */
    uint32                        hw_mask;            /* HW data mask.            */
    int                           rv = BCM_E_UNAVAIL; /* Operation return status. */
    bcm_field_qualify_t           stage_qual;
    bcm_field_group_oper_mode_t   group_mode;
    uint8                         pipe_idx = 0;
    bcm_pbmp_t                    pbmp_temp;
    bcm_pbmp_t                    all_pbmp;   /* Group port bitmap.           */

    if (!(soc_feature(unit, soc_feature_field_preselector_support) &&
                (_BCM_FIELD_IS_PRESEL_ENTRY(entry) == TRUE))) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_field_entry_presel_resolve(unit,
                entry, &presel, &f_presel));

    /* Mixed source class is applicable only
     * for Ingress, extact match and flowtracker stages */
    if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                bcmFieldQualifyStageIngress)) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
        stage_qual = bcmFieldQualifyStageIngress;
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                bcmFieldQualifyStageIngressExactMatch)) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
        stage_qual = bcmFieldQualifyStageIngressExactMatch;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                bcmFieldQualifyStageIngressFlowtracker)) {
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
        stage_qual = bcmFieldQualifyStageIngressFlowtracker;
#endif
    } else {
        return (BCM_E_PARAM);
    }

    /* Verifiy PBMP based on group mode and port config */
    /* Get group mode */
    if (soc_feature(unit, soc_feature_field_single_pipe_support)) {
        group_mode = bcmFieldGroupOperModeGlobal;
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_oper_mode_get(unit,
                    stage_qual, &group_mode));
    }

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

    if (group_mode == bcmFieldGroupOperModeGlobal) {
        /* Pbmp should match all the valid ports */
        if (!BCM_PBMP_EQ(all_pbmp, pbmp)) {
            return BCM_E_PARAM;
        }
    } else if (group_mode == bcmFieldGroupOperModePipeLocal) {
        /* Pbmp provided should be belong to only one pipe */
        for (pipe_idx = 0; pipe_idx < BCM_PIPES_MAX ; pipe_idx++) {
            BCM_PBMP_CLEAR(pbmp_temp);
            BCM_PBMP_ASSIGN(pbmp_temp, PBMP_PIPE(unit, pipe_idx));

            SOC_PBMP_REMOVE(pbmp_temp, PBMP_LB(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_RDB_ALL(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_FAE_ALL(unit));
            if (BCM_PBMP_EQ(pbmp_temp, pbmp)) {
                break;
            }
        }

        /* Pbmp doesn't match any of the pipes */
        if (pipe_idx == BCM_PIPES_MAX) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    switch (stage_fc->field_src_class_mode[pipe_idx]) {
        case bcmFieldSrcClassModeDefault:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xF) << 28))
                    | ((data.src_class_field & 0x3) << 26)
                    | ((data.dst_class_field & 0x3FF) << 16)
                    | ((data.udf_class & 0xFF) << 8)
                    | (data.intf_class_port & 0xFF);

                hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xF) << 28))
                    | ((mask.src_class_field & 0x3) << 26)
                    | ((mask.dst_class_field & 0x3FF) << 16)
                    | ((mask.udf_class & 0xFF) << 8)
                    | (mask.intf_class_port & 0xFF);
            } else {
                hw_data = (uint32)((uint32)(
                    (data.src_class_field & 0x3) << 30))
                    | ((data.dst_class_field & 0x3FF) << 20)
                    | ((data.udf_class & 0xFF) << 12)
                    | ((data.intf_class_port & 0xFF) << 4)
                    | (data.intf_class_l2 & 0xF);

                hw_mask = (uint32)((uint32)(
                    (mask.src_class_field & 0x3) << 30 ))
                    | ((mask.dst_class_field & 0x3FF) << 20)
                    | ((mask.udf_class & 0xFF) << 12)
                    | ((mask.intf_class_port & 0xFF) << 4)
                    | (mask.intf_class_l2 & 0xF);
            }
            break;
        case bcmFieldSrcClassModeSDN:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xF) << 28))
                    | ((data.src_class_field & 0x3) << 26)
                    | ((data.dst_class_field & 0x3FF) << 16)
                    | ((data.udf_class & 0xF) << 12)
                    | ((data.intf_class_port & 0xF) << 4)
                    | (data.intf_class_l3 & 0xF);

                hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xF) << 28))
                    | ((mask.src_class_field & 0x3) << 26)
                    | ((mask.dst_class_field & 0x3FF) << 16)
                    | ((mask.udf_class & 0xF) << 12)
                    | ((mask.intf_class_port & 0xF) << 4)
                    | (mask.intf_class_l3 & 0xF);
            } else {
                hw_data = (uint32)((uint32)(
                    (data.src_class_field & 0x3) << 30))
                    | ((data.dst_class_field & 0x3FF) << 20)
                    | ((data.udf_class & 0xF) << 16)
                    | ((data.intf_class_port & 0xF) << 12)
                    | ((data.intf_class_l2 & 0xF) << 8)
                    | ((data.intf_class_vport & 0xF) << 4)
                    | (data.intf_class_l3 & 0xF);

                hw_mask = (uint32)((uint32)(
                    (mask.src_class_field & 0x3) << 30))
                    | ((mask.dst_class_field & 0x3FF) << 20)
                    | ((mask.udf_class & 0xF) << 16)
                    | ((mask.intf_class_port & 0xF) << 12)
                    | ((mask.intf_class_l2 & 0xF) << 8)
                    | ((mask.intf_class_vport & 0xF) << 4)
                    | (mask.intf_class_l3 & 0xF);
            }
            break;
        case bcmFieldSrcClassModeBalanced:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xF) << 28))
                    | ((data.dst_class_field & 0xFF) << 20)
                    | ((data.udf_class & 0xF) << 16)
                    | ((data.intf_class_port & 0xFF) << 4)
                    | (data.intf_class_l3 & 0xF);
                hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xF) << 28))
                    | ((mask.dst_class_field & 0xFF) << 20)
                    | ((mask.udf_class & 0xF) << 16)
                    | ((mask.intf_class_port & 0xFF) << 4)
                    | (mask.intf_class_l3 & 0xF);
            } else {
                hw_data = (uint32)((uint32)(
                    (data.dst_class_field & 0xFF) << 24))
                    | ((data.udf_class & 0xF) << 20)
                    | ((data.intf_class_port & 0xFF) << 12)
                    | ((data.intf_class_l2 & 0xF) << 8)
                    | ((data.intf_class_vport & 0xF) << 4)
                    | (data.intf_class_l3 & 0xF);

                hw_mask = (uint32)((uint32)(
                    (mask.dst_class_field & 0xFF) << 24))
                    | ((mask.udf_class & 0xF) << 20)
                    | ((mask.intf_class_port & 0xFF) << 12)
                    | ((mask.intf_class_l2 & 0xF) << 8)
                    | ((mask.intf_class_vport & 0xF) << 4)
                    | (mask.intf_class_l3 & 0xF);
            }
            break;
        case bcmFieldSrcClassModeOverlayNetworks:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                hw_data = (uint32)((uint32)((data.intf_class_l2 & 0xFF) << 24))
                    | ((data.dst_class_field & 0xF) << 20)
                    | ((data.intf_class_port & 0xF) << 8)
                    | (data.intf_class_l3 & 0xFF);

                hw_mask = (uint32)((uint32)((mask.intf_class_l2 & 0xFF) << 24))
                    | ((data.dst_class_field & 0xF) << 20)
                    | ((data.intf_class_port & 0xF) << 8)
                    | (data.intf_class_l3 & 0xFF);
            } else {
                hw_data = (uint32)((uint32)(
                    (data.dst_class_field & 0xF) << 28))
                    | ((data.intf_class_port & 0xF) << 24)
                    | ((data.intf_class_l2 & 0xFF) << 16)
                    | ((data.intf_class_vport & 0xFF) << 8)
                    | (data.intf_class_l3 & 0xFF);

                hw_mask = (uint32)((uint32)(
                    (mask.dst_class_field & 0xF) << 28))
                    | ((mask.intf_class_port & 0xF) << 24)
                    | ((mask.intf_class_l2 & 0xFF) << 16)
                    | ((mask.intf_class_vport & 0xFF) << 8)
                    | (mask.intf_class_l3 & 0xFF);
            }
            break;
        default:
            return (BCM_E_PARAM);
            break;
    }
    rv = _field_qualify32(unit, entry, bcmFieldQualifyMixedSrcClassId,
            hw_data, hw_mask);
    if (BCM_SUCCESS(rv)) {
        f_presel->flags |= _FP_ENTRY_MIXED_SRC_CLASS_QUALIFIED;
        BCM_PBMP_ASSIGN(f_presel->mixed_src_class_pbmp, pbmp);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_field_th_qualify_MixedSrcClassId_get
 * Purpose:
 *      Get match criteria for bcmFieildQualifyMixedSrcClassId
 *                     qualifier from the field entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      entry - (IN) Field Entry id.
 *      pbmp  - (IN) Device Port Bitmap value
 *      data  - (OUT) Mixed Source Class Id.
 *      mask  - (OUT) Mixed Source Class Id mask.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
    int
_bcm_field_th_qualify_MixedSrcClassId_get(int unit,
                                          bcm_field_entry_t entry,
                                          bcm_pbmp_t pbmp,
                                          uint32 hw_data,
                                          uint32 hw_mask,
                                          bcm_field_src_class_t *data,
                                          bcm_field_src_class_t *mask)
{
    bcm_field_presel_t            presel;             /* Presel id. */
    _field_presel_entry_t         *f_presel = NULL;   /* Presel Entry descriptor. */
    _field_stage_id_t             stage_id;
    _field_stage_t                *stage_fc;
    bcm_field_qualify_t           stage_qual;
    bcm_field_group_oper_mode_t   group_mode;
    uint8                         pipe_idx = 0;
    bcm_pbmp_t                    pbmp_temp;
    bcm_pbmp_t                    all_pbmp;   /* Group port bitmap.           */

    if ((data == NULL) || (mask == NULL)) {
        return BCM_E_PARAM;
    }

    if (!(soc_feature(unit, soc_feature_field_preselector_support) &&
                (_BCM_FIELD_IS_PRESEL_ENTRY(entry) == TRUE))) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_field_entry_presel_resolve(unit,
                entry, &presel, &f_presel));

    /* Mixed source class is applicable only
     * for Ingress, extact match and flowtracker stages */
    if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                bcmFieldQualifyStageIngress)) {
        stage_id = _BCM_FIELD_STAGE_INGRESS;
        stage_qual = bcmFieldQualifyStageIngress;
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                bcmFieldQualifyStageIngressExactMatch)) {
        stage_id = _BCM_FIELD_STAGE_EXACTMATCH;
        stage_qual = bcmFieldQualifyStageIngressExactMatch;
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (BCM_FIELD_QSET_TEST(f_presel->p_qset,
                bcmFieldQualifyStageIngressFlowtracker)) {
        stage_id = _BCM_FIELD_STAGE_FLOWTRACKER;
        stage_qual = bcmFieldQualifyStageIngressFlowtracker;
#endif
    } else {
        return (BCM_E_PARAM);
    }

    /* Verifiy PBMP based on group mode and port config */
    /* Get group mode */
    if (soc_feature(unit, soc_feature_field_single_pipe_support)) {
        group_mode = bcmFieldGroupOperModeGlobal;
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_oper_mode_get(unit,
                    stage_qual, &group_mode));
    }

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

    if (group_mode == bcmFieldGroupOperModeGlobal) {
        /* Pbmp should match all the valid ports */
        if (!BCM_PBMP_EQ(all_pbmp, pbmp)) {
            return BCM_E_PARAM;
        }
    } else if (group_mode == bcmFieldGroupOperModePipeLocal) {
        /* Pbmp provided should be belong to only one pipe */
        for (pipe_idx = 0; pipe_idx < BCM_PIPES_MAX ; pipe_idx++) {
            BCM_PBMP_CLEAR(pbmp_temp);
            BCM_PBMP_ASSIGN(pbmp_temp, PBMP_PIPE(unit, pipe_idx));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_LB(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_RDB_ALL(unit));
            SOC_PBMP_REMOVE(pbmp_temp, PBMP_FAE_ALL(unit));
            if (BCM_PBMP_EQ(pbmp_temp, pbmp)) {
                break;
            }
        }

        /* Pbmp doesn't match any of the pipes */
        if (pipe_idx == BCM_PIPES_MAX) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    sal_memset(data, 0, sizeof(bcm_field_src_class_t));
    sal_memset(mask, 0, sizeof(bcm_field_src_class_t));

    switch (stage_fc->field_src_class_mode[pipe_idx]) {
        case bcmFieldSrcClassModeDefault:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                data->intf_class_l2     = (hw_data >> 28) & 0xF;
                data->src_class_field   = (hw_data >> 26) & 0x3;
                data->dst_class_field   = (hw_data >> 16) & 0x3FF;
                data->udf_class         = (hw_data >> 8) & 0xFF;
                data->intf_class_port   = hw_data & 0xFF;

                mask->intf_class_l2     = (hw_data >> 28) & 0xF;
                mask->src_class_field   = (hw_data >> 26) & 0x3;
                mask->dst_class_field   = (hw_data >> 16) & 0x3FF;
                mask->udf_class         = (hw_data >> 8) & 0xFF;
                mask->intf_class_port   = hw_data & 0xFF;
            } else {
                data->src_class_field   = (hw_data >> 30) & 0x3;
                data->dst_class_field   = (hw_data >> 20) & 0x3FF;
                data->udf_class         = (hw_data >> 12) & 0xFF;
                data->intf_class_port   = (hw_data >> 4) & 0xFF;
                data->intf_class_l2     = hw_data & 0xF;

                mask->src_class_field   = (hw_mask >> 30) & 0x3;
                mask->dst_class_field   = (hw_mask >> 20) & 0x3FF;
                mask->udf_class         = (hw_mask >> 12) & 0xFF;
                mask->intf_class_port   = (hw_mask >> 4) & 0xFF;
                mask->intf_class_l2     = hw_mask & 0xF;
            }
            break;
        case bcmFieldSrcClassModeSDN:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                data->intf_class_l2     = (hw_data >> 28) & 0xF;
                data->src_class_field   = (hw_data >> 26) & 0x3;
                data->dst_class_field   = (hw_data >> 16) & 0x3FF;
                data->udf_class         = (hw_data >> 12) & 0xF;
                data->intf_class_port   = (hw_data >> 4) & 0xF;
                data->intf_class_l3     = hw_data & 0xF;

                mask->intf_class_l2     = (hw_data >> 28) & 0xF;
                mask->src_class_field   = (hw_data >> 26) & 0x3;
                mask->dst_class_field   = (hw_data >> 16) & 0x3FF;
                mask->udf_class         = (hw_data >> 12) & 0xF;
                mask->intf_class_port   = (hw_data >> 4) & 0xF;
                mask->intf_class_l3     = hw_data & 0xF;
            } else {
                data->src_class_field   = (hw_data >> 30) & 0x3;
                data->dst_class_field   = (hw_data >> 20) & 0x3FF;
                data->udf_class         = (hw_data >> 16) & 0xF;
                data->intf_class_port   = (hw_data >> 12) & 0xF;
                data->intf_class_l2     = (hw_data >> 8) & 0xF;
                data->intf_class_vport  = (hw_data >> 4) & 0xF;
                data->intf_class_l3     = hw_data & 0xF;

                mask->src_class_field   = (hw_mask >> 30) & 0x3;
                mask->dst_class_field   = (hw_mask >> 20) & 0x3FF;
                mask->udf_class         = (hw_mask >> 16) & 0xF;
                mask->intf_class_port   = (hw_mask >> 12) & 0xF;
                mask->intf_class_l2     = (hw_mask >> 8) & 0xF;
                mask->intf_class_vport  = (hw_mask >> 4) & 0xF;
                mask->intf_class_l3     = hw_mask & 0xF;
            }
            break;
        case bcmFieldSrcClassModeBalanced:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                data->intf_class_l2     = (hw_data >> 28) & 0xF;
                data->dst_class_field   = (hw_data >> 20) & 0xFF;
                data->udf_class         = (hw_data >> 16) & 0xF;
                data->intf_class_port   = (hw_data >> 4) & 0xFF;
                data->intf_class_l3     = hw_data & 0xF;

                mask->intf_class_l2     = (hw_data >> 28) & 0xF;
                mask->dst_class_field   = (hw_data >> 20) & 0xFF;
                mask->udf_class         = (hw_data >> 16) & 0xF;
                mask->intf_class_port   = (hw_data >> 4) & 0xFF;
                mask->intf_class_l3     = hw_data & 0xF;
            } else {
                data->dst_class_field   = (hw_data >> 24) & 0xFF;
                data->udf_class         = (hw_data >> 20) & 0xF;
                data->intf_class_port   = (hw_data >> 12) & 0xFF;
                data->intf_class_l2     = (hw_data >> 8) & 0xF;
                data->intf_class_vport  = (hw_data >> 4) & 0xF;
                data->intf_class_l3     = hw_data & 0xF;

                mask->dst_class_field   = (hw_mask >> 24) & 0xFF;
                mask->udf_class         = (hw_mask >> 20) & 0xF;
                mask->intf_class_port   = (hw_mask >> 12) & 0xFF;
                mask->intf_class_l2     = (hw_mask >> 8) & 0xF;
                mask->intf_class_vport  = (hw_mask >> 4) & 0xF;
                mask->intf_class_l3     = hw_mask & 0xF;
            }
            break;
        case bcmFieldSrcClassModeOverlayNetworks:
            if (soc_feature(unit, soc_feature_th3_style_fp)) {
                data->intf_class_l2     = (hw_data >> 24) & 0xFF;
                data->dst_class_field   = (hw_data >> 20) & 0xF;
                data->intf_class_port   = (hw_data >> 8) & 0xF;
                data->intf_class_l3     = hw_data & 0xFF;

                mask->intf_class_l2     = (hw_data >> 24) & 0xFF;
                mask->dst_class_field   = (hw_data >> 20) & 0xF;
                mask->intf_class_port   = (hw_data >> 8) & 0xF;
                mask->intf_class_l3     = hw_data & 0xFF;
            } else {
                data->dst_class_field   = (hw_data >> 28) & 0xF;
                data->intf_class_port   = (hw_data >> 24) & 0xF;
                data->intf_class_l2     = (hw_data >> 16) & 0xFF;
                data->intf_class_vport  = (hw_data >> 8) & 0xFF;
                data->intf_class_l3     = hw_data & 0xFF;

                mask->dst_class_field   = (hw_mask >> 28) & 0xF;
                mask->intf_class_port   = (hw_mask >> 24) & 0xF;
                mask->intf_class_l2     = (hw_mask >> 16) & 0xFF;
                mask->intf_class_vport  = (hw_mask >> 8) & 0xFF;
                mask->intf_class_l3     = hw_mask & 0xFF;
            }
            break;
        default:
            return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_th_qualifier_MixedSrcClassId_delete
 *
 * Purpose: Resets data related to MixedSrxClassId
 *
 * Parameters:
 *     unit  - (IN)  BCM device number
 *     entry - (IN)  Field presel entry to operate on.
 *
 * Returns:
 *     BCM_E_XX
 */
int
_bcm_field_th_qualifier_MixedSrcClassId_delete(int unit, bcm_field_entry_t entry)
{
    int                     rv;                /* Operation return status. */
    _field_presel_entry_t   *f_presel;         /* Presel entry structure.  */
    bcm_field_presel_t      presel;            /* Field Preselector ID.    */

    /* Preselector feature check */
    if (!soc_feature(unit, soc_feature_field_preselector_support)) {
       return BCM_E_UNAVAIL;
    }

    /* Resolve the Preselector ID from the given Entry ID */
    rv = _bcm_field_entry_presel_resolve(unit, entry, &presel, &f_presel);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Unable to resolve the Presel Entry ID %d.\n\r"), entry));
       return rv;
    }

    f_presel->flags &= ~(_FP_ENTRY_MIXED_SRC_CLASS_QUALIFIED);
    BCM_PBMP_CLEAR(f_presel->mixed_src_class_pbmp);

    return BCM_E_NONE;
}

int
_bcm_field_max_lt_part_prio_get(int unit)
{
    if (soc_feature(unit, soc_feature_field_slices18)) {
        return 18;
    }
#ifdef BCM_UTT_SUPPORT
    if (soc_feature(unit, soc_feature_utt)) {
        if (soc_feature(unit, soc_feature_field_slices17)) {
            return 17;
        } else if (soc_feature(unit, soc_feature_field_slices16)) {
            return 16;
        } else if (soc_feature(unit, soc_feature_field_slices15)) {
            return 15;
        } else if (soc_feature(unit, soc_feature_field_slices14)) {
            return 14;
        } else if (soc_feature(unit, soc_feature_field_slices13)) {
            return 13;
        } else if (soc_feature(unit, soc_feature_field_slices12)) {
            return 12;
        } else if (soc_feature(unit, soc_feature_field_slices11)) {
            return 11;
        } else if (soc_feature(unit, soc_feature_field_slices10)) {
            return 10;
        } else if (soc_feature(unit, soc_feature_field_slices9)) {
            return 9;
        } else if (soc_feature(unit, soc_feature_field_slices8)) {
            return 8;
        }
    }
#endif
    return 15;
}

int
_bcm_field_th_max_lt_parts_get(int unit)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return 9;
    }
#endif
    if (soc_feature(unit, soc_feature_field_slices18)) {
        return 18;
    }
#ifdef BCM_UTT_SUPPORT
    if (soc_feature(unit, soc_feature_utt)) {
        if (soc_feature(unit, soc_feature_field_slices17)) {
            return 17;
        } else if (soc_feature(unit, soc_feature_field_slices16)) {
            return 16;
        } else if (soc_feature(unit, soc_feature_field_slices15)) {
            return 15;
        } else if (soc_feature(unit, soc_feature_field_slices14)) {
            return 14;
        } else if (soc_feature(unit, soc_feature_field_slices13)) {
            return 13;
        } else if (soc_feature(unit, soc_feature_field_slices12)) {
            return 12;
        } else if (soc_feature(unit, soc_feature_field_slices11)) {
            return 11;
        } else if (soc_feature(unit, soc_feature_field_slices10)) {
            return 10;
        } else if (soc_feature(unit, soc_feature_field_slices9)) {
            return 9;
        } else if (soc_feature(unit, soc_feature_field_slices8)) {
            return 8;
        }
    }
#endif
    return 12;
}

#endif /* (BCM_TOMAHAWK_SUPPORT) && (BCM_FIELD_SUPPORT) */
