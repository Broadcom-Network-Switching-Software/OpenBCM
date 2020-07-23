/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ecmp.c
 * Purpose:     Ecmp SOC functions.
 */


#include <soc/defs.h>

#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/esw/ecmp.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>

#define SOC_ESW_ECMP_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)

/* Default warmboot version */
#define SOC_ESW_ECMP_WB_DEFAULT_VERSION            SOC_ESW_ECMP_WB_VERSION_1_1


#define SOC_ESW_ECMP_WB_SIZE(_data_)  sizeof(*(_data_))


#define SOC_ESW_ECMP_WB_SYNC(_scache_, _data_)                \
    do {                                                      \
        sal_memcpy((_scache_), (_data_), sizeof(*(_data_)));  \
        (_scache_) +=  sizeof(*(_data_));                     \
    } while (0)

#define SOC_ESW_ECMP_WB_RECOVER(_scache_, _data_)             \
    do {                                                      \
        sal_memcpy((_data_), (_scache_), sizeof(*(_data_)));  \
        (_scache_) +=  sizeof(*(_data_));                     \
    } while (0)

#endif /* BCM_WARM_BOOT_SUPPORT */


/* SOC ESW ECMP State */
soc_esw_ecmp_state_t *soc_esw_ecmp_state[SOC_MAX_NUM_DEVICES];

static const soc_mem_t ecmp_member_mems[] = {
        INITIAL_L3_ECMPm,
        L3_ECMPm
    };

static const soc_mem_t ecmp_group_mems[] = {
        INITIAL_L3_ECMP_GROUPm,
        L3_ECMP_COUNTm
    };



STATIC void
soc_esw_nh_if_ecmp_grp_decode(int unit, uint32 *nh_idx, int *ecmpf)
{
    int ecmp_if_offset;

    ecmp_if_offset = SOC_ESW_ECMP_STATE(unit)->ecmp_grp_if_offset;

    if (*nh_idx >= ecmp_if_offset) {
        *ecmpf = 1;
        *nh_idx -= ecmp_if_offset;
    }

    return;
}

STATIC void
soc_esw_nh_if_ecmp_grp_encode(int unit, uint32 *nh_idx, int ecmpf)
{
    int ecmp_if_offset;

    ecmp_if_offset = SOC_ESW_ECMP_STATE(unit)->ecmp_grp_if_offset;

    if (ecmpf) {
        *nh_idx += ecmp_if_offset;
    }

    return;
}



/*
 * Function:
 *      soc_esw_mem_dma_read
 * Purpose:
 *
 * Parameters:
 *      unit         -(IN)SOC unit number.
 *      tbl_mem      -(IN)Table memory.
 *      tbl_entry_sz -(IN)Table entry size.
 *      descr        -(IN)Table descriptor.
 *      start        -(IN) Start index
 *      count        -(IN) Number of entries
 *      res_ptr      -(OUT) Allocated pointer filled with table info
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_mem_dma_read(int unit, soc_mem_t tbl_mem, uint16 tbl_entry_sz,
                     const char *descr, int start, int end, uint8 **res_ptr)
{
    int alloc_size;             /* Allocation size.          */
    uint8 *buffer;               /* Buffer to read the table. */
    int tbl_size;               /* HW table size.            */

    /* Input parameters sanity check. */
    if ((NULL == res_ptr) || (NULL == descr)) {
        return (SOC_E_PARAM);
    }

    /* If entry size is not deterministic. reject. */
    if (tbl_entry_sz <= 0) {
        return (SOC_E_PARAM);
    }

    if (INVALIDm == tbl_mem) {
        return (SOC_E_NOT_FOUND);
    }

    if (start < soc_mem_index_min(unit, tbl_mem) ||
        end > soc_mem_index_max(unit, tbl_mem)) {
        return SOC_E_PARAM;
    }

    /* Calculate table size. */
    tbl_size = (end - start) + 1;
    alloc_size = tbl_entry_sz * tbl_size;

    /* Allocate memory buffer. */
    buffer = soc_cm_salloc(unit, alloc_size, descr);
    if (buffer == NULL) {
        return (SOC_E_MEMORY);
    }

    /* Reset allocated buffer. */
    memset(buffer, 0, alloc_size);

    /* Read table to the buffer. */
    if (soc_mem_read_range(unit, tbl_mem, MEM_BLOCK_ANY,
                           start,
                           end, buffer) < 0) {
        soc_cm_sfree(unit, buffer);
        return (SOC_E_INTERNAL);
    }

    *res_ptr = buffer;
    return (SOC_E_NONE);
}


STATIC int
soc_esw_ecmp_protected_idx_alloc(int unit, int bix, int ecmp_idx, int count)
{
    int offset;
    int start;
    soc_esw_ecmp_bank_state_t *bank_info;

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bix);
    offset = bank_info->tbl_offset;

    start = (ecmp_idx - offset);
    SHR_BITSET_RANGE(bank_info->idx_used, start, count);

    return SOC_E_NONE;
}

STATIC int
soc_esw_ecmp_protected_idx_free(int unit, int bix, int ecmp_idx, int count)
{
    int start;
    int offset;
    soc_esw_ecmp_bank_state_t *bank_info;

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bix);
    offset = bank_info->tbl_offset;

    start = (ecmp_idx - offset);
    SHR_BITCLR_RANGE(bank_info->idx_used, start, count);

    return SOC_E_NONE;
}

STATIC int
soc_esw_ecmp_protected_idx_move(int unit,
                                int src_bix, int to_bix,
                                int ecmp_idx, int count)
{
    int rv;
    int start;

    start = (SOC_ESW_ECMP_BANK_INFO(unit, src_bix))->tbl_offset + ecmp_idx;
    rv = soc_esw_ecmp_protected_idx_free(unit, src_bix, start, count);
    SOC_IF_ERROR_RETURN(rv);

    start = (SOC_ESW_ECMP_BANK_INFO(unit, to_bix))->tbl_offset + ecmp_idx;
    rv = soc_esw_ecmp_protected_idx_alloc(unit, to_bix, start, count);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}




STATIC int
soc_esw_ecmp_idx_bank_get(int unit, int idx, int *bank_idx)
{
    int bix;
    soc_esw_ecmp_bank_state_t *bank_info;

    for (bix = 0; bix < SOC_ESW_ECMP_STATE(unit)->ecmp_member_banks; bix++) {
        bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bix);
        if ((idx >= bank_info->tbl_offset) &&
            (idx < (bank_info->tbl_offset + bank_info->bank_size))) {
            *bank_idx = bix;
            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}


STATIC int
soc_esw_ecmp_idx_bank_offset_get(int unit, int *if_id)
{
    int rv;
    int bank_idx;

    rv = soc_esw_ecmp_idx_bank_get(unit, *if_id, &bank_idx);
    SOC_IF_ERROR_RETURN(rv);

    *if_id -= (SOC_ESW_ECMP_BANK_INFO(unit, bank_idx))->tbl_offset;

    return SOC_E_NONE;
}



STATIC int
soc_esw_ecmp_protected_state_set(int unit, int grp_idx,
                                 int bank_idx, int base, int count)
{
    soc_esw_ecmp_group_state_t *grp_state;

    grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx);

    grp_state->bank = bank_idx;
    grp_state->base = base;
    grp_state->count = count;

    return SOC_E_NONE;
}

STATIC int
soc_esw_ecmp_protected_state_get(int unit, int grp_idx,
                                 int *bank_idx, int *base, int *count)
{
    soc_esw_ecmp_group_state_t *grp_state;

    grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx);

    if (bank_idx) {
        *bank_idx = grp_state->bank;
    }
    if (base) {
        *base = grp_state->base;
    }
    if (count) {
        *count = grp_state->count;
    }

    return SOC_E_NONE;
}

STATIC int
soc_esw_ecmp_bank_used_count_get(int unit, int bank_idx, int *count)
{
    int ix;
    int use_count = 0;
    soc_esw_ecmp_bank_state_t *bank_info;

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bank_idx);

    for (ix = 0; ix < _SHR_BITDCLSIZE(bank_info->bank_size); ix++) {
        use_count += _shr_popcount(bank_info->idx_used[ix]);
    }

    *count = use_count;

    return SOC_E_NONE;
}

STATIC int
soc_esw_ecmp_bank_active_set(int unit, int bank_idx)
{
    int rv;
    int rsvd_bank;
    uint32 regval = 0;
    uint32 banksel[1] = {0};

    rsvd_bank = SOC_ESW_ECMP_BANK_RSVD_GET(unit);

    /* Update BANK_SEL config. */
    rv = soc_reg32_get(unit, ING_L3_ECMP_BANK_SELr, REG_PORT_ANY, 0, &regval);
    SOC_IF_ERROR_RETURN(rv);

    banksel[0] = soc_reg_field_get(unit, ING_L3_ECMP_BANK_SELr, regval, BANK_SELf);
    if (SHR_BITGET(banksel, bank_idx)) {
        SHR_BITSET(banksel, rsvd_bank);
    } else {
        SHR_BITCLR(banksel, rsvd_bank);
    }
    soc_reg_field_set(unit, ING_L3_ECMP_BANK_SELr, &regval, BANK_SELf, banksel[0]);

    rv = soc_reg32_set(unit, ING_L3_ECMP_BANK_SELr, REG_PORT_ANY, 0, regval);
    SOC_IF_ERROR_RETURN(rv);
    rv = soc_reg32_set(unit, INITIAL_ING_L3_ECMP_BANK_SELr, REG_PORT_ANY, 0, regval);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}

STATIC int
soc_esw_ecmp_bank_reserved_set(int unit, int bank_idx)
{
    int rv;
    int rsvd_bank;
    int tmp_level;
    int tmp_prot_disabled;
    uint32 regval = 0;
    uint32 banksel[1] = {0};
    soc_esw_ecmp_bank_state_t *src_bank_info, *dst_bank_info;

    rsvd_bank = SOC_ESW_ECMP_BANK_RSVD_GET(unit);

    src_bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bank_idx);
    dst_bank_info = SOC_ESW_ECMP_BANK_INFO(unit, rsvd_bank);

    tmp_level = dst_bank_info->level;
    tmp_prot_disabled = dst_bank_info->protected_write_disabled;

    dst_bank_info->level = src_bank_info->level;
    dst_bank_info->protected_write_disabled = src_bank_info->protected_write_disabled;

    src_bank_info->level = tmp_level;
    src_bank_info->protected_write_disabled = tmp_prot_disabled;

    /* Update BANK_SEL config. */
    rv = soc_reg32_get(unit, ING_L3_ECMP_BANK_SELr, REG_PORT_ANY, 0, &regval);
    SOC_IF_ERROR_RETURN(rv);

    banksel[0] = soc_reg_field_get(unit, ING_L3_ECMP_BANK_SELr, regval, BANK_SELf);
    SHR_BITCLR(banksel, bank_idx);
    soc_reg_field_set(unit, ING_L3_ECMP_BANK_SELr, &regval, BANK_SELf, banksel[0]);

    rv = soc_reg32_set(unit, ING_L3_ECMP_BANK_SELr, REG_PORT_ANY, 0, regval);
    SOC_IF_ERROR_RETURN(rv);
    rv = soc_reg32_set(unit, INITIAL_ING_L3_ECMP_BANK_SELr, REG_PORT_ANY, 0, regval);
    SOC_IF_ERROR_RETURN(rv);

    /* Mark bank_idx as reserved */
    SOC_ESW_ECMP_STATE(unit)->ecmp_spare_bank_idx = bank_idx;

    return SOC_E_NONE;
}

STATIC int
soc_esw_ecmp_protected_idx_range_get(int unit, int bank_idx, int rsvd_bank,
                                     int ecmp_idx, int max_grp_size,
                                     int *rd_start, int *rd_end,
                                     int *wr_start, int *wr_end)
{
    int use_count;
    soc_esw_ecmp_bank_state_t *bank_info;

    SOC_IF_ERROR_RETURN(soc_esw_ecmp_bank_used_count_get(unit, bank_idx, &use_count));

    if ((!SOC_ESW_ECMP_BANK_PROTECTED_ACCESS(unit, bank_idx)) || (use_count == 0)) {
        *rd_start = *wr_start = (ecmp_idx);
        *rd_end   = *wr_end   = (ecmp_idx + max_grp_size);

        return SOC_E_NONE;
    }

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bank_idx);
    *rd_start = (bank_info->tbl_offset);
    *rd_end   = (*rd_start + bank_info->bank_size - 1);

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, rsvd_bank);
    *wr_start = (bank_info->tbl_offset);
    *wr_end   = (*wr_start + bank_info->bank_size - 1);

    return SOC_E_NONE;
}


STATIC int
soc_esw_ecmp_protected_bank_update(int unit, int src_bank, int dst_bank)
{
    int rv;
    int grp_idx;
    int ecmp_base_offset;
    int idx_min, idx_max;
    soc_mem_t ecmp_grp_mem1, ecmp_grp_mem2;
    soc_esw_ecmp_group_state_t *grp_state;

    uint8 *ecmp1_tbl_p = NULL;    /* L3_ECMP_GROUP table pointer. */
    uint8 *ecmp2_tbl_p = NULL;    /* INITIAL_L3_ECMP_GROUP table pointer. */

    uint32 *ecmp_grp_ptr = NULL;  /* ECMP group pointer.*/
    uint32 *ecmp2_grp_ptr = NULL; /* INITIAL_ECMP group pointer.*/

    ecmp_grp_mem1 = L3_ECMP_COUNTm;
    ecmp_grp_mem2 = INITIAL_L3_ECMP_GROUPm;
    idx_min = soc_mem_index_min(unit, ecmp_grp_mem1);
    idx_max = soc_mem_index_max(unit, ecmp_grp_mem1);

    /* Read ecmp group tables */
    rv = soc_esw_mem_dma_read(unit, ecmp_grp_mem1,
                              sizeof(ecmp_count_entry_t),
                              "ecmp_grp_tbl",
                              idx_min, idx_max,
                              &ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_mem_dma_read(unit, ecmp_grp_mem2,
                              sizeof(initial_l3_ecmp_group_entry_t),
                              "ecmp2_grp_tbl",
                              idx_min, idx_max,
                              &ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);


    for (grp_idx = 0; grp_idx < SOC_ESW_ECMP_MAX_GROUPS; grp_idx++) {
        grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx);

        /* Group is not installed yet */
        if (grp_state->count == 0) {
            continue;
        }
        /* Group does not have members in the src_bank */
        if (grp_state->bank != src_bank) {
            continue;
        }

        ecmp_base_offset =  grp_state->base;
        rv = soc_esw_ecmp_idx_bank_offset_get(unit, &ecmp_base_offset);
        SOC_ESW_ECMP_IF_ERR_EXIT(rv);

        /* Update ecmp base and bank_idx for the group */
        grp_state->base = ((SOC_ESW_ECMP_BANK_INFO(unit, dst_bank))->tbl_offset + ecmp_base_offset);
        grp_state->bank = dst_bank;


        /* Move ecmp idx range from src_bank to dst_bank. */
        rv = soc_esw_ecmp_protected_idx_move(unit, src_bank, dst_bank,
                                             ecmp_base_offset, grp_state->count);
        SOC_ESW_ECMP_IF_ERR_EXIT(rv);

        ecmp_grp_ptr =
            soc_mem_table_idx_to_pointer(unit, ecmp_grp_mem1, uint32 *,
                                         ecmp1_tbl_p, grp_idx);
        ecmp2_grp_ptr =
            soc_mem_table_idx_to_pointer(unit, ecmp_grp_mem2, uint32 *,
                                         ecmp2_tbl_p, grp_idx);

        soc_mem_field32_set(unit, ecmp_grp_mem1, ecmp_grp_ptr,
                            BASE_PTRf, grp_state->base);
        soc_mem_field32_set(unit, ecmp_grp_mem2, ecmp2_grp_ptr,
                            BASE_PTRf, grp_state->base);
    }

    rv = soc_esw_ecmp_bank_active_set(unit, src_bank);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Write updated member tables  */
    rv = soc_mem_write_range(unit, ecmp_grp_mem1, MEM_BLOCK_ALL,
                             idx_min, idx_max, ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_mem_write_range(unit, ecmp_grp_mem2, MEM_BLOCK_ALL,
                             idx_min, idx_max, ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_ecmp_bank_reserved_set(unit, src_bank);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

err_exit:
    if (ecmp2_tbl_p) {
        soc_cm_sfree(unit, ecmp2_tbl_p);
    }
    if (ecmp1_tbl_p) {
        soc_cm_sfree(unit, ecmp1_tbl_p);
    }

    return rv;
}


STATIC int
soc_esw_ecmp_protected_base_ptr_update(int unit)
{
    int rv;
    int grp_idx;
    int idx_min, idx_max;
    soc_mem_t ecmp_grp_mem1, ecmp_grp_mem2;

    soc_esw_ecmp_group_state_t *grp_state;

    uint8 *ecmp1_tbl_p = NULL;    /* L3_ECMP_GROUP table pointer. */
    uint8 *ecmp2_tbl_p = NULL;    /* INITIAL_L3_ECMP_GROUP table pointer. */

    uint32 *ecmp_grp_ptr = NULL;  /* ECMP group pointer.*/
    uint32 *ecmp2_grp_ptr = NULL; /* INITIAL_ECMP group pointer.*/

    ecmp_grp_mem1 = L3_ECMP_COUNTm;
    ecmp_grp_mem2 = INITIAL_L3_ECMP_GROUPm;
    idx_min = soc_mem_index_min(unit, ecmp_grp_mem1);
    idx_max = soc_mem_index_max(unit, ecmp_grp_mem1);

    /* Read ecmp group tables */
    rv = soc_esw_mem_dma_read(unit, ecmp_grp_mem1,
                              sizeof(ecmp_count_entry_t),
                              "ecmp_grp_tbl",
                              idx_min, idx_max,
                              &ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_mem_dma_read(unit, ecmp_grp_mem2,
                              sizeof(initial_l3_ecmp_group_entry_t),
                              "ecmp2_grp_tbl",
                              idx_min, idx_max,
                              &ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);


    for (grp_idx = 0; grp_idx < SOC_ESW_ECMP_MAX_GROUPS; grp_idx++) {
        grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx);

        /* Group not in use. */
        if (!grp_state->count && !grp_state->base) {
            continue;
        }

        ecmp_grp_ptr =
            soc_mem_table_idx_to_pointer(unit, ecmp_grp_mem1, uint32 *,
                                         ecmp1_tbl_p, grp_idx);
        ecmp2_grp_ptr =
            soc_mem_table_idx_to_pointer(unit, ecmp_grp_mem2, uint32 *,
                                         ecmp2_tbl_p, grp_idx);

        soc_mem_field32_set(unit, ecmp_grp_mem1, ecmp_grp_ptr,
                            BASE_PTRf, grp_state->base);
        soc_mem_field32_set(unit, ecmp_grp_mem2, ecmp2_grp_ptr,
                            BASE_PTRf, grp_state->base);
    }

    /* Write updated member tables  */
    rv = soc_mem_write_range(unit, ecmp_grp_mem1, MEM_BLOCK_ALL,
                             idx_min, idx_max, ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_mem_write_range(unit, ecmp_grp_mem2, MEM_BLOCK_ALL,
                             idx_min, idx_max, ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

err_exit:
    if (ecmp2_tbl_p) {
        soc_cm_sfree(unit, ecmp2_tbl_p);
    }
    if (ecmp1_tbl_p) {
        soc_cm_sfree(unit, ecmp1_tbl_p);
    }

    return rv;
}


STATIC int
soc_esw_ecmp_protected_member_move(int unit,
                                   int grp_idx,
                                   int src_bank_idx,
                                   int dst_bank_idx,
                                   uint8 *src_ecmp_tbl1_p,
                                   uint8 *src_ecmp_tbl2_p,
                                   uint8 *dst_ecmp_tbl1_p,
                                   uint8 *dst_ecmp_tbl2_p,
                                   int new_base_idx
                                   )
{
    int rv;
    int idx;
    int ecmp_base_offset;
    soc_esw_ecmp_bank_state_t *src_bank_info, *dst_bank_info;
    soc_esw_ecmp_group_state_t *grp_state;

    uint32 *src_ecmp1_p = NULL;
    uint32 *src_ecmp2_p = NULL;
    uint32 *dst_ecmp1_p = NULL;
    uint32 *dst_ecmp2_p = NULL;

    src_bank_info = SOC_ESW_ECMP_BANK_INFO(unit, src_bank_idx);
    dst_bank_info = SOC_ESW_ECMP_BANK_INFO(unit, dst_bank_idx);

    grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx);
    ecmp_base_offset = (grp_state->base - src_bank_info->tbl_offset);

    for (idx = ecmp_base_offset;
         idx < (ecmp_base_offset + grp_state->count);
         idx++) {
        src_ecmp1_p =
            soc_mem_table_idx_to_pointer(unit, L3_ECMPm, uint32 *,
                                          src_ecmp_tbl1_p, idx);
        src_ecmp2_p =
            soc_mem_table_idx_to_pointer(unit, INITIAL_L3_ECMPm, uint32 *,
                                         src_ecmp_tbl2_p, idx);

        dst_ecmp1_p =
            soc_mem_table_idx_to_pointer(unit, L3_ECMPm, uint32 *,
                                          dst_ecmp_tbl1_p, new_base_idx);
        dst_ecmp2_p =
            soc_mem_table_idx_to_pointer(unit, INITIAL_L3_ECMPm, uint32 *,
                                         dst_ecmp_tbl2_p, new_base_idx);

        sal_memcpy(dst_ecmp1_p, src_ecmp1_p, sizeof(ecmp_entry_t));
        sal_memcpy(dst_ecmp2_p, src_ecmp2_p,
                   sizeof(initial_l3_ecmp_entry_t));

        new_base_idx++;
    }

    /* Free idx from src_bank. */
    rv = soc_esw_ecmp_protected_idx_free(unit,
                                         src_bank_idx,
                                         grp_state->base,
                                         grp_state->count);
    SOC_IF_ERROR_RETURN(rv);

    grp_state->base = (dst_bank_info->tbl_offset + new_base_idx - grp_state->count);

    /* Alloc idx in dst_bank. */
    rv = soc_esw_ecmp_protected_idx_alloc(unit,
                                          dst_bank_idx,
                                          grp_state->base,
                                          grp_state->count);
    SOC_IF_ERROR_RETURN(rv);

    /* Update ECMP group bank */
    grp_state->bank = dst_bank_idx;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_grp_size_update
 * Purpose:
 *      Update the max group size of an ECMP group.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      grp_info   - (IN)ecmp group id to write.
 *      base_ptr   - (IN)ecmp member base pointer.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_esw_ecmp_protected_grp_size_update(int unit,
                                       soc_esw_ecmp_group_info_t *grp_info,
                                       int *base_ptr)
{
    int rv = SOC_E_NONE;
    int bix;
    int bank;
    int grp_idx;
    int old_max_grp_sz;
    int new_max_grp_size;
    int free_count = 0;
    int free_idx;
    int level;
    uint32 bank_bmp = 0;
    soc_esw_ecmp_group_state_t *grp_state;
    soc_esw_ecmp_bank_state_t *bank_info;

    grp_idx = grp_info->ecmp_grp;
    new_max_grp_size = grp_info->max_paths;

    grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx);
    bank = grp_state->bank;
    old_max_grp_sz = grp_state->count;

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bank);
    level = bank_info->level;

    /* Get the number of free entries in this bank. */
    rv = soc_esw_ecmp_bank_free_count_get(unit,
                                          bank,
                                          &free_count);
    SOC_IF_ERROR_RETURN(rv);

    if (new_max_grp_size <= (free_count + old_max_grp_sz)) {
        rv = soc_esw_ecmp_protected_bank_defragment(unit,
                                                    bank,
                                                    grp_idx,
                                                    &free_count);
        SOC_IF_ERROR_RETURN(rv);

        /* Check if the current bank can accommodate the new max path */
        *base_ptr = grp_state->base;
    } else {
        rv = soc_esw_ecmp_protected_banks_get(unit,
                                              level,
                                              &bank_bmp);
        SOC_IF_ERROR_RETURN(rv);

        free_count = 0;
        /* Compress other banks of the same level to make room for new group */
        for (bix = 0; bix < SOC_ESW_ECMP_MAX_BANKS; bix++) {
            if (SHR_BITGET(&bank_bmp, bix) && bix != grp_state->bank) {
                rv = soc_esw_ecmp_bank_free_count_get(unit,
                                                      bix,
                                                      &free_count);
                SOC_IF_ERROR_RETURN(rv);

                /* Not enough space in this bank, continue. */
                if (free_count < new_max_grp_size) {
                    continue;
                }

                rv = soc_esw_ecmp_protected_bank_defragment(unit,
                                                            bix,
                                                            -1,
                                                            &free_count);
                SOC_IF_ERROR_RETURN(rv);

                /* If the defragmented bank can fit the new group, break */
                if (free_count >= new_max_grp_size) {
                    break;
                }
            }
        }

        if (free_count < new_max_grp_size) {
            return SOC_E_FULL;
        }

        /* Check if a free idx can be found now */
        free_idx = -1;
        bank_bmp = 0;
        rv = soc_esw_ecmp_protected_banks_get(unit,
                                              level,
                                              &bank_bmp);
        SOC_IF_ERROR_RETURN(rv);


        for (bix = 0; bix < SOC_ESW_ECMP_MAX_BANKS; bix++) {
            if (SHR_BITGET(&bank_bmp, bix) && bix != grp_state->bank) {
                rv = soc_esw_ecmp_protected_idx_free_get(unit,
                                                         bix,
                                                         new_max_grp_size,
                                                         &free_idx);
                if (rv == SOC_E_NONE) {
                    *base_ptr = free_idx;
                    break;
                }
            }
        }
    }

    return rv;
}

/* | *************************************************** |
 * | *************************************************** |
 * | *           Start - Public Functions              * |
 * | *************************************************** |
 * | *************************************************** |
 */

/* Function to initialize ecmp_init_info struct */
void
soc_esw_ecmp_init_info_t_init(soc_esw_ecmp_init_info_t *info)
{
    sal_memset(info, 0, sizeof(soc_esw_ecmp_init_info_t));

    return;
}

/* Function to initialize ecmp_group_info struct */
void
soc_esw_ecmp_group_info_t_init(soc_esw_ecmp_group_info_t *info)
{
    sal_memset(info, 0, sizeof(soc_esw_ecmp_group_info_t));

    return;
}

/*
 * Function:
 *      soc_esw_ecmp_state_lock
 * Purpose:
 *      Mutext Take (lock) ecmp state
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_state_lock(int unit)
{
    if (SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock == NULL) {
        return SOC_E_INIT;
    }

    sal_mutex_take(SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock,
                   sal_mutex_FOREVER);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_ecmp_state_unlock
 * Purpose:
 *      Mutext Give (unlock) ecmp state
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_state_unlock(int unit)
{
    if (SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock == NULL) {
        return SOC_E_INIT;
    }

    if (sal_mutex_give(SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock) != 0) {
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

STATIC void
soc_esw_ecmp_protected_state_wb_alloc_size(int unit, uint32 *size)
{
    int ix;
    int alloc_size;
    soc_esw_ecmp_group_state_t *group_info;
    soc_esw_ecmp_bank_state_t *bank_info;

    alloc_size = 0;

    /* spare bank idx */
    alloc_size += SOC_ESW_ECMP_WB_SIZE(&(SOC_ESW_ECMP_STATE(unit)->ecmp_spare_bank_idx));

    /* Sync group_info structs. */
    for (ix = 0; ix < SOC_ESW_ECMP_MAX_GROUPS; ix++) {
        group_info = SOC_ESW_ECMP_GROUP_INFO(unit, ix);

        alloc_size += SOC_ESW_ECMP_WB_SIZE(group_info);
    }

    /* Sync bank_info structs. */
    for (ix = 0; ix < SOC_ESW_ECMP_MAX_BANKS; ix++) {
        bank_info = SOC_ESW_ECMP_BANK_INFO(unit, ix);

        alloc_size += SOC_ESW_ECMP_WB_SIZE(bank_info);
    }

    /* Return the calculated size. */
    *size = alloc_size;

    return;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_state_wb_alloc_size
 * Purpose:
 *      Allocate persistent info memory for soc esw ecmp module
 * Parameters:
 *     unit - (IN) SOC device id
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_state_warmboot_alloc(int unit)
{

    uint8 *ecmp_scache_ptr;
    uint32 size = 0;
    int rv = SOC_E_NONE;
    soc_scache_handle_t scache_handle;

    scache_handle = SOC_ESW_ECMP_STATE(unit)->scache_handle;


    soc_esw_ecmp_protected_state_wb_alloc_size(unit, &size);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle, TRUE,
                                 &size, &ecmp_scache_ptr,
                                 SOC_ESW_ECMP_WB_DEFAULT_VERSION, NULL);

    if (SOC_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = SOC_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *     soc_esw_ecmp_protected_state_warmboot_sync
 * Purpose:
 *     Sync soc esw ecmp state.
 * Parameters:
 *     unit - (IN) SOC device id
 *
 * Returns:
 *     SOC_E_XXX
 */
int
soc_esw_ecmp_protected_state_warmboot_sync(int unit)
{
    int ix;
    uint32 size;
    soc_esw_ecmp_group_state_t *group_info;
    soc_esw_ecmp_bank_state_t *bank_info;
    uint8 *ecmp_scache_ptr;
    soc_scache_handle_t scache_handle;

    scache_handle = SOC_ESW_ECMP_STATE(unit)->scache_handle;


    /* Get the size to save ecmp protected warmboot state. */
    soc_esw_ecmp_protected_state_wb_alloc_size(unit, &size);

    SOC_IF_ERROR_RETURN
        (soc_versioned_scache_ptr_get(unit, scache_handle, FALSE,
                                 &size, &ecmp_scache_ptr,
                                 SOC_ESW_ECMP_WB_DEFAULT_VERSION, NULL));


    /* spare bank idx */
    SOC_ESW_ECMP_WB_SYNC(ecmp_scache_ptr, &(SOC_ESW_ECMP_STATE(unit)->ecmp_spare_bank_idx));

    /* Sync group_info structs. */
    for (ix = 0; ix < SOC_ESW_ECMP_MAX_GROUPS; ix++) {
        group_info = SOC_ESW_ECMP_GROUP_INFO(unit, ix);

        SOC_ESW_ECMP_WB_SYNC(ecmp_scache_ptr, group_info);
    }

    /* Sync bank_info structs. */
    for (ix = 0; ix < SOC_ESW_ECMP_MAX_BANKS; ix++) {
        bank_info = SOC_ESW_ECMP_BANK_INFO(unit, ix);

        SOC_ESW_ECMP_WB_SYNC(ecmp_scache_ptr, bank_info);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_esw_ecmp_protected_state_warmboot_recover
 * Purpose:
 *     Recover soc esw ecmp state.
 * Parameters:
 *     unit - (IN) SOC device id
 *
 * Returns:
 *     SOC_E_XXX
 */

int
soc_esw_ecmp_protected_state_warmboot_recover(int unit)
{
    int ix;
    uint32 size;
    soc_esw_ecmp_group_state_t *group_info;
    soc_esw_ecmp_bank_state_t *bank_info;
    uint8 *ecmp_scache_ptr;
    soc_scache_handle_t scache_handle;

    scache_handle = SOC_ESW_ECMP_STATE(unit)->scache_handle;

    /* Get the size to save ecmp protected warmboot state. */
    soc_esw_ecmp_protected_state_wb_alloc_size(unit, &size);


    SOC_IF_ERROR_RETURN
        (soc_versioned_scache_ptr_get(unit, scache_handle, FALSE,
                                 &size, &ecmp_scache_ptr,
                                 SOC_ESW_ECMP_WB_DEFAULT_VERSION, NULL));

    /* spare bank idx */
    SOC_ESW_ECMP_WB_RECOVER(ecmp_scache_ptr, &(SOC_ESW_ECMP_STATE(unit)->ecmp_spare_bank_idx));

    /* Sync group_info structs. */
    for (ix = 0; ix < SOC_ESW_ECMP_MAX_GROUPS; ix++) {
        group_info = SOC_ESW_ECMP_GROUP_INFO(unit, ix);

        SOC_ESW_ECMP_WB_RECOVER(ecmp_scache_ptr, group_info);
    }

    /* Sync bank_info structs. */
    for (ix = 0; ix < SOC_ESW_ECMP_MAX_BANKS; ix++) {
        bank_info = SOC_ESW_ECMP_BANK_INFO(unit, ix);

        SOC_ESW_ECMP_WB_RECOVER(ecmp_scache_ptr, bank_info);
    }

    return SOC_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      soc_esw_ecmp_protected_init
 * Purpose:
 *      Initialize state for ecmp protected accesses
 * Parameters:
 *      unit            - (IN)SOC unit number.
 *      first_lkup_sz   - (IN)ECMP table first lookup size.
 *      ecmp_table_sz   - (IN)ECMP table total size.
 *      ecmp_nh_offset  - (IN)ECMP next hop offset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_init(int unit,
                            soc_esw_ecmp_init_info_t *init_info)
{
    int ix;
    int bix, bix_r;
    int tbl_offset;
    int bank_size;
    int lkup1_banks, lkup2_banks;
    soc_mem_t mem;
    soc_esw_ecmp_bank_state_t *bank_info;

    if ((init_info == NULL) ||
        (init_info->bank_size > SOC_ESW_ECMP_BANK_SIZE)) {
        return SOC_E_INIT;
    }

    /* Make sure the ECMP member and group tables are always enabled for memcache */
    for (ix = 0; ix < COUNTOF(ecmp_member_mems); ix++) {
        mem = ecmp_member_mems[0];
        if (SOC_MEM_STATE(unit, mem).cache[SOC_MEM_BLOCK_ANY(unit, mem)] == NULL) {
            return SOC_E_INIT;
        }
    }
    for (ix = 0; ix < COUNTOF(ecmp_group_mems); ix++) {
        mem = ecmp_group_mems[0];
        if (SOC_MEM_STATE(unit, mem).cache[SOC_MEM_BLOCK_ANY(unit, mem)] == NULL) {
            return SOC_E_INIT;
        }
    }


    /* Allocate export software state. */
    if (SOC_ESW_ECMP_STATE(unit) == NULL) {
        SOC_ESW_ECMP_STATE(unit) =
            sal_alloc(sizeof(soc_esw_ecmp_state_t),
                      "SOC esw ecmp sw state");
    }

    if (SOC_ESW_ECMP_STATE(unit) == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(SOC_ESW_ECMP_STATE(unit), 0, sizeof(soc_esw_ecmp_state_t));

    /* Create ECMP State lock */
    if (SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock == NULL) {
        SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock =
            sal_mutex_create("soc.esw.ecmp.state.mutex");

        if (SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock == NULL) {
            sal_free(SOC_ESW_ECMP_STATE(unit));
            SOC_ESW_ECMP_STATE(unit) = NULL;
            return SOC_E_MEMORY;
        }
    }

    SOC_ESW_ECMP_STATE(unit)->ecmp_member_banks = init_info->num_banks;
    SOC_ESW_ECMP_STATE(unit)->ecmp_members_per_banks = init_info->bank_size;
    SOC_ESW_ECMP_STATE(unit)->ecmp_grp_if_offset = init_info->ecmp_if_offset;
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_ESW_ECMP_STATE(unit)->scache_handle = init_info->scache_handle;
#endif /* BCM_WARM_BOOT_SUPPORT */

    tbl_offset = 0;
    bank_size = init_info->bank_size;
    lkup1_banks = ((init_info->lkup1_size + (bank_size - 1)) / bank_size);
    lkup2_banks = ((init_info->lkup2_size + (bank_size - 1)) / bank_size);

    bix_r = -1;
    for (bix = 0; bix < SOC_ESW_ECMP_MAX_BANKS; bix++) {
        bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bix);

        if (init_info->num_levels < 2) {
            bank_info->protected_write_disabled = 1;
            bank_info->level = socEswEcmpBankLkupLevelInvalid;
            continue;
        }

        
        if (bix < lkup1_banks) {
            bank_info->level = socEswEcmpBankLkupLevelOverlay;
            bank_info->protected_write_disabled = 0;
        } else if (bix < (lkup1_banks + lkup2_banks)) {
            bank_info->level = socEswEcmpBankLkupLevelUnderlay;
            bank_info->protected_write_disabled = 0;
        } else {
            if (bix_r == -1) {
                bix_r = bix;
                bank_info->level = socEswEcmpBankLkupLevelReserved;

                SOC_ESW_ECMP_STATE(unit)->ecmp_spare_bank_idx = bix;
            } else {
                bank_info->level = socEswEcmpBankLkupLevelInvalid;
            }
            bank_info->protected_write_disabled = 1;
        }

        
        bank_info->idx_min = 0;
        bank_info->idx_max = bank_size;
        bank_info->bank_size = bank_size;

        bank_info->tbl_offset = tbl_offset;
        tbl_offset += bank_size;
    }


#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(soc_esw_ecmp_protected_state_warmboot_alloc(unit));
    } else {
        SOC_IF_ERROR_RETURN(soc_esw_ecmp_protected_state_warmboot_recover(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_detach
 * Purpose:
 *      De-Initialize state for ecmp protected accesses
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_detach(int unit)
{

    /* Destroy ECMP State lock */
    if (SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock != NULL) {
        sal_mutex_destroy(SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock);
        SOC_ESW_ECMP_STATE(unit)->soc_esw_ecmp_lock = NULL;
    }

    /* Free export software state. */
    if (SOC_ESW_ECMP_STATE(unit)) {
        sal_free(SOC_ESW_ECMP_STATE(unit));
        SOC_ESW_ECMP_STATE(unit) = NULL;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_esw_ecmp_protected_enabled
 * Purpose:
 *      Checks whether protected ecmp is required or not.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 * Returns:
 *      TRUE:  Protected ECMP access routines should be used
 *      FALSE: Protected ECMP access routines are not required
 */
int
soc_esw_ecmp_protected_enabled(int unit)
{
    int bix;

    /* SOC featue is not set. Return FALSE. */
    if (!soc_feature(unit, soc_feature_l3_ecmp_protected_access)) {
        return FALSE;
    }

    /* Ecmp state is not initialized yet. Return FALSE. */
    if (SOC_ESW_ECMP_STATE(unit) == NULL) {
        return FALSE;
    }

    /* TRUE if at least one bank requires protected writing. */
    for (bix = 0; bix < SOC_ESW_ECMP_STATE(unit)->ecmp_member_banks; bix++) {
        if (SOC_ESW_ECMP_BANK_PROTECTED_ACCESS(unit, bix)) {
            return TRUE;
        }
    }

    return FALSE;
}

int
soc_esw_ecmp_protected_idx_free_get(int unit,
                                    int bix,
                                    int count,
                                    int *free_idx)
{
    int idx, lkup_idx;
    soc_esw_ecmp_bank_state_t *bank_info;

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bix);

    *free_idx = -1;
    for (lkup_idx = 0;
         lkup_idx <= bank_info->bank_size; lkup_idx++) {

        if (!SHR_BITGET(bank_info->idx_used, lkup_idx)) {
            idx = count;
            /* Check if the group crosses the bank boundary. */
            if ((lkup_idx + idx) > bank_info->bank_size) {
                break;
            }
            while (--idx) {
                /* Check if free slots can accomodate wide entry. */
                if (SHR_BITGET(bank_info->idx_used, lkup_idx + idx)) {
                    break;
                }
            }
            if (idx) {
                /* There is no room to accomodate wide entry. */
                continue;
            }
            *free_idx = (lkup_idx + bank_info->tbl_offset);
            return (SOC_E_NONE);
        }
    }

    return SOC_E_FULL;
}


int
soc_esw_ecmp_protected_banks_get(int unit,
                                 soc_esw_ecmp_bank_lkup_level_t lvl,
                                 uint32 *bank_bitmap)
{
    int bix;
    soc_esw_ecmp_bank_state_t *bank_info;

    for (bix = 0; bix < SOC_ESW_ECMP_STATE(unit)->ecmp_member_banks; bix++) {
        bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bix);
        if (bank_info->level == lvl) {
            SHR_BITSET(bank_bitmap, bix);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_bank_defragment
 * Purpose:
 *      Defragment an ecmp bank to fill up holes in
 *      the middle of the ecmp member table and compress.
 * Parameters:
 *      unit            - (IN)  SOC unit number.
 *      bank_idx        - (IN)  bank idx to compress.
 *      grp_idx_move    - (IN)  ecmp group id to write.
 *      free_count      - (OUT) Free count in bank after compression.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_bank_defragment(int unit,
                                       int bank_idx,
                                       int grp_idx_move,
                                       int *free_count)
{
    int rv;
    int grp_idx;
    int new_base_idx;
    int rd_start, rd_end, wr_start, wr_end;

    soc_esw_ecmp_group_state_t *grp_state;
    soc_esw_ecmp_bank_state_t *src_bank_info, *dst_bank_info;

    uint8 *ecmp1_tbl_p = NULL;     /* L3_ECMP table pointer. */
    uint8 *ecmp2_tbl_p = NULL;     /* INITIAL_L3_ECMP table pointer. */
    uint8 *dst_ecmp1_tbl_p = NULL; /* L3_ECMP table pointer. */
    uint8 *dst_ecmp2_tbl_p = NULL; /* INITIAL_L3_ECMP table pointer. */

    src_bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bank_idx);
    dst_bank_info = SOC_ESW_ECMP_BANK_INFO(unit, SOC_ESW_ECMP_BANK_RSVD_GET(unit));


    rd_start = src_bank_info->tbl_offset;
    rd_end = (src_bank_info->tbl_offset + src_bank_info->bank_size - 1);

    wr_start = dst_bank_info->tbl_offset;
    wr_end = (dst_bank_info->tbl_offset + dst_bank_info->bank_size - 1);

    /* Copy all member table entries into buffer */
    rv = soc_esw_mem_dma_read(unit, L3_ECMPm,
                              sizeof(ecmp_entry_t),
                              "ecmp_tbl",
                              rd_start, rd_end,
                              &ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_mem_dma_read(unit, INITIAL_L3_ECMPm,
                              sizeof(initial_l3_ecmp_entry_t),
                              "ecmp2_tbl",
                              rd_start, rd_end,
                              &ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Read ecmp member tables */
    rv = soc_esw_mem_dma_read(unit, L3_ECMPm,
                              sizeof(ecmp_entry_t),
                              "ecmp_tbl",
                              wr_start, wr_end,
                              &dst_ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_mem_dma_read(unit, INITIAL_L3_ECMPm,
                              sizeof(initial_l3_ecmp_entry_t),
                              "ecmp2_tbl",
                              wr_start, wr_end,
                              &dst_ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);


    new_base_idx = 0;

    for (grp_idx = 0; grp_idx < SOC_ESW_ECMP_MAX_GROUPS; grp_idx++) {
        grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx);

        if (grp_state->bank != bank_idx) {
            continue;
        }

        /* Group not in use. */
        if (!grp_state->bank && !grp_state->base && !grp_state->count) {
            continue;
        }

        /* Do it at the end */
        if (grp_idx == grp_idx_move) {
            continue;
        }

        rv = soc_esw_ecmp_protected_member_move(unit,
                                                grp_idx,
                                                bank_idx,
                                                SOC_ESW_ECMP_BANK_RSVD_GET(unit),
                                                ecmp1_tbl_p,
                                                ecmp2_tbl_p,
                                                dst_ecmp1_tbl_p,
                                                dst_ecmp2_tbl_p,
                                                new_base_idx);
        SOC_ESW_ECMP_IF_ERR_EXIT(rv);

        new_base_idx += grp_state->count;
    }

    if (grp_idx_move != -1) {
        grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_idx_move);

        rv = soc_esw_ecmp_protected_member_move(unit,
                                                grp_idx_move,
                                                bank_idx,
                                                SOC_ESW_ECMP_BANK_RSVD_GET(unit),
                                                ecmp1_tbl_p,
                                                ecmp2_tbl_p,
                                                dst_ecmp1_tbl_p,
                                                dst_ecmp2_tbl_p,
                                                new_base_idx);
        SOC_ESW_ECMP_IF_ERR_EXIT(rv);

        new_base_idx += grp_state->count;
    }

    /* Write updated member tables  */
    rv = soc_mem_write_range(unit, L3_ECMPm, MEM_BLOCK_ALL,
                             wr_start, wr_end, dst_ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_mem_write_range(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL,
                             wr_start, wr_end, dst_ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_ecmp_bank_active_set(unit, bank_idx);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* traverse group tables and update base pointers. */
    rv = soc_esw_ecmp_protected_base_ptr_update(unit);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_ecmp_bank_reserved_set(unit, bank_idx);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    if (free_count) {
        *free_count = (dst_bank_info->bank_size - new_base_idx);
    }

err_exit:
    if (ecmp2_tbl_p) {
        soc_cm_sfree(unit, ecmp2_tbl_p);
    }
    if (ecmp1_tbl_p) {
        soc_cm_sfree(unit, ecmp1_tbl_p);
    }

    if (dst_ecmp1_tbl_p) {
        soc_cm_sfree(unit, dst_ecmp1_tbl_p);
    }

    if (dst_ecmp2_tbl_p) {
        soc_cm_sfree(unit, dst_ecmp2_tbl_p);
    }
    return rv;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_ser_update
 * Purpose:
 *      Free the bank corresponding to index of errored ECMP memory.
 * Parameters:
 *      unit            - (IN)  SOC unit number.
 *      mem             - (IN)  SOC memory.
 *      ecmp_idx        - (IN)  ECMP memory index.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_ser_update(int unit,
                                  soc_mem_t mem,
                                  int ecmp_idx)
{
    int rv;
    int bank_idx;

    /* Bank update required only for INITIAL_L3_ECMP memory. */
    if (mem != INITIAL_L3_ECMPm) {
        return SOC_E_NONE;
    }

    /* get ECMP member bank idx */
    rv = soc_esw_ecmp_idx_bank_get(unit, ecmp_idx, &bank_idx);
    SOC_IF_ERROR_RETURN(rv);

    if (!SOC_ESW_ECMP_BANK_PROTECTED_ACCESS(unit, bank_idx)) {
        return SOC_E_NONE;
    }

    /* Call defragment function for the bank_idx. This does necessary shuffling. */
    rv = soc_esw_ecmp_protected_bank_defragment(unit, bank_idx, -1, NULL);

    return rv;
}

/*
 * Function:
 *      soc_esw_ecmp_bank_free_count_get
 * Purpose:
 *      Get the number of free entries in this bank.
 * Parameters:
 *      unit            - (IN)  SOC unit number.
 *      bank_idx        - (IN)  Bank index.
 *      count           - (OUT) Free entry count.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_bank_free_count_get(int unit, int bank_idx, int *count)
{
    int use_count = 0;
    soc_esw_ecmp_bank_state_t *bank_info;

    bank_info = SOC_ESW_ECMP_BANK_INFO(unit, bank_idx);

    SOC_IF_ERROR_RETURN(
        soc_esw_ecmp_bank_used_count_get(unit, bank_idx, &use_count));

    *count = (bank_info->bank_size - use_count);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_grp_add
 * Purpose:
 *      Add ecmp group next hop members, or reset ecmp group entry.
 *      NOTE: Function only updates the INITIAL_L3_ECMP and L3_ECMP tables only.
 *            Group tables and other associated tables are updated by the callee.
 *            Function may update group tables of other groups as part of this API though.
 * Parameters:
 *      unit        - (IN)SOC unit number.
 *      grp_info    - (IN)ecmp group id to write.
 *      member_list - (IN)Next hop indexes or NULL for entry reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_grp_add(int unit,
                               soc_esw_ecmp_group_info_t *grp_info,
                               void *member_list)
{
    int idx;                /* Iteration index.  */
    int rv;                 /* Operation return value. */
    int ecmpf;              /* Ecmp group vs NextHop Id indicator. */
    int nh_idx;             /* NextHop Idx or Ecmp Group. */
    int bank_idx;           /* Ecmp Member Table Bank. */
    int bank_idx_r;         /* Reserved ecmp member bank. */
    int ecmp_idx;           /* Ecmp table entry index. */
    int ecmp_base_offset;   /* Ecmp table entry index. */
    int max_grp_size;       /* Maximum ecmp group size. */
    int rd_start, rd_end;   /* Read idx range. */
    int wr_start, wr_end;   /* Write idx range. */
    int bank_swap_reqd;     /* Bank Swap needed  */

    uint8 *ecmp1_tbl_p = NULL;         /* L3_ECMP table pointer. */
    uint8 *ecmp2_tbl_p = NULL; /* INITIAL_L3_ECMP table pointer. */
    uint32 *ecmp1_entry_ptr = NULL;         /* ECMP entry pointer.*/
    uint32 *ecmp2_entry_ptr = NULL; /* INITIAL ECMP entry pointer.*/
    soc_esw_ecmp_member_info_t *ecmp_members;  /* Ecmp group member info.  */
    soc_esw_ecmp_group_state_t *grp_state; /* ECMP group info. */
    void *ecmp1_null_entry = NULL, *ecmp2_null_entry = NULL;

    /* Cast input buffer. */
    ecmp_members = member_list;

    bank_idx_r = SOC_ESW_ECMP_BANK_RSVD_GET(unit);

    max_grp_size = grp_info->max_paths;
    ecmp_idx = grp_info->ecmp_member_base;
    ecmp_base_offset = ecmp_idx;
    grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_info->ecmp_grp);

    if (grp_state->count != 0) {
        if (max_grp_size > grp_state->count) {
            rv = soc_esw_ecmp_protected_grp_size_update(unit,
                                                        grp_info,
                                                        &ecmp_idx);
            SOC_ESW_ECMP_IF_ERR_EXIT(rv);
            ecmp_base_offset = ecmp_idx;
        }
    }

    /* get ECMP member bank idx */
    rv = soc_esw_ecmp_idx_bank_get(unit, ecmp_idx, &bank_idx);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_ecmp_protected_idx_range_get(unit, bank_idx, bank_idx_r,
                                              ecmp_idx, max_grp_size,
                                              &rd_start, &rd_end,
                                              &wr_start, &wr_end);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Bank swap required if read and write ranges different */
    bank_swap_reqd = ((wr_start != rd_start) ? 1 : 0);

    rv = soc_esw_ecmp_idx_bank_offset_get(unit, &ecmp_base_offset);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Read ecmp member tables */
    rv = soc_esw_mem_dma_read(unit, L3_ECMPm,
                              sizeof(ecmp_entry_t),
                              "ecmp_tbl",
                              rd_start, rd_end,
                              &ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_esw_mem_dma_read(unit, INITIAL_L3_ECMPm,
                              sizeof(initial_l3_ecmp_entry_t),
                              "ecmp2_tbl",
                              rd_start, rd_end,
                              &ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    ecmp1_null_entry = soc_mem_entry_null(unit, L3_ECMPm);
    ecmp2_null_entry = soc_mem_entry_null(unit, INITIAL_L3_ECMPm);

    /* Populate new nexthop indices in the member tables. */
    for (idx = ecmp_base_offset, nh_idx = 0;
         idx < (ecmp_base_offset + max_grp_size);
         idx++, nh_idx++) {

         /* Set next hop index. */
         ecmp1_entry_ptr =
             soc_mem_table_idx_to_pointer(unit, L3_ECMPm, uint32 *,
                                          ecmp1_tbl_p, idx);
         ecmp2_entry_ptr =
             soc_mem_table_idx_to_pointer(unit, INITIAL_L3_ECMPm, uint32 *,
                                          ecmp2_tbl_p, idx);

         sal_memcpy(ecmp1_entry_ptr, ecmp1_null_entry,
                    sizeof(ecmp_entry_t));
         sal_memcpy(ecmp2_entry_ptr, ecmp2_null_entry,
                    sizeof(initial_l3_ecmp_entry_t));

        ecmpf = 0;
        soc_esw_nh_if_ecmp_grp_decode(unit, &(ecmp_members[nh_idx].nh), &ecmpf);

        /* Set ecmp flag if member is an ecmp group */
        if (ecmpf == 1) {
            soc_mem_field32_set(unit, L3_ECMPm,
                                ecmp1_entry_ptr, ECMP_FLAGf, 1);
            soc_mem_field32_set(unit, INITIAL_L3_ECMPm,
                                ecmp2_entry_ptr, ECMPf, 1);

            soc_mem_field32_dest_set(unit, L3_ECMPm,
                                     ecmp1_entry_ptr,
                                     DESTINATIONf, SOC_MEM_FIF_DEST_ECMP, 
                                     ecmp_members[nh_idx].nh);
            soc_mem_field32_dest_set(unit, INITIAL_L3_ECMPm,
                                     ecmp2_entry_ptr,
                                     DESTINATIONf, SOC_MEM_FIF_DEST_ECMP, 
                                     ecmp_members[nh_idx].nh);
        } else {
            soc_mem_field32_dest_set(unit, L3_ECMPm,
                                     ecmp1_entry_ptr,
                                     DESTINATIONf, SOC_MEM_FIF_DEST_NEXTHOP, 
                                     ecmp_members[nh_idx].nh);
            soc_mem_field32_dest_set(unit, INITIAL_L3_ECMPm,
                                     ecmp2_entry_ptr,
                                     DESTINATIONf, SOC_MEM_FIF_DEST_NEXTHOP, 
                                     ecmp_members[nh_idx].nh);
        }

        if (grp_info->vp_lag == 1) {
            soc_mem_field32_set(unit, L3_ECMPm,
                                ecmp1_entry_ptr, DVPf,
                                ecmp_members[nh_idx].dvp);
        }

        soc_mem_field32_set(unit, INITIAL_L3_ECMPm,
                            ecmp2_entry_ptr,
                            PROT_NEXT_HOP_INDEXf,
                            ecmp_members[nh_idx].prot_nh);

        soc_mem_field32_set(unit, INITIAL_L3_ECMPm,
                            ecmp2_entry_ptr,
                            PROT_GROUPf,
                            ecmp_members[nh_idx].prot_grp);
    }

    /* Write updated member tables  */
    rv = soc_mem_write_range(unit, L3_ECMPm, MEM_BLOCK_ALL,
                             wr_start, wr_end, ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    rv = soc_mem_write_range(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL,
                             wr_start, wr_end, ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* If the group is being updated and index has moved, free old index. */
    rv = soc_esw_ecmp_protected_idx_free(unit,
                                         grp_state->bank,
                                         grp_state->base,
                                         grp_state->count);
    SOC_IF_ERROR_RETURN(rv);

    /* Update ECMP group's base ptr */
    grp_info->ecmp_member_base = (ecmp_idx + (wr_start - rd_start));

    /* Update ECMP member state */
    rv = soc_esw_ecmp_protected_idx_alloc(unit,
                                          (bank_swap_reqd ? bank_idx_r : bank_idx),
                                          grp_info->ecmp_member_base,
                                          grp_info->max_paths);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Update ECMP group state */
    rv = soc_esw_ecmp_protected_state_set(unit,
                                          grp_info->ecmp_grp,
                                          (bank_swap_reqd ? bank_idx_r : bank_idx),
                                          grp_info->ecmp_member_base,
                                          grp_info->max_paths);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    if (bank_swap_reqd) {
        /* Update other groups with members in this bank */
        rv = soc_esw_ecmp_protected_bank_update(unit, bank_idx, bank_idx_r);
        SOC_ESW_ECMP_IF_ERR_EXIT(rv);
    }

err_exit:
    if (ecmp2_tbl_p) {
        soc_cm_sfree(unit, ecmp2_tbl_p);
    }
    if (ecmp1_tbl_p) {
        soc_cm_sfree(unit, ecmp1_tbl_p);
    }


    return rv;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_grp_delete
 * Purpose:
 *      Delete ECMP members
 *      NOTE: Function does not update the INITIAL_L3_ECMP and L3_ECMP tables.
 *            Group tables need to be updated to reflect group delete.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      grp_info   - (IN)ecmp group id to write.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_grp_delete(int unit,
                                  soc_esw_ecmp_group_info_t *grp_info)
{
    int rv;
    soc_esw_ecmp_group_state_t *grp_state;

    grp_state = SOC_ESW_ECMP_GROUP_INFO(unit, grp_info->ecmp_grp);

    /* Update ECMP member state */
    rv = soc_esw_ecmp_protected_idx_free(unit,
                                         grp_state->bank,
                                         grp_state->base,
                                         grp_state->count);
    SOC_IF_ERROR_RETURN(rv);

    /* Update ECMP group state */
    rv = soc_esw_ecmp_protected_state_set(unit,
                                          grp_info->ecmp_grp,
                                          0,
                                          0,
                                          0);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_ecmp_protected_grp_get
 * Purpose:
 *      Fetch ecmp members info for a given group
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      grp_info   - (IN)ecmp group id to write.
 *      mem_list   - (OUT)Next hop indexes or NULL for entry reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_ecmp_protected_grp_get(int unit,
                               soc_esw_ecmp_group_info_t *grp_info,
                               void *mem_list)
{
    int max_grp_size;  /* Maximum ecmp group size.*/
    int ecmp_idx;      /* Ecmp table entry index. */
    int nh_idx;        /* NextHop Idx or Ecmp Group */
    int bank_idx;      /* Ecmp Member Table Bank */
    int idx;           /* Iteration index.  */
    int rv;            /* Operation return value. */

    uint8 *ecmp1_tbl_p = NULL;         /* Dma L3 ECMP table pointer. */
    uint8 *ecmp2_tbl_p = NULL;         /* Dma L3 ECMP table pointer. */
    uint32 *ecmp1_entry_ptr = NULL;    /* ECMP entry pointer.*/
    uint32 *ecmp2_entry_ptr = NULL;    /* ECMP entry pointer.*/
    uint32 dest_val;
    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;
    soc_esw_ecmp_member_info_t *ecmp_members;/* Ecmp group nh indexes.  */

    /* Get ECMP group state */
    rv = soc_esw_ecmp_protected_state_get(unit,
                                          grp_info->ecmp_grp,
                                          &bank_idx,
                                          &ecmp_idx,
                                          &max_grp_size);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Cast input buffer. */
    ecmp_members = mem_list;

    /* Read ecmp member tables */
    rv = soc_esw_mem_dma_read(unit, L3_ECMPm,
                              sizeof(ecmp_entry_t),
                              "ecmp_tbl",
                              ecmp_idx,
                              (ecmp_idx + max_grp_size - 1),
                              &ecmp1_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Read ecmp member tables */
    rv = soc_esw_mem_dma_read(unit, INITIAL_L3_ECMPm,
                              sizeof(ecmp_entry_t),
                              "initial_ecmp_tbl",
                              ecmp_idx,
                              (ecmp_idx + max_grp_size - 1),
                              &ecmp2_tbl_p);
    SOC_ESW_ECMP_IF_ERR_EXIT(rv);

    /* Populate new nexthop indices from the member tables. */
    for (idx = 0, nh_idx = 0; idx < max_grp_size; idx++, nh_idx++) {
        /* Set next hop index. */
        ecmp1_entry_ptr =
            soc_mem_table_idx_to_pointer(unit, L3_ECMPm, uint32 *,
                                         ecmp1_tbl_p, idx);
        ecmp2_entry_ptr =
            soc_mem_table_idx_to_pointer(unit, INITIAL_L3_ECMPm, uint32 *,
                                         ecmp2_tbl_p, idx);

        dest_val = soc_mem_field32_dest_get(unit, L3_ECMPm, ecmp1_entry_ptr,
                                            DESTINATIONf, &dest_type);
        soc_esw_nh_if_ecmp_grp_encode(unit, &(dest_val),
                                      (dest_type == SOC_MEM_FIF_DEST_ECMP));
        ecmp_members[nh_idx].nh = dest_val;

        if (grp_info->vp_lag == 1) {
            ecmp_members[nh_idx].dvp = soc_mem_field32_get(unit, L3_ECMPm,
                                                           ecmp1_entry_ptr, DVPf);
        }

        ecmp_members[nh_idx].prot_nh = soc_mem_field32_get(unit, INITIAL_L3_ECMPm,
                                                           ecmp2_entry_ptr,
                                                           PROT_NEXT_HOP_INDEXf);
        ecmp_members[nh_idx].prot_grp = soc_mem_field32_get(unit, INITIAL_L3_ECMPm,
                                                           ecmp2_entry_ptr,
                                                           PROT_GROUPf);
    }

err_exit:
    if (ecmp1_tbl_p) {
        soc_cm_sfree(unit, ecmp1_tbl_p);
    }

    if (ecmp2_tbl_p) {
        soc_cm_sfree(unit, ecmp2_tbl_p);
    }
    return rv;
}

#else
typedef int _include_soc_ecmp_not_empty; /* Make ISO compilers happy. */
#endif /* SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

