/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
'==================================================================================================
'@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Test bed @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'
'     |--------------|             |--------------|                        |--------------|
'     | IXIA port3-5 |             | IXIA port3-7 |                        | IXIA port3-6 |
'     |--------------|             |--------------|                        |--------------|
'           |                              |                                       |
'           |                              |                                       |
'           |-------------|       |--------|                                       |
'                         |       |                                                |
'                 xe44(45)|       | xe46(47)                                       |
'                       |-----------|                                              |
'                       |    TD2    |                                              |
'                       |           |                                              |
'                       |-----------|                                              |
'                             |xe28(29)                                            |
'                             |                                                    ^
'                             v                                                    |
'                             |(Higig2)                                            |
'                             |                                                    |
'                             | xe21/xe22                                          |
'                       |-----------|                                              |
'                       |    QMX    |----------------------->----------------------|
'                       |-----------|(xe13)
'
'==================================================================================================
*/


/**
 * 0. Description
 *
 *      This test support both Higig TM and PP mode.
 *      IXIA port3-7 send PP traffic and mapped to xe22(pp port) on QMX side;
 *      IXIA port3-5 send TM traffic and mapped to xe21(tm port) on QMX side;
 *      IXIA port3-6 send normal traffic to xe13 and out of chip by Higig port xe21
 *      PP traffic is swithed to xe13 through MC while TM traffic forwarded to xe13 according to ITMH(0x01000d00)
 * 
 * 1. SOC config:
 *      ucode_port_21.BCM88675=10GBase-R69.0:core_0.21
 *      ucode_port_22.BCM88675=10GBase-R69.1:core_0.22
 *      ucode_port_13.BCM88675=10GBase-R66.0:core_0.13
 *      ucode_port_14.BCM88675=10GBase-R66.1:core_0.14
 *      ucode_port_15.BCM88675=10GBase-R66.2:core_0.15
 *      tm_port_header_type_in_21.BCM88675= XGS_MAC_EXT
 *      tm_port_header_type_out_21.BCM88675= XGS_MAC_EXT
 *      tm_port_header_type_in_22.BCM88675= XGS_MAC_EXT
 *      tm_port_header_type_out_22.BCM88675= XGS_MAC_EXT
 *      xgs_mac_ext_tm_mode_21.BCM88675=1
 *
 * 2. cint config
 *      cint ../../../../src/examples/dpp/cint_xgs_mac_extender_mapping_tm.c
 *      cint
 *      print xgs_mac_extender_tm_main(unit); exit;
 *
 * 3. create switch service for PP mode
 *      vlan create 20 pbm=xe22,xe13
 *
 *
 * 4. Diag for Tx direction
 *      cint ../../../../src/examples/dpp/internal/cint_mirror.c
 *      cint
 *      print set_port_mirror(unit, 21, 13,
 *      BCM_MIRROR_PORT_EGRESS); exit;
 *
 *      d chg IHP_PTC_INFO  [display the recycle port for egress mirror]
 *      mod IHP_PTC_INFO 2 1 PTC_OUTER_HEADER_START=0 [modify the recycle port's header start to 0]
 *
 *  Then we can capture the mirror packet on IXIA with Higig Header. 
 */


/**/
bcm_port_t port_higig_pp = 22;
bcm_port_t port_higig_tm = 21;
bcm_port_t port_regular = 13;

int        remote_mod_id = 1;
int        local_mod_id = 0;

bcm_port_t remote_port_id_pp = 47;
bcm_port_t remote_port_id_tm = 45;

int        higig_bit_len = 128;
int        itmh_bit_len = 32;


/* only for Higig pp mode */
int xgs_mac_extender_tm_create_service(int unit, int mode)
{
    
}


int xgs_mac_extender_tm_src_port_config(int unit)
{
    bcm_error_t rv;

    /* pp mode config */
    rv = bcm_stk_modport_remote_map_set(unit, port_higig_pp, 0, remote_mod_id, remote_port_id_pp);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_modport_remote_map_set for PP mode failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* tm mode config */
    rv = bcm_stk_modport_remote_map_set(unit, port_higig_tm, 0, remote_mod_id, remote_port_id_tm);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_modport_remote_map_set for TM mode failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


int xgs_mac_extender_tm_pmf_config(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_t presel_id = 0;
    int presel_flags = 0;
    bcm_field_presel_set_t psset;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_group_config_t group;
    bcm_field_aset_t aset;

    bcm_field_entry_t entry;
    bcm_field_extraction_field_t ext_field;
    bcm_field_extraction_action_t ext_action;

    /* A. preselector: create PMF program */
    BCM_FIELD_PRESEL_INIT(psset);

    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
            rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    } else {
            rv = bcm_field_presel_create(unit, &presel_id);
    }
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_presel_create_id failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_Stage failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    /* we can also use "bcm_field_qualify_InPorts()" */
    rv = bcm_field_qualify_InPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, port, -1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_SrcPort failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    BCM_FIELD_PRESEL_ADD(psset, presel_id);
    printf("Setting pre-selector successfully!\n");

    /* B. qualifier, define my own qualifier */
    bcm_field_data_qualifier_t_init(&data_qual);
    /*data_qual.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_WITH_ID; */
    /* data_qual.flags =  BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; */
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;/*start from the beginning of pkt*/
    /* data_qual.offset = (higig_bit_len + 8); */
    data_qual.offset = 17;
    data_qual.length = 2; 
    /*data_qual.qual_id = 58;*/
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_data_qualifier_create failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    printf("bcm_field_data_qualifier_create: qual_id[%d] successfully\n", data_qual.qual_id);


    /* C. field group, also called DataBase */
    bcm_field_group_config_t_init(&group);
    /* C.1 Qset */
    BCM_FIELD_QSET_INIT(group.qset);
    /* add user define qualifier to QSET */
    rv = bcm_field_qset_data_qualifier_add(unit, &group.qset, data_qual.qual_id);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qset_data_qualifier_add failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    /* designate the QSET as Ingress(default) */
    
    /* C.2 Aset */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirectPort);
    /* BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip); */

    /* C.3 config field group mode and create */
    group.priority = BCM_FIELD_GROUP_PRIO_ANY;/*unit 0 only supports priority 0..127 or BCM_FIELD_GROUP_PRIO_ANY*/
    /* group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET; */
    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    group.mode = bcmFieldGroupModeDirectExtraction; /* direct-mapping data define */
    group.preselset = psset;

    /* create field group */
    rv = bcm_field_group_config_create(unit, &group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_config_create failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    printf("bcm_field_group_config_create: group_id[%d] - priority[%d] successfully\n", group.group, group.priority);

    /* C.4 attach the action set */
    rv = bcm_field_group_action_set(unit, group.group, aset);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_action_set failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* D. Create Direct Extraction entries */
    /* D.1 create entry */
    rv = bcm_field_entry_create(unit, group.group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }
    /* D.2 construct the entry */    
    bcm_field_extraction_field_t_init(&ext_field);
    ext_field.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_field.qualifier = data_qual.qual_id;
    ext_field.lsb = 0;
    ext_field.bits = 16;
    
    bcm_field_extraction_action_t_init(&ext_action);
    ext_action.action = bcmFieldActionRedirectPort; /*new action*/
    ext_action.bias = 0x10000;  /*prefix for dest-system-port*/

    rv = bcm_field_direct_extraction_action_add(unit, entry, ext_action, 1, &ext_field);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_direct_extraction_action_add failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    /* D.3 install the entry(write to hardware) */
    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_install failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


/* only for TM mode to remove ITMH(4B) */
void xgs_mac_extender_tm_rm_header_size(int unit, bcm_port_t port)
{ 
    /*
    char cmd[300] = {0}; 

    sprintf(cmd, "mod IHP_PTC_INFO %d 1 PTC_OUTER_HEADER_START=0x14", port);
    bshell(unit, cmd);
    */
    bcm_error_t rv = BCM_E_NONE;
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_entry_t entry;
    bcm_field_group_t group;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);

    rv = bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_create failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_action_set(unit, group, aset);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_group_action_set failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_create(unit, group, &entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_create failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, entry, port, -1);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_qualify_SrcPort failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionStartPacketStrip, bcmFieldStartToConfigurableStrip, 4);
    /*rv = bcm_field_action_add(unit, entry, bcmFieldActionStartPacketStrip, bcmFieldStartToL2Strip, 28); */
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_action_add failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_install(unit, entry);
    if (BCM_FAILURE(rv)) {
        printf("bcm_field_entry_install failed. Error:%d (%s)\n", rv, bcm_errmsg(rv));
        return rv;
    }


    return rv;

}


/*
 *  Higig TM mode's main entry
 */
int xgs_mac_extender_tm_main(int unit)
{
    int rv = BCM_E_NONE;    

    /* 0. create service */

    /* 1. Rx configuration */
    
    /* 1.1 set for in_pp_port mapping as pp mode */
    rv = xgs_mac_extender_tm_src_port_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Setting for in pp port mapping failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    printf("Setting for in pp port mapping successfully\n");

    /* 1.2PMF config: direct mapping */
    rv = xgs_mac_extender_tm_pmf_config(unit, port_higig_tm);
    if (BCM_FAILURE(rv)) {
        printf("Setting for PMF(direct-mapping) failed. Error:%d (%s) \n", rv, bcm_errmsg(rv));
        return rv;
    }
    printf("Setting with PMF for port[%d] successfully\n", port_higig_tm);

    /* 1.2 set pkt header to remove, include Higig(16B) and ITMH(4B) */
    /* xgs_mac_extender_tm_rm_header_size(unit, port_higig_tm);
    printf("Setting with removing ITMH for port[%d] successfully\n", port_higig_tm);*/


    /* 2. Tx configuration */

    return rv;
}

