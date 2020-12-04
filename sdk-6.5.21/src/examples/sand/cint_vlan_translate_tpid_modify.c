/* $Id: cint_vlan_translate_tpid_modify.c,
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
 * This CINT demonstrates IVE (Ingress VLAN Editing) action when outer_tpid_action/inner_tpid_action is set to bcmVlanTpidActionModify.             *
 *                                                                                                                                                  *
 * The cint creates a basic L2VPN scheme with two different Logical Interfaces(LIF) that are based on <Port, VLAN, VLAN>.                           *
 * The test uses two input ports, iPort1 and iPort2 and one output port oPort.                                                                      *
 * Two "almost the same" IVE commands are set, one for packets coming from iPort1 and the other for packet coming from iPort2.                      *
 * The only difference between the two IVE commands is that for iPort1, the TPID action is NONE while for iPort2 it is MODIFY.                      *
 * The packets are forwarded to the egress using MACT with oPort and Out-LIF forwarding information.                                                *
 * EVE command is configured, with {none, none} action for keeping the IVE action unchanged.                                                        *
 * Generally, the tests "needs" four TPID values, that is {s_tag, c_tag} as dtag input and {outer_tpid, inner_tpid}                                 *
 * as the modified output dtag packet.                                                                                                              *
 * In JR1, only two TPIDs per port are avaliable, thus the test uses {s_tag, s_tag} for the tx dtag packet and                                      *
 * {c_tag, c_tag} rx packet for the modified TPID values.                                                                                           *
 * The test procedure contains eight different scenarios, each scenario differs in the "number of tags to be remove" and                            *
 * "number of tags to be constructed".                                                                                                              *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                          tpid action                                                                                                             *
 *             +-------+       none                                                                                                                 *
 *    iPort1   |       |      +-----+      +------+                                                                                                 *
 *    +------> | LIF 1 +----> |IVE 1+----> |      |                                                                                                 *
 *             |       |      +-----+      |      |     +-------+   oPort                                                                           *
 *             +-------+                   |      |     |       |   Out-LIF                                                                         *
 *                          tpid action    | VSI  +---> | MACT  +-------------+                                                                     *
 *             +-------+      modify       |      |     |       |             |                                                                     *
 *    iPort2   |       |      +-----+      |      |     +-------+             |                                                                     *
 *    +------> | LIF 2 +----> |IVE 2+----> |      |                           |                                                                     *
 *             |       |      +-----+      +------+                           |                                                                     *
 *             +-------+                                                      |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                  +---------+                               |                                                                     *
 *     oPort            +------+    |         |                               |                                                                     *
 *    <-----------------+ EVE  | <--+ Out LIF +<------------------------------+                                                                     *
 *                      +------+    |         |                                                                                                     *
 *                     {None,None}  +---------+                                                                                                     *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/sand                                                                                                                 *
 * cint cint_vlan_translate_tpid_modify.c                                                                                                           *
 * cint utility/cint_sand_utils_vlan_translate.c                                                                                                    *
 * cint utility/cint_sand_utils_global.c                                                                                                            *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * vlan_translate_tpid_modify_with_ports__main_config__start_run(int unit,  int in_port1, int in_port2, int out_port)                               *
 * for step 1 to 9:                                                                                                                                 *
 *      - vlan_translate_tpid_modify__step_config__start_run(int unit, int step)                                                                    *
 *      - send ETH-2 packet via iPort1                                                                                                              *
 *      - receive packet via oPort, verify its header fields                                                                                        *
 *      - send same ETH-2 packet via iPort2                                                                                                         *
 *      - receive packet via oPort, verify its header fields                                                                                        *
 * vlan_translate_tpid_modify__cleanup__start_run(int unit)                                                                                         *
 * INPUT:                                                                                                                                           *
 * Send ETH-2 packet to iPort1/iPort2:                                                                                                              *
 *      ethernet header with DA 00:00:00:00:0x33:0x44                                                                                               *
 *      Vlan tag:                                                                                                                                   *
 *          outer:                                                                                                                                  *
 *              vlan-id: 5                                                                                                                          *
 *              vlan tag type: 0x8100                                                                                                               *
 *          inner:                                                                                                                                  *
 *              vlan-id 6                                                                                                                           *
 *              vlan tag type 0x8100                                                                                                                *
 * OUTPUT:                                                                                                                                          *
 * Get packet on oPort:                                                                                                                             *
 *      ethernet header with DA 00:00:00:00:0x33:0x44                                                                                               *
 *      Vlan tag:                                                                                                                                   *
 *          outer:                                                                                                                                  *
 *              vlan-id: 5 or 10 (based on the IVE)                                                                                                 *
 *              vlan tag type: 0x8100 or 0x9100 (based on the IVE)                                                                                  *
 *          inner:                                                                                                                                  *
 *              vlan-id: 6 or 12 (based on the IVE)                                                                                                 *
 *              vlan tag type 0x8100 or 9100 (based on the IVE)                                                                                     *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_vlan_translate_tpid_modify', before calling                *
 * the main  function                                                                                                                               *
 * In order to re-write only part of values, call 'vlan_translate_tpid_modify_struct_get(vlan_translate_tpid_modify_s)' function and                *
 * re-write values, prior calling the main function                                                                                                 *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int IN_PORT1_INDEX = 0;
int IN_PORT2_INDEX = 1;
int OUT_PORT_INDEX = 2;
int NUM_OF_PORTS = 3;
int NUM_OF_IN_PORTS = 2;
int NUM_OF_TEST_STEPS = 9;

struct global_tpids_s
{
    uint16 s_tag;               /* ingress packet s_tag (outer tag) */
    uint16 c_tag;               /* ingress packet c_tag (inner tag) */
    uint16 outer_tpid;          /* egress packet outer tag */
    uint16 inner_tpid;          /* egress packet inner tag */
};

struct ive_action_s
{
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    bcm_vlan_tpid_action_t outer_tpid_action;
    bcm_vlan_tpid_action_t inner_tpid_action;
};

/*  Main Struct  */
struct vlan_translate_tpid_modify_s
{
    bcm_gport_t ports[NUM_OF_PORTS];
    bcm_vlan_t vsi;
    global_tpids_s tpid_value;
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    bcm_gport_t inLifList[NUM_OF_PORTS];
    bcm_gport_t outLifList[NUM_OF_PORTS];
    uint32 vlan_edit_profile[NUM_OF_PORTS];
    bcm_port_tag_format_class_t tag_format[NUM_OF_PORTS];
    ive_action_s action_value[NUM_OF_TEST_STEPS][NUM_OF_IN_PORTS];      /* 8 - number of different scenarios, 2 - one
                                                                         * for bcmVlanTpidActionNone and one for
                                                                         * bcmVlanTpidActionModify */
    bcm_mac_t d_mac;
};

/* Initialization of global struct*/
vlan_translate_tpid_modify_s g_vlan_translate_tpid_modify = {
/*  ports:*/
    {0, 0, 0},
/*  vsi:*/
    77,
/*  tpid_value:
     |s_tag  | c_tag | outer | inner |*/
    {0x9100, 0x9100, 0x8100, 0x8100},
/*  outer_vid:*/
    5,
/*  inner_vid: */
    6,
/*  inLifList: */
    {0, 0},
/*  outLifList: */
    {0, 0},
/*  vlan_edit_profile:*/
    {4, 5, 6},
/*  tag_format:*/
    {9, 10, 11},

    /*
     * action_value:
     */
    {

/*
    ##
    # Test #1:
    # Define IVE command:
    #   {delete, none}:
    #       input: {X1, X2}
    #       output: {X2}
    #
    # First command with outer_tpid_action = bcmVlanTpidActionNone
    # Second command with outer_tpid_action = bcmVlanTpidActionModify
    #
    action_value[0]:*/
     {{bcmVlanActionDelete, bcmVlanActionNone, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionDelete, bcmVlanActionNone, bcmVlanTpidActionModify, bcmVlanTpidActionNone}},
/*
    ##
    # Test #2:
    # Set IVE command:
    #   {none, delete}:
    #       input: {X1, X2}
    #       output: {X1}
    #
    # First command with outer_tpid_action = bcmVlanTpidActionNone
    # Second command with outer_tpid_action = bcmVlanTpidActionModify
    #
    action_value[1]:*/
     {{bcmVlanActionNone, bcmVlanActionDelete, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionNone, bcmVlanActionDelete, bcmVlanTpidActionModify, bcmVlanTpidActionNone}},
/*
    ##
    # Test #3:
    # Set IVE command:
    #   {none, none}:
    #       input: {X1, X2}
    #       output: {X1, X2}
    #
    # First command with outer_tpid_action = bcmVlanTpidActionNone
    # Second command with outer_tpid_action = bcmVlanTpidActionModify
    #
    action_value[2]:*/
     {{bcmVlanActionNone, bcmVlanActionNone, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionNone, bcmVlanActionNone, bcmVlanTpidActionModify, bcmVlanTpidActionNone}},
/*

    ##
    # Test #4:
    # Set IVE command:
    #   {none, none}:
    #       input: {X1, X2}
    #       output: {X1, X2}
    #
    # First command with both outer/inner_tpid_action = bcmVlanTpidActionNone
    # Second command with both outer/innter_tpid_action = bcmVlanTpidActionModify
    #
    action_value[3]:*/
     {{bcmVlanActionNone, bcmVlanActionNone, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionNone, bcmVlanActionNone, bcmVlanTpidActionModify, bcmVlanTpidActionModify}},
/*
    ##
    # Test #5:
    # Set IVE command:
    #   {replace, none}:
    #       input: {X1, X2}
    #       output: {Xnew1, X2}
    #
    # First command with inner_tpid_action = bcmVlanTpidActionNone
    # Second command with innter_tpid_action = bcmVlanTpidActionModify
    #
    action_value[4]:*/
     {{bcmVlanActionReplace, bcmVlanActionNone, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionReplace, bcmVlanActionNone, bcmVlanTpidActionNone, bcmVlanTpidActionModify}},
/*
    ##
    # Test #6:
    # Set IVE command:
    #   {replace, add}:
    #       input: {X1, X2}
    #       output: {Xnew1,Xnew2, X2}
    #
    # First command with outer_tpid_action = bcmVlanTpidActionNone
    # Second command with outter_tpid_action = bcmVlanTpidActionModify
    #
    action_value[5]:*/
     {{bcmVlanActionReplace, bcmVlanActionAdd, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionReplace, bcmVlanActionAdd, bcmVlanTpidActionModify, bcmVlanTpidActionNone}},
/*
    ##
    # Test #7:
    # Set IVE command:
    #   {none, add}:
    #       input: {X1, X2}
    #       output: {Xnew1,X1, X2}
    #
    # First command with inner_tpid_action = bcmVlanTpidActionNone
    # Second command with inner_tpid_action = bcmVlanTpidActionModify
    #
    action_value[6]:*/
     {{bcmVlanActionNone, bcmVlanActionAdd, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionNone, bcmVlanActionAdd, bcmVlanTpidActionNone, bcmVlanTpidActionModify}},
/*
    ##
    # Test #8:
    # Set IVE command:
    #   {add, add}:
    #       input: {X1,X2}
    #       output: {Xnew1,Xnew2,X1,X2}
    #
    # First command with outer_tpid_action = bcmVlanTpidActionNone
    # Second command with outer_tpid_action = bcmVlanTpidActionModify
    #
     action_value[7]:*/
     {{bcmVlanActionAdd, bcmVlanActionAdd, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionAdd, bcmVlanActionAdd, bcmVlanTpidActionModify, bcmVlanTpidActionNone}},
/*
    ##
    # Test #9:
    # Set IVE command:
    #   {none, none}:
    #       input: {X1, X2}
    #       output: {X1, X2}
    #
    # First command with both outer/inner_tpid_action = bcmVlanTpidActionNone
    # Second command with inner_tpid_action = bcmVlanTpidActionModify
    #
    action_value[8]:*/
     {{bcmVlanActionNone, bcmVlanActionNone, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionNone, bcmVlanActionNone, bcmVlanTpidActionNone, bcmVlanTpidActionModify}}
     },

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
 *   param: new values for g_vlan_translate_tpid_modify
 */
int
vlan_translate_tpid_modify_init(
    int unit,
    vlan_translate_tpid_modify_s * param)
{
    int rv;

    if (param != NULL)
    {
        sal_memcpy(&g_vlan_translate_tpid_modify, param, sizeof(g_vlan_translate_tpid_modify));
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
    else
    {
       int i;
       /*
       * In JR2 - need to "create" the TAG-Struct before it can be used.
       */
       for (i=0; i<NUM_OF_PORTS; i++) {
          rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeSCTag, &g_vlan_translate_tpid_modify.tag_format[i]);

          if (rv != BCM_E_NONE)
          {
              printf("Error, bcm_port_tpid_class_create(tag_format_class_id = %d\n", g_vlan_translate_tpid_modify.tag_format[i]);
              return rv;
          }
       }
    }

    return BCM_E_NONE;
}

/*
 * Return vlan_translate_tpid_modify information
 */
void
vlan_translate_tpid_modify_struct_get(
    int unit,
    vlan_translate_tpid_modify_s * param)
{

    sal_memcpy(param, &g_vlan_translate_tpid_modify, sizeof(g_vlan_translate_tpid_modify));

    return;
}

/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit     - unit
 *        iPort1 - ingress port
 *        iPort2 - ingress port
 *        oPort - egress port
 */
int
vlan_translate_tpid_modify_with_ports__main_config__start_run(
    int unit,
    int iPort1,
    int iPort2,
    int oPort)
{

    vlan_translate_tpid_modify_s param;

    vlan_translate_tpid_modify_struct_get(unit, &param);

    param.ports[IN_PORT1_INDEX] = iPort1;
    param.ports[IN_PORT2_INDEX] = iPort2;
    param.ports[OUT_PORT_INDEX] = oPort;

    return vlan_translate_tpid_modify__main_config__start_run(unit, &param);
}

/*
 * Main function runs the vlan translate tpid modify example
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Setting VLAN domain for ingress/egress ports.
 *    3. Setting relevant TPID's for ingress/egress ports.
 *    4. Creating a VSI and set VLAN port membership (setting VLAN port membership is not mandatory, it is optional).
 *    5. Creating symmetric LIFs for ingress/egress ports.
 *    6. Setting EVE action to {None, None}.
 *    7. Setting Ingress/Egress LLVP.
 *    8. Setting MACT DA with egress port and LIF information.
 *
 * INPUT: unit  - unit
 *        param - new values for explicit_rif(in case it's NULL default values will be used).
 */
int
vlan_translate_tpid_modify__main_config__start_run(
    int unit,
    vlan_translate_tpid_modify_s * param)
{
    int rv;
    int i;

    rv = vlan_translate_tpid_modify_init(unit, param);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_tpid_modify_init\n");
        return rv;
    }

    /*
     * Set vlan domain:
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {

        rv = bcm_port_class_set(unit, g_vlan_translate_tpid_modify.ports[i], bcmPortClassId,
                                g_vlan_translate_tpid_modify.ports[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", g_vlan_translate_tpid_modify.ports[i]);
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

        for (i = 0; i < NUM_OF_PORTS; i++)
        {

            rv = bcm_port_tpid_delete_all(unit, g_vlan_translate_tpid_modify.ports[i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_delete_all(port=%d)\n", g_vlan_translate_tpid_modify.ports[i]);
                return rv;
            }

            rv = bcm_port_tpid_add(unit, g_vlan_translate_tpid_modify.ports[i],
                                   g_vlan_translate_tpid_modify.tpid_value.s_tag, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_add(port=%d, tpid=0x%04x)\n", g_vlan_translate_tpid_modify.ports[i],
                       g_vlan_translate_tpid_modify.tpid_value.s_tag);
                return rv;
            }

            rv = bcm_port_tpid_add(unit, g_vlan_translate_tpid_modify.ports[i],
                                   g_vlan_translate_tpid_modify.tpid_value.outer_tpid, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_add(port=%d, tpid=0x%04x)\n", g_vlan_translate_tpid_modify.ports[i],
                       g_vlan_translate_tpid_modify.tpid_value.outer_tpid);
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
            printf("Error, in tpid__tpids_clear_all, rv = %d\n", rv);
            return rv;
        }

        rv = tpid__tpid_add(unit, g_vlan_translate_tpid_modify.tpid_value.s_tag);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_vlan_translate_tpid_modify.tpid_value.s_tag, rv);
            return rv;
        }

        rv = tpid__tpid_add(unit, g_vlan_translate_tpid_modify.tpid_value.outer_tpid);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in tpid__tpid_add(tpid_value=0x%04x, rv = %d)\n", g_vlan_translate_tpid_modify.tpid_value.outer_tpid, rv);
            return rv;
        }
    }

    /*
     * Create VSI:
     */
    rv = bcm_vlan_create(unit, g_vlan_translate_tpid_modify.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_create(vis=%d)\n", g_vlan_translate_tpid_modify.vsi);
        return rv;
    }

    /*
     * Set VLAN port membership
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, g_vlan_translate_tpid_modify.ports[i]);
    }

    rv = bcm_vlan_port_add(unit, g_vlan_translate_tpid_modify.vsi, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, g_vlan_translate_tpid_modify.outer_vid, g_vlan_translate_tpid_modify.ports[IN_PORT1_INDEX], 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, g_vlan_translate_tpid_modify.outer_vid, g_vlan_translate_tpid_modify.ports[IN_PORT2_INDEX], 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    /*
     * Creates symmetric LIF (port x VLAN x VLAN)
     */

    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.flags = 0;        /* symmetric LIF */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.match_vlan = g_vlan_translate_tpid_modify.outer_vid;
    vlan_port.match_inner_vlan = g_vlan_translate_tpid_modify.inner_vid;

    for (i = 0; i < NUM_OF_PORTS; i++)
    {

        vlan_port.port = g_vlan_translate_tpid_modify.ports[i];
        vlan_port.vsi = 0;

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
            return rv;
        }

        g_vlan_translate_tpid_modify.inLifList[i] = vlan_port.vlan_port_id;
        g_vlan_translate_tpid_modify.outLifList[i] = vlan_port.encap_id;

        printf("port=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.vlan_port_id,
               vlan_port.encap_id);

        rv = bcm_vswitch_port_add(unit, g_vlan_translate_tpid_modify.vsi, vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }

    /*
     * Since testing is performed using IVE only, first define EVE command as:
     * {none, none}
     */

    rv = vlan_translate_ive_eve_translation_set(unit, g_vlan_translate_tpid_modify.inLifList[OUT_PORT_INDEX],  /* lif - need the gport, thus use inLifList instead of outLifList */
                                                g_vlan_translate_tpid_modify.tpid_value.outer_tpid,            /* outer_tpid */
                                                g_vlan_translate_tpid_modify.tpid_value.inner_tpid,            /* inner_tpid */
                                                bcmVlanActionNone,                                             /* outer_action */
                                                bcmVlanActionNone,                                             /* inner_action */
                                                g_vlan_translate_tpid_modify.outer_vid * 2,                    /* new_outer_vid, Note: the *2 is to differ the original VID in the packet */
                                                g_vlan_translate_tpid_modify.inner_vid * 2,                    /* new_inner_vid, Note: the *2 is to differ the original VID in the packet */
                                                g_vlan_translate_tpid_modify.vlan_edit_profile[OUT_PORT_INDEX],/* vlan_edit_profile */
                                                g_vlan_translate_tpid_modify.tag_format[OUT_PORT_INDEX],       /* tag_format */
                                                FALSE                                                          /* is_ive */
        );

    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting EVE to {none,none}\n");
        return rv;
    }

    /*
     * Update LLVP
     */
    bcm_port_tpid_class_t port_tpid_class;

    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;

    for (i = 0; i < NUM_OF_IN_PORTS; i++)
    {
        /*
         * Ingress LLVP
         */
        port_tpid_class.port = g_vlan_translate_tpid_modify.ports[i];
        port_tpid_class.tag_format_class_id = g_vlan_translate_tpid_modify.tag_format[i];

        printf("bcm_port_tpid_class_set(port=%d, tag_format_class_id=%d)\n", port_tpid_class.port,
               port_tpid_class.tag_format_class_id);

        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_tpid_class_set( iPort$d )\n", i);
            return rv;
        }
    }

    /*
     * Egress LLVP - for oPort:
     */
    port_tpid_class.port = g_vlan_translate_tpid_modify.ports[OUT_PORT_INDEX];
    port_tpid_class.tag_format_class_id = g_vlan_translate_tpid_modify.tag_format[OUT_PORT_INDEX];
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;

    printf("bcm_port_tpid_class_set(port=%d, tag_format_class_id=%d)\n", port_tpid_class.port,
           port_tpid_class.tag_format_class_id);

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set( oPort )\n");
        return rv;
    }

    /*
     * Add l2 address -
     * adding oPort and outLif to MACT
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_vlan_translate_tpid_modify.d_mac, g_vlan_translate_tpid_modify.vsi);

    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_vlan_translate_tpid_modify.ports[OUT_PORT_INDEX];
    l2_addr.encap_id = g_vlan_translate_tpid_modify.outLifList[OUT_PORT_INDEX];

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

/*
 * Step function runs the vlan translate tpid modify example
 *
 * Main steps of configuration:
 *    1. Configures IVE1 and IVE2 actions
 *
 * INPUT: unit - unit
 *        step - which step in the configuration to set [1:8].
 */
int
vlan_translate_tpid_modify__step_config__start_run(
    int unit,
    int step)
{
    int rv = BCM_E_PARAM;
    int i;

    if ((step < 1) || (step > NUM_OF_TEST_STEPS))
    {
        printf("Error, step = %d!!! Should be in range [1:%d]\n", step, NUM_OF_TEST_STEPS);
        return rv;
    }

    printf("vlan_translate_tpid_modify__step_config__start_run( step = %d )\n", step);

    /*
     * Configure IVE actions
     */
    for (i = 0; i < NUM_OF_IN_PORTS; i++)
    {
        rv = vlan_translate_ive_eve_translation_set_with_tpid_action(unit, g_vlan_translate_tpid_modify.inLifList[i],   /* lif
                                                                                                                         */
                                                                     g_vlan_translate_tpid_modify.tpid_value.outer_tpid,        /* outer_tpid
                                                                                                                                 */
                                                                     g_vlan_translate_tpid_modify.tpid_value.inner_tpid,        /* inner_tpid
                                                                                                                                 */
                                                                     g_vlan_translate_tpid_modify.action_value[step - 1][i].outer_tpid_action,  /* outer_tpid_action
                                                                                                                                                 */
                                                                     g_vlan_translate_tpid_modify.action_value[step - 1][i].inner_tpid_action,  /* inner_tpid_action
                                                                                                                                                 */
                                                                     g_vlan_translate_tpid_modify.action_value[step - 1][i].outer_action,       /* outer_action
                                                                                                                                                 */
                                                                     g_vlan_translate_tpid_modify.action_value[step - 1][i].inner_action,       /* inner_action
                                                                                                                                                 */
                                                                     g_vlan_translate_tpid_modify.outer_vid * 2,        /* new_outer_vid,
                                                                                                                         * Note:
                                                                                                                         * the
                                                                                                                         * *2
                                                                                                                         * is
                                                                                                                         * to
                                                                                                                         * differ
                                                                                                                         * the
                                                                                                                         * original
                                                                                                                         * VID
                                                                                                                         * in
                                                                                                                         * the
                                                                                                                         * packet
                                                                                                                         */
                                                                     g_vlan_translate_tpid_modify.inner_vid * 2,        /* new_inner_vid,
                                                                                                                         * Note:
                                                                                                                         * the
                                                                                                                         * *2
                                                                                                                         * is
                                                                                                                         * to
                                                                                                                         * differ
                                                                                                                         * the
                                                                                                                         * original
                                                                                                                         * VID
                                                                                                                         * in
                                                                                                                         * the
                                                                                                                         * packet
                                                                                                                         */
                                                                     g_vlan_translate_tpid_modify.vlan_edit_profile[i], /* vlan_edit_profile
                                                                                                                         */
                                                                     g_vlan_translate_tpid_modify.tag_format[i],        /* tag_format
                                                                                                                         */
                                                                     TRUE       /* is_ive */
            );

        if (rv != BCM_E_NONE)
        {
            printf
                ("Error, in vlan_translate_ive_eve_translation_set_with_tpid_action(setting iPort%d IVE action, step=%d\n",
                 i, step);
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
 *    4. Destroy IVE and EVE actions.
 *
 * INPUT: unit - unit
 */
int
vlan_translate_tpid_modify__cleanup__start_run(
    int unit)
{
    int rv;
    int i;

    printf("vlan_translate_tpid_modify__cleanup__start_run\n");

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    /*
     * IVE and EVE actions:
     */
    rv = bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_INGRESS);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_translate_action_id_destroy_all(BCM_VLAN_ACTION_SET_INGRESS)\n");
        return rv;
    }

    rv = bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_EGRESS);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_translate_action_id_destroy_all(BCM_VLAN_ACTION_SET_INGRESS)\n");
        return rv;
    }

    /*
     * symmetric LIF
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
       rv = bcm_vlan_port_destroy(unit, g_vlan_translate_tpid_modify.inLifList[i]);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_destroy(port = %d)\n", g_vlan_translate_tpid_modify.inLifList[i]);
          return rv;
       }
    }
    
    /*
     * VSI
     */
    rv = bcm_vlan_destroy(unit, g_vlan_translate_tpid_modify.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_destroy(vsi=%d)\n", g_vlan_translate_tpid_modify.vsi);
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

        for (i = 0; i < NUM_OF_PORTS; i++)
        {

            rv = bcm_port_tpid_delete_all(unit, g_vlan_translate_tpid_modify.ports[i]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_delete_all(port=%d)\n", g_vlan_translate_tpid_modify.ports[i]);
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
            printf("Error, in tpid__tpids_clear_all, rv = %d\n", rv);
            return rv;
        }

    }

    return rv;
}
