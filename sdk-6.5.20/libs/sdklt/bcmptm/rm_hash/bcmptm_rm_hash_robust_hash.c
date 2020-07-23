/*! \file bcmptm_rm_hash_robust_hash.c
 *
 * Resource Manager for Hash Table.
 *
 * This file contains the routines that implement the robust hash feature.
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
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_robust_hash.h"
#include "bcmptm_rm_hash_pt_register.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMHASH

#define RM_HASH_REGS_ENTRY_WORDS_MAX               8

#define RM_HASH_REMAP_DATA_FIELD_START_BIT         0

#define RM_HASH_REMAP_DATA_FIELD_END_BIT          11

#define RM_HASH_ACTION_FIELD_START_BIT             0

#define RM_HASH_ACTION_FIELD_END_BIT              63

/*! Number of robust hash modules for a hash table */
#define RM_HASH_RHASH_MODULES_NUM                  2

/*! Remap table size */
#define RM_HASH_RHASH_REMAP_TABLE_SIZE           256

/*! Action table size */
#define RM_HASH_RHASH_ACTION_TABLE_SIZE          256

/*! Action table width */
#define RM_HASH_RHASH_ACTION_TABLE_WIDTH          64

/*! Action table depth */
#define RM_HASH_RHASH_ACTION_TABLE_DEPTH           4

/*******************************************************************************
 * Typedefs
 */
typedef struct rm_hash_rhash_ctrl_s {
    /*! If Robust hash is enabled. */
    bool enable;

    /*! If word boundary alignment is required. */
    bool alignment;

    /*! Random seed for robust hash. */
    int seed;

    /*! Flag indicating if it has been init. */
    bool init;

    /*! Remap table memories. */
    bcmdrd_sid_t remap_tab[RM_HASH_RHASH_MODULES_NUM];

    /*! Action table memories. */
    bcmdrd_sid_t action_tab[RM_HASH_RHASH_MODULES_NUM];

    /*! Remap Table bank 0 SW cache. */
    uint32_t remap_data_a[RM_HASH_RHASH_REMAP_TABLE_SIZE];

    /*! Remap Table bank 1 SW cache. */
    uint32_t remap_data_b[RM_HASH_RHASH_REMAP_TABLE_SIZE];

    /*! Action Table bank 0 SW cache. */
    uint64_t action_data_a[RM_HASH_RHASH_ACTION_TABLE_DEPTH];

    /*! Action Table bank 1 SW cache. */
    uint64_t action_data_b[RM_HASH_RHASH_ACTION_TABLE_DEPTH];

    /*! Next element. */
    struct rm_hash_rhash_ctrl_s *next;
} rm_hash_rhash_ctrl_t;

/*
 *! \brief Information about robust hash K+N modules for a device.
 *
 * This data structure describes the robust hash K+N modules for a device.
 */
typedef struct rm_hash_dev_rhash_ctrl_s {
    /*! \brief Number of robust hash K+N modules for a device. */
    uint16_t num_rhash;

    /*! \brief Pointer to the first robust hash K+N module */
    struct rm_hash_rhash_ctrl_s *head;
}rm_hash_dev_rhash_ctrl_t;

/*******************************************************************************
 * Private variables
 */
static rm_hash_dev_rhash_ctrl_t dev_rhash_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Insert a robust hash K+N module into the list of a device.
 *
 * \param [in] unit Unit number.
 * \param [in] rhash_ctrl Pointer to rm_hash_rhash_ctrl_t containing robust
 *             hash info.
 */
static void
rm_hash_rbust_info_insert(int unit, rm_hash_rhash_ctrl rhash_ctrl)
{
    rm_hash_rhash_ctrl head = dev_rhash_ctrl[unit].head;

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "robust hash registering...\n")));

    rhash_ctrl->next = head;
    dev_rhash_ctrl[unit].head = rhash_ctrl;
}

/*!
 * \brief Allocate memory for a rm_hash_rhash_ctrl_t structure.
 *
 * \param [in] unit Unit number.
 * \param [out] rhash_ctrl Buffer to recording the pointer to a
 *              rm_hash_rhash_ctrl_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_rbust_info_alloc(int unit, rm_hash_rhash_ctrl *rhash_ctrl)
{
    rm_hash_rhash_ctrl ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(ctrl, sizeof(rm_hash_rhash_ctrl_t), "bcmptmRmhashRobustHashInfo");
    SHR_NULL_CHECK(ctrl, SHR_E_MEMORY);
    sal_memset(ctrl, 0, sizeof(rm_hash_rhash_ctrl_t));
    *rhash_ctrl = ctrl;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free all the memory space used for robust hash management in a device.
 *
 * \param [in] unit Unit number.
 */
static void
rm_hash_rbust_info_free(int unit)
{
    rm_hash_rhash_ctrl tmp_ctrl = NULL, cur_ctrl = NULL;

    cur_ctrl = dev_rhash_ctrl[unit].head;
    while (cur_ctrl != NULL) {
        tmp_ctrl = cur_ctrl;
        cur_ctrl = cur_ctrl->next;
        SHR_FREE(tmp_ctrl);
    }
    dev_rhash_ctrl[unit].head = NULL;
    dev_rhash_ctrl[unit].num_rhash = 0;
}

/*!
 * \brief Write a physical table in interactive path.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table SID.
 * \param [in] sid Physical table SID.
 * \param [in] index Physical table index.
 * \param [in] entry_data Buffer to contain the entry content.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ireq_write(int unit,
                   bcmltd_sid_t lt_id,
                   bcmdrd_sid_t sid,
                   int index,
                   void *entry_data)
{
    bcmbd_pt_dyn_info_t dyn_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    dyn_info.index = index;
    dyn_info.tbl_inst = -1;
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           0,
                           sid,
                           &dyn_info,
                           NULL,
                           entry_data,
                           lt_id,
                           &rsp_ltid,
                           &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure robust hash remap and action table based on random seed.
 *
 * \param [in] unit Unit number.
 * \param [in] init Flag indicating if this is in init.
 * \param [in] warm Flag indicating if this is in warmboot.
 * \param [in] rhash_ctrl Pointer to a rm_hash_rhash_ctrl_t structure.
 * \param [in] write_cb Pointer to a bcmptm_rm_hash_robust_hash_write structure.
 * \param [in] ltid Logical table id.
 * \param [in] seed Random seed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_remap_and_action_config(int unit,
                                bool init,
                                bool warm,
                                rm_hash_rhash_ctrl rhash_ctrl,
                                bcmptm_rm_hash_robust_hash_write write_cb,
                                bcmltd_sid_t ltid,
                                uint32_t trans_id,
                                int seed)
{
    int index, mod_idx, action = 0, action_lo = 0;
    uint32_t random_data, swap, remap_rand;
    uint8_t action_arr[RM_HASH_RHASH_ACTION_TABLE_SIZE];
    uint32_t val64[2] = {0};
    uint32_t *remap_data[RM_HASH_RHASH_MODULES_NUM];
    uint64_t *action_data[RM_HASH_RHASH_MODULES_NUM], *pact = NULL;
    uint32_t entry[RM_HASH_REGS_ENTRY_WORDS_MAX];
    bcmltd_op_arg_t op_arg = {0};
    const bcmltd_op_arg_t *op_arg_p = &op_arg;

    SHR_FUNC_ENTER(unit);

    remap_data[0]  = rhash_ctrl->remap_data_a;
    remap_data[1]  = rhash_ctrl->remap_data_b;
    action_data[0] = rhash_ctrl->action_data_a;
    action_data[1] = rhash_ctrl->action_data_b;

    /* Set random seed. */
    sal_srand(seed);

    /* Fill remap and action tables with random data. */
    for (mod_idx = 0; mod_idx < RM_HASH_RHASH_MODULES_NUM; mod_idx++) {
        /*
         * Initialize local variable to fill action table with equal number
         * of 1s and 0s.
         */
        for (index = 0; index < RM_HASH_RHASH_ACTION_TABLE_SIZE; index++) {
            action_arr[index] = index % 2;
        }
        for (index = 0; index < RM_HASH_RHASH_REMAP_TABLE_SIZE; index++) {
            /* coverity[dont_call] */
            random_data = sal_rand();
            remap_rand = random_data & 0xFFF;
            sal_memset(entry, 0, sizeof(entry));
            bcmdrd_field_set(entry,
                             RM_HASH_REMAP_DATA_FIELD_START_BIT,
                             RM_HASH_REMAP_DATA_FIELD_END_BIT,
                             &remap_rand);
            if ((init == TRUE) && (warm == FALSE)) {
                /* This is in initialization stage. */
                SHR_IF_ERR_EXIT
                (rm_hash_ireq_write(unit,
                                    DEVICE_EM_BANK_INFOt,
                                    rhash_ctrl->remap_tab[mod_idx],
                                    index,
                                    entry));
            }
            if (init == FALSE) {
                if (write_cb == NULL) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                op_arg.trans_id = trans_id;
                SHR_IF_ERR_EXIT
                    (write_cb(unit,
                              ltid,
                              op_arg_p,
                              rhash_ctrl->remap_tab[mod_idx],
                              index,
                              entry));
            }
            /* Copy remap data to SW cache. */
            *(remap_data[mod_idx] + index) = remap_rand;
            /* Randomize action variable values. */
            swap = action_arr[index];
            action_arr[index] = action_arr[random_data %
                                           RM_HASH_RHASH_ACTION_TABLE_SIZE];
            action_arr[random_data % RM_HASH_RHASH_ACTION_TABLE_SIZE] = swap;
        }

        /* Write randomized action table values to memory. */
        action_lo = 0;
        for (index = 0; index < RM_HASH_RHASH_ACTION_TABLE_SIZE; index++) {
            action |= action_arr[index] <<
                      (index % (RM_HASH_RHASH_ACTION_TABLE_WIDTH / 2));
            if ((index + 1) % (RM_HASH_RHASH_ACTION_TABLE_WIDTH / 2) == 0) {
                if ((index + 1) % RM_HASH_RHASH_ACTION_TABLE_WIDTH == 0) {
                    val64[0] = action_lo;
                    val64[1] = action;
                    sal_memset(entry, 0, sizeof(entry));
                    bcmdrd_field_set(entry,
                                     RM_HASH_ACTION_FIELD_START_BIT,
                                     RM_HASH_ACTION_FIELD_END_BIT,
                                     val64);
                    if ((init == TRUE) && (warm == FALSE)) {
                        SHR_IF_ERR_EXIT
                            (rm_hash_ireq_write(unit,
                                                DEVICE_EM_BANK_INFOt,
                                                rhash_ctrl->action_tab[mod_idx],
                                                index / RM_HASH_RHASH_ACTION_TABLE_WIDTH,
                                                entry));
                    }
                    if (init == FALSE) {
                        if (write_cb == NULL) {
                            SHR_ERR_EXIT(SHR_E_PARAM);
                        }
                        op_arg.trans_id = trans_id;
                        SHR_IF_ERR_EXIT
                            (write_cb(unit,
                                      ltid,
                                      op_arg_p,
                                      rhash_ctrl->action_tab[mod_idx],
                                      index / RM_HASH_RHASH_ACTION_TABLE_WIDTH,
                                      entry));
                    }
                    /* Copy action data to SW cache */
                    pact = action_data[mod_idx] + (index / RM_HASH_RHASH_ACTION_TABLE_WIDTH);
                    *pact = (uint32_t)action;
                    *pact <<= 32;
                    *pact |= (uint32_t)action_lo;
                    action = 0;
                    action_lo = 0;
                } else {
                    action_lo = action;
                    action = 0;
                }
            }
        }
    }

    rhash_ctrl->seed = seed;
    rhash_ctrl->init = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_rbust_enable_get(int unit,
                                rm_hash_rhash_ctrl rhash_ctrl,
                                bool *enabled)
{
    SHR_FUNC_ENTER(unit);

    if (rhash_ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *enabled = rhash_ctrl->enable;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_rbust_enable_set(int unit,
                                rm_hash_rhash_ctrl rhash_ctrl,
                                bool enabled)
{
    SHR_FUNC_ENTER(unit);

    if (rhash_ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    rhash_ctrl->enable = enabled;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_rbust_alignment_get(int unit,
                                   rm_hash_rhash_ctrl rhash_ctrl,
                                   bool *alignment)
{
    SHR_FUNC_ENTER(unit);

    if (rhash_ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *alignment = rhash_ctrl->alignment;

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_robust_hash_table_index_get(int unit,
                                           rm_hash_rhash_ctrl rhash_ctrl,
                                           uint8_t *key,
                                           int n_bits,
                                           int *remap_idx,
                                           int *act_idx)
{
    int ix, n_bytes;
    int remap_table_index = 0, action_table_index = 0;
    int nibble_swapped_key = 0;

    n_bytes = (n_bits + 7) / 8;
    /* Generate Remap table index */
    for (ix = 0; ix < n_bytes; ix++) {
        remap_table_index = remap_table_index ^ key[ix];

        nibble_swapped_key = ((key[ix] & 0x0F) << 4) |
                             ((key[ix] & 0xF0) >> 4 );
        action_table_index = action_table_index ^ nibble_swapped_key;
    }

    *remap_idx = remap_table_index;
    *act_idx = action_table_index;
}

void
bcmptm_rm_hash_robust_hash_get(int unit,
                               rm_hash_rhash_ctrl rhash_ctrl,
                               int rh_idx,
                               uint8_t *key,
                               int n_bits,
                               int remap_table_index,
                               int action_table_index)
{
    int ix, n_bytes, byte_offset = 0, x;
    int append = 0;
    int key_size = 0;
    uint32_t remap_val, bit_pos = 0;
    uint16_t tmp;
    uint64_t action;
    int sr_bits = 0;
    int sl_bits = 0;

    action = 0;
    n_bytes = (n_bits + 7) / 8;

    /* Read Remap and Action table values from SW cache*/
    if (rh_idx == 1) {
        remap_val = rhash_ctrl->remap_data_b[remap_table_index];
        action = rhash_ctrl->action_data_b[action_table_index /
                                          RM_HASH_RHASH_ACTION_TABLE_WIDTH];
    } else {
        remap_val = rhash_ctrl->remap_data_a[remap_table_index];
        action = rhash_ctrl->action_data_a[action_table_index /
                                          RM_HASH_RHASH_ACTION_TABLE_WIDTH];
    }

    bit_pos = ((uint32_t)action_table_index) % RM_HASH_RHASH_ACTION_TABLE_WIDTH;
    /* Robust hash key transformation */
    if ((action & (((uint64_t)1) << bit_pos)) == 0) {
        /* PREPEND */
        for (ix = n_bytes; ix > 0; ix--) {
            key[ix + 1] = key[ix - 1];
        }
        byte_offset = n_bytes + (BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH / 8);
    } else {
        /* APPEND */
        key_size = n_bytes;
        byte_offset = n_bytes;
        append = 1;
    }

    /* Some device does not have byte boundary aligned robust hash values */
    x = n_bits & 0x7;

    tmp = (((remap_val >> 8) & 0xF) << 8) | (remap_val & 0xFF);
    tmp |= (action_table_index & 0xF) << 12;

    if ((rhash_ctrl->alignment == TRUE) && (x != 0) && (append == 1)){
        key[n_bits / 8] &= ~((((1 << (8 - x)) - 1)) << x);
        key[n_bits / 8] |= (tmp & ((1 << (8 - x)) - 1)) << x;
        key[(n_bits / 8) + 1] = (tmp >> (8 - x)) & 0xFF;
        key[(n_bits / 8) + 2] = tmp >> (16 - x);
    } else {
        key[key_size] = tmp & 0xFF;
        key[key_size + 1] = (tmp >> 8) & 0xFF;
    }

    if (rhash_ctrl->alignment == FALSE) {
        /* Adjust the byte boundary */
        if (n_bits % 8 != 0) {
            sr_bits = n_bits % 8;
            sl_bits = 8 - sr_bits;
            for (ix = byte_offset - 1; ix > 0; ix--) {
                key[ix] = key[ix] << sl_bits;
                key[ix] |= key[ix - 1] >> sr_bits;
            }
            key[ix] = key[ix] << sl_bits;
        }
    }
}

int
bcmptm_rm_hash_rbust_info_init(int unit,
                               const bcmptm_rm_hash_key_format_t *key_format,
                               rm_hash_rhash_ctrl *rhash_ctrl)
{
    rm_hash_rhash_ctrl tmp_ctrl = NULL, curr = dev_rhash_ctrl[unit].head;

    SHR_FUNC_ENTER(unit);

    for ( ; curr != NULL; curr = curr->next) {
        if ((curr->remap_tab[0] == key_format->rh_remap_a_sid[0]) &&
            (curr->remap_tab[1] == key_format->rh_remap_b_sid[0])) {
            *rhash_ctrl = curr;
            SHR_EXIT();
        }
    }
    /* The robust hash module has not been initialized. */
    SHR_IF_ERR_EXIT
        (rm_hash_rbust_info_alloc(unit, &tmp_ctrl));
    tmp_ctrl->alignment = key_format->alignment;
    tmp_ctrl->remap_tab[0]  = key_format->rh_remap_a_sid[0];
    tmp_ctrl->remap_tab[1]  = key_format->rh_remap_b_sid[0];
    tmp_ctrl->action_tab[0] = key_format->rh_act_a_sid[0];
    tmp_ctrl->action_tab[1] = key_format->rh_act_b_sid[0];

    rm_hash_rbust_info_insert(unit, tmp_ctrl);
    *rhash_ctrl = tmp_ctrl;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_rbust_info_config(int unit,
                                 bool warm,
                                 bool init,
                                 rm_hash_rhash_ctrl rhash_ctrl,
                                 bcmptm_rm_hash_robust_hash_write write_cb,
                                 bcmltd_sid_t ltid,
                                 uint32_t trans_id,
                                 int seed)
{
    SHR_FUNC_ENTER(unit);

    if (init == TRUE) {
        if (rhash_ctrl->init == TRUE) {
            SHR_EXIT();
        } else {
            /* Configure remap and action tables. */
            SHR_IF_ERR_EXIT
                (rm_hash_remap_and_action_config(unit, init, warm, rhash_ctrl,
                                                 write_cb, ltid, trans_id,
                                                 seed));
        }
    } else {
        if (rhash_ctrl->seed == seed) {
            SHR_EXIT();
        } else {
            SHR_IF_ERR_EXIT
                (rm_hash_remap_and_action_config(unit, init, warm, rhash_ctrl,
                                                 write_cb, ltid, trans_id,
                                                 seed));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_rbust_info_cleanup(int unit)
{
    rm_hash_rbust_info_free(unit);
}

