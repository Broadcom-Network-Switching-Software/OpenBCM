/* $Id: cint_l2_bridge_trunk.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * set following soc properties                                                                                                                     *
 * # Specify the "bcm886xx_vlan_translate_mode" mode. 0: disabled, 1: enabled.                                                                      *
 * bcm886xx_vlan_translate_mode=1                                                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * This CINT demonstrates basic l2 bridging over LAG.                                                                                               *
 *                                                                                                                                                  *
 * The cint creates a basic L2VPN scheme with two different configurations:                                                                         *
 *                                                                                                                                                  *
 * is_lag_ingress (TRUE):                                                                                                                           *
 *  iPort1 - ingress port, member in a LAG                                                                                                          *
 *  iPort2 - ingress port, member in a LAG                                                                                                          *
 *  iPort3 - ingress port, member in a LAG                                                                                                          *
 *  oPort - egress port                                                                                                                             *
 * is_lag_ingress (FALSE):                                                                                                                          *
 *  iPort1 - ingress port                                                                                                                           *
 *  oPort1 - egress port, member in a LAG                                                                                                           *
 *  oPort2 - egress port, member in a LAG                                                                                                           *
 *  oPort3 - egress port, member in a LAG                                                                                                           *
 *                                                                                                                                                  *
 * FWD  lookup :                                                                                                                                    *
 * The packets are forwarded to the egress using MACT (vsi and MacDa) with forwarding information.                                                  *
 *                                                                                                                                                  *
 * Incoming tag format (post IVE) : Untag                                                                                                           *
 * Outgoing tag format            : STAG                                                                                                            *
 * EVE command (default)          : {bcmVlanActionMappedAdd,bcmVlanActionNone}                                                                      *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                            None,                                                                                                                 *
 *             +-------+      None                                                                                                                  *
 *    iPort    |       |      +-----+      +------+                                                                                                 *
 *    +------> | InLIF +----> | IVE +----> |      |                                                                                                 *
 *             |       |      +-----+      |      |     +-------+                                                                                   *
 *             +-------+                   |      |     |       |  Trunk_id                                                                         *
 *                                         | VSI  +---> | MACT  +-------------+                                                                     *
 *                                         |      |     |       |             |                                                                     *
 *                                         |      |     +-------+             |                                                                     *
 *                                         |      |                      +------------+                                                             *
 *                                         +------+                      |  Trunk_ID  |                                                             *
 *                                                                       | Resolution |                                                             *
 *                                                                       +------------+                                                             *
 *                                                                            | oPort                                                               *
 *                                  +---------+                               |                                                                     *
 *     oPort            +------+    |         |                               |                                                                     *
 *    <-----------------+ EVE  | <--+ Out LIF +<------------------------------+                                                                     *
 *                      +------+    |         |                                                                                                     *
 *               {MappedAdd ,None}  +---------+                                                                                                     *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * TX Pkt:                                                                                                                                          *
 * Send IPv4oETH0 packet from iPort :                                                                                                               *
 *      ethernet header with DA 00:00:00:00:0x12:0x34                                                                                               *
 *                                                                                                                                                  *
 * OUTPUT:                                                                                                                                          *
 * Get packet on oPort:                                                                                                                             *
 *      ethernet header with DA 00:00:00:00:0x12:0x34                                                                                               *
 *      Vlan tag:                                                                                                                                   *
 *          outer:                                                                                                                                  *
 *              vlan-id: 111 (From VSI)                                                                                                             *
 *              vlan tag type: 0x8100                                                                                                               *
 *                                                                                                                                                  *
*************************************************************************************************************************************************** */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

int NUM_OF_IN_PORTS = 3;
int IN_PORT_INDEX[NUM_OF_IN_PORTS] = { 0, 1, 2 };
int NUM_OF_OUT_PORTS = 3;
int OUT_PORT_INDEX[NUM_OF_OUT_PORTS] = { 1, 2, 3 };
int NUM_OF_PORTS = 4;


/*  Main Struct  */
struct l2_basic_bridge_trunk_s
{
    int is_lag_ingress;
    bcm_gport_t ports[NUM_OF_PORTS];
    bcm_vlan_t vsi;
    bcm_mac_t d_mac;
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    bcm_trunk_t trunk_id;
    bcm_gport_t trunk_gport;
    int trunk_vlan_domain;
    int entry_group;
};

/* Initialization of Main struct */
l2_basic_bridge_trunk_s g_l2_basic_bridge_trunk = {
    /*
     * is_lag_ingress
     */
    0,
    /*
     * ports:
     */
    {0, 0, 0, 0},
    /*
     * vsi
     */
    111,
    /*
     * d_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0x12, 0x34},
    /*
     * outer_vid
     */
    100,
    /*
     * inner_vid
     */
    333,
    /*
     * trunk id
     */
    0xf,
    /*
     * trunk_gport
     */
    0,
    /*
     * Trunk_vlan_domain
     */
    250,
    /*
     * entry group
     */
    0,
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_l2_basic_bridge_trunk
 */
int
l2_basic_bridge_trunk_modify_init(
    int unit,
    l2_basic_bridge_trunk_s * param)
{
    int rv;

    if (param != NULL)
    {
        sal_memcpy(&g_l2_basic_bridge_trunk, param, sizeof(g_l2_basic_bridge_trunk));
    }

    int is_jericho2_or_above = 0;
    is_jericho2_or_above = is_device_or_above(unit, JERICHO2);


    if (!is_jericho2_or_above)
    {
        /*
         * JER1
         */
        advanced_vlan_translation_mode = soc_property_get(unit, "bcm886xx_vlan_translate_mode", 0);

        if (!advanced_vlan_translation_mode)
        {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Return l2_basic_bridge_trunk information
 */
void
l2_basic_bridge_trunk_modify_struct_get(
    int unit,
    l2_basic_bridge_trunk_s * param)
{

    sal_memcpy(param, &g_l2_basic_bridge_trunk, sizeof(g_l2_basic_bridge_trunk));

    return;
}

/*
 * set pp lag property
 * When creating trunk port, the new pp port is allocated, so it's treated as a new port.
 * In default trunk init, the vlan domain and pvid is 0,
 * enable per port filtering, enable learning ARL and FWD, enable bcmVlanTranslateIngressMissDrop, disable stp
 * So usually we need to do the following configuration and also need to force enabling trunk visibility
 */
int l2_pp_lag_property_set (int unit, int trunk_gport) {
    int rv;
    int trunk_id;
    int trunk_group_id;
    int i;
    int is_jericho2_or_above = 0;
    is_jericho2_or_above = is_device_or_above(unit, JERICHO2);

    trunk_id = BCM_GPORT_TRUNK_GET(trunk_gport);
    BCM_TRUNK_ID_GROUP_GET(trunk_group_id, trunk_id);
    rv = bcm_port_class_set(unit, trunk_gport, bcmPortClassId, 256 + trunk_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set\n");
        return rv;
    }

    rv = bcm_port_control_set(unit, trunk_gport, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set\n");
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, trunk_gport, g_l2_basic_bridge_trunk.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    rv = bcm_port_stp_set(unit, trunk_gport, BCM_STG_STP_FORWARD);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_stp_set\n");
        return rv;
    }

    rv = bcm_port_learn_set(unit, trunk_gport, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_learn_set\n");
        return rv;
    }

    rv = bcm_vlan_control_port_set(unit, trunk_gport, bcmVlanTranslateIngressMissDrop, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_control_port_set\n");
        return rv;
    }
    if (is_jericho2_or_above) {

        rv = bcm_instru_gport_control_set(unit, trunk_gport, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS, bcmInstruGportControlVisEnable, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_instru_gport_control_set\n");
            return rv;
        }

        rv = bcm_instru_gport_control_set(unit, trunk_gport, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS, bcmInstruGportControlVisForce, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_instru_gport_control_set\n");
            return rv;
        }

        if (g_l2_basic_bridge_trunk.is_lag_ingress == 0) {
           for (i = 0; i < NUM_OF_OUT_PORTS; i++)
           {
               rv = bcm_instru_gport_control_set(unit, g_l2_basic_bridge_trunk.ports[OUT_PORT_INDEX[i]], BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, bcmInstruGportControlVisEnable, 1);
               if (rv != BCM_E_NONE) {
                   printf("Error, in bcm_instru_gport_control_set\n");
                   return rv;
               }

               rv = bcm_instru_gport_control_set(unit, g_l2_basic_bridge_trunk.ports[OUT_PORT_INDEX[i]], BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, bcmInstruGportControlVisForce, 1);
               if (rv != BCM_E_NONE) {
                   printf("Error, in bcm_instru_gport_control_set\n");
                   return rv;
               }
           }
        }
    }
    return rv;
}

/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit     - unit
 *       is_lag_ingress (TRUE):
 *        iPort1 - ingress port
 *        iPort2 - ingress port
 *        iPort3 - ingress port
 *        oPort - egress port
 *       is_lag_ingress (FALSE):
 *        iPort1 - ingress port
 *        oPort1 - egress port
 *        oPort2 - egress port
 *        oPort3 - egress port 
 */
int
l2_basic_bridge_trunk_main_run(
    int unit,
    int Port_1,
    int Port_2,
    int Port_3,
    int Port_4,
    int is_lag_ingress)
{

    l2_basic_bridge_trunk_s param;

    l2_basic_bridge_trunk_modify_struct_get(unit, &param);

    param.is_lag_ingress = is_lag_ingress;
    param.ports[0] = Port_1;
    param.ports[1] = Port_2;
    param.ports[2] = Port_3;
    param.ports[3] = Port_4;

    return l2_basic_bridge_trunk_run(unit, &param);
}

int
l2_basic_bridge_trunk_run(
    int unit,
    l2_basic_bridge_trunk_s * param)
{

    int rv;
    int i;
    uint32 flags;
    bcm_switch_control_info_t value;
    bcm_switch_control_key_t key;
    int is_jericho2_or_above = 0;

    /*
     * vid for tag actions
     */
    int new_outer_vid = 1024;
    int egr_new_outer_vid = 100;
    int new_inner_vid = 1500;
    int egr_new_inner_vid = 1500;

    bcm_trunk_info_t trunk_info;
    bcm_gport_t gport;
    bcm_gport_t trunk_gport;
    bcm_trunk_member_t member;
    bcm_trunk_t trunk_id;
    bcm_pbmp_t pbmp, untag_pbmp;

    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    rv = l2_basic_bridge_trunk_modify_init(unit, param);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in l2_basic_bridge_trunk_modify_init\n");
        return rv;
    }

    /*
     * Create VSI:
     */
    printf("bcm_vlan_create :");
    rv = bcm_vlan_create(unit, g_l2_basic_bridge_trunk.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_create(vis=%d)\n", g_l2_basic_bridge_trunk.vsi);
        return rv;
    }

    /*
     * Set VLAN port membership
     */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge_trunk.ports[i]);
    }

    if (g_l2_basic_bridge_trunk.is_lag_ingress) {
       for (i = 0; i < NUM_OF_IN_PORTS; i++)
       {
          BCM_PBMP_PORT_ADD(untag_pbmp, g_l2_basic_bridge_trunk.ports[IN_PORT_INDEX[i]]);
       }
    }
    else {
       BCM_PBMP_PORT_ADD(untag_pbmp, g_l2_basic_bridge_trunk.ports[IN_PORT_INDEX[0]]);
    }
    rv = bcm_vlan_port_add(unit, g_l2_basic_bridge_trunk.vsi, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }

    /*
     * Set initial vid for inport
     */
    if (g_l2_basic_bridge_trunk.is_lag_ingress) {
       for (i = 0; i < NUM_OF_IN_PORTS; i++)
       {
          rv = bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge_trunk.ports[IN_PORT_INDEX[i]], g_l2_basic_bridge_trunk.vsi);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in bcm_port_untagged_vlan_set\n");
              return rv;
          }
       }
    }
    else {
       rv = bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge_trunk.ports[IN_PORT_INDEX[0]], g_l2_basic_bridge_trunk.vsi);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_port_untagged_vlan_set\n");
           return rv;
       }
    }

    if (g_l2_basic_bridge_trunk.is_lag_ingress == 0) {
       /*
        * Creates default In-Lif, Default Port based InLif creation on Port_1
        */
       bcm_vlan_port_t vlan_port;
       bcm_vlan_port_t_init(&vlan_port);
       vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
       vlan_port.port = g_l2_basic_bridge_trunk.ports[IN_PORT_INDEX[0]];
       vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;
       rv = bcm_vlan_port_create(unit, &vlan_port);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
           return rv;
       }
       printf("vlan_port_id = 0x%08X \n", vlan_port.vlan_port_id);
    }

    /*
     * Configure Trunk
     */
    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    BCM_TRUNK_ID_SET(trunk_id, 0, g_l2_basic_bridge_trunk.trunk_id);
    flags = BCM_TRUNK_FLAG_WITH_ID;
    rv = bcm_trunk_create(unit, flags, &trunk_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_trunk_create\n");
        return rv;
    }
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
    printf("trunk_id = 0x%08X, trunk_gport = 0x%08X \n", trunk_id, trunk_gport);
    g_l2_basic_bridge_trunk.trunk_gport = trunk_gport;

    is_jericho2_or_above = is_device_or_above(unit, JERICHO2);

    /*
     * Get the port interface type
     */
    rv = bcm_port_get(unit, g_l2_basic_bridge_trunk.ports[0], &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_get \n");
        return rv;
    }

    if (is_jericho2_or_above)
    {
        value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        key.type = bcmSwitchPortHeaderType;
        key.index = 2;
        /** Set trunk header type to match members header types */
        rv = bcm_switch_control_indexed_port_set(unit, trunk_gport, key, value);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
            return rv;
        }

        if (interface_info.interface == BCM_PORT_IF_NIF_ETH) {
            value.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        } else {
            value.value = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
        }

        key.type = bcmSwitchPortHeaderType;
        key.index = 1;
        /** Set trunk header type to match members header types */
        rv = bcm_switch_control_indexed_port_set(unit, trunk_gport, key, value);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_dnx_switch_control_indexed_port_set (LAG gport)\n");
            return rv;
        }
    }

    /*
     * Add trunk member ports
     */
    int nof_ports;
    int * port_index;

    if (g_l2_basic_bridge_trunk.is_lag_ingress) {
       nof_ports = NUM_OF_IN_PORTS;
       port_index = IN_PORT_INDEX;
    }
    else {
       nof_ports = NUM_OF_OUT_PORTS;
       port_index = OUT_PORT_INDEX;
    }

    for (i = 0; i < nof_ports; i++)
    {
        bcm_trunk_member_t_init(&member);
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, g_l2_basic_bridge_trunk.ports[port_index[i]]);
        member.gport = gport;

        if (i != 0)
        {
            rv = bcm_trunk_member_add(unit, trunk_id, &member);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_trunk_member_add( %d)\n", g_l2_basic_bridge_trunk.ports[port_index[i]]);
                return rv;
            }
        }
        else
        {
            /*
             * Add First member
             */
            rv = bcm_trunk_set(unit, trunk_id, &trunk_info, 1 /* member count */ , &member);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_trunk_set( %d)\n", trunk_id);
                return rv;
            }
        }
    }

    /*
     * Set the trunk pp property
     */
    rv = l2_pp_lag_property_set(unit, trunk_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in l2_pp_lag_property_set\n");
        return rv;
    }

    if (g_l2_basic_bridge_trunk.is_lag_ingress) {
        /*
     * Creates default In-Lif, Default Port based InLif creation on trunk
     */
       bcm_vlan_port_t vlan_port;
       bcm_vlan_port_t_init(&vlan_port);
       vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
       vlan_port.port = trunk_gport;
       vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
       vlan_port.vsi = g_l2_basic_bridge_trunk.vsi;
       vlan_port.group = g_l2_basic_bridge_trunk.entry_group;
       rv = bcm_vlan_port_create(unit, &vlan_port);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
           return rv;
       }

       printf("vlan_port_id = 0x%08X \n", vlan_port.vlan_port_id);

       rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge_trunk.vsi,  vlan_port.vlan_port_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vswitch_port_add\n");
           return rv;
       }
    }

    rv = bcm_vlan_gport_add(unit, g_l2_basic_bridge_trunk.vsi, g_l2_basic_bridge_trunk.trunk_gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_gport_add\n");
        return rv;
    }


    /*
     * Configure MACT lookup
     */
    if (g_l2_basic_bridge_trunk.is_lag_ingress) {

       bcm_l2_addr_t l2_addr;
       bcm_l2_addr_t_init(&l2_addr, g_l2_basic_bridge_trunk.d_mac, g_l2_basic_bridge_trunk.vsi);
       l2_addr.flags = BCM_L2_STATIC;
       l2_addr.port = g_l2_basic_bridge_trunk.ports[OUT_PORT_INDEX[NUM_OF_OUT_PORTS-1]];
       rv = bcm_l2_addr_add(unit, &l2_addr);
       if (rv != BCM_E_NONE)
       {
           printf("Error, bcm_l2_addr_add\n");
           return rv;
       }

    }
    else {
       bcm_l2_addr_t l2_addr;
       bcm_l2_addr_t_init(&l2_addr, g_l2_basic_bridge_trunk.d_mac, g_l2_basic_bridge_trunk.vsi);
       l2_addr.flags = BCM_L2_STATIC | BCM_L2_TRUNK_MEMBER;
       l2_addr.tgid = trunk_id;
       rv = bcm_l2_addr_add(unit, &l2_addr);
       if (rv != BCM_E_NONE)
       {
           printf("Error, bcm_l2_addr_add\n");
           return rv;
       }
    }

    return rv;
}
