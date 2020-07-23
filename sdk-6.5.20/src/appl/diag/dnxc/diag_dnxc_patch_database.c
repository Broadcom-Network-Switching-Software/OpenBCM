
/** \file diag_dnxc_patch_database.c
 *
 * Diagnostic pack for PATCHES
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COMMON

/* shared */
#include <shared/bsl.h>
/* appl */
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include "diag_dnxc_patch_database.h"

static diag_dnxc_patch dnxc_patch_database[] = {
    {1, NULL}
    ,
    /*
     *   patch1
     *   here
     */
    {2, NULL}
    ,
    /*
     *   patch2
     *   here
     */
    {3, NULL}
    ,
    /*
     *   patch3
     *   here
     */
    {4, NULL}
    ,
    /*
     *   patch4
     *   here
     */
    {5, NULL}
    ,
    /*
     *   patch5
     *   here
     */
    {6, NULL}
    ,
    /*
     *   patch6
     *   here
     */
    {7, NULL}
    ,
    /*
     *   patch7
     *   here
     */
    {8, NULL}
    ,
    /*
     *   patch8
     *   here
     */
    {9, NULL}
    ,
    /*
     *   patch9
     *   here
     */
    {10, NULL}
    ,
    /*
     *   patch10
     *   here
     */
    {11, NULL}
    ,
    /*
     *   patch11
     *   here
     */
    {12, NULL}
    ,
    /*
     *   patch12
     *   here
     */
    {13, NULL}
    ,
    /*
     *   patch13
     *   here
     */
    {14, NULL}
    ,
    /*
     *   patch14
     *   here
     */
    {15, NULL}
    ,
    /*
     *   patch15
     *   here
     */
    {16, NULL}
    ,
    /*
     *   patch16
     *   here
     */
    {17, NULL}
    ,
    /*
     *   patch17
     *   here
     */
    {18, NULL}
    ,
    /*
     *   patch18
     *   here
     */
    {19, NULL}
    ,
    /*
     *   patch19
     *   here
     */
    {20, NULL}
    ,
    /*
     *   patch20
     *   here
     */
    {21, NULL}
    ,
    /*
     *   patch21
     *   here
     */
    {22, NULL}
    ,
    /*
     *   patch22
     *   here
     */
    {23, NULL}
    ,
    /*
     *   patch23
     *   here
     */
    {24, NULL}
    ,
    /*
     *   patch24
     *   here
     */
    {25, NULL}
    ,
    /*
     *   patch25
     *   here
     */
    {26, NULL}
    ,
    /*
     *   patch26
     *   here
     */
    {27, NULL}
    ,
    /*
     *   patch27
     *   here
     */
    {28, NULL}
    ,
    /*
     *   patch28
     *   here
     */
    {29, NULL}
    ,
    /*
     *   patch29
     *   here
     */
    {30, NULL}
    ,
    /*
     *   patch30
     *   here
     */
    {31, NULL}
    ,
    /*
     *   patch31
     *   here
     */
    {32, NULL}
    ,
    /*
     *   patch32
     *   here
     */
    {33, NULL}
    ,
    /*
     *   patch33
     *   here
     */
    {34, NULL}
    ,
    /*
     *   patch34
     *   here
     */
    {35, NULL}
    ,
    /*
     *   patch35
     *   here
     */
    {36, NULL}
    ,
    /*
     *   patch36
     *   here
     */
    {37, NULL}
    ,
    /*
     *   patch37
     *   here
     */
    {38, NULL}
    ,
    /*
     *   patch38
     *   here
     */
    {39, NULL}
    ,
    /*
     *   patch39
     *   here
     */
    {40, NULL}
    ,
    /*
     *   patch40
     *   here
     */
    {41, NULL}
    ,
    /*
     *   patch41
     *   here
     */
    {42, NULL}
    ,
    /*
     *   patch42
     *   here
     */
    {43, NULL}
    ,
    /*
     *   patch43
     *   here
     */
    {44, NULL}
    ,
    /*
     *   patch44
     *   here
     */
    {45, NULL}
    ,
    /*
     *   patch45
     *   here
     */
    {46, NULL}
    ,
    /*
     *   patch46
     *   here
     */
    {47, NULL}
    ,
    /*
     *   patch47
     *   here
     */
    {48, NULL}
    ,
    /*
     *   patch48
     *   here
     */
    {49, NULL}
    ,
    /*
     *   patch49
     *   here
     */
    {50, NULL}
    ,
    /*
     *   patch50
     *   here
     */
};

/** See .h file */
shr_error_e
diag_dnxc_patches_table_size_get(
    int unit,
    int *table_size)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table_size, _SHR_E_PARAM, "table_size");

    *table_size = sizeof(dnxc_patch_database) / sizeof(dnxc_patch_database[0]);

exit:
    SHR_FUNC_EXIT;

}

/** See .h file */
shr_error_e
diag_dnxc_patches_installed_get(
    int unit,
    int max_arraysize,
    diag_dnxc_patch * installed_patches,
    int *num_of_installed)
{
    int index, table_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(installed_patches, _SHR_E_PARAM, "installed_patches");
    SHR_NULL_CHECK(num_of_installed, _SHR_E_PARAM, "num_of_installed");

    *num_of_installed = 0;

    SHR_IF_ERR_EXIT(diag_dnxc_patches_table_size_get(unit, &table_size));
    for (index = 0; index < table_size; index++)
    {
        if (dnxc_patch_database[index].str != NULL)
        {
            if ((*num_of_installed) == max_arraysize)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "not enough room for all installed patches\n");
            }
            else
            {
                installed_patches[*num_of_installed].id = dnxc_patch_database[index].id;
                installed_patches[*num_of_installed].str = dnxc_patch_database[index].str;
                (*num_of_installed)++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/** See .h file */
shr_error_e
diag_dnxc_version_get(
    int unit,
    char **version_pointer)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(version_pointer, _SHR_E_PARAM, "version_pointer");

    *version_pointer = _build_release;

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
diag_dnxc_version_patch_get(
    int unit,
    int *version_patch)
{
    int index, table_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(version_patch, _SHR_E_PARAM, "version_patch");

    *version_patch = 0;
    SHR_IF_ERR_EXIT(diag_dnxc_patches_table_size_get(unit, &table_size));
    for (index = 0; index < table_size; index++)
    {
        if (dnxc_patch_database[index].str == NULL)
        {
            break;
        }
        *version_patch = dnxc_patch_database[index].id;
    }

exit:
    SHR_FUNC_EXIT;
}
