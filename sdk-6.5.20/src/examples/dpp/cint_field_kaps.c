/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* $ID: cint_field_kaps.c, v 1.2 2016/08/05 skoparan$
 *
 * File: cint_field_kaps.c
 * Purpose: configuration for PMF using KAPS direct table
 *
 * Main function:
 *   - field_direct_kaps_setup
 *
 * Other functions:
 *   - field_direct_kaps_destroy
 */

/* Global variables of the IDs that would be destroyed  */
int nof_entries;
int entry_id[4];

/*
 * Main function to configure a DT group in KAPS
 */
int field_direct_kaps_setup(/* in */ int unit,
                            /* in */ bcm_field_group_t group, /* defines the BB used*/
                            /* in */ int group_priority,
                            /* in */ uint32 statId,
                            /* in */ bcm_port_t in_port,
                            /* in */ uint8 is_64 /* Set for 64 bit result, unset for 32 bit result*/)
{
    bcm_field_group_t grp;
    char *proc_name;

    proc_name = "field_direct_kaps_setup";
    bcm_field_presel_set_t psset;
    int presel_id = 0;
    int presel_flags = 0;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_gport_t local_gport;
    bcm_gport_t snoop_gport;
    bcm_field_group_config_t grp1;
    int auxRes;
    int result = BCM_E_NONE;
    uint32 key_base[4] = {0x0120, /* row 72, zone 0 */
                          0x0121, /* row 72, zone 1 - for 32-bit only*/
                          0x0122, /* row 72, zone 2 */
                          0x0123};/* row 72, zone 3 - for 32-bit only*/

    bcm_field_data_qualifier_t data_qual;
    uint8 data1[2], mask1[2];
    nof_entries = 0;
    int zone; /* counter for the loop */
    int is_jer_plus = 0;

    result = is_jericho_plus_only(unit, &is_jer_plus);
    if (result != BCM_E_NONE) { 
        printf("Failed(%d) is_jericho_plus_only\n", result);
        return result;
    }

    /* Create a data qualifier to control the key */
    result = bcm_field_control_set(unit, bcmFieldControlLargeDirectLuKeyLength, 14);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_control_set with Err %x\n", result);
        return result;
    }

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.qualifier = bcmFieldQualifySrcMac;
    data_qual.length = 14;
    data_qual.offset = 0;

    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create with Err %x\n", result);
        return result;
    }

    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
      presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
      result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create_stage_id\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
          return result;
      }
    } else {
      result = bcm_field_presel_create(unit, &presel_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create\n");
          auxRes = bcm_field_presel_destroy(unit, presel_id);
          return result;
      }
    }
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }
    result = bcm_field_qualify_InPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, in_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InPort\n");
        return result;
    }
    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /* group ID */
    grp1.flags = BCM_FIELD_GROUP_CREATE_WITH_ID;
    grp1.group = group;
    printf("%s(): Group ID = %d\n", proc_name, group);

    /* priority */
    grp1.priority = group_priority;
    printf("%s(): Group priority = %d\n", proc_name, group_priority);

    /* Qualifiers Set */
    BCM_FIELD_QSET_INIT(grp1.qset);
    BCM_FIELD_QSET_ADD(grp1.qset, bcmFieldQualifyStageIngress);
    result = bcm_field_qset_data_qualifier_add(unit, &grp1.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("bcm_field_qset_data_qualifier_add %x\n", result);
        return result;
    }

    /* Actions Set */
    grp1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_INIT(grp1.aset);
    BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionStat); /* 18-bit action for Jericho*/
    if(is_64) { /* Create ASET > 32 bits */
        BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionDrop); /* 19-bit action, total = 37*/
        if (is_jer_plus)
        {
            BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionSystemHeaderSet); /* for JR+ drop action is 1 bit */
        }
    }

    /* Make the field group use a direct table */
    grp1.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp1.mode = bcmFieldGroupModeDirect;
    printf("%s(): Group mode = Direct\n", proc_name);

    /* Make the direct table to be located in KAPS, by using LARGE group*/
    grp1.flags |= BCM_FIELD_GROUP_CREATE_LARGE;
    printf("%s(): Group size = LARGE\n", proc_name);

    /* Make the field group use preselset */
    grp1.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp1.preselset = psset;

    result = bcm_field_group_config_create(unit, &grp1);
    if (BCM_E_NONE != result) {
        printf("%s(): Error in bcm_field_group_config_create\n", proc_name);
        return result;
    }
    printf("%s(): Group created.\n", proc_name);

    /* Create entries */
    for (zone = 3-is_64; zone >= 0; zone -= 1 + is_64 /* use zones 0,1,2,3 for 32-bit and 0,2 for 64-bit*/ ) {
        result = bcm_field_entry_create(unit, grp1.group, &ent);
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_entry_create\n", proc_name);
            auxRes = bcm_field_entry_destroy(unit, ent);
            auxRes = bcm_field_group_destroy(unit, grp);
            return result;
        }
        printf("%s(): Entry %d created.\n", proc_name, ent);
        entry_id[nof_entries++] = ent; /* Save in the global variable for destroy purposes*/
        /* Fill the key and mask for the data qualifier. Last 2 bits show the zone within row. */
        data1[0] = ((key_base[zone] >> 8) % 0x100) & 0x3F; /* Bits 13 to 8 */
        data1[1] = (key_base[zone] % 0x100);        /* Bits 7 to 0 */
        mask1[0] = 0x3F;
        mask1[1] = 0xFF;
        result = bcm_field_qualify_data(unit, ent, data_qual.qual_id, &data1, &mask1, 2);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_qualify_data\n");
            return result;
        }
        printf("%s(): Qualifier added to entry.\n", proc_name);

        BCM_GPORT_LOCAL_SET(local_gport, 15);
        result = bcm_field_action_add(unit, ent, bcmFieldActionStat, statId, local_gport);
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_action_add\n", proc_name);
            auxRes = bcm_field_entry_destroy(unit,ent);
            auxRes = bcm_field_group_destroy(unit, grp);
            return result;
        }

        if(is_64) {
            result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);

            if (is_jer_plus)
            {
                result |= bcm_field_action_add(unit, ent, bcmFieldActionSystemHeaderSet, bcmFieldSystemHeaderPphEei, 0xB00000);
            }

            if (BCM_E_NONE != result) {
                printf("%s(): Error in bcm_field_action_add\n", proc_name);
                auxRes = bcm_field_entry_destroy(unit,ent);
                auxRes = bcm_field_group_destroy(unit, grp);
                return result;
            }
        }
        printf("%s(): Action(s) added to entry %d.\n", proc_name, ent);

        result = bcm_field_entry_install(unit, ent);
        if (BCM_E_NONE != result) {
            printf("%s(): Error in bcm_field_entry_install\n", proc_name);
            auxRes = bcm_field_group_destroy(unit, grp);
            return result;
        }
        printf("%s(): Entry %d installed in zone %d.\n", proc_name, ent, zone);
    }

    return result;
}

/*
 * Main function to destroy configured entry and preselector
 * Attention! Field groups in KAPS cannot be destroyed.
 */
int field_direct_kaps_destroy (/* in */ int unit) {
    int rv = BCM_E_NONE;
    char *proc_name;
    int i;

    proc_name = "field_direct_kaps_destroy";
    for (i = 0; i < nof_entries; i++) {
        rv = bcm_field_entry_destroy(unit, entry_id[i]);
        if (rv != BCM_E_NONE) {
            printf("%s():Error in function bcm_field_entry_destroy(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}


/*
 * For large direct lookup, it is user's responsibility to manage the resources in the KAPS DB. As a result,
 * it is important to understand the construction of the KAPS DB.
 *
 * When presented in large direct lookup, KAPS DB is organized as that direct lookup key bits [n:2] is
 * used to index the KAPS entry, and direct lookup key bits [1:0] is used to further index the one of the 4
 * zones in the KAPS entry:
 *
 *        key = 0/1/2/3
 *      --------------------------> <zone 3/key=3><zone 2/key=2><zone 1/key=1><zone 0/key=0>            <-- KAPS entry 0
 *        key = 4/5/6/7
 *      --------------------------> <zone 3/key=7><zone 2/key=6><zone 1/key=5><zone 0/key=4>            <-- KAPS entry 1
 *                                                 ...
 *        key = 4n/4n+1/4n+2/4n+3
 *      --------------------------> <zone 3/key=4n+3><zone 2/key=4n+2><zone 1/key=4n+1><zone 0/key=4n>  <-- KAPS entry n
 *
 * For JR, the 4 zones are:
 *     <zone 3/32bits><zone 2/32bits><zone 1/32bits><zone 0/32bits>
 * For QAX, the 4 zones are:
 *     <zone 3/31bits><zone 2/32bits><zone 1/32bits><zone 0/32bits>
 * For QUX, the 4 zones are:
 *     <zone 3/24bits><zone 2/32bits><zone 1/32bits><zone 0/32bits>
 * For JR+, the 4 zones are:
 *     <zone 3/30bits><zone 2/30bits><zone 1/30bits><zone 0/30bits>
 * 
 * PMF has two accesses to the KAPS DB, each returns up to two zones (depends on the 2-LSBs of the key) in the KAPS entry.
 * An action can be positioned in one zone only, and could not be positioned across two zones. Multiple actions can be positioned
 * in up to two zones. If the actions of a large direct lookup DB takes two zones, then the LSB of the lookup key must be 0.
 *
 * Multiple programs can share the KAPS direct lookup DB. The KAPS direct lookup DB can be partioned between the logical DBs 
 * by adding constant values to the lookup key. The constant values can be prepended to the MSB of the key, or appended to the LSB
 * of the key. For example, assuming large direct lookup key length is 14, and two programs are sharing the KAPS DB for large
 * direct lookup:
 *     Program 0:
 *         key = {1'b0, 13'bqualifier_x}
 *     Program 1:
 *         key = {1'b1, 13'bqualifier_y}
 *
 * Logical DBs can be partioned by adding constant values to the LSB as well. This is useful in some cases, for example, in QUX, if 
 * a large direct lookup DB has actions up to 64 bits, then it can take zone 0/1, but not zone 2/3 (since zone 3 only has 24 bits).
 * In such a case, the key of the two programs above can be constructed as follows:
 *     Program 0:
 *         key = {12'bqualifier_x, 2'b00}
 *     Program 1:
 *         key = {12'bqualifier_y, 2'b10}
 *
 * If multiple groups share the KAPS direct lookup DB, then the keys of those groups must not be overlapped.
 *
 * Specifically for QUX:
 *     SDK automatically pads two bits zero at key position[13:12].
 *
 * This example demonstrates how to create large direct lookup field group, using the same qualifier, with actions spread across
 * all 4 action zones. It creates two field groups, with 1st group being:
 *     {src_mac[11:0], 2'b00} => bcmFieldActionStat/bcmFieldActionRedirect
 * And 2nd group being:
 *     {src_mac[11:0], 1'b1, 1'b0} => bcmFieldActionSrcGportNew/bcmFieldActionPolicerLevel0
 */
int field_direct_kaps_multi_setup(/* in */ int unit, /* in */ int priority)
{
    bcm_field_group_config_t grp1;
    bcm_field_group_config_t grp2;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    bcm_gport_t dst_gport;
    bcm_field_data_qualifier_t data_qual[4]; /* the 5 data qualifiers */
    uint32 dq_ndx, dq_length[4] = {2, 1, 1, 12};
    bcm_field_qualify_t dq_qualifier[5] = {bcmFieldQualifyConstantZero, bcmFieldQualifyConstantZero, bcmFieldQualifyConstantOne, bcmFieldQualifySrcMac};
    uint8 data[2], mask[2];
    int rv = BCM_E_NONE;

    /* Create a data qualifier to control the key */
    rv = bcm_field_control_set(unit, bcmFieldControlLargeDirectLuKeyLength, 14);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_control_set with Err %x\n", rv);
        return rv;
    }

    for (dq_ndx = 0; dq_ndx < 4; dq_ndx++) {
        bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
        data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
        data_qual[dq_ndx].offset = 0;  
        data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
        data_qual[dq_ndx].length = dq_length[dq_ndx]; 
        rv = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_data_qualifier_create\n");
            return rv;
        }
    }

    /*
    * Group 1:
    *     key = {src_mac[11:0], 2'b00}
    *     action = bcmFieldActionStat/bcmFieldActionRedirect
    */
    bcm_field_group_config_t_init(&grp1);
    grp1.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_LARGE | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp1.mode = bcmFieldGroupModeDirect;
    grp1.priority = priority;

    BCM_FIELD_QSET_INIT(grp1.qset);
    BCM_FIELD_QSET_ADD(grp1.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp1.qset, data_qual[0].qual_id);
    if (BCM_E_NONE != rv) {
        printf("bcm_field_qset_data_qualifier_add %x\n", rv);
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp1.qset, data_qual[3].qual_id);
    if (BCM_E_NONE != rv) {
        printf("bcm_field_qset_data_qualifier_add %x\n", rv);
        return rv;
    }

    BCM_FIELD_ASET_INIT(grp1.aset);
    BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionStat);
    BCM_FIELD_ASET_ADD(grp1.aset, bcmFieldActionRedirect);

    rv = bcm_field_group_config_create(unit, &grp1);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    grp = grp1.group;

    /*
    * Entry:
    *     if src_mac[11:0] == 0x555 then counter pointer = 0x555/redirect to port 202
    */
    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) {
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, grp);
        return result;
    }

    data[0] = 5;
    data[1] = 0x55;
    mask[0] = 0xF;
    mask[1] = 0xFF;
    rv = bcm_field_qualify_data(unit, ent, data_qual[3].qual_id, &data, &mask, 2);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_data\n");
        return rv;
    }

    /* must be two-bits 0 for two-bits constant-zero qualifier */
    data[0] = 0;
    mask[0] = 0x3;
    rv = bcm_field_qualify_data(unit, ent, data_qual[0].qual_id, &data, &mask, 1);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_data\n");
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionStat, 0x555, 0);
    if (BCM_E_NONE != rv) {
        bcm_field_entry_destroy(unit,ent);
        bcm_field_group_destroy(unit, grp);
        return rv;
    }

    BCM_GPORT_LOCAL_SET(dst_gport, 202);

    rv = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, dst_gport);
    if (BCM_E_NONE != rv) {
        bcm_field_entry_destroy(unit,ent);
        bcm_field_group_destroy(unit, grp);
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != rv) {
        bcm_field_group_destroy(unit, grp);
        return rv;
    }

    /*
    * Group 2:
    *     key = {src_mac[11:0], 1'b1, 1'b0}
    *     action = bcmFieldActionSrcGportNew/bcmFieldActionPolicerLevel0
    */
    bcm_field_group_config_t_init(&grp2);
    grp2.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_LARGE | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp2.mode = bcmFieldGroupModeDirect;
    grp2.priority = priority + 1;

    BCM_FIELD_QSET_INIT(grp2.qset);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_qset_data_qualifier_add(unit, &grp2.qset, data_qual[1].qual_id);
    if (BCM_E_NONE != rv) {
        printf("bcm_field_qset_data_qualifier_add %x\n", rv);
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp2.qset, data_qual[2].qual_id);
    if (BCM_E_NONE != rv) {
        printf("bcm_field_qset_data_qualifier_add %x\n", rv);
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp2.qset, data_qual[3].qual_id);
    if (BCM_E_NONE != rv) {
        printf("bcm_field_qset_data_qualifier_add %x\n", rv);
        return rv;
    }

    BCM_FIELD_ASET_INIT(grp2.aset);
    BCM_FIELD_ASET_ADD(grp2.aset, bcmFieldActionSrcGportNew);
    BCM_FIELD_ASET_ADD(grp2.aset, bcmFieldActionPolicerLevel0);

    rv = bcm_field_group_config_create(unit, &grp2);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    grp = grp2.group;

    /*
    * Entry:
    *     if src_mac[11:0] == 0x555 then source system port = 202/meter 0 = 0xAAA
    */
    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) {
        bcm_field_entry_destroy(unit, ent);
        bcm_field_group_destroy(unit, grp);
        return result;
    }

    data[0] = 5;
    data[1] = 0x55;
    mask[0] = 0xF;
    mask[1] = 0xFF;
    rv = bcm_field_qualify_data(unit, ent, data_qual[3].qual_id, &data, &mask, 2);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_data\n");
        return rv;
    }

    /*
    * Value must match the value of the constant qualifier.
    */
    data[0] = 0;
    mask[0] = 0x1;
    rv = bcm_field_qualify_data(unit, ent, data_qual[1].qual_id, &data, &mask, 1);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_data\n");
        return rv;
    }

    data[0] = 1;
    mask[0] = 0x1;
    rv = bcm_field_qualify_data(unit, ent, data_qual[2].qual_id, &data, &mask, 1);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_data\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(dst_gport, 202);
    rv = bcm_field_action_add(unit, ent, bcmFieldActionSrcGportNew, dst_gport, 0);
    if (BCM_E_NONE != rv) {
        bcm_field_entry_destroy(unit,ent);
        bcm_field_group_destroy(unit, grp);
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionPolicerLevel0, 0xAAA, 0);
    if (BCM_E_NONE != rv) {
        bcm_field_entry_destroy(unit,ent);
        bcm_field_group_destroy(unit, grp);
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != rv) {
        bcm_field_group_destroy(unit, grp);
        return rv;
    }

    return rv;
}

