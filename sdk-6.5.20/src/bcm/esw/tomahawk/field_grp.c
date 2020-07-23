 /*
  * 
  *
  * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
  * 
  * Copyright 2007-2020 Broadcom Inc. All rights reserved.
  *
  * File:       field_grp.c
  * Purpose:    BCM56960 Field Processor Group management functions.
  */

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/tomahawk.h>

#if defined (BCM_UTT_SUPPORT)
#include <soc/esw/utt.h>
#endif

#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm/field.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/udf.h>

#if defined(BCM_HELIX5_SUPPORT)
#include <bcm_int/esw/helix5.h>
#endif

#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/xgs4.h>
#endif
int th_prio_set_with_no_free_entries = FALSE;

/* Local functions prototypes. */
STATIC int
_field_th_group_add(int unit, _field_group_add_fsm_t *fsm_ptr);
STATIC int
_field_th_lt_default_tcam_entry_install(int unit, bcm_field_entry_t lt_entry,
                                        int group_expand, int part_index,
                                        _field_lt_slice_t *lt_fs);
STATIC int
_field_th_ingress_group_expand_slice_install(int unit, _field_stage_t *stage_fc,
                                         _field_group_t *fg, uint8 slice_number,
                                         int part_index, int group_slice_prio);
STATIC int
_field_th_ingress_group_expand_install(int unit, _field_stage_t *stage_fc,
                                       _field_group_t *fg, int entry_part,
                                       _field_lt_slice_t *lt_fs);
#define IS_POST_MUX_QUALIFIER(qid) \
            ((_BCM_FIELD_IS_PBMP_QUALIFIER(qid)) \
                || (bcmFieldQualifyNatDstRealmId == qid) \
                || (bcmFieldQualifyNatNeeded == qid) \
                || (bcmFieldQualifyDrop == qid) \
                || (bcmFieldQualifyDstTrunk == qid) \
                || (bcmFieldQualifyDstMultipath == qid) \
                || (bcmFieldQualifyDstMultipathOverlay == qid) \
                || (bcmFieldQualifyDstMultipathUnderlay == qid) \
                || (bcmFieldQualifyDstL2MulticastGroup == qid) \
                || (bcmFieldQualifyDstL3MulticastGroup == qid) \
                || (bcmFieldQualifyDstL3EgressNextHops == qid) \
                || (bcmFieldQualifyDstL3Egress == qid) \
                || (bcmFieldQualifyDstPort == qid) \
                || (bcmFieldQualifyDstMimGport == qid) \
                || (bcmFieldQualifyDstNivGport == qid) \
                || (bcmFieldQualifyDstVxlanGport == qid) \
                || (bcmFieldQualifyDstVlanGport == qid) \
                || (bcmFieldQualifyDstMplsGport == qid) \
                || (bcmFieldQualifyDstWlanGport == qid) \
                || (bcmFieldQualifyDstMimGports == qid) \
                || (bcmFieldQualifyDstNivGports == qid) \
                || (bcmFieldQualifyDstVxlanGports == qid) \
                || (bcmFieldQualifyDstVlanGports == qid) \
                || (bcmFieldQualifyDstMplsGports == qid) \
                || (bcmFieldQualifyDstWlanGports == qid) \
                || ((int)_bcmFieldQualifyExactMatchHitStatusLookup0 == (int)qid) \
                || ((int)_bcmFieldQualifyExactMatchHitStatusLookup1 == (int)qid) \
                || ((int)_bcmFieldQualifyExactMatchActionClassIdLookup0 == (int)qid) \
                || ((int)_bcmFieldQualifyExactMatchActionClassIdLookup1 == (int)qid))

#define IS_EXT_OVERLAID_POST_MUXERS(ace_flags, ext_flags) \
          (((ace_flags & _FP_POST_MUX_NAT_DST_REALM_ID) &&  \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_NAT_DST_REALM_ID)) || \
           ((ace_flags & _FP_POST_MUX_NAT_NEEDED) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_NAT_NEEDED)) || \
           ((ace_flags & _FP_POST_MUX_EM_HIT_STATUS_LOOKUP_0) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_0)) || \
           ((ace_flags & _FP_POST_MUX_EM_HIT_STATUS_LOOKUP_1) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_EM_HIT_STATUS_LOOKUP_1)) || \
           ((ace_flags & _FP_POST_MUX_EM_ACTION_CLASSID_LOOKUP_0) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_0)) || \
           ((ace_flags & _FP_POST_MUX_EM_ACTION_CLASSID_LOOKUP_1) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_EM_ACTION_CLASSID_LOOKUP_1)) || \
           ((ace_flags & _FP_POST_MUX_DROP) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_DROP)) || \
           ((ace_flags & _FP_POST_MUX_SRC_DST_CONT_1_SLI_B) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_B)) || \
           ((ace_flags & _FP_POST_MUX_SRC_DST_CONT_0_SLI_B) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_B)) || \
           ((ace_flags & _FP_POST_MUX_SRC_DST_CONT_1_SLI_C) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1_C)) || \
           ((ace_flags & _FP_POST_MUX_SRC_DST_CONT_0_SLI_C) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0_C)) || \
           ((ace_flags & _FP_POST_MUX_SRC_DST_CONT_1) &&  \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_1)) || \
           ((ace_flags & _FP_POST_MUX_SRC_DST_CONT_0) && \
            (ext_flags & _FP_EXT_ATTR_NOT_WITH_SRC_DST_CONT_0)))


#define IS_EXT_OVERLAID_IPBM(ace_flags, ext_flags) \
                   ((ace_flags & _FP_POST_MUX_IPBM) && \
                    (ext_flags & _FP_EXT_ATTR_NOT_WITH_IPBM))


#define _NUM_CHUNKS_PER_GRAN 50
#define _NUM_GRAN 5

#define GRAN_BASE(gran, base)            \
        {                                \
           if (gran == 32) {             \
               base = 0;                 \
           } else if (gran == 16) {      \
               base = 1;                 \
           } else if (gran == 8) {       \
               base = 2;                 \
           } else if (gran == 4) {       \
               base = 3;                 \
           } else if (gran == 2) {       \
               base = 4;                 \
           }                             \
        }

/*
 * Enum to define IPBM_SOURCE
 */
typedef enum _bcmFieldIpbmSource_e {
   _bcmFieldIpbmSourcePipeLocal = 0,   /* Port Based Selective Mask.      */
   _bcmFieldIpbmSourcePort      = 1,   /* Ttrunk Based Selective Mask.    */
   _bcmFieldIpbmSourceVp        = 2,   /* Source VP based Selective Mask. */
   _bcmFieldIpbmSourceDevPort   = 3    /* IPBM Index based selective Mask.*/
} _bcmFieldIpbmSource_t;



/*
 * Function:
 *     _field_th_group_add_initialize
 * Purpose:
 *     Perform fsm initialization & execute basic checks
 *     required before field group creation.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX.
 */
STATIC int
_field_th_group_add_initialize(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t *fg_temp; /* Pointer to an existing Field Group.  */
    _field_stage_id_t stage; /* Field Processor stage ID.            */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Save current FSM state in previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    if (BCM_SUCCESS(_field_group_get(unit, fsm_ptr->group_id, &fg_temp))) {
        LOG_ERROR(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "FP(unit %d) Error: group=%d already exists.\n"),
             unit, fsm_ptr->group_id));
        fsm_ptr->rv = (BCM_E_EXISTS);
    }

    /* Get Field Stage control pointer. */
    if (BCM_SUCCESS(fsm_ptr->rv)) {
        fsm_ptr->rv = _field_control_get(unit, &fsm_ptr->fc);
    }

    /* Get pipeline stage from qualifiers set.  */
    if (BCM_SUCCESS(fsm_ptr->rv)) {
        fsm_ptr->rv = _bcm_field_group_stage_get(unit, &fsm_ptr->qset, &stage);
        if ((_BCM_FIELD_STAGE_INGRESS != stage) &&
            (_BCM_FIELD_STAGE_EXACTMATCH != stage) &&
            (_BCM_FIELD_STAGE_FLOWTRACKER != stage)) {
            fsm_ptr->rv = BCM_E_INTERNAL;
        }
    }

    /*
     * Attempt IntraSlice Double wide Groups if stage supports it.
     * Note: TH IFP stage default comes up in IntraSlice Double wide mode.
     */
    if (BCM_SUCCESS(fsm_ptr->rv)
        && soc_feature(unit, soc_feature_field_intraslice_double_wide)
        && (fsm_ptr->fc->flags & _FP_INTRASLICE_ENABLE)
        && (stage != _BCM_FIELD_STAGE_EXACTMATCH)) {
        fsm_ptr->flags |= _BCM_FP_GROUP_ADD_INTRA_SLICE;
    }

    /* Get Field Stage Control Pointer. */
    if (BCM_SUCCESS(fsm_ptr->rv)) {
        fsm_ptr->rv = _field_stage_control_get(unit, stage, &fsm_ptr->stage_fc);
    }

    if (BCM_SUCCESS(fsm_ptr->rv)) {
        /* Verify if requested QSET is supported by the stage. */
        if (FALSE == _field_qset_is_subset(&fsm_ptr->qset,
                    &fsm_ptr->stage_fc->_field_supported_qset)) {
            LOG_INFO(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "FP(unit %d) Error: Qualifier set is not"
                                " supported by the device.\n"), unit));
            fsm_ptr->rv = (BCM_E_UNAVAIL);
        }
    }

    /* Verify Group Priority for flowtracker stage */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (BCM_SUCCESS(fsm_ptr->rv) &&
        soc_feature(unit, soc_feature_field_flowtracker_support) &&
        (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER)) {
        if (fsm_ptr->priority != 0) {
            fsm_ptr->rv = (BCM_E_CONFIG);
            LOG_INFO(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "FP(unit %d) Error: Field Group Priority"
                                " not supported at this stage.\n"), unit));
        }
    }
#endif

    if (BCM_FAILURE(fsm_ptr->rv)) {
        /* Error return hence perform clean up. */
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
    } else {
        /* Proceed to group allocation state. */
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_ALLOC;
    }

    /* Execute the state machine for the new state. */
    return (BCM_E_NONE);
}

#if 0
/*
 * Function:
 *     _field_th_group_priority_validate
 * Purpose:
 *     Validate group prioirty value, if specified priority value is NOT
 *     BCM_FIELD_GROUP_PRIO_ANY then it must be unique.
 * Parameters:
 *     unit     - (IN  BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_priority_validate(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg)
{
    int lt_idx;                  /* Logical Table iterator.    */
    _field_lt_config_t *lt_info; /* Logical Table information. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    /*
     * If Group doesn't care about priority, one of the available
     * priorities is allocated. Hence return done.
     */
    if (BCM_FIELD_GROUP_PRIO_ANY == fg->priority) {
        return (BCM_E_NONE);
    }

    /*
     * Two Field groups cannot have the same priority value as hardware
     * behavior is Indeterministic if packet matches rules in both the groups
     * that have same Group/Action priority value.
     */
    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {
        lt_info = stage_fc->lt_info[fg->instance][lt_idx];
        if ((TRUE == lt_info->valid) && (fg->priority == lt_info->priority)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Group=%d Priority=%d already in-use.\n"),
                 unit, fg->gid, fg->priority));
            return (BCM_E_PARAM);
        }
    }

    return (BCM_E_NONE);
}
#endif /* 0 */

/*
 * Function:
 *     _field_th_group_add_alloc
 * Purpose:
 *     Allocate & initialize field group structure.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX.
 */
STATIC int
_field_th_group_add_alloc(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    int idx;                    /* Iterator variable.               */
    int mem_sz;                 /* Memory size to be allocated.     */
    _field_group_t *fg = NULL;  /* New group inforation structure.  */
    int hints_ref_updated = FALSE;

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Save current state as previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    /* Determine field group structure buffer size. */
    mem_sz = sizeof(_field_group_t);

    /* Allocate and initialize memory for Field Group. */
    _FP_XGS3_ALLOC(fg, mem_sz, "field group");
    if (NULL == fg) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error:Allocation failure for "
             "_field_group_t\n"), unit));
        /* Memory allocation failure hence proceed to clean up. */
        fsm_ptr->rv = BCM_E_MEMORY;
        goto bcm_error;
    }

    /* Initialize group structure. */
    fg->gid            = fsm_ptr->group_id;
    fg->stage_id       = fsm_ptr->stage_fc->stage_id;
    fg->qset           = fsm_ptr->qset;
    fg->pbmp           = fsm_ptr->pbmp;
    fg->priority       = fsm_ptr->priority;
    fg->hintid         = fsm_ptr->hintid;
    fg->action_res_id  = fsm_ptr->action_res_id;
    sal_memcpy(&fg->aset, &fsm_ptr->aset, sizeof(bcm_field_aset_t));

    /* Initialize the action profile index's to -1 */
    for (idx=0; idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; idx++) {
        fg->action_profile_idx[idx] =  -1;
    }

    for (idx = 0; idx < _FP_PAIR_MAX; idx++) {
        fg->vmap_group[idx] = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    }

    /* Clear the group's select codes and its slice Qsets. */
    for (idx = 0; idx < _FP_MAX_ENTRY_WIDTH; idx++) {
        _FIELD_SELCODE_CLEAR(fg->sel_codes[idx]);
    }

    /* Set the Field Group's instance information. */
    fsm_ptr->rv = _bcm_field_th_group_instance_set(unit, fg);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }

    fsm_ptr->rv = _bcm_field_hints_group_count_update(unit, fg->hintid, 1);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }
    hints_ref_updated = TRUE;

    /* Initialize group status. */
    fsm_ptr->rv = _bcm_field_group_status_init(unit, &fg->group_status);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }

    /* Hint Check. */
    fsm_ptr->rv = _bcm_field_hints_group_info_update(unit, fg);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto bcm_error;
    }

    /* Set slice size selection group flags. */
    if (fsm_ptr->flags & _BCM_FP_GROUP_ADD_SMALL_SLICE) {
        fg->flags |= _FP_GROUP_SELECT_SMALL_SLICE;
    } else if (fsm_ptr->flags & _BCM_FP_GROUP_ADD_LARGE_SLICE) {
        fg->flags |= _FP_GROUP_SELECT_LARGE_SLICE;
    }

    /* Clear the group's slice extractor selectors value. */
    for (idx = 0; idx < _FP_MAX_ENTRY_WIDTH; idx++) {
        _FP_EXT_SELCODE_CLEAR(fg->ext_codes[idx]);
    }

    /* By default mark group as active. */
    fg->flags |= _FP_GROUP_LOOKUP_ENABLED;

    /* Update Preselector Support Flags */
    if (fsm_ptr->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESELSET) {
       bcm_field_presel_t       presel;

       /* Preselector feature check */
       if (!soc_feature(unit, soc_feature_field_preselector_support)) {
          fsm_ptr->rv = BCM_E_UNAVAIL;
          goto bcm_error;
       }

       /* initialize the group preselector array */
       sal_memset(&fg->presel_ent_arr[0], 0x0,
            sizeof(_field_presel_entry_t *) * _FP_PRESEL_ENTRIES_MAX_PER_GROUP);

       for (presel = 0; presel < BCM_FIELD_PRESEL_SEL_MAX; presel++) {
          if (BCM_FIELD_PRESEL_TEST(fsm_ptr->preselset, presel)) {
             /* Assign the preselectors to the group presel array. */
             fsm_ptr->rv = _bcm_field_presel_group_add(unit, fg, presel);
             if (BCM_FAILURE(fsm_ptr->rv)) {
                 goto bcm_error;
             }
             fg->flags |= _FP_GROUP_PRESELECTOR_SUPPORT;
          }
       }

       if (!(fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Group (GID= %d) should have atleast"
               " one preselector assigned.\n\r"),fg->gid));
           fsm_ptr->rv = BCM_E_PARAM;
           goto bcm_error;
        }
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else {
        if (fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "ERROR: Group (GID= %d) should have atleast"
               " one preselector assigned.\n\r"),fg->gid));
            fsm_ptr->rv = BCM_E_PARAM;
            goto bcm_error;
        }
#endif
    }

#ifdef BCM_FIREBOLT6_SUPPORT
    if (soc_feature(unit, soc_feature_field_aacl_compression)) {
        if (fsm_ptr->flags & BCM_FIELD_GROUP_CREATE_WITH_COMPRESSION) {
            if (fsm_ptr->stage_fc->flags & _FP_STAGE_COMPRESSION) {
                fg->flags |= _FP_GROUP_COMPRESSED;
            } else {
                /* Stage doesn't support compression */
                fsm_ptr->rv = BCM_E_PARAM;
                goto bcm_error;
            }
        }
    }
#endif

    /* Set allocated poniter to FSM structure. */
    fsm_ptr->fg = fg;

    /* Update group QSET with internal qualifiers. */
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_QSET_UPDATE;

    /* Execute the state machine for the new state. */
    return (BCM_E_NONE);

bcm_error:
    if (fg != NULL) {
        /* Reset Hint*/
        if (hints_ref_updated) {
            (void) _bcm_field_hints_group_count_update(unit, fg->hintid, 0);
        }
       sal_free(fg);
       fg = NULL;
    }
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_calc_aset_size_all_actions
 * Purpose:
 *     For TD3 IFP
 *     Calculate the Aset size
 * Parameters:
 *     unit       - (IN)     BCM device number.
 *     fg         - (IN)     Field group structure.
 *     aset_size  - (IN/OUT) group's Aset size
 * Returns:
 *     BCM_E_XXX
 */
static int
_field_calc_aset_size_all_actions(int unit, _field_group_t *fg)
{
    _field_stage_t *stage_fc = NULL;    /* Field Stage Control.         */
    _bcm_field_aset_t aset;             /* Action Set.           */
    uint16 aset_key_size = 0;           /* Action Set Size.      */
    uint8 aset_cnt[_FieldActionSetCount] = {0};
                                        /* Action Per Set Count. */
    int a_idx;                          /* Action Iterator.      */
    _bcm_field_action_set_t            *action_set_ptr;
                                        /* action set pointer */
    _bcm_field_action_conf_t           *action_conf_ptr;
                                        /* action configuration pointer */
    uint8 field_action_set;             /* to hold Aset enum */


    /* Input parameters check. */
    if (NULL == fg) {
        return BCM_E_INTERNAL;
    }

    /* Retreive Action Set */
    aset = fg->aset;

    /* Check whether it is a valid stage */
    /* this fucntion is valid for TD3 IFP */
    if (!(soc_feature(unit, soc_feature_ifp_action_profiling)
        && (fg->stage_id == _BCM_FIELD_STAGE_INGRESS)))
    {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                           &stage_fc));

    action_set_ptr = stage_fc->action_set_ptr;

    /* Calculate Action Size. */
    for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
        if (BCM_FIELD_ASET_TEST(aset, a_idx)) {
            /* test whether the current action is supported in this stage */
            action_conf_ptr = stage_fc->f_action_arr[a_idx];
            if (NULL != action_conf_ptr) {
                /* get the aset enum from action_conf_pointer */
                field_action_set = action_conf_ptr->offset->aset;
            } else if (a_idx == bcmFieldActionPolicerGroup) {
                /* Flag in ASETs to indicate group
                 * policer usage with the group */
                field_action_set = _FieldActionMeterSet;
            } else if (a_idx == bcmFieldActionStatGroup) {
                /* Flag in ASETs to indicate stat group
                 * usage with the group */
                 field_action_set = _FieldActionCounterSet;
            } else {
                /* Ignore the unsupported actions */
                continue;
            }
            /* if the aset for current action is 0, or
               action_set size is 0 for this stage/chip
               skip the hw field updation step */
            if ((0 == field_action_set) ||
                (0 == action_set_ptr[field_action_set].size)) {
                continue;
            } else {
               if (aset_cnt[field_action_set] ==0) {
                   aset_key_size +=
                              action_set_ptr[field_action_set].size;
                   aset_cnt[field_action_set]++;
               }
            }
        }
    }
    /* Check if L3SW/Redirect action is set.
     * If yes then reserve 1 bit for Green To Pid. */
    if (aset_cnt[_FieldActionL3SwChangeL2Set] ||
        aset_cnt[_FieldActionRedirectSet]) {
        aset_key_size +=
                   action_set_ptr[_FieldActionGreenToPidSet].size;
    }
    return aset_key_size;
}


/* Add internal actions associated with main Action */
int
_bcm_field_qset_update_with_internal_actions(int unit, _field_group_t *fg)
{
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionIntEncapEnable) ||
        BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionNshEncapEnable)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionEditCtrlId);
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionExtractionCtrlId);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionIntTurnAround)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionEditCtrlId);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionIntEncapUpdate)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionExtractionCtrlId);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionAddIngOuterVlanToEgrOuterVlan) ||
        BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionErspan3HdrVlanCosPktCopy)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionExtractionCtrlId);
    }
    
#if 0
    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionErspan3HdrGbpSrcIdAdd)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque3);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionErspan3HdrUdf2Add)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque4);
    }
#endif /* 0 */
    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionNshServiceIndex)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque2);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionNshServicePathId)) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque1);
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque2);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionGbpSrcIdNew)
        || BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionAssignNatClassId)) {
        BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque1);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionGbpDstIdNew)) {
        BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque2);
    }

    if (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionAssignChangeL2FieldsClassId)) {
        BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionOpaque3);
    }

    if ((BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionGbpSrcMacMcastBitSet))
        || (BCM_FIELD_ASET_TEST(fg->aset, bcmFieldActionGbpClassifierAdd))) {
       BCM_FIELD_ASET_ADD(fg->aset, _bcmFieldActionExtractionCtrlId);
    }

#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *  _field_group_check_action_profile
 * Purpose:
 *  Check if same action profile is shared
 * Parameters:
 *  unit       - (IN)     BCM device number.
 *  fg         - (IN)     Field group structure.
 *  new_action_profile - (OUT) New Action Profile pointer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This function will check if action profile indexes associated
 *  to provided group is shared by any other group in same stage.
 *  If found then it will update new_action_profile to 1.
 */
int
_field_group_check_action_profile(int unit, _field_group_t *fg,
                                     uint16 *new_action_profile)
{
    _field_control_t *fc = NULL;
    uint16 ap_idx1 = 0, ap_idx2 = 0;
    _field_group_t *fgt = NULL;

    /* input parameter check */
    if (fg == NULL || new_action_profile == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* get field control strucutre */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* check if there is any other group with same action_profile index */
    for (fgt = fc->groups; fgt != NULL; fgt = fgt->next) {
        if ((fgt->gid == fg->gid) || (fgt->stage_id != fg->stage_id)) {
            continue;
        }
        for (ap_idx1 = 0; ap_idx1 < MAX_ACT_PROF_ENTRIES_PER_GROUP; ap_idx1++) {
            if (fgt->action_profile_idx[ap_idx1] != -1) {
                for (ap_idx2 = 0; ap_idx2 < MAX_ACT_PROF_ENTRIES_PER_GROUP; ap_idx2++) {
                    if (fgt->action_profile_idx[ap_idx1] ==
                                    fg->action_profile_idx[ap_idx2]) {
                        *new_action_profile = 1;
                        break;
                    }
                }
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_calc_group_aset_size
 * Purpose:
 *     For TD3 IFP
 *     Calculate the Aset size
 * Parameters:
 *     unit       - (IN)     BCM device number.
 *     fg         - (IN)     Field group structure.
 *     aset_size  - (IN/OUT) group's Aset size
 * Returns:
 *     BCM_E_XXX
 */
int
_field_calc_group_aset_size(int unit, _field_group_t *fg, uint16 *aset_size)
{
    _field_stage_t *stage_fc = NULL;    /* Field Stage Control.         */
    _bcm_field_aset_t aset;             /* Action Set.           */
    uint16 aset_key_size = 0;           /* Action Set Size.      */
    uint8 aset_cnt[_FieldActionSetCount] = {0};
                                        /* Action Per Set Count. */
    int a_idx;                          /* Action Iterator.      */
    _bcm_field_action_set_t            *action_set_ptr;
                                        /* action set pointer */
    _bcm_field_action_conf_t           *action_conf_ptr;
                                        /* action configuration pointer */
    uint8 field_action_set;             /* to hold Aset enum */
    uint8 setChangeCosOrIntPriSet = 0;  /* For Actions which has 2 Asets */
    uint8 setMeterSet = 0;              /* For color dependent actions
                                           meter set should be enabled */

    /* Input parameters check. */
    if (NULL == fg) {
        return BCM_E_INTERNAL;
    }

    /* Retreive Action Set */
    aset = fg->aset;

    /* Check whether it is a valid stage */
    /* this fucntion is valid for TD3 IFP */
    if (!(soc_feature(unit, soc_feature_ifp_action_profiling)
        && (fg->stage_id == _BCM_FIELD_STAGE_INGRESS)))
    {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                           &stage_fc));

    action_set_ptr = stage_fc->action_set_ptr;
    /* Calculate Action Size. */
    for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
        if (BCM_FIELD_ASET_TEST(aset, a_idx)) {
            /* test whether the current action is supported in this stage */
            action_conf_ptr = stage_fc->f_action_arr[a_idx];
            if (NULL != action_conf_ptr) {
                /* get the aset enum from action_conf_pointer */
                field_action_set = action_conf_ptr->offset->aset;
                /* PrioPktAndInt* actions needs 2 action Sets:
                   1: _FieldActionChangePktPriSet
                   2: _FieldActionChangeCosOrIntPriSet
                   _FieldActionChangePktPriSet is set as part of
                   action initialization. 2nd Set to be taken care here
                */
                /* check whether the action is PrioPktAndInt* actions */
                if (BCM_FIELD_ACTION_PRIO_PKT_AND_INT_TEST(a_idx)) {
                    setChangeCosOrIntPriSet = 1;
                }
            } else if (a_idx == bcmFieldActionPolicerGroup) {
                /* Flag in ASETs to indicate group
                 * policer usage with the group */
                field_action_set = _FieldActionMeterSet;
            } else if (a_idx == bcmFieldActionStatGroup) {
                /* Flag in ASETs to indicate stat group
                 * usage with the group */
                 field_action_set = _FieldActionCounterSet;
            } else {
                 /* action is not supported in this stage */
                 /* add log_error with stage, action, chip*/
                 LOG_ERROR(BSL_LS_BCM_FP,
                       (BSL_META_U(unit,
                        "FP(unit %d) Error: action=%s(%d) not supported\n"),
                         unit,
                         _field_action_name(a_idx),
                         a_idx
                         ));
                 return BCM_E_UNAVAIL;
            }
            /* if the aset for current action is 0, or
               action_set size is 0 for this stage/chip
               skip the hw field updation step */
            if ((0 == field_action_set) ||
                (0 == action_set_ptr[field_action_set].size)) {
                continue;
            } else {
               if (aset_cnt[field_action_set] ==0) {
                   aset_key_size +=
                              action_set_ptr[field_action_set].size;
                   aset_cnt[field_action_set]++;
               }
               if (1 == action_set_ptr[field_action_set].is_color_dependent) {
                   setMeterSet = 1;
               }
            }
        }
    }
    /* Set CosorIntPriSet in action profile mem and
       update key size if prioPktAndInt* actions are
       present in current group's Aset */
    if (1 == setChangeCosOrIntPriSet) {
        if (0 == aset_cnt[_FieldActionChangeCosOrIntPriSet]) {
           aset_key_size +=
                   action_set_ptr[_FieldActionChangeCosOrIntPriSet].size;
        }
    }
    /* Set MeterSet in action profile mem and
       update key size if any color dependent actions are
       present in current group's Aset */
    if (1 == setMeterSet) {
        if (0 == aset_cnt[_FieldActionMeterSet]) {
           aset_key_size +=
                   action_set_ptr[_FieldActionMeterSet].size;
           /* add meter Aset to fg's aset internally */
           BCM_FIELD_ASET_ADD(fg->aset, bcmFieldActionPolicerGroup);
        }
    }
    /* Check if L3SW/Redirect action is set.
     * If yes then reserve 1 bit for Green To Pid. */
    if (aset_cnt[_FieldActionL3SwChangeL2Set] ||
        aset_cnt[_FieldActionRedirectSet]) {
        aset_key_size +=
                   action_set_ptr[_FieldActionGreenToPidSet].size;
    }
    /* Update Aset size in group's structure */
    if (0 == aset_key_size) {
        LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
            "VERB: Trying to create group with no actions\n"
             "Creating the group with all supporting actions\n")));
        /* Update the group's Aset with all supported actions */
        BCM_IF_ERROR_RETURN(_field_group_default_aset_set(unit, fg));
        BCM_FIELD_ASET_ADD(fg->aset, bcmFieldActionPolicerGroup);
        BCM_FIELD_ASET_ADD(fg->aset, bcmFieldActionStatGroup);
        (void)_bcm_field_qset_update_with_internal_actions(unit, fg);
        /* Set the group's Aset size to
        cummulative of all supporing Action sets size */
        aset_key_size = _field_calc_aset_size_all_actions(unit, fg);
    }
  *aset_size = aset_key_size;
  return BCM_E_NONE;
}

/*
 * Function:
 *     _field_th_group_add_aset_validate
 * Purpose:
 *     For TH EM and TD3 IFP, EM
 *     Calculate the Aset size and
 *     allocate the action profile pointer for group
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_aset_validate(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t *fg;                 /* Field Group structure pointer. */
    _field_stage_t *stage_fc = NULL;    /* Field Stage Control.         */
    _bcm_field_aset_t aset;             /* Action Set.           */
    uint16 aset_key_size = 0, aset_size =0;          /* Action Set Size.      */
    uint8 aset_cnt[_FieldActionSetCount] = {0};
                                        /* Action Per Set Count. */
    int a_idx;                          /* Action Iterator.      */
    void *entries[1];                   /* Profile Entry.               */
    void *act_prof_entry[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                        /* Profile table buffer pointer */
    soc_mem_t mem;                      /* Memory Identifier.           */
    exact_match_action_profile_entry_t em_act_prof_entry;
                                        /* Action Profile Table Buffer. */
    ifp_policy_action_profile_entry_t
                        ifp_act_prof_entry[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                        /* Action Profile Table Buffer. */
    uint32 action_profile_idx[MAX_ACT_PROF_ENTRIES_PER_GROUP];
                                        /* Action profile index         */
    _bcm_field_action_set_t            *action_set_ptr;
                                        /* action set pointer */
    _bcm_field_action_conf_t           *action_conf_ptr;
                                        /* action configuration pointer */
    uint8 field_action_set;             /* to hold Aset enum */
    uint8 multi_mode = 0;
    uint8 setChangeCosOrIntPriSet = 0;  /* For Actions which has 2 Asets */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    if (NULL == fsm_ptr->fg) {
        fsm_ptr->rv = (BCM_E_PARAM);
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        return BCM_E_NONE;
    }
    /* Get field group structure. */
    fg = fsm_ptr->fg;

    /* Save current state as the previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;
    fsm_ptr->rv = (BCM_E_NONE);

    /* Retreive Action Set */
    aset = fg->aset;

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if ((soc_feature(unit, soc_feature_field_flowtracker_support)) &&
            (_BCM_FIELD_STAGE_FLOWTRACKER == fg->stage_id)) {
        /* Verify actions */
        for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
            if (BCM_FIELD_ASET_TEST(aset, a_idx)) {
                switch(a_idx) {
                    case bcmFieldActionFlowtrackerGroupId:
                    case bcmFieldActionFlowtrackerEnable:
                    case bcmFieldActionFlowtrackerNewLearnEnable:
                        aset_size++;
                        break;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
                    case bcmFieldActionFlowtrackerAggregateClass:
                    case bcmFieldActionFlowtrackerAggregateIngressGroupId:
                    case bcmFieldActionFlowtrackerAggregateIngressFlowIndex:
                        if (!soc_feature(unit,
                                    soc_feature_field_flowtracker_v3_support)) {
                            fsm_ptr->rv = (BCM_E_UNAVAIL);
                            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
                            return (BCM_E_NONE);
                        }
                        aset_size++;
                        break;
#endif
                    default:
                        fsm_ptr->rv = (BCM_E_UNAVAIL);
                        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
                        return (BCM_E_NONE);
                }
            }
        }
        if (aset_size == 0) {
            /* Update the group's Aset with all supported actions */
            BCM_IF_ERROR_RETURN(_field_group_default_aset_set(unit, fg));
        }
        goto clean_up;
    }
#endif

    /* Check whether it is a valid stage */
    if (!(((SOC_IS_TOMAHAWKX(unit))
        && (fg->stage_id == _BCM_FIELD_STAGE_EXACTMATCH))
        || soc_feature(unit, soc_feature_ifp_action_profiling))) {
         goto clean_up;
    }
    /* Assign correct Action Profile Memory
     * and Action set's depending on stage */
    if (fg->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        /* Assign Exact Match Action Profile Memory */
        mem = EXACT_MATCH_ACTION_PROFILEm;
        act_prof_entry[0] = &em_act_prof_entry;
        sal_memcpy(act_prof_entry[0], soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));
    } else {
        /* Assign ifp Action Profile Memory */
        mem = IFP_POLICY_ACTION_PROFILEm;
        for (a_idx =0;a_idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; a_idx++) {
            act_prof_entry[a_idx] = &ifp_act_prof_entry[a_idx];
            sal_memcpy(act_prof_entry[a_idx], soc_mem_entry_null(unit, mem),
                soc_mem_entry_words(unit, mem) * sizeof(uint32));
        }
    }

    /* Update internal pseudo actions for the given ASET */
    BCM_IF_ERROR_RETURN(_bcm_field_qset_update_with_internal_actions(unit, fg));

    /* calculate Aset size */
    fsm_ptr->rv = _field_calc_group_aset_size(unit, fg, &aset_size);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto clean_up;
    }

    /* Retreive Action Set (it can be updated in above func */
    aset = fg->aset;

    if (soc_feature(unit, soc_feature_ifp_action_profiling) &&
       (fg->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
       if (aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE) {
           /* we need to go for multi mode Policy tables */
           /* i.e., 2 IFP_POLICY_TABLE_WIDE entries will be
           used to save the policy data corresponding to each entry
           and correspondingly 2 action profiles will be used
           for saving group's actions bitmaps.
           The split will be based on color dependent Asets
           in one action profile and color independent actions in
           another Action profile entry */
           multi_mode = 1;
       }
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                           &stage_fc));
    action_set_ptr = stage_fc->action_set_ptr;

    /* Calculate Action Size. */
    for (a_idx = 0; a_idx < _bcmFieldActionCount; a_idx++) {
        if (BCM_FIELD_ASET_TEST(aset, a_idx)) {
            /* test whether the current action is supported in this stage */
            action_conf_ptr = stage_fc->f_action_arr[a_idx];
            if (NULL != action_conf_ptr) {
                /* get the aset enum from action_conf_pointer */
                field_action_set = action_conf_ptr->offset->aset;
                /* PrioPktAndInt* actions needs 2 action Sets:
                   1: _FieldActionChangePktPriSet
                   2: _FieldActionChangeCosOrIntPriSet
                   _FieldActionChangePktPriSet is set as part of
                   action initialization. 2nd Set to be taken care here
                */
                /* check whether the action is PrioPktAndInt* actions */
                if (BCM_FIELD_ACTION_PRIO_PKT_AND_INT_TEST(a_idx)
                    && fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                    setChangeCosOrIntPriSet = 1;
                }
            } else if (a_idx == bcmFieldActionPolicerGroup) {
                /* Flag in ASETs to indicate group
                 * policer usage with the group */
                field_action_set = _FieldActionMeterSet;
            } else if (a_idx == bcmFieldActionStatGroup) {
                /* Flag in ASETs to indicate stat group
                 * usage with the group */
                 field_action_set = _FieldActionCounterSet;
            } else {
                 /* action is not supported in this stage */
                 /* add log_error with stage, action, chip*/
                 LOG_ERROR(BSL_LS_BCM_FP,
                       (BSL_META_U(unit,
                        "FP(unit %d) Error: action=%s(%d) not supported\n"),
                         unit,
                         _field_action_name(a_idx),
                         a_idx));
                 fsm_ptr->rv = (BCM_E_UNAVAIL);
                 goto clean_up;
            }
            /* if the aset for current action is 0, or
               action_set size is 0 for this stage/chip
               skip the hw field updation step */
            if ((0 == field_action_set) ||
                (0 == action_set_ptr[field_action_set].size)) {
                continue;
            } else {
               if (aset_cnt[field_action_set] ==0) {
                   if (1 == multi_mode) {
                       /* check the color dependency of aset */
                       /* always 1st action profile will hold color dependent asets */
                       if (1 == action_set_ptr[field_action_set].is_color_dependent) {
                           soc_mem_field32_set(unit, mem, act_prof_entry[0],
                                 action_set_ptr[field_action_set].hw_field, 1);
                       } else {
                           soc_mem_field32_set(unit, mem, act_prof_entry[1],
                               action_set_ptr[field_action_set].hw_field, 1);
                       }
                   } else {
                       soc_mem_field32_set(unit, mem, act_prof_entry[0],
                           action_set_ptr[field_action_set].hw_field, 1);
                   }

                   aset_key_size +=
                              action_set_ptr[field_action_set].size;
                   aset_cnt[field_action_set]++;
               }
            }
        }
    }
    /* Set CosorIntPriSet in action profile mem and
       update key size if prioPktAndInt* actions are
       present in current group's Aset */
    if (1 == setChangeCosOrIntPriSet) {
        if (0 == aset_cnt[_FieldActionChangeCosOrIntPriSet]) {
           aset_key_size +=
                   action_set_ptr[_FieldActionChangeCosOrIntPriSet].size;
           soc_mem_field32_set(unit, mem, act_prof_entry[0],
                action_set_ptr[_FieldActionChangeCosOrIntPriSet].hw_field, 1);
        }
    }
    /* Check if L3SW/Redirect action is set.
     * If yes then reserve 1 bit for Green To Pid. */
    if (aset_cnt[_FieldActionL3SwChangeL2Set] ||
        aset_cnt[_FieldActionRedirectSet]) {
        aset_key_size +=
                   action_set_ptr[_FieldActionGreenToPidSet].size;
        soc_mem_field32_set(unit, mem, act_prof_entry[0],
             action_set_ptr[_FieldActionGreenToPidSet].hw_field , 1);
    }

    /* Update Aset size in group's structure */
    fsm_ptr->aset_size = aset_key_size;

    /* for single wide group, throw e-resource error
     * if the aset size is more than 181b */
    if ((fg->stage_id == _BCM_FIELD_STAGE_INGRESS) &&
        (((fsm_ptr->aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_NARROW)
        && (fsm_ptr->mode == bcmFieldGroupModeSingle))
        || ((fsm_ptr->aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE) &&
         (fsm_ptr->mode == bcmFieldGroupModeIntraSliceDouble)))) {
        /* ADD error msg here */
        LOG_ERROR(BSL_LS_BCM_FP,
             (BSL_META_U(unit, "FP(unit %d) Error: Aset size =%d is not"
             "supported in this group mode.\n"),
             unit, fsm_ptr->aset_size));

        fsm_ptr->rv = (BCM_E_CONFIG);
        goto clean_up;
    }

    entries[0] = act_prof_entry[0];
    /* Add Entries To Action Profile Table In Hardware. */
    fsm_ptr->rv = soc_profile_mem_add(unit,
                             &stage_fc->action_profile[fg->instance],
                             entries, 1, &action_profile_idx[0]);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        goto clean_up;
    }

    fg->action_profile_idx[0] = action_profile_idx[0];

    if (1 == multi_mode) {
        entries[0] = act_prof_entry[1];
        /* Add Entries To Action Profile Table In Hardware. */
        fsm_ptr->rv = soc_profile_mem_add(unit,
                             &stage_fc->action_profile[fg->instance],
                             entries, 1, &action_profile_idx[1]);
        fg->action_profile_idx[1] = action_profile_idx[1];
    }

    LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
            "VERB: groups aset size %d \n\r"),
             fsm_ptr->aset_size));

    clean_up:
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_qset_update
 * Purpose:
 *     Update application requested qset with internal qualifiers.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_qset_update(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t *fg; /* Field Group structure pointer. */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    if (NULL == fsm_ptr->fg) {
        fsm_ptr->rv = (BCM_E_PARAM);
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        return BCM_E_NONE;
    }

    /* Get field group structure. */
    fg = fsm_ptr->fg;

    /* Save current state as the previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

#ifdef BCM_FIREBOLT6_SUPPORT
    if (fg->flags & _FP_GROUP_COMPRESSED) {
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifySrcIp)) {
            BCM_FIELD_QSET_REMOVE(fg->qset, bcmFieldQualifySrcIp);
            BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifySrcIpv4CompressedClassId);
        }
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifySrcIp6)) {
            BCM_FIELD_QSET_REMOVE(fg->qset, bcmFieldQualifySrcIp6);
            BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifySrcIpv6CompressedClassId);
        }
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyDstIp)) {
            BCM_FIELD_QSET_REMOVE(fg->qset, bcmFieldQualifyDstIp);
            BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyDstIpv4CompressedClassId);
        }
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyDstIp6)) {
            BCM_FIELD_QSET_REMOVE(fg->qset, bcmFieldQualifyDstIp6);
            BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyDstIpv6CompressedClassId);
        }
        if ((BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifySrcIpv4CompressedClassId) &&
             BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifySrcIpv6CompressedClassId)) ||
            (BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifyDstIpv4CompressedClassId) &&
             BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifyDstIpv6CompressedClassId))) {
            /* Ipv4 and Ipv6 qualifiers cannot be present together */
            fsm_ptr->rv = BCM_E_PARAM;
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
            return BCM_E_NONE;
        }
        if (!BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifySrcIpv4CompressedClassId) &&
            !BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifySrcIpv6CompressedClassId) &&
            !BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifyDstIpv4CompressedClassId) &&
            !BCM_FIELD_QSET_TEST(fg->qset, _bcmFieldQualifyDstIpv6CompressedClassId)) {
            /* No compression eligible qualifiers present in group */
            fsm_ptr->rv = BCM_E_PARAM;
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
            return BCM_E_NONE;
        }
    }
#endif

    /* InPorts Qualifier is not supported in TD3. */
    if (soc_feature(unit, soc_feature_ifp_no_inports_support) &&
        BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPorts)) {
          LOG_WARN(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "InPorts Qualifier is not supported.\n\r")));
          fsm_ptr->rv = (BCM_E_PARAM);
          fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
          return BCM_E_NONE;
    }

#if defined(BCM_HELIX5_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyFlowtrackerCheck) &&
            (0 == BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyFlowtrackerGroupId))) {
           LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                      "(unit %d) Error: Qualifier FlowtrackerCheck needs "
                      " to be used qualifier FlowtrackerGroupId"),
                      unit));
           fsm_ptr->rv = BCM_E_CONFIG;
           fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
           return BCM_E_NONE;
        }
    }
#endif
    /*
     * In-case of Atomic update, InPorts qualifier is not supported
     * in Global mode.
     */
    if ((_BCM_FIELD_STAGE_INGRESS == fg->stage_id) &&
        (_BCM_FIELD_QSET_PBMP_TEST(fg->qset))) {
       _field_stage_t *stage_fc;   /* Stage Field control structure. */

       BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
                           &stage_fc));
       if ((soc_property_get(unit, spn_FIELD_ATOMIC_UPDATE, FALSE) == TRUE) &&
           (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal)) {
          LOG_WARN(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "InPorts Qualifier is not supported in Global mode"
              " incase of atomic update.\n\r")));
          fsm_ptr->rv = (BCM_E_PARAM);
          fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
          return BCM_E_NONE;
       }
    }

    /*
     * PBMP qualifiers conflict check.
     * Group QSET should not contain more than one PBMP qualifier.
     */
    if (_BCM_FIELD_QSET_PBMP_TEST(fg->qset)) {
       int pbm_qual;

       if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPorts)) {
          pbm_qual = bcmFieldQualifyInPorts;
          BCM_FIELD_QSET_REMOVE(fg->qset, pbm_qual);
       } else if (BCM_FIELD_QSET_TEST(fg->qset,
                                bcmFieldQualifyDevicePortBitmap)) {
          pbm_qual = bcmFieldQualifyDevicePortBitmap;
          BCM_FIELD_QSET_REMOVE(fg->qset, pbm_qual);
       } else if (BCM_FIELD_QSET_TEST(fg->qset,
                                bcmFieldQualifySystemPortBitmap)) {
          pbm_qual = bcmFieldQualifySystemPortBitmap;
          BCM_FIELD_QSET_REMOVE(fg->qset, pbm_qual);
       } else {
          pbm_qual = bcmFieldQualifySourceGportBitmap;
          BCM_FIELD_QSET_REMOVE(fg->qset, pbm_qual);
       }

       if (_BCM_FIELD_QSET_PBMP_TEST(fg->qset)) {
          LOG_WARN(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "More than one Port Bitmap Qualifier is not supported in a"
              " group.\n\r")));
          fsm_ptr->rv = (BCM_E_PARAM);
          fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
          BCM_FIELD_QSET_ADD(fg->qset, pbm_qual);
          return BCM_E_NONE;
       }

       BCM_FIELD_QSET_ADD(fg->qset, pbm_qual);
#if defined BCM_TRIDENT3_SUPPORT
       if (soc_feature(unit, soc_feature_td3_style_fp) &&
           (pbm_qual == bcmFieldQualifyInPorts)) {
          BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyInPorts_1);
          BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyInPorts_2);
       }
#endif /* BCM_TRIDENT3_SUPPORT */
    }

    /* All ingress devices implicitly have "bcmFieldQualifyStage" in QSETs. */
    if ((_BCM_FIELD_STAGE_INGRESS == fg->stage_id) ||
        (_BCM_FIELD_STAGE_EXACTMATCH == fg->stage_id) ||
        (_BCM_FIELD_STAGE_FLOWTRACKER == fg->stage_id)) {
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyStage);
    }

    /* Automatically include "IpType" if QSET contains Ip4 or Ip6 qualifier. */
    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)
        || BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyIpType);
    }

    /* Include "LogicalTableId", if preselectors are associated to the group. */
    if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) &&
        (fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER)) {
        BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyPreLogicalTableId);
    }

    if((fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) &&
        (soc_feature(unit, soc_feature_bscan_fae_port_flow_exclude))) {
        BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyInPort);
    }

    /* Update Exact Match Qset. */
    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchHitStatus)) {
        if ((!BCM_FIELD_QSET_TEST(fg->qset,
                              _bcmFieldQualifyExactMatchHitStatusLookup0)) &&
            (!BCM_FIELD_QSET_TEST(fg->qset,
                              _bcmFieldQualifyExactMatchHitStatusLookup1))) {
            BCM_FIELD_QSET_ADD(fg->qset,
                              _bcmFieldQualifyExactMatchHitStatusLookup0);
            if (!soc_feature(unit, soc_feature_th3_style_fp)) {
                BCM_FIELD_QSET_ADD(fg->qset,
                                   _bcmFieldQualifyExactMatchHitStatusLookup1);
            }
        }
    }

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchGroupClassId)) {
        if ((!BCM_FIELD_QSET_TEST(fg->qset,
                        _bcmFieldQualifyExactMatchGroupClassIdLookup0)) &&
            (!BCM_FIELD_QSET_TEST(fg->qset,
                        _bcmFieldQualifyExactMatchGroupClassIdLookup1))) {
            BCM_FIELD_QSET_ADD(fg->qset,
                    _bcmFieldQualifyExactMatchGroupClassIdLookup0);
            if (!soc_feature(unit, soc_feature_th3_style_fp)) {
                BCM_FIELD_QSET_ADD(fg->qset,
                                   _bcmFieldQualifyExactMatchGroupClassIdLookup1);
            }
        }
    }

    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyExactMatchActionClassId)) {
        if ((!BCM_FIELD_QSET_TEST(fg->qset,
                       _bcmFieldQualifyExactMatchActionClassIdLookup0)) &&
            (!BCM_FIELD_QSET_TEST(fg->qset,
                       _bcmFieldQualifyExactMatchActionClassIdLookup1))) {
            BCM_FIELD_QSET_ADD(fg->qset,
                    _bcmFieldQualifyExactMatchActionClassIdLookup0);
            if (!soc_feature(unit, soc_feature_th3_style_fp)) {
                BCM_FIELD_QSET_ADD(fg->qset,
                                   _bcmFieldQualifyExactMatchActionClassIdLookup1);
            }
        }
    }


    /*
     * NOTE:
     * Since IPBM field is only optionally included in the IFP TCAM lookup key,
     * InPorts qualifier is part of the QSET only when user explicitly adds it
     * to the Group's QSET. SDK does not include it by default as its done
     * in Pre-TH devices.
     */

    /* Proceed to Extractor Mux selector selection. */
    fsm_ptr->rv = (BCM_E_NONE);
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_ASET_VALIDATE;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_extractors_init
 * Purpose:
 *     Initialize field group extractor codes.
 * Parameters:
 *     unit      - (IN)     BCM unit number.
 *     fg        - (IN/OUT) Field Group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_extractors_init(int unit, _field_group_t *fg)
{
    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_NONE);
    }

    /*
     * Initialize group extractor code flags based on group mode being
     * created.
     */
    if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        fg->ext_codes[1].intraslice = _FP_EXT_SELCODE_DONT_USE;
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        fg->ext_codes[1].intraslice = _FP_EXT_SELCODE_DONT_USE;
        fg->ext_codes[2].intraslice = _FP_EXT_SELCODE_DONT_USE;
    }

    fg->ext_codes[0].secondary = _FP_EXT_SELCODE_DONT_USE;
    if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        fg->ext_codes[1].secondary = _FP_EXT_SELCODE_DONT_USE;
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        fg->ext_codes[1].secondary = _FP_EXT_SELCODE_DONT_USE;
        fg->ext_codes[2].secondary = _FP_EXT_SELCODE_DONT_USE;
    }

    return (BCM_E_NONE);
}


int
_field_th_group_qual_info_get(int unit,
                              _field_stage_t *stage_fc,
                              _field_group_t *fg,
                              bcm_field_qset_t *qset_req,
                              bcmi_keygen_qual_info_t **qual_info_arr,
                              uint16 *qual_info_count)
{
    uint16 idx = 0;
    uint16 qual_idx = 0;
    _bcm_field_internal_qualify_t qual_id = _bcmFieldQualifyCount;
    _field_hints_t *f_ht = NULL;
    _field_hint_t *hint_node = NULL;

    /* Input parameters check. */
    if (NULL == qual_info_arr || NULL == qset_req || NULL == fg ||
        NULL == qual_info_count || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    SHR_BIT_ITER(qset_req->w, BCM_FIELD_QUALIFY_MAX, idx) {
        if (stage_fc->qual_cfg_info_db->qual_cfg_info[idx] == NULL) {
            continue;
        }
        (*qual_info_count)++;
    }

    /* If No valid qualifiers, return */
    if ((*qual_info_count) == 0) {
        *qual_info_arr = NULL;
        return (BCM_E_NONE);
    }

    _FP_XGS3_ALLOC((*qual_info_arr),
                   sizeof(bcmi_keygen_qual_info_t) * (*qual_info_count),
                   "Keygen Qualifier Information");
    SHR_BIT_ITER(qset_req->w, BCM_FIELD_QUALIFY_MAX, idx) {
        if (stage_fc->qual_cfg_info_db->qual_cfg_info[idx] == NULL) {
            continue;
        }
        (*qual_info_arr)[qual_idx].qual_id = idx;
        qual_idx++;
    }

    if (!fg->hintid) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(_field_hints_control_get(unit, fg->hintid, &f_ht));

    for (qual_idx = 0; qual_idx < *qual_info_count; qual_idx++) {
        qual_id = (*qual_info_arr)[qual_idx].qual_id;
        if (NULL != f_ht) {
            if (NULL != f_ht->hints) {
                /* Go through all hints in HintId and get the bit map of
                 * qualifers with hints.
                 */
                hint_node = f_ht->hints;
                while (NULL != hint_node) {
                    if (bcmFieldQualifyUdf != hint_node->hint->qual) {
#if defined(BCM_HELIX5_SUPPORT)
                        if ((bcmFieldHintTypeExtraction == hint_node->hint->hint_type) &&
                                (qual_id == (_bcm_field_internal_qualify_t) hint_node->hint->qual) &&
                                ((qual_id == (_bcm_field_internal_qualify_t) bcmFieldQualifyDosAttackEvents) ||
                                 (qual_id == (_bcm_field_internal_qualify_t) bcmFieldQualifyInnerDosAttackEvents))) {
                            BCM_IF_ERROR_RETURN(
                                    _bcm_field_hx5_dosattack_event_qual_bitmap(unit,
                                        hint_node, &(*qual_info_arr)[qual_idx]));
                            hint_node = hint_node->next;
                            continue;
                        }
#endif
                        if (bcmFieldHintTypeExtraction == hint_node->hint->hint_type &&
                                (qual_id == (_bcm_field_internal_qualify_t)hint_node->hint->qual)) {
                            SHR_BITSET_RANGE((*qual_info_arr)[qual_idx].bitmap,
                                    hint_node->hint->start_bit,
                                    (hint_node->hint->end_bit -
                                     hint_node->hint->start_bit + 1));
                            (*qual_info_arr)[qual_idx].partial = TRUE;
                        }
                        hint_node = hint_node->next;
                    } else {
                        if ((bcmFieldHintTypeExtraction == hint_node->hint->hint_type) &&
                                (_FIELD_IS_INTERNAL_UDF_QUAL(qual_id))) {
                            BCM_IF_ERROR_RETURN(_bcm_th_qual_hint_bmp_get(unit,
                                        (int)qual_id, 0, hint_node,
                                        (*qual_info_arr)[qual_idx].bitmap, 1,
                                        qset_req, &(*qual_info_arr)[qual_idx]));
                        }
                        hint_node = hint_node->next;
                    }
                }
            }
        }
    }

    for (qual_idx = 0; qual_idx < *qual_info_count; qual_idx++) {
        qual_id = (*qual_info_arr)[qual_idx].qual_id;
        if (_FIELD_IS_INTERNAL_UDF_QUAL(qual_id)) {
            if((TRUE == (*qual_info_arr)[qual_idx].partial)
                    && (0 == (*qual_info_arr)[qual_idx].bitmap[0])) {
                SHR_BITCLR(qset_req->w, qual_id);
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_field_th_group_keygen_oper_insert(int unit,
                                   _field_group_t *fg,
                                   bcmi_keygen_oper_t *keygen_oper)
{
    int rv = BCM_E_NONE;
    uint8 part = 0;
    uint8 gran = 0;
    uint8 level = 0;
    uint8 ctrl_sel_val = 0;
    int parts_count = 0;
    uint16 ext_num = 0;
    uint16 ext_code_idx = 0;
    uint16 qual_idx = 0;
    uint16 offset = 0;
    uint16 num_offsets = 0;
    _field_ext_sel_t *grp_ext_code = NULL;
    _bcm_field_group_qual_t *group_qual = NULL;
    bcmi_keygen_qual_offset_info_t *qual_offset_info = NULL;
    bcmi_keygen_ext_ctrl_sel_info_t *keygen_ext_code = NULL;

    /* Get number of entry parts based on the field group flags. */
    BCM_IF_ERROR_RETURN(
        _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count));

    for (part = 0; part < parts_count; part++) {
        if (keygen_oper->qual_offset_info == NULL) {
            break;
        }
        group_qual = &(fg->qual_arr[0][part]);
        qual_offset_info = &(keygen_oper->qual_offset_info[part]);
        group_qual->size = qual_offset_info->size;
        /* Alloc larger mem since _bcmFieldInternalQualQid is wrongly set to 4.*/
        _FP_XGS3_ALLOC(group_qual->qid_arr,
                       qual_offset_info->size * sizeof(uint32),
                       "Group Qualifer Information");
        _FP_XGS3_ALLOC(group_qual->offset_arr,
                       qual_offset_info->size * sizeof(_bcm_field_qual_offset_t),
                       "Group Qualifer Offset Information");
        for (qual_idx = 0; qual_idx < qual_offset_info->size; qual_idx++) {
            group_qual->qid_arr[qual_idx] =
                qual_offset_info->qid_arr[qual_idx];
            group_qual->offset_arr[qual_idx].field = KEYf;
            num_offsets = 0;
            for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
                 if (!qual_offset_info->offset_arr[qual_idx].width[offset]) {
                     continue;
                 }
                 group_qual->offset_arr[qual_idx].offset[offset] =
                     qual_offset_info->offset_arr[qual_idx].offset[offset];
                 group_qual->offset_arr[qual_idx].width[offset] =
                     qual_offset_info->offset_arr[qual_idx].width[offset];
                 num_offsets++;
                 if (num_offsets ==
                     qual_offset_info->offset_arr[qual_idx].num_offsets) {
                     break;
                 }
            }
            group_qual->offset_arr[qual_idx].num_offsets = (offset + 1);
        }
    }

    for (ext_code_idx = 0;
        ext_code_idx < keygen_oper->ext_ctrl_sel_info_count;
        ext_code_idx++) {
        keygen_ext_code = &(keygen_oper->ext_ctrl_sel_info[ext_code_idx]);
        part = keygen_ext_code->part;
        gran = keygen_ext_code->gran;
        level = keygen_ext_code->level;
        ext_num = keygen_ext_code->ext_num;
        ctrl_sel_val = keygen_ext_code->ctrl_sel_val;
        grp_ext_code = &(fg->ext_codes[part]);
        if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER) {
            if (level == 1) {
                if (gran == 32) {
                    grp_ext_code->l1_e32_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 16) {
                    grp_ext_code->l1_e16_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 8) {
                    grp_ext_code->l1_e8_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 4) {
                    grp_ext_code->l1_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l1_e2_sel[ext_num] = ctrl_sel_val;
                } else {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
            } else if (level == 2) {
                if (gran == 16) {
                    grp_ext_code->l2_e16_sel[ext_num] = ctrl_sel_val;
                }
#if defined(BCM_TOMAHAWK3_SUPPORT)
                else if (gran == 4) {
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        grp_ext_code->l2_e4_sel[ext_num] = ctrl_sel_val;
                    } else {
                        rv = BCM_E_INTERNAL;
                        goto cleanup;
                    }
                } else if (gran == 2) {
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        grp_ext_code->l2_e2_sel[ext_num] = ctrl_sel_val;
                    } else {
                        rv = BCM_E_INTERNAL;
                        goto cleanup;
                    }
                } else if (gran == 1) {
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        grp_ext_code->l2_e1_sel[ext_num] = ctrl_sel_val;
                    } else {
                        rv = BCM_E_INTERNAL;
                        goto cleanup;
                    }
                }
#endif
                else {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
            } else if (level == 3) {
                if (gran == 4) {
                    grp_ext_code->l3_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l3_e2_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 1) {
                    grp_ext_code->l3_e1_sel[ext_num] = ctrl_sel_val;
                } else {
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
                }
            } else {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A) {
            grp_ext_code->aux_tag_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B) {
            grp_ext_code->aux_tag_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C) {
            grp_ext_code->aux_tag_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D) {
            grp_ext_code->aux_tag_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN) {
            grp_ext_code->tcp_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN) {
            grp_ext_code->tos_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN) {
            grp_ext_code->ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A) {
            grp_ext_code->class_id_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B) {
            grp_ext_code->class_id_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C) {
            grp_ext_code->class_id_cont_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D) {
            grp_ext_code->class_id_cont_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A) {
            grp_ext_code->src_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B) {
            grp_ext_code->src_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_DST_A) {
            grp_ext_code->dst_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_DST_B) {
            grp_ext_code->dst_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0) {
            grp_ext_code->src_dest_cont_0_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1) {
            grp_ext_code->src_dest_cont_1_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            grp_ext_code->ipbm_present = 1;
            grp_ext_code->ipbm_source = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM) {
            grp_ext_code->normalize_l3_l4_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_MAC_NORM) {
            grp_ext_code->normalize_mac_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_UDF) {
            grp_ext_code->udf_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_PMUX) {
            grp_ext_code->pmux_sel[ctrl_sel_val] = 1;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN) {
            grp_ext_code->alt_ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_LTID) {
            continue;
        } else {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
    }

    for (part = 0; part < parts_count; part++) {
        fg->ext_codes[part].normalize_l3_l4_sel =
            BCM_FIELD_QSET_TEST(fg->qset,
                bcmFieldQualifyNormalizeIpAddrs) ? 1 : 0;
        fg->ext_codes[part].normalize_mac_sel =
            BCM_FIELD_QSET_TEST(fg->qset,
                bcmFieldQualifyNormalizeMacAddrs) ? 1 : 0;
    }


    if ((BCM_FIELD_QSET_TEST(fg->qset,
        _bcmFieldQualifyPreLogicalTableId)) &&
        ((fg->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) ||
         (fg->stage_id == _BCM_FIELD_STAGE_INGRESS))) {
        if (SOC_IS_TOMAHAWK(unit)
        #if defined(BCM_TOMAHAWK2_SUPPORT)
             || SOC_IS_TOMAHAWK2(unit)
        #endif
        #if defined(BCM_TRIDENT3_SUPPORT)
            || (soc_feature(unit, soc_feature_td3_style_fp))
        #endif
            ) {
            fg->ext_codes[0].l1_e8_sel[6]=5;
            fg->ext_codes[0].l3_e4_sel[20]=36;
            fg->ext_codes[0].l3_e2_sel[1]=74;
        }
#if defined(BCM_TOMAHAWK3_SUPPORT)
        else if (SOC_IS_TOMAHAWK3(unit)) {
            fg->ext_codes[0].l1_e8_sel[5]=12;
            fg->ext_codes[0].l2_e4_sel[11]=46;
        }
#endif
    }

    return rv;

cleanup:
    for (part = 0; part < parts_count; part++) {
        group_qual = &(fg->qual_arr[0][part]);
        if (group_qual->qid_arr != NULL) {
            sal_free(group_qual->qid_arr);
        }
        if (group_qual->offset_arr != NULL) {
            sal_free(group_qual->offset_arr);
        }
    }
    return rv;
}

STATIC int
_field_th_keygen_ext_ctrl_info_count_get(int unit,
                                  _field_group_t *fg,
                                  uint16 *ext_ctrl_info_count)
{
    uint8 part = 0;
    int parts_count = 0;
    uint8 idx = 0;

    /* Input parameters check. */
    if (NULL == fg || NULL == ext_ctrl_info_count) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts based on the field group flags. */
    BCM_IF_ERROR_RETURN(
        _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count));
    *ext_ctrl_info_count = 0;
    for (part = 0; part < parts_count; part++) {
        if (fg->ext_codes[part].aux_tag_a_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].aux_tag_b_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].aux_tag_c_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].aux_tag_d_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].tcp_fn_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].ttl_fn_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].tos_fn_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].class_id_cont_a_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].class_id_cont_b_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].class_id_cont_c_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].class_id_cont_d_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].src_cont_a_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].src_cont_b_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].src_dest_cont_0_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].src_dest_cont_1_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].ipbm_present != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].normalize_l3_l4_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        if (fg->ext_codes[part].normalize_mac_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
        for (idx = 0; idx < 15; idx++) {
            if (fg->ext_codes[part].pmux_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 4; idx++) {
            if (fg->ext_codes[part].l1_e32_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 7; idx++) {
            if (fg->ext_codes[part].l1_e16_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 7; idx++) {
            if (fg->ext_codes[part].l1_e8_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 8; idx++) {
            if (fg->ext_codes[part].l1_e4_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 8; idx++) {
            if (fg->ext_codes[part].l1_e2_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 10; idx++) {
            if (fg->ext_codes[part].l2_e16_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 16; idx++) {
            if (fg->ext_codes[part].l2_e4_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 7; idx++) {
            if (fg->ext_codes[part].l2_e2_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 2; idx++) {
            if (fg->ext_codes[part].l2_e1_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 21; idx++) {
            if (fg->ext_codes[part].l3_e4_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 5; idx++) {
            if (fg->ext_codes[part].l3_e2_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        for (idx = 0; idx < 2; idx++) {
            if (fg->ext_codes[part].l3_e1_sel[idx] != -1) {
                (*ext_ctrl_info_count)++;
            }
        }
        if (fg->ext_codes[part].alt_ttl_fn_sel != -1) {
            (*ext_ctrl_info_count)++;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_field_th_keygen_ext_ctrl_info_get(int unit,
                               _field_group_t *fg,
                               bcmi_keygen_cfg_t *keygen_cfg)
{
    uint8 part = 0;
    int parts_count = 0;
    uint16 idx = 0;
    uint16 ext_idx = 0;
    uint16 ext_ctrl_info_count = 0;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;

    /* Input parameters check. */
    if (NULL == fg || NULL == keygen_cfg) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts based on the field group flags. */
    BCM_IF_ERROR_RETURN(
        _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count));
    BCM_IF_ERROR_RETURN(
        _field_th_keygen_ext_ctrl_info_count_get(unit, fg, &ext_ctrl_info_count));
    if (ext_ctrl_info_count == 0) {
        return BCM_E_NONE;
    }
    _FP_XGS3_ALLOC(ext_ctrl_info,
             ext_ctrl_info_count * sizeof(bcmi_keygen_ext_ctrl_sel_info_t),
             "Keygen Extractor control information");
    if (ext_ctrl_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(ext_ctrl_info, 0,
        ext_ctrl_info_count * sizeof(bcmi_keygen_ext_ctrl_sel_info_t));
    keygen_cfg->ext_ctrl_sel_info_count = ext_ctrl_info_count;
    keygen_cfg->ext_ctrl_sel_info = ext_ctrl_info;
    for (part = 0; part < parts_count; part++) {
        if (fg->ext_codes[part].aux_tag_a_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].aux_tag_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].aux_tag_b_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].aux_tag_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].aux_tag_c_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].aux_tag_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].aux_tag_d_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].aux_tag_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].tcp_fn_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_TCP_FN;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].tcp_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].ttl_fn_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].ttl_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].tos_fn_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_TOS_FN;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].tos_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].class_id_cont_a_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_A;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].class_id_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].class_id_cont_b_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_B;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].class_id_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].class_id_cont_c_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_C;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].class_id_cont_c_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].class_id_cont_d_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_CLASSID_D;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].class_id_cont_d_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].src_cont_a_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_A;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].src_cont_a_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].src_cont_b_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_B;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].src_cont_b_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].src_dest_cont_0_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_0;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].src_dest_cont_0_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].src_dest_cont_1_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_SRC_DST_1;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].src_dest_cont_1_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].ipbm_present == 1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].ipbm_source;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].normalize_l3_l4_sel == 1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].normalize_l3_l4_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        if (fg->ext_codes[part].normalize_mac_sel == 1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_MAC_NORM;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].normalize_mac_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
        for (ext_idx = 0; ext_idx < 15; ext_idx++) {
            if (fg->ext_codes[part].pmux_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_PMUX;
                ext_ctrl_info[idx].ctrl_sel_val = ext_idx;
                ext_ctrl_info[idx].part = part;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 4; ext_idx++) {
            if (fg->ext_codes[part].l1_e32_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l1_e32_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 32;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 7; ext_idx++) {
            if (fg->ext_codes[part].l1_e16_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l1_e16_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 16;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 7; ext_idx++) {
            if (fg->ext_codes[part].l1_e8_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l1_e8_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 8;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 8; ext_idx++) {
            if (fg->ext_codes[part].l1_e4_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l1_e4_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 4;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 8; ext_idx++) {
            if (fg->ext_codes[part].l1_e2_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l1_e2_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 1;
                ext_ctrl_info[idx].gran = 2;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 10; ext_idx++) {
            if (fg->ext_codes[part].l2_e16_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l2_e16_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 2;
                ext_ctrl_info[idx].gran = 16;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 16; ext_idx++) {
            if (fg->ext_codes[part].l2_e4_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l2_e4_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 2;
                ext_ctrl_info[idx].gran = 4;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 7; ext_idx++) {
            if (fg->ext_codes[part].l2_e2_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l2_e2_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 2;
                ext_ctrl_info[idx].gran = 2;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 2; ext_idx++) {
            if (fg->ext_codes[part].l2_e1_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l2_e1_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 2;
                ext_ctrl_info[idx].gran = 1;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 21; ext_idx++) {
            if (fg->ext_codes[part].l3_e4_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l3_e4_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 3;
                ext_ctrl_info[idx].gran = 4;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 5; ext_idx++) {
            if (fg->ext_codes[part].l3_e2_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                          fg->ext_codes[part].l3_e2_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 3;
                ext_ctrl_info[idx].gran = 2;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        for (ext_idx = 0; ext_idx < 2; ext_idx++) {
            if (fg->ext_codes[part].l3_e1_sel[ext_idx] != -1) {
                ext_ctrl_info[idx].ctrl_sel =
                                          BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[idx].ctrl_sel_val =
                                        fg->ext_codes[part].l3_e1_sel[ext_idx];
                ext_ctrl_info[idx].part = part;
                ext_ctrl_info[idx].level = 3;
                ext_ctrl_info[idx].gran = 1;
                ext_ctrl_info[idx].ext_num = ext_idx;
                idx++;
            }
        }
        if (fg->ext_codes[part].alt_ttl_fn_sel != -1) {
            ext_ctrl_info[idx].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN;
            ext_ctrl_info[idx].ctrl_sel_val = fg->ext_codes[part].alt_ttl_fn_sel;
            ext_ctrl_info[idx].part = part;
            idx++;
        }
    }
    return BCM_E_NONE;
}

#define IS_IBUS_CONT_QSET(_qset_) \
            (BCM_FIELD_QSET_TEST(_qset_, bcmFieldQualifyIncomingOuterVlanId)) \

/*
 * Function:
 *     _field_th_ingress_ext_code_get
 * Purpose:
 *     Finds extractor select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN)     BCM unit number.
 *     stage_fc  - (IN)     Stage Field control structure.
 *     qset_req  - (IN)     Client qualifier set.
 *     fg        - (IN/OUT) Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_ext_code_get(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    int rv = BCM_E_NONE;
    int parts_count = 0;
    uint16 rkey_size = 0;
    uint16 qual_info_count = 0;
    bcmi_keygen_mode_t keygen_mode;
    bcmi_keygen_mode_t keygen_ext_mode;
    bcmi_keygen_cfg_t keygen_cfg;
    bcmi_keygen_oper_t keygen_oper;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    _field_group_t *fg;
    _field_stage_t *stage_fc;
    bcm_field_qset_t *qset_req;
#if defined (BCM_TRIDENT3_SUPPORT)
    int ct = 0;
    uint8 bkup_ct = 0;
    uint8 num_qual_cfg_bkup[2] = {0};
    bcm_field_qualify_t qual_bkup[2];
    bcmi_keygen_qual_cfg_t *qual_cfg_arr_bkup[2] = {NULL};
#endif /* BCM_TRIDENT3_SUPPORT */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    fg = fsm_ptr->fg;
    stage_fc = fsm_ptr->stage_fc;
    qset_req = &(fg->qset);
    /* Initialize group extractor codes based on group flags. */
    rv = _field_th_group_extractors_init(unit, fg);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));
    sal_memset(&keygen_oper, 0, sizeof(bcmi_keygen_oper_t));

    /* Get number of entry parts based on the field group flags. */
    BCM_IF_ERROR_RETURN(
        _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count));

    if (soc_feature(unit, soc_feature_fp_ltid_match_all_parts) &&
        (fg->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
       if (parts_count > 1) {
           BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyPreLogicalTableId1);
           if (parts_count == 3) {
               BCM_FIELD_QSET_ADD(fg->qset, _bcmFieldQualifyPreLogicalTableId2);
           }
       }
    }

    rv = _field_th_group_qual_info_get(unit, stage_fc, fg, qset_req,
                             &qual_info_arr, &qual_info_count);
    BCM_IF_ERROR_RETURN(rv);

    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE &&
        !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
        keygen_ext_mode = BCMI_KEYGEN_MODE_DBLINTRA;
    } else if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE &&
               fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
        if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
            if (fg->em_mode == _FieldExactMatchMode160) {
                keygen_mode = BCMI_KEYGEN_MODE_SINGLE_ASET_NARROW;
            } else if (fg->em_mode == _FieldExactMatchMode128) {
                keygen_mode = BCMI_KEYGEN_MODE_SINGLE_ASET_WIDE;
            } else {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
        } else {
            keygen_mode = BCMI_KEYGEN_MODE_DBLINTRA;
        }
        keygen_ext_mode = BCMI_KEYGEN_MODE_DBLINTRA;
    } else if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        keygen_mode = BCMI_KEYGEN_MODE_DBLINTER;
        keygen_ext_mode = BCMI_KEYGEN_MODE_DBLINTER;
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        keygen_mode = BCMI_KEYGEN_MODE_TRIPLE;
        keygen_ext_mode = BCMI_KEYGEN_MODE_TRIPLE;
    } else {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

#if defined (BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_fp)) {
       if (IS_IBUS_CONT_QSET(fsm_ptr->qset)) {
          if (BCM_FIELD_QSET_TEST(fsm_ptr->qset, bcmFieldQualifyIncomingOuterVlanId)) {
             qual_bkup[bkup_ct++] = bcmFieldQualifyIncomingOuterVlanId;
          }

          for (ct = 0; ct < bkup_ct; ct++) {
              num_qual_cfg_bkup[ct] =
                stage_fc->qual_cfg_info_db->qual_cfg_info[qual_bkup[ct]]->num_qual_cfg;
              qual_cfg_arr_bkup[ct] =
                stage_fc->qual_cfg_info_db->qual_cfg_info[qual_bkup[ct]]->qual_cfg_arr;

              rv = _bcm_field_qual_cfg_info_db_update(unit, fsm_ptr->fg->instance,
                                                      qual_bkup[ct], stage_fc);
              if (BCM_FAILURE(rv)) {
                 goto cleanup;
              }
          }
       }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    keygen_cfg.qual_info_arr = qual_info_arr;
    keygen_cfg.qual_info_count = qual_info_count;
    keygen_cfg.qual_cfg_info_db = stage_fc->qual_cfg_info_db;
    keygen_cfg.ext_cfg_db = stage_fc->ext_cfg_db_arr[keygen_ext_mode];
    keygen_cfg.mode = keygen_mode;
    keygen_cfg.num_parts = parts_count;
    if (fsm_ptr->flags & _FP_GROUP_CONFIG_VALIDATE) {
        keygen_cfg.flags = BCMI_KEYGEN_CFG_FLAGS_VALIDATE_ONLY;
    }
    if (fsm_ptr->flags & _FP_GROUP_QSET_UPDATE) {
        keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE;
        rv = _field_th_keygen_ext_ctrl_info_get(unit, fg, &keygen_cfg);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS &&
        !(fsm_ptr->flags & _FP_GROUP_QSET_UPDATE)) {
        rv = bcmi_keygen_keysize_get(unit, &keygen_cfg, &rkey_size);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        if (((keygen_mode == BCMI_KEYGEN_MODE_SINGLE) && (rkey_size > 80))
            || ((keygen_mode == BCMI_KEYGEN_MODE_DBLINTRA) && (rkey_size > 160))
            || ((keygen_mode == BCMI_KEYGEN_MODE_DBLINTER) && (rkey_size > 320))
            || ((keygen_mode == BCMI_KEYGEN_MODE_TRIPLE) && (rkey_size > 480))) {
            rv = BCM_E_CONFIG;
            goto cleanup;
        }
    }
    rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, &keygen_oper);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    if ((fsm_ptr->flags & _FP_GROUP_CONFIG_VALIDATE) == 0) {
        rv = _field_th_group_keygen_oper_insert(unit, fg, &keygen_oper);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

cleanup:
    if (qual_info_arr != NULL) {
        sal_free(qual_info_arr);
    }
    if (fsm_ptr->flags & _FP_GROUP_QSET_UPDATE) {
        if (keygen_cfg.ext_ctrl_sel_info != NULL) {
            sal_free(keygen_cfg.ext_ctrl_sel_info);
        }
    }

#if defined (BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_fp)) {
       if (bkup_ct != 0) {
          uint8 ct;
          for (ct = 0; ct < bkup_ct; ct++) {
             if (BCM_FAILURE(rv)) {
                stage_fc->qual_cfg_info_db->qual_cfg_info[qual_bkup[ct]]->qual_cfg_arr
                                   = qual_cfg_arr_bkup[ct];
                stage_fc->qual_cfg_info_db->qual_cfg_info[qual_bkup[ct]]->num_qual_cfg
                                   = num_qual_cfg_bkup[ct];
                BCM_IF_ERROR_RETURN
                    (_bcm_field_td3_field_bus_cont_sel_remove(unit,
                                              fsm_ptr->fg->instance,
                                              stage_fc, fg,
                                              qual_bkup[ct]));
             } else {
                sal_free(qual_cfg_arr_bkup[ct]);
             }
          }
       }
    }
#endif /* BCM_TRIDENT3_SUPPORT */ 
    (void)bcmi_keygen_oper_free(unit, parts_count, &keygen_oper);
    return rv;
}

/*
 * Function:
 *     _field_th_ext_code_assign
 *
 * Purpose:
 *     Calculate the extractor select codes from a qualifier set and group mode.
 *
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     qset           - (IN) Client qualifier set.
 *     selcode_clear  - (IN) Clear the selcodes
 *     fsm_ptr        - (IN/OUT) fsm pointer which has pointer to
 *                           group structure ans aset size
 *
 * Returns:
 *     BCM_E_PARAM    - mode unknown
 *     BCM_E_RESOURCE - No select code will satisfy qualifier set
 *     BCM_E_NONE     - Success
 *
 * Notes:
 *     Calling function is responsible for ensuring appropriate slices
 *     are available.
 *     selcode_clear will be 0 (don't clear) when this function is called
 *         from bcm_field_group_set
 */
int
_field_th_ext_code_assign(int unit,
                          int selcode_clear,
                          _field_group_add_fsm_t *fsm_ptr)
{
    int idx;                    /* slice index iterator.            */
    _field_group_t *fg;         /* Field Group structure            */

    /*retrieve the group structure */
    fg = fsm_ptr->fg;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /*
     * Clear group extractor selector codes.
     */
    for (idx = 0; idx < _FP_MAX_ENTRY_WIDTH; idx++) {
        if (selcode_clear) {
            _FP_EXT_SELCODE_CLEAR(fg->ext_codes[idx]);
        }
    }

    /* Get keygen extractor codes for group qset. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_ext_code_get(unit, fsm_ptr));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_extractor_codes_get
 * Purpose:
 *     Calculate the extractor select codes from a qualifier set and group mode.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_extractor_codes_get(int unit,
                                        _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t *fg;     /* Field Group pointer.     */
    int orig_group_flags;   /* Original Group flags.    */

    /* Input parameters check. */
    if (NULL == fsm_ptr || NULL == fsm_ptr->fg) {
        return (BCM_E_PARAM);
    }

    /* Check if requested mode is supported by device. */
    switch (fsm_ptr->mode) {
        case bcmFieldGroupModeQuad:
        case bcmFieldGroupModeDirect:
        case bcmFieldGroupModeDirectExtraction:
        case bcmFieldGroupModeHashing:
        case bcmFieldGroupModeExactMatch:
            /* Mode not supported by device. */
            fsm_ptr->rv = (BCM_E_CONFIG);
            break;
        case bcmFieldGroupModeTriple: /* 480 bit mode. */
            /* Check Triple wide mode group support in device feature list. */
            if (0 == soc_feature(unit, soc_feature_field_ingress_triple_wide)) {
                fsm_ptr->rv = (BCM_E_CONFIG);
            } else {
                if (fsm_ptr->stage_fc->stage_id
                                            == _BCM_FIELD_STAGE_EXACTMATCH) {
                    fsm_ptr->rv = (BCM_E_CONFIG);
                }
            }
            break;
        case bcmFieldGroupModeDouble: /* 320 bit mode. */
            /* Check if device supports Double wide mode. */
            if (0 == soc_feature(unit, soc_feature_field_wide)) {
                fsm_ptr->rv = (BCM_E_CONFIG);
            }
            break;
        case bcmFieldGroupModeIntraSliceDouble: /*160 bit mode */
            if (!(soc_feature(unit, soc_feature_ifp_no_narrow_mode_support) ||
                  soc_feature(unit, soc_feature_hx5_style_ifp_no_narrow_mode_support))) {
                /* Check if device supports intraslice Double wide mode. */
                if (0 == soc_feature(unit, soc_feature_field_intraslice_double_wide)) {
                      fsm_ptr->rv = (BCM_E_CONFIG);
                }
            } else {
                fsm_ptr->rv = (BCM_E_UNAVAIL);
            }
            break;
        case bcmFieldGroupModeSingle: /* 80 bit mode. */
            if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                if (!soc_feature(unit, soc_feature_ifp_no_narrow_mode_support) &&
                    !soc_feature(unit, soc_feature_hx5_style_ifp_no_narrow_mode_support)) {
                    if (_BCM_FIELD_QSET_PBMP_TEST(fsm_ptr->qset)) {
                        fsm_ptr->rv = (BCM_E_CONFIG);
                    }
                } else {
                    /* Single mode is 160 bit mode */
                    fsm_ptr->mode = bcmFieldGroupModeIntraSliceDouble;
                }
            }
            break;
        default:
            break;
    }

    /* If mode check returned failure, proceed to cleanup state. */
    if (BCM_FAILURE(fsm_ptr->rv)) {
        fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        return (BCM_E_NONE);
    }

    /* Group pointer initialization. */
    fg = fsm_ptr->fg;

    fsm_ptr->rv = BCM_E_RESOURCE;
    orig_group_flags = fg->flags;

    if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {

        if (bcmFieldGroupModeAuto == fsm_ptr->mode) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Group mode auto.\n")));

            /* For Auto mode, try Single -> Double -> Triple modes. */
            /* Single wide Non-Intra slice selection. */
            if (!soc_feature(unit, soc_feature_ifp_no_narrow_mode_support) &&
                 !soc_feature(unit, soc_feature_hx5_style_ifp_no_narrow_mode_support)) {
                if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY) &&
                        !_BCM_FIELD_QSET_PBMP_TEST(fsm_ptr->qset)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP,
                            (BSL_META_U(unit, "Trying single...\n")));
                    if (soc_feature(unit, soc_feature_ifp_action_profiling) &&
                        (fsm_ptr->aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_NARROW))
                    {
                        LOG_VERBOSE(BSL_LS_BCM_FP,
                            (BSL_META_U(unit, "When Aset size is %d, Single wide group is not possible\n"),
                            fsm_ptr->aset_size));
                        fsm_ptr->rv = BCM_E_RESOURCE;
                    } else {
                       /* 80bit mode. */
                       fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE);
                       fsm_ptr->rv = _field_th_ext_code_assign(unit, 1, fsm_ptr);
                    }
                }
            }

            /* Single wide intra slice selection. */
            if ((BCM_FAILURE(fsm_ptr->rv)
                        && (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE))) {
                if (soc_feature(unit, soc_feature_field_intraslice_double_wide)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying intra...\n")));
                    if (soc_feature(unit, soc_feature_ifp_action_profiling) &&
                        (fsm_ptr->aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE))
                    {
                        LOG_VERBOSE(BSL_LS_BCM_FP,
                            (BSL_META_U(unit, "When Aset size is %d, Intra Slice Double wide"
                                " group is not possible\n"), fsm_ptr->aset_size));
                        fsm_ptr->rv = BCM_E_RESOURCE;
                    } else {
                        /* 160bit mode. */
                        fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                | _FP_GROUP_INTRASLICE_DOUBLEWIDE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, 1, fsm_ptr);
            }
                }
            }

            /* Double wide inter slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                if (BCM_FAILURE(fsm_ptr->rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying double...\n")));
                    /* 320bit mode. */
                    fg->flags = (orig_group_flags | _FP_GROUP_SPAN_DOUBLE_SLICE);
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, 1, fsm_ptr);
                }
            }

            /* Triple wide inter slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                if (BCM_FAILURE(fsm_ptr->rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying triple...\n")));
                    /* 480bit mode. */
                    fg->flags = (orig_group_flags | _FP_GROUP_SPAN_TRIPLE_SLICE);
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, 1, fsm_ptr);
                }
            }
        } else {
            switch (fsm_ptr->mode) {
                case bcmFieldGroupModeSingle:
                    /* Single wide non intra slice selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP,
                                (BSL_META_U(unit, "Trying single...\n")));
                        if (soc_feature(unit, soc_feature_ifp_action_profiling) &&
                            (fsm_ptr->aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_NARROW))
                        {
                            LOG_VERBOSE(BSL_LS_BCM_FP,(BSL_META_U(unit,
                                       "When Aset size is %d,Single wide group is not possible\n"),
                                        fsm_ptr->aset_size));
                            fsm_ptr->rv = BCM_E_RESOURCE;
                        }
                        else {
                            fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE);
                            fsm_ptr->rv = _field_th_ext_code_assign(unit, 1,
                                fsm_ptr);
                        }
                    }
                    break;
                case bcmFieldGroupModeIntraSliceDouble:
                    /* Double wide intra slice selection. */
                    if ((soc_feature(unit, soc_feature_field_intraslice_double_wide)
                                && (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE))) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying intra...\n")));
                        if (soc_feature(unit, soc_feature_ifp_action_profiling) &&
                            (fsm_ptr->aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE))
                        {
                            LOG_VERBOSE(BSL_LS_BCM_FP,
                                (BSL_META_U(unit, "When Aset size is %d, Intra Slice Double wide"
                                        " group is not possible\n"), fsm_ptr->aset_size));
                            fsm_ptr->rv = BCM_E_RESOURCE;
                        }
                        else {
                            fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                | _FP_GROUP_INTRASLICE_DOUBLEWIDE);
                            fsm_ptr->rv = _field_th_ext_code_assign(unit, 1,
                                fsm_ptr);
                       }
                    }
                break;
                case bcmFieldGroupModeDouble:
                    /* Double wide intra slice selection. */
                if (!soc_feature(unit, soc_feature_ifp_no_narrow_mode_support) &&
                        !soc_feature(unit, soc_feature_hx5_style_ifp_no_narrow_mode_support)) {
                        if ((soc_feature(unit, soc_feature_field_intraslice_double_wide)
                                    && (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE))) {
                            if (soc_feature(unit, soc_feature_ifp_action_profiling) &&
                                (fsm_ptr->aset_size > _FP_ASET_DATA_SIZE_FOR_POLICY_WIDE))
                            {
                                LOG_VERBOSE(BSL_LS_BCM_FP,
                                    (BSL_META_U(unit, "When Aset size is %d, Intra Slice Double wide"
                                        " group is not possible\n"), fsm_ptr->aset_size));
                                fsm_ptr->rv = BCM_E_RESOURCE;
                            } else {
                                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                                    "Trying intra...\n")));
                                    fg->flags = (orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                            | _FP_GROUP_INTRASLICE_DOUBLEWIDE);
                                    fsm_ptr->rv = _field_th_ext_code_assign(unit, 1,
                                            fsm_ptr);
                            }
                        }
                    }
                    /* Double wide inter slice selection. */
                    if ((BCM_FAILURE(fsm_ptr->rv))
                            && (0 == (fsm_ptr->flags
                                    & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY))) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying double...\n")));
                        fg->flags = (orig_group_flags |
                                _FP_GROUP_SPAN_DOUBLE_SLICE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, 1,
                                fsm_ptr);
                    }
                    break;
                case bcmFieldGroupModeTriple:
                    /* Triple wide inter slice selection. */
                    if (0 == (fsm_ptr->flags
                                & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying triple...\n")));
                        fg->flags = (orig_group_flags |
                                _FP_GROUP_SPAN_TRIPLE_SLICE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, 1,
                                fsm_ptr);
                    }
                    break;
                default:
                    fsm_ptr->rv = (BCM_E_CONFIG);
            }
        }
    } else if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {

        if (bcmFieldGroupModeAuto == fsm_ptr->mode) {

            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Group mode auto.\n")));

            /* Single wide intra slice selection. */
            if ((0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY))) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Trying single ...\n")));
                if (fsm_ptr->aset_size <= EM_MODE160_ACTION_DATA_SIZE) {
                    fg->em_mode = _FieldExactMatchMode160;
                    fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                                 | _FP_GROUP_INTRASLICE_DOUBLEWIDE;
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, 1, fsm_ptr);
                    fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
                } else if ((EM_MODE160_ACTION_DATA_SIZE < fsm_ptr->aset_size)
                    && (fsm_ptr->aset_size <= EM_MODE128_ACTION_DATA_SIZE)) {
                    fg->em_mode = _FieldExactMatchMode128;
                    fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                                 | _FP_GROUP_INTRASLICE_DOUBLEWIDE;
                    fsm_ptr->rv = _field_th_ext_code_assign(unit, 1, fsm_ptr);
                    fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
                }
            }

            /* Double wide inter slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                if (BCM_FAILURE(fsm_ptr->rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying double...\n")));
                    if (fsm_ptr->aset_size <= EM_MODE320_ACTION_DATA_SIZE) {
                        /* 320bit mode. */
                        fg->em_mode = _FieldExactMatchMode320;
                        fg->flags = (orig_group_flags |
                                          _FP_GROUP_SPAN_DOUBLE_SLICE);
                        fsm_ptr->rv = _field_th_ext_code_assign(unit,
                                          1, fsm_ptr);
                    }
                }
            }
        } else {
            switch (fsm_ptr->mode) {
                case bcmFieldGroupModeSingle:
                    /* Single Wide selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying intra...\n")));
                        fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE
                                                     | _FP_GROUP_INTRASLICE_DOUBLEWIDE;
                        fsm_ptr->rv = _field_th_ext_code_assign(unit, 1,
                                                                                fsm_ptr);
                        fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
                    }
                    break;
                case bcmFieldGroupModeDouble:
                    /* Double wide inter slice selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        if (fsm_ptr->aset_size <= EM_MODE320_ACTION_DATA_SIZE) {
                            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying double ..\n")));
                            fg->em_mode = _FieldExactMatchMode320;
                            fg->flags = (orig_group_flags |
                                          _FP_GROUP_SPAN_DOUBLE_SLICE);
                            fsm_ptr->rv = _field_th_ext_code_assign(unit, 1,
                                          fsm_ptr);
                        } else {
                            fsm_ptr->rv = (BCM_E_CONFIG);
                        }
                    }
                    break;
                default:
                    fsm_ptr->rv = (BCM_E_CONFIG);
            }
        }
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        if (bcmFieldGroupModeAuto == fsm_ptr->mode) {

            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Group mode auto.\n")));

            /* Single wide intra slice selection. */
            if ((0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY))) {
                LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Trying single ...\n")));
                /* 180Bit mode. */
                fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
                fsm_ptr->rv = _field_hx5_ft_ext_code_assign(unit, 1, fsm_ptr);
            }

            /* Double wide inter slice selection. */
            if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                if (BCM_FAILURE(fsm_ptr->rv)) {
                    LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                    "Trying double...\n")));
                    /* 360bit mode. */
                    fg->flags = (orig_group_flags |
                                 _FP_GROUP_SPAN_SINGLE_SLICE |
                                 _FP_GROUP_DW_DEPTH_EXPANDED);
                    fsm_ptr->rv = _field_hx5_ft_ext_code_assign(unit, 1, fsm_ptr);
                }
            }
        } else {
            switch (fsm_ptr->mode) {
                case bcmFieldGroupModeSingle:
                    /* Single Wide selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying Single...\n")));
                        fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
                        fsm_ptr->rv = _field_hx5_ft_ext_code_assign(unit, 1,
                                        fsm_ptr);
                        fg->flags = orig_group_flags | _FP_GROUP_SPAN_SINGLE_SLICE;
                    }
                    break;
                case bcmFieldGroupModeDouble:
                    /* Double wide selection. */
                    if (0 == (fsm_ptr->flags & _BCM_FP_GROUP_ADD_INTRA_SLICE_ONLY)) {
                        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                        "Trying double ..\n")));
                        fg->flags = (orig_group_flags |
                                    _FP_GROUP_SPAN_SINGLE_SLICE |
                                    _FP_GROUP_DW_DEPTH_EXPANDED);
                        fsm_ptr->rv = _field_hx5_ft_ext_code_assign(unit, 1,
                                        fsm_ptr);
                    }
                    break;
                default:
                    fsm_ptr->rv = (BCM_E_CONFIG);
            }
        }
#endif
    } else {
        return (BCM_E_INTERNAL);
    }

    if (BCM_FAILURE(fsm_ptr->rv)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit, "No success so far.\n")));

        /* No luck so far - try different alternative qset if possible. */
        if ((BCM_E_RESOURCE == fsm_ptr->rv)
             && (_BCM_FP_GROUP_ADD_STATE_QSET_UPDATE
                 == fsm_ptr->fsm_state_prev)) {
            fsm_ptr->rv = (BCM_E_NONE);
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE;
        }
    } else {

        /* Insert group into units group list. */
        if (BCM_SUCCESS(fsm_ptr->rv)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Configuration found...\n")));
            fsm_ptr->rv = _bcm_field_group_linked_list_insert(unit, fsm_ptr);
        }

        if (BCM_SUCCESS(fsm_ptr->rv)) {
            /* Success proceed to slice allocation. */
            if ((fsm_ptr->flags & _FP_GROUP_CONFIG_VALIDATE)) {
                fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
            } else {
                fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE;
            }
        } else {
            /* Failure proceed to cleanup. */
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        }
    }

    /* Set current state to be previous state. */
    fsm_ptr->fsm_state_prev = _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_qset_alternate
 * Purpose:
 *     Currently no alternate QSET identified for TH IFP qualifiers.
 *     Proceed to the next state in the state machine.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_qset_alternate(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Set current state to be previous state. */
    fsm_ptr->fsm_state_prev =  fsm_ptr->fsm_state;

    /* Alternate QSET is not available, proceed to end. */
    fsm_ptr->rv = (BCM_E_RESOURCE);
    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_lt_slice_validate
 * Purpose:
 *     Validate if the slice has enought free entries.
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     stage_fc      - (IN) Stage field control structure pointer.
 *     fg            - (IN) Field group structure pointer.
 *     slice_id      - (IN) Physical slice number.
 *     lt_fs         - (IN) Reference to LT Slice structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_th_group_lt_slice_validate(int unit,
                                  _field_stage_t *stage_fc,
                                  _field_group_t *fg,
                                  int slice_id,
                                  _field_lt_slice_t *lt_fs)
{
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       int            id;                  /* iterator.           */
       uint8          presel_ent_ct = 0;   /* Presel Entry count. */
       uint8          first_entry_flg = 0;
       uint8          atomicity_entry = 0;

       /* calculated the presel entries to be created */
       for (id = 0; id < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; id++) {
           if (fg->presel_ent_arr[id] != NULL) {
              presel_ent_ct++;
           }
       }

       if (lt_fs == NULL) {
          lt_fs = stage_fc->lt_slices[fg->instance] + slice_id;
          first_entry_flg = 1;
       }

       /*
        * To provide SW atomicity, an entry in the slice has to be kept
        * free to move other entries in the slice during priority configuration.
        * Hence, free entries has to be reduced to 1 during validation.
        */
       if (lt_fs->slice_flags & _BCM_FIELD_SLICE_SW_ATOMICITY_SUPPORT) {
          if (lt_fs->free_count == 0) {
             return BCM_E_INTERNAL;
          }
          atomicity_entry = 1;
       }

       if ((lt_fs->free_count - atomicity_entry) < presel_ent_ct) {
          if (first_entry_flg == 1) {
             LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "No free entries left in the LT slice[%d] to create [%d]"
                " Preselector entries.\n\r"),
                slice_id, presel_ent_ct));
             return (BCM_E_CONFIG);
          } else {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "ERROR: Can not create the requested %d Presel entries"
                " for the given group's priority[%d].\n\r"
                "Only %d free entries left in the LT slice[%d]\n\r"),
                presel_ent_ct, fg->priority,
               (lt_fs->free_count - atomicity_entry), slice_id));
             return (BCM_E_RESOURCE);
          }
       }
    } else {
       int             part;            /* Part iterator */
       int             parts_count = 0; /* Entry Parts */
       _field_slice_t  *fs;             /* Field slice structure pointer.*/

       /* Get number of entry parts based on the field group flags. */
       BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                          &parts_count));

       /* Validate slice LT_MAP status. */
       for (part = 0; part < parts_count; part++) {
          fs = stage_fc->slices[fg->instance] + slice_id + part;
          if (0 != fs->lt_map) {
              if (!((soc_feature(unit, soc_feature_sw_mmu_flush_rqe_operation)) &&
                  (fs->lt_map == 1))) {
                   LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                       "FP(unit %d) Verb: Slice=%d in-use.\n"),
                        unit, (slice_id + part)));
                   return (BCM_E_CONFIG);
              }
          }
       }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_field_th_priority_group_get_next
 * Purpose:
 *  Retrieve the next valid group pointer associated with the given priority.
 * Parameters:
 *  unit       - (IN   BCM device number.
 *  instance   - (IN)  Pipe to which group belongs to.
 *  stage_id   - (IN)  Stage (IFP/VFP/EFP) to which group belongs to.
 *  priority   - (IN)  Field Group Priority.
 *  curr_group - (IN)  Reference to current group index associated to group
 *                     priority.
 *  next_group - (OUT) Reference to next available group index associated to
 *                     the given priority.
 * Returns:
 *  BCM_E_XXX
 * Note:
 *  "*curr_group == NULL" to retrieve the first group associated with the
 *   given slice.
 */
int
_bcm_field_th_priority_group_get_next(int unit,
                                      int instance,
                                      _field_stage_id_t stage_id,
                                      int priority,
                                      _field_group_t **curr_group,
                                      _field_group_t **next_group)
{
    _field_group_t *fg = NULL;

    if (curr_group == NULL || next_group == NULL) {
        return BCM_E_PARAM;
    }

    if (*curr_group == NULL) {
        _field_control_t  *fc;     /* Field Control Structure */

        /* Get device Field control structure. */
        BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
        fg = fc->groups;
    } else {
        fg = (*curr_group)->next;
    }

    *next_group = NULL;
    for (; fg != NULL; fg = fg->next) {
        if ((instance != fg->instance) || (stage_id != fg->stage_id)) {
            continue;
        }

        if (fg->priority == priority) {
            *next_group = fg;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     _bcm_field_th_group_add_slice_validate
 * Purpose:
 *     Validate a candidate slice if it fits for a group.
 * Parameters:
 *     unit          - (IN  BCM device number.
 *     stage_fc      - (IN) State machine tracking structure.
 *     fg            - (IN) Field group structure.
 *     slice_id      - (IN) Candidate slice id.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_add_slice_validate(int unit,
                                       _field_stage_t *stage_fc,
                                       _field_group_t *fg,
                                       int slice_id)
{
    int                rv;                                /* Return status. */
    _field_slice_t     *fs_part[_FP_MAX_ENTRY_WIDTH] = {NULL}; /* Field slice
                                                                  structure
                                                                  pointer. */
    _field_group_t     *fg_ptr = NULL;
    _field_lt_slice_t  *lt_slice = NULL;

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Validate slice for exact match group. */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        return _field_th_exactmatch_slice_validate(unit, stage_fc,
                                                     fg, slice_id);
    }

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        return _field_hx5_ft_slice_validate(unit, stage_fc, fg, slice_id);
    }
#endif

    /*
     * If the slice belongs to an auto-expanded group and it is not the first
     * slice of that group, cannot use it.
     */
    if (stage_fc->slices[fg->instance][slice_id].prev != NULL) {
        return (BCM_E_CONFIG);
    }

    /*
     *  Skip the slice if group slice size is small  and
     *  slice is not of smaller size.
     */
    if ((fg->flags & _FP_GROUP_SELECT_SMALL_SLICE) &&
        (0 == (stage_fc->slices[fg->instance][slice_id].slice_flags &
               _BCM_FIELD_SLICE_SIZE_SMALL))) {
        return (BCM_E_CONFIG);
    }

    /*
     *  Skip the slice if group slice size is large  and
     *  slice is not of large size.
     */
    if ((fg->flags & _FP_GROUP_SELECT_LARGE_SLICE) &&
        (0 == (stage_fc->slices[fg->instance][slice_id].slice_flags &
               _BCM_FIELD_SLICE_SIZE_LARGE))) {
        return (BCM_E_CONFIG);
    }

    /*
     * Skip the slice if group is Intraslice and slice is not Intraslice
     * capable.
     */
    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)
        && !(stage_fc->slices[fg->instance][slice_id].slice_flags
            & _BCM_FIELD_SLICE_INTRASLICE_CAPABLE)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for IntraSliceDouble wide-mode.\n"),
             unit, slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * Requested 80bit single wide group mode. Check if slice is already
     * operating in 160bit mode and return error if that's true
     */
    fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
    if (((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
            && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
         && (fs_part[0]->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for Single wide-mode.\n"), unit,
             slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * Three TCAMs are grouped to give a memory macro of size "160b * 256 * 3".
     * Triple wide group is created using slices within the same memory macro.
     */
    if ((fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) && (slice_id % 3 != 0)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for Triple wide-mode.\n"), unit,
             slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * Double wide group is created using consecutive slices within a memory
     * macro. But consecutive slices across a memory macro boundary cannot be
     * paried to create a double wide group. Skip slices 2, 5, 8 and 11 from
     * allocating for Double Wide Group base slice.
     */
    if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) &&
        ((soc_feature(unit, soc_feature_utt_ver1_5)
          && soc_feature(unit, soc_feature_utt)) ?
        (5 == (slice_id % 6)) : (2 == (slice_id % 3)))) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: bad slice=%d for Double wide-mode.\n"), unit,
             slice_id));
        return (BCM_E_CONFIG);
    }

    /*
     * For slices 1, 4, 7 and 10 check if previous slice is configured for Double
     * wide mode. If TRUE, then skip current slice for Double wide groups.
     */
    if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) && (1 == (slice_id % 3))) {
        fs_part[0] = stage_fc->slices[fg->instance] + (slice_id - 1);
        if (fs_part[0]->group_flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: slice=%d is secondary slice of DoubleWide"
                " group.\n"), unit, slice_id));
            return (BCM_E_CONFIG);
        }

        /*
         * If previous slice is unused, then current slice will be used as
         * secondary slice for the double wide group, hence skip it.
         */
        if (0 == (fs_part[0]->group_flags & _FP_GROUP_SPAN_MASK)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: slice=%d accounted for DoubleWide"
                " group.\n"), unit, slice_id));
            return (BCM_E_CONFIG);
        }
    }

    /*
     * Validate slice current mode for double wide group's allocation
     * eligibility.
     */
    if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        if (stage_fc->tcam_slices <= (slice_id + 1)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Not enough resource slice=%d for"
                " Double wide-mode.\n"),
                 unit, slice_id));
            return (BCM_E_CONFIG);
        }
        fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
        fs_part[1] = stage_fc->slices[fg->instance] + (slice_id + 1);
        if (((fs_part[0]->group_flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                && !(fs_part[0]->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))
            || ((fs_part[1]->group_flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                && !(fs_part[1]->group_flags
                     & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
            /*
             * HW Limitation: Single wide mode slices (80-bit wide) in use
             * cannot be paired with another slice.
             */
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: bad slice=%d for Double wide-mode.\n"),
                 unit, slice_id));
            return (BCM_E_CONFIG);
        }
    }

    /* Validate slice for triple wide mode use. */
    if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        if (stage_fc->tcam_slices <= (slice_id + 2)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Not enough resource slice=%d for"
                " Triple wide-mode.\n"),
                 unit, slice_id));
            return (BCM_E_CONFIG);
        }
        fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
        fs_part[1] = stage_fc->slices[fg->instance] + slice_id + 1;
        fs_part[2] = stage_fc->slices[fg->instance] + slice_id + 2;
        /* Check whether the Slices have same characteristics or not */
        if (((fs_part[0]->group_flags != fs_part[1]->group_flags)
              || (fs_part[0]->group_flags != fs_part[2]->group_flags))
                && (0 == (fs_part[0]->group_flags & _FP_GROUP_SPAN_TRIPLE_SLICE))) {
            /*
             * HW Limitation: Single wide mode slices (80-bit wide) in use
             * cannot be paired with another slice.
             */
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: bad slice=%d for Triple-mode.\n"), unit,
                 slice_id));
            return (BCM_E_CONFIG);
        }
    }

    /* Initialize group auto expansion support status. */
    if (soc_feature(unit, soc_feature_field_virtual_slice_group)) {
        if (stage_fc->flags & _FP_STAGE_AUTO_EXPANSION) {
            fg->flags |= _FP_GROUP_SELECT_AUTO_EXPANSION;
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Verb: Group Auto Expansion status=%d.\n"),
                 unit, (fg->flags & _FP_GROUP_SELECT_AUTO_EXPANSION) ? 1: 0));
        }
    }

    /* Retrieve the existing groups associated with the Slice */
    rv = _bcm_field_slice_group_get_next(unit, fg->instance,
                                            fg->stage_id, slice_id,
                                            &fg_ptr, &fg_ptr);

    if (BCM_SUCCESS(rv)) {

        int mode;

        /* In case of auto-expansion, go for a next slice for the same group. */
        if (fg == fg_ptr) {
           return (BCM_E_CONFIG);
        }

        /* Compare the group priority with the existing group in the slice. */
        if (fg->priority != fg_ptr->priority) {
           LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Slice[%d] contains other groups with different priority.\n\r"),
              slice_id));
           return (BCM_E_CONFIG);
        }

        /* Allow two groups without preselectors and with same prioirty for
         *  backward comaptability. If two groups have same prioirty, that
         *  means prioirty b/w these two groups doesnot matter to the user.
         */
        if ((fg_ptr->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0 &&
            (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
            return BCM_E_CONFIG;
        }

       /*
        * Groups can't share the slice, if the existing group
        * doesn't support preselector
        */
        if ((fg_ptr->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Group creation failed, Already default group exists"
             " with the same priority[%d].\n\r"),
              fg_ptr->priority));
           return (BCM_E_PARAM);
        }

        if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) == 0) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "Group creation failed, can't create a group with the priority"
            " same as existing preselector group priority[%d]\n\r"),
              fg_ptr->priority));
           return (BCM_E_PARAM);
        }

        /* Compare the group modes */
        mode = _FP_GROUP_SPAN_SINGLE_SLICE | _FP_GROUP_SPAN_DOUBLE_SLICE |
               _FP_GROUP_SPAN_TRIPLE_SLICE | _FP_GROUP_INTRASLICE_DOUBLEWIDE;

        if ((fg->flags & mode) != (fg_ptr->flags & mode)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
             "Group creation failed, already existing group with the same"
             " priority[%d] is in different mode. Can't create same priority"
             " groups with different modes.\n\r"),
              slice_id));
           return (BCM_E_PARAM);
        }

        /* Update the LT Slice to which the group's
         * preselector entries are to be created. */
        lt_slice = fg_ptr->lt_slices;
    } else if (rv == BCM_E_NOT_FOUND) {
        fs_part[0] = stage_fc->slices[fg->instance] + slice_id;
        if (fs_part[0]->group_flags != 0x0) {
            /*
             *  If group_flags is not NULL, the slice must belongs to some
             *  other group and should be a part of double-wide or Triple-wide
             *  for the previous slice.
             */
            return BCM_E_CONFIG;
        }
    } else {
        return rv;
    }

    /*
     * To share slices with more than one field group, mutually exclusive
     * pre-selection rules must be setup in LT select TCAM. In legacy mode,
     * only a default pre-selection rule is installed per-slice. Hence slices
     * cannot be shared by multiple field groups. Validate slice LT mapping to
     * confirm it's available for new group. For wide mode group, all slices
     * should be available for group create to succeed.
     */
    BCM_IF_ERROR_RETURN(_field_th_group_lt_slice_validate(unit, stage_fc, fg,
                                                          slice_id, lt_slice));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_ingress_slice_mode_set
 * Purpose:
 *     Program group's slice mode in hardware.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN) Stage field control structure.
 *     slice_num    - (IN) Slice for which mode is to be set in HW.
 *     fg           - (IN) Field group structure pointer.
 *     clear        - (IN) Reset to hardware default value.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_slice_mode_set(int unit, _field_stage_t *stage_fc,
                                 uint8 slice_num, _field_group_t *fg,
                                 int clear)
{
    uint8 mode;                          /* Slice mode: 0-Narrow, 1-Wide.  */
    uint32 ifp_config;                   /* Register SW entry buffer.      */
    soc_reg_t ifp_config_reg = INVALIDr; /* Per-Pipe IFP config registers. */

    /* Input Parameter Check */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Do nothing for exact match stage. */
    if ((stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) ||
        (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER)) {
        return (BCM_E_NONE);
    }

    if (0 == clear) {
        /* Determine the slice mode based on group flags. */
        mode = (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                    && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 0 : 1;
    } else {
        /* HW reset default is "1 - Wide" mode. */
        mode = 1;
    }


    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            /* Get slice configuration from hardware. */
            BCM_IF_ERROR_RETURN(READ_IFP_CONFIGr(unit, slice_num, &ifp_config));

            /* Set slice mode value in register buffer. */
            if (!soc_feature(unit,
                             soc_feature_ifp_no_narrow_mode_support)) {
                soc_reg_field_set(unit, IFP_CONFIGr, &ifp_config,
                                  IFP_SLICE_MODEf,
                                  mode);
            }

            /* Write register value to hardware. */
            BCM_IF_ERROR_RETURN(WRITE_IFP_CONFIGr(unit, slice_num, ifp_config));
            break;

        case bcmFieldGroupOperModePipeLocal:
            BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                           IFP_CONFIGr,
                                                           fg->instance,
                                                           &ifp_config_reg));
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit,
                ifp_config_reg, REG_PORT_ANY, slice_num,
                &ifp_config));

            /* Set slice mode value in register buffer. */
            if (!soc_feature(unit,
                             soc_feature_ifp_no_narrow_mode_support)) {
                soc_reg_field_set(unit, ifp_config_reg, &ifp_config,
                    IFP_SLICE_MODEf, mode);
            }

            BCM_IF_ERROR_RETURN(soc_reg32_set(unit,
                ifp_config_reg, REG_PORT_ANY, slice_num,
                ifp_config));
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_dirty
 * Purpose:
 *     Check if logical table entry was modified after last installation
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     lt_f_ent - (IN)  Primary entry pointer.
 *     dirty    - (OUT) Entry dirty flag status.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_dirty(int unit, _field_lt_entry_t *lt_f_ent, int *dirty)
{
    int parts_count = 0;    /* Entry TCAM parts count.  */
    int idx;                /* Entry parts iterator.    */

    /* Input parameters check. */
    if (NULL == lt_f_ent || NULL == dirty) {
        return (BCM_E_PARAM);
    }

    /* Get number of TCAM parts based on group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        lt_f_ent->group->flags, &parts_count));

    for (idx = 0; idx < parts_count; idx++) {
        if (lt_f_ent[idx].flags & _FP_ENTRY_DIRTY) {
            break;
        }
    }

    /* Update dirty flags status based on part index value. */
    *dirty = (idx < parts_count) ? TRUE : FALSE;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_lt_entry_t_compare
 * Purpose:
 *      Compare logical table entry id in _field_lt_entry_t structure.
 * Parameters:
 *      a - (IN) first entry to compare.
 *      b - (IN) second entry to compare with.
 * Returns:
 *      a<=>b
 */
STATIC int
_field_lt_entry_t_compare(void *a, void *b)
{
    _field_lt_entry_t **first; /* First compared entry. */
    _field_lt_entry_t **second; /* Second compared entry. */

    first = (_field_lt_entry_t **)a;
    second = (_field_lt_entry_t **)b;

    if ((*first)->eid < (*second)->eid) {
        return (-1);
    } else if ((*first)->eid > (*second)->eid) {
        return (1);
    }
    return (0);
}

/*
 * Function:
 *     _field_th_entry_flags_to_tcam_part
 * Purpose:
 *     Each logical table field entry contains up to 3 TCAM entries. This
 *     routine maps sw entry flags to tcam entry (0-2).
 *     Note this is not a physical address in tcam.
 *     Single: 0
 *     Single & Intraslice Double: 0
 *     Paired: 0, 1
 *     Triple: 0, 1, 2
 * Parameters:
 *     unit        - (IN)  BCM device number.
 *     entry_flags - (IN)  Entry flags.
 *     group_flags - (IN)  Entry group flags.
 *     entry_part  - (OUT) Entry part (0-2)
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_INTERNAL   - Error
 */
int
_bcm_field_th_entry_flags_to_tcam_part(int unit, uint32 entry_flags,
                                       uint32 group_flags,
                                       uint8 *entry_part)
{
    /* Input parameter check. */
    if (NULL == entry_part) {
        return (BCM_E_PARAM);
    }

    if (entry_flags & _FP_ENTRY_PRIMARY) {
        *entry_part = 0;
    } else if (entry_flags & _FP_ENTRY_SECONDARY) {
        *entry_part = 1;
    } else if (entry_flags & _FP_ENTRY_TERTIARY) {
        *entry_part = 2;
    } else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_get
 * Purpose:
 *     Lookup a LT entry in Group's LT entries array for a given unit ID,
 *     Entry ID and entry flags.
 * Parmeters:
 *     unit     - (IN)  BCM device number.
 *     lt_eid   - (IN)  Logical table Entry ID.
 *     flags    - (IN)  Entry flags to match.
 *     lt_ent_p - (OUT) Entry lookup result.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_get(int unit, bcm_field_entry_t lt_eid, uint32 flags,
                       _field_lt_entry_t **lt_ent_p)
{
    _field_lt_entry_t target; /* LT entry lookup variable. */
    _field_lt_entry_t *lt_f_ent; /* LT entry lookup pointer. */
    _field_control_t *fc; /* Device Field control structure. */
    _field_group_t *fg; /* Field group structure. */
    uint8 entry_part = 0; /* Wide entry part number. */
    int idx; /* LT entry index. */

    /* Input parameters check. */
    if (NULL == lt_ent_p) {
        return (BCM_E_PARAM);
    }

    /* Initialize target logical table entry. */
    target.eid = lt_eid;
    lt_f_ent = &target;

    /* Get device Field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get field groups handle. */
    fg = fc->groups;

    while (NULL != fg) {
        /* Skip empty groups. */
        if (NULL == fg->lt_entry_arr) {
            fg = fg->next;
            continue;
        }

        /* Search in group entry array list.*/
        idx = _shr_bsearch(fg->lt_entry_arr, fg->lt_grp_status.entry_count,
                    sizeof(_field_lt_entry_t *), (void *)&lt_f_ent,
                    _field_lt_entry_t_compare);
        if (idx >= 0) {
            (void) _bcm_field_th_entry_flags_to_tcam_part(unit, flags,
                                   fg->flags, &entry_part);
            *lt_ent_p = fg->lt_entry_arr[idx] + entry_part;
            return (BCM_E_NONE);
        }
        fg = fg->next;
    }

    /* LT entry with lt_eid not found. */
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _field_th_lt_entry_get_by_id
 * Purpose:
 *     Lookup a complete Logical table FP rule(entry) from a unit & rule id
 *     choice.
 * Parmeters:
 *     unit         - (IN) BCM device number.
 *     lt_eid       - (IN) LT Entry id.
 *     lt_ent_arr   - (OUT) Entry lookup result array.
 *                          (Array of pointers to primary/secondary/teriary ...
 *                           parts of entry).
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_get_by_id(int unit, bcm_field_entry_t lt_eid,
                             _field_lt_entry_t **lt_ent_arr)
{
    int rv; /* Operation return status. */

    /* Input parameter check. */
    if (NULL == lt_ent_arr) {
        return (BCM_E_PARAM);
    }

    /* Initialize entry pointer array*/
    *lt_ent_arr = NULL;

    rv = _field_th_lt_entry_get(unit, lt_eid, _FP_ENTRY_PRIMARY, lt_ent_arr);
    return (rv);
}

/*
 * Function:
 *     _field_th_lt_entry_tcam_idx_get
 * Purpose:
 *     Get the TCAM index of an entry ID.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     lt_f_ent     - (IN) Pointer to field logical table entry structure.
 *     lt_fs        - (IN) Pointer to field logical table slice structure.
 *     tcam_idx     - (OUT) Entry tcam index.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_tcam_idx_get(int unit, _field_lt_entry_t *lt_f_ent,
                                _field_lt_slice_t *lt_fs, int *tcam_idx)
{
    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == tcam_idx || (NULL == lt_fs))) {
        return (BCM_E_PARAM);
    }

    /* Requested entry structure sanity check. */
    if ((NULL == lt_f_ent->group) || (NULL == lt_f_ent->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Calculate the TCAM index. */
    *tcam_idx = lt_fs->start_tcam_idx + lt_f_ent->index;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_tcam_mem_get
 * Purpose:
 *     Get logica table memory identifier based on stage group operational mode.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     lt_f_ent     - (IN)     Pointer to field logical table entry structure.
 *     lt_tcam_mem  - (IN/OUT) LT memory identifer pointer.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_tcam_mem_get(int unit, _field_stage_t *stage_fc,
                          _field_lt_entry_t *lt_f_ent,
                          soc_mem_t *lt_tcam_mem)
{
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_f_ent) || (NULL == lt_tcam_mem)) {
        return (BCM_E_PARAM);
    }

    /* Set LT selection memory identifier based on stage group oper mode. */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                *lt_tcam_mem = IFP_LOGICAL_TABLE_SELECTm;
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                *lt_tcam_mem = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        case bcmFieldGroupOperModePipeLocal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                       IFP_LOGICAL_TABLE_SELECTm,
                                                       lt_f_ent->group->instance,
                                                       lt_tcam_mem));
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                       EXACT_MATCH_LOGICAL_TABLE_SELECTm,
                                                       lt_f_ent->group->instance,
                                                       lt_tcam_mem));
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_tcam_entry_get
 * Purpose:
 *     Read an entry installed in hardware into SW entry buffer.
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     fg       - (IN)  Pointer to field group structure.
 *     lt_f_ent - (OUT) Pointer to field logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_tcam_entry_get(int unit, _field_group_t *fg,
                            _field_lt_entry_t *lt_f_ent)
{
    uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Entry buffer.    */
    _field_stage_t *stage_fc;     /* Stage field control structure. */
    int tcam_idx;                 /* TCAM index.                    */
    static soc_mem_t lt_tcam_mem; /* LT selection TCAM memory ID.   */
    int rv;                       /* Operation return status.       */

    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    /* Check if LT TCAM entry is already present in SW. */
    if (NULL != lt_f_ent->tcam.key) {
        return (BCM_E_NONE);
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get key and data field based on stage */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        /* Get key field size in logical table entry. */
        lt_f_ent->tcam.key_size = WORDS2BYTES(soc_mem_field_length(unit,
                    IFP_LOGICAL_TABLE_SELECTm, KEYf));

        /* Get data field size in logical table entry. */
        lt_f_ent->tcam.data_size = WORDS2BYTES(soc_mem_field_length(unit,
                    IFP_LOGICAL_TABLE_SELECTm, DATAf));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        /* Get key field size in logical table entry. */
        lt_f_ent->tcam.key_size = WORDS2BYTES(soc_mem_field_length(unit,
                    EXACT_MATCH_LOGICAL_TABLE_SELECTm, KEYf));

        /* Get data field size in logical table entry. */
        lt_f_ent->tcam.data_size = WORDS2BYTES(soc_mem_field_length(unit,
                    EXACT_MATCH_LOGICAL_TABLE_SELECTm, DATAf));
    } else {
        return (BCM_E_PARAM);
    }

    /* Allocate memory to store entry Key info. */
    _FP_XGS3_ALLOC(lt_f_ent->tcam.key, lt_f_ent->tcam.key_size,
                    "LT Key Alloc.");
    if (NULL == lt_f_ent->tcam.key) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    /* Allocate memory to store entry Mask info. */
    _FP_XGS3_ALLOC(lt_f_ent->tcam.mask, lt_f_ent->tcam.key_size,
                    "LT Mask Alloc.");
    if (NULL == lt_f_ent->tcam.mask) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    /* Allocate memory to store entry Data info. */
    _FP_XGS3_ALLOC(lt_f_ent->tcam.data, lt_f_ent->tcam.data_size,
                    "LT Data Alloc.");
    if (NULL == lt_f_ent->tcam.data) {
        rv = BCM_E_MEMORY;
        goto error;
    }

    /* Get entry TCAM index. */
    rv = _field_th_lt_entry_tcam_idx_get(unit, lt_f_ent, lt_f_ent->lt_fs,
                                         &tcam_idx);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Get logical table memory name based on the group operational mode. */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent,
        &lt_tcam_mem));

    /* Read entry part from hardware. */
    rv = soc_th_ifp_mem_read(unit, lt_tcam_mem, MEM_BLOCK_ANY, tcam_idx,
            tcam_entry);
    if (BCM_FAILURE(rv)) {
        goto error;
    }

    /* Get entry Key value. */
    soc_mem_field_get(unit, lt_tcam_mem, tcam_entry, KEYf,
        lt_f_ent->tcam.key);

    /* Get entry Mask value. */
    soc_mem_field_get(unit, lt_tcam_mem, tcam_entry, MASKf,
        lt_f_ent->tcam.mask);

    /* Get entry Data value. */
    soc_mem_field_get(unit, lt_tcam_mem, tcam_entry, DATAf,
        lt_f_ent->tcam.data);

    return (BCM_E_NONE);

error:
    if (lt_f_ent->tcam.key) {
        sal_free(lt_f_ent->tcam.key);
        lt_f_ent->tcam.key = NULL;
    }

    if (lt_f_ent->tcam.mask) {
        sal_free(lt_f_ent->tcam.mask);
        lt_f_ent->tcam.mask = NULL;
    }

    if (lt_f_ent->tcam.data) {
        sal_free(lt_f_ent->tcam.data);
        lt_f_ent->tcam.data = NULL;
    }

    return (rv);
}

/*
 * Function:
 *     _field_th_lt_default_entry_install
 * Purpose:
 *     Install a logical table entry in hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_entry - (IN) Field logical table entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_install(int unit, bcm_field_entry_t lt_entry)
{
    _field_control_t *fc;           /* Field control structure. */
    _field_lt_entry_t *lt_f_ent;    /* LT field entry.          */
    int rv;                         /* Operation return status. */
    int dirty;                      /* Field entry dirty flag.  */

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Lookup using entry ID and get the SW entry copy. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get(unit, lt_entry,
        _FP_ENTRY_PRIMARY, &lt_f_ent));

    /* Check if entry has been modified since last install in hardware. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_dirty(unit, lt_f_ent, &dirty));

    if (dirty) {
        /* Entry has changed, install in hardware. */
        rv = _field_th_lt_default_tcam_entry_install(unit, lt_entry, 0, 0, NULL);
    } else {
        /* No change and nothing to do, return success. */
        rv = BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *     _field_th_group_lt_entry_add
 * Purpose:
 *     Given stage, group instance, slice and entry offset in slice, calculate
 *     TCAM index.
 * Parameters:
 *     unit     - (IN)  BCM device number.
 *     fg       - (IN)  Field group structure.
 *     lt_f_ent - (OUT) Field logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_lt_entry_add(int unit, _field_group_t *fg,
                             _field_lt_entry_t *lt_f_ent)
{
    _field_lt_entry_t **lt_f_ent_arr;   /* Field group LT entry array. */
    int mem_sz;                         /* Allocation memory size. */
    int idx;                            /* Entry insertion index. */
    int tmp;                            /* Temporary iterator. */

    /* Input parameters check. */
    if (NULL == fg || NULL == lt_f_ent) {
        return (BCM_E_PARAM);
    }

    /* Verify if entry is already present in the group. */
    if (NULL != fg->lt_entry_arr) {
        idx = _shr_bsearch(fg->lt_entry_arr, fg->lt_grp_status.entry_count,
                    sizeof(_field_lt_entry_t *), &lt_f_ent,
                    _field_lt_entry_t_compare);
        if (idx >= 0) {
            return (BCM_E_NONE);
        }
    } else {
        idx = -1; /* Insert into 0th location. */
    }

    /* Check if group has enough room for new entry. */
    if ((fg->lt_grp_status.entry_count + 1)
        > (fg->lt_ent_blk_cnt * _FP_GROUP_ENTRY_ARR_BLOCK)) {
        mem_sz = _FP_GROUP_ENTRY_ARR_BLOCK * (fg->lt_ent_blk_cnt + 1)
                    * sizeof(_field_lt_entry_t *);
        lt_f_ent_arr = NULL;
        _FP_XGS3_ALLOC(lt_f_ent_arr, mem_sz, "field group LT entries array");
        if (NULL == lt_f_ent_arr) {
            return (BCM_E_MEMORY);
        }

        /* Copy original array to newly allocated one. */
        if (NULL != fg->lt_entry_arr) {
            mem_sz = _FP_GROUP_ENTRY_ARR_BLOCK * (fg->lt_ent_blk_cnt)
                        * sizeof(_field_lt_entry_t *);
            sal_memcpy(lt_f_ent_arr, fg->lt_entry_arr, mem_sz);
            sal_free(fg->lt_entry_arr);
        }

        fg->lt_entry_arr = lt_f_ent_arr;
        fg->lt_ent_blk_cnt++;
    }

    /* Check entry block exists to add the entry. */
    if (NULL != fg->lt_entry_arr) {
        /*
         * Make room for entry insertion.
         */
        idx = (((-1) * idx) - 1);
        tmp = fg->lt_grp_status.entry_count - 1;
        while (tmp >= idx) {
            fg->lt_entry_arr[tmp + 1] = fg->lt_entry_arr[tmp];
            tmp--;
        }
        fg->lt_entry_arr[idx] = lt_f_ent;
        fg->lt_grp_status.entry_count++;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_slice_offset_to_tcam_index
 * Purpose:
 *     Given stage, group instance, slice and entry offset in slice, calculate
 *     TCAM index.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Pointer to field entry structure.
 *     slice    - (IN) Entry slice number.
 *     slice_idx -(IN) Entry offset in the slice.
 *     tcam_idx - (OUT) Entry tcam index.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_slice_offset_to_tcam_index(int unit, _field_stage_t *stage_fc,
                                            int instance, int slice,
                                            int slice_idx, int *tcam_idx)
{
    _field_lt_slice_t *lt_fs; /* Logical table slice number. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == tcam_idx)) {
        return (BCM_E_PARAM);
    }

    /* Input parameters check. */
    if (instance < 0 || instance >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Get logical table slice structure. */
    lt_fs = stage_fc->lt_slices[instance] + slice;

    /*
     * Compute TCAM index using the slice start TCAM index and supplied
     * slice offset index.
     */
    *tcam_idx = lt_fs->start_tcam_idx + slice_idx;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_tcam_idx_to_slice_offset
 * Purpose:
 *     Given stage, group instance and tcam_index, calculate slice and entry
 *     offset in the slice.
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     stage_fc     - (IN) Stage field control structure.
 *     instance     - (IN) Field Group instance.
 *     tcam_idx     - (IN) Entry tcam index.
 *     slice        - (OUT) Entry slice number.
 *     slice_idx    - (OUT) Entry offset in the slice.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_tcam_idx_to_slice_offset(int unit, _field_stage_t *stage_fc,
                                          int instance, int tcam_idx,
                                          int *slice, int *slice_idx)
{
    _field_lt_slice_t *lt_fs;   /* Field Slice number.  */
    int idx;                    /* Slice iterator.      */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == slice_idx) || (NULL == slice)
        || (instance < 0 || instance > stage_fc->num_instances)) {
        return (BCM_E_PARAM);
    }

    for (idx = 0; idx < stage_fc->tcam_slices; idx++) {
        lt_fs = stage_fc->lt_slices[instance] + idx;
        if (tcam_idx < (lt_fs->start_tcam_idx + lt_fs->entry_count)) {
            *slice = idx;
            *slice_idx = tcam_idx - lt_fs->start_tcam_idx;
            break;
        }
    }

    /* TCAM index sanity check. */
    if (idx == stage_fc->tcam_slices) {
        return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_entry_part_tcam_idx_get
 *
 * Purpose:
 *     Given primary entry slice/idx calculate
 *     secondary/tertiary slice/index.
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     f_ent   - (IN) Field entry pointer.
 *     idx_pri - (IN) Primary entry tcam index.
 *     ent_part    - (IN) Entry part id.
 *     idx_out - (OUT) Entry part tcam index.
 * Returns
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_part_tcam_idx_get(int unit, _field_group_t *fg,
                                         uint32 idx_pri, uint8 ent_part,
                                         int *idx_out)
{
    uint8 slice_num;            /* Primary entry slice number.      */
    int pri_slice = -1;          /* Prmary entry slice number.       */
    int pri_index = -1;          /* Primary entry slice index.       */
    _field_stage_t *stage_fc;   /* Stage field control structure.   */
    _field_lt_slice_t *lt_fs;   /* Logical table slice structure.   */
    int rv;                     /* Operation return status.         */


    /* Input parameters check. */
    if (NULL == fg || NULL == idx_out) {
        return (BCM_E_PARAM);
    }

    /* Primary entry index. */
    if (0 == ent_part) {
        *idx_out = idx_pri;
        return (BCM_E_NONE);
    }

    /* Get Stage Field Control Structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /*
     * Get primary part entry slice number and slice offset given entry TCAM
     * index.
     */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_idx_to_slice_offset(unit,
        stage_fc, fg->instance, idx_pri, &pri_slice, &pri_index));

    /* Get entry part to slice number 0, 1, 2 */
    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(ent_part,
        fg->flags, &slice_num));

    /* Get slice descriptor structure. */
    lt_fs = stage_fc->lt_slices[fg->instance] + (pri_slice + slice_num);

    /* Get the TCAM index given slice number and slice entry offset. */
    rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
            fg->instance, lt_fs->slice_number, pri_index, idx_out);

    return (rv);
}

/*
 * Function:
 *    _field_th_lt_default_entry_phys_create
 * Purpose:
 *    Create a physical entry in a logical table slice.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN) Stage field control structure.
 *    lt_ent    - (IN) Logical table entry identifier.
 *    prio      - (IN) Logical table entry priority.
 *    lt_fs     - (IN) Logical table slice structure.
 *    fg        - (IN) Field group structure.
 *    ent_p     - (OUT) Allocated & initialized entry.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_phys_create(int unit, _field_stage_t *stage_fc,
                                   bcm_field_entry_t lt_ent, int prio,
                                   _field_lt_slice_t *lt_fs, _field_group_t *fg,
                                   _field_lt_entry_t **ent_p)
{
    _field_control_t *fc;   /* Field control structure.              */
    int idx;                /* Slice entries iteration index.        */
    int parts_count = -1;   /* Number of entry parts.                */
    int part_index;         /* Entry parts iterator.                 */
    int mem_sz;             /* Memory allocation size.               */
    _field_lt_entry_t *lt_f_ent = NULL; /* LT field entry structure. */
    int rv;                 /* Operation return status.              */
    int pri_tcam_idx = -1;  /* Primary entry TCAM index.             */
    int slice_num = -1;     /* Slice number.                         */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_fs) || (NULL == fg)
         || (NULL == ent_p)) {
        return (BCM_E_PARAM);
    }

    /* Check if free entries are available in the slice. */
    if (0 == lt_fs->free_count) {
        return (BCM_E_RESOURCE);
    }

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get number of entry parts based on group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /* Calcuate buffer size required for SW entry. */
    mem_sz = sizeof(_field_lt_entry_t) * parts_count;

    /* Allocate and zero memory for LT selection entry. */
    _FP_XGS3_ALLOC(lt_f_ent, mem_sz, "LT field entry");
    if (NULL == lt_f_ent) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: allocation failure for LT entry\n"), unit));
        return (BCM_E_MEMORY);
    }

    /* Get index for primary entry. */
    for (idx = 0; idx < lt_fs->entry_count; idx++) {
        if (NULL == lt_fs->entries[idx]) {
#if defined (BCM_TOMAHAWK3_SUPPORT)
            if ((soc_feature(unit, soc_feature_sw_mmu_flush_rqe_operation)) &&
                (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH)) {
                 if (NULL == lt_fs->p_entries[idx]) {
                     lt_f_ent->index = idx;
                     break;
                 }
            } else
#endif
            {
                 lt_f_ent->index = idx;
                 break;
            }
        }
    }

    /* Get entry TCAM index given slice number and entry offset in slice. */
    rv = _bcm_field_th_lt_slice_offset_to_tcam_index(unit, stage_fc,
            fg->instance, lt_fs->slice_number, lt_f_ent->index, &pri_tcam_idx);
    if (BCM_FAILURE(rv)) {
        sal_free(lt_f_ent);
        return (rv);
    }

    /* Fill entry primitives. */
    for (idx = 0; idx < parts_count; idx++) {
        lt_f_ent[idx].eid = lt_ent;
        lt_f_ent[idx].prio = prio;
        lt_f_ent[idx].group = fg;

        /* Get entry part flags. */
        rv = _bcm_field_th_tcam_part_to_entry_flags(unit, idx, fg->flags,
                &lt_f_ent[idx].flags);
        if (BCM_FAILURE(rv)) {
            sal_free(lt_f_ent);
            return rv;
        }

        /* Given primary entry TCAM index calculate entry part tcam index. */
        if (0 != idx) {
            rv = _bcm_field_th_lt_entry_part_tcam_idx_get(unit, lt_f_ent->group,
                                                pri_tcam_idx, idx, &part_index);
            if (BCM_FAILURE(rv)) {
                sal_free(lt_f_ent);
                return (rv);
            }

            /*
             * Given entry part TCAM index, determine the slice number and slice
             * offset index.
             */
            rv = _bcm_field_th_lt_tcam_idx_to_slice_offset(unit, stage_fc,
                    fg->instance, part_index, &slice_num,
                    (int *)&lt_f_ent[idx].index);
            if (BCM_FAILURE(rv)) {
                sal_free(lt_f_ent);
                return (rv);
            }
            lt_f_ent[idx].lt_fs = stage_fc->lt_slices[fg->instance] + slice_num;
        } else {
            /* Set entry slice. */
            lt_f_ent[idx].lt_fs = lt_fs;
        }

        /* Mark entry dirty. */
        lt_f_ent[idx].flags |= _FP_ENTRY_DIRTY;
    }

    /* Insert the entry into group entry array. */
    rv = _field_th_group_lt_entry_add(unit, fg, lt_f_ent);
    if (BCM_FAILURE(rv)) {
        sal_free(lt_f_ent);
        return (rv);
    }

    /* Update slice related parameters after succesful update */
    for (idx = 0; idx < parts_count; idx++) {
        /* Decrement slice free entry count for primary entries. */
        lt_f_ent[idx].lt_fs->free_count--;

        /* Assign entry to a slice. */
        lt_f_ent[idx].lt_fs->entries[lt_f_ent[idx].index] = lt_f_ent + idx;
    }

    /* Return allocated/filled entry structure to the caller. */
    *ent_p = lt_f_ent;
    return (rv);
}

/*
 * Function:
 *    _field_th_group_lt_entry_delete
 * Purpose:
 *    Delete an entry from group LT entries array.
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    fg       - (IN) Field group structure.
 *    lt_f_ent - (IN) Logical table entry structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_lt_entry_delete(int unit, _field_group_t *fg,
                                _field_lt_entry_t *lt_f_ent)
{
    int idx; /* Entry deletion index.   */
    int tmp; /* Temporary iterator.     */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Ensure group entry array is valid. */
    if (NULL == fg->lt_entry_arr) {
        return (BCM_E_INTERNAL);
    }

    /* Confirm entry is present in the group. */
    idx = _shr_bsearch(fg->lt_entry_arr, fg->lt_grp_status.entry_count,
                sizeof(_field_lt_entry_t *), &lt_f_ent,
                _field_lt_entry_t_compare);
    if (idx < 0) {
        return (BCM_E_NOT_FOUND);
    }

    /* Store array index where the entry was found. */
    tmp = idx;
    /*
     * Re-Pack the entries array to clear the last pointer due to entry deletion.
     */
    while (tmp < fg->lt_grp_status.entry_count - 1) {
        fg->lt_entry_arr[tmp] = fg->lt_entry_arr[tmp + 1];
        tmp++;
    }

    /* Decrement total valid entry count. */
    fg->lt_grp_status.entry_count--;

    /* Clear entry address stored in the last array index. */
    fg->lt_entry_arr[fg->lt_grp_status.entry_count] = NULL;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_lt_default_entry_phys_destroy
 * Purpose:
 *    Destroy a physical entry from a logical table slice.
 * Parameters:
 *    unit     - (IN) BCM device number.
 *    lt_f_ent - (IN) Logical table entry structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_phys_destroy(int unit, _field_lt_entry_t *lt_f_ent)
{
    _field_control_t *fc;       /* Device field control structure. */
    _field_lt_slice_t *lt_fs;   /* Logical table slice structure.  */
    _field_group_t *fg;         /* Field group structure.          */
    int parts_count = -1;       /* Entry parts count.              */
    int idx;                    /* Entry iterator index.           */
    uint8 slice_number;         /* Slice number.                   */
    int rv;                     /* Operation return status.        */

    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == lt_f_ent->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get entry slice information. */
    lt_fs = lt_f_ent->lt_fs;

    /* Get the group to which the entry belongs. */
    fg = lt_f_ent->group;

    /* Get entry parts count. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    for (idx = 0; idx < parts_count; idx++) {

        /* Free entry TCAM key buffer if valid. */
        if (NULL != lt_f_ent[idx].tcam.key) {
            sal_free(lt_f_ent[idx].tcam.key);
        }

        /* Free entry TCAM mask buffer if valid. */
        if (NULL != lt_f_ent[idx].tcam.mask) {
            sal_free(lt_f_ent[idx].tcam.mask);
        }

        /* Free entry Data mask buffer if valid. */
        if (NULL != lt_f_ent[idx].tcam.data) {
            sal_free(lt_f_ent[idx].tcam.data);
        }

        /* Get entry part slice number. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(idx,
            fg->flags, &slice_number));

        /* Remove entry pointer from the slice. */
        lt_fs[slice_number].entries[lt_f_ent[idx].index] = NULL;

        /* Increment slice unused entry count. */
        if (0 == (lt_f_ent[idx].flags & _FP_ENTRY_SECOND_HALF)) {
            lt_fs[slice_number].free_count++;
        }
    }

    /* Remove the entry from group entry array. */
    rv = _field_th_group_lt_entry_delete(unit, fg, lt_f_ent);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Error: Group entry delete Eid=%d.\n"),
             unit, lt_f_ent->eid));
    }

    /* Free entry memory. */
    sal_free(lt_f_ent);
    return (rv);
}

/*
 * Function:
 *    _field_th_lt_tcam_data_mem_get
 * Purpose:
 *    Get logical table data memory identifier based on stage group operational
 *    mode.
 * Parameters:
 *    unit          - (IN) BCM device number.
 *    stage_fc      - (IN) Stage field control structure.
 *    fg            - (IN) Field group control structure.
 *    lt_tcam_mem   - (IN) Logical table data view identifier.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_tcam_data_mem_get(int unit, _field_stage_t *stage_fc,
                                _field_group_t *fg,
                                soc_mem_t *lt_tcam_mem)
{
    /* Input parameters check. */
    if (NULL == lt_tcam_mem) {
        return (BCM_E_PARAM);
    }

    /*
     * Assign LT selection TCAM memory identifier based on
     * stage's group operational mode.
     */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                *lt_tcam_mem = IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm;
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                *lt_tcam_mem = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        case bcmFieldGroupOperModePipeLocal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                        IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm,
                                        fg->instance,
                                        lt_tcam_mem));
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                    EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm,
                                    fg->instance,
                                    lt_tcam_mem));
            } else {
                return (BCM_E_INTERNAL);
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_lt_entry_data_value_set
 * Purpose:
 *     Set LT entry DATA portion of an entry.
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     stage_fc    - (IN) Stage field control structure.
 *     fg          - (IN) Field group structure.
 *     index       - (IN) Entry part index.
 *     lt_data_mem - (IN) LT Data Memory.
 *     data        - (IN/OUT) Reference to LT Data to be configured.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_data_value_set(int unit, _field_stage_t *stage_fc,
                                      _field_group_t *fg, int index,
                                      soc_mem_t lt_data_mem, uint32 *data)
{
    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == data)) {
        return (BCM_E_PARAM);
    }

    /* Presel entry in Flowtracker stage is handled differently */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if ((soc_feature(unit, soc_feature_field_flowtracker_support)) &&
        (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER)) {
        return _bcm_field_hx5_ft_lt_entry_data_value_set(unit, stage_fc,
                fg, index, lt_data_mem, data);
    }
#endif

    /* Group must have a valid KeyGen index, else return error. */
    if (_FP_EXT_SELCODE_DONT_CARE == fg->ext_codes[index].keygen_index) {
        return (BCM_E_INTERNAL);
    }

    /* Check and set keygen index value. */
    soc_mem_field32_set(unit, lt_data_mem, data,
        KEY_GEN_PROGRAM_PROFILE_INDEXf, fg->ext_codes[index].keygen_index);

    /* Check and set IP Normalize control. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].normalize_l3_l4_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            NORMALIZE_L3_L4f, fg->ext_codes[index].normalize_l3_l4_sel);
    }

    /* Check and set MAC Normalize control. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].normalize_mac_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            NORMALIZE_L2f, fg->ext_codes[index].normalize_mac_sel);
    }

    /* Check and set AUX TAG A selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_a_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_A_SELf, fg->ext_codes[index].aux_tag_a_sel);
    }

    /* Check and set AUX TAG B selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_b_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_B_SELf, fg->ext_codes[index].aux_tag_b_sel);
    }

    /* Check and set AUX TAG C selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_c_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_C_SELf, fg->ext_codes[index].aux_tag_c_sel);
    }

    /* Check and set AUX TAG D selector value. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].aux_tag_d_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            AUX_TAG_D_SELf, fg->ext_codes[index].aux_tag_d_sel);
    }

    /* Check and set TCP function selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].tcp_fn_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            TCP_FN_SELf, fg->ext_codes[index].tcp_fn_sel);
    }

    /* Check and set TOS function selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].tos_fn_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            TOS_FN_SELf, fg->ext_codes[index].tos_fn_sel);
    }

    /* Check and set TTL function selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].ttl_fn_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            TTL_FN_SELf, fg->ext_codes[index].ttl_fn_sel);
    }

    /* Check and set Class ID container A selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_a_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_A_SELf,
            fg->ext_codes[index].class_id_cont_a_sel);
    }

    /* Check and set Class ID container B selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_b_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_B_SELf,
            fg->ext_codes[index].class_id_cont_b_sel);
    }

    /* Check and set Class ID container C selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_c_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_C_SELf,
            fg->ext_codes[index].class_id_cont_c_sel);
    }

    /* Check and set Class ID container D selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].class_id_cont_d_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            CLASS_ID_CONTAINER_D_SELf,
            fg->ext_codes[index].class_id_cont_d_sel);
    }

    /* Check and set Source Container A selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_cont_a_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_CONTAINER_A_SELf, fg->ext_codes[index].src_cont_a_sel);
    }

    /* Check and set Source Container B selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_cont_b_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_CONTAINER_B_SELf, fg->ext_codes[index].src_cont_b_sel);
    }

    /* Check and set Src Dest Container 0 selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_dest_cont_0_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_DST_CONTAINER_0_SELf, fg->ext_codes[index].src_dest_cont_0_sel);
    }

    /* Check and set Src Dest Container 1 selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].src_dest_cont_1_sel) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            SRC_DST_CONTAINER_1_SELf, fg->ext_codes[index].src_dest_cont_1_sel);
    }

    /* Check and set IPBM selector. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].ipbm_present) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            IPBM_PRESENTf, fg->ext_codes[index].ipbm_present);
    }

    /* Check and set IPBM Source. */
    if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].ipbm_source) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            IPBM_SOURCEf, fg->ext_codes[index].ipbm_source);
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
            if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].udf_sel) {
            soc_mem_field32_set(unit, lt_data_mem, data,
                UDF_SELf, fg->ext_codes[index].udf_sel);
        }
        /* Check and set Destination Container A selector. */
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].dst_cont_a_sel) {
            soc_mem_field32_set(unit, lt_data_mem, data,
                DST_CONTAINER_0_SELf, fg->ext_codes[index].dst_cont_a_sel);
        }

        /* Check and set Destination Container B selector. */
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].dst_cont_b_sel) {
            soc_mem_field32_set(unit, lt_data_mem, data,
                DST_CONTAINER_1_SELf, fg->ext_codes[index].dst_cont_b_sel);
        }

        /* Check and set ALT TTL function selector. */
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[index].alt_ttl_fn_sel) {
            soc_mem_field32_set(unit, lt_data_mem, data,
                ALT_TTL_FN_SELf, fg->ext_codes[index].alt_ttl_fn_sel);
        }
    }
#endif

    /* Set Logical Table ID value. */
    if (-1 != fg->lt_id) {
        soc_mem_field32_set(unit, lt_data_mem, data,
            LOGICAL_TABLE_IDf, (fg->lt_id));
    }

    /* Enable lookup for this logical table in the slice. */
    soc_mem_field32_set(unit, lt_data_mem, data, ENABLEf,
        (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 1 : 0);

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        /* Switch on entry part. */
        switch (index) {
            case 0:
                if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE)
                        || (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE)) {
                    /* Multiwide mode first slice. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 1);
                } else {
                    /* Independent slice. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 0);
                }
                break;
            case 1:
                if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
                    /* Multiwide mode second slice of THREE. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 3);
                } else {
                    /* Multiwide mode second slice of TWO. */
                    soc_mem_field32_set(unit, lt_data_mem, data,
                            MULTIWIDE_MODEf, 2);
                }
                break;
            case 2:
                /* Multiwide mode third slice. */
                soc_mem_field32_set(unit, lt_data_mem, data,
                        MULTIWIDE_MODEf, 4);
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        soc_mem_field32_set(unit, lt_data_mem, data,
                                         KEY_TYPEf, fg->em_mode);
    } else {
        /* Invalid Stage Id */
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_default_data_value_set
 * Purpose:
 *     Set LT entry DATA portion of an entry.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     stage_fc - (IN)     Stage field control structure.
 *     fg       - (IN)     Field group structure.
 *     index    - (IN)     Entry part index.
 *     lt_f_ent - (IN/OUT) Logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_default_data_value_set(int unit, _field_stage_t *stage_fc,
                                          _field_group_t *fg, int index,
                                          _field_lt_entry_t *lt_f_ent)
{
    soc_mem_t lt_tcam_mem; /* LT selection TCAM data view memory name. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Group must have a valid KeyGen index, else return error. */
    if (_FP_EXT_SELCODE_DONT_CARE == fg->ext_codes[index].keygen_index) {
        return (BCM_E_INTERNAL);
    }

    /* Get LT select TCAM memory identifier. */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_data_mem_get(unit, stage_fc, fg,
        &lt_tcam_mem));

    /* Allocate entry part key, mask & data buffer. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_entry_get(unit, fg, lt_f_ent));

    /* Configure TCAM data values */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_data_value_set(unit, stage_fc,
                                  fg, index, lt_tcam_mem, lt_f_ent->tcam.data));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_keygen_profiles_mem_get
 * Purpose:
 *     Get LT keygen program profile memory names.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     stage_fc - (IN)     Stage field control structure.
 *     fg       - (IN)     Field group structure.
 *     mem_arr  - (IN/OUT) Pointer to memory name array.
 *     arr_len  - (IN)     Memory name array length.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_keygen_profiles_mem_get(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg, soc_mem_t *mem_arr,
                                  int arr_len)
{
    int idx; /* Array index iterator. */
    static const soc_mem_t ifp_global_mem[2] =
    {
        IFP_KEY_GEN_PROGRAM_PROFILEm,
        IFP_KEY_GEN_PROGRAM_PROFILE2m
    };
    static const soc_mem_t em_global_mem[2] =
    {
        EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm,
        EXACT_MATCH_KEY_GEN_MASKm
    };


    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == mem_arr)) {
        return (BCM_E_PARAM);
    }

    /*
     * Assign keygen memory identifier name based on stage group oper mode.
     */
    switch (stage_fc->oper_mode) {
        case bcmFieldGroupOperModeGlobal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                for (idx = 0; idx < arr_len; idx++) {
                    mem_arr[idx] = ifp_global_mem[idx];
                }
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                for (idx = 0; idx < arr_len; idx++) {
                    mem_arr[idx] = em_global_mem[idx];
                }
            } else {
                return (BCM_E_PARAM);
            }
            break;
        case bcmFieldGroupOperModePipeLocal:
            if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                for (idx = 0; idx < arr_len; idx++) {
                    BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                             ifp_global_mem[idx],
                                             fg->instance,
                                             &mem_arr[idx]));
                }
            } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
                for (idx = 0; idx < arr_len; idx++) {
                    BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                             em_global_mem[idx],
                                             fg->instance,
                                             &mem_arr[idx]));
                }
            } else {
                return (BCM_E_PARAM);
            }
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_keygen_profile2_entry_pack
 * Purpose:
 *     Build a LT keygen program profile2 table entry.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     fg           - (IN)     Field group structure.
 *     part         - (IN)     Entry part number.
 *     mem          - (IN)     Memory name identifier.
 *     prof2_entry  - (IN/OUT) Entry buffer pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_keygen_profile2_entry_pack(int unit, _field_stage_t *stage_fc,
                            _field_group_t *fg, int part, soc_mem_t mem,
                            ifp_key_gen_program_profile2_entry_t *prof2_entry)
{
    int idx; /* Iterator index. */
    static const soc_field_t pmux_sel[] = /* Post mux selectors. */
        {
            POST_EXTRACTOR_MUX_0_SELf,
            POST_EXTRACTOR_MUX_1_SELf,
            POST_EXTRACTOR_MUX_2_SELf,
            POST_EXTRACTOR_MUX_3_SELf,
            POST_EXTRACTOR_MUX_4_SELf,
            POST_EXTRACTOR_MUX_5_SELf,
            POST_EXTRACTOR_MUX_6_SELf,
            POST_EXTRACTOR_MUX_7_SELf,
            POST_EXTRACTOR_MUX_8_SELf,
            POST_EXTRACTOR_MUX_9_SELf,
            POST_EXTRACTOR_MUX_10_SELf,
            POST_EXTRACTOR_MUX_11_SELf,
            POST_EXTRACTOR_MUX_12_SELf,
            POST_EXTRACTOR_MUX_13_SELf,
            POST_EXTRACTOR_MUX_14_SELf
        };

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == prof2_entry)) {
        return (BCM_E_PARAM);
    }

    /* Set Post MUX extractor selcode values. */
    for (idx = 0; idx < 15; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].pmux_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof2_entry, pmux_sel[idx],
                fg->ext_codes[part].pmux_sel[idx]);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_keygen_profile1_entry_pack
 * Purpose:
 *     Build a LT keygen program profile1 table entry.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     fg           - (IN)     Field group structure.
 *     part         - (IN)     Entry part number.
 *     mem          - (IN)     Memory name identifier.
 *     prof1_entry  - (IN/OUT) Entry buffer pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_keygen_profile1_entry_pack(int unit, _field_stage_t *stage_fc,
                               _field_group_t *fg, int part, soc_mem_t mem,
                               ifp_key_gen_program_profile_entry_t *prof1_entry)
{
    int idx; /* Iterator index. */
    static const soc_field_t l1_32_sel[] = /* 32 bit extractors. */
        {
            L1_E32_SEL_0f,
            L1_E32_SEL_1f,
            L1_E32_SEL_2f,
            L1_E32_SEL_3f
        };
    static const soc_field_t l1_16_sel[] = /* 16 bit extractors. */
        {
            L1_E16_SEL_0f,
            L1_E16_SEL_1f,
            L1_E16_SEL_2f,
            L1_E16_SEL_3f,
            L1_E16_SEL_4f,
            L1_E16_SEL_5f,
            L1_E16_SEL_6f
        };
    static const soc_field_t l1_8_sel[] = /* 8 bit extractors. */
        {
            L1_E8_SEL_0f,
            L1_E8_SEL_1f,
            L1_E8_SEL_2f,
            L1_E8_SEL_3f,
            L1_E8_SEL_4f,
            L1_E8_SEL_5f,
            L1_E8_SEL_6f
        };
    static const soc_field_t l1_4_sel[] = /* 4 bit extractors. */
        {
            L1_E4_SEL_0f,
            L1_E4_SEL_1f,
            L1_E4_SEL_2f,
            L1_E4_SEL_3f,
            L1_E4_SEL_4f,
            L1_E4_SEL_5f,
            L1_E4_SEL_6f,
            L1_E4_SEL_7f
        };
    static const soc_field_t l1_2_sel[] = /* 2 bit extractors. */
        {
            L1_E2_SEL_0f,
            L1_E2_SEL_1f,
            L1_E2_SEL_2f,
            L1_E2_SEL_3f,
            L1_E2_SEL_4f,
            L1_E2_SEL_5f,
            L1_E2_SEL_6f,
            L1_E2_SEL_7f
        };
    static const soc_field_t l2_16_sel[] = /* 16 bit extractors. */
        {
            L2_E16_SEL_0f,
            L2_E16_SEL_1f,
            L2_E16_SEL_2f,
            L2_E16_SEL_3f,
            L2_E16_SEL_4f,
            L2_E16_SEL_5f,
            L2_E16_SEL_6f,
            L2_E16_SEL_7f,
            L2_E16_SEL_8f,
            L2_E16_SEL_9f
        };
    static const soc_field_t l3_4_sel[] = /* 4 bit extractors. */
        {
            L3_E4_SEL_0f,
            L3_E4_SEL_1f,
            L3_E4_SEL_2f,
            L3_E4_SEL_3f,
            L3_E4_SEL_4f,
            L3_E4_SEL_5f,
            L3_E4_SEL_6f,
            L3_E4_SEL_7f,
            L3_E4_SEL_8f,
            L3_E4_SEL_9f,
            L3_E4_SEL_10f,
            L3_E4_SEL_11f,
            L3_E4_SEL_12f,
            L3_E4_SEL_13f,
            L3_E4_SEL_14f,
            L3_E4_SEL_15f,
            L3_E4_SEL_16f,
            L3_E4_SEL_17f,
            L3_E4_SEL_18f,
            L3_E4_SEL_19f,
            L3_E4_SEL_20f
        };
    static const soc_field_t l3_2_sel[] = /* 2 bit extractors. */
        {
            L3_E2_SEL_0f,
            L3_E2_SEL_1f,
            L3_E2_SEL_2f,
            L3_E2_SEL_3f,
            L3_E2_SEL_4f
        };
    static const soc_field_t l3_1_sel[] = /* 1 bit extractors. */
        {
            L3_E1_SEL_0f,
            L3_E1_SEL_1f
        };

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == prof1_entry)) {
        return (BCM_E_PARAM);
    }

    /* Set 32bit extractor selcode values. */
    for (idx = 0; idx < 4; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e32_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_32_sel[idx],
                fg->ext_codes[part].l1_e32_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_16_sel[idx],
                fg->ext_codes[part].l1_e16_sel[idx]);
        }
    }

    /* Set 8bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e8_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_8_sel[idx],
                fg->ext_codes[part].l1_e8_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_4_sel[idx],
                fg->ext_codes[part].l1_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 8; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l1_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l1_2_sel[idx],
                fg->ext_codes[part].l1_e2_sel[idx]);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l2_e16_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l2_16_sel[idx],
                fg->ext_codes[part].l2_e16_sel[idx]);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 21; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e4_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l3_4_sel[idx],
                fg->ext_codes[part].l3_e4_sel[idx]);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e2_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l3_2_sel[idx],
                fg->ext_codes[part].l3_e2_sel[idx]);
        }
    }

    /* Set 1bit extractor selcode values. */
    for (idx = 0; idx < 2; idx++) {
        if (_FP_EXT_SELCODE_DONT_CARE != fg->ext_codes[part].l3_e1_sel[idx]) {
            soc_mem_field32_set(unit, mem, prof1_entry, l3_1_sel[idx],
                fg->ext_codes[part].l3_e1_sel[idx]);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_group_keygen_profiles_index_alloc
 * Purpose:
 *     Install LT keygen program profile table entries in hardware.
 * Parameters:
 *     unit         - (IN)     BCM device number.
 *     stage_fc     - (IN)     Stage field control structure.
 *     fg           - (IN)     Field group structure.
 *     part_index   - (IN)     Entry part number.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_keygen_profiles_index_alloc(int unit,
                                                _field_stage_t *stage_fc,
                                                _field_group_t *fg,
                                                int part_index)
{
    ifp_key_gen_program_profile_entry_t ifp_prof1_entry;
                                     /* IFP Keygen profile1. */
    ifp_key_gen_program_profile2_entry_t ifp_prof2_entry;
                                     /* IFP Keygen profile2. */
    exact_match_key_gen_program_profile_entry_t em_prof_entry;
                                     /* Exact Match Profile. */
    exact_match_key_gen_mask_entry_t em_prof_mask_entry;
                                     /* Exact Match Mask Profile. */
    void *entries[2];                /* Entries array.   */
    soc_mem_t mem[2];                /* Keygen profile memory identifier. */
    int rv;                          /* Operation return status.          */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        /* Get keygen program profiles memory name identifer. */
        BCM_IF_ERROR_RETURN(_field_th_keygen_profiles_mem_get(unit, stage_fc, fg,
                    mem, COUNTOF(mem)));

        /* Clear keygen profile entry. */
        sal_memcpy(&ifp_prof1_entry, soc_mem_entry_null(unit, mem[0]),
                soc_mem_entry_words(unit, mem[0]) * sizeof(uint32));

        /* Clear keygen profile2 entry. */
        sal_memcpy(&ifp_prof2_entry, soc_mem_entry_null(unit, mem[1]),
                soc_mem_entry_words(unit, mem[1]) * sizeof(uint32));

#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* Construct keygen profile1 table entry. */
            BCM_IF_ERROR_RETURN(
                _bcm_field_th3_keygen_profile1_entry_pack(unit, stage_fc, fg,
                                       part_index, mem[0], &ifp_prof1_entry));
        } else
#endif
        {
            /* Construct keygen profile1 table entry. */
            BCM_IF_ERROR_RETURN(
                _field_th_keygen_profile1_entry_pack(unit, stage_fc, fg,
                                       part_index, mem[0], &ifp_prof1_entry));
        }

        /* Construct keygen profile2 table entry. */
        BCM_IF_ERROR_RETURN(_field_th_keygen_profile2_entry_pack(unit, stage_fc, fg,
                    part_index, mem[1], &ifp_prof2_entry));

        /* Initialize entries array pointers. */
        entries[0] = &ifp_prof1_entry;
        entries[1] = &ifp_prof2_entry;

        /* Add entries to profile tables in hardware. */
        rv = soc_profile_mem_add(unit,
                            &stage_fc->keygen_profile[fg->instance].profile,
                            entries, 1, &fg->ext_codes[part_index].keygen_index);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {

        BCM_IF_ERROR_RETURN(_field_th_keygen_profiles_mem_get(unit, stage_fc, fg,
                    mem, COUNTOF(mem)));

        /* Clear keygen profile entry. */
        sal_memcpy(&em_prof_entry, soc_mem_entry_null(unit, mem[0]),
                soc_mem_entry_words(unit, mem[0]) * sizeof(uint32));

        sal_memcpy(&em_prof_mask_entry, soc_mem_entry_null(unit, mem[1]),
                soc_mem_entry_words(unit, mem[1]) * sizeof(uint32));

        /* Construct keygen profile table entry. */
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (soc_feature(unit, soc_feature_th3_style_fp)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_field_th3_keygen_em_profile_entry_pack(unit, stage_fc, fg,
                                                                part_index, mem[0], 
                                                                &em_prof_entry));
        } else
#endif
        {
            BCM_IF_ERROR_RETURN(_field_th_keygen_em_profile_entry_pack(unit, stage_fc, fg,
                        part_index, mem[0], &em_prof_entry));
        }

        /* Construct keygen mask entry */
        BCM_IF_ERROR_RETURN(_field_th_keygen_em_profile_mask_entry_pack(unit, stage_fc, fg,
                    part_index, mem[1], &em_prof_mask_entry));

        /* Initialize entries array pointers. */
        entries[0] = &em_prof_entry;
        entries[1] = &em_prof_mask_entry;

        /* Add entries to profile tables in hardware. */
        rv = soc_profile_mem_add(unit,
                         &stage_fc->keygen_profile[fg->instance].profile,
                         entries, 1, &fg->ext_codes[part_index].keygen_index);
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        /* KeyGen is set along with lts install */
        rv = BCM_E_NONE;
#endif
    } else {
        /* Invalid Stage */
        rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *    _field_th_lt_entry_default_rule_init
 * Purpose:
 *    Create a default rule for the group in logical table selection TCAM with
 *    appropriate key, mask and data values.
 * Parameters:
 *     unit     - (IN)      BCM device number.
 *     stage_fc - (IN)      Stage field control structure.
 *     lt_f_ent - (IN/OUT)  Logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_default_rule_init(int unit, _field_stage_t *stage_fc,
                                     _field_lt_entry_t *lt_f_ent)
{
    _field_group_t *fg;   /* Field group structure.       */
    int parts_count = -1; /* Number of field entry parts. */
    int idx;              /* Entry parts iterator.        */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Get entry's group structure. */
    fg = lt_f_ent->group;

    /* Get number of entry parts using group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /* Install entry parts in hardware. */
    for (idx = 0; idx < parts_count; idx++) {
        /*
         * Install keygen program profile table entries for the group in
         * hardware.
         */
        BCM_IF_ERROR_RETURN(_bcm_field_th_group_keygen_profiles_index_alloc(
                                                   unit, stage_fc, fg, idx));
        /*
         * Build LT selection TCAM default entry.
         */
        BCM_IF_ERROR_RETURN(_field_th_lt_entry_default_data_value_set(unit,
                            stage_fc, fg, idx, lt_f_ent + idx));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_default_tcam_entry_install
 * Purpose:
 *     Install a logical table entry in hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_entry - (IN) Field logical table entry.
 *     group_expand - (IN) Indicates whether the LT tcam install
 *                         is for auto-expanded slices.
 *     part_idx - (IN) Slice part, valid only if group_expand is 1.
 *     lt_fs    - (IN) Reference to LT slice, valid only if group_expand is 1.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_tcam_entry_install(int unit, bcm_field_entry_t lt_entry,
                                        int group_expand, int part_idx,
                                        _field_lt_slice_t *lt_fs)
{
    _field_stage_t *stage_fc;           /* Stage Field Control structure.  */
    int tcam_idx[_FP_MAX_ENTRY_WIDTH];  /* Entry TCAM index.               */
    int parts_count = 0;                /* Field entry parts count.        */
    _field_lt_entry_t *lt_f_ent;        /* Field LT entry pointer.         */
    _field_lt_slice_t *lt_part_fs = NULL; /* Field LT entry pointer.       */
    _field_control_t *fc;               /* Field control pointer.          */
    int idx;                            /* Index iterator.                 */
    int rv = BCM_E_NONE;                /* Operation return status.        */
    soc_mem_t lt_tcam_mem;              /* TCAM memory ID.                 */
    uint8          valid;               /* Entry Valid bit.                */

    /* Get device field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit , &fc));

    /* Get all part of the entry. */
    BCM_IF_ERROR_RETURN
         (_field_th_lt_entry_get_by_id(unit, lt_entry, &lt_f_ent));

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
        lt_f_ent->group->stage_id, &stage_fc));

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        lt_f_ent->group->flags, &parts_count));

    for (idx = 0; idx < parts_count; idx++) {
        if (group_expand == 1) {
            if (part_idx != idx) {
                continue;
            }
            lt_part_fs = lt_fs;
        } else {
            lt_part_fs = (lt_f_ent + idx)->lt_fs;
        }

        BCM_IF_ERROR_RETURN(_field_th_lt_entry_tcam_idx_get(unit,
                        lt_f_ent + idx, lt_part_fs, tcam_idx + idx));
    }

    if (lt_part_fs == NULL) {
        return BCM_E_INTERNAL;
    }

    /*
     * Get TCAM view to be used for programming the hardware based on the group
     * operational mode.
     */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent,
        &lt_tcam_mem));

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_field_multi_pipe_enhanced) &&
        _BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        valid = (lt_f_ent->group->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 0;
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
    {
        valid = (lt_f_ent->group->flags & _FP_GROUP_LOOKUP_ENABLED) ?
                (soc_feature(unit, soc_feature_td3_style_fp) ? 3 : 1) : 0;
    }

    /* Install all parts of the entry in hardware. */
    for (idx = 0; idx < parts_count; idx++) {
        /* In case of group expand, iterate for the given part. */
        if ((group_expand == 1) && (part_idx != idx)) {
            continue;
        }
       /*
        * Maximum index value for entry in this TCAM is limited to
        * No. of slices * No. of LT_IDs/slice (12 * 32 == 384).
        */
        if ((tcam_idx[idx] < soc_mem_index_min(unit, lt_tcam_mem)) ||
            (tcam_idx[idx] > (stage_fc->lt_tcam_sz - 1) )) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "Error: tcam_idx[%d]:%d is out of range.\n\r"), idx, tcam_idx[idx]));
           return (BCM_E_INTERNAL);
        }


        rv = _bcm_field_th_lt_entry_hw_install(unit, lt_tcam_mem, tcam_idx[idx],
                                               (lt_f_ent + idx)->tcam.key,
                                               (lt_f_ent + idx)->tcam.mask,
                                               (lt_f_ent + idx)->tcam.data,
                                               valid);
        if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "Error: LT entry HW Install failed[%d] tcam_idx[%d]:%d.\n\r"),
               rv, idx, tcam_idx[idx]));
           return (BCM_E_INTERNAL);
        }
        lt_f_ent[idx].flags &= ~_FP_ENTRY_DIRTY;
        lt_f_ent[idx].flags |= _FP_ENTRY_INSTALLED;
        lt_f_ent[idx].flags |= _FP_ENTRY_ENABLED;
    }

    return (rv);
}

/*
 * Function:
 *    _field_th_lt_default_entry_create
 * Purpose:
 *    Create an entry in the slice/s reserved in logical table selection TCAM
 *    for the field group.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     stage_fc     - (IN)  Stage field control structure.
 *     fg           - (IN)  Field group structure.
 *     lt_fs        - (IN)  Field logical table slice structure.
 *     lt_entry     - (OUT) Logical Table field entry identifier.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_create(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg, _field_lt_slice_t *lt_fs,
                                  bcm_field_entry_t *lt_entry)
{
    _field_control_t *fc;        /* Field control structure.    */
    _field_lt_entry_t *lt_f_ent; /* LT field entry pointer.     */
    int rv;                      /* Operation return status.    */

    /* Input parameter check. */
    if ((NULL == lt_entry) || (NULL == stage_fc) || (NULL == fg)
         || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Get device field control handle. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Generate an LT entry ID. */
    fc->last_allocated_lt_eid++;
    while (BCM_SUCCESS(_field_th_lt_entry_get(unit, fc->last_allocated_lt_eid,
                _FP_ENTRY_PRIMARY, &lt_f_ent))) {
        fc->last_allocated_lt_eid++;
        if (_FP_ID_MAX == fc->last_allocated_lt_eid) {
            /* Initialize base LT Entry ID*/
            fc->last_allocated_lt_eid = _FP_ID_BASE;
        }
    }

    /* Update allocated entry ID to the caller. */
    *lt_entry = fc->last_allocated_lt_eid;

    /* Create entry in primary slice. */
    rv = _field_th_lt_default_entry_phys_create(unit, stage_fc, *lt_entry,
            BCM_FIELD_ENTRY_PRIO_LOWEST, lt_fs, fg, &lt_f_ent);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Set up default LT selection rule based on group's keygen codes. */
    rv = _field_th_lt_entry_default_rule_init(unit, stage_fc, lt_f_ent);
    if (BCM_FAILURE(rv)) {
        /* Destroy resources allocated for the entry. */
        _field_th_lt_default_entry_phys_destroy(unit, lt_f_ent);
        return (rv);
    }
    return (rv);
}

/*
 * Function:
 *    _field_th_ingress_selcodes_install
 * Purpose:
 *    Install Logical table selection rule and TCAM key generation rules in
 *    hardware.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     stage_fc     - (IN)  Stage field control structure.
 *     fg           - (IN)  Field group structure.
 *     lt_fs        - (IN)  Field logical table slice structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_selcodes_install(int unit,
                                   _field_stage_t *stage_fc,
                                   _field_group_t *fg,
                                   _field_lt_slice_t *lt_fs,
                                   int lt_pri_part_prio)
{
    int   rv;            /* Operation return status.    */
    int   idx;           /* iteration index.            */
    _field_control_t *fc; /* Field Control Structure.   */
    int   lt_part_prio = 0;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       _field_presel_entry_t  *f_presel;

       /*
        * Create Presel Entries in the LT slice associated with the group
        */
       for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
           if (fg->presel_ent_arr[idx] != NULL) {
              f_presel = fg->presel_ent_arr[idx];
           } else {
              continue;
           }

           /* Allocate the presel entries in the LT slice for this group. */
           rv = _bcm_field_th_lt_entry_config_set(unit, stage_fc, fg, lt_fs,
                                                  f_presel);
           BCM_IF_ERROR_RETURN(rv);

           /* Install the LT Entry in HW. */
           rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel);
           BCM_IF_ERROR_RETURN(rv);

           /* Check if the group is expanded across slices. */
           if (f_presel->lt_fs->next != NULL) {
              _field_lt_slice_t     *lt_part_fs;
              _field_presel_entry_t *f_presel_p;
              int                   p, parts_count = -1;

              /* Get number of entry parts. */
              rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                                        &parts_count);
              BCM_IF_ERROR_RETURN(rv);

              f_presel_p = f_presel;
              for (p = 0; (p < parts_count && f_presel_p != NULL); p++) {
                  lt_part_fs = f_presel_p->lt_fs->next;
                  lt_part_prio = lt_pri_part_prio;
                  for (;lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
                     /* LT part prio update */
                     lt_part_prio -= 1; 
                     rv = _field_th_ingress_group_expand_slice_install(unit, stage_fc,
                                           fg, lt_part_fs->slice_number, p, lt_part_prio);
                     BCM_IF_ERROR_RETURN(rv);
                  }
              }
           }
       }
    } else {
       bcm_field_entry_t lt_entry; /* LT entry ID. */

       /* Create a default LT entry for this group. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_create(unit, stage_fc, fg,
                                                             lt_fs, &lt_entry));
       /* Store LT Entry ID in group LT information structure. */
       fc->lt_info[fg->instance][fg->lt_id]->lt_entry = lt_entry;

       /* Install the LT entry in hardware. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_install(unit, lt_entry));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_ltmap_unused_resource_get
 * Purpose:
 *     Get unused LT IDs and LT action priorities information.
 * Parameters:
 *     unit                 - (IN) BCM device number.
 *     stage_fc             - (IN) Field stage control structure.
 *     ltmap                - (IN) Logical table configuration structure.
 *     unused_ltids         - (OUT) Unused Logical Table IDs.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_ltmap_unused_resource_get(int unit,
                                 _field_stage_t *stage_fc,
                                 _field_group_t *fg,
                                 uint32 *unused_ltids)
{
    int lt_idx;                 /* Logical table iterator. */
    uint32 used_ltids = 0;      /* Used Logical Table IDs. */
    _field_control_t   *fc;     /* Field Control Strucutre.*/
    uint32 supported_ltids = 0; /* Supported Logical Table IDs. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == unused_ltids)) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (_BCM_FIELD_STAGE_FLOWTRACKER == stage_fc->stage_id) {
        return BCM_E_NONE;
    }
#endif

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Based on stage oper mode, derive used ltid */

     /* Iterate over all supported LT IDs. */
    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {
        /* Skip unused LT IDs. */
        if (FALSE == fc->lt_info[fg->instance][lt_idx]->valid) {
            continue;
        }
        /* Update bit value in the used bitmaps. */
        used_ltids |= (1 << lt_idx);
    }

    /* Unused caller requested information. */
    *unused_ltids = ~(used_ltids);

    /* Validate with supported LTIDs */
    SHR_BITSET_RANGE(&supported_ltids, 0, stage_fc->num_logical_tables);
    *unused_ltids = ((*unused_ltids) & (supported_ltids));


    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ltid_alloc
 * Purpose:
 *    Allocate a logical table identifier from unused logical table ID pool.
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    max_ltid          - (IN) Logical Table ID maximum value possible.
 *    lt_unused_bmap    - (IN) Unused Logical Table IDs.
 *    lt_id             - (OUT) Logical Table Identifier.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_ltid_alloc(int unit, int max_ltid, uint32 *lt_unused_bmap, int *lt_id)
{
    int lt_idx; /* Logical table iterator. */

    /* Input parameters check. */
    if (NULL == lt_unused_bmap || NULL == lt_id) {
        return (BCM_E_PARAM);
    }

    /* If all LT_IDs have been assigned, return resource error. */
    if (0 == *lt_unused_bmap) {
        return (BCM_E_RESOURCE);
    }

    /* Iterate over unused LT_IDs bit array. */
    for (lt_idx = 0; lt_idx < max_ltid; lt_idx++) {
        if (*lt_unused_bmap & (1 << lt_idx)) {
            break;
        }
    }

    /* Mark selected LT_ID as used. */
    *lt_unused_bmap &= ~(1 << lt_idx);

    /* Update LT_ID value in caller argument. */
    *lt_id = lt_idx;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_prio_cmp
 * Purpose:
 *     Compare two group priorities linked to logical tables.
 * Parameters:
 *     a - (IN) First logical table group priority.
 *     b - (IN) Second logical table group priority.
 * Returns:
 *     -1 if prio_first <  prio_second
 *     0 if prio_first == prio_second
 *     1 if prio_first >  prio_second
 */
STATIC int
_field_th_lt_prio_cmp(void *a, void *b)
{
    _field_lt_config_t *first;  /* First Logical table configuration.   */
    _field_lt_config_t *second; /* Second Logical table configuration.  */

    first = (_field_lt_config_t *)a;
    second = (_field_lt_config_t *)b;

    if (first->priority < second->priority) {
        return (-1);
    } else if (first->priority > second->priority) {
        return (1);
    }

    return (0);
}

/*
 * Function:
 *    _field_th_lt_priority_alloc
 * Purpose:
 *    Allocate logical table action priority for the group.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN/OUT) Stage field control structure.
 *    fg        - (IN) State machine tracking structure.
 *    lt_id     - (IN) Logical Table Identifier.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_lt_priority_alloc(int unit, _field_stage_t *stage_fc,
                            _field_group_t *fg, int lt_id)
{
    int lt_idx;                                 /* Logical table iterator.   */
    _field_lt_config_t *lt_info = NULL; /* LT information.           */
    int action_prio;                    /* LT action priority value. */
    int idx;                                    /* Iterator.                 */
    _field_control_t *fc;                       /* Field Control Strucutre.  */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

    action_prio = stage_fc->num_logical_tables - 1;
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    _FP_XGS3_ALLOC(lt_info, 
        (stage_fc->num_logical_tables * sizeof(_field_lt_config_t)),
        "lt info for all lt ids");
    if (lt_info == NULL) {
       return (BCM_E_MEMORY);
    }
    /* Get LT information for the group. */
    for (idx = 0; idx < stage_fc->num_logical_tables; idx++) {
        sal_memcpy(&lt_info[idx], fc->lt_info[fg->instance][idx],
                sizeof(_field_lt_config_t));
    }

    /*
     * Initialize LT information for the new logical table ID, for it to be
     * in the correct location after the sort.
     */
    lt_info[lt_id].priority = fg->priority;
    lt_info[lt_id].lt_action_pri = 0;
    lt_info[lt_id].valid = TRUE;

    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d): Verb-B4-Sort: lt_id(%d): lt_grp_prio(%d): "
           "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
            unit, lt_info[lt_idx].lt_id,
            lt_info[lt_idx].priority, lt_info[lt_idx].lt_action_pri,
            lt_info[lt_idx].valid, lt_info[lt_idx].flags));
    }

    /* Sort the logical tables array based on group priority value. */
    _shr_sort(lt_info, stage_fc->num_logical_tables,
              sizeof(_field_lt_config_t),
              _field_th_lt_prio_cmp);

    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d): Verb-Aft-Sort: lt_id(%d): lt_grp_prio(%d): "
            "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
            unit, lt_info[lt_idx].lt_id, lt_info[lt_idx].priority,
            lt_info[lt_idx].lt_action_pri, lt_info[lt_idx].valid,
            lt_info[lt_idx].flags));
    }

    /*
     * Assign LT action priority based on the position of LT ID in the sorted
     * array.
     */
    for (idx = (stage_fc->num_logical_tables - 1); idx >= 0; idx--) {
        if (FALSE == lt_info[idx].valid) {
            continue;
        }
        lt_info[idx].lt_action_pri = action_prio--;
    }

    for (lt_idx = 0; lt_idx < stage_fc->num_logical_tables; lt_idx++) {
       LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "FP(unit %d): Verb-Aft-PrioAssign: lt_id(%d): lt_grp_prio(%d): "
           "lt_actn_prio(%d): valid:(%d) flags(0x%x)\n"),
            unit, lt_info[lt_idx].lt_id, lt_info[lt_idx].priority,
            lt_info[lt_idx].lt_action_pri, lt_info[lt_idx].valid,
           lt_info[lt_idx].flags));
    }

    for (idx = 0; idx < stage_fc->num_logical_tables; idx++) {
        if (FALSE == lt_info[idx].valid) {
            continue;
        }
        lt_idx = lt_info[idx].lt_id;
        fc->lt_info[fg->instance][lt_idx]->lt_action_pri =
            lt_info[idx].lt_action_pri;
    }

    sal_free(lt_info);
    return (BCM_E_NONE);
}
STATIC int
_field_th_identical_lt_id_alloc(int unit, _field_stage_t *stage_fc,
                                _field_group_t *fg)
{
    _field_control_t *fc = NULL;
    _field_group_t   *fg_orig = NULL;
    bcm_error_t      rv = BCM_E_NONE;
    uint8            hint_found = 0;
    _field_hints_t   *f_ht = NULL; /* Field hints Structure. */
    _field_hint_t    *hint_list = NULL; /* Field Hints Structure. */
    bcm_field_hint_t *hint_entry; /* Hint Entry. */


    if ((NULL == fg) || (NULL == stage_fc)) {
        return BCM_E_PARAM;
    }

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->hintid) {
        rv = _field_hints_control_get (unit, fg->hintid, &f_ht);
        if ((NULL == f_ht) || BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Error: Hints error.\n\r")));
            return BCM_E_INTERNAL;
        }
        hint_list = f_ht->hints;
        while (hint_list) {
            hint_entry = hint_list->hint;
            if (hint_entry) {
                if (bcmFieldHintTypeGroupIdenticalLookupId ==
                                               hint_entry->hint_type) {
                    BCM_IF_ERROR_RETURN(_field_group_get(unit,
                                                    hint_entry->group_id,
                                                    &fg_orig));
                    fg->lt_id = fg_orig->lt_id;
                    hint_found = 1;
                    break;
                }
            }
            hint_list = hint_list->next;
        }
    }

    if (0 == hint_found) {
        return BCM_E_INTERNAL;
    }

    fc->lt_info[fg->instance][fg->lt_id]->lt_ref_count++;

    return rv;
}

/*
 * Function:
 *    _field_th_group_ltmap_alloc
 * Purpose:
 *    Allocate a Logitcal Table ID for the field group.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    stage_fc  - (IN/OUT) Stage field control structure.
 *    fg        - (IN/OUT) Field group structure pointer.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_ltmap_alloc(int unit, _field_stage_t *stage_fc,
                            _field_group_t *fg)
{
    uint32 unused_ltids = 0;                    /* Unused LT IDs.             */
    int lt_id;                                  /* Logical Table Identifier.  */
    _field_control_t *fc;                       /* Field Control Strucutre.   */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fg) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (_BCM_FIELD_STAGE_FLOWTRACKER == stage_fc->stage_id) {
        return BCM_E_NONE;
    }
#endif

    if (fg->flags & _FP_GROUP_IDENTICAL_LT_ID) {
        return _field_th_identical_lt_id_alloc(unit, stage_fc, fg);
    }

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get unused LT IDs and LT action priorites. */
    BCM_IF_ERROR_RETURN(_field_ltmap_unused_resource_get(unit, stage_fc,
                                                         fg, &unused_ltids));
    /* Allocate a LT ID from unused LT ID pool. */
    BCM_IF_ERROR_RETURN(_field_th_ltid_alloc(unit, stage_fc->num_logical_tables,
                                             &unused_ltids, &lt_id));

    /* Allocate LT action priority. */
    BCM_IF_ERROR_RETURN(_field_th_lt_priority_alloc(unit, stage_fc, fg, lt_id));

    /* Set LT ID info in database. */
    fc->lt_info[fg->instance][lt_id]->valid = TRUE;
    fc->lt_info[fg->instance][lt_id]->priority = fg->priority;
    fc->lt_info[fg->instance][lt_id]->flags = fg->flags;
    fc->lt_info[fg->instance][lt_id]->lt_ref_count++;

    /* Get group's LT info structure pointer. */
    fg->lt_id = lt_id;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_group_install
 * Purpose:
 *    Auxiliary routine used to install field group in hardware.
 * Parameters:
 *     unit         - (IN)      BCM device number.
 *     stage_fc     - (IN)      Installed group structure.
 *     fg           - (IN/OUT)  Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_group_install(int unit, _field_stage_t *stage_fc,
                                _field_group_t *fg)
{
    _field_lt_slice_t *lt_fs;   /* LT Slice pointer.                */
    _field_slice_t *fs;         /* Field slice structure pointer.   */
    int parts_count = -1;       /* Number of entry parts.           */
    int part;                   /* Entry parts iterator.            */
    uint8 slice_number;         /* LT Slice number.                 */
    _field_control_t *fc;       /* Field control structure.         */
    int lt_part_prio = _FP_MAX_LT_PART_PRIO(unit);
                                /* LT part priority value.          */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == fg->lt_slices)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Allocate a Logical Table ID for the group. */
    BCM_IF_ERROR_RETURN(_field_th_group_ltmap_alloc(unit, stage_fc, fg));

#if defined(BCM_FLOWTRACKER_SUPPORT)
    /* Assign Group Key Type for the group */
    BCM_IF_ERROR_RETURN(_field_hx5_ft_group_keytype_assign(unit, stage_fc, fg));
#endif

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
        stage_fc->stage_id, fg->flags, &parts_count));

    /* Iterate over entry parts and program hardware for each slice. */
    for (part = 0; part < parts_count; part++) {
        /* Get slice number to which the entry part belongs to. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(part,
            fg->flags, &slice_number));

        /* Get LT entry part slice control structure. */
        lt_fs = stage_fc->lt_slices[fg->instance] +
                fg->slices->slice_number + slice_number;

        if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
            break;
        }

        /* Set slice mode in hardware based on Group mode/flags. */
        BCM_IF_ERROR_RETURN(_field_th_ingress_slice_mode_set(unit, stage_fc,
            lt_fs->slice_number, fg, 0));
        /*
         * Initialize slice free entries count value to number of
         * entries in the slice.
         */
        if (0 == fg->slices[0].lt_map) {
            lt_fs->free_count = lt_fs->entry_count;
        }

        /* Get IFP TCAM slice information. */
        fs = fg->slices + part;

        /* Update slice LT_ID mapping in group's slice. */
        fs->lt_map |= (1 << fg->lt_id);

        fc->lt_info[fg->instance][fg->lt_id]->lt_part_map |= (1 << fs->slice_number);
        fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[fs->slice_number] = lt_part_prio;
    }

    /* Install TCAM keygen program configuration in hardware for each slice. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_selcodes_install(unit, stage_fc, fg,
                                                           fg->lt_slices, lt_part_prio));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_group_install
 * Purpose:
 *     Install group slice mode in hardware and update slice parameters based on
 *     the group mode.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_group_install(int unit,
                                  _field_group_add_fsm_t *fsm_ptr)
{
    _field_slice_t *fs;     /* Slice pointer.           */
    _field_group_t *fg;     /* Field Group Pointer.     */
    int parts_count = -1;   /* Number of entry parts.   */
    int idx;                /* Slice iterator.          */
    uint32 entry_flags;     /* Entry flags.             */
    uint8 slice_number;     /* Slice number.            */
    int slice_init;         /* Slice SW init needed.    */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Get field group pointer. */
    fg = fsm_ptr->fg;

    /* Check if this is the first group installed in slice. */
    slice_init = (0 == fg->slices[0].lt_map) ? TRUE : FALSE;

    /* Write group parameters to hardware. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_group_install(unit,
                                     fsm_ptr->stage_fc, fg));

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
        fsm_ptr->stage_fc->stage_id, fg->flags, &parts_count));

    for (idx = parts_count - 1; idx >=0; idx--) {
        /* Get entry flags. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_entry_flags(unit, idx,
            fg->flags, &entry_flags));

        /* Get slice number for given entry part. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(idx,
            fg->flags, &slice_number));

        /* Get slice pointer. */
        fs = fg->slices + slice_number;

        if (!(entry_flags & _FP_ENTRY_SECOND_HALF)) {
            if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
                if (slice_init) {
                    /*
                     * Initialize slice free entries count value based on group
                     * flags.
                     */
#if defined(BCM_TOMAHAWK3_SUPPORT)
                    if (SOC_IS_TOMAHAWK3(unit) &&
                        (fg->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
                        fs->free_count = fs->entry_count;
                    } else
#endif
                    {

                        if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                            && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
                            fs->free_count = fs->entry_count;
                        } else {
                            fs->entry_count >>= 1;
                            fs->free_count = fs->entry_count;
                            fs->start_tcam_idx >>= 1;
                        }
                    }
                    /* Initialize slice group flags. */
                    fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;
                }
            } else {
                fs->group_flags |= fg->flags & _FP_GROUP_STATUS_MASK;
            }
        }
    }

    return (BCM_E_NONE);
}

/* Function:
 * _bcm_field_th_group_add_mode_validate
 *
 * Purpose:
 *    validate the group to make sure the group with same priority and mode
 *    doesnot exist in other slices
 *
 * Paramaters:
 * unit             - (IN) BCM device number
 * fg               - (IN) Pointer to device field group structure
 *
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_field_th_group_add_mode_validate(int unit,
                                      _field_group_t *fg,
                                      int *slice_number)
{
    _field_control_t  *fc;
    _field_group_t    *curr_group;

	if (fg == NULL) {
        return BCM_E_PARAM;
    }
     /* Get device Field control structure. */
     BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
     curr_group = fc->groups;

    if (curr_group == NULL) {
        return BCM_E_NONE;
    }

    while (curr_group != NULL) {
        if ((curr_group->stage_id == fg->stage_id) &&
            (curr_group->gid != fg->gid) &&
            (curr_group->stage_id != _BCM_FIELD_STAGE_EXACTMATCH) &&
            (curr_group->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) &&
            (curr_group->instance == fg->instance) &&
            (curr_group->priority == fg->priority)) {
            /*
             * Default group and Presel group can't have same priority.
             */
            if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) !=
                (curr_group->flags & _FP_GROUP_PRESELECTOR_SUPPORT)) {
                 LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "Error: Same priority is not possible between default group and"
                    " presel group.\n\r")));
                 return BCM_E_PARAM;
            }

            if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
                /* Return Error if modes are different */
                if ((fg->flags & _FP_GROUP_SPAN_MASK) !=
                    (curr_group->flags & _FP_GROUP_SPAN_MASK)) {
                    return BCM_E_PARAM;
                }

                /* update slice shared with same priority groups */
                *slice_number = curr_group->slices->slice_number;
                break;
            }
        }
        curr_group = curr_group->next;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *    _field_th_group_add_slice_allocate
 * Purpose:
 *    Allocate slice for a new group.
 * Parameters:
 *    unit     - (IN)     BCM device number.
 *    fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_add_slice_allocate(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    _field_group_t     *fg;           /* Field Group structure pointer. */
    _field_stage_t     *stage_fc;     /* Field Stage control pointer.   */
    int                slice;         /* Slice iterator.                */
    uint8              slice_found = 0; /* Slice found indicator.       */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Initialize field group pointer. */
    fg = fsm_ptr->fg;

    /* Initialize field stage pointer. */
    stage_fc = fsm_ptr->stage_fc;

    /*
     * If group priority is BCM_FIELD_GROUP_PRIO_ANY,
     * generate a new unique priority.
     */
    if ((stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) &&
        (fg->priority == BCM_FIELD_GROUP_PRIO_ANY)) {
        stage_fc->group_running_prio += 1;
        fg->priority = stage_fc->group_running_prio;
    }

    slice = -1;
    fsm_ptr->rv = _bcm_field_th_group_add_mode_validate(unit, fg, &slice);
    if (BCM_FAILURE(fsm_ptr->rv)) {
        fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        return BCM_E_NONE;
    }

    if (slice != -1) {
        slice_found = 1;
    } else {
        slice = 0;
    }

    /* Reserve a slice for this new Field Group. */
    for (; slice < stage_fc->tcam_slices; slice++ ) {
        fsm_ptr->rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                                                             slice);
        if (BCM_SUCCESS(fsm_ptr->rv)) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "==> %s(): SLICE[%d] allocated for group[%d].\n\r"),
            __func__, slice, fg->gid));
            break;
        } else if (slice_found == 1) {
            fsm_ptr->rv = (BCM_E_RESOURCE);
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
            return BCM_E_NONE;
        }

        if ((BCM_E_PARAM == fsm_ptr->rv) || (BCM_E_RESOURCE == fsm_ptr->rv)) {
            break;
        }
    }

    /* Check if allocation was successful. */
    if (slice == stage_fc->tcam_slices) {
        fsm_ptr->rv = (BCM_E_RESOURCE);
    }

    if (BCM_FAILURE(fsm_ptr->rv)) {
        if (fsm_ptr->fsm_state_prev != _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS) {
            fsm_ptr->rv = (BCM_E_NONE);
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS;
        } else {
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        }
    } else {
        /* Update Group slices pointer. */
        fg->slices = stage_fc->slices[fg->instance] + slice;

        /* Update Group LT slices pointer. */
        fg->lt_slices = stage_fc->lt_slices[fg->instance] + slice;

        /*
         * Install key generation extractor codes and LT selection rule for the
         * group in hardware.
         */
        fsm_ptr->rv = _field_th_group_add_group_install(unit, fsm_ptr);
        if (BCM_SUCCESS(fsm_ptr->rv)) {
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP;
        } else {
            fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
        }
    }

    /* Set current state as the previous state. */
    fsm_ptr->fsm_state_prev = _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE;
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_entry_tcam_parts_count
 * Purpose:
 *    Get number of tcam entries needed to accomodate a SW entry.
 * Parameters:
 *    unit        - (IN)  BCM device number.
 *    group_flags - (IN)  Entry group flags.
 *    part_count  - (OUT) Entry parts count.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_entry_tcam_parts_count(int unit, uint32 group_flags,
                                     int *part_count)
{
    /* Input parameters check. */
    if (NULL == part_count) {
        return (BCM_E_PARAM);
    }

    /* Check group flags settings and determine number of TCAM entry parts. */
    if (group_flags & _FP_GROUP_DW_DEPTH_EXPANDED) {
        /* Double-wide in BSK_FTFP_TCAM */
        *part_count = 2;
    } else if (group_flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        /*
         * Part value is same for Narrow and Wide TCAMs
         * (IFP_TCAM and IFP_TCAM_WIDE)
         * (group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)
         */
        *part_count = 1;
    } else if (group_flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        /* IFP_TCAM_WIDE view only. */
        *part_count = 2;
    } else if (group_flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        /* IFP_TCAM_WIDE view only. */
        *part_count = 3;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_lt_entry_tcam_parts_count
 * Purpose:
 *    Get number of lt tcam entries needed to accomodate a SW entry.
 * Parameters:
 *    unit        - (IN)  BCM device number.
 *    stage_id    - (IN)  Stage Identifier.
 *    group_flags - (IN)  Entry group flags.
 *    part_count  - (OUT) Entry parts count.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_lt_entry_tcam_parts_count(int unit,
                                        _field_stage_id_t stage_id,
                                        uint32 group_flags,
                                        int *part_count)
{
    /* Input parameters check. */
    if (NULL == part_count) {
        return (BCM_E_PARAM);
    }

    if (stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        *part_count = 1;
        return BCM_E_NONE;
    }

    return _bcm_field_th_entry_tcam_parts_count(unit, group_flags, part_count);

}

/*
 * Function:
 *     _bcm_field_th_slice_offset_to_tcam_idx
 * Purpose:
 *     Given stage, slice number and entry index in the slice, calculate TCAM
 *     index.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Pointer to field entry structure.
 *     instance - (IN) Field processor pipe instance.
 *     slice    - (IN) Entry slice number.
 *     slice_idx -(IN) Entry offset in the slice.
 *     tcam_idx - (OUT) Entry tcam index.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_slice_offset_to_tcam_idx(int unit, _field_stage_t *stage_fc,
                                       int instance, int slice, int slice_idx,
                                       int *tcam_idx)
{
    _field_slice_t *fs; /* Field slice structure pointer. */

    /* Input parameters check. */
    if (NULL == stage_fc || NULL == tcam_idx) {
        return (BCM_E_PARAM);
    }

    /* Input parameters check. */
    if (instance < 0 || instance >= stage_fc->num_instances) {
        return (BCM_E_PARAM);
    }

    /* Get the slice pointer. */
    fs = stage_fc->slices[instance] + slice;

    /* Given stage slice number and slice index, calcuate TCAM index. */
    *tcam_idx = fs->start_tcam_idx + slice_idx;

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_tcam_idx_to_slice_offset
 *
 * Purpose:
 *     Given stage and tcam_index,
 *     calculate slice and entry offset in the
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Pointer to field stage structure.
 *     f_ent    - (IN) Reference to field entry structure.
 *     tcam_idx - (IN) Entry tcam index.
 *     slice    - (OUT) Entry slice number.
 *     slice_idx -(OUT) Entry offset in the slice.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_tcam_idx_to_slice_offset(int unit, _field_stage_t *stage_fc,
                                       _field_entry_t *f_ent, int tcam_idx,
                                       int *slice, int *slice_idx)

{
    _field_slice_t  *fs;                  /* Field Slice number.  */
    int             idx;                  /* Slice iterator.      */

   /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == slice_idx) || (NULL == slice)
        || (NULL == f_ent)) {
        return (BCM_E_PARAM);
    }

    for (idx = 0; idx < stage_fc->tcam_slices; idx++) {
        fs = stage_fc->slices[f_ent->group->instance] + idx;
        /*
         * Single wide groups will be in IFP_TCAM view and depth of slice full.
         * Other wide modes are in IFP_TCAM_WIDE view, so depth of slice is half of IFP_TCAM view.
         * So slice start_tcam_idx will vary for slices which use IFP_TCAM and IFP_TCAM_WIDE.
         * Skip, if the slices which are not same in same mode as f_ent->group.
         * Base key point is to lookup in slices which have same view as f_ent.
         */
        if ((0x0 == fs->group_flags) ||
            (0 == (fs->group_flags &  f_ent->group->flags)) ||
            ((f_ent->group->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) != \
             (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE))) {
            continue;
        }


        if (tcam_idx < (fs->start_tcam_idx + fs->entry_count)) {
            *slice = idx;
            *slice_idx = tcam_idx - fs->start_tcam_idx;
            break;
        }
    }

    /* TCAM index sanity check. */
    if (idx == stage_fc->tcam_slices) {
        return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_tcam_part_to_slice_number
 * Purpose:
 *     Each field entry contains up to 3 TCAM entries. This routine maps tcam
 *     entry (0-2) to a slice number
 * Parameters:
 *     entry_part   - (IN)  Entry Part number.
 *     group_flags  - (IN)  Field Group flags.
 *     slice_number - (OUT) Slice number (0-2)
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_tcam_part_to_slice_number(int entry_part, uint32 group_flags,
                                        uint8 *slice_number)
{
    /* Input parameters check. */
    if (NULL == slice_number) {
        return (BCM_E_PARAM);
    }

    switch (entry_part) {
        case 0:
            *slice_number = 0;
            break;
        case 1:
            *slice_number = 1;
            break;
        case 2:
            *slice_number = 2;
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_entry_part_tcam_idx_get
 *
 * Purpose:
 *     Given primary entry slice/idx calculate secondary/tertiary slice/index.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry pointer.
 *     idx_pri  - (IN) Primary entry tcam index.
 *     ent_part - (IN) Entry part id.
 *     idx_out  - (OUT) Entry part tcam index.
 * Returns
 *     BCM_E_XXX
 */
int
_bcm_field_th_entry_part_tcam_idx_get(int unit, _field_entry_t *f_ent,
                                      uint32 idx_pri, uint8 entry_part,
                                      int *idx_out)
{
    uint8 slice_num = 0;        /* Primary entry slice number.              */
    int pri_slice = 0;          /* Primary slice number.                    */
    int pri_index = 0;          /* Primary entry.                           */
    int part_index;             /* Entry part slice index.                  */
    _field_stage_t *stage_fc;   /* Field stage control structure pointer.   */
    _field_slice_t *fs;         /* Field slice structure pointer.           */
    _field_group_t *fg;         /* Field Group structure.                   */
    int rv;                     /* Operation return status.                 */

    /* Input parameters check. */
    if (NULL == f_ent || NULL == idx_out) {
        return (BCM_E_PARAM);
    }

    /* Primary entry index. */
    if (0 == entry_part) {
        *idx_out = idx_pri;
        return (BCM_E_NONE);
    }

    fg = f_ent->group;

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get primary part slice and entry index. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_idx_to_slice_offset(unit, stage_fc,
        f_ent, idx_pri, &pri_slice, &pri_index));

    /* Get entry part slice number 0, 1, 2. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(entry_part,
        fg->flags, &slice_num));

    fs = stage_fc->slices[fg->instance] + (pri_slice + slice_num);

    part_index = pri_index;

    /*
     * Given slice number & entry's offset index in slice, calculcate TCAM
     * index.
     */
    rv = _bcm_field_th_slice_offset_to_tcam_idx(unit, stage_fc, fg->instance,
            fs->slice_number, part_index, idx_out);
    return (rv);
}

/*
 * Function:
 *     _field_th_entry_slice_idx_change
 * Purpose:
 *     Move the software entry to a new slice index.
 * Parmeters:
 *     unit         - (IN)      BCM device number.
 *     f_ent        - (IN/OUT)  Field entry to be moved.
 *     parts_count  - (IN)      Number of entry parts count.
 *     tcam_idx_new - (IN)      Entry new TCAM index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_entry_slice_idx_change(int unit, _field_entry_t *f_ent,
                                 int parts_count, int *tcam_idx_new)
{
    _field_slice_t *fs;         /* Field slice structure pointer.           */
    _field_group_t *fg;         /* Field Group structure pointer.           */
    _field_stage_t *stage_fc;   /* Stage Field control structure pointer.   */
    int new_slice_num = 0;      /* Entry new slice number.                  */
    int new_slice_idx = 0;      /* Entry's new offset in the slice.         */
    int part;                   /* Entry parts iterator.                    */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == f_ent->group) || (NULL == f_ent->fs)) {
        return (BCM_E_PARAM);
    }

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, f_ent->group->stage_id,
        &stage_fc));

    /* Get entry group information. */
    fg = f_ent->group;
    for (part = 0; part < parts_count; part++) {
        /* Get slice control structure. */
        fs = f_ent[part].fs;

        if (FALSE == th_prio_set_with_no_free_entries) {
            fs->entries[f_ent[part].slice_idx] = NULL;
        }

        /*
         * Calculate entry new slice number and offset in slice from TCAM index
         * value.
         */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_idx_to_slice_offset(unit, stage_fc,
            f_ent, tcam_idx_new[part], &new_slice_num, &new_slice_idx));

        /* If entry moves across the slice, update free counter.*/
        if (new_slice_num != f_ent[part].fs->slice_number
            && (0 == (f_ent[part].flags & _FP_ENTRY_SECOND_HALF))) {
            fs->free_count++;
            stage_fc->slices[fg->instance][new_slice_num].free_count--;
        }

        /* Update entry structure. */
        stage_fc->slices[fg->instance][new_slice_num].entries[new_slice_idx] =
            f_ent + part;
        f_ent[part].fs = &stage_fc->slices[fg->instance][new_slice_num];
        f_ent[part].slice_idx = new_slice_idx;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_entry_move
 * Purpose:
 *     Move an entry within a slice by "amount" indexes
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - entry to be moved
 *     amount   - number of indexes to move + or -
 * Returns:
 *     BCM_E_NONE - Success
 */
STATIC int
_field_th_entry_move(int unit, _field_entry_t *f_ent, int amount)
{
    int tcam_idx_old[_FP_MAX_ENTRY_WIDTH] = {0}; /* Original entry TCAM
                                                    index.          */
    int tcam_idx_new[_FP_MAX_ENTRY_WIDTH] = {0}; /* New destination TCAM
                                                    index.          */
    _field_control_t *fc;       /* Field control structure pointer. */
    _field_stage_t *stage_fc;   /* Field stage structure pointer.   */
    _field_group_t *fg;         /* Field Group structure pointer.   */
    /* _field_slice_t *fs; */   /* Field slice structure pointer.   */
    int parts_count = 0;        /* Number of parts per-entry.       */
    int idx;                    /* Entry parts iterator.            */
    int new_offset;             /* New entry offset in slice.       */
    int new_slice_num = -1;     /* New slice number.                */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == f_ent->fs) || (NULL == f_ent->group)) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "FP(unit %d) vverb: BEGIN _field_th_entry_move(entry=%d, amount=%d)\n"),
         unit, f_ent->eid, amount));

    fg = f_ent->group;

    if (0 == amount) {
        LOG_WARN(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) warn:  moving entry=%d, same slice_idx=%d(%#x)\n"),
             unit, f_ent->eid, f_ent->slice_idx, f_ent->slice_idx));
        return (BCM_E_NONE);
    }

    /* Get Field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get Stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get number of parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    for (idx = 0; idx < parts_count; idx++) {
        /* Calculate entry parts OLD TCAM indices. */
        BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_idx_get(unit, f_ent + idx,
            &tcam_idx_old[idx]));

        /* Calculate entry part tcam offset. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_entry_part_tcam_idx_get(unit, f_ent,
            tcam_idx_old[0] + amount, idx, &tcam_idx_new[idx]));
    }

    BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_idx_to_slice_offset(unit, stage_fc,
        f_ent, tcam_idx_new[0], &new_slice_num, &new_offset));

    if (f_ent->flags & _FP_ENTRY_INSTALLED) {
        /* fs = stage_fc->slices[fg->instance] + new_slice_num; */

        /* Move the entry in hardware if it's already installed in HW. */
        if (f_ent->flags & _FP_ENTRY_INSTALLED) {
            BCM_IF_ERROR_RETURN(fc->functions.fp_entry_move(unit, f_ent,
                parts_count, tcam_idx_old, tcam_idx_new));
        }
    }
    /* Move the SW entry to the new slice index. */
    BCM_IF_ERROR_RETURN(_field_th_entry_slice_idx_change(unit, f_ent,
        parts_count, tcam_idx_new));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_slice_is_empty
 * Purpose:
 *     Report if a slice has any entries.
 * Parameters:
 *     fc       - (IN)  Field control structure.
 *     fs       - (IN)  Slice control structure.
 *     empty    - (OUT) True - slice is empty/False otherwise.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_slice_is_empty(int unit, _field_slice_t *fs, uint8 *empty)
{
    /* Input parameters check. */
    if (NULL == fs || NULL == empty) {
        return (BCM_E_PARAM);
    }

    /* Check if total entries count is equal to the free count. */
    if (fs->entry_count == fs->free_count) {
        /* Slice has not valid group entries. */
        *empty = TRUE;
    } else {
        /* Slice has atleast one valid entry. */
        *empty = FALSE;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_slice_clear
 *
 * Purpose:
 *    Reset the fields in a slice. Note that the entries list must be
 *    empty before calling this. Also, this does NOT deallocate the memory for
 *    the slice itself. Normally, this is used when a group no longer needs
 *    ownership of a slice so the slice gets returned to the pool of available
 *    slices.
 *
 * Paramters:
 *    unit  - (IN)     BCM device number
 *    fg    - (IN)     Field group structure.
 *    fs    - (IN/OUT) Link to physical slice structure to be cleared
 *
 * Returns:
 *    BCM_E_NONE    - Success
 *    BCM_E_BUSY    - Entries still in slice, can't clear slice
 */
STATIC int
_field_th_ingress_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs)
{
    int rv = BCM_E_NONE;        /* return value */
    _field_stage_t *stage_fc;   /* Stage field control structure.   */
    _field_control_t *fc;       /* Field control structure.         */
    uint32 entry_idx;           /* Slice entries iterator.          */
    bcm_port_t port;            /* Port iterator.                   */
    int slice;                  /* Slice iterator. */
    int tcam_idx;               /* Slice first entry tcam index tracker. */
    _field_slice_t *slice_ptr;  /* Slice Pointer for traversal */
#if defined(BCM_UTT_SUPPORT)
    int *slice_depth = NULL;    /* Slice depth array */
#endif

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == fs)) {
        return (BCM_E_PARAM);
    }

    /* Skip for Flowtracker stage */
    if (fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        return (BCM_E_NONE);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fs->stage_id,
        &stage_fc));

    for (entry_idx = 0; entry_idx < fs->entry_count; entry_idx++) {
        if ((NULL != fs->entries[entry_idx])
             && (fs->entries[entry_idx]->group->gid == fg->gid)) {
             LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FP(unit %d) Error: Entries still in slice=%d.\n"),
                 unit, fs->slice_number));
            return (BCM_E_BUSY);
        }
    }

    /* Remove ports from slice's port bitmap. */
    BCM_PBMP_ITER(fg->pbmp, port) {
        BCM_PBMP_PORT_REMOVE(fs->pbmp, port);
    }

    /* Clear Group's LT_ID bit in slice LT map. */
    fs->lt_map &= ~(1 << fg->lt_id);

    /* Set entry count to invalid number */
    fs->entry_count = -1;

#if defined(BCM_UTT_SUPPORT)
    if ((_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id)
        && (soc_feature(unit, soc_feature_utt_ver1_5)
        && soc_feature(unit, soc_feature_utt))) {

        _FP_XGS3_ALLOC(slice_depth, stage_fc->tcam_slices * sizeof(int), "array of slice depth.");
        if (NULL == slice_depth) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META(
               "slice_depth memory resource error\n")));
            return (BCM_E_MEMORY);
        }

        rv = soc_utt_ifp_lookup_depth_get(unit, stage_fc->tcam_slices, slice_depth);
        BCM_IF_ERROR_CLEANUP(rv);

        /*
         * UTT maintains depth of IFP_TCAM_WIDE view but FP internal data structures
         * do maintain IFP_TCAM view. So multiply with 2.
         */
        fs->entry_count = slice_depth[fs->slice_number] * 2;
        for (slice = 0, tcam_idx = 0; slice < fs->slice_number; slice++) {
            slice_ptr = stage_fc->slices[fg->instance] + slice;
            /*
             * For the allocated slices entry_count gets half.
             * So need to use multiples of 2.
             */
            if (slice_ptr->group_flags) {
                tcam_idx += slice_ptr->entry_count * 2;
            } else {
                tcam_idx += slice_ptr->entry_count;
            }
        }
        fs->start_tcam_idx = tcam_idx;
    } else
#endif /* BCM_UTT_SUPPORT */
    /*  Initialize entries per-slice count. */
    if ((soc_feature(unit, soc_feature_th3_style_fp) &&
        (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS))) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (fs->slice_number < _BCM_FIELD_TH3_INGRESS_MAX_SMALL_SLICES) {
            fs->entry_count = _BCM_FIELD_TH3_INGRESS_SMALL_SLICE_SZ;
        } else {
            fs->entry_count = _BCM_FIELD_TH3_INGRESS_LARGE_SLICE_SZ;
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

        for (slice = 0, tcam_idx = 0; slice < fs->slice_number; slice++) {
            slice_ptr = stage_fc->slices[fg->instance] + slice;
            tcam_idx += slice_ptr->entry_count;
        }
        fs->start_tcam_idx = tcam_idx;
    } else {
        fs->entry_count = stage_fc->tcam_sz / stage_fc->tcam_slices;
        fs->start_tcam_idx = fs->slice_number * fs->entry_count;
    }

    /* Initialize free entries count. */
    fs->free_count = fs->entry_count;

    /* Initialize start TCAM index. */
    slice_ptr = NULL;

    /* Clear group flags settings. */
    fs->group_flags = 0;

#if defined(BCM_UTT_SUPPORT)
cleanup:
    if (slice_depth) {
        sal_free(slice_depth);
    }
#endif

    return (rv);
}
/*
 * Function:
 *    _bcm_field_th_ingress_slice_clear
 *
 * Purpose:
 *    Reset the fields in a slice. Note that the entries list must be
 *    empty before calling this. Also, this does NOT deallocate the memory for
 *    the slice itself. Normally, this is used when a group no longer needs
 *    ownership of a slice so the slice gets returned to the pool of available
 *    slices.
 *
 * Paramters:
 *    unit  - (IN)     BCM device number
 *    fg    - (IN)     Field group structure.
 *    fs    - (IN/OUT) Link to physical slice structure to be cleared
 *
 * Returns:
 *    BCM_E_NONE    - Success
 *    BCM_E_BUSY    - Entries still in slice, can't clear slice
 */
int
_bcm_field_th_ingress_slice_clear(int unit,
                                  _field_group_t *fg,
                                  _field_slice_t *fs)
{
    return _field_th_ingress_slice_clear(unit, fg, fs);
}

/*
 * Function to delete the LT entry from Hardware.
 */
STATIC int
_field_th_lt_hw_entry_delete(int unit,
                             _field_stage_t *stage_fc,
                             _field_group_t *fg,
                             int tcam_idx)
{
    int        rv;
    soc_mem_t  lt_tcam_mem;
    uint32     entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /*Entry buffer*/

    /* Get TCAM view to be used for programming the hardware */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                                     fg->instance,
                                       _BCM_FIELD_MEM_TYPE_IFP_LT,
                          _BCM_FIELD_MEM_VIEW_TYPE_TCAM_DATA_COMB,
                                             &lt_tcam_mem, NULL));
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        rv = (_bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                                                     fg->instance,
                                        _BCM_FIELD_MEM_TYPE_EM_LT,
                                    _BCM_FIELD_MEM_VIEW_TYPE_TCAM,
                                             &lt_tcam_mem, NULL));
    } else {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the entry in hardware. */
    return (soc_th_ifp_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                                                tcam_idx, entry));
}


/*
 * Function:
 *    _bcm_field_th_lt_slice_clear
 *
 * Purpose:
 *    Reset the default LT entry or Preselector entries in a slice. 
 *    Note that the entries list must be empty before calling this.
 *    Also, this does NOT deallocate the memory for the slice itself.
 *    Normally, this is used when a group no longer needs ownership
 *    of a slice so the slice gets returned to the pool of available
 *    slices.
 *
 * Paramters:
 *    unit      - (IN) BCM device number
 *    fg        - (IN) Field group structure.
 *    slice_num - (IN) Slice to be reset.
 *
 * Returns:
 *    BCM_E_xxx
 */
STATIC int
_bcm_field_th_lt_slice_clear(int unit, _field_group_t *fg, int slice_num)
{
    int                   rv, idx;
    int                   tcam_idx;
    int                   part, parts = -1;
    _field_stage_t        *stage_fc;
    _field_control_t      *fc;
    _field_lt_slice_t     *lt_fs;
    _field_lt_slice_t     *lt_part_fs = NULL;
    bcm_field_entry_t      lt_entry;
    _field_lt_entry_t     *lt_f_entry,
                          *lt_f_entry_p = NULL;
    _field_presel_entry_t *f_presel,
                          *f_presel_p;

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Find the LT Slice to be cleared. */
    lt_fs = stage_fc->lt_slices[fg->instance] + slice_num;

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
        for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
            if (fg->presel_ent_arr[idx] != NULL) {
               f_presel = fg->presel_ent_arr[idx];
            } else {
               continue;
            }

            f_presel_p = f_presel;

            tcam_idx = -1;
            for (; f_presel_p != NULL; f_presel_p = f_presel_p->next) {
                lt_part_fs = f_presel_p->lt_fs;

                for (; lt_part_fs != NULL; lt_part_fs=lt_part_fs->next) {
                    if (lt_part_fs == lt_fs) {
                       lt_part_fs->free_count++;
                       lt_part_fs->p_entries[f_presel_p->hw_index] = NULL;
        
                       rv = _bcm_field_presel_entry_tcam_idx_get(unit,
                                        f_presel_p, lt_fs, &tcam_idx);
                       BCM_IF_ERROR_RETURN(rv);
                       rv = _field_th_lt_hw_entry_delete(unit, stage_fc,
                                                          fg, tcam_idx);
                       BCM_IF_ERROR_RETURN(rv);

                       if (f_presel_p->lt_fs == lt_part_fs) {
                           f_presel_p->lt_fs = lt_part_fs->next;
                       }
                       break;
                    }
                }

                if (tcam_idx != -1) {
                   break;
                }
            } 
        }
    } else {
        /* Get entry parts count. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
                                                  fg->flags, &parts));
        lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry;

        /* Get all part of the entry. */
        BCM_IF_ERROR_RETURN(_field_th_lt_entry_get_by_id(unit,
                            lt_entry, &lt_f_entry));
        if (lt_f_entry->lt_fs == NULL) {
            return BCM_E_INTERNAL;
        }

        tcam_idx = -1;
        for (part = 0; part < parts; part++) {
            lt_f_entry_p = lt_f_entry + part;
            lt_part_fs = lt_f_entry_p->lt_fs;
            for (;lt_part_fs != NULL;lt_part_fs=lt_part_fs->next) {
                if (lt_part_fs == lt_fs) {
                    lt_part_fs->free_count++;
                    lt_part_fs->entries[lt_f_entry_p->index] = NULL;

                    rv = _field_th_lt_entry_tcam_idx_get(unit, lt_f_entry_p,
                                                          lt_fs, &tcam_idx);
                    BCM_IF_ERROR_RETURN(rv);
                    rv = _field_th_lt_hw_entry_delete(unit, stage_fc,
                                                       fg, tcam_idx);
                    BCM_IF_ERROR_RETURN(rv);

                    if (lt_f_entry_p->lt_fs == lt_part_fs) {
                        lt_f_entry_p->lt_fs = lt_part_fs->next;
                    }
                    break;
                }
            }
            if (tcam_idx != -1) {
                break;
            }
        }     
    }

    /* Update group's primary lt slice */
    if (fg->lt_slices == lt_fs) {
        fg->lt_slices = lt_fs->next;
    }

    if (lt_fs->free_count == lt_fs->entry_count) {
        if (lt_fs->prev != NULL) {
            lt_fs->prev->next = lt_fs->next;
        }

        if (lt_fs->next != NULL) {
            lt_fs->next->prev = lt_fs->prev;
        }

        lt_fs->next = NULL;
        lt_fs->prev = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_th_group_free_unused_slices
 * Purpose:
 *     Unallocate group unused slices.
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     stage_fc       - (IN) Stage field control structure.
 *     fg             - (IN) Field group structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_free_unused_slices(int unit,
                                       _field_stage_t *stage_fc,
                                       _field_group_t *fg)
{
    _field_slice_t *fs;         /* Field slice structure pointer.           */
    _field_slice_t *fs_next;    /* Field slice structure pointer.           */
    _field_slice_t *fs_ptr;     /* Field slice structure pointer.           */
    uint8 empty;                /* Slice is empty flag.                     */
    int slice;                  /* Group slices iterator.                   */
    int count;                  /* Group slices span count.                 */
    _field_control_t *fc;       /* field control                            */
    _field_meter_pool_t *f_mp;  /* Meter pool descriptor.                   */
    _field_group_t *fg_next = NULL;
    int pri_slice = -1;
    int pool_idx = 0;

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg)) {
        return (BCM_E_PARAM);
    }

    if (NULL == fg->slices) {
        return (BCM_E_INTERNAL);
    }

    fs = fg->slices;
    /* Don't free the slice if it is the last slice in the group */
    if (fs->next == NULL) {
       return BCM_E_NONE;
    }

    while (NULL != fs) {
       if (pri_slice == -1) {
           pri_slice = fs->slice_number;
       }
       fs_next = fs->next;
       BCM_IF_ERROR_RETURN(_field_th_slice_is_empty(unit, fs, &empty));

       if (empty) {
           BCM_IF_ERROR_RETURN(_bcm_field_group_slice_count_get(fs->group_flags,
               &count));

           do {
               /* Retrieve the existing groups associated with the Slice */
               (void)_bcm_field_slice_group_get_next(unit,
                                               fg->instance, fg->stage_id,
                                               fs->slice_number,
                                               &fg_next, &fg_next);
               if (fg_next != NULL) {
                   for (slice = 0; slice < count; slice++) {
                      /* Remove slice from Group's slices linked list. */
                      fs_ptr = fs + slice;
                      LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "%s(): SLICE TO BE detached SLICE:%d fs_ptr:%p fg_slice:%p"
                        " fg_slice_num:%d\n\r\n\r"), __func__,
                        fs_ptr->slice_number, fs_ptr, fg->slices,
                        (fg->slices!=NULL)?fg->slices->slice_number:-1));

                      /* Reset the partition priority for the slice. */
                      BCM_IF_ERROR_RETURN(_bcm_field_th_lt_part_prio_reset(unit,
                                                     stage_fc, fg_next, fs_ptr));
                      /* Clear unused LT slice settings. */
                      BCM_IF_ERROR_RETURN(_bcm_field_th_lt_slice_clear(unit,
                                             fg_next, fs_ptr->slice_number));
                      /* Clear unused slice settings. */
                      BCM_IF_ERROR_RETURN(_field_th_ingress_slice_clear(unit,
                                                            fg_next, fs_ptr));
                   }
               }

               if ((fg_next != NULL) && (fg_next->slices == fs)) {
                   fg_next->slices = fs->next;

                   if (fg_next->presel_ent_arr[0]) {
                      LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                         "$$ GID:[%d] fg->presel_ent_arr[0]->lt_fs:%p slice:%d\n\r"),
                         fg_next->gid, fg_next->presel_ent_arr[0]->lt_fs,
                         (fg_next->presel_ent_arr[0]->lt_fs == NULL) ?
                         -1 : fg_next->presel_ent_arr[0]->lt_fs->slice_number));

                      LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                         "$$        fg->presel_ent_arr[0]->next->lt_fs:%p slice:%d\n\r"),
                          fg_next->presel_ent_arr[0]->next->lt_fs,
                          (fg_next->presel_ent_arr[0]->next->lt_fs == NULL) ?
                          -1 : fg_next->presel_ent_arr[0]->next->lt_fs->slice_number));
                   } 
              } 
           } while(fg_next != NULL);

           /* Get field control structure. */
           BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

           /* When the first slice (primary slice) of the group is released,
            * the meter pool slice_id of the policers attached to
            * the remaining entries are updated to the new primary slice id.
            */

           if ((!(_FP_POLICER_GROUP_SHARE_ENABLE & fc->flags)) &&
               (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) &&
               (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) &&
               (fs->slice_number == pri_slice)) {
               for (pool_idx = 0; pool_idx < stage_fc->num_meter_pools;
                    pool_idx++) {
                   f_mp = stage_fc->meter_pool[fg->instance][pool_idx];
                   if ((f_mp->level == 0) &&
                       (f_mp->slice_id == pri_slice)) {
                       f_mp->slice_id = fs->next->slice_number;
                   }
               }
           }

           /* Unlink the slice from the group's list */
           for (slice = 0; slice < count; slice++) {
               fs_ptr = fs + slice;
               if (fs_ptr->next != NULL) {
                  fs_ptr->next->prev = fs_ptr->prev;
               }

               if (fs_ptr->prev != NULL) {
                  fs_ptr->prev->next = fs_ptr->next;
               }
               fs_ptr->prev = NULL;
               fs_ptr->next = NULL;
           }
       }
       fs = fs_next;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_group_compress
 * Purpose:
 *     Compress field group entries in order to free slices used by the group.
 * Paramters:
 *     unit     - (IN) BCM device number
 *     fg       - (IN) Field group structure
 *     stage_fc - (IN) Stage field control structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_compress(int unit, _field_group_t *fg, _field_stage_t *stage_fc)
{
    _field_slice_t *fs;     /* Field slice structure pointer.   */
    _field_slice_t *efs;    /* Slice that contains free slots.  */
    int eidx;               /* Empty slot index.                */
    int idx;                /* Slice entry iterator.            */
    int tmp_idx1;           /* Slice entry index 1.             */
    int tmp_idx2;           /* Slice entry index 2.             */
    int slice_sz;           /* Number of entries in a slice.    */
    int rv;                 /* Operation return status.         */

    /* Get group slice pointer. */
    fs = fg->slices;

    efs = NULL;
    eidx = -1;

    while (NULL != fs) {
        _BCM_FIELD_ENTRIES_IN_SLICE(unit, fg, fs, slice_sz);
        for (idx = 0; idx < slice_sz; idx++) {
            /* Find an empty slot. */
            if (NULL == fs->entries[idx]) {
                if (NULL == efs) {
                    efs = fs;
                    eidx = idx;
                }
                continue;
            }

            if (NULL != efs) {
                /*
                 * Given Stage, Slice number and Entry offset in the slice,
                 * calculate the TCAM entry indices.
                 */
                BCM_IF_ERROR_RETURN(_bcm_field_th_slice_offset_to_tcam_idx(unit,
                    stage_fc, fg->instance, efs->slice_number, eidx, &tmp_idx1));

                BCM_IF_ERROR_RETURN(_bcm_field_th_slice_offset_to_tcam_idx(unit,
                    stage_fc, fg->instance, fs->slice_number, idx, &tmp_idx2));

                /* Move the entry in hardware. */
                BCM_IF_ERROR_RETURN(_field_th_entry_move(unit, fs->entries[idx],
                    (tmp_idx1 - tmp_idx2)));
                fs = efs;
                idx = eidx;
                efs = NULL;
                eidx = -1;
                _BCM_FIELD_ENTRIES_IN_SLICE(unit, fg, fs, slice_sz);
            }
        }
        fs = fs->next;
    }
    rv = _bcm_field_th_group_free_unused_slices(unit, stage_fc, fg);
    return (rv);
}

/*
 * Function:
 *     _field_th_stage_group_compress
 * Purpose:
 *     Compress field group entries in order to free slices used by the group.
 * Paramters:
 *     unit     - (IN) BCM device number
 *     fg       - (IN) Field group structure
 *     stage_fc - (IN) Stage field control structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_stage_group_compress(int unit,
                               _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    _field_group_t *fg;     /* Field Group structure pointer.   */
    int rv = BCM_E_NONE;    /* Operation return status.         */

    for (fg = fc->groups; fg != NULL; fg = fg->next) {
        /* Skit other stages. */
        if (fg->stage_id != stage_fc->stage_id) {
            continue;
        }

        /* Skit new groups. */
        if (NULL == fg->slices) {
            continue;
        }

        /* Ignore Non-Expanded Groups. */
        if (NULL == fg->slices->next) {
            continue;
        }

        /*
         * Best effort to compress groups that occupy more than one physical
         * table.
         */
        rv = _bcm_field_th_group_compress(unit, fg, stage_fc);
    }
    return (rv);
}

/*
 * Function:
 *     _field_th_group_add_cam_compress
 * Purpose:
 *     Compress other groups to free room for inserted group.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_cam_compress(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Set current stage to be previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    /* Compress expanded groups. */
    fsm_ptr->rv = _field_th_stage_group_compress(unit, fsm_ptr->fc,
                                                 fsm_ptr->stage_fc);

    if (BCM_SUCCESS(fsm_ptr->rv)) {
       /* Retry allocating slice/s for the new field group. */
       fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE;
    } else {
       fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_tcam_part_to_entry_flags
 * Purpose:
 *     Each field entry contains up to 3 TCAM
 *     entries. This routine maps tcam entry (0-2)
 *     to SW entry flags.
 *     Single: 0 (IFP_TCAM view)
 *     Single & Intraslice Double: 0 (IFP_TCAM_WIDE view)
 *     Paired: 0, 1 (IFP_TCAM_WIDE view)
 *     Triple: 0, 1, 2 (IFP_TCAM_WIDE view)
 *     Retrieve the part of the group entry resides in.
 * Parameters:
 *     unit        - (IN)  BCM device number.
 *     entry_flags - (IN)  Entry flags.
 *     group_flags - (IN)  Entry group flags.
 *     entry_part  - (OUT) Entry part (0-2)
 * Returns:
 *     BCM_E_NONE       - Success.
 *     BCM_E_INTERNAL   - Invalid entry part.
 */
int
_bcm_field_th_tcam_part_to_entry_flags(int unit, int entry_part,
                                       uint32 group_flags,
                                       uint32 *entry_flags)
{
    /* Input parameters check. */
    if (NULL == entry_flags) {
        return (BCM_E_PARAM);
    }

    /* Map entry part count to entry flags. */
    switch (entry_part) {
        case 0:
            *entry_flags = _FP_ENTRY_PRIMARY;
            break;
        case 1:
            *entry_flags = _FP_ENTRY_SECONDARY;
            break;
        case 2:
            *entry_flags = _FP_ENTRY_TERTIARY;
            break;
        default:
            return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_group_ltmap_delete
 * Purpose:
 *    Delete Logical Table's association with group.
 * Parameters:
 *    unit      - (IN) BCM device number.
 *    fg        - (IN/OUT) Field group structure pointer.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_ltmap_delete(int unit, _field_group_t *fg)
{
    _field_control_t *fc;       /* Field Control Strucutre.       */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* skip for Flowtracker stage */
    if (fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        return (BCM_E_NONE);
    }

    /* Check if group has a valid logical table information pointer. */
    if (fg->lt_id == -1) {
        return (BCM_E_NONE);
    }

    /* Get field control information. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    fc->lt_info[fg->instance][fg->lt_id]->lt_ref_count--;
    if (fc->lt_info[fg->instance][fg->lt_id]->lt_ref_count > 0) {
        /* Another group is using the same LT ID. So don't destroy it */
        return BCM_E_NONE;
    }

    fc->lt_info[fg->instance][fg->lt_id]->valid = FALSE;
    fc->lt_info[fg->instance][fg->lt_id]->priority = 0;
    fc->lt_info[fg->instance][fg->lt_id]->flags = 0;
    fc->lt_info[fg->instance][fg->lt_id]->lt_action_pri = 0;
    fc->lt_info[fg->instance][fg->lt_id]->lt_entry = 0;

    /* Clear Group's LT_ID information. */
    fg->lt_id = -1;

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_lt_entry_tcam_remove
 * Purpose:
 *     Clears a LT selection TCAM entry for a given index from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_f_ent - (IN) Logical Table entry structure.
 *     tcam_idx - (IN) Entry TCAM index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_lt_entry_tcam_remove(int unit, _field_lt_entry_t *lt_f_ent,
                            int tcam_idx)
{
    _field_stage_t *stage_fc;   /* Stage field control structure.   */
    soc_mem_t lt_tcam_mem;      /* LT TCAM memory name identifier.  */

    /* Input parameters check. */
    if ((NULL == lt_f_ent) || (NULL == lt_f_ent->lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
        lt_f_ent->lt_fs->stage_id, &stage_fc));

    /* Get LT TCAM memory name. */
    BCM_IF_ERROR_RETURN(_field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent,
        &lt_tcam_mem));

    /* Validate supplied TCAM index. */
    if (tcam_idx < soc_mem_index_min(unit, lt_tcam_mem)
        || tcam_idx > soc_mem_index_max(unit, lt_tcam_mem)) {
        return (BCM_E_PARAM);
    }

    /* Clear TCAM entry in hardware at the supplied TCAM index. */
    SOC_IF_ERROR_RETURN(soc_th_ifp_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
        tcam_idx, soc_mem_entry_null(unit, lt_tcam_mem)));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_default_entry_remove
 * Purpose:
 *     Remove a logical table entry from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     lt_entry - (IN) Logical Table entry identifier.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_default_entry_remove(int unit, bcm_field_entry_t lt_entry)
{
    _field_lt_entry_t *lt_f_ent; /* LT entry structure pointer. */
    int parts_count = -1;        /* Entry parts count.          */
    int part;                    /* Entry part.                 */
    int tcam_idx;                /* LT TCAM entry index.        */

    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get(unit, lt_entry,
        _FP_ENTRY_PRIMARY, &lt_f_ent));

    if (!(lt_f_ent->flags & _FP_ENTRY_INSTALLED)) {
        /* Entry not installed in hardware to remove. */
        return (BCM_E_NONE);
    }

    /* Get number of entry parts using group flags. . */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit,
        lt_f_ent->group->flags, &parts_count));

    /* Iterate over entry parts and remove the entry from HW. */
    for (part = 0; part < parts_count; part++) {
        /* Get LT TCAM entry details from hardware. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_lt_tcam_entry_get(unit, lt_f_ent->group,
            lt_f_ent + part));

        /* Given entry part number, get the entry TCAM index. */
        BCM_IF_ERROR_RETURN(_field_th_lt_entry_tcam_idx_get(unit,
            lt_f_ent + part, (lt_f_ent + part)->lt_fs, &tcam_idx));

        /* Clear the entry from hardware. */
        BCM_IF_ERROR_RETURN(_field_lt_entry_tcam_remove(unit, lt_f_ent + part,
            tcam_idx));

        lt_f_ent[part].flags |= _FP_ENTRY_DIRTY;
        lt_f_ent[part].flags &= ~(_FP_ENTRY_INSTALLED);
        lt_f_ent[part].flags &= ~(_FP_ENTRY_ENABLED);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_lt_entry_default_rule_deinit
 * Purpose:
 *     Deinit logical table default rule from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     lt_f_ent - (IN) Logical table entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_lt_entry_default_rule_deinit(int unit, _field_stage_t *stage_fc,
                                       _field_lt_entry_t *lt_f_ent)
{
    int parts_count = 0; /* Entry parts count.               */
    int part;            /* Entry part iterator.             */
    _field_group_t *fg;  /* Field group structure pointer.   */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == lt_f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Get field group structure pointer. */
    fg = lt_f_ent->group;

    /* Get number of entry parts using group flags. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /* Uninstall entry parts keygen program profile entries from hardware. */
    for (part = 0; part < parts_count; part++) {
        BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
            &stage_fc->keygen_profile[fg->instance].profile,
            fg->ext_codes[part].keygen_index));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_ingress_selcodes_uninstall
 * Purpose:
 *     Uninstall group's key generation program from hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field Group control structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_selcodes_uninstall(int unit, _field_stage_t *stage_fc,
                                     _field_group_t *fg)
{
    _field_control_t *fc;         /* Field Control Strucutre.  */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       int                     part, parts_ct = 0;
       int                     idx;
       _field_presel_entry_t  *f_presel;


       /* Get number of entry parts. */
       BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_tcam_parts_count(unit,
                                    fg->stage_id, fg->flags, &parts_ct));

       /*
        * Detach Presel Entries from the LT slice associated with the group
        */
       for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
           if (fg->presel_ent_arr[idx] != NULL) {
              f_presel = fg->presel_ent_arr[idx];
           } else {
              continue;
           }

           /* Uninstall entry parts keygen program profile
            * entries from hardware. */
           if (fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
               for (part = 0; part < parts_ct; part++) {
                   BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                               &stage_fc->keygen_profile[fg->instance].profile,
                               fg->ext_codes[part].keygen_index));
               }
           }

           /* Remove Presel entries from hardware. */
           BCM_IF_ERROR_RETURN(_bcm_field_th_presel_entry_hw_remove
                                                   (unit, f_presel));

           /* Destroy resources allocated for the entry. */
           BCM_IF_ERROR_RETURN(_bcm_field_th_lt_entry_phys_destroy
                                                   (unit, f_presel));
       }
    } else {
       _field_lt_entry_t *lt_f_ent; /* LT field entry structure pointer.  */
       bcm_field_entry_t lt_entry;  /* LT selection Entry ID.             */

        /* Get Entry ID Group's LT information structure. */
       if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
           lt_entry = fc->lt_info[0][fg->lt_id]->lt_entry;
       } else {
           lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry;
       }

       /* Get entry LT entry structure pointer. */
       BCM_IF_ERROR_RETURN(_field_th_lt_entry_get(unit, lt_entry,
                           _FP_ENTRY_PRIMARY, &lt_f_ent));

       /* Remove LT entry from hardware. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_remove(unit, lt_entry));

       /* Delete keygen profiles used by the group. */
       BCM_IF_ERROR_RETURN(_field_th_lt_entry_default_rule_deinit(unit,
                                                     stage_fc, lt_f_ent));

       /* Destroy resources allocated for the entry. */
       BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_phys_destroy(
                                                     unit, lt_f_ent));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_group_uninstall
 * Purpose:
 *     Uninstall group from all slices.
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN) Field stage control structure.
 *     fg       - (IN) Field group.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_group_uninstall(int unit, _field_stage_t *stage_fc,
                                  _field_group_t *fg)
{
    _field_control_t *fc;       /* Field control structure.     */
    _field_slice_t *fs;         /* Field slice structure.       */
    _field_slice_t *temp_fs;    /* Expanded slices iterator.    */
    int parts_count = 0;        /* Number of entry parts.       */
    uint16 group_count = 0;     /* Number of groups.            */
    uint8 slice_number;         /* Slice iterator.              */
    uint8 slice_reset = 0;      /* Slice reset flg.             */
    int idx;                    /* Slice iterator.              */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }
    /* Get Field Control structure pointer. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /*
     * Uninstall TCAM keygen program configuration from hardware for each
     * slice.
     */
    BCM_IF_ERROR_RETURN(_field_th_ingress_selcodes_uninstall(unit, stage_fc,
        fg));

#if defined(BCM_FLOWTRACKER_SUPPORT)
    /* clear keytype bit */
    BCM_IF_ERROR_RETURN(_field_hx5_ft_group_keytype_clear(unit, stage_fc, fg));
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        return (BCM_E_NONE);
    }
#endif

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
        (void)_bcm_field_group_count_get(unit, fg->instance,
                                         stage_fc,
                                         fg->priority,
                                         _FP_GROUP_SPAN_MASK,
                                         &group_count);
        if (group_count <= 1) {
            slice_reset = 1;
        }
    } else {
        slice_reset = 1;
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
       "SLICE CLEAR: Gid:%d prio:%d count:%d slice_num:%d mode:0x%x slice_reset:%d \n\r"),
       fg->gid, fg->priority, group_count, fg->slices->slice_number,
       (fg->flags & _FP_GROUP_SPAN_MASK), slice_reset));

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
        &parts_count));

    /*
     * Parse through the Group parts to clear
     * LT partition priority and slices.
     */
    for (idx = parts_count - 1; idx >= 0; idx--) {
        /* Get Slice ID from TCAM entry part number. */
        BCM_IF_ERROR_RETURN(_bcm_field_th_tcam_part_to_slice_number(idx,
            fg->flags, &slice_number));

        fs = fg->slices + slice_number;

        if (slice_reset == 1) {
           /* Clear slice mode in hardware and reset to HW default. */
           BCM_IF_ERROR_RETURN(_field_th_ingress_slice_mode_set(unit, stage_fc,
                               fs->slice_number, fg, 1));
        }

        while (NULL != fs) {
            temp_fs = fs->next;
            /* Reset the LT partition priority for the slice. */
            BCM_IF_ERROR_RETURN(_bcm_field_th_lt_part_prio_reset(unit,
                                                   stage_fc, fg, fs));
            if (slice_reset == 1) {
                /* Clear slice if in-use by the group alone. */
                BCM_IF_ERROR_RETURN(_field_th_ingress_slice_clear(unit, fg, fs));
                fs->prev = NULL;
                fs->next = NULL;
            }

            fs = temp_fs;
        }
    }

    /*  Free LT_ID association with this group. */
    BCM_IF_ERROR_RETURN(_field_th_group_ltmap_delete(unit, fg));

    /* Install LT Action Priority. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_ingress_lt_action_prio_install(unit,
                                                           stage_fc, fg));
    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_group_lt_prio_update
 * Purpose:
 *    Update a group's logical table action priority value.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     fg       - (IN) Field group.
 *     priority - (IN) New field group priority value.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_lt_prio_update(int unit, _field_group_t *fg,
                                   int priority)
{
    _field_stage_t *stage_fc;   /* Field stage control.         */
    int lt_id;                  /* Logical Table Identifier.    */
    _field_group_t     *fg_ptr = NULL;
    _field_control_t *fc;       /* Field control structure.     */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Return success if there is no change in group's priority value. */
    if (fg->priority == priority) {
        return (BCM_E_NONE);
    }

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->slices == NULL) {
        return BCM_E_INTERNAL;
    }

    do {
        /* Retrieve the existing groups associated with the Slice */
        (void)_bcm_field_slice_group_get_next(unit, fg->instance,
                                                 fg->stage_id,
                                                 fg->slices->slice_number,
                                                 &fg_ptr, &fg_ptr);
        if ((fg_ptr != NULL) && (fg_ptr != fg)) {
           if (!((soc_feature(unit,
               soc_feature_sw_mmu_flush_rqe_operation))
               && (fg_ptr->gid == _FP_INTERNAL_RESERVED_EM_ID))) {
               LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                    "Error: Can not set the priority, group's Slice has more"
                    " than one group.\n\r")));
               return BCM_E_CONFIG;
           }
        }
    } while(fg_ptr != NULL);

    /* For exact match stage group priority cannot change
     * even if group exist with same priority but in different
     * slice.
     */
    if (fg->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        do {
            /* Retrieve the existing groups associated with the priority */
            (void)_bcm_field_th_priority_group_get_next(unit, fg->instance,
                                                        fg->stage_id,
                                                        priority,
                                                        &fg_ptr, &fg_ptr);
            if ((fg_ptr != NULL) && (fg_ptr != fg)) {
                if (!((soc_feature(unit,
                    soc_feature_sw_mmu_flush_rqe_operation))
                    && (fg_ptr->gid == _FP_INTERNAL_RESERVED_EM_ID))) {
                    LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                              "Error: Can not set the priority, exact match"
                              " group(%d) has same priority.\n\r"),
                              fg_ptr->gid));
                    return BCM_E_CONFIG;
                }
            }
        } while(fg_ptr != NULL);
    }

    /* Get current Logical Table ID value assigned to the Group. */
    lt_id = fg->lt_id;

    /* Update group's priority value. */
    fg->priority = priority;

    /* Allocate LT action priority. */
    BCM_IF_ERROR_RETURN(_field_th_lt_priority_alloc(unit, stage_fc, fg,
        lt_id));

    /* Update priority value in Group's logical table information structure. */
    fc->lt_info[fg->instance][lt_id]->priority = fg->priority;

    /* Install LT part mapping to hardware. */
    BCM_IF_ERROR_RETURN(_bcm_field_th_ingress_logical_table_map_write(unit,
                                                             stage_fc, fg));

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_th_group_deinit
 * Purpose:
 *    Destroy field group structure.
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     fg     - (IN) Allocated group structure.
 *
 * Returns:
 *     BCM_E_NONE      - Success
 */
int
_bcm_field_th_group_deinit(int unit, _field_group_t *fg)
{
    int rv = BCM_E_NONE;        /* OPerational status. */
    _field_control_t *fc;       /* Field control structure.       */
    int idx;                    /* Qualifiers iteration index.    */
    _field_stage_t *stage_fc;   /* Stage field control structure. */

    /* Nothing to do as group is already uninstalled. */
    if (NULL == fg) {
        return (BCM_E_NONE);
    }

    /* Get Field Control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, fg->stage_id,
        &stage_fc));

    /* Uninstall group from every slice. */
    if (NULL != fg->slices) {
        _field_th_ingress_group_uninstall(unit, stage_fc, fg);
    }

    /* Deallocate group qualifiers list. */
    for (idx = 0; idx < _FP_MAX_ENTRY_WIDTH; idx++) {
        /* If qualifier set was updated, free original qualifiers arrary. */
        BCM_IF_ERROR_RETURN(_bcm_field_group_qualifiers_free(fg, idx));
    }

    /* Decrement the use counts for any UDFs used by the group */
    for (idx = 0; idx < BCM_FIELD_USER_NUM_UDFS; idx++) {
        if (SHR_BITGET(fg->qset.udf_map, idx))  {
            if (fc->udf[idx].use_count > 0) {
                fc->udf[idx].use_count--;
            }
        }
    }

    /* Deallocate group entry array if any. */
    if (NULL != fg->entry_arr) {
        sal_free(fg->entry_arr);
    }

    /* Deallocate group LT entry array if any. */
    if (NULL != fg->lt_entry_arr) {
        sal_free(fg->lt_entry_arr);
    }
    /* Free Flowtracker stage extractor codes. */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (NULL != fg->ft_ext_codes) {
        sal_free(fg->ft_ext_codes);
    }
#endif
    /* Deallocte the action profile index allocated for group */
    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id ||
        _BCM_FIELD_STAGE_EXACTMATCH ==  fg->stage_id) {
        for (idx =0; idx < MAX_ACT_PROF_ENTRIES_PER_GROUP;idx++) {
            if (-1 != fg->action_profile_idx[idx]) {
                rv = soc_profile_mem_delete(unit,
                                 &stage_fc->action_profile[fg->instance],
                                 fg->action_profile_idx[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    rv = _bcm_field_group_stat_destroy(unit, fg->gid);
    BCM_IF_ERROR_RETURN(rv);

    /* Remove group from unit's group list. */
    BCM_IF_ERROR_RETURN(_bcm_field_group_linked_list_remove(unit, fg));

    /* Decrement Hint count if any. */
    BCM_IF_ERROR_RETURN(
            _bcm_field_hints_group_count_update (unit, fg->hintid, 0));


#if defined (BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_td3_style_fp)) {
        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIncomingOuterVlanId)) {
            bcm_field_qualify_t qual = bcmFieldQualifyIncomingOuterVlanId;

            rv = _bcm_field_td3_field_bus_cont_sel_remove(unit, fg->instance,
                                                          stage_fc, fg, qual);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    /* Deallocate group memory. */
    sal_free(fg);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_group_add_end
 * Purpose:
 *     Group add state machine last/clean up state.
 * Parameters:
 *     unit     - (IN)     BCM device number.
 *     fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_add_end(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /*
     * Clear slice size selection group flags.
     * These internal flags are valid only at the time of group creation.
     * These flags are need not be intersted post group creation.
     * Anyways for Autoexpansion, Autoexpansion hint has to be used
     * for respective group sizes.
     */
    if (fsm_ptr->flags & _BCM_FP_GROUP_ADD_SMALL_SLICE) {
        fsm_ptr->fg->flags &= ~_FP_GROUP_SELECT_SMALL_SLICE;
    } else if (fsm_ptr->flags & _BCM_FP_GROUP_ADD_LARGE_SLICE) {
        fsm_ptr->fg->flags &= ~_FP_GROUP_SELECT_LARGE_SLICE;
    }

    /* Deallocate Field Group memory on failure. */
    if (fsm_ptr->stage_fc != NULL) {
        if (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {

            if (BCM_FAILURE(fsm_ptr->rv)) {
                _bcm_field_th_group_deinit(unit, fsm_ptr->fg);
            }
            else if (!soc_feature(unit, soc_feature_ifp_action_profiling)) {
                if (BCM_FAILURE(fsm_ptr->rv =
                    _field_group_default_aset_set(unit, fsm_ptr->fg))) {
                    _bcm_field_th_group_deinit(unit, fsm_ptr->fg);
                }
            }
        } else if ((fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) ||
                (fsm_ptr->stage_fc->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER)) {
            if (BCM_FAILURE(fsm_ptr->rv)) {
                _bcm_field_th_group_deinit(unit, fsm_ptr->fg);
            }
        } else {
            return (BCM_E_PARAM);
        }
    } else {
        if (BCM_FAILURE(fsm_ptr->rv)) {
            _bcm_field_th_group_deinit(unit, fsm_ptr->fg);
        }
    }

    return (fsm_ptr->rv);
}

/*
 * Function:
 *    _field_th_group_add_adjust_lt_map
 * Purpose:
 *    Update slices logical table mapping and priority in hardware.
 * Parameters:
 *    unit     - (IN)     BCM device number.
 *    fsm_ptr  - (IN/OUT) State machine tracking structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_group_add_adjust_lt_map(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    /* Input parameter check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    /* Set current state to be the previous state. */
    fsm_ptr->fsm_state_prev = fsm_ptr->fsm_state;

    /* Install LT part mapping to hardware. */
    fsm_ptr->rv = _bcm_field_th_ingress_logical_table_map_write(unit,
                                      fsm_ptr->stage_fc, fsm_ptr->fg);

    fsm_ptr->fsm_state = _BCM_FP_GROUP_ADD_STATE_END;

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_group_add
 * Purpose:
 *    Create a field processor group
 * Parameters:
 *    unit      - (IN)     BCM device number.
 *    fsm_ptr   - (IN/OUT) State machine tracking structure.
 * Returns:
 *    BCM_E_PARAM       - Null state machine structure pointer.
 *    BCM_E_UNAVAIL     - Unsupported group create option specified.
 *    BCM_E_RESOURCE    - Requested HW resources unavailable
 *                        or Specified QSET cannot be accomodated with available
 *                        hardware resources.
 *    BCM_E_MEMORY      - Memory allocation failure.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_th_group_add(int unit, _field_group_add_fsm_t *fsm_ptr)
{
    int rv = BCM_E_INTERNAL; /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_PARAM);
    }

    do
    {
        switch (fsm_ptr->fsm_state) {
            case _BCM_FP_GROUP_ADD_STATE_START:
                rv = _field_th_group_add_initialize(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_START\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_ALLOC:
                rv = _field_th_group_add_alloc(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_ALLOC\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_QSET_UPDATE:
                rv = _field_th_group_add_qset_update(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_QSET_UPDATE\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_ASET_VALIDATE:
                rv = _field_th_group_add_aset_validate(unit, fsm_ptr);
                    break;
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_ASET_VALIDATE\r\n")));
                break;

            case _BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET:
                rv = _field_th_group_add_extractor_codes_get(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_SEL_CODES_GET\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE:
                rv = _field_th_group_add_qset_alternate(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_QSET_ALTERNATE\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE:
                rv = _field_th_group_add_slice_allocate(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_SLICE_ALLOCATE\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS:
                rv = _field_th_group_add_cam_compress(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_CAM_COMPRESS\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP:
                rv = _field_th_group_add_adjust_lt_map(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_ADJUST_VIRTUAL_MAP\r\n")));
                break;
            case _BCM_FP_GROUP_ADD_STATE_END:
                rv = _field_th_group_add_end(unit, fsm_ptr);
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                          "Completed state->"
                          "_BCM_FP_GROUP_ADD_STATE_END\r\n")));
                return rv;
                break;
            default:
                /* Not a valid switch-case option. */
                return rv;
        }
    } while(1);

    return (rv);
}

/*
 * Function:
 *    _bcm_field_th_group_add
 * Purpose:
 *    Create a field processor group
 * Parameters:
 *    unit  - (IN)     BCM device number.
 *    flags - (IN)     Input flags.
 *    gc    - (IN/OUT) Group configuration structure pointer.
 * Returns:
 *    BCM_E_PARAM - Null group configuration structure pointer.
 *    BCM_E_UNAVAIL - Unsupported group create option specified.
 *    BCM_E_RESOURCE - Requested HW resources unavailable
 *                     or Specified QSET cannot be accomodated with available
 *                     hardware resources.
 *    BCM_E_MEMORY - Memory allocation failure.
 *    BCM_E_NONE - Success.
 */
int
_bcm_field_th_group_add(int unit, uint32 flags,
                        bcm_field_group_config_t *gc)
{
    int                    rv = BCM_E_INTERNAL; /* Operation return status. */
    _field_stage_id_t      stage;    /* FP stage information. */
    _field_group_add_fsm_t fsm;      /* Group creation state machine.    */
    _field_control_t       *fc;
    _field_hints_t         *f_ht = NULL;
                                     /* Field hints Structure. */
    _field_hint_t          *hint_list = NULL;
                                     /* Field Hints Structure. */
    bcm_field_hint_t       *hint_entry;
                                     /* Hint Entry.            */
    bcm_pbmp_t             all_pbmp;   /* Group port bitmap.           */

    /* Input parameter check. */
    if (NULL == gc) {
        return (BCM_E_PARAM);
    }

    /* Get unit FP control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /*
     * Check and return error when SMALL or LARGE slice request flag is set
     * for TH device.
     */
    if ((0 == soc_feature(unit, soc_feature_field_ingress_two_slice_types))
        && (gc->flags & BCM_FIELD_GROUP_CREATE_SMALL
            || gc->flags & BCM_FIELD_GROUP_CREATE_LARGE)) {
        return (BCM_E_UNAVAIL);
    }


    /* Get the FP stage in which new group must be created. */
    BCM_IF_ERROR_RETURN(_bcm_field_group_stage_get(unit, &gc->qset, &stage));
    BCM_IF_ERROR_RETURN(_bcm_field_group_stage_validate(unit, stage));

    if (!(gc->flags & BCM_FIELD_GROUP_CREATE_WITH_ID)) {
        /* qset and pri are validated in bcm_field_group_create_mode_id(). */
        BCM_IF_ERROR_RETURN(_bcm_field_group_id_generate(unit, &gc->group));
    }

    if (soc_property_get(unit, spn_FLOWTRACKER_ENABLE, 0) == 1) {
    /*
     * Return BCM_E_UNAVAIL if soc property FLOWTRACKER_ENABLE is enabled and
     * As flow tracker creates ExactMatch group during
     * their initialization and User doesn't have provision to create groups under ExactMatch stage.
     */
        if ((_BCM_FIELD_STAGE_EXACTMATCH == stage) &&
            (0 == (flags & _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC))) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Error: ExactMatch Field stage is dedicated to Flowtracker module.\n\r")));
            return BCM_E_UNAVAIL;
        }

        if ((flags & _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC) &&
             (_BCM_FIELD_STAGE_EXACTMATCH != stage)){
            /* FLOWTRACKER SPECIFIC flag should be used for EXACTMATCH Stage only */
            return BCM_E_PARAM;
        }

        /* Check to Restrict EM for internal use */
        if (((_FP_FLOWTRACKER_RESERVED_EM_GID_AFTER_SHIFT) ==
             ((uint32)gc->group >> (_FP_FLOWTRACKER_RESERVED_EM_GID_SHIFT_VAL))) &&
             (0 == (flags & _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC))) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Error: Field Group Id : %d is internally reserved for Flowtracker module.\n\r"),
                            gc->group));
            return BCM_E_UNAVAIL;
        }

        /* Validation check for flowtracker module to use only reserved _FP_FLOWTRACKER_RESERVED_EM_GID_* GIDs  */
        if (((_FP_FLOWTRACKER_RESERVED_EM_GID_AFTER_SHIFT) !=
             ((uint32)gc->group >> (_FP_FLOWTRACKER_RESERVED_EM_GID_SHIFT_VAL))) &&
             (flags & _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC)) {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Error: Field Group Id : %d is not reserved for Flowtracker module. \n\r"),
                            gc->group));
            return BCM_E_CONFIG;
        }
    } else {
        if (flags & _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC) {
            /* FLOWTRACKER SPECIFIC flag should be used only if soc property FLOWTRACKER_ENABLE is enabled */
            return BCM_E_PARAM;
        }
    }

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

    /* Initialize group creation tracking structure. */
    sal_memset(&fsm, 0, sizeof(_field_group_add_fsm_t));

    /* Initialize group creation tracking structure. */
    fsm.fsm_state = _BCM_FP_GROUP_ADD_STATE_START;
    fsm.priority = gc->priority;
    fsm.group_id = gc->group;
    fsm.qset = gc->qset;
    fsm.aset = gc->aset;
    fsm.rv = BCM_E_NONE;
    fsm.action_res_id = gc->action_res_id;
    fsm.hintid = gc->hintid;

    /* Apply slice selection configuration */
    if (gc->flags & BCM_FIELD_GROUP_CREATE_SMALL) {
        fsm.flags |= _BCM_FP_GROUP_ADD_SMALL_SLICE;
    } else if (gc->flags & BCM_FIELD_GROUP_CREATE_LARGE) {
        fsm.flags |= _BCM_FP_GROUP_ADD_LARGE_SLICE;
    }

    if (fsm.hintid != 0) {
        if (fsm.hintid >= _FP_HINT_ID_MAX) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "HintId is out of allowed range\r\n")));
            rv = BCM_E_PARAM;
            return (rv);
        }
        if (_FP_HINTID_BMP_TEST(fc->hintid_bmp, fsm.hintid) == 0) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Trying to attach hintid which is"
                                " not created.\r\n")));
            rv = BCM_E_NOT_FOUND;
            return (rv);
        }
        if ((BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageExternal)) ||
            (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageClass)) ||
            (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageClassExactMatch))) {

            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit,"Unsupported hint type attached"
                                     "to hintid(%d).\r\n"),
                                      gc->hintid));
             rv = BCM_E_PARAM;
             return (rv);
        }

        BCM_IF_ERROR_RETURN(_field_hints_control_get (unit, gc->hintid, &f_ht));
        if (f_ht == NULL) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,"Hints are not attached to hintid(%d)."
                                "\r\n"),
                                gc->hintid));
            rv = BCM_E_NOT_FOUND;
            return (rv);
        }

        hint_list = f_ht->hints;
        while (hint_list != NULL) {
            hint_entry = hint_list->hint;
            if (hint_entry != NULL) {
                /* hint type _field_hints_control_get is not supported for any stage */
                if (hint_entry->hint_type ==
                                 bcmFieldHintTypeCompression) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                       (BSL_META_U(unit,"Unsupported hint type attached"
                                             "to hintid(%d).\r\n"),
                                              gc->hintid));
                    rv = BCM_E_PARAM;
                    return (rv);
                }

                if (!soc_feature(unit, soc_feature_field_ingress_two_slice_types)) {
                    /*
                     * For Ingress state, hint type auto expansion is
                     * supported only for flags BCM_FIELD_GROUP_MAX_SIZE_HARD_LIMIT
                     */
                    if((BCM_FIELD_QSET_TEST(fsm.qset,
                                    bcmFieldQualifyStageIngress)) &&
                            (hint_entry->hint_type ==
                             bcmFieldHintTypeGroupAutoExpansion)) {

                        if(hint_entry->flags != BCM_FIELD_GROUP_MAX_SIZE_HARD_LIMIT) {
                            LOG_DEBUG(BSL_LS_BCM_FP,
                                    (BSL_META_U(unit,"Unsupported hint type attached"
                                                "to hintid(%d).\r\n"),
                                     gc->hintid));
                            rv = BCM_E_PARAM;
                            return (rv);
                        }
                    }
                }
                /* for egress and lookup stages, only the hint type
                 * bcmFieldHintTypeGroupAutoExpansion along with flag
                 * BCM_FIELD_GROUP_MAX_SIZE_HARD_LIMIT is supported
                 */

                if ((BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageEgress)) ||
                    (BCM_FIELD_QSET_TEST(fsm.qset, bcmFieldQualifyStageLookup))) {

                    if(!( (hint_entry->hint_type ==
                              bcmFieldHintTypeGroupAutoExpansion) &&
                          (hint_entry->flags == BCM_FIELD_GROUP_MAX_SIZE_HARD_LIMIT) ) ) {
                        LOG_DEBUG(BSL_LS_BCM_FP,
                       (BSL_META_U(unit,"Unsupported hint type attached"
                                             "to hintid(%d).\r\n"),
                                              gc->hintid));
                        rv = BCM_E_PARAM;
                        return (rv);
                    }
                }
            }
            hint_list = hint_list->next;
        }
    }

    /* Apply group member port configuration. */
    if (gc->flags & BCM_FIELD_GROUP_CREATE_WITH_PORT) {
        BCM_PBMP_ASSIGN(fsm.pbmp, gc->ports);
    } else {
        BCM_PBMP_ASSIGN(fsm.pbmp, all_pbmp);
    }

    /* Apply group mode configuration */
    if (gc->flags & BCM_FIELD_GROUP_CREATE_WITH_MODE) {
        fsm.mode = gc->mode;
    } else {
        fsm.mode = bcmFieldGroupModeDefault;
    }

    /* Apply Validate flag if the group needs to be created partially */
    if (flags & _FP_GROUP_CONFIG_VALIDATE) {
       fsm.flags |= _FP_GROUP_CONFIG_VALIDATE;
    }

    /* Validate and Apply group Preselset configuration */
    if (gc->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESELSET) {
       fsm.preselset = gc->preselset;
       fsm.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    }

#ifdef BCM_FIREBOLT6_SUPPORT
    if (gc->flags & BCM_FIELD_GROUP_CREATE_WITH_COMPRESSION) {
        fsm.flags |= BCM_FIELD_GROUP_CREATE_WITH_COMPRESSION;
    }
#endif

    switch (stage) {
        case _BCM_FIELD_STAGE_LOOKUP:
        case _BCM_FIELD_STAGE_EGRESS:
        case _BCM_FIELD_STAGE_AMFTFP:
        case _BCM_FIELD_STAGE_AEFTFP:
            /*  Add FP group. */
            rv = _bcm_field_group_add(unit, &fsm);
            break;
        case _BCM_FIELD_STAGE_EXACTMATCH:
            /*  Add FP group under EM Stage. */
            rv = _field_th_group_add(unit, &fsm);
            if (BCM_SUCCESS(rv) &&
                (soc_property_get(unit, spn_FLOWTRACKER_ENABLE, 0) == 1) &&
                (flags & _FP_GROUP_CONFIG_FLOWTRACKER_SPECIFIC)) {
                /*
                 * _field_detach shoud not destroy Exact Match groups created by Flow Tracker.
                 * So Setting _FP_EXACTMATCH_STAGE_SKIP_DETACH flag to retain Exact Match Stage.
                 */
                fc->flags |= _FP_EXACTMATCH_STAGE_SKIP_DETACH;
            }
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            rv = _field_th_group_add(unit, &fsm);
            break;
        case _BCM_FIELD_STAGE_CLASS:
            rv = _bcm_field_th_class_group_add(unit, &fsm, gc->aset);
            break;
#if defined(BCM_FLOWTRACKER_SUPPORT)
        case _BCM_FIELD_STAGE_FLOWTRACKER:
            if ((!soc_feature(unit, soc_feature_field_flowtracker_support)) ||
                 (soc_property_get(unit, spn_FLOWTRACKER_ENABLE, 1) != 1)) {
                return (BCM_E_UNAVAIL);
            }
            rv = _field_th_group_add(unit, &fsm);
            break;
#endif
        default:
            /* Invalid FP stage. */
            return (BCM_E_PARAM);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return (rv);
}

/*
 * Function:
 *    _field_th_ingress_selcodes_update
 * Purpose:
 *    Install Logical table selection rule and TCAM key generation rules in
 *    hardware.
 * Parameters:
 *     unit         - (IN)  BCM device number.
 *     stage_fc     - (IN)  Stage field control structure.
 *     fg           - (IN)  Field group structure.
 *     lt_fs        - (IN)  Field logical table slice structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_selcodes_update(int unit, _field_stage_t *stage_fc,
                                   _field_group_t *fg, _field_lt_slice_t *lt_fs)
{
    int   rv;            /* Operation return status.    */
    int   idx;           /* iteration index.            */
    _field_control_t *fc; /* Field Control Structure.   */
    bcm_field_entry_t lt_entry; /* LT entry ID. */
    _field_lt_entry_t *lt_f_ent; /* LT field entry structure pointer.  */
    uint8 part;
    int parts_count = 0; /* number of slices allocated for the group. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       _field_presel_entry_t  *f_presel;

       /*
        * Create Presel Entries in the LT slice associated with the group
        */
       for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
           if (fg->presel_ent_arr[idx] != NULL) {
              f_presel = fg->presel_ent_arr[idx];
           } else {
              continue;
           }

           rv = _field_th_lt_entry_data_config_set(unit, stage_fc, f_presel);
           if (BCM_FAILURE(rv)) {
              return (rv);
           }

           /* Install the LT Entry in HW. */
           rv = _bcm_field_th_lt_entry_parts_install(unit, f_presel);
           BCM_IF_ERROR_RETURN(rv);

           /* Check if the group is expanded across slices. */
           if (f_presel->lt_fs->next != NULL) {
              _field_lt_slice_t     *lt_part_fs;
              _field_presel_entry_t *f_presel_p;
              int                   p, parts_count = -1;

              /* Get number of entry parts. */
              rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                                        &parts_count);
              BCM_IF_ERROR_RETURN(rv);

              f_presel_p = f_presel;
              for (p = 0; (p < parts_count && f_presel_p != NULL); p++) {
                  lt_part_fs = f_presel_p->lt_fs->next;
                  for (;lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
                     rv = _field_th_ingress_group_expand_install(unit, stage_fc,
                                                             fg, p, lt_part_fs);
                     BCM_IF_ERROR_RETURN(rv);
                  }
              }
           }
       }
    } else {
       rv = _bcm_field_th_entry_tcam_parts_count(unit,fg->flags,&parts_count);
       if (BCM_FAILURE(rv)) {
            return rv;
       }
       if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
           lt_entry = fc->lt_info[0][fg->lt_id]->lt_entry;
       } else {
           lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry;
       }
       BCM_IF_ERROR_RETURN(_field_th_lt_entry_get(unit, lt_entry,
           _FP_ENTRY_PRIMARY, &lt_f_ent));

      for (part = 0; part < parts_count; part++) {
          BCM_IF_ERROR_RETURN(_bcm_field_th_group_keygen_profiles_index_alloc(
              unit, stage_fc, fg, part));

          BCM_IF_ERROR_RETURN(_field_th_lt_entry_default_data_value_set(unit,
              stage_fc, fg, part, lt_f_ent + part));
          lt_f_ent[part].flags |= _FP_ENTRY_DIRTY;
      }
      BCM_IF_ERROR_RETURN(_field_th_lt_default_entry_install(unit, lt_entry));
   }

    return (BCM_E_NONE);
}

int
_bcm_field_th_group_update(int unit, bcm_field_group_t group, bcm_field_qset_t *qset)
{
    int rv = BCM_E_NONE; /* Return status.  */
    int idx = 0;         /* Index Variable. */
    int qid_index = 0;   /* Index Variable. */
    int qid;             /* Qualifier ID.   */
    int parts_count = 0; /* number of slices allocated for the group. */
    uint8 part;          /* Group Partition. */
    uint16 *qid_array[_FP_MAX_ENTRY_WIDTH] = { NULL };
                                            /* Array of qualifiers.    */
    uint16 size = 0;                        /* Total number of qualifiers in
                                               each partition of the group. */
    _field_group_t *fg = NULL;           /* Field group operational Structure. */
    _field_group_t *temp_fg = NULL;      /* Temp field group operational structure. */
    _field_stage_t *stage_fc = NULL;     /* Field Stage operational structure. */
    bcm_field_qset_t temp_qset;          /* Temporary qset information. */
    _bcm_field_qual_offset_t  *offset_array[_FP_MAX_ENTRY_WIDTH] = { NULL };
                                         /* Array of qualifiers offset information. */
    _field_group_add_fsm_t fsm_ptr;
    _field_presel_entry_t  *f_presel;
    uint32 max_udf_chunks = 0;
    int indx = 0;

    if(qset == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc));

    sal_memcpy(&temp_qset, qset, sizeof(bcm_field_qset_t));

    for (qid = 0; qid < _bcmFieldQualifyCount; qid++) {

        if (BCM_FIELD_QSET_TEST(temp_qset, qid) &&
            !BCM_FIELD_QSET_TEST(fg->qset, qid) &&
            IS_POST_MUX_QUALIFIER(qid)) {
            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                        "FP(unit %d) Verb: Adding new post mux"
                        "qualifiers to QSET is not supported.\n"), unit));
            return BCM_E_PARAM;
        }

        if (BCM_FIELD_QSET_TEST(fg->qset, qid)) {
            BCM_FIELD_QSET_REMOVE(temp_qset, qid);
        }
    }

    _FP_XGS3_ALLOC(temp_fg, sizeof(_field_group_t), "Temp group for update");
    if (NULL == temp_fg) {
        return BCM_E_MEMORY;
    }

    temp_fg->gid = fg->gid;
    temp_fg->flags = fg->flags;
    sal_memcpy(&temp_fg->qset, &temp_qset, sizeof(bcm_field_qset_t));
    sal_memcpy(temp_fg->ext_codes, fg->ext_codes,
                      (sizeof(_field_ext_sel_t) * _FP_MAX_ENTRY_WIDTH));
    sal_memset(&fsm_ptr, 0, sizeof(_field_group_add_fsm_t));
    fsm_ptr.fg = temp_fg;
    fsm_ptr.stage_fc = stage_fc;
    fsm_ptr.flags = _FP_GROUP_QSET_UPDATE;
    rv = _field_th_ext_code_assign(unit, 0, &fsm_ptr);

    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "FP(unit %d) Verb: Processing group QSET.\n"), unit));
        goto cleanup;
    }

    for (part = 0; part < _FP_MAX_ENTRY_WIDTH; part++)  {
        size = 0;

        size = temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size +
                fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size;

        if (size != 0) {

             _FP_XGS3_ALLOC(qid_array[part], size * sizeof(uint16), "Qid Array");
             if (NULL == qid_array[part]) {
                rv = BCM_E_MEMORY;
                goto cleanup;
             }

             _FP_XGS3_ALLOC(offset_array[part], size * sizeof(_bcm_field_qual_offset_t),
                                                        "Qualifier Offset Array");
             if (NULL == offset_array[part]) {
                rv = BCM_E_MEMORY;
                goto cleanup;
             }


             qid_index = 0;
             for (idx = 0; idx < fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size; idx++) {

                 sal_memcpy(&qid_array[part][qid_index],
                        &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr[idx]),
                        sizeof(uint16));

                 sal_memcpy(&offset_array[part][qid_index],
                            &(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr[idx]),
                            sizeof(_bcm_field_qual_offset_t));

                 qid_index++;

             }

             fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size = size;

             /* Copy qualifier array and offset array of new
              * qualifiers in this partition.
              */
             size = temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].size;
             sal_memcpy(&qid_array[part][qid_index],
                        temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr,
                        size * sizeof(uint16));

             sal_memcpy(&offset_array[part][qid_index],
                        temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr,
                        size * sizeof(_bcm_field_qual_offset_t));

             if (NULL != fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr) {
                 sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr);
             }

             if (NULL != fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr) {
                 sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr);
             }

             if (NULL != temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr) {
                 sal_free(temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr);
             }

             if (NULL != temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr) {
                 sal_free(temp_fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr);
             }

             fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].qid_arr = qid_array[part];
             fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part].offset_arr = offset_array[part];
        }
    }
    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
        for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
            if (fg->presel_ent_arr[idx] != NULL) {
               f_presel = fg->presel_ent_arr[idx];
            } else {
               continue;
            }
            parts_count = 0;
            rv = _bcm_field_th_lt_entry_tcam_parts_count(unit,
                                f_presel->group->stage_id,
                                f_presel->group->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            /* Uninstall entry parts keygen program profile entries from hardware. */
           for (part = 0; part < parts_count; part++) {
               BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                   &stage_fc->keygen_profile[fg->instance].profile,
                   fg->ext_codes[part].keygen_index));
           }

        }
    } else {
        rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Uninstall entry parts keygen program profile entries from hardware. */
        for (part = 0; part < parts_count; part++) {
            rv = soc_profile_mem_delete(unit,
                &stage_fc->keygen_profile[fg->instance].profile,
                fg->ext_codes[part].keygen_index);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

    for (part = 0; part < _FP_MAX_ENTRY_WIDTH; part++)  {
         offset_array[part] = NULL;
         qid_array[part] = NULL;
    }
    sal_memcpy(fg->ext_codes, temp_fg->ext_codes,
                      (sizeof(_field_ext_sel_t) * _FP_MAX_ENTRY_WIDTH));

    rv = _field_th_ingress_selcodes_update(unit, stage_fc, fg,fg->lt_slices);
    if (BCM_FAILURE(rv)) {
            goto cleanup;
    }
    for (qid = 0; qid < _bcmFieldQualifyCount; qid++) {
        if (BCM_FIELD_QSET_TEST(*qset, qid)) {
            BCM_FIELD_QSET_ADD(fg->qset, qid);
        }
    }

    max_udf_chunks = _SHR_BITDCLSIZE(BCM_FIELD_USER_NUM_UDFS);

    for ( indx=0; indx < max_udf_chunks; indx++) {
       fg->qset.udf_map[indx] |=  qset->udf_map[indx];
    }

cleanup:
    for (part = 0; part < _FP_MAX_ENTRY_WIDTH; part++) {
         if (NULL != qid_array[part]) {
             sal_free(qid_array[part]);
             qid_array[part] = NULL;
         }

         if(NULL != offset_array[part]) {
             sal_free(offset_array[part]);
             offset_array[part] = NULL;
         }
    }

    if (NULL != temp_fg) {
        sal_free(temp_fg);
    }

    return (rv);
}
/*
 * Function: _bcm_field_th_find_empty_slice
 *
 * Purpose:
 *     Get a new slice for an existing group if available.
 *     Used for auto-expansion of groups
 *     Currently: supported in Raptor, Firebolt2.
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     fg     - (IN) field group
 *     fs_ptr - (IN/OUT) _field_slice_t for the PRIMARY if there is one available.
 *
 * Returns:
 *     BCM_E_xxx
 */
int
_bcm_field_th_find_empty_slice(int unit, _field_group_t *fg,
                               _field_slice_t **fs_ptr)
{
    _field_control_t *fc = NULL;       /* Field control structure. */
    _field_stage_t *stage_fc = NULL;   /* Field stage control.     */
    _field_slice_t   *fs = NULL;       /* Field slice pointer.     */
    uint8 slice_idx = 0;               /* Slice iterator.          */
    int parts_count = 0;               /* Number of entry parts.   */
    uint32 entry_flags = 0;            /* Field entry part flags.  */
    int rv = BCM_E_NONE;               /* Operation return status. */
    int lt_part_prio = 0;              /* LT Partition priority.   */
    int part_index = 0;
    uint8 old_physical_slice = 0;      /* Last slice in group.     */
    uint8 new_physical_slice = 0;      /* Allocated slice.         */
    _field_group_t  *curr_fg = NULL;
    _field_group_t  *next_fg = NULL;
    uint8 *slice_arr = NULL;

    /* Get field control structure. */
    rv = _field_control_get(unit, &fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    if (0 == (stage_fc->flags & _FP_STAGE_AUTO_EXPANSION)) {
        return (BCM_E_RESOURCE);
    }

    fs = &fg->slices[0];
    while (fs->next != NULL) {
        fs = fs->next;
    }
    old_physical_slice = fs->slice_number;

    _FP_XGS3_ALLOC(slice_arr, sizeof(uint8) * stage_fc->tcam_slices,
                      "Slice Array");
    if (NULL == slice_arr) {
        return BCM_E_MEMORY;
    }

    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        slice_arr[slice_idx] = slice_idx;
    }

    if ((fg->flags & _FP_GROUP_AUTO_EXPAND_SMALL_SLICE)
            || (fg->flags & _FP_GROUP_AUTO_EXPAND_LARGE_SLICE)){
        /* Sort the slice array based on requested slice sizes */
        rv = _field_fill_slice_array(unit, fg, slice_arr);
        BCM_IF_ERROR_CLEANUP(rv);
    }

    /*
     * Find an empty slice
     * Validate the ports and mode
     */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        if (slice_arr[slice_idx] == old_physical_slice) {
           continue;
        }

        rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                                                    slice_arr[slice_idx]);
        if (BCM_SUCCESS(rv)) {
            uint8 empty;

            fs = stage_fc->slices[fg->instance] + slice_arr[slice_idx];
            BCM_IF_ERROR_CLEANUP(_field_th_slice_is_empty(unit, fs, &empty));
            if (empty == TRUE) {
               break;
            }
        }
    }

    if (slice_idx == stage_fc->tcam_slices) {
        /* No free slice; attempt to compress existing auto expanded groups. */
        rv = _field_th_stage_group_compress(unit, fc, stage_fc);
        BCM_IF_ERROR_CLEANUP(rv);

        /* Retry slice allocation. */
        for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
            _field_slice_t *fs_next = NULL;
            uint8 curr_grp_slice = 0;

            for (fs_next = &fg->slices[0]; fs_next; fs_next = fs_next->next) {
                /* Don't use current group's slices */
                if (fs_next->slice_number == slice_arr[slice_idx]) {
                    curr_grp_slice = 1;
                    break;
                }
            }

            if (1 != curr_grp_slice) {
                rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                                                            slice_arr[slice_idx]);
                if (BCM_SUCCESS(rv)) {
                    break;
                }
            }
        }

        if (slice_idx == stage_fc->tcam_slices)
        {
           /* No free slice */
           rv = BCM_E_RESOURCE;
           BCM_IF_ERROR_CLEANUP(rv);
        }
    }

    new_physical_slice = slice_arr[slice_idx];

    /* Get number of entry parts for the group. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags, &parts_count);
    BCM_IF_ERROR_CLEANUP(rv);

    /* Retrieve the partition priority value for wide mode group's slices.*/
    rv = _bcm_field_th_lt_part_prio_value_get(unit, fg, &lt_part_prio);
    BCM_IF_ERROR_CLEANUP(rv);

    /* Set up the new physical slice parameters in Software */
    for(part_index = 0; part_index < parts_count; part_index++) {
        /* Get entry flags. */
        rv = _bcm_field_th_tcam_part_to_entry_flags(unit, part_index, fg->flags,
                                                    &entry_flags);
        BCM_IF_ERROR_CLEANUP(rv);

        /* Get slice id for entry part */
        rv = _bcm_field_th_tcam_part_to_slice_number(part_index, fg->flags,
                                                     &slice_arr[slice_idx]);
        BCM_IF_ERROR_CLEANUP(rv);

        /* Get slice pointer. */
        fs = stage_fc->slices[fg->instance] + new_physical_slice
                + slice_arr[slice_idx];

        /* Enable slice. */
        if (0 == (entry_flags & _FP_ENTRY_SECOND_HALF)) {
            /*
             * Initialize slice free entries count value based on group
             * flags.
             */
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit) &&
                (fg->stage_id == _BCM_FIELD_STAGE_INGRESS)) {
                fs->free_count = fs->entry_count;
            } else
#endif
            {
                if ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                    && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
                    fs->free_count = fs->entry_count;
                } else {
                    fs->entry_count >>= 1;
                    fs->free_count = fs->entry_count;
                    fs->start_tcam_idx >>= 1;
                }
            } 

            /* Set group flags in in slice.*/
            fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;

            /* Add slice to slices linked list . */
            stage_fc->slices[fg->instance][old_physical_slice + slice_arr[slice_idx]].next = fs;
            fs->prev =
             &stage_fc->slices[fg->instance][old_physical_slice + slice_arr[slice_idx]];
        }

        curr_fg = NULL;
        next_fg = NULL;
        do {
           rv = _bcm_field_slice_group_get_next(unit, fg->instance,
                                                   fg->stage_id,
                                                   old_physical_slice,
                                                   &curr_fg, &next_fg);
           /* Mark it as SUCCESS when you reach the end of the group's slices */
           if (BCM_E_NOT_FOUND == rv) {
               rv = BCM_E_NONE;
               break;
           } else if (BCM_FAILURE(rv)) {
               break;
           }

          /*
           * Install TCAM keygen program configuration in hardware for all
           * the groups for each slice.
           */
           rv = _field_th_ingress_group_expand_slice_install(unit, stage_fc,
                                           next_fg,
                                           new_physical_slice + slice_arr[slice_idx],
                                           part_index, lt_part_prio);
           BCM_IF_ERROR_CLEANUP(rv);
           curr_fg = next_fg;
        } while (next_fg != NULL);
    }

    *fs_ptr = stage_fc->slices[fg->instance] + new_physical_slice;

cleanup:
    if (NULL != slice_arr) {
        sal_free(slice_arr);
        slice_arr = NULL;
    }
    return (rv);
}

/*
 * Function: _bcm_field_th_group_enable_set
 *
 * Purpose:
 *     To enable/diable the field group operational status.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     group  - (IN) field group
 *     enable - (IN) enable (or) disbale flag.
 *
 * Returns:
 *     BCM_E_xxx
 */
int
_bcm_field_th_group_enable_set(int unit, bcm_field_group_t group, uint32 enable)
{
    int rv;                             /* operational status */
    int tcam_idx[_FP_MAX_ENTRY_WIDTH];  /* Entry TCAM index.  */
    int parts_count = -1;               /* Parts count.       */
    int idx;                         /* Temporary pointer variable. */
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* Entry buffer. */
    _field_group_t *fg;           /* Field Group Structure. */
    soc_mem_t lt_tcam_mem;        /* TCAM memory ID. */
    bcm_field_entry_t lt_entry;   /* Logical table entry. */
    _field_lt_entry_t *lt_f_ent;  /* Field LT entry pointer. */
    _field_stage_t *stage_fc;     /* Stage Field control structure. */
    _field_control_t *fc;         /* Field Control Strucutre.       */

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get the group control pointer. */
    BCM_IF_ERROR_RETURN(_field_group_get(unit, group, &fg));

    /* Get the stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            fg->stage_id, &stage_fc));

    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
        return _bcm_field_th_group_presel_enable_set(unit, stage_fc, fg, enable);
    }

    lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry;

    /* Get all part of the entry. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get_by_id(unit, lt_entry,
        &lt_f_ent));

    /* Get number of entry parts. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, lt_f_ent->group->flags,
            &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < parts_count; idx++) {
        rv = _field_th_lt_entry_tcam_idx_get(unit, lt_f_ent + idx,
                (lt_f_ent + idx)->lt_fs, tcam_idx + idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    /*
     * Get TCAM view to be used for programming the hardware based on the group
     * operational mode.
     */
    rv = _field_th_lt_tcam_mem_get(unit, stage_fc, lt_f_ent, &lt_tcam_mem);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < parts_count; idx++)  {
        /*
         * Maximum index value for entry in this TCAM is limited to
         * No. of slices * No. of LT_IDs/slice (12 * 32 == 384).
         */
        if ((tcam_idx[idx] < soc_mem_index_min(unit, lt_tcam_mem))
            || (tcam_idx[idx] > (stage_fc->lt_tcam_sz - 1) )) {
            return (BCM_E_PARAM);
        }

        /* Clear entry buffer. */
        sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);

        /* Read entry into SW buffer. */
        rv = soc_th_ifp_mem_read(unit, lt_tcam_mem, MEM_BLOCK_ANY, tcam_idx[idx],
                entry);
        BCM_IF_ERROR_RETURN(rv);

        /*valid bit in th2 is two bit width*/
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (soc_feature(unit, soc_feature_field_multi_pipe_enhanced) &&
            _BCM_FIELD_STAGE_INGRESS  == fg->stage_id) {
            enable = enable ? 3 : 0;
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        {
            enable = enable ? (soc_feature(unit, soc_feature_td3_style_fp) ? 3 : 1) : 0;
        }
        soc_mem_field32_set(unit, lt_tcam_mem, entry, VALIDf, enable);

        /* Install entry in hardware. */
        rv = soc_th_ifp_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL, tcam_idx[idx],
                entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Enable/Disable group flags. */
    if (enable) {
        fg->flags |= _FP_GROUP_LOOKUP_ENABLED;
    } else {
        fg->flags &= ~_FP_GROUP_LOOKUP_ENABLED;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_th_group_status_calc
 * Purpose:
 *    Fill in the values of a group status structure.
 * Parameters:
 *    unit - (IN)     BCM device number.
 *    fg   - (IN/OUT) Group to update the status structure.
 * Returns:
 *    BCM_E_NONE - Success.
 */
int
_bcm_field_th_group_status_calc(int unit, _field_group_t *fg)
{
    bcm_field_group_status_t *status;   /* Group status structure pointer. */
    _field_stage_t *stage_fc;           /* Stage field control structure.  */
    _field_stage_t *stage_ingress;      /* Stage field control structure.  */
    _field_slice_t *fs;                 /* Field slice control structure.  */
    int count = 0;                      /* Generic purposes counter.       */
    int rv = BCM_E_NONE;                /* Operation return status.        */
    int slice_index;                    /* Hardware slice index.           */
    uint8 single_wide = 0;              /* Single wide group indicator.    */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get field group stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, fg->stage_id, &stage_fc));

    /* Get field group status structure pointer. */
    status = &fg->group_status;
    if (NULL == status) {
        return (BCM_E_INTERNAL);
    }

    /* Get group primary slice structure pointer. */
    fs = &fg->slices[0];

    /* Get field group intraslice double wide status. */
    single_wide = ((fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE)
                    && !(fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) ? 1 : 0;

    if (SOC_IS_TOMAHAWK3(unit)) {
        single_wide = (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE);
    }

    status->entries_total = 0;
    status->entries_free = 0;

    if (fg->stage_id == _BCM_FIELD_STAGE_EXACTMATCH) {
        int slice = 0;
        /* Retreive Slice Instance. */
        fs = stage_fc->slices[fg->instance];
        /* Calculate free entry count. */
        for (slice = 0; slice < stage_fc->tcam_slices; slice++) {
            status->entries_free += fs[slice].free_count;
            status->entries_total += fs[slice].entry_count;
        }
#if defined(BCM_FLOWTRACKER_SUPPORT)
    } else if (fg->stage_id == _BCM_FIELD_STAGE_FLOWTRACKER) {
        single_wide = (fg->flags & _FP_GROUP_DW_DEPTH_EXPANDED) ? 0 : 1;
        if (single_wide) {
            status->entries_total = fs->entry_count;
            status->entries_free = fs->free_count;
        } else {
            status->entries_total = (fs->entry_count / 2);
            status->entries_free = (fs->free_count / 2);
        }
#endif
    } else {
        /* Calculate total entries count in slices allocated for the group. */
        while (NULL != fs) {
            status->entries_total += fs->entry_count;
            fs = fs->next;
        }

        /* Calculate total free entries count in slices allocated for the group. */
        status->entries_free = 0;
        fs = &fg->slices[0];
        while (NULL != fs) {
            /* Get free entries count in the slice. */
            _bcm_field_entries_free_get(unit, fs, fg, &count);
            status->entries_free += count;
            fs = fs->next;
        }
    }

    if (_BCM_FIELD_STAGE_FLOWTRACKER != fg->stage_id) {
        rv = _bcm_field_th_flex_counter_status_get(unit, fg,
                                               &(status->counters_total),
                                               &(status->counters_free));
    }
    BCM_IF_ERROR_RETURN(rv);
    /*
     * Calculate total counters and meters count, free counters and meters
     * count.
     */
    fs = fg->slices;

    /* ExactMatch stage doesnt have meter pool seperately.
     * It uses the meters allocated to ingress stage.
     */
    if(_BCM_FIELD_STAGE_EXACTMATCH == fg->stage_id)
    {
        /* Get ingress field group stage control structure. */
        BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_ingress));

        status->meters_total = _bcm_field_meters_total_get(stage_ingress,
                                  fg->instance, fs);
        status->meters_free = _bcm_field_meter_free_get(stage_ingress,
                                  fg, fs);
    } else {

        status->meters_total = _bcm_field_meters_total_get(stage_fc, fg->instance,
                                   fs);
        status->meters_free = _bcm_field_meter_free_get(stage_fc, fg,
                                   fs);
    }
    /*
     * Calculate total entries count and total free entries count for the group
     * if it expands to other available slices.
     */
    if (stage_fc->flags & _FP_STAGE_AUTO_EXPANSION) {
        for (slice_index = 0; slice_index < stage_fc->tcam_slices;
             ++slice_index) {
            /* Ignore the secondary and tertiary slices */
            if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ||
                    (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE)) {
                if (fg->slices[1].slice_number == slice_index) {
                    continue;
                }
            }
            if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
                if (fg->slices[2].slice_number == slice_index) {
                    continue;
                }
            }
            /*Ignore the primary slice as well as this is already accounted for*/
            if (fg->slices[0].slice_number == slice_index) {
                continue;
            }

            rv = _bcm_field_th_group_add_slice_validate(unit, stage_fc, fg,
                    slice_index);
            if (BCM_SUCCESS(rv)) {
                fs = stage_fc->slices[fg->instance] + slice_index;
                if (single_wide) {
                    status->entries_total += fs->entry_count;
                    status->entries_free += fs->free_count;
                } else {
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        status->entries_total += fs->entry_count;
                        status->entries_free += fs->free_count;
                    } else {
                        status->entries_total += (fs->entry_count / 2);
                        status->entries_free += (fs->free_count / 2);
                    }
                }
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _field_th_ingress_default_group_expand_install
 * Purpose:
 *    Allocate and Install LT Entries in a group expanded slice.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Reference to Field Stage structure.
 *    fg         - (IN) Reference to Field Group structure.
 *    entry_part - (IN) Slice Entry Part.
 *    lt_fs      - (IN) Reference to LT expanded slice.
 * Returns:
 *    BCM_E_xxx.
 */
int
_field_th_ingress_default_group_expand_install(int unit,
                                               _field_stage_t *stage_fc,
                                               _field_group_t *fg,
                                               int entry_part,
                                               _field_lt_slice_t *lt_fs)
{
    int rv;
    int parts_count = -1;
    int part;
    uint8 slice_attached = 0;
    _field_lt_slice_t     *lt_part_fs = NULL;
    _field_lt_entry_t     *lt_f_entry;
    _field_lt_entry_t     *lt_f_entry_p = NULL;
    _field_control_t *fc;
    bcm_field_entry_t lt_entry;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    /* Field control get. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    if ((fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) != 0) {
       return BCM_E_INTERNAL;
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "\n\r%s(): fg:%p fg->gid:%d lt_fs:%p slice_number:%d entry_part:%d\n\r"),
        __func__, fg, fg->gid, lt_fs, lt_fs->slice_number, entry_part));


    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
         lt_entry = fc->lt_info[0][fg->lt_id]->lt_entry;
    } else {
         lt_entry = fc->lt_info[fg->instance][fg->lt_id]->lt_entry;
    }

    /* Get all part of the entry. */
    BCM_IF_ERROR_RETURN(_field_th_lt_entry_get_by_id(unit,
                      lt_entry, &lt_f_entry));

    /* Get number of entry parts. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                              &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    for (part = 0; part < parts_count; part++) {
       if (part == entry_part) {
          lt_f_entry_p = lt_f_entry + part;
          break;
       }
    }

    if (lt_f_entry_p == NULL) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
          "Error[%d]: LT Part[%d]-entry_part[%d] entry is NULL,"
          " lt_f_entry->eid:%d\n\r"),
          rv, part, entry_part, lt_f_entry->eid));
       return BCM_E_INTERNAL;
    }

    /* Get entry part flags. */
    rv = _bcm_field_th_tcam_part_to_entry_flags(unit, part, fg->flags,
                                                   &(lt_f_entry_p->flags));
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
       "LT entry part flags get failed.\n\r")));
       return rv;
    }

    /* Given primary entry TCAM index calculate entry part tcam index. */
    if (0 != part) {
       lt_f_entry_p->index = lt_f_entry->index;
    }

    /* Check if the given lt slice is already associated to the entry. */
    lt_part_fs = lt_f_entry_p->lt_fs;
    for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
        if (lt_part_fs == lt_fs) {
           slice_attached = 1;
           break;
        }
    }

    if (slice_attached == 0) {
       /* Attach the expanded slice for the part entry. */
       lt_part_fs = lt_f_entry_p->lt_fs;
       for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
          if (lt_part_fs->next == NULL) {
             lt_part_fs->next = lt_fs;
             lt_part_fs->next->next = NULL;
             lt_part_fs->next->prev = lt_part_fs;
             break;
          }
       }
    }

    /* Mark entry dirty. */
    lt_f_entry_p->flags |= _FP_ENTRY_DIRTY;

    /* Update LT Slices. */
    /* Decrement slice free entry count for primary entries. */
    lt_fs->free_count--;
    /* Assign entry to a slice. */
    lt_fs->entries[lt_f_entry_p->index] = lt_f_entry_p;

    for (part = 0; part < parts_count; part++) {
       lt_f_entry_p = lt_f_entry + part;

       if (entry_part == part) {
          break;
       }
    }

    /* Install the LT Entry in HW. */
    rv = _field_th_lt_default_tcam_entry_install(unit, lt_f_entry_p->eid, 1,
                                                 entry_part, lt_fs);
    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
       "LT TCAM entry install failed[%d].\n\r"), rv));
       return rv;
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "default_group_expand_install():"
     "lt_f_entry:%p eid:%d lt_fs:%p slice_num:%d slice_idx:%d\n\r"),
     lt_f_entry, lt_f_entry->eid, lt_fs,lt_fs->slice_number,lt_f_entry->index));
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));

    return rv;
}

/*
 * Function:
 *    _field_th_ingress_group_expand_install
 * Purpose:
 *    Allocate and Install Presel Entries in a group expanded slice.
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Reference to Field Stage structure.
 *    fg         - (IN) Reference to Field Group structure.
 *    entry_part - (IN) Slice Entry Part.
 *    lt_fs      - (IN) Reference to LT expanded slice.
 * Returns:
 *    BCM_E_xxx.
 */
STATIC int
_field_th_ingress_group_expand_install(int unit, _field_stage_t *stage_fc,
                                       _field_group_t *fg, int entry_part,
                                       _field_lt_slice_t *lt_fs)
{
    int rv, idx;
    int parts_count = -1;
    int part;
    uint8 slice_attached = 0;
    _field_lt_slice_t     *lt_part_fs;
    _field_presel_entry_t *f_presel_p;
    _field_presel_entry_t *f_presel;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == stage_fc) || (NULL == lt_fs)) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
        "%s(): fg:%p fg->gid:%d lt_fs:%p slice_number:%d entry_part:%d\n\r"),
        __func__, fg, fg->gid, lt_fs, lt_fs->slice_number, entry_part));

    f_presel = fg->presel_ent_arr[0];
    if (f_presel == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Get number of entry parts. */
    rv = _bcm_field_th_entry_tcam_parts_count(unit, fg->flags,
                                              &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    f_presel_p = f_presel;
    for (part = 0; (part < parts_count && f_presel_p != NULL); part++) {
       if (part == entry_part) {
          break;
       }
       f_presel_p = f_presel_p->next;
    }

    if (f_presel_p == NULL || part >= parts_count) {
       return BCM_E_INTERNAL;
    }

    /* Get entry part flags. */
    rv = _bcm_field_th_tcam_part_to_entry_flags(unit, part, fg->flags,
                                                &(f_presel_p->flags));
    BCM_IF_ERROR_RETURN(rv);

    /* Given primary entry TCAM index calculate entry part tcam index. */
    if (0 != part) {
       f_presel_p->hw_index = f_presel->hw_index;
    }

    /* Check if the given lt slice is already associated to the presel entry. */
    lt_part_fs = f_presel_p->lt_fs;
    for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
        if (lt_part_fs == lt_fs) {
           slice_attached = 1;
           break;
        }
    }

    if (slice_attached == 0) {
       /* Attach the expanded slice for the part entry. */
       lt_part_fs = f_presel_p->lt_fs;
       for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
          if (lt_part_fs->next == NULL) {
             lt_part_fs->next = lt_fs;
             lt_part_fs->next->next = NULL;
             lt_part_fs->next->prev = lt_part_fs;
             break;
          }
       }
    }

    if (lt_part_fs == NULL) {
       return BCM_E_INTERNAL;
    }

    /* Mark entry dirty. */
    f_presel_p->flags |= _FP_ENTRY_DIRTY;

    /* Update Presel LT Slices. */
    for (idx = 0; idx < _FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
        if (fg->presel_ent_arr[idx] != NULL) {
           f_presel = fg->presel_ent_arr[idx];
        } else {
           continue;
        }

        f_presel_p = f_presel;
        for (part = 0; (part < parts_count && f_presel_p != NULL); part++) {
           if (entry_part == part) {
              break;
           }
           f_presel_p = f_presel_p->next;
        }

        if (f_presel_p == NULL || part == parts_count) {
           return BCM_E_INTERNAL;
        }

        lt_part_fs = f_presel_p->lt_fs;
        for (; lt_part_fs != NULL; lt_part_fs = lt_part_fs->next) {
           /* Goto the last slice. */
           if (lt_part_fs->next == NULL) {
              /* Decrement slice free entry count for primary entries. */
              lt_part_fs->free_count--;
              /* Assign entry to a slice. */
              lt_part_fs->p_entries[f_presel_p->hw_index] = f_presel_p;
              break;
           }
        }

        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "group_expand_install():"
              "f_presel_p:%p presel:%d lt_fs:%p slice_num:%d slice_idx:%d\n\r"),
              f_presel_p,f_presel_p->presel_id,lt_fs,lt_fs->slice_number,
              f_presel_p->hw_index));
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
               "-----------------------------------------------------\n\r")));

        /* Install the LT Entry in HW. */
        rv = _bcm_field_th_lt_entry_install(unit, f_presel_p, lt_fs);
        BCM_IF_ERROR_RETURN(rv);
    }


    return BCM_E_NONE;
}



/*
 * Function:
 *    _field_th_ingress_group_expand_slice_install
 * Purpose:
 *    Auxiliary routine used to install field group in hardware
 *    for a given slice.
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     stage_fc      - (IN) Installed group structure.
 *     fg            - (IN/OUT)  Field group structure.
 *     slice_number  - (IN) Slice number group installed in.
 *     part_index    - (IN) Entry Part Index.
 *     group_slice_prio - (IN) Group's Priority for the particular slice.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_ingress_group_expand_slice_install(int unit, _field_stage_t *stage_fc,
                                         _field_group_t *fg, uint8 slice_number,
                                         int part_index, int group_slice_prio)
{
    int rv;
    _field_lt_slice_t *lt_fs;   /* LT Slice pointer.                */
    _field_slice_t *fs;         /* Field slice structure pointer.   */
    _field_control_t          *fc;           /* Field control structure. */

    /* Input parameters check. */
    if ((NULL == stage_fc) || (NULL == fg) || (NULL == fg->lt_slices)) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get LT entry part slice control structure. */
    lt_fs = stage_fc->lt_slices[fg->instance] + slice_number;

    /* Set slice mode in hardware based on Group mode/flags. */
    BCM_IF_ERROR_RETURN(_field_th_ingress_slice_mode_set(unit, stage_fc,
        lt_fs->slice_number, fg, 0));
    /*
     * Initialize slice free entries count value to number of
     * entries in the slice.
     */
    if (0 == fg->slices[0].lt_map) {
        lt_fs->free_count = lt_fs->entry_count;
    }

    /* Get IFP TCAM slice information. */
    fs = stage_fc->slices[fg->instance] + slice_number;

    /* Update slice LT_ID mapping in group's slice. */
    fs->lt_map |= (1 << fg->lt_id);


    fc->lt_info[fg->instance][fg->lt_id]->lt_part_map |= (1 << fs->slice_number);
    fc->lt_info[fg->instance][fg->lt_id]->lt_part_pri[fs->slice_number] = group_slice_prio;

    /* Install TCAM keygen program configuration in hardware for each slice. */
    if (fg->flags & _FP_GROUP_PRESELECTOR_SUPPORT) {
       rv = _field_th_ingress_group_expand_install(unit, stage_fc, fg,
                                                   part_index, lt_fs);
    } else {
       /* Default case. */
       rv = _field_th_ingress_default_group_expand_install(unit, stage_fc, fg,
                                                           part_index, lt_fs);
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "Error[%d]: Group slice auto-expand failed for"
             " group:%d part:%d slice:%d\n\r"),
             rv, fg->gid, part_index, lt_fs->slice_number));
        return rv;
    }

    /* Set the partition Priority. */
#if defined (BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5X(unit)) {
        rv = _bcm_field_hx5_ingress_lt_partition_prio_write(unit, stage_fc, fg,
                                                           fg->lt_id, slice_number);
    } else
#endif
    {
        rv =_bcm_field_th_ingress_lt_partition_prio_write(unit, stage_fc, fg,
                                           fg->lt_id, slice_number);
    }
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "Error: LT Partition Set failed for"
             " group:%d lt_id:%d slice:%d val:%d\n\r"),
             fg->gid, fg->lt_id, slice_number, group_slice_prio));
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_field_th_group_instance_set
 *
 * Purpose:
 *     Set the group instance based on the group's operational mode
 *     and group's pbmp.
 *
 * Parameters:
 *     unit       -  (IN)  BCM device number.
 *     fg         -  (IN)  Reference to Field Group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_group_instance_set(int unit, _field_group_t *fg)
{
    int                rv = BCM_E_NONE; /* Return Variable */
    int                pipe;      /* Device pipe iterator.    */
    _field_stage_t     *stage_fc; /* Stage control structure. */
    bcm_pbmp_t         t_pbmp;    /* Pipe Port bitmap.        */
    bcm_pbmp_t         all_pbmp;   /* Group port bitmap.      */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

   /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
       goto bcm_error;
    }

   /*
    * Based on the Stage Group Operational mode, set the Field Group's instance
    * information.
    */
    if (bcmFieldGroupOperModeGlobal == stage_fc->oper_mode) {

      /*
       * Validate user supplied IPBM value.
       * For global mode, all ports must be part of IPBM value, if supplied.
       */
       if (BCM_PBMP_NEQ(all_pbmp, fg->pbmp)) {
           /* If port configuration get fails, go to end. */
           rv = BCM_E_PARAM;
           goto bcm_error;
       }

       /* For group global operational mode, use the default instance.  */
       fg->instance = _FP_DEF_INST;
    } else {
      /*
       * For PipeLocal group oper mode, derive the instance
       * information from group's PBM value.
       */
       for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
           if (BCM_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
               continue;
           }
#endif
           BCM_PBMP_ASSIGN(t_pbmp, PBMP_PIPE(unit, pipe));
           BCM_PBMP_REMOVE(t_pbmp, PBMP_LB(unit));
          /*
           * User must use bcm_port_config_get API to retrieve Pipe PBMP value
           * and pass this value in Group's pbmp parameter when creating
           * PipeLocal groups.
           */
           if (BCM_PBMP_EQ(t_pbmp, fg->pbmp)) {
              /*
               * Pipe PBM matches user supplied PBM, update field group
               * instance.
               */
               fg->instance = pipe;
               break;
           }
       }

      /*
       * Portbit map value supplied by the user not matching any of the pipe's
       * port bitmap mask value, hence return input paramter error.
       */
       if (NUM_PIPE(unit) == pipe) {
          rv = BCM_E_PARAM;
          goto bcm_error;
       }
    }

bcm_error:
    return rv;
}

#else /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _th_field_not_empty; /* Make ISO compilers happy. */
#endif /* !BCM_TOMAHAWK_SUPPORT && !BCM_FIELD_SUPPORT */
