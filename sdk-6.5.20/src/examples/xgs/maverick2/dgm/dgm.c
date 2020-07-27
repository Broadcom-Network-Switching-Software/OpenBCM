/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Dynamic Group Multipathing
 *  
 *  Usage    : BCM.0> cint dgm.c
 *  
 *  config   : dgm_config.bcm
 *  
 *  Log file : dgm_log.txt
 *  
 *  Test Topology :
 * +-----------------+            +---------+
 * |             XE0 <------------+         |
 * |                 |            |         |
 * |             XE1 +------------>         |
 * |                 |            |         |
 * |             XE2 +------------>Traffic  |
 * |     SVK         |            |Generator|
 * |                 |            |         |
 * |             XE3 +------------>         |
 * |             XE4 +------------>         |
 * |             XE5 +------------>         |
 * |                 |            |         |
 * |             XE6 <------------+         |
 * |                 |            |         |
 * +-----------------+            +---------+
 * 
 *  Summary:
 *  ========
 * This CINT script demonstrates how to configure DGM using BCM APIs.
 * In this example, the least cost paths are on xe1 ~ xe3. The non-least costs
 * paths are on xe4 ~ xe5.
 * This is not a self-sufficient script since multiple flows with specific 
 * bandwidth should be sent from traffic generator. 
 *
 *  Detailed steps done in the CINT script:
 *  =====================================================
 * 1) Step1 - Test Setup
 * ============================================
 * a) Connect xe0~xe6 with traffic generator.
 * b) Make sure xe0~xe4 link up with traffic generator.
 * 
 * 2) Step2 - Configuration
 * ============================================
 * a) Config RTAG7 for ECMP;
 * b) Create ECMP with DGM;
 * c) Attach the ECMP with route;
 * 
 * 3) Step3 - Verification
 * ==========================================
 * Send the following flow into xe0 with 100M bandwidth.
 * 00000000AAAA0000000000018100000C08004500002E0000000040119C0B0A0A0A01C0A80A010000000000000000000000000000000000000000000000000000
 *
 * Expected Result:   
 * ===================
 * a) Traffic is split evenly on xe1-xe3
 * b) 
 *  vlan create 5 pbm=xe6,xe1
 *  vlan create 6 pbm=xe6,xe2
 *  vlan create 7 pbm=xe6,xe3
 *  l2 add mac=0x5 pbm=xe1 vlan=5 rp=false
 *  l2 add mac=0x6 pbm=xe2 vlan=6 rp=false
 *  l2 add mac=0x7 pbm=xe3 vlan=7 rp=false
 * c) Send the following flows into xe6 in wirespeed.
 * 0000000000050042434D007F810020050B0400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 * 
 * 0000000000060042434D007F810020060B0400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 * 
 * 0000000000070042434D007F810020070B0400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 * d) Traffic from xe0 is split evenly on xe4-xe5.
 * e) Stop one of the flows into xe6.
 * f) Traffic from xe0 is split evenly on xe1-xe3 again.
 */

cint_reset();

/* Global (per device) Configuration */
struct glb_dlb_config_t {
    int   sc_type;
    int   sc_value;
};

glb_dlb_config_t
glb_ecmp_config[] =
{
    /* EMWA parameters */
    {bcmSwitchEcmpDynamicEgressBytesExponent, 2},
    {bcmSwitchEcmpDynamicEgressBytesDecreaseReset, 1},
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent, 2},
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset, 1},

    /* Quantization parameters, SDK config 8 equally spaced bands between Min & Max */
    {bcmSwitchEcmpDynamicEgressBytesMinThreshold, 500},   /* 500Mbps, unit Mbs */
    {bcmSwitchEcmpDynamicEgressBytesMaxThreshold, 9500},  /* 9.5Gbps, unit Mbs */
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold, 2080},/* in bytes, granularity 208Bytes */
    {bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold, 208000},
    /* Not supported on MV2.
    {bcmSwitchEcmpDynamicQueuedBytesExponent, 2},
    {bcmSwitchEcmpDynamicQueuedBytesDecreaseReset, 1}, 
    */
    {-1, 0}
};

/* Global DLB Configuraton */
bcm_error_t global_ecmp_dlb_config(int unit)
{
    int glb_ecmp_dlb_ethtypes[] = {0x0800, 0x86DD};
    int i, rv = 0;

    /* Sample Rate */
    rv = bcm_switch_control_set(unit, bcmSwitchEcmpDynamicSampleRate, 100000);  /* 10 us*/
    if(BCM_FAILURE(rv)) {
       printf("\nError in setting DLB sample rate: %s.\n",bcm_errmsg(rv));
       return rv;
    }

    /* EMWA and Quantization parameters */
    for (i = 0; ; i++) {
        if (glb_ecmp_config[i].sc_type == -1) {
            break;
        }
        rv = bcm_switch_control_set(unit, glb_ecmp_config[i].sc_type, glb_ecmp_config[i].sc_value);
        if(BCM_FAILURE(rv)) {
       	   printf("\nError in setting DLB global parameter: %s.\n",bcm_errmsg(rv));
           return rv;
        }
    }

    /* Set the random seed */
    rv = bcm_switch_control_set(unit, bcmSwitchEcmpDynamicRandomSeed, 0x5555);
    if(BCM_FAILURE(rv)) {
       printf("\nError in setting DLB random seed: %s.\n",bcm_errmsg(rv));
       return rv;
    }

    /* Eligibility based on EtherType */
    rv = bcm_l3_egress_ecmp_ethertype_set(unit,
                      BCM_L3_ECMP_DYNAMIC_ETHERTYPE_ELIGIBLE,
                      sizeof(glb_ecmp_dlb_ethtypes)/sizeof(glb_ecmp_dlb_ethtypes[0]),
                      glb_ecmp_dlb_ethtypes);
    if(BCM_FAILURE(rv)) {
       printf("\nError in setting eligible DLB Ethernet type: %s.\n",bcm_errmsg(rv));
       return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t
ecmp_rtag7_enable(int unit, int port)
{
    int flags; 

    BCM_IF_ERROR_RETURN(bcm_switch_control_port_get(unit, port, bcmSwitchHashControl, &flags));
    flags |= BCM_HASH_CONTROL_ECMP_ENHANCE;
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchHashControl, flags));
    
    return BCM_E_NONE;
}

void
ecmp_rtag7_config(int unit)
{
    int flags;

    /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, 0));

    flags = BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_PROTOCOL |
            BCM_HASH_FIELD_DSTL4 |
            BCM_HASH_FIELD_SRCL4 |
            BCM_HASH_FIELD_IP4DST_LO |
            BCM_HASH_FIELD_IP4DST_HI |
            BCM_HASH_FIELD_IP4SRC_LO |
            BCM_HASH_FIELD_IP4SRC_HI;

    /* Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, flags));

    /* Block B - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField1, flags));

    /* Configure Seed */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555));

    /* Enable preprocess */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE));

    /* Configure HASH A and HASH B functions */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config, BCM_HASH_FIELD_CONFIG_CRC32LO));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config1, BCM_HASH_FIELD_CONFIG_CRC32HI));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config, BCM_HASH_FIELD_CONFIG_CRC32LO));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config1, BCM_HASH_FIELD_CONFIG_CRC32HI));

    return BCM_E_NONE;
}

/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

int
add_to_l2_station(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_station_t l2_station;
    int station_id;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    l2_station.vlan         = vid;
    l2_station.vlan_mask    = 0xfff;
    l2_station.flags   = BCM_L2_STATION_IPV4;
    rv = bcm_l2_station_add(unit, &station_id, &l2_station);

    return rv;
}

/*
 * DGM service setup
 *    dgm_ecmp_id (OUT) - DLB ECMP interface ID
 *    dynamic_age - the inactivity duration of the ECMP group
 *    dynamic_size - the flow table size for this ECMP group
 *    primary_count - # of egress objects in primary portion of ECMP group
 *    primary_ports/vlans/eo - array of ports/vlan/egress object IDs for primary members
 *    alt_count - # of egress objects in alternate portion of ECMP group
 *    alt_ports/vlans/eo - array of ports/vlan/egress object IDs for alternate members
 *    dgm_sc - PRIMARY_PATH_THRESHOLD, ALTERNATE_PATH_BIAS, ALTERNATE_PATH_COST
 *    loading_weight_p, queue_size_weight_p - array specifying the load and queue size weight
 *    scaling_factor_p - scaling factor for each ECMP member
 *
 *  Note
 *    1) bcm_l3_ecmp_create() API must be used to create DGM group
 *    2) when specifying the array of port, vlan, EO, you can use port/vlan repeatedly
 *       to test the case that the same port is used in multiple nexthop
 */

int
dgm_ecmp_setup(int unit, int *dgm_ecmp_id,
         int dynamic_age, int dynamic_size, bcm_l3_dgm_t *dgm_sc,
         int primary_count, int alt_count,
         int *port_p, bcm_vlan_t *vlan_p, bcm_if_t *l3_eo_array,
         int *loading_weight_p, int *queue_size_weight_p, int *scaling_factor_p)
{
    int rv, my_modid, i;
    uint32 flags;
    bcm_vlan_t egress_vid;
    bcm_port_t egress_port;
    bcm_pbmp_t pbmp, ubmp;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    int alloc_size;
    bcm_l3_ecmp_member_t ecmp_member,  *member_p;
    bcm_l3_egress_ecmp_t ecmp_group_info;

    alloc_size = sizeof(ecmp_member) * (primary_count + alt_count);
    member_p = sal_alloc (alloc_size, "DGM Member");
    if (!member_p) {
        return(BCM_E_MEMORY);
    }

    /* Create L3 Interface and L3 Egress Object */
    for (i = 0; i < (primary_count + alt_count); i++) {
        /* Create VLAN and VLAN membership */
        rv = vlan_create_add_port(unit, vlan_p[i], port_p[i]);
        if(BCM_FAILURE(rv)) {
           printf("\nError in setting VLAN: %s.\n",bcm_errmsg(rv));
           return rv;
        }

        /* Create L3 interfaces */
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_mac_addr[3] = 0x1;
        l3_intf.l3a_mac_addr[4] = i;
        l3_intf.l3a_vid = vlan_p[i];
        rv = bcm_l3_intf_create(unit, &l3_intf);
        if(BCM_FAILURE(rv)) {
           printf("\nError in creating L3 interface: %s.\n",bcm_errmsg(rv));
           return rv;
        }

        /*
         * Create L3 objects. Assign member DLB attributes.
         * Attributes - Scaling factor, Loading Weight, Queue Size weight
         */
        bcm_l3_egress_t_init(&l3_egress);
        l3_egress.intf = l3_intf.l3a_intf_id;
        l3_egress.mac_addr[3] = 0x2;
        l3_egress.mac_addr[4] = i;
        BCM_GPORT_MODPORT_SET(l3_egress.port, my_modid, port_p[i]);
        l3_egress.dynamic_load_weight       = loading_weight_p[i];
        l3_egress.dynamic_queue_size_weight = queue_size_weight_p[i];
        l3_egress.dynamic_scaling_factor    = scaling_factor_p[i];
        flags = l3_eo_array[i] == -1 ? 0 : BCM_L3_WITH_ID;
        rv = bcm_l3_egress_create(unit, flags, &l3_egress, &l3_eo_array[i]);
        if(BCM_FAILURE(rv)) {
           printf("\nError in creating next hop: %s.\n",bcm_errmsg(rv));
           return rv;
        }
        print l3_eo_array[i];

        /* Member DGM attributes */
        bcm_l3_ecmp_member_t_init(&member_p[i]);
        member_p[i].egress_if = l3_eo_array[i];
        member_p[i].flags = (i < primary_count) ? 0 : BCM_L3_ECMP_MEMBER_DGM_ALTERNATE;
    }

    /*
     * Create L3 ECMP group with DGM attributes and Member DGM attributes.
     * DGM Attributes -  DGM Mode, Size (flowset entries), Aging
     * Member Attributes -  primary member or Alternate member
     */
    bcm_l3_egress_ecmp_t_init(&ecmp_group_info);
    ecmp_group_info.ecmp_intf = *dgm_ecmp_id;
    flags = (*dgm_ecmp_id != -1) ?  BCM_L3_ECMP_O_CREATE_WITH_ID : 0;
    ecmp_group_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DGM;  /* DGM */
    ecmp_group_info.dynamic_size = dynamic_size;
    ecmp_group_info.dynamic_age  = dynamic_age;
    ecmp_group_info.max_paths = 128;
    ecmp_group_info.dgm.threshold = dgm_sc->threshold;
    ecmp_group_info.dgm.cost      = dgm_sc->cost;
    ecmp_group_info.dgm.bias      = dgm_sc->bias;
    rv = bcm_l3_ecmp_create(unit, flags, &ecmp_group_info,
                            primary_count + alt_count, member_p);
    if(BCM_FAILURE(rv)) {
       printf("\nFail to create ECMP: %s.\n",bcm_errmsg(rv));
       return rv;    
    }

    /* cleanup */
    sal_free(member_p);

    return BCM_E_NONE;
}


int unit = 0;

bcm_vlan_t vid_in = 12;
int port_in = 1;
bcm_l2_addr_t l2addr;
bcm_mac_t router_mac_in  = {0x00,0x00,0x00,0x00,0xAA,0xAA};

bcm_l3_route_t route_info;
bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

/* ECMP Group - handle, (managed by application) */
int dgm_ecmp_intf = 200256;

/* ECMP Group Attributes - dynamic age, dynamic size*/
int dgm_ecmp_dynamic_age = 16;  /* usec */
int dgm_ecmp_dynamic_size = 1024;

/* ECMP Member's - primary ports, alt ports */
int primary_count = 3, alt_count = 2;  /* Total is 5 */

int app_ecmp_port[] = {2, 3, 4,  /* primary */
                       5, 6};    /* Alternate */
bcm_vlan_t app_ecmp_vlan[] = {102, 103, 104,
                       105, 106};
bcm_if_t l3_egress_object_p[] = {100002, 100003, 100004,
                       100005, 100006};
int app_ecmp_loading_weight[] = {50, 50, 50,
                       50, 50};
int app_ecmp_queue_size_weight[] = {50, 50, 50,
                       50, 50};
int app_ecmp_scaling_factor[] = {10, 10, 10,
                       10, 10};

/* ECMP Group - primary versus alt path selection criteria */
bcm_l3_dgm_t
dgm_selection_criteria =
{
    6, /* threshold */
    3, /* cost */
    5, /* bias */
};

bcm_error_t test_setup(int unit)
{
  int rv = BCM_E_NONE;
  bcm_switch_control_set(unit, bcmSwitchUseGport, 1);
  bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
  
  /* RTAG7 Configuraton */
  rv = ecmp_rtag7_config(unit);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to set RTAG7: %s.\n",bcm_errmsg(rv));
     return rv;
  }
  
  rv = ecmp_rtag7_enable(unit, port_in);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to enable RTAG7: %s.\n",bcm_errmsg(rv));
     return rv;
  }
  
  /* Global DLB Configuraton */
  rv = global_ecmp_dlb_config(unit);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to set DLB: %s.\n",bcm_errmsg(rv));
     return rv;
  }
  
  /* Ingress side of the VLAN */
  rv = vlan_create_add_port(unit, vid_in, port_in);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to set VLAN: %s.\n",bcm_errmsg(rv));
     return rv;
  }
  
  /* Enable ingress L3 lookup */
  rv = add_to_l2_station(unit, router_mac_in, vid_in);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to set local station MAC: %s.\n",bcm_errmsg(rv));
     return rv;
  }
  
  /* DGM ECMP setup */
  rv = dgm_ecmp_setup(unit, &dgm_ecmp_intf,
                dgm_ecmp_dynamic_age, dgm_ecmp_dynamic_size,
                &dgm_selection_criteria,
                primary_count, alt_count,
                app_ecmp_port, app_ecmp_vlan, l3_egress_object_p,
                app_ecmp_loading_weight, app_ecmp_queue_size_weight, app_ecmp_scaling_factor);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to set DGM: %s.\n",bcm_errmsg(rv));
     return rv;
  }
  
  /*
   * Install the route for DIP, note this is routed out on the port_out
   */
  bcm_l3_route_t_init(&route_info);
  route_info.l3a_flags = BCM_L3_MULTIPATH;
  route_info.l3a_intf = dgm_ecmp_intf;
  route_info.l3a_subnet = dip;
  route_info.l3a_ip_mask = mask;
  rv = bcm_l3_route_add(unit, &route_info);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to set route: %s.\n",bcm_errmsg(rv));
     return rv;
  }
  
  return BCM_E_NONE;
}

bcm_error_t execute()
{
    int rrv;
    
    if( (rrv = test_setup(unit)) != BCM_E_NONE )
    {
        printf("Fail to configure DGM %d\n", rrv);
        return rrv;
    }
    
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
