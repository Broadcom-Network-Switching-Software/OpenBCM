/*! \file bcm56880_a0_cth_ctr_eflex.c
 *
 * Chip stub for BCMCTH CTR EFLEX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_ts.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_feature.h>

#include <bcmlrd/bcmlrd_enum.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>
#include <bcmcth/chip/bcm56880_a0/bcm56880_a0_ctr_eflex.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Number of counters per section in a pool */
#define NUM_CTR_SECTION 128

/*! Pipe number */
#define NUM_PIPE 4

/*! Pipe instance number */
#define NUM_PIPE_INST 0

/*! Number of flex counter ingress actions */
#define NUM_CTR_EFLEX_ING_ACTION 64

/*! Number of flex counter egress actions */
#define NUM_CTR_EFLEX_EGR_ACTION 64

/*! Number of flex counter reserved actions */
#define NUM_CTR_EFLEX_RSVD_ACTION 2

/*! Number of flex counter simultaneous ingress actions */
#define NUM_CTR_EFLEX_ING_SIM_ACTION 11

/*! Number of flex counter simultaneous egress actions */
#define NUM_CTR_EFLEX_EGR_SIM_ACTION 7

/*! Number of hitbits per flex counter */
#define NUM_CTR_EFLEX_HITBITS 64

/*! Number of flex state ingress actions */
#define NUM_FLEX_STATE_ING_ACTION 16

/*! Number of flex state egress actions */
#define NUM_FLEX_STATE_EGR_ACTION 16

/*! Number of flex state reserved actions */
#define NUM_FLEX_STATE_RSVD_ACTION 1

/*! Number of flex state simultaneous ingress actions */
#define NUM_FLEX_STATE_ING_SIM_ACTION 1

/*! Number of flex state simultaneous egress actions */
#define NUM_FLEX_STATE_EGR_SIM_ACTION 1

/*! Number of operand profiles */
#define NUM_OPERAND 64

/*! Number of range checkers */
#define NUM_RANGE_CHECKER 32

/*! Number of range checkers per bank */
#define NUM_RANGE_CHECKER_PER_BANK 8

/*! Number of objects per action */
#define NUM_OBJECT 4

/*! Number of global objects per action */
#define NUM_GLOBAL_OBJECT 0

/*! Number of slim counters per entry. */
#define NUM_SLIM_CTRS 3

/*! Slim counters are organised coloumn wise in hardware. */
#define SLIM_CTR_ROW FALSE

/*! Number of flex state ingress instances */
#define NUM_FLEX_STATE_ING_INST 2

/*! Number of flex state ingress instances */
#define NUM_FLEX_STATE_EGR_INST 1

static bcmdrd_sid_t ingress_counter_table[] = {
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
    FLEX_CTR_ING_COUNTER_TABLE_19m
};

static bcmdrd_sid_t egress_counter_table[] = {
    FLEX_CTR_EGR_COUNTER_TABLE_0m,
    FLEX_CTR_EGR_COUNTER_TABLE_1m,
    FLEX_CTR_EGR_COUNTER_TABLE_2m,
    FLEX_CTR_EGR_COUNTER_TABLE_3m,
    FLEX_CTR_EGR_COUNTER_TABLE_4m,
    FLEX_CTR_EGR_COUNTER_TABLE_5m,
    FLEX_CTR_EGR_COUNTER_TABLE_6m,
    FLEX_CTR_EGR_COUNTER_TABLE_7m,
};

static bcmdrd_sid_t state_ingress_0_counter_table[] = {
    FLEX_CTR_ST_ING0_COUNTER_TABLE_0m,
    FLEX_CTR_ST_ING0_COUNTER_TABLE_1m,
    FLEX_CTR_ST_ING0_COUNTER_TABLE_2m,
    FLEX_CTR_ST_ING0_COUNTER_TABLE_3m,
};

static bcmdrd_sid_t state_ingress_1_counter_table[] = {
    FLEX_CTR_ST_ING1_COUNTER_TABLE_0m,
    FLEX_CTR_ST_ING1_COUNTER_TABLE_1m,
    FLEX_CTR_ST_ING1_COUNTER_TABLE_2m,
    FLEX_CTR_ST_ING1_COUNTER_TABLE_3m,
};

static bcmdrd_sid_t state_egress_counter_table[] = {
    FLEX_CTR_ST_EGR_COUNTER_TABLE_0m,
    FLEX_CTR_ST_EGR_COUNTER_TABLE_1m,
    FLEX_CTR_ST_EGR_COUNTER_TABLE_2m,
    FLEX_CTR_ST_EGR_COUNTER_TABLE_3m,
};

static const bcmdrd_sid_t ctr_grp_action_reg[][4] = {
    {
        FLEX_CTR_EGR_GROUP_ACTION_0r,
        FLEX_CTR_EGR_GROUP_ACTION_1r,
        FLEX_CTR_EGR_GROUP_ACTION_2r,
        FLEX_CTR_EGR_GROUP_ACTION_3r,
    },
    {
        FLEX_CTR_ING_GROUP_ACTION_0r,
        FLEX_CTR_ING_GROUP_ACTION_1r,
        FLEX_CTR_ING_GROUP_ACTION_2r,
        FLEX_CTR_ING_GROUP_ACTION_3r,
    },
};

static const bcmdrd_sid_t ctr_grp_action_bmp_reg[][4] = {
    {
        FLEX_CTR_EGR_GROUP_ACTION_BITMAP_0r,
        FLEX_CTR_EGR_GROUP_ACTION_BITMAP_1r,
        FLEX_CTR_EGR_GROUP_ACTION_BITMAP_2r,
        FLEX_CTR_EGR_GROUP_ACTION_BITMAP_3r,
    },
    {
        FLEX_CTR_ING_GROUP_ACTION_BITMAP_0r,
        FLEX_CTR_ING_GROUP_ACTION_BITMAP_1r,
        FLEX_CTR_ING_GROUP_ACTION_BITMAP_2r,
        FLEX_CTR_ING_GROUP_ACTION_BITMAP_3r,
    },
};

static const bcmdrd_sid_t state_grp_action_reg[][2][4] = {
    {
        {
            FLEX_CTR_ST_EGR_GROUP_ACTION_0r,
            FLEX_CTR_ST_EGR_GROUP_ACTION_1r,
            FLEX_CTR_ST_EGR_GROUP_ACTION_2r,
            FLEX_CTR_ST_EGR_GROUP_ACTION_3r,
        },
        {
            0,
            0,
            0,
            0,
        },
    },
    {
        {
            FLEX_CTR_ST_ING0_GROUP_ACTION_0r,
            FLEX_CTR_ST_ING0_GROUP_ACTION_1r,
            FLEX_CTR_ST_ING0_GROUP_ACTION_2r,
            FLEX_CTR_ST_ING0_GROUP_ACTION_3r,
        },
        {
            FLEX_CTR_ST_ING1_GROUP_ACTION_0r,
            FLEX_CTR_ST_ING1_GROUP_ACTION_1r,
            FLEX_CTR_ST_ING1_GROUP_ACTION_2r,
            FLEX_CTR_ST_ING1_GROUP_ACTION_3r,
        },
    },
};

static const bcmdrd_sid_t state_grp_action_bmp_reg[][2][4] = {
    {
        {
            FLEX_CTR_ST_EGR_GROUP_ACTION_BITMAP_0r,
            FLEX_CTR_ST_EGR_GROUP_ACTION_BITMAP_1r,
            FLEX_CTR_ST_EGR_GROUP_ACTION_BITMAP_2r,
            FLEX_CTR_ST_EGR_GROUP_ACTION_BITMAP_3r,
        },
        {
            0,
            0,
            0,
            0,
        },
    },
    {
        {
            FLEX_CTR_ST_ING0_GROUP_ACTION_BITMAP_0r,
            FLEX_CTR_ST_ING0_GROUP_ACTION_BITMAP_1r,
            FLEX_CTR_ST_ING0_GROUP_ACTION_BITMAP_2r,
            FLEX_CTR_ST_ING0_GROUP_ACTION_BITMAP_3r,
        },
        {
            FLEX_CTR_ST_ING1_GROUP_ACTION_BITMAP_0r,
            FLEX_CTR_ST_ING1_GROUP_ACTION_BITMAP_1r,
            FLEX_CTR_ST_ING1_GROUP_ACTION_BITMAP_2r,
            FLEX_CTR_ST_ING1_GROUP_ACTION_BITMAP_3r,
        },
    },
};

/*
 * For each IFTAx0_E2T_0y_* bank memory,
 * the table_id and uft/non-uft index is obtained from regsfile.
 */
static ctr_eflex_hitbit_bank_map_t hitbit_sid_log_id_map[] = {
    /* IFTA80 non-UFT memories */
    {IFTA80_E2T_00_B0_SINGLEm, 6, 0},
    {IFTA80_E2T_00_B0_DOUBLEm, 6, 0},
    {IFTA80_E2T_00_B0_QUADm,   6, 0},
    {IFTA80_E2T_00_B1_SINGLEm, 7, 0},
    {IFTA80_E2T_00_B1_DOUBLEm, 7, 0},
    {IFTA80_E2T_00_B1_QUADm,   7, 0},

    {IFTA80_E2T_01_B0_SINGLEm, 8, 0},
    {IFTA80_E2T_01_B0_DOUBLEm, 8, 0},
    {IFTA80_E2T_01_B0_QUADm,   8, 0},
    {IFTA80_E2T_01_B1_SINGLEm, 9, 0},
    {IFTA80_E2T_01_B1_DOUBLEm, 9, 0},
    {IFTA80_E2T_01_B1_QUADm,   9, 0},

    {IFTA80_E2T_02_B0_SINGLEm, 10, 0},
    {IFTA80_E2T_02_B0_DOUBLEm, 10, 0},
    {IFTA80_E2T_02_B0_QUADm,   10, 0},
    {IFTA80_E2T_02_B1_SINGLEm, 11, 0},
    {IFTA80_E2T_02_B1_DOUBLEm, 11, 0},
    {IFTA80_E2T_02_B1_QUADm,   11, 0},

    {IFTA80_E2T_03_B0_SINGLEm, 12, 0},
    {IFTA80_E2T_03_B0_DOUBLEm, 12, 0},
    {IFTA80_E2T_03_B0_QUADm,   12, 0},
    {IFTA80_E2T_03_B1_SINGLEm, 13, 0},
    {IFTA80_E2T_03_B1_DOUBLEm, 13, 0},
    {IFTA80_E2T_03_B1_QUADm,   13, 0},

    /* IFTA90 UFT memories */
    {IFTA90_E2T_00_B0_SINGLEm, 0, 1},
    {IFTA90_E2T_00_B0_DOUBLEm, 0, 1},
    {IFTA90_E2T_00_B0_QUADm,   0, 1},
    {IFTA90_E2T_00_B1_SINGLEm, 1, 1},
    {IFTA90_E2T_00_B1_DOUBLEm, 1, 1},
    {IFTA90_E2T_00_B1_QUADm,   1, 1},

    {IFTA90_E2T_01_B0_SINGLEm, 2, 1},
    {IFTA90_E2T_01_B0_DOUBLEm, 2, 1},
    {IFTA90_E2T_01_B0_QUADm,   2, 1},
    {IFTA90_E2T_01_B1_SINGLEm, 3, 1},
    {IFTA90_E2T_01_B1_DOUBLEm, 3, 1},
    {IFTA90_E2T_01_B1_QUADm,   3, 1},

    {IFTA90_E2T_02_B0_SINGLEm, 4, 1},
    {IFTA90_E2T_02_B0_DOUBLEm, 4, 1},
    {IFTA90_E2T_02_B0_QUADm,   4, 1},
    {IFTA90_E2T_02_B1_SINGLEm, 5, 1},
    {IFTA90_E2T_02_B1_DOUBLEm, 5, 1},
    {IFTA90_E2T_02_B1_QUADm,   5, 1},

    {IFTA90_E2T_03_B0_SINGLEm, 6, 1},
    {IFTA90_E2T_03_B0_DOUBLEm, 6, 1},
    {IFTA90_E2T_03_B0_QUADm,   6, 1},
    {IFTA90_E2T_03_B1_SINGLEm, 7, 1},
    {IFTA90_E2T_03_B1_DOUBLEm, 7, 1},
    {IFTA90_E2T_03_B1_QUADm,   7, 1},
};

/*
 * For each IFTAx0_E2T_0y_* hit index profile,
 * set logical id to UFT/non-UFT index value from above structure.
 */
static ctr_eflex_hitbit_idx_profile_map_t ctr_eflex_hitbit_idx_profile_map[] = {
    /* IFTA80 memories */
    {IFTA80_E2T_00_HIT_INDEX_PROFILEm, 0},
    {IFTA80_E2T_01_HIT_INDEX_PROFILEm, 0},
    {IFTA80_E2T_02_HIT_INDEX_PROFILEm, 0},
    {IFTA80_E2T_03_HIT_INDEX_PROFILEm, 0},

    /* IFTA90 memories */
    {IFTA90_E2T_00_HIT_INDEX_PROFILEm, 1},
    {IFTA90_E2T_01_HIT_INDEX_PROFILEm, 1},
    {IFTA90_E2T_02_HIT_INDEX_PROFILEm, 1},
    {IFTA90_E2T_03_HIT_INDEX_PROFILEm, 1},
};

static ctr_eflex_hitbit_bank_map_t
    *s_hitbit_sid_log_id_map[BCMDRD_CONFIG_MAX_UNITS];

static bcmcth_ctr_eflex_device_info_t bcm56880_a0_device_info = {
        .num_ingress_pools = COUNTOF(ingress_counter_table),
        .num_egress_pools = COUNTOF(egress_counter_table),
        .num_ctr_section = NUM_CTR_SECTION,
        .num_pipes = NUM_PIPE,
        .num_pipe_instance = NUM_PIPE_INST,
        .num_ingress_action_id = NUM_CTR_EFLEX_ING_ACTION,
        .num_egress_action_id = NUM_CTR_EFLEX_EGR_ACTION,
        .num_rsvd_action_id = NUM_CTR_EFLEX_RSVD_ACTION,
        .num_op_profile_id = NUM_OPERAND,
        .num_range_checker = NUM_RANGE_CHECKER,
        .num_range_checker_per_bank = NUM_RANGE_CHECKER_PER_BANK,
        .num_objs = NUM_OBJECT,
        .num_global_objs = NUM_GLOBAL_OBJECT,
        .num_ing_actions = NUM_CTR_EFLEX_ING_SIM_ACTION,
        .num_egr_actions = NUM_CTR_EFLEX_EGR_SIM_ACTION,
        .num_slim_ctrs = NUM_SLIM_CTRS,
        .num_hitbit_ctr = NUM_CTR_EFLEX_HITBITS,
        .slim_ctr_row = SLIM_CTR_ROW,
};

static bcmcth_ctr_eflex_device_info_t bcm56880_a0_state_device_info = {
        .num_ingress_pools = COUNTOF(state_ingress_0_counter_table),
        .num_egress_pools = COUNTOF(state_egress_counter_table),
        .num_ctr_section = NUM_CTR_SECTION,
        .num_pipes = NUM_PIPE,
        .num_pipe_instance = NUM_PIPE_INST,
        .num_ingress_action_id = NUM_FLEX_STATE_ING_ACTION,
        .num_egress_action_id = NUM_FLEX_STATE_EGR_ACTION,
        .num_rsvd_action_id = NUM_FLEX_STATE_RSVD_ACTION,
        .num_op_profile_id = NUM_OPERAND,
        .num_range_checker = NUM_RANGE_CHECKER,
        .num_range_checker_per_bank = NUM_RANGE_CHECKER_PER_BANK,
        .num_objs = NUM_OBJECT,
        .num_global_objs = NUM_GLOBAL_OBJECT,
        .num_ing_actions = NUM_FLEX_STATE_ING_SIM_ACTION,
        .num_egr_actions = NUM_FLEX_STATE_EGR_SIM_ACTION,
        .state_ext.num_ingress_instance = NUM_FLEX_STATE_ING_INST,
        .state_ext.num_egress_instance = NUM_FLEX_STATE_EGR_INST
};

/*******************************************************************************
* Private functions
 */

/* Enhanced flex counter functions */

static int bank_map_comp(const void *x, const void *y)
{
    ctr_eflex_hitbit_bank_map_t *p1 = (ctr_eflex_hitbit_bank_map_t *)x;
    ctr_eflex_hitbit_bank_map_t *p2 = (ctr_eflex_hitbit_bank_map_t *)y;

    return (int)p1->bank_sid - (int)p2->bank_sid;
}

/*! \brief Function to perform binary search on a sorted table.
 *
 * \param [in] tbl Is the table to search in.
 * \param [in] item Is the item to search for.
 * \param [in] size Is the max index to use in the search. Typically this
 * will be the number of rows in the table - 1 (index starts for 0).
 * \param [out] idx Is the index in the table where the item was found. The
 * caller should define this parameter as type int and should initialize it to
 * -1 before the search. If the idx is still -1 after the search it indicates
 * that the element was not found.
 */
static inline void
bcm56880_a0_cth_ctr_eflex_bin_search(ctr_eflex_hitbit_bank_map_t *tbl,
                                     uint32_t item,
                                     uint32_t size,
                                     int *idx)
{
    int start = 0, end = size, middle, res;

    while (end >= start) {
        middle = start + (end - start) / 2;
        res = (int)tbl[middle].bank_sid - (int)item;
        if (res == 0) {
            *idx = middle;
            break;
        } else if (res < 0) {
            start = middle + 1;
        } else {
            end = middle - 1;
        }
    }

    return;
}

/*! Get list of COUNTER_TABLEm. */
static int
bcm56880_a0_cth_ctr_eflex_ctr_table_get(int unit,
                                        bool ingress,
                                        bcmdrd_sid_t **ctr_table_list,
                                        uint32_t *num)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num, SHR_E_PARAM);

    if (ingress) {
        *ctr_table_list = ingress_counter_table;
        *num = COUNTOF(ingress_counter_table);
    } else {
        *ctr_table_list = egress_counter_table;
        *num = COUNTOF(egress_counter_table);
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Get device and pool info */
static int
bcm56880_a0_cth_ctr_eflex_device_info_get(
    int unit,
    bcmcth_ctr_eflex_device_info_t **device_info)
{
    SHR_FUNC_ENTER(unit);

    *device_info = &bcm56880_a0_device_info;

    SHR_FUNC_EXIT();
}

/*! Set counter action enable reg */
static int
bcm56880_a0_cth_eflex_action_reg_set_common(
    int unit,
    bool enable,
    bcmdrd_sid_t sid,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    uint32_t *reg_buf = NULL, pipe_idx;
    size_t size, wsize;

    SHR_FUNC_ENTER(unit);
    size = bcmdrd_pt_entry_size(unit, sid);
    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(reg_buf, size, "bcmCthCtrEflexRegBuf");
    SHR_NULL_CHECK(reg_buf, SHR_E_MEMORY);
    sal_memset(reg_buf, 0, size);

    /* Set action enable register for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        sal_memset(reg_buf, 0, size);

        /* Read the current reg value */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, sid, &pt_dyn_info, NULL, reg_buf, wsize));

        if (enable) {
            /* Set the bit for specified action id */
            SHR_BITSET(reg_buf, entry->action_id);
        } else {
            /* Clear the bit for specified action id */
            SHR_BITCLR(reg_buf, entry->action_id);
        }

        /* Write the updated reg value */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, sid, &pt_dyn_info, NULL, reg_buf));
    }

exit:
    SHR_FREE(reg_buf);
    SHR_FUNC_EXIT();
}

/*! Set flex counter action enable reg */
static int
bcm56880_a0_cth_ctr_eflex_action_reg_set(int unit,
                                         bool enable,
                                         ctr_eflex_action_profile_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct register based on entry ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_ENABLEr :
                           FLEX_CTR_EGR_COUNTER_ACTION_ENABLEr;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_action_reg_set_common(unit, enable, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Set flex state action enable reg */
static int
bcm56880_a0_cth_state_eflex_action_reg_set(
    int unit,
    bool enable,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    if (entry->ingress) {
        switch (entry->state_ext.inst) {
            case ING_POST_LKUP_INST:
                sid = FLEX_CTR_ST_ING0_COUNTER_ACTION_ENABLEr;
                break;
            case ING_POST_FWD_INST:
                sid = FLEX_CTR_ST_ING1_COUNTER_ACTION_ENABLEr;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        sid = FLEX_CTR_ST_EGR_COUNTER_ACTION_ENABLEr;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_action_reg_set_common(unit, enable, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write counter action table 0 entry */
static int
bcm56880_a0_cth_eflex_action_table_0_write_common(
    int unit,
    bcmdrd_sid_t sid,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0, pipe_idx;
    uint32_t count_on_mirror;
    size_t wsize;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate entry buffer and clear */
    SHR_ALLOC(entry_buf,
              (wsize * sizeof(uint32_t)),
              "bcmCthCtrEflexAction0Buf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Write action table 0 for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = entry->action_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /*
         * Read current PT entry.
         * Part of ACTION_TABLE_0 entry can be written by TRIGGER LT
         */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_read(unit,
                              entry->req_flags,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              wsize,
                              req_ltid,
                              entry_buf,
                              &rsp_ltid,
                              &rsp_flags));

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_0f,
                                         &entry->obj[0]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_1f,
                                         &entry->obj[1]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_2f,
                                         &entry->obj[2]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_3f,
                                         &entry->obj[3]));

        /* For ingress, count on mirror is always set to 1. */
        count_on_mirror = entry->ingress ? 1 : entry->count_on_mirror_egress;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNT_ON_MIRRORf,
                                        &count_on_mirror));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNT_ON_DROPf,
                                         &entry->count_on_rule_drop));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, POOL_BASEf,
                                         &entry->pool_idx));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNTER_MODEf,
                                         &entry->ctr_mode));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, CINDEX_BASEf,
                                         &entry->base_idx));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, CINDEX_OPf,
                                         &entry->op_profile_ctr_index));

        /* Flex counter data */
        if (entry->comp == CTR_EFLEX) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_1_OPf,
                                             &entry->op_profile_ctr_a));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_1_OPf,
                                             &entry->op_profile_ctr_b));
        } else if (entry->comp == STATE_EFLEX) {
            /* Flex state data */
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_1_OPf,
                    &entry->state_ext.op_profile_comp[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_1_OPf,
                    &entry->state_ext.op_profile_comp[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_2_OPf,
                &entry->state_ext.op_profile_upd_true[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_2_OPf,
                &entry->state_ext.op_profile_upd_true[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_3_OPf,
                &entry->state_ext.op_profile_upd_false[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_3_OPf,
                &entry->state_ext.op_profile_upd_false[UPDATE_MODE_B_IDX]));
        }

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Write counter action table 1 entry */
static int
bcm56880_a0_cth_eflex_action_table_1_write_common(
    int unit,
    bcmdrd_sid_t sid,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0, pipe_idx;
    size_t wsize;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate entry buffer and clear */
    SHR_ALLOC(entry_buf,
             (wsize * sizeof(uint32_t)),
             "bcmCthCtrEflexAction0Buf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Write action table 1 for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = entry->action_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, CVALUE_DATAf,
                                 &entry->ctr_val_data));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNT_ON_HW_DROPf,
                                 &entry->count_on_hw_drop));

        /* Flex counter data */
        if (entry->comp == CTR_EFLEX) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    CONDITIONAL_EVICTION_OPf, &entry->evict_comp));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                     CONDITIONAL_EVICTION_RESETf,
                                     &entry->evict_reset));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_UPDATE_FN_TRUEf,
                                &entry->ctr_update_mode[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_UPDATE_FN_TRUEf,
                                &entry->ctr_update_mode[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_1_SELECTf,
                                     &entry->ctr_src_a));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_1_SELECTf,
                                     &entry->ctr_src_b));
        } else if (entry->comp == STATE_EFLEX) {
            /* Flex state data */
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_COMPARE_FNf,
                        &entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_COMPARE_FNf,
                        &entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_UPDATE_FN_TRUEf,
                        &entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_UPDATE_FN_TRUEf,
                        &entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_UPDATE_FN_FALSEf,
                        &entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_UPDATE_FN_FALSEf,
                        &entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_1_SELECTf,
                        &entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_1_SELECTf,
                        &entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_2_SELECTf,
                        &entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_2_SELECTf,
                        &entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_3_SELECTf,
                    &entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_3_SELECTf,
                    &entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX]));
        }

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter action table entry */
static int
bcm56880_a0_cth_ctr_eflex_action_table_write(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0m;

    /* Write ACTION_TABLE_0 entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_action_table_0_write_common(unit, sid, entry));

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_1m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_1m;

    /* Write ACTION_TABLE_1 entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_action_table_1_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state action table entry */
static int
bcm56880_a0_cth_state_eflex_action_table_write(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmdrd_sid_t sid_0, sid_1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (entry->ingress) {
        switch (entry->state_ext.inst) {
            case ING_POST_LKUP_INST:
                sid_0 = FLEX_CTR_ST_ING0_COUNTER_ACTION_TABLE_0m;
                sid_1 = FLEX_CTR_ST_ING0_COUNTER_ACTION_TABLE_1m;
                break;
            case ING_POST_FWD_INST:
                sid_0 = FLEX_CTR_ST_ING1_COUNTER_ACTION_TABLE_0m;
                sid_1 = FLEX_CTR_ST_ING1_COUNTER_ACTION_TABLE_1m;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        sid_0 = FLEX_CTR_ST_EGR_COUNTER_ACTION_TABLE_0m;
        sid_1 = FLEX_CTR_ST_EGR_COUNTER_ACTION_TABLE_1m;
    }

    /* Write ACTION_TABLE_0 entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_action_table_0_write_common(unit, sid_0, entry));

    /* Write ACTION_TABLE_1 entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_action_table_1_write_common(unit, sid_1, entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_eflex_ing_bitp_profile_set_common(int unit, bcmdrd_sid_t sid)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0;
    uint32_t mirror_copy_value = 1, mirror_copy_mask = 0;
    size_t wsize;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(entry_buf, (wsize * sizeof(uint32_t)), "bcmCthCtrEflexIngBitBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    pt_dyn_info.index = 0;

    /* Read PT entry fields */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          0,
                          sid,
                          &pt_dyn_info,
                          NULL,
                          wsize,
                          req_ltid,
                          entry_buf,
                          &rsp_ltid,
                          &rsp_flags));

    /* Modify PT entry fields */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, sid, entry_buf, MIRROR_COPY_VALUEf,
                                     &mirror_copy_value));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, sid, entry_buf, MIRROR_COPY_MASKf,
                                     &mirror_copy_mask));

    /* Write PT entry */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           0,
                           sid,
                           &pt_dyn_info,
                           NULL,
                           entry_buf,
                           req_ltid,
                           &rsp_ltid,
                           &rsp_flags));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_eflex_egr_bitp_profile_set_common(int unit,
                                                  bool flex_ctr,
                                                  bcmdrd_sid_t sid)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0;
    size_t wsize;
    uint64_t val = 0;
    uint32_t tmp_buf[2];
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(entry_buf, (wsize * sizeof(uint32_t)), "bcmCthCtrEflexEgrBitBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    pt_dyn_info.index = 0;

    /* Read PT entry fields */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          0,
                          sid,
                          &pt_dyn_info,
                          NULL,
                          wsize,
                          req_ltid,
                          entry_buf,
                          &rsp_ltid,
                          &rsp_flags));

    /* Modify PT entry fields */
    if (flex_ctr) {
        rv = bcmlrd_enum_symbol_to_scalar_by_type(
                unit,
                "CTR_EGR_EFLEX_CMD_BUS_LTS_T",
                "MIRROR_PKT_CTRL",
                &val);

        if (SHR_SUCCESS(rv)) {
            /* Convert logical offset to physical by dividing by 2 */
            val /= 2;

            /*
             * Store u64 as u32 array to populate entry_buf in little endian
             * order as expected by the bcmdrd API
             */
            tmp_buf[1] = (uint32_t)(val >> 32);
            tmp_buf[0] = (uint32_t)(val);

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     sid,
                                     entry_buf,
                                     BITP_MUX_SELf,
                                     tmp_buf));
        }
    }

    val = 0;
    tmp_buf[1] = 0;
    tmp_buf[0] = 0;

    rv = bcmlrd_enum_symbol_to_scalar_by_type(unit,
                                              "CTR_EGR_EFLEX_MIRROR_PKT_CTRL_T",
                                              "MIRROR_COPY",
                                              &val);

    if (SHR_SUCCESS(rv)) {
        /*
         * Store u64 as u32 array to populate entry_buf in little endian
         * order as expected by the bcmdrd API
         */
        tmp_buf[1] = (uint32_t)(val >> 32);
        tmp_buf[0] = (uint32_t)(val);

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 sid,
                                 entry_buf,
                                 MIRROR_COPY_VALUEf,
                                 tmp_buf));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 sid,
                                 entry_buf,
                                 MIRROR_COPY_MASKf,
                                 tmp_buf));

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               0,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    }

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter bitp profiles */
static int
bcm56880_a0_cth_ctr_eflex_bitp_profile_set(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_ing_bitp_profile_set_common(
            unit,
            FLEX_CTR_ING_BITP_PROFILEm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_egr_bitp_profile_set_common(
            unit,
            true,
            FLEX_CTR_EGR_BITP_PROFILEm));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state bitp profiles */
static int
bcm56880_a0_cth_state_eflex_bitp_profile_set(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_ing_bitp_profile_set_common(
            unit,
            FLEX_CTR_ST_ING0_BITP_PROFILEm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_ing_bitp_profile_set_common(
            unit,
            FLEX_CTR_ST_ING1_BITP_PROFILEm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_egr_bitp_profile_set_common(
            unit,
            false,
            FLEX_CTR_ST_EGR_BITP_PROFILEm));

exit:
    SHR_FUNC_EXIT();
}

/*! Write operand profile table entry */
static int
bcm56880_a0_cth_eflex_op_profile_write_common(
    int unit,
    bcmdrd_sid_t sid,
    ctr_eflex_op_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0, pipe_idx;
    size_t wsize;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(entry_buf,
             (wsize * sizeof(uint32_t)),
             "bcmCthCtrEflexOpEntryBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Write operand profile table for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = entry->op_profile_index;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SEL_1f, &entry->obj_1));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SEL_2f, &entry->obj_2));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MASK_SIZE_1f, &entry->mask_size_1));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MASK_SIZE_2f, &entry->mask_size_2));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SHIFT_1f, &entry->shift_1));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SHIFT_2f, &entry->shift_2));

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter operand profile table entry */
static int
bcm56880_a0_cth_ctr_eflex_op_profile_write(
    int unit,
    ctr_eflex_op_profile_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_OP_PROFILE_TABLEm :
                           FLEX_CTR_EGR_COUNTER_OP_PROFILE_TABLEm;

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_op_profile_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state operand profile table entry */
static int
bcm56880_a0_cth_state_eflex_op_profile_write(
    int unit,
    ctr_eflex_op_profile_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    if (entry->ingress) {
        switch (entry->inst) {
            case ING_POST_LKUP_INST:
                sid = FLEX_CTR_ST_ING0_COUNTER_OP_PROFILE_TABLEm;
                break;
            case ING_POST_FWD_INST:
                sid = FLEX_CTR_ST_ING1_COUNTER_OP_PROFILE_TABLEm;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        sid = FLEX_CTR_ST_EGR_COUNTER_OP_PROFILE_TABLEm;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_op_profile_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write range check profile table entry */
static int
bcm56880_a0_cth_eflex_range_chk_write_common(
    int unit,
    bcmdrd_sid_t sid,
    ctr_eflex_range_chk_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0, pipe_idx;
    size_t wsize;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(entry_buf, (wsize * sizeof(uint32_t)), "bcmCthCtrEflexRngChkBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Write range check table for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = entry->bank_num;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MAXf, &entry->max));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MINf, &entry->min));
        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter range check profile table entry */
static int
bcm56880_a0_cth_ctr_eflex_range_chk_write(
    int unit,
    ctr_eflex_range_chk_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ?
          FLEX_CTR_ING_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIGm :
          FLEX_CTR_EGR_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIGm;

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_range_chk_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state range check profile table entry */
static int
bcm56880_a0_cth_state_eflex_range_chk_write(
    int unit,
    ctr_eflex_range_chk_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    if (entry->ingress) {
        switch (entry->inst) {
            case ING_POST_LKUP_INST:
                sid = FLEX_CTR_ST_ING0_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIGm;
                break;
            case ING_POST_FWD_INST:
                sid = FLEX_CTR_ST_ING1_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIGm;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        sid = FLEX_CTR_ST_EGR_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIGm;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_range_chk_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write trigger entry */
static int
bcm56880_a0_cth_eflex_trigger_write_common(
    int unit,
    bcmdrd_sid_t sid,
    ctr_eflex_trigger_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t *data_ptr = NULL;
    uint32_t rsp_flags = 0, pipe_idx;
    size_t wsize;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate entry buffer and clear */
    SHR_ALLOC(entry_buf,
             (wsize * sizeof(uint32_t)),
             "bcmCthCtrEflexTriggerBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Write action table 0 for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = entry->action_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /*
         * Read current PT entry.
         * Part of ACTION_TABLE_0 entry can be written by
         * STATE/CTR_xxx_EFLEX_ACTION_PROFILE LT
         */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_read(unit,
                              entry->req_flags,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              wsize,
                              req_ltid,
                              entry_buf,
                              &rsp_ltid,
                              &rsp_flags));

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, TRIGGER_ENABLEf,
                                         &entry->trigger));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, START_TRIGGER_TYPEf,
                                         &entry->start));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, STOP_TRIGGER_TYPEf,
                                         &entry->stop));

        if (entry->start == TIME) {
            data_ptr = (uint32_t *)(&(entry->start_time));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_START_TIMEf, data_ptr));
        } else if (entry->start == START_CONDITION) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_START_VALUEf, &entry->start_value));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));
        }

        if (entry->stop == PERIOD) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, PERIOD_MAXf,
                                            &entry->num_periods));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, SCALEf,
                                             &entry->scale));
        } else if (entry->stop == STOP_CONDITION) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_STOP_VALUEf, &entry->stop_value));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));
        }

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter trigger entry */
static int
bcm56880_a0_cth_ctr_eflex_trigger_write(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0m;

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_trigger_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state trigger entry */
static int
bcm56880_a0_cth_state_eflex_trigger_write(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    if (entry->ingress) {
        switch (entry->inst) {
            case ING_POST_LKUP_INST:
                sid = FLEX_CTR_ST_ING0_COUNTER_ACTION_TABLE_0m;
                break;
            case ING_POST_FWD_INST:
                sid = FLEX_CTR_ST_ING1_COUNTER_ACTION_TABLE_0m;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        sid = FLEX_CTR_ST_EGR_COUNTER_ACTION_TABLE_0m;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_trigger_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Read trigger entry */
static int
bcm56880_a0_cth_eflex_trigger_read_common(
    int unit,
    bcmdrd_sid_t sid,
    ctr_eflex_trigger_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0;
    size_t wsize;
    uint64_t tmp_data = 0;

    SHR_FUNC_ENTER(unit);
    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate entry buffer and clear */
    SHR_ALLOC(entry_buf,
             (wsize * sizeof(uint32_t)),
             "bcmCthCtrEflexTriggerBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Read action table 0 for given pipe */
    pt_dyn_info.index = entry->action_id;
    pt_dyn_info.tbl_inst = entry->pipe_idx;

    /* Read current PT entry */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          entry->req_flags,
                          sid,
                          &pt_dyn_info,
                          NULL,
                          wsize,
                          req_ltid,
                          entry_buf,
                          &rsp_ltid,
                          &rsp_flags));

    /* Parse PT entry fields */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, TRIGGER_ENABLEf,
                                     &entry->trigger));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, START_TRIGGER_TYPEf,
                                     &entry->start));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, STOP_TRIGGER_TYPEf,
                                     &entry->stop));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_START_TIMEf, (uint32_t *)(&tmp_data)));
    entry->start_time = tmp_data;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_START_VALUEf, &entry->start_value));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, PERIOD_MAXf,
                                    &entry->num_periods));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, SCALEf,
                                     &entry->scale));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_STOP_VALUEf, &entry->stop_value));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Read flex counter trigger entry */
static int
bcm56880_a0_cth_ctr_eflex_trigger_read(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0m;

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_trigger_read_common(unit, sid, entry));
exit:
    SHR_FUNC_EXIT();
}

/*! Read flex state trigger entry */
static int
bcm56880_a0_cth_state_eflex_trigger_read(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    if (entry->ingress) {
        switch (entry->inst) {
            case ING_POST_LKUP_INST:
                sid = FLEX_CTR_ST_ING0_COUNTER_ACTION_TABLE_0m;
                break;
            case ING_POST_FWD_INST:
                sid = FLEX_CTR_ST_ING1_COUNTER_ACTION_TABLE_0m;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        sid = FLEX_CTR_ST_EGR_COUNTER_ACTION_TABLE_0m;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_trigger_read_common(unit, sid, entry));
exit:
    SHR_FUNC_EXIT();
}

/*! Get flex counter physical object id */
static int
bcm56880_a0_cth_ctr_eflex_object_id_get(int unit,
                                        int select,
                                        uint64_t data)
{
    return (data / 4);
}

/*! Get flex state physical object id */
static int
bcm56880_a0_cth_state_eflex_object_id_get(int unit,
                                          int select,
                                          uint64_t data)
{
    return (data / 4);
}

/*! Get flex counter logical object id */
static int
bcm56880_a0_cth_ctr_eflex_logical_object_id_get(int unit,
                                                int select,
                                                uint64_t data)
{
    return (data * 4);
}

/*! Get flex state logical object id */
static int
bcm56880_a0_cth_state_eflex_logical_object_id_get(int unit,
                                                  int select,
                                                  uint64_t data)
{
    return (data * 4);
}

/*! Write group action entry */
static int
bcm56880_a0_cth_eflex_group_action_write_common(
    int unit,
    bcmdrd_sid_t grp_action_sid,
    bcmdrd_sid_t grp_action_bmp_sid,
    ctr_eflex_group_action_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    uint32_t *grp_action_reg_buf = NULL, *grp_action_bmp_reg_buf = NULL;
    uint32_t pipe_idx;
    size_t grp_action_size, grp_action_bmp_size;
    size_t i;

    SHR_FUNC_ENTER(unit);

    grp_action_size = bcmdrd_pt_entry_size(unit, grp_action_sid);
    grp_action_bmp_size = bcmdrd_pt_entry_size(unit, grp_action_bmp_sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(grp_action_reg_buf, grp_action_size, "bcmCthCtrEflexGrpRegBuf");
    SHR_NULL_CHECK(grp_action_reg_buf, SHR_E_MEMORY);
    sal_memset(grp_action_reg_buf, 0, grp_action_size);

    SHR_ALLOC(grp_action_bmp_reg_buf, grp_action_bmp_size,
              "bcmCthCtrEflexGrpBmpRegBuf");
    SHR_NULL_CHECK(grp_action_bmp_reg_buf, SHR_E_MEMORY);
    sal_memset(grp_action_bmp_reg_buf, 0, grp_action_bmp_size);

    /* Set group action registers for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, grp_action_sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Write FLEX_CTR_XXX_GROUP_ACTION_BITMAP_Xr register */
        sal_memset(grp_action_bmp_reg_buf, 0, grp_action_bmp_size);

        for (i = 0; i < entry->num_actions; i++) {
            if (entry->action_id_map[i]) {
                /* Set the bit for specified action id */
                SHR_BITSET(grp_action_bmp_reg_buf, entry->action_id_map[i]);
            }
        }

        /* Write the updated bitmap reg value */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit,
                            grp_action_bmp_sid,
                            &pt_dyn_info,
                            NULL,
                            grp_action_bmp_reg_buf));

        /* Write FLEX_CTR_XXX_GROUP_ACTION_Xr register */
        *grp_action_reg_buf = entry->action_id;

        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit,
                            grp_action_sid,
                            &pt_dyn_info,
                            NULL,
                            grp_action_reg_buf));
    } /* end FOR */

exit:
    SHR_FREE(grp_action_reg_buf);
    SHR_FREE(grp_action_bmp_reg_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter group action entry */
static int
bcm56880_a0_cth_ctr_eflex_group_action_write(
    int unit,
    ctr_eflex_group_action_data_t *entry)
{
    bcmdrd_sid_t grp_action_sid, grp_action_bmp_sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct register based on entry ingress value */
    grp_action_sid = ctr_grp_action_reg[entry->ingress][entry->group_id];
    grp_action_bmp_sid =
        ctr_grp_action_bmp_reg[entry->ingress][entry->group_id];

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_group_action_write_common(unit,
                                                         grp_action_sid,
                                                         grp_action_bmp_sid,
                                                         entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state group action entry */
static int
bcm56880_a0_cth_state_eflex_group_action_write(
    int unit,
    ctr_eflex_group_action_data_t *entry)
{
    int inst;
    bcmdrd_sid_t grp_action_sid, grp_action_bmp_sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* For egress entry, only 1 instance present */
    inst = entry->ingress ? entry->inst : 0;

    /* Select correct register based on entry ingress value */
    grp_action_sid =
        state_grp_action_reg[entry->ingress][inst][entry->group_id];
    grp_action_bmp_sid =
        state_grp_action_bmp_reg[entry->ingress][inst][entry->group_id];

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_group_action_write_common(unit,
                                                         grp_action_sid,
                                                         grp_action_bmp_sid,
                                                         entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write error stats entry */
static int
bcm56880_a0_cth_eflex_error_stats_write_common(
    int unit,
    bcmdrd_sid_t misconfig_sid,
    bcmdrd_sid_t too_many_actions_sid,
    ctr_eflex_error_stats_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    uint32_t *misconfig_reg_buf = NULL, *too_many_actions_reg_buf = NULL;
    size_t misconfig_size, too_many_actions_size;
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t rsp_flags = 0, pipe_idx;
    uint32_t *data_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    misconfig_size = bcmdrd_pt_entry_size(unit, misconfig_sid);
    too_many_actions_size = bcmdrd_pt_entry_size(unit, too_many_actions_sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(misconfig_reg_buf, misconfig_size, "bcmCthCtrEflexMiscfgRegBuf");
    SHR_NULL_CHECK(misconfig_reg_buf, SHR_E_MEMORY);
    sal_memset(misconfig_reg_buf, 0, misconfig_size);

    SHR_ALLOC(too_many_actions_reg_buf, too_many_actions_size,
              "bcmCthCtrEflexTooManyRegBuf");
    SHR_NULL_CHECK(too_many_actions_reg_buf, SHR_E_MEMORY);
    sal_memset(too_many_actions_reg_buf, 0, too_many_actions_size);

    /* Write error stats registers for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, misconfig_sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        /* Write ACTION_MISCONFIG stat */
        if (entry->upd_action_misconfig) {
            pt_dyn_info.index = 0;
            pt_dyn_info.tbl_inst = pipe_idx;

            sal_memset(misconfig_reg_buf, 0, misconfig_size);
            /* Modify PT entry field */
            data_ptr = (uint32_t *)(&(entry->action_misconfig));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, misconfig_sid,
                                     misconfig_reg_buf, COUNTf, data_ptr));
            /* Write PT entry */
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_write(unit,
                                   0,
                                   misconfig_sid,
                                   &pt_dyn_info,
                                   NULL,
                                   misconfig_reg_buf,
                                   req_ltid,
                                   &rsp_ltid,
                                   &rsp_flags));
        }

        /* Write TOO_MANY_ACTIONS stat */
        if (entry->upd_too_many_actions) {
            pt_dyn_info.index = 0;
            pt_dyn_info.tbl_inst = pipe_idx;
            sal_memset(too_many_actions_reg_buf, 0, too_many_actions_size);

            /* Modify PT entry field */
            data_ptr = (uint32_t *)(&(entry->too_many_actions));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     too_many_actions_sid,
                                     too_many_actions_reg_buf,
                                     COUNTf, data_ptr));

            /* Write PT entry */
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_write(unit,
                                   0,
                                   too_many_actions_sid,
                                   &pt_dyn_info,
                                   NULL,
                                   too_many_actions_reg_buf,
                                   req_ltid,
                                   &rsp_ltid,
                                   &rsp_flags));
        }

        /*
         * In global pipe operating mode,
         * write counter value only to first valid pipe and 0 to the rest.
         */
        if (!entry->pipe_unique) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "In global pipe mode, write to pipe %d only\n"), pipe_idx));

            /* Write 0 to remaining pipes */
            entry->action_misconfig = 0;
            entry->too_many_actions = 0;
        }
    } /* end FOR pipe */

exit:
    SHR_FREE(misconfig_reg_buf);
    SHR_FREE(too_many_actions_reg_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter error stats entry */
static int
bcm56880_a0_cth_ctr_eflex_error_stats_write(
    int unit,
    ctr_eflex_error_stats_data_t *entry)
{
    bcmdrd_sid_t misconfig_sid, too_many_actions_sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select error stats register sid */
    if (entry->ingress) {
        misconfig_sid = FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTERr;
        too_many_actions_sid = FLEX_CTR_ING_FC_TOO_MANY_ACTIONS_COUNTERr;
    } else {
        misconfig_sid = FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTERr;
        too_many_actions_sid = FLEX_CTR_EGR_FC_TOO_MANY_ACTIONS_COUNTERr;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_error_stats_write_common(unit,
                                                        misconfig_sid,
                                                        too_many_actions_sid,
                                                        entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state error stats entry */
static int
bcm56880_a0_cth_state_eflex_error_stats_write(
    int unit,
    ctr_eflex_error_stats_data_t *entry)
{
    bcmdrd_sid_t misconfig_sid, too_many_actions_sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select error stats register sid */
    if (entry->ingress) {
        switch (entry->inst) {
            case ING_POST_LKUP_INST:
                misconfig_sid = FLEX_CTR_ST_ING0_FC_ACTION_MISCONFIG_COUNTERr;
                too_many_actions_sid =
                    FLEX_CTR_ST_ING0_FC_TOO_MANY_ACTIONS_COUNTERr;
                break;
            case ING_POST_FWD_INST:
                misconfig_sid = FLEX_CTR_ST_ING1_FC_ACTION_MISCONFIG_COUNTERr;
                too_many_actions_sid =
                    FLEX_CTR_ST_ING1_FC_TOO_MANY_ACTIONS_COUNTERr;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        misconfig_sid = FLEX_CTR_ST_EGR_FC_ACTION_MISCONFIG_COUNTERr;
        too_many_actions_sid = FLEX_CTR_ST_EGR_FC_TOO_MANY_ACTIONS_COUNTERr;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_error_stats_write_common(unit,
                                                        misconfig_sid,
                                                        too_many_actions_sid,
                                                        entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Read error stats entry */
static int
bcm56880_a0_cth_eflex_error_stats_read_common(
    int unit,
    bcmdrd_sid_t misconfig_sid,
    bcmdrd_sid_t too_many_actions_sid,
    ctr_eflex_error_stats_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *misconfig_reg_buf = NULL, *too_many_actions_reg_buf = NULL;
    uint32_t misconfig_wsize, too_many_actions_wsize;
    size_t misconfig_size, too_many_actions_size;
    uint32_t rsp_flags = 0, pipe_idx;
    uint64_t tmp_data, req_flags;

    SHR_FUNC_ENTER(unit);

    misconfig_wsize = bcmdrd_pt_entry_wsize(unit, misconfig_sid);
    misconfig_size = misconfig_wsize * sizeof(uint32_t);

    too_many_actions_wsize = bcmdrd_pt_entry_wsize(unit, too_many_actions_sid);
    too_many_actions_size = too_many_actions_wsize * sizeof(uint32_t);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(misconfig_reg_buf, misconfig_size, "bcmCthCtrEflexMiscfgRegBuf");
    SHR_NULL_CHECK(misconfig_reg_buf, SHR_E_MEMORY);
    sal_memset(misconfig_reg_buf, 0, misconfig_size);

    SHR_ALLOC(too_many_actions_reg_buf, too_many_actions_size,
              "bcmCthCtrEflexTooManyRegBuf");
    SHR_NULL_CHECK(too_many_actions_reg_buf, SHR_E_MEMORY);
    sal_memset(too_many_actions_reg_buf, 0, too_many_actions_size);

    entry->action_misconfig = 0;
    entry->too_many_actions = 0;
    req_flags =
        bcmptm_lt_entry_attrib_to_ptm_req_flags(BCMLT_ENT_ATTR_GET_FROM_HW);

    /* Read error stats from all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, misconfig_sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Read ACTION_MISCONFIG stat */
        sal_memset(misconfig_reg_buf, 0, misconfig_size);
        SHR_IF_ERR_EXIT(bcmptm_ireq_read(unit,
                                         req_flags,
                                         misconfig_sid,
                                         &pt_dyn_info,
                                         NULL,
                                         misconfig_wsize,
                                         req_ltid,
                                         misconfig_reg_buf,
                                         &rsp_ltid,
                                         &rsp_flags));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_get(unit,
                                 misconfig_sid,
                                 misconfig_reg_buf,
                                 COUNTf,
                                 (uint32_t *)(&tmp_data)));
        entry->action_misconfig += tmp_data;

        /* Read TOO_MANY_ACTIONS stat */
        sal_memset(too_many_actions_reg_buf, 0, too_many_actions_size);
        SHR_IF_ERR_EXIT(bcmptm_ireq_read(unit,
                                         req_flags,
                                         too_many_actions_sid,
                                         &pt_dyn_info,
                                         NULL,
                                         too_many_actions_wsize,
                                         req_ltid,
                                         too_many_actions_reg_buf,
                                         &rsp_ltid,
                                         &rsp_flags));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_get(unit,
                                 too_many_actions_sid,
                                 too_many_actions_reg_buf, COUNTf,
                                 (uint32_t *)(&tmp_data)));

        entry->too_many_actions += tmp_data;
    }/* End for pipe */

exit:
    SHR_FREE(misconfig_reg_buf);
    SHR_FREE(too_many_actions_reg_buf);
    SHR_FUNC_EXIT();
}

/*! Read flex counter error stats entry */
static int
bcm56880_a0_cth_ctr_eflex_error_stats_read(
    int unit,
    ctr_eflex_error_stats_data_t *entry)
{

    bcmdrd_sid_t misconfig_sid, too_many_actions_sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select error stats register sid */
    if (entry->ingress) {
        misconfig_sid = FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTERr;
        too_many_actions_sid = FLEX_CTR_ING_FC_TOO_MANY_ACTIONS_COUNTERr;
    } else {
        misconfig_sid = FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTERr;
        too_many_actions_sid = FLEX_CTR_EGR_FC_TOO_MANY_ACTIONS_COUNTERr;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_error_stats_read_common(unit,
                                                       misconfig_sid,
                                                       too_many_actions_sid,
                                                       entry));
exit:
    SHR_FUNC_EXIT();
}

/*! Read flex state error stats entry */
static int
bcm56880_a0_cth_state_eflex_error_stats_read(
    int unit,
    ctr_eflex_error_stats_data_t *entry)
{
    bcmdrd_sid_t misconfig_sid, too_many_actions_sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select error stats register sid */
    if (entry->ingress) {
        switch (entry->inst) {
            case ING_POST_LKUP_INST:
                misconfig_sid = FLEX_CTR_ST_ING0_FC_ACTION_MISCONFIG_COUNTERr;
                too_many_actions_sid =
                    FLEX_CTR_ST_ING0_FC_TOO_MANY_ACTIONS_COUNTERr;
                break;
            case ING_POST_FWD_INST:
                misconfig_sid = FLEX_CTR_ST_ING1_FC_ACTION_MISCONFIG_COUNTERr;
                too_many_actions_sid =
                    FLEX_CTR_ST_ING1_FC_TOO_MANY_ACTIONS_COUNTERr;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        misconfig_sid = FLEX_CTR_ST_EGR_FC_ACTION_MISCONFIG_COUNTERr;
        too_many_actions_sid = FLEX_CTR_ST_EGR_FC_TOO_MANY_ACTIONS_COUNTERr;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_error_stats_read_common(unit,
                                                       misconfig_sid,
                                                       too_many_actions_sid,
                                                       entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Function to print sorted hitbit bank sid to logical id mapping */
static void
bcmcth_ctr_eflex_hitbit_sid_map_print(int unit,
                                      ctr_eflex_hitbit_bank_map_t **sid_map)
{
    int i;

    for (i = 0; i < COUNTOF(hitbit_sid_log_id_map); i++) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                "Bank sid %d, Name %s, table id %d, uft idx %d\n"),
                sid_map[unit][i].bank_sid,
                bcmdrd_pt_sid_to_name(unit, sid_map[unit][i].bank_sid),
                sid_map[unit][i].table_id,
                sid_map[unit][i].uft_idx));
    }

    return;
}

/*! Initialize flex counter hitbit bank sid to logical id mapping */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_sid_map_init(int unit, bool warm)
{
    uint32_t idx, num_hitbit_profile, num_entries, entry, hit_idx_sid;
    uint32_t *entry_buf = NULL, rsp_flags = 0, uft_idx;
    size_t wsize;
    bcmdrd_sym_info_t sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    void *tmp_alloc_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Sort hitbit bank memories based on sid */
    SHR_ALLOC(tmp_alloc_ptr,
              sizeof(hitbit_sid_log_id_map),
              "bcmcthCtrEflexHitbitMap");
    SHR_NULL_CHECK(tmp_alloc_ptr, SHR_E_MEMORY);
    sal_memcpy(tmp_alloc_ptr, hitbit_sid_log_id_map,
               sizeof(hitbit_sid_log_id_map));
    s_hitbit_sid_log_id_map[unit] = tmp_alloc_ptr;

    sal_qsort(s_hitbit_sid_log_id_map[unit],
              COUNTOF(hitbit_sid_log_id_map),
              sizeof(hitbit_sid_log_id_map[0]),
              bank_map_comp);

    num_hitbit_profile = COUNTOF(ctr_eflex_hitbit_idx_profile_map);

    if (!warm) {
        /* Write logical bank id to PT */
        for (idx = 0; idx < num_hitbit_profile; idx++) {
            hit_idx_sid = ctr_eflex_hitbit_idx_profile_map[idx].hit_idx_sid;
            uft_idx = ctr_eflex_hitbit_idx_profile_map[idx].uft_idx;

            sal_memset(&sinfo, 0, sizeof(sinfo));

            /* Get number of entries in HIT_INDEX_PROFILE PT */
            SHR_IF_ERR_EXIT(bcmdrd_pt_info_get(unit, hit_idx_sid, &sinfo));
            num_entries = sinfo.index_max + 1;

            wsize = bcmdrd_pt_entry_wsize(unit, hit_idx_sid);

            /* Allocate entry buffer and clear */
            SHR_ALLOC(entry_buf, (wsize * sizeof(uint32_t)),
                      "bcmCthCtrEflexHitIdxBuf");
            SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
            sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

            for (entry = 0; entry < num_entries; entry++) {
                sal_memset(&pt_dyn_info, 0, sizeof(pt_dyn_info));
                pt_dyn_info.index = entry;
                pt_dyn_info.tbl_inst = -1;

                /* Read HIT_INDEX_PROFILE PT entry */
                SHR_IF_ERR_EXIT
                    (bcmptm_ireq_read(unit,
                                      0,
                                      hit_idx_sid,
                                      &pt_dyn_info,
                                      NULL,
                                      wsize,
                                      req_ltid,
                                      entry_buf,
                                      &rsp_ltid,
                                      &rsp_flags));

                /* Modify PT entry field */
                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         hit_idx_sid,
                                         entry_buf,
                                         HIT_CONTEXTf,
                                         &uft_idx));

                /* Write PT entry */
                SHR_IF_ERR_EXIT
                    (bcmptm_ireq_write(unit,
                                       0,
                                       hit_idx_sid,
                                       &pt_dyn_info,
                                       NULL,
                                       entry_buf,
                                       req_ltid,
                                       &rsp_ltid,
                                       &rsp_flags));
            } /* end INNER FOR */

            SHR_FREE(entry_buf);
        } /* end OUTTER FOR */
    }

    /* For debug, print sorted hitbit bank sid to logical id mapping */
    if (PRINT_HITBIT_MAP) {
        bcmcth_ctr_eflex_hitbit_sid_map_print(unit,
                                              s_hitbit_sid_log_id_map);
    }

exit:
    SHR_FUNC_EXIT();
}

/*! De-initialize flex counter hitbit bank sid to logical id mapping */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_sid_map_deinit(int unit)
{
    SHR_FREE(s_hitbit_sid_log_id_map[unit]);
    return SHR_E_NONE;
}

/*! Get flex counter logical bank id for given hitbit bank sid */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_table_id_get(int unit,
                                              bcmdrd_sid_t sid,
                                              uint32_t *table_id,
                                              uint32_t *uft_idx)
{
    int idx = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(table_id, SHR_E_PARAM);
    SHR_NULL_CHECK(uft_idx, SHR_E_PARAM);

    bcm56880_a0_cth_ctr_eflex_bin_search(s_hitbit_sid_log_id_map[unit],
                                         sid,
                                         (COUNTOF(hitbit_sid_log_id_map) - 1),
                                         &idx);

    if (idx == -1) {
        /* Not found case */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Hitbit bank sid %d not found\n"), sid));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else {
        /*  Return physical table id and uft index */
        *table_id = s_hitbit_sid_log_id_map[unit][idx].table_id;
        *uft_idx = s_hitbit_sid_log_id_map[unit][idx].uft_idx;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Set shadow pool enable reg */
static int
bcm56880_a0_cth_eflex_shadow_pool_set_common(
    int unit,
    bool enable,
    bcmdrd_sid_t shdw_tbl_sid,
    bcmdrd_sid_t shdw_bmp_sid,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    uint32_t *tbl_buf = NULL, *bmp_buf = NULL;
    uint32_t pool_idx, pipe_idx, shdw_pool_idx, val, rsp_flags = 0;
    size_t tbl_wsize, bmp_wsize;
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;

    SHR_FUNC_ENTER(unit);
    tbl_wsize = bcmdrd_pt_entry_wsize(unit, shdw_tbl_sid);
    bmp_wsize = bcmdrd_pt_entry_wsize(unit, shdw_bmp_sid);

    /* Allocate shadow pool table and enable bitmap buffers and clear */
    SHR_ALLOC(tbl_buf, (tbl_wsize * sizeof(uint32_t)),
              "bcmCthCtrEflexShdwTblBuf");
    SHR_NULL_CHECK(tbl_buf, SHR_E_MEMORY);

    SHR_ALLOC(bmp_buf, (bmp_wsize * sizeof(uint32_t)),
              "bcmCthCtrEflexShdwBmpBuf");
    SHR_NULL_CHECK(bmp_buf, SHR_E_MEMORY);

    /* Set shadow pool enable registers for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, shdw_tbl_sid, pipe_idx, -1)) {
            LOG_DEBUG(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        /* Write SHADOW_POOL_MAP_ENABLE register */
        sal_memset(&pt_dyn_info, 0, sizeof(pt_dyn_info));
        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        sal_memset(bmp_buf, 0, (bmp_wsize * sizeof(uint32_t)));

        /* Read the current reg value */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit,
                           shdw_bmp_sid,
                           &pt_dyn_info,
                           NULL,
                           bmp_buf,
                           bmp_wsize));

        for (pool_idx = entry->start_shdw_pool_idx;
             pool_idx <= entry->end_shdw_pool_idx;
             pool_idx++) {

            if (enable) {
                /* Set the bit for specified shadow pool idx */
                SHR_BITSET(bmp_buf, pool_idx);
            } else {
                /* Clear the bit for specified shadow pool idx  */
                SHR_BITCLR(bmp_buf, pool_idx);
            }
        }

        /* Write the updated reg value */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, shdw_bmp_sid, &pt_dyn_info, NULL, bmp_buf));

        /* Write SHADOW_POOL_MAP_TABLE PT entry */
        for (pool_idx = entry->start_pool_idx,
             shdw_pool_idx = entry->start_shdw_pool_idx;
             pool_idx <= entry->end_pool_idx;
             pool_idx++, shdw_pool_idx++) {

            sal_memset(&pt_dyn_info, 0, sizeof(pt_dyn_info));
            /* Primary pool index */
            pt_dyn_info.index = pool_idx;
            pt_dyn_info.tbl_inst = pipe_idx;

            sal_memset(tbl_buf, 0, (tbl_wsize * sizeof(uint32_t)));

            /* If enable, then use shadow pool idx, else use 0 */
            val = enable ? shdw_pool_idx : 0;

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, shdw_tbl_sid, tbl_buf, SHADOW_POOLf,
                                     &val));

            /* Write PT entry */
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_write(unit,
                                   0,
                                   shdw_tbl_sid,
                                   &pt_dyn_info,
                                   NULL,
                                   tbl_buf,
                                   req_ltid,
                                   &rsp_ltid,
                                   &rsp_flags));
        }  /* end SHADOW_POOL_MAP_TABLE PT entry write FOR */
    } /* end pipe_idx FOR */

exit:
    SHR_FREE(tbl_buf);
    SHR_FREE(bmp_buf);
    SHR_FUNC_EXIT();
}

/*! Set flex counter shadow pool reg */
static int
bcm56880_a0_cth_ctr_eflex_shadow_pool_set(
    int unit,
    bool enable,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmdrd_sid_t shdw_tbl_sid, shdw_bmp_sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct register based on entry ingress value */
    if (entry->ingress) {
        shdw_tbl_sid = FLEX_CTR_ING_COUNTER_POOL_SHADOW_POOL_MAP_TABLEm;
        shdw_bmp_sid = FLEX_CTR_ING_COUNTER_POOL_SHADOW_POOL_MAP_ENABLEr;
    } else {
        shdw_tbl_sid = FLEX_CTR_EGR_COUNTER_POOL_SHADOW_POOL_MAP_TABLEm;
        shdw_bmp_sid = FLEX_CTR_EGR_COUNTER_POOL_SHADOW_POOL_MAP_ENABLEr;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_shadow_pool_set_common(unit,
                                                      enable,
                                                      shdw_tbl_sid,
                                                      shdw_bmp_sid,
                                                      entry));
exit:
    SHR_FUNC_EXIT();
}

/*! Set flex state shadow pool reg */
static int
bcm56880_a0_cth_state_eflex_shadow_pool_set(
    int unit,
    bool enable,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmdrd_sid_t shdw_tbl_sid;
    bcmdrd_sid_t shdw_bmp_sid;

    SHR_FUNC_ENTER(unit);

    /* Select correct register based on entry ingress value */
    if (entry->ingress) {
        switch (entry->state_ext.inst) {
            case ING_POST_LKUP_INST:
                shdw_tbl_sid =
                    FLEX_CTR_ST_ING0_COUNTER_POOL_SHADOW_POOL_MAP_TABLEm;
                shdw_bmp_sid =
                    FLEX_CTR_ST_ING0_COUNTER_POOL_SHADOW_POOL_MAP_ENABLEr;
                break;
            case ING_POST_FWD_INST:
                shdw_tbl_sid =
                    FLEX_CTR_ST_ING1_COUNTER_POOL_SHADOW_POOL_MAP_TABLEm;
                shdw_bmp_sid =
                    FLEX_CTR_ST_ING1_COUNTER_POOL_SHADOW_POOL_MAP_ENABLEr;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        shdw_tbl_sid = FLEX_CTR_ST_EGR_COUNTER_POOL_SHADOW_POOL_MAP_TABLEm;
        shdw_bmp_sid = FLEX_CTR_ST_EGR_COUNTER_POOL_SHADOW_POOL_MAP_ENABLEr;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_eflex_shadow_pool_set_common(unit,
                                                      enable,
                                                      shdw_tbl_sid,
                                                      shdw_bmp_sid,
                                                      entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_eflex_timestamp_init_common(int unit)
{
    bool ts_en = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, 1, &ts_en));
    if (!ts_en) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_ctr_eflex_timestamp_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcm56880_a0_cth_eflex_timestamp_init_common(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_state_eflex_timestamp_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcm56880_a0_cth_eflex_timestamp_init_common(unit));

exit:
    SHR_FUNC_EXIT();
}

/*! Write object quantization entry */
static int
bcm56880_a0_cth_eflex_obj_quant_write_common(
    int unit,
    bcmdrd_sid_t sid,
    ctr_eflex_obj_quant_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0, pipe_idx;
    size_t wsize;

    SHR_FUNC_ENTER(unit);

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(entry_buf,
             (wsize * sizeof(uint32_t)),
             "bcmCthCtrEflexObjQuantBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Write range check table for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.tbl_inst = pipe_idx;

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_0_ENABLEf, &entry->quantize[0]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_1_ENABLEf, &entry->quantize[1]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_2_ENABLEf, &entry->quantize[2]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_3_ENABLEf, &entry->quantize[3]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_0_OBJECTf, &entry->obj[0]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_1_OBJECTf, &entry->obj[1]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_2_OBJECTf, &entry->obj[2]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_3_OBJECTf, &entry->obj[3]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_0_1_MODEf, &entry->obj_0_1_mode));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 BANK_2_3_MODEf, &entry->obj_2_3_mode));

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*! Write flex counter object quantization entry */
static int
bcm56880_a0_cth_ctr_eflex_obj_quant_write(
    int unit,
    ctr_eflex_obj_quant_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_OBJECT_QUANTIZATION_CTRLr :
                           FLEX_CTR_EGR_OBJECT_QUANTIZATION_CTRLr;

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_obj_quant_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write flex state range check profile table entry */
static int
bcm56880_a0_cth_state_eflex_obj_quant_write(
    int unit,
    ctr_eflex_obj_quant_data_t *entry)
{
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    if (entry->ingress) {
        switch (entry->inst) {
            case ING_POST_LKUP_INST:
                sid = FLEX_CTR_ST_ING0_OBJECT_QUANTIZATION_CTRLr;
                break;
            case ING_POST_FWD_INST:
                sid = FLEX_CTR_ST_ING1_OBJECT_QUANTIZATION_CTRLr;
                break;
            default:
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        sid = FLEX_CTR_ST_EGR_OBJECT_QUANTIZATION_CTRLr;
    }

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_obj_quant_write_common(unit, sid, entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Function to check if OBJ_QUANTIZATION handlers should be
 * registered with IMM. For some devices, OBJ_QUANTIZATION LTs
 * are implemented as direct mapped and hence do not require IMM support.
 */
static int
bcm56880_a0_cth_ctr_eflex_obj_quant_imm_reg(int unit, bool *imm_reg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(imm_reg, SHR_E_PARAM);

    *imm_reg = true;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get number of flex counter/state objects and values
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] comp    Enhanced Flex component.
 * \param [in,out] num_obj Number of flex counter objects.
 * \param [out] obj Array of flex counter object values.
 *
 * \return SHR_E_NONE Success.
 */
static int
bcm56880_a0_cth_eflex_obj_info_get_common(
    int unit,
    bool ingress,
    eflex_comp_t comp,
    uint32_t *num_obj,
    uint32_t *obj)
{
    uint32_t i;
    const bcmltd_enum_type_t *enum_type;

    SHR_FUNC_ENTER(unit);

    /* Get counter objects enum for current device variant */
    if (comp == CTR_EFLEX) {
        if (ingress) {
            SHR_IF_ERR_EXIT
                (bcmlrd_enum_find_symbol_type(unit,
                                              "CTR_ING_EFLEX_OBJ_BUS_T",
                                              &enum_type));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlrd_enum_find_symbol_type(unit,
                                              "CTR_EGR_EFLEX_OBJ_BUS_T",
                                              &enum_type));
        }
    } else if (comp == STATE_EFLEX) {
        if (ingress) {
            SHR_IF_ERR_EXIT
                (bcmlrd_enum_find_symbol_type(unit,
                                              "FLEX_STATE_POST_LKUP_OBJ_BUS_T",
                                              &enum_type));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlrd_enum_find_symbol_type(unit,
                                              "FLEX_STATE_EGR_OBJ_BUS_T",
                                              &enum_type));
        }
    }

    /* Check if there is enough space in given array */
    if (enum_type->num_sym > (*num_obj)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Insufficient space for obj values [%d]\n"),
            *num_obj));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get number of counter objects for current device variant */
    *num_obj = enum_type->num_sym;

    /* Get counter objects values for current device variant */
    for (i = 0; i < (*num_obj); i++) {
        obj[i] = enum_type->sym[i].val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Get flex counter object info */
static int
bcm56880_a0_cth_ctr_eflex_obj_info_get(
    int unit,
    bool ingress,
    uint32_t *num_obj,
    uint32_t *obj)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_obj, SHR_E_PARAM);
    SHR_NULL_CHECK(obj, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_obj_info_get_common(
            unit,
            ingress,
            CTR_EFLEX,
            num_obj,
            obj));

exit:
    SHR_FUNC_EXIT();
}

/*! Get flex state object info */
static int
bcm56880_a0_cth_state_eflex_obj_info_get(
    int unit,
    bool ingress,
    uint32_t *num_obj,
    uint32_t *obj)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_obj, SHR_E_PARAM);
    SHR_NULL_CHECK(obj, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_obj_info_get_common(
            unit,
            ingress,
            STATE_EFLEX,
            num_obj,
            obj));

exit:
    SHR_FUNC_EXIT();
}

/*! Verify given pipe index is valid */
static int
bcm56880_a0_cth_eflex_pipe_valid_check_common(int unit,
                                              bool ingress,
                                              uint32_t pipe,
                                              bool *valid)
{
    uint32_t valid_pipe_map;
    int blktype;
    const bcmdrd_chip_info_t *cinfo = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get blktype */
    cinfo = bcmdrd_dev_chip_info_get(unit);

    if (ingress) {
        blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "ipipe");
    } else {
        blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "epipe");
    }

    /* Get valid pipe bitmap */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit,
                                        0,
                                        blktype,
                                        &valid_pipe_map));

    *valid = (valid_pipe_map & (1 << pipe));

exit:
    SHR_FUNC_EXIT();
}

/*! Verify given flex counter pipe index is valid */
static int
bcm56880_a0_cth_ctr_eflex_pipe_valid_check(int unit,
                                           bool ingress,
                                           uint32_t pipe,
                                           bool *valid)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(valid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_pipe_valid_check_common(unit,
                                                       ingress,
                                                       pipe,
                                                       valid));

exit:
    SHR_FUNC_EXIT();
}

/*! Verify given flex state pipe index is valid */
static int
bcm56880_a0_cth_state_eflex_pipe_valid_check(int unit,
                                             bool ingress,
                                             uint32_t pipe,
                                             bool *valid)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(valid, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_cth_eflex_pipe_valid_check_common(unit,
                                                       ingress,
                                                       pipe,
                                                       valid));

exit:
    SHR_FUNC_EXIT();
}

/*! Add hitbit control info to internal software state */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_info_add(
    int unit,
    bool notify,
    uint32_t skip_index,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_group_action_data_t *grp_action_entry,
    ctr_eflex_hitbit_lt_info_t **hitbit_lt_info)
{
    int i, num_actions;
    ctr_eflex_group_action_data_t dst_grp_action_entry = {0};
    ctr_eflex_group_action_data_t src_grp_action_entry = {0};

    SHR_FUNC_ENTER(unit);

    num_actions = entry->ingress ? bcm56880_a0_device_info.num_ing_actions :
                                   bcm56880_a0_device_info.num_egr_actions;

    dst_grp_action_entry.ingress = src_grp_action_entry.ingress =
                                                        entry->ingress;
    dst_grp_action_entry.comp = src_grp_action_entry.comp = entry->comp;

    /*
     * If both src and dst group action profile id are invalid,
     * return SHR_E_NOT_FOUND to set OPERATIONAL_STATE as INACTIVE
     */
    if ((entry->dst_grp_action_profile_id ==
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID) &&
        (entry->src_grp_action_profile_id ==
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID)) {

        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (skip_index != DST_LKUP_IDX &&
        entry->dst_grp_action_profile_id !=
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        dst_grp_action_entry.group_profile_id =
                                            entry->dst_grp_action_profile_id;

        /*
         * Lookup GROUP_ACTION_PROFILE LT with destination
         * group action profile id as key
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_lookup(unit,
                                                  &dst_grp_action_entry));

        /*
         * In pipe unique mode, if pipe index do not match,
         * then return error to set OPER_STATE to INACTIVE
         */
        if (entry->pipe_unique &&
            entry->pipe_idx != dst_grp_action_entry.pipe_idx) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Pipe index mismatch between hitbit entry "
                    "and dst group action profile id \n")));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    if (skip_index != SRC_LKUP_IDX &&
        entry->src_grp_action_profile_id !=
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        src_grp_action_entry.group_profile_id =
                                            entry->src_grp_action_profile_id;

        /*
         * Lookup GROUP_ACTION_PROFILE LT with source
         * group action profile id as key
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_lookup(unit,
                                                  &src_grp_action_entry));

        /*
         * In pipe unique mode, if pipe index do not match,
         * then return error to set OPER_STATE to INACTIVE
         */
        if (entry->pipe_unique &&
            entry->pipe_idx != src_grp_action_entry.pipe_idx) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Pipe index mismatch between hitbit entry "
                    "and src group action profile id \n")));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Insert %s Entry:\n"
            "lt_id %d, pipe unique %d, pipe idx %d ,"
            "dst_grp_action_profile_id %d ,"
            "src_grp_action_profile_id %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->lt_id, entry->pipe_unique, entry->pipe_idx,
            entry->dst_grp_action_profile_id,
            entry->src_grp_action_profile_id));

    /*
     * If insert call is made as part of bcmcth_ctr_eflex_hitbit_ctrl_notify(),
     * then copy the group profile id and associated action profile ids of the
     * group action profile id being currently inserted in GROUP_ACTION_PROFILE
     */
    if (notify) {
        if (skip_index == DST_LKUP_IDX) {
            dst_grp_action_entry.group_profile_id =
                                            grp_action_entry->group_profile_id;
            dst_grp_action_entry.action_profile_id =
                                            grp_action_entry->action_profile_id;
            for (i = 0; i < num_actions; i++) {
                dst_grp_action_entry.action_profile_id_map[i] =
                    grp_action_entry->action_profile_id_map[i];
            }
        } else if (skip_index == SRC_LKUP_IDX) {
            src_grp_action_entry.group_profile_id =
                                            grp_action_entry->group_profile_id;
            src_grp_action_entry.action_profile_id =
                                            grp_action_entry->action_profile_id;
            for (i = 0; i < num_actions; i++) {
                src_grp_action_entry.action_profile_id_map[i] =
                    grp_action_entry->action_profile_id_map[i];
            }
        }
    }

    /* Add hitbit control info to internal software state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_add_internal(
            unit,
            entry,
            ctrl,
            &dst_grp_action_entry,
            &src_grp_action_entry,
            hitbit_lt_info));

exit:
    SHR_FUNC_EXIT();
}

/*! Determine which dst/src index to skip during update */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_skip_idx_get(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_hitbit_ctrl_data_t *cur_entry,
    uint32_t *skip_index)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "Update %s Entry:\n"
            "lt_id %d, pipe unique %d, pipe idx %d ,"
            "dst_grp_action_profile_id %d ,"
            "src_grp_action_profile_id %d\n"),
            entry->ingress ? "Ingress" : "Egress",
            entry->lt_id, entry->pipe_unique, entry->pipe_idx,
            entry->dst_grp_action_profile_id,
            entry->src_grp_action_profile_id));

    /* Check if dst and/or src action profile id is updated */
    if ((cur_entry->dst_grp_action_profile_id ==
             entry->dst_grp_action_profile_id) &&
        (cur_entry->src_grp_action_profile_id ==
             entry->src_grp_action_profile_id)) {
        /* Both src and dst group action profile ids are NOT updated */
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Src & dst grp action profile id unchanged \n")));
        if (entry->dst_grp_action_profile_id ==
                CTR_EFLEX_INVALID_ACTION_PROFILE_ID &&
            entry->src_grp_action_profile_id ==
                CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
            /*
             * If both src and dst group action profile id are invalid,
             * return SHR_E_NOT_FOUND to set OPERATIONAL_STATE as INACTIVE
             */
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_EXIT();
        }
    } else if ((cur_entry->dst_grp_action_profile_id !=
                    entry->dst_grp_action_profile_id) &&
               (cur_entry->src_grp_action_profile_id !=
                    entry->src_grp_action_profile_id)) {
        /* Both src and dst group action profile ids are updated */
        *skip_index = CTR_EFLEX_INVALID_VAL;
    } else if (cur_entry->dst_grp_action_profile_id !=
               entry->dst_grp_action_profile_id) {
        /* Only dst group action profile ids is updated */
        *skip_index = SRC_LKUP_IDX;
    } else if (cur_entry->src_grp_action_profile_id !=
               entry->src_grp_action_profile_id) {
        /* Only src group action profile ids is updated */
        *skip_index = DST_LKUP_IDX;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Check if dst and src group action profiles are configured */
static int
bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_lookup_help(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry)
{
    ctr_eflex_group_action_data_t dst_grp_action_entry = {0};
    ctr_eflex_group_action_data_t src_grp_action_entry = {0};

    SHR_FUNC_ENTER(unit);

    /*
     * If both src and dst action profile id are invalid,
     * return SHR_E_NOT_FOUND to set OPERATIONAL_STATE as INACTIVE
     */
    if ((entry->dst_grp_action_profile_id ==
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID) &&
        (entry->src_grp_action_profile_id ==
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Lookup entry with dst action profile id key in GROUP_ACTION_PROFILE LT */
    if (entry->dst_grp_action_profile_id !=
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        dst_grp_action_entry.ingress = entry->ingress;
        dst_grp_action_entry.comp = entry->comp;
        dst_grp_action_entry.group_profile_id =
                                            entry->dst_grp_action_profile_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_lookup(unit, &dst_grp_action_entry));

        /*
         * If dst GROUP_ACTION_PROFILE LT entry found and its OPERATIONAL_STATE
         * is INACTIVE, then return SHR_E_NOT_FOUND to set
         * OPERATIONAL_STATE as INACTIVE
         */
        if (dst_grp_action_entry.oper_state == CTR_EFLEX_OPER_STATE_INACTIVE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        /*
         * In pipe unique mode, if pipe index do not match,
         * then return error to set OPER_STATE to INACTIVE
         */
        if (entry->pipe_unique &&
            entry->pipe_idx != dst_grp_action_entry.pipe_idx) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Pipe index mismatch between hitbit entry "
                    "and dst group action profile id \n")));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* Lookup entry with src action profile id key in GROUP_ACTION_PROFILE LT */
    if (entry->src_grp_action_profile_id !=
            CTR_EFLEX_INVALID_ACTION_PROFILE_ID) {
        src_grp_action_entry.ingress = entry->ingress;
        src_grp_action_entry.comp = entry->comp;
        src_grp_action_entry.group_profile_id =
                                            entry->src_grp_action_profile_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_group_action_lookup(unit, &src_grp_action_entry));

        /*
         * If src GROUP_ACTION_PROFILE LT entry found and its OPERATIONAL_STATE
         * is INACTIVE, then return SHR_E_NOT_FOUND to set
         * OPERATIONAL_STATE as INACTIVE
         */

        if (src_grp_action_entry.oper_state == CTR_EFLEX_OPER_STATE_INACTIVE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        /*
         * In pipe unique mode, if pipe index do not match,
         * then return error to set OPER_STATE to INACTIVE
         */
        if (entry->pipe_unique &&
            entry->pipe_idx != src_grp_action_entry.pipe_idx) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "Pipe index mismatch between hitbit entry "
                    "and src group action profile id \n")));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to calculate hash entry index. */
static int
bcm56880_a0_cth_ctr_eflex_hash_entry_index_calc(
    int unit,
    bcmltd_sid_t ltid,
    bcmdrd_sid_t sid,
    bcmptm_rm_hash_hit_context_t *hit_context,
    int tbl_inst,
    uint32_t bkt_num,
    uint32_t bkt_entry,
    void *context,
    uint32_t *entry_idx)
{
    uint32_t table_id = 0, uft_idx = 0, lkup_idx = 0;
    ctr_eflex_hitbit_lt_info_t *hitbit_lt_info;

    SHR_FUNC_ENTER(unit);

    /* Get hitbit lt info from context param */
    hitbit_lt_info = (ctr_eflex_hitbit_lt_info_t *)context;

    /* Get hash bank physical table id and uft index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_ctr_eflex_hitbit_table_id_get(unit,
                                                       sid,
                                                       &table_id,
                                                       &uft_idx));

    /*
     * Hash entry index format is as follows
     *
     * Bits 00-01 : Bucket entry number
     * Bits 02-15 : Bucket number
     * Bits 16-20 : Physical table id
     * Bits 21-31 : N/A
     */

    *entry_idx = ((table_id << 16)           |
                  ((bkt_num & 0x3FFF) << 2)  |
                  (bkt_entry & 0x3));

    /*
     * Add base index as offset due to overlap between
     * UFT tiles -- table id from 0 - 7 &
     * NON UFT tiles -- table id from 6 - 13.
     *
     * Thus 2 NON UFT tiles 6 & 7 are overlapped with 16K hitbits each.
     * The size for each UFT tile is 64K and for each NON UFT is 16K.
     * However, when the counters are allocated/laid out in the 2 pools,
     * each UFT and non-UFT tile will occupy 64K size. Thus for NON UFT tile,
     * it will utilize only 16K out of the 64K counters.
     *
     * In UFT case, the base index is set to 0.
     * In non-UFT case, the base index is set to non zero value
     *
     * For ingress, it is calculated as
     * 64 - (((64K * 6)/64)/128) = 16
     */

    lkup_idx = (hitbit_lt_info->enable[DST_LKUP_IDX]) ? DST_LKUP_IDX :
                                                        SRC_LKUP_IDX;

    *entry_idx += (hitbit_lt_info->base_idx[uft_idx] *
                   hitbit_lt_info->factor[lkup_idx]);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
        "ltid %d , sid %d, tbl inst %d, uft idx %d, tbl id %d "
        "offset %d factor %d, bkt_num %d, bkt_entry %d, entry idx 0x%x\n"),
        ltid, sid, tbl_inst, uft_idx, table_id,
        hitbit_lt_info->base_idx[uft_idx],
        hitbit_lt_info->factor[lkup_idx],
        bkt_num, bkt_entry, *entry_idx));

exit:
    SHR_FUNC_EXIT();
}

static bcmcth_ctr_eflex_drv_t bcm56880_a0_cth_ctr_eflex_drv = {
    .device_info_get_fn = bcm56880_a0_cth_ctr_eflex_device_info_get,
    .ctr_table_get_fn   = bcm56880_a0_cth_ctr_eflex_ctr_table_get,
    .action_reg_set_fn  = bcm56880_a0_cth_ctr_eflex_action_reg_set,
    .action_table_write_fn = bcm56880_a0_cth_ctr_eflex_action_table_write,
    .bitp_profile_set_fn = bcm56880_a0_cth_ctr_eflex_bitp_profile_set,
    .op_profile_write_fn = bcm56880_a0_cth_ctr_eflex_op_profile_write,
    .range_chk_write_fn = bcm56880_a0_cth_ctr_eflex_range_chk_write,
    .trigger_write_fn = bcm56880_a0_cth_ctr_eflex_trigger_write,
    .trigger_read_fn = bcm56880_a0_cth_ctr_eflex_trigger_read,
    .object_id_get_fn = bcm56880_a0_cth_ctr_eflex_object_id_get,
    .group_action_write_fn = bcm56880_a0_cth_ctr_eflex_group_action_write,
    .error_stats_write_fn = bcm56880_a0_cth_ctr_eflex_error_stats_write,
    .error_stats_read_fn = bcm56880_a0_cth_ctr_eflex_error_stats_read,
    .hitbit_sid_map_init_fn = bcm56880_a0_cth_ctr_eflex_hitbit_sid_map_init,
    .hitbit_sid_map_deinit_fn = bcm56880_a0_cth_ctr_eflex_hitbit_sid_map_deinit,
    .logical_object_id_get_fn = bcm56880_a0_cth_ctr_eflex_logical_object_id_get,
    .shadow_pool_set_fn = bcm56880_a0_cth_ctr_eflex_shadow_pool_set,
    .timestamp_init_fn = bcm56880_a0_cth_ctr_eflex_timestamp_init,
    .obj_quant_write_fn = bcm56880_a0_cth_ctr_eflex_obj_quant_write,
    .obj_quant_imm_reg_fn = bcm56880_a0_cth_ctr_eflex_obj_quant_imm_reg,
    .obj_info_get_fn = bcm56880_a0_cth_ctr_eflex_obj_info_get,
    .pipe_valid_check_fn = bcm56880_a0_cth_ctr_eflex_pipe_valid_check,
    .hitbit_ctrl_info_add_fn = bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_info_add,
    .hitbit_ctrl_skip_idx_get_fn =
                            bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_skip_idx_get,
    .hitbit_ctrl_lookup_help_fn =
                            bcm56880_a0_cth_ctr_eflex_hitbit_ctrl_lookup_help,
    .hash_entry_index_calc_fn = bcm56880_a0_cth_ctr_eflex_hash_entry_index_calc
};

/* Enhanced flex state functions */

/*! Get list of COUNTER_TABLEm. */
static int
bcm56880_a0_cth_state_eflex_ctr_table_get(int unit,
                                          bool ingress,
                                          state_eflex_inst_t inst,
                                          bcmdrd_sid_t **ctr_table_list,
                                          uint32_t *num)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num, SHR_E_PARAM);

    if (ingress) {
        switch (inst) {
            case ING_POST_LKUP_INST:
                *ctr_table_list = state_ingress_0_counter_table;
                *num = COUNTOF(state_ingress_0_counter_table);
                break;
            case ING_POST_FWD_INST:
                *ctr_table_list = state_ingress_1_counter_table;
                *num = COUNTOF(state_ingress_1_counter_table);
                break;
            default:
                break;
        }
    } else {
        *ctr_table_list = state_egress_counter_table;
        *num = COUNTOF(state_egress_counter_table);
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Get device and pool info */
static int
bcm56880_a0_cth_state_eflex_device_info_get(
    int unit,
    bcmcth_ctr_eflex_device_info_t **device_info)
{
    SHR_FUNC_ENTER(unit);

    *device_info = &bcm56880_a0_state_device_info;

    SHR_FUNC_EXIT();
}

static bcmcth_state_eflex_drv_t bcm56880_a0_cth_state_eflex_drv = {
    .device_info_get_fn = bcm56880_a0_cth_state_eflex_device_info_get,
    .ctr_table_get_fn   = bcm56880_a0_cth_state_eflex_ctr_table_get,
    .action_reg_set_fn  = bcm56880_a0_cth_state_eflex_action_reg_set,
    .action_table_write_fn = bcm56880_a0_cth_state_eflex_action_table_write,
    .bitp_profile_set_fn = bcm56880_a0_cth_state_eflex_bitp_profile_set,
    .op_profile_write_fn = bcm56880_a0_cth_state_eflex_op_profile_write,
    .range_chk_write_fn = bcm56880_a0_cth_state_eflex_range_chk_write,
    .trigger_write_fn = bcm56880_a0_cth_state_eflex_trigger_write,
    .trigger_read_fn = bcm56880_a0_cth_state_eflex_trigger_read,
    .object_id_get_fn = bcm56880_a0_cth_state_eflex_object_id_get,
    .group_action_write_fn = bcm56880_a0_cth_state_eflex_group_action_write,
    .error_stats_write_fn = bcm56880_a0_cth_state_eflex_error_stats_write,
    .error_stats_read_fn = bcm56880_a0_cth_state_eflex_error_stats_read,
    .logical_object_id_get_fn =
                           bcm56880_a0_cth_state_eflex_logical_object_id_get,
    .shadow_pool_set_fn  = bcm56880_a0_cth_state_eflex_shadow_pool_set,
    .timestamp_init_fn = bcm56880_a0_cth_state_eflex_timestamp_init,
    .obj_quant_write_fn = bcm56880_a0_cth_state_eflex_obj_quant_write,
    .obj_info_get_fn = bcm56880_a0_cth_state_eflex_obj_info_get,
    .pipe_valid_check_fn = bcm56880_a0_cth_state_eflex_pipe_valid_check
};

/*******************************************************************************
* Public functions
 */

bcmcth_ctr_eflex_drv_t *
bcm56880_a0_cth_ctr_eflex_drv_get(int unit)
{
    return &bcm56880_a0_cth_ctr_eflex_drv;
}

bcmcth_state_eflex_drv_t *
bcm56880_a0_cth_state_eflex_drv_get(int unit)
{
    return &bcm56880_a0_cth_state_eflex_drv;
}
