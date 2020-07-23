/*! \file bcmltm_md_fs.h
 *
 * Logical Table Manager - Field Selection.
 *
 * This file contains interfaces to construct the field select
 * subtrees.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_FS_H
#define BCMLTM_MD_FS_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_md_internal.h>

#include "bcmltm_md_lta.h"

/*!
 * \brief Field Select Cluster Types.
 *
 * This information is used to indicate the type of field select
 * subtrees cluster to create.
 */
typedef enum bcmltm_fs_cluster_type_e {
    /*! Field select value write. */
    BCMLTM_FS_CLUSTER_TYPE_VALUE_WRITE,

    /*! Field select value read. */
    BCMLTM_FS_CLUSTER_TYPE_VALUE_READ,

    /*! Key field selector map. */
    BCMLTM_FS_CLUSTER_TYPE_KEY_FSM_MAP,
} bcmltm_fs_cluster_type_t;

/*!
 * \brief Field select Flags.
 *
 * These flags are used to indicate special handling within a field select
 * subtree:
 *     BCMLTM_MD_FS_FLAG_*
 *
 * This information is used by the field select subtree creation interfaces
 * to construct and install the appropriate function nodes in the
 * field select subtree.
 */
typedef uint32_t bcmltm_md_fs_flags_t;

/*
 * \brief Defaults flag.
 */
#define BCMLTM_MD_FS_FLAG_DEFAULTS                0

/*
 * \brief API Cache input update flag.
 *
 * Indicates to use the update (unset) FA function version for copying
 * the LTM entry input fields into API cache.  This version does not
 * apply defaults if field is not provided.
 *
 * Usage: Value write cluster on UPDATE.
 */
#define BCMLTM_MD_FS_FLAG_APIC_IN_UPDATE           0x0001

/*
 * \brief API Cache input skip flag.
 *
 * Indicates to skip the copy of the LTM entry input fields into the
 * API cache.
 *
 * Usage: Value write cluster on DELETE.
 */
#define BCMLTM_MD_FS_FLAG_APIC_IN_SKIP             0x0002

/*
 * \brief Apply fixed field delete value flag.
 *
 * Indicates to apply the delete value for fixed fields.
 *
 * Usage: Value write cluster on DELETE.
 */
#define BCMLTM_MD_FS_FLAG_FIXED_DEL                0x0004

/*
 * \brief Skip API output flag.
 *
 * Indicates to skip the copy of API cache fields to LTM entry
 * output fields.
 *
 * Usage: Value read cluster on UPDATE.
 */
#define BCMLTM_MD_FS_FLAG_API_OUT_SKIP             0x0008

/*!
 * \brief Field select cluster flags.
 *
 * These flags are used to indicate special handling within
 * the field select cluster subtrees.
 */
typedef struct bcmltm_md_fs_cluster_flags_s {
    /*! Flags for field select subtrees. */
    bcmltm_md_fs_flags_t fs_flags;

    /*! Flags for LTA subtrees (transform/validations). */
    bcmltm_md_lta_flags_t lta_flags;
} bcmltm_md_fs_cluster_flags_t;


/*!
 * \brief Create the key field select map cluster of FA subtrees.
 *
 * This routine creates the cluster of key field select map FA subtrees
 * for the specified field select cluster type for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs_type Field select cluster type.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_chain Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fs_key_fsm_fa_cluster_create(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_fs_cluster_type_t fs_type,
                                       bcmltm_node_side_t node_side,
                                       bcmltm_node_t *parent_chain,
                                       bcmltm_lt_op_md_t *op_md_ptr,
                                       bcmltm_node_t **next_chain);
/*!
 * \brief Create the key fsm map cluster, selector value to select map index.
 *
 * This routine creates the cluster of key field select map FA subtrees
 * to convert the field selector values to the corresponding
 * field select map indexes for a given table.
 *
 * This interface is a helper routine that sets the right arguments
 * for the core cluster create function.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_node Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fs_key_fsm_map_cluster_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmltm_node_side_t node_side,
                                        bcmltm_node_t *parent_node,
                                        bcmltm_lt_op_md_t *op_md_ptr,
                                        bcmltm_node_t **next_chain);

/*!
 * \brief Create the field select value cluster of FA subtrees.
 *
 * This routine creates the cluster of field select FA subtrees
 * for the specified field select value cluster type for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fs_type Field select cluster type.
 * \param [in] fsc_flags Field select cluster flags.
 * \param [in] opcode LT opcode.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_chain Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fs_value_fa_cluster_create(int unit,
                               bcmlrd_sid_t sid,
                               bcmltm_fs_cluster_type_t fs_type,
                               const bcmltm_md_fs_cluster_flags_t *fsc_flags,
                               bcmlt_opcode_t opcode,
                               bcmltm_node_side_t node_side,
                               bcmltm_node_t *parent_chain,
                               bcmltm_lt_op_md_t *op_md_ptr,
                               bcmltm_node_t **next_chain);

/*!
 * \brief Create the value write field select cluster.
 *
 * This routine creates the cluster of FA subtrees for
 * value write field selects for a given table.
 *
 * This interface is a helper routine that sets the right arguments
 * for the core cluster create function.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] opcode LT opcode.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_node Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fs_value_write_cluster_create(int unit,
                                        bcmlrd_sid_t sid,
                                        bcmlt_opcode_t opcode,
                                        bcmltm_node_side_t node_side,
                                        bcmltm_node_t *parent_node,
                                        bcmltm_lt_op_md_t *op_md_ptr,
                                        bcmltm_node_t **next_chain);

/*!
 * \brief Create the value read field select cluster.
 *
 * This routine creates the cluster of FA subtrees for
 * value read field selects for a given table.
 *
 * This interface is a helper routine that sets the right arguments
 * for the core cluster create function.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] opcode LT opcode.
 * \param [in] node_side Side of parent node to attach cluster to.
 * \param [in] parent_node Parent node to attach cluster to.
 * \param [in,out] op_md_ptr Pointer to operation metadata.
 * \param [out] next_chain Node that can be used to chain next node to tree.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_fs_value_read_cluster_create(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmlt_opcode_t opcode,
                                       bcmltm_node_side_t node_side,
                                       bcmltm_node_t *parent_node,
                                       bcmltm_lt_op_md_t *op_md_ptr,
                                       bcmltm_node_t **next_chain);

#endif /* BCMLTM_MD_FS_H */
