/* $Id: cint_dnx_l2_vlan_match_sa.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The cint creates a basic L2VPN scheme for Source MAC based VLAN Resolution.                                                                      *
 *                                                                                                                                                  *
 * Workflow(four classificaiton as below):                                                                                                          *
 * 1. PP_Port (Default): VTT1 stage                                                                                                                 * 
 *    A packet on a port configured to "Source MAC based VLAN Resolution" might not succeed in "Source MAC" classification.                         *
 *    In this case the packet still needs an "AC InLIF", and the LIF will be taken from the PP_Port default.                                        *
 * 2. MAC(SA),PP_Port untagged packets: VTT3 stage                                                                                                  *
 * 3. MAC(SA),PP_Port tagged packets: VTT4 stage                                                                                                    *
 * 4. VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packets: VTT5 stage                                                                      *
 *                                                                                                                                                  *
 * The test uses four input ports, iPort and three output ports oPort1, oPort2, oPort3.                                                             *
 * First step:                                                                                                                                      *
 * 1. Initialize test parameters.                                                                                                                   *
 * 2. Setting VLAN domain for ingress/egress ports.                                                                                                 *
 * 3. Enable the source MAC lookup.                                                                                                                 *
 * 4. Create the default/untagged/tagged/vlan_tagged VSI.                                                                                           *
 * 5. Create In-LIF associated with PP_Port Default.                                                                                                *
 * 6. Setting MACT DA with egress port.                                                                                                             *
 * 7. Ingress untagged-packet:                                                                                                                      *
 *    Expected result: Drop and VSI is default_vsi.                                                                                                 *
 * 8. Ingress tagged-packet(c-tag: 0x22):                                                                                                           *
 *    Expected result: Drop and VSI is default_vsi.                                                                                                 *
 * 9. Ingress tagged-packet(c-tag: 0x33):                                                                                                           *
 *    Expected result: Drop and VSI is default_vsi.                                                                                                 *
 *                                                                                                                                                  *
 * Second step:                                                                                                                                     *
 * 1. Create In-LIF associated with MAC(SA),PP_Port untagged packet.                                                                                *
 * 2. Create In-LIF associated with MAC(SA),PP_Port tagged packet.                                                                                  *
 * 3. Create In-LIF associated with VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packets.                                                   *
 * 4. Create IVE (replace, add) for vlan specific tagged packets.                                                                                   *
 * 5. Ingress untagged-packet:                                                                                                                      *
 *    Expected result: c-tag and VSI is untagged_vsi.                                                                                               *
 * 6. Ingress tagged-packet(c-tag: 0x22):                                                                                                           *
 *    Expected result: c-tag(same packet) and VSI is tagged_vsi.                                                                                    *
 * 7. Ingress tagged-packet(c-tag: 0x33):                                                                                                           *
 *    Expected result: d-tag and VSI is vlan_tagged_vsi.                                                                                            *
 *                                                                                                                                                  *
 * Third step:                                                                                                                                      *
 * 1. Add ISEM lookup for MAC(SA),PP_Port untagged packet to the LIF created in step 2.                                                             *
 * 2. Add ISEM lookup for MAC(SA),PP_Port tagged packet to the LIF created in step 2.                                                               *
 * 3. Add ISEM lookup for VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet to the LIF created in step 2.                                 *
 * 4. Ingress untagged-packet:                                                                                                                      *
 *    Expected result: c-tag and VSI is untagged_vsi.                                                                                               *
 * 5. Ingress tagged-packet(c-tag: 0x22):                                                                                                           *
 *    Expected result: c-tag(same packet) and VSI is tagged_vsi.                                                                                    *
 * 6. Ingress tagged-packet(c-tag: 0x33):                                                                                                           *
 *    Expected result: d-tag and VSI is vlan_tagged_vsi.                                                                                            *
 *                                                                                                                                                  *
 * Fourth step:                                                                                                                                     *
 * 1. Remove the ISEM lookup for VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet to the LIF created in step 2.                          *
 * 2. Destroy the In-LIF associated with VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet  (created in step 2).                          *
 * 3. Ingress tagged-packet(c-tag: 0x33):                                                                                                           *
 *    Expected result: c-tag(same packet) and VSI is tagged_vsi.                                                                                    *
 *                                                                                                                                                  *
 * Fifth step:                                                                                                                                      *
 * 1. Remove the ISEM lookup for MAC(SA),PP_Port tagged packet to the LIF created in step 2.                                                        *
 * 2. Destroy the In-LIF associated with MAC(SA),PP_Port tagged packet  (created in step 2).                                                        *
 * 3. Ingress tagged-packet(c-tag: 0x22):                                                                                                           *
 *    Expected result: c-tag(same packet) and VSI is default_vsi.                                                                                   *
 *                                                                                                                                                  *
 * INPUT packets:                                                                                                                                   *
 * ETH0:                                                                                                                                            *
 *      ethernet header with DA 00:00:00:00:0xAA:0xBB, SA 00:00:00:00:0x11:0x22                                                                     *
 * ETH1:                                                                                                                                            *
 *      ethernet header with DA 00:00:00:00:0xAA:0xBB, SA 00:00:00:00:0x11:0x22                                                                     *
 *      Vlan tag:                                                                                                                                   *
 *         outer:                                                                                                                                   *
 *             vlan-id: 0x22                                                                                                                        *
 *             TPID: 0x8100                                                                                                                         *
 * ETH1:                                                                                                                                            *
 *      ethernet header with DA 00:00:00:00:0xAA:0xBB, SA 00:00:00:00:0x11:0x22                                                                     *
 *      Vlan tag:                                                                                                                                   *
 *         outer:                                                                                                                                   *
 *             vlan-id: 0x33                                                                                                                        *
 *             TPID: 0x8100                                                                                                                         *
 *                                                                                                                                                  *
 * OUTPUT packets:                                                                                                                                  *
 * ETH1 (EVE: VSI is added):                                                                                                                        *
 *       ethernet header with DA 00:00:00:00:0xAA:0xBB, SA 00:00:00:00:0x11:0x22                                                                    *
 *       Vlan tag:                                                                                                                                  *
 *          outer:                                                                                                                                  *
 *             vlan-id: 100 (VSI = 100)                                                                                                             *
 *             TPID: 0x8100                                                                                                                         *
 * ETH1:                                                                                                                                            *
 *       ethernet header with DA 00:00:00:00:0xAA:0xBB, SA 00:00:00:00:0x11:0x22                                                                    *
 *       Vlan tag:                                                                                                                                  *
 *          outer:                                                                                                                                  *
 *             vlan-id: 0x22                                                                                                                        *
 *             TPID: 0x8100                                                                                                                         *
 * ETH2 (IVE: replace+add):                                                                                                                         *
 *       ethernet header with DA 00:00:00:00:0xAA:0xBB, SA 00:00:00:00:0x11:0x22                                                                    *
 *       Vlan tag:                                                                                                                                  *
 *          outer:                                                                                                                                  *
 *             vlan-id: 0x55                                                                                                                        *
 *             TPID: 0x9100                                                                                                                         *
 *          inner:                                                                                                                                  *
 *             vlan-id: 0x66                                                                                                                        *
 *             TPID: 0x8100                                                                                                                         *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *             +-----------+   Action:{Replace,Add}                                                                                                 *
 * iPort1/2    |           |      +-----+      +-------+                                                                                            *
 *    +------> | LIF       +----> |IVE  +----> |       |                                                                                            *
 *             |           |      +-----+      |       |     +-------+                                                                              *
 *             +-----------+                   |       |     |       |  oPort                                                                       *
 *                                             | VSI   +---> | MACT  +-------------+                                                                *
 *                                             |       |             |             |                                                                *
 *                                             |       |     +-------+             |                                                                *
 *                                             |       |                           |                                                                *
 *                                             +-------+                           |                                                                *
 *                                                                                 |                                                                *
 *                                                                                 |                                                                *
 *                                                                                 |                                                                *
 *     oPort                                                                       |                                                                *
 *    <----------------------------------------------------------------------------+                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/dnx                                                                                                                  *
 * cint utility/cint_dnx_utils_global.c                                                                                                             * 
 * cint utility/cint_dnx_utils_vlan_translate.c                                                                                                     *
 * cint vlan/cint_dnx_l2_auth.c                                                                                                                     *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * l2_vlan_match_sa_main_run (int unit, int iPort, int oPort1, int oPort2, int oPort3)                                                              *
 * l2_vlan_match_sa_step_run (int unit,  int step = 2)                                                                                              *
 * l2_vlan_match_sa_step_run (int unit,  int step = 3)                                                                                              *
 * l2_vlan_match_sa_step_run (int unit,  int step = 4)                                                                                              *
 * l2_vlan_match_sa_step_run (int unit,  int step = 5)                                                                                              *
 * l2_vlan_match_sa_step_run_cleanup_run(int unit)                                                                                                  *
 *                                                                                                                                                  * 
*************************************************************************************************************************************************** */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

/*  Main Struct  */
struct l2_vlan_match_sa_info_s
{
    bcm_gport_t in_port;
    bcm_gport_t out_port[3];
    bcm_mac_t s_mac;        /* Used by In-LIF creation */            
    bcm_mac_t s_mac_2;      /* Used by ISEM lookup */  
    bcm_mac_t d_mac;

    /* PP_Port (Default) */
    bcm_gport_t default_inlif;
    bcm_vlan_t default_vsi;
    
    /* MAC(SA),PP_Port untagged packets */
    bcm_gport_t untagged_inlif;
    bcm_vlan_t untagged_vsi;
    
    /* MAC(SA),PP_Port tagged packets */
    bcm_gport_t tagged_inlif;
    bcm_vlan_t tagged_vsi;
    bcm_vlan_t tagged_outer_vid;
    
    /* VID(Outer_VLAN),MAC(SA),PP_Port */
    bcm_gport_t vlan_tagged_inlif;
    bcm_vlan_t vlan_tagged_vsi;
    bcm_vlan_t vlan_tagged_outer_vid;
    uint16 vlan_tagged_s_tag; 
    uint16 vlan_tagged_c_tag;
    bcm_vlan_t vlan_tagged_new_outer_vid;
    bcm_vlan_t vlan_tagged_new_inner_vid;    
    uint32 vlan_tagged_vlan_edit_profile;
    uint16 vlan_tagged_tag_format;          
};

/* Initialization of global struct*/
l2_vlan_match_sa_info_s l2_vlan_match_sa_info = {
   /* in_port: */
   0,
   /* out_port: */
   {0, 0, 0},
   /* src_mac */
   {0x00, 0x00, 0x00, 0x00, 0x11, 0x22},
   /* src_mac_2 */
   {0x00, 0x00, 0x00, 0x00, 0x77, 0x88},
   /* d_mac */
   {0x00, 0x00, 0x00, 0x00, 0xAA, 0xBB},
   
   /* default_inlif */
   0,
   /* default_vsi */
   10,
   
   /* untagged_inlif */
   0,
   /* untagged_vsi */
   100,
   
   /* tagged_inlif */
   0,
   /* tagged_vsi */
   200,
   /* tagged_outer_vis */
   0x22,
   
   /* vlan_tagged_inlif */
   0,
   /* vlan_tagged_vsi */
   300,
   /* vlan_tagged_outer_vid */
   0x33,   
   /* vlan_tagged_packet_s_tag */ 
   0x9100, 
   /* vlan_tagged_packet_c_tag */ 
   0x8100, 
   /* vlan_tagged_new_outer_vid */
   0x55,
   /* vlan_tagged_new_inner_vid */
   0x66,     
   /* vlan_tagged_vlan_edit_profile */
   5,
   /* vlan_tagged_tag_format */
   8
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/*
 * Create In-LIF for Source MAC based VLAN Resolution.
 */
int l2_vlan_match_sa_inlif_add(int unit, int idx)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_match_t criteria[3] = {BCM_VLAN_PORT_MATCH_MAC_PORT, BCM_VLAN_PORT_MATCH_MAC_PORT_TAGGED, BCM_VLAN_PORT_MATCH_MAC_PORT_VLAN};
    bcm_vlan_t vsi[3] = {l2_vlan_match_sa_info.untagged_vsi, l2_vlan_match_sa_info.tagged_vsi, l2_vlan_match_sa_info.vlan_tagged_vsi};
    bcm_vlan_port_t vlan_port;
    
    bcm_vlan_port_t_init(&vlan_port);
    
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;  
    vlan_port.criteria = criteria[idx];
    vlan_port.port = l2_vlan_match_sa_info.in_port;
    sal_memcpy(vlan_port.match_srcmac, l2_vlan_match_sa_info.s_mac, 6);
    if (vlan_port.criteria == BCM_VLAN_PORT_MATCH_MAC_PORT_VLAN) {
        vlan_port.match_vlan = l2_vlan_match_sa_info.vlan_tagged_outer_vid; 
    }
    vlan_port.vsi = vsi[idx];
    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_create(IDX = %d)\n", idx);
        return rv;
    }

    if (idx == 0) {
        l2_vlan_match_sa_info.untagged_inlif = vlan_port.vlan_port_id;
    } else if (idx == 1) {
        l2_vlan_match_sa_info.tagged_inlif = vlan_port.vlan_port_id;
    } else if (idx == 2) {
        l2_vlan_match_sa_info.vlan_tagged_inlif = vlan_port.vlan_port_id;
    }
    
    printf("index=%d, vlan_port_id=0x%08x\n", idx, vlan_port.vlan_port_id);

    return rv;
}


/*
 * Destroy In-LIF for Source MAC based VLAN Resolution.
 */
int l2_vlan_match_sa_inlif_del(int unit, int idx)
{
    int rv = BCM_E_NONE;
    bcm_gport_t inlif[3] = {l2_vlan_match_sa_info.untagged_inlif, l2_vlan_match_sa_info.tagged_inlif, l2_vlan_match_sa_info.vlan_tagged_inlif};
 
    rv = bcm_vlan_port_destroy(unit, inlif[idx]);
    if (rv != BCM_E_NONE)
    {
    printf("Error, in bcm_vlan_port_destroy(IDX = %d)\n", idx);
    return rv;
    }

    return rv;
}

/*
 * Add ISEM lookup for Source MAC based VLAN Resolution.
 */
int l2_vlan_match_sa_isem_add(int unit, int idx)
{
    int rv = BCM_E_NONE;
    bcm_port_match_t criteria[3] = {BCM_PORT_MATCH_MAC_PORT, BCM_PORT_MATCH_MAC_PORT_TAGGED, BCM_PORT_MATCH_MAC_PORT_VLAN};
    bcm_gport_t inlif[3] = {l2_vlan_match_sa_info.untagged_inlif, l2_vlan_match_sa_info.tagged_inlif, l2_vlan_match_sa_info.vlan_tagged_inlif};
    bcm_port_match_info_t match_info;
    
    bcm_port_match_info_t_init(&match_info);
    
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;  
    match_info.match = criteria[idx];
    match_info.port = l2_vlan_match_sa_info.in_port;
    sal_memcpy(match_info.match_tunnel_srcmac, l2_vlan_match_sa_info.s_mac_2, 6);
    if (match_info.match == BCM_PORT_MATCH_MAC_PORT_VLAN) {
        match_info.match_vlan = l2_vlan_match_sa_info.vlan_tagged_outer_vid; 
    }
    
    rv = bcm_port_match_add(unit, inlif[idx], &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_match_add (IDX = %d, rv = %d)\n", idx, rv );
        return rv;
    }

    return rv;
}

/*
 * Delete ISEM lookup for Source MAC based VLAN Resolution.
 */
int l2_vlan_match_sa_isem_del(int unit, int idx)
{
    int rv = BCM_E_NONE;
    bcm_port_match_t criteria[3] = {BCM_PORT_MATCH_MAC_PORT, BCM_PORT_MATCH_MAC_PORT_TAGGED, BCM_PORT_MATCH_MAC_PORT_VLAN};
    bcm_gport_t inlif[3] = {l2_vlan_match_sa_info.untagged_inlif, l2_vlan_match_sa_info.tagged_inlif, l2_vlan_match_sa_info.vlan_tagged_inlif};
    bcm_port_match_info_t match_info;
    
    bcm_port_match_info_t_init(&match_info);
    
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;  
    match_info.match = criteria[idx];
    match_info.port = l2_vlan_match_sa_info.in_port;
    sal_memcpy(match_info.match_tunnel_srcmac, l2_vlan_match_sa_info.s_mac_2, 6);
    if (match_info.match == BCM_PORT_MATCH_MAC_PORT_VLAN) {
        match_info.match_vlan = l2_vlan_match_sa_info.vlan_tagged_outer_vid; 
    }
    
    rv = bcm_port_match_delete(unit, inlif[idx], &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_match_delete (IDX = %d)\n", idx);
        return rv;
    }

    return rv;
}

/*
 * Main function runs the vlan "Match Source MAC based VLAN" example
 *
 * Main steps of configuration:
 *    1. Initialize test parameters.
 *    2. Setting VLAN domain for ingress/egress ports.
 *    3. Enable the source MAC lookup.
 *    4. Create the default/untagged/tagged/vlan_tagged VSI
 *    5. Create In-LIF associated with PP_Port Default
 *    6. Setting MACT DA with egress port.
 *
 * INPUT: unit  - unit
 *        iPort - ingress port
 *        oPort1 - egress port
 *        oPort2 - egress port
 *        oPort3 - egress port
 */
int
l2_vlan_match_sa_main_run(
    int unit,
    int iPort,
    int oPort1,
    int oPort2,
    int oPort3)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_pbmp_t pbmp, untag_pbmp;
    bcm_vlan_port_t vlan_port;
    bcm_l2_addr_t l2_addr;    
    bcm_vlan_t vsi[4] = {l2_vlan_match_sa_info.default_vsi, l2_vlan_match_sa_info.untagged_vsi, l2_vlan_match_sa_info.tagged_vsi, l2_vlan_match_sa_info.vlan_tagged_vsi};
    
    l2_vlan_match_sa_info.in_port = iPort;
    l2_vlan_match_sa_info.out_port[0] = oPort1;
    l2_vlan_match_sa_info.out_port[1] = oPort2;
    l2_vlan_match_sa_info.out_port[2] = oPort3;    

    /* set vlan domain */
    rv = bcm_port_class_set(unit, l2_vlan_match_sa_info.in_port, bcmPortClassId, l2_vlan_match_sa_info.in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set(port = %d)\n", l2_vlan_match_sa_info.in_port);
        return rv;
    }

    for (i = 0; i < 3; i++) 
    {
        rv = bcm_port_class_set(unit, l2_vlan_match_sa_info.out_port[i], bcmPortClassId, l2_vlan_match_sa_info.out_port[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", l2_vlan_match_sa_info.out_port[i]);
            return rv;
        }
    }

    /* Set VLAN port membership */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    BCM_PBMP_PORT_ADD(pbmp, l2_vlan_match_sa_info.in_port);
    for (i = 0; i < 3; i++) 
    {
        BCM_PBMP_PORT_ADD(pbmp, l2_vlan_match_sa_info.out_port[i]);
    }

    rv = bcm_vlan_port_add(unit, l2_vlan_match_sa_info.tagged_outer_vid, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }

    rv = bcm_vlan_port_add(unit, l2_vlan_match_sa_info.vlan_tagged_outer_vid, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    } 
    
    /* Enable the source MAC lookup */
    rv = bcm_vlan_control_port_set(unit, l2_vlan_match_sa_info.in_port, bcmVlanLookupMACEnable, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_port_set(port = %d)\n", l2_vlan_match_sa_info.in_port);
        return rv;
    }

    /* Create VSI */
    for (i = 0; i < 4; i++) 
    {        
        rv = bcm_vlan_create(unit, vsi[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_create(vsi=%d)\n", vsi[i]);
            return rv;
        }
    }      

    /* Create PP_Port (Default) AC  */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = l2_vlan_match_sa_info.in_port;
    vlan_port.vsi = l2_vlan_match_sa_info.default_vsi;    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_create\n");
        return rv;
    }
    l2_vlan_match_sa_info.default_inlif = vlan_port.vlan_port_id;
    printf("l2_vlan_match_sa_info.default_inlif=0x%08x\n", vlan_port.vlan_port_id);

    /*
     * Add l2 address -
     *   - for untagged packet: VSI is 100.
     *   - for tagged packet with unknown outer vid: VSI is 200.
     *   - for tagged packet with outer vid: VSI is 300.
     */
    for (i = 0; i < 3; i++) 
    { 
        bcm_l2_addr_t_init(&l2_addr, l2_vlan_match_sa_info.d_mac, vsi[1 + i]);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = l2_vlan_match_sa_info.out_port[i];
    
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l2_addr_add (VSI = %d\n", vsi[1 + i]);
            return rv;
        }
    }

    return rv;
}

/*
 * Step function runs the vlan "Match Source MAC based VLAN" example
 *
 * Main steps of configuration:
 * step 2:
 *    1. Create In-LIF associated with MAC(SA),PP_Port untagged packet.
 *    2. Create In-LIF associated with MAC(SA),PP_Port tagged packet.
 *    3. Create In-LIF associated with VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packets.
 *    4. Create IVE (replace, add) for vlan specific tagged packets.
 * 
 * step 3:
 *    1. Add ISEM lookup for MAC(SA),PP_Port untagged packet to the LIF created in step 2.
 *    2. Add ISEM lookup for MAC(SA),PP_Port tagged packet to the LIF created in step 2.
 *    3. Add ISEM lookup for VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet to the LIF created in step 2.
 * 
 * step 4:
 *    1. Remove the ISEM lookup for VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet to the LIF created in step 2.
 *    2. Destroy the In-LIF associated with VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet  (created in step 2).
 * 
 * step 5:
 *    1. Remove the ISEM lookup for MAC(SA),PP_Port tagged packet to the LIF created in step 2.
 *    2. Destroy the In-LIF associated with MAC(SA),PP_Port tagged packet  (created in step 2).
 *
 * INPUT: unit - unit
 *        step - which step in the configuration to set [2:5].
 */
int
l2_vlan_match_sa_step_run(
    int unit,
    int step)
{
    int idx;
    int rv = BCM_E_PARAM;

    if ( (step < 2 ) && (step > 5) )
    {
        printf("Error, step = %d!!! Should be [2:5]\n", step);
        return rv;
    }

    printf("l2_auth_step_run( step = %d )\n", step);

    switch (step)
    {
        case 2:
        {
            /* 
            * Step 2:
            * 1. Create In-LIF associated with MAC(SA),PP_Port untagged packet.
            * 2. Create In-LIF associated with MAC(SA),PP_Port tagged packet.
            * 3. Create In-LIF associated with VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packets.
            * 4. Create IVE (replace, add) for vlan specific tagged packets.
            */
            for (idx = 0; idx < 3; idx++) 
            {
                rv = l2_vlan_match_sa_inlif_add(unit, idx);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, l2_vlan_match_sa_inlif_add (IDX = %d)\n", idx);
                    return rv;
                }
            } 

            /*
            * Create IVE (replace, add) command for vlan tagged-packet:
            */ 
            rv = vlan_translate_ive_eve_translation_set(unit, l2_vlan_match_sa_info.vlan_tagged_inlif,           /* lif gport */
                                                       l2_vlan_match_sa_info.vlan_tagged_s_tag,                  /* outer_tpid */
                                                       l2_vlan_match_sa_info.vlan_tagged_c_tag,                  /* inner_tpid */
                                                       bcmVlanActionReplace,                            /* outer_action */
                                                       bcmVlanActionAdd,                                /* inner_action */
                                                       l2_vlan_match_sa_info.vlan_tagged_new_outer_vid,          /* new_outer_vid */
                                                       l2_vlan_match_sa_info.vlan_tagged_new_inner_vid,          /* new_inner_vid */
                                                       l2_vlan_match_sa_info.vlan_tagged_vlan_edit_profile,      /* vlan_edit_profile */
                                                       l2_vlan_match_sa_info.vlan_tagged_tag_format,             /* tag_format - untagged = 0 */
                                                       TRUE                                             /* is_ive */
               );
           if (rv != BCM_E_NONE)
           {
               printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {Replace,Add}\n");
               return rv;
           }
            break;
        }
       case 3:
       {
          /*
          * Step 3:
          * 1. Add ISEM lookup for MAC(SA),PP_Port untagged packet to the LIF created in step 2.
          * 2. Add ISEM lookup for MAC(SA),PP_Port tagged packet to the LIF created in step 2.
          * 3. Add ISEM lookup for VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet to the LIF created in step 2.
          */
            for (idx = 0; idx < 3; idx++) 
            {
                rv = l2_vlan_match_sa_isem_add(unit, idx);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, l2_vlan_match_sa_isem_add (IDX = %d)\n", idx);
                    return rv;
                }
            }                
            break;
       }
       case 4:
       {
          /*
          * Step 4:
          * 1. Remove the ISEM lookup for VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet to the LIF created in step 2.
          * 2. Destroy the In-LIF associated with VID(Outer_VLAN),MAC(SA),PP_Port vlan specific tagged packet  (created in step 2).
          */
          rv = l2_vlan_match_sa_isem_del(unit, 2);
          if (rv != BCM_E_NONE)
          {
              printf("Error, l2_vlan_match_sa_isem_del (IDX = %d)\n", 2);
              return rv;
          }

          rv = l2_vlan_match_sa_inlif_del(unit, 2);
          if (rv != BCM_E_NONE)
          {
              printf("Error, l2_vlan_match_sa_inlif_del (IDX = %d)\n", 2);
              return rv;
          }

          break;
       }
       case 5:
       {
          /*
          * Step 5:
          * 1. Remove the ISEM lookup for MAC(SA),PP_Port tagged packet to the LIF created in step 2.
          * 2. Destroy the In-LIF associated with MAC(SA),PP_Port tagged packet  (created in step 2).
          */
          rv = l2_vlan_match_sa_isem_del(unit, 1);
          if (rv != BCM_E_NONE)
          {
              printf("Error, l2_vlan_match_sa_isem_del (IDX = %d)\n", 1);
              return rv;
          }

          rv = l2_vlan_match_sa_inlif_del(unit, 1);
          if (rv != BCM_E_NONE)
          {
              printf("Error, l2_vlan_match_sa_inlif_del (IDX = %d)\n", 1);
              return rv;
          }

          break;
       }
    }
    return rv;
}

/*
 * Cleanup function runs the vlan "Match Source MAC based VLANt" example
 *
 * The steps of cleanup (in invers order of the configuration):
 *    1. Delete IVE actions.
 *    2. Remove ISEMlookups.
 *    3. Destroy LIF.
 *    4. Destroy VSI.
 *    5. Disable the source MAC lookup.
 *
 * INPUT: unit - unit
 */
int
l2_vlan_match_sa_cleanup_run(
    int unit)
{
    int i;
    int rv;
    bcm_vlan_t vsi[4] = {l2_vlan_match_sa_info.default_vsi, l2_vlan_match_sa_info.untagged_vsi, l2_vlan_match_sa_info.tagged_vsi, l2_vlan_match_sa_info.vlan_tagged_vsi};

    printf("l2_auth_cleanup_run\n");

    /*
     * IVE actions:
     */
    rv = bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_INGRESS);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_translate_action_id_destroy_all(BCM_VLAN_ACTION_SET_INGRESS)\n");
        return rv;
    }

    /*
     * ISEM lookup
     */
    rv = l2_vlan_match_sa_isem_del(unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l2_vlan_match_sa_isem_del (IDX = %d)\n", 0);
        return rv;
    }

    /*
     * LIF
     */
    rv = bcm_vlan_port_destroy(unit, l2_vlan_match_sa_info.untagged_inlif);
    if (rv != BCM_E_NONE)
    {
       printf("Error, in bcm_vlan_port_destroy(gport = 0x%08X)\n", l2_vlan_match_sa_info.untagged_inlif);
       return rv;
    }

    rv = bcm_vlan_port_destroy(unit, l2_vlan_match_sa_info.default_inlif);
    if (rv != BCM_E_NONE)
    {
       printf("Error, in bcm_vlan_port_destroy(gport = 0x%08X)\n", l2_vlan_match_sa_info.default_inlif);
       return rv;
    }
    
    /*
     * VSI
     */
    for (i = 0; i < 4; i++) 
    {
        rv = bcm_vlan_destroy(unit, vsi[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_destroy(vsi=%d)\n", vsi[i]);
            return rv;
        }
    }

    /*
     * Disable the source MAC lookup
     */
    rv = bcm_vlan_control_port_set(unit, l2_vlan_match_sa_info.in_port, bcmVlanLookupMACEnable, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_port_set(port = %d)\n", l2_vlan_match_sa_info.in_port);
        return rv;
    }

    return rv;
}


