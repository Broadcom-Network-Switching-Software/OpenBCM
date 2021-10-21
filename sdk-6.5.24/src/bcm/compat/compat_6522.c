/*
* $Id: compat_6522.c,v 1.0 2021/04/13
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2021 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.22 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6522.h>




/*
 * Function:
 *      _bcm_compat6522in_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_mirror_pkt_dnx_pp_header_t(
    bcm_compat6522_mirror_pkt_dnx_pp_header_t *from,
    bcm_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->eth_header_remove = from->eth_header_remove;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
    to->vsi = from->vsi;
}

/*
 * Function:
 *      _bcm_compat6522out_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from SDK 6.5.23+ to
 *      <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_mirror_pkt_dnx_pp_header_t(
    bcm_mirror_pkt_dnx_pp_header_t *from,
    bcm_compat6522_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->eth_header_remove = from->eth_header_remove;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
    to->vsi = from->vsi;
}

/*
 * Function:
 *      _bcm_compat6522in_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_mirror_header_info_t(
    bcm_compat6522_mirror_header_info_t *from,
    bcm_mirror_header_info_t *to)
{
    int i1 = 0;

    bcm_mirror_header_info_t_init(to);
    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6522in_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      _bcm_compat6522out_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from SDK 6.5.23+ to
 *      <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_mirror_header_info_t(
    bcm_mirror_header_info_t *from,
    bcm_compat6522_mirror_header_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6522out_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      bcm_compat6522_mirror_header_info_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_MIRROR_DEST_* flags
 *      mirror_dest_id - (IN) (IN/OUT) Mirrored destination ID
 *      mirror_header_info - (IN) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6522_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_mirror_header_info_t(mirror_header_info, new_mirror_header_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_set(unit, flags, mirror_dest_id, new_mirror_header_info);


    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_mirror_header_info_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mirror_dest_id - (IN) Mirrored destination ID
 *      flags - (INOUT) (IN/OUT) BCM_MIRROR_DEST_* flags
 *      mirror_header_info - (OUT) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6522_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_get(unit, mirror_dest_id, flags, new_mirror_header_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_mirror_header_info_t(new_mirror_header_info, mirror_header_info);

    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6522in_collector_info_t
 * Purpose:
 *      Convert the bcm_collector_info_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_collector_info_t(
    bcm_compat6522_collector_info_t *from,
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
    to->mirror_dest_id = from->mirror_dest_id;
}

/*
 * Function:
 *      _bcm_compat6522out_collector_info_t
 * Purpose:
 *      Convert the bcm_collector_info_t datatype from SDK 6.5.23+ to
 *      <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_collector_info_t(
    bcm_collector_info_t *from,
    bcm_compat6522_collector_info_t *to)
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
    to->mirror_dest_id = from->mirror_dest_id;
}

/*
 * Function:
 *      bcm_compat6522_collector_create
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
int bcm_compat6522_collector_create(
    int unit,
    uint32 options,
    bcm_collector_t *collector_id,
    bcm_compat6522_collector_info_t *collector_info)
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
        _bcm_compat6522in_collector_info_t(collector_info, new_collector_info);
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
 *      bcm_compat6522_collector_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_collector_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      id - (IN) Collector software ID.
 *      collector_info - (OUT) Pointer to the collector information structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_collector_get(
    int unit,
    bcm_collector_t id,
    bcm_compat6522_collector_info_t *collector_info)
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
    _bcm_compat6522out_collector_info_t(new_collector_info, collector_info);

    /* Deallocate memory*/
    sal_free(new_collector_info);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6522in_switch_pkt_control_action_t
 * Purpose:
 *   Convert the bcm_switch_pkt_control_action_t datatype from <=6.5.22 to
 *   6.5.22+.
 * Parameters:
 *   from     - (IN) The <=6.5.22 version of the datatype.
 *   to       - (OUT) The 6.5.22+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6522in_switch_pkt_control_action_t(
    bcm_compat6522_switch_pkt_control_action_t *from,
    bcm_switch_pkt_control_action_t *to)
{
    bcm_switch_pkt_control_action_t_init(to);
    to->discard        = from->discard;
    to->copy_to_cpu    = from->copy_to_cpu;
    to->flood          = from->flood;
    to->bpdu           = from->bpdu;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->class_id       = from->class_id;
}

/*
 * Function:
 *   _bcm_compat6522out_switch_pkt_control_action_t
 * Purpose:
 *   Convert the bcm_switch_pkt_control_action_t datatype from 6.5.22+ to
 *   <=6.5.22.
 * Parameters:
 *   from     - (IN) The 6.5.22+ version of the datatype.
 *   to       - (OUT) The <=6.5.22 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6522out_switch_pkt_control_action_t(
    bcm_switch_pkt_control_action_t *from,
    bcm_compat6522_switch_pkt_control_action_t *to)
{
    to->discard        = from->discard;
    to->copy_to_cpu    = from->copy_to_cpu;
    to->flood          = from->flood;
    to->bpdu           = from->bpdu;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->class_id       = from->class_id;
}

/*
 * Function:
 *   _bcm_compat6522in_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_switch_pkt_protocol_match_t datatype from <=6.5.22 to
 *   6.5.22+.
 * Parameters:
 *   from     - (IN) The <=6.5.22 version of the datatype.
 *   to       - (OUT) The 6.5.22+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6522in_switch_pkt_protocol_match_t(
    bcm_compat6522_switch_pkt_protocol_match_t *from,
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
    to->vlan_xlate_opaque_ctrl_id        = from->vlan_xlate_opaque_ctrl_id;
    to->vlan_xlate_opaque_ctrl_id_mask   = from->vlan_xlate_opaque_ctrl_id_mask;
    to->vlan_xlate_opaque_ctrl_id_1      = from->vlan_xlate_opaque_ctrl_id_1;
    to->vlan_xlate_opaque_ctrl_id_1_mask = from->vlan_xlate_opaque_ctrl_id_1_mask;
    to->l2_iif_opaque_ctrl_id_1          = from->l2_iif_opaque_ctrl_id_1;
    to->l2_iif_opaque_ctrl_id_1_mask     = from->l2_iif_opaque_ctrl_id_1_mask;
    to->fixed_hve_result_set_4           = from->fixed_hve_result_set_4;
    to->fixed_hve_result_set_4_mask      = from->fixed_hve_result_set_4_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   _bcm_compat6522out_switch_pkt_protocol_match_t
 * Purpose:
 *   Convert the bcm_switch_pkt_protocol_match_t datatype from 6.5.22+ to
 *   <=6.5.22.
 * Parameters:
 *   from     - (IN) The 6.5.22+ version of the datatype.
 *   to       - (OUT) The <=6.5.22 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6522out_switch_pkt_protocol_match_t(
    bcm_switch_pkt_protocol_match_t *from,
    bcm_compat6522_switch_pkt_protocol_match_t *to)
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
    to->vlan_xlate_opaque_ctrl_id        = from->vlan_xlate_opaque_ctrl_id;
    to->vlan_xlate_opaque_ctrl_id_mask   = from->vlan_xlate_opaque_ctrl_id_mask;
    to->vlan_xlate_opaque_ctrl_id_1      = from->vlan_xlate_opaque_ctrl_id_1;
    to->vlan_xlate_opaque_ctrl_id_1_mask = from->vlan_xlate_opaque_ctrl_id_1_mask;
    to->l2_iif_opaque_ctrl_id_1          = from->l2_iif_opaque_ctrl_id_1;
    to->l2_iif_opaque_ctrl_id_1_mask     = from->l2_iif_opaque_ctrl_id_1_mask;
    to->fixed_hve_result_set_4           = from->fixed_hve_result_set_4;
    to->fixed_hve_result_set_4_mask      = from->fixed_hve_result_set_4_mask;
    sal_memcpy(to->macda, from->macda, sizeof(bcm_mac_t));
    sal_memcpy(to->macda_mask, from->macda_mask, sizeof(bcm_mac_t));
}

/*
 * Function:
 *   bcm_compat6522_switch_pkt_protocol_control_add
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
bcm_compat6522_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6522_switch_pkt_protocol_match_t *match,
    bcm_compat6522_switch_pkt_control_action_t *action,
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
        _bcm_compat6522in_switch_pkt_protocol_match_t(match, new_match);
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
        _bcm_compat6522in_switch_pkt_control_action_t(action, new_action);
    } else {
        sal_free(new_match);
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_add(unit, options, new_match,
                                             new_action, priority);

    if (rv >= 0) {
        _bcm_compat6522out_switch_pkt_protocol_match_t(new_match, match);
        _bcm_compat6522out_switch_pkt_control_action_t(new_action, action);
    }

    sal_free(new_action);
    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6522_switch_pkt_protocol_control_get
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
bcm_compat6522_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6522_switch_pkt_protocol_match_t *match,
    bcm_compat6522_switch_pkt_control_action_t *action,
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
        _bcm_compat6522in_switch_pkt_protocol_match_t(match, new_match);
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
        _bcm_compat6522in_switch_pkt_control_action_t(action, new_action);
    } else {
        sal_free(new_match);
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_get(unit, new_match, new_action, priority);

    if (rv >= 0) {
        _bcm_compat6522out_switch_pkt_protocol_match_t(new_match, match);
        _bcm_compat6522out_switch_pkt_control_action_t(new_action, action);
    }

    sal_free(new_action);
    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6522_switch_pkt_protocol_control_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_protocol_control_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Protocol match structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6522_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6522_switch_pkt_protocol_match_t *match)
{
    int rv = 0;
    bcm_switch_pkt_protocol_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_protocol_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_protocol_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6522in_switch_pkt_protocol_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_protocol_control_delete(unit, new_match);

    if (rv >= 0) {
        _bcm_compat6522out_switch_pkt_protocol_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6522in_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_udf_pkt_format_info_t(
    bcm_compat6522_udf_pkt_format_info_t *from,
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
    to->outer_ifa = from->outer_ifa;
    to->inner_ifa = from->inner_ifa;
    return;
}

/*
 * Function:
 *      _bcm_compat6522out_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from 6.5.23+ to
 *      SDK <=6.5.22
 * Parameters:
 *      from        - (IN) The 6.5.23+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_udf_pkt_format_info_t(
    bcm_udf_pkt_format_info_t *from,
    bcm_compat6522_udf_pkt_format_info_t *to)
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
    to->outer_ifa = from->outer_ifa;
    to->inner_ifa = from->inner_ifa;
    return;
}

int
bcm_compat6522_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_compat6522_udf_pkt_format_info_t *pkt_format,
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
        _bcm_compat6522in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_create(unit, options,
            new_pkt_format, pkt_format_id);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6522out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

int
bcm_compat6522_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_compat6522_udf_pkt_format_info_t *pkt_format)
{
    int rv;
    bcm_udf_pkt_format_info_t *new_pkt_format = NULL;

    if (pkt_format != NULL) {
        new_pkt_format = sal_alloc(sizeof (bcm_udf_pkt_format_info_t),
                "New Pkt fmt");
        if (new_pkt_format == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6522in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_info_get(unit, pkt_format_id, new_pkt_format);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6522out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6522in_sat_ctf_stat_config_t
 * Purpose:
 *      Convert the bcm_sat_ctf_stat_config_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_sat_ctf_stat_config_t(
    bcm_compat6522_sat_ctf_stat_config_t *from,
    bcm_sat_ctf_stat_config_t *to)
{
    bcm_sat_ctf_stat_config_t_init(to);
    to->use_global_bin_config = from->use_global_bin_config;
    to->bin_min_delay = from->bin_min_delay;
    to->bin_step = from->bin_step;
    to->update_counters_in_unvavail_state = from->update_counters_in_unvavail_state;
}

/*
 * Function:
 *      bcm_compat6522_sat_ctf_stat_config_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_sat_ctf_stat_config_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ctf_id - (IN) CTF ID
 *      stat - (IN) Pointer to SAT ctf stat config structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_sat_ctf_stat_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_compat6522_sat_ctf_stat_config_t *stat)
{
    int rv = BCM_E_NONE;
    bcm_sat_ctf_stat_config_t *new_stat = NULL;

    if (stat != NULL) {
        new_stat = (bcm_sat_ctf_stat_config_t *)
                     sal_alloc(sizeof(bcm_sat_ctf_stat_config_t),
                     "New stat");
        if (new_stat == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_sat_ctf_stat_config_t(stat, new_stat);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_sat_ctf_stat_config_set(unit, ctf_id, new_stat);


    /* Deallocate memory*/
    sal_free(new_stat);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6522in_l3_egress_ecmp_t
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_l3_egress_ecmp_t(
    bcm_compat6522_l3_egress_ecmp_t *from,
    bcm_l3_egress_ecmp_t *to)
{
    bcm_l3_egress_ecmp_t_init(to);
    to->flags = from->flags;
    to->ecmp_intf = from->ecmp_intf;
    to->max_paths = from->max_paths;
    to->ecmp_group_flags = from->ecmp_group_flags;
    to->dynamic_mode = from->dynamic_mode;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_l3_dgm_t));
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->rpf_mode = from->rpf_mode;
    sal_memcpy(&to->tunnel_priority, &from->tunnel_priority, sizeof(bcm_l3_tunnel_priority_info_t));
    to->rh_random_seed = from->rh_random_seed;
}

/*
 * Function:
 *      _bcm_compat6522out_l3_egress_ecmp_t
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from SDK 6.5.23+ to
 *      <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_l3_egress_ecmp_t(
    bcm_l3_egress_ecmp_t *from,
    bcm_compat6522_l3_egress_ecmp_t *to)
{
    to->flags = from->flags;
    to->ecmp_intf = from->ecmp_intf;
    to->max_paths = from->max_paths;
    to->ecmp_group_flags = from->ecmp_group_flags;
    to->dynamic_mode = from->dynamic_mode;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_l3_dgm_t));
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->rpf_mode = from->rpf_mode;
    sal_memcpy(&to->tunnel_priority, &from->tunnel_priority, sizeof(bcm_l3_tunnel_priority_info_t));
    to->rh_random_seed = from->rh_random_seed;
}

/*
 * Function:
 *      _bcm_compat6522in_l3_egress_ecmp_resilient_entry_t
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_resilient_entry_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_l3_egress_ecmp_resilient_entry_t(
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *from,
    bcm_l3_egress_ecmp_resilient_entry_t *to)
{
    to->hash_key = from->hash_key;
    _bcm_compat6522in_l3_egress_ecmp_t(&from->ecmp, &to->ecmp);
    to->intf = from->intf;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_resilient_replace
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_replace.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_TRUNK_RESILIENT flags.
 *      match_entry - (IN) Matching criteria
 *      num_entries - (OUT) Number of entries matched.
 *      replace_entry - (IN) Replacing entry when the action is replace.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *replace_entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_resilient_entry_t *new_match_entry = NULL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_replace_entry = NULL;

    if (match_entry != NULL && replace_entry != NULL) {
        new_match_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New match_entry");
        if (new_match_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_resilient_entry_t(match_entry, new_match_entry);
        new_replace_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New replace_entry");
        if (new_replace_entry == NULL) {
            sal_free(new_match_entry);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_resilient_entry_t(replace_entry, new_replace_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_resilient_replace(unit, flags, new_match_entry, num_entries, new_replace_entry);


    /* Deallocate memory*/
    sal_free(new_match_entry);
    sal_free(new_replace_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_resilient_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_resilient_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_resilient_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_resilient_add(unit, flags, new_entry);


    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_resilient_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6522_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_resilient_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_resilient_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, new_entry);


    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_ecmp_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_create.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      options - (IN) L3_ECMP_O_xxx flags.
 *      ecmp_info - (INOUT) (IN/OUT) ECMP group info.
 *      ecmp_member_count - (IN) Number of elements in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of Egress forwarding objects.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6522_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp_info = NULL;

    if (ecmp_info != NULL) {
        new_ecmp_info = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp_info");
        if (new_ecmp_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp_info, new_ecmp_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_create(unit, options, new_ecmp_info, ecmp_member_count, ecmp_member_array);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_l3_egress_ecmp_t(new_ecmp_info, ecmp_info);

    /* Deallocate memory*/
    sal_free(new_ecmp_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_ecmp_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp_info - (INOUT) (IN/OUT) ECMP group info.
 *      ecmp_member_size - (IN) Size of allocated entries in ecmp_member_array.
 *      ecmp_member_array - (OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count - (OUT) Number of entries of ecmp_member_count actually filled in. This will be a value less than or equal to the value passed in as ecmp_member_size unless ecmp_member_size is 0. If ecmp_member_size is 0 then ecmp_member_array is ignored and ecmp_member_count is filled in with the number of entries that would have been filled into ecmp_member_array if ecmp_member_size was arbitrarily large.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_ecmp_get(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp_info = NULL;

    if (ecmp_info != NULL) {
        new_ecmp_info = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp_info");
        if (new_ecmp_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp_info, new_ecmp_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_get(unit, new_ecmp_info, ecmp_member_size, ecmp_member_array, ecmp_member_count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_l3_egress_ecmp_t(new_ecmp_info, ecmp_info);

    /* Deallocate memory*/
    sal_free(new_ecmp_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_ecmp_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp_member_count - (IN) Number of member in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of Egress forwarding objects.
 *      ecmp_info - (OUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6522_l3_egress_ecmp_t *ecmp_info)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp_info = NULL;

    if (ecmp_info != NULL) {
        new_ecmp_info = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp_info");
        if (new_ecmp_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_find(unit, ecmp_member_count, ecmp_member_array, new_ecmp_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_l3_egress_ecmp_t(new_ecmp_info, ecmp_info);

    /* Deallocate memory*/
    sal_free(new_ecmp_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_create.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (INOUT) (IN/OUT) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_create(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_create(unit, new_ecmp, intf_count, intf_array);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_l3_egress_ecmp_t(new_ecmp, ecmp);

    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_destroy
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_destroy.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (IN) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_destroy(unit, new_ecmp);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (INOUT) (IN/OUT) ECMP group info.
 *      intf_size - (IN) Size of allocated entries in intf_array.
 *      intf_array - (OUT) Array of Egress forwarding objects.
 *      intf_count - (OUT) Number of entries of intf_count actually filled in. This will be a value less than or equal to the value passed in as intf_size unless intf_size is 0. If intf_size is 0 then intf_array is ignored and intf_count is filled in with the number of entries that would have been filled into intf_array if intf_size was arbitrarily large.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_get(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_get(unit, new_ecmp, intf_size, intf_array, intf_count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_l3_egress_ecmp_t(new_ecmp, ecmp);

    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_add(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_add(unit, new_ecmp, intf);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_delete(unit, new_ecmp, intf);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      ecmp - (OUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6522_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_find(unit, intf_count, intf_array, new_ecmp);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_l3_egress_ecmp_t(new_ecmp, ecmp);

    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_egress_ecmp_tunnel_priority_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_tunnel_priority_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6522_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_tunnel_priority_set(unit, new_ecmp, intf_count, intf_array);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}
#endif /** INCLUDE_L3 */

#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6522in_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from <=6.5.22 to 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The >6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_flow_match_config_t(
    bcm_compat6522_flow_match_config_t *from,
    bcm_flow_match_config_t *to)
{
    bcm_flow_match_config_t_init(to);
    to->vnid                = from->vnid;
    to->vlan                = from->vlan;
    to->inner_vlan          = from->inner_vlan;
    to->sip                 = from->sip;
    to->sip_mask            = from->sip_mask;
    to->dip                 = from->dip;
    to->dip_mask            = from->dip_mask;
    to->udp_dst_port        = from->udp_dst_port;
    to->udp_src_port        = from->udp_src_port;
    to->protocol            = from->protocol;
    to->mpls_label          = from->mpls_label;
    to->flow_port           = from->flow_port;
    to->vpn                 = from->vpn;
    to->intf_id             = from->intf_id;
    to->options             = from->options;
    to->criteria            = from->criteria;
    to->valid_elements      = from->valid_elements;
    to->flow_handle         = from->flow_handle;
    to->flow_option         = from->flow_option;
    to->flags               = from->flags;
    sal_memcpy(to->sip6,      from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->sip6_mask, from->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,      from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6_mask, from->dip6_mask, sizeof(bcm_ip6_t));
}

/*
 * Function:
 *      _bcm_compat6522out_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from >6.5.22 to <=6.5.22
 * Parameters:
 *      from        - (IN) The >6.5.22 version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_flow_match_config_t(
    bcm_flow_match_config_t *from,
    bcm_compat6522_flow_match_config_t *to)
{
    to->vnid                = from->vnid;
    to->vlan                = from->vlan;
    to->inner_vlan          = from->inner_vlan;
    to->sip                 = from->sip;
    to->sip_mask            = from->sip_mask;
    to->dip                 = from->dip;
    to->dip_mask            = from->dip_mask;
    to->udp_dst_port        = from->udp_dst_port;
    to->udp_src_port        = from->udp_src_port;
    to->protocol            = from->protocol;
    to->mpls_label          = from->mpls_label;
    to->flow_port           = from->flow_port;
    to->vpn                 = from->vpn;
    to->intf_id             = from->intf_id;
    to->options             = from->options;
    to->criteria            = from->criteria;
    to->valid_elements      = from->valid_elements;
    to->flow_handle         = from->flow_handle;
    to->flow_option         = from->flow_option;
    to->flags               = from->flags;
    sal_memcpy(to->sip6,      from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->sip6_mask, from->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,      from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6_mask, from->dip6_mask, sizeof(bcm_ip6_t));
}

int
bcm_compat6522_flow_match_add(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_add(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6522_flow_match_delete(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_delete(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6522_flow_match_get(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6522_flow_match_flexctr_object_set(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 value)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_set(unit, new_info, value);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6522_flow_match_flexctr_object_get(
    int unit,
    bcm_compat6522_flow_match_config_t *info,
    uint32 *value)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_get(unit, new_info, value);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6522_flow_match_stat_attach(
    int unit, 
    bcm_compat6522_flow_match_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field, 
    uint32 stat_counter_id)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6522_flow_match_stat_detach(
    int unit, 
    bcm_compat6522_flow_match_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_detach(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6522_flow_match_stat_id_get(
    int unit, 
    bcm_compat6522_flow_match_config_t *info, 
    uint32 num_of_fields, 
    bcm_flow_logical_field_t *field, 
    uint32 *stat_counter_id)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

#endif /** INCLUDE_L3 */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6522in_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from <=6.5.22 to
 *      >SDK 6.5.22
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The >SDK 6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_mpls_port_t(
    bcm_compat6522_mpls_port_t *from,
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
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    to->egress_label = from->egress_label;
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
    sal_memcpy(&to->egress_tunnel_label, &from->egress_tunnel_label, sizeof(bcm_mpls_egress_label_t));
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
 *      _bcmcompat6522out_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from >SDK 6.5.22 to
 *      <=6.5.22
 * Parameters:
 *      from        - (IN) The >SDK 6.5.22 version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_mpls_port_t(
    bcm_mpls_port_t *from,
    bcm_compat6522_mpls_port_t *to)
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
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
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
    sal_memcpy(&to->egress_tunnel_label, &from->egress_tunnel_label, sizeof(bcm_mpls_egress_label_t));
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
 *      bcm_compat6522_mpls_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6522_mpls_port_t *mpls_port)
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
        _bcm_compat6522in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_add(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_mpls_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6522_mpls_port_t *mpls_port)
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
        _bcm_compat6522in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_mpls_port_get_all
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
int bcm_compat6522_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6522_mpls_port_t *port_array,
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
        _bcm_compat6522out_mpls_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_port_array);

    return rv;
}
#endif /* INCLUDE_L3 */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6522in_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from <=6.5.22 to 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_l3_host_t(
    bcm_compat6522_l3_host_t *from,
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
}

/*
 * Function:
 *      _bcm_compat6522out_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from 6.5.23+ to <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_l3_host_t(
    bcm_l3_host_t *from,
    bcm_compat6522_l3_host_t *to)
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
}

int bcm_compat6522_l3_host_find(
    int unit,
    bcm_compat6522_l3_host_t *info)
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_find(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_add(
    int unit,
    bcm_compat6522_l3_host_t *info)
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_add(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_delete(
    int unit,
    bcm_compat6522_l3_host_t *ip_addr)
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
        _bcm_compat6522in_l3_host_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_delete_by_interface(
    int unit,
    bcm_compat6522_l3_host_t *info)
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_delete_all(
    int unit,
    bcm_compat6522_l3_host_t *info)
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_stat_attach(
    int unit,
    bcm_compat6522_l3_host_t *info,
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_stat_detach(
    int unit,
    bcm_compat6522_l3_host_t *info)
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_flexctr_object_set(
    int unit,
    bcm_compat6522_l3_host_t *info,
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_flexctr_object_set(unit, new_info, value);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_flexctr_object_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
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
        _bcm_compat6522in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_flexctr_object_get(unit, new_info, value);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_host_stat_counter_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
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
        _bcm_compat6522in_l3_host_t(info, new_info);
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

int bcm_compat6522_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
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
        _bcm_compat6522in_l3_host_t(info, new_info);
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

int bcm_compat6522_l3_host_stat_counter_set(
    int unit,
    bcm_compat6522_l3_host_t *info,
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
        _bcm_compat6522in_l3_host_t(info, new_info);
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

int bcm_compat6522_l3_host_stat_id_get(
    int unit,
    bcm_compat6522_l3_host_t *info,
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
        _bcm_compat6522in_l3_host_t(info, new_info);
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
 *      _bcm_compat6522in_l3_route_t
 * Purpose:
 *      Convert the bcm_l3_route_t datatype from <=6.5.22 to 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_l3_route_t(
    bcm_compat6522_l3_route_t *from,
    bcm_l3_route_t *to)
{
    int i = 0;

    bcm_l3_route_t_init(to);
    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_flags3 = from->l3a_flags3;
    to->l3a_ipmc_flags = from->l3a_ipmc_flags;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_subnet = from->l3a_subnet;
    sal_memcpy(to->l3a_ip6_net, from->l3a_ip6_net, sizeof(bcm_ip6_t));
    to->l3a_ip_mask = from->l3a_ip_mask;
    sal_memcpy(to->l3a_ip6_mask, from->l3a_ip6_mask, sizeof(bcm_ip6_t));
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    to->l3a_nexthop_ip = from->l3a_nexthop_ip;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_vid = from->l3a_vid;
    to->l3a_pri = from->l3a_pri;
    to->l3a_tunnel_option = from->l3a_tunnel_option;
    to->l3a_mpls_label = from->l3a_mpls_label;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->l3a_expected_intf = from->l3a_expected_intf;
    to->l3a_rp = from->l3a_rp;
    to->l3a_mc_group = from->l3a_mc_group;
    to->l3a_expected_src_gport = from->l3a_expected_src_gport;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        sal_memcpy(&to->logical_fields[i], &from->logical_fields[i], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
    to->l3a_int_action_profile_id = from->l3a_int_action_profile_id;
    to->l3a_int_opaque_data_profile_id = from->l3a_int_opaque_data_profile_id;
    to->l3a_opaque_ctrl_id = from->l3a_opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6522out_l3_route_t
 * Purpose:
 *      Convert the bcm_l3_route_t datatype from 6.5.23+ to <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_l3_route_t(
    bcm_l3_route_t *from,
    bcm_compat6522_l3_route_t *to)
{
    int i = 0;

    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_flags3 = from->l3a_flags3;
    to->l3a_ipmc_flags = from->l3a_ipmc_flags;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_subnet = from->l3a_subnet;
    sal_memcpy(to->l3a_ip6_net, from->l3a_ip6_net, sizeof(bcm_ip6_t));
    to->l3a_ip_mask = from->l3a_ip_mask;
    sal_memcpy(to->l3a_ip6_mask, from->l3a_ip6_mask, sizeof(bcm_ip6_t));
    to->l3a_intf = from->l3a_intf;
    to->l3a_ul_intf = from->l3a_ul_intf;
    to->l3a_nexthop_ip = from->l3a_nexthop_ip;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_vid = from->l3a_vid;
    to->l3a_pri = from->l3a_pri;
    to->l3a_tunnel_option = from->l3a_tunnel_option;
    to->l3a_mpls_label = from->l3a_mpls_label;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->l3a_expected_intf = from->l3a_expected_intf;
    to->l3a_rp = from->l3a_rp;
    to->l3a_mc_group = from->l3a_mc_group;
    to->l3a_expected_src_gport = from->l3a_expected_src_gport;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        sal_memcpy(&to->logical_fields[i], &from->logical_fields[i], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->l3a_mtu = from->l3a_mtu;
    to->l3a_int_action_profile_id = from->l3a_int_action_profile_id;
    to->l3a_int_opaque_data_profile_id = from->l3a_int_opaque_data_profile_id;
    to->l3a_opaque_ctrl_id = from->l3a_opaque_ctrl_id;
}

int bcm_compat6522_l3_host_delete_by_network(
    int unit,
    bcm_compat6522_l3_route_t *ip_addr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != ip_addr) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_network(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_add(
    int unit,
    bcm_compat6522_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_add(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_delete(
    int unit,
    bcm_compat6522_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_delete_by_interface(
    int unit,
    bcm_compat6522_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_delete_all(
    int unit,
    bcm_compat6522_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_get(
    int unit,
    bcm_compat6522_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_get(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_multipath_get(
    int unit,
    bcm_compat6522_l3_route_t *the_route,
    bcm_compat6522_l3_route_t *path_array,
    int max_path,
    int *path_count)
{
    int rv, i;
    bcm_l3_route_t *new_info = NULL;
    bcm_l3_route_t *new_paths = NULL;

    if (NULL != the_route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(the_route, new_info);
    }

    if (NULL != path_array) {
        /* Create from heap to avoid the stack to bloat */
        new_paths = sal_alloc((sizeof(bcm_l3_route_t) * max_path), "New Route");
        if (NULL == new_paths) {
            if (NULL != new_info) {
                sal_free(new_info);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_multipath_get(unit, new_info, new_paths, max_path,
                                    path_count);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    if (NULL != new_paths) {
        for (i = 0; i < max_path; i++) {
            /* Transform the entry from the new format to old one */
            _bcm_compat6522out_l3_route_t(&new_paths[i], &path_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_paths);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_get(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 *val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_get(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_get32(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 *val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_get32(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_set(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_set(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_set32(
    int unit,
    bcm_compat6522_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_set32(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_enable_set(
    int unit,
    bcm_compat6522_l3_route_t *route,
    int enable)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_enable_set(unit, new_info, enable);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_attach(
    int unit,
    bcm_compat6522_l3_route_t *route,
    uint32 stat_counter_id)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_detach(
    int unit,
    bcm_compat6522_l3_route_t *route)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_counter_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_get(unit, new_info, stat, num_entries,
                                       counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_counter_sync_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_sync_get(unit, new_info, stat, num_entries,
                                            counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_counter_set(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_set(unit, new_info, stat, num_entries,
                                       counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_multi_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_get(unit, new_info, nstat, stat_arr,
                                     value_arr);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_multi_get32(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_get32(unit, new_info, nstat, stat_arr,
                                       value_arr);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_multi_set(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_set(unit, new_info, nstat, stat_arr,
                                     value_arr);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_multi_set32(
    int unit,
    bcm_compat6522_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_set32(unit, new_info, nstat, stat_arr,
                                       value_arr);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_stat_id_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6522_l3_route_find(
    int unit,
    bcm_compat6522_l3_host_t *host,
    bcm_compat6522_l3_route_t *route)
{
    int rv;
    bcm_l3_host_t *new_host = NULL;
    bcm_l3_route_t *new_route = NULL;

    if (NULL != host) {
        /* Create from heap to avoid the stack to bloat */
        new_host = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_host) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_host_t(host, new_host);
    }

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_route = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_route) {
            if (NULL != new_host) {
                sal_free(new_host);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_find(unit, new_host, new_route);

    if (NULL != new_host) {
        /* Deallocate memory*/
        sal_free(new_host);
    }

    if (NULL != new_route) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_l3_route_t(new_route, route);
        /* Deallocate memory*/
        sal_free(new_route);
    }

    return rv;
}

int bcm_compat6522_l3_subnet_route_find(
    int unit,
    bcm_compat6522_l3_route_t *input,
    bcm_compat6522_l3_route_t *route)
{
    int rv;
    bcm_l3_route_t *new_input = NULL;
    bcm_l3_route_t *new_route = NULL;

    if (NULL != input) {
        /* Create from heap to avoid the stack to bloat */
        new_input = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_input) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(input, new_input);
    }

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_route = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_route) {
            if (NULL != new_input) {
                sal_free(new_input);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_subnet_route_find(unit, new_input, new_route);

    if (NULL != new_input) {
        /* Deallocate memory*/
        sal_free(new_input);
    }

    if (NULL != new_route) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6522out_l3_route_t(new_route, route);
        /* Deallocate memory*/
        sal_free(new_route);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_route_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_route_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 route information.
 *      value - (IN) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_route_flexctr_object_set(
    int unit,
    bcm_compat6522_l3_route_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_l3_route_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_route_t *)
                     sal_alloc(sizeof(bcm_l3_route_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_l3_route_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_route_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 route information.
 *      value - (OUT) The flex counter object value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_l3_route_flexctr_object_get(
    int unit,
    bcm_compat6522_l3_route_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_l3_route_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_l3_route_t *)
                     sal_alloc(sizeof(bcm_l3_route_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_l3_route_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_compat6522in_mirror_destination_t
 * Purpose:
 *      Convert the bcm_mirror_destination_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_mirror_destination_t(
    bcm_compat6522_mirror_destination_t *from,
    bcm_mirror_destination_t *to)
{
    int i1 = 0;

    bcm_mirror_destination_t_init(to);
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src6_addr[i1] = from->src6_addr[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dst6_addr[i1] = from->dst6_addr[i1];
    }
    to->flow_label = from->flow_label;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac[i1] = from->dst_mac[i1];
    }
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
    sal_memcpy(&to->packet_control_updates, &from->packet_control_updates, sizeof(bcm_mirror_pkt_header_updates_t));
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    for (i1 = 0; i1 < BCM_MIRROR_EXT_STAT_ID_COUNT; i1++) {
        to->ext_stat_id[i1] = from->ext_stat_id[i1];
    }
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    sal_memcpy(&to->erspan_header, &from->erspan_header, sizeof(bcm_mirror_pkt_erspan_encap_t));
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    for (i1 = 0; i1 < BCM_MIRROR_EXT_STAT_ID_COUNT; i1++) {
        to->ext_stat_type_id[i1] = from->ext_stat_type_id[i1];
    }
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
    sal_memcpy(&to->int_probe_header, &from->int_probe_header, sizeof(bcm_mirror_int_probe_header_t));
    to->duplicate_pri = from->duplicate_pri;
    to->ip_proto = from->ip_proto;
    to->switch_id = from->switch_id;
}

/*
 * Function:
 *      _bcm_compat6522out_mirror_destination_t
 * Purpose:
 *      Convert the bcm_mirror_destination_t datatype from SDK 6.5.23+ to
 *      <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_mirror_destination_t(
    bcm_mirror_destination_t *from,
    bcm_compat6522_mirror_destination_t *to)
{
    int i1 = 0;

    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    for (i1 = 0; i1 < 16; i1++) {
        to->src6_addr[i1] = from->src6_addr[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dst6_addr[i1] = from->dst6_addr[i1];
    }
    to->flow_label = from->flow_label;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac[i1] = from->dst_mac[i1];
    }
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
    sal_memcpy(&to->packet_control_updates, &from->packet_control_updates, sizeof(bcm_mirror_pkt_header_updates_t));
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    for (i1 = 0; i1 < BCM_MIRROR_EXT_STAT_ID_COUNT; i1++) {
        to->ext_stat_id[i1] = from->ext_stat_id[i1];
    }
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    sal_memcpy(&to->erspan_header, &from->erspan_header, sizeof(bcm_mirror_pkt_erspan_encap_t));
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    for (i1 = 0; i1 < BCM_MIRROR_EXT_STAT_ID_COUNT; i1++) {
        to->ext_stat_type_id[i1] = from->ext_stat_type_id[i1];
    }
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
    sal_memcpy(&to->int_probe_header, &from->int_probe_header, sizeof(bcm_mirror_int_probe_header_t));
    to->duplicate_pri = from->duplicate_pri;
    to->ip_proto = from->ip_proto;
    to->switch_id = from->switch_id;
}

/*
 * Function:
 *      bcm_compat6522_mirror_destination_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_destination_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mirror_dest - (INOUT) (IN/OUT) Mirrored destination and encapsulation
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_mirror_destination_create(
    int unit,
    bcm_compat6522_mirror_destination_t *mirror_dest)
{
    int rv = BCM_E_NONE;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t *)
                     sal_alloc(sizeof(bcm_mirror_destination_t),
                     "New mirror_dest");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_destination_create(unit, new_mirror_dest);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_mirror_destination_t(new_mirror_dest, mirror_dest);

    /* Deallocate memory*/
    sal_free(new_mirror_dest);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_mirror_destination_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_destination_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      mirror_dest_id - (IN) Mirrored destination ID to be removed.
 *      mirror_dest - (INOUT) (IN/OUT) Matching Mirrored destination descriptor.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6522_mirror_destination_t *mirror_dest)
{
    int rv = BCM_E_NONE;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t *)
                     sal_alloc(sizeof(bcm_mirror_destination_t),
                     "New mirror_dest");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_destination_get(unit, mirror_dest_id, new_mirror_dest);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_mirror_destination_t(new_mirror_dest, mirror_dest);

    /* Deallocate memory*/
    sal_free(new_mirror_dest);

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6522in_extender_port_t
 * Purpose:
 *      Convert the bcm_extender_port_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_extender_port_t(
    bcm_compat6522_extender_port_t *from,
    bcm_extender_port_t *to)
{
    bcm_extender_port_t_init(to);
    to->criteria                  = from->criteria;
    to->flags                     = from->flags;
    to->extender_port_id          = from->extender_port_id;
    to->port                      = from->port;
    to->extended_port_vid         = from->extended_port_vid;
    to->match_vlan                = from->match_vlan;
    to->match_inner_vlan          = from->match_inner_vlan;
    to->pcp_de_select             = from->pcp_de_select;
    to->qos_map_id                = from->qos_map_id;
    to->pcp                       = from->pcp;
    to->de                        = from->de;
    to->encap_id                  = from->encap_id;
    to->failover_id               = from->failover_id;
    to->ingress_failover_id       = from->ingress_failover_id;
    to->failover_mc_group         = from->failover_mc_group;
    to->inlif_counting_profile    = from->inlif_counting_profile;
    to->outlif_counting_profile   = from->outlif_counting_profile;
}

/*
 * Function:
 *      _bcm_compat6522out_extender_port_t
 * Purpose:
 *      Convert the bcm_extender_port_t datatype from SDK 6.5.23+ to
 *      <=6.5.22
 * Parameters:
 *      from        - (IN) The SDK 6.5.23+ version of the datatype
 *      to          - (OUT) The <=6.5.22 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522out_extender_port_t(
    bcm_extender_port_t *from,
    bcm_compat6522_extender_port_t *to)
{
    from->criteria                  = to->criteria;
    from->flags                     = to->flags;
    from->extender_port_id          = to->extender_port_id;
    from->port                      = to->port;
    from->extended_port_vid         = to->extended_port_vid;
    from->match_vlan                = to->match_vlan;
    from->match_inner_vlan          = to->match_inner_vlan;
    from->pcp_de_select             = to->pcp_de_select;
    from->qos_map_id                = to->qos_map_id;
    from->pcp                       = to->pcp;
    from->de                        = to->de;
    from->encap_id                  = to->encap_id;
    from->failover_id               = to->failover_id;
    from->ingress_failover_id       = to->ingress_failover_id;
    from->failover_mc_group         = to->failover_mc_group;
    from->inlif_counting_profile    = to->inlif_counting_profile;
    from->outlif_counting_profile   = to->outlif_counting_profile;
}

/*
 * Function:
 *      bcm_compat6522_extender_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_extender_port_add.
 * Parameters:
 *      unit - (IN) SOC unit Number
 *      extender_port - (IN/OUT) Extender port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_extender_port_add(
    int unit,
    bcm_compat6522_extender_port_t *extender_port)
{
    int rv = BCM_E_NONE;
    bcm_extender_port_t *new_extender_port = NULL;

    if (extender_port != NULL) {
        new_extender_port = (bcm_extender_port_t *)sal_alloc(sizeof(bcm_extender_port_t), "New extender_port");
        if (new_extender_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_extender_port_t(extender_port, new_extender_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_extender_port_add(unit, new_extender_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_extender_port_t(new_extender_port, extender_port);

    /* Deallocate memory */
    sal_free(new_extender_port);

    return rv;
}


/*
 * Function:
 *      bcm_compat6522_extender_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_extender_port_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      unit - (IN) SOC unit number
 *      extender_port - (IN/OUT) Extender port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_extender_port_get(
    int unit,
    bcm_compat6522_extender_port_t *extender_port)
{
    int rv = BCM_E_NONE;
    bcm_extender_port_t *new_extender_port = NULL;

    if (extender_port != NULL) {
        new_extender_port = (bcm_extender_port_t *)
                     sal_alloc(sizeof(bcm_extender_port_t),
                     "New extender_port");
        if (new_extender_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_extender_port_t(extender_port, new_extender_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_extender_port_get(unit, new_extender_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6522out_extender_port_t(new_extender_port, extender_port);

    /* Deallocate memory */
    sal_free(new_extender_port);

    return rv;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_compat6522in_vlan_translate_egress_key_config_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_key_config_t datatype from <=6.5.22 to
 *      SDK 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.22 version of the datatype
 *      to          - (OUT) The SDK 6.5.23+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6522in_vlan_translate_egress_key_config_t(
    bcm_compat6522_vlan_translate_egress_key_config_t *from,
    bcm_vlan_translate_egress_key_config_t *to)
{
    bcm_vlan_translate_egress_key_config_t_init(to);
    to->flags = from->flags;
    to->key_type = from->key_type;
    to->inner_vlan = from->inner_vlan;
    to->outer_vlan = from->outer_vlan;
    to->port = from->port;
    to->otag_preserve = from->otag_preserve;
    to->itag_preserve = from->itag_preserve;
}

/*
 * Function:
 *      bcm_compat6522_vlan_translate_egress_action_extended_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN)
 *      action - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_add(unit, new_key_config, action);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_vlan_translate_egress_action_extended_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_delete(unit, new_key_config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_vlan_translate_egress_action_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN)
 *      action - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_get(unit, new_key_config, action);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_vlan_translate_egress_flexctr_extended_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      config - (IN) Flex counter configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_vlan_translate_egress_flexctr_extended_attach(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_attach(unit, new_key_config, config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_vlan_translate_egress_flexctr_extended_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      config - (IN) Flex counter configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_vlan_translate_egress_flexctr_extended_detach(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_detach(unit, new_key_config, config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_vlan_translate_egress_flexctr_extended_detach_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_detach_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_vlan_translate_egress_flexctr_extended_detach_all(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_detach_all(unit, new_key_config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_vlan_translate_egress_flexctr_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      array_size - (IN) Size of allocated entries in config array.
 *      config_array - (OUT) Array of flex counter configurations.
 *      count - (OUT) Number of elements in config array. If array_size is 0, config array will be ignored, and count will be filled with the actual number.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_vlan_translate_egress_flexctr_extended_get(
    int unit,
    bcm_compat6522_vlan_translate_egress_key_config_t *key_config,
    int array_size,
    bcm_vlan_flexctr_config_t *config_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6522in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_get(unit, new_key_config, array_size, config_array, count);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6522in_bcm_port_resource_t
 * Purpose:
 *      Convert the bcm_port_resource_t datatype from <=6.5.22 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6522in_port_resource_t(
    bcm_compat6522_port_resource_t *from,
    bcm_port_resource_t *to)
{
    bcm_port_resource_t_init(to);
    to->flags = from->flags;
    to->port = from->port;
    to->physical_port = from->physical_port;
    to->speed = from->speed;
    to->lanes = from->lanes;
    to->encap = from->encap;
    to->fec_type = from->fec_type;
    to->phy_lane_config = from->phy_lane_config;
    to->link_training = from->link_training;
    to->roe_compression = from->roe_compression;
    to->link_training = from->link_training;
    to->num_subports = from->num_subports;
    to->line_card = from->line_card;
    to->base_flexe_instance = from->base_flexe_instance;
};
/*
 * Function:
 *      bcm_compat6522_port_resource_default_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      flags - (IN)Flags
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    int flags,
    bcm_compat6522_port_resource_t *match_array)
{
    int rv = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6522in_port_resource_t(match_array,
                    new_match_array);
    }

    rv = bcm_port_resource_default_get(unit, port, flags, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6522_port_resource_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_resource_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array)
{
    int rv = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6522in_port_resource_t(match_array,
                    new_match_array);
    }

    rv = bcm_port_resource_get(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_port_resource_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_resource_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array)
{
    int rv = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6522in_port_resource_t(match_array,
                    new_match_array);
    }

    rv = bcm_port_resource_set(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6522_port_resource_speed_set_
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array)
{
    int rv = 0 ;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6522in_port_resource_t(match_array,
                    new_match_array);
    }

    rv = bcm_port_resource_speed_set(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_port_resource_speed_get_
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6522_port_resource_t *match_array)
{
    int rv = 0 ;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6522in_port_resource_t(match_array,
                    new_match_array);
    }

    rv = bcm_port_resource_speed_get(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6522_port_resource_speed_config_validate
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_config_validate.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      match_array - (IN) Match array
 *      pbmp - (IN) port pbmp
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_speed_config_validate(
    int unit,
    bcm_compat6522_port_resource_t *match_array,
    bcm_pbmp_t *pbmp)
{
    int rv = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6522in_port_resource_t(match_array,
                    new_match_array);
    }

    rv = bcm_port_resource_speed_config_validate(unit,new_match_array,pbmp);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6522_port_resource_multi_set
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_multi_set(
    int unit,
    int nport,
    bcm_compat6522_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(nport * sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0 ; i < nport; i++) {
            _bcm_compat6522in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
        }
    }

    rv = bcm_port_resource_multi_set(unit, nport, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6522_port_resource_speed_multi_set
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_multi_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6522_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_compat6522_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(nport * sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0 ; i < nport; i++) {
            _bcm_compat6522in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
        }
    }

    rv = bcm_port_resource_speed_multi_set(unit, nport, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

#endif /* BCM_RPC_SUPPORT */
