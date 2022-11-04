/* $Id: cint_l2_basic_bridge_default_ingress_with_eve.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * This CINT simulates the TM ASIC bring up snake without using actual loopback. The packet is ingressed using the Default In-LIF                   *
 * per port that selects VSI=VLAN as the VSI Assignment mode. The IVE is configured to NOP and MAC table includes an entry with a                   *
 * destination to the next port. The EVE on the next port increments the VLAN by one till the initial port is reached.                              *
 * In prective, the CINT and test are defined for 3 ports, but that can be easyly modifed. For each port, the appropriate packet                    *
 * is sent and expected to egress the next port with an incremented VLAN (except for the last port the returns the VLAN to the                      *
 * original value.                                                                                                                                  * 
 * Assumption: SDK Init is run for default TPID, VLAN-Domain, LLVP, IVE and Default In-LIF                                                                                                                                                 * 
 *                                                                                                                                                  *
 * For each port:                                                                                                                                   *
 *                                                                                                                                                  *
 *             +---------+            +---------+       +------+                                                                                    *
 *    In-Port  | Default | VSI=VLAN   |         |       |      |    Out-Port                                                                        *
 *    +------> | In-LIF  |----------->| NOP IVE |------>| MACT |--------------+                                                                     *
 *             |         |            |         |       |      |              |                                                                     *
 *             +---------+            +---------+       +------+              |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                    +---------+       +--------+            |                                                                     *
 *     Out-Port                       |         |       |        |            |                                                                     *
 *    <-------------------------------| EVE:    |<------| ESEM:  |------------+                                                                     *
 *                                    | VLAN++  |       | VDxVSI |                                                                                  *
 *                                    +---------+       +--------+                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * Test Scenario - start
  cd   ../../../../src/examples/dnx                                                                                                                
  cint_l2_basic_bridge_default_ingress_with_eve.c                                                                                                  
  cint                                                                                                                                             
  l2_basic_bridge_default_ingress_with_eve_with_ports__main_config__start_run(Unit, Port1, Port2, Port3)                                           
 * Test Scenario - end
 *                                                                                                                                                  *
 * Send Packet:                                                                                                                                     *
 *             +-----------------------------------------------------+                                                                              *
 *        eth: |    DA     |     SA    |   TPID   |  VLAN  | PCP-DEI |                                                                              *
 *             |-----------------------------------------------------|                                                                              *
 *             |    d_mac  | Any s_mac |  0x9100  | 0x10+i |   any   |                                                                              *
 *             +-----------------------------------------------------+                                                                              *
 *                                                                                                                                                  *
*************************************************************************************************************************************************** */
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

int NUM_OF_PORTS = 3;


/*  Main Struct  */
struct l2_basic_bridge_default_ingress_with_eve_s
{
    bcm_gport_t ports[NUM_OF_PORTS];
    uint16 tpid_value;
    bcm_vlan_t first_outer_vid;
    bcm_gport_t outLifList[NUM_OF_PORTS];
    uint32 vlan_edit_profile;
    bcm_port_tag_format_class_t tag_format;
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    int eve_action_id;
    bcm_mac_t d_mac;
};

/* Initialization of the global struct*/
l2_basic_bridge_default_ingress_with_eve_s g_l2_basic_bridge_default_ingress_with_eve = {
    /*  ports:*/
    {200, 201, 202},
    /*  s_tag tpid_value: */
    0x9100,
    /*  first_outer_vid: */
    0x10,
    /*  outLifList: */
    {0, 0, 0},
    /*  vlan_edit_profile: */
    2,
    /*  tag_format: */
    4,
    /*  EVE action: */
    bcmVlanActionReplace, bcmVlanActionNone,
    /*  EVE Action ID */
    0,
    /*  d_mac:*/
    {0x00, 0x00, 0x00, 0x00, 0x33, 0x44},
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */


/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_l2_basic_bridge_default_ingress_with_eve
 */
int
l2_basic_bridge_default_ingress_with_eve_init(
    int unit,
    l2_basic_bridge_default_ingress_with_eve_s * param)
{

    if (param != NULL)
    {
        sal_memcpy(&g_l2_basic_bridge_default_ingress_with_eve, param, sizeof(g_l2_basic_bridge_default_ingress_with_eve));
    }

    return BCM_E_NONE;
}

/*
 * Return l2_basic_bridge_default_ingress_with_eve information
 */
void
l2_basic_bridge_default_ingress_with_eve_struct_get(
    int unit,
    l2_basic_bridge_default_ingress_with_eve_s * param)
{

    sal_memcpy(param, &g_l2_basic_bridge_default_ingress_with_eve, sizeof(g_l2_basic_bridge_default_ingress_with_eve));

    return;
}

/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit     - unit
 *        Port1 - start port and destination from end port
 *        Port2 - in and out port
 *        Port3 - end port and destination to start port
 */
int
l2_basic_bridge_default_ingress_with_eve_with_ports__main_config__start_run(
    int unit,
    int Port1,
    int Port2,
    int Port3)
{
    l2_basic_bridge_default_ingress_with_eve_s param;

    l2_basic_bridge_default_ingress_with_eve_struct_get(unit, &param);

    param.ports[0] = Port1;
    param.ports[1] = Port2;
    param.ports[2] = Port3;

    return l2_basic_bridge_default_ingress_with_eve__main_config__start_run(unit, &param);
}

/*
 * Main function runs the example of default In-LIF with EVE
 * Rely on SDK init for global TPIDs, VLAN-Domains and LLVP configuration
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Perform per port:
 *          a. Create a VSI
 *          b. Create an ESEM entry
 *          c. Set VLAN Translation parameters in the ESEM entry
 *          d. Add a MACT entry
 *    3. Set an EVE action of {Replace, None}
 *    4. Map the EVE action
 *
 * INPUT: unit  - unit
 *        param - the new values
 */
int
l2_basic_bridge_default_ingress_with_eve__main_config__start_run(
    int unit,
    l2_basic_bridge_default_ingress_with_eve_s * param)
{
    char error_msg[200]={0,};
    int i, in_vsi, out_vsi, action_id;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_translation_t vlan_port_translation;
    bcm_l2_addr_t l2_addr;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;

    /*
     * Step 1: Init
     */
    BCM_IF_ERROR_RETURN_MSG(l2_basic_bridge_default_ingress_with_eve_init(unit, param), "");

    /*
     * Step 2: Perform operatins per port
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        /*
         * Set In-VSI and Out-VSI per port
         */
        in_vsi = g_l2_basic_bridge_default_ingress_with_eve.first_outer_vid + i;
        out_vsi = g_l2_basic_bridge_default_ingress_with_eve.first_outer_vid + ((NUM_OF_PORTS + i - 1) % NUM_OF_PORTS);

        /*
         * Step 2a: Create VSI according to the expected incoming VID as VSI=VID
         */
        snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", in_vsi);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, in_vsi), error_msg);

        /*
         * Step 2b: Create an ESEM entry
         */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.vsi = out_vsi;
        vlan_port.port = g_l2_basic_bridge_default_ingress_with_eve.ports[i];
        snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);
        snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge_default_ingress_with_eve.ports[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, in_vsi, g_l2_basic_bridge_default_ingress_with_eve.ports[i], 0), error_msg);

        g_l2_basic_bridge_default_ingress_with_eve.outLifList[i] = vlan_port.vlan_port_id;

        /*
         * Step 2c: Set VLAN Translation parameters in the ESEM entry
         */
        bcm_vlan_port_translation_t_init(&vlan_port_translation);
        vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS;
        vlan_port_translation.gport = g_l2_basic_bridge_default_ingress_with_eve.outLifList[i];
        vlan_port_translation.new_outer_vlan = g_l2_basic_bridge_default_ingress_with_eve.first_outer_vid + i;
        vlan_port_translation.vlan_edit_class_id = g_l2_basic_bridge_default_ingress_with_eve.vlan_edit_profile;
        snprintf(error_msg, sizeof(error_msg), "(gport = %d)", vlan_port_translation.gport);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(unit,&vlan_port_translation), error_msg);

        /*
         * Step 2d: Add a MACT entry
         */
        bcm_l2_addr_t_init(&l2_addr, g_l2_basic_bridge_default_ingress_with_eve.d_mac, g_l2_basic_bridge_default_ingress_with_eve.first_outer_vid + i);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = g_l2_basic_bridge_default_ingress_with_eve.ports[(i + 1) % NUM_OF_PORTS];
        snprintf(error_msg, sizeof(error_msg), "(vsi = %d)", g_l2_basic_bridge_default_ingress_with_eve.first_outer_vid + i);
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), error_msg);
    }

    /*
     * Step 3: Create and Set an EVE action of {Replace, None}
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &g_l2_basic_bridge_default_ingress_with_eve.eve_action_id), "");

    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = g_l2_basic_bridge_default_ingress_with_eve.tpid_value;
    action.inner_tpid = g_l2_basic_bridge_default_ingress_with_eve.tpid_value;
    action.dt_outer = g_l2_basic_bridge_default_ingress_with_eve.outer_action;
    action.dt_inner = g_l2_basic_bridge_default_ingress_with_eve.inner_action;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS,
                                          g_l2_basic_bridge_default_ingress_with_eve.eve_action_id, &action), "");

    /*
     * Step 4: Map the EVE action
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = g_l2_basic_bridge_default_ingress_with_eve.vlan_edit_profile;
    action_class.tag_format_class_id = g_l2_basic_bridge_default_ingress_with_eve.tag_format;
    action_class.vlan_translation_action_id = g_l2_basic_bridge_default_ingress_with_eve.eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    return BCM_E_NONE;
}

/*
 * Cleanup function
 *
 * The steps of cleanup (in invers order of the configuration):
 *    1. Delete EVE actions.
 *    2. Destroy LIFs.
 *    3. Destroy VSI.
 *
 * INPUT: unit - unit
 */
int
l2_basic_bridge_default_ingress_with_eve__cleanup__start_run(int unit)
{
    int i, in_vsi;
    char error_msg[200]={0,};

    /*
     * Free the allocated EVE action ID
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS, g_l2_basic_bridge_default_ingress_with_eve.eve_action_id), "");

    /*
     * Per port: Delete the ESEM Entry and the VSI
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        /*
         * Destroy the ESEM for each Out-Port
         */
        snprintf(error_msg, sizeof(error_msg), "(Out-LIF = %d)", g_l2_basic_bridge_default_ingress_with_eve.outLifList[i]);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, g_l2_basic_bridge_default_ingress_with_eve.outLifList[i]), error_msg);

        /*
         * Destroy the VSI that corresponds to each In-Port
         */
        snprintf(error_msg, sizeof(error_msg), "(vsi=%d)", in_vsi);
        in_vsi = g_l2_basic_bridge_default_ingress_with_eve.first_outer_vid + i;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_destroy(unit, in_vsi), error_msg);
    }

    return BCM_E_NONE;
}
