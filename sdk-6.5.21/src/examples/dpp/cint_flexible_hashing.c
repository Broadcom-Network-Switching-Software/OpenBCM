/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

/**
 * This CINT details an example of flexible hashing. 
 * Flexible hashing utilizes the key construction of stateful-load-balancing 
 * without learning or looking-up the keys. Once a key has been 
 * constructed in the Flexible-Hashing, it can either be used as the 
 * ECMP-LB-Key (Jericho and above only) or as an additional 
 * input for the configured Load-Balancing. 
 *  
 * DETAILED FLOW: 
 * 1. Setup Flexible-Hashing hash key generation (see slb_field_group_example 
 * in cint_stateful_load_balancing): 
 * a. Define a program to create 256 bit FP-LB-Vector. 
 * b. This vector will be hashed by Flexible-Hashing dedicated HW to 
 * genereate 160 bit Intermediate-Flow-Label. 
 * c. Define a program to generate a 74 bit 
 * Flow-Label-Destination key. 
 *  
 * 2. Two options are available for ECMP-LB-Key generation: 
 *  
 * a. Jericho and above only- When ECMP groups are configured 
 * with RESILANT_HASHING mode, use the 47 lsb of the 
 * Intermediate-Flow-Label (Flow-Label-Key) as input for the 
 * ECMP-LB-Key. When Hirarchical ECMP is used, the 24 msb are 
 * used as ECMP-LB-Key0 and the 24 lsb are used as ECMP-LB-Key1.
 *  
 * b. Copy the Flow-Label-Destination key to the 80 msbs of one 
 * (or more) of the PMF keys in the 2nd cycle. From there, copy 
 * it to be used as additional data in configured Load-Balancing 
 * ECMP hashing which will ouput the ECMP-LB-Key. 
 *  
 * SOC PROPERTIES
 * No soc properties are required.
 *  
 * RUN: 
 * For option 2.a above:
    cd ../../../../src/examples/dpp
    cint cint_utils_l3.c
    cint cint_l3_multipath.c
    cint cint_stateful_load_balancing.c
    cint cint_flexible_hashing.c
    cint
    cint_l3_multipath_cfg.ecmp_is_resilant_hashing = 1;
    print flex_hash_l3_multipath_dvapi(unit, 13, 10, 14, 15);
    cint_stateful_load_balancing_cfg.slb_flow_label_key_only = 1;
 *  print slb_field_group_example(unit); exit;
 
 * For option 2.b above:
    cd ../../../../src/examples/dpp
    cint cint_utils_l3.c
    cint cint_l3_multipath.c
    cint cint_stateful_load_balancing.c
    cint cint_flexible_hashing.c
    cint
    print flex_hash_l3_multipath_dvapi(unit, 13, 10, 14, 15);
 *  print slb_field_group_example(unit); print
 *  setup_flexible_hash_database_in_pmf(unit,0); exit;
*/

/**
 * Setup a database in the PMF to copy the 20 lsbs of the final 
 * Flexible-Hashing key to the PMF ECMP LB key. 
 * ECMP_LAG: 0 for ECMP PMF use, 1 for LAG PMF use
 */ 
int setup_flexible_hash_database_in_pmf(int unit,int ECMP_LAG)
{
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_extraction_field_t ext_qual;
    bcm_field_extraction_action_t ext_act;
    bcm_field_action_t action = (ECMP_LAG == 0) ? bcmFieldActionMultipathHashAdditionalInfo : bcmFieldActionTrunkHashKeySet;
    int group_priority = 33;
    int result;

    bcm_field_group_config_t_init(&grp);
    bcm_field_extraction_field_t_init(&ext_qual);
    bcm_field_extraction_action_t_init(&ext_act);

    grp.group = -1;

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyHashValue);
    /* Define the ASET */
    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, action);

    /* Use direct extraction. */
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_LARGE;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create\n");
        bcm_field_group_destroy(unit, grp.group);
        return result;
    }

    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        bcm_field_group_destroy(unit, grp.group);
        return result;
    }

    /* Build the action to set the PMF ECMP LB key. */
    ext_act.action = action;
    ext_act.bias = 0;

    /* Build the source for the direct extraction to be the 20 lsbs of the Flexible-Hashing value. */
    ext_qual.flags = 0;
    ext_qual.bits = 20;
    ext_qual.lsb = 0;
    ext_qual.qualifier = bcmFieldQualifyHashValue;
    result = bcm_field_direct_extraction_action_add(unit,
            ent,
            ext_act,
            1 /* count */,
            ext_qual);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        bcm_field_group_destroy(unit, grp.group);
        return result;
    } 

    result = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != result) {
        bcm_field_group_destroy(unit, grp.group);
        return result;
    }

    print grp.group;
    return result;
}

/* Depends on cint_l3_multipath.c, ecmp group will be set to resilant hashing if cint_l3_multipath_cfg.ecmp_is_resilant_hashing = 1 */
int flex_hash_l3_multipath_dvapi(int unit, int inPort, int vlan, int outPort0, int outPort1)
{
  int rc;
  int in_port = inPort;
  int nof_paths = 2;
  int out_port_arr[2] = {outPort0,outPort1};
  int modid_arr[2] = {unit,unit};

  rc = l3_multipath_run(unit,-1,inPort,vlan,nof_paths,out_port_arr,modid_arr);
  if (BCM_E_NONE != rc) {
      printf("Error in l3_multipath_run\n");
      return rc;
  }

  rc = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_NONE);
  if (BCM_E_NONE != rc) {
      printf("Error in bcm_switch_control_set\n");
      return rc;
  }

  return rc;
}

/*
 * This cint gets two sets of qualifiers (one for the pre-hashing stage and the second for the  post-hashing stage)
 * and create a field group that results in two keys for the hierarchical ECMP or one key in case of a one stage ECMP.
 */
int flexible_field_group_hash_two_keys(const int unit, bcm_field_qualify_t *hash_dq_qualifier, uint32 *hash_dq_length, uint16 *hash_dq_offset, bcm_field_data_qualifier_t *hash_dq_data_qual, uint8 hash_dq_nof, bcm_field_qualify_t *key_dq_qualifier, uint32 *key_dq_length, uint16 *key_dq_offset, bcm_field_data_qualifier_t *key_dq_data_qual, uint8 key_dq_nof, bcm_field_header_format_t preselector_header_format){
    int result, qual_id;
    uint32 fg_ndx, dq_ndx, dq_ndx_max;

    /*Hash and key field groups*/
    bcm_field_group_t group[2] = {22, 23};
    bcm_field_group_config_t group_config[2];

    /*Pre selector*/
    int presel_id = 2; /* ID from 0 to 11 for Advanced mode */
    int presel_flags = 0;
    bcm_field_presel_set_t psset;

    /*
     *1. Create Preselector
     */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageHash, presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return result;
        }
    } else {
        result = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_presel_create\n");
            return result;
        }
    }

    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageHash);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_HeaderFormat(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, preselector_header_format);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_HeaderFormat\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /*
     * 2. Define all the data qualifiers
     */
    for (dq_ndx = 0; dq_ndx < hash_dq_nof; dq_ndx++) {
        bcm_field_data_qualifier_t_init(&hash_dq_data_qual[dq_ndx]);
        hash_dq_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
        hash_dq_data_qual[dq_ndx].offset = hash_dq_offset[dq_ndx];
        hash_dq_data_qual[dq_ndx].qualifier = hash_dq_qualifier[dq_ndx];
        hash_dq_data_qual[dq_ndx].length = hash_dq_length[dq_ndx];
        hash_dq_data_qual[dq_ndx].stage = bcmFieldStageHash;
        result = bcm_field_data_qualifier_create(unit, &hash_dq_data_qual[dq_ndx]);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_data_qualifier_create %d\n", dq_ndx);
            return result;
        }
    }
    for (dq_ndx = 0; dq_ndx < key_dq_nof; dq_ndx++) {
        bcm_field_data_qualifier_t_init(&key_dq_data_qual[dq_ndx]);
        key_dq_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
        key_dq_data_qual[dq_ndx].offset = key_dq_offset[dq_ndx];
        key_dq_data_qual[dq_ndx].qualifier = key_dq_qualifier[dq_ndx];
        key_dq_data_qual[dq_ndx].length = key_dq_length[dq_ndx];
        key_dq_data_qual[dq_ndx].stage = bcmFieldStageHash;
        result = bcm_field_data_qualifier_create(unit, &key_dq_data_qual[dq_ndx]);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_data_qualifier_create %d\n", dq_ndx);
            return result;
        }
    }

    /*
     * 3. Create the field groups
     */
    for (fg_ndx = 0; fg_ndx < 2; fg_ndx++) {
        bcm_field_group_config_t_init(&group_config[fg_ndx]);
        group_config[fg_ndx].group = group[fg_ndx];
        group_config[fg_ndx].priority = BCM_FIELD_GROUP_PRIO_ANY;

        /*
         * Define the QSET
         */
        BCM_FIELD_QSET_INIT(group_config[fg_ndx].qset);
        BCM_FIELD_QSET_ADD(group_config[fg_ndx].qset, bcmFieldQualifyStageHash);
        dq_ndx_max = (fg_ndx == 0 ? hash_dq_nof : key_dq_nof);
        for (dq_ndx = 0; dq_ndx < dq_ndx_max; dq_ndx++) {
            qual_id = (fg_ndx == 0 ? hash_dq_data_qual[dq_ndx].qual_id : key_dq_data_qual[dq_ndx].qual_id);
            /* Add the Data qualifier to the QSET */
            result = bcm_field_qset_data_qualifier_add(unit, &group_config[fg_ndx].qset, qual_id);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_qset_data_qualifier_add DQ %d FG %d\n", dq_ndx, fg_ndx);
                return result;
            }
        }

        /*
         *  Define the ASET - use counter 0.
         */
        BCM_FIELD_ASET_INIT(group_config[fg_ndx].aset);
        BCM_FIELD_ASET_ADD(group_config[fg_ndx].aset, (fg_ndx == 0 ? bcmFieldActionHashValueSet : bcmFieldActionTrunkHashKeySet));

        /*
         *  Create the field group with direct extraction
         */
        group_config[fg_ndx].flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ASET;
        group_config[fg_ndx].mode = bcmFieldGroupModeHashing;
        group_config[fg_ndx].preselset = psset;
        result = bcm_field_group_config_create(unit, &group_config[fg_ndx]);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_group_create\n");
            return result;
        }
    }

    return BCM_E_NONE;
}

/*
 *  Hash the packet sip and create two different ecmp keys out if it.
 */
int flexible_two_keys_sip(const int unit , int bcm_hash_config)
{
    int result;
    bcm_field_header_format_t preselector_header_format = bcmFieldHeaderFormatIp4AnyL2L3;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 1;
    bcm_field_qualify_t hash_dq_qualifier[1];
    uint32 hash_dq_length[1];
    uint16 hash_dq_offset[1];
    bcm_field_data_qualifier_t hash_dq_data_qual[1];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 4;
    bcm_field_qualify_t key_dq_qualifier[4];
    uint32 key_dq_length[4];
    uint16 key_dq_offset[4];
    bcm_field_data_qualifier_t key_dq_data_qual[4];

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifySrcIp;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 0;

    /*Build Key, 24 msb are key0, 23 lsb are key1*/
    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 16; /*CRC-D*/
    key_dq_offset[0] = 0;
    key_dq_qualifier[1] = bcmFieldQualifyConstantZero;
    key_dq_length[1] = 7;
    key_dq_offset[1] = 0;
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 16; /*CRC-C*/
    key_dq_offset[2] = 16;
    key_dq_qualifier[3] = bcmFieldQualifyConstantZero;
    key_dq_length[3] = 8;
    key_dq_offset[3] = 0;

    result = flexible_field_group_hash_two_keys(unit, hash_dq_qualifier, hash_dq_length, hash_dq_offset, hash_dq_data_qual, hash_dq_nof, key_dq_qualifier, key_dq_length, key_dq_offset, key_dq_data_qual, key_dq_nof, preselector_header_format);
    if (BCM_E_NONE != result) {
        printf("Error in flexible_field_group_hash_two_keys\n");
        return result;
    }

    /*Set final hash function for the two created keys*/
    result = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, bcm_hash_config);
    if (result != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", result);
    }

    return result;
}


/*
 *  Hash the packet sip and sport and create two different ecmp keys out if it.
 *  The first level ECMP will resolve based on Source IP hash, the second level
 *  ECMP will resolve based on the Source Port hash.
 */
int flexible_two_keys_sport_sip(const int unit , int bcm_hash_config)
{
    int result;
    bcm_field_header_format_t preselector_header_format = bcmFieldHeaderFormatIp4AnyL2L3;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 1;
    bcm_field_qualify_t hash_dq_qualifier[1];
    uint32 hash_dq_length[1];
    uint16 hash_dq_offset[1];
    bcm_field_data_qualifier_t hash_dq_data_qual[1];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 4;
    bcm_field_qualify_t key_dq_qualifier[4];
    uint32 key_dq_length[4];
    uint16 key_dq_offset[4];
    bcm_field_data_qualifier_t key_dq_data_qual[4];

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifySrcIp;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 0;

    /*Build Key, 24 msb are key1, 23 lsb are key0*/
    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 16; /*CRC-D*/
    key_dq_offset[0] = 0;
    key_dq_qualifier[1] = bcmFieldQualifyConstantZero;
    key_dq_length[1] = 8;
    key_dq_offset[1] = 0;
    key_dq_qualifier[2] = bcmFieldQualifyL4SrcPort;
    key_dq_length[2] = 16;
    key_dq_offset[2] = 0;
    key_dq_qualifier[3] = bcmFieldQualifyConstantZero;
    key_dq_length[3] = 7;
    key_dq_offset[3] = 0;

    result = flexible_field_group_hash_two_keys(unit, hash_dq_qualifier, hash_dq_length, hash_dq_offset, hash_dq_data_qual, hash_dq_nof, key_dq_qualifier, key_dq_length, key_dq_offset, key_dq_data_qual, key_dq_nof, preselector_header_format);
    if (BCM_E_NONE != result) {
        printf("Error in flexible_field_group_hash_two_keys\n");
        return result;
    }

    /*Set final hash function for the two created keys*/
    result = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, bcm_hash_config);
    if (result != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", result);
    }

    /*Set final hash function for the two created keys*/
    result = bcm_switch_control_set(unit, bcmSwitchECMPHashOffset, 14);
    if (result != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashOffset failed: %d \n", result);
    }

    return result;
}


/*
 * This cint checks the two options to use the UDP field through two different pre selector in the flexible LB.
 * It create two pre-selectors (L2 and L3 forward) that received packets that contain UDP.
 * The L2 forward uses the UDP port and the SIP for the LAG hash and the L3 forward program uses only the SIP for the LAG hash.
 * This cint also creates L3 interface for L3 forwards and an L2 entry that both are using the same LAG (trunk).
 */
int flexible_IP_L2_fwrd_with_UDP(int unit, int inPort, int outPort0, int outPort1, int outPort2) {
    int group_priority_pre_hash = 22;
    bcm_field_group_t group_pre_hash= 22;
    int group_priority_post_hash= 23;
    bcm_field_group_t group_post_hash= 23;
    int result = BCM_E_NONE ;
    uint32 fg_ndx, dq_ndx, nof_dqs, nof_fgs, dq_min[2], dq_max[2], dq_length[8];
    uint32 DIP = 0x37373737;
    uint16 dq_offset[8];
    bcm_field_group_config_t grp[2];
    bcm_field_data_qualifier_t data_qual[8];
    bcm_field_qualify_t dq_qualifier[8];
    bcm_field_IpProtocolCommon_t protocolGet;
    int presel_id,i;
    int presel_flags = 0;
    bcm_field_presel_set_t psset;
    int L3_config = 0;
    int L2_config = 1;
    int index = 0;
    bcm_l3_intf_t intf;
    bcm_mac_t my_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
    bcm_mac_t out_mac         = {0x00, 0x45, 0x45, 0x45, 0x45, 0x00};
    int vrf = 100;
    bcm_l3_egress_t l3eg;
    bcm_l3_host_t l3host;
    int fecId;
    int outPorts[3];
    bcm_trunk_t trunk_id;
    bcm_trunk_member_t member;
    bcm_l3_ingress_t ingress_intf;
    bcm_gport_t trunk_gport;


    outPorts[0] = outPort0;
    outPorts[1] = outPort1;
    outPorts[2] = outPort2;

    bcm_field_header_format_t  header_format[2]  = {bcmFieldHeaderFormatIp4, bcmFieldHeaderFormatIp4};
    bcm_field_ForwardingType_t ForwardingType[2] = {bcmFieldForwardingTypeIp4Ucast, bcmFieldForwardingTypeL2};


    for(i = 0; i < 2; i++) {

        nof_fgs = 2;
        dq_min[0] = 0;/*min data-qualifier index for pre*/
        index = 0;
        presel_id = i; /* Unique ID for Advanced mode */


        if (i == L3_config) {
            /* Pre data qualifiers*/
            dq_qualifier[index] = bcmFieldQualifySrcIp;
            dq_length[index] = 32;
            dq_offset[index] = 0;
            dq_max[0] = index;/*max data-qualifier index for pre*/
            index++;
            dq_min[1] = index;/*min data-qualifier index for post*/
        } else {
            /* Pre data qualifiers*/
            dq_qualifier[index] = bcmFieldQualifySrcIp;
            dq_length[index] = 32;
            dq_offset[index] = 0;
            index++;
            dq_qualifier[index] = bcmFieldQualifyL4SrcPort;
            dq_length[index] = 16;
            dq_offset[index] = 0;
            dq_max[0] = index;/*max data-qualifier index for pre*/
            index++;
            dq_min[1] = index;/*min data-qualifier index for post*/
        }


        /* Post data qualifiers*/
        dq_qualifier[index] = bcmFieldQualifyHashValue;
        dq_length[index] = 32;
        dq_offset[index] = 0;
        index++;
        dq_qualifier[index] = bcmFieldQualifyHashValue;
        dq_length[index] = 32;
        dq_offset[index] = 32;
        index++;
        dq_qualifier[index] = bcmFieldQualifyHashValue;
        dq_length[index] = 10;
        dq_offset[index] = 64;
        dq_max[1] = index;/*max data-qualifier index for post*/
        index++;
        nof_dqs = index;

        /*
        * 1. Create the Preselector on IPv4 packets
        */
        /* Create a presel entity */
        if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH;
            result = bcm_field_presel_create_stage_id(unit, bcmFieldStageHash, presel_id);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_stage_id\n");
                return result;
            }
        } else {
            result = bcm_field_presel_create(unit, &presel_id);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_presel_create_id\n");
                return result;
            }
        }

        result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageHash);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_Stage\n");
            return result;
        }

        result = bcm_field_qualify_HeaderFormat(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, header_format[i]);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_HeaderFormat\n");
            return result;
        }

        result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, ForwardingType[i]);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_ForwardingType\n");
            return result;
        }

        result = bcm_field_qualify_IpProtocolCommon(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldIpProtocolCommonUdp);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_IpProtocolCommon\n");
            return result;
        }




        result = bcm_field_qualify_IpProtocolCommon_get(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, &protocolGet);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_IpProtocolCommon_get\n");
            return result;
        }

        if(bcmFieldIpProtocolCommonUdp != protocolGet) {
            printf("Wrong protocol was received for IpProtocolCommon!!\n");
            return BCM_E_PARAM;
        }


        BCM_FIELD_PRESEL_INIT(psset);
        BCM_FIELD_PRESEL_ADD(psset, presel_id);

        /*
        * 2. Define all the data qualifiers
        */
        for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
            bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]);
            data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
            data_qual[dq_ndx].offset = dq_offset[dq_ndx];
            data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx];
            data_qual[dq_ndx].length = dq_length[dq_ndx];
            data_qual[dq_ndx].stage = bcmFieldStageHash;
            result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_data_qualifier_create %d\n", dq_ndx);
                return result;
            }
        }

        /*
        * 3. Create the pre and post hashing Field groups
        */
        for (fg_ndx = 0; fg_ndx < nof_fgs; fg_ndx++) {
            bcm_field_group_config_t_init(&grp[fg_ndx]);
            grp[fg_ndx].group = (fg_ndx == 0)? group_pre_hash: group_post_hash;
            grp[fg_ndx].priority = (fg_ndx == 0)? group_priority_pre_hash: group_priority_post_hash;

            /*
            * Define the QSET
            */
            BCM_FIELD_QSET_INIT(grp[fg_ndx].qset);
            BCM_FIELD_QSET_ADD(grp[fg_ndx].qset, bcmFieldQualifyStageHash);
            for (dq_ndx = dq_min[fg_ndx]; dq_ndx <= dq_max[fg_ndx]; dq_ndx++) {
                /* Add the Data qualifier to the QSET */
                result = bcm_field_qset_data_qualifier_add(unit, &grp[fg_ndx].qset, data_qual[dq_ndx].qual_id);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qset_data_qualifier_add DQ %d FG %d\n", dq_ndx, fg_ndx);
                    return result;
                }
            }

            /*
            * Define the ASET - use counter 0.
            */
            BCM_FIELD_ASET_INIT(grp[fg_ndx].aset);
            if (fg_ndx == 0) {
                BCM_FIELD_ASET_ADD(grp[fg_ndx].aset, bcmFieldActionHashValueSet);
            }
            else {
                BCM_FIELD_ASET_ADD(grp[fg_ndx].aset, bcmFieldActionTrunkHashKeySet);
            }

            /* Create the Field group with type Direct Extraction */
            grp[fg_ndx].flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ASET;
            grp[fg_ndx].mode = bcmFieldGroupModeHashing;
            grp[fg_ndx].preselset = psset;
            result = bcm_field_group_config_create(unit, &grp[fg_ndx]);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_group_create\n");
                return result;
            }
        }

        group_priority_pre_hash = 24;
        group_pre_hash = 24;
        group_priority_post_hash = 25;
        group_post_hash = 25;
    }


    /* create trunk */
    result = bcm_trunk_create(unit, 0, &trunk_id);
    if (result != BCM_E_NONE) {
        printf ("bcm_trunk_create failed: %d \n", result);
        return result;
    }


    result = bcm_trunk_psc_set(unit, trunk_id, BCM_TRUNK_PSC_DYNAMIC_RESILIENT);
    if (result != BCM_E_NONE) {
        printf ("bcm_trunk_psc_set(%d, %d, BCM_TRUNK_PSC_DYNAMIC_RESILIENT);\n", unit, trunk_id);
        return result;
    }


    /* add ports to trunk */
    for (i = 0; i < 3; i++) {
        bcm_trunk_member_t_init(&member);
        BCM_GPORT_LOCAL_SET(member.gport, outPorts[i]); /* phy port to local port */
        result = bcm_trunk_member_add(unit, trunk_id, &member);

        printf("gport = %d, outPorts = %d, i= %d,\n",member.gport,outPorts[i],i);

        if (result != BCM_E_NONE) {
            printf ("bcm_trunk_member_add with port %d failed: %d \n", outPorts[i], result);
            return result;
        }
    }

    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id); /* create a trunk gport  instead of the dest-port */



    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, out_mac, 6);

    l3eg.vlan   = 401;
    l3eg.port   = trunk_gport;
    l3eg.flags  = BCM_L3_EGRESS_ONLY;
    result = bcm_l3_egress_create(unit, 0, &l3eg, &fecId);
    if (result != BCM_E_NONE) {
        printf ("bcm_l3_egress_create failed \n");
        return result;
    }

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = DIP;
    l3host.l3a_vrf     = vrf;
    l3host.l3a_intf    = fecId;

    result = bcm_l3_host_add(unit, &l3host);
    if (result != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", result);
        return result;
    }

    setup_flexible_hash_database_in_pmf(unit,1 /*LAG*/);

    /* create L3 interface */
    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, my_mac_address, 6);
    intf.l3a_vid = vrf;
    intf.l3a_vrf = vrf;
    result = bcm_l3_intf_create(unit, intf);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return result;
    }

    /* create ingress */
    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_intf.vrf= vrf;
    result = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
    }

    /* create L2 entry */
    my_mac_address[5]++;
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, my_mac_address, vrf);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = trunk_gport;
    bcm_l2_addr_add(unit, &l2_addr);

    return result;
}

/*
 * This function creates a program that would use the destination port in the flow label destination vector.
 * It matches on IPv4oETH packets.
 * In order for it to work, it needs to be sourced along with cint_advanced_flb_hashing.c file.
 */
int program_ipv4oeth_dst_port(int unit)
{
    int rv = BCM_E_NONE;
    int qual_ids[4];

    /*Hash and key field groups*/
    bcm_field_group_t group_id[2] = {41, 42};
    bcm_field_group_config_t group_config[2];

    /*Pre selector*/
    int presel_id = 2; /* ID from 0 to 11 for Advanced mode */
    int presel_flags = 0;
    bcm_field_presel_set_t psset;
    bcm_field_header_format_t preselector_header_format = bcmFieldHeaderFormatIp4;
    int dq_ndx;
    uint32 flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 4;
    bcm_field_qualify_t hash_dq_qualifier[4];
    uint32 hash_dq_length[4];
    uint16 hash_dq_offset[4];
    int hash_offset_base[4] = {0,0,0,0};
    bcm_field_data_qualifier_t hash_dq_data_qual[4];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 4;
    bcm_field_qualify_t key_dq_qualifier[4];
    uint32 key_dq_length[4];
    uint16 key_dq_offset[4] = {0, 0, 0, 0};
    int key_offset_base[4] ={0,0,0,0};
    bcm_field_data_qualifier_t key_dq_data_qual[4];

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifyDstIp;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 0;
    hash_dq_qualifier[1] = bcmFieldQualifySrcIp;
    hash_dq_length[1] = 32;
    hash_dq_offset[1] = 0;
    hash_dq_qualifier[2] = bcmFieldQualifySrcMac;
    hash_dq_length[2] = 32;
    hash_dq_offset[2] = 0;
    hash_dq_qualifier[3] = bcmFieldQualifyDstMac;
    hash_dq_length[3] = 32;
    hash_dq_offset[3] = 0;

    key_dq_qualifier[0] = bcmFieldQualifyDstPort;
    key_dq_length[0] = 16;
    key_dq_offset[0] = 0;
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 32;
    key_dq_offset[1] = 32; /*CRC-A + CRC-B*/
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 16;
    key_dq_offset[2] = 0; /*CRC-D*/
    key_dq_qualifier[3] = bcmFieldQualifyHashValue;
    key_dq_length[3] = 10;
    key_dq_offset[3] = 16; /*CRC-C*/

    rv = field_preselector_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_extension_create - (%d)\n", rv);
        return rv;
    }
    /*
     * 2. Define all the data qualifiers
     */
    rv = field_data_qualifier_array_create(unit, hash_dq_nof, hash_dq_qualifier, hash_dq_offset, hash_offset_base, hash_dq_length,
                flags, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    rv = field_data_qualifier_array_create(unit, key_dq_nof, key_dq_qualifier, key_dq_offset, key_offset_base, key_dq_length,
                flags, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }
    return rv;
}

