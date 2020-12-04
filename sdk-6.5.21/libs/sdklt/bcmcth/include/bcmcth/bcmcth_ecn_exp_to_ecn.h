/*! \file bcmcth_ecn_exp_to_ecn.h
 *
 * BCMCTH function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_ECN_EXP_TO_ECN_H
#define BCMCTH_ECN_EXP_TO_ECN_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <shr/shr_bitop.h>

/*! Max field number for EXP_TO_ECN logical table. */
#define BCMCTH_ECN_EXP_TO_ECN_MAP_FIELD_ID_MAX 6

/*!
 * \brief DLB ECMP LT fields information.
 */
typedef enum bcmcth_ecn_exp_to_ecn_map_lt_fields_e {
    BCMCTH_ECN_MPLS_EXP_TO_IP_ECN_ID = 0,
    BCMCTH_ECN_LAST_DERIVED_ECN = 1,
    BCMCTH_ECN_EXP = 2,
    BCMCTH_ECN_NEW = 3,
    BCMCTH_ECN_DROP = 4,
} bcmcth_ecn_exp_to_ecn_map_lt_fields_t;


/*!
 * BCMCTH load balance hashing information.
 */
typedef struct bcmcth_ecn_exp_to_ecn_map_s {

    /*! LT symbol ID. */
    bcmltd_sid_t glt_sid;

    /*! LT operation transaction ID. */
    uint32_t trans_id;

    /*! Bitmap of fields to be operated */
    SHR_BITDCLNAME(fbmp, BCMCTH_ECN_EXP_TO_ECN_MAP_FIELD_ID_MAX);

    /*! LT entry logical table ID key field value. */
    uint8_t map_id;

    /*! LT entry ecn key field value. */
    uint8_t ecn;

    /*!  LT entry exp key field value. */
    uint8_t exp;

    /*! New ecn after mapping. */
    uint8_t new_ecn;

    /*! Drop packet. */
    bool    drop;
} bcmcth_ecn_exp_to_ecn_map_t;

/*!
 * \brief ECN_MPLS_EXP_TO_IP_ECN  set handler.
 *
 * The method inserts the configuration for a particular EXP_TO_ECN map entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  lt_entry      LT entry data buffer.
 *
 * \retval SHR_E_NONE         No errors.
 */
extern int
bcmcth_ecn_exp_to_ecn_map_set(int unit,
                              bcmcth_ecn_exp_to_ecn_map_t *lt_entry);

#endif /* BCMCTH_ECN_EXP_TO_ECN_H */
