/* $Id: cint_field_tcam_bank_reserve.c,v 1.4
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * Cint to demonstrate TCAM banks reservation SOC property usage.
 */

/*
 * Init function.
 * Checks that the TCAM banks reservation soc properties has been set to the following:
 * 1) Bank 0 -> IPMF_0
 * 2) Bank 1 -> IPMF_1
 * 3) Bank 2 -> EPMF
 * 4) Bank 3 -> FLP_0
 * 5) Bank 4 -> FLP_1
 * 6) Bank 5 -> VTT_0
 * 7) Bank 6 -> VTT_1
 */

int cint_field_tcam_bank_reserve_init(int unit)
{
    char *tcam_bank_block_owner;

    tcam_bank_block_owner = soc_property_port_get_str(unit, 0, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE);
    if (tcam_bank_block_owner != NULL && sal_strcmp(tcam_bank_block_owner, "IPMF_0")) {
        printf("Error in tcam_bank_resreve_init(): Bank 0 has not been reserved to IPMF_0 stage\n");
        return BCM_E_PARAM;
    }

    tcam_bank_block_owner = soc_property_port_get_str(unit, 1, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE);
    if (tcam_bank_block_owner != NULL && sal_strcmp(tcam_bank_block_owner, "IPMF_1")) {
        printf("Error in tcam_bank_resreve_init(): Bank 1 has not been reserved to IPMF_1 stage\n");
        return BCM_E_PARAM;
    }

    tcam_bank_block_owner = soc_property_port_get_str(unit, 2, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE);
    if (tcam_bank_block_owner != NULL && sal_strcmp(tcam_bank_block_owner, "EPMF")) {
        printf("Error in tcam_bank_resreve_init(): Bank 2 has not been reserved to EPMF stage\n");
        return BCM_E_PARAM;
    }

    tcam_bank_block_owner = soc_property_port_get_str(unit, 3, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE);
    if (tcam_bank_block_owner != NULL && sal_strcmp(tcam_bank_block_owner, "FLP_0")) {
        printf("Error in tcam_bank_resreve_init(): Bank 3 has not been reserved to FLP_0 stage\n");
        return BCM_E_PARAM;
    }

    tcam_bank_block_owner = soc_property_port_get_str(unit, 4, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE);
    if (tcam_bank_block_owner != NULL && sal_strcmp(tcam_bank_block_owner, "FLP_1")) {
        printf("Error in tcam_bank_resreve_init(): Bank 4 has not been reserved to FLP_1 stage\n");
        return BCM_E_PARAM;
    }

    tcam_bank_block_owner = soc_property_port_get_str(unit, 5, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE);
    if (tcam_bank_block_owner != NULL && sal_strcmp(tcam_bank_block_owner, "VTT_0")) {
        printf("Error in tcam_bank_resreve_init(): Bank 5 has not been reserved to VTT_0 stage\n");
        return BCM_E_PARAM;
    }

    tcam_bank_block_owner = soc_property_port_get_str(unit, 6, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE);
    if (tcam_bank_block_owner != NULL && sal_strcmp(tcam_bank_block_owner, "VTT_1")) {
        printf("Error in tcam_bank_resreve_init(): Bank 6 has not been reserved to VTT_1 stage\n");
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int               cint_field_tcam_bank_resreve_presel_id = 5;
bcm_field_group_t cint_field_tcam_bank_resreve_group;
bcm_field_entry_t cint_field_tcam_bank_resreve_ent;

/*
 * TCAM Bank reserve main function:
 * - Create new presel_id for stage Ingress
 * - Create new FG for IPMF_1 stage (Q:SA|A:DP)
 * - Create new entry for the FG (SA=00:0c:00:02:00:00|DP=3)
 */
int cint_field_tcam_bank_reserve_main(int unit)
{
    int cint_field_tcam_bank_resreve_presel_id=5;
    int result;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
    bcm_field_stage_t stage =  bcmFieldStageIngress;

    result = cint_field_tcam_bank_reserve_init(unit);
    if (result != BCM_E_NONE) {
            printf("Error in cint_field_tcam_bank_reserve_init\n");
            return result;
    }

/*     Create a presel entity*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        result = bcm_field_presel_create_stage_id(unit, stage, cint_field_tcam_bank_resreve_presel_id);

        presel_flags = BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;

        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, cint_field_tcam_bank_resreve_presel_id);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create_id\n");
            return result;
        }
    }

 /*    Depending on stage, configure the presel stage and in/out port qualifier*/
    result = bcm_field_qualify_Stage(unit, cint_field_tcam_bank_resreve_presel_id | presel_flags, stage);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    int nof_qual = 0;
    bcm_field_group_config_t group_config;
	uint32 cint_fg_with_hw_id_flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_CYCLE | BCM_FIELD_GROUP_CREATE_WITH_MODE;

    bcm_field_group_config_t_init(&group_config);

    BCM_FIELD_PRESEL_INIT(group_config.preselset);
    BCM_FIELD_PRESEL_ADD(group_config.preselset, cint_field_tcam_bank_resreve_presel_id);

    BCM_FIELD_QSET_INIT(group_config.qset);

    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstMac);

    BCM_FIELD_ASET_INIT(group_config.aset);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDropPrecedence);

    group_config.flags = cint_fg_with_hw_id_flags;
    group_config.mode = bcmFieldGroupModeSingle;
    group_config.cycle = bcmFieldGroupCycle0;
    group_config.priority = cint_field_tcam_bank_resreve_presel_id;

    result = bcm_field_group_config_create(unit,&group_config);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_create_id\n");
        return result;
    }
    printf("\n group %d created \n",group_config.group);

    cint_field_tcam_bank_resreve_group = group_config.group;
    bcm_field_action_t action = bcmFieldActionDropPrecedence;
    int action_val=3;
    bcm_mac_t macData;
    bcm_mac_t macMask;

    macData[0] = 0x00;
    macData[1] = 0x0c;
    macData[2] = 0x00;
    macData[3] = 0x02;
    macData[4] = 0x00;
    macData[5] = 0x00;

    macMask[0] = 0xff;
    macMask[1] = 0xff;
    macMask[2] = 0xff;
    macMask[3] = 0xff;
    macMask[4] = 0xff;
    macMask[5] = 0xff;

    result = bcm_field_entry_create(unit, cint_field_tcam_bank_resreve_group, &cint_field_tcam_bank_resreve_ent);
    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_entry_create failed (%s)\n", bcm_errmsg(result));
        return result;
    }

    result =  bcm_field_qualify_DstMac(unit, cint_field_tcam_bank_resreve_ent, macData, macMask);
    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_qualify_DstMac failed (%s)\n", bcm_errmsg(result));
        return result;
    }

    result = bcm_field_action_add(unit, cint_field_tcam_bank_resreve_ent, action, action_val, 0);

    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_action_add failed (%s)\n", bcm_errmsg(result));
        return result;
    }
    result = bcm_field_entry_install(unit, cint_field_tcam_bank_resreve_ent);
    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_entry_install failed (%s)\n", bcm_errmsg(result));
        return result;
    }
    printf("FIELD ENTRY CREATED: %d\n-~-~-~-~-~-~-~-~-~-\n", cint_field_tcam_bank_resreve_ent);

    return result;
}


/*
 * TCAM Bank reserve destroy function:
 * - Destroy everything created by the main function
 */
int cint_field_tcam_bank_reserve_destroy(int unit)
{
    int result;
    result = bcm_field_entry_destroy(unit, cint_field_tcam_bank_resreve_ent);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_entry_destroy");
        return result;
    }
    result = bcm_field_group_destroy(unit, cint_field_tcam_bank_resreve_group);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_destroy");
        return result;
    }
    result = bcm_field_presel_destroy(unit, cint_field_tcam_bank_resreve_presel_id);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_presel_destroy");
        return result;
    }
    return result;
}
