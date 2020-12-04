/*! \file bcmcth_trunk_vp.h
 *
 * Top-level TRUNK_VP device functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_H
#define BCMCTH_TRUNK_VP_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmcth/bcmcth_trunk_vp_types.h>
#include <bcmimm/bcmimm_int_comp.h>

/*!
 * \brief Structure for TRUNK_VP physical table operation.
 *
 * This structure is used to supply PTM API attributes used during TRUNK_VP logical
 * table entry Read/Write operations to hardware/physical tables.
 */
typedef struct bcmcth_trunk_vp_pt_op_info_s {

    /*! Device Resource Database Symbol ID. */
    bcmdrd_sid_t drd_sid;

    /*! Request Logical Table Source ID information. */
    bcmltd_sid_t req_lt_sid;

    /*! Response Logical Table Source ID information. */
    bcmltd_sid_t rsp_lt_sid;

    /*! Logical table operation transaction identifier. */
    uint32_t trans_id;

    /*! Request PTM API option flags. */
    uint32_t req_flags;

    /*! Response PTM API option flags. */
    uint32_t rsp_flags;

    /*! PTM operation type. */
    bcmptm_op_type_t op;

    /*! Physical Table Entry Size in number of words. */
    size_t wsize;

    /*! Enable DMA. */
    bool dma;

    /*! Total number of entries for DMA (SLAM/write or TDMA/read) operation. */
    uint32_t ecount;

    /*!
     * Dynamic address information of the physical table.
     * Physical Table Entry Hardware Index and Pipe Instances to Install Info.
     */
    bcmbd_pt_dyn_info_t dyn_info;

    /*!
     * Operational argument.
     */
    const bcmltd_op_arg_t *op_arg;
} bcmcth_trunk_vp_pt_op_info_t;

#endif /* BCMCTH_TRUNK_VP_H */
