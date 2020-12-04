/*! \file bcmgene_editor.h
 *
 * Broadcom Generic Extensible NPL Encapsulation (GENE) editor header file.
 * This file contains GENE editor definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_GENE_EDITOR_H
#define BCMINT_LTSW_GENE_EDITOR_H

#include <bcm_int/ltsw/chip/bcmgene_db.h>
#include <bcmlt/bcmlt.h>


/*!
 * \brief The struct of GENE handle.
 */
typedef struct bcmgene_field_data_s {

    /*! The struct of GENE field. */
    bcmgene_field_desc_t *fld;

    /*! The index of field array. */
    int index;

    /*! The value of field array. */
    uint64_t *value;

    /*! The count of field array. */
    int count;

    /*! The depth of field array. */
    int depth;

} bcmgene_field_data_t;

/*!
 * \brief The struct of GENE handle.
 */
typedef struct bcmgene_handle_s {

    /*! The unit number. */
    int unit;

    /*! The GENE targeting table. */
    bcmgene_table_desc_t *tbl;

#define BCMGENE_F_HANDLE_RESERVED   (1 << 0)
#define BCMGENE_F_HANDLE_GRACEFUL   (1 << 1)
#define BCMGENE_F_HANDLE_CALLBACK   (1 << 2)

    /*! The GENE flags. */
    uint32_t flags;

    /*! The GENE handler. */
    bcmlt_entry_handle_t handle;

    /*! The GENE batch handler. */
    bcmlt_transaction_hdl_t batch_handle;

    /*! The GENE handler priority. < BCMLT_PRIORITY_xxx */
    bcmlt_priority_level_t priority;

    /*! The GENE handler opcode. < BCMLT_OPCODE_xxx */
    bcmlt_opcode_t opcode;

    /*! The GENE handler callback function */
    int (*cb)(struct bcmgene_handle_s *hndl, void *user_data);

} bcmgene_handle_t;

#endif /* BCMINT_LTSW_GENE_EDITOR_H */
