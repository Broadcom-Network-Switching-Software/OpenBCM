
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 *  Sequence:
 *  1.  Define a preselection with In/Out ports as qualifier.
 *      Only packets which arrive or are destined to the specified
 *      ports will be processed.
 *  2.  Create a Field Group with the explicitly specified hw id, and add an entry.
 *  3.  Create the connection between the Presel and Field Group.
 *
 *
 *


cint ../../../../src/examples/dpp/cint_field_fg_create_with_hw_id.c
c
cint_field_stage_presel(0,5);
cint_field_fg_create_single_key(0,2,5);

cint_field_header_type_presel(0,2);
cint_field_fg_create_double_key(0,1,2);
cint_field_presel_example_teardown(0,2,5);
exit;
d chg IHB_PMF_PASS_1_KEY_GEN_LSB

*
*
*

cd ../../../../regress/bcm
tcl
source tests/sand/testDvapiSand.tcl
Logging::LevelSet 10
NTestSeedSet 123456789
test run ResetOnFail::AT_Sand_Cint_field_db_create_with_hw_id_2
AT_Sand_Cint_field_db_create_with_hw_id
bcm shell
diag field res 1
diag pp flp last=1
diag pp sig block=IRPP from=PMF to=TCAM
d chg IHB_PMF_PASS_1_KEY_GEN_LSB
d chg IHB_PMF_PASS_1_KEY_GEN_MSB
d chg IHB_PMF_PASS_2_KEY_GEN_LSB
d chg IHB_PMF_PASS_2_KEY_GEN_MSB

 */

bcm_field_entry_t ent_1;
bcm_field_entry_t ent_2;
bcm_field_group_t group_1;
bcm_field_group_t group_2;
bcm_field_group_t group_3;
bcm_field_group_t group_4;

uint32 cint_fg_with_hw_id_flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_CYCLE | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_CE_ID;

int cint_field_fg_create_entry(int unit, bcm_field_group_t group, bcm_field_action_t action, int action_val, bcm_field_entry_t *ent)
{
    bcm_mac_t macData;
    bcm_mac_t macMask;
    int result;

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

    result = bcm_field_entry_create(unit, group, ent);
    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_entry_create failed (%s)\n", bcm_errmsg(result));
        return result;
    }

    result =  bcm_field_qualify_DstMac(unit, *ent, macData, macMask);
    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_qualify_DstMac failed (%s)\n", bcm_errmsg(result));
        return result;
    }

    result = bcm_field_action_add(unit, *ent, action, action_val, 0);

    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_action_add failed (%s)\n", bcm_errmsg(result));
        return result;
    }
    result = bcm_field_entry_install(unit, *ent);
    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_entry_install failed (%s)\n", bcm_errmsg(result));
        return result;
    }
    printf("FIELD ENTRY CREATED: %d\n-~-~-~-~-~-~-~-~-~-\n", *ent);

    result = bcm_field_entry_dump(unit, *ent);
    if (result != BCM_E_NONE) {
        printf("Error: bcm_field_entry_dump failed (%s)\n", bcm_errmsg(result));
        return result;
    }
    return result;

}

int cint_field_fg_create_single_key(int unit,int num, int presel_id, int dp, int in_ttl)
{
    int ii;
    int auxRes;
    int result;
    int nof_qual = 0;
    int presel_index = 0;
    bcm_field_group_config_extension_t group_config;
    bcm_field_group_presel_info_t presel_info[BCM_FIELD_NOF_PRESEL_PER_FG]; /* Group qualifier hw info, per presel. */
    int nof_presels = BCM_FIELD_NOF_PRESEL_PER_FG;

    bcm_field_group_config_extension_t_init(&group_config);

    bcm_field_group_presel_info_init(nof_presels, presel_info);

    BCM_FIELD_PRESEL_INIT(group_config.preselset);
    BCM_FIELD_PRESEL_ADD(group_config.preselset, presel_id);

    BCM_FIELD_QSET_INIT(group_config.qset);

    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyColor);


    BCM_FIELD_ASET_INIT(group_config.aset);

    group_config.presel_info = presel_info;

    group_config.flags = cint_fg_with_hw_id_flags;
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.cycle = bcmFieldGroupCycle0;
    group_config.mode = bcmFieldGroupModeDouble;
    printf("\n---------------------Running test %s BCM_FIELD_GROUP_CREATE_WITH_CE_ID flag---------------------\n", (group_config.flags & BCM_FIELD_GROUP_CREATE_WITH_CE_ID) ? "with" : "without");
    /* Set the hw ids (ce id) */

    group_config.nof_presel_info = 1;
    presel_info[presel_index].presel_id = presel_id;

    presel_info[presel_index].qual_info[nof_qual].qual = bcmFieldQualifySrcMac;
    presel_info[presel_index].qual_info[nof_qual].nof_ce_ids = 2;
    presel_info[presel_index].qual_info[nof_qual].ce_id[0] = 7;
    presel_info[presel_index].qual_info[nof_qual].ce_id[1] = 19;
    presel_info[presel_index].qual_info[nof_qual].key_index[0] = 0;
    presel_info[presel_index].qual_info[nof_qual].key_index[1] = 0;
    nof_qual++;

    presel_info[presel_index].qual_info[nof_qual].qual = bcmFieldQualifyDstMac;
    presel_info[presel_index].qual_info[nof_qual].nof_ce_ids = 2;
    presel_info[presel_index].qual_info[nof_qual].ce_id[0] = 15;
    presel_info[presel_index].qual_info[nof_qual].ce_id[1] = 27;
    presel_info[presel_index].qual_info[nof_qual].key_index[0] = 0;
    presel_info[presel_index].qual_info[nof_qual].key_index[1] = 0;
    nof_qual++;

    presel_info[presel_index].qual_info[nof_qual].qual = bcmFieldQualifyColor;
    presel_info[presel_index].qual_info[nof_qual].nof_ce_ids = 1;
    presel_info[presel_index].qual_info[nof_qual].ce_id[0] = 3;
    presel_info[presel_index].qual_info[nof_qual].key_index[0] = 0;
    nof_qual++;

    presel_info[presel_index].nof_qual_info = nof_qual;

    presel_index++;
    group_config.nof_presel_info = presel_index;

    for(ii = 0; ii < num ; ii++)
    {
        group_config.priority = ii + presel_id;

        if(ii == 0)
        {
            BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDropPrecedence);

            result = bcm_field_group_config_extension_create(unit,&group_config);
            if (result != BCM_E_NONE) {
                printf("Error in bcm_field_group_create_id\n");
                auxRes = bcm_field_group_destroy(unit, group_config.group);
                return result;
            }
            group_1 = group_config.group;

            result = cint_field_fg_create_entry(unit, group_config.group, bcmFieldActionDropPrecedence, dp, &ent_1);
            if (result != BCM_E_NONE) {
                printf("Error in cint_field_fg_create_entry\n");
                auxRes = bcm_field_group_destroy(unit, group_config.group);
                return result;
            }
        }
        else
        {
            BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionTtlSet);

            result = bcm_field_group_config_extension_create(unit,&group_config);
            if (result != BCM_E_NONE) {
                printf("Error in bcm_field_group_create_id\n");
                auxRes = bcm_field_group_destroy(unit, group_config.group);
                return result;
            }
            group_2 = group_config.group;

            result = cint_field_fg_create_entry(unit, group_config.group, bcmFieldActionTtlSet, in_ttl, &ent_2);
            if (result != BCM_E_NONE) {
                printf("Error in cint_field_fg_create_entry\n");
                auxRes = bcm_field_group_destroy(unit, group_config.group);
                return result;
            }
        }
        printf("\n group %d created \n",group_config.group);

    }

    return result;
}


int cint_field_fg_create_double_key(int unit,int num, int presel_id)
{
    int ii;
    int auxRes;
    int result;
    int nof_qual = 0;
    int presel_index = 0;
    bcm_field_group_config_extension_t group_config;
    bcm_field_group_presel_info_t presel_info[BCM_FIELD_NOF_PRESEL_PER_FG]; /* Group qualifier hw info, per presel. */
    int nof_presels = BCM_FIELD_NOF_PRESEL_PER_FG;

    bcm_field_group_config_extension_t_init(&group_config);

    bcm_field_group_presel_info_init(nof_presels, presel_info);

    BCM_FIELD_PRESEL_INIT(group_config.preselset);
    BCM_FIELD_PRESEL_ADD(group_config.preselset, presel_id);

    BCM_FIELD_QSET_INIT(group_config.qset);

    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcIp6);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyDstIp6);

    BCM_FIELD_ASET_INIT(group_config.aset);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDropPrecedence);

    group_config.presel_info = presel_info;

    group_config.flags = cint_fg_with_hw_id_flags;
    group_config.mode = bcmFieldGroupModeAuto;
    group_config.cycle = bcmFieldGroupCycle0;
    group_config.mode = bcmFieldGroupModeQuad;

    /* Set the hw ids (ce id) */

    group_config.nof_presel_info = 1;
    presel_info[presel_index].presel_id = presel_id;

    presel_info[presel_index].qual_info[nof_qual].qual = bcmFieldQualifySrcMac;
    presel_info[presel_index].qual_info[nof_qual].nof_ce_ids = 3;
    presel_info[presel_index].qual_info[nof_qual].ce_id[0] = 18;
    presel_info[presel_index].qual_info[nof_qual].ce_id[1] = 1;
    presel_info[presel_index].qual_info[nof_qual].ce_id[2] = 11;
    presel_info[presel_index].qual_info[nof_qual].key_index[0] = 0;
    presel_info[presel_index].qual_info[nof_qual].key_index[1] = 0;
    presel_info[presel_index].qual_info[nof_qual].key_index[2] = 1;
    nof_qual++;

    presel_info[presel_index].qual_info[nof_qual].qual = bcmFieldQualifyColor;
    presel_info[presel_index].qual_info[nof_qual].nof_ce_ids = 1;
    presel_info[presel_index].qual_info[nof_qual].ce_id[0] = 27;
    presel_info[presel_index].qual_info[nof_qual].key_index[0] = 1;
    nof_qual++;

    presel_info[presel_index].qual_info[nof_qual].qual = bcmFieldQualifySrcIp6;
    presel_info[presel_index].qual_info[nof_qual].nof_ce_ids = 4;
    presel_info[presel_index].qual_info[nof_qual].ce_id[0] = 13;
    presel_info[presel_index].qual_info[nof_qual].ce_id[1] = 31;
    presel_info[presel_index].qual_info[nof_qual].ce_id[2] = 5;
    presel_info[presel_index].qual_info[nof_qual].ce_id[3] = 23;
    presel_info[presel_index].qual_info[nof_qual].key_index[0] = 0;
    presel_info[presel_index].qual_info[nof_qual].key_index[1] = 0;
    presel_info[presel_index].qual_info[nof_qual].key_index[2] = 0;
    presel_info[presel_index].qual_info[nof_qual].key_index[3] = 0;
    nof_qual++;

    presel_info[presel_index].qual_info[nof_qual].qual = bcmFieldQualifyDstIp6;
    presel_info[presel_index].qual_info[nof_qual].nof_ce_ids = 4;
    presel_info[presel_index].qual_info[nof_qual].ce_id[0] = 14;
    presel_info[presel_index].qual_info[nof_qual].ce_id[1] = 30;
    presel_info[presel_index].qual_info[nof_qual].ce_id[2] = 6;
    presel_info[presel_index].qual_info[nof_qual].ce_id[3] = 22;
    presel_info[presel_index].qual_info[nof_qual].key_index[0] = 1;
    presel_info[presel_index].qual_info[nof_qual].key_index[1] = 1;
    presel_info[presel_index].qual_info[nof_qual].key_index[2] = 1;
    presel_info[presel_index].qual_info[nof_qual].key_index[3] = 1;
    nof_qual++;

    presel_info[presel_index].nof_qual_info = nof_qual;
    presel_index++;
    group_config.nof_presel_info = presel_index;

    for(ii = 0; ii < num ; ii++)
    {
        group_config.priority = ii + presel_id;

        result = bcm_field_group_config_extension_create(unit,&group_config);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_group_create_id\n");
            auxRes = bcm_field_group_destroy(unit, group_config.group);
            return result;
        }
        printf("\n group %d created \n",group_config.group);

        if(ii == 0)
            group_3 = group_config.group;
        else
            group_4 = group_config.group;
    }

    return result;
}

int cint_field_stage_presel(int unit, int presel_id)
{
    int result;
    int auxRes;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
    bcm_field_stage_t stage =  bcmFieldStageIngress;

/*     Create a presel entity*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        result = bcm_field_presel_create_stage_id(unit, stage, presel_id);

        presel_flags = BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;

        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
            return result;
        }
    } else {
        result = bcm_field_presel_create_id(unit, presel_id);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
    }

 /*    Depending on stage, configure the presel stage and in/out port qualifier*/
    result = bcm_field_qualify_Stage(unit, presel_id | presel_flags, stage);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    return result;
}

int cint_field_header_type_presel(int unit, int presel_id)
{
    int result;
    int auxRes;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
    bcm_field_stage_t stage =  bcmFieldStageIngress;

/*     Create a presel entity*/
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        result = bcm_field_presel_create_stage_id(unit, stage, presel_id);

        presel_flags = BCM_FIELD_QUALIFY_PRESEL | BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;

        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id | presel_flags);
            return result;
        }

    } else {
        result = bcm_field_presel_create_id(unit, presel_id);
        if (result != BCM_E_NONE) {
            printf("Error in bcm_field_presel_create_id\n");
            auxRes = bcm_field_presel_destroy(unit, presel_id);
            return result;
        }
    }

    /*    configure the presel based on header format  qualifier*/
    result = bcm_field_qualify_ForwardingType(unit, presel_id  | presel_flags, bcmFieldForwardingTypeAny);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }
    return result;
}


int cint_field_presel_example_teardown(int unit, int presel_id_1, int  presel_id_2)
{
/*    bcm_error_e result;*/
    int result;
    result = bcm_field_entry_destroy(unit, ent_1);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_entry_destroy ent_1\n");
    }
    result = bcm_field_entry_destroy(unit, ent_2);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_entry_destroy ent2\n");
    }
    result = bcm_field_group_destroy(unit, group_1);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_destroy group_1\n");
    }
    result = bcm_field_group_destroy(unit, group_2);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_destroy group_2\n");
    }
    result = bcm_field_group_destroy(unit, group_3);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_destroy group_2\n");
    }
    result = bcm_field_group_destroy(unit, group_4);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_destroy group_2\n");
    }
    result = bcm_field_presel_destroy(unit, presel_id_1);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_presel_destroy\n");
    }
    result = bcm_field_presel_destroy(unit, presel_id_2);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_presel_destroy\n");
    }
    return result;
}
