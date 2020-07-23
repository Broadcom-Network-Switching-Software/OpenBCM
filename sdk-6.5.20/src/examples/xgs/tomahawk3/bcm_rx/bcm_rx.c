/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : BCM RX
 *                   
 *  Usage    : BCM.0> cint bcm_rx.c
 *                                 
 *  config   : bcm_rx_config.bcm   
 *                                 
 *  Log file : bcm_rx_log.txt      
 *                                 
 *  Test Topology :                
 *                                 
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  ingress_port     |                              |  cpu_port
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
 *    This CINT script registers RX callback using BCM APIs.
 *    This exampls hsows how application can register RX callback function
 *    and process the packets received by CPU port.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    ============================================= 
 *      a) Select one ingress port and configure it in loopback mode.
 *      b) Add static L2 entry on cpu port with mac=00:00:00:00:00:03 and vlan=1
 *
 *    2) Step2 - Configuration (Done in rx_setup())
 *    ===================================================
 *      a) Check if RX is active, if not then Init RX and start RX thread
 *      b) Register RX callback function which will print the received packet along
 *         with the metadata of the packet.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Transmit multiple packets with different vlan_prio and mac=00:00:00:00:00:03
 *      b) Expected Result:
 *         ================
 *         The packets are looped back on ingress port and received by CPU port
 *         The RX callback registered in step#4 will dump the packet along with metadata
 *         on console
 */


cint_reset();
bcm_port_t ingress_port;

/*
 * packet_info()
 *
 * This routine prints of some key bits of information from a bcm_pkt_t structure.
 */
void
packet_info(bcm_pkt_t * pkt)
{
    /*
     * Define a mapping from reason code to its corresponding text string. All
     * reasons may not be used by some devices.
     */
    const char *reason_strings[] = {
        "Invalid", "Arp", "Bpdu", "Broadcast", "ClassBasedMove",
        "ClassTagPackets", "Control", "CpuLearn", "DestLookupFail",
        "Dhcp", "DosAttack", "E2eHolIbp", "EncapHigigError", "FilterMatch",
        "GreChecksum", "GreSourceRoute", "HigigControl", "HigigHdrError",
        "IcmpRedirect", "Igmp", "IngressFilter", "Ip", "IpfixRateViolation",
        "IpMcastMiss", "IpmcReserved", "IpOptionVersion", "Ipmc",
        "L2Cpu", "L2DestMiss", "L2LearnLimit", "L2Move", "L2MtuFail",
        "L2NonUnicastMiss", "L2SourceMiss", "L3AddrBindFail",
        "L3DestMiss", "L3HeaderError", "L3MtuFail", "L3Slowpath",
        "L3SourceMiss", "L3SourceMove", "MartianAddr", "McastIdxError",
        "McastMiss", "MimServiceError", "MplsCtrlWordError", "MplsError",
        "MplsInvalidAction", "MplsInvalidPayload", "MplsLabelMiss",
        "MplsSequenceNumber", "MplsTtl", "Multicast", "Nhop", "OAMError",
        "OAMSlowpath", "OAMLMDM", "ParityError", "Protocol", "SampleDest",
        "SampleSource", "SharedVlanMismatch", "SourceRoute", "TimeStamp",
        "Ttl", "Ttl1", "TunnelError", "UdpChecksum", "UnknownVlan",
        "UrpfFail", "VcLabelMiss", "VlanFilterMatch", "WlanClientError",
        "WlanSlowpath", "WlanDot1xDrop", "ExceptionFlood", "TimeSync",
        "EAVData", "SamePortBridge", "SplitHorizon", "L4Error", "Stp",
        "EgressFilterRedirect", "FilterRedirect", "Loopback", "VlanTranslate",
        "Mmrp", "Srp", "TunnelControl", "L2Marked", "WlanSlowpathKeepalive",
        "Station", "Niv", "NivPrioDrop", "NivInterfaceMiss", "NivRpfFail",
        "NivTagInvalid", "NivTagDrop", "NivUntagDrop", "Trill", "TrillInvalid",
        "TrillMiss", "TrillRpfFail", "TrillSlowpath", "TrillCoreIsIs",
        "TrillTtl", "TrillName", "BfdSlowpath", "Bfd", "Mirror",
        "RegexAction", "RegexMatch", "FailoverDrop", "WlanTunnelError",
        "CongestionCnmProxy", "CongestionCnmProxyError", "CongestionCnm",
        "MplsUnknownAch", "MplsLookupsExceeded", "MplsReservedEntropyLabel",
        "MplsIllegalReservedLabel", "MplsRouterAlertLabel", "NivPrune",
        "VirtualPortPrune", "NonUnicastDrop", "TrillPacketPortMismatch",
        "WlanClientMove", "WlanSourcePortMiss", "WlanClientSourceMiss",
        "WlanClientDestMiss", "WlanMtu", "L2GreSipMiss", "L2GreVpnIdMiss",
        "TimesyncUnknownVersion", "BfdUnknownVersion", "BfdInvalidVersion",
        "BfdLookupFailure", "BfdInvalidPacket", "VxlanSipMiss",
        "VxlanVpnIdMiss", "FcoeZoneCheckFail", "IpmcInterfaceMismatch",
        "Nat", "TcpUdpNatMiss", "IcmpNatMiss", "NatFragment", "NatMiss",
        "OAMCCMSlowpath", "BHHOAM", "UnknownSubtendingPort", "Reserved0",
        "OAMMplsLmDm", "Sat", "SampleSourceFlex"
    };
    bcm_rx_reason_t reason;
    int         i;
    int         reason_count;

    /* Print basic packet information first */
    printf
      ("  length %4d; rx-port %2d; COS %2d; prio_int %2d; VLAN %4d; DMA chan: %d; %s;\n",
       pkt->pkt_len, pkt->rx_port, pkt->cos, pkt->prio_int, pkt->vlan,
       pkt->dma_channel, (pkt->rx_untagged & BCM_PKT_OUTER_UNTAGGED)
       ? ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED)
          ? "Untagged" : "Inner tagged")
       : ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED) ? "Outer tagged" :
          "Double tagged"));

    /* Print additional packet information */
    if ((pkt->flags & BCM_PKT_STK_F_SRC_PORT) && (pkt->flags & BCM_PKT_STK_F_DST_PORT)) {
        /* Both destination and source GPORTs available */
        printf
          ("  dest-gport %d; src-gport %d; opcode %d;%s matched %d; classification-tag %d;\n",
           pkt->dst_gport, pkt->src_gport, pkt->opcode,
           (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
           pkt->rx_classification_tag);
    } else if ((pkt->flags & BCM_PKT_STK_F_SRC_PORT)) {
        /* Source GPORT available */
        printf
          ("  dest-port %d; dest-mod %d; src-gport %d; opcode %d;%s matched %d; classification-tag %d;\n",
           pkt->dest_port, pkt->dest_mod, pkt->src_gport, pkt->opcode,
           (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
           pkt->rx_classification_tag);
    } else if ((pkt->flags & BCM_PKT_STK_F_DST_PORT)) {
        /* Destination GPORT available */
        printf
          ("  dest-gport %d; %s %d; src-mod %d; opcode %d;%s matched %d; classification-tag %d;\n",
           pkt->dst_gport, (pkt->flags & BCM_PKT_F_TRUNK) ? "src-trunk" : "src-port",
           (pkt->flags & BCM_PKT_F_TRUNK) ? pkt->src_trunk : pkt->src_port,
           pkt->src_mod, pkt->opcode,
           (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
           pkt->rx_classification_tag);
    } else {
        /* No GPORTs */
        printf
          ("  dest-port %d; dest-mod %d;%s %d; src-mod %d; opcode %d;%s matched %d; classification-tag %d;\n",
           pkt->dest_port, pkt->dest_mod,
           (pkt->flags & BCM_PKT_F_TRUNK) ? "src-trunk" : "src-port",
           (pkt->flags & BCM_PKT_F_TRUNK) ? pkt->src_trunk : pkt->src_port,
           pkt->src_mod, pkt->opcode,
           (pkt->flags & BCM_RX_TRUNCATED) ? " Truncated;" : "", pkt->rx_matched,
           pkt->rx_classification_tag);
    }

    /*
     * Print out reasons (if any) by iterating through the entire reasons
     * mask and printing the reason string associated with each reason.
     */
    reason_count = 0;
    BCM_RX_REASON_ITER(pkt->rx_reasons, reason) {
        reason_count++;
        printf(" %s", reason_strings[reason]);
    }
    if (reason_count) {
        printf("\nPacket sent for %d reason(s)\n", reason_count);
    } else {
        /* Assume that if there were no reasons, must be switched */
        printf("Packet switched to CPU\n");
    }
}

/* A little routine to print a MAC address */
void
mac_print(unsigned char *macAddr)
{

    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
           (macAddr[0] & 0xFF), (macAddr[1] & 0xFF), (macAddr[2] & 0xFF),
           (macAddr[3] & 0xFF), (macAddr[4] & 0xFF), (macAddr[5] & 0xFF));
}

/* Receive Task Callback
 *
 * This is called for every packet received. It prints the packet metadata
 * along with the entire packet.
 *
 * Param "cookie" is a pointer to current received packet count.
 */
bcm_rx_t
packetWatcher(int unit, bcm_pkt_t * pkt, void *cookie)
{
    int        BCM_RX_HANDLED=2;
    int        *packet_count = (auto) cookie;
    int         i;

    /* Increment packet count */
    (*packet_count)++;

    printf("Received Packet: %2d\n", *packet_count);

    for (i = 0; i < pkt->tot_len; i++) {
        uint32      print_byte = pkt->pkt_data->data[i] & 0xFF;

        if ((i & 0xf) == 0) {
            printf("\n%03X:", i);
        }
        printf(" %02X", print_byte);
    }
    if (((i & 0xf) != 1)) {
        printf("\n");
    }

    printf("  DM=");
    mac_print(&pkt->pkt_data->data[0]);
    printf("  SM=");
    mac_print(&pkt->pkt_data->data[6]);

    /* print packet metadata */
    packet_info(pkt);

    printf("\n");

    return BCM_RX_HANDLED;
}

/* 
 * rx_setup()
 *
 * Start Rx task (if necessary) and install a single Rx callback routine.
 */
int         rx_count;   /* Global received packet count */
int
rx_setup(int unit)
{
    int         rv;

    /*
     * Use a priority above 100 to be able to co-exist with diagnostic
     * shell packetWatcher.
     */
    const int   priority = 101;

    /* Receive packets from all CPU queues. */
    const int   flags = BCM_RCO_F_ALL_COS;

    if (!bcm_rx_active(unit)) {
        /*
         * If necessary, initialize and start the receive task. 
         */
        rv = bcm_rx_init(unit);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_rx_init(): %s.\n",bcm_errmsg(rv));
            return rv;
        }
        
        rv = bcm_rx_start(unit, NULL);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_rx_start(): %s.\n",bcm_errmsg(rv));
            return rv;
        }
        printf("Started the Rx task\n");
    }

    /*
     * Register Packet Watcher Rx Callback. 
     */
    rv = bcm_rx_register(unit, "Rx PacketWatch", packetWatcher, \
                         priority, &rx_count,flags);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_rx_init(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("Registered CINT Packet Watcher callback\n");

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and 
 * add L2 entry with mac-03 for CPU port.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
    const char *mac = "00:00:00:00:00:03";
    const int   max_vlan_pri = 8;

    char        command[128];   /* Buffer for diagnostic shell commands */
    int         vlan_pri;

    /* Put test port into loopback mode */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY));
    
    /* Configure "tx" utility to send IEEE packets (not HiGig) */
    bshell(unit, "hm ieee");

    /* Set up L2 map to direct packets to CPU */
    sprintf(command, "l2 add PortBitMap=cpu0 vlan=1 mac=%s rp=0 st=1", mac);
    bshell(unit, command);
    return BCM_E_NONE;
}

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

  ingress_port = port_list[0];
  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }
  return BCM_E_NONE;
}

/*
 * Test Harness
 *
 * Demonstrate use of Rx callbacks.
 *
 * Verify behavior by sending packets through the switch and back through
 * the CPU to the Rx callback routine.
 */
bcm_error_t
test_harness(int unit)
{
    int         len;
    const char *mac = "00:00:00:00:00:03";
    const int   max_vlan_pri = 8;
    char        command[128];   /* Buffer for diagnostic shell commands */
    int         vlan_pri;
 
    /* Send packets, loop them back through CPU port */
    for (len = 100; len <= 120; len += 10) {
        for (vlan_pri = 0; vlan_pri < max_vlan_pri; vlan_pri += 2) {
            printf
              ("----------------------------------------------------------------\n");
            sprintf(command,
                    "tx 1 PortBitMap=%d Length=%d DestMac=%s VlanPrio=%d VLantag=1 
                     PatternRandom=yes",
                     ingress_port, len, mac, vlan_pri);
            bshell(unit, command);
            sal_sleep(1);       /* Pause long enough to print packet data */
        }
    }
    return BCM_E_NONE;
}

void verify(int unit)
{
    bcm_error_t rv;
    if (BCM_FAILURE(rv = test_harness(0))) {
        printf("Rx verification failed: %s\n", bcm_errmsg(rv));
    }
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

    if (BCM_FAILURE((rv = rx_setup(unit)))) {
        printf("RX Setup Failed\n");
        return -1;
    }

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
