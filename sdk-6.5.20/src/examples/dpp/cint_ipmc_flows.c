/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~IPMC FLOWS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_ipmc_flows.c
 * Purpose: Example of various IPMC flows.
 *
 *
 *        -----------------------                      ------------------                 -------------------------
 *       |  bcmSwitchL3McastL2==1 |  <-- NO  <--------| RIF.IPMC enable? |------> YES --->| ipmc_vpn_lookup_enable |
 *        -----------------------                      ------------------                 -------------------------
 *                    |                                                                                |
 *                    |                                                                                |
 *                    |                                                                                |
 *                    |                                                                                |
 *        ---NO------- -------YES---                                                       ---NO------- -------YES---
 *        |                         |                                                      |                         |
 *        |                         |                                                      |                         |
 *   -----------               -------------------                                   -------------------       -------------------
 *  |LEM<FID,DA>|             | LEM<FID,DIP>      |                                | TCAM<RIF,DIP,SIP> |     | TCAM<RIF,DIP,SIP> |
 *   -----------              |TCAM<FID,DIP,SIP>(optional in A+)                   | LEM<RIF,DIP,SIP>  |     |     LEM<VRF,DIP>  | 
 *                             -------------------                                  -------------------       -------------------
 *        |                         |                                                       |                        |
 *        |        Hit?             |                                                       |                        |
 *         -------------------------                                                        |                        |
 *                     |                                                                    |                        |
 *                     |                                                                     -------------------------
 *                     |                                                                                 |
 *        ---NO-------  -------YES---                                                                    |  Hit?
 *        |                          |                                                                   |
 *        |                          |                                                     ---NO--------- ---------YES----
 *   ------------               -------------                                              |                              |
 *  |L2-FLOODING |             |L2-FORWARDING|                                             |                              |
 *  |   ON VSI   |              -------------                                 ---------------------------------         --------------
 *   ------------                                                            |bcmSwitchUnknownIpmcAsMcast == 1|        |L3-FORWARDING|
 *                                                                            ---------------------------------         --------------
 *                                                                                         |
 *                                                                                         |
 *                                                                              ---NO------- -------YES---
 *                                                                              |                        |
 *                                                                              |                        |
 *                                                                              |                        |
 *                                                                          -------------             ------------
 *                                                                         |L3-FORWARDING|           |L2-FLOODING |
 *                                                                         | VRF DEFAULT |           |   ON VSI   |
 *                                                                          -------------             ------------
 *
 *
 *
 *  Abstract:
 *
 *  When an IPv4oE packet arrives at the ingress with multicast-compatible destination address (that is,
 *  DA[47:23]=={24'h01_00_5E, 1'b0} and Ethernet-Header.DA[22:0] = IP-Header.DIP[22:0] or IPHeader-DIP[31:28] = to 4'hE)
 *  , one of two main flows take place, according to whether RIF.IPMC is enabled.
 *  The flow in which RIF.IPMC is enabled is exemplified in the function ipmc_flows_rif_ipmc_enabled.
 *  The flow in which RIF.IPMC is disabled is exemplified in the function ipmc_flows_rif_ipmc_disabled.
 *
 *  Each function exemplifies sub flows of these two main flows. Each sub flow is defined in a matching
 *  section which also determines a possible traffic flow of a packet.
 *  Each function's header contains exact information required to understand each sub flow and send
 *  a packet according to the sub-flow's stipulations.
 *
 *
 *  We present a pre-configuration for ipmc_flows_rif_ipmc_disabled(), after which we can send traffic
 *  according to sub-flows in this flow. The sub flows are presented in the preamble of the function, each one
 *  exemplified by certain packet sending and receiving.
 *
 *  BCM> cint cint_ipmc_flows.c
 *  BCM> cint
 *  cint> int rv;
 *  cint> int inP = 200;
 *  cint> int outP = 201;
 *  cint> int outP2 = 202;
 *  cint> int outP3= 203;
 *  cint> int vid1 = 4000;
 *  cint> int vid2 = 4001;
 *  cint> int vid3 = 4002;
 *  cint> int vid4 = 4003;
 *  cint> rv = ipmc_flows_rif_ipmc_disabled(unit, inP, outP,outP2 ,outP3 ,vid1, vid2, vid3, vid4);
 *
 * Packet sending for ipmc_flows_rif_ipmc_disabled():
 *
 * 1. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid1);
 *    Then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.11)    dst_ip: 0xe0000102 (224.0.1.2).
 *    Expect it to be forwarded to ports outP, outP2, outP3, with vid == 4000; src_mac, dst_mac, src_ip, dst_ip without change.
 * 
 * 1.1. Valid only in ARAD+ and in case SOC property "ipmc_l2_ssm_mode=1".
 *      Include 3 types of scenarios in SSM:  1. (*,G) forwarding   2. (S,G) forwarding  3. (S,G) drop  
 *      Send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000f (192.168.0.15)    dst_ip: 0xe0000102 (224.0.1.2).
 *      Expect it to be forwarded to ports outP, outP2, outP3, with vid == 4001; src_mac, dst_mac, src_ip, dst_ip without change. 
 *      Send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000e (192.168.0.14)    dst_ip: 0xe0000102 (224.0.1.2).
 *      Expect it to be dropped;
 *      
 *
 * 2. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid2);
 *                  cint> switch_control_set(unit,bcmSwitchL3McastL2,0);
 *    Then, send packet with: src_mac: 00:02:05:00:00:01    dst_mac :01:00:5E:00:01:03    src_ip: 0xc0a8000c (192.168.0.12)    dst_ip: 0xe0000103 (224.0.1.3).
 *    Expect it to be forwarded to ports outP, outP2, outP3, with vid == 4001; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 3. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid3);
 *                  cint> switch_control_set(unit,bcmSwitchL3McastL2,1);
 *    Then, send packet with: src_mac: 00:02:05:00:00:02    dst_mac :01:00:5E:00:01:04    src_ip: 0xc0a8000d (192.168.0.13)    dst_ip: 0xe0000104 (224.0.1.4).
 *    Expect it to be forwarded to ports outP, outP2, outP3, with vid == 4002; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 4. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid4);
 *                  cint> switch_control_set(unit,bcmSwitchL3McastL2,0);
 *    Then, send packet with: src_mac: 00:02:05:00:00:03    dst_mac :01:00:5E:00:01:05    src_ip: 0xc0a8000e (192.168.0.14)    dst_ip: 0xe0000105 (224.0.1.5).
 *    Expect it to be flooded to ports outP, outP2, with vid == 4003; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 *
 *  We present a pre-configuration for ipmc_flows_rif_ipmc_enabled(), after which we can send traffic
 *  according to sub-flows in this flow. The sub flows are presented in the preamble of the function, each one
 *  exemplified by certain packet sending and receiving.
 *
 *  BCM> cint cint_ipmc_flows.c
 *  BCM> cint
 *  cint> int rv;
 *  cint> int inP = 200;
 *  cint> int outP = 201;
 *  cint> int outP2 = 202;
 *  cint> int outP3= 203;
 *  cint> int rif1 = 4000;
 *  cint> int rif2 = 4001;
 *  cint> int rif3 = 4002;
 *  cint> int rif4 = 4003;
 *  cint> int rif5 = 4009;
 *  cint> int vlan = 4005;
 *  cint> rv = ipmc_flows_rif_ipmc_enabled(unit, inP, outP,outP2 ,outP3 , rif1, rif2, rif3, rif4, rif5, vlan);
 *
 * Packet sending for ipmc_flows_rif_ipmc_enabled():
 *
 * 1. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif1);
 *    If ipmc_vpn_lookup_enable ==1, then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.11)    dst_ip: 0xe0000102 (224.0.1.2)
 *    Otherwise(ipmc_vpn_lookup_enable==0), Then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.13)    dst_ip: 0xe0000102 (224.0.1.4)
 *    Both them expect to be forwarded to ports outP, outP2, outP3, with vid == 4000; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 2. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif2);
 *    Then, send packet with: src_mac: 00:02:05:00:00:01    dst_mac :01:00:5E:00:01:03    src_ip: 0xc0a8000c (192.168.0.12)    dst_ip: 0xe0000103 (224.0.1.3).
 *    Expect it to be forwarded to ports outP, outP2, with vid == 4001; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 3. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif3);
 *    Then, send packet with: src_mac: 00:02:05:00:00:01    dst_mac :01:00:5E:00:01:03    src_ip: 0xc0a8000d (192.168.0.13)    dst_ip: 0xe0000103 (224.0.1.3).
 *    Expect it to be forwarded to ports outP2, outP3, with vid == 4002; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 4. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif4);
 *    If ipmc_vpn_lookup_enable ==1, then, send packet with: src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.11)    dst_ip: 0xe0000102 (224.0.1.2).
 *    Otherwise(ipmc_vpn_lookup_enable==0), then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000e (192.168.0.14)    dst_ip: 0xe0000102 (224.0.1.5)
 *    Expect it to be forwarded to ports outP, outP3, with vid == 4003; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 5. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vlan);
 *    Then, send packet with: src_mac: 00:02:05:00:00:02    dst_mac :01:00:5E:00:01:04    src_ip: 0xc0a8000d (192.168.0.13)    dst_ip: 0xe0000104 (224.0.1.4).
 *    Expect it to be flooded to ports outP, outP2, with vid == 4005; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 6. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif5);
 *                  cint> switch_control_set(unit,bcmSwitchUnknownIpmcAsMcast,0);
 *    Then, send packet with: src_mac: 00:02:05:00:00:02    dst_mac :01:00:5E:00:01:04    src_ip: 0xc0a8000e (192.168.0.14)    dst_ip: 0xe0000105 (224.0.1.5).
 *    Expect it to be forwarded to ports outP, outP2, with vid == 4009; src_mac, dst_mac, src_ip, dst_ip without change.
 */


/* Main struct
 * holds data structures relevant for the hereby exemplified flows
 */
struct ipmc_flows_info_s {

    bcm_ip_t mc_ip_arr[4];
    bcm_ip_t src_ip_arr[4];
    int ipmc_indices[8]; /* this array's entries will keep the indices of the groups without change*/
    int ipmc_mc_groups[8]; /* this array's entries will be changed after creating multicast groups*/
    int forwarding_ports_1[3];
    int forwarding_ports_2[2];
    int forwarding_ports_3[2];
    int forwarding_ports_4[2];
};

ipmc_flows_info_s ipmc_flows_info1;

/*my_macs*/
bcm_mac_t mac_address_1 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
bcm_mac_t mac_address_2 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};
bcm_mac_t mac_address_3 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
bcm_mac_t mac_address_4 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x03};
bcm_mac_t mac_address_5 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x04};
bcm_mac_t mac1 = {0x01, 0x00, 0x5E, 0x00, 0x01, 0x03};
bcm_mac_t mac2 = {0x01, 0x00, 0x5E, 0x00, 0x01, 0x02};

/*
 * 1: for IPMC packet with VRF !=0  (VPN) forwarding is according to VRF, G
 * 0: for IPMC packet forwarding is according to <RIF,G,SIP> regardless the VRF value
 */
int ipmc_vpn_lookup_enable = 1;


/*
 * 1: egress multicast
 * 0: ingress multicase
 */
int ipmc_egress_multicast_enable = 0;

/* create egress mc PMF action to match CUD */
int field_egress_mc_matching_cud(int unit, int cud) {
    int rv;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOutVPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);

    rv =  bcm_field_group_create(unit, qset, 3, &group);
    if (rv != BCM_E_NONE) {
        printf("field group create error: %s\n",bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("field group action set error: %s\n",bcm_errmsg(rv));
        return rv;
    }

    rv =  bcm_field_entry_create(unit, group, &entry);
    if (rv != BCM_E_NONE) {
        printf("field entry create error: %s\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_OutVPort32(unit, entry, (cud & 0xffffff), 0xffffff);
    if (rv != BCM_E_NONE) {
        printf("field qualify OutVPort error: %s\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("field action add error: %s\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("field group install error: %s\n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Init function. configures relevant data to be placed
 * in the above declared data structures
 */
void
ipmc_flows_init(int unit, int outP, int outP2, int outP3){


    int ipmc_index_offset = diag_test_mc(unit,"get_nof_egress_bitmap_groups");
    int ipmc_index_upper_bound = diag_test_mc(unit,"get_nof_ingress_mcids");
    int i;

    ipmc_flows_info1.forwarding_ports_1[0] = outP; /* Different combinations of ports representing multiple destinations for a certain mc group*/
    ipmc_flows_info1.forwarding_ports_1[1] = outP2;
    ipmc_flows_info1.forwarding_ports_1[2] = outP3;

    ipmc_flows_info1.forwarding_ports_2[0] = outP ;
    ipmc_flows_info1.forwarding_ports_2[1] = outP2;

    ipmc_flows_info1.forwarding_ports_3[0] = outP2 ;
    ipmc_flows_info1.forwarding_ports_3[1] = outP3;

    ipmc_flows_info1.forwarding_ports_4[0] = outP ;
    ipmc_flows_info1.forwarding_ports_4[1] = outP3;

    ipmc_flows_info1.mc_ip_arr[0] = 0xE0000102; /* 224.0.1.2 */
    ipmc_flows_info1.mc_ip_arr[1] = 0xE0000103; /* 224.0.1.3 */
    ipmc_flows_info1.mc_ip_arr[2] = 0xE0000104; /* 224.0.1.4 */
    ipmc_flows_info1.mc_ip_arr[3] = 0xE0000105; /* 224.0.1.5 */

    ipmc_flows_info1.src_ip_arr[0] = 0xC0A8000B; /* 192.168.0.11 */
    ipmc_flows_info1.src_ip_arr[1] = 0xC0A8000C; /* 192.168.0.12 */
    ipmc_flows_info1.src_ip_arr[2] = 0xC0A8000D; /* 192.168.0.13 */
    ipmc_flows_info1.src_ip_arr[3] = 0xC0A8000E; /* 192.168.0.14 */

    if (is_device_or_above(unit, QUMRAN_AX)){
        ipmc_index_offset = diag_test_mc(unit,"get_nof_egress_bitmaps");
    }
    sal_srand(sal_time());
    for (i=0; i <= 5; i++) {  /* picking random values for the mc groups */
        ipmc_flows_info1.ipmc_mc_groups[i] = (sal_rand() % ipmc_index_upper_bound);
        if(ipmc_flows_info1.ipmc_mc_groups[i] < ipmc_index_offset) {
            ipmc_flows_info1.ipmc_mc_groups[i] += ipmc_index_offset; /* ipmc index will be in range (4k,32k)*/
        }
        while (is_in_array(ipmc_flows_info1.ipmc_mc_groups, i-1, ipmc_flows_info1.ipmc_mc_groups[i])){ /* verifying that we don't get a previous value */
            ipmc_flows_info1.ipmc_mc_groups[i] = (sal_rand() % ipmc_index_upper_bound) + ipmc_index_offset;
        }

        ipmc_flows_info1.ipmc_indices[i] = ipmc_flows_info1.ipmc_mc_groups[i];
    }


}

/* utility function that checks whether a certain value already exists in a given array*/
int is_in_array(int *arr, int last_index, int val){

    int i;

    for (i=0; i <= last_index ; i++) {
        if (val == arr[i]) {
            return 1;
        }
    }

    return 0;
}


/* Destroys objects created in this feature*/
int
ipmc_flows_destroy(int unit, int nof_mc_indices){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i <= nof_mc_indices - 1; i++) {
        rv = bcm_multicast_destroy(unit, ipmc_flows_info1.ipmc_indices[i]);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
            printf("Error, in multicast_destroy\n");
            return rv;
        }
    }

    rv = bcm_l3_intf_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_intf_delete_all\n");
        return rv;
    }

    return rv;

}



int add_l3_interface_multicast_forwarding(int unit, int ipmc_index, int *ports , int nof_ports, int l3InterfaceId){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i < nof_ports; i++) {

        rv = bcm_multicast_ingress_add(unit,ipmc_index, ports[i]  ,l3InterfaceId);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d l3InterfaceId: %d \n", ports[i] ,l3InterfaceId);
            return rv;
        }

    }

    return rv;
}

/* Adding ports to MC group*/
int add_ingress_multicast_forwarding(int unit, int ipmc_index, int *ports , int nof_ports, int vlan){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i < nof_ports; i++) {
        rv = bcm_multicast_ingress_add(unit,ipmc_index, ports[i]  ,vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", ports[i] ,vlan);
            return rv;
        }

    }

    return rv;
}

/*
 * Adding ports to ingress MC group with encapsulation
 * AVT - advanced_vlan_translation_mode
 */
int add_ingress_multicast_forwarding_with_encap(int unit, int ipmc_index, int *ports , int nof_ports, int vlan,int AVT){

    int rv = BCM_E_NONE;
    int i;
    bcm_if_t encap_id;
    bcm_vlan_port_t vp;

    for (i=0; i < nof_ports; i++) {

        bcm_vlan_port_t_init(&vp);
        vp.port = ports[i];
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.egress_vlan = vlan;
        vp.vsi = vlan;
        vp.match_vlan = vp.vsi;

        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }


        if (AVT /*advanced_vlan_translation_mode*/) {
            rv = vlan_port_translation_set(unit, vlan, BCM_VLAN_NONE, vp.vlan_port_id, 2, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, vlan_port_translation_set\n");
                return rv;
            }
        }

        rv = bcm_multicast_vlan_encap_get(unit, ipmc_index, 0/*not in use in the function*/, vp.vlan_port_id, &encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_vlan_encap_get\n");
            return rv;
        }

        rv = bcm_multicast_ingress_add(unit,ipmc_index, ports[i]  ,encap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", ports[i] ,vlan);
            return rv;
        }

    }

    return rv;
}

/* Adding ports to egress MC group*/
int add_egress_multicast_forwarding(int unit, int ipmc_index, int *ports , int nof_ports, int vlan){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i < nof_ports; i++) {
        rv = bcm_multicast_egress_add(unit,ipmc_index, ports[i]  ,vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_egress_add: port %d vlan: %d \n", ports[i] ,vlan);
            return rv;
        }

    }

    return rv;
}

/* adding ports to VLAN flood MC group
 * In the ipmc flows hereby presented, flooding is determined by an MC group id which is in fact vsi
 */
int add_ingress_multicast_flooding(int unit, int vlan, int *ports , int nof_ports){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i < nof_ports; i++) {

        printf("port[i]: %d\n", ports[i]);
        rv = bcm_multicast_ingress_add(unit,vlan, ports[i]  ,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", ports[i]   ,vlan);
            return rv;
        }

    }

    return rv;
}


int create_rif_vlan(bcm_mac_t mac_address, int unit, int vlan, bcm_l3_intf_t* intf){

    int rv = BCM_E_NONE;

    bcm_l3_intf_t_init(intf);
    sal_memcpy(intf->l3a_mac_addr, mac_address, 6);
    intf->l3a_vid = intf->l3a_vrf = vlan;

    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
        return rv;
    }

    return rv;

}

/*  Create L3 Intf
 *  vrf <0 implies we create a rif with no vrf
*/
int create_rif(bcm_mac_t mac_address, int unit, int intf_id, bcm_l3_intf_t* intf, bcm_vrf_t vrf){

    int rv = BCM_E_NONE;
    bcm_l3_intf_t intf_ori;
  
    /* Create L3 Intf and disable its IPMC */

    bcm_l3_intf_t_init(intf);

    /* before creating L3 INTF, check whether L3 INTF already exists*/
    bcm_l3_intf_t_init(&intf_ori);
    intf_ori.l3a_intf_id = intf_id;
    rv = bcm_l3_intf_get(unit, &intf_ori);
    if (rv == BCM_E_NONE) {
        /* if L3 INTF already exists, replace it*/
        intf.l3a_flags = BCM_L3_REPLACE | BCM_L3_WITH_ID;
        intf.l3a_intf_id = intf_id;
    }
 
    sal_memcpy(intf->l3a_mac_addr, mac_address, 6);

    intf->l3a_vid = intf->l3a_intf_id = intf_id;

    if (vrf >= 0) {
        intf->l3a_vrf = vrf;
    }

    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
        return rv;
    }

    return rv;

}


/* Enable/Disable IPMC for RIF
 * For enabling, pass enable == 1
 * For disabling, pass enable == 0
*/
int enable_rif_ipmc(int unit, bcm_l3_intf_t* intf, int enable){

    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_intf_t aux_intf;

    aux_intf.l3a_vid = intf->l3a_vid;
    aux_intf.l3a_mac_addr = intf->l3a_mac_addr;

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;

    if (!enable) {
        ingress_intf.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    }

    rv = bcm_l3_intf_find(unit, &aux_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_find\n");
        return rv;
    }


    ingress_intf.vrf = aux_intf.l3a_vrf;


    rv = bcm_l3_ingress_create(unit, &ingress_intf, &(intf->l3a_intf_id));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}


/* In our context, this function configures one of the following two states:
 * 1. RIF.IPMC disabled: the function configures the lookup in LEM:
 *    In this case, we put bcmSwitchL3McastL2.
 *    if enabled == 0, we perform a lookup with <FID,DA>
 *    if enabled == 1, we perform a lookup with <FID,DIP>
 *
 * 2. RIF.IPMC enabled: the function configures the packet's forwarding following a miss in LEM and TCAM
 *    In this case, we put bcmSwitchUnknownIpmcAsMcast.
 *    if enabled == 0, the packet is L3 MC forwarded with default vrf.
 *    if enabled == 1, the packet is L2 flooded on the vsi.
 */
int switch_control_set(int unit, bcm_switch_control_t switch_control, int enabled){

    int rv;

    rv = bcm_switch_control_set(unit,switch_control,enabled); \
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    return rv;

}


/* Creates a L2 forwarding entry. The entry will be accessed by LEM<FID,DA> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a destination MAC address. The packet will exit with vid == vlan.
 */
int create_l2_forwarding_entry_da(int unit, bcm_mac_t mac, int l2mc_group,int vlan){

    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    /* Configurations relevant for LEM<FID,DA> lookup*/
    bcm_l2_addr_t_init(&l2_addr,mac,vlan); 

    l2_addr.flags = BCM_L2_MCAST;
    l2_addr.l2mc_group = l2mc_group;

    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }

    return rv;

}

/* Creates a forwarding entry. src_ip == 0x0 implies L2 forwarding. The entry will be accessed by LEM<FID,DIP> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a (MC) destination ip.
 * src_ip > 0x0 implies L3 forwarding. The entry will be accessed by TCAM <RIF, SIP, DIP> Lookup                                   .
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip.                                                                                                                                 .
 * src_ip == -1 implies creation of entry without sip.
 * src_ip == 0 implies vid =0
 * mc_bridge == 1 => we can accept sip = 0 and vid != 0 (valid value after dont care value)                                                                                                                                                                                                              .
 */
int create_forwarding_entry_dip_sip_inner(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan, int mc_bridge, uint32 flags){
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    /* Configurations relevant for LEM<FID,DIP> lookup*/
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    if ((src_ip != -1 && src_ip != 0) || mc_bridge) {
        data.s_ip_addr = src_ip;
        data.s_ip_mask = 0xffffffff;
        data.vid = vlan;
    }

    data.flags = flags;
    data.group = ipmc_index;
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}

/* calls inner function with flag mc_bridge = 0 which means ipmc is enabled
 * if ipmc is disabled then call create_forwarding_entry_dip_sip_inner with mc_bridge = 1 (additional parameter)                                                                                                                                                                                                          .
 */
int create_forwarding_entry_dip_sip(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan){
    return create_forwarding_entry_dip_sip_inner(unit,mc_ip,src_ip,ipmc_index,vlan,0,0);
}

/* Don't use create_forwarding_entry_dip_sip() because it isn't working well as description */
int create_forwarding_entry_dip_sip2(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan){

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    data.s_ip_addr = src_ip;
	if ((src_ip != 0)) {
		data.s_ip_mask = 0xffffffff;
		data.vid = vlan;
    }

    data.flags = 0;
    data.group = ipmc_index;

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip2 \n");
        return rv;
    }

    return rv;
}


/* Creates a L3 forwarding entry.
 * vrf != 0: The entry will be accessed by LEM<VRF,DIP> Lookup.
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip and vrf.                                                                                                                                  .
 * vrf == 0: The entry will be accessed following a miss in LEM<VRF,DIP> and TCAM<RIF,SIP,DIP> lookup and                                                                                                                                                                                                                                      .
 * bcmSwitchUnknownIpmcAsMcast == 0.                                                                                                                                                                                                                                                                                                                                             .
 */
int create_forwarding_entry_vrf(int unit, bcm_ip_t mc_ip, int ipmc_index, int vrf, bcm_mac_t mac,  int vlan, bcm_l3_intf_t* intf){

    int rv = BCM_E_NONE;
    bcm_l3_host_t data;
    bcm_ipmc_addr_t info;

    /* Create the entry */

    if (vrf != 0) {
        bcm_l3_host_t_init(&data);
        data.l3a_flags = 0x0;
        data.l3a_vrf = intf->l3a_vrf;
        data.l3a_intf = intf->l3a_intf_id;
        data.l3a_ip_addr = mc_ip;
        data.l3a_nexthop_mac = mac;
        BCM_GPORT_MCAST_SET(data.l3a_port_tgid, ipmc_index);

        rv = bcm_l3_host_add(unit, &data);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_host_add\n");
            return rv;
        }
    }
    else{ /* vrf==0, so we create a default vrf forwarding*/

        bcm_ipmc_addr_t_init(&info);
        if (is_device_or_above(unit,JERICHO)) {
            info.vid = 0;
        } else {
            info.vid = vlan;
        }
        /*Make sure that the MC address is valid, regardless of mask.*/
        info.mc_ip_addr = 0xE0000000;
        info.flags = 0x0;
        info.group = ipmc_index;
        info.vrf = vrf;
        /* Creates the entry */
        rv = bcm_ipmc_add(unit,&info);

    }


    return rv;

}


/*
*  This function exhibits the usage of an ipmc flow that is determined by the unsetting of the RIF IPMC. This flow includes four sub-flows,
*  which are employed by different sections in the test.
*
*  Section 1: Disables RIF IPMC to enable L2 MC forwarding.
*  Section 2: Creating a vsi to enable VLAN MC flooding.
*  Section 3: Creating MC groups: first two groups are the destination of L2 MC forwarding. The other two are the destination of VLAN MC flooding.
*  Section 4: Sets information for MC forwarding upon successful hit in LEM. The entries are built in Section 7.
*  Section 5: Sets information for VLAN MC flooding upon miss in LEM. In these cases, packets will be sent with a dip and/or dst_mac that won't be matched in LEM.
*  Section 6: Configures the lookup in LEM: bcmSwitchL3McastL2 == 1 leads to <FID,DIP> lookup, while bcmSwitchL3McastL2 == 0 leads to <FID,DA> lookup.
*  Section 7: Configuring the LEM entries that will be matched with (Multicast compatible) dip or dst_mac.
*
*  We present the following sub-flows:
*
*  1. bcmSwitchL3McastL2 == 1 && lookup in LEM<FID,DIP> is successful: packet is sent with dip that is matched to the entry. Upon this successful hit,
*  the packet will be forwarded according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3 with vid == vlan1.
*  Relevant sections: 1.1, 3.1, 4.1, 6, 7.1.
*
*  2. bcmSwitchL3McastL2 ==	0 && lookup in LEM<FID,DA> is successful: packet is sent with dst_mac that is matched to the entry. Upon this successful hit,
*  the packet will be forwarded according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3 with vid == vlan2.
*  Relevant sections: 1.2, 3.2, 4.2, 6, 7.2.
*
*  3. bcmSwitchL3McastL2 == 1 && lookup in LEM<FID,DIP> is unsuccessful: packet is sent with dip that is unmatched to the entry. To reproduce this
*  scenario, simply send a packet with a different mc_ip than the one  configured in the function. Upon this miss,the packet will be flooded according
*  to the VLAN flood MC group created in this function (vlan == vlan3). It will arrive at ports outP, outP2 with vid == vlan3.
*  Relevant sections: 2.1, 3.3, 5.1, 6.
*
*  4. bcmSwitchL3McastL2 == 0 && lookup in LEM<FID,DA> is unsuccessful: packet is sent with dst_mac that is unmatched to the entry. To reproduce this
*  scenario, simply send a packet with a different mac than the one configured in the function .Upon this miss,
*  the packet will be flooded according to the VLAN flood MC group created in this function (vlan == vlan4). It will arrive at ports outP, outP2 with vid == vlan4.
*  Relevant sections: 2.2, 3.4, 5.2, 6.
*/
int ipmc_flows_rif_ipmc_disabled(int unit, int inP, int outP, int outP2 ,int outP3 ,int vlan1, int vlan2, int vlan3, int vlan4){

    int rv = BCM_E_NONE;
    int SwitchL3McastL2_enabled = 1;
    int flags;

    bcm_l3_intf_t intf1, intf2;
    int advanced_vlan_translation_mode =  soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    int ipmc_ssm_enable = soc_property_get(unit, "ipmc_l2_ssm_mode", 0);
    /* In case the ipmc_set_entry_type_by_rif is set the IPMC entry L2/L3 will be determine be the RIF IPMC state,
     * otherwise it will be set by the BCM_IPMC_L2 flag */
    int L2_flag = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_set_entry_type_by_rif", 0) == 1) ? 0 : BCM_IPMC_L2;

    ipmc_flows_info1.ipmc_indices[6] = vlan3;
    ipmc_flows_info1.ipmc_indices[7] = vlan4;

    ipmc_flows_init(unit, outP, outP2, outP3);

    if (advanced_vlan_translation_mode) {
        rv = vlan_translate_action_set(unit, 7, 0, 0, bcmVlanActionReplace, bcmVlanActionNone);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_default_translate_action_set\n");
            return rv;
        }

        /* set action class */
        rv = vlan_default_translate_action_class_set(unit, 7);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translate_action_class_set\n");
            return rv;
        }
    }


    /* Section 1
     * create a rif, then disable ipmc
     */
    rv = create_rif_vlan(mac_address_1, unit , vlan1, &intf1);/* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_2, unit , vlan2, &intf2);/* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf1, 0); /* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf2, 0); /* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }


    /* Section 2:
     * creating a vsi; will be used as a parameter for L2 flooding
     */
    rv = bcm_vswitch_create_with_id(unit, vlan3); /* 2.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vlan4); /* 2.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }


    /* Section 3:
     * creating MC groups
     */

    /* Create the L2 MC Group with ipmc_index */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0])); /* 3.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    if (ipmc_ssm_enable) {
        /* Create the L2 MC Group with ipmc_index */
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[1])); /* 3.1.1 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[2])); /* 3.1.2 */
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
    }

    if (ipmc_egress_multicast_enable == 1) { 
        flags = BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;
    } else{ 
        flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;
    }
    /* Create the L2 MC Group with l2mc_group */
    rv = bcm_multicast_create(unit, flags, &(ipmc_flows_info1.ipmc_mc_groups[5])); /* 3.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan3 */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &vlan3); /* 3.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan4 */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &vlan4); /* 3.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }


    /* Section 4:
     * Setting inforamtion relevant to MC forwarding, upon successful hits in LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<FID,DIP> lookup */
    rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[0], ipmc_flows_info1.forwarding_ports_1, 3, vlan1,advanced_vlan_translation_mode); /* 4.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding_with_encap \n");
        return rv;
    }

    if (ipmc_ssm_enable) {
        /* set information relevant to MC forwarding: ipmc_index will connect the MC group to TCAM<FID,DIP,SIP> lookup */
        rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[1], ipmc_flows_info1.forwarding_ports_1, 3, vlan2,advanced_vlan_translation_mode); /* 4.1.1 */
        if (rv != BCM_E_NONE) {
            printf("Error, add_ingress_multicast_forwarding_with_encap \n");
            return rv;
        }
    }

    /* set information relevant to MC forwarding: l2mc_group will connect the MC group to LEM<FID,DA> lookup*/
    if (ipmc_egress_multicast_enable == 1) {
       rv = add_egress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[5], ipmc_flows_info1.forwarding_ports_1, 3, vlan2); /* 4.2 */
    } else{
       rv = add_ingress_multicast_forwarding_with_encap(unit, ipmc_flows_info1.ipmc_mc_groups[5], ipmc_flows_info1.forwarding_ports_1, 3, vlan2, advanced_vlan_translation_mode); /* 4.2 */
    }
    if (rv != BCM_E_NONE) {
        printf("Error, add_l2_ingress_multicast_forwarding \n");
        return rv;
    }


    /* Section 5:
     * Setting inforamtion relevant to VLAN MC flooding, upon misses in LEM
     */

    /* set information relevant to MC flooding with vid == vlan3*/
    rv = add_ingress_multicast_flooding(unit, vlan3, ipmc_flows_info1.forwarding_ports_2, 2); /* 5.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }

    /* set information relevant to MC flooding with vid == vlan4*/
    rv = add_ingress_multicast_flooding(unit, vlan4,  ipmc_flows_info1.forwarding_ports_2, 2); /* 5.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }


    /* Section 6:
     * Configuring the lookup in LEM:
     * if SwitchL3McastL2_enabled == 0, we perform a lookup with <FID,DA>
     * if SwitchL3McastL2_enabled == 1, we perform a lookup with <FID,DIP>
    */
    rv = bcm_switch_control_set(unit,bcmSwitchL3McastL2,SwitchL3McastL2_enabled);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }


    /* Section 7:
     * defining entries for LEM which will forward the packet to the relevant mc group
     */
    rv = create_forwarding_entry_dip_sip_inner(unit, ipmc_flows_info1.mc_ip_arr[0], 0x0, ipmc_flows_info1.ipmc_mc_groups[0], vlan1,1,L2_flag); /* 7.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip\n");
        return rv;
    }

    if (ipmc_ssm_enable) {
        /* configure entry to TCAM <FID,DIP,SIP>*/
        rv = create_forwarding_entry_dip_sip_inner(unit, ipmc_flows_info1.mc_ip_arr[0], 0xc0a8000f/*sip*/ ,ipmc_flows_info1.ipmc_mc_groups[1], vlan1,1,L2_flag); /* 7.1.1 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip\n");
            return rv;
        }
        rv = create_forwarding_entry_dip_sip_inner(unit, ipmc_flows_info1.mc_ip_arr[0], 0xc0a8000e/*sip*/ ,ipmc_flows_info1.ipmc_mc_groups[2], vlan1,1,L2_flag); /* 7.1.2 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip\n");
            return rv;
        }
    }

    rv = create_l2_forwarding_entry_da(unit, mac1, ipmc_flows_info1.ipmc_mc_groups[5], vlan2); /* 7.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_l2_forwarding_entry_da\n");
        return rv;
    }

    return rv;
}



/*
*  This function exhibits the usage of an ipmc flow that is determined by the setting of the RIF IPMC. This flow includes six sub-flows,
*  which are employed by different sections in the test.
*
*  Section 1: Enables RIF IPMC to enable MC forwarding.
*  Section 2: Creating a vsi to enable VLAN MC flooding.
*  Section 3: Creating MC groups: first four groups are the destination of L3 MC forwarding.
*             The other two are the destination of VLAN MC flooding and L3 forwarding with default VRF.
*  Section 4: Sets information for MC forwarding upon successful hit in LEM and/or TCAM. The entries are built in Section 7.
*  Section 5: Sets information for VLAN MC flooding upon miss in LEM and TCAM or L3 forwarding with default vrf.
*             In these cases, packets will be sent with a dip that won't be matched in LEM and TCAM.
*  Section 6: Configures the sub flow after a miss in LEM and/or TCAM:
*             bcmSwitchUnknownIpmcAsMcast == 1 leads to flooding on vsi, while bcmSwitchUnknownIpmcAsMcast == 0
*             leads to L3 MC forwarding with default vrf.
*  Section 7: Configuring the entries that will be matched with (Multicast compatible) dip (and/or sip in TCAM).
*             This also includes forwarding with default vrf.
*
*  We present the following sub-flows:
*
*  1. If soc property ipmc_vpn_lookup_enable == 1,
*      Packet arrives with a dip that matches a LEM entry. The packet will be forwarded, upon the successful hit,
*      according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3.
*      Otherwise(ipmc_vpn_lookup_enable == 0), Packet arrives with sip + dip that matches a LEM entry.
*      The packet will be forwarded, upon the successful hit, according to the multicast group created in this function.
*      It will arrive at ports outP, outP2, outP3.
*    Relevant sections: 1.1, 3.1, 4.1, 7.4.
*
*  2. Packet arrives with a dip and sip that matches a TCAM entry, configured with DIP + SIP. The packet will be forwarded, upon the successful hit,
*   according to the multicast group created in this function. It will arrive at ports outP2, outP3.
*   Relevant sections: 1.2, 3.2, 4.2, 7.1.
*
*  3. Packet arrives with only a dip that matches a TCAM entry . The packet will be forwarded, upon the successful hit,
*   according to the multicast group created in this function. It will arrive at ports outP, outP2.
*   Relevant sections: 1.3, 3.3, 4.3, 7.2.
*
*  4. If soc property ipmc_vpn_lookup_enable == 1,
*      Packet arrives with a dip that matches both LEM and TCAM entries . The packet will be forwarded, upon the successful hit,
*      according to the multicast group that is defined in the TCAM entry. It will arrive at ports outP, outP3.
*      Otherwise(ipmc_vpn_lookup_enable == 0), Packet arrives with a dip that matches TCAM entries, and sip+dip that matches LEM entries.
*      Because TCAM > LEM in priority in case both match, the packet will be forwarded, upon the successful hit
*      according to the multicast group that is defined in the TCAM entry.It will arrive at ports outP, outP3.
*    Relevant sections: 1.4, 3.4, 4.4, 7.3.
*
*  5. Packet arrives with a dip that is unmatched in the LEM and TCAM . Upon setting bcmSwitchUnknownIpmcAsMcast == 1,
*  The packet will be L2 flooded on the given vsi (vlan). It will arrive at ports outP, outP2.
*  sections: 2.1, 3.5, 5.1, 6.1.
*
*  6. Packet arrives with a dip that is unmatched in the LEM and TCAM . Upon setting bcmSwitchUnknownIpmcAsMcast == 0,
*  The packet will be L3 forwarded on the given vrf. It will arrive at ports outP, outP2.
*  sections: 1.5, 3.6, 6.1 (unset bcmSwitchUnknownIpmcAsMcast), 7.5.
*/
int ipmc_flows_rif_ipmc_enabled(int unit, int inP, int outP, int outP2 ,int outP3 ,int rif1, int rif2, int rif3, int rif4, int rif5, int vlan){

    int rv = BCM_E_NONE;
    int SwitchUnknownIpmcAsMcast_enabled = 1;

    bcm_l3_intf_t intf1, intf2, intf3, intf4, intf5;
    bcm_vrf_t vrf = 2;

    ipmc_flows_info1.ipmc_indices[6] = vlan;
    ipmc_vpn_lookup_enable = soc_property_get(unit , "ipmc_vpn_lookup_enable", 1);

    ipmc_flows_init(unit, outP, outP2, outP3);

    /* Section 1
     * create a rif, then enable ipmc
     */
    rv = create_rif_vlan(mac_address_1, unit , rif1, &intf1); /* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_2, unit , rif2, &intf2); /* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_3, unit , rif3, &intf3); /* 1.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_4, unit , rif4, &intf4); /* 1.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }

    rv = create_rif_vlan(mac_address_5, unit , rif5, &intf5); /* 1.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }


    /* Section 2:
     * creating a vsi; will be used as a parameter for L2 flooding
     */
    rv = bcm_vswitch_create_with_id(unit, vlan); /* 2.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }

    /* Section 3:
     * creating MC groups
     */

    /* In the following 4 calls we create the L3 MC Group with an index taken from the main struct's data structure */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0])); /* 3.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[1])); /* 3.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[2])); /* 3.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[3])); /* 3.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &vlan); /* 3.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L3 MC Group with default vrf */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[4])); /* 3.6 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Section 4:
     * Setting inforamtion relevant to L3 MC forwarding, upon successful hits in TCAM and/or LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<vrf,dip> lookup */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[0],ipmc_flows_info1.forwarding_ports_1, 3 ,intf1.l3a_intf_id); /* 4.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }
    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to TCAM<rif,sip,dip> lookup with sip + dip given */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[1],ipmc_flows_info1.forwarding_ports_2, 2 ,intf2.l3a_intf_id); /* 4.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to TCAM<rif,sip,dip> lookup with only dip given */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[2],ipmc_flows_info1.forwarding_ports_3, 2 ,intf3.l3a_intf_id); /* 4.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to TCAM<rif,sip,dip>, LEM<vrf,dip> lookup. TCAM is chosen. */
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[3],ipmc_flows_info1.forwarding_ports_4, 2 ,intf4.l3a_intf_id); /* 4.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l3_interface_multicast_forwarding\n");
        return rv;
    }

    /* Section 5:
     * Setting inforamtion relevant to VLAN MC flooding, upon misses in LEM and TCAM
     */

    /* set information relevant to MC flooding with vid == vlan*/
    rv = add_ingress_multicast_flooding(unit, vlan, ipmc_flows_info1.forwarding_ports_2, 2); /* 5.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }

    /* set information relevant to L3 MC forwarding with default vrf,  vid == rif5*/
    rv = add_l3_interface_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[4],ipmc_flows_info1.forwarding_ports_2, 2 ,intf5.l3a_intf_id); /* 5.2 */

    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding_with_encap\n");
        return rv;
    }

    /* Section 6:
     * Configuring the sub flow taken after a miss in LEM and TCAM:
     * if bcmSwitchUnknownIpmcAsMcast == 1, the packet will be flooded on vsi.
     * if bcmSwitchUnknownIpmcAsMcast == 0, the packet will be L3 MC forwarded with default vrf.
    */
   rv = bcm_switch_control_set(unit, bcmSwitchUnknownIpmcAsMcast, SwitchUnknownIpmcAsMcast_enabled); /* 6.1 */
   if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
   }

    /* Section 7:
     * defining entries for LEM and TCAM which will forward the packet to the relevant mc group
     */
    rv = create_forwarding_entry_dip_sip(unit,ipmc_flows_info1.mc_ip_arr[1],ipmc_flows_info1.src_ip_arr[1],ipmc_flows_info1.ipmc_mc_groups[1],rif2); /* 7.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip, step - 7.1\n");
        return rv;
    }

    rv = create_forwarding_entry_dip_sip(unit,ipmc_flows_info1.mc_ip_arr[1],-1,ipmc_flows_info1.ipmc_mc_groups[2],rif3); /* 7.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip, step - 7.2\n");
        return rv;
    }

    if (ipmc_vpn_lookup_enable) {
        rv = create_forwarding_entry_dip_sip(unit,ipmc_flows_info1.mc_ip_arr[0],ipmc_flows_info1.src_ip_arr[0],ipmc_flows_info1.ipmc_mc_groups[3],rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3\n");
            return rv;
        }

        rv = create_forwarding_entry_vrf(unit, ipmc_flows_info1.mc_ip_arr[0],ipmc_flows_info1.ipmc_mc_groups[0], vrf, mac2,rif1, &intf1); /* 7.4 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_vrf, step - 7.4\n");
            return rv;
        }
    } else {
        /* Add a none-exact match IPMC entry in TCAM */
        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[3],0,ipmc_flows_info1.ipmc_mc_groups[3],rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3.1\n");
            return rv;
        }
        /* Add a exact match IPMC entry in LEM */
        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[3],ipmc_flows_info1.src_ip_arr[3],ipmc_flows_info1.ipmc_mc_groups[0],rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3.2\n");
            return rv;
        }

        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[2],ipmc_flows_info1.src_ip_arr[2],ipmc_flows_info1.ipmc_mc_groups[0],rif1); /* 7.4 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.4\n");
            return rv;
        }
    }

    rv = create_forwarding_entry_vrf(unit, 0 ,ipmc_flows_info1.ipmc_mc_groups[4],0, mac2,rif5, &intf5);  /* 7.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_vrf\n");
        return rv;
    }

    return rv;
}
/*
* This cint creates two IPMC groups that are almost identical besides the masks of the MC IP.
* This cint is part of a test that verifies that in case two entries of IPV4 MC are matching
* two entries , the one with the bigger mask will be selected (LPM).
*/
int ipmc_ipv4mc_priority_test(int unit,int outP1, int outP2 ,int outP3 ,int ipmc_group,int vlan){

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    bcm_l3_intf_t l3Intf;
    int i,group;
    int nofIpmcGroups = 2;
    int ipmcGroups[2];
    int mcMasks[2];
    int numOfPortsForGroup[2];
    int forwarding_ports[3];

    ipmcGroups[0] = ipmc_group;
    ipmcGroups[1] = ipmc_group+1;
    mcMasks[0] = 0xffffffff;
    mcMasks[1] = 0xfffffff0;
    numOfPortsForGroup[0] = 3;
    numOfPortsForGroup[1] = 2;
    forwarding_ports[0] = outP1;
    forwarding_ports[1] = outP2;
    forwarding_ports[2] = outP3;


     bcm_l3_intf_t_init(&l3Intf);
     sal_memcpy(l3Intf.l3a_mac_addr, mac_address_1, 6);
     l3Intf.l3a_vid = vlan;
     rv = bcm_l3_intf_create(unit, &l3Intf);
     if (rv != BCM_E_NONE) {
         printf("Error, bcm_l3_intf_create \n");
         return rv;
     }


     for(group = 0; group < nofIpmcGroups; group++) {

         rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &ipmcGroups[group]);
         if (rv != BCM_E_NONE) {
             printf("Error, bcm_multicast_create \n");
             return rv;
         }


         for (i=0; i < numOfPortsForGroup[group]; i++) {
             rv = bcm_multicast_ingress_add(unit, ipmcGroups[group], forwarding_ports[i], vlan);
             if (rv != BCM_E_NONE) {
                 printf("Error, bcm_multicast_ingress_add");
                 return rv;
             }
         }

        bcm_ipmc_addr_t_init(&data);
        data.mc_ip_addr = 0xE0000102;
        data.mc_ip_mask = mcMasks[group];
        if(data.mc_ip_mask == 0xffffffff){
            data.s_ip_addr = 0xC0A8000B;
            data.s_ip_mask = 0xffffffff;
            data.vid = vlan;
        }
        data.flags = 0;
        data.group = ipmcGroups[group];

        /* Creates the entry */
        rv = bcm_ipmc_add(unit,&data);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_ipmc_add \n");
            return rv;
        }

     }
     return rv;
}
/*
 * This cint is sued to test the TCAM use for IPV4 MC without the VRF qualifier.
 * It creates 2 interfaces with different VRFs (to verify that the VRF doesn't affect anything ) and 3 IPMC entries.
 * First entry has only DIP un-masked to verify that both in-rif can hit this entry regardless of the VRF.
 * Second and third entries are the same except the IN-RIF values to verify that the IN-RIF affect the TCAM entry selection.
 */
int ipmc_ipv4mc_tcam_without_vrf(int unit,int inport, int outP1, int outP2 ,int outP3 ,int ipmc_group){
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    bcm_l3_intf_t l3Intf;
    bcm_l3_egress_t l3eg;
    bcm_mac_t MyMac  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
    bcm_mac_t outMac = {0x00, 0x0d, 0x00, 0x02, 0x00, 0x22};
    int i,group, l3egid;
    int nofIpmcGroups = 3;
    int ipmcGroups[3];
    int numOfPortsForGroup[3] = {2, 2, 3};
    int forwarding_ports[3];
    int vlan = 400;
    uint32 MC_IP[3] = {0xE0000102, 0xE0000103, 0xE0000103};
    uint32 sipMask[3] = {0x0, 0xffffffff, 0xffffffff};
    uint32 sip = 0xC0A8000B; /* 192.168.0.11 */
    ipmcGroups[0] = ipmc_group++;
    ipmcGroups[1] = ipmc_group++;
    ipmcGroups[2] = ipmc_group++;


    forwarding_ports[0] = outP1;
    forwarding_ports[1] = outP2;
    forwarding_ports[2] = outP3;


    for (i = 0; i < 2; i++) {
        bcm_l3_intf_t_init(&l3Intf);
        MyMac[5] = i;
        sal_memcpy(l3Intf.l3a_mac_addr, MyMac, 6);
        l3Intf.l3a_vid = vlan+i;
        l3Intf.l3a_vrf = vlan+i;
        rv = bcm_l3_intf_create(unit, &l3Intf);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_intf_create \n");
            return rv;
        }
    }

    for(group = 0; group < nofIpmcGroups; group++) {

        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &ipmcGroups[group]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }

        for (i=0; i < numOfPortsForGroup[group]; i++) {

            int encap_id;
            bcm_l3_egress_t_init(&l3eg);
            sal_memcpy(l3eg.mac_addr, outMac, 6);
            l3eg.vlan   = 300+i+(group*30);
            l3eg.port   = forwarding_ports[i];
            l3eg.flags  = BCM_L3_EGRESS_ONLY;

            rv = bcm_l3_egress_create(unit, 0, &l3eg, &l3egid);
            if (rv != BCM_E_NONE) {
                printf ("bcm_l3_egress_create failed: %d \n", rv);
            }

            bcm_multicast_egress_object_encap_get(unit, ipmcGroups[group], l3egid, &encap_id);

            rv = bcm_multicast_ingress_add(unit, ipmcGroups[group], forwarding_ports[i], encap_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_ingress_add");
                return rv;
            }
       }

       bcm_ipmc_addr_t_init(&data);
       data.mc_ip_addr = MC_IP[group];
       data.mc_ip_mask = 0xffffffff;
       data.s_ip_mask  = sipMask[group];

       if(data.s_ip_mask != 0x0){
           data.s_ip_addr = 0xC0A8000B;
           data.vid = vlan + group - 1;
       }
       data.flags = 0;
       data.group = ipmcGroups[group];

       /* Creates the entry */
       rv = bcm_ipmc_add(unit,&data);
       if (rv != BCM_E_NONE) {
           printf("Error, bcm_ipmc_add \n");
           return rv;
       }

    }
    return rv;
}

/* set_up_ipmc_disable sets a configuration for the case of soc property ipmc_l3mcastl2_mode = 0.
 * Two MC groups are created - L2 and L3.
 * Entries in the LEM are added for <FID, DIP> and <FID, DA> lookups.
 * When switch bcmSwitchL3McastL2 = 0 - the forwarding is carried out based on the L2 MC group replication list.
 * When bcmSwitchL3McastL2 = 1 the forwarding is carried out based on the L3 MC group replication list.
 * On successful lookup for the bcmSwitchL3McastL2 = 1 case, packets will be forwarded to ports oport0, oport1 and oport2.
 * On successful lookup for the bcmSwitchL3McastL2 = 0 case, packets will be forwarded to ports oport0 and oport2.
 * On unsuccessful lookup, packets will not be replicated. The device will send zero packets.
 * Flag BCM_L3_INGRESS_L3_MCAST_L2 is forbidden when soc property ipmc_l3mcastl2_mode = 0. BCM_E_PARAM is returned.
 */
int set_up_ipmc_disable(int unit, int iport, int oport0, int oport1, int oport2, bcm_ip_t mc_dip, int vlan, int vlan2) {

    int rv = BCM_E_NONE;
    int SwitchL3McastL2_enabled = 1;
    bcm_mac_t da_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02};
    bcm_mac_t da_match = {0x01, 0x00, 0x5E, 0x00, 0x03, 0x02};
    int L2_flag = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_set_entry_type_by_rif", 0) == 1) ? 0 : BCM_IPMC_L2;
    int ipmc_l3mcastl2 = soc_property_get(unit, "ipmc_l3mcastl2_mode", 0);
    
    ipmc_flows_init(unit, oport0, oport1, oport2);
    int flags [2] = {BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 
                     BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP};
    int nof_groups = 2;
    int level_group[2] = {2, 3};
    int group;
    /*Create MC groups for packet replication - L2 and L3 groups*/
    for (group = 0; group < nof_groups; group++ ){
        rv = bcm_multicast_create(unit, flags[group], &(ipmc_flows_info1.ipmc_mc_groups[group])); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        printf ("Created an L%d MC group with id %d. \n", level_group[group], ipmc_flows_info1.ipmc_mc_groups[group]);
        /*Add ports for replication to the Mc groups.*/
        if (level_group[group] == 2) {
            rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[group], ipmc_flows_info1.forwarding_ports_4, 2, vlan);
        }
        else {
            rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[group], ipmc_flows_info1.forwarding_ports_1, 3, vlan);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, add_ingress_multicast_forwarding \n");
            return rv;
        }
    }
    
    /*3. Create L3 interfaces using bcm_l3_intf_create and disable its IPMC routing*/
    bcm_l3_intf_t intf, intf2;
    rv = create_rif_vlan(da_mac, unit, vlan, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }
    /*If ipmc_l3mcastl2 mode is 0 then flag BCM_L3_INGRESS_L3_MCAST_L2 cannot be used. BCM_E_PARAM is returned.*/
    /*Else, BCM_E_NONE is returned.*/
    if (SOC_IS_JERICHO(unit)) {
        rv = disable_rif_ipmc_flags(unit, &intf);
        if (rv != BCM_E_PARAM && ipmc_l3mcastl2 == 0 ) {
            printf ("Error, enable_rif_ipmc_flags - %d\n", rv);
            return rv;
        }
        else if (rv != BCM_E_NONE && ipmc_l3mcastl2 == 1) {
            printf ("Error, enable_rif_ipmc_flags");
            return rv;
        }
    }
    else if (ipmc_l3mcastl2 == 1) {
        /*If the device is Arad and the soc propr is set to 1, then we will define the RIF with the L3_INGRESS_L3_MCAST_L2 flag*/
        rv = disable_rif_ipmc_flags(unit, &intf);
        if (rv != BCM_E_NONE) {
            printf ("Error, enable_rif_ipmc_flags - %d\n", rv);
            return rv;
        }
    }
    
    /*if soc prop is 0, then disable_rif_ipmc_flags would have returned BCM_E_PARAM.*/
    /*Next function should successfully disable the IPMC routing for the RIF.*/
    if (ipmc_l3mcastl2 == 0) {
        rv = enable_rif_ipmc(unit, &intf, 0);
        if (rv != BCM_E_NONE) {
            printf ("Error, enable_rif_ipmc");
            return rv;
        }
    }
    
    /* defining entries for LEM which will forward the packet to the relevant mc group - Create an entry <VRF, DIP, SIP, RIF> */
    rv = create_forwarding_entry_dip_sip_inner(unit, mc_dip, 0x0, ipmc_flows_info1.ipmc_mc_groups[1], vlan, 1, L2_flag);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip\n");
        return rv;
    }
    /* Create a <FID, DA> entry.*/
    rv = create_l2_forwarding_entry_da(unit, da_match, ipmc_flows_info1.ipmc_mc_groups[0], vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, create_l2_forwarding_entry_da\n");
        return rv;
    }
    /*If a second vlan is provided, another RIF will be created.*/
    if (vlan2 != 0) {
        rv = create_rif_vlan(da_mac, unit, vlan2, &intf2);
        if (rv != BCM_E_NONE) {
            printf("Error, create_rif_vlan\n");
            return rv;
        }
        rv = enable_rif_ipmc(unit, &intf2, 0);
        if (rv != BCM_E_NONE) {
            printf ("Error, enable_rif_ipmc");
            return rv;
        }
        
        /* defining entries for LEM which will forward the packet to the relevant mc group */
        rv = create_forwarding_entry_dip_sip_inner(unit, mc_dip, 0x0, ipmc_flows_info1.ipmc_mc_groups[1], vlan2, 1, L2_flag);
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip\n");
            return rv;
        }
        rv = create_l2_forwarding_entry_da(unit, da_match, ipmc_flows_info1.ipmc_mc_groups[0], vlan2);
        if (rv != BCM_E_NONE) {
            printf("Error, create_l2_forwarding_entry_da\n");
            return rv;
        }
    }
    /*Set bcmSwitchL3McastL2 to enabled.*/
    rv = bcm_switch_control_set(unit,bcmSwitchL3McastL2,SwitchL3McastL2_enabled);
    /*If soc prop ipmc_l3mcastl2_mode is set to 0, then this switch control can be used. 
    If it is 1 then the switch control is not supported and BCM_E_CONFIG will be returned.
    */
    if (rv != BCM_E_NONE && ipmc_l3mcastl2 == 0) {
        printf ("Error, bcm_switch_control_set");
        return rv;
    }
    else if (rv != BCM_E_CONFIG && ipmc_l3mcastl2 == 1) {
        printf ("Error, bcm_switch_control_set");
        return rv;
    }
    else if (rv == BCM_E_CONFIG && ipmc_l3mcastl2 == 1) {
        /*Make sure that if the soc prop is 1 and the API returned BCM_E_CONFIG as expected, the return value will be set to BCM_E_NONE.*/
        rv = BCM_E_NONE;
    }
    
    return rv;
}

/* Disables IPMC-routing per RIF.
 * When flag BCM_L3_INGRESS_L3_MCAST_L2 is active, lookups are made for LEM <FID, DIP>
 */
int disable_rif_ipmc_flags(int unit, bcm_l3_intf_t* intf) {

    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_intf_t aux_intf;

    aux_intf.l3a_vid = intf->l3a_vid;
    aux_intf.l3a_mac_addr = intf->l3a_mac_addr;

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_L3_MCAST_L2;
    rv = bcm_l3_intf_find(unit, &aux_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_find\n");
        return rv;
    }

    ingress_intf.vrf = aux_intf.l3a_vrf;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &(intf->l3a_intf_id));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create - %d\n", rv);
        return rv;
    }

    return rv;
}

/*
 * This function sets the general configuration for the IPv6 MC flows.
 * It creates two IPMC entries and two L2 entries that lead to four different MC groups (two L2 and two L3).
 * The keys for the entries are as follows <VRF, DIP, RIF1>, <VRF, DIP, RIF2>, <FID1, DA>, <FID2, DA>.
 * One of the RIFs is set to be ipmc-disabled so no IPMC packets are allowed to be forwarded as such.
 */
int ipv6_ipmc_flows (int unit, int vlan1, int vlan2, int inP, int outP, int outP1, int outP2) {

    int rv = BCM_E_NONE;
    bcm_mac_t da_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02};
    bcm_mac_t da_mac_mc = {0x33, 0x33, 0x00, 0x00, 0x12, 0x34};
    bcm_ip6_t dip  = {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x34};
    
    /*Load initial settings*/
    ipmc_flows_init(unit, outP, outP1, outP2);
    
    /*1. Create a MC group and add forwarding port information to it*/
    int nof_groups = 4;
    int nof_ports = 2;
    int group;
    int ipmc_vlan[4] = {vlan1, vlan2, vlan1, vlan2};
    /*Two forwarding ports to each MC group.*/
    int frw_ports[4][2]={{outP,  outP1},
                         {outP1, outP2},
                         {inP,   outP1},
                         {inP,   outP2}};
    /*Both groups are L3 MC ingress groups.*/
    int mc_flags[4] = { BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 
                        BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 
                        BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 
                        BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP};
    for (group = 0; group < nof_groups; group++)
    {
        rv = bcm_multicast_create(unit, mc_flags[group], &(ipmc_flows_info1.ipmc_mc_groups[group])); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        printf ("Created an L3/L2 MC group with id %d. \n", ipmc_flows_info1.ipmc_mc_groups[group]);
        rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[group], frw_ports[group], nof_ports, ipmc_vlan[group]);
        if (rv != BCM_E_NONE) {
            printf("Error, add_ingress_multicast_forwarding \n");
            return rv;
        }
        printf("Added ports to the MC group - frw_ports[%d]\n", group);
    }
    /*2. Create two RIFs and disable IPMC forwarding for one of them.*/
    bcm_l3_intf_t intf;
    rv = create_rif_vlan(da_mac, unit, vlan1, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }
    rv = enable_rif_ipmc(unit, &intf, 0);
    if (rv != BCM_E_NONE) {
        printf ("Error, enable_rif_ipmc");
        return rv;
    }
    bcm_l3_intf_t intf2;
    rv = create_rif_vlan(da_mac, unit, vlan2, &intf2);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }
    
    /*3. Create four ipmc entries - two with RIF1, two with RIF2*/
    int nof_entries = 2;
    int ipmc_rifs[2] = {vlan1, vlan2};
    int ipmc_indx[2] = {ipmc_flows_info1.ipmc_mc_groups[2], ipmc_flows_info1.ipmc_mc_groups[1]};
    int entry;
    for (entry = 0; entry < nof_entries; entry++)
    {
        /*Create a forwarding entry for ipmc-enabled case.*/
        rv = create_forwarding_entry_dip_sip_ipv6(unit, dip, ipmc_indx[entry], ipmc_rifs[entry]);
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip\n");
            return rv;
        }
    }
    ipmc_indx[0] = ipmc_flows_info1.ipmc_mc_groups[0];
    ipmc_indx[1] = ipmc_flows_info1.ipmc_mc_groups[3];
    for (entry = 0; entry < nof_entries; entry++)
    {
        /*Create forwarding entry for ipmc-disabled case.*/
        rv = create_l2_forwarding_entry_da(unit, da_mac_mc, ipmc_indx[entry], ipmc_rifs[entry]);
        if (rv != BCM_E_NONE) {
            printf("Error, create_l2_forwarding_entry_da\n");
            return rv;
        }
    }
    
    return rv;
}

/* This function creates a public IPMC entry with flag BCM_IPMC_IP6.
 * It uses the provided DIP, full masking it and the In-RIF to form the key for the entry.
 * The payload is the ipmc index signifying the MC group.
 */
int create_forwarding_entry_dip_sip_ipv6 (int unit, bcm_ip6_t dip, int ipmc_index, int vlan) {
    int rv = BCM_E_NONE;
    bcm_ip6_t full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    
    bcm_ipmc_addr_t data;
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip6_addr = dip;
    data.mc_ip6_mask = full_mask;
    data.flags = BCM_IPMC_IP6;
    data.group = ipmc_index;
    data.vid = vlan;
    data.vrf = vlan;
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip_ipv6 \n");
        return rv;
    }
    return rv;
}




/* This function sets the configuration for testing the bcm_ipmc_enable API with IPv4/IPv6 entries.
 * It creates two MC groups - one is the destination of the IPMC entry (the L3 MC group) and the other - the L2 entry.
 * The L3 MC group leads to ports outP0 and outP2, the L2 MC group leads to ports outP1 and outP2.
 * The RIF of vlan2 is made to be IPMC disabled.
 * Create one IPMC entry if ipmc is enabled by API. If it is disabled (bcm_ipmc_enable(unit,0) has been called)
 * then the API will return an error - BCM_E_INIT.
 * Create two L2 entries <FID, DA>.
 * The flag ipv4_flag signifies (if it has the value of 1) that IPv4 IPMC entry will be created. 
 * If its value is 0, then an IPv6 IPMC entry will be created. A different DA MAC is used for the two cases.
 */
int ipmc_enable_by_api 
             (int unit, int inP, int outP0, int outP1, int outP2, bcm_ip_t dip_mc_ip, bcm_ip_t sip_uc_ip, 
             int vlan1, int vlan2, int ipmc_enabled, int ipv4_flag)
{
    int rv = BCM_E_NONE;
    bcm_mac_t da_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02};
    bcm_mac_t da_match = {0x01, 0x00, 0x5E, 0x00, 0x03, 0x02};
    bcm_mac_t da_match_6 = {0x33, 0x33, 0x00, 0x00, 0x12, 0x12};
    bcm_ip6_t dip_mc_ip6  = {0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x12};
    int disabled_vlan = 0;
    ipmc_flows_init(unit, outP0, outP1, outP2);
    
    rv = bcm_ipmc_enable(unit, ipmc_enabled);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_enable\n");
        return rv;
    }
    
    int group, nof_groups = 2;
    int ipmc_index[2] = {ipmc_flows_info1.ipmc_mc_groups[0], ipmc_flows_info1.ipmc_mc_groups[1]};
    int mc_group_flags[2] = {BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP};
    int vlan[2] = {vlan1, vlan2};
    int nof_ports = 2;
    int fwd_ports[2][2] = {{outP0, outP2},{outP1, outP2}};
    /*Create two MC groups - L2 and L3 and add replication ports.*/
    for (group = 0; group < nof_groups; group++) {
        rv = bcm_multicast_create(unit, mc_group_flags[group], &(ipmc_index[group])); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        printf ("Created an L3 MC group with id %d. \n", ipmc_index[group]);
        rv = add_ingress_multicast_forwarding(unit, ipmc_index[group], fwd_ports[group], nof_ports, vlan[group]);
        if (rv != BCM_E_NONE) {
            printf("Error, add_ingress_multicast_forwarding \n");
            return rv;
        }
    }
    /*Create two interfaces. Make one to be IPMC-disabled with flag.*/
    bcm_l3_intf_t intf, intf2;
    rv = create_rif_vlan(da_mac, unit, vlan1, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }
    rv = create_rif_vlan(da_mac, unit, vlan2, &intf2);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_vlan\n");
        return rv;
    }
    rv = enable_rif_ipmc(unit, &intf2, disabled_vlan);
    if (rv != BCM_E_NONE) {
        printf ("Error, enable_rif_ipmc");
        return rv;
    }
    if (ipv4_flag == 1) {
        /*Create an IPMC ipv4 entry.*/
        rv = create_forwarding_entry_dip_sip3(unit, dip_mc_ip, sip_uc_ip, ipmc_index[0], vlan1, 0, vlan1);
        if ((rv != BCM_E_NONE && ipmc_enabled == 1) || (rv != BCM_E_INIT && ipmc_enabled == 0)) {
            printf("Error, create_forwarding_entry_dip\n");
            return rv;
        }
        /*Create L2 entry - FID, DA*/
        rv = create_l2_forwarding_entry_da(unit, da_match, ipmc_index[1], vlan[1]);
        if (rv != BCM_E_NONE) {
            printf("Error, create_l2_forwarding_entry_da\n");
            return rv;
        }
    }
    else {
        rv = create_forwarding_entry_dip_sip_ipv6(unit, dip_mc_ip6, ipmc_index[0], vlan1);
        if ((rv != BCM_E_NONE && ipmc_enabled == 1) || (rv != BCM_E_INIT && ipmc_enabled == 0)) {
            printf("Error, create_forwarding_entry_dip_sip_ipv6\n");
            return rv;
        }
        /*Create L2 entry - FID, DA*/
        rv = create_l2_forwarding_entry_da(unit, da_match_6, ipmc_index[1], vlan[1]);
        if (rv != BCM_E_NONE) {
            printf("Error, create_l2_forwarding_entry_da\n");
            return rv;
        }
    }

    return rv;
}





/*
 * This function creates the configuration used for IPv4 IPMC-disabled flow.
 * It takes into account the ipmc_l2_ssm_mode and ipmc_l3mcastl2_mode soc properties.
 * It creates an L3 Mc group and adds replication ports to it -  outP0 and outP1.
 * It creates an interface and then disables IPMC traffic.
 * Based on the value of the soc property ipmc_l3mcastl2_mode it directs traffic to the Ipv4 bridge.
 * This is done either using the switch bcmSwitchL3McastL2 or using flag BCM_L3_INGRESS_L3_MCAST_L2.
 * A single IPMC entry is created - either <FID, DIP> when SSM is disabled or <FID, DIP, SIP> otherwise.
 */
int ipmc_ssm_mode_l3mcastl2(int unit, int inP, int outP0, int outP1, int outP2, bcm_ip_t sip_uc_ip, bcm_ip_t dip_mc_ip, int vlan1, int vlan2)
{
    int rv = BCM_E_NONE;
    int ipmc_l3mcastl2 = soc_property_get(unit, "ipmc_l3mcastl2_mode", 0);
    int ipmc_ssm_l2 = soc_property_get(unit, "ipmc_l2_ssm_mode", 0);
    int L2_flag = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_set_entry_type_by_rif", 0) == 1) ? 0 : BCM_IPMC_L2;

    bcm_mac_t da_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02};
    bcm_mac_t da_match = {0x01, 0x00, 0x5E, 0x00, 0x00, 0xFF};
    ipmc_flows_init(unit, outP0, outP1, outP2);
    int nof_ports = 2;
    int switch_enable = 1;
    int flags = BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP;
    /*1. Create MC group for packet replication */
    rv = bcm_multicast_create(unit, flags, &(ipmc_flows_info1.ipmc_mc_groups[0])); 
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf ("Created an L3 MC group with id %d. \n", ipmc_flows_info1.ipmc_mc_groups[0]);
    /*2. Add ports for replication to the Mc groups.*/
    rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[0], ipmc_flows_info1.forwarding_ports_2, nof_ports, vlan1);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, add_ingress_multicast_forwarding \n");
        return rv;
    }
    
    /*3. Create L3 interfaces using bcm_l3_intf_create. */
    bcm_l3_intf_t intf;
    rv = create_rif_vlan(da_mac, unit, vlan1, &intf);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, create_rif_vlan\n");
        return rv;
    }
    /*4. Update the RIF to be ipmc-disabled using bcm_l3_ingress_create with flags BCM_L3_INGRESS_REPLACE and:
        * BCM_L3_INGRESS_L3_MCAST_L2 and BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST - for ipmc_l3mcastl2_mode = 1
        * BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST - for ipmc_l3mcastl2_mode = 0
    */
    if (ipmc_l3mcastl2 == 1)
    {
        /*If the soc propr is set to 1, then we will define the RIF with the L3_INGRESS_L3_MCAST_L2 flag*/
        rv = disable_rif_ipmc_flags(unit, &intf);
        if (rv != BCM_E_NONE) 
        {
            printf ("Error, disable_rif_ipmc_flags - %d\n", rv);
            return rv;
        }
    }
    else
    {
        rv = enable_rif_ipmc(unit, &intf, 0);
        if (rv != BCM_E_NONE) 
        {
            printf ("Error, enable_rif_ipmc");
            return rv;
        }
        /*5. Set switch bcmSwitchL3McastL2 to 1 using bcm_switch_control_set.*/
        rv = bcm_switch_control_set(unit,bcmSwitchL3McastL2,switch_enable);
        /*If soc prop ipmc_l3mcastl2_mode is set to 0, then this switch control can be used. 
        If it is 1 then the switch control is not supported and BCM_E_CONFIG will be returned.
        */
        if (rv != BCM_E_NONE) 
        {
            printf ("Error, bcm_switch_control_set");
            return rv;
        }
    }
    
    /*6. Create an IPMC entry with key <FID, DIP1> using bcm_ipmc_add. */
    /* defining entries for LEM which will forward the packet to the relevant mc group - Create an entry <FID, DIP> */
    if (ipmc_ssm_l2) 
    {
        /* create an bridge entry SSM <FID, DIP, SIP> */
        rv = create_forwarding_entry_dip_sip_inner(unit, dip_mc_ip, sip_uc_ip ,ipmc_flows_info1.ipmc_mc_groups[0], vlan1,1,L2_flag);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, create_forwarding_entry_dip_sip_inner1\n");
            return rv;
        }
    }
    else 
    {
        /* If ipmc_l2_ssm is disabled, a <FID, DIP> entry will be created.*/
        rv = create_forwarding_entry_dip_sip_inner(unit, dip_mc_ip, 0x0, ipmc_flows_info1.ipmc_mc_groups[0], vlan1,1,L2_flag);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, create_forwarding_entry_dip_sip_inner2\n");
            return rv;
        }
    }
    return rv;
}


int create_forwarding_entry_dip_sip3 (int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan, int l3_intf, int vrf) 
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xFFFFFFFF;
    data.s_ip_addr = src_ip;
    data.s_ip_mask = 0xFFFFFFFF;
    data.vid = vlan;
    data.vrf = vrf;
    data.flags = 0;
    data.group = ipmc_index;
    data.l3a_intf = l3_intf;
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_ipmc_add in create_forwarding_entry_dip_sip3 \n");
        return rv;
    }
    return rv;
}




/*
 * The cint function creates three different configurations :
 *      - ipmc enabled, ipmc-disabled, ipmc-disabled with IPMC check.
 * Three MC groups are created - L2, and two L3.
 * Each of them has a different vlan destination and different replication ports.
 * Three RIFs are created - one with DISABLE_IP4_MCAST, one with L3_MCAST_L2 and one is not reconfigured.
 * One L2 entry is created with key <FID, DA>.
 * Two IPMC entries are created - one with key <VRF, DIP, SIP, RIF> and one with key <FID, DIP> (by providing flag BCM_IPMC_L2)
 * If the soc property ipmc_enable is set to 0, then the expected return message when creating the IPMC entries is BCM_E_UNAVAIL.
 */
int soc_prop_ipmc_enable (int unit, int iport, int oport0, int oport1, int oport2, int vlan1, int vlan2, int vlan3, bcm_ip_t mc_dip, bcm_ip_t uc_sip, int ipv4_flag) {
    int rv = BCM_E_NONE;
    printf("Received vlans: %d, %d\n", vlan1, vlan2);
    bcm_mac_t da_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02};
    bcm_mac_t da_match = {0x01, 0x00, 0x5E, 0x00, 0x03, 0x02};
    bcm_mac_t da_match_v6 = {0x33, 0x33, 0x00, 0x00, 0x11, 0x00};
    int ipmc_enable = soc_property_get(unit, "ipmc_enable", 0);
    bcm_ip6_t mc_dip_v6 = {0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x11};
    ipmc_flows_init(unit, oport0, oport1, oport2);
    int flags [3] = {BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 
                     BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP,
                     BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP};
    int nof_groups = 3;
    int nof_ports = 2;
    int level_group[3] = {2, 3, 3};
    int group;
    int frw_ports[3][2]={{oport0, oport1},
                         {oport1, oport2},
                         {oport0, oport2}};
    int vlans[3] = {vlan1, vlan2, vlan3};
    /*vlan1 leads to an L2 MC group with oport0 and oport1 replication ports. vlan2 leads to an L3 Mc group with oport1 and oport2 as replication ports.*/
    /*Create MC groups for packet replication - L2 and L3 groups*/
    for (group = 0; group < nof_groups; group++ )
    {
        rv = bcm_multicast_create(unit, flags[group], &(ipmc_flows_info1.ipmc_mc_groups[group])); 
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        printf ("Created an L%d MC group with id %d. \n", level_group[group], ipmc_flows_info1.ipmc_mc_groups[group]);
        /*Add ports for replication to the Mc groups.*/
        rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[group], frw_ports[group], nof_ports, vlans[group]);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, add_ingress_multicast_forwarding \n");
            return rv;
        }
    }
    
    /*Create three RIFs.*/
    bcm_l3_intf_t in_intf[3];
    int nof_intf = 3, intf;
    int disable = 0;
    for (intf = 0; intf < nof_intf; intf++) 
    {
        rv = create_rif_vlan(da_mac, unit, vlans[intf], &in_intf[intf]);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, create_rif_vlan\n");
            return rv;
        }
    }
    rv = enable_rif_ipmc(unit, &in_intf[0], disable);
    if (rv != BCM_E_NONE) 
    {
        printf ("Error, disable_rif_ipmc_flags - %d\n", rv);
        return rv;
    }
    rv = disable_rif_ipmc_flags(unit, &in_intf[1]);
    if (rv != BCM_E_NONE) 
    {
        printf ("Error, enable_rif_ipmc - %d\n", rv);
        return rv;
    }

    if (ipv4_flag == 1)
    {
        /*Create forwarding entry <FID, DIP>*/
        rv = create_forwarding_entry_dip_sip_inner(unit, mc_dip, 0x0, ipmc_flows_info1.ipmc_mc_groups[1], vlans[1], 1,BCM_IPMC_L2);
        if ((ipmc_enable == 1 && rv != BCM_E_NONE) || (ipmc_enable == 0 && rv != BCM_E_UNAVAIL)) 
        {
            printf("Error, create_forwarding_entry_dip_sip_inner - soc prop %d\n", ipmc_enable);
            return rv;
        }
        /*Create <VRF, DIP, SIP, RIF> forwarding entry*/
        
        rv = create_forwarding_entry_dip_sip3(unit, mc_dip, uc_sip, ipmc_flows_info1.ipmc_mc_groups[2], vlans[2], -1, vlans[2]);
        if ((ipmc_enable == 1 && rv != BCM_E_NONE) || (ipmc_enable == 0 && rv != BCM_E_UNAVAIL)) 
        {
            printf("Error, create_forwarding_entry_dip_sip2 - soc prop %d\n", ipmc_enable);
            return rv;
        }
        /*Create forwarding entry in MACT.LEM <FID, DA>*/
        rv = create_l2_forwarding_entry_da(unit, da_match, ipmc_flows_info1.ipmc_mc_groups[0], vlans[0]);
        if (rv != BCM_E_NONE) {
            printf("Error, create_l2_forwarding_entry_da\n");
            return rv;
        }
    }
    else
    {
        rv = create_forwarding_entry_dip_sip_ipv6(unit, mc_dip_v6, ipmc_flows_info1.ipmc_mc_groups[2], vlans[2]);
        if ((ipmc_enable == 1 && rv != BCM_E_NONE) || (ipmc_enable == 0 && rv != BCM_E_UNAVAIL)) 
        {
            printf("Error, create_forwarding_entry_dip_sip_ipv6 - soc prop %d\n", ipmc_enable);
            return rv;
        }
        /*Create forwarding entry in MACT.LEM <FID, DA>*/
        rv = create_l2_forwarding_entry_da(unit, da_match_v6, ipmc_flows_info1.ipmc_mc_groups[0], vlans[0]);
        if (rv != BCM_E_NONE) {
            printf("Error, create_l2_forwarding_entry_da\n");
            return rv;
        }
    }
    
    return rv;
}



/* This cint creates a configuration for testing the rpf-eligibility of a RIF.
 * It creates three MC groups - one for the RPF case and two for the two different RIFs.
 * Four IPMC entries are created - two of which lead to a FEC for SIP-based RPF check and one for explicit.
 * The other one leads to a MC group.
 * Only one entry for SIP RPF check is defined in order to test both fail and success of RPF check.
 */
int ipmc_rpf_eligible_rif (int unit, int vlan1, int vlan2, bcm_ip_t dip_1, bcm_ip_t dip_2, bcm_ip_t sip_1, bcm_ip_t sip_2, int inPort, int outPort0, int outPort1, int outPort2) 
{
    printf("vars: %d, %d, %d, %d, %d, %d, %d \n", unit, vlan1, vlan2, inPort, outPort0, outPort1, outPort2);
    int rv = BCM_E_NONE;
    bcm_mac_t da_mac_in = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02};
    bcm_mac_t da_mac_eg = {0x01, 0x00, 0x5E, 0x00, 0x03, 0x02};
    uint32 full_mask = 0xFFFFFFFF;
    bcm_l3_intf_t l3_intf[2];
    bcm_if_t frwd_l3_eg_id;
    int nof_ports = 2;
    bcm_if_t l3_eg_id;
    int vlan_if[2] = {vlan1, vlan2};
    int nof_intf = 2, intf;
    int nof_groups = 3;
    ipmc_flows_init(unit, outPort0, outPort1, outPort2);
    
    /*1. Define trap configurations for failed RPF lookup using trap codes bcmRxTrapMcExplicitRpfFail and bcmRxTrapMcUseSipRpfFail.*/
    int traps_types[2] = {bcmRxTrapMcExplicitRpfFail, bcmRxTrapMcUseSipRpfFail};
    int trap_handle;
    bcm_rx_trap_config_t config_trap;
    int trap_count = 2;
    int trap, group;
    for (trap = 0; trap < trap_count; trap++) 
    {
        bcm_rx_trap_type_create(unit, 0, traps_types[trap], &trap_handle);
        bcm_rx_trap_config_t_init(&config_trap);
        config_trap.flags = (BCM_RX_TRAP_UPDATE_DEST);
        config_trap.trap_strength = 5;
        config_trap.dest_port =BCM_GPORT_BLACK_HOLE;
        rv = bcm_rx_trap_set(unit,trap_handle,&config_trap);
        if (rv != BCM_E_NONE) 
        {
           printf("Error, bcm_rx_trap_set \n");
           return rv;
        }
    }
    printf("Created trap configurations for SIP-based and explicit RPF. \n");
    /*2. Create two L3 interfaces using bcm_l3_intf_create - RIF1, RIF2. */
    for (intf = 0; intf < nof_intf ; intf++)
    {
        bcm_l3_intf_t_init(&l3_intf[intf]);
        sal_memcpy(l3_intf[intf].l3a_mac_addr, da_mac_in, 6);
        l3_intf[intf].l3a_vid = vlan_if[intf];
        l3_intf[intf].l3a_vrf = vlan_if[intf];
        rv = bcm_l3_intf_create(unit, &l3_intf[intf]);
        printf("interface number is 0x%x\n",l3_intf[intf].l3a_intf_id);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_intf_create - iter %d\n", intf);
            return rv;
        }
        printf("Created IF with ID %d\n", l3_intf[intf].l3a_intf_id );
    }
    
    /*3. Create three L3 MC groups and add replication ports to them*/
    int mc_group_dest[3] = {l3_intf[0].l3a_intf_id, l3_intf[1].l3a_intf_id, vlan1};
    int forw_ports[3][2] = {{outPort1, outPort2},
                            {outPort0, outPort1},
                            {outPort0, outPort2}};
    for (group = 0; group < nof_groups; group++)
    {
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[group]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        printf("Created L3 MC group - %d \n", ipmc_flows_info1.ipmc_mc_groups[group]);
        rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[group], forw_ports[group], nof_ports, mc_group_dest[group] );
        if (rv != BCM_E_NONE) 
        {
            printf("Error bcm_ingress_multicast_forwarding1 \n");
            return rv;
        }
    }
    printf("Added ports for multicast replications to all groups \n");
    /*4. Create four IPMC entries - each one with a different dip, sip, vid combination
    * Each entry has a different destination.
    */
    int nof_ipmc_entries = 4;
    bcm_ip_t ipmc_dip [4] = {dip_1, dip_1, dip_2, dip_2};
    bcm_ip_t ipmc_sip [4] = {sip_1, sip_1, sip_2, sip_2};
    int ipmc_index [4] = {0, 0, 0, ipmc_flows_info1.ipmc_mc_groups[2]};
    int ipmc_vlan [4] = {vlan1, vlan2, vlan2, vlan1};
    /*ipmc_intf ID will later be redefined if its value is not 0.*/
    int ipmc_intf [4] = {1, 1, 2, 0};
    int ipmc_vrf [4] = {vlan1, vlan2, vlan2, vlan1};
    int entry;
    for (entry = 0; entry < nof_ipmc_entries; entry++) 
    {
        if (ipmc_intf[entry] != 0) 
        {
            /*Creating a FEC to have a payload for the SIP check (won't be used for actual forwarding)*/
            bcm_gport_t mc_gport_l;
            BCM_GPORT_MCAST_SET(mc_gport_l, ipmc_flows_info1.ipmc_mc_groups[0]);
            create_l3_egress_s l3_eg;
            sal_memcpy(l3_eg.next_hop_mac_addr, da_mac_eg, 6);
            l3_eg.l3_flags |= BCM_L3_RPF;
            if (ipmc_intf[entry] == 2) 
            {
                l3_eg.l3_flags |= BCM_L3_IPMC;
                l3_eg.out_tunnel_or_rif =  l3_intf[1].l3a_intf_id;
                BCM_GPORT_MCAST_SET(mc_gport_l, ipmc_flows_info1.ipmc_mc_groups[1]);
            }
            else 
            {
                l3_eg.out_tunnel_or_rif =  l3_intf[0].l3a_intf_id;
            }
            l3_eg.out_gport  = mc_gport_l;
            /* function call from cint_utils_l3.c */
            rv = l3__egress__create(unit, &l3_eg); 
            if (rv != BCM_E_NONE) 
            {
               printf("Error, l3__egress__create \n");
               return rv;
            }
            ipmc_intf[entry] = l3_eg.fec_id;
        }
        rv = create_forwarding_entry_dip_sip3(unit, ipmc_dip[entry], ipmc_sip[entry], ipmc_index[entry], ipmc_vlan[entry], ipmc_intf[entry], ipmc_vrf[entry]);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, create_forwarding_entry_dip_sip3 \n");
            return rv;
        }
    }
    
    /*5. Create route entries for the verification of the SIP. (part of RPF check)*/
    bcm_l3_route_t l3route1;
    bcm_l3_route_t_init(&l3route1);
    l3route1.l3a_subnet = sip_1;
    l3route1.l3a_ip_mask = full_mask;
    l3route1.l3a_vrf = vlan1;
    /* dest is FEC + OutLif */
    l3route1.l3a_intf = ipmc_intf[0];     /* fec */
    rv = bcm_l3_route_add(unit, &l3route1);
    if (rv != BCM_E_NONE) 
    {
        printf ("bcm_l3_route_add failed: %d \n", rv);
        return rv;
    }
    bcm_l3_route_t l3route2;
    bcm_l3_route_t_init(&l3route2);
    l3route2.l3a_subnet = sip_2;
    l3route2.l3a_ip_mask = full_mask;
    l3route2.l3a_vrf = vlan2;
    /* dest is FEC + OutLif */
    l3route2.l3a_intf = ipmc_intf[2];     /* fec */
    rv = bcm_l3_route_add(unit, &l3route2);
    if (rv != BCM_E_NONE) 
    {
        printf ("bcm_l3_route_add failed: %d \n", rv);
        return rv;
    }

    return rv;
}

int create_forwarding_entry_dip_sip3 (int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan, int l3_intf, int vrf) 
{
    int rv = BCM_E_NONE;
    
    bcm_ipmc_addr_t data;
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.mc_ip_mask = 0xffffffff;
    data.s_ip_addr = src_ip;
    data.s_ip_mask = 0xffffffff;
    data.vid = vlan;
    data.vrf = vrf;
    data.flags = 0;
    data.group = ipmc_index;
    data.l3a_intf = l3_intf;
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_ipmc_add in create_forwarding_entry_dip_sip3 \n");
        return rv;
    }
    return rv;
}


int mc_rpf_trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;
    int traps_types[2] = {bcmRxTrapMcExplicitRpfFail, bcmRxTrapMcUseSipRpfFail};
    int nof_traps = 2;
    int i;
    int trap_id;
    for (i = 0; i<nof_traps; i++)
    {
        /*get trap id*/
        rv = bcm_rx_trap_type_get(unit, 0, traps_types[i], &trap_id);
        if (rv != BCM_E_NONE) 
        {
            printf("Error in : bcm_rx_trap_type_get, returned %d\n", rv);
            return rv;
        }
        /*delete trap*/
        rv = bcm_rx_trap_type_destroy(unit, trap_id);
        if (rv != BCM_E_NONE) 
        {
            printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
            return rv;
        }
    }
    return rv;
}
