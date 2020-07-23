
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_internal_tables_sw_state_pp_qos.h>

shr_error_e
dbal_init_tables_sw_state_pp_qos_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_init_tables_sw_state_pp_qos_sw_pp_qos_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
