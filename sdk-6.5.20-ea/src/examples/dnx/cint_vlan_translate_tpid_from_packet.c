/* $Id: cint_vlan_translate_tpid_from_packet.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * This CINT demonstrates EVE (Egress VLAN Editing) action when outer_tpid_action/inner_tpid_action is set to                                       *
 * bcmVlanTpidActionOuter/bcmVlanTpidActionInner.                                                                                                   *
 *                                                                                                                                                  *
 * The cint creates a basic L2VPN scheme with three different Logical Interfaces(LIF).                                                              *
 * The test uses three input ports, iPort1, iPort2 and iPort3 and three output ports oPort1, oPort2 and oPort3.                                     *
 * Three EVE commands are set, one for packets coming from iPort1 to oPort1, second for packets coming from iPort2 to oPort2                        *
 * and the third for packets coming from iPort3 to oPort3.                                                                                           * 
 * The three EVE commands are the same but for the first one the TPID action is NONE, for the second the TPID action is "use packet's outer TPID"   *
 * and for the third one the TPID action is "use packet's inner tpid".                                                                              * 
 * The packets are forwarded to the egress using MACT with oPort1/2/3 and Out-LIF forwarding information.                                           *                                                                                                                                                 * 
 * The tests "needs" six TPID values, that is {packet_s_tag = 0x9100, packet_c_tag = 0x8100} as double-tag input,                                   *
 * {ingress_outer_tpid = 0x9101, ingress_inner_tpid = 0x8101} to be configured in the IVE command and                                               *
 * {egress_outer_tpid = 0x9102, egress_inner_tpid = 0x8102} to be configured in the EVE command.                                                    *
 * The test ingresses double tag packet with two TPIDs {0x9100, 0x8100} thus the packet is classified as double-tag-packet.                           *
 * IVE command for double-tag-packet is configured with {Replace, Replace} action for changing the outcoming packet's TPIDs to {0x9101, 0x8101}.    *
 * EVE command for double-tag-packet is configured with {Replace, Replace} action for changing the outcoming packet's TPIDs to {0x9102, 0x8102}.    *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *             +-----------+   Action:{Replace,Replace}                                                                                             *
 * iPort1/2/3  |           |      +-----+      +-------+                                                                                            *
 *    +------> | LIF 1/2/3 +----> |IVE  +----> |       |                                                                                            *
 *             |           |      +-----+      |       |     +-------+   oPort1/2/3                                                                 *
 *             +-----------+                   |       |     |       |   Out-LIF 1/2/3                                                              *
 *                                             | VSI   +---> | MACT  +-------------+                                                                *
 *                                             | 1/2/3 |             |             |                                                                *
 *                                             |       |     +-------+             |                                                                *
 *                                             |       |                           |                                                                *
 *                                             +-------+                           |                                                                *
 *                                                                                 |                                                                *
 *                                                                                 |                                                                *
 *                                  +----------+                                   |                                                                *
 *     oPort1/2/3       +------+    |          |                                   |                                                                *
 *    <-----------------+ EVE  | <--+ Out LIF  +<----------------------------------+                                                                *
 *                      +------+    |   1/2/3  |                                                                                                    *
 *         Action:{Replace,Replace} +----------+                                                                                                    *
 *         TPID Action: None / "use packet's outer TPID" / "use packet's inner TPID"                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/sand                                                                                                                 *
 * cint cint_vlan_translate_tpid_from_packet.c                                                                                                      *
 * cint utility/cint_sand_utils_vlan_translate.c                                                                                                    *
 * cint utility/cint_sand_utils_global.c                                                                                                            *
 * cint utility/cint_sand_utils_tpid.c                                                                                                              *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * vlan_translate_tpid_from_packet_with_ports__main_config__start_run                                                                               *
 * (int unit,  int in_port1, int in_port2, int in_port3, int out_port1, int out_port2, int out_port3)                                               *
 *      - send ETH-2 packet via iPort1                                                                                                              *
 *      - receive packet via oPort1, verify its header fields                                                                                       *
 *      - send same ETH-2 packet via iPort2                                                                                                         *
 *      - receive packet via oPort2, verify its header fields                                                                                       *
 *      - send same ETH-2 packet via iPort3                                                                                                         *
 *      - receive packet via oPort3, verify its header fields                                                                                       *
 * vlan_translate_tpid_from_packet__cleanup__start_run(int unit)                                                                                    *
 * INPUT:                                                                                                                                           *
 * Send ETH-2 packet to iPort1/2/3:                                                                                                                 *
 *      ethernet header with DA 00:00:00:00:0x33:0x44                                                                                               *
 *      Vlan tag:                                                                                                                                   *
 *          outer:                                                                                                                                  *
 *              vlan-id: 0x55                                                                                                                       *
 *              vlan tag type: 0x9100                                                                                                               *
 *          inner:                                                                                                                                  *
 *              vlan-id 0x66                                                                                                                        *
 *              vlan tag type 0x8100                                                                                                                *
 * OUTPUT:                                                                                                                                          *
 * Get packet on oPort1/2/3:                                                                                                                        *
 *      ethernet header with DA 00:00:00:00:0x33:0x44                                                                                               *
 *      Vlan tag:                                                                                                                                   *
 *          outer:                                                                                                                                  *
 *              vlan-id: 0x77                                                                                                                       *
 *              TPID: 0x9100/0x9101/0x8101                                                                                                          *
 *          inner:                                                                                                                                  *
 *              vlan-id: 0x88                                                                                                                       *
 *              TPID: 0x8100/0x9101/0x8101                                                                                                          *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_vlan_translate_tpid_from_packet', before calling           *
 * the main  function                                                                                                                               *
 * In order to re-write only part of values, call 'vlan_translate_tpid_from_packet_struct_get(vlan_translate_tpid_from_packet_s)' function and      *
 * re-write values, prior calling the main function                                                                                                 *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int IN_PORT1_INDEX = 0;
int IN_PORT2_INDEX = 1;
int IN_PORT3_INDEX = 2;

int OUT_PORT1_INDEX = 0;
int OUT_PORT2_INDEX = 1;
int OUT_PORT3_INDEX = 2;

int NUM_OF_PORTS = 3;

struct global_tpids_s
{
    uint16 packet_s_tag;         /* ingress packet service tag (outer tag) */
    uint16 packet_c_tag;         /* ingress packet customer tag (inner tag) */
    uint16 ingress_outer_tpid;   /* ingress packet outer tag */
    uint16 ingress_inner_tpid ;  /* ingress packet inner tag */
    uint16 egress_outer_tpid;    /* egress packet outer tag */
    uint16 egress_inner_tpid ;   /* egress packet inner tag */
};

struct eve_action_s
{
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    bcm_vlan_tpid_action_t outer_tpid_action;
    bcm_vlan_tpid_action_t inner_tpid_action;
};

/*  Main Struct  */
struct vlan_translate_tpid_from_packet_s
{
    bcm_gport_t in_ports[NUM_OF_PORTS];
    bcm_gport_t out_ports[NUM_OF_PORTS];
    bcm_vlan_t vsi[NUM_OF_PORTS];
    global_tpids_s tpid_value;
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    bcm_vlan_t new_outer_vid;
    bcm_vlan_t new_inner_vid;
    bcm_gport_t inLifList[NUM_OF_PORTS];
    bcm_gport_t outLifList[NUM_OF_PORTS];
    uint32 vlan_edit_profile[NUM_OF_PORTS];
    bcm_port_tag_format_class_t tag_format[NUM_OF_PORTS];
    eve_action_s action_value[NUM_OF_PORTS];
    bcm_mac_t d_mac;
};

/* Initialization of global struct*/
vlan_translate_tpid_from_packet_s g_vlan_translate_tpid_from_packet = {
/*  in_ports:*/
    {0, 0, 0},
/*  out_ports:*/
    {0, 0, 0},
    /* vsi:*/
    {101, 102, 103},
/*  tpid_values: */
/*   packet_s_tag | packet_c_tag */
    {0x9100, 0x8100, 
/*   ingress_outer_tpid | ingress_inner_tpid */
       0x9101, 0x8101,
/*   egress_outer_tpid | egress_inner_tpid */
       0x9102, 0x8102 },
/*  outer_vid, inner_vid: */
    0x55, 0x66,
/*  new_outer_vid, new_inner_vid: */
    0x77, 0x88,
/*  inLifList: */
    {0, 0, 0},
/*  outLifList: */
    {0, 0, 0},
/*  vlan_edit_profile: */
    {4, 5, 6},
/*  tag_format: */
    {9, 10, 11},
    /*
     * EVE action_value:
     */
    {
/*
    ##
    # Define EVE command:
    #   {Replace, Replace}:
    #       input: {X1, X2}
    #       output: {Xnew1, Xnew2}
    #
    # First command with outer/inner_tpid_action = bcmVlanTpidActionNone
    # Second command with outer/inner_tpid_action = bcmVlanTpidActionOuter
    # Third command with outer/inner_tpid_action = bcmVlanTpidActionInner
    #
*/
      {bcmVlanActionReplace, bcmVlanActionReplace, bcmVlanTpidActionNone, bcmVlanTpidActionNone},
      {bcmVlanActionReplace, bcmVlanActionReplace, bcmVlanTpidActionOuter, bcmVlanTpidActionOuter},
      {bcmVlanActionReplace, bcmVlanActionReplace, bcmVlanTpidActionInner, bcmVlanTpidActionInner}
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
 *   param: new values for g_vlan_translate_tpid_from_packet
 */
int
vlan_translate_tpid_from_packet_init(
    int unit,
    vlan_translate_tpid_from_packet_s * param)
{
    int i, rv;

    if (param != NULL)
    {
        sal_memcpy(&g_vlan_translate_tpid_from_packet, param, sizeof(g_vlan_translate_tpid_from_packet));
    }

    /*
    * In JR2 - need to "create" the TAG-Struct before it can be used.
    */
    for (i=0; i<NUM_OF_PORTS; i++)
    {
       rv = bcm_port_tpid_class_create(unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeSCTag, &g_vlan_translate_tpid_from_packet.tag_format[i]);

       if (rv != BCM_E_NONE)
       {
           printf("Error, bcm_port_tpid_class_create(tag_format_class_id = %d\n", g_vlan_translate_tpid_from_packet.tag_format[i]);
           return rv;
       }
    }

    return BCM_E_NONE;
}

/*
 * Return vlan_translate_tpid_from_packet information
 */
void
vlan_translate_tpid_from_packet_struct_get(
    int unit,
    vlan_translate_tpid_from_packet_s * param)
{

    sal_memcpy(param, &g_vlan_translate_tpid_from_packet, sizeof(g_vlan_translate_tpid_from_packet));

    return;
}

/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit     - unit
 *        iPort1 - ingress port
 *        iPort2 - ingress port
 *        iPort3 - ingress port
 *        oPort1 - egress port
 *        oPort2 - egress port
 *        oPort3 - egress port
 */
int
vlan_translate_tpid_from_packet_with_ports__main_config__start_run(
    int unit,
    int iPort1,
    int iPort2,
    int iPort3,
    int oPort1,
    int oPort2,
    int oPort3)
{

    vlan_translate_tpid_from_packet_s param;

    vlan_translate_tpid_from_packet_struct_get(unit, &param);

    param.in_ports[IN_PORT1_INDEX] = iPort1;
    param.in_ports[IN_PORT2_INDEX] = iPort2;
    param.in_ports[IN_PORT3_INDEX] = iPort3;
    param.out_ports[OUT_PORT1_INDEX] = oPort1;
    param.out_ports[OUT_PORT2_INDEX] = oPort2;
    param.out_ports[OUT_PORT3_INDEX] = oPort3;

    return vlan_translate_tpid_from_packet__main_config__start_run(unit, &param);
}

/*
 * Main function runs the vlan translate "tpid from packet" example
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Setting VLAN domain for ingress/egress ports.
 *    3. Setting relevant TPID's for ingress/egress ports.
 *    4. Creating a VSI and set VLAN port membership (setting VLAN port membership is not mandatory, it is optional).
 *    5. Creating symmetric LIFs for ingress/egress ports.
 *    6. Setting IVE action to {None, None}.
 *    7. Setting Ingress/Egress LLVP.
 *    8. Setting EVE actions to {Replace, Replace} with TPID action None / "from outer packet" / "from inner packet".
 *    9. Setting MACT DA with egress port and LIF information.
 *
 * INPUT: unit  - unit
 *        param - the new values
 */
int
vlan_translate_tpid_from_packet__main_config__start_run(
    int unit,
    vlan_translate_tpid_from_packet_s * param)
{
    int rv;
    int i, j;

    rv = vlan_translate_tpid_from_packet_init(unit, param);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_tpid_from_packet_init\n");
        return rv;
    }

    /*
     * Set vlan domain:
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        rv = bcm_port_class_set(unit, g_vlan_translate_tpid_from_packet.in_ports[i], bcmPortClassId,
                                g_vlan_translate_tpid_from_packet.in_ports[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", g_vlan_translate_tpid_from_packet.in_ports[i]);
            return rv;
        }

        rv = bcm_port_class_set(unit, g_vlan_translate_tpid_from_packet.out_ports[i], bcmPortClassId,
                                g_vlan_translate_tpid_from_packet.out_ports[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", g_vlan_translate_tpid_from_packet.out_ports[i]);
            return rv;
        }
    }

    /*
     * Set TPIDs
     */
     rv = tpid__tpids_clear_all(unit);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in tpid__tpids_clear_all, rv = %d\n", rv);
         return rv;
     }

     rv = tpid__tpid_add(unit, g_vlan_translate_tpid_from_packet.tpid_value.packet_s_tag);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x), rv = %d\n", g_vlan_translate_tpid_from_packet.tpid_value.packet_s_tag, rv);
         return rv;
     }

     rv = tpid__tpid_add(unit, g_vlan_translate_tpid_from_packet.tpid_value.packet_c_tag);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x), rv = %d\n", g_vlan_translate_tpid_from_packet.tpid_value.packet_c_tag, rv);
         return rv;
     }

     rv = tpid__tpid_add(unit, g_vlan_translate_tpid_from_packet.tpid_value.ingress_outer_tpid);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x), rv = %d\n", g_vlan_translate_tpid_from_packet.tpid_value.ingress_outer_tpid, rv);
         return rv;
     }

     rv = tpid__tpid_add(unit, g_vlan_translate_tpid_from_packet.tpid_value.ingress_inner_tpid);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x), rv = %d\n", g_vlan_translate_tpid_from_packet.tpid_value.ingress_inner_tpid, rv);
         return rv;
     }

     rv = tpid__tpid_add(unit, g_vlan_translate_tpid_from_packet.tpid_value.egress_outer_tpid);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x), rv = %d\n", g_vlan_translate_tpid_from_packet.tpid_value.egress_outer_tpid, rv);
         return rv;
     }

     rv = tpid__tpid_add(unit, g_vlan_translate_tpid_from_packet.tpid_value.egress_inner_tpid);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_switch_tpid_add(tpid_value=0x%04x), rv = %d\n", g_vlan_translate_tpid_from_packet.tpid_value.egress_inner_tpid, rv);
         return rv;
     }

    /*
     * Create VSI:
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
       rv = bcm_vlan_create(unit, g_vlan_translate_tpid_from_packet.vsi[i]);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vlan_create(vis=%d)\n", g_vlan_translate_tpid_from_packet.vsi[i]);
           return rv;
       }

       /*
        * Set VLAN port membership
        */
       bcm_pbmp_t pbmp, untag_pbmp;

       BCM_PBMP_CLEAR(pbmp);
       BCM_PBMP_CLEAR(untag_pbmp);

       for (j = 0; j < NUM_OF_PORTS; j++)
       {
           BCM_PBMP_PORT_ADD(pbmp, g_vlan_translate_tpid_from_packet.in_ports[j]);
           BCM_PBMP_PORT_ADD(pbmp, g_vlan_translate_tpid_from_packet.out_ports[j]);
       }

       rv = bcm_vlan_port_add(unit, g_vlan_translate_tpid_from_packet.vsi[i], pbmp, untag_pbmp);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vlan_port_add\n");
           return rv;
       }
    }

    /*
     * Creates symmetric LIF (with criteria port X VLAN X VLAN) 
     * Add match port to this LIF. 
     */
    bcm_vlan_port_t vlan_port;

    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED; 
    vlan_port.match_vlan = g_vlan_translate_tpid_from_packet.outer_vid;
    vlan_port.match_inner_vlan = g_vlan_translate_tpid_from_packet.inner_vid;

    for (i = 0; i < NUM_OF_PORTS; i++)
    {
       vlan_port.port = g_vlan_translate_tpid_from_packet.in_ports[i];

       rv = bcm_vlan_port_create(unit, &vlan_port);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
          return rv;
       }

       g_vlan_translate_tpid_from_packet.inLifList[i] = vlan_port.vlan_port_id;
       g_vlan_translate_tpid_from_packet.outLifList[i] = vlan_port.encap_id;

       printf("port=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.vlan_port_id,
            vlan_port.encap_id);

       rv = bcm_vswitch_port_add(unit, g_vlan_translate_tpid_from_packet.vsi[i], vlan_port.vlan_port_id);
       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_vswitch_port_add\n");
          return rv;
       }
    }

    /*
     * Define IVE command as:
     * {Replace, Replace} 
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        rv = vlan_translate_ive_eve_translation_set(unit, g_vlan_translate_tpid_from_packet.inLifList[i],                  /* lif  */
                                                          g_vlan_translate_tpid_from_packet.tpid_value.ingress_outer_tpid, /* outer_tpid */
                                                          g_vlan_translate_tpid_from_packet.tpid_value.ingress_inner_tpid ,/* inner_tpid */
                                                          bcmVlanActionReplace,                                            /* outer_action */
                                                          bcmVlanActionReplace,                                            /* inner_action */
                                                          g_vlan_translate_tpid_from_packet.new_outer_vid,                 /* new_outer_vid */
                                                          g_vlan_translate_tpid_from_packet.new_inner_vid,                 /* new_inner_vid */
                                                          g_vlan_translate_tpid_from_packet.vlan_edit_profile[i],          /* vlan_edit_profile */
                                                          g_vlan_translate_tpid_from_packet.tag_format[i],                 /* tag_format */
                                                          TRUE                                                             /* is_ive */
                                                          );

        if (rv != BCM_E_NONE)
        {
            printf
                ("Error, in vlan_translate_ive_eve_translation_set_with_tpid_action(setting iPort%d IVE action\n", i);
            return rv;
        }
    }

    /*
     * Configure EVE actions
     */

    /*
    * Port1: with tpid_action set to "NONE"
    */
     rv = vlan_translate_ive_eve_translation_set_with_tpid_action(unit, g_vlan_translate_tpid_from_packet.inLifList[IN_PORT1_INDEX],                  /* lif  */
                                                                  g_vlan_translate_tpid_from_packet.tpid_value.egress_outer_tpid,                            /* outer_tpid */
                                                                  g_vlan_translate_tpid_from_packet.tpid_value.egress_inner_tpid ,                            /* inner_tpid */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT1_INDEX].outer_tpid_action,   /* outer_tpid_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT1_INDEX].inner_tpid_action,   /* inner_tpid_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT1_INDEX].outer_action,        /* outer_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT1_INDEX].inner_action,        /* inner_action */
                                                                  g_vlan_translate_tpid_from_packet.new_outer_vid,                                    /* new_outer_vid */
                                                                  g_vlan_translate_tpid_from_packet.new_inner_vid,                                    /* new_inner_vid */
                                                                  g_vlan_translate_tpid_from_packet.vlan_edit_profile[IN_PORT1_INDEX],                /* vlan_edit_profile */
                                                                  g_vlan_translate_tpid_from_packet.tag_format[IN_PORT1_INDEX],                       /* tag_format */
                                                                  FALSE                                                                               /* is_ive */
                                                                  );

     if (rv != BCM_E_NONE)
     {
         printf
             ("Error, in vlan_translate_ive_eve_translation_set_with_tpid_action(setting iPort%d EVE action\n", IN_PORT1_INDEX);
         return rv;
     }

    /*
    * Port2: with tpid_action set to "Use packet's outer TPID"
    */
     rv = vlan_translate_ive_eve_translation_set_with_tpid_action(unit, g_vlan_translate_tpid_from_packet.inLifList[IN_PORT2_INDEX],                  /* lif  */
                                                                  BCM_SWITCH_TPID_VALUE_INVALID,                                                      /* outer_tpid */
                                                                  BCM_SWITCH_TPID_VALUE_INVALID,                                                      /* inner_tpid */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT2_INDEX].outer_tpid_action,   /* outer_tpid_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT2_INDEX].inner_tpid_action,   /* inner_tpid_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT2_INDEX].outer_action,        /* outer_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT2_INDEX].inner_action,        /* inner_action */
                                                                  g_vlan_translate_tpid_from_packet.new_outer_vid,                                    /* new_outer_vid */
                                                                  g_vlan_translate_tpid_from_packet.new_inner_vid,                                    /* new_inner_vid */
                                                                  g_vlan_translate_tpid_from_packet.vlan_edit_profile[IN_PORT2_INDEX],                /* vlan_edit_profile */
                                                                  g_vlan_translate_tpid_from_packet.tag_format[IN_PORT2_INDEX],                       /* tag_format */
                                                                  FALSE                                                                               /* is_ive */
                                                                  );

     if (rv != BCM_E_NONE)
     {
         printf
             ("Error, in vlan_translate_ive_eve_translation_set_with_tpid_action(setting iPort%d EVE action\n", IN_PORT2_INDEX);
         return rv;
     }

    /*
    * Port3: with tpid_action set to "Use packet's inner TPID"
    */
     rv = vlan_translate_ive_eve_translation_set_with_tpid_action(unit, g_vlan_translate_tpid_from_packet.inLifList[IN_PORT3_INDEX],                  /* lif  */
                                                                  BCM_SWITCH_TPID_VALUE_INVALID,                                                      /* outer_tpid */
                                                                  BCM_SWITCH_TPID_VALUE_INVALID,                                                      /* inner_tpid */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT3_INDEX].outer_tpid_action,   /* outer_tpid_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT3_INDEX].inner_tpid_action,   /* inner_tpid_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT3_INDEX].outer_action,        /* outer_action */
                                                                  g_vlan_translate_tpid_from_packet.action_value[IN_PORT3_INDEX].inner_action,        /* inner_action */
                                                                  g_vlan_translate_tpid_from_packet.new_outer_vid,                                    /* new_outer_vid */
                                                                  g_vlan_translate_tpid_from_packet.new_inner_vid,                                    /* new_inner_vid */
                                                                  g_vlan_translate_tpid_from_packet.vlan_edit_profile[IN_PORT3_INDEX],                /* vlan_edit_profile */
                                                                  g_vlan_translate_tpid_from_packet.tag_format[IN_PORT3_INDEX],                       /* tag_format */
                                                                  FALSE                                                                               /* is_ive */
                                                                  );

     if (rv != BCM_E_NONE)
     {
         printf
             ("Error, in vlan_translate_ive_eve_translation_set_with_tpid_action(setting iPort%d EVE action\n", IN_PORT3_INDEX);
         return rv;
     }

    /*
     * Update LLVP: 
     * At the ingress: the input packet is double tag {packet_s_tag, packet_c_tag}:
     */
    bcm_port_tpid_class_t port_tpid_class;

    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.tpid1 = g_vlan_translate_tpid_from_packet.tpid_value.packet_s_tag;
    port_tpid_class.tpid2 = g_vlan_translate_tpid_from_packet.tpid_value.packet_c_tag;

    for (i = 0; i < NUM_OF_PORTS; i++)
    {
       port_tpid_class.tag_format_class_id = g_vlan_translate_tpid_from_packet.tag_format[i];

       /*
        * Ingress:
        */
       port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
       port_tpid_class.port = g_vlan_translate_tpid_from_packet.in_ports[i];
       printf("bcm_port_tpid_class_set(port=%d, tag_format_class_id=%d)\n", port_tpid_class.port,
                     port_tpid_class.tag_format_class_id);

        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_tpid_class_set( iPort%d )\n", i);
            return rv;
        }
    }

    /*
     * Update LLVP: 
     * At the egress: the packet after IVE is double tag {ingress_outer_tpid, ingress_inner_tpid}:
     */
    port_tpid_class.tpid1 = g_vlan_translate_tpid_from_packet.tpid_value.ingress_outer_tpid;
    port_tpid_class.tpid2 = g_vlan_translate_tpid_from_packet.tpid_value.ingress_inner_tpid;

    for (i = 0; i < NUM_OF_PORTS; i++)
    {
       port_tpid_class.tag_format_class_id = g_vlan_translate_tpid_from_packet.tag_format[i];

        /*
         * Egress:
         */
        port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
        port_tpid_class.port = g_vlan_translate_tpid_from_packet.out_ports[i];

        printf("bcm_port_tpid_class_set(port=%d, tag_format_class_id=%d)\n", port_tpid_class.port,
               port_tpid_class.tag_format_class_id);

        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_tpid_class_set( oPort%d )\n", i);
            return rv;
        }
    }

    /*
     * Add l2 address -
     * adding oPort1/2/3 and outLif1/2/3 to MACT
     */
    bcm_l2_addr_t l2_addr;

    for (i = 0; i < NUM_OF_PORTS; i++)
    {
       bcm_l2_addr_t_init(&l2_addr, g_vlan_translate_tpid_from_packet.d_mac, g_vlan_translate_tpid_from_packet.vsi[i]);
       l2_addr.flags = BCM_L2_STATIC;
       l2_addr.port = g_vlan_translate_tpid_from_packet.out_ports[i];
       l2_addr.encap_id = g_vlan_translate_tpid_from_packet.outLifList[i];

       rv = bcm_l2_addr_add(unit, &l2_addr);
       if (rv != BCM_E_NONE)
       {
           printf("Error, bcm_l2_addr_add\n");
           return rv;
       }
    }

    return BCM_E_NONE;
}


/*
 * Cleanup function runs the vlan translate tpid from packet example
 *
 * The steps of cleanup (in invers order of the configuration):
 *    1. Delete EVE actions.
 *    2. Destroy LIFs.
 *    3. Destroy VSI.
 *    4. Delete all TPIDs.
 *    5. Destroy LLVP classifiaction.
 *
 * INPUT: unit - unit
 */
int
vlan_translate_tpid_from_packet__cleanup__start_run(
    int unit)
{
    int rv;
    int i;

    printf("vlan_translate_tpid_from_packet__cleanup__start_run\n");

    /*
     * IVE/EVE actions:
     */
    rv = bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_EGRESS);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_translate_action_id_destroy_all(BCM_VLAN_ACTION_SET_EGRESS)\n");
        return rv;
    }

    rv = bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_INGRESS);

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
       rv = bcm_vlan_port_destroy(unit, g_vlan_translate_tpid_from_packet.inLifList[i]);
       if (rv != BCM_E_NONE)
       {
          printf("Error, in bcm_vlan_port_destroy(gport = 0x%08X)\n", g_vlan_translate_tpid_from_packet.inLifList[i]);
          return rv;
       }
    }
    
    /*
     * VSI
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
       rv = bcm_vlan_destroy(unit, g_vlan_translate_tpid_from_packet.vsi[i]);
       if (rv != BCM_E_NONE)
       {
           printf("Error, in bcm_vlan_destroy(vsi=%d)\n", g_vlan_translate_tpid_from_packet.vsi[i]);
           return rv;
       }

    }

    /*
     * Disable traffic before deleting TPIDs
     */
     rv = bcm_stk_module_enable(unit, 0 , 0, 0);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_stk_module_enable\n");
         return rv;
     }

    /*
     * TPIDs
     */
     rv = bcm_switch_tpid_delete_all(unit);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_switch_tpid_delete_all\n");
         return rv;
     }

    /*
     * Re-enable traffic before deleting TPIDs
     */
     rv = bcm_stk_module_enable(unit, 0 , 0, 1);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_stk_module_enable\n");
         return rv;
     }

     /*
      * LLVP classification:
      */
     for (i=0; i<NUM_OF_PORTS; i++)
     {
        rv = bcm_port_tpid_class_destroy(unit, 0, g_vlan_translate_tpid_from_packet.tag_format[i]);

        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_tpid_class_destroy(tag_format_class_id = %d\n", g_vlan_translate_tpid_from_packet.tag_format[i]);
            return rv;
        }
     }

    return BCM_E_NONE;
}
