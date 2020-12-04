/*! \file bcmcth_l3_prot.h
 *
 * L3_PROTECTION_ENABLE  Custom Handler header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L3_PROT_H
#define BCMCTH_L3_PROT_H

/*******************************************************************************
  Includes
 */
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_bitop.h>

/*!
 * MPLS protection switching lt structure
 */
typedef struct bcmcth_l3_prot_en_lt_s {
    /*! Nexthop id. */
    uint32_t    nhop_id;
    /*! Enable/Disable protection. */
    bool        protection;
    /*! Validity of protect field. */
    bool        protection_valid;
} bcmcth_l3_prot_en_lt_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_l3_prot_enable_set(int unit, uint32_t trans_id,         \
                                          bcmltd_sid_t lt_id, uint32_t nhop_id,\
                                          bool prot);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Enable/Disable l3 nexthop protection, applicable for XFS device.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      Transaction id.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  nhop_id       Nexthop id.
 * \param [in]  prot          Enable/Disable protection.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_l3_prot_enable_set(int unit, uint32_t trans_id, bcmltd_sid_t lt_id,
                          uint32_t nhop_id, bool prot);

#endif /* BCMCTH_L3_PROT_H */
