/* 
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dcmn_patch_database.c
 */



#include <shared/bsl.h>

#include <bcm/debug.h>

#ifdef BCM_PETRA_SUPPORT
#include <bcm_int/dpp/error.h>
#endif

#include <appl/diag/dcmn/dcmn_patch_database.h>
#include <appl/diag/system.h>


STATIC appl_diag_dcmn_patch patch_database[] = {
    {1, NULL},
    /*   
    *   patch1
    *   here
    */
    {2, NULL},
    /*   
    *   patch2
    *   here
    */
    {3, NULL},
    /*   
    *   patch3
    *   here
    */
    {4, NULL},
    /*   
    *   patch4
    *   here
    */
    {5, NULL},
    /*   
    *   patch5
    *   here
    */
    {6, NULL},
    /*   
    *   patch6
    *   here
    */  
    {7, NULL},
    /*   
    *   patch7
    *   here
    */
    {8, NULL},
    /*   
    *   patch8
    *   here
    */
    {9, NULL},
    /*   
    *   patch9
    *   here
    */
    {10, NULL},
    /*   
    *   patch10
    *   here
    */
    {11, NULL},
    /*   
    *   patch11
    *   here
    */
    {12, NULL},
    /*   
    *   patch12
    *   here
    */
    {13, NULL},
    /*   
    *   patch13
    *   here
    */
    {14, NULL},
    /*   
    *   patch14
    *   here
    */
    {15, NULL},
    /*   
    *   patch15
    *   here
    */
    {16, NULL},
    /*   
    *   patch16
    *   here
    */  
    {17, NULL},
    /*   
    *   patch17
    *   here
    */
    {18, NULL},
    /*   
    *   patch18
    *   here
    */
    {19, NULL},
    /*   
    *   patch19
    *   here
    */
    {20, NULL},
    /*   
    *   patch20
    *   here
    */
    {21, NULL},
    /*   
    *   patch21
    *   here
    */
    {22, NULL},
    /*   
    *   patch22
    *   here
    */
    {23, NULL},
    /*   
    *   patch23
    *   here
    */
    {24, NULL},
    /*   
    *   patch24
    *   here
    */
    {25, NULL},
    /*   
    *   patch25
    *   here
    */
    {26, NULL},
    /*   
    *   patch26
    *   here
    */  
    {27, NULL},
    /*   
    *   patch27
    *   here
    */
    {28, NULL},
    /*   
    *   patch28
    *   here
    */
    {29, NULL},
    /*   
    *   patch29
    *   here
    */
    {30, NULL},
    /*   
    *   patch30
    *   here
    */
    {31, NULL},
    /*   
    *   patch31
    *   here
    */
    {32, NULL},
    /*   
    *   patch32
    *   here
    */
    {33, NULL},
    /*   
    *   patch33
    *   here
    */
    {34, NULL},
    /*   
    *   patch34
    *   here
    */
    {35, NULL},
    /*   
    *   patch35
    *   here
    */
    {36, NULL},
    /*   
    *   patch36
    *   here
    */  
    {37, NULL},
    /*   
    *   patch37
    *   here
    */
    {38, NULL},
    /*   
    *   patch38
    *   here
    */
    {39, NULL},
    /*   
    *   patch39
    *   here
    */
    {40, NULL},
    /*   
    *   patch40
    *   here
    */
    {41, NULL},
    /*   
    *   patch41
    *   here
    */
    {42, NULL},
    /*   
    *   patch42
    *   here
    */
    {43, NULL},
    /*   
    *   patch43
    *   here
    */
    {44, NULL},
    /*   
    *   patch44
    *   here
    */
    {45, NULL},
    /*   
    *   patch45
    *   here
    */
    {46, NULL},
    /*   
    *   patch46
    *   here
    */  
    {47, NULL},
    /*   
    *   patch47
    *   here
    */
    {48, NULL},
    /*   
    *   patch48
    *   here
    */
    {49, NULL},
    /*   
    *   patch49
    *   here
    */
    {50, NULL},
    /*   
    *   patch50
    *   here
    */
};
  
/*
* Function:
*      soc_dcmn_patches_installed_get
* Purpose:
*      Find all installed patches
* Parameters:
*      max_arraysize        - (IN)   Max number of installed patches
*      installed_patches    - (OUT)  All installed patches
*      num_of_installed     - (OUT)  Number of installed patches
* Returns:
*      VOID
*/
int
appl_diag_dcmn_patches_installed_get (int unit, int max_arraysize, appl_diag_dcmn_patch* installed_patches, int* num_of_installed)
{
    int index, rv = 0;


    if(NULL == installed_patches) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "installed_patches is NULL\n")));
        return rv;            
    }

    if(NULL == num_of_installed) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "installed_patches is NULL\n")));
        return rv;            
    }

    *num_of_installed = 0;

    for (index = 0; index < TOTAL_PATCHES; index++){
        if (patch_database[index].str != NULL){
            if ((*num_of_installed) == max_arraysize) {
                rv = -1;
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "not enough room for all installed patches\n")));
                return rv;            
            }
            else {
                installed_patches[*num_of_installed].id  = patch_database[index].id;
                installed_patches[*num_of_installed].str = patch_database[index].str;
                (*num_of_installed)++;
            }
        }
    }

    return rv;
}

/* Get Current Version Insalled */
int
appl_diag_dcmn_version_get (int unit, char **version_pointer)
{
    int rv = 0;

    if(NULL == version_pointer) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "version_pointer is NULL\n")));
        return rv;            
    }

    *version_pointer = _build_release;

    return rv;
}

int appl_diag_dcmn_version_patch_get (int unit, 
                                int *version_patch)
{
    int index, rv = 0;

    if(NULL == version_patch) {
        rv = -1;
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "version_patch is NULL\n")));
        return rv;            
    }
    *version_patch = 0;
    for (index = 0; index < TOTAL_PATCHES; index++){               
        if (patch_database[index].str == NULL){
            break;
        }
        *version_patch = patch_database[index].id; 
        
    }
    return rv;


}
