/* $Id: cint_dnx_basic_bridge.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * set following soc properties                                                                                                                     *
 *                                                                                                                                                  * 
 * This CINT demonstrates basic l2 bridging for C_VID packet using static MACT entry.                                                               * 
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *             +-------+                                                                                                                            *
 *    iPort    |       |                   +------+                                                                                                 *
 *    +------> | InLIF +------------------>|      |                                                                                                 *
 *             |       |                   |      |     +-------+   oPort                                                                           *
 *             +-------+                   |      |     |       |                                                                                   *
 *                                         | VSI  +---> | MACT  +-------------+                                                                     *
 *                                         |      |     |       |             |                                                                     *
 *                                         |      |     +-------+             |                                                                     *
 *                                         |      |                           |                                                                     *
 *                                         +------+                           |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *     oPort                                                                  |                                                                     *
 *    <-----------------------------------------------------------------------+                                                                     *
 *                                                                                                                                                  *
 * How to run:                                                                                                                                      *
 * Test Scenario - start
   cint ../../../../src/examples/dnx/l2/cint_dnx_basic_bridge.c                                                                                          
 *                                                                                                                                                  *
   dnx_basic_bridge_main_run(unit, iPort, oPort, c_vid, vsi, d_mac_0, d_mac_1, d_mac_2, d_mac_3, d_mac_4, d_mac_5);                                    
 * Test Scenario - end
 *                                                                                                                                                  *
 *                                                                                                                                                  *
*************************************************************************************************************************************************** */
 
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
/*  Main Struct  */
struct dnx_basic_bridge_s {
    bcm_gport_t lif;
    bcm_gport_t in_port;
    bcm_gport_t out_port;
    bcm_vlan_t vsi;
    bcm_mac_t d_mac;
    bcm_vlan_t c_vid;
};

/* Initialization of Main struct */
dnx_basic_bridge_s g_dnx_basic_bridge = {
   /*lif*/
   0,
   /*in_port*/
   0,
   /*out_port*/
   0,
   /*vsi*/
   0,
   /*d_mac*/
   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
   /* c_vid */
   0,
};


 
/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of main struct
 * Function allows to re-write default values 
 *
 * INPUT: 
 *   param: new values for g_dnx_basic_bridge
 */
void dnx_basic_bridge_init(dnx_basic_bridge_s *param){

    if (param != NULL) {
       sal_memcpy(&g_dnx_basic_bridge, param, sizeof(g_dnx_basic_bridge));
    }

}


/* 
 * Return l2_basic_bridge information
 */
void dnx_basic_bridge_modify_struct_get(int unit, dnx_basic_bridge_s *param){

    sal_memcpy( param, &g_dnx_basic_bridge, sizeof(g_dnx_basic_bridge));

    return;
}


/* This function runs the main test configuration function with given input parameters
 *  
 * INPUT: unit     - unit
 *        iPort - ingress port
 *        oPort - egress port
 *        c_vid - the customer VID value
 *        vsi - the VSI value
 *        d_mac0/1/2/3/4/5/ - the destination MAC address
 */
int dnx_basic_bridge_main_run(int unit,  int iPort, int oPort ,int c_vid, int vsi, uint8 d_mac_0, uint8 d_mac_1, uint8 d_mac_2, uint8 d_mac_3, uint8 d_mac_4, uint8 d_mac_5){
    
    dnx_basic_bridge_s param;

    printf("dnx_basic_bridge_main_run: START, iPort=%d, oPort=%d, c_vid=%d, vsi=%d\n",
           iPort, oPort, c_vid, vsi);

    dnx_basic_bridge_modify_struct_get(unit, &param);

    param.in_port = iPort;
    param.out_port = oPort;
    param.vsi = vsi;
    param.c_vid = c_vid;
    param.d_mac[0] = d_mac_0;
    param.d_mac[1] = d_mac_1;
    param.d_mac[2] = d_mac_2;
    param.d_mac[3] = d_mac_3;
    param.d_mac[4] = d_mac_4;
    param.d_mac[5] = d_mac_5;

    BCM_IF_ERROR_RETURN_MSG(dnx_basic_bridge_run(unit, &param), "");

    printf("basic_bridge_main_run: END, rv = 0\n");

    return BCM_E_NONE;
}


/* 
 * Internal function for perfroming the test configuration function with given input parameters
 */
int dnx_basic_bridge_run(int unit,  dnx_basic_bridge_s *param){
    char error_msg[200]={0,};
    int i;

    dnx_basic_bridge_init(param);


    /* Set vlan domain:*/
    BCM_IF_ERROR_RETURN_MSG(dnx_vlan_domain_create(unit), "");

    /* Create VSI:*/
    snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", g_dnx_basic_bridge.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, g_dnx_basic_bridge.vsi), error_msg);

    /* 
     * Set VLAN port membership
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    BCM_PBMP_PORT_ADD(untag_pbmp, g_dnx_basic_bridge.in_port);
    BCM_PBMP_PORT_ADD(pbmp, g_dnx_basic_bridge.in_port);
    BCM_PBMP_PORT_ADD(pbmp, g_dnx_basic_bridge.out_port);

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, g_dnx_basic_bridge.c_vid, pbmp, untag_pbmp), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, g_dnx_basic_bridge.in_port, g_dnx_basic_bridge.c_vid), "");
   
    /* Create Lif */
    BCM_IF_ERROR_RETURN_MSG(dnx_create_lif(unit), "");
    
    /*
     * Add l2 address - 
     * adding oPort statically to MACT
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_dnx_basic_bridge.d_mac, g_dnx_basic_bridge.vsi);
   
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_dnx_basic_bridge.out_port;
    bcm_gport_t forward_encap_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");

    return BCM_E_NONE;
}


/* Internal function for setting port's vlan domain*/

int dnx_vlan_domain_create(int unit) {
    char error_msg[200]={0,};

    /*Input Port */
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_dnx_basic_bridge.in_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_dnx_basic_bridge.in_port, bcmPortClassId, g_dnx_basic_bridge.in_port), error_msg);
    
    /*Output Port*/
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_dnx_basic_bridge.out_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_dnx_basic_bridge.out_port, bcmPortClassId, g_dnx_basic_bridge.out_port), error_msg);

    return BCM_E_NONE;
}


/* 
 * Internal function for creating the In-Lif.
 */
int dnx_create_lif(int unit) {
    char error_msg[200]={0,};

    /*
     * Creates In-Lif
     */

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.flags = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.match_vlan = g_dnx_basic_bridge.c_vid;
    vlan_port.port = g_dnx_basic_bridge.in_port;

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

    g_dnx_basic_bridge.lif = vlan_port.vlan_port_id;
    printf("Lif created : port=%d, vlan_port_id=0x%08x\n", vlan_port.port, vlan_port.vlan_port_id);
 
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_dnx_basic_bridge.lif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_dnx_basic_bridge.vsi, g_dnx_basic_bridge.lif), error_msg);

    return BCM_E_NONE;
}


int dnx_vlan_port_vsi_create(int unit, int vsi, bcm_port_t port, int vlan_domain, int match_criteria, bcm_vlan_t outer_vid, bcm_gport_t* vlan_port_id)
{
    bcm_vlan_port_t vlan_port;
    char error_msg[200]={0,};

    /* First, set the port's vlan domain */
    snprintf(error_msg, sizeof(error_msg), "port %d, vlan_domain %d", port, vlan_domain);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, port, bcmPortClassId, vlan_domain), error_msg);
    printf("VLAN DOMAIN: port %d, vlan_domain %d\n", port, vlan_domain);

    /* Now, set the port. */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = match_criteria;
    vlan_port.port = port;
    if (outer_vid != BCM_VLAN_INVALID)
        vlan_port.match_vlan = outer_vid;
    vlan_port.vsi = vsi;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    printf("Created vlan port for vsi mapping: 0x%x\n", vlan_port.vlan_port_id);

    *vlan_port_id = vlan_port.vlan_port_id;

    return BCM_E_NONE;
}
