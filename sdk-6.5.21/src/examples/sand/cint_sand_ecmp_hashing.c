/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ECMP hashing example
 */
/*
 * the cint creates an ECMP, containing 10 FEC entries, and points an IPV4 host and MPLS LSR to this ECMP.
 * each FEC points at a LL entry with the same DA, but different out-port.
 * default ECMP hashing example:
 * -----------------------------
 * run:
 * cint utility/cint_utils_l3.c
 * cint utility/cint_utils_vlan.c
 * cint cint_ip_route.c
 * cint cint_ecmp_hashing.c
 * cint
 * print ecmp_hashing_main(unit, <in_port>, <out_port>, <ecmp_size>);
 *
 * traffic example:
 * run:
 *  1) ethernet header with DA 0:C:0:2:01:23 and vlan tag id 17
 *        and IPV4 header with DIP 10.0.255.0 and various SIPs (random)
 *  2) ethernet header with DA 0:C:0:2:01:23 and vlan tag id 17
 *        MPLS header with label_1 44 and various label_2 (incremental)
 *
 * ecmp_hashing_main() does not change the default ECMP hashing configuration, which is to look at all the packet fields:
 * for IPV4 packet - DIP, SIP, protocol, dest L4 port and src L4 port.
 * for MPLS packet - fisrt label, second label and third label.
 *
 * traffic will be divided equally between the ECMP memebers (FEC entries, each pointing to a different port).
 *
 * disable ECMP hashing example:
 * -----------------------------
 * run:
 * disable_ecmp_hashing(unit);
 *
 * run same traffic. this time ECMP hashing will be set to "look at nothing".
 * none of the header fields mentioned above will be used for hashing.
 * in this case, all packets will be sent to one FEC entry.
 */

/*
 * ecmp_hashing_main():
 * 1) create ECMP, containing <ecmp_size> FEC entries, each FEC points at a LL entry with the same DA, but different out-port
 * 2) Add IPV4 host entry and point to ECMP.
 * 3) Add switch entry to swap MPLS labels and also send to ECMP
 *
 * ecmp_size = the size of the ECMP that will be created (how many FECs will it contain)
 * this will also determine the number of out-ports that will be used (because each FEC points to a different out-port)
 * out_port  = the out ports numbers will be: <out_port>, <out_port> + 1, ... , <out_port> + <ecmp_size> -1
*/
int ecmp_hashing_main(int unit, int in_port, int out_port, int ecmp_size, int slb) {

    int CINT_NO_FLAGS = 0;
    int rc, i;
    int vrf = 5;
    int in_vlan = 17;
    int out_vlan = 17;
    bcm_pbmp_t pbmp, ubmp;

    if (ecmp_size > 1000) {
        printf("Error - Max ECMP size allowed is 1000.\n");
        return BCM_E_PARAM;
    }

    int ingress_intfs[2]; /* in-RIF and out-RIF */
    int encap_ids[ecmp_size];
    bcm_if_t multipath_id; /* ECMP */

    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x23};  /* my-MAC */
    bcm_mac_t next_mac_address  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

    int host = 0x0a00ff00; /* 10.0.255.0 */
    bcm_ip6_t ipv6_host = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
    bcm_l3_host_t l3host;

    int in_label = 44;
    int eg_label = 66;
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;

    BCM_PBMP_CLEAR(ubmp);

    /* create in-RIF */
    rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rc;
    }

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;

    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[0] = intf.rif;
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /* create out-RIF */
    rc = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
        return rc;
    }

    intf.vsi = out_vlan;

    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[1] = intf.rif;
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /* create 10 FEC entries (all with the same out-RIF)
     each FEC will point to a different out-port.
     also set vlan-port membership. each out-port will have a different vlan */
    out_port--;
    for (i = 0; i < ecmp_size; i++) {
        out_port++;
        out_vlan++;

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, out_port);

        /* create FEC */
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, next_mac_address , 6);
        l3eg.allocation_flags = CINT_NO_FLAGS;
        l3eg.l3_flags = CINT_NO_FLAGS;
        if (is_device_or_above(unit, JERICHO2)){
            l3eg.out_tunnel_or_rif = 0;
        } else {
            l3eg.out_tunnel_or_rif = ingress_intfs[1];
        }
        l3eg.out_gport = out_port;
        l3eg.vlan = out_vlan;
        l3eg.fec_id = cint_ecmp_hashing_data.egress_intfs[i];
        l3eg.arp_encap_id = encap_ids[i];

        rc = l3__egress__create(unit,&l3eg);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rc;
        }

        cint_ecmp_hashing_data.egress_intfs[i] = l3eg.fec_id;
        encap_ids[i] = l3eg.arp_encap_id;

        /* set vlan port membership for out-vlan and out-port */
        rc = bcm_vlan_port_add(unit, out_vlan, pbmp, ubmp);
        if (rc != BCM_E_NONE) {
            printf ("bcm_vlan_port_add no. %d failed: %d \n", i, rc);
            print rc;
        }
    }

    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        bcm_l3_egress_ecmp_t_init(&cint_ecmp_hashing_data.ecmp);
        cint_ecmp_hashing_data.ecmp.max_paths = ecmp_size;
        if (slb) {
            cint_ecmp_hashing_data.ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        }
        /* create an ECMP, containing the FEC entries */
        rc = bcm_l3_egress_ecmp_create(unit, &cint_ecmp_hashing_data.ecmp, ecmp_size, cint_ecmp_hashing_data.egress_intfs);
        if (rc != BCM_E_NONE) {
            printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
            return rc;
        }
    } else {
        rc = bcm_l3_egress_multipath_create(unit, CINT_NO_FLAGS, ecmp_size, cint_ecmp_hashing_data.egress_intfs, &multipath_id);
        if (rc != BCM_E_NONE) {
            printf ("bcm_l3_egress_multipath_create failed: %d \n", rc);
            return rc;
        }
    }

    /* add host entry and point to the ECMP */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        l3host.l3a_intf = cint_ecmp_hashing_data.ecmp.ecmp_intf;
    } else {
        l3host.l3a_intf = multipath_id; /* ECMP */
    }

    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rc);
        return rc;
    }

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_vrf = vrf;
    l3host.l3a_flags = BCM_L3_IP6;
    l3host.l3a_ip6_addr = ipv6_host;

    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp)
    {
        l3host.l3a_intf = cint_ecmp_hashing_data.ecmp.ecmp_intf;
    } else
    {
        l3host.l3a_intf = multipath_id; /* ECMP */
    }
    
    if (is_device_or_above(unit, JERICHO)) {
        rc = bcm_l3_host_add(unit, &l3host);
        if (rc != BCM_E_NONE) {
            printf("bcm_l3_host_add failed: %x \n", rc);
            return rc;
        }
    }

    /* add switch entry to swap labels and map to ECMP */
    bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
    mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT; /* TTL decrement has to be present */
    if (!is_device_or_above(unit, JERICHO2))
    {
        mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    }
    mpls_tunnel_info.label = in_label; /* incoming label */
    mpls_tunnel_info.egress_label.label = eg_label; /* outgoing (egress) label */
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        mpls_tunnel_info.egress_if = cint_ecmp_hashing_data.ecmp.ecmp_intf;
    } else {
        mpls_tunnel_info.egress_if = multipath_id; /* ECMP */
    }

    rc = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
    if (rc != BCM_E_NONE) {
        printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rc);
        return rc;
    }

    return 0;
}

/*
 * disable_ecmp_hashing():
 * set ECMP hashing to "look at nothing".
 * no part of the header will be used in hashing.
 * in this case, hashing result will be the same for every packet that arrives.
 */
int disable_ecmp_hashing(int unit) {
    int rc;
    int arg = 0; /* arg = 0 so no field in the Eth/IPV4/MPLS header will be looked at */
    bcm_switch_control_t type;

    /* disable L2 hashing*/
    type = bcmSwitchHashL2Field0;
    rc = bcm_switch_control_set(unit, type, arg);
    if (rc != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchHashL2Field0 failed: %d \n", rc);
    }

    /* disable IPV4 hashing */
    type = bcmSwitchHashIP4Field0;
    rc = bcm_switch_control_set(unit, type, arg);
    if (rc != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 failed: %d \n", rc);
    }

    /* disable MPLS hashing */
    type = bcmSwitchHashMPLSField0;
    rc = bcm_switch_control_set(unit, type, arg);
    if (rc != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField0 failed: %d \n", rc);
    }

    type = bcmSwitchHashMPLSField1;
    rc = bcm_switch_control_set(unit, type, arg);
    if (rc != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField1 failed: %d \n", rc);
    }

    return rc;
}


/*
 * ecmp_hash_func_config():
 * set ECMP hashing function (polynomial).
 * bcm_hash_config = BCM_HASH_CONFIG_*
 */
int ecmp_hash_func_config(int unit, int bcm_hash_config) {
    int rc;
    bcm_switch_control_t type = bcmSwitchECMPHashConfig;
    if (is_device_or_above(unit, JERICHO2))
    {
        bcm_switch_hash_config_t arg;
        bcm_switch_control_key_t control_key;
        bcm_switch_control_info_t control_info;

        rc = bcm_switch_control_get(unit, type, &arg);
        if (rc != BCM_E_NONE)
        {
            printf("Error bcm_switch_control_get\n");
            return rc;
        }
        control_key.type = bcmSwitchHashSeed;
        control_key.index = arg;
        rc = bcm_switch_control_indexed_get(unit, control_key, control_info);
        if (rc != BCM_E_NONE)
        {
            printf("Error bcm_switch_control_indexed_get\n");
            return rc;
        }
        control_info.value = control_info.value + 0x1234;
        rc = bcm_switch_control_indexed_set(unit, control_key, control_info);
        if (rc != BCM_E_NONE)
        {
            printf("Error bcm_switch_control_indexed_set\n");
            return rc;
        }
    }
    else
    {
        rc = bcm_switch_control_set(unit, type, bcm_hash_config);
        if (rc != BCM_E_NONE) {
            printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", rc);
        }
    }
    return rc;
}

