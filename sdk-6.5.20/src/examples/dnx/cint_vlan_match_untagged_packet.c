/* $Id: cint_vlan_match_untagged_packet.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The cint creates a basic L2VPN scheme with a Logical Interfaces(LIF) associated with port X untagged-packet.                                     *
 * The test uses two input ports, iPort1 and iPort2 and one output ports oPort.                                                                     *
 * First step:                                                                                                                                      *
 * 1. Default BRIDGE settings.                                                                                                                      *
 * 2. Adding oPort to MACT.                                                                                                                         *
 * 3. Ingress untagged-packet:                                                                                                                      *
 *   Expected result: c-tag (EVE: VID = VSI is added to the packet).                                                                                *
 * 4. Ingress tagged-packet (one tag: c-tag):                                                                                                       *
 *   Expected result: c-tag (same packet).                                                                                                          *
 *                                                                                                                                                  *
 * ==> untagged-packet and tagged-packet (with no lookup match in ISEM) gets same default-LIF.                                                      *
 *                                                                                                                                                  *
 * Second step:                                                                                                                                     *
 * 1. Create a VSI and set VLAN port membership (setting VLAN port membership is not mandatory, it is optional).                                    *
 * 2. Create In-LIF associated with iPort1 X untagged-packet.                                                                                       *
 * 3. Create IVE (add, add) for untagged-packet.                                                                                                    *
 * 4. Ingress untagged-packet                                                                                                                       *
 *  Expected result: d-tag (IVE).                                                                                                                   *
 * 5. Ingress tagged-packet (one tag: c-tag):                                                                                                       *
 *  Expected result: c-tag (same packet).                                                                                                           *
 *                                                                                                                                                  *
 * ==> untagged-packet is treated differently than tagged packet with no hit in the ISEM!                                                           *
 *                                                                                                                                                  *
 * Third step:                                                                                                                                      *
 * 1. Add ISEM lookup for iPort2 X untagged-packet to the LIF created in second step.                                                               *
 * 2. Ingress untagged-packet                                                                                                                       *
 *  Expected result: d-tag (IVE).                                                                                                                   *
 * 3. Ingress tagged-packet (one tag: c-tag):                                                                                                       *
 *  Expected result: c-tag (same packet).                                                                                                           *
 *                                                                                                                                                  *
 * INPUT packets:                                                                                                                                   *
 * ETH0:                                                                                                                                            *
 *      ethernet header with DA 00:00:00:00:0x33:0x44                                                                                               *
 * ETH1:                                                                                                                                            *
 *      ethernet header with DA 00:00:00:00:0x33:0x44                                                                                               *
 *      Vlan tag:                                                                                                                                   *
 *         outer:                                                                                                                                   *
 *             vlan-id: 0x77                                                                                                                        *
 *             TPID: 0x8100                                                                                                                         *
 *                                                                                                                                                  *
 * OUTPUT packets:                                                                                                                                  *
 * ETH1 (EVE: VSI is added):                                                                                                                        *
 *       ethernet header with DA 00:00:00:00:0x33:0x44                                                                                              *
 *       Vlan tag:                                                                                                                                  *
 *          outer:                                                                                                                                  *
 *             vlan-id: 100 (VSI = 100)                                                                                                             *
 *             TPID: 0x8100                                                                                                                         *
 * ETH1:                                                                                                                                            *
 *       ethernet header with DA 00:00:00:00:0x33:0x44                                                                                              *
 *       Vlan tag:                                                                                                                                  *
 *          outer:                                                                                                                                  *
 *             vlan-id: 0x77                                                                                                                        *
 *             TPID: 0x8100                                                                                                                         *
 * ETH2 (IVE: add+add):                                                                                                                             *
 *       ethernet header with DA 00:00:00:00:0x33:0x44                                                                                              *
 *       Vlan tag:                                                                                                                                  *
 *          outer:                                                                                                                                  *
 *             vlan-id: 0x55                                                                                                                        *
 *             TPID: 0x9100                                                                                                                         *
 *          inner:                                                                                                                                  *
 *             vlan-id: 0x66                                                                                                                        *
 *             TPID: 0x8100                                                                                                                         *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *             +-----------+   Action:{Add,Add}                                                                                                     *
 * iPort1/2    |           |      +-----+      +-------+                                                                                            *
 *    +------> | LIF       +----> |IVE  +----> |       |                                                                                            *
 *             |           |      +-----+      |       |     +-------+                                                                              *
 *             +-----------+                   |       |     |       |  oPort                                                                       *
 *                                             | VSI   +---> | MACT  +-------------+                                                                *
 *                                             |       |             |             |                                                                *
 *                                             |       |     +-------+             |                                                                *
 *                                             |       |                           |                                                                *
 *                                             +-------+                           |                                                                *
 *                                                                                 |                                                                *
 *                                                                                 |                                                                *
 *                                                                                 |                                                                *
 *     oPort                                                                       |                                                                *
 *    <----------------------------------------------------------------------------+                                                                *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/sand                                                                                                                 *
 * cint cint_vlan_match_untagged_packet.c                                                                                                           *
 * cint utility/cint_sand_utils_vlan_translate.c                                                                                                    *
 * cint utility/cint_sand_utils_global.c                                                                                                            *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * vlan_match_untagged_packet_with_ports__main_config__start_run (int unit,  int in_port1, int in_port2, int out_port)                              *
 * vlan_match_untagged_packet__step_config__start_run (int unit,  int step = 2)                                                                     *
 * vlan_match_untagged_packet__step_config__start_run (int unit,  int step =3)                                                                      *
 * vlan_match_untagged_packet__cleanup__start_run(int unit)                                                                                         *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_vlan_match_untagged_packet', before calling                *
 * the main  function                                                                                                                               *
 * In order to re-write only part of values, call 'vlan_match_untagged_packet_struct_get(vlan_match_untagged_packet_s)' function and                *
 * re-write values, prior calling the main function                                                                                                 *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int IN_PORT1_INDEX = 0;
int IN_PORT2_INDEX = 1;

int NUM_OF_PORTS = 2;

/*  Main Struct  */
struct vlan_match_untagged_packet_s
{
    bcm_gport_t in_ports[NUM_OF_PORTS];
    bcm_gport_t out_port;
    bcm_vlan_t vsi;
    uint16 packet_s_tag;         /* ingress packet service tag (outer tag) */
    uint16 packet_c_tag;         /* ingress packet customer tag (inner tag) */
    bcm_vlan_t new_outer_vid;
    bcm_vlan_t new_inner_vid;
    bcm_vlan_t unknown_outer_vid;
    bcm_gport_t inLif;
    uint32 vlan_edit_profile;
    bcm_mac_t d_mac;
};

/* Initialization of global struct*/
vlan_match_untagged_packet_s g_vlan_match_untagged_packet = {
   /* in_ports: */
   {0, 0},
   /* out_port: */
   0,
   /* vsi:*/
   100,
   /* packet_s_tag */ 
   0x9100, 
   /* packet_c_tag */ 
   0x8100, 
   /* new_outer_vid */
   0x55,
   /* new_inner_vid */
   0x66,
   /* unknown_outer_vid */
    0x77,
   /* inLif */
   0,
   /* vlan_edit_profile */
   4,
   /* d_mac */
    {0x00, 0x00, 0x00, 0x00, 0x33, 0x44}
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_vlan_match_untagged_packet
 */
int
vlan_match_untagged_packet_init(
    int unit,
    vlan_match_untagged_packet_s * param)
{
    int i, rv;

    if (param != NULL)
    {
        sal_memcpy(&g_vlan_match_untagged_packet, param, sizeof(g_vlan_match_untagged_packet));
    }

    return BCM_E_NONE;
}

/*
 * Return vlan_match_untagged_packet information
 */
void
vlan_match_untagged_packet_struct_get(
    int unit,
    vlan_match_untagged_packet_s * param)
{

    sal_memcpy(param, &g_vlan_match_untagged_packet, sizeof(g_vlan_match_untagged_packet));

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
vlan_match_untagged_packet_with_ports__main_config__start_run(
    int unit,
    int iPort1,
    int iPort2,
    int oPort)
{

    vlan_match_untagged_packet_s param;

    vlan_match_untagged_packet_struct_get(unit, &param);

    param.in_ports[IN_PORT1_INDEX] = iPort1;
    param.in_ports[IN_PORT2_INDEX] = iPort2;
    param.out_port = oPort;

    return vlan_match_untagged_packet__main_config__start_run(unit, &param);
}

/*
 * Main function runs the vlan "match untagged packet" example
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Setting VLAN domain for ingress/egress ports.
 *    3. Setting MACT DA with egress port.
 *
 * INPUT: unit  - unit
 *        param - the new values
 */
int
vlan_match_untagged_packet__main_config__start_run(
    int unit,
    vlan_match_untagged_packet_s * param)
{
    int rv;
    int i;

    rv = vlan_match_untagged_packet_init(unit, param);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_match_untagged_packet_init\n");
        return rv;
    }

    /*
     * Set vlan domain:
     */
    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        rv = bcm_port_class_set(unit, g_vlan_match_untagged_packet.in_ports[i], bcmPortClassId,
                                g_vlan_match_untagged_packet.in_ports[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", g_vlan_match_untagged_packet.in_ports[i]);
            return rv;
        }
    }

    rv = bcm_port_class_set(unit, g_vlan_match_untagged_packet.out_port, bcmPortClassId,
                            g_vlan_match_untagged_packet.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_vlan_match_untagged_packet.out_port);
        return rv;
    }

    /*
     * Set VLAN port membership of the unknown_outer_vid
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    for (i = 0; i < NUM_OF_PORTS; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, g_vlan_match_untagged_packet.in_ports[i]);
    }
    BCM_PBMP_PORT_ADD(pbmp, g_vlan_match_untagged_packet.out_port);

    rv = bcm_vlan_port_add(unit, g_vlan_match_untagged_packet.unknown_outer_vid, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }

    /*
     * Add l2 address -
     * adding oPort to MACT: 
     * port is using default LIF (bridge LIF):
     * - FODO is base + VID.
     * - VSI is 0. 
     * - Initial VID is 1. 
     * - For tagged packet the VID is unknown_outer_vid.  
     * Thus: 
     *   - for untagged packet: VSI is 1.
     *   - for tagged packet: VSI is unknown_outer_vid.
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_vlan_match_untagged_packet.d_mac, 1);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_vlan_match_untagged_packet.out_port;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add (VSI = 1\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2_addr, g_vlan_match_untagged_packet.d_mac, g_vlan_match_untagged_packet.unknown_outer_vid);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_vlan_match_untagged_packet.out_port;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add (VSI = %d\n", g_vlan_match_untagged_packet.unknown_outer_vid);
        return rv;
    }


    return BCM_E_NONE;
}

/*
 * Step function runs the vlan "match untagged packet" example
 *
 * Main steps of configuration:
 * step 2:
 * 1. Create a VSI and set VLAN port membership (setting VLAN port membership is not mandatory, it is optional).
 * 2. Create In-LIF associated with iPort1 X untagged-packet.
 * 3. Create IVE (add, add) for untagged-packet.
 * 4. Setting MACT DA with egress port.
 * 
 * step 3:
 * 1. Add ISEM lookup for iPort2 X untagged-packet to the LIF created in second step.
 * 
 * step 4:
 * 1. Remove the ISEM lookup for iPort2 X untagged-packet to the LIF created in step 2.
 * 2. Destroy the In-LIF associated with iPort1 X untagged-packet (created in step 2).
 * 3. Create In-LIF associated with iPort1 X untagged-packet X ethernet type = IPv4 (TCAM look up).
 * 
 * step 5:
 * 1. Add ISEM lookup for iPort2 X untagged-packet X ethernet type = IPv4 (TCAM look up, create in step 4).
 *
 * INPUT: unit - unit
 *        step - which step in the configuration to set [2:5].
 */
int
vlan_match_untagged_packet__step_config__start_run(
    int unit,
    int step)
{
    int rv = BCM_E_PARAM;

    if ( (step < 2 ) && (step > 5) )
    {
        printf("Error, step = %d!!! Should be [2:5]\n", step);
        return rv;
    }

    printf("vlan_match_untagged_packet__step_config__start_run( step = %d )\n", step);

    switch (step)
    {
       case 2:
       {
          int i;

          /*
           * Create VSI:
           */
          rv = bcm_vlan_create(unit, g_vlan_match_untagged_packet.vsi);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in bcm_vlan_create(vis=%d)\n", g_vlan_match_untagged_packet.vsi);
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
              BCM_PBMP_PORT_ADD(pbmp, g_vlan_match_untagged_packet.in_ports[i]);
          }
          BCM_PBMP_PORT_ADD(pbmp, g_vlan_match_untagged_packet.out_port);

          rv = bcm_vlan_port_add(unit, g_vlan_match_untagged_packet.vsi, pbmp, untag_pbmp);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in bcm_vlan_port_add\n");
              return rv;
          }

          /*
           * Create In-LIF associated with iPort1 X untagged-packet
           */

          bcm_vlan_port_t vlan_port;

          bcm_vlan_port_t_init(&vlan_port);

          vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
          vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
          vlan_port.vsi = g_vlan_match_untagged_packet.vsi;
          vlan_port.port = g_vlan_match_untagged_packet.in_ports[IN_PORT1_INDEX];

           rv = bcm_vlan_port_create(unit, &vlan_port);
           if (rv != BCM_E_NONE)
           {
               printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
               return rv;
           }

           g_vlan_match_untagged_packet.inLif = vlan_port.vlan_port_id;

           printf("port=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.vlan_port_id, vlan_port.encap_id);

           /*
            * Create IVE (add, add) command for untagged-packet:
            * {Add, Add}
            */

           rv = vlan_translate_ive_eve_translation_set(unit, g_vlan_match_untagged_packet.inLif,         /* lif gport */
                                                       g_vlan_match_untagged_packet.packet_s_tag,        /* outer_tpid */
                                                       g_vlan_match_untagged_packet.packet_c_tag,        /* inner_tpid */
                                                       bcmVlanActionAdd,                                 /* outer_action */
                                                       bcmVlanActionAdd,                                 /* inner_action */
                                                       g_vlan_match_untagged_packet.new_outer_vid,       /* new_outer_vid */
                                                       g_vlan_match_untagged_packet.new_inner_vid,       /* new_inner_vid */
                                                       g_vlan_match_untagged_packet.vlan_edit_profile,   /* vlan_edit_profile */
                                                       0,                                                /* tag_format - untagged = 0 */
                                                       TRUE                                              /* is_ive */
               );
           if (rv != BCM_E_NONE)
           {
               printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {Add,Add}\n");
               return rv;
           }

          /*
           * Add l2 address -
           * adding oPort to MACT: 
           */
          bcm_l2_addr_t l2_addr;

          bcm_l2_addr_t_init(&l2_addr, g_vlan_match_untagged_packet.d_mac, g_vlan_match_untagged_packet.vsi);
          l2_addr.flags = BCM_L2_STATIC;
          l2_addr.port = g_vlan_match_untagged_packet.out_port;

          rv = bcm_l2_addr_add(unit, &l2_addr);
          if (rv != BCM_E_NONE)
          {
              printf("Error, bcm_l2_addr_add (VSI = 0\n");
              return rv;
          }

          break;
       }
       case 3:
       {
          /*
          * step 3:
          * Add ISEM lookup for iPort2 X untagged-packet to the LIF created in second step.
          */
          bcm_port_match_info_t match_info;

          match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
          match_info.match = BCM_PORT_MATCH_PORT_UNTAGGED;
          match_info.port = g_vlan_match_untagged_packet.in_ports[IN_PORT2_INDEX];

          rv = bcm_port_match_add(unit, g_vlan_match_untagged_packet.inLif, &match_info);
          if (rv != BCM_E_NONE)
          {
              printf("Error, bcm_port_match_add (port = 0x%08X)\n", g_vlan_match_untagged_packet.inLif);
              return rv;
          }

          break;
       }
       case 4:
       {
          /*
           * 1. Remove the ISEM lookup for iPort2 X untagged-packet to the LIF created in step 2.
           * 2. Destroy the In-LIF associated with iPort1 X untagged-packet (created in step 2).
           * 3. Create In-LIF associated with iPort1 X untagged-packet X ethernet type = IPv4 (TCAM look up).
           */
          bcm_port_match_info_t match_info;

          match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
          match_info.match = BCM_PORT_MATCH_PORT_UNTAGGED;
          match_info.port = g_vlan_match_untagged_packet.in_ports[IN_PORT2_INDEX];

          rv = bcm_port_match_delete(unit, g_vlan_match_untagged_packet.inLif, &match_info);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in bcm_port_match_delete(port = %d)\n", g_vlan_match_untagged_packet.inLif);
              return rv;
          }

          rv = bcm_vlan_port_destroy(unit, g_vlan_match_untagged_packet.inLif);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in bcm_vlan_port_destroy(port = %d)\n", g_vlan_match_untagged_packet.inLif);
              return rv;
          }

          bcm_vlan_port_t vlan_port;

          bcm_vlan_port_t_init(&vlan_port);

          vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
          vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_UNTAGGED;
          vlan_port.vsi = g_vlan_match_untagged_packet.vsi;
          vlan_port.port = g_vlan_match_untagged_packet.in_ports[IN_PORT1_INDEX];
          vlan_port.match_ethertype = 0x0800;

           rv = bcm_vlan_port_create(unit, &vlan_port);
           if (rv != BCM_E_NONE)
           {
               printf("Error, in bcm_vlan_port_create(port = %d, match_ethertype = 0x%04X)\n", vlan_port.port, vlan_port.match_ethertype);
               return rv;
           }

           g_vlan_match_untagged_packet.inLif = vlan_port.vlan_port_id;

           printf("port=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.port, vlan_port.vlan_port_id, vlan_port.encap_id);

           /*
            * Update the LIF's vlan-translation parameters:
            */
           bcm_vlan_port_translation_t vlan_port_translation;

           bcm_vlan_port_translation_t_init(&vlan_port_translation);
           vlan_port_translation.new_outer_vlan = g_vlan_match_untagged_packet.new_outer_vid;
           vlan_port_translation.new_inner_vlan = g_vlan_match_untagged_packet.new_inner_vid;
           vlan_port_translation.gport = g_vlan_match_untagged_packet.inLif;
           vlan_port_translation.vlan_edit_class_id = g_vlan_match_untagged_packet.vlan_edit_profile;
           vlan_port_translation.flags = BCM_VLAN_ACTION_SET_INGRESS;

           rv = bcm_vlan_port_translation_set(unit, &vlan_port_translation);
           if (rv != BCM_E_NONE)
           {
               printf("Error, bcm_vlan_port_translation_set\n");
               return rv;
           }

          break;
       }
       case 5:
       {
          /*
          * step 5:
          * Add ISEM lookup for iPort2 X untagged-packet X ethernet type = IPv4 (TCAM look up, create in step 4).
          */
          bcm_port_match_info_t match_info;

          match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
          match_info.match = BCM_PORT_MATCH_PORT_UNTAGGED;
          match_info.port = g_vlan_match_untagged_packet.in_ports[IN_PORT2_INDEX];
          match_info.match_ethertype = 0x0800;

          rv = bcm_port_match_add(unit, g_vlan_match_untagged_packet.inLif, &match_info);
          if (rv != BCM_E_NONE)
          {
              printf("Error, bcm_port_match_add (port = 0x%08X)\n", g_vlan_match_untagged_packet.inLif);
              return rv;
          }

          break;
       }
    }
    return rv;
}

/*
 * Cleanup function runs the vlan "match untagged packet" example
 *
 * The steps of cleanup (in invers order of the configuration):
 *    1. Delete IVE actions.
 *    2. Remove ISEM/TCAM lookups.
 *    3. Destroy LIF.
 *    4. Destroy VSI.
 *
 * INPUT: unit - unit
 */
int
vlan_match_untagged_packet__cleanup__start_run(
    int unit)
{
    int rv;

    printf("vlan_match_untagged_packet__cleanup__start_run\n");

    /*
     * IVE actions:
     */
    rv = bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_INGRESS);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_translate_action_id_destroy_all(BCM_VLAN_ACTION_SET_INGRESS)\n");
        return rv;
    }

    /*
     * ISEM lookup
     */
    bcm_port_match_info_t match_info;

    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    match_info.match = BCM_PORT_MATCH_PORT_UNTAGGED;
    match_info.port = g_vlan_match_untagged_packet.in_ports[IN_PORT2_INDEX];
    match_info.match_ethertype = 0x0800;

    rv = bcm_port_match_delete(unit, g_vlan_match_untagged_packet.inLif, &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_match_delete(port = %d)\n", g_vlan_match_untagged_packet.inLif);
        return rv;
    }

    /*
     * LIF
     */
    rv = bcm_vlan_port_destroy(unit, g_vlan_match_untagged_packet.inLif);
    if (rv != BCM_E_NONE)
    {
       printf("Error, in bcm_vlan_port_destroy(gport = 0x%08X)\n", g_vlan_match_untagged_packet.inLif);
       return rv;
    }
    
    /*
     * VSI
     */
    rv = bcm_vlan_destroy(unit, g_vlan_match_untagged_packet.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_destroy(vsi=%d)\n", g_vlan_match_untagged_packet.vsi);
        return rv;
    }


    return BCM_E_NONE;
}
