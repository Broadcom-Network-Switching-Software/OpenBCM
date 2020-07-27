/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : IFP Flex stat and policer
 *
 *  Usage    : BCM.0> cint ifp_flex_stat_policer.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : ifp_flex_stat_policer_log.txt
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
 *  +----------------+                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *  Summary:
 *  ========
 *      IFP flex stat and policer demonstration using BCM APIs.
 *      From TH2 not much has changed in this area.
 *
 *  Detailed steps done in the CINT script:
 *  ====================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ================================
 *       a) Select one ingress and three egress ports and configure them in
 *          Loopback mode. Install a rule to copy incoming packets to CPU and
 *          additional action to drop the packets when it loops back on egress
 *          ports. Start packet watcher.
 *
 *    2) Step2 - Configuration (Done in fp_flex_stat_configure()).
 *    ===================================================
 *       a) Create entry
 *             QUAL    : InPort and DstMac
 *             ACTION  : None
 *             POLICER : TrTcm with cir+cbs and eir+ebs
 *             STAT    : Counter for
 *                             GREEN
 *                             YELLOW if opt_IncludeYellowStat = 1
 *                             RED    if opt_IncludeRedStat = 1
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Goal is to see
 *             a) if green counter is incrementing
 *             b) if yellow counter is incrementing
 *             c) if red counter is incrementing
 *       b) Send traffic in such a way that packets are collored according to
 *          their ingress rate.
 *       c) Send   1 packet in a burst from CPU to get GREEN only hit
 *       d) Send 500 packets in a burst from CPU to get GREEN+YELLOW hit
 *          To some extent Red also hits
 *       e) Send 100000 packets in a burst from CPU to get GREEN+YELLOW+RED hit
 *       f) Call fp_flex_stat_get() to get the stat values.
 *       g) To get a better result, you can integrate the sample code snippet
 *          pasted at the bottom of this file.
 *       h) Expected Result:
 *       ================
 *       You will see that Green, Yellow and RED STAT counter should increment
 *
 * NOTE: Use opt_* variables to change the test variant
 */

cint_reset();

/****************   GENERIC UTILITY FUNCTIONS STARTS  **************************
 */
// Gets the next valid port after the last valid returned port.            /* */
bcm_port_t PORT_ANY = -1;                                                  /* */
bcm_port_t __last_returned_port = 1;                                       /* */
// Get next valid Ethernet port                                            /* */
bcm_port_t                                                                 /* */
portGetNextE(int unit, bcm_port_t PreferredPort)                           /* */
{                                                                          /* */
  int i=0;                                                                 /* */
  int rv=0;                                                                /* */
  bcm_port_config_t configP;                                               /* */
  bcm_pbmp_t ports_pbmp;                                                   /* */
  if( PreferredPort != PORT_ANY )                                          /* */
  {                                                                        /* */
    printf("Using preferred port %d\n", PreferredPort);                    /* */
    return PreferredPort;                                                  /* */
  }                                                                        /* */
  rv = bcm_port_config_get(unit, &configP);                                /* */
  if(BCM_FAILURE(rv)) {                                                    /* */
    printf("\nError in retrieving port configuration: %s %d.\n",           /* */
    bcm_errmsg(rv), __last_returned_port);                                 /* */
    exit;                                                                  /* */
  }                                                                        /* */
  ports_pbmp = configP.e;    // configP.hg;                                /* */
  for (i= __last_returned_port; i < BCM_PBMP_PORT_MAX; i++)                /* */
  {                                                                        /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp, i) )                                 /* */
    {                                                                      /* */
        __last_returned_port = i+1; // point to a probable next port       /* */
        return i;                                                          /* */
    }                                                                      /* */
  }                                                                        /* */
  printf("ERROR: Not enough ports %d\n", __last_returned_port);            /* */
  exit;                                                                    /* */
}                                                                          /* */
//                                                                         /* */
// Configures the port in loopback mode and installs                       /* */
// an IFP rule. This IFP rule copies the packets ingressing                /* */
// on the specified port to CPU.                                           /* */
//                                                                         /* */
bcm_error_t                                                                /* */
ingress_port_setup(int unit, bcm_port_t port, int LoopBackType)            /* */
{                                                                          /* */
  bcm_field_group_config_t group_config;                                   /* */
  bcm_field_aset_t    actn;                                                /* */
  bcm_field_group_t group = 9998;                                          /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99981;                                         /* */
  int         fp_statid = 9998;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
                            BCM_PORT_DISCARD_NONE));                       /* */
  bcm_field_group_config_t_init(&group_config);                            /* */
  BCM_FIELD_ASET_INIT(actn);                                               /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);                       /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);                       /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.aset= actn;                                                 /* */
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
            bcmFieldActionCopyToCpu, 1, 222));                             /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,             /* */
            fp_statid));                                                   /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
//                                                                         /* */
// Configures the port in loopback mode and installs                       /* */
// an IFP rule. This IFP rule copies the packets ingressing                /* */
// on the specified port to CPU. Port is also configured                   /* */
// to discard all packets. This is to avoid continuous                     /* */
// loopback of the packet.                                                 /* */
//                                                                         /* */
bcm_error_t                                                                /* */
egress_port_setup(int unit, bcm_port_t port, int LoopBackType)             /* */
{                                                                          /* */
  bcm_field_group_t group = 9999;                                          /* */
  bcm_field_group_config_t group_config;                                   /* */
  bcm_field_aset_t    actn;                                                /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99991;                                         /* */
  int         fp_statid = 9999;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
            BCM_PORT_DISCARD_ALL));                                        /* */
  bcm_field_group_config_t_init(&group_config);                            /* */
  BCM_FIELD_ASET_INIT(actn);                                               /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);                       /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);                       /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.aset= actn;                                                 /* */
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
            bcmFieldActionCopyToCpu, 1, 223));                             /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid));/* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
bcm_error_t                                                                /* */
egress_port_multi_setup(int unit, bcm_pbmp_t pbm, int LoopBackType)        /* */
{                                                                          /* */
  bcm_field_group_t group = 10000;                                         /* */
  bcm_field_group_config_t group_config;                                   /* */
  bcm_field_aset_t    actn;                                                /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 100001;                                        /* */
  int         fp_statid = 10000;                                           /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
  bcm_port_t     port;                                                     /* */
  BCM_PBMP_ITER(pbm, port)                                                 /* */
  {                                                                        /* */
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
  BCM_FIELD_ASET_INIT(actn);                                               /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionCopyToCpu);                       /* */
  BCM_FIELD_ASET_ADD(actn, bcmFieldActionStatGroup);                       /* */
  group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ID;                    /* */
  group_config.aset= actn;                                                 /* */
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
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

int opt_IncludeYellowStat = 1;
int opt_IncludeRedStat = 1;


int unit=0;

bcm_vlan_t test_vid = 100;

bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;
bcm_port_t egr_port = PORT_ANY;

/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

egr_port = portGetNextE(unit, egr_port);

print ing_port1;
print ing_port2;
print egr_port;


/* Default Configuration */
struct fp_cfg_t{
    bcm_mac_t dest_mac;               /* Destination Mac Address to qualify */
    bcm_mac_t dest_mask;              /* Destination Mac Mask */
    bcm_port_t input_port;            /* Inport */
    bcm_port_t input_port_mask;       /* InPort Mask */
};

/* Set the reguired Configuration here */
fp_cfg_t fp_cfg = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD}, /* Destination Mac Address */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, /* Destination Mac Mask */
    ing_port1,                            /* Inport  */
    0xFF,                                 /* InPort Mask */
};

/*
This function creates group and entry. Also attached TrTcm policer
*/
int
configure_ifp_with_policer (int unit, fp_cfg_t *ifp_cfg, bcm_field_group_config_t *group_config,
               bcm_field_entry_t entry)
{
    /* FP Group Configuration and Creation */
    bcm_field_group_config_t_init(group_config);

    BCM_FIELD_QSET_INIT(group_config->qset);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(group_config->qset, bcmFieldQualifyInPort);
    
    BCM_FIELD_ASET_INIT(group_config->aset);
    BCM_FIELD_ASET_ADD(group_config->aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config->aset, bcmFieldActionStatGroup);

    /*
     * Different group modes [Single, Double, Triple or Quad] are possible and
     * availability depends on the chip being used. bcmFieldGroupModeAuto is the
     * default mode and it expands based on the given Qset
     */

    group_config->mode = bcmFieldGroupModeAuto;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, group_config));

    /* FP Entry Configuration and Creation */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group_config->group,
                                               entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstMac(unit, entry, ifp_cfg->dest_mac,
                                                 ifp_cfg->dest_mask));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry,
                                                 ifp_cfg->input_port,
                                                 ifp_cfg->input_port_mask));

    if( 1 )
    {
        bcm_policer_t             policerId;
        bcm_policer_config_t     pol_cfg;
        bcm_error_t rrc;
        
        bcm_policer_config_t_init(&pol_cfg);

        pol_cfg.ckbits_sec = 20*8;
        pol_cfg.ckbits_burst = 0;
        pol_cfg.pkbits_sec = 50*8;
        pol_cfg.pkbits_burst = 0;
        pol_cfg.mode = bcmPolicerModeTrTcm;
        //pol_cfg.flags |= BCM_POLICER_MODE_PACKETS;
        pol_cfg.flags |= BCM_POLICER_MODE_BYTES;
        
        pol_cfg.action_id = 0;
        
        rrc = bcm_policer_create(unit, &pol_cfg, &policerId);
        if (rrc != BCM_E_NONE)
        {
            printf("Failed to create policer for unit: %d, entry %d Error:%s (%d)\r\n",
                unit, entry,  bcm_errmsg (rrc), rrc);
            return rrc;
        }
        
        rrc = bcm_field_entry_policer_attach(unit, entry, 0, policerId);
        if (rrc != BCM_E_NONE)
        {
            printf("Failed to attach policer for unit: %d, entry %d Error:%s (%d)\r\n",
                unit, entry,  bcm_errmsg (rrc), rrc);
            return rrc;
        }
    }
    
    /* Installing FP Entry to FP TCAM */
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
    This function attaches flex stat for Green, Yellow and Red packets.
    Then re-installs the entry.
*/
int
configure_flex_stats(int unit, bcm_field_group_t group, bcm_field_entry_t entry,
                     uint32 stat_id)
{
    int num_sel = 0;
    bcm_stat_group_mode_attr_selector_t attr_sel[3];
    uint32 flex_stat_id, num_entries;
    uint32 mode_id;
    bcm_stat_group_mode_hint_type_t stat_hint;
    bcm_stat_group_mode_id_config_t stat_config;
    bcm_error_t rv;

    if( 1 )
    {
        bcm_stat_group_mode_attr_selector_t_init(&attr_sel[num_sel]);
        attr_sel[num_sel].counter_offset = num_sel;
        attr_sel[num_sel].attr = bcmStatGroupModeAttrFieldIngressColor;
        attr_sel[num_sel].attr_value = bcmColorGreen;
        num_sel++;
    }

    if( opt_IncludeYellowStat )
    {
        bcm_stat_group_mode_attr_selector_t_init(&attr_sel[num_sel]);
        attr_sel[num_sel].counter_offset = num_sel;
        attr_sel[num_sel].attr = bcmStatGroupModeAttrFieldIngressColor;
        attr_sel[num_sel].attr_value = bcmColorYellow;
        num_sel++;
    }
    
    if( opt_IncludeYellowStat )
    {
        bcm_stat_group_mode_attr_selector_t_init(&attr_sel[num_sel]);
        attr_sel[num_sel].counter_offset = num_sel;
        attr_sel[num_sel].attr = bcmStatGroupModeAttrFieldIngressColor;
        attr_sel[num_sel].attr_value = bcmColorRed;
        num_sel++;
    }
    
    bcm_stat_group_mode_id_config_t_init(&stat_config);
    stat_config.flags = BCM_STAT_GROUP_MODE_INGRESS;
    stat_config.total_counters = num_sel;
    stat_config.hint.type = bcmStatGroupAllocHintIngressFieldGroup;
    stat_config.hint.value = group;

    /* Create Customized Stat Group mode for given Counter Attributes */
    rv = bcm_stat_group_mode_id_config_create(unit, 0, &stat_config, num_sel,
                                              attr_sel, &mode_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in creating stat group mode: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create Custom Stat Group */
    rv = bcm_stat_custom_group_create(unit, mode_id, bcmStatObjectIngFieldStageIngress,
                                      &flex_stat_id, &num_entries);
    if(BCM_FAILURE(rv)) {
        printf("\nError in creating stat group: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Use the bcm_field_* APIs for attaching and retrieving the stats */
    rv = bcm_field_stat_attach(unit, group, flex_stat_id, &stat_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in attaching stat to group : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_stat_attach(unit, entry, stat_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in attaching stat to entry : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_install(unit, entry);
    if(BCM_FAILURE(rv)) {
        printf("\nError in entry install : %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
    This function reads the stat values and then prints them.
*/
int
fp_flex_stat_get()
{
    int unit = 0;
    int i;
    uint32 stat_id = 1;
    bcm_field_stat_t stat[6] = {bcmFieldStatGreenPackets, bcmFieldStatGreenBytes,
                                bcmFieldStatYellowPackets, bcmFieldStatYellowBytes,
                                bcmFieldStatRedPackets, bcmFieldStatRedBytes};
    uint64 val;

    /*
     * COMPILER_64_HI and COMPILER_64_LO for 64-32 bit conversion
     */

    /* FP stat collection */
    for(i=0; i<6; i++) {
        BCM_IF_ERROR_RETURN(bcm_field_stat_get(unit, stat_id, stat[i], &val));

        switch(stat[i]) {
            case bcmFieldStatGreenPackets :
                    printf("The value is 0x%x%x green packets\n", COMPILER_64_HI(val),
                             COMPILER_64_LO(val));
                    break;

            case bcmFieldStatGreenBytes :
                    printf("The value is 0x%x%x green bytes\n", COMPILER_64_HI(val),
                            COMPILER_64_LO(val));
                    break;

            case bcmFieldStatYellowPackets :
                    printf("The value is 0x%x%x yellow packets\n", COMPILER_64_HI(val),
                            COMPILER_64_LO(val));
                    break;

            case bcmFieldStatYellowBytes :
                    printf("The value is 0x%x%x yellow bytes\n", COMPILER_64_HI(val),
                            COMPILER_64_LO(val));
                    break;
                    
            case bcmFieldStatRedPackets :
                    printf("The value is 0x%x%x red packets\n", COMPILER_64_HI(val),
                            COMPILER_64_LO(val));
                    break;

            case bcmFieldStatRedBytes :
                    printf("The value is 0x%x%x red bytes\n", COMPILER_64_HI(val),
                            COMPILER_64_LO(val));
                    break;
            }
    }

    return BCM_E_NONE;
}

/*
    The main test function.
*/
int fp_flex_stat_configure()
{
    int unit = 0;
    bcm_field_group_config_t fp_group_config;
    bcm_field_entry_t field_entry = 1;
    uint32 stat_id = 1;
    bcm_error_t rv;
    
    if( 1 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port);

        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    
    if( 1 )
    {
        bcm_l2_addr_t l2_addr;
        
        dmac[0] = fp_cfg.dest_mac[0];
        dmac[1] = fp_cfg.dest_mac[1];
        dmac[2] = fp_cfg.dest_mac[2];
        dmac[3] = fp_cfg.dest_mac[3];
        dmac[4] = fp_cfg.dest_mac[4];
        dmac[5] = fp_cfg.dest_mac[5];
        
        bcm_l2_addr_t_init(&l2_addr, dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    rv = configure_ifp_with_policer(unit, &fp_cfg, &fp_group_config, field_entry);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring fp: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = configure_flex_stats(unit, fp_group_config.group, field_entry, stat_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring flex stats: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}



bcm_mac_t dmac;
/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    if (BCM_E_NONE != ingress_port_setup(unit, ing_port1, BCM_PORT_LOOPBACK_MAC)) {
        printf("ingress_port_setup() failed for port %d\n", ing_port1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egr_port, BCM_PORT_LOOPBACK_MAC)) {
        printf("egress_port_setup() failed for port %d\n", egr_port);
        return -1;
    }

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));

    //bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}


/*
 How to verify: It is documented within the function for various packets and options
*/
bcm_error_t verify()
{
    char cmd[1024*2];
    printf("\n\n");
    bbshell(unit, "fp show");
    printf("\n\n");
    int num_pkt_in_a_burst = 0;
    int i;
    
    /////////////////////////////////////////////////////////////////////////////
    for(i=0; i<2; ++i)
    {
        num_pkt_in_a_burst = 1;
        printf("############\n");
        printf("Sending %d packets so that GREEN only hits\n", num_pkt_in_a_burst);
        sprintf(cmd, "tx %d pbm=%d data=0000000000DD0000000000CC8100006408004500002E0000000040063E8C141414160A0A0A0B227D00500000000000000000500000004ACA0000000102030405B0F9EB4F", num_pkt_in_a_burst, ing_port1);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);

        fp_flex_stat_get();
        
        if( i == 0 )    // Illustration of clearing of counters
        {
            uint64 val;
            COMPILER_64_SET(val, 0, 0);
            print bcm_field_stat_all_set(unit, 1 /* stat id */, val);
        }
    }
    /////////////////////////////////////////////////////////////////////////////
    // Sending packet with Non_Matching TTL value
    if( 1 )
    {
        num_pkt_in_a_burst = 500;
        printf("\n\n\n############\n");
        printf("Sending %d packets so that GREEN+YELLOW+RED hits\n", num_pkt_in_a_burst);
        sprintf(cmd, "tx %d pbm=%d data=0000000000DD0000000000CC8100006408004500002E0000000040063E8C141414160A0A0A0B227D00500000000000000000500000004ACA0000000102030405B0F9EB4F", num_pkt_in_a_burst, ing_port1);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);
    }
    fp_flex_stat_get();
    /////////////////////////////////////////////////////////////////////////////
    // Sending packet with Non_Matching TTL value
    if( 1 )
    {
        num_pkt_in_a_burst = 100000;
        printf("\n\n\n############\n");
        printf("Sending %d packets so that more of RED hits\n", num_pkt_in_a_burst);
        sprintf(cmd, "tx %d pbm=%d data=0000000000DD0000000000CC8100006408004500002E0000000040063E8C141414160A0A0A0B227D00500000000000000000500000004ACA0000000102030405B0F9EB4F", num_pkt_in_a_burst, ing_port1);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);
    }
    fp_flex_stat_get();
    /////////////////////////////////////////////////////////////////////////////
    printf("\n\nFor more accurate and controlled packet tx from CPU, you have to modify the SDK code.\n");
    printf("Code with which to modify is placed at the bottom of this CINT.\n");
    printf("Steps:\n");
    printf("\t 1. Modify the SDK code and compile and then load the new bcm.user.\n");
    printf("\t 2. Then execute this CINT which will run automated coarse tests.\n");
    printf("\t 3. Then you can run the hacked memwatch command as described in the CINT code snippet.\n");
    
    return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rrv;
    
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = fp_flex_stat_configure()) != BCM_E_NONE )
    {
        printf("Configuring compression IFP failed with %d\n", rrv);
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
/*

// FILE to modify:
// FILE: src/appl/diag/esw/mem.c

// Command hijacked is 'memwatch'
// New usage: memwatch <port_number> <usec interval after which 1 packet will be sent>
//                 port_number is only considered for 1st invocation of the command.

// Test commands to run: (pol_cfg.ckbits_sec = 20*8; 94 bytes per packet)
// For GREEN PACKET
// memwatch ing_port1 5000

// For YELLOW+GREEN PACKET: (pol_cfg.pkbits_sec = 50*8; 94 bytes per packet)
// memwatch ing_port1 2000

// For YELLOW+GREEN+RED PACKET:
// memwatch ing_port1 500

////////////////////////////////////////////////////////////

#include "bcm/pkt.h"

volatile static int txport = 4;
volatile static int sleep_duration_usec = 1000;

void tx_mpls_packet_from_cpu(void * p_unit)
{
    bcm_mac_t mac_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};
    bcm_mac_t mac_da = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
    char pkt_vlan_lengthtype[6] = {0x81,0x00,0x00,0x0c,0x88,0x47};
    char pkt_mpls_hdr[8] = {0x01, 0x00, 0x00, 0x40, 0x05, 0x00, 0x01, 0x40};
    bcm_mac_t payload_mac_sa = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCD};
    bcm_mac_t payload_mac_da = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDE};
    bcm_pkt_t *pkt;
    int pkt_size = 94; // including Tag if BCM_PKT_F_NO_VTAG = 0 
    int flags = BCM_TX_CRC_APPEND; 
    int unit = (int)p_unit;
    unit = 0;

    if (BCM_E_NONE != bcm_pkt_alloc(unit, pkt_size, flags, &pkt)) {
        printf("TX: Failed to allocate packets\n");
    }
                
    sal_memset(pkt->_pkt_data.data, 0x00, pkt_size);
    sal_memcpy(pkt->pkt_data->data, mac_da, 6);
    sal_memcpy(pkt->pkt_data->data+6, mac_sa, 6);
    sal_memcpy(pkt->pkt_data->data+12, pkt_vlan_lengthtype, 6);
    sal_memcpy(pkt->pkt_data->data+18, pkt_mpls_hdr, 8);
    sal_memcpy(pkt->pkt_data->data+26, payload_mac_da, 6);
    sal_memcpy(pkt->pkt_data->data+32, payload_mac_sa, 6);
    pkt->call_back = NULL;
    BCM_PKT_PORT_SET(pkt, txport, 0, 0);

    printf("Sending packet to port %d\n", txport);

    while( 1 )
    {
        if (BCM_E_NONE != bcm_tx(unit, pkt, NULL)) {
            printf("bcm tx error\n");
        }
        sal_usleep(sleep_duration_usec);
    }
}

static int t_started = 0;

// NOTE: Rename Existing mem_watch() function to mem_watch1() to avoid linker error.

cmd_result_t
mem_watch(int unit, args_t *a)
{
    char            *options;

    options = ARG_GET(a);
    if (options != NULL) {
        txport = sal_atoi(options);
        printf("Setting port number %d\n", txport);
    }
    options = ARG_GET(a);
    if (options != NULL) {
        sleep_duration_usec = sal_atoi(options);
        printf("Sleep duration %d\n", sleep_duration_usec);
    }
    
    if( !t_started )
    {
        printf("Starting the thread...\n");
        sal_thread_create("BCMTX-TST",
                         16384,
                         100,
                         tx_mpls_packet_from_cpu, NULL);
        t_started = 1;
    }
    else
    {
        printf("Thread already started.\n");
    }
    
    return CMD_OK;
}
////////////////////////////////////////////////////////////

*/
