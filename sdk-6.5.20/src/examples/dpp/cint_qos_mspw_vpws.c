/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MS PWE NODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 
 * File: cint_qos_mspw_vpws.c
 * Purpose: An example of cos mapping for MS PWE Transit Node

 * To Activate Above Settings Run:
   BCM> cd ../../../../src/examples/dpp
   BCM> cint ../sand/utility/cint_sand_utils_global.c
   BCM> cint ../sand/utility/cint_sand_utils_mpls.c
   BCM> cint utility/cint_utils_l3.c
   BCM> cint cint_ip_route.c
   BCM> cint cint_qos_mspw_vswitch.c
   BCM> cint
   cint> mpls_pipe_mode_exp_set=0(1);
   cint> qos_map_vpws(unit);
   cint> vswitch_vpws_run(unit,15,16,mpls_pipe_mode_exp_set);
  */
 

static int vpws_eg_map_id; /* QOS remark profile for l2 tunnel exp*/


/* Map Egress Remark-Profile to do TC/DP->EXP */
int qos_map_vpws_egress_profile(int unit)
{
    bcm_qos_map_t vpws_eg_map;
    int flags = 0;
    int int_pri, dp;
    int rv = BCM_E_NONE;

    /* Clear structure */
    bcm_qos_map_t_init(&vpws_eg_map);

    flags = BCM_QOS_MAP_EGRESS;
    if(vpws_eg_map_id != -1){
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, flags, &vpws_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in VPLS egress bcm_qos_map_create() $rv \n");
        return rv;
    }

    for (int_pri=0; int_pri<8; int_pri++) {
        for (dp =0; dp < 4; dp ++) {
            flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_ENCAP;
            vpws_eg_map.remark_int_pri = int_pri; /* Out-DSCP-EXP equals TC Input */
            vpws_eg_map.color = dp;
            vpws_eg_map.exp = 3; /* EXP Output */

            rv = bcm_qos_map_add(unit, flags, &vpws_eg_map, vpws_eg_map_id);
            if (rv != BCM_E_NONE) {
               printf("error in VPLS egress bcm_qos_map_add() $rv \n");
               return rv;
            }
        }
    }
    return rv;
}

int qos_map_vpws (int unit, int map_id)
{
    int rv = BCM_E_NONE;

    vpws_eg_map_id = map_id;
    rv = qos_map_vpws_egress_profile(unit);
    if (rv) {
        printf("error setting up egress qos profile $rv\n");
        return rv;
    }
    printf("vpls_eg_map_id is %d\n", vpws_eg_map_id);
    return rv;
}

int vswitch_vpws_run (int unit, int in_port, int  out_port, int in_vlan, int out_vlan)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port, mpls_group;
    int tunnel_id;
    uint8 my_mac[6] = {0x00, 0x00, 0x00, 0x00,0x00, 0x11};
    uint8 nh_mac[6] = {0x00, 0x00, 0x00, 0x00,0x00, 0x22};
    int ll_encap;
    int pw_encap_index = 0x8410, pw_fec_index = 0x3010, encode_pwe_encap_id;
    bcm_vlan_port_t vp;
    bcm_pbmp_t pbmp, upbmp;
    int label_count;
    int cross_port[2];
    bcm_vswitch_cross_connect_t gports;


    rv = bcm_switch_control_set(0, bcmSwitchMplsPipeTunnelLabelExpSet,0);
    if (rv) {
        printf("error setting up egress qos profile $rv\n");
        return rv;
    }

    encode_pwe_encap_id = pw_encap_index | (2 <<29);
    /*l2 egress encap*/
    bcm_l2_egress_t l2_egress_overlay;
    bcm_l2_egress_t_init(&l2_egress_overlay);
    l2_egress_overlay.dest_mac   = nh_mac;
    l2_egress_overlay.src_mac    = my_mac;
    l2_egress_overlay.dest_mac[5] = l2_egress_overlay.dest_mac[5]+1;
    l2_egress_overlay.ethertype  = 0x8847;
    l2_egress_overlay.outer_tpid = 0x8100;
    l2_egress_overlay.outer_vlan = out_vlan;
    rv = bcm_l2_egress_create(unit, &l2_egress_overlay);
    ll_encap = l2_egress_overlay.encap_id;
    printf("bcm_l2_egress_create: ll_encap 0x%x\n", ll_encap);

    /*create egress mpls port*/
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = BCM_MPLS_PORT_NETWORK | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.port = BCM_GPORT_INVALID;
    mpls_port.egress_label.label = 1000;
    mpls_port.encap_id = pw_encap_index;
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id,  mpls_port.encap_id);
    if (soc_property_get(unit, "split_horizon_forwarding_groups_mode", 1)) {
        mpls_port.network_group_id = 2;
    }
    mpls_port.egress_label.qos_map_id = vpws_eg_map_id; 
    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY;
    mpls_port.egress_tunnel_if = ll_encap;
    rv = bcm_mpls_port_add(unit, 0, &mpls_port);
    if (rv) {
        printf("error bcm_mpls_port_add $rv\n");
        return rv;
    }
    printf("mpls_port: egress_tunnel_if=0x%x\n\tencap_id=0x%x\n\tmpls_port_id=0x%x\n",
                mpls_port.egress_tunnel_if, mpls_port.encap_id, mpls_port.mpls_port_id);

    /*forward group*/

    bcm_mpls_port_t_init(&mpls_group);
    mpls_group.flags = BCM_MPLS_PORT_NETWORK | BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_group.flags |= BCM_MPLS_PORT_FORWARD_GROUP;
    mpls_group.port = out_port;
    mpls_group.network_group_id = 2;
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_group.mpls_port_id, pw_fec_index);
    mpls_group.encap_id = pw_encap_index;
    BCM_ENCAP_REMOTE_SET(mpls_group.encap_id);
    mpls_group.egress_tunnel_if = encode_pwe_encap_id;
    mpls_group.egress_label.label = BCM_MPLS_LABEL_INVALID;
    rv = bcm_mpls_port_add(unit, 0, &mpls_group);
    if (rv) {
        printf("error bcm_mpls_port_add $rv\n");
        return rv;
    }

    cross_port[0] = mpls_group.mpls_port_id;
    pw_fec_index  = (BCM_GPORT_MPLS_PORT_ID_GET(mpls_group.mpls_port_id)) & 0x3FFFFF;
    printf("fec:0x%x\n",pw_fec_index);
    printf("mpls_group: egress_tunnel_if=0x%x\n\tencap_id=0x%x\n\tmpls_port_id=0x%x\n\t*pw_fec_index=0x%x\n",
               mpls_group.egress_tunnel_if, mpls_group.encap_id, mpls_group.mpls_port_id, pw_fec_index);

    /*create tunnel*/
    bcm_mpls_egress_label_t label_array[2];

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);
    rv = bcm_mpls_tunnel_initiator_get(unit, encode_pwe_encap_id, 1, label_array, &label_count);
    if ((rv == BCM_E_NONE) || (rv== BCM_E_EXISTS)) {
        printf("bcm_mpls_tunnel_initiator_get\n");
    }

    label_array[0].qos_map_id = vpws_eg_map_id;
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;

    label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE|BCM_MPLS_EGRESS_LABEL_TTL_SET;
    label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    label_array[1].tunnel_id = encode_pwe_encap_id;
    label_array[1].l3_intf_id = 0;
    label_array[1].label = 2000;
    label_array[1].ttl=40;
    label_array[1].exp=2;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if ((rv == BCM_E_NONE) || (rv== BCM_E_EXISTS)) {
        printf(" bcm_mpls_tunnel_initiator_create\n\t");
        printf("Labels: %d, %d, returned Tunnel id: 0x%x\n",
            label_array[0].label, label_array[1].label, label_array[0].tunnel_id);
    } else {
        printf("error bcm_mpls_tunnel_initiator_create rv=%d\n", rv);
        return rv;
    }

    bcm_l2_egress_t_init(&l2_egress_overlay);
    l2_egress_overlay.flags = BCM_L2_EGRESS_REPLACE | BCM_L2_EGRESS_WITH_ID;
    l2_egress_overlay.dest_mac   = nh_mac;
    l2_egress_overlay.src_mac    = my_mac;
    l2_egress_overlay.dest_mac[5] = l2_egress_overlay.dest_mac[5]+1;
    l2_egress_overlay.ethertype  = 0x8847;
    l2_egress_overlay.outer_tpid = 0x8100;
    l2_egress_overlay.outer_vlan = out_vlan;
    l2_egress_overlay.l3_intf = encode_pwe_encap_id;
    l2_egress_overlay.encap_id = ll_encap;
    rv = bcm_l2_egress_create(unit, &l2_egress_overlay);
    printf("bcm_l2_egress_create: ll_encap=0x%x, lable enc:0x%x, rv:%d\n",
              ll_encap,encode_pwe_encap_id,rv);

      /*Set the TC to EXP Mapping*/
    bcm_gport_t overlay_port;
    BCM_GPORT_TUNNEL_ID_SET(overlay_port,BCM_L3_ITF_VAL_GET(ll_encap));
    rv = bcm_qos_port_map_set(0,overlay_port,-1,vpws_eg_map_id);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_qos_port_map_set qos map failed %d\n",rv);
       return rv;
    }
    print overlay_port;

    rv = bcm_vlan_create(unit, in_vlan);
    if ((rv != BCM_E_NONE)  && (rv!= BCM_E_EXISTS)){
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", in_vlan);
        return rv;
    }
    printf ("bcm_vlan_create:  created vlan %d  \n", in_vlan);

    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = in_port;
    vp.match_vlan = in_vlan;
    vp.egress_vlan = out_vlan;
    vp.vsi = 0;
    vp.flags = 0;
    rv = bcm_vlan_port_create(unit, &vp);
    if ((rv != BCM_E_NONE)  && (rv!= BCM_E_EXISTS)){
         printf("Error, bcm_vlan_port_create\n");
         return rv;
    }

    printf("bcm_vlan_port_create single tagged \n");

    cross_port[1] = vp.vlan_port_id;
    printf("   vlan-port:0x0%8x  \n", vp.vlan_port_id);
    printf("   (LIF Id) encap-id: 0x%8x=%d\n", vp.encap_id, vp.encap_id);
    printf("   in-port: %d\n", vp.port);
    printf("   in-vlan: %d\n", vp.match_vlan);
    printf("   egress-vlan: %d\n", vp.egress_vlan);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);
    rv = bcm_vlan_port_remove(unit, in_vlan, pbmp);
    rv = bcm_vlan_port_add(unit, in_vlan, pbmp, upbmp);
    if ((rv != BCM_E_NONE) &&  (rv!= BCM_E_EXISTS)) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, in_vlan, rv);
        return rv;
    }
    if (rv == BCM_E_EXISTS) {
        printf ("vlan %d aleady exist \n", in_vlan);
    } else {
        printf("success bcm_vlan_port_add in_vlan=%d port=%d\n", in_vlan, in_port);
    }

    rv = bcm_vlan_create(unit, out_vlan);
    if ((rv != BCM_E_NONE) &&  (rv!= BCM_E_EXISTS)) {
         printf("Failed bcm_vlan_create outvlan=%d\n", out_vlan);
         return rv;
    }
    rv = bcm_vlan_port_add(unit, out_vlan, pbmp, upbmp);
    if ((rv != BCM_E_NONE) &&  (rv!= BCM_E_EXISTS)) {
     printf("Failed bcm_vlan_port_add ac_port=%d outvlan=%d\n", in_port, out_vlan);
     return rv;
    }
    printf("egress port/vlan add: ac_port=%d outvlan=%d\n", in_port, out_vlan);

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = cross_port[1];
    gports.port2 = cross_port[0];
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(0, &gports);
    printf("rv=%d: cross connect port1:0x0%8x with port2:0x0%8x \n", rv,gports.port1, gports.port2);

    return rv;
}

int qos_vpws_clean(int unit)
{
    int rv = 0;
    int map_id;

    printf("qos_vpws_clean..\n");

    map_id = vpws_eg_map_id;
    rv =bcm_qos_map_destroy(unit, map_id);
    if (rv != BCM_E_NONE) {
        printf("Error doing bcm_qos_map_destroy %d.\n", map_id);
        return rv;
    }

    rv =bcm_mpls_port_delete_all(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error doing bcm_mpls_port_delete_all .\n");
        return rv;
    }
    return rv;
}

