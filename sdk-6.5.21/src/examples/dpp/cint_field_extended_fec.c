

/* $Id: cint_field_extended_fec.c 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Configuration only for Jericho+ with KBP.
 *
 * Purpose - increases the number of FECs by using PMF Large-Direct DB
 * This CINT crates a Direct Look-up field group, 
 * using a KBP returned FEC value(17 bits) in the 1st pass and Hash value(2 bits) in the second pass, as a key.
 * The 19 bit key [17 FEC:2 HASH] does the look-up in the KAPS and use the whole KAPS.
 *
 * SOC Properties:
 * private_ip_frwrd_table_size=0
 * public_ip_frwrd_table_size=0
 * pmf_kaps_large_db_size=32000
 */


    int unit = 0;
    bcm_error_t result = BCM_E_NONE ;
    bcm_field_group_config_t grp;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_data_qualifier_t data_qual[2];
    uint8 data[3];
    uint8 mask[3];

    bcm_field_control_t field_control;

    /* We set the key size to the max(19 bits), because its 14 by default */
    field_control = bcmFieldControlLargeDirectLuKeyLength;
    bcm_field_control_set(unit,field_control,19);

    bcm_field_group_config_t_init(&grp);
    grp.group = 5;

    /* 
    * Define the QSET - use FEC and HASH values as qualifiers. 
    */

    bcm_field_data_qualifier_t_init(&data_qual[0]);
    /* Qual0 - FEC from KBP */
    data_qual[0].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual[0].offset = 0;
    data_qual[0].stage = bcmFieldStageIngress;
    data_qual[0].qualifier = bcmFieldQualifyExternalValue0;
    data_qual[0].length = 17;
    result = bcm_field_data_qualifier_create(unit, data_qual[0]);   
    if (BCM_FAILURE(result)) 
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create 0 failed. Error:%d (%s)\n"),  
                   result, bcm_errmsg(result)));
        return result;
    }

    bcm_field_data_qualifier_t_init(&data_qual[1]);
    /* Qual1 - Hash value */
    data_qual[1].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual[1].offset = 0;
    data_qual[1].stage = bcmFieldStageIngress;
    data_qual[1].qualifier = bcmFieldQualifyHashValue;
    data_qual[1].length = 2;

    result = bcm_field_data_qualifier_create(unit, data_qual[1]);
    if (BCM_FAILURE(result)) 
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_data_qualifier_create 1 failed. Error:%d (%s)\n"),  
                   result, bcm_errmsg(result)));
        return result;
    }

    bcm_field_qset_data_qualifier_add(unit,grp.qset,data_qual[0].qual_id);
    bcm_field_qset_data_qualifier_add(unit,grp.qset,data_qual[1].qual_id);

    
    /*
    *  Define the ASET - the result of the look-up should be DEST and EEI.
    */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionForward/* action DEST */);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSystemHeaderSet/* action EEI */);

    /*  Create the Field group with type Direct Lookup in KAPS */
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_LARGE ;
    grp.mode = bcmFieldGroupModeDirect;
    
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_FAILURE(result)) 
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_config_create failed. Error:%d (%s)\n"),  
                   result, bcm_errmsg(result)));
        return result;
    }


    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp.group, aset);
    if (BCM_FAILURE(result)) 
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_group_action_set failed. Error:%d (%s)\n"),  
                   result, bcm_errmsg(result)));
        return result;
    }
    
    
    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_FAILURE(result)) 
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "bcm_field_entry_create failed. Error:%d (%s)\n"),  
                   result, bcm_errmsg(result)));
        return result;
    }
   
    /* FEC Data qualifier values should be configured acording to the configured FEC LSBs */
    data[0] = 0x00;
    data[1] = 0xFF;
    data[2] = 0xFF;

    mask[0] = 0x01;
    mask[1] = 0xFF;
    mask[2] = 0xFF;

    result = bcm_field_qualify_data(unit, ent, data_qual[0].qual_id, data, mask, 3); 
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }
    
    /* Hash Data qualifier values should be configured acording to the configured Hash LSBs */
    data[0] = 0x0;
    mask[0] = 0x3;
    result = bcm_field_qualify_data(unit, ent, data_qual[1].qual_id, data, mask, 1);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }

    /* The action values should be configured as the desired system values, current ones are just for testing/example */
    bcm_field_action_add(unit,ent ,bcmFieldActionForward ,0xAACC,0);
    bcm_field_action_add(unit,ent ,bcmFieldActionSystemHeaderSet ,bcmFieldSystemHeaderPphEei,0xffffA);

    bcm_field_group_install(unit,grp.group);
    if (result != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_install\n");
        return result;
    }

    






    

