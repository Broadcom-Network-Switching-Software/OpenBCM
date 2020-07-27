/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : DLB for ECMP
 *  
 *  Usage    : BCM.0> cint dlb_ecmp.c
 *  
 *  config   : dgm_config.bcm
 *  
 *  Log file : dlb_ecmp_log.txt
 *  
 *  Test Topology :
 * +-----------------+            +---------+
 * |             XE0 <------------+         |
 * |                 |            |         |
 * |             XE1 +------------>         |
 * |                 |            |         |
 * |                 |            |Traffic  |
 * |     SVK     XE2 +------------>Generator|
 * |                 |            |         |
 * |             XE3 +------------>         |
 * |                 |            |         |
 * |             XE4 +------------>         |
 * |                 |            |         |
 * |                 |            |         |
 * |                 |            |         |
 * +-----------------+            +---------+
 * 
 *  Summary:
 *  ========
 * This CINT script demonstrates how to configure DLB for ECMP.
 * 
 *  Detailed steps done in the CINT script:
 *  =====================================================
 * 1) Step1 - Test Setup
 * ============================================
 * a) Connect xe0~xe4 with traffic generator.
 * b) Make sure xe0~xe4 link up with traffic generator.
 * 
 * 2) Step2 - Configuration
 * ============================================
 * ./bcm.user
 * c dlb_ecmp.c
 * 
 * 3) Step3 - Verification
 * ==========================================
 * Send wirespeed traffic into xe0
 * 00000000AAAA0042434D00798100000C
 * 08004500002E0000000040119C020A0A
 * 0A0AC0A80A0100000000000000000000
 * 00000000000000000000000000000000
 * Expected Result:   
 * ===================
 * Traffic is split evenly on xe1-xe4
 * BCM.0> show c
 * RIPC4_64.xe0                :            49,102,130         +49,102,130      14,203,277/s
 * RUC_64.xe0                  :            49,102,188         +49,102,188      14,203,277/s
 * ING_NIV_RX_FRAMES_VL.xe0    :            49,102,378         +49,102,378      14,203,277/s
 * CLMIB_R127.xe0              :            49,102,828         +49,102,828      14,203,304/s
 * CLMIB_RPKT.xe0              :            49,102,842         +49,102,842      14,203,304/s
 * CLMIB_RUCA.xe0              :            49,102,844         +49,102,844      14,203,304/s
 * CLMIB_RPRM.xe0              :            49,102,870         +49,102,870      14,203,304/s
 * CLMIB_RVLN.xe0              :            49,102,871         +49,102,871      14,203,304/s
 * CLMIB_RPOK.xe0              :            49,102,875         +49,102,875      14,203,303/s
 * CLMIB_RBYT.xe0              :         3,338,998,152      +3,338,998,152     965,824,676/s
 * CLMIB_RPROG0.xe0            :            49,102,994         +49,102,994      14,203,304/s
 * TDBGC4_64.xe1               :            12,299,897         +12,299,897       3,560,523/s
 * CLMIB_T127.xe1              :            12,299,999         +12,299,999       3,560,523/s
 * CLMIB_TPOK.xe1              :            12,300,007         +12,300,007       3,560,527/s
 * CLMIB_TPKT.xe1              :            12,300,008         +12,300,008       3,560,527/s
 * CLMIB_TUCA.xe1              :            12,300,010         +12,300,010       3,560,529/s
 * CLMIB_TVLN.xe1              :            12,300,021         +12,300,021       3,560,528/s
 * CLMIB_TBYT.xe1              :           836,401,428        +836,401,428     242,115,597/s
 * UC_PERQ_PKT(0).xe1          :            12,356,687         +12,356,687       3,561,310/s
 * UC_PERQ_BYTE(0).xe1         :           840,254,716        +840,254,716     242,169,072/s
 * TDBGC4_64.xe2               :            12,269,913         +12,269,913       3,547,952/s
 * CLMIB_T127.xe2              :            12,269,988         +12,269,988       3,547,909/s
 * CLMIB_TPOK.xe2              :            12,269,988         +12,269,988       3,547,909/s
 * CLMIB_TPKT.xe2              :            12,269,988         +12,269,988       3,547,909/s
 * CLMIB_TUCA.xe2              :            12,269,988         +12,269,988       3,547,909/s
 * CLMIB_TVLN.xe2              :            12,269,988         +12,269,988       3,547,909/s
 * CLMIB_TBYT.xe2              :           834,359,524        +834,359,524     241,258,131/s
 * UC_PERQ_PKT(0).xe2          :            12,325,722         +12,325,722       3,547,363/s
 * UC_PERQ_BYTE(0).xe2         :           838,149,096        +838,149,096     241,220,685/s
 * TDBGC4_64.xe3               :            12,260,927         +12,260,927       3,543,642/s
 * CLMIB_T127.xe3              :            12,260,994         +12,260,994       3,543,680/s
 * CLMIB_TPOK.xe3              :            12,260,994         +12,260,994       3,543,675/s
 * CLMIB_TPKT.xe3              :            12,260,994         +12,260,994       3,543,674/s
 * CLMIB_TUCA.xe3              :            12,260,994         +12,260,994       3,543,674/s
 * CLMIB_TVLN.xe3              :            12,260,994         +12,260,994       3,543,668/s
 * CLMIB_TBYT.xe3              :           833,747,592        +833,747,592     240,968,673/s
 * UC_PERQ_PKT(0).xe3          :            12,316,912         +12,316,912       3,543,601/s
 * UC_PERQ_BYTE(0).xe3         :           837,550,016        +837,550,016     240,964,882/s
 * TDBGC4_64.xe4               :            12,274,667         +12,274,667       3,551,176/s
 * CLMIB_T127.xe4              :            12,274,781         +12,274,781       3,551,197/s
 * CLMIB_TPOK.xe4              :            12,274,781         +12,274,781       3,551,189/s
 * CLMIB_TPKT.xe4              :            12,274,781         +12,274,781       3,551,189/s
 * CLMIB_TUCA.xe4              :            12,274,781         +12,274,781       3,551,189/s
 * CLMIB_TVLN.xe4              :            12,274,781         +12,274,781       3,551,177/s
 * CLMIB_TBYT.xe4              :           834,685,108        +834,685,108     241,479,082/s
 * UC_PERQ_PKT(0).xe4          :            12,331,126         +12,331,126       3,552,053/s
 * UC_PERQ_BYTE(0).xe4         :           838,516,568        +838,516,568     241,539,608/s
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
    {bcmSwitchEcmpDynamicQueuedBytesMinThreshold, 2080}, 
    {bcmSwitchEcmpDynamicQueuedBytesMaxThreshold, 208000}, 
    */
    {-1, 0}
};

/* Global DLB Configuraton */
bcm_error_t global_ecmp_dlb_config(int unit)
{
    int glb_ecmp_dlb_ethtypes[] = {0x0800, 0x86DD};
    int i, rv = 0;

    /* Sample Rate */
    rv = bcm_switch_control_set(unit, bcmSwitchEcmpDynamicSampleRate, 100000);  /* 16 us*/
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
       	   printf("\nError in setting DLB global parameter %d: %s.\n",i, bcm_errmsg(rv));
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
 * Create DLB ECMP group
 *     ecmp_intf_p (OUT) - DLB ECMP interface ID
 *     dynamic_age - the inactivity duration of the ECMP group
 *     dynamic_size - the flow table size for this ECMP group
 *     ecmp_count - # of egress objects in the ECMP group
 *     l3_eo_array - arrary of egress objects
 *     port_p, vlan_p - corresponding ports and VLANs
 *     loading_weight_p, queue_size_weight_p - array specifying the load and queue size weight
 *     scaling_factor_p - scaling factor for each ECMP member
 *
 *  NOTE:
 *     1) bcm_l3_egress_ecmp_create() is used to create ECMP group
 *     2) when specifying the array of port, vlan, EO, you can use port/vlan repeatedly
 *        to test the case that the same port is used in multiple nexthop
 */
int dlb_ecmp_create(int unit, int *ecmp_intf_p,
          int dynamic_age, int dynamic_size,
          int ecmp_count, int *port_p, bcm_vlan_t *vlan_p, bcm_if_t *l3_eo_array,
          int *loading_weight_p, int *queue_size_weight_p, int *scaling_factor_p)
{
    int rv = 0, i, flags;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_l3_egress_ecmp_t ecmp_group_info;
    int my_modid;

    BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &my_modid));

    /* Create L3 Interface and L3 Egress Object */
    for (i = 0; i < ecmp_count; i++) {
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
    }

    /*
     * Create L3 ECMP group with DLB attributes.
     *     Attributes -  DLB Mode, Size (flowset entries), Aging 
     */
    bcm_l3_egress_ecmp_t_init(&ecmp_group_info);
    ecmp_group_info.ecmp_intf = *ecmp_intf_p;
    ecmp_group_info.flags = (*ecmp_intf_p != -1) ? BCM_L3_WITH_ID: 0;
    ecmp_group_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL;
    ecmp_group_info.dynamic_size = dynamic_size;
    ecmp_group_info.dynamic_age = dynamic_age;
    ecmp_group_info.max_paths = 64;
    /* one member per port */
    rv = bcm_l3_egress_ecmp_create(unit, &ecmp_group_info, ecmp_count, l3_eo_array);
    if(BCM_FAILURE(rv)) {       
	     printf("\nFail to create ECMP: %s.\n",bcm_errmsg(rv));       
	     return rv;
	}

    /* Configure ECMP member, Link Status. This configuration has one member per port */
    for (i = 0; i < ecmp_count; i++) {
      BCM_IF_ERROR_RETURN(bcm_l3_egress_ecmp_member_status_set(unit, l3_eo_array[i],
                                    BCM_L3_ECMP_DYNAMIC_MEMBER_HW));
    }

    return(rv);
}

int unit = 0;
bcm_vlan_t vid_in = 12;
int port_in = 1;
bcm_l2_addr_t l2addr;
bcm_mac_t router_mac_in  = {0x00,0x00,0x00,0x00,0xAA,0xAA};
bcm_l3_route_t route_info;
bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

int dlb_ecmp_intf = 200256;
int app_ecmp_dynamic_age = 256; /* usec */
int app_ecmp_dynamic_size = 512; /* Flowset table size */

int ecmp_count = 4;
int app_ecmp_port[] = {2, 3, 4, 5};
bcm_vlan_t app_ecmp_vlan[] = {102, 103, 104, 105};
bcm_if_t l3_egress_object_p[] = {100002, 100003, 100004, 100005};
int app_ecmp_loading_weight[] = {50, 50, 50, 50};
int app_ecmp_queue_size_weight[] = {0, 0, 0, 0};
int app_ecmp_scaling_factor[] = {10, 10, 10, 10};


bcm_error_t test_setup(int unit)
{
  int rv = BCM_E_NONE;
  bcm_switch_control_set(unit, bcmSwitchUseGport, 1);
  bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);

  rv = ecmp_rtag7_config(unit);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to configure RTAG7: %s.\n",bcm_errmsg(rv));
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
     printf("\nFail to set DLB global configuration: %s.\n",bcm_errmsg(rv));
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
  /* Create ECMP and do DLB Configuraton */
  ecmp_count = sizeof(l3_egress_object_p)/sizeof(l3_egress_object_p[0]);
  rv = dlb_ecmp_create(unit, &dlb_ecmp_intf,
               app_ecmp_dynamic_age, app_ecmp_dynamic_size,
               ecmp_count, app_ecmp_port, app_ecmp_vlan, l3_egress_object_p,
               app_ecmp_loading_weight, app_ecmp_queue_size_weight, app_ecmp_scaling_factor);
  if(BCM_FAILURE(rv)) {
     printf("\nFail to create DLB ECMP: %s.\n",bcm_errmsg(rv));
     return rv;
  }

  /* Install the route for DIP, note this is routed out on the port_out */
  bcm_l3_route_t_init(&route_info);
  route_info.l3a_flags = BCM_L3_MULTIPATH;
  route_info.l3a_intf = dlb_ecmp_intf;
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
        printf("Fail to configure DLB ECMP %d\n", rrv);
        return rrv;
    }
    return BCM_E_NONE;
}
const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
