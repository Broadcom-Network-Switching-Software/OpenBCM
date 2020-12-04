/** \file bcm_int/dnx/oam/oam_punt_and_protection.h
 *
 * Punt and Protection definitions for use by other modules
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef OAM_PUNT_AND_PROTECTION_H_INCLUDED__
/* { */
#define OAM_PUNT_AND_PROTECTION_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */

/* } */

/**
 * \brief
 *  Add a mapping from a trap ID to an OAMP protection or punt trap
 *   \param [in] unit     -  Unit ID
 *   \param [in] type     -  punt or protection trap type to map
 *   \param [in] trap_id_p  -  trap ID
 *
 * \return
 *   shr_error - Error indication
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_error_trap_type_set(
    int unit,
    bcm_rx_trap_t type,
    int trap_id_p);

/**
 * \brief
 *  This function checks if the configured trap is
 *  an OAM punt or protection trap, and if it is,
 *  makes sure the WITH_ID flag is set
 * \param [in] unit  - Device ID
 * \param [in] flags - flags used to configure this
 *        trap
 * \param [in] type - trap type
 * \param [in] trap_id - user provided trap ID
 * \param [out] is_oam_punt_or_protection - pointer
 *        to byte that will be set to TRUE if the
 *        trap type is punt or protection, and
 *        FALSE otherwise
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_punt_and_protection_trap_create_verify(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int trap_id,
    uint8 *is_oam_punt_or_protection);

/**
 * \brief
 *  This function sets the destination for a punt
 *  trap or the protection packet header
 * \param [in] unit  - Device ID
 * \param [in] trap_type - key to OAM error trap
 *        SW state table
 * \param [in] config_p - pointer to config params
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_error_trap_set(
    int unit,
    int trap_type,
    bcm_rx_trap_config_t * config_p);

/**
 * \brief
 *  This function reads the destination for a punt
 *  trap or the protection packet header
 * \param [in] unit  - Device ID
 * \param [in] trap_type - key to OAM error trap
 *        SW state table
 * \param [out] config_p - pointer to config params
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_error_trap_get(
    int unit,
    int trap_type,
    bcm_rx_trap_config_t * config_p);

/**
 * \brief
 *  Destroy previously created OAM error trap, i.e. delete the entry from the table
 * \param [in] unit        -  Unit id
 * \param [in] trap_type   -  OAM error trap type to destroy
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_error_trap_destroy(
    int unit,
    int trap_type);

/**
 * \brief
 *  Get trap code used for particular punt trap type
 * \param [in] unit        -  Unit id
 * \param [in] type        -  trap type used to create trap
 * \param [out] trap_id_p  -  pointer to resulting trap ID value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_get_punt_protection_trap_from_type(
    int unit,
    bcm_rx_trap_t type,
    int *trap_id_p);

/* } */
#endif /* OAM_H_INCLUDED__ */
