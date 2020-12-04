/*! \file bcmcth_tnl_mpls_exp_remark_ctrl.h
 *
 * BCMCTH TNL function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_MPLS_EXP_REMARK_CTRL_H
#define BCMCTH_TNL_MPLS_EXP_REMARK_CTRL_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <shr/shr_bitop.h>

/*! Max field number for MPLS EXP remark logical table. */
#define BCMCTH_TNL_MPLS_EXP_REMARK_CTRL_FIELD_ID_MAX 6

/*!
 * \brief MPLS_EXP_PHB_CONTROL LT fields information.
 */
typedef enum bcmcth_tnl_mpls_exp_remark_ctrl_lt_fields_e {
    BCMCTH_TNL_MPLS_EXP_REMARK_CONTROL_ID_1 = 0,
    BCMCTH_TNL_MPLS_EXP_REMARK_CONTROL_ID_2 = 1,
    BCMCTH_TNL_MPLS_EXP_REMARK_CONTROL_ID_3 = 2,
    BCMCTH_TNL_MPLS_EFFECTIVE_EXP_REMARK_PROFILE = 3,
    BCMCTH_TNL_MPLS_EXP_REMARK_SEL = 4,
} bcmcth_tnl_mpls_exp_remark_ctrl_lt_fields_t;


/*!
 * BCMCTH MPLS exp remark control information.
 */
typedef struct bcmcth_tnl_mpls_exp_remark_ctrl_s {

    /*! LT symbol ID. */
    bcmltd_sid_t glt_sid;

    /*! LT operation argument. */
    const bcmltd_op_arg_t *op_arg;

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_CTRL_FIELD_ID_MAX);

    /*! LT entry logical table ID key field value. */
    uint8_t map_id[BCMCTH_TNL_MPLS_MAX_LOOKUP];

    /*! LT entry logical table ID key field value. */
    bool    effective;

    /*! PHB map selection. */
    uint8_t remark_sel;
} bcmcth_tnl_mpls_exp_remark_ctrl_t;

/*!
 * \brief MPLS_EXP_REMARK_CONTROL  set handler.
 *
 * The method inserts the configuration for a particular MPLS_EXP_REMARK_CONTROL map entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  lt_entry      LT entry data buffer.
 *
 * \retval SHR_E_NONE         No errors.
 */
extern int
bcmcth_tnl_mpls_exp_remark_ctrl_set(int unit,
                                    bcmcth_tnl_mpls_exp_remark_ctrl_t *lt_entry);

#endif /* BCMCTH_TNL_MPLS_EXP_REMARK_CTRL_H */
