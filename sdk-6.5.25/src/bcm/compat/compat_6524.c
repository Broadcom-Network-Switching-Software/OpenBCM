/*
* $Id: compat_6524.c,v 1.0 2021/10/21
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2021 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.24 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6524.h>




/*
 * Function:
 *      _bcm_compat6524in_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from <=6.5.24 to
 *      SDK 6.5.25+
 * Parameters:
 *      from        - (IN) The <=6.5.24 version of the datatype
 *      to          - (OUT) The SDK 6.5.25+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524in_mirror_pkt_dnx_pp_header_t(
    bcm_compat6524_mirror_pkt_dnx_pp_header_t *from,
    bcm_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->bytes_to_remove = from->bytes_to_remove;
    to->eth_header_remove = from->eth_header_remove;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
    to->vsi = from->vsi;
}

/*
 * Function:
 *      _bcm_compat6524out_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from SDK 6.5.25+ to
 *      <=6.5.24
 * Parameters:
 *      from        - (IN) The SDK 6.5.25+ version of the datatype
 *      to          - (OUT) The <=6.5.24 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524out_mirror_pkt_dnx_pp_header_t(
    bcm_mirror_pkt_dnx_pp_header_t *from,
    bcm_compat6524_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->bytes_to_remove = from->bytes_to_remove;
    to->eth_header_remove = from->eth_header_remove;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
    to->vsi = from->vsi;
}

/*
 * Function:
 *      _bcm_compat6524in_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from <=6.5.24 to
 *      SDK 6.5.25+
 * Parameters:
 *      from        - (IN) The <=6.5.24 version of the datatype
 *      to          - (OUT) The SDK 6.5.25+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524in_mirror_header_info_t(
    bcm_compat6524_mirror_header_info_t *from,
    bcm_mirror_header_info_t *to)
{
    int i1 = 0;

    bcm_mirror_header_info_t_init(to);
    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6524in_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      _bcm_compat6524out_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from SDK 6.5.25+ to
 *      <=6.5.24
 * Parameters:
 *      from        - (IN) The SDK 6.5.25+ version of the datatype
 *      to          - (OUT) The <=6.5.24 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524out_mirror_header_info_t(
    bcm_mirror_header_info_t *from,
    bcm_compat6524_mirror_header_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6524out_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      bcm_compat6524_mirror_header_info_set
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
int bcm_compat6524_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6524_mirror_header_info_t *mirror_header_info)
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
        _bcm_compat6524in_mirror_header_info_t(mirror_header_info, new_mirror_header_info);
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
 *      bcm_compat6524_mirror_header_info_get
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
int bcm_compat6524_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6524_mirror_header_info_t *mirror_header_info)
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
    _bcm_compat6524out_mirror_header_info_t(new_mirror_header_info, mirror_header_info);

    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6524out_port_fdr_stats_t
 * Purpose:
 *      Convert the bcm_port_fdr_stats_t datatype from SDK 6.5.25+ to
 *      <=6.5.24
 * Parameters:
 *      from        - (IN) The SDK 6.5.25+ version of the datatype
 *      to          - (OUT) The <=6.5.24 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524out_port_fdr_stats_t(
    bcm_port_fdr_stats_t *from,
    bcm_compat6524_port_fdr_stats_t *to)
{
    to->start_time = from->start_time;
    to->end_time = from->end_time;
    to->num_uncorrect_cws = COMPILER_64_LO(from->num_uncorrect_cws);
    to->num_cws = COMPILER_64_LO(from->num_cws);
    to->num_symb_errs = COMPILER_64_LO(from->num_symb_errs);
    to->cw_s0_errs = COMPILER_64_LO(from->cw_s0_errs);
    to->cw_s1_errs = COMPILER_64_LO(from->cw_s1_errs);
    to->cw_s2_errs = COMPILER_64_LO(from->cw_s2_errs);
    to->cw_s3_errs = COMPILER_64_LO(from->cw_s3_errs);
    to->cw_s4_errs = COMPILER_64_LO(from->cw_s4_errs);
    to->cw_s5_errs = COMPILER_64_LO(from->cw_s5_errs);
    to->cw_s6_errs = COMPILER_64_LO(from->cw_s6_errs);
    to->cw_s7_errs = COMPILER_64_LO(from->cw_s7_errs);
    to->cw_s8_errs = COMPILER_64_LO(from->cw_s8_errs);
}

/*
 * Function:
 *      bcm_compat6524_port_fdr_stats_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_fdr_stats_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Logical Port Number
 *      fdr_stats - (OUT) FDR statistics
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_port_fdr_stats_get(
    int unit,
    bcm_port_t port,
    bcm_compat6524_port_fdr_stats_t *fdr_stats)
{
    int rv = BCM_E_NONE;
    bcm_port_fdr_stats_t *new_fdr_stats = NULL;

    if (fdr_stats != NULL) {
        new_fdr_stats = (bcm_port_fdr_stats_t *)
                     sal_alloc(sizeof(bcm_port_fdr_stats_t),
                     "New fdr_stats");
        if (new_fdr_stats == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_fdr_stats_get(unit, port, new_fdr_stats);

    /* Transform the entry from the new format to old one */
    _bcm_compat6524out_port_fdr_stats_t(new_fdr_stats, fdr_stats);

    /* Deallocate memory*/
    sal_free(new_fdr_stats);

    return rv;
}

#if defined(INCLUDE_INT)
/*
 * Function:
 *   _bcm_compat6524in_int_metadata_field_entry_t
 * Purpose:
 *   Convert the bcm_int_metadata_field_entry_t datatype from <=6.5.24 to
 *   SDK 6.5.24+.
 * Parameters:
 *   from        - (IN)  The <=6.5.24 version of the datatype.
 *   to          - (OUT) The SDK 6.5.24+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6524in_int_metadata_field_entry_t(bcm_compat6524_int_metadata_field_entry_t *from,
                                             bcm_int_metadata_field_entry_t *to)
{
    bcm_int_metadata_field_entry_t_init(to);

    to->select.field = from->select.field;
    to->select.start = from->select.start;
    to->select.size  = from->select.size;
    to->select.shift = from->select.shift;

    to->construct.offset = from->construct.offset;
}

/*
 * Function:
 *   _bcm_compat6524out_int_metadata_field_entry_t
 * Purpose:
 *   Convert the bcm_int_metadata_field_entry_t datatype from 6.5.24+ to
 *   <=6.5.24.
 * Parameters:
 *   from     - (IN)  The 6.5.24+ version of the datatype.
 *   to       - (OUT) The <=6.5.24 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6524out_int_metadata_field_entry_t(bcm_int_metadata_field_entry_t *from,
                                              bcm_compat6524_int_metadata_field_entry_t *to)
{
    to->select.field = from->select.field;
    to->select.start = from->select.start;
    to->select.size  = from->select.size;
    to->select.shift = from->select.shift;

    to->construct.offset = from->construct.offset;
}

/*
 * Function:
 *      bcm_compat6524_int_metadata_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_int_metadata_profile_get
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      profile_id  - (IN)  Metadata profile ID
 *      array_size  - (IN)  Metadata field array size
 *      entry_array - (OUT) Metadata field array
 *      field_count - (OUT) Metadata field count
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_compat6524_int_metadata_profile_get(
    int unit,
    int profile_id,
    int array_size,
    bcm_compat6524_int_metadata_field_entry_t *entry_array,
    int *field_count)
{
    bcm_int_metadata_field_entry_t *new_entry_array = NULL;
    int rv;
    int i;

    if (array_size == 0) {
        return bcm_int_metadata_profile_get(unit, profile_id, array_size,
                                            NULL, field_count);
    }

    if (entry_array != NULL) {
        new_entry_array = (bcm_int_metadata_field_entry_t *)
            sal_alloc(sizeof(bcm_int_metadata_field_entry_t) * array_size,
                      "New entry array");
        if (new_entry_array == NULL) {
            return BCM_E_MEMORY;
        }
    }

    rv = bcm_int_metadata_profile_get(unit, profile_id, array_size,
                                      new_entry_array, field_count);
    for (i = 0; i < *field_count; i++) {
        _bcm_compat6524out_int_metadata_field_entry_t(&(new_entry_array[i]),
                                                      &(entry_array[i]));
    }

    sal_free(new_entry_array);
    return rv;
}

/*
 * Function:
 *      bcm_compat6524_int_metadata_field_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_int_metadata_field_add
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      profile_id  - (IN)  Metadata profile ID
 *      field_entry - (IN)  Metadata field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_int_metadata_field_add(
    int unit,
    int profile_id,
    bcm_compat6524_int_metadata_field_entry_t *field_entry)
{
    bcm_int_metadata_field_entry_t *new_field_entry = NULL;
    int rv;

    if (field_entry != NULL) {
        new_field_entry = (bcm_int_metadata_field_entry_t *)
            sal_alloc(sizeof(bcm_int_metadata_field_entry_t), "New field entry");

        if (new_field_entry == NULL) {
            return BCM_E_MEMORY;
        }
    }

    _bcm_compat6524in_int_metadata_field_entry_t(field_entry, new_field_entry);

    rv = bcm_int_metadata_field_add(unit, profile_id, new_field_entry);

    sal_free(new_field_entry);
    return rv;
}

/*
 * Function:
 *      bcm_compat6524_int_metadata_field_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_int_metadata_field_delete
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      profile_id  - (IN)  Metadata profile ID
 *      field_entry - (IN)  Metadata field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_int_metadata_field_delete(
    int unit,
    int profile_id,
    bcm_compat6524_int_metadata_field_entry_t *field_entry)
{
    bcm_int_metadata_field_entry_t *new_field_entry = NULL;
    int rv;

    if (field_entry != NULL) {
        new_field_entry = (bcm_int_metadata_field_entry_t *)
            sal_alloc(sizeof(bcm_int_metadata_field_entry_t), "New field entry");

        if (new_field_entry == NULL) {
            return BCM_E_MEMORY;
        }
    }

    _bcm_compat6524in_int_metadata_field_entry_t(field_entry, new_field_entry);

    rv = bcm_int_metadata_field_delete(unit, profile_id, new_field_entry);

    sal_free(new_field_entry);
    return rv;
}
#endif /* defined(INCLUDE_INT) */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6524in_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype from <=6.5.24 to
 *      SDK 6.5.25+
 * Parameters:
 *      from        - (IN) The <=6.5.24 version of the datatype
 *      to          - (OUT) The SDK 6.5.25+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524in_flow_tunnel_terminator_t(
    bcm_compat6524_flow_tunnel_terminator_t *from,
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
}

/*
 * Function:
 *      _bcm_compat6524out_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype from SDK 6.5.25+ to
 *      <=6.5.24
 * Parameters:
 *      from        - (IN) The SDK 6.5.25+ version of the datatype
 *      to          - (OUT) The <=6.5.24 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524out_flow_tunnel_terminator_t(
    bcm_flow_tunnel_terminator_t *from,
    bcm_compat6524_flow_tunnel_terminator_t *to)
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
}

/*
 * Function:
 *      bcm_compat6524_flow_tunnel_terminator_create
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
int bcm_compat6524_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_create(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6524out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6524_flow_tunnel_terminator_destroy
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
int bcm_compat6524_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_destroy(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6524out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6524_flow_tunnel_terminator_get
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
int bcm_compat6524_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6524out_flow_tunnel_terminator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6524_flow_tunnel_terminator_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6524_flow_tunnel_terminator_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_terminator_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Tunnel config info structure
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6524_flow_tunnel_terminator_stat_attach
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
int bcm_compat6524_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6524_flow_tunnel_terminator_stat_detach
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
int bcm_compat6524_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
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
 *      bcm_compat6524_flow_tunnel_terminator_stat_id_get
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
int bcm_compat6524_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
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
        _bcm_compat6524in_flow_tunnel_terminator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_terminator_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6524in_l3_vpbr_entry_t
 * Purpose:
 *      Convert the bcm_l3_vpbr_entry_t datatype from <=6.5.24 to
 *      SDK 6.5.25+
 * Parameters:
 *      from        - (IN) The <=6.5.24 version of the datatype
 *      to          - (OUT) The SDK 6.5.25+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524in_l3_vpbr_entry_t(
    bcm_compat6524_l3_vpbr_entry_t *from,
    bcm_l3_vpbr_entry_t *to)
{
    int i1 = 0;

    bcm_l3_vpbr_entry_t_init(to);
    to->flags = from->flags;
    to->priority = from->priority;
    to->vrf = from->vrf;
    to->vrf_mask = from->vrf_mask;
    to->sip_addr = from->sip_addr;
    to->sip_addr_mask = from->sip_addr_mask;
    to->dip_addr = from->dip_addr;
    to->dip_addr_mask = from->dip_addr_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_addr[i1] = from->sip6_addr[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_addr_mask[i1] = from->sip6_addr_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_addr[i1] = from->dip6_addr[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_addr_mask[i1] = from->dip6_addr_mask[i1];
    }
    to->dscp = from->dscp;
    to->dscp_mask = from->dscp_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->dst_port = from->dst_port;
    to->dst_port_mask = from->dst_port_mask;
    to->new_vrf = from->new_vrf;
    to->l3_intf_id = from->l3_intf_id;
    to->l3_intf_id_mask = from->l3_intf_id_mask;
    to->ip_protocol = from->ip_protocol;
    to->ip_protocol_mask = from->ip_protocol_mask;
}

/*
 * Function:
 *      _bcm_compat6524out_l3_vpbr_entry_t
 * Purpose:
 *      Convert the bcm_l3_vpbr_entry_t datatype from SDK 6.5.25+ to
 *      <=6.5.24
 * Parameters:
 *      from        - (IN) The SDK 6.5.25+ version of the datatype
 *      to          - (OUT) The <=6.5.24 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6524out_l3_vpbr_entry_t(
    bcm_l3_vpbr_entry_t *from,
    bcm_compat6524_l3_vpbr_entry_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->priority = from->priority;
    to->vrf = from->vrf;
    to->vrf_mask = from->vrf_mask;
    to->sip_addr = from->sip_addr;
    to->sip_addr_mask = from->sip_addr_mask;
    to->dip_addr = from->dip_addr;
    to->dip_addr_mask = from->dip_addr_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_addr[i1] = from->sip6_addr[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_addr_mask[i1] = from->sip6_addr_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_addr[i1] = from->dip6_addr[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_addr_mask[i1] = from->dip6_addr_mask[i1];
    }
    to->dscp = from->dscp;
    to->dscp_mask = from->dscp_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->dst_port = from->dst_port;
    to->dst_port_mask = from->dst_port_mask;
    to->new_vrf = from->new_vrf;
    to->l3_intf_id = from->l3_intf_id;
    to->l3_intf_id_mask = from->l3_intf_id_mask;
    to->ip_protocol = from->ip_protocol;
    to->ip_protocol_mask = from->ip_protocol_mask;
}

/*
 * Function:
 *      bcm_compat6524_l3_vpbr_entry_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_vpbr_entry_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_l3_vpbr_entry_delete(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_vpbr_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_vpbr_entry_t *)
                     sal_alloc(sizeof(bcm_l3_vpbr_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6524in_l3_vpbr_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_vpbr_entry_delete(unit, new_entry);


    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6524_l3_vpbr_entry_delete_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_vpbr_entry_delete_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_l3_vpbr_entry_delete_all(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_vpbr_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_vpbr_entry_t *)
                     sal_alloc(sizeof(bcm_l3_vpbr_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6524in_l3_vpbr_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_vpbr_entry_delete_all(unit, new_entry);


    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6524_l3_vpbr_entry_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_vpbr_entry_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_l3_vpbr_entry_get(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_vpbr_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_vpbr_entry_t *)
                     sal_alloc(sizeof(bcm_l3_vpbr_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6524in_l3_vpbr_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_vpbr_entry_get(unit, new_entry);

    /* Transform the entry from the new format to old one */
    _bcm_compat6524out_l3_vpbr_entry_t(new_entry, entry);

    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6524_l3_vpbr_entry_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_vpbr_entry_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6524_l3_vpbr_entry_add(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_vpbr_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_vpbr_entry_t *)
                     sal_alloc(sizeof(bcm_l3_vpbr_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6524in_l3_vpbr_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_vpbr_entry_add(unit, new_entry);


    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

#endif /* defined(INCLUDE_L3) */
#endif /* BCM_RPC_SUPPORT */
