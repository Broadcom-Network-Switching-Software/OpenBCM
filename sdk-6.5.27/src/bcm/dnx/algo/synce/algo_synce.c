/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_synce.c
 *
 *  Include:
 *      SyncE DB initialization and deinitialization.
 *      SyncE DB set and get functions.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SYNCEDNX

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_synce_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>

/*
 * }
 */
/*
 * Macros
 * {
 */
/*
 * }
 */
/*
 * Module Init / Deinit  
 * { 
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_synce_init(
    int unit)
{

    int nof_synce_types;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create SW State instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_synce_db.init(unit));
    /*
     * synce_type is equal to nof synce pll
     */
    nof_synce_types = dnx_data_synce.general.nof_synce_type_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_synce_db.synce_type_core_group_id.alloc(unit, nof_synce_types));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * SyncE Set/Get functions
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_synce_type_core_group_id_get(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    uint32 *core_group)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_synce_db.synce_type_core_group_id.get(unit, synce_type, core_group));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_synce_type_core_group_id_set(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    uint32 core_group)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_synce_db.synce_type_core_group_id.set(unit, synce_type, core_group));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_fabric_synce_enable_set(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (synce_type)
    {
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.master_synce_enabled.set(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.slave_synce_enabled.set(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.master_set1_synce_enabled.set(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.slave_set1_synce_enabled.set(unit, enable));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid synce type");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_fabric_synce_enable_get(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (synce_type)
    {
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.master_synce_enabled.get(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.slave_synce_enabled.get(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.master_set1_synce_enabled.get(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.slave_set1_synce_enabled.get(unit, enable));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid synce type");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_nif_synce_enable_set(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (synce_type)
    {
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.master_synce_enabled.set(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.slave_synce_enabled.set(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.master_set1_synce_enabled.set(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.slave_set1_synce_enabled.set(unit, enable));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid synce type");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_nif_synce_enable_get(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (synce_type)
    {
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.master_synce_enabled.get(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET0:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.slave_synce_enabled.get(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_MASTER_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.master_set1_synce_enabled.get(unit, enable));
            break;
        case DBAL_ENUM_FVAL_SYNCE_TYPE_SLAVE_SYNCE_TO_SET1:
            SHR_IF_ERR_EXIT(dnx_algo_synce_db.nif_synce_status.slave_set1_synce_enabled.get(unit, enable));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid synce type");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
