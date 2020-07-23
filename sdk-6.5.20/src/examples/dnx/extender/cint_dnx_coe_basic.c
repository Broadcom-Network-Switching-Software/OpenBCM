/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_coe_basic.c
 * Purpose: Example of COE basic configuration.
 */

/*
 * This cint demonstrate configuration of COE.
 *
 * SOC:
 *      ucode_port_0.BCM8869X=CPU.0:core_0.0
 *      ucode_port_200.BCM8869X=CPU.8:core_1.200
 *      ucode_port_201.BCM8869X=CPU.16:core_0.201
 *      ucode_port_202.BCM8869X=CPU.24:core_0.202
 *      ucode_port_203.BCM8869X=CPU.32:core_0.203
 *
 * Usage:
 *
 * cint ../../../../src/examples/sand/cint_port_extender_dynamic_switching.c
 * cint ../../../../src/examples/dnx/extender/cint_dnx_coe_basic.c
 * cint
 * int unit=0;
 * uint32 pp_port, flags;
 * bcm_port_interface_info_t interface_info;
 * bcm_port_mapping_info_t mapping_info;
 *
 * non_coe_port=200;
 * non_coe_vlan=5;
 * print bcm_port_get(unit,201,&flags,&interface_info,&mapping_info);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(0,201,mapping_info.pp_port,10,0);
 * print bcm_port_get(unit,202,&flags,&interface_info,&mapping_info);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(1,202,mapping_info.pp_port,20,0);
 * print bcm_port_get(unit,203,&flags,&interface_info,&mapping_info);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(2,203,mapping_info.pp_port,30,0);
 * print port_dnx_coe_basic_service(0);
 *
 * print bcm_vlan_gport_add(unit, 15, 201, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * print bcm_vlan_gport_add(unit, 25, 202, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * print bcm_vlan_gport_add(unit, 35, 203, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 */

int non_coe_port;
bcm_vlan_t non_coe_vlan;

/**just to hold lif when each time create P2P service*/
bcm_gport_t g_p2p_lif_id_1;
bcm_gport_t g_p2p_lif_id_2;

 /*
  * Set coe port mapping. 
  */
 int port_dnx_coe_config(int unit, int coe_vlan, bcm_port_t coe_port, int pp_port, bcm_port_t phy_port)
 {
     int rv = 0;
     bcm_port_extender_mapping_info_t mapping_info;
     bcm_gport_t sys_gport;
 
     mapping_info.vlan = coe_vlan;
     mapping_info.phy_port = coe_port;
 
     /* 
      * Map port and COE VLAN ID to in pp port.
      * Add encapsulation VID.
      */
     rv = bcm_port_extender_mapping_info_set(
                unit,
                BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS,
                bcmPortExtenderMappingTypePortVlan,
                &mapping_info);
     if (rv != BCM_E_NONE) {
         printf("bcm_port_extender_mapping_info_set failed $rv\n");
         return rv;
     }
 
     /* 
      * Set the system port of the in pp port
      */
     BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, coe_port);
     rv = bcm_stk_sysport_gport_set(unit, sys_gport, coe_port);
     if (rv != BCM_E_NONE) {
         printf("bcm_stk_sysport_gport_set failed $rv\n");
         return rv;
     }
     
     return rv;
 }


 int port_dnx_create_p2p_service(int unit, int p1, bcm_vlan_t vlan1, int p2, bcm_vlan_t vlan2)
 {
     int rv;
     bcm_vlan_port_t vlan_port_1;
     bcm_vlan_port_t vlan_port_2;
     bcm_vswitch_cross_connect_t gports;
     
     bcm_vlan_port_t_init(&vlan_port_1);
     bcm_vlan_port_t_init(&vlan_port_2);
     vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
     vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;

     vlan_port_1.flags = 0;
     vlan_port_2.flags = 0;
     
     vlan_port_1.port = p1;
     vlan_port_1.match_vlan = vlan1;
     
     vlan_port_2.port = p2;
     vlan_port_2.match_vlan = vlan2;
     
     rv = bcm_vlan_port_create(unit, &vlan_port_1);
     if (rv != BCM_E_NONE) {
         printf("bcm_vlan_port_create failed!\n");
         return rv;
     }

     rv = bcm_vlan_port_create(unit, &vlan_port_2);
     if (rv != BCM_E_NONE) {
         printf("bcm_vlan_port_create failed!\n");
         return rv;
     }

 
     bcm_vswitch_cross_connect_t_init(&gports);
     gports.port1= vlan_port_1.vlan_port_id;
     gports.port2= vlan_port_2.vlan_port_id;
     rv = bcm_vswitch_cross_connect_add(unit, &gports);
     if (rv != BCM_E_NONE) {
         printf("P2P service create failed!\n");
         return rv;
     }
     g_p2p_lif_id_1 = vlan_port_1.vlan_port_id;
     g_p2p_lif_id_2 = vlan_port_2.vlan_port_id;
        
     return rv;

 }


 int port_dnx_coe_basic_service(int unit)
 {
    int rv = 0;
    int index;

    /*
     * init COE mapping info(at least 3 COE port)
     * Calling port_extender_dynamic_swithing_coe_info_init_multi_flows() for each coe port mapping.
     */

    /** enable port and set mapping*/
    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 1);
            if (rv != BCM_E_NONE) {
                printf("Enable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
            
            rv = port_dnx_coe_config(
                        unit,
                        coe_port_info_multi_flows[index].coe_vlan,
                        coe_port_info_multi_flows[index].coe_port,
                        coe_port_info_multi_flows[index].pp_port,
                        coe_port_info_multi_flows[index].phy_port);
            if (rv != BCM_E_NONE) {
                printf("Mapping for COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }

    /*
     * create P2P service 
     *      1: coe_port <---> coe_port
     *      2: regular port <---> coe_port
     */
    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[0].coe_port,
            coe_port_info_multi_flows[0].coe_vlan+5,
            coe_port_info_multi_flows[1].coe_port,
            coe_port_info_multi_flows[1].coe_vlan+5);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for COE<-->COE failed!\n");
        return rv;
    }

    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[2].coe_port,
            coe_port_info_multi_flows[2].coe_vlan+5,
            non_coe_port,
            non_coe_vlan);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for COE<-->Regular failed!\n");
        return rv;
    }
    return rv;
 }


int port_dnx_coe_basic_service_clean_up(int unit)
{
    int rv = 0;
    int index;

    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 0);
            if (rv != BCM_E_NONE) {
                printf("Disable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }


    return rv;
}


/**
 * Application on multicore device.
 *  packet flow:
 *   tx_pkt --> cpu_coe_port --p2p--> nif_coe_port ---------------->|
 *                                                                  |
 *                                                              phy_loopback
 *                                                                  |
 *   rx_pkt <-- cpu_coe_port <--p2p-- nif_coe_port <----------------|
 *
 *  SoC property setting:
 *      cpu_coe_port & nif_coe_port on different cores with same PTC
 */
int port_dnx_coe_multi_core_service(int unit)
{
    int rv = 0;
    int index;
    char cmd[300] = {0};

    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 1);
            if (rv != BCM_E_NONE) {
                printf("Enable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
            /** pp_port and phy_port is useless now*/
            rv = port_dnx_coe_config(
                    unit, 
                    coe_port_info_multi_flows[index].coe_vlan,
                    coe_port_info_multi_flows[index].coe_port,
                    0,
                    0);
            if (rv != BCM_E_NONE) {
                printf("port_dnx_coe_config for port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }

    /*
     * P2P service:
     *      cpu_coe_port init at coe_port_info_multi_flows[1,0]
     *      nif_coe_port init at coe_port_info_multi_flows[3,2]
     */
    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[0].coe_port,
            coe_port_info_multi_flows[0].coe_vlan+5,
            coe_port_info_multi_flows[2].coe_port,
            coe_port_info_multi_flows[2].coe_vlan+5);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for cpu_port<-->nif_coe_port failed 1!\n");
        return rv;
    }

    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[1].coe_port,
            coe_port_info_multi_flows[1].coe_vlan+5,
            coe_port_info_multi_flows[3].coe_port,
            coe_port_info_multi_flows[3].coe_vlan+5);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for cpu_port<-->nif_coe_port failed 2!\n");
        return rv;
    }

    /** nif ports on same interface, only set one nif_coe_port to loopback */
    sprintf(cmd, "port LooPBack pbmp=eth%d mode=phy", coe_port_info_multi_flows[2].coe_port);
    bshell(unit, cmd);

    return rv;
}


int port_dnx_coe_multi_core_service_clean_up(int unit)
{
    int rv;
    rv = port_dnx_coe_basic_service_clean_up(unit);
    if (rv != BCM_E_NONE) {
        printf("port_dnx_coe_multi_core_service_clean_up failed!\n");
        return rv;
    }
    return rv;
}



/*
 * EPMF and COE port setting for Injected packet with only IMTH:
 *   print cint_tm_to_coe_main_config(0);  
 */
uint32 pmf_profile = 3;
bcm_field_presel_t presel_id = 1;

int cint_tm_to_coe_context_create(
    int unit,
    int app_type,
    bcm_field_context_t *context_id)
{
    void *dest_char;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    int rv;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "TM_to_COE_ePMF_CTX", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = pmf_profile;
    p_data.qual_data[0].qual_mask = 0x7;

    p_data.qual_data[1].qual_type = bcmFieldQualifyContextId;
    p_data.qual_data[1].qual_arg = 0;
    if (app_type == 0) 
    {
        p_data.qual_data[1].qual_value = bcmFieldForwardContextTm;
    }
    else if (app_type == 1)
    {
        p_data.qual_data[1].qual_value = bcmFieldForwardContextIngressTrapLegacy;
    }
    else if (app_type == 2)
    {
        p_data.qual_data[1].qual_value = bcmFieldForwardContextMirrorOrSs;
    }

    p_data.qual_data[1].qual_mask = 0x3F;
    
    /** PPH doesn't exist */
    /*
    p_data.qual_data[1].qual_type = bcmFieldQualifyPphPresent;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = 0;
    p_data.qual_data[1].qual_mask = 1;
    */

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }

    return rv;
}

int cint_tm_to_coe_fp_create(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_group_t * fg_id,
    bcm_field_group_info_t * fg_info,
    uint32 *ace_entry_handle)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id;
    int qual_index, action_index;
    void *dest_char;
    int rv;

    /** define ace action */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }
    
    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextTmToCOE;
    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    /** Field Group: to set Context value */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->stage = bcmFieldStageEgress;
    fg_info->nof_quals = 1;
    fg_info->qual_types[0] = bcmFieldQualifyDstPort;

    fg_info->nof_actions = 1;
    fg_info->action_types[0] = bcmFieldActionAceEntryId;
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "ACE ctxt value: TM to COE", sizeof(fg_info->name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add failed - Map SSP to TRAP\n");
        return rv;
    }

    /** attach program to given context */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info->nof_quals;
    attach_info.payload_info.nof_actions = fg_info->nof_actions;

    for (qual_index = 0; qual_index < fg_info->nof_quals; qual_index++)
    {
        attach_info.key_info.qual_types[qual_index] = fg_info->qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type = bcmFieldInputTypeMetaData;
    }

    for (action_index = 0; action_index < fg_info->nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] = fg_info->action_types[action_index];
    }
    rv = bcm_field_group_context_attach(unit, 0, *fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach failed \n");
        return rv;
    }
    
    return rv;
}


int cint_tm_to_coe_fp_entry_add(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t *fg_info,
    uint32 ace_entry_handle)
{
    int qual_index, action_index;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;
    int rv;
    
    /** add entry */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info->nof_quals;
    entry_info.nof_entry_actions = fg_info->nof_actions;
    
    for (qual_index = 0; qual_index < fg_info->nof_quals; qual_index++)
    {
        entry_info.entry_qual[qual_index].type = fg_info->qual_types[qual_index];
    }

    for (action_index = 0; action_index < fg_info->nof_actions; action_index++)
    {
        entry_info.entry_action[action_index].type = fg_info->action_types[action_index];
    }
    /** don't care of key */
    entry_info.entry_qual[0].value[0] = 0;
    entry_info.entry_qual[0].mask[0] = 0;
    entry_info.entry_action[0].value[0] = ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    
    return rv;
}



int cint_tm_to_coe_port_config(
    int unit,
    bcm_port_t coe_port)
{
    int rv;

    /** set epmf selection: pmf_profile(TM_to_CoE_Temp) */
    rv = bcm_port_class_set(unit, coe_port, bcmPortClassFieldEgressPacketProcessingPortCs, pmf_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_class_set failed\n");
        return rv;
    }

    return rv;
}


/*
 * Main configuration
 * app_type == 2: config for Mirror application
 * app_type == 1: config for Trap application
 * app_type == 0: config for TM application
 */
int cint_tm_to_coe_main_config(int unit, int app_type)
{
    bcm_field_context_t context_id;
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    uint32 ace_entry_handle;
    int rv, index;

    /** PMF configure */
    rv = cint_tm_to_coe_context_create(unit, app_type, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_tm_to_coe_context_create failed\n");
        return rv;
    }
    rv = cint_tm_to_coe_fp_create(unit, context_id, &fg_id, &fg_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_tm_to_coe_fp_create failed\n");
        return rv;
    }
    rv = cint_tm_to_coe_fp_entry_add(unit, fg_id, &fg_info, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_tm_to_coe_fp_entry_add failed\n");
        return rv;
    }

    /** COE port setting */
    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = cint_tm_to_coe_port_config(unit, coe_port_info_multi_flows[index].coe_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error in cint_tm_to_coe_port_config failed, COE port[%d]\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }
    return rv;

}


bcm_gport_t mirror_dest_id;

/*
 * ingress mirror to COE port should be different: need to append new system header
 * flag: BCM_MIRROR_PORT_INGRESS / BCM_MIRROR_PORT_EGRESS
 */
int cint_dnx_mirror_to_coe_mirror_create(int unit, bcm_port_t mir_src, bcm_port_t mir_dest, int flags)
{
    int rv;
    bcm_mirror_destination_t dest;
    bcm_mirror_header_info_t mirror_header_info;

    bcm_mirror_destination_t_init(&dest);

    /* check the flags parameter, it must be BCM_MIRROR_PORT_INGRESS or BCM_MIRROR_PORT_EGRESS */
    if (flags != BCM_MIRROR_PORT_INGRESS && flags != BCM_MIRROR_PORT_EGRESS)
    {
        printf("Only support flag: BCM_MIRROR_PORT_INGRESS  or BCM_MIRROR_PORT_EGRESS\n");
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(mir_dest))
    {
        dest.gport = mir_dest;
    }
    else
    {
        BCM_GPORT_LOCAL_SET(dest.gport, mir_dest);
    }

    rv = bcm_mirror_destination_create(unit, &dest);
    if(rv != BCM_E_NONE)
    {
        printf("failed to create mirror destination , return value %d\n", rv);
        return rv;
    }
    
    mirror_dest_id = dest.mirror_dest_id;
    
    rv = bcm_mirror_port_dest_add(unit, mir_src, flags, mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("failed to add mir_src in bcm_mirror_port_dest_add\n");
        return rv;
    }

    /** for ingress mirror, should build new system header */
    if (flags == BCM_MIRROR_PORT_INGRESS) {
        bcm_mirror_header_info_t_init(&mirror_header_info);
        mirror_header_info.tm.src_sysport = mir_src;

        rv = bcm_mirror_header_info_set(unit, 0, mirror_dest_id, &mirror_header_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error: bcm_mirror_header_info_set %d\n", rv);
            return rv;
        }
    }

    return rv;
}


int cint_dnx_mirror_to_coe_mirror_destroy(int unit, int mir_src, uint32 flags)
{
    int rv;

    rv = bcm_mirror_port_dest_delete(unit, mir_src, flags, mirror_dest_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_destroy\n");
        return rv;
    }

    rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_destroy\n");
        return rv;
    }

    return rv;
}


/*
 *  Config L3 service(ipv4/ipv6/mpls) for mirror
 */
int cint_dnx_mirror_to_coe_l3_config(int unit, int vsi, int out_port)
{
    int rv;

    rv = inlif_large_wide_data_l3_fwd_config(unit, vsi, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in inlif_large_wide_data_l3_fwd_config\n", rv);
        return rv;
    }

    return rv;
}



/*
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/cint_port_extender_dynamic_switching.c
 * cint ../../src/./examples/dnx/extender/cint_dnx_coe_basic.c
 * cint ../../src/./examples/dnx/cint_inlif_wide_data.c
 * cint
 * non_coe_port=16;
 * non_coe_vlan=5;
 * port_extender_dynamic_swithing_coe_info_init_multi_flows(0,200,24,10,0);
 * port_extender_dynamic_swithing_coe_info_init_multi_flows(1,14,1,20,14);
 * port_extender_dynamic_swithing_coe_info_init_multi_flows(2,15,2,30,15);
 * port_dnx_coe_basic_service(0);
 * bcm_vlan_gport_add(0,100,201,BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * cint_dnx_mirror_to_coe_main(0,100,201,200,13,BCM_MIRROR_PORT_INGRESS);
 * exit;
 *
 * Sending IPv4 forward packet for ingress mirror
 * tx 1 psrc=201 data=0x000c0002000100000000002281000064080045000035000000008000fb1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000c000200010000000000228100000a81000064080045000035000000008000fb1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending IPv6 forward packet for ingress mirror
 * tx 1 psrc=201 data=0x000c000200010000000000228100006486dd6000000000210680fe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000c000200010000000000228100000a8100006486dd6000000000210680fe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending mpls forward packet for ingress mirror
 * tx 1 psrc=201 data=0x000c00020001000000000022810000648847003e83ff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000c000200010000000000228100000a810000648847003e83ff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * cint
 * cint_dnx_mirror_to_coe_mirror_destroy(0,201,BCM_MIRROR_PORT_INGRESS);
 * cint_dnx_mirror_to_coe_mirror_create(0,13,200,BCM_MIRROR_PORT_EGRESS);
 * exit;
 *
 * Sending IPv4 forward packet for egress mirror
 * tx 1 psrc=201 data=0x000c0002000100000000002281000064080045000035000000008000fb1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000102030405000c000200028100000a810000c8080045000035000000007f00fc1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending IPv6 forward packet for egress mirror
 * tx 1 psrc=201 data=0x000c000200010000000000228100006486dd6000000000210680fe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000102030405000c000200028100000a810000c886dd600000000021067ffe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending mpls forward packet for egress mirror
 * tx 1 psrc=201 data=0x000c00020001000000000022810000648847003e83ff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000102030405000c000200028100100a810010c88847007d03fe000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 */

/**Config for L3 service mirror test*/
int cint_dnx_mirror_to_coe_main(int unit, int vsi, bcm_port_t mir_src, bcm_port_t mir_dest, bcm_port_t fwd_dest, int flags)
{
    int rv;
    
    rv = cint_dnx_mirror_to_coe_l3_config(unit, vsi, fwd_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_dnx_mirror_to_coe_l3_config\n", rv);
        return rv;
    }

    rv = cint_dnx_mirror_to_coe_mirror_create(unit, mir_src, mir_dest, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_dnx_mirror_to_coe_mirror_create\n", rv);
        return rv;
    }

    rv = cint_tm_to_coe_main_config(unit, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_tm_to_coe_main_config\n", rv);
        return rv;
    }
    
    return rv;

}


int mep_id = 4;

int cint_dnx_upmep_on_coe_set_counter(int unit, int db_id, int engine_id, int cmd_id, bcm_stat_counter_interface_type_t intf_type)
{
    int rv;
    int flags = 0;
    int i = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_engine_t engine;
    bcm_stat_counter_enable_t enable_config;
    bcm_stat_counter_interface_t interface;
    bcm_stat_counter_set_map_t ctr_set_map;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion_select;
    bcm_stat_expansion_data_mapping_t data_mapping_array[1];
    int type_id = 0; /**Configure only one type*/

    bcm_stat_counter_interface_key_t_init(&interface_key);
    bcm_stat_expansion_select_t_init(&expansion_select);
    expansion_select.nof_elements = 0;
    interface_key.command_id = cmd_id;
    interface_key.core_id = 0;
    interface_key.interface_source = intf_type;
    interface_key.type_id = type_id;
    rv = bcm_stat_counter_expansion_select_set(unit, flags, &interface_key, &expansion_select);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_counter_expansion_select_set\n", rv);
        return rv;
    }

    bcm_stat_counter_database_t_init(&database);
    rv = bcm_stat_counter_database_create(unit,BCM_STAT_DATABASE_CREATE_WITH_ID, 0, &db_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_counter_database_create\n", rv);
        return rv;
    }
    database.database_id = db_id;
    database.core_id = 0;

    bcm_stat_engine_t_init(&engine);
    engine.core_id = 0;
    engine.engine_id = engine_id;
    rv = bcm_stat_counter_engine_attach(unit, flags, &database,engine.engine_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_counter_engine_attach\n", rv);
        return rv;
    }

    bcm_stat_counter_interface_t_init(&interface);
    interface.source = intf_type;
    interface.command_id = cmd_id;
    interface.format_type = bcmStatCounterFormatDoublePackets;
    interface.counter_set_size = 1;
    interface.type_config[0].valid = TRUE;
    interface.type_config[0].object_type_offset = 0;
    interface.type_config[0].start = 0;
    interface.type_config[0].end = 0x1fff;
    rv = bcm_stat_counter_interface_set(unit, flags, &database, &interface);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_counter_interface_set\n", rv);
        return rv;
    }
    
    bcm_stat_counter_set_map_t_init(&ctr_set_map);
    bcm_stat_expansion_data_mapping_t_init(&data_mapping_array);
    data_mapping_array[0].nof_key_conditions = 0;
    data_mapping_array[0].value.counter_set_offset = 0;
    data_mapping_array[0].value.valid = TRUE;
    ctr_set_map.expansion_data_mapping = data_mapping_array;
    ctr_set_map.nof_entries = 1;
    rv = bcm_stat_counter_set_mapping_set(unit, flags, &database, &ctr_set_map);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_counter_set_mapping_set\n", rv);
        return rv;
    }

    bcm_stat_counter_enable_t_init(&enable_config);
    enable_config.enable_counting = TRUE;
    enable_config.enable_eviction = FALSE; 
    rv = bcm_stat_counter_database_enable_set(unit, flags, &database, &enable_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_counter_set_mapping_set\n", rv);
        return rv;
    }
    
    return rv;
}


int cint_dnx_upmep_on_coe_trap_to_cpu(int unit, int dest_port, int profile_id, int opcode)
{
    int rv = 0;
    int new_trap_id = 0;
    int trap_id;

    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    bcm_rx_trap_config_t trap_config;

    bcm_oam_action_key_t_init(&action_key);
    bcm_oam_action_result_t_init(&action_result);
    bcm_rx_trap_config_t_init(&trap_config);


    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = opcode;
    action_key.endpoint_type = bcmOAMMatchTypeMEP;

    if(opcode ==1)
    {
        action_key.dest_mac_type = bcmOAMDestMacTypeMcast;
    }
    else
    {
        action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    }

    action_result.flags = 0;
    action_result.counter_increment = 0;
    action_result.meter_enable = 0;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &new_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error (%d), in bcm_rx_trap_type_create for bcmRxTrapUserDefine\n", rv);
        return rv;
    }

    trap_config.dest_port = dest_port; 
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    rv =  bcm_rx_trap_set(unit, new_trap_id, &trap_config);
    if (rv != BCM_E_NONE) 
    {
        printf("Error (%d), in bcm_rx_trap_type_create for bcmRxTrapUserDefine\n", rv);
        return rv;
    }
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, new_trap_id, 10, 0);

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPDest1, &trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error (%d), in bcm_rx_trap_type_get for bcmRxTrapEgTxOamUpMEPDest1\n", rv);
        return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) 
    {
        printf("Error (%d), in bcm_rx_trap_set for bcmRxTrapEgTxOamUpMEPDest1\n", rv);
        return rv;
    }
    BCM_GPORT_TRAP_SET(action_result.destination, trap_id, 10, 0);

    rv = bcm_oam_profile_action_set(unit, 0, profile_id, &action_key, &action_result);
    if (rv != BCM_E_NONE) 
    {
        printf("Error (%d), in bcm_oam_profile_action_set\n", rv);
        return rv;
    }
    return rv; 
}


int cint_dnx_upmep_on_coe_upmep_add(int unit, int mep_id, bcm_gport_t gport, int trap_dest)
{
    int rv;
    bcm_oam_group_info_t group_info_short_ma;
    bcm_oam_group_info_t *group_info;
    bcm_oam_endpoint_info_t mep_acc_info;

    int md_level = 7;
    int port_1;

    int lm_counter_base_id_1;
    uint32 flags= 0;

    bcm_mac_t src_mac_mep = {0x00, 0x01, 0x02, 0x03, 0x04, 0x01};
    bcm_mac_t mac_mep = {0x00, 0x00, 0x00, 0x01, 0x02, 0xff};

    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;

    bcm_oam_group_info_t_init(&group_info_short_ma);
    uint8 short_name[48] = {1, 32,13 , 0xab, 0xcd};
    sal_memcpy(group_info_short_ma.name,short_name,48);
    group_info_short_ma.id = mep_id;
    group_info_short_ma.flags = BCM_OAM_GROUP_WITH_ID; 

    rv = bcm_oam_group_create(unit,&group_info_short_ma);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_oam_group_create\n", rv);
        return rv;
    }

    group_info = &group_info_short_ma;

    bcm_oam_endpoint_info_t_init(&mep_acc_info);
    mep_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    mep_acc_info.gport = gport;
    mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.id = mep_id;
    mep_acc_info.tx_gport = BCM_GPORT_INVALID;
    mep_acc_info.level = md_level;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    mep_acc_info.sampling_ratio = 0;

    mep_acc_info.timestamp_format = soc_property_get(unit , "oam_dm_ntp_enable",1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;

    mep_acc_info.lm_counter_base_id = 8;
    mep_acc_info.lm_counter_if = 0;

    rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
    if (rv != BCM_E_NONE) {
      printf("Error (%d), in bcm_oam_profile_id_get_by_type(ingress) \n", rv);
      return rv;
    }
    
    rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
    if (rv != BCM_E_NONE) {
      printf("Error (%d), in bcm_oam_profile_id_get_by_type(egress)\n", rv);
      return rv;
    }
    
    rv = bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile);
    if (rv != BCM_E_NONE) {
      printf("Error (%d), in bcm_oam_profile_id_get_by_type(accelerated egress) \n", rv);
      return rv;
    }

    rv = bcm_oam_lif_profile_set(unit, 0, gport,ingress_profile, egress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_oam_lif_profile_set\n", rv);
        return rv;
    }

    mep_acc_info.acc_profile_id = acc_egress_profile;

    rv = bcm_oam_endpoint_create(unit,&mep_acc_info);
    if(rv != BCM_E_NONE)
    {
         printf("Error (%d), in bcm_oam_endpoint_create\n", rv);
         return rv;
    }
    printf(">>>Created MEP with id %x\n", mep_acc_info.id);

    int i = 0;
    int opcode[16] = {1,2,3,4,5,33,35,37,39,40,41,42,43,45,46,47};
    for(i = 0;i < 16; i++)
    {
        rv = cint_dnx_upmep_on_coe_trap_to_cpu(unit, trap_dest, mep_acc_info.acc_profile_id, opcode[i]);
        if (rv != BCM_E_NONE) 
        {
            printf("Error (%d), in cint_dnx_upmep_on_coe_trap_to_cpu for OPcode[%d]\n", rv, opcode[i]);
            return rv;
        }
    }
    return rv;

}


/*
 * Config for UP MEP on COE port, packet trapped to CPU.
 * This is simple example of UP-MEP just to check the COE tag isn't include for 2nd pass.
 */
int cint_dnx_upmep_on_coe_main(int unit, bcm_port_t coe_port, bcm_port_t non_coe_port, bcm_port_t trap_dest)
{
    int rv;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int coe_vlan = 10;
    int match_vlan_coe = 15, match_vlan_non_coe = 25;

    /*
     * Config COE port and create P2P servie
     */
    /**coe_port is NIF port, such as 13, then ptc=coe_port*/
    rv = bcm_port_get(unit, coe_port, &flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_get for port[%d]\n", rv, coe_port);
        return rv;
    }
    rv = bcm_port_control_set(unit, coe_port, bcmPortControlExtenderEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Enable COE port[%d] failed!\n", coe_port);
        return rv;
    }
    rv = port_dnx_coe_config(unit, coe_vlan, coe_port, mapping_info.pp_port, coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in port_dnx_coe_config for port[%d]\n", rv, coe_port);
        return rv;
    }
    rv = port_dnx_create_p2p_service(unit, coe_port, match_vlan_coe, non_coe_port, match_vlan_non_coe);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in port_dnx_create_p2p_service\n", rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, match_vlan_coe, coe_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_vlan_gport_add for coe_port\n", rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, match_vlan_non_coe, non_coe_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error (%d), in bcm_vlan_gport_add for non_coe_port\n", rv);
        return rv;
    }


    /*
     * Following config for UP-MEP
     */
    rv = cint_dnx_upmep_on_coe_set_counter(unit, 8, 2, 7, bcmStatCounterInterfaceIngressOam);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_dnx_upmep_on_coe_set_counter for Ingress OAM\n", rv);
        return rv;
    }
    rv = cint_dnx_upmep_on_coe_set_counter(unit, 9, 3, 0, bcmStatCounterInterfaceEgressOam);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_dnx_upmep_on_coe_set_counter for Egress OAM\n", rv);
        return rv;
    }    

    rv = cint_dnx_upmep_on_coe_upmep_add(unit, mep_id, g_p2p_lif_id_1, trap_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_dnx_upmep_on_coe_set_counter for Egress OAM\n", rv);
        return rv;
    }

    /**Disable OAMP*/
    bshell(unit, "m OAMP_MODE_REGISTER TRANSMIT_ENABLE=0");

    printf(">>>Config for Up-MEP successfully\n");
    return rv;
}


/*
 * Application description:
 *  1. COE ports:
 *      PTC-------->coe_port1   ---->   trunk_id_1
 *              |-->coe_port2   ---->   trunk_id_1  (dynamically add/del)
 *              |-->coe_port3   ---->   trunk_id_2
 *              |-->coe_port4   ---->   Not in trunk group
 *
 *  2. P2P service1:
 *          trunk_id_1  <----p2p---->   trunk_id_2
 *
 *  3. P2P service2:
 *          coe_port4   <----p2p---->   Nif_port(phy loopback)
 */ 
bcm_trunk_t p2p_trunk_id_1=10, p2p_trunk_id_2=20;
bcm_gport_t trunk_gport_1, trunk_gport_2;
bcm_vlan_t p2p_match_vlan_1=15, p2p_match_vlan_2=25;

/*
 * Add a member to a Trunk
 */
int cint_dnx_coe_trunk_member_add(int unit, bcm_trunk_t trunk_id, bcm_port_t coe_port)
{
    int rv;
    bcm_trunk_member_t  member;
    bcm_gport_t gport;


    bcm_trunk_member_t_init(member);
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, coe_port);
    member.gport = gport;
    rv = bcm_trunk_member_add(unit, trunk_id, &member);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_trunk_member_add\n", rv);
        return rv;
    }
    return rv;
}


/*
 * Delete a member from a Trunk
 */
int cint_dnx_coe_trunk_member_del(int unit, bcm_trunk_t trunk_id, bcm_port_t coe_port)
{
    int rv;
    bcm_trunk_member_t  member;
    bcm_gport_t gport;


    bcm_trunk_member_t_init(member);
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, coe_port);
    member.gport = gport;
    rv = bcm_trunk_member_delete(unit, trunk_id, &member);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_trunk_member_delete\n", rv);
        return rv;
    }
    return rv;
}


int cint_dnx_coe_trunk_create(int unit, int *trunk_id, bcm_gport_t *trunk_gport, bcm_port_t coe_port)
{
    int rv = 0;
    bcm_trunk_member_t  member;
    bcm_trunk_info_t trunk_info;
    int flags = 0;
    int id = *trunk_id;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_gport_t gport;
    bcm_port_tpid_class_t tpid_class;

    
    BCM_TRUNK_ID_SET(*trunk_id, 0, *trunk_id);
    flags = BCM_TRUNK_FLAG_WITH_ID;
    rv = bcm_trunk_create(unit, flags, trunk_id);
    if (rv != BCM_E_NONE) {
        printf("Create trunk[%d] failed!\n", id);
        return rv;
    }
    BCM_GPORT_TRUNK_SET(*trunk_gport, *trunk_id);

    key.index = 0;
    key.type = bcmSwitchPortHeaderType;
    value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    rv =  bcm_switch_control_indexed_port_set(unit, *trunk_gport, key, value);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_control_indexed_port_set for trunk[%d] failed!\n", id);
        return rv;
    }

    bcm_trunk_info_t_init(trunk_info);
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    
    bcm_trunk_member_t_init(member);
    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, coe_port);
    member.gport = gport;
    rv = bcm_trunk_set(unit, *trunk_id, &trunk_info, 1, &member);
    if (rv != BCM_E_NONE) {
        printf("bcm_trunk_set for trunk[%d] failed!\n", id);
        return rv;
    }

    tpid_class.tag_format_class_id = 8;
    tpid_class.tpid1 = 0x8100;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.port = *trunk_gport;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_tpid_class_set for trunk[%d] failed!\n", id);
        return rv;
    }

    return rv;
}


int cint_dnx_coe_trunk_service(int unit)
{
    bcm_trunk_member_t member;
    char cmd[300] = {0};

    int flags = 0;

    int rv = 0;
    int index;
    bcm_trunk_t trunk_id_1, trunk_id_2;


    /** enable port and set mapping, at least 4 coe_port*/
    for (index = 0; index < 4; index++) 
    {
        rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("Enable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
            return rv;
        }
        
        rv = port_dnx_coe_config(
                    unit,
                    coe_port_info_multi_flows[index].coe_vlan,
                    coe_port_info_multi_flows[index].coe_port,
                    0,
                    0);
        if (rv != BCM_E_NONE) {
            printf("Mapping for COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
            return rv;
        }
    }

    /*
     *  coe_port_info_multi_flows[0] & coe_port_info_multi_flows[1] in p2p_trunk_id_1
     */
    rv = cint_dnx_coe_trunk_create(unit, &p2p_trunk_id_1, &trunk_gport_1, coe_port_info_multi_flows[0].coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_create!\n");
        return rv;
    }    
    rv = cint_dnx_coe_trunk_member_add(unit, p2p_trunk_id_1, coe_port_info_multi_flows[1].coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_member_add!\n");
        return rv;
    }

    /*
     *  coe_port_info_multi_flows[2] in p2p_trunk_id_1
     */
    rv = cint_dnx_coe_trunk_create(unit, &p2p_trunk_id_2, &trunk_gport_2, coe_port_info_multi_flows[2].coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_create!\n");
        return rv;
    }

    /**p2p service: trunk1<--> trunk2*/
    rv = port_dnx_create_p2p_service(unit, trunk_gport_1, p2p_match_vlan_1, trunk_gport_2, p2p_match_vlan_2);
    if (rv != BCM_E_NONE) {
        printf("Error in port_dnx_create_p2p_service for trunk_gport_1<--->trunk_gport_2!\n");
        return rv;
    }

    /**create P2P coe_port_4<--> NIF port*/
    rv = port_dnx_create_p2p_service(unit, coe_port_info_multi_flows[3].coe_port, p2p_match_vlan_1, non_coe_port, p2p_match_vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error in port_dnx_create_p2p_service for coe_port<--->NIF_port!\n");
        return rv;
    }
    /**set phy loopback on non_coe_port*/
    sprintf(cmd, "port LooPBack pbmp=eth%d mode=phy", non_coe_port);
    bshell(unit, cmd);

    rv = bcm_vlan_gport_add(unit, p2p_match_vlan_1, trunk_gport_1, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_gport_add for trunk_gport_1!\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, p2p_match_vlan_2, trunk_gport_2, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_gport_add for trunk_gport_2!\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, p2p_match_vlan_1, coe_port_info_multi_flows[3].coe_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_gport_add for coe_port_4!\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, p2p_match_vlan_1, non_coe_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_gport_add for non_coe_port!\n");
        return rv;
    }

    return rv;
}


/*
 * Application description:
 *  1. COE ports:
 *      PTC-------->coe_port1   ---->   trunk_id_1          ----> in MC group 
 *              |-->coe_port2   ---->   trunk_id_1          ----> in MC group (dynamically add/del)
 *              |-->coe_port3   ---->   Not in trunk group  ----> in MC group
 *              |-->coe_port4   ---->   Not in trunk group(tx port)
 *
 *  2. service1:
 *          coe_port4  ----ingress MC---->   trunk_id_1 (coe_port2 add/del) + coe_port3
 *
 *  3. service2:
 *          coe_port4  ----egress MC---->   trunk_id_1 (coe_port2 add/del) + coe_port3
 */
bcm_multicast_t mcid_ing=100, mcid_egr=200;
bcm_vlan_t vid_ing=101, vid_egr=201;

int cint_dnx_coe_trunk_multicast_config(int unit, int is_ingress_mc)
{
    int rv = 0;
    int flags = 0;
    bcm_vlan_t vid = is_ingress_mc ? vid_ing : vid_egr;
    bcm_multicast_t mcid = is_ingress_mc ? mcid_ing : mcid_egr;
    bcm_mac_t mc_mac = {0x01,0x00,0x5e,0x00,0x00,0x01};
    bcm_l2_addr_t l2addr;

    bcm_multicast_replication_t rep_array;

    /**Add MC address*/
    if (!is_ingress_mc) {
        mc_mac[5] = 0x02;
    }
    bcm_l2_addr_t_init(&l2addr, mc_mac, vid);
    l2addr.l2mc_group = mcid;
    l2addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l2_addr_add for %s!\n", is_ingress_mc?"Ingress MC":"Egress MC");
        return rv;
    }

    /**config MC*/
    flags = BCM_MULTICAST_WITH_ID;
    flags |= (is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP);
    rv = bcm_multicast_create(unit, flags, &mcid);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_create for %s!\n", is_ingress_mc?"Ingress MC":"Egress MC");
        return rv;
    }

    /**add trunk + coe_port to MC group*/    
    bcm_multicast_replication_t_init(&rep_array);
    flags = (is_ingress_mc ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP);
    rep_array.port = trunk_gport_1;
    rv = bcm_multicast_add(unit, mcid, flags, 1, &rep_array);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_add for %s with Trunk!\n", is_ingress_mc?"Ingress MC":"Egress MC");
        return rv;
    }
    
    rep_array.port = coe_port_info_multi_flows[2].coe_port;
    rv = bcm_multicast_add(unit, mcid, flags, 1, &rep_array);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_add for %s with coe_port!\n", is_ingress_mc?"Ingress MC":"Egress MC");
        return rv;
    }    

    if (!is_ingress_mc) {
        rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mcid, mcid);
        if (rv != BCM_E_NONE) {
            printf("Error in multicast__open_fabric_mc_or_ingress_mc_for_egress_mc!\n");
            return rv;
        }
    }

    rv = bcm_vlan_gport_add(unit, vid, coe_port_info_multi_flows[3].coe_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_gport_add for non_coe_port!\n");
        return rv;
    }

    return rv;
}

int cint_dnx_coe_trunk_multicast_service(int unit)
{
    int rv = 0;
    bcm_trunk_member_t member;
    int index;
    
    /** enable port and set mapping, at least 4 coe_port*/
    for (index = 0; index < 4; index++) 
    {
        rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("Enable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
            return rv;
        }
        
        rv = port_dnx_coe_config(
                    unit,
                    coe_port_info_multi_flows[index].coe_vlan,
                    coe_port_info_multi_flows[index].coe_port,
                    0,
                    0);
        if (rv != BCM_E_NONE) {
            printf("Mapping for COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
            return rv;
        }
    }

    /*
     *  coe_port_info_multi_flows[0] & coe_port_info_multi_flows[1] in p2p_trunk_id_1
     */
    rv = cint_dnx_coe_trunk_create(unit, &p2p_trunk_id_1, &trunk_gport_1, coe_port_info_multi_flows[0].coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_create!\n");
        return rv;
    }    
    rv = cint_dnx_coe_trunk_member_add(unit, p2p_trunk_id_1, coe_port_info_multi_flows[1].coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_member_add!\n");
        return rv;
    }

    
    /**Ingress MC*/
    rv = cint_dnx_coe_trunk_multicast_config(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_multicast_config for Ingress MC!\n");
        return rv;
    }

    /**Egress MC*/
    rv = cint_dnx_coe_trunk_multicast_config(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_multicast_config for Egress MC!\n");
        return rv;
    }

    return rv;
}


/*
 * Application description:
 *  1. NIF COE ports:
 *      PTC-------->coe_port1   ---->   trunk_id_1
 *              |-->coe_port2   ---->   trunk_id_1
 *
 *  2. service1:
 *          tx_port  ----Force forward---->   trunk_id_1 -----|
 *                                                            | phy loopback
 *                                                            v
 *          rx_port  <---Force forward----    trunk_id_1 -----|
 */
int cint_dnx_coe_trunk_check_src_sys_port(int unit, int tx_port, int rx_port)
{
    bcm_trunk_member_t member;
    char cmd[300] = {0};

    int flags = 0;

    int rv = 0;
    int index;

    /** enable port and set mapping, at least 4 coe_port*/
    for (index = 0; index < 4; index++) 
    {
        rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("Enable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
            return rv;
        }
        
        rv = port_dnx_coe_config(
                    unit,
                    coe_port_info_multi_flows[index].coe_vlan,
                    coe_port_info_multi_flows[index].coe_port,
                    0,
                    0);
        if (rv != BCM_E_NONE) {
            printf("Mapping for COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
            return rv;
        }
    }

    /*
     *  coe_port_info_multi_flows[0] & coe_port_info_multi_flows[1] in p2p_trunk_id_1
     */
    rv = cint_dnx_coe_trunk_create(unit, &p2p_trunk_id_1, &trunk_gport_1, coe_port_info_multi_flows[0].coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_create!\n");
        return rv;
    }    
    rv = cint_dnx_coe_trunk_member_add(unit, p2p_trunk_id_1, coe_port_info_multi_flows[1].coe_port);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_dnx_coe_trunk_member_add!\n");
        return rv;
    }

    /**set phy loopback on coe_port*/
    sprintf(cmd, "port LooPBack pbmp=eth%d mode=phy", coe_port_info_multi_flows[0].coe_port);
    bshell(unit, cmd);

    rv = bcm_port_force_forward_set(unit, tx_port, trunk_gport_1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_force_forward_set from tx_port!\n");
        return rv;
    }

    rv = cint_tm_to_coe_main_config(unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_tm_to_coe_main_config!\n");
        return rv;
    }
    rv = cint_tm_to_coe_port_config(unit, trunk_gport_1);
    if (rv != BCM_E_NONE) {
        printf("Error in cint_tm_to_coe_port_config!\n");
        return rv;
    }

    rv = bcm_port_force_forward_set(unit, trunk_gport_1, rx_port, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_force_forward_set to rx_port!\n");
        return rv;
    }

    return rv;
}


