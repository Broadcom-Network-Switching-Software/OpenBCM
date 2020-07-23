/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_link_bonding.c

 */

enum device_type_t {
    device_type_qax=0,
    device_type_qux=1
} ;

int MAX_NUM_DEVICES = 568;
int SOC_NUM_PORTS = 30;
int MAX_MODIDS_PER_DEVICE = 2;
int MAX_COS = 8;

/* a simple version of cint_dpp_diag_init_s in $SDK/src/appl/diag/dcmn/init.c */
struct cint_dpp_diag_init_s {
    int         my_modid;
    int         my_modid_idx;
    int         base_modid;
    int         nof_devices;
    int         num_ports;
    int         num_cos;
    int         cpu_sys_port;
    int         offset_start_of_sys_port;
    int         offset_start_of_voq;
    int         offset_start_of_voq_connector;
    bcm_gport_t dest_local_gport[SOC_NUM_PORTS];
    bcm_gport_t dest_local_gport_nof_priorities[SOC_NUM_PORTS];   /* number of priorities per port */
    bcm_gport_t gport_ucast_voq_connector_group[MAX_NUM_DEVICES][MAX_MODIDS_PER_DEVICE][SOC_NUM_PORTS];
    bcm_gport_t gport_ucast_queue_group[MAX_NUM_DEVICES][SOC_NUM_PORTS];
    bcm_gport_t gport_ucast_scheduler[SOC_NUM_PORTS][MAX_COS];
    uint32 nof_active_cores;
    uint32 is_symmetric;
};

bcm_pbmp_t lbg_pbmp;
BCM_PBMP_CLEAR(lbg_pbmp);

int get_device_type(int unit, device_type_t *device_type)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
    printf("Error in bcm_info_get\n");
    print rv;
    return rv;
  }

  *device_type = (info.device == 0x8470) ? device_type_qax :
                 (info.device == 0x8270 || info.device == 0x8278) ? device_type_qux :
                 -1;

  return rv;
}

/*
  Only config rx modem map.
*/
int lbg_rx_mdm_map(int unit, bcm_port_t lbgLif,
                   bcm_lb_format_type_t lbg_type, bcm_vlan_t vlan,
                   bcm_modem_t modem_id)
{
  int lbgType=lbg_type;
  printf("args <unit=%d>, <lbgLif=%d>,<lbg_type=%d>,<vlan=%d>,<modem_id=%d>,\n",
         unit, lbgLif,lbgType,vlan,modem_id);

  int flags;
  flags = 0;
  bcm_lb_rx_modem_map_index_t map_index;
  bcm_lb_rx_modem_map_config_t map_config;
  bcm_lb_rx_modem_map_config_t map_config_get;
  map_index.port=lbgLif;
  map_index.vlan=vlan;
  map_config.modem_id=modem_id;
  print "bcm_lb_rx_modem_map_set";
  map_config.lbg_type=lbg_type;
  print bcm_lb_rx_modem_map_set(unit, map_index, flags, map_config);
  print bcm_lb_rx_modem_map_get(unit, map_index, flags, &map_config_get);
  print map_config;
  print map_config_get;

}

/*
  Enable the lbg and set the forward path for both tx and rx
*/
int lbg_enable_forward(int unit, bcm_lbg_t lbg_id, bcm_port_t normalLif,
                       bcm_port_t lbg_tm_port) {

  printf("args <unit=%d>, <lbg_id=%d>,<normalLif=%d>,<lbg_tm_port=%d>\n",
         unit, lbg_id,normalLif,lbg_tm_port);
  int flags;
  flags=0;
  /******Forward ******/
  print "bcm_lb_enable_set, bcm_port_force_forward_set";
  print bcm_lb_enable_set(unit, lbg_id, flags, FALSE);
  print bcm_lb_enable_set(unit, lbg_id, flags, TRUE);
  print bcm_port_force_forward_set(unit, lbg_tm_port, normalLif, TRUE);

  print bcm_port_force_forward_set(unit, normalLif, lbg_tm_port, TRUE);

  bshell(unit, "m EGQ_QPAIR_SPR_DIS.EGQ   QPAIR_SPR_DIS=0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  bshell(unit, "m EGQ_TCG_SPR_DIS.EGQ           TCG_SPR_DIS=0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  bshell(unit, "m EGQ_OTM_PORT_SPR_DIS.EGQ OTM_PORT_SPR_DIS=0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
}

/* Global var to mark whether the lbg port is already added. Use this flag
   because if the lbg port aleady exists, new port add would be failed, and
   this would also save a lot of time between different test cases.*/
int port_exist_flag=0;

int lbg_enable_set(int unit, bcm_lbg_t lbg_id) {
    print "enable lbg: bcm_lb_enable_set";
    int flags=0;
    print bcm_lb_enable_set(unit, lbg_id, flags, TRUE);
    return 0;
}
int lbg_disable_set(int unit, bcm_lbg_t lbg_id) {
    print "enable lbg: bcm_lb_enable_set";
    int flags=0;
    print bcm_lb_enable_set(unit, lbg_id, flags, FALSE);
    return 0;
}
/*
New defined port add for lbg
 */
int lbg_port_add(int unit, bcm_lbg_t lbg_id, bcm_port_t phy_port,
             bcm_port_t lbg_tm_port) {

  printf("args <unit=%d>, <lbg_id=%d>,<phyPort=%d>,<bg_tm_port=%d>\n",
         unit, lbg_id,phy_port,lbg_tm_port);

  uint32 flags = 0;
  bcm_port_interface_info_t interface_info_check;
  bcm_port_mapping_info_t mapping_info_check;
  int check_rv;
  check_rv=bcm_port_get(unit, lbg_tm_port, flags, &interface_info_check,
                        &mapping_info_check);

  if (!check_rv) {
    print interface_info_check;
    print "LBG port exists. Skip add";
    port_exist_flag=1;
    return 0;
  }

  /*Port Add Start */
  printf("Add tm port %d on phy_port %d", lbg_tm_port, phy_port);

  bcm_port_interface_info_t interface_info;
  bcm_port_mapping_info_t mapping_info;
  int port_priority = 8;
  flags =  BCM_PORT_ADD_BASE_Q_PAIR_WITH_ID | BCM_PORT_ADD_DONT_PROBE;
  interface_info.interface = BCM_PORT_IF_LBG;
  interface_info.interface_id = lbg_id;
  if (flags&BCM_PORT_ADD_USE_PHY_PBMP) {
    BCM_PBMP_PORT_ADD(interface_info.phy_pbmp, phy_port);
  }
  else {
    interface_info.phy_port = phy_port;
  }

  print "bcm_port_add";
  /* make the value different, not related with lbg_id itself*/
  mapping_info.base_q_pair = lbg_id*8;
  mapping_info.tm_port = lbg_tm_port;
  mapping_info.channel = 0;
  mapping_info.core = 0;
  mapping_info.num_priorities = port_priority;
  print interface_info;
  print mapping_info;
  print bcm_port_add(unit, lbg_tm_port, flags, &interface_info, &mapping_info);
  print interface_info;
  print mapping_info;

  bcm_port_t                        sys_port = 0;
  bcm_gport_t                       modport_gport, sysport_gport;

  BCM_GPORT_MODPORT_SET(modport_gport, 0, lbg_tm_port);
  sys_port = lbg_tm_port;
  BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
  print bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
  print sysport_gport;
  print modport_gport;
  BCM_PBMP_PORT_ADD(lbg_pbmp, lbg_tm_port);
  return 0;
}
int lbg_modem_port_add(int unit, bcm_port_t phy_port, bcm_port_t modem_port) {
    bcm_port_mapping_info_t     mapping_info;
    bcm_port_interface_info_t   interface_info;
    uint32 flags = BCM_PORT_ADD_MODEM_PORT;
    mapping_info.channel = 0;
    mapping_info.core = 0;
    mapping_info.tm_port = 0;
    mapping_info.num_priorities = 0;
    interface_info.phy_port = phy_port;
    interface_info.interface = BCM_PORT_IF_XFI;
    interface_info.num_lanes = 1;
    interface_info.interface_id = 0;
    BCM_PBMP_CLEAR(interface_info.phy_pbmp);
    print bcm_port_add(unit, modem_port, flags, &interface_info, &mapping_info);
}
int add_system_port(int unit,bcm_port_t port) {
    bcm_gport_t                 modport_gport, sysport_gport;
    bcm_port_t                  sys_port = 0;

    BCM_GPORT_MODPORT_SET(modport_gport, 0, port);
    sys_port = port;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);
    print bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
}

/*
  Only include the lbg map to modems
*/
int lbg_mdm_map_cfg(int unit, bcm_lbg_t lbg_id, bcm_modem_t modem_id,
                    bcm_lb_direction_type_t direction) {
  int lb_direction=direction;
  printf("args <unit=%d>, <lbg_id=%d>,<modem_id=%d>,<direction=%d>\n",
         unit, lbg_id,modem_id,lb_direction);

  bcm_lbg_t lbg_id_get;
  lbg_id_get = -1;
  int flags;
  flags = 0;
  print "connect modem to lbg: bcm_lb_modem_to_lbg_map_set";
  print bcm_lb_modem_to_lbg_map_set(unit, modem_id, direction, flags, lbg_id);
  print bcm_lb_modem_to_lbg_map_get(unit, modem_id, direction, flags,
                                    lbg_id_get);
  printf("expect<%d> get<%d>\n", lbg_id, lbg_id_get);

}

/*
  Only include the port map to modems
*/
int port_mdm_map_cfg(int unit, bcm_modem_t modem_id, bcm_port_t lbgLif) {

  printf("args <unit=%d>, <modem_id=%d>,<lbgLif=%d>,\n", unit, modem_id,lbgLif);

  /******Port Map Start******/
  bcm_port_t modem_port_get;

  print "connect specific modem to nif port: bcm_lb_modem_to_port_map_set";
  int flags;
  flags = 0;
  print bcm_lb_modem_to_port_map_set(unit, modem_id, flags, lbgLif);
  print bcm_lb_modem_to_port_map_get(unit, modem_id, flags, &modem_port_get);
  printf("expect<%d> get <%d>\n", lbgLif, modem_port_get);
}

/*
  Config the modem tx parameters
*/
int lb_mdm_pkt_cfg(int unit,
                      bcm_modem_t modem_id,
                      bcm_vlan_t outer_vlan_id,
                      int dstMac_Id,
                      bcm_lb_format_type_t lbg_type,
                      int use_global_priority_map)
{
    bcm_mac_t dst_mac_modems[16] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x33},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x34},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x35},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x36},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x37},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x38},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x39},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x3a},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x3b},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x3c},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x3d},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x3e},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x3f},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x40},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x41},
                                   {0x00, 0x00, 0x00, 0x00, 0x00, 0x42},
                                   };
    bcm_mac_t src_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    int flags = 0;
    bcm_lb_modem_packet_config_t mdm_packet_config;
    bcm_lb_modem_packet_config_t mdm_packet_config_get;
    mdm_packet_config.pkt_format=lbg_type;
    sal_memcpy(mdm_packet_config.dst_mac, dst_mac_modems[dstMac_Id], 6);
    mdm_packet_config.src_mac=src_mac;
    mdm_packet_config.vlan=outer_vlan_id;
    mdm_packet_config.use_global_priority_map=use_global_priority_map;

    print bcm_lb_modem_packet_config_set(unit, modem_id, flags, mdm_packet_config);
    print bcm_lb_modem_packet_config_get(unit, modem_id,flags,&mdm_packet_config_get);

    print mdm_packet_config;
    print mdm_packet_config_get;
}


int lb_pkt_cfg(int unit, uint16 outer_vlan_tpid, uint16 lbg_tpid)
{
    int flags = 0;
    bcm_lb_packet_config_t packet_config;
    bcm_lb_packet_config_t packet_config_get;
    packet_config.outer_vlan_tpid = outer_vlan_tpid;
    packet_config.lbg_tpid = lbg_tpid;
    print "bcm_lb_packet_config_set";
    print bcm_lb_packet_config_set(unit, flags, &packet_config);
    print bcm_lb_packet_config_get(unit, flags, packet_config_get);
    print packet_config;
    print packet_config_get;
}

int lb_ctrl_set(int unit,bcm_lbg_t lbg_id,
                bcm_port_t lbg_tm_port,bcm_lb_direction_type_t direction,
                bcm_lb_control_t lbCtrl, int value) {

  int flags;
  device_type_t dev_type;
  get_device_type(unit, &dev_type);

  flags = BCM_LB_FLAG_GLOBAL;
  if ((dev_type == device_type_qux) &&
      ((lbCtrl == bcmLbControlSegmentationMode) || (lbCtrl == bcmLbControlPacketCrcEnable))) {
    flags = 0;
  }

  print "configure segment mode: bcm_lb_control_set";
  print bcm_lb_control_set(unit, lbg_id, direction, flags, lbCtrl,
                           value);
}
/* Only include the lbg enable and forward path for tx side. */
int lbg_tx_ctrl_cfg(int unit, bcm_lbg_t lbg_id,bcm_port_t normalLif,
                    bcm_port_t lbg_tm_port,bcm_lb_control_t lbCtrl,
                    int value) {

  int flags;
  device_type_t dev_type;
  get_device_type(unit, &dev_type);
  flags = BCM_LB_FLAG_GLOBAL;
  if ((dev_type == device_type_qux) &&
      ((lbCtrl == bcmLbControlSegmentationMode) || (lbCtrl == bcmLbControlPacketCrcEnable))) {
    flags = 0;
  }
  print "configure segment mode: bcm_lb_control_set";
  print bcm_lb_control_set(unit, lbg_id, bcmLbDirectionTx, flags, lbCtrl,
                           value);

  print "bcm_lb_enable_set,bcm_port_force_forward_set";
  /******Forward ******/
  flags=0;
  print bcm_lb_enable_set(unit, lbg_id, flags, TRUE);

  print bcm_port_force_forward_set(unit, normalLif, lbg_tm_port, TRUE);

  bshell(unit, "m EGQ_QPAIR_SPR_DIS.EGQ   QPAIR_SPR_DIS=0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  bshell(unit, "m EGQ_TCG_SPR_DIS.EGQ           TCG_SPR_DIS=0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  bshell(unit, "m EGQ_OTM_PORT_SPR_DIS.EGQ OTM_PORT_SPR_DIS=0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

}
int connect_modem_to_port(int unit, bcm_modem_t modem_id, bcm_port_t modem_port,
                          bcm_vlan_t modem_vlan, bcm_lbg_t lbg_id) {
    /*(basic) Connect modem to nif port*/
    uint32 flags = 0;
    print bcm_lb_modem_to_port_map_set(unit, modem_id, flags, modem_port);
    /*(Ingress) mapping traffic to modem*/
    bcm_lb_rx_modem_map_index_t map_index;
    bcm_lb_rx_modem_map_config_t map_config;
    map_index.port = modem_port;
    map_index.vlan = modem_vlan;
    map_config.lbg_type = bcmLbFormatTypechannelize;
    map_config.modem_id = modem_id;
    print bcm_lb_rx_modem_map_set(unit, &map_index, flags, &map_config);
    /*(Igress) Connect modem to lbg*/
    bcm_lb_direction_type_t direction = bcmLbDirectionRx;
    print bcm_lb_modem_to_lbg_map_set(unit, modem_id, direction, flags, lbg_id);
    /*(Egress) Connect modem to lbg*/
    direction = bcmLbDirectionTx;
    print bcm_lb_modem_to_lbg_map_set(unit, modem_id, direction, flags, lbg_id);
}

int disconect_modem_from_port(int unit, bcm_modem_t modem_id, bcm_port_t modem_port,
                              bcm_vlan_t modem_vlan, bcm_lbg_t lbg_id) {
    /*(Egress) Disconnect modem from lbg*/
    bcm_lb_direction_type_t direction = bcmLbDirectionTx;
    uint32 flags = BCM_LB_FLAG_UNMAP;
    print bcm_lb_modem_to_lbg_map_set(unit, modem_id, direction, flags, lbg_id);
    /*(Ingress) Unmapping traffic to modem*/
    flags = 0;
    bcm_lb_rx_modem_map_index_t map_index;
    bcm_lb_rx_modem_map_config_t map_config;
    map_index.port = modem_port;
    map_index.vlan = modem_vlan;
    map_config.lbg_type = bcmLbFormatTypeBypass;
    map_config.modem_id = modem_id;
    print bcm_lb_rx_modem_map_set(unit, &map_index, flags, &map_config);
    /*(Igress) Disconnect modem from lbg*/
    direction = bcmLbDirectionRx;
    flags = BCM_LB_FLAG_UNMAP;
     print bcm_lb_modem_to_lbg_map_set(unit, modem_id, direction, flags, lbg_id);
    /*(basic) Disconnect modem from nif port*/
     print bcm_lb_modem_to_port_map_set(unit, modem_id, flags, modem_port);
}
/* Only include the shaper config */
int lbg_shaper_cfg(int unit,bcm_modem_t modem_id, uint32 enable,
                   uint32 rate, uint32 max_burst) {
  bcm_lb_modem_shaper_config_t shaperSet,shaperGet;
  int flags;
  flags=0;
  shaperSet.enable=enable;
  shaperSet.rate=rate;
  shaperSet.max_burst=max_burst;
  /*shaperSet.hdr_compensation=hdr_compensation;*/
  print "bcm_lb_modem_shaper_set";
  print bcm_lb_modem_shaper_set(unit,modem_id, flags,shaperSet);
  print bcm_lb_modem_shaper_get(unit,modem_id, flags,shaperGet);
  print shaperSet;
  print shaperGet;
}
int lb_tx_sched_set(int unit, bcm_lbg_t lbg_id) {
    uint32 flags = 0;
    int i,lbg_count,max_lbg_count=6;
    bcm_lb_lbg_weight_t lbg_weights[max_lbg_count];
    bcm_lb_tx_sched_get(unit,flags,max_lbg_count,lbg_weights,lbg_count);
    if ((lbg_count==1) && (lbg_weights[0].lbg_weight==16)) {
        lbg_count--;
    } else {
        for (i=0;i<max_lbg_count;i++) {
            if ((lbg_weights[i].lbg_id == lbg_id)&&(lbg_weights[i].lbg_weight = 1)) {
                print "lbg_id has been set";
                return 0;
            }
        }
    }
    /*bcm_lb_tx_sched_get will output the number of lbg_weight to lbg_count*/
    lbg_weights[lbg_count].lbg_id = lbg_id;
    lbg_weights[lbg_count].lbg_weight = 1;
    print "configure lbg scheduler:bcm_lb_tx_sched_set.";
    print bcm_lb_tx_sched_set(unit, flags, lbg_count+1, lbg_weights);
    return 0;
}
/* Only include the config for tc dp mapping*/
int lbg_tc_dp_prio_cfg(int unit,uint32 tc, uint32 dp, uint32 pkt_pri,
                       uint32 pkt_cfi, uint32 pkt_dp) {

  int flags=0;
  bcm_lb_tc_dp_t tcDpStruct;
  tcDpStruct.tc=tc;
  tcDpStruct.dp=dp;
  bcm_lb_pkt_pri_t pktStruct, pktStructGet;
  pktStruct.pkt_pri=pkt_pri;
  pktStruct.pkt_cfi=pkt_cfi;
  pktStruct.pkt_dp=pkt_dp;
  print "bcm_lb_tc_dp_to_priority_config_set";
  bcm_lb_tc_dp_to_priority_config_set(unit,flags,tcDpStruct,pktStruct);
  bcm_lb_tc_dp_to_priority_config_get(unit,flags,tcDpStruct,pktStructGet);
  print pktStruct;
  print pktStructGet;

}

/* Add the port to vlan, this is only used by anat case
   test_bcm_link_bonding_047 */
int vlan_port_add(int unit, uint16 vid, bcm_gport_t *port, int numPort) {
  bcm_pbmp_t pbmp;
  bcm_pbmp_t ubmp;
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(ubmp);
  int idx=0;
  for (idx=0;idx<numPort;idx++) {
    BCM_PBMP_PORT_ADD(pbmp, port[idx]);
    print port[idx];
  }
  print pbmp;
  print ubmp;
  print "bcm_vlan_port_add";
  print bcm_vlan_port_remove(unit, vid, pbmp);
  print bcm_vlan_port_add(unit, vid, pbmp, ubmp);
}

/*  Add l2 add to mac table, this is only used by anat case
    test_bcm_link_bonding_047 */
int l2_addr_add(int unit, bcm_mac_t mac, uint16 vid, bcm_gport_t port) {
  bcm_l2_addr_t l2addr;
  int rv;
  bcm_l2_addr_t_init(&l2addr, mac, vid);
  BCM_GPORT_LOCAL_SET(l2addr.port, port);
  print "bcm_l2_addr_add";
  rv = bcm_l2_addr_add(unit, &l2addr);
  if (rv != BCM_E_NONE) {
    printf("Error, bcm_l2_addr_add\n");
    return rv;
  }
  return BCM_E_NONE;
}

/* Add port to vlan and mac table, this is only used by anat case
   test_bcm_link_bonding_047 */
int l2_config(int unit, uint16 vid, bcm_gport_t *port, int numPort) {
  bcm_mac_t mac1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
  bcm_mac_t mac2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
  bcm_mac_t mac3 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
  print "vlan_port_add, l2_addr_add";
  print bcm_vlan_create(unit, vid);
  int idx=0;
  for (idx=0;idx<numPort;idx++) {
    print vlan_port_add(unit, vid, port, numPort);
  }
  print l2_addr_add(unit, mac1, vid, port[0]);
  print l2_addr_add(unit, mac2, vid, port[1]);
  print l2_addr_add(unit, mac3, vid, port[2]);

  return BCM_E_NONE;
}



/* Function:
 *      config_port_rate
 * Purpose:
 *      Reconfigure shapers
 * Note:
 *      This function is a copy of config_ports_rate in $SDK/src/examples/dpp/cint_dynamic_port_add_remove.c
 */
int config_port_rate(int unit, bcm_port_t port, int kbits_sec_max, int is_port_add)
{
    int                   rv;
    bcm_gport_t           fap_port;
    int                   tc_i;
    bcm_cosq_gport_info_t gport_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    int                   new_interface_speed, interface_rate;
    int                   port_phy_speed, is_channelized, egq_rate, sch_rate;
    uint32                dummy1 = 0,dummy2 = 0;
    uint32                flags, local_port_speed, kbits_sec_max_get;
    int                   LB_PORT_PHY_SPEED = 10000000;   /* 10Gbits */

    sch_rate = kbits_sec_max + (kbits_sec_max / 100) * 5; /* 5% speedup */
    egq_rate = kbits_sec_max + (kbits_sec_max / 100) * 1; /* 1% speedup */

    gport_info.cosq=0;
    gport_info.in_gport=port;

    printf("Start config_port_rate port(%d) kbits_sec_max(%d) is_port_add(%d)\n", port, kbits_sec_max, is_port_add);

    /* get port priorities */
    rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
            return rv;
    }

    is_channelized = (flags & BCM_PORT_ADD_CONFIG_CHANNELIZED) ? 1 : 0;

    if(mapping_info.num_priorities==8) {/*for 8 priorities ser TCG bandwidth*/
        /*EGQ TCG bandwidth*/
        rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPortTCG,gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port , gport_type %x, $rv\n", bcmCosqGportTypeLocalPortTCG);
            return rv;
        }
        rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,egq_rate,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
            return rv;
        }
        /*sch TCG bandwidth */
        if(is_port_add)
        {
            rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPortTCG,gport_info);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeE2EPortTCG);
                return rv;
            }
            rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,sch_rate,0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
                return rv;
            }
        }
    }
    /*EGQ TC bandwidth */
    rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPortTC,gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeLocalPortTC);
        return rv;
    }
    for (tc_i=0;tc_i<mapping_info.num_priorities;++tc_i)
    {/*for each tc set bandwidth*/
        rv = bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,tc_i,0,egq_rate,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, tc: %d,$rv\n",gport_info.out_gport,tc_i);
            return rv;
        }
    }
    /*sch TC bandwidth */
    if(is_port_add)
    {
        rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPortTC,gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeE2EPortTC);
            return rv;
        }
        for (tc_i=0;tc_i<mapping_info.num_priorities;++tc_i) {
            /*for each tc set bandwidth*/
            rv = bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,tc_i,0,sch_rate,0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, tc: %d,$rv\n",gport_info.out_gport,tc_i);
                return rv;
            }
        }
    }

    /*set EGQ bandwidth */
    rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,gport_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeLocalPort);
        return rv;
    }
    rv = bcm_cosq_gport_bandwidth_get(unit,gport_info.out_gport,0,&dummy1,&local_port_speed,&dummy2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_bandwidth_get,unit $unit fap gport: %x, $rv\n", fap_port);
        return rv;
    }
    /*set EGQ interface bandwidth*/
    rv = bcm_fabric_port_get(unit,gport_info.out_gport,0,&fap_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_fabric_port_get,unit $unit gport: %x, $rv\n",gport_info.out_gport);
        return rv;
    }

    /*set EGQ interface bandwidth: setting non-chanellized interface is done to set the whole calender, not to one interface*/
    /*translate to port rate*/
    rv = bcm_cosq_gport_bandwidth_get(unit,fap_port,0,&dummy1,&kbits_sec_max_get,&dummy2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_bandwidth_get,unit $unit fap gport: %x, $rv\n", fap_port);
        return rv;
    }


    if (is_port_add)
    {
        port_phy_speed = LB_PORT_PHY_SPEED;
        if (is_channelized) {
            if (interface_info.interface == BCM_PORT_IF_ILKN) {
                interface_rate = port_phy_speed*interface_info.num_lanes*1000;
            } else {
                interface_rate = port_phy_speed*1000;
            }
            if ((egq_rate + kbits_sec_max_get) <= (interface_rate /* maximal interface speed */)) {
                rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,(kbits_sec_max_get + egq_rate),0);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                    return rv;
                }
            }
            else
            {
                rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,(interface_rate /* maximal interface speed */),0);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                    return rv;
                }
            }
        }
    }
    /*set EGQ bandwidth */
    rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeLocalPort,gport_info);
    if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeLocalPort);
            return rv;
    }
    rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,egq_rate,0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
        return rv;
    }
    if (!is_port_add)
    {
        if (is_channelized) {
            new_interface_speed = kbits_sec_max_get - local_port_speed;
            new_interface_speed = (new_interface_speed<0) ? 0 : new_interface_speed;
            rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,new_interface_speed,0);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                return rv;
            }
        }
    }

    /*set sch bandwidth*/
    if(is_port_add)
    {
        rv=bcm_cosq_gport_handle_get(unit,bcmCosqGportTypeE2EPort,gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get,unit $unit port: $port ,gport_type %x, $rv\n", bcmCosqGportTypeE2EPort);
            return rv;
        }
        rv=bcm_cosq_gport_bandwidth_set(unit,gport_info.out_gport,0,0,sch_rate,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit gport: %x, $rv\n",gport_info.out_gport);
            return rv;
        }
    }

    /*set sch interface bandwidth*/
    if(is_port_add)
    {
        rv = bcm_fabric_port_get(unit,gport_info.out_gport,0,&fap_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_port_get,unit $unit gport: %x, $rv\n",gport);
            return rv;
        }

        port_phy_speed = LB_PORT_PHY_SPEED;
        if (is_channelized) {
            if (interface_info.interface == BCM_PORT_IF_ILKN) {
                interface_rate = port_phy_speed*interface_info.num_lanes*1000;
            } else {
                interface_rate = port_phy_speed*1000;
            }
            if ((kbits_sec_max_get + sch_rate) <= (interface_rate /* maximal interface speed */)) {
                rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,(kbits_sec_max_get + sch_rate),0);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                    return rv;
                }
            }
            else
            {
                rv = bcm_cosq_gport_bandwidth_set(unit,fap_port,0,0,(interface_rate /* maximal interface speed */),0);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_cosq_gport_bandwidth_set,unit $unit fap gport: %x, $rv\n", fap_port);
                    return rv;
                }
            }
        }
    }

    printf("End sucess config_port_rate\n");

    return BCM_E_NONE;
}

cint_dpp_diag_init_s   g_dii; /* global diag init info */

int
CORE_ID2INDEX(int core_id)
{
    return ((core_id == BCM_CORE_ALL) ? 0 : core_id);
}

/* Function:
 *      cint_dpp_convert_modport_to_sysport_id
 * Purpose:
 *      Convert (mod,port) id to sysport id
 * Note:
 *      This function is a copy of appl_dpp_convert_modport_to_sysport_id in $SDK/src/appl/diag/dcmn/init.c
 */
int cint_dpp_convert_modport_to_sysport_id(int modid_idx,int port)
{
    int internal_port_idx;

    /* Petra-B ITMH cannot be sent to system port 0, so we're using system port
               different than 0 instead (for CPU). ARAD can use system port 0 but we will do the same as Petra-B */
    if (port == 0) {
        port = g_dii.cpu_sys_port;
    }

    return g_dii.offset_start_of_sys_port * modid_idx + port;
}

/* Function:
 *      cint_dpp_convert_sysport_id_to_base_voq_id
 * Purpose:
 *      Convert sysport to base VOQ id
 * Note:
 *      This function is a copy of appl_dpp_convert_sysport_id_to_base_voq_id in $SDK/src/appl/diag/dcmn/init.c
 */
int cint_dpp_convert_sysport_id_to_base_voq_id(int sysport)
{
    /* Assume, no system port 0 */
    if (sysport == 0) {
        return -1;
    }
    return g_dii.offset_start_of_voq + (sysport-1)*g_dii.num_cos;
}

/* Function:
 *      cint_dpp_convert_modport_to_base_voq_connector
 * Purpose:
 *      Convert (local port,ingress_modid) to VOQ connector base
 * Note:
 *      This function is a copy of appl_dpp_convert_modport_to_base_voq_connector in $SDK/src/appl/diag/dcmn/init.c
 */
int cint_dpp_convert_modport_to_base_voq_connector(int local_port, int core_id, int ingress_modid)
{
    int voq_conn_id, num_of_512, voq_conn_internal_id;

    voq_conn_id = g_dii.offset_start_of_voq_connector + local_port * (g_dii.nof_devices * MAX_MODIDS_PER_DEVICE + g_dii.base_modid) * MAX_COS
        + ingress_modid * MAX_COS;

    /*
     * For dual core support (SYMMETRIC MODE)
     * Each 1024 connectors are devided to 512 connected to core 0 and 512 connected to core 1
     * so this map remap connectors to valid range in symmetric mode
     */
    num_of_512 = voq_conn_id / 512;
    voq_conn_internal_id = voq_conn_id % 512;
    voq_conn_id = (num_of_512 * 1024) + (CORE_ID2INDEX(core_id) * 512) + voq_conn_internal_id;

    return voq_conn_id;
}

/* Function:
 *      cint_dpp_convert_port_to_remote_modid_port
 * Purpose:
 *      Convert local port to an example of Remote XGS port and XGS Modid
 *      See convert details in function
 * Note:
 *      This function is a copy of appl_dpp_convert_port_to_remote_modid_port in $SDK/src/appl/diag/dcmn/init.c
 */
void cint_dpp_convert_port_to_remote_modid_port(int unit, int local_port, int* remote_modid, int* remote_port)
{
    /*
     * Remote Modid 0 <=> local_port 0-63
     * Remote Modid 1 <=> local_port 64-127
     * Remote Modid 2 <=> local_port 128-191
     * Remote Modid 3 <=> local_port 192-255
     */
    *remote_modid = (local_port >> 6);

    /* Remote Port = local_port first 6 bits */
    *remote_port =  (local_port & 0x3F);
}
int cint_port_init(int unit)
{
    print cint_dpp_appl_init();
    print cint_port_stk_init(unit);
    print cint_port_cosq_init(unit);
}
/* Function:
 *      cint_port_stk_init
 * Note:
 *      This function is a simple version of appl_dpp_stk_diag_init in $SDK/src/appl/diag/dcmn/init.c
 */
int cint_port_stk_init(int unit)
{
    int rv = BCM_E_NONE;
    int idx, port, sys_port, modid_idx;
    bcm_gport_t modport_gport,sysport_gport;
    uint32 dummy_flags;
    bcm_pbmp_t pbmp;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    bcm_port_t tm_port = 0;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_OR(pbmp, lbg_pbmp);
    BCM_PBMP_COUNT(pbmp, g_dii.num_ports);

    idx = 0;
    BCM_PBMP_ITER(pbmp, port) {
        /* initialize local ports array */
        BCM_GPORT_LOCAL_SET(g_dii.dest_local_gport[idx],port);

        /* per local port, indicate port number of priorities */
        rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
        if (rv < 0) {
            printf("unit %d, bcm_port_get failed. Error:%d\n", unit, rv);
            return rv;
        }

        g_dii.dest_local_gport_nof_priorities[idx] = mapping_info.num_priorities;
        idx++;
    }

    /*
     * Creation of system ports in the system
     * Iterate over port + internal ports. After the iteration, num_ports will
     *  be incremented by internal_num_ports
     * Assuming each device has the same num ports + internal ports
     */
    for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
        BCM_PBMP_ITER(pbmp, port) {
            /* System port id depends on modid + port id */
            sys_port = cint_dpp_convert_modport_to_sysport_id(modid_idx, port);

            if (bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info) < 0) {
                printf("unit %d, Invalid local port %d\n", unit, port);
                return BCM_E_PARAM;
            }

            tm_port = mapping_info.tm_port;
            BCM_GPORT_MODPORT_SET(modport_gport, modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid + mapping_info.core, tm_port);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);

            rv = bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
            if (rv < 0) {
                printf("unit %d, bcm_stk_sysport_gport_set(%d, %d, %d) failed.\n", unit, sysport_gport, modport_gport, rv);
                return rv;
            }
        }
    }

    printf("unit(%d) g_dii.num_ports is %d \n", unit, g_dii.num_ports);
    return rv;
}

/* Function:
 *      cint_dpp_cosq_diag_credit_request_thresholds_config
 * Purpose:
 *      Configure queue bundle gport according to port.
 * Note:
 *      This function is a copy of appl_dpp_cosq_diag_credit_request_thresholds_config in $SDK/src/appl/diag/dcmn/init.c
 */
int cint_dpp_cosq_diag_credit_request_thresholds_config(int unit, int flags, bcm_port_t port, bcm_gport_t modport_gport, bcm_port_interface_info_t *interface_info, bcm_cosq_ingress_queue_bundle_gport_config_t *config)
{
    int rv = BCM_E_NONE;
    int cosq;
    int default_delay_tolerance_level;
    int speed;

    if (interface_info->interface == BCM_PORT_IF_CPU || BCM_PBMP_IS_NULL(interface_info->phy_pbmp)) {
        speed = -1;
    } else if (interface_info->interface == BCM_PORT_IF_LBG) {
        speed = 10000;
    } else {
        rv = bcm_port_speed_get(unit, port, &speed);
        if (BCM_FAILURE(rv)) {
            printf("unit %d, bcm_port_speed_get port(%d) failed. Error:%d \n", unit, port, rv);
           return rv;
        }
        if (interface_info->interface == BCM_PORT_IF_ILKN) {
            speed *= interface_info->num_lanes;
        }
    }
    /* Set credit request profile according to port's speed */
    if (speed <= 1000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_1G;
    } else if (speed <= 10000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
    } else if (speed <= 40000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_40G_SLOW_ENABLED;
    } else if (speed <= 100000) {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_100G_SLOW_ENABLED;
    } else {
        default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_200G_SLOW_ENABLED;
    }
    config->flags = flags;
    config->port = modport_gport;
    config->local_core_id = BCM_CORE_ALL;
    config->numq = g_dii.num_cos;
    for (cosq = 0 ; cosq < BCM_COS_COUNT; cosq++) {
        /*setting all of the credit request profiles to default slow enabled mode (corresponds to port speed)*/
        config->queue_atrributes[cosq].delay_tolerance_level = default_delay_tolerance_level;
        /*setting all of the rate classes zero*/
        config->queue_atrributes[cosq].rate_class = 0;
    }

    return rv;
}

/* Function:
 *      cint_port_cosq_init
 * Note:
 *      This function is a simple version of appl_dpp_cosq_diag_init in $SDK/src/appl/diag/dcmn/init.c
 */
int cint_port_cosq_init(int unit)
{
    int                         rv = BCM_E_NONE;
    int                         idx, cos, port;
    uint32                      flags, dummy_flags;
    bcm_cosq_gport_connection_t connection;
    int modid_idx, sysport_id, voq_base_id, voq_connector_id;
    bcm_gport_t voq_connector_gport, sysport_gport, modport_gport, voq_base_gport, e2e_gport;
    bcm_cos_t tc_i;
    bcm_cos_queue_t to_cosq;
    int int_flags, core_id, is_pdm_wide_mode, is_hdr_comp_tag_exists;
    bcm_gport_t egress_q_gport;
    bcm_port_config_t port_config;
    int egress_port, my_domain, domain;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_ingress_queue_bundle_gport_config_t ingress_config;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    bcm_pbmp_t                pbmp;
    bcm_cosq_voq_connector_gport_t config;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;
    bcm_port_t tm_port = 0;

    {
        /*
         * Creating Scheduling Scheme
         * This is done with the following stages:
         * 1. Egress: Create for each local port: following FQs, VOQ connectors.
         * 2. Ingress: Create VOQs for each system port.
         * 3. Connect Ingress VOQs <=> Egress VOQ connectors.
         * Pay attention the scheduling scheme assumes static IDs for VOQ connectors,VOQs.
         * This is depended by the local and system ports in the system.
         * Conversion is done static using utils functions.
         */

        /* Stage I: Egress change HR to enhanced mode */
        for (idx = 0; idx < g_dii.num_ports; idx++) {
            /* Replace HR mode to enhance */
            flags = (BCM_COSQ_GPORT_SCHEDULER |
                    BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED |
                     BCM_COSQ_GPORT_REPLACE);

            /* e2e gport */
            BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]));
            rv = bcm_cosq_gport_add(unit, g_dii.dest_local_gport[idx], 1, flags,
                                    &e2e_gport);

            if (rv < 0) {
                printf("unit %d, bcm_cosq_gport_add replace hr scheduler idx:%d failed. \n", unit, idx, rv);
                return rv;
            }
        }
        printf("after Egress change HR to enhanced mode\n");

        /* Stage I: Egress Create VOQ connectors */
        for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
            for (idx = 0; idx < g_dii.num_ports; idx++) {
                for (core_id = 0; core_id < g_dii.nof_active_cores; core_id++) {
                    /* create voq connector */
                    if (g_dii.base_modid == 0) {
                        flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;
                    } else {
                        flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
                    }
                    port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);

                    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
                    if (rv < 0) {
                        printf("unit %d, bcm_port_get failed. Error:%d \n", unit, rv);
                        return rv;
                    }

                    if (g_dii.base_modid == 0) {
                        voq_connector_id = cint_dpp_convert_modport_to_base_voq_connector(port, core_id, modid_idx * MAX_MODIDS_PER_DEVICE);
                        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], voq_connector_id, mapping_info.core);
                    }

                    config.flags = flags;
                    config.nof_remote_cores = (g_dii.is_symmetric ? MAX_MODIDS_PER_DEVICE : 1);
                    config.numq = g_dii.num_cos;
                    config.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid;
                    BCM_COSQ_GPORT_E2E_PORT_SET(config.port, port);

                    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx]);
                    if (rv < 0) {
                        printf("unit %d, bcm_cosq_gport_add connector port:%d failed. \n", unit, port, rv);
                        return rv;
                    }

                    for (cos = 0; cos < g_dii.num_cos; cos++) {
                        /* Each VOQ connector attach suitable HR */
                        rv = bcm_cosq_gport_sched_set(unit,
                                     g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], cos,
                                     BCM_COSQ_SP0,0);
                        if (rv < 0) {
                            printf("unit %d, bcm_cosq_gport_sched_set connector port:%d cos:%d failed %d.\n", unit, port, cos, rv);
                            return rv;
                        }

                        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, g_dii.dest_local_gport[idx]);
                        gport_info.cosq = (g_dii.dest_local_gport_nof_priorities[idx] == 8 ? cos : 0);
                        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);
                        if (rv < 0) {
                            printf("unit %d, bcm_cosq_gport_handle_get failed %d. \n", unit,
                                                   gport_info.in_gport, rv);
                            return rv;
                        }

                        /* attach HR SPi to connecter */
                        rv = bcm_cosq_gport_attach(unit, gport_info.out_gport,
                                         g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], cos);
                        if (rv < 0) {
                            printf("unit %d, bcm_cosq_gport_attach fq scheduler-connector port:%d cos:%d failed. Error:%d \n", unit, port, cos, rv);
                            return rv;
                        }

                        /* Set Slow Rate to be bcmCosqControlFlowSlowRate1 */
                        rv = bcm_cosq_control_set(unit, g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx], cos, bcmCosqControlFlowSlowRate, 1);
                        if (rv < 0) {
                            printf("unit %d, bcm_cosq_control_set bcmCosqControlFlowSlowRate connector port:%d cos:%d failed. Error:%d \n", unit, port, cos, rv);
                            return rv;
                        }
                    }

                    if (g_dii.is_symmetric) {
                        int nof_active_cores = g_dii.nof_active_cores;
                        for (core_id++; core_id < nof_active_cores; core_id++) {
                            g_dii.gport_ucast_voq_connector_group[modid_idx][core_id][idx] = g_dii.gport_ucast_voq_connector_group[modid_idx][0][idx];
                        }
                    }
                }
            }
        }
    }
	printf("after Egress Create VOQ connectors\n");

    /* Stage 2: Ingress: Create VOQs for each system port. */
    for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
        for (idx = 0; idx < g_dii.num_ports; idx++) {

            port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);

            if (bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info) < 0) {
                printf("unit %d, Invalid local port %d\n", unit, port);
                return BCM_E_PARAM;
            }

            tm_port = mapping_info.tm_port;
            BCM_GPORT_MODPORT_SET(modport_gport, modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid + mapping_info.core, tm_port);

            rv = bcm_stk_gport_sysport_get(unit,modport_gport,&sysport_gport);
            if (rv < 0) {
               printf("unit %d, bcm_stk_gport_sysport_get get sys port failed. Error:%d \n", unit, rv);
               return rv;
            }

            sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);
            voq_base_id = cint_dpp_convert_sysport_id_to_base_voq_id(sysport_id);

            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(g_dii.gport_ucast_queue_group[modid_idx][idx],voq_base_id);

            /* Map sysport_gport to voq_base_id. Could have used sysport_gport instead of modport_gport in the function call below */
            flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;

            /* Configure credit request profile for the port */
            rv = cint_dpp_cosq_diag_credit_request_thresholds_config(unit, flags, port, modport_gport, &interface_info, &ingress_config);
            if (rv < 0) {
                printf("unit %d, Failed to configure credit request profile for port(%d). Error:%d \n", unit, port, rv);
               return rv;
            }

            /* Map the port with the credit request profile */
            rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &ingress_config, &g_dii.gport_ucast_queue_group[modid_idx][idx]);
            if (rv < 0) {
                printf("unit %d, bcm_cosq_ingress_queue_bundle_gport_add UC queue failed. Error:%d\n", unit, rv);
               return rv;
            }

            is_pdm_wide_mode = soc_property_get(unit, "bcm886xx_pdm_mode", 0);
            is_hdr_comp_tag_exists = soc_property_get(unit, "ingress_congestion_management_pkt_header_compensation_enable", 0);

            /* Ingress compensation configuration */
			my_domain = 0;
            domain = 0;

            if(domain == my_domain) {
                if ((is_hdr_comp_tag_exists) || (is_pdm_wide_mode && is_hdr_comp_tag_exists)) /* new mechanism */
                {
                    int comp_value;
                    /* Constant compensation value should be 20 bytes for Preamble and IPG + 4 bytes for CRC = 24 bytes
                       In devices before QAX, we decrease 2 bytes for internal CRC (20+4-2 = 22).
                       In QAX, these 2 bytes are encountered as  part of Header Delta compensation */
                    comp_value = 24;
                    for (tc_i = 0; tc_i < g_dii.num_cos; tc_i++) { /* set compensation at ingress to +22 */
                        adjust_info.cosq = tc_i;
                        adjust_info.flags = 0;
                        adjust_info.gport = g_dii.gport_ucast_queue_group[modid_idx][idx];
                        adjust_info.source_info.source_id = 0; /* irrelevant for scheduler */
                        adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceScheduler; /* scheduler type */
                        rv = bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, comp_value);
                        if (rv < 0) {
                            printf("unit %d, bcm_cosq_gport_pkt_size_adjust_set failed. Error:%d\n", unit, rv);
                            return rv;
                        }
                    }
                }
                else /* Old mechanism */
                {
                    for (tc_i = 0; tc_i < g_dii.num_cos; tc_i++) { /* set compensation at ingress to +2 */
                        rv = bcm_cosq_control_set(unit, g_dii.gport_ucast_queue_group[modid_idx][idx], tc_i, bcmCosqControlPacketLengthAdjust, 2);
                        if (rv < 0) {
                            printf("unit %d, bcm_cosq_control_set failed in setting compensation. Error:%d \n", unit, rv);
                           return rv;
                        }
                    }
                }
            }

        }
    }
	printf("after Creating VOQs per port\n");

    {
        /* Stage 3: Connect Ingress VOQs <=> Egress VOQ connectors. */
        /* Ingress: connect voqs to voq connectors */
        for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
            for (idx = 0; idx < g_dii.num_ports; idx++) {
                for (core_id = (g_dii.is_symmetric ? BCM_CORE_ALL : 0);
                     core_id < (g_dii.is_symmetric ? BCM_CORE_ALL + 1: g_dii.nof_active_cores);
                     core_id++){
                    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
                    connection.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid;
                    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(
                       connection.voq, core_id, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(g_dii.gport_ucast_queue_group[modid_idx][idx]));
                    port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);

                    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
                    if (rv < 0) {
                        printf("unit %d, bcm_port_get failed. Error:%d\n",
                                   unit, rv);
                        return rv;
                    }

                    if (g_dii.base_modid == 0) {
                        voq_connector_id = cint_dpp_convert_modport_to_base_voq_connector(port, core_id, g_dii.my_modid);
                        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(voq_connector_gport, voq_connector_id, mapping_info.core);

                        connection.voq_connector = voq_connector_gport;
                    } else {
                        connection.voq_connector = g_dii.gport_ucast_voq_connector_group[g_dii.my_modid_idx][CORE_ID2INDEX(core_id)][idx];
                    }

                    rv = bcm_cosq_gport_connection_set(unit, &connection);
                    if (rv < 0) {
                        printf("unit %d, bcm_cosq_gport_connection_set ingress modid_idx: %d, port:%d failed. Error:%d \n", unit, modid_idx, port, rv);
                        return rv;
                    }
                }
            }
        }
        printf("after connecting VOQs to connectors in ingress\n");

        /* Egress: connect voq connectors to voqs */
        for (modid_idx = 0; modid_idx < g_dii.nof_devices; modid_idx++) {
            for (idx = 0; idx < g_dii.num_ports; idx++) {
                connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
                connection.remote_modid = modid_idx * MAX_MODIDS_PER_DEVICE + g_dii.base_modid;
                port = BCM_GPORT_LOCAL_GET(g_dii.dest_local_gport[idx]);

                if (bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info) < 0) {
                    printf("unit %d, Invalid local port %d\n", unit, port);
                    return BCM_E_PARAM;
                }

				tm_port = mapping_info.tm_port;
                BCM_GPORT_MODPORT_SET(modport_gport, g_dii.my_modid + mapping_info.core, tm_port);
                rv = bcm_stk_gport_sysport_get(unit,modport_gport,&sysport_gport);
                if (rv < 0) {
                    printf("unit %d, bcm_stk_gport_sysport_get get sys port failed. Error:%d \n", unit, rv);
                   return rv;
                }

                sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport);

                for (core_id = (g_dii.is_symmetric ? BCM_CORE_ALL : 0);
                     core_id < (g_dii.is_symmetric ? BCM_CORE_ALL + 1 : g_dii.nof_active_cores);
                     core_id++) {
                    connection.voq_connector = g_dii.gport_ucast_voq_connector_group[modid_idx][CORE_ID2INDEX(core_id)][idx];

                    voq_base_id = cint_dpp_convert_sysport_id_to_base_voq_id(sysport_id);
                    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_base_gport, core_id, voq_base_id);

                    connection.voq = voq_base_gport;

                    rv = bcm_cosq_gport_connection_set(unit, &connection);
                    if (rv < 0) {
                        printf("unit %d, bcm_cosq_gport_connection_set egress modid_idx: %d, port:%d failed. Error:%d \n", unit, modid_idx, port);
                        return rv;
                    }
                }
            }
        }
        printf("after connecting VOQs to connectors in egress\n");

    }

	BCM_PBMP_CLEAR(lbg_pbmp);

    return rv;
}

/* Function:
 *      cint_port_appl_init
 * Purpose:
 *      init variable in cint_dpp_diag_init_s.
 * Note:
 *      This function is a simple version of appl_dpp_misc_diag_init in $SDK/src/appl/diag/dcmn/init.c
 */
int cint_dpp_appl_init()
{
    g_dii.my_modid       = 0;
    g_dii.my_modid_idx   = 0;
    g_dii.base_modid     = 0;
    g_dii.nof_devices    = 1;
    g_dii.num_ports      = 16;
    g_dii.num_cos        = 8;
    g_dii.cpu_sys_port   = 255;
    g_dii.offset_start_of_sys_port          = 256;
    g_dii.offset_start_of_voq               = 32;
    g_dii.offset_start_of_voq_connector     = 32;
    g_dii.nof_active_cores = 1;
    g_dii.is_symmetric     = 1;

	return BCM_E_NONE;
}
