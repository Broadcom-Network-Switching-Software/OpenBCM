
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_internal_tables_hard_logic_pp_kleap.h>

shr_error_e
dbal_init_tables_hard_logic_pp_kleap_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_context_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_epmf_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_fwd12_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf1_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf1_initial_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf2_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_ipmf3_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_vt1_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_vt23_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_pp_kleap_hl_pp_kleap_vt45_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
