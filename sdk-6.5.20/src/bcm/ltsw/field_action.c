/*! \file field_action.c
                        bcm_field_action_match_config_t *match_config,
 *
 * Field Module Action Implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/cosq.h>


#include <bcm_int/control.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/qos.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/*
 * Function to add or delete action from the entry operational db.
 */
int
bcmint_field_action_entry_update(
    int unit,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_field_action_t action,
    bool add)
{
    uint16 indx = 0;
    uint16 indx2 = 0;
    bool is_action_found = false;
    SHR_FUNC_ENTER(unit);

    for (indx = 0; indx < entry_oper->num_actions; indx++) {
        if (entry_oper->action_arr[indx] == action) {
            is_action_found = true;
            break;
        }
    }

    if ((add == true) && (is_action_found == 0)) {
        if ((entry_oper->num_actions + 1) >= BCMI_FIELD_HA_ENT_ACTIONS_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        entry_oper->action_arr[indx] = action;
        entry_oper->num_actions++;
    } else if (add == false) {
        if (is_action_found == false) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        for (indx2 = indx; indx2 < entry_oper->num_actions-1; indx2++) {
            entry_oper->action_arr[indx2] = entry_oper->action_arr[indx2 + 1];
        }
        (entry_oper->num_actions)--;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Do search for given action and return success or failure.
 *
 * Algorithm:
 *  1. Search provided action in database as indicated by
 *     stage id
 *  2. If action is not present, return NOT FOUND
 *  3. If action is present in database then return
 *     SUCCESS
 *  4. If action is present in database then
 *     pointer to db if input p2p is not NULL
 */
int
bcmint_field_action_map_find(int unit,
                             bcm_field_action_t action,
                             bcmint_field_stage_info_t *stage_info,
                             const bcm_field_action_map_t **lt_map)
{
    int start = 0, end = 0;
    int midpoint = 0, result = 0;
    bool found = 0;
    const bcm_field_action_map_t *lt_map_db = NULL;
    const bcm_field_action_map_t *lt_map_db_tmp = NULL;

    SHR_FUNC_ENTER(unit);

    /* Retreive entry count & db from stage first */
    lt_map_db_tmp = lt_map_db = stage_info->action_lt_map_db;
    end = stage_info->action_db_count - 1;

    /* do binary search over db */
    while (end >= start) {
        midpoint = (end + start) / 2;
        lt_map_db_tmp = lt_map_db + midpoint;

        /* compare current db node with target action */
        if (lt_map_db_tmp->action < action) {
            result = -1;
        } else if (lt_map_db_tmp->action > action) {
            result = 1;
        } else {
            result = 0;
        }

        /* adjust start/end based on compare results */
        if (result == 1) {
            end = midpoint - 1;
        } else if (result == -1) {
            start = midpoint + 1;
        } else {
            found = 1;
            break;
        }
    }

    /* return results */
    if (found) {
        if (lt_map != NULL) {
            *lt_map = lt_map_db + midpoint;
        }
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "Error: Action set is not "
                   "supported by the device\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


void
bcmint_field_action_buffer_set(int unit,
                               const bcm_field_action_map_info_t *map,
                               bcmlt_field_def_t *fld_defs,
                               bcm_field_action_t action,
                               uint32_t *src_ofst_current,
                               uint32_t *act_value_inp_buf,
                               uint32_t *act_value)
{
    uint16_t idx = 0;
    uint32_t width = 0;
    uint64_t *ibuf_ptr = NULL;
    uint64_t *obuf_ptr = NULL;
    uint64_t mask = 0, clr_mask = 0;

    ibuf_ptr = (uint64_t *)act_value_inp_buf;
    obuf_ptr = (uint64_t *)act_value;
    if (map->width[0] == 0) {
        /* action takes full field width */
        width = fld_defs->width;
        mask = (((uint64_t)1 << width) - 1);
        clr_mask = ~(mask);
        *obuf_ptr &= clr_mask;
        *obuf_ptr |= ((*ibuf_ptr >> *src_ofst_current) & mask);
        *src_ofst_current += width;
    } else {
        /* Set action in o/p as per offset & widths specified in map */
        for (idx = 0; idx < map->num_offsets; idx++) {
            width = map->width[idx];
            mask = (((uint64_t)1 << width) - 1);
            clr_mask = ~(mask << map->offset[idx]);
            *obuf_ptr &= clr_mask;
            *obuf_ptr |= (((*ibuf_ptr >> *src_ofst_current) & mask) << map->offset[idx]);
            *src_ofst_current +=  width;
        }
    }
}

void
bcmint_field_int_action_buffer_set(int unit,
                                   const bcm_field_action_internal_map_info_t *map,
                                   bcmlt_field_def_t *fld_defs,
                                   bcm_field_action_t action,
                                   uint32_t *src_ofst_current,
                                   uint32_t *act_value_inp_buf,
                                   uint32_t *act_value)
{
    uint16_t idx = 0;
    uint32_t width = 0;
    uint64_t *ibuf_ptr = NULL;
    uint64_t *obuf_ptr = NULL;
    uint64_t mask = 0, clr_mask = 0;

    ibuf_ptr = (uint64_t *)act_value_inp_buf;
    obuf_ptr = (uint64_t *)act_value;
    if (map->width[0] == 0) {
        /* action takes full field width */
        width = fld_defs->width;
        mask = (((uint64_t)1 << width) - 1);
        clr_mask = ~(mask);
        *obuf_ptr &= clr_mask;
        *obuf_ptr |= ((*ibuf_ptr >> *src_ofst_current) & mask);
        *src_ofst_current += width;
    } else {
        /* Set action in o/p as per offset & widths specified in map */
        for (idx = 0; idx < map->num_offsets; idx++) {
            width = map->width[idx];
            mask = (((uint64_t)1 << width) - 1);
            clr_mask = ~(mask << map->offset[idx]);
            *obuf_ptr &= clr_mask;
            *obuf_ptr |= (((*ibuf_ptr >> *src_ofst_current) & mask) << map->offset[idx]);
            *src_ofst_current +=  width;
        }
    }
}

void
bcmint_field_action_buffer_get(int unit,
                                     const bcm_field_action_map_info_t *map,
                                     bcmlt_field_def_t *fld_defs,
                                     bcm_field_action_t action,
                                     uint32_t *src_ofst_current,
                                     uint32_t *act_value_inp_buf,
                                     uint32_t *act_value)
{
    uint16_t idx = 0;
    uint32_t width = 0;
    uint64_t *ibuf_ptr = NULL;
    uint64_t *obuf_ptr = NULL;
    uint64_t mask = 0;

    ibuf_ptr = (uint64_t *)act_value;
    obuf_ptr = (uint64_t *)act_value_inp_buf;
    if (map->width[0] == 0) {
        /* Action takes full field width */
        width = fld_defs->width;
        mask = (((uint64_t)1 << width) - 1);
        *obuf_ptr |= ((*ibuf_ptr & mask) << *src_ofst_current);
        *src_ofst_current += width;
    } else {
        /* Get action from o/p as per offset & widths specified in map */
        for (idx = 0; idx < map->num_offsets; idx++) {
            width = map->width[idx];
            mask = (((uint64_t)1 << width) - 1);
            *obuf_ptr |= (((*ibuf_ptr >> map->offset[idx]) & mask) << *src_ofst_current);
            *src_ofst_current += width;
        }
    }
}

int
bcmint_field_action_internal_map_set(int unit,
                                uint8_t set_int_val,
                                bcm_field_action_t action,
                                bcmlt_entry_handle_t action_template_hdl,
                                bcmlt_entry_handle_t action_template_lkup_hdl,
                                bcmint_field_stage_info_t *stage_info,
                                const bcm_field_action_map_info_t *map,
                                uint32_t start_idx,
                                uint32_t num_elem,
                                bool map_val_valid,
                                uint32_t *map_val)
{
    uint32_t int_value = 0, width = 0;
    uint32_t dst_offset = 0, dst_bit_offset = 0;
    uint64_t mask = 0, clr_mask = 0;
    uint64_t *act_value_inp_buf = NULL;
    uint64_t *obuf_ptr = NULL;
    uint16_t idx = 0, iter = 0, src_ofst_int = 0;
    char *action_int_field = NULL, *action_template_name = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL, *fld_defs_iter = NULL;
    const bcm_field_action_internal_map_info_t *lt_map_int_ref = NULL;
    uint32_t elem = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Set internal map for a action map if present. */
    if (map->num_internal_maps > 0) {
        /* Retreive action template name from stage. */
        action_template_name = stage_info->tbls_info->policy_sid;

        /* Allocate memory for all action ltmap fields. */
        BCMINT_FIELD_MEM_ALLOC
            (fld_defs_ref,
                sizeof(bcmlt_field_def_t) * map->num_internal_maps,
                "internal action map field definitions");
        fld_defs_iter = fld_defs_ref;

        /* Retreive internal map first. */
        lt_map_int_ref = map->internal_map;
        if (lt_map_int_ref != NULL) {
            /* Retreive field definition for provided lt field in map */
            for (iter = 0; iter < map->num_internal_maps; iter++) {

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit, action_template_name,
                      (lt_map_int_ref[iter]).lt_field_name, &(fld_defs_iter[iter])));

                /* Validate field definitions for valid values */
                if ((0 == fld_defs_iter[iter].elements) ||
                    (1 == fld_defs_iter[iter].symbol)) {
                    /* Not handling fields of array type or symbols for
                     * internal map, can be added later in case of need. */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
            }

            src_ofst_int = 0;
            fld_defs_iter = fld_defs_ref;
            if ((map_val_valid == true) && (map_val == NULL)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            for (iter = 0; iter < map->num_internal_maps; iter++) {
                BCMINT_FIELD_MEM_ALLOC
                    (act_value_inp_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
                     "action value");
                BCMINT_FIELD_MEM_ALLOC(action_int_field,
                        (strlen(lt_map_int_ref[iter].lt_field_name) + 1),
                        "action internal field");
                sal_strcpy(action_int_field, lt_map_int_ref[iter].lt_field_name);

                /* Retreive previously set qualifier & mask value */
                if (fld_defs_iter[iter].depth > 1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(action_template_lkup_hdl,
                                                     action_int_field,
                                                     start_idx,
                                                     act_value_inp_buf,
                                                     num_elem, &elem));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(action_template_lkup_hdl,
                                               action_int_field,
                                               act_value_inp_buf));
                }

                if (set_int_val) {
                    if (map_val_valid == true) {
                        int_value = map_val[iter];
                    } else {
                        int_value = lt_map_int_ref[iter].value;
                    }
                }

                if ((0 == lt_map_int_ref[iter].num_offsets) &&
                    (0 == lt_map_int_ref[iter].width[0])) {
                    /* Action takes full field width */
                    width = fld_defs_iter[iter].width;
                    mask = ((1 << width) - 1);
                    clr_mask = ~(mask);
                    *act_value_inp_buf &= clr_mask;
                    *act_value_inp_buf |= (int_value & mask);
                } else {
                    /* Set action in o/p as per offset & widths specified in map */
                    for (idx = 0; idx < lt_map_int_ref[iter].num_offsets; idx++) {
                        dst_offset = (lt_map_int_ref[iter].offset[idx] / 64);
                        dst_bit_offset = (lt_map_int_ref[iter].offset[idx] % 64);
                        obuf_ptr = (act_value_inp_buf + dst_offset);
                        width = lt_map_int_ref[iter].width[idx];
                        mask = ((1 << width) - 1);
                        clr_mask = ~(mask << dst_bit_offset);
                        *obuf_ptr &= clr_mask;
                        *obuf_ptr |= (((int_value >> src_ofst_int) & mask) << dst_bit_offset);
                        src_ofst_int += width;
                    }
                }

                if (fld_defs_iter[iter].depth > 1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_add(action_template_hdl,
                                                     action_int_field,
                                                     start_idx,
                                                     act_value_inp_buf,
                                                     num_elem));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(action_template_hdl,
                                               action_int_field,
                                               *act_value_inp_buf));
                }
                BCMINT_FIELD_MEM_FREE(act_value_inp_buf);
                BCMINT_FIELD_MEM_FREE(action_int_field);
            }
        }
    }

exit:
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(act_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(action_int_field);
    SHR_FUNC_EXIT();
}

static int
bcmint_field_action_mirror_remove(int unit,
                                       bcm_field_entry_t entry,
                                       bcmint_field_stage_info_t *stage_info,
                                       bcm_field_action_t action,
                                       uint32 param)
{
    bcm_gport_t mirror_gport;
    SHR_FUNC_ENTER(unit);

    /* Mirror GPORT validation */
    if (BCM_GPORT_IS_MIRROR(param) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    mirror_gport = param;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_action_mirror_remove(unit,
                                              stage_info,
                                              entry,
                                              action,
                                              mirror_gport));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_field_action_mirror_remove_all(int unit,
                                      bcm_field_entry_t entry,
                                      bcmint_field_stage_info_t *stage_info,
                                      bcm_field_action_t action)
{
    int idx;
    int num_gports = 0;
    uint32 mirror_gport[4] = {0};
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_mirror_gport_get(
                                unit, entry,
                                stage_info,
                                action,
                                mirror_gport,
                                &num_gports));

    for (idx = 0; idx < num_gports; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_mirror_remove(
                                unit, entry,
                                stage_info,
                                action,
                                mirror_gport[idx]));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_field_action_param_set(int unit,
                              bcmint_field_stage_info_t *stage_info,
                              bcmi_field_ha_entry_oper_t *entry_oper,
                              bcm_field_action_t action,
                              bcm_field_action_params_t *params,
                              int *flags,
                              uint64_t lt_map_action_width,
                              bcm_field_action_match_config_t *match_config,
                              uint64_t *action_value)
{
    int class_index = 0;
    bcmi_ltsw_qos_map_type_t type;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(params, SHR_E_PARAM);

    /* Some action need chip specific calls for set/get */
    rv = mbcm_ltsw_field_action_value_set(unit, entry_oper->entry_id,
                                          stage_info->stage_id,
                                          action, params,
                                          match_config,
                                          action_value);
    if (rv == SHR_E_NONE) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv,BCM_E_UNAVAIL);
    if ((action == bcmFieldActionCosMapNew) ||
        (action == bcmFieldActionGpCosMapNew) ||
        (action == bcmFieldActionYpCosMapNew) ||
        (action == bcmFieldActionRpCosMapNew)){
        /* retrieve field profile index from classifier ID */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_classifier_field_get(unit, params->param0, flags,
                                                 &class_index));
        *action_value = class_index;
        if ((*flags & BCM_COSQ_CLASSIFIER_NO_INT_PRI) == 0) {
            
            *action_value = (*action_value << 4);
            entry_oper->action_flags &= ~BCMINT_FIELD_ENTRY_ACTION_COSQNEW_WITH_INT;
        } else {
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_COSQNEW_WITH_INT;
        }
    } else if ((bcmFieldActionRedirectPbmp == action)
            || (bcmFieldActionEgressPortsAdd == action)
            || (bcmFieldActionEgressMask == action)
            || (bcmFieldActionRedirectBcastPbmp == action)
            || (bcmFieldActionMatchPbmpDrop == action)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_action_profile_add(unit,
                                                stage_info->stage_id,
                                                action, params,
                                                match_config,
                                                action_value));
    } else if (bcmFieldActionIngressGportSet == action) {
        int svp = 0;

        if (BCM_GPORT_IS_MPLS_PORT(params->param0)) {
            svp = BCM_GPORT_MPLS_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS;
        } else if (BCM_GPORT_IS_MIM_PORT(params->param0)) {
            svp = BCM_GPORT_MIM_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM;
        } else if (BCM_GPORT_IS_L2GRE_PORT(params->param0)) {
            svp = BCM_GPORT_L2GRE_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_L2GRE;
        } else if (BCM_GPORT_IS_WLAN_PORT(params->param0)) {
            svp = BCM_GPORT_WLAN_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN;
        } else if (BCM_GPORT_IS_TRILL_PORT(params->param0)) {
            svp = BCM_GPORT_TRILL_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_TRILL;
        } else if (BCM_GPORT_IS_NIV_PORT(params->param0)) {
            svp = BCM_GPORT_NIV_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV;
        } else if (BCM_GPORT_IS_VXLAN_PORT(params->param0)) {
            svp = BCM_GPORT_VXLAN_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN;
        } else if (BCM_GPORT_IS_FLOW_PORT(params->param0)) {
            svp = BCM_GPORT_FLOW_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW;
        } else if (BCM_GPORT_IS_VLAN_PORT(params->param0)) {
            svp = BCM_GPORT_VLAN_PORT_ID_GET((bcm_gport_t)params->param0);
            entry_oper->action_flags |= BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
        }
        *action_value = svp;
    } else if (bcmFieldActionDscpMapNew == action) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_qos_map_id_resolve(unit, params->param0,
                             &type, &class_index));
        *action_value = class_index;
        entry_oper->qos_type = type;
    } else {
        if (params->param0 != 0) {
            
            if ((action == bcmFieldActionDrop) ||
                (action == bcmFieldActionCopyToCpu) ||
                (action == bcmFieldActionGpDrop) ||
                (action == bcmFieldActionYpDrop) ||
                (action == bcmFieldActionRpDrop) ||
                (action == bcmFieldActionGpCopyToCpu) ||
                (action == bcmFieldActionYpCopyToCpu) ||
                (action == bcmFieldActionRpCopyToCpu) ||
                (action == bcmFieldActionGpCopyToCpuCancel) ||
                (action == bcmFieldActionYpCopyToCpuCancel) ||
                (action == bcmFieldActionRpCopyToCpuCancel) ||
                (action == bcmFieldActionGpDropCancel) ||
                (action == bcmFieldActionYpDropCancel) ||
                (action == bcmFieldActionRpDropCancel)) {
                *action_value = (1 << lt_map_action_width) - 1;
            } else if ((action == bcmFieldActionOuterVlanCfiNew) ||
                       (action == bcmFieldActionGpOuterVlanCfiNew) ||
                       (action == bcmFieldActionYpOuterVlanCfiNew) ||
                       (action == bcmFieldActionRpOuterVlanCfiNew)) {
                if (params->param0 == 1) {
                    *action_value = (1 << lt_map_action_width) - 1;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
                }
            } else {
                /* If param0 is provided we set
                 * incoming value in action LT field. */
                *action_value = params->param0;
            }
        } else {
            if ((action == bcmFieldActionNewClassId) ||
                (action == bcmFieldActionOuterVlanCfiNew) ||
                (action == bcmFieldActionGpOuterVlanCfiNew) ||
                (action == bcmFieldActionYpOuterVlanCfiNew) ||
                (action == bcmFieldActionRpOuterVlanCfiNew) ||
                (action == bcmFieldActionDgmTrunkPriPathThreshold) ||
                (action == bcmFieldActionDgmTrunkAltPathCost) ||
                (action == bcmFieldActionDgmTrunkAltPathBias) ||
                (action == bcmFieldActionDgmEcmpPriPathThreshold) ||
                (action == bcmFieldActionDgmEcmpAltPathCost) ||
                (action == bcmFieldActionMirrorOnDropProfileSet) ||
                (action == bcmFieldActionDgmEcmpAltPathBias) ||
                (action == bcmFieldActionRecircleHdrCode) ||
                (action == bcmFieldActionEgrTimestampProfileIndex)) {
                *action_value = params->param0;
            } else {
                /* If param0 is 0, then we set LT field
                 * value to all 1 for action width. */
                *action_value = (1 << lt_map_action_width) - 1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_field_action_param_get(int unit,
                              bcmint_field_stage_info_t *stage_info,
                              bcmi_field_ha_entry_oper_t *entry_oper,
                              bcm_field_action_t action,
                              bcm_field_action_params_t *params,
                              bcm_field_action_match_config_t *match_config,
                              uint64_t *action_value)
{
    int flags = 0;
    int rv;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(params, SHR_E_PARAM);

    /* Get the action params based on the input action value. */
    rv= mbcm_ltsw_field_action_value_get(unit, entry_oper->entry_id,
                                          stage_info->stage_id,
                                          action, params,
                                          match_config,
                                          action_value);
    if (rv == SHR_E_NONE) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv,BCM_E_UNAVAIL);
    if ((action == bcmFieldActionCosMapNew) ||
        (action == bcmFieldActionGpCosMapNew) ||
        (action == bcmFieldActionYpCosMapNew) ||
        (action == bcmFieldActionRpCosMapNew)){
        if (entry_oper->action_flags &
              BCMINT_FIELD_ENTRY_ACTION_COSQNEW_WITH_INT) {
            flags = BCM_COSQ_CLASSIFIER_NO_INT_PRI;
        } else {
            
            *action_value = (*action_value >> 4);
            flags = 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_classifier_field_set(unit, *action_value, flags,
                                          (int *) &(params->param0)));
    } else if ((bcmFieldActionRedirectPbmp == action)
            || (bcmFieldActionEgressPortsAdd == action)
            || (bcmFieldActionEgressMask == action)
            || (bcmFieldActionRedirectBcastPbmp == action)
            || (bcmFieldActionMatchPbmpDrop == action)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_action_profile_get(unit,
                                                stage_info->stage_id,
                                                action,
                                                action_value,
                                                params,
                                                match_config));
    } else if (bcmFieldActionIngressGportSet == action) {
        int svp = *action_value;
        if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_MPLS) {
            BCM_GPORT_MPLS_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_MIM) {
            BCM_GPORT_MIM_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_L2GRE) {
            BCM_GPORT_L2GRE_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_WLAN) {
            BCM_GPORT_WLAN_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_TRILL) {
            BCM_GPORT_TRILL_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_NIV) {
            BCM_GPORT_NIV_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_VXLAN) {
            BCM_GPORT_VXLAN_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_FLOW) {
            BCM_GPORT_FLOW_PORT_ID_SET(params->param0, svp);
        } else if (entry_oper->action_flags & BCMINT_FIELD_ENTRY_ACTION_GPORT_VLAN) {
            BCM_GPORT_VLAN_PORT_ID_SET(params->param0, svp);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    } else if (bcmFieldActionDscpMapNew == action) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmi_ltsw_qos_map_id_construct(unit, *action_value,
                                           entry_oper->qos_type,
                                           (int *) &(params->param0)));
    } else if (
               ((action == bcmFieldActionDoNotChangeTtl)
               || (action == bcmFieldActionIntEncapEnable)
               || (action == bcmFieldActionIntEncapDisable)
               || (action == bcmFieldActionIntResidenceTimeEnable)
               || (action == bcmFieldActionEgressMirrorDisable)
               || (action == bcmFieldActionElephantLookupEnable)
               || (action == bcmFieldActionTrunkRandomRoundRobinHashCancel)
               || (action == bcmFieldActionEcmpRandomRoundRobinHashCancel)
               || (action == bcmFieldActionDynamicTrunkCancel)
               || (action == bcmFieldActionCopyToCpu)
               || (action == bcmFieldActionCopyToCpuCancel)
               || (action == bcmFieldActionDoNotCheckUrpf)
               || (action == bcmFieldActionDoNotCutThrough)
               || (action == bcmFieldActionEgressTimeStampInsert)
               || (action == bcmFieldActionIngressTimeStampInsert)
               || (action == bcmFieldActionEgressTimeStampInsertCancel)
               || (action == bcmFieldActionDrop)
               || (action == bcmFieldActionDropCancel)
               || (action == bcmFieldActionMirrorOnDropEnable)
               || (action == bcmFieldActionDoNotModify)
               || (action == bcmFieldActionDlbEcmpAlternatePathEnable)
               || (action == bcmFieldActionDlbTrunkAlternatePathEnable)
               || (action == bcmFieldActionMyStationHit)
               || (action == bcmFieldActionGpDrop)
               || (action == bcmFieldActionYpDrop)
               || (action == bcmFieldActionRpDrop)
               || (action == bcmFieldActionGpCopyToCpu)
               || (action == bcmFieldActionYpCopyToCpu)
               || (action == bcmFieldActionRpCopyToCpu)
               || (action == bcmFieldActionGpCopyToCpuCancel)
               || (action == bcmFieldActionYpCopyToCpuCancel)
               || (action == bcmFieldActionRpCopyToCpuCancel)
               || (action == bcmFieldActionGpDropCancel)
               || (action == bcmFieldActionYpDropCancel)
               || (action == bcmFieldActionRpDropCancel)
               || (action == bcmFieldActionInsertHiGig3Ext0)
               || (action == bcmFieldActionEgressMembershipCheck))) {
        if ((*action_value >= 1)) {
            params->param0 = 0;
        } else {
            SHR_ERR_EXIT(BCM_E_NOT_FOUND);
        }
    } else {
        if ((*action_value >= 1) || (BCMINT_FIELD_IS_COLORED_ACTION(action))) {
            params->param0 = *action_value;
        } else {
            if ((action == bcmFieldActionOuterVlanCfiNew) ||
                (action == bcmFieldActionMirrorOnDropProfileSet) ||
                (action == bcmFieldActionRecircleHdrCode) ||
                (action == bcmFieldActionEgrTimestampProfileIndex)) {
                params->param0 = 0;
            } else {
                SHR_ERR_EXIT(BCM_E_NOT_FOUND);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_color_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action,
    uint64_t *param)
{
    int dunit = 0;
    int iter = 0;
    int pool = -1, color_idx = -1;
    uint64_t val = 0;
    uint32_t curr_offset = 0;
    bcmlt_field_def_t fdef;
    const bcm_field_action_map_t *lt_map = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmlt_entry_handle_t color_action_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    color_tbl_info = stage_info->color_action_tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);

    if (entry_oper->color_action_id == -1 ||
        group_oper->policer_pool_id == -1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pool = group_oper->policer_pool_id;
    color_idx = entry_oper->color_action_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              color_tbl_info->tbl_sid[pool],
                              &color_action_template));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(color_action_template,
                               color_tbl_info->tbl_key_fid[pool],
                               color_idx));
    if (pipe != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(color_action_template,
                                   color_tbl_info->pipe_fid,
                                   pipe));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, color_action_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map));

    curr_offset = 0;
    for (iter = 0; iter <  lt_map->num_maps; iter++) {
        if (((lt_map->map[iter]).policer == TRUE) &&
            ((lt_map->map[iter]).policer_pdd == FALSE) &&
            ((lt_map->map[iter]).sbr_enabled == FALSE)) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(
                        unit, color_tbl_info->tbl_sid[pool],
                        (lt_map->map[iter]).lt_field_name,
                        &fdef));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(color_action_template,
                                       (lt_map->map[iter]).lt_field_name,
                                       &val));
            bcmint_field_action_buffer_get(unit,
                                           &(lt_map->map[iter]),
                                           &fdef,
                                           action,
                                           &curr_offset,
                                           (uint32_t *)param,
                                           (uint32_t *)&val);
        }
    }

exit:
    if (BCMLT_INVALID_HDL != color_action_template) {
        (void) bcmlt_entry_free(color_action_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_color_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action,
    bcm_field_action_params_t *params)
{
    uint8_t a;
    int dunit = 0;
    int iter = 0, int_iter = 0;
    int pool = -1;
    bool insert = false;
    bool action_exists = false;
    int color_action_idx = -1;
    int del_color_action_idx = -1;
    int flags = 0;
    uint32_t curr_offset = 0, int_offset = 0;
    uint32_t width = 0;
    uint64_t act_val = 0, act_out = 0;
    uint8_t ha_blk_id = 0;
    void *ha_ptr = NULL;
    bool delete = false;
    bcmlt_field_def_t fdef;
    bcmi_field_ha_profile_info_t *profile_ptr = NULL;
    const bcm_field_action_map_t *lt_map = NULL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmlt_entry_handle_t color_action_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    color_tbl_info = stage_info->color_action_tbls_info;
    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    /*
     * Color PDD and policer pool should be configured by group
     */
    if ((group_oper->color_pdd_id == -1) ||
        (group_oper->policer_pool_id == -1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* check if action is preseent in entry_oper action_arr*/
    for (a = 0; a < entry_oper->num_actions; a++) {
        if (entry_oper->action_arr[a] == action) {
            action_exists = true;
            break;
        }
    }

    if (action_exists == true) {
        uint64_t act_val = 0;
        bcm_field_action_params_t params_get;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_color_get(unit,
                                           stage_info,
                                           group_oper,
                                           entry_oper,
                                           pipe,
                                           action,
                                           &act_val));

        sal_memset(&params_get, 0x0, sizeof(params_get));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_param_get(unit, stage_info,
                                           entry_oper, action,
                                           &params_get, NULL, &act_val));

        if (params->param0 == params_get.param0) {
            SHR_EXIT();
        }
    }

    FP_COLOR_TBL_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    ha_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    profile_ptr = (bcmi_field_ha_profile_info_t *)
                  (ha_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    SHR_NULL_CHECK(profile_ptr, SHR_E_INTERNAL);

    pool = group_oper->policer_pool_id;

    /* Handle for METER_ACTION_TEMPLATE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              color_tbl_info->tbl_sid[pool],
                              &color_action_template));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(color_action_template,
                               color_tbl_info->pdd_tbl_key_fid,
                               group_oper->color_pdd_id));

    if (pipe != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(color_action_template,
                                   color_tbl_info->pipe_fid,
                                   pipe));
    }

    if (entry_oper->color_action_id != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(color_action_template,
                                   color_tbl_info->tbl_key_fid[pool],
                                   entry_oper->color_action_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit,
                                  color_action_template,
                                  BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_remove(color_action_template,
                                      "OPERATIONAL_STATE"));
        del_color_action_idx = entry_oper->color_action_id;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map));

    curr_offset = 0;
    for (iter = 0; iter <  lt_map->num_maps; iter++) {
            /*
             * These fields have to be programmed in the
             * METER_ACTION_TEMPLATE table.
             */
        if (((lt_map->map[iter]).policer == TRUE) &&
            ((lt_map->map[iter]).policer_pdd == FALSE) &&
            ((lt_map->map[iter]).sbr_enabled == FALSE)) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(
                        unit, color_tbl_info->tbl_sid[pool],
                        (lt_map->map[iter]).lt_field_name,
                        &fdef));

            if ((lt_map->map[iter]).width[0] != 0) {
                width = (lt_map->map[iter]).width[0];
            } else {
                width = fdef.width;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_param_set(unit, stage_info,
                                               entry_oper, action,
                                               params, &flags,
                                               width,
                                               NULL,
                                               &act_val));

            act_out = 0;
            if (entry_oper->color_action_id != -1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(color_action_template,
                                           (lt_map->map[iter]).lt_field_name,
                                           &act_out));
            }
            bcmint_field_action_buffer_set(unit,
                                           &(lt_map->map[iter]),
                                           &fdef,
                                           action,
                                           &curr_offset,
                                           (uint32_t *)&act_val,
                                           (uint32_t *)&act_out);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(color_action_template,
                                       (lt_map->map[iter]).lt_field_name,
                                       act_out));
            int_offset = 0;
            for (int_iter = 0; int_iter < lt_map->map[iter].num_internal_maps; int_iter++) {
                act_val = lt_map->map[iter].internal_map[int_iter].value;

                act_out = 0;
                if (entry_oper->color_action_id != -1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(
                                color_action_template,
                                lt_map->map[iter].internal_map[int_iter].lt_field_name,
                                &act_out));
                }

                bcmint_field_int_action_buffer_set(
                    unit,
                    &(lt_map->map[iter].internal_map[int_iter]),
                    &fdef,
                    action,
                    &int_offset,
                    (uint32_t *)&act_val,
                    (uint32_t *)&act_out);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(
                            color_action_template,
                            lt_map->map[iter].internal_map[int_iter].lt_field_name,
                            act_out));
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_color_profile_id_alloc(unit,
                                             color_tbl_info->tbl_sid[pool],
                                             color_tbl_info->tbl_key_fid[pool],
                                             color_tbl_info->pipe_fid,
                                             color_action_template,
                                             pipe,
                                             pool,
                                             color_tbl_info->num_pools,
                                             profile_ptr,
                                             entry_oper->color_action_id,
                                             &color_action_idx,
                                             &insert));
    entry_oper->color_action_id = color_action_idx;

    if (insert == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(color_action_template,
                                   color_tbl_info->tbl_key_fid[pool],
                                   entry_oper->color_action_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit,
                                      color_action_template,
                                      BCMLT_PRIORITY_NORMAL));
    }

    if ((del_color_action_idx != -1) &&
        (del_color_action_idx != entry_oper->color_action_id)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_profile_id_free(
                   unit,
                   color_tbl_info->tbl_sid[pool],
                   pipe,
                   profile_ptr,
                   del_color_action_idx,
                   pool,
                   color_tbl_info->num_pools,
                   &delete));
        if (delete == true) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(color_action_template,
                                       color_tbl_info->tbl_key_fid[pool],
                                       del_color_action_idx));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit,
                                      color_action_template,
                                      BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL));
        }
    }

exit:
    if (SHR_FUNC_ERR() && entry_oper) {
        if (entry_oper->color_action_id != -1 &&
            group_oper->policer_pool_id != -1) {
            (void) bcmint_field_color_profile_id_free(
                unit,
                color_tbl_info->tbl_sid[pool],
                pipe,
                profile_ptr,
                entry_oper->color_action_id,
                group_oper->policer_pool_id,
                color_tbl_info->num_pools,
                &delete);
        }
    }
    if (BCMLT_INVALID_HDL != color_action_template) {
        (void) bcmlt_entry_free(color_action_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_color_pdd_sbr_set(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action)
{
    int iter = 0;
    int dunit = 0;
    int pool = -1, color_idx = -1;
    int color_sbr_idx = -1, a = 0;
    uint16_t colored_actions = 0;
    uint32_t action_strength = 0;
    uint32_t action_val, sbr_val = 0;
    uint64_t pdd_sbr_index = 0;
    const bcm_field_action_map_t *lt_map = NULL;
    bool sbr_update = false;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmlt_entry_handle_t action_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t sbr_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    color_tbl_info = stage_info->color_action_tbls_info;
    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    /*
     * Color PDD and policer pool should be configured by group
     */
    if ((group_oper->color_pdd_id == -1) ||
        (group_oper->policer_pool_id == -1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (a = 0; a < entry_oper->num_actions; a++) {
        if (BCMINT_FIELD_IS_COLORED_ACTION(entry_oper->action_arr[a])) {
            colored_actions++;
        }
    }
    /*
     * Color SBR should be configured by group or entry
     * based on group/entry level sbr handling.
     */
    if (group_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE) {
        if (entry_oper->color_strength_idx == -1) {
            if (!(entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) ||
                (colored_actions == 0)) {
                SHR_EXIT();
            }

            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        color_sbr_idx = entry_oper->color_strength_idx;
    } else {
        if (group_oper->color_sbr_id == -1) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        color_sbr_idx = group_oper->color_sbr_id;
    }

    pool = group_oper->policer_pool_id;
    color_idx = entry_oper->color_action_id;
    pdd_sbr_index = ((color_sbr_idx << 5) | group_oper->color_pdd_id);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->policy_sid,
                              &action_template));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->sbr_sid,
                              &sbr_template));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template,
                               tbls_info->policy_key_fid,
                               entry_oper->entry_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(sbr_template,
                               tbls_info->sbr_key_fid,
                               group_oper->strength_prof_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map));

    for (iter = 0; iter <  lt_map->num_maps; iter++) {
        if ((lt_map->map[iter]).policer == FALSE) {
            /* These fields are part of FP Policy, PDD, SBR tables. */
            if ((lt_map->map[iter]).color_index != -1) {
                /* COLOR_TABLE_0/1_INDEX fields. */
                if ((lt_map->map[iter]).color_index == pool) {
                    action_val = color_idx;
                    sbr_val = 1;
                } else {
                    continue;
                }
            } else if ((lt_map->map[iter]).pdd_sbr_index != -1) {
                /* PDD_SBR_0/1_INDEX fields. */
                if ((lt_map->map[iter]).pdd_sbr_index == pool) {
                    action_val = pdd_sbr_index;
                    sbr_val = 1;
                } else {
                    continue;
                }
            } else {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(action_template,
                                       (lt_map->map[iter]).lt_field_name,
                                       action_val));

            /*
             * Strength should be assigned for this action
             * during group_create.
             */
            if ((lt_map->map[iter]).sbr_enabled == TRUE) {
                if (sbr_val == 1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_group_action_strength_get(unit,
                                    stage_info, (pipe == -1) ? 0 : pipe,
                                    group_oper->priority,
                                    action, &action_strength));
                    sbr_val = action_strength;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(sbr_template,
                                           (lt_map->map[iter]).lt_field_name,
                                           sbr_val));
                sbr_update = true;
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit,
                                  action_template,
                                  BCMLT_PRIORITY_NORMAL));
    if (sbr_update == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit,
                                      sbr_template,
                                      BCMLT_PRIORITY_NORMAL));
    }
exit:
    if (BCMLT_INVALID_HDL != action_template) {
        (void) bcmlt_entry_free(action_template);
    }
    if (BCMLT_INVALID_HDL != sbr_template) {
        (void) bcmlt_entry_free(sbr_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_color_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action)
{
    int a;
    int dunit = 0;
    int iter = 0, int_iter = 0;
    uint8_t pipe_val = 0;
    uint64_t act_val = 0, act_out = 0;
    uint8_t ha_blk_id;
    int pool = -1, curr_color_action_idx = -1;
    int new_action_idx = -1;
    bool delete = false;
    uint32_t colored_actions = 0;
    uint32_t max_entries = 0;
    uint32_t curr_offset = 0, int_offset = 0;
    bool insert = false;
    void *ha_ptr = NULL;
    bcmlt_field_def_t fdef;
    bcmi_field_ha_profile_info_t *profile_ptr = NULL;
    const bcm_field_action_map_t *lt_map = NULL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmlt_entry_handle_t color_action_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    color_tbl_info = stage_info->color_action_tbls_info;
    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    if((entry_oper->color_action_id == -1) ||
       (group_oper->policer_pool_id == -1)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    pool = group_oper->policer_pool_id;
    curr_color_action_idx = entry_oper->color_action_id;

    FP_COLOR_TBL_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    ha_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    profile_ptr = (bcmi_field_ha_profile_info_t *)
        (ha_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    SHR_NULL_CHECK(profile_ptr, SHR_E_INTERNAL);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              color_tbl_info->tbl_sid[pool],
                              &color_action_template));

    if (pipe != -1) {
        pipe_val = pipe;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(color_action_template,
                                   color_tbl_info->pipe_fid,
                                   pipe_val));
    }

    for (a = 0; a < entry_oper->num_actions; a++) {
        if (BCMINT_FIELD_IS_COLORED_ACTION(entry_oper->action_arr[a])) {
            colored_actions++;
        }
    }

    if (colored_actions == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_profile_id_free(
                   unit,
                   color_tbl_info->tbl_sid[pool],
                   pipe,
                   profile_ptr,
                   curr_color_action_idx,
                   pool,
                   color_tbl_info->num_pools,
                   &delete));
        if (delete == true) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(color_action_template,
                                       color_tbl_info->tbl_key_fid[pool],
                                       curr_color_action_idx));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit,
                                      color_action_template,
                                      BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL));
        }
        entry_oper->color_action_id = -1;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(color_action_template,
                               color_tbl_info->tbl_key_fid[pool],
                               curr_color_action_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              color_action_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_remove(color_action_template,
                                  "OPERATIONAL_STATE"));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map));

    act_val = 0;
    for (iter = 0; iter <  lt_map->num_maps; iter++) {
        if (((lt_map->map[iter]).policer == TRUE) &&
            ((lt_map->map[iter]).policer_pdd == FALSE) &&
            ((lt_map->map[iter]).sbr_enabled == FALSE)) {

            act_out = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(color_action_template,
                                       (lt_map->map[iter]).lt_field_name,
                                       &act_out));

            bcmint_field_action_buffer_set(unit,
                                           &(lt_map->map[iter]),
                                           &fdef,
                                           action,
                                           &curr_offset,
                                           (uint32_t *)&act_val,
                                           (uint32_t *)&act_out);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(color_action_template,
                                       (lt_map->map[iter]).lt_field_name,
                                       act_out));
            for (int_iter = 0; int_iter < lt_map->map[iter].num_internal_maps; int_iter++) {
                act_out = 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(
                            color_action_template,
                            lt_map->map[iter].internal_map[int_iter].lt_field_name,
                            &act_out));

                bcmint_field_int_action_buffer_set(
                    unit,
                    &(lt_map->map[iter].internal_map[int_iter]),
                    &fdef,
                    action,
                    &int_offset,
                    (uint32_t *)&act_val,
                    (uint32_t *)&act_out);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(
                            color_action_template,
                            lt_map->map[iter].internal_map[int_iter].lt_field_name,
                            act_out));
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              color_tbl_info->tbl_sid[pool],
                              &max_entries));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_color_profile_id_alloc(unit,
                                             color_tbl_info->tbl_sid[pool],
                                             color_tbl_info->tbl_key_fid[pool],
                                             color_tbl_info->pipe_fid,
                                             color_action_template,
                                             pipe,
                                             pool,
                                             color_tbl_info->num_pools,
                                             profile_ptr,
                                             entry_oper->color_action_id,
                                             &new_action_idx,
                                             &insert));

    if (insert == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(color_action_template,
                                   color_tbl_info->tbl_key_fid[pool],
                                   new_action_idx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit,
                                      color_action_template,
                                      BCMLT_PRIORITY_NORMAL));
    }

    entry_oper->color_action_id = new_action_idx;

    if (curr_color_action_idx != entry_oper->color_action_id) {
        delete = false;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_profile_id_free(
                    unit,
                    color_tbl_info->tbl_sid[pool],
                    pipe,
                    profile_ptr,
                    curr_color_action_idx,
                    pool,
                    color_tbl_info->num_pools,
                    &delete));
        if (delete == true) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(color_action_template,
                                       color_tbl_info->tbl_key_fid[pool],
                                       curr_color_action_idx));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit,
                                      color_action_template,
                                      BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_color_pdd_sbr_set(unit, stage_info,
                                                   group_oper, entry_oper,
                                                   pipe,
                                                   action));
    }

exit:
    if (BCMLT_INVALID_HDL != color_action_template) {
        (void) bcmlt_entry_free(color_action_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_color_pdd_sbr_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    int pipe,
    bcm_field_action_t action)
{
    int dunit = 0;
    int iter = 0;
    uint32_t param;
    const bcm_field_action_map_t *lt_map = NULL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmlt_entry_handle_t action_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    color_tbl_info = stage_info->color_action_tbls_info;
    tbls_info = stage_info->tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map));

    /* Entry does not have any more colored action.*/
    if (entry_oper->color_action_id == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->policy_sid,
                                  &action_template));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(action_template,
                                   tbls_info->policy_key_fid,
                                   entry_oper->entry_id));

        param = 0;
        for (iter = 0; iter <  lt_map->num_maps; iter++) {
            if ((lt_map->map[iter]).policer == TRUE) {
                continue;
            } else {
                /* Clear COLOR_TABLE_0/1_INDEX .*/
                if ((lt_map->map[iter]).color_index != -1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(action_template,
                                               (lt_map->map[iter]).lt_field_name,
                                               param));
                }

                /* Clear COLOR_PDD_SBR_0/1_INDEX .*/
                if ((lt_map->map[iter]).pdd_sbr_index != -1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(action_template,
                                               (lt_map->map[iter]).lt_field_name,
                                               param));
                }
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit,
                                      action_template,
                                      BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (BCMLT_INVALID_HDL != action_template) {
        (void) bcmlt_entry_free(action_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_mirror_set(int unit,
                               bcm_field_entry_t entry,
                               bcmint_field_stage_info_t *stage_info,
                               bcm_field_action_t action,
                               bcm_field_action_params_t *params)
{
    bcm_gport_t mirror_gport;
    SHR_FUNC_ENTER(unit);

    /* Mirror GPORT validation */
    if (BCM_GPORT_IS_MIRROR(params->param1) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    mirror_gport = params->param1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_action_mirror_set(unit,
                                           stage_info,
                                           entry,
                                           action,
                                           mirror_gport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_mirror_gport_get(int unit,
                                     bcm_field_entry_t entry,
                                     bcmint_field_stage_info_t *stage_info,
                                     bcm_field_action_t action,
                                     uint32 *mirror_gport,
                                     int *num_gports)
{
    int rv = SHR_E_NONE;
    bcmi_ltsw_mirror_info_t info;
    bool m_enable;
    int cont = 0, ct = 0, flags = 0;
    int mirror_dest = 0, mirror_index = 0;

    SHR_FUNC_ENTER(unit);
 
    for (cont = 0; cont < bcmiMirrorConLast; cont++) {
        m_enable = false;
        mirror_index = 0;
        rv = (bcmi_ltsw_field_mirror_index_get(
                    unit, entry, action,
                    cont, &m_enable, &mirror_index));
        if (SHR_FAILURE(rv) || (m_enable == false)) {
            continue;
        }

        info.mirror_cont = cont;
        info.mirror_index = mirror_index;
        flags = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mirror_dest_id_get(unit, &info,
                                          &mirror_dest,
                                          &flags));
        if (((action == bcmFieldActionMirrorIngress) &&
                    (flags & BCMI_LTSW_MIRROR_INGRESS)) ||
                ((action == bcmFieldActionMirrorEgress) &&
                 (flags & BCMI_LTSW_MIRROR_EGRESS))) {
            mirror_gport[ct++] = mirror_dest;
        }
    }

    *num_gports = ct;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_presel_action_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action,
                                    bcm_field_action_params_t *params)
{
    int dunit = 0;
    int iter = 0, entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    char *action_template_name = NULL, *action_template_id_name = NULL;
    uint64_t act_value = 0;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry, if its valid entry, update stage info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    /* Mask the stage bits from given entry ID. */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Retreive action template name from stage. */
    action_template_name = stage_info->tbls_info->presel_entry_sid;
    action_template_id_name = stage_info->tbls_info->presel_entry_key_fid;

    /* Entry handle allocate for action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                             &action_template_hdl));

    /* Add action template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               action_template_id_name,
                               entry_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, action_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Retreive previously set qualifier & mask value */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(action_template_hdl,
                               (lt_map_ref->map[iter]).lt_field_name,
                               &act_value));
    params->param0 = act_value;
    params->param1 = 0;
    params->param2 = 0;

exit:
    if (BCMLT_INVALID_HDL != action_template_hdl) {
        (void) bcmlt_entry_free(action_template_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_field_presel_action_set(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action,
                                    bcm_field_action_params_t *params)
{
    int dunit = 0;
    int iter = 0, entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    char *action_template_name = NULL, *action_template_id_name = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry, if its valid entry, update stage info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    /* Mask the stage bits from given entry ID. */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Retreive action template name from stage. */
    action_template_name = stage_info->tbls_info->presel_entry_sid;
    action_template_id_name = stage_info->tbls_info->presel_entry_key_fid;

    if (params->param0 >= 16) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Entry handle allocate for action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                             &action_template_hdl));

    /* Add action template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               action_template_id_name,
                               entry_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               (lt_map_ref->map[iter]).lt_field_name,
                               (uint64_t) params->param0));
    /* Insert action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, action_template_hdl,
                                     BCMLT_PRIORITY_NORMAL));


exit:
    if (BCMLT_INVALID_HDL != action_template_hdl) {
        (void) bcmlt_entry_free(action_template_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_field_presel_action_delete(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action)
{
    int dunit = 0;
    int iter = 0, entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    char *action_template_name = NULL, *action_template_id_name = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry, if its valid entry, update stage info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    /* Mask the stage bits from given entry ID. */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Retreive action template name from stage. */
    action_template_name = stage_info->tbls_info->presel_entry_sid;
    action_template_id_name = stage_info->tbls_info->presel_entry_key_fid;


    /* Entry handle allocate for action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                             &action_template_hdl));

    /* Add action template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               action_template_id_name,
                               entry_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               (lt_map_ref->map[iter]).lt_field_name,
                               0));
    /* Insert action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(action_template_hdl,
                            BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));


exit:
    if (BCMLT_INVALID_HDL != action_template_hdl) {
        (void) bcmlt_entry_free(action_template_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_entry_strength_action_update(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcm_field_action_t action,
    const bcm_field_action_map_t *lt_map_ref,
    bool add)
{
    int iter;
    bool sbr_set = false;
    bcm_field_aset_t new_aset;

    SHR_FUNC_ENTER(unit);

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR)) {
        SHR_EXIT();
    }

    if (!((entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) &&
          (group_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE))) {
        SHR_EXIT();
    }

    if (lt_map_ref == NULL) {
        /* Find lt_map for provided action from db. */
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmint_field_action_map_find(unit, action,
                                           stage_info, &lt_map_ref));
    }

    if (add) {
        for (iter = 0; iter < entry_oper->num_actions; iter++) {
            if (entry_oper->action_arr[iter] == action) {
                SHR_EXIT();
            }
        }
    }

    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
         if ((lt_map_ref->map[iter]).sbr_enabled == TRUE) {
             sbr_set = true;
         }
    }

    if (sbr_set == true) {
        if (add) {
            BCM_FIELD_ASET_INIT(new_aset);
            BCM_FIELD_ASET_ADD(new_aset, action);
        }

        /* Create SBR strength profile */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_strength_aset_create(
                        unit, stage_info,
                        group_oper, entry_oper,
                        (add == 1) ? &new_aset : NULL,
                        1));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_field_entry_action_present(
        int unit,
        bcm_field_action_t action,
        bcmi_field_ha_entry_oper_t *entry_oper,
        bool *is_action_found)
{
    int indx = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry_oper, SHR_E_INTERNAL);
    SHR_NULL_CHECK(is_action_found, SHR_E_INTERNAL);

    /* check if action is preseent in entry_oper action_arr*/
    for (indx = 0; indx < entry_oper->num_actions; indx++) {
        if (entry_oper->action_arr[indx] == action) {
            *is_action_found = true;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_config_info_add(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action,
                                    bcm_field_action_params_t *params,
                                    bcm_field_action_match_config_t *match_config)
{
    bool found;
    int dunit = 0;
    int flags = 0;
    int pipe = -1;
    bool action_added = false;
    char *action_field = NULL;
    uint8_t act_ip_buf_size = 0;
    uint16_t lt_map_action_width = 0;
    uint64_t old_action_prof_val = 0;
    uint16_t old_action_profile_idx = 0;
    int idx = 0, iter = 0, entry_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    uint32_t num_element = 0, src_ofset_current = 0;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    uint64_t *act_value_inp_buf = NULL, *act_value_op_buf = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL, *fld_defs_iter = NULL;
    bcmlt_entry_handle_t action_template_lkup_hdl = BCMLT_INVALID_HDL;
    char *action_template_name = NULL, *action_template_id_name = NULL;
    char *lt_sym_str_op = NULL;
    bool *lt_field_map = NULL;
    uint16_t map_flags = 0;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Find lt_map for provided qualifier from db */
    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        if (action == bcmFieldActionGroupClassSet) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_action_set(unit,
                                           entry,
                                           action,
                                           params));
            SHR_FUNC_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

    /* Validate the entry, if its valid entry, update stage info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Find lt_map for provided action from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    /* Mask the stage bits from given entry ID. */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Get the entry hash info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Get the group hash info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit,
                                        entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    /* Add only if the action is part of Group's ASET. */
    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        if (entry_oper->entry_flags &
            BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
            FP_HA_GROUP_OPER_DEF_ENTRY_ASET_TEST(group_oper, action, found);
        } else {
            FP_HA_GROUP_OPER_ASET_TEST(group_oper, action, found);
        }
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

    if ((action == bcmFieldActionMirrorIngress) ||
         (action == bcmFieldActionMirrorEgress)) {
        /* Mirror actions */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_mirror_set(unit, entry, stage_info,
                                                    action, params));
        action_added = true;
    } else if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
               (BCMINT_FIELD_IS_COLORED_ACTION(action))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                entry_oper->group_id,
                                                &pipe));
        /* Add colored actions. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_color_set(unit, stage_info,
                                           group_oper, entry_oper,
                                           pipe,
                                           action, params));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_color_pdd_sbr_set(unit, stage_info,
                                                   group_oper, entry_oper,
                                                   pipe,
                                                   action));
        action_added = true;
    }

    /* Entry based SBR handling */
    if (action_added == true) {
        action_added = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_strength_action_update(unit,
                        stage_info, group_oper,
                        entry_oper, action, lt_map_ref, 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_entry_update(unit, entry_oper,
                                              action, true));
        SHR_EXIT();
    }

    /* Find earlier associated profile index for valid actions. */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_field_action_profile_index_get(unit, entry, action,
                                                 &old_action_profile_idx),
                                                 _SHR_E_NOT_FOUND);

    /* Retreive action template name from stage. */
    action_template_name = stage_info->tbls_info->policy_sid;
    action_template_id_name = stage_info->tbls_info->policy_key_fid;

    /* Allocate memory for all action ltmap fields. */
    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t) * lt_map_ref->num_maps,
                                                    "field definitions");
    fld_defs_iter = fld_defs_ref;

    /* Iterate through all lt maps */
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {

        SHR_IF_ERR_VERBOSE_EXIT
          (bcmi_lt_field_def_get(unit, action_template_name,
             (lt_map_ref->map[iter]).lt_field_name, fld_defs_iter));

        for (idx = 0; idx < (lt_map_ref->map[iter]).num_offsets; idx++) {
            if ((lt_map_ref->map[iter]).width[idx] == 0) {
                lt_map_action_width += fld_defs_iter->width;
                break;
            } else {
                lt_map_action_width += (lt_map_ref->map[iter]).width[idx];
            }
        }
        fld_defs_iter++;
    }

    /* Entry handle allocate for action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                             &action_template_hdl));

    /* Entry handle allocate for action lookup template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                              &action_template_lkup_hdl));

    /* Add action template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               action_template_id_name,
                               entry_id));

    /* Add action lookup template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_lkup_hdl,
                               action_template_id_name,
                               entry_id));

    /* Look for action template entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, action_template_lkup_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Allocate memory to hold action input values */
    act_ip_buf_size = sizeof(uint64_t);
    BCMINT_FIELD_MEM_ALLOC
        (act_value_inp_buf, act_ip_buf_size, "ip action buf");

    /* Total action width requested cannot be
     * greater than allocated buffer size */
    if (lt_map_action_width > (act_ip_buf_size * 8)) {
        LOG_DEBUG(BSL_LS_BCM_FP,
           (BSL_META_U(unit, "Error: action total width "
               "exceeds mem allocated for i/p buffer, check db.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    BCMINT_FIELD_MEM_ALLOC
        (lt_field_map, (sizeof(bool) * lt_map_ref->num_maps),
         "lt field valid map");
    map_flags |= BCMINT_FIELD_ENTRY_ACTION_INVALID_LT_FIELDS_SET;

    /* Set value inp buffer to be set in LT field
     * as per action & params provided by user. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_param_set(unit, stage_info,
                                       entry_oper, action,
                                       params, &flags,
                                       lt_map_action_width,
                                       match_config,
                                       act_value_inp_buf));

    /* Get valid LT fields for current action as per parameter info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_action_lt_map_valid_info_get(unit,
                                            stage_info->stage_id,
                                            entry_id, action,
                                            map_flags,
                                            lt_map_ref->num_maps,
                                            lt_field_map));

    if (*act_value_inp_buf > ((1 << lt_map_action_width) - 1) ) {
        LOG_DEBUG(BSL_LS_BCM_FP,
           (BSL_META_U(unit, "Error: action param value "
               "exceeds max allowed, check param value passed.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*
     * Sequence to set a value in a lt map is:
     *  Go though all lt maps for a qualifier
     *  - get current value set for a lt field
     *  - set new incoming value
     *    - loop through all offsets/width and set in field
     */
    src_ofset_current = 0;
    fld_defs_iter = fld_defs_ref;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (lt_field_map[iter] == 1) {
            continue;
        }
        BCMINT_FIELD_MEM_ALLOC
            (act_value_op_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
             "action value");
        BCMINT_FIELD_MEM_ALLOC
            (action_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 1),
             "action field");

        sal_strcpy(action_field, (lt_map_ref->map[iter]).lt_field_name);
        num_element = fld_defs_iter[iter].elements;

        if (fld_defs_iter[iter].elements > 1) {

            if (fld_defs_iter[iter].symbol) {
                /* Not handled till now as no such case
                 * is present in db till date */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);

            } else {

                /* Set internal maps associated if any. */
                if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_internal_map_set(
                                     unit,
                                     1, action, action_template_hdl,
                                     action_template_lkup_hdl, stage_info,
                                     &(lt_map_ref->map[iter]), 0, 0, 0, NULL));
                }

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(action_template_lkup_hdl,
                                                 action_field,
                                                 0, act_value_op_buf,
                                                 num_element, &num_element));

                /* Set values in input buffer as per offset & width specified */
                bcmint_field_action_buffer_set(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                 action,
                                                 &src_ofset_current,
                                                (uint32_t *)act_value_inp_buf,
                                                (uint32_t *)act_value_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(action_template_hdl,
                                                 action_field,
                                                 0, act_value_op_buf,
                                                 num_element));
            }

        } else {

            if (fld_defs_iter[iter].symbol) {
                BCMINT_FIELD_MEM_ALLOC
                    (lt_sym_str_op, 21, "Action ltstr buf");
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_field_action_param_symbol_set(unit,
                                                             action,
                                                             params,
                                                             lt_sym_str_op));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(action_template_hdl,
                                                  action_field,
                                                  lt_sym_str_op));
            } else {

                /* Set internal maps associated if any. */
                if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_action_internal_map_set(
                                     unit,
                                     1, action, action_template_hdl,
                                     action_template_lkup_hdl, stage_info,
                                     &(lt_map_ref->map[iter]), 0, 0, 0, NULL));
                }

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(action_template_lkup_hdl,
                                           action_field,
                                           act_value_op_buf));

                /* Set values in input buffer as per offset & width specified */
                bcmint_field_action_buffer_set(unit,
                                               &(lt_map_ref->map[iter]),
                                               &fld_defs_iter[iter],
                                               action,
                                               &src_ofset_current,
                                               (uint32_t *)act_value_inp_buf,
                                               (uint32_t *)act_value_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(action_template_hdl,
                                           action_field,
                                          *act_value_op_buf));
            }
        }
        BCMINT_FIELD_MEM_FREE(action_field);
        BCMINT_FIELD_MEM_FREE(act_value_op_buf);
        BCMINT_FIELD_MEM_FREE(lt_sym_str_op);
    }

    /* Insert action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, action_template_hdl,
                                     BCMLT_PRIORITY_NORMAL));

    /* Delete old profile if any found. */
    if (old_action_profile_idx != 0) {
        old_action_prof_val = old_action_profile_idx;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_action_profile_del(unit, action,
                                                stage_info->stage_id,
                                                &old_action_prof_val));
    }

    if (!BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        action_added = true;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_strength_action_update(unit,
                        stage_info, group_oper,
                        entry_oper, action, lt_map_ref, 1));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_entry_update(unit, entry_oper,
                                              action, true));
    }

exit:
    if (SHR_FUNC_ERR() && (action_added == true)) {
        (void) bcmint_field_action_entry_update(unit, entry_oper,
                                                action, true);
        (void) bcmint_field_action_delete(unit, entry,
                                          action, 0, 0, 1);
        (void) bcmint_field_action_entry_update(unit, entry_oper,
                                                action, false);
    }

    if (BCMLT_INVALID_HDL != action_template_hdl) {
        (void) bcmlt_entry_free(action_template_hdl);
    }
    if (BCMLT_INVALID_HDL != action_template_lkup_hdl) {
        (void) bcmlt_entry_free(action_template_lkup_hdl);
    }
    if (lt_sym_str_op != NULL) {
        BCMINT_FIELD_MEM_FREE(lt_sym_str_op);
    }
    BCMINT_FIELD_MEM_FREE(action_field);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(act_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(act_value_op_buf);
    BCMINT_FIELD_MEM_FREE(lt_field_map);
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_config_info_get(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_action_t action,
                                    uint64_t *action_lt_val,
                                    bcm_field_action_params_t *params,
                                    bcm_field_action_match_config_t *match_config)
{
    int dunit = 0;
    int pipe = -1;
    bool found;
    uint64_t action_val = 0;
    char *action_field = NULL;
    uint8_t act_ip_buf_size = 0;
    uint32_t mirror_gport[4] = {0};
    int iter = 0, entry_id = 0, num_gports = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    uint32_t num_element = 0, src_ofset_current = 0;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    uint64_t *act_value_inp_buf = NULL, *act_value_op_buf = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL, *fld_defs_iter = NULL;
    char *action_template_name = NULL, *action_template_id_name = NULL;
    bool is_action_found = false;
    char *lt_sym_str_op = NULL;
    bool *lt_field_map = NULL;
    uint16_t map_flags = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(params, SHR_E_PARAM);
    dunit = bcmi_ltsw_dev_dunit(unit);

    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        if (action == bcmFieldActionGroupClassSet) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_action_get(unit,
                                           entry,
                                           action,
                                           params));
            SHR_FUNC_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Find lt_map for provided action from db */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Get the entry hash info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Get the group hash info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        /* Get only if the action is part of Group's ASET */
        if (entry_oper->entry_flags &
            BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
            FP_HA_GROUP_OPER_DEF_ENTRY_ASET_TEST(group_oper, action, found);
        } else {
            FP_HA_GROUP_OPER_ASET_TEST(group_oper, action, found);
        }
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    /* check if action is preseent in entry_oper action_arr*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_entry_action_present(unit,
                                                action,
                                                entry_oper,
                                                &is_action_found));

    if (!(is_action_found)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Mirror actions */
    if ((action == bcmFieldActionMirrorIngress) ||
        (action == bcmFieldActionMirrorEgress)) {
        num_gports = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_mirror_gport_get(unit, entry,
                                                stage_info,
                                                action, mirror_gport,
                                                &num_gports));
        params->param1 = mirror_gport[0];
        SHR_EXIT();
    }

    if ((stage_info->flags &
                BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
                    (BCMINT_FIELD_IS_COLORED_ACTION(action))) {
        if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
            FP_HA_GROUP_OPER_ASET_TEST(group_oper,
                                bcmFieldActionPolicerGroup, found);
            if (found == FALSE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                entry_oper->group_id,
                                                &pipe));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_color_get(unit,
                                           stage_info,
                                           group_oper,
                                           entry_oper,
                                           pipe,
                                           action,
                                           &action_val));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_param_get(unit, stage_info,
                                           entry_oper, action,
                                           params, match_config,
                                           &action_val));
        SHR_EXIT();
    }

    /* Retreive action template name from stage. */
    action_template_name = stage_info->tbls_info->policy_sid;
    action_template_id_name = stage_info->tbls_info->policy_key_fid;

    /* Validate action lt map for valid values. */
    BCMINT_FIELD_MEM_ALLOC
        (lt_field_map, (sizeof(bool) * lt_map_ref->num_maps),
         "lt field valid map");
    map_flags |= BCMINT_FIELD_ENTRY_ACTION_INVALID_LT_FIELDS_SET;

    /* Get valid LT fields for current action as per parameter info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_action_lt_map_valid_info_get(unit,
                                            stage_info->stage_id,
                                            entry_id, action,
                                            map_flags,
                                            lt_map_ref->num_maps,
                                            lt_field_map));

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t) * lt_map_ref->num_maps,
         "field definitions");
    fld_defs_iter = fld_defs_ref;

    /* Iterate through all lt maps */
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        /* Retreive field definition for provided lt field in map */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, action_template_name,
              (lt_map_ref->map[iter]).lt_field_name, fld_defs_iter));

        fld_defs_iter++;
    }

    /* Entry handle allocate for action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                              &action_template_hdl));

    /* Add action template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               action_template_id_name,
                               entry_id));

    /* Look for action template entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, action_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Allocate input buffer */
    act_ip_buf_size = sizeof(uint64_t);
    BCMINT_FIELD_MEM_ALLOC
        (act_value_inp_buf, act_ip_buf_size, "ip action buf");

    /*
     * Sequence to get a value in a lt map is:
     *  Go though all lt maps for a qualifier
     *    - loop through all offsets/width and get field value
     */
    src_ofset_current = 0;
    fld_defs_iter = fld_defs_ref;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (lt_field_map[iter] == 1) {
            continue;
        }
        BCMINT_FIELD_MEM_ALLOC
            (act_value_op_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
             "action value");
        BCMINT_FIELD_MEM_ALLOC
            (action_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 1),
             "action field");

        sal_strcpy(action_field, (lt_map_ref->map[iter]).lt_field_name);
        num_element = fld_defs_iter[iter].elements;

        if (fld_defs_iter[iter].elements > 1) {
            if (fld_defs_iter[iter].symbol) {
                /* Not handled till now as no such
                 * case is present in db till date */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);

            } else {
                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(action_template_hdl,
                                                 action_field,
                                                 0, act_value_op_buf,
                                                 num_element, &num_element));

                /* Get values in input buffer as per offset & width specified */
                bcmint_field_action_buffer_get(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                 action,
                                                 &src_ofset_current,
                                                 (uint32_t *)act_value_inp_buf,
                                                 (uint32_t *)act_value_op_buf);
            }
        } else {
            if (fld_defs_iter[iter].symbol) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(action_template_hdl,
                                                  action_field,
                                                  (const char **)&lt_sym_str_op));
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_field_action_param_symbol_get(unit,
                                                             action,
                                                             params,
                                                             lt_sym_str_op));
            } else {

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(action_template_hdl,
                                           action_field,
                                           act_value_op_buf));

                /* Set values in input buffer as per offset & width specified */
                bcmint_field_action_buffer_get(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                 action,
                                                 &src_ofset_current,
                                                 (uint32_t *)act_value_inp_buf,
                                                 (uint32_t *)act_value_op_buf);
            }
        }
        BCMINT_FIELD_MEM_FREE(action_field);
        BCMINT_FIELD_MEM_FREE(act_value_op_buf);
    }

    /* Get LT field values set. */
    if (action_lt_val != NULL) {
        *action_lt_val = *act_value_inp_buf;
    }

    if (!fld_defs_iter[0].symbol) {
        /* Populate params from action buffer value. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_param_get(unit, stage_info,
                                           entry_oper, action,
                                           params, match_config,
                                           act_value_inp_buf));
    }

exit:
    if (BCMLT_INVALID_HDL != action_template_hdl) {
        (void) bcmlt_entry_free(action_template_hdl);
    }
    BCMINT_FIELD_MEM_FREE(action_field);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(act_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(act_value_op_buf);
    BCMINT_FIELD_MEM_FREE(lt_field_map);
    SHR_FUNC_EXIT();
}

STATIC int
bcmint_field_action_default_value_get(int unit,
        bcm_field_entry_t entry,
        bcmint_field_stage_info_t *stage_info,
        bcm_field_action_t action,
        bool del_action,
        bcmlt_field_def_t *fld_defs_out)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    rv = mbcm_ltsw_field_action_default_value_get(unit, entry, stage_info, action,
                                                  del_action, fld_defs_out);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv,BCM_E_UNAVAIL);
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_delete(int unit,
                           bcm_field_entry_t entry,
                           bcm_field_action_t action,
                           uint32 param0, uint32 param1,
                           int no_param)
{
    bool found;
    int pipe = -1;
    int dunit = 0;
    char *action_field = NULL;
    bool action_deleted = false;
    uint16_t ip_op_buf_size = 0;
    uint64_t old_action_prof_val = 0;
    int i = 0, iter = 0, entry_id = 0;
    uint16_t old_action_profile_idx = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    uint32_t num_element = 0, src_ofset_current = 0;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t action_template_hdl = BCMLT_INVALID_HDL;
    uint64_t *act_value_inp_buf = NULL, *act_value_op_buf = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL, *fld_defs_iter = NULL;
    bcmlt_entry_handle_t action_template_lkup_hdl = BCMLT_INVALID_HDL;
    char* action_template_name = NULL, *action_template_id_name = NULL;
    uint16 indx = 0;
    bool is_action_found = false;
    int num_gports = 0;
    uint32 mirror_gport[4] = {0};

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        if (action == bcmFieldActionGroupClassSet) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_action_delete(unit,
                                           entry,
                                           action));
            SHR_FUNC_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Find lt_map for provided qualifier from db. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Get the entry hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, entry_oper->group_id,
                                        stage_info,
                                        &group_oper));

    /* Delete only if action is part of Group's ASET */
    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        if (entry_oper->entry_flags &
            BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
            FP_HA_GROUP_OPER_DEF_ENTRY_ASET_TEST(group_oper, action, found);
        } else {
            FP_HA_GROUP_OPER_ASET_TEST(group_oper, action, found);
        }
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* check if action is preseent in entry_oper action_arr*/
    for (indx = 0; indx < entry_oper->num_actions; indx++) {
        if (entry_oper->action_arr[indx] == action) {
            is_action_found = true;
            break;
        }
    }

    if (!(is_action_found)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Mirror actions */
    if ((action == bcmFieldActionMirrorIngress) ||
        (action == bcmFieldActionMirrorEgress)) {
        if (no_param) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_mirror_remove_all(unit, entry,
                                                     stage_info,
                                                     action));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_mirror_remove(unit, entry, stage_info,
                                                 action, param1));

        }

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmint_field_action_mirror_gport_get(
                      unit, entry,stage_info, action,
                      mirror_gport, &num_gports), SHR_E_NOT_FOUND);
        if (num_gports != 0) {
            SHR_EXIT();
        }
        action_deleted = true;
    }

    if ((stage_info->flags &
            BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
                            (BCMINT_FIELD_IS_COLORED_ACTION(action))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                entry_oper->group_id,
                                                &pipe));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_color_remove(unit, stage_info,
                                              group_oper, entry_oper,
                                              pipe,
                                              action));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_color_pdd_sbr_remove(unit, stage_info,
                                                      group_oper, entry_oper,
                                                      pipe,
                                                      action));
        action_deleted = true;
    }

    /* Entry based SBR handling */
    if (action_deleted == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_entry_update(unit, entry_oper,
                                              action, false));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_strength_action_update(unit,
                        stage_info, group_oper,
                        entry_oper, action, lt_map_ref, 0));
        SHR_EXIT();
    }


    /* Find earlier associated profile index for valid actions. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_action_profile_index_get(unit, entry, action,
                                                  &old_action_profile_idx));

    /* Retreive action template name from stage. */
    action_template_name = stage_info->tbls_info->policy_sid;
    action_template_id_name = stage_info->tbls_info->policy_key_fid;

    /* Check all lt maps width is within max allowed range */
    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t) * lt_map_ref->num_maps,
         "field definitions");
    fld_defs_iter = fld_defs_ref;

    /* Iterate through all lt maps */
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        /* Retreive field definition for provided lt field in map */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, action_template_name,
              (lt_map_ref->map[iter]).lt_field_name, fld_defs_iter));

        fld_defs_iter++;
    }

    /* Entry handle allocate for action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                              &action_template_hdl));

    /* Entry handle allocate for action lookup template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              action_template_name,
                              &action_template_lkup_hdl));

    /* Add action template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_hdl,
                               action_template_id_name,
                               entry_id));

    /* Add action lookup template_id field to action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(action_template_lkup_hdl,
                               action_template_id_name,
                               entry_id));

    /* Look for action template entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, action_template_lkup_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /*  Go though all lt maps for a action */
    found = 0;
    fld_defs_iter = fld_defs_ref;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_default_value_get(unit, entry,
                                               stage_info,
                                               action,
                                               1,
                                               fld_defs_iter));
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        ip_op_buf_size = sizeof(uint64_t) * fld_defs_iter[iter].elements;
        BCMINT_FIELD_MEM_ALLOC
            (act_value_inp_buf, ip_op_buf_size, "inp buf value");
        BCMINT_FIELD_MEM_ALLOC
            (act_value_op_buf, ip_op_buf_size, "op buf value");
        BCMINT_FIELD_MEM_ALLOC
            (action_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 1),
                                                               "action field");

        /* Initialize with field default values */
        *act_value_inp_buf = fld_defs_iter[iter].def;
        src_ofset_current = 0;

        sal_strcpy(action_field, (lt_map_ref->map[iter]).lt_field_name);
        num_element = fld_defs_iter[iter].elements;

        if (fld_defs_iter[iter].elements > 1) {

            if (fld_defs_iter[iter].symbol) {
                /* Not handled till now as no such case is present
                 * in db till date */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);

            } else {

                /* Set internal maps associated if any. */
                if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                     (bcmint_field_action_internal_map_set(unit,
                                    0, action, action_template_hdl,
                                    action_template_lkup_hdl, stage_info,
                                    &(lt_map_ref->map[iter]), 0, 0, 0, NULL));
                }

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(action_template_lkup_hdl,
                                                 action_field,
                                                 0, act_value_op_buf,
                                                 num_element, &num_element));

                for (i = 0; i < num_element; i++) {
                    if (act_value_op_buf[i] != 0) {
                        found = 1;
                    }
                }

                /* Set values in input buffer as per offset & width specified */
                bcmint_field_action_buffer_set(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                 action,
                                                 &src_ofset_current,
                                                 (uint32_t *)act_value_inp_buf,
                                                 (uint32_t *)act_value_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(action_template_hdl,
                                                 action_field,
                                                 0, act_value_op_buf,
                                                 num_element));
            }

        } else {

            if (fld_defs_iter[iter].symbol) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(action_template_hdl,
                                                  action_field,
                                                  fld_defs_iter[iter].sym_def));
            } else {

                /* Set internal maps associated if any. */
                if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                     (bcmint_field_action_internal_map_set(unit,
                                    0, action, action_template_hdl,
                                    action_template_lkup_hdl, stage_info,
                                    &(lt_map_ref->map[iter]), 0, 0, 0, NULL));
                }

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(action_template_lkup_hdl,
                                           action_field,
                                           act_value_op_buf));
                if (*act_value_op_buf != 0) {
                    found = 1;
                }

                /* Set values in input buffer as per offset & width specified */
                bcmint_field_action_buffer_set(unit,
                                              &(lt_map_ref->map[iter]),
                                              &fld_defs_iter[iter],
                                               action,
                                              &src_ofset_current,
                                              (uint32_t *)act_value_inp_buf,
                                              (uint32_t *)act_value_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(action_template_hdl,
                                           action_field,
                                           *act_value_op_buf));
            }
        }
        BCMINT_FIELD_MEM_FREE(action_field);
        BCMINT_FIELD_MEM_FREE(act_value_inp_buf);
        BCMINT_FIELD_MEM_FREE(act_value_op_buf);
    }

    /* If action all values were 0 means it was never added. */
    if (!found) {
        if (!is_action_found) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* Update action template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, action_template_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    /* Delete action profile if any found. */
    if (old_action_profile_idx != 0) {
        old_action_prof_val = old_action_profile_idx;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_action_profile_del(unit, action,
                                                stage_info->stage_id,
                                                &old_action_prof_val));
    }

    /* reset entry action flags */
    if (bcmFieldActionIngressGportSet == action) {
        entry_oper->action_flags &= ~(BCMINT_FIELD_ENTRY_ACTION_GPORT_FLAGS);
    }

    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_entry_update(unit, entry_oper,
                                              action, false));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_strength_action_update(unit,
                        stage_info, group_oper,
                        entry_oper, action, lt_map_ref, 0));
    }

exit:
    if (BCMLT_INVALID_HDL != action_template_hdl) {
        (void) bcmlt_entry_free(action_template_hdl);
    }

    if (BCMLT_INVALID_HDL != action_template_lkup_hdl) {
        (void) bcmlt_entry_free(action_template_lkup_hdl);
    }
    BCMINT_FIELD_MEM_FREE(action_field);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(act_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(act_value_op_buf);
    SHR_FUNC_EXIT();
}

int
bcmint_field_action_delete_all(int unit,
                               bcm_field_entry_t entry)
{
    int rv = 0;
    int a, entry_id = 0;
    uint16_t num_actions = 0;
    bcm_field_action_t action = 0;
    bcm_field_action_t *action_arr = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bool is_default_entry = FALSE;

    SHR_FUNC_ENTER(unit);

    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
       SHR_IF_ERR_VERBOSE_EXIT
           (bcmint_field_presel_action_delete(unit,
                                      entry,
                                      bcmFieldActionGroupClassSet));
       SHR_FUNC_EXIT();
    }

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Get the entry hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry_id,
                                        stage_info,
                                        &entry_oper));

    /* Get the group hash info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit,
                                        entry_oper->group_id,
                                        stage_info,
                                        &group_oper));
    if ((entry_oper->entry_flags) &
        BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
        is_default_entry = TRUE;
    }

    if (entry_oper->num_actions) {
        int size;

        num_actions = entry_oper->num_actions;
        size = sizeof(bcm_field_action_t) * entry_oper->num_actions;

        BCMINT_FIELD_MEM_ALLOC
            (action_arr, size,
             "Action array alloc during action delete all");

        for (a = 0; a < num_actions; a++) {
            action_arr[a] = entry_oper->action_arr[a];
        }
    }

    for (a = 0; a < num_actions; a++) {
        action = action_arr[a];
        if ((bcmFieldActionStatGroup == action) ||
            (bcmFieldActionStatGroupWithoutCounterIndex == action) ||
            (bcmFieldActionPolicerGroup == action) ||
            (bcmFieldActionRpStatGroup == action) ||
            (bcmFieldActionYpStatGroup == action) ||
            (bcmFieldActionGpStatGroup == action)) {
            /* Need to do stat detach operations */
            continue;
        }

        rv = bcmint_field_action_delete(unit, entry, action, 0, 0, 1);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (is_default_entry) {
        for (a = 0; a < group_oper->def_action_cnt; a++) {
            action = group_oper->def_aset_arr[a];
            if ((bcmFieldActionStatGroup == action) ||
                (bcmFieldActionStatGroupWithoutCounterIndex == action) ||
                (bcmFieldActionPolicerGroup == action) ||
                (bcmFieldActionRpStatGroup == action) ||
                (bcmFieldActionYpStatGroup == action) ||
                (bcmFieldActionGpStatGroup == action)) {
                /* Need to do stat detach operations */
                continue;
            }

            rv = (bcmint_field_action_delete(unit, entry, action, 0, 0, 1));
            /* Ignore.. Action may not be supported by the device
             * or not added to entry */
            if (SHR_E_NOT_FOUND == rv) {
                rv = SHR_E_NONE;
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (action_arr != NULL) {
        BCMINT_FIELD_MEM_FREE(action_arr);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_field_action_map_sbr_aset_get(int unit,
                                     bcmint_field_stage_info_t *stage_info,
                                     const char *lt_field_name,
                                     bcmi_field_aset_t *aset,
                                     int *action_cnt)
{
    int idx, iter, action_db_cnt = 0;
    const bcm_field_action_map_t *ltmap = NULL;
    const bcm_field_action_map_t *ltmap_tmp = NULL;

    SHR_FUNC_ENTER(unit);

    ltmap = stage_info->action_lt_map_db;
    action_db_cnt = stage_info->action_db_count - 1;

    *action_cnt = 0;
    BCMI_FIELD_ASET_INIT(*aset);
    for (idx = 0; idx <= action_db_cnt; idx++) {
        ltmap_tmp = ltmap + idx;
        for (iter = 0; iter < ltmap_tmp->num_maps; iter++) {
            if ((ltmap_tmp != NULL) &&
                    (sal_strcmp(lt_field_name,
                           ltmap_tmp->map[iter].lt_field_name) == 0)) {
                if (ltmap_tmp->map[iter].sbr_enabled == false) {
                    continue;
                }
                if (ltmap_tmp->action == bcmFieldActionStatGroup) {
                    continue;
                }
                BCM_FIELD_ASET_ADD(*aset, ltmap_tmp->action);
                *action_cnt += 1;
            }
        }
    }

    if (*action_cnt == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_field_mirror_index_get(
    int unit,
    int entry,
    bcm_field_action_t action,
    int mirror_cont,
    bool *enable,
    int *mirror_index)
{
    bcmint_field_stage_info_t *stage_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mirror_index, SHR_E_PARAM);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_action_mirror_index_get(
               unit,
               stage_info,
               entry,
               action,
               mirror_cont,
               enable,
               mirror_index));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to retrieve SBR action set for the given ASET.
 */
int
bcmint_field_action_strength_aset_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_aset_t *aset,
    bcmi_field_aset_t *sbr_aset,
    uint8_t *a_cnt,
    bcmi_field_aset_t *sbr_color_aset,
    uint8_t *color_a_cnt)
{
    int cnt;
    int iter = 0;
    bcmi_field_action_t action;
    bcmi_field_aset_t map_aset;
    const bcm_field_action_map_t *lt_map_ref = NULL;

    SHR_FUNC_ENTER(unit);

    for (action = 0; action < bcmiFieldActionCount; action++) {
        if (!BCM_FIELD_ASET_TEST(*aset, action)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_map_find(unit, action,
                                   stage_info, &lt_map_ref));

        for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
            if ((lt_map_ref->map[iter]).sbr_enabled == FALSE) {
                continue;
            }

            if (lt_map_ref->action == bcmFieldActionStatGroup) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_action_map_sbr_aset_get(
                        unit,
                        stage_info,
                        lt_map_ref->map[iter].lt_field_name,
                        &map_aset, &cnt));
            if ((lt_map_ref->map[iter]).policer == FALSE) {
                if (sbr_aset) {
                    *a_cnt += cnt;
                    BCMI_FIELD_ASET_OR((*sbr_aset), map_aset,
                                       bcmFieldActionCount);
                }
            } else {
                if (sbr_color_aset) {
                    *color_a_cnt += cnt;
                    BCMI_FIELD_ASET_OR((*sbr_color_aset), map_aset,
                                       bcmFieldActionCount);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_stage_aset_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_aset_t *aset)
{
    int iter;
    bcmi_ltsw_field_stage_t stage_id;
    const bcm_field_action_map_t *lt_map_db;
    bcmint_field_stage_info_t *stage_info = NULL;
    SHR_FUNC_ENTER(unit);

    switch(stage) {
        case bcmFieldStageIngress:
            stage_id = bcmiFieldStageIngress;
            break;
        case bcmFieldStageEgress:
            stage_id = bcmiFieldStageEgress;
            break;
        case bcmFieldStageLookup:
            stage_id = bcmiFieldStageVlan;
            break;
        case bcmFieldStageIngressExactMatch:
            stage_id = bcmiFieldStageExactMatch;
            break;
        case bcmFieldStageIngressFlowtracker:
            stage_id = bcmiFieldStageFlowTracker;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    if (stage_info->action_lt_map_db == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    BCM_FIELD_ASET_INIT(*aset);
    for (iter = 0; iter < stage_info->action_db_count; iter++) {
        lt_map_db = stage_info->action_lt_map_db + iter;
        BCM_FIELD_ASET_ADD(*aset, (int)lt_map_db->action);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_field_action_cont_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_action_t action,
    uint8_t max_ct,
    uint16_t *cont_id_arr,
    uint16_t *cont_size_arr,
    uint8_t *actual_ct,
    uint8_t *action_size)
{
    uint8_t cnt = 0;
    uint32_t elem = 0;
    uint64_t cont_id;
    uint64_t cont_sz;
    uint16_t iter, iter1;
    uint16_t cont;
    uint64_t num_cont = 0;
    bcmi_ltsw_field_stage_t stage_id;
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    const bcm_field_action_map_info_t *map;
    const bcm_field_action_internal_map_info_t *lt_map_int_ref;
    bcmlt_entry_handle_t act_cont_info_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t int_act_cont_info_templ = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    /* Fixed field in TCAM, not part of action container.*/
    if (action == bcmFieldActionGroupClassSet) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    switch(stage) {
        case bcmFieldStageIngress:
            stage_id = bcmiFieldStageIngress;
            break;
        case bcmFieldStageEgress:
            stage_id = bcmiFieldStageEgress;
            break;
        case bcmFieldStageLookup:
            stage_id = bcmiFieldStageVlan;
            break;
        case bcmFieldStageIngressExactMatch:
            stage_id = bcmiFieldStageExactMatch;
            break;
        case bcmFieldStageIngressFlowtracker:
            stage_id = bcmiFieldStageFlowTracker;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    if ((stage_info->tbls_info->action_cont_info_sid == NULL) ||
        (stage_info->tbls_info->action_cont_info_key_fid == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_map_find(unit, action,
                                      stage_info, &lt_map_ref));

    if (actual_ct == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else {
        *actual_ct = 0;
    }

    if (action_size) {
        *action_size = 0;
    }

    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        map = &(lt_map_ref->map[iter]);
        if (map->policer == TRUE) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(
                      unit,
                      &act_cont_info_templ,
                      BCMLT_OPCODE_LOOKUP,
                      BCMLT_PRIORITY_NORMAL,
                      stage_info->tbls_info->action_cont_info_sid,
                      stage_info->tbls_info->action_cont_info_key_fid,
                      1, 0, (char *)map->lt_field_name,
                      NULL, 0, 0, 0, 0));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(act_cont_info_templ,
                                   "NUM_CONTAINERS",
                                   &num_cont));

        for (cont = 0; cont < num_cont; cont++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                       act_cont_info_templ,
                       "CONT_ID", cont,
                       &cont_id, 1, &elem));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(
                       act_cont_info_templ,
                       "CONT_SIZE", cont,
                       &cont_sz, 1, &elem));
            if (cnt < max_ct) {
                cont_id_arr[cnt] = cont_id;
                cont_size_arr[cnt] = cont_sz;
            }

            if (action_size) {
                *action_size += cont_sz;
            }
            cnt++;
        }

        if (act_cont_info_templ != BCMLT_INVALID_HDL) {
            (void) bcmlt_entry_free(act_cont_info_templ);
            act_cont_info_templ = BCMLT_INVALID_HDL;
        }

        if (map->num_internal_maps > 0) {
            /* Retreive internal map first. */
            lt_map_int_ref = map->internal_map;
            if (lt_map_int_ref != NULL) {
                /* Retreive field definition for provided lt field in map */
                for (iter1 = 0;
                     iter1 < map->num_internal_maps;
                     iter1++) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_lt_entry_commit(
                             unit,
                             &int_act_cont_info_templ,
                             BCMLT_OPCODE_LOOKUP,
                             BCMLT_PRIORITY_NORMAL,
                             stage_info->tbls_info->action_cont_info_sid,
                             stage_info->tbls_info->action_cont_info_key_fid,
                             1, 0,
                             (char *)(lt_map_int_ref[iter1]).lt_field_name,
                             NULL, 0, 0, 0, 0));

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(int_act_cont_info_templ,
                                               "NUM_CONTAINERS",
                                               &num_cont));
                    for (cont = 0; cont < num_cont; cont++) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_array_get(
                                   int_act_cont_info_templ,
                                   "CONT_ID", cont,
                                   &cont_id, 1, &elem));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_array_get(
                                   int_act_cont_info_templ,
                                   "CONT_SIZE", cont,
                                   &cont_sz, 1, &elem));
                        if (cnt < max_ct) {
                            cont_id_arr[cnt] = cont_id;
                            cont_size_arr[cnt] = cont_sz;
                        }

                        if (action_size) {
                            *action_size += cont_sz;
                        }
                        cnt++;
                    }
                    if (int_act_cont_info_templ != BCMLT_INVALID_HDL) {
                        (void) bcmlt_entry_free(int_act_cont_info_templ);
                        int_act_cont_info_templ = BCMLT_INVALID_HDL;
                    }
                }
            }
        }
    }

    *actual_ct = cnt;

exit:
    if (act_cont_info_templ != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(act_cont_info_templ);
    }
    if (int_act_cont_info_templ != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(int_act_cont_info_templ);
    }
    SHR_FUNC_EXIT();
}

