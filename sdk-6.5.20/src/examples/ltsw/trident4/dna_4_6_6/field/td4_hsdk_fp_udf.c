/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Demonstration of UDF features in TD4. (Chunk based architecture)
 *
 *  Usage : BCM.0> cint fp_udf.c YES
 *           E.g.  cint fp_udf.c YES
 *
 *  config   : bcm56880_a0-generic-32x400.config.yml
 *
 *  Log file : fp_udf_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *  +----------------+                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *      UDF Extracts the user defined fields from the packet for match in FP. In this test, L4 Dest
 *      Port is extracted from wither TCP packet or a UDP packet. Then this extracted field is
 *      matched in an IFP entry.
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Port 2 ports in loopback for testing.
 *       b) Create a vlan.
 *       c) Add ports to the VLAN.
 *       d) Add test DMAC to the L2 Entry table for this VLAN for the selected Egress Port.
 *
 *    2) Step2 - Configuration (Done in DoUdfFpConfig()).
 *    ===================================================
 *       a) Configure UDF to extract L4 Source Port from TCP packets and a UDP Packets, BOTH. So
 *          two UDF objects are created. 2 Bytes Field will be extracted from offset of 2 bytes
 *          from the base of L4. That will be L4 Dest Port.
 *       b) IFP Entry is created to match on the extracted range by both UDF objects.
 *       c) STAT is also attached to entry for verification
 *
 *    2) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send IPv4 UDP packet with matching Dest Port
 *       b) Send IPv4 TCP packet with matching Dest Port
 *       b) Expected Result:
 *          ================
 *          STAT count should increment for both the cases.
 *
 * NOTE: Use opt_* variables to change the test variant
 */

cint_reset();

/****************   GENERIC UTILITY FUNCTIONS STARTS  **************************
 */
bcm_port_t PORT_ANY = -1;                                                  /*H*/
bcm_port_t __next_port_to_return = 1;                                      /*H*/
bcm_port_t         // Get next valid Ethernet port                         /*H*/
 portGetNextE(int unit, bcm_port_t PreferredPort)                          /*H*/
{                                                                          /*H*/
  int i=0, rv=0;                                                           /*H*/
  bcm_port_config_t configP;                                               /*H*/
  bcm_pbmp_t ports_pbmp;                                                   /*H*/
  if( PreferredPort != PORT_ANY ) {                                        /*H*/
    printf("Using preferred port %d\n", PreferredPort);                    /*H*/
    return PreferredPort;                                                  /*H*/
  }                                                                        /*H*/
  rv = bcm_port_config_get(unit, &configP);                                /*H*/
  if(BCM_FAILURE(rv)) {                                                    /*H*/
    printf("\nError in retrieving port configuration: %s %d.\n",           /*H*/
        bcm_errmsg(rv), __next_port_to_return);                            /*H*/
    exit;                                                                  /*H*/
  }                                                                        /*H*/
  ports_pbmp = configP.e;    // configP.hg;                                /*H*/
  for (i= __next_port_to_return; i < BCM_PBMP_PORT_MAX; i++) {             /*H*/
    if ( BCM_PBMP_MEMBER(&ports_pbmp, i) ) {                               /*H*/
        __next_port_to_return = i+1; // point to a probable next port      /*H*/
        return i;                                                          /*H*/
    }                                                                      /*H*/
  }                                                                        /*H*/
  printf("ERROR: Not enough ports %d\n", __next_port_to_return);           /*H*/
  exit;                                                                    /*H*/
}                                                                          /*H*/
bcm_error_t                                                                /*H*/
 common_fp_port_setup(int unit, bcm_port_t port, int LoopBackType, int Ing)/*H*/
{                                                                          /*H*/
  bcm_field_group_t group;                                                 /*H*/
  bcm_field_group_config_t group_config;                                   /*H*/
  uint32  pri, fp_statid, match_id, mode, ctr_action_id;                   /*H*/
  bcm_field_entry_t entry;                                                 /*H*/
  if(Ing) {                                                                /*H*/
      group = 60;                                                          /*H*/
      fp_statid = test_setup_flexctr_cfg_index_base;                       /*H*/
      mode = BCM_PORT_DISCARD_NONE; match_id = 0xAA;                       /*H*/
  } else {                                                                 /*H*/
      group = 61;                                                          /*H*/
      fp_statid = test_setup_flexctr_cfg_index_base+1;                     /*H*/
      mode = BCM_PORT_DISCARD_ALL; match_id = 0x55;                        /*H*/
      BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,                 /*H*/
                                    bcmPortControlTrustIncomingVlan, 0));  /*H*/
  }                                                                        /*H*/
  entry = group;                                                           /*H*/
  pri = 64-group;                                                          /*H*/
  //printf("\n1.Doing port loopback set...\n");                            /*H*/
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /*H*/
//printf("1.Doing port discard set...\n");                                 /*H*/
//BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, mode));             /*H*/
  bcm_field_group_config_t_init(&group_config);                            /*H*/
  BCM_FIELD_ASET_INIT(group_config.aset);                                  /*H*/
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);          /*H*/
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);          /*H*/
  //BCM_FIELD_ASET_ADD(group_config.aset,                                  /*H*/
  //                        bcmFieldActionStatGroupWithoutCounterIndex);   /*H*/
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /*H*/
  group_config.mode = bcmFieldGroupModeAuto;                               /*H*/
  group_config.group = group;                                              /*H*/
  group_config.priority = pri;                                             /*H*/
  BCM_FIELD_QSET_INIT(group_config.qset);                                  /*H*/
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);            /*H*/
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);      /*H*/
  //printf("1.Doing field group create...\n");                             /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config)); /*H*/
  //printf("1.Doing field entry create...\n");                             /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /*H*/
  //printf("1.Doing qualify InPort...\n");                                 /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /*H*/
            BCM_FIELD_EXACT_MATCH_MASK));                                  /*H*/
  //printf("1.Doing action add...\n");                                     /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /*H*/
            bcmFieldActionCopyToCpu, 1, match_id));                        /*H*/
  //printf("1.Doing flexctr create...\n");                                 /*H*/
  BCM_IF_ERROR_RETURN(flexctr_fp_stat_create(unit, group,                  /*H*/
                                bcmFieldStageIngress, &ctr_action_id));    /*H*/
  flexctr_cfg[fp_statid-1].flexctr_action_id = ctr_action_id;              /*H*/
  flexctr_cfg[fp_statid-1].counter_index = fp_statid;                      /*H*/
  //printf("1.Doing flexctr attach...\n");                                 /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_entry_flexctr_attach(unit, entry,          /*H*/
            &flexctr_cfg[fp_statid-1]));                                   /*H*/
  //printf("1.Doing entry install...\n");                                  /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /*H*/
  return BCM_E_NONE;                                                       /*H*/
}                                                                          /*H*/
bcm_error_t                                                                /*H*/
 ingress_port_setup(int unit, bcm_port_t port, int LoopBackType)           /*H*/
{                                                                          /*H*/
  return common_fp_port_setup(unit, port, LoopBackType, 1);                /*H*/
}                                                                          /*H*/
bcm_error_t                                                                /*H*/
 egress_port_setup(int unit, bcm_port_t port, int LoopBackType)            /*H*/
{                                                                          /*H*/
  return common_fp_port_setup(unit, port, LoopBackType, 0);                /*H*/
}                                                                          /*H*/
bcm_error_t                                                                /*H*/
 egress_port_multi_setup(int unit, bcm_pbmp_t pbm, int LoopBackType)       /*H*/
{       /* ifp_inports_support_enable */                                   /*H*/
  bcm_field_group_t group = 62;                                            /*H*/
  uint32  pri, fp_statid, match_id, mode, ctr_action_id;                   /*H*/
  bcm_field_group_config_t group_config;                                   /*H*/
  bcm_field_entry_t   entry;                                               /*H*/
  bcm_port_t     port;                                                     /*H*/
  BCM_PBMP_ITER(pbm, port) {                                               /*H*/
      BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));/*H*/
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,                   /*H*/
                                     bcmPortControlTrustIncomingVlan, 0)); /*H*/
  }                                                                        /*H*/
  entry = group; fp_statid = test_setup_flexctr_cfg_index_base+2;          /*H*/
  match_id = group; pri = 64-group;                                        /*H*/
  print pbm;                                                               /*H*/
  bcm_pbmp_t pbm_mask;                                                     /*H*/
  BCM_PBMP_CLEAR(pbm_mask);                                                /*H*/
  BCM_PBMP_NEGATE(pbm_mask, pbm_mask);                                     /*H*/
  bcm_field_group_config_t_init(&group_config);                            /*H*/
  BCM_FIELD_ASET_INIT(group_config.aset);                                  /*H*/
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);          /*H*/
  BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);          /*H*/
  //BCM_FIELD_ASET_ADD(group_config.aset,                                  /*H*/
  //                        bcmFieldActionStatGroupWithoutCounterIndex);   /*H*/
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /*H*/
  group_config.mode = bcmFieldGroupModeAuto;                               /*H*/
  group_config.group = group;                                              /*H*/
  group_config.priority = pri;                                             /*H*/
  BCM_FIELD_QSET_INIT(group_config.qset);                                  /*H*/
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPorts);           /*H*/
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);      /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit,                  /*H*/
                                                    &group_config));       /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, entry,               /*H*/
              pbm, pbm_mask));                                             /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /*H*/
              bcmFieldActionCopyToCpu, 1, match_id));                      /*H*/
  BCM_IF_ERROR_RETURN(flexctr_fp_stat_create(unit, group,                  /*H*/
                                bcmFieldStageIngress, &ctr_action_id));    /*H*/
  flexctr_cfg[fp_statid-1].flexctr_action_id = ctr_action_id;              /*H*/
  flexctr_cfg[fp_statid-1].counter_index = fp_statid;                      /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_entry_flexctr_attach(unit, entry,          /*H*/
            &flexctr_cfg[fp_statid-1]));                                   /*H*/
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /*H*/
  return BCM_E_NONE;                                                       /*H*/
}                                                                          /*H*/
void bbshell(int unit, char *str)                                          /*H*/
{                                                                          /*H*/
  printf("B_CM.%d> %s\n", unit, str);                                      /*H*/
  bshell(unit, str);                                                       /*H*/
}                                                                          /*H*/
int chartoint(int c)                                                       /*H*/
{                                                                          /*H*/
    char *hex = "aAbBcCdDeEfF";                                            /*H*/
    int i, result = 0;                                                     /*H*/
    for(i = 0; (result == 0) && (hex[i] != '\0'); i++) {                   /*H*/
        if(hex[i] == c) {                                                  /*H*/
            result = 10 + (i / 2);                                         /*H*/
        }                                                                  /*H*/
    }                                                                      /*H*/
    return result;                                                         /*H*/
}                                                                          /*H*/
uint32 mine_dtoi(char *str)                                                /*H*/
{                                                                          /*H*/
    int i;                                                                 /*H*/
    uint32 dec = 0;                                                        /*H*/
    for(i=0; str[i]!=0; i++) {                                             /*H*/
        dec = dec * 10 + ( str[i] - '0' );                                 /*H*/
    }                                                                      /*H*/
    return dec;                                                            /*H*/
}                                                                          /*H*/
/* Converts String to Hexadecimal or Decimal. Hex is preceding is */       /*H*/
/* 'x' or 'X'. Else Decimal */                                             /*H*/
uint32 mine_atoi(char *str)                                                /*H*/
{                                                                          /*H*/
    uint32 result = 0;                                                     /*H*/
    int i = 0, proper = 1, temp, is_decimal = 1;                           /*H*/
    //To take care of 0x and 0X added before the hex no.                   /*H*/
    if((str[0] == 'x') || (str[0] == 'X') ) {                              /*H*/
        is_decimal = 0;                                                    /*H*/
    }                                                                      /*H*/
    if(str[0] == '0') {                                                    /*H*/
        is_decimal = 0;                                                    /*H*/
        ++i;                                                               /*H*/
        if((str[i] == 'x') || (str[i] == 'X') ) {                          /*H*/
            ++i;                                                           /*H*/
        } else {                                                           /*H*/
            i--; // '0' was a valid 0 as no trailing 'x' found             /*H*/
        }                                                                  /*H*/
    }                                                                      /*H*/
    if( is_decimal ) {                                                     /*H*/
        return mine_dtoi(str);                                             /*H*/
    }                                                                      /*H*/
    while(proper && (str[i] != '\0') ) {                                   /*H*/
        result = result * 16;                                              /*H*/
        if((str[i] >= '0') && (str[i] <= '9') ) {                          /*H*/
            result = result + (str[i] - '0');                              /*H*/
        } else {                                                           /*H*/
            temp = chartoint(str[i]);                                      /*H*/
            if(temp == 0) {                                                /*H*/
                printf("Improper char 0x%X", str[i]);                      /*H*/
                proper = 0;                                                /*H*/
            } else {                                                       /*H*/
                result = result + temp;                                    /*H*/
            }                                                              /*H*/
        }                                                                  /*H*/
        ++i;                                                               /*H*/
    }                                                                      /*H*/
    //If any character is not a proper hex no. ,  return 0                 /*H*/
    if(!proper) {                                                          /*H*/
     printf("ERROR EVALUATING IN mine_atoi [%s]. Defaulting to 0.\n", str);/*H*/
     result = 0;                                                           /*H*/
    }                                                                      /*H*/
    return result;                                                         /*H*/
}                                                                          /*H*/
uint32 ip4_sum_n_size_update(char *headerStr,                              /*H*/
                                int StartOfIpHdrByte,int SkipEqualCheck)   /*H*/
{                                                                          /*H*/
    uint32 word16, sum=0, i=0, ip4len;                                     /*H*/
    char numStr1[5], numStr2[5];                                           /*H*/
    unsigned char byte1, byte2;                                            /*H*/
    if( !SkipEqualCheck )                                                  /*H*/
        while(i<2) { if( *headerStr++ == '=' ) i++; }                      /*H*/
    StartOfIpHdrByte = StartOfIpHdrByte * 2;  // ascii in text, hence *2   /*H*/
    headerStr = &headerStr[StartOfIpHdrByte];                              /*H*/
    ip4len = (sal_strlen(&headerStr[4*2]))/2;                              /*H*/
    sprintf(numStr1, "%04X", ip4len);                                      /*H*/
    headerStr[4] = numStr1[0]; headerStr[5] = numStr1[1];                  /*H*/
    headerStr[6] = numStr1[2]; headerStr[7] = numStr1[3];                  /*H*/
    numStr1[0] = '0'; numStr1[1] = 'x'; numStr1[2] = 0;                    /*H*/
    numStr1[3] = 0; numStr1[4] = 0;                                        /*H*/
    numStr2[0] = '0'; numStr2[1] = 'x'; numStr2[2] = 0;                    /*H*/
    numStr2[3] = 0; numStr2[4] = 0;                                        /*H*/
    // make 16 bit words out of every two adjacent 8 bit words in          /*H*/
    // the packet and add them up                                          /*H*/
    for (i=0;i<20;i+=2){                                                   /*H*/
        if(i>=10 && i<=11) {                                               /*H*/
            continue;                                                      /*H*/
        } else {                                                           /*H*/
            numStr1[2] = headerStr[i*2];                                   /*H*/
            numStr1[3] = headerStr[(i*2)+1];                               /*H*/
            numStr2[2] = headerStr[(i*2)+2];                               /*H*/
            numStr2[3] = headerStr[(i*2)+3];                               /*H*/
        }                                                                  /*H*/
        byte1 = mine_atoi(numStr1); byte2 = mine_atoi(numStr2);            /*H*/
        word16 =((byte1<<8)&0xFF00)+(byte2&0xFF);                          /*H*/
        sum = sum + word16;                                                /*H*/
    }                                                                      /*H*/
    // take only 16 bits out of the 32 bit sum and add up the carries      /*H*/
    while (sum>>16) sum = (sum & 0xFFFF)+(sum >> 16);                      /*H*/
    sum = (~sum) & 0xFFFF;                                                 /*H*/
    sprintf(numStr1, "%04X", sum);                                         /*H*/
    headerStr[20] = numStr1[0]; headerStr[21] = numStr1[1];                /*H*/
    headerStr[22] = numStr1[2]; headerStr[23] = numStr1[3];                /*H*/
    return sum;                                                            /*H*/
}                                                                          /*H*/
int FORM_VID(int PCP, int DEI, int VID) {                                  /*H*/
    return ( ((PCP)<<13) | ((DEI)<<12) | ((VID) & 0xFFF) );                /*H*/
}                                                                          /*H*/
int FORM_MPLS(int LABEL, int EXP, int S, int TTL) {                        /*H*/
    return ( ((LABEL)<<12) | ((EXP)<<9) | ((S)<<8) | ((TTL) & 0xFF) );     /*H*/
}                                                                          /*H*/
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

// 1st parameter is YES. E.g. cint thisflie.c YES 1 1

int opt_TestOption1 = 1;
int opt_TestOption2 = 1;

print ARGC;

if( ARGC >= 2 ) {
    if( *ARGV[1] == '1' ) {
        opt_TestOption1 = 1;
    } else {
        opt_TestOption1 = 0;
    }
}

if( ARGC >= 3 ) {
    if( *ARGV[2] == '1' ) {
        opt_TestOption2 = 1;
    } else {
        opt_TestOption2 = 0;
    }
}

//print opt_TestOption1;
//print opt_TestOption2;

////////////////////////////////////////////////////////////////////////////////////////

int unit = 0;

bcm_vlan_t  test_vid = 0x64;

bcm_mac_t   test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t   test_smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};

bcm_mac_t   test_mac_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bcm_ip_t    test_sip = 0x10101011;
bcm_ip_t    test_dip = 0x20202021;
bcm_ip_t    test_ip_mask = 0xFFFFFFFF;

uint16      test_L4_src_port = 0x1111;
uint16      test_L4_dst_port = 0x2222;

bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;
bcm_port_t egr_port1 = PORT_ANY;

/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

egr_port1 = portGetNextE(unit, egr_port1);

print ing_port1;
print ing_port2;
print egr_port1;

///////////////////////////////////////////////////////////////////////
int         statid_used = 0;
bcm_field_flexctr_config_t flexctr_cfg[10+3];/* 3 extra for test setup */
int         test_setup_flexctr_cfg_index_base = 11;

bcm_error_t
 flexctr_fp_stat_create(int unit, int group_id, bcm_field_stage_t FpStage,
                                           uint32 *stat_action_id /*OUT*/)
{
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = &action.index_operation;
    bcm_flexctr_value_operation_t *value_a_op = &action.operation_a;
    bcm_flexctr_value_operation_t *value_b_op = &action.operation_b;
    int options = 0;
    
    bcm_flexctr_action_t_init(&action);
    
    action.mode = bcmFlexctrCounterModeNormal;
    action.hint = group_id;
    action.drop_count_mode = bcmFlexctrDropCountModeAll;
    action.index_num = 8192;

    if( FpStage == bcmFieldStageLookup ) {
        action.source = bcmFlexctrSourceVfp;
        index_op->object[0] = bcmFlexctrObjectIngVfpOpaqueObj0;
    } else if( FpStage == bcmFieldStageIngress ) {
        action.source = bcmFlexctrSourceIfp;
        index_op->object[0] = bcmFlexctrObjectIngIfpOpaqueObj0;
    } else if( FpStage == bcmFieldStageEgress ) {
        action.source = bcmFlexctrSourceEfp;
        index_op->object[0] = bcmFlexctrObjectEgrEfpOpaqueObj1;
    } else if( FpStage == bcmFlexctrSourceExactMatch ) {
        action.source = bcmFlexctrSourceExactMatch;
        index_op->object[0] = bcmFlexctrObjectIngEmFtOpaqueObj0;
    }
    
    //index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 13;
    index_op->shift[0] = 0;
    index_op->object[1] = bcmFlexctrObjectConstZero;    // alway = 0
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;
    
    /* Increase counter per packet. */
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne; // Increment by '1'
    value_a_op->mask_size[0] = 16;
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;  // NOOP
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;
    
    /* Increase counter per packet bytes. */
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;
    
    //print action;
    
    BCM_IF_ERROR_RETURN( bcm_flexctr_action_create(unit, options, &action,
                                                    stat_action_id) );
    return BCM_E_NONE;
}

void PrintFpStat(int unit)
{
    int i, rv;
    bcm_flexctr_counter_value_t     counter_value;
    
    printf("\n");
    
    for(i=test_setup_flexctr_cfg_index_base;
        i<test_setup_flexctr_cfg_index_base+3; ++i) {
        if( flexctr_cfg[i-1].counter_index ) {
            switch(i) {
              case test_setup_flexctr_cfg_index_base: printf("Counter for ing:\n"); break;
              case test_setup_flexctr_cfg_index_base+1: printf("Counter for egr:\n"); break;
              case test_setup_flexctr_cfg_index_base+2: printf("Counter for ing multi:\n"); break;
            }
            rv = bcm_flexctr_stat_sync_get(unit,
                                      flexctr_cfg[i-1].flexctr_action_id, 1,
                                      &flexctr_cfg[i-1].counter_index,
                                      &counter_value);
            if(BCM_E_NONE!=rv) {
                printf("Error in reading TEST_SETUP stat %d. i=%d\n", rv, i);
                return;
            }
            print counter_value;
        }
    }
    printf("\n------- Statid count=%d -------\n", statid_used);
    for(i=1; i<=statid_used; ++i)
    {
        rv = bcm_flexctr_stat_sync_get(unit,
                                  flexctr_cfg[i-1].flexctr_action_id, 1,
                                  &flexctr_cfg[i-1].counter_index,
                                  &counter_value);
        if(BCM_E_NONE!=rv) {
            printf("1Error in reading stat %d\n", rv);
            return;
        }
        print counter_value;
    }
    printf("--------------------------\n");
    return;
}

int DoUdfFpConfig()
{
    print bcm_udf_init(unit);
    print bcm_field_init(unit);
    const int offset = 2;           // L4 Dest Port starts after 2 Bytes from Base of L4
    const int width_bytes = 2;      // 2 Bytes of L4 Dest Port
    uint32 bmpChunk;
    int udf_id[10];
    int udf_id_used = 0;
    bcm_udf_multi_chunk_info_t chunk_info;
    
    if( 0 ) {
        bmpChunk = ChunksBitmapAllocate(0, width_bytes);
        if( bmpChunk == ChunkError )
        {
            printf("Not enough chunks remaining\n");
            return BCM_E_RESOURCE;
        }
    } else {
        if( 0 ) {
            bcm_switch_trace_event_mon_t monitor;
            monitor.trace_event = bcmPktTraceEventIngressVfp;
            monitor.actions = BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
            print bcm_switch_trace_event_mon_set(0, &monitor);

            monitor.trace_event = bcmPktTraceEventIngressIfp;
            monitor.actions = BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
            print bcm_switch_trace_event_mon_set(0, &monitor);
        }
        
        if( 1 ) {
            bcm_udf_multi_chunk_info_t_init(&chunk_info);
            chunk_info.offset = offset * 8;  // bits. Aiming for L4 DST Port (2B)
            chunk_info.abstract_pkt_format = bcmUdfAbstractPktFormatL4Tcp;
            BCM_UDF_CBMP_CHUNK_ADD(chunk_info.one_byte_chunk_bmap, 0);
            BCM_UDF_CBMP_CHUNK_ADD(chunk_info.one_byte_chunk_bmap, 1);
            printf("Doing multi chunk create %d for L4Tcp...\n", udf_id_used+1);
            BCM_IF_ERROR_RETURN( bcm_udf_multi_chunk_create(unit, NULL, &chunk_info,
                                                                        &udf_id[udf_id_used]) );
            udf_id_used++;
        }
        if( 1 ) {
            bcm_udf_multi_chunk_info_t_init(&chunk_info);
            chunk_info.offset = offset * 8;  // bits. Aiming for L4 DST Port (2B)
            chunk_info.abstract_pkt_format = bcmUdfAbstractPktFormatL4Udp;
            BCM_UDF_CBMP_CHUNK_ADD(chunk_info.one_byte_chunk_bmap, 0);
            BCM_UDF_CBMP_CHUNK_ADD(chunk_info.one_byte_chunk_bmap, 1);
            printf("Doing multi chunk create %d for L4Udp...\n", udf_id_used+1);
            BCM_IF_ERROR_RETURN( bcm_udf_multi_chunk_create(unit, NULL, &chunk_info,
                                                                        &udf_id[udf_id_used]) );
            udf_id_used++;
        }
    }
    
    if( 1 ) {
        uint8 udf_data[width_bytes];
        uint8 udf_mask[width_bytes] = {0xFF, 0xFF};
        int i;
        
        udf_data[0] = test_L4_dst_port & 0xFF;
        udf_data[1] = (test_L4_dst_port >> 8) & 0xFF;
        
        bcm_field_qset_t qset;
        BCM_FIELD_QSET_INIT(qset);

        printf("Doing qset multi set...\n");
        BCM_IF_ERROR_RETURN( bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 
                                          udf_id_used, &udf_id, &qset) );
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
        
        bcm_field_group_config_t group_config;
        bcm_field_group_config_t_init(group_config);
        group_config.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
        group_config.qset= qset;
        group_config.priority= 2;
        group_config.mode= bcmFieldGroupModeAuto;
        group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);
        printf("Doing group config create...\n");
        BCM_IF_ERROR_RETURN( bcm_field_group_config_create(unit, &group_config) );
        
        int entry;
        
        printf("Doing entry create...\n");
        BCM_IF_ERROR_RETURN( bcm_field_entry_create(unit, group_config.group, &entry) );
        printf("Doing qualify InPort...\n");
        BCM_IF_ERROR_RETURN( bcm_field_qualify_InPort(unit, entry, ing_port1, 0xFF) );
        for(i=0; i<udf_id_used; ++i) {
            printf("Doing qualify udf %d...\n", i);
            BCM_IF_ERROR_RETURN( bcm_field_qualify_udf(unit, entry, udf_id[i], width_bytes,
                                                                            udf_data, udf_mask) );
        }
        printf("Doing action add CopyToCpu...\n");
        BCM_IF_ERROR_RETURN( bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 1, 0xCC) );
        if( 1 )
        {
            uint32 ctr_action_id;
            printf("Doing flexctr create to group 0x%08X. StatID: %d..\n",
                                        group_config.group, statid_used+1);
            BCM_IF_ERROR_RETURN( flexctr_fp_stat_create(unit, 
                        group_config.group, bcmFieldStageIngress, &ctr_action_id) );
            flexctr_cfg[statid_used].flexctr_action_id = ctr_action_id;
            flexctr_cfg[statid_used].counter_index = statid_used;
            printf("Doing flexctr stat attach to entry 0x%08X...\n", entry);
            BCM_IF_ERROR_RETURN( bcm_field_entry_flexctr_attach(unit, entry,
                                            &flexctr_cfg[statid_used]) );
            statid_used++;
        }
        printf("Doing entry install...\n");
        BCM_IF_ERROR_RETURN( bcm_field_entry_install(unit, entry) );
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
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1);     // MTP port need not be part of the VLAN

        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    if( 0 )
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, test_smac_1, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = egr_port1;
        printf("Doing L2 entry add\n");
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( 0 )
    {
        rv = ingress_port_setup(unit, ing_port1, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", ing_port1, rv);
            return rv;
        }
        
        rv = egress_port_setup(unit, egr_port1, loopBackType);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", egr_port1, rv);
            return rv;
        }
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, loopBackType));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port1, loopBackType));
        //BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port1, BCM_PORT_DISCARD_ALL));
        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, egr_port1, bcmPortControlTrustIncomingVlan, 0));
    }
    
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, loopBackType));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, ing_port2, bcmPortControlTrustIncomingVlan, 0));

    //bshell(unit, "pw start report +raw +decode dma");
    bshell(unit, "pw start report +all");

    return BCM_E_NONE;
}


bcm_error_t
verify()
{
    char cmd[2048];
    int prio = 0;
    
    if( 1 )
    {
        printf("##########  Sending IPv4 UDP packet ########\n");
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500LLLLAB494000FF11SSSS%08X%08X%04X%04X0012722868656C6C6F68656C6C6F112233445566778899",
            ing_port1,
            test_dmac[0], test_dmac[1], test_dmac[2],
            test_dmac[3], test_dmac[4], test_dmac[5],
            test_smac[0], test_smac[1], test_smac[2],
            test_smac[3], test_smac[4], test_smac[5],
            FORM_VID(prio, 0, test_vid), test_sip, test_dip,
            test_L4_src_port,
            test_L4_dst_port );
        //printf("BEFORE: %s\n", cmd);
        /* For single tag IP packet; 18th byte starts at ->4500... Byte in hex; not ascii. */
        ip4_sum_n_size_update(cmd, 18, 0);
        //ip4_sum_n_size_update(cmd, 18+20, 0); // for inner ipv4 in Ip-in-IP packet with single tag
        //printf("AFTER:  %s\n", cmd);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        PrintFpStat(unit);
        printf("#####################################################\n\n");
    }
    
    if( 1 )
    {
        printf("##########  Sending IPv4 TCP packet ########\n");
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X08004500LLLL08B84000FF06SSSS%08X%08X%04X%04X7214F1140000000060022238A92C0000020405B4",
            ing_port1,
            test_dmac[0], test_dmac[1], test_dmac[2],
            test_dmac[3], test_dmac[4], test_dmac[5],
            test_smac[0], test_smac[1], test_smac[2],
            test_smac[3], test_smac[4], test_smac[5],
            FORM_VID(prio, 0, test_vid), test_sip, test_dip,
            test_L4_src_port,
            test_L4_dst_port );
        //printf("BEFORE: %s\n", cmd);
        /* For single tag IP packet; 18th byte starts at ->4500... Byte in hex; not ascii. */
        ip4_sum_n_size_update(cmd, 18, 0);
        //ip4_sum_n_size_update(cmd, 18+20, 0); // for inner ipv4 in Ip-in-IP packet with single tag
        //printf("AFTER:  %s\n", cmd);
        bbshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 1);
        bbshell(unit, cmd);
        PrintFpStat(unit);
        printf("#####################################################\n\n");
    }
    
    return BCM_E_NONE;
}


bcm_error_t
execute()
{
    int rrv;
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = DoUdfFpConfig()) != BCM_E_NONE )
    {
        printf("Configuring UDF and FP failed with %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = verify()) != BCM_E_NONE )
    {
        printf("Verify mirror failed with %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


