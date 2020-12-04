/*
 * $Id: compat_6511.c,v 2.0 2017/10/23
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with <=sdk-6.5.11 routines
 */

#ifdef  BCM_RPC_SUPPORT

#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/mirror.h>
#include <bcm_int/compat/compat_6511.h>

/*
 *   Function:
 *        bcm_compat6511in_cosq_pfc_class_mapping_t 
 *    Purpose:
 *         Convert the bcm_cosq_pfc_class_mapping_t datatype from <=6.5.11 to
 *         SDK 6.5.12+
 *    Parameters:
 *          from        - (IN) The <=6.5.11 version of the datatype
 *          to          - (OUT) The SDK 6.5.12+ version of the datatype
 *          Returns:
 *                        Nothing
 */
    STATIC void
bcm_compat6511in_cosq_pfc_class_mapping_t(bcm_compat6511_cosq_pfc_class_mapping_t *from,
        bcm_cosq_pfc_class_mapping_t *to)
{
    int index = 0;
    bcm_cosq_pfc_class_mapping_t_init(to);
    to->class_id = from->class_id;
    for (index = 0; index < BCM_COMPAT6511_COSQ_PFC_GPORT_COUNT; index++) {
        to->gport_list[index] = from->gport_list[index];
    }
}

/*
 *   Function:
 *         bcm_compat6511out_cosq_pfc_class_mapping_t 
 *    Purpose:
 *         Convert bcm_cosq_pfc_class_mapping_t datatype from 6.5.12+ to
 *         <=6.5.11
 *    Parameters:
 *         from        - (IN) The SDK 6.5.12+ version of the datatype
 *         to          - (OUT) The <=6.5.11 version of the datatype
 *    Returns:
 *         Nothing
 */

    STATIC void
bcm_compat6511out_cosq_pfc_class_mapping_t(bcm_cosq_pfc_class_mapping_t *from,
        bcm_compat6511_cosq_pfc_class_mapping_t *to)
{
    int index = 0;
    to->class_id = from->class_id;
    for (index = 0; index < BCM_COMPAT6511_COSQ_PFC_GPORT_COUNT; index++) {
        to->gport_list[index] = from->gport_list[index];
    }
}

/*
 *   Function: bcm_compat6511_cosq_pfc_class_mapping_set 
 *   
 *   Purpose:
 *       Compatibility function for RPC call to bcm_cosq_pfc_class_mapping_set.
 *  
 *   Parameters:
 *           unit - (IN) BCM device number.
 *           port - (IN) Port number.
 *           array_count - (IN) Number of mappings in mapping_array.
 *           mapping_array - (IN/OUT) Array of PFC class mappings with old format
 *       
 *        Returns:
 *            BCM_E_XXX
 */
    int
bcm_compat6511_cosq_pfc_class_mapping_set(int unit, bcm_gport_t port, int array_count, 
        bcm_compat6511_cosq_pfc_class_mapping_t *mapping_array)
{
    int rv;
    bcm_cosq_pfc_class_mapping_t  *new_mapping_array;
    int index;

    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }
    /* Create from heap to avoid the stack to bloat */
    new_mapping_array = (bcm_cosq_pfc_class_mapping_t*)
        sal_alloc(array_count * sizeof(bcm_cosq_pfc_class_mapping_t), "new_mapping_array");
    if (NULL == new_mapping_array) {
        return BCM_E_MEMORY;
    }

    for(index = 0; index < array_count; index++) {
        /* Transform the mapping from the old format to new one */
        bcm_compat6511in_cosq_pfc_class_mapping_t( &mapping_array[index],
                &new_mapping_array[index]);
    }
    /* Call the BCM API with new format */
    rv = bcm_cosq_pfc_class_mapping_set(unit, port, array_count, new_mapping_array);

    for (index = 0; index < array_count; index++) {
        /* Transform the mapping from the new format to old one */
        bcm_compat6511out_cosq_pfc_class_mapping_t(&new_mapping_array[index],
                &mapping_array[index]);
    }

    /* Deallocate memory*/
    sal_free(new_mapping_array);

    return rv;
}


/*
 *   Function: bcm_compat6511_cosq_pfc_class_mapping_get 
 *   
 *   Purpose:
 *      Compatibility function for RPC call to bcm_cosq_pfc_class_mapping_set.
 *      
 *      Parameters:
 *         unit - (IN) BCM device number.
 *         port - (IN) Port number.
 *         array_max - (IN) Number of mappings allocated in mapping_array.
 *         mapping_array - (IN/OUT) Array of PFC class mappings with old format.
 *         array_count - (OUT) Number of mappings returned.
 *         
 *      Returns:
 *         BCM_E_XXX
 */
    int
bcm_compat6511_cosq_pfc_class_mapping_get(int unit, bcm_gport_t port, int array_max, 
        bcm_compat6511_cosq_pfc_class_mapping_t *mapping_array, int *array_count)
{
    int rv;
    int index;
    bcm_cosq_pfc_class_mapping_t  *new_mapping_array = NULL;

    if ((mapping_array == NULL) && (array_max > 0)) {
        return BCM_E_PARAM;
    }

    if (array_max > 0) {
        /* Create from heap to avoid the stack to bloat */
        new_mapping_array = (bcm_cosq_pfc_class_mapping_t*)
            sal_alloc(array_max * sizeof(bcm_cosq_pfc_class_mapping_t), "new_mapping_array");
        if (NULL == new_mapping_array) {
            return BCM_E_MEMORY;
        }

        for(index = 0; index < array_max; index++) {
            /* Transform the mapping from the old format to new one */
            bcm_compat6511in_cosq_pfc_class_mapping_t(&mapping_array[index],
                    &new_mapping_array[index]);
        }
        /* Call the BCM API with new format */
        rv = bcm_cosq_pfc_class_mapping_get(unit, port, array_max, new_mapping_array, array_count);
    } else {
        /* Call the BCM API with new format */
        rv = bcm_cosq_pfc_class_mapping_get(unit, port, array_max, NULL, array_count);
    }

    if (array_max > 0) {
        /* Transform the mapping from the new format to old one */
        for(index = 0; index < array_max; index++) {
            bcm_compat6511out_cosq_pfc_class_mapping_t(&new_mapping_array[index],
                    &mapping_array[index]);
        }
        /* Deallocate memory*/
        sal_free(new_mapping_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6511in_mirror_destination_t
 * Purpose:
 *      Convert the bcm_mirror_destination_t datatype from <=6.5.11 to
 *      SDK 6.5.12+
 * Parameters:
 *      from        - (IN) The <=6.5.11 version of the datatype
 *      to          - (OUT) The SDK 6.5.12+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6511in_mirror_destination_t(bcm_compat6511_mirror_destination_t
                                       *from,
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
    sal_memcpy(to->src6_addr, from->src6_addr, 16);
    sal_memcpy(to->dst6_addr, from->dst6_addr, 16);
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, 6);
    sal_memcpy(to->dst_mac, from->dst_mac, 6);
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
}

/*
 * Function:
 *      _bcm_compat6511out_mirror_destination_t
 * Purpose:
 *      Convert the bcm_mirror_destination_t datatype from 6.5.12+ to
 *      <=6.5.11
 * Parameters:
 *      from        - (IN) The SDK 6.5.12+ version of the datatype
 *      to          - (OUT) The <=6.5.11 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6511out_mirror_destination_t(bcm_mirror_destination_t *from,
                                        bcm_compat6511_mirror_destination_t *to)
{
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    sal_memcpy(to->src6_addr, from->src6_addr, 16);
    sal_memcpy(to->dst6_addr, from->dst6_addr, 16);
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, 6);
    sal_memcpy(to->dst_mac, from->dst_mac, 6);
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
}

/*
 * Function: bcm_compat6511_mirror_destination_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_destination_create
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     mirror_dest - (IN/OUT) Mirrored destination and encapsulation.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6511_mirror_destination_create(int unit,
                                         bcm_compat6511_mirror_destination_t
                                         *mirror_dest)
{
    int rv;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (NULL != mirror_dest) {
        /* Create from heap to avoid the stack to bloat */
        new_mirror_dest = (bcm_mirror_destination_t *)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New Mirror Dst Entry");
        if (NULL == new_mirror_dest) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6511in_mirror_destination_t(mirror_dest,
                                               new_mirror_dest);
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_destination_create(unit, new_mirror_dest);

    if (NULL != new_mirror_dest) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6511out_mirror_destination_t(new_mirror_dest,
                                                mirror_dest);

        /* Deallocate memory */
        sal_free(new_mirror_dest);
    }

    return rv;
}

/*
 * Function: bcm_compat6511_mirror_destination_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_destination_get
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     mirror_dest_id - (IN) Mirrored destination ID.
 *     mirror_dest - (IN/OUT) Matching Mirrored destination descriptor.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6511_mirror_destination_get(int unit, bcm_gport_t mirror_dest_id,
                                      bcm_compat6511_mirror_destination_t
                                      *mirror_dest)
{
    int rv;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (NULL != mirror_dest) {
        /* Create from heap to avoid the stack to bloat */
        new_mirror_dest = (bcm_mirror_destination_t *)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New Mirror Dst Entry");
        if (NULL == new_mirror_dest) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6511in_mirror_destination_t(mirror_dest,
                                               new_mirror_dest);
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_destination_get(unit, mirror_dest_id, new_mirror_dest);

    if (NULL != new_mirror_dest) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6511out_mirror_destination_t(new_mirror_dest,
                                                mirror_dest);

        /* Deallocate memory */
        sal_free(new_mirror_dest);
    }

    return rv;
}
#endif  /* BCM_RPC_SUPPORT */
