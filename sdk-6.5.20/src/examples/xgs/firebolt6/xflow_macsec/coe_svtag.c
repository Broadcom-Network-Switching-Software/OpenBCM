/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *  Feature  : Channelization over ethernet for Firelight MACSEC Encryption
 *
 *  Usage : BCM.0> cint <thisfile.c> YES
 *           E.g.  cint coe_svtag.c
 *
 *  config   : configFb6_coe.bcm
 *
 *              Feature specific Config Variables:
 *                   flow_init_mode=1
 *                   pbmp_subport=0x22222
 *                   num_subports=10
 *                   portmap_1.0=1:100:l:1      # << Port1 connected to Line card # 1
 *                   portmap_5.0=5:100:l:2      # << .... Line card # 2
 *                   portmap_9.0=9:100:l:3      # << .... Line card # 3
 *                   portmap_13.0=13:100:l:4
 *
 *                      Above is small letter 'L' and not '1' indicating linecard number attached
 *                      to the physical port mentioned in portmap. Typically Linecard is Firelight.
 *
 *  Log file : coe_svtag_log.txt
 *
 *
 *  Test Topology : Standalone FB6 device. Firelight is shown for deployment scenario.
 *
 *  non   +--------------+
 *  CoE   |              |     BP
 *  ----->|ce4           |  CoE Port   +------------+
 *  port  |           ce3+------@----->|xe0         |       AP
 *        |              |      |      |            |
 *        |  FIREBOLT6   |      |      |         ge1+---@--> Access
 *        |    FB6       |      |      |            |   |    Port
 *        |              |      |      | FIRELIGHT  |   |
 *        +--------------+      |      |    FL      |   |
 *                              |      +------------+   |
 *                              V                       V
 *                    |DMAC|SMAC|SVTAG|VLAN|        |DMAC|SMAC|OVID|SVTAG|IVID|SECTAG|PAYLOAD|
 *
 *                        +----------------------------------+ 
 *                        |   SVTAG SIG   |  PAYLOAD OFFSET  |
 *                        +---------------+------------------+ 
 *             SVTAG--->  |P|R|DEV|PKT| RS|                  |
 *                        |R|S| # |TYP| VD|     CHANNEL#/    |
 *                        |I|V|   |   |   |     SUBPORT#     |
 *                        | |D| 2b| 2b| 2b|       8b         |
 *                        +-+-+---+---+---+------------------+ 
 *
 *  Summary:
 *  ========
 *      Create a coe subport on coe_port1. Create Flow Encap, so that on receiving, Firelight can
 *      do ACSEC Encryption. Create a egress nexthop so that coe subport is selected for our test
 *      packet as egress port. CoE subport already had Flow Encap.
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Put test ports in loopback.
 *       b) Install FP entries to display ingress and Egress test packets to CPU
 *       c) Start Packet Watcher
 *
 *    2) Step2 - Configuration (Done in TestFunc()).
 *    ===================================================
 *       a) Create test vlan and add test ports into vlan.
 *       b) Create a coe subport. (similar to a VP - when packet come on this port with a given VID,
 *          this port is assigned)
 *       c) Configure subport encapsulation for packets egressing from this subport. A subport ID
 *          will be inserted into the SVTAG header.
 *       d) Confugure EFP to enable SVTAG if egress port type is SVTAG enabled CoE port. It sets
 *          the SVTAG signature
 *
 *
 *    2) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send unicast L3 UDP packet to non_coe_port1 from CPU.
 *       b) Expected Result:
 *          ================
 *          You will see that from coe_port1, an SVTAGged packet is egressing out. It should have a
 *          svtag_sig(0x13) and fl_subport_num2(0x0B). Also, it is meant for encryption as
 *          reflected by the packet type field in the SVTAG.
 *
 * NOTE: Use opt_* variables to change the test variant
 *
 * INTERNAL:
 *       Rack used: BLR: 20.05 AE    Firebolt6/GTS
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

// 1st parameter is YES. E.g. cint thisflie.c YES 1 1
int opt_IsTestConfig = 0;

print ARGC;

/* Which Stage? VFP:1  IFP:2  EFP:3 */
if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_IsTestConfig = 1;
    } else {
        opt_IsTestConfig = 0;
    }
}

print opt_IsTestConfig;

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
int unit = 0;

bcm_vlan_t  test_vid = 0x64;
bcm_vlan_t  test_vid65 = 0x65;

bcm_mac_t   test_mac_dd = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t   test_mac_cc = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};
bcm_mac_t   test_mac_aa = {0x00, 0x00, 0x00, 0x00, 0x00, 0xAA};
bcm_mac_t   test_mac_bb = {0x00, 0x00, 0x00, 0x00, 0x00, 0xBB};
bcm_mac_t   test_mac_ee = {0x00, 0x00, 0x00, 0x00, 0x00, 0xEE};

bcm_mac_t   test_mac_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bcm_ip_t    test_sip = 0x10101011;
bcm_ip_t    test_dip = 0x20202021;
bcm_ip_t    test_ip3 = 0x20202031;
bcm_ip_t    test_ip_mask = 0xFFFFFFFF;

uint16      test_L4_src_port = 0x1111;
uint16      test_L4_dst_port = 0x2222;

int         statid_used = 0;
char        *stat_desc[32];

bcm_port_t cpu_port = 0;

bcm_port_t ing_port1 = 1; // Set to a valid port if known already.
bcm_port_t ing_port2 = 5;
bcm_port_t non_coe_port1 = 17;

bcm_port_t coe_port1 = 13;

/* Populate the XE/CE/CD ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);
non_coe_port1 = portGetNextE(unit, non_coe_port1);

coe_port1 = portGetNextE(unit, coe_port1);

print ing_port1;
print ing_port2;
print non_coe_port1;
print coe_port1;

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

// This is the subport number from which packet should egress from firelight finally
// In-Sync with Firelight scripts  ----   START
int fl_subport_num1 = 0x0A;   // This is for matching of ingressing packet into FB6. For decap case.
int fl_subport_num2 = 0x0B;    // In-Sync with Firelight scripts

uint32 svtag_sig = 0x13;    // In-Sync with Firelight scripts
// In-Sync with Firelight scripts  ----   END

/* For traffic from Firelite to FB6; Ingress to FB6 */
bcm_error_t create_subport(int vlan, int phy_port, bcm_gport_t *subport_gport)
{

    int unit = 0;
    bcm_stk_modid_config_t mod_cfg;
    int subtag_vlan = fl_subport_num1; /* Used to decap subtag's subport number. */
                                    // If this is the subport number in the ingress svtagged pkt,
                                    // go for decap in FB6
    
    mod_cfg.modid = 5;
    mod_cfg.num_ports = 10;
    mod_cfg.modid_type = bcmStkModidTypeCoe;

    printf("Doing stk modid add...\n");
    BCM_IF_ERROR_RETURN( bcm_stk_modid_config_add(unit, &mod_cfg) );

    int                 port = phy_port;  /* Physical port number */
    bcm_gport_t         port_gport;
    //uint16              subport_tag_tpid = 0x8100;

    printf("Doing port ctrl set SubportTagEnable...port=%d\n", port);
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port, bcmPortControlSubportTagEnable, 1) );
    
    printf("Doing gport get...\n");
    BCM_IF_ERROR_RETURN( bcm_port_gport_get(unit, port, &port_gport) );
    
    bcm_subport_group_config_t      subport_group_cfg;
    bcm_gport_t                     subport_group_gport;
    
    subport_group_cfg.port  = port_gport;
    subport_group_cfg.flags = BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG;
    printf("Doing subport group create...\n");
    BCM_IF_ERROR_RETURN( bcm_subport_group_create(unit, &subport_group_cfg, &subport_group_gport) );
    print subport_group_gport;
    
    bcm_subport_config_t       subport_cfg;
    /* add subports to subport group */
    bcm_subport_config_t_init(&subport_cfg);
    subport_cfg.group = subport_group_gport;
     /* PHB */
    subport_cfg.prop_flags  = BCM_SUBPORT_PROPERTY_PHB;
    subport_cfg.int_pri     = 2;
    subport_cfg.color       = 2;
    subport_cfg.subport_modport = 0;
    subport_cfg.pkt_vlan = subtag_vlan;
    printf("Doing subport port add...\n");
    BCM_IF_ERROR_RETURN( bcm_subport_port_add(unit, &subport_cfg, subport_gport) );
    print *subport_gport;
    
    printf("Doing port vlan membership set...\n");
    BCM_IF_ERROR_RETURN( bcm_port_vlan_member_set(unit, *subport_gport,
                                BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS) );
    
    printf("Doing port vlan membership set for vlan= %d \n", vlan);
    BCM_IF_ERROR_RETURN( bcm_vlan_gport_add(unit, vlan, *subport_gport, 0) );
    
    return BCM_E_NONE;

} /* End - Create_subport */


bcm_error_t
    create_vlan_and_add_ports(int unit, int vid, int tport, int uport) {
    bcm_pbmp_t pbmp;
    bcm_pbmp_t upbmp;
    int status;

    printf("Doing vlan create %d...\n", vid);
    status = bcm_vlan_create(unit, vid);
    if (status < 0 && status != BCM_E_EXISTS) { /* -8 is EEXISTS*/
        return status;
    }
    BCM_PBMP_PORT_ADD(pbmp, tport);
    BCM_PBMP_PORT_ADD(upbmp, uport);
    printf("Doing vlan port add...\n");
    BCM_IF_ERROR_RETURN( bcm_vlan_port_add(unit, vid, pbmp,upbmp) );
    
    return BCM_E_NONE;
}

bcm_error_t
    config_efp(int unit)
{
    bcm_field_group_config_t group_config;
    uint32 data, mask;
    bcm_field_entry_t entry;
    
    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_ASET_INIT(group_config.aset);

    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyEgressPortCtrlType);
    
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionSubportSvtagSignature);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionSubportSvtagPrio);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionSubportSvtagEnable);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionSubportSvtagSubportNum);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionSubportSvtagPktType);
    
    group_config.priority = 500;
    
    printf("Doing efp group create...\n");
    BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_config) );
    
    printf("Doing efp entry create...\n");
    BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_config.group, &entry) );
    print entry;
    
    printf("Doing qualify EgressPortCtrlType...\n");
    /* Qualify the egress port type to add SVTAG. */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_EgressPortCtrlType(unit, entry, 
                                                bcmFieldEgressPortCtrlTypeSubportSvtagEncrypt) );
    
    printf("Doing action add SubportSvtagSignature...\n");
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionSubportSvtagSignature,
                                                                                svtag_sig, 0) );
    printf("Doing action add SubportSvtagPrio...\n");
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionSubportSvtagPrio, 0, 0) );
    printf("Doing action add SubportSvtagEnable...\n");
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionSubportSvtagEnable,
                                                                                0, 0) );
    if( 0 ) {
        printf("Doing action add SvtagSubportNum...\n");
        BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionSubportSvtagSubportNum,
                                                                                    4, 0) );
    }
    if( 0 ) {   // '0': Done by BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT
        printf("Doing action add SubportSvtagPktType...\n");
        BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionSubportSvtagPktType,
                                                               bcmFieldMacSecPktTypeMacSec, 0) );
    }
    printf("Doing entry install...\n");
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit,entry) );
    
    return BCM_E_NONE;
}

/* For traffic from FB6 to Firelite */
bcm_error_t configure_flex_macsec_encoding(int vlan, bcm_gport_t subport_gport)
{

    int unit =0;
    int pri = 0;
    bcm_flow_handle_t handle;
    bcm_flow_option_id_t option_id;
    bcm_flow_encap_config_t encap_config;
    
    
    bcm_flow_logical_field_t logical_fields;
    bcm_flow_logical_field_t_init(&logical_fields);
    bcm_flow_field_id_t field_id;
    
    printf("Doing flow handle get...\n");
    BCM_IF_ERROR_RETURN( bcm_flow_handle_get(unit, "SUBPORT_SVTAG_ENCRYPT", &handle) );

    printf("Doing flow option id get...\n");
    BCM_IF_ERROR_RETURN( bcm_flow_option_id_get(unit, handle,
                        "LOOKUP_DGPP_OVLAN_INTPRI_ASSIGN_CLASS_ID", &option_id) );
    bcm_flow_encap_config_t_init(&encap_config);
    encap_config.flow_handle = handle;
    encap_config.flow_option = option_id;
    encap_config.flow_port = subport_gport; /* Destination port number */
    encap_config.flags |= BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT;
    encap_config.vlan = vlan;
    encap_config.pri=pri;
    encap_config.class_id = fl_subport_num2;
    encap_config.criteria = BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI;
    encap_config.valid_elements = BCM_FLOW_ENCAP_FLOW_PORT_VALID;
    encap_config.valid_elements |= BCM_FLOW_ENCAP_CLASS_ID_VALID;
    encap_config.valid_elements |= BCM_FLOW_ENCAP_VLAN_VALID;
    printf("Doing flow encap add...\n");
    BCM_IF_ERROR_RETURN( bcm_flow_encap_add(unit, &encap_config, 0, &logical_fields) );

    return BCM_E_NONE;
}

bcm_error_t
    configure_nexthop(int unit, int vrf, int dest_port, bcm_vlan_t vid,
                        bcm_mac_t *intf_mac, bcm_mac_t *nhop_mac, bcm_if_t *nh_index, int flags)
{
    bcm_l3_intf_t intf;
    bcm_l3_egress_t   egress_object;
    int status;

    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, intf_mac, 6);
    intf.l3a_vid =  vid;
    intf.l3a_mtu = 4096;
    intf.l3a_flags = BCM_L3_ADD_TO_ARL;
    intf.l3a_vrf = vrf;
    printf("Doing L3 intf create...\n");
    BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &intf) );
 
    bcm_l3_egress_t_init(&egress_object);
    sal_memcpy(egress_object.mac_addr, nhop_mac, 6);
    egress_object.intf = intf.l3a_intf_id; 
    egress_object.port = dest_port;
    egress_object.flags = flags;
    /* go ahead and create the egress object */ 
    printf("Doing L3 egress create...\n");
    BCM_IF_ERROR_RETURN( bcm_l3_egress_create(unit, 0, &egress_object, nh_index) );
    
    return BCM_E_NONE;
}

bcm_error_t
    configure_v4_route(int unit,int vrf,  uint32 ip, uint32 subnet_mask, bcm_if_t nh_index) {

    bcm_l3_route_t defroute;
    int status;
    int i = 0;

    bcm_l3_route_t_init(&defroute);
    defroute.l3a_subnet = ip & subnet_mask;
    defroute.l3a_ip_mask = subnet_mask;
    defroute.l3a_intf = nh_index;
    defroute.l3a_vrf = vrf;
    
    printf("Doing L3 route add...\n");
    BCM_IF_ERROR_RETURN( bcm_l3_route_add(unit, &defroute) );
    
    return BCM_E_NONE;
}

bcm_error_t
    configure_v4_defroute(int unit, int vrf, uint32 ip, uint32 subnet_mask, bcm_if_t ecmp_intf) {

    bcm_l3_route_t defroute;
    int status;
    int i = 0;

    bcm_l3_route_t_init(&defroute);
    defroute.l3a_subnet = ip & subnet_mask;
    defroute.l3a_ip_mask = subnet_mask;
    defroute.l3a_intf = ecmp_intf;
    defroute.l3a_vrf = vrf;
    defroute.l3a_flags = BCM_L3_MULTIPATH;
    printf("Doing L3 route add...\n");
    BCM_IF_ERROR_RETURN( bcm_l3_route_add(unit, &defroute) );
    
    return BCM_E_NONE;
}

bcm_error_t TestFunc()
{
    int vrf = 10;
    bcm_gport_t subport_gport = 0;
    bcm_if_t    nh_index[2];

    BCM_IF_ERROR_RETURN( bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1) );
    
    BCM_IF_ERROR_RETURN( create_vlan_and_add_ports(unit, test_vid, ing_port1, 0) );
    BCM_IF_ERROR_RETURN( create_vlan_and_add_ports(unit, test_vid65, coe_port1, 0) );
    
    if( 1 ) {
        BCM_IF_ERROR_RETURN( configure_nexthop(unit, vrf, ing_port1, test_vid, 
                              test_mac_dd/*intf_mac*/, test_mac_bb/*nhop_mac*/, &nh_index[0], 0) );
        BCM_IF_ERROR_RETURN( configure_v4_route(unit, vrf,  test_ip3, 0xfffffff0, nh_index[0]) );
        printf("nexthop created with index0  =  %d\n", nh_index[0]);
    }
    
    BCM_IF_ERROR_RETURN( create_subport(test_vid65, coe_port1, &subport_gport) );
    
    // for egress encap ; from fb6 to FL
    BCM_IF_ERROR_RETURN( configure_flex_macsec_encoding(test_vid65, subport_gport) );
                                    
    /* config EFP */
    BCM_IF_ERROR_RETURN( config_efp(unit) );
    
    if( 1 ) {
        //printf("Create the egress object with the subport. \n");
        BCM_IF_ERROR_RETURN( configure_nexthop(unit, vrf, subport_gport, test_vid65, 
                              test_mac_ee/*intf_mac*/, test_mac_aa/*nhop_mac*/, &nh_index[1], 0) );
        BCM_IF_ERROR_RETURN( configure_v4_route(unit, vrf,  test_dip, 0xfffffff0, nh_index[1]) );
        printf("nexthop created with index1  =  %d\n", nh_index[1]);
    }
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
    
    if( 0 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
        BCM_PBMP_PORT_ADD(vlan_pbmp, non_coe_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, coe_port1);     // MTP port need not be part of the VLAN

        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    if( 0 )
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, test_mac_dd, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        printf("Doing L2 entry add\n");
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( 1 )
    {
        rv = ingress_port_setup(unit, non_coe_port1, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", non_coe_port1, rv);
            return rv;
        }

        rv = egress_port_setup(unit, coe_port1, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", coe_port1, rv);
            return rv;
        }
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, non_coe_port1, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, non_coe_port1, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, coe_port1, loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, coe_port1, BCM_PORT_DISCARD_ALL));
    }
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, loopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, loopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_NONE));

    //bshell(unit, "pw start report +raw +decode dma");
    bshell(unit, "pw start");

    return BCM_E_NONE;
}

void PrintStat(int unit)
{
    char cmd[32];
    int i;
    for(i=1; i<=statid_used; ++i)
    {
        if( stat_desc[i] )  printf("%s:\n", stat_desc[i]);
        sprintf(cmd, "fp stat get si=%d t=p", i);
        bbshell(unit, cmd);
        sprintf(cmd, "fp stat set si=%d t=p val=0", i);
        bshell(unit, cmd);
    }
    return;
}

bcm_error_t verify()
{
    char cmd[2*1024];
    int prio = 0;
    int count = 0;
    
    if( 1 )
    {
        printf("##########   Sending L3 packet %d ########\n", count++);
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500LLLL000000004011SSSS%08X%08X%04X%04X0036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2",
            non_coe_port1,
            test_mac_dd[0], test_mac_dd[1], test_mac_dd[2],
            test_mac_dd[3], test_mac_dd[4], test_mac_dd[5],
            test_mac_cc[0], test_mac_cc[1], test_mac_cc[2],
            test_mac_cc[3], test_mac_cc[4], test_mac_cc[5],
            FORM_VID(prio, 0, test_vid), test_sip, test_dip,
            test_L4_src_port,
            test_L4_dst_port );
        //printf("BEFORE: %s\n", cmd);
        /* For single tag IP packet; 18th byte starts at ->4500... Byte in hex; not ascii. */
        ip4_sum_n_size_update(cmd, 18, 0);
        //ip4_sum_n_size_update(cmd, 38, 0); // for inner ipv4 in Ip-in-IP packet with single tag
        //printf("AFTER:  %s\n", cmd);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep quiet %d", 1);
        bbshell(unit, cmd);
        PrintStat(unit);
        printf("#####################################################\n\n");
    }
    
    if( 0 )
    {
        printf("##########   Sending L3 packet %d ########\n", count++);
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500LLLL000000004011SSSS%08X%08X%04X%04X0036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D29197737CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC2222222222222222222222",
            non_coe_port1,
            test_mac_dd[0], test_mac_dd[1], test_mac_dd[2],
            test_mac_dd[3], test_mac_dd[4], test_mac_dd[5],
            test_mac_cc[0], test_mac_cc[1], test_mac_cc[2],
            test_mac_cc[3], test_mac_cc[4], test_mac_cc[5],
            FORM_VID(prio, 0, test_vid65), test_sip, test_dip,
            test_L4_src_port,
            test_L4_dst_port );
        //printf("BEFORE: %s\n", cmd);
        /* For single tag IP packet; 18th byte starts at ->4500... Byte in hex; not ascii. */
        ip4_sum_n_size_update(cmd, 18, 0);
        //ip4_sum_n_size_update(cmd, 38, 0); // for inner ipv4 in Ip-in-IP packet with single tag
        //printf("AFTER:  %s\n", cmd);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep quiet %d", 1);
        bbshell(unit, cmd);
        PrintStat(unit);
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


const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}
