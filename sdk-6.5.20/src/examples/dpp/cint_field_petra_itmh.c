/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_field_petra_itmh.c
 * Purpose: pmf Tm program for parsing the ITMH petra mode 
 *  	supported only in Arad+
 *  	this PMF program olny parse tc, dp and dest fields
 *      whitout ITMH extensions
 *
 * Usage:
 * Enable soc property custom_feature_support_petra_itmh
 */



int bcm_field_petra_itmh_program_selection(int unit) {

    int result = 0;
    bcm_field_presel_t            presel = 0;
    int                           presel_flags = 0;
    bcm_field_presel_set_t        presel_set;
    int auxRes;

    /* 
   * Create a preselector according to Forwading-Type = TM
   */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create_stage_id\n");
          auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
          return result;
      }
    } else {
      result = bcm_field_presel_create(unit, &presel);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel);
          return result;
      }
    }
    result = bcm_field_qualify_ForwardingType(unit, presel | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeTrafficManagement);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        auxRes = bcm_field_presel_destroy(unit, presel | presel_flags);
        return result;
    }

    /* Create the Preslector-set with a single preselector */
    BCM_FIELD_PRESEL_INIT(presel_set);
    BCM_FIELD_PRESEL_ADD(presel_set, presel);

    result = bcm_field_petra_itmh_unicast(unit, presel_set);
    if (BCM_E_NONE != result) {
        printf("Error in bfd_ipv4_single_hop_field_trap_qualifier Err %x\n", result);
        return result;
    }

    result = bcm_field_petra_itmh_unicast_flow(unit, presel_set);
    if (BCM_E_NONE != result) {
        printf("Error in bfd_ipv4_single_hop_field_oam_action Err %x\n", result);
        return result;
    }

    result = bcm_field_petra_itmh_mc(unit, presel_set);
    if (BCM_E_NONE != result) {
        printf("Error in bfd_ipv4_single_hop_field_oam_restore Err %x\n", result);
        return result;
    }

    return result;
}

/* this field group parse in case the detination is UC port*/

int bcm_field_petra_itmh_unicast(int unit, bcm_field_presel_set_t presel_set) {

    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry;
    bcm_field_entry_t entry_tc;
    bcm_field_entry_t entry_dp;
    bcm_field_entry_t entry_bytes_to_remove;
    bcm_field_data_qualifier_t dest_qualifier;
    bcm_field_data_qualifier_t dest_type_qualifier;
    bcm_field_data_qualifier_t tc_qualifier;
    bcm_field_data_qualifier_t dp_qualifier;
    uint8 data, mask;
    int rv = 0;
    int sys_gport;
    bcm_field_stage_t stage;
    int result = 0;



    /*create user define quelifier*/

    /*Data Qualifier for FWD_DEST_INFO.type - ITMH Base[17:16]*/
    bcm_field_data_qualifier_t_init(&dest_type_qualifier);
    dest_type_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dest_type_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dest_type_qualifier.offset = 9;
    dest_type_qualifier.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &dest_type_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /*Data Qualifier for FWD_DEST_INFO - ITMH Base[15:0]*/
    bcm_field_data_qualifier_t_init(&dest_qualifier);
    dest_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dest_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dest_qualifier.offset = 23;
    dest_qualifier.length = 16;
    rv = bcm_field_data_qualifier_create(unit, &dest_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }


    /*Data Qualifier for TC - ITMH Base[22:20]*/
    bcm_field_data_qualifier_t_init(&tc_qualifier);
    tc_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    tc_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    tc_qualifier.offset = 14;
    tc_qualifier.length = 3;
    rv = bcm_field_data_qualifier_create(unit, &tc_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /*Data Qualifier for DP - ITMH Base[19:18]*/
    bcm_field_data_qualifier_t_init(&dp_qualifier);
    dp_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dp_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dp_qualifier.offset = 11;
    dp_qualifier.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &dp_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
    grp.group = -1;




    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dest_type_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dest_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               tc_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dp_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }




    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);


    /* 
     * Define Programabble Field Group
     */
    grp.priority = 1;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeDirectExtraction;


    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n", result);
        return result;
    }

    /* Attach the Preselector-Set */
    result = bcm_field_group_presel_set(unit, grp.group, &presel_set);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        return result;
    };

    /* Attached the action to the field group */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n", result);
        return result;
    }




    /*create an entry for unicast*/
    result = bcm_field_entry_create(unit, grp.group, &entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* action will be done only if ITMH.dest_ifo_type= unicast*/
    data = 0;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t action[3];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionRedirect;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 15;
    action[0].lsb = 0;
    action[0].qualifier = dest_qualifier.qual_id;

    action[1].bits = 3;
    action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[1].value = 0;

    action[2].bits = 1;
    action[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[2].value = 1;


    /*new entry*/
    result = bcm_field_direct_extraction_action_add(unit, entry, extract, 3, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /*create an entry for tc action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_tc);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action tc
    */

    /* action will be done only if ITMH.dest_ifo_type= unicast*/
    data = 0;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_tc, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionPrioIntNew;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 3;
    action[0].lsb = 0;
    action[0].qualifier = tc_qualifier.qual_id;


    result = bcm_field_direct_extraction_action_add(unit, entry_tc, extract, 1, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }



    /*create an entry for dp action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_dp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action dp
    */

    /* action will be done only if ITMH.dest_ifo_type= unicast*/
    data = 0;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_dp, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionDropPrecedence;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 2;
    action[0].lsb = 0;
    action[0].qualifier = dp_qualifier.qual_id;



    result = bcm_field_direct_extraction_action_add(unit, entry_dp, extract, 1, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }


    /*create an entry for entry_bytes_to_remove action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_bytes_to_remove);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action entry_bytes_to_remove
    */

    /* action will be done only if ITMH.dest_ifo_type= unicast*/
    data = 0;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_dp, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionStartPacketStrip;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[0].bits  = 6;
    action[0].value = 0x4;

    action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[1].bits  = 2;
    action[1].value = 0x1; /*bcmFieldStartToL2Strip*/


    result = bcm_field_direct_extraction_action_add(unit, entry_bytes_to_remove, extract, 2, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }


    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }

    return result;

}



/* this field group parse in case the detination is UC flow*/

int bcm_field_petra_itmh_unicast_flow(int unit, bcm_field_presel_set_t presel_set) {

    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry;
    bcm_field_entry_t entry_tc;
    bcm_field_entry_t entry_dp;
    bcm_field_entry_t entry_bytes_to_remove;
    bcm_field_data_qualifier_t dest_qualifier;
    bcm_field_data_qualifier_t dest_type_qualifier;
    bcm_field_data_qualifier_t tc_qualifier;
    bcm_field_data_qualifier_t dp_qualifier;
    uint8 data, mask;
    int rv = 0;

    int sys_gport;
    bcm_field_stage_t stage;

    int result = 0;



    /*create user define quelifier*/

    /*Data Qualifier for FWD_DEST_INFO.type - ITMH Base[17:16]*/
    bcm_field_data_qualifier_t_init(&dest_type_qualifier);
    dest_type_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dest_type_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dest_type_qualifier.offset = 9;
    dest_type_qualifier.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &dest_type_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /*Data Qualifier for FWD_DEST_INFO - ITMH Base[15:0]*/
    bcm_field_data_qualifier_t_init(&dest_qualifier);
    dest_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dest_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dest_qualifier.offset = 23;
    dest_qualifier.length = 16;
    rv = bcm_field_data_qualifier_create(unit, &dest_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }


    /*Data Qualifier for TC - ITMH Base[22:20]*/
    bcm_field_data_qualifier_t_init(&tc_qualifier);
    tc_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    tc_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    tc_qualifier.offset = 14;
    tc_qualifier.length = 3;
    rv = bcm_field_data_qualifier_create(unit, &tc_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /*Data Qualifier for DP - ITMH Base[19:18]*/
    bcm_field_data_qualifier_t_init(&dp_qualifier);
    dp_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dp_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dp_qualifier.offset = 11;
    dp_qualifier.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &dp_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
    grp.group = -1;




    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dest_type_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dest_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               tc_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dp_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }




    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);


    /* 
     * Define Programabble Field Group
     */
    grp.priority = 2;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeDirectExtraction;


    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n", result);
        return result;
    }

    /* Attach the Preselector-Set */
    result = bcm_field_group_presel_set(unit, grp.group, &presel_set);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        return result;
    };

    /* Attached the action to the field group */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n", result);
        return result;
    }




    /*create an entry for unicast*/
    result = bcm_field_entry_create(unit, grp.group, &entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* action will be done only if ITMH.dest_ifo_type= unicast flow*/
    data = 3;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t action[3];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionRedirect;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 15;
    action[0].lsb = 0;
    action[0].qualifier = dest_qualifier.qual_id;

    action[1].bits = 2;
    action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[1].value = 0;

    action[2].bits = 2;
    action[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[2].value = 3;


    /*new entry*/
    result = bcm_field_direct_extraction_action_add(unit, entry, extract, 3, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /*create an entry for tc action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_tc);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action tc
    */

    /* action will be done only if ITMH.dest_ifo_type= unicast flow*/
    data = 3;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_tc, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionPrioIntNew;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 3;
    action[0].lsb = 0;
    action[0].qualifier = tc_qualifier.qual_id;


    result = bcm_field_direct_extraction_action_add(unit, entry_tc, extract, 1, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }



    /*create an entry for dp action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_dp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action dp
    */

    /* action will be done only if ITMH.dest_ifo_type= unicast flow*/
    data = 3;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_dp, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionDropPrecedence;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 2;
    action[0].lsb = 0;
    action[0].qualifier = dp_qualifier.qual_id;



    result = bcm_field_direct_extraction_action_add(unit, entry_dp, extract, 1, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }


    /*create an entry for entry_bytes_to_remove action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_bytes_to_remove);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action entry_bytes_to_remove
    */

    /* action will be done only if ITMH.dest_ifo_type= unicast flow*/
    data = 3;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_dp, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionStartPacketStrip;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[0].bits  = 6;
    action[0].value = 0x4;

    action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[1].bits  = 2;
    action[1].value = 0x1; /*bcmFieldStartToL2Strip*/

    result = bcm_field_direct_extraction_action_add(unit, entry_bytes_to_remove, extract, 2, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }

    return result;

}

/* this field group parse in case the detination is MC*/
int bcm_field_petra_itmh_mc(int unit, bcm_field_presel_set_t presel_set) {

    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t entry;
    bcm_field_entry_t entry_tc;
    bcm_field_entry_t entry_dp;
    bcm_field_entry_t entry_bytes_to_remove;
    bcm_field_entry_t entry_mc;
    bcm_field_data_qualifier_t dest_qualifier;
    bcm_field_data_qualifier_t dest_type_qualifier;
    bcm_field_data_qualifier_t tc_qualifier;
    bcm_field_data_qualifier_t dp_qualifier;
    uint8 data, mask;
    int rv = 0;

    int sys_gport;
    bcm_field_stage_t stage;

    int result = 0;

    /*create user define quelifier*/

    /*Data Qualifier for FWD_DEST_INFO.type - ITMH Base[17:16]*/
    bcm_field_data_qualifier_t_init(&dest_type_qualifier);
    dest_type_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dest_type_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dest_type_qualifier.offset = 9;
    dest_type_qualifier.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &dest_type_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /*Data Qualifier for FWD_DEST_INFO - ITMH Base[15:0]*/
    bcm_field_data_qualifier_t_init(&dest_qualifier);
    dest_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dest_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dest_qualifier.offset = 23;
    dest_qualifier.length = 16;
    rv = bcm_field_data_qualifier_create(unit, &dest_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }


    /*Data Qualifier for TC - ITMH Base[22:20]*/
    bcm_field_data_qualifier_t_init(&tc_qualifier);
    tc_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    tc_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    tc_qualifier.offset = 14;
    tc_qualifier.length = 3;
    rv = bcm_field_data_qualifier_create(unit, &tc_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /*Data Qualifier for DP - ITMH Base[19:18]*/
    bcm_field_data_qualifier_t_init(&dp_qualifier);
    dp_qualifier.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dp_qualifier.offset_base = bcmFieldDataOffsetBaseL2Header;
    dp_qualifier.offset = 11;
    dp_qualifier.length = 2;
    rv = bcm_field_data_qualifier_create(unit, &dp_qualifier);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%x \n", rv);
        return rv;
    }

    /* Initialized Field Group */
    bcm_field_group_config_t_init(&grp);
    grp.group = -1;

    /*add qualifiers*/
    BCM_FIELD_QSET_INIT(grp.qset);

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dest_type_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dest_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               tc_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit,
                                               grp.qset,
                                               dp_qualifier.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add Err %x\n", result);
        return result;
    }




    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassSourceSet);



    /* 
     * Define Programabble Field Group
     */
    grp.priority = 3;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeDirectExtraction;


    /* create field group */
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create with Err %x\n", result);
        return result;
    }

    /* Attach the Preselector-Set */
    result = bcm_field_group_presel_set(unit, grp.group, &presel_set);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        return result;
    };

    /* Attached the action to the field group */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set Err %x\n", result);
        return result;
    }




    /*create an entry for mc*/
    result = bcm_field_entry_create(unit, grp.group, &entry);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* action will be done only if ITMH.dest_ifo_type= mc*/
    data = 2;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t action[3];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionRedirect;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 15;
    action[0].lsb = 0;
    action[0].qualifier = dest_qualifier.qual_id;

    action[1].bits = 1;
    action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[1].value = 0;

    action[2].bits = 3;
    action[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[2].value = 5;


    /*new entry*/
    result = bcm_field_direct_extraction_action_add(unit, entry, extract, 3, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /*create an entry for tc action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_tc);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action tc
    */

    /* action will be done only if ITMH.dest_ifo_type= mc*/
    data = 2;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_tc, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionPrioIntNew;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 3;
    action[0].lsb = 0;
    action[0].qualifier = tc_qualifier.qual_id;


    result = bcm_field_direct_extraction_action_add(unit, entry_tc, extract, 1, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }



    /*create an entry for dp action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_dp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action dp
    */

    /* action will be done only if ITMH.dest_ifo_type= mc*/
       data = 2;
       mask = 3;
       rv = bcm_field_qualify_data(unit, entry_dp, dest_type_qualifier.qual_id, &data, &mask, 1);
       if (BCM_FAILURE(rv)) {
           printf("bcm_field_qualify_data failed. Error:%x \n", rv);
           return rv;
       }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionDropPrecedence;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 2;
    action[0].lsb = 0;
    action[0].qualifier = dp_qualifier.qual_id;



    result = bcm_field_direct_extraction_action_add(unit, entry_dp, extract, 1, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /*create an entry for entry_bytes_to_remove action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_bytes_to_remove);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }

    /* 
    action entry_bytes_to_remove
    */

    /* action will be done only if ITMH.dest_ifo_type= mc*/
    data = 2;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_dp, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }

    /* construct the action*/
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionStartPacketStrip;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[0].bits  = 6;
    action[0].value = 0x4;

    action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[1].bits  = 2;
    action[1].value = 0x1; /*bcmFieldStartToL2Strip*/



    result = bcm_field_direct_extraction_action_add(unit, entry_bytes_to_remove, extract, 2, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    bcm_field_entry_t entry_user_header;


    /*create an entry for entry_user_header action*/
    result = bcm_field_entry_create(unit, grp.group, &entry_user_header);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create Err %x\n", result);
        return result;
    }


    data = 2;
    mask = 3;
    rv = bcm_field_qualify_data(unit, entry_user_header, dest_type_qualifier.qual_id, &data, &mask, 1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_data failed. Error:%x \n", rv);
        return rv;
    }


    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&action);

    extract.action = bcmFieldActionClassSourceSet;
    extract.bias = 0;

    action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    action[0].bits  = 16;
    action[0].lsb = 0;
    action[0].qualifier = dest_qualifier.qual_id;

    action[1].bits = 1;
    action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    action[1].value = 0;

    /*new entry*/
    result = bcm_field_direct_extraction_action_add(unit, entry_user_header, extract, 2, &action);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }


    result = bcm_field_group_install(unit, grp.group);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }

    return result;

}


/* Function to verify ForwardingTypeNew action took effect used in regression tests*/
int run_verify_itmh_parsing(int unit) {
    int rv = BCM_E_NONE;
    char *signal_name;
    char *block = "ERPP";
    char *from;
    char *to;
    uint32 signal_value = -1;
    int expected_tc = 5;
    int expected_dp = 3;
    char *proc_name;
    int core = 0;

    printf("\n Enter to verify ITMH parsing\n");

    proc_name = "TC";
    signal_name = "TC";
    from = "Parser";
    to = NULL;

    rv =  dpp_dsig_read(unit, core, block, from, to, signal_name, &signal_value, 1);
    if (rv != BCM_E_NONE || signal_value < 0) {
        printf("\n Error in dpp_dsig_read() \n");
        return BCM_E_NONE;
    }
    if (signal_value != expected_tc) {
        printf("%s(): FAIL: %s is expected %d but received %d!\n", proc_name, signal_name, expected_tc, signal_value);
        return BCM_E_FAIL;
    }
    printf("%s(): Done. Verified %s change to 0x%x.\n", proc_name, signal_name, signal_value);

    proc_name = "DP";
    signal_name = "DP";

    rv =  dpp_dsig_read(unit, core, block, from, to, signal_name, &signal_value, 1);
    if (rv != BCM_E_NONE || signal_value < 0) {
        printf("\n Error in dpp_dsig_read() \n");
        return BCM_E_NONE;
    }
    if (signal_value != expected_dp) {
        printf("%s(): FAIL: %s is expected %d but received %d!\n", proc_name, signal_name, expected_dp, signal_value);
        return BCM_E_FAIL;
    }
    printf("%s(): Done. Verified %s change to 0x%x.\n", proc_name, signal_name, signal_value);

    return rv;
}




