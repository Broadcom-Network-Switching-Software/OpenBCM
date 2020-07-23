/*! \file bcm56780_a0_cth_ctr_evict.c
 *
 * Chip stub for BCMCTH couter eviction control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmcth/bcmcth_ctr_evict_drv.h>
#include <bcmcth/bcmcth_imm_util.h>

/******************************************************************************
* Local definitions
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREVICT

#define IS_PIPE_ENABLE(map, p) ((map) & (0x01 << p))

static const bcmdrd_sid_t evict_control_reg[] = {
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_0r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_1r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_2r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_3r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_4r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_5r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_6r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_7r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_8r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_9r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_10r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_11r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_12r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_13r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_14r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_15r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_16r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_17r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_18r,
    FLEX_CTR_ING_EVICTION_CONTROL_POOL_19r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_0r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_1r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_2r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_3r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_4r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_5r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_6r,
    FLEX_CTR_EGR_EVICTION_CONTROL_POOL_7r,
    EPOST_EGR_PERQ_EVICTION_CONTROLr
};

static const bcmdrd_sid_t evict_thd_reg[] = {
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_0r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_1r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_2r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_3r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_4r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_5r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_6r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_7r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_8r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_9r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_10r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_11r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_12r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_13r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_14r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_15r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_16r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_17r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_18r,
    FLEX_CTR_ING_EVICTION_CMD_A_THRESHOLD_POOL_19r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_0r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_1r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_2r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_3r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_4r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_5r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_6r,
    FLEX_CTR_EGR_EVICTION_CMD_A_THRESHOLD_POOL_7r,
    EPOST_EGR_PERQ_EVICTION_CMD_A_THRESHOLDr,
};

static const bcmdrd_sid_t evict_thd_b_reg[] = {
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_0r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_1r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_2r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_3r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_4r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_5r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_6r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_7r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_8r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_9r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_10r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_11r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_12r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_13r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_14r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_15r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_16r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_17r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_18r,
    FLEX_CTR_ING_EVICTION_CMD_B_THRESHOLD_POOL_19r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_0r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_1r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_2r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_3r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_4r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_5r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_6r,
    FLEX_CTR_EGR_EVICTION_CMD_B_THRESHOLD_POOL_7r,
    EPOST_EGR_PERQ_EVICTION_CMD_B_THRESHOLDr
};

static const bcmdrd_sid_t evict_seed_reg[] = {
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_0r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_1r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_2r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_3r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_4r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_5r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_6r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_7r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_8r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_9r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_10r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_11r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_12r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_13r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_14r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_15r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_16r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_17r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_18r,
    FLEX_CTR_ING_EVICTION_LFSR_SEED_POOL_19r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_0r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_1r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_2r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_3r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_4r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_5r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_6r,
    FLEX_CTR_EGR_EVICTION_LFSR_SEED_POOL_7r,
    EPOST_EGR_PERQ_EVICTION_SEEDr
};


static const bcmdrd_sid_t evict_pool[] = {
    FLEX_CTR_ING_COUNTER_TABLE_0m,
    FLEX_CTR_ING_COUNTER_TABLE_1m,
    FLEX_CTR_ING_COUNTER_TABLE_2m,
    FLEX_CTR_ING_COUNTER_TABLE_3m,
    FLEX_CTR_ING_COUNTER_TABLE_4m,
    FLEX_CTR_ING_COUNTER_TABLE_5m,
    FLEX_CTR_ING_COUNTER_TABLE_6m,
    FLEX_CTR_ING_COUNTER_TABLE_7m,
    FLEX_CTR_ING_COUNTER_TABLE_8m,
    FLEX_CTR_ING_COUNTER_TABLE_9m,
    FLEX_CTR_ING_COUNTER_TABLE_10m,
    FLEX_CTR_ING_COUNTER_TABLE_11m,
    FLEX_CTR_ING_COUNTER_TABLE_12m,
    FLEX_CTR_ING_COUNTER_TABLE_13m,
    FLEX_CTR_ING_COUNTER_TABLE_14m,
    FLEX_CTR_ING_COUNTER_TABLE_15m,
    FLEX_CTR_ING_COUNTER_TABLE_16m,
    FLEX_CTR_ING_COUNTER_TABLE_17m,
    FLEX_CTR_ING_COUNTER_TABLE_18m,
    FLEX_CTR_ING_COUNTER_TABLE_19m,
    FLEX_CTR_EGR_COUNTER_TABLE_0m,
    FLEX_CTR_EGR_COUNTER_TABLE_1m,
    FLEX_CTR_EGR_COUNTER_TABLE_2m,
    FLEX_CTR_EGR_COUNTER_TABLE_3m,
    FLEX_CTR_EGR_COUNTER_TABLE_4m,
    FLEX_CTR_EGR_COUNTER_TABLE_5m,
    FLEX_CTR_EGR_COUNTER_TABLE_6m,
    FLEX_CTR_EGR_COUNTER_TABLE_7m,
    EPOST_EGR_PERQ_XMT_COUNTERS_0m
};

/*******************************************************************************
 * Private Functions
 */
static bool is_pipe_enable(int unit,  bcmdrd_sid_t sid, int pipe)
{
    int blktype;
    uint32_t pipe_map;
    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map);

    return IS_PIPE_ENABLE(pipe_map, pipe);
}

static int
evict_mem_idx_get(int unit, ctr_pool_t pool, uint16_t pool_id, int *mem_idx)
{
    int offset = 0;

    SHR_FUNC_ENTER(unit);
    switch (pool) {
        case CTR_POOL_ING_FLEX:
            offset = 0;
            break;
        case CTR_POOL_EGR_FLEX:
            offset = 20;
            break;
        case CTR_POOL_EGR_PERQ:
            offset = 28;
            break;
        default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *mem_idx = offset + pool_id;

exit:
    SHR_FUNC_EXIT();
}

static int
evict_data_modify(int unit,
                  bcmltd_sid_t ltid,
                  uint32_t trans_id,
                  bcmdrd_sid_t sid,
                  bcmdrd_fid_t fid,
                  uint32_t *value)
{
    int pipe, tbl_inst_num;
    uint32_t entry_sz;
    uint32_t *entry_buf = NULL;

    SHR_FUNC_ENTER(unit);

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthCtrEvictData");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    tbl_inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    for (pipe = 0; pipe < tbl_inst_num; pipe++) {
        if (is_pipe_enable(unit, sid, pipe)) {
            SHR_IF_ERR_EXIT
                (bcmcth_imm_cb_read(unit,
                                    ltid,
                                    sid,
                                    pipe,
                                    0,
                                    entry_buf));

            bcmdrd_pt_field_set(unit, sid, entry_buf, fid, value);

            SHR_IF_ERR_EXIT
                (bcmcth_imm_cb_write(unit,
                                     ltid,
                                     sid,
                                     pipe,
                                     0,
                                     entry_buf));
        }
    }
exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_ctr_evict_entry_update(int unit,
                                    bcmltd_sid_t ltid,
                                    uint32_t trans_id,
                                    ctr_control_entry_t *entry)
{
    int             mem_idx = 0;
    bcmdrd_sid_t    sid;
    bcmdrd_fid_t    fid;
    uint32_t        value[BCMDRD_MAX_PT_WSIZE];
    bool enable;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (evict_mem_idx_get(unit, entry->pool, entry->pool_id, &mem_idx));

    if (entry->update_flags & CTR_UPDATE_F_E_MODE) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_control_reg[mem_idx];
        value[0] = entry->evict_mode;
        fid = MODEf;
        SHR_IF_ERR_EXIT
            (evict_data_modify(unit, ltid, trans_id, sid, fid, value));

        if (entry->evict_mode) {
            enable = 1;
        } else {
            enable = 0;
        }

        sid = evict_pool[mem_idx];
        SHR_IF_ERR_EXIT
            (bcmptm_cci_evict_control_set(unit, sid, enable,
                                          entry->evict_mode));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_THD_BYTES) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_thd_reg[mem_idx];
        value[0] = entry->evict_thd_bytes & 0xFFFFFFFF;
        value[1] = (entry->evict_thd_bytes >> 32) & 0xFFFFFFFF;
        fid = THRESHOLDf;
        SHR_IF_ERR_EXIT
            (evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_THD_PKTS) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_thd_b_reg[mem_idx];
        value[0] = entry->evict_thd_pkts;
        fid = THRESHOLDf;
        SHR_IF_ERR_EXIT
            (evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_SEED) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_seed_reg[mem_idx];
        value[0] = entry->evict_seed & 0xFFFFFFFF;
        value[1] = (entry->evict_seed >> 32) & 0xFFFFFFFF;
        fid = SEEDf;
        SHR_IF_ERR_EXIT
            (evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_THD_CTR_A) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_thd_reg[mem_idx];
        value[0] = entry->evict_thd_ctr_a & 0xFFFFFFFF;
        value[1] = (entry->evict_thd_ctr_a >> 32) & 0xFFFFFFFF;
        fid = THRESHOLDf;
        SHR_IF_ERR_EXIT
            (evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }
    if (entry->update_flags & CTR_UPDATE_F_E_THD_CTR_B) {
        sal_memset(value, 0, sizeof(value));
        sid = evict_thd_b_reg[mem_idx];
        value[0] = entry->evict_thd_ctr_b & 0xFFFFFFFF;
        value[1] = (entry->evict_thd_ctr_b >> 32) & 0xFFFFFFFF;
        fid = THRESHOLDf;
        SHR_IF_ERR_EXIT
            (evict_data_modify(unit, ltid, trans_id, sid, fid, value));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_ctr_evict_init(int unit)
{
    bcmdrd_sid_t        sid;
    uint32_t            *entry_buf = NULL;
    uint32_t            entry_sz;
    uint32_t            mem_id, pipe;
    int                 i, tbl_inst;
    int                 tbl_inst_num;

    SHR_FUNC_ENTER(unit);

    entry_sz = bcmdrd_pt_entry_wsize(unit, evict_control_reg[0]);
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthCtrEvictInit");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    for (i = 0; i < COUNTOF(evict_control_reg); i++) {
        sid = evict_control_reg[i];
        mem_id = i + 1;
        tbl_inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
        pipe = 0;
        for (tbl_inst = 0; tbl_inst < tbl_inst_num; tbl_inst++) {
            if (is_pipe_enable(unit, sid, tbl_inst)) {
                SHR_IF_ERR_EXIT
                    (bcmcth_imm_cb_read(unit, -1, sid, tbl_inst, 0, entry_buf));
                bcmdrd_pt_field_set(unit, sid, entry_buf, MEMORY_IDf, &mem_id);
                bcmdrd_pt_field_set(unit, sid, entry_buf, PIPE_IDf, &pipe);
                SHR_IF_ERR_EXIT
                    (bcmcth_imm_cb_write(unit, -1, sid, tbl_inst, 0, entry_buf));
            }
            pipe++;
        }
    }

    sid = CENTRAL_CTR_EVICTION_CONTROLr;
    SHR_IF_ERR_EXIT
        (bcmcth_imm_cb_read(unit, -1, sid, 0, 0, entry_buf));
    bcmdrd_pt_field_set(unit, sid, entry_buf, NUM_CE_PER_PIPEf, &mem_id);
    SHR_IF_ERR_EXIT
        (bcmcth_imm_cb_write(unit, -1, sid, 0, 0, entry_buf));

exit:
    if (entry_buf != NULL) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static bcmcth_ctr_evict_drv_t bcm56780_a0_cth_ctr_evict_drv = {
        .evict_init_fn = &bcm56780_a0_cth_ctr_evict_init,
        .evict_entry_update_fn = &bcm56780_a0_cth_ctr_evict_entry_update,
};

/*******************************************************************************
 * Public functions
 */
bcmcth_ctr_evict_drv_t *
bcm56780_a0_cth_ctr_evict_drv_get(int unit)
{
    return &bcm56780_a0_cth_ctr_evict_drv;
}
