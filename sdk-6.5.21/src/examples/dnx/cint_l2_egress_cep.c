/* $Id: cint_l2_egress_cep.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * The CINT demonstrates the required JR2 configuration sequence for CEP Egress lookup.                                                             *
 * The packets originate from the same In-Port according to the Default In-LIF.                                                                     *
 * At the Egress, an EVE is performed according to one of the configured ESEM lookups:                                                              *
 *   - Non CEP                                                                                                                                      *
 *   - CEP according to the Outer-VID                                                                                                               *
 *   - CEP according to the Inner-VID                                                                                                               *
 * Only the CEP Egress LLVP configuration is modified between packet sending. The tag format remains unchanged.                                     *
 *                                                                                                                                                  *
 * For each packet:                                                                                                                                 *
 *                                                                                                                                                  *
 *             +---------+            +---------+       +------+                                                                                    *
 *    In-Port  | Default | VSI=VLAN   |         |       |      |    Out-Port                                                                        *
 *    +------> | In-LIF  |----------->| NOP IVE |------>| MACT |--------------+                                                                     *
 *             |         |            |         |       |      |              |                                                                     *
 *             +---------+            +---------+       +------+              |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                              +---------+       +--------------+            |                                                                     *
 *     Out-Port                 |         |       |              |            |                                                                     *
 *    <-------------------------| EVE:    |<------|     ESEM:    |------------+                                                                     *
 *                              | VLAN++  |       | VDxVSIxC_VID |                                                                                  *
 *                              +---------+       +--------------+                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd   ../../../../src/examples/dnx                                                                                                                *
 * cint_l2_egress_cep.c                                                                                                                             *
 * cint                                                                                                                                             *
 * l2_egress_cep_with_ports__main_config__start_run (Unit, In-Port, Out-Port)                                                                       *
 *                                                                                                                                                  *
 * To modify the LLVP for the selected CEP lookup call:                                                                                             *
 * l2_egress_cep__set_cep_port_class (Unit, Out-Port, Is-Outer-CEP)                                                                                 *
 *                                                                                                                                                  *
 * Sent Packets:                                                                                                                                    *
 *             +-----------------------------------------------------------------------------------------------+                                    *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI | Inner TPID | Inner-VLAN | PCP-DEI |                                    *
 *             |-----------------------------------------------------------------------------------------------|                                    *
 *             |    d_mac  |   s_mac   |   0x9100   |     10     |   any   |   0x8100   |     20     |   any   |                                    *
 *             +-----------------------------------------------------------------------------------------------+                                    *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * Expected Packets:                                                                                                                                *
 *        Non-CEP:                                                                                                                                  *
 *             +-----------------------------------------------------------------------------------------------+                                    *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI | Inner TPID | Inner-VLAN | PCP-DEI |                                    *
 *             |-----------------------------------------------------------------------------------------------|                                    *
 *             |    d_mac  |   s_mac   |   0x9100   |     50     |   any   |   0x8100   |     20     |   any   |                                    *
 *             +-----------------------------------------------------------------------------------------------+                                    *
 *                                                                                                                                                  *
 *        CEP Lookup according to Outer-VID:                                                                                                        *
 *             +-----------------------------------------------------------------------------------------------+                                    *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI | Inner TPID | Inner-VLAN | PCP-DEI |                                    *
 *             |-----------------------------------------------------------------------------------------------|                                    *
 *             |    d_mac  |   s_mac   |   0x9100   |     60     |   any   |   0x8100   |     20     |   any   |                                    *
 *             +-----------------------------------------------------------------------------------------------+                                    *
 *                                                                                                                                                  *
 *        CEP Lookup according to Inner-VID:                                                                                                        *
 *             +-----------------------------------------------------------------------------------------------+                                    *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI | Inner TPID | Inner-VLAN | PCP-DEI |                                    *
 *             |-----------------------------------------------------------------------------------------------|                                    *
 *             |    d_mac  |   s_mac   |   0x9100   |     70     |   any   |   0x8100   |     20     |   any   |                                    *
 *             +-----------------------------------------------------------------------------------------------+                                    * 
 *                                                                                                                                                  * 
 * Test name: AT_Dnx_Cint_l2_egress_cep                                                                                                             * 
 */
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int NUM_OF_ESEM_ENTRIES = 3;


/*  Main Struct  */
struct l2_egress_cep_s
{
    bcm_gport_t in_port;
    bcm_gport_t out_port;
    uint16 outer_tpid;
    uint16 inner_tpid;
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    bcm_gport_t esem_id_list[NUM_OF_ESEM_ENTRIES];
    uint32 vlan_edit_profile[NUM_OF_ESEM_ENTRIES];
    bcm_port_tag_format_class_t tag_format;
    int eve_action_id[NUM_OF_ESEM_ENTRIES];
    bcm_vlan_action_t outer_action[NUM_OF_ESEM_ENTRIES];
    bcm_vlan_action_t inner_action[NUM_OF_ESEM_ENTRIES];
    bcm_vlan_t new_outer_vid[NUM_OF_ESEM_ENTRIES];
    bcm_mac_t d_mac;
};

/* Initialization of the global struct*/
l2_egress_cep_s g_l2_egress_cep = {
    /*  ports:*/
    200, 201,
    /*  TPIDs: */
    0x9100, 0x8100,
    /*  Incoming packet VIDs: */
    10, 20,
    /*  EsemIdList: */
    {0, 0, 0},
    /*  vlan_edit_profile per ESEM Entry: */
    {2, 3, 2},
    /*  tag_format: */
    16,
    /*  EVE Action ID per ESEM Entry */
    {5, 6, 7},
    /*  EVE outer action per Action ID: */
    { bcmVlanActionReplace, bcmVlanActionReplace, bcmVlanActionReplace},
    /*  EVE inner action per Action ID: */
    { bcmVlanActionNone, bcmVlanActionNone, bcmVlanActionNone},
    /*  EVE new outer_vid per Action ID: */
    {50, 60, 70},
    /*  d_mac:*/
    {0x00, 0x00, 0x00, 0x00, 0x33, 0x44},
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_l2_egress_cep
 */
int
l2_egress_cep_init(
    int unit,
    l2_egress_cep_s * param)
{
    int i, rv;

    if (param != NULL)
    {
        sal_memcpy(&g_l2_egress_cep, param, sizeof(g_l2_egress_cep));
    }

    return BCM_E_NONE;
}

/*
 * Returns g_l2_egress_cep information
 */
void
l2_egress_cep_struct_get(
    int unit,
    l2_egress_cep_s * param)
{

    sal_memcpy(param, &g_l2_egress_cep, sizeof(g_l2_egress_cep));

    return;
}

/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit     - unit
 *        in_port  - In-Port for all sent packets
 *        out_port - Out-Port
 */
int
l2_egress_cep_with_ports__main_config__start_run(
    int unit,
    int in_port,
    int out_port)
{
    l2_egress_cep_s param;

    l2_egress_cep_struct_get(unit, &param);

    param.in_port = in_port;
    param.out_port = out_port;

    return l2_egress_cep__main_config__start_run(unit, &param);
}

/*
 * Main function runs the example of ESEM CEP lookups
 * Rely on SDK init for global TPIDs and VLAN-Domains
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Set the MACT to point to the Out-Port
 *    3. Create a VSI according to the expected incoming VID (VSI=VID)
 *    4. Perform per ESEM entry (Non-CEP, CEP Outer-VID, CEP Inner-VID):
 *          a. Create an ESEM entry
 *          b. Set VLAN Translation parameters for the ESEM entry
 *          c. Create and Set an EVE action
 *          d. Map the EVE action
 *
 * INPUT: unit  - unit
 *        param - The required values for all the configurations
 */
int
l2_egress_cep__main_config__start_run(
    int unit,
    l2_egress_cep_s * param)
{
    int rv;
    int i, in_vsi, out_vsi, action_id;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_translation_t vlan_port_translation;
    bcm_l2_addr_t l2_addr;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_port_tpid_class_t port_tpid_class;

    /*
     * Step 1: Init
     */
    rv = l2_egress_cep_init(unit, param);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in l2_basic_bridge_default_ingress_with_eve_init\n");
        return rv;
    }

    /*
     * Step 2: Add a MACT entry to be used by all packets
     */
    bcm_l2_addr_t_init(&l2_addr, g_l2_egress_cep.d_mac, g_l2_egress_cep.outer_vid);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_l2_egress_cep.out_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l2_addr_add (vsi = %d)\n",  g_l2_egress_cep.outer_vid);
        return rv;
    }

    /*
     * Step 3: Create VSI according to the expected incoming VID (VSI=VID)
     */
    rv = bcm_vlan_create(unit, g_l2_egress_cep.outer_vid);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in bcm_vlan_create(vsi=%d)\n", g_l2_egress_cep.outer_vid);
         return rv;
    }


    rv = bcm_vlan_gport_add(unit, g_l2_egress_cep.outer_vid, g_l2_egress_cep.in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add (port = %d)\n", g_l2_egress_cep.in_port);
        return rv;
    }

    /*
     * Reset the LLVP configuration for the non-CEP ESEM lookup
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.port = g_l2_egress_cep.out_port;
    port_tpid_class.tpid1 = g_l2_egress_cep.outer_tpid;
    port_tpid_class.tpid2 = g_l2_egress_cep.inner_tpid;
    port_tpid_class.tag_format_class_id = g_l2_egress_cep.tag_format;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_tpid_class_set for Non-CEP\n");
        return rv;
    }

    /*
     * Step 4: Configure separate ESEM entries with separate VLAN Translation
     *         for each of the demonstrated scenarios:
     *         1. Non-CEP
     *         2. CEP with Outer-VID
     *         3. CEP with Inner-VID
     */
    bcm_vlan_t c_vid[NUM_OF_ESEM_ENTRIES] = {0, g_l2_egress_cep.outer_vid,  g_l2_egress_cep.inner_vid};

    for (i = 0; i < NUM_OF_ESEM_ENTRIES; i++)
    {

        /*
         * Step 4a: Create an ESEM entry
         */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
        vlan_port.vsi = g_l2_egress_cep.outer_vid;
        vlan_port.port = g_l2_egress_cep.out_port;
        vlan_port.match_vlan = c_vid[i];
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_port_create for non-CEP ESEM (match_vlan = %d)\n", vlan_port.match_vlan);
            return rv;
        }

        g_l2_egress_cep.esem_id_list[i] = vlan_port.vlan_port_id;

        /*
         * Step 4b: Set VLAN Translation parameters for the ESEM entry
         */
        bcm_vlan_port_translation_t_init(&vlan_port_translation);
        vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS;
        vlan_port_translation.gport = g_l2_egress_cep.esem_id_list[i];
        vlan_port_translation.new_outer_vlan = g_l2_egress_cep.new_outer_vid[i];
        vlan_port_translation.vlan_edit_class_id = g_l2_egress_cep.vlan_edit_profile[i];
        rv = bcm_vlan_port_translation_set(unit,&vlan_port_translation);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_port_translation_set(gport = %d)\n", vlan_port_translation.gport);
            return rv;
        }

        /*
         * Step 4c: Create and Set an EVE
         */
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &g_l2_egress_cep.eve_action_id[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_id_create for action id - %d\n", g_l2_egress_cep.eve_action_id[i]);
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        action.outer_tpid = g_l2_egress_cep.outer_tpid;
        action.inner_tpid = g_l2_egress_cep.inner_tpid;
        action.dt_outer = g_l2_egress_cep.outer_action[i];
        action.dt_inner = g_l2_egress_cep.inner_action[i];
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS,
                                              g_l2_egress_cep.eve_action_id[i], &action);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_id_set for action id - %d\n", g_l2_egress_cep.eve_action_id[0]);
            return rv;
        }

        /*
         * Step 4d: Map the EVE action
         */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = g_l2_egress_cep.vlan_edit_profile[i];
        action_class.tag_format_class_id = g_l2_egress_cep.tag_format;
        action_class.vlan_translation_action_id = g_l2_egress_cep.eve_action_id[i];
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Cleanup function
 *
 * The steps of cleanup (in invers order of the configuration):
 *    1. Destroy the VSI.
 *    2  Delete the allocated EVE actions.
 *    3. Destroy the ESEM gports.
 *
 * INPUT: unit - unit
 */
int
l2_egress_cep__cleanup__start_run(int unit)
{
    int rv, i;
    bcm_port_tpid_class_t port_tpid_class;

    /*
     * Destroy the VSI that corresponds to the Incoming Outer VID
     */
    rv = bcm_vlan_destroy(unit, g_l2_egress_cep.outer_vid);
    if (rv != BCM_E_NONE)
    {
         printf("Error, bcm_vlan_destroy (vsi = %d)\n", g_l2_egress_cep.outer_vid);
         return rv;
    }

    /*
     * Reset the LLVP configuration
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.port = g_l2_egress_cep.out_port;
    port_tpid_class.tpid1 = g_l2_egress_cep.outer_tpid;
    port_tpid_class.tpid2 = g_l2_egress_cep.inner_tpid;
    port_tpid_class.tag_format_class_id = g_l2_egress_cep.tag_format;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_tpid_class_set during cleanup\n");
        return rv;
    }

    /*
     * Per port: Delete the ESEM Entry and the VSI
     */
    for (i = 0; i < NUM_OF_ESEM_ENTRIES; i++)
    {
        /*
         * Free all the allocated EVE action IDs
         */
        rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS, g_l2_egress_cep.eve_action_id[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_id_destroy for action ID %d\n", g_l2_egress_cep.eve_action_id[i]);
            return rv;
        }

        /*
         * Destroy all the created ESEM entries
         */
        rv = bcm_vlan_port_destroy(unit, g_l2_egress_cep.esem_id_list[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_destroy (gport = %d)\n",
                   g_l2_egress_cep.outLifList[i]);
            return rv;
        }

    }

    return BCM_E_NONE;
}


/*
 * Function to set the Egress port classification to CEP for the expected TPIDs.
 * The Tag-Format isn't changed, only the CEP flags.
 * The requested CEP flag is set: Outer-VID or Inner-VID
 *
 * INPUT: unit  - unit
 *        out_port - The Out-Port for all the packets
 *        is_outer_cep - Selects the type of CEP lookup - According to
 *                        the Outer-VID or the Inner-VID
 */
int
l2_egress_cep__set_cep_port_class(
    int unit,
    int out_port,
    int is_outer_cep)
{
    int cep_flag = (is_outer_cep) ? BCM_PORT_TPID_CLASS_OUTER_C : BCM_PORT_TPID_CLASS_INNER_C;
    bcm_port_tpid_class_t port_tpid_class;
    int rv;

    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | cep_flag;
    port_tpid_class.port = out_port;
    port_tpid_class.tpid1 = g_l2_egress_cep.outer_tpid;
    port_tpid_class.tpid2 = g_l2_egress_cep.inner_tpid;
    port_tpid_class.tag_format_class_id = g_l2_egress_cep.tag_format;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error in l2_egress_cep__set_cep_port_class, is_outer_cep = %d\n", is_outer_cep);
        return rv;
    }

    return BCM_E_NONE;
}
