/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * File: cint_vlan_port_initial_vid.c
 * Purpose: Examples of initial-VID.
 *          - By default, bcm API differs between tagged packets and untagged packets by Initial-VID database. 
 *          - CINT introduce per port decision if to act as default or always to use Initial-VID with no difference
 *            between untagged packets and tagged packets. Database used in this case are match MATCH_PORT_VLAN and
 *            MATCH_PORT_VLAN_VLAN (no use for MATCH_PORT_INITIAL_VLAN).
 *
 * Calling sequence:
 *
 * Initialization:
 *   1. Add the following port configureations to config-sand.bcm, so use Initial-VID with no difference
 *      between untagged packets and tagged packets.
 *      vlan_translation_initial_vlan_enable_<port>=0
 *
 *   2. Set differnce vlan domain per port. 
 *      - call examples_initial_vid_init(unit, port1, port2, port3);
 *
 *   3. Set VT profile as double tag priority initial vid.
 *      - call examples_initial_vid_set(unit, in_port, enable);
 *
 *   4. create a pp model with double tag AC
 *      - vswitch_metro_mp_run_with_defaults(unit, port1, port2, port3);
 *
 *   5. create a single tag AC, and connect with vsi.
 *      - examples_initial_vid_setup_lif(unit, in_port, vlan, vsi, 0);
 *
 *   6. create a initial vid AC for untag packet, and connect with vsi.
 *      - examples_initial_vid_setup_lif(unit, in_port, vlan, vsi, 1);
 *
 *   7. send double tag(10, 20), single tag(10), untag packet, can receive it in port 16.
 *      Port 15 outer vid 10 inner vid 20 ---> vsi ---> Port 16 outer vid 3 inner vid 6
 *              outer vid 10              <---  | <---       outer vid 5 inner vid 2
 *              untag                     <---  | <---       outer vid 5 inner vid 2
 *              outer vid 30 inner vid 60 <---  | <---       outer vid 5 inner vid 2
 *        - From port 15:
 *              - double tag
 *                - ethernet header with any DA, SA 
 *                - outer vid 10
 *                - inner vid 20
 *              - single tag
 *                - ethernet header with any DA, SA 
 *                - outer vid 10
 *              - untag
 *                - ethernet header with any DA, SA
 *
 *        - From port 16:
 *              -   ethernet header with any DA, SA
 *              -   outer vid 5
 *              -   inner vid 2
 *
 * To Activate Above Settings:
 *      BCM> cint src/examples/dpp/cint_port_tpid.c
 *      BCM> cint src/examples/dpp/cint_vlan_port_initial_vid.c
 *      BCM> cint src/examples/dpp/cint_l2_mact.c
 *      BCM> cint src/examples/dpp/cint_vswitch_metro_mp.c
 *      BCM> cint src/examples/dpp/cint_multi_device_utils.c
 *      BCM> cint
 *      cint> 
 *      cint> print examples_initial_vid_init(unit, port1, port2, port3);
 *      cint> print examples_initial_vid_set(unit, in_port, enable);
 *      cint> print vswitch_metro_mp_run_with_defaults(unit, port1, port2, port3);
 *      cint> print examples_initial_vid_setup_lif(unit, in_port, vlan, vsi, 0);
 *      cint> print examples_double_lookup_initial_vid_set(unit, in_port, vlan, vsi, 1);
 *
 * 
 * Notes: Please see cint_vswitch_metro_mp.c to know pp model.
 */

/* Set differnce vlan domain per port */
int examples_initial_vid_init(int unit, int init_vid_port, int port2, int port3)
{
  int rv  = 0;

  rv = bcm_port_class_set(unit, init_vid_port, bcmPortClassId, init_vid_port);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, init_vid_port, rv);
      return rv;
  }

  rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, port2, rv);
      return rv;
  }

  rv = bcm_port_class_set(unit, port3, bcmPortClassId, port3);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_class_set unit %d, port %d rv %d\n", unit, port3, rv);
      return rv;
  }

  return rv;

}

/* Create a single tag AC, or a initial-vid AC for untag packet */
int examples_initial_vid_setup_lif(int unit, int in_port, int vlan, int vsi, int is_init_vid_ac)
{
  bcm_vlan_port_t pvid_map;
  int rv  = 0;

  if (is_init_vid_ac)
  {
    rv =  bcm_port_untagged_vlan_set(unit, in_port, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_untagged_vlan_set unit %d, in_port %d, vid %d, rv %d\n", unit, in_port, vlan, rv);
        return rv;
    }
  }

  bcm_vlan_port_t_init(&pvid_map);
  if (is_init_vid_ac)
  {
    pvid_map.criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
  }
  else
  {
    pvid_map.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  }
  pvid_map.match_vlan = vlan;
  pvid_map.vsi = 0;
  pvid_map.port = in_port;
  rv = bcm_vlan_port_create(unit, &pvid_map);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_port_create unit %d, in_port %d, vid %d, vsi:%d rv %d\n", unit, in_port, vlan, vsi, rv);
      return rv;
  }

  rv = vswitch_add_port(unit, vsi,in_port, pvid_map.vlan_port_id);
  if (rv != BCM_E_NONE) {
      printf("Error, vswitch_add_port\n");
      return rv;
  }

  return rv;
}

/* Set VT profile as double tag priority initial vid */
int examples_double_lookup_initial_vid_set(int unit, int init_vid_port, int enable)
{
  int rv  = 0;

  rv = bcm_vlan_control_port_set(unit, init_vid_port, bcmVlanPortDoubleLookupEnable, enable); 
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_control_port_set unit %d, port %d rv %d\n", unit, init_vid_port, rv);
      return rv;
  }

  return rv;
}

int cint_vlan_port_learning_info_overwrite (int unit, int port)
{
    bcm_failover_t eg_in_failover_id;
    bcm_failover_t fec_failover_id;
    bcm_vlan_port_t vp2;
    bcm_vlan_port_t vp3;
    bcm_vlan_port_t vp4;
    int mcast_group = 10000;
    int vpn = 8000;
    bcm_if_t encap_id=0;
    bcm_vlan_control_vlan_t control_vlan;
    bcm_vlan_control_vlan_t_init(&control_vlan);
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    bcm_error_t rv=0;
    bcm_l3_egress_t egr;
    bcm_if_t if_id;
    int flags = 0;
    int egress_vlan = 513;
    int egress_port = 202;
    int egress_vlan2 = 512;

    rv = bcm_vxlan_init(unit);
    if (rv != BCM_E_NONE) {
          printf("Error, bcm_vxlan_init \n");
    };

    bcm_failover_create (0,BCM_FAILOVER_ENCAP,eg_in_failover_id);
    bcm_failover_create(unit, ( BCM_FAILOVER_FEC), &fec_failover_id);

    flags = BCM_MULTICAST_WITH_ID;
    /* create ingress/egress MC */
    flags |= BCM_MULTICAST_INGRESS_GROUP;

    rv = bcm_multicast_create(unit, flags, mcast_group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }
    vpn_config.flags = BCM_VXLAN_VPN_ELAN|BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = mcast_group;
    vpn_config.unknown_unicast_group = mcast_group;
    vpn_config.unknown_multicast_group = mcast_group;
    rv = bcm_vxlan_vpn_create(unit,&vpn_config);

    bcm_vlan_create (0,egress_vlan);
    bcm_vlan_control_vlan_get (0, egress_vlan, &control_vlan);

    control_vlan.unknown_unicast_group = egress_vlan;
    control_vlan.unknown_multicast_group = egress_vlan;
    control_vlan.broadcast_group = egress_vlan;
    bcm_vlan_control_vlan_set(unit, egress_vlan, control_vlan);
    bcm_vlan_gport_add(unit, egress_vlan, egress_port, 0);

    vp4.criteria=BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp4.port=202;
    vp4.match_vlan=egress_vlan;
    vp4.egress_vlan=egress_vlan;
    vp4.inlif_counting_profile = 2;
    vp4.outlif_counting_profile =0;

    bcm_vlan_port_create(0,&vp4);
    bcm_vswitch_port_add(unit, vpn, vp4.vlan_port_id);
    bcm_multicast_vlan_encap_get (unit, mcast_group, egress_port, vp4.vlan_port_id, &encap_id);
    bcm_multicast_ingress_add (unit, mcast_group, egress_port, encap_id);
    bcm_vlan_create (0,egress_vlan2);
    bcm_vlan_control_vlan_get (0, egress_vlan2, &control_vlan);

    control_vlan.unknown_unicast_group = egress_vlan2;
    control_vlan.unknown_multicast_group = egress_vlan2;
    control_vlan.broadcast_group = egress_vlan2;
    bcm_vlan_control_vlan_set(unit, egress_vlan2, control_vlan);

    rv = bcm_vlan_gport_add(unit, egress_vlan2, egress_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_gport_add.\n");
        return rv;
    }
    vp3.criteria=BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp3.port=egress_port;
    vp3.match_vlan=egress_vlan2;
    vp3.egress_vlan=egress_vlan2;
    vp3.inlif_counting_profile = 2;
    vp3.outlif_counting_profile =0;

    bcm_vlan_port_create(0,&vp3);
    bcm_vswitch_port_add(unit, vpn, vp3.vlan_port_id);
    bcm_multicast_vlan_encap_get (unit, mcast_group, egress_port, vp3.vlan_port_id, &encap_id);
    bcm_multicast_ingress_add (unit, mcast_group, egress_port, encap_id);

    egr.port     = 202;
    egr.mpls_label = BCM_MPLS_LABEL_INVALID;
    if_id = BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET (0x4440100e);
    if_id = if_id +1;
    egr.failover_id = fec_failover_id;
    bcm_l3_egress_create (unit, (BCM_L3_INGRESS_ONLY|BCM_L3_WITH_ID), &egr, &if_id);

    egress_vlan = 511;
    bcm_vlan_create (0,egress_vlan);
    bcm_vlan_control_vlan_get (0, egress_vlan, &control_vlan);

    control_vlan.unknown_unicast_group = egress_vlan;
    control_vlan.unknown_multicast_group = egress_vlan;
    control_vlan.broadcast_group = egress_vlan;
    bcm_vlan_control_vlan_set(unit, egress_vlan, control_vlan);

    rv = bcm_vlan_gport_add(unit, egress_vlan, egress_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_gport_add.\n");
        return rv;
    }
    vp2.criteria=BCM_VLAN_PORT_MATCH_NONE;
    vp2.port=egress_port;
    vp2.match_vlan=egress_vlan;
    vp2.egress_vlan=egress_vlan;
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp2.flags = BCM_VLAN_PORT_WITH_ID;
    BCM_GPORT_VLAN_PORT_ID_SET(vp2.vlan_port_id, 4110);
    vp2.failover_id = fec_failover_id;
    vp2.failover_port_id = vp2.vlan_port_id + 1;
    vp2.inlif_counting_profile = 2;
    rv = bcm_vlan_port_create(0,&vp2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    rv = bcm_vswitch_port_add(unit, vpn, vp2.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    rv = bcm_multicast_vlan_encap_get (unit, mcast_group, egress_port, vp2.vlan_port_id, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_vlan_encap_get\n");
        return rv;
    }
    rv = bcm_multicast_ingress_add (unit, mcast_group, egress_port, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }
    return BCM_E_NONE;
}

