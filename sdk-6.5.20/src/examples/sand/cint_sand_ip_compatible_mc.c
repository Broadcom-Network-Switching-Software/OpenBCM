/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     6         |  Bridge Flood - 1 | 01:00:5E:00:00:04 |  224.0.0.4    |   22        |     Port_2,Port_3           | Unknown da flood       | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     7         |  LEM V4 SVL - 2   | 01:00:5E:00:00:05 |  224.0.0.5    |   50        |     Port_2,Port_3           |                        | *
 *   +--------------------------------------------------------------------------------------------------------------------------------------------+ *
 *   |     7         | KAPS V4 SVL - 2   | 01:00:5E:00:00:07 |  224.0.0.7    |   50        |     Port_2,Port_3           |                        | *
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
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

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
 */
/* Initialization of main struct
 * Function allow to re-write default values 
 *
 * INPUT: 
 *   params: new values for g_ip_compatible_mc
 */
int
ip_compatible_mc_init(
    int unit,
    ip_compatible_mc_s * param)
{

    int rv;

    if (param != NULL)
    {
        sal_memcpy(&g_ip_compatible_mc, param, sizeof(g_ip_compatible_mc));
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
    int rv;

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
    int rv;
    int temp_i;
    int temp_j;
    int flags;

    rv = ip_compatible_mc_init(unit, param);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in ip_compatible_mc_init\n");
        return rv;
    }

    /*
     * set VLAN domain to each port 
     */
    for (temp_i = 0; temp_i < NUM_OF_PORTS; temp_i++)
    {
        rv = bcm_port_class_set(unit, g_ip_compatible_mc.ports[temp_i], bcmPortClassId,
                                g_ip_compatible_mc.ports[temp_i]);

        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", g_ip_compatible_mc.ports[temp_i]);
            return rv;
        }
    }

    /*
     * Set TPIDs
     */
    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    /*
     * In JR1, only two TPIDs per port, thus the test uses {s_tag, s_tag} for the tx dtag packet and 
     * {c_tag, c_tag} rx packet for the modified TPID values.
     */
    if (!is_jericho2)
    {
        /*
         * JER1
         */

        for (temp_i = 0; temp_i < NUM_OF_PORTS; temp_i++)
        {

            rv = bcm_port_tpid_delete_all(unit, g_ip_compatible_mc.ports[temp_i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_delete_all(port=%d)\n", g_ip_compatible_mc.ports[temp_i]);
                return rv;
            }

            rv = bcm_port_tpid_add(unit, g_ip_compatible_mc.ports[temp_i], g_ip_compatible_mc.tpid_value.s_tag, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_add(port=%d, tpid=0x%04x)\n", g_ip_compatible_mc.ports[temp_i],
                       g_ip_compatible_mc.tpid_value.s_tag);
                return rv;
            }

            rv = bcm_port_tpid_add(unit, g_ip_compatible_mc.ports[temp_i], g_ip_compatible_mc.tpid_value.outer_tpid, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_add(port=%d, tpid=0x%04x)\n", g_ip_compatible_mc.ports[temp_i],
                       g_ip_compatible_mc.tpid_value.outer_tpid);
                return rv;
            }
        }
    }
    else
    {
        /*
         * JR2
         */
        rv = tpid__tpids_clear_all(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_switch_tpid_delete_all\n");
            return rv;
        }

        rv = tpid__tpid_add(unit, g_ip_compatible_mc.tpid_value.s_tag);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_ip_compatible_mc.tpid_value.s_tag, rv);
            return rv;
        }

        rv = tpid__tpid_add(unit, g_ip_compatible_mc.tpid_value.c_tag);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_ip_compatible_mc.tpid_value.c_tag, rv);
            return rv;
        }
    }

    /*
     * Create VSI:
     */
    for (temp_i = 0; temp_i < VSI_INDEX; temp_i++)
    {
        rv = bcm_vlan_create(unit, g_ip_compatible_mc.vsi[temp_i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_create(vsi=%d)\n", g_ip_compatible_mc.vsi[temp_i]);
            return rv;
        }
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
        rv = bcm_vlan_port_add(unit, g_ip_compatible_mc.vsi[temp_i], pbmp, untag_pbmp);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_port_add(vsi=%d)\n", g_ip_compatible_mc.vsi[temp_i]);
            return rv;
        }
        rv = bcm_vlan_gport_add(unit, g_ip_compatible_mc.outer_vid[temp_i], g_ip_compatible_mc.ports[IN_PORT_INDEX], 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_gport_add(vsi=%d)\n", g_ip_compatible_mc.outer_vid[temp_i]);
            return rv;
        }
    }

    /*
     * Creates Default LIF for Tx port
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

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
            return rv;
        }
        g_ip_compatible_mc.inLifList[IN_PORT_INDEX + temp_i] = vlan_port.vlan_port_id;

        /*
         * Add inlifs to VSI
         */
        rv = bcm_vswitch_port_add(unit, g_ip_compatible_mc.vsi[temp_i],
                                  g_ip_compatible_mc.inLifList[IN_PORT_INDEX + temp_i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_vlan_switch_port_add for vsi=%d, lif=%d\n", g_ip_compatible_mc.vsi[temp_i],
                   g_ip_compatible_mc.inLifList[IN_PORT_INDEX + temp_i]);
            return rv;
        }
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

            rv = bcm_vlan_port_create(unit, &vlan_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
                return rv;
            }

            g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]] = vlan_port.vlan_port_id;

            printf("port=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.vlan_port_id,
                   vlan_port.encap_id);

            /*
             * Add outlifs to VSI
             */
            rv = bcm_vswitch_port_add(unit, g_ip_compatible_mc.vsi[temp_j],
                                      g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]]);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_vlan_switch_port_add for vsi=%d, lif=%d\n", g_ip_compatible_mc.vsi[temp_j],
                       g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]]);
                return rv;
            }

        }
    }

    /*
     * IP compatible MC with DA and FID lookup 
     */
    /*
     * rv = bcm_switch_control_set(unit,bcmSwitchL3McastL2,0); if (rv != BCM_E_NONE) { printf("Error,
     * bcm_switch_control_set\n"); return rv; } 
     */

    /*
     * SVL with MAC = 01:00:5E:00:00:02 and VSI = 10
     */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, g_ip_compatible_mc.d_mac, g_ip_compatible_mc.vsi[0]);
    l2_addr.flags = BCM_L2_MCAST;
    l2_addr.l2mc_group = g_ip_compatible_mc.mcid[0];
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    /*
     * Multicast Configuration for Bridge IVL and SVL
     */
    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);

    for (temp_j = 0; temp_j < 2; temp_j++)
    {

        flags = ((is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);
        rv = bcm_multicast_create(unit, flags, &g_ip_compatible_mc.mcid[temp_j]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_create\n");
            return rv;
        }

        flags = (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_INGRESS_GROUP;

        for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
        {

            rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
            rep_array.encap1 = (g_ip_compatible_mc.outLifList[temp_j][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

            rv = bcm_multicast_add(unit, g_ip_compatible_mc.mcid[temp_j], flags, g_ip_compatible_mc.nof_replications,
                                   &rep_array);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_multicast_add in Port= %d\n", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
                return rv;
            }

        }
    }

    /*
     * Adding input port to the mc group source kncokout should happen
     */
    rep_array.port = g_ip_compatible_mc.ports[IN_PORT_INDEX];
    rep_array.encap1 = (g_ip_compatible_mc.inLifList[IN_PORT_INDEX] & 0x3fffff);
    rv = bcm_multicast_add(unit, g_ip_compatible_mc.mcid[0], flags, g_ip_compatible_mc.nof_replications, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= %d\n", g_ip_compatible_mc.ports[IN_PORT_INDEX]);
        return rv;
    }

    /*
     * Bridge Unknown Multicast configuration
     */
    if (!is_jericho2)
    {
        /*
         * Configures PINFO_FLP_MEM.action_profile_da_not_found_index=1 and Unknown_DA_profile.destination 
         */
        rv = bcm_port_control_set(unit, g_ip_compatible_mc.ports[IN_PORT_INDEX], bcmPortControlFloodUnknownMcastGroup,
                                  g_ip_compatible_mc.unknown_da_profile_destination);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_control_set in Port= %d\n", g_ip_compatible_mc.ports[IN_PORT_INDEX]);
            return rv;
        }
    }
    else
    {
        bcm_port_flood_group_t flood_groups;
        bcm_gport_t flood_destination;
        bcm_port_flood_group_t_init(&flood_groups);
        BCM_GPORT_MCAST_SET(flood_destination, g_ip_compatible_mc.unknown_da_profile_destination);
        flood_groups.unknown_multicast_group = flood_destination;
        if (*dnxc_data_get(unit, "l2", "feature", "bc_same_as_unknown_mc", NULL)) {
            flood_groups.broadcast_group = flood_groups.unknown_multicast_group;
        }
        rv = bcm_port_flood_group_set(unit, g_ip_compatible_mc.ports[IN_PORT_INDEX], 0, &flood_groups);
    }
    
    bcm_vlan_control_vlan_t control;
    /** Configure L3 rif for VSI=40, this will enable fwd mode = IPMC*/
    bcm_mac_t mac_address_1 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
    bcm_l3_intf_t intf;
    bcm_l3_intf_t_init(&intf);
    intf.l3a_vid = g_ip_compatible_mc.vsi[3];
    sal_memcpy(intf.l3a_mac_addr, mac_address_1, 6);
    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create");
        return rv;
    }
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_ingress_t_init(ingress_intf);
    ingress_intf.vrf = g_ip_compatible_mc.vsi[3];
    ingress_intf.flags =  BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_L3_MCAST_L2;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create");
        return rv;
    }

    /** Configure L3 rif for VSI=30, this will enable fwd mode = IPMC*/
    intf.l3a_vid = g_ip_compatible_mc.vsi[2];
    sal_memcpy(intf.l3a_mac_addr, mac_address_1, 6);
    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create");
        return rv;
    }

    ingress_intf.vrf = g_ip_compatible_mc.vsi[2];
    ingress_intf.flags =  BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST | BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_L3_MCAST_L2;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }
    /**Configure SVL mode for VSI=30*/
    control.flags2 = 0;
    control.broadcast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
    control.unknown_multicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
    control.unknown_unicast_group = g_ip_compatible_mc.vsi_da_not_found_destination[0];
    control.forwarding_vlan = g_ip_compatible_mc.vsi[2];
    rv = bcm_vlan_control_vlan_set(unit, g_ip_compatible_mc.vsi[2], control);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_vlan_set in vsi= %d\n", g_ip_compatible_mc.vsi[2]);
        return rv;
    }
    g_ip_compatible_mc.mcid[2] =
        g_ip_compatible_mc.vsi_da_not_found_destination[0] + g_ip_compatible_mc.unknown_da_profile_destination;

    /*
     * Replication configuration for unknown multicast
     */
    flags = ((is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);

    rv = bcm_multicast_create(unit, flags, &g_ip_compatible_mc.mcid[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    flags = (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_INGRESS_GROUP;

    /*
     * bcm_multicast_replication_t rep_array; bcm_multicast_replication_t_init(&rep_array);
     */

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS - 1; temp_i++)
    {

        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[1][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        rv = bcm_multicast_add(unit, g_ip_compatible_mc.mcid[2], flags, g_ip_compatible_mc.nof_replications,
                               &rep_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add in Port= %d\n", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
            return rv;
        }

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
    rv = bcm_ipmc_add(unit, &l2_ipmc_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_ipmc_add\n");
        return rv;
    }

    /*
     * Multicast Configuration for V4_SVL
     */
    flags = ((is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);

    rv = bcm_multicast_create(unit, flags, &g_ip_compatible_mc.mcid[3]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    flags = (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_INGRESS_GROUP;

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS - 1; temp_i++)
    {

        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[2][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        rv = bcm_multicast_add(unit, g_ip_compatible_mc.mcid[3], flags, g_ip_compatible_mc.nof_replications,
                               &rep_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add in Port= %d\n", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
            return rv;
        }

    }

    /*
     * Multicast Configuration for V4_SVL
     */
    flags = ((is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);

    rv = bcm_multicast_create(unit, flags, &g_ip_compatible_mc.mcid[4]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    flags = (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_INGRESS_GROUP;

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
    {

        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[3][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        rv = bcm_multicast_add(unit, g_ip_compatible_mc.mcid[4], flags, g_ip_compatible_mc.nof_replications,
                               &rep_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add in Port= %d\n", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
            return rv;
        }

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
    rv = bcm_ipmc_add(unit, &l2_ipmc_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_ipmc_add\n");
        return rv;
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
    rv = bcm_vlan_control_vlan_set(unit, g_ip_compatible_mc.vsi[2], control);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_control_vlan_set in vsi= %d\n", g_ip_compatible_mc.vsi[2]);
        return rv;
    }

    g_ip_compatible_mc.mcid[5] =
        g_ip_compatible_mc.vsi_da_not_found_destination[1] + g_ip_compatible_mc.unknown_da_profile_destination;

    /*
     * Replication configuration for unknown multicast
     */
    flags = ((is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);

    rv = bcm_multicast_create(unit, flags, &g_ip_compatible_mc.mcid[5]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    flags = (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2) | BCM_MULTICAST_INGRESS_GROUP;

    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
    {

        rep_array.port = g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]];
        rep_array.encap1 = (g_ip_compatible_mc.outLifList[2][OUT_PORT_INDEX[temp_i]] & 0x3fffff);

        rv = bcm_multicast_add(unit, g_ip_compatible_mc.mcid[5], flags, g_ip_compatible_mc.nof_replications,
                               &rep_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add in Port= %d\n", g_ip_compatible_mc.ports[OUT_PORT_INDEX[temp_i]]);
            return rv;
        }

    }

    return rv;

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
    int rv;
    int temp_i;

    printf("ip_compatible_mc__cleanup__start_run\n");

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    /*
     * Destroy OutLIFs
     */
    for (temp_i = 0; temp_i < NUM_OF_OUT_PORTS; temp_i++)
    {

        rv = bcm_vlan_port_destroy(unit, g_ip_compatible_mc.outLifList[OUT_PORT_INDEX[temp_i]]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_port_destroy(port = %d)\n",
                   g_ip_compatible_mc.outLifList[OUT_PORT_INDEX[temp_i]]);
            return rv;
        }

    }

    /*
     * Destroy InLIFs
     */

    rv = bcm_vlan_port_destroy(unit, g_ip_compatible_mc.inLifList[IN_PORT_INDEX]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_destroy(port = %d)\n", g_ip_compatible_mc.inLifList[IN_PORT_INDEX]);
        return rv;
    }

    /*
     * VSI
     */
    rv = bcm_vlan_destroy(unit, g_ip_compatible_mc.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_destroy(vsi=%d)\n", g_ip_compatible_mc.vsi);
        return rv;
    }

    /*
     * TPIDs
     */
    /*
     * In JR1, only two TPIDs per port, thus the test uses {s_tag, s_tag} for the tx dtag packet and 
     * {c_tag, c_tag} rx packet for the modified TPID values.
     */
    if (!is_jericho2)
    {
        /*
         * JER1
         */

        for (temp_i = 0; temp_i < NUM_OF_PORTS; temp_i++)
        {

            rv = bcm_port_tpid_delete_all(unit, g_ip_compatible_mc.ports[temp_i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_delete_all(port=%d)\n", g_ip_compatible_mc.ports[temp_i]);
                return rv;
            }

        }
    }
    else
    {
        /*
         * JR2
         */
        rv = tpid__tpids_clear_all(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpids_clear_all, rv = %d\n");
            return rv;
        }

    }

    /*
     * Destroy Multicast Group 
     */

    rv = bcm_multicast_destroy(unit, g_ip_compatible_mc.mcid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_destroy\n");
        return rv;
    }

    return rv;
}
