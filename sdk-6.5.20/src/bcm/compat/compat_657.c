/*
 * $Id: compat_657.c,v 2.0 2016/12/07
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with <=sdk-6.5.7 routines
 */

#ifdef	BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_657.h>
#include <bcm/port.h> 
#if defined(INCLUDE_L3)
#include <bcm/extender.h>

/*
 * Function:
 *      _bcm_compat657in_extender_forward_t
 * Purpose:
 *      Convert the bcm_extender_forward_t datatype from <=6.5.7 to
 *      SDK 6.5.8+
 * Parameters:
 *      from        - (IN) The <=6.5.7 version of the datatype
 *      to          - (OUT) The SDK 6.5.8+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657in_extender_forward_t(bcm_compat657_extender_forward_t *from,
                                    bcm_extender_forward_t *to)
{
    bcm_extender_forward_t_init(to);
    to->flags = from->flags;
    to->name_space = from->name_space;
    to->extended_port_vid = from->extended_port_vid;
    to->dest_port = from->dest_port;
    to->dest_multicast = from->dest_multicast;
    to->extender_forward_id = from->extender_forward_id;
}

/*
 * Function:
 *      _bcm_compat657out_extender_forward_t
 * Purpose:
 *      Convert the bcm_field_qset_t datatype from 6.5.8+ to
 *      <=6.5.7
 * Parameters:
 *      from        - (IN) The SDK 6.5.8+ version of the datatype
 *      to          - (OUT) The <=6.5.7 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657out_extender_forward_t(bcm_extender_forward_t *from,
                                     bcm_compat657_extender_forward_t *to)
{
    to->flags = from->flags;
    to->name_space = from->name_space;
    to->extended_port_vid = from->extended_port_vid;
    to->dest_port = from->dest_port;
    to->dest_multicast = from->dest_multicast;
    to->extender_forward_id = from->extender_forward_id;
}

/*
 * Function: bcm_compat657_extender_forward_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_extender_forward_add.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN/OUT) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_extender_forward_add(int unit, bcm_compat657_extender_forward_t
                                   *extender_forward_entry)
{
    int rv;
    bcm_extender_forward_t *new_extender_forward_entry = NULL;

    if (NULL != extender_forward_entry) {
        /* Create from heap to avoid the stack to bloat */
        new_extender_forward_entry = (bcm_extender_forward_t *)
            sal_alloc(sizeof(bcm_extender_forward_t), "New Extender Fwd Entry");
        if (NULL == new_extender_forward_entry) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat657in_extender_forward_t(extender_forward_entry,
                                            new_extender_forward_entry);
    }

    /* Call the BCM API with new format */
    rv = bcm_extender_forward_add(unit, new_extender_forward_entry);

    if (NULL != new_extender_forward_entry) {
        /* Transform the entry from the new format to old one */
        _bcm_compat657out_extender_forward_t(new_extender_forward_entry,
                                             extender_forward_entry);

        /* Deallocate memory*/
        sal_free(new_extender_forward_entry);
    }

    return rv;
}

/*
 * Function: bcm_compat657_extender_forward_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_extender_forward_delete.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_extender_forward_delete(int unit, bcm_compat657_extender_forward_t
                                      *extender_forward_entry)
{
    int rv;
    bcm_extender_forward_t *new_extender_forward_entry = NULL;

    if (NULL != extender_forward_entry) {
        /* Create from heap to avoid the stack to bloat */
        new_extender_forward_entry = (bcm_extender_forward_t *)
            sal_alloc(sizeof(bcm_extender_forward_t), "New Extender Fwd Entry");
        if (NULL == new_extender_forward_entry) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat657in_extender_forward_t(extender_forward_entry,
                                            new_extender_forward_entry);
    }

    /* Call the BCM API with new format */
    rv = bcm_extender_forward_delete(unit, new_extender_forward_entry);

    if (NULL != new_extender_forward_entry) {
        /* Deallocate memory*/
        sal_free(new_extender_forward_entry);
    }

    return rv;
}

/*
 * Function: bcm_compat657_extender_forward_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_extender_forward_get.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN/OUT) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_extender_forward_get(int unit, bcm_compat657_extender_forward_t
                                   *extender_forward_entry)
{
    int rv;
    bcm_extender_forward_t *new_extender_forward_entry = NULL;

    if (NULL != extender_forward_entry) {
        /* Create from heap to avoid the stack to bloat */
        new_extender_forward_entry = (bcm_extender_forward_t *)
            sal_alloc(sizeof(bcm_extender_forward_t), "New Extender Fwd Entry");
        if (NULL == new_extender_forward_entry) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat657in_extender_forward_t(extender_forward_entry,
                                            new_extender_forward_entry);
    }

    /* Call the BCM API with new format */
    rv = bcm_extender_forward_get(unit, new_extender_forward_entry);

    if (NULL != new_extender_forward_entry) {
        /* Transform the entry from the new format to old one */
        _bcm_compat657out_extender_forward_t(new_extender_forward_entry,
                                             extender_forward_entry);
        /* Deallocate memory*/
        sal_free(new_extender_forward_entry);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat657in_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from <=6.5.7 to
 *      SDK 6.5.8+
 * Parameters:
 *      from        - (IN) The <=6.5.7 version of the datatype
 *      to          - (OUT) The SDK 6.5.8+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657in_mpls_tunnel_switch_t(bcm_compat657_mpls_tunnel_switch_t *from,
                                    bcm_mpls_tunnel_switch_t *to)
{
    bcm_mpls_tunnel_switch_t_init(to);
    to->flags = from->flags;
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
    to->egress_label = from->egress_label;
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
}

/*
 * Function:
 *      _bcm_compat657out_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from 6.5.8+ to
 *      <=6.5.7
 * Parameters:
 *      from        - (IN) The SDK 6.5.8+ version of the datatype
 *      to          - (OUT) The <=6.5.7 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657out_mpls_tunnel_switch_t(bcm_mpls_tunnel_switch_t *from,
                                     bcm_compat657_mpls_tunnel_switch_t *to)
{
    to->flags = from->flags;
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
    to->egress_label = from->egress_label;
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
}

/*
 * Function: bcm_compat657_mpls_tunnel_switch_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_add
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     bcm_mpls_tunnel_switch_t - (IN/OUT) Tunnel entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_mpls_tunnel_switch_add (int unit,
                                      bcm_compat657_mpls_tunnel_switch_t *info)
{
    int rv;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_mpls_tunnel_switch_t *)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                      "New MPLS tunnel switch entry");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat657in_mpls_tunnel_switch_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_add (unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat657out_mpls_tunnel_switch_t(new_info , info);

        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat657_mpls_tunnel_switch_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_create
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     bcm_mpls_tunnel_switch_t - (IN/OUT) Tunnel entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_mpls_tunnel_switch_create (int unit,
                                    bcm_compat657_mpls_tunnel_switch_t *info)
{
    int rv;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_mpls_tunnel_switch_t *)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                      "New MPLS tunnel switch entry");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat657in_mpls_tunnel_switch_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_create (unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat657out_mpls_tunnel_switch_t(new_info , info);

        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat657_mpls_tunnel_switch_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_delete
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     bcm_mpls_tunnel_switch_t - (IN/OUT) Tunnel entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_mpls_tunnel_switch_delete (int unit,
                                    bcm_compat657_mpls_tunnel_switch_t *info)
{
    int rv;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_mpls_tunnel_switch_t *)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                      "New MPLS tunnel switch entry");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat657in_mpls_tunnel_switch_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_delete (unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat657out_mpls_tunnel_switch_t(new_info , info);

        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat657_mpls_tunnel_switch_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_get
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     bcm_mpls_tunnel_switch_t - (IN/OUT) Tunnel entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_mpls_tunnel_switch_get (int unit,
                                      bcm_compat657_mpls_tunnel_switch_t *info)
{
    int rv;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_mpls_tunnel_switch_t *)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t),
                      "New MPLS tunnel switch entry");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat657in_mpls_tunnel_switch_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_switch_get (unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat657out_mpls_tunnel_switch_t(new_info , info);

        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}
#endif /* INCLUDE_L3 */
/*
 * Function:
 *      _bcm_compat657in_port_e2efc_remote_port_config_t
 * Purpose:
 *      Convert the bcm_port_e2efc_remote_port_config_t datatype from <=6.5.7 to
 *      SDK 6.5.8+
 * Parameters:
 *      from        - (IN) The <=6.5.7 version of the datatype
 *      to          - (OUT) The SDK 6.5.8+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657in_port_e2efc_remote_port_config_t(bcm_compat657_port_e2efc_remote_port_config_t *from,
                                    bcm_port_e2efc_remote_port_config_t *to)
{
    bcm_port_e2efc_remote_port_config_t_init(to);
    to->remote_module = from->remote_module;
    to->remote_port = from->remote_port;
    to->xoff_threshold_bytes = from->xoff_threshold_bytes;
    to->xoff_threshold_packets = from->xoff_threshold_packets;
    to->xon_threshold_bytes = from->xon_threshold_bytes;
    to->xon_threshold_packets = from->xon_threshold_packets;
    to->drop_threshold_bytes = from->drop_threshold_bytes;
    to->drop_threshold_packets = from->drop_threshold_packets;
}

/*
 * Function:
 *      _bcm_compat657out_port_e2efc_remote_port_config_t
 * Purpose:
 *      Convert the bcm_port_e2efc_remote_port_config_t datatype from 6.5.8+ to
 *      SDK <= 6.5.7
 * Parameters:
 *      from        - (IN) The SDK 6.5.8+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.7 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657out_port_e2efc_remote_port_config_t(bcm_port_e2efc_remote_port_config_t *from,
                                                  bcm_compat657_port_e2efc_remote_port_config_t *to)
{
    to->remote_module = from->remote_module;
    to->remote_port = from->remote_port;
    to->xoff_threshold_bytes = from->xoff_threshold_bytes;
    to->xoff_threshold_packets = from->xoff_threshold_packets;
    to->xon_threshold_bytes = from->xon_threshold_bytes;
    to->xon_threshold_packets = from->xon_threshold_packets;
    to->drop_threshold_bytes = from->drop_threshold_bytes;
    to->drop_threshold_packets = from->drop_threshold_packets;
}
/*
 * Function: bcm_compat657_port_e2efc_remote_port_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_e2efc_remote_port_add
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     bcm_port_e2efc_remote_port_config_t - (IN) e2efc_remote_port_info with
 *                                                old format
 *     rport_handle_id - (OUT) output handle to be used in get and set API.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_port_e2efc_remote_port_add (int unit,
                            bcm_compat657_port_e2efc_remote_port_config_t *info,
                            int *rport_handle_id)
{
    int rv;
    bcm_port_e2efc_remote_port_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_port_e2efc_remote_port_config_t *)
            sal_alloc(sizeof(bcm_port_e2efc_remote_port_config_t),
                      "New E2EFC Remote port info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the info from the old format to new one */
        _bcm_compat657in_port_e2efc_remote_port_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_e2efc_remote_port_add (unit, new_info, rport_handle_id);

    if (NULL != new_info) {
        /* Transform the info from the new format to old one */
        _bcm_compat657out_port_e2efc_remote_port_config_t(new_info , info);

        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat657_port_e2efc_remote_port_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_e2efc_remote_port_set
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     rport_handle_id - (IN) output handle to be used in get and set API.
 *     bcm_port_e2efc_remote_port_config_t - (IN) e2efc_remote_port_info with
 *                                                old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_port_e2efc_remote_port_set (int unit, int rport_handle_id,
                            bcm_compat657_port_e2efc_remote_port_config_t *info)
{
    int rv;
    bcm_port_e2efc_remote_port_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_port_e2efc_remote_port_config_t *)
            sal_alloc(sizeof(bcm_port_e2efc_remote_port_config_t),
                      "New E2EFC Remote port info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the info from the old format to new one */
        _bcm_compat657in_port_e2efc_remote_port_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_e2efc_remote_port_set (unit, rport_handle_id, new_info);

    if (NULL != new_info) {
        /* Transform the info from the new format to old one */
        _bcm_compat657out_port_e2efc_remote_port_config_t(new_info , info);

        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat657_port_e2efc_remote_port_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_e2efc_remote_port_get
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     rport_handle_id - (IN) output handle to be used in get and set API.
 *     bcm_port_e2efc_remote_port_config_t - (OUT) e2efc_remote_port_info with
 *                                                 old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_port_e2efc_remote_port_get (int unit, int rport_handle_id,
                            bcm_compat657_port_e2efc_remote_port_config_t *info)
{
    int rv;
    bcm_port_e2efc_remote_port_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_port_e2efc_remote_port_config_t *)
            sal_alloc(sizeof(bcm_port_e2efc_remote_port_config_t),
                      "New E2EFC Remote port info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the info from the old format to new one */
        _bcm_compat657in_port_e2efc_remote_port_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_e2efc_remote_port_get (unit, rport_handle_id, new_info);

    if (NULL != new_info) {
        /* Transform the info from the new format to old one */
        _bcm_compat657out_port_e2efc_remote_port_config_t(new_info , info);

        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat657in_field_qset_t
 * Purpose:
 *      Convert the bcm_field_qset_t datatype from SDK 6.5.7 version to
 *      SDK 6.5.7+ version
 * Parameters:
 *      from        - (IN) The old version of the datatype
 *      to          - (OUT) The SDK 6.5.7+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657in_field_qset_t(
    bcm_compat657_field_qset_t *from,
    bcm_field_qset_t *to)
{
    uint32  field_qualify_cnt_size =
        (BCM_COMPAT657_FIELD_QUALIFY_CNT < bcmFieldQualifyCount) ?
            BCM_COMPAT657_FIELD_QUALIFY_CNT :  bcmFieldQualifyCount;
    uint32  field_user_num_udfs_size =
       (BCM_COMPAT657_FIELD_USER_NUM_UDFS < BCM_FIELD_USER_NUM_UDFS) ?
        BCM_COMPAT657_FIELD_USER_NUM_UDFS : BCM_FIELD_USER_NUM_UDFS;

    SHR_BITCOPY_RANGE(to->w, 0, from->w, 0, field_qualify_cnt_size);
    SHR_BITCOPY_RANGE(to->w, bcmFieldQualifyCount,
                      from->w, BCM_COMPAT657_FIELD_QUALIFY_CNT,
                      field_user_num_udfs_size);
    SHR_BITCOPY_RANGE(to->udf_map, 0,
                      from->udf_map, 0,
                      field_user_num_udfs_size);
}

/*
 * Function:
 *      _bcm_compat657out_field_qset_t
 * Purpose:
 *      Convert the bcm_field_qset_t datatype from SDK 6.5.7+ to
 *      SDK 6.5.7 version
 * Parameters:
 *      from        - (IN) The SDK 6.5.7+ version of the datatype
 *      to          - (OUT) The old version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat657out_field_qset_t(
    bcm_field_qset_t *from,
    bcm_compat657_field_qset_t *to)
{
    uint32  field_qualify_cnt_size =
        (BCM_COMPAT657_FIELD_QUALIFY_CNT < bcmFieldQualifyCount) ?
            BCM_COMPAT657_FIELD_QUALIFY_CNT :  bcmFieldQualifyCount;
    uint32  field_user_num_udfs_size =
       (BCM_COMPAT657_FIELD_USER_NUM_UDFS < BCM_FIELD_USER_NUM_UDFS) ?
        BCM_COMPAT657_FIELD_USER_NUM_UDFS : BCM_FIELD_USER_NUM_UDFS;

    SHR_BITCOPY_RANGE(to->w, 0, from->w, 0, field_qualify_cnt_size);
    SHR_BITCOPY_RANGE(to->w, BCM_COMPAT657_FIELD_QUALIFY_CNT,
                      from->w, bcmFieldQualifyCount,
                      field_user_num_udfs_size);
    SHR_BITCOPY_RANGE(to->udf_map, 0,
                      from->udf_map, 0,
                      field_user_num_udfs_size);
}

/*
 * Function: bcm_compat657_field_group_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_create.
 *      Used to create a FP group using an older version of the datatype.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     qset - (IN) Old field qualifier set
 *     pri  - (IN) Priority within allowable range,
 *                 or BCM_FIELD_GROUP_PRIO_ANY to automatically assign
 *                 priority; each priority value may be used only once
 *     group - (OUT) New field group ID
 *
 * Returns:
 *     BCM_E_INIT     - BCM unit not initialized
 *     BCM_E_PARAM    - pri out of range (0-15 for FB & ER) or group == NULL
 *     BCM_E_RESOURCE - no select codes will satisfy qualifier set
 *     BCM_E_MEMORY   - allocation failure
 *     BCM_E_NONE     - Success
 */
int
bcm_compat657_field_group_create(int unit,
                                bcm_compat657_field_qset_t qset,
                                int pri,
                                bcm_field_group_t *group)
{
    int rv;
    bcm_field_qset_t *p_new_qset;

    /* Create the qualifier set from heap to avoid the stack to bloat */
    p_new_qset =
        (bcm_field_qset_t *) sal_alloc(sizeof(bcm_field_qset_t), "NewQset");
    if (NULL == p_new_qset) {
        return BCM_E_MEMORY;
    }

    /* Initialize the new qualifier set */
    BCM_FIELD_QSET_INIT(*p_new_qset);

    /* Transform the qulaifier set from the old format to new one */
    _bcm_compat657in_field_qset_t(&qset, p_new_qset);

    /* Call the BCM API with new qset format */
    rv = bcm_field_group_create(unit, *p_new_qset, pri, group);

    /* Deallocate the q set memory */
    sal_free(p_new_qset);

    return rv;
}

/*
 * Function: bcm_compat657_field_group_create_id
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_create_id.
 *      Used to create a FP group using an older version of the datatype.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     qset - (IN) Old field qualifier set
 *     pri  - (IN) Priority within allowable range,
 *                 or BCM_FIELD_GROUP_PRIO_ANY to automatically assign
 *                 priority; each priority value may be used only once
 *     group - (IN) Requested new field group ID
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - No unused group/slices left
 *     BCM_E_PARAM     - priority out of range (0-15 for FB & ER)
 *     BCM_E_EXISTS    - group with that id already exists on this unit.
 *     BCM_E_MEMORY    - Memory allocation failure
 *     BCM_E_NONE      - Success
 */
int
bcm_compat657_field_group_create_id(int unit,
                                    bcm_compat657_field_qset_t qset,
                                    int pri,
                                    bcm_field_group_t group)
{
    int rv;
    bcm_field_qset_t *p_new_qset;

    /* Create the qualifier set from heap to avoid the huge stack variable */
    p_new_qset =
        (bcm_field_qset_t *) sal_alloc(sizeof(bcm_field_qset_t), "NewQset");
    if (NULL == p_new_qset) {
        return BCM_E_MEMORY;
    }

    /* Initialize the new qualifier set */
    BCM_FIELD_QSET_INIT(*p_new_qset);

    /* Transform the qulaifier set from the old format to new one */
    _bcm_compat657in_field_qset_t(&qset, p_new_qset);

    /* Call the BCM API with new qset format */
    rv = bcm_field_group_create_id(unit, *p_new_qset, pri, group);

    /* Deallocate the q set memory */
    sal_free(p_new_qset);

    return (rv);
}

/*
 * Function: bcm_compat657_field_qset_data_qualifier_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_qset_data_qualifier_add
 *      Used to add field data qualifier to group qset.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     qset - (IN/OUT) Old group qualifier set
 *     qualifier_id  - (IN) Data qualifier id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat657_field_qset_data_qualifier_add(int unit,
                                            bcm_compat657_field_qset_t *qset,
                                            int qual_id)
{
    int rv;
    bcm_field_qset_t *p_new_qset=NULL;

    /* Only tranform qualifier set if given a valid qset */
    if (qset) {

        /* Create the qualifier set from heap to avoid the stack to bloat */
        p_new_qset =
            (bcm_field_qset_t *)sal_alloc(sizeof(bcm_field_qset_t), "NewQset");
        if (NULL == p_new_qset) {
            return BCM_E_MEMORY;
        }

        /* Initialize the new qualifier set */
        BCM_FIELD_QSET_INIT(*p_new_qset);

        /* Transform the qualifier set from the old format to new one */
        _bcm_compat657in_field_qset_t(qset, p_new_qset);
    }

    /* Call the BCM API with new qset format */
    rv = bcm_field_qset_data_qualifier_add(unit, p_new_qset, qual_id);

    if (qset) {
        /* Copy back the qset from new to old format */
        if (BCM_SUCCESS(rv)) {
            _bcm_compat657out_field_qset_t(p_new_qset, qset);
        }

        /* Deallocate the q set memory */
        sal_free(p_new_qset);
    }

    return (rv);
}

#endif  /* BCM_RPC_SUPPORT */
