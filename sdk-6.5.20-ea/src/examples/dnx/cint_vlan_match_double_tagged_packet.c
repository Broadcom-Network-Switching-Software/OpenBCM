/* $Id: cint_vlan_match_double_tagged_packet.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The cint creates a basic L2VPN scheme with a Logical Interfaces(LIF) associated with port X double-tagged-packet.                                *
 * The test uses one input ports, iPort and three output ports, oPort1, oPort2 and oPort3.                                                          *
 * Through the input port, three types of double tagged packet will be injected:                                                                    *
 *      - Packet tagged with S-tag + C-tag VLANs.                                                                                                   *
 *      - Packet tagged with C-tag + C-tag VLANs.                                                                                                   *
 *      - Packet tagged with S-tag + S-tag VLANs.                                                                                                   *
 * Each packet hit different In-LIF, different VSI is packeted and different outport is set.                                                        *
 * The below diagram discribes the settings:                                                                                                        *
 *                                                                                                                                                  *
 *                                                                                              MACT                                                *
 *                                                                                 +-----------------+                                              *
 *                                       +------------+           +-------+        |                 |                                              *
 *                      SC ISEM Lookup   |            |           |       |        |                 |                                              *
 *                     +---------------->+  In-LIF 1  +---------> | VSI 1 +----------> oPort1 +------------------------+                            *
 *        iPort        |                 |            |           |       |        |                 |                 |                            *
 *       +-----------> |                 +------------+           +-------+        |                 |                 |                            *
 *                     |                                                           |                 |                 |                            *
 *                     |                 +------------+           +-------+        |                 |                 |                            *
 *                     | CC ISEM Lookup  |            |           |       |        |                 |                 |                            *
 *                     +---------------->+  In-LIF2   +---------> | VSI 2 +----------> oPort2 +--------------------+   |                            *
 *                     |                 |            |           |       |        |                 |             |   |                            *
 *                     |                 +------------+           +-------+        |                 |             |   |                            *
 *                     |                                                           |                 |             |   |                            *
 *                     |                 +------------+           +-------+        |                 |             |   |                            *
 *                     | SS ISEM Lookup  |            |           |       |        |                 |             |   |                            *
 *                     +---------------->+  In-LIF 3  +---------> | VSI 3 +----------> oPort3 +---------------+    |   |                            *
 *                                       |            |           |       |        |                 |        |    |   |                            *
 *                                       +------------+           +-------+        |                 |        |    |   |                            *
 *                                                                                 |                 |        |    |   |                            *
 *                                                                                 +-----------------+        |    |   |                            *
 *                                                                                                            |    |   |                            *
 *                                                                                                            |    |   |                            *
 *                                                                                                            |    |   |                            *
 *      oPort3  <---------------------------------------------------------------------------------------------+    |   |                            *
 *                                                                                                                 |   |                            *
 *      oPort2  <--------------------------------------------------------------------------------------------------+   |                            *
 *                                                                                                                     |                            *
 *      oPort1  <------------------------------------------------------------------------------------------------------+                            *
 *                                                                                                                                                  *
 * Test steps:                                                                                                                                      *
 * 1. Build the above BRIDGE configuration (using bcm_vlan_port_create).                                                                            *
 * 2. Ingress SC double-tagged packet {0x9100, 0x8100}:                                                                                             *
 *      Expected result: packet is egressed via port oPort1.                                                                                        *
 * 3. Ingress SC double-tagged packet {0x88A8, 0x8100}:                                                                                             *
 *      Expected result: packet is egressed via port oPort1.                                                                                        *
 * 4. Ingress CC double-tagged packet {0x8100, 0x8100}:                                                                                             *
 *      Expected result: packet is egressed via port oPort2.                                                                                        *
 * 5. Ingress SS double-tagged packet {0x9100, 0x9100}:                                                                                             *
 *      Expected result: packet is egressed via port oPort3.                                                                                        *
 * 6. Ingress SS double-tagged packet {0x88A8, 0x88A8}:                                                                                             *
 *      Expected result: packet is egressed via port oPort3.                                                                                        *
 * 7. Re-build the above BRIDGE configuration (using bcm_vlan_port_create and bcm_port_match_add).                                                  * 
 * Resend all above packet and expect same results.                                                                                                 * 
 *                                                                                                                                                  *
 * INPUT packets:                                                                                                                                   *
 * ETH2:                                                                                                                                            *
 *       ethernet header with DA 00:00:00:00:0x33:0x44                                                                                              *
 *       Vlan tag:                                                                                                                                  *
 *          outer:                                                                                                                                  *
 *             vlan-id: 0x55                                                                                                                        *
 *             TPID: 0x9100/0x88A8/0x8100                                                                                                           *
 *          inner:                                                                                                                                  *
 *             vlan-id: 0x66                                                                                                                        *
 *             TPID: 0x9100/0x88A8/0x8100                                                                                                           *
 *                                                                                                                                                  *
 * OUTPUT packets:                                                                                                                                  *
 *      Same as input packet.                                                                                                                       *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/dnx                                                                                                                  *
 * cint cint_vlan_match_double_tagged_packet.c                                                                                                      * 
 * cd ../sand/                                                                                                                                      * 
 * cint utility/cint_sand_utils_global.c                                                                                                            *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * vlan_match_double_tagged_packet_with_ports__main_config__start_run (int unit,  int in_port, int out_port1, int out_port2, int out_port3)         *
 * vlan_match_double_tagged_packet__step_config__start_run (int unit,  int step =1)                                                                 *
 * Send the above double-tagged packets                                                                                                             *
 * Expected resutls - the packet is transmitted via out_port 1/2/3                                                                                  *
 * vlan_match_double_tagged_packet__step_config__start_run (int unit,  int step =2)                                                                 * 
 * Send the above double-tagged packets                                                                                                             *
 * Expected resutls - the packet is transmitted via out_port 1/2/3                                                                                  *
 * vlan_match_double_tagged_packet__cleanup__start_run(int unit)                                                                                    *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_vlan_match_double_tagged_packet', before calling           *
 * the main  function                                                                                                                               *
 * In order to re-write only part of values, call 'vlan_match_double_tagged_packet_struct_get(vlan_match_double_tagged_packet_s)' function and      *
 * re-write values, prior calling the main function                                                                                                 *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int OUT_PORT1_INDEX = 0;
int OUT_PORT2_INDEX = 1;
int OUT_PORT3_INDEX = 2;
int NUM_OF_OUT_PORTS = 3;

/*  Main Struct  */
struct vlan_match_double_tagged_packet_s
{
    bcm_gport_t in_port;
    bcm_gport_t out_ports[NUM_OF_OUT_PORTS];
    bcm_vlan_t vsi[NUM_OF_OUT_PORTS];
    bcm_gport_t inLifs[NUM_OF_OUT_PORTS];
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    bcm_mac_t d_mac;
};

/* Initialization of global struct*/
vlan_match_double_tagged_packet_s g_vlan_match_double_tagged_packet = {
   /* in_port: */
   0,
   /* out_ports: */
   {0, 0, 0},
   /* vsi:*/
   {100, 101, 102},
   /* inLifs */
   {0, 0 , 0},
   /* outer_vid */
   0x55,
   /* inner_vid */
   0x66,
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
 *   param: new values for g_vlan_match_double_tagged_packet
 */
int
vlan_match_double_tagged_packet_init(
    int unit,
    vlan_match_double_tagged_packet_s * param)
{
    int i, rv;

    if (param != NULL)
    {
        sal_memcpy(&g_vlan_match_double_tagged_packet, param, sizeof(g_vlan_match_double_tagged_packet));
    }

    return BCM_E_NONE;
}

/*
 * Return vlan_match_double_tagged_packet information
 */
void
vlan_match_double_tagged_packet_struct_get(
    int unit,
    vlan_match_double_tagged_packet_s * param)
{

    sal_memcpy(param, &g_vlan_match_double_tagged_packet, sizeof(g_vlan_match_double_tagged_packet));

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
vlan_match_double_tagged_packet_with_ports__main_config__start_run(
    int unit,
    int iPort,
    int oPort1,
    int oPort2,
    int oPort3)
{

    vlan_match_double_tagged_packet_s param;

    vlan_match_double_tagged_packet_struct_get(unit, &param);

    param.in_port = iPort;
    param.out_ports[OUT_PORT1_INDEX] = oPort1;
    param.out_ports[OUT_PORT2_INDEX] = oPort2;
    param.out_ports[OUT_PORT3_INDEX] = oPort3;

    return vlan_match_double_tagged_packet__main_config__start_run(unit, &param);
}

/*
 * Main function runs the vlan "match double-tagged packet" example
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Create VSIs.
 *    3. Setting VLAN domain for ingress/egress ports.
 *    4. Set VLAN membership.
 *    5. Setting MACT DA with egress port.
 *
 * INPUT: unit  - unit
 *        param - the new values
 */
int
vlan_match_double_tagged_packet__main_config__start_run(
    int unit,
    vlan_match_double_tagged_packet_s * param)
{
    int rv;
    int i;

    rv = vlan_match_double_tagged_packet_init(unit, param);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_match_double_tagged_packet_init\n");
        return rv;
    }

  /*
   * Create VSI:
   */
    for (i = 0; i < NUM_OF_OUT_PORTS; i++)
    {
      rv = bcm_vlan_create(unit, g_vlan_match_double_tagged_packet.vsi[i]);
      if (rv != BCM_E_NONE)
      {
          printf("Error, in bcm_vlan_create(vis=%d)\n", g_vlan_match_double_tagged_packet.vsi[i]);
          return rv;
      }
    }

    /*
     * Set vlan domain:
     */
    for (i = 0; i < NUM_OF_OUT_PORTS; i++)
    {
        rv = bcm_port_class_set(unit, g_vlan_match_double_tagged_packet.out_ports[i], bcmPortClassId,
                                g_vlan_match_double_tagged_packet.out_ports[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set(port = %d)\n", g_vlan_match_double_tagged_packet.out_port[i]);
            return rv;
        }
    }

    rv = bcm_port_class_set(unit, g_vlan_match_double_tagged_packet.in_port, bcmPortClassId,
                            g_vlan_match_double_tagged_packet.in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_vlan_match_double_tagged_packet.in_port);
        return rv;
    }

    /*
     * Set VLAN port membership for the double-tagged packet: add the outer VID.
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    for (i = 0; i < NUM_OF_OUT_PORTS; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, g_vlan_match_double_tagged_packet.out_ports[i]);
    }
    BCM_PBMP_PORT_ADD(pbmp, g_vlan_match_double_tagged_packet.in_port);

    rv = bcm_vlan_port_add(unit, g_vlan_match_double_tagged_packet.outer_vid, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_port_add(vid=%d)\n", g_vlan_match_double_tagged_packet.outer_vid);
        return rv;
    }

   /*
    * Add l2 address -
    * adding oPort to MACT
    */
   bcm_l2_addr_t l2_addr;

   for (i = 0; i < NUM_OF_OUT_PORTS; i++)
   {
       bcm_l2_addr_t_init(&l2_addr, g_vlan_match_double_tagged_packet.d_mac, g_vlan_match_double_tagged_packet.vsi[i]);
       l2_addr.flags = BCM_L2_STATIC;
       l2_addr.port = g_vlan_match_double_tagged_packet.out_ports[i];
    
       rv = bcm_l2_addr_add(unit, &l2_addr);
       if (rv != BCM_E_NONE)
       {
           printf("Error, bcm_l2_addr_add (VSI = %d\n, g_vlan_match_double_tagged_packet.vsi[i]");
           return rv;
       }
   }

    return BCM_E_NONE;
}

/*
 * Step function runs the vlan "match double-tagged packet" example
 *
 * Main steps of configuration:
 * step 1:
 *  Creates tha above BRIDGE configuration using bcm_vlan_port_create using criteria
 *      - BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
 *      - BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED
 *      - BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED
 * step 2:
 *  Creates tha above BRIDGE configuration using bcm_vlan_port_create using criteria BCM_VLAN_PORT_MATCH_NONE
 *  and using bcm_port_match_add with match
 *      - BCM_PORT_MATCH_PORT_VLAN_STACKED
 *      - BCM_PORT_MATCH_PORT_CVLAN_STACKE
 *      - BCM_PORT_MATCH_PORT_SVLAN_STACKED
 *
 * INPUT: unit  - unit
 *        param - the new values
 */

int
vlan_match_double_tagged_packet__step_config__start_run(
    int unit,
    int step)
{
    int rv = BCM_E_PARAM;

    printf("vlan_match_double_tagged_packet__step_config__start_run( step = %d )\n", step);

    switch (step)
    {
       case 1:
       {
           int i;

          /*
           * Create In-LIF associated with:
           *    - iPort X VLAN X VLAN
           *    - iPort X CVLAN X CVLAN
           *    - iPort X SVLAN X SVLAN
           */
           bcm_vlan_port_t vlan_port;
           bcm_vlan_port_match_t criteria[NUM_OF_OUT_PORTS] = {BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED, BCM_VLAN_PORT_MATCH_PORT_CVLAN_STACKED, BCM_VLAN_PORT_MATCH_PORT_SVLAN_STACKED};

           bcm_vlan_port_t_init(&vlan_port);
           vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
           vlan_port.port = g_vlan_match_double_tagged_packet.in_port;
           vlan_port.match_vlan = g_vlan_match_double_tagged_packet.outer_vid;
           vlan_port.match_inner_vlan = g_vlan_match_double_tagged_packet.inner_vid;

           for (i = 0; i < NUM_OF_OUT_PORTS; i++)
           {
               vlan_port.criteria = criteria[i];
               vlan_port.vsi = g_vlan_match_double_tagged_packet.vsi[i];

               rv = bcm_vlan_port_create(unit, &vlan_port);
               if (rv != BCM_E_NONE)
               {
                   printf("Error, in bcm_vlan_port_create(vsi = %d)\n", vlan_port.vsi);
                   return rv;
               }
               g_vlan_match_double_tagged_packet.inLifs[i] = vlan_port.vlan_port_id;
               printf("vsi = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.vsi, vlan_port.vlan_port_id, vlan_port.encap_id);
            }

           break;
        }
        case 2:
        {
            int i;
            /*
             * First, if LIF were already created, destroy the LIFs
             */
            if (g_vlan_match_double_tagged_packet.inLifs[0])
            {
                for (i = 0; i < NUM_OF_OUT_PORTS; i++)
                {

                    rv = bcm_vlan_port_destroy(unit, g_vlan_match_double_tagged_packet.inLifs[i]);
                    if (rv != BCM_E_NONE)
                    {
                       printf("Error, in bcm_vlan_port_destroy(gport = 0x%08X)\n", g_vlan_match_double_tagged_packet.inLifs[i]);
                       return rv;
                    }
                }

            }
          /*
           * Re-create In-LIF with NONE criteria
           */
           bcm_vlan_port_t vlan_port;
           bcm_vlan_port_t_init(&vlan_port);
           vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
           vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;

           for (i = 0; i < NUM_OF_OUT_PORTS; i++)
           {
               vlan_port.vsi = g_vlan_match_double_tagged_packet.vsi[i];

               rv = bcm_vlan_port_create(unit, &vlan_port);
               if (rv != BCM_E_NONE)
               {
                   printf("Error, in bcm_vlan_port_create(vsi = %d)\n", vlan_port.vsi);
                   return rv;
               }
               g_vlan_match_double_tagged_packet.inLifs[i] = vlan_port.vlan_port_id;
               printf("vsi = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vlan_port.vsi, vlan_port.vlan_port_id, vlan_port.encap_id);
            }

          /*
           * Associate In-LIF with:
           *    - iPort X VLAN X VLAN
           *    - iPort X CVLAN X CVLAN
           *    - iPort X SVLAN X SVLAN
           */
           bcm_port_match_t criteria[NUM_OF_OUT_PORTS] = {BCM_PORT_MATCH_PORT_VLAN_STACKED, BCM_PORT_MATCH_PORT_CVLAN_STACKED, BCM_PORT_MATCH_PORT_SVLAN_STACKED};

           bcm_port_match_info_t match_info;
           match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
           match_info.port = g_vlan_match_double_tagged_packet.in_port;
           match_info.match_vlan = g_vlan_match_double_tagged_packet.outer_vid;
           match_info.match_inner_vlan = g_vlan_match_double_tagged_packet.inner_vid;

           for (i = 0; i < NUM_OF_OUT_PORTS; i++)
           {
               match_info.match = criteria[i];

               rv = bcm_port_match_add(unit, g_vlan_match_double_tagged_packet.inLifs[i], &match_info);
               if (rv != BCM_E_NONE)
               {
                   printf("Error, bcm_port_match_add (port = 0x%08X, criteria = %d)\n", g_vlan_match_double_tagged_packet.inLifs[i], match_info.match);
                   return rv;
               }
           }

           break;
        }
        default:
        {
            printf("Error, step = %d is out of range [1:2]!\n", step);
        }
    }

    return rv;
}


/*
 * Cleanup function clears the vlan "match double-tagged packet" example
 *
 * The steps of cleanup (in invers order of the configuration):
 *    1. Clear ISEM. 
 *    2. Destroy LIF.
 *    3. Destroy VSI.
 *
 * INPUT: unit - unit
 */
int
vlan_match_double_tagged_packet__cleanup__start_run(
    int unit)
{
    int rv;
    int i;

    printf("vlan_match_double_tagged_packet__cleanup__start_run\n");

    /*
     * ISEM
     */
    bcm_port_match_t criteria[NUM_OF_OUT_PORTS] = {BCM_PORT_MATCH_PORT_VLAN_STACKED, BCM_PORT_MATCH_PORT_CVLAN_STACKED, BCM_PORT_MATCH_PORT_SVLAN_STACKED};

    bcm_port_match_info_t match_info;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    match_info.port = g_vlan_match_double_tagged_packet.in_port;
    match_info.match_vlan = g_vlan_match_double_tagged_packet.outer_vid;
    match_info.match_inner_vlan = g_vlan_match_double_tagged_packet.inner_vid;

    for (i = 0; i < NUM_OF_OUT_PORTS; i++)
    {
        match_info.match = criteria[i];

        rv = bcm_port_match_delete(unit, g_vlan_match_double_tagged_packet.inLifs[i], &match_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_match_delete (port = 0x%08X, criteria = %d)\n", g_vlan_match_double_tagged_packet.inLifs[i], match_info.match);
            return rv;
        }
    }


    /*
     * LIFs
     */
    for (i = 0; i < NUM_OF_OUT_PORTS; i++)
    {
         
        rv = bcm_vlan_port_destroy(unit, g_vlan_match_double_tagged_packet.inLifs[i]);
        if (rv != BCM_E_NONE)
        {
           printf("Error, in bcm_vlan_port_destroy(gport = 0x%08X)\n", g_vlan_match_double_tagged_packet.inLifs[i]);
           return rv;
        }
    }
    
    /*
     * VSIs
     */
    for (i = 0; i < NUM_OF_OUT_PORTS; i++)
    {
        rv = bcm_vlan_destroy(unit, g_vlan_match_double_tagged_packet.vsi[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_destroy(vsi=%d)\n", g_vlan_match_double_tagged_packet.vsi[i]);
            return rv;
        }
    }


    return BCM_E_NONE;
}