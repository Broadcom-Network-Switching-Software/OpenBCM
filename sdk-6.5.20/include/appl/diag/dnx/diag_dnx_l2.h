/*! \file diag_dnx_l2.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_L2_H_INCLUDED
#define DIAG_DNX_L2_H_INCLUDED

/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */
#define         L2_LEARN_CALLBACK_ACTIVE        0x1
#define         L2_LEARN_CALLBACK_LOGGER        0x2
#define         L2_LEARN_COUNTER_ONLY           0x4

/*************
 *  MACROES  *
 */

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_l2_man;
extern sh_sand_cmd_t sh_dnx_l2_cmds[];

/*************
 * FUNCTIONS *
 */
/**
 * \brief
 *  This routine return entry count in MACT
 * \param [in] unit               -    Device ID
 * \param [in] l2_entry_counter_p -    Pointer to place were counter will returned on success
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 */
shr_error_e sh_dnx_l2_get_mact_count(
    int unit,
    uint32 *l2_entry_counter_p);

/**
 * \brief
 *  This routine return entry count in MACT per vsi
 * \param [in] unit               -    Device ID
 * \param [in] vsi                -    vsi id
 * \param [in] l2_entry_counter_p -    Pointer to place were counter will returned on success
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 */
shr_error_e sh_dnx_l2_get_mact_vsi_count(
    int unit,
    int vsi,
    uint32 *l2_entry_counter_p);

/**
 * \brief
 *  This routine return entry count in MACT per vsi
 * \param [in] unit               -    Device ID
 * \param [in] gport              -    encoded gport with inlif inside
 * \param [in] l2_entry_counter_p -    Pointer to place were counter will returned on success
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 */
shr_error_e sh_dnx_l2_get_mact_inlif_count(
    int unit,
    bcm_gport_t gport,
    uint32 *l2_entry_counter_p);

/**
 * \brief
 *  This routine return vsi existence status
 * \param [in] unit               -    Device ID
 * \param [in] vsi                -    vsi id
 * \return
 *   shr_error_e - _SHR_E_NONE - if vsi exists
 *                 _SHR_E_NOT_FOUND - if not
 * \remark
 *
 * \see
 */
shr_error_e sh_dnx_l2_vsi_exists(
    int unit,
    bcm_vlan_t vsi);

/**
 * \brief
 *  This routine clear all entries from MACT
 * \param [in] unit              -    Device ID
 * \param [in] svl               -    If is TRUE delete entire SVL table
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 */
shr_error_e sh_dnx_l2_clear_all(
    int unit,
    int svl);

#endif /** DIAG_DNX_L2_H_INCLUDED */
