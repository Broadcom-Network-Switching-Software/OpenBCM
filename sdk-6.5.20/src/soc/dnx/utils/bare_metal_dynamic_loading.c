/*
 * $Id: bare_metal_dynamic_loading.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/utils/bare_metal_dynamic_loading.h>

shr_error_e
dnx_arr_prefix_dynamic_file_get(
    int unit,
    char arr_prefix_dynamic_file[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncat(file_name, "/bare_metal/auto_generated/dnx_data/arr/", RHFILE_MAX_SIZE - 1);
    sal_strncat(file_name, image_name, RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncat(file_name, "_arr.xml", RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncpy(arr_prefix_dynamic_file, file_name, RHFILE_MAX_SIZE - 1);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aod_tables_dynamic_file_get(
    int unit,
    char aod_dynamic_file[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/bare_metal/auto_generated/dnx_data/aod/", RHFILE_MAX_SIZE - 1);
    sal_strncat(file_name, image_name, RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncat(file_name, "_aod.xml", RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncpy(aod_dynamic_file, file_name, RHFILE_MAX_SIZE - 1);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_mdb_app_db_dynamic_file_get(
    int unit,
    char mdb_app_db_dynamic_file[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/bare_metal/auto_generated/dnx_data/mdb_app_db/", RHFILE_MAX_SIZE - 1);
    sal_strncat(file_name, image_name, RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncat(file_name, "_mdb_app_db.xml", RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncpy(mdb_app_db_dynamic_file, file_name, RHFILE_MAX_SIZE - 1);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_pemla_soc_dynamic_file_get(
    int unit,
    char pemal_soc_file[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/bare_metal/auto_generated/dnx_data/pemla_soc/", RHFILE_MAX_SIZE - 1);
    sal_strncat(file_name, image_name, RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncat(file_name, "_pemla_soc.xml", RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncpy(pemal_soc_file, file_name, RHFILE_MAX_SIZE - 1);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aod_sizes_dynamic_file_get(
    int unit,
    char aod_sizes_file[RHFILE_MAX_SIZE])
{
    char file_name[RHFILE_MAX_SIZE] = { 0 };
    char *image_name;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_legacy_get(unit, &image_name));
    sal_strncpy(file_name, "/bare_metal/auto_generated/dnx_data/aod_sizes/", RHFILE_MAX_SIZE - 1);
    sal_strncat(file_name, image_name, RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncat(file_name, "_aod_sizes.xml", RHFILE_MAX_SIZE - sal_strlen(file_name) - 1);
    sal_strncpy(aod_sizes_file, file_name, RHFILE_MAX_SIZE - 1);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
