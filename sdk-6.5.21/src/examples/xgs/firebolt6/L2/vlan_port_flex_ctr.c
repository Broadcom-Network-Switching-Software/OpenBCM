/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : VLAN Flex Counters
 *
 *  Usage    : BCM.0> cint vlan_port_flex_ctr.c
 *
 *  config   : l2_config.bcm
 *
 *  Log file : vlan_port_flex_ctr_log.txt
 *
 *  Test Topology :
 *                  +------------------------+
 *                  |                        |
 *    ingress_port1 |                        |
 *    +-------------+                        |
 *                  |      SWITCH            |
 *    ingress_port2 |                        |
 *    +-------------+                        |
 *                  |                        |
 *                  |                        |
 *                  +------------------------+
 *
 *  Summary:
 *  ========
 *      This CINT example configures flex counters using BCM APIs.
 *
 *      Flex counters provide common counter resource that can be assigned for
 *      different modules.The packet attributes on which the counters can be
 *      configured are also flexible.They can be configured either in Fixed mode
 *      or Custom mode. The following example shows Vlan flex stats in custom
 *      mode and Port flex stats in fixed mode.
 *
 *      vlan flex stats [custom mode]
 *      ===============================
 *      Accountung object : Vlan [5]
 *      Attributes        : Outer tag [offset 0] and untag [offset 1]
 *      Stage             : Ingress specified through flag BCM_STAT_GROUP_MODE_INGRESS
 *                          of bcm_stat_group_mode_id_create
 *
 *      port flex stats [fixed mode]
 *      ===============================
 *      Accounting object : ingress_port2
 *      GroupMode         : TrafficType [unicast counter at offset 0
 *                                       multicast counter at offset 1
 *                                       broadcast counter at offset 2]
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects two ingress ports and configure them in Loopback mode.
 *         Install a rule to copy incoming packets to CPU and start packet watcher.
 *
 *    2) Step2 - Configuration (Done in vlan_port_flex_ctr())
 *    =======================================================
 *      a) Creates a VLAN(5).
 *      b) Create a custom stat group mode and create a stat group using the
 *         created "custom stat group mode".
 *      c) Attach the stat group to VLAN.
 *      d) Create another stat group using existing fixed group and attach it to port.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Send 3 outer tagged packet, retrieve vlan flex counters and print them.
 *
 *      b) Send 2 untagged packets, retrieve vlan flex counters and print them.
 *
 *      c) Send 2 broadcast packets onto ingress_port2, retrieve port
 *         flex counters and print them.
 *
 *      d) Send 3 multicast packets onto ingress_port2, retrieve port
 *         flex counters and print them.
 *
 *      e) Send 4 unicast packets onto ingress_port2, retrieve port
 *        flex counters and print them.
 *
 *      f) Expected Result:
 *      ===================
 *        After step 3.a, verify(from generated console log) that packet counter
 *        at offset 0 (outer tagged packets) is 3.
 *        After step 3.b, verify(from generated console log) that packet counter
 *        at offset 1 (untagged packets) is 2.
 *        After step 3.c, verify(from generated console log) that packet counter
 *        at offset 2 (broadcast packets) is 2.
 *        After step 3.d, verify(from generated console log) that packet counter
 *        at offset 1 (multicast packets) is 3.
 *        After step 3.e, verify(from generated console log) that packet counter
 *        at offset 0 (unicast packets) is 4.
 */
cint_reset();

bcm_port_t ingress_port1, ingress_port2;

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
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU and drop the packets. This is
 * to avoid continuous loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}
/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[2], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port1 = port_list[0];
  ingress_port2 = port_list[1];

  for (i = 0; i < 2; i++) {
    if (BCM_E_NONE != ingress_port_setup(unit, port_list[i])) {
      printf("ingress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}
/* Create vlan and add port to vlan */
int
configure_vlan(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in creating vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if(BCM_FAILURE(rv)) {
        printf("\nError executing bcm_vlan_port_add(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Configure flex counters for Vlan in Custom mode*/
int
configure_vlan_flex_ctr(int unit, bcm_vlan_t vlan)
{
    uint32 mode_id;
    bcm_stat_group_mode_attr_selector_t attr_sel[2];
    uint32 num_sel = 2;
    uint32 vlan_counters = 2;
    uint32 stat_id;
    uint32 num_entries;
    bcm_error_t rv;

    /*
     * Customized group mode is defined using a set of bcm_stat_group_mode_attr_selectors.
     * To uniquely specify a counter, below three properties are needed
     * Attribute selector[in this case bcmStatGroupModeAttrVlan],
     * Attribute Value to match [Outer tag and untagged]
     * and Offset [Outer tag at offset '0' and untag at offset '1']
     */

    bcm_stat_group_mode_attr_selector_t_init(&attr_sel[0]);
    attr_sel[0].counter_offset = 0;
    attr_sel[0].attr = bcmStatGroupModeAttrVlan;
    attr_sel[0].attr_value = bcmStatGroupModeAttrVlanOuterTag;

    bcm_stat_group_mode_attr_selector_t_init(&attr_sel[1]);
    attr_sel[1].counter_offset = 1;
    attr_sel[1].attr = bcmStatGroupModeAttrVlan;
    attr_sel[1].attr_value = bcmStatGroupModeAttrVlanUnTagged;

    /* Create Customized Stat Group mode for given Counter Attributes */
    rv = bcm_stat_group_mode_id_create(unit, BCM_STAT_GROUP_MODE_INGRESS,
                                       vlan_counters, num_sel, attr_sel, &mode_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in creating stat group mode: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Associating accounting object[vlan] to Custom Stat Group mode */
    rv = bcm_stat_custom_group_create(unit, mode_id, bcmStatObjectIngVlan,
                                      &stat_id, &num_entries);
    if(BCM_FAILURE(rv)) {
        printf("\nError in creating stat group : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Attaching the counter entries to specific Vlan[5] */
    rv =  bcm_vlan_stat_attach(unit, vlan, stat_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in vlan stat attach %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Configure flex counters for Port in Fixed mode */
int
configure_port_flex_ctr(int unit, bcm_port_t port)
{
    bcm_error_t rv;
    uint32 stat_id;
    uint32 num_entries;
    bcm_gport_t gport;

    /* Create Stat Group */
    rv = bcm_stat_group_create(unit, bcmStatObjectIngPort, bcmStatGroupModeTrafficType,
                               &stat_id, &num_entries);
    if(BCM_FAILURE(rv)) {
        printf("\nError in stat group creation %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Counter Id is %d\n", stat_id);

    rv = bcm_port_gport_get(unit,port, &gport);
    if(BCM_FAILURE(rv)) {
        printf("\nError in gport get %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Attaching the stats to the port */
    rv =  bcm_port_stat_attach(unit, gport, stat_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in port stat attach %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

int
vlan_flex_stat_get(bcm_vlan_t vlan)
{
    int unit = 0;
    int i;
    uint32 index[2] = {0, 1};
    bcm_stat_value_t value[2];
    uint32 num_entries = 2;
    bcm_vlan_stat_t stat[2] = {bcmVlanStatIngressPackets, bcmVlanStatIngressBytes};
    uint64 val;
    bcm_error_t rv;

    /*
     * COMPILER_64_HI and COMPILER_64_LO for 64-32 bit conversion
     */

    /* Vlan stat collection */
    for(i=0; i<2; i++) {
        rv = bcm_vlan_stat_counter_get(unit, vlan, stat[i], num_entries, index, value);
        if(BCM_FAILURE(rv)) {
            printf("\nError executing bcm_vlan_stat_counter_get(): %s.\n",bcm_errmsg(rv));
            return rv;
        }
        switch(stat[i]) {
                case bcmVlanStatIngressPackets :
                    printf("Packet counter at offset 0 (outer tagged packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[0].packets64),
                            COMPILER_64_LO(value[0].packets64));
                    printf("Packet counter at offset 1 (untag packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[1].packets64),
                            COMPILER_64_LO(value[1].packets64));
                    break;

                case bcmVlanStatIngressBytes :
                    printf("Byte counter at offset 0 (for outer tagged packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[0].bytes),
                            COMPILER_64_LO(value[0].bytes));
                    printf("Byte counter at offset 1 (for untag packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[1].bytes),
                            COMPILER_64_LO(value[1].bytes));
                    break;
            }
    }

    return BCM_E_NONE;
}

int
port_flex_stat_get(bcm_port_t port)
{
    int unit = 0;
    int i,j;
    bcm_gport_t gport;
    uint32 index[3] = {0, 1, 2};
    bcm_stat_value_t value[3];
    uint32 num_entries = 3;
    bcm_port_stat_t stat[2] = {bcmPortStatIngressPackets, bcmPortStatIngressBytes};
    uint64 val;
    bcm_error_t rv;

    rv = bcm_port_gport_get(0, port, &gport);
    if(BCM_FAILURE(rv)) {
        printf("\nError in gport get: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /*
     * COMPILER_64_HI and COMPILER_64_LO for 64-32 bit conversion
     */

    /* Port stat collection */
    for(i = 0; i < 2; i++) {
        rv = bcm_port_stat_counter_get(unit, gport, stat[i], num_entries,
                                       index, value);
        if(BCM_FAILURE(rv)) {
            printf("\nError in port counter get: %s.\n",bcm_errmsg(rv));
            return rv;
        }

        switch(stat[i]) {
                case bcmVlanStatIngressPackets :
                    printf("Packet counter at offset 0 (unicast packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[0].packets64),
                            COMPILER_64_LO(value[0].packets64));
                    printf("Packet counter at offset 1 (multicast packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[1].packets64),
                            COMPILER_64_LO(value[1].packets64));
                    printf("Packet counter at offset 2 (broadcast packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[2].packets64),
                            COMPILER_64_LO(value[2].packets64));
                    break;

                case bcmVlanStatIngressBytes :
                    printf("Byte counter at offset 0 (for unicast packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[0].bytes),
                            COMPILER_64_LO(value[0].bytes));
                    printf("Byte counter at offset 1 (for multicast packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[1].bytes),
                            COMPILER_64_LO(value[1].bytes));
                    printf("Byte counter at offset 2 (for broadcast packets) is 0x%x%x\n",
                            COMPILER_64_HI(value[2].bytes),
                            COMPILER_64_LO(value[2].bytes));
                    break;
            }
        }

    return BCM_E_NONE;
}

void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

 /* Send 3 outer tagged packet.
 *
 *      Ethernet header: DA=00:00:00:00:00:02, SA=00:00:00:00:00:01
 *      Vlan Header: TPID0x8100, VLAN=5, Priority=0, CFI=0
 *
 *      000000000002 000000000001
 *      8100
 *      0005
 *      0800080045000062000000004011A4D7C0A801010A0A0A01003F2118004E000008000000012345000000000011AA00006DBAC047
 */
  printf("Sending 3 outer tagged packets.\n");
  snprintf(str, 512, "tx 3 pbm=%d data=0x000000000002000000000001810000050800080045000062000000004011A4D7C0A801010A0A0A01003F2118004E000008000000012345000000000011AA00006DBAC047; sleep 1", ingress_port1);
  bshell(unit, str);

  /*Retrieve vlan flex counters and print them.*/
  printf("Retrieving vlan flex counters and displaying them\n");
  vlan_flex_stat_get(5);

 /* Send 2 untagged packets.
 *      Ethernet header: DA=00:00:00:00:00:02, SA=00:00:00:00:00:01
 *
 *      000000000002 000000000001
 *      000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2FDF52E538
 */
  printf("Sending 2 untagged packets.\n");
  snprintf(str, 512, "tx 2 pbm=%d data=0x000000000002000000000001000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2FDF52E538; sleep 1", ingress_port1);
  bshell(unit, str);

  /*Retrieve vlan flex counters and print them.*/
  printf("Retrieving vlan flex counters and displaying them\n");
  vlan_flex_stat_get(5);

 /* Send 2 broadcast packets onto ingress_port2.
 *      Ethernet header: DA=FF:FF:FF:FF:FF:FF, SA=00:00:00:00:00:10
 *      Vlan Header: TPID0x8100, VLAN=2, Priority=1, CFI=0
 *
 *      FFFFFFFFFFFF 000000000010
 *      8100
 *      2002
 *      0800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D58093531
 */
  printf("Sending 2 broadcast packets onto ingress_port2:%d\n", ingress_port2);
  snprintf(str, 512, "tx 2 pbm=%d data=0xFFFFFFFFFFFF000000000010810020020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D58093531; sleep 1", ingress_port2);
  bshell(unit, str);
  /*Retrieve port flex counters and print them.*/
  printf("Retrieving port flex counters and displaying them\n");
  port_flex_stat_get(ingress_port2);

 /* Sending 3 multicast packets onto ingress_port2.
 *      Ethernet header: DA=01:00:04:00:01:00, SA=00:00:04:00:00:00
 *      Vlan Header: TPID0x8100, VLAN=2, Priority=0, CFI=0
 *
 *      010004000100 000004000000
 *      8100
 *      0002
 *      0800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D8E0BEC9E
 */
  printf("Sending 3 multicast packets onto ingress_port2:%d\n", ingress_port2);
  snprintf(str, 512, "tx 3 pbm=%d data=0x010004000100000004000000810000020800000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D8E0BEC9E; sleep 1", ingress_port2);
  bshell(unit, str);
  /*Retrieve port flex counters and print them.*/
  printf("Retrieving port flex counters and displaying them\n");
  port_flex_stat_get(ingress_port2);
 /* Sending 4 unicast packets onto ingress_port2.
 *      Ethernet header: DA=00:00:00:00:00:02, SA=00:00:00:00:00:01
 *      Vlan Header: TPID0x8100, VLAN=2, Priority=6, CFI=0
 *
 *      000000000002 000000000001
 *      8100
 *      C002
 *      08004500002E0000000040FF63BC010101010A0A0A0A000102030405060708090A0B0C0D0E0F101112131415161718194FA8E232
 */
  printf("Sending 4 unicast packets onto ingress_port2:%d\n", ingress_port2);
  snprintf(str, 512, "tx 4 pbm=%d data=0x0000000000020000000000018100C00208004500002E0000000040FF63BC010101010A0A0A0A000102030405060708090A0B0C0D0E0F101112131415161718194FA8E232; sleep 1", ingress_port2);
  bshell(unit, str);
  /*Retrieve port flex counters and print them.*/
  printf("Retrieving port flex counters and displaying them\n");
  port_flex_stat_get(ingress_port2);

}
/* Creates a VLAN(5) and attaches a flex counter to it.
 * Attaches a flex counter to ingress_port2.
 */
int
vlan_port_flex_ctr(int unit)
{
    bcm_error_t rv;
    bcm_vlan_t vlan = 5;  /* Vlan for which Stats are collected */
    bcm_vlan_t vlan1 = 2;
    bcm_port_t vlan_port = ingress_port1;


    rv = configure_vlan(unit, vlan, ingress_port1);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_untagged_vlan_set(unit, ingress_port1, vlan);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring port_untagged_vlan: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = configure_vlan_flex_ctr(unit, vlan);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring flex counter for vlan: %s.\n",
                bcm_errmsg(rv));
        return rv;
    }

    rv = configure_vlan(unit, vlan1, ingress_port2);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = configure_port_flex_ctr(unit, ingress_port2);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring flex counter for port: %s.\n",
                bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in vlan_port_flex_ctr())
 * c)demonstrates the functionality(done in verify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");
  if (BCM_FAILURE((rv = vlan_port_flex_ctr(unit)))) {
    printf("vlan_port_flex_ctr() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing vlan_port_flex_ctr()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
