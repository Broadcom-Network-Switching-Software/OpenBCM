/*! \file bcmcth_mon_flowtracker_util.h
 *
 * Flowtracker utility functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_UTIL_H
#define BCMCTH_MON_FLOWTRACKER_UTIL_H

/*!
 * \brief Free LT fields handle
 *
 * \param [in]  unit BCM device number
 * \param [in]  num_fields Number of fields
 * \param [in]  fields Fields to be freed
 *
 * \return Free the fields handle.
 */
extern int
bcmcth_mon_ft_util_fields_free(int unit, int num_fields, bcmltd_fields_t *fields);

/*!
 * \brief Alloc LT fields handle
 *
 * \param [in]  unit BCM device number
 * \param [in]  num_fields Number of fields
 * \param [out]  fields Fields alloc handle
 *
 * \return Alloc for specified number of fields
 */
extern int
bcmcth_mon_ft_util_fields_alloc(int unit,size_t num_fields, bcmltd_fields_t *fields);

/*!
 * \brief Check if LT is IMM mapped
 *
 * \param [in]  unit BCM device number
 * \param [in]  sid LT id
 *
 * \return Verifies if LT is IMM mapped
 */
extern bool
bcmcth_mon_ft_imm_mapped(int unit, const bcmdrd_sid_t sid);

/*!
 * \brief Utility function to retrive flowtracker export template LT data fields count.
 *
 * \param [in] unit Unit number.
 * \param [out] count Number of data fields
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_flowtracker_export_template_out_fields_count(
    int unit,
    uint32_t *count);

/*!
 * \brief Utility function to retrive flowtracker group LT data fields count.
 *
 * \param [in] unit Unit number.
 * \param [out] count Number of data fields
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_flowtracker_group_out_fields_count(
    int unit,
    uint32_t *count);

/*!
 * \brief Utility function to retrive flowtracker control LT data fields count.
 *
 * \param [in] unit Unit number.
 * \param [out] count Number of data fields
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_flowtracker_control_out_fields_count(
    int unit,
    uint32_t *count);
#endif /*! BCMCTH_MON_FLOWTRACKER_PACK_H */
