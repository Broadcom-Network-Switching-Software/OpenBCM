/* $Id: cint_vswitch_cross_connect_p2p_ac_pwe.c,
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
 * This CINT demonstrates Point to Point (P2P) cross connect of AC/PWE to AC/PWE.                                                                   *
 *                                                                                                                                                  *
 * The cint creates a basic P2P scheme.                                                                                                             *
 *                                                                                                                                                  *
 * The test has six scenarios:                                                                                                                      *
 * 1. AC to AC P2P:                                                                                                                                 *
 *  The test configuration:                                                                                                                         *
 *      - two ports, Port1 and Port2, linked by PORT x VLAN to LIF-1 and LIF-2.                                                                     *
 *      - LIF-1 is AC P2P.                                                                                                                          *
 *      - LIF-2 is AC P2P.                                                                                                                          *
 *      - LIF-1 is P2P cross-connect to LIF-2 (symmetric, from LIF-1 to LIF-2 and vice versa).                                                      *
 *  The test scenario:                                                                                                                              *
 *      - Create the above configuration.                                                                                                           *
 *      - Send one ETH1 packet via port1.                                                                                                           *
 *        Expected result:                                                                                                                          *
 *      - Same ETH1 packet to be received via port2.                                                                                                *
 *      - Send one ETH1 packet via port2.                                                                                                           *
 *        Expected result:                                                                                                                          *
 *      - Same ETH1 packet to be received via port1.                                                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                      +-----------------+                                                                                         *
 *                                      |                 |                                                                                         *
 *                       port1          |   LIF-1         |                                                                                         *
 *                      +-------------> |   AC, P2P       |                                                                                         *
 *                                      |                 |                                                                                         *
 *                     <--XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                      P2P             |              X  |                                                                                         *
 *                      Cross Connect   +--------------X--+                                                                                         *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                      +--------------X--+                                                                                         *
 *                                      |              X  |                                                                                         *
 *                        port2         |  LIF-2       X  |                                                                                         *
 *                       +------------> |  AC, P2P     X  |                                                                                         *
 *                                      |              X  |                                                                                         *
 *                      <--XXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                                      |                 |                                                                                         *
 *                                      +-----------------+                                                                                         *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * 2. AC to PWE P2P.                                                                                                                                *
 *  The test configuration:                                                                                                                         *
 *      - two ports, Port1 and Port3, linked by PORT x VLAN to LIF-1 and PORT to LIF-3.                                                             *
 *      - LIF-1 is AC P2P.                                                                                                                          *
 *      - LIF-3 is PWE P2P                                                                                                                          *
 *      - LIF-1 is P2P cross-connect to LIF-3 (none symmetric, one direction, from LIF-1 to LIF-3).                                                 *
 *  The test scenario:                                                                                                                              *
 *      - Create the above configuration.                                                                                                           *
 *      - Send one ETH1 packet via port1.                                                                                                           *
 *  Expected result:                                                                                                                                *
 *      - ETH1oMPLSoMPLSoETH1 packet to be received via port3.                                                                                      *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                      +-----------------+                                                                                         *
 *                                      |                 |                                                                                         *
 *                       port1          |   LIF-1         |                                                                                         *
 *                      +-------------> |   AC, P2P       |                                                                                         *
 *                                      |                 |                                                                                         *
 *                     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                      P2P             |              X  |                                                                                         *
 *                      Cross Connect   +--------------X--+                                                                                         *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                      +--------------X--+                                                                                         *
 *                                      |              X  |                                                                                         *
 *                        port3         |  LIF-3       X  |                                                                                         *
 *                       +------------> |  PWE, P2P    X  |                                                                                         *
 *                                      |              X  |                                                                                         *
 *                      <--XXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                                      |                 |                                                                                         *
 *                                      +-----------------+                                                                                         *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * 3. PWE to AC P2P.                                                                                                                                *
 *  The test configuration:                                                                                                                         *
 *      - two ports, Port3 and Port1, linked by PORT to LIF-3 and PORT X VLAN to LIF-1.                                                             *
 *      - LIF-3 is PWE P2P                                                                                                                          *
 *      - LIF-1 is AC P2P.                                                                                                                          *
 *      - LIF-3 is P2P cross-connect to LIF-1 (none symmetric, one direction, from LIF-3 to LIF-1).                                                 *
 *  The test scenario:                                                                                                                              *
 *      - Create the above configuration.                                                                                                           *
 *      - Send one ETH1oMPLSoMPLSoETH1 packet via port3.                                                                                            *
 *  Expected result:                                                                                                                                *
 *      - ETH1 packet to be received via port1.                                                                                                     *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                      +-----------------+                                                                                         *
 *                                      |                 |                                                                                         *
 *                       port3          |   LIF-3         |                                                                                         *
 *                      +-------------> |   PWE, P2P      |                                                                                         *
 *                                      |                 |                                                                                         *
 *                     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                      P2P             |              X  |                                                                                         *
 *                      Cross Connect   +--------------X--+                                                                                         *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                      +--------------X--+                                                                                         *
 *                                      |              X  |                                                                                         *
 *                        port1         |  LIF-1       X  |                                                                                         *
 *                       +------------> |  AC, P2P     X  |                                                                                         *
 *                                      |              X  |                                                                                         *
 *                      <--XXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                                      |                 |                                                                                         *
 *                                      +-----------------+                                                                                         *
 *                                                                                                                                                  *
 * 4. PWE to PWE P2P.                                                                                                                               *
 *  The test configuration:                                                                                                                         *
 *      - two ports, Port3 and Port4, linked by PORT to LIF-3 and PORT X VLAN to LIF-1.                                                             *
 *      - LIF-3 is PWE P2P                                                                                                                          *
 *      - LIF-4 is PWE P2P.                                                                                                                         *
 *      - LIF-3 is P2P cross-connect to LIF-4 (none symmetric, one direction, from LIF-3 to LIF-4).                                                 *
 *  The test scenario:                                                                                                                              *
 *      - Create the above configuration.                                                                                                           *
 *      - Send one ETH1oMPLSoMPLSoETH1 packet via port3.                                                                                            *
 *  Expected result:                                                                                                                                *
 *      - ETH1oMPLSoMPLSoETH1 packet to be received via port4.                                                                                      *
 *      - New MPLS Tunnel is encapped (Old tunnel decapped) with MPLS, PWE label different from incoming packet.                                    *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                      +-----------------+                                                                                         *
 *                                      |                 |                                                                                         *
 *                       port3          |   LIF-3         |                                                                                         *
 *                      +-------------> |   PWE, P2P      |                                                                                         *
 *                                      |                 |                                                                                         *
 *                     XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                      P2P             |              X  |                                                                                         *
 *                      Cross Connect   +--------------X--+                                                                                         *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                      +--------------X--+                                                                                         *
 *                                      |              X  |                                                                                         *
 *                        port4         |  LIF-4       X  |                                                                                         *
 *                       +------------> |  PWE, P2P    X  |                                                                                         *
 *                                      |              X  |                                                                                         *
 *                      <--XXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                                      |                 |                                                                                         *
 *                                      +-----------------+                                                                                         *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * 5. AC to Port P2P:                                                                                                                                 *
 *  The test configuration:                                                                                                                         *
 *      - two ports, Port1 and Port2, linked by PORT x VLAN to LIF-1 and LIF-2.                                                                     *
 *      - LIF-1 is AC P2P.                                                                                                                          *
 *      - LIF-1 is P2P cross-connect to port-2 (non-symmetric, from LIF-1 to port-2).                                                               *
 *  The test scenario:                                                                                                                              *
 *      - Create the above configuration.                                                                                                           *
 *      - Send one ETH1 packet via port1.                                                                                                           *
 *        Expected result:                                                                                                                          *
 *      - Same ETH1 packet to be received via port2.                                                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                      +-----------------+                                                                                         *
 *                                      |                 |                                                                                         *
 *                       port1          |   LIF-1         |                                                                                         *
 *                      +-------------> |   AC, P2P       |                                                                                         *
 *                                      |                 |                                                                                         *
 *                        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                      P2P             |              X  |                                                                                         *
 *                      Cross Connect   +--------------X--+                                                                                         *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                      +--------------X--+                                                                                         *
 *                                      |              X  |                                                                                         *
 *                        port2         |              X  |                                                                                         *
 *                       <--------------|      P2P     X  |                                                                                         *
 *                                      |              X  |                                                                                         *
 *                      <--XXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                                      |                 |                                                                                         *
 *                                      +-----------------+                                                                                         *
 * 6. AC to Port + Encap P2P:                                                                                                                                 *
 *  The test configuration:                                                                                                                         *
 *      - two ports, Port1 and Port2, linked by PORT x VLAN to LIF-1 and LIF-2.                                                                     *
 *      - LIF-1 is AC P2P.                                                                                                                          *
 *      - LIF-2 is AC P2P.                                                                                                                          *
 *      - LIF-1 is P2P cross-connect to port2 + ENCAP-ID-2 (non-symetric, from LIF-1 to port2 + ENCAP-ID-2).                                        *
 *  The test scenario:                                                                                                                              *
 *      - Create the above configuration.                                                                                                           *
 *      - Send one ETH1 packet via port1.                                                                                                           *
 *        Expected result:                                                                                                                          *
 *      - Same ETH1 packet to be received via port2.                                                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                      +-----------------+                                                                                         *
 *                                      |                 |                                                                                         *
 *                       port1          |   LIF-1         |                                                                                         *
 *                      +-------------> |   AC, P2P       |                                                                                         *
 *                                      |                 |                                                                                         *
 *                        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                      P2P             |              X  |                                                                                         *
 *                      Cross Connect   +--------------X--+                                                                                         *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                                     X                                                                                            *
 *                                      +--------------X--+                                                                                         *
 *                                      |              X  |                                                                                         *
 *                        port2         |  LIF-2       X  |                                                                                         *
 *                       <--------------|  AC, P2P     X  |                                                                                         *
 *                                      |              X  |                                                                                         *
 *                      <--XXXXXXXXXXXXXXXXXXXXXXXXXXXXX  |                                                                                         *
 *                                      |                 |                                                                                         *
 *                                      +-----------------+                                                                                         *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cint;                                                                                                                                            *
 * cint_reset();                                                                                                                                    *
 * exit;                                                                                                                                            *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c                                                                              *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c                                                                              *
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vpws.c                                                                                *
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c                                                                                         *
 * cint ../../../../src/examples/sand/cint_vpls_mp_basic.c                                                                                          *
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c                                                                          *
 * cint ../../../../src/examples/sand/cint_vswitch_cross_connect_p2p_ac_pwe.c                                                                       *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * int rv = 0;                                                                                                                                      *
 * int port1 = 200;                                                                                                                                 *
 * int port2 = 201;                                                                                                                                 *
 * int port3 = 202;                                                                                                                                 *
 * int port4 = 203;                                                                                                                                 *
 * rv = vswitch_cross_connect_p2p_ac_pwe_with_ports__main_config__start_run(unit, port1, port2, port3, port4);                                      *
 * print rv;                                                                                                                                        *
 * To Check AC to AC:                                                                                                                               *
 *      int scenario = 1                                                                                                                            *
 *      vswitch_cross_connect_p2p_ac_pwe_scenario_config__start_run(int unit, int scenario)                                                         *
 *      Send ETH-1 packet via port1                                                                                                                 *
 *      Receive same packet via port2, verify its header fields.                                                                                    *
 *      Send ETH-1 packet via port2                                                                                                                 *
 *      Receive same packet via port1, verify its header fields.                                                                                    *
 * To Check AC to PWE:                                                                                                                              *
 *      int scenario = 2                                                                                                                            *
 *      vswitch_cross_connect_p2p_ac_pwe_scenario_config__start_run(int unit, int scenario)                                                         *
 *      Send ETH-1 packet via port1                                                                                                                 *
 *      Receive ETH1oMPLSoMPLSoETH1 packet via port3, verify its header fields.                                                                     *
 * To Check PWE to AC:                                                                                                                              *
 *      int scenario = 3                                                                                                                            *
 *      vswitch_cross_connect_p2p_ac_pwe_scenario_config__start_run(int unit, int scenario)                                                         *
 *      Send ETH1oMPLSoMPLSoETH1 packet via port3                                                                                                   *
 *      Receive ETH1 packet via port1, verify its header fields.                                                                                    *
 * To Check PWE to PWE:                                                                                                                             *
 *      int scenario = 4                                                                                                                            *
 *      vswitch_cross_connect_p2p_ac_pwe_scenario_config__start_run(int unit, int scenario)                                                         *
 *      Send ETH1oMPLSoMPLSoETH1 packet via port3                                                                                                   *
 *      Receive ETH1oMPLSoMPLSoETH1 packet via port4, verify its header fields.                                                                     *
 * To Check AC to Port:                                                                                                                             *
 *      int scenario = 5                                                                                                                            *
 *      vswitch_cross_connect_p2p_ac_pwe_scenario_config__start_run(int unit, int scenario)                                                         *
 *      Send ETH-1 packet via port1                                                                                                                 *
 *      Receive same packet via port2, verify its header fields.                                                                                    *
 * To Check AC to Port + Encap:                                                                                                                     *
 *      int scenario = 6                                                                                                                            *
 *      vswitch_cross_connect_p2p_ac_pwe_scenario_config__start_run(int unit, int scenario)                                                         *
 *      Send ETH-1 packet via port1                                                                                                                 *
 *      Receive same packet via port2, verify its header fields.                                                                                    *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_vswitch_cross_connect_p2p_ac_pwe', before calling          *
 * the main  function                                                                                                                               *
 * In order to re-write only part of values, call 'vswitch_cross_connect_p2p_ac_pwe_struct_get(vswitch_cross_connect_p2p_ac_pwe_s)' function and    *
 * re-write values, prior calling the main function                                                                                                 *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int NUM_OF_PORTS = 4;
int PORT1_INDEX = 0;
int PORT2_INDEX = 1;
int PORT3_INDEX = 2;
int PORT4_INDEX = 3;

int NUM_OF_LIFS = 4;
int LIF_1_INDEX = 0;
int LIF_2_INDEX = 1;
int LIF_3_INDEX = 2;
int LIF_4_INDEX = 3;

enum vswitch_cross_connect_p2p_ac_pwe_scenario_e
{
    AC_TO_AC = 1,
    AC_TO_PWE,
    PWE_TO_AC,
    PWE_TO_PWE,
    AC_TO_PORT,
    AC_TO_PORT_AND_ENCAP
};

/*  Main Struct  */
struct vswitch_cross_connect_p2p_ac_pwe_s
{
    bcm_gport_t ports[NUM_OF_PORTS];
    bcm_vlan_t vsi;
    bcm_vlan_t vid[NUM_OF_LIFS];
    bcm_gport_t inLifList[NUM_OF_LIFS];
    bcm_gport_t outLifList[NUM_OF_LIFS];
};

/* Initialization of global struct*/
vswitch_cross_connect_p2p_ac_pwe_s g_vswitch_cross_connect_p2p_ac_pwe = {
/*  ports:*/
    {0, 0, 0, 0},
/*  vsi:*/
    0,
/*  vid:*/
    {101, 102, 103, 104},
/*  inLifList: */
    {0, 0, 0, 0},
/*  outLifList: */
    {0, 0, 0, 0}
};

/** enable/disable wide data for IN-LIF AC */
int IN_LIF_AC_WIDE_DATA_ENABLE = 0;

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_vswitch_cross_connect_p2p_ac_pwe
 */
int
vswitch_cross_connect_p2p_ac_pwe_init(
    int unit,
    vswitch_cross_connect_p2p_ac_pwe_s * param)
{
    int rv;

    if (param != NULL)
    {
        sal_memcpy(&g_vswitch_cross_connect_p2p_ac_pwe, param, sizeof(g_vswitch_cross_connect_p2p_ac_pwe));
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
        BCM_L3_ITF_SET(cint_pwe_basic_info.mpls_encap_fec_id, BCM_L3_ITF_TYPE_FEC, 0x1000);
    }
    else
    {

        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_pwe_basic_info.mpls_encap_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }

    }

    return BCM_E_NONE;
}

/*
 * Return g_vswitch_cross_connect_p2p_ac_pwe information
 */
void
vswitch_cross_connect_p2p_ac_pwe_struct_get(
    int unit,
    vswitch_cross_connect_p2p_ac_pwe_s * param)
{

    sal_memcpy(param, &g_vswitch_cross_connect_p2p_ac_pwe, sizeof(g_vswitch_cross_connect_p2p_ac_pwe));

    return;
}

/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit     - unit
 *        port1 - port of LIF-1
 *        port2 - port of LIF-2
 *        port3 - port of LIF-3
 *        port4 - port of LIF-4
 */
int
vswitch_cross_connect_p2p_ac_pwe_with_ports__main_config__start_run(
    int unit,
    int port1,
    int port2,
    int port3,
    int port4)
{

    vswitch_cross_connect_p2p_ac_pwe_s param;

    vswitch_cross_connect_p2p_ac_pwe_struct_get(unit, &param);

    param.ports[PORT1_INDEX] = port1;
    param.ports[PORT2_INDEX] = port2;
    param.ports[PORT3_INDEX] = port3;
    param.ports[PORT4_INDEX] = port4;

    return vswitch_cross_connect_p2p_ac_pwe__main_config__start_run(unit, &param);
}

/*
 * Main function runs the P2P Cross connect test
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Creating LIF-1, LIF-2, LIF-3 and LIF-4.
 *    3. P2P cross-connect LIF-1 to LIF-X; none symmetric, one direction, from LIF-1 to LIF-X.
 *
 * INPUT: unit  - unit
 *        param - the parameters for runing the test.
 */
int
vswitch_cross_connect_p2p_ac_pwe__main_config__start_run(
    int unit,
    vswitch_cross_connect_p2p_ac_pwe_s * param)
{
    int rv;
    int i;
    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    rv = vswitch_cross_connect_p2p_ac_pwe_init(unit, param);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in vswitch_cross_connect_p2p_ac_pwe_init\n");
        return rv;
    }

    /*
     * Set vlan domain:
     */

    for (i = 0; i < NUM_OF_PORTS; i++)
    {

        rv = bcm_port_class_set(unit, g_vswitch_cross_connect_p2p_ac_pwe.ports[i], bcmPortClassId,
                                g_vswitch_cross_connect_p2p_ac_pwe.ports[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", g_vswitch_cross_connect_p2p_ac_pwe.ports[i]);
            return rv;
        }
    }

    /*
     * Creates LIF-1:
     * P2P LIF (port x VLAN)
     */
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);

    /*
     * Temporary flag till added to PAPI:
     * Creates a P2P vlan port.
     */
    vlan_port.flags = 0;
    if (is_jericho2)
    {
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    } else {
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }


    if (IN_LIF_AC_WIDE_DATA_ENABLE == TRUE)
    {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

    vlan_port.vsi = g_vswitch_cross_connect_p2p_ac_pwe.vsi;
    vlan_port.match_vlan = g_vswitch_cross_connect_p2p_ac_pwe.vid[LIF_1_INDEX];
    vlan_port.port = g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT1_INDEX];

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, g_vswitch_cross_connect_p2p_ac_pwe.vid[LIF_1_INDEX],
                            g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT1_INDEX], BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add(port = %d)\n", g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT1_INDEX]);
    }
    g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX] = vlan_port.vlan_port_id;
    g_vswitch_cross_connect_p2p_ac_pwe.outLifList[LIF_1_INDEX] = vlan_port.encap_id;

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.match_vlan,
           vlan_port.vlan_port_id, vlan_port.encap_id);

    /*
     * Creates LIF-2:
     * P2P LIF (port x VLAN)
     */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.flags = 0;
    if (is_jericho2)
    {
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    } else {
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }

    if (IN_LIF_AC_WIDE_DATA_ENABLE == TRUE)
    {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

    vlan_port.vsi = g_vswitch_cross_connect_p2p_ac_pwe.vsi;
    vlan_port.match_vlan = g_vswitch_cross_connect_p2p_ac_pwe.vid[LIF_2_INDEX];
    vlan_port.port = g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT2_INDEX];

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, g_vswitch_cross_connect_p2p_ac_pwe.vid[LIF_2_INDEX],
                                  g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT2_INDEX], BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add(port = %d)\n", g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT2_INDEX]);
        return rv;
    }

    g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_2_INDEX] = vlan_port.vlan_port_id;
    g_vswitch_cross_connect_p2p_ac_pwe.outLifList[LIF_2_INDEX] = vlan_port.encap_id;

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.match_vlan,
           vlan_port.vlan_port_id, vlan_port.encap_id);

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT1_INDEX], g_vswitch_cross_connect_p2p_ac_pwe.vid[LIF_1_INDEX]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT2_INDEX], g_vswitch_cross_connect_p2p_ac_pwe.vid[LIF_2_INDEX]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    /*
     * Creates LIF-3:
     * PWE, P2P
     */
    rv = pwe_basic_main(unit, g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT3_INDEX]);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in pwe_basic_main(port = %d)\n", g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT3_INDEX]);
        return rv;
    }
    g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_3_INDEX] = cint_pwe_basic_info.mpls_port_id_ingress;

    /*
     * Creates LIF-4:
     * PWE, P2P
     */

    /*
     * Fixing tunnel parameters for Lif-4 PWE port
     */
    /*only one global msb mymac is supported in JR1*/

    if (is_jericho2)
    {
        bcm_mac_t sa = { 0x00, 0xac, 0xbb, 0x02, 0xf5, 0x34 };
        for ( i=0; i<6; i++) {
            cint_pwe_basic_info.pwe_intf_mac_address[i] = sa[i];
        }
    } else {
        bcm_mac_t sa = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x02 };
        for ( i=0; i<6; i++) {
            cint_pwe_basic_info.pwe_intf_mac_address[i] = sa[i];
        }
    }

    bcm_mac_t da = { 0x00, 0xaf, 0x99, 0x02, 0x7e, 0x72 };
    cint_pwe_basic_info.pwe_port = g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT4_INDEX];
    cint_pwe_basic_info.pwe_intf = 104;
    cint_pwe_basic_info.mpls_port_id_ingress = 9989;
    cint_pwe_basic_info.mpls_port_id_egress = 8879;
    cint_pwe_basic_info.encap_id = 8879;
    cint_pwe_basic_info.core_arp_id = 12292;
    cint_pwe_basic_info.mpls_encap_fec_id += 6;
    cint_pwe_basic_info.pwe_arp_next_hop_mac = da;
    cint_pwe_basic_info.mpls_tunnel_id = 8391;
    cint_pwe_basic_info.mpls_tunnel_label = 2839;
    cint_pwe_basic_info.pwe_label = 7164;

    rv = pwe_basic_main(unit, g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT4_INDEX]);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in pwe_basic_main(port = %d)\n", g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT4_INDEX]);
        return rv;
    }
    g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_4_INDEX] = cint_pwe_basic_info.mpls_port_id_ingress;



    if (is_jericho2)
    {

      /*
       * Destinguish between different AC LIFs by their VLAN-Translation paramters
       */

       /* 
        * INGRESS
        */
       bcm_vlan_port_translation_t vlan_port_translation;
       bcm_vlan_port_translation_t_init(&vlan_port_translation);
       vlan_port_translation.flags = BCM_VLAN_ACTION_SET_INGRESS;
       vlan_port_translation.gport = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX];
       vlan_port_translation.new_outer_vlan = 11;
       vlan_port_translation.new_inner_vlan = 11;
       vlan_port_translation.vlan_edit_class_id = 11;
       rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_translation_set(flags = %d, gport = %d)\n", vlan_port_translation.flags, vlan_port_translation.gport);
       }

       vlan_port_translation.gport = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_2_INDEX];
       vlan_port_translation.new_outer_vlan = 12;
       vlan_port_translation.new_inner_vlan = 12;
       vlan_port_translation.vlan_edit_class_id = 12;
       rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_translation_set(flags = %d, gport = %d)\n", vlan_port_translation.flags, vlan_port_translation.gport);
       }

       /* 
        * EGRESS
        */
       vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS;
       vlan_port_translation.gport = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX];
       vlan_port_translation.new_outer_vlan = 21;
       vlan_port_translation.new_inner_vlan = 21;
       vlan_port_translation.vlan_edit_class_id = 21;
       rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_translation_set(flags = %d, gport = %d)\n", vlan_port_translation.flags, vlan_port_translation.gport);
       }

       vlan_port_translation.gport = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_2_INDEX];
       vlan_port_translation.new_outer_vlan = 22;
       vlan_port_translation.new_inner_vlan = 22;
       vlan_port_translation.vlan_edit_class_id = 22;
       rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_translation_set(flags = %d, gport = %d)\n", vlan_port_translation.flags, vlan_port_translation.gport);
       }

       vlan_port_translation.gport = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_2_INDEX];
       vlan_port_translation.new_outer_vlan = 22;
       vlan_port_translation.new_inner_vlan = 22;
       vlan_port_translation.vlan_edit_class_id = 22;
       rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_translation_set(flags = %d, gport = %d)\n", vlan_port_translation.flags, vlan_port_translation.gport);
       }

       /*
        * Egress ESEM-Default
        */
       int esem_handle;
       bcm_gport_t gport_esem_default;

       esem_handle = *(dnxc_data_get(unit,  "esem", "default_result_profile", "default_ac", NULL));

       BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(gport_esem_default, esem_handle);
       BCM_GPORT_VLAN_PORT_ID_SET(gport_esem_default, gport_esem_default);

       printf("gport_esem_default = 0x%08X\n", gport_esem_default);

       vlan_port_translation.gport = gport_esem_default;
       vlan_port_translation.new_outer_vlan = 23;
       vlan_port_translation.new_inner_vlan = 0;
       vlan_port_translation.vlan_edit_class_id = 23;
       rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_translation_set(flags = %d, gport = %d)\n", vlan_port_translation.flags, vlan_port_translation.gport);
       }
    }

    return rv;
}

/* This function runs the P2P cross connect configuration per each scenario
 *
 * INPUT: unit     - unit
 *        scenario - the scenario index:
 *        1 - AC to AC
 *        2 - AC to PWE
 *        3 - PWE to AC
 *        4 - PWE to PWE
 *        (see enum definition of vswitch_cross_connect_p2p_ac_pwe_scenario_e)
 */
int
vswitch_cross_connect_p2p_ac_pwe_scenario_config__start_run(
    int unit,
    vswitch_cross_connect_p2p_ac_pwe_scenario_e scenario)
{
    int rv;

    vswitch_cross_connect_p2p_ac_pwe_s param;

    vswitch_cross_connect_p2p_ac_pwe_struct_get(unit, &param);

    bcm_vswitch_cross_connect_t gports;

    gports.encap1 = BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2 = BCM_FORWARD_ENCAP_ID_INVALID;

    switch (scenario)
    {
        case AC_TO_AC:
            /*
             * Connect P2P: port1 <==> port2
             */
            printf("scenario = %d, connect P2P: port1 <==> port2\n", scenario);

            gports.flags = 0;
            gports.port1 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX];
            gports.port2 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_2_INDEX];

            break;

        case AC_TO_PWE:
            /*
             * Connect P2P: port1 ==> port3
             */

            printf("scenario = %d, connect P2P: port1 ==> port3\n", scenario);

            gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

            gports.port1 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX];
            gports.port2 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_3_INDEX];

            break;

        case PWE_TO_AC:
            /*
             * Connect P2P: port3 ==> port1
             */

            printf("scenario = %d, connect P2P: port3 ==> port1\n", scenario);

            gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

            gports.port1 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_3_INDEX];
            gports.port2 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX];

            break;

        case PWE_TO_PWE:
            /*
             * Connect P2P: port3 ==> port4
             */
            printf("scenario = %d, connect P2P: port3 ==> port4\n", scenario);

            gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

            gports.port1 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_3_INDEX];
            gports.port2 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_4_INDEX];

            break;

            case AC_TO_PORT:
            /*
             * Connect P2P: port1 ==> port2
             */
            printf("scenario = %d, connect P2P: port1 ==> port2\n", scenario);

            /* 
             * Cross-connect LIF-1 to port2
             * Note: Out-LIF will be resolved from ESEM lookup (ESEM or ESEM-Default)
             */
            gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
            gports.port1 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX];
            gports.port2 = g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT2_INDEX];
  
            break;

            case AC_TO_PORT_AND_ENCAP:
            /*
             * Connect P2P: port1 ==> port2
             */
            printf("scenario = %d, connect P2P: port1 ==> port2 + encap2\n", scenario);

            /* 
             * Cross-connect LIF to port2 + encap2
             */
            gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
            gports.port1 = g_vswitch_cross_connect_p2p_ac_pwe.inLifList[LIF_1_INDEX];
            gports.port2 = g_vswitch_cross_connect_p2p_ac_pwe.ports[PORT2_INDEX];
            gports.encap2 = g_vswitch_cross_connect_p2p_ac_pwe.outLifList[LIF_2_INDEX];
  
            break;

        default:

            printf("Error, scenario = %d\n", scenario);

            return BCM_E_PARAM;

            break;

    }

    printf("bcm_vswitch_cross_connect_add( gport1 = %d, gport2 = %d, encap1 = %d, encap2 = %d )\n", gports.port1,
           gports.port2, gports.encap1, gports.encap2);

    rv = bcm_vswitch_cross_connect_add(unit, &gports);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}

/*
 * Cleanup function
 *    Destroy the created LIFs.
 * INPUT: unit - unit
 */
int
vswitch_cross_connect_p2p_ac_pwe__cleanup__start_run(
    int unit)
{
    int rv;
    int i;

    printf("vswitch_cross_connect_p2p_ac_pwe__cleanup__start_run\n");

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (!is_jericho2)
    {

        for (i = 0; i < 2; i++)
        {

            rv = bcm_vlan_port_destroy(unit, g_vswitch_cross_connect_p2p_ac_pwe.inLifList[i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_vlan_port_destroy(port = %d)\n", g_vswitch_cross_connect_p2p_ac_pwe.inLifList[i]);
                return rv;
            }
        }
    }

    return rv;
}


