/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     Field Processor routines Specific to Firebolt6.
 */

#include <soc/defs.h>

#if defined(BCM_FIREBOLT6_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <bcm/error.h>
#include <bcm/field.h>
#include <shared/bsl.h>

#include <bcm_int/esw/field_compression/field_compression_internal.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/firebolt6.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm_int/esw/flowtracker/ft_interface.h>
#include <soc/firebolt6.h>

/*
 * Function:
 *    _field_fb6_amft_qualifiers_init
 *
 * Purpose:
 *    Initializes flowtracker stage qualifiers for
 *    aggregate mmu flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_amft_qualifiers_init(
    int unit,
    _field_stage_t *stage_fc)
{
    soc_mem_t mem;
    soc_field_info_t *field_info = NULL;
    uint16 key_base_off = 0;

    _FP_QUAL_DECL;

    /* validate input params */
    if (stage_fc == NULL) {
        return BCM_E_PARAM;
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
            (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
            "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }

    mem = BSK_AMFP_TCAMm;

    field_info = soc_mem_fieldinfo_get(unit, mem, KEYf);
    if (field_info == NULL) {
        return BCM_E_PARAM;
    }
    key_base_off = field_info->bp;

    /* Dummy Qualifiers */
    /* Stage */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
            _bcmFieldSliceSelDisable, 0, 0, 0);

    /* Stage Aggregate Egress Flowtracker */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageAggregateMmuFlowtracker,
            _bcmFieldSliceSelDisable, 0, 0, 0);

    /* Aggregate Class */
    field_info = soc_mem_fieldinfo_get(unit, mem, ACIDf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFlowtrackerAggregateClass,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, ECMP_GROUP_ID_LEVEL1f);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipathLevel1,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, ECMP_GROUP_ID_LEVEL2f);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMultipathLevel2,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, TRUNK_GROUP_IDf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, MMU_QUEUE_IDf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCosQueueId,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
#if 0
    field_info = soc_mem_fieldinfo_get(unit, mem, MMU_QUEUE_GROUP_IDf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, ,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
#endif
    field_info = soc_mem_fieldinfo_get(unit, mem, PACKET_TOSf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, CLASS_ID1f);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFlowtrackerClassIdSource1,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, CLASS_ID2f);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFlowtrackerClassIdSource2,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_fb6_amftstage_init
 *
 * Purpose:
 *    Initializes aggregate mmu flowtracker stage related
 *    information.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fc         - (IN) Field control structure.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_amftstage_init(
    int unit,
    _field_control_t *fc,
    _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;

    /* Input parameter Check */
    if ((fc == NULL) || (stage_fc == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for flowtracker stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_AMFTFP) {
        return (BCM_E_NONE);
    }

    /* Initializing supported Qset */
    sal_memset(&stage_fc->_field_supported_qset, 0, sizeof(bcm_field_qset_t));

    /* Qualifiers initialization */
    rv = _field_fb6_amft_qualifiers_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d) Error: _field_fb6_amft_qualifiers_init=%d"
           "\n"), unit, rv));
        return (rv);
    }

    return (rv);
}

/*
 * Function:
 *    _field_fb6_aeft_qualifiers_init
 *
 * Purpose:
 *    Initializes flowtracker stage qualifiers for
 *    aggregate egress flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_aeft_qualifiers_init(
    int unit,
    _field_stage_t *stage_fc)
{
    uint16 key_base_off = 0;
    soc_mem_t mem = INVALIDm;
    soc_field_info_t *field_info = NULL;

    _FP_QUAL_DECL;

    /* validate input params */
    if (stage_fc == NULL) {
        return BCM_E_PARAM;
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
            (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
            "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }

    mem = BSK_AEFP_TCAMm;

    field_info = soc_mem_fieldinfo_get(unit, mem, KEYf);
    if (field_info == NULL) {
        return BCM_E_PARAM;
    }
    key_base_off = field_info->bp;

    /* Dummy Qualifiers */
    /* Stage */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
            _bcmFieldSliceSelDisable, 0, 0, 0);

    /* Stage Aggregate Egress Flowtracker */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageAggregateEgressFlowtracker,
            _bcmFieldSliceSelDisable, 0, 0, 0);

    /* Aggregate Class */
    field_info = soc_mem_fieldinfo_get(unit, mem, ACIDf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFlowtrackerAggregateClass,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, CEf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCongestionExperienced,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, DGPPf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, EGR_PORTf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, INT_PRIf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, CNGf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, INT_CNf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, CONGESTION_MARKEDf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCongestionMarked,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, UNICAST_QUEUINGf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIsUcastCosQueue,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, MMU_COSf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEgressPortQueue,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, MMU_QUEUE_IDf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyCosQueueId,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, PACKET_TOSf);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, CLASS_ID1f);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFlowtrackerClassIdSource1,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, CLASS_ID2f);
    if (field_info != NULL) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFlowtrackerClassIdSource2,
                _bcmFieldSliceSelFpf1, 0, field_info->bp - key_base_off,
                field_info->len);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_fb6_aeftstage_init
 *
 * Purpose:
 *    Initializes aggregate egress flowtracker stage related
 *    information.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fc         - (IN) Field control structure.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_aeftstage_init(
    int unit,
    _field_control_t *fc,
    _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;

    /* Input parameter Check */
    if ((fc == NULL) || (stage_fc == NULL)) {
        return (BCM_E_PARAM);
    }

    /* Applicable only for Aggregate Egress flowtracker stage */
    if (stage_fc->stage_id != _BCM_FIELD_STAGE_AEFTFP) {
        return (BCM_E_NONE);
    }

    /* Initializing supported Qset */
    sal_memset(&stage_fc->_field_supported_qset, 0, sizeof(bcm_field_qset_t));

    /* Qualifiers initialization */
    rv = _field_fb6_aeft_qualifiers_init(unit, stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d) Error: _field_fb6_aeft_qualifiers_init=%d"
           "\n"), unit, rv));
        return (rv);
    }

    return (rv);
}

/*
 * Function:
 *    _bcm_field_fb6_aft_tcam_policy_mem_get
 *
 * Purpose:
 *    Get Tcam/Policy memory for Aggregate stages.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_id   - (IN) Stage number
 *    tcam_mem   - (OUT) TCAM table
 *    policy_mem - (OUT) Policy table
 *
 * Returns:
 *    BCM_E_PARAM       - Incorrect stage
 *    BCM_E_NONE        - Success.
 */
STATIC int
_bcm_field_fb6_aft_tcam_policy_mem_get(
    int unit,
    int stage_id,
    soc_mem_t *tcam_mem,
    soc_mem_t *policy_mem)
{
    soc_mem_t tcam = INVALIDm;
    soc_mem_t policy = INVALIDm;

    switch(stage_id) {
        case _BCM_FIELD_STAGE_AMFTFP:
            tcam = BSK_AMFP_TCAMm;
            policy = BSK_AMFP_POLICYm;
            break;
        case _BCM_FIELD_STAGE_AEFTFP:
            tcam = BSK_AEFP_TCAMm;
            policy = BSK_AEFP_POLICYm;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (tcam_mem) {
        *tcam_mem = tcam;
    }
    if (policy_mem) {
        *policy_mem = policy;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_fb6_aft_tcam_policy_init
 *
 * Purpose:
 *    Fill Tcam/Policy memories with default entries.
 *    Default entries are required to handle cases
 *    where packet has not HIT BSK_FTFP(2)_TCAM.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Wrong stage
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_aft_tcam_policy_init(
    int unit,
    _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;
    _field_slice_t *fs = NULL;
    soc_mem_t tcam_mem = INVALIDm;
    uint32 e_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    if (!soc_feature(unit,
            soc_feature_field_flowtracker_v3_support)) {
        return BCM_E_NONE;
    }

    /* Decrese number of entries by 1 */
    stage_fc->tcam_sz -= _BCM_FIELD_FB6_AFT_RESERVED_ENTRIES;

    fs = stage_fc->slices[0];
    /* Move tcam start index after reserved entries */
    fs->start_tcam_idx += _BCM_FIELD_FB6_AFT_RESERVED_ENTRIES;
    /* Reduce number of free entries */
    fs->free_count -= _BCM_FIELD_FB6_AFT_RESERVED_ENTRIES;
    /* Reduce number of entries in the slice */
    fs->entry_count -= _BCM_FIELD_FB6_AFT_RESERVED_ENTRIES;

    /* Install default entry */
    rv = _bcm_field_fb6_aft_tcam_policy_mem_get(unit,
                stage_fc->stage_id, &tcam_mem, NULL);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(e_buf, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

    soc_mem_field32_set(unit, tcam_mem, (void *) e_buf, ACIDf, 0);
    soc_mem_field32_set(unit, tcam_mem, (void *) e_buf, ACID_MASKf, 0x3f);
    soc_mem_field32_set(unit, tcam_mem, (void *) e_buf, VALIDf, 3);

    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, 0, e_buf);

    return rv;
}

/*
 * Function:
 *    _field_fb6_aft_entry_policy_set
 *
 * Purpose:
 *    Sets policy data information to buffer
 *    for aggregate entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    policy_mem - (IN) Policy hw table.
 *    e_buf      - (IN/OUT) Entry buffer.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_aft_entry_policy_set(
    int unit,
    _field_entry_t *f_ent,
    soc_mem_t policy_mem,
    uint32 *e_buf)
{
    int rv = BCM_E_NONE;
    uint32 value = 0;
    _field_action_t *fa = NULL;
    soc_field_t ft_group_fld = INVALIDf;
    soc_field_t ft_idx_fld = INVALIDf;
    soc_field_t ft_idx_valid_fld = INVALIDf;
    bcm_flowtracker_group_t aft_group_id = -1;
    uint32 action_aft_index_set = FALSE;
    uint32 action_aft_index = 0;

    if ((f_ent == NULL) || (policy_mem == INVALIDm) || (e_buf == NULL)) {
        return BCM_E_PARAM;
    }

    /* Loop through all actions */
    for (fa = f_ent->actions;
            (fa != NULL) && (fa->flags & _FP_ACTION_VALID);
            fa = fa->next) {
        switch(fa->action) {
            case bcmFieldActionFlowtrackerAggregateMmuGroupId:
                if (f_ent->group->stage_id != _BCM_FIELD_STAGE_AMFTFP) {
                    return (BCM_E_PARAM);
                }
                aft_group_id = fa->param[0];
                break;
            case bcmFieldActionFlowtrackerAggregateMmuFlowIndex:
                if (f_ent->group->stage_id != _BCM_FIELD_STAGE_AMFTFP) {
                    return (BCM_E_PARAM);
                }
                action_aft_index_set = TRUE;
                action_aft_index = fa->param[1];
                break;
            case bcmFieldActionFlowtrackerAggregateEgressGroupId:
                if (f_ent->group->stage_id != _BCM_FIELD_STAGE_AEFTFP) {
                    return (BCM_E_PARAM);
                }
                aft_group_id = fa->param[0];
                break;
            case bcmFieldActionFlowtrackerAggregateEgressFlowIndex:
                if (f_ent->group->stage_id != _BCM_FIELD_STAGE_AEFTFP) {
                    return (BCM_E_PARAM);
                }
                action_aft_index_set = TRUE;
                action_aft_index = fa->param[1];
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    }

    /* Get Fields */
    switch(f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_AMFTFP:
            ft_group_fld = AMGIDf;
            ft_idx_fld = AMIDf;
            ft_idx_valid_fld = AMID_VALIDf;
            break;
        case _BCM_FIELD_STAGE_AEFTFP:
            ft_group_fld = AEGIDf;
            ft_idx_fld = AEIDf;
            ft_idx_valid_fld = AEID_VALIDf;
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    /*
     * Both AFT Group and Index must be present to install
     * aggregate fields.
     */
    if (((aft_group_id != -1) && (action_aft_index_set == FALSE)) ||
            ((aft_group_id == -1) && (action_aft_index_set == TRUE))) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit, "FP(unit %d)"
                        " Error: Action for aggregate Group or FlowIndex"
                        " is missing\n"), unit));
        return BCM_E_CONFIG;
    }

    if (aft_group_id != -1) {
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, ft_group_fld)) {
            value = BCMI_FT_GROUP_INDEX_GET(aft_group_id);
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    ft_group_fld, &(value));
        }

        value = 0;
        if (action_aft_index_set == TRUE) {
            value = action_aft_index;
        }
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, ft_idx_fld)) {
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    ft_idx_fld, &(value));
        }
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, ft_idx_valid_fld)) {
            value = 1;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    ft_idx_valid_fld, &(value));
        }
    } else {
        /* Reset policy memory */
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, DATAf)) {
            value = 0;
            soc_mem_field_set(unit, policy_mem, (void *) e_buf,
                    DATAf, &(value));
        }
    }

    /* Reset DIRTY flag */
    for (fa = f_ent->actions;
            (fa != NULL) && (fa->flags & _FP_ACTION_VALID);
            fa = fa->next) {
        fa->flags &= (~_FP_ACTION_DIRTY);
    }

    return rv;
}

/*
 * Function:
 *    _field_fb6_aft_entry_policy_mem_install
 *
 * Purpose:
 *    Installs policy hw table for entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) field entry structure.
 *    policy_mem - (IN) policy hw table
 *    tcam_idx   - (IN) Index in policy table
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_aft_entry_policy_mem_install(
    int unit,
    _field_entry_t *f_ent,
    soc_mem_t policy_mem,
    int tcam_idx)
{
    int rv = BCM_E_NONE;
    uint32 e_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    if ((f_ent == NULL) || (policy_mem == INVALIDm) || (tcam_idx == -1)) {
        return BCM_E_PARAM;
    }

    /* Read policy memory */
    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, tcam_idx, e_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Set policy buffer with actions */
    rv = _field_fb6_aft_entry_policy_set(unit, f_ent, policy_mem, e_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Write to hardware */
    rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx, e_buf);

    return rv;
}

/*
 * Function:
 *    _field_fb6_aft_entry_qual_tcam_install
 *
 * Purpose:
 *    Installs tcam hw tables for entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) field entry structure.
 *    tcam_mem   - (IN) TCAM memory
 *    tcam_idx   - (IN) Index in tcam table
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Inconsistent state.
 *    BCM_E_NONE        - Success.
 */
int
_field_fb6_aft_entry_qual_tcam_install(
    int unit,
    _field_entry_t *f_ent,
    soc_mem_t tcam_mem,
    int tcam_idx)
{
    int rv = BCM_E_NONE;
    _field_tcam_t *tcam = NULL;
    uint32 e_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    if (f_ent == NULL) {
        return BCM_E_PARAM;
    }

    tcam = &f_ent->tcam;
    if (tcam == NULL) {
        return (BCM_E_INTERNAL);
    }

    soc_mem_field_set(unit, tcam_mem, e_buf, KEYf, tcam->key);
    soc_mem_field_set(unit, tcam_mem, e_buf, MASKf, tcam->mask);
    soc_mem_field32_set(unit, tcam_mem, e_buf, VALIDf, 3);

    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, e_buf);

    return rv;
}

/*
 * Function:
 *    _bcm_field_fb6_ft_user_entry_hw_alloc
 *
 * Purpose:
 *    Increment reference count for user entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    fa         - (IN) Field action structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb6_ft_user_entry_hw_alloc(
    int unit,
    _field_entry_t *f_ent,
    _field_action_t *fa)
{
    int rv = BCM_E_NONE;
    int flow_index;
    bcm_flowtracker_group_type_t group_type;

    /* Get Corresponding FT Group Type */
    if (f_ent->group->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        group_type = bcmFlowtrackerGroupTypeAggregateIngress;
    } else if (f_ent->group->stage_id == _BCM_FIELD_STAGE_AMFTFP) {
        group_type = bcmFlowtrackerGroupTypeAggregateMmu;
    } else {
        group_type = bcmFlowtrackerGroupTypeAggregateEgress;
    }

    fa->old_index = fa->hw_index;
    flow_index = (int) fa->param[1];

    rv = bcmi_esw_ftv3_user_entry_ref_update(unit,
            group_type, flow_index, 1);
    if (BCM_SUCCESS(rv)) {
        fa->hw_index = flow_index;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_fb6_ft_user_entry_hw_free
 *
 * Purpose:
 *    Decrement the reference count for user
 *    entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    fa         - (IN) Field action structure.
 *    flags      - (IN) Flags
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb6_ft_user_entry_hw_free(
    int unit,
    _field_entry_t *f_ent,
    _field_action_t *fa,
    uint32 flags)
{
    int rv = BCM_E_NONE;
    int flow_index;
    bcm_flowtracker_group_type_t group_type;

    if (!soc_feature(unit, soc_feature_field_flowtracker_v3_support)) {
        return BCM_E_UNAVAIL;
    }

    /* Get Corresponding FT Group Type */
    if (f_ent->group->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        group_type = bcmFlowtrackerGroupTypeAggregateIngress;
    } else if (f_ent->group->stage_id == _BCM_FIELD_STAGE_AMFTFP) {
        group_type = bcmFlowtrackerGroupTypeAggregateMmu;
    } else {
        group_type = bcmFlowtrackerGroupTypeAggregateEgress;
    }

    if ((flags & _FP_ACTION_OLD_RESOURCE_FREE) &&
            (fa->old_index != _FP_INVALID_INDEX)) {
        flow_index = fa->old_index;

        rv = bcmi_esw_ftv3_user_entry_ref_update(unit,
                group_type, flow_index, -1);
    }
    BCM_IF_ERROR_RETURN(rv);

    if ((flags & _FP_ACTION_RESOURCE_FREE) &&
            (fa->hw_index != _FP_INVALID_INDEX)) {
        flow_index = fa->hw_index;
        rv = bcmi_esw_ftv3_user_entry_ref_update(unit,
                group_type, flow_index, -1);
        if (!(flags & _BCM_FIELD_ACTION_REF_STALE)) {
            fa->hw_index = _FP_INVALID_INDEX;
        }
    }
    if (flags & _BCM_FIELD_ACTION_REF_STALE) {
        fa->flags |= _BCM_FIELD_ACTION_REF_STALE;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_aggregate_class_hw_alloc
 *
 * Purpose:
 *    Increment reference count for aggregate class.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    fa         - (IN) Field action structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb6_ft_aggregate_class_hw_alloc(
    int unit,
    _field_entry_t *f_ent,
    _field_action_t *fa)
{
    int rv = BCM_E_NONE;
    uint32 aggregate_class;

    fa->old_index = fa->hw_index;
    aggregate_class = fa->param[0];

    rv = bcmi_esw_ftv3_aggregate_class_ref_update(unit,
            aggregate_class, 1);
    if (BCM_SUCCESS(rv)) {
        fa->hw_index = (int) aggregate_class;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_fb6_ft_aggregate_class_hw_free
 *
 * Purpose:
 *    Decrement the reference count for aggregate
 *    class.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_ent      - (IN) Field entry structure.
 *    fa         - (IN) Field action structure.
 *    flags      - (IN) Flags
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb6_ft_aggregate_class_hw_free(
    int unit,
    _field_entry_t *f_ent,
    _field_action_t *fa,
    uint32 flags)
{
    int rv = BCM_E_NONE;
    uint32 aggregate_class;

    if ((flags & _FP_ACTION_OLD_RESOURCE_FREE) &&
            (fa->old_index != _FP_INVALID_INDEX)) {
        aggregate_class = (uint32 )fa->old_index;

        rv = bcmi_esw_ftv3_aggregate_class_ref_update(unit,
                    aggregate_class, -1);
    }
    BCM_IF_ERROR_RETURN(rv);

    if ((flags & _FP_ACTION_RESOURCE_FREE) &&
            (fa->hw_index != _FP_INVALID_INDEX)) {
        aggregate_class = (uint32) fa->hw_index;
        rv = bcmi_esw_ftv3_aggregate_class_ref_update(unit,
                    aggregate_class, -1);
        if (!(flags & _BCM_FIELD_ACTION_REF_STALE)) {
            fa->hw_index = _FP_INVALID_INDEX;
        }
    }
    if (flags & _BCM_FIELD_ACTION_REF_STALE) {
        fa->flags |= _BCM_FIELD_ACTION_REF_STALE;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_field_fb6_entry_hw_to_sw_tcam_idx
 * Purpose:
 *     Convert hw index to sw index for tcam.
 * Parameters:
 *  unit       - (IN)   BCM device number.
 *  fs         - (IN)   Field Slice Struct
 *  hw_tcam_idx- (IN)   Hardware tcam index
 *  sw_tcam_idx- (OUT)  Software tcam index
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_fb6_entry_hw_to_sw_tcam_idx(
    int unit,
    _field_slice_t *fs,
    int hw_tcam_idx,
    int *sw_tcam_idx)
{
    int offset = 0;

    if (_BCM_FIELD_STAGE_FLOWTRACKER != fs->stage_id) {
        *sw_tcam_idx = hw_tcam_idx;
        return BCM_E_NONE;
    }

    /* If entries in slice are reserved, offset it */
    if (soc_feature(unit, soc_feature_bscan_fae_port_flow_exclude)) {
        offset = _BCM_FIELD_HX5_INGRESS_FT_RESERVED_ENTRIES;
    }


    if (fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
        *sw_tcam_idx = fs->start_tcam_idx + hw_tcam_idx - offset;
    } else {
        *sw_tcam_idx = hw_tcam_idx;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_fb6_entry_sw_to_hw_tcam_idx
 * Purpose:
 *     Convert sw index to hw index for tcam.
 * Parameters:
 *  unit       - (IN)   BCM device number.
 *  fs         - (IN)   Field Slice Struct
 *  sw_tcam_idx- (IN)   Software tcam index
 *  hw_tcam_idx- (OUT)  Hardware tcam index
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  In firebolt6, tcam/policy in flowtracker stage
 *  are independent unlike ifp stage. So, a conversion
 *  from logical tcam idx to hardware tcam idx is
 *  required.
 */
int
_bcm_field_fb6_entry_sw_to_hw_tcam_idx(
    int unit,
    _field_slice_t *fs,
    int sw_tcam_idx,
    int *hw_tcam_idx)
{
    int offset = 0;

    if (_BCM_FIELD_STAGE_FLOWTRACKER != fs->stage_id) {
        *hw_tcam_idx = sw_tcam_idx;
        return BCM_E_NONE;
    }

    /* Entries in slice are reserved, offset it */
    if (soc_feature(unit, soc_feature_bscan_fae_port_flow_exclude)) {
        offset = _BCM_FIELD_HX5_INGRESS_FT_RESERVED_ENTRIES;
    }

    if (fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
        *hw_tcam_idx = sw_tcam_idx - fs->start_tcam_idx + offset;
    } else {
        *hw_tcam_idx = sw_tcam_idx;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_fb6_qualify_cos_queueid
 * Purpose:
 *     Qualify on COs Queue Id.
 * Parameters:
 *  unit       - (IN)   BCM device number.
 *  entry      - (IN)   Field Entry
 *  queue_gport- (IN)   COs Queue Gport Number
 * Returns:
 *  BCM_E_XXX
 * Notes:
 */
int
_bcm_field_fb6_qualify_cos_queueid(int unit,
    bcm_field_entry_t entry,
    bcm_gport_t queue_gport)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    int qnum;
    _field_entry_t *f_ent = NULL;

    if (soc_feature(unit, soc_feature_field_flowtracker_v3_support)) {

        rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (f_ent->group->stage_id == _BCM_FIELD_STAGE_AMFTFP) {
            if (BCM_GPORT_IS_SET(queue_gport) &&
                BCM_GPORT_IS_UCAST_QUEUE_GROUP(queue_gport)) {
                rv = _bcm_fb6_cosq_gport_hw_qnum_get(unit,
                        queue_gport, &qnum);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            if (BCM_GPORT_IS_SET(queue_gport) &&
                (BCM_GPORT_IS_UCAST_QUEUE_GROUP(queue_gport) ||
                 BCM_GPORT_IS_MCAST_QUEUE_GROUP(queue_gport))) {
                rv = _bcm_fb6_cosq_gport_hw_qnum_get(unit,
                        queue_gport, &qnum);
            } else {
                return BCM_E_PARAM;
            }
        }

        rv = _field_qualify32(unit, entry,
                bcmFieldQualifyCosQueueId, (uint32) qnum, -1);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     _bcm_field_fb6_qualify_cos_queueid_get
 * Purpose:
 *     To Get Value of qualified COs Queue Id from entry.
 * Parameters:
 *  unit       - (IN)   BCM device number.
 *  entry      - (IN)   Field Entry
 *  queue_gport- (OUT)   COs Queue Gport Number
 * Returns:
 *  BCM_E_XXX
 * Notes:
 */
int
_bcm_field_fb6_qualify_cos_queueid_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_gport_t *queue_gport)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    uint32 data, mask;

    if (soc_feature(unit, soc_feature_field_flowtracker_v3_support)) {

        rv = _bcm_field_entry_qualifier_uint32_get(unit, entry,
                bcmFieldQualifyCosQueueId, &data, &mask);
        BCM_IF_ERROR_RETURN(rv);
        COMPILER_REFERENCE(mask);

        rv = _bcm_fb6_cosq_hw_qnum_gport_get(unit, (int) data, queue_gport);
    }
#endif

    return rv;
}

/*
 * Function:
 *     _field_fb6_aftfp_group_aset_set
 * Purpose:
 *     Set Aset to the group for FB6 Aggreagate FTFP stages.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_fb6_aftfp_group_aset_set(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    int a_idx;                          /* Action Iterator.      */
    uint16 aset_size =0;                /* Action Set Size.      */
    _field_group_t *fg = NULL;          /* Group Struct          */

    /* Get Group */
    fg = fsm_ptr->fg;

    if ((soc_feature(unit, soc_feature_field_flowtracker_v3_support)) &&
            (_BCM_FIELD_STAGE_AMFTFP == fg->stage_id)) {
        /* Verify actions */
        for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
            if (BCM_FIELD_ASET_TEST(fg->aset, a_idx)) {
                switch(a_idx) {
                    case bcmFieldActionFlowtrackerAggregateMmuGroupId:
                    case bcmFieldActionFlowtrackerAggregateMmuFlowIndex:
                        aset_size++;
                        break;
                    default:
                        fsm_ptr->rv = (BCM_E_UNAVAIL);
                        return fsm_ptr->rv;
                }
            }
        }
        if (aset_size == 0) {
            /* Update the group's Aset with all supported actions */
            BCM_IF_ERROR_RETURN(_field_group_default_aset_set(unit, fg));
        }
    }
    if ((soc_feature(unit, soc_feature_field_flowtracker_v3_support)) &&
            (_BCM_FIELD_STAGE_AEFTFP == fg->stage_id)) {
        /* Verify actions */
        for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
            if (BCM_FIELD_ASET_TEST(fg->aset, a_idx)) {
                switch(a_idx) {
                    case bcmFieldActionFlowtrackerAggregateEgressGroupId:
                    case bcmFieldActionFlowtrackerAggregateEgressFlowIndex:
                        aset_size++;
                        break;
                    default:
                        fsm_ptr->rv = (BCM_E_UNAVAIL);
                        return fsm_ptr->rv;
                }
            }
        }
        if (aset_size == 0) {
            /* Update the group's Aset with all supported actions */
            BCM_IF_ERROR_RETURN(_field_group_default_aset_set(unit, fg));
        }
    }

  return BCM_E_NONE;
}

STATIC _bcm_field_trie_info_t
_bcm_field_fb6_trie_info[SOC_MAX_NUM_DEVICES][_BCM_FIELD_COMPRESS_TYPE_MAX];

STATIC _bcm_field_compress_cid_list_t
*_bcm_field_fb6_compress_cid_list[SOC_MAX_NUM_DEVICES][_BCM_FIELD_COMPRESS_TYPE_MAX];

STATIC int
_bcm_field_fb6_compress_trie_type_get(int unit, _field_entry_t *f_ent,
                                      bcm_field_qualify_t qual, int *trie_type)
{
    int i, j;

    for (i = 0; i < _BCM_FIELD_COMPRESS_TYPE_MAX; i++) {
        if (BCM_FIELD_QSET_TEST(f_ent->group->qset, _bcm_field_fb6_trie_info[unit][i].iqual)) {
            for (j = 0; j < _bcm_field_fb6_trie_info[unit][i].num_qual; j++) {
                if (qual == _bcm_field_fb6_trie_info[unit][i].qual[j]) {
                    *trie_type = i;
                    return BCM_E_NONE;
                }
            }
        }
    }
    return BCM_E_PARAM;
}

/* Update FP entry to compress key mapping for insert operation */
STATIC int
_bcm_field_fb6_compress_basic_info_key_insert(int unit, bcm_field_entry_t entry,
                                              int trie_type,
                                              _bcm_field_cid_t *cid)
{
    _bcm_field_compress_cid_list_t *cid_list = NULL, *cid_list_prev = NULL;
    bcm_field_entry_t *list_entry = NULL;
    int found = 0, i;

    cid_list = _bcm_field_fb6_compress_cid_list[unit][trie_type];
    while (cid_list) {
        if (0 == sal_memcmp(cid_list->cid.w, cid, sizeof(_bcm_field_cid_t))) {
            found = 1;
            break;
        }
        cid_list_prev = cid_list;
        cid_list = cid_list->next;
    }
    if (found) {
        /* Cid exists */
        for (i = 0; i < cid_list->num_entries; i++) {
            if (cid_list->entries[i] == entry) {
                /* Same entry already exists */
                return BCM_E_NONE;
            }
        }

        list_entry = sal_alloc(sizeof(bcm_field_entry_t) *
                               (cid_list->num_entries + 1), "CidList Entry");
        if (list_entry == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memcpy(list_entry, cid_list->entries,
                   sizeof(bcm_field_entry_t) * cid_list->num_entries);
        sal_free(cid_list->entries);
        cid_list->entries = list_entry;
        cid_list->entries[cid_list->num_entries] = entry;
        cid_list->num_entries += 1;
    } else {
        /* New cid */
        cid_list = sal_alloc(sizeof(_bcm_field_compress_cid_list_t), "CidList Node");
        if (cid_list == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(cid_list, 0, sizeof(_bcm_field_compress_cid_list_t));

        cid_list->entries = sal_alloc(sizeof(bcm_field_entry_t), "CidList Entry");
        if (cid_list->entries == NULL) {
            sal_free(cid_list);
            return BCM_E_MEMORY;
        }

        cid_list->entries[0] = entry;
        cid_list->num_entries += 1;
        sal_memcpy(cid_list->cid.w, cid, sizeof(_bcm_field_cid_t));
        if (cid_list_prev) {
            cid_list_prev->next = cid_list;
        } else {
            _bcm_field_fb6_compress_cid_list[unit][trie_type] = cid_list;
        }
    }

    return BCM_E_NONE;
}

/* Update FP entry to compress key mapping for delete operation */
STATIC int
_bcm_field_fb6_compress_basic_info_key_delete(int unit, bcm_field_entry_t entry,
                                              int trie_type,
                                              _bcm_field_cid_t *cid)
{
    _bcm_field_compress_cid_list_t *cid_list = NULL, *cid_list_prev = NULL;
    bcm_field_entry_t *list_entry = NULL;
    int found = 0, i;

    cid_list = _bcm_field_fb6_compress_cid_list[unit][trie_type];
    while (cid_list) {
        if (0 == sal_memcmp(cid_list->cid.w, cid, sizeof(_bcm_field_cid_t))) {
            found = 1;
            break;
        }
        cid_list_prev = cid_list;
        cid_list = cid_list->next;
    }
    if (found) {
        /* Cid exists */
        found = 0;
        for (i = 0; i < cid_list->num_entries; i++) {
            if (cid_list->entries[i] == entry) {
                found = 1;
                break;
            }
        }
        if (found) {
            /* Entry found */
            cid_list->num_entries -= 1;
            if (cid_list->num_entries == 0) {
                /* Cid not in use by any entry */
                if (cid_list_prev) {
                    cid_list_prev->next = cid_list->next;
                } else {
                    _bcm_field_fb6_compress_cid_list[unit][trie_type] = cid_list->next;
                }
                sal_free(cid_list->entries);
                cid_list->entries = NULL;
                sal_free(cid_list);
                cid_list = NULL;
                return BCM_E_NONE;
            }
            list_entry = sal_alloc(sizeof(bcm_field_entry_t) *
                                   (cid_list->num_entries), "CidList Entry");
            if (list_entry == NULL) {
                return BCM_E_MEMORY;
            }
            cid_list->entries[i] = cid_list->entries[cid_list->num_entries];
            sal_memcpy(list_entry, cid_list->entries, sizeof(bcm_field_entry_t) *
                       (cid_list->num_entries));
            sal_free(cid_list->entries);
            cid_list->entries = list_entry;
        } else {
            /* Entry not found */
            return BCM_E_NOT_FOUND;
        }
    } else {
        /* Key not found */
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

#ifdef INCLUDE_L3
STATIC void
_bcm_field_fb6_compress_lpm_cfg_init(int unit, int trie_type,
                                     _bcm_field_basic_info_key_t *key_info,
                                     _bcm_defip_cfg_t *lpm_cfg)
{
    sal_memset(lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
    if (trie_type == _BCM_FIELD_COMPRESS_TYPE_SRC_IPV4) {
        lpm_cfg->defip_inter_flags = BCM_L3_INTER_FLAGS_PRE_FILTER |
                                    BCM_L3_INTER_FLAGS_PRE_FILTER_SRC;
        sal_memcpy(&lpm_cfg->defip_ip_addr, key_info->key.w, sizeof(bcm_ip_t));
    } else if (trie_type == _BCM_FIELD_COMPRESS_TYPE_DST_IPV4) {
        lpm_cfg->defip_inter_flags = BCM_L3_INTER_FLAGS_PRE_FILTER;
        sal_memcpy(&lpm_cfg->defip_ip_addr, key_info->key.w, sizeof(bcm_ip_t));
    } else if (trie_type == _BCM_FIELD_COMPRESS_TYPE_SRC_IPV6) {
        lpm_cfg->defip_inter_flags = BCM_L3_INTER_FLAGS_PRE_FILTER |
                                    BCM_L3_INTER_FLAGS_PRE_FILTER_SRC;
        lpm_cfg->defip_flags = BCM_L3_IP6;
        SAL_IP6_ADDR_FROM_UINT32(lpm_cfg->defip_ip6_addr, key_info->key.w);
    } else {
        lpm_cfg->defip_inter_flags = BCM_L3_INTER_FLAGS_PRE_FILTER;
        lpm_cfg->defip_flags = BCM_L3_IP6;
        SAL_IP6_ADDR_FROM_UINT32(lpm_cfg->defip_ip6_addr, key_info->key.w);
    }
    lpm_cfg->defip_sub_len = key_info->prefix;
    lpm_cfg->defip_acl_class_id = key_info->cid.w[0];
    lpm_cfg->defip_vrf = BCM_L3_VRF_GLOBAL;
}
#endif

STATIC int
_bcm_field_fb6_compress_alpm_insert(int unit, _field_stage_t *stage_fc, int trie_type,
                                    _bcm_field_basic_info_key_t *key_info)
{
#ifdef INCLUDE_L3
    _bcm_defip_cfg_t lpm_cfg;

    _bcm_field_fb6_compress_lpm_cfg_init(unit, trie_type, key_info, &lpm_cfg);

    /* Sync ref_count and verify key exists */
    BCM_IF_ERROR_RETURN
        (_bcm_field_compress_key_lookup(unit, key_info,
                                        stage_fc->compress_tries[trie_type]));

    if (key_info->ref_count == 1) {
        BCM_IF_ERROR_RETURN(bcm_xgs3_defip_add(unit, &lpm_cfg));
    }
#endif
    return BCM_E_NONE;
}

STATIC int
_bcm_field_fb6_compress_alpm_delete(int unit, _field_stage_t *stage_fc, int trie_type,
                                    _bcm_field_basic_info_key_t *key_info)
{
#ifdef INCLUDE_L3
    _bcm_defip_cfg_t lpm_cfg;

    _bcm_field_fb6_compress_lpm_cfg_init(unit, trie_type, key_info, &lpm_cfg);

    /* Sync ref_count and verify key exists */
    BCM_IF_ERROR_RETURN
        (_bcm_field_compress_key_lookup(unit, key_info,
                                        stage_fc->compress_tries[trie_type]));

    if (key_info->ref_count == 1) {
        /* Delete from ALPM table only if the key is not used by other rules */
        BCM_IF_ERROR_RETURN(bcm_xgs3_defip_del(unit, &lpm_cfg));
    }
#endif
    return BCM_E_NONE;
}

STATIC int
_bcm_field_fb6_compress_alpm_update(int unit, int trie_type,
                                    _bcm_field_basic_info_key_t *key_info)
{
#ifdef INCLUDE_L3
    _bcm_defip_cfg_t lpm_cfg;

    _bcm_field_fb6_compress_lpm_cfg_init(unit, trie_type, key_info, &lpm_cfg);

    lpm_cfg.defip_flags |= BCM_L3_REPLACE;
    BCM_IF_ERROR_RETURN(bcm_xgs3_defip_add(unit, &lpm_cfg));
#endif
    return BCM_E_NONE;
}

/* Dynamically assign slice index to list compressed entries moving to trie
 * which need to be installed in FP TCAM
 */
STATIC int
_bcm_field_fb6_compress_entry_slice_assign(int unit, _field_entry_t *f_ent)
{
    _field_group_t *fg;
    _field_slice_t *fs = NULL;
    _field_stage_t *stage_fc;
    int parts_count, idx, pri_tcam_idx = -1, part_index, slice_number = 0;

    fg = f_ent->group;

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id, &stage_fc));

    /* Reserve a slice with room for new entry. */
    BCM_IF_ERROR_RETURN(_field_group_slice_entry_reserve(unit, fg, &fs));

    BCM_IF_ERROR_RETURN
        (_bcm_field_entry_tcam_parts_count(unit, fg->stage_id, fg->flags, &parts_count));

    BCM_IF_ERROR_RETURN
        (_bcm_field_slice_idx_allocate(unit, stage_fc, fg, fs, &(f_ent->slice_idx)));

    BCM_IF_ERROR_RETURN
        (_bcm_field_slice_offset_to_tcam_idx(unit, stage_fc, fg->instance,
                                             fs->slice_number, f_ent->slice_idx,
                                             &pri_tcam_idx));
    for (idx = 0; idx < parts_count; idx++) {
        BCM_IF_ERROR_RETURN
            (_bcm_field_entry_part_tcam_idx_get(unit, f_ent, pri_tcam_idx,
                                                idx, &part_index));
        BCM_IF_ERROR_RETURN
            (_bcm_field_th_tcam_idx_to_slice_offset(unit, stage_fc, f_ent,
                                                    part_index, &slice_number,
                                                    (int *)&f_ent[idx].slice_idx));

        f_ent[idx].fs = stage_fc->slices[fg->instance] + slice_number;

        /* Decrement slice free entry count. */
        if ((0 == (f_ent[idx].flags & _FP_ENTRY_SECOND_HALF)) ||
            (f_ent[idx].flags & _FP_ENTRY_MODIFY_SLICE_SW_ENTRY_COUNT)) {
            f_ent[idx].fs->free_count--;
        }

        /* Assign entry to a slice */
        f_ent[idx].fs->entries[f_ent[idx].slice_idx] = f_ent + idx;

        /* Reset the priority */
        f_ent[idx].prio = BCM_FIELD_ENTRY_PRIO_LOWEST;
        f_ent[idx].flags &= ~_FP_ENTRY_CREATED_WITHOUT_PRIO;
    }
    /* Increment the HW Slice entry count */
    f_ent->fs->hw_ent_count++;

    return BCM_E_NONE;
}

/* Callback function which actually programs the cid in FP TCAM */
STATIC int
_bcm_field_fb6_compress_entry_update(int unit,  _bcm_field_cid_update_list_t *list,
                                     void *user_data)
{
    _bcm_field_compress_cid_list_t *cid_list;
    _bcm_field_cid_update_list_t *list_iter;
    _bcm_field_cid_t cid;
    _field_entry_t *f_ent;
    int *trie_type, i, flag ;

    trie_type = (int *)user_data;

    sal_memset(&cid, 0, sizeof(_bcm_field_cid_t));
    list_iter = list;
    while (list_iter) {
        if (0 == sal_memcmp(&cid, list_iter->old_cid.w, sizeof(_bcm_field_cid_t))) {
            list_iter = list_iter->next;
            continue;
        }
        cid_list = _bcm_field_fb6_compress_cid_list[unit][*trie_type];

        while (cid_list) {
            if (0 == sal_memcmp(cid_list->cid.w, list_iter->old_cid.w, sizeof(_bcm_field_cid_t))) {
                /* Found matching cid in cid_list */
                flag = 1;
                for (i = 0; flag && (i < cid_list->num_entries); i++) {
                    BCM_IF_ERROR_RETURN
                        (_field_entry_get(unit, cid_list->entries[i], _FP_ENTRY_PRIMARY, &f_ent));
                    if ((list_iter->prefix == f_ent->key_info[*trie_type].prefix) &&
                        SHR_BITEQ_RANGE(f_ent->key_info[*trie_type].key.w, list_iter->key.w,
                                        _bcm_field_fb6_trie_info[unit][*trie_type].key_width - list_iter->prefix,
                                        list_iter->prefix)) {
                        /* Found the entry with matching key */
                        if (list_iter->flags & _BCM_FIELD_TRIE_NODE_WIDTH_COMPRESSED) {
                            /* List compressed node moved to trie */
                            f_ent->key_info[*trie_type].flags &= ~_BCM_FIELD_TRIE_NODE_LIST_COMPRESSED;
                        }

                        if (cid_list->num_entries == 1) {
                            /* Last entry. cid_list will be freed */
                            flag = 0;
                        }
                        /* Delete old cid mappings */
                        BCM_IF_ERROR_RETURN
                            (_bcm_field_fb6_compress_basic_info_key_delete(unit, f_ent->eid, *trie_type,
                                                                           &f_ent->key_info[*trie_type].cid));

                        /* Insert new mappings only after all old mappings are deleted
                         * to prevent double updates since cid assignments can be shuffled.
                         * Use dummy cid 0 to keep track of these entries
                         */

                        BCM_IF_ERROR_RETURN
                            (_bcm_field_fb6_compress_basic_info_key_insert
                                (unit, f_ent->eid, *trie_type, &cid));

                        /* Update the cid */
                        sal_memcpy(f_ent->key_info[*trie_type].cid.w,
                                   list_iter->new_cid.w, sizeof(_bcm_field_cid_t));
                        sal_memcpy(f_ent->key_info[*trie_type].cid_mask.w,
                                   list_iter->new_cid_mask.w, sizeof(_bcm_field_cid_t));
                    }
                }
                break;
            }
            cid_list = cid_list->next;
        }
        list_iter = list_iter->next;
    }

    cid_list = _bcm_field_fb6_compress_cid_list[unit][*trie_type];
    /* Now update mappings for all entries for which cid has changed */
    while (cid_list) {
        if (0 == sal_memcmp(cid_list->cid.w, &cid, sizeof(_bcm_field_cid_t))) {
            for (i = cid_list->num_entries - 1; i >= 0 ; i--) {
                BCM_IF_ERROR_RETURN
                    (_field_entry_get(unit, cid_list->entries[i], _FP_ENTRY_PRIMARY, &f_ent));
                if (f_ent->fs == NULL) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_field_fb6_compress_entry_slice_assign(unit, f_ent));
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_field_th_qualify_set(unit, f_ent->eid,
                                               _bcm_field_fb6_trie_info[unit][*trie_type].iqual,
                                               f_ent->key_info[*trie_type].cid.w, f_ent->key_info[*trie_type].cid_mask.w,
                                               _FP_QUALIFIER_ADD));
                if (f_ent->flags & _FP_ENTRY_INSTALLED) {
                    /* Write TCAM and Policy table for this entry.*/
                    BCM_IF_ERROR_RETURN(_field_tcam_policy_install(unit, f_ent->eid));
                    BCM_IF_ERROR_RETURN
                        (_bcm_field_fb6_compress_alpm_update(unit, *trie_type,
                                                             &f_ent->key_info[*trie_type]));
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_field_fb6_compress_basic_info_key_insert(unit, cid_list->entries[i],
                                                                   *trie_type, &f_ent->key_info[*trie_type].cid));
                BCM_IF_ERROR_RETURN
                    (_bcm_field_fb6_compress_basic_info_key_delete(unit, cid_list->entries[i],
                                                                   *trie_type, &cid));
            }
            break;
        }
        cid_list = cid_list->next;
    }

    return BCM_E_NONE;
}

STATIC void
_bcm_field_fb6_compress_entry_list_compressable(int unit, _field_entry_t *f_ent,
                                                _field_entry_t **f_ent_lc)
{
    int idx, action, qual, trie_type, rv0, rv1, action_dirty, dirty, found = 0;
    uint32 p0, p1, p2, p3;
    _bcm_field_qual_data_t data0, data1, mask0, mask1;
    _field_group_t *fg;
    _field_entry_t *f_ent_iter;

    fg = f_ent->group;

    /* Iterate on all entries in group. */
    for (idx = 0; idx < fg->group_status.entry_count; idx++) {
        f_ent_iter = fg->entry_arr[idx];
        if (f_ent_iter->eid == f_ent->eid) {
            continue;
        }
        if (!(f_ent_iter->flags & _FP_ENTRY_INSTALLED)) {
            continue;
        }
        if (!(f_ent_iter->flags & _FP_ENTRY_ENABLED)) {
            continue;
        }
        action_dirty = 0;
        if (BCM_FAILURE(_field_entry_action_dirty(unit, f_ent_iter, &action_dirty))) {
            continue;
        }
        dirty = 0;
        if (BCM_FAILURE(_field_entry_dirty(unit, f_ent_iter, &dirty))) {
            continue;
        }
        if (dirty || action_dirty) {
            continue;
        }
        if (f_ent_iter->prio != f_ent->prio) {
            /* Priority not matching */
            continue;
        }

        found = 1;
        for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
            /* Check if same compression qualifiers are present */
            if ((f_ent->new_key_info[trie_type].ref_count > 0 &&
                 f_ent_iter->key_info[trie_type].ref_count == 0) ||
                (f_ent->new_key_info[trie_type].ref_count == 0 &&
                 f_ent_iter->key_info[trie_type].ref_count > 0)) {
                found = 0;
                break;
            }
        }

        if (found == 0) {
            continue;
        }

        found = 0;
        /* Check if same actions are present and matching */
        for (action = 0; action < bcmFieldActionCount; action++) {
            if (!SHR_BITGET(fg->aset.w, action)) {
                continue;
            }
            p0 = p1 = p2 = p3 = 0;
            rv0 = bcm_esw_field_action_get(unit, f_ent->eid, action, &p0, &p1);
            rv1 = bcm_esw_field_action_get(unit, f_ent_iter->eid, action, &p2, &p3);
            if (rv0 == BCM_E_NOT_FOUND && rv1 == BCM_E_NOT_FOUND) {
                /* Action not present for both entries */
                continue;
            }
            if ((rv0 == BCM_E_NOT_FOUND && rv1 == BCM_E_NONE) ||
                (rv1 == BCM_E_NOT_FOUND && rv0 == BCM_E_NONE)) {
                /* Action present only in one entry */
                break;
            }
            if ((p0 == p2 && p1 == p3 && rv0 == BCM_E_NONE && rv1 == BCM_E_NONE)) {
                found = 1;
            } else {
                /* Action parameters not matching */
                found = 0;
                break;
            }
        }
        if (action != bcmFieldActionCount || !found) {
            continue;
        }

        /* Check if same non-compressed qualifiers are present and matching */
        for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
            if (!SHR_BITGET(fg->qset.w, qual)) {
                continue;
            }
             _FP_QUAL_DATA_CLEAR(data0);
             _FP_QUAL_DATA_CLEAR(data1);
             _FP_QUAL_DATA_CLEAR(mask0);
             _FP_QUAL_DATA_CLEAR(mask1);
            (void)_bcm_field_th_entry_qualifier_key_get(unit, f_ent->eid, qual,
                                                        data0, mask0);
            (void)_bcm_field_th_entry_qualifier_key_get(unit, f_ent_iter->eid, qual,
                                                        data1, mask1);
            rv0 = sal_memcmp(data0, data1, sizeof(_bcm_field_qual_data_t));
            rv1 = sal_memcmp(mask0, mask1, sizeof(_bcm_field_qual_data_t));
            if (rv0 == 0 && rv1 == 0) {
                found = 1;
            } else {
                /* Qualifier values don't match */
                found = 0;
                break;
            }
        }
        if (qual != bcmFieldQualifyCount || !found) {
            continue;
        } else {
            break;
        }
    }
    if (found) {
        *f_ent_lc = f_ent_iter;
    }
}

int
_bcm_field_fb6_compress_entry_install(int unit,  _field_entry_t *f_ent)
{
    _field_stage_t *stage_fc;
    _field_entry_t *f_ent_lc = NULL;
    int trie_type, reuse_tcam_entry = 1;

    if (f_ent->eid == _FP_INTERNAL_RESERVED_ID) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, f_ent->group->stage_id,
                                                 &stage_fc));

    _bcm_field_fb6_compress_entry_list_compressable(unit, f_ent, &f_ent_lc);

    for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
        if ((f_ent->key_info[trie_type].ref_count)) {
            /* Destroy existing key before installing new one */
            BCM_IF_ERROR_RETURN
                (_bcm_field_fb6_compress_alpm_delete(unit, stage_fc, trie_type,
                                                     &f_ent->key_info[trie_type]));
            BCM_IF_ERROR_RETURN
                (_bcm_field_fb6_compress_basic_info_key_delete(unit, f_ent->eid, trie_type,
                                                               &f_ent->key_info[trie_type].cid));
            BCM_IF_ERROR_RETURN
                (_bcm_field_compress_key_delete(unit, &f_ent->key_info[trie_type],
                                                stage_fc->compress_tries[trie_type],
                                                &trie_type));
        }

        if (f_ent->new_key_info[trie_type].ref_count) {
            if (f_ent_lc) {
                f_ent->new_key_info[trie_type].flags |= _BCM_FIELD_TRIE_NODE_LIST_COMPRESSED;
                sal_memcpy(f_ent->new_key_info[trie_type].cid.w,
                           f_ent_lc->key_info[trie_type].cid.w,
                           sizeof(_bcm_field_cid_t));
                sal_memcpy(f_ent->new_key_info[trie_type].cid_mask.w,
                           f_ent_lc->key_info[trie_type].cid_mask.w,
                           sizeof(_bcm_field_cid_t));
            } else {
                f_ent->new_key_info[trie_type].flags = 0;
                sal_memset(f_ent->new_key_info[trie_type].cid.w, 0,
                           sizeof(_bcm_field_cid_t));
                sal_memset(f_ent->new_key_info[trie_type].cid_mask.w, 0,
                           sizeof(_bcm_field_cid_t));
            }
            BCM_IF_ERROR_RETURN
                (_bcm_field_compress_key_insert(unit, &f_ent->new_key_info[trie_type],
                                                stage_fc->compress_tries[trie_type],
                                                &trie_type));

            BCM_IF_ERROR_RETURN
                (_bcm_field_fb6_compress_basic_info_key_insert(unit, f_ent->eid, trie_type,
                                                               &f_ent->new_key_info[trie_type].cid));

            BCM_IF_ERROR_RETURN
                (_bcm_field_fb6_compress_alpm_insert(unit, stage_fc, trie_type,
                                                     &f_ent->new_key_info[trie_type]));

            /* Update the entry */
            BCM_IF_ERROR_RETURN
                (_bcm_field_th_qualify_set(unit, f_ent->eid,
                                           _bcm_field_fb6_trie_info[unit][trie_type].iqual,
                                           f_ent->new_key_info[trie_type].cid.w,
                                           f_ent->new_key_info[trie_type].cid_mask.w,
                                           _FP_QUALIFIER_ADD));

            if (!(f_ent->new_key_info[trie_type].flags & _BCM_FIELD_TRIE_NODE_LIST_COMPRESSED)) {
                reuse_tcam_entry = 0;
                if (f_ent->fs == NULL) {
                    /* List compressed entry moving to trie */
                    BCM_IF_ERROR_RETURN
                        (_bcm_field_fb6_compress_entry_slice_assign(unit, f_ent));
                }
            }
        }
        sal_memcpy(&f_ent->key_info[trie_type], &f_ent->new_key_info[trie_type],
                   sizeof(_bcm_field_basic_info_key_t));
    }

    if (reuse_tcam_entry && !f_ent->fs) {
        int i, parts_count = 0;
        /* Get number of tcam parts based on group flags. */
        BCM_IF_ERROR_RETURN
            (_bcm_field_entry_tcam_parts_count(unit, f_ent->group->stage_id,
                                               f_ent->group->flags, &parts_count));
        for (i = 0; i < parts_count; i++) {
            f_ent[i].flags = f_ent_lc[i].flags;
            /* Free the slice index so it can be reused */
            f_ent[i].fs[i].entries[f_ent[i].slice_idx] = NULL;
            f_ent[i].fs[i].free_count++;
            f_ent[i].fs = NULL;
        }
    }
    return BCM_E_NONE;
}

int
_bcm_field_fb6_compress_entry_remove(int unit, _field_entry_t *f_ent)
{
    _field_stage_t *stage_fc;
    int trie_type;

    if (f_ent->eid == _FP_INTERNAL_RESERVED_ID) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, f_ent->group->stage_id,
                                                 &stage_fc));

    for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
        if (f_ent->key_info[trie_type].ref_count) {
            if (f_ent->fs == NULL) {
                f_ent->flags &= ~_FP_ENTRY_INSTALLED;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_field_fb6_compress_alpm_delete(unit, stage_fc, trie_type,
                                                     &f_ent->key_info[trie_type]));
            break;
        }
    }

    return BCM_E_NONE;
}

int
_bcm_field_fb6_compress_entry_destroy(int unit, _field_entry_t *f_ent)
{
    _field_stage_t *stage_fc;
    int trie_type;

    if (f_ent->eid == _FP_INTERNAL_RESERVED_ID) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, f_ent->group->stage_id,
                                                 &stage_fc));

    for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
        if ((f_ent->key_info[trie_type].ref_count)) {
            BCM_IF_ERROR_RETURN
                (_bcm_field_fb6_compress_basic_info_key_delete(unit, f_ent->eid, trie_type,
                                                               &f_ent->key_info[trie_type].cid));
            BCM_IF_ERROR_RETURN
                (_bcm_field_compress_key_delete(unit, &f_ent->key_info[trie_type],
                                                stage_fc->compress_tries[trie_type],
                                                &trie_type));
        }
    }

    return BCM_E_NONE;
}

/* Set compression key from the compressed qualifiers data and mask */
int
_bcm_field_fb6_compress_qualify_set(int unit,  _field_entry_t *f_ent,
                                    bcm_field_qualify_t qual,
                                    uint32 *data, uint32 *mask)
{
    int trie_type, i, prefix, count;

    if (NULL == data || NULL == mask) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(_bcm_field_fb6_compress_trie_type_get(unit, f_ent, qual,
                                                          &trie_type))) {
        /* Non-compressed qualifiers */
        return (_bcm_field_th_qualify_set(unit, f_ent->eid, qual, data,
                                          mask, _FP_QUALIFIER_ADD));
    }

    for (i = 0; i <  _bcm_field_fb6_trie_info[unit][trie_type].num_qual; i++) {
        if (qual == _bcm_field_fb6_trie_info[unit][trie_type].qual[i]) {
            SHR_BITCOUNT_RANGE(mask, prefix, 0,
                               _bcm_field_fb6_trie_info[unit][trie_type].width[i]);
            SHR_BITCOUNT_RANGE(mask, count,
                               _bcm_field_fb6_trie_info[unit][trie_type].width[i] - prefix,
                               prefix);
            if (count != prefix) {
                /* Mask has 0's in between */
                return BCM_E_PARAM;
            }

            SHR_BITCOPY_RANGE(f_ent->new_key_info[trie_type].key.w,
                              _bcm_field_fb6_trie_info[unit][trie_type].start_bit[i],
                              data, 0,
                              _bcm_field_fb6_trie_info[unit][trie_type].width[i]);
            SHR_BITCOPY_RANGE(f_ent->new_key_info[trie_type].key_mask.w,
                              _bcm_field_fb6_trie_info[unit][trie_type].start_bit[i],
                              mask, 0,
                              _bcm_field_fb6_trie_info[unit][trie_type].width[i]);
            SHR_BITCOUNT_RANGE(f_ent->new_key_info[trie_type].key_mask.w,
                               prefix, 0,
                               _bcm_field_fb6_trie_info[unit][trie_type].key_width);
            f_ent->new_key_info[trie_type].prefix = (uint8)prefix;

            /* Keep track of which compression keys are valid for this entry */
            f_ent->new_key_info[trie_type].ref_count = 1;
        }
    }

    return BCM_E_NONE;
}

/* Get compressed qualifiers data and mask from the compression key */
int
_bcm_field_fb6_compress_entry_qualifier_key_get(int unit,  _field_entry_t *f_ent,
                                                bcm_field_qualify_t qual,
                                                _bcm_field_qual_data_t data,
                                                _bcm_field_qual_data_t mask)
{
    int trie_type, i;

    /* Input Parameter Check. */
    if (NULL == data || NULL == mask) {
        return BCM_E_PARAM;
    }

    if (BCM_FAILURE(_bcm_field_fb6_compress_trie_type_get(unit, f_ent, qual,
                                                          &trie_type))) {
        /* Non-compressed qualifiers */
        return (_bcm_field_th_entry_qualifier_key_get(unit, f_ent->eid, qual,
                                                      data, mask));
    }

    for (i = 0; i <  _bcm_field_fb6_trie_info[unit][trie_type].num_qual; i++) {
        if (qual == _bcm_field_fb6_trie_info[unit][trie_type].qual[i]) {
            SHR_BITCOPY_RANGE(data, 0, f_ent->new_key_info[trie_type].key.w,
                              _bcm_field_fb6_trie_info[unit][trie_type].start_bit[i],
                              _bcm_field_fb6_trie_info[unit][trie_type].width[i]);

            SHR_BITCOPY_RANGE(mask, 0, f_ent->new_key_info[trie_type].key_mask.w,
                              _bcm_field_fb6_trie_info[unit][trie_type].start_bit[i],
                              _bcm_field_fb6_trie_info[unit][trie_type].width[i]);
        }
    }

    return BCM_E_NONE;
}

int
_field_fb6_compress_init(int unit, _field_stage_t *stage_fc)
{
    _bcm_field_shr_trie_md_t *trie = NULL;
    uint8 trie_type;
    uint64 rval64;

    /* validate input params */
    if (stage_fc == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_field_fb6_compress_deinit(unit, stage_fc));

    for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
        _bcm_field_fb6_trie_info[unit][trie_type].num_qual = 1; /* Ip */
        _bcm_field_fb6_trie_info[unit][trie_type].qual =
            sal_alloc(sizeof(bcm_field_qualify_t), "_bcm_field_fb6_trie_info");
        if (_bcm_field_fb6_trie_info[unit][trie_type].qual == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_field_fb6_trie_info[unit][trie_type].width =
            sal_alloc(sizeof(int), "_bcm_field_fb6_trie_info");
        if (_bcm_field_fb6_trie_info[unit][trie_type].width == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_field_fb6_trie_info[unit][trie_type].start_bit =
            sal_alloc(sizeof(int), "_bcm_field_fb6_trie_info");
        if (_bcm_field_fb6_trie_info[unit][trie_type].start_bit == NULL) {
            return BCM_E_MEMORY;
        }

        if (trie_type == _BCM_FIELD_COMPRESS_TYPE_SRC_IPV4) {
            /* Ipv4 Src Ip(32 LSb) */
            _bcm_field_fb6_trie_info[unit][trie_type].qual[0] = bcmFieldQualifySrcIp;
            _bcm_field_fb6_trie_info[unit][trie_type].width[0] = BCM_L3_IP4_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].start_bit[0] = 0;
            _bcm_field_fb6_trie_info[unit][trie_type].key_width = BCM_L3_IP4_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].iqual = _bcmFieldQualifySrcIpv4CompressedClassId;
        } else if (trie_type == _BCM_FIELD_COMPRESS_TYPE_DST_IPV4) {
            /* Ipv4 Dst Ip(32 LSb) */
            _bcm_field_fb6_trie_info[unit][trie_type].qual[0] = bcmFieldQualifyDstIp;
            _bcm_field_fb6_trie_info[unit][trie_type].width[0] = BCM_L3_IP4_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].start_bit[0] = 0;
            _bcm_field_fb6_trie_info[unit][trie_type].key_width = BCM_L3_IP4_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].iqual = _bcmFieldQualifyDstIpv4CompressedClassId;
        } else if (trie_type == _BCM_FIELD_COMPRESS_TYPE_SRC_IPV6) {
            /* Ipv6 Src Ip(128 LSb) */
            _bcm_field_fb6_trie_info[unit][trie_type].qual[0] = bcmFieldQualifySrcIp6;
            _bcm_field_fb6_trie_info[unit][trie_type].width[0] = BCM_L3_IP6_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].start_bit[0] = 0;
            _bcm_field_fb6_trie_info[unit][trie_type].key_width = BCM_L3_IP6_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].iqual = _bcmFieldQualifySrcIpv6CompressedClassId;
        } else {
            /* Ipv6 Dst Ip(128 LSb) */
            _bcm_field_fb6_trie_info[unit][trie_type].qual[0] = bcmFieldQualifyDstIp6;
            _bcm_field_fb6_trie_info[unit][trie_type].width[0] = BCM_L3_IP6_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].start_bit[0] = 0;
            _bcm_field_fb6_trie_info[unit][trie_type].key_width = BCM_L3_IP6_MAX_NETLEN;
            _bcm_field_fb6_trie_info[unit][trie_type].iqual = _bcmFieldQualifyDstIpv6CompressedClassId;
        }

        _bcm_field_fb6_trie_info[unit][trie_type].cid_width = 16;

        /* Create the compression tries. */
        BCM_IF_ERROR_RETURN
            (_bcm_field_compress_trie_create(unit,
                                             _bcm_field_fb6_trie_info[unit][trie_type].key_width,
                                             _bcm_field_fb6_trie_info[unit][trie_type].cid_width,
                                             trie_type, _bcm_field_fb6_compress_entry_update,
                                             &trie));
        trie->ckey_acquire = NULL;
        trie->ckey_release = NULL;
        trie->stage_id = stage_fc->stage_id;
        stage_fc->compress_tries[trie_type] = trie;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_bcm_field_fb6_compress_scache_allocate(unit));
    }
#endif

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ENABLE_L3_CLASSID_FOR_L2PKTSf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));

    return BCM_E_NONE;
}

int
_field_fb6_compress_deinit(int unit, _field_stage_t *stage_fc)
{
    int trie_type;
    _bcm_field_compress_cid_list_t *cid_list, *cid_list_tmp;

    /* validate input params */
    if (stage_fc == NULL) {
        return BCM_E_PARAM;
    }

    for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
        if (_bcm_field_fb6_trie_info[unit][trie_type].qual != NULL) {
            sal_free(_bcm_field_fb6_trie_info[unit][trie_type].qual);
            _bcm_field_fb6_trie_info[unit][trie_type].qual = NULL;
        }
        if (_bcm_field_fb6_trie_info[unit][trie_type].width != NULL) {
            sal_free(_bcm_field_fb6_trie_info[unit][trie_type].width);
            _bcm_field_fb6_trie_info[unit][trie_type].width = NULL;
        }
        if (_bcm_field_fb6_trie_info[unit][trie_type].start_bit != NULL) {
            sal_free(_bcm_field_fb6_trie_info[unit][trie_type].start_bit);
            _bcm_field_fb6_trie_info[unit][trie_type].start_bit = NULL;
        }

        cid_list = _bcm_field_fb6_compress_cid_list[unit][trie_type];
        while (cid_list) {
            if (cid_list->entries) {
                sal_free(cid_list->entries);
                cid_list->entries = NULL;
            }
            cid_list_tmp = cid_list;
            cid_list = cid_list->next;
            sal_free(cid_list_tmp);
            cid_list_tmp = NULL;
        }
        _bcm_field_fb6_compress_cid_list[unit][trie_type] = NULL;

        if (stage_fc->compress_tries[trie_type]) {
            /* Delete the compression tries. */
            BCM_IF_ERROR_RETURN
                (_bcm_field_compress_trie_deinit(unit, stage_fc->compress_tries[trie_type]));
            stage_fc->compress_tries[trie_type] = NULL;
        }
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FIELD_COMPRESS_WB_VERSION_1_0          SOC_SCACHE_VERSION(1,0)
#define SOC_FIELD_COMPRESS_WB_DEFAULT_VERSION      SOC_FIELD_COMPRESS_WB_VERSION_1_0
#define BCM_FIELD_COMPRESSION_MAX_NODES 5000
int
_bcm_field_fb6_compress_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0, alloc_size = 0;
    int stable_size, default_ver = SOC_FIELD_COMPRESS_WB_DEFAULT_VERSION;

    alloc_size = sizeof(bcm_field_entry_t) +
                 (sizeof(_bcm_field_basic_info_key_t) * _BCM_FIELD_COMPRESS_TYPE_MAX) +
                 (sizeof(_bcm_field_basic_info_key_t) * _BCM_FIELD_COMPRESS_TYPE_MAX);
    alloc_size *= BCM_FIELD_COMPRESSION_MAX_NODES;

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FIELD_COMPRESSION_HANDLE, 0);
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_get,
                                          &scache_ptr,
                                          default_ver,
                                          NULL);

    if (rv  == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }

    if (NULL == scache_ptr) {
        return SOC_E_MEMORY;
    }
    return SOC_E_NONE;
}

int
_bcm_field_fb6_compress_scache_sync(int unit)
{
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0, alloc_size = 0, var_size = 0, scache_offset = 0;
    _field_control_t *fc = NULL;
    _field_group_t *fg = NULL;
    _field_entry_t *f_ent = NULL;
    int rv = 0, i, trie_type;

    alloc_size = sizeof(bcm_field_entry_t) +
                 (sizeof(_bcm_field_basic_info_key_t) * _BCM_FIELD_COMPRESS_TYPE_MAX) +
                 (sizeof(_bcm_field_basic_info_key_t) * _BCM_FIELD_COMPRESS_TYPE_MAX);
    alloc_size *= BCM_FIELD_COMPRESSION_MAX_NODES;

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FIELD_COMPRESSION_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &scache_ptr,
                        SOC_FIELD_COMPRESS_WB_DEFAULT_VERSION,
                        NULL);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }
    if (NULL == scache_ptr) {
        return SOC_E_MEMORY;
    }

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    fg = fc->groups;
    while (NULL != fg)  {
        if ((fg->stage_id != _BCM_FIELD_STAGE_INGRESS) ||
            !(fg->flags & _FP_GROUP_COMPRESSED)) {
            fg = fg->next;
            continue;
        }
        for (i = 0; i < fg->group_status.entry_count; i++) {
            if (fg->entry_arr[i] == NULL) {
                continue;
            }
            f_ent = fg->entry_arr[i];

            for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
                if ((f_ent->key_info[trie_type].ref_count)) {
                    var_size = sizeof(bcm_field_entry_t);
                    /* Entry Id */
                    sal_memcpy(&scache_ptr[scache_offset], &f_ent->eid, var_size);
                    scache_offset += var_size;
                    var_size = sizeof(_bcm_field_basic_info_key_t) * _BCM_FIELD_COMPRESS_TYPE_MAX;
                    /* Key basic info */
                    sal_memcpy(&scache_ptr[scache_offset], f_ent->key_info, var_size);
                    scache_offset += var_size;

                    sal_memcpy(&scache_ptr[scache_offset], f_ent->new_key_info, var_size);
                    scache_offset += var_size;
                    break;
                }
            }
        }
        fg = fg->next;
    }

    return SOC_E_NONE;
}

int
_bcm_field_fb6_compress_scache_recovery(int unit)
{
    uint32 alloc_get = 0, alloc_size = 0, scache_offset = 0, var_size = 0;
    int rv = SOC_E_NONE, i, trie_type;
    uint8 *scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint16 recovered_ver = 0;
    bcm_field_entry_t eid;
    _field_stage_t *stage_fc = NULL;
    _field_entry_t *f_ent = NULL;

    alloc_size = sizeof(bcm_field_entry_t) +
                 (sizeof(_bcm_field_basic_info_key_t) * _BCM_FIELD_COMPRESS_TYPE_MAX);
    alloc_size *= BCM_FIELD_COMPRESSION_MAX_NODES;

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FIELD_COMPRESSION_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &scache_ptr,
                        SOC_FIELD_COMPRESS_WB_DEFAULT_VERSION,
                        &recovered_ver);
    if (SOC_FAILURE(rv)) {
        if ((rv == SOC_E_CONFIG) ||
            (rv == SOC_E_NOT_FOUND)) {
            /* warmboot file does not contain this
            * module, or the warmboot state does not exist.
            * in this case return SOC_E_NOT_FOUND
            */
            return SOC_E_NOT_FOUND;
        } else {
            /* Only Level2 - treat this as a error */
            return rv;
        }
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }

    if (NULL == scache_ptr) {
        return SOC_E_MEMORY;
    }

    for (i = 0; i < BCM_FIELD_COMPRESSION_MAX_NODES; i++) {
        var_size = sizeof(bcm_field_entry_t);
        /* Entry Id */
        sal_memcpy(&eid, &scache_ptr[scache_offset], var_size);
        scache_offset += var_size;

        BCM_IF_ERROR_RETURN(_field_entry_get(unit, eid, _FP_ENTRY_PRIMARY, &f_ent));
        /* Key basic info */
        var_size = sizeof(_bcm_field_basic_info_key_t) * _BCM_FIELD_COMPRESS_TYPE_MAX;
        sal_memcpy(f_ent->key_info, &scache_ptr[scache_offset], var_size);
        scache_offset += var_size;

        sal_memcpy(f_ent->new_key_info, &scache_ptr[scache_offset], var_size);
        scache_offset += var_size;

        BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, f_ent->group->stage_id,
                                                     &stage_fc));
        for (trie_type = 0; trie_type < _BCM_FIELD_COMPRESS_TYPE_MAX; trie_type++) {
            if ((f_ent->key_info[trie_type].ref_count)) {
                (void)_bcm_field_compress_key_recover(unit, &f_ent->key_info[trie_type],
                                                      NULL, stage_fc->compress_tries[trie_type]);
                BCM_IF_ERROR_RETURN
                    (_bcm_field_fb6_compress_basic_info_key_insert
                                (unit, f_ent->eid, trie_type, &f_ent->key_info[trie_type].cid));
            }
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#else /* BCM_FIREBOLT6_SUPPORT && BCM_FIELD_SUPPORT */
int bcmi_fb6_field_not_entry;
#endif /* BCM_FIREBOLT6_SUPPORT && BCM_FIELD_SUPPORT */
