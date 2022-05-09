/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Service using EEI~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_service_using_eei.c
 * Purpose: Demo service in which EEI could be resolved at ingress.
 *
 *  1. PWE using EEI+FEC as learning data, EEI point to MPLS push command;
 *  2. PWE using EEI+FEC as learning data, EEI point to PWE EEDB;
 *  3. L3 routing resolving EEI+DEST, EEI point to ARP EEDB.
 *  4. L3 routing resolving EEI+DEST, EEI resolving push-profile+MPLS label.
 *  5. VPWS service, AC--PWE(EEI+Dest), TBD
 *  6. VPLS P2P service, AC--PWE(outlif+Dest), get outlif(PWE)+dest from AC.
 *     The dest is a pointer to FEC in which EEI is resolved, and EEI is a pointer
 *     to EEDB for LSP x 2.
 *
 * Test Scenario(3)
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_mpls.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/dnx/mpls/cint_dnx_mpls_ingress_push.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_service_using_eei.c
 * cint
 * initialize_required_system_tools(0);
 * service_using_eei_l3_route_example(0,200,202);
 * exit;
 *
 * 1: IP host lookup into EEI with MPLS Tunnel: payo127.255.255.14oE  ->  payo127.255.255.14o1000o400o401o402oE.
 * tx 1 psrc=200 data=0x0000000000110000000000008100706408004500004c00000000403dfb61000000007fffff14000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be:
 *  Source port: 200, Destination port: 202
 *  Data: 0x000000000022000000000011810000c8884700191014001901144500004c000000003f3dfc61000000007fffff14000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  2: IP host lookup into EEI as mpls push commnd: payo127.255.255.15oE  ->  payo127.255.255.15o5555oE
 *  tx 1 psrc=200 data=0x0000000000110000000000008100706408004500004c00000000403dfb60000000007fffff15000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be:
 *  Source port: 200, Destination port: 202
 *  Data: 0x00000022000000000011810000c88847015b313f4500004c000000003f3dfc60000000007fffff15000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b000000000000
 */

 struct cint_service_using_eei_s
 {
    int vpls_pwe_port_ingress_label[2];  /*pwe incoming label*/
    int vpls_pwe_port_egress_label[2];   /*pwe  outgoing label*/
    int vpls_pwe_port_encap_id ;         /*pwe  encap_id*/

    bcm_ip_t host_ip[3];                    /*host ip for IP host fwd*/
    int      fec_with_arp_id;            /*FEC ID, storing ARP ID*/

    int mpls_label_using_push_cmd;       /*MPLS label encapsulated by MPLS push command*/
    int  ac_use_wide_data;
  };

 cint_service_using_eei_s cint_service_using_eei_info =
{

     /*
     * vpls_pwe_port_ingress_label
     */
     {1111,2222},


     /*
     * vpls_pwe_port_egress_label
     * PWE port 1: egress label is resolved from EEI mpls push command;
     * PWE port 2: egress label is resolved from EEDB, pointed by EEI
     */
     {3333,4444},

     /*
      *vpls_pwe_port_encap_id
      * for PWE port 2
      */
     19999,

     /*
     * host_ip
     */
     {
        0x7FFFFF14, /* 127.255.255.14 */
        0x7FFFFF15, /* 127.255.255.15 */
        0x80FFFF15, /* 128.255.255.15 */
     },
     /*
     * fec_with_arp_id
     */
     0, /*auto allocation*/

     /*
     * mpls_label_using_push_cmd
     */
     5555
};

/**
  * Main entrance for L3 service example
  * INPUT:
  *   inP    - incoming Port
  *   outP   - outgoing Port
*/
int
service_using_eei_l3_route_example(
     int unit,
     int inP,
     int outP)
{
    int rv;
    dnx_utils_l3_fec_s fec_structure;
    bcm_l3_host_t l3host;
    int   eei;
    char *proc_name;

    proc_name = "service_using_eei_l3_route_example";

    /*
     * when EEI is used for L3 fwd, there are 2 scenarios:
     * 1. IP host table lookup-------->EEI+destination(FEC)
     * 2. IP route table lookup-------->FEC(EEI+destination)
     */


    /*
     * Note:
     * it only need the L3 basic setup(L3 Intf, ARP)
     * and mpls tunnel encapsulation function.
     */
    rv = mpls_ingress_push_l3_example(unit,inP,outP);
    if (rv != BCM_E_NONE)
    {
    printf("Error, mpls_ingress_push_l3_example\n");
    return rv;
    }

    /*
     * Case 3, EEI is used as EEDB pointer
     */
    /*
     * step 1: create FEC for arp, format is Out-LIF + destination
     */
    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);

    fec_structure.destination = outP;
    fec_structure.tunnel_gport = cint_mpls_ingress_push_info.out_arp_id;
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    cint_service_using_eei_info.fec_with_arp_id = fec_structure.l3eg_fec_id;


    /*
     * step 2: add host table entry, with EEI+FEC result type
     */
    int encap_id;
    /** In case of JR2, no needs to pass a 2nd outlif by EEI, just by outlif itself.*/
    BCM_FORWARD_ENCAP_ID_VAL_SET(encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF, BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL, cint_mpls_ingress_push_info.mpls_tunnel_id);
    rv = add_host_ipv4(unit, cint_service_using_eei_info.host_ip[0],
                        cint_mpls_ingress_push_info.vrf,
                        cint_service_using_eei_info.fec_with_arp_id,
                        encap_id,
                        0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    /*
     * Case 4, EEI is used as MPLS push command
     */

    /*
     * Step 1:add entry into host table
     */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = cint_service_using_eei_info.host_ip[1];
    l3host.l3a_vrf = cint_mpls_ingress_push_info.vrf;
    l3host.l3a_intf = cint_service_using_eei_info.fec_with_arp_id;
    l3host.l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    /*Use the default MPLS push command 0 */
    BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(eei, cint_service_using_eei_info.mpls_label_using_push_cmd,0);
    BCM_FORWARD_ENCAP_ID_VAL_SET(l3host.encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI, BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, eei);
    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_host_add on unit %d\n",unit);
        return rv;
    }

    return rv;

}



