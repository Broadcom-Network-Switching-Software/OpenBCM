
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
l3_vrrp_exem_db_sw_indirect_table_size_0_cb(
int unit,
void * entry_handle,
dbal_fields_e current_mapped_field_id,
uint32 * offset)
{
    int phy_db_size__sexem_1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FORMULA_CB_GET_PHYDB_SIZE(unit, DBAL_PHYSICAL_TABLE_SEXEM_1, phy_db_size__sexem_1);
    *offset = phy_db_size__sexem_1+1;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
