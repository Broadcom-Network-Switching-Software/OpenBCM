/*
* $Id: compat_6520.c,v 1.0 2020/07/07
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.20 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6520.h>


/*
 * Function:
 *      _bcm_compat6520in_mpls_egress_label_t
 * Purpose:
 *      Convert the bcm_mpls_egress_label_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_mpls_egress_label_t(
    bcm_compat6520_mpls_egress_label_t *from,
    bcm_mpls_egress_label_t *to)
{
    bcm_mpls_egress_label_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->qos_map_id = from->qos_map_id;
    to->exp = from->exp;
    to->ttl = from->ttl;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->tunnel_id = from->tunnel_id;
    to->l3_intf_id = from->l3_intf_id;
    to->action = from->action;
    to->ecn_map_id = from->ecn_map_id;
    to->int_cn_map_id = from->int_cn_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_if_id = from->egress_failover_if_id;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->spl_label_push_type = from->spl_label_push_type;
    to->encap_access = from->encap_access;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
}

/*
 * Function:
 *      _bcm_compat6520out_mpls_egress_label_t
 * Purpose:
 *      Convert the bcm_mpls_egress_label_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_mpls_egress_label_t(
    bcm_mpls_egress_label_t *from,
    bcm_compat6520_mpls_egress_label_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->qos_map_id = from->qos_map_id;
    to->exp = from->exp;
    to->ttl = from->ttl;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->tunnel_id = from->tunnel_id;
    to->l3_intf_id = from->l3_intf_id;
    to->action = from->action;
    to->ecn_map_id = from->ecn_map_id;
    to->int_cn_map_id = from->int_cn_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_if_id = from->egress_failover_if_id;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->spl_label_push_type = from->spl_label_push_type;
    to->encap_access = from->encap_access;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
}


/*
 * Function:
 *      _bcm_compat6520in_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_oam_endpoint_info_t(
    bcm_compat6520_oam_endpoint_info_t *from,
    bcm_oam_endpoint_info_t *to)
{
    int i1 = 0;

    bcm_oam_endpoint_info_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->opcode_flags = from->opcode_flags;
    to->lm_flags = from->lm_flags;
    to->id = from->id;
    to->type = from->type;
    to->group = from->group;
    to->name = from->name;
    to->local_id = from->local_id;
    to->level = from->level;
    to->ccm_period = from->ccm_period;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->trunk_index = from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    _bcm_compat6520in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    sal_memcpy(&to->mpls_network_info, &from->mpls_network_info, sizeof(bcm_oam_mpls_network_info_t));
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac_address[i1] = from->dst_mac_address[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_address[i1] = from->src_mac_address[i1];
    }
    to->pkt_pri = from->pkt_pri;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid = from->outer_tpid;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    for (i1 = 0; i1 < BCM_OAM_INTPRI_MAX; i1++) {
        to->pri_map[i1] = from->pri_map[i1];
    }
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->ing_map = from->ing_map;
    to->egr_map = from->egr_map;
    to->ms_pw_ttl = from->ms_pw_ttl;
    to->port_state = from->port_state;
    to->interface_state = from->interface_state;
    to->vccv_type = from->vccv_type;
    to->vpn = from->vpn;
    to->lm_counter_base_id = from->lm_counter_base_id;
    to->lm_counter_if = from->lm_counter_if;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->timestamp_format = from->timestamp_format;
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset = from->lm_cos_offset;
    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id = from->pri_map_id;
    to->lm_ctr_pool_id = from->lm_ctr_pool_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_base_id[i1] = from->ccm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_offset[i1] = from->ccm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_action[i1] = from->ccm_tx_update_lm_counter_action[i1];
    }
    to->ccm_tx_update_lm_counter_size = from->ccm_tx_update_lm_counter_size;
    to->session_id = from->session_id;
    to->session_num_entries = from->session_num_entries;
    to->lm_count_profile = from->lm_count_profile;
    to->mpls_exp = from->mpls_exp;
    to->action_reference_id = from->action_reference_id;
    to->acc_profile_id = from->acc_profile_id;
    to->endpoint_memory_type = from->endpoint_memory_type;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->extra_data_index = from->extra_data_index;
    to->rx_signal = from->rx_signal;
    to->tx_signal = from->tx_signal;
}

/*
 * Function:
 *      _bcm_compat6520out_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_oam_endpoint_info_t(
    bcm_oam_endpoint_info_t *from,
    bcm_compat6520_oam_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->opcode_flags = from->opcode_flags;
    to->lm_flags = from->lm_flags;
    to->id = from->id;
    to->type = from->type;
    to->group = from->group;
    to->name = from->name;
    to->local_id = from->local_id;
    to->level = from->level;
    to->ccm_period = from->ccm_period;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->trunk_index = from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    _bcm_compat6520out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    sal_memcpy(&to->mpls_network_info, &from->mpls_network_info, sizeof(bcm_oam_mpls_network_info_t));
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac_address[i1] = from->dst_mac_address[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_address[i1] = from->src_mac_address[i1];
    }
    to->pkt_pri = from->pkt_pri;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid = from->outer_tpid;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    for (i1 = 0; i1 < BCM_OAM_INTPRI_MAX; i1++) {
        to->pri_map[i1] = from->pri_map[i1];
    }
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->ing_map = from->ing_map;
    to->egr_map = from->egr_map;
    to->ms_pw_ttl = from->ms_pw_ttl;
    to->port_state = from->port_state;
    to->interface_state = from->interface_state;
    to->vccv_type = from->vccv_type;
    to->vpn = from->vpn;
    to->lm_counter_base_id = from->lm_counter_base_id;
    to->lm_counter_if = from->lm_counter_if;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->timestamp_format = from->timestamp_format;
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset = from->lm_cos_offset;
    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id = from->pri_map_id;
    to->lm_ctr_pool_id = from->lm_ctr_pool_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_base_id[i1] = from->ccm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_offset[i1] = from->ccm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_action[i1] = from->ccm_tx_update_lm_counter_action[i1];
    }
    to->ccm_tx_update_lm_counter_size = from->ccm_tx_update_lm_counter_size;
    to->session_id = from->session_id;
    to->session_num_entries = from->session_num_entries;
    to->lm_count_profile = from->lm_count_profile;
    to->mpls_exp = from->mpls_exp;
    to->action_reference_id = from->action_reference_id;
    to->acc_profile_id = from->acc_profile_id;
    to->endpoint_memory_type = from->endpoint_memory_type;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->extra_data_index = from->extra_data_index;
    to->rx_signal = from->rx_signal;
    to->tx_signal = from->tx_signal;
}


/*
 * Function:
 *      bcm_compat6520_oam_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      endpoint_info - (INOUT) (IN/OUT) Pointer to an OAM endpoint info structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_oam_endpoint_create(
    int unit,
    bcm_compat6520_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_oam_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_oam_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_oam_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_oam_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_oam_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      endpoint - (IN) ID of the endpoint to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint info structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6520_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_oam_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_oam_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_endpoint_get(unit, endpoint, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_oam_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6520in_srv6_srh_base_initiator_info_t
 * Purpose:
 *      Convert the bcm_srv6_srh_base_initiator_info_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_srv6_srh_base_initiator_info_t(
    bcm_compat6520_srv6_srh_base_initiator_info_t *from,
    bcm_srv6_srh_base_initiator_info_t *to)
{
    bcm_srv6_srh_base_initiator_info_t_init(to);
    to->flags = from->flags;
    to->tunnel_id = from->tunnel_id;
    to->nof_sids = from->nof_sids;
    to->qos_map_id = from->qos_map_id;
    to->ttl = from->ttl;
    to->dscp = from->dscp;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->next_encap_id = from->next_encap_id;
    to->nof_additional_sids_extended_encap = from->nof_additional_sids_extended_encap;
}

/*
 * Function:
 *      _bcm_compat6520out_srv6_srh_base_initiator_info_t
 * Purpose:
 *      Convert the bcm_srv6_srh_base_initiator_info_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_srv6_srh_base_initiator_info_t(
    bcm_srv6_srh_base_initiator_info_t *from,
    bcm_compat6520_srv6_srh_base_initiator_info_t *to)
{
    to->flags = from->flags;
    to->tunnel_id = from->tunnel_id;
    to->nof_sids = from->nof_sids;
    to->qos_map_id = from->qos_map_id;
    to->ttl = from->ttl;
    to->dscp = from->dscp;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->next_encap_id = from->next_encap_id;
    to->nof_additional_sids_extended_encap = from->nof_additional_sids_extended_encap;
}


/*
 * Function:
 *      bcm_compat6520_srv6_srh_base_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_srv6_srh_base_initiator_create(
    int unit,
    bcm_compat6520_srv6_srh_base_initiator_info_t *info)
{
    int rv = BCM_E_NONE;
    bcm_srv6_srh_base_initiator_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_srv6_srh_base_initiator_info_t *)
                     sal_alloc(sizeof(bcm_srv6_srh_base_initiator_info_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_srv6_srh_base_initiator_info_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_srv6_srh_base_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_srv6_srh_base_initiator_get(
    int unit,
    bcm_compat6520_srv6_srh_base_initiator_info_t *info)
{
    int rv = BCM_E_NONE;
    bcm_srv6_srh_base_initiator_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_srv6_srh_base_initiator_info_t *)
                     sal_alloc(sizeof(bcm_srv6_srh_base_initiator_info_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_srv6_srh_base_initiator_info_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_srv6_srh_base_initiator_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_srv6_srh_base_initiator_delete(
    int unit,
    bcm_compat6520_srv6_srh_base_initiator_info_t *info)
{
    int rv = BCM_E_NONE;
    bcm_srv6_srh_base_initiator_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_srv6_srh_base_initiator_info_t *)
                     sal_alloc(sizeof(bcm_srv6_srh_base_initiator_info_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_delete(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_vlan_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_action_set_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_vlan_action_set_t(
    bcm_compat6520_vlan_action_set_t *from,
    bcm_vlan_action_set_t *to)
{
    bcm_vlan_action_set_t_init(to);
    to->new_outer_vlan = from->new_outer_vlan;
    to->new_inner_vlan = from->new_inner_vlan;
    to->new_inner_pkt_prio = from->new_inner_pkt_prio;
    to->new_outer_cfi = from->new_outer_cfi;
    to->new_inner_cfi = from->new_inner_cfi;
    to->ingress_if = from->ingress_if;
    to->priority = from->priority;
    to->dt_outer = from->dt_outer;
    to->dt_outer_prio = from->dt_outer_prio;
    to->dt_outer_pkt_prio = from->dt_outer_pkt_prio;
    to->dt_outer_cfi = from->dt_outer_cfi;
    to->dt_inner = from->dt_inner;
    to->dt_inner_prio = from->dt_inner_prio;
    to->dt_inner_pkt_prio = from->dt_inner_pkt_prio;
    to->dt_inner_cfi = from->dt_inner_cfi;
    to->ot_outer = from->ot_outer;
    to->ot_outer_prio = from->ot_outer_prio;
    to->ot_outer_pkt_prio = from->ot_outer_pkt_prio;
    to->ot_outer_cfi = from->ot_outer_cfi;
    to->ot_inner = from->ot_inner;
    to->ot_inner_pkt_prio = from->ot_inner_pkt_prio;
    to->ot_inner_cfi = from->ot_inner_cfi;
    to->it_outer = from->it_outer;
    to->it_outer_pkt_prio = from->it_outer_pkt_prio;
    to->it_outer_cfi = from->it_outer_cfi;
    to->it_inner = from->it_inner;
    to->it_inner_prio = from->it_inner_prio;
    to->it_inner_pkt_prio = from->it_inner_pkt_prio;
    to->it_inner_cfi = from->it_inner_cfi;
    to->ut_outer = from->ut_outer;
    to->ut_outer_pkt_prio = from->ut_outer_pkt_prio;
    to->ut_outer_cfi = from->ut_outer_cfi;
    to->ut_inner = from->ut_inner;
    to->ut_inner_pkt_prio = from->ut_inner_pkt_prio;
    to->ut_inner_cfi = from->ut_inner_cfi;
    to->outer_pcp = from->outer_pcp;
    to->inner_pcp = from->inner_pcp;
    to->policer_id = from->policer_id;
    to->outer_tpid = from->outer_tpid;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid_action = from->outer_tpid_action;
    to->inner_tpid_action = from->inner_tpid_action;
    to->action_id = from->action_id;
    to->class_id = from->class_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->flags = from->flags;
    to->outer_tag = from->outer_tag;
    to->inner_tag = from->inner_tag;
    to->forwarding_domain = from->forwarding_domain;
    to->inner_qos_map_id = from->inner_qos_map_id;
    to->outer_qos_map_id = from->outer_qos_map_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6520out_vlan_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_action_set_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_vlan_action_set_t(
    bcm_vlan_action_set_t *from,
    bcm_compat6520_vlan_action_set_t *to)
{
    to->new_outer_vlan = from->new_outer_vlan;
    to->new_inner_vlan = from->new_inner_vlan;
    to->new_inner_pkt_prio = from->new_inner_pkt_prio;
    to->new_outer_cfi = from->new_outer_cfi;
    to->new_inner_cfi = from->new_inner_cfi;
    to->ingress_if = from->ingress_if;
    to->priority = from->priority;
    to->dt_outer = from->dt_outer;
    to->dt_outer_prio = from->dt_outer_prio;
    to->dt_outer_pkt_prio = from->dt_outer_pkt_prio;
    to->dt_outer_cfi = from->dt_outer_cfi;
    to->dt_inner = from->dt_inner;
    to->dt_inner_prio = from->dt_inner_prio;
    to->dt_inner_pkt_prio = from->dt_inner_pkt_prio;
    to->dt_inner_cfi = from->dt_inner_cfi;
    to->ot_outer = from->ot_outer;
    to->ot_outer_prio = from->ot_outer_prio;
    to->ot_outer_pkt_prio = from->ot_outer_pkt_prio;
    to->ot_outer_cfi = from->ot_outer_cfi;
    to->ot_inner = from->ot_inner;
    to->ot_inner_pkt_prio = from->ot_inner_pkt_prio;
    to->ot_inner_cfi = from->ot_inner_cfi;
    to->it_outer = from->it_outer;
    to->it_outer_pkt_prio = from->it_outer_pkt_prio;
    to->it_outer_cfi = from->it_outer_cfi;
    to->it_inner = from->it_inner;
    to->it_inner_prio = from->it_inner_prio;
    to->it_inner_pkt_prio = from->it_inner_pkt_prio;
    to->it_inner_cfi = from->it_inner_cfi;
    to->ut_outer = from->ut_outer;
    to->ut_outer_pkt_prio = from->ut_outer_pkt_prio;
    to->ut_outer_cfi = from->ut_outer_cfi;
    to->ut_inner = from->ut_inner;
    to->ut_inner_pkt_prio = from->ut_inner_pkt_prio;
    to->ut_inner_cfi = from->ut_inner_cfi;
    to->outer_pcp = from->outer_pcp;
    to->inner_pcp = from->inner_pcp;
    to->policer_id = from->policer_id;
    to->outer_tpid = from->outer_tpid;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid_action = from->outer_tpid_action;
    to->inner_tpid_action = from->inner_tpid_action;
    to->action_id = from->action_id;
    to->class_id = from->class_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->flags = from->flags;
    to->outer_tag = from->outer_tag;
    to->inner_tag = from->inner_tag;
    to->forwarding_domain = from->forwarding_domain;
    to->inner_qos_map_id = from->inner_qos_map_id;
    to->outer_qos_map_id = from->outer_qos_map_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6520in_fcoe_vsan_action_set_t
 * Purpose:
 *      Convert the bcm_fcoe_vsan_action_set_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_fcoe_vsan_action_set_t(
    bcm_compat6520_fcoe_vsan_action_set_t *from,
    bcm_fcoe_vsan_action_set_t *to)
{
    bcm_fcoe_vsan_action_set_t_init(to);
    to->flags = from->flags;
    to->new_vsan = from->new_vsan;
    to->new_vsan_pri = from->new_vsan_pri;
    sal_memcpy(&to->vft, &from->vft, sizeof(bcm_fcoe_vsan_vft_t));
    to->ingress_if = from->ingress_if;
    to->dt_vsan = from->dt_vsan;
    to->dt_vsan_pri = from->dt_vsan_pri;
    to->it_vsan = from->it_vsan;
    to->it_vsan_pri = from->it_vsan_pri;
    to->ot_vsan = from->ot_vsan;
    to->ot_vsan_pri = from->ot_vsan_pri;
    to->ut_vsan = from->ut_vsan;
    to->ut_vsan_pri = from->ut_vsan_pri;
    _bcm_compat6520in_vlan_action_set_t(&from->vlan_action, &to->vlan_action);
}

/*
 * Function:
 *      bcm_compat6520_fcoe_vsan_translate_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_fcoe_vsan_translate_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key - (IN) VSAN Translate key configuration
 *      action - (IN) VSAN and VLAN Translate action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6520_fcoe_vsan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_fcoe_vsan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_fcoe_vsan_action_set_t *)
                     sal_alloc(sizeof(bcm_fcoe_vsan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_fcoe_vsan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_fcoe_vsan_translate_action_add(unit, key, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

STATIC void
_bcm_compat6520in_udf_abstract_pkt_format_info_t(
        bcm_compat6520_udf_abstract_pkt_format_info_t *from,
        bcm_udf_abstract_pkt_format_info_t *to)
{
    to->base_offset = from->base_offset;
    to->num_chunks_max = from->num_chunks_max;
    to->chunk_bmap_used = from->chunk_bmap_used;
    to->unavail_chunk_bmap = from->unavail_chunk_bmap;
}

STATIC void
_bcm_compat6520out_udf_abstract_pkt_format_info_t(
        bcm_udf_abstract_pkt_format_info_t *to,
        bcm_compat6520_udf_abstract_pkt_format_info_t *from)
{
    to->base_offset = from->base_offset;
    to->num_chunks_max = from->num_chunks_max;
    to->chunk_bmap_used = from->chunk_bmap_used;
    to->unavail_chunk_bmap = from->unavail_chunk_bmap;
}

/* API to retrieve Abstract packet format information. */
int
bcm_compat6520_udf_abstract_pkt_format_info_get(
        int unit, 
        bcm_udf_abstract_pkt_format_t abstract_pkt_format, 
        bcm_compat6520_udf_abstract_pkt_format_info_t *pkt_format_info)
{
    int rv = 0;
    bcm_udf_abstract_pkt_format_info_t *new_config = NULL;

    if (NULL == pkt_format_info) {
        return BCM_E_PARAM;
    }

    new_config = (bcm_udf_abstract_pkt_format_info_t *)
        sal_alloc(sizeof(bcm_udf_abstract_pkt_format_info_t), "UDF PKT fmt info");
    if (NULL == new_config) {
        return BCM_E_MEMORY;
    }
    _bcm_compat6520in_udf_abstract_pkt_format_info_t(pkt_format_info, new_config);

    rv = bcm_udf_abstract_pkt_format_info_get(unit, abstract_pkt_format, new_config);
    if (NULL != new_config) {
        _bcm_compat6520out_udf_abstract_pkt_format_info_t(new_config, pkt_format_info);
        sal_free(new_config);
    }
    return rv;
}
/*
 * Function:
 *      bcm_compat6520_field_action_vlan_actions_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_action_vlan_actions_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field Entry ID.
 *      action - (IN) Field Action value \ref bcm_field_action_t enumeration.
 *      vlan_action_set - (IN) VLAN Action Set \ref BCM_VLAN_ACTION_SET_t structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6520_vlan_action_set_t *vlan_action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_vlan_action_set = NULL;

    if (vlan_action_set != NULL) {
        new_vlan_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New vlan_action_set");
        if (new_vlan_action_set == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(vlan_action_set, new_vlan_action_set);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_add(unit, entry, action, new_vlan_action_set);


    /* Deallocate memory*/
    sal_free(new_vlan_action_set);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_action_vlan_actions_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_action_vlan_actions_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field Entry ID.
 *      action - (IN) Field Action Value.
 *      vlan_action_set - (IN) Reference to \ref BCM_VLAN_ACTION_SET_t structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6520_vlan_action_set_t *vlan_action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_vlan_action_set = NULL;

    if (vlan_action_set != NULL) {
        new_vlan_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New vlan_action_set");
        if (new_vlan_action_set == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(vlan_action_set, new_vlan_action_set);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_get(unit, entry, action, new_vlan_action_set);


    /* Deallocate memory*/
    sal_free(new_vlan_action_set);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_port_default_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_default_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (OUT) (for "_set") Default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_get(unit, port, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_port_default_action_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_default_action_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (IN) (for "_set") Default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_set(unit, port, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_port_egress_default_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_egress_default_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (OUT) (for "_set") Egress default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns egress default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_get(unit, port, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_port_egress_default_action_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_egress_default_action_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (IN) (for "_set") Egress default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns egress default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_set(unit, port, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_port_protocol_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_protocol_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device or logical port number
 *      frame - (IN) Frame type, as specified in \ref BCM_PORT_FRAMETYPE_e
 *      ether - (IN) 16-bit EtherType
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_add(unit, port, frame, ether, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_port_protocol_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_protocol_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device or logical port number
 *      frame - (IN) Frame type, as specified in \ref BCM_PORT_FRAMETYPE_e
 *      ether - (IN) 16-bit EtherType
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_get(unit, port, frame, ether, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_mac_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_mac_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) IEEE 802.3 MAC address
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_add(unit, mac, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_mac_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_mac_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) IEEE 802.3 MAC address
 *      action - (OUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_get(unit, mac, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier. Note: The port parameter is used to indicate port group when the VLAN translation key type
is any of bcmVlanTranslateKeyPortGroupDouble/bcmVlanTranslateKeyPortGroupOuter/bcmVlanTranslateKeyPortGroupInner/bcmVlanTranslateKeyPortGroupOuterTag/
bcmVlanTranslateKeyPortGroupInnerTag/bcmVlanTranslateKeyPortGroupOuterPri.<br>GPORT (global port) identifier
 *      key_type - (IN) VLAN translation key type<br>VLAN translation key type
 *      outer_vlan - (IN) Outer VLAN ID or tag<br>Outer VLAN ID or tag
 *      inner_vlan - (IN) Inner VLAN ID or tag<br>Inner VLAN ID or tag
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_add(unit, port, key_type, outer_vlan, inner_vlan, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_action_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier. Note: The port parameter is used to indicate port group when the VLAN translation key type
is any of bcmVlanTranslateKeyPortGroupDouble/bcmVlanTranslateKeyPortGroupOuter/bcmVlanTranslateKeyPortGroupInner/bcmVlanTranslateKeyPortGroupOuterTag/
bcmVlanTranslateKeyPortGroupInnerTag/bcmVlanTranslateKeyPortGroupOuterPri.<br>GPORT (global port) identifier
 *      key_type - (IN) VLAN translation key type<br>VLAN translation key type
 *      outer_vlan - (IN) Outer VLAN ID or tag<br>Outer VLAN ID or tag
 *      inner_vlan - (IN) Inner VLAN ID or tag<br>Inner VLAN ID or tag
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_create(unit, port, key_type, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier. Note: The port parameter is used to indicate port group when the VLAN translation key type
is any of bcmVlanTranslateKeyPortGroupDouble/bcmVlanTranslateKeyPortGroupOuter/bcmVlanTranslateKeyPortGroupInner/bcmVlanTranslateKeyPortGroupOuterTag/
bcmVlanTranslateKeyPortGroupInnerTag/bcmVlanTranslateKeyPortGroupOuterPri.
 *      key_type - (IN) VLAN translation key type
 *      outer_vlan - (IN) Outer VLAN ID or tag
 *      inner_vlan - (IN) Inner VLAN ID or tag
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_get(unit, port, key_type, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_egress_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Port class
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_add(unit, port_class, outer_vlan, inner_vlan, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_egress_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Port class
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_get(unit, port_class, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_egress_gport_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_gport_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Egress gport
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_add(unit, port, outer_vlan, inner_vlan, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_egress_gport_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_gport_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Egress gport
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_get(unit, port, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_action_range_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_range_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier
 *      outer_vlan_low - (IN) Low Outer VLAN ID
 *      outer_vlan_high - (IN) High Outer VLAN ID
 *      inner_vlan_low - (IN) Low Inner VLAN ID
 *      inner_vlan_high - (IN) High Inner VLAN ID
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_add(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_action_range_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_range_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier
 *      outer_vlan_low - (IN) Low Outer VLAN ID
 *      outer_vlan_high - (IN) High Outer VLAN ID
 *      inner_vlan_low - (IN) Low Inner VLAN ID
 *      inner_vlan_high - (IN) High Inner VLAN ID
 *      action - (OUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_get(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_action_id_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_id_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Handling flags of type BCM_VLAN_ACTION_SET_XXX
 *      action_id - (IN) Action ID. Has to be created before it can be set.
 *      action - (IN) Pointer to a structure that holds tag actions per action ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_action_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Handling flags of type BCM_VLAN_ACTION_SET_XXX
 *      action_id - (IN) Action ID. Has to be created before get can be performed.
 *      action - (OUT) Pointer to a structure that holds tag actions per action ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_get(unit, flags, action_id, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_ip_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_ip_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_ip - (IN) A container for subnet-to-VLAN association data
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_add(unit, vlan_ip, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_ip_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_ip_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_ip - (IN) A container for subnet-to-VLAN association data
 *      action - (OUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6520_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_get(unit, vlan_ip, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_match_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Option from BCM_VLAN_MATCH_ACTION_XXX
 *      match_info - (IN) Match criteria info
 *      action_set - (IN) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6520_vlan_action_set_t *action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action_set = NULL;

    if (action_set != NULL) {
        new_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action_set");
        if (new_action_set == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_action_set_t(action_set, new_action_set);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_add(unit, options, match_info, new_action_set);


    /* Deallocate memory*/
    sal_free(new_action_set);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_match_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match_info - (IN) Match criteria info
 *      action_set - (OUT) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_match_action_get(
    int unit,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6520_vlan_action_set_t *action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action_set = NULL;

    if (action_set != NULL) {
        new_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action_set");
        if (new_action_set == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_get(unit, match_info, new_action_set);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_action_set_t(new_action_set, action_set);

    /* Deallocate memory*/
    sal_free(new_action_set);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_match_action_multi_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_multi_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Vlan match. See bcm_vlan_match_t
 *      size - (IN) Number of elements in match info array.
 *      match_info_array - (OUT) Match criteria info array.
 *      action_set_array - (OUT) Action set array
 *      count - (OUT) Valid Match criteria info count.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_vlan_match_info_t *match_info_array,
    bcm_compat6520_vlan_action_set_t *action_set_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action_set_array = NULL;
    int i = 0;

    if (action_set_array != NULL && size > 0) {
        new_action_set_array = (bcm_vlan_action_set_t *)
                     sal_alloc(size * sizeof(bcm_vlan_action_set_t),
                     "New action_set_array");
        if (new_action_set_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_multi_get(unit, match, size, match_info_array, new_action_set_array, count);

    for (i = 0; i < size; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6520out_vlan_action_set_t(&new_action_set_array[i], &action_set_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_action_set_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_vlan_control_vlan_t(
    bcm_compat6520_vlan_control_vlan_t *from,
    bcm_vlan_control_vlan_t *to)
{
    bcm_vlan_control_vlan_t_init(to);
    to->vrf = from->vrf;
    to->forwarding_vlan = from->forwarding_vlan;
    to->ingress_if = from->ingress_if;
    to->outer_tpid = from->outer_tpid;
    to->flags = from->flags;
    to->ip6_mcast_flood_mode = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode = from->l2_mcast_flood_mode;
    to->if_class = from->if_class;
    to->forwarding_mode = from->forwarding_mode;
    to->urpf_mode = from->urpf_mode;
    to->cosq = from->cosq;
    to->qos_map_id = from->qos_map_id;
    to->distribution_class = from->distribution_class;
    to->broadcast_group = from->broadcast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->trill_nonunicast_group = from->trill_nonunicast_group;
    to->source_trill_name = from->source_trill_name;
    to->trunk_index = from->trunk_index;
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt, sizeof(bcm_vlan_protocol_packet_ctrl_t));
    to->nat_realm_id = from->nat_realm_id;
    to->l3_if_class = from->l3_if_class;
    to->vp_mc_ctrl = from->vp_mc_ctrl;
    to->aging_cycles = from->aging_cycles;
    to->entropy_id = from->entropy_id;
    to->vpn = from->vpn;
    to->egress_vlan = from->egress_vlan;
    to->learn_flags = from->learn_flags;
    to->sr_flags = from->sr_flags;
    to->flags2 = from->flags2;
    to->ingress_stat_id = from->ingress_stat_id;
    to->ingress_stat_pp_profile = from->ingress_stat_pp_profile;
    to->egress_stat_id = from->egress_stat_id;
    to->egress_stat_pp_profile = from->egress_stat_pp_profile;
    _bcm_compat6520in_vlan_action_set_t(&from->egress_action, &to->egress_action);
    to->l2_view = from->l2_view;
    to->egress_class_id = from->egress_class_id;
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
    to->unknown_dest = from->unknown_dest;
}

/*
 * Function:
 *      _bcm_compat6520out_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_vlan_control_vlan_t(
    bcm_vlan_control_vlan_t *from,
    bcm_compat6520_vlan_control_vlan_t *to)
{
    to->vrf = from->vrf;
    to->forwarding_vlan = from->forwarding_vlan;
    to->ingress_if = from->ingress_if;
    to->outer_tpid = from->outer_tpid;
    to->flags = from->flags;
    to->ip6_mcast_flood_mode = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode = from->l2_mcast_flood_mode;
    to->if_class = from->if_class;
    to->forwarding_mode = from->forwarding_mode;
    to->urpf_mode = from->urpf_mode;
    to->cosq = from->cosq;
    to->qos_map_id = from->qos_map_id;
    to->distribution_class = from->distribution_class;
    to->broadcast_group = from->broadcast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->trill_nonunicast_group = from->trill_nonunicast_group;
    to->source_trill_name = from->source_trill_name;
    to->trunk_index = from->trunk_index;
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt, sizeof(bcm_vlan_protocol_packet_ctrl_t));
    to->nat_realm_id = from->nat_realm_id;
    to->l3_if_class = from->l3_if_class;
    to->vp_mc_ctrl = from->vp_mc_ctrl;
    to->aging_cycles = from->aging_cycles;
    to->entropy_id = from->entropy_id;
    to->vpn = from->vpn;
    to->egress_vlan = from->egress_vlan;
    to->learn_flags = from->learn_flags;
    to->sr_flags = from->sr_flags;
    to->flags2 = from->flags2;
    to->ingress_stat_id = from->ingress_stat_id;
    to->ingress_stat_pp_profile = from->ingress_stat_pp_profile;
    to->egress_stat_id = from->egress_stat_id;
    to->egress_stat_pp_profile = from->egress_stat_pp_profile;
    _bcm_compat6520out_vlan_action_set_t(&from->egress_action, &to->egress_action);
    to->l2_view = from->l2_view;
    to->egress_class_id = from->egress_class_id;
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
    to->unknown_dest = from->unknown_dest;
}

/*
 * Function:
 *      bcm_compat6520_vlan_control_vlan_selective_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VPN
 *      valid_fields - (IN) Valid fields for VLAN control structure
 *      control - (OUT) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6520_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t *new_control = NULL;

    if (control != NULL) {
        new_control = (bcm_vlan_control_vlan_t *)
                     sal_alloc(sizeof(bcm_vlan_control_vlan_t),
                     "New control");
        if (new_control == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_selective_get(unit, vlan, valid_fields, new_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_control_vlan_t(new_control, control);

    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_control_vlan_selective_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VPN
 *      valid_fields - (IN) Valid fields for VLAN control structure
 *      control - (IN) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6520_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t *new_control = NULL;

    if (control != NULL) {
        new_control = (bcm_vlan_control_vlan_t *)
                     sal_alloc(sizeof(bcm_vlan_control_vlan_t),
                     "New control");
        if (new_control == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_control_vlan_t(control, new_control);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_selective_set(unit, vlan, valid_fields, new_control);


    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_control_vlan_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      control - (OUT) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6520_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t *new_control = NULL;

    if (control != NULL) {
        new_control = (bcm_vlan_control_vlan_t *)
                     sal_alloc(sizeof(bcm_vlan_control_vlan_t),
                     "New control");
        if (new_control == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_get(unit, vlan, new_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_control_vlan_t(new_control, control);

    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_control_vlan_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      control - (IN) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    /*
     * COVERITY
     *
     * Passing value instead of reference is kept intentionally as
     * the API was there for a fairly long time.
     */
    /* coverity[pass_by_value] */
    bcm_compat6520_vlan_control_vlan_t control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t new_control;

    /* Transform the entry from the old format to new one */
    _bcm_compat6520in_vlan_control_vlan_t(&control, &new_control);
    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_set(unit, vlan, new_control);


    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_vlan_translate_egress_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_action_set_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_vlan_translate_egress_action_set_t(
    bcm_compat6520_vlan_translate_egress_action_set_t *from,
    bcm_vlan_translate_egress_action_set_t *to)
{
    bcm_vlan_translate_egress_action_set_t_init(to);
    to->flags = from->flags;
    _bcm_compat6520in_vlan_action_set_t(&from->vlan_action, &to->vlan_action);
}

/*
 * Function:
 *      _bcm_compat6520out_vlan_translate_egress_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_action_set_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_vlan_translate_egress_action_set_t(
    bcm_vlan_translate_egress_action_set_t *from,
    bcm_compat6520_vlan_translate_egress_action_set_t *to)
{
    to->flags = from->flags;
    _bcm_compat6520out_vlan_action_set_t(&from->vlan_action, &to->vlan_action);
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_egress_action_extended_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) 
 *      action - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6520_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_translate_egress_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_vlan_translate_egress_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_add(unit, key_config, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vlan_translate_egress_action_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) 
 *      action - (OUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6520_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_translate_egress_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_get(unit, key_config, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_vlan_translate_egress_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}


#if defined(INCLUDE_L3)

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_initiator_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_initiator_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) The egress L3 interface<br>The egress L3 interface
 *      num_labels - (IN) Number of labels in the array<br>Number of labels in the array
 *      label_array - (IN) Array of MPLS label and header information<br>(INOUT) Array of MPLS label and header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6520_mpls_egress_label_t *label_array)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *new_label_array = NULL;
    int i = 0;

    if (label_array != NULL && num_labels > 0) {
        new_label_array = (bcm_mpls_egress_label_t *)
                     sal_alloc(num_labels * sizeof(bcm_mpls_egress_label_t),
                     "New label_array");
        if (new_label_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < num_labels; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6520in_mpls_egress_label_t(&label_array[i], &new_label_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_initiator_set(unit, intf, num_labels, new_label_array);


    /* Deallocate memory*/
    sal_free(new_label_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      num_labels - (IN) 
 *      label_array - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6520_mpls_egress_label_t *label_array)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *new_label_array = NULL;
    int i = 0;

    if (label_array != NULL && num_labels > 0) {
        new_label_array = (bcm_mpls_egress_label_t *)
                     sal_alloc(num_labels * sizeof(bcm_mpls_egress_label_t),
                     "New label_array");
        if (new_label_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < num_labels; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6520in_mpls_egress_label_t(&label_array[i], &new_label_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_initiator_create(unit, intf, num_labels, new_label_array);

    for (i = 0; i < num_labels; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6520out_mpls_egress_label_t(&new_label_array[i], &label_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_label_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) The egress L3 interface
 *      label_max - (IN) Number of entries in label_array
 *      label_array - (OUT) MPLS header information
 *      label_count - (OUT) Actual number of labels returned
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_compat6520_mpls_egress_label_t *label_array,
    int *label_count)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *new_label_array = NULL;
    int i = 0;

    if (label_array != NULL && label_max > 0) {
        new_label_array = (bcm_mpls_egress_label_t *)
                     sal_alloc(label_max * sizeof(bcm_mpls_egress_label_t),
                     "New label_array");
        if (new_label_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_initiator_get(unit, intf, label_max, new_label_array, label_count);

    for (i = 0; i < label_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6520out_mpls_egress_label_t(&new_label_array[i], &label_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_label_array);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6520in_mpls_tunnel_encap_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_encap_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_mpls_tunnel_encap_t(
    bcm_compat6520_mpls_tunnel_encap_t *from,
    bcm_mpls_tunnel_encap_t *to)
{
    int i1 = 0;

    bcm_mpls_tunnel_encap_t_init(to);
    to->tunnel_id = from->tunnel_id;
    to->flags = from->flags;
    to->num_labels = from->num_labels;
    for (i1 = 0; i1 < BCM_MPLS_EGRESS_LABEL_MAX; i1++) {
        _bcm_compat6520in_mpls_egress_label_t(&from->label_array[i1], &to->label_array[i1]);
    }
    to->qos_map_id = from->qos_map_id;
}

/*
 * Function:
 *      _bcm_compat6520out_mpls_tunnel_encap_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_encap_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_mpls_tunnel_encap_t(
    bcm_mpls_tunnel_encap_t *from,
    bcm_compat6520_mpls_tunnel_encap_t *to)
{
    int i1 = 0;

    to->tunnel_id = from->tunnel_id;
    to->flags = from->flags;
    to->num_labels = from->num_labels;
    for (i1 = 0; i1 < BCM_MPLS_EGRESS_LABEL_MAX; i1++) {
        _bcm_compat6520out_mpls_egress_label_t(&from->label_array[i1], &to->label_array[i1]);
    }
    to->qos_map_id = from->qos_map_id;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_encap_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_encap_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Operation options
 *      tunnel_encap - (INOUT) (IN/OUT) MPLS tunnel initiator information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_encap_create(
    int unit,
    uint32 options,
    bcm_compat6520_mpls_tunnel_encap_t *tunnel_encap)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_encap_t *new_tunnel_encap = NULL;

    if (tunnel_encap != NULL) {
        new_tunnel_encap = (bcm_mpls_tunnel_encap_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_encap_t),
                     "New tunnel_encap");
        if (new_tunnel_encap == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_tunnel_encap_t(tunnel_encap, new_tunnel_encap);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_encap_create(unit, options, new_tunnel_encap);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_mpls_tunnel_encap_t(new_tunnel_encap, tunnel_encap);

    /* Deallocate memory*/
    sal_free(new_tunnel_encap);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tunnel_encap - (INOUT) (IN/OUT) MPLS tunnel initiator information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_encap_get(
    int unit,
    bcm_compat6520_mpls_tunnel_encap_t *tunnel_encap)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_encap_t *new_tunnel_encap = NULL;

    if (tunnel_encap != NULL) {
        new_tunnel_encap = (bcm_mpls_tunnel_encap_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_encap_t),
                     "New tunnel_encap");
        if (new_tunnel_encap == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_tunnel_encap_t(tunnel_encap, new_tunnel_encap);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_encap_get(unit, new_tunnel_encap);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_mpls_tunnel_encap_t(new_tunnel_encap, tunnel_encap);

    /* Deallocate memory*/
    sal_free(new_tunnel_encap);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6520in_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_mpls_tunnel_switch_t(
    bcm_compat6520_mpls_tunnel_switch_t *from,
    bcm_mpls_tunnel_switch_t *to)
{
    bcm_mpls_tunnel_switch_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->second_label = from->second_label;
    to->port = from->port;
    to->action = from->action;
    to->action_if_bos = from->action_if_bos;
    to->action_if_not_bos = from->action_if_not_bos;
    to->mc_group = from->mc_group;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->policer_id = from->policer_id;
    to->vpn = from->vpn;
    _bcm_compat6520in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->egress_if = from->egress_if;
    to->ingress_if = from->ingress_if;
    to->mtu = from->mtu;
    to->qos_map_id = from->qos_map_id;
    to->failover_id = from->failover_id;
    to->tunnel_id = from->tunnel_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->egress_port = from->egress_port;
    to->oam_global_context_id = from->oam_global_context_id;
    to->class_id = from->class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->ecn_map_id = from->ecn_map_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
}

/*
 * Function:
 *      _bcm_compat6520out_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_mpls_tunnel_switch_t(
    bcm_mpls_tunnel_switch_t *from,
    bcm_compat6520_mpls_tunnel_switch_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->second_label = from->second_label;
    to->port = from->port;
    to->action = from->action;
    to->action_if_bos = from->action_if_bos;
    to->action_if_not_bos = from->action_if_not_bos;
    to->mc_group = from->mc_group;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->policer_id = from->policer_id;
    to->vpn = from->vpn;
    _bcm_compat6520out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->egress_if = from->egress_if;
    to->ingress_if = from->ingress_if;
    to->mtu = from->mtu;
    to->qos_map_id = from->qos_map_id;
    to->failover_id = from->failover_id;
    to->tunnel_id = from->tunnel_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->egress_port = from->egress_port;
    to->oam_global_context_id = from->oam_global_context_id;
    to->class_id = from->class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->ecn_map_id = from->ecn_map_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_switch_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Label (switch) information<br>(INOUT) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_switch_add(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_add(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_switch_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Label (switch) information<br>(INOUT) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_switch_create(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_mpls_tunnel_switch_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_switch_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_switch_delete(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_delete(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_tunnel_switch_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Label (switch) information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_tunnel_switch_get(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t *)
                     sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_tunnel_switch_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_mpls_tunnel_switch_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6520in_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_mpls_port_t(
    bcm_compat6520_mpls_port_t *from,
    bcm_mpls_port_t *to)
{
    bcm_mpls_port_t_init(to);
    to->mpls_port_id = from->mpls_port_id;
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->if_class = from->if_class;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->service_tpid = from->service_tpid;
    to->port = from->port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_label = from->match_label;
    to->egress_tunnel_if = from->egress_tunnel_if;
    _bcm_compat6520in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->mtu = from->mtu;
    to->egress_service_vlan = from->egress_service_vlan;
    to->pw_seq_number = from->pw_seq_number;
    to->encap_id = from->encap_id;
    to->ingress_failover_id = from->ingress_failover_id;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->policer_id = from->policer_id;
    to->failover_mc_group = from->failover_mc_group;
    to->pw_failover_id = from->pw_failover_id;
    to->pw_failover_port_id = from->pw_failover_port_id;
    to->vccv_type = from->vccv_type;
    to->network_group_id = from->network_group_id;
    to->match_subport_pkt_vid = from->match_subport_pkt_vid;
    to->tunnel_id = from->tunnel_id;
    to->per_flow_queue_base = from->per_flow_queue_base;
    to->qos_map_id = from->qos_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ecn_map_id = from->ecn_map_id;
    to->class_id = from->class_id;
    to->egress_class_id = from->egress_class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    _bcm_compat6520in_mpls_egress_label_t(&from->egress_tunnel_label, &to->egress_tunnel_label);
    to->nof_service_tags = from->nof_service_tags;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->context_label = from->context_label;
    to->ingress_if = from->ingress_if;
    to->port_group = from->port_group;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
}

/*
 * Function:
 *      _bcm_compat6520out_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_mpls_port_t(
    bcm_mpls_port_t *from,
    bcm_compat6520_mpls_port_t *to)
{
    to->mpls_port_id = from->mpls_port_id;
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->if_class = from->if_class;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->service_tpid = from->service_tpid;
    to->port = from->port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_label = from->match_label;
    to->egress_tunnel_if = from->egress_tunnel_if;
    _bcm_compat6520out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->mtu = from->mtu;
    to->egress_service_vlan = from->egress_service_vlan;
    to->pw_seq_number = from->pw_seq_number;
    to->encap_id = from->encap_id;
    to->ingress_failover_id = from->ingress_failover_id;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->policer_id = from->policer_id;
    to->failover_mc_group = from->failover_mc_group;
    to->pw_failover_id = from->pw_failover_id;
    to->pw_failover_port_id = from->pw_failover_port_id;
    to->vccv_type = from->vccv_type;
    to->network_group_id = from->network_group_id;
    to->match_subport_pkt_vid = from->match_subport_pkt_vid;
    to->tunnel_id = from->tunnel_id;
    to->per_flow_queue_base = from->per_flow_queue_base;
    to->qos_map_id = from->qos_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ecn_map_id = from->ecn_map_id;
    to->class_id = from->class_id;
    to->egress_class_id = from->egress_class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    _bcm_compat6520out_mpls_egress_label_t(&from->egress_tunnel_label, &to->egress_tunnel_label);
    to->nof_service_tags = from->nof_service_tags;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->context_label = from->context_label;
    to->ingress_if = from->ingress_if;
    to->port_group = from->port_group;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
}

/*
 * Function:
 *      bcm_compat6520_mpls_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6520_mpls_port_t *mpls_port)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (mpls_port != NULL) {
        new_mpls_port = (bcm_mpls_port_t *)
                     sal_alloc(sizeof(bcm_mpls_port_t),
                     "New mpls_port");
        if (new_mpls_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_add(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6520_mpls_port_t *mpls_port)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (mpls_port != NULL) {
        new_mpls_port = (bcm_mpls_port_t *)
                     sal_alloc(sizeof(bcm_mpls_port_t),
                     "New mpls_port");
        if (new_mpls_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mpls_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      port_max - (IN) Maximum number of ports in array
 *      port_array - (OUT) Array of MPLS ports
 *      port_count - (OUT) Number of ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6520_mpls_port_t *port_array,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_port_array = NULL;
    int i = 0;

    if (port_array != NULL && port_max > 0) {
        new_port_array = (bcm_mpls_port_t *)
                     sal_alloc(port_max * sizeof(bcm_mpls_port_t),
                     "New port_array");
        if (new_port_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get_all(unit, vpn, port_max, new_port_array, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6520out_mpls_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_port_array);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6520in_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_tunnel_initiator_t(
    bcm_compat6520_tunnel_initiator_t *from,
    bcm_tunnel_initiator_t *to)
{
    int i1 = 0;

    bcm_tunnel_initiator_t_init(to);
    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->stat_cmd = from->stat_cmd;
    to->counter_command_id = from->counter_command_id;
}

/*
 * Function:
 *      _bcm_compat6520out_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_tunnel_initiator_t(
    bcm_tunnel_initiator_t *from,
    bcm_compat6520_tunnel_initiator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->stat_cmd = from->stat_cmd;
    to->counter_command_id = from->counter_command_id;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_initiator_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6520_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_set(unit, intf, new_tunnel);


    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (INOUT) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_initiator_create(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6520_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_create(unit, intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_initiator_get(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6520_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_get(unit, intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vxlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vxlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_wlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_wlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l2gre_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l2gre_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_l3_aacl_t
 * Purpose:
 *      Convert the bcm_l3_aacl_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_l3_aacl_t(
    bcm_compat6520_l3_aacl_t *from,
    bcm_l3_aacl_t *to)
{
    int i1 = 0;

    bcm_l3_aacl_t_init(to);
    to->flags = from->flags;
    to->ip = from->ip;
    to->ip_mask = from->ip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->ip6[i1] = from->ip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->ip6_mask[i1] = from->ip6_mask[i1];
    }
    to->l4_port = from->l4_port;
    to->l4_port_mask = from->l4_port_mask;
    to->class_id = from->class_id;
}

/*
 * Function:
 *      _bcm_compat6520out_l3_aacl_t
 * Purpose:
 *      Convert the bcm_l3_aacl_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_l3_aacl_t(
    bcm_l3_aacl_t *from,
    bcm_compat6520_l3_aacl_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->ip = from->ip;
    to->ip_mask = from->ip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->ip6[i1] = from->ip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->ip6_mask[i1] = from->ip6_mask[i1];
    }
    to->l4_port = from->l4_port;
    to->l4_port_mask = from->l4_port_mask;
    to->class_id = from->class_id;
}

/*
 * Function:
 *      bcm_compat6520_l3_aacl_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_aacl_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) L3 AACL options.
 *      aacl - (IN) Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l3_aacl_add(
    int unit,
    uint32 options,
    bcm_compat6520_l3_aacl_t *aacl)
{
    int rv = BCM_E_NONE;
    bcm_l3_aacl_t *new_aacl = NULL;

    if (aacl != NULL) {
        new_aacl = (bcm_l3_aacl_t *)
                     sal_alloc(sizeof(bcm_l3_aacl_t),
                     "New aacl");
        if (new_aacl == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_l3_aacl_t(aacl, new_aacl);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_aacl_add(unit, options, new_aacl);


    /* Deallocate memory*/
    sal_free(new_aacl);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l3_aacl_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_aacl_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      aacl - (IN) Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l3_aacl_delete(
    int unit,
    bcm_compat6520_l3_aacl_t *aacl)
{
    int rv = BCM_E_NONE;
    bcm_l3_aacl_t *new_aacl = NULL;

    if (aacl != NULL) {
        new_aacl = (bcm_l3_aacl_t *)
                     sal_alloc(sizeof(bcm_l3_aacl_t),
                     "New aacl");
        if (new_aacl == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_l3_aacl_t(aacl, new_aacl);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_aacl_delete(unit, new_aacl);


    /* Deallocate memory*/
    sal_free(new_aacl);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l3_aacl_delete_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_aacl_delete_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      aacl - (IN) Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l3_aacl_delete_all(
    int unit,
    bcm_compat6520_l3_aacl_t *aacl)
{
    int rv = BCM_E_NONE;
    bcm_l3_aacl_t *new_aacl = NULL;

    if (aacl != NULL) {
        new_aacl = (bcm_l3_aacl_t *)
                     sal_alloc(sizeof(bcm_l3_aacl_t),
                     "New aacl");
        if (new_aacl == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_l3_aacl_t(aacl, new_aacl);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_aacl_delete_all(unit, new_aacl);


    /* Deallocate memory*/
    sal_free(new_aacl);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l3_aacl_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_aacl_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      aacl - (INOUT) Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l3_aacl_find(
    int unit,
    bcm_compat6520_l3_aacl_t *aacl)
{
    int rv = BCM_E_NONE;
    bcm_l3_aacl_t *new_aacl = NULL;

    if (aacl != NULL) {
        new_aacl = (bcm_l3_aacl_t *)
                     sal_alloc(sizeof(bcm_l3_aacl_t),
                     "New aacl");
        if (new_aacl == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_l3_aacl_t(aacl, new_aacl);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_aacl_find(unit, new_aacl);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_l3_aacl_t(new_aacl, aacl);

    /* Deallocate memory*/
    sal_free(new_aacl);

    return rv;
}
#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_BFD)
/*
 * Function:
 *      _bcm_compat6520in_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_bfd_endpoint_info_t(
    bcm_compat6520_bfd_endpoint_info_t *from,
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
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
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
    to->vxlan_vnid = 0;
    to->vlan_tpid = 0;
    to->inner_vlan_tpid = 0;
}

/*
 * Function:
 *      _bcm_compat6520out_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_bfd_endpoint_info_t(
    bcm_bfd_endpoint_info_t *from,
    bcm_compat6520_bfd_endpoint_info_t *to)
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
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
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
}

/*
 * Function:
 *      bcm_compat6520_bfd_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (INOUT) Pointer to an BFD endpoint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_bfd_endpoint_create(
    int unit,
    bcm_compat6520_bfd_endpoint_info_t *endpoint_info)
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
        _bcm_compat6520in_bfd_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_bfd_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure to receive the data.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6520_bfd_endpoint_info_t *endpoint_info)
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
    _bcm_compat6520out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}
#endif /* defined(INCLUDE_BFD) */

/*
 * Function:
 *      _bcm_compat6520in_port_mapping_info_t
 * Purpose:
 *      Convert the bcm_port_mapping_info_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_port_mapping_info_t(
    bcm_compat6520_port_mapping_info_t *from,
    bcm_port_mapping_info_t *to)
{
    bcm_port_mapping_info_t_init(to);
    to->channel = from->channel;
    to->core = from->core;
    to->tm_port = from->tm_port;
    to->pp_port = from->pp_port;
    to->base_q_pair = from->base_q_pair;
    to->num_priorities = from->num_priorities;
    to->base_hr = from->base_hr;
    to->num_sch_priorities = from->num_sch_priorities;
    to->base_uc_queue = from->base_uc_queue;
    to->num_uc_queue = from->num_uc_queue;
    to->base_mc_queue = from->base_mc_queue;
    to->num_mc_queue = from->num_mc_queue;
}

/*
 * Function:
 *      _bcm_compat6520out_port_mapping_info_t
 * Purpose:
 *      Convert the bcm_port_mapping_info_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_port_mapping_info_t(
    bcm_port_mapping_info_t *from,
    bcm_compat6520_port_mapping_info_t *to)
{
    to->channel = from->channel;
    to->core = from->core;
    to->tm_port = from->tm_port;
    to->pp_port = from->pp_port;
    to->base_q_pair = from->base_q_pair;
    to->num_priorities = from->num_priorities;
    to->base_hr = from->base_hr;
    to->num_sch_priorities = from->num_sch_priorities;
    to->base_uc_queue = from->base_uc_queue;
    to->num_uc_queue = from->num_uc_queue;
    to->base_mc_queue = from->base_mc_queue;
    to->num_mc_queue = from->num_mc_queue;
}

/*
 * Function:
 *      bcm_compat6520_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device or logical port number.
 *      flags - (IN) (for "_set") See \ref BCM_PORT_ADD_flags for details.<br>(for "_get") See \ref BCM_PORT_ADD_flags for details.
 *      interface_info - (IN) (for "_set") The requested interface configuration.<br>(for "_get") The requested interface configuration.
 *      mapping_info - (IN) (for "_set") The requested port mapping configuration.<br>(for "_get") The requested port mapping configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_port_add(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_interface_info_t *interface_info,
    bcm_compat6520_port_mapping_info_t *mapping_info)
{
    int rv = BCM_E_NONE;
    bcm_port_mapping_info_t *new_mapping_info = NULL;

    if (mapping_info != NULL) {
        new_mapping_info = (bcm_port_mapping_info_t *)
                     sal_alloc(sizeof(bcm_port_mapping_info_t),
                     "New mapping_info");
        if (new_mapping_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_port_mapping_info_t(mapping_info, new_mapping_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_add(unit, port, flags, interface_info, new_mapping_info);


    /* Deallocate memory*/
    sal_free(new_mapping_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device or logical port number.
 *      flags - (OUT) (for "_set") See \ref BCM_PORT_ADD_flags for details.<br>(for "_get") See \ref BCM_PORT_ADD_flags for details.
 *      interface_info - (OUT) (for "_set") The requested interface configuration.<br>(for "_get") The requested interface configuration.
 *      mapping_info - (OUT) (for "_set") The requested port mapping configuration.<br>(for "_get") The requested port mapping configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_port_get(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_port_interface_info_t *interface_info,
    bcm_compat6520_port_mapping_info_t *mapping_info)
{
    int rv = BCM_E_NONE;
    bcm_port_mapping_info_t *new_mapping_info = NULL;

    if (mapping_info != NULL) {
        new_mapping_info = (bcm_port_mapping_info_t *)
                     sal_alloc(sizeof(bcm_port_mapping_info_t),
                     "New mapping_info");
        if (new_mapping_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_get(unit, port, flags, interface_info, new_mapping_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_port_mapping_info_t(new_mapping_info, mapping_info);

    /* Deallocate memory*/
    sal_free(new_mapping_info);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6520in_mirror_sample_profile_t
 * Purpose:
 *      Convert the bcm_mirror_sample_profile_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_mirror_sample_profile_t(
    bcm_compat6520_mirror_sample_profile_t *from,
    bcm_mirror_sample_profile_t *to)
{
    bcm_mirror_sample_profile_t_init(to);
    to->enable = from->enable;
    to->rate = from->rate;
    to->pool_count_enable = from->pool_count_enable;
    to->sample_count_enable = from->sample_count_enable;
    to->trace_enable = from->trace_enable;
    to->mirror_mode = from->mirror_mode;
}

/*
 * Function:
 *      _bcm_compat6520out_mirror_sample_profile_t
 * Purpose:
 *      Convert the bcm_mirror_sample_profile_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_mirror_sample_profile_t(
    bcm_mirror_sample_profile_t *from,
    bcm_compat6520_mirror_sample_profile_t *to)
{
    to->enable = from->enable;
    to->rate = from->rate;
    to->pool_count_enable = from->pool_count_enable;
    to->sample_count_enable = from->sample_count_enable;
    to->trace_enable = from->trace_enable;
    to->mirror_mode = from->mirror_mode;
}

/*
 * Function:
 *      bcm_compat6520_mirror_sample_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_sample_profile_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) Sample type
 *      profile_id - (IN) Sample profile id
 *      profile - (IN) Sample profile entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mirror_sample_profile_set(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6520_mirror_sample_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_sample_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_sample_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_sample_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_mirror_sample_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_sample_profile_set(unit, type, profile_id, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_mirror_sample_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_sample_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) Sample type
 *      profile_id - (IN) Sample profile id
 *      profile - (OUT) Sample profile entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_mirror_sample_profile_get(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6520_mirror_sample_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_sample_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_sample_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_sample_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_sample_profile_get(unit, type, profile_id, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_mirror_sample_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_ecn_traffic_map_info_t
 * Purpose:
 *      Convert the bcm_ecn_traffic_map_info_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_ecn_traffic_map_info_t(
    bcm_compat6520_ecn_traffic_map_info_t *from,
    bcm_ecn_traffic_map_info_t *to)
{
    bcm_ecn_traffic_map_info_t_init(to);
    to->flags = from->flags;
    to->ecn = from->ecn;
    to->int_cn = from->int_cn;
    to->tunnel_ecn = from->tunnel_ecn;
    to->tunnel_type = from->tunnel_type;
    to->type = from->type;
}

/*
 * Function:
 *      _bcm_compat6520out_ecn_traffic_map_info_t
 * Purpose:
 *      Convert the bcm_ecn_traffic_map_info_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_ecn_traffic_map_info_t(
    bcm_ecn_traffic_map_info_t *from,
    bcm_compat6520_ecn_traffic_map_info_t *to)
{
    to->flags = from->flags;
    to->ecn = from->ecn;
    to->int_cn = from->int_cn;
    to->tunnel_ecn = from->tunnel_ecn;
    to->tunnel_type = from->tunnel_type;
    to->type = from->type;
}

/*
 * Function:
 *      bcm_compat6520_ecn_traffic_map_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_map_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      map - (INOUT) Internal congestion notification map configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_ecn_traffic_map_get(
    int unit,
    bcm_compat6520_ecn_traffic_map_info_t *map)
{
    int rv = BCM_E_NONE;
    bcm_ecn_traffic_map_info_t *new_map = NULL;

    if (map != NULL) {
        new_map = (bcm_ecn_traffic_map_info_t *)
                     sal_alloc(sizeof(bcm_ecn_traffic_map_info_t),
                     "New map");
        if (new_map == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_ecn_traffic_map_info_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_map_get(unit, new_map);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_ecn_traffic_map_info_t(new_map, map);

    /* Deallocate memory*/
    sal_free(new_map);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_ecn_traffic_map_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_map_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      map - (IN) Internal congestion notification map configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_ecn_traffic_map_set(
    int unit,
    bcm_compat6520_ecn_traffic_map_info_t *map)
{
    int rv = BCM_E_NONE;
    bcm_ecn_traffic_map_info_t *new_map = NULL;

    if (map != NULL) {
        new_map = (bcm_ecn_traffic_map_info_t *)
                     sal_alloc(sizeof(bcm_ecn_traffic_map_info_t),
                     "New map");
        if (new_map == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_ecn_traffic_map_info_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_map_set(unit, new_map);


    /* Deallocate memory*/
    sal_free(new_map);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6520in_oam_loss_t
 * Purpose:
 *   Convert the bcm_oam_loss_t datatype from <=6.5.20 to
 *   SDK 6.5.20+
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_oam_loss_t(
        bcm_compat6520_oam_loss_t *from,
        bcm_oam_loss_t *to)
{
    int i1 = 0;

    bcm_oam_loss_t_init(to);
    to->flags = from->flags;
    to->loss_id = from->loss_id;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->period = from->period;
    to->loss_threshold = from->loss_threshold;
    to->loss_nearend = from->loss_nearend;
    to->loss_farend = from->loss_farend;
    to->loss_nearend_byte = from->loss_nearend_byte;
    to->loss_farend_byte = from->loss_farend_byte;
    to->loss_nearend_byte_upper = from->loss_nearend_byte_upper;
    to->loss_farend_byte_upper = from->loss_farend_byte_upper;
    to->loss_nearend_max = from->loss_nearend_max;
    to->loss_nearend_acc = from->loss_nearend_acc;
    to->loss_farend_max = from->loss_farend_max;
    to->loss_farend_acc = from->loss_farend_acc;
    to->tx_nearend = from->tx_nearend;
    to->rx_nearend = from->rx_nearend;
    to->tx_farend = from->tx_farend;
    to->rx_farend = from->rx_farend;
    to->tx_nearend_byte = from->tx_nearend_byte;
    to->rx_nearend_byte = from->rx_nearend_byte;
    to->tx_farend_byte = from->tx_farend_byte;
    to->rx_farend_byte = from->rx_farend_byte;
    to->tx_nearend_byte_upper = from->tx_nearend_byte_upper;
    to->rx_nearend_byte_upper = from->rx_nearend_byte_upper;
    to->tx_farend_byte_upper = from->tx_farend_byte_upper;
    to->rx_farend_byte_upper = from->rx_farend_byte_upper;
    to->pkt_pri_bitmap = from->pkt_pri_bitmap;
    to->pkt_dp_bitmap = from->pkt_dp_bitmap;
    to->pkt_pri = from->pkt_pri;
    to->int_pri = from->int_pri;
    to->gport = from->gport;
    to->rx_oam_packets = from->rx_oam_packets;
    to->tx_oam_packets = from->tx_oam_packets;
    for (i1 = 0; i1 < 6; i1++) {
        to->peer_da_mac_address[i1] = from->peer_da_mac_address[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_base_id[i1] = from->lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_offset[i1] = from->lm_counter_base_id[i1];
    }
        for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_action[i1] = from->lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_byte_offset[i1] = from->lm_counter_base_id[i1];
    }
    to->lm_counter_size = from->lm_counter_size;
    to->pm_id = from->pm_id;
    to->measurement_period = from->measurement_period;
    to->slm_counter_base_id = from->slm_counter_base_id;
    to->slm_counter_core_id = from->slm_counter_core_id;
}

/*
 * Function:
 *   _bcm_compat6520out_oam_loss_t
 * Purpose:
 *   Convert the bcm_oam_loss_t datatype from 6.5.20+ to
 *   <=6.5.20
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype
 *   to       - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_oam_loss_t(
        bcm_oam_loss_t *from,
        bcm_compat6520_oam_loss_t *to)
{
    int i1 = 0;
 
    to->flags = from->flags;
    to->loss_id = from->loss_id;
    to->id = from->id;
    to->remote_id = from->remote_id;
    to->period = from->period;
    to->loss_threshold = from->loss_threshold;
    to->loss_nearend = from->loss_nearend;
    to->loss_farend = from->loss_farend;
    to->loss_nearend_byte = from->loss_nearend_byte;
    to->loss_farend_byte = from->loss_farend_byte;
    to->loss_nearend_byte_upper = from->loss_nearend_byte_upper;
    to->loss_farend_byte_upper = from->loss_farend_byte_upper;
    to->loss_nearend_max = from->loss_nearend_max;
    to->loss_nearend_acc = from->loss_nearend_acc;
    to->loss_farend_max = from->loss_farend_max;
    to->loss_farend_acc = from->loss_farend_acc;
    to->tx_nearend = from->tx_nearend;
    to->rx_nearend = from->rx_nearend;
    to->tx_farend = from->tx_farend;
    to->rx_farend = from->rx_farend;
    to->tx_nearend_byte = from->tx_nearend_byte;
    to->rx_nearend_byte = from->rx_nearend_byte;
    to->tx_farend_byte = from->tx_farend_byte;
    to->rx_farend_byte = from->rx_farend_byte;
    to->tx_nearend_byte_upper = from->tx_nearend_byte_upper;
    to->rx_nearend_byte_upper = from->rx_nearend_byte_upper;
    to->tx_farend_byte_upper = from->tx_farend_byte_upper;
    to->rx_farend_byte_upper = from->rx_farend_byte_upper;
    to->pkt_pri_bitmap = from->pkt_pri_bitmap;
    to->pkt_dp_bitmap = from->pkt_dp_bitmap;
    to->pkt_pri = from->pkt_pri;
    to->int_pri = from->int_pri;
    to->gport = from->gport;
    to->rx_oam_packets = from->rx_oam_packets;
    to->tx_oam_packets = from->tx_oam_packets;
    for (i1 = 0; i1 < 6; i1++) {
        to->peer_da_mac_address[i1] = from->peer_da_mac_address[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_base_id[i1] = from->lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_offset[i1] = from->lm_counter_base_id[i1];
    }
        for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_action[i1] = from->lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->lm_counter_byte_offset[i1] = from->lm_counter_base_id[i1];
    }
    to->lm_counter_size = from->lm_counter_size;
    to->pm_id = from->pm_id;
    to->measurement_period = from->measurement_period;
    to->slm_counter_base_id = from->slm_counter_base_id;
    to->slm_counter_core_id = from->slm_counter_core_id;
}

/*
 * Function:
 *   bcm_compat6520_oam_loss_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_loss_add
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   oam_loss       - (INOUT) Loss information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_oam_loss_add(
        int unit,
        bcm_compat6520_oam_loss_t *oam_loss)
{
    int rv = 0;
    bcm_oam_loss_t *new_oam_loss = NULL;

    if (oam_loss != NULL) {
        new_oam_loss = (bcm_oam_loss_t*)
            sal_alloc(sizeof(bcm_oam_loss_t), "New loss info");
        if (new_oam_loss == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6520in_oam_loss_t(oam_loss, new_oam_loss);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_loss_add(unit, new_oam_loss);

    if (rv >= 0) {
        _bcm_compat6520out_oam_loss_t(new_oam_loss, oam_loss);
    }

    sal_free(new_oam_loss);
    return rv;
}

/*
 * Function:
 *   bcm_compat6520_oam_loss_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_loss_get
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   oam_loss       - (INOUT) Loss information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_oam_loss_get(
        int unit,
        bcm_compat6520_oam_loss_t *oam_loss)
{
    int rv = 0;
    bcm_oam_loss_t *new_oam_loss = NULL;

    if (oam_loss != NULL) {
        new_oam_loss = (bcm_oam_loss_t*)
            sal_alloc(sizeof(bcm_oam_loss_t), "New loss info");
        if (new_oam_loss == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6520in_oam_loss_t(oam_loss, new_oam_loss);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_loss_get(unit, new_oam_loss);

    if (rv >= 0) {
        _bcm_compat6520out_oam_loss_t(new_oam_loss, oam_loss);
    }

    sal_free(new_oam_loss);
    return rv;
}

/*
 * Function:
 *   bcm_compat6520_oam_loss_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_loss_delete
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   oam_loss       - (INOUT) Loss information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_oam_loss_delete(
        int unit,
        bcm_compat6520_oam_loss_t *oam_loss)
{
    int rv = 0;
    bcm_oam_loss_t *new_oam_loss = NULL;

    if (oam_loss != NULL) {
        new_oam_loss = (bcm_oam_loss_t*)
            sal_alloc(sizeof(bcm_oam_loss_t), "New loss info");
        if (new_oam_loss == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6520in_oam_loss_t(oam_loss, new_oam_loss);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_loss_delete(unit, new_oam_loss);

    if (rv >= 0) {
        _bcm_compat6520out_oam_loss_t(new_oam_loss, oam_loss);
    }

    sal_free(new_oam_loss);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6520in_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from <=6.5.20 to
 *   SDK 6.5.20+.
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype.
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_mirror_destination_t(
    bcm_compat6520_mirror_destination_t *from,
    bcm_mirror_destination_t *to)
{
    bcm_mirror_destination_t_init(to);
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    sal_memcpy(to->src6_addr, from->src6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->dst6_addr, from->dst6_addr, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    to->tpid = from->tpid;
    to->vlan_id = from->vlan_id;
    to->trill_src_name = from->trill_src_name;
    to->trill_dst_name = from->trill_dst_name;
    to->trill_hopcount = from->trill_hopcount;
    to->niv_src_vif = from->niv_src_vif;
    to->niv_dst_vif = from->niv_dst_vif;
    to->niv_flags = from->niv_flags;
    to->gre_protocol = from->gre_protocol;
    to->policer_id = from->policer_id;
    to->stat_id = from->stat_id;
    to->stat_id2 = from->stat_id2;
    to->encap_id = from->encap_id;
    to->tunnel_id = from->tunnel_id;
    to->span_id = from->span_id;
    to->pkt_prio = from->pkt_prio;
    to->sample_rate_dividend = from->sample_rate_dividend;
    to->sample_rate_divisor = from->sample_rate_divisor;
    to->int_pri = from->int_pri;
    to->etag_src_vid = from->etag_src_vid;
    to->etag_dst_vid = from->etag_dst_vid;
    to->udp_src_port = from->udp_src_port;
    to->udp_dst_port = from->udp_dst_port;
    to->egress_sample_rate_dividend = from->egress_sample_rate_dividend;
    to->egress_sample_rate_divisor = from->egress_sample_rate_divisor;
    to->recycle_context = from->recycle_context;
    to->packet_copy_size = from->packet_copy_size;
    to->egress_packet_copy_size = from->egress_packet_copy_size;
    to->packet_control_updates = from->packet_control_updates;
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    sal_memcpy(to->ext_stat_id, from->ext_stat_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    to->erspan_header = from->erspan_header;
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    sal_memcpy(to->ext_stat_type_id, from->ext_stat_type_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->ipfix_version = from->ipfix_version;
    to->psamp_epoch = from->psamp_epoch;
    to->cosq = from->cosq;
    to->cfi = from->cfi;
    to->timestamp_mode = from->timestamp_mode;
    to->multi_dip_group = from->multi_dip_group;
    to->drop_group_bmp = from->drop_group_bmp;
    to->second_pass_src_port = from->second_pass_src_port;
    to->encap_truncate_mode = from->encap_truncate_mode;
    to->encap_truncate_profile_id = from->encap_truncate_profile_id;
    to->loopback_header_type = from->loopback_header_type;
    to->second_pass_dst_port = from->second_pass_dst_port;
    to->int_probe_header = from->int_probe_header;
    to->duplicate_pri = from->duplicate_pri;
    to->ip_proto = from->ip_proto;
}

/*
 * Function:
 *   _bcm_compat6520out_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from 6.5.20+ to
 *   <=6.5.20.
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype.
 *   to       - (OUT) The <=6.5.20 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_mirror_destination_t(
    bcm_mirror_destination_t *from,
    bcm_compat6520_mirror_destination_t *to)
{
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    sal_memcpy(to->src6_addr, from->src6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->dst6_addr, from->dst6_addr, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    to->tpid = from->tpid;
    to->vlan_id = from->vlan_id;
    to->trill_src_name = from->trill_src_name;
    to->trill_dst_name = from->trill_dst_name;
    to->trill_hopcount = from->trill_hopcount;
    to->niv_src_vif = from->niv_src_vif;
    to->niv_dst_vif = from->niv_dst_vif;
    to->niv_flags = from->niv_flags;
    to->gre_protocol = from->gre_protocol;
    to->policer_id = from->policer_id;
    to->stat_id = from->stat_id;
    to->stat_id2 = from->stat_id2;
    to->encap_id = from->encap_id;
    to->tunnel_id = from->tunnel_id;
    to->span_id = from->span_id;
    to->pkt_prio = from->pkt_prio;
    to->sample_rate_dividend = from->sample_rate_dividend;
    to->sample_rate_divisor = from->sample_rate_divisor;
    to->int_pri = from->int_pri;
    to->etag_src_vid = from->etag_src_vid;
    to->etag_dst_vid = from->etag_dst_vid;
    to->udp_src_port = from->udp_src_port;
    to->udp_dst_port = from->udp_dst_port;
    to->egress_sample_rate_dividend = from->egress_sample_rate_dividend;
    to->egress_sample_rate_divisor = from->egress_sample_rate_divisor;
    to->recycle_context = from->recycle_context;
    to->packet_copy_size = from->packet_copy_size;
    to->egress_packet_copy_size = from->egress_packet_copy_size;
    to->packet_control_updates = from->packet_control_updates;
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    sal_memcpy(to->ext_stat_id, from->ext_stat_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    to->erspan_header = from->erspan_header;
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    sal_memcpy(to->ext_stat_type_id, from->ext_stat_type_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->ipfix_version = from->ipfix_version;
    to->psamp_epoch = from->psamp_epoch;
    to->cosq = from->cosq;
    to->cfi = from->cfi;
    to->timestamp_mode = from->timestamp_mode;
    to->multi_dip_group = from->multi_dip_group;
    to->drop_group_bmp = from->drop_group_bmp;
    to->second_pass_src_port = from->second_pass_src_port;
    to->encap_truncate_mode = from->encap_truncate_mode;
    to->encap_truncate_profile_id = from->encap_truncate_profile_id;
    to->loopback_header_type = from->loopback_header_type;
    to->second_pass_dst_port = from->second_pass_dst_port;
    to->int_probe_header = from->int_probe_header;
    to->duplicate_pri = from->duplicate_pri;
    to->ip_proto = from->ip_proto;
}

/*
 * Function:
 *   bcm_compat6520_mirror_destination_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_destination_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   mirror_dest    - (INOUT) Mirrored destination and encapsulation.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6520_mirror_destination_create(
    int unit,
    bcm_compat6520_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6520in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_create(unit, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6520out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *   bcm_compat6520_mirror_destination_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_destination_get.
 * Parameters:
 *   unit             - (IN) BCM device number.
 *   mirror_dest_id   - (IN) Mirrored destination ID.
 *   mirror_dest      - (INOUT) Matching Mirrored destination descriptor.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6520_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6520_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6520in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_get(unit, mirror_dest_id, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6520out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6520in_oam_pm_stats_t
 * Purpose:
 *   Convert the bcm_oam_pm_stats_t datatype from <=6.5.20 to
 *   SDK 6.5.20+
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_oam_pm_stats_t(
        bcm_compat6520_oam_pm_stats_t *from,
        bcm_oam_pm_stats_t *to)
{
    int i1 = 0;

    sal_memset(to, 0, sizeof(*to));
    to->far_loss_min = from->far_loss_min;
    to->far_tx_min = from->far_tx_min;
    to->far_loss_max = from->far_loss_max;
    to->far_tx_max = from->far_tx_max;
    to->far_loss = from->far_loss;
    to->near_loss_min = from->near_loss_min;
    to->near_tx_min = from->near_tx_min;
    to->near_loss_max = from->near_loss_max;
    to->near_tx_max = from->near_tx_max;
    to->near_loss = from->near_loss;
    to->lm_tx_count = from->lm_tx_count;
    to->DM_min = from->DM_min;
    to->DM_max = from->DM_max;
    to->DM_avg = from->DM_avg;
    to->dm_tx_count = from->dm_tx_count;
    to->profile_id = from->profile_id;
    for (i1 = 0; i1 < BCM_OAM_MAX_PM_PROFILE_BIN_EDGES; i1++) {
        to->bin_counters[i1] = from->bin_counters[i1];
    }
    to->lm_rx_count = from->lm_rx_count;
    to->dm_rx_count = from->dm_rx_count;
    to->far_total_tx_pkt_count = from->far_total_tx_pkt_count;
    to->near_total_tx_pkt_count = from->near_total_tx_pkt_count;
    to->flags = from->flags;
}

/*
 * Function:
 *   _bcm_compat6520out_oam_pm_stats_t
 * Purpose:
 *   Convert the bcm_oam_pm_stats_t datatype from 6.5.20+ to
 *   <=6.5.20
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype
 *   to       - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_oam_pm_stats_t(
        bcm_oam_pm_stats_t *from,
        bcm_compat6520_oam_pm_stats_t *to)
{
    int i1 = 0;

    to->far_loss_min = from->far_loss_min;
    to->far_tx_min = from->far_tx_min;
    to->far_loss_max = from->far_loss_max;
    to->far_tx_max = from->far_tx_max;
    to->far_loss = from->far_loss;
    to->near_loss_min = from->near_loss_min;
    to->near_tx_min = from->near_tx_min;
    to->near_loss_max = from->near_loss_max;
    to->near_tx_max = from->near_tx_max;
    to->near_loss = from->near_loss;
    to->lm_tx_count = from->lm_tx_count;
    to->DM_min = from->DM_min;
    to->DM_max = from->DM_max;
    to->DM_avg = from->DM_avg;
    to->dm_tx_count = from->dm_tx_count;
    to->profile_id = from->profile_id;
    for (i1 = 0; i1 < BCM_OAM_MAX_PM_PROFILE_BIN_EDGES; i1++) {
        to->bin_counters[i1] = from->bin_counters[i1];
    }
    to->lm_rx_count = from->lm_rx_count;
    to->dm_rx_count = from->dm_rx_count;
    to->far_total_tx_pkt_count = from->far_total_tx_pkt_count;
    to->near_total_tx_pkt_count = from->near_total_tx_pkt_count;
    to->flags = from->flags;
}

/*
 * Function:
 *   bcm_compat6520_oam_loss_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_pm_stats_get
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   endpoint_id    - (IN) Endpoint ID.
 *   pm_stats       - (OUT) Stats information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_oam_pm_stats_get(
        int unit,
        bcm_oam_endpoint_t endpoint_id,
        bcm_compat6520_oam_pm_stats_t *pm_stats)
{
    int rv = 0;
    bcm_oam_pm_stats_t *new_stats = NULL;

    if (pm_stats != NULL) {
        new_stats = (bcm_oam_pm_stats_t*)
            sal_alloc(sizeof(bcm_oam_pm_stats_t), "New loss info");
        if (new_stats == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6520in_oam_pm_stats_t(pm_stats, new_stats);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_pm_stats_get(unit, endpoint_id, new_stats);

    if (rv >= 0) {
        _bcm_compat6520out_oam_pm_stats_t(new_stats, pm_stats);
    }

    sal_free(new_stats);
    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6520in_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_flow_encap_config_t(
    bcm_compat6520_flow_encap_config_t *from,
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
}

/*
 * Function:
 *      _bcm_compat6520out_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_flow_encap_config_t(
    bcm_flow_encap_config_t *from,
    bcm_compat6520_flow_encap_config_t *to)
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
}

/*
 * Function:
 *      bcm_compat6520_flow_encap_add
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
int bcm_compat6520_flow_encap_add(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
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
 *      bcm_compat6520_flow_encap_delete
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
int bcm_compat6520_flow_encap_delete(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
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
 *      bcm_compat6520_flow_encap_get
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
int bcm_compat6520_flow_encap_get(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_flow_encap_config_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_flow_encap_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flow_encap_flexctr_object_set(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
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
 *      bcm_compat6520_flow_encap_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flow_encap_flexctr_object_get(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
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
 *      bcm_compat6520_flow_encap_stat_attach
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
int bcm_compat6520_flow_encap_stat_attach(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
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
 *      bcm_compat6520_flow_encap_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flow_encap_stat_detach(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
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
 *      bcm_compat6520_flow_encap_stat_id_get
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
int bcm_compat6520_flow_encap_stat_id_get(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
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
        _bcm_compat6520in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6520in_field_destination_match_t
 * Purpose:
 *      Convert the bcm_field_destination_match_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_field_destination_match_t(
    bcm_compat6520_field_destination_match_t *from,
    bcm_field_destination_match_t *to)
{
    bcm_field_destination_match_t_init(to);
    to->priority = from->priority;
    to->gport = from->gport;
    to->gport_mask = from->gport_mask;
    to->mc_group = from->mc_group;
    to->mc_group_mask = from->mc_group_mask;
    to->port_encap = from->port_encap;
    to->port_encap_mask = from->port_encap_mask;
    to->color = from->color;
    to->color_mask = from->color_mask;
    to->elephant_color = from->elephant_color;
    to->elephant_color_mask = from->elephant_color_mask;
    to->elephant = from->elephant;
    to->elephant_mask = from->elephant_mask;
    to->elephant_notify = from->elephant_notify;
    to->elephant_notify_mask = from->elephant_notify_mask;
    to->int_pri = from->int_pri;
    to->int_pri_mask = from->int_pri_mask;
    to->opaque_object_2 = from->opaque_object_2;
    to->opaque_object_2_mask = from->opaque_object_2_mask;
    to->ifp_opaque_ctrl_id = from->ifp_opaque_ctrl_id;
    to->ifp_opaque_ctrl_id_mask = from->ifp_opaque_ctrl_id_mask;
}

/*
 * Function:
 *      _bcm_compat6520out_field_destination_match_t
 * Purpose:
 *      Convert the bcm_field_destination_match_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_field_destination_match_t(
    bcm_field_destination_match_t *from,
    bcm_compat6520_field_destination_match_t *to)
{
    to->priority = from->priority;
    to->gport = from->gport;
    to->gport_mask = from->gport_mask;
    to->mc_group = from->mc_group;
    to->mc_group_mask = from->mc_group_mask;
    to->port_encap = from->port_encap;
    to->port_encap_mask = from->port_encap_mask;
    to->color = from->color;
    to->color_mask = from->color_mask;
    to->elephant_color = from->elephant_color;
    to->elephant_color_mask = from->elephant_color_mask;
    to->elephant = from->elephant;
    to->elephant_mask = from->elephant_mask;
    to->elephant_notify = from->elephant_notify;
    to->elephant_notify_mask = from->elephant_notify_mask;
    to->int_pri = from->int_pri;
    to->int_pri_mask = from->int_pri_mask;
    to->opaque_object_2 = from->opaque_object_2;
    to->opaque_object_2_mask = from->opaque_object_2_mask;
    to->ifp_opaque_ctrl_id = from->ifp_opaque_ctrl_id;
    to->ifp_opaque_ctrl_id_mask = from->ifp_opaque_ctrl_id_mask;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_entry_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_entry_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Entry operation options
 *      match - (IN) Field destination match
 *      action - (IN) Field destination action
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_entry_add(
    int unit,
    uint32 options,
    bcm_compat6520_field_destination_match_t *match,
    bcm_field_destination_action_t *action)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_add(unit, options, new_match, action);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_entry_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_entry_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (INOUT) Field destination match
 *      action - (OUT) Field destination action
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_entry_get(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    bcm_field_destination_action_t *action)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_get(unit, new_match, action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_field_destination_match_t(new_match, match);

    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_entry_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_entry_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_entry_delete(
    int unit,
    bcm_compat6520_field_destination_match_t *match)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_delete(unit, new_match);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      stat_counter_id - (IN) Statistic counter ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_stat_attach(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_attach(unit, new_match, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      stat_counter_id - (OUT) Statistic counter ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_stat_id_get(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_id_get(unit, new_match, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_stat_detach(
    int unit,
    bcm_compat6520_field_destination_match_t *match)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_detach(unit, new_match);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      value - (IN) Field destination match
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_flexctr_object_set(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_flexctr_object_set(unit, new_match, value);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_destination_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      value - (OUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_destination_flexctr_object_get(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_flexctr_object_get(unit, new_match, value);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6520in_flexstate_index_operation_t
 * Purpose:
 *   Convert the bcm_flexstate_index_operation_t datatype from <=6.5.20 to
 *   SDK 6.5.20+.
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype.
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flexstate_index_operation_t(
    bcm_compat6520_flexstate_index_operation_t *from,
    bcm_flexstate_index_operation_t *to)
{
    int i = 0;

    for (i = 0; i < BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
}

/*
 * Function:
 *   _bcm_compat6520out_flexstate_index_operation_t
 * Purpose:
 *   Convert the bcm_flexstate_index_operation_t datatype from 6.5.20+ to
 *   <=6.5.20.
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype.
 *   to       - (OUT) The <=6.5.20 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flexstate_index_operation_t(
    bcm_flexstate_index_operation_t *from,
    bcm_compat6520_flexstate_index_operation_t *to)
{
    int i = 0;

    for (i = 0; i < BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
}

/*
 * Function:
 *   _bcm_compat6520in_flexstate_value_operation_t
 * Purpose:
 *   Convert the bcm_flexstate_value_operation_t datatype from <=6.5.20 to
 *   SDK 6.5.20+.
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype.
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flexstate_value_operation_t(
    bcm_compat6520_flexstate_value_operation_t *from,
    bcm_flexstate_value_operation_t *to)
{
    int i = 0;

    to->select = from->select;
    for (i = 0; i < BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
    to->type = from->type;
}

/*
 * Function:
 *   _bcm_compat6520out_flexstate_value_operation_t
 * Purpose:
 *   Convert the bcm_flexstate_value_operation_t datatype from 6.5.20+ to
 *   <=6.5.20.
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype.
 *   to       - (OUT) The <=6.5.20 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flexstate_value_operation_t(
    bcm_flexstate_value_operation_t *from,
    bcm_compat6520_flexstate_value_operation_t *to)
{
    int i = 0;

    to->select = from->select;
    for (i = 0; i < BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
    to->type = from->type;
}

/*
 * Function:
 *   _bcm_compat6520in_flexstate_trigger_t
 * Purpose:
 *   Convert the bcm_flexstate_trigger_t datatype from <=6.5.20 to
 *   SDK 6.5.20+.
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype.
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flexstate_trigger_t(
    bcm_compat6520_flexstate_trigger_t *from,
    bcm_flexstate_trigger_t *to)
{
    to->trigger_type = from->trigger_type;
    to->period_num = from->period_num;
    to->interval = from->interval;
    to->object = from->object;
    to->object_value_start = from->object_value_start;
    to->object_value_stop = from->object_value_stop;
    to->object_value_mask = from->object_value_mask;
}

/*
 * Function:
 *   _bcm_compat6520out_flexstate_trigger_t
 * Purpose:
 *   Convert the bcm_flexstate_trigger_t datatype from 6.5.20+ to
 *   <=6.5.20.
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype.
 *   to       - (OUT) The <=6.5.20 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flexstate_trigger_t(
    bcm_flexstate_trigger_t *from,
    bcm_compat6520_flexstate_trigger_t *to)
{
    to->trigger_type = from->trigger_type;
    to->period_num = from->period_num;
    to->interval = from->interval;
    to->object = from->object;
    to->object_value_start = from->object_value_start;
    to->object_value_stop = from->object_value_stop;
    to->object_value_mask = from->object_value_mask;
}

/*
 * Function:
 *   _bcm_compat6520in_flexstate_operation_t
 * Purpose:
 *   Convert the bcm_flexstate_operation_t datatype from <=6.5.20 to
 *   SDK 6.5.20+.
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype.
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flexstate_operation_t(
    bcm_compat6520_flexstate_operation_t *from,
    bcm_flexstate_operation_t *to)
{
    to->update_compare = from->update_compare;
    _bcm_compat6520in_flexstate_value_operation_t(&from->compare_operation,
                                                  &to->compare_operation);
    _bcm_compat6520in_flexstate_value_operation_t(&from->operation_true,
                                                  &to->operation_true);
    _bcm_compat6520in_flexstate_value_operation_t(&from->operation_false,
                                                  &to->operation_false);
}

/*
 * Function:
 *   _bcm_compat6520out_flexstate_operation_t
 * Purpose:
 *   Convert the bcm_flexstate_operation_t datatype from 6.5.20+ to
 *   <=6.5.20.
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype.
 *   to       - (OUT) The <=6.5.20 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flexstate_operation_t(
    bcm_flexstate_operation_t *from,
    bcm_compat6520_flexstate_operation_t *to)
{
    to->update_compare = from->update_compare;
    _bcm_compat6520out_flexstate_value_operation_t(&from->compare_operation,
                                                   &to->compare_operation);
    _bcm_compat6520out_flexstate_value_operation_t(&from->operation_true,
                                                   &to->operation_true);
    _bcm_compat6520out_flexstate_value_operation_t(&from->operation_false,
                                                   &to->operation_false);
}

/*
 * Function:
 *   _bcm_compat6520in_flexstate_action_t
 * Purpose:
 *   Convert the bcm_flexstate_action_t datatype from <=6.5.20 to
 *   SDK 6.5.20+.
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype.
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flexstate_action_t(
    bcm_compat6520_flexstate_action_t *from,
    bcm_flexstate_action_t *to)
{
    bcm_flexstate_action_t_init(to);
    to->flags = from->flags;
    to->source = from->source;
    to->ports = from->ports;
    to->hint = from->hint;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    _bcm_compat6520in_flexstate_index_operation_t(&from->index_operation,
                                                  &to->index_operation);
    _bcm_compat6520in_flexstate_operation_t(&from->operation_a,
                                            &to->operation_a);
    _bcm_compat6520in_flexstate_operation_t(&from->operation_b,
                                            &to->operation_b);
    _bcm_compat6520in_flexstate_trigger_t(&from->trigger, &to->trigger);
}

/*
 * Function:
 *   _bcm_compat6520out_flexstate_action_t
 * Purpose:
 *   Convert the bcm_flexstate_action_t datatype from 6.5.20+ to
 *   <=6.5.20.
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype.
 *   to       - (OUT) The <=6.5.20 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flexstate_action_t(
    bcm_flexstate_action_t *from,
    bcm_compat6520_flexstate_action_t *to)
{
    to->flags = from->flags;
    to->source = from->source;
    to->ports = from->ports;
    to->hint = from->hint;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    _bcm_compat6520out_flexstate_index_operation_t(&from->index_operation,
                                                   &to->index_operation);
    _bcm_compat6520out_flexstate_operation_t(&from->operation_a,
                                             &to->operation_a);
    _bcm_compat6520out_flexstate_operation_t(&from->operation_b,
                                             &to->operation_b);
    _bcm_compat6520out_flexstate_trigger_t(&from->trigger, &to->trigger);
}

/*
 * Function:
 *   bcm_compat6520_flexstate_action_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexstate_action_create.
 * Parameters:
 *   param [in] unit Unit number.
 *   param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 *   param [in] action Flex state action data structure.
 *   param [in/out] action_id Flex state ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6520_flexstate_action_create(
    int unit,
    int options,
    bcm_compat6520_flexstate_action_t *action,
    uint32 *action_id)
{
    int rv = BCM_E_NONE;
    bcm_flexstate_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_flexstate_action_t *)
                     sal_alloc(sizeof(bcm_flexstate_action_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flexstate_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexstate_action_create(unit, options, new_action, action_id);

    if (rv >= 0) {
        _bcm_compat6520out_flexstate_action_t(new_action, action);
    }

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flexstate_action_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexstate_action_get.
 * Parameters:
 *   param [in] unit Unit number.
 *   param [in] action_id Flex state ID.
 *   param [out] action Flex state action data structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6520_flexstate_action_get(
    int unit,
    uint32 action_id,
    bcm_compat6520_flexstate_action_t *action)
{
    int rv = BCM_E_NONE;
    bcm_flexstate_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_flexstate_action_t *)
                     sal_alloc(sizeof(bcm_flexstate_action_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flexstate_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexstate_action_get(unit, action_id, new_action);

    if (rv >= 0) {
        _bcm_compat6520out_flexstate_action_t(new_action, action);
    }

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6520in_flexstate_quantization_t
 * Purpose:
 *   Convert the bcm_flexstate_quantization_t datatype from <=6.5.20 to
 *   SDK 6.5.20+.
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype.
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flexstate_quantization_t(
    bcm_compat6520_flexstate_quantization_t *from,
    bcm_flexstate_quantization_t *to)
{
    int i = 0;

    bcm_flexstate_quantization_t_init(to);
    to->object = from->object;
    to->ports = from->ports;
    for (i = 0; i < BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }
    to->range_num = from->range_num;
}

/*
 * Function:
 *   _bcm_compat6520out_flexstate_quantization_t
 * Purpose:
 *   Convert the bcm_flexstate_quantization_t datatype from 6.5.20+ to
 *   <=6.5.20.
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype.
 *   to       - (OUT) The <=6.5.20 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flexstate_quantization_t(
    bcm_flexstate_quantization_t *from,
    bcm_compat6520_flexstate_quantization_t *to)
{
    int i = 0;

    to->object = from->object;
    to->ports = from->ports;
    for (i = 0; i < BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }
    to->range_num = from->range_num;
}

/*
 * Function:
 *   bcm_compat6520_flexstate_quantization_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexstate_quantization_create.
 * Parameters:
 *   param [in] unit Unit number.
 *   param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 *   param [in] quantization Flex state quantization data structure.
 *   param [in/out] quant_id Flex state quantization ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6520_flexstate_quantization_create(
    int unit,
    int options,
    bcm_compat6520_flexstate_quantization_t *quantization,
    uint32 *quant_id)
{
    int rv = BCM_E_NONE;
    bcm_flexstate_quantization_t *new_quantization = NULL;

    if (quantization != NULL) {
        new_quantization = (bcm_flexstate_quantization_t *)
                           sal_alloc(sizeof(bcm_flexstate_quantization_t),
                           "New quantization");
        if (new_quantization == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flexstate_quantization_t(quantization,
                                                   new_quantization);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexstate_quantization_create(unit, options, new_quantization,
                                           quant_id);

    if (rv >= 0) {
        _bcm_compat6520out_flexstate_quantization_t(new_quantization,
                                                    quantization);
    }

    /* Deallocate memory*/
    sal_free(new_quantization);

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flexstate_quantization_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexstate_quantization_get.
 * Parameters:
 *   param [in] unit Unit number.
 *   param [in] quant_id Flex state quantization ID.
 *   param [out] quantization Flex state quantization data structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6520_flexstate_quantization_get(
    int unit,
    uint32 quant_id,
    bcm_compat6520_flexstate_quantization_t *quantization)
{
    int rv = BCM_E_NONE;
    bcm_flexstate_quantization_t *new_quantization = NULL;

    if (quantization != NULL) {
        new_quantization = (bcm_flexstate_quantization_t *)
                           sal_alloc(sizeof(bcm_flexstate_quantization_t),
                           "New quantization");
        if (new_quantization == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flexstate_quantization_t(quantization,
                                                   new_quantization);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexstate_quantization_get(unit, quant_id, new_quantization);

    if (rv >= 0) {
        _bcm_compat6520out_flexstate_quantization_t(new_quantization,
                                                    quantization);
    }

    /* Deallocate memory*/
    sal_free(new_quantization);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_flowtracker_tracking_param_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_tracking_param_info_t datatype
 *      from <=6.5.20 to SDK 6.5.20+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_flowtracker_tracking_param_info_t(
    bcm_compat6520_flowtracker_tracking_param_info_t *from,
    bcm_flowtracker_tracking_param_info_t *to)
{
    bcm_flowtracker_tracking_param_info_t_init(to);
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->check_id                 = from->check_id;
    to->src_ts                   = from->src_ts;
    to->udf_id                   = from->udf_id;
    sal_memcpy(to->mask, from->mask, BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    sal_memcpy(to->tracking_data, from->tracking_data,
                    BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    to->direction                = from->direction;
    to->custom_id                = from->custom_id;
    return;
}

/*
 * Function:
 *      _bcm_compat6520out_flowtracker_tracking_param_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_tracking_param_info_t datatype from
 *      6.5.20+ to SDK <=6.5.20
 * Parameters:
 *      from        - (IN) The 6.5.20+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_flowtracker_tracking_param_info_t(
    bcm_flowtracker_tracking_param_info_t *from,
    bcm_compat6520_flowtracker_tracking_param_info_t *to)
{
    to->flags                    = from->flags;
    to->param                    = from->param;
    to->check_id                 = from->check_id;
    to->src_ts                   = from->src_ts;
    to->udf_id                   = from->udf_id;
    sal_memcpy(to->mask, from->mask, BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    sal_memcpy(to->tracking_data, from->tracking_data,
                    BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);
    to->direction                 = from->direction;
    to->custom_id                 = from->custom_id;
    return;
}

/*
 * Function:
 *   _bcm_compat6520in_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from <=6.5.20 to
 *   SDK 6.5.20+
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flowtracker_check_info_t(
        bcm_compat6520_flowtracker_check_info_t *from,
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
}

/*
 * Function:
 *   _bcm_compat6520out_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from 6.5.20+ to
 *   <=6.5.20
 * Parameters:
 *   from        - (IN) The 6.5.20+ version of the datatype
 *   to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flowtracker_check_info_t(
        bcm_flowtracker_check_info_t *from,
        bcm_compat6520_flowtracker_check_info_t *to)
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
}

/*
 * Function:
 *   _bcm_compat6520in_flowtracker_export_element_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_export_element_info_t datatype from <=6.5.20 to
 *   SDK 6.5.20+
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flowtracker_export_element_info_t(
        bcm_compat6520_flowtracker_export_element_info_t *from,
        bcm_flowtracker_export_element_info_t *to)
{
    sal_memset(to, 0, sizeof(bcm_flowtracker_export_element_info_t));
    to->flags = from->flags;
    to->element = from->element;
    to->data_size = from->data_size;
    to->info_elem_id = from->info_elem_id;
    to->check_id = from->check_id;
    to->direction = from->direction;
    to->custom_id = from->custom_id;
}

/*
 * Function:
 *   _bcm_compat6520out_flowtracker_export_element_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_export_element_info_t datatype from 6.5.20+ to
 *   <=6.5.20
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype
 *   to       - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flowtracker_export_element_info_t(
        bcm_flowtracker_export_element_info_t *from,
        bcm_compat6520_flowtracker_export_element_info_t *to)
{
    to->flags = from->flags;
    to->element = from->element;
    to->data_size = from->data_size;
    to->info_elem_id = from->info_elem_id;
    to->check_id = from->check_id;
    to->direction = from->direction;
    to->custom_id = from->custom_id;
}

/*
 * Function:
 *   _bcm_compat6520in_flowtracker_export_record_data_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_export_record_data_info_t datatype from
 *   <=6.5.20 to SDK 6.5.20+
 * Parameters:
 *   from        - (IN) The <=6.5.20 version of the datatype
 *   to          - (OUT) The SDK 6.5.20+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520in_flowtracker_export_record_data_info_t(
        bcm_compat6520_flowtracker_export_record_data_info_t *from,
        bcm_flowtracker_export_record_data_info_t *to)
{
    _bcm_compat6520in_flowtracker_export_element_info_t(
            &from->export_element_info, &to->export_element_info);
    sal_memcpy(&to->data[0], &from->data[0],
            sizeof (uint8) * BCM_FLOWTRACKER_EXPORT_RECORD_DATA_MAX_LENGTH);
}

/*
 * Function:
 *   _bcm_compat6520out_flowtracker_export_record_data_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_export_element_info_t datatype from 6.5.20+ to
 *   <=6.5.20
 * Parameters:
 *   from     - (IN) The 6.5.20+ version of the datatype
 *   to       - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6520out_flowtracker_export_record_data_info_t(
        bcm_flowtracker_export_record_data_info_t *from,
        bcm_compat6520_flowtracker_export_record_data_info_t *to)
{
    _bcm_compat6520out_flowtracker_export_element_info_t(
            &from->export_element_info, &to->export_element_info);
    sal_memcpy(&to->data[0], &from->data[0],
            sizeof (uint8) * BCM_FLOWTRACKER_EXPORT_RECORD_DATA_MAX_LENGTH);
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_group_tracking_params_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_tracking_params_set
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   group_id          - (IN)    Group Id
 *   num_tracking_params - (IN)  Numer of tracking params
 *   list_of_tracking_params - (IN) List of tracking params
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_flowtracker_group_tracking_params_set(
        int unit,
        bcm_flowtracker_group_t id,
        int num_tracking_params,
        bcm_compat6520_flowtracker_tracking_param_info_t *list_of_tracking_params)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_tp_list = NULL;

    if (list_of_tracking_params != NULL) {
        new_tp_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    num_tracking_params), "new tracking param list");
        if (new_tp_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < num_tracking_params; i++) {
            _bcm_compat6520in_flowtracker_tracking_param_info_t(
                    &list_of_tracking_params[i], &new_tp_list[i]);
        }
    }

    rv = bcm_flowtracker_group_tracking_params_set(unit, id,
            num_tracking_params, new_tp_list);

    if (new_tp_list != NULL) {
        sal_free(new_tp_list);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_group_tracking_params_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_tracking_params_get
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   group_id          - (IN)    Group Id
 *   max_size          - (IN)    size of list_of_tracking_params
 *   list_of_tracking_params - (OUT) List of tracking params
 *   list_size         - (OUT)   Actual number of entries
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_flowtracker_group_tracking_params_get(
        int unit,
        bcm_flowtracker_group_t id,
        int max_size,
        bcm_compat6520_flowtracker_tracking_param_info_t *list_of_tracking_params,
        int *list_size)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_tp_list = NULL;

    if (list_of_tracking_params != NULL) {
        new_tp_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    max_size), "new tracking param list");
        if (new_tp_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < max_size; i++) {
            _bcm_compat6520in_flowtracker_tracking_param_info_t(
                    &list_of_tracking_params[i], &new_tp_list[i]);
        }
    }

    rv = bcm_flowtracker_group_tracking_params_get(unit, id,
            max_size, new_tp_list, list_size);

    if (new_tp_list != NULL) {
        for (i = 0; i < max_size; i++) {
            _bcm_compat6520out_flowtracker_tracking_param_info_t(
                    &new_tp_list[i], &list_of_tracking_params[i]);
        }
        sal_free(new_tp_list);
    }

    return rv;

}

/*
 * Function:
 *   bcm_compat6520_flowtracker_check_create
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
int bcm_compat6520_flowtracker_check_create(
        int unit,
        uint32 options,
        bcm_compat6520_flowtracker_check_info_t check_info,
        bcm_flowtracker_check_t *check_id)
{
    int rv = 0;
    bcm_flowtracker_check_info_t new_check_info;

    _bcm_compat6520in_flowtracker_check_info_t(&check_info, &new_check_info);

    rv = bcm_flowtracker_check_create(unit, options, new_check_info, check_id);

    if (rv >= 0) {
        _bcm_compat6520out_flowtracker_check_info_t(&new_check_info, &check_info);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_check_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_get
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   check_id   - (IN) Software id of check.
 *   check_info - (OUT) check info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_flowtracker_check_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6520_flowtracker_check_info_t *check_info)
{
    int rv = 0;
    bcm_flowtracker_check_info_t *new_check_info = NULL;

    if (check_info != NULL) {
        new_check_info = (bcm_flowtracker_check_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_check_info_t), "New check info");
        if (new_check_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6520in_flowtracker_check_info_t(check_info, new_check_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_check_get(unit, check_id, new_check_info);

    if (rv >= 0) {
        _bcm_compat6520out_flowtracker_check_info_t(new_check_info, check_info);
    }

    sal_free(new_check_info);
    return rv;
}

/*
 * Function:
 *      bcm_compat6520_flowtracker_export_template_validate
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_flowtracker_export_template_validate
 *      Validate the template and return the list supported by the
 *      device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      num_in_export_elements - (IN) Number of export elements intended in the tempate.
 *      in_export_elements - (IN) List of export elements intended to be in the template.
 *      num_out_export_elements - (IN/OUT) Number of export elements in the supportable tempate.
 *      out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_compat6520_flowtracker_export_template_validate(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        int max_in_export_elements,
        bcm_compat6520_flowtracker_export_element_info_t *in_export_elements,
        int max_out_export_elements,
        bcm_compat6520_flowtracker_export_element_info_t *out_export_elements,
        int *actual_out_export_elements)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_export_element_info_t *new_in_info = NULL;
    bcm_flowtracker_export_element_info_t *new_out_info = NULL;

    if ((in_export_elements != NULL) && (max_in_export_elements > 0)) {
        new_in_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                              max_in_export_elements), "New in info");
        if (new_in_info == NULL) {
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < max_in_export_elements; iter++) {
            _bcm_compat6520in_flowtracker_export_element_info_t
                                        (&in_export_elements[iter],
                                         &new_in_info[iter]);
        }
    }
    if ((out_export_elements != NULL) && (max_out_export_elements != 0)) {
        new_out_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                              max_out_export_elements), "New out info");
        if (new_out_info == NULL) {
            if (new_in_info != NULL) {
                sal_free(new_in_info);
            }
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < max_out_export_elements; iter++) {
            _bcm_compat6520in_flowtracker_export_element_info_t
                                        (&out_export_elements[iter],
                                         &new_out_info[iter]);
        }
    }

    rv = bcm_flowtracker_export_template_validate(unit, flow_group_id,
                    max_in_export_elements, new_in_info,
                    max_out_export_elements, new_out_info,
                    actual_out_export_elements);

    if (new_in_info != NULL) {
        sal_free(new_in_info);
    }

    if (rv >= 0) {
        if (new_out_info != NULL) {
            for (iter = 0; iter < *actual_out_export_elements; iter++) {
                _bcm_compat6520out_flowtracker_export_element_info_t
                                    (&new_out_info[iter],
                                     &out_export_elements[iter]);
            }
            sal_free(new_out_info);
        }
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_export_template_create
 * Purpose:
 *   Compatibility function for RPC call to
 *   bcm_flowtracker_export_template_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Options
 *   id             - (INOUT) Template Id
 *   set_id         - (IN) Set id
 *   max_size       - (IN) Size of array 'list_of_export_elements'
 *   list_of_export_elements - (IN) Array of export element info
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_flowtracker_export_template_create(
        int unit,
        uint32 options,
        bcm_flowtracker_export_template_t *id,
        uint16 set_id,
        int num_export_elements,
        bcm_compat6520_flowtracker_export_element_info_t *list_of_export_elements)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_export_element_info_t *new_info = NULL;

    if ((list_of_export_elements != NULL) && (num_export_elements > 0)) {
        new_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                              num_export_elements), "New export elements info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < num_export_elements; iter++) {
            _bcm_compat6520in_flowtracker_export_element_info_t
                                        (&list_of_export_elements[iter],
                                         &new_info[iter]);
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_export_template_create(unit, options, id,
                                set_id, num_export_elements, new_info);

    sal_free(new_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_export_template_get
 * Purpose:
 *   Compatibility function for RPC call to
 *   bcm_flowtracker_export_template_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   id             - (IN) Template Id
 *   set_id         - (OUT) Set id
 *   max_size       - (IN) Size of array 'list_of_export_elements'
 *   list_of_export_elements - (OUT) Array of export element info
 *   list_size      - (OUT) Actual number of valid elements in
                           'list_of_export_elements'
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_flowtracker_export_template_get(
        int unit,
        bcm_flowtracker_export_template_t id,
        uint16 *set_id,
        int max_size,
        bcm_compat6520_flowtracker_export_element_info_t *list_of_export_elements,
        int *list_size)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_export_element_info_t *new_info = NULL;

    if ((list_of_export_elements != NULL) && (list_size != NULL) && (max_size > 0)) {
        new_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                                        max_size), "New export element info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_export_template_get(unit, id, set_id,
                                max_size, new_info, list_size);

    if (new_info != NULL) {
        for (iter = 0; iter < max_size; iter++) {
            _bcm_compat6520out_flowtracker_export_element_info_t
                (&new_info[iter],
                 &list_of_export_elements[iter]);
        }
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_user_entry_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_user_entry_add
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   flow_group_id           - (IN) Flowtracker group Id.
 *   options                 - (IN) Options to create user entry.
 *   num_user_entry_params   - (IN) Number of user entry params.
 *   user_entry_param_list   - (IN) List of user entry params.
 *   entry_handle            - (IN) User entry handle.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_flowtracker_user_entry_add(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        uint32 options,
        int num_user_entry_params,
        bcm_compat6520_flowtracker_tracking_param_info_t *user_entry_param_list,
        bcm_flowtracker_user_entry_handle_t *entry_handle)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_ep_list = NULL;

    if (user_entry_param_list != NULL) {
        new_ep_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    num_user_entry_params), "new tracking param list");
        if (new_ep_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < num_user_entry_params; i++) {
            _bcm_compat6520in_flowtracker_tracking_param_info_t(
                    &user_entry_param_list[i], &new_ep_list[i]);
        }
    }

    rv = bcm_flowtracker_user_entry_add(unit, flow_group_id,
            options, num_user_entry_params, new_ep_list, entry_handle);

    if (new_ep_list != NULL) {
        sal_free(new_ep_list);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_user_entry_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_user_entry_get
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   entry_handle            - (IN) User entry handle.
 *   num_user_entry_params   - (IN) Maximum Number of user entry params.
 *   user_entry_param_list   - (OUT) List of user entry params.
 *   actual_user_entry_params   - (OUT) Number of user entry params.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6520_flowtracker_user_entry_get(
        int unit,
        bcm_flowtracker_user_entry_handle_t *entry_handle,
        int num_user_entry_params,
        bcm_compat6520_flowtracker_tracking_param_info_t *user_entry_param_list,
        int *actual_user_entry_params)
{
    int rv, i;
    bcm_flowtracker_tracking_param_info_t *new_ep_list = NULL;

    if (user_entry_param_list != NULL) {
        new_ep_list = sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                    num_user_entry_params), "new tracking param list");
        if (new_ep_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < num_user_entry_params; i++) {
            _bcm_compat6520in_flowtracker_tracking_param_info_t(
                    &user_entry_param_list[i], &new_ep_list[i]);
        }
    }

    rv = bcm_flowtracker_user_entry_get(unit, entry_handle,
            num_user_entry_params, new_ep_list, actual_user_entry_params);

    if (new_ep_list != NULL) {
        for (i = 0; i < num_user_entry_params; i++) {
            _bcm_compat6520out_flowtracker_tracking_param_info_t(
                    &new_ep_list[i], &user_entry_param_list[i]);
        }
        sal_free(new_ep_list);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_ifa_collector_info_t
 * Purpose:
 *      Convert the bcm_ifa_collector_info_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_ifa_collector_info_t(
    bcm_compat6520_ifa_collector_info_t *from,
    bcm_ifa_collector_info_t *to)
{
    bcm_ifa_collector_info_t_init(to);
    to->transport_type = from->transport_type;
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_ifa_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_ifa_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_ifa_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_ifa_collector_udp_header_t));
    to->mtu = from->mtu;
}

/*
 * Function:
 *      _bcm_compat6520out_ifa_collector_info_t
 * Purpose:
 *      Convert the bcm_ifa_collector_info_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_ifa_collector_info_t(
    bcm_ifa_collector_info_t *from,
    bcm_compat6520_ifa_collector_info_t *to)
{
    to->transport_type = from->transport_type;
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_ifa_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_ifa_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_ifa_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_ifa_collector_udp_header_t));
    to->mtu = from->mtu;
}

/*
 * Function:
 *      bcm_compat6520_ifa_collector_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_ifa_collector_set.
 * Parameters:
 *      unit - (IN) BCM device number
 *      options - (IN) Options bitmap of BCM_IFA_COLLECTOR_XXX. Add and Modify collector.
 *      collector_info - (IN) Pointer to the collector information structure.
 This API creates a collector with the information present in the collector_info structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_ifa_collector_set(
    int unit,
    uint32 options,
    bcm_compat6520_ifa_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;
    bcm_ifa_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_ifa_collector_info_t *)
                     sal_alloc(sizeof(bcm_ifa_collector_info_t),
                     "New collector_info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_ifa_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ifa_collector_set(unit, options, new_collector_info);


    /* Deallocate memory*/
    sal_free(new_collector_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_ifa_collector_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_ifa_collector_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      collector_info - (OUT) Pointer to the collector information structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_ifa_collector_get(
    int unit,
    bcm_compat6520_ifa_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;
    bcm_ifa_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_ifa_collector_info_t *)
                     sal_alloc(sizeof(bcm_ifa_collector_info_t),
                     "New collector_info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ifa_collector_get(unit, new_collector_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_ifa_collector_info_t(new_collector_info, collector_info);

    /* Deallocate memory*/
    sal_free(new_collector_info);

    return rv;
}

/*
 * Function:
 *   bcm_compat6520_flowtracker_export_record_data_info_get
 * Purpose:
 *  Get the decoded data for the export record
 * Parameters:
 *  unit          - (IN) BCM device number
 *  record        - (IN) Export Record
 *  max_elements  - (IN) Size of array of export_element_info and data_info
 *  export_element_info - (IN) Export Elements used to decode record
 *  data_info     - (OUT) decoded Data info per export element
 *  count         - (OUT) Number of valid data_info
 * Returns:
 *  BCM_E_XXX   - BCM error code.
 */
int bcm_compat6520_flowtracker_export_record_data_info_get(
        int unit,
        bcm_flowtracker_export_record_t *record,
        int max_elements,
        bcm_compat6520_flowtracker_export_element_info_t *export_element_info,
        bcm_compat6520_flowtracker_export_record_data_info_t *data_info,
        int *count)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_export_element_info_t *new_export_elem_info = NULL;
    bcm_flowtracker_export_record_data_info_t *new_data_info = NULL;

    if ((export_element_info != NULL) && (max_elements > 0)) {
        new_export_elem_info = (bcm_flowtracker_export_element_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) *
                        max_elements), "New export elem info");
        if (new_export_elem_info == NULL) {
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < max_elements; iter++) {
            _bcm_compat6520in_flowtracker_export_element_info_t
                (&export_element_info[iter],
                 &new_export_elem_info[iter]);
        }

        if (data_info != NULL) {
            new_data_info = (bcm_flowtracker_export_record_data_info_t*)
                sal_alloc((sizeof(bcm_flowtracker_export_record_data_info_t) *
                            max_elements), "New data info");
            if (new_data_info == NULL) {
                sal_free(new_export_elem_info);
                return BCM_E_MEMORY;
            }

            for (iter = 0; iter < max_elements; iter++) {
                _bcm_compat6520in_flowtracker_export_record_data_info_t
                    (&data_info[iter],
                     &new_data_info[iter]);
            }
        }
    }

    rv = bcm_flowtracker_export_record_data_info_get(unit, record,
                max_elements, new_export_elem_info,
                new_data_info, count);

    if (new_export_elem_info != NULL) {
        sal_free(new_export_elem_info);
    }

    if (rv >= 0) {
        if ((data_info != NULL) && (new_data_info != NULL)) {
            for (iter = 0; iter < *count; iter++) {
                _bcm_compat6520out_flowtracker_export_record_data_info_t
                                    (&new_data_info[iter],
                                     &data_info[iter]);
            }
            sal_free(new_data_info);
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520out_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_port_config_t(
    bcm_port_config_t *from,
    bcm_compat6520_port_config_t *to)
{
    int i1 = 0;

    to->fe = from->fe;
    to->ge = from->ge;
    to->xe = from->xe;
    to->ce = from->ce;
    to->le = from->le;
    to->cc = from->cc;
    to->cd = from->cd;
    to->e = from->e;
    to->hg = from->hg;
    to->sci = from->sci;
    to->sfi = from->sfi;
    to->spi = from->spi;
    to->spi_subport = from->spi_subport;
    to->port = from->port;
    to->cpu = from->cpu;
    to->all = from->all;
    to->stack_int = from->stack_int;
    to->stack_ext = from->stack_ext;
    to->tdm = from->tdm;
    to->pon = from->pon;
    to->llid = from->llid;
    to->il = from->il;
    to->xl = from->xl;
    to->rcy = from->rcy;
    to->sat = from->sat;
    to->ipsec = from->ipsec;
    for (i1 = 0; i1 < BCM_PIPES_MAX; i1++) {
        to->per_pipe[i1] = from->per_pipe[i1];
    }
    to->nif = from->nif;
    to->control = from->control;
    to->eventor = from->eventor;
    to->olp = from->olp;
    to->oamp = from->oamp;
    to->erp = from->erp;
    to->roe = from->roe;
    to->rcy_mirror = from->rcy_mirror;
    for (i1 = 0; i1 < BCM_PP_PIPES_MAX; i1++) {
        to->per_pp_pipe[i1] = from->per_pp_pipe[i1];
    }
    to->stat = from->stat;
    to->crps = from->crps;
    return;
}

/*
 * Function:
 *      bcm_compat6520_port_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (OUT) Pointer to port configuration structure populated on successful return.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_port_config_get(
    int unit,
    bcm_compat6520_port_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_port_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_port_config_t *)
                     sal_alloc(sizeof(bcm_port_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_config_get(unit, new_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_port_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6520in_flow_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_initiator_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_flow_tunnel_initiator_t(
    bcm_compat6520_flow_tunnel_initiator_t *from,
    bcm_flow_tunnel_initiator_t *to)
{
    int i1 = 0;

    bcm_flow_tunnel_initiator_t_init(to);
    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->flow_port = from->flow_port;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->ecn = from->ecn;
}

/*
 * Function:
 *      _bcm_compat6520out_flow_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_initiator_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_flow_tunnel_initiator_t(
    bcm_flow_tunnel_initiator_t *from,
    bcm_compat6520_flow_tunnel_initiator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->flow_port = from->flow_port;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->ecn = from->ecn;
}

/*
 * Function:
 *      bcm_compat6520_flow_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flow_tunnel_initiator_create(
    int unit,
    bcm_compat6520_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flow_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_initiator_create(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_flow_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_flow_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flow_tunnel_initiator_get(
    int unit,
    bcm_compat6520_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flow_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_initiator_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_flow_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
/*
 * Function:
 *      _bcm_compat6520in_flexctr_action_t
 * Purpose:
 *      Convert the bcm_flexctr_action_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_flexctr_action_t(
    bcm_compat6520_flexctr_action_t *from,
    bcm_flexctr_action_t *to)
{
    bcm_flexctr_action_t_init(to);
    to->flags = from->flags;
    to->source = from->source;
    to->ports = from->ports;
    to->hint = from->hint;
    to->hint_ext = from->hint_ext;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    sal_memcpy(&to->index_operation, &from->index_operation, sizeof(bcm_flexctr_index_operation_t));
    sal_memcpy(&to->operation_a, &from->operation_a, sizeof(bcm_flexctr_value_operation_t));
    sal_memcpy(&to->operation_b, &from->operation_b, sizeof(bcm_flexctr_value_operation_t));
    sal_memcpy(&to->trigger, &from->trigger, sizeof(bcm_flexctr_trigger_t));
    to->hint_type = from->hint_type;
}

/*
 * Function:
 *      _bcm_compat6520out_flexctr_action_t
 * Purpose:
 *      Convert the bcm_flexctr_action_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_flexctr_action_t(
    bcm_flexctr_action_t *from,
    bcm_compat6520_flexctr_action_t *to)
{
    to->flags = from->flags;
    to->source = from->source;
    to->ports = from->ports;
    to->hint = from->hint;
    to->hint_ext = from->hint_ext;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    sal_memcpy(&to->index_operation, &from->index_operation, sizeof(bcm_flexctr_index_operation_t));
    sal_memcpy(&to->operation_a, &from->operation_a, sizeof(bcm_flexctr_value_operation_t));
    sal_memcpy(&to->operation_b, &from->operation_b, sizeof(bcm_flexctr_value_operation_t));
    sal_memcpy(&to->trigger, &from->trigger, sizeof(bcm_flexctr_trigger_t));
    to->hint_type = from->hint_type;
}

/*
 * Function:
 *      bcm_compat6520_flexctr_action_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_flexctr_action_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) BCM_FLEXCTR_OPTIONS_XXX options.
 *      action - (IN) Flex counter action data structure.
 *      stat_counter_id - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flexctr_action_create(
    int unit,
    int options,
    bcm_compat6520_flexctr_action_t *action,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flexctr_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_flexctr_action_t *)
                     sal_alloc(sizeof(bcm_flexctr_action_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flexctr_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_action_create(unit, options, new_action, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_flexctr_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flexctr_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      stat_counter_id - (IN) 
 *      action - (OUT) Flex counter action data structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flexctr_action_get(
    int unit,
    uint32 stat_counter_id,
    bcm_compat6520_flexctr_action_t *action)
{
    int rv = BCM_E_NONE;
    bcm_flexctr_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_flexctr_action_t *)
                     sal_alloc(sizeof(bcm_flexctr_action_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_action_get(unit, stat_counter_id, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_flexctr_action_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}


#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6520in_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_tunnel_terminator_t(
    bcm_compat6520_tunnel_terminator_t *from,
    bcm_tunnel_terminator_t *to)
{
    int i1 = 0;

    bcm_tunnel_terminator_t_init(to);
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
    to->pbmp = from->pbmp;
    to->vlan = from->vlan;
    to->fwd_vlan = from->fwd_vlan;
    to->remote_port = from->remote_port;
    to->tunnel_id = from->tunnel_id;
    to->if_class = from->if_class;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->failover_id = from->failover_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->tunnel_class = from->tunnel_class;
    to->qos_map_id = from->qos_map_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->priority = from->priority;
    to->subport_pbmp_profile_id = from->subport_pbmp_profile_id;
    to->egress_if = from->egress_if;
    to->default_vrf = from->default_vrf;
    to->sci = from->sci;
}

/*
 * Function:
 *      _bcm_compat6520out_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_tunnel_terminator_t(
    bcm_tunnel_terminator_t *from,
    bcm_compat6520_tunnel_terminator_t *to)
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
    to->pbmp = from->pbmp;
    to->vlan = from->vlan;
    to->fwd_vlan = from->fwd_vlan;
    to->remote_port = from->remote_port;
    to->tunnel_id = from->tunnel_id;
    to->if_class = from->if_class;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->failover_id = from->failover_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->tunnel_class = from->tunnel_class;
    to->qos_map_id = from->qos_map_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->priority = from->priority;
    to->subport_pbmp_profile_id = from->subport_pbmp_profile_id;
    to->egress_if = from->egress_if;
    to->default_vrf = from->default_vrf;
    to->sci = from->sci;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_add(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_add(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_create(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_delete(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_delete(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_update(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_update(unit, new_info);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_stat_attach(unit, new_terminator, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_stat_detach(unit, new_terminator);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_stat_id_get(unit, new_terminator, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      value - (IN) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_flexctr_object_set(unit, new_terminator, value);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_tunnel_terminator_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      terminator - (IN) Tunnel terminator.
 *      value - (OUT) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_terminator = NULL;

    if (terminator != NULL) {
        new_terminator = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New terminator");
        if (new_terminator == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(terminator, new_terminator);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_flexctr_object_get(unit, new_terminator, value);


    /* Deallocate memory*/
    sal_free(new_terminator);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vxlan_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vxlan_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_vxlan_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l2gre_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l2gre_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_l2gre_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_terminator_t *)
                     sal_alloc(sizeof(bcm_tunnel_terminator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6520in_collector_info_t
 * Purpose:
 *      Convert the bcm_collector_info_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_collector_info_t(
    bcm_compat6520_collector_info_t *from,
    bcm_collector_info_t *to)
{
    bcm_collector_info_t_init(to);
    to->collector_type = from->collector_type;
    to->transport_type = from->transport_type;
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
    to->src_ts = from->src_ts;
    to->max_records_reserve = from->max_records_reserve;
}

/*
 * Function:
 *      _bcm_compat6520out_collector_info_t
 * Purpose:
 *      Convert the bcm_collector_info_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_collector_info_t(
    bcm_collector_info_t *from,
    bcm_compat6520_collector_info_t *to)
{
    to->collector_type = from->collector_type;
    to->transport_type = from->transport_type;
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
    to->src_ts = from->src_ts;
    to->max_records_reserve = from->max_records_reserve;
}

/*
 * Function:
 *      bcm_compat6520_collector_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_collector_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Options bitmap of BCM_COLLECTOR_XXX. Create or Modify collector.
 *      collector_id - (INOUT) (IN/OUT) The ID of collector created. IN parameter if options parameter has BCM_COLLECTOR_WITH_ID or BCM_COLLECTOR_REPLACE option set.
 *      collector_info - (IN) Pointer to the collector information structure.
 This API creates a collector with the information present in the collector_info structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_collector_create(
    int unit,
    uint32 options,
    bcm_collector_t *collector_id,
    bcm_compat6520_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t *)
                     sal_alloc(sizeof(bcm_collector_info_t),
                     "New collector_info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_collector_create(unit, options, collector_id, new_collector_info);


    /* Deallocate memory*/
    sal_free(new_collector_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_collector_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_collector_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      id - (IN) Collector software ID.
 *      collector_info - (OUT) Pointer to the collector information structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_collector_get(
    int unit,
    bcm_collector_t id,
    bcm_compat6520_collector_info_t *collector_info)
{
    int rv = BCM_E_NONE;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t *)
                     sal_alloc(sizeof(bcm_collector_info_t),
                     "New collector_info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_collector_get(unit, id, new_collector_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6520out_collector_info_t(new_collector_info, collector_info);

    /* Deallocate memory*/
    sal_free(new_collector_info);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6520in_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_flow_port_encap_t(
    bcm_compat6520_flow_port_encap_t *from,
    bcm_flow_port_encap_t *to)
{
    bcm_flow_port_encap_t_init(to);
    to->flow_port = from->flow_port;
    to->options = from->options;
    to->flags = from->flags;
    to->class_id = from->class_id;
    to->mtu = from->mtu;
    to->network_group = from->network_group;
    to->dvp_group = from->dvp_group;
    to->pri = from->pri;
    to->cfi = from->cfi;
    to->tpid = from->tpid;
    to->vlan = from->vlan;
    to->egress_if = from->egress_if;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->ip4_id = from->ip4_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->dst_port = from->dst_port;
    to->es_id = from->es_id;
}

/*
 * Function:
 *      bcm_compat6520_flow_port_encap_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_encap_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) (IN/OUT) Flow port encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flow_port_encap_set(
    int unit,
    bcm_compat6520_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_port_encap_t *)
                     sal_alloc(sizeof(bcm_flow_port_encap_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flow_port_encap_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_encap_set(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_flow_port_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_port_encap_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) (IN/OUT) Flow port encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_flow_port_encap_get(
    int unit,
    bcm_compat6520_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_port_encap_t *)
                     sal_alloc(sizeof(bcm_flow_port_encap_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6520in_flow_port_encap_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_encap_get(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6520in_field_presel_config_t
 * Purpose:
 *      Convert the bcm_field_presel_config_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_field_presel_config_t(
    bcm_compat6520_field_presel_config_t *from,
    bcm_field_presel_config_t *to)
{
    sal_memcpy(to->name, from->name, (sizeof(uint8) * BCM_FIELD_MAX_NAME_LEN));
    to->flags = from->flags;
    to->stage = from->stage;
    to->presel_id = from->presel_id;
}

/*
 * Function:
 *      _bcm_compat6520out_field_presel_config_t
 * Purpose:
 *      Convert the bcm_field_presel_config_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_field_presel_config_t(
    bcm_field_presel_config_t *from,
    bcm_compat6520_field_presel_config_t *to)
{
    sal_memcpy(to->name, from->name, (sizeof(uint8) * BCM_FIELD_MAX_NAME_LEN));
    to->flags = from->flags;
    to->stage = from->stage;
    to->presel_id = from->presel_id;
}

/*
 * Function:
 *      bcm_compat6520_field_presel_config_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_presel_config_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      presel_id - (IN) Presel ID.
 *      presel_config - (IN) Pointer to the presel config structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_presel_config_set(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6520_field_presel_config_t *presel_config)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_config_t *new_presel_config = NULL;

    if (presel_config != NULL) {
        new_presel_config = (bcm_field_presel_config_t *)
                     sal_alloc(sizeof(bcm_field_presel_config_t),
                     "New presel_config");
        if (new_presel_config == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6520in_field_presel_config_t(presel_config, new_presel_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_presel_config_set(unit, presel_id, new_presel_config);

    if (new_presel_config != NULL) {
        _bcm_compat6520out_field_presel_config_t(new_presel_config, presel_config);
        sal_free(new_presel_config);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_presel_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_presel_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      presel_id - (IN) Presel ID.
 *      presel_config - (OUT) Pointer to the presel config structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_presel_config_get(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6520_field_presel_config_t *presel_config)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_config_t *new_presel_config = NULL;

    if (presel_config != NULL) {
        new_presel_config = (bcm_field_presel_config_t *)
                     sal_alloc(sizeof(bcm_field_presel_config_t),
                     "New presel_config");
        if (new_presel_config == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6520in_field_presel_config_t(presel_config, new_presel_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_presel_config_get(unit, presel_id, new_presel_config);

    if (new_presel_config != NULL) {
        _bcm_compat6520out_field_presel_config_t(new_presel_config, presel_config);
        sal_free(new_presel_config);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_presel_config_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_presel_config_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      presel_config - (IN) Pointer to the presel config structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_presel_config_create(
    int unit,
    bcm_compat6520_field_presel_config_t *presel_config)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_config_t *new_presel_config = NULL;

    if (presel_config != NULL) {
        new_presel_config = (bcm_field_presel_config_t *)
                     sal_alloc(sizeof(bcm_field_presel_config_t),
                     "New presel_config");
        if (new_presel_config == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6520in_field_presel_config_t(presel_config, new_presel_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_presel_config_create(unit, new_presel_config);

    if (new_presel_config != NULL) {
        _bcm_compat6520out_field_presel_config_t(new_presel_config, presel_config);
        sal_free(new_presel_config);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6520in_field_hint_t
 * Purpose:
 *      Convert the bcm_field_hint_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520in_field_hint_t(
    bcm_compat6520_field_hint_t *from,
    bcm_field_hint_t *to)
{
    to->hint_type = from->hint_type;
    to->qual = from->qual;
    to->max_values = from->max_values;
    to->start_bit = from->start_bit;
    to->end_bit = from->end_bit;
    to->flags = from->flags;
    to->dosattack_event_flags = from->dosattack_event_flags;
    to->max_group_size = from->max_group_size;
    to->priority = from->priority;
    to->udf_id = from->udf_id;
    to->group_id = from->group_id;
    to->value = from->value;
}

/*
 * Function:
 *      _bcm_compat6520out_field_hint_t
 * Purpose:
 *      Convert the bcm_field_hint_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6520out_field_hint_t(
    bcm_field_hint_t *from,
    bcm_compat6520_field_hint_t *to)
{
    to->hint_type = from->hint_type;
    to->qual = from->qual;
    to->max_values = from->max_values;
    to->start_bit = from->start_bit;
    to->end_bit = from->end_bit;
    to->flags = from->flags;
    to->dosattack_event_flags = from->dosattack_event_flags;
    to->max_group_size = from->max_group_size;
    to->priority = from->priority;
    to->udf_id = from->udf_id;
    to->group_id = from->group_id;
    to->value = from->value;
}

/*
 * Function:
 *      bcm_compat6520_field_hints_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_hints_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      hint_id - (IN) Hint ID.
 *      hint - (IN) Pointer to the hint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_hints_add(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6520_field_hint_t *hint)
{
    int rv = BCM_E_NONE;
    bcm_field_hint_t *new_hint = NULL;

    if (hint != NULL) {
        new_hint = (bcm_field_hint_t *)
                     sal_alloc(sizeof(bcm_field_hint_t),
                     "New hint");
        if (new_hint == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6520in_field_hint_t(hint, new_hint);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_hints_add(unit, hint_id, new_hint);

    if (new_hint != NULL) {
        _bcm_compat6520out_field_hint_t(new_hint, hint);
        sal_free(new_hint);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_hints_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_hints_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      hint_id - (IN) Hint ID.
 *      hint - (OUT) Pointer to the hint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_hints_get(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6520_field_hint_t *hint)
{
    int rv = BCM_E_NONE;
    bcm_field_hint_t *new_hint = NULL;

    if (hint != NULL) {
        new_hint = (bcm_field_hint_t *)
                     sal_alloc(sizeof(bcm_field_hint_t),
                     "New hint");
        if (new_hint == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6520in_field_hint_t(hint, new_hint);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_hints_get(unit, hint_id, new_hint);

    if (new_hint != NULL) {
        _bcm_compat6520out_field_hint_t(new_hint, hint);
        sal_free(new_hint);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_hints_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_hints_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      hint_id - (IN) Hint ID.
 *      hint - (IN) Pointer to the hint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_hints_delete(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6520_field_hint_t *hint)
{
    int rv = BCM_E_NONE;
    bcm_field_hint_t *new_hint = NULL;

    if (hint != NULL) {
        new_hint = (bcm_field_hint_t *)
                     sal_alloc(sizeof(bcm_field_hint_t),
                     "New hint");
        if (new_hint == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6520in_field_hint_t(hint, new_hint);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_hints_delete(unit, hint_id, new_hint);

    if (new_hint != NULL) {
        _bcm_compat6520out_field_hint_t(new_hint, hint);
        sal_free(new_hint);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6520_field_hints_multi_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_hints_multi_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      hint_id - (IN) Hint ID.
 *      hint - (OUT) Pointer to the hint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6520_field_hints_multi_get(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_field_hint_type_t hint_type,
    int hints_size,
    bcm_compat6520_field_hint_t *hints_arr,
    int *hints_count)
{
    int rv = BCM_E_NONE;
    int i = 0;
    bcm_field_hint_t *new_hint_arr = NULL;

    if (hints_arr != NULL) {
        new_hint_arr = (bcm_field_hint_t *)
                     sal_alloc((hints_size * sizeof(bcm_field_hint_t)),
                     "New hint");
        if (new_hint_arr == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < hints_size; i++) {
            _bcm_compat6520in_field_hint_t(&hints_arr[i], &new_hint_arr[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_hints_multi_get(unit, hint_id, hint_type,
                                   hints_size, new_hint_arr,
                                   hints_count);

    if (new_hint_arr != NULL) {
        for (i = 0; i < hints_size; i++) {
            _bcm_compat6520out_field_hint_t(&new_hint_arr[i], &hints_arr[i]);
        }
        sal_free(new_hint_arr);
    }

    return rv;
}

#endif /* BCM_RPC_SUPPORT */
