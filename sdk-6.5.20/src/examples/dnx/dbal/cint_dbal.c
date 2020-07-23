/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_dbal.c
 * Purpose: Shows an example for using dabl in cint environment
 *
 * Main function:
 * dbal_access_example(unit);
 *
 *
 * cint ../../../src/examples/dnx/dbal/cint_dbal.c
 * c
 * dbal_access_example(0);
 */

uint32 get_val[10][64];


/**
* \brief
*  Function shows example for dbal access
*   \param [in] unit      -  unit Id
*/
int
dbal_access_example(
    int unit)
{
    int rv = BCM_E_NONE;
    uint32 entry_handle_id;
    int field_index, update_index;
    uint32 key_field[4];
    uint32 set_val[5];
    int inst_id = -1;
    char symbol_name[160];
    char result_type_name[160];

    /*
     * Get table handle
     */
    dnx_dbal_entry_handle_take(unit, "EGRESS_PP_PORT", &entry_handle_id);

    /*
     * Set key fields
     */
    key_field[0] = 1;
    key_field[1] = 0;
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "PP_PORT", &key_field[0]);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "CORE_ID", &key_field[1]);

    /*
     * Set Value fields
     */
    set_val[0] = 0xa;
    set_val[1] = 0xb;
    set_val[2] = 0xc;
    set_val[3] = 0xd;
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "VLAN_DOMAIN", inst_id, &set_val[0]);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "ESEM_ACCESS_CMD", inst_id, &set_val[1]);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "TX_OUTER_TAG_VID", inst_id, &set_val[2]);
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "VLAN_MEMBERSHIP_IF", inst_id, &set_val[3]);
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "EGRESS_FWD_CODE_PORT_PROFILE", inst_id, "STACKING_PORT");

    /*
     * Commit table values
     */
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");

    /*
     * Clear table handle
     */
    dnx_dbal_entry_handle_clear(unit, "EGRESS_PP_PORT", entry_handle_id);

    /*
     * Read table fields
     */

    /*
     * Set key fields
     */
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "PP_PORT", &key_field[0]);
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "CORE_ID", &key_field[1]);

    /*
     * Get all Value fields
     */
    dnx_dbal_entry_get(unit, entry_handle_id);

    /*
     * Get Value fields from handle
     */
    dnx_dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, "VLAN_DOMAIN", inst_id, &get_val[0]);
    dnx_dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, "ESEM_ACCESS_CMD", inst_id, &get_val[1]);
    dnx_dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, "TX_OUTER_TAG_VID", inst_id, &get_val[2]);
    dnx_dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, "VLAN_MEMBERSHIP_IF", inst_id, &get_val[3]);
    dnx_dbal_entry_handle_value_field_symbol_get(unit, entry_handle_id, "EGRESS_FWD_CODE_PORT_PROFILE", inst_id, &symbol_name[0]);

    dnx_dbal_table_clear(unit,"EGRESS_PP_PORT");


    /* Test RESULT TYPE case */
    /*
     * Clear table handle
     */
    dnx_dbal_entry_handle_clear(unit, "ESEM_DEFAULT_RESULT_TABLE", entry_handle_id);

    /*
     * Set key fields
     */
    key_field[2] = 1;
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "ESEM_DEFAULT_RESULT_PROFILE", &key_field[2]);

    /*
     * Set Value fields
     */
    set_val[4] = 0x6;
    dnx_dbal_entry_value_field_symbol_set(unit, entry_handle_id, "RESULT_TYPE", inst_id, "ETPS_AC");
    dnx_dbal_entry_value_field_arr32_set(unit, entry_handle_id, "VLAN_EDIT_VID_1", inst_id, &set_val[4]);

    /*
    * Commit table values
    */
    dnx_dbal_entry_commit(unit, entry_handle_id, "DBAL_COMMIT");

    /*
    * Clear table handle
    */
    dnx_dbal_entry_handle_clear(unit, "ESEM_DEFAULT_RESULT_TABLE", entry_handle_id);

    /*
    * Read table fields
    */

    /*
    * Set key fields
    */
    dnx_dbal_entry_key_field_arr32_set(unit, entry_handle_id, "ESEM_DEFAULT_RESULT_PROFILE", &key_field[2]);

    /*
    * Get all Value fields
    */
    dnx_dbal_entry_get(unit, entry_handle_id);

    /*
    * Get Value fields from handle
    */
    dnx_dbal_entry_handle_value_field_symbol_get(unit, entry_handle_id, "RESULT_TYPE", inst_id, &result_type_name[0]);
    dnx_dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, "VLAN_EDIT_VID_1", inst_id, &get_val[4]);

    dnx_dbal_table_clear(unit,"OLP_HEADER_CONFIGURATION");

    /*
     * compare results
     */
    if ((get_val[0][0] != 0xa) || (get_val[1][0] != 0xb) || (get_val[2][0] != 0xc) || (get_val[3][0] != 0xd) || (get_val[4][0] != 0x6) ||
        (sal_strcmp(symbol_name, "STACKING_PORT")))
    {
        printf("Compare Error: got %x,%x,%x,%x,%x,%s,%s expected 0xa,0xb,0xc,0xd,0x6,STACKING_PORT,ETPS_AC\n",
                get_val[0][0],get_val[1][0],get_val[2][0],get_val[3][0],get_val[4][0],symbol_name,result_type_name);

        return BCM_E_PARAM;
    }
    else
    {
        printf("Compare MATCH: got %x,%x,%x,%x,%x,%s,%s expected 0xa,0xb,0xc,0xd,0x6,STACKING_PORT,ETPS_AC\n",
                get_val[0][0],get_val[1][0],get_val[2][0],get_val[3][0],get_val[4][0],symbol_name,result_type_name);
        return rv;
    }
}

