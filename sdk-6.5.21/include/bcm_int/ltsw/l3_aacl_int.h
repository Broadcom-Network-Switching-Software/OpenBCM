/*! \file l3_aacl_int.h
 *
 * L3 AACL internal header file.
 * This file contains L3 AACL definitions internal to the L3 module itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_L3_AACL_INT_H
#define BCMI_LTSW_L3_AACL_INT_H

#include <bcm_int/ltsw/types.h>
#include <sal/sal_types.h>

/* Field description. */
typedef struct bcmint_ltsw_l3_aacl_fld_s {
    /* Field name. */
    const char *fld_name;

    /* Filed ID. */
    uint32_t fld_id;

    /* Key field. */
    bool key;

    /* Symbol field. */
    bool symbol;

    /* Number of elements needed for field. */
    uint32_t elements;

    /* Callback to get scalar value from symbol. */
    bcmi_ltsw_symbol_to_scalar_f symbol_to_scalar;

    /* Callback to get symbol from scalar value. */
    bcmi_ltsw_scalar_to_symbol_f scalar_to_symbol;

} bcmint_ltsw_l3_aacl_fld_t;

/* L3 AACL tables. */
typedef enum bcmint_ltsw_l3_aacl_tbl_id_e {
    BCMINT_LTSW_L3_AACL_TBL_IPV4_DST,
    BCMINT_LTSW_L3_AACL_TBL_IPV6_DST,
    BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC,
    BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC,
    BCMINT_LTSW_L3_AACL_TBL_CNT
} bcmint_ltsw_l3_aacl_tbl_id_t;

/* The structure of L3 forwarding table. */
typedef struct bcmint_ltsw_l3_aacl_tbl_s {
    /* Table name. */
    const char *name;

    /* Table ID. */
    uint32_t tbl_id;

    /* Field count. */
    uint32_t fld_cnt;

    /* Bitmap of valid fields. */
    uint64_t fld_bmp;

    /* The fields of table. */
    const bcmint_ltsw_l3_aacl_fld_t *flds;

} bcmint_ltsw_l3_aacl_tbl_t;

/* L3 forwarding table database. */
typedef struct bcmint_ltsw_l3_aacl_tbl_db_s {
    /* The table bitmap. */
    uint32_t tbl_bmp;

    /* The table array. */
    const bcmint_ltsw_l3_aacl_tbl_t *tbls;
} bcmint_ltsw_l3_aacl_tbl_db_t;

/*!
 * \brief Get L3 AACL logical table info.
 *
 * \param [in] unit Unit Number.
 * \param [in] tbl_id AACL logical table description ID.
 * \param [out] tbl_info Pointer to table description info.
 *
 * \retval SHR_E_UNAVAIL Logical table is not supported.
 * \retval SHR_E_NONE No errer.
 */
extern int
bcmint_ltsw_l3_aacl_tbl_get(int unit,
                            bcmint_ltsw_l3_aacl_tbl_id_t tbl_id,
                            const bcmint_ltsw_l3_aacl_tbl_t **tbl_info);

#endif /* BCMI_LTSW_L3_AACL_INT_H */


