/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : IFP Presel (Logical Table)
 *
 *  Usage    : BCM.0> cint ifp_presel.c
 *
 *  config   : field_config.bcm
 *
 *  Log file : ifp_presel_log.txt
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
 *      IFP Logical Table Selection [LTS]/Presel demonstration using BCM APIs.
 *      It demonstrates either in PIPE LOCAL or GLOBAL mode.
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
 *    2) Step2 - Configuration (Done in configure_global_or_perPipe_ifp()).
 *    ===================================================
 *       a) Create Presel entry with qualifier as
 *                QSET: IpTypeIpv4Any and
 *                QSET: StageIngress (for IFP presel).
 *       b) Create IFP Entry with
 *                 QSET: SrcIp
 *                 QSET: InPort
 *                 QSET: Presel (presel_id)
 *                 ACTION: DscpNew
 *                 ACTION: CopyToCpu
 *                 STAT: main_ifp_statid = 1
 *       c) Create an L3 interface so that L3 packet gets recognised as L3.
 *                 DMAC: 0xDD (test_dmac)
 *                 VLAN: 100  (test_vid)
 *       d) Create L3 egress object and set above interface (OPTIONAL)
 *       e) Create a route entry for dest ip = 0x0A0A0A0B (OPTIONAL)
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==============================
 *       a) Send a packet with
 *             test_dmac = 0xDD | any srcmac | test_vid = 100 |
 *             fp_cfg.src_ip = 0x0A0A0A14 | dest_ip = 0x0A0A0A0B |
 *       b) STAT with ID = 1 incremented by 1.
 *       c) Expected Result:
 *          ================
 *          Final STAT count should be 1.
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

int opt_PipeModeApiSupported = 0;  // Firebolt6 does not support pipe mode API
int opt_PipeLocal = 0; /* Set 0: Global mode and 1:Pipe Local Mode */

print opt_PipeModeApiSupported;
print opt_PipeLocal;


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


/* fp configuration structure */
struct fp_cfg_t{
    bcm_ip_t src_ip;            /* IP Address to match in main IFP TCAM */
    bcm_ip_t ip_mask;           /* IP Address Mask to match in main IFP TCAM */
    int dscp_new_data;          /* New DSCP value to assign for matched flows */
    int pipe;                   /* Pipe to configure in Pipe local mode */
    bcm_field_IpType_t ip_type; /* IP Type to match in Presel TCAM */
};

/* Set the reguired fp Configuration here */
fp_cfg_t fp_cfg = {
    0x0A0A0A14,                     /* IP Address to match in main IFP TCAM */
    0xFFFFFFFF,                     /* IP Address Mask to match in main IFP TCAM */
    0x9,                            /* New DSCP value to assign for matched flows */
    0x1,                            /* Pipe to configure in Pipe local mode */
    bcmFieldIpTypeIpv4Any           /* IP Type to match in Presel TCAM */
};

bcm_ip_t dest_ip = 0x0A0A0A0B;    // dest IP
bcm_ip_t dest_ip_mask = 0xffffff00;
bcm_mac_t remote_mac_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xEE};
bcm_mac_t test_dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDD};
bcm_mac_t test_smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};
bcm_mac_t mac_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int         main_ifp_statid = 1;

bcm_error_t ConfigureL3(int unit, bcm_mac_t InterfaceMac, bcm_port_t EgrPort)
{
    bcm_l3_intf_t         l3_intf_1;
    bcm_l3_egress_t       l3_egress;
    bcm_if_t              egr_obj_1;
    bcm_gport_t           gport_1;
    bcm_module_t          modid_1;
    bcm_port_t            modport_1;
    bcm_if_t              intf_id_1 = 1;
    bcm_vrf_t             vrf = 0;
    bcm_l3_route_t        l3_route;
    
    printf("Doing sc egress mode\n");
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    
    printf("Doing gport get\n");
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, EgrPort, &gport_1));
    
    modid_1 = BCM_GPORT_MODPORT_MODID_GET(gport_1);
    modport_1 = BCM_GPORT_MODPORT_PORT_GET(gport_1);
    
    // l3 intf for customer port
    printf("Doing l3 intf create\n");
    bcm_l3_intf_t_init(&l3_intf_1);
    l3_intf_1.l3a_flags  = BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID;
    l3_intf_1.l3a_intf_id = intf_id_1;
    sal_memcpy(l3_intf_1.l3a_mac_addr, InterfaceMac, 6);
    l3_intf_1.l3a_vid = test_vid;
    l3_intf_1.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN( bcm_l3_intf_create(unit, &l3_intf_1) );

    /* MPLS termination flow */
    printf("Doing l3 egress object\n");
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf = intf_id_1; 
    sal_memcpy(l3_egress.mac_addr, remote_mac_1, 6);
    l3_egress.vlan = test_vid;
    l3_egress.module = modid_1;
    l3_egress.port = modport_1;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_1));

    printf("Doing l3 route add\n");
    bcm_l3_route_t_init(&l3_route);
    l3_route.l3a_subnet = dest_ip;
    l3_route.l3a_ip_mask = dest_ip_mask;
    l3_route.l3a_intf = egr_obj_1;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, l3_route));
    
    int station_id = 1;
    bcm_l2_station_t station_info;
    bcm_l2_station_t_init(&station_info);
    
    if( 0 ) // BCM_L3_ADD_TO_ARL used earlier. Station add not needed.
    {
        sal_memcpy(station_info.dst_mac, InterfaceMac, 6);
        sal_memcpy(station_info.dst_mac_mask, mac_mask, 6);
        //station_info.vlan = test_vid;
        //station_info.vlan_mask = 0xFFFF;
        station_info.flags = BCM_L2_STATION_IPV4 | BCM_L2_STATION_IPV6;
        printf("Doing l2 station add..\n");
        BCM_IF_ERROR_RETURN( bcm_l2_station_add(unit, &station_id, &station_info) );
    }
    
    return BCM_E_NONE;
}

int
configure_global_or_perPipe_ifp(int unit)
{
    bcm_field_entry_t entry;
    bcm_field_presel_t presel_id, presel_entry_id;
    
    bcm_field_group_config_t group_config;
    
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
        
        bcm_l2_addr_t_init(&l2_addr, test_dmac, test_vid);
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = ing_port2;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));
    }
    
    if( opt_PipeModeApiSupported )
    {
        if( opt_PipeLocal )
        {
            printf("------ PIPE LOCAL MODE  ------\n");
            /* Setting the Operational mode to PipeLocal Mode */
            BCM_IF_ERROR_RETURN(bcm_field_group_oper_mode_set(unit,
                                        bcmFieldQualifyStageIngress,
                                        bcmFieldGroupOperModePipeLocal));
        }
        else
        {
            printf("------ PIPE GLOBAL MODE  ------\n");
            /* Setting the Operational mode to Global Mode [Default is Global mode] */
            BCM_IF_ERROR_RETURN(bcm_field_group_oper_mode_set(unit,
                                        bcmFieldQualifyStageIngress,
                                        bcmFieldGroupOperModeGlobal));
        }
    }

    /* IFP Group Configuration and Creation */
    bcm_field_group_config_t_init(&group_config);

    /* IFP Presel Entry Configuration and Creation */
    printf("Doing PRESEL create..\n");
    BCM_IF_ERROR_RETURN(bcm_field_presel_create(unit, &presel_id));
    BCM_FIELD_PRESEL_ENTRY_SET(presel_entry_id, presel_id);
    
    printf("Doing PRESEL qualify for IFP..\n");
    BCM_IF_ERROR_RETURN(bcm_field_qualify_Stage(unit, presel_entry_id, bcmFieldStageIngress));
    printf("Doing PRESEL qualify IpType..\n");
    BCM_IF_ERROR_RETURN(bcm_field_qualify_IpType(unit, presel_entry_id,
                                                 fp_cfg.ip_type));

    if( opt_PipeLocal )
    {
        bcm_port_config_t port_config;
        int pipe;
        
        bcm_port_config_t_init(&port_config);
        
        printf("Doing port config get..\n");
        BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));
        
        for(pipe=0; pipe<8; ++pipe)    // TH3 has 8 pipes
        {
            if( BCM_PBMP_MEMBER(port_config.per_pipe[pipe], ing_port1) )
            {
                break;
            }
        }
        
        printf("ing_port1(%d) belongs to pipe [%d]\n", ing_port1, pipe);
        
        group_config.ports = port_config.per_pipe[pipe];
        group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PORT;
    }
    
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;

    /*
     * Different group modes [Single, Double, Triple or Quad] are possible and
     * availability depends on the chip being used. bcmFieldGroupModeAuto is the
     * default mode and it expands based on the given Qset
     */

    group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_ASET_INIT(group_config.aset);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDscpNew);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionStatGroup);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_PRESEL_INIT(group_config.preselset);
    BCM_FIELD_PRESEL_ADD(group_config.preselset, presel_id);
    printf("Doing IFP Group Config Create..\n");
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    /* IFP Entry Configuration and Creation */
    printf("Doing IFP Entry Create..\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    printf("Doing IFP qualify for SrcIp..\n");
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcMac(unit, entry, test_smac,
                                  mac_mask));
    printf("Doing IFP qualify for InPort..\n");
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, ing_port1,0xFF));
                                  
    printf("Doing entry action add DscpNew..\n");
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDscpNew,
                                             fp_cfg.dscp_new_data, 0));
    printf("Doing entry action add CopyToCpu..\n");
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu,
                                  3, 3));
                                  
    printf("Doing stat attach..\n");
    
    const bcm_field_stat_t stats[2] = { bcmFieldStatPackets, bcmFieldStatBytes };
    BCM_IF_ERROR_RETURN(bcm_field_stat_create_id(unit, group_config.group, 2, stats, main_ifp_statid));
    BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, main_ifp_statid));
    
    printf("Doing Entry Install..\n");
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    
    if( 1 )
    {
        printf("Doing L3 Configuration...\n");
        BCM_IF_ERROR_RETURN( ConfigureL3(unit, test_dmac, egr_port) );
    }
    
    
    return BCM_E_NONE;
}



/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int        rv;
    int cpu_port = 0;
    
    //print bcm_port_control_set(unit, cpu_port, bcmPortControlIP4, TRUE);
    
    if( 0 )    // disabling this code as we are testing FP functionality in local mode
    {
        rv = ingress_port_setup(unit, ing_port1, BCM_PORT_LOOPBACK_MAC);
        if ( rv != BCM_E_NONE ) {
            printf("ingress_port_setup() failed for port %d (Error: %d)\n", ing_port1, rv);
            return rv;
        }

        rv = egress_port_setup(unit, egr_port, BCM_PORT_LOOPBACK_MAC);
        if ( rv != BCM_E_NONE ) {
            printf("egress_port_setup() failed for port %d (Error: %d)\n", egr_port, rv);
            return rv;
        }
    }
    else
    {
        printf("Doing port loopbacks...\n");
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port1, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port1, BCM_PORT_DISCARD_NONE));
        
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egr_port, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egr_port, BCM_PORT_DISCARD_ALL));
    
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ing_port2, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ing_port2, BCM_PORT_DISCARD_ALL));
    }
    
    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}


/*
 How to verify: It is documented within the function for various packets and options
*/
bcm_error_t verify()
{
    char cmd[1024*2];
    sprintf(cmd, "fp show");
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    int num_pkt_in_a_burst = 1;
    int match_expected = 0;
    
    /////////////////////////////////////////////////////////////////////////////
    // Sending IPv4 pkt with known dest IP addr
    if( 1 )
    {
        printf("\n\n\n############  IP4 and Resolved  #############n");
        sprintf(cmd, "tx %d pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X%04X4500002E0000000040063E8C%08X%08X227D00500000000000000000500000004ACA0000000102030405B0F9EB4F",
                num_pkt_in_a_burst, ing_port1,
                test_dmac[0], test_dmac[1], test_dmac[2],
                test_dmac[3], test_dmac[4], test_dmac[5],
                test_smac[0], test_smac[1], test_smac[2],
                test_smac[3], test_smac[4], test_smac[5],
                test_vid, 0x0800, fp_cfg.src_ip, dest_ip);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);
        match_expected++;
    }
    
    if( 1 )
    {
        printf("\n\n\n############  IP4 and Un-resolved  #############\n");
        sprintf(cmd, "tx %d pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X%04X4500002E0000000040063E8C%08X%08X227D00500000000000000000500000004ACA0000000102030405B0F9EB4F",
                num_pkt_in_a_burst, ing_port1,
                test_dmac[0], test_dmac[1], test_dmac[2],
                test_dmac[3], test_dmac[4]+10, test_dmac[5]+10,
                test_smac[0], test_smac[1], test_smac[2],
                test_smac[3], test_smac[4], test_smac[5],
                test_vid+10, 0x0800, fp_cfg.src_ip*10, dest_ip*10);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);
        match_expected++;
    }
    
    if( 1 )
    {
        printf("\n\n\n############  NON IP  #############\n");
        sprintf(cmd, "tx %d pbm=%d data=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X8100%04X%04X4500002E0000000040063E8C%08X%08X227D00500000000000000000500000004ACA0000000102030405B0F9EB4F",
                num_pkt_in_a_burst, ing_port1,
                test_dmac[0], test_dmac[1], test_dmac[2],
                test_dmac[3], test_dmac[4], test_dmac[5],
                test_smac[0], test_smac[1], test_smac[2],
                test_smac[3], test_smac[4], test_smac[5],
                test_vid+10, 0x0900, fp_cfg.src_ip*10, dest_ip*10);
        printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
        
        sprintf(cmd, "sleep %d", 2);
        printf("%s\n", cmd); bshell(unit, cmd);
    }
    
    sprintf(cmd, "fp stat get si=%d t=p", main_ifp_statid);
    printf("\n\n%s\n\n", cmd); bshell(unit, cmd);
    
    printf("Finally the STAT count should be %d\n", num_pkt_in_a_burst * match_expected);
    
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
    
    if( (rrv = configure_global_or_perPipe_ifp(unit)) != BCM_E_NONE )
    {
        printf("Configuring presel IFP failed with %d\n", rrv);
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

