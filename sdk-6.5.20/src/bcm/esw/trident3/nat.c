/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    nat.c
 * Purpose: Implement Trident3 specific Large Scale NAT APIs.
 */

#include <shared/bsl.h>
#include <bcm/error.h>
#include <soc/defs.h>
#include <soc/mem.h>

#if defined(INCLUDE_L3) 
#include <bcm/l3.h>
#include <bcm/nat.h>

#include <bcm_int/esw/nat.h>

#include <soc/trident3.h>

#if defined (BCM_TRIDENT3_SUPPORT)

STATIC soc_field_t sip_f[3] = {
    LS_NAT_L3UC_DNAT__INNER_IPV4_SIPf,
    LS_NAT_L3UC_SNAT__INNER_IPV4_SIPf,
    LS_NAT_L3MC__INNER_IPV4_SIPf};

STATIC soc_field_t dip_31_17_f[3] = {
    LS_NAT_L3UC_DNAT__INNER_IPV4_DIP_31_17f,
    LS_NAT_L3UC_SNAT__INNER_IPV4_DIP_31_17f,
    LS_NAT_L3MC__INNER_IPV4_DIP_31_17f};

STATIC soc_field_t dip_16_0_f[3] = {
    LS_NAT_L3UC_DNAT__INNER_IPV4_DIP_16_0f,
    LS_NAT_L3UC_SNAT__INNER_IPV4_DIP_16_0f,
    LS_NAT_L3MC__INNER_IPV4_DIP_16_0f};

STATIC soc_field_t l4src_f[3] = {
    LS_NAT_L3UC_DNAT__L4_SRC_PORTf,
    LS_NAT_L3UC_SNAT__L4_SRC_PORTf,
    LS_NAT_L3MC__L4_SRC_PORTf};

STATIC soc_field_t l4dst_f[3] = {
    LS_NAT_L3UC_DNAT__L4_DST_PORTf,
    LS_NAT_L3UC_SNAT__L4_DST_PORTf,
    LS_NAT_L3MC__L4_DST_PORTf};

STATIC soc_field_t edit_f[3] = {
    LS_NAT_L3UC_DNAT__EDIT_CTRL_IDf,
    LS_NAT_L3UC_SNAT__EDIT_CTRL_IDf,
    LS_NAT_L3MC__EDIT_CTRL_IDf};

STATIC soc_field_t key_type_v[3] = {
    BCM_TD3_L3UC_DNAT_KEY_TYPE,
    BCM_TD3_L3UC_SNAT_KEY_TYPE,
    BCM_TD3_L3MC_NAT_KEY_TYPE};

STATIC soc_field_t opaque_f[3] = {
    LS_NAT_L3UC_DNAT__LARGE_SCALE_NAT_DST_EDIT_INDEXf,
    LS_NAT_L3UC_SNAT__LARGE_SCALE_NAT_SRC_EDIT_INDEXf,
    LS_NAT_L3MC__LARGE_SCALE_NAT_DST_EDIT_INDEXf};

STATIC soc_field_t rw_type_f[3] = {
    L3MC_RW_TYPE_0_SELf,
    L3MC_RW_TYPE_1_SELf,
    L3MC_RW_TYPE_2_SELf};

STATIC soc_field_t da_update_f[3] = {
    L3MC_RW_TYPE_0_MAC_DA_UPDATEf,
    L3MC_RW_TYPE_1_MAC_DA_UPDATEf,
    L3MC_RW_TYPE_2_MAC_DA_UPDATEf};

int
_bcm_td3_large_nat_egress_key_set(
    int unit,
    soc_mem_t mem,
    bcm_l3_large_nat_egress_t *nat_info,
    int *nat_key,
    uint32 *entry)
{
    int rv = BCM_E_NONE;
    int nat_lookup;
    uint32 data_type;

    data_type = BCM_L3_NAT_DATA_TYPE(unit);

    if (soc_mem_field_valid(unit, mem, BASE_VALIDf)) {
        soc_mem_field32_set(unit, mem, entry, BASE_VALIDf, 1);
    } else {
        soc_mem_field32_set(unit, mem, entry, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, entry, BASE_VALID_1f, 7);
    }

    if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_MULTICAST)  {
        soc_mem_field32_set(unit, mem, entry,
             LS_NAT_L3MC__REPLICATION_IDf,
             nat_info->replication_id);
        nat_lookup = BCM_TD3_L3MC_FIELDS;
    }
    else if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_SNAT) {
        soc_mem_field32_set(unit, mem, entry,
             LS_NAT_L3UC_SNAT__EGR_L3_INTF_CLASS_IDf,
             nat_info->intf_class_id);
        nat_lookup = BCM_TD3_L3UC_SNAT_FIELDS;
    }
    else {
        nat_lookup = BCM_TD3_L3UC_DNAT_FIELDS;
        nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_DNAT;
    }

    soc_mem_field32_set(unit, mem, entry,
             opaque_f[nat_lookup], nat_info->nat_class_id);

    soc_mem_field32_set(unit, mem, entry, KEY_TYPEf,  key_type_v[nat_lookup]);
    soc_mem_field32_set(unit, mem, entry, DATA_TYPEf, data_type);
    *nat_key = nat_lookup;

    return rv;
}

STATIC int
_bcm_td3_nat_egress_ls_find_mc_edit_type(
    int unit,
    bcm_l3_large_nat_egress_t *nat_info,
    int *rw_type) {

    int num_rw_types;
    int i, exp_rw_type;
    uint32   reg_val;

    if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_DNAT &&
        nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_SNAT) {
        exp_rw_type = (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_NAPT) ? 6 : 3;
    }
    else if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_DNAT) {
        exp_rw_type = (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_NAPT) ? 4 : 1;
    }
    else if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_SNAT) {
        exp_rw_type = (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_NAPT) ? 5 : 2;
    }
    else {
        exp_rw_type = 0;
    }

   /* find IPMC RW register */
   READ_LARGE_SCALE_NAT_CONTROLr(unit, &reg_val);

   num_rw_types = sizeof(rw_type_f)/sizeof(rw_type_f[0]);

   for (i=0; i<num_rw_types; i++) {
       int field_val = soc_reg_field_get(unit,
                            LARGE_SCALE_NAT_CONTROLr, reg_val,
                            rw_type_f[i]);
       int da_update = soc_reg_field_get(unit,
                            LARGE_SCALE_NAT_CONTROLr, reg_val,
                            da_update_f[i]);
       if ((field_val == exp_rw_type) &&
           (da_update == ((nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_MACDA_UPDATE)!=0))) {
            break;
        }
   }

   if (i==num_rw_types) return BCM_E_CONFIG;
   /* found a matching type */
   *rw_type = i+1;

   return BCM_E_NONE;
}

STATIC int
_bcm_td3_large_nat_egress_entry_set(
    int unit,
    soc_mem_t mem,
    bcm_l3_large_nat_egress_t *nat_info,
    uint32 nat_key,
    uint32 *entry)
{
    int rv = BCM_E_NONE;
    int index = -1;
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    int new_entry_add = FALSE;
    int edit_type = 0x1;
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index, entry,
                        return_entry, 0);

    if (BCM_E_NONE == rv) {
        /* found memory entry */
        if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_REPLACE) {
            sal_memcpy(entry,return_entry,SOC_MAX_MEM_WORDS * 4);
        } else {
            return BCM_E_EXISTS;
        }
    } else if (BCM_E_NOT_FOUND == rv) {
        if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_REPLACE) {
            return BCM_E_NOT_FOUND;
        } else {
            new_entry_add = TRUE;
        }
    } else {
        return rv;
    }

    if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_MULTICAST) {
        BCM_IF_ERROR_RETURN(
            _bcm_td3_nat_egress_ls_find_mc_edit_type(unit, nat_info, &edit_type));
    }

    soc_mem_field32_set(unit, mem, entry, edit_f[nat_key],
        edit_type);

    soc_mem_field32_set(unit, mem, entry, sip_f[nat_key],
        nat_info->sip_addr);

    soc_mem_field32_set(unit, mem, entry, l4src_f[nat_key],
        nat_info->src_port);

    soc_mem_field32_set(unit, mem, entry, dip_16_0_f[nat_key],
        nat_info->dip_addr & 0x1ffff);

    soc_mem_field32_set(unit, mem, entry, dip_31_17_f[nat_key],
        nat_info->dip_addr>>17);

    soc_mem_field32_set(unit, mem, entry, l4dst_f[nat_key],
        nat_info->dst_port);

    if (new_entry_add) {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, entry);
    } else { /* replace action */
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    }

    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

int
_bcm_td3_large_nat_egress_add(
    int unit,
    bcm_l3_large_nat_egress_t *nat_info)
{
    soc_mem_t mem = EGR_VLAN_XLATE_2_DOUBLEm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int rv, nat_key;

    sal_memset(entry, 0, sizeof(entry));

    BCM_IF_ERROR_RETURN(
        _bcm_td3_large_nat_egress_key_set(unit,
             mem, nat_info, &nat_key, entry));

    soc_mem_lock(unit,mem);

    /* set the match object and attributes and write to memory */
    rv = _bcm_td3_large_nat_egress_entry_set(unit, mem,
                        nat_info, nat_key, entry);

    soc_mem_unlock(unit,mem);

    return rv;
}

STATIC void
_bcm_td3_nat_egress_ls_get_mc_edit_type(
    int unit,
    int rw_type,
    bcm_l3_large_nat_egress_t *nat_info)
{
    uint32 reg_val;
    uint32 field_val;

    if (rw_type >= 3) return;

    READ_LARGE_SCALE_NAT_CONTROLr(unit, &reg_val);

    if (soc_reg_field_get(unit,
        LARGE_SCALE_NAT_CONTROLr, reg_val, da_update_f[rw_type])) {
        nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_MACDA_UPDATE;
    }

    field_val = soc_reg_field_get(unit,
        LARGE_SCALE_NAT_CONTROLr, reg_val, rw_type_f[rw_type]);

    switch (field_val) {
        case 1:
            nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_DNAT;
            break;
        case 2:
            nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_SNAT;
            break;
        case 3:
            nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_DNAT |
                               BCM_L3_LARGE_NAT_EGRESS_SNAT;
            break;
        case 4:
            nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_DNAT |
                               BCM_L3_LARGE_NAT_EGRESS_NAPT;
            break;
        case 5:
            nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_SNAT |
                               BCM_L3_LARGE_NAT_EGRESS_NAPT;
            break;
        case 6:
            nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_DNAT |
                               BCM_L3_LARGE_NAT_EGRESS_SNAT |
                               BCM_L3_LARGE_NAT_EGRESS_NAPT;
            break;
        default:
            break;
   }
}

STATIC void
_bcm_td3_large_nat_egress_data_get(
    int unit,
    soc_mem_t mem,
    uint32* entry,
    uint32 nat_lookup,
    bcm_l3_large_nat_egress_t *nat_info)
{
    uint32 rw_type;
    uint32 edit_type;

    if (nat_info->flags & BCM_L3_LARGE_NAT_EGRESS_MULTICAST) {
        edit_type = soc_mem_field32_get(unit, mem, entry, edit_f[nat_lookup]);
        rw_type = edit_type - 1;

        (void) _bcm_td3_nat_egress_ls_get_mc_edit_type(
            unit, rw_type, nat_info);
    }

    nat_info->sip_addr = soc_mem_field32_get(unit, mem, entry,
        sip_f[nat_lookup]);

    nat_info->dip_addr = soc_mem_field32_get(unit, mem, entry,
        dip_16_0_f[nat_lookup]);

    nat_info->dip_addr |= soc_mem_field32_get(unit, mem, entry,
        dip_31_17_f[nat_lookup])<<17;

    nat_info->src_port = soc_mem_field32_get(unit, mem, entry,
        l4src_f[nat_lookup]);

    nat_info->dst_port = soc_mem_field32_get(unit, mem, entry,
        l4dst_f[nat_lookup]);
}
int
_bcm_td3_large_nat_egress_entry_get(
    int unit,
    soc_mem_t mem,
    uint32* entry,
    uint32 nat_lookup,
    bcm_l3_large_nat_egress_t *nat_info)
{
    int index = -1;
    uint32 return_entry[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
        entry, return_entry, 0));

    _bcm_td3_large_nat_egress_data_get(unit, mem, return_entry,
        nat_lookup, nat_info);

    return BCM_E_NONE;
}

int
_bcm_td3_evxlt_entry_valid(
    int unit,
    soc_mem_t mem,
    uint32 *entry,
    int *valid)
{
    uint32 base_valid_0 = 0;
    uint32 base_valid_1 = 0;
    uint32 base_valid = 0;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if ((NULL == entry) || (NULL == valid)) {
        return BCM_E_PARAM;
    }
    if (soc_mem_field_valid(unit, mem, BASE_VALID_0f)) {
        base_valid_0 = soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f);
    }
    if (soc_mem_field_valid(unit, mem, BASE_VALID_1f)) {
        base_valid_1 = soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f);
    }
    if (soc_mem_field_valid(unit, mem, BASE_VALIDf)) {
        base_valid = soc_mem_field32_get(unit, mem, entry, BASE_VALIDf);
    }

    if ((base_valid != 0x1) && ((base_valid_0 == 0) || (base_valid_1 == 0)) ) {
        *valid = FALSE;
    } else {
        *valid = TRUE;
    }
    return BCM_E_NONE;
}

STATIC
int _bcm_td3_large_nat_egress_entry_parse(
    int unit,
    soc_mem_t mem,
    uint32 *entry,
    bcm_l3_large_nat_egress_t *nat_info)
{
    uint32 key_type;
    uint32 nat_lookup;

    key_type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);

    if (key_type == BCM_TD3_L3MC_NAT_KEY_TYPE) {
        nat_info->replication_id = soc_mem_field32_get(unit,
                                       mem, entry, LS_NAT_L3MC__REPLICATION_IDf);
        nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_MULTICAST;
        nat_lookup = BCM_TD3_L3MC_FIELDS;
    }
    else if (key_type == BCM_TD3_L3UC_SNAT_KEY_TYPE) {
        nat_info->intf_class_id = soc_mem_field32_get(unit,
                                   mem, entry,
                                   LS_NAT_L3UC_SNAT__EGR_L3_INTF_CLASS_IDf);
        nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_SNAT;
        nat_lookup = BCM_TD3_L3UC_SNAT_FIELDS;
    }
    else if (key_type == BCM_TD3_L3UC_DNAT_KEY_TYPE) {
        nat_info->flags |= BCM_L3_LARGE_NAT_EGRESS_DNAT;
        nat_lookup = BCM_TD3_L3UC_DNAT_FIELDS;
    }
    else {
        return BCM_E_NOT_FOUND;
    }

    nat_info->nat_class_id = soc_mem_field32_get(unit,
                                mem, entry, opaque_f[nat_lookup]);

    _bcm_td3_large_nat_egress_data_get(unit,
                                mem, entry, nat_lookup, nat_info);

    return BCM_E_NONE;
}

int _bcm_td3_large_nat_egress_traverse(
    int unit,
    soc_mem_t mem,
    bcm_l3_large_nat_egress_traverse_cb cb,
    void *user_data)
{

    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of table we currently work on */
    uint32          *ls_tbl_chnk;
    uint32          *entry;
    int             valid, rv = BCM_E_NONE;
    bcm_l3_large_nat_egress_t nat_info;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = 256;

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    ls_tbl_chnk = soc_cm_salloc(unit, buf_size, "large scale nat traverse");
    if (NULL == ls_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem);
         chnk_idx <= mem_idx_max;
         chnk_idx += chunksize) {
        sal_memset((void *)ls_tbl_chnk, 0, buf_size);

        chnk_idx_max =
            ((chnk_idx + chunksize) <= mem_idx_max) ?
            chnk_idx + chunksize - 1: mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, ls_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            entry =
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             ls_tbl_chnk, ent_idx);
            rv = _bcm_td3_evxlt_entry_valid(unit, mem, entry, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (!valid) {
                continue;
            }

            bcm_l3_large_nat_egress_t_init(&nat_info);

            rv = _bcm_td3_large_nat_egress_entry_parse(unit, mem, entry, &nat_info);
            if (rv == BCM_E_NOT_FOUND) { /* not a large scale nat entry */
                continue;
            } else if (BCM_FAILURE(rv)) {
                break;
            }
            rv = cb(unit, &nat_info, user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }
    soc_cm_sfree(unit, ls_tbl_chnk);
    return rv;
}

#endif  /* BCM_TRIDENT3_SUPPORT */

#endif /* INCLUDE_L3 */
