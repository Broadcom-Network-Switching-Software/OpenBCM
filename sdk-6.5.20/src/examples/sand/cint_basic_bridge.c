/* $Id: cint_basic_bridge.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
 *  cint ../../../../src/examples/sand/cint_basic_bridge.c                                                                                          *
 *                                                                                                                                                  *
 *  basic_bridge_main_run(unit, iPort, oPort, c_vid, vsi, d_mac_0, d_mac_1, d_mac_2, d_mac_3, d_mac_4, d_mac_5);                                    *
 */
 
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
/*  Main Struct  */
struct basic_bridge_s {
    bcm_gport_t lif;
    bcm_gport_t in_port;
    bcm_gport_t out_port;
    bcm_vlan_t vsi;
    bcm_mac_t d_mac;
    bcm_vlan_t c_vid;
};

/* Initialization of Main struct */
basic_bridge_s g_basic_bridge = {
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
 */

/* Initialization of main struct
 * Function allows to re-write default values 
 *
 * INPUT: 
 *   param: new values for g_basic_bridge
 */
int basic_bridge_init(int unit, basic_bridge_s *param){
     int rv;

    if (param != NULL) {
       sal_memcpy(&g_basic_bridge, param, sizeof(g_basic_bridge));
    }

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);

    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (!is_jericho2)
    {
        /* JER1*/
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

        if (!advanced_vlan_translation_mode ) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


/* 
 * Return l2_basic_bridge information
 */
void basic_bridge_modify_struct_get(int unit, basic_bridge_s *param){

    sal_memcpy( param, &g_basic_bridge, sizeof(g_basic_bridge));

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
int basic_bridge_main_run(int unit,  int iPort, int oPort ,int c_vid, int vsi, uint8 d_mac_0, uint8 d_mac_1, uint8 d_mac_2, uint8 d_mac_3, uint8 d_mac_4, uint8 d_mac_5){
    
    basic_bridge_s param;
    int rv;

    printf("basic_bridge_main_run: START, iPort=%d, oPort=%d, c_vid=%d, vsi=%d\n",
           iPort, oPort, c_vid, vsi);

    basic_bridge_modify_struct_get(unit, &param);

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

    rv = basic_bridge_run(unit, &param);

    printf("basic_bridge_main_run: END, rv = %d\n", rv);

    return rv;
}


/* 
 * Internal function for perfroming the test configuration function with given input parameters
 */
int basic_bridge_run(int unit,  basic_bridge_s *param){
    int rv;
    int i;

    rv = basic_bridge_init(unit, param);

    if (rv != BCM_E_NONE) {
        printf("Error, in l2_basic_bridge_init\n");
        return rv;
    }

    /* Set vlan domain:*/
    rv = vlan_domain_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_domain_create\n");
        return rv;
    }

    /* Create VSI:*/
    rv = bcm_vlan_create(unit, g_basic_bridge.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vsi=%d)\n", g_basic_bridge.vsi);
        return rv;
    }

    /* 
     * Set VLAN port membership
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    BCM_PBMP_PORT_ADD(untag_pbmp, g_basic_bridge.in_port);
    BCM_PBMP_PORT_ADD(pbmp, g_basic_bridge.in_port);
    BCM_PBMP_PORT_ADD(pbmp, g_basic_bridge.out_port);

    rv = bcm_vlan_port_add(unit, g_basic_bridge.c_vid, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, g_basic_bridge.in_port, g_basic_bridge.c_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }
   
    /* Create Lif */
    rv = create_lif(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_lif\n");
        return rv;
    }
    
    /*
     * Add l2 address - 
     * adding oPort statically to MACT
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_basic_bridge.d_mac, g_basic_bridge.vsi);
   
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_basic_bridge.out_port;
    bcm_gport_t forward_encap_id;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}


/* Internal function for setting port's vlan domain*/

int vlan_domain_create(int unit) {
    int rv;

    /*Input Port */
    rv = bcm_port_class_set(unit, g_basic_bridge.in_port, bcmPortClassId, g_basic_bridge.in_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_basic_bridge.in_port);
        return rv;
    }
    
    /*Output Port*/
    rv = bcm_port_class_set(unit, g_basic_bridge.out_port, bcmPortClassId, g_basic_bridge.out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_basic_bridge.out_port);
        return rv;
    }

    return rv;
}


/* 
 * Internal function for creating the In-Lif.
 */
int create_lif(int unit) {
    int rv;

    /*
     * Creates In-Lif
     */

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.flags = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.match_vlan = g_basic_bridge.c_vid;
    vlan_port.port = g_basic_bridge.in_port;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_basic_bridge.lif = vlan_port.vlan_port_id;
    printf("Lif created : port=%d, vlan_port_id=0x%08x\n", vlan_port.port, vlan_port.vlan_port_id);
 
    rv = bcm_vswitch_port_add(unit, g_basic_bridge.vsi, g_basic_bridge.lif);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_basic_bridge.inlif);
       return rv;
    }

    return rv;
}
