/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : IFP Delayed Actions
 *
 *  Usage    : BCM.0> cint ifp_delayed_actions.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : ifp_delayed_actions_log.txt
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
 *      IFP Delayed action demonstration using BCM APIs.
 *      TH3 is optimized for residency of the packet in the chip. Due to
 *      which many pipeline stages are folded. Packet resolution is one of them.
 *      Due to this, at IFP stage port resolution is not known. Hence, the
 *      qualifier MatchPbmpRedirect is introduced to apply delayed action of
 *      legacy PortRedirect.
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
 *    2) Step2 - Configuration (Done in ConfigureIfpDelayedAction()).
 *    ===================================================
 *       a) Create a VLAN(100) and add ing_port1, ing_port2, egr_port1,
 *          egr_port2, egr_nhop_redirect_port and egr_match_redirect_port as
 *          members. Where,
 *             egr_port1              : is the routed next hop dest port for
 *                                      ip_subnet_1
 *             egr_nhop_redirect_port : is the RedirectEgrNextHop dest port
 *                                      overriddedn by IFP
 *             egr_match_redirect_port: is the MatchPbmpRedirect dest port in
 *                                      case resolved port is any of egr_port1
 *                                      OR egr_port2 OR egr_nhop_redirect_port
 *       b) Configure MPLS pop action so that terminated packet should go to
 *             egr_obj_1 from l3_intf_1. (LOCALMAC:0xDD, REMOTEMAC=0xEE
 *                                        VLAN=100, EGR_PORT=egr_port1)
 *       c) Create another next hop as 'redirected_nh_index'.
 *                                     (LOCAL_MAC:0xCE, REMOTE_MAC=0xCD
 *                                   VLAN=100, EGR_PORT=egr_nhop_redirect_port)
 *       d) Create entry:
 *                 QSET: Ingress, InPort
 *                 ACTION: MatchPbmpRedirect
 *                 ACTION: RedirectEgrNextHop
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send an IPv4 packet:
 *             DMAC        : local_mac_1
 *             VLAN        : test_vid
 *             MPLS LABEL  : rx_label
 *             DIP         : ip_subnet_1
 *       b) You will see that packets are egressing from the redirected port
 *          egr_match_redirect_port if opt_EnableMatchPbmpRedirect = 1.
 *       c) Expected Result:
 *       ================
 *       Please see the "TEST CONFIGURATIONS SUMMARY" section below.
 *   LOG SUMMARY:
 *      PACKET1:
 *      MACDA:0xDD, MACSA: 0xCC, VLANID: 0x0064, MPLS LABEL: 1000,
 *                                      SIP: 10.10.10.11, DIP: 20.20.20.21
 *
 *      PACKET2:
 *      MACDA:0xEE, MACSA: 0xDD, VLANID: 0x0064, MPLS LABEL: 1000,
 *                                      SIP: 10.10.10.11, DIP: 20.20.20.21
 *
 *      PACKET3:
 *      MACDA:0xCD, MACSA: 0xCE, VLANID: 0x0064, SIP: 10.10.10.11,
 *                                      DIP: 20.20.20.21
 *
 *      Summary:
 *          In simple statement: If NHOP egress port falls in any of the
 *          given PBMP, redirect it to a given port.
 *          
 *          Test case:
 *          #. PACKET1 ingresses
 *          #. 2 nexthop entries are created.
 *              NHOOP_a. Makes PACKET2 to egress from egr_port1
 *              NHOOP_b. Makes PACKET3 to egress from egr_nhop_redirect_port
 *              
 *              Kind of NHOOP_b_. Makes PACKET3 to egress from
 *              egr_match_redirect_port
 *
 *   TEST CONFIGURATIONS SUMMARY of egressing packet
 *
 *     A]   opt_EnableMatchPbmpRedirect = 0 (0x0)
 *          opt_EnableNhopRedirect = 0 (0x0)
 *              DMAC: 0xEE
 *              SMAC: 0xDD
 *              EGR port: egr_port1
 *
 *     B]   opt_EnableMatchPbmpRedirect = 0 (0x0)
 *          opt_EnableNhopRedirect = 1 (0x1)
 *              DMAC: 0xCD
 *              SMAC: 0xCE
 *              EGR port: egr_nhop_redirect_port
 *
 *     C]   opt_EnableMatchPbmpRedirect = 1 (0x1)
 *                                      [Match PBMP=egr_nhop_redirect_port]
 *          opt_EnableNhopRedirect = 1 (0x1)
 *              DMAC: 0xCD
 *              SMAC: 0xCE
 *              EGR port: egr_match_redirect_port
 *
 *     D]   opt_EnableMatchPbmpRedirect = 1 (0x1) [Match PBMP=egr_port1]
 *          opt_EnableNhopRedirect = 0 (0x0)
 *              DMAC: 0xEE
 *              SMAC: 0xDD
 *              EGR port: egr_match_redirect_port
 *
 *
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
    printf("Executing \"%s \" \n", str);                                     /* */
    bshell(unit, str);                                                     /* */
}                                                                          /* */
/****************   GENERIC UTILITY FUNCTIONS ENDS  ****************************
 */

int opt_EnableMatchPbmpRedirect = 1;
int opt_EnableNhopRedirect = 1;

print opt_EnableMatchPbmpRedirect;
print opt_EnableNhopRedirect;

int unit=0;

bcm_vlan_t test_vid = 0x0064;    // d100

bcm_port_t ing_port1 = PORT_ANY; // Set to a valid port if known already.
bcm_port_t ing_port2 = PORT_ANY;

// Below is the routed next hop dest port for ip_subnet_1
bcm_port_t egr_port1 = PORT_ANY;

bcm_port_t egr_port2 = PORT_ANY;

// Below is the RedirectEgrNextHop dest port overriddedn by IFP
bcm_port_t egr_nhop_redirect_port = PORT_ANY;

// Below is the MatchPbmpRedirect dest port in case resolved
//  port is any of egr_port1 OR egr_port2 OR egr_nhop_redirect_port
bcm_port_t egr_match_redirect_port = PORT_ANY;


/* Populate the ports */
ing_port1 = portGetNextE(unit, ing_port1);
ing_port2 = portGetNextE(unit, ing_port2);

egr_port1 = portGetNextE(unit, egr_port1);
egr_port2 = portGetNextE(unit, egr_port2);

egr_nhop_redirect_port = portGetNextE(unit, egr_nhop_redirect_port);
egr_match_redirect_port = portGetNextE(unit, egr_match_redirect_port);

print ing_port1;
print ing_port2;
print egr_port1;
print egr_port2;
print egr_nhop_redirect_port;
print egr_match_redirect_port;

/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    if (BCM_E_NONE != ingress_port_setup(unit, ing_port1,
            BCM_PORT_LOOPBACK_MAC)) {
        printf("ingress_port_setup() failed for port %d\n", ing_port1);
        return -1;
    }

    bcm_pbmp_t     pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egr_port1);
    BCM_PBMP_PORT_ADD(pbmp, egr_port2);
    BCM_PBMP_PORT_ADD(pbmp, egr_nhop_redirect_port);
    BCM_PBMP_PORT_ADD(pbmp, egr_match_redirect_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp,
            BCM_PORT_LOOPBACK_MAC)) {
        printf("egress_port_multi_setup() failed for port %d\n", egr_port1);
        return -1;
    }

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2,
            BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2,
            BCM_PORT_DISCARD_ALL));

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

bcm_mac_t local_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xEE};

bcm_mac_t redirect_local_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCE};
bcm_mac_t redirect_remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCD};

uint32 rx_label = 1000;

bcm_ip_t ip_subnet_1=0x0A0A0A0B;    // dest IP

int main_ifp_statid = 1;

// NHOOP_b
bcm_error_t
configure_redirect_nexthop(int unit, bcm_port_t dest_port, bcm_vlan_t vid,
                            bcm_mac_t* smac, bcm_mac_t* dmac, bcm_if_t *nh_index)
{
    bcm_l3_intf_t         l3_intf;
    bcm_l3_egress_t        l3_egress;
    bcm_vrf_t vrf = 0;
    bcm_gport_t gport;
    bcm_module_t modid;
    bcm_port_t modport;
    int rv;

    printf("Doing gport get\n");
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, dest_port, &gport));
    modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    modport = BCM_GPORT_MODPORT_PORT_GET(gport);

    bcm_l3_intf_t_init(&l3_intf);

    sal_memcpy(l3_intf.l3a_mac_addr, smac, 6);
    l3_intf.l3a_vid =  vid;
    l3_intf.l3a_flags  = BCM_L3_ADD_TO_ARL;
    l3_intf.l3a_vrf = vrf;
    l3_intf.l3a_ttl = 5;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (rv < 0) {
        printf("bcm_l3_intf_create failed!!! rv = %d\n",rv);
        return rv;
    }      
    printf("bcm_l3_intf_create l3_intf - %d\n", l3_intf.l3a_intf_id);
    /* Now create the l3_egress object which gives the index to l3 interface during lookup */ 
    bcm_l3_egress_t_init(&l3_egress);
    
    /* Now copy the nexthop destmac, set dest port and index of L3_INTF table which is created above */
    sal_memcpy(l3_egress.mac_addr, dmac, 6);
    l3_egress.intf = l3_intf.l3a_intf_id;
    l3_egress.port = modport;   // for egr_nhop_redirect_port
    l3_egress.module = modid;
    l3_egress.vlan = vid;
    
    /* go ahead and create the egress object */
    printf("++++ Create L3 egress\n");
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, nh_index); 
    if (rv < 0) {
        printf("1 bcm_l3_egress_create failed !!! rv = %d\n", rv);
        return rv;
    }
    printf("++++ Created L3 egress %d\n", *nh_index);

    return BCM_E_NONE;
}


bcm_error_t
mpls_action()
{
    bcm_port_t port_1 = egr_port1; 
    bcm_gport_t gport_1;

    bcm_vlan_t vid_1 = test_vid;

    bcm_l3_intf_t l3_intf_1;
    bcm_vrf_t vrf = 0; 
    bcm_mpls_tunnel_switch_t tun_switch;

    bcm_l3_route_t l3_route;
    bcm_ip_t mask = 0xffffff00;

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
        bcm_l3_intf_t_init(&l3_intf_1);
        l3_intf_1.l3a_flags  = BCM_L3_ADD_TO_ARL;
        sal_memcpy(l3_intf_1.l3a_mac_addr, local_mac_1, 6);
        l3_intf_1.l3a_vid = vid_1;
        l3_intf_1.l3a_vrf = vrf;
        printf("Doing l3 intf create\n");
        BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf_1));

        /* MPLS termination flow */
        bcm_l3_egress_t_init(&l3_egress);
        l3_egress.intf = l3_intf_1.l3a_intf_id; 
        sal_memcpy(l3_egress.mac_addr, remote_mac_1, 6);
        l3_egress.vlan = vid_1;
        l3_egress.module = modid_1;
        l3_egress.port = modport_1; // for egr_port1
        printf("Doing l3 egress create\n");
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_1));    // NHOOP_a

        bcm_l3_route_t_init(&l3_route);
        l3_route.l3a_subnet = ip_subnet_1;
        l3_route.l3a_ip_mask = mask;
        l3_route.l3a_intf = egr_obj_1;
        printf("Doing l3 route add\n");
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
    
    printf("Doing Add l2 tunnel switch add\n");
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_switch_add(unit, &tun_switch));
    
    printf("Doing Add l2 tunnel add\n");
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, local_mac_1, vid_1));
    
    return BCM_E_NONE;
}

bcm_mac_t test_dmac;


bcm_error_t ConfigureIfpDelayedAction()
{
    bcm_udf_pkt_format_id_t     pkt_format_id;
    
    if( 1 )
    {
        bcm_pbmp_t     vlan_pbmp;
        bcm_pbmp_t     ut_pbmp;

        BCM_PBMP_CLEAR(vlan_pbmp);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, ing_port2);
        
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port1);
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_port2);
        
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_nhop_redirect_port);
        BCM_PBMP_PORT_ADD(vlan_pbmp, egr_match_redirect_port);

        BCM_PBMP_CLEAR(ut_pbmp);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, test_vid));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, test_vid, vlan_pbmp, ut_pbmp));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, 1, vlan_pbmp)); // remove from default vlan
    }
    if( 0 )    // Not needed as BCM_L3_ADD_TO_ARL flag is used later
    {

        bcm_l2_addr_t l2_addr;
        
        sal_memcpy(&test_dmac, &local_mac_1, 6);
        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    bcm_if_t     redirected_nh_index;
    
    printf("Doing Mpls configure\n");
    BCM_IF_ERROR_RETURN( mpls_action() );
    
    // configure_redirect_nexthop(int unit, bcm_port_t dest_port, bcm_vlan_t vid,
    //                        bcm_mac_t* smac, bcm_mac_t* dmac, bcm_if_t *nh_index)
    
    printf("Doing redirect nexthop configure\n");
    BCM_IF_ERROR_RETURN( configure_redirect_nexthop(unit, egr_nhop_redirect_port, test_vid,
                                redirect_local_mac_1, redirect_remote_mac_1, &redirected_nh_index) );
    
    bcm_field_qset_t    testQset;
    
    BCM_FIELD_QSET_INIT(testQset);
    BCM_FIELD_QSET_ADD(testQset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(testQset, bcmFieldQualifyInPort);
    
    ////////////////////////////////////////////////////////////////////////////
    if( 1 )
    {
        bcm_field_group_t group = 100;
        const int   pri = group;
        /* And finally create a group based on QSET and UDF qualifiers */
        printf("Doing Group create %d\n", group);
        BCM_IF_ERROR_RETURN(bcm_field_group_create_id(unit, testQset, pri, group));
        
        bcm_field_entry_t     entry;

        printf("Doing bcm_field_entry_create!\n");
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));
        
        /* Remove all qualifiers from field entry */
        printf("Doing bcm_field_qualify_clear!\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_clear(unit, entry));
        
        printf("Doing InPort qualifier\n");
        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort
                            (unit, entry, ing_port1, 0xFFFF));
        
        if( 0 )
        {
            printf("Doing CopyToCpu action add\n");
            BCM_IF_ERROR_RETURN(bcm_field_action_add
                                (unit, entry, bcmFieldActionCopyToCpu, 1, entry));
        }
        
        if( opt_EnableMatchPbmpRedirect )    // NHOOP_b_
        {
            bcm_field_action_match_config_t match;
            bcm_field_action_params_t params;
            
            match.elephant_pkt_only = 0;
            BCM_PBMP_CLEAR(match.egr_pbmp);
            BCM_PBMP_PORT_ADD(match.egr_pbmp, egr_port1);
            BCM_PBMP_PORT_ADD(match.egr_pbmp, egr_port2);
            BCM_PBMP_PORT_ADD(match.egr_pbmp, egr_nhop_redirect_port);
            
            params.param0 = egr_match_redirect_port;
            params.param1 = 0;
            BCM_PBMP_CLEAR(params.pbmp);
            
            printf("Doing MatchPbmpRedirect action add\n");
            BCM_IF_ERROR_RETURN(bcm_field_action_config_info_add
                                (unit, entry, bcmFieldActionMatchPbmpRedirect, &params, &match));
        }
        
        if( opt_EnableNhopRedirect )
        {
            printf("Doing RedirectEgrNextHop action add %d\n", redirected_nh_index);
            BCM_IF_ERROR_RETURN(bcm_field_action_add
                                (unit, entry, bcmFieldActionRedirectEgrNextHop, redirected_nh_index, 0));
        }
        
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

bcm_error_t verify()
{
    char cmd[2048];
    
    if( 1 )
    {
        // MACDA: 0xDD, VID: 0x64, MPLS ONE label: 1000(h3E8), 7(h7), 39(h27)
        sprintf(cmd, "tx 1 pbm=%d data=%02X%02X%02X%02X%02X%02X0000000000CC8100%04X88470%04XF274500003A0000000040FF3D8714141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021222324258569A726",
        ing_port1,
        local_mac_1[0], local_mac_1[1], local_mac_1[2],
        local_mac_1[3], local_mac_1[4], local_mac_1[5],
        test_vid, rx_label, ip_subnet_1);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 3);
        printf("%s\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
        printf("%s\n", cmd); bshell(unit, cmd);
    }
    
    if( 0 )
    {
        /////////////////////////////////////////////////////////////////////////////
        // MACDA: 0xDD, VID: 0x64, MPLS TWO label: 1000(h3E8), 7(h7), 33(h21)  |  2000(h7D0), 6(h6), 39(h27)
        sprintf(cmd, "tx 1 pbm=%d data=0000000000%02X0000000000CC8100%04X88470%04XE21007D0D27450000360000000040FF3D8B14141416%08X000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021AAA1C956", ing_port1, local_mac_1[5], test_vid, rx_label, ip_subnet_1);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 3);
        printf("%s\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
        printf("%s\n", cmd); bshell(unit, cmd);
        /////////////////////////////////////////////////////////////////////////////
    }
    if( 1 )
    {
        /////////////////////////////////////////////////////////////////////////////
        bbshell(unit, "show c");
        /////////////////////////////////////////////////////////////////////////////
    }
    
    return BCM_E_NONE;
}

bcm_error_t execute()
{
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Creating the test setup failed %d\n", rrv);
        return rrv;
    }
    
    if( (rrv = ConfigureIfpDelayedAction()) != BCM_E_NONE )
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

