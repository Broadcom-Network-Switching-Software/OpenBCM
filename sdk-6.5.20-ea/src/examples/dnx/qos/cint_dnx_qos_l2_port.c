/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~l2 port qos~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 
 * File: cint_dnx_qos_l2_port.c
 * Purpose: An example of the L2 packet use port PHB/QOS

 * Explanation:
   1. Assign packets to a Traffic class and Drop precedence according to incoming packet header and incoming Port
   2. Most configuration is done in Port and VLAN modules 
   
 * QoS Map Setting:
   dnx_qos_l2_dscp_map: Map both TC and DP from the packet TOS
                        including two case: packet is tagged, packet is untagged
   dnx_qos_l2_untag_map:   Map TC and DP with the default value by the parameter input for untagged
                        bind profile to In-AC
   dnx_qos_l2_port_dft_map. Map TC and DP with the default value by the parameter input
                        1. set Default TC and Default DP
                        2. set profile model of port so that port qos is most prior 
  
*  Map ingress and egress as follows 
*  Map TOS                                                    
*  Ingress TOS/TC           INTERNAL           Egress    
    *     IPV4/6            TC/DP              PKT_PRI/CFI
    *     0-31              0/0(1)             0/0(1)
    *     32-63             1/0(1)             1/0(1)
    *     64-95             2/0(1)             2/0(1)
    *     96-127            3/0(1)             3/0(1)
    *     128-159           4/0(1)             4/0(1)
    *     160-191           5/0(1)             5/0(1)
    *     192-223           6/0(1)             6/0(1)
    *     224-255           7/0(1)             7/0(1)

    
*  Map dft tc and dp for the untagged packet: tc and dp can be set as the inputted parameter
*  Ingress                  INTERNAL                Egress 
*  PKT_PRI/CFI               TC/DP               PKT_PRI/CFI
*    N/A                       0/0                    0/0
*    N/A                       0/1                    0/1
*    N/A                       1/0                    1/0
*    N/A                       1/1                    1/1
*    N/A                       2/0                    2/0
*    N/A                       2/1                    2/1
*    N/A                       3/0                    3/0
*    N/A                       3/1                    3/1
*    N/A                       4/0                    4/0
*    N/A                       4/1                    4/1
*    N/A                       5/0                    5/0
*    N/A                       5/1                    5/1
*    N/A                       6/0                    6/0
*    N/A                       6/1                    6/1
*    N/A                       7/0                    7/0
*    N/A                       7/1                    7/1

*  Map dft tc and dp for all L2 packet through port: tc and dp can be set as the inputted parameter
*  Ingress                  INTERNAL                Egress 
*  PKT_PRI/CFI               TC/DP               PKT_PRI/CFI
*    N/A                       0/0                    0/0
*    N/A                       0/1                    0/1
*    N/A                       1/0                    1/0
*    N/A                       1/1                    1/1
*    N/A                       2/0                    2/0
*    N/A                       2/1                    2/1
*    N/A                       3/0                    3/0
*    N/A                       3/1                    3/1
*    N/A                       4/0                    4/0
*    N/A                       4/1                    4/1
*    N/A                       5/0                    5/0
*    N/A                       5/1                    5/1
*    N/A                       6/0                    6/0
*    N/A                       6/1                    6/1
*    N/A                       7/0                    7/0
*    N/A                       7/1                    7/1


*  To Activate Above Settings Run:
   BCM> cd ../../../src/examples/dnx
   BCM> cint cint_dnx_qos_l2_port.c
   BCM> cint
   cint> dnx_qos_l2_service_init(unit,port_in,port_out);
   cint> dnx_qos_l2_bridge_service(unit);
   cint> dnx_qos_l2_dscp_map(unit, color, is_tagged);
         *  color_idx can be 0 or 1. 0 stands for green pakcet, 1 stands for yellow packet
         *  Send Traffic from port 15 and receive it on port 16:
         *  1. In port 15 tos 0   <-----CrossConnect----->  Out port 16 vid 200 prio 0 cfi $color tos 0
         *  2. In port 15 tos 64  <-----CrossConnect----->  Out port 16 vid 200 prio 2 cfi $color tos 64
         *  3. In port 15 tos 128 <-----CrossConnect----->  Out port 16 vid 200 prio 4 cfi $color tos 128
         *  4. In port 15 tos 192 <-----CrossConnect----->  Out port 16 vid 200 prio 6 cfi $color tos 192
         *  5. In port 15 vid 100 tos 0   <-CrossConnect->  Out port 16 vid 300 prio 0 tos 0
         *  6. In port 15 vid 100 tos 64  <-CrossConnect->  Out port 16 vid 300 prio 2 tos 64
         *  7. In port 15 vid 100 tos 128 <-CrossConnect->  Out port 16 vid 300 prio 4 tos 128
         *  8. In port 15 vid 100 tos 192 <-CrossConnect->  Out port 16 vid 300 prio 6 tos 192      
   cint> dnx_qos_l2_untag_map(unit, default_color, defaut_tc); 
         *  color_idx can be 0 or 1. 0 stands for green pakcet, 1 stands for yellow packet
         *  Send Traffic from port 15 and receive it on port 16:
         *  1. In port 15 tos 0   <-----CrossConnect----->  Out port 16 vid 200 prio 0 cfi $color tos 0
         *  2. In port 15 tos 64  <-----CrossConnect----->  Out port 16 vid 200 prio 2 cfi $color tos 64
         *  3. In port 15 tos 128 <-----CrossConnect----->  Out port 16 vid 200 prio 4 cfi $color tos 128
         *  4. In port 15 tos 192 <-----CrossConnect----->  Out port 16 vid 200 prio 6 cfi $color tos 192
   cint> 
   cint> dnx_qos_l2_port_dft_map(unit,port,dft_color,dft_tc);
         *  dft_color can be from 0 to 1 and dft_tc can be from 0 to 7.
         *  Send Traffic from port 15 and receive it on port 16:
         *  1. In port 15  <-----CrossConnect----->  Out port 16 vid 200 prio $dft_tc cfi $dft_color
 */
int qos_l3_ingress_id = -1;
int qos_l2_ingress_id = -1;
int qos_l2_ingress_untag_id = -1;
int qos_ingress_default_id = -1;
int qos_l2_map_phb_code = -1;
int qos_l2_map_remark_code = -1;
int qos_l3_map_phb_code = -1;
int qos_l3_map_remark_code = -1;
bcm_vlan_t up_ovlan = 100;
bcm_vlan_t down_ovlan1 = 200;
bcm_vlan_t down_ovlan2 = 300;
bcm_vlan_port_t in_vlan_port1, in_vlan_port2, out_vlan_port1, out_vlan_port2;
bcm_vswitch_cross_connect_t gports_untag, gports_tag;


int prio[2][8] = {{0,1,2,3,4,5,6,7},
                    {1,3,5,7,2,4,6,0}};
 
/* Map TC and DP from the packet TOS for bridge*/
int dnx_qos_l2_dscp_map(int unit, int color, int is_tagged) { 
    int rv = BCM_E_NONE;
    int map_opcode;
    uint32 flags;
    int tos;
    bcm_qos_map_t ingress_phb_map;
    bcm_qos_map_t ingress_rm_map;

    /**  allocate profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_create(unit, flags, &qos_l3_ingress_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_create() $rv\n");
        return rv;
    }

    /*set qos profile phb to be used for L2 bridge use L3 mapping*/
    rv = bcm_qos_map_control_set(unit, qos_l3_ingress_id, BCM_QOS_MAP_PHB, bcmQosMapControlL3L2, 1);
	if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    }

    /** allocate opcode for phb*/
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_create(unit, flags, &map_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_create() $rv\n");
        return rv;
    }
    qos_l3_map_phb_code = map_opcode;
    /** opcode map */
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &ingress_phb_map, qos_l3_ingress_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    } 

    /** phb mapping */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB;
    for (tos = 0; tos < 256; tos++)
    {
        bcm_qos_map_t_init(&ingress_phb_map);
        ingress_phb_map.dscp = tos;
        ingress_phb_map.int_pri = prio[color][tos/32];
        ingress_phb_map.color = color;
        rv = bcm_qos_map_add(unit, flags, &ingress_phb_map, map_opcode);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_qos_map_add() $rv\n");
            return rv;
        } 
    }

    /*set qos profile qos to be used for L2 bridge use L3 mapping*/
    rv = bcm_qos_map_control_set(unit, qos_l3_ingress_id, BCM_QOS_MAP_REMARK, bcmQosMapControlL3L2, 1);
	if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    }
    /** allocate opcode for qos*/
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_create(unit, flags, &map_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_create() $rv\n");
        return rv;
    }
    qos_l3_map_remark_code = map_opcode;
    /** opcode map */
    bcm_qos_map_t_init(&ingress_rm_map);
    ingress_rm_map.opcode = map_opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &ingress_rm_map, qos_l3_ingress_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    } 
    /** remark mapping */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
    for (tos = 0; tos < 256; tos++)
    {
        bcm_qos_map_t_init(&ingress_rm_map);
        ingress_rm_map.dscp = tos;
        ingress_rm_map.remark_int_pri = prio[color][tos/32];
        rv = bcm_qos_map_add(unit, flags, &ingress_rm_map, map_opcode);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_qos_map_add() $rv\n");
            return rv;
        } 
    }

    /*in_vlan_port1 is for untagged packet, in_vlan_port2 is for tagged */
    if (!is_tagged)
    {
        rv = bcm_qos_port_map_set(unit, in_vlan_port1.vlan_port_id, qos_l3_ingress_id, -1);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_qos_port_map_set() $rv\n");
            return rv;
        }
    }
    else
    {
        rv = bcm_qos_port_map_set(unit, in_vlan_port2.vlan_port_id, qos_l3_ingress_id, -1);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_qos_port_map_set() $rv\n");
            return rv;
        }
    }
    return rv;
}

/* Map TC and DP with the default value from the inputted parameter */
int dnx_qos_l2_untag_map(int unit, int dft_color, int dft_tc) {
    int rv = BCM_E_NONE;
    uint32 flags;
    int qos_var, dp;
    bcm_qos_map_t ingress_phb_map;
    bcm_qos_map_t ingress_rm_map;

    /**  allocate profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_create(unit, flags,&qos_l2_ingress_untag_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_create() $rv\n");
        return rv;
    }        

    /**  phb mapping */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_PHB;
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.int_pri = dft_tc;
    ingress_phb_map.color = dft_color;
    rv = bcm_qos_map_add(unit, flags, &ingress_phb_map, qos_l2_ingress_untag_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    }

    /**  qos remark mapping */
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_REMARK;
    bcm_qos_map_t_init(&ingress_rm_map);
    ingress_rm_map.remark_int_pri = dft_tc;
    rv = bcm_qos_map_add(unit, flags, &ingress_rm_map, qos_l2_ingress_untag_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    }

    /*set profile to port*/
    rv = bcm_qos_port_map_set(unit, in_vlan_port1.vlan_port_id, qos_l2_ingress_untag_id, -1);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_port_map_set() $rv\n");
        return rv;
    }

    return rv;
}

/* Map TC and DP with the default value from the inputted parameter */
int dnx_qos_l2_port_dft_map(int unit, int port, int dft_color, int dft_tc) {
    int rv = BCM_E_NONE;
    uint32 flags;
    int qos_var, dp;
    bcm_qos_map_t ingress_phb_map;
    bcm_qos_map_t ingress_rm_map;

    /**  allocate profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_create(unit, flags,&qos_ingress_default_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_create() $rv\n");
        return rv;
    }        

    /**  phb mapping */
    flags = BCM_QOS_MAP_PORT | BCM_QOS_MAP_PHB;
    bcm_qos_map_t_init(&ingress_phb_map);
    ingress_phb_map.int_pri = dft_tc;
    ingress_phb_map.color = dft_color;
    rv = bcm_qos_map_add(unit, flags, &ingress_phb_map, qos_ingress_default_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    }

    /**  remark mapping */
    flags = BCM_QOS_MAP_PORT | BCM_QOS_MAP_REMARK;
    bcm_qos_map_t_init(&ingress_rm_map);
    ingress_rm_map.remark_int_pri = dft_tc;
    rv = bcm_qos_map_add(unit, flags, &ingress_rm_map, qos_ingress_default_id);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_map_add() $rv\n");
        return rv;
    }

    /*set profile to port*/
    rv = bcm_qos_port_map_set(unit, port, qos_ingress_default_id, -1);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_port_map_set() $rv\n");
        return rv;
    }

    /*set port qos model to select port phb*/
    rv = bcm_port_control_set(unit, port, bcmPortControlIngressQosModelPhb, bcmQosIngressModelStuck);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_port_map_set() $rv\n");
        return rv;
    }

    /*set port qos model to select port qos*/
    rv = bcm_port_control_set(unit, port, bcmPortControlIngressQosModelRemark, bcmQosIngressModelStuck);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_port_map_set() $rv\n");
        return rv;
    }

    return rv;
}

int dnx_qos_l2_service_init(int unit, bcm_port_t port_in, bcm_port_t port_out) 
{
    /* initialize the vlan ports */
    int rv = BCM_E_NONE;
    bcm_vlan_port_t_init(&in_vlan_port1);
    in_vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT;
    in_vlan_port1.port = port_in;
    in_vlan_port1.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    
    bcm_vlan_port_t_init(&out_vlan_port1);
    out_vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port1.match_vlan = down_ovlan1;
    out_vlan_port1.port = port_out;
    out_vlan_port1.flags = 0;
    
    bcm_vlan_port_t_init(&in_vlan_port2);
    in_vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_vlan_port2.match_vlan = up_ovlan;
    in_vlan_port2.port = port_in;
    in_vlan_port2.flags = 0;
    
    bcm_vlan_port_t_init(&out_vlan_port2);
    out_vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port2.match_vlan = down_ovlan2;
    out_vlan_port2.port = port_out;
    out_vlan_port2.flags = 0;

    return rv;
}


int dnx_qos_l2_bridge_service(int unit)
{
    int rv;
    int s_tpid = 0x9100;
    int c_tpid = 0x8100;
    bcm_gport_t in_gport1, in_gport2, out_gport1, out_gport2;

    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all\n");
        return rv;
    }
    rv = tpid__tpid_add(unit, c_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }
    rv = tpid__tpid_add(unit, s_tpid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpid_add\n");
        return rv;
    }

    rv = port_tpid_class_add(unit, in_vlan_port2.port, s_tpid, BCM_PORT_TPID_CLASS_TPID_INVALID, 4);
    if (rv != BCM_E_NONE)
    {
        printf("Error, port_tpid_class_add\n");
        return rv;
    }

    /* create inLIF for untagged service */
    rv = bcm_vlan_port_create(unit, &in_vlan_port1);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, up_ovlan, in_vlan_port1.port, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_gport_add failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    in_gport1 = in_vlan_port1.vlan_port_id;
    printf("in_gport1 = 0x%08X %d\n", in_gport1, in_gport1);
    
    /* create inLIF for tagged service */
    rv = bcm_vlan_port_create(unit, &in_vlan_port2);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, up_ovlan, in_vlan_port2.port, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_gport_add failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    in_gport2 = in_vlan_port2.vlan_port_id;
    printf("in_gport2 = 0x%08X %d\n", in_gport2, in_gport2);
    
    /* create outLIF for untagged service */
    rv = bcm_vlan_port_create(unit, &out_vlan_port1);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    out_gport1 = out_vlan_port1.vlan_port_id;
    printf("out_gport1 = 0x%08X %d\n", out_gport1, out_gport1);

    /*set eve default*/
    rv = vlan_translate_ive_eve_translation_set_with_pri_action(
                                unit,
                                out_gport1,   /* lif */
                                c_tpid,       /* outer_tpid */
                                0,            /* inner_tpid */
                                bcmVlanTpidActionModify, /*outer tpid action*/
                                bcmVlanTpidActionNone, /*inner tpid action*/
                                bcmVlanActionAdd,              /* outer_action */
                                bcmVlanActionNone,             /* inner_action */
                                bcmVlanActionAdd,              /* outer pri action*/
                                bcmVlanActionNone,             /* inner pri action*/
                                down_ovlan1,                   /* new_outer_vid*/
                                0,                   /* new_inner_vid*/
                                1,                   /* vlan_edit_profile */
                                0,                   /* tag_format untag*/
                                FALSE                                /* is_ive */
                                );

    if (rv != BCM_E_NONE) {
        printf("vlan_translate_ive_eve_translation_set failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    /* create outLIF for tagged service */
    rv = bcm_vlan_port_create(unit, &out_vlan_port2);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    out_gport2 = out_vlan_port2.vlan_port_id;
    printf("out_gport2 = 0x%08x %d\n", out_gport2, out_gport2);

    /*set eve default*/
    rv = vlan_translate_ive_eve_translation_set_with_pri_action(
                                unit,
                                out_gport2,   /* lif */
                                c_tpid,       /* outer_tpid */
                                0,            /* inner_tpid */
                                bcmVlanTpidActionModify, /*outer tpid action*/
                                bcmVlanTpidActionNone, /*inner tpid action*/
                                bcmVlanActionReplace,              /* outer_action */
                                bcmVlanActionNone,             /* inner_action */
                                bcmVlanActionReplace,              /* outer pri action*/
                                bcmVlanActionNone,             /* inner pri action*/
                                down_ovlan2,                   /* new_outer_vid*/
                                0,                   /* new_inner_vid*/
                                2,                   /* vlan_edit_profile */
                                4,                   /* tag_format untag*/
                                FALSE                                /* is_ive */
                                );

    bcm_vswitch_cross_connect_t_init(&gports_untag);
    gports_untag.port1 = in_gport1;
    gports_untag.port2 = out_gport1;
    gports_untag.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    if (verbose >= 2) {
        printf("Cross connecting the ports: port1 = 0x%08X port2 = 0x%08X \n", gports_untag.port1, gports_untag.port2);
    }
    /* cross connect two lifs for tagged service */
    rv = bcm_vswitch_cross_connect_add(unit, &gports_untag);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_vswitch_cross_connect_add() $rv\n");
        return rv;
    }

    bcm_vswitch_cross_connect_t_init(&gports_tag);
    gports_tag.port1 = in_gport2;
    gports_tag.port2 = out_gport2;
    if (verbose >= 2) {
        printf("Cross connecting the ports: port1 = 0x%08X port2 = 0x%08X \n", gports_tag.port1, gports_tag.port2);
    }
    /* cross connect two lifs for tagged service */
    rv = bcm_vswitch_cross_connect_add(unit, &gports_tag);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_vswitch_cross_connect_add() $rv\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
* clean l2 service 
*/
int dnx_qos_l2_service_cleanup(int unit)
{
    int rv = BCM_E_NONE;

    gports_untag.port1 = in_vlan_port1.vlan_port_id;
    gports_untag.port2 = out_vlan_port1.vlan_port_id;
    gports_tag.port1 = in_vlan_port2.vlan_port_id;
    gports_tag.port2 = out_vlan_port2.vlan_port_id;
     
    /* Delete the cross connected LIFs for untagged service */
    rv = bcm_vswitch_cross_connect_delete(unit, &gports_untag);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete() $rv\n");
        return rv;
    }
    
    /* Delete the cross connected LIFs for tagged service */
    rv = bcm_vswitch_cross_connect_delete(unit, &gports_tag);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete() $rv\n");
        return rv;
    }

    /* Delete inLIF for untagged service */
    rv = bcm_vlan_port_destroy(unit, in_vlan_port1.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
    
    /* Delete outLIF for untagged service */
    rv = bcm_vlan_port_destroy(unit, out_vlan_port1.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
    
    /* Delete inLIF for tagged service */
    rv = bcm_vlan_port_destroy(unit, in_vlan_port2.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
    
    /* Delete outLIF for tagged service*/
    rv = bcm_vlan_port_destroy(unit, out_vlan_port2.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
  
    return rv;        
}

/*
* clean qos map 
*/
int dnx_qos_map_cleanup(int unit)
{
    int rv = BCM_E_NONE;

    /* destroy the qos profile  and opcode*/
    if (qos_l2_map_phb_code != -1)
    {
        bcm_qos_map_destroy(unit, qos_l2_map_phb_code);
        qos_l2_map_phb_code = -1;
    }
    if (qos_l2_map_remark_code != -1)
    {
        bcm_qos_map_destroy(unit, qos_l2_map_remark_code);
        qos_l2_map_remark_code = -1;
    }
    if (qos_l3_map_phb_code != -1)
    {
        bcm_qos_map_destroy(unit, qos_l3_map_phb_code);
        qos_l3_map_phb_code = -1;
    }
    if (qos_l3_map_remark_code != -1)
    {
        bcm_qos_map_destroy(unit, qos_l3_map_remark_code);
        qos_l3_map_remark_code = -1;
    }
    if(qos_l2_ingress_id != -1)
    {
        bcm_qos_map_destroy(unit, qos_l2_ingress_id);
        qos_l2_ingress_id = -1;
    }
    if(qos_l3_ingress_id != -1)
    {
        bcm_qos_map_destroy(unit, qos_l3_ingress_id);
        qos_l3_ingress_id = -1;
    }
    if (qos_ingress_default_id != -1)
    {
        bcm_qos_map_destroy(unit, qos_ingress_default_id);
        qos_ingress_default_id = -1;
    }

    /*set port qos model to disable port phb*/
    rv = bcm_port_control_set(unit, in_vlan_port1.port, bcmPortControlIngressQosModelPhb, bcmQosIngressModelShortpipe);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_port_map_set() $rv\n");
        return rv;
    }

    /*set port qos model to disable port qos*/
    rv = bcm_port_control_set(unit, in_vlan_port1.port, bcmPortControlIngressQosModelRemark, bcmQosIngressModelShortpipe);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_qos_port_map_set() $rv\n");
        return rv;
    }
    
    return rv;        
}

/**
 * \brief - Configure editing of PCP, DEI of the Ethernet VLAN header
 *
 * \param [in] unit - Relevant unit
 * \param [in] out_port - Out port
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
dnx_qos_l2_vlan_pcp_dei_update(int unit, bcm_port_t out_port)
{
    int rv = 0;
    int flags;
    int qos_egress;
    int opcode_egress;
    bcm_qos_map_t l2_eg_map;
    int int_pri =0;

    /** Create Egress Remark profile which is used by VLAN-Port (or ARP VLAN translation) */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_create(unit, flags, &qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in L3 egress bcm_qos_map_create\n");
    }

    /** Create Egress opcode which is later used to set the required pcp mapping */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_create(unit, flags, &opcode_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in egress l3 opcode bcm_qos_map_create\n");
        return rv;
    }

    /** Clear structure */
    bcm_qos_map_t_init(&l2_eg_map);

    /** Map Egress Remark profile to L2 Opcode */
    l2_eg_map.opcode = opcode_egress;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in egress l2 opcode bcm_qos_map_add\n");
        return rv;
    }
    /** Set L2 opcode mappings */
    int idx;
    for (idx = 0; idx < 256; idx++)
    {
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_REMARK;
        bcm_qos_map_t_init(&l2_eg_map);
        /** Input internal priority NWK-QOS */
        l2_eg_map.int_pri = idx;
        /** Input color */
        l2_eg_map.color = 0;
        if (idx == int_pri)
        {
            /** Set egress PCP and DEI */
            l2_eg_map.pkt_pri = 5;
            l2_eg_map.pkt_cfi = 1;
        }
        else
        {
            /** Set egress PCP and DEI */
            l2_eg_map.pkt_pri = 0;
            l2_eg_map.pkt_cfi = 0;
        }

        rv = bcm_qos_map_add(unit, flags, &l2_eg_map, opcode_egress);
        if (rv != BCM_E_NONE)
        {
            printf("Error in L2 egress bcm_qos_map_add profile(0x%08x)\n", opcode_egress);
            return rv;
        }
    }

    /** Create VLAN-Port */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = out_port;
    vlan_port.match_vlan = 5;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vlan_port_create()");
        return rv;
    }

    /** Connect it to VPN 1 */
    rv = bcm_vswitch_port_add(unit, 1, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vswitch_port_add");
        return rv;
    }

    /** Connect VLAN Port AC (or later ARP VLAN translation) to VLAN-Translation Command 0 which always add VLAN */
    int vlan_edit_profile =3;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = vlan_edit_profile;
    port_trans.new_outer_vlan = 30;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vlan_port_translation_set");
        return rv;
    }

    /** Create action ID */
    int action_id_1;
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /** Set translation action */
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x9100;
    action.dt_outer = bcmVlanActionAdd;
    action.dt_outer_pkt_prio = bcmVlanActionAdd;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /** Set translation action class (map edit_profile & tag_format to action_id) */
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile;
    action_class.tag_format_class_id = 0; /* Untagged */
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /** Connect VLAN Port AC (or later ARP VLAN translation) to Remark profile which is used to decide the PCP value on VLAN tag */
    rv = bcm_qos_port_map_set(unit, vlan_port.vlan_port_id, -1, qos_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in L2 egress bcm_qos_port_map_set");
        return rv;
    }

    /** L2 addr example */
    bcm_mac_t mac = {0x00, 0x00, 0x00, 0x00, 0x12, 0x34};
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, 1);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = vlan_port.vlan_port_id;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return 0;
}



