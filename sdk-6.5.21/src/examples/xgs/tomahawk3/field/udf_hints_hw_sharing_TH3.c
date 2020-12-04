/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : UDF Hints in IFP with HW Sharing
 *
 *  Usage    : BCM.0> cint udf_hints_hw_sharing_TH3.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : udf_hints_hw_sharing_TH3_log.txt
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
 *  Summary:
 *  ========
 *    Opaque TAG usage demonstration using BCM APIs.
 *    TH3 does not support double tagged packets. If a double tagged
 *    packets enters the pipeline it treats everything beyong 1st tag
 *    as data. E.g. a double tagged L3 packet will not be recognised as
 *    an IP packet. This CINT excercises the processing of opaque tag in
 *    TH3. To do this we are sending double tag packet to qualify on
 *    fields beyond the double tag field. You have to use Opaque TAG
 *    to successfully parse the fields beyond 2nd tag.
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
 *    2) Step2 - Configuration (Done in ConfigureUdfAndFp()).
 *    ===================================================
 *       a) Create L3 interface MAC=0xDD, VID=100
 *       b) Create L3 egress object DMAC=0xEE, VID=100, Interface as above.
 *       c) Configure MPLS POP action for label=1000. After POP,
 *                      applicable egress object as above.
 *       d) Create 1st UDF extraction for 1 label MPLS packets
 *                  OFFSET to fetch TTL from mpls label
 *       e) Create 2nd UDF extraction for 2 labels MPLS packets
 *                  OFFSET to fetch TTL from INNER mpls label (HW SHARED)
 *       f) Create IFP Entry
 *             QUAL    : UDF1 and UDF2
 *             ACTION  : None
 *             STAT    : Counter with STAT ID = 1
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send PACKET1, STAT UP (See below)
 *       b) Send PACKET2, STAT SAME
 *       c) Send PACKET3, STAT UP
 *       d) Send PACKET4, STAT SAME
 *       e) Expected Result:
 *          ================
 *          The final STAT count should be 2
 *
 *    PACKET1:
 *          MACDA: 0xDD, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 39(h27)
 *    PACKET2:
 *          MACDA: 0xDD, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 33(h21)
 *    PACKET3:
 *          MACDA: 0xDD, VID: 100, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21) 
 *                                                 2000(h7D0), 6(h6), 39(h27)
 *    PACKET4:
 *          MACDA: 0xDD, VID: 100, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21)
 *                                                 2000(h7D0), 6(h6), 33(h21)
 * NOTE: Use opt_* variables to change the test variant
 */

cint_reset();

/****************   GENERIC UTILITY FUNCTIONS STARTS  **************************
 */
                                                                           /* */
// Gets the next valid port after the last valid returned port.            /* */
                                                                           /* */
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
                                                                           /* */
  if( PreferredPort != PORT_ANY )                                          /* */
  {                                                                        /* */
    printf("Using preferred port %d\n", PreferredPort);                    /* */
    return PreferredPort;                                                  /* */
  }                                                                        /* */
                                                                           /* */
  rv = bcm_port_config_get(unit, &configP);                                /* */
  if(BCM_FAILURE(rv)) {                                                    /* */
    printf("\nError in retrieving port configuration: %s %d.\n",           /* */
    bcm_errmsg(rv), __last_returned_port);                                 /* */
    exit;                                                                  /* */
  }                                                                        /* */
                                                                           /* */
  ports_pbmp = configP.e;    // configP.hg;                                /* */
  for (i= __last_returned_port; i < BCM_PBMP_PORT_MAX; i++)                /* */
  {                                                                        /* */
    if ( BCM_PBMP_MEMBER(&ports_pbmp, i) )                                 /* */
    {                                                                      /* */
        __last_returned_port = i+1; // point to a probable next port       /* */
        return i;                                                          /* */
    }                                                                      /* */
  }                                                                        /* */
                                                                           /* */
  printf("ERROR: Not enough ports %d\n", __last_returned_port);            /* */
  exit;                                                                    /* */
}                                                                          /* */
                                                                           /* */
//                                                                         /* */
// Configures the port in loopback mode and installs                       /* */
// an IFP rule. This IFP rule copies the packets ingressing                /* */
// on the specified port to CPU.                                           /* */
//                                                                         /* */
bcm_error_t                                                                /* */
ingress_port_setup(int unit, bcm_port_t port, int LoopBackType)            /* */
{                                                                          /* */
  bcm_field_qset_t  qset;                                                  /* */
  bcm_field_group_t group = 9998;                                          /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99981;                                         /* */
  int         fp_statid = 9998;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
                            BCM_PORT_DISCARD_NONE));                       /* */
                                                                           /* */
  BCM_FIELD_QSET_INIT(qset);                                               /* */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);                         /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, qset, pri, group));  /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /* */
            BCM_FIELD_EXACT_MATCH_MASK));                                  /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
            bcmFieldActionCopyToCpu, 1, 222));                             /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,             /* */
            fp_statid));                                                   /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
                                                                           /* */
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
  bcm_field_qset_t  qset;                                                  /* */
  bcm_field_group_t group = 9999;                                          /* */
  int                pri = group;                                          /* */
  bcm_field_entry_t entry = 99991;                                         /* */
  int         fp_statid = 9999;                                            /* */
  const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,                 /* */
                                      bcmFieldStatBytes };                 /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, LoopBackType));    /* */
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,                     /* */
            BCM_PORT_DISCARD_ALL));                                        /* */
                                                                           /* */
  BCM_FIELD_QSET_INIT(qset);                                               /* */
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);                         /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, qset, pri, group));  /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));      /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,          /* */
            BCM_FIELD_EXACT_MATCH_MASK));                                  /* */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                    /* */
            bcmFieldActionCopyToCpu, 1, 223));                             /* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,             /* */
            stats, fp_statid));                                            /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, fp_statid));/* */
                                                                           /* */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));               /* */
                                                                           /* */
  return BCM_E_NONE;                                                       /* */
}                                                                          /* */
                                                                           /* */
bcm_error_t                                                                /* */
egress_port_multi_setup(int unit, bcm_pbmp_t pbm, int LoopBackType)        /* */
{                                                                          /* */
    bcm_field_qset_t  qset;                                                /* */
    bcm_field_group_t group = 9999;                                        /* */
    int                pri = group;                                        /* */
    bcm_field_entry_t entry = 99991;                                       /* */
    int         fp_statid = 9999;                                          /* */
    const bcm_field_stat_t stats[2] = { bcmFieldStatPackets,               /* */
                                        bcmFieldStatBytes };               /* */
    bcm_port_t     port;                                                   /* */
                                                                           /* */
                                                                           /* */
    BCM_PBMP_ITER(pbm, port)                                               /* */
    {                                                                      /* */
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,              /* */
                    LoopBackType));                                        /* */
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port,               /* */
                    BCM_PORT_DISCARD_ALL));                                /* */
    }                                                                      /* */
    print pbm;                                                             /* */
    bcm_pbmp_t pbm_mask;                                                   /* */
    BCM_PBMP_CLEAR(pbm_mask);                                              /* */
    BCM_PBMP_NEGATE(pbm_mask, pbm_mask);                                   /* */
    BCM_FIELD_QSET_INIT(qset);                                             /* */
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);                      /* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, qset, pri, group));/* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, group, entry));    /* */
                                                                           /* */
    // Due to a bug SDK-144931 related to InPorts, commenting              /* */
    // out below line.                                                     /* */
    //BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, entry,           /* */
    //            pbm, pbm_mask));                                         /* */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,                  /* */
                bcmFieldActionCopyToCpu, 1, 224));                         /* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2,           /* */
                stats, fp_statid));                                        /* */
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry,           /* */
                fp_statid));                                               /* */
                                                                           /* */
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));             /* */
                                                                           /* */
    return BCM_E_NONE;                                                     /* */
}                                                                          /* */
                                                                           /* */
void bbshell(int unit, char *str)                                          /* */
{                                                                          /* */
    printf("BCM.%d> %s\n", unit, str);                                     /* */
    bshell(unit, str);                                                     /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

int unit=0;

bcm_vlan_t test_vid = 100;

bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;
bcm_port_t egr_port = PORT_ANY;

/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

egr_port = portGetNextE(unit, egr_port);

//bcm_mac_t test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};

bcm_mac_t local_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xEE};

bcm_ip_t ip_subnet=0x0A0A0A0B;    // dest IP
bcm_ip_t ip_subnet_mask = 0xffffff00;

uint32 rx_label = 1000;
uint32 rx_label_two = 2000;

const int   data_width = 1;
const uint8 ttl_match_data[data_width] = { 0x27 };     // d39 TTL
const uint8 ttl_match_mask[data_width] = { 0xFF };
const uint8 ttl_unmatch = 0x21;

int main_ifp_statid = 1;

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

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*
 *  Creates L3 interface and object. Creates corresponding route entry.
 *  Creates MPLS tunnel termination with POP action
 */
bcm_error_t mpls_action()
{
    bcm_port_t port_1 = egr_port; 
    bcm_gport_t gport_1;

    bcm_vlan_t vid_1 = test_vid;
    bcm_pbmp_t pbmp, ubmp;

    bcm_l3_intf_t l3_intf_1;
    bcm_if_t intf_id_1 = 1;
    bcm_vrf_t vrf = 0; 
    bcm_mpls_tunnel_switch_t tun_switch;

    bcm_l3_route_t l3_route;

    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj_1;
    bcm_module_t modid_1;
    bcm_port_t modport_1;

    printf("Doing sc egress mode\n");
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    
    printf("Doing gport get\n");
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_1, &gport_1));
    
    
    if( 1 )
    {
        modid_1 = BCM_GPORT_MODPORT_MODID_GET(gport_1);
        modport_1 = BCM_GPORT_MODPORT_PORT_GET(gport_1);

        // l3 intf for customer port
        printf("Doing l3 intf create\n");
        bcm_l3_intf_t_init(&l3_intf_1);
        l3_intf_1.l3a_flags  = BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID;
        l3_intf_1.l3a_intf_id = intf_id_1;
        sal_memcpy(l3_intf_1.l3a_mac_addr, local_mac_1, 6);
        l3_intf_1.l3a_vid = vid_1;
        l3_intf_1.l3a_vrf = vrf;
        BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf_1));

        /* MPLS termination flow */
        printf("Doing l3 egress object\n");
        bcm_l3_egress_t_init(&l3_egress);
        l3_egress.intf = intf_id_1; 
        sal_memcpy(l3_egress.mac_addr, remote_mac_1, 6);
        l3_egress.vlan = vid_1;
        l3_egress.module = modid_1;
        l3_egress.port = modport_1;
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_1));

        printf("Doing l3 route add\n");
        bcm_l3_route_t_init(&l3_route);
        l3_route.l3a_subnet = ip_subnet;
        l3_route.l3a_ip_mask = ip_subnet_mask;
        l3_route.l3a_intf = egr_obj_1;
        BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, l3_route));
    }

    
    bcm_mpls_tunnel_switch_t_init(&tun_switch);
    tun_switch.flags = BCM_MPLS_SWITCH_COUNTED;//BCM_MPLS_SWITCH_COUNTED;
    //tun_switch.flags |= BCM_MPLS_SWITCH_DIRECT_ATTACHED;
    tun_switch.label = rx_label;
    
    tun_switch.port = BCM_GPORT_INVALID;
    
    if( 1 ) // POP
    {
        tun_switch.action = BCM_MPLS_SWITCH_ACTION_POP;
        tun_switch.egress_if = egr_obj_1;
    }
    else if( 0 ) // SWAP
    {
        tun_switch.action = BCM_MPLS_SWITCH_ACTION_SWAP;
        tun_switch.egress_label.label = rx_label+1; //1000;    // new label
        tun_switch.egress_if = egr_obj_1;
    }
    else // NOP
    {
        tun_switch.action = BCM_MPLS_SWITCH_ACTION_NOP;
        tun_switch.egress_port = gport_1;
        tun_switch.egress_if = egr_obj_1;
    }
    
    printf("Doing Add mpls tunnel switch\n");
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tun_switch));
    
    printf("Doing L2 tunnel add\n");
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, local_mac_1, vid_1));
    
    return BCM_E_NONE;
}

/*
 *  Calls MPLS related CINT function
 *  Creates VLAN and adds ports
 *  Creates UDF Extraction for inner most TTL for 2 packet formats: ONE LABEL and TWO LABELs of MPLS tunnelled packets
 *  Creates IFP entry to match on onner most TTL value == 0x27
 *  Action is nothing but STAT increses.
 */
bcm_error_t ConfigureUdfAndFp()
{
    bcm_udf_pkt_format_id_t     pkt_format_id;
    
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
    
    if( 0 )    // Not needed as BCM_L3_ADD_TO_ARL flag is used later
    {
        bcm_l2_addr_t l2_addr;
        
        bcm_l2_addr_t_init(&l2_addr, local_mac_1, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    printf("Doing MPLS CONFIG\n");
    BCM_IF_ERROR_RETURN( mpls_action() );
    
    bcm_field_qset_t    testQset;
    
    BCM_FIELD_QSET_INIT(testQset);
    BCM_FIELD_QSET_ADD(testQset, bcmFieldQualifyStageIngress);
    //BCM_FIELD_QSET_ADD(testQset, bcmFieldQualifyDstMac);
    //BCM_FIELD_QSET_ADD(testQset, bcmFieldQualifyInPort);
    
    /////////////////////////////////////
    bcm_udf_pkt_format_info_t     pkt_format;

    bcm_udf_pkt_format_info_t_init(&pkt_format);
    //pkt_format.mpls = BCM_PKT_FORMAT_MPLS_ONE_LABEL;
    pkt_format.mpls = BCM_PKT_FORMAT_MPLS_ONE_LABEL;
    pkt_format.tunnel = BCM_PKT_FORMAT_TUNNEL_MPLS;
    //pkt_format.ip_protocol =     BCM_PKT_FORMAT_IP4;
    pkt_format.prio = 1;

    /* Create the new UDF packet format */
    /* CREATE_O_NONE means no options, options are "with_id" and "replace" */
    printf("Doing MPLS_ONE_LABEL format create\n");
    BCM_IF_ERROR_RETURN(bcm_udf_pkt_format_create
                        (unit, BCM_UDF_PKT_FORMAT_CREATE_O_NONE, &pkt_format,
                         &pkt_format_id));
    
    bcm_udf_alloc_hints_t     hints;
    bcm_udf_t               udf_info;
    bcm_udf_id_t            udf_id[2]; // 1 for outer tag, 1 for inner tag
    
    if( 1 )
    {
        bcm_udf_alloc_hints_t_init(&hints);
        hints.qset = testQset;
        hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;
        //hints.flags |= BCM_UDF_CREATE_O_SHARED_HWID;
        //hints.shared_udf = shared;
        
        bcm_udf_t_init(&udf_info);
        /* Layer is MPLS (Base offset) */
        udf_info.layer = bcmUdfLayerTunnelHeader;
        //udf_info.layer = bcmUdfLayerL2Header;
        /* Offset relative to layer to start of UDF base offset in bits */
        udf_info.start = (3 * 8);    // Start of TTL of 1st Label MPLS
        //udf_info.start = (20 * 8);    // Start of TTL of 1st Label MPLS
        /* Width of UDF field in bits */
        udf_info.width = (1 * 8);    // Entire TTL
        
        printf("Doing bcm_udf_create for 0\n");
        BCM_IF_ERROR_RETURN(bcm_udf_create(unit, &hints, &udf_info, &udf_id[0]));
        
        /* Add the UDF packet format to the new UDF. */
        printf("Doing Create UDF %d from pkt_format %d;  Offset=%d; Width: %d;\n", udf_id[0],
                pkt_format_id, udf_info.start, udf_info.width);
        BCM_IF_ERROR_RETURN(bcm_udf_pkt_format_add(unit, udf_id[0], pkt_format_id));
    }
    
    //////////////////////////////////////////////////////////
    bcm_udf_pkt_format_info_t_init(&pkt_format);
    pkt_format.mpls = BCM_PKT_FORMAT_MPLS_TWO_LABELS;
    //pkt_format.mpls = BCM_PKT_FORMAT_MPLS_ONE_LABEL;
    pkt_format.tunnel = BCM_PKT_FORMAT_TUNNEL_MPLS;
    //pkt_format.ip_protocol = BCM_PKT_FORMAT_IP6;
    pkt_format.prio = 2;

    /* Create the new UDF packet format */
    /* CREATE_O_NONE means no options, options are "with_id" and "replace" */
    printf("Doing MPLS_TWO_LABELS format create\n");
    BCM_IF_ERROR_RETURN(bcm_udf_pkt_format_create
                        (unit, BCM_UDF_PKT_FORMAT_CREATE_O_NONE, &pkt_format,
                         &pkt_format_id));
    
    if( 1 )
    {
        bcm_udf_alloc_hints_t_init(&hints);
        hints.qset = testQset;
        hints.flags = BCM_UDF_CREATE_O_FIELD_INGRESS;
        hints.flags |= BCM_UDF_CREATE_O_SHARED_HWID;
        hints.shared_udf = udf_id[0];
        
        bcm_udf_t_init(&udf_info);
        /* Layer is MPLS (Base offset) */
        udf_info.layer = bcmUdfLayerTunnelHeader;
        /* Offset relative to layer to start of UDF base offset in bits */
        udf_info.start = (7 * 8);    // Start of TTL of 2nd Label of MPLS
        /* Width of UDF field in bits */
        udf_info.width = (1 * 8);    // Entire Outer Tag
        
        printf("Doing bcm_udf_create for 1\n");
        BCM_IF_ERROR_RETURN(bcm_udf_create(unit, &hints, &udf_info, &udf_id[1]));
        
        /* Add the UDF packet format to the new UDF. */
        printf("Doing create UDF %d from pkt_format %d;  Offset=%d; Width: %d;\n", udf_id[1],
                pkt_format_id, udf_info.start, udf_info.width);
        BCM_IF_ERROR_RETURN(bcm_udf_pkt_format_add(unit, udf_id[1], pkt_format_id));
    }
    
    /* Add all UDF qualifier objects to field group QSET. */
    printf("Doing UDF Qsets multi done\n");
    BCM_IF_ERROR_RETURN(bcm_field_qset_id_multi_set
                        (unit, bcmFieldQualifyUdf, 2, udf_id, testQset));
    
    ////////////////////////////////////////////////////////////////////////////
    if( 1 )
    {
        bcm_field_group_t group = 100;
        const int   pri = group;
        /* And finally create a group based on QSET and UDF qualifiers */
        printf("Doing Group created %d\n", group);
        BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, testQset, pri, group));
        
        bcm_field_entry_t entry;

        printf("Doing bcm_field_entry_create done!\n");
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));
        
        /* Remove all qualifiers from field entry */
        printf("Doing bcm_field_qualify_clear..\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_clear(unit, entry));
        
        printf("Doing bcm_field_qualify_udf 0..\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_udf
                            (unit, entry, udf_id[0], data_width, ttl_match_data, ttl_match_mask));
        
        printf("Doing bcm_field_qualify_udf 1..\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_udf
                            (unit, entry, udf_id[1], data_width, ttl_match_data, ttl_match_mask));
        
        //BCM_IF_ERROR_RETURN(bcm_field_action_add
        //                    (unit, entry, bcmFieldActionCopyToCpu, 1, entry));

        const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };
        /* Also set up an FP stat counter to count matches */
        BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group, 2, stats, main_ifp_statid));
        /* Attach the stat object to the entry */
        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, main_ifp_statid));
        
        printf("Doing Entry create %d\n", entry);
        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }
    
    return BCM_E_NONE;
}

/*
 * VERIFICATION:
 * PACKET DETAILS: (Tests with 2 packets)
 *     INGRESS PKT1: (MPLS packet with 1 label)
 *         MACDA: 0xDD, MACSA: 0xCC, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 39(h27) ... IP header and payload
 *     EGRESS PKT1: (Outer most Popped MPLS label)
 *         MACDA: 0xEE  MACSA: 0xEE, VID: 100, IP Header and payload
 *         
 *     INGRESS PKT2: (MPLS packet with 2 labels)
 *         MACDA: 0xDD, VID: 100, MPLS LABLEL1: 1000(h3E8), 7(h7), 33(h21),  MPLS LABEL2: 2000(h7D0), 6(h6), 39(h27) ... IP header and payload
 *     EGRESS PKT2: (Outer most Popped MPLS label)
 *         MACDA: 0xDD, VID: 100, MPLS LABEL: 2000(h7D0), 6(h6), 39(h27) ... IP header and payload
 * 
 * VERIFY:
 *     1. Finally the stat 1 should become equal to 2 because both the packets has inner most TTL as h27.
 *         Look for this string in the log: "The value is: 0x000000002"
 *     #
 */
bcm_error_t verify()
{
    char cmd[2048];
    
    // MACDA: 0xDD, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 39(h27)
    printf("#########  Sending 1 label mpls packet with matching TTL  #########\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X8847%05XF%02X4500003A0000000040FF3D8714141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324258569A726",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_vid, rx_label, ttl_match_data[0], ip_subnet);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // MACDA: 0xDD, VID: 100, MPLS ONE label: 1000(h3E8), 7(h7), 33(h21)
    printf("#########  Sending 1 label mpls packet with NON-matching TTL  #########\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X8847%05XF%02X4500003A0000000040FF3D8714141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324258569A726",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_vid, rx_label, ttl_unmatch, ip_subnet);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // MACDA: 0xDD, VID: 100, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21)  |  2000(h7D0), 6(h6), 39(h27)
    printf("#########  Sending 2 label mpls packet with matching TTL  #########\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X8847%05XE%02X%05XD%02X450000360000000040FF3D8B14141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021AAA1C956",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_vid, rx_label, ttl_unmatch, rx_label_two, ttl_match_data[0], ip_subnet);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    /////////////////////////////////////////////////////////////////////////////
    // MACDA: 0xDD, VID: 100, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21)  |  2000(h7D0), 6(h6), 33(h21)
    printf("#########  Sending 2 label mpls packet with NON-matching TTL  #########\n");
    sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X8847%05XE%02X%05XD%02X450000360000000040FF3D8B14141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021AAA1C956",
    ing_port1,
    local_mac_1[0], local_mac_1[1], local_mac_1[2],
    local_mac_1[3], local_mac_1[4], local_mac_1[5],
    test_vid, rx_label, ttl_unmatch, rx_label_two, ttl_unmatch, ip_subnet);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "sleep %d", 3);
    printf("%s\n", cmd); bshell(unit, cmd);
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("%s\n", cmd); bshell(unit, cmd);
    /////////////////////////////////////////////////////////////////////////////
    printf("\n\t Final STAT count should be 2.\n\n");

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = ConfigureUdfAndFp()) != BCM_E_NONE )
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


int rrv;

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

