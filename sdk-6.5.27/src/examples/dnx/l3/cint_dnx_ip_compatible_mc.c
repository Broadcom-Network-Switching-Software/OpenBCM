/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  * 
 * The cint creates IP Compatible MC scenario                                                                                                       * 
 * IPv4oETH1 packet with MACDA[47:23] =={24'h01_00_5E,1'b0} and DIP is MC Group                                                                     * 
 *                                                                                                                                                  * 
 *    Packet_is_ip_compatible                                                                                                                       * 
 *   -------------------------------------------------->+--------------------+                                                                      *
 *                              +------+  VSI.Profile   |       LOOKUP       |                                                                      *
 *                              |  VSI |--------------->|       ======       +                                                                      *
 *                              |      |      5         +                    |                                                                      *    
 *                              |  10  |--------------->|   1.Bridge SVL     +-----> MCID1(70)                                                      *
 *             +-------+        |      |      6         |                    |                                                                      *
 *    Port_1   |       |        |  20  |--------------->|   2.Bridge IVL     +-----> MCID2(80)                                                      *
 *    +------> | LIF   +------> |      |      6         |                    |                                                                      *
 *             |       |        |  20  |--------------->+   3.Bridge Flood   +-----> Unknown DA Destination flooding (22)                           *
 *             +-------+        |      |      7         |                    |                                                                      *
 *                              |  30  |--------------->|   4.V4 SVL LEM     +-----> MCID3 (50)                                                     *
 *                              |      |      8         |                    |                                                                      *
 *                              |  40  |--------------->|   5.V4 IVL LEM     +-----> MCID4 (60)                                                     *
 *                              |      |      7         |                    |                                                                      *
 *                              |  30  |--------------->|   6.V4 SVL KAPS    +-----> MCID3 (50)                                                     *
 *                              |      |      8         |                    |                                                                      *
 *                              |  40  |--------------->|   7.V4 IVL KAPS    +-----> MCID4 (60)                                                     *
 *                              |      |      7         |                    |                                                                      *
 *                              |  30  |--------------->|   7.V4 Flood       +-----> Unknown DA Destination flooding (27)                           *
 *                              +------+                +--------------------+                                                                      *
 *                                                                                                                                                  *
 * Following is th MCID configurations                                                                                                              *
 *                                                                                                                                                  *
 *                  +-----------+                               +-----------+                        +-----------+                                  *
 *                  |           |--------> Port_2               |           |--------> Port_2        |           |--------> Port_2                  *
 *                  |  MCID1    |--------> Port_3               |  MCID2    |--------> Port_3        |  MCID4    |--------> Port_3                  *
 *                  |    70     |--------> Port_4               |    80     |--------> Port_4        |    60     |--------> Port_4                  *
 *                  |           |--------> Port_1               |           |                        |           |                                  *
 *                  +-----------+   (SRC port should not        +-----------+                        +-----------+                                  *
 *                                           Packet)                                                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                  +-----------+                               +-----------+                                                                       *
 *                  |           |--------> Port_2               |           |--------> Port_2              +-----------+                            *
 *                  |  MCID3    |--------> Port_3               |  MCID1    |--------> Port_3              |           |--------> Port_2            *
 *                  |    50     |                               |    22     |                              |  MCID1    |--------> Port_3            *
 *                  |           |                               |           |                              |    27     |--------> Port_4            *
 *                  +-----------+                               +-----------+                              |           |                            *
 *                                                                                                         +-----------+                            *
 *                                                                                                                                                  *
 * Following is the detailed flows covered by the cint :                                                                                            *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   | VSI Profile   |   FLP Context     |  MACDA            |    DIP        |   MCID      |  Egress Ports              | Remarks                 | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     5         |  Bridge SVL - 0   | 01:00:5E:00:00:02 |  224.0.0.2    |   70        | Port_2,Port_3,Port_4,Port_1| SRC_PORT_REMOVE         | *
 *   +-------------------------------------------------------------------------------------------------------------------------------------+------+ *
 *   |     6         |  Bridge IVL - 1   | 01:00:5E:00:00:03 |  224.0.0.3    |   80        | Port_2,Port_3,Port_4        |                        | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     6         |  Bridge Flood - 1 | 01:00:5E:00:00:04 |  224.0.0.4    |   22        |     Port_2,Port_3           | Unknown da flood       | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     7         |  LEM V4 SVL - 2   | 01:00:5E:00:00:05 |  224.0.0.5    |   50        |     Port_2,Port_3           |                        | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     8         |  LEM V4 IVL - 3   | 01:00:5E:00:00:06 |  224.0.0.6    |   60        | Port_2,Port_3,Port_4        |                        | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     7         | KAPS V4 SVL - 2   | 01:00:5E:00:00:07 |  224.0.0.7    |   50        |     Port_2,Port_3           |                        | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     8         | KAPS V4 IVL - 3   | 01:00:5E:00:00:08 |  224.0.0.8    |   60        | Port_2,Port_3,Port_4        |                        | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     7         | KAPS V4 SVL - 2   | 01:00:5E:00:00:07 |  224.0.0.7    |   27        | Port_2,Port_3,Port_4        | lookup miss flood      | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *                                                                                                                                                  *
 *                                                                                                                                                  * *                                                                                                                                                  *
 * Inputs :                                                                                                                                         * 
 *         STAG IP compatible Packet                                                                                                                * 
 * Outputs :                                                                                                                                        * 
 *         According to VSI profile lookup happens as described above and packets will be forwarded to corresponding group                          * 
 *                                                                                                                                                  * 
 * Cint use :                                                                                                                                       * 
 *         ip_compatible_mc_with_ports__main_config__start_run($unit,$Port_1,$Port_2,$Port_3,$Port_4)                                               * 
 *                                                                                                                                                  * 
*************************************************************************************************************************************************** */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

int IN_PORT_INDEX = 0;
int NUM_OF_OUT_PORTS = 3;
int OUT_PORT_INDEX[NUM_OF_OUT_PORTS] = { 1, 2, 3 };
int NUM_OF_PORTS = 4;
int VSI_INDEX = 4;
int MCID_INDEX = 6;
int FLOOD_INDEX = 2;

struct global_tpids_s
{
    uint16 s_tag;               /* ingress packet s_tag (outer tag) */
    uint16 c_tag;               /* ingress packet c_tag (inner tag) */
    uint16 outer_tpid;          /* egress packet outer tag */
    uint16 inner_tpid;          /* egress packet inner tag */
};

/*  Main Struct  */
struct ip_compatible_mc_s
{
    bcm_gport_t ports[NUM_OF_PORTS];
    bcm_vlan_t vsi[VSI_INDEX];
    bcm_multicast_t mcid[MCID_INDEX];
    int nof_replications;
    global_tpids_s tpid_value;
    bcm_vlan_t outer_vid[VSI_INDEX];
    bcm_gport_t inLifList[NUM_OF_PORTS];
    bcm_gport_t outLifList[VSI_INDEX][NUM_OF_PORTS];
    bcm_mac_t d_mac;
    int unknown_da_profile_destination;
    int vsi_da_not_found_destination[FLOOD_INDEX];
    bcm_ip_t lem_svl_dip;
    bcm_ip_t lem_ivl_dip;
    bcm_ip_t kaps_svl_dip;
    bcm_ip_t kaps_svl_sip;
    bcm_ip_t kaps_ivl_dip;
    bcm_ip_t kaps_ivl_sip;
};

/* Initialization of global struct*/
ip_compatible_mc_s g_ip_compatible_mc = {
    /*
     * ports:
     */
    {0, 0, 0, 0},
    /*
     * vsi:
     */
    {10, 20, 30, 40},
    /*
     * mcid 
     */
    {70, 80, 70, 50, 60, 0},
    /*
     * nof_replications 
     */
    1,
    /*
     * tpid_value: |s_tag | c_tag | outer | inner |
     */
    {0x8100, 0x9100, 0x8100, 0x9100},
    /*
     * outer_vid:
     */
    {100, 200, 300, 400},
    /*
     * inLifList: 
     */
    {0, 0, 0, 0},
    /*
     * outLifList: 
     */
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    /*
     * d_mac 
     */
    {0x01, 0x00, 0x5E, 0x00, 0x00, 0x02},
    /*
     * unknown_da_profile_destination
     */
    7,
    /*
     * vsi_da_not_found_destination
     */
    {15, 20},
    /*
     * lem_svl_dip
     */
    0xe0000005,
    /*
     * lem_ivl_dip
     */
    0xe0000006,
    /*
     * kaps_svl_dip
     */
    0xe0000007,
    /*
     * kaps_svl_dip
     */
    0xa0b0c07,
    /*
     * kaps_svl_dip
     */
    0xe0000008,
    /*
     * kaps_svl_dip
     */
    0xa0b0c08,
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */
/* Initialization of main struct
 * Function allow to re-write default values 
 *
 * INPUT: 
 *   params: new values for g_ip_compatible_mc
 */
void
ip_compatible_mc_init(
    ip_compatible_mc_s * param)
{


    if (param != NULL)
    {
        sal_memcpy(&g_ip_compatible_mc, param, sizeof(g_ip_compatible_mc));
    }

}

/* 
 * Return g_ip_compatible_mc information
 */
void
ip_compatible_mc_struct_get(
    int unit,
    ip_compatible_mc_s * param)
{

    sal_memcpy(param, &g_ip_compatible_mc, sizeof(g_ip_compatible_mc));

    return;
}

/* This function runs the main test function with given ports
 *  
 * INPUT: unit     - unit
 *        in_port  - ingress port 
 *        out_port - egress port
 */
int
ip_compatible_mc_with_ports__main_config__start_run(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3)
{

    ip_compatible_mc_s param;

    ip_compatible_mc_struct_get(unit, &param);

    param.ports[IN_PORT_INDEX] = in_port;
    param.ports[OUT_PORT_INDEX[0]] = out_port1;
    param.ports[OUT_PORT_INDEX[1]] = out_port2;
    param.ports[OUT_PORT_INDEX[2]] = out_port3;

    return ip_compatible_mc__main_config__start_run(unit, &param);

}

/*
 *  
 * Main steps of configuration: 
 *    1. Initialize test parameters
 *    2. Setting VLAN domain for ingress/egress ports.
 *    3. Setting relevant TPID's for ingress/egress ports.
 *    4. Configuring Lookup tables to get Multicast group
 *    5. Creating multicast groups with associated ports 
 *    
 * INPUT: unit  - unit
 *        param - new values for ip_compatible struct                                        
 */
int
ip_compatible_mc__main_config__start_run(
    int unit,
    ip_compatible_mc_s * param)
{
    char error_msg[200]={0,};
    int temp_i;
    int temp_j;
    int flags;
    int ivl_feature_support = 1;
    uint32 mc_flags = 0;

    ip_compatible_mc_init(param);


    /*
     * set VLAN domain to each port 
     */
    for (temp_i = 0; temp_i < NUM_OF_PORTS; temp_i++)
    {
        snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_ip_compatible_mc.ports[temp_i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_ip_compatible_mc.ports[temp_i], bcmPortClassId,
                                g_ip_compatible_mc.ports[temp_i]), error_msg);

    }

    /*
     * Set TPIDs
     */
    ivl_feature_support = *dnxc_data_get(unit, "l2", "general", "ivl_feature_support", NULL);

    BCM_IF_ERROR_RETURN_MSG(tpid__tpids_clear_all(unit), "");

    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_ip_compatible_mc.tpid_value.s_tag);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_ip_compatible_mc.tpid_value.s_tag), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_ip_compatible_mc.tpid_value.c_tag);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_ip_compatible_mc.tpid_value.c_tag), error_msg);
    /*
     * Create VSI:
     */
    for (temp_i = 0; temp_i < VSI_INDEX; temp_i++)
    {
        snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", g_ip_compatible_mc.vsi[temp_i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, g_ip_compatible_mc.vsi[temp_i]), error_msg);
    }

    /*
     * Set VLAN port membership
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    for (temp_i = 0; temp_i < NUM_OF_PORTS; temp_i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, g_ip_compatible_mc.ports[temp_i]);
    }

    for (temp_i = 0; temp_i < VSI_INDEX; temp_i++)
    {
        snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", g_ip_compatible_mc.vsi[temp_i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, g_ip_compatible_mc.vsi[temp_i], pbmp, untag_pbmp), error_msg);
        snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", g_ip_compatible_mc.outer_vid[temp_i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, g_ip_compatible_mc.outer_vid[temp_i], g_ip_compatible_mc.ports[IN_PORT_INDEX], 0), "");
    }

    /*
     * Creates Default LIF for Tx port
     *
     */

    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);

    bcm_gport_t temp_inlif[VSI_INDEX];
    temp_inlif[0] = (17 << 26 | 2 << 22 | 5000);
    temp_inlif[1] = (17 << 26 | 2 << 22 | 6000);
    temp_inlif[2] = (17 << 26 | 2 << 22 | 7000);
    temp_inlif[3] = (17 << 26 | 2 << 22 | 8000);

    /*
     * Port_1 create lif create ISEM-{VDxVLAN}
     */
    /*
     * 4 different lisf will be created with the following configs ---------------------------- | VSI | VLAN | Inlif |
     * ---------------------------- | 10 | 100 | 5000 | ---------------------------- | 20 | 200 | 6000 |
     * ---------------------------- | 30 | 300 | 7000 | ---------------------------- | 40 | 400 | 8000 |
     * ---------------------------- 
     */
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = g_ip_compatible_mc.ports[IN_PORT_INDEX];

    for (temp_i = 0; temp_i < VSI_INDEX; temp_i++)
    {
        vlan_port.vlan_port_id = temp_inlif[temp_i];
        vlan_port.vsi = 0;
        vlan_port.match_vlan = g_ip_compatible_mc.outer_vid[temp_i];

        snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);
        g_ip_compatible_mc.inLifList[IN_PORT_INDEX + temp_i] = vlan_port.vlan_port_id;

        /*
         * Add inlifs to VSI
         */
        snprintf(error_msg, sizeof(error_msg), "for vsi=%d, lif=%d", g_ip_compatible_mc.vsi[temp_i],
            g_ip_compatible_mc.inLifList[IN_PORT_INDEX + temp_i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_ip_compatible_mc.vsi[temp_i],
                                  g_ip_compatible_mc.inLifList[IN_PORT_INDEX + temp_i]), error_msg);
    }

    /*
     * Creates Out LIFs (port x VLAN)
     */

    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;

    for (temp_j = 0; temp_j < VSI_INDEX; temp_j++)
    {
        vlan_port.match_vlan = g_ip_compatible_mc.outer_vid[temp_j];
        vlan_port.vsi = 0;

        for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
        {
            vlan_port.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];

            snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
            BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

            g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]] = vlan_port.vlan_port_id;

            printf("port=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.vlan_port_id,
                   vlan_port.encap_id);

            /*
             * Add outlifs to VSI
             */
            snprintf(error_msg, sizeof(error_msg), "for vsi=%d, lif=%d", g_ip_compatible_mc.vsi[temp_j],
                g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]]);
            BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_ip_compatible_mc.vsi[temp_j],
                                      g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]]), error_msg);
        }
    }

    /*
     * IP compatible MC with DA and FID lookup 
     */

    /*
     * SVL with MAC = 01:00:5E:00:00:02 and VSI = 10
     */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, g_ip_compatible_mc.d_mac, g_ip_compatible_mc.vsi[0]);
    l2_addr.flags = BCM_L2_MCAST;
    l2_addr.l2mc_group = g_ip_compatible_mc.mcid[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");
    /*
     * IVL with MAC = 01:00:5E:00:00:03 and VSI = 20 VID=200
     */
    if (ivl_feature_support == 1)
    {
        bcm_mac_t ivl_da = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0x03 };
        g_ip_compatible_mc.d_mac = ivl_da;
        bcm_l2_addr_t_init(&l2_addr, g_ip_compatible_mc.d_mac, g_ip_compatible_mc.vsi[1]);
        l2_addr.flags = BCM_L2_MCAST;
        l2_addr.l2mc_group = g_ip_compatible_mc.mcid[1];
        l2_addr.modid = g_ip_compatible_mc.outer_vid[1];
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");
    }

    /*
     * Multicast Configuration for Bridge IVL and SVL
     */
    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);

    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        mc_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    } else {
        mc_flags = BCM_MULTICAST_INGRESS_GROUP;
    }
    for (temp_j = 0; temp_j < 2; temp_j++)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | mc_flags, &g_ip_compatible_mc.mcid[temp_j]), "");

        for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
        {
            rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
            rep_array.encap1 = (g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

            snprintf(error_msg, sizeof(error_msg), "in Port= %d", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, g_ip_compatible_mc.mcid[temp_j], mc_flags, g_ip_compatible_mc.nof_replications,
                                   &rep_array), error_msg);
        }
    }

    /*
     * Adding input port to the mc group source kncokout should happen
     */
    rep_array.port = g_ip_compatible_mc.ports[IN_PORT_INDEX];
    rep_array.encap1 = (g_ip_compatible_mc.inLifList[IN_PORT_INDEX] & 0x3fffff);
    snprintf(error_msg, sizeof(error_msg), "in Port= %d", g_ip_compatible_mc.ports[IN_PORT_INDEX]);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, g_ip_compatible_mc.mcid[0], mc_flags, g_ip_compatible_mc.nof_replications, &rep_array), error_msg);

    /*
     * Bridge Unknown Multicast configuration
     */
    bcm_port_flood_group_t flood_groups;
    bcm_gport_t flood_destination;
    bcm_port_flood_group_t_init(&flood_groups);
    BCM_GPORT_MCAST_SET(flood_destination, g_ip_compatible_mc.unknown_da_profile_destination);
    flood_groups.unknown_multicast_group = flood_destination;
    if (*dnxc_data_get(unit, "l2", "feature", "bc_same_as_unknown_mc", NULL)) {
        flood_groups.broadcast_group = flood_groups.unknown_multicast_group;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_port_flood_group_set(unit, g_ip_compatible_mc.ports[IN_PORT_INDEX], 0, &flood_groups), "");

    bcm_vlan_control_vlan_t control;
    if (ivl_feature_support == 1)
    {
        /**Configure IVL mode for VSI=40*/
        control.flags2 = BCM_VLAN_FLAGS2_IVL;
        control.broadcast_group = g_ip_compatible_mc.vsi[3];
        control.unknown_multicast_group = g_ip_compatible_mc.vsi[3];
        control.unknown_unicast_group = g_ip_compatible_mc.vsi[3];
        control.forwarding_vlan = g_ip_compatible_mc.vsi[3];
        snprintf(error_msg, sizeof(error_msg), "in vsi= %d", g_ip_compatible_mc.vsi[3]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(unit, g_ip_compatible_mc.vsi[3], control), error_msg);
        /**Configure IVL mode for VSI=20*/
        control.flags2 = BCM_VLAN_FLAGS2_IVL;
        control.broadcast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
        control.unknown_multicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
        control.unknown_unicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
        control.forwarding_vlan = g_ip_compatible_mc.vsi[1];
        snprintf(error_msg, sizeof(error_msg), "in vsi= %d", g_ip_compatible_mc.vsi[1]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(unit, g_ip_compatible_mc.vsi[1], control), error_msg);

    }

    /** Configure L3 rif for VSI=40, this will enable fwd mode = IPMC*/
    bcm_mac_t mac_address_1 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
    bcm_l3_intf_t intf;
    bcm_l3_intf_t_init(&intf);
    intf.l3a_vid = g_ip_compatible_mc.vsi[3];
    sal_memcpy(intf.l3a_mac_addr, mac_address_1, 6);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, intf), "");
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_ingress_t_init(ingress_intf);
    ingress_intf.vrf = g_ip_compatible_mc.vsi[3];
    ingress_intf.flags =  BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_L3_MCAST_L2;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id), "");

    /** Configure L3 rif for VSI=30, this will enable fwd mode = IPMC*/
    intf.l3a_vid = g_ip_compatible_mc.vsi[2];
    sal_memcpy(intf.l3a_mac_addr, mac_address_1, 6);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, intf), "");

    ingress_intf.vrf = g_ip_compatible_mc.vsi[2];
    ingress_intf.flags =  BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_L3_MCAST_L2;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id), "");
    /**Configure SVL mode for VSI=30*/
    control.flags2 = 0;
    control.broadcast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
    control.unknown_multicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
    control.unknown_unicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
    control.forwarding_vlan = g_ip_compatible_mc.vsi[2];
    snprintf(error_msg, sizeof(error_msg), "in vsi= %d", g_ip_compatible_mc.vsi[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(unit, g_ip_compatible_mc.vsi[2], control), error_msg);
    g_ip_compatible_mc.mcid[2] =
        g_ip_compatible_mc.vsi_da_not_found_destination[0] + g_ip_compatible_mc.unknown_da_profile_destination;

    /*
     * Replication configuration for unknown multicast
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | mc_flags, &g_ip_compatible_mc.mcid[2]), "");

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS - 1; temp_i++)
    {
        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[1][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        snprintf(error_msg, sizeof(error_msg), "in Port= %d", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, g_ip_compatible_mc.mcid[2], mc_flags, g_ip_compatible_mc.nof_replications,
                               &rep_array), error_msg);
    }

    /*
     * setting LEM V4_SVL
     */
    bcm_ipmc_addr_t l2_ipmc_addr;
    bcm_ipmc_addr_t_init(&l2_ipmc_addr);
    l2_ipmc_addr.mc_ip_addr = g_ip_compatible_mc.lem_svl_dip;
    l2_ipmc_addr.mc_ip_mask = 0xFFFFFFFF;
    l2_ipmc_addr.vid = g_ip_compatible_mc.vsi[2];
    l2_ipmc_addr.group = g_ip_compatible_mc.mcid[3];
    l2_ipmc_addr.flags |= BCM_IPMC_L2;
    BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &l2_ipmc_addr), "");

    /*
     * Multicast Configuration for V4_SVL
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | mc_flags, &g_ip_compatible_mc.mcid[3]), "");

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS - 1; temp_i++)
    {
        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[2][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        snprintf(error_msg, sizeof(error_msg), "in Port= %d", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, g_ip_compatible_mc.mcid[3], mc_flags, g_ip_compatible_mc.nof_replications,
                               &rep_array), error_msg);
    }

    /*
     * LEM V4_IVL
     */
    if (ivl_feature_support == 1)
    {
        bcm_ipmc_addr_t_init(&l2_ipmc_addr);
        l2_ipmc_addr.mc_ip_addr = g_ip_compatible_mc.lem_ivl_dip;
        l2_ipmc_addr.mc_ip_mask = 0xFFFFFFFF;
        l2_ipmc_addr.vid = g_ip_compatible_mc.vsi[3];
        l2_ipmc_addr.mod_id = g_ip_compatible_mc.outer_vid[3];
        l2_ipmc_addr.group = g_ip_compatible_mc.mcid[4];
        l2_ipmc_addr.flags |= BCM_IPMC_L2;
        BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &l2_ipmc_addr), "");
    }

    /*
     * Multicast Configuration for V4_SVL
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | mc_flags, &g_ip_compatible_mc.mcid[4]), "");

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
    {
        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[3][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        snprintf(error_msg, sizeof(error_msg), "in Port= %d", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, g_ip_compatible_mc.mcid[4], mc_flags, g_ip_compatible_mc.nof_replications,
                               &rep_array), error_msg);
    }

    /*
     * setting KAPS V4_SVL
     */
    bcm_ipmc_addr_t_init(&l2_ipmc_addr);
    l2_ipmc_addr.mc_ip_addr = g_ip_compatible_mc.kaps_svl_dip;
    l2_ipmc_addr.mc_ip_mask = 0xFFFFFFFF;
    l2_ipmc_addr.s_ip_addr = g_ip_compatible_mc.kaps_svl_sip;
    l2_ipmc_addr.s_ip_mask = 0xFFFFFFFF;
    l2_ipmc_addr.vid = g_ip_compatible_mc.vsi[2];
    l2_ipmc_addr.group = g_ip_compatible_mc.mcid[3];
    l2_ipmc_addr.flags |= BCM_IPMC_L2;
    BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &l2_ipmc_addr), "");

    /*
     * setting KAPS V4_IVL
     */
    if (ivl_feature_support == 1)
    {
        bcm_ipmc_addr_t_init(&l2_ipmc_addr);
        l2_ipmc_addr.mc_ip_addr = g_ip_compatible_mc.kaps_ivl_dip;
        l2_ipmc_addr.mc_ip_mask = 0xFFFFFFFF;
        l2_ipmc_addr.s_ip_addr = g_ip_compatible_mc.kaps_ivl_sip;
        l2_ipmc_addr.s_ip_mask = 0xFFFFFFFF;
        l2_ipmc_addr.vid = g_ip_compatible_mc.vsi[3];
        l2_ipmc_addr.mod_id = g_ip_compatible_mc.outer_vid[3];
        l2_ipmc_addr.group = g_ip_compatible_mc.mcid[4];
        l2_ipmc_addr.flags |= BCM_IPMC_L2;
        BCM_IF_ERROR_RETURN_MSG(bcm_ipmc_add(unit, &l2_ipmc_addr), "");
    }

    /*
     * v4 Unknown Multicast configuration
     */

    /*
     * Configures vsi.da_not_found_destination
     */
    control.flags2 = 0;
    control.broadcast_group = g_ip_compatible_mc.vsi_da_not_found_destination[1];
    control.unknown_multicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[1];
    control.unknown_unicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[1];
    control.forwarding_vlan = g_ip_compatible_mc.vsi[2];
    snprintf(error_msg, sizeof(error_msg), "in vsi= %d", g_ip_compatible_mc.vsi[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(unit, g_ip_compatible_mc.vsi[2], control), error_msg);


    g_ip_compatible_mc.mcid[5] =
        g_ip_compatible_mc.vsi_da_not_found_destination[1] + g_ip_compatible_mc.unknown_da_profile_destination;

    /*
     * Replication configuration for unknown multicast
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | mc_flags, &g_ip_compatible_mc.mcid[5]), "");

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
    {
        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[2][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        snprintf(error_msg, sizeof(error_msg), "in Port= %d", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, g_ip_compatible_mc.mcid[5], mc_flags, g_ip_compatible_mc.nof_replications,
                               &rep_array), error_msg);

    }

    return BCM_E_NONE;
}

/*
 * Cleanup function runs the vlan translate tpid modify example 
 *  
 * Main steps of configuration: 
 *    1. Delete all TPIDs.
 *    2. Destroy VSI.
 *    3. Destroy LIFs.
 *    4. Destroy Multicast group.
 *    
 * INPUT: unit - unit
 */
int
ip_compatible_mc__cleanup__start_run(
    int unit)
{
    char error_msg[200]={0,};
    int temp_i;

    printf("ip_compatible_mc__cleanup__start_run\n");

    /*
     * Destroy OutLIFs
     */
    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
    {

        snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_ip_compatible_mc.outLifList[OUT_PORT_INDEX[temp_i]]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, g_ip_compatible_mc.outLifList[OUT_PORT_INDEX[temp_i]]), error_msg);

    }

    /*
     * Destroy InLIFs
     */

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_ip_compatible_mc.inLifList[IN_PORT_INDEX]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, g_ip_compatible_mc.inLifList[IN_PORT_INDEX]), error_msg);

    /*
     * VSI
     */
    snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", g_ip_compatible_mc.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_destroy(unit, g_ip_compatible_mc.vsi), error_msg);

    /*
     * TPIDs
     */
    BCM_IF_ERROR_RETURN_MSG(tpid__tpids_clear_all(unit), "");

    /*
     * Destroy Multicast Group 
     */
    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_group_destroy(unit, (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC), g_ip_compatible_mc.mcid), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_destroy(unit, g_ip_compatible_mc.mcid), "");
    }

    return BCM_E_NONE;
}
