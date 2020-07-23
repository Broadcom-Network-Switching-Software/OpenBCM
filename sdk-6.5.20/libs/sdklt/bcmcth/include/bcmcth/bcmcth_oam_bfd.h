/*! \file bcmcth_oam_bfd.h
 *
 * BCMCTH OAM BFD LT related functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_OAM_BFD_H
#define BCMCTH_OAM_BFD_H

#include <bcmlrd/bcmlrd_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_oam_bfd_uc.h>

/*! Structure to register callbacks for the BFD IMM backed LTs. */
typedef struct bcmcth_oam_bfd_imm_reg_s {
    /*! LT ID. */
    bcmltd_sid_t sid;

    /*!
     * Prepopulate function. Used to insert dummy entries for update_only
     * tables. Will be called prior to event registration if not NULL.
     */
    int (*prepopulate)(int unit, bcmltd_sid_t sid);

    /*! IMM event callback handler. */
    bcmimm_lt_cb_t *cb;
} bcmcth_oam_bfd_imm_reg_t;

/*!
 * \brief Register BFD IMM handlers.
 *
 * Register the IMM handlers required by the OAM_BFD_XXX LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_imm_register(int unit, bool warm);

/*!
 * \brief Register IMM handlers for global BFD LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_control_imm_register(int unit, bool warm);

/*!
 * \brief Register IMM handlers for BFD authentication LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_auth_imm_register(int unit, bool warm);

/*!
 * \brief Register IMM handlers for BFD event LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_event_imm_register(int unit, bool warm);

/*!
 * \brief Register IMM handlers for BFD endpoint LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_endpoint_imm_register(int unit, bool warm);

/*!
 * \brief Register callbacks for IMM events.
 *
 * Register callbacks for IMM events for LTs specified in \c imm_reg.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 * \param [in] num_sids Number of IMM backed LTs to be registered.
 * \param [in] imm_reg Information about the LTs that need to be registed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_imm_cb_register(int unit, bool warm, int num_sids,
                               bcmcth_oam_bfd_imm_reg_t *imm_reg);

/*!
 * \brief Get the number of fields in \c sid.
 *
 * Get the number of fields in \c sid to allocate memory for holding an entry.
 * For array fields, each array index is considered as a separate field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid LT ID for which the number of fields need to be retreived.
 * \param [out] num_fields Number of fields in \c sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_num_data_fields_get(int unit,
                                   bcmltd_sid_t sid,
                                   size_t *num_fields);

/*!
 * \brief Free the memory allocated to hold LT fields.
 *
 * \param [in] unit Unit number.
 * \param [in] num_fields Number of LT fields.
 * \param [in] fields Array of fields that need to be freed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_fields_free(int unit,
                           size_t num_fields,
                           bcmltd_fields_t *fields);

/*!
 * \brief Allocate memory to hold \c num_fields number of LT fields.
 *
 * \param [in] unit Unit number.
 * \param [in] num_fields Number of LT fields.
 * \param [out] fields Array of fields with memory allocated to each of them.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_fields_alloc(int unit, size_t num_fields,
                            bcmltd_fields_t *fields);

/*!
 * \brief Get the OAM_BFD_CONTROL entry.
 *
 * Get the OAM_BFD_CONTROL entry. Default values are filled for fields which
 * are not configured by user.
 *
 * \param [in] unit Unit number.
 * \param [out] entry OAM_BFD_CONTROL entry.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_NOT_FOUND Entry not yet created.
 */
extern int
bcmcth_oam_bfd_control_entry_get(int unit, bcmcth_oam_bfd_control_t *entry);

/*!
 * \brief Populate the default values of all the fields in OAM_BFD_CONTROL.
 *
 * \param [in] unit Unit number.
 * \param [out] entry OAM_BFD_CONTROL entry.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmcth_oam_bfd_control_defaults_get(int unit, bcmcth_oam_bfd_control_t *entry);

/*!
 * \brief Get an OAM_BFD_AUTH_SIMPLE_PASSWORD entry.
 *
 * Get the OAM_BFD_AUTH_SIMPLE_PASSWORD entry indexed by \c id. Default values
 * are filled for fields which are not configured by user.
 *
 * \param [in] unit Unit number.
 * \param [in] id Key of the OAM_BFD_AUTH_SIMPLE_PASSWORD table.
 * \param [out] entry OAM_BFD_AUTH_SIMPLE_PASSWORD entry.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_NOT_FOUND Entry is not created.
 */
extern int
bcmcth_oam_bfd_auth_sp_entry_get(int unit, uint32_t id,
                                 bcmcth_oam_bfd_auth_sp_t *entry);

/*!
 * \brief Get an OAM_BFD_AUTH_SHA1 entry.
 *
 * Get the OAM_BFD_AUTH_SHA1 entry indexed by \c id. Default values are filled
 * for fields which are not configured by user.
 *
 * \param [in] unit Unit number.
 * \param [in] id Key of the OAM_BFD_AUTH_SHA1 table.
 * \param [out] entry OAM_BFD_AUTH_SHA1 entry.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval SHR_E_NOT_FOUND Entry is not created.
 */
extern int
bcmcth_oam_bfd_auth_sha1_entry_get(int unit, uint32_t id,
                                   bcmcth_oam_bfd_auth_sha1_t *entry);

/*!
 * \brief Initialize BFD.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_init(int unit, bool warm);

/*!
 * \brief De-initialize BFD.
 *
 * \param [in] unit Unit number.
 *
 * \returns None
 */
extern void
bcmcth_oam_bfd_deinit(int unit);

/*!
 * \brief Initialize BFD event reporting.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_oam_bfd_event_report_init(int unit, bool warm);

/*!
 * \brief De-initialize BFD event reporting.
 *
 * \param [in] unit Unit number.
 *
 * \returns None
 */
extern void
bcmcth_oam_bfd_event_report_deinit(int unit);

#endif /* BCMCTH_OAM_BFD_H */
