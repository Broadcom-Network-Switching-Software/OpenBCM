
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
l2_gport_to_forwarding_sw_info_indirect_table_size_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    int phy_db_size__inlif_1;
    int phy_db_size__inlif_2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_INLIF_1, phy_db_size__inlif_1);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_INLIF_2, phy_db_size__inlif_2);
    *offset = phy_db_size__inlif_1/dnx_data_lif.in_lif.inlif_minimum_index_size_get(unit)+dnx_data_device.general.nof_cores_get(unit)*phy_db_size__inlif_2/dnx_data_lif.in_lif.inlif_minimum_index_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
l2_forwarding_sw_info_to_gport_indirect_table_size_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    int phy_db_size__inlif_1;
    int phy_db_size__inlif_2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_INLIF_1, phy_db_size__inlif_1);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_INLIF_2, phy_db_size__inlif_2);
    *offset = phy_db_size__inlif_1/dnx_data_lif.in_lif.inlif_minimum_index_size_get(unit)+dnx_data_device.general.nof_cores_get(unit)*phy_db_size__inlif_2/dnx_data_lif.in_lif.inlif_minimum_index_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ingress_gport_to_local_in_lif_sw_indirect_table_size_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    int phy_db_size__inlif_2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_INLIF_2, phy_db_size__inlif_2);
    *offset = dnx_data_device.general.nof_cores_get(unit)*phy_db_size__inlif_2/dnx_data_lif.in_lif.inlif_minimum_index_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
egress_virtual_gport_to_match_info_sw_indirect_table_size_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    int phy_db_size__esem;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_ESEM, phy_db_size__esem);
    *offset = phy_db_size__esem;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
local_sbc_in_lif_match_info_sw_indirect_table_size_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    int phy_db_size__inlif_1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_INLIF_1, phy_db_size__inlif_1);
    *offset = phy_db_size__inlif_1/dnx_data_lif.in_lif.inlif_minimum_index_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
local_dpc_in_lif_match_info_sw_indirect_table_size_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    int phy_db_size__inlif_2;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_INLIF_2, phy_db_size__inlif_2);
    *offset = dnx_data_device.general.nof_cores_get(unit)*phy_db_size__inlif_2/dnx_data_lif.in_lif.inlif_minimum_index_size_get(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
