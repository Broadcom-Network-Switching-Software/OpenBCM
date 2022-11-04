/**
 * \file algo_mdb_em.c $Id$ Contains all of the MDB Exact Match algo callbacks
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
/*
 * Includes.
 * {
 */

#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_mdb_em_access.h>

#include <soc/sand/shrextend/shrextend_debug.h>

/*
 * }
 */

/*
 * Defines.
 * {
 */

#define DNX_ALGO_MDB_EM_KEY_FORMAT_RESOURCE "MDB EM cluster format"

/*
 * }
 */

/*
 * Macros.
 * {
 */

/*
 * }
 */

/**
 * See H file.
 */
void
dnx_algo_mdb_em_format_template_manager_print_cb(
    int unit,
    const void *data)
{
#if defined(DNX_SW_STATE_DIAGNOSTIC)
    uint8 *key_size_p = (uint8 *) data;
#endif /* DNX_SW_STATE_DIAGNOSTIC */

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "key_size", *key_size_p, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/**
 * See H file.
 */
shr_error_e
dnx_algo_mdb_em_format_template_manager_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    dnx_algo_template_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    data.first_profile = 0;
    data.nof_profiles = DNX_DATA_MAX_MDB_EM_NOF_FORMATS;
    data.flags = 0;
    data.max_references = DNX_DATA_MAX_MDB_EM_NOF_FORMATS;
    data.default_profile = 0;
    data.data_size = sizeof(uint8);
    sal_strncpy_s(data.name, DNX_ALGO_MDB_EM_KEY_FORMAT_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(mdb_em_format_template_manager.template_manager.create(unit, dbal_physical_table_id, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}
