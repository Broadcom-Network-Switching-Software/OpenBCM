/*
 * $Id: compat_6513.c,v 2.0 2018/02/19
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with <=sdk-6.5.13 routines
 */

#ifdef   BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6513.h>

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6513in_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from <=6.5.13 to
 *      SDK 6.5.13+
 * Parameters:
 *      from        - (IN) The <=6.5.13 version of the datatype
 *      to          - (OUT) The SDK 6.5.13+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6513in_flow_encap_config_t(bcm_compat6513_flow_encap_config_t *from,
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
 }

/*
 * Function:
 *      _bcm_compat6513out_ecn_map_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from 6.5.13+ to
 *      <=6.5.13
 * Parameters:
 *      from        - (IN) The SDK 6.5.13+ version of the datatype
 *      to          - (OUT) The <=6.5.13 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6513out_flow_encap_config_t(bcm_flow_encap_config_t *from,
                            bcm_compat6513_flow_encap_config_t *to)
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
    
}

/*
 * Function: bcm_compat6513_flow_encap_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_get.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)flow encap Config
 *      num_of_fields - (IN) Number of logical fields
 *      field    - (IN/OUT)Point to logical field array
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6513_flow_encap_get(int unit, 
    bcm_compat6513_flow_encap_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_encap_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_encap_config_t *)
            sal_alloc(sizeof(bcm_flow_encap_config_t), "New flow encap config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6513in_flow_encap_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_get(unit, new_info, num_of_fields, field);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6513out_flow_encap_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat6513_flow_encap_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_flow_encap_delete.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)flow encap Config
 *      num_of_fields - (IN) Number of logical fields
 *      field    - (IN/OUT)Point to logical field array
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6513_flow_encap_delete(int unit, 
    bcm_compat6513_flow_encap_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_encap_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_encap_config_t *)
            sal_alloc(sizeof(bcm_flow_encap_config_t), "New flow encap config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6513in_flow_encap_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_delete(unit, new_info, num_of_fields, field);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6513out_flow_encap_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat6513_flow_encap_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_flow_encap_add.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)flow encap Config
 *      num_of_fields - (IN) Number of logical fields
 *      field    - (IN/OUT)Point to logical field array
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6513_flow_encap_add(int unit, 
    bcm_compat6513_flow_encap_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_encap_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_encap_config_t *)
            sal_alloc(sizeof(bcm_flow_encap_config_t), "New flow encap config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6513in_flow_encap_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_add(unit, new_info, num_of_fields, field);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6513out_flow_encap_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6513in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.13 to
 *      SDK 6.5.13+
 * Parameters:
 *      from        - (IN) The <=6.5.13 version of the datatype
 *      to          - (OUT) The SDK 6.5.13+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6513in_l3_egress_t(
    bcm_compat6513_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
}

/*
 * Function:
 *      _bcm_compat6513out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from 6.5.13+ to
 *      <=6.5.13
 * Parameters:
 *      from        - (IN) The SDK 6.5.13+ version of the datatype
 *      to          - (OUT) The <=6.5.13 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6512out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6513_l3_egress_t *to)
{
    int i = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
}

/*
 * Function: bcm_compat6513_l3_egress_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_l3_egress_create.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN/OUT) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6513_l3_egress_create(int unit, uint32 flags,
                                bcm_compat6513_l3_egress_t *egr,
                                bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6513in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, intf);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6512out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

/*
 * Function: bcm_compat6513_l3_egress_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_l3_egress_get.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN/OUT) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6513_l3_egress_get(int unit, bcm_if_t intf,
                             bcm_compat6513_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6513in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_get(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6512out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

/*
 * Function: bcm_compat6513_l3_egress_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_l3_egress_find.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN/OUT) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6513_l3_egress_find(int unit, bcm_compat6513_l3_egress_t *egr,
                              bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6513in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_find(unit, new_egr, intf);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6512out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}
#endif

#ifdef INCLUDE_TCB
/*
 * Function:
 *      _bcm_compat6513in_cosq_tcb_config_t
 * Purpose:
 *      Convert the bcm_cosq_tcb_config_t datatype from <=6.5.13 to
 *      SDK 6.5.14+
 * Parameters:
 *      from        - (IN) The <=6.5.13 version of the datatype
 *      to          - (OUT) The SDK 6.5.14+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6513in_cosq_tcb_config_t(
    bcm_compat6513_cosq_tcb_config_t *from,
    bcm_cosq_tcb_config_t *to)
{
    bcm_cosq_tcb_config_t_init(to);
    to->scope_type = from->scope_type;
    to->gport = from->gport;
    to->cosq = from->cosq;
    to->trigger_reason = from->trigger_reason;
    to->pre_freeze_capture_num = from->pre_freeze_capture_num;
    to->pre_freeze_capture_time = from->pre_freeze_capture_time;
    to->post_sample_probability = from->post_sample_probability;
    to->pre_sample_probability = from->pre_sample_probability;
}

/*
 * Function:
 *      _bcm_compat6513out_cosq_tcb_config_t
 * Purpose:
 *      Convert the bcm_cosq_tcb_config_t datatype from 6.5.14+ to
 *      <=SDK 6.5.13
 * Parameters:
 *      from        - (IN) The SDK 6.5.14+ version of the datatype
 *      to          - (OUT) The <=6.5.13 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6513out_cosq_tcb_config_t(
    bcm_cosq_tcb_config_t *from,
    bcm_compat6513_cosq_tcb_config_t *to)
{
    to->scope_type = from->scope_type;
    to->gport = from->gport;
    to->cosq = from->cosq;
    to->trigger_reason = from->trigger_reason;
    to->pre_freeze_capture_num = from->pre_freeze_capture_num;
    to->pre_freeze_capture_time = from->pre_freeze_capture_time;
    to->post_sample_probability = from->post_sample_probability;
    to->pre_sample_probability = from->pre_sample_probability;
}

/*
 * Function:
 *      bcm_compat6513_cosq_tcb_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_tcb_config_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (OUT) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_compat6513_cosq_tcb_config_get(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6513_cosq_tcb_config_t *config)
{
    int rv;
    bcm_cosq_tcb_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_cosq_tcb_config_t*)
            sal_alloc(sizeof(bcm_cosq_tcb_config_t), "New cosq tcb config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6513in_cosq_tcb_config_t(config, new_config);
    }

    rv = bcm_cosq_tcb_config_get(unit, buffer_id, new_config);

    if (new_config != NULL) {
        _bcm_compat6513out_cosq_tcb_config_t(new_config, config);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6513_cosq_tcb_config_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_tcb_config_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (OUT) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_compat6513_cosq_tcb_config_set(
    int unit,
    bcm_cosq_buffer_id_t buffer_id,
    bcm_compat6513_cosq_tcb_config_t *config)
{
    int rv;
    bcm_cosq_tcb_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_cosq_tcb_config_t*)
            sal_alloc(sizeof(bcm_cosq_tcb_config_t), "New cosq tcb config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6513in_cosq_tcb_config_t(config, new_config);
    }

    rv = bcm_cosq_tcb_config_set(unit, buffer_id, new_config);

    if (new_config != NULL) {
        _bcm_compat6513out_cosq_tcb_config_t(new_config, config);
        sal_free(new_config);
    }

    return rv;
}
#endif
#endif  /* BCM_RPC_SUPPORT */

