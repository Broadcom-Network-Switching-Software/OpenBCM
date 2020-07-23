/*
 * $Id: bare_metal_dynamic_loading.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BARE_METAL_DYNAMIC_LOADING_H_INCLUDED

#define BARE_METAL_DYNAMIC_LOADING_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>

shr_error_e dnx_arr_prefix_dynamic_file_get(
    int unit,
    char arr_prefix_dynamic_file[RHFILE_MAX_SIZE]);

shr_error_e dnx_aod_tables_dynamic_file_get(
    int unit,
    char aod_dynamic_file[RHFILE_MAX_SIZE]);

shr_error_e dnx_mdb_app_db_dynamic_file_get(
    int unit,
    char mdb_app_db_dynamic_file[RHFILE_MAX_SIZE]);

shr_error_e dnx_pemla_soc_dynamic_file_get(
    int unit,
    char pemal_soc_file[RHFILE_MAX_SIZE]);

shr_error_e dnx_aod_sizes_dynamic_file_get(
    int unit,
    char aod_sizes_file[RHFILE_MAX_SIZE]);

#endif
