
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Inband Telemetry
 *
 * Usage    : BCM.0> cint int.c
 *
 * config   : int.bcm
 *
 * Log file : int_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *     port_acc     |                              |  port_network
 * +----------------+          SWITCH              +-----------------+
 *                  |                              | packet egress with
 *                  |                              | INT header
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   This Cint example to show configuration of INT using BCM APIs
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Mirror both ingress and egress ports to CPU as MTP and start
 *        packet watcher.
 *
 *     Note: Mirrorinf is meant for testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in configure_INT()):
 *   =========================================================
 *     a) Configure INT feature. It enables INT on port and ingress interface
 *        does the necessary device configurations through switch control and
 *        port controls.
 *     b) IFP can be used to override pipeline decision which is mostly done
 *        for specific SrcIP. This feature of IFP is shown by disabling INT
 *        encap.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show', 'l3 l3table show' and 'fp show'
 *
 *     b) Transmit the known probe packet. The contents of the packet
 *        are printed on screen. The packet used matches the INT configurations
 *        done through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port and also INT header is added.
 */


cint_reset();

bcm_port_t port_acc;
bcm_port_t port_network;

/* INT configuration */
int int_udp_port = 0x1a6f;
int max_payload_len = 1024;
int int_pm1 = 0xaaaaaaaa;
int int_pm2 = 0xbbbbbbbb;
int int_ver = 0x1;
int int_req_vec_val = 0xffffffff;
int int_req_vec_mask = 0xffffffff;
int int_switch_id = 0x900ddeed;


/*
 * This function checks if given port is already present
 * in the list so that same port number is not generated again
 */
int
checkPortAssigned(int *port_index_list,int no_entries, int index)
{
  int i=0;

  for (i= 0; i < no_entries; i++) {
    if (port_index_list[i] == index)
      return 1;
  }

  /* no entry found */
  return 0;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
int
portNumbersGet(int unit, int *port_list, int num_ports)
{
  int i = 0, port = 0,rv = 0, index = 0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;
  int tempports[BCM_PBMP_PORT_MAX];
  int port_index_list[num_ports];

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
      tempports[port] = i;
      port++;
    }
  }

  if (( port == 0 ) || ( port < num_ports )) {
      printf("portNumbersGet() failed \n");
      return -1;
  }

  /* generating random ports */
  for (i= 0; i < num_ports; i++) {
    index = sal_rand()% port;
    if (checkPortAssigned(port_index_list, i, index) == 0)
    {
      port_list[i] = tempports[index];
      port_index_list[i] = index;
    } else {
      i = i - 1;
    }
  }

  return BCM_E_NONE;
}

int
switch_control_set(int unit)
{
    bcm_error_t rv;

    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for  bcmSwitchL3EgressMode: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchIntL4DestPort1, int_udp_port);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for bcmSwitchIntL4DestPort1: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchIntProbeMarker1, int_pm1);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for INT header ProbeMarker1: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchIntProbeMarker2, int_pm2);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for INT header ProbeMarker2: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchIntVersion, int_ver);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for INT header version: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchIntRequestVectorValue, int_req_vec_val);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for INT header request vector: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchIntRequestVectorMask, int_req_vec_mask);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for INT header request vector mask: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchIntSwitchId, int_switch_id);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_set for INT payload switch ID: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    return BCM_E_NONE;
}

int
port_control_set(int unit)
{
    bcm_error_t rv;

    rv = bcm_port_control_set(unit, port_acc, bcmPortControlIntEnable, 1);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_port_control_set for bcmPortControlIntEnable: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_port_control_set(unit, port_network, bcmPortControlIntEnable, 1);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_port_control_set for bcmPortControlIntEnable: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_port_set(unit, port_acc, bcmSwitchL3IngressMode, 1);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_port_set for bcmSwitchL3IngressMode: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_switch_control_port_set(unit, port_network, bcmSwitchL3IngressMode, 1);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_switch_control_port_set for bcmSwitchL3IngressMode: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    return BCM_E_NONE;
}

int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port, bcm_gport_t *gport)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, upbmp;

    rv = bcm_port_gport_get(unit, port, gport);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_port_gport_get() for port %d : %s.\n", port, bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_vlan_create(unit, vlan);
    if (BCM_FAILURE(rv) & (rv != BCM_E_EXISTS)) {
            printf("Error in bcm_vlan_create() for vlan %d: %s.\n", vlan, bcm_errmsg(rv));
            return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vlan, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_vlan_port_add() for vlan %d, port %d: %s.\n", vlan, port, bcm_errmsg(rv));
            return rv;
    }

    return BCM_E_NONE;
}

/* Create L3 interface */
int
create_l3_intf_my_station(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t l3_intf_id, bcm_vrf_t vrf)
{
    bcm_error_t rv;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags =       BCM_L3_WITH_ID | BCM_L3_ADD_TO_ARL;
    l3_intf.l3a_intf_id =     l3_intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));
    l3_intf.l3a_vid =         vlan;
    l3_intf.l3a_vrf =         vrf;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_l3_intf_create() for my station for vlan %d vrf %d : %s.\n", vlan, vrf, bcm_errmsg(rv));
            return rv;
    }

    return BCM_E_NONE;
}

/* Create L3 ingress interface */
int
create_l3_ingress_intf(int unit, bcm_vlan_t vlan, bcm_if_t * l3_intf_id, bcm_vrf_t vrf)
{
    bcm_error_t rv;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE | BCM_L3_INGRESS_INT;
    l3_ingress.vrf = vrf;
    l3_ingress.ipmc_intf_id  = vlan;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, l3_intf_id);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_l3_ingress_create() for vlan %d: %s.\n", vlan, bcm_errmsg(rv));
            return rv;
    }

    return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport, bcm_vlan_t vlan, bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    l3_egr.vlan = vlan;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));
    rv = bcm_l3_egress_create(unit, 0, &l3_egr, egr_if);

    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_l3_egress_create() for egress if.  %s.\n",  bcm_errmsg(rv));
            return rv;
    }

    return BCM_E_NONE;
}

int
l3_host_config(int unit, bcm_vrf_t vrf, bcm_if_t egr_if, bcm_ip_t dest_ip)
{
    bcm_error_t rv;
    bcm_l3_host_t hinfo;

    bcm_l3_host_t_init(hinfo);
    hinfo.l3a_vrf     = vrf;
    hinfo.l3a_intf    = egr_if;
    hinfo.l3a_ip_addr = dest_ip;
    rv = bcm_l3_host_add(unit, &hinfo);

    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_l3_host_add() for vrf %d: %s.\n", vrf, bcm_errmsg(rv));
            return rv;
    }

    return BCM_E_NONE;
}

int
time_config_set(int unit)
{

    bcm_time_init(unit);
    bcm_time_interface_t intf;
    intf.id = 0;
    intf.flags |= BCM_TIME_ENABLE | BCM_TIME_WITH_ID;
    bcm_error_t rv;

    /* Enable timestamping */
    rv = bcm_time_interface_add(unit,intf);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_time_interface_add: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    uint64 ctl;
    rv = bcm_port_control_phy_timesync_set(unit, port_acc, bcmPortControlPhyTimesyncOneStepEnable, ctl);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_port_control_phy_timesync_set - port_acc: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    rv = bcm_port_control_phy_timesync_set(unit, port_network, bcmPortControlPhyTimesyncOneStepEnable, ctl);
    if (BCM_FAILURE(rv)) {
            printf("Error in bcm_port_control_phy_timesync_set - port_ network: %s.\n", bcm_errmsg(rv));
            return rv;
    }

    bcm_time_ts_counter_t counter;
    bcm_time_tod_t tod;
    uint64 secs, nano_secs, div;
    uint32 stages;

    bcm_time_ts_counter_get(unit, &counter);
    stages = BCM_TIME_STAGE_EGRESS;
    tod.time_format = counter.time_format;
    tod.ts_adjustment_counter_ns = counter.ts_counter_ns;

    /* Get seconds from the timestamp value */
    secs = counter.ts_counter_ns;
    COMPILER_64_SET(div, 0, 1000000000);
    COMPILER_64_UDIV_64(secs, div);
    tod.ts.seconds = secs;

    /* Get nanoseconds from the timestamp value */
    nano_secs = counter.ts_counter_ns;
    COMPILER_64_UMUL_32(secs, 1000000000);
    COMPILER_64_SUB_64(nano_secs, secs);
    tod.ts.nanoseconds = COMPILER_64_LO(nano_secs);
    bcm_time_tod_set(unit, stages, &tod);

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports.
 */

int
test_setup(int unit)
{
    int port_list[2];
    bcm_error_t rv;
    char str[512];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    port_acc = port_list[0];
    port_network = port_list[1];

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_acc, BCM_PORT_LOOPBACK_MAC));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port_network, BCM_PORT_DISCARD_ALL));

    bshell(unit, "pw start report +raw +decode");

    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", port_acc);
    bshell(unit, str);

    snprintf(str, 512, "dmirror %d mode=ingress destport=cpu0", port_network);
    bshell(unit, str);

    return BCM_E_NONE;
}

int
configure_ifp(int unit, bcm_ip_t src_ip)
{
    bcm_field_group_config_t group;
    int gid = 1;
    bcm_field_entry_t entry;
    bcm_error_t rv;

    bcm_field_group_config_t_init(group);
    group.priority=1;
    group.group = gid;
    group.mode=bcmFieldGroupModeAuto;
    group.flags = BCM_FIELD_GROUP_CREATE_WITH_ID;

    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyL4DstPort);

    /* Assign ASET */
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionIntEncapDisable);

    group.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;

    /* Group create */
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcIp(unit, entry, src_ip, 0xFFFFFFFF));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_L4DstPort(unit, entry, int_udp_port, 0xFFFF));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionIntEncapDisable, 1, 1));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

int
configure_INT(int unit)
{
    bcm_error_t rv;
    bcm_mac_t remote_mac_network = "00:00:00:00:00:02";
    bcm_mac_t local_mac_network = "00:00:00:00:22:22";

    bcm_mac_t pkt_in_src_mac = "00:00:00:00:aa:aa";
    bcm_mac_t pkt_in_dst_mac = "00:00:00:00:bb:bb";

    bcm_vlan_t vid_acc = 21;
    bcm_vlan_t vid_network = 22;

    bcm_if_t intf_id_acc = 21;
    bcm_if_t intf_id_network = 22;

    bcm_vrf_t vrf = 10;

    bcm_gport_t gport_acc = BCM_GPORT_INVALID;
    bcm_gport_t gport_network = BCM_GPORT_INVALID;

    bcm_if_t egr_obj_network;
    bcm_if_t egr_obj_acc;

    bcm_ip_t pkt_in_dst_ip = 0x0a0a0200;
    bcm_ip_t pkt_in_src_ip = 0x0a0a0101;

    bcm_ip_t src_ip_not_int =0x0a0a010a;

    /* Common Configuration */
    rv = switch_control_set(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in switch_control_set: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = port_control_set(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in port_control_set: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = time_config_set(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in time_config_set: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* vlan creation and configuration */
    rv = create_vlan_add_port(unit, vid_acc, port_acc, &gport_acc);
    if (BCM_FAILURE(rv)) {
        printf("Error in  create_vlan_add_port - port_acc: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, vid_network, port_network, &gport_network);
    if (BCM_FAILURE(rv)) {
        printf("Error in  create_vlan_add_port - port_network: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* L3 routing setup */
    rv = create_l3_intf_my_station(unit, pkt_in_dst_mac, vid_acc, intf_id_acc, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_intf_my_station - pkt_dst_mac + vid_acc: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_ingress_intf(unit, vid_acc, &intf_id_acc, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_ingress_intf - vid_acc + intf_acc: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf_my_station(unit, local_mac_network, vid_network, intf_id_network, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_intf_my_station- local_mac + vid_network: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_ingress_intf(unit, vid_network, &intf_id_network, vrf);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_l3_ingress_intf - vid_netwok + intf_network: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_egr_obj(unit, intf_id_network, remote_mac_network, gport_network, vid_network, &egr_obj_network);
    if (BCM_FAILURE(rv)) {
        printf("Error in create_egr_obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = l3_host_config(unit, vrf, egr_obj_network, pkt_in_dst_ip);
    if (BCM_FAILURE(rv)) {
        printf("Error in l3_host_config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configuring IFP to disable INT encap for a specific SIP  */
    rv = configure_ifp(unit, src_ip_not_int);
    if (BCM_FAILURE(rv)) {
        printf("Error in configure_ifp: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 How to verify: It is documented within the function for various packets and options
*/
int
verify(int unit)
{
    char str[512];

    printf("vlan show\n");
    bshell(unit, "vlan show");

    printf("\nl2 show\n");
    bshell(unit, "l2 show");

    printf("\nl3 intf show\n");
    bshell(unit, "l3 intf show");

    printf("\nl3 egress show\n");
    bshell(unit, "l3 egress show");

    printf("\nl3 l3table show\n");
    bshell(unit, "l3 l3table show");

    printf("\nfp show\n");
    bshell(unit, "fp show");

    bshell(unit, "clear c");
    printf("\nSending probe packet on port_acc:0x%x and observe packet on port_network:0x%x\n", port_acc, port_network);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000bbbb00000000aaaa810000150800453a006e000000003f1164310a0a01010a0a0200003e1a6f005a0000aaaaaaaabbbbbbbb01010000ffffffffff0000001fe0000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132330001259ffb7a; sleep quiet 1", port_acc);
    bshell(unit, str);

    printf("\nshow c\n");
    bshell(unit, "show c");

    printf("\nSending probe packet on port_acc:0x%x and observe packet on port_network:0x%x carrying no INT header disabled by IFP\n", port_acc, port_network);
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000BBBB00000000AAAA81000015080045000072000000004011635E0A0A010A0A0A0200003E1A6F005E0000AAAAAAAABBBBBBBB01010000FFFFFFFFFF0000001FE0000000000001000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F30313233000100000000B8CAE43B;sleep quiet 1", port_acc);
    bshell(unit, str);

    printf("\nshow c\n");
    bshell(unit, "show c");

    return BCM_E_NONE;
}

int execute()
{
    bcm_error_t rv;
    int unit = 0;

    if( (rv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %s\n", bcm_errmsg(rv));
        return rv;
    }

    if( (rv = configure_INT(unit)) != BCM_E_NONE )
    {
        printf("Configuring INT failed with %s\n", bcm_errmsg(rv));
        return rv;
    }

    if( (rv = verify(unit)) != BCM_E_NONE )
    {
        printf("Verify INT failed with %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
