/*! \file bcmtm_ct_internal.h
 *
 * This file contains TM Cut Through Custom Handler
 * data structures and Macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_CT_INTERNAL_H
#define BCMTM_CT_INTERNAL_H

#include <bcmpc/bcmpc_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>

/*!
 * \brief Cut-through port information.
 */
typedef struct bcmtm_ct_port_info_s {
    /*! Source port max speed. */
    uint32_t src_port_max_speed;

    /*! Cut-through class. */
    uint8_t ct_class;

    /*! Max credit in cells. */
    uint16_t max_credit_cells;

    /*! Transmit start count for Cut-through class. */
    uint16_t xmit_start_cnt[MAX_TM_CUT_THROUGH_CLASS];

    /*! Fifo threshold cells. */
    uint16_t fifo_thd_cells;
} bcmtm_ct_port_info_t;

/*!
 * \brief TM cut through port mode set.
 */
typedef int (*bcmtm_ct_port_set_f)(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmpc_lport_t lport,
                                   uint32_t ct_enable);

/*!
 * \brief TM cut through port info get.
 */
typedef int (*bcmtm_ct_port_info_get_f)(int unit,
                                        bcmlrd_sid_t sid,
                                        uint64_t flags,
                                        bcmpc_lport_t lport,
                                        bcmtm_ct_port_info_t *ct_info);


/*!
 * \brief TM Cut Through Function pointers.
 */
typedef struct bcmtm_ct_drv_s{

    /*! TM Cut-through port set. */
    bcmtm_ct_port_set_f port_set;

    /*! TM Cut-through port info get. */
    bcmtm_ct_port_info_get_f port_info_get;
} bcmtm_ct_drv_t;

/*!
 * \brief TM_CUT_THROUGH_PORTt Logical table intenal updates.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] lport Logical port ID.
 * \param [in] action Internal updates action.
 */
extern int
bcmtm_ct_port_internal_update(int unit,
                              bcmtm_lport_t lport,
                              uint8_t action);

#endif /* BCMTM_CT_INTERNAL_H */
