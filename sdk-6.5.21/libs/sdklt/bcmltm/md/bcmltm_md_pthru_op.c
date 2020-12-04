/*! \file bcmltm_md_pthru_op.c
 *
 * Logical Table Manager - Pass Thru Opcode.
 *
 * This file contains routines to create and provide
 * the opcode metadata for Pass Thru tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_wb_internal.h>
#include <bcmltm/bcmltm_db_pthru_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>
#include <bcmltm/bcmltm_ee_tree_nodes_internal.h>

#include "bcmltm_md_op.h"
#include "bcmltm_md_pthru_op.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA

/* Number of roots for SET operation */
#define MD_PTHRU_OP_SET_NUM_ROOTS          1

/* Number of roots for MODIFY operation */
#define MD_PTHRU_OP_MODIFY_NUM_ROOTS       2

/* Number of roots for GET operation */
#define MD_PTHRU_OP_GET_NUM_ROOTS          2

/* Number of roots for LOOKUP operation */
#define MD_PTHRU_OP_LOOKUP_NUM_ROOTS       2

/*!
 * \brief Table Opcode Information.
 *
 * This structure contains the metadata for various opcodes for a table.
 */
typedef struct md_pthru_op_table_info_s {
    /*! Physical table ID. */
    bcmdrd_sid_t sid;

    /*! Opcode metadata array. */
    bcmltm_lt_op_md_t *op[BCMLT_PT_OPCODE_NUM];
} md_pthru_op_table_info_t;

/*!
 * \brief Opcode Information.
 *
 * This structure contains the metadata for various opcodes.
 *
 * This structure contains the information for one table.
 * This matches the current PT Pass Thru design (only 1 PT metadata
 * cached at a time).  If there is a need to cache more tables,
 * the member can be expanded to an array.
 *
 * Optimization:
 * If there is certainty that the design will remain unchanged
 * (store no more than one table), the per-table structure can be
 * removed and its content moved here.
 */
typedef struct md_pthru_op_info_s {
    /*! Opcode metadata (1 table). */
    md_pthru_op_table_info_t table;
} md_pthru_op_info_t;

/*!
 * \brief PT Pass Thru Opcode.
 *
 * This structure contains the PT Pass Thru opcode metadata.
 */
typedef struct md_pthru_op_s {
    /*! Total number of symbols. */
    uint32_t sid_max_count;

    /* Opcode information. */
    md_pthru_op_info_t info;
} md_pthru_op_t;

/* PT Pass Thru Opcode Metadata */
static md_pthru_op_t *md_pthru_op[BCMLTM_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check if opcode metadata has been initialized.
 *
 * This routine checks if the PT Pass Thru opcode metadata
 * has been initialized for the given unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE If opcode metadata has been initialized.
 * \retval FALSE If opcode metadata has not been initialized or unit is invalid.
 */
static inline bool
md_pthru_op_is_init(int unit)
{
    if ((unit < 0) || (unit >= BCMLTM_MAX_UNITS)) {
        return FALSE;
    }

    return (md_pthru_op[unit] != NULL);
}

/*!
 * \brief Check if table ID is valid.
 *
 * This routine checks if given physical table ID is valid.
 * It assumes that the unit is valid and the opcode metadata
 * has been initialized.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 *
 * \retval TRUE If table ID is valid.
 * \retval FALSE If table ID is invalid.
 */
static inline bool
md_pthru_op_sid_is_valid(int unit,
                         bcmdrd_sid_t sid)
{
    if (sid >= md_pthru_op[unit]->sid_max_count) {
        return FALSE;
    }

    return bcmdrd_pt_is_valid(unit, sid);
}

/*!
 * \brief Create FA tree roots for the SET operation.
 *
 * This function creates the FA tree roots for a SET operation
 * for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_fa_node_roots_set_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *value_a2p_node = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    const bcmltm_field_select_mapping_t *values;
    const bcmltm_field_select_mapping_t *keys;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmPthruSetFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_value_fields_get(unit, sid, &values));
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_key_fields_get(unit, sid, &keys));

    /*-----------------
     * Tree_0
     */

    /* [Value_A2P]: Copy API values to PT */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_value_fields_to_wb,
                                  (void *)values,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &value_a2p_node));
    fa_node_roots[0] = value_a2p_node;

    /* [Key_A2P]: Copy API keys to PT */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_value_fields_to_wb,
                                  (void *)keys,
                                  BCMLTM_NODE_SIDE_LEFT,
                                  value_a2p_node,
                                  op_md_ptr,
                                  &key_a2p_node));

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for a SET operation.
 *
 * This function creates the EE tree roots for a SET operation
 * for given PT Pass Thru table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_ee_node_roots_set_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *write_node = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmPthruSetEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_pt_list_get(unit, sid, &pt_list));

    /*-----------------
     * Tree_0
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_schan_write,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &write_node));
    ee_node_roots[0] = write_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create SET operation metadata.
 *
 * This routine creates the operation metadata for the SET opcode
 * for given PT Pass Thru table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_set_create(int unit,
                       bcmdrd_sid_t sid,
                       const bcmltm_wb_handle_t *wb_handle,
                       bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmPthruSetLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = MD_PTHRU_OP_SET_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_fa_node_roots_set_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_ee_node_roots_set_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for the MODIFY operation.
 *
 * This function creates the FA tree roots for a MODIFY operation
 * for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_fa_node_roots_modify_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    bcmltm_node_t *value_a2p_node = NULL;
    const bcmltm_field_select_mapping_t *keys;
    const bcmltm_field_select_mapping_t *values;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmPthruModifyFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_key_fields_get(unit, sid, &keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_value_fields_get(unit, sid, &values));

    /*-----------------
     * Tree_0
     */

    /* [Key_A2P]: Copy API keys to PT */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_value_fields_to_wb,
                                  (void *)keys,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &key_a2p_node));
    fa_node_roots[0] = key_a2p_node;

    /*-----------------
     * Tree_1
     */

    /* [Value_A2P]: Copy API values to PT */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_value_fields_to_wb,
                                  (void *)values,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &value_a2p_node));
    fa_node_roots[1] = value_a2p_node;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for a MODIFY operation.
 *
 * This function creates the EE tree roots for a MODIFY operation
 * for given PT Pass Thru table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_ee_node_roots_modify_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    bcmltm_node_t *write_node = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmPthruModifyEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_pt_list_get(unit, sid, &pt_list));

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_schan_read,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &read_node));
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */

    /* [Write]: Execute PTM write operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_schan_write,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &write_node));
    ee_node_roots[1] = write_node;

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create MODIFY operation metadata.
 *
 * This routine creates the operation metadata for the MODIFY opcode
 * for the given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_modify_create(int unit,
                          bcmdrd_sid_t sid,
                          const bcmltm_wb_handle_t *wb_handle,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmPthruModifyLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = MD_PTHRU_OP_MODIFY_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_fa_node_roots_modify_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_ee_node_roots_modify_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for the GET operation.
 *
 * This function creates the FA tree roots for a GET operation
 * for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_fa_node_roots_get_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    bcmltm_node_t *value_p2a_node = NULL;
    const bcmltm_field_select_mapping_t *keys;
    const bcmltm_field_select_mapping_t *values;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmPthruGetFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_key_fields_get(unit, sid, &keys));
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_value_fields_get(unit, sid, &values));

    /*-----------------
     * Tree_0
     */

    /* [Key_A2P]: Copy API keys to PT */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_value_fields_to_wb,
                                  (void *)keys,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &key_a2p_node));
    fa_node_roots[0] = key_a2p_node;

    /*-----------------
     * Tree_1
     */

    /* [Value_P2A]: PT fields to API values */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_wb_to_api_fields,
                                  (void *)values,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &value_p2a_node));
    fa_node_roots[1] = value_p2a_node;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for a GET operation.
 *
 * This function creates the EE tree roots for a GET operation
 * for given PT Pass Thru table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_ee_node_roots_get_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *read_node = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmPthruGetEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_pt_list_get(unit, sid, &pt_list));

    /*-----------------
     * Tree_0
     */

    /* [Read]: Execute PTM read operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_schan_read,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &read_node));
    ee_node_roots[0] = read_node;

    /*-----------------
     * Tree_1
     */

    /* [NULL]: No actions */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create GET operation metadata.
 *
 * This routine creates the operation metadata for the GET opcode
 * for the given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_get_create(int unit,
                       bcmdrd_sid_t sid,
                       const bcmltm_wb_handle_t *wb_handle,
                       bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmPthruGetLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = MD_PTHRU_OP_GET_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_fa_node_roots_get_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_ee_node_roots_get_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create FA tree roots for the LOOKUP operation.
 *
 * This function creates the FA tree roots for a LOOKUP operation
 * for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_fa_node_roots_lookup_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **fa_node_roots = NULL;
    bcmltm_node_t *key_a2p_node = NULL;
    bcmltm_node_t *value_p2a_node = NULL;
    const bcmltm_field_select_mapping_t *keys;
    const bcmltm_field_select_mapping_t *values;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to FA tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(fa_node_roots, alloc_size, "bcmltmPthruLookupFaNodeRoots");
    SHR_NULL_CHECK(fa_node_roots, SHR_E_MEMORY);
    sal_memset(fa_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_value_fields_get(unit, sid, &values));

    /*-----------------
     * Tree_0
     */

    /* [Key_A2P]: Copy API keys to PT */
    /*
     * Since it is unknown whether a field is a Hash Key or not,
     * the node cookie used for KEYS lookup need to contain
     * all the PT fields which includes PT Hash Key and Data fields.
     * The list of all PT fields is the value field mapping.
     */
    keys = values;
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_api_value_fields_to_wb,
                                  (void *)keys,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &key_a2p_node));
    fa_node_roots[0] = key_a2p_node;

    /*-----------------
     * Tree_1
     */

    /* [Value_P2A]: PT fields to API values */
    SHR_IF_ERR_EXIT
        (bcmltm_md_fa_node_create(unit,
                                  bcmltm_fa_node_wb_to_api_fields,
                                  (void *)values,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &value_p2a_node));
    fa_node_roots[1] = value_p2a_node;

    /* Set returning pointer */
    op_md_ptr->fa_node_roots = fa_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, fa_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->fa_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create EE tree roots for a LOOKUP operation.
 *
 * This function creates the EE tree roots for a LOOKUP operation
 * for given PT Pass Thru table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_ee_node_roots_lookup_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_lt_op_md_t *op_md_ptr)
{
    unsigned int alloc_size;
    bcmltm_node_t **ee_node_roots = NULL;
    bcmltm_node_t *lookup_node = NULL;
    const bcmltm_pt_list_t *pt_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate pointers to EE tree roots */
    alloc_size = sizeof(bcmltm_node_t *) * op_md_ptr->num_roots;
    if (alloc_size == 0) {
        /* No roots */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_ALLOC(ee_node_roots, alloc_size, "bcmltmPthruLookupEeNodeRoots");
    SHR_NULL_CHECK(ee_node_roots, SHR_E_MEMORY);
    sal_memset(ee_node_roots, 0, alloc_size);

    /* Gather all metadata node cookies */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_pt_list_get(unit, sid, &pt_list));

    /*-----------------
     * Tree_0
     */

    /* [Lookup]: Execute PTM lookup operation */
    SHR_IF_ERR_EXIT
        (bcmltm_md_ee_node_create(unit,
                                  bcmltm_ee_node_pt_hash_lookup,
                                  (void *)pt_list,
                                  BCMLTM_NODE_SIDE_ROOT,
                                  NULL,
                                  op_md_ptr,
                                  &lookup_node));
    ee_node_roots[0] = lookup_node;

    /*-----------------
     * Tree_1
     */

    /* [NULL]: No actions */
    ee_node_roots[1] = NULL;  /* Not used */

    /* Set returning pointer */
    op_md_ptr->ee_node_roots = ee_node_roots;

 exit:
    if (SHR_FUNC_ERR()) {
        /* Destroy trees */
        bcmltm_md_node_roots_destroy(op_md_ptr->num_roots, ee_node_roots,
                                     &(op_md_ptr->num_nodes), TRUE);
        op_md_ptr->ee_node_roots = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create LOOKUP operation metadata.
 *
 * This routine creates the operation metadata for the LOOKUP opcode
 * for the given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_lookup_create(int unit,
                          bcmdrd_sid_t sid,
                          const bcmltm_wb_handle_t *wb_handle,
                          bcmltm_lt_op_md_t **op_md_ptr)
{
    bcmltm_lt_op_md_t *op_md = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate */
    SHR_ALLOC(op_md, sizeof(*op_md), "bcmltmPthruLookupLtOpMd");
    SHR_NULL_CHECK(op_md, SHR_E_MEMORY);
    sal_memset(op_md, 0, sizeof(*op_md));

    /* Fill data */
    op_md->num_roots = MD_PTHRU_OP_LOOKUP_NUM_ROOTS;
    op_md->working_buffer_size =
        BCMLTM_WORDS2BYTES(bcmltm_wb_wsize_get(wb_handle));

    /* Create FA node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_fa_node_roots_lookup_create(unit, sid, op_md));

    /* Create EE node roots */
    SHR_IF_ERR_EXIT
        (md_pthru_op_ee_node_roots_lookup_create(unit, sid, op_md));

    *op_md_ptr = op_md;

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(op_md);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create operation metadata for the given opcode.
 *
 * This routine creates the operation metadata for given table ID
 * and PT opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] wb_handle Working buffer handle.
 * \param [in] opcode Opcode.
 * \param [out] op_md_ptr Returning pointer to operation metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_create(int unit,
                   bcmdrd_sid_t sid,
                   const bcmltm_wb_handle_t *wb_handle,
                   bcmlt_pt_opcode_t opcode,
                   bcmltm_lt_op_md_t **op_md_ptr)
{
    SHR_FUNC_ENTER(unit);

    *op_md_ptr = NULL;

    switch (opcode) {
    case BCMLT_PT_OPCODE_NOP:
        SHR_EXIT();
        break;
    case BCMLT_PT_OPCODE_SET:  /* Set the entry contents */
        SHR_IF_ERR_EXIT
            (md_pthru_op_set_create(unit, sid, wb_handle, op_md_ptr));
        break;
    case BCMLT_PT_OPCODE_MODIFY:  /* Modify the specified fields of an entry */
        SHR_IF_ERR_EXIT
            (md_pthru_op_modify_create(unit, sid, wb_handle, op_md_ptr));
        break;
    case BCMLT_PT_OPCODE_GET:  /* Retrieve the contents of an entry */
        SHR_IF_ERR_EXIT
            (md_pthru_op_get_create(unit, sid, wb_handle, op_md_ptr));
        break;
    case BCMLT_PT_OPCODE_LOOKUP:  /* Search for an entry via key */
        SHR_IF_ERR_EXIT
            (md_pthru_op_lookup_create(unit, sid, wb_handle, op_md_ptr));
        break;
    case BCMLT_PT_OPCODE_CLEAR:  /* Restore entry to default values */
    case BCMLT_PT_OPCODE_FIFO_POP:  /* Other PT operations */
    case BCMLT_PT_OPCODE_FIFO_PUSH:
    case BCMLT_PT_OPCODE_INSERT:
    case BCMLT_PT_OPCODE_DELETE:
        /* TBD */
        SHR_EXIT();
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
        break;
    }

    /* Create array */
    if (*op_md_ptr != NULL) {
        SHR_IF_ERR_EXIT
            (bcmltm_md_op_trees_to_array(unit, *op_md_ptr));
    }

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_op_destroy(*op_md_ptr);
        *op_md_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Create table opcode information.
 *
 * This routine creates the opcode information for the
 * given table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info Table opcode information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_table_info_create(int unit,
                              bcmdrd_sid_t sid,
                              md_pthru_op_table_info_t *info)
{
    bcmlt_pt_opcode_t opcode;
    bcmltm_lt_op_md_t *op_md = NULL;
    const bcmltm_wb_handle_t *wb_handle = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(info, 0, sizeof(*info));

    info->sid = sid;

    /* Get working buffer handle */
    SHR_IF_ERR_EXIT
        (bcmltm_db_pthru_wb_handle_get(unit, sid, &wb_handle));
    if (wb_handle == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Create metadata for PT opcodes */
    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        SHR_IF_ERR_EXIT
            (md_pthru_op_create(unit, sid, wb_handle, opcode, &op_md));
        info->op[opcode] = op_md;
    }

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy table opcode information.
 *
 * This routine destroys the given table opcode information.
 *
 * \param [in] info Table opcode information to destroy.
 */
static void
md_pthru_op_table_info_destroy(md_pthru_op_table_info_t *info)
{
    bcmlt_pt_opcode_t opcode;

    if (info == NULL) {
        return;
    }

    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        bcmltm_md_op_destroy(info->op[opcode]);
        info->op[opcode] = NULL;
    }

    info->sid = BCMDRD_INVALID_ID;
}

/*!
 * \brief Initialize table opcode information.
 *
 * This routine initializes the given table opcode information.
 *
 * \param [in] unit Unit number.
 * \param [out] info Table opcode information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_table_info_init(int unit,
                            md_pthru_op_table_info_t *info)
{
    bcmlt_pt_opcode_t opcode;

    sal_memset(info, 0, sizeof(*info));

    info->sid = BCMDRD_INVALID_ID;

    for (opcode = 0; opcode < BCMLT_PT_OPCODE_NUM; opcode++) {
        info->op[opcode] = NULL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get table opcode information.
 *
 * This routine gets the table opcode information for the given
 * table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] info_ptr Returning pointer to table opcode information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
md_pthru_op_table_info_get(int unit,
                           bcmdrd_sid_t sid,
                           const md_pthru_op_table_info_t **info_ptr)
{
    const md_pthru_op_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *info_ptr = NULL;

    if (!md_pthru_op_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    info = &md_pthru_op[unit]->info.table;

    /* Check if ID matches current database */
    if (info->sid != sid) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *info_ptr = info;

 exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup opcode information.
 *
 * This routine destroys the given opcode information.
 *
 * \param [in] info Opcode information to destroy.
 */
static inline void
md_pthru_op_info_cleanup(md_pthru_op_info_t *info)
{
    md_pthru_op_table_info_destroy(&info->table);
}

/*!
 * \brief Initialize opcode information.
 *
 * This routine initializes the given opcode information.
 *
 * \param [in] unit Unit number.
 * \param [out] info Opcode information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static inline int
md_pthru_op_info_init(int unit,
                      md_pthru_op_info_t *info)
{
    return md_pthru_op_table_info_init(unit, &info->table);
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_md_pthru_op_init(int unit,
                        uint32_t sid_max_count)
{
    md_pthru_op_t *op = NULL;

    SHR_FUNC_ENTER(unit);

    /* Check if opcode metadata has been initialized */
    if (md_pthru_op_is_init(unit)) {
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(op, sizeof(*op), "bcmltmMdPthruOp");
    SHR_NULL_CHECK(op, SHR_E_MEMORY);
    sal_memset(op, 0, sizeof(*op));
    md_pthru_op[unit] = op;

    /* Initialize information */
    op->sid_max_count = sid_max_count;

    SHR_IF_ERR_EXIT
        (md_pthru_op_info_init(unit, &op->info));

 exit:
    if (SHR_FUNC_ERR()) {
        bcmltm_md_pthru_op_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

void
bcmltm_md_pthru_op_cleanup(int unit)
{
    md_pthru_op_t *op = NULL;

    /* Check if opcode metadata has been initialized */
    if (!md_pthru_op_is_init(unit)) {
        return;
    }

    op = md_pthru_op[unit];

    /* Cleanup internal information */
    md_pthru_op_info_cleanup(&op->info);

    SHR_FREE(op);
    md_pthru_op[unit] = NULL;

    return;
}

int
bcmltm_md_pthru_op_table_create(int unit,
                                bcmdrd_sid_t sid)
{
    md_pthru_op_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!md_pthru_op_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!md_pthru_op_sid_is_valid(unit, sid)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    info = &md_pthru_op[unit]->info.table;

    /* Check if existing cached table matches new ID */
    if (info->sid == sid) {
        SHR_EXIT();
    }

    /* Check if there is an existing cached table metadata */
    if (info->sid != BCMDRD_INVALID_ID) {
        /* No more internal space for another table */
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    /* Create table opcode metadata */
    SHR_IF_ERR_EXIT
        (md_pthru_op_table_info_create(unit, sid, info));

 exit:
    if (SHR_FUNC_ERR()) {
        (void) bcmltm_md_pthru_op_table_destroy(unit, BCMDRD_INVALID_ID);
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_op_table_destroy(int unit,
                                 bcmdrd_sid_t sid)
{
    md_pthru_op_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    if (!md_pthru_op_is_init(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    info = &md_pthru_op[unit]->info.table;

    /*
     * Check if ID matches current database.
     * BCMDRD_INVALID_ID indicates to destroy current cached data (any ID).
     */
    if ((sid != BCMDRD_INVALID_ID) && (info->sid != sid)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    md_pthru_op_table_info_destroy(info);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_md_pthru_op_get(int unit,
                       bcmdrd_sid_t sid,
                       bcmlt_pt_opcode_t opcode,
                       bcmltm_lt_op_md_t **op_md_ptr)
{
    const md_pthru_op_table_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    *op_md_ptr = NULL;

    SHR_IF_ERR_EXIT
        (md_pthru_op_table_info_get(unit, sid, &info));

    if (opcode >= BCMLT_PT_OPCODE_NUM) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (info != NULL) {
        *op_md_ptr = info->op[opcode];
    }

 exit:
    SHR_FUNC_EXIT();
}
