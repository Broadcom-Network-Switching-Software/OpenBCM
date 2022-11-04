/* $Id: cint_dnx_vswitch_split_horizon.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************                                                                                                             *                                                                                                                                               *
 * This CINT demonstrates Split Horizon trapping.                                                                                                   *
 *                                                                                                                                                  *
 * The cint creates a Simple Brigde alike scheme.                                                                                                   *
 *                                                                                                                                                  *
 * The test configuration:                                                                                                                          *
 *      - two ports, Port1 and Port2.                                                                                                               *
 *      - two symmetric LIFs, LIF-1 and LIF-2.                                                                                                      *
 *      - MACT contains port+Out-LIF information                                                                                                    *
 *                                                                                                                                                  *
 * The test has three scenarios:                                                                                                                    *
 *                                                                                                                                                  *
 * 1. Split-Horizon filter is disabled:                                                                                                             *
 *                                                                                                                                                  *
 *      - Send one ETH1 packet via port1.                                                                                                           *
 *      - Expected result:                                                                                                                          *
 *          Same ETH1 packet to be received via port2.                                                                                              *
 *      - Send one ETH1 packet via port2.                                                                                                           *
 *      - Expected result:                                                                                                                          *
 *          Same ETH1 packet to be received via port1.                                                                                              *
 *                                                                                                                                                  *
 * 2. Split-Horizon filter is enabled:                                                                                                              *
 *                                                                                                                                                  *
 *      - Set Split Horizon filter to trap packets from IN-LIF-1 to OUT-LIF-2.                                                                      *
 *      - Send one ETH1 packet via port1.                                                                                                           *
 *      - Expected result:                                                                                                                          *
 *          No packet is received via port2.                                                                                                        *
 *          Trap packet received in the CPU.                                                                                                        *
 *      - Send one ETH1 packet via port2.                                                                                                           *
 *      - Expected result:                                                                                                                          *
 *          Same ETH1 packet to be received via port1.                                                                                              *
 *                                                                                                                                                  *
 * 3. Split-Horizon filter is enabled:                                                                                                              *
 *                                                                                                                                                  *
 *      - Set Split Horizon filter to trap packets from IN-LIF-2 to OUT-LIF-1.                                                                      *
 *      - Send one ETH1 packet via port2.                                                                                                           *
 *      - Expected result:                                                                                                                          *
 *          No packet is received via port1.                                                                                                        *
 *          Trap packet received in the CPU.                                                                                                        *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                  +------------+              +-------+           +-------------+                                                                 *
 *                  |            |              |       |           |             |                                                                 *
 *     port 1/2     | IN-LIF 1/2 |              |  VSI  |           |   MACT      |                                                                 *
 *                  |            |              |       |           |             |                                                                 *
 *   +------------> |            +------------->+       |           |             |                                                                 *
 *                  |            |              |       +---------->+             |                                                                 *
 *                  |            |              |       |           |             |    port 2/1                                                     *
 *                  |            |              |       |           |             |    OUT-LIF 2/1                                                  *
 *                  +------------+              |       |           |    dmac 2/1 +-----------------------+                                         *
 *                                              |       |           |             |                       |                                         *
 *                                              |       |           |             |                       |                                         *
 *                                              +-------+           |             |                       |                                         *
 *                                                                  |             |                       |                                         *
 *                                                                  |             |                       |                                         *
 *                                                                  |             |                       |                                         *
 *                                                                  |             |                       |                                         *
 *                                                                  +-------------+                       |                                         *
 *                                                                                                        |                                         *
 *                              +     +                                                                   |                                         *
 *                    outgoing  |     | incoming                                                          |                                         *
 *                    orientaion|     | orientaion                                                        |                                         *
 *                              |     |                                                                   |                                         *
 *                           +--v-----v--+                                                                |                                         *
 *                           |           |                                                                |                                         *
 *                           |   TRAP    |            +-------------+                                     |                                         *
 *       port 2/1            |           |            |             |                                     |                                         *
 *                           |           |            | OUT-LIF 2/1 |                                     |                                         *
 *    <----------------------+           |            |             |                                     |                                         *
 *                           |           | <----------+             | <-----------------------------------+                                         *
 *                           |           |            |             |                                                                               *
 *                           |           |            |             |                                                                               *
 *                    +------+           |            +-------------+                                                                               *
 *               To   |      |           |                                                                                                          *
 *               CPU  |      +-----------+                                                                                                          *
 *                    |                                                                                                                             *
 *                    v                                                                                                                             *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * Load cints:                                                                                                                                      *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/vswitch/cint_dnx_vswitch_split_horizon.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint
  int unit = 0;
  vswitch_split_horizon_with_ports__main_config__start_run(unit,200,201);
  exit;
 * int scenario = 1                                                                                                                                 *
 *      vswitch_split_horizon_scenario_config__start_run(int unit, int scenario)                                                                    *
 *      Send ETH-1 packet via port1                                                                                                                 *
 *      Receive same packet via port2, verify its header fields.                                                                                    *
 *      Send ETH-1 packet via port2                                                                                                                 *
 *      Receive same packet via port1, verify its header fields.                                                                                    *
 * int scenario = 2                                                                                                                                 *
 *      vswitch_split_horizon_scenario_config__start_run(int unit, int scenario)                                                                    *
 *      Send ETH-1 packet via port1                                                                                                                 *
 *      No packet is received via port2.                                                                                                            *
 *      Trap packet received in the CPU.                                                                                                            *
 *      Send ETH-1 packet via port2                                                                                                                 *
 *      Receive same packet via port1, verify its header fields.                                                                                    *
 * int scenario = 3                                                                                                                                 *
 *      vswitch_split_horizon_scenario_config__start_run(int unit, int scenario)                                                                    *
 *      Send ETH-1 packet via port2                                                                                                                 *
 *      No packet is received via port1.                                                                                                            *
 *      Trap packet received in the CPU.                                                                                                            *
 * Test Scenario - end
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_vswitch_split_horizon', before calling                     *
 * the main  function                                                                                                                               *
 * In order to re-write only part of values, call 'vswitch_split_horizon_struct_get(vswitch_split_horizon_s)' function and                          *
 * re-write values, prior calling the main function                                                                                                 *
*************************************************************************************************************************************************** */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

int NUM_OF_PORTS = 2;
int PORT1_INDEX = 0;
int PORT2_INDEX = 1;

int NUM_OF_LIFS = 2;
int LIF_1_INDEX = 0;
int LIF_2_INDEX = 1;

/*  Main Struct  */
struct vswitch_split_horizon_s
{
    bcm_gport_t ports[NUM_OF_PORTS];
    bcm_vlan_t vsi;
    bcm_vlan_t vid[NUM_OF_LIFS];
    bcm_gport_t inLifList[NUM_OF_LIFS];
    bcm_switch_network_group_t ingress_network_group_id[NUM_OF_LIFS];
    bcm_gport_t outLifList[NUM_OF_LIFS];
    bcm_switch_network_group_t egress_network_group_id[NUM_OF_LIFS];
    bcm_mac_t d_mac1;
    bcm_mac_t d_mac2;
};

/* Initialization of global struct*/
vswitch_split_horizon_s g_vswitch_split_horizon = {
    /*  ports:*/
    {0, 0},
    /*  vsi:*/
    55,
    /*  vid:*/
    {101, 102},
    /*  inLifList: */
    {0, 0},
    /*  ingress_network_group_id: */
    {0, 1},
    /*  outLifList: */
    {0, 0},
    /*  egress_network_group_id: */
    {2, 3},
    /*  d_mac1*/
    {0x00, 0x00, 0x0A, 0xB8, 0x42, 0x01},
    /*  d_mac2*/
    {0x00, 0x00, 0x0A, 0xB8, 0x42, 0x02}
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_vswitch_split_horizon
 */
void
vswitch_split_horizon__init(
    vswitch_split_horizon_s * param)
{
    if (param != NULL)
    {
        sal_memcpy(&g_vswitch_split_horizon, param, sizeof(g_vswitch_split_horizon));
    }

}

/*
 * Return g_vswitch_split_horizon information
 */
void
vswitch_split_horizon__struct_get(
    vswitch_split_horizon_s * param)
{
    sal_memcpy(param, &g_vswitch_split_horizon, sizeof(g_vswitch_split_horizon));
    return;
}

/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit     - unit
 *        port1 - port of LIF-1
 *        port2 - port of LIF-2
 */
int
vswitch_split_horizon_with_ports__main_config__start_run(
    int unit,
    int port1,
    int port2)
{
    vswitch_split_horizon_s param;
    vswitch_split_horizon__struct_get(&param);
    param.ports[PORT1_INDEX] = port1;
    param.ports[PORT2_INDEX] = port2;
    return vswitch_split_horizon__main_config__start_run(unit, &param);
}

/*
 * Main function runs the Split Horizon test
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Create VSI.
 *    3. Set VLAN membership.
 *    4. Create LIF-1, LIF-2.
 *    5. Set MACT
 *
 * INPUT: unit  - unit
 *        param - the parameters for runing the test.
 */
int
vswitch_split_horizon__main_config__start_run(
    int unit,
    vswitch_split_horizon_s * param)
{
    int i;
    vswitch_split_horizon__init(param);
    char error_msg[200]={0,};

    /*
     * Set vlan domain: for value range for vlan domain should be from 0 to 511
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_vswitch_split_horizon.ports[i]);
        if (!BCM_GPORT_IS_TRUNK(g_vswitch_split_horizon.ports[i])) {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_vswitch_split_horizon.ports[i], bcmPortClassId,
                                    g_vswitch_split_horizon.ports[i]), error_msg);
        } else {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_vswitch_split_horizon.ports[i], bcmPortClassId,
                                    i + 256), error_msg);
        }
    }

    /*
     * Create VSI:
     */
    snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", g_vswitch_split_horizon.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, g_vswitch_split_horizon.vsi), error_msg);

    /*
     * Set VLAN port membership, For lag port, only support bcm_vlan_gport_add API
     */
    bcm_pbmp_t pbmp, untag_pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        if (!BCM_GPORT_IS_TRUNK(g_vswitch_split_horizon.ports[i])) {
            BCM_PBMP_PORT_ADD(pbmp, g_vswitch_split_horizon.ports[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, g_vswitch_split_horizon.vsi, pbmp, untag_pbmp), "");
        } else {
            BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, g_vswitch_split_horizon.vsi, g_vswitch_split_horizon.ports[i], 0), "");
        }
    }

    /*
     * Creates symmetric LIF:
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.vsi = 0;

    for (i = 0; i < NUM_OF_LIFS; i++)
    {
        vlan_port.port = g_vswitch_split_horizon.ports[i];
        vlan_port.ingress_network_group_id = g_vswitch_split_horizon.ingress_network_group_id[i];
        vlan_port.egress_network_group_id = g_vswitch_split_horizon.egress_network_group_id[i];
        vlan_port.match_vlan = g_vswitch_split_horizon.vid[i];
        snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

        g_vswitch_split_horizon.inLifList[i] = vlan_port.vlan_port_id;
        g_vswitch_split_horizon.outLifList[i] = vlan_port.encap_id;
        printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.match_vlan,
               vlan_port.vlan_port_id, vlan_port.encap_id);

        snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_vswitch_split_horizon.inLifList[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_vswitch_split_horizon.vsi, g_vswitch_split_horizon.inLifList[i]), error_msg);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, g_vswitch_split_horizon.vid[i], g_vswitch_split_horizon.ports[i], 0), "");
    }

    /*
     * Add l2 address -
     * adding oPort and outLif to MACT, for trunk dst, use BCM_L2_TRUNK_MEMBER flag and tgid
     */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, g_vswitch_split_horizon.d_mac1, g_vswitch_split_horizon.vsi);
    if (!BCM_GPORT_IS_TRUNK(g_vswitch_split_horizon.ports[PORT1_INDEX])) {
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = g_vswitch_split_horizon.ports[PORT1_INDEX];
        l2_addr.encap_id = g_vswitch_split_horizon.outLifList[PORT1_INDEX];
    } else {
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_TRUNK_MEMBER;
        l2_addr.tgid = g_vswitch_split_horizon.ports[PORT1_INDEX] & 0x3ffffff;
        l2_addr.encap_id = g_vswitch_split_horizon.outLifList[PORT1_INDEX];
    }
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", l2_addr.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), error_msg);

    bcm_l2_addr_t_init(&l2_addr, g_vswitch_split_horizon.d_mac2, g_vswitch_split_horizon.vsi);
    if (!BCM_GPORT_IS_TRUNK(g_vswitch_split_horizon.ports[PORT2_INDEX])) {
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = g_vswitch_split_horizon.ports[PORT2_INDEX];
        l2_addr.encap_id = g_vswitch_split_horizon.outLifList[PORT2_INDEX];
    } else {
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_TRUNK_MEMBER;
        l2_addr.tgid = g_vswitch_split_horizon.ports[PORT2_INDEX] & 0x3ffffff;
        l2_addr.encap_id = g_vswitch_split_horizon.outLifList[PORT2_INDEX];
    }
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", l2_addr.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), error_msg);

    return BCM_E_NONE;
}

/* This function runs the Split Horizon configuration per each scenario
 *
 * INPUT: unit     - unit
 *        scenario - the scenario index:
 *        1 - Split Horizon filter is disabled
 *        2 - Split Horizon filter is enabled
 */
int
vswitch_split_horizon_scenario_config__start_run(
    int unit,
    int scenario)
{
    char error_msg[200]={0,};
    vswitch_split_horizon_s param;
    vswitch_split_horizon__struct_get(&param);
    printf("#######\nscenario = %d\n#######\n", scenario);
    switch (scenario)
    {
        case 1:
            /*
             * Split horizon filter is disabled:
             */
            printf("scenario = %d, Split horizon Filter = disabled\n", scenario);

            bcm_switch_network_group_config_t config;
            int i;
            config.config_flags = 0;
            for (i = 0; i < NUM_OF_LIFS; i++)
            {
                config.dest_network_group_id = g_vswitch_split_horizon.egress_network_group_id[i];
                snprintf(error_msg, sizeof(error_msg), "(iteration = %d)", i);
                BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit, g_vswitch_split_horizon.ingress_network_group_id[i],
                                                         &config), error_msg);
            }
            break;
        case 2:
            /*
             * Split horizon filter is enabled: IN_LIF-1 --> OUT_LIF-2
             */
            printf("scenario = %d, Split horizon Filter = enabled\n", scenario);

            bcm_switch_network_group_config_t config;
            config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
            config.dest_network_group_id = g_vswitch_split_horizon.egress_network_group_id[LIF_2_INDEX];
            BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                                     g_vswitch_split_horizon.ingress_network_group_id[LIF_1_INDEX],
                                                     &config), "");
            break;
        case 3:
            /*
             * Split horizon filter is enabled: IN_LIF-2 --> OUT_LIF-1
             */
            printf("scenario = %d, Split horizon Filter = enabled\n", scenario);

            bcm_switch_network_group_config_t config;
            config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
            config.dest_network_group_id = g_vswitch_split_horizon.egress_network_group_id[LIF_1_INDEX];
            BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                                     g_vswitch_split_horizon.ingress_network_group_id[LIF_2_INDEX],
                                                     &config), "");
            break;
        default:
            printf("Error, scenario = %d\n", scenario);
            return BCM_E_PARAM;
            break;
    }

    return BCM_E_NONE;
}

/*
 * Cleanup function
 *    Destroy split horizon filter
 * INPUT: unit - unit
 */
int
vswitch_split_horizon__cleanup__start_run(
    int unit)
{

    bcm_switch_network_group_config_t config;
    config.config_flags = 0;
    config.dest_network_group_id = g_vswitch_split_horizon.egress_network_group_id[LIF_2_INDEX];
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                             g_vswitch_split_horizon.ingress_network_group_id[LIF_1_INDEX],
                                             &config), "");

    config.dest_network_group_id = g_vswitch_split_horizon.egress_network_group_id[LIF_1_INDEX];
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                             g_vswitch_split_horizon.ingress_network_group_id[LIF_2_INDEX],
                                             &config), "");


    printf("vswitch_split_horizon__cleanup__start_run\n");
    return BCM_E_NONE;
}
