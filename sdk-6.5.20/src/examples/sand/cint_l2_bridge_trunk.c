/* $Id: cint_dnx_l2_basic_bridge_trunk.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * set following soc properties                                                                                                                     *
 * # Specify the "bcm886xx_vlan_translate_mode" mode. 0: disabled, 1: enabled.                                                                      *
 * bcm886xx_vlan_translate_mode=1                                                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * This CINT demonstrates basic l2 bridging with IVE and EVE actions for canonical format untag                                                     *
 *                                                                                                                                                  *
 * The cint creates a basic L2VPN scheme with two different Logical Interfaces(LIF) as follows.                                                     *
 * InLif : Based on Default_port_lif                                                                                                                *
 * Outlif :Based on Default_ac_profile based on egress port.                                                                                                                                *
 *                                                                                                                                                  *
 * Ports :                                                                                                                                          *
 * In_port  : Port_1                                                                                                                                *
 * Out_port : Based on Lag resolution on Port_2, Port_3, Port_4                                                                                     *
 *                                                                                                                                                  *
 * Canonical format : Untag                                                                                                                         *
 * Incoming Packet : Untag                                                                                                                           *
 *                                                                                                                                                  *
 * FWD  lookup :                                                                                                                                    *
 * The packets are forwarded to the egress using MACT (vsi and MacDa) with Trunk_ID forwarding information.                                         *
 *                                                                                                                                                  *
 * EVE command :                                                                                                                                    *
 * Outlif lookup happens in EEDB                                                                                                                    *
 * Incoming tag format (post IVE) : Untag                                                                                                           *
 * Outgoing tag format            : STAG                                                                                                            *
 * EVE command                    : {bcmvlanactionadd,bcmvlanactionnone}                                                                            *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                            Delete,                                                                                                               *
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
 *                     {Add ,None}  +---------+                                                                                                     *
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
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
/* tag formats */
int tag_format_untag = 0;
int tag_format_stag = 1;
int tag_format_ctag = 2;
int tag_format_dtag = 3;

int NUM_OF_IN_PORTS = 3;
int IN_PORT_INDEX[NUM_OF_IN_PORTS] = { 0, 1, 2 };
int NUM_OF_OUT_PORTS = 3;
int OUT_PORT_INDEX[NUM_OF_OUT_PORTS] = { 1, 2, 3 };
int NUM_OF_PORTS = 4;

struct global_tpids_s
{
    uint16 tpid0;               /* tpid0 */
    uint16 tpid1;               /* tpid1 */
    uint16 tpid2;               /* tpid2 */
    uint16 tpid3;               /* tpid3 */
    uint16 tpid4;               /* tpid4 */
    uint16 tpid5;               /* tpid5 */
    uint16 tpid6;               /* tpid6 */
};

/*  Main Struct  */
struct l2_basic_bridge_trunk_s
{
    int is_lag_ingress;
    bcm_gport_t ports[NUM_OF_PORTS];
    bcm_vlan_t vsi;
    bcm_mac_t d_mac;
    global_tpids_s tpids;
    bcm_gport_t inLifList[NUM_OF_PORTS];
    bcm_gport_t outLifList[NUM_OF_PORTS];
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    bcm_trunk_t trunk_id;
    bcm_gport_t trunk_gport;
    int trunk_vlan_domain;
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
     /*TPIDS*/ {0x8100, 0x9100, 0x88a8, 0x88e7, 0xaaaa, 0xbbbb, 0x5555},
    /*
     * inLifList:
     */
    {0, 0, 0, 0},
    /*
     * outLifList:
     */
    {0, 0, 0, 0},
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
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_vlan_translate_tpid_modify
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

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (!is_jericho2)
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
    int device_is_jericho2 = 0;
    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error in is_device_jericho2\n");
        return rv;
    }

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
    if (device_is_jericho2) {

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
    int device_is_jericho2 = 0;

    /*
     * vid for tag actions
     */
    int new_outer_vid = 1024;
    int egr_new_outer_vid = 100;
    int new_inner_vid = 1500;
    int egr_new_inner_vid = 1500;

    /*
     * vlan edit profiles
     */
    int ing_vlan_edit_profile_untag = 1;
    int ing_vlan_edit_profile_stag = 2;
    int ing_vlan_edit_profile_ctag = 3;
    int ing_vlan_edit_profile_dtag = 4;
    int egr_vlan_edit_profile_untag = 5;
    int egr_vlan_edit_profile_stag = 6;
    int egr_vlan_edit_profile_ctag = 7;
    int egr_vlan_edit_profile_dtag = 8;

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
        * Creates default In-Lif, Default Port based InLif creation on Port_1, GPORT_TYPE <<26, GPORT_SUB_TYPE <<22;lif vlaue
        */
       BCM_GPORT_SUB_TYPE_LIF_SET(g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]], BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, 5000);
       BCM_GPORT_VLAN_PORT_ID_SET(g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]], g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]]);
       bcm_vlan_port_t vlan_port;
       bcm_vlan_port_t_init(&vlan_port);
       vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
       vlan_port.vlan_port_id = g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]];
       vlan_port.port = g_l2_basic_bridge_trunk.ports[IN_PORT_INDEX[0]];
       vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;
       rv = bcm_vlan_port_create(unit, &vlan_port);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
           return rv;
       }
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


    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2 \n");
        return rv;
    }

    /*
     * Get the port interface type
     */
    rv = bcm_port_get(unit, g_l2_basic_bridge_trunk.ports[0], &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_get \n");
        return rv;
    }

    if (device_is_jericho2)
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
     * Creates default In-Lif, Default Port based InLif creation on trunk, GPORT_TYPE <<26, GPORT_SUB_TYPE <<22;lif vlaue
     */
       BCM_GPORT_SUB_TYPE_LIF_SET(g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]], BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, 5000);
       BCM_GPORT_VLAN_PORT_ID_SET(g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]], g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]]);
       bcm_vlan_port_t vlan_port;
       bcm_vlan_port_t_init(&vlan_port);
       vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
       vlan_port.vlan_port_id = g_l2_basic_bridge_trunk.inLifList[IN_PORT_INDEX[0]];
       vlan_port.port = trunk_gport;
       vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;
       rv = bcm_vlan_port_create(unit, &vlan_port);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
           return rv;
       }

       rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge_trunk.vsi,  vlan_port.vlan_port_id);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vswitch_port_add\n");
           return rv;
       }
    }


    /*
     * Create Outlif for Lag port
     */
    
    /*
     * bcm_port_match_info_t match_info;
     *
     *
     * bcm_vlan_port_t_init(&vlan_port); bcm_port_match_info_t_init(&match_info);
     *
     * vlan_port.flags = (BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_WITH_ID); vlan_port.criteria =
     * BCM_VLAN_PORT_MATCH_PORT; vlan_port.vlan_port_id = (17 << 26 | 2 << 22 | 10000);
     *
     * rv = bcm_vlan_port_create(unit, &vlan_port); if (rv != BCM_E_NONE) { printf("Error, in bcm_vlan_port_create(port
     * = %d)\n", vlan_port.port); return rv; }
     *
     * match_info.match = BCM_PORT_MATCH_PORT; match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY; match_info.port =
     * trunk_gport; rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info); if (rv != BCM_E_NONE) {
     * printf("Error, in bcm_port_match_add \n"); return rv; }
     */

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

       
        rv = bcm_port_force_forward_set(unit, trunk_gport, g_l2_basic_bridge_trunk.ports[OUT_PORT_INDEX[NUM_OF_OUT_PORTS-1]], 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_force_forward_set\n");
            return rv;
        }

        printf("bcm_port_force_forward_set(source_port = 0x%08X, dest_port = %d)\n", trunk_gport, g_l2_basic_bridge_trunk.ports[OUT_PORT_INDEX[NUM_OF_OUT_PORTS-1]]);
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
