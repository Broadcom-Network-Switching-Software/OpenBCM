/*! \file l3_aacl.h
 *
 * XGS L3 AACL module APIs and data structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_XGS_L3_AACL_H
#define BCMI_LTSW_XGS_L3_AACL_H

#include <bcm/l3.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/l3_fib_int.h>

/* L3 AACL fields. */
typedef enum bcmint_xgs_l3_aacl_fld_e {
    BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0,
    BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1,
    BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK,
    BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1_MASK,
    BCMINT_XGS_L3_AACL_TBL_MAC,
    BCMINT_XGS_L3_AACL_TBL_VRF,
    BCMINT_XGS_L3_AACL_TBL_L4_PORT,
    BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL,
    BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS,
    BCMINT_XGS_L3_AACL_TBL_CLASS_ID,
    BCMINT_XGS_L3_AACL_TBL_HIT,
    BCMINT_XGS_L3_AACL_TBL_FLD_CNT
} bcmint_xgs_l3_aacl_fld_t;

/*!
 * \brief Initialize L3 AACL feature.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_init(int unit);

/*!
 * \brief De-initialize the L3 AACL feature.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_deinit(int unit);

/*!
 * \brief Validation for L3 AACL feature.
 *
 * \param [in] unit Unit Number.
 * \param [in] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 * \param [in] strict Stict validation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_validate(
    int unit,
    bcm_l3_aacl_t *aacl,
    bool strict);

/*!
 * \brief Adds a L3 AACL to the compression table.
 *
 * \param [in] unit Unit Number.
 * \param [in] options L3 AACL options. See BCM_L3_AACL_OPTIONS_xxx.
 * \param [in] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_add(
    int unit,
    uint32_t options,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Deletes a L3 AACL from the compression table.
 *
 * \param [in] unit Unit Number.
 * \param [in] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_delete(
    int unit,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Deletes all AACLs.
 *
 * \param [in] unit Unit Number.
 * \param [in] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_delete_all(
    int unit,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Finds information from the AACL table.
 *
 * \param [in] unit Unit Number.
 * \param [inout] aacl Pointer to bcm_l3_aacl_t struct which contains fields related to compression table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_find(
    int unit,
    bcm_l3_aacl_t *aacl);

/*!
 * \brief Traverse through the AACL table and run callback at each entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] trav_fn Traverse call back function.
 * \param [in] user_data User data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_traverse(
    int unit,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data);

/*!
 * \brief Traverse through the AACL table and run callback at each entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags L3 AACL flags. See BCM_L3_AACL_FLAGS_xxx.
 * \param [in] trav_fn Traverse call back function.
 * \param [in] user_data User data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_l3_aacl_matched_traverse(
    int unit,
    uint32_t flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data);

#endif /* BCMI_LTSW_XGS_L3_AACL_H */
