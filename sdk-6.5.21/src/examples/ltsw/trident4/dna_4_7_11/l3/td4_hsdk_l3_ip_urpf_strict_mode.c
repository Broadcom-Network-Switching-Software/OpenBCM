/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IP Urpf Strict mode
 *
 * Usage    : BCM.0> cint td4_hsdk_l3_ip_urpf_strict_mode.c 
 *
 * config   : bcm56880_a0-generic-32x400.config.yml 
 *
 * Log file : td4_hsdk_l3_ip_urpf_strict_mode_log.txt
 *
 * Test Topology :
 *
 *                    +------------------------------+
 * ingress_port_1 cd0 |                              |  
 * +------------------+                              |  DMAC 0xfe02 
 * VLAN 11            |                              |  SMAC 0x02 
 * DMAC 0x1           |                              |  egress_port cd2 
 * DIP 10.58.66.115   |          SWITCH-TD4          +-----------------+   
 * SIP_E 10.58.64.1   |                              |  VLAN 13 
 * SIP_U 10.58.96.1   |                              |  DIP 10.58.66.115 
 * VLAN 12            |                              |  SIP 10.58.64.1 
 * +------------------+                              |
 * ingress_port_2 cd1 |                              |
 *                    +------------------------------+
 *                                                                        
 * Summary:
 * ========
 *   This Cint example to show configuration of the IP Urpf Strict mode
 *   BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        threee ports, two ports are used as ingress_ports and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming and outgoing packets to CPU and
 *        start packet watcher to display packet contents.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in configure_ip_urpf_mode()):
 *   =========================================================
 *      a) Create ingress_vlan_1 and add ingress_port_1 as member.
 *
 *      b) Create ingress_vlan_2 and add ingress_port_2 as member.
 *
 *      c) Create egress_vlan and add egress_port as member.
 *
 *      d) Create ingress interface with Losse mode and egress interface for ingress interface_1 
 *         and bind ingress interface with ingress vlan_1.
 *
 *      e) Create ingress interface with Losse mode and egress interface for ingress interface_2 
 *         and bind ingress interface with ingress vlan_2.
 *
 *      f) Create ingress interface with Losse mode and egress interface for egress interface 
 *         and bind ingress interface with egress vlan.
 *
 *      g) Create egress object for ingress interface_1. 
 *
 *      h) Create egress object for ingress interface_2. 
 *
 *      i) Create egress object for egress interface. 
 *
 *      j) Add host for SIP with ingress interface_1.
 *
 *      k) Add host for DIP with egress interface.
 *
 *      l) Create station MAC entry for L3 forwarding.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *      a) Send below IPv4 packet on ingress_port_1 with expected SIP.
 *
 *         DMAC 0x1
 *         VLAN 11 
 *         DIP 10.58.66.115
 *         SIP 10.58.64.1
 *         Ingress PORT cd0 
 *
 *         0000 0000 0001 002a 1077 7700 8100 000b
 *         0800 4500 003c 6762 0000 ff01 bd76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *      b) Send below IPv4 packet on ingress_port_1 with Unexpected SIP.
 *
 *         DMAC 0x1
 *         VLAN 11
 *         DIP 10.58.66.115
 *         SIP 10.58.96.1
 *         Ingress PORT cd0
 *
 *         0000 0000 0001 002a 1077 7700 8100 000b 
 *         0800 4500 003c 6762 0000 ff01 9d76 0a3a
 *         6001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *      c) Send below IPv4 packet on ingress_port_2 with Unexpected L3_IIF.
 *
 *         DMAC 0x1
 *         VLAN 12
 *         DIP 10.58.66.115
 *         SIP 10.58.64.1
 *         Ingress PORT cd1
 *
 *         0000 0000 0001 002a 1077 7700 8100 000c
 *         0800 4500 003c 6762 0000 ff01 bd76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *     c) Expected Result:
 *     ===================
 *        After step 3.a, {DIP hit, SIP hit, L3_IIF = L3_OIF} met
 *           Results: Urpf check pass so packet L3 forwarded
 *
 *         DMAC 0xfe02
 *         SMAC 0x02
 *         VLAN 13
 *         DIP 10.58.66.115
 *         SIP 10.58.64.1
 *         Egress PORT cd2
 *
 *         0000 0000 fe02 0000 0000 0002 8100 000d
 *         0800 4500 003c 6762 0000 fe01 be76 0a3a
 *         4001 0a3a 4273 0000 550a 0001 0051 6162
 *         6364 6566 6768 696a 6b6c 6d6e 6f70 7172
 *         7374 7576 7761 6263 6465 6667 6869
 *
 *        After step 3.b, {DIP hit, SIP miss} met
 *           Results: Urpf check fail so packet drop at ingress 
 *
 *        After step 3.c, {DIP hit, SIP hit, L3_IIF != L3_OIF} met
 *           Results: Urpf check fail so packet drop at ingress 
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port_1;
bcm_port_t    ingress_port_2;
bcm_port_t    egress_port;
bcm_gport_t   ingress_gport_1;
bcm_gport_t   ingress_gport_2;
bcm_gport_t   egress_gport;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t        ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
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
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config; 

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = port;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("ingress_port_setup configured for ingress_port : %d\n", port);

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config; 

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = port;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("egress_port_setup configured for egress_port : %d\n", port);

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[3];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port_1 = port_list[0];
    ingress_port_2 = port_list[1];
    egress_port    = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port_1)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port_1);
        return -1;
    }

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port_2)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port_2);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l3 ing_intf show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 host show");
    bshell(unit, "clear c");

    printf("\nIP Urpf Strict mode check ====> Expected SIP\n");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port_1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000b08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port_1);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nIP Urpf Strict mode check ====> Uxpected SIP\n");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port_1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000b08004500003c67620000ff019d760a3a60010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port_1);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nIP Urpf Strict mode check ====> Uxpected L3_IIF\n");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port_2);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a107777008100000c08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869; sleep quiet 1", ingress_port_2);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

/*
 * Configure IP Urpf Strict mode functionality.
 */
int
configure_ip_urpf_mode(int unit)
{
  bcm_error_t     rv = BCM_E_NONE;
  bcm_vlan_t      ingress_vlan_1 = 11;
  bcm_vlan_t      ingress_vlan_2 = 12;
  bcm_vlan_t      egress_vlan = 13;
  bcm_vrf_t       vrf = 1;

  bcm_mac_t       router_mac_in_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  bcm_mac_t       router_mac_in_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
  bcm_mac_t       router_mac_out = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
  bcm_mac_t       mac_in_1 = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x01};
  bcm_mac_t       mac_in_2 = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x03};
  bcm_mac_t       mac_out = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
  bcm_mac_t       mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  bcm_ip_t        dip = 0x0a3a4273;  /* 10.58.66.115 */
  bcm_ip_t        sip = 0x0a3a4001;  /* 10.58.64.1 */ 

  bcm_if_t        l3_egr_if_in_1 = 100;
  bcm_if_t        l3_egr_if_in_2 = 200;
  bcm_if_t        l3_egr_if_out = 300;

  bcm_if_t        ingress_if_in_1 = 100;
  bcm_if_t        ingress_if_in_2 = 200;
  bcm_if_t        ingress_if_out = 300;

  bcm_l3_intf_t    l3_intf_in_1;
  bcm_l3_intf_t    l3_intf_in_2;
  bcm_l3_intf_t    l3_intf_out;
  bcm_l3_ingress_t l3_ingress;
  bcm_l3_host_t    host;
  bcm_l2_station_t l2_station;
  int              station_id;

  bcm_vlan_control_vlan_t vlan_ctrl;
  bcm_l3_egress_t         l3_egress;
  bcm_if_t                egr_obj_in_1;
  bcm_if_t                egr_obj_in_2;
  bcm_if_t                egr_obj_out;

  rv = bcm_port_gport_get(unit, ingress_port_1, &ingress_gport_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, ingress_port_2, &ingress_gport_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port, &egress_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create ingress vlan_1 */
  rv = bcm_vlan_create(unit, ingress_vlan_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress port_1 member of ingress vlan_1 */
  rv = bcm_vlan_gport_add(unit, ingress_vlan_1, ingress_gport_1, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create ingress vlan_2 */
  rv = bcm_vlan_create(unit, ingress_vlan_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress port_2 member of ingress vlan_2 */
  rv = bcm_vlan_gport_add(unit, ingress_vlan_2, ingress_gport_2, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress vlan */
  rv = bcm_vlan_create(unit, egress_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress port member of egress vlan */
  rv = bcm_vlan_gport_add(unit, egress_vlan, egress_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for ingress interface_1 */
  bcm_l3_intf_t_init(&l3_intf_in_1);
  l3_intf_in_1.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_in_1.l3a_intf_id = l3_egr_if_in_1;
  l3_intf_in_1.l3a_vid     = ingress_vlan_1;
  sal_memcpy(l3_intf_in_1.l3a_mac_addr, router_mac_in_1, sizeof(router_mac_in_1));
  rv = bcm_l3_intf_create(unit, &l3_intf_in_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for ingress interface_2 */
  bcm_l3_intf_t_init(&l3_intf_in_2);
  l3_intf_in_2.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_in_2.l3a_intf_id = l3_egr_if_in_2;
  l3_intf_in_2.l3a_vid     = ingress_vlan_2;
  sal_memcpy(l3_intf_in_2.l3a_mac_addr, router_mac_in_2, sizeof(router_mac_in_2));
  rv = bcm_l3_intf_create(unit, &l3_intf_in_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress interface */
  bcm_l3_intf_t_init(&l3_intf_out);
  l3_intf_out.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_out.l3a_intf_id = l3_egr_if_out;
  l3_intf_out.l3a_vid     = egress_vlan;
  sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out, sizeof(router_mac_out));
  rv = bcm_l3_intf_create(unit, &l3_intf_out);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 ingress interface for ingress interface_1 with same l3_egr_if_in_1 & ingress_if_in_1 */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags     = BCM_L3_INGRESS_WITH_ID;
  l3_ingress.vrf       = vrf;
  l3_ingress.urpf_mode = bcmL3IngressUrpfStrict; /* IP URPF Strict mode */
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_in_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config ingress vlan_1 to L3_IIF binding */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  vlan_ctrl.ingress_if = ingress_if_in_1;
  rv = bcm_vlan_control_vlan_selective_set(unit, ingress_vlan_1, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_selective_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 ingress interface for ingress interface_2 with same l3_egr_if_in_2 & ingress_if_in_2 */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags     = BCM_L3_INGRESS_WITH_ID;
  l3_ingress.vrf       = vrf;
  l3_ingress.urpf_mode = bcmL3IngressUrpfStrict; /* IP URPF Strict mode */
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_in_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config ingress vlan_2 to L3_IIF binding */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  vlan_ctrl.ingress_if = ingress_if_in_2;
  rv = bcm_vlan_control_vlan_selective_set(unit, ingress_vlan_2, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_selective_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 ingress interface for egress interface with same l3_egr_if_out & ingress_if_out */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags     = BCM_L3_INGRESS_WITH_ID;
  l3_ingress.vrf       = vrf;
  l3_ingress.urpf_mode = bcmL3IngressUrpfStrict; /* IP URPF Strict mode */ 
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_out);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config egress vlan to L3_IIF binding */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  vlan_ctrl.ingress_if = ingress_if_out;
  rv = bcm_vlan_control_vlan_selective_set(unit, egress_vlan, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_selective_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for ingress interface_1 */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = ingress_gport_1;
  l3_egress.intf  = l3_egr_if_in_1;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_in_1, sizeof(mac_in_1));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_in_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for ingress interface_2 */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = ingress_gport_2;
  l3_egress.intf  = l3_egr_if_in_2;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_in_2, sizeof(mac_in_2));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_in_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for egress interface */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = egress_gport;
  l3_egress.intf  = l3_egr_if_out;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_out, sizeof(mac_out));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_out);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Install the route for SIP */
  bcm_l3_host_t_init(&host);
  host.l3a_intf    = egr_obj_in_1;
  host.l3a_ip_addr = sip;
  host.l3a_vrf     = vrf;
  rv = bcm_l3_host_add(unit, &host);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_host_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Install the route for DIP */
  bcm_l3_host_t_init(&host);
  host.l3a_intf    = egr_obj_out;
  host.l3a_ip_addr = dip;
  host.l3a_vrf     = vrf;
  rv = bcm_l3_host_add(unit, &host);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_host_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config my station TCAM */
  bcm_l2_station_t_init(&l2_station);
  sal_memcpy(l2_station.dst_mac, router_mac_in_1, sizeof(router_mac_in_1));
  sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
  rv = bcm_l2_station_add(unit, &station_id, &l2_station);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l2_station_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in configure_ipuc_forwarding())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; a ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) configure_ip_urpf_mode(): ** start **";
    if (BCM_FAILURE((rv = configure_ip_urpf_mode(unit)))) {
        printf("configure_ip_urpf_mode() failed.\n");
        return -1;
    }
    print "~~~ #2) configure_ip_urpf_mode(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}

