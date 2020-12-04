/*! \file bcmcth_meter_fp_sw_state.h
 *
 * FP meter software state.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_FP_SW_STATE_H
#define BCMCTH_METER_FP_SW_STATE_H

#include <sal/sal_types.h>
#include <bcmcth/generated/meter_ha.h>

/*!
 * \brief FP Meter meter table structure.
 *
 * This structure holds the software and hardware fields of
 * all configured FP meters along with bitmap of meter ids that
 * are used.
 */
typedef struct bcmcth_meter_fp_sw_state_s {
    /*! Structure to mark in-use status of hardware meter ids. */
    bool                        *hw_idx;

    /*! List of meter entries configured. */
    bcmcth_meter_fp_entry_t     *list;

    /*! Backup structure to mark in-use status of hardware meter ids. */
    bool                        *bkp_hw_idx;

    /*! Backup list. */
    bcmcth_meter_fp_entry_t     *bkp_list;
} bcmcth_meter_fp_sw_state_t;

#endif /* BCMCTH_METER_FP_SW_STATE_H */
