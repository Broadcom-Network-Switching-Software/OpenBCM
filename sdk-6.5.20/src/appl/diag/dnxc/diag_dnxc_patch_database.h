/** \file diag_dnxc_patch_database.h
 *
 * Diagnostic pack for PATCHES
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNXC_PATCH_DATABASE_H_INCLUDED
#define DIAG_DNXC_PATCH_DATABASE_H_INCLUDED

typedef struct
{
    int id;                     /* patch ID */
    char *str;                  /* data of patch */
} diag_dnxc_patch;

/**
 * \brief - get patches tables size
 *
 * \param [in] unit - chip unit id.
 * \param [out] table_size - size of the patch table size.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_patches_table_size_get(
    int unit,
    int *table_size);

/**
 * \brief - Find all installed patches
 *
 * \param [in] unit - chip unit id.
 * \param [in] max_arraysize - Max number of installed patches.
 * \param [out] installed_patches - All installed patches.
 * \param [out] num_of_installed - Number of installed patches.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_patches_installed_get(
    int unit,
    int max_arraysize,
    diag_dnxc_patch * installed_patches,
    int *num_of_installed);
/**
 * \brief - Get Current SDK Version Insalled
 *
 * \param [in] unit - chip unit id.
 * \param [out] version_pointer - current sdk version pointer.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_version_get(
    int unit,
    char **version_pointer);

/**
 * \brief - Get Current Patch Version
 *
 * \param [in] unit - chip unit id.
 * \param [out] version_patch - current patch version pointer.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_dnxc_version_patch_get(
    int unit,
    int *version_patch);

#endif /* DIAG_DNXC_PATCH_DATABASE_H_INCLUDED */
