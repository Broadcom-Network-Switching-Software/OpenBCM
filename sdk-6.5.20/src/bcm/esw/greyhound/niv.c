/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    niv.c
 * Purpose: Implements bcm_niv_forward_* APIs for Greyhound.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_GREYHOUND_SUPPORT) && defined(INCLUDE_L3)

#include <soc/mem.h>

#include <bcm/error.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/greyhound.h>

/*
 * Purpose:
 *	Create NIV Forwarding table entry
 * Parameters:
 *	unit - (IN) Device Number
 *	iv_fwd_entry - (IN) NIV Forwarding table entry
 */
int
bcm_gh_niv_forward_add(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    l2x_entry_t l2x_entry;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                    iv_fwd_entry->dest_port,
                    &mod_out, &port_out, &tgid_out, &id_out));
        if (-1 != id_out) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             GH_L2_HASH_KEY_TYPE_VIF); /* VIF */
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
        if (-1 != tgid_out) {
            BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, tgid_out));
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DEST_TYPEf, 1);
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__TGIDf, tgid_out);
        } else {
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__MODULE_IDf, mod_out);
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__PORT_NUMf, port_out);
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_BITf, 1);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MULTICAST_IS_L2(iv_fwd_entry->dest_multicast)) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             GH_L2_HASH_KEY_TYPE_VIF); /* VIF */
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__L2MC_PTRf,
                _BCM_MULTICAST_ID_GET(iv_fwd_entry->dest_multicast));
        soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_BITf, 1);
    }

    soc_mem_lock(unit, L2Xm);

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_REPLACE)) {
        l2x_entry_t  l2x_lookup;
        int l2_index;

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
 *	Delete NIV Forwarding table entry
 * Parameters:
 *      unit - (IN) Device Number
 *      iv_fwd_entry - (IN) NIV Forwarding table entry
 */
int
bcm_gh_niv_forward_delete(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             GH_L2_HASH_KEY_TYPE_VIF); /* VIF */
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf, 
                             GH_L2_HASH_KEY_TYPE_VIF); /* VIF */
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
    }

    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

int
_bcm_gh_niv_forward_delete_all(int unit, bcm_niv_forward_t *iv_fwd_entry, void *data)
{
    BCM_IF_ERROR_RETURN(bcm_gh_niv_forward_delete(unit, iv_fwd_entry));

	return BCM_E_NONE;
}

/*
 * Purpose:
 *	Delete all NIV Forwarding table entries
 * Parameters:
 *	unit - Device Number
 */
int
bcm_gh_niv_forward_delete_all(int unit)
{
    int rv = BCM_E_NONE;
#if 1 /* TBD */	
    BCM_IF_ERROR_RETURN(bcm_gh_niv_forward_traverse(unit, 
            _bcm_gh_niv_forward_delete_all, NULL));

#else
    int seconds, enabled;
    l2_entry_1_entry_t match_mask;
    l2_entry_1_entry_t match_data;
    l2_bulk_entry_t l2_bulk;
    int match_mask_index, match_data_index;
    uint32 rval;
    int field_len;

    SOC_IF_ERROR_RETURN
        (SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled));
    if (enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
    }

    soc_mem_lock(unit, L2_ENTRY_1m);

    /* Set match mask and data */
    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));

    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_mask, VALIDf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_data, VALIDf, 1);

    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_mask, WIDEf, 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_data, WIDEf, 0);

    field_len = soc_mem_field_length(unit, L2_ENTRY_1m, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_mask, KEY_TYPEf,
            (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_ENTRY_1m, &match_data, KEY_TYPEf,
            SOC_MEM_KEY_L2_ENTRY_1_VIF_VIF);

    sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
    sal_memcpy(&l2_bulk, &match_mask, sizeof(match_mask));
    match_mask_index = 1;
    rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, match_mask_index, &l2_bulk);
    if (BCM_SUCCESS(rv)) {
        sal_memset(&l2_bulk, 0, sizeof(l2_bulk));
        sal_memcpy(&l2_bulk, &match_data, sizeof(match_data));
        match_data_index = 0;
        rv = WRITE_L2_BULKm(unit, MEM_BLOCK_ALL, match_data_index, &l2_bulk);
    }

    /* Set bulk control */
    rval = 0;
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, L2_MOD_FIFO_RECORDf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ACTIONf, 1);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, BURST_ENTRIESf, 
                      _SOC_TR3_L2_BULK_BURST_MAX);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, ENTRY_WIDTHf, 0);
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, NUM_ENTRIESf, 
                      soc_mem_index_count(unit, L2_ENTRY_1m));
    soc_reg_field_set(unit, L2_BULK_CONTROLr, &rval, EXTERNAL_L2_ENTRYf, 0);
    if (BCM_SUCCESS(rv)) {
        rv = WRITE_L2_BULK_CONTROLr(unit, rval);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_tr3_l2_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    }

    soc_mem_unlock(unit, L2_ENTRY_1m);

    if(enabled) {
        SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_start(unit, seconds));
    }
#endif
    return rv;
}

/*
 * Purpose:
 *      Get NIV Forwarding table entry
 * Parameters:
 *      unit - (IN) Device Number
 *      iv_fwd_entry - (IN/OUT) NIV forwarding table info
 */
int
bcm_gh_niv_forward_get(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry, l2x_entry_out;
    int idx;
    _bcm_gport_dest_t dest;
    int l2mc_index;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf, 
                             GH_L2_HASH_KEY_TYPE_VIF); /* VIF */
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf, 
                             GH_L2_HASH_KEY_TYPE_VIF); /* VIF */
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
    }

    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ALL, &idx, &l2x_entry,
            &l2x_entry_out, 0);
    soc_mem_unlock(unit, L2Xm);

    if (SOC_SUCCESS(rv)) {
        if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
            if (soc_L2Xm_field32_get(unit, &l2x_entry_out,
                        VIF__DEST_TYPEf)) {
                dest.tgid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                    VIF__TGIDf);
                dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                dest.modid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                        VIF__MODULE_IDf);
                dest.port = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                        VIF__PORT_NUMf);
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest,
                        &(iv_fwd_entry->dest_port)));
        } else {
            l2mc_index = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                    VIF__L2MC_PTRf);
            _BCM_MULTICAST_GROUP_SET(iv_fwd_entry->dest_multicast,
                    _BCM_MULTICAST_TYPE_L2, l2mc_index);
        }
    }

    return rv;
}

/*
 * Purpose:
 *      Traverse all valid NIV forward entries and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per NIV forward entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_gh_niv_forward_traverse(int unit,
                             bcm_niv_forward_traverse_cb cb,
                             void *user_data)
{
    int rv = BCM_E_NONE;
    int chunk_entries, chunk_bytes;
    uint8 *l2_tbl_chunk = NULL;
    int chunk_idx_min, chunk_idx_max, chunk_end;
    int ent_idx, mem_idx_min, mem_idx_max;
    l2x_entry_t *l2x_entry;
    bcm_niv_forward_t iv_fwd_entry;
    int l2mc_index;
    _bcm_gport_dest_t dest;

    chunk_entries = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);
    chunk_bytes = 4 * SOC_MEM_WORDS(unit, L2Xm) * chunk_entries;
    l2_tbl_chunk = soc_cm_salloc(unit, chunk_bytes, "niv forward traverse");
    if (NULL == l2_tbl_chunk) {
        return BCM_E_MEMORY;
    }

    mem_idx_min = soc_mem_index_min(unit, L2Xm);
    mem_idx_max = soc_mem_index_max(unit, L2Xm);
    for (chunk_idx_min = mem_idx_min; chunk_idx_min <= mem_idx_max; 
         chunk_idx_min += chunk_entries) {
        sal_memset(l2_tbl_chunk, 0, chunk_bytes);

        chunk_idx_max = chunk_idx_min + chunk_entries - 1;
        if (chunk_idx_max > mem_idx_max) {
            chunk_idx_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                                chunk_idx_min, chunk_idx_max, l2_tbl_chunk);
        if (SOC_FAILURE(rv)) {
            break;
        }

        chunk_end = (chunk_idx_max - chunk_idx_min);
        for (ent_idx = 0; ent_idx <= chunk_end; ent_idx++) {
            l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm,
                    l2x_entry_t *, l2_tbl_chunk, ent_idx);

            if (soc_L2Xm_field32_get(unit, l2x_entry, VALIDf) == 0) {
                continue;
            }

            if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) != 
                GH_L2_HASH_KEY_TYPE_VIF) { 
                continue;
            }

            bcm_niv_forward_t_init(&iv_fwd_entry);

            iv_fwd_entry.name_space = soc_L2Xm_field32_get(unit,
                    l2x_entry, VIF__NAMESPACEf);
            iv_fwd_entry.virtual_interface_id = soc_L2Xm_field32_get(unit,
                    l2x_entry, VIF__DST_VIFf);

            if (soc_L2Xm_field32_get(unit, l2x_entry, VIF__Pf)) {
                /* Multicast NIV forward entry */
                iv_fwd_entry.flags |= BCM_NIV_FORWARD_MULTICAST;
                l2mc_index = soc_L2Xm_field32_get(unit, l2x_entry,
                        VIF__L2MC_PTRf);
                _BCM_MULTICAST_GROUP_SET(iv_fwd_entry.dest_multicast,
                        _BCM_MULTICAST_TYPE_L2, l2mc_index);
            } else {
                if (soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__DEST_TYPEf)) {
                    dest.tgid = soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__TGIDf);
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                } else {
                    dest.modid = soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__MODULE_IDf);
                    dest.port = soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__PORT_NUMf);
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                }
                rv = _bcm_esw_gport_construct(unit, &dest,
                        &(iv_fwd_entry.dest_port));
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            rv = cb(unit, &iv_fwd_entry, user_data);
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


/*
 * Function:
 *      bcm_gh_niv_ethertype_set
 * Purpose:
 *      Set NIV Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (IN) NIV Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_gh_niv_ethertype_set(int unit, int ethertype)
{

    if (ethertype < 0 || ethertype > 0xffff) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                REG_PORT_ANY, ETHERTYPEf, ethertype));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPEr,
                                REG_PORT_ANY, ETHERTYPEf, ethertype));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPEr,
                                REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                REG_PORT_ANY, ETHERTYPEf, ethertype));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_gh_niv_ethertype_get
 * Purpose:
 *      Get NIV Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (OUT) NIV Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_gh_niv_ethertype_get(int unit, int *ethertype)
{
    uint32 rval;

    if (ethertype == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_NIV_ETHERTYPEr(unit, &rval));
    if (soc_reg_field_get(unit, NIV_ETHERTYPEr, rval, ENABLEf)) {
        *ethertype = soc_reg_field_get(unit, NIV_ETHERTYPEr, rval, ETHERTYPEf);
    } else {
        *ethertype = 0;
    }

    return BCM_E_NONE;
}

#endif /* BCM_GREYHOUND_SUPPORT && INCLUDE_L3 */
