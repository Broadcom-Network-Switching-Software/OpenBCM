/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * This cint should be used in conjunction with the soc property mim_num_vsis=32768. 
 * It Completes the configurations required in the field processor in order for the application of MiM with 32K I-SIDs to work. 
 * It creates two direct extraction entries, which are used to carry the I-SID information to the egress on the FTMH extensions. 
 *                                                                                                                             
 * To enable this feature, run mim_32k_isids_setup (mim_num_vsis=32768 must be set)
 * To disable it, run mim_32k_isids_teardown                                                                                                                                                                           .
 */

bcm_field_group_t mim_vsi_32k_mode_dir_ext_grp = -1;

int mim_32k_isids_setup(int unit) {

    bcm_error_t rv;
    bcm_field_group_config_t config;
    bcm_field_entry_t ent;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_pcp;


    bcm_field_group_config_t_init(&config);
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_pcp);

    /*To support two VLAN EDIT CMD 62&63 in 32k mode, and user should set SOC "custom_feature_mim_two_ive_reserved" */
    if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"mim_two_ive_reserved",0)){
        /* modify the INLIF profile of the INLIF to oxe*/
        bshell(unit,"mod IHP_LIF_TABLE_AC_MP[0] 12288 1 IN_LIF_PROFILE=0xe");
    }

    if (soc_property_get(unit, spn_MIM_NUM_VSIS, 0) == 32768) {        

        BCM_FIELD_QSET_ADD(config.qset, bcmFieldQualifyInterfaceClassVPort);
        BCM_FIELD_QSET_ADD(config.qset, bcmFieldQualifyStageIngress);
        BCM_FIELD_QSET_ADD(config.qset, bcmFieldQualifyTranslatedInnerVlanId);
        BCM_FIELD_ASET_ADD(config.aset, bcmFieldActionOuterVlanPrioNew);
        BCM_FIELD_ASET_ADD(config.aset, bcmFieldActionVlanActionSetNew);
        config.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
        config.priority = 2;
        config.mode = bcmFieldGroupModeDirectExtraction;
        rv = bcm_field_group_config_create(unit, &config);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_group_config_create\n");
            print rv;
        }
        rv =  bcm_field_entry_create(unit, config.group, &ent);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_entry_create\n");
            print rv;
        }
        /*To support two VLAN EDIT CMD 62&63 in 32k mode*/
        if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"mim_two_ive_reserved",0)){
            rv = bcm_field_qualify_InterfaceClassVPort(unit, ent, 0xe,0xe);
        }else{
            rv = bcm_field_qualify_InterfaceClassVPort(unit, ent, 0xf,0xf);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_qualify_InterfaceClassVPort\n");
            print rv;
        }

        extract.action = bcmFieldActionOuterVlanPrioNew;
        extract.bias = 0;
        ext_pcp.flags = 0;
        ext_pcp.bits = 4;
        ext_pcp.lsb = 0;
        ext_pcp.qualifier = bcmFieldQualifyTranslatedInnerVlanId;
        rv =  bcm_field_direct_extraction_action_add(unit,
                                                     ent,
                                                     extract,
                                                     1 /* count */,
                                                     &ext_pcp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_direct_extraction_action_add\n");
            print rv;
        }

        extract.action = bcmFieldActionVlanActionSetNew;
        extract.bias = 48;
        ext_pcp.flags = 0;
        ext_pcp.bits = 4;
        ext_pcp.lsb = 0;
        ext_pcp.qualifier = bcmFieldQualifyInterfaceClassVPort;
        rv = bcm_field_direct_extraction_action_add(unit,
                                                     ent,
                                                     extract,
                                                     1 /* count */,
                                                     &ext_pcp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_direct_extraction_action_add\n");
            print rv;
        }

        rv = bcm_field_group_install(unit, config.group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_group_install\n");
            print rv;
        }

        mim_vsi_32k_mode_dir_ext_grp = config.group;
    }

    return rv;
}


int mim_32k_isids_teardown(int unit) {

    bcm_error_t rv;

    if (mim_vsi_32k_mode_dir_ext_grp != -1) {        

        rv = bcm_field_group_flush(unit, mim_vsi_32k_mode_dir_ext_grp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_group_flush\n");
            print rv;
        }
    }

    return rv;
}

