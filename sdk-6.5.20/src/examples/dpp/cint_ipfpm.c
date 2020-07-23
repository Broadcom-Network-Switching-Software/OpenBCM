/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/* File: cint_ipfpm.c
 *
 * SOC: 
 *      field_class_id_size_0=8         -- UDH length, DstClassField
 *      custom_feature_ipfpm_enable=1   -- Enable IPFPM feature
 *      custom_feature_ipfpm_debug=1    -- Open on debug mode
 */

/* function to check if the device is Qumran UX */
int device_is_qux(int unit,
                  uint8* is_qux) {
    bcm_info_t info;
    char *proc_name;

    proc_name = "device_is_qux";
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_info_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" info.device = 0x%x...\n", info.device);
    *is_qux = (info.device == 0x8270);
    if (*is_qux) {
        printf("%s(): Done. Device is QUX.\n", proc_name);
    } else {
        printf("%s(): Done. Device is not QUX.\n", proc_name);
    }
    return rv;
}

int in_ports[] = {13};
int output_port=201;
bcm_field_presel_set_t psset;

bcm_mac_t SrcMac = {0x00,0x00,0x00,0x00,0x00,0x01};
bcm_mac_t DstMac = {0x00,0x00,0x00,0x00,0x00,0x02};
bcm_mac_t MacMask = {0xff,0xff,0xff,0xff,0xff,0xff};
bcm_ip_t SrcIP = 0x64650002;
bcm_ip_t DstIP = 0x15670000;
bcm_ip_t IPMask = 0xffffffff;
bcm_gport_t global_outlif=0;
uint32 value_udh=0x1; /*UDH value to connect ingress and egress PMF*/
uint32 PN_place;

/*
 * PMF config with egress fwd_code to redirect packet
 */
int config_pmf_with_egress_fwd_code(int unit, uint8 fwd_code, int redirect_port)
{
    int rv = 0;
    bcm_field_group_config_t group;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent_v4;
    uint8 data, mask;    
    int sys_gport;

    bcm_field_group_config_t_init(&group);
    group.group = 10;
    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID;
    group.mode = bcmFieldGroupModeAuto;
    group.priority = 10;

    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageEgress);    
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyForwardCode);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);    
    rv = bcm_field_group_config_create(unit, &group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_config_create()\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, group.group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_group_action_set()\n");
        return rv;
    }

    rv = bcm_field_entry_create(unit, group.group, &ent_v4);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_create()\n");
        return rv;
    }

    rv = bcm_field_qualify_ForwardCode(unit, ent_v4, fwd_code, 0xf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_ForwardCode()\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, redirect_port);
    rv = bcm_field_action_add(unit, ent_v4, bcmFieldActionRedirect, 0, sys_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_action_add()\n");
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent_v4);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_install()\n");
        return rv;
    }

    return rv;
}


int config_ipv4_route(int unit, int out_port) {

    int rv = 0;
    int vlan = 100;
    int vrf = 100;
    bcm_l3_intf_t intf;
    bcm_l3_intf_t intf2;
    bcm_l3_intf_t intf_host;
    bcm_l3_intf_t intf_host2;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_ingress_t ingress_intf2;
    bcm_l3_egress_t egress_intf;
    bcm_if_t eg_int1;
    bcm_if_t eg_int2;
    bcm_l3_host_t l3host;
    bcm_l3_route_t l3route;
    int ip_hit1 = 0x15670000;
    int ip_zero = 0x00000000;
    int ip_mask = 0xffffffff;

    bcm_mac_t my_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    bcm_mac_t my_mac2 = { 0x00, 0x0a, 0x00, 0x00, 0x0b, 0x0c };
    bcm_mac_t next_hop_mac = { 0x11, 0x00, 0x00, 0x00, 0x00, 0x11 };
    bcm_mac_t next_hop_mac2 = { 0x22, 0x00, 0x00, 0x00, 0x00, 0x22 };

    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, my_mac, 6);
    intf.l3a_vrf = vrf;
    intf.l3a_vid = vlan;
    rv = bcm_l3_intf_create(unit, &intf);
    if(rv) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_intf.vrf = vrf;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if(rv) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    bcm_l3_egress_t_init(&egress_intf);
    egress_intf.port = out_port;
    sal_memcpy(egress_intf.mac_addr, next_hop_mac, 6);
    egress_intf.intf = intf_host.l3a_intf_id;
    egress_intf.vlan = vlan;
    rv = bcm_l3_egress_create(unit, 0, &egress_intf, &eg_int1);
    if(rv) {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }
    printf("Outlif: %#x\n", egress_intf.encap_id);
    /*local_outlif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port.vlan_port_id);*/
    global_outlif = egress_intf.encap_id;
    
    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = ip_hit1;
    l3route.l3a_ip_mask = ip_mask;
    l3route.l3a_vrf = vrf;
    l3route.l3a_intf = eg_int1;
    rv = bcm_l3_route_add(unit, &l3route);
    if(rv) {
        printf("Error, bcm_l3_route_add Failed, l3a_subnet=0x%x\n", l3route.l3a_subnet);
        return rv;
    }

    return 0;
}


int create_field_presel(int unit)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int presel_id;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    rv = bcm_field_presel_create_id(unit, presel_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_presel_create_id\n");
        return rv;
    }
    
    BCM_PBMP_CLEAR(pbm);
    for (index=0; index<sizeof(in_ports)/sizeof(in_ports[0]); index++) {
        BCM_PBMP_PORT_ADD(pbm, in_ports[index]);
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }
    rv = bcm_field_qualify_InPorts(unit, presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    } 
    
    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);      

    return rv;

}

/*
 * parameter: "pn.4LSB":
 *
 * bit0-1s, bit1-10s, bit2-60s, bit3-600s,
 */

int set_period(int unit, uint32 pn)
{
    int rv = BCM_E_NONE;
    uint32 value;

    value = pn & 0xf;
    
    rv = bcm_field_control_set(0,bcmFieldControlKeyGenVar,value);
    if (rv != BCM_E_NONE) {
        printf("Set period for the device failed\n");
        return rv;
    }

    return rv;
}

void map_counter_per_flow(int unit, uint32 pn, uint32 flow_id, int *counter_id)
{
    *counter_id = (flow_id << 1) | (pn & 1);
}


int create_field_config_upstream_ma(int unit, bcm_field_group_t *group, int group_priority)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_mac_t macData;
    bcm_field_group_config_t grp;
    uint32 tmp_value;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    /*
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstMac);*/
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyKeyGenVar);

    /*
    bcm_field_data_qualifier_t qual_key_gen_var;
    bcm_field_data_qualifier_t_init(&qual_key_gen_var);
    qual_key_gen_var.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    qual_key_gen_var.offset = 0;
    qual_key_gen_var.length = 8;
    qual_key_gen_var.qualifier = bcmFieldQualifyKeyGenVar;
    rv = bcm_field_data_qualifier_create(unit, &qual_key_gen_var);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create: $rv\n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, grp.qset, qual_key_gen_var.qual_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add: $rv\n");
        return rv;
    }
    keygenvar_qual_id = qual_key_gen_var.qual_id;
    */
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
    /*
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);*/

    /*
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeAuto;
    grp.preselset = psset;*/
    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    
    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    *group = grp.group;
    rv = bcm_field_group_action_set(unit, *group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set, destroy the group...\n");
        rv = bcm_field_group_destroy(unit, *group);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_destroy\n");
            return rv;
        }
    }

    rv = bcm_field_control_get(0,bcmFieldControlKeyGenVar,&tmp_value);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_control_get\n");
        return rv;
    }
    PN_place = tmp_value;
    printf("PN set place is [%u] for field group[%d]\n", PN_place, *group);

    return rv;
}

int add_field_entry_upstream_ma(int unit, bcm_field_group_t group, bcm_field_entry_t *entry, uint32 flow_id, uint32 pn, uint32 udh_val)
{
    int rv;
    bcm_field_stat_t stats[2];
    int statId = 0;
    uint32 udh = 0;
    int counter_id;
    uint8 keygenvar_data, keygenvar_mask;
    bcm_port_t port;

    rv = bcm_field_entry_create(unit, group, entry);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }
    
    rv = bcm_field_qualify_SrcIp(unit, *entry, SrcIP, IPMask);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_SrcIp\n");
        return rv;
    }
    rv = bcm_field_qualify_DstIp(unit, *entry, DstIP, IPMask);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_DstIp\n");
        return rv;
    }

    /*just care set bit, so mask=pn*/
    rv = bcm_field_qualify_KeyGenVar(unit, *entry, pn, pn);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_KeyGenVar\n");
        return rv;
    }

    udh = udh_val;
    rv = bcm_field_action_add(unit, *entry, bcmFieldActionClassDestSet, udh, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add for bcmFieldActionClassDestSet\n");
        return rv;
    }
    /*
    BCM_GPORT_LOCAL_SET(port, 200);
    rv = bcm_field_action_add(unit, *entry, bcmFieldActionRedirect, 0, port); 
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add for bcmFieldActionClassDestSet\n");
        return rv;
    }*/

    /* set for counters */
    stats[0] = bcmFieldStatPackets;
    stats[1] = bcmFieldStatBytes;
    map_counter_per_flow(unit, pn, flow_id, &counter_id);
    BCM_FIELD_STAT_ID_SET(&statId, 0, counter_id);
    bcm_field_stat_create_id(unit, group, 2, &(stats[0]), statId); 
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_stat_create_id returned %d\n", rv);
        return rv;
    }
    bcm_field_entry_stat_attach(unit, *entry, statId);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_entry_stat_attach returned %d\n", rv);
        return rv;
    }

    
    rv = bcm_field_entry_install(unit, *entry);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_entry_install returned %d\n", rv);
        return rv;
    }
    return rv;
}

/*
 * entry_pn.4lsb should match the value set for KeyGenVar
 */
 
/*
 * flow_id          -- flowID
 * ip_header_flag   -- indicate which bit to operate(set or reset)
 * entry_pn         -- indicate which PN this flow belong to 
 */

uint32 flow_id=10, entry_pn=8, ip_header_flag=2;

int ace_pointer=3;
uint8 ace_variable=0;   /* 8bits */
/*
 * #define TOS_SET_SEL     0x1
 * #define TOS_RESET_SEL   0x3
 */
uint8 ace_prge_sel=0;   /*2bits*/


int create_egress_field_config_upstream_ma(int unit, bcm_field_group_t *group, int group_priority)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_config_t grp;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstClassField);
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

    grp.priority = group_priority;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeAuto;
    
    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    *group = grp.group;
    rv = bcm_field_group_action_set(unit, *group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set, destroy the group...\n");
        rv = bcm_field_group_destroy(unit, *group);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_destroy\n");
            return rv;
        }
    }
    return rv;
}

int add_egress_field_entry_upstream_ma(int unit, bcm_field_group_t group, bcm_field_entry_t *entry)
{
    int rv;
    bcm_field_action_core_config_t core_config_arr[1] ;
    bcm_gport_t local_gport;

    rv = bcm_field_entry_create(unit, group, entry);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    rv = bcm_field_qualify_DstClassField(unit, *entry, value_udh, 0xff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_petra_field_qualify_DstClassField\n");
        return rv;
    }

    /*ace_prog_sel+ace variable: 10bits at offset 19*/
    /*
     * Set ACE table at line [ace_pointer/2]
     * ace_pointer, decided by yourself, flowID map to ace_pointer(one on one)
     */


    /* UDH signal, we suggest to be flow ID:
     *  |=======================================|
     *  |               Flow ID                 |
     *  |=======================================|
 */
    /*
     * ACE table:
     * if ace_prge_sel = 1, ace_variable[4:0] - set one bit of IP_Header.TOS[4~0]
     * if ace_prge_sel = 3, ace_variable[4:0] - clear one bit of IP_Header.TOS[4~0]
     */
    BCM_GPORT_LOCAL_SET(local_gport, 200);
    if (ace_prge_sel==1) {
        ace_variable = (ip_header_flag & 0x1f);
    } else if (ace_prge_sel==3) {
        ace_variable = (ip_header_flag & 0x1f);
    } else {
        printf("Ace_prge_sel should be 1 or 3 for PMF entry!\n");
        return -1;
    }

    core_config_arr[0].param0 = ace_pointer | ((ace_variable | (ace_prge_sel<<8))<<19);
    core_config_arr[0].param1 = BCM_ILLEGAL_ACTION_PARAMETER;
    core_config_arr[0].param2 = BCM_FORWARD_ENCAP_ID_VAL_GET(global_outlif);
    
    rv = bcm_field_action_config_add(unit, *entry, bcmFieldActionStat, 1, &core_config_arr[0]);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_config_add\n");
        return rv;
    }
    
    rv = bcm_field_entry_install(unit, *entry);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_field_entry_install returned %d\n", rv);
        return rv;
    }
    return rv;
}


int example_upstream_ma(int unit)
{
    int rv;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    rv = config_ipv4_route(unit, output_port);
    if (rv != BCM_E_NONE) {
        printf("Error in : config_ipv4_route\n");
        return rv;
    }


    rv = set_period(unit, 9);
    if (rv != BCM_E_NONE) {
        printf("Error in : set_period\n");
        return rv;
    }

    rv = create_field_config_upstream_ma(unit, &group, 10);
    if (rv != BCM_E_NONE) {
        printf("Error in : create_field_config_upstream_ma\n");
        return rv;
    }

    rv = add_field_entry_upstream_ma(unit, group, &entry, flow_id, entry_pn, value_udh);
    if (rv != BCM_E_NONE) {
        printf("Error in : add_field_entry_upstream_ma\n");
        return rv;
    }
    printf(">>>Create Ingress Group[%d] and install entry[%d] for upstream MA\n", group, entry);
    
    rv = create_egress_field_config_upstream_ma(unit, &group, 10);
    if (rv != BCM_E_NONE) {
        printf("Error in : create_egress_field_config_upstream_ma\n");
        return rv;
    }

    ace_prge_sel = 1;   /*tos_set flag*/
    ip_header_flag = 2; /*set flag bit*/
    rv = add_egress_field_entry_upstream_ma(unit, group, &entry);
    if (rv != BCM_E_NONE) {
        printf("Error in : add_egress_field_entry_upstream_ma\n");
        return rv;
    }
    printf(">>>Create Ingress Group[%d] and install entry[%d] for upstream MA\n", group, entry);
    return rv;
    
}


int example_downstream_ma(int unit)
{
    int rv;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    rv = config_ipv4_route(unit, output_port);
    if (rv != BCM_E_NONE) {
        printf("Error in : config_ipv4_route\n");
        return rv;
    }


    rv = set_period(unit, 12);
    if (rv != BCM_E_NONE) {
        printf("Error in : set_period\n");
        return rv;
    }

    rv = create_field_config_upstream_ma(unit, &group, 10);
    if (rv != BCM_E_NONE) {
        printf("Error in : create_field_config_upstream_ma\n");
        return rv;
    }

    rv = add_field_entry_upstream_ma(unit, group, &entry, flow_id, entry_pn, value_udh);
    if (rv != BCM_E_NONE) {
        printf("Error in : add_field_entry_upstream_ma\n");
        return rv;
    }
    printf(">>>Create Ingress Group[%d] and install entry[%d] for upstream MA\n", group, entry);
    
    rv = create_egress_field_config_upstream_ma(unit, &group, 10);
    if (rv != BCM_E_NONE) {
        printf("Error in : create_egress_field_config_upstream_ma\n");
        return rv;
    }

    ace_prge_sel = 3;   /*tos_clear flag*/
    ip_header_flag = 2; /*clear flag bit*/
    rv = add_egress_field_entry_upstream_ma(unit, group, &entry);
    if (rv != BCM_E_NONE) {
        printf("Error in : add_egress_field_entry_upstream_ma\n");
        return rv;
    }
    printf(">>>Create Ingress Group[%d] and install entry[%d] for Downstream MA\n", group, entry);
    return rv;
}


