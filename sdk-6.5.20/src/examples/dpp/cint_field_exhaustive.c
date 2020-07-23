/* $ID: cint_field_exhaustive.c 2015/11/30 $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_field_exhaustive.c
 * Purpose: Cint for testing the usage of field DBs with Key-A
 *
 * Main Functions:
 *     run_main_last(INPUT unit) -> Creates L2, L3 and Key-A DBs
 *     run_main_first(INPUT unit) -> Creates Key-A, L2 and L3 DBs
 *     run_main_negative(INPUT unit) -> Creates L2, L3(with one more qualifier) and Key-A DBs
 *     exaustive_test_field_group_tcam(INPUT unit, INPUT group_priority,
 *       INPUT num_of_groups, INPUT num_of_tcam_entries) 
 *          -> creates num_of_groups groups with total num_of_tcam_entries
 *             entries to fill TCAM_ACTION
 * 
 * Cleanup functions:
 *     destroyAll(INPUT unit) -> Flushes groups with entries, destroys qualifiers and preselectors
 *     destroyGroup(INPUT unit) -> Flushes groups with entries
 *     destroyQualify(INPUT unit) -> Destroys qualifiers
 *     destroyPresel(INPUT unit) -> Destroys preselectors
 *     cleanup_field_group_tcam(INPUT unit, INPUT num_of_groups) 
 *          -> flushes groups 0 to num_of_groups
 */

int    group_bmp[128];
int    qual_bmp[16];
int    presel_bmp[32];
bcm_field_presel_t    iPreIdL2;
bcm_field_presel_t    iPreIdEthEth;
bcm_field_presel_t    iPreIdIpv4;
bcm_field_presel_t    iPreIdIpv6;
bcm_field_presel_t    iPreIdMplsLabel1;
bcm_field_presel_t    iPreIdMplsLabel2;
bcm_field_presel_t    iPreIdMplsLabel3;
bcm_field_presel_t    iPreIdTrill;
bcm_field_presel_t    iPreIdEthTrillEth;
bcm_field_presel_t    iPreIdEthIp4Eth;
bcm_field_presel_t    iPreIdL2Rcy;
bcm_field_presel_t    iPreIdIpv4Rcy;
bcm_field_presel_t    iPreIdIpv6Rcy;
bcm_field_presel_t    iPreIdRcy;
bcm_field_presel_t    iPreIdPhy;
bcm_field_presel_t    iPreIdIpv4Mc;
bcm_field_presel_t    iPreIdIpv6Mc;
bcm_field_presel_t    iPreIdFwdTypeL2;
bcm_field_presel_t    iPreIdFwdTypeIpv4Ucast;
bcm_field_presel_t    iPreIdFwdTypeIpv4mcast;
bcm_field_presel_t    iPreIdFwdTypeIpv6Ucast;
bcm_field_presel_t    iPreIdFwdTypeIpv6Mcast;
bcm_field_presel_t    iPreIdFwdTypeMpls;
bcm_field_presel_t    iPreIdFwdTypeTrill;
bcm_field_presel_t    iPreIdAppTypeAll;

/* Cleanup function */
int destroyAll(int unit) {
    int rv = BCM_E_NONE;
    int i;
    printf("Destroying all created field objects...\n");
    rv = destroyGroup(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: destroyGroup() failed.\n");
        return rv;
    }
    rv = destroyQualify(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: destroyQualify() failed.\n");
        return rv;
    }
    rv = destroyPresel(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: destroyPresel() failed.\n");
        return rv;
    }
    printf("destroyAll() finished.\n");
    return rv;
}

/* Cleanup function for Groups */
int destroyGroup(int unit) {
    int rv = BCM_E_NONE;
    int i;
    printf("Destroying groups...\n");
    for (i = 0; i < 128; i++) {
        if(group_bmp[i]) {
            bcm_field_group_flush(unit, i);
            group_bmp[i] = 0;
        }
    }
    printf("destroyGroup() finished.\n");
    return rv;
}

/* Cleanup function for Qualifiers */
int destroyQualify(int unit) {
    int rv = BCM_E_NONE;
    int i;
    printf("Destroying Qualifiers...\n");
    for (i = 0; i < 16; i++) {
        if(qual_bmp[i]) {
            bcm_field_data_qualifier_destroy(unit, i);
            qual_bmp[i] = 0;
        }
    }
    printf("destroyQualify() finished.\n");
    return rv;
}

/* Cleanup function for Preselectors */
int destroyPresel(int unit) {
    printf("Destroying all preselectors...\n");
    int rv = BCM_E_NONE;
    int i;
    for (i = 0; i < 32; i++)
    {
        if(presel_bmp[i]) {
            bcm_field_presel_destroy(unit, i);
            presel_bmp[i] = 0;
        }
    }
    printf("destroyPresel() finished.\n");
    return rv;
}

/* Function to initialise Preselectors */
int initPreSel(int unit) {
    int rv = BCM_E_NONE;
    int i = 0;
    bcm_pbmp_t pbmRcy;
    bcm_pbmp_t pbmPhy;
    bcm_pbmp_t pbm_mask;

    BCM_PBMP_CLEAR(pbm_mask);
    for(i=0; i<256; i++) {
        BCM_PBMP_PORT_ADD(pbm_mask, i);
    }
    /* Physical ports: 13-17 */
    BCM_PBMP_CLEAR(pbmPhy);
    for(i=13; i<=17; i++) {
        BCM_PBMP_PORT_ADD(pbmPhy, i);
    }
    /* Recycle ports: 200-203 (CPU ports) */
    BCM_PBMP_CLEAR(pbmRcy);
    for(i=200; i<=203; i++) {
        BCM_PBMP_PORT_ADD(pbmRcy, i);
    }
    i = 0;
    /*1*/
    rv = bcm_field_presel_create(unit, &iPreIdL2);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdL2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatL2);
    rv = bcm_field_qualify_InPorts(unit, iPreIdL2 | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdL2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*2*/
    rv = bcm_field_presel_create(unit, &iPreIdEthEth);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdEthEth | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatEthEth);
    rv = bcm_field_qualify_InPorts(unit, iPreIdEthEth | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdEthEth | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*3*/
    rv = bcm_field_presel_create(unit, &iPreIdIpv4);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdIpv4 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp4AnyL2L3);
    rv = bcm_field_qualify_InPorts(unit, iPreIdIpv4 | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdIpv4 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*4*/
    rv = bcm_field_presel_create(unit, &iPreIdIpv6);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdIpv6 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp6AnyL2L3);
    rv = bcm_field_qualify_InPorts(unit, iPreIdIpv6 | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdIpv6 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*5*/
    rv = bcm_field_presel_create(unit, &iPreIdMplsLabel1);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdMplsLabel1 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatMplsLabel1AnyL2L3);
    rv = bcm_field_qualify_InPorts(unit, iPreIdMplsLabel1 | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdMplsLabel1 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*6*/
    rv = bcm_field_presel_create(unit, &iPreIdMplsLabel2);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdMplsLabel2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatMplsLabel2AnyL2L3);
    rv = bcm_field_qualify_InPorts(unit, iPreIdMplsLabel2 | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdMplsLabel2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*7*/
    rv = bcm_field_presel_create(unit, &iPreIdMplsLabel3);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdMplsLabel3 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatMplsLabel3AnyL2L3);
    rv = bcm_field_qualify_InPorts(unit, iPreIdMplsLabel3 | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdMplsLabel3 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*8*/
    rv = bcm_field_presel_create(unit, &iPreIdTrill);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdTrill | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatTrill);
    rv = bcm_field_qualify_InPorts(unit, iPreIdTrill | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdTrill | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*9*/
    rv = bcm_field_presel_create(unit, &iPreIdEthTrillEth);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdEthTrillEth | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatEthTrillEth);
    rv = bcm_field_qualify_InPorts(unit, iPreIdEthTrillEth | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdEthTrillEth | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*10*/
    rv = bcm_field_presel_create(unit, &iPreIdL2Rcy);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdL2Rcy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatL2);
    rv = bcm_field_qualify_InPorts(unit, iPreIdL2Rcy | BCM_FIELD_QUALIFY_PRESEL, pbmRcy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdL2Rcy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*11*/
    rv = bcm_field_presel_create(unit, &iPreIdIpv4Rcy);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdIpv4Rcy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp4AnyL2L3);
    rv = bcm_field_qualify_InPorts(unit, iPreIdIpv4Rcy | BCM_FIELD_QUALIFY_PRESEL, pbmRcy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdIpv4Rcy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*12*/
    rv = bcm_field_presel_create(unit, &iPreIdIpv6Rcy);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdIpv6Rcy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp6AnyL2L3);
    rv = bcm_field_qualify_InPorts(unit, iPreIdIpv6Rcy | BCM_FIELD_QUALIFY_PRESEL, pbmRcy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdIpv6Rcy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*13*/
    rv = bcm_field_presel_create(unit, &iPreIdEthIp4Eth);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_HeaderFormat(unit, iPreIdEthIp4Eth | BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatEthIp4Eth);
    rv = bcm_field_qualify_Stage(unit, iPreIdEthIp4Eth | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*14*/
    rv = bcm_field_presel_create(unit, &iPreIdRcy);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_InPorts(unit, iPreIdRcy | BCM_FIELD_QUALIFY_PRESEL, pbmRcy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdRcy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*15*/
    rv = bcm_field_presel_create(unit, &iPreIdPhy);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_InPorts(unit, iPreIdPhy | BCM_FIELD_QUALIFY_PRESEL, pbmPhy, pbm_mask);
    rv = bcm_field_qualify_Stage(unit, iPreIdPhy | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    printf(" Presel %d created\n", i);
    /*16*/
    rv = bcm_field_presel_create(unit, &iPreIdIpv4Mc);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdIpv4Mc | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    rv = bcm_field_qualify_HeaderFormat(unit,  iPreIdIpv4Mc| BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp4AnyL2L3);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdIpv4Mc| BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeIp4Mcast);
    printf(" Presel %d created\n", i);
    /*17*/
    rv = bcm_field_presel_create(unit, &iPreIdIpv6Mc);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdIpv6Mc | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    rv = bcm_field_qualify_HeaderFormat(unit,  iPreIdIpv6Mc| BCM_FIELD_QUALIFY_PRESEL, bcmFieldHeaderFormatIp6AnyL2L3);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdIpv6Mc| BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeIp6Mcast);
    printf(" Presel %d created\n", i);
    /*18*/
    rv = bcm_field_presel_create(unit, &iPreIdFwdTypeL2);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdFwdTypeL2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdFwdTypeL2 | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeL2);
    printf(" Presel %d created\n", i);
    /*19*/
    rv = bcm_field_presel_create(unit, &iPreIdFwdTypeIpv4Ucast);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdFwdTypeIpv4Ucast | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdFwdTypeIpv4Ucast| BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeIp4Ucast);
    printf(" Presel %d created\n", i);
    /*20*/
    rv = bcm_field_presel_create(unit, &iPreIdFwdTypeIpv4mcast);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdFwdTypeIpv4mcast | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdFwdTypeIpv4mcast| BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeIp4Mcast);
    printf(" Presel %d created\n", i);
    /*21*/
    rv = bcm_field_presel_create(unit, &iPreIdFwdTypeIpv6Ucast);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdFwdTypeIpv6Ucast | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdFwdTypeIpv6Ucast| BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeIp6Ucast);
    printf(" Presel %d created\n", i);
    /*22*/
    rv = bcm_field_presel_create(unit, &iPreIdFwdTypeIpv6Mcast);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdFwdTypeIpv6Mcast | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdFwdTypeIpv6Mcast| BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeIp6Mcast);
    printf(" Presel %d created\n", i);
    /*23*/
    rv = bcm_field_presel_create(unit, &iPreIdFwdTypeMpls);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdFwdTypeMpls | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdFwdTypeMpls | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeMpls);
    printf(" Presel %d created\n", i);
    /*24*/
    rv = bcm_field_presel_create(unit, &iPreIdFwdTypeTrill);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdFwdTypeTrill | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    rv = bcm_field_qualify_ForwardingType(unit, iPreIdFwdTypeTrill | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeTrill);
    printf(" Presel %d created\n", i);
    /*25*/
    rv = bcm_field_presel_create(unit, &iPreIdAppTypeAll);
    presel_bmp[i++] = 1;
    rv = bcm_field_qualify_Stage(unit, iPreIdAppTypeAll | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageExternal);
    rv = bcm_field_qualify_AppType(unit, iPreIdAppTypeAll | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeL2);
    rv = bcm_field_qualify_AppType(unit, iPreIdAppTypeAll | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4Ucast);
    rv = bcm_field_qualify_AppType(unit, iPreIdAppTypeAll | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4UcastRpf);
    printf(" Presel %d created\n", i);

    return rv;
}

/* Function to create L2 field group DB with entry */
int createGroupL2(int unit) {
    int rv = BCM_E_NONE;
    bcm_field_presel_set_t stPset;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t ConfigGroup;
    bcm_field_group_t grp = 2;
    int grp_pri = 112;
    bcm_field_entry_t ent;
    bcm_mac_t macMask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    printf(" Create Qset\n");
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassProcessingPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIngressStpState);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassL2);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassPort);
    printf(" Create data qualifiers\n");
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.offset_base = bcmFieldDataOffsetBaseFirstHeader;
    data_qual.offset = 0;
    data_qual.length = 4;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    qual_bmp[data_qual.qual_id] = 1;
    rv = bcm_field_qset_data_qualifier_add(unit, &qset, data_qual.qual_id);
    data_qual.offset_base = bcmFieldDataOffsetBaseFirstHeader;
    data_qual.offset = 4;
    data_qual.length = 4;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    qual_bmp[data_qual.qual_id] = 1;
    rv = bcm_field_qset_data_qualifier_add(unit, &qset, data_qual.qual_id);
    printf(" Create Aset\n");
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDoNotLearn);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);

    printf(" Create field group\n");
    rv = bcm_field_group_create_id(unit, qset, grp_pri, grp);
    group_bmp[grp] = 1;
    printf(" Add presel\n");
    BCM_FIELD_PRESEL_INIT(stPset);
    BCM_FIELD_PRESEL_ADD(stPset, iPreIdL2);
    BCM_FIELD_PRESEL_ADD(stPset, iPreIdEthEth);
    rv = bcm_field_group_presel_set(unit, grp, &stPset);
    rv = bcm_field_group_action_set(unit, grp, aset);

    bcm_mac_t macDa2   = {0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

    printf(" Create entry\n");
    rv = bcm_field_entry_create(unit, 2, &ent);
    rv = bcm_field_qualify_DstMac(unit, ent, macDa2, macMask);
    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    rv = bcm_field_entry_prio_set(unit, ent, 2140667903);
    rv = bcm_field_entry_install(unit, ent);

    printf("Created L2 group ID=%d with entry ID=%d\n", grp, ent);

    return rv;
}

/* Function to create L3 field group DB with entry */
int createGroupL3(int unit, int is_keyA_first) {
    int rv = BCM_E_NONE;
    bcm_field_presel_set_t stPset;
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t ConfigGroup;
    bcm_field_group_t grp = 3;
    int grp_pri = 113;
    bcm_field_entry_t ent;
    bcm_mac_t macMask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    printf(" Create Qset\n");
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassProcessingPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstL3Egress);
    if (is_keyA_first) {
        /* Add this qualifier only if key-A DB was added first */
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIngressStpState);
    }
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassL2);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpmcHit);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpFrag);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyExternalHit0);
    printf(" Create Aset\n");
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDoNotLearn);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    printf(" Create field group\n");
    rv = bcm_field_group_create_id(unit, qset, grp_pri, grp);
    group_bmp[grp] = 1;
    printf(" Add presel\n");
    BCM_FIELD_PRESEL_INIT(stPset);
    BCM_FIELD_PRESEL_ADD(stPset, iPreIdIpv4);
    rv = bcm_field_group_presel_set(unit, grp, &stPset);
    rv = bcm_field_group_action_set(unit, grp, aset);

    bcm_mac_t macDa3   = {0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

    printf(" Create entry\n");
    rv = bcm_field_entry_create(unit, 2, &ent);
    rv = bcm_field_qualify_DstMac(unit, ent, macDa3, macMask);
    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    rv = bcm_field_entry_prio_set(unit, ent, 2140667903);
    rv = bcm_field_entry_install(unit, ent);

    printf("Created L3 group ID=%d with entry ID=%d\n", grp, ent);

    return rv;
}

/* Function to create Key-A field group DB with entry */
int create_KeyA(int unit) {
    int rv = BCM_E_NONE;
    bcm_field_presel_set_t stPset;
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t ConfigGroup;
    bcm_field_entry_t ent;

    /* QSET */
    printf(" Create Qset\n");
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInnerVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIngressStpState);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);

    /* ASET */
    printf(" Create Aset\n");
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    /* PSET */
    printf(" Add presel\n");
    BCM_FIELD_PRESEL_INIT(stPset);
    BCM_FIELD_PRESEL_ADD(stPset, iPreIdPhy);

    ConfigGroup.group = 4;
    ConfigGroup.priority = 119;
    ConfigGroup.qset      = qset;
    ConfigGroup.aset      = aset;
    ConfigGroup.preselset = stPset;
    ConfigGroup.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_SINGLE;
    
    printf(" Create field group\n");
    rv = bcm_field_group_config_create(unit,  &ConfigGroup);
    group_bmp[ConfigGroup.group] = 1;
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_group_config_create failed (%s)\n",bcm_errmsg(rv));
        return rv;
    }

    printf(" Create entry\n");
    rv = bcm_field_entry_create(unit, ConfigGroup.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_entry_create failed (%s)\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_qualify_EtherType(unit, ent, 0x0800, 0xffff);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_qualify_EtherType failed (%s)\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_action_add failed (%s)\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_entry_prio_set(unit, ent, 2140667905);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_entry_prio_set failed (%s)\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error: bcm_field_entry_install failed (%s)\n",bcm_errmsg(rv));
        return rv;
    }

    printf("Created Key-A group ID=%d with entry ID=%d\n", ConfigGroup.group, ent);

    return rv;
}


/* Main function: Key-A added last */
int run_main_last (int unit) {
    int rv = BCM_E_NONE;
    int is_keyA_first = 0;
    printf("run_main: createGroupL2...\n");
    rv = createGroupL2(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: createGroupL2 failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    printf("run_main: createGroupL3...\n");
    rv = createGroupL3(unit, is_keyA_first);
    if (rv != BCM_E_NONE) {
        printf("Error: createGroupL3 failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    printf("run_main: create_KeyA...\n");
    rv = create_KeyA(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: create_KeyA failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    return rv;
}

/* Main function 2: Key-A added first */
int run_main_first (int unit) {
    int rv = BCM_E_NONE;
    int is_keyA_first = 0;
    printf("run_main: create_KeyA...\n");
    rv = create_KeyA(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: create_KeyA failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    printf("run_main: createGroupL2...\n");
    rv = createGroupL2(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: createGroupL2 failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    printf("run_main: createGroupL3...\n");
    rv = createGroupL3(unit, is_keyA_first);
    if (rv != BCM_E_NONE) {
        printf("Error: createGroupL3 failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    return rv;
}

/* Main function 3: Negative test */
int run_main_negative (int unit) {
    int rv = BCM_E_NONE;
    int is_keyA_first = 1; /* to add one more qualifier to L3 group */
    printf("run_main: createGroupL2...\n");
    rv = createGroupL2(unit);
    if (rv != BCM_E_NONE) {
        printf("Error: createGroupL2 failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    printf("run_main: createGroupL3...\n");
    rv = createGroupL3(unit, is_keyA_first);
    if (rv != BCM_E_NONE) {
        printf("Error: createGroupL3 failed (%s)", bcm_errmsg(rv));
        return(rv);
    }
    printf("run_main: create_KeyA...\n");
    rv = create_KeyA(unit);
    if (rv != BCM_E_FULL) {
        printf("Error: create_KeyA returned (%s)\nExpected: bcm_errmsg(BCM_E_FULL)\n", bcm_errmsg(rv));
        return(BCM_E_FAIL);
    } else {
        printf("create_KeyA returned (%s) as expected.\n", bcm_errmsg(rv));
        rv = BCM_E_NONE;
    }
    return rv;
}
/* Cleanup function after exaustive_test_field_group_tcam() */
int cleanup_field_group_tcam(int unit, int num_of_groups) {
    bcm_error_t rv = BCM_E_NONE;
    int i;
    for (i = 0; i < num_of_groups; i++) {
        rv = bcm_field_group_flush(unit, i);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_group_flush (%s)\n", bcm_errmsg(rv));
            return rv;
        }
        printf("Flushed field group %d\n", i);
    }
    return rv;
}

/* Main function to fill action entries to TCAM */
int exaustive_test_field_group_tcam(int unit, int group_priority, int num_of_groups, int num_of_tcam_entries) {
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_group_t grp_tcam;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_mac_t macData;
    bcm_mac_t macMask;
    int num_of_enries_per_group = num_of_tcam_entries / num_of_groups;
    bcm_field_entry_t ent_arr[num_of_tcam_entries];
    int index;
    int entry, group;


    /* Define the QSET. No need to use IsEqual as qualifier for this field group.*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);

    for (group = 0; group < num_of_groups; group_priority++, group++) {
        /*  Create the Field group with type TCAM */
        rv = bcm_field_group_create(unit, qset, group_priority, &grp_tcam);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_group_create\n");
            return rv;
        }

        /* Define the ASET - use counter 0. */
        BCM_FIELD_ASET_INIT(aset);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel0);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerLevel1);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionUsePolicerResult);

        /*  Attach the action set */
        rv = bcm_field_group_action_set(unit, grp_tcam, aset);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam);
            return rv;
        }

        printf("BCM Field Group ID: %d\n", grp_tcam);
        for (entry = 0; entry < num_of_enries_per_group; entry++)
        {
            index = group * num_of_enries_per_group + entry;
            rv = bcm_field_entry_create(unit, grp_tcam, &(ent_arr[index]));
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_entry_create\n");
                return rv;
            }

            macData[0] = 0x0;
            macData[1] = 0x0;
            macData[2] = 0x0;
            macData[3] = 0x0;
            macData[4] = 0x0;
            macData[5] = 0x1;
            macMask[0] = 0xff;
            macMask[1] = 0xff;
            macMask[2] = 0xff;
            macMask[3] = 0xff;
            macMask[4] = 0xff;
            macMask[5] = 0xff;
            rv = bcm_field_qualify_SrcMac(unit, ent_arr[index], macData, macMask);
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_qualify_SrcMac\n");
                return rv;
            }

            rv = bcm_field_qualify_EtherType(unit, ent_arr[index], 0x821, 0xffff);
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_qualify_EtherType\n");
                return rv;
            }

            rv = bcm_field_entry_prio_set(unit, ent_arr[index], num_of_tcam_entries - entry);
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_entry_prio_set, index %d prio %d, i %d\n", index, num_of_tcam_entries - entry, entry);
                return rv;
            }

            rv = bcm_field_action_add(unit, ent_arr[index], bcmFieldActionPolicerLevel0, 1000 + entry, 0);
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_action_add\n");
                return rv;
            }

            rv = bcm_field_entry_install(unit, ent_arr[index]);
            if (BCM_E_NONE != rv) {
                printf("Error in bcm_field_entry_install\n");
                return rv;
            }
            if ((index % 100) == 0)
            {
                printf("BCM Field Group Prio: %d, group %d, index %d Entry ID: %d\n", group_priority, group, index, ent_arr[index]);
            }
        }
    }

    return rv;
}
