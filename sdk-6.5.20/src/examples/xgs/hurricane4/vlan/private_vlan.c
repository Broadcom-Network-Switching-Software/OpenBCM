/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Private VLAN
 *
 *  Usage    : BCM.0> cint private_vlan.c
 *
 *  config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : private_vlan_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |  community_port_1
 *                   |                              +-----------------+
 *                   |                    vlan-101  |  community_port_2
 *  Promiscuous_port |         SWITCH               +-----------------+
 *  -----------------+ Vlan-100                     |
 *                   |                              |  isolated_port_1
 *                   |                              +-----------------+
 *                   |                    vlan-102  |  isolated_port_2
 *                   |                              +-----------------+
 *                   +------------------------------+
 *   Summary:
 *   ========
 *     CINT scripts for demonstrating the learning based on the private vlan
 *     using BCM api's
 *
 *   Detailed steps done in the CINT script
 *   ======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Selects all five port and configure them in Loop back mode.
 *         Install a rule to copy incoming packets to CPU and start packet watcher.
 *
 *    2) Step2 - Configuration (Done in pvlan_config())
 *    =================================================
 *      a) configure isolated port1 and isolated port2 with isolated vlan 102
 *
 *      b) configure community port1 and community port2 with community vlan 101
 *
 *      c) configure promiscuous port with promiscuous vlan 100
 *
 *    3) Step3 - Verification(Done in verify())
 *    =========================================
 *      a) Send the below packet on isolated port_1.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=102
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      66 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 6B 53 25 A9
 *
 *      b) Send the below packet on promiscuous port.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:AA:AA
 *      SA=00:00:00:00:11:11
 *      Vlan=100
 *
 *      00 00 00 00 AA AA 00 00 00 00 11 11 81 00 00
 *      64 08 00 00 01 02 03 04 05 06 07 08 09 0A 0B
 *      0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A
 *      1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29
 *      2A 2B 2C 2D 4B 25 17 6E
 *
 *      c) Send the below packet on Community port_1.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=101
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      65 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 6B 53 25 A9
 *
 *      d)Expected Result
 *      ==================
 *      The below packet egresses on promiscuous port for step 3.a.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=100
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      64 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 0C 0B E4 6C
 *
 *      The below packet egresses on all ports for step 3.b.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:AA:AA
 *      SA=00:00:00:00:11:11
 *
 *
 *      00 00 00 00 AA AA 00 00 00 00 11 11 81 00 00
 *      66 08 00 00 01 02 03 04 05 06 07 08 09 0A 0B
 *      0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A
 *      1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29
 *      2A 2B 2C 2D 2C 7D D6 AB
 *
 *      The below packet egresses on promiscuous port and Community port_2 for set 3.c.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=100
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      64 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 0C 0B E4 6C
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=101
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      65 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 6B 53 25 A9
 */
/* Reset c interpreter*/


cint_reset();
bcm_port_t      promiscuous_port = 1;
bcm_port_t      isolated_port_1 = 2,isolated_port_2 = 3;
bcm_port_t      community_port_1 = 4, community_port_2 = 5;
bcm_vlan_t      Primary_vlan=100,Community_vlan=101,Isolated_vlan=102;
bcm_field_group_t group = -1;
/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
   */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
    printf("portNumbersGet() failed \n");
    return -1;
  }

  return BCM_E_NONE;

}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
  }

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}


/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));
  }
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}

/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[5];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 5)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  promiscuous_port = port_list[0];
  isolated_port_1 = port_list[1];
  isolated_port_2 = port_list[2];
  community_port_1 = port_list[3];
  community_port_2 = port_list[4];


  if (BCM_E_NONE != ingress_port_setup(unit, promiscuous_port)) {
    printf("ingress_port_setup() failed for port %d\n", promiscuous_port);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, isolated_port_1)) {
    printf("egress_port_setup() failed for port %d\n", isolated_port_1);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, isolated_port_2)) {
    printf("egress_port_setup() failed for port %d\n", isolated_port_2);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, community_port_1)) {
    printf("egress_port_setup() failed for port %d\n", community_port_1);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, community_port_2)) {
    printf("egress_port_setup() failed for port %d\n", community_port_2);
    return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}


bcm_error_t pvlan_config(int unit)
{
    /* Enable shared VLAN learning on the switch*/
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;
    int pkt_prio=-1;
    uint32 flags=BCM_PORT_FORCE_VLAN_ENABLE;
    bcm_vlan_control_vlan_t control;

    BCM_PBMP_CLEAR(ubmp);
    bcm_vlan_control_vlan_t_init(&control);
    control.forwarding_vlan=Primary_vlan; /*nothing but the primary vlan*/

    rv=bcm_switch_control_set(unit, bcmSwitchSharedVlanEnable, TRUE);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }
    /* isolated port and vlan setting */
    rv=bcm_port_force_vlan_set(unit, isolated_port_1, Isolated_vlan, pkt_prio, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_force_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_untagged_vlan_set(unit, isolated_port_1, Isolated_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_force_vlan_set(unit, isolated_port_2, Isolated_vlan, pkt_prio, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_force_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_untagged_vlan_set(unit, isolated_port_2, Isolated_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_vlan_create(unit, Isolated_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, promiscuous_port);

    rv=bcm_vlan_port_add(unit, Isolated_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /*set fid_id*/
    rv=bcm_vlan_control_vlan_set(unit, Isolated_vlan, control);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* community ports and vlan setting */

    rv=bcm_port_force_vlan_set(unit, community_port_1, Community_vlan, pkt_prio, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_force_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_untagged_vlan_set(unit, community_port_1, Community_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_force_vlan_set(unit, community_port_2, Community_vlan, pkt_prio, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_force_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_untagged_vlan_set(unit, community_port_2, Community_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_vlan_create(unit, Community_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, community_port_1);
    BCM_PBMP_PORT_ADD(pbmp, community_port_2);
    BCM_PBMP_PORT_ADD(pbmp, promiscuous_port); /*promiscuous port*/

    rv=bcm_vlan_port_add(unit, Community_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /*set fid_id*/
    rv=bcm_vlan_control_vlan_set(unit, Community_vlan, control);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /* promiscuous port and vlan setting */
    rv=bcm_port_force_vlan_set(unit, promiscuous_port, Primary_vlan, pkt_prio, flags);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_force_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_untagged_vlan_set(unit, promiscuous_port, Primary_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_vlan_create(unit, Primary_vlan);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, promiscuous_port); /*promiscuous port*/
    BCM_PBMP_PORT_ADD(pbmp, isolated_port_1);
    BCM_PBMP_PORT_ADD(pbmp, isolated_port_2);
    BCM_PBMP_PORT_ADD(pbmp, community_port_1);
    BCM_PBMP_PORT_ADD(pbmp, community_port_2);


    rv=bcm_vlan_port_add(unit, Primary_vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    /*set fid_id*/
    rv=bcm_vlan_control_vlan_set(unit, Primary_vlan, control);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }


    /* set the PORT.EN_IFILTER field */
    rv=bcm_port_vlan_member_set(unit, promiscuous_port, BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_vlan_member_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_vlan_member_set(unit, isolated_port_1, BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_vlan_member_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_vlan_member_set(unit, isolated_port_2, BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_vlan_member_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_vlan_member_set(unit, community_port_1, BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_vlan_member_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv=bcm_port_vlan_member_set(unit, community_port_2, BCM_PORT_VLAN_MEMBER_INGRESS);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_vlan_member_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    return BCM_E_NONE;
}


void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

/*    a) Send the below packet on isolated port_1.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=102
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      66 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 6B 53 25 A9
 *
 *    b)Expected Result
 *    ==================
 *      The below packet egresses on promiscuous port.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=100
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      64 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 0C 0B E4 6C
 *
 *    c) Send the below packet on promiscuous port.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:AA:AA
 *      SA=00:00:00:00:11:11
 *      Vlan=100
 *
 *      00 00 00 00 AA AA 00 00 00 00 11 11 81 00 00
 *      64 08 00 00 01 02 03 04 05 06 07 08 09 0A 0B
 *      0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A
 *      1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29
 *      2A 2B 2C 2D 4B 25 17 6E
 *
 *    d)Expected Result
 *    ==================
 *      The below packet egresses on all ports.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:AA:AA
 *      SA=00:00:00:00:11:11
 *
 *
 *      00 00 00 00 AA AA 00 00 00 00 11 11 81 00 00
 *      66 08 00 00 01 02 03 04 05 06 07 08 09 0A 0B
 *      0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A
 *      1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 28 29
 *      2A 2B 2C 2D 2C 7D D6 AB
 *
 *    e) Send the below packet on Community port_1.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=101
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      65 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 6B 53 25 A9
 *
 *    f)Expected Result
 *    ==================
 *      The below packet egresses on promiscuous port and Community port_2.
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=100
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      64 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 0C 0B E4 6C
 *
 *      Packet:
 *      ======
 *      DA=00:00:00:00:11:11
 *      SA=00:00:00:00:AA:AA
 *      Vlan=101
 *
 *      00 00 00 00 11 11 00 00 00 00 AA AA 81 00 00
 *      65 FF FF 88 E7 00 00 FA CE 00 00 00 00 11 22
 *      00 00 00 00 AA AA 81 00 0F A0 00 01 02 03 04
 *      05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13
 *      14 15 16 17 6B 53 25 A9
 */

  bshell(unit, "clear c");

  printf("\nSending packet to promiscuous_port:%d\n", promiscuous_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA000000001111810000640800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D4B25176E; sleep quiet 1", promiscuous_port);
  bshell(unit, str);
  bshell(unit, "show c");


  if (BCM_E_NONE != ingress_port_setup(unit, isolated_port_1)) {
    printf("ingress_port_setup() failed for port %d\n", isolated_port_1);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, promiscuous_port)) {
    printf("egress_port_setup() failed for port %d\n", promiscuous_port);
    return -1;
  }

  bshell(unit, "l2 clear vlanid=100");
  bshell(unit, "l2 clear vlanid=101");
  bshell(unit, "l2 clear vlanid=102");

  printf("\nSending packet to Isolated_port:%d\n", isolated_port_1);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000111100000000AAAA81000066FFFF88E70000FACE00000000112200000000AAAA81000FA0000102030405060708090A0B0C0D0E0F10111213141516176B5325A9; sleep quiet 1", isolated_port_1);
  bshell(unit, str);
  bshell(unit, "show c");

  if (BCM_E_NONE != ingress_port_setup(unit, community_port_1)) {
    printf("ingress_port_setup() failed for port %d\n", community_port_1);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, isolated_port_1)) {
    printf("egress_port_setup() failed for port %d\n", isolated_port_1);
    return -1;
  }

  bshell(unit, "l2 clear vlanid=100");
  bshell(unit, "l2 clear vlanid=101");
  bshell(unit, "l2 clear vlanid=102");

  printf("\nSending packet to Community_port:%d\n", community_port_1);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000111100000000AAAA81000065FFFF88E70000FACE00000000112200000000AAAA81000FA0000102030405060708090A0B0C0D0E0F10111213141516176B5325A9; sleep quiet 1", community_port_1);
  bshell(unit, str);
  bshell(unit, "show c");

  return BCM_E_NONE;
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
  bshell(unit, "config show; a ; version");
  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }

  if (BCM_FAILURE((rv = pvlan_config(unit)))) {
    printf("pvlan_config() failed.\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


