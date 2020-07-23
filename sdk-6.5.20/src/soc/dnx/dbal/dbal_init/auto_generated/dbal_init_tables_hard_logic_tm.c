
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_internal_tables_hard_logic_tm.h>

shr_error_e
dbal_init_tables_hard_logic_tm_init(
int unit,
table_db_struct_t * cur_table_param,
dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_adapter_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_bier_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_buffers_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_crps_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ecgm_mapping_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ecgm_thresholds_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_egq_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_egw_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_esb_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_eventor_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fabric_cgm_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fabric_control_cells_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fabric_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fabric_if_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fabric_mesh_topology_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fabric_transmit_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fc_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_fmq_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_gddr6_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_gtimer_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_hbm_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_hbm_phy_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_imb_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ingress_compensation_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ingress_congestion_counters_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ingress_port_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ingress_shapers_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_instru_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_ipq_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_iqs_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_latency_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_macsec_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_meter_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_mib_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_multicast_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_nif_status_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_pll_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_pvt_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_rcpu_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_reassembly_context_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_sat_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_sch_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_snif_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_stack_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_stat_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_stif_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_synce_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_system_definition_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_system_red_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_tdm_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_tiny_mac_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_tm_hl_tm_trunk_definition_init(unit, cur_table_param, table_info));
exit:
    SHR_FUNC_EXIT;
}
