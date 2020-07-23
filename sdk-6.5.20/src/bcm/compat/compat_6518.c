/*
 * $Id: compat_6518.c,v 1.0 2019/04/03
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.18 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/qos.h>
#include <bcm/flexctr.h>
#include <bcm/collector.h>
#include <bcm/ifa.h>
#include <bcm_int/compat/compat_6518.h>
#include <bcm/init.h>
#include <bcm/l2.h>
#include <bcm/rx.h>
#include <bcm/srv6.h>
#include <bcm/port.h>
#include <bcm/tunnel.h>
#include <bcm/vxlan.h>
#include <bcm/l2gre.h>
/*
 * Function:
 *   _bcm_compat6518in_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_mirror_destination_t(
    bcm_compat6518_mirror_destination_t *from,
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
}

/*
 * Function:
 *   _bcm_compat6518out_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_mirror_destination_t(
    bcm_mirror_destination_t *from,
    bcm_compat6518_mirror_destination_t *to)
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
}

/*
 * Function:
 *   bcm_compat6518_mirror_destination_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_destination_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   mirror_dest    - (INOUT) Mirrored destination and encapsulation.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_mirror_destination_create(
    int unit,
    bcm_compat6518_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_create(unit, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6518out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_mirror_destination_get
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
bcm_compat6518_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6518_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_get(unit, mirror_dest_id, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6518out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_mirror_source_t
 * Purpose:
 *      Convert the bcm_mirror_source_t datatype from <=6.5.18 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_mirror_source_t(
    bcm_compat6518_mirror_source_t *from,
    bcm_mirror_source_t *to)
{
    bcm_mirror_source_t_init(to);
    to->type = from->type;
    to->flags = from->flags;
    to->port = from->port;
    to->trace_event = from->trace_event;
    to->drop_event = from->drop_event;
    to->sample_profile_id = from->sample_profile_id;
}

/*
 * Function:
 *      _bcm_compat6518out_mirror_source_t
 * Purpose:
 *      Convert the bcm_mirror_source_t datatype from SDK 6.5.18+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_mirror_source_t(
    bcm_mirror_source_t *from,
    bcm_compat6518_mirror_source_t *to)
{
    to->type = from->type;
    to->flags = from->flags;
    to->port = from->port;
    to->trace_event = from->trace_event;
    to->drop_event = from->drop_event;
    to->sample_profile_id = from->sample_profile_id;
}

/*
 * Function:
 *      bcm_compat6518_mirror_source_dest_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 *      mirror_dest_id - (IN) Mirror destination gport id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_mirror_source_dest_add(
    int unit,
    bcm_compat6518_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_add(unit, new_source, mirror_dest_id);

    if (rv >= 0) {
        _bcm_compat6518out_mirror_source_t(new_source, source);
    }

    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_mirror_source_dest_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      source - (IN) Mirror source pointer
 *      mirror_dest_id - (IN) Mirror destination gport id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_mirror_source_dest_delete(
    int unit,
    bcm_compat6518_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_delete(unit, new_source, mirror_dest_id);

    if (rv >= 0) {
        _bcm_compat6518out_mirror_source_t(new_source, source);
    }

    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_mirror_source_dest_delete_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_delete_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6518_mirror_source_t *source)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_delete_all(unit, new_source);

    if (rv >= 0) {
        _bcm_compat6518out_mirror_source_t(new_source, source);
    }

    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_mirror_source_dest_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 *      array_size - (IN) Size of allocated entries in mirror_dest array
 *      mirror_dest - (OUT) Array for mirror destinations information
 *      count - (OUT) Actual number of mirror destinations
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_mirror_source_dest_get_all(
    int unit,
    bcm_compat6518_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_get_all(unit, new_source, array_size, mirror_dest, count);

    if (rv >= 0) {
        _bcm_compat6518out_mirror_source_t(new_source, source);
    }

    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6518in_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype from <=6.5.18 to 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_flow_port_encap_t(
    bcm_compat6518_flow_port_encap_t *from,
    bcm_flow_port_encap_t *to)
{
    bcm_flow_port_encap_t_init(to);
    to->flow_port       = from->flow_port;
    to->options         = from->options;
    to->flags           = from->flags;
    to->class_id        = from->class_id;
    to->mtu             = from->mtu;
    to->network_group   = from->network_group;
    to->dvp_group       = from->dvp_group;
    to->pri             = from->pri;
    to->cfi             = from->cfi;
    to->tpid            = from->tpid;
    to->vlan            = from->vlan;
    to->egress_if       = from->egress_if;
    to->valid_elements  = from->valid_elements;
    to->flow_handle     = from->flow_handle;
    to->flow_option     = from->flow_option;
    to->ip4_id          = from->ip4_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
}

/*
 * Function:
 *      _bcm_compat6518out_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype f from 6.5.18+ to <=6.5.18.
 * Parameters:
 *      from        - (IN) The 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_flow_port_encap_t(
    bcm_flow_port_encap_t *from,
    bcm_compat6518_flow_port_encap_t *to)
{
    to->flow_port       = from->flow_port;
    to->options         = from->options;
    to->flags           = from->flags;
    to->class_id        = from->class_id;
    to->mtu             = from->mtu;
    to->network_group   = from->network_group;
    to->dvp_group       = from->dvp_group;
    to->pri             = from->pri;
    to->cfi             = from->cfi;
    to->tpid            = from->tpid;
    to->vlan            = from->vlan;
    to->egress_if       = from->egress_if;
    to->valid_elements  = from->valid_elements;
    to->flow_handle     = from->flow_handle;
    to->flow_option     = from->flow_option;
    to->ip4_id          = from->ip4_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
}

int
bcm_compat6518_flow_port_encap_set(
    int unit,
    bcm_compat6518_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_port_encap_t *)
            sal_alloc(sizeof(bcm_flow_port_encap_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_flow_port_encap_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_encap_set(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_flow_port_encap_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6518_flow_port_encap_get(
    int unit,
    bcm_compat6518_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_port_encap_t *)
            sal_alloc(sizeof(bcm_flow_port_encap_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_flow_port_encap_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_port_encap_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_flow_port_encap_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_l3_intf_t
 * Purpose:
 *      Convert the bcm_l3_intf_t datatype from <=6.5.18 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_l3_intf_t(
    bcm_compat6518_l3_intf_t *from,
    bcm_l3_intf_t *to)
{
    int i = 0;

    bcm_l3_intf_t_init(to);
    to->l3a_flags                  = from->l3a_flags;
    to->l3a_flags2                 = from->l3a_flags2;
    to->l3a_vrf                    = from->l3a_vrf;
    to->l3a_intf_id                = from->l3a_intf_id;
    sal_memcpy(to->l3a_mac_addr, from->l3a_mac_addr, sizeof(bcm_mac_t));
    to->l3a_vid                    = from->l3a_vid;
    to->l3a_inner_vlan             = from->l3a_inner_vlan;
    to->l3a_tunnel_idx             = from->l3a_tunnel_idx;
    to->l3a_ttl                    = from->l3a_ttl;
    to->l3a_mtu                    = from->l3a_mtu;
    to->l3a_mtu_forwarding         = from->l3a_mtu_forwarding;
    to->l3a_group                  = from->l3a_group;
    to->vlan_qos                   = from->vlan_qos;
    to->inner_vlan_qos             = from->inner_vlan_qos;
    to->dscp_qos                   = from->dscp_qos;
    to->l3a_intf_class             = from->l3a_intf_class;
    to->l3a_ip4_options_profile_id = from->l3a_ip4_options_profile_id;
    to->l3a_nat_realm_id           = from->l3a_nat_realm_id;
    to->outer_tpid                 = from->outer_tpid;
    to->l3a_intf_flags             = from->l3a_intf_flags;
    to->native_routing_vlan_tags   = from->native_routing_vlan_tags;
    to->l3a_source_vp              = from->l3a_source_vp;
    to->flow_handle                = from->flow_handle;
    to->flow_option_handle         = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i]      = from->logical_fields[i];
    }
    to->num_of_fields              = from->num_of_fields;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->stat_id                    = from->stat_id;
    to->stat_pp_profile            = from->stat_pp_profile;
    to->l3a_ttl_dec                = from->l3a_ttl_dec;
    to->opaque_ctrl_id             = from->opaque_ctrl_id;
    return;
}

/*
 * Function:
 *      _bcm_compat6518out_l3_intf_t
 * Purpose:
 *      Convert the bcm_l3_intf_t datatype from 6.5.18+ to
 *      SDK <=6.5.18
 * Parameters:
 *      from        - (IN) The 6.5.18+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_l3_intf_t(
    bcm_l3_intf_t *from,
    bcm_compat6518_l3_intf_t *to)
{
    int i = 0;

    to->l3a_flags                  = from->l3a_flags;
    to->l3a_flags2                 = from->l3a_flags2;
    to->l3a_vrf                    = from->l3a_vrf;
    to->l3a_intf_id                = from->l3a_intf_id;
    sal_memcpy(to->l3a_mac_addr, from->l3a_mac_addr, sizeof(bcm_mac_t));
    to->l3a_vid                    = from->l3a_vid;
    to->l3a_inner_vlan             = from->l3a_inner_vlan;
    to->l3a_tunnel_idx             = from->l3a_tunnel_idx;
    to->l3a_ttl                    = from->l3a_ttl;
    to->l3a_mtu                    = from->l3a_mtu;
    to->l3a_mtu_forwarding         = from->l3a_mtu_forwarding;
    to->l3a_group                  = from->l3a_group;
    to->vlan_qos                   = from->vlan_qos;
    to->inner_vlan_qos             = from->inner_vlan_qos;
    to->dscp_qos                   = from->dscp_qos;
    to->l3a_intf_class             = from->l3a_intf_class;
    to->l3a_ip4_options_profile_id = from->l3a_ip4_options_profile_id;
    to->l3a_nat_realm_id           = from->l3a_nat_realm_id;
    to->outer_tpid                 = from->outer_tpid;
    to->l3a_intf_flags             = from->l3a_intf_flags;
    to->native_routing_vlan_tags   = from->native_routing_vlan_tags;
    to->l3a_source_vp              = from->l3a_source_vp;
    to->flow_handle                = from->flow_handle;
    to->flow_option_handle         = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i]      = from->logical_fields[i];
    }
    to->num_of_fields              = from->num_of_fields;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->stat_id                    = from->stat_id;
    to->stat_pp_profile            = from->stat_pp_profile;
    to->l3a_ttl_dec                = from->l3a_ttl_dec;
    to->opaque_ctrl_id             = from->opaque_ctrl_id;
    return;
}

int
bcm_compat6518_l3_intf_create(
    int unit,
    bcm_compat6518_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_create(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6518_l3_intf_delete(
    int unit,
    bcm_compat6518_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_delete(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6518_l3_intf_find(
    int unit,
    bcm_compat6518_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_find(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6518_l3_intf_find_vlan(
    int unit,
    bcm_compat6518_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_find_vlan(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int
bcm_compat6518_l3_intf_get(
    int unit,
    bcm_compat6518_l3_intf_t *intf)
{
    int rv;
    bcm_l3_intf_t *new_intf = NULL;

    if (NULL != intf) {
        /* Create from heap to avoid the stack to bloat */
        new_intf = sal_alloc(sizeof(bcm_l3_intf_t), "New Egress Interface");
        if (NULL == new_intf) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_intf_get(unit, new_intf);
    if (NULL != new_intf) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_l3_intf_t(new_intf, intf);
        /* Deallocate memory*/
        sal_free(new_intf);
    }

    return rv;
}

int bcm_compat6518_tunnel_initiator_set(
    int unit,
    bcm_compat6518_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6519_tunnel_initiator_set(unit, new_intf, tunnel);


    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

int bcm_compat6518_tunnel_initiator_create(
    int unit,
    bcm_compat6518_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6519_tunnel_initiator_create(unit, new_intf, tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_l3_intf_t(new_intf, intf);

    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

int bcm_compat6518_tunnel_initiator_clear(
    int unit,
    bcm_compat6518_l3_intf_t *intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_clear(unit, new_intf);

    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

int bcm_compat6518_tunnel_initiator_get(
    int unit,
    bcm_compat6518_l3_intf_t *intf,
    bcm_compat6519_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t *new_intf = NULL;

    if (intf != NULL && tunnel != NULL) {
        new_intf = (bcm_l3_intf_t *)
                     sal_alloc(sizeof(bcm_l3_intf_t),
                     "New intf");
        if (new_intf == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_intf_t(intf, new_intf);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6519_tunnel_initiator_get(unit, new_intf, tunnel);

    /* Deallocate memory*/
    sal_free(new_intf);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_mpls_tunnel_encap_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_encap_t datatype from <=6.5.18 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_mpls_tunnel_encap_t(
    bcm_compat6518_mpls_tunnel_encap_t *from,
    bcm_mpls_tunnel_encap_t *to)
{
    bcm_mpls_tunnel_encap_t_init(to);

    to->tunnel_id     = from->tunnel_id;
    to->num_labels    = from->num_labels;
    sal_memcpy(to->label_array, from->label_array,
               sizeof(bcm_mpls_egress_label_t) * BCM_MPLS_EGRESS_LABEL_MAX);
    return;
}

/*
 * Function:
 *      _bcm_compat6518out_mpls_tunnel_encap_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_encap_t datatype from <=6.5.18 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_mpls_tunnel_encap_t(
    bcm_mpls_tunnel_encap_t *from,
    bcm_compat6518_mpls_tunnel_encap_t *to)
{
    to->tunnel_id    = from->tunnel_id;
    to->num_labels   = from->num_labels;
    sal_memcpy(to->label_array, from->label_array,
               sizeof(bcm_mpls_egress_label_t) * BCM_MPLS_EGRESS_LABEL_MAX);
    return;
}

int bcm_compat6518_mpls_tunnel_encap_create(
    int unit,
    uint32 options,
    bcm_compat6518_mpls_tunnel_encap_t *tunnel_encap)
{
    int rv;
    bcm_mpls_tunnel_encap_t *new_tunnel_encap = NULL;

    if (NULL != tunnel_encap) {
        /* Create from heap to avoid the stack to bloat */
        new_tunnel_encap = sal_alloc(sizeof(bcm_mpls_tunnel_encap_t),
                                     "New mpls tunnel encap");
        if (NULL == new_tunnel_encap) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mpls_tunnel_encap_t(tunnel_encap, new_tunnel_encap);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_encap_create(unit, options, new_tunnel_encap);
    if (NULL != new_tunnel_encap) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_mpls_tunnel_encap_t(new_tunnel_encap, tunnel_encap);
        /* Deallocate memory*/
        sal_free(new_tunnel_encap);
    }

    return rv;
}

int bcm_compat6518_mpls_tunnel_encap_get(
    int unit,
    bcm_compat6518_mpls_tunnel_encap_t *tunnel_encap)
{
    int rv;
    bcm_mpls_tunnel_encap_t *new_tunnel_encap = NULL;

    if (NULL != tunnel_encap) {
        /* Create from heap to avoid the stack to bloat */
        new_tunnel_encap = sal_alloc(sizeof(bcm_mpls_tunnel_encap_t),
                                     "New mpls tunnel encap");
        if (NULL == new_tunnel_encap) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mpls_tunnel_encap_t(tunnel_encap, new_tunnel_encap);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_tunnel_encap_get(unit, new_tunnel_encap);
    if (NULL != new_tunnel_encap) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_mpls_tunnel_encap_t(new_tunnel_encap, tunnel_encap);
        /* Deallocate memory*/
        sal_free(new_tunnel_encap);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from <=6.5.18 to
 *      SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_mpls_port_t(
    bcm_compat6518_mpls_port_t *from,
    bcm_mpls_port_t *to)
{
    bcm_mpls_port_t_init(to);

    to->mpls_port_id                 = from->mpls_port_id;
    to->flags                        = from->flags;
    to->flags2                       = from->flags2;
    to->if_class                     = from->if_class;
    to->exp_map                      = from->exp_map;
    to->int_pri                      = from->int_pri;
    to->pkt_pri                      = from->pkt_pri;
    to->pkt_cfi                      = from->pkt_cfi;
    to->service_tpid                 = from->service_tpid;
    to->port                         = from->port;
    to->criteria                     = from->criteria;
    to->match_vlan                   = from->match_vlan;
    to->match_inner_vlan             = from->match_inner_vlan;
    to->egress_tunnel_if             = from->egress_tunnel_if;
    to->egress_label                 = from->egress_label;
    to->mtu                          = from->mtu;
    to->egress_service_vlan          = from->egress_service_vlan;
    to->pw_seq_number                = from->pw_seq_number;
    to->encap_id                     = from->encap_id;
    to->ingress_failover_id          = from->ingress_failover_id;
    to->ingress_failover_port_id     = from->ingress_failover_port_id;
    to->failover_id                  = from->failover_id;
    to->failover_port_id             = from->failover_port_id;
    to->policer_id                   = from->policer_id;
    to->failover_mc_group            = from->failover_mc_group;
    to->pw_failover_id               = from->pw_failover_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->vccv_type                    = from->vccv_type;
    to->network_group_id             = from->network_group_id;
    to->match_subport_pkt_vid        = from->match_subport_pkt_vid;
    to->tunnel_id                    = from->tunnel_id;
    to->per_flow_queue_base          = from->per_flow_queue_base;
    to->qos_map_id                   = from->qos_map_id;
    to->egress_failover_id           = from->egress_failover_id;
    to->egress_failover_port_id      = from->egress_failover_port_id;
    to->ecn_map_id                   = from->ecn_map_id;
    to->class_id                     = from->class_id;
    to->egress_class_id              = from->egress_class_id;
    to->inlif_counting_profile       = from->inlif_counting_profile;
    to->egress_tunnel_label          = from->egress_tunnel_label;
    to->nof_service_tags             = from->nof_service_tags;
    to->ingress_qos_model            = from->ingress_qos_model;
    to->context_label                = from->context_label;
    to->ingress_if                   = from->ingress_if;

    return;
}

/*
 * Function:
 *      _bcm_compat6518out_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from SDK 6.5.18+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_mpls_port_t(
    bcm_mpls_port_t *from,
    bcm_compat6518_mpls_port_t *to)
{
    to->mpls_port_id                 = from->mpls_port_id;
    to->flags                        = from->flags;
    to->flags2                       = from->flags2;
    to->if_class                     = from->if_class;
    to->exp_map                      = from->exp_map;
    to->int_pri                      = from->int_pri;
    to->pkt_pri                      = from->pkt_pri;
    to->pkt_cfi                      = from->pkt_cfi;
    to->service_tpid                 = from->service_tpid;
    to->port                         = from->port;
    to->criteria                     = from->criteria;
    to->match_vlan                   = from->match_vlan;
    to->match_inner_vlan             = from->match_inner_vlan;
    to->egress_tunnel_if             = from->egress_tunnel_if;
    to->egress_label                 = from->egress_label;
    to->mtu                          = from->mtu;
    to->egress_service_vlan          = from->egress_service_vlan;
    to->pw_seq_number                = from->pw_seq_number;
    to->encap_id                     = from->encap_id;
    to->ingress_failover_id          = from->ingress_failover_id;
    to->ingress_failover_port_id     = from->ingress_failover_port_id;
    to->failover_id                  = from->failover_id;
    to->failover_port_id             = from->failover_port_id;
    to->policer_id                   = from->policer_id;
    to->failover_mc_group            = from->failover_mc_group;
    to->pw_failover_id               = from->pw_failover_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->pw_failover_port_id          = from->pw_failover_port_id;
    to->vccv_type                    = from->vccv_type;
    to->network_group_id             = from->network_group_id;
    to->match_subport_pkt_vid        = from->match_subport_pkt_vid;
    to->tunnel_id                    = from->tunnel_id;
    to->per_flow_queue_base          = from->per_flow_queue_base;
    to->qos_map_id                   = from->qos_map_id;
    to->egress_failover_id           = from->egress_failover_id;
    to->egress_failover_port_id      = from->egress_failover_port_id;
    to->ecn_map_id                   = from->ecn_map_id;
    to->class_id                     = from->class_id;
    to->egress_class_id              = from->egress_class_id;
    to->inlif_counting_profile       = from->inlif_counting_profile;
    to->egress_tunnel_label          = from->egress_tunnel_label;
    to->nof_service_tags             = from->nof_service_tags;
    to->ingress_qos_model            = from->ingress_qos_model;
    to->context_label                = from->context_label;
    to->ingress_if                   = from->ingress_if;

    return;
}

int bcm_compat6518_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6518_mpls_port_t *mpls_port)
{
    int rv;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (NULL != mpls_port) {
        /* Create from heap to avoid the stack to bloat */
        new_mpls_port = sal_alloc(sizeof(bcm_mpls_port_t), "New mpls port");
        if (NULL == new_mpls_port) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mpls_port_t(mpls_port, new_mpls_port);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_add(unit, vpn, new_mpls_port);
    if (NULL != new_mpls_port) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_mpls_port_t(new_mpls_port, mpls_port);
        /* Deallocate memory */
        sal_free(new_mpls_port);
    }

    return rv;
}

int bcm_compat6518_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6518_mpls_port_t *mpls_port)
{
    int rv;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (NULL != mpls_port) {
        /* Create from heap to avoid the stack to bloat */
        new_mpls_port = sal_alloc(sizeof(bcm_mpls_port_t), "New mpls port");
        if (NULL == new_mpls_port) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_mpls_port_t(mpls_port, new_mpls_port);
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get(unit, vpn, new_mpls_port);
    if (NULL != new_mpls_port) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_mpls_port_t(new_mpls_port, mpls_port);
        /* Deallocate memory */
        sal_free(new_mpls_port);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_mpls_port_get_all
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
int bcm_compat6518_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6518_mpls_port_t *port_array,
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
        _bcm_compat6518out_mpls_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory */
    sal_free(new_port_array);

    return rv;
}

#endif  /* defined(INCLUDE_L3) */
#if defined(INCLUDE_BFD)
/*
 * Function:
 *      _bcm_compat6518in_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_bfd_endpoint_info_t(
    bcm_compat6518_bfd_endpoint_info_t *from,
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
    sal_memset(to->dst_mac, 0, sizeof(bcm_mac_t));
    sal_memset(to->src_mac, 0, sizeof(bcm_mac_t));
    to->pkt_inner_vlan_id = 0;
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
    to->mpls_hdr = 0;
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
 *      _bcm_compat6518out_bfd_endpoint_info_t
 * Purpose:
 *      Convert the bcm_bfd_endpoint_info_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_bfd_endpoint_info_t(
    bcm_bfd_endpoint_info_t *from,
    bcm_compat6518_bfd_endpoint_info_t *to)
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
 *      bcm_compat6518_bfd_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (INOUT) Pointer to an BFD endpoint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_bfd_endpoint_create(
    int unit,
    bcm_compat6518_bfd_endpoint_info_t *endpoint_info)
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
        _bcm_compat6518in_bfd_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_bfd_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_endpoint_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure to receive the data.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6518_bfd_endpoint_info_t *endpoint_info)
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
    _bcm_compat6518out_bfd_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}
#endif /* defined(INCLUDE_BFD) */

/*
 * Function:
 *      _bcm_compat6518in_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from 6.5.18+ to
 *      SDK <=6.5.18
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_udf_pkt_format_info_t(
    bcm_compat6518_udf_pkt_format_info_t *from,
    bcm_udf_pkt_format_info_t *to)
{
    bcm_udf_pkt_format_info_t_init(to);
    to->prio                = from->prio;
    to->ethertype           = from->ethertype;
    to->ethertype_mask      = from->ethertype_mask;
    to->ip_protocol         = from->ip_protocol;
    to->ip_protocol_mask    = from->ip_protocol_mask;
    to->l2                  = from->l2;
    to->vlan_tag            = from->vlan_tag;
    to->outer_ip            = from->outer_ip;
    to->inner_ip            = from->inner_ip;
    to->tunnel              = from->tunnel;
    to->mpls                = from->mpls;
    to->fibre_chan_outer    = from->fibre_chan_outer;
    to->fibre_chan_inner    = from->fibre_chan_inner;
    to->higig               = from->higig;
    to->vntag               = from->vntag;
    to->etag                = from->etag;
    to->cntag               = from->cntag;
    to->icnm                = from->icnm;
    to->subport_tag         = from->subport_tag;
    to->class_id            = from->class_id;
    to->inner_protocol      = from->inner_protocol;
    to->inner_protocol_mask = from->inner_protocol_mask;
    to->l4_dst_port         = from->l4_dst_port;
    to->l4_dst_port_mask    = from->l4_dst_port_mask;
    to->opaque_tag_type     = from->opaque_tag_type;
    to->opaque_tag_type_mask = from->opaque_tag_type_mask;
    to->int_pkt             = from->int_pkt;
    to->src_port            = from->src_port;
    to->src_port_mask       = from->src_port_mask;
    to->lb_pkt_type         = from->lb_pkt_type;
    to->first_2bytes_after_mpls_bos = from->first_2bytes_after_mpls_bos;
    to->first_2bytes_after_mpls_bos_mask = from->first_2bytes_after_mpls_bos_mask;
    return;
}

/*
 * Function:
 *      _bcm_compat6518out_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from 6.5.18+ to
 *      SDK <=6.5.18
 * Parameters:
 *      from        - (IN) The 6.5.18+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_udf_pkt_format_info_t(
    bcm_udf_pkt_format_info_t *from,
    bcm_compat6518_udf_pkt_format_info_t *to)
{
    to->prio                = from->prio;
    to->ethertype           = from->ethertype;
    to->ethertype_mask      = from->ethertype_mask;
    to->ip_protocol         = from->ip_protocol;
    to->ip_protocol_mask    = from->ip_protocol_mask;
    to->l2                  = from->l2;
    to->vlan_tag            = from->vlan_tag;
    to->outer_ip            = from->outer_ip;
    to->inner_ip            = from->inner_ip;
    to->tunnel              = from->tunnel;
    to->mpls                = from->mpls;
    to->fibre_chan_outer    = from->fibre_chan_outer;
    to->fibre_chan_inner    = from->fibre_chan_inner;
    to->higig               = from->higig;
    to->vntag               = from->vntag;
    to->etag                = from->etag;
    to->cntag               = from->cntag;
    to->icnm                = from->icnm;
    to->subport_tag         = from->subport_tag;
    to->class_id            = from->class_id;
    to->inner_protocol      = from->inner_protocol;
    to->inner_protocol_mask = from->inner_protocol_mask;
    to->l4_dst_port         = from->l4_dst_port;
    to->l4_dst_port_mask    = from->l4_dst_port_mask;
    to->opaque_tag_type     = from->opaque_tag_type;
    to->opaque_tag_type_mask = from->opaque_tag_type_mask;
    to->int_pkt             = from->int_pkt;
    to->src_port            = from->src_port;
    to->src_port_mask       = from->src_port_mask;
    to->lb_pkt_type         = from->lb_pkt_type;
    to->first_2bytes_after_mpls_bos = from->first_2bytes_after_mpls_bos;
    to->first_2bytes_after_mpls_bos_mask = from->first_2bytes_after_mpls_bos_mask;
    return;
}

int
bcm_compat6518_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_compat6518_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    int rv;
    bcm_udf_pkt_format_info_t *new_pkt_format = NULL;

    if (pkt_format != NULL) {
        new_pkt_format = sal_alloc(sizeof (bcm_udf_pkt_format_info_t),
                "New Pkt fmt");
        if (new_pkt_format == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6518in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_create(unit, options,
            new_pkt_format, pkt_format_id);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6518out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

int
bcm_compat6518_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_compat6518_udf_pkt_format_info_t *pkt_format)
{
    int rv;
    bcm_udf_pkt_format_info_t *new_pkt_format = NULL;

    if (pkt_format != NULL) {
        new_pkt_format = sal_alloc(sizeof (bcm_udf_pkt_format_info_t),
                "New Pkt fmt");
        if (new_pkt_format == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6518in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_info_get(unit, pkt_format_id, new_pkt_format);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6518out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_qos_map_t
 * Purpose:
 *   Convert the bcm_qos_map_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_qos_map_t(
    bcm_compat6518_qos_map_t *from,
    bcm_qos_map_t *to)
{
    bcm_qos_map_t_init(to);
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->dscp = from->dscp;
    to->exp = from->exp;
    to->int_pri = from->int_pri;
    to->color = from->color;
    to->remark_int_pri = from->remark_int_pri;
    to->remark_color = from->remark_color;
    to->policer_offset = from->policer_offset;
    to->queue_offset = from->queue_offset;
    to->port_offset = from->port_offset;
    to->etag_pcp = from->etag_pcp;
    to->etag_de = from->etag_de;
    to->counter_offset = from->counter_offset;
    to->inherited_dscp_exp = from->inherited_dscp_exp;
    to->opcode = from->opcode;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_pkt_cfi= from->inner_pkt_cfi;
    to->int_cn= from->int_cn;
    to->pkt_ecn = from->pkt_ecn;
    to->responsive = from->responsive;
    to->latency_marked = from->latency_marked;
}

/*
 * Function:
 *   _bcm_compat6518out_qos_map_t
 * Purpose:
 *   Convert the bcm_qos_map_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_qos_map_t(
    bcm_qos_map_t *from,
    bcm_compat6518_qos_map_t *to)
{
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->dscp = from->dscp;
    to->exp = from->exp;
    to->int_pri = from->int_pri;
    to->color = from->color;
    to->remark_int_pri = from->remark_int_pri;
    to->remark_color = from->remark_color;
    to->policer_offset = from->policer_offset;
    to->queue_offset = from->queue_offset;
    to->port_offset = from->port_offset;
    to->etag_pcp = from->etag_pcp;
    to->etag_de = from->etag_de;
    to->counter_offset = from->counter_offset;
    to->inherited_dscp_exp = from->inherited_dscp_exp;
    to->opcode = from->opcode;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_pkt_cfi= from->inner_pkt_cfi;
    to->int_cn= from->int_cn;
    to->pkt_ecn = from->pkt_ecn;
    to->responsive = from->responsive;
    to->latency_marked = from->latency_marked;
}

/*
 * Function:
 *   bcm_compat6518_qos_map_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_qos_map_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   flags          - (IN) Configuration flags.
 *   map            - (IN) Pointer to a qos map structure.
 *   map_id         - (IN) QoS map ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_qos_map_add(
    int unit,
    uint32 flags,
    bcm_compat6518_qos_map_t *map,
    int map_id)
{
    int rv = 0;
    bcm_qos_map_t *new_map = NULL;

    if (map != NULL) {
        new_map = (bcm_qos_map_t*)
            sal_alloc(sizeof(bcm_qos_map_t), "New map");
        if (new_map == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_qos_map_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_qos_map_add(unit, flags, new_map, map_id);

    if (rv >= 0) {
        _bcm_compat6518out_qos_map_t(new_map, map);
    }

    sal_free(new_map);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_qos_map_multi_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_qos_map_multi_get.
 * Parameters:
 *   unit             - (IN) BCM device number.
 *   flags            - (IN) Configuration flags.
 *   map_id           - (IN) QoS map ID.
 *   array_size       - (IN) Number of elements in array parameter; 0 to query.
 *   array            - (OUT) Storage location for QoS mapping.
 *   array_count      - (OUT) Number of mappings retrieved.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_qos_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_compat6518_qos_map_t *array,
    int *array_count)
{
    int rv = 0, i;
    bcm_qos_map_t *new_array = NULL;

    if (array_size <= 0) {
        return bcm_qos_map_multi_get(unit, flags, map_id, array_size,
                                     (bcm_qos_map_t *)array, array_count);
    }

    if (array != NULL) {
        new_array = (bcm_qos_map_t*)
            sal_alloc(sizeof(bcm_qos_map_t) * array_size, "New array");
        if (new_array == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < array_size; i++) {
            _bcm_compat6518in_qos_map_t(&array[i], &new_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size,
                               new_array, array_count);

    if (rv >= 0) {
        for (i = 0; i < array_size; i++) {
            _bcm_compat6518out_qos_map_t(&new_array[i], &array[i]);
        }
    }

    sal_free(new_array);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_qos_map_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_qos_map_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   flags          - (IN) Configuration flags.
 *   map            - (IN) Pointer to a qos map structure.
 *   map_id         - (IN) QoS map ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_qos_map_delete(
    int unit,
    uint32 flags,
    bcm_compat6518_qos_map_t *map,
    int map_id)
{
    int rv = 0;
    bcm_qos_map_t *new_map = NULL;

    if (map != NULL) {
        new_map = (bcm_qos_map_t*)
            sal_alloc(sizeof(bcm_qos_map_t), "New map");
        if (new_map == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_qos_map_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_qos_map_delete(unit, flags, new_map, map_id);

    if (rv >= 0) {
        _bcm_compat6518out_qos_map_t(new_map, map);
    }

    sal_free(new_map);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_ecn_traffic_action_config_t
 * Purpose:
 *      Convert the bcm_ecn_traffic_action_config_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_ecn_traffic_action_config_t(
    bcm_compat6518_ecn_traffic_action_config_t *from,
    bcm_ecn_traffic_action_config_t *to)
{
    bcm_ecn_traffic_action_config_t_init(to);
    to->action_type = from->action_type;
    to->action_flags = from->action_flags;
    to->int_cn = from->int_cn;
    to->color = from->color;
    to->ecn = from->ecn;
    to->new_ecn = from->new_ecn;
    to->congested_int_cn = from->congested_int_cn;
    to->non_congested_int_cn = from->non_congested_int_cn;
    to->responsive = from->responsive;
}

/*
 * Function:
 *      _bcm_compat6518out_ecn_traffic_action_config_t
 * Purpose:
 *      Convert the bcm_ecn_traffic_action_config_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_ecn_traffic_action_config_t(
    bcm_ecn_traffic_action_config_t *from,
    bcm_compat6518_ecn_traffic_action_config_t *to)
{
    to->action_type = from->action_type;
    to->action_flags = from->action_flags;
    to->int_cn = from->int_cn;
    to->color = from->color;
    to->ecn = from->ecn;
    to->new_ecn = from->new_ecn;
    to->congested_int_cn = from->congested_int_cn;
    to->non_congested_int_cn = from->non_congested_int_cn;
    to->responsive = from->responsive;
}

/*
 * Function:
 *      bcm_compat6518_ecn_traffic_action_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_action_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecn_config - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_ecn_traffic_action_config_get(
    int unit,
    bcm_compat6518_ecn_traffic_action_config_t *ecn_config)
{
    int rv = BCM_E_NONE;
    bcm_ecn_traffic_action_config_t *new_ecn_config = NULL;

    if (ecn_config != NULL) {
        new_ecn_config = (bcm_ecn_traffic_action_config_t *)
                     sal_alloc(sizeof(bcm_ecn_traffic_action_config_t),
                     "New ecn_config");
        if (new_ecn_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_ecn_traffic_action_config_t(ecn_config, new_ecn_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_action_config_get(unit, new_ecn_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_ecn_traffic_action_config_t(new_ecn_config, ecn_config);

    /* Deallocate memory*/
    sal_free(new_ecn_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_ecn_traffic_action_config_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_action_config_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecn_config - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_ecn_traffic_action_config_set(
    int unit,
    bcm_compat6518_ecn_traffic_action_config_t *ecn_config)
{
    int rv = BCM_E_NONE;
    bcm_ecn_traffic_action_config_t *new_ecn_config = NULL;

    if (ecn_config != NULL) {
        new_ecn_config = (bcm_ecn_traffic_action_config_t *)
                     sal_alloc(sizeof(bcm_ecn_traffic_action_config_t),
                     "New ecn_config");
        if (new_ecn_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_ecn_traffic_action_config_t(ecn_config, new_ecn_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_action_config_set(unit, new_ecn_config);


    /* Deallocate memory*/
    sal_free(new_ecn_config);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_ecn_traffic_map_info_t
 * Purpose:
 *      Convert the bcm_ecn_traffic_map_info_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_ecn_traffic_map_info_t(
    bcm_compat6518_ecn_traffic_map_info_t *from,
    bcm_ecn_traffic_map_info_t *to)
{
    bcm_ecn_traffic_map_info_t_init(to);
    to->flags = from->flags;
    to->ecn = from->ecn;
    to->int_cn = from->int_cn;
    to->tunnel_ecn = from->tunnel_ecn;
    to->tunnel_type = from->tunnel_type;
}

/*
 * Function:
 *      _bcm_compat6518out_ecn_traffic_map_info_t
 * Purpose:
 *      Convert the bcm_ecn_traffic_map_info_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_ecn_traffic_map_info_t(
    bcm_ecn_traffic_map_info_t *from,
    bcm_compat6518_ecn_traffic_map_info_t *to)
{
    to->flags = from->flags;
    to->ecn = from->ecn;
    to->int_cn = from->int_cn;
    to->tunnel_ecn = from->tunnel_ecn;
    to->tunnel_type = from->tunnel_type;
}

/*
 * Function:
 *      bcm_compat6518_ecn_traffic_map_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_map_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      map - (INOUT) Internal congestion notification map configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_ecn_traffic_map_get(
    int unit,
    bcm_compat6518_ecn_traffic_map_info_t *map)
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
        _bcm_compat6518in_ecn_traffic_map_info_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_map_get(unit, new_map);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_ecn_traffic_map_info_t(new_map, map);

    /* Deallocate memory*/
    sal_free(new_map);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_ecn_traffic_map_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_map_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      map - (IN) Internal congestion notification map configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_ecn_traffic_map_set(
    int unit,
    bcm_compat6518_ecn_traffic_map_info_t *map)
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
        _bcm_compat6518in_ecn_traffic_map_info_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_map_set(unit, new_map);


    /* Deallocate memory*/
    sal_free(new_map);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6518in_ecn_map_t
 * Purpose:
 *      Convert the bcm_ecn_map_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_ecn_map_t(
    bcm_compat6518_ecn_map_t *from,
    bcm_ecn_map_t *to)
{
    bcm_ecn_map_t_init(to);
    to->action_flags = from->action_flags;
    to->int_cn = from->int_cn;
    to->inner_ecn = from->inner_ecn;
    to->ecn = from->ecn;
    to->exp = from->exp;
    to->new_ecn = from->new_ecn;
    to->new_exp = from->new_exp;
    to->nonip_action_flags = from->nonip_action_flags;
}

/*
 * Function:
 *      _bcm_compat6518out_ecn_map_t
 * Purpose:
 *      Convert the bcm_ecn_map_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_ecn_map_t(
    bcm_ecn_map_t *from,
    bcm_compat6518_ecn_map_t *to)
{
    to->action_flags = from->action_flags;
    to->int_cn = from->int_cn;
    to->inner_ecn = from->inner_ecn;
    to->ecn = from->ecn;
    to->exp = from->exp;
    to->new_ecn = from->new_ecn;
    to->new_exp = from->new_exp;
    to->nonip_action_flags = from->nonip_action_flags;
}

/*
 * Function:
 *      bcm_compat6518_ecn_map_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_map_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) BCM ecn options.
 *      ecn_map_id - (IN) BCM ecn map id.
 *      ecn_map - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_ecn_map_set(
    int unit,
    uint32 options,
    int ecn_map_id,
    bcm_compat6518_ecn_map_t *ecn_map)
{
    int rv = BCM_E_NONE;
    bcm_ecn_map_t *new_ecn_map = NULL;

    if (ecn_map != NULL) {
        new_ecn_map = (bcm_ecn_map_t *)
                     sal_alloc(sizeof(bcm_ecn_map_t),
                     "New ecn_map");
        if (new_ecn_map == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_ecn_map_t(ecn_map, new_ecn_map);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_map_set(unit, options, ecn_map_id, new_ecn_map);


    /* Deallocate memory*/
    sal_free(new_ecn_map);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_ecn_map_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_map_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecn_map_id - (IN) BCM ecn map id.
 *      ecn_map - (INOUT) BCM ecn map structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_ecn_map_get(
    int unit,
    int ecn_map_id,
    bcm_compat6518_ecn_map_t *ecn_map)
{
    int rv = BCM_E_NONE;
    bcm_ecn_map_t *new_ecn_map = NULL;

    if (ecn_map != NULL) {
        new_ecn_map = (bcm_ecn_map_t *)
                     sal_alloc(sizeof(bcm_ecn_map_t),
                     "New ecn_map");
        if (new_ecn_map == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_ecn_map_t(ecn_map, new_ecn_map);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_map_get(unit, ecn_map_id, new_ecn_map);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_ecn_map_t(new_ecn_map, ecn_map);

    /* Deallocate memory*/
    sal_free(new_ecn_map);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_l3_ecmp_dlb_mon_cfg_t
 * Purpose:
 *      Convert the bcm_l3_ecmp_dlb_mon_cfg_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_l3_ecmp_dlb_mon_cfg_t(
    bcm_compat6518_l3_ecmp_dlb_mon_cfg_t *from,
    bcm_l3_ecmp_dlb_mon_cfg_t *to)
{
    bcm_l3_ecmp_dlb_mon_cfg_t_init(to);
    to->sample_rate = from->sample_rate;
    to->action = from->action;
    to->enable = from->enable;
}

/*
 * Function:
 *      _bcm_compat6518out_l3_ecmp_dlb_mon_cfg_t
 * Purpose:
 *      Convert the bcm_l3_ecmp_dlb_mon_cfg_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_l3_ecmp_dlb_mon_cfg_t(
    bcm_l3_ecmp_dlb_mon_cfg_t *from,
    bcm_compat6518_l3_ecmp_dlb_mon_cfg_t *to)
{
    to->sample_rate = from->sample_rate;
    to->action = from->action;
    to->enable = from->enable;
}

/*
 * Function:
 *      bcm_compat6518_l3_ecmp_dlb_mon_config_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_dlb_mon_config_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp_intf - (IN) ECMP interface id obtained from bcm_l3_ecmp_create or bcm_l3_egress_ecmp_create
 *      dlb_mon_cfg - (IN) DLB flow monitoring configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l3_ecmp_dlb_mon_config_set(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_compat6518_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    int rv = BCM_E_NONE;
    bcm_l3_ecmp_dlb_mon_cfg_t *new_dlb_mon_cfg = NULL;

    if (dlb_mon_cfg != NULL) {
        new_dlb_mon_cfg = (bcm_l3_ecmp_dlb_mon_cfg_t *)
                     sal_alloc(sizeof(bcm_l3_ecmp_dlb_mon_cfg_t),
                     "New dlb_mon_cfg");
        if (new_dlb_mon_cfg == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l3_ecmp_dlb_mon_cfg_t(dlb_mon_cfg, new_dlb_mon_cfg);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_dlb_mon_config_set(unit, ecmp_intf, new_dlb_mon_cfg);


    /* Deallocate memory*/
    sal_free(new_dlb_mon_cfg);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_l3_ecmp_dlb_mon_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_dlb_mon_config_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      intf - (IN)
 *      dlb_mon_cfg - (OUT) DLB flow monitoring configuration read from hardware
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l3_ecmp_dlb_mon_config_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_ecmp_dlb_mon_cfg_t *dlb_mon_cfg)
{
    int rv = BCM_E_NONE;
    bcm_l3_ecmp_dlb_mon_cfg_t *new_dlb_mon_cfg = NULL;

    if (dlb_mon_cfg != NULL) {
        new_dlb_mon_cfg = (bcm_l3_ecmp_dlb_mon_cfg_t *)
                     sal_alloc(sizeof(bcm_l3_ecmp_dlb_mon_cfg_t),
                     "New dlb_mon_cfg");
        if (new_dlb_mon_cfg == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_dlb_mon_config_get(unit, intf, new_dlb_mon_cfg);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_l3_ecmp_dlb_mon_cfg_t(new_dlb_mon_cfg, dlb_mon_cfg);

    /* Deallocate memory*/
    sal_free(new_dlb_mon_cfg);

    return rv;
}

#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *   _bcm_compat6518in_flexctr_index_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_index_operation_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_flexctr_index_operation_t(
    bcm_compat6518_flexctr_index_operation_t *from,
    bcm_flexctr_index_operation_t *to)
{
    int i = 0;

    for (i = 0; i < BCM_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
}

/*
 * Function:
 *   _bcm_compat6518out_flexctr_index_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_index_operation_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_flexctr_index_operation_t(
    bcm_flexctr_index_operation_t *from,
    bcm_compat6518_flexctr_index_operation_t *to)
{
    int i = 0;

    for (i = 0; i < BCM_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
}

/*
 * Function:
 *   _bcm_compat6518in_flexctr_value_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_value_operation_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_flexctr_value_operation_t(
    bcm_compat6518_flexctr_value_operation_t *from,
    bcm_flexctr_value_operation_t *to)
{
    int i = 0;

    to->select = from->select;
    for (i = 0; i < BCM_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
    to->type = from->type;
}

/*
 * Function:
 *   _bcm_compat6518out_flexctr_value_operation_t
 * Purpose:
 *   Convert the bcm_flexctr_value_operation_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_flexctr_value_operation_t(
    bcm_flexctr_value_operation_t *from,
    bcm_compat6518_flexctr_value_operation_t *to)
{
    int i = 0;

    to->select = from->select;
    for (i = 0; i < BCM_FLEXCTR_OPERATION_OBJECT_SIZE; i++) {
        to->object[i] = from->object[i];
        to->quant_id[i] = from->quant_id[i];
        to->mask_size[i] = from->mask_size[i];
        to->shift[i] = from->shift[i];
    }
    to->type = from->type;
}

/*
 * Function:
 *   _bcm_compat6518in_flexctr_trigger_t
 * Purpose:
 *   Convert the bcm_flexctr_trigger_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_flexctr_trigger_t(
    bcm_compat6518_flexctr_trigger_t *from,
    bcm_flexctr_trigger_t *to)
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
 *   _bcm_compat6518out_flexctr_trigger_t
 * Purpose:
 *   Convert the bcm_flexctr_trigger_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_flexctr_trigger_t(
    bcm_flexctr_trigger_t *from,
    bcm_compat6518_flexctr_trigger_t *to)
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
 *   _bcm_compat6518in_flexctr_action_t
 * Purpose:
 *   Convert the bcm_flexctr_action_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_flexctr_action_t(
    bcm_compat6518_flexctr_action_t *from,
    bcm_flexctr_action_t *to)
{
    bcm_flexctr_action_t_init(to);
    to->flags = from->flags;
    to->source = from->source;
    to->ports = from->ports;
    to->hint = from->hint;
    to->drop_count_mode = from->drop_count_mode;
    to->exception_drop_count_enable = from->exception_drop_count_enable;
    to->egress_mirror_count_enable = from->egress_mirror_count_enable;
    to->mode = from->mode;
    to->index_num = from->index_num;
    _bcm_compat6518in_flexctr_index_operation_t(&from->index_operation,
                                                &to->index_operation);
    _bcm_compat6518in_flexctr_value_operation_t(&from->operation_a,
                                                &to->operation_a);
    _bcm_compat6518in_flexctr_value_operation_t(&from->operation_b,
                                                &to->operation_b);
    _bcm_compat6518in_flexctr_trigger_t(&from->trigger, &to->trigger);
}

/*
 * Function:
 *   _bcm_compat6518out_flexctr_action_t
 * Purpose:
 *   Convert the bcm_flexctr_action_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_flexctr_action_t(
    bcm_flexctr_action_t *from,
    bcm_compat6518_flexctr_action_t *to)
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
    _bcm_compat6518out_flexctr_index_operation_t(&from->index_operation,
                                                 &to->index_operation);
    _bcm_compat6518out_flexctr_value_operation_t(&from->operation_a,
                                                 &to->operation_a);
    _bcm_compat6518out_flexctr_value_operation_t(&from->operation_b,
                                                 &to->operation_b);
    _bcm_compat6518out_flexctr_trigger_t(&from->trigger, &to->trigger);
}

/*
 * Function:
 *   bcm_compat6518_flexctr_action_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_action_create.
 * Parameters:
 *   unit              - (IN) Unit number.
 *   options           - (IN) BCM_FLEXCTR_OPTIONS_XXX options.
 *   action            - (IN) Flex counter action data structure.
 *   stat_counter_id   - (IN/OUT) Flex counter ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_flexctr_action_create(
    int unit,
    int options,
    bcm_compat6518_flexctr_action_t *action,
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
        _bcm_compat6518in_flexctr_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_action_create(unit, options, new_action, stat_counter_id);

    if (rv >= 0) {
        _bcm_compat6518out_flexctr_action_t(new_action, action);
    }

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *   bcm_compat6518_flexctr_action_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_action_get.
 * Parameters:
 *   unit              - (IN) Unit number.
 *   stat_counter_id   - (IN) Flex counter ID.
 *   action            - (OUT) Flex counter action data structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_flexctr_action_get(
    int unit,
    uint32 stat_counter_id,
    bcm_compat6518_flexctr_action_t *action)
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
        _bcm_compat6518in_flexctr_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_action_get(unit, stat_counter_id, new_action);

    if (rv >= 0) {
        _bcm_compat6518out_flexctr_action_t(new_action, action);
    }

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_flexctr_quantization_t
 * Purpose:
 *   Convert the bcm_flexctr_quantization_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_flexctr_quantization_t(
    bcm_compat6518_flexctr_quantization_t *from,
    bcm_flexctr_quantization_t *to)
{
    int i = 0;

    bcm_flexctr_quantization_t_init(to);
    to->object = from->object;
    to->ports = from->ports;
    for (i = 0; i < BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }
    to->range_num = from->range_num;
}

/*
 * Function:
 *   _bcm_compat6518out_flexctr_quantization_t
 * Purpose:
 *   Convert the bcm_flexctr_quantization_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_flexctr_quantization_t(
    bcm_flexctr_quantization_t *from,
    bcm_compat6518_flexctr_quantization_t *to)
{
    int i = 0;

    to->object = from->object;
    to->ports = from->ports;
    for (i = 0; i < BCM_FLEXCTR_QUANTIZATION_RANGE_SIZE; i++) {
        to->range_check_min[i] = from->range_check_min[i];
        to->range_check_max[i] = from->range_check_max[i];
    }
    to->range_num = from->range_num;
}

/*
 * Function:
 *   bcm_compat6518_flexctr_quantization_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_quantization_create.
 * Parameters:
 *   unit         - (IN) Unit number.
 *   options      - (IN) BCM_FLEXCTR_OPTIONS_XXX options.
 *   quantization - (IN) Flex counter quantization data structure.
 *   quant_id     - (IN/OUT) Flex counter quantization ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_flexctr_quantization_create(
    int unit,
    int options,
    bcm_compat6518_flexctr_quantization_t *quantization,
    uint32 *quant_id)
{
    int rv = BCM_E_NONE;
    bcm_flexctr_quantization_t *new_quantization = NULL;

    if (quantization != NULL) {
        new_quantization = (bcm_flexctr_quantization_t *)
                           sal_alloc(sizeof(bcm_flexctr_quantization_t),
                           "New quantization");
        if (new_quantization == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_flexctr_quantization_t(quantization,
                                                 new_quantization);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_quantization_create(unit, options, new_quantization,
                                         quant_id);

    if (rv >= 0) {
        _bcm_compat6518out_flexctr_quantization_t(new_quantization,
                                                  quantization);
    }

    /* Deallocate memory*/
    sal_free(new_quantization);

    return rv;
}

/*
 * Function:
 *   bcm_compat6518_flexctr_quantization_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_quantization_get.
 * Parameters:
 *   unit         - (IN) Unit number.
 *   quant_id     - (IN) Flex counter quantization ID.
 *   quantization - (OUT) Flex counter quantization data structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_flexctr_quantization_get(
    int unit,
    uint32 quant_id,
    bcm_compat6518_flexctr_quantization_t *quantization)
{
    int rv = BCM_E_NONE;
    bcm_flexctr_quantization_t *new_quantization = NULL;

    if (quantization != NULL) {
        new_quantization = (bcm_flexctr_quantization_t *)
                           sal_alloc(sizeof(bcm_flexctr_quantization_t),
                           "New quantization");
        if (new_quantization == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_flexctr_quantization_t(quantization,
                                                 new_quantization);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flexctr_quantization_get(unit, quant_id, new_quantization);

    if (rv >= 0) {
        _bcm_compat6518out_flexctr_quantization_t(new_quantization,
                                                  quantization);
    }

    /* Deallocate memory*/
    sal_free(new_quantization);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_cosq_ebst_data_entry_t
 * Purpose:
 *   Convert the bcm_cosq_ebst_data_entry_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to           - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_cosq_ebst_data_entry_t(
    bcm_compat6518_cosq_ebst_data_entry_t *from,
    bcm_cosq_ebst_data_entry_t *to)
{
    sal_memset(to, 0, sizeof(bcm_cosq_ebst_data_entry_t));
    to->timestamp                    = from->timestamp;
    to->bytes                        = from->bytes;
}

/*
 * Function:
 *      _bcm_compat6518out_cosq_ebst_data_entry_t
 * Purpose:
 *      Convert the bcm_cosq_ebst_data_entry_t datatype from 6.5.18+ to
 *      <=6.5.18
 * Parameters:
 *      from       - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_cosq_ebst_data_entry_t(
    bcm_cosq_ebst_data_entry_t *from,
    bcm_compat6518_cosq_ebst_data_entry_t *to)
{
    to->timestamp                    = from->timestamp;
    to->bytes                        = from->bytes;
}

/*
 * Function: bcm_compat6518_cosq_ebst_data_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_ebst_data_get.
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to get
 *      discard - (IN/OUT) Discard settings
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6518_cosq_ebst_data_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6518_cosq_ebst_data_entry_t *entry_array,
    int *count)
{
    int rv = BCM_E_NONE, i;
    bcm_cosq_ebst_data_entry_t *new_entry_array = NULL;

    /* Error case and low level driver will return BCM_E_PARAM */
    if (array_size <= 0) {
        return bcm_cosq_ebst_data_get(unit, object_id, bid, array_size,
                                      (bcm_cosq_ebst_data_entry_t *)entry_array,
                                      count);
    }

    if (entry_array != NULL) {
        new_entry_array = (bcm_cosq_ebst_data_entry_t *)
                          sal_alloc(sizeof(bcm_cosq_ebst_data_entry_t) *
                          array_size,
                          "New entry array");
        if (new_entry_array == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        for (i = 0; i < array_size; i++) {
            _bcm_compat6518in_cosq_ebst_data_entry_t(&entry_array[i],
                                                     &new_entry_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_ebst_data_get(unit, object_id, bid, array_size,
                                new_entry_array, count);

    if (rv >= 0) {
        for (i = 0; i < array_size; i++) {
            _bcm_compat6518out_cosq_ebst_data_entry_t(&new_entry_array[i],
                                                      &entry_array[i]);
        }
    }
    /* Deallocate memory*/
    sal_free(new_entry_array);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_switch_pkt_control_action_t
 * Purpose:
 *   Convert the bcm_compat6518_switch_pkt_control_action_t datatype from
 *   <=6.5.18 to SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_switch_pkt_control_action_t(
    bcm_compat6518_switch_pkt_control_action_t *from,
    bcm_switch_pkt_control_action_t *to)
{
    bcm_switch_pkt_control_action_t_init(to);
    to->discard           = from->discard;
    to->copy_to_cpu       = from->copy_to_cpu;
    to->flood             = from->flood;
    to->bpdu              = from->bpdu;
    to->opaque_ctrl_id    = from->opaque_ctrl_id;
}

/*
 * Function:
 *   _bcm_compat6518in_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_compat6518_switch_pkt_protocol_match_t datatype from
 *   <=6.5.18 to SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_switch_pkt_protocol_match_t(
    bcm_compat6518_switch_pkt_protocol_match_t *from,
    bcm_switch_pkt_protocol_match_t *to)
{
    bcm_switch_pkt_protocol_match_t_init(to);
    to->l2_iif_opaque_ctrl_id            = from->l2_iif_opaque_ctrl_id;
    to->l2_iif_opaque_ctrl_id_mask       = from->l2_iif_opaque_ctrl_id_mask;
    to->vfi                              = from->vfi;
    to->vfi_mask                         = from->vfi_mask;
    to->ethertype                        = from->ethertype;
    to->ethertype_mask                   = from->ethertype_mask;
    to->arp_rarp_opcode                  = from->arp_rarp_opcode;
    to->arp_rarp_opcode_mask             = from->arp_rarp_opcode_mask;
    to->icmp_type                        = from->icmp_type;
    to->icmp_type_mask                   = from->icmp_type_mask;
    to->igmp_type                        = from->igmp_type;
    to->igmp_type_mask                   = from->igmp_type_mask;
    to->l4_valid                         = from->l4_valid;
    to->l4_valid_mask                    = from->l4_valid_mask;
    to->l4_src_port                      = from->l4_src_port;
    to->l4_src_port_mask                 = from->l4_src_port_mask;
    to->l4_dst_port                      = from->l4_dst_port;
    to->l4_dst_port_mask                 = from->l4_dst_port_mask;
    to->l5_bytes_0_1                     = from->l5_bytes_0_1;
    to->l5_bytes_0_1_mask                = from->l5_bytes_0_1_mask;
    to->outer_l5_bytes_0_1               = from->outer_l5_bytes_0_1;
    to->outer_l5_bytes_0_1_mask          = from->outer_l5_bytes_0_1_mask;
    to->ip_last_protocol                 = from->ip_last_protocol;
    to->ip_last_protocol_mask            = from->ip_last_protocol_mask;
    to->fixed_hve_result_set_1           = from->fixed_hve_result_set_1;
    to->fixed_hve_result_set_1_mask      = from->fixed_hve_result_set_1_mask;
    to->fixed_hve_result_set_5           = from->fixed_hve_result_set_5;
    to->fixed_hve_result_set_5_mask      = from->fixed_hve_result_set_5_mask;
    to->flex_hve_result_set_1            = from->flex_hve_result_set_1;
    to->flex_hve_result_set_1_mask       = from->flex_hve_result_set_1_mask;
    to->tunnel_processing_results_1      = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask = from->tunnel_processing_results_1_mask;
    to->vfp_opaque_ctrl_id               = from->vfp_opaque_ctrl_id;
    to->vfp_opaque_ctrl_id_mask          = from->vfp_opaque_ctrl_id_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   _bcm_compat6518out_switch_pkt_control_action_t
 * Purpose:
 *   Convert the bcm_switch_pkt_control_action_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_switch_pkt_control_action_t(
    bcm_switch_pkt_control_action_t *from,
    bcm_compat6518_switch_pkt_control_action_t *to)
{
    to->discard           = from->discard;
    to->copy_to_cpu       = from->copy_to_cpu;
    to->flood             = from->flood;
    to->bpdu              = from->bpdu;
    to->opaque_ctrl_id    = from->opaque_ctrl_id;
}

/*
 * Function:
 *   _bcm_compat6518out_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_switch_pkt_protocol_match_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_switch_pkt_protocol_match_t(
    bcm_switch_pkt_protocol_match_t *from,
    bcm_compat6518_switch_pkt_protocol_match_t *to)
{
    to->l2_iif_opaque_ctrl_id            = from->l2_iif_opaque_ctrl_id;
    to->l2_iif_opaque_ctrl_id_mask       = from->l2_iif_opaque_ctrl_id_mask;
    to->vfi                              = from->vfi;
    to->vfi_mask                         = from->vfi_mask;
    to->ethertype                        = from->ethertype;
    to->ethertype_mask                   = from->ethertype_mask;
    to->arp_rarp_opcode                  = from->arp_rarp_opcode;
    to->arp_rarp_opcode_mask             = from->arp_rarp_opcode_mask;
    to->icmp_type                        = from->icmp_type;
    to->icmp_type_mask                   = from->icmp_type_mask;
    to->igmp_type                        = from->igmp_type;
    to->igmp_type_mask                   = from->igmp_type_mask;
    to->l4_valid                         = from->l4_valid;
    to->l4_valid_mask                    = from->l4_valid_mask;
    to->l4_src_port                      = from->l4_src_port;
    to->l4_src_port_mask                 = from->l4_src_port_mask;
    to->l4_dst_port                      = from->l4_dst_port;
    to->l4_dst_port_mask                 = from->l4_dst_port_mask;
    to->l5_bytes_0_1                     = from->l5_bytes_0_1;
    to->l5_bytes_0_1_mask                = from->l5_bytes_0_1_mask;
    to->outer_l5_bytes_0_1               = from->outer_l5_bytes_0_1;
    to->outer_l5_bytes_0_1_mask          = from->outer_l5_bytes_0_1_mask;
    to->ip_last_protocol                 = from->ip_last_protocol;
    to->ip_last_protocol_mask            = from->ip_last_protocol_mask;
    to->fixed_hve_result_set_1           = from->fixed_hve_result_set_1;
    to->fixed_hve_result_set_1_mask      = from->fixed_hve_result_set_1_mask;
    to->fixed_hve_result_set_5           = from->fixed_hve_result_set_5;
    to->fixed_hve_result_set_5_mask      = from->fixed_hve_result_set_5_mask;
    to->flex_hve_result_set_1            = from->flex_hve_result_set_1;
    to->flex_hve_result_set_1_mask       = from->flex_hve_result_set_1_mask;
    to->tunnel_processing_results_1      = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask = from->tunnel_processing_results_1_mask;
    to->vfp_opaque_ctrl_id               = from->vfp_opaque_ctrl_id;
    to->vfp_opaque_ctrl_id_mask          = from->vfp_opaque_ctrl_id_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   bcm_compat6518_switch_pkt_protocol_control_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Options flags.
 *   match          - (IN) Protocol match structure.
 *   action         - (IN) Packet control action.
 *   priority       - (IN) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6518_switch_pkt_protocol_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int priority)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;
    bcm_switch_pkt_control_action_t *new_action = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6518in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    if (action != NULL) {
        new_action = (bcm_switch_pkt_control_action_t *)
            sal_alloc(sizeof(bcm_switch_pkt_control_action_t), "New action");
        if (new_action == NULL) {
            sal_free(new_match);
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_switch_pkt_control_action_t(action, new_action);
    } else {
        sal_free(new_match);
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_add(unit, options, new_match,
                                             new_action, priority);

    if (rv >= 0) {
        _bcm_compat6518out_switch_pkt_protocol_match_t(new_match, match);
        _bcm_compat6518out_switch_pkt_control_action_t(new_action, action);
    }

    sal_free(new_match);
    sal_free(new_action);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_switch_pkt_protocol_control_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Protocol match structure.
 *   action         - (OUT) Packet control action.
 *   priority       - (OUT) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6518_switch_pkt_protocol_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int *priority)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;
    bcm_switch_pkt_control_action_t *new_action = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6518in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    if (action != NULL) {
        new_action = (bcm_switch_pkt_control_action_t *)
            sal_alloc(sizeof(bcm_switch_pkt_control_action_t), "New action");
        if (new_action == NULL) {
            sal_free(new_match);
            return BCM_E_MEMORY;
        }
        _bcm_compat6518in_switch_pkt_control_action_t(action, new_action);
    } else {
        sal_free(new_match);
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_get(unit, new_match, new_action, priority);

    if (rv >= 0) {
        _bcm_compat6518out_switch_pkt_protocol_match_t(new_match, match);
        _bcm_compat6518out_switch_pkt_control_action_t(new_action, action);
    }

    sal_free(new_match);
    sal_free(new_action);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_switch_pkt_protocol_control_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Protocol match structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6518_switch_pkt_protocol_match_t *match)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_delete(unit, new_match);

    if (rv >= 0) {
        _bcm_compat6518out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_switch_pkt_integrity_check_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_integrity_check_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Options flags.
 *   match          - (IN) Integrity match structure.
 *   action         - (IN) Packet control action.
 *   priority       - (IN) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_switch_pkt_integrity_check_add(
    int unit,
    uint32 options,
    bcm_switch_pkt_integrity_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int priority)
{
    int rv = 0;
    bcm_switch_pkt_control_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_switch_pkt_control_action_t *)
            sal_alloc(sizeof(bcm_switch_pkt_control_action_t), "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6518in_switch_pkt_control_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }
    rv = bcm_switch_pkt_integrity_check_add(unit, options, match,
                                             new_action, priority);

    if (rv >= 0) {
        _bcm_compat6518out_switch_pkt_control_action_t(new_action, action);
    }

    sal_free(new_action);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_switch_pkt_integrity_check_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_integrity_check_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Integrity match structure.
 *   action         - (OUT) Packet control action.
 *   priority       - (OUT) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_switch_pkt_integrity_check_get(
    int unit,
    bcm_switch_pkt_integrity_match_t *match,
    bcm_compat6518_switch_pkt_control_action_t *action,
    int *priority)
{
    int rv = 0;
    bcm_switch_pkt_control_action_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_switch_pkt_control_action_t *)
            sal_alloc(sizeof(bcm_switch_pkt_control_action_t), "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6518in_switch_pkt_control_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }
    rv = bcm_switch_pkt_integrity_check_get(unit, match, new_action, priority);
    if (rv >= 0) {
        _bcm_compat6518out_switch_pkt_control_action_t(new_action, action);
    }

    sal_free(new_action);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_vlan_action_set_t
 * Purpose:
 *      Convert the _bcm_compat6518in_vlan_action_set_t datatype from <=6.5.18
 *      to SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_vlan_action_set_t(
    bcm_compat6518_vlan_action_set_t *from,
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
    to->flags= from->flags;
    to->outer_tag = from->outer_tag;
    to->inner_tag = from->inner_tag;
    to->forwarding_domain= from->forwarding_domain;
    to->inner_qos_map_id= from->inner_qos_map_id;
    to->outer_qos_map_id= from->outer_qos_map_id;
}

/*
 * Function:
 *      _bcm_compat6518out_vlan_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_action_set_t datatype from 6.5.18+ to
 *      <=SDK 6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_vlan_action_set_t(
    bcm_vlan_action_set_t *from,
    bcm_compat6518_vlan_action_set_t *to)
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
    to->forwarding_domain= from->forwarding_domain;
    to->inner_qos_map_id= from->inner_qos_map_id;
    to->outer_qos_map_id= from->outer_qos_map_id;
}

int
bcm_compat6518_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_get(unit, port, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_set(unit, port, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_get(unit, port, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_set(unit, port, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_add(unit, port, frame, ether, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_get(unit, port, frame, ether, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_add(unit, mac, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6518_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_get(unit, mac, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6518_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_add(unit, port, key_type,
                                       outer_vlan, inner_vlan, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_create(unit, port, key_type,
                                          outer_vlan, inner_vlan, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_get(unit, port, key_type,
                                       outer_vlan, inner_vlan, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6518_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_add(unit, port_class,
                                              outer_vlan, inner_vlan,
                                              new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_get(unit, port_class, outer_vlan,
                                              inner_vlan, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_add(unit, port,
                                                    outer_vlan, inner_vlan,
                                                    new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_get(unit, port, outer_vlan,
                                                    inner_vlan, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6518_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_add(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high,
                                             new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_get(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high,
                                             new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6518_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_get(unit, flags, action_id, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_add(unit, vlan_ip, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6518_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_get(unit, vlan_ip, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6518_vlan_action_set_t *action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_add(unit, options, match_info, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_match_action_get(
    int unit,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6518_vlan_action_set_t *action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_get(unit, match_info, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, action_set);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_vlan_match_info_t *match_info_array,
    bcm_compat6518_vlan_action_set_t *action_set_array,
    int *count)
{
    int rv, i;
    bcm_vlan_action_set_t *new_action_set_array = NULL;

    if (NULL != action_set_array) {
        /* Create from heap to avoid the stack to bloat */
        new_action_set_array = (bcm_vlan_action_set_t *)
            sal_alloc(size * sizeof(bcm_vlan_action_set_t),
                      "New vlan action set array");
        if (NULL == new_action_set_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0 ; i < size; i++) {
            _bcm_compat6518in_vlan_action_set_t(&action_set_array[i],
                                                &new_action_set_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_multi_get(unit, match, size, match_info_array,
                                         new_action_set_array, count);

    if (NULL != new_action_set_array) {
        /* Transform the entry from the new format to old one */
        for (i = 0 ; i < size; i++) {
            _bcm_compat6518out_vlan_action_set_t(&new_action_set_array[i],
                                                 &action_set_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_action_set_array);
    }

    return rv;
}

int
bcm_compat6518_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6518_vlan_action_set_t *vlan_action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != vlan_action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(vlan_action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_add(unit, entry, action, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6518_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6518_vlan_action_set_t *vlan_action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != vlan_action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vlan_action_set_t(vlan_action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_get(unit, entry, action, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vlan_action_set_t(new_action, vlan_action_set);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_fcoe_vsan_action_set_t
 * Purpose:
 *      Convert the _bcm_compat6518in_fcoe_vsan_action_set_t datatype from <=6.5.18
 *      to SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_fcoe_vsan_action_set_t(
    bcm_compat6518_fcoe_vsan_action_set_t *from,
    bcm_fcoe_vsan_action_set_t *to)
{
    bcm_fcoe_vsan_action_set_t_init(to);

    to->flags = from->flags;
    to->new_vsan = from->new_vsan;
    to->new_vsan_pri = from->new_vsan_pri;
    sal_memcpy(&to->vft, &from->vft,
               sizeof(bcm_fcoe_vsan_vft_t));
    to->ingress_if = from->ingress_if;
    to->dt_vsan = from->dt_vsan;
    to->dt_vsan_pri = from->dt_vsan_pri;
    to->it_vsan = from->it_vsan;
    to->it_vsan_pri = from->it_vsan_pri;
    to->ot_vsan = from->ot_vsan;
    to->ot_vsan_pri = from->ot_vsan_pri;
    to->ut_vsan = from->ut_vsan;
    to->ut_vsan_pri = from->ut_vsan_pri;

    to->vlan_action.new_outer_vlan = from->vlan_action.new_outer_vlan;
    to->vlan_action.new_inner_vlan = from->vlan_action.new_inner_vlan;
    to->vlan_action.new_inner_pkt_prio = from->vlan_action.new_inner_pkt_prio;
    to->vlan_action.new_outer_cfi = from->vlan_action.new_outer_cfi;
    to->vlan_action.new_inner_cfi = from->vlan_action.new_inner_cfi;
    to->vlan_action.ingress_if = from->vlan_action.ingress_if;
    to->vlan_action.priority = from->vlan_action.priority;
    to->vlan_action.dt_outer = from->vlan_action.dt_outer;
    to->vlan_action.dt_outer_prio = from->vlan_action.dt_outer_prio;
    to->vlan_action.dt_outer_pkt_prio = from->vlan_action.dt_outer_pkt_prio;
    to->vlan_action.dt_outer_cfi = from->vlan_action.dt_outer_cfi;
    to->vlan_action.dt_inner = from->vlan_action.dt_inner;
    to->vlan_action.dt_inner_prio = from->vlan_action.dt_inner_prio;
    to->vlan_action.dt_inner_pkt_prio = from->vlan_action.dt_inner_pkt_prio;
    to->vlan_action.dt_inner_cfi = from->vlan_action.dt_inner_cfi;
    to->vlan_action.ot_outer = from->vlan_action.ot_outer;
    to->vlan_action.ot_outer_prio = from->vlan_action.ot_outer_prio;
    to->vlan_action.ot_outer_pkt_prio = from->vlan_action.ot_outer_pkt_prio;
    to->vlan_action.ot_outer_cfi = from->vlan_action.ot_outer_cfi;
    to->vlan_action.ot_inner = from->vlan_action.ot_inner;
    to->vlan_action.ot_inner_pkt_prio = from->vlan_action.ot_inner_pkt_prio;
    to->vlan_action.ot_inner_cfi = from->vlan_action.ot_inner_cfi;
    to->vlan_action.it_outer = from->vlan_action.it_outer;
    to->vlan_action.it_outer_pkt_prio = from->vlan_action.it_outer_pkt_prio;
    to->vlan_action.it_outer_cfi = from->vlan_action.it_outer_cfi;
    to->vlan_action.it_inner = from->vlan_action.it_inner;
    to->vlan_action.it_inner_prio = from->vlan_action.it_inner_prio;
    to->vlan_action.it_inner_pkt_prio = from->vlan_action.it_inner_pkt_prio;
    to->vlan_action.it_inner_cfi = from->vlan_action.it_inner_cfi;
    to->vlan_action.ut_outer = from->vlan_action.ut_outer;
    to->vlan_action.ut_outer_pkt_prio = from->vlan_action.ut_outer_pkt_prio;
    to->vlan_action.ut_outer_cfi = from->vlan_action.ut_outer_cfi;
    to->vlan_action.ut_inner = from->vlan_action.ut_inner;
    to->vlan_action.ut_inner_pkt_prio = from->vlan_action.ut_inner_pkt_prio;
    to->vlan_action.ut_inner_cfi = from->vlan_action.ut_inner_cfi;
    to->vlan_action.outer_pcp = from->vlan_action.outer_pcp;
    to->vlan_action.inner_pcp = from->vlan_action.inner_pcp;
    to->vlan_action.policer_id = from->vlan_action.policer_id;
    to->vlan_action.outer_tpid = from->vlan_action.outer_tpid;
    to->vlan_action.inner_tpid = from->vlan_action.inner_tpid;
    to->vlan_action.outer_tpid_action = from->vlan_action.outer_tpid_action;
    to->vlan_action.inner_tpid_action = from->vlan_action.inner_tpid_action;
    to->vlan_action.action_id = from->vlan_action.action_id;
    to->vlan_action.class_id = from->vlan_action.class_id;
    to->vlan_action.taf_gate_primap = from->vlan_action.taf_gate_primap;
    to->vlan_action.flags = from->vlan_action.flags;
    to->vlan_action.outer_tag = from->vlan_action.outer_tag;
    to->vlan_action.inner_tag = from->vlan_action.inner_tag;
    to->vlan_action.forwarding_domain= from->vlan_action.forwarding_domain;
    to->vlan_action.inner_qos_map_id= from->vlan_action.inner_qos_map_id;
    to->vlan_action.outer_qos_map_id= from->vlan_action.outer_qos_map_id;
}

int
bcm_compat6518_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6518_fcoe_vsan_action_set_t *action)
{
    int rv;
    bcm_fcoe_vsan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_fcoe_vsan_action_set_t *)
            sal_alloc(sizeof(bcm_fcoe_vsan_action_set_t), "New fcoe vsan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_fcoe_vsan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_fcoe_vsan_translate_action_add(unit, key, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_port_match_info_t
 * Purpose:
 *      Convert the bcm_compat6518_port_match_info_t datatype from <=6.5.18
 *      to SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_port_match_info_t(
    bcm_compat6518_port_match_info_t *from,
    bcm_port_match_info_t *to)
{
    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(to->match_tunnel_srcmac, from->match_tunnel_srcmac,
               sizeof(bcm_mac_t));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;

    to->action->new_outer_vlan = from->action->new_outer_vlan;
    to->action->new_inner_vlan = from->action->new_inner_vlan;
    to->action->new_inner_pkt_prio = from->action->new_inner_pkt_prio;
    to->action->new_outer_cfi = from->action->new_outer_cfi;
    to->action->new_inner_cfi = from->action->new_inner_cfi;
    to->action->ingress_if = from->action->ingress_if;
    to->action->priority = from->action->priority;
    to->action->dt_outer = from->action->dt_outer;
    to->action->dt_outer_prio = from->action->dt_outer_prio;
    to->action->dt_outer_pkt_prio = from->action->dt_outer_pkt_prio;
    to->action->dt_outer_cfi = from->action->dt_outer_cfi;
    to->action->dt_inner = from->action->dt_inner;
    to->action->dt_inner_prio = from->action->dt_inner_prio;
    to->action->dt_inner_pkt_prio = from->action->dt_inner_pkt_prio;
    to->action->dt_inner_cfi = from->action->dt_inner_cfi;
    to->action->ot_outer = from->action->ot_outer;
    to->action->ot_outer_prio = from->action->ot_outer_prio;
    to->action->ot_outer_pkt_prio = from->action->ot_outer_pkt_prio;
    to->action->ot_outer_cfi = from->action->ot_outer_cfi;
    to->action->ot_inner = from->action->ot_inner;
    to->action->ot_inner_pkt_prio = from->action->ot_inner_pkt_prio;
    to->action->ot_inner_cfi = from->action->ot_inner_cfi;
    to->action->it_outer = from->action->it_outer;
    to->action->it_outer_pkt_prio = from->action->it_outer_pkt_prio;
    to->action->it_outer_cfi = from->action->it_outer_cfi;
    to->action->it_inner = from->action->it_inner;
    to->action->it_inner_prio = from->action->it_inner_prio;
    to->action->it_inner_pkt_prio = from->action->it_inner_pkt_prio;
    to->action->it_inner_cfi = from->action->it_inner_cfi;
    to->action->ut_outer = from->action->ut_outer;
    to->action->ut_outer_pkt_prio = from->action->ut_outer_pkt_prio;
    to->action->ut_outer_cfi = from->action->ut_outer_cfi;
    to->action->ut_inner = from->action->ut_inner;
    to->action->ut_inner_pkt_prio = from->action->ut_inner_pkt_prio;
    to->action->ut_inner_cfi = from->action->ut_inner_cfi;
    to->action->outer_pcp = from->action->outer_pcp;
    to->action->inner_pcp = from->action->inner_pcp;
    to->action->policer_id = from->action->policer_id;
    to->action->outer_tpid = from->action->outer_tpid;
    to->action->inner_tpid = from->action->inner_tpid;
    to->action->outer_tpid_action = from->action->outer_tpid_action;
    to->action->inner_tpid_action = from->action->inner_tpid_action;
    to->action->action_id = from->action->action_id;
    to->action->class_id = from->action->class_id;
    to->action->taf_gate_primap = from->action->taf_gate_primap;
    to->action->flags = from->action->flags;
    to->action->outer_tag = from->action->outer_tag;
    to->action->inner_tag = from->action->inner_tag;
    to->action->forwarding_domain= from->action->forwarding_domain;
    to->action->inner_qos_map_id= from->action->inner_qos_map_id;
    to->action->outer_qos_map_id= from->action->outer_qos_map_id;

    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    to->port_class = from->port_class;
}

/*
 * Function:
 *      _bcm_compat6518out_port_match_info_t
 * Purpose:
 *      Convert the bcm_port_match_info_t datatype from 6.5.18+ to
 *      <=SDK 6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_port_match_info_t(
    bcm_port_match_info_t *from,
    bcm_compat6518_port_match_info_t *to)
{
    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(to->match_tunnel_srcmac, from->match_tunnel_srcmac,
               sizeof(bcm_mac_t));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;

    to->action->new_outer_vlan = from->action->new_outer_vlan;
    to->action->new_inner_vlan = from->action->new_inner_vlan;
    to->action->new_inner_pkt_prio = from->action->new_inner_pkt_prio;
    to->action->new_outer_cfi = from->action->new_outer_cfi;
    to->action->new_inner_cfi = from->action->new_inner_cfi;
    to->action->ingress_if = from->action->ingress_if;
    to->action->priority = from->action->priority;
    to->action->dt_outer = from->action->dt_outer;
    to->action->dt_outer_prio = from->action->dt_outer_prio;
    to->action->dt_outer_pkt_prio = from->action->dt_outer_pkt_prio;
    to->action->dt_outer_cfi = from->action->dt_outer_cfi;
    to->action->dt_inner = from->action->dt_inner;
    to->action->dt_inner_prio = from->action->dt_inner_prio;
    to->action->dt_inner_pkt_prio = from->action->dt_inner_pkt_prio;
    to->action->dt_inner_cfi = from->action->dt_inner_cfi;
    to->action->ot_outer = from->action->ot_outer;
    to->action->ot_outer_prio = from->action->ot_outer_prio;
    to->action->ot_outer_pkt_prio = from->action->ot_outer_pkt_prio;
    to->action->ot_outer_cfi = from->action->ot_outer_cfi;
    to->action->ot_inner = from->action->ot_inner;
    to->action->ot_inner_pkt_prio = from->action->ot_inner_pkt_prio;
    to->action->ot_inner_cfi = from->action->ot_inner_cfi;
    to->action->it_outer = from->action->it_outer;
    to->action->it_outer_pkt_prio = from->action->it_outer_pkt_prio;
    to->action->it_outer_cfi = from->action->it_outer_cfi;
    to->action->it_inner = from->action->it_inner;
    to->action->it_inner_prio = from->action->it_inner_prio;
    to->action->it_inner_pkt_prio = from->action->it_inner_pkt_prio;
    to->action->it_inner_cfi = from->action->it_inner_cfi;
    to->action->ut_outer = from->action->ut_outer;
    to->action->ut_outer_pkt_prio = from->action->ut_outer_pkt_prio;
    to->action->ut_outer_cfi = from->action->ut_outer_cfi;
    to->action->ut_inner = from->action->ut_inner;
    to->action->ut_inner_pkt_prio = from->action->ut_inner_pkt_prio;
    to->action->ut_inner_cfi = from->action->ut_inner_cfi;
    to->action->outer_pcp = from->action->outer_pcp;
    to->action->inner_pcp = from->action->inner_pcp;
    to->action->policer_id = from->action->policer_id;
    to->action->outer_tpid = from->action->outer_tpid;
    to->action->inner_tpid = from->action->inner_tpid;
    to->action->outer_tpid_action = from->action->outer_tpid_action;
    to->action->inner_tpid_action = from->action->inner_tpid_action;
    to->action->action_id = from->action->action_id;
    to->action->class_id = from->action->class_id;
    to->action->taf_gate_primap = from->action->taf_gate_primap;
    to->action->flags = from->action->flags;
    to->action->outer_tag = from->action->outer_tag;
    to->action->inner_tag = from->action->inner_tag;
    to->action->forwarding_domain= from->action->forwarding_domain;
    to->action->inner_qos_map_id= from->action->inner_qos_map_id;
    to->action->outer_qos_map_id= from->action->outer_qos_map_id;

    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    to->port_class = from->port_class;
 }

int
bcm_compat6518_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_match_info_t *match)
{
    int rv;
    bcm_port_match_info_t *new_match = NULL;
    bcm_vlan_action_set_t action;

    if (NULL != match) {
        /* Create from heap to avoid the stack to bloat */
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (NULL == new_match) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_match);
        bcm_vlan_action_set_t_init(&action);
        new_match->action = &action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_match_info_t(match, new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_add(unit, port, new_match);

    if (NULL != new_match) {
        /* Deallocate memory*/
        sal_free(new_match);
    }

    return rv;
}

int
bcm_compat6518_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_match_info_t *match)
{
    int rv;
    bcm_port_match_info_t *new_match = NULL;
    bcm_vlan_action_set_t action;

    if (NULL != match) {
        /* Create from heap to avoid the stack to bloat */
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (NULL == new_match) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_match);
        bcm_vlan_action_set_t_init(&action);
        new_match->action = &action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_match_info_t(match, new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_delete(unit, port, new_match);

    if (NULL != new_match) {
        /* Deallocate memory*/
        sal_free(new_match);
    }

    return rv;
}

int
bcm_compat6518_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_match_info_t *old_match,
    bcm_compat6518_port_match_info_t *new_match)
{
    int rv;
    bcm_port_match_info_t *new_old_match = NULL;
    bcm_port_match_info_t *new_new_match = NULL;
    bcm_vlan_action_set_t old_action;
    bcm_vlan_action_set_t new_action;

    if (NULL != old_match) {
        /* Create from heap to avoid the stack to bloat */
        new_old_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New old_match info");
        if (NULL == new_old_match) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_old_match);
        bcm_vlan_action_set_t_init(&old_action);
        new_old_match->action = &old_action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_match_info_t(old_match, new_old_match);
    }

    if (NULL != new_match) {
        /* Create from heap to avoid the stack to bloat */
        new_new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New new_match info");
        if (NULL == new_new_match) {
            sal_free(new_old_match);
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_new_match);
        bcm_vlan_action_set_t_init(&new_action);
        new_new_match->action = &new_action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_match_info_t(new_match, new_new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_replace(unit, port, new_old_match, new_new_match);

    if (NULL != new_old_match) {
        /* Deallocate memory*/
        sal_free(new_old_match);
    }

    if (NULL != new_new_match) {
        /* Deallocate memory*/
        sal_free(new_new_match);
    }

    return rv;
}

int
bcm_compat6518_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6518_port_match_info_t *match_array,
    int *count)
{
    int rv, i;
    bcm_port_match_info_t *new_match_array = NULL;
    bcm_vlan_action_set_t action[size];

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t) * size,
            "New port match info");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < size; i++) {
            bcm_port_match_info_t_init(&new_match_array[i]);
            bcm_vlan_action_set_t_init(&action[i]);
            new_match_array[i].action = &action[i];
            _bcm_compat6518in_port_match_info_t(&match_array[i],
                                                &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_multi_get(unit, port, size,
                                  new_match_array, count);

    if (NULL != new_match_array) {

        /* Transform the entry from the new format to old one */
        for (i = 0; i < size; i++) {
            _bcm_compat6518out_port_match_info_t(&new_match_array[i],
                                                 &match_array[i]);
        }

        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

int
bcm_compat6518_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6518_port_match_info_t *match_array)
{
    int rv, i;
    bcm_port_match_info_t *new_match_array = NULL;
    bcm_vlan_action_set_t action[size];

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t) * size,
            "New port match info");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < size; i++) {
            bcm_port_match_info_t_init(&new_match_array[i]);
            bcm_vlan_action_set_t_init(&action[i]);
            new_match_array[i].action = &action[i];
            _bcm_compat6518in_port_match_info_t(&match_array[i],
                                                &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_set(unit, port, size, new_match_array);

    if (NULL != new_match_array) {
        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_oam_action_key_t
 * Purpose:
 *      Convert the bcm_oam_action_key_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_oam_action_key_t(
    bcm_compat6518_oam_action_key_t *from,
    bcm_oam_action_key_t *to)
{
    bcm_oam_action_key_t_init(to);
    to->flags = from->flags;
    to->opcode = from->opcode;
    to->dest_mac_type = from->dest_mac_type;
    to->endpoint_type = from->endpoint_type;
    to->inject = from->inject;

}

/*
 * Function:
 *      bcm_compat6518_oam_profile_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_profile_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) 
 *      profile_id - (IN) Oam action profile id
 *      oam_action_key - (IN) Oam action key.
 *      oam_action_result - (OUT) Return oam action result.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_oam_profile_action_get(
    int unit,
    uint32 flags,
    bcm_oam_profile_t profile_id,
    bcm_compat6518_oam_action_key_t *oam_action_key,
    bcm_oam_action_result_t *oam_action_result)
{
    int rv = BCM_E_NONE;
    bcm_oam_action_key_t *new_oam_action_key = NULL;

    if (oam_action_key != NULL) {
    	new_oam_action_key = (bcm_oam_action_key_t *)
                     sal_alloc(sizeof(bcm_oam_action_key_t),
                     "New oam_action_key");
        if (new_oam_action_key == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_oam_action_key_t(oam_action_key, new_oam_action_key);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_profile_action_get(unit, flags, profile_id, new_oam_action_key, oam_action_result);

    /* Deallocate memory*/
    sal_free(new_oam_action_key);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_oam_profile_action_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_profile_action_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) 
 *      profile_id - (IN) Oam action profile id
 *      oam_action_key - (IN) Oam action key.
 *      oam_action_result - (IN) Return oam action result.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_oam_profile_action_set(
    int unit,
    uint32 flags,
    bcm_oam_profile_t profile_id,
    bcm_compat6518_oam_action_key_t *oam_action_key,
    bcm_oam_action_result_t *oam_action_result)
{
    int rv = BCM_E_NONE;
    bcm_oam_action_key_t *new_oam_action_key = NULL;

    if (oam_action_key != NULL) {
    	new_oam_action_key = (bcm_oam_action_key_t *)
                     sal_alloc(sizeof(bcm_oam_action_key_t),
                     "New oam_action_key");
        if (new_oam_action_key == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_oam_action_key_t(oam_action_key, new_oam_action_key);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_profile_action_set(unit, flags, profile_id, new_oam_action_key, oam_action_result);

    /* Deallocate memory*/
    sal_free(new_oam_action_key);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518out_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_port_config_t(
    bcm_port_config_t *from,
    bcm_compat6518_port_config_t *to)
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
}

/*
 * Function:
 *      bcm_compat6518_port_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (OUT) Pointer to port configuration structure populated on successful return.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_port_config_get(
    int unit,
    bcm_compat6518_port_config_t *config)
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
    _bcm_compat6518out_port_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_sat_header_user_define_offsets_t
 * Purpose:
 *      Convert the bcm_sat_header_user_define_offsets_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_sat_header_user_define_offsets_t(
    bcm_compat6518_sat_header_user_define_offsets_t *from,
    bcm_sat_header_user_define_offsets_t *to)
{
    bcm_sat_header_user_define_offsets_t_init(to);
    to->payload_offset = from->payload_offset;
    to->timestamp_offset = from->timestamp_offset;
    to->seq_number_offset = from->seq_number_offset;
    to->crc_byte_offset = from->crc_byte_offset;
}

/*
 * Function:
 *      _bcm_compat6518in_sat_ctf_packet_info_t
 * Purpose:
 *      Convert the bcm_sat_ctf_packet_info_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_sat_ctf_packet_info_t(
    bcm_compat6518_sat_ctf_packet_info_t *from,
    bcm_sat_ctf_packet_info_t *to)
{
    bcm_sat_ctf_packet_info_t_init(to);
    to->sat_header_type = from->sat_header_type;
    sal_memcpy(&to->payload, &from->payload, sizeof(bcm_sat_payload_t));
    to->flags = from->flags;
    _bcm_compat6518in_sat_header_user_define_offsets_t(&from->offsets, &to->offsets);
}

/*
 * Function:
 *      bcm_compat6518_sat_ctf_packet_config
 * Purpose:
 *      Compatibility function for RPC call to bcm_sat_ctf_packet_config.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ctf_id - (IN) CTF ID
 *      packet_info - (IN) Pointer to SAT CTF Packet config structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_sat_ctf_packet_config(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_compat6518_sat_ctf_packet_info_t *packet_info)
{
    int rv = BCM_E_NONE;
    bcm_sat_ctf_packet_info_t *new_packet_info = NULL;

    if (packet_info != NULL) {
        new_packet_info = (bcm_sat_ctf_packet_info_t *)
                     sal_alloc(sizeof(bcm_sat_ctf_packet_info_t),
                     "New packet_info");
        if (new_packet_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_sat_ctf_packet_info_t(packet_info, new_packet_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_sat_ctf_packet_config(unit, ctf_id, new_packet_info);


    /* Deallocate memory*/
    sal_free(new_packet_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_info_t
 * Purpose:
 *      Convert the bcm_info datatype from <=6.5.18+ to
 *      6.5.18+
 * Parameters:
 *      from       - (IN) The SDK <=6.5.18 version of the datatype
 *      to          - (OUT) The 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_info_t(
    bcm_compat6518_info_t *from,
    bcm_info_t *to)
{
    sal_memset(to, 0, sizeof(bcm_info_t));
    to->vendor = from->vendor;
    to->device = from->device;
    to->revision = from->revision;
    to->capability = from->capability;
    to->num_pipes = from->num_pipes;
    to->num_pp_pipes = from->num_pp_pipes;
}

/*
 * Function:
 *      _bcm_compat6518out_info_t
 * Purpose:
 *      Convert the bcm_info datatype from 6.5.18+ to
 *      <=6.5.18
 * Parameters:
 *      from       - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_info_t(
    bcm_info_t *from,
    bcm_compat6518_info_t *to)
{
    sal_memset(to, 0, sizeof(bcm_compat6518_info_t));
    to->vendor = from->vendor;
    to->device = from->device;
    to->revision = from->revision;
    to->capability = from->capability;
    to->num_pipes = from->num_pipes;
    to->num_pp_pipes = from->num_pp_pipes;
}

/*
 * Function: bcm_compat6518_info_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_info_get.
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      info    - (OUT) Get information about a BCM unit.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6518_info_get(
    int unit,
    bcm_compat6518_info_t *info)
{
    int rv = BCM_E_NONE;
    bcm_info_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_info_t *)
            sal_alloc(sizeof(bcm_info_t), "New bcm info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    if (NULL != new_info) {
        /* Call the BCM API with new format */
        rv = bcm_info_get(unit, new_info);
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_info_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_collector_info_t
 * Purpose:
 *   Convert the bcm_collector_info_t datatype from <=6.5.18 to
 *   SDK 6.5.18+
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_collector_info_t(
        bcm_compat6518_collector_info_t *from,
        bcm_collector_info_t *to)
{
    bcm_collector_info_t_init(to);
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_compat6518_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
    sal_memcpy(&to->src_ts, &from->src_ts, sizeof(bcm_collector_timestamp_source_t));
    to->max_records_reserve = 16;
}

/*
 * Function:
 *   _bcm_compat6518out_collector_info_t
 * Purpose:
 *   Convert the bcm_collector_info_t datatype from 6.5.18+ to
 *   <=6.5.18
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype
 *   to       - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_collector_info_t(
        bcm_collector_info_t *from,
        bcm_compat6518_collector_info_t *to)
{
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_compat6518_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
    sal_memcpy(&to->src_ts, &from->src_ts, sizeof(bcm_collector_timestamp_source_t));
}

/*
 * Function:
 *   bcm_compat6518_collector_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_collector_create
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Collector create options
 *   id             - (INOUT) Collector Id.
 *   collector_info - (IN) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6518_collector_create(
        int unit,
        uint32 options,
        bcm_collector_t *collector_id,
        bcm_compat6518_collector_info_t *collector_info)
{
    int rv = 0;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t*)
            sal_alloc(sizeof(bcm_collector_info_t), "New collector info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_collector_create(unit, options, collector_id, new_collector_info);

    if (rv >= 0) {
        _bcm_compat6518out_collector_info_t(new_collector_info, collector_info);
    }

    sal_free(new_collector_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_collector_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_collector_get
 * Parameters:
 *   unit    - (IN) BCM device number.
 *   id      - (IN) Collector Id.
 *   info    - (OUT) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6518_collector_get(
        int unit,
        bcm_collector_t collector_id,
        bcm_compat6518_collector_info_t *collector_info)
{
    int rv = 0;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t*)
            sal_alloc(sizeof(bcm_collector_info_t), "New collector info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_collector_get(unit, collector_id, new_collector_info);

    if (rv >= 0) {
        _bcm_compat6518out_collector_info_t(new_collector_info, collector_info);
    }

    sal_free(new_collector_info);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_ifa_stat_info_t
 * Purpose:
 *   Convert the bcm_ifa_stat_info_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_ifa_stat_info_t(bcm_compat6518_ifa_stat_info_t *from,
                                  bcm_ifa_stat_info_t *to)
{
    sal_memset(to, 0, sizeof(*to));

    to->rx_pkt_cnt = from->rx_pkt_cnt;
    to->tx_pkt_cnt = from->tx_pkt_cnt;
    to->ifa_no_config_drop = from->ifa_no_config_drop;
    to->ifa_collector_not_present_drop = from->ifa_collector_not_present_drop;
    to->ifa_hop_cnt_invalid_drop = from->ifa_hop_cnt_invalid_drop;
    to->ifa_int_hdr_len_invalid_drop = from->ifa_int_hdr_len_invalid_drop;
    to->ifa_pkt_size_invalid_drop = from->ifa_pkt_size_invalid_drop;
    to->rx_pkt_length_exceed_max_drop_count = from->rx_pkt_length_exceed_max_drop_count;
    to->rx_pkt_parse_error_drop_count = from->rx_pkt_parse_error_drop_count;
    to->rx_pkt_unknown_namespace_drop_count = from->rx_pkt_unknown_namespace_drop_count;
    to->rx_pkt_excess_rate_drop_count = from->rx_pkt_excess_rate_drop_count;
    to->tx_record_count = from->tx_record_count;
    to->tx_pkt_failure_count = from->tx_pkt_failure_count;
}

/*
 * Function:
 *   _bcm_compat6518out_ifa_stat_info_t
 * Purpose:
 *   Convert the bcm_ifa_stat_info_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_ifa_stat_info_t(bcm_ifa_stat_info_t *from,
                                   bcm_compat6518_ifa_stat_info_t *to)
{
    to->rx_pkt_cnt = from->rx_pkt_cnt;
    to->tx_pkt_cnt = from->tx_pkt_cnt;
    to->ifa_no_config_drop = from->ifa_no_config_drop;
    to->ifa_collector_not_present_drop = from->ifa_collector_not_present_drop;
    to->ifa_hop_cnt_invalid_drop = from->ifa_hop_cnt_invalid_drop;
    to->ifa_int_hdr_len_invalid_drop = from->ifa_int_hdr_len_invalid_drop;
    to->ifa_pkt_size_invalid_drop = from->ifa_pkt_size_invalid_drop;
    to->rx_pkt_length_exceed_max_drop_count = from->rx_pkt_length_exceed_max_drop_count;
    to->rx_pkt_parse_error_drop_count = from->rx_pkt_parse_error_drop_count;
    to->rx_pkt_unknown_namespace_drop_count = from->rx_pkt_unknown_namespace_drop_count;
    to->rx_pkt_excess_rate_drop_count = from->rx_pkt_excess_rate_drop_count;
    to->tx_record_count = from->tx_record_count;
    to->tx_pkt_failure_count = from->tx_pkt_failure_count;
}

/*
 * Function:
 *   bcm_compat6518_ifa_stat_info_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_ifa_stat_info_set.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   stat_data      - (IN) In-band flow analyzer - IFA statistics information
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_ifa_stat_info_set(int unit,
                                 bcm_compat6518_ifa_stat_info_t *stat_data)
{
    int rv = 0;
    bcm_ifa_stat_info_t *new_stat_data = NULL;

    if (stat_data != NULL) {
        new_stat_data = (bcm_ifa_stat_info_t *)
            sal_alloc(sizeof(bcm_ifa_stat_info_t), "New stat data");
        if (new_stat_data == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    _bcm_compat6518in_ifa_stat_info_t(stat_data, new_stat_data);
    rv = bcm_ifa_stat_info_set(unit, new_stat_data);

    if (rv >= 0) {
        _bcm_compat6518out_ifa_stat_info_t(new_stat_data, stat_data);
    }

    sal_free(new_stat_data);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_ifa_stat_info_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_ifa_stat_info_get.
 * Parameters:
 *   unit           - (IN)  BCM device number.
 *   stat_data      - (OUT) In-band flow analyzer - IFA statistics information
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_ifa_stat_info_get(int unit,
                                 bcm_compat6518_ifa_stat_info_t *stat_data)
{
    int rv = 0;
    bcm_ifa_stat_info_t *new_stat_data = NULL;

    if (stat_data != NULL) {
        new_stat_data = (bcm_ifa_stat_info_t *)
            sal_alloc(sizeof(bcm_ifa_stat_info_t), "New stat data");
        if (new_stat_data == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_ifa_stat_info_get(unit, new_stat_data);

    if (rv >= 0) {
        _bcm_compat6518out_ifa_stat_info_t(new_stat_data, stat_data);
    }

    sal_free(new_stat_data);
    return rv;
}
/*
 * Function:
 *      _bcm_compat6518in_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_vlan_control_vlan_t(
    bcm_compat6518_vlan_control_vlan_t *from,
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
    _bcm_compat6518in_vlan_action_set_t(&from->egress_action, &to->egress_action);
    to->l2_view = from->l2_view;
    to->egress_class_id = from->egress_class_id;
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6518out_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_vlan_control_vlan_t(
    bcm_vlan_control_vlan_t *from,
    bcm_compat6518_vlan_control_vlan_t *to)
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
    _bcm_compat6518out_vlan_action_set_t(&from->egress_action, &to->egress_action);
    to->l2_view = from->l2_view;
    to->egress_class_id = from->egress_class_id;
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
}

/*
 * Function:
 *      bcm_compat6518_vlan_control_vlan_selective_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      valid_fields - (IN) Valid fields for VLAN control structure
 *      control - (OUT) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6518_vlan_control_vlan_t *control)
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
    _bcm_compat6518out_vlan_control_vlan_t(new_control, control);

    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_vlan_control_vlan_selective_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      valid_fields - (IN) Valid fields for VLAN control structure
 *      control - (IN) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6518_vlan_control_vlan_t *control)
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
        _bcm_compat6518in_vlan_control_vlan_t(control, new_control);
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
 *      bcm_compat6518_vlan_control_vlan_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      control - (OUT) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6518_vlan_control_vlan_t *control)
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
    _bcm_compat6518out_vlan_control_vlan_t(new_control, control);

    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_vlan_control_vlan_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      control - (IN) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    /* coverity[pass_by_value: Intentional]  */
    bcm_compat6518_vlan_control_vlan_t control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t new_control;

    /* Transform the entry from the old format to new one */
    _bcm_compat6518in_vlan_control_vlan_t(&control, &new_control);
    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_set(unit, vlan, new_control);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_port_resource_t
 * Purpose:
 *   Convert the bcm_port_resource_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to           - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_port_resource_t(
    bcm_compat6518_port_resource_t *from,
    bcm_port_resource_t *to)
{
    bcm_port_resource_t_init(to);
    to->flags = from->flags;
    to->port = from->port;
    to->physical_port = from->physical_port;
    to->speed = from->speed;
    to->rx_speed = from->rx_speed;
    to->lanes = from->lanes;
    to->encap = from->encap;
    to->fec_type = from->fec_type;
    to->phy_lane_config = from->phy_lane_config;
    to->link_training = from->link_training;
    to->roe_compression = from->roe_compression;
    to->num_subports = from->num_subports;
    to->line_card = from->line_card;
}

/*
 * Function:
 *      _bcm_compat6518out_port_resource_t
 * Purpose:
 *      Convert the bcm_port_resource_t datatype from 6.5.18+ to
 *      <=6.5.18
 * Parameters:
 *      from       - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_port_resource_t(
    bcm_port_resource_t *from,
    bcm_compat6518_port_resource_t *to)
{
    to->flags = from->flags;
    to->port = from->port;
    to->physical_port = from->physical_port;
    to->speed = from->speed;
    to->rx_speed = from->rx_speed;
    to->lanes = from->lanes;
    to->encap = from->encap;
    to->fec_type = from->fec_type;
    to->phy_lane_config = from->phy_lane_config;
    to->link_training = from->link_training;
    to->roe_compression = from->roe_compression;
    to->num_subports = from->num_subports;
    to->line_card = from->line_card;
}

/*
 * Perform a FlexPort operation changing the port resources for a given
 * logical port.
 */
int
bcm_compat6518_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_set(unit, port, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/* Get the port resource configuration. */
int
bcm_compat6518_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_get(unit, port, new_resource);

    if (new_resource != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_port_resource_t(new_resource, resource);

        /* Deallocate memory*/
        sal_free(new_resource);
    }

    return rv;
}

/*
 * Get suggested default resource values prior to calling
 * bcm_port_resource_set.
 */
int
bcm_compat6518_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_compat6518_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_default_get(unit, port, flags, new_resource);

    if (new_resource != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_port_resource_t(new_resource, resource);

        /* Deallocate memory*/
        sal_free(new_resource);
    }

    return rv;
}

/*
 * Performs a FlexPort operation changing the port resources for a set of
 * ports.
 */
int
bcm_compat6518_port_resource_multi_set(
    int unit,
    int nport,
    bcm_compat6518_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;
    int i = 0;

    if (resource != NULL && nport > 0) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(nport * sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < nport; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6518in_port_resource_t(&resource[i], &new_resource[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_multi_set(unit, nport, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/*
 * Validate a port's speed configuration on the BCM56980 by fetching the
 * affected ports.
 */
int
bcm_compat6518_port_resource_speed_config_validate(
    int unit,
    bcm_compat6518_port_resource_t *resource,
    bcm_pbmp_t *pbmp)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_config_validate(unit, new_resource, pbmp);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/* Get/Modify the speed for a given port. */
int
bcm_compat6518_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_get(unit, port, new_resource);

    if (new_resource != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_port_resource_t(new_resource, resource);

        /* Deallocate memory*/
        sal_free(new_resource);
    }

    return rv;
}

/* Get/Modify the speed for a given port. */
int
bcm_compat6518_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6518_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_set(unit, port, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/* Modify the Port Speeds for a set of ports in a single Port Macro. */
int
bcm_compat6518_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_compat6518_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;
    int i = 0;

    if (resource != NULL && nport > 0) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(nport * sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < nport; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6518in_port_resource_t(&resource[i], &new_resource[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_multi_set(unit, nport, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}





/*
 * Function:
 *   _bcm_compat6518in_field_hint_t
 * Purpose:
 *   Convert the bcm_field_hint_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_field_hint_t(
    bcm_compat6518_field_hint_t *from,
    bcm_field_hint_t *to)
{
    bcm_field_hint_t_init(to);
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
}

/*
 * Function:
 *   _bcm_compat6518out_field_hint_t
 * Purpose:
 *   Convert the bcm_field_hint_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_field_hint_t(
    bcm_field_hint_t *from,
    bcm_compat6518_field_hint_t *to)
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
}

/*
 * Function:
 *   bcm_compat6518_field_hints_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_hints_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   hint_id      - (IN) Hint Id.
 *   hint         - (IN) Hint.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_field_hints_add(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6518_field_hint_t *hint)
{
    int rv = 0;
    bcm_field_hint_t *new_config = NULL;

    if (hint != NULL) {
        new_config = (bcm_field_hint_t*)
            sal_alloc(sizeof(bcm_field_hint_t), "New Hint config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_field_hint_t(hint, new_config);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_field_hints_add(unit, hint_id, new_config);

    if (new_config != NULL) {
        _bcm_compat6518out_field_hint_t(new_config, hint);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6518_field_hints_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_hints_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   hint_id      - (IN) Hint Id.
 *   hint         - (IN) Hint.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_field_hints_delete(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6518_field_hint_t *hint)
{
    int rv = 0;
    bcm_field_hint_t *new_config = NULL;

    if (hint != NULL) {
        new_config = (bcm_field_hint_t*)
            sal_alloc(sizeof(bcm_field_hint_t), "New Hint config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_field_hint_t(hint, new_config);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_field_hints_delete(unit, hint_id, new_config);

    if (new_config != NULL) {
        _bcm_compat6518out_field_hint_t(new_config, hint);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6518_field_hints_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_hints_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   hint_id      - (IN) Hint Id.
 *   hint         - (IN) Hint.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_field_hints_get(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6518_field_hint_t *hint)
{
    int rv;
    bcm_field_hint_t *new_config = NULL;

    if (hint != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_config = sal_alloc(sizeof(bcm_field_hint_t),
                               "New hint cfg");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_field_hint_t(hint, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_hints_get(unit, hint_id, new_config);

    if (new_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_hint_t(new_config, hint);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_field_entry_config_t
 * Purpose:
 *   Convert the bcm_field_entry_config_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_field_entry_config_t(
    bcm_compat6518_field_entry_config_t *from,
    bcm_field_entry_config_t *to)
{
    int idx = 0;
    bcm_field_entry_config_t_init(to);
    BCM_FIELD_ASET_INIT(to->aset);
    for (idx = 0; idx < bcmFieldActionCount; idx++) {
        if (BCM_FIELD_ASET_TEST(from->aset, idx)) {
            BCM_FIELD_ASET_ADD(to->aset, idx);
        }
    }
    to->flags = from->flags;
    to->entry_id = from->entry_id;
    to->group_id = from->group_id;
    to->priority = from->priority;
}

/*
 * Function:
 *   _bcm_compat6518out_field_entry_config_t
 * Purpose:
 *   Convert the bcm_field_entry_config_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_field_entry_config_t(
    bcm_field_entry_config_t *from,
    bcm_compat6518_field_entry_config_t *to)
{
    int idx = 0;
    BCM_FIELD_ASET_INIT(to->aset);
    for (idx = 0; idx < bcmFieldActionCount; idx++) {
        if (BCM_FIELD_ASET_TEST(from->aset, idx)) {
            BCM_FIELD_ASET_ADD(to->aset, idx);
        }
    }

    to->flags = from->flags;
    to->entry_id = from->entry_id;
    to->group_id = from->group_id;
    to->priority = from->priority;
}

/*
 * Function:
 *   bcm_compat6518_field_entry_config_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_entry_config_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   entry_config   - (INOUT) Entry Config.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_field_entry_config_create(
    int unit,
    bcm_compat6518_field_entry_config_t *entry_config)
{
    int rv;
    bcm_field_entry_config_t *new_config = NULL;

    if (entry_config != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_config = sal_alloc(sizeof(bcm_field_entry_config_t),
                               "New entry cfg");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_field_entry_config_t(entry_config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_entry_config_create(unit, new_config);

    if (new_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_entry_config_t(new_config, entry_config);
        sal_free(new_config);
    }

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *   _bcm_compat6518in_l3_egress_t
 * Purpose:
 *   Convert the bcm_compat6518_l3_egress_t datatype from
 *   <=6.5.18 to SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_l3_egress_t(
    bcm_compat6518_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    bcm_l3_egress_t_init(to);
    to->flags                       = from->flags;
    to->flags2                      = from->flags2;
    to->intf                        = from->intf;
    to->vlan                        = from->vlan;
    to->module                      = from->module;
    to->port                        = from->port;
    to->trunk                       = from->trunk;
    to->mpls_flags                  = from->mpls_flags;
    to->mpls_label                  = from->mpls_label;
    to->mpls_action                 = from->mpls_action;
    to->mpls_qos_map_id             = from->mpls_qos_map_id;
    to->mpls_ttl                    = from->mpls_ttl;
    to->mpls_pkt_pri                = from->mpls_pkt_pri;
    to->mpls_pkt_cfi                = from->mpls_pkt_cfi;
    to->mpls_exp                    = from->mpls_exp;
    to->qos_map_id                  = from->qos_map_id;
    to->encap_id                    = from->encap_id;
    to->failover_id                 = from->failover_id;
    to->failover_if_id              = from->failover_if_id;
    to->failover_mc_group           = from->failover_mc_group;
    to->dynamic_scaling_factor      = from->dynamic_scaling_factor;
    to->dynamic_load_weight         = from->dynamic_load_weight;
    to->dynamic_queue_size_weight   = from->dynamic_queue_size_weight;
    to->intf_class                  = from->intf_class;
    to->multicast_flags             = from->multicast_flags;
    to->oam_global_context_id       = from->oam_global_context_id;
    to->vntag                       = from->vntag;
    to->vntag_action                = from->vntag_action;
    to->etag                        = from->etag;
    to->etag_action                 = from->etag_action;
    to->flow_handle                 = from->flow_handle;
    to->flow_option_handle          = from->flow_option_handle;
    to->flow_label_option_handle    = from->flow_label_option_handle;
    to->num_of_fields               = from->num_of_fields;
    to->counting_profile            = from->counting_profile;
    to->mpls_ecn_map_id             = from->mpls_ecn_map_id;
    to->urpf_mode                   = from->urpf_mode;
    to->mc_group                    = from->mc_group;
    to->hierarchical_gport          = from->hierarchical_gport;
    to->stat_id                     = from->stat_id;
    to->stat_pp_profile             = from->stat_pp_profile;
    to->vlan_port_id                = from->vlan_port_id;
    to->replication_id              = from->replication_id;
    to->mtu                         = from->mtu;
    to->nat_realm_id                = from->nat_realm_id;
    to->egress_qos_model            = from->egress_qos_model;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    sal_memcpy(to->src_mac_addr, from->src_mac_addr, sizeof(bcm_mac_t));
    sal_memcpy(to->logical_fields, from->logical_fields, BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
}

/*
 * Function:
 *   _bcm_compat6518out_l3_egress_t
 * Purpose:
 *   Convert the bcm_l3_egress_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6518_l3_egress_t *to)
{
    to->flags                       = from->flags;
    to->flags2                      = from->flags2;
    to->intf                        = from->intf;
    to->vlan                        = from->vlan;
    to->module                      = from->module;
    to->port                        = from->port;
    to->trunk                       = from->trunk;
    to->mpls_flags                  = from->mpls_flags;
    to->mpls_label                  = from->mpls_label;
    to->mpls_action                 = from->mpls_action;
    to->mpls_qos_map_id             = from->mpls_qos_map_id;
    to->mpls_ttl                    = from->mpls_ttl;
    to->mpls_pkt_pri                = from->mpls_pkt_pri;
    to->mpls_pkt_cfi                = from->mpls_pkt_cfi;
    to->mpls_exp                    = from->mpls_exp;
    to->qos_map_id                  = from->qos_map_id;
    to->encap_id                    = from->encap_id;
    to->failover_id                 = from->failover_id;
    to->failover_if_id              = from->failover_if_id;
    to->failover_mc_group           = from->failover_mc_group;
    to->dynamic_scaling_factor      = from->dynamic_scaling_factor;
    to->dynamic_load_weight         = from->dynamic_load_weight;
    to->dynamic_queue_size_weight   = from->dynamic_queue_size_weight;
    to->intf_class                  = from->intf_class;
    to->multicast_flags             = from->multicast_flags;
    to->oam_global_context_id       = from->oam_global_context_id;
    to->vntag                       = from->vntag;
    to->vntag_action                = from->vntag_action;
    to->etag                        = from->etag;
    to->etag_action                 = from->etag_action;
    to->flow_handle                 = from->flow_handle;
    to->flow_option_handle          = from->flow_option_handle;
    to->flow_label_option_handle    = from->flow_label_option_handle;
    to->num_of_fields               = from->num_of_fields;
    to->counting_profile            = from->counting_profile;
    to->mpls_ecn_map_id             = from->mpls_ecn_map_id;
    to->urpf_mode                   = from->urpf_mode;
    to->mc_group                    = from->mc_group;
    to->hierarchical_gport          = from->hierarchical_gport;
    to->stat_id                     = from->stat_id;
    to->stat_pp_profile             = from->stat_pp_profile;
    to->vlan_port_id                = from->vlan_port_id;
    to->replication_id              = from->replication_id;
    to->mtu                         = from->mtu;
    to->nat_realm_id                = from->nat_realm_id;
    to->egress_qos_model            = from->egress_qos_model;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    sal_memcpy(to->src_mac_addr, from->src_mac_addr, sizeof(bcm_mac_t));
    sal_memcpy(to->logical_fields, from->logical_fields, BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
}

/*
 * Function:
 *   bcm_compat6518_l3_egress_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_egress_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   flags          - (IN) Operation flags.
 *   egr            - (INOUT) Egress forwarding destination.
 *   if_id          - (INOUT) L3 interface id pointing to Egress object.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6518_l3_egress_t *egr,
    bcm_if_t *if_id)

{
    int rv = 0;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_egress_create(unit, flags, new_egr, if_id);
    if (rv >= 0) {
        _bcm_compat6518out_l3_egress_t(new_egr, egr);
    }

    sal_free(new_egr);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_l3_egress_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_egress_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   intf           - (IN) Egress object ID.
 *   egr            - (OUT) Egress object info.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_egress_t *egr)

{
    int rv = 0;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_egress_get(unit, intf, new_egr);
    if (rv >= 0) {
        _bcm_compat6518out_l3_egress_t(new_egr, egr);
    }

    sal_free(new_egr);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_l3_egress_find
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_egress_find.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   egr            - (IN) Egress object info to match.
 *   intf           - (OUT) Egress object ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_l3_egress_find(
    int unit,
    bcm_compat6518_l3_egress_t *egr,
    bcm_if_t *intf)

{
    int rv = 0;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_egress_find(unit, new_egr, intf);
    if (rv >= 0) {
        _bcm_compat6518out_l3_egress_t(new_egr, egr);
    }

    sal_free(new_egr);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_l3_egress_allocation_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_egress_allocation_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   flags          - (IN) Operation flags.
 *   egr            - (INOUT) Egress object info.
 *   nof_members    - (IN) Number of members.
 *   if_id          - (INOUT) Egress object ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6518_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id)

{
    int rv = 0;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_egress_allocation_get(unit, flags, new_egr, nof_members, if_id);
    if (rv >= 0) {
        _bcm_compat6518out_l3_egress_t(new_egr, egr);
    }

    sal_free(new_egr);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_failover_egress_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_failover_egress_set.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   intf           - (IN) Primary NH interface.
 *   failover_egr   - (INOUT) Egress object to be used as the protection nexthop
 *                            for primary NH.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_egress_t *failover_egr)

{
    int rv = 0;
    bcm_l3_egress_t *new_egr = NULL;

    if (failover_egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l3_egress_t(failover_egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_failover_egress_set(unit, intf, new_egr);
    if (rv >= 0) {
        _bcm_compat6518out_l3_egress_t(new_egr, failover_egr);
    }

    sal_free(new_egr);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_failover_egress_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_failover_egress_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   intf           - (IN) Primary NH interface.
 *   failover_egr   - (INOUT) Egress object to be used as the protection nexthop
 *                            for primary NH.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6518_l3_egress_t *failover_egr)

{
    int rv = 0;
    bcm_l3_egress_t *new_egr = NULL;

    if (failover_egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l3_egress_t(failover_egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_failover_egress_get(unit, intf, new_egr);
    if (rv >= 0) {
        _bcm_compat6518out_l3_egress_t(new_egr, failover_egr);
    }

    sal_free(new_egr);
    return rv;
}
#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *   _bcm_compat6518in_flexctr_group_action_t
 * Purpose:
 *   Convert the bcm_flexctr_group_action_t datatype from <=6.5.18 to
 *   SDK 6.5.18+.
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_flexctr_group_action_t(
    bcm_compat6518_flexctr_group_action_t *from,
    bcm_flexctr_group_action_t *to)
{
    int i;

    bcm_flexctr_group_action_t_init(to);
    to->source = from->source;
    to->ports = from->ports;
    for (i = 0; i < BCM_FLEXCTR_GROUP_ACTION_SIZE; i++) {
        to->stat_counter_id[i] = from->stat_counter_id[i];
    }
    to->action_num = from->action_num;
}

/*
 * Function:
 *   _bcm_compat6518out_flexctr_group_action_t
 * Purpose:
 *   Convert the bcm_flexctr_group_action_t datatype from 6.5.18+ to
 *   <=6.5.18.
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype.
 *   to       - (OUT) The <=6.5.18 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_flexctr_group_action_t(
    bcm_flexctr_group_action_t *from,
    bcm_compat6518_flexctr_group_action_t *to)
{
    int i;

    to->source = from->source;
    to->ports = from->ports;
    for (i = 0; i < BCM_FLEXCTR_GROUP_ACTION_SIZE; i++) {
        to->stat_counter_id[i] = from->stat_counter_id[i];
    }
    to->action_num = from->action_num;
}

/*
 * Function:
 *   bcm_compat6518_flexctr_group_action_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_group_action_create.
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   options                 - (IN) BCM_FLEXCTR_OPTIONS_XXX options.
 *   group_action            - (IN) Flex counter group action data structure.
 *   group_stat_counter_id   - (IN|OUT) Flex counter group ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_flexctr_group_action_create(
    int unit,
    int options,
    bcm_compat6518_flexctr_group_action_t *group_action,
    uint32 *group_stat_counter_id)
{
    int rv = 0;
    bcm_flexctr_group_action_t *new_p = NULL;

    if (group_action != NULL) {
        new_p = (bcm_flexctr_group_action_t *)
            sal_alloc(sizeof(bcm_flexctr_group_action_t), "New pointer");
        if (new_p == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_flexctr_group_action_t(group_action, new_p);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flexctr_group_action_create(unit, options, new_p,
                                         group_stat_counter_id);

    if (rv >= 0) {
        _bcm_compat6518out_flexctr_group_action_t(new_p, group_action);
    }

    sal_free(new_p);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_flexctr_group_action_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flexctr_group_action_get.
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   group_stat_counter_id   - (IN) Flex counter group ID.
 *   group_action            - (OUT) Flex counter group action data structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_flexctr_group_action_get(
    int unit,
    uint32 group_stat_counter_id,
    bcm_compat6518_flexctr_group_action_t *group_action)
{
    int rv = 0;
    bcm_flexctr_group_action_t *new_p = NULL;

    if (group_action != NULL) {
        new_p = (bcm_flexctr_group_action_t *)
            sal_alloc(sizeof(bcm_flexctr_group_action_t), "New pointer");
        if (new_p == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_flexctr_group_action_t(group_action, new_p);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flexctr_group_action_get(unit, group_stat_counter_id, new_p);

    if (rv >= 0) {
        _bcm_compat6518out_flexctr_group_action_t(new_p, group_action);
    }

    sal_free(new_p);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_field_destination_match_t
 * Purpose:
 *      Convert the bcm_field_destination_match_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_field_destination_match_t(
    bcm_compat6518_field_destination_match_t *from,
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
}

/*
 * Function:
 *      _bcm_compat6518out_field_destination_match_t
 * Purpose:
 *      Convert the bcm_field_destination_match_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_field_destination_match_t(
    bcm_field_destination_match_t *from,
    bcm_compat6518_field_destination_match_t *to)
{
    from->priority = to->priority;
    from->gport = to->gport;
    from->gport_mask = to->gport_mask;
    from->mc_group = to->mc_group;
    from->mc_group_mask = to->mc_group_mask;
    from->port_encap = to->port_encap;
    from->port_encap_mask = to->port_encap_mask;
    from->color = to->color;
    from->color_mask = to->color_mask;
    from->elephant_color = to->elephant_color;
    from->elephant_color_mask = to->elephant_color_mask;
    from->elephant = to->elephant;
    from->elephant_mask = to->elephant_mask;
    from->elephant_notify = to->elephant_notify;
    from->elephant_notify_mask = to->elephant_notify_mask;
    from->int_pri = to->int_pri;
    from->int_pri_mask = to->int_pri_mask;
    from->opaque_object_2 = to->opaque_object_2;
    from->opaque_object_2_mask = to->opaque_object_2_mask;
}

int bcm_compat6518_field_destination_entry_add(
    int unit,
    uint32 options,
    bcm_compat6518_field_destination_match_t *match,
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_add(unit, options, new_match, action);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

int bcm_compat6518_field_destination_entry_get(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_get(unit, new_match, action);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

int bcm_compat6518_field_destination_entry_delete(
    int unit,
    bcm_compat6518_field_destination_match_t *match)
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_delete(unit, new_match);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

int bcm_compat6518_field_destination_stat_attach(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_attach(unit, new_match, stat_counter_id);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

int bcm_compat6518_field_destination_stat_id_get(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_id_get(unit, new_match, stat_counter_id);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

int bcm_compat6518_field_destination_stat_detach(
    int unit,
    bcm_compat6518_field_destination_match_t *match)
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_detach(unit, new_match);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

int bcm_compat6518_field_destination_flexctr_object_set(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_flexctr_object_set(unit, new_match, value);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

int bcm_compat6518_field_destination_flexctr_object_get(
    int unit,
    bcm_compat6518_field_destination_match_t *match,
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
        _bcm_compat6518in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_flexctr_object_get(unit, new_match, value);
    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_field_destination_match_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }
    return rv;
}

/*
 * Function:
 *   _bcm_compat6518in_l2_station_t
 * Purpose:
 *   Convert the bcm_l2_station_t datatype from <=6.5.18 to
 *   SDK 6.5.18+
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype
 *   to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_l2_station_t(
        bcm_compat6518_l2_station_t *from,
        bcm_l2_station_t *to)
{
    bcm_l2_station_t_init(to);
    to->flags = from->flags;
    to->priority = from->priority;
    sal_memcpy(to->dst_mac, from->dst_mac_mask, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac_mask, from->dst_mac_mask, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->vlan_mask = from->vlan_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->taf_gate_primap = from->taf_gate_primap;
    to->forward_domain_type = from->forward_domain_type;
    to->forward_domain_type_mask = from->forward_domain_type_mask;
    to->inner_vlan = from->inner_vlan;
    to->inner_vlan_mask = from->inner_vlan_mask;
    to->vfi = from->vfi;
    to->vfi_mask = from->vfi_mask;
}

/*
 * Function:
 *   _bcm_compat6518out_l2_station_t
 * Purpose:
 *   Convert the bcm_l2_station_t datatype from 6.5.18+ to
 *   <=6.5.18
 * Parameters:
 *   from     - (IN) The 6.5.18+ version of the datatype
 *   to       - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518out_l2_station_t(
        bcm_l2_station_t *from,
        bcm_compat6518_l2_station_t *to)
{
    to->flags = from->flags;
    to->priority = from->priority;
    sal_memcpy(to->dst_mac, from->dst_mac_mask, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac_mask, from->dst_mac_mask, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->vlan_mask = from->vlan_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->taf_gate_primap = from->taf_gate_primap;
    to->forward_domain_type = from->forward_domain_type;
    to->forward_domain_type_mask = from->forward_domain_type_mask;
    to->inner_vlan = from->inner_vlan;
    to->inner_vlan_mask = from->inner_vlan_mask;
    to->vfi = from->vfi;
    to->vfi_mask = from->vfi_mask;
}

/*
 * Function:
 *   bcm_compat6518_l2_station_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_l2_station_add
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   id             - (INOUT) Station Id.
 *   station        - (IN) Pointer to station address information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6518_l2_station_add(
        int unit,
        int *station_id,
        bcm_compat6518_l2_station_t *station)
{
    int rv = 0;
    bcm_l2_station_t *new_station = NULL;

    if (station != NULL) {
        new_station = (bcm_l2_station_t*)
            sal_alloc(sizeof(bcm_l2_station_t), "New l2 station");
        if (new_station == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l2_station_t(station, new_station);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l2_station_add(unit, station_id, new_station);

    if (rv >= 0) {
        _bcm_compat6518out_l2_station_t(new_station, station);
    }

    sal_free(new_station);
    return rv;
}

/*
 * Function:
 *   bcm_compat6518_l2_station_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_l2_station_get
 * Parameters:
 *   unit         - (IN) BCM device number
 *   station_id   - (IN) Station ID
 *   station      - (OUT) Pointer to station address information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6518_l2_station_get(
        int unit,
        int station_id,
        bcm_compat6518_l2_station_t *station)
{
    int rv = 0;
    bcm_l2_station_t *new_station = NULL;

    if (station != NULL) {
        new_station = (bcm_l2_station_t*)
            sal_alloc(sizeof(bcm_l2_station_t), "New l2 station");
        if (new_station == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_l2_station_t(station, new_station);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l2_station_get(unit, station_id, new_station);

    if (rv >= 0) {
        _bcm_compat6518out_l2_station_t(new_station, station);
    }

    sal_free(new_station);
    return rv;
}
/*
 * Function:
 *      _bcm_compat6518in_cpri_encap_info_t
 * Purpose:
 *      Convert the bcm_cpri_encap_info_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_cpri_encap_info_t(
    bcm_compat6518_cpri_encap_info_t *from,
    bcm_cpri_encap_info_t *to)
{
    to->flags = from->flags;
    to->flow_id = from->flow_id;
    to->subtype = from->subtype;
    to->hdr_type = from->hdr_type;
    to->roe_ordering_info_index = from->roe_ordering_info_index;
    to->macda_index = from->macda_index;
    to->macsa_index = from->macsa_index;
    to->vlan_type = from->vlan_type;
    to->vlan0_idx = from->vlan0_idx;
    to->vlan1_idx = from->vlan1_idx;
    to->vlan0_priority = from->vlan0_priority;
    to->vlan1_priority = from->vlan1_priority;
    to->vlan_ethertype_idx = from->vlan_ethertype_idx;
    to->opcode = from->opcode;
    to->type = from->type;
    to->version = from->version;
    to->ecpri_pc_id = from->ecpri_pc_id;
    to->ecpri_msg_type = from->ecpri_msg_type;
    to->ecpri12_header_byte0 = 0;
}
/*
 * Function:
 *      _bcm_compat6518out_cpri_encap_info_t
 * Purpose:
 *      Convert the bcm_cpri_encap_info_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_cpri_encap_info_t(
    bcm_cpri_encap_info_t *from,
    bcm_compat6518_cpri_encap_info_t *to)
{
    to->flags = from->flags;
    to->flow_id = from->flow_id;
    to->subtype = from->subtype;
    to->hdr_type = from->hdr_type;
    to->roe_ordering_info_index = from->roe_ordering_info_index;
    to->macda_index = from->macda_index;
    to->macsa_index = from->macsa_index;
    to->vlan_type = from->vlan_type;
    to->vlan0_idx = from->vlan0_idx;
    to->vlan1_idx = from->vlan1_idx;
    to->vlan0_priority = from->vlan0_priority;
    to->vlan1_priority = from->vlan1_priority;
    to->vlan_ethertype_idx = from->vlan_ethertype_idx;
    to->opcode = from->opcode;
    to->type = from->type;
    to->version = from->version;
    to->ecpri_pc_id = from->ecpri_pc_id;
    to->ecpri_msg_type = from->ecpri_msg_type;
}

/*
 * Function:
 *      bcm_compat6518_cpri_port_encap_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cpri_port_encap_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Switch port number.
 *      queue_num - (IN) Queue Number same as AxC Id.
 *      encap_config - (IN) (IN/OUT) encapsulation header configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_cpri_port_encap_set(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_compat6518_cpri_encap_info_t *encap_config)
{
    int rv = BCM_E_NONE;
    bcm_cpri_encap_info_t *new_encap_config = NULL;

    if (encap_config != NULL) {
        new_encap_config = (bcm_cpri_encap_info_t *)
                     sal_alloc(sizeof(bcm_cpri_encap_info_t),
                     "New encap_config");
        if (new_encap_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_cpri_encap_info_t(encap_config, new_encap_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cpri_port_encap_set(unit, port, queue_num, new_encap_config);


    /* Deallocate memory*/
    sal_free(new_encap_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_cpri_port_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cpri_port_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Switch port number.
 *      queue_num - (IN) Queue Number same as AxC Id.
 *      encap_config - (OUT) (IN/OUT) encapsulation header configuration.
 * Returns:
 *      BCM_E_XXX
 */

int bcm_compat6518_cpri_port_encap_get(
    int unit,
    bcm_gport_t port,
    int queue_num,
    bcm_compat6518_cpri_encap_info_t *encap_config)
{
    int rv = BCM_E_NONE;
    bcm_cpri_encap_info_t *new_encap_config = NULL;

    if (encap_config != NULL) {
        new_encap_config = (bcm_cpri_encap_info_t *)
                     sal_alloc(sizeof(bcm_cpri_encap_info_t),
                     "New encap_config");
        if (new_encap_config == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cpri_port_encap_get(unit, port, queue_num, new_encap_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_cpri_encap_info_t(new_encap_config, encap_config);

    /* Deallocate memory*/
    sal_free(new_encap_config);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_compat6518_rx_cosq_mapping_t datatype from <=6.5.18
 *      to SDK 6.5.18+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.18+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_rx_cosq_mapping_t(
    bcm_compat6518_rx_cosq_mapping_t *from,
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
}

/*
 * Function:
 *      _bcm_compat6518out_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_compat6518_rx_cosq_mapping_t datatype from 6.5.18+ to
 *      <=SDK 6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.18+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_rx_cosq_mapping_t(
    bcm_rx_cosq_mapping_t *from,
    bcm_compat6518_rx_cosq_mapping_t *to)
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
}

/*
 * Function: bcm_compat6518_rx_cosq_mapping_extended_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_set.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      options - options.
 *      rx_cosq_mapping - bcm_compat6518_rx_cosq_mapping_t.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6518_rx_cosq_mapping_extended_set(
    int unit,
    uint32 options,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (NULL != rx_cosq_mapping) {
        /* Create from heap to avoid the stack to bloat */
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
            sal_alloc(sizeof(bcm_rx_cosq_mapping_t), "New rx cos mapping");
        if (NULL == new_rx_cosq_mapping) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_set(unit, options, new_rx_cosq_mapping);

    if (NULL != new_rx_cosq_mapping) {
        /* Deallocate memory*/
        sal_free(new_rx_cosq_mapping);
    }

    return rv;
}

/*
 * Function: bcm_compat6518_rx_cosq_mapping_extended_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_get.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      rx_cosq_mapping - bcm_compat6518_rx_cosq_mapping_t.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6518_rx_cosq_mapping_extended_get(
    int unit,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping)

{
    int rv;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (NULL != rx_cosq_mapping) {
        /* Create from heap to avoid the stack to bloat */
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
            sal_alloc(sizeof(bcm_rx_cosq_mapping_t), "New rx cos mapping");
        if (NULL == new_rx_cosq_mapping) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_get(unit, new_rx_cosq_mapping);

    if (NULL != new_rx_cosq_mapping) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_rx_cosq_mapping_t(new_rx_cosq_mapping, rx_cosq_mapping);
        /* Deallocate memory*/
        sal_free(new_rx_cosq_mapping);
    }

    return rv;
}

/*
 * Function: bcm_compat6518_rx_cosq_mapping_extended_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_add.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      options - options.
 *      rx_cosq_mapping - bcm_compat6518_rx_cosq_mapping_t.
 * Returns:
 *      BCM_E_XXXX
 */

int bcm_compat6518_rx_cosq_mapping_extended_add(
    int unit,
    uint32 options,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (NULL != rx_cosq_mapping) {
        /* Create from heap to avoid the stack to bloat */
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
            sal_alloc(sizeof(bcm_rx_cosq_mapping_t), "New rx cos mapping");
        if (NULL == new_rx_cosq_mapping) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_add(unit, options, new_rx_cosq_mapping);

    if (NULL != new_rx_cosq_mapping) {
        /* Deallocate memory*/
        sal_free(new_rx_cosq_mapping);
    }

    return rv;
}
/*
 * Function: bcm_compat6518_rx_cosq_mapping_extended_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_delete.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      rx_cosq_mapping - bcm_compat6518_rx_cosq_mapping_t.
 * Returns:
 *      BCM_E_XXXX
 */
int bcm_compat6518_rx_cosq_mapping_extended_delete(
    int unit,
    bcm_compat6518_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (NULL != rx_cosq_mapping) {
        /* Create from heap to avoid the stack to bloat */
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
            sal_alloc(sizeof(bcm_rx_cosq_mapping_t), "New rx cos mapping");
        if (NULL == new_rx_cosq_mapping) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_delete(unit, new_rx_cosq_mapping);

    if (NULL != new_rx_cosq_mapping) {
        /* Deallocate memory*/
        sal_free(new_rx_cosq_mapping);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_srv6_srh_base_initiator_info_t
 * Purpose:
 *      Convert the bcm_srv6_srh_base_initiator_info_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_srv6_srh_base_initiator_info_t(
    bcm_compat6518_srv6_srh_base_initiator_info_t *from,
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
}

/*
 * Function:
 *      _bcm_compat6518out_srv6_srh_base_initiator_info_t
 * Purpose:
 *      Convert the bcm_srv6_srh_base_initiator_info_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_srv6_srh_base_initiator_info_t(
    bcm_srv6_srh_base_initiator_info_t *from,
    bcm_compat6518_srv6_srh_base_initiator_info_t *to)
{
    to->flags = from->flags;
    to->tunnel_id = from->tunnel_id;
    to->nof_sids = from->nof_sids;
    to->qos_map_id = from->qos_map_id;
    to->ttl = from->ttl;
    to->dscp = from->dscp;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->next_encap_id = from->next_encap_id;
}

/*
 * Function:
 *      bcm_compat6518_srv6_srh_base_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_srv6_srh_base_initiator_create(
    int unit,
    bcm_compat6518_srv6_srh_base_initiator_info_t *info)
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
        _bcm_compat6518in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_srv6_srh_base_initiator_info_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_srv6_srh_base_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_srv6_srh_base_initiator_get(
    int unit,
    bcm_compat6518_srv6_srh_base_initiator_info_t *info)
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
        _bcm_compat6518in_srv6_srh_base_initiator_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_srv6_srh_base_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_srv6_srh_base_initiator_info_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_srv6_srh_base_initiator_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_srv6_srh_base_initiator_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_srv6_srh_base_initiator_delete(
    int unit,
    bcm_compat6518_srv6_srh_base_initiator_info_t *info)
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
        _bcm_compat6518in_srv6_srh_base_initiator_info_t(info, new_info);
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
 *      _bcm_compat6518in_l2_egress_t
 * Purpose:
 *      Convert the bcm_l2_egress_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_l2_egress_t(
    bcm_compat6518_l2_egress_t *from,
    bcm_l2_egress_t *to)
{
    int i1 = 0;

    bcm_l2_egress_t_init(to);
    to->flags = from->flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->dest_mac[i1] = from->dest_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->outer_vlan = from->outer_vlan;
    to->inner_vlan = from->inner_vlan;
    to->ethertype = from->ethertype;
    to->dest_port = from->dest_port;
    to->encap_id = from->encap_id;
    to->outer_tpid = from->outer_tpid;
    to->outer_prio = from->outer_prio;
    to->inner_tpid = from->inner_tpid;
    to->l3_intf = from->l3_intf;
    to->vlan_qos_map_id = from->vlan_qos_map_id;
    to->mpls_extended_label_value = from->mpls_extended_label_value;
    to->vlan_port_id = from->vlan_port_id;
}

/*
 * Function:
 *      _bcm_compat6518out_l2_egress_t
 * Purpose:
 *      Convert the bcm_l2_egress_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_l2_egress_t(
    bcm_l2_egress_t *from,
    bcm_compat6518_l2_egress_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->dest_mac[i1] = from->dest_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->outer_vlan = from->outer_vlan;
    to->inner_vlan = from->inner_vlan;
    to->ethertype = from->ethertype;
    to->dest_port = from->dest_port;
    to->encap_id = from->encap_id;
    to->outer_tpid = from->outer_tpid;
    to->outer_prio = from->outer_prio;
    to->inner_tpid = from->inner_tpid;
    to->l3_intf = from->l3_intf;
    to->vlan_qos_map_id = from->vlan_qos_map_id;
    to->mpls_extended_label_value = from->mpls_extended_label_value;
    to->vlan_port_id = from->vlan_port_id;
}

/*
 * Function:
 *      bcm_compat6518_l2_egress_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (INOUT) (IN/OUT) pointer to an L2 egress structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l2_egress_create(
    int unit,
    bcm_compat6518_l2_egress_t *egr)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l2_egress_t *)
                     sal_alloc(sizeof(bcm_l2_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l2_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_egress_create(unit, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_l2_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_l2_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) encap_id of the entry to which to get config for
 *      egr - (OUT) config of the entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l2_egress_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6518_l2_egress_t *egr)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l2_egress_t *)
                     sal_alloc(sizeof(bcm_l2_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_egress_get(unit, encap_id, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_l2_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_l2_egress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) egress config to search for
 *      encap_id - (OUT) encap ID of the entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l2_egress_find(
    int unit,
    bcm_compat6518_l2_egress_t *egr,
    bcm_if_t *encap_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l2_egress_t *)
                     sal_alloc(sizeof(bcm_l2_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_l2_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_egress_find(unit, new_egr, encap_id);


    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6518in_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_tunnel_terminator_t(
    bcm_compat6518_tunnel_terminator_t *from,
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
}

/*
 * Function:
 *      _bcm_compat6518out_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_tunnel_terminator_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_tunnel_terminator_t(
    bcm_tunnel_terminator_t *from,
    bcm_compat6518_tunnel_terminator_t *to)
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
}

/*
 * Function:
 *      bcm_compat6518_tunnel_terminator_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_tunnel_terminator_add(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6518_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_tunnel_terminator_create(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_tunnel_terminator_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_tunnel_terminator_delete(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6518_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_tunnel_terminator_update(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6518_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_tunnel_terminator_get(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_vxlan_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_vxlan_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_vxlan_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_l2gre_tunnel_terminator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_l2gre_tunnel_terminator_update
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_update.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_update(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_l2gre_tunnel_terminator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_terminator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Terminator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6518_tunnel_terminator_t *info)
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
        _bcm_compat6518in_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_terminator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

#endif /* defined(INCLUDE_L3) */


/*
 * Function:
 *   _bcm_compat6518in_field_flexctr_config_t
 * Purpose:
 *   Convert the bcm_field_flexctr_config_t datatype from <=6.5.18 to
 *   SDK 6.5.19+
 * Parameters:
 *   from        - (IN) The <=6.5.18 version of the datatype.
 *   to          - (OUT) The SDK 6.5.19+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6518in_field_flexctr_config_t(
    bcm_compat6518_field_flexctr_config_t *from,
    bcm_field_flexctr_config_t *to)
{
    to->flexctr_action_id = from->flexctr_action_id;
    to->counter_index = from->counter_index;
}

/*
 * Function:
 *      _bcm_compat6518out_field_flexctr_config_t
 * Purpose:
 *      Convert the bcm_field_flexctr_config_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_field_flexctr_config_t(
    bcm_field_flexctr_config_t *from,
    bcm_compat6518_field_flexctr_config_t *to)
{
    to->flexctr_action_id = from->flexctr_action_id;
    to->counter_index = from->counter_index;
}

/*
 * Function:
 *   bcm_compat6518_field_entry_flexctr_attach
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_entry_flexctr_attach.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   entry          - (IN) Entry Id.
 *   flexctr_cfg    - (IN) flex control config.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_field_entry_flexctr_attach(
    int unit,
    bcm_field_entry_t entry,
    bcm_compat6518_field_flexctr_config_t *flexctr_cfg)
{
    int rv = 0;
    bcm_field_flexctr_config_t *new_config = NULL;

    if (flexctr_cfg != NULL) {
        new_config = (bcm_field_flexctr_config_t*)
            sal_alloc(sizeof(bcm_field_flexctr_config_t), "New flex config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_field_flexctr_config_t(flexctr_cfg, new_config);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_field_entry_flexctr_attach(unit, entry, new_config);

    if (new_config != NULL) {
        _bcm_compat6518out_field_flexctr_config_t(new_config, flexctr_cfg);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6518_field_entry_flexctr_detach
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_entry_flexctr_detach.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   entry          - (IN) Entry Id.
 *   flexctr_cfg    - (IN) flex control config.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6518_field_entry_flexctr_detach(
    int unit,
    bcm_field_entry_t entry,
    bcm_compat6518_field_flexctr_config_t *flexctr_cfg)
{
    int rv = 0;
    bcm_field_flexctr_config_t *new_config = NULL;

    if (flexctr_cfg != NULL) {
        new_config = (bcm_field_flexctr_config_t*)
            sal_alloc(sizeof(bcm_field_flexctr_config_t), "New flex config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6518in_field_flexctr_config_t(flexctr_cfg, new_config);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_field_entry_flexctr_detach(unit, entry, new_config);

    if (new_config != NULL) {
        _bcm_compat6518out_field_flexctr_config_t(new_config, flexctr_cfg);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6518in_stat_pp_profile_info_t
 * Purpose:
 *      Convert the bcm_stat_pp_profile_info_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_stat_pp_profile_info_t(
    bcm_compat6518_stat_pp_profile_info_t *from,
    bcm_stat_pp_profile_info_t *to)
{
    bcm_stat_pp_profile_info_t_init(to);
    to->counter_command_id = from->counter_command_id;
    to->stat_object_type = from->stat_object_type;
    to->meter_command_id = from->meter_command_id;
    to->meter_qos_map_id = from->meter_qos_map_id;
    to->is_meter_enable = from->is_meter_enable;
    to->is_fp_cs_var = from->is_fp_cs_var;
}

/*
 * Function:
 *      _bcm_compat6518out_stat_pp_profile_info_t
 * Purpose:
 *      Convert the bcm_stat_pp_profile_info_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_stat_pp_profile_info_t(
    bcm_stat_pp_profile_info_t *from,
    bcm_compat6518_stat_pp_profile_info_t *to)
{
    to->counter_command_id = from->counter_command_id;
    to->stat_object_type = from->stat_object_type;
    to->meter_command_id = from->meter_command_id;
    to->meter_qos_map_id = from->meter_qos_map_id;
    to->is_meter_enable = from->is_meter_enable;
    to->is_fp_cs_var = from->is_fp_cs_var;
}

/*
 * Function:
 *      bcm_compat6518_stat_pp_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_stat_pp_profile_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Flags
 *      engine_source - (IN) counting source, Legal values: ingressReceivePp, EgressReceivePp, EgressTransmitPp
 *      stat_pp_profile - (INOUT) 
 *      stat_pp_profile_info - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_stat_pp_profile_create(
    int unit,
    int flags,
    bcm_stat_counter_interface_type_t engine_source,
    int *stat_pp_profile,
    bcm_compat6518_stat_pp_profile_info_t *stat_pp_profile_info)
{
    int rv = BCM_E_NONE;
    bcm_stat_pp_profile_info_t *new_stat_pp_profile_info = NULL;

    if (stat_pp_profile_info != NULL) {
        new_stat_pp_profile_info = (bcm_stat_pp_profile_info_t *)
                     sal_alloc(sizeof(bcm_stat_pp_profile_info_t),
                     "New stat_pp_profile_info");
        if (new_stat_pp_profile_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_stat_pp_profile_info_t(stat_pp_profile_info, new_stat_pp_profile_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_stat_pp_profile_create(unit, flags, engine_source, stat_pp_profile, new_stat_pp_profile_info);


    /* Deallocate memory*/
    sal_free(new_stat_pp_profile_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_stat_pp_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_stat_pp_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      stat_pp_profile - (IN) pp profile statistics ID
 *      stat_pp_profile_info - (OUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_stat_pp_profile_get(
    int unit,
    int stat_pp_profile,
    bcm_compat6518_stat_pp_profile_info_t *stat_pp_profile_info)
{
    int rv = BCM_E_NONE;
    bcm_stat_pp_profile_info_t *new_stat_pp_profile_info = NULL;

    if (stat_pp_profile_info != NULL) {
        new_stat_pp_profile_info = (bcm_stat_pp_profile_info_t *)
                     sal_alloc(sizeof(bcm_stat_pp_profile_info_t),
                     "New stat_pp_profile_info");
        if (new_stat_pp_profile_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_stat_pp_profile_get(unit, stat_pp_profile, new_stat_pp_profile_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_stat_pp_profile_info_t(new_stat_pp_profile_info, stat_pp_profile_info);

    /* Deallocate memory*/
    sal_free(new_stat_pp_profile_info);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6518in_vxlan_port_t
 * Purpose:
 *      Convert the bcm_vxlan_port_t datatype from <=6.5.18 to
 *      SDK 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.18 version of the datatype
 *      to          - (OUT) The SDK 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518in_vxlan_port_t(
    bcm_compat6518_vxlan_port_t *from,
    bcm_vxlan_port_t *to)
{
    bcm_vxlan_port_t_init(to);
    to->vxlan_port_id = from->vxlan_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
    to->vnid = from->vnid;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_pkt_pri = from->tunnel_pkt_pri;
    to->tunnel_pkt_cfi = from->tunnel_pkt_cfi;
    to->flags2 = from->flags2;
}

/*
 * Function:
 *      _bcm_compat6518out_vxlan_port_t
 * Purpose:
 *      Convert the bcm_vxlan_port_t datatype from SDK 6.5.19+ to
 *      <=6.5.18
 * Parameters:
 *      from        - (IN) The SDK 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.18 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6518out_vxlan_port_t(
    bcm_vxlan_port_t *from,
    bcm_compat6518_vxlan_port_t *to)
{
    to->vxlan_port_id = from->vxlan_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
    to->vnid = from->vnid;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_pkt_pri = from->tunnel_pkt_pri;
    to->tunnel_pkt_cfi = from->tunnel_pkt_cfi;
    to->flags2 = from->flags2;
}

/*
 * Function:
 *      bcm_compat6518_vxlan_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN)
 *      vxlan_port - (INOUT) (IN/OUT) VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vxlan_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6518_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t *new_vxlan_port = NULL;

    if (vxlan_port != NULL) {
        new_vxlan_port = (bcm_vxlan_port_t *)
                     sal_alloc(sizeof(bcm_vxlan_port_t),
                     "New vxlan_port");
        if (new_vxlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vxlan_port_t(vxlan_port, new_vxlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_add(unit, l2vpn, new_vxlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_vxlan_port_t(new_vxlan_port, vxlan_port);

    /* Deallocate memory*/
    sal_free(new_vxlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_vxlan_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN)
 *      vxlan_port - (INOUT) (IN/OUT) VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vxlan_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6518_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t *new_vxlan_port = NULL;

    if (vxlan_port != NULL) {
        new_vxlan_port = (bcm_vxlan_port_t *)
                     sal_alloc(sizeof(bcm_vxlan_port_t),
                     "New vxlan_port");
        if (new_vxlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6518in_vxlan_port_t(vxlan_port, new_vxlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_get(unit, l2vpn, new_vxlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6518out_vxlan_port_t(new_vxlan_port, vxlan_port);

    /* Deallocate memory*/
    sal_free(new_vxlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6518_vxlan_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN)
 *      port_max - (IN) Maximum number of VXLAN ports in array
 *      port_array - (OUT) Array of VXLAN ports
 *      port_count - (OUT) Number of VXLAN ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6518_vxlan_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_compat6518_vxlan_port_t *port_array,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t *new_port_array = NULL;
    int i = 0;

    if (port_array != NULL && port_max > 0) {
        new_port_array = (bcm_vxlan_port_t *)
                     sal_alloc(port_max * sizeof(bcm_vxlan_port_t),
                     "New port_array");
        if (new_port_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_get_all(unit, l2vpn, port_max, new_port_array, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6518out_vxlan_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_port_array);

    return rv;
}

#endif /* defined(INCLUDE_L3) */
#endif /* BCM_RPC_SUPPORT*/
