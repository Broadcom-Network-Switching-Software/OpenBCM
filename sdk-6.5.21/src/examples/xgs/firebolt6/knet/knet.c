/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : KNET
 *
 *  Usage    : BCM.0> cint knet.c
 *
 *  config   : knet_config.bcm
 *
 *  Log file : knet_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT script demonstrate how to send and receive packets over KNET interface
 *    This exampls shows how application can create KNET interface 
 *    and send/receive packets over it.
 *    
 *    Prerequistes:
 *    =============
 *      a) Build SDK with KNET feature enabled
 *      b) Build $SDK/src/examples/xgs/maverick2/knet/knet_tx.c and knet_rx.c 
 *         source files for customer target CPU
 *      c) Insert knet kernel module before launching SDK.
 *     
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Enable siwtch control "ArpReplyToCpu" and "ArpRequestCpu"
 *      b) Enable BCM RX module and register Rx Callback
 *      c) Add IFP rules to copy protocol and data packets to CPU
 * 
 *    2) Step2 - Configuration (Done in knet_setup())
 *    ===============================================
 *      a) Create Knet interface of type BCM_KNET_NETIF_T_TX_CPU_INGRESS
 *      b) Create Knet filter to divert packets to Knet interface
 *        
 *    3) Step3 - Verification (Done in verify())
 *    ===========================================
 *      a) Provided steps to user for sending and receiving packets 
 *         through knet interface  
 *      b) Expected Result:
 *         ================
 *         knet_tx executable will send 5 packets over knet interface
 *         knet_rx executable will receive 5 packets over knet interface and display
 */
 
cint_reset();

bcm_error_t
knet_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE; 
    const bcm_mac_t local_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE };
    const char *device_name = "virt-intf0";

    printf("Create a KNET interface to handle protocol and local station packets\n");
    rv = create_knet_interface(unit, device_name, local_mac);
    if(BCM_FAILURE(rv)) {
        printf("\nError in create_knet_interface() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("DONE; don't forget \"ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up\"\n");
    return rv;
}

/*
 * Create a KNET interface with two filters to direct protocol and filter match
 * IP packets to it.
 */
bcm_error_t
create_knet_interface(int unit, char *device_name, bcm_mac_t local_mac)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_knet_filter_t filter1;
    bcm_knet_filter_t filter2;
    bcm_knet_netif_t netif;
    int         count;

    /* Send packets through ingress logic */
    netif.type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
    netif.vlan = 2;
    netif.port = 0 ; 
    netif.flags = 0 ; 
    sal_strcpy(netif.name, device_name);        /* Set device name */
    /* Set MAC address associated with this interface */
    netif.mac_addr = local_mac;

    printf("bcm_knet_netif_create\n");
    rv = bcm_knet_netif_create(unit, &netif);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_netif_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add filter to catch protocol packets */
    sal_strcpy(filter1.desc, "Protocol Packets");
    filter1.type = BCM_KNET_FILTER_T_RX_PKT;
    filter1.flags = BCM_KNET_FILTER_F_STRIP_TAG;
    filter1.priority = 50;
    /* Send packet to network interface */
    filter1.dest_type = BCM_KNET_DEST_T_NETIF;
    filter1.dest_id = netif.id;
    filter1.match_flags = BCM_KNET_FILTER_M_REASON;
    BCM_RX_REASON_SET(&filter1.m_reason, bcmRxReasonProtocol);
    printf("bcm_knet_filter_create 1\n");
    rv = bcm_knet_filter_create(unit, &filter1);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add filter to catch nexthop packets */
    sal_strcpy(filter2.desc, "Filter Match Packets");
    filter2.type = BCM_KNET_FILTER_T_RX_PKT;
    filter2.flags = BCM_KNET_FILTER_F_STRIP_TAG;
    filter2.priority = 55;
    filter2.dest_type = BCM_KNET_DEST_T_NETIF;
    filter2.dest_id = netif.id;
    filter2.match_flags = BCM_KNET_FILTER_M_REASON;
    BCM_RX_REASON_SET(&filter2.m_reason, bcmRxReasonFilterMatch);
    printf("bcm_knet_filter_create 2\n");
    rv = bcm_knet_filter_create(unit, &filter2);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_knet_filter_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t
configure_ifp (int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    bcm_field_group_config_t group_config;
    bcm_field_entry_t eid;
    bcm_vlan_t vlan = 2, vlan_mask = 0xfff;
    bcm_port_t port = 0, port_mask = 0xffffffff;
    int prio;

    /* Enable IFP for CPU port */
    rv = bcm_port_control_set(unit, port, bcmPortControlFilterIngress, 1);

    /* FP group configuration and creation */
    bcm_field_group_config_t_init(&group_config);

    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);

    group_config.mode = bcmFieldGroupModeAuto;

    rv = bcm_field_group_config_create(unit, &group_config);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_group_config_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* FP entry configuration and creation */
    rv = bcm_field_entry_create(unit, group_config.group, &eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_create() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, eid, port, port_mask);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_qualify_InPort() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* FP entry actions configuration */
    rv = bcm_field_action_add(unit, eid, bcmFieldActionDrop, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, eid, bcmFieldActionCopyToCpu, 0, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_action_add() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Installing FP entry to FP TCAM */
    rv = bcm_field_entry_install(unit, eid);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_field_entry_install() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Receive Task Callback
 *
 * This routine catches bcm_rx packets and prints a simple message.
 */
bcm_rx_t
packetWatcher(int unit, bcm_pkt_t * pkt, void *cookie)
{
    int        *count = (auto) cookie;

    (*count)++;
    printf("Packet: %3d; Size: %d; Src Port: %d; VLAN: %d;", *count,
           pkt->pkt_len, pkt->src_port, pkt->vlan);
    if (BCM_RX_REASON_IS_NULL(pkt->rx_reasons)) {
        printf(" Switched to CPU\n");
    } else {
        printf(" Sent for Reason\n");
    }
    return BCM_RX_HANDLED;
}

int         packet_count = 0;

bcm_error_t
rx_init(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    const int   priority = 101;
    const int   flags = BCM_RCO_F_ALL_COS;

    if (!bcm_rx_active(unit)) {
        rv = bcm_rx_init(unit);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_rx_init() : %s\n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_rx_start(unit, NULL);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_rx_start() : %s\n", bcm_errmsg(rv));
            return rv;
        }

    }

    rv = bcm_rx_register(unit, "Rx PacketWatch", 
                         packetWatcher, priority, &packet_count,
                         flags);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_rx_register() : %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


bcm_error_t 
test_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_switch_control_set(unit, bcmSwitchArpReplyToCpu, TRUE);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set() 
                  bcmSwitchArpReplyToCpu : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchArpRequestToCpu, TRUE);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_switch_control_set() 
                  bcmSwitchArpRequestToCpu : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = rx_init(unit);
    if(BCM_FAILURE(rv)) {
        printf("\nError in rx_init(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = configure_ifp(unit);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configure_ifp(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

void verify(int unit)
{
    printf("\nUser has to follow below steps to verify KNET functionality\n");
    printf("====================================================================================\n");
    printf("1. Go to Linux shell prompt using \"shell\" command from BCM diag shell\n");
    printf("2. Configure IP address to knet interface \"ifconfig virt-intf0 192.168.2.254 netmask 255.255.255.0 up\" \n");
    printf("3. Run knet_rx.exe in backround \"./knet_rx.exe -vv virt-intf0 5 &\"\n");
    printf("4. Run knet_tx.exe to send 5 packets \"./knet_tx.exe -vv virt-intf0 5\" \n");
    printf("=====================================================================================\n");
    printf("Above 4 steps will demonstrate transmit and receive functionality over KNET interface\n");

}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;
    bshell(unit, "config show; a ; version");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = knet_setup(unit)))) {
        printf("KNET Setup Failed\n");
        return -1;
    }

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
