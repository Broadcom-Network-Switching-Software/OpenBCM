/* 
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dcmn_patch_database.h
 */
 
#ifndef _APPL_DIAG_DCMN_PATCH_DATABASE_H
#define _APPL_DIAG_DCMN_PATCH_DATABASE_H
 
#define TOTAL_PATCHES 50

typedef struct{
    int     id;				/* patch ID */
    char*   str;			/* data of patch */
} appl_diag_dcmn_patch;

/* Find all installed patches */
int appl_diag_dcmn_patches_installed_get (
    int unit, 
    int max_arraysize, 
    appl_diag_dcmn_patch* installed_patches, 
    int* num_of_installed);

/* Get Current Version Insalled */
int appl_diag_dcmn_version_get (int unit, 
                                char **version_pointer);   
int appl_diag_dcmn_version_patch_get (int unit, 
                                int *version_patch);   

#endif /*_APPL_DCMN_PATCH_DATABASE_H*/
