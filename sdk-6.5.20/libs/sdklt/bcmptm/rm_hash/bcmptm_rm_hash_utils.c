/*! \file bcmptm_rm_hash_utils.c
 *
 * Util functions for hash resource manager
 *
 * This file contains utility functions for hash resource manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_utils.h"
#include "bcmptm_rm_hash_pt_register.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Typedefs
 */
/*
 *! \brief Hardware logical bucket to software logical bucket mapping
 *
 * This data structure describes the information of how a hardware logical
 * bucket maps to the software logical bucket.
 */
typedef struct rm_hash_hlb_slb_map_s {
    /*! \brief Divisior to divide the hlb */
    uint8_t divisor;

    /*! \brief Multiplier to the remainder */
    uint8_t shft_multip;

    /*! \brief Valid base bucket mask */
    uint8_t vbb_mask;
} rm_hash_hlb_slb_map_t;

/*
 *! \brief Base bucket bitmap to base bucket offset mapping
 *
 * This data structure describes the base bucket offset within a software
 * logical bucket.
 */
typedef struct rm_hash_base_bkt_offset_s {
    /*! \brief Valid or not */
    bool valid;

    /*! \brief offset of base bucket within the software logical bucket */
    uint8_t offset;
} rm_hash_base_bkt_offset_t;

/*
 *! \brief Base bucket bitmap to base bucket offset mapping
 *
 * This data structure describes the base bucket offset within a software
 * logical bucket.
 */
typedef struct rm_hash_base_ent_offset_s {
    /*! \brief Valid or not */
    bool valid;

    /*! \brief offset of base entry within the base bucket */
    uint8_t offset;
} rm_hash_base_ent_offset_t;

/*******************************************************************************
 * Private variables
 */
static rm_hash_hlb_slb_map_t bmn_ent_bmn_bkt_map = {0x01, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bmn_ent_bm1_bkt_map = {0x02, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm0_ent_bm0_bkt_map = {0x01, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm0_ent_bm1_bkt_map = {0x02, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm0_ent_bm2_bkt_map = {0x04, 0x01, 0x0001};

static rm_hash_hlb_slb_map_t bm1_ent_bm1_bkt_map = {0x01, 0x02, 0x0003};

static rm_hash_hlb_slb_map_t bm1_ent_bm2_bkt_map = {0x02, 0x02, 0x0003};

static rm_hash_hlb_slb_map_t bm2_ent_bm2_bkt_map = {0x01, 0x04, 0x000F};

static rm_hash_hlb_slb_map_t *hlb_to_slb_map_info[RM_HASH_BM_CNT][RM_HASH_BM_CNT] = {
{&bmn_ent_bmn_bkt_map, NULL, NULL, NULL},
{NULL, &bm0_ent_bm0_bkt_map, NULL, NULL},
{&bmn_ent_bm1_bkt_map, &bm0_ent_bm1_bkt_map, &bm1_ent_bm1_bkt_map, NULL},
{NULL, &bm0_ent_bm2_bkt_map, &bm1_ent_bm2_bkt_map, &bm2_ent_bm2_bkt_map}
};

static rm_hash_base_bkt_offset_t bb_bmp_to_offset_map[] = {
{FALSE, 0}, /* 0000 */
{TRUE,  0}, /* 0001 */
{TRUE,  1}, /* 0010 */
{FALSE, 0}, /* 0011 */
{TRUE,  2}, /* 0100 */
{FALSE, 0}, /* 0101 */
{FALSE, 0}, /* 0110 */
{FALSE, 0}, /* 0111 */
{TRUE,  3}, /* 1000 */
};

static rm_hash_base_ent_offset_t be_bmp_to_offset_map[] = {
{FALSE,  0}, /* 0000 */
{TRUE,   0}, /* 0001 */
{TRUE,   1}, /* 0010 */
{TRUE,   0}, /* 0011 */
{TRUE,   2}, /* 0100 */
{FALSE,  0}, /* 0101 */
{FALSE,  0}, /* 0110 */
{FALSE,  0}, /* 0111 */
{TRUE,   3}, /* 1000 */
{FALSE,  0}, /* 1001 */
{FALSE,  0}, /* 1010 */
{FALSE,  0}, /* 1011 */
{TRUE,   2}, /* 1100 */
{FALSE,  0}, /* 1101 */
{FALSE,  0}, /* 1110 */
{TRUE,   0}, /* 1111 */
};

static rm_hash_bm_t rm_hash_dynamic_bkt_mode[RM_HASH_BM_CNT] = {
    RM_HASH_BM_0,
    RM_HASH_BM_1,
    RM_HASH_BM_2,
    RM_HASH_BM_CNT
    };
/*******************************************************************************
 * Private Functions
 */
static int
rm_hash_data_fields_get(int unit,
                        bcmptm_rm_hash_lt_info_t *lt_info,
                        uint8_t grp_idx,
                        uint16_t ldtype,
                        const bcmlrd_hw_field_list_t **data_fields)
{
    const bcmptm_rm_hash_lt_data_info_t *lt_data_info = NULL;
    const bcmptm_rm_hash_grp_data_info_t *grp_data_info = NULL;
    uint8_t idx;

    SHR_FUNC_ENTER(unit);

    lt_data_info = lt_info->enh_more_info->lt_data_info;
    if (lt_data_info == NULL) {
        /*
         * This LT has only one data format. And there is no physical
         * data type.
         */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    grp_data_info = lt_data_info->grp_data_info + grp_idx;
    for (idx = 0; idx < grp_data_info->num_ldtype; idx++) {
        if (ldtype == grp_data_info->ldtype_val[idx]) {
            *data_fields = &grp_data_info->hw_data_fields[idx];
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();

}

static void
rm_hash_fields_exchange(int unit,
                        const bcmlrd_hw_field_list_t *flds_a,
                        const bcmlrd_hw_field_list_t *flds_b,
                        uint32_t *entry_a,
                        uint32_t *entry_b)
{
    int sbit_a, ebit_a, sbit_b, ebit_b;
    uint8_t idx;
    uint32_t fld_val[BCMPTM_MAX_PT_FIELD_WORDS];

    for (idx = 0; idx < flds_a->num_fields; idx++) {
        sal_memset(fld_val, 0, sizeof(fld_val));
        sbit_a = flds_a->field_start_bit[idx];
        ebit_a = flds_a->field_start_bit[idx] + flds_a->field_width[idx] - 1;
        bcmdrd_field_get(entry_a, sbit_a, ebit_a, fld_val);
        sbit_b = flds_b->field_start_bit[idx];
        ebit_b = flds_b->field_start_bit[idx] + flds_b->field_width[idx] - 1;
        bcmdrd_field_set(entry_b, sbit_b, ebit_b, fld_val);
    }
}

static int
rm_hash_dual_base_ent_offset_get(int unit,
                                 uint8_t ent_bmp,
                                 uint8_t base_bkt_size,
                                 uint8_t *ent_offset)
{
    uint8_t nibble = 0;
    rm_hash_base_ent_offset_t be_offset_hi;
    rm_hash_base_ent_offset_t be_offset_lo;

    SHR_FUNC_ENTER(unit);

    if ((base_bkt_size == RM_HASH_WIDE_DUAL_HASH_BKT_SIZE)) {
        nibble = ent_bmp & 0x0F;
        be_offset_lo = be_bmp_to_offset_map[nibble];
        nibble = (ent_bmp >> 4) & 0x0F;
        be_offset_hi = be_bmp_to_offset_map[nibble];

        if (((be_offset_lo.valid == FALSE) && (be_offset_hi.valid == FALSE)) ||
            ((be_offset_lo.valid == TRUE) && (be_offset_hi.valid == TRUE))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (be_offset_lo.valid == TRUE) {
            *ent_offset = be_offset_lo.offset;
        }
        if (be_offset_hi.valid == TRUE) {
            *ent_offset = be_offset_hi.offset + 4;
        }
    } else {
        if ((ent_bmp >= COUNTOF(be_bmp_to_offset_map)) ||
            (be_bmp_to_offset_map[ent_bmp].valid == FALSE)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        *ent_offset = be_bmp_to_offset_map[ent_bmp].offset;
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_bkt_mode_mapping(int unit,
                                uint8_t hw_bkt_mode_val,
                                rm_hash_bm_t *e_bm)
{
    SHR_FUNC_ENTER(unit);

    if (hw_bkt_mode_val > 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *e_bm = rm_hash_dynamic_bkt_mode[hw_bkt_mode_val];

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_key_type_fill(int unit,
                             uint16_t key_type,
                             const bcmptm_rm_hash_view_info_t *view_info,
                             uint32_t *e_words)
{
    uint8_t idx;
    uint32_t field = key_type;
    const bcmlrd_hw_rfield_info_t *ktype_info = view_info->hw_ktype_field_info;

    /*
     * There may be one or multiple KEY_TYPE fields in entry buffer. PTRM
     * must retrieve the corresponding KEY_TYPE value from LRD and fill
     * KEY_TYPE fields in the buffer.
     */
    for (idx = 0; idx < ktype_info->num_reps; idx++) {
        int sbit, ebit;
        sbit = ktype_info->field_start_bit[idx];
        ebit = sbit + ktype_info->field_width - 1;
        bcmdrd_field_set(e_words, sbit, ebit, &field);
    }
}

void
bcmptm_rm_hash_data_type_fill(int unit,
                              uint16_t pdtype_type,
                              const bcmptm_rm_hash_view_info_t *view_info,
                              uint32_t *e_words)
{
    uint8_t idx;
    uint32_t field[BCMPTM_MAX_PT_FIELD_WORDS] = {0};
    const bcmlrd_hw_rfield_info_t *dtype_info = view_info->hw_dtype_field_info;

    field[0] = pdtype_type;
    for (idx = 0; idx < dtype_info->num_reps; idx++) {
        int sbit, ebit;
        sbit = dtype_info->field_start_bit[idx];
        ebit = sbit + dtype_info->field_width - 1;
        bcmdrd_field_set(e_words, sbit, ebit, field);
    }
}

void
bcmptm_rm_hash_ent_sig_generate(int unit,
                                uint32_t *e_words,
                                const bcmlrd_hw_field_list_t *key_fields,
                                uint32_t *e_sig)
{
    int sbit, ebit, idx, num_words;
    uint32_t key_val[BCMPTM_MAX_PT_FIELD_WORDS];

    sbit = key_fields->field_start_bit[0];
    ebit = key_fields->field_start_bit[0] + key_fields->field_width[0] - 1;
    num_words = key_fields->field_width[0] / 32 + 1;
    key_val[num_words - 1] = 0;
    bcmdrd_field_get(e_words, sbit, ebit, key_val);

    *e_sig = 0;
    for (idx = 0; idx < num_words; idx++) {
        *e_sig += key_val[idx];
    }
}

int
bcmptm_rm_hash_hlb_to_slb_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              rm_hash_inst_srch_info_t *inst_srch_info)
{
    uint8_t rbank = 0, idx = 0, vbb_bmp = 0, num_en_rbanks, *en_rbank;
    rm_hash_hlb_slb_map_t *map = NULL;
    uint32_t hlb = 0, slb_offset = 0;
    rm_hash_bm_t t_bm = pt_info->t_bm;

    SHR_FUNC_ENTER(unit);

    map = hlb_to_slb_map_info[t_bm][inst_srch_info->e_bm];
    if (map == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    num_en_rbanks = inst_srch_info->fmt_info->num_en_rbanks;
    en_rbank = inst_srch_info->fmt_info->en_rbank_list;

    for (idx = 0; idx < num_en_rbanks; idx++) {
        rbank = en_rbank[idx];
        hlb = inst_srch_info->hlb_list[idx];
        slb_offset = hlb / map->divisor;
        vbb_bmp = map->vbb_mask << (map->shft_multip * (hlb % map->divisor));
        inst_srch_info->slb_info_list[idx].slb = pt_info->rbank_slb_base[rbank] +
                                                slb_offset;
        inst_srch_info->slb_info_list[idx].vbb_bmp = vbb_bmp;
        inst_srch_info->slb_info_list[idx].map = 0;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                              "software logical bucket on rbank[%d]:[%d], "
                               "valid base bucket bitmap:[%d]\n"),
                               rbank,
                               inst_srch_info->slb_info_list[idx].slb,
                               inst_srch_info->slb_info_list[idx].vbb_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_slb_to_rbank_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint32_t slb,
                                uint8_t *rbank)
{
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < pt_info->num_rbanks; idx++) {
        if (slb < pt_info->rbank_num_slbs[idx]) {
            /* This register bank has been enabled for this LT. */
            *rbank = idx;
            SHR_EXIT();
        } else {
            slb -= pt_info->rbank_num_slbs[idx];
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_index_get(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint32_t slb,
                             rm_hash_ent_loc_t *e_loc,
                             uint8_t e_size,
                             uint8_t *prbank,
                             uint32_t *e_index)
{
    uint32_t be_idx;
    uint32_t lbank_size, pbkt, slb_offset = 0;
    uint8_t rbank = 0, rbank_cnt, bb_size, bb_bmp, bb_offset, be_bmp, be_offset = 0;
    bcmdrd_sid_t sid;
    uint8_t ratio, bank;
    uint8_t rbank_list[RM_HASH_SHR_MAX_BANK_COUNT], base_rbank;

    SHR_FUNC_ENTER(unit);

    if (prbank != NULL) {
        rbank = *prbank;
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_slb_to_rbank_get(unit, pt_info, slb, &rbank));
    }
    bb_size = pt_info->base_bkt_size[rbank];
    if (pt_info->bank_separate_sid == FALSE) {
        sid = pt_info->rbank_sid[rbank];
        bcmptm_rm_hash_sid_rbank_list_get(unit, sid, pt_info, rbank_list,
                                          &rbank_cnt);
        base_rbank = rbank_list[0];
        slb_offset = slb - pt_info->rbank_slb_base[base_rbank];
        /* For some older devices, all the banks form a single SID. */
        switch (pt_info->hash_type[rbank]) {
        case BCMPTM_RM_HASH_TYPE_SHARED:
            bb_bmp = e_loc->bb_bmp;
            if ((bb_bmp >= COUNTOF(bb_bmp_to_offset_map)) ||
                (bb_bmp_to_offset_map[bb_bmp].valid == FALSE)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            bb_offset = bb_bmp_to_offset_map[bb_bmp].offset;
            be_bmp = e_loc->be_bmp;
            if ((be_bmp >= COUNTOF(be_bmp_to_offset_map)) ||
                (be_bmp_to_offset_map[be_bmp].valid == FALSE)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            be_offset = be_bmp_to_offset_map[be_bmp].offset;
            /*
             * For all the physical non-dual hash tables so far, the size of
             * a base bucket (number of base entries) are always 4.
             */
            be_idx = pt_info->slb_size * slb_offset;
            be_idx = be_idx + bb_offset * bb_size + be_offset;
            *e_index = be_idx / e_size;
            break;
        case BCMPTM_RM_HASH_TYPE_DUAL:
            /*
             * It is assumed that dual hash table has identical bank
             * size for its two banks. And for dual hash tables, only legacy
             * BM0 mode is supported.
             */
            bb_bmp = e_loc->bb_bmp;
            if ((bb_bmp >= COUNTOF(bb_bmp_to_offset_map)) ||
                (bb_bmp_to_offset_map[bb_bmp].valid == FALSE)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            bb_offset = bb_bmp_to_offset_map[bb_bmp].offset;

            lbank_size = pt_info->rbank_num_slbs[rbank];
            ratio = pt_info->slb_size / 4;
            lbank_size *= ratio;
            if (slb_offset >= pt_info->rbank_num_slbs[base_rbank]) {
                bank = 1;
            } else {
                bank = 0;
            }
            slb_offset = slb_offset * ratio + bb_offset;
            pbkt = slb_offset % lbank_size;
            be_bmp = e_loc->be_bmp;
            SHR_IF_ERR_EXIT
                (rm_hash_dual_base_ent_offset_get(unit,
                                                  be_bmp,
                                                  bb_size,
                                                  &be_offset));
            be_idx = pbkt * bb_size + bank * (bb_size / 2) + be_offset;
            *e_index = be_idx / e_size;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        bb_bmp = e_loc->bb_bmp;
        if ((bb_bmp >= COUNTOF(bb_bmp_to_offset_map)) ||
            (bb_bmp_to_offset_map[bb_bmp].valid == FALSE)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        bb_offset = bb_bmp_to_offset_map[bb_bmp].offset;
        be_bmp = e_loc->be_bmp;
        if ((be_bmp >= COUNTOF(be_bmp_to_offset_map)) ||
            (be_bmp_to_offset_map[be_bmp].valid == FALSE)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        be_offset = be_bmp_to_offset_map[be_bmp].offset;
        be_idx = pt_info->slb_size * (slb - pt_info->rbank_slb_base[rbank]);
        be_idx = be_idx + bb_offset * bb_size + be_offset;
        *e_index = be_idx / e_size;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_bkt_mode_get(int unit,
                                const bcmptm_rm_hash_key_format_t *key_format,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                rm_hash_bm_t *e_bm,
                                rm_hash_entry_narrow_mode_t *e_nm)
{
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Bucket mode is defined on per key type basis, if there is no explicit
     * key type definition for a hash table, the default index 0 will be used.
     */
    if (key_format->key_type_val != NULL) {
        idx = key_format->key_type_val[0];
    }
    if ((idx >= RM_HASH_MAX_KEY_TYPE_COUNT) ||
        (pt_info->key_attrib[rbank].attrib[idx].valid == FALSE)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *e_bm = pt_info->key_attrib[rbank].attrib[idx].bm;
    *e_nm = pt_info->key_attrib[rbank].attrib[idx].enm;

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_key_format_sids_get(int unit,
                                   const bcmptm_rm_hash_view_info_t *bview_info,
                                   const bcmptm_rm_hash_key_format_t *key_fmt,
                                   bcmdrd_sid_t *sid_list,
                                   uint8_t *sid_cnt)
{
    uint8_t idx, v_idx;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    const bcmptm_rm_hash_valid_view_info_t *valid_view_info = NULL;
    valid_view_info = key_fmt->valid_view_info;

    for (idx = 0; idx < valid_view_info->view_index_count; idx++) {
        v_idx = valid_view_info->view_index[idx];
        view_info = bview_info + v_idx;
        sid_list[idx] = *view_info->sid;
    }
    *sid_cnt = valid_view_info->view_index_count;
}

void
bcmptm_rm_hash_key_format_view_list_get(int unit,
                                        const bcmptm_rm_hash_view_info_t *bview_info,
                                        const bcmptm_rm_hash_key_format_t *key_fmt,
                                        const bcmptm_rm_hash_view_info_t **view_list,
                                        uint8_t *view_cnt)
{
    uint8_t idx, v_idx;
    const bcmptm_rm_hash_view_info_t *view_info = NULL;
    const bcmptm_rm_hash_valid_view_info_t *valid_view_info = NULL;
    valid_view_info = key_fmt->valid_view_info;

    for (idx = 0; idx < valid_view_info->view_index_count; idx++) {
        v_idx = valid_view_info->view_index[idx];
        view_info = bview_info + v_idx;
        view_list[idx] = view_info;
    }
    *view_cnt = valid_view_info->view_index_count;
}

void
bcmptm_rm_hash_lt_map_grp_cnt_get(int unit,
                                  bcmptm_rm_hash_lt_info_t *lt_info,
                                  uint8_t *grp_cnt)
{
    const bcmptm_rm_hash_lt_key_info_t *lt_key_info = NULL;

    lt_key_info = lt_info->enh_more_info->lt_key_info;

    *grp_cnt = lt_key_info->grp_key_info_count;
}

int
bcmptm_rm_hash_grp_sid_get(int unit,
                           bcmdrd_sid_t sid,
                           bcmdrd_sid_t *grp_sid)
{
    const bcmptm_overlay_info_t *ovly_info = NULL;

    SHR_FUNC_ENTER(unit);

    *grp_sid = sid;
    SHR_IF_ERR_EXIT
        (bcmptm_pt_overlay_info_get(unit, sid, &ovly_info));
    if ((ovly_info->mode != BCMPTM_OINFO_MODE_NONE)) {
        SHR_IF_ERR_EXIT
            (ovly_info->mode != BCMPTM_OINFO_MODE_HASH ?
                                SHR_E_INTERNAL : SHR_E_NONE);
        *grp_sid = ovly_info->w_sid;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_view_info_get(int unit,
                                 bcmptm_rm_hash_lt_info_t *lt_info,
                                 uint8_t grp_idx,
                                 bcmdrd_sid_t sid,
                                 const bcmptm_rm_hash_view_info_t **view_info)
{
    uint8_t view_idx, view_count = 0;
    const bcmptm_rm_hash_grp_view_info_t *base_grp_view_info = NULL;
    const bcmptm_rm_hash_grp_view_info_t *grp_view_info = NULL;
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;

    SHR_FUNC_ENTER(unit);

    base_grp_view_info = lt_info->enh_more_info->lt_view_info->grp_view_info;
    grp_view_info = base_grp_view_info + grp_idx;
    base_view_info = grp_view_info->view_info;
    view_count = grp_view_info->view_info_count;

    for (view_idx = 0; view_idx < view_count; view_idx++) {
        if ((base_view_info + view_idx)->sid[0] == sid) {
            *view_info = base_view_info + view_idx;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_size_based_view_info_get(int unit,
                                        bcmptm_rm_hash_lt_info_t *lt_info,
                                        uint8_t grp_idx,
                                        const bcmptm_rm_hash_key_format_t *key_format,
                                        uint8_t e_size,
                                        const bcmptm_rm_hash_view_info_t **view_info)
{
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL, *view = NULL;
    const bcmptm_rm_hash_view_info_t *view_info_list[RM_HASH_MAX_HW_ENTRY_INFO_COUNT] = {NULL};
    uint8_t view_cnt = 0, view_idx;

    SHR_FUNC_ENTER(unit);

    /* The view info needs to be determined based on input of caller. */
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_base_view_info_get(unit,
                                           lt_info,
                                           grp_idx,
                                           &base_view_info));

    bcmptm_rm_hash_key_format_view_list_get(unit,
                                            base_view_info,
                                            key_format,
                                            view_info_list,
                                            &view_cnt);
    for (view_idx = 0; view_idx < view_cnt; view_idx++) {
        view = view_info_list[view_idx];
        if (view->num_base_entries == e_size) {
            *view_info = view;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_key_format_view_info_get(int unit,
                                        bcmptm_rm_hash_lt_info_t *lt_info,
                                        uint8_t grp_idx,
                                        const bcmptm_rm_hash_key_format_t *key_format,
                                        uint8_t view_idx,
                                        const bcmptm_rm_hash_view_info_t **view_info)
{
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;
    const bcmptm_rm_hash_valid_view_info_t *valid_view_info = NULL;
    const bcmptm_rm_hash_grp_view_info_t *grp_view_info = NULL;
    const bcmptm_rm_hash_grp_view_info_t *base_grp_view_info = NULL;

    SHR_FUNC_ENTER(unit);

    base_grp_view_info = lt_info->enh_more_info->lt_view_info->grp_view_info;
    grp_view_info = base_grp_view_info + grp_idx;
    base_view_info = grp_view_info->view_info;

    valid_view_info = key_format->valid_view_info;
    if (view_idx > valid_view_info->view_index_count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *view_info = base_view_info + valid_view_info->view_index[view_idx];

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_base_view_info_get(int unit,
                                  bcmptm_rm_hash_lt_info_t *lt_info,
                                  uint8_t grp_idx,
                                  const bcmptm_rm_hash_view_info_t **bview_info)
{
    const bcmptm_rm_hash_grp_view_info_t *base_grp_view_info = NULL;
    const bcmptm_rm_hash_grp_view_info_t *grp_view_info = NULL;
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;

    SHR_FUNC_ENTER(unit);

    base_grp_view_info = lt_info->enh_more_info->lt_view_info->grp_view_info;
    grp_view_info = base_grp_view_info + grp_idx;
    base_view_info = grp_view_info->view_info;
    *bview_info = base_view_info;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_grp_key_info_get(int unit,
                                bcmptm_rm_hash_lt_info_t *lt_info,
                                uint8_t grp_idx,
                                const bcmptm_rm_hash_grp_key_info_t **key_info)
{
    const bcmptm_rm_hash_grp_key_info_t *base_grp_key_info = NULL;

    SHR_FUNC_ENTER(unit);

    base_grp_key_info = lt_info->enh_more_info->lt_key_info->grp_key_info;
    *key_info  = base_grp_key_info + grp_idx;

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_pdtype_get(int unit,
                          bcmptm_rm_hash_lt_info_t *lt_info,
                          uint8_t grp_idx,
                          uint8_t ldtype,
                          uint16_t *pdtype)
{
    const bcmptm_rm_hash_lt_data_info_t *lt_data_info = NULL;
    const bcmptm_rm_hash_grp_data_info_t *grp_data_info = NULL;
    uint8_t idx;

    SHR_FUNC_ENTER(unit);

    lt_data_info = lt_info->enh_more_info->lt_data_info;
    if (lt_data_info == NULL) {
        /*
         * This LT has only one data format. And there is no physical
         * data type.
         */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    grp_data_info = lt_data_info->grp_data_info + grp_idx;
    for (idx = 0; idx < grp_data_info->num_ldtype; idx++) {
        if (ldtype == grp_data_info->ldtype_val[idx]) {
            *pdtype = grp_data_info->pdtype_val[idx];
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();

}

int
bcmptm_rm_hash_key_format_get(int unit,
                              bcmptm_rm_hash_entry_attrs_t *e_attrs,
                              bcmptm_rm_hash_lt_info_t *lt_info,
                              uint8_t grp_idx,
                              const bcmptm_rm_hash_key_format_t **key_fmt)
{
    uint8_t fmt_idx, fmt_cnt;
    const bcmptm_rm_hash_grp_key_info_t *base_key_info = NULL, *key_info = NULL;
    const bcmptm_rm_hash_key_format_t *fmt = NULL;

    SHR_FUNC_ENTER(unit);

    base_key_info = lt_info->enh_more_info->lt_key_info->grp_key_info;
    key_info = base_key_info + grp_idx;
    fmt_cnt = key_info->key_format_count;

    if (e_attrs != NULL) {
        for (fmt_idx = 0; fmt_idx < fmt_cnt; fmt_idx++) {
            fmt = key_info->key_format + fmt_idx;
            if (e_attrs->entry_mode == fmt->entry_mode) {
                *key_fmt = fmt;
                break;
            }
        }
        if (fmt_idx == fmt_cnt) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else {
        *key_fmt = key_info->key_format;
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_key_format_num_view_get(int unit,
                                       const bcmptm_rm_hash_key_format_t *fmt,
                                       uint8_t *num_views)
{
    const bcmptm_rm_hash_valid_view_info_t *valid_view_info = NULL;

    valid_view_info = fmt->valid_view_info;
    *num_views = valid_view_info->view_index_count;
}

int
bcmptm_rm_hash_inst_exact_view_get(int unit,
                                   bcmptm_rm_hash_lt_info_t *lt_info,
                                   uint8_t grp_idx,
                                   const bcmptm_rm_hash_key_format_t *key_format,
                                   uint16_t ldtype,
                                   const bcmptm_rm_hash_view_info_t **view_info)
{
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;
    const bcmptm_rm_hash_lt_data_info_t *lt_data_info = NULL;
    const bcmptm_rm_hash_grp_data_info_t *grp_data_info = NULL;
    const bcmptm_rm_hash_grp_view_info_t *grp_view_info = NULL;
    const bcmptm_rm_hash_grp_view_info_t *base_grp_view_info = NULL;
    uint8_t idx, entry_size = 0;

    SHR_FUNC_ENTER(unit);

    base_grp_view_info = lt_info->enh_more_info->lt_view_info->grp_view_info;
    grp_view_info = base_grp_view_info + grp_idx;
    base_view_info = grp_view_info->view_info;
    lt_data_info = lt_info->enh_more_info->lt_data_info;
    if (lt_data_info == NULL) {
        /*
         * This LT has only one data format. The view information can be
         * retrieved from key format.
         */
        *view_info = base_view_info + key_format->valid_view_info->view_index[0];
        SHR_EXIT();
    } else {
        /*
         * This LT has multiple data formats. The view information can be
         * retrieved thru entry size.
         */
        grp_data_info = lt_data_info->grp_data_info + grp_idx;
        for (idx = 0; idx < grp_data_info->num_ldtype; idx++) {
            if (ldtype == grp_data_info->ldtype_val[idx]) {
                entry_size = grp_data_info->entry_size[idx];
                break;
            }
        }
        if (idx == grp_data_info->num_ldtype) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_size_based_view_info_get(unit,
                                                     lt_info,
                                                     grp_idx,
                                                     key_format,
                                                     entry_size,
                                                     view_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_ent_exact_view_get(int unit,
                                  bcmptm_rm_hash_entry_attrs_t *e_attrs,
                                  bcmptm_rm_hash_lt_info_t *lt_info,
                                  uint8_t grp_idx,
                                  uint16_t ldtype,
                                  const bcmptm_rm_hash_view_info_t **view_info)
{
    const bcmptm_rm_hash_view_info_t *base_view_info = NULL;
    const bcmptm_rm_hash_lt_data_info_t *lt_data_info = NULL;
    const bcmptm_rm_hash_grp_data_info_t *grp_data_info = NULL;
    const bcmptm_rm_hash_key_format_t *key_fmt = NULL;
    const bcmptm_rm_hash_grp_view_info_t *grp_view_info = NULL;
    const bcmptm_rm_hash_grp_view_info_t *base_grp_view_info = NULL;
    uint8_t idx, entry_size = 0;

    SHR_FUNC_ENTER(unit);

    base_grp_view_info = lt_info->enh_more_info->lt_view_info->grp_view_info;
    grp_view_info = base_grp_view_info + grp_idx;
    base_view_info = grp_view_info->view_info;
    lt_data_info = lt_info->enh_more_info->lt_data_info;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_key_format_get(unit, e_attrs, lt_info, grp_idx,
                                       &key_fmt));
    if (key_fmt == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (lt_data_info == NULL) {
        /*
         * This LT has only one data format. The view information can be
         * retrieved from key format.
         */
        *view_info = base_view_info + key_fmt->valid_view_info->view_index[0];
        SHR_EXIT();
    } else {
        /*
         * This LT has multiple data formats. The view information can be
         * retrieved thru entry size.
         */
        grp_data_info = lt_data_info->grp_data_info + grp_idx;
        for (idx = 0; idx < grp_data_info->num_ldtype; idx++) {
            if (ldtype == grp_data_info->ldtype_val[idx]) {
                entry_size = grp_data_info->entry_size[idx];
                break;
            }
        }
        if (idx == grp_data_info->num_ldtype) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_size_based_view_info_get(unit,
                                                     lt_info,
                                                     grp_idx,
                                                     key_fmt,
                                                     entry_size,
                                                     view_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_fmt_info_get(int unit,
                            bcmptm_rm_hash_entry_attrs_t *e_attrs,
                            rm_hash_lt_ctrl_t *lt_ctrl,
                            uint8_t grp_idx,
                            rm_hash_fmt_info_t **fmt_info)
{
    uint8_t fmt_idx;
    const bcmptm_rm_hash_key_format_t *key_format = NULL;
    rm_hash_fmt_info_t *tmp_fmt_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_key_format_get(unit, e_attrs, &lt_ctrl->lt_info,
                                       grp_idx, &key_format));
    for (fmt_idx = 0; fmt_idx < lt_ctrl->num_fmts; fmt_idx++) {
        tmp_fmt_info = &lt_ctrl->fmt_info[fmt_idx];
        if (key_format == tmp_fmt_info->key_format) {
            *fmt_info = tmp_fmt_info;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_key_fields_exchange(int unit,
                                   bcmptm_rm_hash_entry_attrs_t *e_attrs,
                                   bcmptm_rm_hash_lt_info_t *lt_info,
                                   uint8_t grp_a,
                                   uint8_t grp_b,
                                   uint32_t *entry_a,
                                   uint32_t *entry_b)
{
    const bcmptm_rm_hash_key_format_t *key_fmt_a = NULL;
    const bcmptm_rm_hash_key_format_t *key_fmt_b = NULL;
    const bcmlrd_hw_field_list_t *key_flds_a = NULL;
    const bcmlrd_hw_field_list_t *key_flds_b = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_key_format_get(unit,
                                       e_attrs,
                                       lt_info,
                                       grp_a,
                                       &key_fmt_a));
    if (key_fmt_a == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    key_flds_a = key_fmt_a->hw_key_fields;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_key_format_get(unit,
                                       e_attrs,
                                       lt_info,
                                       grp_b,
                                       &key_fmt_b));
    if (key_fmt_b == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    key_flds_b = key_fmt_b->hw_key_fields;

    rm_hash_fields_exchange(unit,
                            key_flds_a,
                            key_flds_b,
                            entry_a,
                            entry_b);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_data_fields_exchange(int unit,
                                    bcmptm_rm_hash_lt_info_t *lt_info,
                                    uint16_t ldtype,
                                    uint8_t grp_a,
                                    uint8_t grp_b,
                                    uint32_t *entry_a,
                                    uint32_t *entry_b)
{
    const bcmlrd_hw_field_list_t *data_fields_a = NULL;
    const bcmlrd_hw_field_list_t *data_fields_b = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (rm_hash_data_fields_get(unit,
                                 lt_info,
                                 grp_a,
                                 ldtype,
                                 &data_fields_a));

    SHR_IF_ERR_EXIT
        (rm_hash_data_fields_get(unit,
                                 lt_info,
                                 grp_b,
                                 ldtype,
                                 &data_fields_b));

    rm_hash_fields_exchange(unit,
                            data_fields_a,
                            data_fields_b,
                            entry_a,
                            entry_b);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_req_ent_view_info_get(int unit,
                                     bcmptm_rm_hash_req_t *req_info,
                                     bcmptm_rm_hash_lt_info_t *lt_info,
                                     uint8_t grp_idx,
                                     const bcmptm_rm_hash_key_format_t *key_format,
                                     const bcmptm_rm_hash_view_info_t **view_info)
{
    bcmptm_rm_hash_entry_attrs_t *e_attrs = req_info->entry_attrs;

    SHR_FUNC_ENTER(unit);

    if ((e_attrs != NULL) && (e_attrs->d_entry_attrs != NULL)) {
        /* The view info needs to be determined based on input of caller. */
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_size_based_view_info_get(unit,
                                                     lt_info,
                                                     grp_idx,
                                                     key_format,
                                                     e_attrs->d_entry_attrs->entry_size,
                                                     view_info));
    } else {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_hash_key_format_view_info_get(unit,
                                                     lt_info,
                                                     grp_idx,
                                                     key_format,
                                                     0,
                                                     view_info));
    }

exit:
    SHR_FUNC_EXIT();
}
