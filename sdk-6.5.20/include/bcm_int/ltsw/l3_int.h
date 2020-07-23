/*! \file l3_int.h
 *
 * L3 internal header file.
 * This file contains L3 definitions internal to the L3 module
 * itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMI_LTSW_L3_INT_H
#define BCMI_LTSW_L3_INT_H

/* HA Sub component id for L3 option info. */
#define BCMINT_LTSW_L3_HA_IP_OPTION_FBMP     0
#define BCMINT_LTSW_L3_HA_IP_OPTION_HW     1

#define MAX_FIELD_CNT     3

typedef struct bcmint_ltsw_l3_switch_control_tbl_s {
    const char *ltname;
    int        field_count;
    const char *field_str[MAX_FIELD_CNT];
    bool       field_symbol;
} bcmint_ltsw_l3_switch_control_tbl_t;

/*!
 * \brief Get L3 switch control logical table info.
 *
 * \param [in] unit Unit Number.
 * \param [out] tbl_info Pointer to table description info.
 *
 * \retval SHR_E_UNAVAIL Logical table is not supported.
 * \retval SHR_E_NONE No error.
 */
extern int
bcmint_ltsw_l3_switch_control_tbl_get(
    int unit,
    const bcmint_ltsw_l3_switch_control_tbl_t **tbl_info);

#endif /* BCMI_LTSW_L3_INT_H */

