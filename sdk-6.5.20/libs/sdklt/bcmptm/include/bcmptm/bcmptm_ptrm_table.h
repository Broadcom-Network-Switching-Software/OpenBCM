/*! \file bcmptm_ptrm_table.h
 *
 * Type definitions for generated PTRM data.
 *
 * Note that there is a coupling between the structure definitions in
 * this file and the PTRM data files. Any changes here need to be
 * reflected in any PTRM data files using these structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_PTRM_TABLE_H
#define BCMPTM_PTRM_TABLE_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmlrd_ptrm.h>

/*******************************************************************************
 * Typedefs
 */

/*!
 * \brief PTRM Table handler.
 *
 * Binds a logical table symbol identifier (sid) with a PTRM handler
 * data structure.
 */
typedef struct bcmptm_ptrm_table_s {
    /*! Logical table symbol identifier. */
    bcmltd_sid_t sid;

    /*! PTRM handler data structure. */
    const lt_mreq_info_t *handler;
} bcmptm_ptrm_table_t;

/*!
 * \brief PTRM device handler type.
 *
 * Array and count of PTRM Table handler for a given device or device
 * variant.
 */
typedef struct bcmptm_ptrm_table_data_s {
    /*! Array of PTRM table handler pointers. */
    const bcmptm_ptrm_table_t *table_data;

    /*! Number of PTRM table handlers. */
    size_t                     table_data_count;
} bcmptm_ptrm_table_data_t;

/*!
 * \brief  PTRM device handler data structures.
 *
 * External declarations for all PTRM device handler data structures.
 */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1)   \
    extern const bcmptm_ptrm_table_data_t _bd##_vu##_va##_ptrm_table_data;
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>

#endif /* BCMPTM_PTRM_TABLE_H */
