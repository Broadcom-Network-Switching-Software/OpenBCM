/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev MPLS LSR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_multi_device_mpls_lsr.c
 * Purpose: Demo MPLS LSR under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively.
 * This cint demo how to configurer the MPLS LSR service under multi-device system.
 *  1. Create ARP and out-RIF configuration on egress device
 *  2. Create In-RIF and ILM entry configuration on ingress device
 *  3. Send MPLS packet to verify the result
 *
 *
 *  10 cases are tested:
 *   1.IPv4oMPLSoMPLSoEth---------->IPv4oMPLSoEth(2 labels terminated, 1 MPLS label pushed by EEI push command)
 *   2.IPv4oMPLSoMPLSoEth---------->IPv4oMPLSoEth(2 labels terminated, 1 MPLS label pushed by EEI pointer )
 *   3.MPLSoMPLSoMPLSoEth---------->MPLSoEth(2 labels terminated, 1 MPLS label swapped by EEI from ILM)
 *   4.IPv4oMPLSoMPLSoEth---------->IPv4oEth(2 labels terminated, 1 MPLS label pop by EEI)
 *   5.MPLSoMPLSoMPLSoEth---------->MPLSoEth(2 labels terminated, 1 MPLS label swapped by EEI from FEC)
 *   6.IPv4oMPLSoMPLSoEth---------->IPv4oEth(1 labels terminated, 1 MPLS label popped by EEDB from Out-LIF pointer )
 *   7.IPv6oMPLSoMPLSoEth---------->IPv6oEth(1 labels terminated, 1 MPLS label popped by  EEDB from Out-LIF pointer)
 *   8.MPLSoMPLSoMPLSoEth---------->MPLSoEth(1 labels terminated, 1 MPLS label popped by EEDB from Out-LIF pointer )
 *   9.MPLSoMPLSoMPLSoEth---------->MPLSoEth(2 labels terminated, 1 MPLS label swapped  by EEDB from Out-LIF pointer)
 *   10.IPv4oMPLSoMPLSoEth---------->IPv4oEth(2 labels terminated, 1 MPLS label swaped by EEDB from  EEI pointer)
 *
 *  Test example:
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_multi_device_ipv4_uc.c
 * cint ../../src/examples/sand/cint_sand_multi_device_mpls_lsr.c
 * cint ../../src/examples/dpp/internal/systems/cint_utils_jr2-comp-jr1-mode.c
 * cint
 * initialize_required_system_tools(0);
 * multi_dev_mpls_lsr_example(2,1,1811940041,1811939528);
 * exit;
 *
 * AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: routing into MPLS tunnel from EEI
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d11404500004c00000000403dfb70000000007fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *  Received packets on unit 1 should be:
 *  Data: 0x000000000022000000000011810010c8884700bb913f4500004c000000003f3dfc70000000007fffff05000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 * AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: routing into MPLS tunnel from EEI out-LIF
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d11404500004c00000000403dfb6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 * Received packets on unit 1 should be:
 * Data: 0x000000000022000000000011810010c8884700bba13f4500004c000000003f3dfc6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 * AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: MPLS LSR SWAP  from EEI
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d1040003e8140000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 * Received packets on unit 1 should be:
 * Data: 0x000000000022000000000011810010c8884700bb813f000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 * AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: MPLS LSR PHP  from EEI
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d1040003e91404500004c00000000403dfb6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 * Received packets on unit 1 should be:
 * Data: 0x000000000022000000000011810010c808004500004c000000003f3dfc6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *
 *  AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: MPLS LSR SWAP  from EEDB(Out-Lif) 
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d1040003ec140000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Data: 0x000000000022000000000011810000c8884700bbc13f000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000 
 *
 *  AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: MPLS LSR SWAP  from EEDB(EEI) 
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d1040003ed140000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Data: 0x000000000022000000000011810000c8884700bbc13f000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000 
 *
 *  AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: MPLS LSR PHP into IPv4 from EEDB(Out-LIF) 
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d1040003eb1404500004c00000000403dfb6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Data: 0x000000000022000000000011810000c808004500004c000000003f3dfc6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000 
 *
 *  AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: MPLS LSR PHP into IPv6  from EEDB(Out-LIF) 
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040007d1040003eb14060000000004c00400000000000001234000000000000c0ab000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Data: 0x000000000022000000000011810000c886dd60000000004c003f0000000000001234000000000000c0ab000000000000000012340000ff00ff13000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000 
 *
 *  AT_Sand_Cint_mpls_multi_dev_lsr_with_eei: 0: MPLS LSR PHP into MPLS  from EEDB(Out-LIF) 
 * tx 1 psrc=201 data=0x000000000011000000000000810070648847007d0040003eb040007d11404500004c00000000403dfb6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Data: 0x000000000022000000000011810000c88847007d113f4500004c00000000403dfb6f000000007fffff06000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000 
 */


 struct cint_multi_dev_mpls_lsr_info_s
{
    int dip_into_tunnel_using_push_cmd;                 /* dip into mpls tunnel from push command*/
    int dip_into_tunnel_using_eedb;                     /* dip into mpls tunnel from EEI-pointer*/
    int incoming_label[6];               /* Incoming mpls label */
    int mpls_tunnel_label[2];            /* Incoming mpls label to termination */

    int outgoing_label[5];               /* Outgoing mpls label */

    int is_pipe; /* uniform or pipe for mpls ingress and egress */

    int fec_id_for_arp;                  /*FEC ID for arp, for case 1,2,3,4*/
    int fec_id_for_eei_push_cmd;           /*FEC ID for eei push command, for case 1*/
    int fec_id_for_eei_swap_cmd;           /*FEC ID for eei swap command, for case 1*/
    int fec_id_for_eei_pointer;       /*FEC ID for IP routing using eei as pointer, for case 2*/
    int fec_id_for_swap_outlif_pointer;       /*FEC ID for MPLS SWAP using outlif as tunnel EEDB pointer, for case 9*/
    int fec_id_for_swap_eei_pointer;       /*FEC ID for MPLS SWAP using EEI  as tunnel EEDB pointer, for case 10*/
    int fec_id_for_pop_outlif_pointer;       /*FEC ID for MPLS PHP from EEDB,using outlif as pointer, for case 6,7,8*/

    int egr_mpls_tunnel_encap_id;     /* encap_id for mpls tunnel, for case 2*/
    int swap_mpls_tunnel_encap_id;     /* encap_id for mpls tunnel, for case 9,10*/
    int php_mpls_tunnel_encap_id;     /* encap_id for mpls tunnel, for case 6,7,8*/
};


 cint_multi_dev_mpls_lsr_info_s cint_mpls_lsr_info=
{
    /*
    * dip_into_tunnel_using_push_cmd
    */
    0x7FFFFF05, /* 127.255.255.5 */

    /*
    * dip_into_tunnel_using_eedb
    */
    0x7FFFFF06, /* 127.255.255.6 */

    /*
    * Incoming mpls label,
    */
    {1000,1001,1002,1003,1004,1005},

    /*
    * mpls tunnel label,
    */
    {2000,2001},

    /*
    * outgoing label,
    */
    {3000,3001,3002,3003,3004},

    /*
    * is_pipe
    */
    0
};



/**
 * Initiation for system parameter
 * INPUT:
 *   ingress_unit- traffic incoming unit
 *   egress_unit- traffic ougtoing unit
 *   in_sys_port- traffic incoming port
 *   out_sys_port- traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;
    rv = multi_dev_ipv4_uc_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_init\n");
     return rv;
    }

    /*
     * In case 1, there 2 level FECs,
     * the ARP is in the 2nd level
     */
    if (is_device_or_above(ingress_unit, JERICHO2)) {
        int fec_id;
        rv = get_first_fec_in_range_which_not_in_ecmp_range(ingress_unit, 0, &fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_eei_push_cmd, BCM_L3_ITF_TYPE_FEC, fec_id++);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_eei_pointer, BCM_L3_ITF_TYPE_FEC, fec_id++);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_eei_swap_cmd, BCM_L3_ITF_TYPE_FEC, fec_id++);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_swap_outlif_pointer, BCM_L3_ITF_TYPE_FEC, fec_id++);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_swap_eei_pointer, BCM_L3_ITF_TYPE_FEC, fec_id++);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_pop_outlif_pointer, BCM_L3_ITF_TYPE_FEC, fec_id++);
       rv = get_first_fec_in_range_which_not_in_ecmp_range(ingress_unit, 1, &fec_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
           return rv;
       }
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_arp, BCM_L3_ITF_TYPE_FEC, fec_id);
    } else {
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_arp, BCM_L3_ITF_TYPE_FEC, 0x1000);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_eei_push_cmd, BCM_L3_ITF_TYPE_FEC, 0x5000);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_eei_pointer, BCM_L3_ITF_TYPE_FEC, 0x5001);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_eei_swap_cmd, BCM_L3_ITF_TYPE_FEC, 0x5002);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_swap_outlif_pointer, BCM_L3_ITF_TYPE_FEC, 0x5003);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_swap_eei_pointer, BCM_L3_ITF_TYPE_FEC, 0x5004);
       BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_pop_outlif_pointer, BCM_L3_ITF_TYPE_FEC, 0x5005);
    }

    BCM_L3_ITF_SET(cint_mpls_lsr_info.php_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF,0x3efe1);
    /*
     * In case 2, EEI is a pointer to mpls tunnel EEDB
     */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, 0x3001);
    BCM_L3_ITF_SET(cint_mpls_lsr_info.swap_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, 0x3002);

    /*
     *Sync the PUSH profile accorss multiple devices
     */
    bcm_mpls_port_t mpls_port;
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.mpls_port_id = 1;
    mpls_port.flags = BCM_MPLS_PORT_WITH_ID;
    mpls_port.flags2 = BCM_MPLS_PORT2_TUNNEL_PUSH_INFO;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (!is_device_or_above(ingress_unit, JERICHO2))
    {
        mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY;
    }

    rv = bcm_mpls_port_add(ingress_unit,0,&mpls_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, bcm_mpls_port_add on unit %d\n",ingress_unit);
     return rv;
    }

    if (ingress_unit != egress_unit) {
       rv = bcm_mpls_port_add(egress_unit,0,&mpls_port);
       if (rv != BCM_E_NONE)
       {
            printf("Error, bcm_mpls_port_add on unit %d\n",egress_unit);
	    return rv;
       }
    }

    return rv;
}

/**
 * FEC entry adding
 * INPUT:
 *   in_port    - traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_fec_entry_configuration(
    int unit,
    int in_port,
    int out_sys_port)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    /*
     * 1. FEC for ARP,in 2nd level FEC
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = out_sys_port;
    fec_structure.tunnel_gport = cint_ipv4_route_info.arp_encap_id;
    fec_structure.flags        = BCM_L3_2ND_HIERARCHY;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.fec_id       = cint_mpls_lsr_info.fec_id_for_arp;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }


    /*
     * 2. FEC for mpls push cmd,point to 2nd level FEC(arp)
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_FORWARD_PORT_SET(fec_structure.destination,cint_mpls_lsr_info.fec_id_for_arp);
    fec_structure.mpls_action = BCM_MPLS_EGRESS_ACTION_PUSH;
    fec_structure.mpls_label  = cint_mpls_lsr_info.outgoing_label[1];
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.fec_id      = cint_mpls_lsr_info.fec_id_for_eei_push_cmd;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    cint_mpls_lsr_info.fec_id_for_eei_push_cmd = fec_structure.l3eg_fec_id;

    /*
     * 3. FEC for mpls tunnel eedb ,point to 2nd level FEC(arp)
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_FORWARD_PORT_SET(fec_structure.destination,cint_mpls_lsr_info.fec_id_for_arp);
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    fec_structure.tunnel_gport = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;
    fec_structure.fec_id      = cint_mpls_lsr_info.fec_id_for_eei_pointer;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * 4. FEC for mpls swap cmd,point to 2nd level FEC(arp)
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_FORWARD_PORT_SET(fec_structure.destination,cint_mpls_lsr_info.fec_id_for_arp);
    fec_structure.mpls_action = BCM_MPLS_EGRESS_ACTION_SWAP;
    fec_structure.mpls_label  = cint_mpls_lsr_info.outgoing_label[3];
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.fec_id      = cint_mpls_lsr_info.fec_id_for_eei_swap_cmd;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * 5. FEC for swap  mpls tunnel eedb ,using Out-LIF as pointer
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = out_sys_port;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.tunnel_gport = cint_mpls_lsr_info.swap_mpls_tunnel_encap_id;
    fec_structure.fec_id      = cint_mpls_lsr_info.fec_id_for_swap_outlif_pointer;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * 6. FEC for swap  mpls tunnel eedb ,using EEI as pointer
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = out_sys_port;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    fec_structure.tunnel_gport = cint_mpls_lsr_info.swap_mpls_tunnel_encap_id;
    fec_structure.fec_id      = cint_mpls_lsr_info.fec_id_for_swap_eei_pointer;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    /*
     * 7. FEC for php  mpls tunnel eedb ,using out-lif as pointer
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination = out_sys_port;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.tunnel_gport = cint_mpls_lsr_info.php_mpls_tunnel_encap_id;
    fec_structure.fec_id      = cint_mpls_lsr_info.fec_id_for_pop_outlif_pointer;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    return rv;
}

/**
 * forward entry adding(ILM, IPv4 Route)
 * INPUT:
 *   unit    - traffic incoming unit
*/
 int
multi_dev_mpls_lsr_forward_entry_configuration(
    int unit,
    int out_sys_port)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * 1. For case 1, push over IPv4
     */
    rv = add_host_ipv4(unit, cint_mpls_lsr_info.dip_into_tunnel_using_push_cmd, cint_ipv4_route_info.vrf, cint_mpls_lsr_info.fec_id_for_eei_push_cmd, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /*
     * 2. For case 2, push over IPv4 using eedb
     */
    rv = add_host_ipv4(unit, cint_mpls_lsr_info.dip_into_tunnel_using_eedb, cint_ipv4_route_info.vrf, cint_mpls_lsr_info.fec_id_for_eei_pointer, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }


    /*
     * 3. For case 3, swap over MPLS
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[0];
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.egress_label.label = cint_mpls_lsr_info.outgoing_label[0];
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to fec for arp */
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
     * 3. For case 4, PHP over MPLS
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[1];
    entry.action = BCM_MPLS_SWITCH_ACTION_PHP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
    if (is_device_or_above(unit, JERICHO2)) {
        entry.egress_label.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        entry.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    } else {
        entry.flags |=  BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
     * 5. For case 5, swap by EEI from FEC
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[2];
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to fec for arp */
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_eei_swap_cmd;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
     * 6. For case 6,7,8, pop from eedb from out-lif
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[3];
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to fec for arp */
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_pop_outlif_pointer;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
     * 7. For case 9, swap from eedb from out-lif
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[4];
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to fec for arp */
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_swap_outlif_pointer;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {   
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
     * 8. For case 10, swap from eedb from eei
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[5];
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to fec for arp */
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_swap_eei_pointer;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {   
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

/**
 * mpls tunnel termination entry adding
 * INPUT:
 *   unit    - traffic incoming unit
*/
 int
multi_dev_mpls_lsr_tunnel_termination_configuration(
    int unit)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv, i;

    /*
     * step 1: Create a stack of MPLS labels to be terminated
     */
    for (i = 0; i < 2; i++)
    {
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = BCM_MPLS_SWITCH_ACTION_POP;
        entry.label = cint_mpls_lsr_info.mpls_tunnel_label[i];
        entry.vpn = cint_ipv4_route_info.vrf;
        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }
    }

    return rv;
}

/**
 * MPLS tunnel adding
 * INPUT:
*/
int
multi_dev_mpls_lsr_egress_mpls_tunnel_configuration(
    int unit)
{
    int rv;
    bcm_mpls_egress_label_t label_array[4];

    /**
     * step 1: common tunnel.
     * for case 2
     */
    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].label = cint_mpls_lsr_info.outgoing_label[2];
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_COPY;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
    label_array[0].tunnel_id = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;

    /** Create the mpls tunnel with one call.*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /**
     * step 2: swap tunnel.
     * for case 9,10
     */
    bcm_mpls_egress_label_t_init(&label_array[0]);
    
    label_array[0].label = cint_mpls_lsr_info.outgoing_label[4];
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_ACTION_VALID | BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_COPY;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP;
    label_array[0].tunnel_id = cint_mpls_lsr_info.swap_mpls_tunnel_encap_id;
    label_array[0].l3_intf_id = cint_ipv4_route_info.arp_encap_id;
    
    /** Create the mpls tunnel with one call.*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE)
    {   
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /**
     * step 2: PHP tunnel.
     * for case 6,7,8
     */
    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_ACTION_VALID | BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_COPY;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        label_array[0].label = 0;
    }
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_PHP;
    label_array[0].tunnel_id = cint_mpls_lsr_info.php_mpls_tunnel_encap_id;
    label_array[0].l3_intf_id = cint_ipv4_route_info.arp_encap_id;

    /** Create the mpls tunnel with one call.*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    return rv;
}

/**
 * ingress side configuration
 * INPUT:
 *   in_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_ingress_configuration(
    int ingress_unit,
    int in_port,
    int out_sys_port)
{
    int rv;

    /*
     * 1.Basic ingress L2/L3 configuration(Port,In-RIF)
     */
    rv = multi_dev_ingress_port_configuration(ingress_unit,in_port,cint_ipv4_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(ingress_unit,in_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
     return rv;
    }

    /*
     * 2. tunnel termination
     */
    rv = multi_dev_mpls_lsr_tunnel_termination_configuration(ingress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_fec_entry_configuration\n");
     return rv;
    }


    /*
     * 3. LSR relevant FEC configuration
     */
    rv = multi_dev_mpls_lsr_fec_entry_configuration(ingress_unit,in_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_fec_entry_configuration\n");
     return rv;
    }

    /*
     * 4. FWD configuration
     */
    rv = multi_dev_mpls_lsr_forward_entry_configuration(ingress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_forward_entry_configuration\n");
     return rv;
    }

    return rv;
}

/**
 * egress side configuration
 * INPUT:
 *   out_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_egress_configuration(
    int egress_unit,
    int out_port)
{
    int rv;

    /*
     * 1. Basic L3 egress configuration(Out-RIF,ARP)
     */
    rv = multi_dev_ipv4_uc_egress_configuration(egress_unit,out_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_egress_configuration\n");
     return rv;
    }

    /*
     * 2. mpls tunnel adding, for EEI pointer, case 2
     */
    rv = multi_dev_mpls_lsr_egress_mpls_tunnel_configuration(egress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error,multi_dev_mpls_lsr_egress_mpls_tunnel_configuration\n");
     return rv;
    }

    return rv;
}



/**
 * Main entrance for MPLS LSR on multi-device
 * INPUT:
 *   in_sys_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    rv = multi_dev_mpls_lsr_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_init\n");
     return rv;
    }


    rv = multi_dev_mpls_lsr_ingress_configuration(ingress_unit,cint_ipv4_route_info.in_local_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_ingress_configuration\n");
     return rv;
    }

    rv = multi_dev_mpls_lsr_egress_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_egress_configuration\n");
     return rv;
    }
    return rv;

}

