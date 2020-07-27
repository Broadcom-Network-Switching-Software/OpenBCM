/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *  Feature  : Encrytion and Decryption of SVTAGged packet using XFLOW Macsec in Firelight
 *
 *  Usage : BCM.0> cint <thisfile.c> YES
 *           E.g.  cint firelight_xflow_macsec.c YES
 *
 *  config   : BCM56070_A0-op02_5-macsec.config
 *
 *              Feature specific Config Variables:
 *                  macsec_port_index_2=0
 *                  macsec_port_index_3=1
 *                  macsec_port_index_4=2
 *                  macsec_port_index_5=3
 *
 *  Log file : firelight_xflow_macsec_log.txt
 *
 *
 *  Test Topology :
 *
 *                   +------------------------------+  Front Panel ge0
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              |
 *     FB6    xe0    |                              |  Macsec port  ge1
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *     FB6    xe1    |                              |
 *  +----------------+                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *      uplink_port1 = 50 (0x32)       - xe0
 *      dwnlink_port_macsec1 = 3 (0x3) - ge1
 *
 *
 *  Summary:
 *  ========
 *      This cint demostrates the xflowmacsec feature in Firelight. Ideally the real test scenario
 *      for this feature is to connect Firebolt6 (FB6) to Firelight on a set of CoE ports. But
 *      given the limitations of the environment in which this CINT is supposed to run is that it
 *      can only run on a standalone Firelight device. To facilitate this some of the parameters
 *      are imported from G+FB6 CoE scripts as is.
            1. fl_subport_num2
            2. svtag_sig
            3. The SVTAgged packet (derived using variable in sprintf())
 *      A normal SVTAGGED packet is sent from CPU to a loopbacked packet (xe0) which is a CoE port.
 *      xe0 is connected to FB6 as backplane port. Packet is captured by IFP for encryption. Packet
 *      is sent to a macsec port ge1. Before egressing, packet is encrypted by the macsec engine.
 *      This encrypted packet ingresses doe to ge1 in loopback. This encrypted packet is then
 *      Decrypted. But, for the sake of demonstration, ge1 is not configured for decryption until 1
 *      packet is sent and displayed with its encrypted content.
 *         Once the packet is decrypted, its content is same as the unencrypted packet that was
 *      sent originally from xe0. Not every port can be made a MACSEC port. Chip madates certain
 *      combination. Use the supplied config file to configure the device with legal assignments of
 *      ports.
 
 
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Putting all the test ports in VLAN 65 and ports in loopback
 *       b) Ingress Mirror to CPU is enabled for all the egress ports
 *       c) Started packet watcher on CPU
 *
 *    2) Step2 - Configuration (Done in TestFunc()).
 *    ===================================================
 *       a) Setup downlink configuration where xe0 will be uplink port and ge0 as a NON macsec port.
 *       b) Setup downlink configuration where xe0 will be uplink port and ge1 as a MACSEC port.
 *       c) All the packets ingressing from xe0 and NOT TO BE encrypted i.e. theit CPH header
 *          matches cph_noenc_match_hdr_data, will egress from ge0.
 *       d) All the packets ingressing from xe0 and NOT TO BE encrypted i.e. theit CPH header
 *          matches cph_enc_match_hdr_data, will egress from ge1.
 *       e) Svtag signature is set same as it was configured in FB6.
 *       f) Encryption related configurations are done.
 *       g) Decryption settings will be done once , encryption related demonstration is done in
 *          verify().
 *
 *    2) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send 2 packets as captures from FB6 CoE test, meant for encryption.
 *       b) The configure decryption on port ge1.
 *       c) Send 2 packets.
 *       b) Expected Result:
 *          ================
 *          1st 2 packets will be ONLY encrypted as captured on CPU 'pw'. Subsequent 2 packets will
 *          be encrypted and then decrypted. Original packet is received by CPU 'pw'.
 *
 * NOTE: Use opt_* variables to change the test variant
 *
 * INTERNAL:
 *
 *  HOW TO CLONE XFLOW FROM A PARTICULAR LABEL
 *     git clone ssh://<username>@gerrit-sj1-2.sj.broadcom.com:29418/xflow-macsec
 *     cd xflow-macsec
 *     git tag -l | grep 6_5_19
 *     cd ../
 *     rm -rf xflow-macsec
 *     git clone -b xflow_macsec_1_0_0__sdk_6_5_19__E2020_03_17_10_30_00 ssh://<username>@gerrit-sj1-2.sj.broadcom.com:29418/xflow-macsec
 *
 *  BUILD STEPS for Firelight:
 *     1.  git clone ssh://<username>@gerrit-sj1-2.sj.broadcom.com:29418/xflow-macsec
 *     2.  It will be cloned in folder xflow-macsec
 *     3.  In $SDK/make/Make.local, XFLOW_MACSEC to FEATURE_LIST.
 *     4.  Make sure the older MACSEC feature is not present in this list.
 *     5.  Set XFLOW_MACSEC_HOME to the xflow-macsec local workspace.
 *     6.  Make SDK as normally you do.
 *
 *    [SJ RACK 70 FIRELIGHT / SLK]
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
int opt_TestOption1 = 0;

print ARGC;

/* WithPresel? With:1 Without:0 */
/*
if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_TestOption1 = 1;
    } else {
        opt_TestOption1 = 0;
    }
}
*/
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
int unit = 0;

bcm_vlan_t  test_vid64 = 0x64;
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

/*
From BCM56070_A0-op02_5-macsec.config

Port # 2 to 49 are macsec mapped ports (can be up or down links)
Port # 50 onwards, ports are normal ports (can be up or down links)

*/
bcm_port_t ing_port1 = 50; // Set to a valid port if known already.
bcm_port_t ing_port2 = 4;

bcm_port_t egr_port1 = 2;
bcm_port_t egr_port2 = 3;
bcm_port_t egr_port3 = 5;

/* Populate the XE/CE/CD ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

egr_port1 = portGetNextE(unit, egr_port1);
egr_port2 = portGetNextE(unit, egr_port2);
egr_port3 = portGetNextE(unit, egr_port3);


int uplink_port1 = ing_port1;   /* Connected to FB6 */

// For egressing non-encrypted packets
int dwnlink_port_normal1 = egr_port1;  /* Access/Front port of Firelight */
// For egressing encrypted packets
int dwnlink_port_macsec1 = egr_port2;  /* Access/Front port of Firelight.
                                          Encrypted packet egresses from here */
                                            
int dwnlink_port_macsec2 = egr_port3;  /* Access/Front port of Firelight.
                                 Captured Encrypted packet will be injected by
                                 CPU from here for testing  */

void printVars() {
    print uplink_port1;
    print dwnlink_port_normal1;
    print dwnlink_port_macsec1;
    print dwnlink_port_macsec2;
}

printVars();

////////////////////////////////////////////////////////////////////

bcm_xflow_macsec_instance_id_t          instance_id = 0;

// In-Sync with FB6 scripts  ----   START
int fl_subport_num1;
int fl_subport_num2;    // In-Sync with FB6 scripts

uint32 svtag_sig;    // In-Sync with FB6 scripts

// 1300040B

// In-Sync with FB6 scripts  ----   END

uint32 pkt_type_do_encrypt;
uint32 pkt_type_dont_encrypt;


uint32 cph_enc_match_hdr_data;
uint32 cph_noenc_match_hdr_data;
uint32 cph_match_mask;

int assoc_num;
int include_sci;

void SetAndPrint()
{
    if( 1 ) {
        fl_subport_num1 = 0x0A;
        fl_subport_num2 = 0x0B;    // In-Sync with FB6 scripts

        svtag_sig = 0x13;          // In-Sync with FB6 scripts

        pkt_type_do_encrypt   = 0x04;
        pkt_type_dont_encrypt = 0x00;

        include_sci = TRUE;
        
        assoc_num = 1;
    }
    else if( 0 ) {
        fl_subport_num1 = 0x00;
        fl_subport_num2 = 0x00;

        svtag_sig = 0x13;

        pkt_type_do_encrypt   = 0x04;
        pkt_type_dont_encrypt = 0x00;

        include_sci = FALSE;
        assoc_num = 0;
    }
    else {
        fl_subport_num1 = 0x00;
        fl_subport_num2 = 0x00;

        svtag_sig = 0xFF;
        
        pkt_type_do_encrypt   = 0x04;
        pkt_type_dont_encrypt = 0x00;

        include_sci = FALSE;
        assoc_num = 0;
    }

    if( 1 ) {
        cph_enc_match_hdr_data   = (svtag_sig<<24) | (pkt_type_do_encrypt<<8);
        cph_noenc_match_hdr_data = (svtag_sig<<24) | (pkt_type_dont_encrypt<<8);
        cph_match_mask = (0xFF<<24) | (0xFF<<8);
    }
    else {
        cph_enc_match_hdr_data   = 0xff000000;
        cph_noenc_match_hdr_data = 0xff000002;
        cph_match_mask = 0xffff00ff;
    }

    print(fl_subport_num1);
    print(fl_subport_num2);
    print(pkt_type_do_encrypt);
    print(pkt_type_dont_encrypt);
    print(svtag_sig);
    print(include_sci);
    print(assoc_num);
    
    print(cph_enc_match_hdr_data);
    print(cph_noenc_match_hdr_data);
    print(cph_match_mask);
}

SetAndPrint();

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
int ifp_qid_1_init=0;

/****************************************************/
/*
 * Function:
 *      disable_l2_learn_on_port
 * Purpose:
 *      Turn off the L2 learning on specific logical port(port_tab.CML_FLAGS_NEW=0)
 * Parameters:
 *      port - specific logical port to turn off the L2 learning
 * Returns:
 */
bcm_error_t
    disable_l2_learn_on_port(int port)
{
    BCM_IF_ERROR_RETURN( bcm_port_learn_set(unit, port, BCM_PORT_LEARN_FWD) );
    return BCM_E_NONE;
}

/* Enable CPH parser on cph_port 
 * port_tab.port.CUSTOM_HEADER_ENABLE =1
 */
bcm_error_t
    enable_cph_parser_on_port(int cph_port)
{
    bcm_switch_match_control_info_t match_control;
    
    bcm_switch_match_control_info_t_init(&match_control);
    match_control.port_enable = TRUE;
    BCM_IF_ERROR_RETURN( bcm_switch_match_control_set(unit, bcmSwitchMatchServiceCustomHeader, 
            bcmSwitchMatchControlPortEnable, cph_port, &match_control) );
    
    return BCM_E_NONE;
}

/* Set CPH match mask 
 * CUSTOM_HEADER_MASK = mask
 */
bcm_error_t
    set_cph_match_mask(uint32 mask)
{
    bcm_switch_match_control_info_t match_control;
    
    bcm_switch_match_control_info_t_init(&match_control);
    match_control.mask32 = mask;
    BCM_IF_ERROR_RETURN( bcm_switch_match_control_set(unit, bcmSwitchMatchServiceCustomHeader, 
            bcmSwitchMatchControlMatchMask, -1, &match_control) );
    
    return BCM_E_NONE;
}

/* Trust CPH priority on cph_port 
 * port_tab.port.USE_CUSTOM_HEADER_FOR_PRI = 1
 */
bcm_error_t enable_trust_cph_priority_on_port(int cph_port)
{
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, cph_port, 
        bcmPortControlTrustCustomHeaderPri, 1) );
    
    return BCM_E_NONE;
}

/* Add CPH match entry 
 * CUSTOM_HEADER_MATCH
 * CUSTOM_HEADER_POLICY_TABLE
 */
bcm_error_t
    add_cph_match_entry(uint32 data, int pri, int* match_id)
{
    bcm_switch_match_config_info_t match_config;
    
    bcm_switch_match_config_info_t_init(&match_config);
    match_config.value32 = data;
    match_config.color = bcmColorGreen;
    match_config.int_pri = pri;
    BCM_IF_ERROR_RETURN( bcm_switch_match_config_add(unit, bcmSwitchMatchServiceCustomHeader, 
            &match_config, match_id) );
    
    return BCM_E_NONE;
}

/* disable Encap on port 
 *  EGR_PORT_64.port.CUSTOM_HEADER_ENABLE=0
 */
bcm_error_t
    disable_cph_encap_port(int port)
{
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port,
        bcmPortControlCustomHeaderEncapEnable, FALSE) );
    
    return BCM_E_NONE;
}

/* Enable Encap on port 
 * EGR_PORT_64.port.CUSTOM_HEADER_ENABLE=1
 */
bcm_error_t
    enable_cph_encap_port(int port)
{
    BCM_IF_ERROR_RETURN( bcm_port_control_set(unit, port,
        bcmPortControlCustomHeaderEncapEnable, TRUE) );
    
    return BCM_E_NONE;
}

/* Add FP rule for checking the packet type and redirect */
bcm_error_t
    ifp_qualify_custom_header_redirect(
                                        int gid, 
                                        int fp_id, 
                                        uint32 header_0, 
                                        uint32 cph_mask, 
                                        int redirect_port)
{
    int pri =0;
    bcm_field_qset_t qset;
    bcm_field_entry_t eid;
    printf("ifp_qualify_custom_header_redirect: ifp_qid_1_init=%d cph_mask=0x%08x\n",
        ifp_qid_1_init, cph_mask);
    
    eid = fp_id; /* entry id */
    bcm_field_qset_t_init(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyCustomHeaderPkt);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyCustomHeaderData);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIntPriority);
    if(!ifp_qid_1_init) {
        BCM_IF_ERROR_RETURN( bcm_field_group_create_id(unit, qset, pri, gid) );
        
        ifp_qid_1_init =1;
    }
    
    BCM_IF_ERROR_RETURN( bcm_field_entry_create_id(unit, gid, eid) );
    /* qualify if custom packet present */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_CustomHeaderPkt(unit, eid, 1, 1) );
    /* qualify if custom packet header match */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_CustomHeaderData(unit, eid,
                            header_0, cph_mask) );

    bcm_pbmp_t redirect_pbmp;
    BCM_PBMP_PORT_SET(redirect_pbmp, redirect_port);
    BCM_IF_ERROR_RETURN( bcm_field_action_ports_add(unit, eid,
                            bcmFieldActionRedirectPbmp, redirect_pbmp) );
  
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, eid) );
    
    return BCM_E_NONE;
}


bcm_error_t
    line_card_downstream_fp_default_no_cph_hit_drop(int gid, int fp_id,
                        bcm_port_t dwnlink_port)
{
    int rv =0;
    int pri =0;
    bcm_field_qset_t qset;
    bcm_field_entry_t eid;
    printf("..no_cph_hit_drop: ifp_qid_1_init=%d\n", ifp_qid_1_init);
    
    eid = fp_id; /* entry id */
    bcm_field_qset_t_init(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyCustomHeaderPkt);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyCustomHeaderData);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIntPriority);
    if(!ifp_qid_1_init) {
        BCM_IF_ERROR_RETURN( bcm_field_group_create_id(unit, qset, pri, gid) );
        
        ifp_qid_1_init =1;
    }
    
    BCM_IF_ERROR_RETURN( bcm_field_entry_create_id(unit, gid, eid) );
    /* qualify if custom packet NOT present */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_CustomHeaderPkt(unit, eid, 0, 1) );
    /* qualify if source port match */
    BCM_IF_ERROR_RETURN( bcm_field_qualify_SrcPort(unit, eid,
             0 /*Modid*/, 0xFF /*Modid Mask*/, dwnlink_port, 0xFFFF /*mask*/) );
    BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, eid, bcmFieldActionDrop,
                                           0 /* priority */, 0 /* unused */) );
    //bcm_field_action_add(unit, eid, bcmFieldActionCopyToCpu, 0, 0);
  
    BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, eid) );
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      line_card_downstream_fb6_to_fl_non_macsec
 * Parameters:
 *      uplink_port - uplink port connected to the FB6.
 *      downlink_port - redirect to the FL's downlink port(front/access port).
 *      cph_header - custom header
 *      cph_match_mask - custom header match mask to check if present
 *      cph_mask - custom header mask used for FP qualify
 *      pri     - trust priority; INT_PRI will be limited to a single values
 *                  of 0 for all traffic.
 *      fp_gid - fp group ID
 *      fp_eid - fp entry ID
 */
bcm_error_t
    line_card_downstream_fb6_to_fl_non_macsec(
        int uplink_port, int downlink_port,
        uint32 cph_header, uint32 cph_match_mask, uint32 cph_mask, int pri,
        int fp_gid, int fp_eid)
{
    int match_id;
    
    /* Ingress port CPU Managed Learning (CML) is disabled */
    BCM_IF_ERROR_RETURN( disable_l2_learn_on_port(uplink_port) );
    
    BCM_IF_ERROR_RETURN( enable_cph_parser_on_port(uplink_port) );
    BCM_IF_ERROR_RETURN( set_cph_match_mask(cph_match_mask) );
    BCM_IF_ERROR_RETURN( enable_trust_cph_priority_on_port(uplink_port) );
    
    BCM_IF_ERROR_RETURN( add_cph_match_entry(cph_header, pri, &match_id) );
    
    /* decap the Custom Header to downlink port */
    BCM_IF_ERROR_RETURN( disable_cph_encap_port(downlink_port) );
    
    BCM_IF_ERROR_RETURN( ifp_qualify_custom_header_redirect(fp_gid, fp_eid,
                            cph_header, cph_mask, downlink_port) );
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      line_card_downstream_fb6_to_fl_with_macsec
 * Parameters:
 *      uplink_port - uplink port connected to the FB6.
 *      downlink_port - redirect to the FL's downlink port(front/access port).
 *      cph_header - custom header
 *      cph_match_mask - custom header match mask to check if present
 *      cph_mask - custom header mask used for FP qualify
 *      pri     - trust priority; INT_PRI will be limited to a single values of
 *                  0 for all traffic.
 *      fp_gid - fp group ID
 *      fp_eid - fp entry ID
 */
bcm_error_t
    line_card_downstream_fb6_to_fl_with_macsec(
        int uplink_port, int downlink_port,
        uint32 cph_header, uint32 cph_match_mask, uint32 cph_mask, int pri,
        int fp_gid, int fp_eid)
{
    int match_id;

    /* Ingress port CPU Managed Learning (CML) is disabled */
    BCM_IF_ERROR_RETURN( disable_l2_learn_on_port(uplink_port) );
    
    BCM_IF_ERROR_RETURN( enable_cph_parser_on_port(uplink_port) );
    BCM_IF_ERROR_RETURN( set_cph_match_mask(cph_match_mask) );
    BCM_IF_ERROR_RETURN( enable_trust_cph_priority_on_port(uplink_port) );
    
    BCM_IF_ERROR_RETURN( add_cph_match_entry(cph_header, pri, &match_id) );
    
    /* retain the Custom Header */
    BCM_IF_ERROR_RETURN( enable_cph_encap_port(downlink_port) );
   
    BCM_IF_ERROR_RETURN( ifp_qualify_custom_header_redirect(fp_gid, fp_eid,
                            cph_header, cph_mask, downlink_port) );
    
    return BCM_E_NONE;
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

bcm_error_t
    macsec_app_init(uint64 *sci, uint64 *sci_mask,
                    bcm_xflow_macsec_crypto_aes128_gcm_t *crypto_key)
{
    uint64 val64;
    
    /* Set the crypto key to be used for encryption. */
    crypto_key->key[0]  = 0xef;  crypto_key->key[1] = 0xcd;
    crypto_key->key[2]  = 0xab; crypto_key->key[3]  = 0x89;
    crypto_key->key[4]  = 0x67; crypto_key->key[5]  = 0x45;
    crypto_key->key[6]  = 0x23;  crypto_key->key[7] = 0x01;
    crypto_key->key[8]  = 0xec; crypto_key->key[9]  = 0xff;
    crypto_key->key[10] = 0x00; crypto_key->key[11] = 0x00;
    crypto_key->key[12] = 0x00; crypto_key->key[13] = 0x00;
    crypto_key->key[14] = 0x00; crypto_key->key[15] = 0x00;

    /* Set the Secure Channel ID. */
    if( 0 ) {
        COMPILER_64_SET(*sci, 0x0, fl_subport_num2);
    }
    else {
        COMPILER_64_SET(*sci, 0x0, 0x7);
    }
    print *sci;
    
    COMPILER_64_SET(*sci_mask, 0xFFFFFFFFul, 0xFFFFFFFFul);
    
    if( 1 ) {
        COMPILER_64_SET(val64, 0x0, svtag_sig);
                                // Same as bcmFieldActionSubportSvtagSignature
                                // from FB6 'svtag_sig'
        printf("Doing SVTagTPIDEtype set...\n");
        BCM_IF_ERROR_RETURN( bcm_xflow_macsec_control_set(unit,
            BCM_XFLOW_MACSEC_ENCRYPT, instance_id,
            bcmXflowMacsecControlSVTagTPIDEtype, val64) );
    }
    if( 1 ) {
        COMPILER_64_SET(val64, 0x0, svtag_sig);
                                // Same as bcmFieldActionSubportSvtagSignature
                                // from FB6 'svtag_sig'
        printf("Doing SVTagTPIDEtype set...\n");
        BCM_IF_ERROR_RETURN( bcm_xflow_macsec_control_set(unit,
            BCM_XFLOW_MACSEC_DECRYPT, instance_id,
            bcmXflowMacsecControlSVTagTPIDEtype, val64) );
    }
    
    if( 0 ) {
        COMPILER_64_SET(val64, 0x0, 1);
        printf("Doing SVTagEnable enable...\n");
        BCM_IF_ERROR_RETURN( bcm_xflow_macsec_control_set(unit,
            BCM_XFLOW_MACSEC_ENCRYPT, instance_id,
            bcmXflowMacsecControlSVTagEnable, val64) );
    }
    
    if( 0 ) {
        bcm_xflow_macsec_vlan_tpid_t tpid;
        
        tpid.enable[0]=1;
        tpid.enable[1]=0;
        tpid.enable[2]=0;
        tpid.enable[3]=0;
        
        tpid.tpid[0]=svtag_sig;
        tpid.tpid[1]=0;
        tpid.tpid[2]=0;
        tpid.tpid[3]=0;
        printf("Doing tpid array set...\n");
        BCM_IF_ERROR_RETURN( bcm_xflow_macsec_vlan_tpid_array_set(unit,
                                                        instance_id, &tpid) );
    }
    
    return BCM_E_NONE;
}

bcm_error_t
    create_sc_encrypt(uint64 *sci, uint64 *sci_mask,
                            bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    bcm_xflow_macsec_secure_chan_info_t     sc1;
    uint32 flag = 0;
    
    /* Initialize Security Channel structure */
    bcm_xflow_macsec_secure_chan_info_t_init(&sc1);
    /* Set the SCI value and mask. */
    sc1.sci = *sci;
    sc1.sci_mask = *sci_mask;
    
    /* Select the crypto algorithm. */
    sc1.crypto = bcmXflowMacsecCryptoAes128Gcm;
    
    /* Insert SecTAG after (DA,SA, VID) */
    sc1.sectag_offset = 16; // 6 + 6 + 4
    sc1.mtu = 1514;
    sc1.active_an = assoc_num;
    sc1.tci = ((include_sci?0x1:0x0)<<5) | (0x1<<3) | (sc1.active_an & 0x3); /* SC=X, E=X AN=X */
        /*
Encrypted Packet Header Decoding:
SCI=7   SC=1   E=1   AN=1   next_pkt_num=1
{0000000000aa}->(DMAC) {0000000000ee}->(SMAC) {8100 0001}->(VLANID)
{ff01 1001}->(SVTAG?) {8100 0065}->(VLANID)) {{88e5}->(MACSEC ETYPE) {29}->(TCI|AN) {00}->(SL) {0000 0001}->(PN)
{0000 0000 0000 0007}->(SCI)}->(SECTAG)

            SC: Explicit SCI encoding
            E : Encryption

            SC=0 E=1 AN=0
            {0000000000aa} {0000000000ee} 8100 0001
            ff01 1001 8100 0065 88e5 0800 0000 0001

            SC=0 E=1 AN=1
            {0000000000aa} {0000000000ee} 8100 0001
            ff01 1001 8100 0065 88e5 0900 0000 0001

            SC=1 E=1 AN=0
            {0000000000aa} {0000000000ee} 8100 0001
            ff01 1001 8100 0065 88e5 2800 0000 0001
            0000 0000 0000 0007

            SC=1 E=1 AN=1
            {0000000000aa} {0000000000ee} 8100 0001
            ff01 1001 8100 0065 88e5 2900 0000 0001
            0000 0000 0000 0007

        */
    
    printf("sc1.tci = 0x%02X\n", sc1.tci);
    sc1.flags = BCM_XFLOW_MACSEC_SECURE_CHAN_ENCRYPT_CONTROLLED_SECURED_DATA;
    if( sc1.tci & (0x1<<5) ) {
        sc1.flags |= BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_INCLUDE_SCI;
    }
    sc1.flags |= BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_CONTROLLED_PORT;
    
    if( 1 ) {
        *chan_id = BCM_XFLOW_MACSEC_SECURE_CHAN_ID_CREATE(
                     BCM_XFLOW_MACSEC_ENCRYPT, fl_subport_num2 /*channel id*/);
        flag = BCM_XFLOW_MACSEC_SECURE_CHAN_WITH_ID;
    }
    
    /* Create a new Security Channel for an encrypt flow */
    printf("++++ Create a new Security Channel for an encrypt flow *chan_id=0x%X\n",
                                                                *chan_id);
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_secure_chan_create(unit,
            BCM_XFLOW_MACSEC_ENCRYPT | flag,
            instance_id, &sc1, 0/*prio*/, chan_id) );
    printf("++++ Created *chan_id=0x%X\n", *chan_id);
    
    return BCM_E_NONE;
}

bcm_error_t
    create_sa(bcm_xflow_macsec_secure_chan_id_t chan_id,
                bcm_xflow_macsec_crypto_aes128_gcm_t *crypto_key,
                 bcm_xflow_macsec_secure_assoc_id_t *assoc_id)
{
    bcm_xflow_macsec_secure_assoc_info_t    sa1;
    
    /* Initialize Security Association structure */
    bcm_xflow_macsec_secure_assoc_info_t_init(&sa1);
    sa1.an = assoc_num;
    sa1.aes = *crypto_key;
    sa1.next_pkt_num = 1;
    sa1.enable = 1;
    sa1.flags = BCM_XFLOW_MACSEC_SECURE_ASSOC_INFO_SET_NEXT_PKT_NUM;
    
    /* Create a security association for the given chan_id.*/
    printf("++++ Create a security association for the given chan_id.\n");
    BCM_IF_ERROR_RETURN(bcm_xflow_macsec_secure_assoc_create (unit, 0,
                                                    chan_id, &sa1, assoc_id) );
    
    return BCM_E_NONE;
}

bcm_error_t
    get_stats_encrypt(bcm_xflow_macsec_secure_chan_id_t chan_id,
                bcm_xflow_macsec_stat_type_t stat_type, uint64 *val)
{
    bcm_xflow_macsec_subport_id_t subport_id;
    
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_subport_id_get(unit, chan_id,
                                                                &subport_id) );
    print subport_id;

    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_stat_get(unit,
                        BCM_XFLOW_MACSEC_ENCRYPT, subport_id, stat_type, val) );
    
    return BCM_E_NONE;
}


bcm_error_t
  create_sc_decrypt(uint64 *sci, uint64 *sci_mask,
                        bcm_xflow_macsec_secure_chan_id_t *chan_id)
{
    uint32 flag = 0;
    bcm_xflow_macsec_secure_chan_info_t  sc1;
    
    /* Initialize Security Channel structure */
    bcm_xflow_macsec_secure_chan_info_t_init(&sc1);
    /* Set the SCI value and mask. */
    sc1.sci = *sci;
    sc1.sci_mask = *sci_mask;
    /* Select the crypto algorithm. */
    sc1.crypto = bcmXflowMacsecCryptoAes128Gcm;
    sc1.replay_protect_window_size = 0;
    sc1.an_control = bcmXflowMacsecSecureAssocAnRollover;
    // Allow both, data + management pkts
    sc1.flags = BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_CONTROLLED_PORT;
    if( include_sci ) {
        sc1.flags |= BCM_XFLOW_MACSEC_SECURE_CHAN_INFO_INCLUDE_SCI;
    }
    
    if( 1 ) {
        *chan_id = BCM_XFLOW_MACSEC_SECURE_CHAN_ID_CREATE(
                      BCM_XFLOW_MACSEC_DECRYPT, fl_subport_num2 /*channel id*/);
        flag = BCM_XFLOW_MACSEC_SECURE_CHAN_WITH_ID;
    }
    /* Create a new Security Channel for an encrypt flow */
    printf("++++ Create a new Security Channel for an decrypt flow *chan_id=0x%X\n",
                                                                    *chan_id);
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_secure_chan_create(unit,
                            BCM_XFLOW_MACSEC_DECRYPT | flag,
                            instance_id, &sc1, 0/*prio*/, chan_id) );
    printf("++++ Created decrypt *chan_id=0x%X\n", *chan_id);
    
    /* Enable secure channel */
    printf("++++ Enable secure channel\n");
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_secure_chan_enable_set(unit, *chan_id, 1) );
    
    return BCM_E_NONE;
}

bcm_error_t
    create_policy(uint64 *sci, bcm_xflow_macsec_secure_chan_id_t chan_id,
                        bcm_xflow_macsec_policy_id_t *policy_id)
{
    bcm_xflow_macsec_decrypt_policy_info_t  policy_info1;
    bcm_xflow_macsec_secure_chan_info_t     sc_get;
    int priority;

    /* Initialize decrypt policy */
    bcm_xflow_macsec_decrypt_policy_info_t_init(&policy_info1);
    
    policy_info1.flags  = BCM_XFLOW_MACSEC_DECRYPT_POLICY_TAGGED_CONTROL_PORT_ENABLE;
    policy_info1.flags |= BCM_XFLOW_MACSEC_DECRYPT_POLICY_UNTAGGED_CONTROL_PORT_ENABLE;
    policy_info1.flags |= BCM_XFLOW_MACSEC_DECRYPT_POLICY_POINT_TO_POINT_ENABLE;
    
    policy_info1.sci = *sci;
    policy_info1.tag_validate = bcmXflowMacsecTagValidateStrict;
    policy_info1.sectag_offset = 16;    // 12 ?????
    policy_info1.mtu = 1514;

    /* Create a decryption policy.*/
    printf("++++ Create the policy \n");
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_decrypt_policy_create(unit, 0, 0,
                                                &policy_info1, policy_id) );

    printf("++++ Get the SC\n");
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_secure_chan_get(unit, chan_id,
                                                        &sc_get, &priority) );
    
    sc_get.policy_id = *policy_id;
    printf("++++ Set the SC with the policy\n");
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_secure_chan_set(unit, 0,
                                                  chan_id, sc_get, priority) );

    return BCM_E_NONE;
}

bcm_error_t
    create_flows(bcm_port_t  dwnlink_port, bcm_xflow_macsec_policy_id_t policy_id,
                        bcm_xflow_macsec_flow_id_t *flow_id)
{
    bcm_xflow_macsec_decrypt_flow_info_t flow_info1, flow_info_get;
    bcm_pbmp_t pbm;

    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, dwnlink_port);

    bcm_xflow_macsec_decrypt_flow_info_t_init(&flow_info1);
    flow_info1.policy_id = policy_id;
    flow_info1.vlan_tag_mpls_label_flags = BCM_XFLOW_MACSEC_1_VLAN_TAG_1_MPLS_LABEL;
    flow_info1.frame_type = bcmXflowMacsecFlowFrameEII;
    flow_info1.src_pbm = pbm;

    printf("++++ Create a flow\n");
    /* ISEC_SP_TCAM_KEY
       ISEC_SP_TCAM_MASK
       SUB_PORT_MAP_TABLE */
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_decrypt_flow_create(unit, 0, 0,
                                            &flow_info1, 0, flow_id) );
    
    printf("++++ Enable flows\n");
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_decrypt_flow_enable_set(unit,
                                                                *flow_id, 1) );
    
    return BCM_E_NONE;
}

bcm_error_t
    get_flows(bcm_xflow_macsec_flow_id_t flow_id,
                bcm_xflow_macsec_decrypt_flow_info_t *flow_info, int *priority)
{
    printf("++++ Get the flow\n");
    return bcm_xflow_macsec_decrypt_flow_get(unit, flow_id, &flow_info, priority);
}

/* Required to avoid SP tcam miss. */
bcm_error_t
    set_portcontrol(bcm_port_t dwnlink_port)
{
    bcm_xflow_macsec_port_info_t port_info;
    int flags, id, oper = BCM_XFLOW_MACSEC_DECRYPT;

    flags = BCM_XFLOW_MACSEC_MATCH_TPID_SEL_0;
    id = bcmXflowMacsecPortTPIDEnable;
    COMPILER_64_SET(port_info.value, 0, flags);
    BCM_IF_ERROR_RETURN( bcm_xflow_macsec_port_control_set(unit, oper,
                                            dwnlink_port, id, port_info) );
    return BCM_E_NONE;
}


bcm_xflow_macsec_secure_chan_id_t       chan_id_encrypt;
bcm_xflow_macsec_secure_chan_id_t       chan_id_decrypt;
uint64                                  sci;
uint64                                  sci_mask;
bcm_xflow_macsec_crypto_aes128_gcm_t    crypto_key;

bcm_error_t
    ConfigureDecrypt()
{
    bcm_xflow_macsec_secure_assoc_id_t      assoc_id_decrypt;
    bcm_xflow_macsec_policy_id_t    policy_id;
    bcm_xflow_macsec_flow_id_t      flow_id;
    
    printf("Doing create sc decrypt...\n");
    BCM_IF_ERROR_RETURN( create_sc_decrypt(&sci, &sci_mask, &chan_id_decrypt) );
    
    printf("Doing create sa...\n");
    BCM_IF_ERROR_RETURN( create_sa(chan_id_decrypt, crypto_key, &assoc_id_decrypt) );
    
    printf("Doing create policy...\n");
    BCM_IF_ERROR_RETURN( create_policy(&sci, chan_id_decrypt, &policy_id) );
    
    printf("Doing create flows...\n");
    BCM_IF_ERROR_RETURN( create_flows(dwnlink_port_macsec1, policy_id, &flow_id) );
    
    printf("Doing set port control...\n");
    BCM_IF_ERROR_RETURN( set_portcontrol(dwnlink_port_macsec1) );
    
    return BCM_E_NONE;
}


bcm_error_t
    TestFunc(int prio)
{
    bcm_field_group_t gid = 1;
    bcm_xflow_macsec_secure_assoc_id_t      assoc_id_encrypt;
    
    /*
     *      uplink_port - uplink port connected to the FB6.
     *      downlink_port - redirect to the FL's downlink port(front/access port).
     *      cph_header - custom header
     *      cph_match_mask - custom header match mask to check if present
     *      cph_mask - custom header mask used for FP qualify
     *      pri     - trust priority; INT_PRI will be limited to a single
     *                              values of 0 for all traffic.
     *      fp_gid - fp group ID
     *      fp_eid - fp entry ID
    */
    BCM_IF_ERROR_RETURN( line_card_downstream_fb6_to_fl_non_macsec(
        uplink_port1 /*xe0*/, dwnlink_port_normal1 /*ge0*/,
        cph_noenc_match_hdr_data /*cph_header*/, cph_match_mask /*cph_match_mask*/,
        cph_match_mask /*cph_mask*/, 0 /* trust priority */,
        gid /*FP Group ID*/, 1 /*FP Entry ID*/) );
    
    /*
     *      uplink_port - uplink port connected to the FB6.
     *      downlink_port - redirect to the FL's downlink port(front/access port).
     *      cph_header - custom header
     *      cph_match_mask - custom header match mask to check if present
     *      cph_mask - custom header mask used for FP qualify
     *      pri     - trust priority; INT_PRI will be limited to a single
     *                                      values of 0 for all traffic.
     *      fp_gid - fp group ID
     *      fp_eid - fp entry ID
     */
    BCM_IF_ERROR_RETURN( line_card_downstream_fb6_to_fl_with_macsec(
        uplink_port1 /*xe0*/, dwnlink_port_macsec1 /*ge1*/,
        cph_enc_match_hdr_data /*cph_header*/, cph_match_mask /*cph_match_mask*/,
        cph_match_mask /*cph_mask*/, 0 /* trust priority */,
        gid /*FP Group ID*/, 2 /*FP Entry ID*/) );
    
    if( 0 ) {
        /*  Default drop packet for not matching the cph; send packets with no
         *  CPH header or un-matched CPH to test.
         */
        BCM_IF_ERROR_RETURN( line_card_downstream_fp_default_no_cph_hit_drop(
            gid /*FP Group ID*/, 3 /*FP Entry ID*/, uplink_port1 /*source port: xe0*/) );
    }
    
    printf("Doing init...\n");
    BCM_IF_ERROR_RETURN( macsec_app_init(&sci, &sci_mask, &crypto_key) );
    
    if( 1 ) {
        printf("Doing create sc...\n");
        BCM_IF_ERROR_RETURN( create_sc_encrypt(&sci, &sci_mask, &chan_id_encrypt) );
        
        printf("Doing create sa...\n");
        BCM_IF_ERROR_RETURN( create_sa(chan_id_encrypt, &crypto_key, &assoc_id_encrypt) );
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
    
    if( 1 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, uplink_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, dwnlink_port_normal1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, dwnlink_port_macsec1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, dwnlink_port_macsec2);
        
        BCM_PBMP_CLEAR(ut_pbmp);
        if( 0 ) {
            BCM_PBMP_PORT_ADD(ut_pbmp, dwnlink_port_normal1);
            BCM_PBMP_PORT_ADD(ut_pbmp, dwnlink_port_macsec1);
            BCM_PBMP_PORT_ADD(ut_pbmp, dwnlink_port_macsec2);
        }
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid65));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid65, vlan_pbmp, ut_pbmp));

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, 0);    // cpu
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    /*
    if( 0 )
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_mac_aa, test_vid65);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        printf("Doing L2 entry add\n");
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    */
    if( 0 )
    {
        rv = ingress_port_setup(unit, uplink_port1, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", uplink_port1, rv);
            return rv;
        }
        
        bcm_pbmp_t pbmp;
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, dwnlink_port_normal1);
        BCM_PBMP_PORT_ADD(pbmp, dwnlink_port_macsec1);
        rv = egress_port_multi_setup(unit, pbmp, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_multi_setup() failed for ports (Error: %d)\n", rv);
            return rv;
        }
    }
    else if( 1 ) {
        char cmd[128];
        
        printf("Doing port loopbacks n dmirror...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, uplink_port1,
                                                        loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, uplink_port1,
                                                        BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, dwnlink_port_macsec2,
                                                        loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, dwnlink_port_macsec2,
                                                        BCM_PORT_DISCARD_NONE));
        
        /* Egress ports */
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, dwnlink_port_normal1,
                                                        loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, dwnlink_port_normal1,
                                                        BCM_PORT_DISCARD_ALL));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, dwnlink_port_macsec1,
                                                        loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, dwnlink_port_macsec1,
                                                        BCM_PORT_DISCARD_ALL));
        
        sprintf(cmd, "dmirror %d mode=ingress dp=cpu0 dm=0", uplink_port1);
            bbshell(unit, cmd);
        sprintf(cmd, "dmirror %d mode=ingress dp=cpu0 dm=0", dwnlink_port_macsec2);
            bbshell(unit, cmd);
        sprintf(cmd, "dmirror %d mode=ingress dp=cpu0 dm=0", dwnlink_port_normal1);
            bbshell(unit, cmd);
        sprintf(cmd, "dmirror %d mode=ingress dp=cpu0 dm=0", dwnlink_port_macsec1);
            bbshell(unit, cmd);
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, uplink_port1,
                                                        loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, uplink_port1,
                                                        BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, dwnlink_port_normal1,
                                                        loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, dwnlink_port_normal1,
                                                        BCM_PORT_DISCARD_ALL));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, dwnlink_port_macsec1,
                                                        loopBackType));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, dwnlink_port_macsec1,
                                                        BCM_PORT_DISCARD_ALL));
    }
    
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
    int i;
    uint64 val;
    int rv;
    
    COMPILER_64_SET(val, 0x0, 0x0);
    get_stats_encrypt(chan_id_encrypt, bcmXflowMacsecCtrlPortOutOctets, &val);
    print val;
    printf("\n");
    
    for(i=0; i<4; ++i) {
        
        if( i == 2) {
            printf("\n\n ######## Configuring Decryption...#########\n\n");
            rv = ConfigureDecrypt();
            if( BCM_FAILURE(rv) ) {
                printf("Configuring Decryption FAILED : %s (%d)\n", bcm_errmsg(rv), rv);
                return rv;
            }
        }
        
        // tx 1 pbm=2 data=0000000000AA0000000000EE1300040B81000065080045000046000000003F111B461010101120202021111122220036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DF89253DA
        printf("\n\n##########   Sending FB6 Captured packet with SVTAG ENCRYPT %d ########\n", i+1);
        
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X00%02X%02X8100%04X080045000046000000003F111B461010101120202021111122220036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DF89253DA",
            uplink_port1,
            test_mac_aa[0], test_mac_aa[1], test_mac_aa[2],
            test_mac_aa[3], test_mac_aa[4], test_mac_aa[5],
            test_mac_ee[0], test_mac_ee[1], test_mac_ee[2],
            test_mac_ee[3], test_mac_ee[4], test_mac_ee[5],
            svtag_sig,
            pkt_type_do_encrypt,
            fl_subport_num2,
            FORM_VID(prio, 0, test_vid65));
        bbshell(unit, cmd);
        
        bshell(unit, "sleep quiet 1");
        
        COMPILER_64_SET(val, 0x0, 0x0);
        get_stats_encrypt(chan_id_encrypt, bcmXflowMacsecCtrlPortOutOctets, &val);
        print val;
        bshell(unit, "sleep quiet 2");
    }
    
    printf("\n");
    
    bbshell(unit, "show c");
    bbshell(unit, "fp show");
    bbshell(unit, "ps");
    bbshell(unit, "vlan show");
    bbshell(unit, "ver");
    bbshell(unit, "a");
    
    printf("\n\n\t Expected Result:  1st 2 packets will be ONLY encrypted. ");
    printf("After that you will see Decrypted packets.\n\n");
    
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
    
    if( (rv = TestFunc(999)) != BCM_E_NONE )
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

void main() {
    const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
    if (!sal_strcmp(auto_execute, "YES")) {
      print "execute(): Start";
      print execute();
      print "execute(): End";
    }
}

main();

/*

BCM.0> 
BCM.0> noe == Ports {xe4,xe8} are connected via a DAC cable.
BCM.0> noe == Port xe4 is MACsec-enabled.
BCM.0> noe == Port xe8 is a normal port.
BCM.0> noe =================================================
BCM.0> ps xe4,xe8
                 ena/    speed/ link auto    STP                  lrn  inter   max  loop
           port  link    duplex scan neg?   state   pause  discrd ops   face frame  back
       xe4(  6)  up     10G  FD   SW  No   Forward  TX RX   None   FA    XFI 16356
       xe8( 10)  up     10G  FD   SW  No   Forward  TX RX   None   FA    XFI 16356
BCM.0>
BCM.0> vlan create 2 pbm=xe4,xe8
BCM.0>
BCM.0> fp init
BCM.0> fp qset clear
BCM.0> fp qset add Stageingress
BCM_FIELD_QSET_ADD(StageIngress) okay
BCM.0> fp qset add Inport
BCM_FIELD_QSET_ADD(InPort) okay
BCM.0> fp group create 0 0
BCM.0> fp entry create 0 1
BCM.0> fp qual 1 Inport xe8 0x3f
BCM.0> fp action add 1 copytocpu 1 1
BCM.0> fp action add 1 drop
BCM.0> fp entry install 1
BCM.0> pw start report +decode
BCM.0>
BCM.0>
BCM.0> cint encrypt.cint  // Scripts at the pages 33-35 
BCM.0> cint
Entering C Interpreter. Type 'exit;' to quit.

cint> init();
cint> create_sc();
++++ Create a new Security Channel for an encrypt flow
cint> create_sa();
++++ Create a security association for the given chan_id.
cint>
cint> get_stats(bcmXflowMacsecCtrlPortOutOctets);
int $$ = 0 (0x0)
xflow_macsec_subport_id_t subport_id = 1140850688 (0x44000000)
int $$ = 0 (0x0)
uint64 val = {0x00000000 0x00000000}
cint>
cint> bshell(unit, "tx 1 pbm=xe4 data=FFFFFFFFFFFF000000000901FF04040081000002080600010800060400010000000009010A00006400000000FFFF0A00FFFF000000000000000000000000000000000000000000000000000000005566");
Packet from data=<>, length=84
Warning:  Untagged packet read from file for tx.
cint> 
[bcmPW.0]Packet[1]: data[0000]: {ffffffffffff} {000000000901} 8100 0002
[bcmPW.0]Packet[1]: data[0010]: 88e5 0800 0000 0001 1785 728a b52b 675e
[bcmPW.0]Packet[1]: data[0020]: c8bd e19c ea89 91c9 0a1d 3782 4418 5e2e
[bcmPW.0]Packet[1]: data[0030]: d226 f651 d8a7 af0e da8d f1b6 ec8a dc2e
[bcmPW.0]Packet[1]: data[0040]: 7c6e 2a1b 2aa1 c6e5 566f 5ef6 b91f 8e2d
[bcmPW.0]Packet[1]: data[0050]: c159 1be1 0361 ea4c 9fd8 649d 6961 60e4
[bcmPW.0]Packet[1]: data[0060]: 031a 1cc5 2dea 583d
[bcmPW.0]Packet[1]: length 104 (104). rx-port 10. cos 0. prio_int 0. vlan 2. reason 0x1000. Outer tagged.
[bcmPW.0]Packet[1]: dest-port 2. dest-mod 0. src-port 10. src-mod 0. opcode 2.  matched 1. classification-tag 0.
[bcmPW.0]Packet[1]: reasons: FilterMatch
[bcmPW.0]Packet[1]:
Packet[1]:   Ethernet: dst<ff:ff:ff:ff:ff:ff> src<00:00:00:00:09:01> Tagged Packet ProtID<0x8100> Ctrl<0x0002> * Unknown/Experimental format * ff ff ff ff ff ff 00 00 00 00 09 01 81 00 00 02 88 e5 08 00 00 00 00 01 17 85 72 8a b5 2b 67 5e c8 bd e1 9c ea 89 91 c9 0a 1d 37 82 44 18 5e 2e d2 26 f6 51 d8 a7 af 0e da 8d f1 b6 ec 8a dc 2e

cint>
cint> get_stats(bcmXflowMacsecCtrlPortOutOctets);
int $$ = 0 (0x0)
xflow_macsec_subport_id_t subport_id = 1140850688 (0x44000000)
int $$ = 0 (0x0)
uint64 val = {0x00000000 0x00000048}
cint>
cint>


*/

/*

rack# ./bcm.user
Broadcom Command Monitor: Copyright (c) 1998-2020 Broadcom
Release: sdk-6.5.20 built 20200420 (Mon Apr 20 09:46:01 2020)
From sr936857@xl-sj1-31:/projects/ntsw-sw6/home/sr936857/gitcode/sdk
Platform: SLK_BCM957812
OS: Unix (Posix)
DMA pool size: 33554432
BDE dev 0 (PCI), Dev 0xb070, Rev 0x01, Chip BCM56070_A0, Driver BCM56070_A0
SOC unit 0 attached to PCI device BCM56070_A0
WARNING: bcm esw command CoupledMemWrite not alphabetized
Boot flags: Cold boot
rc: unit 0 device BCM56070_A0
open /dev/linux-bcm-knet: : No such device or address
MCSLoad: Error: Unable to open file: BCM56070_A0_1_ptpfull.srec
rc: BCM driver initialized
rc: L2 Table shadowing enabled
rc: Port modes initialized
BCM.0> cd cint
BCM.0> 
BCM.0> 
BCM.0> aa
int ARGC = 0 (0x0)
int opt_TestOption1 = 0 (0x0)
bcm_field_stage_t opt_TestFpStage = bcmFieldStageIngress (5)
bcm_field_qualify_t opt_TestFpQualStage = bcmFieldQualifyStageIngress (68)
int opt_IsVfp = 0 (0x0)
int opt_IsIfp = 1 (0x1)
int opt_IsEfp = 0 (0x0)
int ing_port1 = 2 (0x2)
int ing_port2 = 3 (0x3)
int ing_port3 = 4 (0x4)
int egr_port1 = 5 (0x5)
execute(): Start
Doing L2 entry add
Doing init...
SCI=11 (0000000B)
uint64 $$ = {0x00000000 0x0000000B}
Doing create sc...
++++ Create a new Security Channel for an encrypt flow *chan_id=0x400000B
++++ Created *chan_id=0x400000B
Doing create sa...
++++ Create a security association for the given chan_id.
int $$ = 0 (0x0)
xflow_macsec_subport_id_t subport_id = 1140850699 (0x4400000b)
int $$ = 0 (0x0)
uint64 val = {0x00000000 0x00000000}

##########   Sending ARP packet with SVTAG ########
B_CM.0> tx 1 pbm=2 data=0000000000AA0000000000EE1300040B81000065080045000046000000003F111B461010101120202021111122220036C6B0000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2DF89253DA
Packet from data=<>, length=104
Warning:  Untagged packet read from file for tx.
int $$ = 0 (0x0)
xflow_macsec_subport_id_t subport_id = 1140850699 (0x4400000b)
int $$ = 0 (0x0)
uint64 val = {0x00000000 0x00000000}


	 Expected Result: You will see that Tunnelled packets..

bcm_error_t $$ = BCM_E_NONE (0)
execute(): End
[bcmPW.0]
[bcmPW.0]Packet[1]: data[0000]: {0000000000aa} {0000000000ee} 8100 0001
[bcmPW.0]Packet[1]: data[0010]: 1300 040b 8100 0065 0800 4500 0046 0000 
[bcmPW.0]Packet[1]: data[0020]: 0000 3f11 1b46 1010 1011 2020 2021 1111 
[bcmPW.0]Packet[1]: data[0030]: 2222 0036 c6b0 0001 0203 0405 0607 0809 
[bcmPW.0]Packet[1]: data[0040]: 0a0b 0c0d 0e0f 1011 1213 1415 1617 1819 
[bcmPW.0]Packet[1]: data[0050]: 1a1b 1c1d 1e1f 2021 2223 2425 2627 2829 
[bcmPW.0]Packet[1]: data[0060]: 2a2b 2c2d f892 53da 1cdf 4421 
[bcmPW.0]Packet[1]: length 108 (108). rx-port 2. cos 0. prio_int 0. vlan 1. reason 0x1000. Untagged.
[bcmPW.0]Packet[1]: dest-port 1. dest-mod 0. src-port 2. src-mod 0. opcode 2.  matched 222. classification-tag 0.
[bcmPW.0]Packet[1]: reasons: FilterMatch
BCM.0> 
BCM.0> 
BCM.0> 


*/


/*
  ESEC_CONFIG
  ESEC_CTRL
      ESEC_CW_FIFO_INTR
      ESEC_CW_FIFO_STATUS
      ESEC_EGRESS_MTU_FOR_MGMT_PKT
      ESEC_ERROR_INTR_ENABLE
      ESEC_ERROR_INTR_STATUS
      ESEC_FIPS_DATA_REG0
      ESEC_FIPS_DATA_REG1
      ESEC_FIPS_ENC_DATA_REG0
      ESEC_FIPS_ENC_DATA_REG1
      ESEC_FIPS_ICV_REG0
      ESEC_FIPS_ICV_REG1
      ESEC_FIPS_OVR_CONTROL
      ESEC_FIPS_OVR_IV_REG0
      ESEC_FIPS_OVR_IV_REG1
      ESEC_FIPS_OVR_KEY_REG0
      ESEC_FIPS_OVR_KEY_REG1
      ESEC_FIPS_OVR_KEY_REG2
      ESEC_FIPS_OVR_KEY_REG3
      ESEC_FIPS_STATUS
      ESEC_HASH_TABLE_ECC_STATUS
      ESEC_ICF_INTR
      ESEC_ICF_STATUS
      ESEC_IDF_INTR
      ESEC_IDF_STATUS
      ESEC_INTR_ENABLE
      ESEC_MIB_MISC_ECC_STATUS
      ESEC_MIB_SA_ECC_STATUS
      ESEC_MIB_SC_CTRL_ECC_STATUS
      ESEC_MIB_SC_ECC_STATUS
      ESEC_MIB_SC_UNCTRL_ECC_STATUS
      ESEC_ODF_INTR
      ESEC_ODF_STATUS
      ESEC_PN_THD
      ESEC_SA_TABLE_ECC_STATUS
      ESEC_SC_TABLE_ECC_STATUS
  ESEC_SPARE
  ESEC_STATUS
  ESEC_SVTAG_ETYPE
      ESEC_TAG_FIFO_INTR
      ESEC_TAG_FIFO_STATUS
  ESEC_VXLANSEC_DEST_PORT_NO
      ESEC_XPN_THD
      MACSEC_ESEC_ISEC_STATUS
//////////////////////////////////////////////////
      ISEC_AES_CALC_ICV_REG0
      ISEC_AES_CALC_ICV_REG1
      ISEC_AES_ICV_FAIL_CNT
      ISEC_AES_ICV_FAIL_STATUS
      ISEC_AES_RCV_ICV_REG0
      ISEC_AES_RCV_ICV_REG1
      ISEC_CB_STR_FIFO_INTR
      ISEC_CB_STR_FIFO_STATUS
  ISEC_CTRL
      ISEC_CW_FIFO_INTR
      ISEC_CW_FIFO_STATUS
      ISEC_ERROR_INTR_ENABLE
      ISEC_ERROR_INTR_STATUS
  ISEC_ETYPE_MAX_LEN
      ISEC_FIPS_DATA_REG0
      ISEC_FIPS_DATA_REG1
      ISEC_FIPS_ENC_DATA_REG0
      ISEC_FIPS_ENC_DATA_REG1
      ISEC_FIPS_ICV_REG0
      ISEC_FIPS_ICV_REG1
      ISEC_FIPS_OVR_CONTROL
      ISEC_FIPS_OVR_IV_REG0
      ISEC_FIPS_OVR_IV_REG1
      ISEC_FIPS_OVR_KEY_REG0
      ISEC_FIPS_OVR_KEY_REG1
      ISEC_FIPS_OVR_KEY_REG2
      ISEC_FIPS_OVR_KEY_REG3
      ISEC_FIPS_STATUS
      ISEC_HASH_TABLE_ECC_STATUS
      ISEC_ICF_INTR
      ISEC_ICF_STATUS
      ISEC_IDF_INTR
      ISEC_IDF_STATUS
      ISEC_INTR_ENABLE
  ISEC_IPV4_ETYPE
  ISEC_IPV6_ETYPE
      ISEC_MGMTRULE_CFG_DA_0
      ISEC_MGMTRULE_CFG_DA_1
      ISEC_MGMTRULE_CFG_DA_2
      ISEC_MGMTRULE_CFG_DA_3
      ISEC_MGMTRULE_CFG_DA_4
      ISEC_MGMTRULE_CFG_DA_5
      ISEC_MGMTRULE_CFG_DA_6
      ISEC_MGMTRULE_CFG_DA_7
      ISEC_MGMTRULE_CFG_ETYPE_0
      ISEC_MGMTRULE_CFG_ETYPE_1
      ISEC_MGMTRULE_DA_ETYPE_1ST
      ISEC_MGMTRULE_DA_ETYPE_2ND
      ISEC_MGMTRULE_DA_RANGE_HIGH
      ISEC_MGMTRULE_DA_RANGE_LOW
      ISEC_MIB_SA_ECC_STATUS
      ISEC_MIB_SC_ECC_STATUS
      ISEC_MIB_SP_CTRL_1_ECC_STATUS
      ISEC_MIB_SP_CTRL_2_ECC_STATUS
      ISEC_MIB_SP_UNCTRL_ECC_STATUS
  ISEC_MISC_CTRL
  ISEC_MPLS_ETYPE
      ISEC_MTU_FAIL_CNT
  ISEC_NIV_ETYPE
      ISEC_ODF_INTR
      ISEC_ODF_STATUS
      ISEC_OUT_DESTPORT_NO
      ISEC_PAD_CTRL
      ISEC_PBB_TPID
      ISEC_PCF_BANK0_INTR
      ISEC_PCF_BANK0_STATUS
      ISEC_PCF_BANK1_INTR
      ISEC_PCF_BANK1_STATUS
      ISEC_PDF_INTR
      ISEC_PDF_STATUS
  ISEC_PE_ETYPE
  ISEC_PN_EXPIRE_THD
      ISEC_PORT_COUNTERS_ECC_STATUS
      ISEC_PP_CTRL
      ISEC_PTP_DEST_PORT_NO
  ISEC_PTP_ETYPE
      ISEC_RUD_MGMT_RULE_CTRL
      ISEC_SA_TABLE_ECC_STATUS
      ISEC_SCTCAM_HIT_COUNT_ECC_STATUS
      ISEC_SCTCAM_SER_STATUS
      ISEC_SC_TABLE_ECC_STATUS
      ISEC_SER_CONTROL
      ISEC_SER_SCAN_CONFIG
      ISEC_SER_SCAN_STATUS
  ISEC_SPARE
      ISEC_SPTCAM_HIT_COUNT_ECC_STATUS
      ISEC_SPTCAM_SER_STATUS
      ISEC_SP_MAP_ECC_STATUS
      ISEC_SP_POLICY_ECC_STATUS
  ISEC_SVTAG_CTRL
      ISEC_TAG_FIFO_INTR
      ISEC_TAG_FIFO_STATUS
  ISEC_TCP_PROTOCOL
  ISEC_TPID_0
  ISEC_TPID_1
  ISEC_UDP_PROTOCOL
      ISEC_VXLANSEC_DEST_PORT_NO
      ISEC_XPN_EXPIRE_THD
  MACSEC_ESEC_ISEC_STATUS

*/

