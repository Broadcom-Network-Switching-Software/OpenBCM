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
 *                   |                              |  VXLAN header with GBP assigned by L2 table
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
 *      Create a vxlan flex flow based tunnel. Let L2 table assign GBP for vxlan tunnel header
 *      based on smac of the packet coming in from access port
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
void bbshell(int unit, char *str)                                          /* */
{                                                                          /* */
  printf("B_CM.%d> %s\n", unit, str);                                      /* */
  bshell(unit, str);                                                       /* */
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
/* Vxlan VXLAN vxlan definitions */
bcm_mac_t remote_mac_acc = "00:00:00:00:00:01";
bcm_mac_t local_mac_acc = "00:00:00:00:11:11";
bcm_mac_t remote_mac_network_1 = "00:00:00:00:00:02";
bcm_mac_t local_mac_network1 = "00:00:00:00:22:22";
bcm_mac_t payload_remote_mac1 = "00:00:00:00:aa:aa";
bcm_mac_t payload_local_mac = "00:00:00:00:bb:bb";
bcm_flow_tunnel_terminator_t tnl_term;

bcm_ip_t    test_sip = 0x10101011;
bcm_ip_t    test_dip = 0x20202021;
bcm_ip_t    test_ip_mask = 0xFFFFFFFF;

uint16      test_L4_src_port = 0x1111;
uint16      test_L4_dst_port = 0x3333;

uint16      gbp_sid_value    = 0x2222;

uint32 vnid = 0x12345;
bcm_vlan_t vid_acc = 21;
bcm_vlan_t vid_network_1 = 22;
bcm_if_t intf_id_acc = 1;
bcm_if_t intf_id_network_1 = 2;
bcm_if_t intf_id_network_2 = 3;
uint8 ttl = 16;
bcm_vrf_t vrf = 0;
bcm_multicast_t bcast_group = 0;    /* For VXLAN access and network VPs */

bcm_port_t port_acc = portGetNextE(unit, PORT_ANY);
bcm_port_t port_network_1 = portGetNextE(unit, PORT_ANY);


bcm_gport_t gport_acc = BCM_GPORT_INVALID;
bcm_gport_t gport_network_1 = BCM_GPORT_INVALID;

print bcm_port_gport_get(unit, port_acc, &gport_acc);
print bcm_port_gport_get(unit, port_network_1, &gport_network_1);

bcm_pbmp_t pbmp, upbmp;
uint32 flags;
bcm_flow_vpn_config_t vpn_info;
bcm_flow_port_t flow_port;
bcm_l3_intf_t l3_intf;
bcm_if_t egr_obj_acc1, egr_obj_network1;
bcm_l3_egress_t l3_egress;
uint32 flags_temp;
bcm_vpn_t vpn;
int acc_flow_port;
int net_flow_port;
bcm_l2_addr_t l2_addr;
bcm_l2_station_t l2_station;
int station_id;
bcm_ip_t tnl_local_ip = 0x0A0A0A01;
bcm_ip_t tnl_remote_ip1 = 0xC0A80101;   /* 192.168.1.1 */
uint16 udp_dst_port = 4789; // 8472;                /* Destination UDP port */
uint16 udp_src_port = 0xFFFF;                /* Source UDP port */

uint32 flow_handle, flow_handle_gbp;
uint32 dvp_flow_option, encap_flow_option;
bcm_flow_logical_field_t lfield[10];

bcm_error_t
    TestFunc()
{
    printf("Doing vxlan global settings...\n");
    /* Switch control to enable VXLAN */
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port_acc, bcmPortControlVxlanEnable, 0) );
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port_acc,
                                                           bcmPortControlVxlanTunnelbasedVnId, 0) );
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port_network_1, bcmPortControlVxlanEnable, 1) );
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port_network_1,
                                                           bcmPortControlVxlanTunnelbasedVnId, 0) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, udp_dst_port) );
    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchVxlanTunnelMissToCpu, 1) );
    BCM_IF_ERROR_RETURN( bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE) );
    BCM_IF_ERROR_RETURN( bcm_vlan_control_port_set(0, port_acc, bcmVlanTranslateIngressEnable, 1) );

    printf("Doing L3 interface create...\n");
    bcm_l3_intf_t_init(&l3_intf);
    //l3_intf.l3a_flags =       BCM_L3_WITH_ID;
    l3_intf.l3a_intf_id =     intf_id_network_1;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac_network1, sizeof(local_mac_network1));
    l3_intf.l3a_vid =         vid_network_1;
    l3_intf.l3a_vrf =         vrf;
    BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf) );
    intf_id_network_1 = l3_intf.l3a_intf_id;

    printf("Doing egress object create...\n");
    /* Egress object for network port 1 */
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2 =      BCM_L3_FLOW_ONLY;
    l3_egress.intf =       intf_id_network_1;
    sal_memcpy(l3_egress.mac_addr,  remote_mac_network_1, sizeof(remote_mac_network_1));
    l3_egress.vlan =       vid_network_1;
    l3_egress.port =       gport_network_1;
    BCM_IF_ERROR_RETURN( bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_network1) );

    printf("Doing VPN create...\n");
    /* VPN create */
    /* Multicast group for non-UC */
    bcm_flow_vpn_config_t_init(&vpn_info);
    flags  = BCM_MULTICAST_TYPE_FLOW;
    print bcm_multicast_create(unit, flags, &bcast_group);
    vpn_info.flags = BCM_FLOW_VPN_ELAN;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_multicast_group = bcast_group;
    vpn_info.unknown_unicast_group = bcast_group;
    BCM_IF_ERROR_RETURN( bcm_flow_vpn_create(unit, &vpn,&vpn_info) );
    printf("vpn: 0x%x\n",vpn);

    printf("Doing access flow port create in this vpn...\n");
    /* access VP creation*/
    bcm_flow_port_t_init(&flow_port);  
    BCM_IF_ERROR_RETURN( bcm_flow_port_create(unit, vpn, &flow_port) );
    acc_flow_port = flow_port.flow_port_id;
    printf("acc_flow_port: 0x%x\n",acc_flow_port);
    
    printf("Doing network flow port create in this vpn...\n");
    /* network VP creation */
    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = BCM_FLOW_PORT_NETWORK;
    BCM_IF_ERROR_RETURN( bcm_flow_port_create(unit, vpn, &flow_port) );
    net_flow_port = flow_port.flow_port_id;
    printf("net_flow_port: 0x%x\n",net_flow_port);
    
    printf("Doing get handle for relevant flows...\n");
    BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle) );
    BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "VXLAN_GBP", &flow_handle_gbp) );
    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, flow_handle_gbp, "IPV4", &dvp_flow_option) );
    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, flow_handle_gbp, "LOOKUP_VFI_ASSIGN_VNID",
                                                        &encap_flow_option) );
    
    printf("Doing adding of access flow port to these flows...\n");
    /* access port side */
    bcm_flow_match_config_t info;
    bcm_flow_match_config_t_init(&info);
    
    /* generic: port match criteria, flow_handle must be 0, error check */
    /* port/vlan criteria */
    info.criteria = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN;
    info.port = gport_acc;
    info.flow_port = acc_flow_port;
    info.vlan = vid_acc;
    info.valid_elements = BCM_FLOW_MATCH_PORT_VALID |
                          BCM_FLOW_MATCH_VLAN_VALID |
                          BCM_FLOW_MATCH_FLOW_PORT_VALID;
    BCM_IF_ERROR_RETURN( bcm_flow_match_add(unit, &info, 0, NULL) );
    
    printf("Doing encap setting for the vxlan flow...\n");
    /* network port side */
    /* flow_port encap set. Set the flow type matching the tunnel */
    bcm_flow_port_encap_t peinfo;
    bcm_flow_port_encap_t_init(&peinfo);
    
    peinfo.flow_handle = flow_handle_gbp;
    peinfo.flow_option = dvp_flow_option;
    /*peinfo.class_id = 0x3;*/
    peinfo.flow_port = net_flow_port;
    peinfo.egress_if = egr_obj_network1;
    peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_PORT_VALID |
                              BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID|
                              BCM_FLOW_PORT_ENCAP_FLEX_DATA_VALID;
    BCM_IF_ERROR_RETURN( bcm_flow_port_encap_set(0,&peinfo,0,NULL) );
    
    printf("Doing vnid related encap settings for vxlan flow...\n");
    /* encap add, {DVP,VFI}-->VNID */
    bcm_flow_encap_config_t einfo;
    bcm_flow_encap_config_t_init(&einfo);

    einfo.flow_handle = flow_handle_gbp;
    einfo.flow_option = encap_flow_option;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
    einfo.vnid = vnid;
    einfo.vpn = vpn;
    einfo.flow_port = net_flow_port;
    einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID | 
                           BCM_FLOW_ENCAP_VPN_VALID;
    BCM_IF_ERROR_RETURN( bcm_flow_encap_add(unit, &einfo, 0, NULL) );
    
    printf("Doing vxlan tunnel initiator settings...\n");
    /* create a vxlan IP tunnel for the given flow port */
    bcm_flow_tunnel_initiator_t tiinfo;
    bcm_flow_tunnel_initiator_t_init(&tiinfo);
    
    tiinfo.flow_handle = flow_handle;
    tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
                           BCM_FLOW_TUNNEL_INIT_SIP_VALID |
                           BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID |
                           BCM_FLOW_TUNNEL_INIT_UDP_SPORT_VALID;
    tiinfo.flow_port = net_flow_port;
    tiinfo.type = bcmTunnelTypeVxlan;
    tiinfo.udp_src_port = udp_src_port;
    tiinfo.udp_dst_port = udp_dst_port;
    tiinfo.sip = tnl_local_ip;
    tiinfo.dip = tnl_remote_ip1;
    BCM_IF_ERROR_RETURN( bcm_flow_tunnel_initiator_create(unit, &tiinfo, 0, NULL) );
    
    printf("Doing vxlan global settings...\n");
    /* L2 address setup */
    bcm_l2_addr_t_init(&l2_addr, payload_remote_mac1, vpn);
    l2_addr.flags = BCM_L2_STATIC;
    sal_memcpy(l2_addr.mac, payload_remote_mac1, sizeof(payload_remote_mac1));
    l2_addr.vid = vpn;
    l2_addr.port = net_flow_port;
    BCM_IF_ERROR_RETURN( bcm_l2_addr_add(unit, &l2_addr) );
    
    printf("Doing mac sa match in L2 table with gbp assignment...\n");
    /* assign GBP using L2 table */
    bcm_l2_addr_t sa_entry;
    bcm_mac_t macsa = "00:00:00:00:bb:bb";
    bcm_l2_addr_t_init(&sa_entry, macsa, vpn);
    sa_entry.flags = BCM_L2_STATIC;
    sal_memcpy(sa_entry.mac, macsa, sizeof(macsa));
    sa_entry.vid = vpn;
    sa_entry.port = acc_flow_port;
    sa_entry.gbp_src_id = gbp_sid_value;
    BCM_IF_ERROR_RETURN( bcm_l2_addr_add(unit, &sa_entry) );
    
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
        BCM_PBMP_PORT_ADD(vlan_pbmp, port_acc);
        
        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_acc));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_acc, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
        
        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, port_network_1);
        
        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_network_1));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_network_1, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, local_mac_network1, vid_network_1);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = port_acc;
        printf("Doing L2 entry add\n");
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( 1 )
    {
        rv = ingress_port_setup(unit, port_acc, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", port_acc, rv);
            return rv;
        }
        
        rv = egress_port_setup(unit, port_network_1, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", port_network_1, rv);
            return rv;
        }
    }
    else if( 1 ) {
        char cmd[128];
        
        printf("Doing port loopbacks and mirror...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_acc, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port_acc, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network_1, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port_network_1, BCM_PORT_DISCARD_ALL));
        
        sprintf(cmd, "dmirror %d mode=ingress dp=cpu0 dm=0", port_acc); bbshell(unit, cmd);
        sprintf(cmd, "dmirror %d mode=ingress dp=cpu0 dm=0", port_network_1); bbshell(unit, cmd);
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_acc, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port_acc, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network_1, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port_network_1, BCM_PORT_DISCARD_ALL));
    }
    
    //bshell(unit, "pw start report +raw +decode dma");
    bshell(unit, "pw start");
    
    return BCM_E_NONE;
}

bcm_error_t verify()
{
    char cmd[2*1024];
    int prio = 0;
    
    if( 1 )
    {
        printf("##########   Sending L3 packet; Expected a vxlan packet with SID ########\n");
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500LLLL000000004011SSSS%08X%08X%04X%04X0036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737",
            port_acc,
            payload_remote_mac1[0], payload_remote_mac1[1], payload_remote_mac1[2],
            payload_remote_mac1[3], payload_remote_mac1[4], payload_remote_mac1[5],
            payload_local_mac[0], payload_local_mac[1], payload_local_mac[2],
            payload_local_mac[3], payload_local_mac[4], payload_local_mac[5],
            FORM_VID(prio, 0, vid_acc), test_sip, test_dip,
            test_L4_src_port,
            test_L4_dst_port );
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
    
    printf("\n\n\t Expected Result: You will see that vxlan Tunnelled packet has SID assigned..\n\n");
    
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
