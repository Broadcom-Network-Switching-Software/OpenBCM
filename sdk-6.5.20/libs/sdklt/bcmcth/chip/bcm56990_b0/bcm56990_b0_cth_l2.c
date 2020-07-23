/*! \file bcm56990_b0_cth_l2.c
 *
 * L2 learn drivers for bcm56990 B0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <shr/shr_ha.h>

#include <bcmbd/bcmbd_cmicx_lp_intr.h>
#include <bcmbd/chip/bcm56990_b0_lp_intr.h>
#include <bcmbd/chip/bcm56990_b0_acc.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_util.h>

#include <bcmcth/bcmcth_l2_util.h>
#include <bcmcth/bcmcth_l2.h>


#define BSL_LOG_MODULE          BSL_LS_BCMCTH_L2

/* L2_VFI_LEARN_DATAt_MODIDf not mapped */
#define L2_LEARN_DATA_FLD_CNT   (L2_VFI_LEARN_DATAt_FIELD_COUNT - 1)

/* FLTG did not generate MACRO for minimal valid value for each field */
#define VALID_MIN_L3_MC_GRP_ID  1
#define VALID_MIN_NHOP_ID       1

/* Data struct defined in bcmcth/generated/l2_ha.h */
static bcmcth_l2_vfi_learn_cache_t *learn_cache[BCML2_DEV_NUM_MAX];

/*
 * IMM table is doubled of h/w cache size, to avoid index collision on write
 * entry into IMM, but the supported maximum number of simultaneously learnt
 * entries for LT is not changed.
 * offset for each h/w entry acts as an indicator of ping pong buffer.
 */
static bcmcth_l2_learn_cache_offset_t *learn_cache_offset[BCML2_DEV_NUM_MAX];

/* L2 fields array IMM handler. */
static shr_famm_hdl_t l2_fld_arr_hdl[BCML2_DEV_NUM_MAX];

static int l2_learn_intr[] = {
    QUAD0_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD1_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD2_IP_TO_CMIC_LEARN_CACHE_INTR,
    QUAD3_IP_TO_CMIC_LEARN_CACHE_INTR
};

static int
l2_learn_data_insert(int unit, l2_vfi_learn_addr_t *l2addr)
{
    bcmltd_fields_t in_flds;
    int i = 0;
    uint32_t fval32[2] = {0};

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\t vfi=%#4x, type=%d, src=%#4x, svp=%#4x \
                             mac=%02x:%02x:%02x:%02x:%02x:%02x \n"),
                l2addr->vfi,
                l2addr->src_type,
                l2addr->source.data,
                l2addr->svp,
                l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
                l2addr->mac[3], l2addr->mac[4], l2addr->mac[5]));

    in_flds.count = L2_LEARN_DATA_FLD_CNT;
    in_flds.field = shr_famm_alloc(l2_fld_arr_hdl[unit], in_flds.count);
    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    i = 0;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_PIPEf;
    in_flds.field[i]->data = l2addr->pipe;

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_L2_VFI_LEARN_DATA_IDf;
    in_flds.field[i]->data = l2addr->data_id;

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_VFI_IDf;
    in_flds.field[i]->data = l2addr->vfi;

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_MAC_ADDRf;
    BCMCTH_MAC_ADDR_TO_UINT32(l2addr->mac, fval32);
    in_flds.field[i]->data = fval32[1];
    in_flds.field[i]->data = in_flds.field[i]->data << 32 | fval32[0];

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_SVPf;
    in_flds.field[i]->data = l2addr->svp;

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_SRC_TYPEf;
    in_flds.field[i]->data = l2addr->src_type;

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_MODPORTf;
    if (l2addr->src_type == L2_DEST_TYPE_PORT) {
        in_flds.field[i]->data = l2addr->source.port;
    }

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_TRUNK_IDf;
    if (l2addr->src_type == L2_DEST_TYPE_TRUNK) {
        in_flds.field[i]->data = l2addr->source.trunk_id;
    }

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_L2_MC_GROUP_IDf;
    if (l2addr->src_type == L2_DEST_TYPE_L2_MC_GRP) {
        in_flds.field[i]->data = l2addr->source.l2_mc_grp_id;
    }

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_TM_MC_GROUP_IDf;
    if (l2addr->src_type == L2_DEST_TYPE_L3_MC_GRP) {
        in_flds.field[i]->data = l2addr->source.l3_mc_grp_id;
    } else {
        in_flds.field[i]->data = VALID_MIN_L3_MC_GRP_ID;
    }

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_NHOP_IDf;
    if (l2addr->src_type == L2_DEST_TYPE_NHOP) {
        in_flds.field[i]->data = l2addr->source.nhop_id;
    } else {
        in_flds.field[i]->data = VALID_MIN_NHOP_ID;
    }

    i++;
    in_flds.field[i]->id   = L2_VFI_LEARN_DATAt_ECMP_IDf;
    if (l2addr->src_type == L2_DEST_TYPE_ECMP_GRP) {
        in_flds.field[i]->data = l2addr->source.ecmp_grp_id;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit, L2_VFI_LEARN_DATAt, &in_flds));

exit:
    if (in_flds.field != NULL) {
        shr_famm_free(l2_fld_arr_hdl[unit], in_flds.field, in_flds.count);
    }
    SHR_FUNC_EXIT();
}

static void
l2_learn_hw_to_l2addr(L2_LEARN_CACHEm_t hw_entry, l2_vfi_learn_addr_t *l2addr)
{
    uint32_t fval32[2] = {0};

    l2addr->vfi = L2_LEARN_CACHEm_L2_VFIf_GET(hw_entry);
    L2_LEARN_CACHEm_L2_MAC_ADDRf_GET(hw_entry, fval32);
    BCMCTH_MAC_ADDR_FROM_UINT32(l2addr->mac, fval32);
    l2addr->svp = L2_LEARN_CACHEm_L2_DVPf_GET(hw_entry);
    l2addr->src_type = L2_LEARN_CACHEm_L2_DEST_TYPEf_GET(hw_entry);
    l2addr->source.data = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(hw_entry);

    return;
}

static bool
l2_learn_sw_equal_to_l2addr(bcmcth_l2_vfi_learn_cache_t *sw_cache,
                            l2_vfi_learn_addr_t *l2addr)
{
    if (sw_cache->vfi != l2addr->vfi) {
        return FALSE;
    }

    if (sal_memcmp((const void *)sw_cache->mac,
                   (const void *)l2addr->mac, BCM_MAC_BYTES)) {
        return FALSE;
    }

    if (sw_cache->svp != l2addr->svp) {
        return FALSE;
    }

    if (sw_cache->src_type != l2addr->src_type) {
        return FALSE;
    }

    if (sw_cache->source != l2addr->source.data) {
        return FALSE;
    }

    return TRUE;
}

static int
bcm56990_b0_cth_l2_intr_enable(int unit, int enable)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
        if (enable) {
            bcmbd_cmicx_lp_intr_enable(unit, l2_learn_intr[idx]);
        } else {
            bcmbd_cmicx_lp_intr_disable_nosync(unit, l2_learn_intr[idx]);
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_intr_handler_set(int unit, bcmcth_l2_drv_intr_handler_f cb)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, idx) {
        SHR_IF_ERR_EXIT
            (bcmbd_cmicx_lp_intr_func_set(unit, l2_learn_intr[idx], cb, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_hw_enable(int unit, uint32_t trans_id, bool enable)
{
    L2_LEARN_COPY_CACHE_CTRLr_t cache_ctrl;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_read(unit,
                                      trans_id,
                                      L2_LEARN_CONTROLt,
                                      L2_LEARN_COPY_CACHE_CTRLr,
                                      0,
                                      0,
                                      &cache_ctrl));
    L2_LEARN_COPY_CACHE_CTRLr_CLEAR_ON_READ_ENf_SET(cache_ctrl, 0);
    L2_LEARN_COPY_CACHE_CTRLr_CACHE_INTERRUPT_CTRLf_SET(cache_ctrl, 1);
    L2_LEARN_COPY_CACHE_CTRLr_L2_LEARN_CACHE_ENf_SET(cache_ctrl, enable);

    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_mreq_write(unit,
                                       trans_id,
                                       L2_LEARN_CONTROLt,
                                       L2_LEARN_COPY_CACHE_CTRLr,
                                       0,
                                       0,
                                       &cache_ctrl));

exit:
    SHR_FUNC_EXIT();
}


/* Traverse pt to populate entries to IMM. */
static int
bcm56990_b0_cth_l2_cache_traverse(int unit, l2_trav_status_t *status)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t pipe_idx, cache_idx, tbl_idx;
    int rv = 0, valid_entry = 0;
    uint32_t fval32[2] = {0};
    L2_LEARN_CACHEm_t hw_entry;
    l2_vfi_learn_addr_t l2addr;

    bcmcth_l2_vfi_learn_cache_t sw_cache_temp;
    bcmcth_l2_vfi_learn_cache_t *sw_cache = learn_cache[unit];
    bcmcth_l2_learn_cache_offset_t *offset = learn_cache_offset[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sw_cache, SHR_E_INTERNAL);
    SHR_NULL_CHECK(offset, SHR_E_INTERNAL);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    SHR_BIT_ITER(pi->map, pi->max_idx + 1, pipe_idx) {
        for (cache_idx = 0; cache_idx < pi->tbl_size; cache_idx++) {
            rv = bcmcth_l2_per_pipe_ireq_read(unit,
                                              drv->state_sid,
                                              drv->state_ptsid,
                                              pipe_idx,
                                              cache_idx,
                                              &hw_entry);
            if (rv < SHR_E_NONE) {
                valid_entry = 1;
                continue;
            }

            tbl_idx = pipe_idx * pi->tbl_size + cache_idx;

            /* ignore the entry not valid */
            if (!L2_LEARN_CACHEm_VALID_EOPf_GET(hw_entry) ||
                !L2_LEARN_CACHEm_VALID_SOPf_GET(hw_entry)) {
                    sal_memset(&sw_cache[tbl_idx], 0,
                               sizeof(bcmcth_l2_vfi_learn_cache_t));
                    continue;
            }

            /* HW entry is valid. */
            valid_entry = 1;
            sal_memset(&l2addr, 0, sizeof(l2_vfi_learn_addr_t));
            l2_learn_hw_to_l2addr(hw_entry, &l2addr);
            l2addr.pipe = pipe_idx;
            l2addr.data_id = cache_idx + offset[tbl_idx].offset * pi->tbl_size;

            if (l2_learn_sw_equal_to_l2addr(&sw_cache[tbl_idx], &l2addr)) {
                /* Already reported. */
                continue;
            }

            sal_memcpy(&sw_cache_temp, &sw_cache[tbl_idx],
                       sizeof(bcmcth_l2_vfi_learn_cache_t));

            sw_cache[tbl_idx].vfi = L2_LEARN_CACHEm_L2_VFIf_GET(hw_entry);
            L2_LEARN_CACHEm_L2_MAC_ADDRf_GET(hw_entry, fval32);
            BCMCTH_MAC_ADDR_FROM_UINT32(sw_cache[tbl_idx].mac, fval32);
            sw_cache[tbl_idx].svp = L2_LEARN_CACHEm_L2_DVPf_GET(hw_entry);
            sw_cache[tbl_idx].src_type = L2_LEARN_CACHEm_L2_DEST_TYPEf_GET(hw_entry);
            sw_cache[tbl_idx].source = L2_LEARN_CACHEm_L2_DESTINATIONf_GET(hw_entry);

            status->new_learn = 1;

            /* Not reported. */
            rv = l2_learn_data_insert(unit, &l2addr);
            if (rv < SHR_E_NONE && rv != SHR_E_EXISTS) {
                sal_memcpy(&sw_cache[tbl_idx], &sw_cache_temp,
                           sizeof(bcmcth_l2_vfi_learn_cache_t));
                continue;
            }

            offset[tbl_idx].offset = !offset[tbl_idx].offset;
        }
    }

    status->cache_empty = !valid_entry;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_entry_delete(int unit, void *data)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t pipe_idx = 0, cache_idx = 0, tbl_idx;
    L2_LEARN_CACHEm_t hw_entry;
    bcmcth_l2_vfi_learn_cache_t *sw_cache = learn_cache[unit];
    const bcmltd_field_t *lt_field;
    uint32_t fid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sw_cache, SHR_E_INTERNAL);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    lt_field = (bcmltd_field_t *)data;
    while (lt_field) {
        fid = lt_field->id;
        switch (fid) {
        case L2_VFI_LEARN_DATAt_PIPEf:
            pipe_idx = (uint8_t)lt_field->data;
            break;
        case L2_VFI_LEARN_DATAt_L2_VFI_LEARN_DATA_IDf:
            cache_idx = (uint16_t)lt_field->data % pi->tbl_size;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        lt_field = lt_field->next;
    }

    if (SHR_BITGET(pi->map, pipe_idx) == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    L2_LEARN_CACHEm_CLR(hw_entry);
    SHR_IF_ERR_EXIT
        (bcmcth_l2_per_pipe_ireq_write(unit,
                                       drv->state_sid,
                                       drv->state_ptsid,
                                       pipe_idx,
                                       cache_idx,
                                       &hw_entry));

    /* clean entry in sw cache */
    tbl_idx = pipe_idx * pi->tbl_size + cache_idx;
    sal_memset(&sw_cache[tbl_idx], 0, sizeof(bcmcth_l2_vfi_learn_cache_t));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_entry_parse(int unit, const bcmltd_field_t *key,
                               const bcmltd_field_t *data, void *lt)
{
    l2_learn_control_info_t *lt_info = (l2_learn_control_info_t *)lt;
    const bcmltd_field_t *lt_field = data;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(key);

    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;

        switch (fid) {
        case L2_LEARN_CONTROLt_REPORTf:
            lt_info->enable = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_REPORT);
            break;

        case L2_LEARN_CONTROLt_SLOW_POLLf:
            lt_info->slow_poll = fval ? TRUE : FALSE;
            SHR_BITSET(lt_info->fbmp, LEARN_CONTROL_SLOW_POLL);
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        lt_field = lt_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_init(int unit, bool warm)
{
    bcmcth_l2_drv_t *drv;
    bcmcth_l2_pipe_info_t *pi;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, array_size = 0;

    SHR_FUNC_ENTER(unit);

    drv = bcmcth_l2_drv_get(unit);
    pi = &(drv->pipe_info);

    array_size = (pi->max_idx + 1) * pi->tbl_size;

    /* allocate array for pipes until the last avtive pipe */
    ha_req_size = sizeof(bcmcth_l2_vfi_learn_cache_t) * array_size;
    ha_alloc_size = ha_req_size;
    learn_cache[unit] = shr_ha_mem_alloc(unit,
                                         BCMMGMT_L2_COMP_ID,
                                         BCML2_LEARN_DRV_COMP_ID,
                                         "bcmcthL2LearnDrvCache",
                                         &ha_alloc_size);

    SHR_NULL_CHECK(learn_cache[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(learn_cache[unit], 0, ha_alloc_size);
    }

    /* allocate array for learn_cache_offset */
    ha_req_size = sizeof(bcmcth_l2_learn_cache_offset_t) * array_size;
    ha_alloc_size = ha_req_size;
    learn_cache_offset[unit] = shr_ha_mem_alloc(unit,
                                             BCMMGMT_L2_COMP_ID,
                                             BCML2_LEARN_CACHE_OFFSET_COMP_ID,
                                             "bcmcthL2LearnCacheOffset",
                                             &ha_alloc_size);

    SHR_NULL_CHECK(learn_cache_offset[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(learn_cache_offset[unit], 0, ha_alloc_size);
    }

    /* ISSU support */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_L2_COMP_ID,
                                              BCML2_LEARN_DRV_COMP_ID, 0,
                                              sizeof(bcmcth_l2_vfi_learn_cache_t),
                                              array_size,
                                              BCMCTH_L2_VFI_LEARN_CACHE_T_ID));
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_L2_COMP_ID,
                                              BCML2_LEARN_CACHE_OFFSET_COMP_ID,
                                              0,
                                              sizeof(bcmcth_l2_learn_cache_offset_t),
                                              array_size,
                                              BCMCTH_L2_LEARN_CACHE_OFFSET_T_ID));
    }

    SHR_IF_ERR_EXIT
        (shr_famm_hdl_init(L2_LEARN_DATA_FLD_CNT, &l2_fld_arr_hdl[unit]));

exit:
    if (SHR_FUNC_ERR()) {
        if (learn_cache[unit] != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_L2_VFI_LEARN_CACHE_T_ID,
                                      BCMMGMT_L2_COMP_ID,
                                      BCML2_LEARN_DRV_COMP_ID);
            /* only on error need to free memory */
            shr_ha_mem_free(unit, learn_cache[unit]);
            learn_cache[unit] = NULL;
        }
        if (learn_cache_offset[unit] != NULL) {
            bcmissu_struct_info_clear(unit,
                                      BCMCTH_L2_LEARN_CACHE_OFFSET_T_ID,
                                      BCMMGMT_L2_COMP_ID,
                                      BCML2_LEARN_CACHE_OFFSET_COMP_ID);
            /* only on error need to free memory */
            shr_ha_mem_free(unit, learn_cache_offset[unit]);
            learn_cache_offset[unit] = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_l2_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (l2_fld_arr_hdl[unit] != NULL) {
        shr_famm_hdl_delete(l2_fld_arr_hdl[unit]);
        l2_fld_arr_hdl[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcm56990_b0_cth_l2_attach(int unit)
{
    bcmcth_l2_drv_t *drv = bcmcth_l2_drv_get(unit);

    drv->cfg_sid = L2_LEARN_CONTROLt;
    drv->state_sid = L2_VFI_LEARN_DATAt;
    drv->state_ptsid = L2_LEARN_CACHEm;

    drv->init = bcm56990_b0_cth_l2_init;
    drv->cleanup = bcm56990_b0_cth_l2_cleanup;
    drv->hw_enable = bcm56990_b0_cth_l2_hw_enable;
    drv->intr_enable = bcm56990_b0_cth_l2_intr_enable;
    drv->intr_handler_set = bcm56990_b0_cth_l2_intr_handler_set;
    drv->cache_traverse = bcm56990_b0_cth_l2_cache_traverse;
    drv->entry_delete = bcm56990_b0_cth_l2_entry_delete;
    drv->entry_parse = bcm56990_b0_cth_l2_entry_parse;

    return SHR_E_NONE;
}
