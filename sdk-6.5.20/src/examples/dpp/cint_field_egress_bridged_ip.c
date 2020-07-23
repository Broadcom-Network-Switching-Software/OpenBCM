/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* $ID: cint_field_egress_bridged_ip.c, v 2.0 2017/01/20 skoparan$
 *
 * File: cint_field_egress_bridged_ip.c
 * Purpose: configuration for egress PMF to qualify on SIP/DIP of a bridged packet
 * 
 * 
 * To qualify on IP header fields when ACL-while-bridging:
 * 1) SOC property:
 *     field_egress_enable_ip_acl_on_bridge=1
 *
 * 2) Use Data qualifiers to extract the needed field, depending on the number of VLAN tags of the packet:
 *
 *           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *           | 0 tags                                 | 1 tag                                 | 2 tags                                | 3 tags                             |
 *           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * SIP - MSB | EgressBridgedForwardingDataHigh[31:0]  | EgressBridgedForwardingDataLow[55:24] | EgressBridgedForwardingDataLow[23:0]  | EgressBridgedAclDataHigh[31:0]     |
 * SIP - LSB |                                        |                                       | EgressBridgedAclDataHigh[39:32]       |                                    |
 * DIP - MSB | EgressBridgedForwardingDataLow[55:24]  | EgressBridgedForwardingDataLow[23:0]  | EgressBridgedAclDataHigh[31:0]        | EgressBridgedAclDataLow[31:0]      |
 * DIP - LSB |                                        | EgressBridgedAclDataHigh[39:32]       |                                       |                                    |
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The example provided here performs drop of packets matching given SIP+DIP, and count on IPv4oEth0 packets matching SIP+DIP.
 * The desired number of VLAN tags for the configuration is input to the function as nof_tags.
 *
 * Main function:
 *   - field_setup
 *
 * Other functions:
 *   - destroy_all
 */

/*Global variables*/
int nof_dqs = 6;
/* Data qualifier IDs (used for destroying at cleanup)*/
int dq_id[6];
/* Preselector ID (predefined if advanced mode, updated if not*/
bcm_field_presel_t pid = 5;
/* Offset to indicate entry is a preselector. For advanced mode is updated to also indicate stage.*/
int presel_flags = BCM_FIELD_QUALIFY_PRESEL;

/*
 * Main function to configure PMF
 */
int field_setup(/* in */ int unit,
                /* in */ int nof_tags,
                /* in */ bcm_field_group_t group,
                /* in */ int group_priority,
                /* in */ int counter_id)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "egress_pmf_setup";
    uint8 smac[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    uint8 mask[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_ip_t sip = 0x01020304;
    bcm_ip_t dip = 0x04030201;
    bcm_ip_t ip_mask = 0xffffffff;

    bcm_field_group_config_t fg;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_presel_set_t psset, psset1;
    bcm_field_data_qualifier_t dq;
    uint8 qdata[4], qmask[4];
    int qsize;
    bcm_field_qualify_t dq_base0 = bcmFieldQualifyEgressBridgedForwardingDataHigh;
    bcm_field_qualify_t dq_base1 = bcmFieldQualifyEgressBridgedForwardingDataLow;
    bcm_field_qualify_t dq_base2 = bcmFieldQualifyEgressBridgedAclDataHigh;
    bcm_field_qualify_t dq_base3 = bcmFieldQualifyEgressBridgedAclDataLow;

    bcm_field_qualify_t dq_base[6]   = {dq_base0, dq_base1, dq_base1, dq_base2, dq_base2, dq_base3};
    uint8               dq_offset[6] = {0       , 24      , 0       , 32      , 0       , 0       };
    uint8               dq_length[6] = {32      , 32      , 24      , 8       , 32      , 32      };
    int32_t bcm_counter_id = counter_id;
    bcm_field_action_core_config_t core_config_arr[SOC_DPP_DEFS_GET_NOF_CORES(unit)];
    int dq_idx;

    /* Create Data qualifiers */
    for(dq_idx = 0; dq_idx < nof_dqs; dq_idx++) {
        bcm_field_data_qualifier_t_init(&dq);
        dq.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        dq.stage = bcmFieldStageEgress;
        dq.qualifier = dq_base[dq_idx];
        dq.offset = dq_offset[dq_idx];
        dq.length = dq_length[dq_idx];
        rv = bcm_field_data_qualifier_create(unit, &dq);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_field_data_qualifier_create\n");
        }
        dq_id[dq_idx] = dq.qual_id;
        printf("Created DQ[%d] = %d\n", dq_idx, dq_id[dq_idx]);
    }

    /*Create preselectors*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS;
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageEgress, pid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_presel_create_stage_id\n");
        }
    } else {
        rv = bcm_field_presel_create(unit, &pid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_presel_create\n");
        }
    }
    rv = bcm_field_qualify_Stage(unit,
                                 pid | presel_flags,
                                 bcmFieldStageEgress);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_Stage\n");
    }
    rv = bcm_field_qualify_ForwardingType(unit, pid | presel_flags, bcmFieldForwardingTypeAny);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_VlanFormat\n");
    }
    bcm_field_presel_set_t_init(&psset);
    BCM_FIELD_PRESEL_ADD(psset, pid);

    /* Create group */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardCopy);
    switch(nof_tags) {
    case 0:
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[0]);
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[1]);
        break;
    case 1:
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[1]);
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[2]);
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[3]);
        break;
    case 2:
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[2]);
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[3]);
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[4]);
        break;
    case 3:
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[4]);
        bcm_field_qset_data_qualifier_add(unit, qset, dq_id[5]);
        breaak;
    default:
        break;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

    bcm_field_group_config_t_init(&fg);
    fg.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET ;
    fg.group = group;
    fg.priority = group_priority;
    fg.qset = qset;
    fg.preselset = psset;
    fg.mode = bcmFieldGroupModeAuto;
    rv = bcm_field_group_config_create(unit, fg);
    if(rv != BCM_E_NONE) {
        printf("%s: error! bcm_field_group_create_id returned %d\n", proc_name, rv);
    }
    rv = bcm_field_group_action_set(unit,group, aset);
    if(rv != BCM_E_NONE) {
        printf("%s: error! bcm_field_group_action_set returned %d\n", proc_name, rv);
    }
    /* Add entry */
    rv = bcm_field_entry_create(unit, group, &ent);
    if(rv != BCM_E_NONE) {
        printf("%s: error! bcm_field_entry_create returned %d\n", proc_name, rv);
    }
    /* Add qualifiers */
    rv = bcm_field_qualify_ForwardCopy(unit, ent, 0, 0);
    switch(nof_tags) {
    case 0:
        /* for 0 VLAN tags: */
        /* SIP */
        qsize = 4;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (sip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[0], &qdata, &qmask, qsize); /* SIP */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        /* DIP */
        qsize = 4;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (dip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[1], &qdata, &qmask, qsize); /* DIP */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        break;
    case 1:
        /* 1 tag, SIP */
        qsize = 4;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (sip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[1], &qdata, &qmask, qsize); /* SIP */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        /* 1 tag, DIP pt 1*/
        qsize = 3;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (dip >> ((qsize - dq_idx) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[2], &qdata, &qmask, qsize); /* DIP-high */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        /* 1 tag, DIP pt 2*/
        qsize = 1;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx = 0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (dip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[3], &qdata, &qmask, qsize); /* DIP-low */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        break;
    case 2:
        /* 2 tags, SIP pt 1*/
        qsize = 3;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (sip >> ((qsize - dq_idx) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[2], &qdata, &qmask, qsize); /* SIP-high */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        /* 2 tags, SIP pt 2*/
        qsize = 1;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx = 0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (sip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[3], &qdata, &qmask, qsize); /* SIP-low */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        /* 2 tags, DIP */
        qsize = 4;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (dip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[4], &qdata, &qmask, qsize); /* DIP */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        break;
    case 3:
        /* 3 tags, SIP */
        qsize = 4;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (sip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[4], &qdata, &qmask, qsize); /* SIP */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        /* 3 tags, DIP */
        qsize = 4;
        sal_memset(&qdata, 0, sizeof(qdata));
        sal_memset(&qmask, 0, sizeof(qmask));
        for (dq_idx =0; dq_idx < qsize; dq_idx++) {
            qdata[dq_idx] = (dip >> ((qsize - dq_idx - 1) * 8)) % 256;
            qmask[dq_idx] = (ip_mask >> ((qsize - dq_idx - 1) * 8)) % 256;
        }
        rv = bcm_field_qualify_data(unit, ent, dq_id[5], &qdata, &qmask, qsize); /* DIP */
        if (BCM_E_NONE != rv)
        {
            printf("Error in bcm_field_qualify_data\n");
            return rv;
        }
        break;
    default:
        break;
    }

    /* Actions : count + drop*/
    if(nof_tags == 0 ) {
        core_config_arr[0].param0 = bcm_counter_id;
        core_config_arr[0].param1 = BCM_ILLEGAL_ACTION_PARAMETER;
        core_config_arr[0].param2 = BCM_ILLEGAL_ACTION_PARAMETER;

        core_config_arr[1].param0 = bcm_counter_id;
        core_config_arr[1].param1 = BCM_ILLEGAL_ACTION_PARAMETER;
        core_config_arr[1].param2 = BCM_ILLEGAL_ACTION_PARAMETER;

        bcm_field_action_config_add(unit,ent,bcmFieldActionStat,2,&core_config_arr[0]);
        if(rv != BCM_E_NONE) {
            printf("%s: error! bcm_field_action_add returned %d\n", proc_name, rv);
        }
    }
    rv = bcm_field_action_add(unit,ent,bcmFieldActionDrop,0,0);
    if(rv != BCM_E_NONE) {
        printf("%s: error! bcm_field_action_add returned %d\n", proc_name, rv);
    }
    rv = bcm_field_group_install(unit, group);
    if(rv != BCM_E_NONE) {
        printf("%s: error! bcm_field_group_install returned %d\n", proc_name, rv);
    }

    return rv;
}

int destroy_all (/* in */ int unit,
                 /* in */ bcm_field_group_t group)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    int dq_idx;

    proc_name = "destroy";
    rv = bcm_field_group_flush(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s():Error in function bcm_field_group_flush(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }

    for(dq_idx = 0; dq_idx < nof_dqs; dq_idx++) {
        rv = bcm_field_data_qualifier_destroy(unit, dq_id[dq_idx]);
        if (rv != BCM_E_NONE) {
            printf("%s():Error in function bcm_field_group_flush(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
    }

    rv = bcm_field_presel_destroy(unit, pid | presel_flags & (~BCM_FIELD_QUALIFY_PRESEL));
    if (rv != BCM_E_NONE) {
        printf("%s():Error in function bcm_field_presel_destroy(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


