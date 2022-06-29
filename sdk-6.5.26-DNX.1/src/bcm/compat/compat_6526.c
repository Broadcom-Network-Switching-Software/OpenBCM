/*
* $Id: compat_6526.c,v 1.0 2021/11/18
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.26 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6526.h>

/*
 * Function:
 *   _bcm_compat6526in_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from <=6.5.26 to
 *   SDK 6.5.26+
 * Parameters:
 *   from        - (IN) The <=6.5.26 version of the datatype
 *   to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_check_info_t(
        bcm_compat6526_flowtracker_check_info_t *from,
        bcm_flowtracker_check_info_t *to)
{
    bcm_flowtracker_check_info_t_init(to);
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->mask_value = from->mask_value;
    to->shift_value = from->shift_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
    to->state_transition_flags = from->state_transition_flags;
    to->num_drop_reason_group_id = from->num_drop_reason_group_id;

    sal_memcpy(&to->drop_reason_group_id[0], &from->drop_reason_group_id[0],
            sizeof (bcm_flowtracker_drop_reason_group_t) * \
            BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);

    to->custom_id = from->custom_id;
    to->stage = from->stage;
}

/*
 * Function:
 *   _bcm_compat6526out_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from 6.5.26+ to
 *   <=6.5.26
 * Parameters:
 *   from        - (IN) The 6.5.26+ version of the datatype
 *   to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_check_info_t(
        bcm_flowtracker_check_info_t *from,
        bcm_compat6526_flowtracker_check_info_t *to)
{
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->mask_value = from->mask_value;
    to->shift_value = from->shift_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
    to->state_transition_flags = from->state_transition_flags;
    to->num_drop_reason_group_id = from->num_drop_reason_group_id;

    sal_memcpy(&to->drop_reason_group_id[0], &from->drop_reason_group_id[0],
            sizeof (bcm_flowtracker_drop_reason_group_t) * \
            BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX);

    to->custom_id = from->custom_id;
    to->stage = from->stage;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_create
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   options    - (IN) Options to create check.
 *   check_info - (IN) check info.
 *   check_id   - (INOUT) Software id of check.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_create(
        int unit,
        uint32 options,
        bcm_compat6526_flowtracker_check_info_t check_info,
        bcm_flowtracker_check_t *check_id)
{
    int rv = 0;
    bcm_flowtracker_check_info_t new_check_info;

    _bcm_compat6526in_flowtracker_check_info_t(&check_info, &new_check_info);

    rv = bcm_flowtracker_check_create(unit, options, new_check_info, check_id);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_check_info_t(&new_check_info, &check_info);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_get
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   check_id   - (IN) Software id of check.
 *   check_info - (OUT) check info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6526_flowtracker_check_info_t *check_info)
{
    int rv = 0;
    bcm_flowtracker_check_info_t *new_check_info = NULL;

    if (check_info != NULL) {
        new_check_info = (bcm_flowtracker_check_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_check_info_t), "New check info");
        if (new_check_info == NULL) {
            return BCM_E_MEMORY;
        }
        bcm_flowtracker_check_info_t_init(new_check_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_check_get(unit, check_id, new_check_info);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_check_info_t(new_check_info, check_info);
    }

    sal_free(new_check_info);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_flowtracker_check_action_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_action_info_t datatype
 *      from <=6.5.26 to SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_check_action_info_t(
    bcm_compat6526_flowtracker_check_action_info_t *from,
    bcm_flowtracker_check_action_info_t *to)
{
    bcm_flowtracker_check_action_info_t_init(to);
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->action                   = from->action;
    to->mask_value               = from->mask_value;
    to->shift_value              = from->shift_value;
    to->weight                   = from->weight;
    to->direction                = from->direction;
    to->custom_id                = from->custom_id;
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_flowtracker_check_action_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_action_info_t datatype from
 *      6.5.26+ to SDK <=6.5.26
 * Parameters:
 *      from        - (IN) The 6.5.26+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_check_action_info_t(
    bcm_flowtracker_check_action_info_t *from,
    bcm_compat6526_flowtracker_check_action_info_t *to)
{
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->action                   = from->action;
    to->mask_value               = from->mask_value;
    to->shift_value              = from->shift_value;
    to->weight                   = from->weight;
    to->direction                = from->direction;
    to->custom_id                = from->custom_id;

    return;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_action_info_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (IN)    flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_action_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_action_info_t info)
{
    int rv;
    bcm_flowtracker_check_action_info_t info_new;

    /* Transform the entry from the old format to new one */
    _bcm_compat6526in_flowtracker_check_action_info_t(&info, &info_new);

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_action_info_set(unit, check_id, info_new);

    return rv;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_action_info_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (OUT)   flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_action_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_action_info_t *info)
{
    int rv;
    bcm_flowtracker_check_action_info_t *info_new = NULL;

    if (info != NULL) {
        info_new = sal_alloc(sizeof(bcm_flowtracker_check_action_info_t),
                "New check action info");
        if (info_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flowtracker_check_action_info_t(info, info_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_action_info_get(unit, check_id, info_new);

    if (info_new != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_flowtracker_check_action_info_t(info_new, info);

        sal_free(info_new);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_flowtracker_check_export_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_export_info_t datatype
 *      from <=6.5.26 to SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_check_export_info_t(
    bcm_compat6526_flowtracker_check_export_info_t *from,
    bcm_flowtracker_check_export_info_t *to)
{
    bcm_flowtracker_check_export_info_t_init(to);
    to->export_check_threshold = from->export_check_threshold;
    to->operation              = from->operation;
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_flowtracker_check_export_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_check_export_info_t datatype from
 *      6.5.26+ to SDK <=6.5.26
 * Parameters:
 *      from        - (IN) The 6.5.26+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_check_export_info_t(
    bcm_flowtracker_check_export_info_t *from,
    bcm_compat6526_flowtracker_check_export_info_t *to)
{
    to->export_check_threshold = from->export_check_threshold;
    to->operation              = from->operation;

    return;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_export_info_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_export_info_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (IN)    flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_export_info_set(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_export_info_t info)
{
    int rv;
    bcm_flowtracker_check_export_info_t info_new;

    /* Transform the entry from the old format to new one */
    _bcm_compat6526in_flowtracker_check_export_info_t(&info, &info_new);

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_export_info_set(unit, check_id, info_new);

    return rv;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_check_export_info_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_export_info_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   check_id          - (IN)    Flowtracker check id
 *   info              - (OUT)   flowtracker Check info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6526_flowtracker_check_export_info_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6526_flowtracker_check_export_info_t *info)
{
    int rv;
    bcm_flowtracker_check_export_info_t *info_new = NULL;

    if (info != NULL) {
        info_new = sal_alloc(sizeof(bcm_flowtracker_check_export_info_t),
                "New check export info");
        if (info_new == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flowtracker_check_export_info_t(info, info_new);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_check_export_info_get(unit, check_id, info_new);

    if (info_new != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_flowtracker_check_export_info_t(info_new, info);

        sal_free(info_new);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6526in_flowtracker_group_flow_action_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_group_flow_action_info_t datatype from <=6.5.26 to
 *   SDK 6.5.26+
 * Parameters:
 *   from        - (IN) The <=6.5.26 version of the datatype
 *   to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526in_flowtracker_group_flow_action_info_t(
        bcm_compat6526_flowtracker_group_flow_action_info_t *from,
        bcm_flowtracker_group_flow_action_info_t *to)
{
    sal_memset(to, 0, sizeof(*to));
    to->pipe_idx = from->pipe_idx;
    to->mode = from->mode;
    to->exact_match_idx = from->exact_match_idx;
}

/*
 * Function:
 *   _bcm_compat6526out_flowtracker_group_flow_action_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_group_flow_action_info_t datatype from 6.5.26+ to
 *   <=6.5.26
 * Parameters:
 *   from        - (IN) The 6.5.26+ version of the datatype
 *   to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6526out_flowtracker_group_flow_action_info_t(
        bcm_flowtracker_group_flow_action_info_t *from,
        bcm_compat6526_flowtracker_group_flow_action_info_t *to)
{
    to->pipe_idx = from->pipe_idx;
    to->mode = from->mode;
    to->exact_match_idx = from->exact_match_idx;
}

/*
 * Function:
 *   bcm_compat6526_flowtracker_group_flow_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_flow_delete
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for delete action.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_compat6526_flowtracker_group_flow_delete(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_compat6526_flowtracker_group_flow_action_info_t *action_info)
{
    int rv = 0;
    bcm_flowtracker_group_flow_action_info_t *new_action_info = NULL;

    if (action_info != NULL) {
        new_action_info = (bcm_flowtracker_group_flow_action_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_group_flow_action_info_t), "New action info");
        if (new_action_info == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6526in_flowtracker_group_flow_action_info_t(action_info, new_action_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_group_flow_delete(unit, flow_group_id, new_action_info);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_group_flow_action_info_t(new_action_info, action_info);
    }

    sal_free(new_action_info);
    return rv;
}

/*
 * Function:
 *      bcm_flowtracker_group_flow_action_update
 * Purpose:
 *      Update actions of a HW learnt flow.
 * Parameters:
 *      unit                    - (IN)  BCM device number
 *      flow_group_id           - (IN)  Flow group ID
 *      action_info             - (IN)  Information for update action.
 *      num_actions             - (IN)  Number of actions to update
 *      action_list             - (IN)  List of actions to update.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_compat6526_flowtracker_group_flow_action_update(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_compat6526_flowtracker_group_flow_action_info_t *action_info,
        int num_actions,
        bcm_flowtracker_group_action_info_t *action_list)
{
    int rv = 0;
    bcm_flowtracker_group_flow_action_info_t *new_action_info = NULL;

    if (action_info != NULL) {
        new_action_info = (bcm_flowtracker_group_flow_action_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_group_flow_action_info_t), "New action info");
        if (new_action_info == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6526in_flowtracker_group_flow_action_info_t(action_info, new_action_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_group_flow_action_update(unit, flow_group_id, new_action_info, num_actions, action_list);

    if (rv >= 0) {
        _bcm_compat6526out_flowtracker_group_flow_action_info_t(new_action_info, action_info);
    }

    sal_free(new_action_info);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from <=6.5.26 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l2_addr_t(
    bcm_compat6526_l2_addr_t *from,
    bcm_l2_addr_t *to)
{
    int i1 = 0;

    sal_memset(to, 0, sizeof(*to));
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac[i1] = from->mac[i1];
    }
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->learn_strength = from->learn_strength;
    to->age_profile = from->age_profile;
}

/*
 * Function:
 *      _bcm_compat6526out_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from SDK 6.5.26+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_l2_addr_t(
    bcm_l2_addr_t *from,
    bcm_compat6526_l2_addr_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac[i1] = from->mac[i1];
    }
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->learn_strength = from->learn_strength;
    to->age_profile = from->age_profile;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      l2addr - (IN) L2 address which is properly initialized
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_add(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_add(unit, new_l2addr);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac_addr - (IN) Input MAC address to search
 *      vid - (IN) Input VLAN ID to search
 *      l2addr - (OUT) Pointer to L2 address structure to receive results
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_get(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vid,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_get(unit, mac_addr, vid, new_l2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l2_addr_t(new_l2addr, l2addr);

    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_conflict_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_conflict_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      addr - (IN) L2 address to search for conflicts
 *      cf_array - (OUT) List of L2 addresses conflicting with addr
 *      cf_max - (IN) Number of entries allocated to cf_array
 *      cf_count - (OUT) Actual number of cf_array entries filled
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_conflict_get(
    int unit,
    bcm_compat6526_l2_addr_t *addr,
    bcm_compat6526_l2_addr_t *cf_array,
    int cf_max,
    int *cf_count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_addr = NULL;
    bcm_l2_addr_t *new_cf_array = NULL;
    int i = 0;

    if (addr != NULL && cf_array != NULL && cf_max > 0) {
        new_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New addr");
        if (new_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(addr, new_addr);
        new_cf_array = (bcm_l2_addr_t *)
                     sal_alloc(cf_max * sizeof(bcm_l2_addr_t),
                     "New cf_array");
        if (new_cf_array == NULL) {
            sal_free(new_addr);
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_conflict_get(unit, new_addr, new_cf_array, cf_max, cf_count);

    for (i = 0; i < cf_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l2_addr_t(&new_cf_array[i], &cf_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_addr);
    sal_free(new_cf_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to retrieve, as defined by bcm_l2_stat_t.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_get(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_get32
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_get32.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to retrieve, as defined by bcm_l2_stat_t.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_get32(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get32(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to set, as defined by bcm_l2_stat_t.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_set(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_set32
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_set32.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to set, as defined by bcm_l2_stat_t.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_set32(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set32(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_stat_enable_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_enable_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) pointer to an L2 address structure.
 *      enable - (IN) Zero to disable, anything else to enable.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_stat_enable_set(
    int unit,
    bcm_compat6526_l2_addr_t *l2_addr,
    int enable)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_enable_set(unit, new_l2_addr, enable);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_replace
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_replace.
 * Parameters:
 *      unit - (IN) BCM device number
 *      flags - (IN) Replace/delete flags BCM_L2_REPLACE_*
 *      match_addr - (IN) L2 parameters to match on delete/replace
 *      new_module - (IN) New module ID for a replace
 *      new_port - (IN) New port for a replace
 *      new_trunk - (IN) New trunk ID for a replace
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_replace(
    int unit,
    uint32 flags,
    bcm_compat6526_l2_addr_t *match_addr,
    bcm_module_t new_module,
    bcm_port_t new_port,
    bcm_trunk_t new_trunk)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_match_addr = NULL;

    if (match_addr != NULL) {
        new_match_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New match_addr");
        if (new_match_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(match_addr, new_match_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace(unit, flags, new_match_addr, new_module, new_port, new_trunk);


    /* Deallocate memory*/
    sal_free(new_match_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_replace_match
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_replace_match.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Replace/delete flags BCM_L2_REPLACE_*
 *      match_addr - (IN) L2 parameters to match on delete/replace
 *      mask_addr - (IN) L2 mask to match on delete/replace
 *      replace_addr - (IN) value to replace match entries (not relevant in case of delete)
 *      replace_mask_addr - (IN) sets which fields/bits from replace_addr are relevant for replace. Unmasked fields/bit will not be affected. (not relevant for delete)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_replace_match(
    int unit,
    uint32 flags,
    bcm_compat6526_l2_addr_t *match_addr,
    bcm_compat6526_l2_addr_t *mask_addr,
    bcm_compat6526_l2_addr_t *replace_addr,
    bcm_compat6526_l2_addr_t *replace_mask_addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_match_addr = NULL;
    bcm_l2_addr_t *new_mask_addr = NULL;
    bcm_l2_addr_t *new_replace_addr = NULL;
    bcm_l2_addr_t *new_replace_mask_addr = NULL;

    if (match_addr != NULL && mask_addr != NULL && replace_addr != NULL && replace_mask_addr != NULL) {
        new_match_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New match_addr");
        if (new_match_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(match_addr, new_match_addr);
        new_mask_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New mask_addr");
        if (new_mask_addr == NULL) {
            sal_free(new_match_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(mask_addr, new_mask_addr);
        new_replace_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New replace_addr");
        if (new_replace_addr == NULL) {
            sal_free(new_match_addr);
            sal_free(new_mask_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(replace_addr, new_replace_addr);
        new_replace_mask_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New replace_mask_addr");
        if (new_replace_mask_addr == NULL) {
            sal_free(new_match_addr);
            sal_free(new_mask_addr);
            sal_free(new_replace_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(replace_mask_addr, new_replace_mask_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace_match(unit, flags, new_match_addr, new_mask_addr, new_replace_addr, new_replace_mask_addr);


    /* Deallocate memory*/
    sal_free(new_match_addr);
    sal_free(new_mask_addr);
    sal_free(new_replace_addr);
    sal_free(new_replace_mask_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_multi_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_multi_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      l2addr - (IN) L2 address which is properly initialized
 *      count - (IN) L2 address count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_multi_add(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr,
    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_add(unit, new_l2addr, count);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_multi_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_multi_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (IN) Pointer to layer2 address type<br>L2 address which is properly initialized
 *      count - (IN) L2 address count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_multi_delete(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr,
    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_delete(unit, new_l2addr, count);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_by_struct_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_by_struct_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (INOUT) Pointer to layer2 address type
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_by_struct_get(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_get(unit, new_l2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_l2_addr_t(new_l2addr, l2addr);

    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l2_addr_by_struct_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_by_struct_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (IN) Pointer to layer2 address type
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_l2_addr_by_struct_delete(
    int unit,
    bcm_compat6526_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_delete(unit, new_l2addr);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6526in_l3_ipmc_t
 * Purpose:
 *      Convert the bcm_l3_ipmc_t datatype from <=6.5.26 to 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_ipmc_addr_t(
    bcm_compat6526_ipmc_addr_t *from,
    bcm_ipmc_addr_t *to)
{
    int i = 0;

    bcm_ipmc_addr_t_init(to);
    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(to->s_ip6_addr, from->s_ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->mc_ip6_addr, from->mc_ip6_addr, sizeof(bcm_ip6_t));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(to->mc_ip6_mask, from->mc_ip6_mask, sizeof(bcm_ip6_t));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->priority = from->priority;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_logical_fields = from->num_logical_fields;
    to->mtu = from->mtu;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6526out_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from 6.5.26+ to <=6.5.26
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_ipmc_addr_t(
    bcm_ipmc_addr_t *from,
    bcm_compat6526_ipmc_addr_t *to)
{
    int i = 0;

    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(to->s_ip6_addr, from->s_ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->mc_ip6_addr, from->mc_ip6_addr, sizeof(bcm_ip6_t));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(to->mc_ip6_mask, from->mc_ip6_mask, sizeof(bcm_ip6_t));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->priority = from->priority;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_logical_fields = from->num_logical_fields;
    to->mtu = from->mtu;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

int bcm_compat6526_ipmc_add(
    int unit,
    bcm_compat6526_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_add(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_find(
    int unit,
    bcm_compat6526_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_find(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_remove(
    int unit,
    bcm_compat6526_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_remove(unit, new_data);

    if (NULL != new_data) {
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_attach(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    uint32 stat_counter_id)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_attach(unit, new_data, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_detach(
    int unit,
    bcm_compat6526_ipmc_addr_t *info)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_detach(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_counter_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_get(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_counter_sync_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_sync_get(unit, new_data, stat, num_entries,
                                        counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_counter_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_set(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_get32(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_set(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_multi_set32(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_stat_id_get(
    int unit,
    bcm_compat6526_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_id_get(unit, new_data, stat, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_config_add(
    int unit,
    bcm_compat6526_ipmc_addr_t *config)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != config) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_add(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_config_find(
    int unit,
    bcm_compat6526_ipmc_addr_t *config)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != config) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_find(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6526_ipmc_config_remove(
    int unit,
    bcm_compat6526_ipmc_addr_t *config)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != config) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_ipmc_addr_t(config, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_config_remove(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_ipmc_addr_t(new_data, config);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from <=6.5.26 to 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l3_host_t(
    bcm_compat6526_l3_host_t *from,
    bcm_l3_host_t *to)
{
    int i = 0;

    bcm_l3_host_t_init(to);
    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_ip_addr = from->l3a_ip_addr;
    sal_memcpy(to->l3a_ip6_addr, from->l3a_ip6_addr, sizeof(bcm_ip6_t));
    to->l3a_pri = from->l3a_pri;
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_ipmc_ptr = from->l3a_ipmc_ptr;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->encap_id = from->encap_id;
    to->native_intf = from->native_intf;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        sal_memcpy(&to->logical_fields[i], &from->logical_fields[i], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
    to->l3a_opaque_ctrl_id = from->l3a_opaque_ctrl_id;
    to->l3a_traffic_class = from->l3a_traffic_class;
}

/*
 * Function:
 *      _bcm_compat6526out_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from 6.5.26+ to <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_l3_host_t(
    bcm_l3_host_t *from,
    bcm_compat6526_l3_host_t *to)
{
    int i = 0;

    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_ip_addr = from->l3a_ip_addr;
    sal_memcpy(to->l3a_ip6_addr, from->l3a_ip6_addr, sizeof(bcm_ip6_t));
    to->l3a_pri = from->l3a_pri;
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_ipmc_ptr = from->l3a_ipmc_ptr;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->encap_id = from->encap_id;
    to->native_intf = from->native_intf;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        sal_memcpy(&to->logical_fields[i], &from->logical_fields[i], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
    to->l3a_opaque_ctrl_id = from->l3a_opaque_ctrl_id;
    to->l3a_traffic_class = from->l3a_traffic_class;
}

int bcm_compat6526_l3_host_find(
    int unit,
    bcm_compat6526_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_find(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_add(
    int unit,
    bcm_compat6526_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_add(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_delete(
    int unit,
    bcm_compat6526_l3_host_t *ip_addr)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != ip_addr) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_delete_by_interface(
    int unit,
    bcm_compat6526_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_delete_all(
    int unit,
    bcm_compat6526_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_attach(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 stat_counter_id)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_detach(
    int unit,
    bcm_compat6526_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_flexctr_object_set(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 value)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_flexctr_object_set(unit, new_info, value);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_flexctr_object_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    uint32 *value)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_flexctr_object_get(unit, new_info, value);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_counter_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_get(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_sync_get(unit, new_info, stat, num_entries,
                                           counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_counter_set(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_set(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6526_l3_host_stat_id_get(
    int unit,
    bcm_compat6526_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from <= 6.5.26 to SDK 6.5.27+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.26 version of the datatype.
 *      to   - (OUT) SDK 6.5.27+ version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6526in_l3_egress_ecmp_t(bcm_compat6526_l3_egress_ecmp_t *from,
                                   bcm_l3_egress_ecmp_t *to)
{
    if (from && to) {
        bcm_l3_egress_ecmp_t_init(to);
        to->flags = from->flags;
        to->ecmp_intf = from->ecmp_intf;
        to->max_paths = from->max_paths;
        to->ecmp_group_flags = from->ecmp_group_flags;
        to->dynamic_mode = from->dynamic_mode;
        to->dynamic_size = from->dynamic_size;
        to->dynamic_load_exponent = from->dynamic_load_exponent;
        sal_memcpy(&to->dgm,
                   &from->dgm,
                   sizeof(bcm_l3_dgm_t));
        to->stat_id = from->stat_id;
        to->stat_pp_profile = from->stat_pp_profile;
        to->rpf_mode = from->rpf_mode;
        sal_memcpy(&to->tunnel_priority,
                   &from->tunnel_priority,
                   sizeof(bcm_l3_tunnel_priority_info_t));
        to->rh_random_seed          = from->rh_random_seed;
        to->user_profile            = from->user_profile;
        to->failover_id             = from->failover_id;
        to->failover_size           = from->failover_size;
        to->failover_base_egress_id = from->failover_base_egress_id;
        to->failover_lb_mode        = from->failover_lb_mode;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6526out_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from SDK 6.5.27+ to <=6.5.26.
 *
 * Parameters:
 *      from - (IN) SDK 6.5.27+ version of the datatype.
 *      to   - (OUT) <= SDK 6.5.26 version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6526out_l3_egress_ecmp_t(bcm_l3_egress_ecmp_t *from,
                                    bcm_compat6526_l3_egress_ecmp_t *to)
{
    if (from && to) {
        to->flags = from->flags;
        to->ecmp_intf = from->ecmp_intf;
        to->max_paths = from->max_paths;
        to->ecmp_group_flags = from->ecmp_group_flags;
        to->dynamic_mode = from->dynamic_mode;
        to->dynamic_size = from->dynamic_size;
        to->dynamic_load_exponent = from->dynamic_load_exponent;
        sal_memcpy(&to->dgm,
                   &from->dgm,
                   sizeof(bcm_l3_dgm_t));
        to->stat_id = from->stat_id;
        to->stat_pp_profile = from->stat_pp_profile;
        to->rpf_mode = from->rpf_mode;
        sal_memcpy(&to->tunnel_priority,
                   &from->tunnel_priority,
                   sizeof(bcm_l3_tunnel_priority_info_t));
        to->rh_random_seed          = from->rh_random_seed;
        to->user_profile            = from->user_profile;
        to->failover_id             = from->failover_id;
        to->failover_size           = from->failover_size;
        to->failover_base_egress_id = from->failover_base_egress_id;
        to->failover_lb_mode        = from->failover_lb_mode;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6526in_l3_egress_ecmp_resilient_entry_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_resilient_entry_t datatype from <= 6.5.26
 *      to SDK 6.5.27+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.26 version of the datatype.
 *      to   - (OUT) SDK 6.5.27+ version of the datatype.
 *
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *from,
    bcm_l3_egress_ecmp_resilient_entry_t *to)
{
    if (from && to) {
        to->hash_key = from->hash_key;
        (void)_bcm_compat6526in_l3_egress_ecmp_t(&from->ecmp, &to->ecmp);
        to->intf = from->intf;
    }
    return;
}

/*
 * Function:
 *      bcm_compat6526_l3_ecmp_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_create.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      options           - (IN) L3_ECMP_O_xxx flags.
 *      ecmp_info         - (INOUT) ECMP group info.
 *      ecmp_member_count - (IN) Number of elements in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_ecmp_create(int unit,
                              uint32 options,
                              bcm_compat6526_l3_egress_ecmp_t *ecmp_info,
                              int ecmp_member_count,
                              bcm_l3_ecmp_member_t *ecmp_member_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_create(unit, options, new, ecmp_member_count,
                            ecmp_member_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_ecmp_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_get.
 *
 * Parameters:
 *      unit              - (IN) Unit number.
 *      ecmp_info         - (INOUT) ECMP group info.
 *      ecmp_member_size  - (IN) Size of allocated entries in ecmp_member_array.
 *      ecmp_member_array - (OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count - (OUT) Number of entries of ecmp_member_count
 *                                actually filled in. This will be a value less
 *                                than or equal to the value passed in as
 *                                ecmp_member_size unless ecmp_member_size is 0.
 *                                If ecmp_member_size is 0 then
 *                                ecmp_member_array is ignored and
 *                                ecmp_member_count is filled in with the number
 *                                of entries that would have been filled into
 *                                ecmp_member_array if ecmp_member_size was
 *                                arbitrarily large.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_ecmp_get(int unit,
                           bcm_compat6526_l3_egress_ecmp_t *ecmp_info,
                           int ecmp_member_size,
                           bcm_l3_ecmp_member_t *ecmp_member_array,
                           int *ecmp_member_count)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_get(unit, new, ecmp_member_size, ecmp_member_array,
                         ecmp_member_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_ecmp_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_find.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      ecmp_member_count - (IN) Number of member in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of egress forwarding objects.
 *      ecmp_info         - (OUT) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_ecmp_find(int unit,
                            int ecmp_member_count,
                            bcm_l3_ecmp_member_t *ecmp_member_array,
                            bcm_compat6526_l3_egress_ecmp_t *ecmp_info)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_find(unit, ecmp_member_count, ecmp_member_array, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of OUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_create.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (INOUT) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_create(int unit,
                                     bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                     int intf_count,
                                     bcm_if_t *intf_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_create(unit, new, intf_count, intf_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_destroy
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_destroy.
 *
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (IN) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_destroy(int unit,
                                      bcm_compat6526_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_destroy(unit, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_get.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (INOUT) ECMP group info.
 *      intf_size  - (IN) Size of allocated entries in intf_array.
 *      intf_array - (OUT) Array of Egress forwarding objects.
 *      intf_count - (OUT) Number of entries of intf_count actually filled in.
 *                         This will be a value less than or equal to the value
 *                         passed in as intf_size unless intf_size is 0. If
 *                         intf_size is 0 then intf_array is ignored and
 *                         intf_count is filled in with the number of entries
 *                         that would have been filled into intf_array if
 *                         intf_size was arbitrarily large.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_get(int unit,
                                  bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                  int intf_size,
                                  bcm_if_t *intf_array,
                                  int *intf_count)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_get(unit, new, intf_size, intf_array, intf_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_add.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_add(int unit,
                                  bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                  bcm_if_t intf)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_add(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_delete.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_delete(int unit,
                                     bcm_compat6526_l3_egress_ecmp_t *ecmp,
                                     bcm_if_t intf)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_delete(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_find.
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      ecmp       - (OUT) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_find(int unit,
                                   int intf_count,
                                   bcm_if_t *intf_array,
                                   bcm_compat6526_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the structure members. */
    bcm_l3_egress_ecmp_t_init(new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_find(unit, intf_count, intf_array, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of OUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6526_l3_egress_ecmp_t
         */
        (void)_bcm_compat6526out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_tunnel_priority_set
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_tunnel_priority_set.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6526_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6526L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_tunnel_priority_set(unit, new, intf_count,
                                                intf_array);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_resilient_replace
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_resilient_replace.
 *
 * Parameters:
 *      unit          - (IN) Unit number.
 *      flags         - (IN) BCM_TRUNK_RESILIENT flags.
 *      match_entry   - (IN) Matching criteria
 *      num_entries   - (OUT) Number of entries matched.
 *      replace_entry - (IN) Replacing entry when the action is replace.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *replace_entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_match = NULL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_replace = NULL;

    /* Validate input parameters for NULL ptr. */
    if (match_entry == NULL || replace_entry == NULL) {
        return (BCM_E_PARAM);
    }

    new_match = (bcm_l3_egress_ecmp_resilient_entry_t *)
                    sal_alloc(sizeof(*new_match),
                              "bcmcompat6526L3EgrEcmpRhNewMatchEnt");
    if (new_match == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new_match, 0, sizeof(*new_match));
    new_replace = (bcm_l3_egress_ecmp_resilient_entry_t *)
                        sal_alloc(sizeof(*new_replace),
                                  "bcmcompat6526L3EgrEcmpRhNewReplaceEnt");
    if (new_replace == NULL) {
        sal_free(new_match);
        return (BCM_E_MEMORY);
    }
    sal_memset(new_replace, 0, sizeof(*new_replace));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(match_entry,
                                                             new_match);
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(replace_entry,
                                                             new_replace);
    rv = bcm_l3_egress_ecmp_resilient_replace(unit, flags, new_match,
                                              num_entries, new_replace);
    if (new_match) {
        sal_free(new_match);
    }
    if (new_replace) {
        sal_free(new_replace);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_resilient_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_add.
 *
 * Parameters:
 *      unit  - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6526L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_add(unit, flags, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6526_l3_egress_ecmp_resilient_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_resilient_delete.
 *
 * Parameters:
 *      unit  - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6526_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6526_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6526L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6526_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6526in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_match_config_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_match_config_t(
    bcm_compat6526_flow_match_config_t *from,
    bcm_flow_match_config_t *to)
{
    int i1 = 0;

    bcm_flow_match_config_t_init(to);
    to->vnid = from->vnid;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->port = from->port;
    to->sip = from->sip;
    to->sip_mask = from->sip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    to->dip = from->dip;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->protocol = from->protocol;
    to->mpls_label = from->mpls_label;
    to->flow_port = from->flow_port;
    to->vpn = from->vpn;
    to->intf_id = from->intf_id;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->flags = from->flags;
    to->default_vlan = from->default_vlan;
    to->default_vpn = from->default_vpn;
    to->class_id = from->class_id;
    to->next_hdr = from->next_hdr;
    to->tunnel_action = from->tunnel_action;
    to->etag_vlan = from->etag_vlan;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_match_config_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_match_config_t(
    bcm_flow_match_config_t *from,
    bcm_compat6526_flow_match_config_t *to)
{
    int i1 = 0;

    to->vnid = from->vnid;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->port = from->port;
    to->sip = from->sip;
    to->sip_mask = from->sip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    to->dip = from->dip;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->protocol = from->protocol;
    to->mpls_label = from->mpls_label;
    to->flow_port = from->flow_port;
    to->vpn = from->vpn;
    to->intf_id = from->intf_id;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->flags = from->flags;
    to->default_vlan = from->default_vlan;
    to->default_vpn = from->default_vpn;
    to->class_id = from->class_id;
    to->next_hdr = from->next_hdr;
    to->tunnel_action = from->tunnel_action;
    to->etag_vlan = from->etag_vlan;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_add(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_add(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_delete(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_delete(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) (IN/OUT) match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (INOUT) (IN/OUT) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_match_config_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_stat_attach(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_stat_detach(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_detach(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_match_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_match_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Match configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_match_stat_id_get(
    int unit,
    bcm_compat6526_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_match_config_t *)
                     sal_alloc(sizeof(bcm_flow_match_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_match_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6526in_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_tunnel_terminator_t(
    bcm_compat6526_flow_tunnel_terminator_t *from,
    bcm_flow_tunnel_terminator_t *to)
{
    int i1 = 0;

    bcm_flow_tunnel_terminator_t_init(to);
    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->vlan = from->vlan;
    to->protocol = from->protocol;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->vlan_mask = from->vlan_mask;
    to->class_id = from->class_id;
    to->term_pbmp = from->term_pbmp;
    to->next_hdr = from->next_hdr;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->vpn = from->vpn;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_tunnel_terminator_t(
    bcm_flow_tunnel_terminator_t *from,
    bcm_compat6526_flow_tunnel_terminator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->vlan = from->vlan;
    to->protocol = from->protocol;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->vlan_mask = from->vlan_mask;
    to->class_id = from->class_id;
    to->term_pbmp = from->term_pbmp;
    to->next_hdr = from->next_hdr;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->vpn = from->vpn;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) tunnel config info structure
 *      num_of_fields - (IN) Number of logical fields
 *      field - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_create(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_destroy
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_destroy.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) tunnel config info structure
 *      num_of_fields - (IN) number of logical fields
 *      field - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_destroy(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) tunnel config info structure
 *      num_of_fields - (IN) number of logical fields
 *      field - (IN) logical field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_stat_detach(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_tunnel_terminator_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6526_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
/*
 * Function:
 *      _bcm_compat6526in_cosq_mod_profile_t
 * Purpose:
 *      Convert the bcm_cosq_mod_profile_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_cosq_mod_profile_t(
    bcm_compat6526_cosq_mod_profile_t *from,
    bcm_cosq_mod_profile_t *to)
{
    bcm_cosq_mod_profile_t_init(to);
    to->percent_0_25 = from->percent_0_25;
    to->percent_25_50 = from->percent_25_50;
    to->percent_50_75 = from->percent_50_75;
    to->percent_75_100 = from->percent_75_100;
    to->enable = from->enable;
}

/*
 * Function:
 *      _bcm_compat6526out_cosq_mod_profile_t
 * Purpose:
 *      Convert the bcm_cosq_mod_profile_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_cosq_mod_profile_t(
    bcm_cosq_mod_profile_t *from,
    bcm_compat6526_cosq_mod_profile_t *to)
{
    to->percent_0_25 = from->percent_0_25;
    to->percent_25_50 = from->percent_25_50;
    to->percent_50_75 = from->percent_50_75;
    to->percent_75_100 = from->percent_75_100;
    to->enable = from->enable;
}

/*
 * Function:
 *      bcm_compat6526_cosq_mod_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_mod_profile_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      profile_id - (IN) Profile id
 *      profile - (IN) (IN/OUT) Mirror-on-drop profile pointer
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_cosq_mod_profile_set(
    int unit,
    int profile_id,
    bcm_compat6526_cosq_mod_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_mod_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_mod_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_mod_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_cosq_mod_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_mod_profile_set(unit, profile_id, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_cosq_mod_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_mod_profile_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      profile_id - (IN) Profile id
 *      profile - (OUT) (IN/OUT) Mirror-on-drop profile pointer
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_cosq_mod_profile_get(
    int unit,
    int profile_id,
    bcm_compat6526_cosq_mod_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_mod_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_mod_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_mod_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_mod_profile_get(unit, profile_id, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_cosq_mod_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_mirror_ingress_mod_event_profile_t
 * Purpose:
 *      Convert the bcm_mirror_ingress_mod_event_profile_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_mirror_ingress_mod_event_profile_t(
    bcm_compat6526_mirror_ingress_mod_event_profile_t *from,
    bcm_mirror_ingress_mod_event_profile_t *to)
{
    bcm_mirror_ingress_mod_event_profile_t_init(to);
    to->reason_code = from->reason_code;
    to->sample_rate = from->sample_rate;
    to->priority = from->priority;
}

/*
 * Function:
 *      _bcm_compat6526out_mirror_ingress_mod_event_profile_t
 * Purpose:
 *      Convert the bcm_mirror_ingress_mod_event_profile_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_mirror_ingress_mod_event_profile_t(
    bcm_mirror_ingress_mod_event_profile_t *from,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *to)
{
    to->reason_code = from->reason_code;
    to->sample_rate = from->sample_rate;
    to->priority = from->priority;
}

/*
 * Function:
 *      bcm_compat6526_mirror_ingress_mod_event_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_ingress_mod_event_profile_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      profile - (IN) Ingress mirror-on-drop event profile
 *      profile_id - (OUT) Profile id returned on success
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_mirror_ingress_mod_event_profile_create(
    int unit,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *profile,
    int *profile_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_ingress_mod_event_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_ingress_mod_event_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_ingress_mod_event_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_mirror_ingress_mod_event_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_ingress_mod_event_profile_create(unit, new_profile, profile_id);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_mirror_ingress_mod_event_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_ingress_mod_event_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      profile_id - (IN) Event profile id
 *      profile - (OUT) Event profile
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_mirror_ingress_mod_event_profile_get(
    int unit,
    int profile_id,
    bcm_compat6526_mirror_ingress_mod_event_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_ingress_mod_event_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_ingress_mod_event_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_ingress_mod_event_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_ingress_mod_event_profile_get(unit, profile_id, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_mirror_ingress_mod_event_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_rx_cosq_mapping_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_rx_cosq_mapping_t(
    bcm_compat6526_rx_cosq_mapping_t *from,
    bcm_rx_cosq_mapping_t *to)
{
    bcm_rx_cosq_mapping_t_init(to);
    to->flags = from->flags;
    to->index = from->index;
    to->reasons = from->reasons;
    to->reasons_mask = from->reasons_mask;
    to->int_prio = from->int_prio;
    to->int_prio_mask = from->int_prio_mask;
    to->packet_type = from->packet_type;
    to->packet_type_mask = from->packet_type_mask;
    to->cosq = from->cosq;
    to->flex_key1 = from->flex_key1;
    to->flex_key1_mask = from->flex_key1_mask;
    to->flex_key2 = from->flex_key2;
    to->flex_key2_mask = from->flex_key2_mask;
    sal_memcpy(&to->trace_reasons, &from->trace_reasons, sizeof(bcm_rx_pkt_trace_reasons_t));
    sal_memcpy(&to->trace_reasons_mask, &from->trace_reasons_mask, sizeof(bcm_rx_pkt_trace_reasons_t));
    to->drop_event = from->drop_event;
    to->drop_event_mask = from->drop_event_mask;
    to->priority = from->priority;
}

/*
 * Function:
 *      _bcm_compat6526out_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_rx_cosq_mapping_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_rx_cosq_mapping_t(
    bcm_rx_cosq_mapping_t *from,
    bcm_compat6526_rx_cosq_mapping_t *to)
{
    to->flags = from->flags;
    to->index = from->index;
    to->reasons = from->reasons;
    to->reasons_mask = from->reasons_mask;
    to->int_prio = from->int_prio;
    to->int_prio_mask = from->int_prio_mask;
    to->packet_type = from->packet_type;
    to->packet_type_mask = from->packet_type_mask;
    to->cosq = from->cosq;
    to->flex_key1 = from->flex_key1;
    to->flex_key1_mask = from->flex_key1_mask;
    to->flex_key2 = from->flex_key2;
    to->flex_key2_mask = from->flex_key2_mask;
    sal_memcpy(&to->trace_reasons, &from->trace_reasons, sizeof(bcm_rx_pkt_trace_reasons_t));
    sal_memcpy(&to->trace_reasons_mask, &from->trace_reasons_mask, sizeof(bcm_rx_pkt_trace_reasons_t));
    to->drop_event = from->drop_event;
    to->drop_event_mask = from->drop_event_mask;
    to->priority = from->priority;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) 
 *      rx_cosq_mapping - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_set(
    int unit,
    uint32 options,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_set(unit, options, new_rx_cosq_mapping);


    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      rx_cosq_mapping - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_get(
    int unit,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_get(unit, new_rx_cosq_mapping);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_rx_cosq_mapping_t(new_rx_cosq_mapping, rx_cosq_mapping);

    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) 
 *      rx_cosq_mapping - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_add(
    int unit,
    uint32 options,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_add(unit, options, new_rx_cosq_mapping);


    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_rx_cosq_mapping_extended_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      rx_cosq_mapping - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_rx_cosq_mapping_extended_delete(
    int unit,
    bcm_compat6526_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv = BCM_E_NONE;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (rx_cosq_mapping != NULL) {
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
                     sal_alloc(sizeof(bcm_rx_cosq_mapping_t),
                     "New rx_cosq_mapping");
        if (new_rx_cosq_mapping == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_delete(unit, new_rx_cosq_mapping);


    /* Deallocate memory*/
    sal_free(new_rx_cosq_mapping);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_vlan_port_t(
    bcm_compat6526_vlan_port_t *from,
    bcm_vlan_port_t *to)
{
    bcm_vlan_port_t_init(to);
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->flags2 = from->flags2;
    to->learn_strength = from->learn_strength;
}

/*
 * Function:
 *      _bcm_compat6526out_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_vlan_port_t(
    bcm_vlan_port_t *from,
    bcm_compat6526_vlan_port_t *to)
{
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->flags2 = from->flags2;
    to->learn_strength = from->learn_strength;
}

/*
 * Function:
 *      bcm_compat6526_vlan_port_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 Logical port.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_port_create(
    int unit,
    bcm_compat6526_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_create(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_vlan_port_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 logical port
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_vlan_port_find(
    int unit,
    bcm_compat6526_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_find(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6526in_flow_port_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_port_t(
    bcm_compat6526_flow_port_t *from,
    bcm_flow_port_t *to)
{
    bcm_flow_port_t_init(to);
    to->flow_port_id = from->flow_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->network_group_id = from->network_group_id;
    to->egress_service_tpid = from->egress_service_tpid;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->drop_mode = from->drop_mode;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_port_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_port_t(
    bcm_flow_port_t *from,
    bcm_compat6526_flow_port_t *to)
{
    to->flow_port_id = from->flow_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->network_group_id = from->network_group_id;
    to->egress_service_tpid = from->egress_service_tpid;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->drop_mode = from->drop_mode;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
}


#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6526in_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_flow_encap_config_t(
    bcm_compat6526_flow_encap_config_t *from,
    bcm_flow_encap_config_t *to)
{
    bcm_flow_encap_config_t_init(to);
    to->vpn = from->vpn;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->dvp_group = from->dvp_group;
    to->oif_group = from->oif_group;
    to->vnid = from->vnid;
    to->pri = from->pri;
    to->cfi = from->cfi;
    to->tpid = from->tpid;
    to->vlan = from->vlan;
    to->flags = from->flags;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->src_flow_port = from->src_flow_port;
    to->class_id = from->class_id;
    to->port_group = from->port_group;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->function = from->function;
}

/*
 * Function:
 *      _bcm_compat6526out_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_flow_encap_config_t(
    bcm_flow_encap_config_t *from,
    bcm_compat6526_flow_encap_config_t *to)
{
    to->vpn = from->vpn;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->dvp_group = from->dvp_group;
    to->oif_group = from->oif_group;
    to->vnid = from->vnid;
    to->pri = from->pri;
    to->cfi = from->cfi;
    to->tpid = from->tpid;
    to->vlan = from->vlan;
    to->flags = from->flags;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->src_flow_port = from->src_flow_port;
    to->class_id = from->class_id;
    to->port_group = from->port_group;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->function = from->function;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) (IN/OUT) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_add(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_add(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) Flow encap configuration parameters to specify the key
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_delete(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_delete(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (INOUT) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_encap_config_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_flexctr_object_set(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_flexctr_object_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_stat_attach(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_stat_detach(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_detach(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_encap_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_encap_stat_id_get(
    int unit,
    bcm_compat6526_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_port_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vpn - (IN) BCM default vpn
 *      flow_port - (INOUT) (IN/OUT) FLOW port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_port_create(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6526_flow_port_t *flow_port)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_t *new_flow_port = NULL;

    if (flow_port != NULL) {
        new_flow_port = (bcm_flow_port_t *)
                     sal_alloc(sizeof(bcm_flow_port_t),
                     "New flow_port");
        if (new_flow_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_port_t(flow_port, new_flow_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_create(unit, vpn, new_flow_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_port_t(new_flow_port, flow_port);

    /* Deallocate memory*/
    sal_free(new_flow_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) Vpn Instance
 *      flow_port - (INOUT) (IN/OUT) FLOW port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6526_flow_port_t *flow_port)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_t *new_flow_port = NULL;

    if (flow_port != NULL) {
        new_flow_port = (bcm_flow_port_t *)
                     sal_alloc(sizeof(bcm_flow_port_t),
                     "New flow_port");
        if (new_flow_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_flow_port_t(flow_port, new_flow_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_get(unit, vpn, new_flow_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_flow_port_t(new_flow_port, flow_port);

    /* Deallocate memory*/
    sal_free(new_flow_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_flow_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN Instance
 *      port_max - (IN) Maximum number of FLOW ports in array
 *      flow_port - (OUT) 
 *      port_count - (OUT) Number of FLOW ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_flow_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6526_flow_port_t *flow_port,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_t *new_flow_port = NULL;
    int i = 0;

    if (flow_port != NULL && port_max > 0) {
        new_flow_port = (bcm_flow_port_t *)
                     sal_alloc(port_max * sizeof(bcm_flow_port_t),
                     "New flow_port");
        if (new_flow_port == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_get_all(unit, vpn, port_max, new_flow_port, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6526out_flow_port_t(&new_flow_port[i], &flow_port[i]);
    }

    /* Deallocate memory*/
    sal_free(new_flow_port);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_BFD)
/*
 * Function:
 *      _bcm_compat6526in_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from <=6.5.26 to
 *      SDK 6.5.27+
 * Parameters:
 *      from        - (IN) The <=6.5.26 version of the datatype
 *      to          - (OUT) The SDK 6.5.27+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526in_bfd_endpoint_info_t(
    bcm_compat6526_bfd_endpoint_info_t *from,
    bcm_bfd_endpoint_info_t *to)
{
    int i1 = 0;

    bcm_bfd_endpoint_info_t_init(to);
    to->flags = from->flags;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->type = from->type;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->remote_gport = from->remote_gport;
    to->bfd_period = from->bfd_period;
    to->vpn = from->vpn;
    to->vlan_pri = from->vlan_pri;
    to->inner_vlan_pri = from->inner_vlan_pri;
    to->vrf_id = from->vrf_id;
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac[i1] = from->dst_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->pkt_inner_vlan_id = from->pkt_inner_vlan_id;
    to->dst_ip_addr = from->dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->dst_ip6_addr[i1] = from->dst_ip6_addr[i1];
    }
    to->src_ip_addr = from->src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src_ip6_addr[i1] = from->src_ip6_addr[i1];
    }
    to->ip_tos = from->ip_tos;
    to->ip_ttl = from->ip_ttl;
    to->inner_dst_ip_addr = from->inner_dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_dst_ip6_addr[i1] = from->inner_dst_ip6_addr[i1];
    }
    to->inner_src_ip_addr = from->inner_src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_src_ip6_addr[i1] = from->inner_src_ip6_addr[i1];
    }
    to->inner_ip_tos = from->inner_ip_tos;
    to->inner_ip_ttl = from->inner_ip_ttl;
    to->udp_src_port = from->udp_src_port;
    to->label = from->label;
    to->mpls_hdr = from->mpls_hdr;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    to->egress_if = from->egress_if;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mep_id[i1] = from->mep_id[i1];
    }
    to->mep_id_length = from->mep_id_length;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    to->local_state = from->local_state;
    to->local_discr = from->local_discr;
    to->local_diag = from->local_diag;
    to->local_flags = from->local_flags;
    to->local_min_tx = from->local_min_tx;
    to->local_min_rx = from->local_min_rx;
    to->local_min_echo = from->local_min_echo;
    to->local_detect_mult = from->local_detect_mult;
    to->auth = from->auth;
    to->auth_index = from->auth_index;
    to->tx_auth_seq = from->tx_auth_seq;
    to->rx_auth_seq = from->rx_auth_seq;
    to->remote_flags = from->remote_flags;
    to->remote_state = from->remote_state;
    to->remote_discr = from->remote_discr;
    to->remote_diag = from->remote_diag;
    to->remote_min_tx = from->remote_min_tx;
    to->remote_min_rx = from->remote_min_rx;
    to->remote_min_echo = from->remote_min_echo;
    to->remote_detect_mult = from->remote_detect_mult;
    to->sampling_ratio = from->sampling_ratio;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->ip_subnet_length = from->ip_subnet_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->remote_mep_id[i1] = from->remote_mep_id[i1];
    }
    to->remote_mep_id_length = from->remote_mep_id_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mis_conn_mep_id[i1] = from->mis_conn_mep_id[i1];
    }
    to->mis_conn_mep_id_length = from->mis_conn_mep_id_length;
    to->bfd_detection_time = from->bfd_detection_time;
    to->pkt_vlan_id = from->pkt_vlan_id;
    to->rx_pkt_vlan_id = from->rx_pkt_vlan_id;
    to->gal_label = from->gal_label;
    to->faults = from->faults;
    to->flags2 = from->flags2;
    to->ipv6_extra_data_index = from->ipv6_extra_data_index;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->bfd_period_cluster = from->bfd_period_cluster;
    to->vxlan_vnid = from->vxlan_vnid;
    to->vlan_tpid = from->vlan_tpid;
    to->inner_vlan_tpid = from->inner_vlan_tpid;
    to->injected_network_qos = from->injected_network_qos;
}

/*
 * Function:
 *      _bcm_compat6526out_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from SDK 6.5.27+ to
 *      <=6.5.26
 * Parameters:
 *      from        - (IN) The SDK 6.5.27+ version of the datatype
 *      to          - (OUT) The <=6.5.26 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6526out_bfd_endpoint_info_t(
    bcm_bfd_endpoint_info_t *from,
    bcm_compat6526_bfd_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->type = from->type;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->remote_gport = from->remote_gport;
    to->bfd_period = from->bfd_period;
    to->vpn = from->vpn;
    to->vlan_pri = from->vlan_pri;
    to->inner_vlan_pri = from->inner_vlan_pri;
    to->vrf_id = from->vrf_id;
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac[i1] = from->dst_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->pkt_inner_vlan_id = from->pkt_inner_vlan_id;
    to->dst_ip_addr = from->dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->dst_ip6_addr[i1] = from->dst_ip6_addr[i1];
    }
    to->src_ip_addr = from->src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src_ip6_addr[i1] = from->src_ip6_addr[i1];
    }
    to->ip_tos = from->ip_tos;
    to->ip_ttl = from->ip_ttl;
    to->inner_dst_ip_addr = from->inner_dst_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_dst_ip6_addr[i1] = from->inner_dst_ip6_addr[i1];
    }
    to->inner_src_ip_addr = from->inner_src_ip_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->inner_src_ip6_addr[i1] = from->inner_src_ip6_addr[i1];
    }
    to->inner_ip_tos = from->inner_ip_tos;
    to->inner_ip_ttl = from->inner_ip_ttl;
    to->udp_src_port = from->udp_src_port;
    to->label = from->label;
    to->mpls_hdr = from->mpls_hdr;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    to->egress_if = from->egress_if;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mep_id[i1] = from->mep_id[i1];
    }
    to->mep_id_length = from->mep_id_length;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    to->local_state = from->local_state;
    to->local_discr = from->local_discr;
    to->local_diag = from->local_diag;
    to->local_flags = from->local_flags;
    to->local_min_tx = from->local_min_tx;
    to->local_min_rx = from->local_min_rx;
    to->local_min_echo = from->local_min_echo;
    to->local_detect_mult = from->local_detect_mult;
    to->auth = from->auth;
    to->auth_index = from->auth_index;
    to->tx_auth_seq = from->tx_auth_seq;
    to->rx_auth_seq = from->rx_auth_seq;
    to->remote_flags = from->remote_flags;
    to->remote_state = from->remote_state;
    to->remote_discr = from->remote_discr;
    to->remote_diag = from->remote_diag;
    to->remote_min_tx = from->remote_min_tx;
    to->remote_min_rx = from->remote_min_rx;
    to->remote_min_echo = from->remote_min_echo;
    to->remote_detect_mult = from->remote_detect_mult;
    to->sampling_ratio = from->sampling_ratio;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->ip_subnet_length = from->ip_subnet_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->remote_mep_id[i1] = from->remote_mep_id[i1];
    }
    to->remote_mep_id_length = from->remote_mep_id_length;
    for (i1 = 0; i1 < BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i1++) {
        to->mis_conn_mep_id[i1] = from->mis_conn_mep_id[i1];
    }
    to->mis_conn_mep_id_length = from->mis_conn_mep_id_length;
    to->bfd_detection_time = from->bfd_detection_time;
    to->pkt_vlan_id = from->pkt_vlan_id;
    to->rx_pkt_vlan_id = from->rx_pkt_vlan_id;
    to->gal_label = from->gal_label;
    to->faults = from->faults;
    to->flags2 = from->flags2;
    to->ipv6_extra_data_index = from->ipv6_extra_data_index;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->bfd_period_cluster = from->bfd_period_cluster;
    to->vxlan_vnid = from->vxlan_vnid;
    to->vlan_tpid = from->vlan_tpid;
    to->inner_vlan_tpid = from->inner_vlan_tpid;
    to->injected_network_qos = from->injected_network_qos;
}

/*
 * Function:
 *      bcm_compat6526_bfd_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (INOUT) Pointer to an BFD endpoint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_bfd_endpoint_create(
    int unit,
    bcm_compat6526_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_bfd_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_bfd_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6526in_bfd_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6526_bfd_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure to receive the data.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6526_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6526_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_bfd_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_bfd_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_get(unit, endpoint, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6526out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}


#endif /* defined(INCLUDE_BFD) */
#endif /* BCM_RPC_SUPPORT */
