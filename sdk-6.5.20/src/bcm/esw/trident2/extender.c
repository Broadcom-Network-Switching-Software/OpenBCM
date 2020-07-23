/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    extender.c
 * Purpose: Implements bcm_extender_forward_* APIs for Trident2.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <soc/katana2.h>

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_PE_SUPPORT)
#if defined(INCLUDE_L3)

#include <soc/mem.h>
#include <soc/ism_hash.h>

#include <bcm/error.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trident2.h>

/*
 * Purpose:
 *	Add a downstream forwarding entry
 * Parameters:
 *	unit - (IN) Device Number
 *	forward_entry - (IN) Forwarding entry
 */
int
bcm_td2_extender_forward_add(int unit, bcm_extender_forward_t *forward_entry)
{
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    l2x_entry_t l2x_entry;
    int lower_th, higher_th;

    if (forward_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastLowerThreshold,
                                   &lower_th));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastHigherThreshold, 
                                   &higher_th));

    if ((forward_entry->extended_port_vid >=
         (1 << soc_mem_field_length(unit, L2Xm, PE_VID__ETAG_VIDf))) ||
        (forward_entry->class_id >=
         (1 << soc_mem_field_length(unit, L2Xm, PE_VID__CLASS_IDf)))) {
        return BCM_E_PARAM;
    }

    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
        if ((forward_entry->extended_port_vid >= lower_th) &&
            (forward_entry->extended_port_vid <= higher_th)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                    forward_entry->dest_port,
                    &mod_out, &port_out, &tgid_out, &id_out));
        if (-1 != id_out) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry,
            soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);
#ifdef BCM_PE_SUPPORT
        if (soc_feature(unit, soc_feature_port_extension_kt2_key_type)) {
            soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                    KT2_L2_HASH_KEY_TYPE_PE_VID);
        } else
#endif /* BCM_PE_SUPPORT */
        {
            soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                    TR_L2_HASH_KEY_TYPE_PE_VID);
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__NAMESPACEf,
                forward_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__ETAG_VIDf,
                forward_entry->extended_port_vid);
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__CLASS_IDf,
                forward_entry->class_id);
        if (-1 != tgid_out) {
            BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, tgid_out));
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                    PE_VID__DESTINATIONf, SOC_MEM_FIF_DEST_LAG, tgid_out);
            } else {
                soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__DEST_TYPEf, 1);
                soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__TGIDf, tgid_out);
            }
        } else {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                    PE_VID__DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                    mod_out << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | port_out);
            } else {
                soc_L2Xm_field32_set(unit, &l2x_entry,
                                     PE_VID__MODULE_IDf, mod_out);
                soc_L2Xm_field32_set(unit, &l2x_entry,
                                     PE_VID__PORT_NUMf, port_out);
            }
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__STATIC_BITf, 1);
#ifdef BCM_HURRICANE4_SUPPORT
        if (soc_feature(unit, soc_feature_hr4_l2x_static_bit_war)) {
            soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__STATIC_DUPLICATEf, 1);
        }
#endif /* BCM_HURRICANE4_SUPPORT */

    } else { /* Multicast forward entry */
        if ((forward_entry->extended_port_vid < lower_th) ||
               (forward_entry->extended_port_vid > higher_th)) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MULTICAST_IS_L2(forward_entry->dest_multicast)) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry,
            soc_feature(unit, soc_feature_base_valid) ? BASE_VALIDf : VALIDf, 1);
#ifdef BCM_PE_SUPPORT
        if (soc_feature(unit, soc_feature_port_extension_kt2_key_type)) {
            soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                    KT2_L2_HASH_KEY_TYPE_PE_VID);
        } else
#endif /* BCM_PE_SUPPORT */
        {
            soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                    TR_L2_HASH_KEY_TYPE_PE_VID);
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__NAMESPACEf,
                forward_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__ETAG_VIDf,
                forward_entry->extended_port_vid);
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__CLASS_IDf,
                forward_entry->class_id);
        if (soc_feature(unit, soc_feature_generic_dest)) {
            soc_mem_field32_dest_set(unit, L2Xm, &l2x_entry,
                PE_VID__DESTINATIONf, SOC_MEM_FIF_DEST_L2MC,
                _BCM_MULTICAST_ID_GET(forward_entry->dest_multicast));
        } else {
            soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__L2MC_PTRf,
                    _BCM_MULTICAST_ID_GET(forward_entry->dest_multicast));
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__STATIC_BITf, 1);
#ifdef BCM_HURRICANE4_SUPPORT
        if (soc_feature(unit, soc_feature_hr4_l2x_static_bit_war)) {
            soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__STATIC_DUPLICATEf, 1);
        }
#endif /* BCM_HURRICANE4_SUPPORT */
    }

    if (forward_entry->flags & BCM_EXTENDER_FORWARD_COPY_TO_CPU) {
        if (SOC_MEM_FIELD_VALID(unit, L2Xm, PE_VID__DST_CPUf)) {
            soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__DST_CPUf, 1);
        } else if (SOC_MEM_FIELD_VALID(unit, L2Xm, PE_VID__CPUf)) {
            soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__CPUf, 1);
        }
    }

    soc_mem_lock(unit, L2Xm);

    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_REPLACE)) {
        l2x_entry_t  l2x_lookup;
        int          l2_index;

        /* See if the entry already exists */
        rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                    &l2x_lookup, &l2_index);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        if (BCM_SUCCESS(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return BCM_E_EXISTS;
        }

        rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    } else { /* Replace existing entry */
        rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    }

    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *	Delete a downstream forwarding entry
 * Parameters:
 *      unit - (IN) Device Number
 *      forward_entry - (IN) Forwarding entry
 */
int
bcm_td2_extender_forward_delete(int unit, bcm_extender_forward_t *forward_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry;
    int lower_th, higher_th;

    if (forward_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastLowerThreshold,
                                   &lower_th));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastHigherThreshold, 
                                   &higher_th));
    
    if (forward_entry->extended_port_vid >=
         (1 << soc_mem_field_length(unit, L2Xm, PE_VID__ETAG_VIDf))) {
        return BCM_E_PARAM;
    }

    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
        if ((forward_entry->extended_port_vid >= lower_th) &&
            (forward_entry->extended_port_vid <= higher_th)) {
            return BCM_E_PARAM;
        }
    } else { /* Multicast forward entry */
        if ((forward_entry->extended_port_vid < lower_th) ||
               (forward_entry->extended_port_vid > higher_th)) {
            return BCM_E_PARAM;
        }
    }

    sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
#ifdef BCM_PE_SUPPORT
    if (soc_feature(unit, soc_feature_port_extension_kt2_key_type)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                KT2_L2_HASH_KEY_TYPE_PE_VID);
    } else
#endif /* BCM_PE_SUPPORT */
    {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_PE_VID);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__NAMESPACEf,
            forward_entry->name_space);
    soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__ETAG_VIDf,
            forward_entry->extended_port_vid);
    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *	Delete all downstream forwarding entries
 * Parameters:
 *	unit - Device Number
 */
int
bcm_td2_extender_forward_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    int field_len;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_l2_bulk_unified_table)) {
        l2_bulk_entry_t match_mask, match_data;

        sal_memset(&match_mask, 0, sizeof(match_mask));
        sal_memset(&match_data, 0, sizeof(match_data));

        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, L2_BULKm, &match_mask, BASE_VALIDf, 1);
            soc_mem_field32_set(unit, L2_BULKm, &match_data, BASE_VALIDf, 1);
        } else {
            soc_mem_field32_set(unit, L2_BULKm, &match_mask, VALIDf, 1);
            soc_mem_field32_set(unit, L2_BULKm, &match_data, VALIDf, 1);
        }

        field_len = soc_mem_field_length(unit, L2_BULKm, KEY_TYPEf);
        soc_mem_field32_set(unit, L2_BULKm, &match_mask, KEY_TYPEf,
                            (1 << field_len) - 1);
        soc_mem_field32_set(unit, L2_BULKm, &match_data, KEY_TYPEf,
                            TR_L2_HASH_KEY_TYPE_PE_VID);
        soc_mem_lock(unit, L2Xm);
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, _BCM_L2_BULK_MATCH_MASK_INX,
                            &match_mask);
        if (BCM_SUCCESS(rv)) {
            rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL,
                                _BCM_L2_BULK_MATCH_DATA_INX,
                                &match_data);
        }
        if (BCM_SUCCESS(rv)) {
            rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                                        NUM_ENTRIESf, 
                                        soc_mem_index_count(unit, L2Xm));
        }
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
        l2_bulk_match_mask_entry_t match_mask;
        l2_bulk_match_data_entry_t match_data;

        sal_memset(&match_mask, 0, sizeof(match_mask));
        sal_memset(&match_data, 0, sizeof(match_data));

        soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, VALIDf, 1);
        soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, VALIDf, 1);

        field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, KEY_TYPEf);
        soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, KEY_TYPEf,
                            (1 << field_len) - 1);
#ifdef BCM_PE_SUPPORT
        if (soc_feature(unit, soc_feature_port_extension_kt2_key_type)) {
            soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, KEY_TYPEf,
                    KT2_L2_HASH_KEY_TYPE_PE_VID);
        } else
#endif /* BCM_PE_SUPPORT */
        {
            soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, KEY_TYPEf,
                    TR_L2_HASH_KEY_TYPE_PE_VID);
        }
       soc_mem_lock(unit, L2Xm);
        rv = WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0,
                                       &match_mask);
        if (BCM_SUCCESS(rv)) {
            rv = WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                                           &match_data);
        }
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                                    ACTIONf, 1);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    }
    soc_mem_unlock(unit, L2Xm);
    return rv;
}

/*
 * Purpose:
 *      Get a downstream forwarding entry
 * Parameters:
 *      unit - (IN) Device Number
 *      forward_entry - (IN/OUT) Forwarding entry
 */
int
bcm_td2_extender_forward_get(int unit, bcm_extender_forward_t *forward_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry, l2x_entry_out;
    int idx;
    _bcm_gport_dest_t dest;
    int l2mc_index;
    int lower_th, higher_th;

    if (forward_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastLowerThreshold,
                                   &lower_th));
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit,
                                   bcmSwitchExtenderMulticastHigherThreshold, 
                                   &higher_th));
   
    if (forward_entry->extended_port_vid >=
        (1 << soc_mem_field_length(unit, L2Xm, PE_VID__ETAG_VIDf))) {
        return BCM_E_PARAM;
    }

    if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
        if ((forward_entry->extended_port_vid >= lower_th) &&
            (forward_entry->extended_port_vid <= higher_th)) {
            return BCM_E_PARAM;
        }
    } else { /* Multicast forward entry */
        if ((forward_entry->extended_port_vid < lower_th) ||
               (forward_entry->extended_port_vid > higher_th)) {
            return BCM_E_PARAM;
        }
    }

    sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
#ifdef BCM_PE_SUPPORT
    if (soc_feature(unit, soc_feature_port_extension_kt2_key_type)) {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                KT2_L2_HASH_KEY_TYPE_PE_VID);
    } else
#endif /* BCM_PE_SUPPORT */
    {
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_PE_VID);
    }
    soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__NAMESPACEf,
            forward_entry->name_space);
    soc_L2Xm_field32_set(unit, &l2x_entry, PE_VID__ETAG_VIDf,
            forward_entry->extended_port_vid);
    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ALL, &idx, &l2x_entry,
            &l2x_entry_out, 0);
    soc_mem_unlock(unit, L2Xm);

    if (SOC_SUCCESS(rv)) {
        uint32 dv, dt = SOC_MEM_FIF_DEST_INVALID;
        if (!(forward_entry->flags & BCM_EXTENDER_FORWARD_MULTICAST)) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                dv = soc_mem_field32_dest_get(unit, L2Xm, &l2x_entry_out,
                                              PE_VID__DESTINATIONf, &dt);
                if (dt == SOC_MEM_FIF_DEST_LAG) {
                    dest.tgid = dv;
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                } else {
                    dest.port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                    dest.modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                    SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                }
            } else {
                if (soc_L2Xm_field32_get(unit, &l2x_entry_out,
                            PE_VID__DEST_TYPEf)) {
                    dest.tgid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                        PE_VID__TGIDf);
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                } else {
                    dest.modid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                            PE_VID__MODULE_IDf);
                    dest.port = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                            PE_VID__PORT_NUMf);
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                }
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest,
                        &(forward_entry->dest_port)));
        } else {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                dv = soc_mem_field32_dest_get(unit, L2Xm, &l2x_entry_out,
                                              PE_VID__DESTINATIONf, &dt);
                l2mc_index = (dt == SOC_MEM_FIF_DEST_L2MC) ? dv : 0;
            } else {
                l2mc_index = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                                                  PE_VID__L2MC_PTRf);
            }
            _BCM_MULTICAST_GROUP_SET(forward_entry->dest_multicast,
                    _BCM_MULTICAST_TYPE_L2, l2mc_index);
        }
        forward_entry->class_id = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                                                       PE_VID__CLASS_IDf);
    }

    return rv;
}

/*
 * Purpose:
 *      Traverse all valid downstream forwarding entries and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per forwarding entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_extender_forward_traverse(int unit,
                             bcm_extender_forward_traverse_cb cb,
                             void *user_data)
{
    int rv = BCM_E_NONE;
    int chunk_entries, chunk_bytes;
    uint8 *l2_tbl_chunk = NULL;
    int chunk_idx_min, chunk_idx_max;
    int ent_idx;
    l2x_entry_t *l2x_entry;
    bcm_extender_forward_t forward_entry;
    int l2mc_index;
    _bcm_gport_dest_t dest;
    int lower_th, higher_th;
    int key_val = 0;

    /* Get ETAG VID multicast range */
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchExtenderMulticastLowerThreshold, &lower_th));
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit,
                bcmSwitchExtenderMulticastHigherThreshold, &higher_th));

    chunk_entries = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);
    chunk_bytes = 4 * SOC_MEM_WORDS(unit, L2Xm) * chunk_entries;
    l2_tbl_chunk = soc_cm_salloc(unit, chunk_bytes, "extender forward traverse");
    if (NULL == l2_tbl_chunk) {
        return BCM_E_MEMORY;
    }
    for (chunk_idx_min = soc_mem_index_min(unit, L2Xm); 
         chunk_idx_min <= soc_mem_index_max(unit, L2Xm); 
         chunk_idx_min += chunk_entries) {

        /* Read a chunk of L2 table */
        sal_memset(l2_tbl_chunk, 0, chunk_bytes);
        chunk_idx_max = chunk_idx_min + chunk_entries - 1;
        if (chunk_idx_max > soc_mem_index_max(unit, L2Xm)) {
            chunk_idx_max = soc_mem_index_max(unit, L2Xm);
        }
        rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                                chunk_idx_min, chunk_idx_max, l2_tbl_chunk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        for (ent_idx = 0;
             ent_idx <= (chunk_idx_max - chunk_idx_min);
             ent_idx++) {
            uint32 dv, dt = SOC_MEM_FIF_DEST_INVALID;
            l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm, l2x_entry_t *,
                                             l2_tbl_chunk, ent_idx);
            if (soc_L2Xm_field32_get(unit, l2x_entry,
                                     soc_feature(unit, soc_feature_base_valid) ?
                                     BASE_VALIDf :
                                     VALIDf) == 0) {
                continue;
            }
#ifdef BCM_PE_SUPPORT
            if (soc_feature(unit, soc_feature_port_extension_kt2_key_type)) {
                key_val = (int)KT2_L2_HASH_KEY_TYPE_PE_VID;
            } else
#endif /* BCM_PE_SUPPORT */
            {
                key_val = (int)TR_L2_HASH_KEY_TYPE_PE_VID;
            }
            if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) != key_val) {
                continue;
            }

            /* Get fields of forwarding entry */
            bcm_extender_forward_t_init(&forward_entry);
            forward_entry.name_space = soc_L2Xm_field32_get(unit, l2x_entry,
                    PE_VID__NAMESPACEf);
            forward_entry.extended_port_vid = soc_L2Xm_field32_get(unit,
                    l2x_entry, PE_VID__ETAG_VIDf);
            forward_entry.class_id = soc_L2Xm_field32_get(unit,
                    l2x_entry, PE_VID__CLASS_IDf);
            if ((forward_entry.extended_port_vid >= lower_th) &&
                (forward_entry.extended_port_vid <= higher_th)) {
                /* Multicast forward entry */
                forward_entry.flags |= BCM_EXTENDER_FORWARD_MULTICAST;
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    dv = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry,
                                                  PE_VID__DESTINATIONf, &dt);
                    l2mc_index = (dt == SOC_MEM_FIF_DEST_L2MC) ? dv : 0;
                } else {
                    l2mc_index = soc_L2Xm_field32_get(unit, l2x_entry,
                                                      PE_VID__L2MC_PTRf);
                }
                _BCM_MULTICAST_GROUP_SET(forward_entry.dest_multicast,
                        _BCM_MULTICAST_TYPE_L2, l2mc_index);
            } else {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    dv = soc_mem_field32_dest_get(unit, L2Xm, l2x_entry,
                                                  PE_VID__DESTINATIONf, &dt);
                    if (dt == SOC_MEM_FIF_DEST_LAG) {
                        dest.tgid = dv;
                        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    } else {
                        dest.port = dv & SOC_MEM_FIF_DGPP_PORT_MASK;
                        dest.modid = (dv & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                                        SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    }
                } else {
                    if (soc_L2Xm_field32_get(unit, l2x_entry,
                                PE_VID__DEST_TYPEf)) {
                        dest.tgid = soc_L2Xm_field32_get(unit, l2x_entry,
                                PE_VID__TGIDf);
                        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                    } else {
                        dest.modid = soc_L2Xm_field32_get(unit, l2x_entry,
                                PE_VID__MODULE_IDf);
                        dest.port = soc_L2Xm_field32_get(unit, l2x_entry,
                                PE_VID__PORT_NUMf);
                        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    }
                }
                rv = _bcm_esw_gport_construct(unit, &dest,
                        &(forward_entry.dest_port));
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            if (SOC_MEM_FIELD_VALID(unit, L2Xm, PE_VID__DST_CPUf)) {
                if (soc_L2Xm_field32_get(unit, l2x_entry,
                            PE_VID__DST_CPUf)) {
                    forward_entry.flags |= BCM_EXTENDER_FORWARD_COPY_TO_CPU;
                }
            } else if (SOC_MEM_FIELD_VALID(unit, L2Xm, PE_VID__CPUf)) {
                if (soc_L2Xm_field32_get(unit, l2x_entry,
                            PE_VID__CPUf)) {
                    forward_entry.flags |= BCM_EXTENDER_FORWARD_COPY_TO_CPU;
                }
            }

            rv = cb(unit, &forward_entry, user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_tbl_chunk);

    return rv;
}
#endif
#endif /* BCM_TRIDENT2_SUPPORT || BCM_PE_SUPPORT && INCLUDE_L3 */
