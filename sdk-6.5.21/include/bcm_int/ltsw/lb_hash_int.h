/*! \file lb_hash_int.h
 *
 * LB hash internal header file.
 * This file contains lb hash definitions internal to the LB hash module
 * itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMI_LTSW_LB_HASH_INT_H
#define BCMI_LTSW_LB_HASH_INT_H

#include <bcm_int/ltsw/lb_hash.h>

/* LB hash output selection table database. */
typedef struct bcmint_ltsw_lb_hash_os_tbl_s {
    /* Support. */
    int valid;

    /* Flow based. */
    int flow_based;

    /* Table name. */
    const char *name;

    /* Key name. */
    const char *key_name;

    /* Offset name. */
    const char *offset_name;

    /* Sub sel name. */
    const char *sub_sel_name;

    /* Concat name. */
    const char *concat_name;
} bcmint_ltsw_lb_hash_os_tbl_t;

typedef struct bcmint_ltsw_lb_hash_os_tbl_db_s {
    /* The table array. */
    const bcmint_ltsw_lb_hash_os_tbl_t *tbls;
} bcmint_ltsw_lb_hash_os_tbl_db_t;

/*!
 * \brief Get the lb hash outut selection table database.
 *
 * \param [in] unit Unit number.
 * \param [in] type LB hash output selection type.
 * \param [out] tbl_info LB hash output selection table databse.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmint_ltsw_lb_hash_os_tbl_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    const bcmint_ltsw_lb_hash_os_tbl_t **tbl_info);

#endif /* BCMI_LTSW_LB_HASH_INT_H */

