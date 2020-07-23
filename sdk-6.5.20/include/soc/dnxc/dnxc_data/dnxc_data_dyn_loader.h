
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXC_DATA_DYN_LOADER_H_

#define _DNXC_DATA_DYN_LOADER_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <shared/utilex/utilex_str.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

shr_error_e dnxc_data_dyn_loader_parse_init(
    int unit);

shr_error_e dnxc_data_dyn_loader_parse_deinit(
    int unit);

typedef shr_error_e(
    *dnxc_data_dyn_file_path_get_f) (
    int unit,
    char file_path[RHFILE_MAX_SIZE]);

shr_error_e dnxc_data_dyn_loader_define_value_set(
    int unit,
    dnxc_data_define_t * define_info);

shr_error_e dnxc_data_dyn_loader_feature_value_set(
    int unit,
    dnxc_data_feature_t * feature_info);

shr_error_e dnxc_data_dyn_loader_numeric_value_set(
    int unit,
    dnxc_data_define_t * numeric_info);

shr_error_e dnxc_data_dyn_loader_table_value_set(
    int unit,
    dnxc_data_table_t * table_info);

shr_error_e dnxc_data_dyn_loader_dev_id_verify(
    int unit);

#endif
