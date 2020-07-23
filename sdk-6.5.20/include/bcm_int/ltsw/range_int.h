/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*! \file range_int.h
 *
 * Range internal management header file.
 * This file contains the management for RANGE module.
 */

#ifndef BCMINT_LTSW_RANGE_INT_H
#define BCMINT_LTSW_RANGE_INT_H

#include <bcm/range.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <sal/sal_mutex.h>

#include <shr/shr_bitop.h>

#define _RANGE_ID_MAX        (0x000000FF)
#define _RANGE_GRP_ID_MAX    (0x0000000F)

/*
 * Typedef:
 *   bcmint_range_id_bmp_t
 * Purpose:
 *   Entry bit map for tracking allocation of range ids.
 */
typedef struct bcmint_range_id_bmp_s{
    SHR_BITDCL  w[_SHR_BITDCLSIZE(_RANGE_ID_MAX)];
} bcmint_range_id_bmp_t;

/*
 * Typedef:
 *   bcmint_range_id_bmp_t
 * Purpose:
 *   Entry bit map for tracking allocation of range ids.
 */
typedef struct bcmint_range_grp_id_bmp_s{
    SHR_BITDCL  w[_SHR_BITDCLSIZE(_RANGE_GRP_ID_MAX)];
} bcmint_range_grp_id_bmp_t;

/* Different RANGE LT and key fields. */
typedef struct bcmint_range_tbls_info_s {
    /*! Range SID */
    char *range_sid;

    /*! Range Key FID */
    char *range_key_fid;
    
    /*! Range Max Value */
    char *range_max;

    /*! Range Min Value */
    char *range_min;

    /*! Range type */
    char *range_type;

    /*! Range Group SID */
    char *range_grp_sid;

    /*! Range Group Key FID */
    char *range_grp_key_fid;

    /*! Range Group Key Type FID */
    char *range_grp_key_type_fid;

    /*! Range Operational Mode SID */
    char *oper_mode_sid;

    /*! Range Operational Mode FID */
    char *oper_mode_fid;
} bcmint_range_tbls_info_t;

/*!
 * \brief Range Module Information
 */
typedef struct bcmint_range_info_s {
    /*! Bitmap of Range IDs */
    bcmint_range_id_bmp_t range_id_bmp;

    /*! Bitmap of Range Group IDs */
    bcmint_range_grp_id_bmp_t range_grp_id_bmp;

    /*! LT table and key field names */
    bcmint_range_tbls_info_t *tbls_info;

} bcmint_range_info_t;

/*!
 * \brief Range Module control Information
 */
typedef struct bcmint_range_control_info_s {
    bcmint_range_info_t range_info;
} bcmint_range_control_info_t;

/*!
 * \brief Range Module database.
 */
typedef struct bcmi_ltsw_range_db_s {

    /*! Range module lock. */
    sal_mutex_t lock;

    /*! Range control database. */
    bcmint_range_control_info_t control_db;

} bcmi_ltsw_range_db_t;

extern int bcm_ltsw_range_init(int unit);
extern int bcm_ltsw_range_detach(int unit);
#endif /* BCMINT_LTSW_RANGE_INT_H */
