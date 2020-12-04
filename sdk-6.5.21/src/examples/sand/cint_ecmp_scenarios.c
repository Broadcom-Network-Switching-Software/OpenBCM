/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/**
 * Test Scenario
 *   Example for configuring Equal-Cost Multi-Path (ECMP) Routing.
 *
 *   Introduction
 *
 *     ECMPs are used for load-balancing packets among several FECs.
 *     An ECMP is composed of a group of FECs.
 *     ECMPs can be configured as a destination, for example as IP routes.
 *     When a packet is sent to this route, the device will perform hashing
 *     on (some part of) the packet to decide on which FEC it should be sent.
 *     In this way packets can be load-balanced among several paths.
 *
 * Running
 *   ./bcm.user
 *   cd ../../../../regress/bcm
 *   cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 *   cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 *   cint ../../src/examples/sand/cint_ecmp_scenarios.c
 *   cint
 *   l3_ecmp_hash_set(0,203);
 *   exit;
 *
 *   cint
 *   l3_multipath_run_with_defaults_dvapi(0,-1,203,1,16,17,14);
 *   exit;
 *
 *   tx 1 psrc=203 data=0x0000000055342176178932468100000c08004508002e0000400040060bc6151a93ad703416013344556600a1a2a300b1b2b3500001f5d1c70000f5f5f5f5f5f5f5f5f5f5
 *
 *   Configuration
 *
 *     The example configures routing for a stream.
 *     The destination of that stream is set to be an ECMP.
 *     The ECMP is configured to contain a group of FECs,
 *     with each FEC having a different destination port.
 *
 *     Details:
 *      1. Setup ECMP hashing based on the Ethernet header of packets.
 *      2. Create an l3 ingress interface to setup routing for packets arriving on inport with
 *         vlan 12 and MAC 00:00:00:00:55:34.
 *      3. Create 4 egress interfaces (FECs) with destination ports 1-4.
 *      4. Create an ECMP group and add the FECs to it.
 *      5. Add route for 112.52.22.1, and set the ECMP as the destination.
 *
 *   Traffic:
 *
 *     Send packets from port inport with vlan 12, DA 00:00:00:00:55:34 and DIP 112.52.22.1.
 *     The device will route packets through one of the configured ports (1-4).
 *     To get the device to route to different ports, change the SA of the packet.
 */

int CMD_FAIL = -1;
int CINT_NO_FLAGS = 0;
int egress_intf[16];
int encap_id[16];
bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34};
bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};

int ecmp_is_resilant_hashing = 0;

int
add_route (
    int unit,
    bcm_if_t intf )
{
    bcm_l3_route_t ecmp_l3rt;
    uint32 route;
    int rc;

    route = ((112%192)<<24)+((52%192)<<16)+((22%192)<<8)+(1%192); /*112.52.22.1*/

    bcm_l3_route_t_init(&ecmp_l3rt);
    ecmp_l3rt.l3a_flags = 0;
    ecmp_l3rt.l3a_subnet = route;
    ecmp_l3rt.l3a_ip_mask = 0xfffff000;
    ecmp_l3rt.l3a_intf = intf;

    rc = bcm_l3_route_add(unit, &ecmp_l3rt);
    if (BCM_FAILURE(rc)) {
        printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
        return CMD_FAIL;
    }
    printf("L3 multipath route created, 0x%08x (0x%08x)\n", ecmp_l3rt.l3a_subnet, ecmp_l3rt.l3a_ip_mask);

    return rc;
}

/*
 * Utilities APIs
 */
/*
 * Set ecmp hash.
 */
int
l3_ecmp_hash_set (
    int unit,
    int in_port)
{
    int rc = BCM_E_NONE;

    if (is_device_or_above(unit, JERICHO2)) {
        
        /* To do */


    } 
    else
    {
        /* hash settings */
        rc = bcm_switch_control_set(unit,bcmSwitchECMPHashSrcPortEnable ,0);
        if (rc != BCM_E_NONE)
        {
            printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashSrcPortEnable  %d: %s\n", rc, bcm_errmsg(rc));
            return rc;
        }
        rc = bcm_switch_control_set(unit,bcmSwitchECMPHashOffset ,1);
        if (rc != BCM_E_NONE)
        {
            printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashOffset  %d: %s\n", rc, bcm_errmsg(rc));
            return rc;
        }
        rc = bcm_switch_control_set(unit,bcmSwitchECMPHashSeed ,1234);
        if (rc != BCM_E_NONE)
        {
            printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashSeed  %d: %s\n", rc, bcm_errmsg(rc));
            return rc;
        }
        rc = bcm_switch_control_set(unit,bcmSwitchECMPHashConfig,BCM_HASH_CONFIG_CLOCK_BASED);
        if (rc != BCM_E_NONE)
        {
            printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashConfig  %d: %s\n", rc, bcm_errmsg(rc));
            return rc;
        }
        rc = bcm_switch_control_port_set(unit, in_port, bcmSwitchECMPHashPktHeaderCount ,1);
        if (rc != BCM_E_NONE)
        {
            printf("BCM FAIL (bcm_switch_control_set): bcmSwitchECMPHashPktHeaderCount  %d: %s\n", rc, bcm_errmsg(rc));
            return rc;
        }
    }
    return rc;
}


int
l3_multipath_run (
    int unit,
    int unit_2nd,
    int in_port,
    int vlan,
    int nof_paths,
    int *out_port_arr,
    int *modid_arr)
{
    int ingress_intf;
    int rc;
    int i;
    int gport;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_if_t ecmp_mpath_id;
    int core, tm_port;

    /* Setup routing for packets arriving on in_port with vlan vlan (param) and MAC 00:00:00:00:55:34. */
    bcm_pbmp_t pbmp;

    /* create vlan */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);

    rc = bcm_vlan_create (unit, vlan);
    if (BCM_FAILURE(rc)) {
        printf("bcm_vlan_create %d: %s\n", rc, bcm_errmsg(rc));
        return CMD_FAIL;
    }

    rc = bcm_vlan_port_add(unit, vlan, pbmp, pbmp);
    if (BCM_FAILURE(rc)) {
        printf("bcm_vlan_port_add %d: %s\n", rc, bcm_errmsg(rc));
        return CMD_FAIL;
    }

    sand_utils_l3_eth_rif_s eth_rif_structure;
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, vlan, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, mac_l3_ingress, 0);
    rc = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rc != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rc;
    }
    ingress_intf = eth_rif_structure.l3_rif;

    /* Create FECs and set ports out_port_arr[i] to be their destinations. */
    for (i=0; i<nof_paths; i++)
    {

        BCM_GPORT_LOCAL_SET(gport, out_port_arr[i]);

        /* it is assumed modid == unit id */
        l3_egress_s l3eg;

        l3eg.out_tunnel_or_rif = ingress_intf;
        sal_memcpy(l3eg.next_hop_mac_addr, &mac_l3_egress, 6);
        l3eg.vlan = vlan;
        l3eg.out_gport = gport;
        l3eg.arp_encap_id = encap_id[i];
        l3eg.fec_id = egress_intf[i];
        l3eg.allocation_flags = CINT_NO_FLAGS;
        rc = sand_utils_l3_egress_create(modid_arr[i],l3eg);
        if (BCM_FAILURE(rc)) {
            printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
            return CMD_FAIL;
        }

        printf("L3 egress created, MAC 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x on VLAN 0x%x\n",
            l3eg.next_hop_mac_addr[0],
            l3eg.next_hop_mac_addr[1],
            l3eg.next_hop_mac_addr[2],
            l3eg.next_hop_mac_addr[3],
            l3eg.next_hop_mac_addr[4],
            l3eg.next_hop_mac_addr[5], vlan);

        encap_id[i] = l3eg.arp_encap_id;
        egress_intf[i] = l3eg.fec_id ;
        if (unit_2nd >=0)
        {
            l3eg.allocation_flags = BCM_L3_WITH_ID;
            rc = sand_utils_l3_egress_create(1-modid_arr[i],l3eg);
            if (BCM_FAILURE(rc)) {
                printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
                return CMD_FAIL;
            }
            printf("L3 egress created, MAC 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x on VLAN 0x%x\n",
                l3eg.next_hop_mac_addr[0],
                l3eg.next_hop_mac_addr[1],
                l3eg.next_hop_mac_addr[2],
                l3eg.next_hop_mac_addr[3],
                l3eg.next_hop_mac_addr[4], 
                l3eg.next_hop_mac_addr[5], vlan);
        }
    }

    /* Create ECMP. */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.max_paths = nof_paths;

    if (ecmp_is_resilant_hashing) {
        ecmp.dynamic_mode = ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    }

    /* create ECMP L3 egress */
    rc = bcm_l3_egress_ecmp_create(unit, &ecmp, nof_paths, egress_intf);
    if (BCM_FAILURE(rc)) {
        printf("bcm_l3_egress_ecmp_create\n");
        printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
        return CMD_FAIL;
    }
    printf ("L3 egress multipath created: 0x%x\n", ecmp.ecmp_intf);

    /* add route to ecmp */
    rc = add_route(unit,ecmp.ecmp_intf);
    if (rc!=BCM_E_NONE)
    {
        printf("add_route\n");
        print rc;
        return rc;
    }

    return rc;
}

int
l3_multipath_run_with_defaults (
    int unit,
    int unit_2nd)
{
    int rc;
    int in_port = 1;
    int vlan = 12;
    int nof_paths = 4;
    int out_port_arr[4] = {2,3,4,5};
    int modid_arr_1_device[4] = {0,0,0,0};
    int modid_arr_2_devices[4] = {0,0,1,1};
    int *modid_arr;

    if (unit_2nd < 0)
    {
        modid_arr = modid_arr_1_device;
    }
    else
    {
        modid_arr = modid_arr_2_devices;
    }

    rc = l3_multipath_run(unit,unit_2nd,in_port,vlan,nof_paths,out_port_arr,modid_arr);
    return rc;
}

int
l3_multipath_run_with_defaults_2 (
    int unit,
    int unit_2nd,
    int in_port)
{
    int rc;
    int vlan = 12;
    int nof_paths = 4;
    int out_port_arr[4] = {1,2,3,4};
    int modid_arr_1_device[4] = {0,0,0,0};
    int modid_arr_2_devices[4] = {0,0,1,1};
    int *modid_arr;

    if (unit_2nd < 0)
    {
        modid_arr = modid_arr_1_device;
    } 
    else
    {
        modid_arr = modid_arr_2_devices;
    } 
  
    rc = l3_multipath_run(unit,unit_2nd,in_port,vlan,nof_paths,out_port_arr,modid_arr);
    return rc;
}

int
l3_multipath_run_with_defaults_dvapi (
    int unit,
    int unit_2nd,
    int inPort,
    int outPort0,
    int outPort1,
    int outPort2,
    int outPort3)
{
    int rc;
    int in_port = inPort;
    int vlan = 12;
    int nof_paths = 4;
    int out_port_arr[4] = {outPort0,outPort1,outPort2,outPort3};
    int modid_arr_1_device[4] = {unit,unit,unit,unit};
    int modid_arr_2_devices[4] = {unit,unit,unit_2nd,unit_2nd};
    int *modid_arr;

    if (unit_2nd < 0)
    {
        modid_arr = modid_arr_1_device;
    }
    else
    {
        modid_arr = modid_arr_2_devices;
    } 

    rc = l3_multipath_run(unit,unit_2nd,in_port,vlan,nof_paths,out_port_arr,modid_arr);
    return rc;
}
