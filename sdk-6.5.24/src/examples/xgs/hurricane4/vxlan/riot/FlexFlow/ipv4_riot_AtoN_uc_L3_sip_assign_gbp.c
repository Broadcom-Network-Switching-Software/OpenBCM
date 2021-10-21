/*  Feature  : Using Flow APIs to performs vxlan encapsulation operation on the TD3 fixed 
 *             hardware logic. This flow is  identified with the flow name "CLASSIC_VXLAN"
 *
 *  Usage : BCM.0> cint ipv4_riot_AtoN_uc_L2_smac_assign_gbp.c
 *
 *  config   : BCM56275_A1-RIOT-FLEXFLOW.bcm
 *
 *  Log file : ipv4_riot_AtoN_uc_L2_smac_assign_gbp_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              +
 *                   |                              |
 *                   |                              |  VXLAN header with GBP assigned by L3 table
 *    [ACCESS port]  |                              |  [NETWORK port]
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   +                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *      Create acc and network vlans and add ports to it. Add the L2 address path. Match the packet
 *      based on Port. Assign the GBP based on Src IP and VRF. Update the L3 Host table for Dest IP.
 *      Create the access and network flow ports.
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Take 2 ports as access and network ports
 *       b) Put them in access and network vlans respectively
 *       c) Put these ports in loopback and capture packet from these ports to cpu
 *
 *    2) Step2 - Configuration (Done in TestFunc()).
 *    ===================================================
 *       a) Create vpn and put network flow port and access flow ports in to this vpn
 *       b) Create an L2 entry to match on smac of the access port packet
 *       c) Assign GBP to the matching packet
 *
 *    2) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send unicast L3 UDP packet to access port from cpu matching the smac programmed
 *          in L2 table
 *       b) Expected Result:
 *          ================
 *          You should see that vxlan packet is egressing out of network port with GBP assign
 *          as programmed
 */
/*

##  Gbp_Riot_001
##
# Verify that GBP_SID tag assigned for the IPv4 Packet.
#
#
# Test Procedure:
# 1. Use Topology 1.0
# 2. Create acc and network vlans and add ports to it.
# 3. Add the L2 address path.
# 4. Match the packet based on Port.
# 5. Assign the GBP based on Src IP and VRF.
# 6. Update the L3 Host table for Dest IP.
# 7. Create the access and network flow ports.
# 8. Send the L3 traffic.
# 9. Verify that VXLAN_GBP packet egresses on network port.
# 10. Verify the GBP_SID in the Vxlan Header.
# 11. Save the configuration.
# 12. Perform the warmBoot.
# 13. Send the L3 traffic after warmBoot.
# 14. Verify that VXLAN_GBP egresses on the network flow port.
# 15. Verify the GBP_SID in vxlan header.
# 16. Unconfigure all the above configuration.
#
*/

cint_reset();

/****************   GENERIC UTILITY FUNCTIONS STARTS  **************************
 */
bcm_port_t PORT_ANY = -1;                                                  /* */
bcm_port_t __next_port_to_return = 1;                                      /* */
bcm_port_t         // Get next valid Ethernet port                         /* */
 portGetNextE(int unit, bcm_port_t PreferredPort)                          /* */
{                                                                          /* */
  int i=0, rv=0;                                                           /* */
  bcm_port_config_t configP;                                               /* */
  bcm_pbmp_t ports_pbmp;                                                   /* */
  if( PreferredPort != PORT_ANY ) {                                        /* */
    printf("Using preferred port %d\n", PreferredPort);                    /* */
    return PreferredPort;                                                  /* */
  }                                                                        /* */
  rv = bcm_port_config_get(unit, &configP);                                /* */
  if(BCM_FAILURE(rv)) {                                                    /* */
    printf("\nError in retrieving port configuration: %s %d.\n",           /* */
        bcm_errmsg(rv), __next_port_to_return);                            /* */
    exit;                                                                  /* */
  }                                                                        /* */
  ports_pbmp = configP.e;    // configP.hg;                                /* */
  for (i= __next_port_to_return; i < BCM_PBMP_PORT_MAX; i++) {             /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp, i) ) {                               /* */
        __next_port_to_return = i+1; // point to a probable next port      /* */
        return i;                                                          /* */
    }                                                                      /* */
  }                                                                        /* */
  printf("ERROR: Not enough ports %d\n", __next_port_to_return);           /* */
  exit;                                                                    /* */
}                                                                          /* */
bcm_error_t                                                                /* */
 common_fp_port_setup(int unit, bcm_port_t port, int LoopBackType, int Ing)/* */
{                                                                          /* */
  bcm_field_group_t group;                                                 /* */
  bcm_field_group_config_t group_config;                                   /* */
  int                 pri, fp_statid, match_id, mode;                      /* */
  bcm_field_entry_t entry;                                                 /* */
  if(Ing) {                                                                /* */
      group = 9998; entry = 9998; fp_statid = 9998;                        /* */
      match_id = 222;                                                      /* */
      mode = BCM_PORT_DISCARD_NONE;                                        /* */
  } else {                                                                 /* */
      group = 9999; entry = 9999; fp_statid = 9999;                        /* */
      match_id = 223;                                                      /* */
      mode = BCM_PORT_DISCARD_ALL;                                         /* */
  }                                                                        /* */
  pri = -1 * group;                                                        /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, mode));             /* */
  bcm_field_group_config_t_init(&group_config);                            /* */
  BCM_FIELD_ASET_INIT(group_config.aset);                                  /* */
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);          /* */
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);          /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.mode = bcmFieldGroupModeAuto;                               /* */
  group_config.group = group;                                              /* */
  group_config.priority = pri;                                             /* */
  BCM_FIELD_QSET_INIT(group_config.qset);                                  /* */
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);            /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit,                  /* */
                                                &group_config));           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /* */
            BCM_FIELD_EXACT_MATCH_MASK));                                  /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
            bcmFieldActionCopyToCpu, 1, match_id));                        /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid));/* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
bcm_error_t                                                                /* */
 ingress_port_setup(int unit, bcm_port_t port, int LoopBackType)           /* */
{                                                                          /* */
  return common_fp_port_setup(unit, port, LoopBackType, 1);                /* */
}                                                                          /* */
bcm_error_t                                                                /* */
 egress_port_setup(int unit, bcm_port_t port, int LoopBackType)            /* */
{                                                                          /* */
  return common_fp_port_setup(unit, port, LoopBackType, 0);                /* */
}                                                                          /* */
bcm_error_t                                                                /* */
 egress_port_multi_setup(int unit, bcm_pbmp_t pbm, int LoopBackType)       /* */
{       /* ifp_inports_support_enable */                                   /* */
  bcm_field_group_t group = 10000;                                         /* */
  bcm_field_group_config_t group_config;                                   /* */
  int                 pri = -1 * group, fp_statid = group;                 /* */
  bcm_field_entry_t   entry = 10000;                                       /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  bcm_port_t     port;                                                     /* */
  BCM_PBMP_ITER(pbm, port) {                                               /* */
      BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,                /* */
                  LoopBackType));                                          /* */
      BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                 /* */
                  BCM_PORT_DISCARD_ALL));                                  /* */
  }                                                                        /* */
  print pbm;                                                               /* */
  bcm_pbmp_t pbm_mask;                                                     /* */
  BCM_PBMP_CLEAR(pbm_mask);                                                /* */
  BCM_PBMP_NEGATE(pbm_mask, pbm_mask);                                     /* */
  bcm_field_group_config_t_init(&group_config);                            /* */
  BCM_FIELD_ASET_INIT(group_config.aset);                                  /* */
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);          /* */
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);          /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.mode = bcmFieldGroupModeAuto;                               /* */
  group_config.group = group;                                              /* */
  group_config.priority = pri;                                             /* */
  BCM_FIELD_QSET_INIT(group_config.qset);                                  /* */
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPorts);           /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit,                  /* */
                                                    &group_config));       /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, entry,               /* */
              pbm, pbm_mask));                                             /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
              bcmFieldActionCopyToCpu, 1, 224));                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
              stats, fp_statid));                                          /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,             /* */
              fp_statid));                                                 /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
void bbshell(int unit, char *str)                                          /* */
{                                                                          /* */
  printf("B_CM.%d> %s\n", unit, str);                                      /* */
  bshell(unit, str);                                                       /* */
}                                                                          /* */
int chartoint(int c)                                                       /* */
{                                                                          /* */
    char *hex = "aAbBcCdDeEfF";                                            /* */
    int i, result = 0;                                                     /* */
    for(i = 0; (result == 0) && (hex[i] != '\0'); i++) {                   /* */
        if(hex[i] == c) {                                                  /* */
            result = 10 + (i / 2);                                         /* */
        }                                                                  /* */
    }                                                                      /* */
    return result;                                                         /* */
}                                                                          /* */
uint32 mine_dtoi(char *str)                                                /* */
{                                                                          /* */
    int i;                                                                 /* */
    uint32 dec = 0;                                                        /* */
    for(i=0; str[i]!=0; i++) {                                             /* */
        dec = dec * 10 + ( str[i] - '0' );                                 /* */
    }                                                                      /* */
    return dec;                                                            /* */
}                                                                          /* */
/* Converts String to Hexadecimal or Decimal. Hex is preceding is */       /* */
/* 'x' or 'X'. Else Decimal */                                             /* */
uint32 mine_atoi(char *str)                                                /* */
{                                                                          /* */
    uint32 result = 0;                                                     /* */
    int i = 0, proper = 1, temp, is_decimal = 1;                           /* */
    //To take care of 0x and 0X added before the hex no.                   /* */
    if((str[0] == 'x') || (str[0] == 'X') ) {                              /* */
        is_decimal = 0;                                                    /* */
    }                                                                      /* */
    if(str[0] == '0') {                                                    /* */
        is_decimal = 0;                                                    /* */
        ++i;                                                               /* */
        if((str[i] == 'x') || (str[i] == 'X') ) {                          /* */
            ++i;                                                           /* */
        } else {                                                           /* */
            i--; // '0' was a valid 0 as no trailing 'x' found             /* */
        }                                                                  /* */
    }                                                                      /* */
    if( is_decimal ) {                                                     /* */
        return mine_dtoi(str);                                             /* */
    }                                                                      /* */
    while(proper && (str[i] != '\0') ) {                                   /* */
        result = result * 16;                                              /* */
        if((str[i] >= '0') && (str[i] <= '9') ) {                          /* */
            result = result + (str[i] - '0');                              /* */
        } else {                                                           /* */
            temp = chartoint(str[i]);                                      /* */
            if(temp == 0) {                                                /* */
                printf("Improper char 0x%X", str[i]);                      /* */
                proper = 0;                                                /* */
            } else {                                                       /* */
                result = result + temp;                                    /* */
            }                                                              /* */
        }                                                                  /* */
        ++i;                                                               /* */
    }                                                                      /* */
    //If any character is not a proper hex no. ,  return 0                 /* */
    if(!proper) {                                                          /* */
     printf("ERROR EVALUATING IN mine_atoi [%s]. Defaulting to 0.\n", str);/* */
     result = 0;                                                           /* */
    }                                                                      /* */
    return result;                                                         /* */
}                                                                          /* */
uint32 ip4_sum_n_size_update(char *headerStr,                              /* */
                                int StartOfIpHdrByte,int SkipEqualCheck)   /* */
{                                                                          /* */
    uint32 word16, sum=0, i=0, ip4len;                                     /* */
    char numStr1[5], numStr2[5];                                           /* */
    unsigned char byte1, byte2;                                            /* */
    if( !SkipEqualCheck )                                                  /* */
        while(i<2) { if( *headerStr++ == '=' ) i++; }                      /* */
    StartOfIpHdrByte = StartOfIpHdrByte * 2;  // ascii in text, hence *2   /* */
    headerStr = &headerStr[StartOfIpHdrByte];                              /* */
    ip4len = (sal_strlen(&headerStr[4*2]))/2;                              /* */
    sprintf(numStr1, "%04X", ip4len);                                      /* */
    headerStr[4] = numStr1[0]; headerStr[5] = numStr1[1];                  /* */
    headerStr[6] = numStr1[2]; headerStr[7] = numStr1[3];                  /* */
    numStr1[0] = '0'; numStr1[1] = 'x'; numStr1[2] = 0;                    /* */
    numStr1[3] = 0; numStr1[4] = 0;                                        /* */
    numStr2[0] = '0'; numStr2[1] = 'x'; numStr2[2] = 0;                    /* */
    numStr2[3] = 0; numStr2[4] = 0;                                        /* */
    // make 16 bit words out of every two adjacent 8 bit words in          /* */
    // the packet and add them up                                          /* */
    for (i=0;i<20;i+=2){                                                   /* */
        if(i>=10 && i<=11) {                                               /* */
            continue;                                                      /* */
        } else {                                                           /* */
            numStr1[2] = headerStr[i*2];                                   /* */
            numStr1[3] = headerStr[(i*2)+1];                               /* */
            numStr2[2] = headerStr[(i*2)+2];                               /* */
            numStr2[3] = headerStr[(i*2)+3];                               /* */
        }                                                                  /* */
        byte1 = mine_atoi(numStr1); byte2 = mine_atoi(numStr2);            /* */
        word16 =((byte1<<8)&0xFF00)+(byte2&0xFF);                          /* */
        sum = sum + word16;                                                /* */
    }                                                                      /* */
    // take only 16 bits out of the 32 bit sum and add up the carries      /* */
    while (sum>>16) sum = (sum & 0xFFFF)+(sum >> 16);                      /* */
    sum = (~sum) & 0xFFFF;                                                 /* */
    sprintf(numStr1, "%04X", sum);                                         /* */
    headerStr[20] = numStr1[0]; headerStr[21] = numStr1[1];                /* */
    headerStr[22] = numStr1[2]; headerStr[23] = numStr1[3];                /* */
    return sum;                                                            /* */
}                                                                          /* */
int FORM_VID(int PCP, int DEI, int VID) {                                  /* */
    return ( ((PCP)<<13) | ((DEI)<<12) | ((VID) & 0xFFF) );                /* */
}                                                                          /* */
int FORM_MPLS(int LABEL, int EXP, int S, int TTL) {                        /* */
    return ( ((LABEL)<<12) | ((EXP)<<9) | ((S)<<8) | ((TTL) & 0xFF) );     /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

int unit = 0;
bcm_vlan_t  acc_vid = 21;
bcm_vlan_t  net_vid = 22;

bcm_mac_t     local_mac_network  = "00:00:00:00:22:22";
bcm_mac_t     remote_mac_network = "00:00:00:00:00:02";
bcm_mac_t     payload_dst_mac    = "00:00:00:00:33:00";
bcm_mac_t     payload_src_mac    = "00:00:00:00:44:00";
bcm_mac_t     mac_mask           = "FF:FF:FF:FF:FF:FF";

bcm_mac_t     ol_remote_mac      = "00:00:00:00:aa:aa";
bcm_mac_t     ol_local_mac       = "00:00:00:00:bb:bb";
bcm_vlan_t    ol_vid = 30;

uint32 vnid = 0x12345;
int ttl = 16;
int vrf = 0;

bcm_vpn_t vpn = 0x7001; // BCM_VPN_TYPE_VXLAN   ELAN

bcm_ip_t    tnl_local_ip   = 0x0a0a0a02;
bcm_ip_t    tnl_remote_ip  = 0xC0A80101;
bcm_ip_t    payload_dst_ip = 0x0a0a0a01;

uint16      udp_dst_port = 4789; //8472;
uint16      udp_src_port = 65535;
bcm_multicast_t     bc_group = 0;
uint32 flag = 0;
uint16 tpid = 0x9100;

uint32 vxlan_gbp_flow_handle = -1;
uint32 vxlan_flow_handle = -1;

uint16 vlan_mask = 0x73FF;

bcm_ip_t    ip_mask = 0xFFFFFFFF;

bcm_ip_t    remote_ip = 0xC0A80101;
bcm_ip_t    dst_ip = 0x0a0a0a01;
bcm_ip_t    src_ip = 0x0a0a0a02;

uint16  l4_src_port = 65535;
uint16  l4_dst_port = 4789; //8472;
uint16  pay_tpid = 8100;
int     pri = 4;

bcm_flow_logical_field_t    lfield[20];

uint32 sgpp_flow_option = -1;
uint32 dvp_flow_option = -1;
uint32 encap_flow_option = -1;
uint32 l3host_flow_option = -1;

uint16 gbp_value = 0x2222;

bcm_port_t acc_port = portGetNextE(0, -1);
bcm_port_t net_port = portGetNextE(0, -1);

bcm_gport_t acc_gport;
bcm_gport_t net_gport;

BCM_GPORT_MODPORT_SET(acc_gport, 0, acc_port);
BCM_GPORT_MODPORT_SET(net_gport, 0, net_port);

bcm_error_t TestFunc()
{
    printf("Get the flow Handle and flow options...\n");
    BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "VXLAN_GBP", &vxlan_gbp_flow_handle) );
    BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &vxlan_flow_handle) );

    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, vxlan_gbp_flow_handle,
                                    "GBP_SID_LKUP_FORWARDING_TABLE", &sgpp_flow_option) );
    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, vxlan_gbp_flow_handle,
                                    "LOOKUP_IPV4_SIP_VRF_ASSIGN_GBP_SID", &l3host_flow_option) );

    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, vxlan_gbp_flow_handle, "IPV4",
                                    &dvp_flow_option) );
    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, vxlan_gbp_flow_handle,
                                                            "LOOKUP_DVP_VFI_ASSIGN_VNID",
                                                            &encap_flow_option) );
    
    printf("Enable the Vxlan Global Configuration...\n");
    // Vxlan_Global_Configuration
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchVxlanTunnelMissToCpu, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, udp_dst_port) );

    printf("Enable the Access and Network port controls...\n");
    // Vxlan_Access_Port_Configuration
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, acc_port, bcmPortControlVxlanEnable, FALSE) );
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, acc_port, bcmPortControlVxlanTunnelbasedVnId,
                                                                                         FALSE) );

    // Vxlan_Network_Port_Configuration
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, net_port, bcmPortControlVxlanEnable, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, net_port, bcmPortControlVxlanTunnelbasedVnId,
                                                                                        FALSE) );

    printf("Enable the Riot Global Configuration...\n");
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3IngressMode, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3IngressInterfaceMapSet, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchGbpEtherType, 0x8909) );
    
    printf("Create the Multicast group...\n");
    uint32 multi_flags = BCM_MULTICAST_TYPE_FLOW;
    BCM_IF_ERROR_RETURN( bcm_multicast_create(unit, multi_flags, &bc_group) );

    printf("Create the Flow VPN...\n");
    uint32 vpn_flags = BCM_FLOW_VPN_ELAN;
    vpn_flags |= BCM_FLOW_VPN_WITH_ID;
    
    bcm_flow_vpn_config_t   vpn_info;
    bcm_flow_vpn_config_t_init(&vpn_info);
    vpn_info.flags                   = vpn_flags;
    vpn_info.broadcast_group         = bc_group;
    vpn_info.unknown_multicast_group = bc_group;
    vpn_info.unknown_unicast_group   = bc_group;
    BCM_IF_ERROR_RETURN( bcm_flow_vpn_create(unit, &vpn, &vpn_info) );
    print vpn;

    printf("Create the Access Flow Port...\n");
    bcm_gport_t         access_flow_port;
    bcm_flow_port_t     flow_port;
    bcm_flow_port_t_init(&flow_port);
    BCM_IF_ERROR_RETURN( bcm_flow_port_create(unit, vpn, &flow_port) );
    access_flow_port = flow_port.flow_port_id;
    print access_flow_port;

    printf("Create the Network Flow Port...\n");
    bcm_gport_t     network_flow_port;
    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = BCM_FLOW_PORT_NETWORK;
    BCM_IF_ERROR_RETURN( bcm_flow_port_create(unit, vpn, &flow_port) );
    network_flow_port = flow_port.flow_port_id;
    print network_flow_port;
    
    printf("Create the L3 Interface...\n");
    bcm_l3_intf_t   l3_intf;
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac_network, sizeof(local_mac_network));
    l3_intf.l3a_vid = net_vid;
    BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf) );
    bcm_if_t    l3_intf_id = l3_intf.l3a_intf_id;

    bcm_l3_egress_t     l3_egress;
    bcm_l3_egress_t_init(&l3_egress);

    printf("Create the L3 Egress Interface...\n");
    l3_egress.flags2 = BCM_L3_FLOW_ONLY;
    l3_egress.intf = l3_intf_id;
    sal_memcpy(l3_egress.mac_addr, remote_mac_network, sizeof(remote_mac_network));
    l3_egress.vlan = net_vid;
    l3_egress.port = net_gport;
    bcm_if_t    egr_obj_remote;
    BCM_IF_ERROR_RETURN( bcm_l3_egress_create(unit, flag, &l3_egress, &egr_obj_remote) );

    printf("Create the L3 interface for the overlay local mac...\n");
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, ol_local_mac, sizeof(ol_local_mac));
    l3_intf.l3a_vid = vpn;
    BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf) );
    bcm_if_t    l3_intf_id_overlay = l3_intf.l3a_intf_id;
    
    printf("Create the L3 Egress object for Overlay remote mac...\n");
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf = l3_intf_id_overlay;
    sal_memcpy(l3_egress.mac_addr, ol_remote_mac, sizeof(ol_remote_mac));
    l3_egress.port = network_flow_port;
    bcm_if_t    egr_obj_overlay;
    BCM_IF_ERROR_RETURN( bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_overlay) );
    
    printf("Allow L3 IPv4 on the Access flow port...\n");
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, access_flow_port, bcmPortControlIP4, TRUE) );
    
    printf("Add the flow match...\n");
    uint32 criteria_value = BCM_FLOW_MATCH_CRITERIA_PORT;
    uint32 val_ele        = BCM_FLOW_MATCH_PORT_VALID | BCM_FLOW_MATCH_FLOW_PORT_VALID;
    
    bcm_flow_logical_field_t flow_logical;
    bcm_flow_logical_field_t_init(&flow_logical);

    printf("Add the flow match...\n");
    bcm_flow_match_config_t flow_match;
    bcm_flow_match_config_t_init(&flow_match);
    flow_match.criteria         = criteria_value;
    flow_match.valid_elements   = val_ele;
    flow_match.port             = acc_gport;
    flow_match.flow_port        = access_flow_port;
    flow_match.vlan             = acc_vid;
    BCM_IF_ERROR_RETURN( bcm_flow_match_add(unit, &flow_match, 1, &flow_logical) );
    
    printf("Set the SGPP controls...\n");
    bcm_flow_match_config_t_init(&flow_match);
    bcm_flow_logical_field_t_init(&flow_logical);
    
    criteria_value = BCM_FLOW_MATCH_CRITERIA_PORT;
    val_ele        = BCM_FLOW_MATCH_PORT_VALID;
    
    flow_match.flow_handle      = vxlan_gbp_flow_handle;
    flow_match.flow_option      = sgpp_flow_option;
    flow_match.criteria         = criteria_value;
    flow_match.port             = acc_gport;
    flow_match.valid_elements   = val_ele;
    BCM_IF_ERROR_RETURN( bcm_flow_match_add(unit, &flow_match, 1, &flow_logical) );
    
    printf("Assign GBP..\n");
    bcm_l3_host_t    hinfo;
    bcm_l3_host_t_init(&hinfo);
    uint32 field_id = -1;
    
    BCM_IF_ERROR_RETURN( bcm_flow_logical_field_id_get(unit, vxlan_gbp_flow_handle, "GBP_SID",
                                                                        &field_id) );
    hinfo.l3a_intf = egr_obj_overlay;
    hinfo.l3a_vrf = vrf;
    hinfo.l3a_ip_addr = remote_ip;
    hinfo.flow_handle = vxlan_gbp_flow_handle;
    hinfo.flow_option_handle = l3host_flow_option;
    
    bcm_flow_logical_field_t    field_lf[1];
    bcm_flow_logical_field_t_init(&field_lf[0]);
    field_lf[0].id = field_id;
    field_lf[0].value = gbp_value;
    
    hinfo.logical_fields[0] = field_lf[0];
    hinfo.num_of_fields = 1;
    BCM_IF_ERROR_RETURN( bcm_l3_host_add(unit, &hinfo) );
    
    printf("Create the ingress interface...\n");
    bcm_if_t  ing_if_id;
    bcm_l3_ingress_t  ing_if;
    bcm_l3_ingress_t_init(&ing_if);

    ing_if.vrf = vrf;
    BCM_IF_ERROR_RETURN( bcm_l3_ingress_create(unit, &ing_if, &ing_if_id) );
    print ing_if_id;

    bcm_vlan_control_vlan_t     vlan_ctr;
    bcm_vlan_control_vlan_t_init(&vlan_ctr);
    vlan_ctr.ingress_if = ing_if_id;
    vlan_ctr.vrf = vrf;
    BCM_IF_ERROR_RETURN( bcm_vlan_control_vlan_selective_set(unit, vpn,
                                BCM_VLAN_CONTROL_VLAN_VRF_MASK | 
                                BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK,
                                &vlan_ctr) );
    
    printf("Set the Flow Encap...\n");
    val_ele        = BCM_FLOW_PORT_ENCAP_PORT_VALID | BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
    
    bcm_flow_port_encap_t   flow_encap;
    bcm_flow_port_encap_t_init(&flow_encap);
    bcm_flow_logical_field_t_init(&flow_logical);
    
    flow_encap.valid_elements = val_ele;
    flow_encap.flow_handle = vxlan_gbp_flow_handle;
    flow_encap.flow_option = dvp_flow_option;
    flow_encap.flow_port = network_flow_port;
    flow_encap.egress_if = egr_obj_remote;
    BCM_IF_ERROR_RETURN( bcm_flow_port_encap_set(unit, &flow_encap, 1, &flow_logical) );
    
    printf("Add the Encapsulation...\n");
    criteria_value = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP;
    val_ele        = BCM_FLOW_ENCAP_VNID_VALID | BCM_FLOW_ENCAP_VPN_VALID |
                     BCM_FLOW_ENCAP_VLAN_VALID | BCM_FLOW_ENCAP_FLOW_PORT_VALID |
                     BCM_FLOW_ENCAP_FLAGS_VALID | BCM_FLOW_ENCAP_TPID_VALID;

    bcm_flow_encap_config_t     encap_add;
    bcm_flow_encap_config_t_init(&encap_add);

    bcm_flow_logical_field_t_init(&lfield);
    encap_add.flow_handle = vxlan_gbp_flow_handle;
    encap_add.flow_option = encap_flow_option;
    encap_add.criteria = criteria_value;
    encap_add.vnid = vnid;
    encap_add.vpn = vpn;
    encap_add.valid_elements = val_ele;
    encap_add.flow_port = network_flow_port;
    encap_add.vlan = ol_vid;
    encap_add.tpid = tpid;
    encap_add.flags = BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED | BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD;
    BCM_IF_ERROR_RETURN( bcm_flow_encap_add(unit, &encap_add, 1, &lfield) );
    
    BCM_IF_ERROR_RETURN( bcm_vlan_control_port_set(unit, network_flow_port,
                                                        bcmVlanPortTranslateEgressKey,
                                                        bcmVlanTranslateEgressKeyVpn) );
    
    printf("Create the Flow Tunnel...\n");
    bcm_tunnel_type_t tunnel_type = bcmTunnelTypeVxlan;
    val_ele = BCM_FLOW_TUNNEL_INIT_DIP_VALID | BCM_FLOW_TUNNEL_INIT_SIP_VALID;
    val_ele |= BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID | BCM_FLOW_TUNNEL_INIT_UDP_SPORT_VALID;

    bcm_flow_tunnel_initiator_t     tnl_config;
    bcm_flow_tunnel_initiator_t_init(&tnl_config);

    bcm_flow_logical_field_t    tnl_field;
    bcm_flow_logical_field_t_init(&tnl_field);

    tnl_config.flow_handle = vxlan_flow_handle;
    tnl_config.valid_elements = val_ele;
    tnl_config.flow_port = network_flow_port;
    tnl_config.type = tunnel_type;
    tnl_config.udp_src_port = udp_src_port;
    tnl_config.udp_dst_port = udp_dst_port;
    tnl_config.sip = tnl_local_ip;
    tnl_config.dip = tnl_remote_ip;
    BCM_IF_ERROR_RETURN( bcm_flow_tunnel_initiator_create(unit, &tnl_config, 1, &tnl_field) );
    bcm_gport_t  tnl_init_id = tnl_config.tunnel_id;
    
    bcm_l2_station_t    station;
    bcm_l2_station_t_init(&station);
    int sid;
    station.vlan = vpn;
    station.vlan_mask = vlan_mask;
    station.flags = BCM_L2_STATION_IPV4;
    station.dst_mac = payload_dst_mac;
    station.dst_mac_mask = mac_mask;
    BCM_IF_ERROR_RETURN( bcm_l2_station_add(unit, &sid, &station) );
    print sid;
    
    printf("Add the L3 Host Add...\n");
    bcm_l3_host_t   hostroute;
    bcm_l3_host_t_init(&hostroute);
    
    hostroute.l3a_ip_addr = dst_ip;
    hostroute.l3a_intf = egr_obj_overlay;
    hostroute.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN( bcm_l3_host_add(unit, &hostroute) );
    
    return BCM_E_NONE;
}


/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int rv;
    
    int loopBackType = BCM_PORT_LOOPBACK_MAC;   // BCM_PORT_LOOPBACK_PHY
    
    if( 1 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, acc_port);

        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, acc_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, acc_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
        
        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, net_port);
        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, net_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, net_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, local_mac_network, net_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = acc_port;
        printf("Doing L2 entry add\n");
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( 1 )
    {
        rv = ingress_port_setup(unit, acc_port, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", acc_port, rv);
            return rv;
        }

        rv = egress_port_setup(unit, net_port, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", net_port, rv);
            return rv;
        }
    }
    else if( 1 )
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, acc_port, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, acc_port, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, net_port, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, net_port, BCM_PORT_DISCARD_ALL));
    }

    //bshell(unit, "pw start report +raw +decode dma");
    bshell(unit, "pw start");

    return BCM_E_NONE;
}

/*
set payload [PktDataBuild L3V6 \
                      -src   $payload_src_mac \
                      -dest  $payload_dst_mac \
                      -vid   $acc_vid \
                      -prio  $pri   \
                      -sip6   $payload_src_ip6 \
                      -dip6   $payload_dst_ip6]

PktTxReady $unit PKT $payload port $acc_port
*/



bcm_error_t verify()
{
    char cmd[2*1024];
    int i;
    int prio = 0;
    
    sprintf(cmd, "a"); bbshell(unit, cmd);
    //sprintf(cmd, "ver"); bbshell(unit, cmd);
    printf("\n\n");
    
    if( 1 )
    {
        printf("##########   Sending IPv4 packet ########\n");
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500LLLL000000004011SSSS%08X%08X%04X%04X0036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737",
            acc_port,
            payload_dst_mac[0], payload_dst_mac[1], payload_dst_mac[2],
            payload_dst_mac[3], payload_dst_mac[4], payload_dst_mac[5],
            payload_src_mac[0], payload_src_mac[1], payload_src_mac[2],
            payload_src_mac[3], payload_src_mac[4], payload_src_mac[5],
            FORM_VID(prio, 0, acc_vid), remote_ip, dst_ip,
            0xC1C1 /* udp src port */,
            0xD1D1 /* udp dst port */ );
        printf("BEFORE: %s\n", cmd);
        /* For single tag IP packet; 18th byte starts at ->4500... Byte in hex; not ascii. */
        ip4_sum_n_size_update(cmd, 18, 0);
        //ip4_sum_n_size_update(cmd, 38, 0); // for inner ipv4 in Ip-in-IP packet with single tag
        //printf("AFTER:  %s\n", cmd);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        printf("#####################################################\n\n");
    }
    
    return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rv;
    
    if( (rv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d - %s\n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    if( (rv = TestFunc()) != BCM_E_NONE )
    {
        printf("Configuring TestFunc() failed with %d - %s\n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    if( (rv = verify()) != BCM_E_NONE )
    {
        printf("Verify TestFunc failed with %d - %s\n", rv, bcm_errmsg(rv));
        return rv;
    }
    
    return BCM_E_NONE;
}


char *auto_execute;
if( ARGC >= 1 ) {
    auto_execute =  ARGV[0];
} else {
    auto_execute = "YES";
}

if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


/*

Loading M0 Firmware located at ../../../../rc/cmicfw/linkscan_led_fw.bin
Firmware download successed (0x9689889).
Loading M0 Firmware located at ../../../../rc/cmicfw/custom_led.bin
Firmware download successed (0x4bc6bdc6).
UNIT0 CANCUN:
        CIH: LOADED
        Ver: 03.01.00

        CMH: LOADED
        Ver: 03.01.00
        SDK Ver: 06.05.22

        CCH: LOADED
        Ver: 03.01.00
        SDK Ver: 06.05.22

        CEH: LOADED
        Ver: 03.01.00
        SDK Ver: 06.05.22

        CFH: LOADED
        Ver: 03.01.00

rc: MMU initialized
rc: BCM driver initialized
rc: L2 Table shadowing enabled
rc: Port modes initialized
BCM.0>
BCM.0>
BCM.0>
BCM.0>
BCM.0> cint CS00012186502_03__SIP_based_gbp_gbpRiot.c
Doing L2 entry add
Get the flow Handle and flow options...
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
Enable the Vxlan Global Configuration...
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
Enable the Access and Network port controls...
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
Enable the Riot Global Configuration...
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int $$ = 0 (0x0)
Create the Multicast group...
int $$ = 0 (0x0)
Create the Flow VPN...
int $$ = 0 (0x0)
bcm_vlan_t vpn = 28673 (0x7001)
Create the Access Flow Port...
int $$ = 0 (0x0)
int access_flow_port = -1342177279 (0xb0000001)
Create the Network Flow Port...
int $$ = 0 (0x0)
int network_flow_port = -1342177278 (0xb0000002)
Create the L3 Interface...
int $$ = 0 (0x0)
Create the L3 Egress Interface...
int $$ = 0 (0x0)
Create the L3 interface for the overlay local mac...
int $$ = 0 (0x0)
Create the L3 Egress object for Overlay remote mac...
int $$ = 0 (0x0)
Allow L3 IPv4 on the Access flow port...
int $$ = 0 (0x0)
Add the flow match...
Add the flow match...
int $$ = 0 (0x0)
Set the SGPP controls...
int $$ = 0 (0x0)
Assign GBP..
int $$ = 0 (0x0)
int $$ = 0 (0x0)
Create the ingress interface...
int $$ = 0 (0x0)
int ing_if_id = 2 (0x2)
int $$ = 0 (0x0)
Set the Flow Encap...
int $$ = 0 (0x0)
Add the Encapsulation...
int $$ = 0 (0x0)
0:bcmi_esw_flow_dvp_vlan_xlate_key_set: DVP 2 is for flex flow, TBD
int $$ = 0 (0x0)
Create the Flow Tunnel...
int $$ = 0 (0x0)
int $$ = 0 (0x0)
int sid = 33554433 (0x2000001)
Add the L3 Host Add...
int $$ = 0 (0x0)
Add the L2 address...
B_CM.0> a
Attach: Unit 0 (BCM56275_A1): attached (current unit)


##########   Sending IPv4 packet ########
BEFORE: tx 1 pbm=1 data=0000000033000000000044008100001508004500LLLL000000004011SSSSC0A801010A0A0A01C1C1D1D10036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2222222222222222222222
B_CM.0> tx 1 pbm=1 data=000000003300000000004400810000150800450000A7000000004011A492C0A801010A0A0A01C1C1D1D10036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2222222222222222222222
Packet from data=<>, length=193
B_CM.0> sleep 1
Sleeping for 1 second
[bcmPW.0]
[bcmPW.0]Packet[1]: data[0000]: {000000003300} {000000004400} 8100 0015
[bcmPW.0]Packet[1]: data[0010]: 0800 4500 00a7 0000 0000 4011 a492 c0a8
[bcmPW.0]Packet[1]: data[0020]: 0101 0a0a 0a01 c1c1 d1d1 0036 c6b0 0001
[bcmPW.0]Packet[1]: data[0030]: 0203 0405 0607 0809 0a0b 0c0d 0e0f 1011
[bcmPW.0]Packet[1]: data[0040]: 1213 1415 1617 1819 1a1b 1c1d 1e1f 2021
[bcmPW.0]Packet[1]: data[0050]: 2223 2425 2627 2829 2a2b 2c2d 2919 7737
[bcmPW.0]Packet[1]: data[0060]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[1]: data[0070]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[1]: data[0080]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[1]: data[0090]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[1]: data[00a0]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[1]: data[00b0]: cccc c222 2222 2222 2222 2222 2207 c209
[bcmPW.0]Packet[1]: data[00c0]: 00
[bcmPW.0]Packet[1]: length 193 (193). rx-port 1. cos 0. prio_int 0. vlan 28673. reason 0x1. Outer tagged.
[bcmPW.0]Packet[1]: dest-gport -1342177278. src-gport -1342177279. opcode 1.  matched 222. classification-tag 0.
[bcmPW.0]Packet[1]: reasons: FilterMatch
[bcmPW.0]
[bcmPW.0]Packet[2]: data[0000]: {000000000002} {000000002222} 8100 0016
[bcmPW.0]Packet[2]: data[0010]: 0800 4500 00e1 0000 0000 3f11 a557 0a0a
[bcmPW.0]Packet[2]: data[0020]: 0a02 c0a8 0101 ffff 12b5 00cd 0000 8800
[bcmPW.0]Packet[2]: data[0030]: 2222 0123 4500 0000 0000 aaaa 0000 0000     >>>>>>>> 2222 = GBP
[bcmPW.0]Packet[2]: data[0040]: bbbb 8100 001e 0800 4500 00a7 0000 0000
[bcmPW.0]Packet[2]: data[0050]: 3f11 a592 c0a8 0101 0a0a 0a01 c1c1 d1d1
[bcmPW.0]Packet[2]: data[0060]: 0036 c6b0 0001 0203 0405 0607 0809 0a0b
[bcmPW.0]Packet[2]: data[0070]: 0c0d 0e0f 1011 1213 1415 1617 1819 1a1b
[bcmPW.0]Packet[2]: data[0080]: 1c1d 1e1f 2021 2223 2425 2627 2829 2a2b
[bcmPW.0]Packet[2]: data[0090]: 2c2d 2919 7737 cccc cccc cccc cccc cccc
[bcmPW.0]Packet[2]: data[00a0]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[2]: data[00b0]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[2]: data[00c0]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[2]: data[00d0]: cccc cccc cccc cccc cccc cccc cccc cccc
[bcmPW.0]Packet[2]: data[00e0]: cccc cccc cccc cccc c222 2222 2222 2222
[bcmPW.0]Packet[2]: data[00f0]: 2222 222f ae05 85
[bcmPW.0]Packet[2]: length 247 (247). rx-port 2. cos 0. prio_int 0. vlan 22. reason 0x1. Outer tagged.
[bcmPW.0]Packet[2]: dest-port 0. dest-mod 0. src-port 2. src-mod 0. opcode 0.  matched 223. classification-tag 0.
[bcmPW.0]Packet[2]: reasons: FilterMatch
#####################################################

bcm_error_t $$ = BCM_E_NONE (0)
BCM.0>
BCM.0>
BCM.0>
BCM.0> config show
    phy_port_primary_and_offset_4=0x0103
    portmap_29.0=29:2.5
    portmap_4.0=4:1
    phy_port_primary_and_offset_5=0x0500
    portmap_10.0=10:1
    portmap_38.0=38:2.5
    phy_port_primary_and_offset_6=0x0501
    portmap_47.0=51:1
    phy_port_primary_and_offset_7=0x0502
    portmap_5.0=5:1
    phy_port_primary_and_offset_8=0x0503
    cache_coherency_check=0
    portmap_11.0=11:1
    portmap_65.0=65:20
    portmap_39.0=39:2.5
    phy_port_primary_and_offset_9=0x0900
    portmap_20.0=20:1
    portmap_48.0=52:1
    flowtracker_ipfix_observation_domain_id=1
    portmap_6.0=6:1
    portmap_12.0=12:1
    portmap_66.0=66:20
    portmap_21.0=21:1
    portmap_49.0=57:25
    l3_ecmp_levels=2
    portmap_30.0=30:2.5
    portmap_7.0=7:1
    portmap_13.0=13:1
    portmap_67.0=67:20
    riot_overlay_l3_egress_mem_size=16384
    portmap_22.0=22:1
    portmap_31.0=31:2.5
    portmap_8.0=8:1
    portmap_40.0=40:2.5
    portmap_14.0=14:1
    portmap_68.0=68:20
    portmap_23.0=23:1
    pbmp_xport_xe=0x01FFFFFFFFFFFFE
    os=unix
    portmap_32.0=32:2.5
    portmap_9.0=9:1
    portmap_41.0=41:1
    portmap_15.0=15:1
    core_clock_to_pm_clock_factor=1
    phy_chain_rx_polarity_flip_physical{29.0}=1
    portmap_50.0=58:25
    portmap_24.0=24:1
    port_gmii_mode{41}=1
    port_flex_enable=1
    portmap_33.0=33:2.5
    flowtracker_num_unique_user_entry_keys=10
    phy_port_primary_and_offset_10=0x0901
    riot_enable=1
    portmap_42.0=42:1
    portmap_16.0=16:1
    port_gmii_mode{25}=2
    l2delete_chunks=4096
    phy_port_primary_and_offset_11=0x0902
    portmap_51.0=59:25
    portmap_25.0=25:2.5
    riot_overlay_l3_intf_mem_size=1024
    phy_port_primary_and_offset_20=0x1103
    phy_port_primary_and_offset_12=0x0903
    portmap_34.0=34:2.5
    core_clock_frequency=495
    phy_port_primary_and_offset_21=0x1500
    phy_port_primary_and_offset_13=0x0d00
    portmap_43.0=43:1
    portmap_17.0=17:1
    l2xmsg_mode=1
    phy_port_primary_and_offset_22=0x1501
    phy_port_primary_and_offset_14=0x0d01
    portmap_52.0=60:25
    portmap_26.0=26:2.5
    portmap_1.0=1:1
    phy_port_primary_and_offset_23=0x1502
    phy_port_primary_and_offset_15=0x0d02
    portmap_35.0=35:2.5
    ifp_inports_support_enable=1
    phy_port_primary_and_offset_24=0x1503
    phy_port_primary_and_offset_16=0x0d03
    portmap_44.0=44:1
    portmap_18.0=18:1
    flow_init_mode=1
    portmap_53.0=61:20
    phy_port_primary_and_offset_17=0x1100
    portmap_27.0=27:2.5
    portmap_2.0=2:1
    portmap_36.0=36:2.5
    phy_port_primary_and_offset_18=0x1101
    portmap_45.0=49:1
    portmap_19.0=19:1
    phy_port_primary_and_offset_19=0x1102
    phy_port_primary_and_offset_1=0x0100
    portmap_54.0=63:20
    portmap_28.0=28:2.5
    phy_port_primary_and_offset_2=0x0101
    portmap_3.0=3:1
    portmap_37.0=37:2.5
    phy_port_primary_and_offset_3=0x0102
    portmap_46.0=50:1
BCM.0>
BCM.0>
BCM.0>
BCM.0> ps
                 ena/        speed/ link auto    STP                  lrn  inter   max   cut   loop
           port  link  Lns   duplex scan neg?   state   pause  discrd ops   face frame  thru?  back   encap
       ge0(  1)  up     1    1G  HD   SW  Yes  Forward  TX RX   None   FA   GMII 12284         MAC    IEEE
       ge1(  2)  up     1    1G  HD   SW  Yes  Forward  TX RX    All   FA   GMII 12284         MAC    IEEE
       ge2(  3)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
       ge3(  4)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
       ge4(  5)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
       ge5(  6)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
       ge6(  7)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
       ge7(  8)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
       ge8(  9)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
       ge9( 10)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge10( 11)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge11( 12)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge12( 13)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge13( 14)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge14( 15)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge15( 16)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge16( 17)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge17( 18)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge18( 19)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge19( 20)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge20( 21)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge21( 22)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge22( 23)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge23( 24)  down   1     -       SW  Yes  Forward  TX RX   None   FA   GMII 12284                IEEE
      ge24( 25)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge25( 26)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge26( 27)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge27( 28)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge28( 29)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge29( 30)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge30( 31)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge31( 32)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge32( 33)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge33( 34)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge34( 35)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge35( 36)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge36( 37)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge37( 38)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge38( 39)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge39( 40)  down   1  2.5G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge40( 41)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge41( 42)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge42( 43)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge43( 44)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge44( 45)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge45( 46)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge46( 47)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
      ge47( 48)  down   1    1G  FD   SW  Yes  Forward  TX RX   None   FA  SGMII 12284                IEEE
       xe0( 49)  down   1   25G  FD   SW  No   Forward  TX RX   None   FA     CR 12284                IEEE
       xe1( 50)  down   1   25G  FD   SW  No   Forward  TX RX   None   FA     CR 12284                IEEE
       xe2( 51)  down   1   25G  FD   SW  No   Forward  TX RX   None   FA     CR 12284                IEEE
       xe3( 52)  down   1   25G  FD   SW  No   Forward  TX RX   None   FA     CR 12284                IEEE
       hg0( 53)  down   2   20G  FD   SW  No   Forward          None   FA    CR2 12288              HIGIG2
       hg1( 54)  down   2   20G  FD   SW  No   Forward          None   FA    CR2 12288              HIGIG2
BCM.0>
BCM.0>
BCM.0>
BCM.0> show pmap
             pipe   logical  physical    idb mmu   ucast_Qbase/Numq  mcast_Qbase/Numq  half-pipe
      cpu0      0     0         0        70  70         0/0               1290/48          0
       ge0      0     1         1         0   8       208/26               208/26          0
       ge1      0     2         2         1   9       234/26               234/26          0
       ge2      0     3         3         2  10       260/26               260/26          0
       ge3      0     4         4         3  11       286/26               286/26          0
       ge4      0     5         5         4  12       312/26               312/26          0
       ge5      0     6         6         5  13       338/26               338/26          0
       ge6      0     7         7         6  14       364/26               364/26          0
       ge7      0     8         8         7  15       390/26               390/26          0
       ge8      0     9         9         8  16       416/16               416/16          0
       ge9      0    10        10         9  17       432/16               432/16          0
      ge10      0    11        11        10  18       448/16               448/16          0
      ge11      0    12        12        11  19       464/16               464/16          0
      ge12      0    13        13        12  20       480/16               480/16          0
      ge13      0    14        14        13  21       496/16               496/16          0
      ge14      0    15        15        14  22       512/16               512/16          0
      ge15      0    16        16        15  23       528/16               528/16          0
      ge16      0    17        17        16  24       544/16               544/16          0
      ge17      0    18        18        17  25       560/16               560/16          0
      ge18      0    19        19        18  26       576/16               576/16          0
      ge19      0    20        20        19  27       592/16               592/16          0
      ge20      0    21        21        20  28       608/16               608/16          0
      ge21      0    22        22        21  29       624/16               624/16          0
      ge22      0    23        23        22  30       640/16               640/16          0
      ge23      0    24        24        23  31       656/16               656/16          0
      ge24      0    25        25        24  32       672/16               672/16          0
      ge25      0    26        26        25  33       688/16               688/16          0
      ge26      0    27        27        26  34       704/16               704/16          0
      ge27      0    28        28        27  35       720/16               720/16          0
      ge28      0    29        29        28  36       736/16               736/16          0
      ge29      0    30        30        29  37       752/16               752/16          0
      ge30      0    31        31        30  38       768/16               768/16          0
      ge31      0    32        32        31  39       784/16               784/16          0
      ge32      0    33        33        32  40       800/16               800/16          0
      ge33      0    34        34        33  41       816/16               816/16          0
      ge34      0    35        35        34  42       832/16               832/16          0
      ge35      0    36        36        35  43       848/16               848/16          0
      ge36      0    37        37        36  44       864/16               864/16          0
      ge37      0    38        38        37  45       880/16               880/16          0
      ge38      0    39        39        38  46       896/16               896/16          0
      ge39      0    40        40        39  47       912/16               912/16          0
      ge40      0    41        41        40  48       928/16               928/16          0
      ge41      0    42        42        41  49       944/16               944/16          0
      ge42      0    43        43        42  50       960/16               960/16          0
      ge43      0    44        44        43  51       976/16               976/16          0
      ge44      0    45        49        48  56      1056/16              1056/16          0
      ge45      0    46        50        49  57      1072/16              1072/16          0
      ge46      0    47        51        50  58      1088/16              1088/16          0
      ge47      0    48        52        51  59      1104/16              1104/16          0
       xe0      0    49        57        56   0         0/26                 0/26          0
       xe1      0    50        58        57   1        26/26                26/26          0
       xe2      0    51        59        58   2        52/26                52/26          0
       xe3      0    52        60        59   3        78/26                78/26          0
       hg0      0    53        61        60   4       104/26               104/26          0
       hg1      0    54        63        62   6       156/26               156/26          0
     msec0      0    65        65        65  65      1200/16              1200/16          0
     msec1      0    66        66        66  66      1216/16              1216/16          0
     msec2      0    67        67        67  67      1232/16              1232/16          0
     msec3      0    68        68        68  68      1248/16              1248/16          0
       lb0      0    70        69        71  71         0/0               1280/10          0
      fae0      0    71        70        69  69         0/0                  0/0           0
BCM.0>
BCM.0>
BCM.0>
BCM.0> ver
Broadcom Command Monitor: Copyright (c) 1998-2021 Broadcom
Release: sdk-6.5.22 built 20210503 (Mon May  3 03:13:47 2021)
From sr936857@xl-sj1-30:/projects/ntsw-sw-ae/home/sr936857/git-code/sdk6
Platform: SLK_BCM957812
OS: Unix (Posix)
Chips: BCM56504_A0, BCM56504_B0, BCM56314_A0, BCM56800_A0,
       BCM56514_A0, BCM56624_A0, BCM56624_B0, BCM56680_A0,
       BCM56680_B0, BCM56224_A0, BCM56224_B0, BCM56820_A0,
       BCM56725_A0, BCM53314_A0, BCM53324_A0, BCM56634_A0,
       BCM56634_B0, BCM56524_A0, BCM56524_B0, BCM56685_A0,
       BCM56685_B0, BCM56334_A0, BCM56334_B0, BCM56840_A0,
       BCM56840_B0, BCM56440_A0,
       BCM56440_B0, BCM56640_A0,
       BCM56850_A0, BCM56450_A0, BCM56450_B0,
       BCM56450_B1, BCM56340_A0, BCM56142_A0, BCM56150_A0,
       BCM53400_A0, BCM56960_A0, BCM56860_A0, BCM56560_A0,
       BCM56560_B0, BCM56260_A0, BCM56260_B0,



       BCM56160_A0, BCM56270_A0,
       BCM56965_A0, BCM56970_A0, BCM56870_A0, BCM56870_B0,
       BCM53570_A0, BCM53570_B0, BCM56980_A0, BCM56980_B0,
       BCM53540_A0, BCM56670_A0, BCM56670_B0, BCM56670_C0,
       BCM56370_A0, BCM56275_A0, BCM56770_A0, BCM56470_A0,
       BCM56070_A0
PHYs:  BCM5218, BCM5220/21, BCM5226, BCM5228,
    BCM5238, BCM5248, BCM5400, BCM5401,
    BCM5402, BCM5404, BCM5424/34, BCM5411,
    BCM5461, BCM5464, BCM5466, BCM5478,
    BCM5488, BCM54980, BCM54980, BCM54980,
    BCM54980, BCM53314, BCM5482/801x, BCM54684,
    BCM54640, BCM54682E, BCM54684E, BCM54685,
    BCM54616, BCM54618E, BCM54618SE, BCM84707,
    BCM84073, BCM84074, BCM84728, BCM84748,
    BCM84727, BCM84747, BCM84762, BCM84764,
    BCM84042, BCM84044, BCM8806X, BCM54182,
    BCM54185, BCM54180, BCM54140, BCM54192,
    BCM54195, BCM54190, BCM54194, BCM54210,
    BCM54220, BCM54280, BCM54282, BCM54240,
    BCM54285, BCM5428X, BCM54290, BCM54292,
    BCM54294, BCM54295, BCM54296, BCM5421S
    BCM54680, BCM53324, BCM56160-GPHY, BCM53540-GPHY,
    BCM56275-GPHY, BCM54880, BCM54881, BCM54810,
    BCM54811, BCM54640E, BCM54880E, BCM54680E,
    BCM52681E, BCM8703, BCM8704, BCM8705/24/25,
    BCM8706/8726, BCM8727, BCM8728/8747, BCM8072,
    BCM8073, BCM8074, BCM8040, BCM8481X,
    BCM84812, BCM84821, BCM84822, BCM84823,
    BCM84833, BCM84834, BCM84835, BCM84836,
    BCM84844, BCM84846, BCM84848, BCM84858,
    BCM84856, BCM84860, BCM84861, BCM84864,
    BCM84868, BCM84888, BCM84884, BCM84888E,
    BCM84884E, BCM84881, BCM84880, BCM84888S,
    BCM84887, BCM84886, BCM84885, BCM8750,
    BCM8752, BCM8754, BCM84740, BCM84164,
    BCM84758, BCM84780, BCM84784, BCM84318,
    BCM84328, BCM84793, BCM82328, Furia
    Huracan, Sesto, Madura, BCM82780,
    copper sfp

BCM.0>
BCM.0>
BCM.0>
BCM.0> fp show
FP:     unit 0:
PIPELINE STAGE FLOWTRACKER
FP:           :tcam_sz=508(0x1fc), tcam_slices=1, tcam_ext_numb=0,
PIPELINE STAGE INGRESS
FP:           :tcam_sz=18432(0x4800), tcam_slices=18, tcam_ext_numb=0,
PIPELINE STAGE EGRESS
FP:           :tcam_sz=1024(0x400), tcam_slices=4, tcam_ext_numb=0,
PIPELINE STAGE LOOKUP
FP:           :tcam_sz=1024(0x400), tcam_slices=4, tcam_ext_numb=0,
GID       9999: gid=0x270f, instance=0 mode=Single, stage=Ingress lookup=Enabled, ActionResId={-1}, pbmp={0x000000000000000000000000000000000000000000000000007fffffffffffff}
         qset={InPort, Stage},
         aset={CopyToCpu, PolicerGroup, StatGroup},

         group_priority= -9999
         slice_primary =  {slice_number=1, Entry count=1024(0x400), Entry free=1023(0x3ff)},
         group_status={prio_min=-2147483647, prio_max=2147483647, entries_total=17408, entries_free=17407,
                       counters_total=20480, counters_free=19455, meters_total=6144, meters_free=6144}
EID 0x0000270f: gid=0x270f,
 slice=1, slice_idx=0, part=0, prio=0, flags=0x810602, Installed, Enabled, color_indep=1

 InPort
    Part:0 Offset0: 16 Width0:   4
    Part:0 Offset1: 36 Width1:   4
    DATA=0x00000002
    MASK=0x000000ff
         action={act=CopyToCpu, param0=1(0x1), param1=223(0xdf), param2=0(0), param3=0(0)}
         policer=
         statistics={stat id 9999  slice = 9 idx=3 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
GID       9998: gid=0x270e, instance=0 mode=Single, stage=Ingress lookup=Enabled, ActionResId={-1}, pbmp={0x000000000000000000000000000000000000000000000000007fffffffffffff}
         qset={InPort, Stage},
         aset={CopyToCpu, PolicerGroup, StatGroup},

         group_priority= -9998
         slice_primary =  {slice_number=0, Entry count=1024(0x400), Entry free=1023(0x3ff)},
         group_status={prio_min=-2147483647, prio_max=2147483647, entries_total=17408, entries_free=17407,
                       counters_total=20480, counters_free=19455, meters_total=6144, meters_free=6144}
EID 0x0000270e: gid=0x270e,
 slice=0, slice_idx=0, part=0, prio=0, flags=0x810602, Installed, Enabled, color_indep=1

 InPort
    Part:0 Offset0: 16 Width0:   4
    Part:0 Offset1: 36 Width1:   4
    DATA=0x00000001
    MASK=0x000000ff
         action={act=CopyToCpu, param0=1(0x1), param1=222(0xde), param2=0(0), param3=0(0)}
         policer=
         statistics={stat id 9998  slice = 8 idx=3 entries=1}{Packets}{Bytes}
         Extended statistics=NULL
BCM.0>
BCM.0>
BCM.0> l2 show
mac=00:00:00:00:22:22 vlan=22 GPORT=0x0 modid=0 port=0/cpu0 Static Hit CPU
mac=00:00:00:00:44:00 vlan=28673 GPORT=0xb0000001 port=0xb0000001(flow) Hit
BCM.0>
BCM.0>
BCM.0>

*/