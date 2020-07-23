/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    multicast.c
 * Purpose: Implements procedures needed by the feature of
 *          L3 interface-on-virtual port multicast encapsulation ID.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/types.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/qos.h>


typedef struct _td2_l3_vp_encap_id_s {
    int vp; /* The virtual port on which the L3 interface resides */
    int nh_index; /* The next hop index used by MMU replication */
    struct _td2_l3_vp_encap_id_s *next; /* Pointer to next element in
                                           linked list */
} _td2_l3_vp_encap_id_t;

typedef struct _td2_multicast_l3_vp_s {
    _td2_l3_vp_encap_id_t **list_array; /* Array of lists of encap IDs,
                                          indexed by L3 interface */
    int list_array_size; /* Number of lists in list_array */
} _td2_multicast_l3_vp_t;

STATIC _td2_multicast_l3_vp_t *_td2_multicast_l3_vp_info[BCM_MAX_NUM_UNITS];

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      _bcm_td2_multicast_l3_vp_reinit
 * Purpose:
 *      Recover software state for warm boot.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_td2_multicast_l3_vp_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int chunk_size, num_chunks;
    uint8 *egr_nh_buf = NULL;
    int chunk_index;
    int entry_index_min, entry_index_max;
    int i;
    uint32 *egr_nh_entry;
    int vp, intf;
    _td2_l3_vp_encap_id_t *l3_vp_encap_id;
    soc_field_t   entry_type_field;

    chunk_size = 1024;
    num_chunks = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) / chunk_size;
    if (soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) % chunk_size) {
        num_chunks++;
    }

    egr_nh_buf = soc_cm_salloc(unit, sizeof(egr_l3_next_hop_entry_t) *
            chunk_size, "EGR_L3_NEXT_HOP entry buffer");
    if (NULL == egr_nh_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    for (chunk_index = 0; chunk_index < num_chunks; chunk_index++) {

        /* Get a chunk of entries */
        entry_index_min = chunk_index * chunk_size;
        entry_index_max = entry_index_min + chunk_size - 1;
        if (entry_index_max > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) {
            entry_index_max = soc_mem_index_max(unit, EGR_L3_NEXT_HOPm);
        }
        rv = soc_mem_read_range(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                entry_index_min, entry_index_max, egr_nh_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        /* Read each entry of the chunk */
        for (i = 0; i < (entry_index_max - entry_index_min + 1); i++) {
            egr_nh_entry = soc_mem_table_idx_to_pointer(unit,
                    EGR_L3_NEXT_HOPm, uint32 *, egr_nh_buf, i);
            if (SOC_IS_TRIDENT3X(unit)) {
                entry_type_field = DATA_TYPEf;
            } else {
                entry_type_field = ENTRY_TYPEf;
            }
            if (7 != soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        entry_type_field)) {
                continue;
            }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
                if (1 != soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                            L3MC__NEXT_PTR_TYPEf)) {
                    continue;
                }
            } else
#endif
            if (1 != soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                        L3MC__DVP_VALIDf)) {
                continue;
            }

            vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__DVPf);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender) &&
                !_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                continue;
            }
            intf = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    L3MC__INTF_NUMf);

            /* Add the VP and the next hop index to the L3 interface's
             * linked list of encap IDs.
             */
            l3_vp_encap_id = sal_alloc(sizeof(_td2_l3_vp_encap_id_t),
                    "L3 interface-on-virtual port encap ID");
            if  (NULL == l3_vp_encap_id) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            l3_vp_encap_id->vp = vp;
            l3_vp_encap_id->nh_index = entry_index_min + i;
            l3_vp_encap_id->next = _td2_multicast_l3_vp_info[unit]->list_array[intf];
            _td2_multicast_l3_vp_info[unit]->list_array[intf] = l3_vp_encap_id;
        }
    }

cleanup:
    if (egr_nh_buf) {
        soc_cm_sfree(unit, egr_nh_buf);
    }

    if (BCM_FAILURE(rv)) {
        (void) bcm_td2_multicast_l3_vp_detach(unit);
    }

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_td2_multicast_l3_vp_detach
 * Purpose:
 *      Deallocate data structures for storing L3 interface-on-virtual port
 *      multicast encapsulation IDs.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_multicast_l3_vp_detach(int unit)
{
    int i;
    _td2_l3_vp_encap_id_t *curr_ptr;
    _td2_l3_vp_encap_id_t *next_ptr;

    if (_td2_multicast_l3_vp_info[unit]) {
        if (_td2_multicast_l3_vp_info[unit]->list_array) {
            for (i = 0; i < _td2_multicast_l3_vp_info[unit]->list_array_size;
                    i++) {
                /* Traverse linked list of encap IDs */
                curr_ptr = _td2_multicast_l3_vp_info[unit]->list_array[i];
                while (NULL != curr_ptr) {
                    BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0,
                                curr_ptr->nh_index));
                    next_ptr = curr_ptr->next;
                    sal_free(curr_ptr);
                    curr_ptr = next_ptr;
                }
                _td2_multicast_l3_vp_info[unit]->list_array[i] = NULL;
            }
            sal_free(_td2_multicast_l3_vp_info[unit]->list_array);
            _td2_multicast_l3_vp_info[unit]->list_array = NULL;
        }
        sal_free(_td2_multicast_l3_vp_info[unit]);
        _td2_multicast_l3_vp_info[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_multicast_l3_vp_init
 * Purpose:
 *      Initialize the data structures for storing L3 interface-on-virtual port
 *      multicast encapsulation IDs.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_multicast_l3_vp_init(int unit)
{
    int list_array_size;

    BCM_IF_ERROR_RETURN(bcm_td2_multicast_l3_vp_detach(unit));

    _td2_multicast_l3_vp_info[unit] = sal_alloc
        (sizeof(_td2_multicast_l3_vp_t), "Multicast L3 VP info");
    if (NULL == _td2_multicast_l3_vp_info[unit]) {
        (void) bcm_td2_multicast_l3_vp_init(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_td2_multicast_l3_vp_info[unit], 0,
            sizeof(_td2_multicast_l3_vp_t));

    list_array_size = soc_mem_index_count(unit, EGR_L3_INTFm);
    _td2_multicast_l3_vp_info[unit]->list_array = sal_alloc
        (sizeof(_td2_l3_vp_encap_id_t *) * list_array_size,
         "Encap ID list array");
    if (NULL == _td2_multicast_l3_vp_info[unit]->list_array) {
        (void) bcm_td2_multicast_l3_vp_init(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_td2_multicast_l3_vp_info[unit]->list_array, 0,
            sizeof(_td2_l3_vp_encap_id_t *) * list_array_size);
    _td2_multicast_l3_vp_info[unit]->list_array_size = list_array_size;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_multicast_l3_vp_reinit(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_multicast_l3_vp_encap_get
 * Purpose:
 *      Get the Encap ID for a L3 interface on a virtual port.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      group - (IN) Multicast group ID.
 *      port  - (IN) Virtual port GPORT ID.
 *      intf  - (IN) L3 interface ID.
 *      encap_id - (OUT) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_multicast_l3_vp_encap_get(int unit, bcm_multicast_t group,
                               bcm_gport_t port, bcm_if_t intf,
                               bcm_if_t *encap_id)
{
    int vp;
    _td2_l3_vp_encap_id_t *curr_ptr;
    _td2_l3_vp_encap_id_t *l3_vp_encap_id;
    bcm_l3_egress_t nh_info;
    uint32 nh_flags;
    int nh_index;
    egr_l3_next_hop_entry_t egr_nh;
    bcm_niv_port_t niv_port;
    bcm_extender_port_t extender_port;
    int etag_dot1p_mapping_ptr = 0;
    bcm_niv_egress_t niv_egress;
    int              rv                   = BCM_E_NONE;
    uint16           virtual_interface_id = 0;
    uint32           mc_flags             = 0;
    int              count;
    bcm_extender_egress_t extender_egress;
    uint16 extended_port_vid;
    uint8 pcp, de;
    uint32 pcp_de_select;
    int qos_map_id;

    if (NULL == _td2_multicast_l3_vp_info[unit]) {
        return BCM_E_INIT;
    }

    /* Check virtual port */
    if (BCM_GPORT_IS_VLAN_PORT(port)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_NIV_PORT(port)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(port);
        if(!_bcm_vp_used_get(unit, vp, _bcmVpTypeExtender)) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Search the linked list of L3 interface-on-virtual port encap IDs */
    curr_ptr = _td2_multicast_l3_vp_info[unit]->list_array[intf];
    while (NULL != curr_ptr) {
        if (curr_ptr->vp == vp) {
            *encap_id = curr_ptr->nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            return BCM_E_NONE;
        }
        curr_ptr = curr_ptr->next;
    }

    /* Get a free next hop index */
    bcm_l3_egress_t_init(&nh_info);
    nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
    BCM_IF_ERROR_RETURN(bcm_xgs3_nh_add(unit, nh_flags, &nh_info, &nh_index));

    /* Write EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, DATA_TYPEf, 7);
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
    {
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, ENTRY_TYPEf, 7);
    }
    soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__INTF_NUMf, intf);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh,
                L3MC__NEXT_PTR_TYPEf, 1);
    } else
#endif
    {
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__DVP_VALIDf, 1);
    }
    soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__DVPf, vp);
    if (BCM_GPORT_IS_NIV_PORT(port)) {
        niv_port.niv_port_id = port;
        BCM_IF_ERROR_RETURN(bcm_esw_niv_port_get(unit, &niv_port));
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            bcm_niv_egress_t_init(&niv_egress);
            rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                            1, &niv_egress, &count);
            if(BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            if (niv_egress.flags & BCM_NIV_EGRESS_MULTICAST) {
                return BCM_E_PARAM;
            } else {
                virtual_interface_id = niv_egress.virtual_interface_id;
                mc_flags             = 0;
            }
        } else {
            virtual_interface_id = niv_port.virtual_interface_id;
            mc_flags             = niv_port.flags & BCM_NIV_PORT_MULTICAST;
        }
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__VNTAG_DST_VIFf,
                virtual_interface_id);
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__VNTAG_Pf,
                mc_flags ? 1 : 0);
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__VNTAG_ACTIONSf,
                1);
    } else if (BCM_GPORT_IS_EXTENDER_PORT(port)) {
        extender_port.extender_port_id = port;
        BCM_IF_ERROR_RETURN(bcm_esw_extender_port_get(unit, &extender_port));
        if (extender_port.flags & BCM_EXTENDER_PORT_MATCH_NONE) {
            bcm_extender_egress_t_init(&extender_egress);
            BCM_IF_ERROR_RETURN(bcm_esw_extender_egress_get_all(unit,
                        extender_port.extender_port_id, 1, &extender_egress, &count));
            if (count == 0) {
                /* No Extender egress object has been added to VP yet. */
                return BCM_E_CONFIG;
            }
            if (extender_egress.flags & BCM_EXTENDER_EGRESS_MULTICAST) {
                return BCM_E_PARAM;
            }
            qos_map_id        = extender_egress.qos_map_id;
            extended_port_vid = extender_egress.extended_port_vid;
            pcp_de_select     = extender_egress.pcp_de_select;
            pcp               = extender_egress.pcp;
            de                = extender_egress.de;
        } else {
            qos_map_id        = extender_port.qos_map_id;
            extended_port_vid = extender_port.extended_port_vid;
            pcp_de_select     = extender_port.pcp_de_select;
            pcp               = extender_port.pcp;
            de                = extender_port.de;
        }

        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__VNTAG_DST_VIFf,
                extended_port_vid);
        soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh, L3MC__VNTAG_ACTIONSf,
                2);
        if (pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_DEFAULT) {
            soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh,
                    L3MC__ETAG_PCP_DE_SOURCEf, 2);
            soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh,
                    L3MC__ETAG_PCPf, pcp);
            soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh,
                    L3MC__ETAG_DEf, de);
        } else if (pcp_de_select == BCM_EXTENDER_PCP_DE_SELECT_PHB) {
            soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh,
                    L3MC__ETAG_PCP_DE_SOURCEf, 3);
            bcm_td2_qos_egr_etag_id2profile(unit, qos_map_id,
                                                    &etag_dot1p_mapping_ptr);
            soc_EGR_L3_NEXT_HOPm_field32_set(unit, &egr_nh,
                    L3MC__ETAG_DOT1P_MAPPING_PTRf, etag_dot1p_mapping_ptr);
        } else {
            return BCM_E_INTERNAL;
        }
    }
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Add the next hop index to the linked list of L3 interface-on-virtual
     * port encap IDs.
     */
    l3_vp_encap_id = sal_alloc(sizeof(_td2_l3_vp_encap_id_t),
            "L3 interface-on-virtual port encap ID");
    if  (NULL == l3_vp_encap_id) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);
        return BCM_E_MEMORY;
    }
    l3_vp_encap_id->vp = vp;
    l3_vp_encap_id->nh_index = nh_index;
    l3_vp_encap_id->next = _td2_multicast_l3_vp_info[unit]->list_array[intf];
    _td2_multicast_l3_vp_info[unit]->list_array[intf] = l3_vp_encap_id;

    /* Form encap id */
    *encap_id = nh_index + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_multicast_l3_vp_next_hop_free
 * Purpose:
 *      Free the next hop indices allocated for a given L3 interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) L3 interface.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_td2_multicast_l3_vp_next_hop_free(int unit, int intf)
{
    _td2_l3_vp_encap_id_t *curr_ptr;
    _td2_l3_vp_encap_id_t *next_ptr;
    egr_l3_next_hop_entry_t egr_nh;
    int intf_num, vp_valid, vp;
    soc_field_t entry_type_field;

    if (NULL == _td2_multicast_l3_vp_info[unit]) {
        /* Nothing to free */
        return BCM_E_NONE;
    }

    if (NULL == _td2_multicast_l3_vp_info[unit]->list_array[intf]) {
        /* Nothing to free */
        return BCM_E_NONE;
    }

    /* Traverse the L3 interface's linked list of intf-on-vp encap IDs */
    curr_ptr = _td2_multicast_l3_vp_info[unit]->list_array[intf];
    while (NULL != curr_ptr) {
        /* Ascertaining the egress next hop entry has the right
         * entry type, L3 interface, and virtual port values
         * before clearing it. This is necessary in the following
         * scenario:
         * (1) User calls bcm_l3_egress_create to create a multicast L3
         *     egress object on a virtual port.
         * (2) User does a warm boot. The _bcm_td2_multicast_l3_vp_reinit
         *     procedure wil recover the multicast L3 egress object and
         *     insert it into the _td2_multicast_l3_vp_info[unit]->
         *     list_array[intf] linked list.
         * (3) User calls bcm_l3_egress_destroy to destroy the multicast L3
         *     egress object.
         * (4) The next hop index freed by step (3) is being re-allocated
         *     to some other application.
         * (5) User calls bcm_l3_intf_delete, which will trigger
         *     bcm_td2_multicast_l3_vp_next_hop_free procedure. This procedure
         *     will find that the egress next hop entry does not contain
         *     the expected data, due to step (4). Hence, bcm_xgs3_nh_del
         *     will not be called. Nevertheless, the element will still be
         *     removed from the linked list.
         */
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                    curr_ptr->nh_index, &egr_nh));
        if (SOC_IS_TRIDENT3X(unit)) {
            entry_type_field = DATA_TYPEf;
        } else {
            entry_type_field = ENTRY_TYPEf;
        }
        if (7 == soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    entry_type_field)) {
            intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    L3MC__INTF_NUMf);

            vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    L3MC__DVPf);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_l3_next_hop_next_ptr)) {
                vp_valid = (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                        L3MC__NEXT_PTR_TYPEf) & 0x1);
            } else
#endif
            {
                vp_valid = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                        L3MC__DVP_VALIDf);
            }

            if ((intf_num == intf) &&
                vp_valid &&
                (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv) ||
                 _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender) || 
                 _bcm_vp_used_get(unit, vp, _bcmVpTypeVlan))) {
                BCM_IF_ERROR_RETURN
                    (bcm_xgs3_nh_del(unit, 0, curr_ptr->nh_index));
            }
        }

        /* Remove the element from the linked list */
        next_ptr = curr_ptr->next;
        sal_free(curr_ptr);
        curr_ptr = next_ptr;
    }
    _td2_multicast_l3_vp_info[unit]->list_array[intf] = NULL;

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP

/*
 * Function:
 *     bcm_td2_multicast_l3_vp_sw_dump
 * Purpose:
 *     Displays information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_td2_multicast_l3_vp_sw_dump(int unit)
{
    int i, j, num_items_per_line;
    _td2_l3_vp_encap_id_t *curr_ptr;

    LOG_CLI((BSL_META_U(unit,
                        "L3 Interface-on-Virtual Port Encapsulation ID Information:\n")));

    num_items_per_line = 4;
    for (i = 0; i < _td2_multicast_l3_vp_info[unit]->list_array_size; i++) {
        if (_td2_multicast_l3_vp_info[unit]->list_array[i] != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  L3 interface %4d: virtual_port:next_hop_index"), i));
            curr_ptr = _td2_multicast_l3_vp_info[unit]->list_array[i];
            j = 0;
            while (curr_ptr != NULL) {
                if (j % num_items_per_line == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n                    ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %5d:%-5d"), curr_ptr->vp, curr_ptr->nh_index));
                j++;
                curr_ptr = curr_ptr->next;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return;
}

#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
