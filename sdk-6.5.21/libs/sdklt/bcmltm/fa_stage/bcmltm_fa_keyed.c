/*! \file bcmltm_fa_keyed.c
 *
 * Logical Table Manager Field Adaptation
 *
 * Logical Table Keyed LT field operations
 *
 * This file contains the LTM FA stage functions for Keyed Logical Table
 * purposes.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_debug.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_FIELDADAPTATION


/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_keyed_data_clear(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie)
{
    bcmltm_pt_keyed_op_info_t *keyed_op_info;
    uint8_t data_word_size;
    uint32_t entix;
    uint32_t *clear_buf;

    /*
     * Note that the expected node cookie is that for the PT insert
     * portion of UPDATE, in Pass 1 EE.
     */
    keyed_op_info = BCMLTM_KEYED_OP_INFO(node_cookie);
    data_word_size = keyed_op_info->data_word_size;

    for (entix = 0; entix < BCMLTM_PT_ENTRY_LIST_MAX; entix++) {
        clear_buf = ltm_buffer +
            keyed_op_info->input_data_buffer_offset[entix];
        sal_memset(clear_buf, 0, BCMLTM_WORDS2BYTES(data_word_size));
    }

    return SHR_E_NONE;
}

