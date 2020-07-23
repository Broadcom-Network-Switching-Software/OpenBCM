/*! \file bcmltm_ee_model_keyed.c
 *
 * Logical Table Manager Execution Engine
 *
 * Logical table keyed implementations
 *
 * This file contains the LTM EE stage functions for LT operations
 * on modeled keyed tables.
 *
 * ALLOC  - not supported for modeled keyed LTs
 * INSERT
 * LOOKUP
 * DELETE
 * UPDATE
 *
 * To support these basic operations on modeled keyed LTs, the following
 * capabilities are required:
 *
 * PT insert
 * PT delete
 * PT lookup
 *
 * Note that the PTM interface uses the same functions for
 * hash and TCAM keyed tables.  The implementation of the LTM-PTM
 * interface here is common for the two table types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmptm/bcmptm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmltm/bcmltm_table_op_dop_info.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>
#include <bcmltm/bcmltm_stats_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_EXECUTIONENGINE


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Fill in a PT keyed memory request information structure.
 *
 * Prepare the memory request information structure for a keyed PT op
 * from the parameters and metadata of the LTM.
 *
 * This function will need expansion for TCAMs and PDD support.
 *
 * \param [in,out] mreq_info Pointer to the keyed memory request structure
 * \param [in] entry_key_list Pointer to the start of the array of arrays
 *                            holding the PT key entries.
 * \param [in] entry_data_list Pointer to the start of the array of arrays
 *                             holding the PT data entries.
 * \param [in] pdd_ldtype_list Pointer to the start of the array of
 *                             PDD logical data types.
 * \param [in] same_key Boolean value of whether this PT key has already
 *                      been used as the immediate prior keyed PT op
 *                      during this LTM operation.
 * \param [in] key_word_size The count of uint32_t values in each key entry
 *                            array element.
 * \param [in] data_word_size The count of uint32_t values in each data entry
 *                            array element.
 * \param [in] priority The TCAM priority specified by the application.
 */
static void
keyed_mreq_info_init(bcmptm_keyed_lt_mreq_info_t *mreq_info,
                     uint32_t **entry_key_list,
                     uint32_t **entry_data_list,
                     uint16_t *pdd_ldtype_list,
                     bool same_key,
                     uint8_t key_word_size,
                     uint8_t data_word_size,
                     uint32_t priority)
{
    mreq_info->entry_key_words = entry_key_list;
    mreq_info->entry_data_words = entry_data_list;
    mreq_info->pdd_l_dtyp = pdd_ldtype_list;
    mreq_info->entry_id = 0; /* For TCAMs */
    mreq_info->entry_pri = priority; /* For TCAMs */
    mreq_info->same_key = same_key;
    mreq_info->rsp_entry_key_buf_wsize = key_word_size;
    mreq_info->rsp_entry_data_buf_wsize = data_word_size;
}

/*!
 * \brief Fill in a PT keyed memory response information structure.
 *
 * Prepare the memory response information structure for a keyed PT op
 * from the parameters and metadata of the LTM.
 *
 * This function will need expansion for TCAMs and PDD support.
 *
 * \param [in,out] mrsp_info Pointer to the keyed memory response structure
 * \param [in] rsp_entry_key_list Pointer to the start of the array of
 *                                arrays holding the PT key entries.
 * \param [in] rsp_entry_data_list Pointer to the start of the array of
 *                                 arrays holding the PT data entries.
 * \param [in] rsp_pdd_ldtype_list Pointer to the start of the array of
 *                                 PDD logical data types.
 * \param [in] rsp_pt_op_info Pointer to the bcmptm_pt_op_info_t struct.
 */
static void
keyed_mrsp_info_init(bcmptm_keyed_lt_mrsp_info_t *mrsp_info,
                     uint32_t **rsp_entry_key_list,
                     uint32_t **rsp_entry_data_list,
                     uint16_t *rsp_pdd_ldtype_list,
                     bcmptm_pt_op_info_t *rsp_pt_op_info)
{
    /* These values must be initialized to provide memory for the returned
     * output values */
    mrsp_info->rsp_entry_key_words = rsp_entry_key_list;
    mrsp_info->rsp_entry_data_words = rsp_entry_data_list;
    mrsp_info->rsp_pdd_l_dtyp = rsp_pdd_ldtype_list;

    /* These are clearing the structure in preparation to receive output */
    /* For all Keyed */
    mrsp_info->rsp_entry_pipe = 0;
    /* For TCAMs */
    mrsp_info->rsp_entry_pri = 0;
    mrsp_info->pt_op_info = rsp_pt_op_info;
    if (rsp_pt_op_info) {
        sal_memset(rsp_pt_op_info, 0, sizeof(bcmptm_pt_op_info_t));
    }
}

/*!
 * \brief Common logic for LTM-PTM interface of keyed PT table ops.
 *
 * This function implements the common logic for the LTM-PTM interface
 * to dispatch INSERT, LOOKUP, and DELETE ops for keyed (hash or TCAM)
 * physical tables.  The PTM interface for each op is common, so the
 * preparatory logic is very similar.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 * \param [in] pt_op The PTM op type to be executed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmltm_ee_node_model_keyed_opcode(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie,
                                  bcmptm_op_type_t pt_op)
{
    uint32_t pt_flags;
    uint8_t key_word_size;
    uint8_t data_word_size;
    uint32_t entix;
    bcmltm_pt_keyed_op_info_t *keyed_op_info;
    bcmltm_pt_mem_args_t *cur_mem_args;
    bcmltm_wb_ptm_param_offsets_t param_offsets;
    bcmbd_pt_dyn_info_t cur_pt_dyn_info;
    uint32_t rsp_ltid;
    uint32_t rsp_flags;
    uint64_t req_flags;
    uint32_t ltid;
    bcmptm_keyed_lt_mreq_info_t mreq_info;
    bcmptm_keyed_lt_mrsp_info_t mrsp_info;
    bcmptm_pt_op_info_t rsp_pt_op_info;
    bool op_pt_info;
    uint32_t *input_entry_key_list[BCMLTM_PT_ENTRY_LIST_MAX];
    uint32_t *input_entry_data_list[BCMLTM_PT_ENTRY_LIST_MAX];
    uint32_t *output_entry_key_list[BCMLTM_PT_ENTRY_LIST_MAX];
    uint32_t *output_entry_data_list[BCMLTM_PT_ENTRY_LIST_MAX];
    uint16_t *pdd_ldtype_p = NULL;
    uint16_t pdd_ldtype_in[BCMLTM_PT_ENTRY_LIST_MAX];
    uint16_t pdd_ldtype_out[BCMLTM_PT_ENTRY_LIST_MAX];
    uint32_t pt_suppress_offset;
    int rv;

    SHR_FUNC_ENTER(unit);

    ltid = lt_md->params->ltid;
    op_pt_info = lt_state->table_op_pt_info_enable;

    keyed_op_info = BCMLTM_KEYED_OP_INFO(node_cookie);

    /*
     * Check if an LTA Transform has declared that PT
     * operations should be omitted during this LT op.
     */
    pt_suppress_offset = keyed_op_info->pt_suppress_offset;
    if ((pt_suppress_offset != BCMLTM_WB_OFFSET_INVALID) &&
        (ltm_buffer[pt_suppress_offset] != 0)) {
        SHR_EXIT();
    }

    cur_mem_args = keyed_op_info->mem_args;
    key_word_size = keyed_op_info->key_word_size;
    data_word_size = keyed_op_info->data_word_size;
    pt_flags = keyed_op_info->flags;


    if ((pt_op == BCMPTM_OP_INSERT) &&
        (lt_entry->opcode.lt_opcode == BCMLT_OPCODE_INSERT)) {
        /*
         * This is the PT INSERT of a LT INSERT op.
         * PT LOOKUP has already determined the entry does not
         * exist.  Now verify the LT entry limit is not exceeded.
         */
        if (lt_state->entry_count >= lt_state->max_entry_count) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Prepare PTM request flags */
    req_flags = cur_mem_args->flags;
    req_flags |= bcmptm_lt_entry_attrib_to_ptm_req_flags(lt_entry->attrib);

    /* Get PTM parameter offsets */
    (void)bcmltm_wb_ptm_param_offsets_get(keyed_op_info->param_offset,
                                          &param_offsets);

    /*
     * Check PT entry changed flag.
     *
     * Note that the current Keyed table implementation allows only
     * one PT association.  As such this flag is currently never set.
     * Nevertheless, this PT entry changed logic is present to allow
     * for easy future adaptations in case Keyed table implementations
     * changes.
     */
    if (pt_flags & BCMLTM_PT_OP_FLAGS_PT_CHANGED) {
        if (ltm_buffer[param_offsets.pt_changed] !=
            BCMLTM_PT_ENTRY_CHANGED_VALUE) {
            SHR_EXIT();
        }
    }

    /* Fresh copy for any adjustments with PT_OP_FLAGS */
    sal_memcpy(&cur_pt_dyn_info, cur_mem_args->pt_dyn_info,
               sizeof(bcmbd_pt_dyn_info_t));

    bcmltm_pt_dyn_info_get(pt_flags, &param_offsets,
                           ltm_buffer, &cur_pt_dyn_info);

    if (pt_flags & BCMLTM_PT_OP_FLAGS_ACCTYPE) {
        /*
         * Implementation pending
         */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_PDD_LDTYPE) {
        pdd_ldtype_p = pdd_ldtype_in;
    }

    /* Prepare the entry arrays */
    for (entix = 0; entix < BCMLTM_PT_ENTRY_LIST_MAX; entix++) {
        input_entry_key_list[entix] = ltm_buffer +
            keyed_op_info->input_key_buffer_offset[entix];

        input_entry_data_list[entix] = ltm_buffer +
            keyed_op_info->input_data_buffer_offset[entix];

        output_entry_key_list[entix] = ltm_buffer +
            keyed_op_info->output_key_buffer_offset[entix];

        output_entry_data_list[entix] = ltm_buffer +
            keyed_op_info->output_data_buffer_offset[entix];

        /* Initialize PDD arrays */
        pdd_ldtype_in[entix] =
            ltm_buffer[param_offsets.pdd_ldtype[entix]] & 0xffff;
        pdd_ldtype_out[entix] = 0;
    }

    /* Set up mreq info */
    keyed_mreq_info_init(&mreq_info,
                         input_entry_key_list,
                         input_entry_data_list,
                         pdd_ldtype_p,
                         (pt_op == BCMPTM_OP_INSERT),
                         key_word_size,
                         data_word_size,
                         ltm_buffer[param_offsets.priority]);

    /* Set up mrsq info */
    keyed_mrsp_info_init(&mrsp_info,
                         output_entry_key_list,
                         output_entry_data_list,
                         pdd_ldtype_out,
                         op_pt_info ? &rsp_pt_op_info : NULL);

    /*
     * Perform the PTM write operation.
     */
    bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                           BCMLRD_FIELD_PHYSICAL_TABLE_OP_COUNT);
    rv = bcmptm_mreq_keyed_lt(unit,
                              req_flags,
                              ltid,
                              &cur_pt_dyn_info,
                              cur_mem_args->pt_static_info,
                              pt_op,
                              &mreq_info,
                              lt_entry->trans_id, /* cseq_id */
                              &mrsp_info,
                              &rsp_ltid,
                              &rsp_flags);

    if (rv == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else if (rv == SHR_E_PARAM) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Process the response values - LTID and flags */
    if (op_pt_info) {
        /*
         * PT op info enable is checked inside this function
         * as well due to use by the PTM callbacks for CTH.
         * But the common case is disabled, so LTM internals will
         * be more efficient to skip this function entirely.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmltm_pt_status_mreq_keyed_lt(unit, ltid, lt_entry->trans_id,
                                            &cur_pt_dyn_info,
                                            &mrsp_info));

        /* Update the PT info in TABLE_OP_DOP_INFO LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmltm_dop_status_mreq_keyed_lt(unit, ltid,
                                            &cur_pt_dyn_info,
                                            &mrsp_info));
    };

    if (pt_flags & BCMLTM_PT_OP_FLAGS_PRIORITY) {
        /* Copy response TCAM priority to WB */
        ltm_buffer[param_offsets.priority] =
            mrsp_info.rsp_entry_pri;
    }

    if ((pt_flags & BCMLTM_PT_OP_FLAGS_TABLE_INST) &&
        ((pt_op == BCMPTM_OP_GET_NEXT) ||
         (pt_op == BCMPTM_OP_GET_FIRST))) {
        /* Copy response pipe to WB */
        ltm_buffer[param_offsets.table_inst] =
            mrsp_info.rsp_entry_pipe;
    }

    if (pt_flags & BCMLTM_PT_OP_FLAGS_PDD_LDTYPE) {
        for (entix = 0; entix < BCMLTM_PT_ENTRY_LIST_MAX; entix++) {
            ltm_buffer[param_offsets.pdd_ldtype[entix]] =
                mrsp_info.rsp_pdd_l_dtyp[entix];
        }
    }

exit:
    if (SHR_FUNC_ERR() &&
        !(SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND) ||
          SHR_FUNC_VAL_IS(SHR_E_EXISTS))) {  /* Unexpected errors */
        bcmltm_stats_increment(BCMLTM_STATS_ARRAY(lt_md),
                               BCMLRD_FIELD_PHYSICAL_TABLE_OP_ERROR_COUNT);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_ee_node_model_keyed_insert(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                           lt_state, lt_entry,
                                           ltm_buffer, node_cookie,
                                           BCMPTM_OP_INSERT));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_model_keyed_delete(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                            lt_state, lt_entry,
                                            ltm_buffer, node_cookie,
                                            BCMPTM_OP_DELETE));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_ee_node_model_keyed_lookup(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie)
{
    int rv;

    rv = bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_LOOKUP);
    if (rv == SHR_E_NOT_FOUND) {
        /*
         * An existing keyed entry matching this lookup
         * is required for this function node.
         */
        LOG_VERBOSE(BSL_LS_BCMLTM_EXECUTIONENGINE,
                    (BSL_META_U(unit,
                              "Required existing keyed entry not found\n")));
    }

    return rv;
}

int
bcmltm_ee_node_model_keyed_lookup_not_found(int unit,
                                            bcmltm_lt_md_t *lt_md,
                                            bcmltm_lt_state_t *lt_state,
                                            bcmltm_entry_t *lt_entry,
                                            uint32_t *ltm_buffer,
                                            void *node_cookie)
{
    int rv;

    rv = bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             BCMPTM_OP_LOOKUP);

    if (rv == SHR_E_NONE) {
        /*
         *Finding a previous entry is a failure.
         * INSERT must reference a new entry key.
         */
        LOG_VERBOSE(BSL_LS_BCMLTM_EXECUTIONENGINE,
                  (BSL_META_U(unit,
                              "Unanticipated existing keyed entry\n")));
        rv = SHR_E_EXISTS;
    } else if (rv == SHR_E_NOT_FOUND) {
        rv = SHR_E_NONE;
    }

    return rv;
}

int
bcmltm_ee_node_model_keyed_traverse(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie)
{
    bcmptm_op_type_t pt_op;

    /*
     * Can't have "no key" Keyed LTs, so non-NULL input list indicates
     * TRAVERSE next.
     */
    pt_op = (lt_entry->in_fields != NULL) ?
        BCMPTM_OP_GET_NEXT : BCMPTM_OP_GET_FIRST;

    return bcmltm_ee_node_model_keyed_opcode(unit, lt_md,
                                             lt_state, lt_entry,
                                             ltm_buffer, node_cookie,
                                             pt_op);
}
